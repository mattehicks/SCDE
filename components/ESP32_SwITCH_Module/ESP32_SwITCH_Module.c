/* #################################################################################################
 *
 *      Name: ESP32_SwITCH Module
 *  Function: This is used to set up GPIOs as a switch with the goal to control switching hardware
 *            (relay, ...). The GPIO switching is Hardware PWM driven (for ESP32 SOC only). 
 *            Module for use with SCDE (Smart Connected Device Engine) only !
 *
 *  ESP 8266EX & ESP32 SoC Activities ...
 *  HoME CoNTROL & Smart Connected Device Engine Activities ...
 *  Copyright by EcSUHA
 *
 *  Created by Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany for EcSUHA.de 
 *
 *  MSchulze780@GMAIL.COM
 *  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
 * #################################################################################################
 *
 * DIV calculation and allowed freq in HZ: REF=1.000.000hz / APB 80.000.000 hz
 *  clock source (eg. REF 1.000.000) / 16hz / (1<<RESOLUTION (eg.8bit)) = 
 *
 *#define LEDC_APB_CLK_HZ (APB_CLK_FREQ)  // 80.000.000 hz sample: 9765hz / 256bit-res -> divider is 32
 *#define LEDC_REF_CLK_HZ (1*1000000)
 *
 * wenn Timer setup [<TICK_SOURCE=REF|APB_SLOW> & <RESOLUTION=8> & <FREQ_HZ=> ] 
 *
 * HELP-STRINGS:
 * Define <Def-Name> ESP32_SwITCH <GPIO=> <CHANNEL=> <TIMER=> <TICK_SOURCE=REF|APB_SLOW> <RESOLUTION=8> <FREQ_HZ=> <IDLE_LV=> [<SIG_OUT_EN=> <HPOINT=0> <DUTY=0>]
 * Set <Def-Name> <GPIO=> <TICK_SOURCE=> <RESOLUTION=8> <FREQ_HZ=> <SIG_OUT_EN=> <IDLE_LV=> <HPOINT=0> <DUTY=>
 * (not allowed to change by SET: <BLOCK=> <TIMER=> <CHANNEL=> )
 *
 * SAMPLE USAGE :
 * define Switch.1 esp32_SwITCH GPIO=13&BLOCK=High_Speed&CHANNEL=0&TIMER=0&TICK_SOURCE=Low&RESOLUTION=8&FREQ_HZ=9765&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55
 * set Switch.1 DUTY=210
 *
 */

/*
define Switch.2 esp32_SwITCH GPIO=14&BLOCK=High_Speed&CHANNEL=2&TIMER=0&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=9765&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55
define Switch.3 esp32_SwITCH GPIO=15&BLOCK=High_Speed&CHANNEL=3&TIMER=0&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=9765&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55

set Switch.1 esp32_SwITCH GPIO=13&BLOCK=High_Speed&CHANNEL=0&TIMER=0&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=9765&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=1&DUTY=55
set Switch.1 SIG_OUT_EN=Enabled
set Switch.1 SIG_OUT_EN=Disabled

// for test makerfile 8 ch ssr relay
Test: HS ch REF clock
define SSR.1.at.GPIO.13 esp32_SwITCH GPIO=13&BLOCK=High_Speed&CHANNEL=0&TIMER=0&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55
define SSR.2.at.GPIO.12 esp32_SwITCH GPIO=12&BLOCK=Low_Speed&CHANNEL=0&TIMER=0&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55
define SSR.3.at.GPIO.14 esp32_SwITCH GPIO=14&BLOCK=High_Speed&CHANNEL=1&TIMER=1&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55
define SSR.4.at.GPIO.27 esp32_SwITCH GPIO=27&BLOCK=High_Speed&CHANNEL=2&TIMER=1&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55
define SSR.5.at.GPIO.26 esp32_SwITCH GPIO=26&BLOCK=High_Speed&CHANNEL=3&TIMER=2&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55
define SSR.6.at.GPIO.25 esp32_SwITCH GPIO=25&BLOCK=High_Speed&CHANNEL=4&TIMER=2&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55

define SSR.7.at.GPIO.33 esp32_SwITCH GPIO=33&BLOCK=High_Speed&CHANNEL=5&TIMER=2&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55
define SSR.8.at.GPIO.32 esp32_SwITCH GPIO=32&BLOCK=High_Speed&CHANNEL=6&TIMER=2&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55

//GPIOs 0, 1, 2, 3, 4, 5, 12, 13, 14, 15, 16, 17, 18 ,19, 21, 22, 23, 25, 26, 27, 32 and 33.
//checked GPIOs OK 0, 12, 13, 14, 25, 26, 27.

set Switch.1 DUTY=1
set Switch.1 DUTY=255

set Switch.1 HPOINT=30&DUTY=100
set Switch.1 IDLE_LV=High
set Switch.1 IDLE_LV=Low
set Switch.1 GPIO=13

set Switch.1 RESOLUTION=8&TICK_SOURCE=REF&DIV_FACTOR=7
set Switch.1 TICK_SOURCE=REF
set Switch.1 DIV_FACTOR=7

*/

#include <ProjectConfig.h>
#include <esp8266.h>
#include <Platform.h>

#include <SCDE_s.h>

#include "ESP32_SwITCH_Module.h"

#include "SCDE_Main.h"



// we are using the hardware counter
//#include "soc/pcnt_reg.h"
//#include "soc/pcnt_struct.h"


#include "driver/gpio.h"

#define LEDC_APB_CLK_HZ (APB_CLK_FREQ)  // 80.000.000 hz sample: 9765hz / 256bit-res -> divider is 32
#define LEDC_REF_CLK_HZ (1*1000000)


// -------------------------------------------------------------------------------------------------

// ammount of debug information 0 = off 5 max?
#ifndef ESP32_SwITCH_DBG
#define ESP32_SwITCH_DBG 0
#endif

// -------------------------------------------------------------------------------------------------

// make data root locally available
static SCDERoot_t* SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for operation
static SCDEFn_t* SCDEFn;

// -------------------------------------------------------------------------------------------------

#include <WebIf_EspFSStdFileTX.h>
#include <WebIf_EspFSAdvFileTX.h>

#include "HTools_cgi.h"
#include <ServAPCfg_tpl.h>
#include <CGI_Redirect.h>



// -------------------------------------------------------------------------------------------------



/*
 * Implemented Values for Keys
 */

// ESP32_SwITCH_SET_SIG_OUT_EN -> 'SIG_OUT_EN' -> Disabled|Enabled
SelectAData ESP32_SwITCH_DisEna[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"Disabled"},
  {1,"Enabled"}, 
  {0, NULL}
  };

// ESP32_SwITCH_SET_IDLE_LV -> 'IDLE_LV' -> Low|High
SelectAData ESP32_SwITCH_LoHi[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"Low"},
  {1,"High"}, 
  {0, NULL}
  };

// ESP32_SwITCH_SET_BLOCK -> 'PWM_BLOCK'-> High_Speed|Low_Speed
SelectAData ESP32_SwITCH_PWM_BLOCK[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"High_Speed"},
  {1,"Low_Speed"},
  {0, NULL}
  };

// ESP32_SwITCH_SET_TICK_SOURCE -> 'TICK_SOURCE'-> REF|APB_SLOW
SelectAData ESP32_SwITCH_TICK_SOURCE[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"REF"},
  {1,"APB_SLOW"}, 
  {0, NULL}
  };



/**
 * For Type: ESP32_SwITCH
 * Implemented-Args-Keys, for input (MAX 64!). //IAK!!
 * KEYs-Field-Set for Feature specific query. Will be generated by SCDEH_ParseStrToparsedKVInput()
 * parsed result is stored in ESP32_SwITCH_parsedKVInput_t
 */
enum ESP32_SwITCH_SET_IK {				// Bit #XX for debugging

  // S0 calculation configuration
    ESP32_SwITCH_SET_ON		= 0			// Bit #00 'ON'  -> 
  , ESP32_SwITCH_SET_OFF				// Bit #01 'OFF'  -> 

  , ESP32_SwITCH_SET_MAX				// Bit #02 'MAX'  -> 
  , ESP32_SwITCH_SET_MIN				// Bit #03 'MIN'  -> 

  , ESP32_SwITCH_SET_VAL				// Bit #04 'VAL'  -> 
  , ESP32_SwITCH_SET_DIM				// Bit #05 'DIM'  -> 

  , ESP32_SwITCH_SET_NAME				// Bit #06 'NAME'  -> 
  , ESP32_SwITCH_SET_CAPS				// Bit #07 'CAPS'  -> 

  // Block #01 PWM configuration
  , ESP32_SwITCH_SET_GPIO				// Bit #08 'GPIO' -> 
  , ESP32_SwITCH_SET_BLOCK				// Bit #09 'BLOCK' -> 
  , ESP32_SwITCH_SET_CHANNEL				// Bit #10 'CHANNEL' -> 
  , ESP32_SwITCH_SET_TIMER				// Bit #11 'TIMER' -> 

  , ESP32_SwITCH_SET_DUTY				// Bit #12 'DUTY' ->
  , ESP32_SwITCH_SET_HPOINT				// Bit #13 'HPOINT' ->
  , ESP32_SwITCH_SET_SIG_OUT_EN				// Bit #14 'SIG_OUT_EN' ->
  , ESP32_SwITCH_SET_IDLE_LV				// Bit #15 'IDLE_LV' ->

  , ESP32_SwITCH_SET_RESOLUTION			// Bit #16 'RESOLUTION' -> 
  , ESP32_SwITCH_SET_TICK_SOURCE			// Bit #17 'TICK_SOURCE' ->
  , ESP32_SwITCH_SET_FREQ_HZ				// Bit #18 'FREQ_HZ' ->

  , ESP32_SwITCH_SET_THR_L_LIM_EN			// Bit #19 'THR_L_LIM_EN' -> DIS/ENA
  , ESP32_SwITCH_SET_THR_H_LIM_EN			// Bit #20 'THR_H_LIM_EN' -> DIS/ENA
  , ESP32_SwITCH_SET_THR_ZERO_EN			// Bit #21 'THR_ZERO_EN' -> DIS/ENA
  , ESP32_SwITCH_SET_FILTER_EN				// Bit #22 'FILTER_EN' -> DIS/ENA
  , ESP32_SwITCH_SET_FILTER_THRES			// Bit #23 'FILTER_THRES' -> decval 10 bit zahl
  , ESP32_SwITCH_SET_CNT_THRES1				// Bit #24 'CNT_THRES1' -> decval 16 bit zahl
  , ESP32_SwITCH_SET_CNT_THRES0				// Bit #25 'CNT_THRES0' -> decval 16 bit zahl
  , ESP32_SwITCH_SET_CNT_L_LIM				// Bit #26 'CNT_L_LIM' -> decval 10 bit zahl
  , ESP32_SwITCH_SET_CNT_H_LIM				// Bit #27 'CNT_H_LIM' -> decval 10 bit zahl

// end marker
  , ESP32_SwITCH_SET_IK_Number_of_keys			// Bit #27 MAX 64 IMPLEMENTED !

};

 

/**
 * For Type: ESP32_SwITCH
 * Implemented readings (MAX 32!)
 * Can be assigned to Implemented Keys, if affected
 */
enum ESP32_SwITCH_Readings {			// Bit #XX for debugging

   ESP32_SwITCH_R_DUTY		= (1<<0)	// Bit #00 'Duty'  -> 

  ,ESP32_SwITCH_R_RESOLUTION	= (1<<1)	// Bit #01 'Resolution' -> 

  ,ESP32_SwITCH_R_NAME		= (1<<2)	// Bit #02 'name' + caps + ufid -> 

};



/**
 * For Type: ESP32_SwITCH
 * Implemented KEYS and assigned readings for this query - analyzed by by http_parser_parse_url()

 * Num should meet enum XX_QueryFields
 * Result is stored in struct SCDE_XX_parsedKVInput
 */
