#include "FileConfig.h"

AT24Cxx eep(0x57, 32);

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

  StaticJsonDocument<2048> doc; //  Резервируем памяь для json обекта
  // Дессиарилизация - преобразование строки в родной объкт JavaScript (или преобразование последовательной формы в параллельную)
  deserializeJson(doc, jsonConfig); //  вызовите парсер JSON через экземпляр jsonBuffer

  char TempBuf[15];

  struct _txt
  {
    char TN[17];     // car_name
    uint8_t TNU = 0; // car num
    bool SW = true;
  } T;

  struct _sys
  {
    uint8_t H = 0;
    uint8_t M = 0;
    int D = 0;
    int MO = 0;
    int Y = 0;
  } S;

  HWCFG.bright = doc["br"];
  HWCFG.VOL = doc["vol"];

  STATE.WiFiEnable = doc["wifi_st"];

  // ColorSet(&col_carnum, doc["c_carnum"]);
  // ColorSet(&col_date, doc["c_date"]);
  // ColorSet(&col_day, doc["c_day"]);
  // ColorSet(&col_tempin, doc["c_tempin"]);
  // ColorSet(&col_tempout, doc["c_tempout"]);
  // ColorSet(&col_time, doc["c_time"]);
  // ColorSet(&col_time, doc["c_speed"]);

  // doc["carname"].as<String>().toCharArray(T.TN, 17);
  // memset(UserText.carname, 0, strlen(UserText.carname));
  // strcat(UserText.carname, T.TN);

  // UserText.carnum = doc["carnum"];

  // doc["date"].as<String>().toCharArray(TempBuf, 15);
  // S.Y = atoi(strtok(TempBuf, "-"));
  // S.MO = atoi(strtok(NULL, "-"));
  // S.D = atoi(strtok(NULL, "-"));

  CFG.fw = doc["firmware"].as<String>();

  // UserText.hide_t = doc["hide"];

  CFG.IP1 = doc["ip1"];
  CFG.IP2 = doc["ip2"];
  CFG.IP3 = doc["ip3"];
  CFG.IP4 = doc["ip4"];

  CFG.APPAS = doc["pass"].as<String>();

  CFG.sn = doc["sn"];

  CFG.APSSID = doc["ssid"].as<String>();

  HWCFG.T1_off = doc["t1_offset"];

  CFG.gmt = doc["gmt"];
}

void ShowLoadJSONConfig()
{
  char msg[32] = {0}; // buff for send message

  Serial.println(F("##############  System Configuration  ###############"));
  Serial.println("-------------------- USER DATA -----------------------");
  Serial.printf("####  T1_OFFSET: %d \r\n", HWCFG.T1_off);
  Serial.println("------------------ USER DATA END----------------------");
  Serial.println();
  Serial.println("---------------------- SYSTEM ------------------------");
  Serial.printf("####  GMT: %d \r\n", CFG.gmt);
  sprintf(msg, "####  DATA: %0002d-%02d-%02d", Clock.year, Clock.month, Clock.date);
  Serial.println(F(msg));
  sprintf(msg, "####  TIME: %02d:%02d:%02d", Clock.hour, Clock.minute, Clock.second);
  Serial.println(F(msg));
  Serial.printf("####  WiFI_PWR: %d \r\n", STATE.WiFiEnable);
  Serial.printf("####  WiFI NAME: %s \r\n",CFG.APSSID);
  Serial.printf("####  WiFI PASS: %s \r\n", CFG.APPAS);
  sprintf(msg, "####  IP: %00d.%00d.%00d.%00d", CFG.IP1, CFG.IP2, CFG.IP3, CFG.IP4);
  Serial.println(F(msg));
  Serial.printf("####  Brigh: %d \r\n", HWCFG.bright);
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
  StaticJsonDocument<2048> doc;

  doc["br"] = HWCFG.bright;
  doc["vol"] = HWCFG.VOL;
  doc["wifi_st"] = STATE.WiFiEnable;

  // doc["c_carnum"] = GetColorNum(&col_carnum);
  // doc["c_date"] = GetColorNum(&col_date);
  // doc["c_day"] = GetColorNum(&col_day);
  // doc["c_tempin"] = GetColorNum(&col_tempin);
  // doc["c_tempout"] = GetColorNum(&col_tempout);
  // doc["c_time"] = GetColorNum(&col_time);
  // doc["c_speed"] = GetColorNum(&col_speed);

  // doc["carname"] = String(UserText.carname);
  // doc["carnum"] = UserText.carnum;
  doc["firmware"] = CFG.fw;
  doc["ip1"] = CFG.IP1;
  doc["ip2"] = CFG.IP2;
  doc["ip3"] = CFG.IP3;
  doc["ip4"] = CFG.IP4;
  doc["pass"] = CFG.APPAS;
  doc["sn"] = CFG.sn;
  doc["ssid"] = CFG.APSSID;
  doc["t1_offset"] = HWCFG.T1_off;
  doc["gmt"] = CFG.gmt;

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

void EEP_Write()
{
  // eep.write(0, CFG.sn);
}

// Reading data from EEPROM
void EEP_Read()
{
  // CFG.sn = eep.read(0);
}