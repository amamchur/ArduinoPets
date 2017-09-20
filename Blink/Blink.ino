#include <MCURDK.h>
#include <Board/Auto.hpp>
#include <Utils/ToolSet.hpp>
#include <Utils/MillisecondsCounter.hpp>

using namespace MCURDK;
using namespace MCURDK::GPIO;
using namespace MCURDK::Board;
using namespace MCURDK::Utils;

//typedef MillisecondsCounter<typeof(timer0_millis), &timer0_millis> Counter;
//typedef Utils::ToolSet<Counter> Tools;
//typedef typename ::Tools::Delay D;

void setup() {
  Serial.begin(9600);
  Serial.println("Hello");
//  BuildInLed::mode<Output>();
}

void loop() {
  Serial.println("Hello");
//  BuildInLed::high();
//  D::milliseconds(500);
//  BuildInLed::low();
//  D::milliseconds(500);
}

