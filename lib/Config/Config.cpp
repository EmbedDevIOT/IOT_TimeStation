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
  STATE.IDLE = true;
  STATE.DynamicUPD = true;
  STATE.DUPDBlock = false;
  STATE.LedRS = false;
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
#ifndef DEBUG
  if (STATE.Debug)
  {
    char message[37];

    Serial.println(F("!!!!!!!!!!!!!!  DEBUG INFO  !!!!!!!!!!!!!!!!!!"));
    sprintf(message, "GMT: %d", HWCFG.GMT);
    Serial.println(message);
    sprintf(message, "PWR: %d Battery: %3d %", HWCFG.PwrState, HWCFG.BatValue);
    Serial.println(message);
    sprintf(message, "System Time: %02d:%02d:%02d", SystemClock.hour, SystemClock.minute, SystemClock.second);
    Serial.println(message);
    sprintf(message, "System Date: %4d.%02d.%02d", SystemClock.year, SystemClock.month, SystemClock.date);
    Serial.println(message);
    sprintf(message, "Watch Start: %d State: %d", WatchClock.Start, WatchClock.ClockST);
    Serial.println(message);
    sprintf(message, "Watch: %02d:%02d:%02d", WatchClock.Hour, WatchClock.Minute, WatchClock.Polarity);
    Serial.println(message);
    sprintf(message, "T1: %0.1f T1_OFS: %d", HWCFG.dsT1, HWCFG.T1_ofs);
    Serial.println(message);
    sprintf(message, "VOL: %d", HWCFG.VOL);
    Serial.println(message);
    // If Preparation to Time Synch - done -> PWR GPS -> ON
    if (HWCFG.GPSMode == GPS_ONCE)
    {
      sprintf(message, "GPS: PWR: %d, MODE: ONCE (to %02d:%02d)", HWCFG.GPSPWR, HWCFG.GPSStartHour, HWCFG.GPSStartMin);
    }
    else
    {
      sprintf(message, "GPS: PWR: %d, MODE: %d", HWCFG.GPSPWR, HWCFG.GPSMode);
    }
    Serial.printf("BTN Mode %1d \r\n", HWCFG.BtnMode);

    // If Power connectet to External VCC -> Show WiFi State
    // Else Show elasped time to WiFi OFF
    if (HWCFG.PwrState)
    {
      sprintf(message, "WiFi EN: %d", STATE.WiFiEnable);
    }
    else
    {
      sprintf(message, "WiFi EN: %d T:%02d:%02d", STATE.WiFiEnable, NetworkCFG.TimMin, NetworkCFG.TimSec);
    }

    Serial.println(message);
    Serial.printf("SN: %d \r\n", CFG.sn);

    Serial.println(F("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
    Serial.println();
  }
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
