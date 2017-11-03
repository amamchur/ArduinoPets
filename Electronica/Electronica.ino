#include <MCURDK.h>
#include <Board/Auto.hpp>
#include <IC/MAX72xx.hpp>
#include <Utils/MillisecondsCounter.hpp>
#include <Utils/ToolSet.hpp>
#include "font8x8.h"
#include "MatrixHelpers.hpp"

using namespace MCURDK;
using namespace MCURDK::Board;
using namespace MCURDK::IC;
using namespace MCURDK::Utils;

const constexpr uint8_t DeviceCount = 4;

typedef uint32_t CounterType;
typedef MillisecondsCounter<CounterType, &timer0_millis> Counter;
typedef typename Counter::IRQHandler<F_CPU, 64, uint8_t> IRQHandler;
typedef Utils::ToolSet<Counter> Tools;
typedef Tools::FunctionTimeout<8, int8_t> Timeout;

typedef MAX72xx<HardwareSPI1, BD10> MAX7219;
typedef MAX72xxData<DeviceCount> Matrix;

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

Timeout timeout;
MAX7219 max7219;
Matrix matrix;
Matrix buffer;

template<class T> T shiftRight(T value, uint8_t, uint8_t, Matrix&) {
  return value >> 1;
}

void updateDots_iters(bool displayDots) {
  matrix &= RemoveDots();

  if (displayDots) {
    matrix |= PlaceDots();
  }
}

void updateDots(bool displayDots) {
  for (int i = 0; i < 8; i++) {
    matrix.data[1][i] &= ~0x80;
    matrix.data[2][i] &= ~0x01;
  }

  if (displayDots) {
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
  updateDots(true);
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
  updateDots(true);
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

void setup() {
  matrix.clear();
  max7219.begin(DeviceCount);
  max7219(DeviceCount, MAX7219::Intencity0);
  max7219.display(matrix);
  timeout.schedule(0, &updateValue, 0);
}

void loop() {
  timeout.handle();
}

