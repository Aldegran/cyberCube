#include <WiFi.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <SPI.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <SPIFFSEditor.h>

#define LCD_RST 12
#define LCD_BL 13
#define LCD_DC 14
#define LCD_CS 5

#define BUZ_FR 3400

AsyncWebServer HTTPserver(80);

String ssid = "MYSTERIOUS-NEW";
String password = "0631603132";
String ap_ssid = "CyberCapsule";
String ap_password = "";
byte TOTALL_COUNT = 36;

TaskHandle_t Task0;
TaskHandle_t Task1;
TaskHandle_t Task2;

unsigned long timers[2] = { 0, 0 };
byte current = TOTALL_COUNT;

typedef struct {
  byte a; // down
  byte b; // up
  byte current;
  byte flag;
  byte mode;
  int speed;
  byte firstInit;
} __attribute__((packed, aligned(1))) ButtonsStruct;
ButtonsStruct buttons;

void emptyFunction() {}

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
  if (!FSInit()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
  } else {
    Serial.println("Load config file");
    loadConfig();
  }
  buttons.a = false;
  buttons.b = false;
  buttons.flag = false;
  buttons.current = 0;
  buttons.mode = 0;
  buttons.speed = 0;
  buttons.firstInit = false;
  //queue = xQueueCreate(1, sizeof(SettingsStruct));
  xTaskCreatePinnedToCore(taskCore2, "Core2", 10000, NULL, 0, &Task2, 0);
  delay(100);
  xTaskCreatePinnedToCore(taskCore1, "Core1", 10000, NULL, 0, &Task1, 1);
  delay(100);
  xTaskCreatePinnedToCore(taskCore0, "Core0", 10000, NULL, 0, &Task0, 1);
}

void taskCore2(void* parameter) { //display
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());
  SPI.begin();
  serialConsileInit();
  displaySetup();
  for (;;) {
    displayLoop();
    consoleLoop();
  }
}

void taskCore0(void* parameter) { //wifi
  Serial.print("Task0 running on core ");
  Serial.println(xPortGetCoreID());
  while (!buttons.firstInit) {
    delay(1);
  }
  bool debugMode = buttons.b;
  delay(100);
  if (debugMode) {
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
  } else {
    Serial.println("No debug mode. No WiFi!");
    //vTaskDelete(Task0);
    //return;
  }
  for (;;) {
    if (debugMode) ArduinoOTA.handle();
    if (buttons.flag == true) {
      buttons.flag = false;
      updateMode();
    } else {
      delayMicroseconds(1);
    }
  }
}

void taskCore1(void* parameter) { //extender
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
  setupExtender();
  readButtons();
  for (;;) {
    extenderLoop();
  }
}

void loop() {
  vTaskDelete(NULL);
}


void updateMode() {
  if (!current) {
    buttons.mode = 10;
  } else if (buttons.mode == 0) { // 0 1
    if (buttons.a && !buttons.b) buttons.mode = 1; //out start
  } else if (buttons.mode == 1) { // 0 0
    if (!buttons.a && !buttons.b) buttons.mode = 2; //out process
  } else if (buttons.mode == 2) { // 1 0
    if (!buttons.a && buttons.b) buttons.mode = 3; //out
    if (buttons.a && !buttons.b) buttons.mode = 7; //in end
    if (buttons.a && buttons.b) {
      current = TOTALL_COUNT;
      buttons.mode = 0; //lock
      redrawAllelements();
    }
  } else if (buttons.mode == 3) { // 0 0
    if (!buttons.a && !buttons.b) buttons.mode = 4; //out end
  } else if (buttons.mode == 4) { // 1 0
    if (!buttons.a && buttons.b) buttons.mode = 5; //in start
  } else if (buttons.mode == 5) { // 0 0
    if (!buttons.a && !buttons.b) buttons.mode = 6; //in process
  } else if (buttons.mode == 6) { // 0 1
    if (!buttons.a && buttons.b) buttons.mode = 3; //in end
    if (buttons.a && !buttons.b) buttons.mode = 7; //in end
  } else if (buttons.mode == 7) { // 0 0
    if (!buttons.a && !buttons.b) buttons.mode = 8; //in process
  } else if (buttons.mode == 8) { // 1 1
    if (!buttons.a && buttons.b) buttons.mode = 3; //in end
    if (buttons.a && buttons.b) {
      current = TOTALL_COUNT;
      buttons.mode = 0; //lock
      redrawAllelements();
    }
  }
  Serial.printf(">\t%d\t%d\t%d\r\n", buttons.a, buttons.b, buttons.mode);

  switch (buttons.mode) {
  case 1:
    buttons.speed = 0;
    off();
    digitalWrite(LCD_BL, HIGH);
    break;

  case 2:
    buttons.speed = 300;
    digitalWrite(LCD_BL, HIGH);
    break;

  case 3:
    buttons.speed = 200;
    digitalWrite(LCD_BL, HIGH);
    break;

  case 4:
    buttons.speed = 200;
    digitalWrite(LCD_BL, HIGH);
    break;

  case 5:
    buttons.speed = 100;
    digitalWrite(LCD_BL, HIGH);
    break;

  case 6:
    buttons.speed = 100;
    digitalWrite(LCD_BL, HIGH);
    break;

  case 7:
    buttons.speed = 50;
    digitalWrite(LCD_BL, HIGH);
    break;

  case 8:
    buttons.speed = 50;
    digitalWrite(LCD_BL, HIGH);
    break;

  default:
    buttons.speed = 0;
    digitalWrite(LCD_BL, LOW);
    off();
    break;
  }
}