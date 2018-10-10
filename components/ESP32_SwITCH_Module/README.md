ESP32_SwITCH Module

This is used to set up GPIOs as a switch with the goal to control switching hardware (relay, ...). The GPIO switching is Hardware PWM driven (for ESP32 SOC only). 
Module for use with SCDE (Smart Connected Device Engine) only !
 
   ESP 8266EX & ESP32 SoC Activities ...
   HoME CoNTROL & Smart Connected Device Engine Activities ...
   Copyright by EcSUHA
 
   Created by Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany for EcSUHA.de 
 
   MSchulze780@GMAIL.COM
   EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
   
   
  HELP-STRINGS:
  Define <Def-Name> ESP32_SwITCH <GPIO=> <CHANNEL=> <TIMER=> <TICK_SOURCE=REF|APB_SLOW> <RESOLUTION=8> <FREQ_HZ=> <IDLE_LV=>[<SIG_OUT_EN=> <HPOINT=0> <DUTY=0>]
 * Set <Def-Name> <GPIO=> <TICK_SOURCE=> <RESOLUTION=8> <FREQ_HZ=> <SIG_OUT_EN=> <IDLE_LV=> <HPOINT=0> <DUTY=>
 * (not allowed to change by SET: <BLOCK=> <TIMER=> <CHANNEL=> )
 *
 * SAMPLE USAGE :
 * define Switch.1 esp32_SwITCH GPIO=13&BLOCK=High_Speed&CHANNEL=0&TIMER=0&TICK_SOURCE=Low&RESOLUTION=8&FREQ_HZ=9765&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55
 * set Switch.1 DUTY=210
 
 
 
