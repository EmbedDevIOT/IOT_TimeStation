#ifndef _SysHandler_H
#define _SysHandler_H

#include "Config.h"

void UART_Recieve_Data(void);
void PCF8574_init(void);
void PCF8574_InitPins();
void PCF8574PowerEN(const int8_t &pin, const int8_t &state);

#endif