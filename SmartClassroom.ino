#include "config/pins.h"
#include "config/thresholds.h"
#include "config/device_config.h"
#include "core/systemstate.h"
#include <ESP32Servo.h>
#include "smoke/smoke.h"
#include "ldr_light/ldr_light.h"
#include "dht_fan/dht_fan.h"
#include "window/window.h"
#include "door/door.h"
#include "oleddisplayiot/oleddisplayiot.h"
#include "dashboard/dashboard.h"
#include "network/wifi_manager.h"
#include "network/captiveportal.h"
#include "network/httpserver.h"
static unsigned long lastTelemetryLogMs = 0;
void setup() {
    Serial.begin(115200);
    unsigned long startWait = millis();
    while (!Serial && (millis() - startWait < 1500)) {
        delay(10);
    }
    Serial.println("\n=======================================================");
    Serial.println("  S5CSE A — SMART CLASSROOM MASTER CONTROLLER");
    Serial.println("  Hardware: ESP32-S3 N16R8 (Direct Peripheral Bus)");
    Serial.println("=======================================================");
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    systemStateInit();
    smokeInit();
    ldrLightInit();
    dhtFanInit();
    windowInit();
    doorInit();
    oledInit();
    wifiInit();
    captivePortalInit();
    httpServerInit();
    dashboardInit();
    Serial.println("[BOOT] All Classroom Subsystems Initialized Successfully");
    Serial.printf("[BOOT] Ready. Dashboard hosted at http://%s\n\n", systemState.ipAddress.c_str());
    addSystemLog("system", "[BOOT] All Classroom Subsystems Initialized Successfully");
    addSystemLogf("system", "[BOOT] Dashboard hosted at http://%s", systemState.ipAddress.c_str());
}
void loop() {
    wifiUpdate();
    captivePortalUpdate();
    httpServerUpdate();
    ldrLightUpdate();
    dhtFanUpdate();
    smokeUpdate();
    windowUpdate();
    doorUpdate();
    oledUpdate();
    dashboardUpdate();
    systemStateUpdate();
    unsigned long now = millis();
    if (now - lastTelemetryLogMs >= STATUS_LOG_INTERVAL_MS) {
        lastTelemetryLogMs = now;
        Serial.printf("[STATUS] Temp: %.1f C | Hum: %.1f %% | Light: %d%% (%s) | Fan: %s (%s) | Smoke: %d (%s) | Door: %s | IP: %s\n",
                      systemState.temperature,
                      systemState.humidity,
                      systemState.lightIntensity,
                      controlModeToString(systemState.lightMode),
                      systemState.fanOn ? "ON" : "OFF",
                      controlModeToString(systemState.fanMode),
                      systemState.smokeLevel,
                      systemState.smokeDetected ? "ALERT" : "NORMAL",
                      systemState.doorLocked ? "LOCKED" : "OPEN",
                      systemState.ipAddress.c_str());
        char statusBuf[128];
        snprintf(statusBuf, sizeof(statusBuf), "[STATUS] T:%.1fC | H:%.1f%% | Light:%d | Fan:%s | Smoke:%d | Door:%s",
                 systemState.temperature,
                 systemState.humidity,
                 systemState.lightIntensity,
                 systemState.fanOn ? "ON" : "OFF",
                 systemState.smokeLevel,
                 systemState.doorLocked ? "LCK" : "OPN");
        addSystemLog("system", statusBuf);
    }
}
