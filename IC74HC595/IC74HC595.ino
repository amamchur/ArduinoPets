#include <ARDK.h>

using namespace ARDK::IO;
using namespace ARDK::IC;

IC74HC595<D12, D11, D10> reg;

void setup() {
  reg.begin();
}

uint8_t counter = 0;

void loop() {
  reg << counter;
  counter++;
  delay(1);
}

