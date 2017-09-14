#include <MCURDK.h>
#include <Board/Auto.hpp>
#include <Utils/Tools.hpp>

using namespace MCURDK;
using namespace MCURDK::GPIO;
using namespace MCURDK::Board;

typedef Utils::Tools<uint32_t, &timer0_millis> Tools;
typedef typename ::Tools::Delay Delay;

void setup() {
  BuildInLed::mode<Output>();
}

void loop() {
  BuildInLed::high();
  Delay::milliseconds(500);
  BuildInLed::low();
  Delay::milliseconds(500);
}

