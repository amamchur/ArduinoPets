#include "AppConfig.hpp"

#include <MCURDK.h>
#include <Board/Auto.hpp>
#include <IO/OutputStream.hpp>
#include <Utils/MillisecondsCounter.hpp>
#include <Utils/ToolSet.hpp>

#include "MatrixHelpers.hpp"

using namespace MCURDK;
using namespace MCURDK::Board;
using namespace MCURDK::IC;
using namespace MCURDK::GPIO;

const uint64_t Digits[] = {
  0x3844444444444438,
  0x3810101010103010,
  0x7c20100804044438,
  0x3844040418044438,
  0x04047c4424140c04,
  0x384404047840407c,
  0x3844444478404438,
  0x202020100804047c,
  0x3844444438444438,
  0x3844043c44444438,
};

const int DigitsCount = sizeof(Digits) / sizeof(Digits[0]);
const uint32_t ScrollTimeout = 60;
const int8_t ScrollSpace = 1;

uint16_t value = 0;

::MCURDK::IO::OutputStream<USART> cout;
Timeout timeout;
RTC rtc;
MAX7219 max7219;
Matrix matrix;
Matrix buffer;

Button1 button1;
Button2 button2;
Button3 button3;
Button4 button4;
Button5 button5;
Button6 button6;
Button7 button7;
Button8 button8;
Button9 button9;

uint16_t dateTime = 0;
bool displayDots = false;
bool powerOn = true;
MAX7219::Command intencity = MAX7219::IntencityF;
MCURDK::Data::DateTime currentDateTime;

void displayMinutesSecondsTime(int8_t);
void displayHoursMinutes(int8_t);
void (*displayFn)(int8_t) = &displayHoursMinutes;

template<class T> T shiftRight(T value, uint8_t, uint8_t, Matrix&) {
  return value >> 1;
}

void updateDots_iters(bool displayDots) {
  matrix &= RemoveDots();

  if (displayDots) {
    matrix |= PlaceDots();
  }
}

void updateDots(bool dots) {
  for (int i = 0; i < 8; i++) {
    matrix.data[1][i] &= ~0x80;
    matrix.data[2][i] &= ~0x01;
  }

  if (dots) {
    matrix.data[1][1] |= 0x80;
    matrix.data[1][2] |= 0x80;
    matrix.data[1][5] |= 0x80;
    matrix.data[1][6] |= 0x80;

    matrix.data[2][1] |= 0x01;
    matrix.data[2][2] |= 0x01;
    matrix.data[2][5] |= 0x01;
    matrix.data[2][6] |= 0x01;
  }
}

uint8_t currentData[DeviceCount];
uint8_t nextData[DeviceCount];

void fillMatrix(Matrix &m, uint8_t *data) {
  for (int i = 0; i < DeviceCount; i++) {
    m.setRows(i, &Digits[data[i]]);
  }

  m.transformRow(1, &shiftRight<uint8_t>);
  m.transformRow(3, &shiftRight<uint8_t>);
}

void splitData(uint16_t v, uint8_t *data) {
  for (int i = 0; i < DeviceCount; i++) {
    data[i] = v % 10;
    v /= 10;
  }
}

void scrollScreen(int8_t step) {
  if (step > 0) {
    timeout.schedule(ScrollTimeout, &scrollScreen, step - 1);
  }

  for (int i = 0; i < DeviceCount; i++) {
    if (currentData[i] != nextData[i]) {
      uint8_t r = buffer.insertRow(i, 0);
      matrix.insertRow(i, r);
    }
  }
  updateDots(displayDots);
  max7219.display(matrix);
}

void shiftScreen(int8_t step) {
  if (step == 0) {
    timeout.schedule(ScrollTimeout, &scrollScreen, 7);
  } else {
    timeout.schedule(ScrollTimeout, &shiftScreen, step - 1);
  }

  for (int i = 0; i < DeviceCount; i++) {
    if (currentData[i] != nextData[i]) {
      matrix.insertRow(i, 0);
    }
  }
  updateDots(displayDots);
  max7219.display(matrix);
}

void updateValue(int8_t) {
  splitData(value, currentData);
  value += 1;
  splitData(value, nextData);

  fillMatrix(matrix, currentData);
  fillMatrix(buffer, nextData);
  updateDots(true);
  max7219.display(matrix);
  timeout.schedule(1, &shiftScreen, ScrollSpace);
  timeout.schedule(1000, &updateValue, 0);
}

void fetchDateTime(int8_t) {
  rtc.fetch();
  timeout.schedule(1, displayFn, 0);
}