kvParseImplementedKeys_t ESP32_SwITCH_Set_ImplementedKeys[] = {
// |                                                          affected readings										              	|  CMD

// for usage 
   { ESP32_SwITCH_R_DUTY | ESP32_SwITCH_R_RESOLUTION															, "ON" }		// #00
  ,{ ESP32_SwITCH_R_DUTY | ESP32_SwITCH_R_RESOLUTION															, "OFF" }		// #01
  ,{ ESP32_SwITCH_R_DUTY | ESP32_SwITCH_R_RESOLUTION															, "MAX" }		// #02
  ,{ ESP32_SwITCH_R_DUTY | ESP32_SwITCH_R_RESOLUTION															, "MIN" }		// #03
  ,{ ESP32_SwITCH_R_DUTY | ESP32_SwITCH_R_RESOLUTION															, "VAL" }		// #04
  ,{ ESP32_SwITCH_R_DUTY | ESP32_SwITCH_R_RESOLUTION															, "DIM" }		// #05

  ,{ ESP32_SwITCH_R_DUTY | ESP32_SwITCH_R_RESOLUTION | ESP32_SwITCH_R_NAME												, "NAME" }		// #06
  ,{ ESP32_SwITCH_R_DUTY | ESP32_SwITCH_R_RESOLUTION | ESP32_SwITCH_R_NAME												, "CAPS" }		// #07

// for configuration
  ,{ 0																					, "GPIO" }		// #08
  ,{ 0																					, "BLOCK" }		// #09
  ,{ 0																					, "CHANNEL" }		// #10
  ,{ 0																					, "TIMER" }		// #11
  ,{ 0																					, "DUTY" }		// #12
  ,{ 0																					, "HPOINT" }		// #13
  ,{ 0																					, "SIG_OUT_EN" }	// #14
  ,{ 0																					, "IDLE_LV" }		// #15

  ,{ 0																					, "RESOLUTION" }		// #16
  ,{ 0																					, "TICK_SOURCE" }	// #17
  ,{ 0																					, "FREQ_HZ" }		// #18

  ,{ 0																					, "THR_L_LIM_EN" }	// #19
  ,{ 0																					, "THR_H_LIM_EN" }	// #20
  ,{ 0																					, "THR_ZERO_EN" }	// #21
  ,{ 0																					, "FILTER_EN" }		// #22
  ,{ 0																					, "FILTER_THRES" }	// #23
  ,{ 0																					, "CNT_THRES1" }	// #24
  ,{ 0																					, "CNT_THRES0" }	// #25
  ,{ 0																					, "CNT_L_LIM" }		// #26
  ,{ 0																					, "CNT_H_LIM" }		// #27

}; // number of elements should be equal with XX_SET_IK_Number_of_keys, LIMIT IS 64 ELEMENTS !





/*"WSAP_Password ";
	$list .= "WSAP_RF_Channel:uzsuSelectRadio,1,2,3,4,5,6,7,8,9,10,11,12,13 ";

"WSAP_Beacon_Interval:slider,100,10,60000 ";

uint8Slider,a,b,c

uint16Slider,a,b,c
uint32Slider,a,b,c
int8Slider,a,b,c
int16Slider,a,b,c
int32Slider,a,b,c

*/



/**
 * -------------------------------------------------------------------------------------------------
 *  DName: ESP32_SwITCH_ActiveResourcesDataA_forWebIf
 *  Desc: Resource-Content-structure of active Directory - PART A (Resource-Data-Row)
 *  Data: WebIf_ActiveResourcesDataA_t[X] from HttpD.h
 * -------------------------------------------------------------------------------------------------
 */
// Content:   AllowedMethodBF          | AllowedDocMimeBF  | AllowedSchemeBF |free|CgiNo| EnaByBit | Url
const WebIf_ActiveResourcesDataA_t ESP32_SwITCH_ActiveResourcesDataA_forWebIf[] = {  //ICACHE_RODATA_ATTR = 

  // SOC Hardware Cfg
  { 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0,  0, 0b00000000, "/SoCHWCfg"}

  // WiFi Stations Cfg
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0,  1, 0b00000000, "/WiFi/StationCfg"}

  // WIFI Q-Connect PAGES
 ,{ 0b00000000000000000000000000001010, 0b0000000000000010, 0b0000000000000001,  0,  2, 0b00000000, "/WiFi/QConnect"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000001000, 0b0000000000000001,  0,  3, 0b00000000, "/WiFi/WiFiScan"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000000100, 0b0000000000000001,  0,  4, 0b00000000, "/WiFi/Connect"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000000100, 0b0000000000000001,  0,  5, 0b00000000, "/WiFi/Setmode"}

  // Service Access Point Cfg
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0,  6, 0b00000000, "/WiFi/ServAPCfg"}

  // TimeStamp Cfg
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0,  7, 0b00000000, "/TiStCfg"}

  // Firmware Update
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0,  8, 0b00000000, "/Firmware"}

  // Redirects
 ,{ 0b00000000000000000000000000001010, 0b0000000000000001, 0b0000000000000001,  0,  9, 0b00000000, "/WiFi"}	// nomime
 ,{ 0b00000000000000000000000000001010, 0b0000000000000001, 0b0000000000000001,  0,  9, 0b00000000, "/WiFi/"}	// nomime
 ,{ 0b00000000000000000000000000001010, 0b0000000000000001, 0b0000000000000001,  0, 10, 0b00000000, "/"}	// nomime
 ,{ 0b00000000000000000000000000001010, 0b0000000000000010, 0b0000000000000001,  0, 10, 0b00000000, "/index"}	// .htm

  // Secret services
 ,{ 0b00000000000000000000000000001010, 0b0000000000100000, 0b0000000000000001,  0, 11, 0b00000000, "/32MBitFlash"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000100000, 0b0000000000000001,  0, 12, 0b00000000, "/wfs"}

 ,{0,0,0,0,0,0,"*"}

  };



/**
 * -------------------------------------------------------------------------------------------------
 *  DName: ESP32_SwITCH_ActiveResourcesDataB_forWebIf
 *  Desc: Resource-Content-structure of active Directory - PART B (Procedure-Call-Data-Row) 
 *  Data: WebIf_ActiveResourcesDataB_t[X] from HttpD.h
 * -------------------------------------------------------------------------------------------------
 */
