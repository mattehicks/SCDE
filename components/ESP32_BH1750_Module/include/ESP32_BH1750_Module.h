// SCDE-Module ESP32_BH1750 (BH1750 Module, connected via I2C, communication via IO-Device for I2C Master communication for I2C-hardware)

#ifndef ESP32_BH1750_MODULE_H
#define ESP32_BH1750_MODULE_H



#include "SCDE_s.h"
#include "WebIf_Module global types.h"



// 
enum ESP32_BH1750_QueryStates {
    s_ESP32_INTIAL_DELAY = 0		// #00 default at start
  , s_ESP32_BH1750_SET_MODE		// #01
  , s_ESP32_BH1750_WAIT_24MS		// #02
  , s_ESP32_BH1750_READ_DATA		// #03
};



/* 
 * ESP32_BH1750_Definition stores values for operation valid only for the defined instance of an
 * loaded module. Values are initialized by HCTRLD an the loaded module itself.
 */
typedef struct ESP32_BH1750_Definition_s {

  Common_Definition_t common;		// ... the common part of the definition

  WebIf_Provided_t WebIf_Provided;	// provided data for WebIf

  size_t stage1definitionNameLen;	// Stange 1 name assigned by def
  uint8_t *stage1definitionName;	// Stange 1 nameLen assigned by def

  uint8_t BH1750_QueryState;		// enum BH1750QueryState -> state machine state for sensor data query

  uint8_t sensor_data_h, sensor_data_l; // sensor data coming from I2C

  } ESP32_BH1750_Definition_t;



/*
 *  Functions provided to SCDE by Module - for type operation (A-Z)
 */
strTextMultiple_t* ESP32_BH1750_Attribute(Common_Definition_t* Common_Definition, const uint8_t *attrCmdText, const size_t attrCmdTextLen, const uint8_t *attrNameText, const size_t attrNameTextLen, uint8_t **attrValTextPtr, size_t *attrValTextLenPtr);

strTextMultiple_t* ESP32_BH1750_Define(Common_Definition_t *Common_Definition);

int                ESP32_BH1750_IdleCb(Common_Definition_t *Common_Definition);

int                ESP32_BH1750_Initialize(SCDERoot_t* SCDERoot);

strTextMultiple_t* ESP32_BH1750_Set(Common_Definition_t* Common_Definition, uint8_t *setArgs, size_t setArgsLen);

strTextMultiple_t* ESP32_BH1750_Shutdown(Common_Definition_t* Common_Definition);

strTextMultiple_t* ESP32_BH1750_Undefine(Common_Definition_t* Common_Definition);



/*
 *  helpers provided to module for type operation
 */
bool ESP32_BH1750_ProcessKVInputArgs(ESP32_BH1750_Definition_t* ESP32_BH1750_Definition, parsedKVInputArgs_t *parsedKVInput, uint8_t *argsText, size_t argsTextLen);
bool ESP32_BH1750_SetAffectedReadings(ESP32_BH1750_Definition_t* ESP32_BH1750_Definition, uint64_t affectedReadings);



#endif /*ESP32_BH1750_MODULE_H*/
