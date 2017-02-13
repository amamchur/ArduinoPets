#include <ARDK.h>
#include <EEPROMStorage.hpp>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <IRremote.h>
#include <DS3231.h>

typedef enum : int {
  IRCommandDigit0,
  IRCommandDigit1,
  IRCommandDigit2,
  IRCommandDigit3,
  IRCommandDigit4,
  IRCommandDigit5,
  IRCommandDigit6,
  IRCommandDigit7,
  IRCommandDigit8,
  IRCommandDigit9,
  IRCommandInc,
  IRCommandDec,
  IRCommandLeftComma,
  IRCommandRightComma,
  IRCommandPowerMode,

  IRCommandCount
} IRCommand;

typedef struct {
  unsigned long irCommands[IRCommandCount];
} Config;

typedef enum UpdateFlags {
  UpdateFlagNone = 0,
  UpdateFlagNixie = 1 << 0,
  UpdateFlagOLED = 1 << 1,
  UpdateFlagIR = 1 << 2,
  UpdateFlagComma = 1 << 3,
  UpdateFlagPower = 1 << 4,
  UpdateFlagAll = UpdateFlagNone
                  | UpdateFlagNixie
                  | UpdateFlagOLED
                  | UpdateFlagIR
                  | UpdateFlagComma
                  | UpdateFlagPower
} UpdateFlags;

typedef enum {
  PowerModeNone = 0,
  PowerModeNixie = 1 << 0,
  PowerModeOLED = 1 << 1,
  PowerModeAll = PowerModeNone
                 | PowerModeNixie
                 | PowerModeOLED
} PowerMode;

typedef enum {
  CommaModeOff = 0,
  CommaModeLeft = 1 << 0,
  CommaModeRight = 1 << 1,
  CommaModeAll = CommaModeOff
                 | CommaModeLeft
                 | CommaModeRight
} CommaMode;

template<class T> void setFlag(T &flags, T flag) {
  flags = (T)(flags | flag);
}

template<class T> void toggleFlag(T &flags, T flag) {
  flags = (T)(flags ^ flag);
}

template<class T> void nextFlag(T &flags, T allFlags) {
  flags = (T)((flags + 1) & allFlags);
}

template<class T> void resetFlag(T &flags, T flag) {
  flags = (T)(flags & ~flag);
}

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define RECV_PIN 11
#define NUMPIXELS_PIN 3
#define NUMPIXELS     12

Config cfg;
ARDK::EEPROMStorage<Config> configStorage(0);
Adafruit_SSD1306 display;
DS3231 rtc(SDA, SCL);
IRrecv irrecv(RECV_PIN);

uint8_t nixieDigit = 0;
uint8_t pins[] = {4, 5, 6, 7};
#define PINS_COUNT (sizeof(pins) / sizeof(pins[0]))

PowerMode powerMode = PowerModeAll;
CommaMode commaMode = CommaModeOff;
UpdateFlags updateFlags = UpdateFlagAll;

void initCfg() {
  cfg.irCommands[IRCommandDigit0] = 0x20DF08F7;
  cfg.irCommands[IRCommandDigit1] = 0x20DF8877;
  cfg.irCommands[IRCommandDigit2] = 0x20DF48B7;
  cfg.irCommands[IRCommandDigit3] = 0x20DFC837;
  cfg.irCommands[IRCommandDigit4] = 0x20DF28D7;
  cfg.irCommands[IRCommandDigit5] = 0x20DFA857;
  cfg.irCommands[IRCommandDigit6] = 0x20DF6897;
  cfg.irCommands[IRCommandDigit7] = 0x20DFE817;
  cfg.irCommands[IRCommandDigit8] = 0x20DF18E7;
  cfg.irCommands[IRCommandDigit9] = 0x20DF9867;
  cfg.irCommands[IRCommandInc] = 0x20DF00FF;
  cfg.irCommands[IRCommandDec] = 0x20DF807F;
  cfg.irCommands[IRCommandLeftComma] = 0x20DFCA35;
  cfg.irCommands[IRCommandRightComma] = 0x20DF58A7;
  cfg.irCommands[IRCommandPowerMode] = 0x20DF10EF;
}

void drawTime(const Time &t) {
  char timeStr[] = "00:00:00";
  timeStr[0] += t.hour / 10;
  timeStr[1] += t.hour % 10;

  timeStr[3] += t.min / 10;
  timeStr[4] += t.min % 10;

  timeStr[6] += t.sec / 10;
  timeStr[7] += t.sec % 10;

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 5);
  display.println(timeStr);
}

void drawDate(const Time &t) {
  char dateStr[] = "00.00.0000";
  dateStr[0] += t.date / 10;
  dateStr[1] += t.date % 10;

  dateStr[3] += t.mon / 10;
  dateStr[4] += t.mon % 10;

  dateStr[6] += t.year / 1000;
  dateStr[7] += (t.year / 100) % 10;
  dateStr[8] += (t.year / 10) % 10;
  dateStr[9] += t.year % 10;

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 25);
  display.println(dateStr);
}

