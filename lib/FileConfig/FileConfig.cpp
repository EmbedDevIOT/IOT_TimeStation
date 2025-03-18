#include "FileConfig.h"

AT24Cxx eep(ADR_EEPROM, 32);

//////////////////////////////////////////////
// Loading settings from config.json file   //
//////////////////////////////////////////////
void LoadConfig()
{
  // Serial.println("Loading Configuration from config.json");
  String jsonConfig = "";

  File configFile = SPIFFS.open("/config.json", "r"); // Открываем файл для чтения
  jsonConfig = configFile.readString();               // загружаем файл конфигурации из EEPROM в глобальную переменную JsonObject
  configFile.close();

  StaticJsonDocument<1100> doc; //  Резервируем памяь для json обекта
  // Дессиарилизация - преобразование строки в родной объкт JavaScript (или преобразование последовательной формы в параллельную)
  deserializeJson(doc, jsonConfig); //  вызовите парсер JSON через экземпляр jsonBuffer

  char TempBuf[15];

  CFG.Ssid = doc["ssid"].as<String>();
  CFG.Password = doc["pass"].as<String>();

  doc["d1"] = String(SystemClock.year) + "-" + String(SystemClock.month) + "-" + String(SystemClock.date);

  doc["t2"] = String(WatchClock.Hour) + ":" + String(WatchClock.Minute);

  doc["BrStart"].as<String>().toCharArray(TempBuf, 10);
  HWCFG.LedStartHour = atoi(strtok(TempBuf, ":"));
  HWCFG.LedStartMinute = atoi(strtok(NULL, ":"));

  doc["BrStop"].as<String>().toCharArray(TempBuf, 10);
  HWCFG.LedFinishHour = atoi(strtok(TempBuf, ":"));
  HWCFG.LedFinishMinute = atoi(strtok(NULL, ":"));

  doc["TimSGPS"].as<String>().toCharArray(TempBuf, 10);
  HWCFG.GPSStartHour = atoi(strtok(TempBuf, ":"));
  HWCFG.GPSStartMin = atoi(strtok(NULL, ":"));
  // Config.GPSStartSec = atoi(strtok(NULL, ":"));

  HWCFG.GMT = doc["tz"];
  HWCFG.LedOnOFF = doc["LightEN"];
  HWCFG.LedON = doc["LedON"];

  HWCFG.L_Lim = doc["sens"];
  HWCFG.i_sens = doc["i_sens"];
  HWCFG.T1_ofs = doc["t1_offset"];

  // Config.i_cor = doc["i_cor"];

  WatchClock.PulseNormal = doc["pulsn"];
  WatchClock.PulseFast = doc["pulsf"];

  HWCFG.I_PROT = doc["iLimit"];
  HWCFG.BAT_PROT = doc["batlim"];

  HWCFG.BtnMode = doc["btn"];

  HWCFG.GPSMode = doc["GPSMode"];

  HWCFG.GPSSynh = doc["GPSSyn"];

  CFG.IP1 = doc["ip1"];
  CFG.IP2 = doc["ip2"];
  CFG.IP3 = doc["ip3"];
  CFG.IP4 = doc["ip4"];
  CFG.GW1 = doc["gw1"];
  CFG.GW2 = doc["gw2"];
  CFG.GW3 = doc["gw3"];
  CFG.GW4 = doc["gw4"];
  CFG.MK1 = doc["mk1"];
  CFG.MK2 = doc["mk2"];
  CFG.MK3 = doc["mk3"];
  CFG.MK4 = doc["mk4"];

  CFG.NTPServer = doc["ntpurl"].as<String>();

  CFG.APSSID = doc["apssid"].as<String>();
  CFG.APPAS = doc["appass"].as<String>();

  CFG.sn = doc["sn"];
  CFG.fw = doc["firmware"].as<String>();


  WatchClock.ClockST = doc["ClockST1"];
  WatchClock.Volt = doc["volt1"];
  // SecondaryClock2.ClockST = doc["ClockST2"];

  HWCFG.Bright = doc["br"];
  HWCFG.VOL = doc["vol"];

  STATE.WiFiEnable = doc["wifi_st"];
}

void ShowLoadJSONConfig()
{
  char msg[32] = {0}; // buff for send message

  Serial.println(F("##############  System Configuration  ###############"));
  Serial.println("-------------------- USER DATA -----------------------");
  Serial.printf("####  T1_OFFSET: %d \r\n", HWCFG.T1_ofs);
  Serial.println("------------------ USER DATA END----------------------");
  Serial.println();
  Serial.println("---------------------- SYSTEM ------------------------");
  Serial.printf("####  GMT: %d \r\n", HWCFG.GMT);
  sprintf(msg, "####  DATA: %0002d-%02d-%02d", SystemClock.year, SystemClock.month, SystemClock.date);
  Serial.println(F(msg));
  sprintf(msg, "####  TIME: %02d:%02d:%02d", SystemClock.hour, SystemClock.minute, SystemClock.second);
  Serial.println(F(msg));
  Serial.printf("####  WiFI_PWR: %d \r\n", STATE.WiFiEnable);
  Serial.printf("####  WiFI NAME: %s \r\n",CFG.APSSID);
  Serial.printf("####  WiFI PASS: %s \r\n", CFG.APPAS);
  sprintf(msg, "####  IP: %00d.%00d.%00d.%00d", CFG.IP1, CFG.IP2, CFG.IP3, CFG.IP4);
  Serial.println(F(msg));
  Serial.printf("####  Brigh: %d \r\n", HWCFG.Bright);
  Serial.printf("####  Volume: %d \r\n", HWCFG.VOL);
  Serial.printf("####  SN: %d", CFG.sn);
  Serial.printf(" FW:");
  Serial.print(CFG.fw);
  Serial.println();
  Serial.println("-------------------- SYSTEM END-----------------------");
  Serial.println(F("######################################################"));
}

