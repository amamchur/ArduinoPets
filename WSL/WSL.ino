#include "MCURDK.h"
#include "Board/Auto.hpp"
#include "IO/Button.hpp"
#include "IC/WS2812.hpp"
#include "IO/OutputStream.hpp"
#include "Utils/ToolSet.hpp"
#include "Utils/MillisecondsCounter.hpp"
#include "Utils/Prescalers.hpp"

using namespace MCURDK;
using namespace MCURDK::Board;
using namespace MCURDK::IO;
using namespace MCURDK::IC;
using namespace MCURDK::Data;
using namespace MCURDK::GPIO;
using namespace MCURDK::Periph;

typedef MCURDK::Utils::MillisecondsCounter<decltype(timer0_millis), &timer0_millis> Counter;
typedef MCURDK::Utils::ToolSet<MCURDK::Board::MCU, Counter> Tools;
typedef Tools::Delay Delay;
typedef typename Tools::template FunctionTimeout<8> Timeout;
typedef MCURDK::Utils::PrescalerLE<Timer1, 1>::Result Timer1Prescaler;
typedef MCURDK::Utils::PrescalerLE<Timer2, 1>::Result Timer2Prescaler;
typedef MCURDK::Board::MCU::USARTConfig<F_CPU, 9600> USARTCfg;
typedef MCURDK::Board::MCU::HardwareUSART0<16, 8> USART;

typedef BD09PWM SegL;
typedef BD10PWM SegM;
typedef BD11PWM SegR;

const uint8_t PwdMode[] = {0, 5, 10, 30, 70, 150, 255};
const uint8_t PwdModeCount = sizeof(PwdMode) / sizeof(PwdMode[0]);

OutputStream<USART> cout;

uint8_t nextValue(uint8_t current) {
  cout << "PwdModeCount: " << PwdModeCount << "\n";

  uint8_t next = PwdMode[PwdModeCount - 1];
  for (int i = PwdModeCount - 1; i >= 0; i--) {
    if (PwdMode[i] < current) {
      next = PwdMode[i];
      break;
    }
  }

  cout << "current: " << current << "\n";
  cout << "next: " << next << "\n";

  return next;
}

ISR(USART_RX_vect) {
  USART::handleRxIrq();
}

ISR(USART_UDRE_vect) {
  USART::handleTxIrq();
}

typedef struct Pixel {
  uint8_t green;
  uint8_t red;
  uint8_t blue;
  uint8_t white;
} Pixel;

const int16_t PixelCount = 134;
Pixel renderBuffer[PixelCount];

typedef BD05 LedDataIn;
typedef TransmitterWS2812<LedDataIn, F_CPU, TimingSK6812> Transmitter;

class NeoPixel : public WS2812<Transmitter, typename Tools::Delay> {
  public:
    inline void display() {
      send(renderBuffer, sizeof(renderBuffer));
    }
};

NeoPixel neoPixel;
Timeout timeout;

Button<BA03, Counter> button1;
Button<BA04, Counter> button2;
Button<BA00, Counter> button3;
Button<BA02, Counter> button4;
Button<BA05, Counter> button5;
Button<BA01, Counter> button6;

void initializePWM() {
  Timer1::reset();
  Timer1::mode<TimerMode::FastPWM8bit>();
  Timer1::selectClockSource<Timer1Prescaler>();

  Timer2::reset();
  Timer2::mode<TimerMode::FastPWM8bit>();
  Timer2::selectClockSource<Timer2Prescaler>();

  BD09PWM::on();
  BD10PWM::on();
  BD11PWM::on();
}

void initializeButtons() {
  button1.begin();
  button2.begin();
  button3.begin();
  button4.begin();
  button5.begin();
  button6.begin();
}

void fillData() {
  for (int16_t i = 0; i < PixelCount; i++) {
    renderBuffer[i].green = 0;
    renderBuffer[i].red = 0;
    renderBuffer[i].blue = 0;
    renderBuffer[i].white = 0;
  }
  //
  //  renderBuffer[PixelCount - 1].green = 15;
  //  renderBuffer[PixelCount - 1].red = 15;
  //  renderBuffer[PixelCount - 1].blue = 15;
  //  renderBuffer[PixelCount - 1].white = 15;
}

void displayNextValue() {
  uint16_t current = SegL::value();
  uint8_t next = nextValue(current);
  SegL::write(next);
  timeout.schedule(500, displayNextValue);
}

void ledOn();

void ledOff() {
  BD13::low();
  timeout.schedule(500, ledOn);
}

void ledOn() {
  BD13::high();
  timeout.schedule(500, ledOff);
}

void setup() {
  BD13::mode<PinMode::Output>();
  timeout.schedule(500, ledOn);

  USART::begin<USARTCfg>();

  initializePWM();
  initializeButtons();

  SegL::write(255);
  SegM::write(255);
  SegR::write(255);

  fillData();
  neoPixel.begin();
  neoPixel.display();
}

template <uint8_t no>
class ButtonHandler {
  private:
    static void printButtonEvent(ButtonEvent event) {
      switch (event) {
        case ButtonEventDown:
          cout << "ButtonEventDown\r\n";
          break;
        case ButtonEventPress:
          cout << "ButtonEventPress\r\n";
          break;
        case ButtonEventUp:
          cout << "ButtonEventUp\r\n";
          break;
      }
    }
  public:
    static void handle(ButtonEvent event) {
      cout << no << ": ";
      printButtonEvent(event);
    }
};

void button4Handle(ButtonEvent event) {
  if (event == ButtonEventPress) {
    auto current = SegL::value();
    auto next = nextValue(current);
    SegL::write(next);
  }
}

void button5Handle(ButtonEvent event) {
  if (event == ButtonEventPress) {
    auto current = SegM::value();
    auto next = nextValue(current);
    SegM::write(next);
  }
}

void button6Handle(ButtonEvent event) {
  if (event == ButtonEventPress) {
    auto current = SegR::value();
    auto next = nextValue(current);
    SegR::write(next);
  }
}

void handleButtons() {
  button1.handle(ButtonHandler<1>::handle);
  button2.handle(ButtonHandler<2>::handle);
  button3.handle(ButtonHandler<3>::handle);
  button4.handle(button4Handle);
  button5.handle(button5Handle);
  button6.handle(button6Handle);
}

void loop() {
  handleButtons();
  timeout.handle();
}

