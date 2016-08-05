#include <Arduino.h>
#include <DS3231.h>
#include <Adafruit_NeoPixel.h>
#include "LedControl.h"

#define NUMPIXELS_PIN 6
#define NUMPIXELS     12

LedControl lc = LedControl(10, 12, 11, 1);

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NUMPIXELS_PIN, NEO_GRB + NEO_KHZ800);
DS3231 rtc(SDA, SCL);
boolean displayMode = true;

void setTime() {
  rtc.setDate(9, 7, 2016);
  rtc.setTime(22, 46, 00);
}

void setup() {
  Serial.begin(9600);
  pixels.begin();
  rtc.begin();

  lc.shutdown(0, false);
  lc.setIntensity(0, 1);
  lc.clearDisplay(0);
  
//  setTime();
}

void displayNeoTime() {
  Serial.println(rtc.getTimeStr());

  pixels.clear();

  Time t = rtc.getTime();
  int hour = floor(t.hour / 2.0);
  int minute = floor(t.min / 5.0);
  int second = floor(t.sec / 5.0);


  for (int i = 0; i < NUMPIXELS; i++) {
    byte r = 3;
    byte g = 3;
    byte b = 3;

    if (hour == i) {
      r = 150;
    }

    if (minute == i) {
      g = 150;
    }

    if (second == i) {
      b = 150;
    }

    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }

  pixels.show();
}

void displayLedTime() {
  Time t = rtc.getTime();
  digitalWrite(13, t.sec % 2 == 0);

  if (displayMode) {
    lc.setDigit(0, 0, t.sec % 10, false);
    lc.setDigit(0, 1, t.sec / 10, false);
    lc.setDigit(0, 3, t.min % 10, false);
    lc.setDigit(0, 4, t.min / 10, false);
    lc.setDigit(0, 6, t.hour % 10, false);
    lc.setDigit(0, 7, t.hour / 10, false);

    lc.setLed(0, 3, 0, t.sec % 2 == 0);
  } else {
    lc.setDigit(0, 0, t.year % 10, false);
    lc.setDigit(0, 1, (t.year / 10) % 10, false);
    lc.setDigit(0, 2, (t.year / 100) % 10, false);
    lc.setDigit(0, 3, (t.year / 1000) % 10, false);
    lc.setDigit(0, 4, t.mon % 10, false);
    lc.setDigit(0, 5, t.mon / 10, false);
    lc.setDigit(0, 6, t.date % 10, false);
    lc.setDigit(0, 7, t.date / 10, false);

    lc.setLed(0, 4, 0, true);
    lc.setLed(0, 6, 0, true);
  }
}

void loop() {
  displayNeoTime();
  displayLedTime();
  delay(500);
}
