#include <VirtualWire.h>
#include <ARDK_VW.h>

#define EPIN_A 27
#define EPIN_B 25

struct LedMessage : ARDK::VW::Message {
  uint8_t value;
};

const byte buttonPins[] PROGMEM = {47, 45, 43, 41, 37, 35, 33, 31, 29};
void buttonHanlder(unsigned int button, int event);
void encoderHanlder(int step);

ARDK::ButtonManager<sizeof(buttonPins)> bm(buttonPins, buttonHanlder);
ARDK::RotaryEncoder encoder(EPIN_A, EPIN_B, encoderHanlder);

const int transmit_pin = 53;
const char secret[] PROGMEM = "390c87011aea4176a32227e5ff8f4eae";
ARDK::VW::Transmitter transmitter(secret, sizeof(secret), transmit_pin);

uint8_t ledValue = 15;

void sendMessage() {
  LedMessage msg;
  msg.id = 0;
  msg.value = ledValue;
  transmitter << msg;
}

void encoderHanlder(int step) {
  ledValue += step;
  sendMessage();
}

void buttonHanlder(unsigned int button, int event) {
  if (event != ARDK::BUTTON_EVENT_PRESS) {
    return;
  }

  switch (button) {
    case 0:
      ledValue = ledValue << 1 | 1;
      break;
    case 1:
      ledValue = ledValue >> 1;
      break;
    case 2:
      ledValue = B10101010;
      break;
    case 3:
      ledValue = B01010101;
      break;
    case 4:
      ledValue = B00000000;
      break;
    case 5:
      ledValue = B11111111;
      break;
    case 6:
      ledValue = ledValue >> 1 | ledValue << 1 | 0x81;
      break;
    case 7:
      ledValue = ledValue >> 1 | ledValue << 1;
      break;
    case 8:
      ledValue = 0x80;
      break;
  }

  sendMessage();
}

void setup()
{
  Serial.begin(9600);
  bm.begin();
  encoder.begin();
  transmitter.begin(2000);
}

void loop()
{
  bm.handle();
  encoder.handle();
}

