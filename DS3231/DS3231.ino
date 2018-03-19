#include "AppConfig.hpp"
#include <IC/DS3231.hpp>
#include <IO/OutputStream.hpp>

using namespace MCURDK;
using namespace MCURDK::GPIO;
using namespace MCURDK::Board;
using namespace MCURDK::IC;
using namespace MCURDK::IO;

DS3231<I2C> RTC;

OutputStream<USART> cout;

uint8_t buffer[32];
uint8_t length = 0;

ISR(TWI_vect) {
  if (TW_STATUS == TW_MR_DATA_ACK) {
    buffer[length++] = TWDR;
  }

  I2C::handleIrq();
}

void setup() {
  USART::begin<CfgUSART>();
  I2C::begin<CfgI2C>();
  RTC.seconds(1);
  RTC.minutes(3);
  RTC.hours(7);
}

uint8_t prev = 0;

void loop() {
//  uint8_t v = RTC.seconds();
//  if (v != prev) {
//    prev = v;
////    cout << RTC.hours() << ":" << RTC.minutes() << ":" << RTC.seconds() << endl;
//  }

  length = 0;
  //  RTC.seconds();

  Delay::milliseconds(500);
  RTC.fetch();
}

