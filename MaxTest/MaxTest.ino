#include "MotorPump.h"
#include <ARDK.h>

using namespace ARDK::IO;
using namespace ARDK::IC;

const uint8_t DeviceCount = 4;

/*
  pin 12 is connected to the DataIn
  pin 11 is connected to the CLK
  pin 10 is connected to LOAD
*/

typedef CustomSPI<D12, NAP, D11> SPI;
typedef MAX7221<SPI, D10> Device;
Device max7221;
MAX7221Data<DeviceCount> matrix;

int index = 0;

void setup() {
  Serial.begin(9600);
  matrix.clear();

  max7221.begin();
  max7221(DeviceCount, Device::TestOff);

  max7221() << Device::On << Device::On << Device::On << Device::On;
  //  max7221(DeviceCount, Device::On);

  //max7221() << Device::N(DeviceCount, Device::Intencity | 0);
  max7221() << (Device::Intencity | 0x0)
            << (Device::Intencity | 0x4)
            << (Device::Intencity | 0x8)
            << (Device::Intencity | 0xC);
}

void loop() {
  uint8_t frame[8];
  for (int i = 0; i < 8; i++) {
    frame[i] = pgm_read_byte_near(&IMAGES[0][0] + index * 8 + i);
  }

  index = (index + 1) % IMAGES_LEN;

  matrix.pushRows(frame);
  max7221.display(matrix);
  delay(70);
}

