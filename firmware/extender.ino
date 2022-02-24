#include "PCF8575.h"
#include "Wire.h"

#define RFID_WIRE 0x19

PCF8575 EXT_Led0(0x20);
PCF8575 EXT_Serv(0x21);
PCF8575 EXT_Led1(0x22);
PCF8575 EXT_Led2(0x23);
PCF8575 EXT_Led3(0x24);
extern unsigned long timers[3];
extern byte mode;
extern ConnectorsStatusStruct ConnectorsStatus;
unsigned int currentButtonValue = 0;
byte currentButton = 0;
//const int EXT_BUTTON_INT_PIN = 15;
unsigned int buttonValues[12] = { 65534, 65533, 65531, 65527, 65519, 65503, 65471, 65407, 65279, 65023, 64511, 63487 };
byte inited[6] = { false,false,false,false,false, false };

volatile bool EXTButtonFlag = false;

void EXTButtonInterrupt() {
  EXTButtonFlag = true;
}

void setupExtender() {
  Wire.begin();
  Wire.beginTransmission(RFID_WIRE);
  if (Wire.endTransmission()!=0) {
    Serial.println(F("EXT_RFID init\t[FAIL]"));
  } else {
    Wire.beginTransmission(RFID_WIRE);
    Wire.write('A');
    Wire.endTransmission();
    Wire.requestFrom(RFID_WIRE, 1);
    byte c = Wire.read();
    Serial.println(c);
    if (c != 'A') {
      Serial.println(F("EXT_RFID connect\t[FAIL]"));
    } else {
      inited[1] = true;
      Serial.println(F("EXT_RFID init\t[OK]"));
    }
  }
  if (!EXT_Serv.begin()) {
    Serial.println(F("EXT_Serv init\t[FAIL]"));
  } else if (!EXT_Serv.isConnected()) {
    Serial.println(F("EXT_Serv connect\t[FAIL]"));
  } else {
    EXT_Serv.write16(0xFFFF);
    inited[5] = true;
    Serial.println(F("EXT_Serv init\t[OK]"));
  }
  timers[0] = millis();
}

bool setupBoxExtenders(){
    if(inited[0] && inited[2] && inited[3] && inited[4]){
      Serial.println(F("EXT reinit"));
      return true;
    }
    if (!EXT_Led0.begin()) {
      Serial.println(F("EXT_Led0 init\t[FAIL]"));
      inited[0] = false;
    } else if (!EXT_Led0.isConnected()) {
      Serial.println(F("EXT_Led0 connect\t[FAIL]"));
      inited[0] = false;
    } else {
      EXT_Led0.write16(0xFFFF);
      inited[0] = true;
      Serial.println(F("EXT_Led0 init\t[OK]"));
    }
    if (!EXT_Led1.begin()) {
      Serial.println(F("EXT_Led1 init\t[FAIL]"));
      inited[2] = false;
    } else if (!EXT_Led1.isConnected()) {
      Serial.println(F("EXT_Led1 connect\t[FAIL]"));
      inited[2] = false;
    } else {
      EXT_Led1.write16(0xFFFF);
      inited[2] = true;
      Serial.println(F("EXT_Led1 init\t[OK]"));
    }
    if (!EXT_Led2.begin()) {
      Serial.println(F("EXT_Led2 init\t[FAIL]"));
      inited[3] = false;
    } else if (!EXT_Led2.isConnected()) {
      Serial.println(F("EXT_Led2 connect\t[FAIL]"));
      inited[3] = false;
    } else {
      EXT_Led2.write16(0xFFFF);
      inited[3] = true;
      Serial.println(F("EXT_Led2 init\t[OK]"));
    }
    if (!EXT_Led3.begin()) {
      Serial.println(F("EXT_Led3 init\t[FAIL]"));
      inited[4] = false;
    } else if (!EXT_Led3.isConnected()) {
      Serial.println(F("EXT_Led3 connect\t[FAIL]"));
      inited[4] = false;
    } else {
      EXT_Led3.write16(0xFFFF);
      inited[4] = true;
      Serial.println(F("EXT_Led3 init\t[OK]"));
    }

    if(inited[0] && inited[2] && inited[3] && inited[4]){
      //extDemo();
      return true;
    }
    return false;
}

