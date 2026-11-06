#ifndef CAPTIVEPORTAL_H
#define CAPTIVEPORTAL_H
#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include "wifi_manager.h"
static DNSServer s_dnsServer;
static bool s_dnsRunning = false;
inline void captivePortalInit() {
    if (isApMode()) {
        s_dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
        s_dnsServer.start(53, "*", WiFi.softAPIP());
        s_dnsRunning = true;
        Serial.printf("[PORTAL] DNS Server active on port 53 (Catch-all -> %s)\n", WiFi.softAPIP().toString().c_str());
    } else {
        s_dnsRunning = false;
    }
}
inline void captivePortalUpdate() {
    if (isApMode()) {
        if (!s_dnsRunning) {
            captivePortalInit();
        }
        s_dnsServer.processNextRequest();
    } else {
        if (s_dnsRunning) {
            s_dnsServer.stop();
            s_dnsRunning = false;
        }
    }
}
inline bool isCaptivePortalRequest(const String& host) {
    if (!isApMode()) return false;
    String apIp = WiFi.softAPIP().toString();
    if (host == apIp || host == "smartclassroom.local" || host.startsWith("192.168.4.")) {
        return false;
    }
    return true;
}
#endif  
