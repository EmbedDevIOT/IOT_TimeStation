#ifndef _Config_H
#define _Config_H

#include <Arduino.h>
// #include "ClockController.h"
// #include "BatteryMonitor.h"
#include "esp_log.h"

#include <Audio.h>
#include "HardwareSerial.h"
#include <WiFi.h>
#include <WebServer.h>
#include "SPIFFS.h"
#include <microDS3231.h>
#include <ArduinoJson.h>
#include <EncButton.h>
#include <Wire.h>
#include "PCF8574.h"
#include <BatteryMonitor.h>
#include <INA226.h>
// #include <ElegantOTA.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#define ADR_RTC 0x68
#define ADR_EEPROM 0x57
#define ADR_INA226 0x40
#define ADR_IOEXP 0x27

#define UARTSpeed 115200
#define GPSSpeed 115200

#define WiFi_
#define MAX_MENU  12

#define WiFiTimeON 10
#define Client 0
#define AccessPoint 1
#define WorkNET

#define debug(x) Serial.println(x)
#define DEBUG
// #define I2C_SCAN

#define DISABLE 0
#define ENABLE 1

#define ON 1
#define OFF 0

#define USB_PWR 2 

// Prescaller for ADC
#define R1 130000
#define R2 100000

// Battery voltage limits
#define BATTERY_VOLTAGE_MIN 5.4
#define BATTERY_VOLTAGE_MAX 6.9

// Battery voltage divider resistors
#define VOLTAGE_DIVIDER_RATIO (230.0 / 100.0)  // (R1 + R2) / R2

//======================    G P I O        =====================
// OUT PINS 
#define LED_STG 14   // Green Led State 
#define LED_STR 12   // Red Led State
#define LED_CHA 19   // Led Channel A
#define LED_CHB 18   // Led Channel B

#define GPS_ON 23    // GPS ON/OFF
#define GPS_RST 13   // GPS Reset

// Expander pins
#define DRVA_1 P0    // CH_A Driver control (I2C)
#define DRVA_2 P1    // CH_A Driver control (I2C)
#define DRVB_1 P2    // CH_B Driver control (I2C)
#define DRVB_2 P3    // CH_B Driver control (I2C)
#define VDIV P4      // 12/24 Voltage control (I2C)
#define MAX_PWR P5   // MAX1771 Power control (I2C)
#define LIGHT_EN P6  // Backlighting control (I2C)
#define HEAT_EN P7   // Wire heat control (I2C)

// I2S
#define I2S_DOUT 27
#define I2S_BCLK 26
#define I2S_LRC  25

// INPUT Sensors
#define ALERT 32    // INA226 ALERT
#define BAT 35      // Battery Pin
#define L_SENS 34   // Light Sensor ADC
#define T1 33       // Temperature sensor ds18b20 
#define PWR_SEN 39  // Power Sensor (HIGH or LOW)

// INPUT Buttons
#define INT_BUT 0   // Internal Button
#define EXT_BUT 36  // External Button

// UART 2 - GPS -
#define GPS_SERIAL2
#ifdef GPS_SERIAL2
#define TX2_PIN 17 // UART2_TX
#define RX2_PIN 16 // UART2_RX
#endif
//=======================================================================

//=======================================================================
extern MicroDS3231 RTC;
extern DateTime SystemClock;
extern PCF8574 pcf8574;
//=======================================================================

//========================== ENUMERATION ================================
enum CHANNEL
{
  CH_A = 1,
  CH_B,
  CH_AB
};

enum ClockState
{
  STOP = 0,
  START,
  HOME
};

enum Watches
{
  Watch_1 = 1,
  Watch_2,
  Watch_ALL
};

// GPS Work Modes 
enum GPSmode
{
  GPS_OFF = 0,
  GPS_ONCE,
  GPS_ALWAYS
}; 

// MENU 
enum menu
{
  IDLE = 0,
  _CAR_NUM,
  _GMT,
  _MIN,
  _HOUR,
  _DAY,
  _MONTH,
  _YEAR,
  _BRIGHT,
  _VOL,
  _WCL,
  _WCSS,
  _WiFi
};

