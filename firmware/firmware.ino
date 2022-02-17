#include <WiFi.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <SPI.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <SPIFFSEditor.h>

#define IDLE_TIME 300000

#define LCD_DC 14
#define LCD_BL 13
#define LCD_RST 12

#define GAME_MODE_INIT 0
#define GAME_MODE_START 1
#define GAME_MODE_CONNECT_TOP 2
#define GAME_MODE_CONNECT_BOX 3
#define GAME_MODE_CONNECT_LCD 4
#define GAME_MODE_WAIT_BUTTON 5
#define GAME_MODE_WAIT_ANIMATION 14
#define GAME_MODE_WAIT_CAPSULE 6
#define GAME_MODE_CAPSULE_BEGIN 7
#define GAME_MODE_CAPSULE_END 8
#define GAME_MODE_CAPSULE_READ 9
#define GAME_MODE_CAPSULE_FAIL_READ 10
#define GAME_MODE_CAPSULE_GAME 11
#define GAME_MODE_CAPSULE_GAME_FAIL 12
#define GAME_MODE_CAPSULE_GAME_OK 13
#define GAME_MODE_IDLE 15

AsyncWebServer HTTPserver(80);

String ssid = "MYSTERIOUS-NEW";
String password = "0631603132";
String ap_ssid = "CyberCube";
String ap_password = "";

TaskHandle_t Task0;
TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;

typedef struct {
  int value[3];
  int delta[3];
} __attribute__((packed, aligned(1))) SettingsStruct;
SettingsStruct encoderData;


typedef struct {
  int lineColorChanceUp;
  int lineChaosUp;
  int lineChanceUp;
  int lineCycleDown;
  int userRadiusDown;
  int lineStepUp;
  int levels;
} __attribute__((packed, aligned(1))) GameSettingsStruct;
GameSettingsStruct gameSettings;

typedef struct {
  byte lineColorChance[2];
  byte lineChance[2];
  byte lineChaos[2];
  byte positionBonus[2];
  byte colorBonus[2];
  int maxScore;
  byte userRadius;
  byte lineCycle;
  byte lineStep;
} __attribute__((packed, aligned(1))) RFIDSettingsStruct;
RFIDSettingsStruct RFIDSettings;

typedef struct {
  byte cylinderTop;
  byte cylinderBottom;
  byte cylinderStatus;
  byte cylinderConnection;
  byte LCDConnection;
  byte extendersConnection;
} __attribute__((packed, aligned(1))) ConnectorsStatusStruct;
ConnectorsStatusStruct ConnectorsStatus;

typedef void (*ButtonCallback) ();
unsigned long timers[3] = { 0, 0, 0 };
byte gameMode = 0;
byte mode = GAME_MODE_INIT;

void emptyFunction() {}
ButtonCallback BackButtonCallback = emptyFunction;

bool modeAP() {
  Serial.println(F("WiFi mode AP\n"));
  if (ap_ssid.length()) {
    IPAddress apIP(192, 168, 0, 1);
    IPAddress GateWayIP(0, 0, 0, 0);
    IPAddress netMsk(255, 255, 255, 0);

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, GateWayIP, netMsk);
    WiFi.softAP(ap_ssid.c_str(), ap_password.c_str());
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("======================================================="));
  //Serial2.begin(115200);
  if (!FSInit()) {
    Serial.println(F("An Error has occurred while mounting SPIFFS"));
  } else {
    Serial.println(F("Load config file"));
    loadConfig();
  }
  ConnectorsStatus.cylinderTop = false;
  ConnectorsStatus.cylinderBottom = false;
  ConnectorsStatus.cylinderStatus = false;
  ConnectorsStatus.cylinderConnection = false;
  ConnectorsStatus.LCDConnection = false;
  ConnectorsStatus.extendersConnection = false;
  xTaskCreatePinnedToCore(taskCore0, "Core0", 10000, NULL, 0, &Task0, 0);
  delay(500);
  xTaskCreatePinnedToCore(taskCore1, "Core1", 10000, NULL, 0, &Task1, 1);
  delay(500);
  xTaskCreatePinnedToCore(taskCore2, "Core2", 10000, NULL, 0, &Task2, 0);
  delay(500);
  xTaskCreatePinnedToCore(taskCore3, "Core3", 10000, NULL, 0, &Task3, 1);
}