// CgiFucID=(No.<<16)+AllowedSchemeBF |      cgi            |     cgi_data
const WebIf_ActiveResourcesDataB_t ESP32_SwITCH_ActiveResourcesDataB_forWebIf[] =  {  //ICACHE_RODATA_ATTR =

  // ### SOC Hardware Cfg ###
  {( 0<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	SoCHWCfg_tpl	}
 ,{( 0<<16) +	0b0000000000000100,	NULL,NULL}//	SoCHWCfg_cgi,		NULL		}
 ,{( 0<<16) +	0b0000000000001000,	NULL,NULL}//	SoCHWCfg_jso,		NULL		}

  // ### WiFi Station Cfg ###
 ,{( 1<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	StationCfg_tpl	}
 ,{( 1<<16) +	0b0000000000000100,	NULL,NULL}//	StationCfg_cgi,		NULL		}
 ,{( 1<<16) +	0b0000000000001000,	NULL,NULL}//	StationCfg_jso,		NULL		}

  // ### WIFI Q-Connect PAGES ###
 ,{( 2<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	WifiQConnect_tpl}
 ,{( 3<<16) +	0b0000000000001000,	NULL,NULL}//	WiFiScan_jso,		NULL		}
 ,{( 4<<16) +	0b0000000000000100,	NULL,NULL}//	WiFiConnect_cgi,	NULL		}
 ,{( 5<<16) +	0b0000000000000100,	NULL,NULL}//	WifiSetMode_cgi,	NULL		}	

  // ### Service Access Point Cfg ###
 ,{( 6<<16) +	0b0000000000000010,	WebIf_EspFSAdvFileTX,	ServAPCfg_tpl	}
 ,{( 6<<16) +	0b0000000000000100,	NULL,NULL}//	ServAPCfg_cgi,		NULL		}
 ,{( 6<<16) +	0b0000000000001000,	NULL,NULL}//	ServAPCfg_jso,		NULL		}

  // ### TimeStamp Cfg ###
 ,{( 7<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	TiStCfg_tpl	}
 ,{( 7<<16) +	0b0000000000000100,	NULL,NULL}//	TiStCfg_cgi,		NULL		}
 ,{( 7<<16) +	0b0000000000001000,	NULL,NULL}//	TiStCfg_jso,		NULL		}

  // ### Firmware Update ###
 ,{( 8<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	FirmwareUd_tpl	}
 ,{( 8<<16) +	0b0000000000000100,	NULL,NULL}//	FirmwareUd_cgi,		NULL		}
 ,{( 8<<16) +	0b0000000000001000,	NULL,NULL}//	FirmwareUd_jso,		NULL		}

  // ### Redirects ###
 ,{( 9<<16) +	0b0000000000000001,	cgiRedirect,	"/WiFi/QConnect.htm"		}
 ,{(10<<16) +	0b0000000000000001,	cgiRedirect,	"/CoNTROL_8S-1C-1ADC.htm"	}
 ,{(10<<16) +	0b0000000000000010,	cgiRedirect,	"/CoNTROL_8S-1C-1ADC.htm"	}

  // ### Secret services ###
 ,{(11<<16) +	0b0000000000100000,	ReadFullFlash_cgi,	NULL		}		//.bin
 ,{(12<<16) +	0b0000000000100000,	NULL,NULL}//	WriteFileSystem_cgi,	NULL		}		//.bin

  };



/**
 * -------------------------------------------------------------------------------------------------
 *  DName: ESP32_SwITCH_Module
 *  Desc: Data 'Provided By Module' for the ESP32_SwITCH module (functions + infos this module provides
 *        to SCDE)
 *  Data: 
 * -------------------------------------------------------------------------------------------------
 */
ProvidedByModule_t ESP32_SwITCH_ProvidedByModule = {
   // A-Z order

  "ESP32_SwITCH"			// Type-Name of module -> should be same name as libfilename.so !
  ,12					// size of Type-Name

  ,NULL					// Add

  ,ESP32_SwITCH_Attribute		// Attribute

  ,ESP32_SwITCH_Define			// Define

  ,NULL					// Delete

  ,NULL					// Except

  ,NULL					// Get

  ,ESP32_SwITCH_IdleCb			// IdleCb

  ,ESP32_SwITCH_Initialize		// Initialize

  ,NULL					// Notify

  ,NULL					// Parse

  ,NULL					// Read

  ,NULL					// Ready

  ,NULL					// Rename

  ,ESP32_SwITCH_Set			// Set

  ,ESP32_SwITCH_Shutdown		// Shutdown

  ,NULL					// State

  ,NULL					// Sub

  ,ESP32_SwITCH_Undefine		// Undefine

  ,NULL					// DirectRead

  ,NULL					// DirectWrite

//  ,NULL		 		// FnProvided

  ,sizeof(ESP32_SwITCH_Definition_t)	// Size of modul specific definition structure (Common_Definition_t + X)

};



/* -------------------------------------------------------------------------------------------------
 *  FName: ESP32_SwITCH_Attribute
 *  Desc: Informs an Definition of this Type for attribute activities (set/del)
 *  Info: 'attrCmd' is the command text for the activity: set, del
 *        'attrName' is the attribute name text
 *        'attrVal' is the attribute value text
 *        THE attrVal CAN BE MANIPULATED / CORRECTED
 *  Para: Common_Definition_t *Common_Definition -> the belonging definition for the attribute-activitie
 *	  const uint8_t *attrCmdText -> ptr to attribute-command text "attrCmd"
 *	  const size_t attrCmdTextLen -> length of attribute-command text "attrCmd"
 *	  const uint8_t *attrNameText -> ptr to attribute-name text "attrName"
 *	  const size_t attrNameTextLen -> length of attribute-name text "attrName"
 *	  uint8_t **attrValTextPtr -> ptr to ptr holding attribute-value text "attrValue" - manipulation possible
 *	  size_t *attrValLenTextPtr -> ptr to length of attribute-value text "attrValue" - manipulation possible
 *  Rets: strTextMultiple_t* -> veto = error-text-string in allocated mem, or NULL = OK - no veto
 * -------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* ICACHE_FLASH_ATTR
ESP32_SwITCH_Attribute(Common_Definition_t *Common_Definition
		,const uint8_t *attrCmdText
		,const size_t attrCmdTextLen
		,const uint8_t *attrNameText
		,const size_t attrNameTextLen
		,uint8_t **attrValTextPtr
		,size_t *attrValTextLenPtr)
{

  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  uint8_t *attrValText = *attrValTextPtr;
  size_t attrValTextLen = *attrValTextLenPtr;

  printf("ESP32_SwITCH_AttributeFN for defName:%.*s -> attrCmd:%.*s attrName:%.*s attrVal:%.*s\n"
	,Common_Definition->nameLen
	,Common_Definition->name
	,attrCmdTextLen
	,attrCmdText
  	,attrNameTextLen
	,attrNameText
	,(int) attrValTextLen
	,attrValText);



/*
  // set start of possible def-Name
  const uint8_t *defName = args;

  // set start of possible attr-Name
  const uint8_t *attrName = args;

  // a seek-counter

  int i = 0;

  // seek to next space !'\32'
  while( (i < argsLen) && (*attrName != ' ') ) {i++;attrName++;}

  // length of def-Name
  size_t defNameLen = i;

  // seek to start position of attr-Name '\32'
  while( (i < argsLen) && (*attrName == ' ') ) {i++;attrName++;}

  // set start of possible attr-Val
  const uint8_t *attrVal = attrName;

  // a 2nd seek-counter
  int j = 0;

  // seek to next space !'\32'
  while( (i < argsLen) && (*attrVal != ' ') ) {i++,j++;attrVal++;}

  // length of attr-Name
  size_t attrNameLen = j;

  // start position of attr-Val
  while( (i < argsLen) && (*attrVal == ' ') ) {i++;attrVal++;}


  // length of attr-Val
  size_t attrValLen = argsLen - i;

  // veryfy lengths > 0, definition 0 allowed
  if ( (defNameLen == 0) || (attrNameLen == 0) )

		{

		// response with error text
		asprintf(&retMsg
				,"Could not interpret command ''! Usage: Attr <defname> <attrname> [<attrval>]");

		return retMsg;

	//	}

// -------------------------------------------------------------------------------------------------
*/

  return retMsg;

}



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_SwITCH_Define
 *  Desc: Finalizes the defines a new "device" of 'ESP32_S0' type. Contains devicespecific init code.
 *  Info: 
 *  Para: Common_Definition_t *Common_Definition -> prefilled ESP32Control Definition
 *        char *Definition -> the last part of the CommandDefine arg* 
 *  Rets: strTextMultiple_t* -> response text NULL=no text
 * -------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* ICACHE_FLASH_ATTR 
ESP32_SwITCH_Define(Common_Definition_t *Common_Definition)
//	,uint8_t *defArgsText
//	,size_t defArgsTextLen)
{

  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // make common ptr to modul specific ptr
  ESP32_SwITCH_Definition_t* ESP32_SwITCH_Definition =
		  (ESP32_SwITCH_Definition_t*) Common_Definition;
/*
  #if ESP32_SwITCH_DBG >= 5
  printf("\n|ESP32_SwITCH_Def, Def:%.*s>"
	,ESP32_SwITCH_Definition->common.definitionLen
	,ESP32_SwITCH_Definition->common.definition);
  #endif
*/
  // new conversation
  uint8_t *defArgsText = Common_Definition->definition;
  size_t defArgsTextLen = Common_Definition->definitionLen;

  #if ESP32_SwITCH_DBG >= 5
  printf("\n|ESP32_SwITCH_Def, Name:%.*s, got args:%.*s>"
	,ESP32_SwITCH_Definition->common.nameLen
	,ESP32_SwITCH_Definition->common.name
	,defArgsTextLen
	,defArgsText);
  #endif

// ------------------------------------------------------------------------------------------------

  // Check for args. This type requires args...
  if (!defArgsTextLen) {

	// alloc mem for retMsg
	retMsg = malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Parsing Error! Expected Args!");

	return retMsg;
  }

// ------------------------------------------------------------------------------------------------

  // store FD to Definition. Will than be processed in global loop ... -> THIS MODULE USES NO FD
  ESP32_SwITCH_Definition->common.fd = -1;

// ------------------------------------------------------------------------------------------------

  // init WebIf_Provided offset
//  ESP32Control_Definition->common.link =
//	&ESP32Control_Definition->WebIf_Provided;

  // check for loaded Module 'WebIf' -> get provided Fn
  ESP32_SwITCH_Definition->WebIf_Provided.WebIf_FnProvided =
	NULL;//(WebIf_FnProvided_t *) SCDEFn->GetFnProvidedByModule("WebIf");

 // Providing data for WebIf? Initialise data provided for WebIf
  if (ESP32_SwITCH_Definition->WebIf_Provided.WebIf_FnProvided) {

	ESP32_SwITCH_Definition->WebIf_Provided.ActiveResourcesDataA =
		(WebIf_ActiveResourcesDataA_t *) &ESP32_SwITCH_ActiveResourcesDataA_forWebIf;

	ESP32_SwITCH_Definition->WebIf_Provided.ActiveResourcesDataB =
		(WebIf_ActiveResourcesDataB_t *) &ESP32_SwITCH_ActiveResourcesDataB_forWebIf;

	}

  else	{

	SCDEFn->Log3Fn(Common_Definition->name
		,Common_Definition->nameLen
		,1
		,"Could not enable WebIf support for '%.*s'. Type '%.*s' detects Type 'WebIf' is NOT loaded!"
		,ESP32_SwITCH_Definition->common.nameLen
		,ESP32_SwITCH_Definition->common.name
		,ESP32_SwITCH_Definition->common.module->ProvidedByModule->typeNameLen
		,ESP32_SwITCH_Definition->common.module->ProvidedByModule->typeName);
	}

// ------------------------------------------------------------------------------------------------

  // Parse define-args (KEY=VALUE) protocol -> gets parsedKVInput in allocated mem, NULL = ERROR
  parsedKVInputArgs_t *parsedKVInput = 
	SCDEFn->ParseKVInputArgsFn(ESP32_SwITCH_SET_IK_Number_of_keys	// Num Implementated KEYs MAX
	,ESP32_SwITCH_Set_ImplementedKeys		// Implementated Keys
	,defArgsText				// our args text
	,defArgsTextLen);			// our args text len

  // parsing may report an problem. args contain: unknown keys, double keys, ...?
  if (!parsedKVInput) {

	// alloc mem for retMsg
	retMsg = malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Parsing Error! Args '%.*s' not taken! Check the KEYs!"
		,defArgsTextLen
		,defArgsText);

	return retMsg;
  }

// ------------------------------------------------------------------------------------------------














#include "driver/periph_ctrl.h"

#include "driver/gpio.h"
//#include "driver/pcnt.h"


#include "esp_attr.h"
#include "esp_log.h"
#include "soc/gpio_sig_map.h"


//esp_err_t pcnt_set_pin(pcnt_unit_t unit, pcnt_channel_t channel, int pulse_io, int ctrl_io)






  // enable the PWM (LED-Controller)
  SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_LEDC_CLK_EN);
  CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_LEDC_RST);




/*

  #if ESP32_SwITCH_DBG >= 5

  printf("\n|ESP32_SwITCH_Def, installed pulseIO:%d ctrlIO:%d PCNT_CTRL_REG:%u>"

	,pulse_io

	,ctrl_io
	,*xREG);
  #endif
*/



 //clr pause
// uint32_t *xREG = 0x3FF570b0;
// *xREG &= ~(1 << 1);


































// ------------------------------------------------------------------------------------------------

  // set required Keys -> Keys that should be there in this request
  parsedKVInput->requiredKVBF = ( (1 << ESP32_SwITCH_SET_GPIO)
			        | (1 << ESP32_SwITCH_SET_BLOCK)
			        | (1 << ESP32_SwITCH_SET_CHANNEL)
			        | (1 << ESP32_SwITCH_SET_TIMER)
			        | (1 << ESP32_SwITCH_SET_DUTY)
			        | (1 << ESP32_SwITCH_SET_HPOINT)
			        | (1 << ESP32_SwITCH_SET_SIG_OUT_EN)
			        | (1 << ESP32_SwITCH_SET_IDLE_LV)
			        | (1 << ESP32_SwITCH_SET_RESOLUTION)
			        | (1 << ESP32_SwITCH_SET_TICK_SOURCE)
			        | (1 << ESP32_SwITCH_SET_FREQ_HZ) );

  // set forbidden Keys -> Keys that are not allowed in this request
  parsedKVInput->forbiddenKVBF = 0;

  // process the set-args (key=value@) protocol
  if (ESP32_SwITCH_ProcessKVInputArgs(ESP32_SwITCH_Definition
	,parsedKVInput				// KVInput parsed
	,defArgsText				// our args text
	,defArgsTextLen) ) {			// our args text len

 	// Processing reports an problem. Args not taken. Response with error text.

	// alloc mem for retMsg
	retMsg = malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Processing Error! Args '%.*s' not taken! Check the VALUEs!"
		,defArgsTextLen
		,defArgsText);

	// free allocated memory for query result key-field
	free(parsedKVInput);

	return retMsg;
  }

// ------------------------------------------------------------------------------------------------

  // set affected readings
  ESP32_SwITCH_SetAffectedReadings(ESP32_SwITCH_Definition
	,parsedKVInput->affectedReadingsBF);

// ---------------------

  // free allocated memory for query result key-field
  free(parsedKVInput);

//  return retMsg;

//}

// ------------------------------------------------------------------------------------------------







  // set up 1st idle Callback
  ESP32_SwITCH_Definition->common.Common_CtrlRegA |= F_WANTS_IDLE_TASK;





/*
  #if ESP32_SwITCH_DBG >= 5

  printf("\n|ESP32_SwITCH_Def, installed pulseIO:%d ctrlIO:%d PCNT_CTRL_REG:%u>"

	,pulse_io

	,ctrl_io
	,*xREG);
  #endif
*/

  return retMsg;

}



/*
 * ------------------------------------------------------------------------------------------------
 *  FName: ESP32_SwITCH_IdleCb	
 *  Desc: Timed callback, X Hz, to do type dependend processing tasks.
 *  Info: 
 *  Para: Common_Definition_t *Common_Definition -> prefilled WebIf Definition 
 *  Rets: ? unclear char* -> response text NULL=no text
 * ------------------------------------------------------------------------------------------------
 */
int //feedModuleTask
ESP32_SwITCH_IdleCb(Common_Definition_t *Common_Definition)
{

  // make common ptr to modul specific ptr
  ESP32_SwITCH_Definition_t* ESP32_SwITCH_Definition =
		  (ESP32_SwITCH_Definition_t*) Common_Definition;

  #if ESP32_SwITCH_DBG >= 5
  printf("\n|ESP32_SwITCH_IdleCb, Def:%.*s>"
	,ESP32_SwITCH_Definition->common.nameLen
	,ESP32_SwITCH_Definition->common.name);
  #endif

// -------------------------------------------------------------------------------------------



  // build pointer to make the hardware counter accessible
  ESP32_PWM_Channel_t *pPWM_Channel =
	(ESP32_PWM_Channel_t*) (0x3FF59000 + (0x14 * ESP32_SwITCH_Definition->Channel));

  // access low speed channels (instead of high speed channels)?
  if (ESP32_SwITCH_Definition->Block) pPWM_Channel =
	(ESP32_PWM_Channel_t*) ( (uint8_t*) pPWM_Channel + 0xa0);


  // build pointer to make the hardware timer accessible
  ESP32_PWM_Timer_t *pPWM_Timer =
	(ESP32_PWM_Timer_t*) (0x3FF59140 + (0x08 * ESP32_SwITCH_Definition->Timer));

  // access low speed timers (instead of high speed timers)?
  if (ESP32_SwITCH_Definition->Block) pPWM_Timer =
	(ESP32_PWM_Timer_t*) ( (uint8_t*) pPWM_Timer + 0x20);

/*
  printf("|STATUS: A:%u, B:%u>"
	,pPWM_Timer->LEDC_HSTIMER_CNT
	,pPWM_Channel->DUTY_CYCLEX);
*/

/*
  // build ptr to access hw-counter
  ESP32_SwITCH_CNT_CNT_REG_t *pESP32_SwITCH_CNT_CNT_REG =
	(ESP32_SwITCH_CNT_CNT_REG_t*) 0x3FF57060;

  printf("|STATUS: Using ctr-unit:%u, old HW-Ctr:%d, new ATT:%u, gpio:%u>"
	,ESP32_SwITCH_Definition->counterUnit
	,pESP32_SwITCH_CNT_CNT_REG[ESP32_SwITCH_Definition->counterUnit].CNT_PULS_CNT
	,ESP32_SwITCH_Definition->ATTShiftBuf[0]
	,gpio_get_level(22) ); //=att ?


  ESP32_SwITCH_CNT_CONF_REG_t *pESP32_SwITCH_CNT_CONF_REG = (ESP32_SwITCH_CNT_CONF_REG_t*) 0x3FF57000;

	SCDEFn->HexDumpOutFn ("verify:"
		,&pESP32_SwITCH_CNT_CONF_REG[0]
		, 0x80);  //sizeof(0x80) ); //ESP32_SwITCH_CNT_CONF_REG_t) );
*/
/*
 if (WebIf_Definition->reverse) {


  // get assigned HTTPD-Connection-Slot-Data
  WebIf_HTTPDConnSlotData_t *conn
	= WebIf_Definition->reverse;

  // execute disconnection (indicated by NEEDS_CLOSE flag) or send more data ...
  if (conn->ConnCtrlFlags & F_GENERATE_IDLE_CALLBACK)

	{

	// execute Idle Callback
	WebIf_IdleCb(WebIf_Definition);

	}

}
*/


  // set up next idle Callback
  ESP32_SwITCH_Definition->common.Common_CtrlRegA |= F_WANTS_IDLE_TASK;

  return 0;

}



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_SwITCH_Initialize
 *  Desc: Initializion of SCDE Function Callbacks of an new loaded module
 *  Info: Stores Module-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
