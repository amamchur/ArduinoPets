#include <ARDK.h>
#include <EEPROMStorage.hpp>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <IRremote.h>

#define DIGIT_0 0
#define DIGIT_1 1
#define DIGIT_2 2
#define DIGIT_3 3
#define DIGIT_4 4
#define DIGIT_5 5
#define DIGIT_6 6
#define DIGIT_7 7
#define DIGIT_8 8
#define DIGIT_9 9
#define DIGIT_INC 10
#define DIGIT_DEC 11
#define DIGIT_L_COMMA 12
#define DIGIT_R_COMMA 13
#define DIGIT_OFF_ON 14

#define IR_COMMAND_LENGTH (DIGIT_OFF_ON + 1)

typedef struct {
  unsigned long irCommands[IR_COMMAND_LENGTH];
} Config;

int RECV_PIN = 11;

Config cfg;
ARDK::EEPROMStorage<Config> configStorage(0);
Adafruit_SSD1306 display;
IRrecv irrecv(RECV_PIN);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

uint8_t pins[] = {2, 3, 4, 5};
uint8_t pinsCount = sizeof(pins) / sizeof(pins[0]);
uint8_t leds = 0;
bool lComma = false;
bool rComma = false;
bool nixieEnabled = true;
bool refresh = true;

void initCfg() {
  cfg.irCommands[DIGIT_0] = 0x20DF08F7;
  cfg.irCommands[DIGIT_1] = 0x20DF8877;
  cfg.irCommands[DIGIT_2] = 0x20DF48B7;
  cfg.irCommands[DIGIT_3] = 0x20DFC837;
  cfg.irCommands[DIGIT_4] = 0x20DF28D7;
  cfg.irCommands[DIGIT_5] = 0x20DFA857;
  cfg.irCommands[DIGIT_6] = 0x20DF6897;
  cfg.irCommands[DIGIT_7] = 0x20DFE817;
  cfg.irCommands[DIGIT_8] = 0x20DF18E7;
  cfg.irCommands[DIGIT_9] = 0x20DF9867;
  cfg.irCommands[DIGIT_INC] = 0x20DF00FF;
  cfg.irCommands[DIGIT_DEC] = 0x20DF807F;
  cfg.irCommands[DIGIT_L_COMMA] = 0x20DFCA35;
  cfg.irCommands[DIGIT_R_COMMA] = 0x20DF58A7;
  cfg.irCommands[DIGIT_OFF_ON] = 0x20DF10EF;
}

void setup() {
  initCfg();
  //configStorage >> cfg;

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  digitalWrite(10, nixieEnabled ? 1 : 0);

  for (int i = 0; i < pinsCount; i++) {
    digitalWrite(pins[i], 0);
  }

  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.display();
}

void processCommand(unsigned long results) {
  int index = -1;
  for (int i = 0; i < IR_COMMAND_LENGTH; i++) {
    if (cfg.irCommands[i] == results) {
      index = i;
      break;
    }
  }

//  Serial.println(results, HEX);
//  Serial.println(index);

  if (index == -1) {
    return;
  }

  switch (index) {
    case DIGIT_0:
      leds = 0;
      break;
    case DIGIT_1:
      leds = 1;
      break;
    case DIGIT_2:
      leds = 2;
      break;
    case DIGIT_3:
      leds = 3;
      break;
    case DIGIT_4:
      leds = 4;
      break;
    case DIGIT_5:
      leds = 5;
      break;
    case DIGIT_6:
      leds = 6;
      break;
    case DIGIT_7:
      leds = 7;
      break;
    case DIGIT_8:
      leds = 8;
      break;
    case DIGIT_9:
      leds = 9;
      break;
    case DIGIT_INC:
      leds = (leds + 1) % 10;
      break;
    case DIGIT_DEC:
      leds = (leds + 9) % 10;
      break;
    case DIGIT_L_COMMA:
      lComma = !lComma;
      digitalWrite(12, lComma ? 1 : 0);
      break;
    case DIGIT_R_COMMA:
      rComma = !rComma;
      digitalWrite(13, rComma ? 1 : 0);
      break;
    case DIGIT_OFF_ON:
      nixieEnabled = !nixieEnabled;
      digitalWrite(10, nixieEnabled ? 1 : 0);
      break;
  }

  refresh = true;
}

void loop() {
  decode_results results;

  if (irrecv.decode(&results)) {
    processCommand(results.value);
    irrecv.resume(); // Receive the next value
  }

  if (refresh) {
    Serial.print("Update!!!: ");
    Serial.println(leds);
  }

  for (int i = 0; refresh && i < pinsCount; i++) {
    uint8_t value = (leds >> i) & 1;
    digitalWrite(pins[i], value);
  }

  refresh = false;

  delay(100);
}

