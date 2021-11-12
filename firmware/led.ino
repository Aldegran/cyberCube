#include <Adafruit_NeoPixel.h>
#define LED_PIN 27
#define LED_COUNT 45
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
extern SettingsStruct encoderData;
extern int userColor(byte n);

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
  showUser(n, false);
  setleds();
  strip.show();
  timers[1] = millis();
}
void setleds() {
  for (byte i = 0; i < 6;i++) {
    strip.setPixelColor(tn(i * 7 - 1), ledColors[i + 1]);
    strip.setPixelColor(i * 7, ledColors[i + 1]);
    strip.setPixelColor(i * 7 + 1, ledColors[i + 1]);
  }
}
void showUser(byte n, byte erase) {
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
  if (millis() - timers[1] > 10) {
    if (n != encoderData.value[2]) {
      showUser(n, true);
      n = encoderData.value[2];
      //if (n == LED_COUNT)n = 0;
      showUser(n, false);
      setleds();
      strip.show();
    }
    timers[1] = millis();
  }
}