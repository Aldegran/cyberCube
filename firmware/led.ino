#include <Adafruit_NeoPixel.h>
#define LED_PIN 27
#define LED_COUNT 45
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

byte n = 0;

void ledSetup() {
  strip.begin();
  strip.setBrightness(255);
  strip.show();
  timers[1] = millis();
}

void ledLoop() {
  if (millis() - timers[1] > 10) {
    if (n != encoderData.value[2]) {
      strip.setPixelColor(n, strip.Color(0, 0, 0));
      n = encoderData.value[2];
      //if (n == LED_COUNT)n = 0;
      strip.setPixelColor(n, strip.Color(127, 127, 127));
      strip.show();
    }
    timers[1] = millis();
  }
}