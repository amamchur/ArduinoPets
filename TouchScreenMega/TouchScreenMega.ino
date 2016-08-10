// Touch screen library with X Y and Z (pressure) readings as well
// as oversampling to avoid 'bouncing'
// This demo code returns raw readings, public domain

#include <stdint.h>
#include <ARDK.h>
#include "TouchScreen.h"
#include "SWTFT.h"
#include "SdFat.h"

const uint8_t SOFT_MISO_PIN = 12;
const uint8_t SOFT_MOSI_PIN = 11;
const uint8_t SOFT_SCK_PIN  = 13;
const uint8_t SD_CHIP_SELECT_PIN = 10;
SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> sd;

const byte buttonPins[] PROGMEM = {53, 51, 49, 47, 37, 35, 33, 31};
void buttonHanlder(unsigned int button, int event);
ARDK::ButtonManager<sizeof(buttonPins)> bm(buttonPins, buttonHanlder);

struct SensorValue {
  float multiplier;
  float constant;
  float divider;
};

struct TouchConfig {
  SensorValue x;
  SensorValue y;
  SensorValue z;
  int screenWidth;
  int screenHeight;
  bool swap;
};

TouchConfig touchConfig = {
  {1, 0, 500},
  {1, 0, 500},
  {1, 0, 1023},
  240, 320, // 90 deg rotation
  true
};

class TouchSensorReader {
  public:
    void read();
    int x() const;
    int y() const;
    int z() const;
};

ARDK::EEPROMStorage<TouchConfig> touchConfigStorage(0);

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 600);

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
SWTFT tft;

void setup(void) {
  Serial.begin(9600);

  if (!sd.begin(SD_CHIP_SELECT_PIN)) {
    sd.initErrorHalt();
  }

  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);

  tft.fillScreen(BLACK);
  testText();

  bm.begin();

  touchConfigStorage >> touchConfig;
}

const int iterationCount = 32;

void calibrateTouchConstants() {
  tft.fillScreen(WHITE);
  tft.setCursor(0, 20);
  tft.setTextColor(BLACK);  tft.setTextSize(2);
  tft.println("Constants Calibration");
  tft.println("Touch Top Left");
  tft.println("Screen Corner");

  int counter = 0;
  float xOffset = 0;
  float yOffset = 0;
  while (counter < iterationCount) {
    TSPoint p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    if (p.z < ts.pressureThreshhold) {
      continue;
    }

    xOffset += p.x;
    yOffset += p.y;

    counter++;

    tft.drawFastHLine(0, 120, tft.width() * ((float)counter / iterationCount), BLACK);

    delay(100);
  }

  touchConfig.x.constant = -xOffset / counter;
  touchConfig.y.constant = -yOffset / counter;
  tft.print("X constant: "); tft.println(touchConfig.x.constant);
  tft.print("Y constant: "); tft.println(touchConfig.y.constant);
  touchConfigStorage << touchConfig;
}

void calibrateTouchDividers() {
  tft.fillScreen(WHITE);
  tft.setCursor(0, 20);
  tft.setTextColor(BLACK);  tft.setTextSize(2);
  tft.println("Dividers Calibration");
  tft.println("Touch Bottom Right");
  tft.println("Screen Corner");

  int counter = 0;
  float xOffset = 0;
  float yOffset = 0;
  while (counter < iterationCount) {
    TSPoint p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    if (p.z < ts.pressureThreshhold) {
      continue;
    }

    float x = p.x * touchConfig.x.multiplier + touchConfig.x.constant;
    float y = p.y * touchConfig.y.multiplier + touchConfig.y.constant;
    if (touchConfig.swap) {
      float tmp = x;
      x = y;
      y = tmp;
    }

    xOffset += x;
    yOffset += y;

    counter++;

    tft.drawFastHLine(0, 120, tft.width() * ((float)counter / iterationCount), BLACK);

    delay(100);
  }

  touchConfig.x.divider = xOffset / counter;
  touchConfig.y.divider = yOffset / counter;
  tft.print("X divider: "); tft.println(touchConfig.x.divider);
  tft.print("Y divider: "); tft.println(touchConfig.y.divider);
  touchConfigStorage << touchConfig;
}

