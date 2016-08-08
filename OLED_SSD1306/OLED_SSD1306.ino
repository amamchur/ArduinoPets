#include <DS3231.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display;
DS3231 rtc(SDA, SCL);
Time t;

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup() {
  rtc.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
}

void drawTime() {
  char timeStr[] = "00:00:00";
  timeStr[0] += t.hour / 10;
  timeStr[1] += t.hour % 10;
  
  timeStr[3] += t.min / 10;
  timeStr[4] += t.min % 10;
  
  timeStr[6] += t.sec / 10;
  timeStr[7] += t.sec % 10;

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println(timeStr);
}

void drawDate() {
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
  display.setCursor(0, 30);
  display.println(dateStr);
}

void loop() {
  t = rtc.getTime();
  display.clearDisplay();
  drawTime();
  drawDate();
  display.display();
  delay(1000);
}

