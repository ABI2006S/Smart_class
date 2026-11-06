#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H
#include "../config/device_config.h"
#include "../core/systemstate.h"
#include <Arduino.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_netif_sta_list.h>
static Preferences s_prefs;
static bool s_wifiConnected = false;
static bool s_apMode = false;
static unsigned long s_lastReconnectAttemptMs = 0;
static unsigned long s_lastStaSyncMs = 0;
static String s_savedSsid = "";
static String s_savedPass = "";

static inline void syncSoftApStations() {
  if (!s_apMode) return;
  wifi_sta_list_t wifi_sta_list;
  memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));
  if (esp_wifi_ap_get_sta_list(&wifi_sta_list) == ESP_OK && wifi_sta_list.num > 0) {
    esp_netif_sta_list_t netif_sta_list;
    memset(&netif_sta_list, 0, sizeof(netif_sta_list));
    esp_netif_get_sta_list(&wifi_sta_list, &netif_sta_list);

    for (int i = 0; i < wifi_sta_list.num; i++) {
      char macStr[18];
      snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
               wifi_sta_list.sta[i].mac[0], wifi_sta_list.sta[i].mac[1],
               wifi_sta_list.sta[i].mac[2], wifi_sta_list.sta[i].mac[3],
               wifi_sta_list.sta[i].mac[4], wifi_sta_list.sta[i].mac[5]);

      IPAddress staIp(netif_sta_list.sta[i].ip.addr);
      String ipStr = staIp.toString();
      if (ipStr == "0.0.0.0") {
        ipStr = "";
      }
      registerOrUpdateClient(ipStr.c_str(), "", nullptr, macStr);
    }
  }
  updateConnectedClientsExpiry();
}
static inline void startAccessPoint() {
  WiFi.disconnect(true);
  delay(50);
  WiFi.mode(WIFI_AP);
  IPAddress apIP(192, 168, 4, 1);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  if (strlen(DEFAULT_AP_PASSWORD) >= 8) {
    WiFi.softAP(DEFAULT_AP_SSID, DEFAULT_AP_PASSWORD);
    Serial.printf("[WIFI] Password: %s\n", DEFAULT_AP_PASSWORD);
  } else {
    WiFi.softAP(DEFAULT_AP_SSID);  
    Serial.println("[WIFI] Security: OPEN (No password required)");
  }
  s_apMode = true;
  s_wifiConnected = false;
  systemState.wifiConnected = false;
  systemState.apModeActive = true;
  systemState.ipAddress = WiFi.softAPIP().toString();
  systemState.ssid = DEFAULT_AP_SSID;
  systemState.wifiRssi = 0;
  Serial.println("[WIFI] Running in SoftAP Mode");
  Serial.printf("[WIFI] SSID: %s\n", DEFAULT_AP_SSID);
  Serial.printf("[WIFI] SoftAP IP Address: %s\n",
                systemState.ipAddress.c_str());
  Serial.println(
      "[WIFI] Connect to access the S5CSE A Captive Portal and Dashboard");
}
static inline bool attemptStationConnection(const String &ssid,
                                            const String &password) {
  if (ssid.length() == 0) {
    return false;
  }
  Serial.printf("[WIFI] Attempting connection to SSID: \"%s\"...\n",
                ssid.c_str());
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  unsigned long startMs = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - startMs < 12000)) {
    delay(250);
    Serial.print(".");
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    s_wifiConnected = true;
    s_apMode = false;
    systemState.wifiConnected = true;
    systemState.apModeActive = false;
    systemState.ipAddress = WiFi.localIP().toString();
    systemState.ssid = ssid;
    systemState.wifiRssi = WiFi.RSSI();
    Serial.println("[WIFI] Successfully connected via DHCP!");
    Serial.printf("[WIFI] IP Address: %s\n", systemState.ipAddress.c_str());
    Serial.printf("[WIFI] Signal RSSI: %d dBm\n", systemState.wifiRssi);
    if (MDNS.begin(DEFAULT_MDNS_HOSTNAME)) {
      Serial.printf("[WIFI] mDNS responder active at http://%s.local\n",
                    DEFAULT_MDNS_HOSTNAME);
    }
    return true;
  }
  Serial.println("[WIFI] Failed to connect within timeout.");
  return false;
}
inline void wifiInit() {
  Serial.println("[WIFI] Initializing Wi-Fi Manager...");
  s_prefs.begin("smartclass", false);
  s_savedSsid = s_prefs.getString("ssid", "");
  s_savedPass = s_prefs.getString("pass", "");
  bool connected = false;
  if (s_savedSsid.length() > 0) {
    connected = attemptStationConnection(s_savedSsid, s_savedPass);
  } else {
    Serial.println("[WIFI] No saved Wi-Fi credentials in persistent storage.");
  }
  if (!connected) {
    startAccessPoint();
  }
}
inline void wifiUpdate() {
  unsigned long now = millis();
  if (s_wifiConnected) {
    if (WiFi.status() != WL_CONNECTED) {
      s_wifiConnected = false;
      systemState.wifiConnected = false;
      Serial.println("[WIFI] Connection lost! Starting SoftAP fallback...");
      startAccessPoint();
    } else {
      systemState.wifiRssi = WiFi.RSSI();
    }
  } else if (!s_wifiConnected && s_savedSsid.length() > 0) {
    if (now - s_lastReconnectAttemptMs >= 30000) {
      s_lastReconnectAttemptMs = now;
      Serial.println("[WIFI] Background retry: attempting reconnection to "
                     "saved network...");
      if (attemptStationConnection(s_savedSsid, s_savedPass)) {
        Serial.println("[WIFI] Reconnection successful!");
      } else {
        startAccessPoint();
      }
    }
  }
  if (s_apMode && (now - s_lastStaSyncMs >= 3000)) {
    s_lastStaSyncMs = now;
    syncSoftApStations();
  }
}
inline bool isWifiConnected() { return s_wifiConnected; }
inline bool isApMode() { return s_apMode; }
inline String getIpAddress() { return systemState.ipAddress; }
inline void saveWifiCredentials(const String &ssid, const String &password) {
  String cleanSsid = ssid;
  String cleanPassword = password;
  cleanSsid.trim();
  cleanPassword.trim();
  if (cleanSsid.length() == 0) {
    Serial.println("[WIFI] ERROR: SSID is empty.");
    return;
  }
  s_savedSsid = cleanSsid;
  s_savedPass = cleanPassword;
  s_prefs.putString("ssid", s_savedSsid);
  s_prefs.putString("pass", s_savedPass);
  Serial.printf(
      "[WIFI] Saved credentials for \"%s\" to non-volatile storage.\n",
      s_savedSsid.c_str());
  attemptStationConnection(s_savedSsid, s_savedPass);
  if (!s_wifiConnected) {
    startAccessPoint();
  }
}
#endif 