void testText() {
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(RED);    tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
}

void readTouch() {
  // a point object holds x y and z coordinates
  TSPoint p = ts.getPoint();

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z > ts.pressureThreshhold) {
    //    Serial.print("X = "); Serial.print(p.x);
    //    Serial.print("\tY = "); Serial.print(p.y);
    //    Serial.print("\tPressure = "); Serial.println(p.z);

    int x = p.x * touchConfig.x.multiplier + touchConfig.x.constant;
    int y = p.y * touchConfig.y.multiplier + touchConfig.y.constant;
    if (touchConfig.swap) {
      int tmp = x;
      x = y;
      y = tmp;
    }

    int sx = x / touchConfig.x.divider * touchConfig.screenWidth;
    int sy = y / touchConfig.y.divider * touchConfig.screenHeight;
//    Serial.print("pz = "); Serial.println(p.z);
    
    //    Serial.print("px = "); Serial.print(p.x);
    //    Serial.print("; tx = "); Serial.print(x);
    //    Serial.print("; sx = "); Serial.println(sx);
    //
    //    Serial.print("py = "); Serial.print(p.y);
    //    Serial.print("; ty = "); Serial.print(y);
    //    Serial.print("; sy = "); Serial.println(sy);

    tft.fillCircle(sx, sy, 1, BLACK);
  }
}

// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#define BUFFPIXEL 64

uint16_t read16(SdFile &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(SdFile &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

void bmpDraw(const char *filename, int x, int y) {

  SdFile     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3 * BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
  uint16_t lcdbuffer[BUFFPIXEL];  // pixel out buffer (16-bit per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  uint8_t  lcdidx = 0;
  boolean  first = true;

  if ((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print(F("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');
  // Open requested file on SD card
  if (!bmpFile.open(filename, O_READ)) {
    Serial.println(F("File not found"));
    return;
  }

  // Parse BMP header
  if (read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.println(F("File size: ")); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print(F("Header size: ")); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if (read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
      if ((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if (bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if ((x + w - 1) >= tft.width())  w = tft.width()  - x;
        if ((y + h - 1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x + w - 1, y + h - 1);

        for (row = 0; row < h; row++) { // For each scanline...
          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if (bmpFile.curPosition() != pos) { // Need seek?
            bmpFile.seekSet(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col = 0; col < w; col++) { // For each column...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              // Push LCD buffer to the display first
              if (lcdidx > 0) {
                tft.pushColors(lcdbuffer, lcdidx, first);
                lcdidx = 0;
                first  = false;
              }
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            lcdbuffer[lcdidx++] = tft.color565(r, g, b);
            //            lcdbuffer[lcdidx++] = tft.color565(255, 0, 255);
          } // end pixel
        } // end scanline
        // Write any remaining data to LCD
        if (lcdidx > 0) {
          tft.pushColors(lcdbuffer, lcdidx, first);
        }
        Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if (!goodBmp) Serial.println(F("BMP format not recognized."));
}

void buttonHanlder(unsigned int button, int event) {
  if (event != ARDK::BUTTON_EVENT_PRESS) {
    return;
  }

  switch (button) {
    case 0:
      tft.fillScreen(WHITE);
      break;
    case 1:
      calibrateTouchConstants();
      break;
    case 2:
      calibrateTouchDividers();
      break;
    case 3:
      bmpDraw("woof.bmp", 0, 0);
      break;
    case 4:
      bmpDraw("miniwoof.bmp", 0, 0);
      break;
    case 5:
      bmpDraw("test.bmp", 0, 0);
      break;
    case 6:
      touchConfigStorage >> touchConfig;
      Serial.println("Read Value: ");
      break;
    case 7:
      touchConfigStorage << touchConfig;
      Serial.print("Write Value: ");
      break;
  }
}

void loop(void) {
  bm.handle();
  readTouch();
}