ESP32_SwITCH_Initialize(SCDERoot_t* SCDERootptr)
{

  // make data root locally available
  SCDERoot = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn = SCDERootptr->SCDEFn;

  SCDEFn->Log3Fn(ESP32_SwITCH_ProvidedByModule.typeName
		  ,ESP32_SwITCH_ProvidedByModule.typeNameLen
		  ,3
		  ,"InitializeFn called. Type '%.*s' now useable.\n"
		  ,ESP32_SwITCH_ProvidedByModule.typeNameLen
		  ,ESP32_SwITCH_ProvidedByModule.typeName);

  return 0;

}



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_SwITCH_Set
 *  Desc: Processes the device-specific command line arguments from the set command
 *  Info: Invoked by cmd-line 'Set ESP32Control_Definition.common.Name setArgs'
 *  Para: ESP32Control_Definition_t *ESP32Control_Definition -> WebIF Definition that should get a set cmd
 *        uint8_t *setArgsText -> the setArgsText
 *        size_t setArgsTextLen -> length of the setArgsText
 *  Rets: strTextMultiple_t* -> response text in allocated memory, NULL=no text
 * -------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* ICACHE_FLASH_ATTR
ESP32_SwITCH_Set(Common_Definition_t* Common_Definition
	,uint8_t *setArgsText
	,size_t setArgsTextLen)
{

  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // make common ptr to modul specific ptr
  ESP32_SwITCH_Definition_t* ESP32_SwITCH_Definition =
	(ESP32_SwITCH_Definition_t*) Common_Definition;

  #if ESP32_SwITCH_DBG >= 5
  printf("\n|ESP32_SwITCH_Set, Name:%.*s, got args:%.*s>"
	,ESP32_SwITCH_Definition->common.nameLen
	,ESP32_SwITCH_Definition->common.name
	,setArgsTextLen
	,setArgsText);
  #endif

// ------------------------------------------------------------------------------------------------

  // note: _Set Fn call not allowed with setArgsTextLen = 0 (CHECK!)

  // setArgsText '?' -> respond with help
  if ( (setArgsTextLen > 0) && (*setArgsText == '?') ) {

	// set start of possible Type-Name
	const uint8_t *tempTxt = setArgsText + 1;

	// a seek-counter
	int i = 1;

	// skip spaces after '?' (search for more ...)
	while( (i < setArgsTextLen) && (*tempTxt == ' ') ) {i++;tempTxt++;}

	// only spaces after '?' -> answer with Set-Capabilities
	if (i == setArgsTextLen) {

		// response with error text
		// alloc mem for retMsg
		retMsg = malloc(sizeof(strTextMultiple_t));

		// response with error text
		retMsg->strTextLen = asprintf(&retMsg->strText
			,"requested ? '%.*s' !"
			,setArgsTextLen
			,setArgsText);

		return retMsg;

		}
	// '? + X' here !!! -> normal parse
	}

// ------------------------------------------------------------------------------------------------

  // Parse set-args (KEY=VALUE) protocol -> gets parsedKVInput in allocated mem, NULL = ERROR
  parsedKVInputArgs_t *parsedKVInput = 
	SCDEFn->ParseKVInputArgsFn(ESP32_SwITCH_SET_IK_Number_of_keys	// Num Implementated KEYs MAX for Set Fn
	,ESP32_SwITCH_Set_ImplementedKeys		// Implementated Keys for Set Fn
	,setArgsText				// our args text
	,setArgsTextLen);			// our args text len

  // parsing may report an problem. args contain: unknown keys, double keys, ...?
  if (!parsedKVInput) {

	// alloc mem for retMsg
	retMsg = malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Parsing Error! Args '%.*s' not taken! Check the KEYs!"
		,setArgsTextLen
		,setArgsText);

	return retMsg;
  }

// ------------------------------------------------------------------------------------------------

  // set required Keys -> Keys that should be there in this request
  parsedKVInput->requiredKVBF = 0;

  // set forbidden Keys -> Keys that are not allowed in this request
//  parsedKVInput->forbiddenKVBF = ( (1 << ESP32_SwITCH_SET_CNT_UNIT) );
  parsedKVInput->forbiddenKVBF = 0;

  // process the set-args (key=value@) protocol
  if (ESP32_SwITCH_ProcessKVInputArgs(ESP32_SwITCH_Definition
	,parsedKVInput				// KVInput parsed
	,setArgsText				// our args text
	,setArgsTextLen) ) {			// our args text len

 	// Processing reports an problem. Args not taken. Response with error text.

	// alloc mem for retMsg
	retMsg = malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Processing Error! Args '%.*s' not taken! Check the VALUEs!"
		,setArgsTextLen
		,setArgsText);

	// free allocated memory for query result key-field
	free(parsedKVInput);

	return retMsg;
  }

// ------------------------------------------------------------------------------------------------

  // set affected readings
  ESP32_SwITCH_SetAffectedReadings(ESP32_SwITCH_Definition
	,parsedKVInput->affectedReadingsBF);

// ---------------------

  // free allocated memory for query result key-field
  free(parsedKVInput);

  return retMsg;

}



/**
 * --------------------------------------------------------------------------------------------------
 *  FName: ESP32_SwITCH_Shutdown
 *  Desc: called after 
 *  Info: Invoked by cmd-line 'Undefine ESP32_SwITCH_Definition.common.Name'
 *  Para: ESP32_SwITCH_Definition_t *ESP32_SwITCH_Definition -> WebIF Definition that should be removed
 *  Rets: strTextMultiple_t* -> response text NULL=no text
 * --------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* ICACHE_FLASH_ATTR
ESP32_SwITCH_Shutdown(Common_Definition_t *Common_Definition)
{

  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // make common ptr to modul specific ptr
  ESP32_SwITCH_Definition_t* ESP32_SwITCH_Definition =
	(ESP32_SwITCH_Definition_t*) Common_Definition;

// -------------------------------------------------------------------------------------------------

  #if ESP32_SwITCH_DBG >= 5
  printf("\n|ESP32_SwITCH_Shutdown, Name:%.*s>"
	,ESP32_SwITCH_Definition->common.nameLen
	,ESP32_SwITCH_Definition->common.name);
  #endif

// -------------------------------------------------------------------------------------------------

  return retMsg;

}



/**
 * --------------------------------------------------------------------------------------------------
 *  FName: ESP32_SwITCH_Undefine
 *  Desc: Removes the define of an "device" of 'WebIF' type. Contains devicespecific init code.
 *  Info: Invoked by cmd-line 'Undefine ESP32Control_Definition.common.Name'
 *  Para: ESP32Control_Definition_t *ESP32Control_Definition -> WebIF Definition that should be removed
 *  Rets: strTextMultiple_t* -> response text NULL=no text
 * --------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* ICACHE_FLASH_ATTR
ESP32_SwITCH_Undefine(Common_Definition_t *Common_Definition)
{

  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // make common ptr to modul specific ptr
  ESP32_SwITCH_Definition_t* ESP32_SwITCH_Definition =
	(ESP32_SwITCH_Definition_t*) Common_Definition;

  #if ESP32_SwITCH_DBG >= 5
  printf("\n|ESP32_SwITCH_Undefine, Name:%.*s>"
	,ESP32_SwITCH_Definition->common.nameLen
	,ESP32_SwITCH_Definition->common.name);

  #endif


  // response with error text
	// alloc mem for retMsg
  retMsg = malloc(sizeof(strTextMultiple_t));

  // response with error text
  retMsg->strTextLen = asprintf(&retMsg->strText
	,"ESP32_SwITCH_Undefine, Name:%.*s"
	,ESP32_SwITCH_Definition->common.nameLen
	,ESP32_SwITCH_Definition->common.name);

  return retMsg;

}



/*
 * -------------------  helpers provided to module for type operation ------------------------------
 */



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_SwITCH_ProcessKVInputArgs
 *  Desc: Processes the KEY=VALUE input from Args. Information is pre-parsed to *parsedKVInput
 *  Info: .requiredKV -> Keys that should be there / .forbiddenKV -> Keys that are not allowed
 *  Para: ESP32_SwITCH_Definition_t *ESP32_SwITCH_Definition -> Definition that should get the input
 *        parsedKVInputArgs_t *parsedKVInput -> ptr to allocated mem filled with results from parse
 *        uint8_t *argsText -> the ArgsText
 *        size_t argsTextLen -> length of the ArgsText
 *  Rets: Bool -> TRUE = input parsed successful, FALSE = input parsed, but NOT successful, CANCELED
 * -------------------------------------------------------------------------------------------------
 */
bool ICACHE_FLASH_ATTR
ESP32_SwITCH_ProcessKVInputArgs(ESP32_SwITCH_Definition_t *ESP32_SwITCH_Definition
	,parsedKVInputArgs_t *parsedKVInput
	,uint8_t *argsText
	,size_t argsTextLen)
{

// 1. Step: Prepare structures with current values from TYPE & SYSTEM 
//          (to allow an abort if values not complete or in case of processing errors)

  // block #1 get current assigned PWM-Block (low speed / high speed)
  uint8_t newBlock = ESP32_SwITCH_Definition->Block;

  // block #2 get current assigned PWM Channel
  uint8_t newChannel = ESP32_SwITCH_Definition->Channel;

  // block #3 get current assigned PWM Timer
  uint8_t newTimer = ESP32_SwITCH_Definition->Timer;

// ------------------------------------------------------------------------------------------------
  
  // we need to process the 'Block' KEY in advance here, because we need this information early

  // Block=High_Speed|Low_Speed
  // -> Setzt den Hardware-PWM-Block des ESP32 (0=HighSpeed / 1= LowSpeed) der verwendet werden soll
  // -> Sets the hardware-PWM-Block of the ESP32 (0=HighSpeed / 1= LowSpeed) that should be used
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SwITCH_SET_BLOCK) ) {

	// valid input happened ?
	if (SCDEH_GetQueryKeyID((uint8_t*) argsText + parsedKVInput->keyData_t[ESP32_SwITCH_SET_BLOCK].off
		,parsedKVInput->keyData_t[ESP32_SwITCH_SET_BLOCK].len, &newBlock, ESP32_SwITCH_PWM_BLOCK)
		&& (newBlock <= 1) ) {
/*

		// save new value, from input
		SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].ATTShiftBuf[0]
			= NewVal;

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_att].affectedReadings;


		// New Timestamp
		SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=att");


*/
		}

	// invalid input detected ?
	else return true;
  }

// ------------------------------------------------------------------------------------------------
  
  // we need to process the 'CHANNEL' KEY in advance here, because we need this information early

  // CHANNEL=[0-7]
  // -> Setzt den Hardware-PWM-Kanal des ESP32 (0-7) der verwendet werden soll
  // -> Sets the hardware-pwm-channel of the ESP32 (0-7) that should be used
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SwITCH_SET_CHANNEL) ) {

	// valid input happened ?
	if (SCDEH_GetDecUInt8Val((uint8_t*) argsText + parsedKVInput->keyData_t[ESP32_SwITCH_SET_CHANNEL].off
		,parsedKVInput->keyData_t[ESP32_SwITCH_SET_CHANNEL].len, &newChannel) 
		&& (newChannel <= 7) ) {

/*

		// save new value, from input
		SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].ATTShiftBuf[0]
			= NewVal;

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_att].affectedReadings;


		// New Timestamp
		SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=att");


*/
		}

	// invalid input detected ?
	else return true;
  }

