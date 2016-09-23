#include <Arduino.h>
#include <ARDK.h>
#include <Servo.h>
#include "LedControl.h"

using namespace ARDK;

void rotaryButtonHandler1(void *, IO::RotaryEvent re, IO::ButtonEvent be);
void rotaryButtonHandler2(void *, IO::RotaryEvent re, IO::ButtonEvent be);

typedef IO::ButtonRotaryEncoder<43, 45, 41, rotaryButtonHandler1, IO::RotaryHSM> ButtonRotaryEncoder1;
typedef IO::ButtonRotaryEncoder<33, 31, 29, rotaryButtonHandler2, IO::RotaryFSM> ButtonRotaryEncoder2;

LedControl lc = LedControl(11, 13, 12, 1);
Servo servo;

ButtonRotaryEncoder1 encoder1;
ButtonRotaryEncoder2 encoder2;

long int counter = 0;
bool needUpdate = true;
unsigned long updateTime = 0;

void setCounter(int c) {
  counter = constrain(c, 0, 160);
  servo.write(counter);
  updateTime = millis();
  needUpdate = true;
}

void rotaryButtonHandler2(void *, IO::RotaryEvent re, IO::ButtonEvent be) {
  if (be == IO::ButtonEventPress) {
    setCounter(0);
    return;
  }

  switch (re) {
    case IO::RotaryEventDirection1:
      setCounter(counter + 1);
      break;
    case IO::RotaryEventDirection2:
      setCounter(counter - 1);
      break;
  }
}

void rotaryButtonHandler1(void *sender, IO::RotaryEvent re, IO::ButtonEvent be) {
  if (re == IO::RotaryEventNone) {
    return;
  }

  ButtonRotaryEncoder1 *bre = (ButtonRotaryEncoder1 *)sender;
  uint8_t state = bre->getState();
  int value = re == IO::RotaryEventDirection1 ? -1 : 1;
  if ((state & IO::BUTTON_STATE_CURRENT_STATE) == 1) {
    value *= 10;
  }

  if (re != IO::RotaryEventNone) {
    setCounter(counter + value);
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
  servo.attach(7);
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

