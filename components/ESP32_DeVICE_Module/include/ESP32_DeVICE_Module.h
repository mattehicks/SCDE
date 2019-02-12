﻿// SCDE-Module ESP32_DeVICE

#ifndef ESP32_DEVICE_MODULE_H
#define ESP32_DEVICE_MODULE_H



#include "SCDE_s.h"


#include "WebIf_Module global types.h"



/* 
 * ESP32_DeVICE_Definition stores values for operation valid only for the defined instance of an
 * loaded module. Values are initialized by HCTRLD an the loaded module itself.
 */
typedef struct ESP32_DeVICE_Definition_s {

  Common_Definition_t common;		// ... the common part of the definition

  WebIf_Provided_t WebIf_Provided;	// provided data for WebIf

  uint32_t demo;			// custom data

} ESP32_DeVICE_Definition_t;



/*
 *  Functions provided to SCDE by Module - for type operation (A-Z)
 */
strTextMultiple_t* ESP32_DeVICE_Attribute(Common_Definition_t* Common_Definition, const uint8_t *attrCmdText, const size_t attrCmdTextLen, const uint8_t *attrNameText, const size_t attrNameTextLen, uint8_t **attrValTextPtr, size_t *attrValTextLenPtr);

strTextMultiple_t* ESP32_DeVICE_Define(Common_Definition_t *Common_Definition);

int ESP32_DeVICE_Initialize(SCDERoot_t* SCDERoot);

strTextMultiple_t* ESP32_DeVICE_Rename(Common_Definition_t *Common_Definition, uint8_t *newName, size_t newNameLen, uint8_t *oldName, size_t oldNameLen);

strTextMultiple_t* ESP32_DeVICE_Set(Common_Definition_t* Common_Definition, uint8_t *setArgs, size_t setArgsLen);

strTextMultiple_t* ESP32_DeVICE_Shutdown(Common_Definition_t* Common_Definition);

xMultipleStringSLTQE_t* ESP32_DeVICE_State(Common_Definition_t *Common_Definition, const time_t stateTiSt, const xString_t stateNameString, const xString_t stateValueString, const xString_t stateMimeString);

strTextMultiple_t* ESP32_DeVICE_Undefine(Common_Definition_t* Common_Definition);




/*
 *  helpers provided to module for type operation
 */
bool ESP32_DeVICE_ProcessKVInputArgs(ESP32_DeVICE_Definition_t* ESP32_DeVICE_Definition, parsedKVInputArgs_t *parsedKVInput, uint8_t *argsText, size_t argsTextLen);
bool ESP32_DeVICE_SetAffectedReadings(ESP32_DeVICE_Definition_t* ESP32_DeVICE_Definition, uint64_t affectedReadings);



#endif /*ESP32_DEVICE_MODULE_H*/