// ------------------------------------------------------------------------------------------------
  
  // we need to process the 'TIMER' KEY in advance here, because we need this information early

  // TIMER=[0-3]
  // -> Setzt den Hardware-Timer für den gewählten PWM-Kanal des ESP32 (0-3) der verwendet werden soll
  // -> Sets the hardware-timer for the choosen pwm-channel of the ESP32 (0-3) that should be used
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SwITCH_SET_TIMER) ) {

	// valid input happened ?
	if (SCDEH_GetDecUInt8Val((uint8_t*) argsText + parsedKVInput->keyData_t[ESP32_SwITCH_SET_TIMER].off
		,parsedKVInput->keyData_t[ESP32_SwITCH_SET_TIMER].len, &newTimer) 
		&& (newTimer <= 3) ) {

/*

		// save new value, from input
		SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].ATTShiftBuf[0]
			= NewVal;

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_att].affectedReadings;


		// New Timestamp
		SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=att");


*/
		}

	// invalid input detected ?
	else return true;
  }

// ------------------------------------------------------------------------------------------------

  // block #4 get current assigned / connected GPIO
  uint8_t newGPIO = ESP32_SwITCH_Definition->GPIO;

  // block #5 reserve for new ATT
  uint32_t newATTVal;

  // block #6 reserve for new AVU
  char newAverageUnit[3];	

  // block #7  get ESP32 PWM channel configuration from selected channel
  // this stores our temp values
  ESP32_PWM_Channel_t newPWM_Channel;
  // pointer to make the hardware counters accessible
  ESP32_PWM_Channel_t *pPWM_Channel =
	(ESP32_PWM_Channel_t*) (0x3FF59000 + (0x14 * newChannel));
  // access low speed channels (instead of high speed channels)?
  if (newBlock) pPWM_Channel =
	(ESP32_PWM_Channel_t*) ( (uint8_t*) pPWM_Channel + 0xa0);
  // move data to temp storage
  memcpy(&newPWM_Channel, pPWM_Channel, sizeof(ESP32_PWM_Channel_t));

  // block #8  get ESP32 PWM timer configuration from selected timer
  // this stores our temp values
  ESP32_PWM_Timer_t newPWM_Timer;
  // pointer to make the hardware timers accessible
  ESP32_PWM_Timer_t *pPWM_Timer =
	(ESP32_PWM_Timer_t*) (0x3FF59140 + (0x08 * newTimer));
  // access low speed timers (instead of high speed timers)?
  if (newBlock) pPWM_Timer =
	(ESP32_PWM_Timer_t*) ( (uint8_t*) pPWM_Timer + 0x20);
  // move data to temp storage
  memcpy(&newPWM_Timer, pPWM_Timer, sizeof(ESP32_PWM_Timer_t));


  // store choosen timer in choosen channel
  newPWM_Channel.TIMER_SEL = newTimer;

// ------------------------------------------------------------------------------------------------

// 2. Step: Process the possible input keys













/*
//>Caps Bit 0---------------------------------------------------------------------------------------

  // on=[ ] -> Setze Kanal an (0-x)
  // on=[ ] -> Set channel on (0-x)

 if (QueryResultKF->KeyField_Set == (uint64_t) 1 << SwITCH_QF_on)

	{

	// new val, save mirror
	SCDE_WriteUInt32toSysCfgRamRtcMirrored(&SysCfgRamRtcMirrored->MySwITCH_FeatCfgRamRtcMirrored[ADID].RB01_Val,
		SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[ADID].IB01_SwValMax);

	// mark affected readings for TX
	AffectedReadings |= QueryResultKF->KeyField_Data_t[SwITCH_QF_on].AffectedReadings;

	// New Timestamp
	SysCfgRamNoMirror->MySwITCH_FeatCfgRamNoMirror[ADID].RB01_X_TiSt =
		GetUniqueTIST();

	// Push processed CMD to to Response ...
	RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=on");

	}

//--------------------------------------------------------------------------------------------------

  // off=[ ] -> Setze Kanal aus (0-x)
  // off=[ ] -> Set channel off (0-x)

  else if (QueryResultKF->KeyField_Set == (uint64_t) 1 << SwITCH_QF_off)

	{

	// new val, save mirror
	SCDE_WriteUInt32toSysCfgRamRtcMirrored(&SysCfgRamRtcMirrored->MySwITCH_FeatCfgRamRtcMirrored[ADID].RB01_Val,
		SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[ADID].IB01_SwValMin);

	// mark affected readings for TX
	AffectedReadings |= QueryResultKF->KeyField_Data_t[SwITCH_QF_off].AffectedReadings;

	// New Timestamp
	SysCfgRamNoMirror->MySwITCH_FeatCfgRamNoMirror[ADID].RB01_X_TiSt =
		GetUniqueTIST();

	// Push processed CMD to to Response ...
	RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=off");

	}

//>Caps Bit 1---------------------------------------------------------------------------------------

  // max=[ ] -> Setze Kanal maximal (0-x)
  // max=[ ] -> Set channel maximum (0-x) 

  else if (QueryResultKF->KeyField_Set == (uint64_t) 1 << SwITCH_QF_max)

	{

	// new val, save mirror
	SCDE_WriteUInt32toSysCfgRamRtcMirrored(&SysCfgRamRtcMirrored->MySwITCH_FeatCfgRamRtcMirrored[ADID].RB01_Val,
		SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[ADID].IB01_SwValMax);

	// mark affected readings for TX
	AffectedReadings |= QueryResultKF->KeyField_Data_t[SwITCH_QF_max].AffectedReadings;

	// New Timestamp
	SysCfgRamNoMirror->MySwITCH_FeatCfgRamNoMirror[ADID].RB01_X_TiSt =
		GetUniqueTIST();

	// Push processed CMD to to Response ...
	RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=max");

	}

//--------------------------------------------------------------------------------------------------

  // min=[ ] -> Setze Kanal minimal (0-x)
  // min=[ ] -> Set channel minimal (0-x)

  else if (QueryResultKF->KeyField_Set == (uint64_t) 1 << SwITCH_QF_min)

	{

	// New Value = Min
	SCDE_WriteUInt32toSysCfgRamRtcMirrored(&SysCfgRamRtcMirrored->MySwITCH_FeatCfgRamRtcMirrored[ADID].RB01_Val,
		SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[ADID].IB01_SwValMin);

	// mark affected readings for TX
	AffectedReadings |= QueryResultKF->KeyField_Data_t[SwITCH_QF_min].AffectedReadings;

	// New Timestamp
	SysCfgRamNoMirror->MySwITCH_FeatCfgRamNoMirror[ADID].RB01_X_TiSt =
		GetUniqueTIST();

	// Push processed CMD to to Response ...
	RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=min");

	}

//>Caps Bit 2---------------------------------------------------------------------------------------

  // val=[0-9] -> Setze Wert für Kanal (uint8) (0-x)
  // val=[0-9] -> Set value for channel (uint8) (0-x)

 else if (QueryResultKF->KeyField_Set == (uint64_t) 1 << SwITCH_QF_val)
	{

	uint32_t NewVal;

	// valid input happened ?
	if (SCDEH_GetDecUInt32Val((uint8*)Args+QueryResultKF->KeyField_Data_t[SwITCH_QF_val].off
		,QueryResultKF->KeyField_Data_t[SwITCH_QF_val].len, &NewVal))

		{

		// val input value clipping
		if (NewVal > SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[ADID].IB01_SwValMax)

			{

			NewVal = SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[ADID].IB01_SwValMax;

			}

		else if (NewVal < SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[ADID].IB01_SwValMin)

			{

			NewVal = SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[ADID].IB01_SwValMin;

			}

		// save mirror
		SCDE_WriteUInt32toSysCfgRamRtcMirrored(&SysCfgRamRtcMirrored->MySwITCH_FeatCfgRamRtcMirrored[ADID].RB01_Val,
			NewVal);

		// mark affected readings for TX
		AffectedReadings |= QueryResultKF->KeyField_Data_t[SwITCH_QF_val].AffectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MySwITCH_FeatCfgRamNoMirror[ADID].RB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=val");

		}

	}

//>Caps Bit 3---------------------------------------------------------------------------------------

  // dim=[0-9] -> Setze dimmer für Kanal (uint8) (0-x)
  // dim=[0-9] -> Set dimming for channel (uint8) (0-x)

  else if (QueryResultKF->KeyField_Set == (uint64_t) 1 << SwITCH_QF_dim)
	{

	uint32_t NewVal;

	// valid input happened ?
	if (SCDEH_GetDecUInt32Val((uint8*)Args+QueryResultKF->KeyField_Data_t[SwITCH_QF_dim].off
		,QueryResultKF->KeyField_Data_t[SwITCH_QF_dim].len, &NewVal))
		
		{

		// dim input value clipping
		if (NewVal > 100) NewVal = 100;
		else if (NewVal < 0) NewVal = 0;

		// calculate val from dim
		NewVal = ( (NewVal *
			(SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[ADID].IB01_SwValMax - 
			SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[ADID].IB01_SwValMin) ) /
			100 );

		// New Value = calculated from input
		SCDE_WriteUInt32toSysCfgRamRtcMirrored(&SysCfgRamRtcMirrored->MySwITCH_FeatCfgRamRtcMirrored[ADID].RB01_Val,
			NewVal);

		// save mirror
		AffectedReadings |= QueryResultKF->KeyField_Data_t[SwITCH_QF_dim].AffectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MySwITCH_FeatCfgRamNoMirror[ADID].RB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=dim");

		}

	}

//>Caps Bit n.a.------------------------------------------------------------------------------------

  // name=[a-zA-Z0-9_.] -> Setze einen neuen Feature Namen (char[31]) (0-x)
  // name=[a-zA-Z0-9_.] -> Set a new Feature Name (char[31]) (0-x)

  else if (QueryResultKF->KeyField_Set == (uint64_t) 1 << SwITCH_QF_name)

	{

	// valid input happened ?
	if (SCDEH_GetSpecialStrVal((uint8_t*)Args+QueryResultKF->KeyField_Data_t[SwITCH_QF_name].off
		,QueryResultKF->KeyField_Data_t[SwITCH_QF_name].len
		,(char*) &SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[ADID].IB01_Name
		,sizeof (SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[ADID].IB01_Name)
		,2) )

		{	

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		AffectedReadings |= QueryResultKF->KeyField_Data_t[SwITCH_QF_name].AffectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MySwITCH_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=name");

		}

	}

//>Caps Bit n.a.------------------------------------------------------------------------------------

  // caps=[ ] -> Fragt nach den Fähigkeiten-Bits (0-x)
  // caps=[ ] -> Request for Capabilities-Bits (0-x)

 if (QueryResultKF->KeyField_Set == (uint64_t) 1 << SwITCH_QF_caps)

	{

	// mark affected readings for TX
	AffectedReadings |= QueryResultKF->KeyField_Data_t[SwITCH_QF_caps].AffectedReadings;

	// New Timestamp
	SysCfgRamNoMirror->MySwITCH_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
		GetUniqueTIST();

	// Push processed CMD to to Response ...
	RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=on");

	}

*/

