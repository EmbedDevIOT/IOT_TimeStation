#include "ClockController.h"
#include "FileConfig.h"

/******************************* DC - DC Step UP Initialisation ***************************************/
void StepUP_EN(bool state, uint8_t Voltage)
{
    // ESP_LOGI("StepUP_EN", "Function called with state: %d, Voltage: %d", state, Voltage);

    if (state == HIGH)
    {
        switch (Voltage)
        {
        case 12:
            STATE.DCEnable = true;
            pcf8574.digitalWrite(VDIV, LOW);
            // ESP_LOGI("StepUP_EN", "Setting Voltage to 12V");
            break;

        case 24:
            STATE.DCEnable = true;
            pcf8574.digitalWrite(VDIV, HIGH);
            // ESP_LOGI("StepUP_EN", "Setting Voltage to 24V");
            break;

        default:
            ESP_LOGW("StepUP_EN", "Invalid Voltage value: %d", Voltage);
            break;
        }
        pcf8574.digitalWrite(MAX_PWR, STATE.DCEnable);
        // ESP_LOGI("StepUP_EN", "DC-DC converter enabled");
    }
    else
    {
        STATE.DCEnable = false;
        digitalWrite(MAX_PWR, STATE.DCEnable);
        digitalWrite(VDIV, LOW);
        // ESP_LOGI("StepUP_EN", "DC-DC converter disabled");
    }
}
/*******************************************************************************************************/

/*******************************************************************************************************/
// Disable other DRV channels
void DisableAllDRVChannels()
{
    pcf8574.digitalWrite(DRVA_1, LOW);
    pcf8574.digitalWrite(DRVA_2, LOW);
    pcf8574.digitalWrite(DRVB_1, LOW);
    pcf8574.digitalWrite(DRVB_2, LOW);
}
/*******************************************************************************************************/
void WatchHandler()
{
    if (WatchClock.Hold_time > 0)
        WatchClock.Hold_time--;

    if (WatchClock.Start == START)
        ClockController((SystemClock.hour >= 12) ? (SystemClock.hour - 12) : SystemClock.hour, SystemClock.minute);

    else if (WatchClock.Start == HOME || (HWCFG.PwrState == 0 && (HWCFG.BatPercent < HWCFG.BAT_PROT)))
    {
        ClockController(0, 0);
        // debug("home position");
    }
    else
    {
        StepUP_EN(LOW, WatchClock.Volt);
        WatchClock.ClockState = 2; // Set status
    }
}

/*******************************************************************************************************/
// Accepted parametrs
// Hour and Minute - Time RTC (Primary Clock)
void ClockController(uint8_t Hour, uint8_t Minute)
{
    int TimeSC = WatchClock.Hour * 60 + WatchClock.Minute; // Check time sum secondary clock
    int Time = Hour * 60 + Minute;                         // Check time sum primary clock

    if ((TimeSC == Time) && (WatchClock.Hold_time == 0)) // Если время удержание полярности истекло то выключаем полярность
    {
        StepUP_EN(LOW, WatchClock.Volt); // DC - DC converter disable
        // SecondaryClock.ClockST = 1;
        WatchClock.Start == HOME ? WatchClock.ClockState = 0 : WatchClock.ClockState = 1;
    }

    if (((Time - TimeSC) == 1) || ((Time - TimeSC) == -719)) // The clock is slow by 1 minute
    {
        ClockPulse(Watch_ALL, 1, WatchClock.PulseNormal);
        WatchClock.Hour = Hour;
        WatchClock.Minute = Minute;
        WatchClock.Hold_time = 2;
        EEP_Write();

        ESP_LOGI("ClockController", "T < or = 1M");
    }
    else // The clock is more than 1 minute slow
    {

        if (((Time - TimeSC > 1) && (Time - TimeSC < 710)) || (TimeSC - Time > 10))
        {
            // SecondaryClock.ClockST = 0;
            ESP_LOGI("ClockController", "T > 1M");

            ClockPulse(Watch_ALL, 1, WatchClock.PulseFast);

            if (WatchClock.Minute + 1 == 60)
            {
                if (WatchClock.Hour + 1 == 12)
                {
                    WatchClock.Hour = 0;
                }
                else
                    WatchClock.Hour++;
                WatchClock.Minute = 0;
            }
            else
                WatchClock.Minute++;
            WatchClock.Hold_time = 1; // Задаем время удержания полярности
            EEP_Write();              // Сохраняем новое время вторичных часов
        }
    }
}
/*******************************************************************************************************/

