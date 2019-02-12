/* #################################################################################################
 *
 *  Function: List Command for SCDE (Smart Connected Device Engine)
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

#include "include/List_Command.h"



// set default build verbose - if no external override
#ifndef List_Command_DBG  
#define List_Command_DBG  5	// 5 is default
#endif



// -------------------------------------------------------------------------------------------------

// make data root locally available
static SCDERoot_t* SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for faster operation
static SCDEFn_t* SCDEFn;

// -------------------------------------------------------------------------------------------------



/* --------------------------------------------------------------------------------------------------
 *  DName: List_ProvidedByCommand
 *  Desc: Data 'Provided By Command' for this Command (functions + infos this command provides
 *        to SCDE)
 *  Data: 
 * --------------------------------------------------------------------------------------------------
 */

// Command Help
const uint8_t List_helpText[] = 
  "<name> <type> <options>";
// CommandHelp (detailed)
const uint8_t List_helpDetailText[] = 
  "list definitions and state info";

providedByCommand_t List_ProvidedByCommand =
{
   "List"					// Command-Name of command -> libfilename.so !
  ,4						// length of cmd
  ,List_InitializeCommandFn			// Initialize Fn
  ,List_CommandFn				// the Fn code
  ,&List_helpText
  ,sizeof(List_helpText)
  ,&List_helpDetailText
  ,sizeof(List_helpDetailText)
};

//(const uint8_t *) "Usage: define <name> <type> <options>, to define a device",57);	// CommandHelp, Len


/* --------------------------------------------------------------------------------------------------
 *  FName: List_InitializeCommandFn
 *  Desc: Initializion of SCDE Function Callback of an new loaded command
 *  Info: Stores Command-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
List_InitializeCommandFn(SCDERoot_t* SCDERootptr)
{
  // make data root locally available
  SCDERoot = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn = SCDERootptr->SCDEFn;

// --------------------------------------------------------------------------------------------------

  #if List_Command_DBG >= 3
  SCDEFn->Log3Fn(List_ProvidedByCommand.commandNameText
	,List_ProvidedByCommand.commandNameTextLen
	,3
	,"InitializeFn called. Now useable.");
  #endif

// --------------------------------------------------------------------------------------------------

  return 0;
}



/**
  * --------------------------------------------------------------------------------------------------
  * FName: CommandList -> gets arguments from SET 'Name Args'
  * Desc: Passes args "Args" to the SetFn of the define with name "Name"
  *
  * , and Module "TypeName" and calls Modules SetFn with
  *       args "Args"
  * Info: SET 'Name' 'Args'
  *       'Name' is a valid definition name [azAZ09._] char[31]
  *       'Args' are passed to modules SetFn for processing
  * Para: char* Args -> prt to space seperated txt string "Name TypeName Definition..."
  * Rets: char* -> error-text-string in allocated mem, or NULL=OK
  * --------------------------------------------------------------------------------------------------
  **/

