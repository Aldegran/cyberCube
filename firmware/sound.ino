#include "DFRobotDFPlayerMini.h"
DFRobotDFPlayerMini myDFPlayer;

#define BUZZER_PIN 2

int notes[75][3] = {{1318,150,150},{1318,300,300},{1318,150,300},{1046,150,150},{1318,300,300},{1568,600,600},{784,600,600},{1046,450,450},{784,150,450},{659,300,450},{880,300,300},{987,150,300},{932,150,150},{880,300,300},{784,210,210},{1318,210,210},{1568,150,150},{1750,300,300},{1396,150,150},{1568,150,300},{1318,300,300},{1046,150,150},{1174,150,150},{987,450,450},{1046,450,450},{784,150,450},{659,300,450},{880,300,300},{987,150,300},{932,150,150},{880,300,300},{784,210,210},{1318,210,210},{1568,150,150},{1750,300,300},{1396,150,150},{1568,150,300},{1318,300,300},{1046,150,150},{1174,150,150},{987,450,600},{1568,150,150},{1480,150,150},{1396,150,150},{1244,300,300},{1318,150,300},{830,150,150},{880,150,150},{1046,150,300},{880,150,150},{1046,150,150},{1174,150,150},{0,0,300},{1568,150,150},{1480,150,150},{1396,150,150},{1244,300,300},{1318,150,300},{2093,300,300},{2093,150,150},{2093,600,600},{1568,150,150},{1480,150,150},{1396,150,150},{1244,300,300},{1318,150,300},{830,150,150},{880,150,150},{1046,150,300},{880,150,150},{1046,150,150},{1174,150,450},{1244,300,450},{1174,450,450},{1046,600,1200}};

byte soundInited = false;
void soundSetup() {
  ledcAttachPin(BUZZER_PIN, 0);
  MIDIStart();
  Serial2.begin(9600);
  myDFPlayer.begin(Serial2);
  soundInited = true;
  Serial.println(F("DFPlayer init\t[OK]"));
  myDFPlayer.setTimeOut(500);
  myDFPlayer.volume(5);
  soundPlay(1, false);
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
  someNoise();
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

void nota(int fr, int d, int d2) {
  ledcSetup(0, fr, 8);
  ledcWrite(0, 127);
  delay(d);
  if(d2)delay(d2);
}

void MIDIStart(){
  nota(1700, 100, 0);
  nota(3400, 200, 0);
  nota(1700, 300, 0);
  nota(0,0,0);
}

void MIDIPlayRandom(){
byte samp = random(7)+3;
byte offs = random(74-samp);
byte mu = random(7)+3;
for(byte i = 0 ; i<samp; i++){
  int no = offs + i;
  no = random(2) == 1 ? 74-no : no;
  nota(notes[no][0]*2,notes[no][1]/mu,notes[no][2]/mu);
}
nota(0,0,0);
}

void someNoise(){
  if(mode == GAME_MODE_CAPSULE_END 
  || mode == GAME_MODE_CAPSULE_READ  
  || mode == GAME_MODE_WAIT_ANIMATION  
  || mode == GAME_MODE_CAPSULE_FAIL_READ  
  || mode == GAME_MODE_CAPSULE_GAME  
  || mode == GAME_MODE_CAPSULE_GAME_FAIL  
  || mode == GAME_MODE_CAPSULE_GAME_OK ) return;
  if(mode > GAME_MODE_START && random(1000) == 10) {
    MIDIPlayRandom();
  }
  delay(10);
}