//////////////////////////////////////////////
// Save configuration in config.json file   //
//////////////////////////////////////////////
void SaveConfig()
{
  String jsonConfig = "";
  StaticJsonDocument<1100> doc;

  doc["d1"] = String(SystemClock.year) + "-" + ((SystemClock.month < 10) ? "0" : "") + String(SystemClock.month) + "-" + ((SystemClock.date < 10) ? "0" : "") + String(SystemClock.date);
  doc["t2"] = ((WatchClock.Hour < 10) ? "0" : "") + String(WatchClock.Hour) + ":" + ((WatchClock.Minute < 10) ? "0" : "") + String(WatchClock.Minute);
  doc["BrStart"] = ((HWCFG.LedStartHour < 10) ? "0" : "") + String(HWCFG.LedStartHour) + ":" + ((HWCFG.LedStartMinute < 10) ? "0" : "") + String(HWCFG.LedStartMinute);
  doc["BrStop"] = ((HWCFG.LedFinishHour < 10) ? "0" : "") + String(HWCFG.LedFinishHour) + ":" + ((HWCFG.LedFinishMinute < 10) ? "0" : "") + String(HWCFG.LedFinishMinute);

  doc["TimSGPS"] = ((HWCFG.GPSStartHour < 10) ? "0" : "") + String(HWCFG.GPSStartHour) + ":" + ((HWCFG.GPSStartMin < 10) ? "0" : "") + String(HWCFG.GPSStartMin);

  doc["tz"] = HWCFG.GMT;

  doc["GPSMode"] = HWCFG.GPSMode;

  doc["GPSSyn"] = HWCFG.GPSSynh;

  doc["btn"] = HWCFG.BtnMode;

  doc["LightEN"] = HWCFG.LedOnOFF;

  doc["LedON"] = HWCFG.LedON;

  doc["sens"] = HWCFG.L_Lim;

  doc["i_sens"] = HWCFG.i_sens;
  // doc["i_cor"] = Config.i_cor;

  doc["iLimit"] = HWCFG.I_PROT;

  doc["pulsn"] = WatchClock.PulseNormal;
  doc["pulsf"] = WatchClock.PulseFast;

  doc["batlim"] = HWCFG.BAT_PROT;

  doc["firmware"] = CFG.fw;
  doc["MAC"] = CFG.MacAdr;
  doc["sn"] = CFG.sn;

  doc["ssid"] = CFG.Ssid;
  doc["pass"] = CFG.Password;

  doc["ip1"] = CFG.IP1;
  doc["ip2"] = CFG.IP2;
  doc["ip3"] = CFG.IP3;
  doc["ip4"] = CFG.IP4;
  doc["gw1"] = CFG.GW1;
  doc["gw2"] = CFG.GW2;
  doc["gw3"] = CFG.GW3;
  doc["gw4"] = CFG.GW4;
  doc["mk1"] = CFG.MK1;
  doc["mk2"] = CFG.MK2;
  doc["mk3"] = CFG.MK3;
  doc["mk4"] = CFG.MK4;

  doc["ntpurl"] = CFG.NTPServer;
  doc["apssid"] = CFG.APSSID;
  doc["appass"] = CFG.APPAS;

  doc["WiFiMode"] = CFG.WiFiMode;

  doc["ClockST1"] = WatchClock.ClockST;
  doc["volt1"] = WatchClock.Volt;  

  // doc["ClockST2"] = SecondaryClock2.ClockST;


  File configFile = SPIFFS.open("/config.json", "w");
  serializeJson(doc, configFile); // Writing json string to file
  configFile.close();

  Serial.println("Configuration SAVE");
}

void TestDeserializJSON()
{
  String jsonConfig = "";

  File configFile = SPIFFS.open("/config.json", "r"); // Открываем файл для чтения
  jsonConfig = configFile.readString();               // загружаем файл конфигурации из EEPROM в глобальную переменную JsonObject
  configFile.close();

  StaticJsonDocument<2048> doc;
  deserializeJson(doc, jsonConfig); //  вызовите парсер JSON через экземпляр jsonBuffer

  serializeJsonPretty(doc, Serial);
  Serial.println();

  Serial.println("JSON testing comleted");
}

// Write data to EEPROM
void EEP_Write()
{
  eep.write(0, WatchClock.Hour);
  eep.write(1, WatchClock.Minute);
  eep.write(2, WatchClock.Polarity);
  eep.write(3, WatchClock.Start);
  eep.write(4, CFG.WiFiMode);       // ???
}

// Reading data from EEPROM
void EEP_Read()
{
  WatchClock.Hour = eep.read(0);
  WatchClock.Minute = eep.read(1);
  WatchClock.Polarity = eep.read(2);
  WatchClock.Start = eep.read(3);
  CFG.WiFiMode = eep.read(4);         // ???
}