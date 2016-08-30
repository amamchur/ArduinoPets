#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <ARDK.h>

using namespace ARDK;
using namespace ARDK::IO;

void keyPressed(char, ButtonEvent);
const uint8_t keypadPins[] PROGMEM = {52, 50, 48, 46, 44, 42, 40, 38};

const char keys[4][4] PROGMEM = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '1', '#', 'D'}
};

KeyLayout<4, 4> layout(keys, keyPressed, 10, 200);
Keypad<4, 4> keypad(keypadPins);

Adafruit_PCD8544 display = Adafruit_PCD8544(53, 51, 49, 47, 45);
uint8_t contrast = 62;

const uint8_t BUZZER_PIN = 2;

void keyPressed(char ch, ButtonEvent event) {
  if (event != ButtonEventPress) {
    return;
  }

  Serial.println(ch);

  switch (ch) {
    case '1':
      contrast++;
      display.setContrast(contrast);
      break;
    case '2':
      contrast--;
      display.setContrast(contrast);
      break;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
  display.print("Contrast:");
  display.print(contrast);
  display.display();
}

void setup()   {
  Serial.begin(9600);

  display.begin();
  display.setContrast(contrast);
  display.setRotation(2);
  display.display();

  keypad.begin();

  pinMode(BUZZER_PIN, OUTPUT);
  analogWrite(BUZZER_PIN, 50);
  delay(150);
  analogWrite(BUZZER_PIN, 0);
}

void loop() {
  keypad.scan(layout);
}