void taskCore2(void* parameter) { //display, rfid
  Serial.print(F("Task2 running on core "));
  Serial.println(xPortGetCoreID());
  SPI.begin();
  /*RFIDSettings.lineColorChance[0] = 50;
  RFIDSettings.lineColorChance[1] = 50;
  RFIDSettings.lineChance[0] = 10;
  RFIDSettings.lineChance[1] = 10;
  RFIDSettings.lineChaos[0] = 20;
  RFIDSettings.lineChaos[1] = 20;
  RFIDSettings.positionBonus[0] = 2;
  RFIDSettings.positionBonus[1] = 9;
  RFIDSettings.colorBonus[0] = 2;
  RFIDSettings.colorBonus[1] = 9;
  RFIDSettings.maxScore = 1000;
  RFIDSettings.userRadius = 15;
  RFIDSettings.lineCycle = 6;
  RFIDSettings.lineStep = 2;*/
  RFIDSetup();
  mode = GAME_MODE_START;
  statusChanged();
  //displaySetup();
  //setLedCS();
  for (;;) {
    if(mode == GAME_MODE_CONNECT_BOX && ConnectorsStatus.LCDConnection){
      if(ConnectorsStatus.LCDConnection == 2 ){
        //displaySetup();
        ConnectorsStatus.LCDConnection = true;
        mode = GAME_MODE_CONNECT_LCD;
        statusChanged();
      }
    }
    if( ConnectorsStatus.LCDConnection == true && mode != GAME_MODE_IDLE){
      displayLoop();
    }
    if(mode == GAME_MODE_CAPSULE_END) {
      //RFIDLoop();
      delay(3000);
      mode = GAME_MODE_CAPSULE_READ;
      statusChanged();
    }
    /*for (byte i = 0; i < 3; i++) {
      if (encoderData.delta[i]) {
        Serial.printf("Encoder %d = %d\r\n", i, encoderData.value[i]);
        encoderData.delta[i] = 0;
      }
    }*/
  }
}

void taskCore0(void* parameter) { //wifi
  Serial.print("Task0 running on core ");
  Serial.println(xPortGetCoreID());
  serialConsileInit();
  SPIFFS.begin();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  byte c = 0;
  while (WiFi.waitForConnectResult() != WL_CONNECTED && c < 5) {
    Serial.println("Connection Failed!");
    delay(5000);
    c++;
    //ESP.restart();
  }
  if (WiFi.waitForConnectResult() != WL_CONNECTED) modeAP();
  ArduinoOTA
    .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
      })
    .onEnd([]() { Serial.println("\nEnd"); })
        .onProgress([](unsigned int progress, unsigned int total) { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
        .onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
          Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
          Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
          Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
          Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
          Serial.println("End Failed");
          });

      ArduinoOTA.begin();

      Serial.println("Ready");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      HTTPserver.addHandler(new SPIFFSEditor(SPIFFS, "admin", "admin"));
      HTTPserver.begin();
      for (;;) {
        ArduinoOTA.handle();
        consoleLoop();
      }
}

