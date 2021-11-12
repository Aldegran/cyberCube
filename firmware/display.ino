
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
extern SettingsStruct encoderData;
extern int userColor(byte n);

int colors[7] = { WHITE, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA };
int userColorsList[7] = { RED, YELLOW, GREEN, CYAN, BLUE, MAGENTA, WHITE };
typedef struct {
  int gray;
  byte userRadius;
  byte cycle;
  byte lineCycle;
  byte mode;
  byte gameMode;
  byte lineStep;
  int markerPosition[2];
  int centerPosition[4];
  int randomPosition[2];
  int lineColor[2];
  int lines[4][ELEMENTS + 1];
  byte lineChance[2];
  byte lineColorChance[2];
  byte lineChaos[2];
  int score[5];
} __attribute__((packed, aligned(1))) displayInfoStruct;

displayInfoStruct displayInfo;

uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
}

void displaySetup() {
  if (!Waveshield.begin()) {
    Serial.println("Display init\t[FAIL]");
    return;
  }
  displayInfo.gameMode = 0;
  displayInfo.mode = 1;
  delay(500);
  Serial.println("Display init\t[OK]");
  tft.setRotation(2);
  tft.setFont(&Aurebesh10pt7b);
  tft.fillScreen(BLACK);
  tft.setTextColor(YELLOW, BLACK);
  tft.setCursor(1, 14);
  tft.setTextSize(1);
  /*tft.print("F_CPU:");
  tft.print(0.000001 * F_CPU);
  tft.println("MHz Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nunc varius mollis turpis, quis tincidunt velit. Nulla efficitur dolor congue nisi volutpat tempor. Ut fringilla quam nunc, vitae hendrerit nunc fringilla at. ");
  tft.drawRect(0, 0, 320, 320, GREEN);*/

  /*int i, i2, cx = tft.width() / 2 - 1,

    n = min(tft.width(), tft.height());
  for (i = n; i > 0; i -= 6) {
    i2 = i / 2;
    tft.drawRect(cx - i2, cx - i2, i, i, GREEN);
  }*/
}

void drawMarker(byte erase) {
  int uc = erase ? BLACK : userColorsList[userColor(encoderData.value[2])];
  for (byte i = 0;i < 4;i++)
    tft.drawCircle(displayInfo.markerPosition[0], displayInfo.markerPosition[1], displayInfo.userRadius - i, uc);
  tft.drawFastHLine(15, displayInfo.markerPosition[1], displayInfo.markerPosition[0] - 30, erase ? BLACK : displayInfo.gray);
  tft.drawFastHLine(displayInfo.markerPosition[0] + 15, displayInfo.markerPosition[1], (WIDTH - displayInfo.markerPosition[0]) - 30, erase ? BLACK : displayInfo.gray);

  tft.drawFastVLine(displayInfo.markerPosition[0], 15, displayInfo.markerPosition[1] - 30, erase ? BLACK : displayInfo.gray);
  tft.drawFastVLine(displayInfo.markerPosition[0], displayInfo.markerPosition[1] + 15, (WIDTH - displayInfo.markerPosition[1]) - 30, erase ? BLACK : displayInfo.gray);
}

