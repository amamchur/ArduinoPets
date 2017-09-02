#include <ARDK.h>
#include <IC/IC74HC595.hpp>

using namespace ARDK::GPIO;
using namespace ARDK::Board;
using namespace ARDK::IC;

IC74HC595<BD08, BD09, BD10> shiftRegister;

void setup() {
  BD11::mode<Output>();
  BD11::high();
  
  shiftRegister.begin();
}

uint16_t counter = 0;
uint8_t index = 0;

void loop() {
  counter = 1 << index;
  index = (index + 1) & 0xF; 
  shiftRegister << counter;
  
  delay(100);
}

