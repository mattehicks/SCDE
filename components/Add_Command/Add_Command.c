/* #################################################################################################
 *
 *  Function: Add Command for SCDE (Smart Connected Device Engine)
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

#include "Add_Command.h"



// set default build verbose - if no external override
#ifndef Add_Command_DBG  
#define Add_Command_DBG  5	// 5 is default
#endif 



// --------------------------------------------------------------------------------------------------

// make data root locally available
static SCDERoot_t* SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for operation
static SCDEFn_t* SCDEFn;

// --------------------------------------------------------------------------------------------------



/**
 * --------------------------------------------------------------------------------------------------
 *  DName: Add_Command
 *  Desc: Data 'Provided By Command' for the Add Command (functions + infos this command provides
 *        to SCDE)
 *  Data: 
 * --------------------------------------------------------------------------------------------------
 */

// Command Help
const uint8_t Add_helpText[] = 
  "Usage: Add <Type-Name> <Name> <Key> <Value>, to add a key=value pair to an device";
// CommandHelp (detailed)
const uint8_t Add_helpDetailText[] = 
  "Usagebwrebwerb: Add <Type-Name> <Name> <Key> <Value>, to add a key=value pair to an device";


ProvidedByCommand_t Add_ProvidedByCommand = {
  "Add",					// Command-Name of command -> libfilename.so !
  3,						// length of cmd
  Add_InitializeCommandFn,			// Initialize Fn
  Add_CommandFn,				// the Fn code
  { &Add_helpText, sizeof(Add_helpText) },
  { &Add_helpDetailText, sizeof(Add_helpDetailText) }
};



/**
 * --------------------------------------------------------------------------------------------------
 *  FName: Add_Initialize
 *  Desc: Initializion of SCDE Function Callback of an new loaded command
 *  Info: Stores Command-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
Add_InitializeCommandFn(SCDERoot_t* SCDERootptr)
  {

  // make data root locally available
  SCDERoot = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn = SCDERootptr->SCDEFn;

  SCDEFn->Log3Fn(Add_ProvidedByCommand.commandNameText
		  ,Add_ProvidedByCommand.commandNameTextLen
		  ,3
		  ,"InitializeFn called. Command '%.*s' now useable."
		  ,Add_ProvidedByCommand.commandNameTextLen
		  ,Add_ProvidedByCommand.commandNameText);

  return 0;

  }



/**
 * --------------------------------------------------------------------------------------------------
 *  FName: Add_CommandFn
 *  Desc: Adds a Key=Value pair as an attribute to an device with name "Name". Allowed attributes-Keys are
 *        provided by Module "Type-Name".
 *        The Add Command calls Modules AddFn with for module-specific execution.
 *  Info: Add <Type-Name> <Name> <Key> <Value>
 *        'Name' is the custom definition name. Allowed: [azAZ09._], uint8_t[32]
 *        'Type-Name' is module name
 *        'Key' is an allowed attribute-Keys - provided by module "Type-Name"
 *        'Value' is checked by module "Type-Name"
 *  Para: const uint8_t *args  -> prt to space seperated Add-args text "<Type-Name> <Name> <Key> <Value>"
 *        const size_t argsLen -> length of Add-args
 *  Rets: struct headRetMsgMultiple_s -> STAILQ head of multiple retMsg, if NULL -> no retMsg-entry
 * --------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s ICACHE_FLASH_ATTR
Add_CommandFn (const uint8_t *args
		, const size_t argsLen)
{

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
  const uint8_t *kvArgs = name;

  // a 2nd seek-counter
  int j = 0;

  // seek to next space !'\32'
  while( (i < argsLen) && (*kvArgs != ' ') ) {i++,j++;kvArgs++;}

  // length of Name
  size_t nameLen = j;

  // start position of Definition
  while( (i < argsLen) && (*kvArgs == ' ') ) {i++;kvArgs++;}

  // length of Key-Value-Args
  size_t kvArgsLen = argsLen - i;

  // veryfy lengths > 0, definition 0 allowed
  if ( (typeNameLen == 0  || (nameLen) == 0 || (kvArgsLen) == 0 ) ) {

	// alloc mem for retMsg
	strTextMultiple_t *retMsg =
		 malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Error! Could not execute command ADD! Arguments '%.*s' not sufficient! Usage: Add <Type-Name> <Def-Name> <Key> <Value>"
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
		,"Error! Could not execute command ADD with arguments '%.*s'! '%.*s' is an unknown <Type-Name>!"
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

  STAILQ_FOREACH(Common_Definition, &SCDERoot->HeadCommon_Definitions, entries) {

	if ( (Common_Definition->nameLen == nameLen)
		&& (!strncasecmp((const char*) Common_Definition->name, (const char*) name, nameLen)) ) {

		// AddFn assigned by module ?
		if (Module->provided->AddFn) {

			// execute AddFn and get error msg
			strTextMultiple_t *retMsg = 
				Module->provided->AddFn(Common_Definition, kvArgs, kvArgsLen);

			// got an error msg?
			if (retMsg) {

				// insert retMsg in stail-queue
				STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);
			}
		}

		// AddFn NOT assigned by module
		else	{

			// alloc mem for retMsg
			strTextMultiple_t *retMsg =
				 malloc(sizeof(strTextMultiple_t));

			// response with error -> Type doesnt support set Fn
			retMsg->strTextLen = asprintf(&retMsg->strText
				,"Error! Could not ADD Key=Value attributes '%.*s' to define '%.*s'! Type '%.*s' does not support it!"
				,kvArgsLen
				,kvArgs
				,Common_Definition->nameLen
				,Common_Definition->name
			   	,Common_Definition->module->provided->typeNameLen
				,Common_Definition->module->provided->typeName);

			// insert retMsg in stail-queue
			STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

		}

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;

	}

  }
// -------------------------------------------------------------------------------------------------

  // alloc mem for retMsg
  strTextMultiple_t *retMsg =
	malloc(sizeof(strTextMultiple_t));

  // response with error -> name not found
  retMsg->strTextLen = asprintf(&retMsg->strText
	,"Error! Could not execute command ADD! Define '%.*s' not found!"
 	,nameLen
	,name);
	
  // insert retMsg in stail-queue
  STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

  // return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
  return headRetMsgMultiple;

}


