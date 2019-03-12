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
#include "stdatomic.h"

//define mySPI ESP32_SPI KEY=ARG


#include <ProjectConfig.h>
#include <esp8266.h>
#include <Platform.h>

// the Smart-Connected-Device-Engine structures & types ..
#include <SCDE_s.h>

// provides WebIf, need the structures & types ...
#include "WebIf_Module.h"

// this Modules structures & types ...
#include "ESP32_SPI_Module_s.h"
#include "ESP32_SPI_Module.h"

// -------------------------------------------------------------------------------------------------

// set default build verbose - if no external override
#ifndef ESP32_SPI_Module_DBG  
#define ESP32_SPI_Module_DBG  5	// 5 is default
#endif 

// -------------------------------------------------------------------------------------------------






//#include "SCDE_Main.h"
//#include "driver/gpio.h"




// ptr zur Fehlermeldung in allokierter strTextMultiple_t WENN NOCH KEIN FEHLER WAR
//#define SCDE_OK  NULL

/*
static const char* I2S_TAG = "I2S";
#define SCDE_CHECK(a, str, ret) if (!(a)) {                                              \
        ESP_LOGE(I2S_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);       \
        return (ret);                                                                   \
        }
//  I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
*/



// SCDE_CHECK MAKRO: Prüft Bedingung und gibt Fehler in allokierter strTextMultiple_t zurück
#define SCDE_CHECK(a, str) if (!(a)) {							\
	retMsg = malloc(sizeof(strTextMultiple_t));					\
	retMsg->strTextLen = asprintf(&retMsg->strText					\
		,"Error! '%s'"								\
		,str);									\
	return retMsg;									\
	}






























// macro to CHECK, if FALSE -> load error msg in strTextMultiple_t and return
#define SCDE_CHECK_LOAD_ERROR_TEXT_GOTO(a, str, dest) do {			\
  if (!(a)) {									\
	/* alloc mem for retMsg */						\
	p_retMsg = malloc(sizeof(strTextMultiple_t));				\
										\
	/* response with error text */						\
	p_retMsg->strTextLen = asprintf(&p_retMsg->strText			\
		,"Parsing Error! %s(%d): %s!",					\
		__FUNCTION__, __LINE__, str);					\
	goto dest;								\
  }										\
} while(0)






// macro to CHECK, if FALSE -> load error msg in strTextMultiple_t and return
#define SCDE_CHECK_LOADERRORTXT_RET(a, str) do {				\
  if (!(a)) {									\
	/* alloc mem for retMsg */						\
	retMsg = malloc(sizeof(strTextMultiple_t));				\
										\
	/* response with error text */						\
	retMsg->strTextLen = asprintf(&retMsg->strText				\
		,"Parsing Error! %s(%d): %s!",					\
		__FUNCTION__, __LINE__, str);					\
	return retMsg;								\
  }										\
} while(0)

// macro to CHECK, if FALSE -> load error msg in strTextMultiple_t and return
#define SCDE_CHECK_LOADERRORTXT_RET2(a, str, ...) do {				\
  if (!(a)) {									\
	/* alloc mem for retMsg */						\
	retMsg = malloc(sizeof(strTextMultiple_t));				\
										\
	/* response with error text */						\
	retMsg->strTextLen = asprintf(&retMsg->strText				\
		,"Parsing Error! %s(%d): "str,					\
		 __FUNCTION__, __LINE__, ##__VA_ARGS__);			\
	return retMsg;								\
  }										\
} while(0)

// macro to CHECK, if FALSE -> only load error msg in strTextMultiple_t and NO! return
#define SCDE_CHECK_LOADERRORTXT(a, str) do {					\
  if (!(a)) {									\
	/* alloc mem for retMsg	 */						\
	retMsg = malloc(sizeof(strTextMultiple_t));				\
										\
	/* response with error text */						\
	retMsg->strTextLen = asprintf(&retMsg->strText				\
		,"Parsing Error! %s(%d): %s!",					\
		__FUNCTION__, __LINE__, str);					\
  }										\
} while(0)




























// -------------------------------------------------------------------------------------------------

// set default build verbose - if no external override
#ifndef ESP32_SPI_Module_DBG  
#define ESP32_SPI_Module_DBG  5	// 5 is default
#endif 

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

// ESP32_SPI_SET_DISENA -> 'SDA_IO_PULLUP' -> Disabled|Enabled
//                             -> 'SCL_IO_PULLUP' -> Disabled|Enabled
SelectAData ESP32_SPI_DisEna[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"Disabled"},
  {1,"Enabled"}, 
  {0, NULL}
  };

// ESP32_SPI_SET_I2C_MODE -> 'I2C_MODE' -> Master|Slave
SelectAData ESP32_SPI_I2C_MODE[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"Master"},
  {1,"Slave"}, 
  {0, NULL}
  };

/*
// ESP32_SPI_SET_IDLE_LV -> 'IDLE_LV' -> Low|High
SelectAData ESP32_SPI_LoHi[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"Low"},
  {1,"High"}, 
  {0, NULL}
  };
// ESP32_SPI_SET_BLOCK -> 'PWM_BLOCK'-> High_Speed|Low_Speed
SelectAData ESP32_SPI_PWM_BLOCK[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"High_Speed"},
  {1,"Low_Speed"},
  {0, NULL}
  };
// ESP32_SPI_SET_TICK_SOURCE -> 'TICK_SOURCE'-> REF|APB_SLOW
SelectAData ESP32_SPI_TICK_SOURCE[] = {  //ID, Text MAX CGI LEN BEACHTEN!!!
  {0,"REF"},
  {1,"APB_SLOW"}, 
  {0, NULL}
  };
*/



/**
 * For Type: ESP32_SPI
 * Implemented-Args-Keys, for input (MAX 64!). //IAK!!
 * KEYs-Field-Set for Feature specific query. Will be generated by SCDEH_ParseStrToparsedKVInput()
 * parsed result is stored in ESP32_SPI_parsedKVInput_t
 */
enum ESP32_SPI_SET_IK {				// Bit #XX for debugging

  // Block #1 I2C Block to use
    ESP32_SPI_SET_I2C_NUM			// Bit #01 'I2C_NUM' -> 

  // Block #2 I2C Master/Slave ?
  , ESP32_SPI_SET_I2C_MODE			// Bit #02 'I2C_MODE' -> 

  // Block #3 I2C Pin connections
  , ESP32_SPI_SET_SDA_IO				// Bit #03 'SDA_IO' -> 
  , ESP32_SPI_SET_SDA_IO_PULLUP			// Bit #04 'SDA_IO_PULLUP' -> 
  , ESP32_SPI_SET_SCL_IO				// Bit #05 'SCL_IO' ->
  , ESP32_SPI_SET_SCL_IO_PULLUP			// Bit #06 'SCL_IO_PULLUP' ->

  // Block #4 I2C Pin connections
  , ESP32_SPI_SET_MASTER_CLOCK			// Bit #07 'MASTER_CLOCK' ->

  // Block #5 I2C Pin connections
  , ESP32_SPI_SET_SLAVE_10BIT_ENA		// Bit #08 'SLAVE_10BIT_ENA' ->

  // Block #6 I2C Pin connections
  , ESP32_SPI_SET_SLAVE_ADRESS			// Bit #09 'SLAVE_ADRESS' ->

// end marker
  , ESP32_SPI_SET_NUMBER_OF_IK			// Bit #10 MAX 64 IMPLEMENTED !

};

 

/**
 * For Type: ESP32_SPI
 * Implemented readings (MAX 32!)
 * Can be assigned to Implemented Keys, if affected
 */
enum ESP32_SPI_Readings {			// Bit #XX for debugging

   ESP32_SPI_R_DUTY		= (1<<0)	// Bit #00 'Duty'  -> 

  ,ESP32_SPI_R_RESOLUTION	= (1<<1)	// Bit #01 'Resolution' -> 

  ,ESP32_SPI_R_NAME		= (1<<2)	// Bit #02 'name' + caps + ufid -> 

};



/**
 * For Type: ESP32_SPI
 * Implemented KEYS and assigned readings for this query - analyzed by by http_parser_parse_url()
 * Num should meet enum XX_QueryFields
 * Result is stored in struct SCDE_XX_parsedKVInput
 */
kvParseImplementedKeys_t ESP32_SPI_Set_ImplementedKeys[] = {
// |                     affectedreadings						       	|  CMD

// for usage

// for configuration
   { 0												,"I2C_NUM" }		// #01
  ,{ 0												,"I2C_MODE" }		// #02
  ,{ 0												,"SDA_IO" }		// #03
  ,{ 0												,"SDA_IO_PULLUP" }	// #04
  ,{ 0												,"SCL_IO" }		// #05
  ,{ 0												,"SCL_IO_PULLUP" }	// #06
  ,{ 0												,"MASTER_CLOCK" }	// #07
  ,{ 0												,"SLAVE_10BIT_ENA" }	// #08
  ,{ 0												,"SLAVE_ADRESS" }	// #09
 
}; // number of elements should be equal with XX_SET_IK_Number_of_keys, LIMIT IS 64 ELEMENTS !



/**
 * -------------------------------------------------------------------------------------------------
 *  DName: ESP32_SPI_ActiveResourcesDataA_forWebIf
 *  Desc: Resource-Content-structure of active Directory - PART A (Resource-Data-Row)
 *  Data: WebIf_ActiveResourcesDataA_t[X] from HttpD.h
 * -------------------------------------------------------------------------------------------------
 */
