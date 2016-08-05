#include <ARDK.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display;

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define PIN_GAMEPAD_X_AXIS 1
#define PIN_GAMEPAD_Y_AXIS 0
#define PIN_GAMEPAD_BUTTON 13

#define FIELD_COLUMNS 10
#define FIELD_ROWS 21
#define DISPLAY_OFFSET_X 2
#define DISPLAY_OFFSET_Y 1
#define DISPLAY_WIDTH 62
#define DISPLAY_HEIGHT 127
#define UNIT_SIZE 5
#define UNIT_MARGIN 1
#define MAX_SHAPE_SIZE 4

typedef enum ShapeType {
  ShapeTypeSquere,
  ShapeTypeLine,
  ShapeTypeL,
  ShapeTypeJ,
  ShapeTypeTree,
  ShapeTypeZ,
  ShapeTypeS,
  ShapeTypeCount
} ShapeType;

void buttonPressed(unsigned int, int);

const byte buttons[] PROGMEM = {10};
ARDK::ButtonManager<sizeof(buttons)> buttonManager(buttons, buttonPressed);

byte shape[MAX_SHAPE_SIZE][MAX_SHAPE_SIZE] = {0};
byte shapeWidth = 0; // in units
byte shapeHeight = 0; // in units
float shapeX = 0; // in screen coordinates
float shapeY = 0; // in screen coordinates
float shapeSpeed = 0.1;
int gamepadX = 0;
int gamepadY = 0;
byte gameField[FIELD_ROWS][FIELD_COLUMNS] = {0};
unsigned long updateTime = 0;

void createShape() {
  memset(shape, 0, sizeof(shape));
  ShapeType type = (ShapeType)random(ShapeTypeCount);
  switch (type) {
    case ShapeTypeSquere:
      shape[0][0] = 1;
      shape[0][1] = 1;
      shape[1][0] = 1;
      shape[1][1] = 1;
      shapeWidth = 2;
      shapeHeight = 2;
      break;
    case ShapeTypeLine:
      shape[0][0] = 1;
      shape[1][0] = 1;
      shape[2][0] = 1;
      shape[3][0] = 1;
      shapeWidth = 4;
      shapeHeight = 1;
      break;
    case ShapeTypeL:
      shape[0][0] = 1;
      shape[1][0] = 1;
      shape[2][0] = 1;
      shape[3][0] = 1;
      shape[3][1] = 1;
      shapeWidth = 4;
      shapeHeight = 2;
      break;
    case ShapeTypeJ:
      shape[0][1] = 1;
      shape[1][1] = 1;
      shape[2][1] = 1;
      shape[3][1] = 1;
      shape[3][0] = 1;
      shapeWidth = 4;
      shapeHeight = 2;
      break;
    case ShapeTypeTree:
      shape[0][0] = 1;
      shape[1][0] = 1;
      shape[2][0] = 1;
      shape[1][1] = 1;
      shapeWidth = 3;
      shapeHeight = 2;
      break;
    case ShapeTypeZ:
      shape[0][0] = 1;
      shape[0][1] = 1;
      shape[1][1] = 1;
      shape[1][2] = 1;
      shapeWidth = 2;
      shapeHeight = 3;
      break;
    case ShapeTypeS:
      shape[0][1] = 1;
      shape[0][2] = 1;
      shape[1][0] = 1;
      shape[1][1] = 1;
      shapeWidth = 2;
      shapeHeight = 3;
      break;
  }

  shapeX = (FIELD_COLUMNS - shapeWidth + 1) / 2 * (UNIT_SIZE + UNIT_MARGIN);
  shapeY = -shapeHeight * UNIT_SIZE;
}

void drawGameField() {
  byte offsetY = DISPLAY_OFFSET_Y;
  for (byte i = 0; i < FIELD_ROWS; i++) {
    byte offsetX = DISPLAY_OFFSET_X;
    for (byte j = 0; j < FIELD_COLUMNS; j++) {
      if (gameField[i][j]) {
        display.drawRect(offsetX, offsetY, UNIT_SIZE, UNIT_SIZE, WHITE);
        display.drawPixel(offsetX + 2, offsetY + 2, WHITE);
      }
      offsetX += UNIT_SIZE + UNIT_MARGIN;
    }
    offsetY += UNIT_SIZE + UNIT_MARGIN;
  }
}

