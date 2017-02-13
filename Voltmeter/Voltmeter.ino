#include <ARDK.h>
#include <EEPROMStorage.hpp>
#include <LiquidCrystal_I2C.h>

using namespace ARDK;
using namespace ARDK::IO;

typedef struct {
  float multiplier = 1.0f;
} Config;

Config cfg;
ARDK::EEPROMStorage<Config> configStorage(0);

LiquidCrystal_I2C lcd(0x3F, 20, 4);

void keyPressed(char, ButtonEvent);
const uint8_t keypadPins[] PROGMEM = {38, 40, 42, 44, 46, 48, 50, 52};

const char keys[4][4] PROGMEM = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '1', '#', 'D'}
};

KeyLayout<4, 4> layout(keys, keyPressed, 10, 200);
Keypad<4, 4> keypad(keypadPins);

void setup() {
  cfg.multiplier = 1.0f;
  configStorage >> cfg;

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Voltage: ");

  keypad.begin();
}

void displayNumber(float value) {
  String str = String(value, 2);
  str += " V     ";
  lcd.setCursor(0, 1);
  lcd.print(str);
}

bool calibrate = false;
uint16_t sumValue = 0;
uint16_t counter = 0;
unsigned long prevTime = 0;

void keyPressed(char ch, ButtonEvent event) {
  if (event != ButtonEventPress) {
    return;
  }

  switch (ch) {
    case '1':
      calibrate = true;
      return;
  }
}

void loop() {
  unsigned long t = millis();
  unsigned long dt = t - prevTime;

  if (dt > 50) {
    int sensorValue = analogRead(A1);
    sumValue += sensorValue;
    counter++;
    prevTime = t;
  }

  if (counter == 10) {
    float avg = (float)sumValue / (float)counter;
    float value = (float)avg * cfg.multiplier;
    counter = 0;
    sumValue = 0;

    Serial.print("Value: ");
    Serial.println(value);
    Serial.print("Avg: ");
    Serial.println(avg);

    if (calibrate) {
      calibrate = false;
      cfg.multiplier = 200.0f / (float)avg;
      configStorage << cfg;

      Serial.print("Multiplier: ");
      Serial.println(cfg.multiplier);
    }

    displayNumber(value);
  }

  keypad.scan(layout);
}

