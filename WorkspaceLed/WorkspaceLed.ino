
#include <IRremote.h>

#include "MCURDK.h"
#include "Board/Auto.hpp"
#include "IO/Button.hpp"
#include "Utils/ToolSet.hpp"
#include "Utils/MillisecondsCounter.hpp"
#include "Shields/UnoMultiFunctionalShield.hpp"
#include "IC/WS2812.hpp"

IRrecv irrecv(2);
IRsend irsend;

using namespace MCURDK;
using namespace MCURDK::Board;
using namespace MCURDK::IO;
using namespace MCURDK::IC;
using namespace MCURDK::Data;
using namespace MCURDK::GPIO;

typedef BD05 LedDataIn;

typedef uint32_t CounterType;
typedef MCURDK::Utils::MillisecondsCounter<CounterType, &timer0_millis> Counter;
typedef MCURDK::Utils::ToolSet<Counter> Tools;
typedef Tools::Delay Delay;
typedef Tools::FunctionTimeout<16, int16_t> Timeout;
typedef MCURDK::Shields::UnoMultiFunctionalShield<Tools> Shield;

typedef enum LedMode {
  LedModeOff,
  LedMode1_3,
  LedMode2_3,
  LedMode1_4,
  LedMode2_4,
  LedMode3_4,
  LedModeFull,
  LedModeCount
} LedMode;

Shield mfs;

typedef struct Pixel {
  uint8_t green;
  uint8_t red;
  uint8_t blue;
  uint8_t white;
} Pixel;

const uint8_t MaxLedValue = 0x0F;
const float BrightnessStep = 1.0 / (MaxLedValue + 1);
const int16_t PixelCount = 120;
const int16_t Half = PixelCount / 2;
const int16_t ThirdPart = PixelCount / 3;

uint8_t shiftCount = PixelCount / 3;

float brightness = 0.5;
LedMode mode = LedMode1_3;
Pixel renderBuffer[PixelCount];

typedef TransmitterWS2812<LedDataIn, F_CPU, TimingSK6812> Transmitter;
class NeoPixel : public WS2812<Transmitter, typename Tools::Delay> {
  public:
    inline void display() {
      send(renderBuffer, sizeof(renderBuffer));
    }
};

NeoPixel neoPixel;
Timeout timeout;

void shiftLeft(int16_t counter) {
  if (counter == 0) {
    return;
  }

  Pixel p = renderBuffer[0];
  for (int16_t i = 0; i < PixelCount - 1; i++) {
    renderBuffer[i] = renderBuffer[i + 1];
  }
  renderBuffer[PixelCount - 1] = p;
  neoPixel.display();

  timeout.schedule(0, shiftLeft, counter - 1);
}

void shiftRight(int16_t counter) {
  if (counter == 0) {
    return;
  }

  Pixel p = renderBuffer[PixelCount - 1];
  for (int16_t i = PixelCount - 1; i > 0; i--) {
    renderBuffer[i] = renderBuffer[i - 1];
  }
  renderBuffer[0] = p;
  neoPixel.display();

  timeout.schedule(0, shiftRight, counter - 1);
}

void runShift(int16_t) {
  timeout.schedule(0, shiftRight, ThirdPart);
  timeout.schedule(2000, runShift);
}

void fillData(int lm) {
  if (lm == LedModeOff) {
    memset (renderBuffer, 0, sizeof(renderBuffer));
    return;
  }

  int16_t count = 0;
  switch (lm) {
    case LedMode1_3:
      count = PixelCount / 3;
      break;
    case LedMode2_3:
      count = 2 * PixelCount / 3;
      break;
    case LedMode1_4:
      count = PixelCount / 4;
      break;
    case LedMode2_4:
      count = 2 * PixelCount / 4;
      break;
    case LedMode3_4:
      count = 3 * PixelCount / 4;
      break;
    case LedModeFull:
      count = PixelCount;
      break;
    default:
      break;
  }

  uint8_t value = MaxLedValue * brightness;
  for (int16_t i = 0; i < PixelCount; i++) {
    uint8_t w = i < count ? value : 0;
    renderBuffer[i].green = w;
    renderBuffer[i].red = w;
    renderBuffer[i].blue = w;
    renderBuffer[i].white = w;
  }
}

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();

  mfs.begin();
  neoPixel.begin();

  fillData(mode);
  neoPixel.display();

  mfs.beep();
  mfs.hexToSegments(mode);
}

void setBrightness(float d) {
  float next = brightness + d;
  if (next > 1.0) {
    next = 1.0;
  }

  if (next < 0.0) {
    next = 0.0;
  }

  Serial.println(next);

  brightness = next;
  fillData(mode);
  neoPixel.display();
}

void loop() {
  mfs.dynamicIndication();
  mfs.handle();
  timeout.handle();

  decode_results results;
  if (!irrecv.decode(&results)) {
    return;
  }
  irrecv.resume();
  switch (results.value) {
    case 0x807FA857:
      mode = (LedMode)((mode + 1) % LedModeCount);
      switch (mode) {
        case LedMode1_3:
        case LedMode2_3:
          shiftCount = PixelCount / 3;
          break;
        case LedMode1_4:
        case LedMode2_4:
        case LedMode3_4:
          shiftCount = PixelCount / 4;
          break;
        default:
          shiftCount = 0;
          break;
      }
      mfs.hexToSegments(mode);
      fillData(mode);
      neoPixel.display();
      break;
    case 0x807FE817:
      setBrightness(BrightnessStep);
      break;
    case 0x807F58A7:
      setBrightness(-BrightnessStep);
      break;
    case 0x807F42BD:
      timeout.schedule(0, shiftLeft, shiftCount);
      break;
    case 0x807F827D:
      timeout.schedule(0, shiftRight, shiftCount);
      break;
  }
  Serial.println(results.value, HEX);
}

