#include <FS.h>
#include <ArduinoJson.h>
#include <ErriezSerialTerminal.h>

extern GameSettingsStruct gameSettings;
extern String ssid;
extern String password;
extern String ap_ssid;
extern String ap_password;

String jsonConfig = "{}";
// Newline character '\r' or '\n'
char newlineChar = '\r';
// Separator character between commands and arguments
char delimiterChar = ' ';

// Create serial terminal object
SerialTerminal term(newlineChar, delimiterChar);

void unknownCommand(const char* command);
void cmdHelp();
void cmdSettings();
void cmdSet_lineColorChanceUp();
void cmdSet_lineChaosUp();
void cmdSet_lineChanceUp();
void cmdSet_lineCycleDown();
void cmdSet_userRadiusDown();
void cmdSet_lineStepUp();
void cmdSet_ssid();
void cmdSet_password();
void cmdSet_ap_ssid();
void cmdSet_ap_password();
void cmdSet_levels();
void printConsoleChar();
void cmdSet_reboot();
void cmdSet_rfid();

bool saveConfig();

void serialConsileInit() {
  term.setDefaultHandler(unknownCommand);

  // Add command callback handlers
  term.addCommand("?", cmdHelp);
  term.addCommand("help", cmdHelp);
  term.addCommand("settings", cmdSettings);
  term.addCommand("lineColorChanceUp", cmdSet_lineColorChanceUp);
  term.addCommand("lineChaosUp", cmdSet_lineChaosUp);
  term.addCommand("lineChanceUp", cmdSet_lineChanceUp);
  term.addCommand("lineCycleDown", cmdSet_lineCycleDown);
  term.addCommand("userRadiusDown", cmdSet_userRadiusDown);
  term.addCommand("lineStepUp", cmdSet_lineStepUp);
  term.addCommand("ssid", cmdSet_ssid);
  term.addCommand("password", cmdSet_password);
  term.addCommand("ap_ssid", cmdSet_ap_ssid);
  term.addCommand("ap_password", cmdSet_ap_password);
  term.addCommand("levels", cmdSet_levels);
  term.addCommand("reboot", cmdSet_reboot);
  term.addCommand("rfid", cmdSet_rfid);
  //Enable Char Echoing
  //term.setSerialEcho(true);
  //Set Post Command Handler
  //term.setPostCommandHandler(printConsoleChar);
}
void consoleLoop() {
  // Read from serial port and handle command callbacks
  term.readSerial();
}

void printConsoleChar() {
  Serial.print("> ");
}

