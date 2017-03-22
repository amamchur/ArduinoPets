#include <ARDK.h>

using namespace ARDK::IO;
using namespace ARDK::IC;

typedef Timer<(uintptr_t)&TCCR1A, (uintptr_t)&TCCR1B, (uintptr_t)&TCNT1> Timer1;
Timer1 timer;

void setup() {
  D13::mode(OUTPUT);
}

void loop() {
  D13::low();
  delay(200);
  D13::high();
  delay(200);
  // put your main code here, to run repeatedly:

}
