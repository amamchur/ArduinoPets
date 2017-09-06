#include <ARDK.h>
#include <Board/Auto.hpp>
#include <Framework/Auto.hpp>

using namespace ARDK;
using namespace ARDK::GPIO;
using namespace ARDK::Board;
using namespace ARDK::Framework;

Delay d;

void setup() {
  BuildInLed::mode<Output>();
}

void loop() {
  BuildInLed::high();
  d.milliseconds(500);
  BuildInLed::low();
  d.milliseconds(500);
}

// Comment it to use Arduino framework (+310 code; +9 memory)
int main() {
  BuildInLed::mode<Output>();
  while(1) {
    BuildInLed::high();
    d.milliseconds(500); 
    BuildInLed::low();
    d.milliseconds(500);
  }
  return 0;
}

