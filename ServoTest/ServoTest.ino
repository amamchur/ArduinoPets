#include <MCURDK.h>
#include <Board/Auto.hpp>
#include <Cfg/ArduinoATmega8.hpp>
#include <IO/OutputStream.hpp>
#include <IO/MemStream.hpp>

#include <Utils/PeriodCalculator.hpp>
#include <Utils/TicksCalculator.hpp>

using namespace MCURDK;
using namespace MCURDK::GPIO;
using namespace MCURDK::Board;
using namespace MCURDK::IO;

OutputStream<USART> cout;

typedef MCURDK::Board::MCU::Timer2 Timer;

volatile uint16_t counter = 0;

typedef MCURDK::Utils::TicksCalculator<Timer, F_CPU, 4> TicksCalc;
typedef MCURDK::Utils::PrescalerLE<Timer, TicksCalc::CyclesPerTick> MP;
const uint32_t ServoPeriodMicrosec = 1000000 / 50;

const uint32_t MinValueMicrosec = 544;
const uint32_t MaxValueMicrosec = 2400;

enum class ServoState : uint8_t {
  Active,
  Inactive,
  Idle
};

class AbstractServo {
  public:
    ServoState state;
    uint16_t ticks;
    uint16_t value;
    virtual void low() = 0;
    virtual void high() = 0;
};

template <class Pin, uint16_t min = 544, uint16_t max = 2400>
class Servomotor : public AbstractServo {
  public:
    virtual void low() {
      Pin::low();
    }

    virtual void high() {
      Pin::high();
    }
};

template <class Tmr, uint32_t freq, uint32_t ServoPeriod = 20000, uint32_t DeadBandWidthMicrosec = 4>
class ServoTiming {
  public:
    typedef Tmr Timer;

    static constexpr uint32_t CyclesPerMicrosecond = freq / 1000000;
    static constexpr uint32_t CyclesPerTick = DeadBandWidthMicrosec * CyclesPerMicrosecond;

    typedef MCURDK::Utils::PrescalerLE<Timer, CyclesPerTick> BestPrescaler;
    static constexpr uint32_t Prescale = BestPrescaler::Result::Value;

    static constexpr uint32_t TicksPerSecond = freq / Prescale;
    static constexpr uint32_t TicksPerServoPeriod = TicksPerSecond / (1000000 / ServoPeriod);
    static constexpr uint32_t NanosecPerTick = 1000000000 / TicksPerSecond;
    static constexpr uint32_t MaxDurationNanosec = NanosecPerTick * (1 << Timer::Resolution);

    static constexpr uint32_t MinTicks = (MinValueMicrosec * 1000) / NanosecPerTick;
    static constexpr uint32_t MaxTicks = (MaxValueMicrosec * 1000) / NanosecPerTick;
    static constexpr uint32_t IdleTicks = TicksPerServoPeriod - MaxTicks;
    static constexpr uint32_t MaxServos = TicksPerServoPeriod / MaxTicks;
    static constexpr uint32_t ServoSlotTicks = TicksPerServoPeriod / MaxServos;
    static constexpr uint32_t IdleSlotTicks = TicksPerServoPeriod - ServoSlotTicks * MaxServos;
};

template <class ServoTmr, uint8_t Channel>
class ServoChannel {
  public:
    typedef ServoTmr Timing;
    typedef typename ServoTmr::Timer Timer;

    ServoChannel()
      : count(0)
      , current(0) {
    }

    void addServo(AbstractServo *servo) {
      servos[count++] = servo;
    }

    void handleCompareMatchIrq() {
      AbstractServo *servo = servos[current];
      typename Timer::Word ocrv = Timer::MaxValue;
      
      switch (servo->state) {
        case ServoState::Active:
          if (Timer::MaxValue > servo->ticks) {
            ocrv = servo->ticks;
            servo->ticks = 0;
          } else {
            servo->ticks -= Timer::MaxValue;
          }
          break;
        case ServoState::Idle:
          break;
      }

      ocrv += Timer::counter();
      Timer::template ocrValue<Channel>(ocrv);
    }

    AbstractServo *servos[ServoTmr::MaxServos];
    uint8_t count;
    uint8_t current;
};

typedef ServoTiming<Timer, F_CPU, ServoPeriodMicrosec, 4> ST;
ISR(TIMER2_OVF_vect) {
  counter++;
}

Servomotor<BD11> servo1;

AbstractServo *sss[] = {&servo1};
ServoChannel<ST, 0> sc;

void setup() {
  sc.addServo(&servo1);
  sc.handleCompareMatchIrq();

  Timer::reset();

  BD10::mode<PinMode::Output>();

  USART::begin<CfgUSART>();

  cout << "CyclesPerMicrosecond:  " << ST::CyclesPerMicrosecond << endl;
  cout << "CyclesPerTick:         " << ST::CyclesPerTick << endl;
  cout << "TicksPerSecond:        " << ST::TicksPerSecond << endl;
  cout << "TicksPerServoPeriod:   " << ST::TicksPerServoPeriod << endl;
  cout << "NanosecPerTick:        " << ST::NanosecPerTick << endl;
  cout << "MaxDurationNanosec:    " << ST::MaxDurationNanosec << endl;
  cout << "MinTicks:              " << ST::MinTicks << endl;
  cout << "MaxTicks:              " << ST::MaxTicks << endl;
  cout << "IdleTicks:             " << ST::IdleTicks << endl;
  cout << "MaxServos:             " << ST::MaxServos << endl;
  cout << "ServoSlotTicks:        " << ST::ServoSlotTicks << endl;
  cout << "IdleSlotTicks:         " << ST::IdleSlotTicks << endl;

  //  Timer::prescaler<MP::ResultIndex>();
  //  Timer::start();
}

void loop() {
  //  delay(1000);
  //  cout << counter << endl;
  //  counter = 0;
}