/* --------------------------------------------------------------------------------------------------
 *  FName: List_Command
 *  Desc: Creates a new common define with name "Name", and Module "Type-Name", prevills some common
 *        values and calls Modules DefFn with for further module-specific initialization.
 *  Info: 'Name' is custom definition name. Allowed: [azAZ09._], uint8_t[32]
 *        'Type-Name' is module name
 *        'Definition ...' is custom and passed to modules DefineFn, stored in Definition->Definition
 *  Para: const uint8_t *args  -> prt to space seperated define text string "Name Type-Name Definition ..."
 *        const size_t argsLen -> length of args
 *  Rets: struct headRetMsgMultiple_s -> STAILQ head of multiple retMsg, if NULL -> no retMsg-entry
 * --------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s
List_CommandFn (const uint8_t *args
		,const size_t argsLen)
{
  #if List_Command_DBG >= 7
  SCDEFn->Log3Fn(List_ProvidedByCommand.commandNameText
	,List_ProvidedByCommand.commandNameTextLen
	,7
	,"CommandFn called with args '%.*s'"
	,argsLen
	,args);
  #endif

// --------------------------------------------------------------------------------------------------

  // prepare STAILQ head for multiple RetMsg storage
  struct headRetMsgMultiple_s headRetMsgMultiple;

  // Initialize the queue
  STAILQ_INIT(&headRetMsgMultiple);

  // set start of possible List-Arguments
  const uint8_t *listArgs = args;

  // a seek-counter
  int i = 0;

  // seek to start position of List-Arguments
  while( (i < argsLen) && (*listArgs == ' ') ) { i++ ; listArgs++ ; }

   // length of List-Arguments
  size_t listArgsLen = argsLen - i;

// FILTER SPACES AFTER ARG!!!!!!!!!!!!!!!

// --------------------------------------------------------------------------------------------------

  // No arguments given - answer with default type-defines-summary
  if (listArgsLen == 0) {

	// build the default return message
	size_t strBufferLen = 0;
	size_t strBufferOffset; //
	strTextMultiple_t *retMsg = NULL;

	// set write buffer start
	char *wBufStart = NULL;

	do {

		// do we have already a requested-buffer-length, then alloc
		if (strBufferLen) {

			// alloc mem for retMsg
			retMsg = malloc(sizeof(strTextMultiple_t));

			// the buffer for the answer
			retMsg->strText = malloc(strBufferLen);
			wBufStart = (char *) retMsg->strText;
			retMsg->strTextLen = strBufferLen;

		}

		// reset write buffer counter
		strBufferOffset = 0;

		// start building intro
		strBufferOffset += snprintf(wBufStart + strBufferOffset
			,strBufferLen
			,"Summary of defined Devices of each Type:\r\n");

		// loop through  currently stored modules
		Module_t *Module;

		STAILQ_FOREACH(Module, &SCDERoot->HeadModules, entries) {

			int count = 0;

			// get the Common_Definitions for current module
			Common_Definition_t *Common_Definition;

			STAILQ_FOREACH(Common_Definition, &SCDERoot->HeadCommon_Definitions, entries) {

				if (Common_Definition->module == Module) {

					count++;

					// Module summary headline ?
					if (count == 1) {

						strBufferOffset += snprintf(wBufStart + strBufferOffset
							,strBufferLen
							,"\r\nType-Name: %.*s\r\n"
							,Module->ProvidedByModule->typeNameLen
							,Module->ProvidedByModule->typeName);

						strBufferOffset += snprintf(wBufStart + strBufferOffset
							,strBufferLen
							,"Cnt |               Name               | (State)\r\n");

					}

					strBufferOffset += snprintf(wBufStart + strBufferOffset
						,strBufferLen
						,"%03d | %.*s%.*s | ("
						,count
						,Common_Definition->nameLen
						,Common_Definition->name
						,32-Common_Definition->nameLen
						,"                                ");

					// the state
					if ( Common_Definition->state) {

						// we have a state
						strBufferOffset += snprintf(wBufStart + strBufferOffset
						,strBufferLen
						,"%.*s)\r\n"
						,Common_Definition->stateLen
						,Common_Definition->state);

					}

					else {

						// no state
						strBufferOffset += snprintf(wBufStart + strBufferOffset
							,strBufferLen
							,"?)\r\n");

					}
				}
			}
		}

		// building end-info
		strBufferOffset += snprintf(wBufStart + strBufferOffset
			,strBufferLen
			,"\r\nType list <name> for a detailed info.");


		// store reqired len
		strBufferLen = strBufferOffset;

	} while (!retMsg);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;

  }

// --------------------------------------------------------------------------------------------------

  // arguments given - should be the definition-name
  else {

	// search for the Common_Definition for the requested definition-name
	Common_Definition_t *Common_Definition = STAILQ_FIRST(&SCDERoot->HeadCommon_Definitions);
	while (Common_Definition != NULL) {

		if ( (Common_Definition->nameLen == listArgsLen)
			&& (!strncasecmp((const char*) Common_Definition->name, (const char*) listArgs, listArgsLen)) ) {

			// found, break and keep prt
			break;

		}

		// get next Common_Definition for processing
		Common_Definition = STAILQ_NEXT(Common_Definition, entries);

	}

	// Common_Definition for the requested definition-name not found
	if (Common_Definition == NULL) {

	// alloc mem for retMsg
	strTextMultiple_t *retMsg =
		 malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
			,"Error, could not find definition: %.*s!\r\n"
			,argsLen
			,args);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;

	}

	// Common_Definition for the requested definition-name found
	else {

		// build the default return message
		size_t strBufferLen = 0;
		size_t strBufferOffset; //
		strTextMultiple_t *retMsg = NULL;

		// set write buffer start
		char *wBufStart = NULL;

		do {

			// do we have already a requested-buffer-length, then alloc
			if (strBufferLen) {

				// alloc mem for retMsg
				retMsg = malloc(sizeof(strTextMultiple_t));

				// the buffer for the answer
				retMsg->strText = malloc(strBufferLen);
				wBufStart = (char *) retMsg->strText;
				retMsg->strTextLen = strBufferLen;

			}

			// reset write buffer counter
			strBufferOffset = 0;


/*
 int info = 0;
 while () {

 if ( (SCDEFn->GetCommonTextFn(Common_Definition, ++info, &strBufferOffset, wBufStart)) == true) break;

  }
*/

			// start building intro
			strBufferOffset += snprintf(wBufStart + strBufferOffset
				,strBufferLen
				,"Common:\r\n");

			// add DEF
			strBufferOffset += snprintf(wBufStart + strBufferOffset
				,strBufferLen
				,"  DEF%.*s %.*s\r\n"
				,10-3 //DEF LEN
				,"                                "
				,Common_Definition->definitionLen
				,Common_Definition->definition);

			// add NAME
			strBufferOffset += snprintf(wBufStart + strBufferOffset
				,strBufferLen
				,"  NAME%.*s %.*s\r\n"
				,10-4 //NAME LEN
				,"                                "
				,Common_Definition->nameLen
				,Common_Definition->name);

			// add NR
			strBufferOffset += snprintf(wBufStart + strBufferOffset
				,strBufferLen
				,"  NR%.*s %d\r\n"
				,10-2 //NR LEN
				,"                                "
				,Common_Definition->nr);

			// add Readings
			strBufferOffset += snprintf(wBufStart + strBufferOffset
				,strBufferLen
				,"  Readings:\r\n");
			
			// list readings stored for definition after processing
			xReadingsSLTQE_t *currentReadingsSLTQE;
			STAILQ_FOREACH(currentReadingsSLTQE, &Common_Definition->headReadings, entries) {

				// get tist text
				strText_t strText =
					SCDEFn->FmtDateTimeFn(currentReadingsSLTQE->readingTist);

					strBufferOffset += snprintf(wBufStart + strBufferOffset
						,strBufferLen
						,"  %.*s | %.*s = %.*s\r\n"
						,strText.strTextLen
						,strText.strText
						,currentReadingsSLTQE->nameString.length
						,currentReadingsSLTQE->nameString.characters
						,currentReadingsSLTQE->valueString.length
						,currentReadingsSLTQE->valueString.characters);

				free(strText.strText);

			}

			// add STATE
			strBufferOffset += snprintf(wBufStart + strBufferOffset
				,strBufferLen
				,"  STATE%.*s %.*s\r\n"
				,10-4 //STATE LEN
				,"                                "
				,Common_Definition->stateLen
				,Common_Definition->state);

			// add TYPE
			strBufferOffset += snprintf(wBufStart + strBufferOffset
				,strBufferLen
				,"  TYPE%.*s %.*s\r\n"
				,10-4 //TYPE LEN
				,"                                "
				,Common_Definition->module->ProvidedByModule->typeNameLen
				,Common_Definition->module->ProvidedByModule->typeName);


			// add Attributes
			strBufferOffset += snprintf(wBufStart + strBufferOffset
				,strBufferLen
				,"Attributes:\r\n");
			
			// list attributes stored for definition after processing
			attribute_t *attribute;
			STAILQ_FOREACH(attribute, &SCDERoot->headAttributes, entries) {

				// get tist text
				if (attribute->attrAssignedDef == Common_Definition) {

					strBufferOffset += snprintf(wBufStart + strBufferOffset
						,strBufferLen
						,"  %.*s = %.*s\r\n"
						,attribute->attrNameTextLen
						,attribute->attrNameText
						,attribute->attrValTextLen
						,attribute->attrValText);
				}

			}




/* demo
Internals: 
   Caps       15 
   DEF        Aussenbereich.IoT.Relais 
   DevName    Aussenbereich.IoT.Relais 
   IODev      HATPD
   LASTInputDev HATPD_172.17.0.103_47450 
   MSGCNT     14 
   NAME       ADC.1_A69E71 
   NR         829 
   STATE      OK 
   TYPE       EcSUHA_ADC 
   lastReceive 2016-11-28 06:58:34 
   Readings: 
     2016-11-28 06:58:34   state           OK 
Attributes: 
   DevicePW   Dev 
   IODev      HATPD 
   UFID       A69E71-ADC-0001 */


		// building end-info
		strBufferOffset += snprintf(wBufStart + strBufferOffset
			,strBufferLen
			,"\r\nOnly the Common-Definition is currently shown!");


			// store reqired len
			strBufferLen = strBufferOffset;

		} while (!retMsg);


	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;

	}

  }

}



