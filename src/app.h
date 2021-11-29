#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <ESP32Encoder.h>
#include <DS3231.h>
#include <SimpleButton.h>

#include "Arduino.h"
#include <SPI.h>
#include <Wire.h>

class App {
public:
  App();
  void Run();
private:
  void InitDisplay();
  void InitEncoder();
  void displayWelcomeScreen(uint32_t delayMs);
  void loop();
private:
  void printTime();
  void printHumidity();
  void printThreshold();
  void setWatering(bool enable);
  Adafruit_SSD1306 m_display;
  ESP32Encoder m_encoder;
  DS3231 m_rtc;
  simplebutton::Button m_btn;
  bool m_isBtnClick;
  bool m_isBtnHold;
  int m_humidity;
  int m_threshold;
  uint64_t m_watering;
  bool m_isWatering;
  uint64_t m_wateringDur;
};
