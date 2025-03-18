# Система часофикации. 
## IoT контроллер первичных часов.
<!-- ![table](https://github.com/EmbedDevIOT/ClockStation_rev0/blob/main/doc/StartPage.jpg) -->
## Software:
* Visual Studio Code
* PlatformIO
* Arduino Framework
* Bootstrap Framework

## HARDWARE

- MCU: SoC Espressif (WROOM-32D)
- I2C Extender MC PCF8574T
- 4CH PushPull Driver
- DC/DC StepDown MP2307 (12 to 5)
- DC/DC StepUP MAX1771 (12 to 24)
- LDO AMS1117 3.3
- Optocuple Isolated Output
- Analog INPUT (Light Sensor)
- RTC DS3231
- GPS / Glonass module L76
- State led indicators
- User Button
  
<details>
<summary>На борту</summary>
  
  1. Wi-Fi
  2. HTTP - Server
  3. OTA - UPDATE
  4. RS-485
  </details>