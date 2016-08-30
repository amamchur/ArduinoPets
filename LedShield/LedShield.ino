#include <ARDK.h>

using namespace ARDK;
using namespace ARDK::IO;

void buttonHanlder(uint8_t button, ButtonEvent event);

const byte buttonPins[] PROGMEM = {13, 12};
IO::Buttons<sizeof(buttonPins)> bm(buttonPins, buttonHanlder);

const byte ledPins[] PROGMEM = {2, 3, 4, 5, 6, 7, 8, 9};
IO::OutputPins<> ld(ledPins, sizeof(ledPins));

int counter = 15;
void buttonHanlder(uint8_t button, ButtonEvent event) {
  if (event != IO::ButtonEventPress) {
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

  Serial.println(counter);
}

void setup() {
  Serial.begin(9600);
  
  ld.init();
  bm.begin();
}

void loop() {
  bm.handle();
}

