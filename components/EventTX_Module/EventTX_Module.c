/* #################################################################################################
 *
 *      Name: ESP32_SPI Module
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
 *
 */

#include <ProjectConfig.h>
#include <esp8266.h>
#include <Platform.h>

// the Smart-Connected-Device-Engine structures & types ..
#include <SCDE_s.h>

// provides WebIf, need the structures & types ...
#include "WebIf_Module.h"

// this Modules structures & types ...
#include "EventTX_Module_s.h"
#include "EventTX_Module.h"

// -------------------------------------------------------------------------------------------------

// set default build verbose - if no external override
#ifndef EVENTTX_Module_DBG  
#define EVENTTX_Module_DBG  5	// 5 is default
#endif 

// -------------------------------------------------------------------------------------------------





#include "lwip/sockets.h"













/**
 * -------------------------------------------------------------------------------------------------
 *  DName: EventTX_ActiveResourcesDataA_forWebIf
 *  Desc: Resource-Content-structure of active Directory - PART A (Resource-Data-Row)
 *  Data: WebIf_ActiveResourcesDataA_t[X] from HttpD.h
 * -------------------------------------------------------------------------------------------------
 */
// Content:   AllowedMethodBF          | AllowedDocMimeBF | AllowedSchemeBF  |free|CgiNo| EnaByBit | Url
const WebIf_ActiveResourcesDataA_t EventTX_ActiveResourcesDataA_forWebIf[] = {  //ICACHE_RODATA_ATTR = 

// Device Home Page
  // Device-Feature Control
  { 0b00000000000000000000000000001010, 0b0000000000000010, 0b0000000000000001,  0,  0, 0b00000000, "/CoNTROL_8S-1C-1ADC"}
  // RENAMED Device-Feature Control
 ,{ 0b00000000000000000000000000001010, 0b0000000000000010, 0b0000000000000001,  0,  0, 0b00000000, "/\xf0\x00 CoNTROL_8S-1C-1ADC.htm"}	// ADC_DevName

  // Device-Feature Control - Home Automation SCDE:// access Resources
 ,{ 0b00000000000000000000000000000110, 0b0000000000010001, 0b0000000000000010,  0,  0, 0b00000000, "/CoNTROL_8S-1C-1ADC"}
  // RENAMED Device-Feature Control - Home Automation SCDE:// access Resources
 ,{ 0b00000000000000000000000000000110, 0b0000000000010001, 0b0000000000000010,  0,  0, 0b00000000, "/\xf0"}	// ADC_DevName

  // Switch-Feature Control
 ,{ 0b00000000000000000000000000001010, 0b0000000000011110, 0b0000000000000001,  0,  1, 0b00000000, "/\xf1\x00\x08/SwITCH\x00 SwITCHX.htm"} // ADC_FeatNr
  // RENAMED Switch-Feature Control
 ,{ 0b00000000000000000000000000001010, 0b0000000000011110, 0b0000000000000001,  0,  1, 0b00000000, "/\xf3\x00\x08\x00 SwITCHX.htm"} // ADC_SwITCHFN
  // Multi Switch(es)-Feature Control
 ,{ 0b00000000000000000000000000001010, 0b0000000000001010, 0b0000000000000001,  0,  2, 0b00000000, "/SwITCH"}

  // Switch-Feature Control - Home Automation Scheme SwITCH:// access Resources
 ,{ 0b00000000000000000000000000000110, 0b0000000000010001, 0b0000000000001000,  0,  1, 0b00000000, "/\xf1\x00\x08"} // ADC_FeatNr
  // RENAMED Switch-Feature Control - Home Automation SwITCH:// access Resources
 ,{ 0b00000000000000000000000000000110, 0b0000000000010001, 0b0000000000001000,  0,  1, 0b00000000, "/\xf3\x00\x08"} // ADC_SwITCHFN

  // Clima-Feature Control
 ,{ 0b00000000000000000000000000001010, 0b0000000000011110, 0b0000000000000001,  0,  3, 0b00000000, "/\xf1\x00\x01/ClIMA\x00 ClIMAX.htm"} // ADC_FeatNr
  // RENAMED Clima-Feature Control
 ,{ 0b00000000000000000000000000001010, 0b0000000000011110, 0b0000000000000001,  0,  3, 0b00000000, "/\xf4\x00\x01\x00 ClIMAX.htm"} // ADC_ClIMAFN
  // Multi ClIMA(s)-Feature Control
 ,{ 0b00000000000000000000000000001010, 0b0000000000001010, 0b0000000000000001,  0,  4, 0b00000000, "/ClIMA"}

  // ClIMA-Feature Control - Home Automation Scheme ClIMA:// access Resources
 ,{ 0b00000000000000000000000000000110, 0b0000000000010001, 0b0000000000010000,  0,  3, 0b00000000, "/\xf1\x00\x01"} // ADC_FeatNr
  // RENAMED ClIMA-Feature Control - Home Automation ClIMA:// access Resources
 ,{ 0b00000000000000000000000000000110, 0b0000000000010001, 0b0000000000010000,  0,  3, 0b00000000, "/\xf4\x00\x01"} // ADC_ClIMAFN	

  // ADC Feature-Control
 ,{ 0b00000000000000000000000000001010, 0b0000000000011110, 0b0000000000000001,  0,  5, 0b00000000, "/\xf1\x00\x01/ADC\x00 ADCX.htm"} // ADC_FeatNr
  // RENAMED ADC-Feature Control
 ,{ 0b00000000000000000000000000001010, 0b0000000000011110, 0b0000000000000001,  0,  5, 0b00000000, "/\xf6\x00\x01\x00 ADCX.htm"} // ADC_ADCFN
  // Multi ADC(s)-Feature Control
 ,{ 0b00000000000000000000000000001010, 0b0000000000001010, 0b0000000000000001,  0,  6, 0b00000000, "/ADC"}

  // ADC-Feature Control - Home Automation Scheme ADC:// access Resources
 ,{ 0b00000000000000000000000000000110, 0b0000000000010001, 0b0000000001000000,  0,  5, 0b00000000, "/\xf1\x00\x01"} // ADC_FeatNr
  // RENAMED ADC-Feature Control - Home Automation ADC:// access Resources
 ,{ 0b00000000000000000000000000000110, 0b0000000000010001, 0b0000000001000000,  0,  5, 0b00000000, "/\xf6\x00\x01"} // ADC_ADCFN

  // SERVICE PAGES
 ,{ 0b00000000000000000000000000001010, 0b0000000000000110, 0b0000000000000001,  0,  7, 0b00000000, "/BasServ"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000000110, 0b0000000000000001,  0,  8, 0b00000000, "/AdvServ"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000000110, 0b0000000000000001,  0,  9, 0b00000000, "/SpcAttr"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000000110, 0b0000000000000001,  0, 10, 0b00000000, "/GenAttr"}

  // Device Cfg
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0, 11, 0b00000000, "/DeviceCfg"}

  // SOC Hardware Cfg
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0, 12, 0b00000000, "/SoCHWCfg"}

  // WiFi Stations Cfg
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0, 13, 0b00000000, "/WiFi/StationCfg"}

  // WIFI Q-Connect PAGES
 ,{ 0b00000000000000000000000000001010, 0b0000000000000010, 0b0000000000000001,  0, 14, 0b00000000, "/WiFi/QConnect"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000001000, 0b0000000000000001,  0, 15, 0b00000000, "/WiFi/WiFiScan"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000000100, 0b0000000000000001,  0, 16, 0b00000000, "/WiFi/Connect"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000000100, 0b0000000000000001,  0, 17, 0b00000000, "/WiFi/Setmode"}

  // Service Access Point Cfg
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0, 18, 0b00000000, "/WiFi/ServAPCfg"}

  // TimeStamp Cfg
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0, 19, 0b00000000, "/TiStCfg"}

  // Firmware Update
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0, 20, 0b00000000, "/Firmware"}

  // Redirects
 ,{ 0b00000000000000000000000000001010, 0b0000000000000001, 0b0000000000000001,  0, 21, 0b00000000, "/WiFi"}	// nomime
 ,{ 0b00000000000000000000000000001010, 0b0000000000000001, 0b0000000000000001,  0, 21, 0b00000000, "/WiFi/"}	// nomime
 ,{ 0b00000000000000000000000000001010, 0b0000000000000001, 0b0000000000000001,  0, 22, 0b00000000, "/"}	// nomime
 ,{ 0b00000000000000000000000000001010, 0b0000000000000010, 0b0000000000000001,  0, 22, 0b00000000, "/index"}	// .htm

  // Secret services
 ,{ 0b00000000000000000000000000001010, 0b0000000000100000, 0b0000000000000001,  0, 23, 0b00000000, "/32MBitFlash"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000100000, 0b0000000000000001,  0, 24, 0b00000000, "/wfs"}

 ,{0,0,0,0,0,0,"*"}
};



/**
 * -------------------------------------------------------------------------------------------------
 *  DName: EventTX_ActiveResourcesDataB_forWebIf
 *  Desc: Resource-Content-structure of active Directory - PART B (Procedure-Call-Data-Row) 
 *  Data: WebIf_ActiveResourcesDataB_t[X] from HttpD.h
 * -------------------------------------------------------------------------------------------------
 */
