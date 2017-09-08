#include <ARDK.h>
#include <Board/Auto.hpp>
#include <IO/Button.hpp>
#include <IC/TM1637.hpp>
#include <Data/Segment7.hpp>
#include <Framework/AvrLibC.hpp>
#include <Utils/Timeout.hpp>

using namespace ARDK;
using namespace ARDK::GPIO;
using namespace ARDK::IO;
using namespace ARDK::Data;
using namespace ARDK::Board;
using namespace ARDK::Utils;
using namespace ARDK::Framework;

typedef IC::TM1637<BD02, BD03> Display;
Display display;

void setup() {
  display.begin();
  display() << Display::CmdPulseWidth0;
}

void loop() {
  display() << Display::CmdSetAddress0
            << Segment7::abcdASCII('D')
            << Segment7::abcdASCII('E')
            << Segment7::abcdASCII('A')
            << Segment7::abcdASCII('D');
  delay(500);
  
  display() << Display::CmdSetAddress0
            << Segment7::abcdASCII('B')
            << Segment7::abcdASCII('E')
            << Segment7::abcdASCII('E')
            << Segment7::abcdASCII('F');
  delay(500);
}

