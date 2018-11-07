/* #################################################################################################
 *
 *  Function: Define Command for SCDE (Smart Connected Device Engine)
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
static SCDERoot_t* SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for operation
static SCDEFn_t* SCDEFn;

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
  "Usage: Define <name> <type> <type specific options>, to define a device";
// CommandHelp (detailed)
const uint8_t Define_helpDetailText[] = 
  "Usagebwrebwerb: Define <name> <type> <options>, to Define a device";


providedByCommand_t Define_ProvidedByCommand = {
  "Define"					// Command-Name of command -> libfilename.so !
  ,6						// length of cmd
  ,Define_InitializeCommandFn			// Initialize Fn
  ,Define_CommandFn				// the Fn code
  ,&Define_helpText
  ,sizeof(Define_helpText)
  ,&Define_helpDetailText
  ,sizeof(Define_helpDetailText)
};

//(const uint8_t *) "Usage: Define <name> <type> <options>, to Define a device",57);	// CommandHelp, Len


/* --------------------------------------------------------------------------------------------------
 *  FName: Define_Initialize
 *  Desc: Initializion of SCDE Function Callback of an new loaded command
 *  Info: Stores Command-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
Define_InitializeCommandFn(SCDERoot_t* SCDERootptr)
{

  // make data root locally available
  SCDERoot = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn = SCDERootptr->SCDEFn;

  SCDEFn->Log3Fn(Define_ProvidedByCommand.commandNameText
		  ,Define_ProvidedByCommand.commandNameTextLen
		  ,3
		  ,"InitializeFn called. Command '%.*s' now useable."
		  ,Define_ProvidedByCommand.commandNameTextLen
		  ,Define_ProvidedByCommand.commandNameText);

  return 0;

}





/* --------------------------------------------------------------------------------------------------
 *  FName: CommandDefine
 *  Desc: Creates a new common Define with name "Name", and Module "Type-Name", prevills some common
 *        values and calls Modules DefFn with for further module-specific initialization.
 *  Info: 'Name' is custom definition name. Allowed: [azAZ09._], uint8_t[32]
 *        'Type-Name' is module name
 *        'Definition ...' is custom and passed to modules DefineFn, stored in Definition->Definition
 *  Para: const uint8_t *args  -> prt to space seperated Define text string "Name Type-Name Definition ..."
 *        const size_t argsLen -> length of args
 *  Rets: struct headRetMsgMultiple_s -> STAILQ head of multiple retMsg, if NULL -> no retMsg-entry
 * --------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s ICACHE_FLASH_ATTR
Define_CommandFn (const uint8_t *args
		, const size_t argsLen)
{

  // prepare STAILQ head for multiple RetMsg storage
  struct headRetMsgMultiple_s headRetMsgMultiple;

  // Initialize the queue
  STAILQ_INIT(&headRetMsgMultiple);

  // set start of possible Name
  const uint8_t *name = args;

  // set start of possible Type-Name
  const uint8_t *typeName = args;

  // a seek-counter
  int i = 0;

  // seek to next space !'\32'
  while( (i < argsLen) && (*typeName != ' ') ) {i++; typeName++;}

  // length of Name
  size_t nameLen = i;

  // seek to start position of Type-Name '\32'
  while( (i < argsLen) && (*typeName == ' ') ) {i++; typeName++;}

  // set start of possible Definition
  const uint8_t *definition = typeName;

  // a 2nd seek-counter
  int j = 0;

  // seek to next space !'\32'
  while( (i < argsLen) && (*definition != ' ') ) {i++, j++; definition++;}

  // length of Type-Name
  size_t typeNameLen = j;

  // start position of Definition
  while( (i < argsLen) && (*definition == ' ') ) {i++; definition++;}

  // length of Type-Name
  size_t definitionLen = argsLen - i;

  // veryfy lengths > 0, definition 0 allowed
  if ( (nameLen) == 0 || (typeNameLen == 0) ) {

    // alloc mem for retMsg
    strTextMultiple_t *retMsg =
      malloc(sizeof(strTextMultiple_t));

    // response with error text
    retMsg->strTextLen = asprintf(&retMsg->strText
      ,"Could not interpret command '%.*s'! Usage: Define <name> <type-name> <type dependent arguments>"
      ,argsLen
      ,args);

    // insert retMsg in stail-queue
    STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

    // return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
    return headRetMsgMultiple;

  }

 /*
   if(Defined($defs{$name}));
   asprintf(&RetMsg, "%s already Defined, Define it first\n", name);
   return RetMsg;

   if($name !~ m/^[a-z0-9.:_]*$/i);
   asprintf(&RetMsg, "Invalid characters in name (not A-Za-z0-9._): %s\n", name);
   return RetMsg;
  */


   // get the module ptr by name
   Module_t* Module = SCDEFn->GetLoadedModulePtrByNameFn(typeName
    ,typeNameLen);

  // do we need to reload Module?
  if (!Module) // NOT FUNCTIONAL!
    Module = SCDEFn->CommandReloadModuleFn(typeName, typeNameLen);

  // alloc mem for modul specific definition structure (Common_Definition_t + X)
  Common_Definition_t *NewCommon_Definition
    = malloc(Module->ProvidedByModule->SizeOfDefinition);

  // zero the struct
  memset(NewCommon_Definition, 0, Module->ProvidedByModule->SizeOfDefinition);

  // prepare default fields in definition

  // create semaphore for definition access
  NewCommon_Definition->def_mux = xSemaphoreCreateMutex();

  // copy ptr to associated module
  NewCommon_Definition->module = Module;

  // copy custom name
  NewCommon_Definition->name = malloc(nameLen);
  memcpy(NewCommon_Definition->name, name, nameLen);
  NewCommon_Definition->nameLen = nameLen;

  // store new definition to SCDE-Root
  STAILQ_INSERT_HEAD(&SCDERoot->HeadCommon_Definitions
	,NewCommon_Definition
	,entries);

  printf("Defined Name:%.*s TypeName:%.*s\n"
    ,NewCommon_Definition->nameLen
    ,NewCommon_Definition->name
    ,NewCommon_Definition->module->ProvidedByModule->typeNameLen
    ,NewCommon_Definition->module->ProvidedByModule->typeName);

  // store Definition string in Defp->Definition
  NewCommon_Definition->definition = malloc(definitionLen);
  memcpy(NewCommon_Definition->definition, definition, definitionLen);
  NewCommon_Definition->definitionLen = definitionLen;

  // assign an unique number
  NewCommon_Definition->nr = SCDERoot->DevCount++;

  // do we need initial state? or NULL ? store initial state
  NewCommon_Definition->stateLen =
    asprintf((char**)&NewCommon_Definition->state, "???");

  // init stailq readings
  STAILQ_INIT(&NewCommon_Definition->headReadings);

  // DefineFn assigned by module ?
  if (Module->ProvidedByModule->DefineFn) {

    printf("Calling DefineFN for Name:%.*s TypeName:%.*s Definition:%.*s cnt:%d state:%.*s\n"
      ,NewCommon_Definition->nameLen
      ,NewCommon_Definition->name
      ,NewCommon_Definition->module->ProvidedByModule->typeNameLen
      ,NewCommon_Definition->module->ProvidedByModule->typeName
      ,NewCommon_Definition->definitionLen
      ,NewCommon_Definition->definition
      ,NewCommon_Definition->nr
      ,NewCommon_Definition->stateLen
      ,NewCommon_Definition->state);

    // execute DefineFn and maybe get an error msg
    strTextMultiple_t *retMsg = 
      Module->ProvidedByModule->DefineFn(NewCommon_Definition);

    // got an error msg?
    if (retMsg) {

      // insert retMsg in stail-queue
      STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);
    }
	  
  }
	
/*
  // DefineFn NOT assigned by module
  else	{

	// alloc mem for retMsg
	strTextMultiple_t *retMsg =
		 malloc(sizeof(strTextMultiple_t));

	// response with error -> Type doesnt support DefineFn
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Error! Could not ADD Key=Value attributes '%.*s' to define '%.*s'! Type '%.*s' does not support it!"
		,kvArgsLen
		,kvArgs
		,Common_Definition->nameLen
		,Common_Definition->name
		,Common_Definition->module->ProvidedByModule->typeNameLen
		,Common_Definition->module->ProvidedByModule->typeName);

		// insert retMsg in stail-queue
		STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

  }
*/

  // do we have an error msg? Interpret it as veto!
  if (!STAILQ_EMPTY(&headRetMsgMultiple)) {

//	printf("Got error from DefineFN %s \n"
//			  ,retMsg.strText);

    printf("Got error from DefineFN!\n");

	//Log 1, "Define $def: $ret";
//	DefineDefinition(Name);	//Define $defs{$name};                            # Veto
//	DefineAttribute(Name);	//Define $attr{$name};

  }

  // return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
  return headRetMsgMultiple;

}