void drawBorder() {
  display.drawFastHLine(0, 127, 63, WHITE);
  display.drawFastVLine(0, 0, 128, WHITE);
  display.drawFastVLine(62, 0, 128, WHITE);
  display.drawFastVLine(63, 0, 128.0 * (shapeSpeed / 5.0), WHITE);
}

void drawShape() {
  byte offsetX = round((float)(shapeX - DISPLAY_OFFSET_X) / (UNIT_SIZE + UNIT_MARGIN)) * (UNIT_SIZE + UNIT_MARGIN) + DISPLAY_OFFSET_X;
  for (byte i = 0; i < MAX_SHAPE_SIZE; i++) {
    byte offsetY = shapeY;
    for (byte j = 0; j < MAX_SHAPE_SIZE; j++) {
      if (shape[i][j]) {
        display.fillRect(offsetX, offsetY, UNIT_SIZE, UNIT_SIZE, WHITE);
      }
      offsetY += UNIT_SIZE + UNIT_MARGIN;
    }
    offsetX += UNIT_SIZE + UNIT_MARGIN;
  }
}

void drawFrame() {
  display.clearDisplay();
  drawBorder();
  drawShape();
  drawGameField();
  display.display();
}

void copyShapeToField(int x, int y) {
  for (int i = 0; i < MAX_SHAPE_SIZE; i++) {
    for (int j = 0; j < MAX_SHAPE_SIZE; j++) {
      int fx = j + x;
      int fy = i + y;
      if (fx < 0 || fy < 0) {
        continue;
      }

      if (fx < FIELD_COLUMNS && fy < FIELD_ROWS) {
        gameField[fy][fx] |= shape[j][i];
      }
    }
  }
}

void checkGameOver() {
  for (int i = 0; i < FIELD_COLUMNS; i++) {
    if (gameField[0][i]) {
      startGame();
      return;
    }
  }
}

bool fullRow(int row) {
  for (int i = 0; i < FIELD_COLUMNS; i++) {
    if (!gameField[row][i]) {
      return false;
    }
  }

  return true;
}

void cleanRow(int row) {
  while (row > 0) {
    for (int i = 0; i < FIELD_COLUMNS; i++) {
      gameField[row][i] = gameField[row - 1][i];
    }

    row--;
  }

  for (int i = 0; i < FIELD_COLUMNS; i++) {
    gameField[0][i] = 0;
  }
}

void cleanupField() {
  int i = FIELD_ROWS - 1;
  while (i > 0) {
    if (fullRow(i)) {
      cleanRow(i);
    } else {
      i--;
    }
  }
}

void placeShape(int x, int y) {
  copyShapeToField(x, y);
  createShape();
  cleanupField();
  checkGameOver();
}

bool checkCollision(int sx, int sy) {
  int x = round((float)(sx - DISPLAY_OFFSET_X) / (UNIT_SIZE + UNIT_MARGIN));
  int y = (sy - DISPLAY_OFFSET_Y) / (UNIT_SIZE + UNIT_MARGIN);
  if (y + shapeHeight >= FIELD_ROWS) {
    return true;
  }

  int dy = y + 1;

  for (int i = 0; i < MAX_SHAPE_SIZE && i + x < FIELD_COLUMNS; i++) {
    for (int j = 0; j < MAX_SHAPE_SIZE && j + dy < FIELD_ROWS; j++) {
      int fx = i + x;
      int fy = j + dy;
      if (fx < 0 || fy < 0) {
        continue;
      }

      byte v1 = shape[i][j];
      byte v2 = gameField[fy][fx];

      if (v1 == 1 && v2 == 1) {
        return true;
      }
    }
  }

  return false;
}

