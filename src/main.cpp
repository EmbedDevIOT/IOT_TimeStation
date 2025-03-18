#include "Config.h"

#include "FileConfig.h"
#include "WF.h"
#include "HTTP.h"

#define DEBUG // Debug Mode ON
//======================================================================

//=========================== GLOBAL VARIABLES =========================
uint8_t sec = 0;
//======================================================================

//================================ OBJECTs =============================
// FreeRTOS Tasks
TaskHandle_t TaskCore0_Gen;
TaskHandle_t TaskCore1_Gen;
TaskHandle_t TaskCore0_TSH;
TaskHandle_t TaskCore1_1000ms;

SemaphoreHandle_t i2c_mutex;


Audio Amplifier;
MicroDS3231 RTC;

OneWire oneWire1(T1);
DallasTemperature ds18b20(&oneWire1);


Button btn1(INT_BUT, INPUT, LOW);
Button btn2(EXT_BUT, INPUT, LOW);
//=======================================================================

//============================== STRUCTURES =============================
DateTime Clock;
GlobalConfig CFG;
HardwareConfig HWCFG;
Flag STATE;
//=======================================================================

//=========================== GLOBAL VARIABLES =========================
uint8_t sec_cnt = 0;
//======================================================================

//======================    FUNCTION PROTOTYPS     ======================
// - FreeRTOS function --
void HandlerCore0(void *pvParameters);
void HandlerCore1(void *pvParameters);
void HandlerTask1000(void *pvParameters);
void HandlerTask1Wire(void *pvParameters);
// Other Function
void GetDSData(void);
void ButtonHandler(void);
void UART_Recieve_Data(void);
//=======================================================================
//=======================       S E T U P       =========================
void setup()
{
    CFG.fw = "0.0.3";
    CFG.fwdate = "18.03.2024";

    Serial.begin(UARTSpeed);
   
    // UART GPS Module 
    // Serial2.begin(GPSSpeed, SERIAL_8N1, RX2_PIN, TX2_PIN);
    // vTaskDelay(20 / portTICK_PERIOD_MS);

    // System Structures Init
    SystemInit();

    // GPIO pin init
    pinMode(LED_STG, OUTPUT);
    digitalWrite(LED_STG, LOW);
    pinMode(LED_STR, OUTPUT);
    digitalWrite(LED_STR, LOW);
    pinMode(LED_CHA, OUTPUT);
    digitalWrite(LED_CHA, HIGH);
    pinMode(LED_CHB, OUTPUT);
    digitalWrite(LED_CHB, HIGH);
  
    // RTC init
    RTC.begin();
    if (RTC.lostPower())
    {
        RTC.setTime(COMPILE_TIME);
    }
    Clock = RTC.getTime();
    Serial.println(F("RTC...Done"));

    // SPIFFS
    // if (!SPIFFS.begin(true))
    // {
    //     Serial.println("An Error has occurred while mounting SPIFFS");
    //     return;
    // }

    ds18b20.begin();
    Serial.println(F(" Temp Sensor ...Done"));
    vTaskDelay(20 / portTICK_PERIOD_MS);
  
    GetDSData();
   
    I2C_Scanning();
    vTaskDelay(500 / portTICK_PERIOD_MS);


    // LoadConfig();         // Load configuration from config.json files
    // ShowLoadJSONConfig(); // Show load configuration

    // if (SerialNumConfig())
    // {
    //     SaveConfig();
    // }

    // CFG.WiFiMode = 0;
    Serial.println("Wifi Enable");
    // WIFIinit();
    WiFi.mode(WIFI_STA);
    WiFi.begin(CFG.Ssid.c_str(), CFG.Password.c_str());
  
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".");
      delay(100);
    }
  
    Serial.printf_P(PSTR("Connected\r\nRSSI: "));
    Serial.print(WiFi.RSSI());
    Serial.print(" IP: ");
    Serial.println(WiFi.localIP());

    vTaskDelay(500 / portTICK_PERIOD_MS);

    // HTTPinit(); // HTTP server initialisation
    vTaskDelay(500 / portTICK_PERIOD_MS);


    Amplifier.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);

    Amplifier.setVolume(HWCFG.VOL);
    Serial.println(F("DAC PCM Amplifier...Done"));

    // Task: I2S Audio Handler | MenuTimer Exit
    xTaskCreatePinnedToCore(
        HandlerCore0,
        "TaskCore0_Gen",
        10000,
        NULL,
        1,
        &TaskCore0_Gen,
        0);
    vTaskDelay(500 / portTICK_PERIOD_MS);

    // Temperature Sensor Task Handler
    xTaskCreatePinnedToCore(
        HandlerTask1Wire,
        "TaskCore0_TSH",
        2048,
        NULL,
        1,
        &TaskCore0_TSH,
        0);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  

    // Core 1. 1000ms (Read RTC / Debug Info / menu timer)
    xTaskCreatePinnedToCore(
        HandlerCore1,
        "TaskCore1_Gen",
        2048,
        NULL,
        1,
        &TaskCore1_Gen,
        1);
    vTaskDelay(500 / portTICK_PERIOD_MS);

  
    xTaskCreatePinnedToCore(
        HandlerTask1000,
        "TaskCore1_1000ms",
        2048, // 16384
        NULL,
        1,
        &TaskCore1_1000ms,
        1);
    vTaskDelay(500 / portTICK_PERIOD_MS);

}
//=======================================================================

//=======================        L O O P        =========================
void loop()
{
    // HTTP Handling WiFi = ON
    if ((STATE.WiFiEnable))
    {
        // HandleClient();
    }
    ButtonHandler();
}
//=======================================================================

