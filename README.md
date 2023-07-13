- **Smart Home System Based on MQTT and LVGL**;
- Lower computer based on ESP8266 and STM32, responsible for collecting data, developed through RT-Thread Studio using official software packages;
- Of course, it can also be achieved through FreeRTOS combined with writing MQTT platform functions through usart;
- Raspberry Pi 3B cooperates with EMQX to build a server, which can achieve external network access through internal network penetration(www.hsk.oray.com);
- IMX6ULL achieves LVGL interface display and MQTT subscription through pthread;

![系统图](D:\Projects\SmartHome\系统图.png)


