
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
extern ConnectorsStatusStruct ConnectorsStatus;
extern byte gameMode;
extern byte mode;
extern unsigned long timers[3];
extern void eraseLeds();
extern void setLeds();
extern void updateStrip();
extern int userColor(byte n);

int colors[7] = { WHITE, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA };
int userColorsList[7] = { RED, YELLOW, GREEN, CYAN, BLUE, MAGENTA, WHITE };
int currentPerson = 0;

typedef struct {
  byte IDLE;
  byte beginCapsule;
  byte endCapsule;
  byte capsuleFail;
} __attribute__((packed, aligned(1))) AnimationsStruct;
AnimationsStruct Animations;

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
  pinMode(LCD_DC, OUTPUT);
  setLedCS();
  Waveshield.useExtender(
    setLedRes,
    resetLedRes,
    setLedDC,
    resetLedDC,
    emptyFunction,
    emptyFunction
  );
  if (!Waveshield.begin(1)) {
    Serial.println(F("Display init\t[FAIL]"));
    return;
  }
  gameMode = 0;
  mode = 1;
  delay(500);
  Serial.println(F("Display init\t[OK]"));
  tft.setRotation(2);
  tft.setFont(&Aurebesh10pt7b);
  tft.fillScreen(BLACK);
  tft.setTextColor(YELLOW, BLACK);
  tft.setCursor(1, 14);
  tft.setTextSize(1);
  tft.drawCircle(100, 100, 100, WHITE);
  Waveshield.setScreenBrightness(0xFF);
  resetLedCS();
}
void showQR(String qrName) {
  tft.fillRoundRect(0, 0, WIDTH, WIDTH, 10, WHITE);
  drawFileMC(qrName, 29, 29, 10, 15, 15, WHITE, BLACK);
}
void drawFileMC(String qrName, int w, int h, byte zoom, int x, int y, int color, int bg) {
  File file = readFile(qrName + ".qr");
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
      delay(5);
    }
  }
  closeFile(file);
  mode = 2;
}

void showIntro() {
  tft.fillScreen(BLACK);
  tft.setCursor(40, HWIDTH - 35);
  tft.setTextColor(YELLOW, BLACK);
  tft.print("Zavantazhenna");
  tft.drawRect(0, HWIDTH - 20, WIDTH, 40, YELLOW);
  for (int i = 5; i < WIDTH - 5;i += 10) {
    tft.fillRect(i, HWIDTH - 15, 8, 30, GREEN);
    //delay(100);
  }
  tft.fillRect(0, HWIDTH - 47, WIDTH, 17, BLACK);
  tft.setCursor(20, HWIDTH + 40);
  tft.print("Initchializhachiya");
  //delay(2000);

  tft.fillScreen(BLACK);
  tft.fillTriangle(0, 50, WIDTH, 50, HWIDTH, 0, WHITE);
  tft.fillTriangle(10, 40, WIDTH - 10, 40, HWIDTH, 10, BLACK);
  tft.drawTriangle(0, 50, WIDTH, 50, HWIDTH, 0, YELLOW);
  tft.setCursor(40, 70);
  tft.print("Ochikuyu knopku");
  tft.fillRect(0, 115, WIDTH, 10, WHITE);
  tft.setCursor(1, 140);
  tft.print("Pidgotuyte vashu capsulu do chytuvanna ta natysnyt knopku");
  tft.fillRect(70, 350, WIDTH - 140, 50, WHITE);
  tft.fillRect(70, 200, WIDTH - 140, 10, WHITE);
  tft.drawRect(70, 350, WIDTH - 140, -150, WHITE);
  tft.drawRect(HWIDTH - 30, 350, 60, -120, YELLOW);
  tft.fillRect(HWIDTH - 10, 232, 20, 50, BLUE);
  tft.fillRect(HWIDTH - 59, 250, -10, 40, WHITE);
  tft.fillRect(HWIDTH + 61, 250, 10, 40, WHITE);
  tft.fillTriangle(HWIDTH + 75, 290, HWIDTH + 55, 290, HWIDTH + 65, 300, WHITE);
  tft.fillTriangle(HWIDTH - 75, 290, HWIDTH - 55, 290, HWIDTH - 65, 300, WHITE);
}