//======================    R T O S  T A S K    =========================
// Core 0. 10ms (I2S Audio Handler | MenuTimer_Exit
void HandlerCore0(void *pvParameters)
{
    Serial.print("Task: I2S Handler | MenuTimer Exit \r\n");
    Serial.print("T:10ms Stack:10000 Core:");
    Serial.println(xPortGetCoreID());
    for (;;)
    {
        Amplifier.loop();
        UART_Recieve_Data();

        if (STATE.TTS)
        {
            // Tell_me_CurrentTime();
            // Tell_me_CurrentData();
            STATE.TTS = false;
        }


        if (STATE.VolumeUPD)
        {
            Amplifier.setVolume(HWCFG.VOL);
            STATE.VolumeUPD = false;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// Core 0. Temperature Sensor Handler
void HandlerTask1Wire(void *pvParameters)
{
    Serial.print("Task: Temperature Sensor Handler \r\n");
    Serial.print("T:10s Stack:2048 Core:");
    Serial.println(xPortGetCoreID());
    for (;;)
    {
        if (sec < 10)
        {
            sec++;

            if ((STATE.WiFiEnable) && (sec % 2 == 0))
            {
                digitalWrite(LED_STG, HIGH);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                digitalWrite(LED_STG, LOW);
            }
        }
        else
        {
            GetDSData();
            sec = 1;
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// Core 1. 1000ms (Read RTC / Debug Info / menu timer)
void HandlerCore1(void *pvParameters)
{
    Serial.print("Task: Read RTC / Debug Info / menu timer \r\n");
    Serial.print("T:1000ms Stack:2048 Core:");
    Serial.println(xPortGetCoreID());
    for (;;)
    {
        // xSemaphoreTake(i2c_mutex, portMAX_DELAY);
        Clock = RTC.getTime();
        // xSemaphoreGive(i2c_mutex);

        DebugInfo();

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// Core 1. 1000ms
void HandlerTask1000(void *pvParameters)
{
    Serial2.print("\r\n\r\n"); // Clear RS485
    Serial.print("Task: Sending RS485 data \r\n");
    Serial.print("T:1000ms Stack:12000 Core:");
    Serial.println(xPortGetCoreID());

    for (;;)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
//=======================================================================
//=========================================================================
// Get Data from DS18B20 Sensor
void GetDSData()
{
    ds18b20.requestTemperatures();
    HWCFG.dsT1 = ds18b20.getTempCByIndex(0);
    HWCFG.dsT1 = HWCFG.dsT1 + HWCFG.T1_off;
}
//=========================================================================
//=================== Keyboard buttons Handler =============================
void ButtonHandler()
{
        // xSemaphoreTake(i2c_mutex, portMAX_DELAY);
        STATE.I2C_Block = true;

        btn1.tick();
        btn2.tick();

        // Click Button 1 Handling (+)
        if (btn1.click())
        {
            Serial.printf("BTN 1 Click \r\n");

        }

        // Click Button 2 Handling (-)
        if (btn2.click())
        {
            Serial.printf(" BTN 2 Click \r\n");
        }

        // Holding Button 3 Handling (+)
        if (btn2.getSteps() == 50)
        {
            Serial.printf(" BTN 3 STEP 50 \r\n");

            uint32_t now = millis();
            while (millis() - now < 3500)
            {
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
            Serial.println("#### FACTORY RESET ####");
            // SystemFactoryReset();
            // SaveConfig();
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            Serial.println("#### SAVE DONE ####");
            ESP.restart();
        }

            // // WiFI ON / OFF
            // case _WiFi:
            //     if (STATE.WiFiEnable)
            //     {
            //         STATE.WiFiEnable = false;
            //         Serial.println("WiFi_Disable");
            //         memset(name_2, 0, 25);
            //         sprintf(name_2, "ОТКЛ");
            //         Send_BS_UserData(name_1, name_2);
            //         WiFi.disconnect(true);
            //         WiFi.mode(WIFI_OFF);
            //     }
            //     else
            //     {
            //         STATE.WiFiEnable = true;
            //         Serial.println("WiFi_Enable");
            //         memset(name_2, 0, 25);
            //         sprintf(name_2, "ВКЛ");
            //         Send_BS_UserData(name_1, name_2);
            //         WIFIinit(AccessPoint);
            //         vTaskDelay(500 / portTICK_PERIOD_MS);
            //         HTTPinit(); // HTTP server initialisation
            //     }
            //     SaveConfig();

}
//=========================================================================
//=========================================================================
void UART_Recieve_Data()
{
    if (Serial.available())
    {
        Serial.println("1");
        // digitalWrite(LED_ST, HIGH);

        // put streamURL in serial monitor
        // audio.stopSong();
        String r = Serial.readString();
        bool block_st = false;
        r.trim();
        if (r.length() > 3)
        {
            Amplifier.connecttohost(r.c_str());
            // if (r == "time")
            // {
            //     Serial.println("Current Time");
            //     Tell_me_CurrentTime();
            // }
            // if (r == "door1")
            // {
            //     Serial.println("DoorState");
            //     Tell_me_DoorState(true);
            // }
            // if (r == "door0")
            // {
            //     Serial.println("DoorState");
            //     Tell_me_DoorState(false);
            // }
            // if (r == "date")
            // {
            //     Serial.println("Current Date");
            //     Tell_me_CurrentData();
            // }
            // if (r == "tellmetime")
            // {
            //     Serial.println("Current Time and Date");
            //     Tell_me_CurrentTime();
            //     Tell_me_CurrentData();
            // }
        }
        else
        {
            // Amplifier.setVolume(r.toInt());
            HWCFG.VOL = r.toInt();
            STATE.VolumeUPD = true;
        }
        log_i("free heap=%i", ESP.getFreeHeap());
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
//=========================================================================