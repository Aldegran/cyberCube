#include "DFRobotDFPlayerMini.h"
DFRobotDFPlayerMini myDFPlayer;

#define BUZZER_PIN 2

int notes[75][3] = { {1318,150,150},{1318,300,300},{1318,150,300},{1046,150,150},{1318,300,300},{1568,600,600},{784,600,600},{1046,450,450},{784,150,450},{659,300,450},{880,300,300},{987,150,300},{932,150,150},{880,300,300},{784,210,210},{1318,210,210},{1568,150,150},{1750,300,300},{1396,150,150},{1568,150,300},{1318,300,300},{1046,150,150},{1174,150,150},{987,450,450},{1046,450,450},{784,150,450},{659,300,450},{880,300,300},{987,150,300},{932,150,150},{880,300,300},{784,210,210},{1318,210,210},{1568,150,150},{1750,300,300},{1396,150,150},{1568,150,300},{1318,300,300},{1046,150,150},{1174,150,150},{987,450,600},{1568,150,150},{1480,150,150},{1396,150,150},{1244,300,300},{1318,150,300},{830,150,150},{880,150,150},{1046,150,300},{880,150,150},{1046,150,150},{1174,150,150},{0,0,300},{1568,150,150},{1480,150,150},{1396,150,150},{1244,300,300},{1318,150,300},{2093,300,300},{2093,150,150},{2093,600,600},{1568,150,150},{1480,150,150},{1396,150,150},{1244,300,300},{1318,150,300},{830,150,150},{880,150,150},{1046,150,300},{880,150,150},{1046,150,150},{1174,150,450},{1244,300,450},{1174,450,450},{1046,600,1200} };


byte isLoop = false;

void soundSetup() {
  ledcAttachPin(BUZZER_PIN, 0);
  MIDIStart();
  Serial2.begin(9600);
  Serial.printf("DFPlayer init\t[%s]\r\n", myDFPlayer.begin(Serial2) ? "OK" : "FAIL");
  //myDFPlayer.reset();
  delay(1000);
  myDFPlayer.setTimeOut(1500);
  myDFPlayer.volume(gameSettings.volume);
  delay(10);
  ConnectorsStatus.DFPlayerConnection = true;
  //soundPlay(SOUND_WAKEUP, false);
}

void printSoundDetail(uint8_t type, int value) {
  switch (type) {
  case TimeOut:
    Serial.println(F("DFPlayer - Time Out!"));
    break;
  case WrongStack:
    Serial.println(F("DFPlayer - Stack Wrong!"));
    break;
  case DFPlayerCardInserted:
    Serial.println(F("DFPlayer - Card Inserted!"));
    break;
  case DFPlayerCardRemoved:
    Serial.println(F("DFPlayer - Card Removed!"));
    break;
  case DFPlayerCardOnline:
    Serial.println(F("DFPlayer - Card Online!"));
    break;
  case DFPlayerUSBInserted:
    Serial.println(F("DFPlayer - USB Inserted!"));
    break;
  case DFPlayerUSBRemoved:
    Serial.println(F("DFPlayer - USB Removed!"));
    break;
  case DFPlayerPlayFinished:
    if (!isLoop) Serial.printf("DFPlayer - Number: %d Play Finished!\r\n", value);
    break;
  case DFPlayerError:
    Serial.print(F("DFPlayer - DFPlayerError: "));
    switch (value) {
    case Busy:
      Serial.println(F("Card not found"));
      break;
    case Sleeping:
      Serial.println(F("Sleeping"));
      break;
    case SerialWrongStack:
      Serial.println(F("Get Wrong Stack"));
      break;
    case CheckSumNotMatch:
      Serial.println(F("Check Sum Not Match"));
      break;
    case FileIndexOut:
      Serial.println(F("File Index Out of Bound"));
      break;
    case FileMismatch:
      Serial.println(F("Cannot Find File"));
      break;
    case Advertise:
      Serial.println(F("In Advertise"));
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

void soundLoop() {
  //someNoise();
  if (!ConnectorsStatus.DFPlayerConnection) return;
  if (myDFPlayer.available())
    printSoundDetail(myDFPlayer.readType(), myDFPlayer.read());
}

void soundPlay(int sound, byte loop) {
  if (!ConnectorsStatus.DFPlayerConnection) return;
  isLoop = loop;
  myDFPlayer.pause();
  if (loop) myDFPlayer.loop(sound);
  else myDFPlayer.play(sound);
  Serial.printf("Sound Play: %d %s\r\n", sound, loop ? "[loop]" : "");
}

void soundStop() {
  if (!ConnectorsStatus.DFPlayerConnection) return;
  isLoop = false;
  myDFPlayer.pause();
}

void setVolume(byte v) {
  if (!ConnectorsStatus.DFPlayerConnection) return;
  myDFPlayer.volume(v);
}

void nota(int fr, int d, int d2) {
#if USE_MIDI
  if (fr) {
    ledcSetup(0, fr, 8);
    ledcWrite(0, 127);
  } else {
    ledcSetup(0, 8, 8);
    ledcWrite(0, 0);
    return;
  }
  delay(d);
  if (d2)delay(d2);
#else
  return;
#endif
}

void MIDIStart() {
  nota(1700, 100, 0);
  nota(3400, 200, 0);
  nota(1700, 300, 0);
  nota(0, 0, 0);
}

void MIDIPlayRandom() {
  byte samp = random(7) + 3;
  byte offs = random(74 - samp);
  byte mu = random(7) + 3;
  for (byte i = 0; i < samp; i++) {
    int no = offs + i;
    no = random(2) == 1 ? 74 - no : no;
    nota(notes[no][0] * 2, notes[no][1] / mu, notes[no][2] / mu);
  }
  nota(0, 0, 0);
}

void someNoise() {
  if (mode == GAME_MODE_CAPSULE_END
    || mode == GAME_MODE_CAPSULE_READ
    || mode == GAME_MODE_IDLE
    || mode == GAME_MODE_OTA
    || mode == GAME_MODE_WAIT_ANIMATION
    || mode == GAME_MODE_CAPSULE_FAIL_READ
    || mode == GAME_MODE_CAPSULE_GAME
    || mode == GAME_MODE_CAPSULE_GAME_FAIL
    || mode == GAME_MODE_CAPSULE_GAME_OK) return;
  if (mode > GAME_MODE_START && random(1000) == 10) {
    MIDIPlayRandom();
  }
  delay(10);
}