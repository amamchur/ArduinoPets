#pragma once

#ifndef APP_CONFIG_HPP
#define APP_CONFIG_HPP

#include <MCURDK.h>
#include <Board/Auto.hpp>
#include <Utils/MillisecondsCounter.hpp>
#include <Utils/ToolSet.hpp>

using namespace MCURDK;
using namespace MCURDK::GPIO;

typedef uint32_t CounterType;
typedef MCURDK::Utils::MillisecondsCounter<CounterType, &timer0_millis> Counter;
typedef MCURDK::Utils::ToolSet<MCURDK::Board::MCU, Counter> Tools;
typedef Tools::FunctionTimeout<8, int8_t> Timeout;
typedef Tools::Delay Delay;

typedef MCURDK::Periph::USARTConfig<F_CPU, 19200> CfgUSART;
typedef MCURDK::Board::MCU::HardwareUSART0<32, 32> USART;

typedef MCURDK::Periph::I2CConfig<F_CPU> CfgI2C;
typedef MCURDK::Board::MCU::HardwareMasterI2C<32> I2C;

ISR(USART_RX_vect) {
  USART::handleRxIrq();
}

ISR(USART_UDRE_vect) {
  USART::handleTxIrq();
}

#endif

