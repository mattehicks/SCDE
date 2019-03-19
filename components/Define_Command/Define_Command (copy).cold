/* #################################################################################################
 *
 *  Function: Define Command - for SCDE (Smart Connected Device Engine)
 *            Creates an Definition, which is then called Device
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

#include "Define_Command.h"



// set default build verbose - if no external override
#ifndef Define_Command_DBG  
#define Define_Command_DBG  5	// 5 is default
#endif



// --------------------------------------------------------------------------------------------------

// make data root locally available
static SCDERoot_t* p_SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for operation
static SCDEFn_t* p_SCDEFn;

// --------------------------------------------------------------------------------------------------



/* --------------------------------------------------------------------------------------------------
 *  DName: Define_Command
 *  Desc: Data 'Provided By Command' for the Define Command (functions + infos this command provides
 *        to SCDE)
 *  Data: 
 * --------------------------------------------------------------------------------------------------
 */

// Command Help
const uint8_t Define_helpText[] = 
  "Usage: Define <definition-name> <module-name> <type dependent arguments>, to define a device";
// CommandHelp (detailed)
const uint8_t Define_helpDetailText[] = 
  "Usagebwrebwerb: Define <name> <type> <options>, to Define a device";


ProvidedByCommand_t Define_ProvidedByCommand = {
  "Define",					// Command-Name of command -> libfilename.so !
  6,						// length of cmd
  Define_InitializeCommandFn,			// Initialize Fn
  Define_CommandFn,				// the Fn code
  { &Define_helpText, sizeof(Define_helpText) },
  { &Define_helpDetailText, sizeof(Define_helpDetailText) }
};



/* --------------------------------------------------------------------------------------------------
 *  FName: Define - Initialize Command Funktion
 *  Desc: Initializion of an (new loaded) SCDE-Command. Init p_SCDERoot and p_SCDE Function Callbacks.
 *  Info: Called only once befor use!
 *  Para: SCDERoot_t* p_SCDERoot_from_core -> ptr to SCDE Data Root from SCDE-Core
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR
Define_InitializeCommandFn(SCDERoot_t* p_SCDERoot_from_core)
{
  // make data root locally available
  p_SCDERoot = p_SCDERoot_from_core;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  p_SCDEFn = p_SCDERoot->SCDEFn;

// --------------------------------------------------------------------------------------------------

  #if Define_Command_DBG >= 3
  p_SCDEFn->Log3Fn(Define_ProvidedByCommand.commandNameText,
	  Define_ProvidedByCommand.commandNameTextLen,
	  3,
	  "InitializeFn called. Now useable.");
  #endif

// --------------------------------------------------------------------------------------------------

  return 0;
}



/* --------------------------------------------------------------------------------------------------
 *  FName: Define - The Command main Fn
 *  Desc: Creates a new Definiton of Module "Type-Name" with custom Name "Name", prevills some common
 *        values and calls Modules DefineFn, to continue further module-specific initialization.
 *  Info: 'Definition-Name' is custom Definition name. Allowed: [azAZ09._], uint8_t[32]
 *        'Module-Name' is Module name
 *        'Definition-Args' is custom, stored in Definition->Definition, and passed to modules DefineFn
 *  Para: const uint8_t* p_args  -> space seperated command args text string "definition_name module_name definition_args"
 *        const size_t args_len -> command args text length
 *  Rets: struct headRetMsgMultiple_s -> STAILQ head of multiple retMsg, if NULL -> no retMsg
 * --------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s ICACHE_FLASH_ATTR
Define_CommandFn (const uint8_t* p_args,
		  const size_t args_len)
{
  #if Define_Command_DBG >= 7
  p_SCDEFn->Log3Fn(Define_ProvidedByCommand.commandNameText,
	Define_ProvidedByCommand.commandNameTextLen,
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
	
  // seek * to start of definition-name text ('\32' after space)
  while( ( i < args_len ) && ( *p_definition_name == ' ' ) ) { i++ ; p_definition_name++ ; }

  // @1

  // set * to start of possible module-name text (seek-start-pos)
  const uint8_t* p_module_name = p_definition_name;

  // an element seek-counter
  int j = 0;

  // seek to next space '\32'
  while( ( i < args_len ) && ( *p_module_name != ' ' ) ) { i++, j++ ; p_module_name++ ; }

  // length of def-spec text determined
  size_t definition_name_len = j;

  // seek * to start of module-name text ('\32' after space)
  while( ( i < args_len ) && ( *p_module_name == ' ' ) ) { i++ ; p_module_name++ ; }

  // @2

  // set * to start of possible definition-args text (seek-start-pos)
  const uint8_t* p_definition_args = p_module_name;

  // clear element seek-counter
  j = 0;

  // seek to next space '\32'
  while( ( i < args_len ) && ( *p_definition_args != ' ' ) ) { i++ , j++ ; p_definition_args++ ; }

  // length of attr-Name-Text determined
  size_t module_name_len = j;

  // seek * to start of definition-args text ('\32' after space)
  while( ( i < args_len ) && ( *p_definition_args == ' ' ) ) { i++ ; p_definition_args++ ; }

  // @3
	
  // set start * of possible 'end of text' seek-start-pos
  const uint8_t* p_end_of_text = p_definition_args;
	
  // clear element seek-counter
  j = 0;

  // seek to next space '\32'
  while( ( i < args_len ) && ( *p_end_of_text != ' ' ) ) { i++ , j++ ; p_end_of_text++ ; }

  // length of definition-args text determined
  size_t definition_args_len = j;

  // @ 'p_end_of_text' ! No further processing ...

// -------------------------------------------------------------------------------------------------

  // veryfy lengths > 0, definition 0 allowed
  if ( ( definition_name_len == 0 ) || ( module_name_len == 0) ) {

	// alloc mem for retMsg
	strTextMultiple_t* p_retMsg =
		malloc(sizeof(strTextMultiple_t));

	// response with error text
	p_retMsg->strTextLen = asprintf(&p_retMsg->strText,
		"Could not interpret command '%.*s'! Usage: Define <definition-name> <module-name> <type dependent arguments>",
		args_len, p_args);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, p_retMsg, entries);

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;
  }

// --------------------------------------------------------------------------------------------------

 /*
   if(Defined($defs{$name}));
   asprintf(&RetMsg, "%s already Defined, Define it first\n", name);
   return RetMsg;
*/

