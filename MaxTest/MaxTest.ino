#include "LedControl.h"
#include "MotorPump.h"
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

//#include "font8x8_basic.h"

MAX7221<D12, D10, D11> max7221;
MAX7221Data<4> matrix;

LedControl lc = LedControl(12, 11, 10, DeviceCount);
int index = 0;

void setup() {
  max7221.begin();

  for (int i = 0; i < lc.getDeviceCount(); i++) {
    lc.shutdown(i, false);
    lc.setIntensity(i, 0);
    lc.clearDisplay(i);
  }

  matrix.clear();
  //  matrix.data[0][0] = ch[0];
  //  matrix.data[0][1] = ch[1];
  //  matrix.data[0][2] = ch[2];
  //  matrix.data[0][3] = ch[3];
  //  matrix.data[0][4] = ch[4];
  //  matrix.data[0][5] = ch[5];
  //  matrix.data[0][6] = ch[6];
  //  matrix.data[0][7] = ch[7];
  max7221.display(matrix);
}

void loop() {
  uint8_t frame[8];
  for (int i = 0; i < 8; i++) {
    matrix.data[0][i] = pgm_read_byte_near(&IMAGES[0][0] + index * 8 + i);
  }
  
//  matrix.setRows(0, frame);
  max7221.display(matrix);
  delay(70);

  index++;
  index %= IMAGES_LEN;
}

