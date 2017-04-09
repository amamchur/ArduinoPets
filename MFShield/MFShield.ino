#include <ARDK.h>

using namespace ARDK;
using namespace ARDK::IO;
using namespace ARDK::IC;
using namespace ARDK::Data;

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

uint8_t segments[4];
uint8_t index = 0;
uint16_t value = 0;

ButtonExt<A01, button1Handler> button1;
ButtonExt<A02, button2Handler> button2;
Button<A03, button3Handler> button3;

void hexToSegments(uint16_t value) {
  uint16_t v = value;
  for (int i = 3; i >= 0; i--) {
    uint8_t d = v & 0x0F;
    segments[i] = ~Segment7::digit(d).dot(false);
    v >>= 4;
  }

  for (int i = 0; i < 3; i++) {
    if (segments[i] == 0xC0) {
      segments[i] = 0xFF;
    } else {
      break;
    }
  }

  segments[3] &= 0x7F;
}

void decToSegments(uint16_t value) {
  uint16_t v = value;
  for (int i = 3; i >= 0; i--) {
    uint8_t d = v % 10;
    segments[i] = ~Segment7::digit(d);
    v = v / 10;
  }

  for (int i = 0; i < 3; i++) {
    if (segments[i] == 0xC0) {
      segments[i] = 0xFF;
    } else {
      break;
    }
  }
}

void (*fn)(uint16_t) = &decToSegments;

void button1Handler(void*, ButtonEvent event) {
  if (event == ButtonEventPress) {
    value++;
    fn(value);
  }
}

void button2Handler(void*, ButtonEvent event) {
  if (event == ButtonEventPress) {
    value--;
    fn(value);
  }
}

void button3Handler(void*, ButtonEvent event) {
  if (event == ButtonEventPress) {
    fn = fn == &decToSegments ? &hexToSegments : &decToSegments;
    fn(value);
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
  fn(value);
}

unsigned long lastTick = 0;

void loop() {
  unsigned long t = millis();
  unsigned long dt = t - lastTick;

  button1.handle(t);
  button2.handle(t);
  button3.handle(t);

  if (dt > 3) { // 1s / 4 segments / 60hz ~ 0.00416 sec
    lastTick = t;
    index = (index + 1) & 0x3;
    uint8_t segmentMask = 1 << index;
    uint8_t segmentValue = segments[index];
    segmentDisplay << segmentValue << segmentMask;
  }
}