// Content:   AllowedMethodBF          | AllowedDocMimeBF  | AllowedSchemeBF |free|CgiNo| EnaByBit | Url
const WebIf_ActiveResourcesDataA_t ESP32_SPI_ActiveResourcesDataA_forWebIf[] = {  //ICACHE_RODATA_ATTR = 

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
 *  DName: ESP32_SPI_ActiveResourcesDataB_forWebIf
 *  Desc: Resource-Content-structure of active Directory - PART B (Procedure-Call-Data-Row) 
 *  Data: WebIf_ActiveResourcesDataB_t[X] from HttpD.h
 * -------------------------------------------------------------------------------------------------
 */
// CgiFucID=(No.<<16)+AllowedSchemeBF |      cgi            |     cgi_data
const WebIf_ActiveResourcesDataB_t ESP32_SPI_ActiveResourcesDataB_forWebIf[] =  {  //ICACHE_RODATA_ATTR =

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



// -------------------------------------------------------------------------------------------------



/**
 * -------------------------------------------------------------------------------------------------
 *  DName: ESP32_SPI_provided_Fn
 *  Desc: Assigns common + custom functions this Module provides to the SCDE (and client Modules)
 *  Data: ESP32_SPI_provided_fn_t
 * -------------------------------------------------------------------------------------------------
 */
ESP32_SPI_ProvidedByModule_t ESP32_SPI_ProvidedByModule = {   // A-Z order
{
// --- first the common module functions ---
  "ESP32_SPI",		// Type-Name of module -> on Linux libfilename.so !
  9,						// size of Type-Name

  NULL,						// Add
  ESP32_SPI_Attribute,				// Attribute
  ESP32_SPI_Define,				// Define
  NULL, 					// Delete
  NULL, 					// DirectRead
  NULL,						// DirectWrite
  NULL,						// Except
  NULL,						// Get
  ESP32_SPI_IdleCb,				// IdleCb
  ESP32_SPI_Initialize,				// Initialize
  NULL,						// Notify
  NULL,						// Parse
  NULL,						// Read
  NULL,						// Ready
  NULL,						// Rename
  ESP32_SPI_Set,				// Set
  ESP32_SPI_Shutdown,				// Shutdown
  NULL, 					// State
  NULL, 					// Sub
  ESP32_SPI_Undefine,				// Undefine
  ESP32_SPI_Write,				// Write
  NULL,						// FnProvided
  sizeof(ESP32_SPI_Definition_t)		// Modul specific Size (Common_Definition_t + X)
},
// --- now the custom module fuctions ---
   ESP32_SPI_bus_add_device,			// adds an device to the definitions host
   ESP32_SPI_bus_remove_device,			// removes an device from the definitions host
   ESP32_SPI_device_queue_trans,		// to queue an interrupt driven transaction
   ESP32_SPI_device_get_trans_result,		// to get result of an interrupt driven transaction
   ESP32_SPI_device_transmit,			// to send transaction, waits to complete, return result
   ESP32_SPI_device_polling_start,		// to immediately start a polling transaction
   ESP32_SPI_device_polling_end,		// to poll until the transaction ends
   ESP32_SPI_device_polling_transmit,		// send polling transaction, wait, return result
   ESP32_SPI_device_acquire_bus,		// to occupy the SPI bus for transactions on a device
   ESP32_SPI_device_release_bus,		// to release the occupied SPI bus 
   ESP32_SPI_cal_clock,				// to calculate the working frequency
   ESP32_SPI_get_timing,			// to calculate the timing settings of specified freq.
   ESP32_SPI_get_freq_limit			// to get the frequency limit of current configuration
};



/* -------------------------------------------------------------------------------------------------
 *  FName: ESP32_SPI_Attribute
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
ESP32_SPI_Attribute(Common_Definition_t *Common_Definition
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

  printf("ESP32_SPI_AttributeFN for defName:%.*s -> attrCmd:%.*s attrName:%.*s attrVal:%.*s\n"
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







/*
#include "freertos/ringbuf.h"


#include "soc/i2c_struct.h"
#include "soc/i2c_reg.h"
#include "driver/periph_ctrl.h"

static const char* I2C_TAG = "i2c";

#define I2C_CHECK(a, str, ret)  if(!(a)) {                                             \
        ESP_LOGE(I2C_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);      \
        return (ret);                                                                   \
        }

*/



/** public function
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_SPI_Define
 *  Desc: Finalizes the creation of the requested Definition of this Module. Includes executing type
 *        specific inits.
 *  Info: Invoked by cmd-line 'Define custom_definition_name module_name'
 *  Para: Common_Definition_t* p_Common_Definition -> prefilled 'This'_Module Definition (should be casted)
 *  Rets: strTextMultiple_t* p_retMsg (error text) -> forces VETO! ; NULL = SCDE_OK
 * -------------------------------------------------------------------------------------------------
 */
strTextMultiple_t*
ESP32_SPI_Define(Common_Definition_t* p_Common_Definition)
{

  // make common ptr to modul specific ptr
  ESP32_SPI_Definition_t* p_ESP32_SPI_Definition =
		  (ESP32_SPI_Definition_t*) p_Common_Definition;

  // for Fn response msg
  strTextMultiple_t* p_retMsg = SCDE_OK;

// -------------------------------------------------------------------------------------------------

  #if ESP32_SPI_Module_DBG >= 5
  SCDEFn_at_ESP32_SPI_M->Log3Fn(p_Common_Definition->name,
	p_Common_Definition->nameLen,
	5,
	"DefineFn of Module '%.*s' is called to continue creation of Definition '%.*s' with args '%.*s'.",
	p_ESP32_SPI_Definition->common.module->provided->typeNameLen,
	p_ESP32_SPI_Definition->common.module->provided->typeName,
	p_ESP32_SPI_Definition->common.nameLen,
	p_ESP32_SPI_Definition->common.name,
	p_ESP32_SPI_Definition->common.definitionLen,
	p_ESP32_SPI_Definition->common.definition);
  #endif

// ------------------------------------------------------------------------------------------------

  // new conversation
  uint8_t* p_defArgsText = p_Common_Definition->definition;
  size_t defArgsTextLen = p_Common_Definition->definitionLen;

  // Check for args. This type requires args...
  if (!defArgsTextLen) {

	// alloc mem for retMsg
	p_retMsg = malloc(sizeof(strTextMultiple_t));

	// response with error text
	p_retMsg->strTextLen = asprintf(&p_retMsg->strText
		,"Parsing Error! Expected Args!");

	return p_retMsg;
  }

// ------------------------------------------------------------------------------------------------

//  xSemaphoreTake(p_ESP32_SPI_Definition->def_mux
//    ,portMAX_DELAY);

// ------------------------------------------------------------------------------------------------

  // store FD to Definition. Will than be processed in global loop ... -> THIS MODULE USES NO FD
  p_ESP32_SPI_Definition->common.fd = -1;

  // store table of function callbacks provided & made accessible for client modules
//  p_ESP32_SPI_Definition->ESP32_SPI_Fn = &ESP32_SPI_Fn;

// ------------------------------------------------------------------------------------------------

  // init WebIf_Provided offset
//  ESP32Control_Definition->common.link =
//	&ESP32Control_Definition->WebIf_Provided;

  // check for loaded Module 'WebIf' -> get provided Fn
  p_ESP32_SPI_Definition->WebIf_Provided.WebIf_FnProvided =
	NULL;//(WebIf_FnProvided_t *) SCDEFn_at_ESP32_SPI_M->GetFnProvidedByModule("WebIf");

 // Providing data for WebIf? Initialise data provided for WebIf
  if (p_ESP32_SPI_Definition->WebIf_Provided.WebIf_FnProvided) {

	p_ESP32_SPI_Definition->WebIf_Provided.ActiveResourcesDataA =
		(WebIf_ActiveResourcesDataA_t *) &ESP32_SPI_ActiveResourcesDataA_forWebIf;

	p_ESP32_SPI_Definition->WebIf_Provided.ActiveResourcesDataB =
		(WebIf_ActiveResourcesDataB_t *) &ESP32_SPI_ActiveResourcesDataB_forWebIf;
  }

  else	{

	SCDEFn_at_ESP32_SPI_M->Log3Fn(p_Common_Definition->name
		,p_Common_Definition->nameLen
		,1
		,"Could not enable WebIf support for '%.*s'. Type '%.*s' detects Type 'WebIf' is NOT loaded!"
		,p_ESP32_SPI_Definition->common.nameLen
		,p_ESP32_SPI_Definition->common.name
		,p_ESP32_SPI_Definition->common.module->provided->typeNameLen
		,p_ESP32_SPI_Definition->common.module->provided->typeName);
  }

// ------------------------------------------------------------------------------------------------

  // Parse define-args (KEY=VALUE) protocol -> gets parsedKVInput in allocated mem, NULL = ERROR
  parsedKVInputArgs_t* p_parsedKVInput = 
	SCDEFn_at_ESP32_SPI_M->ParseKVInputArgsFn(ESP32_SPI_SET_NUMBER_OF_IK	// Num Implementated KEYs MAX
	,ESP32_SPI_Set_ImplementedKeys						// Implementated Keys
	,p_defArgsText								// our args text
	,defArgsTextLen);							// our args text len

  // parsing may report an problem. args contain: unknown keys, double keys, ...?
  if (!p_parsedKVInput) {

	// alloc mem for retMsg
	p_retMsg = malloc(sizeof(strTextMultiple_t));

	// response with error text
	p_retMsg->strTextLen = asprintf(&p_retMsg->strText,
		"Parsing Error! Args '%.*s' not taken! Check the KEYs!",
		defArgsTextLen,
		p_defArgsText);

	if (p_parsedKVInput) {

		free(p_parsedKVInput);
	}

	return p_retMsg;
  }

// ------------------------------------------------------------------------------------------------

  // try 1 - arguments for configuration as i2c master ?

  // set required Keys -> Keys that should be there in this request
  p_parsedKVInput->requiredKVBF = 	( (1 << ESP32_SPI_SET_I2C_NUM)
					| (1 << ESP32_SPI_SET_I2C_MODE)
					| (1 << ESP32_SPI_SET_SDA_IO)
					| (1 << ESP32_SPI_SET_SDA_IO_PULLUP)
					| (1 << ESP32_SPI_SET_SCL_IO)
					| (1 << ESP32_SPI_SET_SCL_IO_PULLUP)
					| (1 << ESP32_SPI_SET_MASTER_CLOCK)
					);

  // set forbidden Keys -> Keys that are not allowed in this request
  p_parsedKVInput->forbiddenKVBF = 	( (1 << ESP32_SPI_SET_SLAVE_10BIT_ENA)
					| (1 << ESP32_SPI_SET_SLAVE_ADRESS)
 					);
/*
  // process the set-args (key=value@) protocol
  p_retMsg = ESP32_SPI_ProcessKVInputArgs(p_ESP32_SPI_Definition,
		p_parsedKVInput,	// KVInput parsed
		p_defArgsText,		// our args text
		defArgsTextLen);	// our args text len*/

  // processing reports an problem. Args not taken. Return with MSG
  if (p_retMsg) {

	// free allocated memory for query result key-field
	free(p_parsedKVInput);

	return p_retMsg;
  }

// ------------------------------------------------------------------------------------------------


// init spi here - MANUAL VALUES for now

/* we need:
REQ:
PIN_NUM_MISO
PIN_NUM_MOSI
PIN_NUM_CLK

opt:
PIN_NUM_QUADWP (Write Protect if used)
PIN_NUM_QUADDH (HolD if used)
MAX_TRANSFER_SIZE (Defaults to 4094 if not assigned)
IRQ_FLAGS ?
*/

// Define which spi bus to use VSPI_HOST or HSPI_HOST, NOT! SPI_HOST
#define SPI_BUS_PHERIPERAL HSPI_HOST

#define PIN_NUM_MISO 19		// SPI MISO
#define PIN_NUM_MOSI 23		// SPI MOSI
#define PIN_NUM_CLK  18		// SPI CLOCK pin



// ------------------------------------------------------------------------------------------------

  // alloc mem for the bus configuration (ESP32_SPI_bus_config_t)
  ESP32_SPI_bus_config_t* p_bus_config =
	malloc(sizeof(ESP32_SPI_bus_config_t));

  // check if we got memory? Else load retMsg an goto NOMEMORY
  SCDE_CHECK_LOAD_ERROR_TEXT_GOTO(p_bus_config,
		"Could not allocate ESP32_SPI_bus_config_t", error_cleanup_1);

  // store the bus config configuration
  p_ESP32_SPI_Definition->p_bus_config = p_bus_config;

  // zero / start clean ESP32_SPI_bus_config_t
  memset(p_bus_config, 0, sizeof(ESP32_SPI_bus_config_t));

  // initial fill ...

  p_bus_config = &(ESP32_SPI_bus_config_t) {
	.miso_io_num = PIN_NUM_MISO,		// set SPI MISO pin
	.mosi_io_num = PIN_NUM_MOSI,		// set SPI MOSI pin
	.sclk_io_num = PIN_NUM_CLK,		// set SPI CLK pin
	.quadwp_io_num = -1,
	.quadhd_io_num = -1,
	.max_transfer_sz = 6 * 1024,		// PARALLEL_LINES*320*2+8
};

// ------------------------------------------------------------------------------------------------

  // Initialize this Definitions SPI bus
  p_retMsg = ESP32_SPI_bus_initialize(p_ESP32_SPI_Definition,	// the Definition which wants a spi host
	SPI_BUS_PHERIPERAL,					// the SPI pheriperal the host uses
	p_bus_config,						// the bus configuration of the pheriperal
	1);

  // an error occured, deinit, free mem
  if (p_retMsg) goto error_cleanup_2;

  #if ESP32_SPI_Module_DBG >= 5
  SCDEFn_at_ESP32_SPI_M->Log3Fn(p_Common_Definition->name,
	p_Common_Definition->nameLen,
	5,
	"Definition '%.*s' (Module '%.*s') initializes SPI bus host. (MISO:%d, MOSI:%d, SCLK:%d, "
	"QUADWP:%d, QUADHD:%d, max.tx-size:%d.",
	p_ESP32_SPI_Definition->common.nameLen,
	p_ESP32_SPI_Definition->common.name,
	p_ESP32_SPI_Definition->common.module->provided->typeNameLen,
	p_ESP32_SPI_Definition->common.module->provided->typeName,
	p_bus_config->miso_io_num,
	p_bus_config->mosi_io_num,
	p_bus_config->sclk_io_num,
	p_bus_config->quadwp_io_num,
	p_bus_config->quadhd_io_num,
	p_bus_config->max_transfer_sz);
  #endif

/*
// ------------------------------------------------------------------------------------------------

  // set required Keys -> Keys that should be there in this request
  p_parsedKVInput->requiredKVBF = ( (1 << ESP32_SPI_SET_I2C_NUM)
			        | (1 << ESP32_SPI_SET_I2C_MODE)
			        | (1 << ESP32_SPI_SET_SDA_IO)
			        | (1 << ESP32_SPI_SET_SDA_IO_PULLUP)
			        | (1 << ESP32_SPI_SET_SCL_IO)
			        | (1 << ESP32_SPI_SET_SCL_IO_PULLUP) );

  // set forbidden Keys -> Keys that are not allowed in this request
  p_parsedKVInput->forbiddenKVBF = 0;

  // process the set-args (key=value@) protocol
  if (ESP32_SPI_ProcessKVInputArgs(p_ESP32_SPI_Definition
    ,p_parsedKVInput				// KVInput parsed
    ,p_defArgsText				// our args text
    ,defArgsTextLen) ) {			// our args text len

    // Processing reports an problem. Args not taken. Response with error text.

    // alloc mem for retMsg
    p_retMsg = malloc(sizeof(strTextMultiple_t));

    // response with error text
    p_retMsg->strTextLen = asprintf(&retMsg->strText
      ,"Processing Error! Args '%.*s' not taken! Check the VALUEs!"
      ,defArgsTextLen
      ,p_defArgsText);

    // free allocated memory for query result key-field
    free(p_parsedKVInput);

    return p_retMsg;
  }

// ------------------------------------------------------------------------------------------------
*/
  // set affected readings
 // ESP32_SPI_SetAffectedReadings(p_ESP32_SPI_Definition
  //  ,p_parsedKVInput->affectedReadingsBF);

// ------------------------------------------------------------------------------------------------

  // free allocated memory for query result key-field
  //free(p_parsedKVInput);


// ------------------------------------------------------------------------------------------------

  // set up 1st idle Callback
  p_ESP32_SPI_Definition->common.Common_CtrlRegA |= F_WANTS_IDLE_TASK;

  return p_retMsg;

// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// alternative end in case of errors - free all allocated things and return with retMsg as veto.

error_cleanup_2:

  // the ESP32_SPI_bus_config_t is allocated here
  free(p_ESP32_SPI_Definition->p_bus_config);

error_cleanup_1:
/*
  if (p_parsedKVInput) {

    free(p_parsedKVInput);

  }

*/
  return p_retMsg;
}




/**
 * --------------------------------------------------------------------------------------------------
 *  FName: ESP32_SPI_DirectWrite
 *  Desc: for 2 stage designs - called to give write job to 1st stage (this stage)
 *  Info: 
 *  Para: ESP32_SPI_Definition_t *ESP32_SPI_Definition -> WebIF Definition that should be removed
 *  Rets: strTextMultiple_t* -> response text NULL=no text
 * --------------------------------------------------------------------------------------------------
 */
/*
strTextMultiple_t *
ESP32_SPI_DirectWrite(
	 Common_Definition_t *Common_Definition_Stage1
	,Common_Definition_t *Common_Definition_Stage2
	,Common_StageXCHG_t *Common_StageXCHG)
{
  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;
  // to adress 1st stage definition - make common ptr to modul specific ptr
  ESP32_SPI_Definition_t *ESP32_SPI_Definition_Stage1 =
	(ESP32_SPI_Definition_t*) Common_Definition_Stage1;
  // make common ptr to modul specific ptr
  ESP32_SPI_StageXCHG_t *ESP32_SPI_StageXCHG =
	(ESP32_SPI_StageXCHG_t*) Common_StageXCHG;
// -------------------------------------------------------------------------------------------------
// check 4 matching modules
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
  #if ESP32_SPI_Module_DBG >= 5
  SCDEFn_at_ESP32_SPI_M->Log3Fn(
	 ESP32_SPI_Definition_Stage1->common.name
	,ESP32_SPI_Definition_Stage1->common.nameLen
	,5
	,"DirectWriteFn of Definition '%.*s' (Module '%.*s') is called "
  	 "from Definition '%.*s' (Module '%.*s') to exec write job."
	,ESP32_SPI_Definition_Stage1->common.nameLen
	,ESP32_SPI_Definition_Stage1->common.name
	,ESP32_SPI_Definition_Stage1->common.module->provided->typeNameLen
	,ESP32_SPI_Definition_Stage1->common.module->provided->typeName
	,Common_Definition_Stage2->nameLen
	,Common_Definition_Stage2->name
	,Common_Definition_Stage2->module->provided->typeNameLen
	,Common_Definition_Stage2->module->provided->typeName);
  #endif
// -------------------------------------------------------------------------------------------------
  return retMsg;
}
*/


/*
 * ------------------------------------------------------------------------------------------------
 *  FName: ESP32_SPI_IdleCb	
 *  Desc: Timed callback, X Hz, to do type dependend processing tasks.
 *  Info: 
 *  Para: Common_Definition_t *Common_Definition -> prefilled WebIf Definition 
 *  Rets: ? unclear char* -> response text NULL=no text
 * ------------------------------------------------------------------------------------------------
 */
int //feedModuleTask
ESP32_SPI_IdleCb(Common_Definition_t *Common_Definition)
{

  // make common ptr to modul specific ptr
  ESP32_SPI_Definition_t* ESP32_SPI_Definition =
		  (ESP32_SPI_Definition_t*) Common_Definition;

  #if ESP32_SPI_Module_DBG >= 5
  printf("\n|ESP32_SPI_IdleCb, Def:%.*s>"
	,ESP32_SPI_Definition->common.nameLen
	,ESP32_SPI_Definition->common.name);
  #endif

// -------------------------------------------------------------------------------------------




  // set up next idle Callback
  ESP32_SPI_Definition->common.Common_CtrlRegA |= F_WANTS_IDLE_TASK;

  return 0;

}



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_SPI_Initialize
 *  Desc: Initializion of SCDE Function Callbacks of an new loaded module
 *  Info: Stores Module-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
ESP32_SPI_Initialize(SCDERoot_t* SCDERootptr)
{
  // make data root locally available
  SCDERoot_at_ESP32_SPI_M = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn_at_ESP32_SPI_M = SCDERootptr->SCDEFn;

  SCDEFn_at_ESP32_SPI_M->Log3Fn(ESP32_SPI_ProvidedByModule.common.typeName
		  ,ESP32_SPI_ProvidedByModule.common.typeNameLen
		  ,3
		  ,"InitializeFn called. Type '%.*s' now useable.\n"
		  ,ESP32_SPI_ProvidedByModule.common.typeNameLen
		  ,ESP32_SPI_ProvidedByModule.common.typeName);

  return 0;
}



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_SPI_Set
 *  Desc: Processes the device-specific command line arguments from the set command
 *  Info: Invoked by cmd-line 'Set ESP32Control_Definition.common.Name setArgs'
 *  Para: ESP32Control_Definition_t *ESP32Control_Definition -> WebIF Definition that should get a set cmd
 *        uint8_t *setArgsText -> the setArgsText
 *        size_t setArgsTextLen -> length of the setArgsText
 *  Rets: strTextMultiple_t* -> response text in allocated memory, NULL=no text
 * -------------------------------------------------------------------------------------------------
 */
strTextMultiple_t *
ESP32_SPI_Set(Common_Definition_t* Common_Definition
	,uint8_t *setArgsText
	,size_t setArgsTextLen)
{

  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // make common ptr to modul specific ptr
  ESP32_SPI_Definition_t* ESP32_SPI_Definition =
	(ESP32_SPI_Definition_t*) Common_Definition;

  #if ESP32_SPI_Module_DBG >= 5
  printf("\n|ESP32_SPI_Set, Name:%.*s, got args:%.*s>"
	,ESP32_SPI_Definition->common.nameLen
	,ESP32_SPI_Definition->common.name
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
	SCDEFn_at_ESP32_SPI_M->ParseKVInputArgsFn(ESP32_SPI_SET_NUMBER_OF_IK	// Num Implementated KEYs MAX for Set Fn
	,ESP32_SPI_Set_ImplementedKeys		// Implementated Keys for Set Fn
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
//  parsedKVInput->forbiddenKVBF = ( (1 << ESP32_SPI_SET_CNT_UNIT) );
  parsedKVInput->forbiddenKVBF = 0;

  // process the set-args (key=value@) protocol
  if (ESP32_SPI_ProcessKVInputArgs(ESP32_SPI_Definition
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
  ESP32_SPI_SetAffectedReadings(ESP32_SPI_Definition
	,parsedKVInput->affectedReadingsBF);

// ---------------------

  // free allocated memory for query result key-field
  free(parsedKVInput);

  return retMsg;

}



/**
 * --------------------------------------------------------------------------------------------------
 *  FName: ESP32_SPI_Shutdown
 *  Desc: called after 
 *  Info: Invoked by cmd-line 'Undefine ESP32_SPI_Definition.common.Name'
 *  Para: ESP32_SPI_Definition_t *ESP32_SPI_Definition -> WebIF Definition that should be removed
 *  Rets: strTextMultiple_t* -> response text NULL=no text
 * --------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* ICACHE_FLASH_ATTR
ESP32_SPI_Shutdown(Common_Definition_t *Common_Definition)
{

  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // make common ptr to modul specific ptr
  ESP32_SPI_Definition_t* ESP32_SPI_Definition =
	(ESP32_SPI_Definition_t*) Common_Definition;

// -------------------------------------------------------------------------------------------------

  #if ESP32_SPI_Module_DBG >= 5
  printf("\n|ESP32_SPI_Shutdown, Name:%.*s>"
	,ESP32_SPI_Definition->common.nameLen
	,ESP32_SPI_Definition->common.name);
  #endif

// -------------------------------------------------------------------------------------------------


  return retMsg;
}



/**
 * --------------------------------------------------------------------------------------------------
 *  FName: ESP32_SPI_Undefine
 *  Desc: Removes the requested Definition of this Module. Includes executing type specific cleanup.
 *  Info: Invoked by cmd-line 'Undefine custom_definition_name'
 *  Para: Common_Definition_t* p_Common_Definition -> An Definition of this Module to remove (should be casted)
 *  Rets: strTextMultiple_t* -> p_retMsg (error text) -> forces VETO! ; NULL=SCDE_OK
 * --------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* ICACHE_FLASH_ATTR
ESP32_SPI_Undefine(Common_Definition_t* p_Common_Definition)
{
  // make common ptr to modul specific ptr
  ESP32_SPI_Definition_t* p_ESP32_SPI_Definition =
	(ESP32_SPI_Definition_t*) p_Common_Definition;

  // for Fn response msg
  strTextMultiple_t* p_retMsg = SCDE_OK;

// -------------------------------------------------------------------------------------------------

  #if ESP32_SPI_Module_DBG >= 5
  SCDEFn_at_ESP32_SPI_M->Log3Fn(p_Common_Definition->name,
	p_Common_Definition->nameLen,
	5,
	"UndefineFn of Module '%.*s' is called to delete Definition '%.*s'.",
	p_ESP32_SPI_Definition->common.module->provided->typeNameLen,
	p_ESP32_SPI_Definition->common.module->provided->typeName,
	p_ESP32_SPI_Definition->common.nameLen,
	p_ESP32_SPI_Definition->common.name);
  #endif

// ------------------------------------------------------------------------------------------------

  // Free this Definitions SPI bus / host
  if (p_ESP32_SPI_Definition) { 
	p_retMsg = ESP32_SPI_bus_free(p_ESP32_SPI_Definition);

	// an error occured, VETO, skip undefine
	if (p_retMsg) goto error_cancel_undefine_1;
  }

  // the ESP32_SPI_bus_config_t is allocated here
  free(p_ESP32_SPI_Definition->p_bus_config);

// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

// alternative end in case of veto - stop undefine with error msg
error_cancel_undefine_1:

  return p_retMsg;
}



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_SPI_Write
 *  Desc: Is called to write data to an 'Definition' of 'ESP32_SPI' Module.
 *  Info: 
 *  Para: Common_Definition_t* Common_Definition -> the 'Definition' that should write the data 
 *                                                  (cast to ESP32_SPI_Definition_t*)
 *        xString_t data          -> data that should be written (characters in allocated mem + len) 
 *  Rets: xMultipleStringSLTQE_t* -> response: ptr to an singly linked tail queue element with return
 *                                   message in allocated memory, or NULL = OK / No message
 * -------------------------------------------------------------------------------------------------
 */
xMultipleStringSLTQE_t*
ESP32_SPI_Write(Common_Definition_t *Common_Definition,
		       xString_t data)
{

  // make common ptr to modul specific ptr
  ESP32_SPI_Definition_t* ESP32_SPI_Definition =
		  (ESP32_SPI_Definition_t*) Common_Definition;

  // for Fn response msg
  xMultipleStringSLTQE_t *retMsgMultipleStringSLTQE = SCDE_OK;

// -------------------------------------------------------------------------------------------------

  #if ESP32_SPI_Module_DBG >= 7
  SCDEFn_at_ESP32_SPI_M->Log3Fn(Common_Definition->name
	,Common_Definition->nameLen
	,7
	,"WriteFn of Module '%.*s' is called for Definition '%.*s'. Got %d bytes raw data."
	,ESP32_SPI_Definition->common.module->provided->typeNameLen
	,ESP32_SPI_Definition->common.module->provided->typeName
	,ESP32_SPI_Definition->common.nameLen
	,ESP32_SPI_Definition->common.name
	,data.length);
  #endif

// -------------------------------------------------------------------------------------------------


  return retMsgMultipleStringSLTQE;
}


















/*
 * -------------------  helpers provided to module for type operation ------------------------------
 */



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_SPI_ProcessKVInputArgs
 *  Desc: Processes the KEY=VALUE input from Args. Information is pre-parsed to *parsedKVInput
 *  Info: .requiredKV -> Keys that should be there / .forbiddenKV -> Keys that are not allowed
 *  Para: ESP32_SPI_Definition_t *ESP32_SPI_Definition -> Definition that should get the input
 *        parsedKVInputArgs_t *parsedKVInput -> ptr to allocated mem filled with results from parse
 *        uint8_t *argsText -> the ArgsText
 *        size_t argsTextLen -> length of the ArgsText
 *  Rets: strTextMultiple_t* -> SCDE_OK, or error txts - for log
 * -------------------------------------------------------------------------------------------------
 */
strTextMultiple_t*
ESP32_SPI_ProcessKVInputArgs(ESP32_SPI_Definition_t *ESP32_SPI_Definition,
	parsedKVInputArgs_t *parsedKVInput,
	uint8_t *argsText,
	size_t argsTextLen)
{
 printf("|0");
  // start without error
  strTextMultiple_t *retMsg = SCDE_OK;


 // remember the readings affected by the parsing process
  uint32_t affectedReadings = 0;

// -------------------------------------------------------------------------------------------------
// 1. Step: Create backup structures, if required mirror current effective values.
// -------------------------------------------------------------------------------------------------
/*
  // Block #01 Backup the current I2C-Block
  uint8_t new_i2c_num;
  new_i2c_num = ESP32_SPI_Definition->i2c_num;

// -------------------------------------------------------------------------------------------------

  // Block #02 Backup the current I2C mode
  i2c_mode_t new_i2c_mode;
  new_i2c_mode = ESP32_SPI_Definition->i2c_config.mode;

// -------------------------------------------------------------------------------------------------

  // Block #03 Backup the current SDA & SCL pin configuration used by I2C
  gpio_num_t new_sda_io_num;
  gpio_pullup_t new_sda_pullup_en;
  gpio_num_t new_scl_io_num;
  gpio_pullup_t new_scl_pullup_en;

  new_sda_io_num = ESP32_SPI_Definition->i2c_config.sda_io_num;
  new_sda_pullup_en = ESP32_SPI_Definition->i2c_config.sda_pullup_en;
  new_scl_io_num = ESP32_SPI_Definition->i2c_config.scl_io_num;
  new_scl_pullup_en = ESP32_SPI_Definition->i2c_config.scl_pullup_en;

// -------------------------------------------------------------------------------------------------

  // Block #04 Backup the current I2C clock speed
  uint32_t new_clk_speed;
  new_clk_speed = ESP32_SPI_Definition->i2c_config.master.clk_speed;

// -------------------------------------------------------------------------------------------------

  // Block #05 Backup the current setting for 10Bit adressing in slave mode
  uint8_t new_addr_10bit_en;
  new_addr_10bit_en = ESP32_SPI_Definition->i2c_config.slave.addr_10bit_en;

// -------------------------------------------------------------------------------------------------

  // Block #06 Backup the current I2C slave adress in slave
  uint8_t new_slave_addr;
  new_slave_addr = ESP32_SPI_Definition->i2c_config.slave.slave_addr;

// -------------------------------------------------------------------------------------------------
*/











// ------------------------------------------------------------------------------------------------
/*  
  // we need to process the 'Block' KEY in advance here, because we need this information early
  // Block=High_Speed|Low_Speed
  // -> Setzt den Hardware-PWM-Block des ESP32 (0=HighSpeed / 1= LowSpeed) der verwendet werden soll
  // -> Sets the hardware-PWM-Block of the ESP32 (0=HighSpeed / 1= LowSpeed) that should be used
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SPI_SET_BLOCK) ) {
	// valid input happened ?
	if (SCDEH_GetQueryKeyID((uint8_t*) argsText + parsedKVInput->keyData_t[ESP32_SPI_SET_BLOCK].off
		,parsedKVInput->keyData_t[ESP32_SPI_SET_BLOCK].len, &newBlock, ESP32_SPI_PWM_BLOCK)
		&& (newBlock <= 1) ) {
*/
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
/*
		}
	// invalid input detected ?
	else return true;
  }
*/
// ------------------------------------------------------------------------------------------------
  /*
  // we need to process the 'CHANNEL' KEY in advance here, because we need this information early
  // CHANNEL=[0-7]
  // -> Setzt den Hardware-PWM-Kanal des ESP32 (0-7) der verwendet werden soll
  // -> Sets the hardware-pwm-channel of the ESP32 (0-7) that should be used
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SPI_SET_CHANNEL) ) {
	// valid input happened ?
	if (SCDEH_GetDecUInt8Val((uint8_t*) argsText + parsedKVInput->keyData_t[ESP32_SPI_SET_CHANNEL].off
		,parsedKVInput->keyData_t[ESP32_SPI_SET_CHANNEL].len, &newChannel) 
		&& (newChannel <= 7) ) {
*/
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
/*
		}
	// invalid input detected ?
	else return true;
  }
*/
// ------------------------------------------------------------------------------------------------
/*  
  // we need to process the 'TIMER' KEY in advance here, because we need this information early
  // TIMER=[0-3]
  // -> Setzt den Hardware-Timer f\FCr den gew\E4hlten PWM-Kanal des ESP32 (0-3) der verwendet werden soll
  // -> Sets the hardware-timer for the choosen pwm-channel of the ESP32 (0-3) that should be used
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SPI_SET_TIMER) ) {
	// valid input happened ?
	if (SCDEH_GetDecUInt8Val((uint8_t*) argsText + parsedKVInput->keyData_t[ESP32_SPI_SET_TIMER].off
		,parsedKVInput->keyData_t[ESP32_SPI_SET_TIMER].len, &newTimer) 
		&& (newTimer <= 3) ) {
*/
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
/*
		}
	// invalid input detected ?
	else return true;
  }
*/
// ------------------------------------------------------------------------------------------------
/*
  // block #4 get current assigned / connected GPIO
  uint8_t newGPIO = ESP32_SPI_Definition->GPIO;
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
*/



// ------------------------------------------------------------------------------------------------
// 2. Step: Process the possible input keys



// -------------------------------------------------------------------------------------------------
// I2C_NUM=[0-9] -> Setzt den I2C-Block der verwendet werden soll (0-1) 
// I2C_NUM=[0-9] -> Set the I2C-Block that should be used (0-1) 
/*
  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_SPI_SET_I2C_NUM) {

	uint8_t number;

	// valid input happened ?
	if (SCDEH_GetDecUInt8Val(argsText + parsedKVInput->keyData_t[ESP32_SPI_SET_I2C_NUM].off
		,parsedKVInput->keyData_t[ESP32_SPI_SET_I2C_NUM].len
		,&number)) {

		if (number < I2C_NUM_MAX) {

			// valid, take it
			new_i2c_num = number;

			// set affected reading
			affectedReadings |= 
				parsedKVInput->keyData_t[ESP32_SPI_SET_I2C_NUM].affectedReadings;

//			// Push processed CMD to to Response ...
//			RespArgsWPos += sprintf( RespArgsWPos,"cmd=I2C_NUM");
		}
	}

	// invalid input detected ?
	else return true;
  }

// --------------------------------------------------------------------------------------------------
// I2C_MODE=[Master|Slave] -> Setzt den I2C modus ()
// I2C_MODE=[Master|Slave] -> Set the I2C mode ()

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_SPI_SET_I2C_MODE) {

	uint8_t number;

	// valid input happened ?
	if (SCDEH_GetQueryKeyID(argsText+parsedKVInput->keyData_t[ESP32_SPI_SET_I2C_MODE].off
		,parsedKVInput->keyData_t[ESP32_SPI_SET_I2C_MODE].len, &number, ESP32_SPI_I2C_MODE)) {

		// valid, take it
		new_i2c_mode = number;

		// mark affected readings for TX
		affectedReadings |= 
			parsedKVInput->keyData_t[ESP32_SPI_SET_I2C_MODE].affectedReadings;

//		// Push processed CMD to to Response ...
//		RespArgsWPos += sprintf( RespArgsWPos,"cmd=I2C_MODE");
	}

	// invalid input detected ?
	else return true;
  }

// --------------------------------------------------------------------------------------------------
// SDA_IO=[0-9] -> Setzt den SDA IO Pin für I2C (?)
// SDA_IO=[0-9] -> Set the SDA IO Pin for I2C (?)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_SPI_SET_SDA_IO) {

	uint8_t number;

	// valid input happened ?
	if (SCDEH_GetDecUInt32Val(argsText + parsedKVInput->keyData_t[ESP32_SPI_SET_SDA_IO].off
		,parsedKVInput->keyData_t[ESP32_SPI_SET_SDA_IO].len, &number)) {

		if (GPIO_IS_VALID_OUTPUT_GPIO(number)) {

			// valid, take it
			new_sda_io_num = number;

			// mark affected readings for TX
			affectedReadings |= 
				parsedKVInput->keyData_t[ESP32_SPI_SET_SDA_IO].affectedReadings;

//			// Push processed CMD to to Response ...
//			RespArgsWPos += sprintf( RespArgsWPos,"cmd=SDA_IO");
		}

		// invalid input detected ?
		else return true;
	}

	// invalid input detected ?
	else return true;
  }

// --------------------------------------------------------------------------------------------------
// SDA_IO_PULLUP=[Disabled|Enabled] -> Setzt einen Pullup-Widerstand am SDA IO Pin für I2C ()
// SDA_IO_PULLUP=[Disabled|Enabled] -> Set an pullup-resistor at SDA IO Pin for I2C ()

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_SPI_SET_SDA_IO_PULLUP) {

	uint8_t number;

	// valid input happened ?
	if (SCDEH_GetQueryKeyID(argsText+parsedKVInput->keyData_t[ESP32_SPI_SET_SDA_IO_PULLUP].off
		,parsedKVInput->keyData_t[ESP32_SPI_SET_SDA_IO_PULLUP].len, &number, ESP32_SPI_DisEna)) {

		if ((number == GPIO_PULLUP_ENABLE && GPIO_IS_VALID_OUTPUT_GPIO(new_sda_io_num)) ||
			number == GPIO_PULLUP_DISABLE) {

			// valid, take it
			new_sda_pullup_en = number;

			// mark affected readings for TX
			affectedReadings |= 
				parsedKVInput->keyData_t[ESP32_SPI_SET_SDA_IO_PULLUP].affectedReadings;

//			// Push processed CMD to to Response ...
//			RespArgsWPos += sprintf( RespArgsWPos,"cmd=SDA_IO_PULLUP");
		}

		// invalid input detected ?
		else return true;
	}

	// invalid input detected ?
	else return true;
  }

// --------------------------------------------------------------------------------------------------
// SCL_IO=[0-9] -> Setzt den SCL IO Pin für I2C (?)
// SCL_IO=[0-9] -> Set the SCL IO Pin for I2C (?)

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_SPI_SET_SCL_IO) {

	uint8_t number;

	// valid input happened ?
	if (SCDEH_GetDecUInt32Val(argsText + parsedKVInput->keyData_t[ESP32_SPI_SET_SCL_IO].off
		,parsedKVInput->keyData_t[ESP32_SPI_SET_SCL_IO].len, &number)) {

		if ((GPIO_IS_VALID_OUTPUT_GPIO(number)) ||
    			(GPIO_IS_VALID_GPIO(number) && new_i2c_mode == I2C_MODE_SLAVE)) {

			// valid, take it
			new_scl_io_num = number;

			// mark affected readings for TX
			affectedReadings |= 
				parsedKVInput->keyData_t[ESP32_SPI_SET_SCL_IO].affectedReadings;

//			// Push processed CMD to to Response ...
//			RespArgsWPos += sprintf( RespArgsWPos,"cmd=SCL_IO");
		}

		// invalid input detected ?
		else return true;
	}

	// invalid input detected ?
	else return true;
  }

// --------------------------------------------------------------------------------------------------
// SCL_IO_PULLUP=[Disabled|Enabled] -> Setzt einen Pullup-Widerstand am SCL IO Pin für I2C ()
// SCL_IO_PULLUP=[Disabled|Enabled] -> Set an pullup-resistor at SCL IO Pin for I2C ()

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_SPI_SET_SCL_IO_PULLUP) {

	uint8_t number;

	// valid input happened ?
	if (SCDEH_GetQueryKeyID(argsText+parsedKVInput->keyData_t[ESP32_SPI_SET_SCL_IO_PULLUP].off
		,parsedKVInput->keyData_t[ESP32_SPI_SET_SCL_IO_PULLUP].len, &number, ESP32_SPI_DisEna)) {

		if ((number == GPIO_PULLUP_ENABLE && GPIO_IS_VALID_OUTPUT_GPIO(new_scl_io_num)) ||
			number == GPIO_PULLUP_DISABLE) {

			// valid, take it
			new_scl_pullup_en = number;

			// mark affected readings for TX
			affectedReadings |= 
				parsedKVInput->keyData_t[ESP32_SPI_SET_SCL_IO_PULLUP].affectedReadings;

//			// Push processed CMD to to Response ...
//			RespArgsWPos += sprintf( RespArgsWPos,"cmd=CLA_IO_PULLUP");
		}

		// invalid input detected ?
		else return true;
	}

	// invalid input detected ?
	else return true;
  }

// -------------------------------------------------------------------------------------------------
// MASTER_CLOCK=[0-9] -> Setzt den I2C-Takt (0-1) 
// MASTER_CLOCK=[0-9] -> Set the I2C-Clock (0-1) 

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_SPI_SET_MASTER_CLOCK) {

	uint32_t number;

	// valid input happened ?
	if (SCDEH_GetDecUInt32Val(argsText + parsedKVInput->keyData_t[ESP32_SPI_SET_MASTER_CLOCK].off
		,parsedKVInput->keyData_t[ESP32_SPI_SET_MASTER_CLOCK].len, &number)) {

//		if (number < 65000) { //?????????????????????????????

			// valid, take it
			new_clk_speed = number;

			// set affected reading
			affectedReadings |= 
				parsedKVInput->keyData_t[ESP32_SPI_SET_MASTER_CLOCK].affectedReadings;

//			// Push processed CMD to to Response ...
//			RespArgsWPos += sprintf( RespArgsWPos,"cmd=MASTER_CLOCK");
//		}
	}

	// invalid input detected ?
	else return true;
  }

// --------------------------------------------------------------------------------------------------
// SLAVE_10BIT_ENA=[Disabled|Enabled] -> Setzt 10Bit Adressierung - im Slave-Mode für I2C ()
// SLAVE_10BIT_ENA=[Disabled|Enabled] -> Set 10Bit adresing - in slave-mode for I2C ()

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_SPI_SET_SLAVE_10BIT_ENA) {

	uint8_t number;

	// valid input happened ?
	if (SCDEH_GetQueryKeyID(argsText+parsedKVInput->keyData_t[ESP32_SPI_SET_SCL_IO_PULLUP].off
		,parsedKVInput->keyData_t[ESP32_SPI_SET_SLAVE_10BIT_ENA].len, &number, ESP32_SPI_DisEna)) {

//		if (number < 65000) { //?????????????????????????????


			// valid, take it
			new_addr_10bit_en = number;

			// mark affected readings for TX
			affectedReadings |= 
				parsedKVInput->keyData_t[ESP32_SPI_SET_SLAVE_10BIT_ENA].affectedReadings;

//			// Push processed CMD to to Response ...
//			RespArgsWPos += sprintf( RespArgsWPos,"cmd=SLAVE_10BIT_ENA");
//		}

		// invalid input detected ?
//		else return true;
	}

	// invalid input detected ?
	else return true;
  }

// -------------------------------------------------------------------------------------------------
// SLAVE_ADRESS=[0-9] -> Setzt die I2C-Slave-Adrsse (0-1) 
// SLAVE_ADRESS=[0-9] -> Set the I2C-Slave-Adress (0-1) 

  if (parsedKVInput->keysFoundBF & (uint64_t) 1 << ESP32_SPI_SET_SLAVE_ADRESS) {

	uint8_t number;

	// valid input happened ?
	if (SCDEH_GetDecUInt8Val(argsText + parsedKVInput->keyData_t[ESP32_SPI_SET_SLAVE_ADRESS].off
		,parsedKVInput->keyData_t[ESP32_SPI_SET_SLAVE_ADRESS].len
		,&number)) {

		if (number < 100) { //?????????????????????????????

			// valid, take it
			new_slave_addr = number;

			// set affected reading
			affectedReadings |= 
				parsedKVInput->keyData_t[ESP32_SPI_SET_SLAVE_ADRESS].affectedReadings;

//			// Push processed CMD to to Response ...
//			RespArgsWPos += sprintf( RespArgsWPos,"cmd=SLAVE_ADRESS");
		}
	}

	// invalid input detected ?
	else return true;
  }










*/




















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
  // val=[0-9] -> Setze Wert f\FCr Kanal (uint8) (0-x)
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
  // dim=[0-9] -> Setze dimmer f\FCr Kanal (uint8) (0-x)
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
  // caps=[ ] -> Fragt nach den F\E4higkeiten-Bits (0-x)
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
  // -> Setze Allzeit-Gesamt-Impulse passend zum aktuellen Z\E4hlerstand
  // -> Set alltime-total-tics meeting the currend meter-reading
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SPI_SET_ATT) ) {
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
  // -> Setze Einheit f\FCr den Durchschnitts-Verbrauchs-Wert (char[3])
  // -> Set unit for average-consumption-value (char[3])
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SPI_SET_AVU) ) {
	// valid input happened ?
	if (SCDEH_GetSpecialStrVal((uint8_t*)Args+QueryResultKF->KeyField_Data_t[S0_QF_avu].off
		,QueryResultKF->KeyField_Data_t[S0_QF_avu].len
		,(char*) &newAverageUnit
		,sizeof (newAverageUnit) // -> case 3 = (Alpha+Num+"\B3") for Unit Name
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
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SPI_SET_AVF) ) {
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
  // -> Setze Einheit f\FCr den Allzeit-Gesamt-Wert (char[3])
  // -> Set unit for alltime-total-value (char[3])
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SPI_SET_ATU) ) {
	// valid input happened ?
	if (SCDEH_GetSpecialStrVal((uint8_t*)Args+QueryResultKF->KeyField_Data_t[S0_QF_atu].off
		,QueryResultKF->KeyField_Data_t[S0_QF_atu].len
		,(char*) &SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].IB01_AlltimeTotalUnit
		,sizeof (SysCfgRamFlashMirrored->MyS0_FeatCfgRamFlashMirrored[ADID].IB01_AlltimeTotalUnit)
		,3) ) // -> case 3 = (Alpha+Num+"\B3") for Unit Name
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
  // -> Setze Umrechnungsfaktor f\FCr den Allzeit-Total-Wert (float)
  // -> Set factor for alltime total value (float)
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SPI_SET_ATF) ) {
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
//> ------------------------------------------------------------------------------------------------
  // CH1_LCTRL_MODE=no_modification|invert|inhibit_modification
  // -> ?? Setzt den Hardware-Counter des ESP32 (0-7) der verwendet werden soll
  // -> ?? Sets the hardware counter of the ESP32 (0-7) that should be used
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SPI_SET_CH1_LCTRL_MODE) ) {
	uint8_t newMode;
	// valid input happened ?
	if (SCDEH_GetQueryKeyID((uint8_t*) argsText + parsedKVInput->keyData_t[ESP32_SPI_SET_CH1_LCTRL_MODE].off
		,parsedKVInput->keyData_t[ESP32_SPI_SET_CH1_LCTRL_MODE].len, &newMode, &ESP32_SPI_CTRL_MODE)
		&& (newMode <= 3) ) {
		// store the value to temp
		ESP32_SPI_CNT_CONF_REG.CNT_CH1_LCTRL_MODE = newMode;
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
  // caps=[ ] -> Fragt nach den F\E4higkeiten-Bits (0-x)
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
//> ------------------------------------------------------------------------------------------------
  // CH1_LCTRL_MODE=no_modification|invert|inhibit_modification
  // -> ?? Setzt den Hardware-Counter des ESP32 (0-7) der verwendet werden soll
  // -> ?? Sets the hardware counter of the ESP32 (0-7) that should be used
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SPI_SET_CH1_LCTRL_MODE) ) {
	uint8_t newMode;
	// valid input happened ?
	if (SCDEH_GetQueryKeyID((uint8_t*) argsText + parsedKVInput->keyData_t[ESP32_SPI_SET_CH1_LCTRL_MODE].off
		,parsedKVInput->keyData_t[ESP32_SPI_SET_CH1_LCTRL_MODE].len, &newMode, &ESP32_SPI_CTRL_MODE)
		&& (newMode <= 3) ) {
		// store the value to temp
		ESP32_SPI_CNT_CONF_REG.CNT_CH1_LCTRL_MODE = newMode;
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
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SPI_SET_CH1_HCTRL_MODE) ) {
	uint8_t NewMode;
	// valid input happened ?
	if (SCDEH_GetQueryKeyID((uint8_t*) argsText + parsedKVInput->keyData_t[ESP32_SPI_SET_CH1_HCTRL_MODE].off
		,parsedKVInput->keyData_t[ESP32_SPI_SET_CH1_HCTRL_MODE].len, &NewMode, ESP32_SPI_CTRL_MODE) ) {
		// store ne value
		ESP32_SPI_CNT_CONF_REG.CNT_CH1_LCTRL_MODE = NewMode;
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
  if (parsedKVInput->keysFoundBF & ( (uint64_t) 1 << ESP32_SPI_SET_CH1_POS_MODE) ) {
	uint8_t NewMode;
	// valid input happened ?
	if (SCDEH_GetQueryKeyID((uint8_t*) argsText + parsedKVInput->keyData_t[ESP32_SPI_SET_CH1_POS_MODE].off
		,parsedKVInput->keyData_t[ESP32_SPI_SET_CH1_POS_MODE].len, &NewMode, ESP32_SPI_CH_MODE) ) {
		// store ne value
		ESP32_SPI_CNT_CONF_REG.CNT_CH1_POS_MODE = NewMode;
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
// 3. Step: check if all required Key=Value components for operation are included
//          -> if not, return with msg

  if ( (parsedKVInput->keysFoundBF & parsedKVInput->requiredKVBF) != parsedKVInput->requiredKVBF) {

	// alloc mem for retMsg
	retMsg = malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Parsing Error! NOT all required Key=Value args found!");

	return retMsg;
  }



// ------------------------------------------------------------------------------------------------
// 4. Step: check if forbidden Key=Value components are included
//          -> if not, return with msg

  if (parsedKVInput->keysFoundBF & parsedKVInput->forbiddenKVBF) {

	// alloc mem for retMsg
	retMsg = malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Parsing Error! FORBIDDEN Key=Value args found!");

	return retMsg;
  }













// ------------------------------------------------------------------------------------------------
// 5. Step: Changes will happen, deinit old ...


/*
esp_err_t i2c_driver_delete(i2c_port_t i2c_num)
{

    I2C_CHECK(i2c_num < I2C_NUM_MAX, I2C_NUM_ERROR_STR, ESP_ERR_INVALID_ARG);
    I2C_CHECK(p_i2c_obj[i2c_num] != NULL, I2C_DRIVER_ERR_STR, ESP_FAIL);
    i2c_obj_t* p_i2c = p_i2c_obj[i2c_num];

    uint32_t intr_mask = I2C_MASTER_TRAN_COMP_INT_ENA_M |
                         I2C_TIME_OUT_INT_ENA_M |
                         I2C_TRANS_COMPLETE_INT_ENA_M |
                         I2C_TRANS_START_INT_ENA_M |
                         I2C_TX_SEND_EMPTY_INT_ENA_M |
                         I2C_ARBITRATION_LOST_INT_ENA_M |
                         I2C_ACK_ERR_INT_ENA_M |
                         I2C_RXFIFO_OVF_INT_ENA_M |
                         I2C_RX_REC_FULL_INT_ENA_M |
                         I2C_SLAVE_TRAN_COMP_INT_ENA_M;

    CLEAR_PERI_REG_MASK(I2C_INT_ENA_REG(i2c_num), intr_mask);

    esp_intr_free(p_i2c->intr_handle);

    p_i2c->intr_handle = NULL;

    if (p_i2c->cmd_mux) {
        xSemaphoreTake(p_i2c->cmd_mux, portMAX_DELAY);
        vSemaphoreDelete(p_i2c->cmd_mux);
    }
    if (p_i2c->cmd_sem) {
        vSemaphoreDelete(p_i2c->cmd_sem);
    }
    if (p_i2c->slv_rx_mux) {
        vSemaphoreDelete(p_i2c->slv_rx_mux);
    }
    if (p_i2c->slv_tx_mux) {
        vSemaphoreDelete(p_i2c->slv_tx_mux);
    }
    if (p_i2c->rx_ring_buf) {
        vRingbufferDelete(p_i2c->rx_ring_buf);
        p_i2c->rx_ring_buf = NULL;
        p_i2c->rx_buf_length = 0;
    }
    if (p_i2c->tx_ring_buf) {
        vRingbufferDelete(p_i2c->tx_ring_buf);
        p_i2c->tx_ring_buf = NULL;
        p_i2c->tx_buf_length = 0;
    }

    free(p_i2c_obj[i2c_num]);
    p_i2c_obj[i2c_num] = NULL;

    return ESP_OK;
}
*/



// ------------------------------------------------------------------------------------------------
// 6. Step: Make the changes effective ...
/*
  // block #1 store/use I2C Block (0 / 1)
  if (parsedKVInput->keysFoundBF | (1 << ESP32_SPI_SET_I2C_NUM) ) {

	#if ESP32_SPI_Module_DBG >= 5
	printf("|W B#01 I2C_NUM=%d>"
		,new_i2c_num);	
  	#endif

	// enable i2c 0
	if (new_i2c_num == I2C_NUM_0) {

		periph_module_enable(PERIPH_I2C0_MODULE);
	}

	// enable i2c 1
	else if (new_i2c_num == I2C_NUM_1) {

		periph_module_enable(PERIPH_I2C1_MODULE);
	}

	// store assigned I2C Block
	ESP32_SPI_Definition->i2c_num = 
		new_i2c_num;
  }

// ------------------------------------------------------------------------------------------------

  // block #2 store/use I2C Mode (Master/Slave)
  if (parsedKVInput->keysFoundBF |   	( (1 << ESP32_SPI_SET_I2C_MODE) ) ) {

	#if ESP32_SPI_Module_DBG >= 5
	printf("|W B#01 I2C_MODE=%d>"
		,new_i2c_mode);	
  	#endif

	// ??
	I2C_ENTER_CRITICAL(&i2c_spinlock[new_i2c_num]);

	// set rx data msb first
	I2C[new_i2c_num]->ctr.rx_lsb_first = I2C_DATA_MODE_MSB_FIRST;

	// set tx data msb first
	I2C[new_i2c_num]->ctr.tx_lsb_first = I2C_DATA_MODE_MSB_FIRST;

	// mode for master or slave
	I2C[new_i2c_num]->ctr.ms_mode = new_i2c_mode;

	// set open-drain output mode
	I2C[new_i2c_num]->ctr.sda_force_out = 1;

	// set open-drain output mode
	I2C[new_i2c_num]->ctr.scl_force_out = 1;

	// sample at high level of clock
	I2C[new_i2c_num]->ctr.sample_scl_level = 0;

	// slave mode ?
	if (new_i2c_mode == I2C_MODE_SLAVE) {

		I2C[new_i2c_num]->fifo_conf.nonfifo_en = 0;

		I2C[new_i2c_num]->fifo_conf.fifo_addr_cfg_en = 0;

		I2C[new_i2c_num]->fifo_conf.rx_fifo_full_thrhd =
			I2C_FIFO_FULL_THRESH_VAL;

		I2C[new_i2c_num]->fifo_conf.tx_fifo_empty_thrhd =
			I2C_FIFO_EMPTY_THRESH_VAL;

		I2C[new_i2c_num]->int_ena.rx_fifo_full = 1;

		I2C[new_i2c_num]->ctr.trans_start = 0;
	}

	// master mode ?
	else {

		I2C[new_i2c_num]->fifo_conf.nonfifo_en = 0;
	}

	// ??
	I2C_EXIT_CRITICAL(&i2c_spinlock[new_i2c_num]);

	// store assigned I2C Mode
	ESP32_SPI_Definition->i2c_config.mode
		= new_i2c_mode;
  }

// ------------------------------------------------------------------------------------------------

  // block #3 store/use I2C Pin connections
  if (parsedKVInput->keysFoundBF |   	( (1 << ESP32_SPI_SET_SDA_IO)
			       		| (1 << ESP32_SPI_SET_SDA_IO_PULLUP)
			       		| (1 << ESP32_SPI_SET_SCL_IO)
			       		| (1 << ESP32_SPI_SET_SCL_IO_PULLUP) ) ) {

	#if ESP32_SPI_Module_DBG >= 5
	printf("|W B#01 I2C Pins SDA_IO=%d, SDA_IO_PULLUP=%d,SCL_IO=%d, SCL_IO_PULLUP=%d,>"
		,new_sda_io_num,new_sda_pullup_en,new_scl_io_num,new_scl_pullup_en);	
  	#endif



*/
/*


  SCDE_CHECK(( i2c_num < I2C_NUM_MAX ), I2C_NUM_ERROR_STR);
  SCDE_CHECK(((GPIO_IS_VALID_OUTPUT_GPIO(sda_io_num))), I2C_SDA_IO_ERR_STR);
  SCDE_CHECK((GPIO_IS_VALID_OUTPUT_GPIO(scl_io_num)) ||
              (GPIO_IS_VALID_GPIO(scl_io_num) && mode == I2C_MODE_SLAVE),
              I2C_SCL_IO_ERR_STR);
  SCDE_CHECK((sda_pullup_en == GPIO_PULLUP_ENABLE && GPIO_IS_VALID_OUTPUT_GPIO(sda_io_num)) ||
               sda_pullup_en == GPIO_PULLUP_DISABLE, I2C_GPIO_PULLUP_ERR_STR);
  SCDE_CHECK((scl_pullup_en == GPIO_PULLUP_ENABLE && GPIO_IS_VALID_OUTPUT_GPIO(scl_io_num)) ||
               scl_pullup_en == GPIO_PULLUP_DISABLE, I2C_GPIO_PULLUP_ERR_STR);
*/
/*
	int sda_in_sig, sda_out_sig, scl_in_sig, scl_out_sig;

	// prepare the i2c gpio settings according to used hardware 
	switch (new_i2c_num) {

		case I2C_NUM_1:
			sda_out_sig = I2CEXT1_SDA_OUT_IDX;
			sda_in_sig = I2CEXT1_SDA_IN_IDX;
			scl_out_sig = I2CEXT1_SCL_OUT_IDX;
			scl_in_sig = I2CEXT1_SCL_IN_IDX;
			break;

		case I2C_NUM_0:
			default:
			sda_out_sig = I2CEXT0_SDA_OUT_IDX;
			sda_in_sig = I2CEXT0_SDA_IN_IDX;
			scl_out_sig = I2CEXT0_SCL_OUT_IDX;
			scl_in_sig = I2CEXT0_SCL_IN_IDX;
			break;
	}

	// sda gpio configuration
	if (new_sda_io_num >= 0) {

		gpio_set_level(new_sda_io_num, I2C_IO_INIT_LEVEL);

		PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[new_sda_io_num], PIN_FUNC_GPIO);

		gpio_set_direction(new_sda_io_num, GPIO_MODE_INPUT_OUTPUT_OD);

		if (new_sda_pullup_en == GPIO_PULLUP_ENABLE) {

			gpio_set_pull_mode(new_sda_io_num, GPIO_PULLUP_ONLY);
		 } 

		else {

			gpio_set_pull_mode(new_sda_io_num, GPIO_FLOATING);
 		}

		gpio_matrix_out(new_sda_io_num, sda_out_sig, 0, 0);

		gpio_matrix_in(new_sda_io_num, sda_in_sig, 0);
	}

	// scl gpio configuration
	if (new_scl_io_num >= 0) {

		gpio_set_level(new_scl_io_num, I2C_IO_INIT_LEVEL);

		PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[new_scl_io_num], PIN_FUNC_GPIO);

		if (new_i2c_mode == I2C_MODE_MASTER) {

			gpio_set_direction(new_scl_io_num, GPIO_MODE_INPUT_OUTPUT_OD);

			gpio_matrix_out(new_scl_io_num, scl_out_sig, 0, 0);
		}

		else {

			gpio_set_direction(new_scl_io_num, GPIO_MODE_INPUT);
		}

		if (new_scl_pullup_en == GPIO_PULLUP_ENABLE) {

			gpio_set_pull_mode(new_scl_io_num, GPIO_PULLUP_ONLY);
		}

		else {

			gpio_set_pull_mode(new_scl_io_num, GPIO_FLOATING);
		}

		gpio_matrix_in(new_scl_io_num, scl_in_sig, 0);
 	}

	// store assigned I2C Pin connections
	ESP32_SPI_Definition->i2c_config.sda_io_num
		= new_sda_io_num;

	ESP32_SPI_Definition->i2c_config.sda_pullup_en
		= new_sda_pullup_en;

	ESP32_SPI_Definition->i2c_config.scl_io_num
		= new_scl_io_num;

	ESP32_SPI_Definition->i2c_config.scl_pullup_en
		= new_scl_pullup_en;
  }

// ------------------------------------------------------------------------------------------------

  // block #4 store/use I2C clock speed (when use as master only!)
  if (parsedKVInput->keysFoundBF |   	( (1 << ESP32_SPI_SET_MASTER_CLOCK) ) ) {

	#if ESP32_SPI_Module_DBG >= 5
	printf("|W B#01 MASTER_CLOCK=%d>"
		,new_clk_speed);	
  	#endif

	// ??
	I2C_ENTER_CRITICAL(&i2c_spinlock[new_i2c_num]);

	// set frequency
	int half_cycle = ( I2C_APB_CLK_FREQ / new_clk_speed ) / 2;
	I2C[new_i2c_num]->scl_low_period.period =
		 half_cycle - 1;
	I2C[new_i2c_num]->scl_high_period.period =
		( I2C_APB_CLK_FREQ / new_clk_speed ) - half_cycle - 1;

	// set timing for start signal
	I2C[new_i2c_num]->scl_start_hold.time = half_cycle;
	I2C[new_i2c_num]->scl_rstart_setup.time = half_cycle;

	// set timing for stop signal
	I2C[new_i2c_num]->scl_stop_hold.time = half_cycle;
	I2C[new_i2c_num]->scl_stop_setup.time = half_cycle;

	// set timing for data
	I2C[new_i2c_num]->sda_hold.time = half_cycle / 2;
	I2C[new_i2c_num]->sda_sample.time = half_cycle / 2;

	// set timeout of receving data
	I2C[new_i2c_num]->timeout.tout = 200000;

	// ???
	I2C_EXIT_CRITICAL(&i2c_spinlock[new_i2c_num]);

	// store assigned I2C Pin connections
	ESP32_SPI_Definition->i2c_config.master.clk_speed =
		new_clk_speed;
  }

// ------------------------------------------------------------------------------------------------

  // block #5 store/use I2C 10bit enable (when use as slave only!)
  if (parsedKVInput->keysFoundBF |   	( (1 << ESP32_SPI_SET_SLAVE_10BIT_ENA) ) ) {
*/
/*	#if ESP32_SPI_Module_DBG >= 5
	printf("|W B#01 PWM_Block=%d>"
		,newBlock);	
  	#endif*/
/*
	// store assigned I2C Pin connections
	ESP32_SPI_Definition->i2c_config.slave.addr_10bit_en =
		new_addr_10bit_en;

	// slave mode ?
	if (new_i2c_mode == I2C_MODE_SLAVE) {

		// ??
		I2C_ENTER_CRITICAL(&i2c_spinlock[new_i2c_num]);

		I2C[new_i2c_num]->slave_addr.en_10bit =
			new_addr_10bit_en;

		// ???
		I2C_EXIT_CRITICAL(&i2c_spinlock[new_i2c_num]);
	}
  }

// ------------------------------------------------------------------------------------------------

  // block #6 store/use I2C slave adress (when use as slave only!)
  if (parsedKVInput->keysFoundBF |   	( (1 << ESP32_SPI_SET_SLAVE_ADRESS) ) ) {
*/
/*	#if ESP32_SPI_Module_DBG >= 5
	printf("|W B#01 PWM_Block=%d>"
		,newBlock);	
  	#endif*/
/*
	// store assigned I2C Pin connections
	ESP32_SPI_Definition->i2c_config.slave.slave_addr =
		new_slave_addr;

	// slave mode ?
	if (new_i2c_mode == I2C_MODE_SLAVE) {

		// ??
		I2C_ENTER_CRITICAL(&i2c_spinlock[new_i2c_num]);

		I2C[new_i2c_num]->slave_addr.addr =
			new_slave_addr;

		// ???
		I2C_EXIT_CRITICAL(&i2c_spinlock[new_i2c_num]);
	}
  }

// ------------------------------------------------------------------------------------------------






*/
/*
  // block #1 store assigned PWM-Block (low speed / high speed)
  if (parsedKVInput->keysFoundBF | ( (1 << ESP32_SPI_SET_BLOCK) ) ) {
	#if ESP32_SPI_Module_DBG >= 5
	printf("|W B#01 PWM_Block=%d>"
		,newBlock);	
  	#endif
	// store assigned PWM-Block
	ESP32_SPI_Definition->Block = newBlock;
  }
  // block #2 store assigned PWM Channel
  if (parsedKVInput->keysFoundBF | ( (1 << ESP32_SPI_SET_CHANNEL) ) ) {
	#if ESP32_SPI_Module_DBG >= 5
	printf("|W B#02 PWM_Channel=%d>"
		,newChannel);	
  	#endif
	// store assigned PWM Channel
	ESP32_SPI_Definition->Channel = newChannel;
  }
  // block #3 store assigned PWM Timer
  if (parsedKVInput->keysFoundBF | ( (1 << ESP32_SPI_SET_TIMER) ) ) {
	#if ESP32_SPI_Module_DBG >= 5
	printf("|W B#03 PWM_Timer=%d>"
		,newTimer);	
  	#endif
	// store assigned PWM Timer
	ESP32_SPI_Definition->Timer = newTimer;
  }
  // block #4 store assigned / connected GPIO
  if (parsedKVInput->keysFoundBF | ( (1 << ESP32_SPI_SET_GPIO) ) ) {
	#if ESP32_SPI_Module_DBG >= 5
	printf("|W B#04 PWM_GPIO=%d>"
		,newGPIO);	
  	#endif
	// store assigned GPIO
	ESP32_SPI_Definition->GPIO = newGPIO;
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
  if (parsedKVInput->keysFoundBF & ( (1 << ESP32_SPI_SET_TIMER)
				   | (1 << ESP32_SPI_SET_SIG_OUT_EN)
				   | (1 << ESP32_SPI_SET_IDLE_LV)
				   | (1 << ESP32_SPI_SET_HPOINT)
				   | (1 << ESP32_SPI_SET_DUTY) ) ) {
	// store ESP32 PWM configuration registers
	memcpy(pPWM_Channel, &newPWM_Channel, sizeof(ESP32_PWM_Channel_t));
	# if ESP32_SPI_Module_DBG >= 5
	printf("|ESP32_PWM_Channel_s addr=%p>"
		,pPWM_Channel);
	SCDEFn_at_ESP32_SPI_M->HexDumpOutFn ("dump"
		,pPWM_Channel
		, sizeof(ESP32_PWM_Channel_t) );
	# endif
  }
  // block #6 store ESP32 pwm-timer configuration registers
  if (parsedKVInput->keysFoundBF & ( (1 << ESP32_SPI_SET_TIMER)
				   | (1 << ESP32_SPI_SET_RESOLUTION )
				   | (1 << ESP32_SPI_SET_TICK_SOURCE)
			   	   | (1 << ESP32_SPI_SET_FREQ_HZ) ) ) {
	// reset? It seems that a reset is needed to start timer
	newPWM_Timer.RESET = 0;
	// low speed timer? -> needs 'LOW_SPEED_UPDATE' bit set to take settings
	if (!newBlock) newPWM_Timer.LOW_SPEED_UPDATE = 1;
	// store ESP32 timer configuration registers
	memcpy(pPWM_Timer, &newPWM_Timer, sizeof(ESP32_PWM_Timer_t));
	# if ESP32_SPI_Module_DBG >= 5
	printf("|ESP32_PWM_Timer_s addr=%p>"
		,pPWM_Timer);
	SCDEFn_at_ESP32_SPI_M->HexDumpOutFn ("dump"
		,pPWM_Timer
		, sizeof(ESP32_PWM_Timer_t) );
	# endif
  }
*/


// ------------------------------------------------------------------------------------------------
// 7. Step:Set affected readings




  return retMsg;

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
 *  FName: ESP32_SPI_SetAffectedReadings
 *  Desc: Do the readings update for this definition according to 'affectedReadings'
 *  Info: 
 *  Para: ESP32_SPI_Definition_t *ESP32_SPI_Definition -> Definition that should get the input
 *        uint64_t affectedReadings -> according to ...
 *  Rets: Bool -> TRUE = input parsed successful, FALSE = input parsed, but NOT successful, CANCELED
 * ------------------------------------------------------------------------------------------------
 */
bool ICACHE_FLASH_ATTR
ESP32_SPI_SetAffectedReadings(ESP32_SPI_Definition_t* ESP32_SPI_Definition
	,uint64_t affectedReadings)
{
/*
 affectedReadings = 0xfffffffffffffff;
  // do we have updates?
  if (affectedReadings) {
  SCDEFn_at_ESP32_SPI_M->readingsBeginUpdateFn((Common_Definition_t*) ESP32_SPI_Definition);
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
	(ESP32_PWM_Channel_t*) (0x3FF59000 + (0x14 * ESP32_SPI_Definition->Channel));
  // access low speed channels (instead of high speed channels)?
  if (ESP32_SPI_Definition->Block) pPWM_Channel =
	(ESP32_PWM_Channel_t*) ( (uint8_t*) pPWM_Channel + 0xa0);
  // pointer to make the hardware timers accessible
  ESP32_PWM_Timer_t *pPWM_Timer =
	(ESP32_PWM_Timer_t*) (0x3FF59140 + (0x08 * ESP32_SPI_Definition->Timer));
  // access low speed timers (instead of high speed timers)?
  if (ESP32_SPI_Definition->Block) pPWM_Timer =
	(ESP32_PWM_Timer_t*) ( (uint8_t*) pPWM_Timer + 0x20);
//>Add Readings Bit 00 -----------------------------------------------------------------------------
  // add reading 'Duty'
  if (affectedReadings & ESP32_SPI_R_DUTY) {
	readingNameTextLen = asprintf((char **) &readingNameText
		,"Duty");
	readingValueTextLen = asprintf((char **) &readingValueText
		,"%u"
		,pPWM_Channel->DUTY);
	SCDEFn_at_ESP32_SPI_M->readingsBulkUpdateFn((Common_Definition_t*) ESP32_SPI_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);
  }
//>Add Readings Bit 01 -----------------------------------------------------------------------------
  // add reading 'Resolution'
  if (affectedReadings & ESP32_SPI_R_RESOLUTION) {
	readingNameTextLen = asprintf((char **) &readingNameText
		,"Resolution");
	readingValueTextLen = asprintf((char **) &readingValueText
		,"%u"
		,(1 << pPWM_Timer->RESOLUTION) );
	SCDEFn_at_ESP32_SPI_M->readingsBulkUpdateFn((Common_Definition_t*) ESP32_SPI_Definition
		,readingNameText
		,readingNameTextLen
		,readingValueText
		,readingValueTextLen);
  }
*/
//>Add Readings Bit 02 -----------------------------------------------------------------------------
/*
  if (affectedReadings & ESP32_SPI_R_AV01_AV05_AV15) {
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
  if (affectedReadings & ESP32_SPI_R_AVF) {
	readingNameTextLen = asprintf((char **) &readingNameText
		,"Average_Factor");
	readingValueTextLen = asprintf((char **) &readingValueText
		,"%d.%04d"
		,(int) ESP32_SPI_Definition->IB01_AverageFactor	
		,(int) ( (ESP32_SPI_Definition->IB01_AverageFactor -
		(int) ESP32_SPI_Definition->IB01_AverageFactor) * 10000) );
	SCDEFn_at_ESP32_SPI_M->readingsBulkUpdateFn((Common_Definition_t*) ESP32_SPI_Definition
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
  if (affectedReadings & ESP32_SPI_R_AVU) {
	readingNameTextLen = asprintf((char **) &readingNameText
		,"Average_Unit");
	readingValueTextLen = asprintf((char **) &readingValueText
		,"%s"
		,ESP32_SPI_Definition->IB01_AverageUnit);
	SCDEFn_at_ESP32_SPI_M->readingsBulkUpdateFn((Common_Definition_t*) ESP32_SPI_Definition
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
  if (affectedReadings & ESP32_SPI_R_ATF) {
	readingNameTextLen = asprintf((char **) &readingNameText
		,"Alltime_Total_Factor");
	readingValueTextLen = asprintf((char **) &readingValueText
		,"%d.%04d"
		,(int) (ESP32_SPI_Definition->IB01_AlltimeTotalFactor)	
		,(int) ( (ESP32_SPI_Definition->IB01_AlltimeTotalFactor -
		(int) ESP32_SPI_Definition->IB01_AlltimeTotalFactor) * 10000) );
	SCDEFn_at_ESP32_SPI_M->readingsBulkUpdateFn((Common_Definition_t*) ESP32_SPI_Definition
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
  if (affectedReadings & ESP32_SPI_R_ATU) {
	readingNameTextLen = asprintf((char **) &readingNameText
		,"Alltime_Total_Unit");
	readingValueTextLen = asprintf((char **) &readingValueText
		,"%s"
		,ESP32_SPI_Definition->IB01_AlltimeTotalUnit);
	SCDEFn_at_ESP32_SPI_M->readingsBulkUpdateFn((Common_Definition_t*) ESP32_SPI_Definition
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
  if (affectedReadings & ESP32_SPI_R_DEB) {
	readingNameTextLen = asprintf((char **) &readingNameText
		,"Debounce");
	readingValueTextLen = asprintf((char **) &readingValueText
		,"%u"
		,ESP32_SPI_Definition->IB01_Debounce);
	SCDEFn_at_ESP32_SPI_M->readingsBulkUpdateFn((Common_Definition_t*) ESP32_SPI_Definition
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
  if (affectedReadings & ESP32_SPI_R_name)
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
//  SCDEFn_at_ESP32_SPI_M->readingsEndUpdateFn((Common_Definition_t*) ESP32_SPI_Definition);


  return true;
}



































/// spi common





// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <string.h>
//#include "driver/spi_master.h"
#include "soc/dport_reg.h"
#include "soc/spi_periph.h"
#include "rom/ets_sys.h"
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_intr.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_err.h"
#include "soc/soc.h"
#include "soc/dport_reg.h"
#include "rom/lldesc.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include "esp_heap_caps.h"
//#include "driver/spi_common.h"
#include "stdatomic.h"











static const char *SPI_TAG = "spi";


#define SPI_CHECK(a, str, ret_val) do { \
    if (!(a)) { \
        ESP_LOGE(SPI_TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val); \
    } \
    } while(0)



#define SPI_CHECK2(a, str, ret_val, ...) \
    if (!(a)) { \
        ESP_LOGE(SPI_TAG,"%s(%d): "str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        return (ret_val); \
    }





#define SPI_CHECK_PIN(pin_num, pin_name, check_output) if (check_output) { \
            SPI_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(pin_num), pin_name" not valid", ESP_ERR_INVALID_ARG); \
        } else { \
            SPI_CHECK(GPIO_IS_VALID_GPIO(pin_num), pin_name" not valid", ESP_ERR_INVALID_ARG); \
        }


typedef struct spi_device_t spi_device_t;

#define FUNC_SPI    1   //all pins of HSPI and VSPI shares this function number
#define FUNC_GPIO   PIN_FUNC_GPIO


#define DMA_CHANNEL_ENABLED(dma_chan)    (BIT(dma_chan-1))

//Periph 1 is 'claimed' by SPI flash code.
static atomic_bool ESP32_SPI_periph_claimed[3] = { ATOMIC_VAR_INIT(true), ATOMIC_VAR_INIT(false), ATOMIC_VAR_INIT(false)};

static const char* ESP32_SPI_claiming_func[3] = {NULL, NULL, NULL};

static uint8_t spi_dma_chan_enabled = 0;

static portMUX_TYPE spi_dma_spinlock = portMUX_INITIALIZER_UNLOCKED;











// Returns true if this peripheral is successfully claimed, false if otherwise.
bool 
ESP32_SPI_common_periph_claim(ESP32_SPI_host_device_t host,
	const char* source)
{
    bool false_var = false;
    bool ret = atomic_compare_exchange_strong(&ESP32_SPI_periph_claimed[host], &false_var, true);
    if (ret) {
        ESP32_SPI_claiming_func[host] = source;
        periph_module_enable(spi_periph_signal[host].module);
    } else {
        ESP_EARLY_LOGE(SPI_TAG, "SPI%d already claimed by %s.", host+1, ESP32_SPI_claiming_func[host]);
    }
    return ret;
}



bool 
ESP32_SPI_common_periph_in_use(ESP32_SPI_host_device_t host)
{
    return atomic_load(&ESP32_SPI_periph_claimed[host]);
}



//Returns true if this peripheral is successfully freed, false if otherwise.
bool 
ESP32_SPI_common_periph_free(ESP32_SPI_host_device_t host)
{
    bool true_var = true;
    bool ret = atomic_compare_exchange_strong(&ESP32_SPI_periph_claimed[host], &true_var, false);
    if (ret) periph_module_disable(spi_periph_signal[host].module);
    return ret;
}



int 
ESP32_SPI_common_irqsource_for_host(ESP32_SPI_host_device_t host)
{
    return spi_periph_signal[host].irq;
}



spi_dev_t* 
ESP32_SPI_common_hw_for_host(ESP32_SPI_host_device_t host)
{
    return spi_periph_signal[host].hw;
}



bool 
ESP32_SPI_common_dma_chan_claim (int dma_chan)
{
    bool ret = false;
    assert( dma_chan == 1 || dma_chan == 2 );

    portENTER_CRITICAL(&spi_dma_spinlock);
    if ( !(spi_dma_chan_enabled & DMA_CHANNEL_ENABLED(dma_chan)) ) {
        // get the channel only when it's not claimed yet.
        spi_dma_chan_enabled |= DMA_CHANNEL_ENABLED(dma_chan);
        ret = true;
    }
    periph_module_enable( PERIPH_SPI_DMA_MODULE );
    portEXIT_CRITICAL(&spi_dma_spinlock);

    return ret;
}



bool 
ESP32_SPI_common_dma_chan_in_use(int dma_chan)
{
    assert(dma_chan==1 || dma_chan == 2);
    return spi_dma_chan_enabled & DMA_CHANNEL_ENABLED(dma_chan);
}



bool
ESP32_SPI_common_dma_chan_free(int dma_chan)
{
    assert( dma_chan == 1 || dma_chan == 2 );
    assert( spi_dma_chan_enabled & DMA_CHANNEL_ENABLED(dma_chan) );

    portENTER_CRITICAL(&spi_dma_spinlock);
    spi_dma_chan_enabled &= ~DMA_CHANNEL_ENABLED(dma_chan);
    if ( spi_dma_chan_enabled == 0 ) {
        //disable the DMA only when all the channels are freed.
        periph_module_disable( PERIPH_SPI_DMA_MODULE );
    }
    portEXIT_CRITICAL(&spi_dma_spinlock);

    return true;
}



static bool 
bus_uses_iomux_pins(ESP32_SPI_host_device_t host,
	const ESP32_SPI_bus_config_t* bus_config)
{
    if (bus_config->sclk_io_num>=0 &&
        bus_config->sclk_io_num != spi_periph_signal[host].spiclk_iomux_pin) return false;
    if (bus_config->quadwp_io_num>=0 &&
        bus_config->quadwp_io_num != spi_periph_signal[host].spiwp_iomux_pin) return false;
    if (bus_config->quadhd_io_num>=0 &&
        bus_config->quadhd_io_num != spi_periph_signal[host].spihd_iomux_pin) return false;
    if (bus_config->mosi_io_num >= 0 &&
        bus_config->mosi_io_num != spi_periph_signal[host].spid_iomux_pin) return false;
    if (bus_config->miso_io_num>=0 &&
        bus_config->miso_io_num != spi_periph_signal[host].spiq_iomux_pin) return false;

return true;
}





/** to do: host not in use marker behaveior!
 * -------------------------------------------------------------------------------------------------
 *  FName: Bus Initialize IO
 *  Desc: Is called to do the common stuff to hook up a SPI host to a bus defined by a bunch of GPIO
 *        pins. Feed it a host number and a bus config struct and it'll set up the GPIO matrix and
 *        enable the device. If a pin is set to non-negative value, it should be able to be initialized.
 *  Info: 
 *  Para: ESP32_SPI_Definition_t* ESP32_SPI_Definition -> the 'Definition' that should init its SPI IO  
 *        ESP32_SPI_host_device_t host_device -> the SPI pheriperal that this Definitions spi-host should use
 *        const ESP32_SPI_bus_config_t *bus_config -> the configuration to set up
 *        int dma_chan ->
 *        uint32_t flags ->
 *        uint32_t* flags_o ->
 *  Rets: strTextMultiple_t* -> response: ptr to an singly linked tail queue element with return
 *                              message (error!) in allocated memory, or NULL = SCDE_OK / NO error message
 * -------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* 
ESP32_SPI_common_bus_initialize_io(ESP32_SPI_Definition_t* ESP32_SPI_Definition,
	ESP32_SPI_host_device_t host_device,
	const ESP32_SPI_bus_config_t *bus_config,
	int dma_chan,
	uint32_t flags,
	uint32_t* flags_o)
{
  // for Fn response msg
  strTextMultiple_t *retMsg = SCDE_OK;

  uint32_t temp_flag = 0;

  bool miso_need_output;
  bool mosi_need_output;
  bool sclk_need_output;

  if ( ( flags & SPICOMMON_BUSFLAG_MASTER ) != 0 ) {

	// initial for master
        miso_need_output = ((flags&SPICOMMON_BUSFLAG_DUAL) != 0) ? true : false;
        mosi_need_output = true;
        sclk_need_output = true;

  } else {

	// initial for slave
	miso_need_output = true;
        mosi_need_output = ( ( flags & SPICOMMON_BUSFLAG_DUAL ) != 0 ) ? true : false;
        sclk_need_output = false;
  }

  const bool wp_need_output = true;
  const bool hd_need_output = true;

  // check pin capabilities
  if ( bus_config->sclk_io_num >= 0 ) {

	temp_flag |= SPICOMMON_BUSFLAG_SCLK;
	SCDE_CHECK_LOADERRORTXT_RET(GPIO_IS_VALID_OUTPUT_GPIO(bus_config->sclk_io_num),
		"SCLK is no valid output GPIO");
  }

  if ( bus_config->quadwp_io_num >= 0 ) {

	SCDE_CHECK_LOADERRORTXT_RET(GPIO_IS_VALID_OUTPUT_GPIO(bus_config->quadwp_io_num),
		"WP is no valid output GPIO");
  }

    if ( bus_config->quadhd_io_num >= 0 ) {

	SCDE_CHECK_LOADERRORTXT_RET(GPIO_IS_VALID_OUTPUT_GPIO(bus_config->quadhd_io_num),
		"HD is no valid output GPIO");
  }

  // set flags for QUAD mode according to the existence of wp and hd
  if ( bus_config->quadhd_io_num >= 0 && bus_config->quadwp_io_num >= 0 ) temp_flag |= SPICOMMON_BUSFLAG_WPHD;

  if ( bus_config->mosi_io_num >= 0 ) {

	temp_flag |= SPICOMMON_BUSFLAG_MOSI;

	SCDE_CHECK_LOADERRORTXT_RET(GPIO_IS_VALID_OUTPUT_GPIO(bus_config->quadhd_io_num),
		"MOSI is no valid output GPIO");
    }

  if ( bus_config->miso_io_num >= 0 ) {

	temp_flag |= SPICOMMON_BUSFLAG_MISO;

	SCDE_CHECK_LOADERRORTXT_RET(GPIO_IS_VALID_OUTPUT_GPIO(bus_config->miso_io_num),
		"MISO is no valid output GPIO");
  }

  // set flags for DUAL mode according to output-capability of MOSI and MISO pins.
  if ( (bus_config->mosi_io_num < 0 || GPIO_IS_VALID_OUTPUT_GPIO(bus_config->mosi_io_num)) &&
	(bus_config->miso_io_num < 0 || GPIO_IS_VALID_OUTPUT_GPIO(bus_config->miso_io_num)) ) {

	temp_flag |= SPICOMMON_BUSFLAG_DUAL;
  }

  // check if the selected pins correspond to the iomux pins of the peripheral
  bool use_iomux = bus_uses_iomux_pins(host_device, bus_config);

  if ( use_iomux ) temp_flag |= SPICOMMON_BUSFLAG_NATIVE_PINS;

  uint32_t missing_flag = flags & ~temp_flag;

  missing_flag &= ~SPICOMMON_BUSFLAG_MASTER;	//don't check this flag

  if ( missing_flag != 0 ) {

	//check pins existence
	if (missing_flag & SPICOMMON_BUSFLAG_SCLK) {

		#if ESP32_DeVICE_Module_DBG >= 1
		SCDEFn_at_ESP32_SPI_M->Log3Fn(ESP32_SPI_ProvidedByModule.common.typeName
			,ESP32_SPI_ProvidedByModule.common.typeNameLen
			,1
			,"sclk pin required!");
		#endif
	}

	if (missing_flag & SPICOMMON_BUSFLAG_MOSI) {

		#if ESP32_DeVICE_Module_DBG >= 1
		SCDEFn_at_ESP32_SPI_M->Log3Fn(ESP32_SPI_ProvidedByModule.common.typeName
			,ESP32_SPI_ProvidedByModule.common.typeNameLen
			,1
			,"mosi pin required!");
		#endif
	}

	if (missing_flag & SPICOMMON_BUSFLAG_MISO) {

		#if ESP32_DeVICE_Module_DBG >= 1
		SCDEFn_at_ESP32_SPI_M->Log3Fn(ESP32_SPI_ProvidedByModule.common.typeName
			,ESP32_SPI_ProvidedByModule.common.typeNameLen
			,1
			,"miso pin required!");
		#endif
	}

	if (missing_flag & SPICOMMON_BUSFLAG_DUAL) {

		#if ESP32_DeVICE_Module_DBG >= 1
		SCDEFn_at_ESP32_SPI_M->Log3Fn(ESP32_SPI_ProvidedByModule.common.typeName
			,ESP32_SPI_ProvidedByModule.common.typeNameLen
			,1
			,"not both mosi and miso output capable!");
		#endif
	}

	if (missing_flag & SPICOMMON_BUSFLAG_WPHD) {

		#if ESP32_DeVICE_Module_DBG >= 1
		SCDEFn_at_ESP32_SPI_M->Log3Fn(ESP32_SPI_ProvidedByModule.common.typeName
			,ESP32_SPI_ProvidedByModule.common.typeNameLen
			,1
			,"both wp and hd required!");
		#endif
	}

	if (missing_flag & SPICOMMON_BUSFLAG_NATIVE_PINS) {

		#if ESP32_DeVICE_Module_DBG >= 1
		SCDEFn_at_ESP32_SPI_M->Log3Fn(ESP32_SPI_ProvidedByModule.common.typeName
			,ESP32_SPI_ProvidedByModule.common.typeNameLen
			,1
			,"not using iomux pins!");
		#endif
	}

	SCDE_CHECK_LOADERRORTXT_RET(missing_flag == 0, "Not all required capabilities satisfied. Details in Debug-Log");
  }

  if ( use_iomux ) {

	// All SPI iomux pin selections resolve to 1, so we put that here instead of trying to figure
	// out which FUNC_GPIOx_xSPIxx to grab; they all are defined to 1 anyway.
	#if ESP32_DeVICE_Module_DBG >= 5
	SCDEFn_at_ESP32_SPI_M->Log3Fn(ESP32_SPI_ProvidedByModule.common.typeName
		,ESP32_SPI_ProvidedByModule.common.typeNameLen
		,5
		,"SPI%d uses iomux pins."
		,host_device + 1);
	#endif

	if ( bus_config->mosi_io_num >= 0 ) {
		gpio_iomux_in(bus_config->mosi_io_num, spi_periph_signal[host_device].spid_in);
		gpio_iomux_out(bus_config->mosi_io_num, FUNC_SPI, false);
	}

	if ( bus_config->miso_io_num >= 0 ) {
		gpio_iomux_in(bus_config->miso_io_num, spi_periph_signal[host_device].spiq_in);
		gpio_iomux_out(bus_config->miso_io_num, FUNC_SPI, false);
	}

	if ( bus_config->quadwp_io_num >= 0 ) {
		gpio_iomux_in(bus_config->quadwp_io_num, spi_periph_signal[host_device].spiwp_in);
		gpio_iomux_out(bus_config->quadwp_io_num, FUNC_SPI, false);
	}

	if ( bus_config->quadhd_io_num >= 0 ) {
		gpio_iomux_in(bus_config->quadhd_io_num, spi_periph_signal[host_device].spihd_in);
		gpio_iomux_out(bus_config->quadhd_io_num, FUNC_SPI, false);
	}

	if ( bus_config->sclk_io_num >= 0 ) {
		gpio_iomux_in(bus_config->sclk_io_num, spi_periph_signal[host_device].spiclk_in);
		gpio_iomux_out(bus_config->sclk_io_num, FUNC_SPI, false);
	}

	temp_flag |= SPICOMMON_BUSFLAG_NATIVE_PINS;

  } else {

	// Use GPIO matrix
	#if ESP32_DeVICE_Module_DBG >= 5
	SCDEFn_at_ESP32_SPI_M->Log3Fn(ESP32_SPI_ProvidedByModule.common.typeName
		,ESP32_SPI_ProvidedByModule.common.typeNameLen
		,5
		,"SPI%d uses gpio matrix."
		,host_device + 1);
	#endif

	if ( bus_config->mosi_io_num >= 0 ) {

		if (mosi_need_output || (temp_flag&SPICOMMON_BUSFLAG_DUAL)) {

		gpio_set_direction(bus_config->mosi_io_num, GPIO_MODE_INPUT_OUTPUT);

		gpio_matrix_out(bus_config->mosi_io_num, spi_periph_signal[host_device].spid_out, false, false);

	} else {

		gpio_set_direction(bus_config->mosi_io_num, GPIO_MODE_INPUT);
	}

	gpio_matrix_in(bus_config->mosi_io_num, spi_periph_signal[host_device].spid_in, false);

	PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->mosi_io_num], FUNC_GPIO);
  }

  if ( bus_config->miso_io_num >= 0 ) {

	if (miso_need_output || (temp_flag & SPICOMMON_BUSFLAG_DUAL)) {

		gpio_set_direction(bus_config->miso_io_num, GPIO_MODE_INPUT_OUTPUT);

		gpio_matrix_out(bus_config->miso_io_num, spi_periph_signal[host_device].spiq_out, false, false);

	} else {

		gpio_set_direction(bus_config->miso_io_num, GPIO_MODE_INPUT);
	}

	gpio_matrix_in(bus_config->miso_io_num, spi_periph_signal[host_device].spiq_in, false);

	PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->miso_io_num], FUNC_GPIO);
  }

  if ( bus_config->quadwp_io_num >= 0) {

	gpio_set_direction(bus_config->quadwp_io_num, GPIO_MODE_INPUT_OUTPUT);

	gpio_matrix_out(bus_config->quadwp_io_num, spi_periph_signal[host_device].spiwp_out, false, false);

	gpio_matrix_in(bus_config->quadwp_io_num, spi_periph_signal[host_device].spiwp_in, false);

	PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->quadwp_io_num], FUNC_GPIO);

  }

  if ( bus_config->quadhd_io_num >= 0) {

	gpio_set_direction(bus_config->quadhd_io_num, GPIO_MODE_INPUT_OUTPUT);

	gpio_matrix_out(bus_config->quadhd_io_num, spi_periph_signal[host_device].spihd_out, false, false);

	gpio_matrix_in(bus_config->quadhd_io_num, spi_periph_signal[host_device].spihd_in, false);

	PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->quadhd_io_num], FUNC_GPIO);
  }

  if (bus_config->sclk_io_num >= 0) {

		if (sclk_need_output) {

			gpio_set_direction(bus_config->sclk_io_num, GPIO_MODE_INPUT_OUTPUT);

			gpio_matrix_out(bus_config->sclk_io_num, spi_periph_signal[host_device].spiclk_out, false, false);

		} else {

			gpio_set_direction(bus_config->sclk_io_num, GPIO_MODE_INPUT);
		}

		gpio_matrix_in(bus_config->sclk_io_num, spi_periph_signal[host_device].spiclk_in, false);

		PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[bus_config->sclk_io_num], FUNC_GPIO);
	}
  }

  // Select DMA channel.
  DPORT_SET_PERI_REG_BITS(DPORT_SPI_DMA_CHAN_SEL_REG, 3, dma_chan, (host_device * 2));

  if (flags_o) *flags_o = temp_flag;

  return retMsg;
}



//Find any pin with output muxed to ``func`` and reset it to GPIO
static void 
reset_func_to_gpio(int func)
{
    for (int x = 0; x < GPIO_PIN_COUNT; x++) {
        if (GPIO_IS_VALID_GPIO(x) && (READ_PERI_REG(GPIO_FUNC0_OUT_SEL_CFG_REG + (x * 4))&GPIO_FUNC0_OUT_SEL_M) == func)  {
            gpio_matrix_out(x, SIG_GPIO_OUT_IDX, false, false);
        }
    }
}



esp_err_t 
ESP32_SPI_common_bus_free_io(ESP32_SPI_host_device_t host)
{
    if (REG_GET_FIELD(GPIO_PIN_MUX_REG[spi_periph_signal[host].spid_iomux_pin], MCU_SEL) == 1) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[spi_periph_signal[host].spid_iomux_pin], PIN_FUNC_GPIO);
    if (REG_GET_FIELD(GPIO_PIN_MUX_REG[spi_periph_signal[host].spiq_iomux_pin], MCU_SEL) == 1) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[spi_periph_signal[host].spiq_iomux_pin], PIN_FUNC_GPIO);
    if (REG_GET_FIELD(GPIO_PIN_MUX_REG[spi_periph_signal[host].spiclk_iomux_pin], MCU_SEL) == 1) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[spi_periph_signal[host].spiclk_iomux_pin], PIN_FUNC_GPIO);
    if (REG_GET_FIELD(GPIO_PIN_MUX_REG[spi_periph_signal[host].spiwp_iomux_pin], MCU_SEL) == 1) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[spi_periph_signal[host].spiwp_iomux_pin], PIN_FUNC_GPIO);
    if (REG_GET_FIELD(GPIO_PIN_MUX_REG[spi_periph_signal[host].spihd_iomux_pin], MCU_SEL) == 1) PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[spi_periph_signal[host].spihd_iomux_pin], PIN_FUNC_GPIO);
    reset_func_to_gpio(spi_periph_signal[host].spid_out);
    reset_func_to_gpio(spi_periph_signal[host].spiq_out);
    reset_func_to_gpio(spi_periph_signal[host].spiclk_out);
    reset_func_to_gpio(spi_periph_signal[host].spiwp_out);
    reset_func_to_gpio(spi_periph_signal[host].spihd_out);
    return ESP_OK;
}



esp_err_t 
ESP32_SPI_common_bus_free_io_cfg(const ESP32_SPI_bus_config_t *bus_cfg)
{
    int pin_array[] = {
        bus_cfg->mosi_io_num,
        bus_cfg->miso_io_num,
        bus_cfg->sclk_io_num,
        bus_cfg->quadwp_io_num,
        bus_cfg->quadhd_io_num,
    };
    for (int i = 0; i < sizeof(pin_array)/sizeof(int); i ++) {
        const int io = pin_array[i];
        if (io >= 0 && GPIO_IS_VALID_GPIO(io)) gpio_reset_pin(io);
    }
    return ESP_OK;
}




void 
ESP32_SPI_common_cs_initialize(ESP32_SPI_host_device_t host,
	int cs_io_num,
	int cs_num,
	int force_gpio_matrix)
{
    if (!force_gpio_matrix && cs_io_num == spi_periph_signal[host].spics0_iomux_pin && cs_num == 0) {
        //The cs0s for all SPI peripherals map to pin mux source 1, so we use that instead of a define.
        gpio_iomux_in(cs_io_num, spi_periph_signal[host].spics_in);
        gpio_iomux_out(cs_io_num, FUNC_SPI, false);
    } else {
        //Use GPIO matrix
        if (GPIO_IS_VALID_OUTPUT_GPIO(cs_io_num)) {
            gpio_set_direction(cs_io_num, GPIO_MODE_INPUT_OUTPUT);
            gpio_matrix_out(cs_io_num, spi_periph_signal[host].spics_out[cs_num], false, false);
        } else {
            gpio_set_direction(cs_io_num, GPIO_MODE_INPUT);
        }
        if (cs_num == 0) gpio_matrix_in(cs_io_num, spi_periph_signal[host].spics_in, false);
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[cs_io_num], FUNC_GPIO);
    }
}



void 
ESP32_SPI_common_cs_free(ESP32_SPI_host_device_t host,
	int cs_io_num)
{
    if (cs_io_num == 0 && REG_GET_FIELD(GPIO_PIN_MUX_REG[spi_periph_signal[host].spics0_iomux_pin], MCU_SEL) == 1) {
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[spi_periph_signal[host].spics0_iomux_pin], PIN_FUNC_GPIO);
    }
    reset_func_to_gpio(spi_periph_signal[host].spics_out[cs_io_num]);
}



void 
ESP32_SPI_common_cs_free_io(int cs_gpio_num)
{
    assert(cs_gpio_num>=0 && GPIO_IS_VALID_GPIO(cs_gpio_num));
    gpio_reset_pin(cs_gpio_num);
}



// Set up a list of dma descriptors. dmadesc is an array of descriptors. Data is the buffer to point to.
void IRAM_ATTR 
ESP32_SPI_common_setup_dma_desc_links(lldesc_t *dmadesc,
	int len,
	const uint8_t *data,
	bool isrx)
{
    int n = 0;
    while (len) {
        int dmachunklen = len;
        if (dmachunklen > SPI_MAX_DMA_LEN) dmachunklen = SPI_MAX_DMA_LEN;
        if (isrx) {
            //Receive needs DMA length rounded to next 32-bit boundary
            dmadesc[n].size = (dmachunklen + 3) & (~3);
            dmadesc[n].length = (dmachunklen + 3) & (~3);
        } else {
            dmadesc[n].size = dmachunklen;
            dmadesc[n].length = dmachunklen;
        }
        dmadesc[n].buf = (uint8_t *)data;
        dmadesc[n].eof = 0;
        dmadesc[n].sosf = 0;
        dmadesc[n].owner = 1;
        dmadesc[n].qe.stqe_next = &dmadesc[n + 1];
        len -= dmachunklen;
        data += dmachunklen;
        n++;
    }
    dmadesc[n - 1].eof = 1; //Mark last DMA desc as end of stream.
    dmadesc[n - 1].qe.stqe_next = NULL;
}


/*
Code for workaround for DMA issue in ESP32 v0/v1 silicon
*/


static volatile int dmaworkaround_channels_busy[2] = {0, 0};
static dmaworkaround_cb_t dmaworkaround_cb;
static void *dmaworkaround_cb_arg;
static portMUX_TYPE dmaworkaround_mux = portMUX_INITIALIZER_UNLOCKED;
static int dmaworkaround_waiting_for_chan = 0;

bool IRAM_ATTR ESP32_SPI_common_dmaworkaround_req_reset(int dmachan, dmaworkaround_cb_t cb, void *arg)
{
    int otherchan = (dmachan == 1) ? 2 : 1;
    bool ret;
    portENTER_CRITICAL_ISR(&dmaworkaround_mux);
    if (dmaworkaround_channels_busy[otherchan-1]) {
        //Other channel is busy. Call back when it's done.
        dmaworkaround_cb = cb;
        dmaworkaround_cb_arg = arg;
        dmaworkaround_waiting_for_chan = otherchan;
        ret = false;
    } else {
        //Reset DMA
        periph_module_reset( PERIPH_SPI_DMA_MODULE );
        ret = true;
    }
    portEXIT_CRITICAL_ISR(&dmaworkaround_mux);
    return ret;
}

bool IRAM_ATTR ESP32_SPI_common_dmaworkaround_reset_in_progress()
{
    return (dmaworkaround_waiting_for_chan != 0);
}

void IRAM_ATTR ESP32_SPI_common_dmaworkaround_idle(int dmachan)
{
    portENTER_CRITICAL_ISR(&dmaworkaround_mux);
    dmaworkaround_channels_busy[dmachan-1] = 0;
    if (dmaworkaround_waiting_for_chan == dmachan) {
        //Reset DMA
        periph_module_reset( PERIPH_SPI_DMA_MODULE );
        dmaworkaround_waiting_for_chan = 0;
        //Call callback
        dmaworkaround_cb(dmaworkaround_cb_arg);

    }
    portEXIT_CRITICAL_ISR(&dmaworkaround_mux);
}

void IRAM_ATTR ESP32_SPI_common_dmaworkaround_transfer_active(int dmachan)
{
    portENTER_CRITICAL_ISR(&dmaworkaround_mux);
    dmaworkaround_channels_busy[dmachan-1] = 1;
    portEXIT_CRITICAL_ISR(&dmaworkaround_mux);
}

















































































// SPI Master






// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*
Architecture:
We can initialize a SPI driver, but we don't talk to the SPI driver itself, we address a device. A device essentially
is a combination of SPI port and CS pin, plus some information about the specifics of communication to the device
(timing, command/address length etc). The arbitration between tasks is also in conception of devices.
A device can work in interrupt mode and polling mode, and a third but
complicated mode which combines the two modes above:
1. Work in the ISR with a set of queues; one per device.
   The idea is that to send something to a SPI device, you allocate a
   transaction descriptor. It contains some information about the transfer
   like the lenghth, address, command etc, plus pointers to transmit and
   receive buffer. The address of this block gets pushed into the transmit
   queue. The SPI driver does its magic, and sends and retrieves the data
   eventually. The data gets written to the receive buffers, if needed the
   transaction descriptor is modified to indicate returned parameters and
   the entire thing goes into the return queue, where whatever software
   initiated the transaction can retrieve it.
   The entire thing is run from the SPI interrupt handler. If SPI is done
   transmitting/receiving but nothing is in the queue, it will not clear the
   SPI interrupt but just disable it by esp_intr_disable. This way, when a
   new thing is sent, pushing the packet into the send queue and re-enabling
   the interrupt (by esp_intr_enable) will trigger the interrupt again, which
   can then take care of the sending.
2. Work in the polling mode in the task.
   In this mode we get rid of the ISR, FreeRTOS queue and task switching, the
   task is no longer blocked during a transaction. This increase the cpu
   load, but decrease the interval of SPI transactions. Each time only one
   device (in one task) can send polling transactions, transactions to
   other devices are blocked until the polling transaction of current device
   is done.
   In the polling mode, the queue is not used, all the operations are done
   in the task. The task calls ``spi_device_polling_start`` to setup and start
   a new transaction, then call ``spi_device_polling_end`` to handle the
   return value of the transaction.
   To handle the arbitration among devices, the device "temporarily" acquire
   a bus by the ``device_acquire_bus_internal`` function, which writes
   acquire_cs by CAS operation. Other devices which wants to send polling
   transactions but don't own the bus will block and wait until given the
   semaphore which indicates the ownership of bus.
   In case of the ISR is still sending transactions to other devices, the ISR
   should maintain an ``isr_free`` flag indicating that it's not doing
   transactions. When the bus is acquired, the ISR can only send new
   transactions to the acquiring device. The ISR will automatically disable
   itself and send semaphore to the device if the ISR is free. If the device
   sees the isr_free flag, it can directly start its polling transaction.
   Otherwise it should block and wait for the semaphore from the ISR.
   After the polling transaction, the driver will release the bus. During the
   release of the bus, the driver search all other devices to see whether
   there is any device waiting to acquire the bus, if so, acquire for it and
   send it a semaphore if the device queue is empty, or invoke the ISR for
   it. If all other devices don't need to acquire the bus, but there are
   still transactions in the queues, the ISR will also be invoked.
   To get better polling efficiency, user can call ``spi_device_acquire_bus``
   function, which also calls the ``device_acquire_bus_internal`` function,
   before a series of polling transactions to a device. The bus acquiring and
   task switching before and after the polling transaction will be escaped.
3. Mixed mode
   The driver is written under the assumption that polling and interrupt
   transactions are not happening simultaneously. When sending polling
   transactions, it will check whether the ISR is active, which includes the
   case the ISR is sending the interrupt transactions of the acquiring
   device. If the ISR is still working, the routine sending a polling
   transaction will get blocked and wait until the semaphore from the ISR
   which indicates the ISR is free now.
   A fatal case is, a polling transaction is in flight, but the ISR received
   an interrupt transaction. The behavior of the driver is unpredictable,
   which should be strictly forbidden.
We have two bits to control the interrupt:
1. The slave->trans_done bit, which is automatically asserted when a transaction is done.
   This bit is cleared during an interrupt transaction, so that the interrupt
   will be triggered when the transaction is done, or the SW can check the
   bit to see if the transaction is done for polling transactions.
   When no transaction is in-flight, the bit is kept active, so that the SW
   can easily invoke the ISR by enable the interrupt.
2. The system interrupt enable/disable, controlled by esp_intr_enable and esp_intr_disable.
   The interrupt is disabled (by the ISR itself) when no interrupt transaction
   is queued. When the bus is not occupied, any task, which queues a
   transaction into the queue, will enable the interrupt to invoke the ISR.
   When the bus is occupied by a device, other device will put off the
   invoking of ISR to the moment when the bus is released. The device
   acquiring the bus can still send interrupt transactions by enable the
   interrupt.
*/

#include <string.h>
//#include "driver/spi_common.h"
//#include "driver/spi_master.h"
#include "soc/dport_reg.h"
#include "soc/spi_periph.h"
#include "rom/ets_sys.h"
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_intr.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_pm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "freertos/task.h"
#include "soc/soc.h"
#include "soc/soc_memory_layout.h"
#include "soc/dport_reg.h"
#include "rom/lldesc.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include "esp_heap_caps.h"
#include "stdatomic.h"

typedef struct spi_device_t spi_device_t;
typedef typeof(SPI1.clock) spi_clock_reg_t;

#define NO_CS 3     // Number of CS pins per SPI host (0,1,2, 3 marks init)

#ifdef CONFIG_SPI_MASTER_ISR_IN_IRAM
#define SPI_MASTER_ISR_ATTR IRAM_ATTR
#else
#define SPI_MASTER_ISR_ATTR
#endif

#ifdef CONFIG_SPI_MASTER_IN_IRAM
#define SPI_MASTER_ATTR IRAM_ATTR
#else
#define SPI_MASTER_ATTR
#endif



//static ESP32_SPI_host_t *spihost[3];

static void spi_intr(void *arg);





/** to do: host not in use marker behaveior!
 * -------------------------------------------------------------------------------------------------
 *  FName: Bus Initialize
 *  Desc: Is called to initializes / allocates this Definitions spi-bus host (ESP32_SPI_host_t host)
 *  Info: 
 *  Para: ESP32_SPI_Definition_t* ESP32_SPI_Definition -> the 'Definition' that should init its SPI host  
 *        ESP32_SPI_host_device_t host_device -> the SPI pheriperal that this Definitions spi-host should use
 *  Rets: strTextMultiple_t* -> response: ptr to an singly linked tail queue element with return
 *                              message (error!) in allocated memory, or NULL = SCDE_OK / NO error message
 * -------------------------------------------------------------------------------------------------
 */
strTextMultiple_t*
ESP32_SPI_bus_initialize(ESP32_SPI_Definition_t* ESP32_SPI_Definition,
	ESP32_SPI_host_device_t host_device,
	const ESP32_SPI_bus_config_t* bus_config,
	int dma_chan)
{
  // for Fn response msg
  strTextMultiple_t* retMsg = SCDE_OK;

  bool spi_chan_claimed, dma_chan_claimed;

  /* ToDo: remove this when we have flash operations cooperating with this */
  SCDE_CHECK_LOADERRORTXT_RET(host_device != SPI_HOST,
	"SPI1 as peripheral for 'Host_Device' is not supported");

  SCDE_CHECK_LOADERRORTXT_RET(host_device >= SPI_HOST && host_device <= VSPI_HOST,
	"Invalid SPI peripheral selected for 'Host_Device'");

  SCDE_CHECK_LOADERRORTXT_RET(dma_chan >= 0 && dma_chan <= 2,
	"Invalid dma channel");

  SCDE_CHECK_LOADERRORTXT_RET( (bus_config->intr_flags & (ESP_INTR_FLAG_HIGH|ESP_INTR_FLAG_EDGE|ESP_INTR_FLAG_INTRDISABLED) ) == 0,
	 "Intr flag not allowed");

  #ifndef CONFIG_SPI_MASTER_ISR_IN_IRAM
  SCDE_CHECK_LOADERRORTXT_RET( ( bus_config->intr_flags & ESP_INTR_FLAG_IRAM ) == 0, "ESP_INTR_FLAG_IRAM should be disabled when CONFIG_SPI_MASTER_ISR_IN_IRAM is not set");
  #endif

  spi_chan_claimed = ESP32_SPI_common_periph_claim(host_device, "spi master");

  SCDE_CHECK_LOADERRORTXT_RET(spi_chan_claimed, "Host already in use");

  if ( dma_chan != 0 ) {

	dma_chan_claimed = ESP32_SPI_common_dma_chan_claim(dma_chan);

	if ( !dma_chan_claimed ) {

		ESP32_SPI_common_periph_free( host_device );

		SCDE_CHECK_LOADERRORTXT_RET(false, "DMA channel already in use");
	}
  }

  // alloc ESP32_SPI_host_t
  ESP32_SPI_host_t* host = malloc(sizeof(ESP32_SPI_host_t));

  // we got the mem?
  if ( host == NULL ) {

 	SCDE_CHECK_LOADERRORTXT(false, "Could not alloc 'ESP32_SPI_host_t'");

        goto cleanup;
  }

  // store ptr in this Definitions ESP32_SPI_Definition_t
  ESP32_SPI_Definition->p_host = host;

  // zero the memory
  memset(host, 0, sizeof(ESP32_SPI_host_t));

  // store the host-pheriperal this Definition is using
   host->host_device = host_device;

  // store the bus config for ?
  memcpy( &host->bus_cfg, bus_config, sizeof(ESP32_SPI_bus_config_t));

  #ifdef CONFIG_PM_ENABLE
  retMsg = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "spi_master",
            &host->pm_lock);

  if (retMsg) goto cleanup;

  #endif //CONFIG_PM_ENABLE

  retMsg = ESP32_SPI_common_bus_initialize_io(ESP32_SPI_Definition, host_device,
	bus_config, dma_chan, SPICOMMON_BUSFLAG_MASTER|bus_config->flags, &host->flags);

  if (retMsg) goto cleanup;

  host->dma_chan = dma_chan;

  if ( dma_chan == 0 ) {

	host->max_transfer_sz = 64;
  }

  else {

	// See how many dma descriptors we need and allocate them
	int dma_desc_ct = (bus_config->max_transfer_sz + SPI_MAX_DMA_LEN -1) / SPI_MAX_DMA_LEN;

	// default to 4k when max is not given
	if ( dma_desc_ct == 0 ) dma_desc_ct = 1; 

        host->max_transfer_sz = dma_desc_ct * SPI_MAX_DMA_LEN;
        host->dmadesc_tx = heap_caps_malloc(sizeof(lldesc_t) * dma_desc_ct, MALLOC_CAP_DMA);
        host->dmadesc_rx = heap_caps_malloc(sizeof(lldesc_t) * dma_desc_ct, MALLOC_CAP_DMA);

        if ( !host->dmadesc_tx || !host->dmadesc_rx ) {

		SCDE_CHECK_LOADERRORTXT(false, "Out of memory");

		goto cleanup;
	}
  }

  int flags = bus_config->intr_flags | ESP_INTR_FLAG_INTRDISABLED;

 esp_err_t err = esp_intr_alloc(ESP32_SPI_common_irqsource_for_host(host_device),
	flags, spi_intr, (void*) host, &host->intr);

  if (err != ESP_OK) {

	SCDE_CHECK_LOADERRORTXT(false, "got error from Fn 'esp_intr_alloc'");

	goto cleanup;
  }

  host->hw = ESP32_SPI_common_hw_for_host(host_device);

  host->cur_cs = NO_CS;
  host->prev_cs = NO_CS;
  atomic_store(&host->acquire_cs, NO_CS);
  host->polling = false;
  host->isr_free = true;
  host->bus_locked = false;

  // Reset DMA
  host->hw->dma_conf.val |= SPI_OUT_RST|SPI_IN_RST|SPI_AHBM_RST|SPI_AHBM_FIFO_RST;
  host->hw->dma_out_link.start = 0;
  host->hw->dma_in_link.start =0;
  host->hw->dma_conf.val &= ~(SPI_OUT_RST|SPI_IN_RST|SPI_AHBM_RST|SPI_AHBM_FIFO_RST);

  // Reset timing
  host->hw->ctrl2.val = 0;

  // master use all 64 bytes of the buffer
  host->hw->user.usr_miso_highpart = 0;
  host->hw->user.usr_mosi_highpart = 0;

  // Disable unneeded ints
  host->hw->slave.rd_buf_done = 0;
  host->hw->slave.wr_buf_done = 0;
  host->hw->slave.rd_sta_done = 0;
  host->hw->slave.wr_sta_done = 0;
  host->hw->slave.rd_buf_inten = 0;
  host->hw->slave.wr_buf_inten = 0;
  host->hw->slave.rd_sta_inten = 0;
  host->hw->slave.wr_sta_inten = 0;

  // Force a transaction done interrupt. This interrupt won't fire yet because we initialized the SPI interrupt as
  // disabled.  This way, we can just enable the SPI interrupt and the interrupt handler will kick in, handling
  // any transactions that are queued.
  host->hw->slave.trans_inten = 1;
  host->hw->slave.trans_done = 1;

  return retMsg;

cleanup:
  free(host->dmadesc_tx);
  free(host->dmadesc_rx);

  #ifdef CONFIG_PM_ENABLE
  if (host[host_device]->pm_lock) {

	esp_pm_lock_delete(host[host_device]->pm_lock);
  }
  #endif

  ESP32_SPI_common_periph_free(host_device);

  ESP32_SPI_common_dma_chan_free(dma_chan);

  return retMsg;
}



/** to do: host not in use marker behaveior!
 * -------------------------------------------------------------------------------------------------
 *  FName: Bus Free
 *  Desc: Is called to deinitialize / free this Definitions spi-bus host (ESP32_SPI_host_t host)
 *  Info: 
 *  Para: ESP32_SPI_Definition_t* ESP32_SPI_Definition -> the 'Definition' that should free its SPI host  
 *  Rets: strTextMultiple_t* -> response: ptr to an singly linked tail queue element with return
 *                              message (error!) in allocated memory, or NULL = SCDE_OK / NO error message
 * -------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* 
ESP32_SPI_bus_free(ESP32_SPI_Definition_t* ESP32_SPI_Definition)
{
  // for Fn response msg
  strTextMultiple_t* retMsg = SCDE_OK;

  int x;

  // get ptr to ESP32_SPI_host_t in this Definitions ESP32_SPI_Definition_t
  ESP32_SPI_host_t* host = ESP32_SPI_Definition->p_host;

  ESP32_SPI_host_device_t host_device = host->host_device;

  SCDE_CHECK_LOADERRORTXT_RET(host_device >= SPI_HOST && host_device <= VSPI_HOST,
	"Host_Device' pheriperal out of range");

//  SCDE_CHECK_LOADERRORTXT_RET(host[host_device] != NULL,
//	 "host was not in use");

  for ( x = 0 ; x < NO_CS ; x++ ) {

	SCDE_CHECK_LOADERRORTXT_RET(atomic_load(&host->device[x]) == NULL,
		"Not all CSses freed at this host");
  }

  ESP32_SPI_common_bus_free_io_cfg(&host->bus_cfg);

  if ( host->dma_chan > 0 ) {

	ESP32_SPI_common_dma_chan_free(host->dma_chan);
  }

  #ifdef CONFIG_PM_ENABLE
  esp_pm_lock_delete(host->pm_lock);
  #endif

  host->hw->slave.trans_inten=0;
  host->hw->slave.trans_done=0;

  esp_intr_free(host->intr);

  ESP32_SPI_common_periph_free(host_device);

  free(host->dmadesc_tx);

  free(host->dmadesc_rx);

  free(host);

  host = NULL;

  return retMsg;
}




void ESP32_SPI_get_timing(bool gpio_is_used,
	int input_delay_ns,
	int eff_clk,
	int* dummy_o,
	int* cycles_remain_o)
{
    const int apbclk_kHz = APB_CLK_FREQ/1000;
    //calculate how many apb clocks a period has
    const int apbclk_n = APB_CLK_FREQ/eff_clk;
    const int gpio_delay_ns = gpio_is_used ? 25 : 0;

    //calculate how many apb clocks the delay is, the 1 is to compensate in case ``input_delay_ns`` is rounded off.
    int apb_period_n = (1 + input_delay_ns + gpio_delay_ns)*apbclk_kHz/1000/1000;
    if (apb_period_n < 0) apb_period_n = 0;

    int dummy_required = apb_period_n/apbclk_n;

    int miso_delay = 0;
    if (dummy_required > 0) {
        //due to the clock delay between master and slave, there's a range in which data is random
        //give MISO a delay if needed to make sure we sample at the time MISO is stable
        miso_delay = (dummy_required+1)*apbclk_n-apb_period_n-1;
    } else {
        //if the dummy is not required, maybe we should also delay half a SPI clock if the data comes too early
        if (apb_period_n*4 <= apbclk_n) miso_delay = -1;
    }
    if (dummy_o!=NULL) *dummy_o = dummy_required;
    if (cycles_remain_o!=NULL) *cycles_remain_o = miso_delay;

    ESP_LOGD(SPI_TAG,"eff: %d, limit: %dk(/%d), %d dummy, %d delay", eff_clk/1000, apbclk_kHz/(apb_period_n+1), apb_period_n, dummy_required, miso_delay);
}



int ESP32_SPI_get_freq_limit(bool gpio_is_used,
	int input_delay_ns)
{
    const int apbclk_kHz = APB_CLK_FREQ/1000;
    const int gpio_delay_ns = gpio_is_used ? 25 : 0;

    //calculate how many apb clocks the delay is, the 1 is to compensate in case ``input_delay_ns`` is rounded off.
    int apb_period_n = (1 + input_delay_ns + gpio_delay_ns)*apbclk_kHz/1000/1000;
    if (apb_period_n < 0) apb_period_n = 0;

    return APB_CLK_FREQ/(apb_period_n+1);
}




/** to do: Host not initialized marker + check here + marker init host initialize/uninitialize
 * -------------------------------------------------------------------------------------------------
 *  FName: Add Device
 *  Desc: Is called to add an spi-device. This allocates a CS line for the device, allocates memory
 *        for the device structure and hooks up the CS pin to whatever is specified.
 *  Info: 
 *  Para: ESP32_SPI_Definition_t* ESP32_SPI_Definition -> the 'Definition' that should add a device
 *                                                        to its SPI host
 *        const ESP32_SPI_device_interface_config_t* dev_config -> the device interface configuration
 *        ESP32_SPI_device_handle_t* handle -> location in memory where the handle should be stored
 *  Rets: strTextMultiple_t* -> response: ptr to an singly linked tail queue element with return
 *                              message (error!) in allocated memory, or NULL = SCDE_OK / NO error message
 * -------------------------------------------------------------------------------------------------
 */
strTextMultiple_t*  
ESP32_SPI_bus_add_device(ESP32_SPI_Definition_t* ESP32_SPI_Definition,
	const ESP32_SPI_device_interface_config_t* dev_config,
	ESP32_SPI_device_handle_t* handle)
{
  // for Fn response msg
  strTextMultiple_t *retMsg = SCDE_OK;

  // create adressing-ptr to this Definitions Host
  ESP32_SPI_host_t* host = ESP32_SPI_Definition->p_host;

  // assigned spi-pheriperal
  ESP32_SPI_host_device_t host_device = host->host_device;

  int freecs;
  int apbclk=APB_CLK_FREQ;
  int eff_clk;
  int duty_cycle;
  int dummy_required;
  int miso_delay;

  ESP32_SPI_clock_reg_t clk_reg;

  SCDE_CHECK_LOADERRORTXT_RET(host_device >= SPI_HOST && host_device <= VSPI_HOST,
	 "Device to add has invalid host");

//x  SCDE_CHECK_LOADERRORTXT_RET(host[host_device] != NULL,
//	 "Device to add has not initialized host");

  SCDE_CHECK_LOADERRORTXT_RET(dev_config->spics_io_num < 0 || GPIO_IS_VALID_OUTPUT_GPIO(dev_config->spics_io_num),
	 "Device to add has invalid Spics pin");

  SCDE_CHECK_LOADERRORTXT_RET(dev_config->clock_speed_hz > 0,
	"Device to add has invalid invalid sclk speed");

  for ( freecs = 0 ; freecs < NO_CS; freecs++ ) {

        // See if this slot is free; reserve if it is by putting a dummy pointer in the slot.
	//  We use an atomic compare&swap to make this thread-safe.
	void* null = NULL;
	if ( atomic_compare_exchange_strong(&host->device[freecs], &null, (spi_device_t *) 1) ) break;
  }

  SCDE_CHECK_LOADERRORTXT_RET(freecs != NO_CS,
	"Device to add has no free cs pins at host");

  // The hardware looks like it would support this, but actually setting cs_ena_pretrans when transferring in full
  // duplex mode does absolutely nothing on the ESP32.
  SCDE_CHECK_LOADERRORTXT_RET(dev_config->cs_ena_pretrans <= 1 ||
			    (dev_config->address_bits == 0 &&
			     dev_config->command_bits == 0) ||
       			    (dev_config->flags & SPI_DEVICE_HALFDUPLEX),
		"In full-duplex mode, only support cs pretrans delay = 1 and without address_bits and command_bits");

  duty_cycle = (dev_config->duty_cycle_pos == 0) ? 128 : dev_config->duty_cycle_pos;

  eff_clk = ESP32_SPI_cal_clock(apbclk, dev_config->clock_speed_hz, duty_cycle, (uint32_t*) &clk_reg);

  int freq_limit = ESP32_SPI_get_freq_limit(!(host->flags&SPICOMMON_BUSFLAG_NATIVE_PINS),
	 dev_config->input_delay_ns);

  // Speed >=40MHz over GPIO matrix needs a dummy cycle, but these don't work for full-duplex connections.
  ESP32_SPI_get_timing(!(host->flags&SPICOMMON_BUSFLAG_NATIVE_PINS),
	dev_config->input_delay_ns,
	eff_clk,
	&dummy_required,
	&miso_delay);


  SCDE_CHECK_LOADERRORTXT_RET2(dev_config->flags & SPI_DEVICE_HALFDUPLEX || dummy_required == 0 || dev_config->flags & SPI_DEVICE_NO_DUMMY,
	"When work in full-duplex mode at frequency > %.1fMHz, device cannot read correct data.\n\
	Try to use IOMUX pins to increase the frequency limit, or use the half duplex mode.\n\
	Please note the SPI master can only work at divisors of 80MHz, and the driver always tries to find the closest frequency to your 	configuration.\n\
	Specify ``SPI_DEVICE_NO_DUMMY`` to ignore this checking. Then you can output data at higher speed, or read data at your own risk.",
            freq_limit/1000./1000 );

  // allocate memory for device
  ESP32_SPI_device_t *dev = malloc(sizeof(ESP32_SPI_device_t));

  // alloc failed ?
  if ( dev == NULL ) goto nomem;

  // start clean ...
  memset(dev, 0, sizeof(ESP32_SPI_device_t));

  // now init struct ...
  atomic_store(&host->device[freecs], dev);

  // will we need a link to the definition?
//  dev->ESP32_SPI_Definition = ESP32_SPI_Definition;

  dev->id = freecs;

  dev->waiting = false;

  // allocate queues, set defaults
  dev->trans_queue = xQueueCreate(dev_config->queue_size, sizeof(ESP32_SPI_trans_priv_t));
  dev->ret_queue = xQueueCreate(dev_config->queue_size, sizeof(ESP32_SPI_trans_priv_t));
  dev->semphr_polling = xSemaphoreCreateBinary();

  if (!dev->trans_queue || !dev->ret_queue || !dev->semphr_polling) {

	goto nomem;
  }

  dev->host = host;

  // we want to save a copy of the dev config in the dev struct.
  memcpy(&dev->cfg, dev_config, sizeof(ESP32_SPI_device_interface_config_t));

  dev->cfg.duty_cycle_pos = duty_cycle;

  // TODO: if we have to change the apb clock among transactions, re-calculate this each time the apb clock lock is acquired.
  dev->clk_cfg= (clock_config_t) {
	.eff_clk = eff_clk,
	.dummy_num = dummy_required,
	.reg = clk_reg,
	.miso_delay = miso_delay,
  };

  // Set CS pin, CS options
  if (dev_config->spics_io_num >= 0) {

	ESP32_SPI_common_cs_initialize(host_device, dev_config->spics_io_num, freecs, !(host->flags&SPICOMMON_BUSFLAG_NATIVE_PINS));
  }

  if (dev_config->flags&SPI_DEVICE_CLK_AS_CS) {

	host->hw->pin.master_ck_sel |= (1<<freecs);
  }

  else {

	host->hw->pin.master_ck_sel &= (1<<freecs);
  }

  if (dev_config->flags&SPI_DEVICE_POSITIVE_CS) {

	host->hw->pin.master_cs_pol |= (1<<freecs);
  }

  else {

	host->hw->pin.master_cs_pol &= (1<<freecs);
  }

  host->hw->ctrl2.mosi_delay_mode = 0;
  host->hw->ctrl2.mosi_delay_num = 0;

  *handle = dev;

  ESP_LOGD(SPI_TAG, "(@ SPI peripheral %d): New device added to CS%d, effective clock: %dkHz",
	host_device + 1,
	freecs,
	dev->clk_cfg.eff_clk/1000);

  return retMsg;

// abort in case of no mem here
nomem:
  if (dev) {

	if (dev->trans_queue) vQueueDelete(dev->trans_queue);

	if (dev->ret_queue) vQueueDelete(dev->ret_queue);

	if (dev->semphr_polling) vSemaphoreDelete(dev->semphr_polling);
  }

  free(dev);

  return retMsg;
}



strTextMultiple_t* 
ESP32_SPI_bus_remove_device(ESP32_SPI_device_handle_t handle)
{
  // for Fn response msg
  strTextMultiple_t *retMsg = SCDE_OK;

  int x;

  SCDE_CHECK_LOADERRORTXT_RET(handle != NULL,
	"Device handle is invalid");

  SCDE_CHECK_LOADERRORTXT_RET(uxQueueMessagesWaiting(handle->trans_queue) == 0,
	"Invalid State. Unfinished transactions");

  SCDE_CHECK_LOADERRORTXT_RET(handle->host->cur_cs == NO_CS || atomic_load(&handle->host->device[handle->host->cur_cs]) != handle,
	"Invalid State. Unfinished transactions");

  SCDE_CHECK_LOADERRORTXT_RET(uxQueueMessagesWaiting(handle->ret_queue) ==0,
	"Invalid State. Unfinished transactions");

  // return
  int spics_io_num = handle->cfg.spics_io_num;

  if ( spics_io_num >= 0 ) ESP32_SPI_common_cs_free_io(spics_io_num);

  // Kill queues
  vQueueDelete(handle->trans_queue);
  vQueueDelete(handle->ret_queue);
  vSemaphoreDelete(handle->semphr_polling);

  // Remove device from list of csses and free memory
  for ( x = 0 ; x < NO_CS ; x++ ) {

	if ( atomic_load(&handle->host->device[x] ) == handle ) {

		atomic_store(&handle->host->device[x], NULL);

		if ( x == handle->host->prev_cs ) handle->host->prev_cs = NO_CS;
	}
  }

  free(handle);

  return retMsg;
}



static int ESP32_SPI_freq_for_pre_n(int fapb, int pre, int n)
{
    return (fapb / (pre * n));
}

int ESP32_SPI_cal_clock(int fapb, int hz, int duty_cycle, uint32_t *reg_o)
{
    ESP32_SPI_clock_reg_t reg;
    int eff_clk;

    //In hw, n, h and l are 1-64, pre is 1-8K. Value written to register is one lower than used value.
    if (hz>((fapb/4)*3)) {
        //Using Fapb directly will give us the best result here.
        reg.clkcnt_l=0;
        reg.clkcnt_h=0;
        reg.clkcnt_n=0;
        reg.clkdiv_pre=0;
        reg.clk_equ_sysclk=1;
        eff_clk=fapb;
    } else {
        //For best duty cycle resolution, we want n to be as close to 32 as possible, but
        //we also need a pre/n combo that gets us as close as possible to the intended freq.
        //To do this, we bruteforce n and calculate the best pre to go along with that.
        //If there's a choice between pre/n combos that give the same result, use the one
        //with the higher n.
        int pre, n, h, l;
        int bestn=-1;
        int bestpre=-1;
        int besterr=0;
        int errval;
        for (n=2; n<=64; n++) { //Start at 2: we need to be able to set h/l so we have at least one high and one low pulse.
            //Effectively, this does pre=round((fapb/n)/hz).
            pre=((fapb/n)+(hz/2))/hz;
            if (pre<=0) pre=1;
            if (pre>8192) pre=8192;
            errval=abs(ESP32_SPI_freq_for_pre_n(fapb, pre, n)-hz);
            if (bestn==-1 || errval<=besterr) {
                besterr=errval;
                bestn=n;
                bestpre=pre;
            }
        }

        n=bestn;
        pre=bestpre;
        l=n;
        //This effectively does round((duty_cycle*n)/256)
        h=(duty_cycle*n+127)/256;
        if (h<=0) h=1;

        reg.clk_equ_sysclk=0;
        reg.clkcnt_n=n-1;
        reg.clkdiv_pre=pre-1;
        reg.clkcnt_h=h-1;
        reg.clkcnt_l=l-1;
        eff_clk=ESP32_SPI_freq_for_pre_n(fapb, pre, n);
    }
    if (reg_o != NULL) *reg_o = reg.val;
    return eff_clk;
}

/*
 * Set the spi clock according to pre-calculated register value.
 */
static inline void SPI_MASTER_ISR_ATTR ESP32_SPI_set_clock(spi_dev_t *hw, spi_clock_reg_t reg)
{
    hw->clock.val = reg.val;
}

// Setup the device-specified configuration registers. Called every time a new
// transaction is to be sent, but only apply new configurations when the device
// changes.
static void SPI_MASTER_ISR_ATTR 
ESP32_SPI_setup_device(ESP32_SPI_host_t *host, int dev_id )
{
  // if the configuration is already applied, skip the following.
  if (dev_id == host->prev_cs) {

	return;
  }

  ESP_EARLY_LOGD(SPI_TAG, "SPI device changed from %d to %d", host->prev_cs, dev_id);

  ESP32_SPI_device_t *dev = atomic_load(&host->device[dev_id]);

  // Configure clock settings
  ESP32_SPI_set_clock(host->hw, dev->clk_cfg.reg);

  // Configure bit order
  host->hw->ctrl.rd_bit_order=(dev->cfg.flags & SPI_DEVICE_RXBIT_LSBFIRST) ? 1 : 0;
  host->hw->ctrl.wr_bit_order=(dev->cfg.flags & SPI_DEVICE_TXBIT_LSBFIRST) ? 1 : 0;

  // Configure polarity
  if (dev->cfg.mode == 0) {

	host->hw->pin.ck_idle_edge=0;
	host->hw->user.ck_out_edge=0;
  }

  else if (dev->cfg.mode==1) {

	host->hw->pin.ck_idle_edge=0;
	host->hw->user.ck_out_edge=1;
  }

  else if (dev->cfg.mode==2) {

	host->hw->pin.ck_idle_edge=1;
	host->hw->user.ck_out_edge=1;
  }

  else if (dev->cfg.mode==3) {

	host->hw->pin.ck_idle_edge=1;
	host->hw->user.ck_out_edge=0;
  }

  // Configure misc stuff
  host->hw->user.doutdin=(dev->cfg.flags & SPI_DEVICE_HALFDUPLEX) ? 0 : 1;
  host->hw->user.sio=(dev->cfg.flags & SPI_DEVICE_3WIRE) ? 1 : 0;

  // Configure CS pin and timing
  host->hw->ctrl2.setup_time=dev->cfg.cs_ena_pretrans-1;
  host->hw->user.cs_setup=dev->cfg.cs_ena_pretrans ? 1 : 0;

  // set hold_time to 0 will not actually append delay to CS
  // set it to 1 since we do need at least one clock of hold time in most cases
  host->hw->ctrl2.hold_time=dev->cfg.cs_ena_posttrans;
  if (host->hw->ctrl2.hold_time == 0) host->hw->ctrl2.hold_time = 1;
  host->hw->user.cs_hold=1;

  host->hw->pin.cs0_dis = (dev_id == 0) ? 0 : 1;
  host->hw->pin.cs1_dis = (dev_id == 1) ? 0 : 1;
  host->hw->pin.cs2_dis = (dev_id == 2) ? 0 : 1;

  //Record the device just configured to save time for next time
  host->prev_cs = dev_id;
}

/*-----------------------------------------------------------------------------
    Arbitration Functions
-----------------------------------------------------------------------------*/

static inline void spi_isr_invoke(ESP32_SPI_device_t *dev)
{
    int acquire_cs = atomic_load(&dev->host->acquire_cs);
    if (acquire_cs == dev->id || acquire_cs == NO_CS) {
        esp_intr_enable(dev->host->intr);
    }
    //otherwise wait for bus release to invoke
}

/*  This function try to race for the arbitration between devices.
 *  Even if this returns successfully, the ISR may be still running.
 *  Call device_wait_for_isr_idle to make sure the ISR is done.
 */
static SPI_MASTER_ISR_ATTR esp_err_t 
device_acquire_bus_internal(ESP32_SPI_device_t *handle, TickType_t wait)
{
    ESP32_SPI_host_t *host = handle->host;
    SPI_CHECK(wait==portMAX_DELAY, "acquire finite time not supported now.", ESP_ERR_INVALID_ARG);

    if (atomic_load(&host->acquire_cs) == handle->id) {
        // Quickly skip if the bus is already acquired.
        // Usually this is only when the bus is locked.
        assert(host->bus_locked);
        return ESP_OK;
    } else {
        // Declare we are waiting for the bus so that if we get blocked later, other device or the ISR will yield to us after their using.
        handle->waiting = true;
        // Clear the semaphore before checking
        xSemaphoreTake(handle->semphr_polling, 0);

        int no_cs = NO_CS;
        atomic_compare_exchange_weak(&host->acquire_cs, &no_cs, handle->id);
        if (atomic_load(&host->acquire_cs) != handle->id) {
            //block until the bus is acquired (help by other task)
            BaseType_t ret = xSemaphoreTake(handle->semphr_polling, wait);
            //TODO: add timeout handling here.
            if (ret == pdFALSE) return ESP_ERR_TIMEOUT;
        }
        handle->waiting = false;
    }
    return ESP_OK;
}

/*  This function check for whether the ISR is done, if not, block until semaphore given.
 */
static inline SPI_MASTER_ISR_ATTR 
esp_err_t 
device_wait_for_isr_idle(ESP32_SPI_device_t *handle, TickType_t wait)
{
    //quickly skip if the isr is already free
    if (!handle->host->isr_free) {
        // Clear the semaphore before checking
        xSemaphoreTake(handle->semphr_polling, 0);
        if (!handle->host->isr_free) {
            //block until the the isr is free and give us the semaphore
            BaseType_t ret = xSemaphoreTake(handle->semphr_polling, wait);
            //TODO: add timeout handling here.
            if (ret == pdFALSE) return ESP_ERR_TIMEOUT;
        }
    }
    return ESP_OK;
}

/*  This function release the bus acquired by device_acquire_internal.
    And it also tries to help other device to acquire the bus.
    If the bus acquring is not needed, it goes through all device queues to see whether to invoke the ISR
 */
static SPI_MASTER_ISR_ATTR void device_release_bus_internal(ESP32_SPI_host_t *host)
{
    //release the bus
    atomic_store(&host->acquire_cs, NO_CS);
    //first try to restore the acquiring device
    for (int i = 0; i < NO_CS; i++) {
        //search for all registered devices
        ESP32_SPI_device_t* dev = atomic_load(&host->device[i]);
        if (dev && dev->waiting) {
            int no_cs = NO_CS;
            atomic_compare_exchange_weak(&host->acquire_cs, &no_cs, i);
            if (atomic_load(&host->acquire_cs) == i) {
                // Success to acquire for new device
                BaseType_t ret = uxQueueMessagesWaiting(dev->trans_queue);
                if (ret > 0) {
                    // If there are transactions in the queue, the ISR should be invoked first
                    // Resume the interrupt to send the task a signal
                    spi_isr_invoke(dev);
                } else {
                    // Otherwise resume the task directly.
                    xSemaphoreGive(dev->semphr_polling);
                }
            }
            return;
        }
    }
    //if no devices waiting, searching in device queues to see whether to recover the ISR
    for( int i = 0; i < NO_CS; i++) {
        ESP32_SPI_device_t *dev = atomic_load(&host->device[i]);
        if (dev == NULL) continue;
        BaseType_t ret = uxQueueMessagesWaiting(dev->trans_queue);
        if ( ret != 0) {
            spi_isr_invoke(dev);
            return;
        }
    }
}

static inline SPI_MASTER_ISR_ATTR bool device_is_polling(ESP32_SPI_device_t *handle)
{
    return atomic_load(&handle->host->acquire_cs) == handle->id && handle->host->polling;
}

/*-----------------------------------------------------------------------------
    Working Functions
-----------------------------------------------------------------------------*/

// The function is called to send a new transaction, in ISR or in the task.
// Setup the transaction-specified registers and linked-list used by the DMA (or FIFO if DMA is not used)
static void SPI_MASTER_ISR_ATTR 
spi_new_trans(ESP32_SPI_device_t *dev, ESP32_SPI_trans_priv_t *trans_buf)
{
    ESP32_SPI_transaction_t *trans = NULL;
    ESP32_SPI_host_t *host = dev->host;
    int dev_id = dev->id;

    //clear int bit
    host->hw->slave.trans_done = 0;

    trans = trans_buf->trans;
    host->cur_cs = dev_id;
    //We should be done with the transmission.
    assert(host->hw->cmd.usr == 0);

    //Reconfigure according to device settings, the function only has effect when the dev_id is changed.
    ESP32_SPI_setup_device(host, dev_id);

    //Reset DMA peripheral
    host->hw->dma_conf.val |= SPI_OUT_RST|SPI_IN_RST|SPI_AHBM_RST|SPI_AHBM_FIFO_RST;
    host->hw->dma_out_link.start=0;
    host->hw->dma_in_link.start=0;
    host->hw->dma_conf.val &= ~(SPI_OUT_RST|SPI_IN_RST|SPI_AHBM_RST|SPI_AHBM_FIFO_RST);
    host->hw->dma_conf.out_data_burst_en=1;
    host->hw->dma_conf.indscr_burst_en=1;
    host->hw->dma_conf.outdscr_burst_en=1;
    //Set up QIO/DIO if needed
    host->hw->ctrl.val &= ~(SPI_FREAD_DUAL|SPI_FREAD_QUAD|SPI_FREAD_DIO|SPI_FREAD_QIO);
    host->hw->user.val &= ~(SPI_FWRITE_DUAL|SPI_FWRITE_QUAD|SPI_FWRITE_DIO|SPI_FWRITE_QIO);
    if (trans->flags & SPI_TRANS_MODE_DIO) {
        if (trans->flags & SPI_TRANS_MODE_DIOQIO_ADDR) {
            host->hw->ctrl.fread_dio=1;
            host->hw->user.fwrite_dio=1;
        } else {
            host->hw->ctrl.fread_dual=1;
            host->hw->user.fwrite_dual=1;
        }
        host->hw->ctrl.fastrd_mode=1;
    } else if (trans->flags & SPI_TRANS_MODE_QIO) {
        if (trans->flags & SPI_TRANS_MODE_DIOQIO_ADDR) {
            host->hw->ctrl.fread_qio=1;
            host->hw->user.fwrite_qio=1;
        } else {
            host->hw->ctrl.fread_quad=1;
            host->hw->user.fwrite_quad=1;
        }
        host->hw->ctrl.fastrd_mode=1;
    }

    //Fill DMA descriptors
    int extra_dummy=0;
    if (trans_buf->buffer_to_rcv) {
        if (host->dma_chan == 0) {
            //No need to setup anything; we'll copy the result out of the work registers directly later.
        } else {
            ESP32_SPI_common_setup_dma_desc_links(host->dmadesc_rx, ((trans->rxlength+7)/8), (uint8_t*)trans_buf->buffer_to_rcv, true);
            host->hw->dma_in_link.addr=(int)(&host->dmadesc_rx[0]) & 0xFFFFF;
            host->hw->dma_in_link.start=1;
        }
        //when no_dummy is not set and in half-duplex mode, sets the dummy bit if RX phase exist
        if (((dev->cfg.flags&SPI_DEVICE_NO_DUMMY)==0) && (dev->cfg.flags&SPI_DEVICE_HALFDUPLEX)) {
            extra_dummy=dev->clk_cfg.dummy_num;
        }
    } else {
        //DMA temporary workaround: let RX DMA work somehow to avoid the issue in ESP32 v0/v1 silicon
        if (host->dma_chan != 0 ) {
            host->hw->dma_in_link.addr=0;
            host->hw->dma_in_link.start=1;
        }
    }

    if (trans_buf->buffer_to_send) {
        if (host->dma_chan == 0) {
            //Need to copy data to registers manually
            for (int x=0; x < trans->length; x+=32) {
                //Use memcpy to get around alignment issues for txdata
                uint32_t word;
                memcpy(&word, &trans_buf->buffer_to_send[x/32], 4);
                host->hw->data_buf[(x/32)]=word;
            }
        } else {
            ESP32_SPI_common_setup_dma_desc_links(host->dmadesc_tx, (trans->length+7)/8, (uint8_t*)trans_buf->buffer_to_send, false);
            host->hw->dma_out_link.addr=(int)(&host->dmadesc_tx[0]) & 0xFFFFF;
            host->hw->dma_out_link.start=1;
        }
    }

    //SPI iface needs to be configured for a delay in some cases.
    //configure dummy bits
    host->hw->user.usr_dummy=(dev->cfg.dummy_bits+extra_dummy) ? 1 : 0;
    host->hw->user1.usr_dummy_cyclelen=dev->cfg.dummy_bits+extra_dummy-1;

    int miso_long_delay = 0;
    if (dev->clk_cfg.miso_delay<0) {
        //if the data comes too late, delay half a SPI clock to improve reading
        miso_long_delay = 1;
        host->hw->ctrl2.miso_delay_num = 0;
    } else {
        //if the data is so fast that dummy_bit is used, delay some apb clocks to meet the timing
        host->hw->ctrl2.miso_delay_num = extra_dummy ? dev->clk_cfg.miso_delay : 0;
    }

    if (miso_long_delay) {
        switch (dev->cfg.mode) {
        case 0:
            host->hw->ctrl2.miso_delay_mode = 2;
            break;
        case 1:
            host->hw->ctrl2.miso_delay_mode = 1;
            break;
        case 2:
            host->hw->ctrl2.miso_delay_mode = 1;
            break;
        case 3:
            host->hw->ctrl2.miso_delay_mode = 2;
            break;
        }
    } else {
        host->hw->ctrl2.miso_delay_mode = 0;
    }

    host->hw->mosi_dlen.usr_mosi_dbitlen=trans->length-1;
    if ( dev->cfg.flags & SPI_DEVICE_HALFDUPLEX ) {
        host->hw->miso_dlen.usr_miso_dbitlen=trans->rxlength-1;
    } else {
        //rxlength is not used in full-duplex mode
        host->hw->miso_dlen.usr_miso_dbitlen=trans->length-1;
    }

    //Configure bit sizes, load addr and command
    int cmdlen;
    int addrlen;
    if (!(dev->cfg.flags & SPI_DEVICE_HALFDUPLEX) && dev->cfg.cs_ena_pretrans != 0) {
        /* The command and address phase is not compatible with cs_ena_pretrans
         * in full duplex mode.
         */
        cmdlen = 0;
        addrlen = 0;
    } else {
        if (trans->flags & SPI_TRANS_VARIABLE_CMD) {
            cmdlen = ((ESP32_SPI_transaction_ext_t *)trans)->command_bits;
        } else {
            cmdlen = dev->cfg.command_bits;
        }
        if (trans->flags & SPI_TRANS_VARIABLE_ADDR) {
            addrlen = ((ESP32_SPI_transaction_ext_t *)trans)->address_bits;
        } else {
            addrlen = dev->cfg.address_bits;
        }
    }

    host->hw->user1.usr_addr_bitlen=addrlen-1;
    host->hw->user2.usr_command_bitlen=cmdlen-1;
    host->hw->user.usr_addr=addrlen ? 1 : 0;
    host->hw->user.usr_command=cmdlen ? 1 : 0;

    if ((dev->cfg.flags & SPI_DEVICE_TXBIT_LSBFIRST)==0) {
        /* Output command will be sent from bit 7 to 0 of command_value, and
         * then bit 15 to 8 of the same register field. Shift and swap to send
         * more straightly.
         */
        host->hw->user2.usr_command_value = SPI_SWAP_DATA_TX(trans->cmd, cmdlen);

        // shift the address to MSB of addr (and maybe slv_wr_status) register.
        // output address will be sent from MSB to LSB of addr register, then comes the MSB to LSB of slv_wr_status register.
        if (addrlen > 32) {
            host->hw->addr = trans->addr >> (addrlen - 32);
            host->hw->slv_wr_status = trans->addr << (64 - addrlen);
        } else {
            host->hw->addr = trans->addr << (32 - addrlen);
        }
    } else {
        /* The output command start from bit0 to bit 15, kept as is.
         * The output address start from the LSB of the highest byte, i.e.
         * addr[24] -> addr[31]
         * ...
         * addr[0] -> addr[7]
         * slv_wr_status[24] -> slv_wr_status[31]
         * ...
         * slv_wr_status[0] -> slv_wr_status[7]
         * So swap the byte order to let the LSB sent first.
         */
        host->hw->user2.usr_command_value = trans->cmd;
        uint64_t addr = __builtin_bswap64(trans->addr);
        host->hw->addr = addr>>32;
        host->hw->slv_wr_status = addr;
    }

    if ((!(dev->cfg.flags & SPI_DEVICE_HALFDUPLEX) && trans_buf->buffer_to_rcv) ||
        trans_buf->buffer_to_send) {
        host->hw->user.usr_mosi = 1;
    } else {
        host->hw->user.usr_mosi = 0;
    }
    host->hw->user.usr_miso = (trans_buf->buffer_to_rcv) ? 1 : 0;

    //Call pre-transmission callback, if any
    if (dev->cfg.pre_cb) dev->cfg.pre_cb(trans);
    //Kick off transfer
    host->hw->cmd.usr=1;
}

// The function is called when a transaction is done, in ISR or in the task.
// Fetch the data from FIFO and call the ``post_cb``.
static void SPI_MASTER_ISR_ATTR 
ESP32_SPI_post_trans(ESP32_SPI_host_t *host)
{
  ESP32_SPI_transaction_t *cur_trans = host->cur_trans_buf.trans;

  if (host->cur_trans_buf.buffer_to_rcv && host->dma_chan == 0 ) {

	// Need to copy from SPI regs to result buffer.
	for (int x = 0; x < cur_trans->rxlength; x += 32) {

		// Do a memcpy to get around possible alignment issues in rx_buffer
		uint32_t word = host->hw->data_buf[x / 32];

		int len = cur_trans->rxlength - x;

		if (len > 32) len = 32;

		memcpy(&host->cur_trans_buf.buffer_to_rcv[x / 32], &word, (len + 7) / 8);
	}
  }

  // Call post-transaction callback, if any
  ESP32_SPI_device_t* dev = atomic_load(&host->device[host->cur_cs]);
  if (dev->cfg.post_cb) dev->cfg.post_cb(cur_trans);

  host->cur_cs = NO_CS;
}



// This is run in interrupt context.
static void SPI_MASTER_ISR_ATTR spi_intr(void *arg)
{
    int i;
    BaseType_t r;
    BaseType_t do_yield = pdFALSE;
    ESP32_SPI_host_t *host = (ESP32_SPI_host_t *)arg;

    assert(host->hw->slave.trans_done == 1);

    /*------------ deal with the in-flight transaction -----------------*/
    if (host->cur_cs != NO_CS) {
        //Okay, transaction is done.
        const int cs = host->cur_cs;
        //Tell common code DMA workaround that our DMA channel is idle. If needed, the code will do a DMA reset.
        if (host->dma_chan) {
            ESP32_SPI_common_dmaworkaround_idle(host->dma_chan);
        }

        //cur_cs is changed to NO_CS here
        ESP32_SPI_post_trans(host);
        //Return transaction descriptor.
        xQueueSendFromISR(atomic_load(&host->device[cs])->ret_queue, &host->cur_trans_buf, &do_yield);
#ifdef CONFIG_PM_ENABLE
        //Release APB frequency lock
        esp_pm_lock_release(host->pm_lock);
#endif
    }

    /*------------ new transaction starts here ------------------*/
    assert(host->cur_cs == NO_CS);

    // Clear isr_free before the checking of acquire_cs so that the task will always block if we find the bus is not acquired.
    // Small possiblility that the task is blocked but we find the bus is acquired.
    host->isr_free = false;

    /* When the bus is not occupied, the task uses esp_intr_enable to inform the ISR there's new transaction.
     * If the queue is empty, we disable the system interrupt.
     * We disable this first, to avoid the conflict when the task enable and the ISR disable at the same time
     * If the transaction is sent (queue not empty), we will re-ebale it (see below).
     */
    esp_intr_disable( host->intr );
    int acquire_cs = atomic_load(&host->acquire_cs);
    if (acquire_cs != NO_CS) {
        // Only look in the queue of the occupying device.
        i = acquire_cs;
        ESP32_SPI_device_t* dev = atomic_load(&host->device[i]);
        assert(dev);
        r = xQueueReceiveFromISR(dev->trans_queue, &host->cur_trans_buf, &do_yield);
        // If the Queue is empty, skip the sending by setting i=NO_CS
        // Otherwise i is kept as is and the transaction will be sent.
        if (!r) {
            // Set the free to true before resume the task
            host->isr_free = true;
            xSemaphoreGiveFromISR(dev->semphr_polling, &do_yield);
            i = NO_CS;
        }
    } else {
        //Go through all device queues to find a transaction to send
        //ToDo: This is a stupidly simple low-cs-first priority scheme. Make this configurable somehow. - JD
        for (i = 0; i < NO_CS; i++) {
            ESP32_SPI_device_t* dev = atomic_load(&host->device[i]);
            if (dev) {
                r = xQueueReceiveFromISR(dev->trans_queue, &host->cur_trans_buf, &do_yield);
                //Stop looking if we have a transaction to send.
                if (r) break;
            }
        }
        if (i==NO_CS) {
            host->isr_free = true;
        }
    }

    // Actually send the transaction
    if (i != NO_CS) {
        ESP32_SPI_trans_priv_t *const cur_trans_buf = &host->cur_trans_buf;
        if (host->dma_chan != 0 && (cur_trans_buf->buffer_to_rcv || cur_trans_buf->buffer_to_send)) {
            //mark channel as active, so that the DMA will not be reset by the slave
            ESP32_SPI_common_dmaworkaround_transfer_active(host->dma_chan);
        }
        spi_new_trans(atomic_load(&host->device[i]), cur_trans_buf);
        //re-enable interrupt disabled above
        esp_intr_enable(host->intr);
    }
    if (do_yield) portYIELD_FROM_ISR();
}

static SPI_MASTER_ISR_ATTR esp_err_t 
check_trans_valid(ESP32_SPI_device_handle_t handle, ESP32_SPI_transaction_t *trans_desc)
{
    SPI_CHECK(handle!=NULL, "invalid dev handle", ESP_ERR_INVALID_ARG);
    ESP32_SPI_host_t *host = handle->host;
    //check transmission length
    SPI_CHECK((trans_desc->flags & SPI_TRANS_USE_RXDATA)==0 ||trans_desc->rxlength <= 32, "rxdata transfer > 32 bits without configured DMA", ESP_ERR_INVALID_ARG);
    SPI_CHECK((trans_desc->flags & SPI_TRANS_USE_TXDATA)==0 ||trans_desc->length <= 32, "txdata transfer > 32 bits without configured DMA", ESP_ERR_INVALID_ARG);
    SPI_CHECK(trans_desc->length <= handle->host->max_transfer_sz*8, "txdata transfer > host maximum", ESP_ERR_INVALID_ARG);
    SPI_CHECK(trans_desc->rxlength <= handle->host->max_transfer_sz*8, "rxdata transfer > host maximum", ESP_ERR_INVALID_ARG);
    SPI_CHECK((handle->cfg.flags & SPI_DEVICE_HALFDUPLEX) || trans_desc->rxlength <= trans_desc->length, "rx length > tx length in full duplex mode", ESP_ERR_INVALID_ARG);
    //check working mode
    SPI_CHECK(!((trans_desc->flags & (SPI_TRANS_MODE_DIO|SPI_TRANS_MODE_QIO)) && (handle->cfg.flags & SPI_DEVICE_3WIRE)), "incompatible iface params", ESP_ERR_INVALID_ARG);
    SPI_CHECK(!((trans_desc->flags & (SPI_TRANS_MODE_DIO|SPI_TRANS_MODE_QIO)) && (!(handle->cfg.flags & SPI_DEVICE_HALFDUPLEX))), "incompatible iface params", ESP_ERR_INVALID_ARG);
    SPI_CHECK( !(handle->cfg.flags & SPI_DEVICE_HALFDUPLEX) || host->dma_chan == 0 || !(trans_desc->flags & SPI_TRANS_USE_RXDATA || trans_desc->rx_buffer != NULL)
        || !(trans_desc->flags & SPI_TRANS_USE_TXDATA || trans_desc->tx_buffer!=NULL), "SPI half duplex mode does not support using DMA with both MOSI and MISO phases.", ESP_ERR_INVALID_ARG );
    //MOSI phase is skipped only when both tx_buffer and SPI_TRANS_USE_TXDATA are not set.
    SPI_CHECK(trans_desc->length != 0 || (trans_desc->tx_buffer == NULL && !(trans_desc->flags & SPI_TRANS_USE_TXDATA)),
        "trans tx_buffer should be NULL and SPI_TRANS_USE_TXDATA should be cleared to skip MOSI phase.", ESP_ERR_INVALID_ARG);
    //MISO phase is skipped only when both rx_buffer and SPI_TRANS_USE_RXDATA are not set.
    //If set rxlength=0 in full_duplex mode, it will be automatically set to length
    SPI_CHECK(!(handle->cfg.flags & SPI_DEVICE_HALFDUPLEX) || trans_desc->rxlength != 0 ||
        (trans_desc->rx_buffer == NULL && ((trans_desc->flags & SPI_TRANS_USE_RXDATA)==0)),
        "trans rx_buffer should be NULL and SPI_TRANS_USE_RXDATA should be cleared to skip MISO phase.", ESP_ERR_INVALID_ARG);
    //In Full duplex mode, default rxlength to be the same as length, if not filled in.
    // set rxlength to length is ok, even when rx buffer=NULL
    if (trans_desc->rxlength==0 && !(handle->cfg.flags & SPI_DEVICE_HALFDUPLEX)) {
        trans_desc->rxlength=trans_desc->length;
    }

    return ESP_OK;
}

static SPI_MASTER_ISR_ATTR void uninstall_priv_desc(ESP32_SPI_trans_priv_t* trans_buf)
{
    ESP32_SPI_transaction_t *trans_desc = trans_buf->trans;
    if ((void *)trans_buf->buffer_to_send != &trans_desc->tx_data[0] &&
        trans_buf->buffer_to_send != trans_desc->tx_buffer) {
        free((void *)trans_buf->buffer_to_send); //force free, ignore const
    }
    //copy data from temporary DMA-capable buffer back to IRAM buffer and free the temporary one.
    if ((void *)trans_buf->buffer_to_rcv != &trans_desc->rx_data[0] &&
        trans_buf->buffer_to_rcv != trans_desc->rx_buffer) {
        if (trans_desc->flags & SPI_TRANS_USE_RXDATA) {
            memcpy((uint8_t *) & trans_desc->rx_data[0], trans_buf->buffer_to_rcv, (trans_desc->rxlength + 7) / 8);
        } else {
            memcpy(trans_desc->rx_buffer, trans_buf->buffer_to_rcv, (trans_desc->rxlength + 7) / 8);
        }
        free(trans_buf->buffer_to_rcv);
    }
}

static SPI_MASTER_ISR_ATTR esp_err_t setup_priv_desc(ESP32_SPI_transaction_t *trans_desc, ESP32_SPI_trans_priv_t* new_desc, bool isdma)
{
    *new_desc = (ESP32_SPI_trans_priv_t) { .trans = trans_desc, };

    // rx memory assign
    uint32_t* rcv_ptr;
    if ( trans_desc->flags & SPI_TRANS_USE_RXDATA ) {
        rcv_ptr = (uint32_t *)&trans_desc->rx_data[0];
    } else {
        //if not use RXDATA neither rx_buffer, buffer_to_rcv assigned to NULL
        rcv_ptr = trans_desc->rx_buffer;
    }
    if (rcv_ptr && isdma && (!esp_ptr_dma_capable(rcv_ptr) || ((int)rcv_ptr % 4 != 0))) {
        //if rxbuf in the desc not DMA-capable, malloc a new one. The rx buffer need to be length of multiples of 32 bits to avoid heap corruption.
        ESP_LOGI( SPI_TAG, "Allocate RX buffer for DMA" );
        rcv_ptr = heap_caps_malloc((trans_desc->rxlength + 31) / 8, MALLOC_CAP_DMA);
        if (rcv_ptr == NULL) goto clean_up;
    }
    new_desc->buffer_to_rcv = rcv_ptr;

    // tx memory assign
    const uint32_t *send_ptr;
    if ( trans_desc->flags & SPI_TRANS_USE_TXDATA ) {
        send_ptr = (uint32_t *)&trans_desc->tx_data[0];
    } else {
        //if not use TXDATA neither tx_buffer, tx data assigned to NULL
        send_ptr = trans_desc->tx_buffer ;
    }
    if (send_ptr && isdma && !esp_ptr_dma_capable( send_ptr )) {
        //if txbuf in the desc not DMA-capable, malloc a new one
        ESP_LOGI( SPI_TAG, "Allocate TX buffer for DMA" );
        uint32_t *temp = heap_caps_malloc((trans_desc->length + 7) / 8, MALLOC_CAP_DMA);
        if (temp == NULL) goto clean_up;

        memcpy( temp, send_ptr, (trans_desc->length + 7) / 8 );
        send_ptr = temp;
        }
    new_desc->buffer_to_send = send_ptr;

    return ESP_OK;

clean_up:
    uninstall_priv_desc(new_desc);
    return ESP_ERR_NO_MEM;
}

esp_err_t SPI_MASTER_ATTR ESP32_SPI_device_queue_trans(ESP32_SPI_device_handle_t handle, ESP32_SPI_transaction_t *trans_desc,  TickType_t ticks_to_wait)
{
    esp_err_t ret = check_trans_valid(handle, trans_desc);
    if (ret != ESP_OK) return ret;

    ESP32_SPI_host_t *host = handle->host;

    SPI_CHECK( !device_is_polling(handle), "Cannot queue new transaction while previous polling transaction is not terminated.", ESP_ERR_INVALID_STATE );

    ESP32_SPI_trans_priv_t trans_buf;
    ret = setup_priv_desc(trans_desc, &trans_buf, (host->dma_chan!=0));
    if (ret != ESP_OK) return ret;

#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(host->pm_lock);
#endif
    //Send to queue and invoke the ISR.

    BaseType_t r = xQueueSend(handle->trans_queue, (void *)&trans_buf, ticks_to_wait);
    if (!r) {
        ret = ESP_ERR_TIMEOUT;
#ifdef CONFIG_PM_ENABLE
        //Release APB frequency lock
        esp_pm_lock_release(host->pm_lock);
#endif
        goto clean_up;
    }
    spi_isr_invoke(handle);
    return ESP_OK;

clean_up:
    uninstall_priv_desc(&trans_buf);
    return ret;
}

esp_err_t SPI_MASTER_ATTR ESP32_SPI_device_get_trans_result(ESP32_SPI_device_handle_t handle, ESP32_SPI_transaction_t **trans_desc, TickType_t ticks_to_wait)
{
    BaseType_t r;
    ESP32_SPI_trans_priv_t trans_buf;
    SPI_CHECK(handle!=NULL, "invalid dev handle", ESP_ERR_INVALID_ARG);

    //use the interrupt, block until return
    r=xQueueReceive(handle->ret_queue, (void*)&trans_buf, ticks_to_wait);
    if (!r) {
        // The memory occupied by rx and tx DMA buffer destroyed only when receiving from the queue (transaction finished).
        // If timeout, wait and retry.
        // Every in-flight transaction request occupies internal memory as DMA buffer if needed.
        return ESP_ERR_TIMEOUT;
    }
    //release temporary buffers
    uninstall_priv_desc(&trans_buf);
    (*trans_desc) = trans_buf.trans;

    return ESP_OK;
}

//Porcelain to do one blocking transmission.
esp_err_t SPI_MASTER_ATTR ESP32_SPI_device_transmit(ESP32_SPI_device_handle_t handle, ESP32_SPI_transaction_t *trans_desc)
{
    esp_err_t ret;
    ESP32_SPI_transaction_t *ret_trans;
    //ToDo: check if any spi transfers in flight
    ret = ESP32_SPI_device_queue_trans(handle, trans_desc, portMAX_DELAY);
    if (ret != ESP_OK) return ret;

    ret = ESP32_SPI_device_get_trans_result(handle, &ret_trans, portMAX_DELAY);
    if (ret != ESP_OK) return ret;

    assert(ret_trans == trans_desc);
    return ESP_OK;
}

esp_err_t SPI_MASTER_ATTR 
ESP32_SPI_device_acquire_bus(ESP32_SPI_device_t *device, TickType_t wait)
{
    ESP32_SPI_host_t *const host = device->host;
    SPI_CHECK(wait==portMAX_DELAY, "acquire finite time not supported now.", ESP_ERR_INVALID_ARG);
    SPI_CHECK( !device_is_polling(device), "Cannot acquire bus when a polling transaction is in progress.", ESP_ERR_INVALID_STATE );

    esp_err_t ret = device_acquire_bus_internal(device, portMAX_DELAY);
    if (ret != ESP_OK) return ret;
    ret = device_wait_for_isr_idle(device, portMAX_DELAY);
    if (ret != ESP_OK) return ret;

    device->host->bus_locked = true;

    ESP_LOGD(SPI_TAG, "device%d acquired the bus", device->id);

#ifdef CONFIG_PM_ENABLE
    // though we don't suggest to block the task before ``release_bus``, still allow doing so.
    // this keeps the spi clock at 80MHz even if all tasks are blocked
    esp_pm_lock_acquire(device->host->pm_lock);
#endif
    //configure the device so that we don't need to do it again in the following transactions
    ESP32_SPI_setup_device( host, device->id );
    //the DMA is also occupied by the device, all the slave devices that using DMA should wait until bus released.
    if (host->dma_chan != 0) {
        ESP32_SPI_common_dmaworkaround_transfer_active(host->dma_chan);
    }
    return ESP_OK;
}

// This function restore configurations required in the non-polling mode
void SPI_MASTER_ATTR ESP32_SPI_device_release_bus(ESP32_SPI_device_t *dev)
{
    ESP32_SPI_host_t *host = dev->host;

    if (device_is_polling(dev)){
        ESP_LOGE(SPI_TAG, "Cannot release bus when a polling transaction is in progress.");
        assert(0);
    }

    if (host->dma_chan != 0) {
        ESP32_SPI_common_dmaworkaround_idle(host->dma_chan);
    }
    //Tell common code DMA workaround that our DMA channel is idle. If needed, the code will do a DMA reset.

    //allow clock to be lower than 80MHz when all tasks blocked
#ifdef CONFIG_PM_ENABLE
    //Release APB frequency lock
    esp_pm_lock_release(host->pm_lock);
#endif
    ESP_LOGD(SPI_TAG, "device%d release bus", dev->id);

    dev->host->bus_locked = false;
    device_release_bus_internal(dev->host);
}



esp_err_t SPI_MASTER_ISR_ATTR 
ESP32_SPI_device_polling_start(ESP32_SPI_device_handle_t handle,
	ESP32_SPI_transaction_t *trans_desc,
	TickType_t ticks_to_wait)
{
  esp_err_t ret;
  SPI_CHECK(ticks_to_wait == portMAX_DELAY, "currently timeout is not available for polling transactions", ESP_ERR_INVALID_ARG);

  ESP32_SPI_host_t *host = handle->host;

  ret = check_trans_valid(handle, trans_desc);

  if (ret != ESP_OK) return ret;

  SPI_CHECK( !device_is_polling(handle), "Cannot send polling transaction while the previous polling transaction is not terminated.", ESP_ERR_INVALID_STATE );

  ret = setup_priv_desc(trans_desc, &host->cur_trans_buf, (handle->host->dma_chan!=0));
  if (ret!=ESP_OK) return ret;

  device_acquire_bus_internal(handle, portMAX_DELAY);

  device_wait_for_isr_idle(handle, portMAX_DELAY);

  assert(atomic_load(&host->acquire_cs) == handle->id);
  assert(host->isr_free);

  // Polling, no interrupt is used.
  host->polling = true;

  ESP_LOGV(SPI_TAG, "polling trans");

  spi_new_trans(handle, &host->cur_trans_buf);

  return ESP_OK;
}



esp_err_t SPI_MASTER_ISR_ATTR 
ESP32_SPI_device_polling_end(ESP32_SPI_device_handle_t handle,
	TickType_t ticks_to_wait)
{
  SPI_CHECK(handle != NULL, "invalid dev handle", ESP_ERR_INVALID_ARG);

  ESP32_SPI_host_t *host = handle->host;

  // if (host->acquire_cs == handle->id && host->polling) {
  assert(host->cur_cs == atomic_load(&host->acquire_cs));
  TickType_t start = xTaskGetTickCount();

  while (!host->hw->slave.trans_done) {

	TickType_t end = xTaskGetTickCount();
	if (end - start > ticks_to_wait) {
		return ESP_ERR_TIMEOUT;
	}
  }

  ESP_LOGV(SPI_TAG, "polling trans done");

  // deal with the in-flight transaction
  ESP32_SPI_post_trans(host);

  // release temporary buffers
  uninstall_priv_desc(&host->cur_trans_buf);

  host->polling = false;

  if (!host->bus_locked) {

	device_release_bus_internal(host);
  }

  return ESP_OK;
}



esp_err_t SPI_MASTER_ISR_ATTR 
ESP32_SPI_device_polling_transmit(ESP32_SPI_device_handle_t handle,
	ESP32_SPI_transaction_t* trans_desc)
{
  esp_err_t ret;
  ret = ESP32_SPI_device_polling_start(handle, trans_desc, portMAX_DELAY);

  if (ret != ESP_OK) return ret;

  ret = ESP32_SPI_device_polling_end(handle, portMAX_DELAY);

  if (ret != ESP_OK) return ret;

  return ESP_OK;
}










