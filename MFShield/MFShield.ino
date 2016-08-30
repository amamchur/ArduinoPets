#include <ARDK.h>

using namespace ARDK;
using namespace ARDK::IO;
using namespace ARDK::IC;

#define BEEPER_PIN    3

const uint8_t ledPins[] PROGMEM = {13, 12, 11, 10};
Pins<OUTPUT> leds(ledPins, sizeof(ledPins));

const uint8_t buttonPins[] PROGMEM = {A1, A2, A3};
void buttonsHandlers(uint8_t, ButtonEvent);
Buttons<sizeof(buttonPins)> buttons(buttonPins, buttonsHandlers);

#define LATCH_DIO 4
#define CLK_DIO 7
#define DATA_DIO 8

const IC74HC595<>::Config cfg PROGMEM = {
  .DS = 8,
  .STCP = 4,
  .SHCP = 7
};

IC74HC595<> icIC74HC595(&cfg);

///* Segment byte maps for numbers 0 to 9 */
//const byte SEGMENT_MAP[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0X80, 0X90};
///* Byte maps to select digit 1 to 4 */
//const byte SEGMENT_SELECT[] = {0xF1, 0xF2, 0xF4, 0xF8};

uint16_t ledValue = 0x0000;

void setup () {
  //Beep beep(BEEPER_PIN, 100);

  Serial.begin(9600);
  leds.init();
  buttons.begin();
  icIC74HC595.begin();
  icIC74HC595 << msb << ledValue << end;

  //  pinMode(LATCH_DIO, OUTPUT);
  //  pinMode(CLK_DIO, OUTPUT);
  //  pinMode(DATA_DIO, OUTPUT);
}

void loop() {
  buttons.handle();
}

void buttonsHandlers(uint8_t button, ButtonEvent event) {
  if (event != ButtonEventPress) {
    return;
  }

  switch (button) {
    case 0:
      digitalWrite(LATCH_DIO, LOW);
      shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, 0xC0);
      shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, 0xF1);
      digitalWrite(LATCH_DIO, HIGH);
      break;
    case 1:
      digitalWrite(LATCH_DIO, LOW);
      shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, 0xF9);
      shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, 0xF2);
      digitalWrite(LATCH_DIO, HIGH);
      break;
    case 2:
      digitalWrite(LATCH_DIO, LOW);
      shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, 0xA4);
      shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, 0xF4);
      digitalWrite(LATCH_DIO, HIGH);
      break;
  }
}

