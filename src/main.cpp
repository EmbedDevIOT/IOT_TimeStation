#include "Config.h"

#include "SysHandler.h"
#include "ClockController.h"
#include "FileConfig.h"
#include "WF.h"
#include "HTTP.h"
#include "esp_log.h"

// #define DEBUG // Debug Mode ON
//======================================================================

//=========================== GLOBAL VARIABLES =========================
uint8_t sec = 0;
static const char *TAG = "APP";
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
PCF8574 pcf8574(ADR_IOEXP);

Button btnINT(INT_BUT, INPUT, LOW);
Button btnEXT(EXT_BUT, INPUT_PULLUP, LOW);
//=======================================================================

//============================== STRUCTURES =============================
DateTime SystemClock;        // RTC System Clock
MechanicalClock WatchClock;  // Mechanical Watch
MechanicalClock WatchClock2; // Mechanical Watch 2
NetworkConfig NetworkCFG;    // General Network Config
GlobalConfig CFG;            // General Global Config (Remove) ???
HardwareConfig HWCFG;        // hardware config
Flag STATE;                  // States FLag
//=======================================================================

//=========================== GLOBAL VARIABLES =========================
char message[32] = {0}; // buff for send message
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

//======================   System Initialization   ======================
void SystemGeneralInit()
{
    bool ERR_RTC, ERR_SPIFFS;

#ifdef DEBUG
    Serial.println(F("System checking"));
    Serial.println(F("UART...init"));
    Serial.println(F("GPS_UART...init"));
#endif

    EEP_Read(); // Reading data from EEPROM

    /*
    Watch State Init:
    -> Validation EEPROM Data
    -> if BuildShit Data -> Set Default Preset and Save config to EEPROM
    */
    if ((WatchClock.Hour > 11) || (WatchClock.Minute > 59) || (WatchClock.Polarity > 1) || (NetworkCFG.WiFiMode > 1))
    {
        Serial.println(F("BuildShit EEPROM data -> setup default preset"));
        WatchClock.Hour = 0;
        WatchClock.Minute = 0;
        WatchClock.Polarity = 0;
        NetworkCFG.WiFiMode = Client;
        EEP_Write();
        WatchClock.Start = STOP;
    }

    // System Structures Init
    SystemStateInit();

    // Инициализация PCF8574
    // Wire.beginTransmission(ADR_IOEXP);
    // Wire.write(0X00);
    // Wire.endTransmission();

// #ifdef DEBUG
//     I2C_Scanning();
// #endif
    // GPIO Init
    GPIOInit();

    // RTC init
    ERR_RTC = RTCInit();
    // I2C Expander
    PCF8574_InitPins();

// Temperature Sensor
#ifdef DEBUG
    Serial.print(F("Temp Sensor_init..."));
#endif
    ds18b20.begin();
    vTaskDelay(20 / portTICK_PERIOD_MS);
    GetDSData();
#ifdef DEBUG
    Serial.println(F("DONE"));
#endif

    // GPS Module
    // #ifdef DEBUG
    //     Serial.print(F("GPS_State_Init..."));
    // #endif
    // Serial2.begin(GPSSpeed, SERIAL_8N1, RX2_PIN, TX2_PIN);
    // vTaskDelay(20 / portTICK_PERIOD_MS);
    // GPS_init(HIGH);
    // ResetGPSData();
    // #ifdef DEBUG
    //     Serial.println(F("DONE"));
    // #endif

    // BAttery ADC
    //   VBAT.attach(BAT_CONT);
    // #ifdef DEBUG
    //     Serial.println(F("ADC...init"));
    // #endif

    // if (((HWConfig.PwrState == 0) || (HWConfig.PwrState == 2)) && (HWConfig.BTVoltPercent < HWConfig.MinBatLimit))
    // {
    //   ShowErrorState(BAT_MIN_VOLTAGE);
    // }

// SPIFFS
#ifdef DEBUG
    Serial.print(F("SPIFFS_Init..."));
#endif
    ERR_SPIFFS = SPIFFS.begin(true);
    if (!ERR_SPIFFS)
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
#ifdef DEBUG
    Serial.println(F("DONE"));
#endif

    LoadConfig(); // Load configuration from config.json files

#ifdef DEBUG
    Serial.println(F("######################  System Configuration  ######################"));
    Serial.printf("####  1. WiFi Mode: %d", NetworkCFG.WiFiMode);
    Serial.println();
    sprintf(message, "####  2. IP: %00d.%00d.%00d.%00d", NetworkCFG.IP1, NetworkCFG.IP2, NetworkCFG.IP3, NetworkCFG.IP4);
    Serial.println(F(message));
    sprintf(message, "####  3. Gate: %00d.%00d.%00d.%00d", NetworkCFG.GW1, NetworkCFG.GW2, NetworkCFG.GW3, NetworkCFG.GW4);
    Serial.println(F(message));
    sprintf(message, "####  4. Mask: %00d.%00d.%00d.%00d", NetworkCFG.MK1, NetworkCFG.MK2, NetworkCFG.MK3, NetworkCFG.MK4);
    Serial.println(F(message));
    Serial.printf("####  5. Sensor Limit: %000d", HWCFG.L_Lim);
    Serial.println();
    Serial.printf("####  7. GPS Mode: %d", HWCFG.GPSMode);
    Serial.println();
    sprintf(message, "####  8. DataRTC: %0002d-%02d-%02d", SystemClock.year, SystemClock.month, SystemClock.date);
    Serial.println(F(message));
    sprintf(message, "####  9. TimeRTC: %02d:%02d:%02d", SystemClock.hour, SystemClock.minute, SystemClock.second);
    Serial.println(F(message));
    sprintf(message, "####  10. TimeSecondaryClock_1: %02d:%02d:%02d", WatchClock.Hour, WatchClock.Minute, WatchClock.Second);
    Serial.println(F(message));
    //   sprintf(message, "####  11. TimeSecondaryClock_2: %02d:%02d:%02d", WatchClock.Hour, SecondaryClock2.Minute, SecondaryClock2.Second);
    //   Serial.println(F(message));
    Serial.printf("####  12. Clock1_Start: %d", WatchClock.Start);
    Serial.println();
    Serial.printf("####  13. Clock1_Polarity: %d", WatchClock.Polarity);
    Serial.println();
    sprintf(message, "####  14. BackLight Start: %02d:%02d", HWCFG.LedStartHour, HWCFG.LedStartMinute);
    Serial.println(F(message));
    sprintf(message, "####  15. BackLight Stop: %02d:%02d", HWCFG.LedFinishHour, HWCFG.LedFinishMinute);
    Serial.println(F(message));
    Serial.printf("####  16. BackLight: %d", HWCFG.LedOnOFF);
    Serial.println();
    sprintf(message, "####  17. CL_Volt1: %02d V", WatchClock.Volt);
    Serial.println(F(message));
    sprintf(message, "####  18. TimSGPS: %02d:%02d:%02d", HWCFG.GPSStartHour, HWCFG.GPSStartMin, HWCFG.GPSStartSec);
    Serial.println(F(message));
    Serial.printf("####  19. Time Zone: %d", HWCFG.GMT);
    Serial.println();
    Serial.printf("####  20. PulseNORMAL: %000d ms", WatchClock.PulseNormal);
    Serial.println();
    Serial.printf("####  21. PulseFAST: %000d ms", WatchClock.PulseFast);
    Serial.println();
    Serial.printf("####  22. Current_Protect: %2d mA", HWCFG.I_PROT);
    Serial.println();
    Serial.printf("####  23. Min_Bat_Voltage: %3d %", HWCFG.BAT_PROT);
    Serial.println();
    Serial.printf("####  24. BTNMode: %d", HWCFG.BtnMode);
    Serial.println();
    Serial.printf("####  25. GPSSynh: %d", HWCFG.GPSSynh);
    Serial.println();
    sprintf(message, "####  26. GPS_Synh_Start: %02d:%02d", HWCFG.GPSStartHour, HWCFG.GPSStartMin);
    Serial.println(F(message));
    Serial.println(F("#####################################################################"));
#endif

// Current Sensor INA226
#ifdef DEBUG
    Serial.println(F("CurrentSensor_INA..."));
#endif
// !!!!!! Place init code
#ifdef DEBUG
    Serial.println(F("DONE"));
#endif

// DAC Amplifier
#ifdef DEBUG
    Serial.println(F("Amplifier..."));
#endif
    Amplifier.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    Amplifier.setVolume(HWCFG.VOL);
#ifdef DEBUG
    Serial.println(F("DONE"));
#endif

// Button Set to Default Preset
#ifdef DEBUG
    Serial.print(F("Waiting_ButtonHold_to_Default"));
#endif
//   while (task_counter > 0)
//   {
//     ButtonSetupHandler(); // First configuration preset
//     task_counter <= 50 ? task_counter++ : task_counter = 0;
//     delay(100);
// Serial.print(".");
//   }
#ifdef DEBUG
    Serial.println(F("Timeout"));
#endif
    STATE.WiFiEnable = true;
    WIFIinit(NetworkCFG.WiFiMode); // Initialisation  Wifi
    vTaskDelay(500 / portTICK_PERIOD_MS);
    HTTPinit(); // HTTP server initialisation

    if ((ERR_SPIFFS == false) || (ERR_RTC == false)) // Check File system and time error
        WatchClock.Start = STOP;                     // Set flag stop
    vTaskDelay(10 / portTICK_PERIOD_MS);

    ShowInfoDevice();

    NetworkCFG.WiFiPeriod = millis(); // "сбросить" таймер
}
//=======================================================================

