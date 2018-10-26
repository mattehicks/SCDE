/* #################################################################################################
 *
 *  Function: Global Module for SCDE (Smart Connected Device Engine)
 *
 *  ESP 8266EX & ESP32 SOC Activities ...
 *  Copyright by EcSUHA
 *
 *  Created by Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany for EcSUHA.de 
 *
 *  MSchulze780@GMAIL.COM
 *  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
 * #################################################################################################
 */



#include <ProjectConfig.h>
#include <esp8266.h>
#include <Platform.h>

#include <SCDE_s.h>

#include "Global_Module.h"

#include "SCDE_Main.h"




static char tag[] = "Global";




// -------------------------------------------------------------------------------------------------

// developers debug information 0 = off 5 max?
#define SCDED_DBG 5
#define SCDEH_DBG 5

// -------------------------------------------------------------------------------------------------

// make data root locally available
static SCDERoot_t* SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for operation
static SCDEFn_t* SCDEFn;

// -------------------------------------------------------------------------------------------------



/**
 * -------------------------------------------------------------------------------------------------
 *  DName: Global_Module
 *  Desc: Data 'Provided By Module' for the Global module (functions + infos this module provides
 *        to SCDE)
 *  Data: 
 * -------------------------------------------------------------------------------------------------
 */
ProvidedByModule_t Global_ProvidedByModule =
  { // A-Z order

  "Global"				// Type-Name of module -> should be same name as libfilename.so !
  ,6					// size of Type-Name

  ,NULL					// Add

  ,NULL					// Attribute

  ,Global_Define			// Define

  ,NULL					// Delete

  ,NULL					// Except

  ,NULL					// Get

  ,NULL					// IdleCb

  ,Global_Initialize			// Initialize

  ,NULL					// Notify

  ,NULL					// Parse

  ,NULL					// Read

  ,NULL					// Ready

  ,NULL					// Rename

  ,NULL					// Set

  ,NULL					// Shutdown

  ,NULL					// State

  ,NULL					// Sub

  ,NULL					// Undefine

  ,NULL					// DirectRead

  ,NULL					// DirectWrite

//  ,NULL		 		// FnProvided

  ,sizeof(Global_Definition_t)		// Size of modul specific definition structure (Common_Definition_t + X)

  };



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: Global_Initialize
 *  Desc: Initializion of SCDE Function Callbacks of an new loaded module
 *  Info: Stores Module-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
Global_Initialize(SCDERoot_t* SCDERootptr)
  {

  // make data root locally available
  SCDERoot = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn = SCDERootptr->SCDEFn;

  SCDEFn->Log3Fn(Global_ProvidedByModule.typeName
		  ,Global_ProvidedByModule.typeNameLen
		  ,3
		  ,"InitializeFn called. Type '%.*s' now useable.\n"
		  ,Global_ProvidedByModule.typeNameLen
		  ,Global_ProvidedByModule.typeName);

  return 0;

  }



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: Global_Define
 *  Desc: Finalizes the defines a new "device" of 'Global' type. Contains devicespecific init code.
 *  Info: 
 *  Para: Common_Definition_t *Common_Definition -> prefilled ESP32Control Definition
 *        char *Definition -> the last part of the CommandDefine arg* 
 *  Rets: strTextMultiple_t* -> response text NULL=no text
 * -------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* //ICACHE_FLASH_ATTR 
Global_Define(Common_Definition_t *Common_Definition)//, const char *Definition)
  {

  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // make common ptr to modul specific ptr
  Global_Definition_t* Global_Definition =
		  (Global_Definition_t*) Common_Definition;

  #if SCDEH_DBG >= 5
  printf("\n|Global_Def, Def:%.*s>"
	,Global_Definition->common.definitionLen
	,Global_Definition->common.definition);
  #endif

// -------------------------------------------------------------------------------------------

  // store FD to Definition. Will than be processed in global loop ... -> THIS MODULE USES NO FD
  Global_Definition->common.fd = -1;

// -------------------------------------------------------------------------------------------

  // init WebIf_Provided offset
//  ESP32Control_Definition->common.link =
//	&ESP32Control_Definition->WebIf_Provided;
/*
  // check for loaded Module 'WebIf' -> get provided Fn
  Global_Definition->WebIf_Provided.WebIf_FnProvided =
	NULL;//(WebIf_FnProvided_t *) SCDEFn->GetFnProvidedByModule("WebIf");

 // Providing data for WebIf? Initialise data provided for WebIf
  if (Global_Definition->WebIf_Provided.WebIf_FnProvided) {

	Global_Definition->WebIf_Provided.ActiveResourcesDataA =
		(WebIf_ActiveResourcesDataA_t *) &ESP32Control_ActiveResourcesDataA_forWebIf;

	ESP32Control_Definition->WebIf_Provided.ActiveResourcesDataB =
		(WebIf_ActiveResourcesDataB_t *) &ESP32Control_ActiveResourcesDataB_forWebIf;

	}

  else	{

	SCDEFn->Log3Fn(Common_Definition->name
		,Common_Definition->nameLen
		,1
		,"Could not enable WebIf support for '%.*s'. Type '%.*s' detects Type 'WebIf' is NOT loaded!"
		,Global_Definition->common.nameLen
		,Global_Definition->common.name
		,Global_Definition->common.module->ProvidedByModule->typeNameLen
		,Global_Definition->common.module->ProvidedByModule->typeName);
	}
*/

  // there should be no return messages - we expect no return messages
//  SCDEFn->AnalyzeCommandFn((const uint8_t *) "attr global verbose 3", 21);
//  SCDEFn->AnalyzeCommandFn((const uint8_t *) "attr global logfile -", 21);

  SCDEFn->AnalyzeCommandFn((const uint8_t *) "attr global configfile maker", 28);
//  SCDEFn->AnalyzeCommandFn((const uint8_t *) "attr global statefile state", 27);
//  SCDEFn->AnalyzeCommandFn((const uint8_t *) "attr global port 1234", 21);
                                                       //|12
// -------------------------------------------------------------------------------------------

  return retMsg;

  }