void showCapsule(bool fast) {
  tft.fillScreen(BLACK);
  tft.setCursor(40, HWIDTH - 35);
  tft.print("Pidgotuvanna");
  tft.drawRect(0, HWIDTH - 20, WIDTH, 40, YELLOW);
  for (int i = 5; i < WIDTH - 5;i += 10) {
    tft.fillRect(i, HWIDTH - 15, 8, 30, BLUE);
    //delay(100);
  }
  //delay(2000);

  tft.fillScreen(BLACK);
  tft.fillRect(0, WIDTH - 50, 110, 10, WHITE);
  tft.fillRect(WIDTH - 100, WIDTH - 50, 100, 10, WHITE);
  tft.fillRect(100, 0, 10, WIDTH - 50, WHITE);
  tft.fillRect(WIDTH - 100, 0, 10, WIDTH - 50, WHITE);
  tft.fillRect(0, WIDTH - 50, 10, 50, WHITE);
  tft.fillRect(WIDTH - 10, WIDTH - 50, 10, 50, WHITE);
  tft.fillRect(100, 0, WIDTH - 200, 10, WHITE);
  tft.drawFastHLine(0, WIDTH, WIDTH, color565(127, 127, 127));

  tft.fillRect(WIDTH - 54, 200, -10, 40, WHITE);
  tft.fillRect(61, 200, 10, 40, WHITE);

  for (int i = WIDTH - 40; i > 40; i -= 45) {
    tft.drawRect(115, i, WIDTH - 220, -40, GREEN);
    if (!fast) delay(50);
  }

  if (fast) return;

  tft.fillTriangle(0, 480, WIDTH, 480, HWIDTH, 440, WHITE);
  tft.fillTriangle(30, 470, WIDTH - 30, 470, HWIDTH, 450, BLACK);
  tft.drawTriangle(0, 480, WIDTH, 480, HWIDTH, 440, YELLOW);

  tft.fillTriangle(75, 200, 55, 200, 65, 190, WHITE);
  tft.fillTriangle(WIDTH - 70, 200, WIDTH - 50, 200, WIDTH - 60, 190, WHITE);

  tft.setCursor(40, 410);
  tft.print("Potribna capsula");
}

uint16_t blueColors[6] = {
  color565(0,0,255),
  color565(55,55,255),
  color565(95,95,255),
  color565(135,135,255),
  color565(175,175,255),
  color565(215,215,255),
};

void beginCapsule() {
  tft.fillRect(0, 390, WIDTH, 90, BLACK);
  byte c = 0;
  for (int i = WIDTH - 45; i > 165; i -= 45) {
    tft.fillRect(120, i, WIDTH - 230, -30, blueColors[c++]);
    delay(300);
  }
}

void endCapsule(bool status) {
  if (!status) {
    tft.setCursor(40, WIDTH + 20);
    tft.print("Capsula pogana");
    for (int i = WIDTH - 45; i > 40; i -= 45) {
      tft.fillRect(120, i, WIDTH - 230, -30, RED);
    }

    tft.fillTriangle(75, 240, 55, 240, 65, 250, WHITE);
    tft.fillTriangle(WIDTH - 70, 240, WIDTH - 50, 240, WIDTH - 60, 250, WHITE);

    tft.fillTriangle(75, 200, 55, 200, 65, 190, BLACK);
    tft.fillTriangle(WIDTH - 70, 200, WIDTH - 50, 200, WIDTH - 60, 190, BLACK);

    tft.fillTriangle(0, 430, WIDTH, 430, HWIDTH, 480, WHITE);
    tft.fillTriangle(30, 440, WIDTH - 30, 440, HWIDTH, 470, RED);
    tft.drawTriangle(0, 430, WIDTH, 430, HWIDTH, 480, YELLOW);
    tft.setCursor(40, 410);
    tft.print("Vytagnit prystriy");
  } else {
    tft.setCursor(40, WIDTH + 20);
    tft.print("Capsula dobra");
    byte c = 0;
    for (int i = WIDTH - 45; i > 40; i -= 45) {
      tft.fillRect(120, i, WIDTH - 230, -30, blueColors[c++]);
      delay(300);
    }
    delay(2000);
  }
}

