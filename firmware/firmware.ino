#include <WiFi.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <SPI.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <SPIFFSEditor.h>

AsyncWebServer HTTPserver(80);

String ssid = "MYSTERIOUS-NEW";
String password = "0631603132";
String ap_ssid = "CyberCube";
String ap_password = "";

TaskHandle_t Task0;
TaskHandle_t Task1;
TaskHandle_t Task2;

//QueueHandle_t queue;

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

typedef void (*ButtonCallback) ();
unsigned long timers[2] = { 0, 0 };
byte gameMode = 0;
byte mode = 1;

void emptyFunction() {}
ButtonCallback BackButtonCallback = emptyFunction;

bool modeAP() {
  Serial.println("WiFi mode AP\n");
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
  Serial2.begin(115200);
  if (!FSInit()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
  } else {
    Serial.println("Load config file");
    loadConfig();
  }
  //queue = xQueueCreate(1, sizeof(SettingsStruct));
  xTaskCreatePinnedToCore(taskCore0, "Core0", 10000, NULL, 0, &Task0, 0);
  delay(500);
  xTaskCreatePinnedToCore(taskCore1, "Core1", 10000, NULL, 0, &Task1, 1);
  delay(500);
  xTaskCreatePinnedToCore(taskCore2, "Core2", 10000, NULL, 0, &Task2, 0);
}

void taskCore2(void* parameter) { //wifi, display, rfid
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());
  SPI.begin();
  RFIDSettings.lineColorChance[0] = 50;
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
  RFIDSettings.lineStep = 2;
  //RFIDSetup();
  displaySetup();
  for (;;) {
    //RFIDLoop();
    displayLoop();
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
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
  encoderSetup();
  //setupExtender();
  ledSetup();
  //soundSetup();
  for (;;) {
    encoderLoop();
    //extenderLoop();
    ledLoop();
    //soundLoop();
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