enum Clicks
{
  ONE = 1,
  TWO,
  THREE
};

enum DAY
{
  MON = 1,
  TUE,
  WED,
  THU,
  FRI,
  SAT,
  SUN
};

enum MONTH
{
  YAN = 1,
  FEB,
  MAR,
  APR,
  MAY,
  JUN,
  JUL,
  AUG,
  SEP,
  OCTB,
  NOV,
  DECM,
};


enum LIGHT_VALUE
{
    high_lev = 4095,
    low_lev = 670
};


//=======================================================================
/*
_____   ___    _________       _________  _                       ______    _          _    _                   
|_   _|.'   `. |  _   _  |     |  _   _  |(_)                    .' ____ \  / |_       / |_ (_)                  
 | | /  .-.  \|_/ | | \_|     |_/ | | \_|__   _ .--..--.  .---. | (___ \_|`| |-',--. `| |-'__   .--.   _ .--.   
 | | | |   | |    | |             | |   [  | [ `.-. .-. |/ /__\\ _.____`.  | | `'_\ : | | [  |/ .'`\ \[ `.-. |  
_| |_\  `-'  /   _| |_  _______  _| |_   | |  | | | | | || \__.,| \____) | | |,// | |,| |, | || \__. | | | | |  
|_____|`.___.'   |_____||_______||_____| [___][___||__||__]'.__.' \______.' \__/\'-;__/\__/[___]'.__.' [___||__] 
*/
// uint8_t asciiArt1[5][128] = {
//   {0x20,0x5F,0x5F,0x5F,0x5F,0x5F,0x20,0x20,0x20,0x5F,0x5F,0x5F,0x20,0x20,0x20,0x5F,0x5F,0x5F,0x5F,0x5F,0x5F,0x5F,0x5F,0x20,0x20,0x20,0x20,0x20,0x5F,0x5F,0x5F,0x5F,0x5F,0x5F,0x5F,0x5F,0x20,0x20,0x5F,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x5F,0x5F,0x5F,0x5F,0x5F,0x5F,0x20,0x20,0x20,0x20,0x5F,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x5F,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
//   {0x7C,0x5F,0x20,0x20,0x20,0x7C,0x2E,0x27,0x20,0x20,0x20,0x60,0x2E,0x20,0x7C,0x20,0x20,0x5F,0x20,0x20,0x20,0x5F,0x20,0x7C,0x20,0x20,0x20,0x20,0x7C,0x5F,0x2F,0x20,0x7C,0x20,0x7C,0x20,0x5C,0x5F,0x7C,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x2E,0x27,0x20,0x5F,0x5F,0x5F,0x5F,0x20,0x5C,0x20,0x20,0x2F,0x20,0x7C,0x5F,0x20,0x20,0x20,0x20,0x20,0x2F,0x20,0x7C,0x5F,0x20,0x28,0x5F,0x29,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
//   {0x7C,0x20,0x7C,0x20,0x2F,0x20,0x20,0x2E,0x2D,0x2E,0x20,0x20,0x5C,0x7C,0x5F,0x2F,0x20,0x7C,0x20,0x7C,0x20,0x5C,0x5F,0x7C,0x20,0x20,0x20,0x20,0x7C,0x5F,0x2F,0x20,0x7C,0x20,0x7C,0x20,0x5C,0x5F,0x7C,0x5F,0x5F,0x20,0x20,0x20,0x5F,0x20,0x2E,0x2D,0x2D,0x2E,0x2E,0x2D,0x2D,0x2E,0x20,0x7C,0x2F,0x20,0x2F,0x5F,0x5F,0x5C,0x5C,0x20,0x2E,0x5F,0x2E,0x5F,0x5F,0x5F,0x60,0x2E,0x20,0x20,0x7C,0x20,0x7C,0x20,0x60,0x27,0x5F,0x5C,0x20,0x3A,0x20,0x7C,0x20,0x7C,0x20,0x5B,0x20,0x20,0x7C,0x2F,0x20,0x2E,0x27,0x60,0x5C,0x5C,0x5B,0x20,0x60,0x2E,0x2D,0x2E,0x20,0x7C},
//   {0x7C,0x20,0x7C,0x20,0x7C,0x20,0x20,0x20,0x7C,0x20,0x7C,0x20,0x20,0x7C,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x7C,0x20,0x7C,0x20,0x20,0x20,0x5B,0x20,0x20,0x7C,0x20,0x5D,0x20,0x7C,0x20,0x7C,0x20,0x7C,0x20,0x7C,0x20,0x7C,0x20,0x7C,0x7C,0x20,0x5C,0x5F,0x5F,0x5F,0x5F,0x2E,0x2C,0x7C,0x20,0x7C,0x20,0x2F,0x2F,0x20,0x7C,0x20,0x7C,0x2C,0x20,0x7C,0x20,0x7C,0x2C,0x20,0x7C,0x20,0x7C,0x20,0x5B,0x20,0x20,0x7C,0x7C,0x20,0x5C,0x5F,0x5F,0x20,0x7C,0x20,0x7C,0x20,0x7C,0x20,0x7C,0x20,0x7C,0x20,0x7C,0x20,0x7C,0x20,0x7C},
//   {0x7C,0x5F,0x5F,0x5F,0x5F,0x5F,0x7C,0x60,0x2E,0x5F,0x5F,0x5F,0x2E,0x27,0x20,0x20,0x20,0x7C,0x5F,0x5F,0x5F,0x5F,0x5F,0x7C,0x7C,0x5F,0x5F,0x5F,0x5F,0x5F,0x5F,0x5F,0x5F,0x7C,0x7C,0x5F,0x5F,0x5F,0x5F,0x5F,0x7C,0x20,0x5B,0x5F,0x5F,0x5F,0x5D,0x5B,0x5F,0x5F,0x5F,0x5F,0x7C,0x7C,0x5F,0x5F,0x5D,0x27,0x2E,0x5F,0x5F,0x2E,0x27,0x20,0x5C,0x5F,0x5F,0x5F,0x5F,0x5F,0x2E,0x27,0x20,0x5C,0x5F,0x5F,0x2F,0x5C,0x27,0x2D,0x3B,0x5F,0x5F,0x2F,0x5C,0x5F,0x5F,0x2F,0x5B,0x5F,0x5F,0x5F,0x5D,0x27,0x2E,0x5F,0x5F,0x2E,0x27,0x20,0x5B,0x5F,0x5F,0x5F,0x5F,0x7C,0x7C,0x5F,0x5F,0x5D}
// };
//=========================== GLOBAL CONFIG =============================
struct GlobalConfig
{
  uint16_t sn = 0;
  String fw = "";                   // accepts from setup()
  String fwdate = "";
  String chipID = "";
  String MacAdr = "";

