#ifndef CLOCKCONTROLLER_H
#define CLOCKCONTROLLER_H

#include "Config.h"

// class ClockController {
// public:
//     ClockController();
//     void begin();
//     void update();
//     void setTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
//     void getTime(uint8_t &hours, uint8_t &minutes, uint8_t &seconds) const;

// private:
//     uint8_t _hours;
//     uint8_t _minutes;
//     uint8_t _seconds;

//     void updateInternalClock();
// };

void ClockPulse(uint8_t clock, uint16_t step, uint32_t t_pulse);
void DisableAllDRVChannels();
void BacklightController();

#endif // CLOCKCONTROLLER_H