/*******************************************************************************************************/
void ClockPulse(uint8_t clock, uint16_t step, uint32_t t_pulse)
{
    uint32_t now;

    // HWConfig.TCLpulse = 5;
    // FlagState.TimState = true; // State for start task (100ms)

    for (uint8_t i = 0; i < step; i++)
    {
        // Watch 1
        if (clock == Watch_1)
        {
            // Channel B Disable
            pcf8574.digitalWrite(DRVB_1, LOW);
            pcf8574.digitalWrite(DRVB_2, LOW);
            // Check Polarity and set next pulse polarity
            if (WatchClock.Polarity == 0)
            {
                WatchClock.Polarity = 1;
                pcf8574.digitalWrite(DRVA_1, HIGH);
                pcf8574.digitalWrite(DRVA_2, LOW);
                digitalWrite(LED_STG, HIGH);
                ESP_LOGI("ClockPulse", "CH A: Pulse: H");
            }
            else
            {
                WatchClock.Polarity = 0;
                pcf8574.digitalWrite(DRVA_1, LOW);
                pcf8574.digitalWrite(DRVA_2, HIGH);
                digitalWrite(LED_STR, HIGH);
                ESP_LOGI("ClockPulse", "CH A: Pulse: L");
            }
            // Set Polarity Indicator state 0
            digitalWrite(LED_CHA, LOW);
        }
        // Watch 2
        else if (clock == Watch_2)
        {
            // Channel A Disable
            pcf8574.digitalWrite(DRVA_1, LOW);
            pcf8574.digitalWrite(DRVA_2, LOW);

            if (WatchClock2.Polarity == 0)
            {
                WatchClock2.Polarity = 1;
                pcf8574.digitalWrite(DRVB_1, HIGH);
                pcf8574.digitalWrite(DRVB_2, LOW);
                digitalWrite(LED_STG, HIGH);

                ESP_LOGI("ClockPulse", "CH B: Pulse: H");
            }
            else
            {
                WatchClock2.Polarity = 0;
                pcf8574.digitalWrite(DRVB_1, LOW);
                pcf8574.digitalWrite(DRVB_2, HIGH);
                digitalWrite(LED_STR, HIGH);

                ESP_LOGI("ClockPulse", "CH B: Pulse: L");
            }
            // Set Polarity Indicator state 0
            digitalWrite(LED_CHB, LOW);
        }
        else if (clock == Watch_ALL)
        {
            if (WatchClock.Polarity == 0)
            {
                WatchClock.Polarity = 1;
                pcf8574.digitalWrite(DRVA_1, HIGH);
                pcf8574.digitalWrite(DRVA_2, LOW);
                digitalWrite(LED_CHA, LOW);

                ESP_LOGI("ClockPulse", "CH A: Pulse: H");
            }
            else
            {
                WatchClock.Polarity = 0;
                pcf8574.digitalWrite(DRVA_1, LOW);
                pcf8574.digitalWrite(DRVA_2, HIGH);
                digitalWrite(LED_CHA, LOW);

                ESP_LOGI("ClockPulse", "CH A: Pulse: L");
            }

            if (WatchClock2.Polarity == 0)
            {
                WatchClock2.Polarity = 1;
                pcf8574.digitalWrite(DRVB_1, HIGH);
                pcf8574.digitalWrite(DRVB_2, LOW);
                digitalWrite(LED_CHB, LOW);

                ESP_LOGI("ClockPulse", "CH B: Pulse: H");
            }
            else
            {
                WatchClock2.Polarity = 0;
                pcf8574.digitalWrite(DRVB_1, LOW);
                pcf8574.digitalWrite(DRVB_2, HIGH);
                digitalWrite(LED_CHB, LOW);

                ESP_LOGI("ClockPulse", "CH B: Pulse: L");
            }
        }
        StepUP_EN(HIGH, WatchClock.Volt); // DC/DC - ON and setup voltage

        vTaskDelay(t_pulse / portTICK_PERIOD_MS);

        digitalWrite(LED_CHB, HIGH);
        digitalWrite(LED_CHA, HIGH);
        digitalWrite(LED_STR, LOW);
        digitalWrite(LED_STG, LOW);

#ifdef CurPROT_OFF
        float_t temp = 0.0;

        for (uint8_t i = 0; i <= 20; i++)
        {
            temp = ACS.read_current();
            Serial.print("Temp:");
            Serial.println(temp);
        }

        HWConfig.Current = temp / 20;
        Serial.print("I:");
        Serial.println(HWConfig.Current);

        now = millis();

        while (millis() - now < t_pulse)
        {
            HWConfig.Current = ACS.read_current();
            // Serial.print("IWHILE:");
            // Serial.println(HWConfig.Current);

            if (CheckCurrent(HWConfig.Current))
            {
                FlagState.IDLE = false;
            }
        }
#endif
    }
    DisableAllDRVChannels();
    EEP_Write(); // Save polarity
    // wayting for next pulse 
    // vTaskDelay( WatchClock.PulsePause / portTICK_PERIOD_MS);
}
/*******************************************************************************************************/

