ESP32_I2C_Master_Module for SCDE (Smart Connedcted Device Engine) V2

- ESP32_ -> intended for use on ESP32 SoC hardware only ! 

- I2C_Master -> I2C_Master, an IO-Device for i2c bus, as 1. Stage-Module (in 2 Stages Design). It is used for 2. Stage-Modules to control devices connected on the i2c bus on the device that runs the SCDE. (temp. sensores, light sensors, ...).

- _Module -> intended for use with SCDE (Smart Connected Device Engine) only !

Created by Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany, for EcSUHA.de

MSchulze780@GMAIL.COM

Copyright by EcSUHA

This is part of

- ESP 8266EX & ESP32 SoC Activities ...

- HoME CoNTROL & Smart Connected Device Engine Activities ...
 
EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE


To use it: Define an definition with an custom name 'I2CM' of the module 'ESP32_I2C_Master'
 and KEY&VALUES to assign initial settings:
define I2CM ESP32_I2C_Master I2C_NUM=1&I2C_MODE=Master&SDA_IO=18&SDA_IO_PULLUP=Enabled&SCL_IO=19&SCL_IO_PULLUP=Enabled&MASTER_CLOCK=100000
