#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H
#include <Arduino.h>
#define RELAY_ACTIVE_LEVEL LOW
#define RELAY_INACTIVE_LEVEL HIGH
#define BUZZER_ACTIVE_LEVEL HIGH
#define BUZZER_INACTIVE_LEVEL LOW
#define SERVO_CLOSED 180
#define SERVO_OPEN   0
#define SERVO_DOOR_OPEN_ANGLE 0
#define SERVO_DOOR_LOCKED_ANGLE 180
#define SERVO_DOOR_CLOSED_ANGLE 180
#define SERVO_STEP_DELAY 20
#define DOOR_SERVO_STEP_MS 20
#define DOOR_OPEN_TIME 2000
#define RFID_COOLDOWN 2500
#ifndef OLED_SCREEN_WIDTH
#define OLED_SCREEN_WIDTH 128
#endif
#ifndef OLED_SCREEN_HEIGHT
#define OLED_SCREEN_HEIGHT 64
#endif
#ifndef OLED_I2C_ADDRESS
#define OLED_I2C_ADDRESS 0x3C
#endif
#ifndef OLED_RESET_PIN
#define OLED_RESET_PIN -1
#endif
#define DEFAULT_AP_SSID "S5CSEA"
#define DEFAULT_AP_PASSWORD ""
#define DEFAULT_MDNS_HOSTNAME "smartclassroom"
#define DEFAULT_HTTP_PORT 80
#define DEFAULT_CLASSROOM_NOTICE ""
struct AuthorizedUser {
    uint8_t uid[7];
    uint8_t uidLength;
    const char *name;
    bool present;
};
#ifndef AUTH_USERS_DEFINED
#define AUTH_USERS_DEFINED
static AuthorizedUser authorizedUsers[] = {
    {
        {0x25, 0xCD, 0xFA, 0xCA},
        4,
        "Rahul",
        false
    },
    {
        {0x35, 0xE5, 0xF7, 0xCA},
        4,
        "Laksmi",
        false
    }
};
#define AUTHORIZED_USER_COUNT \
    (sizeof(authorizedUsers) / sizeof(authorizedUsers[0]))
#endif
#define MAX_STUDENTS AUTHORIZED_USER_COUNT
typedef AuthorizedUser StudentRecord;
#define s_students authorizedUsers
#define MAX_ATTENDANCE_LOGS 20
struct AttendanceLogEntry {
    char name[16];
    bool present;
    unsigned long timestampMs;
};
#ifndef ATTENDANCE_LOG_DEFINED
#define ATTENDANCE_LOG_DEFINED
static AttendanceLogEntry s_attendanceLogs[MAX_ATTENDANCE_LOGS];
static int s_attendanceLogCount = 0;
static int s_attendanceLogHead = 0;
#endif
#endif 