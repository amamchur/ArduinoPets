
#include <IRremote.h>

#include <MCURDK.h>
#include <MCURDK/Board/Auto.hpp>
#include <MCURDK/IO/Button.hpp>
#include <MCURDK/Utils/ToolSet.hpp>
#include <MCURDK/Utils/MillisecondsCounter.hpp>
#include <MCURDK/Shields/UnoMultiFunctionalShield.hpp>
#include <MCURDK/IC/WS2812.hpp>

IRrecv irrecv(2);

using namespace MCURDK;
using namespace MCURDK::Board;
using namespace MCURDK::IO;
using namespace MCURDK::IC;
using namespace MCURDK::Data;
using namespace MCURDK::GPIO;

typedef BD05 LedDataIn;
typedef MCURDK::Utils::MillisecondsCounter<decltype(timer0_millis), &timer0_millis> Counter;
typedef MCURDK::Utils::ToolSet<MCURDK::Board::MCU, Counter> Tools;
typedef Tools::Delay Delay;
typedef Tools::FunctionTimeout<16, int16_t> Timeout;
typedef MCURDK::Shields::UnoMultiFunctionalShield<Tools> Shield;

Shield mfs;
Timeout timeout;

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();

  mfs.begin();
  mfs.beep();
  mfs.hexToSegments(0xF0f0);
}

decode_results results;

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    irrecv.resume(); // Receive the next value
  }

  mfs.dynamicIndication();
  mfs.handle();
  timeout.handle();
}

