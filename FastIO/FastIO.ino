#include <ARDK.h>

using namespace ARDK::IO;

template <class PIN> void test() {
  PIN::mode(OUTPUT);
  while (1) {
    PIN::low();
    delay(200);
    PIN::high();
    delay(200);
  }
}

int pin = A0;

template <class PIN> void atest() {
  //pinMode(A0, INPUT);
  PIN::mode(INPUT);
  while (1) {
    uint16_t value = PIN::readQ();
//    uint16_t value = analogRead(pin);

//    uint16_t value = A0;
    Serial.println(value);
//    Serial.println(A0);
    delay(500);
  }
}

void setup() {
  Serial.begin(9600);
}

void loop() {
//  analogRead(A0);
  atest<A01>();
}

