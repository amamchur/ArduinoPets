#include <ARDK.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DS3231.h>

using namespace ARDK;
using namespace ARDK::IO;


typedef UI::Types<Adafruit_SSD1306> UIT;

const UIT::Label::Config labelCfg PROGMEM = {
  {.width = 13, .height = 17}
};

UIT::Label label(&labelCfg);

UIT::Component *cmps[] = {&label};
UIT::Layout uiLayout(cmps, sizeof(cmps) / sizeof(UIT::Component *));

void keyPressed(char, ButtonEvent);
const uint8_t keypadPins[] PROGMEM = {2, 3, 4, 5, 6, 7, 8, 9};

const char keys[4][4] PROGMEM = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '1', '#', 'D'}
};

KeyLayout<4, 4> layout(keys, keyPressed, 10, 200);
Keypad<4, 4> keypad(keypadPins);
Adafruit_SSD1306 display;
DS3231 rtc(SDA, SCL);

void setTime() {
  rtc.setDate(3, 9, 2016);
  rtc.setTime(14, 34, 00);
}

void setup() {
  Serial.begin(9600);
  keypad.begin();
  rtc.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  //setTime();
}

void loop() {
  keypad.scan(layout);
}

void drawTime(const Time &t) {
  char timeStr[] = "00:00:00";
  timeStr[0] += t.hour / 10;
  timeStr[1] += t.hour % 10;

  timeStr[3] += t.min / 10;
  timeStr[4] += t.min % 10;

  timeStr[6] += t.sec / 10;
  timeStr[7] += t.sec % 10;

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println(timeStr);
}

void drawDate(const Time &t) {
  char dateStr[] = "00.00.0000";
  dateStr[0] += t.date / 10;
  dateStr[1] += t.date % 10;

  dateStr[3] += t.mon / 10;
  dateStr[4] += t.mon % 10;

  dateStr[6] += t.year / 1000;
  dateStr[7] += (t.year / 100) % 10;
  dateStr[8] += (t.year / 10) % 10;
  dateStr[9] += t.year % 10;

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 30);
  display.println(dateStr);
}

void keyPressed(char ch, ButtonEvent event) {
  if (event != ButtonEventPress) {
    return;
  }

  switch (ch) {
    case '1':
      Serial.println(rtc.getTemp());
      break;
    case '2':
      Time t = rtc.getTime();
      display.clearDisplay();
      drawTime(t);
      drawDate(t);
      display.display();
      break;
  }

  Serial.println(ch);
}