void unknownCommand(const char* command) {
  // Print unknown command
  Serial.print(F("Unknown command: "));
  Serial.println(command);
  Serial.println(F("Use 'help' or '?' for command list."));
}
void cmdHelp() {
  // Print usage
  Serial.println(F("Serial terminal usage:"));
  Serial.println(F("\thelp or ?\t\t\tprint this usage"));
  Serial.println(F("\tsettings\t\t\tshow current settings"));
  Serial.println(F("\treboot\t\t\t\treboot device"));
  Serial.println(F("\tlineColorChanceUp [new value]\tChance to change color in percent. Per level."));
  Serial.println(F("\tlineChaosUp [new value]\t\tValue for random line position in pixels. Per level."));
  Serial.println(F("\tlineChanceUp [new value]\tChance to change line position in percent. Per level."));
  Serial.println(F("\tlineCycleDown [new value]\tLine speed Up. Per level."));
  Serial.println(F("\tuserRadiusDown [new value]\tChange user marker radius in px. Per level."));
  Serial.println(F("\tlineStepUp [new value]\t\tLine step per cycle in px. Per level.."));
  Serial.println(F("\tssid [new value]\t\tWiFi name."));
  Serial.println(F("\tpassword [new value]\t\tWiFi password."));
  Serial.println(F("\tap_ssid [new value]\t\tAP name."));
  Serial.println(F("\tap_password [new value]\t\tAP password."));
  Serial.println(F("\tlevels [new value]\t\tCout of levels."));
  Serial.println(F("\trfid\t\tGet RFID data."));
}
void cmdSettings() {
  Serial.println(F("Current settings:"));
  Serial.printf("\tlineColorChanceUp - \t%d\tChance to change color in percent. Per level.\r\n", gameSettings.lineColorChanceUp);
  Serial.printf("\tlineChaosUp - \t\t%d\tValue for random line position in pixels. Per level.\r\n", gameSettings.lineChaosUp);
  Serial.printf("\tlineChanceUp - \t\t%d\tChance to change line position in percent. Per level.\r\n", gameSettings.lineChanceUp);
  Serial.printf("\tlineCycleDown - \t%d\tLine speed Up. Per level.\r\n", gameSettings.lineCycleDown);
  Serial.printf("\tuserRadiusDown - \t%d\tChange user marker radius in px. Per level.\r\n", gameSettings.userRadiusDown);
  Serial.printf("\tlineStepUp - \t\t%d\tLine step per cycle in px. Per level.\r\n", gameSettings.lineStepUp);
  Serial.printf("\tlevels - \t\t%d\tCout of levels.\r\n", gameSettings.levels);
  Serial.printf("\tssid - \t\t\t%s\tWiFi name.\r\n", ssid.c_str());
  Serial.printf("\tpassword - \t\t%s\tWiFi password.\r\n", password.c_str());
  Serial.printf("\tap_ssid - \t\t%s\tAP name.\r\n", ap_ssid.c_str());
  Serial.printf("\tap_password - \t\t%s\tAP password.\r\n", ap_password.c_str());
}
void cmdSet_lineColorChanceUp() {
  gameSettings.lineColorChanceUp = atoi(term.getNext());
  Serial.println(F("Chance to change color in percent. Per level"));
  saveConfig();
}
void cmdSet_lineChaosUp() {
  gameSettings.lineChaosUp = atoi(term.getNext());
  Serial.println(F("Value for random line position in pixels. Per level"));
  saveConfig();
}
void cmdSet_lineChanceUp() {
  gameSettings.lineChanceUp = atoi(term.getNext());
  Serial.println(F("Chance to change line position in percent. Per level\r\n"));
  saveConfig();
}
void cmdSet_lineCycleDown() {
  gameSettings.lineCycleDown = atoi(term.getNext());
  Serial.println(F("Line speed Up. Per level"));
  saveConfig();
}
void cmdSet_userRadiusDown() {
  gameSettings.userRadiusDown = atoi(term.getNext());
  Serial.println(F("Change user marker radius in px. Per level"));
  saveConfig();
}
void cmdSet_lineStepUp() {
  gameSettings.lineStepUp = atoi(term.getNext());
  Serial.println(F("Line step per cycle in px. Per level."));
  saveConfig();
}
void cmdSet_ssid() {
  ssid = term.getNext();
  Serial.println(F("WiFi name"));
  saveConfig();
}
void cmdSet_password() {
  password = term.getNext();
  Serial.println(F("WiFi password"));
  saveConfig();
}
void cmdSet_ap_ssid() {
  ap_ssid = term.getNext();
  Serial.println(F("AP name"));
  saveConfig();
}
void cmdSet_ap_password() {
  ap_password = term.getNext();
  Serial.println(F("AP password"));
  saveConfig();
}
void cmdSet_levels() {
  gameSettings.levels = atoi(term.getNext());
  Serial.println(F("Cout of levels"));
  saveConfig();
}
void cmdSet_reboot() {
  delay(500);
  ESP.restart();
}
void cmdSet_rfid() {
  Serial.println(F("RFID"));
  readRFID();
}

bool FSInit() {
  return SPIFFS.begin();
}

bool loadConfig() {
  //SPIFFS.remove("/config.json"); Serial.println("************ Config reset ************");
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println(F("Failed to open config file"));
    configFile.close();
    gameSettings.lineColorChanceUp = 3;
    gameSettings.lineChaosUp = 10;
    gameSettings.lineChanceUp = 4;
    gameSettings.lineCycleDown = 1;
    gameSettings.userRadiusDown = 1;
    gameSettings.lineStepUp = 1;
    ssid = "MYSTERIOUS-NEW";
    password = "0631603132";
    ap_ssid = "CyberBox";
    ap_password = "0631603132";
    gameSettings.levels = 5;
    saveConfig();
    return false;
  }
  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println(F("Config file size is too large"));
    configFile.close();
    return false;
  }
  jsonConfig = configFile.readString();
  DynamicJsonDocument jsonBuffer(1024 + jsonConfig.length());
  deserializeJson(jsonBuffer, jsonConfig);
  JsonObject root = jsonBuffer.as<JsonObject>();
  gameSettings.lineColorChanceUp = root["lineColorChanceUp"].as<int>();
  gameSettings.lineChaosUp = root["lineChaosUp"].as<int>();
  gameSettings.lineChanceUp = root["lineChanceUp"].as<int>();
  gameSettings.lineCycleDown = root["lineCycleDown"].as<int>();
  gameSettings.userRadiusDown = root["userRadiusDown"].as<int>();
  gameSettings.lineStepUp = root["lineStepUp"].as<int>();
  ssid = root["ssid"].as<String>();
  password = root["password"].as<String>();
  ap_ssid = root["ap_ssid"].as<String>();
  ap_password = root["ap_password"].as<String>();
  gameSettings.levels = root["levels"].as<int>();
  configFile.close();
  return true;
}

