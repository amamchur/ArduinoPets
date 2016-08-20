#include <ARDK.h>

const byte buttonPins[] PROGMEM = {47, 45, 43, 41, 37, 35, 33, 31, 29};
void buttonHanlder(unsigned int button, int event);

ARDK::ButtonManager<sizeof(buttonPins)> bm(buttonPins, buttonHanlder);

const byte relayPins[] PROGMEM = {14, 15, 16, 17};
ARDK::OutputDriver<sizeof(relayPins)> relay(relayPins);

const byte PIN_MOTOR = 9;

void setup() {
  Serial.begin(9600);

  pinMode(PIN_MOTOR, OUTPUT);
  relay.begin();
  relay << ~0;
  bm.begin();
}

void loop() {
  bm.handle();
}

void buttonHanlder(unsigned int button, int event) {
  if (event != ARDK::BUTTON_EVENT_PRESS) {
    return;
  }

  switch (button) {
    case 0:
      relay << (uint8_t)~0;
      break;
    case 1:
      relay << (uint8_t)~1;
      break;
    case 2:
      relay << (uint8_t)~2;
      break;
    case 3:
      relay << (uint8_t)~4;
      break;
    case 4:
      relay << (uint8_t)~8;
      break;
    case 5:
      relay << (uint8_t)0;
      break;
    case 6:
      break;
    case 7:
      break;
    case 8:
      break;
  }
}