void drawGame(byte erase) {
  if (erase) {
    //tft.drawCircle(displayInfo.centerPosition[0], displayInfo.centerPosition[1], 10, BLACK);
    if (displayInfo.cycle == displayInfo.lineCycle) displayInfo.cycle = 0;
  }
  if (displayInfo.cycle == 0) {
    if ((erase && displayInfo.lines[0][ELEMENTS - 1] != displayInfo.lines[0][ELEMENTS]) || !erase)
      tft.drawTriangle(
        displayInfo.lines[0][ELEMENTS - 1] + BWIDTH + BORDER, WIDTH - 18,
        displayInfo.lines[0][ELEMENTS - 1] + BWIDTH + BORDER + 3, WIDTH - 14,
        displayInfo.lines[0][ELEMENTS - 1] + BWIDTH + BORDER - 3, WIDTH - 14,
        erase ? BLACK : colors[displayInfo.lineColor[0]]
      );
    if ((erase && displayInfo.lines[1][ELEMENTS - 1] != displayInfo.lines[1][ELEMENTS]) || !erase)
      tft.drawTriangle(
        WIDTH - 18, displayInfo.lines[1][ELEMENTS - 1] + BWIDTH + BORDER,
        WIDTH - 14, displayInfo.lines[1][ELEMENTS - 1] + BWIDTH + BORDER + 3,
        WIDTH - 14, displayInfo.lines[1][ELEMENTS - 1] + BWIDTH + BORDER - 3,
        erase ? BLACK : colors[displayInfo.lineColor[1]]
      );
    if ((erase && displayInfo.lines[0][0] != displayInfo.lines[0][1]) || !erase)
      tft.drawTriangle(
        displayInfo.lines[0][0] + BWIDTH + BORDER, 18,
        displayInfo.lines[0][0] + BWIDTH + BORDER + 3, 14,
        displayInfo.lines[0][0] + BWIDTH + BORDER - 3, 14,
        erase ? BLACK : colors[displayInfo.lines[2][0]]
      );
    if ((erase && displayInfo.lines[1][0] != displayInfo.lines[1][1]) || !erase)
      tft.drawTriangle(
        18, displayInfo.lines[1][0] + BWIDTH + BORDER,
        14, displayInfo.lines[1][0] + BWIDTH + BORDER + 3,
        14, displayInfo.lines[1][0] + BWIDTH + BORDER - 3,
        erase ? BLACK : colors[displayInfo.lines[3][0]]
      );

    for (byte i = 0; i < ELEMENTS + 1;i++) {
      if (i < ELEMENTS) {
        int color1 = erase ? BLACK : colors[displayInfo.lines[2][i]];
        int color2 = erase ? BLACK : colors[displayInfo.lines[3][i]];
        for (int b = -2; b < 3; b += 4) {
          //for (int b = 0; b < 1; b += 1) {
          if (i == ELEMENTS - 1) {
            if ((erase && displayInfo.lines[0][i] != displayInfo.lines[0][i + 1]) || !erase)
              tft.drawFastVLine(BORDER + BWIDTH + displayInfo.lines[0][i] + b, BORDER + i * STEP, STEP, color1);
            if ((erase && displayInfo.lines[1][i] != displayInfo.lines[1][i + 1]) || !erase)
              tft.drawFastHLine(BORDER + i * STEP, BORDER + BWIDTH + displayInfo.lines[1][i] + b, STEP, color2);
          } else {
            if (displayInfo.lines[0][i] == displayInfo.lines[0][i + 1]) {
              if ((erase && (displayInfo.lines[0][i] != displayInfo.lines[0][i + 2])) || !erase)
                tft.drawFastVLine(BORDER + BWIDTH + displayInfo.lines[0][i] + b, BORDER + i * STEP, STEP + 1, color1);
            } else {
              tft.drawLine(
                BORDER + BWIDTH + displayInfo.lines[0][i] + b,
                BORDER + i * STEP,
                BORDER + BWIDTH + displayInfo.lines[0][i + 1] + b,
                BORDER + (i + 1) * STEP,
                color1);
            }
            if (displayInfo.lines[1][i] == displayInfo.lines[1][i + 1]) {
              if ((erase && (displayInfo.lines[1][i] != displayInfo.lines[1][i + 2])) || !erase)
                tft.drawFastHLine(BORDER + i * STEP, BORDER + BWIDTH + displayInfo.lines[1][i] + b, STEP + 1, color2);
            } else {
              tft.drawLine(
                BORDER + i * STEP,
                BORDER + BWIDTH + displayInfo.lines[1][i] + b,
                BORDER + (i + 1) * STEP,
                BORDER + BWIDTH + displayInfo.lines[1][i + 1] + b,
                color2);
            }
          }
        }
      }
      if (erase && i && displayInfo.cycle == 0) {
        displayInfo.lines[0][i - 1] = displayInfo.lines[0][i];
        displayInfo.lines[1][i - 1] = displayInfo.lines[1][i];
        displayInfo.lines[2][i - 1] = displayInfo.lines[2][i];
        displayInfo.lines[3][i - 1] = displayInfo.lines[3][i];
      }
    }

    if (!erase) {
      if (encoderData.delta[2]) {
        tft.drawRoundRect(0, 0, WIDTH, WIDTH, 10, userColorsList[userColor(encoderData.value[2])]);
        encoderData.delta[2] = 0;
      }
      if (displayInfo.score[0] % 20 == 0) {
        tft.fillRect(52, WIDTH + 17, WIDTH - 64, 21, BLACK);
        int p = map(min(displayInfo.score[0], 3000), 0, 3000, 0, WIDTH - 64);
        tft.drawRoundRect(50, WIDTH + 15, WIDTH - 60, 25, 3, displayInfo.score[0] > 50 ? YELLOW : RED);
        tft.setTextColor(displayInfo.score[0] > 50 ? YELLOW : RED, BLACK);
        tft.setCursor(1, WIDTH + 32);
        tft.print("as");
        tft.fillRect(52, WIDTH + 17, p, 21, GREEN);
        for (int i = 10;i < WIDTH;i += 10) tft.drawFastVLine(52 + i, WIDTH + 17, 21, BLACK);
      }
    }
  }
}

