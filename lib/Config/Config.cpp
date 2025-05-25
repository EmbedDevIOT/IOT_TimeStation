#include "Config.h"

//=========================================================================
boolean SerialNumConfig()
{
  bool sn = false;
  if (CFG.sn == 0)
  {
    Serial.println("Serial number: FALL");
    Serial.println("Please enter SN");
    sn = true;
  }

  while (sn)
  {
    if (Serial.available())
    {

      digitalWrite(LED_STG, HIGH);

      String r = Serial.readString();
      bool block_st = false;
      r.trim();
      CFG.sn = r.toInt();

      digitalWrite(LED_STG, LOW);

      if (CFG.sn >= 1)
      {
        Serial.printf("Serial number: %d DONE \r\n", CFG.sn);
        sn = false;
        return true;
      }

      log_i("free heap=%i", ESP.getFreeHeap());
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
  }
  return false;
}
//=========================================================================

//=======================================================================
void UserPresetInit()
{
}

/**************************************** Scanning I2C bus *********************************************/
void I2C_Scanning(void)
{
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 8; address < 127; address++)
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknow error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(5000);
}
/*******************************************************************************************************/
void GPIOInit()
{
#ifdef DEBUG
  Serial.print(F("GPIO_Init..."));
#endif

  pinMode(LED_STG, OUTPUT);
  digitalWrite(LED_STG, LOW);
  pinMode(LED_STR, OUTPUT);
  digitalWrite(LED_STR, LOW);
  pinMode(LED_CHA, OUTPUT);
  digitalWrite(LED_CHA, HIGH);
  pinMode(LED_CHB, OUTPUT);
  digitalWrite(LED_CHB, HIGH);

  pinMode(PWR_SEN, INPUT);

#ifdef DEBUG
  Serial.println(F("DONE"));
#endif
}

bool RTCInit(void)
{
  bool errRTC = true;
#ifdef DEBUG
  Serial.print(F("RTC_Init..."));
#endif
  errRTC = RTC.begin();
  if (RTC.lostPower())
  {
    RTC.setTime(COMPILE_TIME);
  }
  SystemClock = RTC.getTime();
#ifdef DEBUG
  Serial.println(F("DONE"));
#endif
  return errRTC;
}
/************************ System Initialisation **********************/
void SystemStateInit(void)
{
#ifdef DEBUG
  Serial.print(F("System_State_Init..."));
#endif
  STATE.Watch1_EN = false;
  STATE.IDLE = true;
  STATE.LedWiFi = false;
  STATE.SaveFlash = false;
  STATE.Debug = true;
  STATE.CurDebug = false;
  STATE.WiFiEnable = true;
  STATE.TTS = false; // Flag Start Time Speech
  STATE.VolumeUPD = false;
  GetChipID();
#ifdef DEBUG
  Serial.println(F("DONE"));
#endif
}
/*******************************************************************************************************/

