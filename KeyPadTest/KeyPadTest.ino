
#include <MCURDK.h>
#include <MCURDK/IO/Keypad.hpp>
#include <MCURDK/Board/Auto.hpp>
#include <MCURDK/IO/DigitalSampler.hpp>
#include <MCURDK/Utils/ToolSet.hpp>
#include <MCURDK/Utils/MillisecondsCounter.hpp>

using namespace MCURDK::Board;
using namespace MCURDK::IO;
using namespace MCURDK::GPIO;

typedef MCURDK::Utils::MillisecondsCounter<decltype(timer0_millis), &timer0_millis> Counter;
typedef MCURDK::Utils::ToolSet<MCURDK::Board::MCU, Counter> Tools;

// Optional keypad configuration parameters, but it could be very useful to tune your board
class MyKeypadCfg {
  public:
    // That's head pain of embedded programmers - parasitic capacitanceice
    // Tested with SAM3X 84 MHz clock (Arduino Due)
    // You can decrease it to increase keypad scanning performance
    static constexpr uint16_t CapacitanceCompensationMicrosec = 4;

    // Button bounce compensation - you should know what it is.
    // P.S. Tested with membrane keypad. Regular tactile button could required bigger delay.
    static constexpr uint16_t DebounceDelayMillisec = 5;

    // Key press event delay - common UX approach
    static constexpr uint16_t PressDelayMillisec = 300;
};


/*
   Keypad 5x4 (5 rows, 4 columns) definition
   WARNING! I'm using mathematical approach for matrix definition: rows first then columns.
   But most vendors use another: columns first then rows. Just keep in mind.
   Look at https://www.google.com.ua/search?q=Membrane+Keypad+4x5
   Physical layout:
   +---+---+---+---+
   | F1| F2| # | * |
   +---+---+---+---+
   | 1 | 2 | 3 | ^ |
   +---+---+---+---+
   | 4 | 5 | 6 | v |
   +---+---+---+---+
   | 7 | 8 | 9 |Esc|
   +---+---+---+---+
   | < | 0 | > |Ent|
   +---+---+---+---+

   Please read how keypad works first!

   KeypadRowSelector - prepares pins for reading.
   It set pins to ground or high impedance state, depending on row index.

   KeypadColumnReader - configures columns' pin to input pull up mode
   and provides API for column value reading by specified index.

   Keypad is responsible for pins state handling and event notification
*/
typedef KeypadRowSelector<BD35, BD37, BD39, BD41, BD43> RowSelector5x4;
typedef KeypadColumnReader<BD51, BD49, BD47, BD45> ColumnReader5x4;
typedef Keypad<RowSelector5x4, ColumnReader5x4, Tools, MyKeypadCfg> Keypad5x4;
char layout5x4[5][4] = {
  {'f', 'F', '#', '*'},
  {'1', '2', '3', '^'},
  {'4', '5', '6', 'v'},
  {'7', '8', '9', 'e'},
  {'<', '0', '>', 'r'}
};

typedef KeypadRowSelector<BD51, BD49, BD47, BD45> RowSelector4x4;
typedef KeypadColumnReader<BD43, BD41, BD39, BD37> ColumnReader4x4;
typedef Keypad<RowSelector4x4, ColumnReader4x4, Tools, MyKeypadCfg> Keypad4x4;
char layout4x4[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

typedef KeypadRowSelector<BD51, BD49, BD47, BD45> RowSelector3x4;
typedef KeypadColumnReader<BD43, BD41, BD39> ColumnReader3x4;
typedef Keypad<RowSelector3x4, ColumnReader3x4, Tools, MyKeypadCfg> Keypad4x3;
char layout4x3[4][3] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

typedef KeypadRowSelector<BD33> RowSelector1x4;
typedef KeypadColumnReader<BD31, BD29, BD27, BD25> ColumnReader1x4;
typedef Keypad<RowSelector1x4, ColumnReader1x4, Tools, MyKeypadCfg> Keypad1x4;
char layout1x4[1][4] = {
  {'2', '1', '4', '3'},
};

// Choose one of keypad configuration
typedef Keypad5x4 MyKeypad;
//typedef Keypad4x4 MyKeypad;
//typedef Keypad4x3 MyKeypad;
//typedef Keypad1x4 MyKeypad;

MyKeypad keypad;

void handler(void*, uint8_t button, ButtonEvent e) {
  if (e != ButtonEventPress) {
    return;
  }

  char *layout = nullptr;

  if (MyKeypad::Rows == 5 && MyKeypad::Columns == 4) {
    layout = &layout5x4[0][0];
  }

  if (MyKeypad::Rows == 4 && MyKeypad::Columns == 4) {
    layout = &layout4x4[0][0];
  }

  if (MyKeypad::Rows == 4 && MyKeypad::Columns == 3) {
    layout = &layout4x3[0][0];
  }

  if (MyKeypad::Rows == 1 && MyKeypad::Columns == 4) {
    layout = &layout1x4[0][0];
  }

  if (layout == nullptr) {
    return;
  }

  uint8_t row = button >> 4;
  uint8_t column = button & 0xF;
  char ch = *(layout + MyKeypad::Columns * row + column);

  Serial.print(ch);
  Serial.println("");
}

void setup() {
  Serial.begin(9600);
  keypad.begin();
  BD13::mode<PinMode::Output>();
}

void loop() {
  keypad.handle(handler);
  BD13::toggle();
}

