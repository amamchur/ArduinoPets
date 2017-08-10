#include <ARDK.h>
#include <Board/Auto.hpp>
#include <IC/HD44780.hpp>
#include <IO/Button.hpp>
#include <IO/RotaryEncoder.hpp>
#include <Framework/AvrLibC/Delay.hpp>

using namespace ARDK::Board;
using namespace ARDK::IC;
using namespace ARDK::IO;
using namespace ARDK::GPIO;
using namespace ARDK::Framework;

void buttonHandler(void*, ButtonEvent);
void encoderHandler(void*, RotaryEvent);

typedef HD44780Interface4bit<Delay, BD02, NAP, BD03, BD08, BD09, BD10, BD11> Interface4bit;
typedef HD44780Interface8bit<Delay, BD02, NAP, BD03, BD04, BD05, BD06, BD07, BD08, BD09, BD10, BD11> Interface8bit;
typedef Button<BD16, buttonHandler> EncoderButton;
typedef RotaryEncoder<BD14, BD15, encoderHandler> Encoder;
typedef HD44780<Interface8bit, 16, 2> LCD;
//typedef HD44780<Interface4bit, 16, 2> LCD;

EncoderButton encoderButton;
Encoder encoder;
LCD lcd;

uint8_t armsUp[8] = {
  0b00100,
  0b01010,
  0b00100,
  0b10101,
  0b01110,
  0b00100,
  0b00100,
  0b01010
};

uint8_t smiley[8] = {
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b00000,
  0b10001,
  0b01110,
  0b00000
};

int number = 0;
bool needUpdate = true;

void buttonHandler(void*, ButtonEvent event) {
  number = 0;
  needUpdate = true;
}

void encoderHandler(void*, RotaryEvent event) {
  number += event == RotaryEventDirection2 ? 1 : -1;
  needUpdate = true;
}

void updateIfNeeded() {
  if (!needUpdate) {
    return;    
  }

  char string[20];
  needUpdate = false;
  sprintf(string, "Number%10d", number);
  lcd.clear();
  lcd.write(string);
}

void setup() {
  encoderButton.begin();
  encoder.begin();

  lcd.begin();
  lcd.createChar(0, armsUp);
  lcd.createChar(1, smiley);
}

uint8_t v = 0;

void loop() {
  uint32_t t = millis();
  encoderButton.handle(t);
  encoder.handle();
  updateIfNeeded();
}

