#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include <Arduino.h>
#include <WebServer.h>
#include "../config/device_config.h"
#include "../core/systemstate.h"
#include "../ldr_light/ldr_light.h"
#include "../dht_fan/dht_fan.h"
#include "../smoke/smoke.h"
#include "../window/window.h"
#include "../door/door.h"
#include "../oleddisplayiot/oleddisplayiot.h"
#include "../dashboard/dashboard.h"
#include "wifi_manager.h"
#include "captiveportal.h"
extern WebServer server;
#ifndef HTTPSERVER_DEFINED
#define HTTPSERVER_DEFINED
WebServer server(DEFAULT_HTTP_PORT);
#endif
static inline void handleCaptivePortalRedirect() {
    server.sendHeader("Location", String("http://") + server.client().localIP().toString() + "/", true);
    server.send(302, "text/plain", "Redirecting to S5CSE A Dashboard");
}
static inline void trackRequestingClient() {
    String clientIp = server.client().remoteIP().toString();
    String ua = server.header("User-Agent");
    registerOrUpdateClient(clientIp.c_str(), ua.c_str());
}
static inline void handleRoot() {
    trackRequestingClient();
    if (isCaptivePortalRequest(server.hostHeader())) {
        handleCaptivePortalRedirect();
        return;
    }
    server.send(200, "text/html", getDashboardHtml());
}
static inline void handlePortalPage() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "Redirecting to S5CSE A Dashboard");
}
static inline void handleApiStatus() {
    trackRequestingClient();
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", systemStateToJson());
}
static inline void handleApiEnvironment() {
    String json = "{";
    json += "\"temperature\":" + String(systemState.temperature, 1) + ",";
    json += "\"humidity\":" + String(systemState.humidity, 1) + ",";
    json += "\"lightIntensity\":" + String(systemState.lightIntensity) + ",";
    json += "\"smokeLevel\":" + String(systemState.smokeLevel) + ",";
    json += "\"smokeDetected\":" + String(systemState.smokeDetected ? "true" : "false");
    json += "}";
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", json);
}
static inline void handleApiLightGet() {
    String json = "{";
    json += "\"lightOn\":" + String(systemState.lightOn ? "true" : "false") + ",";
    json += "\"lightMode\":\"" + String(controlModeToString(systemState.lightMode)) + "\",";
    json += "\"lightIntensity\":" + String(systemState.lightIntensity) + ",";
    json += "\"ldrFault\":" + String(systemState.ldrFault ? "true" : "false");
    json += "}";
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", json);
}
static inline void handleApiLightPost() {
    if (server.hasArg("mode")) {
        ControlMode m = stringToControlMode(server.arg("mode"));
        setLightMode(m);
    }
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", "{\"status\":\"ok\",\"lightOn\":" + String(getLightState() ? "true" : "false") + ",\"mode\":\"" + String(controlModeToString(getLightMode())) + "\"}");
}
static inline void handleApiFanGet() {
    String json = "{";
    json += "\"fanOn\":" + String(systemState.fanOn ? "true" : "false") + ",";
    json += "\"fanMode\":\"" + String(controlModeToString(systemState.fanMode)) + "\",";
    json += "\"temperature\":" + String(systemState.temperature, 1) + ",";
    json += "\"humidity\":" + String(systemState.humidity, 1) + ",";
    json += "\"dhtFault\":" + String(systemState.dhtFault ? "true" : "false");
    json += "}";
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", json);
}
static inline void handleApiFanPost() {
    if (server.hasArg("mode")) {
        ControlMode m = stringToControlMode(server.arg("mode"));
        setFanMode(m);
    }
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", "{\"status\":\"ok\",\"fanOn\":" + String(getFanState() ? "true" : "false") + ",\"mode\":\"" + String(controlModeToString(getFanMode())) + "\"}");
}
static inline void handleApiWindowGet() {
    String json = "{";
    json += "\"windowOpen\":" + String(isWindowOpen() ? "true" : "false") + ",";
    json += "\"windowState\":\"" + getWindowState() + "\",";
    json += "\"windowMode\":\"" + String(controlModeToString(getWindowMode())) + "\",";
    json += "\"windowBusy\":" + String(isWindowBusy() ? "true" : "false");
    json += "}";
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", json);
}
static inline void handleApiWindowPost() {
    String action = server.arg("action");
    action.toLowerCase();
    if (action == "open") {
        setWindowManual(true);
    } else if (action == "close" || action == "closed") {
        setWindowManual(false);
    } else if (action == "auto") {
        setWindowAuto();
    }
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json",
        "{\"status\":\"ok\",\"windowOpen\":" + String(isWindowOpen() ? "true" : "false")
        + ",\"windowState\":\"" + getWindowState() + "\",\"windowMode\":\""
        + String(controlModeToString(getWindowMode())) + "\"}");
}
static inline void handleApiDoorGet() {
    String json = "{";
    json += "\"doorOpen\":" + String(systemState.doorOpen ? "true" : "false") + ",";
    json += "\"doorLocked\":" + String(systemState.doorLocked ? "true" : "false") + ",";
    json += "\"doorState\":\"" + String(doorStateToString(systemState.doorState)) + "\",";
    json += "\"lastRfidUid\":\"" + systemState.lastRfidUid + "\",";
    json += "\"lastRfidName\":\"" + systemState.lastRfidName + "\",";
    json += "\"lastRfidStatus\":\"" + systemState.lastRfidStatus + "\",";
    json += "\"lastRfidAuthorized\":" + String(systemState.lastRfidAuthorized ? "true" : "false") + ",";
    json += "\"rfidAvailable\":" + String(systemState.rfidAvailable ? "true" : "false");
    json += "}";
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", json);
}
static inline void handleApiDoorPost() {
    String action = server.arg("action");
    action.toLowerCase();
    if (action == "open" || action == "unlock") {
        openDoor();
    } else if (action == "lock" || action == "close") {
        closeDoor();
    }
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json",
        "{\"status\":\"ok\",\"doorOpen\":" + String(isDoorOpen() ? "true" : "false")
        + ",\"doorLocked\":" + String(isDoorLocked() ? "true" : "false")
        + ",\"doorState\":\"" + String(doorStateToString(systemState.doorState)) + "\"}");
}
static inline void handleApiSmokeGet() {
    String json = "{";
    json += "\"smokeLevel\":" + String(systemState.smokeLevel) + ",";
    json += "\"smokeDetected\":" + String(systemState.smokeDetected ? "true" : "false") + ",";
    json += "\"mq2Fault\":" + String(systemState.mq2Fault ? "true" : "false");
    json += "}";
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", json);
}
static inline void handleApiNoticeGet() {
    if (server.hasArg("notice")) {
        oledShowMessage(server.arg("notice"));
    } else if (server.hasArg("message")) {
        oledShowMessage(server.arg("message"));
    }
    String json = "{";
    json += "\"notice\":\"" + systemState.currentNotice + "\",";
    json += "\"oledAvailable\":" + String(systemState.oledAvailable ? "true" : "false");
    json += "}";
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", json);
}
static inline void handleApiNoticePost() {
    String msg = "";
    if (server.hasArg("notice")) {
        msg = server.arg("notice");
    } else if (server.hasArg("message")) {
        msg = server.arg("message");
    } else if (server.hasArg("msg")) {
        msg = server.arg("msg");
    } else if (server.hasArg("plain")) {
        String plain = server.arg("plain");
        plain.trim();
        if (plain.startsWith("{")) {
            int keyIdx = plain.indexOf("\"notice\"");
            if (keyIdx == -1) keyIdx = plain.indexOf("\"message\"");
            if (keyIdx != -1) {
                int colon = plain.indexOf(':', keyIdx);
                int q1 = plain.indexOf('"', colon);
                int q2 = plain.indexOf('"', q1 + 1);
                if (q1 != -1 && q2 != -1) {
                    msg = plain.substring(q1 + 1, q2);
                }
            }
        }
        if (msg.length() == 0 && !plain.startsWith("{")) {
            msg = plain;
        }
    }
    if (msg.length() > 0) {
        oledShowMessage(msg);
    }
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", "{\"status\":\"ok\",\"notice\":\"" + systemState.currentNotice + "\"}");
}
static inline void handleApiLoginPost() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", "{\"ok\":true}");
}
static inline void handleApiWifiPost() {
    String newSsid = server.arg("ssid");
    String newPass = server.arg("password");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", "{\"status\":\"saving\",\"message\":\"Credentials updated. Connecting...\"}");
    saveWifiCredentials(newSsid, newPass);
}
static inline void handleApiLogsGet() {
    String cat = server.hasArg("cat") ? server.arg("cat") : "";
    int limit = server.hasArg("limit") ? server.arg("limit").toInt() : 60;
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", systemLogsToJson(cat.c_str(), limit));
}
static inline void handleApiLogsClear() {
    clearSystemLogs();
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", "{\"status\":\"ok\",\"cleared\":true}");
}
static inline void handleApiDevicesGet() {
    trackRequestingClient();
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", systemStateToJson());
}
static inline void handleApiDeviceRegister() {
    String clientIp = server.client().remoteIP().toString();
    String ua = server.header("User-Agent");
    String devName = server.hasArg("name") ? server.arg("name") : "";
    registerOrUpdateClient(clientIp.c_str(), ua.c_str(), devName.length() > 0 ? devName.c_str() : nullptr);
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", "{\"status\":\"ok\",\"registered\":true}");
}
static inline void handleApiDeviceRename() {
    String targetIp = server.arg("ip");
    String newName = server.arg("name");
    bool ok = renameConnectedClient(targetIp.c_str(), newName.c_str());
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", ok ? "{\"status\":\"ok\"}" : "{\"status\":\"error\",\"message\":\"Device not found\"}");
}
static inline void handleNotFound() {
    if (isApMode() || isCaptivePortalRequest(server.hostHeader())) {
        handleCaptivePortalRedirect();
        return;
    }
    server.send(404, "text/plain", "S5CSE A Smart Classroom — 404 Not Found");
}
inline void httpServerInit() {
    const char* headerkeys[] = {"User-Agent"};
    server.collectHeaders(headerkeys, 1);
    server.on("/", HTTP_GET, handleRoot);
    server.on("/dashboard", HTTP_GET, handleRoot);
    server.on("/portal", HTTP_GET, handlePortalPage);
    server.on("/generate_204", HTTP_GET, handleCaptivePortalRedirect);
    server.on("/gen_204", HTTP_GET, handleCaptivePortalRedirect);
    server.on("/hotspot-detect.html", HTTP_GET, handleCaptivePortalRedirect);
    server.on("/ncsi.txt", HTTP_GET, handleCaptivePortalRedirect);
    server.on("/connecttest.txt", HTTP_GET, handleCaptivePortalRedirect);
    server.on("/status", HTTP_GET, handleApiStatus);
    server.on("/api/status", HTTP_GET, handleApiStatus);
    server.on("/api/devices", HTTP_GET, handleApiDevicesGet);
    server.on("/api/device/register", HTTP_POST, handleApiDeviceRegister);
    server.on("/api/device/rename", HTTP_POST, handleApiDeviceRename);
    server.on("/api/environment", HTTP_GET, handleApiEnvironment);
    server.on("/api/light", HTTP_GET, handleApiLightGet);
    server.on("/api/light", HTTP_POST, handleApiLightPost);
    server.on("/api/fan", HTTP_GET, handleApiFanGet);
    server.on("/api/fan", HTTP_POST, handleApiFanPost);
    server.on("/api/window", HTTP_GET, handleApiWindowGet);
    server.on("/api/window", HTTP_POST, handleApiWindowPost);
    server.on("/api/door", HTTP_GET, handleApiDoorGet);
    server.on("/api/door", HTTP_POST, handleApiDoorPost);
    server.on("/api/smoke", HTTP_GET, handleApiSmokeGet);
    server.on("/api/notice", HTTP_GET, handleApiNoticeGet);
    server.on("/api/notice", HTTP_POST, handleApiNoticePost);
    server.on("/api/message", HTTP_GET, handleApiNoticeGet);
    server.on("/api/message", HTTP_POST, handleApiNoticePost);
    server.on("/api/oled", HTTP_GET, handleApiNoticeGet);
    server.on("/api/oled", HTTP_POST, handleApiNoticePost);
    server.on("/notice", HTTP_GET, handleApiNoticeGet);
    server.on("/notice", HTTP_POST, handleApiNoticePost);
    server.on("/api/logs", HTTP_GET, handleApiLogsGet);
    server.on("/api/logs/clear", HTTP_POST, handleApiLogsClear);
    server.on("/api/login", HTTP_POST, handleApiLoginPost);
    server.on("/api/login", HTTP_GET, handleApiLoginPost);
    server.on("/api/wifi", HTTP_POST, handleApiWifiPost);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.printf("[HTTP] Web Server successfully started on port %d\n", DEFAULT_HTTP_PORT);
}
inline void httpServerUpdate() {
    server.handleClient();
}
#endif  
