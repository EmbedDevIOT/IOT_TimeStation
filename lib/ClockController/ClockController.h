#ifndef CLOCKCONTROLLER_H
#define CLOCKCONTROLLER_H

#include "Config.h"

void WatchHandler(void);
void ClockController(uint8_t Hour, uint8_t Minute);
void ClockPulse(uint8_t clock, uint16_t step, uint32_t t_pulse);
void DisableAllDRVChannels();
void BacklightController();

#endif // CLOCKCONTROLLER_H