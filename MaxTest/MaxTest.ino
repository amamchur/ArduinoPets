#include "LedControl.h"
#include "font8x8_basic.h"
#include <ARDK.h>
using namespace ARDK::IO;
using namespace ARDK::IC;


/*
  Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
  pin 12 is connected to the DataIn
  pin 11 is connected to the CLK
  pin 10 is connected to LOAD
  We have only a single MAX72XX.
*/
#define DeviceCount 4

MAX7221<D12, D10, D11> max7221;
MAX7221Matrix<4> matrix;

LedControl lc = LedControl(12, 11, 10, DeviceCount);
//
void setup() {
  max7221.begin();

  for (int i = 0; i < lc.getDeviceCount(); i++) {
    lc.shutdown(i, false);
    lc.setIntensity(i, 0);
    lc.clearDisplay(i);
  }

  matrix.clear();
  matrix.pushColumn(0xAA);
  matrix.pushColumn(0xF0);
 
  max7221.display(matrix);
}

void loop() {
}