void displayMinutesSecondsTime(int8_t) {
  if (!rtc.ready) {
    timeout.schedule(1, &displayMinutesSecondsTime, 0);
    return;
  }

  auto dt = rtc.dateTime();
  //  uint16_t value = dt.hours;
  //  value *= 10;
  uint16_t value = dt.minutes;
  value *= 100;
  value += dt.seconds;
  displayDots = true;

  splitData(dateTime, currentData);
  splitData(value, nextData);
  fillMatrix(matrix, currentData);
  fillMatrix(buffer, nextData);

  max7219.display(matrix);
  timeout.schedule(0, &shiftScreen, ScrollSpace);
  timeout.schedule(1000, &fetchDateTime, 0);

  dateTime = value;
  currentDateTime = dt;
}

void displayHoursMinutes(int8_t) {
  if (!rtc.ready) {
    timeout.schedule(1, &displayHoursMinutes, 0);
    return;
  }

  auto dt = rtc.dateTime();
  uint16_t value = dt.hours;
  value *= 100;
  value += dt.minutes;
  displayDots = (dt.seconds & 1) == 0;

  splitData(dateTime, currentData);
  splitData(value, nextData);
  fillMatrix(matrix, currentData);
  fillMatrix(buffer, nextData);

  max7219.display(matrix);
  timeout.schedule(0, &shiftScreen, ScrollSpace);
  timeout.schedule(1000, &fetchDateTime, 0);

  dateTime = value;
  currentDateTime = dt;
}

void setup() {
  USART::begin<CfgUSART>();
  I2C::begin<CfgI2C>();

  button1.begin();
  button2.begin();
  button3.begin();
  button4.begin();
  button5.begin();
  button6.begin();
  button7.begin();
  button8.begin();
  button9.begin();

  matrix.clear();
  max7219.begin(DeviceCount);
  max7219(DeviceCount, intencity);
  max7219.display(matrix);
  //  timeout.schedule(0, &updateValue, 0);
  timeout.schedule(0, &fetchDateTime, 0);
}

void button1Handler(IO::ButtonEvent event) {
  if (event != IO::ButtonEventPress) {
    return;
  }

  powerOn = !powerOn;
  if (powerOn) {
    max7219(DeviceCount, MAX7219::On);
    timeout.schedule(0, &fetchDateTime, 0);
  } else {
    timeout.clear();
    max7219(DeviceCount, MAX7219::Off);
  }
}

void button2Handler(IO::ButtonEvent event) {
  if (event != IO::ButtonEventPress) {
    return;
  }

  if (displayFn == &displayHoursMinutes) {
    displayFn = &displayMinutesSecondsTime;
  } else {
    displayFn = &displayHoursMinutes;
  }
}

void button3Handler(IO::ButtonEvent event) {
  if (event == IO::ButtonEventPress) {
    cout << "button3Handler" << IO::endl;
  }
}

void button4Handler(IO::ButtonEvent event) {
  if (event != IO::ButtonEventPress) {
    return;
  }

  if (intencity < MAX7219::IntencityF) {
    intencity = (MAX7219::Command)((uint16_t)intencity + 1);
  }
  
  max7219(DeviceCount, intencity);
}

void button5Handler(IO::ButtonEvent event) {
  if (event != IO::ButtonEventPress) {
    return;
  }

  if (intencity > MAX7219::Intencity0) {
    intencity = (MAX7219::Command)((uint16_t)intencity - 1);
  }
  
  max7219(DeviceCount, intencity);
}

void button6Handler(IO::ButtonEvent event) {
  if (event == IO::ButtonEventPress) {
    cout << "button6Handler" << IO::endl;
  }
}

void button7Handler(IO::ButtonEvent event) {
  if (event == IO::ButtonEventPress) {
    cout << "button7Handler" << IO::endl;
  }
}

void button8Handler(IO::ButtonEvent event) {
  if (event == IO::ButtonEventPress) {
    cout << "button8Handler" << IO::endl;
  }
}

void button9Handler(IO::ButtonEvent event) {
  if (event == IO::ButtonEventPress) {
    cout << "button9Handler" << IO::endl;
  }
}

void loop() {
  button1.handle(button1Handler);
  button2.handle(button2Handler);
  button3.handle(button3Handler);
  button4.handle(button4Handler);
  button5.handle(button5Handler);
  button6.handle(button6Handler);
  button7.handle(button7Handler);
  button8.handle(button8Handler);
  // TODO: Fix soldering
  //  button9.handle(button9Handler);

  timeout.handle();
}

