#include <MCURDK.h>
#include <MCURDK/Board/Auto.hpp>
#include <MCURDK/IO/Button.hpp>
#include <MCURDK/IO/OutputStream.hpp>
#include <MCURDK/IC/WS2812.hpp>
#include <MCURDK/Utils/ToolSet.hpp>
#include <MCURDK/Utils/MillisecondsCounter.hpp>
#include <MCURDK/Utils/Prescalers.hpp>

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
typedef typename Tools::template FunctionTimeout<8, int> Timeout;
typedef MCURDK::Utils::PrescalerLE<Timer1, 1>::Result Timer1Prescaler;
typedef MCURDK::Utils::PrescalerLE<Timer2, 1>::Result Timer2Prescaler;
typedef MCURDK::Board::MCU::USARTConfig<F_CPU, 9600> USARTCfg;
typedef MCURDK::Board::MCU::HardwareUSART0<16, 8> USART;

typedef BD09PWM SegL;
typedef BD10PWM SegM;
typedef BD11PWM SegR;

const uint8_t PwdMode[] = {0, 5, 10, 30, 70, 150, 255};
const uint8_t PwdModeCount = sizeof(PwdMode) / sizeof(PwdMode[0]);

void * BackLedsMemOffset  = (void *)0x00;

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

const int PixelCount = 134;
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

void writeBackLeds() {
  uint8_t data[3] = {
    (uint8_t)SegL::value(),
    (uint8_t)SegM::value(),
    (uint8_t)SegR::value()
  };

  eeprom_write_block(data, BackLedsMemOffset, sizeof(data));
}

void readBackLeds() {
  uint8_t data[3];
  eeprom_read_block(data, BackLedsMemOffset, sizeof(data));

  SegL::value(data[0]);
  SegM::value(data[1]);
  SegR::value(data[2]);
}

void go(int) {
  readBackLeds();

  memset(renderBuffer, 0, sizeof(renderBuffer));
  neoPixel.display();
}

void startupAnimation(int length) {
  const uint8_t middle = PixelCount / 2;
  if (length > middle) {
    timeout.schedule(0, go);
    return;
  }

  memset(renderBuffer, 0, sizeof(renderBuffer));

  int i = middle;
  for (int k = length; k > 0 && i < PixelCount; k--, i++) {
    renderBuffer[i].green = 1;
    renderBuffer[i].red = 1;
    renderBuffer[i].blue = 1;
    renderBuffer[i].white = 1;
  }

  i = middle;
  for (int k = length; k > 0 && i >= 0; k--, i--) {
    renderBuffer[i].green = 1;
    renderBuffer[i].red = 1;
    renderBuffer[i].blue = 1;
    renderBuffer[i].white = 1;
  }

  neoPixel.display();
  timeout.schedule(0, startupAnimation, length + 1);
}

void ledOn(int);

void ledOff(int) {
  BD13::low();
  timeout.schedule(500, ledOn);
}

void ledOn(int) {
  BD13::high();
  timeout.schedule(500, ledOff);
}

void setup() {
  BD13::mode<PinMode::Output>();
  timeout.schedule(500, ledOn);

  USART::begin<USARTCfg>();

  initializePWM();
  initializeButtons();

  SegL::value(0);
  SegM::value(0);
  SegR::value(0);

  neoPixel.begin();
  timeout.schedule(0, startupAnimation, 1);
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
    SegL::value(next);
    writeBackLeds();
  }
}

void button5Handle(ButtonEvent event) {
  if (event == ButtonEventPress) {
    auto current = SegM::value();
    auto next = nextValue(current);
    SegM::value(next);
    writeBackLeds();
  }
}

void button6Handle(ButtonEvent event) {
  if (event == ButtonEventPress) {
    auto current = SegR::value();
    auto next = nextValue(current);
    SegR::value(next);
    writeBackLeds();
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

//int main() {
//  setup();
//  while (1) {
//    loop();
//  }
//  return 0;
//}
