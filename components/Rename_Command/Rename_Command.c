/* #################################################################################################
 *
 *  Function: Rename Command for SCDE (Smart Connected Device Engine)
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
#include <Platform.h>

#include <esp8266.h>


#include <SCDE_s.h>

#include "Rename_Command.h"



// set default build verbose - if no external override
#ifndef Rename_Command_DBG  
#define Rename_Command_DBG  5	// 5 is default
#endif



// --------------------------------------------------------------------------------------------------

// make data root locally available
static SCDERoot_t* SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for operation
static SCDEFn_t* SCDEFn;

// --------------------------------------------------------------------------------------------------



/* --------------------------------------------------------------------------------------------------
 *  DName: Rename_Command
 *  Desc: Data 'Provided By Command' for the Rename Command (functions + infos this command provides
 *        to SCDE)
 *  Data: 
 * --------------------------------------------------------------------------------------------------
 */

// Command Help
const uint8_t Rename_helpText[] = 
  "Usage: Rename <old> <new>, to rename a definition";	// CommandHelp, Len
// CommandHelp (detailed)
const uint8_t Rename_helpDetailText[] = 
  "Usagebwrebwerb: Rename <old> <new>, to rename a definition";	// CommandHelp, Len


ProvidedByCommand_t Rename_ProvidedByCommand = {
  "Rename",					// Command-Name of command -> libfilename.so !
  6,						// length of cmd
  Rename_InitializeCommandFn,			// Initialize Fn
  Rename_CommandFn,				// the Fn code
  { &Rename_helpText, sizeof(Rename_helpText) },
  { &Rename_helpDetailText, sizeof(Rename_helpDetailText) }
};



/* --------------------------------------------------------------------------------------------------
 *  FName: Rename_Initialize
 *  Desc: Initializion of SCDE Function Callback of an new loaded command
 *  Info: Stores Command-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
Rename_InitializeCommandFn(SCDERoot_t* SCDERootptr)
{
  // make data root locally available
  SCDERoot = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn = SCDERootptr->SCDEFn;

// --------------------------------------------------------------------------------------------------

  #if Rename_Command_DBG >= 3
  SCDEFn->Log3Fn(Rename_ProvidedByCommand.commandNameText
	,Rename_ProvidedByCommand.commandNameTextLen
	,3
	,"InitializeFn called. Now useable.");
  #endif

// --------------------------------------------------------------------------------------------------

  return 0;
}





/* --------------------------------------------------------------------------------------------------
 *  FName: CommandRename
 *  Desc: Renames the definition named "old-name" to the new name "new-name"
 *  Info: 'old-name' is custom definition name. Allowed: [azAZ09._], uint8_t[32]
 *        'new-name' is custom definition name. Allowed: [azAZ09._], uint8_t[32]
 *  Para: const uint8_t *args  -> prt to space seperated Rename text string "old-name mew-name"
 *        const size_t argsLen -> length of args
 *  Rets: struct headRetMsgMultiple_s -> STAILQ head of multiple retMsg, if NULL -> no retMsg-entry
 * --------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s ICACHE_FLASH_ATTR
Rename_CommandFn (const uint8_t *args
		, const size_t argsLen)
{
  #if Rename_Command_DBG >= 7
  SCDEFn->Log3Fn(Rename_ProvidedByCommand.commandNameText
	,Rename_ProvidedByCommand.commandNameTextLen
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

  // set start of possible old name
  const uint8_t *oldName = args;

  // set start of possible new name
  const uint8_t *newName = args;

  // a seek-counter
  int i = 0;

  // seek to next space !'\32'
  while( (i < argsLen) && (*newName != ' ') ) {i++; newName++;}

  // length of old name
  size_t oldNameLen = i;

  // seek to start position of new name '\32'
  while( (i < argsLen) && (*newName == ' ') ) {i++; newName++;}

  // length of new-name
  size_t newNameLen = argsLen - i;

  // veryfy lengths > 0, definition 0 allowed
  if ( (oldNameLen) == 0 || (newNameLen == 0) ) {
	  
    // alloc mem for retMsg
    strTextMultiple_t *retMsg =
      malloc(sizeof(strTextMultiple_t));

    // response with error text
    retMsg->strTextLen = asprintf(&retMsg->strText
      ,"Could not interpret command '%.*s'! Usage: Rename <old-name> <new-name>. Note: 31 chars [azAZ09._] allowed."
      ,argsLen
      ,args);

    // insert retMsg in stail-queue
    STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

    // return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
    return headRetMsgMultiple;
  }

	// get the Common_Definition by Name
  Common_Definition_t *Common_Definition;

	// for possible answer
	strTextMultiple_t *retMsg = NULL;
	
  STAILQ_FOREACH(Common_Definition, &SCDERoot->HeadCommon_Definitions, entries) {

		if ( (Common_Definition->nameLen == oldNameLen)
			&& (!strncasecmp((const char*) Common_Definition->name, (const char*) oldName, oldNameLen)) ) {

			// definition found, backup old value
			uint8_t *oldNameBackup = Common_Definition->name;
			size_t oldNameBackupLen = Common_Definition->nameLen;
			
			// store new name
			Common_Definition->nameLen = 
				asprintf(&Common_Definition->name, "%.*s"
					,newNameLen
					,newName);
			
			// create an log entry
			SCDEFn->Log3Fn(Rename_ProvidedByCommand.commandNameText
				,Rename_ProvidedByCommand.commandNameTextLen
			  ,3
			 	,"Renaming Definition '%.*s' to '%.*s'.\n"
			  ,oldNameBackupLen
				,oldNameBackup
				,Common_Definition->nameLen
				,Common_Definition->name);
			
			// RenameFn assigned by module ?
  		if (Common_Definition->module->provided->RenameFn) {
							 
				printf("Calling module '%.*s' RenameFN(%.*s,%.*s,%.*s)\n"
					,Common_Definition->module->provided->typeNameLen
     			,Common_Definition->module->provided->typeName
					,Common_Definition->nameLen
      		,Common_Definition->name
					,Common_Definition->nameLen
					,Common_Definition->name
					,oldNameBackupLen
					,oldNameBackup);

    		// execute RenameFn and maybe get an error msg
   			strTextMultiple_t *retMsg = 
    	  	Common_Definition->module->provided->RenameFn(Common_Definition
						,Common_Definition->nameLen
						,Common_Definition->name
						,oldNameBackupLen
						,oldNameBackup);

    		// got an error msg?
    		if (retMsg) {

     	 		// insert retMsg in stail-queue
      		STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);
    		}
  		}
				
			// free mem from old name
			free(oldNameBackup);			
			
	 	 // return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
  		return headRetMsgMultiple;	
		}
	}

  // we have a retMsg, alloc mem
	retMsg =
		malloc(sizeof(strTextMultiple_t));

  // response with error text
  retMsg->strTextLen = asprintf(&retMsg->strText
		,"Error! Could not execute command Rename! Definition '%.*s' not found!"
 		,oldNameLen
		,oldName);
	
  // insert retMsg in stail-queue
  STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

  // return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
  return headRetMsgMultiple;
}
	
	
