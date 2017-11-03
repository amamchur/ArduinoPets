//
//#include "Mouse.h"
#include "Keyboard.h"

#include <MCURDK.h>
#include <Board/ArduinoLeonardo.hpp>
#include <Utils/ToolSet.hpp>
#include <Utils/MillisecondsCounter.hpp>
#include <IO/Button.hpp>
#include <IO/RotaryEncoder.hpp>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

using namespace MCURDK;
using namespace MCURDK::GPIO;
using namespace MCURDK::MCU;
using namespace MCURDK::IO;
using namespace MCURDK::Board;
using namespace MCURDK::Utils;

typedef MillisecondsCounter<typeof(timer0_millis), &timer0_millis> Counter;
typedef Utils::ToolSet<Counter> Tools;
typedef Button<BA02, Counter> LeftButton;
typedef Button<BD05, Counter> RightButton;
typedef RotaryEncoder<BD07, BD08, Rotary2PhaseMachine> Encoder;

Tools::FunctionTimeout<16> timeout;

const uint32_t PressInteval = 61000;
uint32_t nextPressing = 0;
int16_t xAxisOrigin = 0;
int16_t yAxisOrigin = 0;
int16_t maxStep = 10;
float originMax = 0;

typedef enum Menu {
  MenuCalibrartion = 0,
  MenuPressingState,
  MenuEmpty,
  MenuCount
} Menu;

Menu currentMenu = MenuCalibrartion;
bool pressingEnabled = false;

void renderMenu();

void sendKeys() {
  Keyboard.print("987654321");
  nextPressing = Counter::now() + PressInteval;
  timeout.schedule(PressInteval, sendKeys);
  renderMenu();
}

void handlerLeftButton(ButtonEvent event) {
  switch (event) {
    case ButtonEventPress:
      timeout.clear();
      nextPressing = Counter::now() + PressInteval;
      pressingEnabled = false;
      timeout.schedule(0, renderManuInterval);
    default:
      break;
  }
}

void handlerRightButton(ButtonEvent event) {
  switch (event) {
    case ButtonEventPress:
      timeout.clear();
      timeout.schedule(0, sendKeys);
      pressingEnabled = true;
      timeout.schedule(0, renderManuInterval);
    default:
      break;
  }
}

void handleEncoder(RotaryEvent event) {
  currentMenu = (currentMenu + 1) % MenuCount;
  renderMenu();
}

void renderCalibrartion() {
  display.println("Calibr-n");

  display.print("XO: ");
  display.println(xAxisOrigin);

  display.print("YO: ");
  display.println(yAxisOrigin);
}

void renderPresseingState() {
  display.print("Auto Press");
  display.println(pressingEnabled ? "Enabled" : "Disabled");
  if (pressingEnabled) {
    uint32_t t = (nextPressing - Counter::now()) / 1000;
    display.print("After: ");
    display.print(t);
    display.print("s");
  }
}

void renderMenu() {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.clearDisplay();

  switch (currentMenu) {
    case MenuCalibrartion:
      renderCalibrartion();
      break;
    case MenuPressingState:
      renderPresseingState();
      break;
    case MenuEmpty:
      display.clearDisplay();
      break;
    default:
      return;
  }

  display.display();
}

void renderManuInterval() {
  renderMenu();
  timeout.schedule(1000, renderManuInterval);
}

LeftButton leftButton;
RightButton rightButton;
Encoder encoder;

void calibrate() {
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);

  for (int i = 0; i < 16; i++) {
    xAxisOrigin += analogRead(A0);
    yAxisOrigin += analogRead(A1);
  }

  xAxisOrigin /= 16;
  yAxisOrigin /= 16;

  originMax = max(xAxisOrigin, yAxisOrigin);
}

void setup() {
  BD09::mode<Input>();
  BD08::mode<Output>();
  BD07::mode<Output>();

  leftButton.begin();
  rightButton.begin();
  encoder.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();

  calibrate();
  Keyboard.begin();

  timeout.schedule(0, renderManuInterval);
}

int value = 0;

void render() {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.clearDisplay();
  display.println(value);
  display.display();
}

void loop() {
  leftButton.handle(&handlerLeftButton);
  rightButton.handle(&handlerRightButton);
  encoder.handle(&handleEncoder);
  timeout.handle();
}