void gameCenterCalculate() {
  byte i, n, found = false;
  for (i = 0; i < ELEMENTS;i++) {
    if (found) break;
    for (n = 0; n < ELEMENTS;n++) {
      if (abs(BWIDTH + displayInfo.lines[0][i] - n * STEP) < STEP && abs(BWIDTH + displayInfo.lines[1][n] - i * STEP) < STEP) {
        displayInfo.centerPosition[0] = BORDER + BWIDTH + displayInfo.lines[0][i];
        displayInfo.centerPosition[1] = BORDER + BWIDTH + displayInfo.lines[1][n];
        displayInfo.centerPosition[2] = displayInfo.lines[2][i];
        displayInfo.centerPosition[3] = displayInfo.lines[3][n];
        found = true;
        break;
      }
      if (BWIDTH + displayInfo.lines[0][i] - n * STEP < 0) break;
    }
  }
  if (found) {
    //tft.drawCircle(displayInfo.centerPosition[0], displayInfo.centerPosition[1], 5, MAGENTA);
  }
}

int randomLimit(int value, int randomValue, int minValue, int maxValue) {
  value += random(randomValue) - float((randomValue - 1) / 2 + 0.2);
  value = min(value, maxValue);
  value = max(value, minValue);
  return value;
}


void displayGame() {
  switch (displayInfo.gameMode) {
  case 0:
    displayInfo.gameMode = 1;
    for (byte i = 0; i < 2;i++) {
      displayInfo.markerPosition[i] = 160;
      displayInfo.centerPosition[i] = 160;
      displayInfo.randomPosition[i] = 0;
      displayInfo.lineChance[i] = 10;
      displayInfo.lineChaos[i] = 20;
      displayInfo.lineColor[i] = 0;
      displayInfo.lineColorChance[i] = 50;
    }
    displayInfo.userRadius = 10;
    displayInfo.lineCycle = 4;
    displayInfo.lineStep = 2;
    displayInfo.cycle = 0;
    displayInfo.score[0] = 100; //current
    displayInfo.score[1] = 3; //right position
    displayInfo.score[2] = 3; //right color
    displayInfo.score[3] = 9; //wrong position
    displayInfo.score[4] = 9; //wrong color
    encoderData.delta[0] = 1;
    displayInfo.gray = color565(127, 127, 127);
    tft.drawRoundRect(0, 0, WIDTH, WIDTH, 10, RED);
    tft.drawRoundRect(50, WIDTH + 15, WIDTH - 60, 25, 3, YELLOW);
    tft.setCursor(1, WIDTH + 32);
    tft.print("as");
    tft.drawCircle(displayInfo.markerPosition[0], displayInfo.markerPosition[1], 10, RED);
    for (byte i = 0; i < ELEMENTS + 1;i++) {
      displayInfo.lines[0][i] = 0;
      displayInfo.lines[1][i] = 0;
      displayInfo.lines[2][i] = 0;
      displayInfo.lines[3][i] = 0;
    }
    for (int i = BORDER; i < WIDTH - BORDER; i += 10) {
      tft.drawFastVLine(i, 3, i % 50 ? 5 : 9, displayInfo.gray);
      tft.drawFastVLine(i, WIDTH - 3, i % 50 ? -5 : -9, displayInfo.gray);
      tft.drawFastHLine(3, i, i % 50 ? 5 : 9, displayInfo.gray);
      tft.drawFastHLine(WIDTH - 3, i, i % 50 ? -5 : -9, displayInfo.gray);
    }
    tft.setTextSize(1);
    break;
  case 1: {
    byte needDrawMarker = false;
    if (encoderData.delta[0]) {
      drawMarker(true);
      displayInfo.markerPosition[0] = BORDER + BWIDTH + encoderData.value[0];
      encoderData.delta[0] = 0;
      needDrawMarker = true;
    } else  if (encoderData.delta[1]) {
      drawMarker(true);
      displayInfo.markerPosition[1] = BORDER + BWIDTH + encoderData.value[1];
      encoderData.delta[1] = 0;
      needDrawMarker = true;
    }
    drawGame(true);
    for (byte i = 0; i < 2;i++) {
      if (displayInfo.cycle == 0) {
        if (abs(displayInfo.randomPosition[i] - displayInfo.lines[i][ELEMENTS]) <= displayInfo.lineStep) {
          if (random(100) > 100 - displayInfo.lineChance[i]) {
            displayInfo.randomPosition[i] = randomLimit(displayInfo.randomPosition[i], displayInfo.lineChaos[i], -BWIDTH + BORDER * 2, BWIDTH - BORDER * 2);
            if (random(100) > 100 - displayInfo.lineColorChance[i]) {
              if (!displayInfo.lineColor[i]) {
                displayInfo.lineColor[i] = random(3) + 1;
              } else displayInfo.lineColor[i] = 0;
            }
          }
        } else {
          displayInfo.lines[i][ELEMENTS] = displayInfo.lines[i][ELEMENTS] + (displayInfo.randomPosition[i] > displayInfo.lines[i][ELEMENTS] ? displayInfo.lineStep : -displayInfo.lineStep);
          displayInfo.lines[i + 2][ELEMENTS] = displayInfo.lineColor[i];
        }
      }
    }
    if (displayInfo.cycle == 0) {
      needDrawMarker = true;
      int d = sqrt(sq(displayInfo.centerPosition[0] - displayInfo.markerPosition[0]) + sq(displayInfo.centerPosition[1] - displayInfo.markerPosition[1]));
      byte c1 = displayInfo.centerPosition[2];
      byte c2 = displayInfo.centerPosition[3];
      byte uc = userColor(encoderData.value[2]);
      int colorBonus = 0;
      if (c1 == c2) {
        if (c1 == 1) colorBonus = (uc == 0) ? displayInfo.score[2] : -displayInfo.score[4];
        else if (c1 == 2) colorBonus = (uc == 2) ? displayInfo.score[2] : -displayInfo.score[4];
        else if (c1 == 3) colorBonus = (uc == 4) ? displayInfo.score[2] : -displayInfo.score[4];
        else colorBonus = displayInfo.score[2];
      } else {
        if (c1 == 1) {
          if (c2 == 2) colorBonus = (uc == 1) ? displayInfo.score[2] : -displayInfo.score[4];
          else if (c2 == 3) colorBonus = (uc == 5) ? displayInfo.score[2] : -displayInfo.score[4];
          else colorBonus = (uc == 0 || uc == 1 || uc == 5) ? displayInfo.score[2] : -displayInfo.score[4];
        } else if (c1 == 2) {
          if (c2 == 1) colorBonus = (uc == 1) ? displayInfo.score[2] : -displayInfo.score[4];
          else if (c2 == 3) colorBonus = (uc == 3) ? displayInfo.score[2] : -displayInfo.score[4];
          else colorBonus = (uc == 1 || uc == 2 || uc == 3) ? displayInfo.score[2] : -displayInfo.score[4];
        } else if (c1 == 3) {
          if (c2 == 1) colorBonus = (uc == 5) ? displayInfo.score[2] : -displayInfo.score[4];
          else if (c2 == 2) colorBonus = (uc == 3) ? displayInfo.score[2] : -displayInfo.score[4];
          else colorBonus = (uc == 3 || uc == 4 || uc == 5) ? displayInfo.score[2] : -displayInfo.score[4];
        } else {
          if (c2 == 1) colorBonus = (uc == 0 || uc == 1 || uc == 5) ? displayInfo.score[2] : -displayInfo.score[4];
          else if (c2 == 2) colorBonus = (uc == 1 || uc == 2 || uc == 3) ? displayInfo.score[2] : -displayInfo.score[4];
          else colorBonus = (uc == 3 || uc == 4 || uc == 5) ? displayInfo.score[2] : -displayInfo.score[4];
        }
      }
      //if (d <= displayInfo.userRadius) displayInfo.score[0] += displayInfo.score[1];
      //else displayInfo.score[0] -= displayInfo.score[3];
      displayInfo.score[0] += colorBonus;
      if (displayInfo.score[0] < 0) displayInfo.score[0] = 0;
    }
    drawGame(false);
    if (needDrawMarker) drawMarker(false);
    gameCenterCalculate();
    displayInfo.cycle++;
    delay(30);
  }
        break;

  default:
    break;
  }
}

void displayLoop() {
  switch (displayInfo.mode) {
  case 1:
    displayGame();
    break;

  default:
    break;
  }
}