/* #################################################################################################
 *
 *  Function: Shutdown Command for SCDE (Smart Connected Device Engine)
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

#include "Shutdown_Command.h"



// set default build verbose - if no external override
#ifndef Shutdown_Command_DBG  
#define Shutdown_Command_DBG  5	// 5 is default
#endif 



// -------------------------------------------------------------------------------------------------

// make data root locally available
static SCDERoot_t* SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for operation
static SCDEFn_t* SCDEFn;

// -------------------------------------------------------------------------------------------------



/*
 * -------------------------------------------------------------------------------------------------
 *  DName: Shutdown_Command
 *  Desc: Data 'Provided By Command' for the Shutdown Command (functions + infos this command provides
 *        to SCDE)
 *  Data: 
 * -------------------------------------------------------------------------------------------------
 */

// Command Help
const uint8_t Shutdown_helpText[] = 
  "Usage: Shutdown [restart], to shutdown engine";
// CommandHelp (detailed)
const uint8_t Shutdown_helpDetailText[] = 
  "Usagebwrebwerb: Shutdown [restart], to shutdown engine";


providedByCommand_t Shutdown_ProvidedByCommand =
  {
   "Shutdown"					// Command-Name of command -> libfilename.so !
  ,8						// length of cmd
  ,Shutdown_InitializeCommandFn			// Initialize Fn
  ,Shutdown_CommandFn				// the Fn code
  ,&Shutdown_helpText
  ,sizeof(Shutdown_helpText)
  ,&Shutdown_helpDetailText
  ,sizeof(Shutdown_helpDetailText)
  };



/* --------------------------------------------------------------------------------------------------
 *  FName: Shutdown_Initialize
 *  Desc: Initializion of SCDE Function Callback of an new loaded command
 *  Info: Stores Command-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
Shutdown_InitializeCommandFn(SCDERoot_t* SCDERootptr)
{
  // make data root locally available
  SCDERoot = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn = SCDERootptr->SCDEFn;

// --------------------------------------------------------------------------------------------------

  #if Shutdown_Command_DBG >= 3
  SCDEFn->Log3Fn(Shutdown_ProvidedByCommand.commandNameText
	,Shutdown_ProvidedByCommand.commandNameTextLen
	,3
	,"InitializeFn called. Now useable.");
  #endif

// --------------------------------------------------------------------------------------------------

  return 0;
}



/* -------------------------------------------------------------------------------------------------
 *  FName: Shutdown_CommandFn
 *  Desc: Tries to do a clean shut-down of the engine. 
 *  Info: 'defName' is an definition name, for that the attribute should be assigned
 *        'attrName' is the attribute name
 *        'attrVal' is the OPTIONAL attribute value
 *  Para: const uint8_t *argsText  -> prt to attr command args text "defName attrName attrVal"
 *        const size_t argsTextLen -> attr command args text length
 *  Rets: struct headRetMsgMultiple_s -> STAILQ head of multiple retMsg, if NULL -> no retMsg-entry
 * -------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s ICACHE_FLASH_ATTR
Shutdown_CommandFn (const uint8_t *argsText
		,const size_t argsTextLen)
{
  #if Shutdown_Command_DBG >= 7
  SCDEFn->Log3Fn(Shutdown_ProvidedByCommand.commandNameText
	,Shutdown_ProvidedByCommand.commandNameTextLen
	,7
	,"CommandFn called with args '%.*s'"
	,argsTextLen
	,argsText);
  #endif

// --------------------------------------------------------------------------------------------------

  // prepare STAILQ head for multiple RetMsg storage
  struct headRetMsgMultiple_s headRetMsgMultiple;

  // Initialize the queue
  STAILQ_INIT(&headRetMsgMultiple);

  // set start of possible def-Name-Text
  const uint8_t *defNameText = argsText;

  // set start of possible attr-Name-Text
  const uint8_t *attrNameText = argsText;

  // a seek-counter
  int i = 0;

  // seek to next space '\32'
  while( (i < argsTextLen) && (*attrNameText != ' ') ) {i++;attrNameText++;}

  // length of def-Name-Text
  size_t defNameTextLen = i;

  // seek to start position of attr-Name-Text '\32'
  while( (i < argsTextLen) && (*attrNameText == ' ') ) {i++;attrNameText++;}

  // set start of possible attr-Val
  const uint8_t *attrValText = attrNameText;

  // a 2nd seek-counter
  int j = 0;

  // seek to next space '\32'
  while( (i < argsTextLen) && (*attrValText != ' ') ) {i++,j++;attrValText++;}

  // length of attr-Name-Text
  size_t attrNameTextLen = j;

  // start position of attr-Val-Text
  while( (i < argsTextLen) && (*attrValText == ' ') ) {i++;attrValText++;}

  // length of attr-Val-Text
  size_t attrValTextLen = argsTextLen - i;

// -------------------------------------------------------------------------------------------------
/*
  // veryfy lengths > 0, definition 0 allowed
  if ( (defNameTextLen == 0) || (attrNameTextLen == 0) ) {

	// alloc mem for retMsg
	strTextMultiple_t *retMsg =
		 malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Could not interpret command '%.*s'! Usage: Attr <devspec> <attrName> [<attrVal>]"
		,argsTextLen
		,argsText);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;
  }
*/
// -------------------------------------------------------------------------------------------------

//  DoTrigger("global", "SHUTDOWN", 1);

//  Log 0, "Server shutdown";

// -------------------------------------------------------------------------------------------------

  strTextMultiple_t *retMsg = NULL;

  // call ShutdownFn (if assigned) for ALL definitions to execute shutdown
  Common_Definition_t *Common_Definition = STAILQ_FIRST(&SCDERoot->HeadCommon_Definitions);
  while (Common_Definition != NULL) {

	if (Common_Definition->module->ProvidedByModule->ShutdownFn) {

		retMsg = Common_Definition->module->ProvidedByModule->ShutdownFn(Common_Definition);

		// got an error msg?
		if (retMsg) {

			// insert retMsg in stail-queue
			STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

		}

	}

	// get next Common_Definition for processing
	Common_Definition = STAILQ_NEXT(Common_Definition, entries);
  }


// -------------------------------------------------------------------------------------------------

  // return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
  return headRetMsgMultiple;

}



