#ifndef SYSTEMSTATE_H
#define SYSTEMSTATE_H
#include <Arduino.h>
#include "../config/device_config.h"
#include "logger.h"
enum ControlMode {
    MODE_AUTO = 0,
    MODE_MANUAL_ON = 1,
    MODE_MANUAL_OFF = 2
};
enum DoorState {
    DOOR_CLOSED = 0,
    DOOR_OPENING = 1,
    DOOR_OPEN = 2,
    DOOR_CLOSING = 3
};
struct ClassroomState {
    float temperature;
    float humidity;
    bool dhtFault;
    int lightIntensity;          
    bool lightOn;                
    ControlMode lightMode;       
    bool ldrFault;
    int smokeLevel;              
    bool smokeDetected;          
    bool mq2Fault;
    bool fanOn;                  
    ControlMode fanMode;         
    bool windowOpen;             
    ControlMode windowMode;      
    bool doorOpen;               
    bool doorLocked;             
    DoorState doorState;         
    String lastRfidUid;          
    String lastRfidName;         
    String lastRfidStatus;       
    bool lastRfidAuthorized;     
    unsigned long lastRfidScanMs; 
    bool rfidAvailable;          
    String currentNotice;        
    bool oledAvailable;          
    bool wifiConnected;          
    bool apModeActive;           
    String ipAddress;            
    String ssid;                 
    int wifiRssi;                
    unsigned long uptimeSeconds; 
    int connectedDevicesCount;   
};

struct ConnectedClient {
    char ip[24];
    char mac[18];
    char name[36];
    char type[16];
    unsigned long lastSeenMs;
    bool active;
};
#define MAX_CONNECTED_CLIENTS 12
#ifndef CONNECTED_CLIENTS_DEFINED
#define CONNECTED_CLIENTS_DEFINED
ConnectedClient connectedClients[MAX_CONNECTED_CLIENTS];
int connectedClientsCount = 0;
#endif

inline void updateConnectedClientsExpiry() {
    unsigned long now = millis();
    int count = 0;
    for (int i = 0; i < MAX_CONNECTED_CLIENTS; i++) {
        if (connectedClients[i].ip[0] != '\0' || connectedClients[i].mac[0] != '\0') {
            if (now - connectedClients[i].lastSeenMs < 60000UL) {
                connectedClients[i].active = true;
                count++;
            } else {
                connectedClients[i].active = false;
            }
        }
    }
    connectedClientsCount = count;
}

