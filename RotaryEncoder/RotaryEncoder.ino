#include <ARDK.h>
#include "TouchScreen.h"
#include "SWTFT.h"

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
SWTFT tft;

int counter = 0;
boolean needRefresh = false;
unsigned long lastRefresh = 0;

const byte buttonPins[] PROGMEM = {53, 51, 49, 47, 37, 35, 33, 31, 29};
void buttonHanlder(unsigned int button, int event);
void encoderHanlder(int step);

ARDK::ButtonManager<sizeof(buttonPins)> bm(buttonPins, buttonHanlder);

#define EPIN_A 27
#define EPIN_B 25

ARDK::RotaryEncoder encoder(EPIN_A, EPIN_B, encoderHanlder);

void buttonHanlder(unsigned int button, int event) {
  if (event != ARDK::BUTTON_EVENT_PRESS) {
    return;
  }

  switch (button) {
    case 0:
      refreshScreenFull();
      break;
  }
  Serial.println(button);
}

void refreshScreenFull() {
  needRefresh = false;
  lastRefresh = millis();

  tft.fillScreen(BLACK);
  tft.setCursor(10, 10);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("Counter: ");
  refreshScreen();
}

void refreshScreen() {
  needRefresh = false;
  lastRefresh = millis();

  tft.fillRect(110, 10, 70, 15, BLACK);
  tft.setCursor(110, 10);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.println(counter);
}

void needsDisplay() {
  needRefresh = true;
  lastRefresh = millis();
}

void encoderHanlder(int step) {
  counter += step;
  needsDisplay();
}

void setup() {
  Serial.begin(9600);

  bm.begin();
  encoder.begin();

  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  
  delay(100);
  refreshScreenFull();
}

void loop() {
  bm.handle();
  encoder.handle();

  unsigned long time = millis();
  if (needRefresh && (time - lastRefresh > 30)) {
    refreshScreen();
  }
}
