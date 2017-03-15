#include <ARDK.h>

using namespace ARDK::IO;

void rotaryHandler(void*, RotaryEvent event) {
  Serial.println(event);
}

RotaryEncoder<D13, D12, rotaryHandler, RotaryHSM, INPUT> encoder;

void setup() {
  Serial.begin(9600);
  encoder.begin();
  //  button2.begin();

  D13::mode(INPUT);
  D12::mode(INPUT);
  D11::mode(OUTPUT);
}

uint8_t last = 0;

void loop() {
  encoder.handle();
}