/*
  // ATT=[0-9]
  // -> Setze Allzeit-Gesamt-Impulse passend zum aktuellen Zählerstand
  // -> Set alltime-total-tics meeting the currend meter-reading

  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SwITCH_SET_ATT) ) {

	// valid input happened ?
	if (SCDEH_GetDecUInt32Val((uint8*)Args+QueryResultKF->KeyField_Data_t[S0_QF_att].off
		,QueryResultKF->KeyField_Data_t[S0_QF_att].len, &newATTVal)) {





		// save new value, from input
		SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].ATTShiftBuf[0]
			= newATTVal;

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_att].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=att");

		}

	// invalid input detected ?
	else return true;
  }

//>Caps Bit 1,2,3,4,5,6-----------------------------------------------------------------------------

  // AVU=[a-zA-Z0-9_.]
  // -> Setze Einheit für den Durchschnitts-Verbrauchs-Wert (char[3])
  // -> Set unit for average-consumption-value (char[3])

  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SwITCH_SET_AVU) ) {

	// valid input happened ?
	if (SCDEH_GetSpecialStrVal((uint8_t*)Args+QueryResultKF->KeyField_Data_t[S0_QF_avu].off
		,QueryResultKF->KeyField_Data_t[S0_QF_avu].len
		,(char*) &newAverageUnit
		,sizeof (newAverageUnit) // -> case 3 = (Alpha+Num+"³") for Unit Name
		,3) ) {


		// SAVE Value
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_avu].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=avu");

		}

	// invalid input detected ?
	else return true;
  }

//>Caps Bit 1---------------------------------------------------------------------------------------

  // AVF=[0-9.]
  // -> Setze Durchschnittsverbrauch Berechnungsfaktor (float)
  // -> Set average consumption calculation factor (float)

  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SwITCH_SET_AVF) ) {

	double NewVal;

	// valid input happened ?
	if (SCDEH_GetFloatVal((uint8*)Args+QueryResultKF->KeyField_Data_t[S0_QF_avf].off
		,QueryResultKF->KeyField_Data_t[S0_QF_avf].len, &NewVal)) {

		// store the value to temp
		//befehl hier!;


		// save new value, from input
		SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].IB01_AverageFactor
			= NewVal;

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_avf].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=avf");

		}

	// invalid input detected ?
	else return true;
  }

//--------------------------------------------------------------------------------------------------

  // ATU=[a-zA-Z0-9_.]
  // -> Setze Einheit für den Allzeit-Gesamt-Wert (char[3])
  // -> Set unit for alltime-total-value (char[3])

  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SwITCH_SET_ATU) ) {

	// valid input happened ?
	if (SCDEH_GetSpecialStrVal((uint8_t*)Args+QueryResultKF->KeyField_Data_t[S0_QF_atu].off
		,QueryResultKF->KeyField_Data_t[S0_QF_atu].len
		,(char*) &SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].IB01_AlltimeTotalUnit
		,sizeof (SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].IB01_AlltimeTotalUnit)
		,3) ) // -> case 3 = (Alpha+Num+"³") for Unit Name

		 {

		// store the value to temp
		//befehl hier!;



		// SAVE Value
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_atu].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=atu");

		}

	// invalid input detected ?
	else return true;
  }

//>Caps Bit 2---------------------------------------------------------------------------------------

  // ATF=[0-9.]
  // -> Setze Umrechnungsfaktor für den Allzeit-Total-Wert (float)
  // -> Set factor for alltime total value (float)

  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SwITCH_SET_ATF) ) {

	double NewVal;

	// valid input happened ?
	if (SCDEH_GetFloatVal((uint8*)Args+QueryResultKF->KeyField_Data_t[S0_QF_atf].off
		,QueryResultKF->KeyField_Data_t[S0_QF_atf].len, &NewVal)) {

		// store the value to temp
		//befehl hier!;



		// save new value, from input
		SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].IB01_AlltimeTotalFactor
			= NewVal;

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_atf].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=atf");

		}

	// invalid input detected ?
	else return true;
  }


*/














//> ------------------------------------------------------------------------------------------------
/*
  // CH1_LCTRL_MODE=no_modification|invert|inhibit_modification
  // -> ?? Setzt den Hardware-Counter des ESP32 (0-7) der verwendet werden soll
  // -> ?? Sets the hardware counter of the ESP32 (0-7) that should be used
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SwITCH_SET_CH1_LCTRL_MODE) ) {

	uint8_t newMode;

	// valid input happened ?
	if (SCDEH_GetQueryKeyID((uint8_t*) argsText + parsedKVInput->keyData_t[ESP32_SwITCH_SET_CH1_LCTRL_MODE].off
		,parsedKVInput->keyData_t[ESP32_SwITCH_SET_CH1_LCTRL_MODE].len, &newMode, &ESP32_SwITCH_CTRL_MODE)
		&& (newMode <= 3) ) {

		// store the value to temp
		ESP32_SwITCH_CNT_CONF_REG.CNT_CH1_LCTRL_MODE = newMode;



		// save new value, from input
		SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].ATTShiftBuf[0]
			= NewVal;

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_att].affectedReadings;


		// New Timestamp
		SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=att");


		}

	// invalid input detected ?
	else return true;
  }
*/





















/*

//>Caps Bit 3---------------------------------------------------------------------------------------

  // deb=[0-9] -> Setze debounce Wert, 10-200 recommended (0-x)
  // deb=[0-9] -> Set debounce value, 10-200 recommended (0-x)

  else if (QueryResultKF->KeyField_Set == (uint64_t) 1 << S0_QF_deb)

	{

	uint32_t NewVal;

	// valid input happened ?
	if (SCDEH_GetDecUInt32Val((uint8*)Args+QueryResultKF->KeyField_Data_t[S0_QF_deb].off
		,QueryResultKF->KeyField_Data_t[S0_QF_deb].len, &NewVal))

		{

		// deb input value clipping
		if (NewVal > 1000) NewVal = 1000;
		else if (NewVal < 0) NewVal = 10;

		// save new value, from input
		SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].IB01_Debounce
			= NewVal;

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_deb].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=deb");

		}

	}


//>Caps Bit n.a.------------------------------------------------------------------------------------

  // name=[a-zA-Z0-9_.] -> Setze einen neuen Feature Namen (char[31]) (0-x)
  // name=[a-zA-Z0-9_.] -> Set a new Feature Name (char[31]) (0-x)

  else if (QueryResultKF->KeyField_Set == (uint64_t) 1 << S0_QF_name)

	{

	// valid input happened ?
	if (SCDEH_GetSpecialStrVal((uint8_t*)Args+QueryResultKF->KeyField_Data_t[S0_QF_name].off
		,QueryResultKF->KeyField_Data_t[S0_QF_name].len
		,(char*) &SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].IB01_Name
		,sizeof (SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].IB01_Name)
		,2) ) // NAME=[a-zA-Z0-9_.] -> Set a new Feature Name (char[31]) (0-x)

		{	

		// SAVE Value
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_name].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=name");

		}

	}

//>Caps Bit n.a.------------------------------------------------------------------------------------

  // caps=[ ] -> Fragt nach den Fähigkeiten-Bits (0-x)
  // caps=[ ] -> Request for Capabilities-Bits (0-x)


 if (QueryResultKF->KeyField_Set == (uint64_t) 1 << S0_QF_caps)
	{

	// mark affected readings for TX
	affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_caps].affectedReadings;

//	// New Timestamp
//	WriteInt32toSysCfgRamRtcMirrored(&SysCfgRamRtcMirrored->MyS0FeatureCfgRamRtcMirrored[ADID].IB01_X_TiSt,
//		GetUniqueTIST());

	// Push processed CMD to to Response ...
	RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=caps");

	}

*/








//> ------------------------------------------------------------------------------------------------
/*
  // CH1_LCTRL_MODE=no_modification|invert|inhibit_modification
  // -> ?? Setzt den Hardware-Counter des ESP32 (0-7) der verwendet werden soll
  // -> ?? Sets the hardware counter of the ESP32 (0-7) that should be used
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SwITCH_SET_CH1_LCTRL_MODE) ) {

	uint8_t newMode;

	// valid input happened ?
	if (SCDEH_GetQueryKeyID((uint8_t*) argsText + parsedKVInput->keyData_t[ESP32_SwITCH_SET_CH1_LCTRL_MODE].off
		,parsedKVInput->keyData_t[ESP32_SwITCH_SET_CH1_LCTRL_MODE].len, &newMode, &ESP32_SwITCH_CTRL_MODE)
		&& (newMode <= 3) ) {

		// store the value to temp
		ESP32_SwITCH_CNT_CONF_REG.CNT_CH1_LCTRL_MODE = newMode;



		// save new value, from input
		SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].ATTShiftBuf[0]
			= NewVal;

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_att].affectedReadings;


		// New Timestamp
		SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=att");


		}

	// invalid input detected ?
	else return true;
  }
*/
//> ------------------------------------------------------------------------------------------------
/*
  // CH1_HCTRL_MODE=no_modification|invert|inhibit_modification
  // -> ?? Setzt den Hardware-Counter des ESP32 (0-7) der verwendet werden soll
  // -> ?? Sets the hardware counter of the ESP32 (0-7) that should be used
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SwITCH_SET_CH1_HCTRL_MODE) ) {

	uint8_t NewMode;

	// valid input happened ?
	if (SCDEH_GetQueryKeyID((uint8_t*) argsText + parsedKVInput->keyData_t[ESP32_SwITCH_SET_CH1_HCTRL_MODE].off
		,parsedKVInput->keyData_t[ESP32_SwITCH_SET_CH1_HCTRL_MODE].len, &NewMode, ESP32_SwITCH_CTRL_MODE) ) {

		// store ne value
		ESP32_SwITCH_CNT_CONF_REG.CNT_CH1_LCTRL_MODE = NewMode;


		// save new value, from input
		SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].ATTShiftBuf[0]
			= NewVal;

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_att].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=att");


		}

	// invalid input detected ?
	else return true;
  }
*/
//> ------------------------------------------------------------------------------------------------
/*
  // CH1_POS_MODE=no_effect|inc_counter|dec_counter
  // -> ?? Setzt den Hardware-Counter des ESP32 (0-7) der verwendet werden soll
  // -> ?? Sets the hardware counter of the ESP32 (0-7) that should be used
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SwITCH_SET_CH1_POS_MODE) ) {

	uint8_t NewMode;

	// valid input happened ?
	if (SCDEH_GetQueryKeyID((uint8_t*) argsText + parsedKVInput->keyData_t[ESP32_SwITCH_SET_CH1_POS_MODE].off
		,parsedKVInput->keyData_t[ESP32_SwITCH_SET_CH1_POS_MODE].len, &NewMode, ESP32_SwITCH_CH_MODE) ) {

		// store ne value
		ESP32_SwITCH_CNT_CONF_REG.CNT_CH1_POS_MODE = NewMode;


		// save new value, from input
		SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].ATTShiftBuf[0]
			= NewVal;

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_att].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=att");


		}

	// invalid input detected ?
	else return true;
  }
*/







// ------------------------------------------------------------------------------------------------

  // GPIO=[0-9]
  // -> Verbindet den PWM-Kanal mit dem angegebenen GPIO (inklusive der Konfiguration)
  // -> Connects the PWM-Channel with the given GPIO (including the setup)
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SwITCH_SET_GPIO) ) {

	// valid input happened ?
	if (SCDEH_GetDecUInt8Val((uint8_t*) argsText + parsedKVInput->keyData_t[ESP32_SwITCH_SET_GPIO].off
		,parsedKVInput->keyData_t[ESP32_SwITCH_SET_GPIO].len, &newGPIO)
		&& (newGPIO <= 33) ) {
/*
		// save new value, from input
		SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].ATTShiftBuf[0]
			= NewVal;

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_att].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=att");

*/
		}

	// invalid input detected ?
	else return true;
  }

// ------------------------------------------------------------------------------------------------

  // DUTY=[0-9]
  // -> Setzt die Leistung der PWM für den zugewiesenen Kanal
  // -> Sets the duty of the PWM for the assigned channel
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SwITCH_SET_DUTY) ) {

	uint32_t newDuty;

	// valid input happened ?
	if (SCDEH_GetDecUInt32Val((uint8_t*) argsText + parsedKVInput->keyData_t[ESP32_SwITCH_SET_DUTY].off
		,parsedKVInput->keyData_t[ESP32_SwITCH_SET_DUTY].len, &newDuty)
		&& (newDuty <= 0b111111111111111111111) ) {

		// store ne value
		newPWM_Channel.DUTY = newDuty;

/*
		// save new value, from input
		SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].ATTShiftBuf[0]
			= NewVal;

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_att].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=att");

*/
		}

	// invalid input detected ?
	else return true;
  }

// ------------------------------------------------------------------------------------------------

  // PWM_HPOINT=[0-9]
  // -> Setzt den Einschaltpunkt (High-Level-Point) der PWM für den zugewiesenen Kanal
  // -> Sets the switch-on-point (High-Level-Point) for the assigned channel
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SwITCH_SET_HPOINT) ) {

	uint32_t newHPoint;

	// valid input happened ?
	if (SCDEH_GetDecUInt32Val((uint8_t*) argsText + parsedKVInput->keyData_t[ESP32_SwITCH_SET_HPOINT].off
		,parsedKVInput->keyData_t[ESP32_SwITCH_SET_HPOINT].len, &newHPoint)
		&& (newHPoint <= 0b11111111111111111111) ) {

		// store ne value
		newPWM_Channel.HPOINT = newHPoint;

/*
		// save new value, from input
		SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].ATTShiftBuf[0]
			= NewVal;

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_att].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=att");

*/
		}

	// invalid input detected ?
	else return true;
  }

