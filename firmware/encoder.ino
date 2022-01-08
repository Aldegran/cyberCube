#include <ESP32Encoder.h>

ESP32Encoder encoder0;
ESP32Encoder encoder1;
ESP32Encoder encoder2;

#define PIN_ENCODER0_A 33
#define PIN_ENCODER0_B 32

#define PIN_ENCODER1_A 26
#define PIN_ENCODER1_B 25

#define PIN_ENCODER2_A 15
#define PIN_ENCODER2_B 4
#define PIN_ENCODER2_BUT 5

extern SettingsStruct encoderData;
int currentEncoder[3] = { 0,0,0 };
int encoderLimit[3][2] = { {-140, 140}, {-140, 140}, {0, 44} };

void encoderSetup() {
  ESP32Encoder::useInternalWeakPullResistors = UP;
  encoder0.attachHalfQuad(PIN_ENCODER0_A, PIN_ENCODER0_B);
  encoder1.attachHalfQuad(PIN_ENCODER1_A, PIN_ENCODER1_B);
  encoder2.attachHalfQuad(PIN_ENCODER2_A, PIN_ENCODER2_B);
  encoder0.clearCount();
  encoder1.clearCount();
  encoder2.clearCount();
  //pinMode(PIN_ENCODER2_BUT, INPUT_PULLUP);
  //encoder0.pauseCount();
  //encoder1.pauseCount();
}

void setCount(byte encoder, int value) {
  switch (encoder) {
  case 0: encoder0.setCount(value);
    break;
  case 1: encoder1.setCount(value);
    break;
  case 2: encoder2.setCount(value);
    break;

  default:
    break;
  }
}

void encoderLoop() {
  currentEncoder[0] = encoder0.getCount();
  currentEncoder[1] = encoder1.getCount();
  currentEncoder[2] = encoder2.getCount();
  for (byte i = 0; i < 3; i++) {
    if (currentEncoder[i] != encoderData.value[i]) {
      Serial.printf("%d = %d\r\n", i, currentEncoder[i]);
      if (currentEncoder[i] < encoderLimit[i][0]) {
        encoderData.value[i] = encoderLimit[i][i == 2 ? 1 : 0];
        setCount(i, encoderData.value[i]);
        encoderData.delta[i] = encoderData.value[i] - currentEncoder[i];
      } else if (currentEncoder[i] > encoderLimit[i][1]) {
        encoderData.value[i] = encoderLimit[i][i == 2 ? 0 : 1];
        setCount(i, encoderData.value[i]);
        encoderData.delta[i] = encoderData.value[i] - currentEncoder[i];
      } else {
        encoderData.delta[i] = encoderData.value[i] - currentEncoder[i];
        encoderData.value[i] = currentEncoder[i];
      }
    }// else encoderData.delta[i] = 0;
  }
  /*if (!digitalRead(PIN_ENCODER2_BUT)) {
    Serial.println("-");
  }
  delay(100);*/
}
