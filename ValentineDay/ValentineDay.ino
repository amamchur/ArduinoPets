#include "LedControl.h"
#include "font8x8_basic.h"

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.
 */
 #define DeviceCount 4
 
LedControl lc = LedControl(12, 11, 10, DeviceCount);

uint8_t data[DeviceCount][8] = {0xFF};

unsigned long delaytime = 35;

void pushColumn(uint8_t col) {
  uint8_t carryFlags[8];;
  for (int i = 0; i < 8; i++) {
    carryFlags[i] = col & 1;
    col >>= 1;
  }
  
  for (uint8_t i = 0; i < DeviceCount; i++) {
    for (uint8_t j = 0; j < 8; j++) {
      uint8_t tmp = (data[i][j] & 0x80) >> 7;
      data[i][j] = data[i][j] << 1 | carryFlags[j];
      carryFlags[j] = tmp;
    }
  }
}

void setup() {
  memset(data, 0, sizeof(data));
  
  for (int i = 0; i < lc.getDeviceCount(); i++) {
    lc.shutdown(i,false);
    lc.setIntensity(i, 8);
    lc.clearDisplay(i);
   }
}

int bt = 0;
int index = 0 ;
//
//char msg[] = "Kill All Humans \u007f\u007f\u007f";
//uint8_t kerning[sizeof(msg)] = {7, 5, 5, 5, 5, 7, 5, 5, 5, 7, 7, 7, 7, 7, 7, 5, 9, 9, 9, 7};

char msg[] = "Welcome To Engineering Suckers";
uint8_t kerning[sizeof(msg)] = { 7, 6, 5, 7, 7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  7, 7};

//char msg[] = "Be My Valentine \u007f\u007f\u007f";
//uint8_t kerning[sizeof(msg)] = {7, 7, 2, 7, 7, 2, 6, 6, 5, 6, 6, 6, 6, 6, 6, 2, 7, 7, 7, 7};

//char msg[] = "Happy Valentine's Day \u007f\u007f\u007f";

//uint8_t kerning[sizeof(msg)] = {
//  6, 7, 7, 7, 7, 4,                     // 'Happy '
//  6, 6, 5, 6, 5, 6, 6, 6, 6, 3, 7, 3,   // 'Valentine's '
//  7, 7, 7, 3,                           // 'Day '
//  7, 7, 7, 7                            // '\u007f\u007f\u007f '
//};

//welcome to engineering

int msgLength = sizeof(msg) - 1;

void displayScreen() {
  for (int i = 0; i < DeviceCount; i++)  {
    for (int j = 0; j < 8; j++)  {
      lc.setRow(i, j, data[i][j]);
    }
  }
}

void pushChar(char ch, int b) {
  if (b > 7) {
    pushColumn(0);
    return;
  }
  
  char *chs = font8x8_basic[ch];
  uint8_t r = 0;
  for (int i = 7; i >= 0; i--) {
    r |= ((chs[i] >> b) & 1) << i;
  }
  pushColumn(r);
}

void loop() {
  displayScreen();
  delay(delaytime);
  pushChar(msg[index], bt);
  
  bt++;
  if (bt > kerning[index]) {
    bt = 0;
    index++;
  }

  if (index > msgLength) {
    index = 0;
  }
}

