
#include "Mouse.h"
#include <ARDK.h>
#include <MCU/ATmega_16_32_U4.hpp>
#include <IO/Button.hpp>

using namespace ARDK;
using namespace ARDK::GPIO;
using namespace ARDK::MCU;
using namespace ARDK::IO;

typedef ATmega_16_32_U4 MMCU;
typedef MMCU::PinD4 MyPin;
typedef BaseAPI<MMCU::GPIOChain> API;

int16_t xAxisOrigin = 0;
int16_t yAxisOrigin = 0;
int16_t maxStep = 10;
float originMax = 0;

void handlerLeftButton(void *, ButtonEvent event) {
  switch (event) {
    case ButtonEventDown:
      Mouse.press(MOUSE_LEFT);
      break;
    case ButtonEventUp:
      Mouse.release(MOUSE_LEFT);
      break;
    default:
      break;
  }
}

void handlerRightButton(void *, ButtonEvent event) {
  switch (event) {
    case ButtonEventDown:
      Mouse.press(MOUSE_RIGHT);
      break;
    case ButtonEventUp:
      Mouse.release(MOUSE_RIGHT);
      break;
    default:
      break;
  }
}

typedef ButtonExt<MMCU::PinF5, handlerLeftButton, InputPullUp, 5, 0> LeftButton;
typedef Button<MMCU::PinB5, handlerRightButton, InputPullUp, 5, 0> RightButton;
LeftButton leftButton;
RightButton rightButton;

void setup() {
  Serial.begin(9600);
  leftButton.begin();
  rightButton.begin();

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);

  for (int i = 0; i < 16; i++) {
    xAxisOrigin += analogRead(A0);
    yAxisOrigin += analogRead(A1);
  }

  xAxisOrigin /= 16;
  yAxisOrigin /= 16;

  originMax = max(xAxisOrigin, yAxisOrigin);

  Mouse.begin();
}

template<class Pin> void pulse(uint32_t delay) {
  Pin::high();
  _delay_ms(delay);
  Pin::low();
}

uint32_t preTime = 0;

void loop() {
  float dx = (analogRead(A0) - xAxisOrigin) / originMax;
  float dy = (analogRead(A1) - yAxisOrigin) / originMax;
  int16_t x = maxStep * dx;
  int16_t y = maxStep * dy;

  uint32_t ms = millis();
  if ((x != 0 || y != 0) && (ms - preTime > 13)) {
    preTime = ms;
    Mouse.move(x, y, 0);
  }

  leftButton.handle(ms);
  rightButton.handle(ms);
}