inline void registerOrUpdateClient(const char* ip, const char* userAgent, const char* customName = nullptr, const char* mac = nullptr) {
    if ((!ip || strlen(ip) == 0 || strcmp(ip, "0.0.0.0") == 0) && (!mac || strlen(mac) == 0)) return;
    unsigned long now = millis();
    int foundIdx = -1;
    int emptyIdx = -1;
    unsigned long oldestTime = 0xFFFFFFFF;
    int oldestIdx = 0;

    for (int i = 0; i < MAX_CONNECTED_CLIENTS; i++) {
        if (ip && strlen(ip) > 0 && strcmp(connectedClients[i].ip, ip) == 0) {
            foundIdx = i;
            break;
        }
        if (mac && strlen(mac) > 0 && strcmp(connectedClients[i].mac, mac) == 0) {
            foundIdx = i;
            break;
        }
        if (emptyIdx == -1 && connectedClients[i].ip[0] == '\0' && connectedClients[i].mac[0] == '\0') {
            emptyIdx = i;
        }
        if (connectedClients[i].lastSeenMs < oldestTime) {
            oldestTime = connectedClients[i].lastSeenMs;
            oldestIdx = i;
        }
    }

    int targetIdx = foundIdx;
    if (targetIdx == -1) {
        targetIdx = (emptyIdx != -1) ? emptyIdx : oldestIdx;
        memset(&connectedClients[targetIdx], 0, sizeof(ConnectedClient));
        if (ip && strlen(ip) > 0) {
            strncpy(connectedClients[targetIdx].ip, ip, sizeof(connectedClients[targetIdx].ip) - 1);
        }
        if (mac && strlen(mac) > 0) {
            strncpy(connectedClients[targetIdx].mac, mac, sizeof(connectedClients[targetIdx].mac) - 1);
        }
        if (userAgent && strlen(userAgent) > 0) {
            String ua = userAgent;
            if (ua.indexOf("iPhone") >= 0) {
                strncpy(connectedClients[targetIdx].name, "Apple iPhone", sizeof(connectedClients[targetIdx].name) - 1);
                strncpy(connectedClients[targetIdx].type, "Mobile", sizeof(connectedClients[targetIdx].type) - 1);
            } else if (ua.indexOf("iPad") >= 0) {
                strncpy(connectedClients[targetIdx].name, "Apple iPad", sizeof(connectedClients[targetIdx].name) - 1);
                strncpy(connectedClients[targetIdx].type, "Tablet", sizeof(connectedClients[targetIdx].type) - 1);
            } else if (ua.indexOf("Android") >= 0) {
                if (ua.indexOf("Mobile") >= 0) {
                    strncpy(connectedClients[targetIdx].name, "Android Phone", sizeof(connectedClients[targetIdx].name) - 1);
                    strncpy(connectedClients[targetIdx].type, "Mobile", sizeof(connectedClients[targetIdx].type) - 1);
                } else {
                    strncpy(connectedClients[targetIdx].name, "Android Tablet", sizeof(connectedClients[targetIdx].name) - 1);
                    strncpy(connectedClients[targetIdx].type, "Tablet", sizeof(connectedClients[targetIdx].type) - 1);
                }
            } else if (ua.indexOf("Windows") >= 0) {
                strncpy(connectedClients[targetIdx].name, "Windows PC", sizeof(connectedClients[targetIdx].name) - 1);
                strncpy(connectedClients[targetIdx].type, "Desktop", sizeof(connectedClients[targetIdx].type) - 1);
            } else if (ua.indexOf("Macintosh") >= 0 || ua.indexOf("Mac OS") >= 0) {
                strncpy(connectedClients[targetIdx].name, "Apple Mac", sizeof(connectedClients[targetIdx].name) - 1);
                strncpy(connectedClients[targetIdx].type, "Laptop", sizeof(connectedClients[targetIdx].type) - 1);
            } else if (ua.indexOf("CrOS") >= 0) {
                strncpy(connectedClients[targetIdx].name, "Chromebook", sizeof(connectedClients[targetIdx].name) - 1);
                strncpy(connectedClients[targetIdx].type, "Laptop", sizeof(connectedClients[targetIdx].type) - 1);
            } else if (ua.indexOf("Linux") >= 0) {
                strncpy(connectedClients[targetIdx].name, "Linux Device", sizeof(connectedClients[targetIdx].name) - 1);
                strncpy(connectedClients[targetIdx].type, "Desktop", sizeof(connectedClients[targetIdx].type) - 1);
            } else {
                strncpy(connectedClients[targetIdx].name, "Wi-Fi Device", sizeof(connectedClients[targetIdx].name) - 1);
                strncpy(connectedClients[targetIdx].type, "Client", sizeof(connectedClients[targetIdx].type) - 1);
            }
        } else {
            strncpy(connectedClients[targetIdx].name, "Wi-Fi Client", sizeof(connectedClients[targetIdx].name) - 1);
            strncpy(connectedClients[targetIdx].type, "Client", sizeof(connectedClients[targetIdx].type) - 1);
        }
    }

    connectedClients[targetIdx].lastSeenMs = now;
    connectedClients[targetIdx].active = true;

    if (ip && strlen(ip) > 0 && strcmp(ip, "0.0.0.0") != 0) {
        strncpy(connectedClients[targetIdx].ip, ip, sizeof(connectedClients[targetIdx].ip) - 1);
    }
    if (mac && strlen(mac) > 0) {
        strncpy(connectedClients[targetIdx].mac, mac, sizeof(connectedClients[targetIdx].mac) - 1);
    }
    if (customName && strlen(customName) > 0) {
        strncpy(connectedClients[targetIdx].name, customName, sizeof(connectedClients[targetIdx].name) - 1);
    }
    updateConnectedClientsExpiry();
}

inline bool renameConnectedClient(const char* ip, const char* newName) {
    if (!ip || !newName || strlen(newName) == 0) return false;
    for (int i = 0; i < MAX_CONNECTED_CLIENTS; i++) {
        if (strcmp(connectedClients[i].ip, ip) == 0) {
            strncpy(connectedClients[i].name, newName, sizeof(connectedClients[i].name) - 1);
            connectedClients[i].name[sizeof(connectedClients[i].name) - 1] = '\0';
            return true;
        }
    }
    return false;
}