  char time[7] = "";
  char date[9] = "";

};
extern GlobalConfig CFG;
//=======================================================================
struct NetworkConfig 
{
  uint8_t WiFiMode = AccessPoint;    //  WiFi Mode
  uint8_t TimMin = 0;
  uint8_t TimSec = 0;
  long WiFiPeriod = 0;

  String MacAdr = "";
  String NTPServer = "pool.ntp.org";  // niddet url NTP Server (Responce from HTTP websocket)

  String APSSID = "GKTime";           // Access Point ID
  String APPAS = "gktime123";            // Access Point PSW

  String Ssid = "EMBNET2G";           // Client SSID Wifi network
  String Password = "Ae19co90$!eT";   // Client Passwords WiFi network

  byte IP1 = 192;
  byte IP2 = 168;
  byte IP3 = 1;
  byte IP4 = 1;

  byte GW1 = 192;
  byte GW2 = 168;
  byte GW3 = 1;
  byte GW4 = 1;

  byte MK1 = 255;
  byte MK2 = 255;
  byte MK3 = 255;
  byte MK4 = 0;

};
extern NetworkConfig NetworkCFG;

//=======================================================================
struct HardwareConfig
{
  int GMT = 0;
  uint8_t VOL = 21;           // Volume  1...21
  int8_t Bright = 70;         // Led Brightness 
  float dsT1 = 0.0;           // Temperature T1 
  int8_t T1_ofs = 0;          // Temperature Offset T1 sensor
  int8_t RTCtemp = 0;         // Temperature RTC
  int L_Lim = 1000;           // Light sensor enable limit (Light_ON)
  int i_sens = 0;             // Current sensor sensetivity 
  int I_PROT = 200;           // Current Protect
  float BatVoltage  = 0;      // Battery Value (Voltage)
  uint8_t BatPercent  = 0;    // Battery Value (Percent) 
  float voltage = 0;          // Voltage value from INA226
  float current = 0;          // Current value from INA226 
  float power = 0;            // Power value from INA226