bool saveConfig() {
  DynamicJsonDocument jsonBuffer(1024 + jsonConfig.length());
  deserializeJson(jsonBuffer, jsonConfig);
  JsonObject json = jsonBuffer.as<JsonObject>();
  json["lineColorChanceUp"] = String(gameSettings.lineColorChanceUp);
  json["lineChaosUp"] = String(gameSettings.lineChaosUp);
  json["lineChanceUp"] = String(gameSettings.lineChanceUp);
  json["lineCycleDown"] = String(gameSettings.lineCycleDown);
  json["userRadiusDown"] = String(gameSettings.userRadiusDown);
  json["lineStepUp"] = String(gameSettings.lineStepUp);
  json["ssid"] = String(ssid);
  json["password"] = String(password);
  json["ap_ssid"] = String(ap_ssid);
  json["ap_password"] = String(ap_password);
  json["levels"] = String(gameSettings.levels);
  File configFile = SPIFFS.open("/config.json", "w+");
  if (!configFile) {
    Serial.println(F("Failed to open config file for writing"));
    return false;
  }
  serializeJson(json, configFile);
  configFile.close();
  return true;
}

void readFileToArray(char* qrName, byte* data) {
  String filename = String("/") + String(qrName);
  File file = SPIFFS.open(filename, "r");
  if (!file) {
    Serial.printf("Failed to open %s for reading\r\n", filename);
    return;
  }
  Serial.printf("File size %d\r\n", file.size());
  byte index = 0;
  while (file.available()) {
    data[index++] = file.read();
  }
  file.close();
}

File readFile(String qrName) {
  return SPIFFS.open(String("/") + qrName, "r");
}

void closeFile(File an) {
  an.close();
}

bool readRFIDFile() {
  String fileName = String(ConnectorsStatus.RFID);
  Serial.printf("Read capsule %s\r\n", fileName.c_str());
  fileName = "/" + fileName;
  File configFile = SPIFFS.open(fileName + ".json", "r");
  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println(F("Config file size is too large"));
    configFile.close();
    return false;
  }
  jsonConfig = configFile.readString();
  DynamicJsonDocument jsonBuffer(1024 + jsonConfig.length());
  deserializeJson(jsonBuffer, jsonConfig);
  JsonObject root = jsonBuffer.as<JsonObject>();
  RFIDSettings.lineColorChance[0] = root["lineColorChance_0"].as<int>();
  RFIDSettings.lineColorChance[1] = root["lineColorChance_1"].as<int>();
  RFIDSettings.lineChance[0] = root["lineChance_0"].as<int>();
  RFIDSettings.lineChance[1] = root["lineChance_1"].as<int>();
  RFIDSettings.lineChaos[0] = root["lineChaos_0"].as<int>();
  RFIDSettings.lineChaos[1] = root["lineChaos_1"].as<int>();
  RFIDSettings.positionBonus[0] = root["positionBonus_0"].as<int>();
  RFIDSettings.positionBonus[1] = root["positionBonus_1"].as<int>();
  RFIDSettings.colorBonus[0] = root["colorBonus_0"].as<int>();
  RFIDSettings.colorBonus[1] = root["colorBonus_1"].as<int>();
  RFIDSettings.maxScore = root["maxScore"].as<int>();
  RFIDSettings.userRadius = root["userRadius"].as<int>();
  RFIDSettings.lineCycle = root["lineCycle"].as<int>();
  RFIDSettings.lineStep = root["lineStep"].as<int>();
  configFile.close();
  return true;
}