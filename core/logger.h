#ifndef LOGGER_H
#define LOGGER_H
#include <Arduino.h>
#include <stdarg.h>
#define MAX_SYSTEM_LOGS 80
#define LOG_MSG_MAX_LEN 128
struct SystemLogEntry {
    uint32_t ts;                
    char cat[12];               
    char msg[LOG_MSG_MAX_LEN];  
};
#ifndef LOGGER_DEFINED
#define LOGGER_DEFINED
static SystemLogEntry s_systemLogs[MAX_SYSTEM_LOGS];
static int s_systemLogHead = 0;
static int s_systemLogCount = 0;
static uint32_t s_logSequence = 0;
#endif
inline void addSystemLog(const char* category, const char* message) {
    if (!category || !message) return;
    SystemLogEntry& entry = s_systemLogs[s_systemLogHead];
    entry.ts = millis();
    strncpy(entry.cat, category, sizeof(entry.cat) - 1);
    entry.cat[sizeof(entry.cat) - 1] = '\0';
    size_t len = strlen(message);
    while (len > 0 && (message[len - 1] == '\r' || message[len - 1] == '\n' || message[len - 1] == ' ')) {
        len--;
    }
    if (len >= sizeof(entry.msg)) {
        len = sizeof(entry.msg) - 1;
    }
    strncpy(entry.msg, message, len);
    entry.msg[len] = '\0';
    s_systemLogHead = (s_systemLogHead + 1) % MAX_SYSTEM_LOGS;
    if (s_systemLogCount < MAX_SYSTEM_LOGS) {
        s_systemLogCount++;
    }
    s_logSequence++;
}
inline void addSystemLogf(const char* category, const char* fmt, ...) {
    char buf[LOG_MSG_MAX_LEN];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    addSystemLog(category, buf);
}
inline String systemLogsToJson(const char* filterCat = nullptr, int limit = 60) {
    String json;
    json.reserve(3072);
    json = "{\"sequence\":";
    json += String(s_logSequence);
    json += ",\"logs\":[";
    int count = (s_systemLogCount < MAX_SYSTEM_LOGS) ? s_systemLogCount : MAX_SYSTEM_LOGS;
    int written = 0;
    for (int i = 0; i < count; i++) {
        int idx;
        if (s_systemLogCount <= MAX_SYSTEM_LOGS) {
            idx = i;
        } else {
            idx = (s_systemLogHead + i) % MAX_SYSTEM_LOGS;
        }
        if (filterCat && strlen(filterCat) > 0 && strcmp(filterCat, "all") != 0) {
            if (strcasecmp(s_systemLogs[idx].cat, filterCat) != 0) {
                continue;
            }
        }
        if (written > 0) json += ",";
        json += "{\"ts\":";
        json += String(s_systemLogs[idx].ts);
        json += ",\"cat\":\"";
        json += s_systemLogs[idx].cat;
        json += "\",\"msg\":\"";
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
        written++;
        if (limit > 0 && written >= limit) {
            break;
        }
    }
    json += "]}";
    return json;
}
inline void clearSystemLogs() {
    s_systemLogHead = 0;
    s_systemLogCount = 0;
    s_logSequence++;
    addSystemLog("system", "[LOGS] System log buffer cleared");
}
#endif  
