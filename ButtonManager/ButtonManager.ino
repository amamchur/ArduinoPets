#include <LedControl.h>
#include <ARDK.h>

using namespace ARDK;

LedControl lc = LedControl(51, 52, 53, 1);

long counter = 88888888;
int intensity = 0;

void buttonHanlder(void*, uint8_t, IO::ButtonEvent);
void balanceHandler(void*, uint8_t, IO::ButtonEvent);

const byte buttonPins[] PROGMEM = {49, 47, 45, 43, 41, 39, 37, 35};
IO::Buttons<sizeof(buttonPins)> bm(buttonPins, buttonHanlder);

void balanceHandler(void* sender, uint8_t button, IO::ButtonEvent event) {
  if (event == IO::ButtonEventPress) {
    return;
  }

  switch (event) {
    case IO::ButtonEventDown:
      counter++;
      break;
    case IO::ButtonEventUp:
      counter--;
      break;
  }

  displayNumber(counter);
}

void buttonHanlder(void* sender, uint8_t button, IO::ButtonEvent event) {
  if (event == IO::ButtonEventPress) {
    return;
  }

  switch (button) {
    case 0:
      counter++;
      break;
    case 1:
      counter--;
      break;
    case 2:
      counter += 10;
      break;
    case 3:
      counter -= 10;
      break;
    case 4:
      intensity = min(intensity + 1, 15);
      lc.setIntensity(0, intensity);
      return;
    case 5:
      intensity = max(intensity - 1, 0);
      lc.setIntensity(0, intensity);
      return;
    case 6:
      counter = counter == 0 ? 88888888 : 0;
      break;
    case 7:
      counter *= -1;
      break;
  }

  displayNumber(counter);
}

void displayNumber(long value) {
  lc.clearDisplay(0);
  lc.setDigit(0, 0, 0, false);
  int index = 0;
  bool negative = value < 0;
  value = abs(value);

  bool dp = value == 88888888;
  while (value > 0) {
    lc.setDigit(0, index, value % 10, dp);
    index++;
    value /= 10;
  }

  if (negative) {
    lc.setChar(0, index, '-', false);
  }
}

void setup() {
  bm.begin(5, 100);
  lc.shutdown(0, false);
  lc.setIntensity(0, intensity);
  lc.clearDisplay(0);

  displayNumber(counter);
}

unsigned long loopCounter = 0;
unsigned long t = 0;

void loop() {
  loopCounter++;
  bm.handle();
}
