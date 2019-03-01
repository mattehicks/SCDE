// SCDE-Module Global

#ifndef GLOBAL_MODULE_H
#define GLOBAL_MODULE_H

// -------------------------------------------------------------------------------------------------

// this Module is made for the Smart-Connected-Device-Engine
#include "SCDE_s.h"

// this Module provides functions for other Modules:
// -- no ---

// this Module uses an 1st stage:
// -- no ---

// -------------------------------------------------------------------------------------------------

// stores the Root Data of the Smart Connected Devices Engine - at/for this Module
SCDERoot_t* SCDERoot_at_Global_M;

// stores SCDEFn (Functions / callbacks) provided for operation - at/for this Module
SCDEFn_t* SCDEFn_at_Global_M;

// -------------------------------------------------------------------------------------------------



/* 
 * Global_Definition stores values for operation valid only for the defined instance of an
 * loaded module. Values are initialized by HCTRLD an the loaded module itself.
 */
typedef struct Global_Definition_s {

  Common_Definition_t common;		// ... the common part of the definition

//  WebIf_Provided_t WebIf_Provided;	// provided data for WebIf

//  uint32_t demo;			// custom data

  } Global_Definition_t;



/*
 *  Functions provided to SCDE by Module - for type operation (A-Z)
 */
strTextMultiple_t* Global_Define(Common_Definition_t *Common_Definition);

int Global_Initialize(SCDERoot_t* SCDERoot);



/*
 *  helpers
 */
//void ESP32Control_HexDumpOut (char *desc, void *addr, int len);



#endif /*GLOBAL_MODULE_H*/
