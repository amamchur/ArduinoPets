#include <ARDK.h>
#include <Board/Auto.hpp>
#include <IO/Button.hpp>
#include <IC/IC74HC595.hpp>
#include <Data/Segment7.hpp>
#include <Utils/Timeout.hpp>
#include <Framework/Auto.hpp>

#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(A4);
DallasTemperature sensors(&oneWire);

using namespace ARDK;
using namespace ARDK::GPIO;
using namespace ARDK::IO;
using namespace ARDK::IC;
using namespace ARDK::Data;
using namespace ARDK::Board;
using namespace ARDK::Utils;

typedef BA00 Potentiometer;
typedef ActiveLow<BD13> Led1;
typedef ActiveLow<BD12> Led2;
typedef ActiveLow<BD11> Led3;
typedef ActiveLow<BD10> Led4;
typedef ActiveLow<BD03> Beeper;
typedef IC74HC595<BD08, BD04, BD07> Display;

void button1Handler(void*, ButtonEvent);
void button2Handler(void*, ButtonEvent);
void button3Handler(void*, ButtonEvent);

Display segmentDisplay;
Timeout<&timer0_millis, 4> timeout;

uint8_t segments[4];
uint16_t value = 0;

ButtonExt<BA01, button1Handler> button1;
ButtonExt<BA02, button2Handler> button2;
Button<BA03, button3Handler> button3;

void hexToSegments(uint16_t value) {
  uint16_t v = value;
  for (int i = 3; i >= 0; i--) {
    uint8_t d = v & 0x0F;
    segments[i] = ~Segment7::abcdASCII(d + 0x30);
    v >>= 4;
  }

  segments[3] &= 0x7F;
}

void decToSegments(uint16_t value) {
  uint16_t v = value;
  for (int i = 3; i >= 0; i--) {
    uint8_t d = v % 10;
    segments[i] = ~Segment7::abcdASCII(d + 0x30);
    v = v / 10;
  }

  for (int i = 0; i < 3; i++) {
    if (segments[i] == 0xC0) {
      segments[i] = 0xff; //~Segment7::gfedASCII(' ');
    } else {
      break;
    }
  }
}

void temperatureToSegments(uint16_t value) {
  decToSegments(value);
  segments[1] &= 0x7F;
  segments[3] = ~(0x08 | 0x10 | 0x40);
}

void (*fn)(uint16_t) = &decToSegments;

void beepSound() {
  Beeper::on();
  delay(1);
  Beeper::off();
}

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
    readTemperature(0);
    //    fn = fn == &decToSegments ? &hexToSegments : &decToSegments;
    //    fn(value);
  }
}

void dynamicIndication(uintptr_t) {
  static uint8_t segmentIndex = 0;
  uint8_t segmentMask = 1 << segmentIndex;
  uint8_t segmentValue = segments[segmentIndex];
  segmentDisplay << segmentValue << segmentMask;
  segmentIndex = (segmentIndex + 1) & 0x3;
  timeout.schedule(0, dynamicIndication);
}

void readTemperature(uintptr_t) {
  sensors.requestTemperatures();
  uint16_t temp = sensors.getTempCByIndex(0) * 100;
  temperatureToSegments(temp);
}

void setup () {
  Potentiometer::mode<InputFloating>();
  API::mode<Output>(
    API::Chain()
    & Led1() & Led2() & Led3() & Led4()
    & Beeper());
  API::write(API::Chain() & Led1() & Led2() & Led3() & Led4() & Beeper());

  sensors.begin();

  segmentDisplay.begin();
  button1.begin();
  button2.begin();
  button3.begin();
  fn(value);
  timeout.schedule(0, dynamicIndication);

  BD05::mode<Output>();
  beepSound();
}

unsigned long prevUpdateTime = 0;

void loop() {
  unsigned long ms = millis();
  button1.handle(ms);
  button2.handle(ms);
  button3.handle(ms);
  timeout.handle();
  BD05::toggle();
}

