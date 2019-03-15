/* #################################################################################################
 *
 *  Function: Delete Command - for SCDE (Smart Connected Device Engine)
 *            To delete Definitions
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
static SCDERoot_t* p_SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for faster operation
static SCDEFn_t* p_SCDEFn;

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
  "Usage: Delete <definition-name> [<type dependent arguments>], to delete a device";
// CommandHelp (detailed)
const uint8_t Delete_helpDetailText[] = 
  "Usagebwrebwerb: define <name> <type> <options>, to define a device";

ProvidedByCommand_t Delete_ProvidedByCommand = {
  "Delete",					// Command-Name of command -> libfilename.so !
  6,						// length of cmd
  Delete_InitializeCommandFn,			// Initialize Fn
  Delete_CommandFn,				// the Fn code
  { &Delete_helpText, sizeof(Delete_helpText) },
  { &Delete_helpDetailText, sizeof(Delete_helpDetailText) }
};



/* --------------------------------------------------------------------------------------------------
 *  FName: Delete - Initialize Command Funktion
 *  Desc: Initializion of an (new loaded) SCDE-Command. Init p_SCDERoot and p_SCDE Function Callbacks.
 *  Info: Called only once befor use!
 *  Para: SCDERoot_t* p_SCDERoot_from_core -> ptr to SCDE Data Root from SCDE-Core
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR
Delete_InitializeCommandFn(SCDERoot_t* p_SCDERoot_from_core)
{
  // make data root locally available
  p_SCDERoot = p_SCDERoot_from_core;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  p_SCDEFn = p_SCDERoot->SCDEFn;

// --------------------------------------------------------------------------------------------------

  #if Delete_Command_DBG >= 3
  p_SCDEFn->Log3Fn(Delete_ProvidedByCommand.commandNameText,
	Delete_ProvidedByCommand.commandNameTextLen,
	3,
	"InitializeFn called. Now useable.");
  #endif

// --------------------------------------------------------------------------------------------------

  return 0;
}



/* --------------------------------------------------------------------------------------------------
 *  FName: Delete - The Command main Fn
 *  Desc: Deletes (undefines) a Definition by calling Modules UndefineFn to do further module-specific
 *        deinitialization. Finally cleans up common values, including the definition.
 *  Info: ret Msg from UndefineFn results in Veto (keep Definition)
 *  Para: const uint8_t* p_args  -> space seperated command args text string "definition_name delete_args"
 *        const size_t args_len -> command args text length
 *  Rets: struct headRetMsgMultiple_s -> STAILQ head of multiple retMsg, if NULL -> no retMsg
 * --------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s ICACHE_FLASH_ATTR
Delete_CommandFn (const uint8_t* p_args
		,const size_t args_len)
{
  #if Delete_Command_DBG >= 7
  p_SCDEFn->Log3Fn(Delete_ProvidedByCommand.commandNameText,
	Delete_ProvidedByCommand.commandNameTextLen,
	7,
	"CommandFn called with args '%.*s'",
	args_len,
	p_args);
  #endif

// --------------------------------------------------------------------------------------------------

  // prepare STAILQ head for multiple RetMsg storage
  struct headRetMsgMultiple_s headRetMsgMultiple;

  // initialize the queue
  STAILQ_INIT(&headRetMsgMultiple);

// --------------------------------------------------------------------------------------------------
	
  // set * to start of possible definition-name text (seek-start-pos)
  const uint8_t* p_definition_name = p_args;

  // the total seek-counter
  int i = 0;
	
  // seek * to start of  definition-name text ('\32' after space)
  while( ( i < args_len ) && ( *p_definition_name == ' ' ) ) { i++ ; p_definition_name++ ; }

  // @1

  // set * to start of possible delete-args text (seek-start-pos)
  const uint8_t* p_delete_args = p_definition_name;

  // an element seek-counter
  int j = 0;

  // seek to next space '\32'
  while( ( i < args_len ) && ( *p_delete_args != ' ' ) ) { i++, j++ ; p_delete_args++ ; }

  // length of definition-name text determined
  size_t definition_name_len = j;

  // seek * to start of delete-args text ('\32' after space)
  while( ( i < args_len ) && ( *p_delete_args == ' ' ) ) { i++ ; p_delete_args++ ; }

  // @2
	
  // set start * of possible 'end of text' seek-start-pos
  const uint8_t* p_end_of_text = p_delete_args;
	
  // clear element seek-counter
  j = 0;

  // seek to next space '\32'
  while( ( i < args_len ) && ( *p_end_of_text != ' ' ) ) { i++ , j++ ; p_end_of_text++ ; }

  // length of attr-Val text determined
  size_t delete_args_len = j;

  // @ 'p_end_of_text' ! No further processing ...

// --------------------------------------------------------------------------------------------------

  // veryfy lengths > 0
  if ( definition_name_len == 0 ) {

	// alloc mem for retMsg
	strTextMultiple_t *p_retMsg =
		 malloc(sizeof(strTextMultiple_t));

	// response with error text
	p_retMsg->strTextLen = asprintf(&p_retMsg->strText
		,"Error! Could not interpret delete command arguments '%.*s'! Usage: delete <definition-name> [<type dependent arguments>]"
		,args_len
		,p_args);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, p_retMsg, entries);

	// return STAILQ head, stores multiple retMsg
	return headRetMsgMultiple;
  }

// --------------------------------------------------------------------------------------------------

/* delete_args are optional !!!! only pass through
  // veryfy lengths > 0
  if (delete_args_len != 0) {

	// alloc mem for retMsg
	strTextMultiple_t* p_retMsg =
		 malloc(sizeof(strTextMultiple_t));

	// response with error text
	p_retMsg->strTextLen = asprintf(&p_retMsg->strText,
		"Error! Could not interprete additional arguments '%.*s'!",
	   	delete_args_len,
		p_delete_args);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, p_retMsg, entries);

	// return STAILQ head, stores multiple retMsg
	return headRetMsgMultiple;
  }*/

