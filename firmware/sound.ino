#include "DFRobotDFPlayerMini.h"
DFRobotDFPlayerMini myDFPlayer;

byte soundInited = false;
void soundSetup() {
  Serial2.begin(9600);
  if (!myDFPlayer.begin(Serial2)) {
    Serial.println("DFPlayer init\t[FAIL]");
  } else {
    soundInited = true;
    Serial.println("DFPlayer init\t[OK]");
    myDFPlayer.setTimeOut(500);
    myDFPlayer.volume(5);
    soundPlay(1, false);
  }
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
    Serial.println("DFPlayer - USB Inserted!");
    break;
  case DFPlayerUSBRemoved:
    Serial.println("DFPlayer - USB Removed!");
    break;
  case DFPlayerPlayFinished:
    Serial.print(F("DFPlayer - Number:"));
    Serial.print(value);
    Serial.println(F("DFPlayer -  Play Finished!"));
    break;
  case DFPlayerError:
    Serial.print(F("DFPlayer - DFPlayerError:"));
    switch (value) {
    case Busy:
      Serial.println(F("DFPlayer - Card not found"));
      break;
    case Sleeping:
      Serial.println(F("DFPlayer - Sleeping"));
      break;
    case SerialWrongStack:
      Serial.println(F("DFPlayer - Get Wrong Stack"));
      break;
    case CheckSumNotMatch:
      Serial.println(F("DFPlayer - Check Sum Not Match"));
      break;
    case FileIndexOut:
      Serial.println(F("DFPlayer - File Index Out of Bound"));
      break;
    case FileMismatch:
      Serial.println(F("DFPlayer - Cannot Find File"));
      break;
    case Advertise:
      Serial.println(F("DFPlayer - In Advertise"));
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
  if (!soundInited) return;
  if (myDFPlayer.available())
    printSoundDetail(myDFPlayer.readType(), myDFPlayer.read());
}

void soundPlay(int sound, byte loop) {
  if (!soundInited) return;
  if (loop) myDFPlayer.loop(sound);
  else myDFPlayer.play(sound);
}

void soundStop() {
  if (!soundInited) return;
  myDFPlayer.pause();
}