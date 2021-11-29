#include "app.h"

static constexpr uint32_t welcomeScreenDelay(1000);

static constexpr uint8_t SCREEN_WIDTH(128);
static constexpr uint8_t SCREEN_HEIGHT(64);
static constexpr uint8_t SCREEN_ADDRESS(0x3c);


App::App()
  : m_display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire)
  , m_btn(25, true)
  , m_threshold(1700)
  , m_watering(millis())
  , m_isWatering(false)
  , m_wateringDur(2000)
{ }

void App::Run() {
  Wire.begin();
  m_rtc.setClockMode(false);
  pinMode(32, INPUT);
  pinMode(5, OUTPUT);
  pinMode(18, OUTPUT);
  digitalWrite(18, false);

  m_btn.setOnClicked([this]() {
      this->m_isBtnClick = true;
  });

  m_btn.setOnHolding([this]() {
      this->m_isBtnHold = true;
  });

  InitDisplay();
  InitEncoder();
  displayWelcomeScreen(welcomeScreenDelay);
  while(true) {
    m_btn.update();
    m_humidity = analogRead(32);
    m_threshold = m_encoder.getCount();
    loop();
  }
}

void App::loop() {

  m_display.clearDisplay();
  m_display.setTextColor(SSD1306_WHITE);
  printTime();
  printHumidity();
  printThreshold();
  m_display.display();

  if (m_humidity > m_threshold && !m_isWatering && m_watering + 10000 < millis()) {
    setWatering(true);
    Serial.println("Start watering");
    m_isWatering = true;
    m_watering = millis();
  }
  if (m_isWatering && m_watering + m_wateringDur < millis()) {
    setWatering(false);
    Serial.println("Finish watering");
    m_isWatering = false;
    m_watering = millis();
  }

  // if (water < 1700) {
  //   digitalWrite(5, true);
  //   delay(1000);
  // } else {
  //   digitalWrite(5, false);
  // }
  // delay(100); 
}

void App::InitDisplay() {
  Serial.println("Init display");
  if (!m_display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
}

void App::InitEncoder() {
  Serial.println("Init encoder");
  ESP32Encoder::useInternalWeakPullResistors = UP;
  pinMode(32, OUTPUT);
  digitalWrite(32, HIGH);
  m_encoder.attachSingleEdge(26, 25);
  m_encoder.setCount(m_threshold);
}

void App::displayWelcomeScreen(uint32_t delayMs) {
  m_display.clearDisplay();
  m_display.setTextSize(2);
  m_display.setTextColor(SSD1306_WHITE);

  m_display.setCursor(20, 24);
  m_display.println(F("Quadcode"));
  m_display.display();
  delay(delayMs);
}
void App::printThreshold() {
  m_display.drawRect(0, 42, 60, 22, WHITE);
  m_display.drawLine(3, 42, 56, 42, BLACK);
  m_display.setCursor(3, 38);
  m_display.setTextSize(1);
  m_display.println("threshold");
  m_display.setTextSize(2);
  m_display.setTextColor(SSD1306_WHITE);
  m_display.setCursor(6, 48);
  m_display.println(m_threshold);
}

void App::printHumidity() {
  m_display.drawRect(0, 13, 60, 23, WHITE);
  m_display.drawLine(4, 13, 55, 13, BLACK);
  m_display.setCursor(6, 10);
  m_display.setTextSize(1);
  m_display.println("humidity");
  m_display.setTextSize(2);
  m_display.setTextColor(SSD1306_WHITE);
  m_display.setCursor(6, 20);
  m_display.println(m_humidity);
}

void App::printTime() {
    m_display.setCursor(5, 0);
    m_display.setTextSize(1);
    bool century;
    bool h12;
    bool PM_time;
    m_rtc.setClockMode(false);
    m_display.print(2000+m_rtc.getYear(), DEC);
    m_display.print('/');
    auto month = m_rtc.getMonth(century);
    if (month < 10) {
      m_display.print(0);
    }
    m_display.print(m_rtc.getMonth(century), DEC);
    m_display.print('/');
    auto day = m_rtc.getDate();
    if (day < 10) {
      m_display.print(0);
    }
    m_display.print(day, DEC);
    m_display.print(' ');
    auto hour = m_rtc.getHour(h12, PM_time);
    if (hour < 10) {
      m_display.print(0);
    }
    m_display.print(hour, DEC);
    m_display.print(':');
    auto minute = m_rtc.getMinute();
    if (minute < 10) {
      m_display.print(0);
    }
    m_display.print(minute, DEC);
    m_display.print(':');
    auto second = m_rtc.getSecond();
    if (second < 10) {
      m_display.print(0);
    }
    m_display.print(second, DEC);
    m_display.drawLine(0, 8, 128, 8, WHITE);
}

void App::setWatering(bool enable) {
  digitalWrite(5, enable);
}