void drawTemp() {
  float temp = rtc.getTemp();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 45);
  display.print(temp);
  display.println(" t,C");
}

void processCommand(unsigned long results) {
  int index = -1;
  for (int i = 0; i < IRCommandCount; i++) {
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
    case IRCommandDigit0:
      nixieDigit = 0;
      break;
    case IRCommandDigit1:
      nixieDigit = 1;
      break;
    case IRCommandDigit2:
      nixieDigit = 2;
      break;
    case IRCommandDigit3:
      nixieDigit = 3;
      break;
    case IRCommandDigit4:
      nixieDigit = 4;
      break;
    case IRCommandDigit5:
      nixieDigit = 5;
      break;
    case IRCommandDigit6:
      nixieDigit = 6;
      break;
    case IRCommandDigit7:
      nixieDigit = 7;
      break;
    case IRCommandDigit8:
      nixieDigit = 8;
      break;
    case IRCommandDigit9:
      nixieDigit = 9;
      break;
    case IRCommandInc:
      nixieDigit = (nixieDigit + 1) % 10;
      break;
    case IRCommandDec:
      nixieDigit = (nixieDigit + 9) % 10;
      break;
    case IRCommandLeftComma:
      toggleFlag(commaMode, CommaModeLeft);
      setFlag(updateFlags, UpdateFlagComma);
      break;
    case IRCommandRightComma:
      toggleFlag(commaMode, CommaModeRight);
      setFlag(updateFlags, UpdateFlagComma);
      break;
    case IRCommandPowerMode:
      nextFlag(powerMode, PowerModeAll);
      setFlag(updateFlags, UpdateFlagPower);
      Serial.println(powerMode);
      break;
  }

  setFlag(updateFlags, UpdateFlagNixie);
}

volatile unsigned int ticks = 0;

void timing() {
  ticks++;

  setFlag(updateFlags, (ticks & 0x3FF == 0x3FF) ? UpdateFlagIR : UpdateFlagNone);
  setFlag(updateFlags, (ticks & 0x1FFF == 0x1FFF) ? UpdateFlagOLED : UpdateFlagNone);

  if (ticks >= 32768) {
    ticks = 0;
  }
}

void setTime() {
  rtc.setDate(28, 12, 2016);
  rtc.setTime(04, 32, 00);
}

void setup() {
  initCfg();
  //configStorage >> cfg;

  pinMode(10, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  for (int i = 0; i < PINS_COUNT; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], 0);
  }

  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  rtc.begin();
  rtc.enable32KHz(true);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.display();

  Serial.println("Pin...");
  Serial.println(digitalPinToInterrupt(3));

  attachInterrupt(digitalPinToInterrupt(3), timing, RISING);

//  setTime();
}

void refreshOLED() {
  Time t = rtc.getTime();
  display.clearDisplay();
  drawTime(t);
  drawDate(t);
  drawTemp();
  display.display();

  resetFlag(updateFlags, UpdateFlagOLED);
}

void readIR() {
  decode_results results;
  if (irrecv.decode(&results)) {
    processCommand(results.value);
    irrecv.resume(); // Receive the next value
  }
}

void refreshNixie() {
  for (int i = 0; i < PINS_COUNT; i++) {
    uint8_t value = (nixieDigit >> i) & 1;
    digitalWrite(pins[i], value);
  }

  resetFlag(updateFlags, UpdateFlagNixie);
}

void refreshPower() {
  digitalWrite(10, (powerMode & PowerModeNixie) == PowerModeNixie ? 1 : 0);
  if ((powerMode & PowerModeOLED) == PowerModeOLED) {
    display.ssd1306_command(SSD1306_DISPLAYON);
    //display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  } else {
    display.ssd1306_command(SSD1306_DISPLAYOFF);
  }

  resetFlag(updateFlags, UpdateFlagPower);
}

void refreshComma() {
  digitalWrite(12, (commaMode & CommaModeLeft) == CommaModeLeft ? 1 : 0);
  digitalWrite(13, (commaMode & CommaModeRight) == CommaModeRight ? 1 : 0);
  resetFlag(updateFlags, UpdateFlagComma);
}

void loop() {
  if ((updateFlags & UpdateFlagIR) != 0) {
    readIR();
  }

  if ((updateFlags & UpdateFlagNixie) != 0) {
    refreshNixie();
  }

  if ((updateFlags & UpdateFlagOLED) != 0) {
    refreshOLED();
  }

  if ((updateFlags & UpdateFlagPower) != 0) {
    refreshPower();
  }

  if ((updateFlags & UpdateFlagComma) != 0) {
    refreshComma();
  }
}

