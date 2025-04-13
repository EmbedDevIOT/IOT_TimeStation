#include "SysHandler.h"
#include "ClockController.h"

uint8_t expander_state = 0xFF; // Все пины изначально выключены (логика PCF8574: 0-активен, 1-выключен)
//=========================================================================

void processCommand(String cmd)
{
    Serial.println(cmd);

    cmd.toLowerCase();
    int space1 = cmd.indexOf(' ');
    String action = cmd.substring(0, space1);

    if (action == "set" && cmd.length() > 6)
    {
        int space2 = cmd.indexOf(' ', space1 + 1);
        int pin = cmd.substring(space1 + 1, space2).toInt();
        int state = cmd.substring(space2 + 1).toInt();

        if (pin < 0 || pin > 7)
        {
            Serial.println("ERROR: Invalid pin (0-7)");
            return;
        }

        // Обновляем состояние
        if (state)
        {
            expander_state &= ~(1 << pin); // Установить бит в 0 (активировать пин)
        }
        else
        {
            expander_state |= (1 << pin); // Установить бит в 1 (деактивировать пин)
        }

        // Отправляем на PCF8574
        Wire.beginTransmission(ADR_IOEXP);
        Wire.write(expander_state);
        Wire.endTransmission();

        Serial.print("Pin ");
        Serial.print(pin);
        Serial.print(" set to ");
        Serial.println(state);
    }
    else if (action == "vcc" && cmd.length() > 5){
        int space2 = cmd.indexOf(' ', space1 + 1);
        int vcc = cmd.substring(space1 + 1, space2).toInt();
        ESP_LOGI(__func__, "vcc: %d", vcc);
        WatchClock.Volt = vcc;
    }
    else if (action == "light" && cmd.length() > 7){
        int space2 = cmd.indexOf(' ', space1 + 1);
        int state = cmd.substring(space1 + 1, space2).toInt();
        ESP_LOGI(__func__, "Light: %d", state);
        if(state){
            HWCFG.LedON = true;
        }else HWCFG.LedON = false;
    }
    else if (action == "get")
    {
        // Читаем текущее состояние
        Wire.requestFrom(ADR_IOEXP, 1);
        if (Wire.available())
        {
            uint8_t state = Wire.read();
            Serial.print("Current state: 0b");
            Serial.println(state, BIN);
        }
    }
    else if (action == "pulse1")
    {
        // Вызываем ClockPulse для Watch_1 с шагом 1 и длительностью 500
        ClockPulse(Watch_1, 1, 500);
        Serial.println("Pulse command executed");
    }
    else if (action == "pulse2")
    {
        // Вызываем ClockPulse для Watch_1 с шагом 1 и длительностью 500
        ClockPulse(Watch_2, 1, 500);
        Serial.println("Pulse command executed");
    }
    else if (action == "help")
    {
        Serial.println("Available commands:");
        Serial.println("VCC <value> <12|24>  - Set VCC value");
        Serial.println("LIGHT <value> <0|1>  - Set Light ON");
        Serial.println("SET <pin 0-7> <0|1>  - Set pin state");
        Serial.println("GET                  - Get pin state");
        Serial.println("HELP                 - Show HELP");
    }
    else
    {
        Serial.println("ERROR: Unknown command");
    }
}
//=========================================================================
//=========================================================================
void UART_Recieve_Data()
{
    static String input_buffer;

    if (Serial.available())
    {
        // Serial.println("1");
        // digitalWrite(LED_ST, HIGH);
        char c = Serial.read();
        if (c == '\n' || c == '\r')
        {
            if (input_buffer.length() > 0)
            {
                processCommand(input_buffer);
                input_buffer = "";
            }
        }
        else
        {
            input_buffer += c;
        }
    }
    // put streamURL in serial monitor
    //     String r = Serial.readString();
    //     bool block_st = false;
    //     r.trim();
    //     if (r.length() > 3)
    //     {
    //         // Amplifier.connecttohost(r.c_str());
    //         if (r == "time")
    //         {
    //             Serial.println("Current Time");

    //         }
    //         // if (r == "door1")
    //         // {
    //         //     Serial.println("DoorState");
    //         //     Tell_me_DoorState(true);
    //         // }
    //         // if (r == "door0")
    //         // {
    //         //     Serial.println("DoorState");
    //         //     Tell_me_DoorState(false);
    //         // }
    //         // if (r == "date")
    //         // {
    //         //     Serial.println("Current Date");
    //         //     Tell_me_CurrentData();
    //         // }
    //         // if (r == "tellmetime")
    //         // {
    //         //     Serial.println("Current Time and Date");
    //         //     Tell_me_CurrentTime();
    //         //     Tell_me_CurrentData();
    //         // }
    //     }
    //     else
    //     {
    //         // Amplifier.setVolume(r.toInt());
    //         HWCFG.VOL = r.toInt();
    //         STATE.VolumeUPD = true;
    //     }
    //     log_i("free heap=%i", ESP.getFreeHeap());
    //     vTaskDelay(10 / portTICK_PERIOD_MS);
    // }
}
//=========================================================================
void DriverControl(uint8_t channel, uint8_t state)
{
    // if (forward) {
    //   digitalWrite(IN1, HIGH);
    //   digitalWrite(IN2, LOW);
    // } else {
    //   digitalWrite(IN1, LOW);
    //   digitalWrite(IN2, HIGH);
    // }
}
//=========================================================================

//=========================================================================
void PCF8574_init()
{
    Wire.begin();
    pcf8574.pinMode(DRVA_1, OUTPUT);
    pcf8574.digitalWrite(DRVA_1, LOW);
    pcf8574.pinMode(DRVA_2, OUTPUT);
    pcf8574.digitalWrite(DRVA_2, LOW);
    pcf8574.pinMode(DRVB_1, OUTPUT);
    pcf8574.digitalWrite(DRVB_1, LOW);
    pcf8574.pinMode(DRVB_2, OUTPUT);
    pcf8574.digitalWrite(DRVB_2, LOW);
    pcf8574.pinMode(VDIV, OUTPUT);
    pcf8574.digitalWrite(VDIV, LOW);
    pcf8574.pinMode(MAX_PWR, OUTPUT);
    pcf8574.digitalWrite(MAX_PWR, LOW);
    pcf8574.pinMode(LIGHT_EN, OUTPUT);
    pcf8574.digitalWrite(LIGHT_EN, LOW);
    pcf8574.pinMode(HEAT_EN, OUTPUT);
    pcf8574.digitalWrite(HEAT_EN, LOW);
}
//=========================================================================

//=========================================================================
void PCF8574_InitPins()
{
    // Массив пинов для инициализации
    const uint8_t pins[] = {DRVA_1, DRVA_2, DRVB_1, DRVB_2, VDIV, MAX_PWR, LIGHT_EN, HEAT_EN};
#ifdef DEBUG
    Serial.print(F("Expander_Init..."));
#endif
    // Инициализация Wire и установка всех пинов в LOW
    Wire.begin();
    for (uint8_t i = 0; i < sizeof(pins) / sizeof(pins[0]); i++)
    {
        pcf8574.pinMode(pins[i], OUTPUT);
        pcf8574.digitalWrite(pins[i], LOW);
    }
#ifdef DEBUG
    Serial.println(F("DONE"));
#endif
}
