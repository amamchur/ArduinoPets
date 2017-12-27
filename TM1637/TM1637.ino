#include <MCURDK.h>
#include <Board/Auto.hpp>
#include <IO/Button.hpp>
#include <IC/TM1637.hpp>
#include <Data/Segment7.hpp>
#include <Utils/Timeout.hpp>
#include <Utils/ToolSet.hpp>
#include <Utils/MillisecondsCounter.hpp>

using namespace MCURDK;
using namespace MCURDK::GPIO;
using namespace MCURDK::MCU;
using namespace MCURDK::IO;
using namespace MCURDK::Board;
using namespace MCURDK::Utils;
using namespace MCURDK::Data;

extern uint32_t timer0_millis;

typedef MillisecondsCounter<typeof(timer0_millis), &timer0_millis> Counter;
typedef Utils::ToolSet<Counter> Tools;
typedef IC::TM1637<BD02, BD03, typename ::Tools::Delay> Display;
Display display;

void setup() {
  Serial.begin(9600);
  display.begin();
  display() << Display::CmdPulseWidth0;
}

void loop() {
  uint16_t value = analogRead(A0);
  Serial.println(value);
  
  display() << Display::CmdSetAddress0
            << Segment7::abcdHex((value >> 12) & 0xF)
            << Segment7::abcdHex((value >> 8) & 0xF)
            << Segment7::abcdHex((value >> 4) & 0xF)
            << Segment7::abcdHex((value >> 0) & 0xF);
  delay(500);
  display() << Display::CmdPulseWidth5;
}