// ------------------------------------------------------------------------------------------------

  // SIG_OUT_EN=Disabled|Enabled
  // -> Verbindet / schaltete das PWM Signal auf den Ausgang (GPIO) durch. 
  // -> Connects / switches the PWM Signal through the output GPIO
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SwITCH_SET_SIG_OUT_EN) ) {

	uint8_t newSigOutEn;

	// valid input happened ?
	if (SCDEH_GetQueryKeyID((uint8_t*) argsText + parsedKVInput->keyData_t[ESP32_SwITCH_SET_SIG_OUT_EN].off
		,parsedKVInput->keyData_t[ESP32_SwITCH_SET_SIG_OUT_EN].len, &newSigOutEn, ESP32_SwITCH_DisEna) ) {

		// store ne value
		newPWM_Channel.SIG_OUT_EN = newSigOutEn;

/*
		// save new value, from input
		SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].ATTShiftBuf[0]
			= NewVal;

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_att].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=att");

*/
		}

	// invalid input detected ?
	else return true;
  }

// ------------------------------------------------------------------------------------------------

  // IDLE_LV=Low|High
  // -> Bestimmt den Ausgangspegel am GPIO (Low|High) wenn SIG_OUT_EN=Disabled
  // -> Defines the output level at the GPIO (Low|High) when SIG_OUT_EN=Disabled
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SwITCH_SET_IDLE_LV) ) {

	uint8_t newIdleLv;

	// valid input happened ?
	if (SCDEH_GetQueryKeyID((uint8_t*) argsText + parsedKVInput->keyData_t[ESP32_SwITCH_SET_IDLE_LV].off
		,parsedKVInput->keyData_t[ESP32_SwITCH_SET_IDLE_LV].len, &newIdleLv, ESP32_SwITCH_LoHi) ) {

		// store ne value
		newPWM_Channel.IDLE_LV = newIdleLv;

/*
		// save new value, from input
		SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].ATTShiftBuf[0]
			= NewVal;

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_att].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=att");

*/
		}

	// invalid input detected ?
	else return true;
  }

// ------------------------------------------------------------------------------------------------

  // RESOLUTION=[0-9]
  // -> Setzt die Bitbreite (max 20 Bit) des PWM Timers
  // -> Sets the Bit length (max 20 Bit) of the PWM timer
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SwITCH_SET_RESOLUTION) ) {

	uint8_t newTimerLim;

	// valid input happened ?
	if (SCDEH_GetDecUInt8Val((uint8_t*) argsText + parsedKVInput->keyData_t[ESP32_SwITCH_SET_RESOLUTION].off
		,parsedKVInput->keyData_t[ESP32_SwITCH_SET_RESOLUTION].len, &newTimerLim)
		&& (newTimerLim <= 20) ) {

		// store ne value
		newPWM_Timer.RESOLUTION = newTimerLim;

		// store ne value
//		ESP32_SwITCH_CNT_CONF_REG.CNT_THR_THRES1_EN = NewMode;

/*
		// save new value, from input
		SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].ATTShiftBuf[0]
			= NewVal;

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_att].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=att");

*/
		}

	// invalid input detected ?
	else return true;
  }

// ------------------------------------------------------------------------------------------------

  // TICK_SOURCE=REF|APB_SLOW
  // -> Selektiert den Takterzeuger 0: REF_TICK / 1: APB_CLK
  // -> Selects the clock source 0: REF_TICK / 1: APB_CLK 
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SwITCH_SET_TICK_SOURCE) ) {

	uint8_t newTickSelTimer;

	// valid input happened ?
	if (SCDEH_GetQueryKeyID((uint8_t*) argsText + parsedKVInput->keyData_t[ESP32_SwITCH_SET_TICK_SOURCE].off
		,parsedKVInput->keyData_t[ESP32_SwITCH_SET_TICK_SOURCE].len, &newTickSelTimer, ESP32_SwITCH_TICK_SOURCE) ) {

		// store ne value
		newPWM_Timer.TICK_SOURCE = newTickSelTimer;

/*
		// save new value, from input
		SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].ATTShiftBuf[0]
			= NewVal;

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_att].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=att");

*/
		}

	// invalid input detected ?
	else return true;
  }

// ------------------------------------------------------------------------------------------------

  // FREQ_HZ=[0-9]	// NOTE: clc bases on .RESOLUTION, .TICK_SOURCE !! (should be parsed here)
  // -> Konfiguriert den Divisionsfaktor
  // -> Configures the division factor for the divider in high-speed timer from given frequency in HZ
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SwITCH_SET_FREQ_HZ) ) {

	uint32_t newFreqHz;

	// valid input happened ?
	if (SCDEH_GetDecUInt32Val((uint8_t*) argsText + parsedKVInput->keyData_t[ESP32_SwITCH_SET_FREQ_HZ].off
		,parsedKVInput->keyData_t[ESP32_SwITCH_SET_FREQ_HZ].len, &newFreqHz) ) {

		// calculate new divider value
		uint32_t divNum = 0;

		// get the choosen precision
		uint32_t precision = ( 0x1 << newPWM_Timer.RESOLUTION );

		// used timer -> 1: APB_CLK or SLOW_CLK / 0: REF_TICK
		if (newPWM_Timer.TICK_SOURCE) {

			if (newBlock) {

				// low speed block 1 -> clc for SLOW_CLK
				divNum = ( (uint64_t) LEDC_APB_CLK_HZ << 8 ) / newFreqHz / precision;		

			}

			else {

				// high speed block 0 -> clc for APB_CLK
				divNum = ( (uint64_t) LEDC_APB_CLK_HZ << 8 ) / newFreqHz / precision;		

			}

		}

		else {
			// clc for REF_TICK
			divNum = ( (uint64_t) LEDC_REF_CLK_HZ << 8 ) / newFreqHz / precision;


		}

//		// if we have a 0, fract_bits or divNum is > 18 bit -> do not accept it !
//		if (divNum <= 256 || divNum > 0x0003FFFF) {
		// if we have a 0, fract_bits or divNum is > 10+8 bit -> do not accept it !
		if (divNum == 0 || divNum > 0x0003FFFF) {

			#if ESP32_SwITCH_DBG >= 5
			printf("|div 4 timer param err!>");	
  			#endif

			return true;

		}

		#if ESP32_SwITCH_DBG >= 5
		if (divNum <= 255) printf("|Note:div with rest!>");	
		#endif

	// store calculated division factor for the divider in the pwm timer
	newPWM_Timer.DIV_FACTOR = divNum;


/*
		// save new value, from input
		SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].ATTShiftBuf[0]
			= NewVal;

		// save mirror
		SCDE_SaveSysCfgRamFlashMirroredToMirror();

		// mark affected readings for TX
		affectedReadings |= QueryResultKF->KeyField_Data_t[S0_QF_att].affectedReadings;

		// New Timestamp
		SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].IB01_X_TiSt =
			GetUniqueTIST();

		// Push processed CMD to to Response ...
		RespArgsWPos += os_sprintf(RespArgsWPos,"cmd=att");

*/
		}

	// invalid input detected ?
	else return true;
  }

// ------------------------------------------------------------------------------------------------

// 3. Step: Check if the ammount of input elements meets our criteria

  // check if all required Key=Value components are included -> return with FALSE
  if ( (parsedKVInput->keysFoundBF & parsedKVInput->requiredKVBF) != parsedKVInput->requiredKVBF) {

	#if ESP32_SwITCH_DBG >= 5
	printf("|requiredKVBF, no match!>");	
  	#endif

	return true;
  }

  // check if forbidden Key=Value components are included -> return with FALSE
  if (parsedKVInput->keysFoundBF & parsedKVInput->forbiddenKVBF) {

	#if ESP32_SwITCH_DBG >= 5
	printf("|forbiddenKVBF, match!>");	
  	#endif

	return true;
  }

// ------------------------------------------------------------------------------------------------

// 4. Step: Passed. Store the new values

  // block #1 store assigned PWM-Block (low speed / high speed)
  if (parsedKVInput->keysFoundBF | ( (1 << ESP32_SwITCH_SET_BLOCK) ) ) {

	#if ESP32_SwITCH_DBG >= 5
	printf("|W B#01 PWM_Block=%d>"
		,newBlock);	
  	#endif

	// store assigned PWM-Block
	ESP32_SwITCH_Definition->Block = newBlock;
  }

  // block #2 store assigned PWM Channel
  if (parsedKVInput->keysFoundBF | ( (1 << ESP32_SwITCH_SET_CHANNEL) ) ) {

	#if ESP32_SwITCH_DBG >= 5
	printf("|W B#02 PWM_Channel=%d>"
		,newChannel);	
  	#endif

	// store assigned PWM Channel
	ESP32_SwITCH_Definition->Channel = newChannel;
  }

  // block #3 store assigned PWM Timer
  if (parsedKVInput->keysFoundBF | ( (1 << ESP32_SwITCH_SET_TIMER) ) ) {

	#if ESP32_SwITCH_DBG >= 5
	printf("|W B#03 PWM_Timer=%d>"
		,newTimer);	
  	#endif

	// store assigned PWM Timer
	ESP32_SwITCH_Definition->Timer = newTimer;
  }

  // block #4 store assigned / connected GPIO
  if (parsedKVInput->keysFoundBF | ( (1 << ESP32_SwITCH_SET_GPIO) ) ) {

	#if ESP32_SwITCH_DBG >= 5
	printf("|W B#04 PWM_GPIO=%d>"
		,newGPIO);	
  	#endif

	// store assigned GPIO
	ESP32_SwITCH_Definition->GPIO = newGPIO;

	PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[newGPIO], PIN_FUNC_GPIO);

	// set GPIO as output
	gpio_set_direction(newGPIO, GPIO_MODE_OUTPUT);

	// configure for high-speed or low-speed ?
	if (newBlock) {

		// setup gpio matrix for low speed
	}

	else {

		// setup gpio matrix for high speed
		gpio_matrix_out(newGPIO, LEDC_HS_SIG_OUT0_IDX + newChannel, 0, 0);
		
	}
  }

  // block #5 store ESP32 pwm configuration registers
  if (parsedKVInput->keysFoundBF & ( (1 << ESP32_SwITCH_SET_TIMER)
				   | (1 << ESP32_SwITCH_SET_SIG_OUT_EN)
				   | (1 << ESP32_SwITCH_SET_IDLE_LV)
				   | (1 << ESP32_SwITCH_SET_HPOINT)
				   | (1 << ESP32_SwITCH_SET_DUTY) ) ) {

	// reset?
//	newPWM_Channel.CLK_EN = 1;
/*
newPWM_Channel.DUTY_SCALE = 0;
newPWM_Channel.DUTY_CYCLE = 0;
newPWM_Channel.DUTY_NUM = 0;
newPWM_Channel.DUTY_INC = 0;
*/
newPWM_Channel.DUTY_START = 1;



	// store ESP32 PWM configuration registers
	memcpy(pPWM_Channel, &newPWM_Channel, sizeof(ESP32_PWM_Channel_t));

	# if ESP32_SwITCH_DBG >= 5
	printf("|ESP32_PWM_Channel_s addr=%p>"
		,pPWM_Channel);

	SCDEFn->HexDumpOutFn ("dump"
		,pPWM_Channel
		, sizeof(ESP32_PWM_Channel_t) );
	# endif
  }

  // block #6 store ESP32 pwm-timer configuration registers
  if (parsedKVInput->keysFoundBF & ( (1 << ESP32_SwITCH_SET_TIMER)
				   | (1 << ESP32_SwITCH_SET_RESOLUTION )
				   | (1 << ESP32_SwITCH_SET_TICK_SOURCE)
			   	   | (1 << ESP32_SwITCH_SET_FREQ_HZ) ) ) {

	// reset? It seems that a reset is needed to start timer
	newPWM_Timer.RESET = 0;

	// low speed timer? -> needs 'LOW_SPEED_UPDATE' bit set to take settings
	if (!newBlock) newPWM_Timer.LOW_SPEED_UPDATE = 1;

	// store ESP32 timer configuration registers
	memcpy(pPWM_Timer, &newPWM_Timer, sizeof(ESP32_PWM_Timer_t));

	# if ESP32_SwITCH_DBG >= 5
	printf("|ESP32_PWM_Timer_s addr=%p>"
		,pPWM_Timer);

	SCDEFn->HexDumpOutFn ("dump"
		,pPWM_Timer
		, sizeof(ESP32_PWM_Timer_t) );
	# endif
  }

  return false;

}

















