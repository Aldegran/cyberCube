#include "Ucglib.h"
#define WIDTH 320
#define HEIGHT 240
#define HEIGHT2 HEIGHT/2
#define WIDTH2 WIDTH-HEIGHT/2

float gr = PI / 180;

Ucglib_ILI9341_18x240x320_HWSPI ucg(LCD_DC, LCD_CS, LCD_RST);

extern unsigned long timers[2];
extern ButtonsStruct buttons;
extern byte TOTALL_COUNT;
int trBuf[4][2];
extern byte current;
bool fl = false;
bool displayInited = false;

int randomLimit(int value, int randomValue, int minValue, int maxValue) {
  value += random(randomValue) - float((randomValue - 1) / 2 + 0.2);
  value = min(value, maxValue);
  value = max(value, minValue);
  return value;
}
void displaySetup() {
  pinMode(LCD_BL, OUTPUT);
  ucg.begin(UCG_FONT_MODE_TRANSPARENT);
  ucg.clearScreen();
  ucg.setRotate90();
  ucg.setFont(ucg_font_ncenR12_tr);
  //ucg.setColor(50, 50, 50);
  //ucg.drawDisc(WIDTH2, HEIGHT2, HEIGHT2, UCG_DRAW_ALL);
  //ucg.setColor(0, 0, 0);
  //ucg.drawDisc(WIDTH2, HEIGHT2, HEIGHT2 - 50, UCG_DRAW_ALL);
  ucg.setColor(255, 255, 255);
  ucg.drawCircle(WIDTH2, HEIGHT2, HEIGHT2 - 50, UCG_DRAW_ALL);
  ucg.drawCircle(WIDTH2, HEIGHT2, HEIGHT2 - 55, UCG_DRAW_ALL);

  float gi = 0;
  float gi2 = 0;
  byte p = 360 / TOTALL_COUNT;
  byte pp = p * 0.4;
  ucg.setColor(255, 255, 255);
  for (int i = 0; i < 360; i += p) {
    gi = (i + pp) * gr;
    gi2 = (i - pp) * gr;
    trBuf[0][0] = WIDTH2 + cos(gi) * HEIGHT2;
    trBuf[0][1] = HEIGHT2 + sin(gi) * HEIGHT2;
    trBuf[1][0] = WIDTH2 + cos(gi2) * HEIGHT2;
    trBuf[1][1] = HEIGHT2 + sin(gi2) * HEIGHT2;
    trBuf[2][0] = WIDTH2 + cos(i * gr) * (HEIGHT2 - 8);
    trBuf[2][1] = HEIGHT2 + sin(i * gr) * (HEIGHT2 - 8);
    ucg.drawTriangle(trBuf[0][0], trBuf[0][1], trBuf[1][0], trBuf[1][1], trBuf[2][0], trBuf[2][1]);
  }
  ucg.setColor(0, 255, 0);
  for (int i = 0; i < TOTALL_COUNT; i++) drawEl(i);
  Serial.println("Display init\t[OK]");
  displayInited = true;
}

void drawEl(byte num) {
  byte r1 = HEIGHT2 - 10;
  byte r2 = HEIGHT2 - 40;
  byte p = 360 / TOTALL_COUNT;
  byte pp = p * 0.4;
  float gi = (num * p + pp) * gr;
  float gi2 = (num * p - pp) * gr;
  trBuf[0][0] = WIDTH2 + cos(gi) * r1;
  trBuf[0][1] = HEIGHT2 + sin(gi) * r1;
  trBuf[1][0] = WIDTH2 + cos(gi2) * r1;
  trBuf[1][1] = HEIGHT2 + sin(gi2) * r1;
  trBuf[3][0] = WIDTH2 + cos(gi) * r2;
  trBuf[3][1] = HEIGHT2 + sin(gi) * r2;
  trBuf[2][0] = WIDTH2 + cos(gi2) * r2;
  trBuf[2][1] = HEIGHT2 + sin(gi2) * r2;
  ucg.drawTetragon(trBuf[0][0], trBuf[0][1], trBuf[1][0], trBuf[1][1], trBuf[2][0], trBuf[2][1], trBuf[3][0], trBuf[3][1]);
}

void redrawAllelements() {
  if (!displayInited) return;
  ucg.setColor(0, 255, 0);
  for (int i = 0; i < TOTALL_COUNT; i++) drawEl(i);
}

void displayLoop() {
  displayButtons();
  if (!buttons.mode || buttons.mode == 10 || !displayInited) {
    timers[1] = millis();
    return;
  }
  if (millis() - timers[1] > 1000 && current) {
    timers[1] = millis();
    ucg.setColor(255, 0, 0);
    drawEl(current);
    fl = !fl;
    ucg.setColor(0, 0, fl ? 255 : 0);
    ucg.drawDisc(WIDTH2, HEIGHT2, HEIGHT2 - 60, UCG_DRAW_ALL);
    current--;
    if (!current) {
      updateMode();
      ledcSetup(0, 1000, 10);
      for (int i = 0x3ff; i > 0; i -= 10) {
        ledcSetup(0, i, 10);
        ledcWrite(0, i);
        delay(10);
      }
      ledcWrite(0, 0);
      ledcSetup(0, BUZ_FR, 10);
    }
  }
}

void displayButtons() {
  ucg.setColor(0, 0, buttons.a ? 255 : 0);
  ucg.drawBox(10, 10, 20, 20);
  ucg.setColor(0, buttons.b ? 255 : 0, 0);
  ucg.drawBox(40, 10, 20, 20);
}