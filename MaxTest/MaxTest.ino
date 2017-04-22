#include "MotorPump.h"
#include <ARDK.h>

using namespace ARDK::IO;
using namespace ARDK::IC;

const uint8_t DeviceCount = 4;

/* Custom SPI configuration
   Exchange Clock Pin 13 to 12
   pin 11 -> DIN (MOSI)
   NAP = Not A Pin (ignore MISO)
   pin 12 -> CLK
*/
//typedef CustomSPI<D11, NAP, D12> SPI;

/*
  Native SPI pins (Arduino UNO Pins) as Custom SPI
  See https://www.arduino.cc/en/reference/SPI
  pin 11 -> DIN (MOSI)
  pin 12 -> not connected (MISO)
  pin 13 -> CLK
*/
//typedef CustomSPI<D11, D12, D13> SPI;

/*
   Navive Arduino UNO SPI
*/
typedef MasterAvrSPI<MSBFIRST> SPI;

/* pin 10 is connected to LOAD (CS) */
typedef MAX72xx<SPI, D10> Device;

Device max7221;
MAX72xxData<DeviceCount> matrix;

int index = 0;
uint8_t frame[8];

void pushFrame(int index) {
  for (int i = 0; i < 8; i++) {
    frame[i] = pgm_read_byte_near(&IMAGES[0][0] + index * 8 + i);
  }

  matrix.pushRows(frame);
}

void setup() {
  Serial.begin(9600);
  D08::mode(OUTPUT);

  max7221.begin().init(DeviceCount);
  max7221() << (Device::Intencity | 0x0)
            << (Device::Intencity | 0x0)
            << (Device::Intencity | 0x0)
            << (Device::Intencity | 0x0);
  matrix.clear();
  max7221.display(matrix);
}

void loop() {
  max7221.display(matrix);
  index = (index + 1) % IMAGES_LEN;
  pushFrame(index);
  delay(50);
}

