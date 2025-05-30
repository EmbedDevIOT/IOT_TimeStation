#ifndef HTTP_H
#define HTTP_H

#include "Config.h"
#include "WF.h"
#include "FileConfig.h"
#include <WebServer.h>


void HTTPinit();
bool handleFileRead(String path);
String getContentType(String filename);
void UpdateData(void);
void HandleClient(void);
void BSaveSN(void);
void StartWC1(void);
void ClockPulseUPD(void);
void UpdateWatchClock1(void);
void BDS();
void IDLEControl(void);
// void UpdateStateWC(void);
// void TimeUpdate(void);
// void SystemUpdate(void);
// void TextUpdate(void);
// void ColorUpdate(void);
// void WCLogiqUPD(void);
// void SerialNumberUPD(void);
// void SaveSecurity(void);
// void Restart(void);
// void TimeToSpeech();
// void WC1DoorStateToSpeech();
// void WC2DoorStateToSpeech();
// void FactoryReset(void);
// void ShowSystemInfo(void);

#endif