// --------------------------------------------------------------------------------------------------

  // get the Common Definition by Name
  Common_Definition_t* p_common_definition =
	STAILQ_FIRST(&p_SCDERoot->HeadCommon_Definitions);

  while (1) {

	// end of stored Definitions? Response with error -> name not found
	if (p_common_definition == NULL) {

		// alloc mem for retMsg
		strTextMultiple_t *p_retMsg =
			malloc(sizeof(strTextMultiple_t));

		// response with error text
		p_retMsg->strTextLen = asprintf(&p_retMsg->strText,
			"Error! Could not find '%.*s' for command execution!",
			definition_name_len, p_definition_name);

		// insert retMsg in stail-queue
		STAILQ_INSERT_TAIL(&headRetMsgMultiple, p_retMsg, entries);

		// return STAILQ head, stores multiple retMsg
		return headRetMsgMultiple;
	}

	// matching Definition Name ? -> break loop
	if ( (p_common_definition->nameLen == definition_name_len)
		&& (!strncasecmp((const char*) p_common_definition->name, (const char*) p_definition_name, definition_name_len)) ) {

		// found, break and keep prt
		break;

	}

	// get next Common_Definition for processing
	p_common_definition = STAILQ_NEXT(p_common_definition, entries);
  }

// --------------------------------------------------------------------------------------------------

  // Execute UndefineFn 
  if (p_common_definition->module->provided->UndefineFn) {

	// execute UndefineFn and get error msg
	strTextMultiple_t* p_retMsg =
		p_common_definition->module->provided->UndefineFn(p_common_definition);

	// got an error msg? -> VETO, cancel delete, return with error msg!
	if (p_retMsg) {

		// insert retMsg in stail-queue
		STAILQ_INSERT_TAIL(&headRetMsgMultiple, p_retMsg, entries);

		// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
 		return headRetMsgMultiple;
	}
  }

  // or error msg if this type (Module) is not capable to undefine
  else	{

	// response with error -> Type doesnt support undefine Fn

	// alloc mem for retMsg
	strTextMultiple_t* p_retMsg =
		malloc(sizeof(strTextMultiple_t));

	// response with error text
	p_retMsg->strTextLen = asprintf(&p_retMsg->strText,
		"Error! Could not execute UNDEFINE command on '%.*s', because a TYPE '%.*s' does not support it!",
		p_common_definition->nameLen,
		p_common_definition->name,
		p_common_definition->module->provided->typeNameLen,
		p_common_definition->module->provided->typeName);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, p_retMsg, entries);

	#if Delete_Command_DBG >= 7
	p_SCDEFn->Log3Fn(Delete_ProvidedByCommand.commandNameText,
		Delete_ProvidedByCommand.commandNameTextLen,
		7,
		"An veto occured while deleting an Definition with args '%.*s'. Check logged msg.",
		args_len,
		p_args);
	#endif

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
 	return headRetMsgMultiple;
  }

// --------------------------------------------------------------------------------------------------


//TAILQ_REMOVE(STAILQ_HEAD *head, TYPE *elm, TYPE, STAILQ_ENTRY NAME);

  // remove the Definition from SCDE-Root
  STAILQ_REMOVE(&p_SCDERoot->HeadCommon_Definitions,
	p_common_definition, Common_Definition_s, entries);

  // now do the common cleanup

  // free the initial state '???' - may be NULL now!
  if (p_common_definition->state) free(p_common_definition->state);

  // check for Readings here, and free if any
  // xxx

  // Attributes ??

  // free the Definition string - may be NULL now!
  if (p_common_definition->definition) free(p_common_definition->definition);

  // free the custom Name - can NOT be NULL now!
  free(p_common_definition->name);

   // delete the semaphore for definition access - is ALWAYS there !
   vSemaphoreDelete(p_common_definition->def_mux);

  // finally free the Definition
  free(p_common_definition);

// --------------------------------------------------------------------------------------------------

  // return STAILQ head, stores multiple retMsg
  return headRetMsgMultiple;
}


