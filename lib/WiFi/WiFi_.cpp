#include "Config.h"
#include "WF.h"
#include "esp_wifi.h"

///////////////////////////////////
//    Wi-Fi   Initialisation     //
///////////////////////////////////
void WIFIinit(boolean mode)
{
#ifdef DEBUG
   Serial.println(F("WIFI_Init..."));
#endif

   if (mode)
   {
      Serial.print("SSID=");
      Serial.print(NetworkCFG.Ssid.c_str());
      Serial.print("PASS=");
      Serial.println(NetworkCFG.Password.c_str());

      IPAddress ip(NetworkCFG.IP1, NetworkCFG.IP2, NetworkCFG.IP3, NetworkCFG.IP4); // Static IP
      IPAddress gateway(NetworkCFG.GW1, NetworkCFG.GW2, NetworkCFG.GW3, NetworkCFG.GW4);
      IPAddress subnet(NetworkCFG.MK1, NetworkCFG.MK2, NetworkCFG.MK3, NetworkCFG.MK4);

      WiFi.disconnect();
      WiFi.mode(WIFI_STA);

      byte tries = 6;

      WiFi.begin(NetworkCFG.Ssid.c_str(), NetworkCFG.Password.c_str());
      WiFi.config(ip, gateway, subnet);
      while (--tries && WiFi.status() != WL_CONNECTED)
      {
         Serial.print(".");
         delay(500);
      }
      if (WiFi.status() == WL_CONNECTED)
      {
         Serial.println("WiFi client");
         Serial.println("WiFi connected");
         Serial.println(WiFi.localIP());
      }
   }
   else
   {
      char tmpssid[15];
      char tmppass[15];

      NetworkCFG.APSSID.toCharArray(tmpssid, 15);
      strcat(tmpssid, "-");
      itoa(CFG.sn, tmpssid + strlen(tmpssid), DEC); // склейка Имени WiFi + SN

      NetworkCFG.APPAS.toCharArray(tmppass, 15);

      IPAddress apIP(192, 168, 1, 1);
      WiFi.disconnect();
      WiFi.mode(WIFI_AP);
      // WiFi.mode(WIFI_STA);
      esp_wifi_set_ps(WIFI_PS_NONE);

      WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
      WiFi.softAP(tmpssid, tmppass);
      Serial.println("WiFi: AP");

      Serial.println(tmpssid);
      Serial.println(tmppass);
   }
#ifdef DEBUG
   Serial.println(F("DONE"));
#endif
}

//////////////////////////////////////////////
//       Get signal level         WiFi      //
//////////////////////////////////////////////
int GetSignalLevel()
{
   if (WiFi.status() == WL_CONNECTED)
      return WiFi.RSSI();
   return -100;
}

//////////////////////////////////////////////
//       Рекконект по истечению времени     //
//////////////////////////////////////////////
void CheckWiFiStatus()
{
   static int DisconnectTime = 0;

   if ((NetworkCFG.WiFiMode == 0) && (WiFi.status() != WL_CONNECTED))
   {
      DisconnectTime++;
      if (DisconnectTime == 300) // 5 min
      {
         DisconnectTime = 0;
         digitalWrite(LED_STG, LOW);
         WIFIinit();
      }
   }
}

// Переписать под таску FreeRTOS
void WiFiHandler()
{
   static uint32_t tim1000 = 0;
   char msg[20];

   // Reset WiFi couter if Power ON and set Flags WiFi Enable ON
   if (HWCFG.PwrState == 1)
   {
      STATE.WiFiEnable = true;
      NetworkCFG.WiFiPeriod = millis();
      NetworkCFG.TimMin = 0;
      NetworkCFG.TimSec = 0;
   }

   // 1min =  1 * 60sec * 1000ms
   // will work in 10 min (WiFi swift in Disable mode)
   if (((long)millis() - NetworkCFG.WiFiPeriod > WiFiTimeON * 60 * 1000) && STATE.WiFiEnable && HWCFG.PwrState == 0)
   {
      NetworkCFG.WiFiPeriod = millis(); // Reset WiFi Timer Counter
      STATE.WiFiEnable = false;
      NetworkCFG.TimMin = 0;
      NetworkCFG.TimSec = 0;
      // sec = 0;
      // min = 0;
      Serial.println("WiFi_Disable");
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
   }

   if ((millis() - tim1000 > 1000) && STATE.WiFiEnable && HWCFG.PwrState == false)
   {
      tim1000 = millis();
      if (NetworkCFG.TimSec < 59)
      {
         NetworkCFG.TimSec++;
      }
      else
      {
         NetworkCFG.TimSec = 0;
         NetworkCFG.TimMin++;
      }
   }
}
