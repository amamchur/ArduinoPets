#include <ARDK.h>
#include <LiquidCrystal_I2C.h>
#include "Animation.h"

using namespace ARDK;
using namespace ARDK::IO;

LiquidCrystal_I2C lcd(0x3F, 20, 4);

const IC::IC74HC595<>::Config cfg595 PROGMEM = {
  .DS = 11,
  .STCP = 13,
  .SHCP = 12
};

const uint8_t BuzzerPin = 10;

const IC::IC74HC165<>::Config cfg165 PROGMEM = {
  .PL = 8,
  .CP = 9,
  .Q = 7
};

typedef void (*LoopHandler)();
typedef struct ModeHanlder {
  LoopHandler loopHandler;
  ButtonHandler buttonHandler;
} ModeHanlder;

const byte buttonPins[] PROGMEM = {47, 45, 43, 41, 37, 35, 33, 31, 29};
void ic74HC595ButtonHanlder(unsigned int button, int event);
void nullButtonHanlder(unsigned int button, int event);

void nullLoopHanlder();
void animate();

uint8_t applicationMode = 0xFF;
LoopHandler loopHandler = nullLoopHanlder;

ModeHanlder modeHandlers[] = {
  {nullLoopHanlder, nullButtonHanlder},
  {animate, nullButtonHanlder},
  {nullLoopHanlder, ic74HC595ButtonHanlder}
};

const size_t ModeHanldersCount = sizeof(modeHandlers) / sizeof(ModeHanlder);

IO::Buttons<sizeof(buttonPins)> bm(buttonPins, nullButtonHanlder);
IC::IC74HC595<> ic74HC595(&cfg595);
IC::IC74HC165<> ic74HC165(&cfg165);

const int framesCount = sizeof(animationFrames) / sizeof(animationFrames[0]);
const int animationDelay = 30;
unsigned long prevFrameTime = 0;
int animateFrameIndex = 0;

void animate() {
  unsigned long dt = millis() - prevFrameTime;
  if (dt > animationDelay) {
    uint32_t frame = pgm_read_dword_near(animationFrames + animateFrameIndex);
    ic74HC595 << frame << end;
    animateFrameIndex = (animateFrameIndex + 1) % framesCount;
    prevFrameTime = millis();
  }
}

void nullButtonHanlder(unsigned int button, int event) {
}

void ic74HC595ButtonHanlder(unsigned int button, int event) {
  if (event != IO::BUTTON_EVENT_PRESS) {
    return;
  }

  Serial.println(button);

  uint8_t value;

  switch (button) {
    case 0:
      ic74HC595 << w0 << end;
      break;
    case 1:
      ic74HC595 << w1 << end;
      break;
    case 2:
      ic74HC595 << (uint32_t)0xF000F731 << end;
      break;
    case 3:
      ic74HC595 << (uint32_t)0xF000F731 << end;
      break;
    case 4:
      ic74HC595 << (uint8_t)0x83 << end;
      break;
    case 5:
      ic74HC595 << (uint8_t)0x83 << end;
      break;
    case 6:
      ic74HC165 >> start >> value;
      ic74HC595 << (uint32_t)value;
      break;
    case 7:
      tone(BuzzerPin, 220, 200);
      break;
  }
}

void nullLoopHanlder() {
}

void readApplicationMode() {
  uint8_t mode;
  ic74HC165 >> start >> mode;
  mode = min(mode, ModeHanldersCount - 1);
  if (applicationMode == mode) {
    return;
  }

  tone(BuzzerPin, 440, 100);

  applicationMode = mode;
  loopHandler = modeHandlers[applicationMode].loopHandler;
  bm.handler = modeHandlers[applicationMode].buttonHandler;
  ic74HC595 << (uint32_t)0;
}

void setup() {
  Serial.begin(9600);

  pinMode(BuzzerPin, OUTPUT);
  lcd.init();
  bm.begin();
  ic74HC595.begin();
  ic74HC165.begin();
  readApplicationMode();

  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Hello, world!");
}

void loop() {
  bm.handle();
  readApplicationMode();
  loopHandler();
}

