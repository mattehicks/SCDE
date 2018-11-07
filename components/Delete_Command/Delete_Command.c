/* #################################################################################################
 *
 *  Function: Delete Command for SCDE (Smart Connected Device Engine)
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

#include "include/Delete_Command.h"



// set default build verbose - if no external override
#ifndef Delete_Command_DBG  
#define Delete_Command_DBG  5	// 5 is default
#endif



// -------------------------------------------------------------------------------------------------

// make data root locally available
static SCDERoot_t* SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for faster operation
static SCDEFn_t* SCDEFn;

// -------------------------------------------------------------------------------------------------



/* --------------------------------------------------------------------------------------------------
 *  DName: Delete_ProvidedByCommand
 *  Desc: Data 'Provided By Command' for this Command (functions + infos this command provides
 *        to SCDE)
 *  Data: 
 * --------------------------------------------------------------------------------------------------
 */

// Command Help
const uint8_t Delete_helpText[] = 
  "Usage: delete <name>, to delete a device";
// CommandHelp (detailed)
const uint8_t Delete_helpDetailText[] = 
  "Usagebwrebwerb: define <name> <type> <options>, to define a device";

providedByCommand_t Delete_ProvidedByCommand =
  {
   "Delete"					// Command-Name of command -> libfilename.so !
  ,6						// length of cmd
  ,Delete_InitializeCommandFn			// Initialize Fn
  ,Delete_CommandFn				// the Fn code
  ,&Delete_helpText
  ,sizeof(Delete_helpText)
  ,&Delete_helpDetailText
  ,sizeof(Delete_helpDetailText)
  };

//(const uint8_t *) "Usage: define <name> <type> <options>, to define a device",57);	// CommandHelp, Len


/* --------------------------------------------------------------------------------------------------
 *  FName: Delete_InitializeCommandFn
 *  Desc: Initializion of SCDE Function Callback of an new loaded command
 *  Info: Stores Command-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
Delete_InitializeCommandFn(SCDERoot_t* SCDERootptr)
  {

  // make data root locally available
  SCDERoot = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn = SCDERootptr->SCDEFn;

  SCDEFn->Log3Fn(Delete_ProvidedByCommand.commandNameText
		  ,Delete_ProvidedByCommand.commandNameTextLen
		  ,3
		  ,"InitializeFn called. Command '%.*s' now useable.\n"
		  ,Delete_ProvidedByCommand.commandNameTextLen
		  ,Delete_ProvidedByCommand.commandNameText);

  return 0;

  }





/* --------------------------------------------------------------------------------------------------
 *  FName: Delete_Command
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
struct headRetMsgMultiple_s ICACHE_FLASH_ATTR
Delete_CommandFn (const uint8_t *args
		,const size_t argsLen)
  {

  // prepare STAILQ head for multiple RetMsg storage
  struct headRetMsgMultiple_s headRetMsgMultiple;

  // Initialize the queue
  STAILQ_INIT(&headRetMsgMultiple);

  // set start of possible Name
  const uint8_t *name = args;

  // set start of possible Delete-Arguments
  const uint8_t *deleteArgs = args;

  // a seek-counter
  int i = 0;

  // seek to next space !'\32'
  while( (i < argsLen) && (*deleteArgs != ' ') ) {i++;deleteArgs++;}

  // length of Name
  size_t nameLen = i;

  // seek to start position of Delete-Arguments '\32'
  while( (i < argsLen) && (*deleteArgs == ' ') ) {i++;deleteArgs++;}

   // length of Delete-Arguments
  size_t deleteArgsLen = argsLen - i;

  // veryfy lengths > 0
  if (nameLen == 0) {

	// alloc mem for retMsg
	strTextMultiple_t *retMsg =
		 malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Error! Could not interpret delete command arguments '%.*s'! Usage: delete <name>"
		,argsLen
		,args);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;

	}

  // veryfy lengths > 0
  if (deleteArgsLen != 0) {

	// alloc mem for retMsg
	strTextMultiple_t *retMsg =
		 malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Error! Could not interprete additional arguments '%.*s'!"
	   	,deleteArgsLen
		,deleteArgs);

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

		if (Common_Definition->module->ProvidedByModule->UndefineFn) {

			// execute DefineFn and get error msg
			strTextMultiple_t *retMsg =
				Common_Definition->module->ProvidedByModule->UndefineFn(Common_Definition);

			// got an error msg?
			if (retMsg) {

				// insert retMsg in stail-queue
				STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);
			}
		}

		else	{

			// response with error -> Type doesnt support undefine Fn

			// alloc mem for retMsg
			strTextMultiple_t *retMsg =
				malloc(sizeof(strTextMultiple_t));

			// response with error text
			retMsg->strTextLen = asprintf(&retMsg->strText
				,"Error! Could not execute UNDEFINE command on '%.*s', because a TYPE '%.*s' does not support it!"
				,Common_Definition->nameLen
				,Common_Definition->name
			   	,Common_Definition->module->ProvidedByModule->typeNameLen
				,Common_Definition->module->ProvidedByModule->typeName);

			// insert retMsg in stail-queue
			STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

		}

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
 	return headRetMsgMultiple;

	}


  }

  // response with error -> name not found

  // alloc mem for retMsg
  strTextMultiple_t *retMsg =
	 malloc(sizeof(strTextMultiple_t));

  // response with error text
  retMsg->strTextLen = asprintf(&retMsg->strText
	,"Error! Could not find '%.*s' for command execution!"
 	,nameLen
	,name);


  // insert retMsg in stail-queue
  STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

  // return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
  return headRetMsgMultiple;

}



















