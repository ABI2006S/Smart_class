#ifndef OLEDDISPLAYIOT_H
#define OLEDDISPLAYIOT_H
#include "../config/device_config.h"
#include "../config/pins.h"
#include "../config/thresholds.h"
#include "../core/systemstate.h"
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
static U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);
static bool s_oledReady = false;
static inline void renderOledMessage() {
  if (!s_oledReady) {
    return;
  }
  oled.clearBuffer();
  String message = systemState.currentNotice;
  message.replace("\r", "");
  message.trim();
  if (message.length() == 0) {
    message = "Notice Board ...";
  }
  const int DISPLAY_WIDTH = 128;
  const int DISPLAY_HEIGHT = 64;
  const int SIDE_MARGIN = 4;
  const int MAX_WIDTH = DISPLAY_WIDTH - (SIDE_MARGIN * 2);
  const uint8_t *selectedFont = nullptr;
  if (message.length() <= 12) {
    selectedFont = u8g2_font_ncenB14_tr;
  }
  else if (message.length() <= 22) {
    selectedFont = u8g2_font_ncenB12_tr;
  }
  else if (message.length() <= 45) {
    selectedFont = u8g2_font_ncenB10_tr;
  }
  else {
    selectedFont = u8g2_font_ncenB08_tr;
  }
  oled.setFont(selectedFont);
  const int MAX_LINES = 5;
  String lines[MAX_LINES];
  int lineCount = 0;
  int startIndex = 0;
  int messageLength = message.length();
  while (startIndex < messageLength && lineCount < MAX_LINES) {
    int newlineIndex = message.indexOf('\n', startIndex);
    int paragraphEnd = (newlineIndex == -1) ? messageLength : newlineIndex;
    String paragraph = message.substring(startIndex, paragraphEnd);
    paragraph.trim();
    startIndex = paragraphEnd + 1;
    if (paragraph.length() == 0) {
      if (lineCount < MAX_LINES) {
        lines[lineCount++] = "";
      }
      continue;
    }
    String currentLine = "";
    int wordStart = 0;
    int paragraphLength = paragraph.length();
    while (wordStart < paragraphLength && lineCount < MAX_LINES) {
      while (wordStart < paragraphLength &&
             paragraph.charAt(wordStart) == ' ') {
        wordStart++;
      }
      if (wordStart >= paragraphLength) {
        break;
      }
      int wordEnd = paragraph.indexOf(' ', wordStart);
      if (wordEnd == -1) {
        wordEnd = paragraphLength;
      }
      String word = paragraph.substring(wordStart, wordEnd);
      wordStart = wordEnd;
      if (oled.getStrWidth(word.c_str()) > MAX_WIDTH) {
        if (currentLine.length() > 0) {
          lines[lineCount++] = currentLine;
          currentLine = "";
          if (lineCount >= MAX_LINES) {
            break;
          }
        }
        for (unsigned int i = 0; i < word.length() && lineCount < MAX_LINES;
             i++) {
          String testLine = currentLine + word.charAt(i);
          if (oled.getStrWidth(testLine.c_str()) <= MAX_WIDTH) {
            currentLine = testLine;
          } else {
            if (currentLine.length() > 0) {
              lines[lineCount++] = currentLine;
            }
            currentLine = String(word.charAt(i));
          }
        }
        continue;
      }
      String testLine;
      if (currentLine.length() == 0) {
        testLine = word;
      } else {
        testLine = currentLine + " " + word;
      }
      if (oled.getStrWidth(testLine.c_str()) <= MAX_WIDTH) {
        currentLine = testLine;
      }
      else {
        if (currentLine.length() > 0) {
          lines[lineCount++] = currentLine;
          if (lineCount >= MAX_LINES) {
            break;
          }
        }
        currentLine = word;
      }
    }
    if (currentLine.length() > 0 && lineCount < MAX_LINES) {
      lines[lineCount++] = currentLine;
    }
  }
  if (lineCount == 0) {
    lines[0] = message;
    lineCount = 1;
  }
  int lineHeight = oled.getMaxCharHeight();
  lineHeight += 2;
  if (lineHeight > 18) {
    lineHeight = 18;
  }
  int totalTextHeight = lineCount * lineHeight;
  int firstBaseline = ((DISPLAY_HEIGHT - totalTextHeight) / 2) + lineHeight;
  if (firstBaseline < lineHeight) {
    firstBaseline = lineHeight;
  }
  for (int i = 0; i < lineCount; i++) {
    if (lines[i].length() == 0) {
      continue;
    }
    int textWidth = oled.getStrWidth(lines[i].c_str());
    int x = (DISPLAY_WIDTH - textWidth) / 2;
    if (x < SIDE_MARGIN) {
      x = SIDE_MARGIN;
    }
    if (x + textWidth > DISPLAY_WIDTH - SIDE_MARGIN) {
      x = DISPLAY_WIDTH - SIDE_MARGIN - textWidth;
    }
    int y = firstBaseline + (i * lineHeight);
    oled.drawStr(x, y, lines[i].c_str());
  }
  oled.sendBuffer();
}
inline void oledInit() {
  Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
  if (!oled.begin()) {
    s_oledReady = false;
    systemState.oledAvailable = false;
    Serial.println("[OLED] Warning: SH1106 OLED initialization failed");
    return;
  }
  s_oledReady = true;
  systemState.oledAvailable = true;
  Serial.println("[OLED] SH1106 initialized");
  Serial.printf("[OLED] SDA: %d\n", PIN_OLED_SDA);
  Serial.printf("[OLED] SCL: %d\n", PIN_OLED_SCL);
  oled.clearBuffer();
  oled.setFont(u8g2_font_ncenB12_tr);
  const char *title = "S5CSE A";
  int titleWidth = oled.getStrWidth(title);
  int titleX = (128 - titleWidth) / 2;
  oled.drawStr(titleX, 20, title);
  oled.setFont(u8g2_font_ncenB08_tr);
  const char *classroom = "SMART CLASSROOM";
  int classroomWidth = oled.getStrWidth(classroom);
  int classroomX = (128 - classroomWidth) / 2;
  oled.drawStr(classroomX, 40, classroom);
  const char *iot = "IoT DISPLAY";
  int iotWidth = oled.getStrWidth(iot);
  int iotX = (128 - iotWidth) / 2;
  oled.drawStr(iotX, 56, iot);
  oled.sendBuffer();
  delay(1200);
  renderOledMessage();
}
inline void oledUpdate() {
}
inline void oledShowMessage(String message) {
  systemState.currentNotice = message;
  Serial.printf("[OLED] IoT message updated: \"%s\"\n", message.c_str());
  if (s_oledReady) {
    renderOledMessage();
  }
}
inline String getCurrentNotice() { return systemState.currentNotice; }
inline bool oledIsReady() { return s_oledReady; }
#endif 