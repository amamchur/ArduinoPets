#include <ARDK.h>

using namespace ARDK::IO;
using namespace ARDK::IC;
using namespace ARDK::Data;

typedef MasterAvrSPI<MSBFIRST> SPI;
typedef MAX72xx<SPI, D10> Device;

const uint8_t DeviceCount = 1;
Device max7221;
MAX72xxData<DeviceCount> matrix;

void setup() {
  Serial.begin(9600);
  D08::mode(OUTPUT);

  max7221.begin().init(DeviceCount);
  max7221() << Device::N(DeviceCount, Device::Intencity5);
  //max7221() << Device::N(DeviceCount, Device::DecodeMode | 0xFF);

  //  double d = 312.123;
  //  matrix.print(d, 3);
  //  matrix.data[0][0] = Segment7::msbf(8);
  //  matrix.data[0][1] = Segment7::msbf(9);
  //  matrix.data[0][2] = Segment7::msbf(10);
  //  matrix.data[0][3] = Segment7::msbf(11);
  //  matrix.data[0][4] = Segment7::msbf(12);
  //  matrix.data[0][5] = Segment7::msbf(13);
  //  matrix.data[0][6] = Segment7::msbf(14);
  //  matrix.data[0][7] = Segment7::msbf(15);
  matrix.clear().printHex(Segment7::msbf, 0xDEADDEAD);
  max7221.display(matrix);
}

uint32_t value = 0;

void loop() {
  matrix.printHex(Segment7::msbf, value++);
  //  matrix.print(millis() / 1000.0f, 3);
  max7221.display(matrix);
  D08::toggle();
  //  delay(10);
}