void taskCore1(void* parameter) { //encoder, led, WS
  Serial.print(F("Task1 running on core "));
  Serial.println(xPortGetCoreID());
  encoderSetup();
  setupExtender();
  ledSetup();
  for (;;) {
    if(mode != GAME_MODE_IDLE) extenderLoop();
    if(mode == GAME_MODE_START && ConnectorsStatus.cylinderConnection){ // надели верх
      mode = GAME_MODE_CONNECT_TOP;
      statusChanged();
      MIDIPlayRandom();
    }
    if(mode == GAME_MODE_CONNECT_TOP){ // подключили верх
      if(!setupBoxExtenders()) {
        delay(1000);
        if(!ConnectorsStatus.cylinderConnection){ // сняли верх
          mode = GAME_MODE_START;
          statusChanged();
        }
      } else {
        if(mode!=GAME_MODE_CONNECT_LCD){
          mode = GAME_MODE_CONNECT_BOX;
          statusChanged();
        }
        cylinderLight(true);
      }
    }
    if(mode == GAME_MODE_WAIT_BUTTON && checkButton()){ // нажали кнопку
      ledFlash();
      while(checkButton()) delay(10);
      mode = GAME_MODE_WAIT_CAPSULE;
      timers[2] = millis();
      statusChanged();
    }

    if(mode == GAME_MODE_WAIT_CAPSULE && ConnectorsStatus.cylinderTop){ // встувили верх капсулы
      mode = GAME_MODE_CAPSULE_BEGIN;
      timers[2] = millis();
      statusChanged();
    }
    if(mode == GAME_MODE_CAPSULE_BEGIN && millis() - timers[2] > IDLE_TIME*2){ //долго впихиваем капсулу или передумали
      mode = GAME_MODE_WAIT_CAPSULE;
      timers[2] = millis();
      statusChanged();
    }
    if(mode == GAME_MODE_CAPSULE_BEGIN && ConnectorsStatus.cylinderBottom){ // встравили низ капсулы
      mode = ConnectorsStatus.cylinderStatus ? GAME_MODE_CAPSULE_END :  GAME_MODE_CAPSULE_FAIL_READ;
      statusChanged();
    }
    if(mode == GAME_MODE_CAPSULE_FAIL_READ && ConnectorsStatus.cylinderTop){ // вытащили капсулу после неудачного чтения
      mode = GAME_MODE_WAIT_BUTTON;
      timers[2] = millis();
      statusChanged();
    }
    if(mode == GAME_MODE_CAPSULE_GAME){ // игра
      encoderLoop();
      ledLoop();
    }
    if(mode == GAME_MODE_CAPSULE_GAME_OK || mode == GAME_MODE_CAPSULE_GAME_FAIL){ // после игры
      if(ConnectorsStatus.cylinderTop){ //ждём пока вытащит
        mode = GAME_MODE_WAIT_BUTTON;
        timers[2] = millis();
        statusChanged();
      }
    }
    if(mode == GAME_MODE_WAIT_CAPSULE || mode == GAME_MODE_WAIT_BUTTON){ 
      if(millis() - timers[2] > IDLE_TIME){ // долго ждали капсулу или кнопку
        mode = GAME_MODE_IDLE;
        statusChanged();
        extIDLE();
        ledIDLE();
      }
    }
    if(mode == GAME_MODE_IDLE && checkButton()){ // проснулись
      while(checkButton()) delay(10);
      mode = GAME_MODE_WAIT_CAPSULE;
      MIDIPlayRandom();
      cylinderLight(true);
      extDemo();
      timers[2] = millis();
      statusChanged();
    }
  }
}

void taskCore3(void* parameter) { //sound
  Serial.print(F("Task3 running on core "));
  Serial.println(xPortGetCoreID());
  soundSetup();
  for (;;) {
    soundLoop();
  }
}

void loop() {
  vTaskDelete(NULL);
}

int userColor(byte n) {
  if (n == 44)return 0;
  byte c = n % 7;
  if (c == 6)return n / 7 + 1;
  else if (c < 2) return n / 7;
  return 6;
}

void statusChanged() {
  Serial.print(F("STATUS: ["));
  Serial.print(mode);
  switch(mode){
    case GAME_MODE_INIT : Serial.println(F("] GAME_MODE_INIT")); break;
    case GAME_MODE_START : Serial.println(F("] GAME_MODE_START")); break;
    case GAME_MODE_CONNECT_TOP : Serial.println(F("] GAME_MODE_CONNECT_TOP")); break;
    case GAME_MODE_CONNECT_BOX : Serial.println(F("] GAME_MODE_CONNECT_BOX")); break;
    case GAME_MODE_CONNECT_LCD : Serial.println(F("] GAME_MODE_CONNECT_LCD")); break;
    case GAME_MODE_WAIT_BUTTON : Serial.println(F("] GAME_MODE_WAIT_BUTTON")); break;
    case GAME_MODE_WAIT_ANIMATION : Serial.println(F("] GAME_MODE_WAIT_ANIMATION")); break;
    case GAME_MODE_WAIT_CAPSULE : Serial.println(F("] GAME_MODE_WAIT_CAPSULE")); break;
    case GAME_MODE_CAPSULE_BEGIN : Serial.println(F("] GAME_MODE_CAPSULE_BEGIN")); break;
    case GAME_MODE_CAPSULE_END : Serial.println(F("] GAME_MODE_CAPSULE_END")); break;
    case GAME_MODE_CAPSULE_READ : Serial.println(F("] GAME_MODE_CAPSULE_READ")); break;
    case GAME_MODE_CAPSULE_FAIL_READ : Serial.println(F("] GAME_MODE_CAPSULE_FAIL_READ")); break;
    case GAME_MODE_CAPSULE_GAME : Serial.println(F("] GAME_MODE_CAPSULE_GAME")); break;
    case GAME_MODE_CAPSULE_GAME_FAIL : Serial.println(F("] GAME_MODE_CAPSULE_GAME_FAIL")); break;
    case GAME_MODE_CAPSULE_GAME_OK : Serial.println(F("] GAME_MODE_CAPSULE_GAME_OK")); break;
    case GAME_MODE_IDLE : Serial.println(F("] GAME_MODE_IDLE")); break;
  }
}
