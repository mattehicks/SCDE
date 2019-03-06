

// set default build verbose - if no external override
#ifndef CORE_SCDE_DBG  
#define CORE_SCDE_DBG  5	// 5 is default
#endif 

// set default build verbose - if no external override
#ifndef Helpers_SCDE_DBG  
#define Helpers_SCDE_DBG  5	// 5 is default
#endif 





#include "ProjectConfig.h"
#include <esp8266.h>
#include "SCDE_s.h"

#include "AnalyzeCommandChainFn.h"




//0d 0a problem cr lf
/* --------------------------------------------------------------------------------------------------
 *  FName: AnalyzeCommandChain
 *  FName: AnalyzeCommand
 *  Desc: Seeks command and commandArgs from given args and calls the commandFn from an matching 
 *  AnalyzeCommandChain() ermöglicht die Ausführung von FHEM-Befehlsketten. 
 *  Dabei werden mehrere FHEM-Kommandos durch ein Semikolon getrennt und nacheinander mittels AnalyzeCommand() ausgeführt.
 *        module name (if loaded / available)
 *  Info: define dummy1 dummy; list dummy1; {Log(3,"dummy created")}
 *  Para: const uint8_t *args  -> ptr to space seperated "command args" text
 *        const size_t argsLen -> length of command and args
 *  Rets: struct headRetMsgMultiple_s -> head from STAILQ, stores multiple RetMsg (errors), NULL=OK/None

 *  Para: const uint8_t *argsString -> ptr to arguments string (SCDE commands sperated by ';')
 *        const size_t argsStringLen -> length of arguments string
 *  Rets: struct xHeadMultipleStringSLTQ_s -> singly linked tail queue head to store multiple 
 *                                            return message strings. Loop entrys till STAILQ_EMPTY!
 * --------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s //new xHeadMultipleStringSLTQ_s
AnalyzeCommandChain(const uint8_t *argsString
		,const size_t argsStringLen)
{
  LOGD("Fn AnalyzeCommandChain called with args '%.*s'"
	,argsStringLen
	,argsString);

  // prepare STAILQ head for multiple RetMsg storage
  struct headRetMsgMultiple_s headRetMsgMultiple;

  // Initialize the queue
  STAILQ_INIT(&headRetMsgMultiple);

  // set start of possible Command
  const uint8_t *commandName = argsString;

  // set start of possible Command-Arguments
  const uint8_t *commandArgs = argsString;

  // a seek-counter
  int i = 0;

  // seek to next space !'\32'
  while( (i < argsStringLen) && (*commandArgs != ' ') ) {i++;commandArgs++;}

  // length of Name
  size_t commandLen = i;

  // seek to start position of Command-Arguments '\32'
  while( (i < argsStringLen) && (*commandArgs == ' ') ) {i++;commandArgs++;}

  // length of Command-Arguments

  size_t commandArgsLen = argsStringLen - i;

// -------------------------------------------------------------------------------------------------

 // veryfy lengths > 0, definition 0 allowed
//  if ( (commandLen) == 0 || (commandArgsLen == 0) ) {
  if ( commandLen == 0 ) {

	// alloc mem for retMsg
	strTextMultiple_t *retMsg =
		malloc(sizeof(strTextMultiple_t));

	// fill retMsg with error text
	retMsg->strTextLen = asprintf((char**) &retMsg->strText
		,"Could not interpret input '%.*s' ! Usage: <command> <command dependent arguments>"
		,(int) argsStringLen
		,argsString);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;
  }

// -------------------------------------------------------------------------------------------------

  // Command SLTQE
  command_t *command;

  // loop the implemented commands
  STAILQ_FOREACH(command, &SCDERoot.headCommands, entries) {

	// matching Command Name ?
	if ( (command->providedByCommand->commandNameTextLen == commandLen)
		&& (!strncasecmp((const char*) command->providedByCommand->commandNameText
			,(const char*) commandName
			,commandLen)) ) {

		// call the CommandFn, if retMsg != NULL -> error ret Msg
		struct headRetMsgMultiple_s headRetMsgMultipleFromFn
			= command->providedByCommand->commandFn(commandArgs, commandArgsLen);

		// retMsgMultiple stailq from Fn filled ?
		while (!STAILQ_EMPTY(&headRetMsgMultipleFromFn)) {

			// for the retMsg elements
			strTextMultiple_t *retMsg =
				STAILQ_FIRST(&headRetMsgMultipleFromFn);

			// first remove this entry from Fn ret queue
			STAILQ_REMOVE(&headRetMsgMultipleFromFn, retMsg, strTextMultiple_s, entries);

			// last insert entry in ret queue
			STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);
		}

	// return STAILQ head, it stores multiple RetMsg, if NULL -> no RetMsg-entries
	return headRetMsgMultiple;
	}
  }

// -------------------------------------------------------------------------------------------------

  // alloc mem for retMsg
  strTextMultiple_t *retMsg =
	 malloc(sizeof(strTextMultiple_t));

  // fill retMsg with error text
  retMsg->strTextLen = asprintf((char**) &retMsg->strText
	,"Unknown command <%.*s> with arguments <%.*s>!"
	,(int) commandLen
	,commandName
	,(int) commandArgsLen
	,commandArgs);

  // insert retMsg in stail-queue
  STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

  // return STAILQ head, stores multiple RetMsg, if NULL -> no RetMsg-entries
  return headRetMsgMultiple;
}

