//
//#include "Mouse.h"
#include "Keyboard.h"

#include <MCURDK.h>
#include <MCURDK/Board/ArduinoLeonardo.hpp>
#include <MCURDK/Utils/ToolSet.hpp>
#include <MCURDK/Utils/MillisecondsCounter.hpp>
#include <MCURDK/IO/Button.hpp>
#include <MCURDK/IO/RotaryEncoder.hpp>

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
typedef Utils::ToolSet<PCB::MCU, Counter> Tools;
typedef Button<PCB::BD08, Counter> LeftButton;
typedef Button<PCB::BD09, Counter> RightButton;
typedef RotaryEncoder<PCB::BD07, PCB::BD06, Rotary2PhaseMachine> Encoder;

::Tools::FunctionTimeout<16> timeout;

const uint32_t PressInteval = 61000;
uint32_t nextPressing = 0;
int16_t xAxisOrigin = 0;
int16_t yAxisOrigin = 0;
int16_t maxStep = 10;
float originMax = 0;

typedef enum Menu {
  MenuPressingState = 0,
  MenuEmpty,
  MenuCount
} Menu;

Menu currentMenu = MenuPressingState;
bool pressingEnabled = false;

void renderMenu();

void sendKeys() {
  Keyboard.print("98765");
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
      timeout.schedule(0, renderMenuInterval);
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
      timeout.schedule(0, renderMenuInterval);
    default:
      break;
  }
}

void handleEncoder(RotaryEvent) {
  currentMenu = (Menu)((currentMenu + 1) % MenuCount);
  renderMenu();
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

void renderMenuInterval() {
  renderMenu();
  timeout.schedule(1000, renderMenuInterval);
}

LeftButton leftButton;
RightButton rightButton;
Encoder encoder;

void setup() {
  PCB::BD09::mode<PinMode::Input>();
  PCB::BD08::mode<PinMode::Output>();
  PCB::BD07::mode<PinMode::Output>();

  leftButton.begin();
  rightButton.begin();
  encoder.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();

  Keyboard.begin();

  timeout.schedule(0, renderMenuInterval);
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

