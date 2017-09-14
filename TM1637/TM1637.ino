#include <MCURDK.h>
#include <Board/Auto.hpp>
#include <IO/Button.hpp>
#include <IC/TM1637.hpp>
#include <Data/Segment7.hpp>
#include <Utils/Timeout.hpp>
#include <Utils/Tools.hpp>

using namespace MCURDK;
using namespace MCURDK::GPIO;
using namespace MCURDK::IO;
using namespace MCURDK::Data;
using namespace MCURDK::Board;
using namespace MCURDK::Utils;

extern uint32_t timer0_millis;

typedef Utils::Tools<uint32_t, &timer0_millis> Tools;
typedef IC::TM1637<BD02, BD03, typename ::Tools::Delay> Display;
Display display;

void setup() {
  display.begin();
  display() << Display::CmdPulseWidth0;
}

void loop() {
  display() << Display::CmdWriteData;
  display() << Display::CmdSetAddress0
            << Segment7::abcdASCII('D')
            << Segment7::abcdASCII('E')
            << Segment7::abcdASCII('A')
            << Segment7::abcdASCII('D');
  display() << Display::CmdPulseWidth0;
  delay(500);

  display() << Display::CmdSetAddress0
            << Segment7::abcdASCII('B')
            << Segment7::abcdASCII('E')
            << Segment7::abcdASCII('E')
            << Segment7::abcdASCII('F');
  delay(500);
  display() << Display::CmdPulseWidth5;
}