// --------------------------------------------------------------------------------------------------
/*
   if($name !~ m/^[a-z0-9.:_]*$/i);
   asprintf(&RetMsg, "Invalid characters in name (not A-Za-z0-9._): %s\n", name);
   return RetMsg;
  */
// --------------------------------------------------------------------------------------------------

   // get the module ptr by name
   Module_t* p_module = p_SCDEFn->GetLoadedModulePtrByNameFn(p_module_name,
 	module_name_len);

  // do we need to reload Module?
  if (!p_module)
	p_module = p_SCDEFn->CommandReloadModuleFn(p_module_name, module_name_len);

  // still no Module -> we don't have it, error!
  if (!p_module) {

	// alloc mem for retMsg
	strTextMultiple_t* p_retMsg =
		malloc(sizeof(strTextMultiple_t));

	// response with error text
	p_retMsg->strTextLen = asprintf(&p_retMsg->strText,
		"Could not find a Module '%.*s'!",
		module_name_len,
		p_module_name);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, p_retMsg, entries);

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;
  }

// --------------------------------------------------------------------------------------------------

  // can we get ptr to an Definition with requested Name? -> same Name is not allowed, error!
  if (p_SCDEFn->GetDefinitionPtrByNameFn(definition_name_len, p_definition_name)) {

	// alloc mem for retMsg
	strTextMultiple_t* p_retMsg =
		malloc(sizeof(strTextMultiple_t));

	// response with error text
	p_retMsg->strTextLen = asprintf(&p_retMsg->strText,
		"An Definition called '%.*s' is already in use!",
		definition_name_len,
		p_definition_name);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, p_retMsg, entries);

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;
  }

// --------------------------------------------------------------------------------------------------

  // alloc mem for modul specific definition structure (Common_Definition_t + X)
  Common_Definition_t* p_new_common_definition
	= malloc(p_module->provided->SizeOfDefinition);

  // zero the struct
  memset(p_new_common_definition, 0, p_module->provided->SizeOfDefinition);

