#pragma once

#ifndef APP_CONFIG_HPP
#define APP_CONFIG_HPP

#include <MCURDK.h>
#include <Board/Auto.hpp>
#include <Utils/MillisecondsCounter.hpp>
#include <Utils/ToolSet.hpp>
#include <IO/Button.hpp>
#include <IC/MAX72xx.hpp>
#include <IC/DS3231.hpp>
#include <Arch/AtmelAVR/HardwareI2C.hpp>

using namespace MCURDK;
using namespace MCURDK::GPIO;

const constexpr uint8_t DeviceCount = 4;

typedef uint32_t CounterType;
typedef MCURDK::Utils::MillisecondsCounter<CounterType, &timer0_millis> Counter;
typedef MCURDK::Utils::ToolSet<Counter> Tools;
typedef Tools::FunctionTimeout<16, int8_t> Timeout;
typedef Tools::Delay Delay;

typedef USARTConfig<F_CPU, 19200> CfgUSART;
typedef MCURDK::Board::MCU::HardwareUSART0<32, 4> USART;

typedef MCURDK::IC::MAX72xx<MCURDK::Board::HardwareSPI1, MCURDK::Board::BD10> MAX7219;
typedef MCURDK::IC::MAX72xxData<DeviceCount> Matrix;

typedef MCURDK::Arch::AtmelAVR::I2CConfig<F_CPU> CfgI2C;
typedef MCURDK::Board::MCU::HardwareMasterI2C<32> I2C;

typedef MCURDK::IC::DS3231<I2C> RTC;


template <class Pin> 
using Button = typename ::MCURDK::IO::Button<Pin, Counter, ::MCURDK::IO::PullUpButtonNoPress>;

typedef Button<MCURDK::Board::BD02> Button1;
typedef Button<MCURDK::Board::BD03> Button2;
typedef Button<MCURDK::Board::BD04> Button3;
typedef Button<MCURDK::Board::BD05> Button4;
typedef Button<MCURDK::Board::BD06> Button5;
typedef Button<MCURDK::Board::BD07> Button6;
typedef Button<MCURDK::Board::BD08> Button7;
typedef Button<MCURDK::Board::BD09> Button8;
typedef Button<MCURDK::Board::BA06> Button9;

ISR(USART_RX_vect) {
  USART::handleRxIrq();
}

ISR(USART_UDRE_vect) {
  USART::handleTxIrq();
}

ISR(TWI_vect) {
  I2C::handleIrq();
}

#endif

