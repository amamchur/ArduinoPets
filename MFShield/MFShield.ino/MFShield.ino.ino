#include <MCURDK.h>
#include <MCURDK/Board/Auto.hpp>
#include <MCURDK/IO/Button.hpp>
#include <MCURDK/IO/IRRemoteReceiver.hpp>
#include <MCURDK/IO/OutputStream.hpp>
#include <MCURDK/Utils/ToolSet.hpp>
#include <MCURDK/Utils/MillisecondsCounter.hpp>
#include <MCURDK/Utils/Prescalers.hpp>
#include <MCURDK/Utils/OverflowPeriod.hpp>
#include <MCURDK/Shields/UnoMultiFunctionalShield.hpp>

using namespace MCURDK;
using namespace MCURDK::Board;
using namespace MCURDK::IO;
using namespace MCURDK::IC;
using namespace MCURDK::Data;
using namespace MCURDK::GPIO;

typedef MCURDK::Utils::MillisecondsCounter<decltype(timer0_millis), &timer0_millis> Counter;
typedef MCURDK::Utils::ToolSet<MCURDK::Board::MCU, Counter> Tools;
typedef Tools::Delay Delay;
typedef Tools::FunctionTimeout<16, int16_t> Timeout;
typedef MCURDK::Shields::UnoMultiFunctionalShield<Tools> Shield;

typedef MCURDK::Board::MCU::HardwareUSART0<32, 8> USART;
typedef MCURDK::Board::MCU::USARTConfig<F_CPU, 57600> USARTCfg;

typedef Timer2 IRTimer;
typedef MCURDK::Utils::PrescalerLE<IRTimer, 4>::Result IRPrescaler;
typedef MCURDK::Utils::OverflowPeriod<Board::MCU::Frequency, IRPrescaler::Value, 256> Period;

Shield mfs;
Timeout timeout;
IO::OutputStream<USART> cout;
IO::IRRemoteReceiver<BD02, Period::Microseconds> receiver;

ISR(USART_RX_vect) {
  USART::handleRxIrq();
}

ISR(USART_UDRE_vect) {
  USART::handleTxIrq();
}

ISR(TIMER2_OVF_vect) {
  receiver.handle();
}

void setup() {
  USART::begin<USARTCfg>();

  IRTimer::mode<MCURDK::Periph::TimerMode::FastPWM8bit>();
  IRTimer::selectClockSource<IRPrescaler>();
  IRTimer::enableOverflowInterrupt();

  BD05::mode<PinMode::Output>();
  BD06::mode<PinMode::Output>();
  BA05::mode<PinMode::Input>();

  MCURDK::Utils::Interrupts::on();

  receiver.begin();
  mfs.begin();
  mfs.beep();
  mfs.display(0xFF, 0xFF, 0xFF, 0xFF);

  cout << "Start\r\n";
}

void handleIrCommand(uint32_t v) {
  switch (v) {
    case 1: // Repeat command
      return;
    default:
      cout << hex << v << endl;
      //      mfs.hexToSegments(v & 0xFFFF);
      break;
  }
}

uint8_t motionValue = 0;

void loop() {
  if (receiver.processed()) {
    auto v = receiver.result();
    receiver.run();
    handleIrCommand(v);
  }

  mfs.dynamicIndication();
  mfs.handle();
  timeout.handle();
}