// CgiFucID=(No.<<16)+AllowedSchemeBF |      cgi            |     cgi_data
const WebIf_ActiveResourcesDataB_t EventTX_ActiveResourcesDataB_forWebIf[] =  {  //ICACHE_RODATA_ATTR =

  // Device-Feature Control
  {(0<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	Device_tpl	}
 ,{(0<<16) +	0b0000000000010001,	NULL,NULL}//	SCDE_set,		NULL		}//Device_set,

  // Switch-Feature Control
 ,{(1<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	SwITCH_tpl	}
 ,{(1<<16) +	0b0000000000000100,	NULL,NULL}//	SwITCH_cgi,		NULL		}
 ,{(1<<16) +	0b0000000000001000,	NULL,NULL}//	SwITCH_jso,		NULL		}
 ,{(1<<16) +	0b0000000000010001,	NULL,NULL}//	SwITCH_set,		NULL		}

  // Multi Switch(es)-Feature Control
 ,{(2<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	SwITCH_tpl	}
 ,{(2<<16) +	0b0000000000001000,	NULL,NULL}//	SwITCH_jso,		NULL		}

  // Clima-Feature Control
 ,{(3<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	ClIMA_tpl	}
 ,{(3<<16) +	0b0000000000000100,	NULL,NULL}//	ClIMA_cgi,		NULL		}
 ,{(3<<16) +	0b0000000000001000,	NULL,NULL}//	ClIMA_jso,		NULL		}
 ,{(3<<16) +	0b0000000000010001,	NULL,NULL}//	ClIMA_set,		NULL		}

  // Multi ClIMA(s)-Feature Control
 ,{(4<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	ClIMA_tpl	}
 ,{(4<<16) +	0b0000000000001000,	NULL,NULL}//	ClIMA_jso,		NULL		}

  // ADC-Feature Control
 ,{(5<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	ADC_tpl		}
 ,{(5<<16) +	0b0000000000000100,	NULL,NULL}//	ADC_cgi,		NULL		}
 ,{(5<<16) +	0b0000000000001000,	NULL,NULL}//	ADC_jso,		NULL		}
 ,{(5<<16) +	0b0000000000010001,	NULL,NULL}//	ADC_set,		NULL		}

  // Multi ADC(s)-Feature Control
 ,{(6<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	ADC_tpl		}
 ,{(6<<16) +	0b0000000000001000,	NULL,NULL}//	ADC_jso,		NULL		}

  // ### SERVICE PAGES ###
 ,{(7<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	BasServ_tpl	}
 ,{(7<<16) +	0b0000000000000100,	NULL,NULL}//	BasServ_cgi,		NULL		}

 ,{(8<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	AdvServ_tpl	}
 ,{(8<<16) +	0b0000000000000100,	NULL,NULL}//	AdvServ_cgi,		NULL		}

 ,{(9<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	SpcAttr_tpl	}
 ,{(9<<16) +	0b0000000000000100,	NULL,NULL}//	SpcAttr_cgi,		NULL		}

 ,{(10<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	GenAttr_tpl	}
 ,{(10<<16) +	0b0000000000000100,	NULL,NULL}//	GenAttr_cgi,		NULL		}

  // ### Device Cfg ###
 ,{(11<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	DeviceCfg_tpl	}
 ,{(11<<16) +	0b0000000000000100,	NULL,NULL}//	DeviceCfg_cgi,		NULL		}
 ,{(11<<16) +	0b0000000000001000,	NULL,NULL}//	DeviceCfg_jso,		NULL		}

  // ### SOC Hardware Cfg ###
 ,{(12<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	SoCHWCfg_tpl	}
 ,{(12<<16) +	0b0000000000000100,	NULL,NULL}//	SoCHWCfg_cgi,		NULL		}
 ,{(12<<16) +	0b0000000000001000,	NULL,NULL}//	SoCHWCfg_jso,		NULL		}

  // ### WiFi Station Cfg ###
 ,{(13<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	StationCfg_tpl	}
 ,{(13<<16) +	0b0000000000000100,	NULL,NULL}//	StationCfg_cgi,		NULL		}
 ,{(13<<16) +	0b0000000000001000,	NULL,NULL}//	StationCfg_jso,		NULL		}

  // ### WIFI Q-Connect PAGES ###
 ,{(14<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	WifiQConnect_tpl}
 ,{(15<<16) +	0b0000000000001000,	NULL,NULL}//	WiFiScan_jso,		NULL		}
 ,{(16<<16) +	0b0000000000000100,	NULL,NULL}//	WiFiConnect_cgi,	NULL		}
 ,{(17<<16) +	0b0000000000000100,	NULL,NULL}//	WifiSetMode_cgi,	NULL		}	

  // ### Service Access Point Cfg ###
 ,{(18<<16) +	0b0000000000000010,	NULL,NULL}//WebIf_EspFSAdvFileTX,	ServAPCfg_tpl	}
 ,{(18<<16) +	0b0000000000000100,	NULL,NULL}//	ServAPCfg_cgi,		NULL		}
 ,{(18<<16) +	0b0000000000001000,	NULL,NULL}//	ServAPCfg_jso,		NULL		}

  // ### TimeStamp Cfg ###
 ,{(19<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	TiStCfg_tpl	}
 ,{(19<<16) +	0b0000000000000100,	NULL,NULL}//	TiStCfg_cgi,		NULL		}
 ,{(19<<16) +	0b0000000000001000,	NULL,NULL}//	TiStCfg_jso,		NULL		}

  // ### Firmware Update ###
 ,{(20<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	FirmwareUd_tpl	}
 ,{(20<<16) +	0b0000000000000100,	NULL,NULL}//	FirmwareUd_cgi,		NULL		}
 ,{(20<<16) +	0b0000000000001000,	NULL,NULL}//	FirmwareUd_jso,		NULL		}

  // ### Redirects ###
 ,{(21<<16) +	0b0000000000000001,	NULL,NULL}//cgiRedirect,	"/WiFi/QConnect.htm"		}
 ,{(22<<16) +	0b0000000000000001,	NULL,NULL}//cgiRedirect,	"/CoNTROL_8S-1C-1ADC.htm"	}
 ,{(22<<16) +	0b0000000000000010,	NULL,NULL}//cgiRedirect,	"/CoNTROL_8S-1C-1ADC.htm"	}

  // ### Secret services ###
 ,{(23<<16) +	0b0000000000100000,	NULL,NULL}//ReadFullFlash_cgi,	NULL		}		//.bin
 ,{(24<<16) +	0b0000000000100000,	NULL,NULL}//	WriteFileSystem_cgi,	NULL		}		//.bin
};



/**
 * -------------------------------------------------------------------------------------------------
 *  DName: EventTX_provided_Fn
 *  Desc: Assigns common + custom functions this Module provides to the SCDE (and client Modules)
 *  Data: WebIf_provided_fn_t
 * -------------------------------------------------------------------------------------------------
 */
EventTX_ProvidedByModule_t EventTX_ProvidedByModule = {   // A-Z order
{
// --- first the common module functions ---
  "EventTX",					// Type-Name of module -> on Linux libfilename.so !
  7,						// size of Type-Name

  EventTX_Add,					// Add
  NULL,						// Attribute
  EventTX_Define,				// Define
  NULL, 					// Delete
  EventTX_Direct_Read,				// Direct_Read
  EventTX_Direct_Write,				// Direct_Write
  NULL,						// Except
  NULL,						// Get
  EventTX_IdleCb,				// IdleCb
  EventTX_Initialize,				// Initialize
  NULL,						// Notify
  NULL,						// Parse
  NULL,						// Read
  NULL,						// Ready
  NULL,						// Rename
  NULL,						// Set
  NULL,						// Shutdown
  NULL, 					// State
  EventTX_Sub, 					// Sub
  EventTX_Undefine,				// Undefine
  NULL,						// Write
  NULL,						// FnProvided
  sizeof(Entry_EventTX_Definition_t)		// Modul specific Size (Common_Definition_t + X)
},
// --- now the custom module fuctions ---
 //  ESP32_SPI_bus_add_device,			// adds an device to the definitions host
};



/**
 * --------------------------------------------------------------------------------------------------
 *  FName: EventTX_Add
 *  Desc: Processes the device-specific command line arguments from the set command
 *  Info: Invoked by cmd-line 'Set Telnet_Definition.common.Name setArgs'
 *  Para: Telnet_Definition_t *Telnet_Definition -> WebIF Definition that should get a set cmd
 *        uint8_t *setArgs -> the setArgs
 *        size_t setArgsLen -> length of the setArgs
 *  Rets: char * -> response text in allocated memory, NULL=no text
 * --------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* 
EventTX_Add(Common_Definition_t* Common_Definition
	,uint8_t *kvArgs
	,size_t kvArgsLen)
  {

  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  #if SCDEH_DBG >= 5
  printf("\n|EventTX_Add, Name:%.*s, got key value args:%.*s>"
	,Common_Definition->nameLen
	,Common_Definition->name
	,kvArgsLen
	,kvArgs);
  #endif

  // alloc mem for retMsg
  retMsg = malloc(sizeof(strTextMultiple_t));

  // response with error text
  retMsg->strTextLen = asprintf(&retMsg->strText
	,"EventTX_Add, Name:%.*s, got key value args:%.*s"
	,Common_Definition->nameLen
	,Common_Definition->name
	,kvArgsLen
	,kvArgs);

  return retMsg;

  }



/*
 * --------------------------------------------------------------------------------------------------
 *  FName: EventTX_IdleCb	
 *  Desc: Finalizes the defines a new "device" of 'WebIf' type. Contains devicespecific init code.
 *  Info: 
 *  Para: Entry_EventTX_Definition_t *EventTX_Definition -> prefilled WebIf Definition
 *        char *Definition -> the last part of the CommandDefine arg* 
 *  Rets: char* -> response text NULL=no text
 * --------------------------------------------------------------------------------------------------
 */
int //feedModuleTask
EventTX_IdleCbX(Common_Definition_t *Common_Definition)
{

  #if SCDEH_DBG >= 5
  printf("\n|EventTX_IdleCb Entering..., Name:%.*s>"
	,Common_Definition->nameLen
	,Common_Definition->name); //(char*)
  #endif

  // make common ptr to modul specific ptr
  Entry_EventTX_Definition_t* EventTX_Definition = (Entry_EventTX_Definition_t*) Common_Definition;


/*
	// do idle callback if Slot is
	// +  allocated
        // + has a conn (!= NULL)
	// + F_GENERATE_IDLE_CALLBACK is set in ConnCtrl ...
	if ( (ConnSlots[i] != NULL) &&
		(ConnSlots[i]->conn != NULL) && //überflüssig ?
		(ConnSlots[i]->ConnCtrlFlags & F_GENERATE_IDLE_CALLBACK) )

		{

		// call it with current connection
		EventTX_IdleCb(ConnSlots[i]->conn);

		}
*/

  if (EventTX_Definition->reverse) {

	// get assigned HTTPD-Connection-Slot-Data
	EventTX_HTTPDConnSlotData_t *conn
		= EventTX_Definition->reverse;

	// execute disconnection (indicated by NEEDS_CLOSE flag) or send more data ...
	if ( conn->ConnCtrlFlags & F_GENERATE_IDLE_CALLBACK ) {

		// execute Idle Callback
		EventTX_IdleCb(EventTX_Definition);
	}
  }

  return 0;
}



/*
 * --------------------------------------------------------------------------------------------------
 *  FName: EventTX_sent (espconn_sent compatible Fn)
 *  Desc: Platform conn - Send data Fn
 *  Para: Entry_EventTX_Definition_t *EventTX_Definition -> WebIf Definition
 *        uint8_t *Buff -> buffer with data to send
 *        uint Len -> length of data to send
 *  Rets: int -> written data
 * --------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR
EventTX_sent(Entry_EventTX_Definition_t* p_entry_EventTX_definition, uint8_t* send_buffer, uint send_buffer_len)
{
  # if SCDED_DBG >= 5
  printf("\n|EventTX_Sent len:%d!>"
	,send_buffer_len);
  # endif

  // select for want writing (F_WANTS_WRITE), because maybe we have more data to send ...
  p_entry_EventTX_definition->common.Common_CtrlRegA |= F_WANTS_WRITE;

  int result = write(p_entry_EventTX_definition->common.fd, send_buffer, send_buffer_len);

  // an error occured ?
  if ( !( result >= 0 ) ) {

	#if SCDED_DBG >= 5
	printf("\n|EventTX_Send has error %d as result!>", result);
	#endif

	result = 0;
  }

 return result; 
//  return (write(p_entry_EventTX_definition->common.fd, send_buffer, send_buffer_len) >= 0);
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: WebIf Disconnect (espconn_disconnect compatible Fn)
 * Desc: Platform conn - Disconnect Fn
 * Info: 
 * Para: Entry_EventTX_Definition_t* p_entry_EventTX_definition -> WebIf Definition = WebIf Conn
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR
EventTX_disconnect(Entry_EventTX_Definition_t* p_entry_EventTX_definition)
{
  # if SCDED_DBG >= 5
  os_printf("\n|EventTX_Sdisconnect Fn>");
  # endif

  // select for disconnecting (F_NEEDS_CLOSE)
  p_entry_EventTX_definition->EventTX_CtrlRegA |= F_NEEDS_CLOSE;

  // select for want writing (F_WANTS_WRITE),
  // because the real close is done in the write select of code
  p_entry_EventTX_definition->common.Common_CtrlRegA |= F_WANTS_WRITE;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: EventTX_UndefineRaw
 * Desc: internal cleanup and EventTX_Definition delete
 * Para: Entry_EventTX_Definition_t *EventTX_Definition -> WebIf Definition that should be deleted
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
int 
EventTX_UndefineRaw(Entry_EventTX_Definition_t* EventTX_Definition)
{
  // connection close
  close(EventTX_Definition->common.fd);

  // FD is gone ...
//EventTX_Definition->common.FD = -1;

// --- dealloc? non master conns?

  // remove WebIf Definition
  STAILQ_REMOVE(&SCDERoot_at_EventTX_M->HeadCommon_Definitions,
	(Common_Definition_t*) EventTX_Definition,
	Common_Definition_s,
	entries);

  // mark slot as unused
  EventTX_Definition->HTTPD_InstCfg->SlotCtrlRegBF &= 
	~( 1 << EventTX_Definition->slot_no);

  // free Name
  free (EventTX_Definition->common.name);

  // free TCP struct
  free (EventTX_Definition->proto.tcp);

  // free EventTX_Definition
  free (EventTX_Definition);

  return 0;
}

























/*
 *--------------------------------------------------------------------------------------------------
 *FName: EventTX_Define
 * Desc: Finalizes the defines a new "device" of 'WebIf' type. Contains devicespecific init code.
 * Info: 
 * Para: Entry_EventTX_Definition_t *EventTX_Definition -> prefilled WebIf Definition
 *       char *Definition -> the last part of the CommandDefine arg* 
 * Rets: strTextMultiple_t* -> response text NULL=no text
 *--------------------------------------------------------------------------------------------------
 */
strTextMultiple_t*
EventTX_Define (Entry_Definition_t* p_entry_definition)
{
  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // make common ptr to modul specific ptr
  Entry_EventTX_Definition_t* p_entry_eventtx_definition =
	(Entry_EventTX_Definition_t*) p_entry_definition;

  #if SCDEH_DBG >= 5
  printf("\n|EventTX_Def, Def:%.*s>"
	,p_entry_eventtx_definition->common.definitionLen
	,p_entry_eventtx_definition->common.definition);
  #endif

//--------------------------------------------------------------------------------------------------

// SCDE Event-TX-Connect-Control State
  p_entry_eventtx_definition->ETXCC_State = s_delay_then_idle;	// delay after boot up ...

// SCDE Event-TX - clear STATISTICS
//p_entry_eventtx_definition->SCDEETX_ConnCNT = 0; cleared by memset ...
//p_entry_eventtx_definition->SCDEETX_MsgCNT = 0; cleared by memset ...
//p_entry_eventtx_definition->SCDEETX_RcvOKCNT = 0; cleared by memset ...
//p_entry_eventtx_definition->SCDEETX_RcvNOKCNT = 0; cleared by memset ...

//--------------------------------------------------------------------------------------------------

  // alloc memory for the HTTPD-Instance-Configuration
  p_entry_eventtx_definition->HTTPD_InstCfg = 
	(HTTPD_InstanceCfg_t*) malloc (sizeof(HTTPD_InstanceCfg_t));

  // memclr the HTTPD-Instance-Configuration
  memset(p_entry_eventtx_definition->HTTPD_InstCfg, 0,
	sizeof (HTTPD_InstanceCfg_t));

 // HTTPD-Instance-Configuration -> set content
  p_entry_eventtx_definition->HTTPD_InstCfg->BuiltInUrls = 
	(WebIf_ActiveResourcesDataA_t *) &EventTX_ActiveResourcesDataA_forWebIf;
  p_entry_eventtx_definition->HTTPD_InstCfg->BuiltInActiveResources = 
	(WebIf_ActiveResourcesDataB_t *) &EventTX_ActiveResourcesDataB_forWebIf;

  // HTTPD-Instance-Configuration -> Set HTTPD Parser Fn callbacks
  http_parser_settings_t* HTTPDparser_settings = 
	&p_entry_eventtx_definition->HTTPD_InstCfg->HTTPDparser_settings;

  // doppelt ?? HTTPD-Instance-Configuration -> assign the HTTPD Parser the settings
  http_parser_settings_init (HTTPDparser_settings);

  HTTPDparser_settings->on_message_begin = HTTPD_On_Message_Begin_Cb;
  HTTPDparser_settings->on_url = HTTPD_On_Url_Cb;
//HTTPDparser_settings->on_status ???
  HTTPDparser_settings->on_header_field = HTTPD_On_Header_Field_Cb;
  HTTPDparser_settings->on_header_value = HTTPD_On_Header_Value_Cb;
  HTTPDparser_settings->on_headers_complete = HTTPD_On_Headers_Complete_Cb;
  HTTPDparser_settings->on_body = HTTPD_On_Body_Cb;
  HTTPDparser_settings->on_message_complete = HTTPD_On_Message_Complete_Cb;
//HTTPDparser_settings->on_chunk_header ???
//HTTPDparser_settings->on_chunk_complete ???


  // ?? how to init !!!!!!!!!!!
//p_entry_eventtx_definition->HTTPD_InstCfg->SCDED_DirConEnaCtrl = 0; cleared by memset ...

  // reset Load-Serializer-Bitfield -> no heavy load tasks now!
//p_entry_eventtx_definition->HTTPD_InstCfg->LoadSerializer = 0; cleared by memset ...

  //reset Slot Control Register Bitfield -> no connections yet!
//p_entry_eventtx_definition->HTTPD_InstCfg->SlotCtrlRegBF = 0; cleared by memset ...

//--------------------------------------------------------------------------------------------------

  // mark this as the server-socket
  p_entry_eventtx_definition->EventTX_CtrlRegA |= F_THIS_IS_SERVERSOCKET;

 // later from definition
  int port = 3000;

  // ???
  int opt = true;

  int ret;

  // socket fd for event tx
  int socket_fd;

  // server address structure
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));

  // Create socket for incoming connections
  do {
	socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// socked created or error?
	if ( socket_fd < 0 ) {

		SCDEFn_at_EventTX_M->Log3Fn(p_entry_eventtx_definition->common.name
				,p_entry_eventtx_definition->common.nameLen
				,1
				,"EventTX_Define ERROR: failed to create sock! retriing\n");

		vTaskDelay(1000/portTICK_RATE_MS);
	}

  } while ( socket_fd < 0 );

  // prepare struct 'sockaddr_in'
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr("192.168.0.1");
  server_addr.sin_port = htons (port);
  server_addr.sin_len = sizeof (server_addr);

  // set master socket to allow multiple connections , this is just a good habit, it will work without this
  ret = setsockopt (socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof (opt) );
  if ( ret < 0 ) {

	SCDEFn_at_EventTX_M->Log3Fn(p_entry_eventtx_definition->common.name
			,p_entry_eventtx_definition->common.nameLen
			,1
			,"EventTX_Define ERROR: 'setsockopt' failed! error:%d\n"
			,ret);
  }

/* server only
  // bind the socket to the local port
  do {
	ret = bind(listenfd, (struct sockaddr *) &server_addr, sizeof(server_addr));

	if (ret != 0)

		{

		SCDEFn_at_EventTX_M->Log3Fn(p_entry_eventtx_definition->common.name
				,p_entry_eventtx_definition->common.nameLen
				,1
				,"EventTX_Define ERROR: 'bind' failed! retriing\n");

		vTaskDelay(1000/portTICK_RATE_MS);
		}

	} while(ret != 0);
*/

//  # if SCDED_DBG >= 3
 /* printf("|local at:%u.%u.%u.%u:%u,port:%u>"
	,(uint8_t*) &server_addr.sin_addr.s_addr
	,(uint8_t*) &server_addr.sin_addr.s_addr+1
	,(uint8_t*) &server_addr.sin_addr.s_addr+2
	,(uint8_t*) &server_addr.sin_addr.s_addr+3
	,(uint16_t*) &server_addr.sin_port);*/

//  #endif



/* server only
#define HTTPD_MAX_CONNECTIONS 16
  // listen to the local port
  do	{

	ret = listen(listenfd, HTTPD_MAX_CONNECTIONS);

	if (ret != 0)

		{

		SCDEFn_at_EventTX_M->Log3Fn(p_entry_eventtx_definition->common.name
				,p_entry_eventtx_definition->common.nameLen
				,1
				,"EventTX_Define ERROR: 'listen' failed! retriing\n");

		vTaskDelay(1000/portTICK_RATE_MS);
		}

	} while(ret != 0);
*/

  // store FD to the Definition. Will than be processed in global loop ...
  p_entry_eventtx_definition->common.fd = socket_fd;

  // using TCP, create, fill and store struct
  esp_tcp* tcp = malloc (sizeof (esp_tcp));
//  tcp->local_ip = server_addr.sin_addr.s_addr;
  tcp->local_port = server_addr.sin_port;
  p_entry_eventtx_definition->proto.tcp = tcp;

  p_entry_eventtx_definition->type = ESPCONN_TCP;
  p_entry_eventtx_definition->state = ESPCONN_NONE;

  // register the connect-callback - used ...
  espconn_regist_connectcb(p_entry_eventtx_definition,
	EventTX_Connect_Cb);

  SCDEFn_at_EventTX_M->Log3Fn(p_entry_eventtx_definition->common.name
		  ,p_entry_eventtx_definition->common.nameLen
		  ,1
		  ,"Defined a EventTX at X.X.X.X:YYYY, FD is:%d\n"
		  ,socket_fd);

  return retMsg;
}



/*
 * -------------------------------------------------------------------------------------------------
 *  FName: EventTX_Direct_Read
 *  Desc: Called from the global select-loop when FD is in read-set
 *  Info: 
 *  Para: Entry_Definition_t* p_entry_definition -> the FD owners definition
 *  Rets: ? - unused
 * -------------------------------------------------------------------------------------------------
 */
int 
EventTX_Direct_Read(Entry_Definition_t* p_entry_definition)
{
  // make common ptr to modul specific ptr
  Entry_EventTX_Definition_t* p_entry_EventTX_definition = 
	(Entry_EventTX_Definition_t*) p_entry_definition;

// -------------------------------------------------------------------------------------------------

  #if EventTX_Module_DBG >= 7
  SCDEFn_at_EventTX_M->Log3Fn(p_entry_EventTX_definition->common.name,
	p_entry_EventTX_definition->common.nameLen,
	7,
	"Direct Read Fn (module '%.*s'), entering.",
	p_entry_EventTX_definition->common.module->provided->typeNameLen,
	p_entry_EventTX_definition->common.module->provided->typeName);
  #endif

// ------------------------------------------------------------------------------------------------

  // ptr to receive-buffer
  char* recv_buffer;

  int32_t len;

  // FD of the new client connection
  int32_t new_client_fd;

  // sockaddr_in, to get info about new client, contains port, ip-addr, family
  struct sockaddr_in remote_addr;
  struct sockaddr_in local_addr;
  
  // sockaddr, to get info about new client
  struct sockaddr peerName;

  // sockaddr, to get info about server
  struct sockaddr sockName;

// -------------------------------------------------------------------------------------------------

  // Check Flag 'THIS_IS_SERVERSOCKET' in EventTX_CtrlRegA. This indicates a server-socket.
  // -> Manage a new connection.
  if (p_entry_EventTX_definition->EventTX_CtrlRegA & F_THIS_IS_SERVERSOCKET) {

// -------------------------------------------------------------------------------------------------

	// check slot availiability, get a slot no. or RETURN, mark slot as 'in use'
	uint32_t SlotCtrlRegBF = 
		p_entry_EventTX_definition->HTTPD_InstCfg->SlotCtrlRegBF;

	uint8_t new_slot_no;

	// MAX_SLOTS_PER_INSTANCE -> uint32_t BF used in code -> 32 64?
	for ( new_slot_no = 0 ; new_slot_no < 32 ; new_slot_no++ ) {

		if ( ! ( SlotCtrlRegBF & ( 0b1 << new_slot_no ) ))
			break;
	}

	// Check if we got a free slot? -> 'no slots free' error
	if ( new_slot_no >= 32 ) {

		SCDEFn_at_EventTX_M->Log3Fn(p_entry_EventTX_definition->common.name,
			p_entry_EventTX_definition->common.nameLen,
			1,
			"EventTX_DirectRead Error no slots free...\n");

		// do not accept new connection
		return 0;//??
	}

	// mark found slot as used in Slot-Control-Register-Bitfield
	SlotCtrlRegBF |= ( 0b1 << new_slot_no );

	// store Slot-Control-Register-Bitfield
	p_entry_EventTX_definition->HTTPD_InstCfg->SlotCtrlRegBF = SlotCtrlRegBF;

// -------------------------------------------------------------------------------------------------

	#if SCDEH_DBG >= 5
	printf("|accept a new conn>");
	#endif

	len = sizeof(struct sockaddr_in);

	// get FD from new connection and store remote address
	new_client_fd = accept(p_entry_EventTX_definition->common.fd,
		(struct sockaddr *) &remote_addr, (socklen_t *) &len);

	// check for error
	if ( new_client_fd < 0 ) {

		SCDEFn_at_EventTX_M->Log3Fn(p_entry_EventTX_definition->common.name
				,p_entry_EventTX_definition->common.nameLen
				,1
				,"EventTX_DirectRead Error! accept failed...\n");

		// error, process next slot?
		return 0;//??
	}

// -------------------------------------------------------------------------------------------------

	// create a new WebIf Definition
	Entry_EventTX_Definition_t* p_new_entry_EventTX_definition;

	// alloc mem for modul specific definition structure (Common_Definition_t + X)
	p_new_entry_EventTX_definition = 
		(Entry_EventTX_Definition_t*) malloc (sizeof (Entry_EventTX_Definition_t));

	// zero the struct
	memset(p_new_entry_EventTX_definition, 0, sizeof (Entry_EventTX_Definition_t));

// -------------------------------------------------------------------------------------------------
				
	// set parameters for new connection 
	int keep_alive = 1;	//enable keepalive
 	int keep_idle = 60;	//60s
 	int keep_interval = 5;	//5s
 	int keep_count = 3;	//retry times

 	setsockopt(new_client_fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keep_alive, sizeof(keep_alive));
  	setsockopt(new_client_fd, IPPROTO_TCP, TCP_KEEPIDLE, (void*)&keep_idle, sizeof(keep_idle));
  	setsockopt(new_client_fd, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&keep_interval, sizeof(keep_interval));
  	setsockopt(new_client_fd, IPPROTO_TCP, TCP_KEEPCNT, (void *)&keep_count, sizeof(keep_count));
			
 	// store clients FD to new WebIf Definition
 	p_new_entry_EventTX_definition->common.fd = new_client_fd;

	// copy link to HTTPD-Instance-Configuration
	p_new_entry_EventTX_definition->HTTPD_InstCfg
		= p_entry_EventTX_definition->HTTPD_InstCfg;

	// copy ptr to associated module (this module)
	p_new_entry_EventTX_definition->common.module
		= p_entry_EventTX_definition->common.module;

	// store Slot-Number
	p_new_entry_EventTX_definition->slot_no = new_slot_no;

	// clear Flag 'WANTS_WRITE' in new WebIf Definition (normally struct is cleared!)
 	p_new_entry_EventTX_definition->common.Common_CtrlRegA
		&= ~F_WANTS_WRITE;

	// clear Flag 'NEEDS_CLOSE' in new WebIf Definition (normally struct is cleared!)
 	p_new_entry_EventTX_definition->EventTX_CtrlRegA
		&= ~F_NEEDS_CLOSE;

	// get info about new client-conn (port, ip, ...)	
 	socklen_t peerSockAddrLen = sizeof (peerName);
  	getpeername(new_client_fd, &peerName, (socklen_t*) &peerSockAddrLen);
  	struct sockaddr_in* peerSockAddr_in = (struct sockaddr_in*) &peerName;

	// get info about our server-conn (port, ip, ...)	
 	socklen_t sockSockAddrLen = sizeof (sockName);
  	getsockname(new_client_fd, &sockName, (socklen_t *) &sockSockAddrLen);
  	struct sockaddr_in* sockSockAddr_in = (struct sockaddr_in *) &sockName;

	// using TCP, create struct
	esp_tcp *tcp = (esp_tcp*) malloc (sizeof (esp_tcp));

	// using TCP, fill struct
	tcp->local_port = ntohs (sockSockAddr_in->sin_port);	// server port
	tcp->remote_port = ntohs (peerSockAddr_in->sin_port);	// client port
	memcpy(&tcp->remote_ip					// client IP
		, &peerSockAddr_in->sin_addr.s_addr
		, sizeof (tcp->remote_ip));

	// store TCP struct
	p_new_entry_EventTX_definition->proto.tcp = tcp;

	// give definition a new unique name
	p_new_entry_EventTX_definition->common.nameLen = 
		asprintf ((char**)&p_new_entry_EventTX_definition->common.name,
		"%.*s.%d.%d.%d.%d.%u",
		p_entry_EventTX_definition->common.nameLen,
		p_entry_EventTX_definition->common.name,
		p_new_entry_EventTX_definition->proto.tcp->remote_ip[0],
		p_new_entry_EventTX_definition->proto.tcp->remote_ip[1],
		p_new_entry_EventTX_definition->proto.tcp->remote_ip[2],
		p_new_entry_EventTX_definition->proto.tcp->remote_ip[3],
		p_new_entry_EventTX_definition->proto.tcp->remote_port);

  	// assign an unique number
 	 p_new_entry_EventTX_definition->common.nr = SCDERoot_at_EventTX_M->device_count++;

  	// make this definition temporary
	p_new_entry_EventTX_definition->common.defCtrlRegA |= F_TEMPORARY;

	// store new WebIf Definition
	STAILQ_INSERT_HEAD(&SCDERoot_at_EventTX_M->head_definition,
		(Entry_Definition_t*) p_new_entry_EventTX_definition,
		entries);

	#if Telnet_Module_DBG >= 7
	SCDEFn_at_EventTX_M->Log3Fn(p_entry_EventTX_definition->common.name,
		p_entry_EventTX_definition->common.nameLen,
		7,
		"Direct Read Fn (module '%.*s'), F_THIS_IS_SERVERSOCKET set for "
		"this conn. Accepting a new conn. Using slot '%d', FD '%d'. "
		"Creating a 'definition' with F_TEMPORARY set. Exec Conn_Cb Fn",
		p_entry_EventTX_definition->common.module->provided->typeNameLen,
		p_entry_EventTX_definition->common.module->provided->typeName,
		p_new_entry_EventTX_definition->slot_no,
		p_new_entry_EventTX_definition->common.fd);
	#endif

 	// execute WebIf Connect Callback to init
	EventTX_Connect_Cb(p_new_entry_EventTX_definition);
  }

// --------------------------------------------------------------------------------------------------

  // Flag 'THIS_IS_SERVERSOCKET' cleared. This indicates that this is not the server-socket.
  // -> Manage old connection.
  else {

 	#if SCDEH_DBG >= 5
	printf("|recv>");
	#endif

	// malloc our receive buffer
	recv_buffer = (char*) malloc(RECV_BUF_SIZE);

	// got no buffer ? Close / Cleanup connection
	if ( recv_buffer == NULL ) {

		os_printf ("platHttpServerTask: memory exhausted!\n");

		EventTX_Disconnect_Cb (p_entry_EventTX_definition);

		close (p_entry_EventTX_definition->common.fd);

		p_entry_EventTX_definition->common.fd = -1;
	}

	// receive the expected data
	int32_t recv_len = recv (p_entry_EventTX_definition->common.fd,
		recv_buffer, RECV_BUF_SIZE, 0);

	// process, if received data
	if ( recv_len > 0 ) {

		// execute Received Callback
		EventTX_Received_Cb (p_entry_EventTX_definition, recv_buffer, recv_len);
	}

	// or has remote closed the connection ?
	else if ( recv_len == 0 ) {

		// execute Disconnect Callback
		EventTX_Disconnect_Cb (p_entry_EventTX_definition);

		// undefinde this p_entry_EventTX_definition
		EventTX_UndefineRaw (p_entry_EventTX_definition);
	}


	// else we got an error ...
	else {

		// execute Error Callback
		EventTX_Reconnect_Cb (p_entry_EventTX_definition, recv_len);

		// undefinde this p_entry_EventTX_definition
		EventTX_UndefineRaw (p_entry_EventTX_definition);
	}

 	// free our receive buffer
  	free (recv_buffer);
  }

  return 0;
}



/*
 * --------------------------------------------------------------------------------------------------
 *  FName: WebIf Direct Write
 *  Desc: Called from the global select-loop when FD is in write-set
 *  Info: But ONLY if Flag 'Want_Write' in Common_CtrlRegA is set !!!
 *  Para: Entry_Definition_t* p_entry_definition -> the FD owners definition
 *  Rets: ? - unused
 * --------------------------------------------------------------------------------------------------
 */
int 
EventTX_Direct_Write (Entry_Definition_t* p_entry_definition)
{
  // make common ptr to modul specific ptr
  Entry_EventTX_Definition_t* p_entry_EventTX_definition = 
	(Entry_EventTX_Definition_t*) p_entry_definition;

// -------------------------------------------------------------------------------------------------

  #if EventTX_Module_DBG >= 7
  SCDEFn_at_EventTX_M->Log3Fn (p_entry_EventTX_definition->common.name,
	p_entry_EventTX_definition->common.nameLen,
	7,
	"Direct Write Fn (module '%.*s'), entering.",
	p_entry_EventTX_definition->common.module->provided->typeNameLen,
	p_entry_EventTX_definition->common.module->provided->typeName);
  #endif

// --------------------------------------------------------------------------------------------------

  // execute disconnection (indicated by NEEDS_CLOSE flag) or send more data ...
  if ( p_entry_EventTX_definition->EventTX_CtrlRegA & F_NEEDS_CLOSE ) {

	#if EventTX_Module_DBG >= 8
  	SCDEFn_at_EventTX_M->Log3Fn(p_entry_EventTX_definition->common.name,
		p_entry_EventTX_definition->common.nameLen,
		8,
		"Direct Write Fn (module '%.*s'), F_NEEDS_CLOSE set for "
		"this conn. Exec Disconn_Cb Fn and Undefine_Raw Fn.",
		p_entry_EventTX_definition->common.module->provided->typeNameLen,
		p_entry_EventTX_definition->common.module->provided->typeName);
 	 #endif

	// execute Disconnect Callback
	EventTX_Disconnect_Cb (p_entry_EventTX_definition);

	// undefinde this EventTX_Definition
	EventTX_UndefineRaw (p_entry_EventTX_definition);

	// definition gone here ...
  }

// --------------------------------------------------------------------------------------------------

  // chance to send more data by SendCb ...
  else	{

	#if EventTX_Module_DBG >= 8
  	SCDEFn_at_EventTX_M->Log3Fn(p_entry_EventTX_definition->common.name,
		p_entry_EventTX_definition->common.nameLen,
		8,
		"Direct Write Fn (module '%.*s'), this conn is ready to write, "
		"exec Send_Cb Fn. "
		p_entry_EventTX_definition->common.module->provided->typeNameLen,
		p_entry_EventTX_definition->common.module->provided->typeName);
 	 #endif

	// execute Sent Callback
	EventTX_Sent_Cb (p_entry_EventTX_definition);
  }

  return 0;
}



/*
 * --------------------------------------------------------------------------------------------------
 *  FName: EventTX_Initialize
 *  Desc: Initializion of SCDE Function Callbacks of an new loaded module
 *  Info: Stores Module-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 * --------------------------------------------------------------------------------------------------

 */
int 
EventTX_Initialize(SCDERoot_t* SCDERootptr)
{
  // make data root locally available
  SCDERoot_at_EventTX_M = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn_at_EventTX_M = SCDERootptr->SCDEFn;

  SCDEFn_at_EventTX_M->Log3Fn(EventTX_ProvidedByModule.common.typeName,
		  EventTX_ProvidedByModule.common.typeNameLen,
		  3,
		  "InitializeFn called. Type '%.*s' now useable.",
		  EventTX_ProvidedByModule.common.typeNameLen,
		  EventTX_ProvidedByModule.common.typeName);

  return 0;
}



/**
 * --------------------------------------------------------------------------------------------------
 *  FName: EventTX_Set
 *  Desc: Processes the device-specific command line arguments from the set command
 *  Info: Invoked by cmd-line 'Set Telnet_Definition.common.Name setArgs'
 *  Para: Telnet_Definition_t *Telnet_Definition -> WebIF Definition that should get a set cmd
 *        uint8_t *setArgs -> the setArgs
 *        size_t setArgsLen -> length of the setArgs
 *  Rets: strTextMultiple_t* -> response text in allocated memory, NULL=no text
 * --------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* 
EventTX_Set(Common_Definition_t* Common_Definition
	,uint8_t *setArgs
	,size_t setArgsLen)
{
  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // make common ptr to modul specific ptr
  Entry_EventTX_Definition_t* EventTX_Definition =
	(Entry_EventTX_Definition_t*) Common_Definition;

  #if SCDEH_DBG >= 5
  printf("\n|EventTX_Set, Name:%.*s, got args:%.*s>"
	,EventTX_Definition->common.nameLen
	,EventTX_Definition->common.name
	,setArgsLen
	,setArgs);
  #endif


  // response with error text
  retMsg = malloc(sizeof(strTextMultiple_t));

  // response with error text
  retMsg->strTextLen = asprintf(&retMsg->strText
	,"EventTX_Set, Name:%.*s, got args:%.*s"
	,EventTX_Definition->common.nameLen
	,EventTX_Definition->common.name
	,setArgsLen
	,setArgs);

  return retMsg;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: EventTX_Undefine
 * Desc: Removes the define of an "device" of 'WebIf' type. Contains devicespecific init code.
 * Info: Invoked by cmd-line 'Undefine EventTX_Definition.common.Name'
 * Para: Entry_EventTX_Definition_t *EventTX_Definition -> WebIf Definition that should be removed
 * Rets: strTextMultiple_t* -> response text NULL=no text
 *--------------------------------------------------------------------------------------------------
 */
strTextMultiple_t*
EventTX_Undefine(Common_Definition_t* Common_Definition)
{
  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // make common ptr to modul specific ptr
  Entry_EventTX_Definition_t* EventTX_Definition = (Entry_EventTX_Definition_t*) Common_Definition;


  #if SCDEH_DBG >= 5
  printf("\n|EventTX_UnDef, Name:%s>"
	,(char*)EventTX_Definition->common.name);
  #endif

  return retMsg;
}



/**
 * --------------------------------------------------------------------------------------------------
 *  FName: EventTX_Sub
 *  Desc: Processes the device-specific command line arguments from the set command
 *  Info: Invoked by cmd-line 'Set Telnet_Definition.common.Name setArgs'
 *  Para: Telnet_Definition_t *Telnet_Definition -> WebIF Definition that should get a set cmd
 *        uint8_t *setArgs -> the setArgs
 *        size_t setArgsLen -> length of the setArgs
 *  Rets: strTextMultiple_t* -> response text in allocated memory, NULL=no text
 * --------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* 
EventTX_Sub(Common_Definition_t* Common_Definition
	,uint8_t *kArgs
	,size_t kArgsLen)
{
  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  #if SCDEH_DBG >= 5
  printf("\n|EventTX_Sub, Name:%.*s, got key args:%.*s>"
	,Common_Definition->nameLen
	,Common_Definition->name
	,kArgsLen
	,kArgs);
  #endif


  // response with error text
	// alloc mem for retMsg
  retMsg = malloc(sizeof(strTextMultiple_t));

  // response with error text
  retMsg->strTextLen = asprintf(&retMsg->strText
	,"EventTX_Sub, Name:%.*s, got key args:%.*s"
	,Common_Definition->nameLen
	,Common_Definition->name
	,kArgsLen
	,kArgs);

  return retMsg;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: espconn_regist_recvcb
 * Desc: Platform conn - Register data received callback
 * Para: 
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR
EventTX_Regist_Recv_Cb(Entry_EventTX_Definition_t* p_conn,
	espconn_recv_callback p_recv_cb)
{
  p_conn->recv_callback = p_recv_cb;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: espconn_regist_Connectcb
 * Desc: Platform conn - Register disconnected callback
 * Para: 
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR
EventTX_Regist_Connect_Cb(Entry_EventTX_Definition_t* p_conn,
	espconn_connect_callback p_connect_cb)
{
  p_conn->proto.tcp->connect_callback = p_connect_cb;
}



/*
 *--------------------------------------------------------------------------------------------------

 *FName: espconn_regist_reconcb
 * Desc: Platform conn - Register error info callback
 * Para: 
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR
EventTX_Regist_Reconn_Cb(Entry_EventTX_Definition_t* p_conn,
	espconn_reconnect_callback p_reconn_cb)
{
  p_conn->proto.tcp->reconnect_callback = p_reconn_cb;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: espconn_regist_disconcb
 * Desc: Platform conn - Register disconnected callback
 * Para: 
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR
EventTX_Regist_Disconn_Cb(Entry_EventTX_Definition_t* p_conn,
	espconn_connect_callback p_disconn_cb)
{
  p_conn->proto.tcp->disconnect_callback = p_disconn_cb;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: espconn_regist_sentcb
 * Desc: Platform conn - Register data sent callback
 * Para: 
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR
EventTX_Regist_Sent_Cb(Entry_EventTX_Definition_t* p_conn,
	espconn_sent_callback p_send_cb)
{
  p_conn->send_callback = p_send_cb;
}



/*
 * --------------------------------------------------------------------------------------------------
 *  FName: EventTX_Connect
 *  Desc: Called from XXX_DirectRead when a new connection at the SERVERSOCKET is detected 
 *  Info: 
 *  Para: Common_Definition* Common_Definition -> XXX_Definition of the FD owners definition
 *  Rets: ? - unused
 * --------------------------------------------------------------------------------------------------
 */
/*
int 
EventTX_Connect(Common_Definition_t* Common_Definition)
{

  // make common ptr to modul specific ptr
  Entry_EventTX_Definition_t* EventTX_Definition = (Entry_EventTX_Definition_t*) Common_Definition;

  // unused
  return 0;
}
*/








/*
 *--------------------------------------------------------------------------------------------------
 *FName: Connect Cb
 * Desc: Connected callback is triggered in case of new connections to WebIf-HTTP-Daemon
 * Info:
 * Para: Entry_EventTX_Definition_t* WebIfDef -> ptr to the WebIf_Definition
 * Rets: int ->
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR 
EventTX_Connect_Cb (void *arg)
{
  // the arg is a ptr to the platform specific conn
  //struct espconn   *platform_conn = arg;		// ESP 8266 NonOS
  Entry_EventTX_Definition_t* platform_conn = arg;	// ESP 32 RTOS

//--------------------------------------------------------------------------------------------------

  #if HTTPD_Module_DBG >= 7
  SCDEFn_at_EventTX_M->Log3Fn(platform_conn->common.name,
	platform_conn->common.nameLen,
	7,
	"EventTX_Conn_Cb Fn (module '%.*s'), gets slot_no '%d' of '%d', from "
	"remote '%d.%d.%d.%d:%d' to local port '%d', f-heap '%d'.",
	platform_conn->common.module->provided->typeNameLen,
	Platform_conn->common.module->provided->typeName,
	platform_conn->slot_no,
	MAX_SCDED_CONN,
	platform_conn->proto.tcp->remote_ip[0],
	platform_conn->proto.tcp->remote_ip[1],
	platform_conn->proto.tcp->remote_ip[2],
	platform_conn->proto.tcp->remote_ip[3],
	platform_conn->proto.tcp->remote_port,
	platform_conn->proto.tcp->local_port,
	system_get_free_heap_size());
  #endif

//--------------------------------------------------------------------------------------------------

  // alloc HTTPD connection slot
  WebIf_HTTPDConnSlotData_t* conn
	= (WebIf_HTTPDConnSlotData_t*) malloc (sizeof(WebIf_HTTPDConnSlotData_t));

  // zero conn-slot memory
  memset (conn, 0, sizeof (WebIf_HTTPDConnSlotData_t));

  // store current connection in created conn slot
  conn->conn = platform_conn;

 // Write slot number for identification
  conn->slot_no = conn->conn->slot_no;

  // store reverse link
  platform_conn->reverse = conn;

  // register data received callback
  espconn_regist_recvcb (platform_conn, WebIf_RecvCb);

  // register data reconnection / error info callback
  espconn_regist_reconcb (platform_conn, WebIf_ReconCb);

  // register disconnected callback
  espconn_regist_disconcb (platform_conn, WebIf_DisconCb);

  // register data was sent callback
  espconn_regist_sentcb (platform_conn, WebIf_SentCb);

//--------------------------------------------------------------------------------------------------

  // init http parser for the new conn
  HTTPD_ParserInit(conn, HTTP_BOTH);
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: Disconnect Cb
 * Desc: Disconnected callback -> conn is disconnected -> clean up, free memory
 *       its unclear what to do in this cases ...
 * Para: Entry_WebIf_Definition_t* WebIfDef ->ptr to the WebIf_Definition
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR
EventTX_Disconnect_Cb (void* arg)
{
  // the arg is a ptr to the platform specific conn
//struct espconn     *platform_conn = arg;		// ESP 8266 NonOS
  Entry_EventTX_Definition_t* platform_conn = arg;	// ESP 32 RTOS

  // get assigned HTTPD-Connection-Slot-Data
  WebIf_HTTPDConnSlotData_t* conn
	= platform_conn->reverse;

//---------------------------------------------------------------------------------------------------

 #if HTTPD_DBG >= 7
  SCDEFn_at_WebIf_M->Log3Fn(platform_conn->common.name,
	platform_conn->common.nameLen,
	7,
	"EventTX_Disconn_Cb Fn (module '%.*s'), slot_no '%d', from "
	"remote '%d.%d.%d.%d:%d' to local port '%d', f-heap '%d'.",
	platform_conn->common.module->provided->typeNameLen,
	platform_conn->common.module->provided->typeName,
	platform_conn->slot_no,
	platform_conn->proto.tcp->remote_ip[0],
	platform_conn->proto.tcp->remote_ip[1],
	platform_conn->proto.tcp->remote_ip[2],
	platform_conn->proto.tcp->remote_ip[3],
	platform_conn->proto.tcp->remote_port,
	platform_conn->proto.tcp->local_port,
	system_get_free_heap_size());
  #endif

//---------------------------------------------------------------------------------------------------

  // mark connections job in load serializer as finnished (by deleting bit)
  conn->conn->HTTPD_InstCfg->LoadSerializer &= ~( 1 << ( conn->slot_no ) );

  // remove the conn / mark that the connection is gone
  conn->conn = NULL;

  // !!! CONN IS GONE FROM THIS POINT !!!

  // call and indicate cgi to clean up and dealloc
  if ( conn->cgi != NULL ) conn->cgi(conn);

  // free allocated memory for the Send-Buffer, if any
//if (conn->p_send_buffer) free (conn->p_send_buffer);
  if (conn->send_buffer_write_pos) free (conn->p_send_buffer);

  // free allocated memory for Trailing Buff, if any
//if (conn->p_trailing_buffer) free (conn->p_trailing_buffer);
  if (conn->trailing_buffer_len) free (conn->p_trailing_buffer);

  // free stored header-fields
  Entry_Header_Field_t* p_entry_header_field;

  while ( !STAILQ_EMPTY (&conn->head_header_field) ) {

        p_entry_header_field = 
		STAILQ_FIRST (&conn->head_header_field);

	# if SCDED_DBG >= 3
	printf ("|DeallocFldVal:\"%s\" (ID:%d)>",
		p_entry_header_field->p_hdr_fld_val,
		p_entry_header_field->hdr_fld_id);
	# endif

        STAILQ_REMOVE_HEAD (&conn->head_header_field, entries);

	// free the value text
	free (p_entry_header_field->p_hdr_fld_val);

	// and the SLTQ entry
        free (p_entry_header_field);
  }

  // free allocated memory for Path
  if (conn->url) free (conn->url);

  // free allocated memory for Query
  if (conn->getArgs) free (conn->getArgs);

  // free allocated memory for 'body_data'
//  if (conn->p_body_data) free (conn->p_body_data);
  if (conn->body_data_len) free (conn->p_body_data);

  // free allocated memory for Header Field Name Buffer (only in case of error)
  if (conn->p_hdr_fld_name_buff) free (conn->p_hdr_fld_name_buff);

  // free allocated memory for Header Field Value Buffer (only in case of error)
  if (conn->p_hdr_fld_value_buff) free(conn->p_hdr_fld_value_buff);

  # if SCDED_DBG >= 3
  printf("\ndone, freeing conn>");
  #endif

  // finally free allocated memory for this conn (WebIf_HTTPDConnSlotData_t struct)
  free (conn);
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: Reconnect Cb
 * Desc: Reconnect Info callback is triggered when the connection to client conn of SCDED is broken
 *       its unclear what to do in this cases ...
 * Para: void *arg -> struct espconn *conn
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR 
EventTX_Reconnect_Cb (void* arg, int8_t error)
{
  // the arg is a ptr to the platform specific conn
  //struct espconn   *platform_conn = arg;	// ESP 8266 NonOS
  Entry_EventTX_Definition_t* platform_conn = arg;	// ESP 32 RTOS

  // get assigned HTTPD-Connection-Slot-Data
  WebIf_HTTPDConnSlotData_t *conn
	= platform_conn->reverse;

  # if SCDED_DBG >= 3
  printf("\nEventTX ReconCb, slot %d, remote:%d.%d.%d.%d:%d,local port:%d, error:%d, mem:%d>"
	,platform_conn->slot_no
	,platform_conn->proto.tcp->remote_ip[0]
	,platform_conn->proto.tcp->remote_ip[1]
	,platform_conn->proto.tcp->remote_ip[2]
	,platform_conn->proto.tcp->remote_ip[3]
	,platform_conn->proto.tcp->remote_port
	,platform_conn->proto.tcp->local_port
	,error
	,system_get_free_heap_size());

  #endif

//---------------------------------------------------------------------------------------------------

  // process error information here! 
/*
sint8 err : error code
ESCONN_TIMEOUT - Timeout
ESPCONN_ABRT - TCP connection aborted
ESPCONN_RST - TCP connection abort
ESPCONN_CLSD - TCP connection closed
ESPCONN_CONN - TCP connection
ESPCONN_HANDSHAKE - TCP SSL handshake fail
ESPCONN_PROTO_MSG - SSL application invalid
*/

  WebIf_DisconCb(arg);
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: Idle Cb
 * Desc: Idle-Callback is triggered when connection has requested an Idle-Callback by ConnCtrl register.
 *       (if F_GENERATE_IDLE_CALLBACK in ConnCtrl is set -> request must be renewed in every cycle!!!)  
 * Para: void *arg -> struct espconn *conn
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR 
EventTX_Idle_Cb (void* arg)
{

  // the arg is a ptr to the platform specific conn
  //struct espconn   *platform_conn = arg;	// ESP 8266 NonOS
  Entry_EventTX_Definition_t* platform_conn = arg;	// ESP 32 RTOS

  // get assigned HTTPD-Connection-Slot-Data
  WebIf_HTTPDConnSlotData_t *conn
	= platform_conn->reverse;

  # if SCDED_DBG >= 3
  printf("\nEventTX IdleCb, slot:%d, remote:%d.%d.%d.%d:%d, local port:%d, iat:%d, mem:%d>"
	,conn->slot_no
	,conn->conn->proto.tcp->remote_ip[0]
	,conn->conn->proto.tcp->remote_ip[1]
	,conn->conn->proto.tcp->remote_ip[2]
	,conn->conn->proto.tcp->remote_ip[3]
	,conn->conn->proto.tcp->remote_port
	,conn->conn->proto.tcp->local_port
	,conn->InactivityTimer
	,system_get_free_heap_size());
  #endif

//--------------------------------------------------------------------------------------------------

  // set Connection-Control Flags - for Idle-Callback
  // CLR: F_GENERATE_IDLE_CALLBACK, because now we have Idle Callback and it is needed once ...
  // CLR: F_CALLED_BY_RXED_CALLBACK, because this is not RX-Callback ...
  // CLR: F_CALLED_BY_TXED_CALLBACK, because this is not TX-Callback ...
  conn->ConnCtrlFlags &= ~(F_GENERATE_IDLE_CALLBACK + F_CALLED_BY_RXED_CALLBACK + F_CALLED_BY_TXED_CALLBACK);
  // SET:  
//conn->ConnCtrlFlags |=  (none);

//--------------------------------------------------------------------------------------------------

  // Response to open connection...
  SCDED_Process_Open_Connection(conn);
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: Sent Cb
 * Desc: Data was sent callback is triggered when data was sent to a client conn of SCDED
 * Para: Entry_WebIf_Definition_t* WebIfDef ->ptr to the WebIf_Definition
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR 
EventTX_Sent_Cb (void* arg)
{
  // the arg is a ptr to the platform specific conn
  //struct espconn   *platform_conn = arg;		// ESP 8266 NonOS
  Entry_EventTX_Definition_t* platform_conn = arg;	// ESP 32 RTOS

  // get assigned HTTPD-Connection-Slot-Data
  WebIf_HTTPDConnSlotData_t* conn = platform_conn->reverse;

//---------------------------------------------------------------------------------------------------

 #if HTTPD_DBG >= 7
  SCDEFn_at_WebIf_M->Log3Fn(platform_conn->common.name,
	platform_conn->common.nameLen,
	7,
	"EventTX_Sent_Cb Fn (module '%.*s'), slot_no '%d', from "
	"remote '%d.%d.%d.%d:%d' to local port '%d', f-heap '%d'.",
	platform_conn->common.module->provided->typeNameLen,
	platform_conn->common.module->provided->typeName,
	platform_conn->slot_no,
	platform_conn->proto.tcp->remote_ip[0],
	platform_conn->proto.tcp->remote_ip[1],
	platform_conn->proto.tcp->remote_ip[2],
	platform_conn->proto.tcp->remote_ip[3],
	platform_conn->proto.tcp->remote_port,
	platform_conn->proto.tcp->local_port,
	system_get_free_heap_size());
  #endif

//--------------------------------------------------------------------------------------------------

  # if SCDED_DBG >= 1
  if (! (conn->ConnCtrlFlags & F_TXED_CALLBACK_PENDING) ) {
	printf("|Err! TXedCbFlag missing>");
  }
  #endif

  // set Connection-Control Flags - for Sent-Callback
  // CLR: F_GENERATE_IDLE_CALLBACK, because now we have a callback. No Idle Callback is needed ...
  // CLR: F_TXED_CALLBACK_PENDING, because SentCb is fired. TXED_CALLBACK is no longer pending ...
  // CLR: F_CALLED_BY_RXED_CALLBACK, because this is not RX-Callback ...
  conn->ConnCtrlFlags &= ~(F_GENERATE_IDLE_CALLBACK + F_TXED_CALLBACK_PENDING + F_CALLED_BY_RXED_CALLBACK);
  // SET:   F_CALLED_BY_TXED_CALLBACK, because this is TX-Callback ...
  conn->ConnCtrlFlags |=  (F_CALLED_BY_TXED_CALLBACK);

//--------------------------------------------------------------------------------------------------

  // Respond to open connection...
  SCDED_Process_Open_Connection(conn);
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: Received Cb
 * Desc: Received callback is triggered when a data block is received from an client conn of SCDED
 * Para: void *arg -> struct espconn *conn
 *       char *recvdata -> ptr to received data
 *       unsigned short recvlen -> length of received data 
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR
EventTX_Received_Cb (void* arg, char* p_recv_data, unsigned short recv_data_len)
{
  // the arg is a ptr to the platform specific conn
  //struct espconn   *platform_conn = arg;		// ESP 8266 NonOS
  Entry_EventTX_Definition_t* platform_conn = arg;	// ESP 32 RTOS

  // get assigned HTTPD-Connection-Slot-Data
  WebIf_HTTPDConnSlotData_t *conn
	= platform_conn->reverse;

//---------------------------------------------------------------------------------------------------

 #if HTTPD_DBG >= 7
  SCDEFn_at_WebIf_M->Log3Fn(platform_conn->common.name,
	platform_conn->common.nameLen,
	7,
	"EventTX_Recv_Cb Fn (module '%.*s'), slot_no '%d', from "
	"remote '%d.%d.%d.%d:%d' to local port '%d', len '%d', f-heap '%d'.",
	platform_conn->common.module->provided->typeNameLen,
	platform_conn->common.module->provided->typeName,
	platform_conn->slot_no,
	platform_conn->proto.tcp->remote_ip[0],
	platform_conn->proto.tcp->remote_ip[1],
	platform_conn->proto.tcp->remote_ip[2],
	platform_conn->proto.tcp->remote_ip[3],
	platform_conn->proto.tcp->remote_port,
	platform_conn->proto.tcp->local_port,
	recv_data_len,
	system_get_free_heap_size());
  #endif

//--------------------------------------------------------------------------------------------------

  # if SCDED_DBG >= 5
  SCDEFn_at_WebIf_M->HexDumpOutFn ("RX-HEX", p_recv_data, recv_data_len);
  # endif

//--------------------------------------------------------------------------------------------------

  // set Connection-Control Flags - for Received-Callback
  // CLR: F_GENERATE_IDLE_CALLBACK, because now we have Idle Callback and it is needed once ...
  // CLR: F_CALLED_BY_TXED_CALLBACK, because this is not TX-Callback ...
  conn->ConnCtrlFlags &= ~(F_GENERATE_IDLE_CALLBACK + F_CALLED_BY_TXED_CALLBACK);
  // SET: F_CALLED_BY_RXED_CALLBACK, because this is RX-Callback ...
  conn->ConnCtrlFlags |= (F_CALLED_BY_RXED_CALLBACK);

//--------------------------------------------------------------------------------------------------

  // always parse received data	
  size_t nparsed = http_parser_execute (conn,
	&conn->conn->HTTPD_InstCfg->HTTPDparser_settings,
	p_recv_data,
	(size_t) recv_data_len);

  # if SCDED_DBG >= 5
  printf("|Msg parsed, (%d from %d)>",
	nparsed,
	recv_data_len);
  # endif

//---------------------------------------------------------------------------------------------------

  // continue or error ?
  if (nparsed != (size_t) recv_data_len) {

	# if SCDED_DBG >= 1
	printf("|Err: %u (%s)>"
		,HTTPD_ErrnoCode(conn->parser_http_errno)
		,HTTPD_ErrnoDescription(conn->parser_http_errno));
	# endif

	// give status about error to client
	SCDED_StartRespHeader2(conn);

	// stop processing more data
	conn->cgi = CGI_FINNISHED;
  }

//---------------------------------------------------------------------------------------------------

  // Upgrade connection to websockets?
  else if (conn->parser_upgrade == 1) {

	# if SCDED_DBG >= 3
	printf("|Upgrade conn>");
	# endif

	// can not answer..
	conn->cgi = CGI_UPGRADE_WEBSOCKET;
  }

//---------------------------------------------------------------------------------------------------

  // Response to open connection now	
  SCDED_Process_Open_Connection(conn);
}



















/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDEETX_Connect
 * Desc: Connects the SCDE-Event TX connection (should be called from 10Hz timer !)
 *       conditions:
 *       + SCDE-ETX enabled && not connected
 *       + Queue filled || "TX Keep Alive to RX" is activated
 *       includes Connect Control by status
 * Para: -/-
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR
EventTX_Manage_Connection (Entry_EventTX_Definition_t* p_entry_eventtx_definition)
{
  // is the SCDE-Event TX activated ?
  if (1) { //SPZ if (SysCfgRamFlashMirrored->MySCDEETX_Cfg.ETX_GenOpt & F_ENABLE_SCDE_EVENT_TX) {

	// is the SCDE-Event TX Slot NOT connected ?
  	if ( p_entry_eventtx_definition->reverse == NULL ) {

		// is SCDE-E-TX-Queue filled or SCDE-E-TX "TX Keep Alive to RX" activated -> we need  a connection
		if (1) { //SPZ		if ( (SCDEETXQ_GetNumJobs() != 0) || (SysCfgRamFlashMirrored->MySCDEETX_Cfg.ETX_GenOpt & F_TX_KEEP_CONN_ALIVE) )

			# if SCDEETX_DBG >= 5
			printf("\nSCDEETX CCS:%d",
				p_entry_eventtx_definition->ETXCC_State);
			# endif

			switch ( p_entry_eventtx_definition->ETXCC_State ) {

//---------------------------------------------------------------------------------------------------

				// is E-TX-Connect-Control State > s_idle? -> Connect Control not active (do make new connection try)
				case s_idle:

				# if SCDEETX_DBG >= 3
				printf("SCDEETX ConnTry\n");
				# endif

				# if SCDEETX_DBG >= 5
				// Display SCDE-Event-TX Queue for debugging
				SCDEETXQ_DisplayJobs();

				printf("SCDEETX CCS:%d. Statistics:Conn=%d; Msg=%d;RcvOK=%d;RcvNOK=%d\n",
					p_entry_eventtx_definition->ETXCC_State,
					p_entry_eventtx_definition->SCDEETX_ConnCNT,
					p_entry_eventtx_definition->SCDEETX_MsgCNT,
					p_entry_eventtx_definition->SCDEETX_RcvOKCNT,
					p_entry_eventtx_definition->SCDEETX_RcvNOKCNT);
  				# endif

				// Try to extract IP ...
				if (1) /*spz			
				if (UTILS_StrToIP((int8_t*) SysCfgRamFlashMirrored->MySCDEETX_Cfg.Domain,
					&SysCfgRamFlashMirrored->MySCDEETX_Cfg.ip))*/

					// we have IP! skipping DNS lookup...
					{
/*
	/ spz				SCDEETX_IPConnect((char *)SysCfgRamFlashMirrored->MySCDEETX_Cfg.Domain,
						(ip_addr_t*) &SysCfgRamFlashMirrored->MySCDEETX_Cfg.ip.addr,
						&SCDEETXConn);/
*/
					}

  				else

					// we have NO IP! we need DNS lookup to get IP
					{

					//SCDEETX_DNSLookup(&SCDEETXConn);

					}

				break;

//---------------------------------------------------------------------------------------------------

				// is E-TX-Connect-Control State > s_dns_lookup_failed? -> Connect Control detects DNS lookup failure
				case s_dns_lookup_failed:

				# if SCDEETX_DBG >= 1
				printf("SCDEETX DNS-Lookup TIMEOUT!\n");
				# endif

				// restart
				p_entry_eventtx_definition->ETXCC_State = s_idle;

				break;

//---------------------------------------------------------------------------------------------------

				// is E-TX-Connect-Control State > s_conn_proc_IP_failed? -> Connect Control detects connection to IP failed
				case s_conn_proc_IP_failed:

				# if SCDEETX_DBG >= 1
				printf("SCDEETX ConnCb TIMEOUT!\n");
				# endif

				// restart
				p_entry_eventtx_definition->ETXCC_State = s_idle;

				break;

//---------------------------------------------------------------------------------------------------

				default:
				p_entry_eventtx_definition->ETXCC_State--;
			}
		}
	}
  }
}




//##################################################################################################
//### FName: SCDEETX_IPConnect
//###  Desc: Connect with IP (->DNS Lookup complete) - starting Connection
//###  Para: const char *name -> const char *DomainNameDestination
//###        ip_addr_t *ip    -> ip_addr_t *IPDestination
//###	     void *espconn    -> struct espconn *espconn
//###  Rets: NONE
//##################################################################################################
void ICACHE_FLASH_ATTR
EventTX_IPConnect(const char* name, ip_addr_t* ip, Entry_EventTX_Definition_t* p_entry_eventtx_definition)
{

//spz  static esp_tcp tcp;

  //struct espconn *pespconn = (struct espconn *)arg;

  if ( ip == NULL ) {

	# if SCDEETX_DBG >= 1
	printf("SCDEETX dest. IP inv.!\n");
	# endif

	// Event-TX-Connect-Control new state: s_delay_then_idle (because IP we got is invalid)
	p_entry_eventtx_definition->ETXCC_State = s_delay_then_idle;

	return;
  }

  # if SCDEETX_DBG >= 3
  printf("SCDEETX conn. to remote:%d.%d.%d.%d:%d from local port:%d\n",
	*((uint8 *)&ip->addr),
	*((uint8 *)&ip->addr + 1),
	*((uint8 *)&ip->addr + 2),
	*((uint8 *)&ip->addr + 3),
	SysCfgRamFlashMirrored->MySCDEETX_Cfg.Dest_Port,
	SysCfgRamFlashMirrored->MySCDEETX_Cfg.ESP_Port);
  # endif

  // prepare connection structure
  p_entry_eventtx_definition->type = ESPCONN_TCP;
  p_entry_eventtx_definition->state = ESPCONN_NONE;
//  p_entry_eventtx_definition->proto.tcp = &tcp; its there in allocated mem
  p_entry_eventtx_definition->proto.tcp->local_port = 23;//SysCfgRamFlashMirrored->MySCDEETX_Cfg.ESP_Port;
  p_entry_eventtx_definition->proto.tcp->remote_port = 66;//SysCfgRamFlashMirrored->MySCDEETX_Cfg.Dest_Port;
  memcpy(p_entry_eventtx_definition->proto.tcp->remote_ip, &ip->addr, 4);

  espconn_regist_connectcb(pespconn, SCDED_PlatformETXSlotConnCb);

  int8_t Result = espconn_connect(pespconn);

  if (Result) {

	# if SCDEETX_DBG >= 1
	printf("SCDEETX ConErr:%d!\n",
		Result);
	# endif

	// Event-TX-Connect-Control new state: s_delay_then_idle (because connect func gives us an error)
	ETXCC_State = s_delay_then_idle;

	return;
  }

  // setup defaults for this conn 
  espconn_set_opt(pespconn,0b11); // free imediatly+disable nagle
//espconn_set_opt(pespconn,0b1011); // free imediatly+disable nagle+TCP keep alive

  // Event-TX-Connect-Control new state: s_waked_up_monitoring_conn_proc_IP #02 (monitoring event conncb)
  ETXCC_State = s_monitoring_conn_proc_IP;
}


