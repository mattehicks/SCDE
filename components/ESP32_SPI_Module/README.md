ESP32_SPI_Module for SCDE (Smart Connedcted Device Engine) V2
may be renamed to:
ESP32_SPI_Master_Module for SCDE (Smart Connedcted Device Engine) V2

- ESP32_ -> intended for use on ESP32 SoC hardware only ! 

- SPI -> Serial Peripheral Interface, an IO-Device for SPI interface, as 1. Stage-Module (in 2 Stages Design). It is used for 2. Stage-Modules to control devices connected on the SPI interface on the device that runs the SCDE. (temp. sensores, light sensors, ...).

- _Module -> intended for use with SCDE (Smart Connected Device Engine) only !

Created by Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany, for EcSUHA.de

MSchulze780@GMAIL.COM

Copyright by EcSUHA

This is part of

- ESP 8266EX & ESP32 SoC Activities ...

- HoME CoNTROL & Smart Connected Device Engine Activities ...
 
EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE


To use it: Define an definition with an custom name 'SPI' of the module 'ESP32_SPI'
 and KEY&VALUES to assign initial settings:
define SPI ESP32_SPI XXX=123
