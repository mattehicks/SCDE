# SCDE
Smart Connected Device Engine - Generation 2

A tool and a firmware for makers to set up IoT Devices

IoT Devices setup is stores in the maker.cfg
IoT Devices state is stores in the state.cfg

Embedded Commands
Add
Attr
Define
Delete
Deleteattr
Help
Include
List
Set
Shutdown
Sub


Embedded Modules
Global
Telnet

Modules
ESP32_BH1750       Experimental
ESP32_Control      To control the ESP32
ESP32_I2C_Master   Experimental
ESP32_S0           For S0 counting
ESP32_SwITCH       For switchig and PWM with internal ESP32 hardware



Created by Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany, for EcSUHA.de

MSchulze780@GMAIL.COM

Copyright by EcSUHA

This is part of

- ESP 8266EX & ESP32 SoC Activities ...

- HoME CoNTROL & Smart Connected Device Engine Activities ...
 
EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE



CMD to build the SPIFFS image:
./mkspiffs -c filesystem/ -b 4096 -p 256 -s 0x100000 spiffs.bin

CMD to flash the SPIFFS Image:
esptool --chip esp32 --port /dev/ttyUSB0 --baud 115200 write_flash -z 0x110000 spiffs.bin

CMD to build and flash SCDE
make flash

CMD to flash the partition table:
make partition_table

CMD to make changes to ESP32 IDF
make menuconfig

picocom -b 115200 /dev/ttyUSB0

