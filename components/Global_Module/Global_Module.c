﻿/* #################################################################################################
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



// the Smart-Connected-Device-Engine structures & types ..
#include <SCDE_s.h>

// provides WebIf, need the structures & types ...
#include "WebIf_Module.h"

// this Modules structures & types ...
#include "Global_Module.h"



// -------------------------------------------------------------------------------------------------

// developers debug information 0 = off 5 max?
#define SCDED_DBG 5
#define SCDEH_DBG 5



/**
 * -------------------------------------------------------------------------------------------------
 *  DName: Global_Module
 *  Desc: Data 'Provided By Module' for the Global module (functions + infos this module provides
 *        to SCDE)
 *  Data: 
 * -------------------------------------------------------------------------------------------------
 */
ProvidedByModule_t Global_ProvidedByModule = { // A-Z order
  "Global"				// Type-Name of module -> on Linux libfilename.so !
  ,6					// size of Type-Name

  ,NULL					// Add
  ,NULL					// Attribute
  ,Global_Define			// Define
  ,NULL					// Delete
  ,NULL					// DirectRead
  ,NULL					// DirectWrite
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
  ,NULL					// Write
  ,NULL					// FnProvided
  ,sizeof(Global_Definition_t)		// Modul specific Size (Common_Definition_t + X)
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
  SCDERoot_at_Global_M = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn_at_Global_M = SCDERootptr->SCDEFn;

  SCDEFn_at_Global_M->Log3Fn(Global_ProvidedByModule.typeName
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
	NULL;//(WebIf_FnProvided_t *) SCDEFn_at_Global_M->GetFnProvidedByModule("WebIf");

 // Providing data for WebIf? Initialise data provided for WebIf
  if (Global_Definition->WebIf_Provided.WebIf_FnProvided) {

	Global_Definition->WebIf_Provided.ActiveResourcesDataA =
		(WebIf_ActiveResourcesDataA_t *) &ESP32Control_ActiveResourcesDataA_forWebIf;

	ESP32Control_Definition->WebIf_Provided.ActiveResourcesDataB =
		(WebIf_ActiveResourcesDataB_t *) &ESP32Control_ActiveResourcesDataB_forWebIf;

	}

  else	{

	SCDEFn_at_Global_M->Log3Fn(Common_Definition->name
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
//  SCDEFn_at_Global_M->AnalyzeCommandFn((const uint8_t *) "attr global verbose 3", 21);
//  SCDEFn_at_Global_M->AnalyzeCommandFn((const uint8_t *) "attr global logfile -", 21);


  // for response msg from AnalyzeCommand
  struct headRetMsgMultiple_s headRetMsgMultiple;

  headRetMsgMultiple = SCDEFn_at_Global_M->AnalyzeCommandFn((const uint8_t *) "attr global configfile maker", 28);

 // retMsgMultiple stailq filled ?
  if (!STAILQ_EMPTY(&headRetMsgMultiple)) {

	// for the retMsg elements
	strTextMultiple_t *retMsg;

	// get the entries till empty
	while (!STAILQ_EMPTY(&headRetMsgMultiple)) {

		retMsg = STAILQ_FIRST(&headRetMsgMultiple);
/*
		Log3( (const uint8_t*)"xxx"
			 ,sizeof("xxx")
			 ,1
			 ,"result is:%.*s\n"
			 ,retMsg->strTextLen
			,retMsg->strText);
*/ printf("an error occurws!!!!!!!!!!!!!!!!!!!1");
		// done, remove this entry
		STAILQ_REMOVE(&headRetMsgMultiple, retMsg, strTextMultiple_s, entries);

		// free the msg-string
		free(retMsg->strText);

		// and the strTextMultiple_t
		free(retMsg);
	}
  }











//  SCDEFn_at_Global_M->AnalyzeCommandFn((const uint8_t *) "attr global statefile state", 27);
//  SCDEFn_at_Global_M->AnalyzeCommandFn((const uint8_t *) "attr global port 1234", 21);
                                                       //|12
// -------------------------------------------------------------------------------------------

  return retMsg;

  }







