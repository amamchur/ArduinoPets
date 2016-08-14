#include <VirtualWire.h>
#include <ARDK_VW.h>

void buttonHanlder(unsigned int button, int event);

const byte buttonPins[] PROGMEM = {13, 12};
ARDK::ButtonManager<sizeof(buttonPins)> bm(buttonPins, buttonHanlder);

const byte ledPins[] PROGMEM = {2, 3, 4, 5, 6, 7, 8, 9};
ARDK::OutputDriver<sizeof(ledPins)> ld(ledPins);

struct LedMessage : ARDK::VW::Message {
  uint8_t value;
};

const int receiver_pin = 11;
const char secret[] PROGMEM = "390c87011aea4176a32227e5ff8f4eae";
ARDK::VW::Receiver receiver(secret, sizeof(secret), receiver_pin);

void buttonHanlder(unsigned int button, int event) {
  if (event != ARDK::BUTTON_EVENT_PRESS) {
    return;
  }

  ld << (uint8_t)random(0, 256);
}

void setup()
{
  Serial.begin(9600);
  receiver.begin(2000);

  bm.begin();
  ld.begin();
  ld << 15;
}

void processMessage(ARDK::VW::Message *msg) {
  LedMessage *ledMessage = (LedMessage *)msg;
  ld << ledMessage->value;
}

void loop()
{
  receiver >> processMessage;
  bm.handle();
}