/***************************** Function Show information or Device *************************************/
void ShowInfoDevice(void)
{
  Serial.println(F("Starting..."));
  Serial.println(F("IOT TimeStation"));
  Serial.print(F("SN:"));
  Serial.println(CFG.sn);
  Serial.print(F("fw_date:"));
  Serial.println(CFG.fwdate);
  Serial.println(CFG.fw);
  Serial.println(CFG.chipID);
  Serial.println(F("by EmbedDev"));
  Serial.println();
}
/*******************************************************************************************************/
/*******************************************************************************************************/
void GetChipID()
{
  uint32_t chipId = 0;

  for (int i = 0; i < 17; i = i + 8)
  {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  CFG.chipID = chipId;
}
/*******************************************************************************************************/
/*******************************************************************************************************/
String GetMacAdr()
{
  CFG.MacAdr = WiFi.macAddress(); //
  Serial.print(F("MAC:"));        // временно
  Serial.println(CFG.MacAdr);     // временно
  return WiFi.macAddress();
}
/*******************************************************************************************************/

/*******************************************************************************************************/
// Debug information
void DebugInfo()
{
#ifdef DEBUG
  char message[37];

  Serial.println(F("!!!!!!!!!!!!!!  DEBUG INFO  !!!!!!!!!!!!!!!!!!"));
  // ESP_LOGI("DEBUG", "GMT: %d", HWCFG.GMT);
  ESP_LOGI("DEBUG", "Power State: %d", HWCFG.PwrState);
  ESP_LOGI("DEBUG", "Battery: %0.2f V %3d %", HWCFG.BatVoltage, HWCFG.BatPercent);
  ESP_LOGI("DEBUG", "Watch VCC %2d", WatchClock.Volt);
  ESP_LOGI("DEBUG", "System Time: %02d:%02d:%02d", SystemClock.hour, SystemClock.minute, SystemClock.second);
  ESP_LOGI("DEBUG", "System Date: %4d.%02d.%02d", SystemClock.year, SystemClock.month, SystemClock.date);
  ESP_LOGI("DEBUG", "Watch Start: %d State: %d", WatchClock.Start, WatchClock.ClockState);
  ESP_LOGI("DEBUG", "Watch: %02d:%02d Polar %2d", WatchClock.Hour, WatchClock.Minute, WatchClock.Polarity);
  // ESP_LOGI("DEBUG", "Watch2 Start: %d State: %d", WatchClock2.Start, WatchClock2.ClockState);
  ESP_LOGI("DEBUG", "Bright: ON %02d:%02d OFF: %02d:%02d", HWCFG.LedStartHour, HWCFG.LedStartMinute, HWCFG.LedFinishHour, HWCFG.LedFinishMinute);
  ESP_LOGI("DEBUG", "Bright: LedON %d | LedOnOFF %2d", HWCFG.LedON, HWCFG.LedOnOFF);
  // ESP_LOGI("DEBUG", "T1: %0.1f T1_OFS: %d", HWCFG.dsT1, HWCFG.T1_ofs);
  // ESP_LOGI("DEBUG", "VOL: %d", HWCFG.VOL);
  ESP_LOGI("DEBUG", "INA226: %.1f V %.1f mA", HWCFG.voltage, HWCFG.current);
  // If Preparation to Time Synch - done -> PWR GPS -> ON
  // if (HWCFG.GPSMode == GPS_ONCE)
  // {
  //   ESP_LOGI("DEBUG", "GPS: PWR: %d, MODE: ONCE (to %02d:%02d)", HWCFG.GPSPWR, HWCFG.GPSStartHour, HWCFG.GPSStartMin);
  // }
  // else
  // {
  //   ESP_LOGI("DEBUG", "GPS: PWR: %d, MODE: %d", HWCFG.GPSPWR, HWCFG.GPSMode);
  // }
  // ESP_LOGI("DEBUG", "BTN Mode %1d", HWCFG.BtnMode);

  // // If Power connectet to External VCC -> Show WiFi State
  // // Else Show elasped time to WiFi OFF
  // if (HWCFG.PwrState)
  // {
  //   ESP_LOGI("DEBUG", "WiFi EN: %d", STATE.WiFiEnable);
  // }
  // else
  // {
  //   ESP_LOGI("DEBUG", "WiFi EN: %d T:%02d:%02d", STATE.WiFiEnable, NetworkCFG.TimMin, NetworkCFG.TimSec);
  // }

  // ESP_LOGI("DEBUG", "SN: %d", CFG.sn);

  Serial.println(F("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
  Serial.println();
#endif
}

//=======================================================================

/*******************************************************************************************************/
void ShowFlashSave()
{
  bool state = false;

  for (uint8_t i = 0; i <= 1; i++)
  {
    digitalWrite(LED_STG, ON);
    vTaskDelay(400 / portTICK_PERIOD_MS);
    digitalWrite(LED_STG, OFF);
    vTaskDelay(400 / portTICK_PERIOD_MS);
  }
}
/***************************************************** ****************************/
void SystemFactoryReset()
{
  HWCFG.GMT = 3;
  NetworkCFG.WiFiMode = AccessPoint;
  NetworkCFG.APSSID = "GKTime";
  NetworkCFG.APPAS = "gktime";
  NetworkCFG.IP1 = 192;
  NetworkCFG.IP2 = 168;
  NetworkCFG.IP3 = 1;
  NetworkCFG.IP4 = 1;
  NetworkCFG.GW1 = 192;
  NetworkCFG.GW2 = 168;
  NetworkCFG.GW3 = 1;
  NetworkCFG.GW4 = 1;
  NetworkCFG.MK1 = 255;
  NetworkCFG.MK2 = 255;
  NetworkCFG.MK3 = 255;
  NetworkCFG.MK4 = 0;

  HWCFG.Bright = 90;
  HWCFG.VOL = 21;
  HWCFG.T1_ofs = 0;

  STATE.WiFiEnable = true;
}
