#include <Adafruit_NeoPixel.h>
#define LED_PIN 27
#define LED_COUNT 45
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
extern SettingsStruct encoderData;
extern int userColor(byte n);
extern byte gameMode;
extern byte mode;

byte n = 0;
uint32_t ledColors[9];
void ledSetup() {
  ledColors[0] = strip.Color(0, 0, 0);
  ledColors[1] = strip.Color(255, 0, 0);
  ledColors[2] = strip.Color(255, 255, 0);
  ledColors[3] = strip.Color(0, 255, 0);
  ledColors[4] = strip.Color(0, 255, 255);
  ledColors[5] = strip.Color(0, 0, 255);
  ledColors[6] = strip.Color(255, 0, 255);
  ledColors[7] = strip.Color(255, 255, 255);
  ledColors[8] = strip.Color(50, 50, 50);
  strip.begin();
  strip.setBrightness(100);
  //showUser(false);
  //setLeds();
  strip.show();
  timers[1] = millis();
}

void eraseLeds() {
  for (byte i = 0; i < LED_COUNT;i++) strip.setPixelColor(i, 0);
  strip.show();
}


void showUser(byte erase) {
  byte mainColor = userColor(n) + 1;
  strip.setPixelColor(tn(n - 2), ledColors[erase ? 0 : 8]);
  strip.setPixelColor(tn(n - 1), ledColors[erase ? 0 : mainColor]);
  strip.setPixelColor(n, ledColors[erase ? 0 : mainColor]);
  strip.setPixelColor(tn(n + 1), ledColors[erase ? 0 : mainColor]);
  strip.setPixelColor(tn(n + 2), ledColors[erase ? 0 : 8]);
}
byte tn(int n) {
  if (n < 0) return 45 + n;
  if (n > 44) return n - 45;
  return n;
}
void ledLoop() {
  if (millis() - timers[1] > 10 && mode == 1 && gameMode == 2) {
    if (n != encoderData.value[2]) {
      showUser(true);
      n = encoderData.value[2];
      //if (n == LED_COUNT)n = 0;
      showUser(false);
      setLeds();
      strip.show();
    }
    timers[1] = millis();
  }
}

void ledFlash(){
  for (byte i = 0; i < LED_COUNT;i++) strip.setPixelColor(i, ledColors[7]);
  strip.show();
  delay(100);
  eraseLeds();
}

void ledIDLE(){
  eraseLeds();
}

void setLeds() {
  for (byte i = 0; i < 6;i++) {
    strip.setPixelColor(tn(i * 7 - 1), ledColors[i + 1]);
    strip.setPixelColor(i * 7, ledColors[i + 1]);
    strip.setPixelColor(i * 7 + 1, ledColors[i + 1]);
  }
}

void showUserZero(byte erase){
  n = 0;
  showUser(erase);
}