// SCDE-Module ESP32_SPI_Module (IO-Device for SPI-Hardware)

#ifndef ESP32_SPI_MODULE_H
#define ESP32_SPI_MODULE_H

// -------------------------------------------------------------------------------------------------

// this Module is made for the Smart-Connected-Device-Engine
#include "SCDE_s.h"

// this Module provides functions for other Modules:
#include "WebIf_Module global types.h"
//#include "WebIf_Module_s.h"

// this Module uses an 1st stage:
// -- no ---

// -------------------------------------------------------------------------------------------------

// stores the Root Data of the Smart Connected Devices Engine - at/for this Module
SCDERoot_t* SCDERoot_at_ESP32_SPI_M;

// stores SCDEFn (Functions / callbacks) provided for operation - at/for this Module
SCDEFn_t* SCDEFn_at_ESP32_SPI_M;

// -------------------------------------------------------------------------------------------------























/*
 *  Functions provided to SCDE by Module - for type operation (A-Z)
 */
strTextMultiple_t* ESP32_SPI_Attribute(Common_Definition_t* Common_Definition, const uint8_t *attrCmdText, const size_t attrCmdTextLen, const uint8_t *attrNameText, const size_t attrNameTextLen, uint8_t **attrValTextPtr, size_t *attrValTextLenPtr);

strTextMultiple_t* ESP32_SPI_Define(Common_Definition_t *Common_Definition);

//strTextMultiple_t* ESP32_SPI_DirectWrite(Common_Definition_t *Common_Definition_Stage1, Common_Definition_t *Common_Definition_Stage2, Common_StageXCHG_t *Common_StageXCHG);

int ESP32_SPI_IdleCb(Common_Definition_t *Common_Definition);

int ESP32_SPI_Initialize(SCDERoot_t* SCDERoot);

strTextMultiple_t* ESP32_SPI_Set(Common_Definition_t* Common_Definition, uint8_t *setArgs, size_t setArgsLen);

strTextMultiple_t* ESP32_SPI_Shutdown(Common_Definition_t* Common_Definition);

strTextMultiple_t* ESP32_SPI_Undefine(Common_Definition_t* Common_Definition);

xMultipleStringSLTQE_t* ESP32_SPI_Write(Common_Definition_t *Common_Definition, xString_t data);

// -------------------------------------------------------------------------------------------------





/*
 *  helpers provided to module for type operation
 */
strTextMultiple_t* ESP32_SPI_ProcessKVInputArgs(ESP32_SPI_Definition_t* ESP32_SPI_Definition, parsedKVInputArgs_t *parsedKVInput, uint8_t *argsText, size_t argsTextLen);

bool ESP32_SPI_SetAffectedReadings(ESP32_SPI_Definition_t* ESP32_SPI_Definition, uint64_t affectedReadings);



#endif /*ESP32_SPI_MODULE_H*/


