
#include <IRremote.h>

#include "MCURDK.h"
#include "Board/Auto.hpp"
#include "IO/Button.hpp"
#include "Utils/ToolSet.hpp"
#include "Utils/MillisecondsCounter.hpp"
#include "IC/WS2812.hpp"
#include "IC/IC74HC595.hpp"

IRrecv irrecv(2);
IRsend irsend;

using namespace MCURDK;
using namespace MCURDK::Board;
using namespace MCURDK::IO;
using namespace MCURDK::IC;
using namespace MCURDK::Data;
using namespace MCURDK::GPIO;

typedef ActiveLow<BD13> Led1;
typedef ActiveLow<BD12> Led2;
typedef ActiveLow<BD11> Led3;
typedef ActiveLow<BD10> Led4;
typedef ActiveDrain<BD03> Beeper;
typedef IC74HC595<BD08, BD04, BD07> ShieldDisplay;
typedef BD05 LedDataIn;

typedef uint32_t CounterType;
typedef MCURDK::Utils::MillisecondsCounter<CounterType, &timer0_millis> Counter;
typedef MCURDK::Utils::ToolSet<Counter> Tools;
typedef Tools::Delay Delay;
typedef Tools::FunctionTimeout<16, int16_t> Timeout;

ButtonExt<BA01, Counter> button1;
ButtonExt<BA02, Counter> button2;
ButtonExt<BA03, Counter> button3;
ShieldDisplay shieldDisplay;

typedef struct Pixel {
  uint8_t green;
  uint8_t red;
  uint8_t blue;
  uint8_t white;
} Pixel;

const int16_t PixelCount = 120;
const int16_t Half = PixelCount / 2;
const int16_t ThirdPart = PixelCount / 3;

Pixel pixels[PixelCount];

typedef TransmitterWS2812<LedDataIn, F_CPU, TimingSK6812> Transmitter;
class NeoPixel : public WS2812<Transmitter, typename Tools::Delay> {
  public:
    inline void display() {
      send(pixels, sizeof(pixels));
    }
};

NeoPixel neoPixel;
Timeout timeout;

void shiftLeft(int16_t counter) {
  Pixel p = pixels[0];
  for (int16_t i = 0; i < PixelCount - 1; i++) {
    pixels[i] = pixels[i + 1];
  }
  pixels[PixelCount - 1] = p;
  neoPixel.display();

  if (counter > 1) {
    timeout.schedule(0, shiftLeft, counter - 1);
  }
}

void shiftRight(int16_t counter) {
  Pixel p = pixels[PixelCount - 1];
  for (int16_t i = PixelCount - 1; i > 0; i--) {
    pixels[i] = pixels[i - 1];
  }
  pixels[0] = p;
  neoPixel.display();

  if (counter > 1) {
    timeout.schedule(0, shiftRight, counter - 1);
  }
}

void runShift(int16_t) {
  timeout.schedule(0, shiftRight, ThirdPart);
  timeout.schedule(2000, runShift);
}

void fillData(int16_t count) {
  for (int16_t i = 0; i < PixelCount; i++) {
    uint8_t w = i < count ? 255 : 0;
    pixels[i].green = w;
    pixels[i].red = w;
    pixels[i].blue = w;
    pixels[i].white = w;
  }
}

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();

  API::mode<PinMode::Output>(API::Chain() & Led1() & Led2() & Led3() & Led4() & Beeper());
  Led1::off();
  Led2::off();
  Led3::off();
  Led4::off();
  Beeper::off();

  button1.begin();
  button2.begin();
  button3.begin();
  shieldDisplay.begin();
  neoPixel.begin();

  fillData(40);
  neoPixel.display();
  //  timeout.schedule(0, runShift);
}

void loop() {
  timeout.handle();

  decode_results results;
  if (!irrecv.decode(&results)) {
    return;
  }
  irrecv.resume();
  switch (results.value) {
    case 0x00FF22DD:
    case 0x03F4807F:
      Serial.println("sinft left");
      timeout.schedule(0, shiftLeft, ThirdPart);
      break;
    case 0x00FFC23D:
    case 0x03F4C03F:
      timeout.schedule(0, shiftRight, ThirdPart);
      break;
  }
  Serial.println(results.value, HEX);
}

