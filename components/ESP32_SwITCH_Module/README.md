ESP32_SwITCH_Module for SCDE (Smart Connedcted Device Engine) V2

ESP32_ -> intended for use on ESP32 SoC hardware only ! 

SwITCH -> SwITCH Feature

The SwITCH Feature is used to drive GPIOs as a switch (on/off/pwm) with the goal to control hardware
(Relay, LED-Controller, Smart Socket, ...). The ESP32 SoC GPIO will be connected to the PWM-Hardware.
The PWM-Hardware is used and controlled by this Module. This is hardware PWM/switching.

_Module -> intended for use with SCDE (Smart Connected Device Engine) only !


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
 
 SAMPLE USAGE :
 Init custom SwITCH Feature named "Switch.1" at GPIO 13:
 define Switch.1 esp32_SwITCH GPIO=13&BLOCK=High_Speed&CHANNEL=0&TIMER=0&TICK_SOURCE=Low&RESOLUTION=8&FREQ_HZ=9765&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55
 
 set Switch.1 DUTY=210
 
 
 
