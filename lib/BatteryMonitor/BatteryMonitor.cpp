#include "BatteryMonitor.h"

#define NO_OF_SAMPLES 64

BatteryMonitor::BatteryMonitor(adc1_channel_t channel, float voltageMin, float voltageMax,
                               uint32_t r1, uint32_t r2, uint32_t vref)
    : _channel(channel), _vMin(voltageMin), _vMax(voltageMax), _vref(vref)
{
    _dividerRatio = (float)(r1 + r2) / r2;
}

void BatteryMonitor::begin() {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(_channel, ADC_ATTEN_DB_11);
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, _vref, &_adc_chars);
}

uint32_t BatteryMonitor::readAdcMv() {
    uint32_t adc_reading = 0;
    for (int i = 0; i < NO_OF_SAMPLES; i++) {
        adc_reading += adc1_get_raw(_channel);
    }
    adc_reading /= NO_OF_SAMPLES;
    return esp_adc_cal_raw_to_voltage(adc_reading, &_adc_chars);
}

float BatteryMonitor::getDividerVoltage() {
    Serial.printf("AdcMv: %d V", readAdcMv());
    return readAdcMv() / 1000.0f;
}

float BatteryMonitor::getBatteryVoltage() {
    return getDividerVoltage() * _dividerRatio;
}

uint32_t BatteryMonitor::getAdcRaw() {
    uint32_t adc_reading = 0;
    for (int i = 0; i < NO_OF_SAMPLES; i++) {
        adc_reading += adc1_get_raw(_channel);
    }
    return adc_reading / NO_OF_SAMPLES;
}

uint8_t BatteryMonitor::getBatteryPercent() {
    float voltage = getBatteryVoltage();
    if (voltage <= _vMin) return 0;
    if (voltage >= _vMax) return 100;

    float percent = (voltage - _vMin) / (_vMax - _vMin) * 100.0f;
    return (uint8_t)percent;
}
