typedef struct {
  int gray;
  byte userRadius;
  byte cycle;
  byte lineCycle;
  int lineStep;
  int markerPosition[2];
  int centerPosition[4];
  int randomPosition[2];
  int lineColor[2];
  int lines[4][ELEMENTS + 1];
  byte lineChance[2];
  byte lineColorChance[2];
  byte lineChaos[2];
  int score[5];
  byte level;
} __attribute__((packed, aligned(1))) displayInfoStruct;

extern byte mode;
extern byte gameMode;
extern void setLeds();

displayInfoStruct displayInfo;

void drawMarker(byte erase) {
  int uc = erase ? BLACK : userColorsList[userColor(encoderData.value[2])];
  for (byte i = 0;i < 5;i += 2)
    tft.drawCircle(displayInfo.markerPosition[0], displayInfo.markerPosition[1], displayInfo.userRadius - i, uc);
  tft.drawFastHLine(15, displayInfo.markerPosition[1], displayInfo.markerPosition[0] - 30, erase ? BLACK : displayInfo.gray);
  tft.drawFastHLine(displayInfo.markerPosition[0] + 15, displayInfo.markerPosition[1], (WIDTH - displayInfo.markerPosition[0]) - 30, erase ? BLACK : displayInfo.gray);

  tft.drawFastVLine(displayInfo.markerPosition[0], 15, displayInfo.markerPosition[1] - 30, erase ? BLACK : displayInfo.gray);
  tft.drawFastVLine(displayInfo.markerPosition[0], displayInfo.markerPosition[1] + 15, (WIDTH - displayInfo.markerPosition[1]) - 30, erase ? BLACK : displayInfo.gray);
}

void drawGame(byte erase) {
  if (erase) {
    //tft.drawCircle(displayInfo.centerPosition[0], displayInfo.centerPosition[1], 10, BLACK);
    if (displayInfo.cycle >= displayInfo.lineCycle) displayInfo.cycle = 0;
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
        int p = map(min(displayInfo.score[0], RFIDSettings.maxScore), 0, RFIDSettings.maxScore, 0, WIDTH - 64);
        tft.drawRoundRect(50, WIDTH + 15, WIDTH - 60, 25, 3, displayInfo.score[0] > 50 ? YELLOW : RED);
        tft.setTextColor(displayInfo.score[0] > 50 ? YELLOW : RED, BLACK);
        tft.setCursor(1, WIDTH + 32);
        tft.print("as");
        tft.fillRect(52, WIDTH + 17, p, 21, GREEN);
        for (int i = 62;i < WIDTH;i += (WIDTH - 62) / 10) tft.drawFastVLine(i, WIDTH + 17, 21, BLACK);
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

void displayGame() {
  switch (gameMode) {
  case 0:
    gameMode = 1;
    tft.fillScreen(BLACK);
    setLeds();
    showUserZero(false);
    break;


  case 1:
    gameMode = 2;
    for (byte i = 0; i < 2;i++) {
      displayInfo.markerPosition[i] = 160;
      displayInfo.centerPosition[i] = 160;
      displayInfo.randomPosition[i] = 0;
      displayInfo.lineColorChance[i] = RFIDSettings.lineColorChance[i];
      displayInfo.lineChance[i] = RFIDSettings.lineChance[i];
      displayInfo.lineChaos[i] = RFIDSettings.lineChaos[i];
      displayInfo.lineColor[i] = 0;
    }
    displayInfo.userRadius = RFIDSettings.userRadius;
    displayInfo.lineCycle = RFIDSettings.lineCycle;
    displayInfo.lineStep = RFIDSettings.lineStep;
    displayInfo.cycle = 0;
    displayInfo.score[0] = 100; //current
    displayInfo.score[1] = RFIDSettings.positionBonus[0]; //right position
    displayInfo.score[2] = RFIDSettings.colorBonus[0]; //right color
    displayInfo.score[3] = RFIDSettings.positionBonus[1]; //wrong position
    displayInfo.score[4] = RFIDSettings.colorBonus[0]; //wrong color
    encoderData.delta[0] = 1;
    displayInfo.level = 0; //level
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


  case 2: {
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
      /*if (d <= displayInfo.userRadius) displayInfo.score[0] += displayInfo.score[1] + colorBonus;
      else displayInfo.score[0] -= displayInfo.score[3];*/
      displayInfo.score[0] += colorBonus;
      if (displayInfo.score[0] < 0) { //lose
        displayInfo.score[0] = 0;
        drawGame(false);
        delay(5000);
        gameMode = 4;
        eraseLeds();
        delay(5000);
        tft.fillScreen(BLACK);
        tft.setCursor(100, 100);
        tft.print("LOSE");
        resetLedCS();
        mode = GAME_MODE_CAPSULE_GAME_OK;
        statusChanged();
      }
      if (displayInfo.score[0] > RFIDSettings.maxScore) { //win
        displayInfo.score[0] = RFIDSettings.maxScore;
        drawGame(false);
        delay(5000);
        gameMode = 3;
        eraseLeds();
        delay(5000);
        tft.fillScreen(BLACK);
        tft.setCursor(100, 100);
        tft.print("WIN");
        resetLedCS();
        mode = GAME_MODE_CAPSULE_GAME_FAIL;
      }
      byte currentlevel = map(displayInfo.score[0], 0, RFIDSettings.maxScore, 0, gameSettings.levels);
      if (currentlevel != displayInfo.level) {
        displayInfo.level = currentlevel;
        displayInfo.lineColorChance[0] = RFIDSettings.lineColorChance[0] + (currentlevel * gameSettings.lineColorChanceUp);
        displayInfo.lineColorChance[1] = RFIDSettings.lineColorChance[1] + (currentlevel * gameSettings.lineColorChanceUp);
        displayInfo.lineChaos[0] = RFIDSettings.lineChaos[0] + (currentlevel * gameSettings.lineChaosUp);
        displayInfo.lineChaos[1] = RFIDSettings.lineChaos[1] + (currentlevel * gameSettings.lineChaosUp);
        displayInfo.lineChance[0] = RFIDSettings.lineChance[0] + (currentlevel * gameSettings.lineChanceUp);
        displayInfo.lineChance[1] = RFIDSettings.lineChance[1] + (currentlevel * gameSettings.lineChanceUp);
        displayInfo.lineCycle = RFIDSettings.lineCycle - (currentlevel * gameSettings.lineCycleDown);
        displayInfo.userRadius = RFIDSettings.userRadius - (currentlevel * gameSettings.userRadiusDown);
        displayInfo.lineStep = RFIDSettings.lineStep + (currentlevel * gameSettings.lineStepUp);
        tft.fillCircle(displayInfo.markerPosition[0], displayInfo.markerPosition[1], RFIDSettings.userRadius, BLACK);
      }
    }
    if (gameMode == 2) {
      drawGame(false);
      if (needDrawMarker) drawMarker(false);
      gameCenterCalculate();
      displayInfo.cycle++;
      delay(10);
    }
  } break;

  case 3: //win
    break;
  case 4: //lose
    break;

  default:
    break;
  }
}
