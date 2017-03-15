#include <ARDK.h>

using namespace ARDK::IO;
using namespace ARDK::IC;

typedef IC74HC595<D13, D12, D11> IC;
IC reg;

void setup() {
  //Serial.begin(9600);
  reg.begin();
  reg << (uint8_t)0xAA;
}

uint8_t counter = 0;

void loop() {
  while (1) {
    reg << (uint8_t)0xAA;
  }
}
