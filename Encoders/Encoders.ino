#include <ARDK.h>
#include <Arduino.h>
#include "LedControl.h"

using namespace ARDK;

LedControl lc = LedControl(11, 13, 12, 1);

void rotaryButtonHandler(void *, IO::RotaryEvent re, IO::ButtonEvent be);
void rotaryHandler(void *, IO::RotaryEvent re);

IO::ButtonRotaryEncoder<43, 45, 41, rotaryButtonHandler, IO::RotaryHSM> encoder1;
IO::RotaryEncoder<33, 31, rotaryHandler, IO::RotaryFSM> encoder2;

long int counter = 0;
bool needUpdate = true;
unsigned long updateTime = 0;

void rotaryHandler(void *, IO::RotaryEvent re) {
  if (re != IO::RotaryEventNone) {
    counter += re == IO::RotaryEventDirection1 ? -1 : 1;
    setNeedUpdate();
  }
}

void rotaryButtonHandler(void *sender, IO::RotaryEvent re, IO::ButtonEvent be) {
  if (be == IO::ButtonEventPress) {
    counter = 0;
    setNeedUpdate();
  }

  if (re != IO::RotaryEventNone) {
    counter += re == IO::RotaryEventDirection1 ? -1 : 1;
    setNeedUpdate();
  }
}

void setNeedUpdate() {
  updateTime = millis();
  needUpdate = true;
}

void updateDisplay() {
  long int value = counter;
  int sign = counter < 0 ? -1 : 1;
  int index = 0;
  lc.clearDisplay(0);
  if (value == 0) {
    lc.setDigit(0, index, 0, false);
  }

  value = abs(value);
  while (value > 0) {
    int d = value % 10;
    value /= 10;
    lc.setDigit(0, index, d, false);
    index++;
  }

  if (sign < 0) {
    lc.setChar(0, index, '-', false);
  }

  needUpdate = false;
  updateTime = millis();
}

void setup() {
  Serial.begin(9600);
  encoder1.begin();
  encoder2.begin();
  
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
}

void loop() {
  encoder1.handle();
  encoder2.handle();

  unsigned long time = millis();
  if (needUpdate && time - updateTime > 100) {
    updateDisplay();
  }
}

