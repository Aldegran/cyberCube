
#include <Adafruit_GFX.h>
#include <Waveshare4InchTftShield.h>
#include "Aurebesh10pt7b.h"

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define WIDTH   320
#define HWIDTH  160
#define BORDER  20
#define BWIDTH HWIDTH-BORDER
#define ELEMENTS 32
#define STEP (WIDTH-BORDER*2)/ELEMENTS

Waveshare4InchTftShield Waveshield;
Adafruit_GFX& tft = Waveshield;
extern GameSettingsStruct gameSettings;
extern SettingsStruct encoderData;
extern RFIDSettingsStruct RFIDSettings;
extern byte gameMode;
extern byte mode;
extern void eraseLeds();
extern void setleds();
extern int userColor(byte n);

int colors[7] = { WHITE, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA };
int userColorsList[7] = { RED, YELLOW, GREEN, CYAN, BLUE, MAGENTA, WHITE };

uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
}

int randomLimit(int value, int randomValue, int minValue, int maxValue) {
  value += random(randomValue) - float((randomValue - 1) / 2 + 0.2);
  value = min(value, maxValue);
  value = max(value, minValue);
  return value;
}

void displaySetup() {
  if (!Waveshield.begin()) {
    Serial.println("Display init\t[FAIL]");
    return;
  }
  gameMode = 0;
  mode = 0;
  delay(500);
  Serial.println("Display init\t[OK]");
  tft.setRotation(2);
  tft.setFont(&Aurebesh10pt7b);
  tft.fillScreen(BLACK);
  tft.setTextColor(YELLOW, BLACK);
  tft.setCursor(1, 14);
  tft.setTextSize(1);
}
void showQR(char* qrName) {
  tft.fillRoundRect(0, 0, WIDTH, WIDTH, 15, WHITE);
  drawFileMC(qrName, 29, 29, 10, 15, 15, WHITE, BLACK);
}
void drawFileMC(char* qrName, int w, int h, byte zoom, int x, int y, int color, int bg) {
  File file = readFile(qrName);
  if (!file) {
    Serial.printf("Failed to open %s for reading\r\n", qrName);
    return;
  }
  int offsetX = x, offsetY = y, t = 0, r = 0;
  x = 0;
  y = 0;
  int16_t byteWidth = (w + 7) / 8;
  byte b = file.read();
  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++) {
      if (i & 7) b <<= 1;
      else {
        r = j * byteWidth + i / 8;
        if (r > t) {
          b = file.read();
          t = r;
        }
      }
      tft.fillRect((x + i) * zoom + offsetX, y * zoom + offsetY, zoom, zoom, (b & 0x80) ? color : bg);
    }
  }
  closeFile(file);
  mode = 2;
}

void displayLoop() {
  switch (mode) {
  case 0:
    showQR("code1.qr");
    break;
  case 1:
    displayGame();
    break;

  default:
    break;
  }
}