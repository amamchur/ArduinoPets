#include <ARDK.h>
#include <Board/Auto.hpp>
#include <Framework/AvrLibC.hpp>

using namespace ARDK;
using namespace ARDK::GPIO;
using namespace ARDK::Board;
using namespace ARDK::Framework;

constexpr uint32_t ClockCyclesPerMicrosecond = F_CPU / 1000000L;
constexpr uint32_t ClockCyclesToMicroseconds(uint32_t a) {
  return (a * 1000L) / (F_CPU / 1000L);
}

constexpr uint32_t MicrosecondsOverflowPerTimer0 = ClockCyclesToMicroseconds(64 * 256);
constexpr uint32_t MillisInc = MicrosecondsOverflowPerTimer0 / 1000;
constexpr uint8_t FractInc = (MicrosecondsOverflowPerTimer0 % 1000) >> 3;
constexpr uint8_t FractMax = 1000 >> 3;

volatile uint32_t timer0_millis = 0;
volatile uint8_t timer0_fract = 0;

ISR(TIMER0_OVF_vect) {
//  uint32_t m = timer0_millis;
//  uint8_t f = timer0_fract;
//
//  m += MillisInc;
//  f += FractInc;
//  if (f >= FractMax) {
//    f -= FractMax;
//    m += 1;
//  }
//
//  timer0_fract = f;
//  timer0_millis = m;

  BD13::toggle();
}

Delay d;

uint32_t getMillis() {
  uint32_t m;
  uint8_t oldSREG = SREG;

  // disable interrupts while we read timer0_millis or we might get an
  // inconsistent value (e.g. in the middle of a write to timer0_millis)
  cli();
  m = timer0_millis;
  SREG = oldSREG;

  return m;
}

int main() {
  //TCCR0B |= (1 << CS01) | (1 << CS00);
  sei();
  TCCR0A |= 1 << WGM01;
  TCCR0A |= 1 << WGM00;
  TCCR0B |= TCCR0B << CS01;
  TCCR0B |= TCCR0B << CS00;
  TIMSK0 |= 1 << TOIE0;

  BD13::mode<Output>();
  uint32_t prev = 0;
  for (int i = 0; i < 300; i++) {
    asm volatile("nop");
  }

  while (true) {
//    uint32_t ms = getMillis();
//    if (ms - prev > 500) {
//      BD13::toggle();
//      prev = ms;
//    }
//    d.milliseconds(200);
//    BD13::toggle();
    //    d.milliseconds(200);
    //    BD13::high();
  }
  return 0;
}

