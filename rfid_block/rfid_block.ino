#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include "DFRobotDFPlayerMini.h"


#define RST_PIN        9 
#define SS_PIN         10
#define DF_RX          2
#define DF_TX          3
#define VOLTAGE        A0
#define WIRE_ADDRESS   0x19

//12.6  644
//8.1   417

DFRobotDFPlayerMini DFPlayer;
SoftwareSerial DFSerial(DF_RX, DF_TX); // RX, TX

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

byte mode = 0;
byte isLoop = false;
byte playerInit = false;

void setup() {
  Serial.begin(115200);   // Initialize serial communications with the PC
  Serial.println("RFID connector");
  pinMode(VOLTAGE, INPUT);
  pinMode(DF_RX, INPUT);
  pinMode(DF_TX, OUTPUT);
  analogReference(DEFAULT);
  DFSerial.begin(9600);
  playerInit = DFPlayer.begin(DFSerial);
  Serial.print("DFPlayer init\t[");
  Serial.print(playerInit ? "OK" : "FAIL");
  Serial.println("]");
  DFPlayer.volume(20);
  SPI.begin();      // Init SPI bus
  Wire.begin(WIRE_ADDRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  //int v = analogRead(VOLTAGE);
  //delay(1000);
  //Serial.print("V: ");
  //Serial.println(v);
  //initRFID();
  //soundPlay(2, false);
}

void printSoundDetail(uint8_t type, int value) {
  switch (type) {
  case TimeOut: Serial.println(F("DFPlayer - Time Out!"));
    break;
  case WrongStack: Serial.println(F("DFPlayer - Stack Wrong!"));
    break;
  case DFPlayerCardInserted: Serial.println(F("DFPlayer - Card Inserted!"));
    break;
  case DFPlayerCardRemoved: Serial.println(F("DFPlayer - Card Removed!"));
    break;
  case DFPlayerCardOnline: Serial.println(F("DFPlayer - Card Online!"));
    break;
  case DFPlayerUSBInserted: Serial.println(F("DFPlayer - USB Inserted!"));
    break;
  case DFPlayerUSBRemoved: Serial.println(F("DFPlayer - USB Removed!"));
    break;
  case DFPlayerPlayFinished:
    if (!isLoop) {
      Serial.print("DFPlayer - Number: ");
      Serial.print(value);
      Serial.println(" Play Finished!");
    }
    break;
  case DFPlayerError:
    Serial.print(F("DFPlayer - DFPlayerError: "));
    switch (value) {
    case Busy: Serial.println(F("Card not found"));
      break;
    case Sleeping: Serial.println(F("Sleeping"));
      break;
    case SerialWrongStack: Serial.println(F("Get Wrong Stack"));
      break;
    case CheckSumNotMatch: Serial.println(F("Check Sum Not Match"));
      break;
    case FileIndexOut: Serial.println(F("File Index Out of Bound"));
      break;
    case FileMismatch: Serial.println(F("Cannot Find File"));
      break;
    case Advertise: Serial.println(F("In Advertise"));
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

void initRFID() {
  digitalWrite(RST_PIN, LOW);		// Make sure we have a clean LOW state.
  delayMicroseconds(2);				// 8.8.1 Reset timing requirements says about 100ns. Let us be generous: 2μsl
  digitalWrite(RST_PIN, HIGH);		// Exit power down mode. This triggers a hard reset.
  mfrc522.PCD_Init();   // Init MFRC522
  delay(4);       // Optional delay. Some board do need more time after init to be ready, see Readme
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  /*if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) {
      Serial.println(mfrc522.uid.size);
      mfrc522.PICC_DumpDetailsToSerial(&mfrc522.uid);
    }
  }*/
}

void soundPlay(int sound, byte loop) {
  isLoop = loop;
  if (loop) DFPlayer.loop(sound);
  else DFPlayer.play(sound);
  Serial.print("Sound Play: ");
  Serial.print(sound);
  Serial.println(loop ? "[loop]" : "");
}

void soundStop() {
  isLoop = false;
  DFPlayer.pause();
}

void setVolume(byte v) {
  DFPlayer.volume(v);
}

void soundReset() {
  DFPlayer.reset();
}

void loop() {
  if (DFPlayer.available()) printSoundDetail(DFPlayer.readType(), DFPlayer.read());
}

void receiveEvent() {
  mode = Wire.read();
  Serial.print("> 0x");
  Serial.print(mode, HEX);
  Serial.print('\t');
  byte f;
  switch (mode) {
  case 0x11: // Set volume
    f = Wire.read();
    setVolume(f);
    Serial.print("Set volume: ");
    Serial.println(f);
    break;
  case 0x12: // Play
    f = Wire.read();
    soundPlay(f, false);
    Serial.print("Play: ");
    Serial.println(f);
    break;
  case 0x13: // Play loop
    f = Wire.read();
    soundPlay(f, true);
    Serial.print("Play loop: ");
    Serial.println(f);
    break;
  case 0x14: // Stop play
    soundStop();
    Serial.println("Stop play");
    break;
  case 0x15: // DFPlayer reset
    soundReset();
    Serial.println("DFPlayer reset");
    break;
  case 0xFF:
    Serial.println("System start\r\n");
    break;
  }
}

void requestEvent(int cc) {
  byte r;
  switch (mode) {
  case 0x01:
    r = mfrc522.PICC_IsNewCardPresent() ? 1 : 0;
    Wire.write(r);
    Serial.print("< ");
    Serial.println(r);
    break;
  case 0x02:
    r = mfrc522.PICC_ReadCardSerial() ? 1 : 0;
    Wire.write(r);
    Serial.print("< ");
    Serial.println(r);
    break;
  case 0x03:
    Wire.write(mfrc522.uid.size);
    Serial.print("< ");
    Serial.println(mfrc522.uid.size);
    break;
  case 0x04:
    for (byte i = 0; i < mfrc522.uid.size; i++)Wire.write(mfrc522.uid.uidByte[i]);
    mfrc522.PICC_DumpDetailsToSerial(&mfrc522.uid);
    break;
  case 0x05:
    Wire.write(0x05);
    Serial.println("< 0x05");
    initRFID();
    break;
  case 0x10: //DFPlayer Inited
    Wire.write(playerInit);
    Serial.print("< ");
    Serial.println(playerInit);
    break;
  case 0x20: //Voltage value
    int v = analogRead(VOLTAGE);
    Wire.write((v >> 8));
    Wire.write((v % 256));
    Serial.print("< ");
    Serial.println(v);
    break;
  default:
    Wire.write(0);
    break;
  }
  mode = 0;
}