void extDemo(){
  for(byte i = 0; i<16;i++) {
    EXT_Led1.write(i, false);
    if(i!=9)EXT_Led2.write(i, false);//кроме цилиндра
    delay(1000-i*50);
  }
  for(byte i = 0; i<16;i++) {
    EXT_Led0.write(i, false);
    EXT_Led3.write(i, false);
    delay(500-i*20);
  }
  delay(3000);
  EXT_Led0.write16(0xFFFF);
  EXT_Led1.write16(0xFFFF);
  EXT_Led2.write16(0xFFFF);
  EXT_Led3.write16(0xFFFF);
  delay(2000);
}

byte t[5] = { 0,0,0,0,0 };
void extenderLoop() {
  MBPoints();
  /*if (inited[1] && EXTButtonFlag) {
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
  }*/
  if (inited[5])readConnectors();
}

void readConnectors() {
  if(mode == GAME_MODE_CAPSULE_END 
  || mode == GAME_MODE_INIT  
  || mode == GAME_MODE_CAPSULE_READ  
  || mode == GAME_MODE_WAIT_ANIMATION  
  || ConnectorsStatus.stopEXT  
  || mode == GAME_MODE_CAPSULE_GAME) return;
  //EXT_Serv.write16(0xFFFF);
  delay(10);
  int v = EXT_Serv.read16();
  if(!(v & (1<<2))) {
    if(!ConnectorsStatus.cylinderConnection){
      ConnectorsStatus.cylinderConnection = true;
      Serial.println(F("Connect: cylinderConnection"));
    }
  } else if(ConnectorsStatus.cylinderConnection){
    ConnectorsStatus.cylinderConnection = false;
    Serial.println(F("Connect: DISCONECT cylinderConnection"));
  }
  if(!(v & (1<<3))) {
    if(!ConnectorsStatus.cylinderTop){
    ConnectorsStatus.cylinderTop = true;
    Serial.println(F("Connect: cylinderTop"));
    }
  } else if(ConnectorsStatus.cylinderTop){
    ConnectorsStatus.cylinderTop = false;
    Serial.println(F("Connect: DISCONECT cylinderTop"));
  }
  if(!(v & (1<<4))) {
    if(!ConnectorsStatus.cylinderStatus){
    ConnectorsStatus.cylinderStatus = true;
    Serial.println(F("Connect: cylinderStatus"));
    }
  } else if(ConnectorsStatus.cylinderStatus){
    ConnectorsStatus.cylinderStatus = false;
    Serial.println(F("Connect: DISCONECT cylinderStatus"));
  }
  if(!(v & (1<<6))) {
    if(!ConnectorsStatus.LCDConnection){
    ConnectorsStatus.LCDConnection = 2;
    Serial.println(F("Connect: LCDConnection"));
    }
  } else if(ConnectorsStatus.LCDConnection){
    ConnectorsStatus.LCDConnection = false;
    Serial.println(F("Connect: DISCONECT LCDConnection"));
  }
  if(!(v>>8 & (1<<0))) {
    if(!ConnectorsStatus.cylinderBottom){
    ConnectorsStatus.cylinderBottom = true;
    Serial.println(F("Connect: cylinderBottom"));
    }
  } else if(ConnectorsStatus.cylinderBottom){
    ConnectorsStatus.cylinderBottom = false;
    Serial.println(F("Connect: DISCONECT cylinderBottom"));
  }
  /*if(v != 0xFFFF){
    Serial.println(v, BIN);
  }*/
  if(mode == GAME_MODE_START && random(50) == 2){
    EXT_Serv.write(15, false);
    delay(50);
    EXT_Serv.write(15, true);
  }
}

void setLedRes() {
  digitalWrite(LCD_RST, LOW);
}
void resetLedRes() {
  digitalWrite(LCD_RST, HIGH);
}
void setLedDC() {
  digitalWrite(LCD_DC, LOW);
}
void resetLedDC() {
  digitalWrite(LCD_DC, HIGH);
}
void setLedCS() {
  if (inited[5]){
    EXT_Serv.write(1, true);
    EXT_Serv.write(0, false);
  }
}
void resetLedCS() {
  if (inited[5])EXT_Serv.write(0, true);
}

