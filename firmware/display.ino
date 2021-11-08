
#include <Adafruit_GFX.h>
#include <Waveshare4InchTftShield.h>
#include <Fonts/FreeMonoBoldOblique12pt7b.h>

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Waveshare4InchTftShield Waveshield;
Adafruit_GFX& tft = Waveshield;

uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
}

void displaySetup() {
  if (!Waveshield.begin()) {
    Serial.println("Display init\t[FAIL]");
    return;
  }
  delay(500);
  Serial.println("Display init\t[OK]");
  tft.setRotation(2);
  tft.setFont(&FreeMonoBoldOblique12pt7b);
  tft.fillScreen(BLACK);
  tft.setTextColor(GREEN);
  tft.setCursor(1, 14);
  tft.setTextSize(1);
  tft.print("F_CPU:");
  tft.print(0.000001 * F_CPU);
  tft.println("MHz");
  tft.drawRect(0, 0, 320, 320, GREEN);

  int           i, i2,
    cx = tft.width() / 2 - 1,

    n = min(tft.width(), tft.height());
  for (i = n; i > 0; i -= 6) {
    i2 = i / 2;
    tft.drawRect(cx - i2, cx - i2, i, i, GREEN);
  }
}