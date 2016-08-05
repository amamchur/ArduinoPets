#include <avr/pgmspace.h>
#include <Arduino.h>
#include <ARDK.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// Digital
const int PIN_DS = 11;
const int PIN_STCP = 12;
const int PIN_SHCP = 13;

ARDK::IC74HC595 ic74HC595(PIN_DS, PIN_STCP, PIN_SHCP);

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
    ic74HC595 << frame;
    frameIndex = (frameIndex + 1) % framesCount;
    prevFrameTime = millis();
  }
}

void setup() {
  Serial.begin(9600);
  ic74HC595 << (uint32_t)0;

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Potentiometer");
  lcd.setCursor(0, 1);
  lcd.print("Value:");
}

uint32_t potentiometer = 0;

void readPotentiometer() {
  int val = analogRead(3);
  val = map(val, 0, 1000, 0, 32);
  if (potentiometer == val) {
    return;
  }

  potentiometer = val;

  uint32_t v = 0;
  for (int i = 0; i < potentiometer; i++) {
    v <<= 1;
    v |= 1;
  }
  ic74HC595 << v;

  lcd.setCursor(7, 1);
  lcd.print(val);
  lcd.print("    ");
}

void loop() {
  readPotentiometer();
//  animate();
}

