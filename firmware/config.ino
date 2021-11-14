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
  Serial.print("Unknown command: ");
  Serial.println(command);
  Serial.println("Use 'help' or '?' for command list.");
}
void cmdHelp() {
  // Print usage
  Serial.println("Serial terminal usage:");
  Serial.println("\thelp or ?\t\t\tprint this usage");
  Serial.println("\tsettings\t\t\tshow current settings");
  Serial.println("\treboot\t\t\t\treboot device");
  Serial.println("\tlineColorChanceUp [new value]\tChance to change color in percent. Per level.");
  Serial.println("\tlineChaosUp [new value]\t\tValue for random line position in pixels. Per level.");
  Serial.println("\tlineChanceUp [new value]\tChance to change line position in percent. Per level.");
  Serial.println("\tlineCycleDown [new value]\tLine speed Up. Per level.");
  Serial.println("\tuserRadiusDown [new value]\tChange user marker radius in px. Per level.");
  Serial.println("\tlineStepUp [new value]\t\tLine step per cycle in px. Per level..");
  Serial.println("\tssid [new value]\t\tWiFi name.");
  Serial.println("\tpassword [new value]\t\tWiFi password.");
  Serial.println("\tap_ssid [new value]\t\tAP name.");
  Serial.println("\tap_password [new value]\t\tAP password.");
  Serial.println("\tlevels [new value]\t\tCout of levels.");
}
void cmdSettings() {
  Serial.println("Current settings:");
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
  Serial.printf("Chance to change color in percent. Per level\r\n");
  saveConfig();
}
void cmdSet_lineChaosUp() {
  gameSettings.lineChaosUp = atoi(term.getNext());
  Serial.printf("Value for random line position in pixels. Per level\r\n");
  saveConfig();
}
void cmdSet_lineChanceUp() {
  gameSettings.lineChanceUp = atoi(term.getNext());
  Serial.printf("Chance to change line position in percent. Per level\r\n");
  saveConfig();
}
void cmdSet_lineCycleDown() {
  gameSettings.lineCycleDown = atoi(term.getNext());
  Serial.printf("Line speed Up. Per level\r\n");
  saveConfig();
}
void cmdSet_userRadiusDown() {
  gameSettings.userRadiusDown = atoi(term.getNext());
  Serial.printf("Change user marker radius in px. Per level\r\n");
  saveConfig();
}
void cmdSet_lineStepUp() {
  gameSettings.lineStepUp = atoi(term.getNext());
  Serial.printf("Line step per cycle in px. Per level.\r\n");
  saveConfig();
}
void cmdSet_ssid() {
  ssid = term.getNext();
  Serial.printf("WiFi name\r\n");
  saveConfig();
}
void cmdSet_password() {
  password = term.getNext();
  Serial.printf("WiFi password\r\n");
  saveConfig();
}
void cmdSet_ap_ssid() {
  ap_ssid = term.getNext();
  Serial.printf("AP name\r\n");
  saveConfig();
}
void cmdSet_ap_password() {
  ap_password = term.getNext();
  Serial.printf("AP password\r\n");
  saveConfig();
}
void cmdSet_levels() {
  gameSettings.levels = atoi(term.getNext());
  Serial.printf("Cout of levels\r\n");
  saveConfig();
}
void cmdSet_reboot() {
  delay(500);
  ESP.restart();
}

bool FSInit() {
  return SPIFFS.begin();
}

bool loadConfig() {
  //SPIFFS.remove("/config.json"); Serial.println("************ Config reset ************");
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
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
    Serial.println("Config file size is too large");
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
    Serial.println("Failed to open config file for writing");
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

File readFile(char* qrName) {
  return SPIFFS.open(String("/") + String(qrName), "r");
}

void closeFile(File an) {
  an.close();
}