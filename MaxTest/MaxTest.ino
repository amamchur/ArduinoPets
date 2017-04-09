#include "MotorPump.h"
#include <ARDK.h>

using namespace ARDK::IO;
using namespace ARDK::IC;

const uint8_t DeviceCount = 4;

/* Custom SPI configuration
 * Exchange Clock Pin 13 to 12
 * pin 11 -> DIN (MOSI)
 * NAP = Not A Pin (ignore MISO)
 * pin 12 -> CLK
*/
//typedef CustomSPI<D11, NAP, D12> SPI;

/*
  Native SPI (Arduino UNO Pins) as Custom SPI
  See https://www.arduino.cc/en/reference/SPI
  pin 11 -> DIN (MOSI)
  pin 12 -> not connected (MISO)
  pin 13 -> CLK
*/
typedef CustomSPI<D11, D12, D13> SPI;

/*
 * Navive Arduino UNO SPI
 */

//typedef MasterAvrSPI<1> SPI;

/* pin 10 is connected to LOAD (CS) */
typedef MAX7221<SPI, D10> Device;

Device max7221;
MAX7221Data<DeviceCount> matrix;

int index = 0;

void setup() {
  max7221.begin().init(DeviceCount);
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

