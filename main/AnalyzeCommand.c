#include "ProjectConfig.h"
#include <esp8266.h>
#include "SCDE_s.h"

#include "SCDE.h"



/* --------------------------------------------------------------------------------------------------
 *  FName: AnalyzeCommand
 *  Desc: Seeks command and commandArgs from given args and calls the commandFn from an matching 
 *        module name (if loaded / available)
 *  Info: 
 *  Para: const uint8_t *args  -> ptr to space seperated "command args" text
 *        const size_t argsLen -> length of command and args
 *  Rets: struct headRetMsgMultiple_s -> head from STAILQ, stores multiple RetMsg (errors), NULL=OK/None
 * --------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s
AnalyzeCommand(const uint8_t *args
		, const size_t argsLen)
{
/*
	LOGD("Fn AnalyzeCommand(%.*s,%d) called.\n"
		,argsLen
		,args
		,argsLen);
*/
  // prepare STAILQ head for multiple RetMsg storage
  struct headRetMsgMultiple_s headRetMsgMultiple;

  // initialize the queue
  STAILQ_INIT(&headRetMsgMultiple);

  // set start of possible Command
  const uint8_t *commandName = args;

  // set start of possible Command-Arguments
  const uint8_t *commandArgs = args;

  // a seek-counter
  int i = 0;

  // seek to next space !'\32'
  while( (i < argsLen) && (*commandArgs != ' ') ) {i++;commandArgs++;}

  // length of command
  size_t commandLen = i;

  // seek to start position of Command-Arguments '\32'
  while( (i < argsLen) && (*commandArgs == ' ') ) {i++;commandArgs++;}

  // length of Command-Arguments
  size_t commandArgsLen = argsLen - i;

 // veryfy lengths > 0, definition 0 allowed
//  if ( (commandLen) == 0 || (commandArgsLen == 0) ) {
  if (commandLen == 0 ) {

	// alloc mem for retMsg
	strTextMultiple_t *retMsg =
		 malloc(sizeof(strTextMultiple_t));

	// fill retMsg with error text
	retMsg->strTextLen = asprintf((char**) &retMsg->strText
		,"Could not interpret input '%.*s' ! Usage: <command> <command dependent arguments>"
		,(int) argsLen
		,args);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;

	}

  // get the Command by Name
  Command_t *command;

  // search for the command
  STAILQ_FOREACH(command, &SCDERoot.headCommands, entries) {

		if ( (command->provided->commandNameTextLen == commandLen)
			&& (!strncasecmp((const char*) command->provided->commandNameText, (const char*) commandName, commandLen)) ) {

			// call the CommandFn, if retMsg != NULL -> error ret Msg
			struct headRetMsgMultiple_s headRetMsgMultipleFromFn
				= command->provided->CommandFn(commandArgs, commandArgsLen);

			// retMsgMultiple stailq from Fn filled ?
			if (!STAILQ_EMPTY(&headRetMsgMultipleFromFn)) {

				// for the retMsg elements
				strTextMultiple_t *retMsg;

				// get the entries till empty
				while (!STAILQ_EMPTY(&headRetMsgMultipleFromFn)) {

					retMsg = STAILQ_FIRST(&headRetMsgMultipleFromFn);

					// insert retMsg in stail-queue
					STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

					// done, remove this entry
					STAILQ_REMOVE(&headRetMsgMultipleFromFn, retMsg, strTextMultiple_s, entries);
				}
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