/*
//>Add Readings Bit 00 -----------------------------------------------------------------------------

  if (AffectedReadings & SwITCH_R_val)

	{

	RespArgsWPos += os_sprintf(RespArgsWPos,
		"&val=%u",(uint) SysCfgRamRtcMirrored->MySwITCH_FeatCfgRamRtcMirrored[ADID].RB01_Val);

	}

//>Add Readings Bit 01 -----------------------------------------------------------------------------

  if (AffectedReadings & SwITCH_R_dim)

	{

	RespArgsWPos += os_sprintf(RespArgsWPos,
		"&dim=%u",(100 *
		(SysCfgRamRtcMirrored->MySwITCH_FeatCfgRamRtcMirrored[ADID].RB01_Val -
		SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[ADID].IB01_SwValMin) / 
		(SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[ADID].IB01_SwValMax - 
		SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[ADID].IB01_SwValMin) ) );

	}

//>Add Readings Bit 02 -----------------------------------------------------------------------------

  if (AffectedReadings & SwITCH_R_name)

	{

	// add Unique-Feature-ID for identification purposes (Name-Change, ...)
	RespArgsWPos += SwITCH_UFID(RespArgsWPos, ADID);


	// add capabilities of this feature ...
	RespArgsWPos += os_sprintf(RespArgsWPos,
		"&caps=000F");						// 0b1111 <-16BIT-HEX

	// add current name of this feature
	RespArgsWPos += os_sprintf(RespArgsWPos,
		"&name=%s",SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[ADID].IB01_Name);

	}

//>always add Reading - state ----------------------------------------------------------------------

	// are we adding ?
	if (RespArgsWPos) RespArgsWPos += os_sprintf(RespArgsWPos,"&");

	// always add state ! on / off
	if (SysCfgRamRtcMirrored->MySwITCH_FeatCfgRamRtcMirrored[ADID].RB01_Val == 0)
		RespArgsWPos += os_sprintf(RespArgsWPos, "state=off");

	else  RespArgsWPos += os_sprintf(RespArgsWPos, "state=on");

//--------------------------------------------------------------------------------------------------

*/
















/**
 * ------------------------------------------------------------------------------------------------
 *  FName: ESP32_SwITCH_SetAffectedReadings
 *  Desc: Do the readings update for this definition according to 'affectedReadings'
 *  Info: 
 *  Para: ESP32_SwITCH_Definition_t *ESP32_SwITCH_Definition -> Definition that should get the input
 *        uint64_t affectedReadings -> according to ...
 *  Rets: Bool -> TRUE = input parsed successful, FALSE = input parsed, but NOT successful, CANCELED
 * ------------------------------------------------------------------------------------------------
 */
bool ICACHE_FLASH_ATTR
ESP32_SwITCH_SetAffectedReadings(ESP32_SwITCH_Definition_t* ESP32_SwITCH_Definition
	,uint64_t affectedReadings)
{

 affectedReadings = 0xfffffffffffffff;

  // do we have updates?
  if (affectedReadings) {

  SCDEFn->readingsBeginUpdateFn((Common_Definition_t*) ESP32_SwITCH_Definition);

  }

  // we have no updates
  else return false;

// -----------------------------------------------------------------------------------------------

  // temp for reading creation
  uint8_t *readingNameText;
  size_t readingNameTextLen;
  uint8_t *readingValueText;
  size_t readingValueTextLen;

  // pointer to make the hardware counters accessible
  ESP32_PWM_Channel_t *pPWM_Channel =
	(ESP32_PWM_Channel_t*) (0x3FF59000 + (0x14 * ESP32_SwITCH_Definition->Channel));
  // access low speed channels (instead of high speed channels)?
  if (ESP32_SwITCH_Definition->Block) pPWM_Channel =
	(ESP32_PWM_Channel_t*) ( (uint8_t*) pPWM_Channel + 0xa0);

  // pointer to make the hardware timers accessible
  ESP32_PWM_Timer_t *pPWM_Timer =
	(ESP32_PWM_Timer_t*) (0x3FF59140 + (0x08 * ESP32_SwITCH_Definition->Timer));
  // access low speed timers (instead of high speed timers)?
  if (ESP32_SwITCH_Definition->Block) pPWM_Timer =
	(ESP32_PWM_Timer_t*) ( (uint8_t*) pPWM_Timer + 0x20);

//>Add Readings Bit 00 -----------------------------------------------------------------------------

  // add reading 'Duty'
  if (affectedReadings & ESP32_SwITCH_R_DUTY) {

	readingNameTextLen = asprintf((char **) &readingNameText
		,"Duty");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%u"
		,pPWM_Channel->DUTY);

	SCDEFn->readingsBulkUpdateFn((Common_Definition_t*) ESP32_SwITCH_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);
  }

//>Add Readings Bit 01 -----------------------------------------------------------------------------

  // add reading 'Resolution'
  if (affectedReadings & ESP32_SwITCH_R_RESOLUTION) {

	readingNameTextLen = asprintf((char **) &readingNameText
		,"Resolution");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%u"
		,(1 << pPWM_Timer->RESOLUTION) );

	SCDEFn->readingsBulkUpdateFn((Common_Definition_t*) ESP32_SwITCH_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);
  }

//>Add Readings Bit 02 -----------------------------------------------------------------------------
/*
  if (affectedReadings & ESP32_SwITCH_R_AV01_AV05_AV15) {

	int ThisS0CounterCaps = 0b11100000000000000000000000000000;
	// Add readings depending on CAPS ...

	// AV01 = AVerage last 01 Min (calculated), zwingt AVF = AVerageFactor und AVU  = AVerageUnit	
	if (ThisS0CounterCaps & 0b10000000000000000000000000000000)	// AV01 Caps avail?

		{

		RespArgsWPos += os_sprintf(RespArgsWPos // 'AV01' -> AVerage01min (calculated)
			,"&av01=%d.%03d"
			,(int)(SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].RB01_Average01Min)	
			,(int)((SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].RB01_Average01Min - 
			 (int)SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].RB01_Average01Min)*1000) );

		}

	// AV05 = AVerage last 05 Min (calculated), zwingt AVF = AVerageFactor und AVU  = AVerageUnit		
	if (ThisS0CounterCaps & 0b01000000000000000000000000000000)	// AV05 Caps avail?

		{

		RespArgsWPos += os_sprintf(RespArgsWPos	// 'AV05' -> AVerage05min (calculated)
			,"&av05=%d.%03d"
			,(int)(SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].RB01_Average05Min)	
			,(int)((SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].RB01_Average05Min -
			 (int)SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].RB01_Average05Min)*1000) );

		}

	// AV15 = AVerage last 15 Min (calculated), zwingt AVF = AVerageFactor und AVU  = AVerageUnit
	if (ThisS0CounterCaps & 0b00100000000000000000000000000000)	// AV15 Caps avail?

		{

		RespArgsWPos += os_sprintf(RespArgsWPos	// 'AV15' -> AVerage15min (calculated)
			,"&av15=%d.%03d"
			,(int)(SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].RB01_Average15Min)	
			,(int)((SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].RB01_Average15Min -
			 (int)SysCfgRamNoMirror->MyS0_FeatCfgRamNoMirror[ADID].RB01_Average15Min)*1000) );	

		}

	}
*/
//>Add Readings Bit 03 -----------------------------------------------------------------------------
/*
  // add reading 'Average_Factor' //avf'
  if (affectedReadings & ESP32_SwITCH_R_AVF) {

	readingNameTextLen = asprintf((char **) &readingNameText
		,"Average_Factor");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%d.%04d"
		,(int) ESP32_SwITCH_Definition->IB01_AverageFactor	
		,(int) ( (ESP32_SwITCH_Definition->IB01_AverageFactor -
		(int) ESP32_SwITCH_Definition->IB01_AverageFactor) * 10000) );

	SCDEFn->readingsBulkUpdateFn((Common_Definition_t*) ESP32_SwITCH_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }
*/
//	RespArgsWPos += os_sprintf(RespArgsWPos
//		,"&avf=%d.%04d"
//		,(int)(SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].IB01_AverageFactor)	
//		,(int)((SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].IB01_AverageFactor -
//		(int)SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].IB01_AverageFactor)*10000) );


//>Add Readings Bit 04 -----------------------------------------------------------------------------
/*
  // add reading 'Average_Unit' //avu'
  if (affectedReadings & ESP32_SwITCH_R_AVU) {

	readingNameTextLen = asprintf((char **) &readingNameText
		,"Average_Unit");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%s"
		,ESP32_SwITCH_Definition->IB01_AverageUnit);

	SCDEFn->readingsBulkUpdateFn((Common_Definition_t*) ESP32_SwITCH_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }

*/
//	RespArgsWPos += os_sprintf(RespArgsWPos
//		,"&avu=%s"
//		,SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].IB01_AverageUnit);



//>Add Readings Bit 05 -----------------------------------------------------------------------------
/*
  // add reading 'Alltime_Total_Factor' //atf'
  if (affectedReadings & ESP32_SwITCH_R_ATF) {

	readingNameTextLen = asprintf((char **) &readingNameText
		,"Alltime_Total_Factor");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%d.%04d"
		,(int) (ESP32_SwITCH_Definition->IB01_AlltimeTotalFactor)	
		,(int) ( (ESP32_SwITCH_Definition->IB01_AlltimeTotalFactor -
		(int) ESP32_SwITCH_Definition->IB01_AlltimeTotalFactor) * 10000) );

	SCDEFn->readingsBulkUpdateFn((Common_Definition_t*) ESP32_SwITCH_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }
*/
//	RespArgsWPos += os_sprintf(RespArgsWPos
//		,"&atf=%d.%04d"
//		,(int)(SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].IB01_AlltimeTotalFactor)	
//		,(int)((SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].IB01_AlltimeTotalFactor -
//		(int)SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].IB01_AlltimeTotalFactor)*10000) );


//>Add Readings Bit 06 -----------------------------------------------------------------------------
/*
  // add reading 'Alltime_Total_Unit' //atu'
  if (affectedReadings & ESP32_SwITCH_R_ATU) {

	readingNameTextLen = asprintf((char **) &readingNameText
		,"Alltime_Total_Unit");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%s"
		,ESP32_SwITCH_Definition->IB01_AlltimeTotalUnit);

	SCDEFn->readingsBulkUpdateFn((Common_Definition_t*) ESP32_SwITCH_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }
*/
//	RespArgsWPos += os_sprintf(RespArgsWPos
//		,"&atu=%s"
//		,SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].IB01_AlltimeTotalUnit);


//>Add Readings Bit 07 -----------------------------------------------------------------------------
/*
  // add reading 'Debounce' //deb'
  if (affectedReadings & ESP32_SwITCH_R_DEB) {

	readingNameTextLen = asprintf((char **) &readingNameText
		,"Debounce");

	readingValueTextLen = asprintf((char **) &readingValueText
		,"%u"
		,ESP32_SwITCH_Definition->IB01_Debounce);

	SCDEFn->readingsBulkUpdateFn((Common_Definition_t*) ESP32_SwITCH_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);

  }

*/
//	RespArgsWPos += os_sprintf(RespArgsWPos
//		,"&deb=%u"
//		,SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].IB01_Debounce);


//>Add Readings Bit 08 -----------------------------------------------------------------------------
/*
  if (affectedReadings & ESP32_SwITCH_R_name)
	{

	// add Unique-Feature-ID for identification purposes (Name-Change, ...)
	RespArgsWPos += S0_UFID(RespArgsWPos, ADID);

	// add capabilities of this feature ...
	RespArgsWPos += os_sprintf(RespArgsWPos,
		"&caps=0007");						// 0b11110 <-16BIT-HEX

	// add current name of this feature
	RespArgsWPos += os_sprintf(RespArgsWPos,
		"&name=%s",SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].IB01_Name);

	}
*/

//>always add Reading - state ----------------------------------------------------------------------
/*
	// are we adding ?
	if (RespArgsWPos) RespArgsWPos += os_sprintf(RespArgsWPos,"&");

	// always add state ! on / off
	RespArgsWPos += os_sprintf(RespArgsWPos,
		"state=on");
*/
//--------------------------------------------------------------------------------------------------


  // all readings done, finnish update
  SCDEFn->readingsEndUpdateFn((Common_Definition_t*) ESP32_SwITCH_Definition);


  return true;
}













































