/* #################################################################################################
 *
 *  Function: Sub Command for SCDE (Smart Connected Device Engine)
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

#include "Sub_Command.h"



// set default build verbose - if no external override
#ifndef Sub_Command_DBG  
#define Sub_Command_DBG  5	// 5 is default
#endif



// --------------------------------------------------------------------------------------------------

// make data root locally available
static SCDERoot_t* SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for operation
static SCDEFn_t* SCDEFn;

// --------------------------------------------------------------------------------------------------



/**
 * --------------------------------------------------------------------------------------------------
 *  DName: Sub_Command
 *  Desc: Data 'Provided By Command' for the Sub Command (functions + infos this command provides
 *        to SCDE)
 *  Data: 
 * --------------------------------------------------------------------------------------------------
 */

// Command Help
const uint8_t Sub_helpText[] = 
  "Usage: Sub <Type-Name> <Def-Name> <Key>, to delete a key=value pair added to an device";
// CommandHelp (detailed)
const uint8_t Sub_helpDetailText[] = 
  "Usagebwrebwerb: Sub <Type-Name> <Name> <Key> <Value>, to Sub a key=value pair to an device";


ProvidedByCommand_t Sub_ProvidedByCommand = {
  "Sub",					// Command-Name of command -> libfilename.so !
  3,						// length of cmd
  Sub_InitializeCommandFn,			// Initialize Fn
  Sub_CommandFn,				// the Fn code
  { &Sub_helpText, sizeof(Sub_helpText) },
  { &Sub_helpDetailText, sizeof(Sub_helpDetailText) }
};



/**
 * --------------------------------------------------------------------------------------------------
 *  FName: Sub_Initialize
 *  Desc: Initializion of SCDE Function Callback of an new loaded command
 *  Info: Stores Command-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
Sub_InitializeCommandFn(SCDERoot_t* SCDERootptr)
{
  // make data root locally available
  SCDERoot = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn = SCDERootptr->SCDEFn;

// --------------------------------------------------------------------------------------------------

  #if Sub_Command_DBG >= 3
  SCDEFn->Log3Fn(Sub_ProvidedByCommand.commandNameText
	,Sub_ProvidedByCommand.commandNameTextLen
	,3
	,"InitializeFn called. Now useable.");
  #endif

// --------------------------------------------------------------------------------------------------

  return 0;
}



/**
 * --------------------------------------------------------------------------------------------------
 *  FName: Sub_CommandFn
 *  Desc: Subs (deletes) a Key=Value attribute pair from an device with name "Name". Allowed attributes-Keys are
 *        provided by Module "Type-Name".
 *        The Sub Command calls Modules SubFn for module-specific execution.
 *  Info: Sub <Type-Name> <Name> <Key to delete>
 *        'Name' is the custom definition name. Allowed: [azAZ09._], uint8_t[32]
 *        'Type-Name' is module name
 *        'Key' is an allowed attribute-Keys - provided by module "Type-Name"
 *  Para: const uint8_t *args  -> prt to space seperated Sub-args text "<Type-Name> <Name> <Key> <Value>"
 *        const size_t argsLen -> length of Sub-args
 *  Rets: struct headRetMsgMultiple_s -> STAILQ head of multiple retMsg, if NULL -> no retMsg-entry
 * --------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s ICACHE_FLASH_ATTR
Sub_CommandFn (const uint8_t *args
		, const size_t argsLen)
{
  #if Sub_Command_DBG >= 7
  SCDEFn->Log3Fn(Sub_ProvidedByCommand.commandNameText
	,Sub_ProvidedByCommand.commandNameTextLen
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

  // set start of possible Name
  const uint8_t *typeName = args;

  // set start of possible Type-Name
  const uint8_t *name = args;

  // a seek-counter
  int i = 0;

  // seek to next space !'\32'
  while( (i < argsLen) && (*name != ' ') ) {i++;name++;}

  // length of Type-Name
  size_t typeNameLen = i;

  // seek to start position of Name '\32'
  while( (i < argsLen) && (*name == ' ') ) {i++;name++;}

  // set start of possible Args
  const uint8_t *kArgs = name;

  // a 2nd seek-counter
  int j = 0;

  // seek to next space !'\32'
  while( (i < argsLen) && (*kArgs != ' ') ) {i++,j++;kArgs++;}

  // length of Name
  size_t nameLen = j;

  // start position of Definition
  while( (i < argsLen) && (*kArgs == ' ') ) {i++;kArgs++;}

  // length of Key-Value-Args
  size_t kArgsLen = argsLen - i;

  // veryfy lengths > 0, definition 0 allowed
  if ( (typeNameLen == 0  || (nameLen) == 0 || (kArgsLen) == 0 ) ) {

	// alloc mem for retMsg
	strTextMultiple_t *retMsg =
		 malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Could not execute command SUB! Arguments '%.*s' not sufficient! Usage: Sub <Type-Name> <Def-Name> <Key>"
		,argsLen
		,args);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;

  }

   // get the module ptr by name
   Module_t* Module = SCDEFn->GetLoadedModulePtrByNameFn(typeName
		   , typeNameLen);

  // do we need to reload Module?
  if (!Module) {

	// alloc mem for retMsg
	strTextMultiple_t *retMsg =
		 malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Could not execute command ADD with arguments '%.*s'! '%.*s' is an unknown <Type-Name>!"
		,argsLen
		,args
		,typeNameLen
		,typeName);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;

  }

 // get the Common_Definition by Name
  Common_Definition_t *Common_Definition;

  strTextMultiple_t *retMsg = NULL;

  STAILQ_FOREACH(Common_Definition, &SCDERoot->HeadCommon_Definitions, entries) {

	if ( (Common_Definition->nameLen == nameLen)
		&& (!strncasecmp((const char*) Common_Definition->name, (const char*) name, nameLen)) ) {

		if (Module->provided->SubFn) {

			retMsg = Module->provided->SubFn(Common_Definition, kArgs, kArgsLen);

			// got an error msg?
			if (retMsg) {

				// insert retMsg in stail-queue
				STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

			}

		}

		else	{

			// response with error -> Type doesnt support set Fn
			// alloc mem for retMsg
			strTextMultiple_t *retMsg =
				 malloc(sizeof(strTextMultiple_t));

			// response with error text
			retMsg->strTextLen = asprintf(&retMsg->strText
				,"Error! Could not SUB Key attributes from define '%.*s'! TYPE '%.*s' does not support it!"
				,Common_Definition->nameLen
				,Common_Definition->name
			   	,Common_Definition->module->provided->typeNameLen
				,Common_Definition->module->provided->typeName);

			// get next Common_Definition for processing
			Common_Definition = STAILQ_NEXT(Common_Definition, entries);

		}

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;

	}

  }

  // alloc mem for retMsg
//  strTextMultiple_t *retMsg =
  retMsg = malloc(sizeof(strTextMultiple_t));

  // response with error text
  retMsg->strTextLen = asprintf(&retMsg->strText
	,"Error! Could not execute command ADD! Define '%.*s' not found!"
 	,nameLen
	,name);
	
  // insert retMsg in stail-queue
  STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

  // return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
  return headRetMsgMultiple;

  }


