#include <FS.h>
#include <ArduinoJson.h>
#include <ErriezSerialTerminal.h>

extern String ssid;
extern String password;
extern String ap_ssid;
extern String ap_password;
extern byte TOTALL_COUNT;
extern byte current;

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
void cmdSet_ssid();
void cmdSet_password();
void cmdSet_ap_ssid();
void cmdSet_ap_password();
void printConsoleChar();
void cmdSet_reboot();
void cmdSet_totall();
bool saveConfig();

void serialConsileInit() {
  term.setDefaultHandler(unknownCommand);

  // Add command callback handlers
  term.addCommand("?", cmdHelp);
  term.addCommand("help", cmdHelp);
  term.addCommand("settings", cmdSettings);
  term.addCommand("ssid", cmdSet_ssid);
  term.addCommand("password", cmdSet_password);
  term.addCommand("ap_ssid", cmdSet_ap_ssid);
  term.addCommand("ap_password", cmdSet_ap_password);
  term.addCommand("reboot", cmdSet_reboot);
  term.addCommand("totall", cmdSet_totall);
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
  Serial.println("\tssid [new value]\t\tWiFi name.");
  Serial.println("\tpassword [new value]\t\tWiFi password.");
  Serial.println("\tap_ssid [new value]\t\tAP name.");
  Serial.println("\tap_password [new value]\t\tAP password.");
  Serial.println("\ttotall [new value]\t\telements count.");
}
void cmdSettings() {
  Serial.println("Current settings:");
  Serial.printf("\tssid - \t\t\t%s\tWiFi name.\r\n", ssid.c_str());
  Serial.printf("\tpassword - \t\t%s\tWiFi password.\r\n", password.c_str());
  Serial.printf("\tap_ssid - \t\t%s\tAP name.\r\n", ap_ssid.c_str());
  Serial.printf("\tap_password - \t\t%s\tAP password.\r\n", ap_password.c_str());
  Serial.printf("\ttotall - \t\t%d\tAP password.\r\n", TOTALL_COUNT);
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
void cmdSet_reboot() {
  delay(500);
  ESP.restart();
}

void cmdSet_totall() {
  TOTALL_COUNT = atoi(term.getNext());
  Serial.printf("Totall elements\t%d\r\n", TOTALL_COUNT);
  saveConfig();
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
    ssid = "MYSTERIOUS-NEW";
    password = "0631603132";
    ap_ssid = "CyberBox";
    ap_password = "0631603132";
    TOTALL_COUNT = 36;
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
  ssid = root["ssid"].as<String>();
  password = root["password"].as<String>();
  ap_ssid = root["ap_ssid"].as<String>();
  ap_password = root["ap_password"].as<String>();
  TOTALL_COUNT = root["totall"].as<int>();
  current = TOTALL_COUNT;
  configFile.close();
  return true;
}

bool saveConfig() {
  DynamicJsonDocument jsonBuffer(1024 + jsonConfig.length());
  deserializeJson(jsonBuffer, jsonConfig);
  JsonObject json = jsonBuffer.as<JsonObject>();
  json["ssid"] = String(ssid);
  json["password"] = String(password);
  json["ap_ssid"] = String(ap_ssid);
  json["ap_password"] = String(ap_password);
  json["totall"] = String(TOTALL_COUNT);
  File configFile = SPIFFS.open("/config.json", "w+");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }
  serializeJson(json, configFile);
  configFile.close();
  return true;
}