// --------------------------------------------------------------------------------------------------

  // prepare default fields in definition

  // create semaphore for definition access
  p_new_common_definition->def_mux = xSemaphoreCreateMutex();

  // copy ptr to associated module
  p_new_common_definition->module = p_module;

  // init the list for 'attr_value's
  LIST_INIT(&p_new_common_definition->head_attr_value);

  // copy custom name (NAME LENGTH IS > 0 !)
  p_new_common_definition->name = malloc(definition_name_len);
  memcpy(p_new_common_definition->name, p_definition_name, definition_name_len);
  p_new_common_definition->nameLen = definition_name_len;

  // store define_args string (the args) in Defp->Definition. ITS ALLOWED TO HAVE NO DEFINITION!!
  if (definition_args_len) {
	p_new_common_definition->definition = malloc(definition_args_len);
	memcpy(p_new_common_definition->definition, p_definition_args, definition_args_len);
	p_new_common_definition->definitionLen = definition_args_len;
  }

  // assign an Unique Number
  p_new_common_definition->nr = p_SCDERoot->device_count++;

  // init readings (stailq), now empty
  STAILQ_INIT(&p_new_common_definition->headReadings);

  // set initial state '???'
  p_new_common_definition->stateLen =
	asprintf((char**)&p_new_common_definition->state, "???");

// --------------------------------------------------------------------------------------------------

  // xx mark definition as under construction?

  // store new definition to SCDE-Root
  STAILQ_INSERT_HEAD(&p_SCDERoot->HeadCommon_Definitions,
	p_new_common_definition, entries);

// --------------------------------------------------------------------------------------------------

  // DefineFn NOT assigned by module ? -> retMsg + VETO!
  if (!p_module->provided->DefineFn) {

	// alloc mem for retMsg
	strTextMultiple_t* p_retMsg =
		 malloc(sizeof(strTextMultiple_t));

	// response with error text
	p_retMsg->strTextLen = asprintf(&p_retMsg->strText
		,"DefineFn not implemented in Module of Type '%.*s'. Veto!"
		,module_name_len
		,p_module_name);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, p_retMsg, entries);
  }

  // DefineFn assigned by module.  -> retMsg forces VETO!
  else {

	#if Define_Command_DBG >= 7
	p_SCDEFn->Log3Fn(Define_ProvidedByCommand.commandNameText,
		Define_ProvidedByCommand.commandNameTextLen,
		7,
		"Calling DefineFn of Module '%.*s' for construction of new Definition '%.*s' "
		"using arguments '%.*s'. (Nr.:%d, Initial State:%.*s)",
		p_new_common_definition->module->provided->typeNameLen,
		p_new_common_definition->module->provided->typeName,
		p_new_common_definition->nameLen,
		p_new_common_definition->name,
		p_new_common_definition->definitionLen,
      		p_new_common_definition->definition,
    		p_new_common_definition->nr,
		p_new_common_definition->stateLen,
     		p_new_common_definition->state);
		#endif

	// call Modules DefineFn. Interpret retMsgMultipleStringSLTQE != NULL as veto !
	strTextMultiple_t* p_retMsg = 
		p_module->provided->DefineFn(p_new_common_definition);

	// got an error msg?
	if (p_retMsg) {

		// insert retMsg in stail-queue
		STAILQ_INSERT_TAIL(&headRetMsgMultiple, p_retMsg, entries);
	}
  }

// --------------------------------------------------------------------------------------------------

  // no retMsg elements in the stailq? no veto!
  if (STAILQ_EMPTY(&headRetMsgMultiple)) {

	// OK return STAILQ head, stores multiple retMsg
	return headRetMsgMultiple;
  }

// --------------------------------------------------------------------------------------------------

  // An veto occured! Cancel the Definition-Creation! We have a ret Msg in the STAILQ

  #if Define_Command_DBG >= 7
  p_SCDEFn->Log3Fn(Define_ProvidedByCommand.commandNameText,
	Define_ProvidedByCommand.commandNameTextLen,
	7,
	"An veto occured while creating an Definition with args '%.*s'. Check logged msg.",
	args_len,
	p_args);
  #endif

  // free the initial state '???' - may be NULL now!
  if (p_new_common_definition->state) free(p_new_common_definition->state);

  // check for Readings here, and free if any
  // xxx

  // Attributes ??

  // free the Definition string - may be NULL now!
  if (p_new_common_definition->definition) free(p_new_common_definition->definition);

  // free the custom Name - can NOT be NULL now!
  free(p_new_common_definition->name);

  // delete the semaphore for definition access - is ALWAYS there !
  vSemaphoreDelete(p_new_common_definition->def_mux);

  // finally free the Definition
  free(p_new_common_definition);

// --------------------------------------------------------------------------------------------------

  // return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
  return headRetMsgMultiple;
}

