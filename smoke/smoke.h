#ifndef SMOKE_H
#define SMOKE_H
#include "../config/device_config.h"
#include "../config/pins.h"
#include "../config/thresholds.h"
#include "../core/systemstate.h"
#include <Arduino.h>
#ifndef PIN_MQ2_AO
#define PIN_MQ2_AO 5
#endif
#ifndef PIN_MQ2_DO
#define PIN_MQ2_DO 6
#endif
#ifndef SMOKE_RELAY_PIN
#ifdef PIN_SMOKE_RELAY
#define SMOKE_RELAY_PIN PIN_SMOKE_RELAY
#else
#define SMOKE_RELAY_PIN 38
#endif
#endif
#ifndef SMOKE_RELAY_ON
#ifdef RELAY_ACTIVE_LEVEL
#define SMOKE_RELAY_ON  RELAY_ACTIVE_LEVEL    
#else
#define SMOKE_RELAY_ON  LOW
#endif
#endif
#ifndef SMOKE_RELAY_OFF
#ifdef RELAY_INACTIVE_LEVEL
#define SMOKE_RELAY_OFF RELAY_INACTIVE_LEVEL  
#else
#define SMOKE_RELAY_OFF HIGH
#endif
#endif
#define SMOKE_ADC_TRIGGER_THRESHOLD  1500
#ifndef SMOKE_DO_ACTIVE_LEVEL
#define SMOKE_DO_ACTIVE_LEVEL LOW
#endif
#ifndef SMOKE_READ_INTERVAL_MS
#define SMOKE_READ_INTERVAL_MS 250
#endif
#ifndef MQ2_STARTUP_WARMUP_MS
#define MQ2_STARTUP_WARMUP_MS 0UL
#endif
#define SMOKE_DEBOUNCE_SAMPLES 3
static int  s_smokeLevel          = 0;
static bool s_hasValidReading     = false;
static bool s_digitalDiagnostic   = false;  
static bool s_hazardState         = false;  
static bool s_smokeRelayState     = false;  
static int  s_lastDoState         = HIGH;
static unsigned long s_lastSmokeReadMs = 0;
static unsigned long s_lastLogMs       = 0;
static unsigned long s_smokeStartupMs  = 0;
static int s_consecutiveHazardSamples = 0;
static int s_consecutiveNormalSamples = 0;
inline void smokeRelayOn() {
  digitalWrite(SMOKE_RELAY_PIN, SMOKE_RELAY_ON);
  s_smokeRelayState = true;
  systemState.smokeDetected = true;
  Serial.printf("[MQ2] Hazard: YES -> Relay: TRIGGERED / ON (GPIO %d)\n", SMOKE_RELAY_PIN);
  addSystemLogf("smoke", "[MQ2] HAZARD ALERT: ADC %d >= 1500 -> Relay TRIGGERED ON (GPIO %d)", s_smokeLevel, SMOKE_RELAY_PIN);
}
inline void smokeRelayOff() {
  digitalWrite(SMOKE_RELAY_PIN, SMOKE_RELAY_OFF);
  s_smokeRelayState = false;
  systemState.smokeDetected = false;
  Serial.printf("[MQ2] Hazard: NO -> Relay: IDLE / OFF (GPIO %d)\n", SMOKE_RELAY_PIN);
  addSystemLogf("smoke", "[MQ2] Air Safe: ADC %d < 1500 -> Relay IDLE OFF (GPIO %d)", s_smokeLevel, SMOKE_RELAY_PIN);
}
inline void buzzerRelayOn()  { smokeRelayOn(); }
inline void buzzerRelayOff() { smokeRelayOff(); }
inline void applySmokeRelay(bool state) {
  if (state) smokeRelayOn();
  else smokeRelayOff();
}
inline void applyBuzzerRelay(bool state) { applySmokeRelay(state); }
inline void smokeInit() {
  Serial.println();
  Serial.println(F("=============================================="));
  Serial.println(F("[SMOKE] INITIALIZING MQ-2 & RELAY SUBSYSTEM"));
  Serial.println(F("=============================================="));
  if (SMOKE_RELAY_OFF == HIGH) {
    if (SMOKE_RELAY_PIN >= 32) {
      GPIO.out1_w1ts.val = (1UL << (SMOKE_RELAY_PIN - 32));
    } else {
      GPIO.out_w1ts = (1UL << SMOKE_RELAY_PIN);
    }
  } else {
    if (SMOKE_RELAY_PIN >= 32) {
      GPIO.out1_w1tc.val = (1UL << (SMOKE_RELAY_PIN - 32));
    } else {
      GPIO.out_w1tc = (1UL << SMOKE_RELAY_PIN);
    }
  }
  pinMode(SMOKE_RELAY_PIN, OUTPUT_OPEN_DRAIN);
  digitalWrite(SMOKE_RELAY_PIN, SMOKE_RELAY_OFF);
  s_smokeRelayState = false;
  s_hazardState     = false;
  systemState.smokeDetected = false;
  Serial.printf("[SMOKE] GPIO %d configured as OUTPUT_OPEN_DRAIN (Idle OFF) during startup.\n", SMOKE_RELAY_PIN);
  pinMode(PIN_MQ2_AO, INPUT);
  pinMode(PIN_MQ2_DO, INPUT_PULLUP);
  s_digitalDiagnostic = false;
  s_consecutiveHazardSamples = 0;
  s_consecutiveNormalSamples = 0;
  systemState.mq2Fault = false;
  long sum = 0;
  for (int i = 0; i < 4; i++) {
    sum += analogRead(PIN_MQ2_AO);
    delayMicroseconds(50);
  }
  int initialAdc = (int)(sum / 4);
  if (initialAdc >= 0 && initialAdc <= 4095) {
    s_smokeLevel = initialAdc;
    s_hasValidReading = true;
  } else {
    s_smokeLevel = 0;
    s_hasValidReading = false;
    systemState.mq2Fault = true;
  }
  systemState.smokeLevel = s_smokeLevel;
  s_lastDoState = digitalRead(PIN_MQ2_DO);
  s_digitalDiagnostic = (s_lastDoState == SMOKE_DO_ACTIVE_LEVEL);
  s_smokeStartupMs  = millis();
  s_lastSmokeReadMs = millis();
  s_lastLogMs       = millis();
  Serial.printf("[SMOKE] Warm-up period started: %lu seconds\n", MQ2_STARTUP_WARMUP_MS / 1000UL);
  Serial.println(F("=============================================="));
}
inline void smokeUpdate() {
  unsigned long now = millis();
  if (now - s_lastSmokeReadMs < SMOKE_READ_INTERVAL_MS) {
    return;
  }
  s_lastSmokeReadMs = now;
  long sum = 0;
  for (int i = 0; i < 4; i++) {
    sum += analogRead(PIN_MQ2_AO);
    delayMicroseconds(50);
  }
  int currentAdc = (int)(sum / 4);
  if (currentAdc >= 0 && currentAdc <= 4095) {
    s_smokeLevel = currentAdc;
    s_hasValidReading = true;
    systemState.smokeLevel = s_smokeLevel;
    systemState.mq2Fault = false;
  } else {
    systemState.mq2Fault = true;
  }
  s_lastDoState = digitalRead(PIN_MQ2_DO);
  s_digitalDiagnostic = (s_lastDoState == SMOKE_DO_ACTIVE_LEVEL);
  if (MQ2_STARTUP_WARMUP_MS > 0) {
    unsigned long warmupElapsed = now - s_smokeStartupMs;
    if (warmupElapsed < MQ2_STARTUP_WARMUP_MS) {
      digitalWrite(SMOKE_RELAY_PIN, SMOKE_RELAY_OFF);
      s_smokeRelayState = false;
      s_hazardState     = false;
      systemState.smokeDetected = false;
      if (now - s_lastLogMs >= 1000) {
        s_lastLogMs = now;
        Serial.printf("[MQ2] WARM-UP | ADC: %d | DO: %d | Diagnostic: %s | Hazard: NO | Relay: OFF | GPIO %d | Remaining: %lus\n",
                      s_smokeLevel,
                      s_lastDoState,
                      s_digitalDiagnostic ? "ACTIVE" : "NORMAL",
                      SMOKE_RELAY_PIN,
                      (MQ2_STARTUP_WARMUP_MS - warmupElapsed) / 1000UL);
      }
      return;
    }
  }
  if (s_smokeLevel >= SMOKE_ADC_TRIGGER_THRESHOLD) {
    s_hazardState = true;
  } else {
    s_hazardState = false;
  }
  if (s_hazardState != s_smokeRelayState) {
    if (s_hazardState) {
      smokeRelayOn();   
    } else {
      smokeRelayOff();  
    }
  }
  if (now - s_lastLogMs >= 1000) {
    s_lastLogMs = now;
    Serial.printf("[MQ2] ADC: %d | DO: %d | Diagnostic: %s | Hazard: %s | Relay: %s (GPIO %d)\n",
                  s_smokeLevel,
                  s_lastDoState,
                  s_digitalDiagnostic ? "ACTIVE" : "NORMAL",
                  s_hazardState ? "YES" : "NO",
                  s_smokeRelayState ? "TRIGGERED (ON)" : "IDLE (OFF)",
                  SMOKE_RELAY_PIN);
  }
}
inline int  getSmokeLevel()        { return s_smokeLevel; }
inline bool isSmokeDetected()      { return s_hazardState; }
inline bool isAnalogHazard()       { return s_hazardState; }
inline bool isDigitalHazard()      { return s_digitalDiagnostic; }
inline int  getSmokeDoState()      { return s_lastDoState; }
inline bool isSmokeRelayOn()       { return s_smokeRelayState; }
inline bool isBuzzerRelayOn()      { return s_smokeRelayState; }
inline bool isSmokeValid()         { return s_hasValidReading; }
inline unsigned long smokeStartupRemainingSeconds() {
  unsigned long now = millis();
  unsigned long elapsed = now - s_smokeStartupMs;
  if (elapsed >= MQ2_STARTUP_WARMUP_MS) return 0;
  return (MQ2_STARTUP_WARMUP_MS - elapsed) / 1000UL;
}
inline bool smokeWarmupComplete() {
  return (millis() - s_smokeStartupMs) >= MQ2_STARTUP_WARMUP_MS;
}
inline void buzzerBeep(int durationMs) { (void)durationMs; }
inline void buzzerPattern(int beeps, int beepDurationMs, int pauseMs) {
  (void)beeps; (void)beepDurationMs; (void)pauseMs;
}
#endif 