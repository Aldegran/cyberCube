#include "PCF8575.h"

#define BUTTON_INT_PIN_A 27
#define BUTTON_INT_PIN_B 25
#define BUZZER_PIN 26

PCF8575 EXT(0x20);
extern unsigned long timers[2];
extern ButtonsStruct buttons;
extern byte current;
bool inited = false;

int l[4] = { 0b11111110, 0b11111101, 0b11110111, 0b11111011 };
byte lc = 0;

void setupExtender() {
  pinMode(BUTTON_INT_PIN_A, INPUT_PULLUP);
  pinMode(BUTTON_INT_PIN_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_INT_PIN_A), readButtons, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BUTTON_INT_PIN_B), readButtons, CHANGE);
  ledcSetup(0, BUZ_FR, 10);
  ledcAttachPin(BUZZER_PIN, 0);
  if (!EXT.begin()) {
    Serial.println("EXT init\t[FAIL]");
  } else if (!EXT.isConnected()) {
    Serial.println("EXT connect\t[FAIL]");
  } else {
    Serial.println("EXT init\t[OK]");
    EXT.write16(0xFFFF);
    inited = true;
  }
  timers[0] = millis();
}
void extenderLoop() {
  if (buttons.speed) {
    if (millis() - timers[0] > buttons.speed) {
      if (inited) {
        byte r = l[lc];
        if (buttons.mode > 3) r &= ~(1 << 6 - lc);
        else r &= ~(1 << 4 + lc);
        if (current) r &= ~(1 << 7);
        if (buttons.mode && buttons.mode != 10)EXT.write16(r);
        lc++;
        if (lc == 4) lc = 0;
      }
      timers[0] = millis();
      ledcWrite(0, buttons.speed);
      delay(20);
      ledcWrite(0, 0);
    }
  }
}

void off() {
  EXT.write16(current ? 0xFF7F : 0xFFFF);
  Serial.println("Off");
}

void readButtons() {
  buttons.a = !digitalRead(BUTTON_INT_PIN_A);
  buttons.b = !digitalRead(BUTTON_INT_PIN_B);
  buttons.flag = true;
  buttons.firstInit = true;
}
