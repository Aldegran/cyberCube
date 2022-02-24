#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

byte mode = 0;

void setup() {
  Serial.begin(115200);   // Initialize serial communications with the PC
  SPI.begin();      // Init SPI bus
  Wire.begin(0x19);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  mfrc522.PCD_Init();   // Init MFRC522
  delay(4);       // Optional delay. Some board do need more time after init to be ready, see Readme
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
}

void loop() {

}

void receiveEvent() {
  mode = Wire.read();
  Serial.print(mode);
  Serial.print('\t');
}

void requestEvent(int cc){
  byte r;
  switch(mode){
    case 0x01: 
      r = mfrc522.PICC_IsNewCardPresent() ? 1 : 0;
      Wire.write(r);
      Serial.println(r);
    break;
    case 0x02: 
      r = mfrc522.PICC_ReadCardSerial() ? 1 : 0;
      Wire.write(r);
      Serial.println(r);
    break;
    case 0x03: 
      Wire.write(mfrc522.uid.size);
      Serial.println(mfrc522.uid.size);
    break;  
    case 0x04: 
      for(byte i=0;i<mfrc522.uid.size;i++)Wire.write(mfrc522.uid.uidByte[i]);
      mfrc522.PICC_DumpDetailsToSerial(&mfrc522.uid);
    break;  
    case 'A': 
      Wire.write('A');
      Serial.println("A");
    break;
  }
  mode = 0;
}
