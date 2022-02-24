
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
extern void setleds();
extern int userColor(byte n);

int colors[7] = { WHITE, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA };
int userColorsList[7] = { RED, YELLOW, GREEN, CYAN, BLUE, MAGENTA, WHITE };

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

void showIntro(){
  tft.fillScreen(BLACK);
  tft.setCursor(40, HWIDTH-35);
  tft.print("Zavantazhenna");
  tft.drawRect(0, HWIDTH-20, WIDTH, 40, YELLOW);
  for(int i=5; i<WIDTH-5;i+=10){
    tft.fillRect(i, HWIDTH-15, 8, 30, GREEN);
    delay(50);
  }
  tft.fillRect(0, HWIDTH-47, WIDTH, 17, BLACK);
  tft.setCursor(20, HWIDTH+40);
  tft.print("Initchializhachiya");
  delay(2000);
  
  tft.fillScreen(BLACK);
  tft.fillTriangle(0, 50, WIDTH, 50, HWIDTH, 0, WHITE);
  tft.fillTriangle(10, 40, WIDTH-10, 40, HWIDTH, 10, BLACK);
  tft.drawTriangle(0, 50, WIDTH, 50, HWIDTH, 0, YELLOW);
  tft.setCursor(40, 70);
  tft.print("Ochikuyu knopku");
  tft.fillRect(0, 115, WIDTH, 10, WHITE);
  tft.setCursor(1, 140);
  tft.print("Pidgotuyte vashu capsulu do chytuvanna ta natysnyt knopku");
}

void showCapsule(){
  tft.fillScreen(BLACK);
  tft.setCursor(40, HWIDTH-35);
  tft.print("Pidgotuvanna");
  tft.drawRect(0, HWIDTH-20, WIDTH, 40, YELLOW);
  for(int i=5; i<WIDTH-5;i+=10){
    tft.fillRect(i, HWIDTH-15, 8, 30, BLUE);
    delay(100);
  }
  delay(2000);
  
  tft.fillScreen(BLACK);
  tft.fillRect(0, WIDTH-50, 110, 10, WHITE);
  tft.fillRect(WIDTH-100, WIDTH-50, 100, 10, WHITE);
  tft.fillRect(100, 0, 10, WIDTH-50, WHITE);
  tft.fillRect(WIDTH-100, 0, 10, WIDTH-50, WHITE);
  tft.fillRect(0, WIDTH-50, 10, 50, WHITE);
  tft.fillRect(WIDTH-10, WIDTH-50, 10, 50, WHITE);
  tft.fillRect(100, 0, WIDTH-200, 10, WHITE);
  tft.drawFastHLine(0,WIDTH,WIDTH,color565(127,127,127));

  tft.fillTriangle(0, 430, WIDTH, 430, HWIDTH, 480, WHITE);
  tft.fillTriangle(10, 440, WIDTH-10, 440, HWIDTH, 470, BLACK);
  tft.drawTriangle(0, 430, WIDTH, 430, HWIDTH, 480, YELLOW);
  tft.setCursor(40, 410);
  tft.print("Potribna capsula");
  for(int i = WIDTH-40; i>40; i-=45){
    tft.drawRect(115, i, WIDTH-220, -40, GREEN);
    delay(50);
  }
}

void beginCapsule(){
  tft.fillRect(0, 390, WIDTH, 90, BLACK);
  for(int i = WIDTH-45; i>165; i-=45){
    tft.fillRect(120, i, WIDTH-230, -30, BLUE);
    delay(300);
  }
}

void displayLoop() {
  switch (mode) {
  /*case 0:
    showQR("code1.qr");
    break;*/
  case GAME_MODE_CAPSULE_GAME:
    //displayGame();
    delay(3000);
    mode = GAME_MODE_CAPSULE_GAME_OK;
    statusChanged();
    break;
  case GAME_MODE_WAIT_ANIMATION:
    ConnectorsStatus.stopEXT = true;
    setLedCS();
    Waveshield.setScreenBrightness(0xFF);
    showCapsule();
    resetLedCS();
    ConnectorsStatus.stopEXT = false;
    mode = GAME_MODE_WAIT_CAPSULE;
    statusChanged();
    break;
  case GAME_MODE_CONNECT_LCD:
    Animations.beginCapsule = false;
    Animations.endCapsule = false;
    Animations.capsuleFail = false;
    Animations.IDLE = false;
    ConnectorsStatus.stopEXT = true;
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
    delay(3000);
    mode = GAME_MODE_CAPSULE_GAME;
    statusChanged();
  break;
  case GAME_MODE_IDLE:{
    if(!Animations.IDLE){
      Animations.IDLE = true;
      ConnectorsStatus.stopEXT = true;
      setLedCS();
      Waveshield.setScreenBrightness(0);
      tft.fillScreen(BLACK);
      resetLedCS();
      ConnectorsStatus.stopEXT = false;
    }
  }
  break;
  case GAME_MODE_CAPSULE_BEGIN:{
    if(!Animations.beginCapsule){
      Animations.beginCapsule = true;
      ConnectorsStatus.stopEXT = true;
      setLedCS();
      beginCapsule();
      resetLedCS();
      ConnectorsStatus.stopEXT = false;
    }
  }
  break;
  default:
    break;
  }
}