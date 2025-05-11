#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include "Config.h"
#include <driver/adc.h>
#include <esp_adc_cal.h>

#define DEFAULT_VREF    1100             // В миллиВольтах, для калибровки (можно уточнить через esp_adc_cal)
#define NO_OF_SAMPLES   64               // Усреднение для снижения шума

// Battery voltage limits
#define BATTERY_VOLTAGE_MIN 5.4
#define BATTERY_VOLTAGE_MAX 6.9


class BatteryMonitor {
public:
    BatteryMonitor(adc1_channel_t channel, float voltageMin, float voltageMax,
                   uint32_t r1, uint32_t r2, uint32_t vref = 1100);

    void begin();
    uint8_t getBatteryPercent();
    float getBatteryVoltage();    // Вольтаж батареи
    float getDividerVoltage();    // Вольтаж на делителе
    uint32_t getAdcRaw();         // Сырое значение ADC

private:
    adc1_channel_t _channel;
    float _vMin, _vMax;
    float _dividerRatio;
    uint32_t _vref;
    esp_adc_cal_characteristics_t _adc_chars;

    uint32_t readAdcMv();  // Усреднение + калибровка
};

#endif