void readGamepad() {
  float value = analogRead(PIN_GAMEPAD_X_AXIS);
  value = map(value, 0, 1023, 0, 10) - gamepadX;
  float x = constrain(shapeX - value, DISPLAY_OFFSET_X, DISPLAY_WIDTH - shapeWidth * (UNIT_SIZE + UNIT_MARGIN));
  bool collision = checkCollision(x, shapeY);
  if (!collision) {
    shapeX = x;
  }

  value = analogRead(PIN_GAMEPAD_Y_AXIS);
  value = map(value, 0, 1023, 0, 10) - gamepadY;
  value = max(value, shapeSpeed);
  float y = constrain(shapeY + value, DISPLAY_OFFSET_Y, DISPLAY_HEIGHT - shapeHeight * (UNIT_SIZE + UNIT_MARGIN));
  collision = checkCollision(shapeX, y);
  if (collision) {
    x = round((float)(shapeX - DISPLAY_OFFSET_X) / (UNIT_SIZE + UNIT_MARGIN));
    y = (y - DISPLAY_OFFSET_Y) / (UNIT_SIZE + UNIT_MARGIN);
    placeShape(x, y);
  } else {
    shapeY = y;
  }
}

void calibrateGamepad() {
  for (int i = 0; i < 5; i++) {
    int value = analogRead(PIN_GAMEPAD_X_AXIS);
    gamepadX += map(value, 0, 1023, 0, 10);

    value = analogRead(PIN_GAMEPAD_Y_AXIS);
    gamepadY += map(value, 0, 1023, 0, 10);
  }

  gamepadX /= 5;
  gamepadY /= 5;
}

void buttonPressed(unsigned int button, int event) {
  if (event != ARDK::BUTTON_EVENT_PRESS) {
    return;
  }

  byte shapeBuffer[MAX_SHAPE_SIZE][MAX_SHAPE_SIZE] = {0};
  byte tmp;

  // Create transpose matrix in buffer
  for (byte i = 0; i < MAX_SHAPE_SIZE; i++) {
    for (byte j = 0; j < MAX_SHAPE_SIZE; j++) {
      shapeBuffer[j][i] = shape[i][j];
    }
  }

  // Reverse columns to main shape matrix
  byte offsetX = 255;
  byte offsetY = 255;
  for (byte i = 0; i < MAX_SHAPE_SIZE; i++) {
    for (byte j = 0; j < MAX_SHAPE_SIZE; j++) {
      tmp = MAX_SHAPE_SIZE - j - 1;
      shape[i][tmp] = shapeBuffer[i][j];
      if (shape[i][tmp]) {
        offsetX = min(offsetX, i);
        offsetY = min(offsetY, tmp);
      }
    }
  }

  // Normilize matrix
  memset(shapeBuffer, 0, sizeof(shapeBuffer));
  for (byte i = offsetX, k = 0; i < MAX_SHAPE_SIZE; i++, k++) {
    for (byte j = offsetY, t = 0; j < MAX_SHAPE_SIZE; j++, t++) {
      shapeBuffer[k][t] = shape[i][j];
    }
  }
  memcpy(shape, shapeBuffer, sizeof(shapeBuffer));

  tmp = shapeWidth;
  shapeWidth = shapeHeight;
  shapeHeight = tmp;
  readGamepad();
}

void readPotentiometer() {
  int value = analogRead(3);
  value = map(value, 0, 1023, 0, 1000);
  shapeSpeed = value / 200.0;
}

void startGame() {
  memset(gameField, 0, sizeof(gameField));
  createShape();
}

void setup() {
  pinMode(PIN_GAMEPAD_BUTTON, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.setRotation(3);
  buttonManager.begin(25);
  calibrateGamepad();
  startGame();
}

void loop() {
  unsigned long t = millis();
  unsigned long dt = t - updateTime;
  if (dt > 35) {
    readPotentiometer();
    readGamepad();
    drawFrame();
    buttonManager.handle();
    updateTime = t;
  }
}

