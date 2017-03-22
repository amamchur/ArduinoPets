#include <ARDK.h>

using namespace ARDK;
using namespace ARDK::IO;
using namespace ARDK::IC;

#define BEEPER_PIN    3

typedef A00 Potentiometer;
typedef D13 Led1;
typedef D12 Led2;
typedef D11 Led3;
typedef D10 Led4;
typedef IC74HC595<D08, D04, D07> Display;

void button1Handler(void*, ButtonEvent);
void button2Handler(void*, ButtonEvent);
void button3Handler(void*, ButtonEvent);

Display segmentDisplay;

/*******************
 *  Segment mask (bit unset - led on)
 *     0x01
 *   ___V____
 *  |        |
 *  |<0x20   |
 *  |        |<0x02
 *  |  0x40  |
 *  |___V____|
 *  |        |
 *  |<0x10   |
 *  |        |<0x04
 *  |  0x08  |
 *  |___V____|o<0x80
 * 
*******************/

const uint8_t SEGMENT_MAP[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0X80, 0X90};
const uint8_t SEGMENT_SELECT[] = {0x01, 0x02, 0x04, 0x08};
uint8_t segments[4] = {182, SEGMENT_MAP[1], SEGMENT_MAP[2], SEGMENT_MAP[3]};
uint8_t segmentValue = 0xC0;
uint8_t segmentMask = 0xF2;
uint8_t displayMask = 0xFF;
uint8_t index = 0;

Button<A01, button1Handler> button1;
Button<A02, button2Handler> button2;
Button<A03, button3Handler> button3;

void button1Handler(void*, ButtonEvent event) {
  if (event == ButtonEventPress) {
    segments[0] ^= 0x40;
  }
}

void button2Handler(void*, ButtonEvent event) {
  if (event == ButtonEventPress) {
    segments[0] ^= 0x80;
  }
}

void button3Handler(void*, ButtonEvent event) {
  if (event == ButtonEventPress) {
    segments[0] ^= 0x20;
  }
}

void setup () {
  Serial.begin(9600);
  Potentiometer::mode(INPUT);
  Led1::mode(OUTPUT);
  Led2::mode(OUTPUT);
  Led3::mode(OUTPUT);
  Led4::mode(OUTPUT);

  segmentDisplay.begin();
  button1.begin();
  button2.begin();
  button3.begin();

  BufferedOutput() << Led1(1) << Led2(1) << Led3(1) << Led4(1);
}

uint16_t value = 0;
unsigned long lastTick = 0;

void loop() {
  unsigned long t = millis();
  unsigned long dt = t - lastTick;

  button1.handle();
  button2.handle();
  button3.handle();

  if (dt > 3) { // 1s / 4 segments / 60hz ~ 0.00416 sec
    lastTick = t;
    index = (index + 1) & 0x3;
    segmentMask = SEGMENT_SELECT[index];
    segmentValue = segments[index];
    segmentDisplay << segmentValue << segmentMask;
  }
}

