#ifndef DHT_FAN_H
#define DHT_FAN_H
#include "../config/device_config.h"
#include "../config/pins.h"
#include "../config/thresholds.h"
#include "../core/systemstate.h"
#include <Arduino.h>
#include <DHT.h>
#define FAN_RELAY_PIN PIN_FAN_RELAY
#define FAN_RELAY_ON LOW
#define FAN_RELAY_OFF HIGH
#ifndef DHT_READ_INTERVAL_MS
#define DHT_READ_INTERVAL_MS 2200UL
#endif
#ifndef DHT_STARTUP_DELAY_MS
#define DHT_STARTUP_DELAY_MS 2500UL
#endif
#define FAN_TEMP_ON_THRESHOLD 40.0f
#define FAN_TEMP_OFF_THRESHOLD 38.0f
static DHT s_dht(PIN_DHT11_DATA, DHT11);
static float s_temperature = NAN;
static float s_humidity = NAN;
static bool s_dhtHasValidReading = false;
static bool s_fanState = false;
static ControlMode s_fanMode = MODE_AUTO;
static unsigned long s_startupMs = 0;
static unsigned long s_lastDhtReadMs = 0;
static uint8_t s_consecutiveFailures = 0;
inline void fanRelayOn() {
  digitalWrite(FAN_RELAY_PIN, FAN_RELAY_ON);
  s_fanState = true;
  systemState.fanOn = true;
  Serial.println("[FAN] Relay -> ON");
  addSystemLog("climate", "[FAN] Ventilation Fan Relay -> ON (GPIO 16)");
}
inline void fanRelayOff() {
  digitalWrite(FAN_RELAY_PIN, FAN_RELAY_OFF);
  s_fanState = false;
  systemState.fanOn = false;
  Serial.println("[FAN] Relay -> OFF");
  addSystemLog("climate", "[FAN] Ventilation Fan Relay -> OFF (GPIO 16)");
}
inline void applyFanRelay(bool state) {
  if (state) {
    fanRelayOn();
  } else {
    fanRelayOff();
  }
}
inline void evaluateFanAuto() {
  if (!s_dhtHasValidReading) {
    return;
  }
  if (!s_fanState) {
    if (s_temperature >= FAN_TEMP_ON_THRESHOLD) {
      fanRelayOn();
      Serial.printf("[FAN] AUTO -> ON | Temperature: %.1f C >= %.1f C\n",
                    s_temperature, FAN_TEMP_ON_THRESHOLD);
    }
  }
  else {
    if (s_temperature <= FAN_TEMP_OFF_THRESHOLD) {
      fanRelayOff();
      Serial.printf("[FAN] AUTO -> OFF | Temperature: %.1f C <= %.1f C\n",
                    s_temperature, FAN_TEMP_OFF_THRESHOLD);
    }
  }
}
inline void dhtFanInit() {
  Serial.println("[DHT11] Initializing DHT11 + Fan subsystem...");
  pinMode(FAN_RELAY_PIN, OUTPUT_OPEN_DRAIN);
  digitalWrite(FAN_RELAY_PIN, FAN_RELAY_OFF);
  s_fanState = false;
  systemState.fanOn = false;
  Serial.println("[FAN] GPIO 16 configured as OPEN-DRAIN");
  Serial.println("[FAN] Startup state: OFF");
  pinMode(PIN_DHT11_DATA, INPUT_PULLUP);
  s_dht.begin();
  s_temperature = NAN;
  s_humidity = NAN;
  s_dhtHasValidReading = false;
  s_consecutiveFailures = 0;
  s_fanMode = MODE_AUTO;
  systemState.fanMode = MODE_AUTO;
  systemState.dhtFault = false;
  s_startupMs = millis();
  s_lastDhtReadMs = s_startupMs;
  Serial.println("[DHT11] Sensor initialized on GPIO 4");
  Serial.println("[DHT11] Waiting for first REAL sensor reading...");
  Serial.println("[FAN] AUTO thresholds: ON >= 40 C | OFF <= 38 C");
}
inline void dhtFanUpdate() {
  unsigned long now = millis();
  if ((now - s_startupMs) < DHT_STARTUP_DELAY_MS) {
    return;
  }
  if ((now - s_lastDhtReadMs) < DHT_READ_INTERVAL_MS) {
    return;
  }
  s_lastDhtReadMs = now;
  float humidity = s_dht.readHumidity();
  float temperature = s_dht.readTemperature();
  bool validReading = !isnan(temperature) && !isnan(humidity) &&
                      temperature >= 0.0f && temperature <= 60.0f &&
                      humidity >= 0.0f && humidity <= 100.0f;
  if (validReading) {
    s_temperature = temperature;
    s_humidity = humidity;
    s_dhtHasValidReading = true;
    s_consecutiveFailures = 0;
    systemState.temperature = s_temperature;
    systemState.humidity = s_humidity;
    systemState.dhtFault = false;
    Serial.printf("[DHT11] Temperature = %.1f C | Humidity = %.1f %%\n",
                  s_temperature, s_humidity);
    if (s_fanMode == MODE_AUTO) {
      evaluateFanAuto();
    }
  }
  else {
    if (s_consecutiveFailures < 255) {
      s_consecutiveFailures++;
    }
    if (s_consecutiveFailures >= 3) {
      systemState.dhtFault = true;
      Serial.printf("[DHT11] Read failed. "
                    "No new valid sensor value. "
                    "Failures: %u\n",
                    s_consecutiveFailures);
    } else {
      Serial.printf("[DHT11] Read failed (%u/3). "
                    "Waiting for real DHT reading...\n",
                    s_consecutiveFailures);
    }
  }
}
inline float getTemperature() { return s_temperature; }
inline float getHumidity() { return s_humidity; }
inline bool isDhtReadingValid() { return s_dhtHasValidReading; }
inline bool isDhtFault() { return systemState.dhtFault; }
inline bool getFanState() { return s_fanState; }
inline bool isFanOn() { return s_fanState; }
inline bool isFanAuto() { return s_fanMode == MODE_AUTO; }
inline ControlMode getFanMode() { return s_fanMode; }
inline void setFanMode(ControlMode mode) {
  s_fanMode = mode;
  systemState.fanMode = mode;
  if (mode == MODE_MANUAL_ON) {
    fanRelayOn();
    Serial.println("[FAN] Mode -> MANUAL ON");
  }
  else if (mode == MODE_MANUAL_OFF) {
    fanRelayOff();
    Serial.println("[FAN] Mode -> MANUAL OFF");
  }
  else {
    Serial.println("[FAN] Mode -> AUTO");
    if (s_dhtHasValidReading) {
      evaluateFanAuto();
    }
  }
}
inline void setFanManual(bool state) {
  if (state) {
    setFanMode(MODE_MANUAL_ON);
  } else {
    setFanMode(MODE_MANUAL_OFF);
  }
}
inline void setFanAuto() { setFanMode(MODE_AUTO); }
#endif 