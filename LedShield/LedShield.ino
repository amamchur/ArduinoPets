#include <ARDK.h>

using namespace ARDK;

void buttonHanlder(unsigned int button, int event);

const byte buttonPins[] PROGMEM = {13, 12};
IO::Buttons<sizeof(buttonPins)> bm(buttonPins, buttonHanlder);

const byte ledPins[] PROGMEM = {2, 3, 4, 5, 6, 7, 8, 9};
IO::OutputPins<> ld(ledPins, sizeof(ledPins));

int counter = 15;
void buttonHanlder(unsigned int button, int event) {
  if (event != IO::BUTTON_EVENT_PRESS) {
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

  ld << lsb<uint8_t>(counter);
  Serial.println(counter);
}

void setup() {
  Serial.begin(9600);
  
  ld.init();
  bm.begin();

  ld << lsb<uint8_t>(counter);
}

void loop() {
  bm.handle();
}

