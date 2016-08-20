#include <ARDK.h>

using namespace ARDK;

const int PIN_DS = 11;
const int PIN_STCP = 12;
const int PIN_SHCP = 13;

using namespace ARDK;

const IC::IC74HC595<>::Config cfg PROGMEM = {
  .DS = 11,
  .STCP = 12,
  .SHCP = 13
};

const byte buttonPins[] PROGMEM = {47, 45, 43, 41, 37, 35, 33, 31, 29};
void buttonHanlder(unsigned int button, int event);

IO::Buttons<sizeof(buttonPins)> bm(buttonPins, buttonHanlder);
IC::IC74HC595<> ic74HC595(&cfg);

const uint32_t animationFrames[] PROGMEM = {
  0x80000001,
  0xC0000003,
  0xE0000007,
  0xF000000F,
  0xF800001F,
  0xFC00003F,
  0xFE00007F,
  0xFF0000FF,
  0xFF8001FF,
  0xFFC003FF,
  0xFFE007FF,
  0xFFF00FFF,
  0xFFF81FFF,
  0xFFFC3FFF,
  0xFFFE7FFF,
  0xFFFFFFFF,
  0xFFFE7FFF,
  0xFFFC3FFF,
  0xFFF81FFF,
  0xFFF00FFF,
  0xFFE007FF,
  0xFFC003FF,
  0xFF8001FF,
  0xFF0000FF,
  0xFE00007F,
  0xFC00003F,
  0xF800001F,
  0xF000000F,
  0xE0000007,
  0xC0000003,
  0x80000001,
  0x00000000
};

const int framesCount = sizeof(animationFrames) / sizeof(animationFrames[0]);
const int animationDelay = 30;
unsigned long prevFrameTime = 0;
int frameIndex = 0;

void animate() {
  unsigned long dt = millis() - prevFrameTime;
  if (dt > animationDelay) {
    uint32_t frame = pgm_read_dword_near(animationFrames + frameIndex);
    ic74HC595 << lsb<uint32_t>(frame) << end;
    frameIndex = (frameIndex + 1) % framesCount;
    prevFrameTime = millis();
  }
}

void buttonHanlder(unsigned int button, int event) {
  if (event != IO::BUTTON_EVENT_PRESS) {
    return;
  }

  Serial.println(button);

  switch (button) {
    case 0:
      ic74HC595 << b0 << end;
      break;
    case 1:
      ic74HC595 << b1 << end;
      break;
    case 2:
      ic74HC595 << lsb<uint32_t>(0xF000F731) << end;
      break;
    case 3:
      ic74HC595 << msb<uint32_t>(0xF000F731) << end;
      break;
    case 4:
      ic74HC595 << lsb<uint8_t>(0x83) << end;
      break;
    case 5:
      ic74HC595 << msb<uint8_t>(0x83) << end;
      break;
    case 6:
      break;
    case 7:
      ic74HC595 << lsb<uint64_t>(0) << end;
      break;
  }
}

void setup() {
  Serial.begin(9600);
  ARDK::LSB<int> q(3);
  bm.begin();
  ic74HC595.begin();
}

void loop() {
  bm.handle();
}

