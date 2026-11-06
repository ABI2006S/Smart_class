#ifndef LDR_LIGHT_H
#define LDR_LIGHT_H
#include "../config/device_config.h"
#include "../config/pins.h"
#include "../config/thresholds.h"
#include "../core/systemstate.h"
#include <Arduino.h>
#define LDR_PIN 1
#define LIGHT_RELAY_PIN 15
#define LIGHT_RELAY_ON LOW
#define LIGHT_RELAY_OFF HIGH
#define LDR_ADC_MIN 0
#define LDR_ADC_MAX 4095
static const unsigned long LDR_READ_INTERVAL = LDR_READ_INTERVAL_MS;
enum LightControlMode {
  LIGHT_AUTO,
  LIGHT_MANUAL
};
static int ldrRawValue = 0;
static int lightIntensity = 0;
static bool lightState = false;
static LightControlMode lightMode = LIGHT_AUTO;
static unsigned long lastLdrReadTime = 0;
inline void lightRelayOn();
inline void lightRelayOff();
inline void setLightAuto();
inline void setLightManual(bool state);
inline void evaluateLightAuto();
inline void lightRelayOn() {
  digitalWrite(LIGHT_RELAY_PIN, LIGHT_RELAY_ON);
  lightState = true;
  systemState.lightOn = true;
  Serial.println("[LIGHT] Relay -> ON");
  addSystemLog("light", "[LIGHT] Classroom Lighting Relay -> ON (GPIO 15)");
}
inline void lightRelayOff() {
  digitalWrite(LIGHT_RELAY_PIN, LIGHT_RELAY_OFF);
  lightState = false;
  systemState.lightOn = false;
  Serial.println("[LIGHT] Relay -> OFF");
  addSystemLog("light", "[LIGHT] Classroom Lighting Relay -> OFF (GPIO 15)");
}
inline void evaluateLightAuto() {
  bool requestedLightState = lightState;
  if (ldrRawValue < (LIGHT_THRESHOLD_DEFAULT - LIGHT_HYSTERESIS)) {
    requestedLightState = false;
  }
  else if (ldrRawValue > (LIGHT_THRESHOLD_DEFAULT + LIGHT_HYSTERESIS)) {
    requestedLightState = true;
  }
  else {
    requestedLightState = lightState;
  }
  if (requestedLightState != lightState) {
    if (requestedLightState) {
      lightRelayOn();
      Serial.printf("[LIGHT] AUTO -> ON | DARK | LDR=%d | ON_LIMIT=%d\n",
                    ldrRawValue, LIGHT_THRESHOLD_DEFAULT + LIGHT_HYSTERESIS);
    }
    else {
      lightRelayOff();
      Serial.printf("[LIGHT] AUTO -> OFF | BRIGHT | LDR=%d | OFF_LIMIT=%d\n",
                    ldrRawValue, LIGHT_THRESHOLD_DEFAULT - LIGHT_HYSTERESIS);
    }
  }
}
inline void ldrLightInit() {
  Serial.println("[LDR] Initializing LDR + classroom light module...");
  pinMode(LDR_PIN, INPUT);
  pinMode(LIGHT_RELAY_PIN, OUTPUT_OPEN_DRAIN);
  digitalWrite(LIGHT_RELAY_PIN, LIGHT_RELAY_OFF);
  lightState = false;
  lightMode = LIGHT_AUTO;
  lastLdrReadTime = millis();
  ldrRawValue = analogRead(LDR_PIN);
  lightIntensity = map(ldrRawValue, LDR_ADC_MIN, LDR_ADC_MAX, 0, 100);
  lightIntensity = constrain(lightIntensity, 0, 100);
  systemState.lightIntensity = lightIntensity;
  systemState.lightOn = false;
  systemState.lightMode = MODE_AUTO;
  systemState.ldrFault = false;
  Serial.println("[LDR] Initialization complete");
  Serial.printf("[LDR] Initial ADC: %d | Intensity: %d%%\n", ldrRawValue,
                lightIntensity);
  Serial.println("[LIGHT] Mode: AUTO | Relay: OFF");
}
inline void ldrLightUpdate() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastLdrReadTime < LDR_READ_INTERVAL) {
    return;
  }
  lastLdrReadTime = currentMillis;
  ldrRawValue = analogRead(LDR_PIN);
  lightIntensity = map(ldrRawValue, LDR_ADC_MIN, LDR_ADC_MAX, 0, 100);
  lightIntensity = constrain(lightIntensity, 0, 100);
  systemState.lightIntensity = lightIntensity;
  systemState.ldrFault = false;
  if (lightMode == LIGHT_AUTO) {
    evaluateLightAuto();
  }
  Serial.printf("[LDR] ADC=%d | Intensity=%d%% | Mode=%s | Light=%s\n",
                ldrRawValue,
                lightIntensity,
                (lightMode == LIGHT_AUTO) ? "AUTO" : "MANUAL",
                lightState ? "ON" : "OFF");
}
inline int getLdrRawValue() { return ldrRawValue; }
inline int getLightIntensity() { return lightIntensity; }
inline bool isLightOn() { return lightState; }
inline bool getLightState() { return lightState; }
inline bool isLightAuto() { return lightMode == LIGHT_AUTO; }
inline ControlMode getLightMode() {
  if (lightMode == LIGHT_AUTO) {
    return MODE_AUTO;
  }
  if (lightState) {
    return MODE_MANUAL_ON;
  }
  return MODE_MANUAL_OFF;
}
inline void setLightManual(bool state) {
  lightMode = LIGHT_MANUAL;
  if (state) {
    systemState.lightMode = MODE_MANUAL_ON;
    lightRelayOn();
  }
  else {
    systemState.lightMode = MODE_MANUAL_OFF;
    lightRelayOff();
  }
  Serial.printf("[LIGHT] MANUAL -> %s\n", state ? "ON" : "OFF");
}
inline void setLightAuto() {
  lightMode = LIGHT_AUTO;
  systemState.lightMode = MODE_AUTO;
  Serial.println("[LIGHT] Mode -> AUTO");
  ldrRawValue = analogRead(LDR_PIN);
  lightIntensity = map(ldrRawValue, LDR_ADC_MIN, LDR_ADC_MAX, 0, 100);
  lightIntensity = constrain(lightIntensity, 0, 100);
  systemState.lightIntensity = lightIntensity;
  evaluateLightAuto();
}
inline void setLightMode(bool autoMode) {
  if (autoMode) {
    setLightAuto();
  }
  else {
    lightMode = LIGHT_MANUAL;
    systemState.lightMode = lightState ? MODE_MANUAL_ON : MODE_MANUAL_OFF;
    Serial.println("[LIGHT] Mode -> MANUAL");
  }
}
inline void setLightMode(ControlMode mode) {
  if (mode == MODE_MANUAL_ON) {
    setLightManual(true);
  }
  else if (mode == MODE_MANUAL_OFF) {
    setLightManual(false);
  }
  else {
    setLightAuto();
  }
}
#endif 