extern ClassroomState systemState;
#ifndef SYSTEMSTATE_DEFINED
#define SYSTEMSTATE_DEFINED
ClassroomState systemState;
#endif
inline void systemStateInit() {
    systemState.temperature = 25.0f;
    systemState.humidity = 50.0f;
    systemState.dhtFault = false;
    systemState.lightIntensity = 0;
    systemState.lightOn = false;
    systemState.lightMode = MODE_AUTO;
    systemState.ldrFault = false;
    systemState.smokeLevel = 0;
    systemState.smokeDetected = false;
    systemState.mq2Fault = false;
    systemState.fanOn = false;
    systemState.fanMode = MODE_AUTO;
    systemState.windowOpen = false;
    systemState.windowMode = MODE_AUTO;
    systemState.doorOpen = false;
    systemState.doorLocked = true;
    systemState.doorState = DOOR_CLOSED;
    systemState.lastRfidUid = "None";
    systemState.lastRfidName = "None";
    systemState.lastRfidStatus = "IDLE";
    systemState.lastRfidAuthorized = false;
    systemState.lastRfidScanMs = 0;
    systemState.rfidAvailable = true;
    systemState.currentNotice = DEFAULT_CLASSROOM_NOTICE;
    systemState.oledAvailable = true;
    systemState.wifiConnected = false;
    systemState.apModeActive = true;
    systemState.ipAddress = "192.168.4.1";
    systemState.ssid = DEFAULT_AP_SSID;
    systemState.wifiRssi = 0;
    systemState.uptimeSeconds = 0;
    systemState.connectedDevicesCount = 0;
    for (int i = 0; i < MAX_CONNECTED_CLIENTS; i++) {
        memset(&connectedClients[i], 0, sizeof(ConnectedClient));
    }
    addSystemLog("system", "[BOOT] Classroom System State Initialized");
    Serial.println("[BOOT] Classroom System State Initialized");
}
inline void systemStateUpdate() {
    systemState.uptimeSeconds = millis() / 1000UL;
}
inline const char* doorStateToString(DoorState state) {
    switch (state) {
        case DOOR_CLOSED:  return "CLOSED";
        case DOOR_OPENING: return "OPENING";
        case DOOR_OPEN:    return "OPEN";
        case DOOR_CLOSING: return "CLOSING";
        default:           return "CLOSED";
    }
}
inline const char* controlModeToString(ControlMode mode) {
    switch (mode) {
        case MODE_AUTO:       return "AUTO";
        case MODE_MANUAL_ON:  return "MANUAL_ON";
        case MODE_MANUAL_OFF: return "MANUAL_OFF";
        default:              return "AUTO";
    }
}
inline ControlMode stringToControlMode(const String& str) {
    String lower = str;
    lower.toLowerCase();
    lower.trim();
    if (lower == "on" || lower == "manual_on" || lower == "manualon") {
        return MODE_MANUAL_ON;
    } else if (lower == "off" || lower == "manual_off" || lower == "manualoff") {
        return MODE_MANUAL_OFF;
    }
    return MODE_AUTO;
}
inline String systemStateToJson() {
    updateConnectedClientsExpiry();
    int activeNetworkDevices = 0;
    for (int i = 0; i < MAX_CONNECTED_CLIENTS; i++) {
        if (connectedClients[i].active && (connectedClients[i].ip[0] != '\0' || connectedClients[i].mac[0] != '\0')) {
            activeNetworkDevices++;
        }
    }
    String json;
    json.reserve(6144);
    json = "{";
    json += "\"temperature\":" + String(systemState.temperature, 1) + ",";
    json += "\"humidity\":" + String(systemState.humidity, 1) + ",";
    json += "\"dhtFault\":" + String(systemState.dhtFault ? "true" : "false") + ",";
    json += "\"lightIntensity\":" + String(systemState.lightIntensity) + ",";
    json += "\"lightOn\":" + String(systemState.lightOn ? "true" : "false") + ",";
    json += "\"lightMode\":\"" + String(controlModeToString(systemState.lightMode)) + "\",";
    json += "\"ldrFault\":" + String(systemState.ldrFault ? "true" : "false") + ",";
    json += "\"smokeLevel\":" + String(systemState.smokeLevel) + ",";
    json += "\"smokeDetected\":" + String(systemState.smokeDetected ? "true" : "false") + ",";
    json += "\"mq2Fault\":" + String(systemState.mq2Fault ? "true" : "false") + ",";
    json += "\"fanOn\":" + String(systemState.fanOn ? "true" : "false") + ",";
    json += "\"fanMode\":\"" + String(controlModeToString(systemState.fanMode)) + "\",";
    json += "\"windowOpen\":" + String(systemState.windowOpen ? "true" : "false") + ",";
    json += "\"windowState\":\"" + String(systemState.windowOpen ? "OPEN" : "CLOSED") + "\",";
    json += "\"windowMode\":\"" + String(controlModeToString(systemState.windowMode)) + "\",";
    json += "\"doorOpen\":" + String(systemState.doorOpen ? "true" : "false") + ",";
    json += "\"doorLocked\":" + String(systemState.doorLocked ? "true" : "false") + ",";
    json += "\"doorState\":\"" + String(doorStateToString(systemState.doorState)) + "\",";
    json += "\"lastRfidUid\":\"" + systemState.lastRfidUid + "\",";
    json += "\"lastRfidName\":\"" + systemState.lastRfidName + "\",";
    json += "\"lastRfidStatus\":\"" + systemState.lastRfidStatus + "\",";
    json += "\"lastRfidAuthorized\":" + String(systemState.lastRfidAuthorized ? "true" : "false") + ",";
    unsigned long timeAgo = (systemState.lastRfidScanMs > 0) ? (millis() - systemState.lastRfidScanMs) / 1000UL : 999999UL;
    json += "\"lastRfidTimeAgo\":" + String(timeAgo) + ",";
    json += "\"rfidAvailable\":" + String(systemState.rfidAvailable ? "true" : "false") + ",";
    String escapedNotice = systemState.currentNotice;
    escapedNotice.replace("\"", "\\\"");
    json += "\"notice\":\"" + escapedNotice + "\",";
    json += "\"oledAvailable\":" + String(systemState.oledAvailable ? "true" : "false") + ",";
    json += "\"wifiConnected\":" + String(systemState.wifiConnected ? "true" : "false") + ",";
    json += "\"apModeActive\":" + String(systemState.apModeActive ? "true" : "false") + ",";
    json += "\"ipAddress\":\"" + systemState.ipAddress + "\",";
    json += "\"ssid\":\"" + systemState.ssid + "\",";
    json += "\"wifiRssi\":" + String(systemState.wifiRssi) + ",";
    json += "\"uptimeSeconds\":" + String(systemState.uptimeSeconds) + ",";
    json += "\"students\":[";
    for (size_t i = 0; i < AUTHORIZED_USER_COUNT; i++) {
        if (i > 0) json += ",";
        json += "{\"name\":\"" + String(authorizedUsers[i].name) + "\",\"present\":" + String(authorizedUsers[i].present ? "true" : "false") + "}";
    }
    json += "],";
    json += "\"attendanceLogs\":[";
    int logCount = (s_attendanceLogCount < MAX_ATTENDANCE_LOGS) ? s_attendanceLogCount : MAX_ATTENDANCE_LOGS;
    for (int i = 0; i < logCount; i++) {
        int idx;
        if (s_attendanceLogCount <= MAX_ATTENDANCE_LOGS) {
            idx = i;
        } else {
            idx = (s_attendanceLogHead + i) % MAX_ATTENDANCE_LOGS;
        }
        if (i > 0) json += ",";
        json += "{\"name\":\"" + String(s_attendanceLogs[idx].name) + "\",\"present\":" + String(s_attendanceLogs[idx].present ? "true" : "false") + ",\"ts\":" + String(s_attendanceLogs[idx].timestampMs) + "}";
    }
    json += "],";
    json += "\"recentLogs\":[";
    int logTotal = (s_systemLogCount < MAX_SYSTEM_LOGS) ? s_systemLogCount : MAX_SYSTEM_LOGS;
    int logStart = (logTotal > 35) ? (logTotal - 35) : 0;
    int sentLogs = 0;
    for (int i = logStart; i < logTotal; i++) {
        int idx = (s_systemLogCount <= MAX_SYSTEM_LOGS) ? i : (s_systemLogHead + i) % MAX_SYSTEM_LOGS;
        if (sentLogs > 0) json += ",";
        json += "{\"ts\":" + String(s_systemLogs[idx].ts) + ",\"cat\":\"" + String(s_systemLogs[idx].cat) + "\",\"msg\":\"";
        const char* p = s_systemLogs[idx].msg;
        while (*p) {
            if (*p == '"') {
                json += "\\\"";
            } else if (*p == '\\') {
                json += "\\\\";
            } else if (*p == '\r' || *p == '\n') {
            } else if (*p == '\t') {
                json += " ";
            } else {
                json += *p;
            }
            p++;
        }
        json += "\"}";
        sentLogs++;
    }
    json += "],";
    json += "\"connectedDevicesCount\":" + String(activeNetworkDevices) + ",";
    json += "\"connectedDevices\":[";
    int sentClients = 0;
    for (int i = 0; i < MAX_CONNECTED_CLIENTS; i++) {
        if (connectedClients[i].ip[0] != '\0' || connectedClients[i].mac[0] != '\0') {
            unsigned long ago = (millis() >= connectedClients[i].lastSeenMs) ? (millis() - connectedClients[i].lastSeenMs) / 1000UL : 0;
            if (sentClients > 0) json += ",";
            json += "{";
            json += "\"name\":\"" + String(connectedClients[i].name) + "\",";
            json += "\"ip\":\"" + String(connectedClients[i].ip) + "\",";
            json += "\"mac\":\"" + String(connectedClients[i].mac) + "\",";
            json += "\"type\":\"" + String(connectedClients[i].type) + "\",";
            json += "\"lastSeenAgo\":" + String(ago) + ",";
            json += "\"active\":" + String(connectedClients[i].active ? "true" : "false");
            json += "}";
            sentClients++;
        }
    }
    json += "],";
    json += "\"hardwareDevicesCount\":10,";
    json += "\"hardwareDevices\":[";
    json += "{\"name\":\"DHT11 Climate Sensor\",\"category\":\"Sensor\",\"pin\":\"GPIO 4\",\"status\":\"" + String(systemState.dhtFault ? "FAULT" : "ONLINE") + "\",\"reading\":\"" + String(systemState.temperature, 1) + " C, " + String(systemState.humidity, 1) + "%\"},";
    json += "{\"name\":\"MQ-2 Smoke & Gas Detector\",\"category\":\"Sensor\",\"pin\":\"GPIO 2 (ADC1_CH1)\",\"status\":\"" + String(systemState.mq2Fault ? "FAULT" : (systemState.smokeDetected ? "ALERT" : "ONLINE")) + "\",\"reading\":\"" + String(systemState.smokeLevel) + " ADC (" + String(systemState.smokeDetected ? "HAZARD" : "NORMAL") + ")\"},";
    json += "{\"name\":\"Optical LDR Light Sensor\",\"category\":\"Sensor\",\"pin\":\"GPIO 1 (ADC1_CH0)\",\"status\":\"" + String(systemState.ldrFault ? "FAULT" : "ONLINE") + "\",\"reading\":\"" + String(systemState.lightIntensity) + " ADC (" + String(systemState.lightOn ? "LIGHT ON" : "LIGHT OFF") + ")\"},";
    json += "{\"name\":\"Classroom Lighting Relay\",\"category\":\"Actuator\",\"pin\":\"GPIO 15\",\"status\":\"ONLINE\",\"reading\":\"" + String(systemState.lightOn ? "ON" : "OFF") + " (" + String(controlModeToString(systemState.lightMode)) + ")\"},";
    json += "{\"name\":\"Ventilation Fan Relay\",\"category\":\"Actuator\",\"pin\":\"GPIO 16\",\"status\":\"ONLINE\",\"reading\":\"" + String(systemState.fanOn ? "ON" : "OFF") + " (" + String(controlModeToString(systemState.fanMode)) + ")\"},";
    json += "{\"name\":\"Emergency Exhaust Safety Relay\",\"category\":\"Safety\",\"pin\":\"GPIO 38\",\"status\":\"ONLINE\",\"reading\":\"" + String(systemState.smokeDetected ? "TRIGGERED (ACTIVE)" : "STANDBY") + "\"},";
    json += "{\"name\":\"Motorized Window Servos (x2)\",\"category\":\"Actuator\",\"pin\":\"GPIO 10, 11 (PWM)\",\"status\":\"ONLINE\",\"reading\":\"" + String(systemState.windowOpen ? "OPEN (0 deg)" : "CLOSED (180 deg)") + "\"},";
    json += "{\"name\":\"Classroom Door Lock Servo\",\"category\":\"Actuator\",\"pin\":\"GPIO 7 (PWM)\",\"status\":\"ONLINE\",\"reading\":\"" + String(doorStateToString(systemState.doorState)) + "\"},";
    json += "{\"name\":\"PN532 RFID SPI Scanner\",\"category\":\"Access\",\"pin\":\"SPI (12, 13, 14, 20)\",\"status\":\"" + String(systemState.rfidAvailable ? "ONLINE" : "OFFLINE") + "\",\"reading\":\"" + (systemState.lastRfidName != "None" ? systemState.lastRfidName : "Ready for Cards") + "\"},";
    json += "{\"name\":\"SH1106 OLED Display (128x64)\",\"category\":\"Display\",\"pin\":\"I2C (39, 40)\",\"status\":\"" + String(systemState.oledAvailable ? "ONLINE" : "FAULT") + "\",\"reading\":\"" + (systemState.currentNotice.length() > 0 ? "Displaying Notice" : "Ready (0x3C)") + "\"}";
    json += "]";
    json += "}";
    return json;
}
#endif  
