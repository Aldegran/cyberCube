#include "PCF8575.h"

PCF8575 EXT_Led0(0x20);
PCF8575 EXT_Button(0x21);
PCF8575 EXT_Led1(0x22);
PCF8575 EXT_Led2(0x23);
PCF8575 EXT_Led3(0x24);
extern unsigned long timers[2];
unsigned int currentButtonValue = 0;
byte currentButton = 0;
const int EXT_BUTTON_INT_PIN = 15;
unsigned int buttonValues[12] = { 65534, 65533, 65531, 65527, 65519, 65503, 65471, 65407, 65279, 65023, 64511, 63487 };
byte inited[5] = { false,false,false,false,false };

volatile bool EXTButtonFlag = false;

void EXTButtonInterrupt() {
  EXTButtonFlag = true;
}

void setupExtender() {
  if (!EXT_Led0.begin()) {
    Serial.println("EXT_Led0 init\t[FAIL]");
  } else if (!EXT_Led0.isConnected()) {
    Serial.println("EXT_Led0 connect\t[FAIL]");
  } else {
    EXT_Led0.write16(0xFFFF);
    inited[0] = true;
    Serial.println("EXT_Led0 init\t[OK]");
  }
  if (!EXT_Led1.begin()) {
    Serial.println("EXT_Led1 init\t[FAIL]");
  } else if (!EXT_Led1.isConnected()) {
    Serial.println("EXT_Led1 connect\t[FAIL]");
  } else {
    EXT_Led1.write16(0xFFFF);
    inited[2] = true;
    Serial.println("EXT_Led1 init\t[OK]");
  }
  if (!EXT_Led2.begin()) {
    Serial.println("EXT_Led2 init\t[FAIL]");
  } else if (!EXT_Led2.isConnected()) {
    Serial.println("EXT_Led2 connect\t[FAIL]");
  } else {
    EXT_Led2.write16(0xFFFF);
    inited[3] = true;
    Serial.println("EXT_Led2 init\t[OK]");
  }
  if (!EXT_Led3.begin()) {
    Serial.println("EXT_Led3 init\t[FAIL]");
  } else if (!EXT_Led3.isConnected()) {
    Serial.println("EXT_Led3 connect\t[FAIL]");
  } else {
    EXT_Led3.write16(0xFFFF);
    inited[4] = true;
    Serial.println("EXT_Led3 init\t[OK]");
  }
  /*if (!EXT_Button.begin()){
    Serial.println("EXT_Button init\t[FAIL]");
  } else if (!EXT_Button.isConnected()){
    Serial.println("EXT_Button connect\t[FAIL]");
  } else {
    pinMode(EXT_BUTTON_INT_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(EXT_BUTTON_INT_PIN), EXTButtonInterrupt, FALLING);
    inited[1] = true;
  }*/
  timers[0] = millis();
}
byte t[3] = { 0,0,0 };
void extenderLoop() {
  if (millis() - timers[0] > 10) {
    if (inited[0])EXT_Led0.write16(random(0xFFFF));
    if (inited[2]) {
      t[1]++;
      if (t[0] % 10 == 0)t[2] = random(0xff);
      int r = t[1];
      r = r << 8;
      r += t[2];
      EXT_Led1.write16(r);
    }
    if (inited[3]) {
      t[1]++;
      if (t[0] % 10 == 0)t[2] = random(0xff);
      int r = t[1];
      r = r << 8;
      r += t[2];
      EXT_Led2.write16(r);
    }
    if (inited[4])EXT_Led3.write16(random(0xFFFF));
    timers[0] = millis();
    t[0]++;
  }
  if (inited[1] && EXTButtonFlag) {
    EXTButtonFlag = false;
    unsigned int cb = EXT_Button.read16();
    if (cb != currentButtonValue) {
      currentButtonValue = cb;
      bool found = false;
      for (byte i = 0;i < 12;i++) {
        if (currentButtonValue == buttonValues[i]) {
          currentButton = i;
          found = true;
        }
      }
      if (found) {
        Serial.printf("Button\t%d\r\n", currentButton);
      }
    }
  }
}