#ifndef WINDOW_H
#define WINDOW_H
#include <Arduino.h>
#include <ESP32Servo.h>
#include "../config/pins.h"
#include "../config/thresholds.h"
#include "../config/device_config.h"
#include "../core/systemstate.h"
#define WIN_START_ANGLE  10    
#define WIN_END_ANGLE   102    
#define WIN_SERVO_STEP_MS 25   
#define WIN_AUTO_EVAL_INTERVAL_MS 3000
static Servo s_winServo1;         
static Servo s_winServo2;         
static bool s_winOpen     = false;  
static bool s_winMoving   = false;  
static bool s_winAttached = false;
static unsigned long s_lastWinAutoEvalMs = 0;
static inline void _winSweepOpen() {
    s_winMoving = true;
    Serial.println(F("[Window] Sweeping OPEN  (S1: 10->102, S2: 102->10)"));
    addSystemLog("window", "[WINDOW] Motorized Windows Sweeping OPEN (S1: 10->102 deg, S2: 102->10 deg)");
    for (int a = WIN_START_ANGLE; a <= WIN_END_ANGLE; a++) {
        if (s_winServo1.attached()) s_winServo1.write(a);
        if (s_winServo2.attached()) s_winServo2.write(WIN_END_ANGLE - (a - WIN_START_ANGLE));
        delay(WIN_SERVO_STEP_MS);
    }
    s_winOpen = true;
    systemState.windowOpen = true;
    s_winMoving = false;
    Serial.println(F("[Window] State -> OPEN"));
    addSystemLog("window", "[WINDOW] State -> OPEN (Synchronized)");
}
static inline void _winSweepClose() {
    s_winMoving = true;
    Serial.println(F("[Window] Sweeping CLOSED (S1: 102->10, S2: 10->102)"));
    addSystemLog("window", "[WINDOW] Motorized Windows Sweeping CLOSED (S1: 102->10 deg, S2: 10->102 deg)");
    for (int a = WIN_END_ANGLE; a >= WIN_START_ANGLE; a--) {
        if (s_winServo1.attached()) s_winServo1.write(a);
        if (s_winServo2.attached()) s_winServo2.write(WIN_END_ANGLE - (a - WIN_START_ANGLE));
        delay(WIN_SERVO_STEP_MS);
    }
    s_winOpen = false;
    systemState.windowOpen = false;
    s_winMoving = false;
    Serial.println(F("[Window] State -> CLOSED"));
    addSystemLog("window", "[WINDOW] State -> CLOSED (Synchronized)");
}
inline bool openWindows() {
    if (s_winMoving) {
        Serial.println(F("[Window] Busy: motion in progress."));
        return false;
    }
    if (s_winOpen) {
        Serial.println(F("[Window] Already OPEN."));
        return true;
    }
    _winSweepOpen();
    return true;
}
inline bool closeWindows() {
    if (s_winMoving) {
        Serial.println(F("[Window] Busy: motion in progress."));
        return false;
    }
    if (!s_winOpen) {
        Serial.println(F("[Window] Already CLOSED."));
        return true;
    }
    _winSweepClose();
    return true;
}
inline void setWindowAuto() {
    systemState.windowMode = MODE_AUTO;
    Serial.println(F("[Window] Mode -> AUTO (LDR Automation Enabled)"));
    addSystemLog("window", "[WINDOW] Mode set to AUTO (LDR Automation)");
}
inline void setWindowManual(bool open) {
    systemState.windowMode = open ? MODE_MANUAL_ON : MODE_MANUAL_OFF;
    Serial.printf("[Window] Mode -> MANUAL_%s\n", open ? "OPEN" : "CLOSED");
    addSystemLogf("window", "[WINDOW] Mode set to MANUAL (%s)", open ? "OPEN" : "CLOSED");
    if (open) {
        openWindows();
    } else {
        closeWindows();
    }
}
inline void windowInit() {
    Serial.printf("[Window] Initializing dual-servo actuator on GPIO %d & %d...\n",
                  PIN_WINDOW_SERVO_1, PIN_WINDOW_SERVO_2);
    s_winServo1.setPeriodHertz(50);
    s_winServo2.setPeriodHertz(50);
    int p1 = s_winServo1.attach(PIN_WINDOW_SERVO_1, 500, 2500);
    int p2 = s_winServo2.attach(PIN_WINDOW_SERVO_2, 500, 2500);
    s_winAttached = (p1 != -1 && p2 != -1);
    if (s_winServo1.attached()) s_winServo1.write(WIN_START_ANGLE);
    if (s_winServo2.attached()) s_winServo2.write(WIN_END_ANGLE);
    s_winOpen   = false;
    s_winMoving = false;
    systemState.windowOpen = false;
    systemState.windowMode = MODE_AUTO;
    if (s_winAttached) {
        Serial.printf("[Window] Servo 1 (GPIO %d) & Servo 2 (GPIO %d) attached. State: CLOSED\n",
                      PIN_WINDOW_SERVO_1, PIN_WINDOW_SERVO_2);
    } else {
        Serial.println(F("[Window] Note: Servos running in software-only state mode."));
    }
}
inline void windowUpdate() {
    if (s_winMoving) {
        return;
    }
    if (systemState.windowMode != MODE_AUTO) {
        return;
    }
    unsigned long now = millis();
    if (now - s_lastWinAutoEvalMs < WIN_AUTO_EVAL_INTERVAL_MS) {
        return;
    }
    s_lastWinAutoEvalMs = now;
    int lightVal = systemState.lightIntensity;
    if (lightVal < (LIGHT_THRESHOLD_DEFAULT - LIGHT_HYSTERESIS)) {
        if (!s_winOpen) {
            Serial.printf("[Window][AUTO] Bright daylight detected (%d < %d) -> Opening windows\n",
                          lightVal, (LIGHT_THRESHOLD_DEFAULT - LIGHT_HYSTERESIS));
            addSystemLogf("window", "[WINDOW][AUTO] Daylight (%d) -> Opening windows", lightVal);
            openWindows();
        }
    }
    else if (lightVal > (LIGHT_THRESHOLD_DEFAULT + LIGHT_HYSTERESIS)) {
        if (s_winOpen) {
            Serial.printf("[Window][AUTO] Low light / night detected (%d > %d) -> Closing windows\n",
                          lightVal, (LIGHT_THRESHOLD_DEFAULT + LIGHT_HYSTERESIS));
            addSystemLogf("window", "[WINDOW][AUTO] Darkness (%d) -> Closing windows", lightVal);
            closeWindows();
        }
    }
}
inline bool isWindowOpen()  { return s_winOpen;   }
inline bool isWindowBusy()  { return s_winMoving; }
inline ControlMode getWindowMode() { return systemState.windowMode; }
inline String getWindowState() {
    if (s_winMoving) return "MOVING";
    return s_winOpen ? "OPEN" : "CLOSED";
}
#endif 