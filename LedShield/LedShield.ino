#include <ARDK.h>

void buttonHanlder(unsigned int button, int event);

const byte buttonPins[] PROGMEM = {13, 12};
ARDK::ButtonManager<sizeof(buttonPins)> bm(buttonPins, buttonHanlder);

const byte ledPins[] PROGMEM = {2, 3, 4, 5, 6, 7, 8, 9};
ARDK::LedDriver<sizeof(ledPins)> ld(ledPins);

int counter = 15;
void buttonHanlder(unsigned int button, int event) {
  if (event != ARDK::BUTTON_EVENT_PRESS) {
    return;
  }

  switch (button) {
    case 0:
      counter = min(255, counter << 1 | 1);
      break;
    case 1:
      counter = counter >> 1;
      break;
  }

  ld << counter;
}

void setup() {
  bm.begin();
  ld.begin();
  ld << counter;
}

void loop() {
  bm.handle();
}