void setRFIDCS() {
  if (inited[5]){
    EXT_Serv.write(0, true);
    EXT_Serv.write(1, false);
  }
}

void resetRFIDCS() {
  if (inited[5])EXT_Serv.write(1, true);
}

void cylinderLight(bool led){
    EXT_Serv.write(14, !led);
}

void MBPoints(){
  if (millis() - timers[0] > 10) {
    MBStrips();
    if (inited[2]) {
      t[1]++;
      if (t[0] % 10 == 0)t[2] = random(0xff);
      int r = t[1];
      r = r << 8;
      r += t[2];
      EXT_Led1.write16(r);
    }
    if (inited[3]) {
      t[3]++;
      if (t[0] % 10 == 0)t[4] = random(0xff);
      int r = t[3];
      if(mode != GAME_MODE_WAIT_BUTTON && mode != GAME_MODE_IDLE) r &= ~(1<<1); /// свет в цилиндре сверху
      else r |= (1<<1);
      r = r << 8;
      r += t[4];
      
      EXT_Led2.write16(r);
    }
    timers[0] = millis();
    t[0]++;
  }
}
void MBStrips(){
    if(mode < GAME_MODE_WAIT_CAPSULE || mode == GAME_MODE_IDLE) {
      if (inited[0])EXT_Led0.write16(0xFFFF);
      if (inited[4])EXT_Led3.write16(0xFFFF);
      return;
    }
    if (inited[0])EXT_Led0.write16(random(0xFFFF));
    if (inited[4])EXT_Led3.write16(cylinderStrips() + random(0xFF));
}

byte cylinderStripsCycle = 0;
byte cylinderStripsValue = 0;
int cylinderStrips(){
  if(mode == GAME_MODE_CAPSULE_GAME_FAIL || mode == GAME_MODE_CAPSULE_GAME_OK){
    if(cylinderStripsCycle<5)cylinderStripsCycle = 80;
    cylinderStripsCycle-=5;
  } else {
    cylinderStripsCycle+=5;
    if(cylinderStripsCycle>80)cylinderStripsCycle = 0;
  }
  switch(cylinderStripsCycle){
    case 10: cylinderStripsValue = B11111110; break;
    case 20: cylinderStripsValue = B11111101; break;
    case 30: cylinderStripsValue = B11111011; break;
    case 40: cylinderStripsValue = B11110111; break;
    case 50: cylinderStripsValue = B11101111; break;
    case 60: cylinderStripsValue = B11011111; break;
    case 70: cylinderStripsValue = B10111111; break;
    case 80: cylinderStripsValue = B01111111; break;
  }
  return cylinderStripsValue<<8;
}

void extIDLE(){
  EXT_Led0.write16(0xFFFF);
  EXT_Led1.write16(0xFFFF);
  EXT_Led2.write16(0xFFFF);
  EXT_Led3.write16(0xFFFF);
  cylinderLight(false);
}

bool readRFID(){
  if(!inited[1]) return false;
  ConnectorsStatus.RFIDok = false;
  if(getByteRFID(0x01) == 1 && getByteRFID(0x02) == 1 ) {
    byte size = getByteRFID(0x03);
    getRFID(0x04, size);
    Serial.printf("size: %d\r\nuid: ",size);
    if(!size || size == 0xFF) {
      Serial.println();
      return false;
    }
    ConnectorsStatus.RFIDok = true;
    byte rfidBufIndex = 0;
    byte rfidBuf[4];
    while (Wire.available()) {
      char c = Wire.read();
      if(rfidBufIndex<4) rfidBuf[rfidBufIndex] = c;
      rfidBufIndex++;
    }
    sprintf(ConnectorsStatus.RFID, "%X",rfidBuf);
    Serial.println(ConnectorsStatus.RFID);
    getByteRFID(0x01);
    return true;
  }
  return false;
}

byte getByteRFID(byte d){
  getRFID(d,1);
  return Wire.read();
}

byte getRFID(byte d, byte size){
  Wire.beginTransmission(RFID_WIRE);
  Wire.write(d);
  Wire.endTransmission();
  Wire.requestFrom(RFID_WIRE, size);
}