#ifndef DOOR_H
#define DOOR_H
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <ESP32Servo.h>
#include "../config/pins.h"
#include "../config/thresholds.h"
#include "../config/device_config.h"
#include "../core/systemstate.h"
#ifndef PN532_SCK
#define PN532_SCK 14
#endif
#ifndef PN532_MISO
#define PN532_MISO 20
#endif
#ifndef PN532_MOSI
#define PN532_MOSI 13
#endif
#ifndef PN532_SS
#define PN532_SS 12
#endif
#ifndef SERVO_PIN
#define SERVO_PIN 18
#endif
#ifndef SERVO_CLOSED
#define SERVO_CLOSED 180
#endif
#ifndef SERVO_OPEN
#define SERVO_OPEN 0
#endif
#ifndef SERVO_STEP_DELAY
#define SERVO_STEP_DELAY 20
#endif
#ifndef DOOR_OPEN_TIME
#define DOOR_OPEN_TIME 2000
#endif
#ifndef RFID_COOLDOWN
#define RFID_COOLDOWN 2500
#endif
static Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
static Servo doorServo;
static bool s_pn532Ready = false;
static uint8_t lastUID[7] = {0};
static uint8_t lastUIDLength = 0;
static unsigned long lastAcceptedScan = 0;
inline bool compareUID(uint8_t *scannedUID, uint8_t scannedLength,
                       const uint8_t *storedUID, uint8_t storedLength) {
    if (scannedLength != storedLength) {
        return false;
    }
    for (uint8_t i = 0; i < storedLength; i++) {
        if (scannedUID[i] != storedUID[i]) {
            return false;
        }
    }
    return true;
}
inline int findUser(uint8_t *uid, uint8_t uidLength) {
    for (size_t i = 0; i < AUTHORIZED_USER_COUNT; i++) {
        if (compareUID(uid, uidLength, authorizedUsers[i].uid, authorizedUsers[i].uidLength)) {
            return (int)i;
        }
    }
    return -1;
}
inline void printUID(uint8_t *uid, uint8_t uidLength) {
    Serial.print(F("[RFID] UID: "));
    for (uint8_t i = 0; i < uidLength; i++) {
        if (uid[i] < 0x10) {
            Serial.print(F("0"));
        }
        Serial.print(uid[i], HEX);
        if (i < uidLength - 1) {
            Serial.print(F(" "));
        }
    }
    Serial.println();
}
inline String buildUidString(const uint8_t *uid, uint8_t uidLength) {
    String result = "";
    result.reserve(uidLength * 3);
    for (uint8_t i = 0; i < uidLength; i++) {
        if (uid[i] < 0x10) {
            result += "0";
        }
        result += String(uid[i], HEX);
        if (i < uidLength - 1) {
            result += " ";
        }
    }
    result.toUpperCase();
    return result;
}
inline void logAttendance(AuthorizedUser &user) {
    user.present = !user.present;
    AttendanceLogEntry &entry = s_attendanceLogs[s_attendanceLogHead];
    strncpy(entry.name, user.name, sizeof(entry.name) - 1);
    entry.name[sizeof(entry.name) - 1] = '\0';
    entry.present = user.present;
    entry.timestampMs = millis();
    s_attendanceLogHead = (s_attendanceLogHead + 1) % MAX_ATTENDANCE_LOGS;
    if (s_attendanceLogCount < MAX_ATTENDANCE_LOGS) {
        s_attendanceLogCount++;
    }
    Serial.println();
    Serial.println(F("[ATTENDANCE] ========================="));
    Serial.print(F("[ATTENDANCE] Student: "));
    Serial.println(user.name);
    Serial.print(F("[ATTENDANCE] Event: "));
    Serial.println(user.present ? F("ENTRY") : F("EXIT"));
    Serial.print(F("[ATTENDANCE] Status: "));
    Serial.println(user.present ? F("PRESENT") : F("ABSENT"));
    Serial.println(F("[ATTENDANCE] Logged"));
    Serial.println(F("[ATTENDANCE] ========================="));
    addSystemLogf("rfid", "[ATTENDANCE] %s marked %s", user.name, user.present ? "PRESENT" : "ABSENT");
}
inline void initializeServo() {
    Serial.println();
    Serial.println(F("[SERVO] Initializing..."));
    Serial.println(F("[SERVO] GPIO 18"));
    doorServo.setPeriodHertz(50);
    int result = doorServo.attach(SERVO_PIN, 500, 2400);
    if (result >= 0 && doorServo.attached()) {
        Serial.println(F("[SERVO] ATTACHED SUCCESSFULLY"));
    } else {
        Serial.println(F("[SERVO] ERROR: ATTACH FAILED"));
    }
    doorServo.write(SERVO_CLOSED);
    delay(1000);
    Serial.println(F("[SERVO] CLOSED @ 180 degrees"));
}
inline bool openDoor() {
    Serial.println();
    Serial.println(F("[DOOR] ================================"));
    Serial.println(F("[DOOR] ACCESS GRANTED"));
    Serial.println(F("[DOOR] OPENING"));
    Serial.println(F("[DOOR] 180 -> 0 degrees"));
    Serial.println(F("[DOOR] ================================"));
    if (!doorServo.attached()) {
        Serial.println(F("[DOOR] ERROR: Servo NOT attached!"));
        addSystemLog("rfid", "[DOOR] ERROR: Door servo not attached");
        return false;
    }
    systemState.doorState = DOOR_OPENING;
    for (int angle = SERVO_CLOSED; angle >= SERVO_OPEN; angle -= 2) {
        doorServo.write(angle);
        delay(SERVO_STEP_DELAY);
    }
    doorServo.write(SERVO_OPEN);
    systemState.doorOpen = true;
    systemState.doorState = DOOR_OPEN;
    systemState.doorLocked = false;
    Serial.println(F("[DOOR] OPEN @ 0 degrees"));
    addSystemLog("rfid", "[DOOR] Door UNLOCKED & OPEN @ 0 deg");
    return true;
}
inline bool closeDoor() {
    Serial.println();
    Serial.println(F("[DOOR] ================================"));
    Serial.println(F("[DOOR] CLOSING"));
    Serial.println(F("[DOOR] 0 -> 180 degrees"));
    Serial.println(F("[DOOR] ================================"));
    if (!doorServo.attached()) {
        Serial.println(F("[DOOR] ERROR: Servo NOT attached!"));
        addSystemLog("rfid", "[DOOR] ERROR: Door servo not attached");
        return false;
    }
    systemState.doorState = DOOR_CLOSING;
    for (int angle = SERVO_OPEN; angle <= SERVO_CLOSED; angle += 2) {
        doorServo.write(angle);
        delay(SERVO_STEP_DELAY);
    }
    doorServo.write(SERVO_CLOSED);
    systemState.doorOpen = false;
    systemState.doorState = DOOR_CLOSED;
    systemState.doorLocked = true;
    Serial.println(F("[DOOR] CLOSED @ 180 degrees"));
    addSystemLog("rfid", "[DOOR] Door CLOSED & LOCKED @ 180 deg");
    return true;
}
inline void performAuthorizedDoorCycle(const char *name) {
    Serial.println();
    Serial.println(F("[ACCESS] ACCESS GRANTED"));
    Serial.print(F("[ACCESS] Welcome "));
    Serial.println(name);
    Serial.println(F("[ACCESS] Starting door cycle"));
    addSystemLogf("rfid", "[ACCESS] ACCESS GRANTED for %s", name);
    bool opened = openDoor();
    if (!opened) {
        Serial.println(F("[DOOR] ERROR: Door could not open"));
        addSystemLog("rfid", "[ACCESS] ERROR: Door could not open");
        return;
    }
    Serial.printf("[DOOR] Holding OPEN for %d ms\n", DOOR_OPEN_TIME);
    delay(DOOR_OPEN_TIME);
    bool closed = closeDoor();
    if (!closed) {
        Serial.println(F("[DOOR] ERROR: Door could not close"));
        addSystemLog("rfid", "[DOOR] ERROR: Door could not close");
    }
    Serial.println();
    Serial.println(F("=============================================="));
    Serial.println(F("[SYSTEM] DOOR CYCLE COMPLETE"));
    Serial.println(F("[SYSTEM] DOOR SECURED"));
    Serial.println(F("[SYSTEM] Waiting for next RFID..."));
    Serial.println(F("=============================================="));
    addSystemLog("rfid", "[DOOR] Cycle complete — Door safely secured");
}
inline void unlockDoor() {
    if (openDoor()) {
        delay(DOOR_OPEN_TIME);
        closeDoor();
    }
}
inline void lockDoor() {
    closeDoor();
}
inline void dashboardDoorOpen() {
    openDoor();
}
inline void dashboardDoorLock() {
    closeDoor();
}
inline bool isDoorLocked() {
    return systemState.doorLocked;
}
inline bool isDoorOpen() {
    return systemState.doorOpen;
}
inline DoorState getDoorState() {
    return systemState.doorState;
}
inline void doorInit() {
    Serial.println();
    Serial.println(F("=============================================="));
    Serial.println(F(" S5 CSE A SMART CLASSROOM"));
    Serial.println(F(" RFID VERIFIED DOOR + ATTENDANCE"));
    Serial.println(F("=============================================="));
    Serial.println();
    Serial.println(F("[CONFIG]"));
    Serial.println(F("----------------------------------------------"));
    Serial.printf("PN532 CS   : GPIO %d\n", PN532_SS);
    Serial.printf("PN532 MOSI : GPIO %d\n", PN532_MOSI);
    Serial.printf("PN532 SCK  : GPIO %d\n", PN532_SCK);
    Serial.printf("PN532 MISO : GPIO %d\n", PN532_MISO);
    Serial.printf("Door Servo : GPIO %d\n", SERVO_PIN);
    Serial.println(F("----------------------------------------------"));
    Serial.println(F("[AUTH] Authorized RFID database:"));
    for (size_t i = 0; i < AUTHORIZED_USER_COUNT; i++) {
        Serial.print(F("[AUTH] "));
        Serial.print(authorizedUsers[i].name);
        Serial.print(F(" -> "));
        for (uint8_t b = 0; b < authorizedUsers[i].uidLength; b++) {
            if (authorizedUsers[i].uid[b] < 0x10) Serial.print(F("0"));
            Serial.print(authorizedUsers[i].uid[b], HEX);
            if (b < authorizedUsers[i].uidLength - 1) Serial.print(F(" "));
        }
        Serial.println();
    }
    initializeServo();
    systemState.doorState = DOOR_CLOSED;
    systemState.doorOpen = false;
    systemState.doorLocked = true;
    Serial.println();
    Serial.println(F("[PN532] Initializing Software SPI reader..."));
    Serial.printf("[PN532] CS: %d | MOSI: %d | SCK: %d | MISO: %d\n",
                  PN532_SS, PN532_MOSI, PN532_SCK, PN532_MISO);
    nfc.begin();
    delay(100);
    uint32_t version = nfc.getFirmwareVersion();
    if (!version) {
        s_pn532Ready = false;
        systemState.rfidAvailable = false;
        Serial.println(F("[PN532] ERROR: READER NOT FOUND!"));
        Serial.println(F("[PN532] Check wiring and power:"));
        Serial.printf("[PN532] - SCK  -> GPIO %d\n", PN532_SCK);
        Serial.printf("[PN532] - MISO -> GPIO %d\n", PN532_MISO);
        Serial.printf("[PN532] - MOSI -> GPIO %d\n", PN532_MOSI);
        Serial.printf("[PN532] - SS   -> GPIO %d\n", PN532_SS);
        addSystemLog("rfid", "[PN532] ERROR: RFID Reader Hardware Not Detected");
        return;
    }
    s_pn532Ready = true;
    systemState.rfidAvailable = true;
    Serial.printf("[PN532] READER DETECTED - Firmware: %d.%d\n",
                  (version >> 16) & 0xFF, (version >> 8) & 0xFF);
    nfc.SAMConfig();
    nfc.setPassiveActivationRetries(0x11);
    Serial.println(F("[PN532] SAMConfig OK - RFID READER READY"));
    addSystemLog("rfid", "[PN532] RFID Access Reader Online (SCK 14, MISO 20, MOSI 13, SS 12)");
    Serial.println();
    Serial.println(F("=============================================="));
    Serial.println(F("[SYSTEM] DOOR ACCESS SYSTEM READY"));
    Serial.println(F("[SYSTEM] Authorized: Rahul + Laksmi"));
    Serial.println(F("[SYSTEM] Unknown cards: REJECTED"));
    Serial.println(F("[SYSTEM] Waiting for RFID..."));
    Serial.println(F("=============================================="));
}
inline void doorUpdate() {
    if (!s_pn532Ready) {
        return;
    }
    uint8_t uid[7] = {0};
    uint8_t uidLength = 0;
    bool detected = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50);
    if (!detected || uidLength == 0) {
        return;
    }
    Serial.println();
    Serial.println(F("=============================================="));
    Serial.println(F("[RFID] CARD DETECTED"));
    Serial.println(F("=============================================="));
    Serial.printf("[RFID] UID Length: %d bytes\n", uidLength);
    printUID(uid, uidLength);
    unsigned long now = millis();
    bool sameCard = (uidLength == lastUIDLength) && (memcmp(uid, lastUID, uidLength) == 0);
    if (sameCard && (now - lastAcceptedScan < RFID_COOLDOWN)) {
        Serial.println(F("[RFID] Duplicate scan ignored"));
        Serial.println(F("[RFID] Cooldown active"));
        return;
    }
    String uidStr = buildUidString(uid, uidLength);
    systemState.lastRfidUid = uidStr;
    systemState.lastRfidScanMs = now;
    int userIndex = findUser(uid, uidLength);
    if (userIndex < 0) {
        Serial.println();
        Serial.println(F("[AUTH] Identity: UNKNOWN"));
        Serial.println(F("[AUTH] STATUS: UNAUTHORIZED"));
        Serial.println(F("[ACCESS] ACCESS DENIED"));
        Serial.println(F("[ATTENDANCE] NOT LOGGED"));
        Serial.println(F("[DOOR] Door remains CLOSED"));
        Serial.println(F("[DOOR] Servo NOT triggered"));
        Serial.println(F("----------------------------------------------"));
        systemState.lastRfidName = "Unknown Card";
        systemState.lastRfidStatus = "ACCESS DENIED";
        systemState.lastRfidAuthorized = false;
        addSystemLogf("rfid", "[AUTH] UID %s: UNKNOWN / UNAUTHORIZED -> ACCESS DENIED (Door CLOSED)", uidStr.c_str());
        memcpy(lastUID, uid, uidLength);
        lastUIDLength = uidLength;
        lastAcceptedScan = now;
        return;
    }
    AuthorizedUser &user = authorizedUsers[userIndex];
    Serial.println();
    Serial.print(F("[AUTH] Identity: "));
    Serial.println(user.name);
    Serial.println(F("[AUTH] STATUS: AUTHORIZED"));
    memcpy(lastUID, uid, uidLength);
    lastUIDLength = uidLength;
    lastAcceptedScan = now;
    logAttendance(user);
    systemState.lastRfidName = user.name;
    systemState.lastRfidStatus = "AUTHORIZED";
    systemState.lastRfidAuthorized = true;
    performAuthorizedDoorCycle(user.name);
}
#endif 