//=======================       S E T U P       =========================
void setup()
{
    // // Установить уровень логирования для всех компонентов на INFO
    esp_log_level_set("*", ESP_LOG_INFO);
    // // Установить уровень логирования для конкретного компонента (например, "ButtonHandler")
    // esp_log_level_set("ButtonHandler", ESP_LOG_INFO);

    CFG.fw = "0.1.0";
    CFG.fwdate = "13.04.2024";

    Serial.begin(UARTSpeed);
    
    SystemGeneralInit();

    // if (SerialNumConfig())
    // {
    //     SaveConfig();
    // }

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
        HandleClient();
    }
    ButtonHandler();
}
//=======================================================================

//======================    R T O S  T A S K    =========================
// Core 0. 10ms (I2S Audio Handler | MenuTimer_Exit
void HandlerCore0(void *pvParameters)
{
    ESP_LOGI(__func__, "Task: I2S Handler | MenuTimer Exit");
    ESP_LOGI(__func__, "T:10ms Stack:10000 Core: %d", xPortGetCoreID());
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
        SystemClock = RTC.getTime();
        // xSemaphoreGive(i2c_mutex);

        BacklightController();

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
        DebugInfo();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
//=======================================================================
//=========================================================================
// Get Data from DS18B20 Sensor
void GetDSData()
{
    ds18b20.requestTemperatures();
    HWCFG.dsT1 = ds18b20.getTempCByIndex(0);
    HWCFG.dsT1 = HWCFG.dsT1 + HWCFG.T1_ofs;
}
//=========================================================================
//=================== Keyboard buttons Handler =============================
void ButtonHandler()
{
    static uint8_t step_count = 0;

    // xSemaphoreTake(i2c_mutex, portMAX_DELAY);
    // STATE.I2C_Block = true;

    btnINT.tick();
    btnEXT.tick();

    // Click Button 1 Handling (+)
    if (btnINT.click())
    {
        ESP_LOGI("ButtonHandler", "BTN 1 Click");
    }

    while (btnEXT.busy())
    {
        btnEXT.tick();
        // One Click (1 Min Update Normal Pulse)
        if (btnEXT.hasClicks(ONE))
        {
            Serial.println("ONE MIN PULSE");
            switch (HWCFG.BtnMode)
            {
            case CH_A:
                Serial.println("CH1");
                ClockPulse(Watch_1, 1, WatchClock.PulseNormal);
                break;
            case CH_B:
                Serial.println("CH2");
                ClockPulse(Watch_2, 1, WatchClock.PulseNormal);
                break;
            case CH_AB:
                Serial.println("CH1 + CH2");
                ClockPulse(Watch_ALL, 1, WatchClock.PulseNormal);
                break;
            default:
                break;
            }
        }

        // #### Double Clicks (HOUR Update) ####
        if (btnEXT.hasClicks(TWO))
        {
            Serial.println("ONE HOUR PULSE");
            step_count = 0;
            // FlagState.LedG = 1;
            // FlagState.LedR = 0;
            switch (HWCFG.BtnMode)
            {
            case 1:
                Serial.println("CH1");
                ClockPulse(Watch_1, 60, WatchClock.PulseFast);
                break;
            case 2:
                Serial.println("CH2");
                ClockPulse(Watch_2, 60, WatchClock.PulseFast);
                break;
            case 3:
                Serial.println("CH1 + CH2");
                ClockPulse(Watch_ALL, 60, WatchClock.PulseFast);
                break;
            default:
                break;
            }
        }
        // #### Triple Clicks (Channel Select) ####
        if (btnEXT.hasClicks(THREE))
        {
            Serial.print("CHANNEL SELECT:");
            if (HWCFG.BtnMode == CH_AB)
            {
                HWCFG.BtnMode = CH_A;
                Serial.println("A");
                digitalWrite(LED_CHA, !HIGH);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                digitalWrite(LED_CHA, !LOW);
            }
            else if (HWCFG.BtnMode == CH_A)
            {
                HWCFG.BtnMode = CH_B;
                Serial.println("B");
                digitalWrite(LED_CHB, !HIGH);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                digitalWrite(LED_CHB, !LOW);
            }
            else if (HWCFG.BtnMode == CH_B)
            {
                HWCFG.BtnMode = CH_AB;
                Serial.println("A + B");
                digitalWrite(LED_CHA, !HIGH);
                digitalWrite(LED_CHB, !HIGH);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                digitalWrite(LED_CHA, !LOW);
                digitalWrite(LED_CHB, !LOW);
            }
            Serial.println("Save preset");
            SaveConfig();
        }
    }
    // // Click Button 2 Handling (-)
    // if (btn2.click())
    // {
    //     Serial.printf(" BTN 2 Click \r\n");
    // }

    // Holding Button 3 Handling (+)
    // if (btn2.getSteps() == 50)
    // {
    //     Serial.printf(" BTN 2 STEP 50 \r\n");

    //     uint32_t now = millis();
    //     while (millis() - now < 3500)
    //     {
    //         vTaskDelay(1000 / portTICK_PERIOD_MS);
    //     }
    //     Serial.println("#### FACTORY RESET ####");
    //     // SystemFactoryReset();
    //     // SaveConfig();
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    //     Serial.println("#### SAVE DONE ####");
    //     ESP.restart();
    // }

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