  int BAT_PROT = 40;          // Minimum Battery Limit in percent (go to sleep) 
  // Time Backlight ON / OFF 
  uint8_t LedStartHour = 16;     
  uint8_t LedStartMinute = 00;
  uint8_t LedFinishHour = 17;
  uint8_t LedFinishMinute = 00;
  // Time Enable GSP Synhronization 
  uint8_t GPSStartHour = 14;
  uint8_t GPSStartMin = 00;
  uint8_t GPSStartSec = 00;
  // Flags 
  uint8_t PwrState = 0;       // Curren Power State (DC power/Battery)
  uint8_t LedOnOFF = 1;       // Флаг старта работы подсветки
  uint8_t LedON = 0;          // Remote Control 
  uint8_t GPSSynh = 1;        // ????
  bool GPSPWR = true;         // Power GPS Module (State:  ON or OFF)
  // Mode 
  uint8_t BtnMode = CH_AB;        // Button Mode (Action channel selection "A", "B" or "A + B")
  uint8_t GPSMode = 1;

  uint8_t ERRORcnt = 0;
};
extern HardwareConfig HWCFG;
//=======================================================================

//=======================================================================
// Mechanical Watch
struct MechanicalClock
{
  byte Hour = 0;
  byte Minute = 0;
  byte Second = 0;
  byte Hold_time = 0;
  byte Polarity = 0;
  byte Start = 0;
  byte ClockState = 2;
  byte Volt = 12;
  int PulseNormal = 700;
  int PulseFast = 300;
  int PulsePause = 100;
};
extern MechanicalClock WatchClock;
extern MechanicalClock WatchClock2;

//=======================================================================
struct GPS
{
  bool Fix = false;

  byte tValid = 0;
  uint32_t tAge = 0;
  byte dValid = 0;
  uint32_t dAge = 0;
  byte SValid = 0;
  byte Sattelite = 0;
  byte TimeZone = 0;
  byte tUpdate = 0;
  byte dUpdate = 0;
  byte LocalHour = 0;
  byte Hour = 0;
  byte Minute = 0;
  byte Second = 0;
  byte Day = 0;
  byte Month = 0;
  uint16_t Year = 0;
};
extern GPS GPST;
//=======================================================================

//=======================================================================
struct Flag
{
  bool Start = 0;
  bool Watch1_EN = 0;
  bool I2C_Block = 0;
  uint8_t cnt_Supd = 0;
  bool IDLE : 1;
  bool LedWiFi : 1;
  bool SaveFlash : 1;
  bool Debug : 1;
  bool CurDebug : 1;
  bool WiFiEnable : 1;  
  bool TTS : 1;        // Time to speech
  bool VolumeUPD : 1;  // Volume update
  uint8_t menu_tmr = 0;  
  byte DCEnable = 0;
  bool BrState = 0;
};
extern Flag STATE;
//============================================================================
//============================================================================
bool GetWCState(uint8_t num);
boolean SerialNumConfig(void);
void UserPresetInit(void);
void SystemStateInit(void);     //  System Initialisation (variables and structure)
void GPIOInit(void);            // GPIO Initializing 
bool RTCInit(void);
void I2C_Scanning(void);
void ShowInfoDevice(void);      // Show information or this Device
void GetChipID(void);
String GetMacAdr();
void DebugInfo(void);
void SystemFactoryReset(void);
void ShowFlashSave(void);
//============================================================================
#endif // _Config_H