/*******************************************************************************************************/
void SetHomePosition(uint8_t Hour, uint8_t Minute)
{
    int TimeSC = WatchClock.Hour * 60 + WatchClock.Minute; // Check time sum secondary clock
    int Time = 12;
    Serial.print("TimeSC:"); // Check time sum primary clock
    Serial.println(TimeSC);
    if ((TimeSC == Time) && (WatchClock.Hold_time == 0)) // Если время удержание полярности истекло то выключаем полярность
    {
        StepUP_EN(LOW, WatchClock.Volt); // DC - DC converter disable
        WatchClock.ClockState = 1;
    }

    if (((Time - TimeSC) == 1) || ((Time - TimeSC) == -719)) // The clock is slow by 1 minute
    {
        ClockPulse(Watch_1, 1, WatchClock.PulseNormal);
        Serial.print("point 1");

        WatchClock.Hour = Hour;
        WatchClock.Minute = Minute;
        WatchClock.Hold_time = 2;
        EEP_Write();
    }
    else // The clock is more than 1 minute slow
    {

        if (((Time - TimeSC > 1) && (Time - TimeSC < 710)) || (TimeSC - Time > 10))
        {
            WatchClock.ClockState = 0;
            ClockPulse(Watch_1, 1, WatchClock.PulseFast);
            Serial.print("point 2");

            if (WatchClock.Minute + 1 == 60)
            {
                if (WatchClock.Hour + 1 == 12)
                {
                    WatchClock.Hour = 0;
                }
                else
                    WatchClock.Hour++;
                WatchClock.Minute = 0;
            }
            else
                WatchClock.Minute++;
            WatchClock.Hold_time = 1; // Задаем время удержания полярности
            EEP_Write();
        }
    }
}
/*******************************************************************************************************/
/********************************** Check validation light intervall *****************************************/
byte CheckLightIntervall(byte TimeStartHour, byte TimeStartMinute, byte TimeFinishHour, byte TimeFinishMinute)
{
    int Time_Start = TimeStartHour * 60 + TimeStartMinute;
    int Time_Finish = TimeFinishHour * 60 + (TimeFinishMinute - 1);
    int Time = SystemClock.hour * 60 + SystemClock.minute;

    if (Time_Start < Time_Finish)
    {
        if ((Time_Start <= Time) && (Time <= Time_Finish))
            return 1;
        else
            return 0;
    }

    if (Time_Start > Time_Finish)
    {
        if (((Time_Start <= Time) && (Time <= 1439)) || ((0 <= Time) && (Time <= Time_Finish))) // 1439=23 * 60 + 59
            return 1;
        else
            return 0;
    }

    return 0;
}
/*******************************************************************************************************/

/********************************** Light Controll (ON / OFF) *****************************************/
void BacklightController()
{
    HWCFG.LedOnOFF = CheckLightIntervall(HWCFG.LedStartHour, HWCFG.LedStartMinute, HWCFG.LedFinishHour, HWCFG.LedFinishMinute);

    if ((HWCFG.LedOnOFF) || (HWCFG.LedON))
    {
        pcf8574.digitalWrite(LIGHT_EN, HIGH);
        // pcf8574.digitalWrite(HEAT_EN, HIGH);

        STATE.BrState = true;
    }
    else
    {
        pcf8574.digitalWrite(LIGHT_EN, LOW);
        // pcf8574.digitalWrite(HEAT_EN, LOW);

        STATE.BrState = false;
    }
}
/*******************************************************************************************************/