void displayOta(int percent) {
  if (percent < 0) {
    ConnectorsStatus.stopEXT = true;
    delay(20);
    setLedCS();
    tft.fillScreen(BLACK);
    tft.setCursor(80, HWIDTH - 35);
    tft.print("Onovlenna");
    tft.drawRect(0, HWIDTH - 20, WIDTH, 40, YELLOW);
    currentPerson = 0;
  } else {
    if (percent > currentPerson && percent < 100) {
      currentPerson = percent;
      uint16_t color = color565(255 - 2.54 * percent, 2.54 * percent, 0);//color565(100 + 1.5 * percent, 0, 100 + 1.5 * percent);
      uint16_t px = 3.1 * percent;
      tft.drawFastVLine(5 + px, HWIDTH - 15, 30, color);
      tft.drawFastVLine(6 + px, HWIDTH - 15, 30, color);
      tft.drawFastVLine(7 + px, HWIDTH - 15, 30, color);
      tft.fillRect(HWIDTH - 25, HWIDTH + 45, 50, -18, BLACK);
      tft.setCursor(HWIDTH - 20, HWIDTH + 44);
      tft.print(percent);
    }
  }
  //resetLedCS();
  //ConnectorsStatus.stopEXT = false;
}

void displayLoop() {
  switch (mode) {
    /*case 0:
      showQR("code1.qr");
      break;*/
  case GAME_MODE_CAPSULE_GAME:
    ConnectorsStatus.stopEXT = true;
    displayGame();
    ConnectorsStatus.stopEXT = false;
    break;
  case GAME_MODE_WAIT_ANIMATION:
    ConnectorsStatus.stopEXT = true;
    setLedCS();
    Waveshield.setScreenBrightness(0xFF);
    showCapsule(false);
    resetLedCS();
    ConnectorsStatus.stopEXT = false;
    mode = GAME_MODE_WAIT_CAPSULE;
    statusChanged();
    break;
  case GAME_MODE_CONNECT_LCD:
    Animations.beginCapsule = false;
    Animations.capsuleFail = false;
    Animations.endCapsule = false;
    Animations.IDLE = false;
    ConnectorsStatus.stopEXT = true;
    delay(20);
    setLedCS();
    showIntro();
    resetLedCS();
    ConnectorsStatus.stopEXT = false;
    //delay(3000);
    timers[2] = millis();
    mode = GAME_MODE_WAIT_BUTTON;
    statusChanged();
    break;
  case GAME_MODE_CAPSULE_READ:
    ConnectorsStatus.stopEXT = true;
    delay(20);
    setLedCS();
    endCapsule(true);
    resetLedCS();
    ConnectorsStatus.stopEXT = false;
    gameMode = 0;
    mode = GAME_MODE_CAPSULE_GAME;
    showUser(false);
    setLeds();
    updateStrip();
    statusChanged();
    setLedCS();
    break;
  case GAME_MODE_IDLE: {
    if (!Animations.IDLE) {
      Serial.println(F("Animation IDLE"));
      Animations.IDLE = true;
      ConnectorsStatus.stopEXT = true;
      delay(20);
      setLedCS();
      Waveshield.setScreenBrightness(0);
      tft.fillScreen(BLACK);
      resetLedCS();
      ConnectorsStatus.stopEXT = false;
    }
  }
                     break;
  case GAME_MODE_CAPSULE_BEGIN: {
    if (!Animations.beginCapsule) {
      Serial.println(F("Animation"));
      Animations.beginCapsule = true;
      ConnectorsStatus.stopEXT = true;
      delay(20);
      setLedCS();
      beginCapsule();
      resetLedCS();
      ConnectorsStatus.stopEXT = false;
    }
  }
                              break;
  case GAME_MODE_CAPSULE_FAIL_READ: {
    if (!Animations.capsuleFail) {
      Serial.println(F("Animation*"));
      Animations.capsuleFail = true;
      ConnectorsStatus.stopEXT = true;
      delay(20);
      setLedCS();
      endCapsule(false);
      resetLedCS();
      ConnectorsStatus.stopEXT = false;
      delay(20);
    }
    if (ConnectorsStatus.cylinderTop) { // вытащили капсулу после неудачного чтения
      if (!Animations.endCapsule) {
        Serial.println(F("Animation"));
        Animations.endCapsule = true;
        ConnectorsStatus.stopEXT = true;
        delay(20);
        setLedCS();
        tft.fillScreen(BLACK);
        resetLedCS();
        ConnectorsStatus.stopEXT = false;
      }
      mode = GAME_MODE_CONNECT_LCD;
      timers[2] = millis();
      statusChanged();
    }
  }
                                  break;
  case GAME_MODE_OTA: {
  }
  default:
    break;
  }
}