/* #################################################################################################
 *
 *  Function: Help Command for SCDE (Smart Connected Device Engine)
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

#include "ProjectConfig.h"

#if defined(ESP_PLATFORM)


#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include <esp_err.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_wifi.h>
//#include <spiffs.h>

//#include "lwip/err.h"
//#include "lwip/arch.h"
//#include "lwip/api.h"
//#include "lwip/sockets.h"

//#include "duktape_spiffs.h"
//#include "esp32_specific.h"


//#include <esp8266.h>
//#include "Platform.h"


#include "lwip/ip.h"

#include <stdint.h>
#include "esp_attr.h"
#include "esp_deep_sleep.h"
//#include "esp_err.h" bug mit assert
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_heap_alloc_caps.h"
#include "esp_intr.h"
#include "esp_ipc.h"
#include "esp_ssc.h"
#include "esp_system.h"
#include "esp_task.h"
#include "esp_wifi.h"
#include "esp_types.h"
#include "esp_wifi_types.h"
#include "esp_log.h"
//#include "heap_alloc_caps.h"

#include "sdkconfig.h"

#endif // ESP_PLATFORM

#define _GNU_SOURCE
#include <assert.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>

//#include "scde_task.h"
#include "logging.h"
#include "SCDE_s.h"
//#include "SCDE_Main.h"
//#include "SCDE.h"



#include "Help_Command.h"

LOG_TAG("Help_Command");









/*
#include <ProjectConfig.h>
#include <esp8266.h>
#include <Platform.h>

#include <SCDE_s.h>

#include "Help_Command.h"
*/



// set default build verbose - if no external override
#ifndef Help_Command_DBG  
#define Help_Command_DBG  5	// 5 is default
#endif 



// -------------------------------------------------------------------------------------------------

// make data root locally available
static SCDERoot_t* SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for faster operation
static SCDEFn_t* SCDEFn;

// -------------------------------------------------------------------------------------------------



/* --------------------------------------------------------------------------------------------------
 *  DName: Help_ProvidedByCommand
 *  Desc: Data 'Provided By Command' for this Command (functions + infos this command provides
 *        to SCDE)
 *  Data: 
 * --------------------------------------------------------------------------------------------------
 */

// Command Help
const uint8_t Help_helpText[] = 
  "Usage: define <name> <type> <options>, to define a device";
// CommandHelp (detailed)
const uint8_t Help_helpDetailText[] = 
  "Usagebwrebwerb: define <name> <type> <options>, to define a device";

providedByCommand_t Help_ProvidedByCommand =
  {
   "Help"					// Command-Name of command -> libfilename.so !
  ,4						// length of cmd
  ,Help_InitializeCommandFn			// Initialize Fn
  ,Help_CommandFn				// the Fn code
  ,&Help_helpText[0]
  ,sizeof(Help_helpText)
  ,&Help_helpDetailText[0]
  ,sizeof(Help_helpDetailText)
  };

//(const uint8_t *) "Usage: define <name> <type> <options>, to define a device",57);	// CommandHelp, Len


/* --------------------------------------------------------------------------------------------------
 *  FName: Help_InitializeCommandFn
 *  Desc: Initializion of SCDE Function Callback of an new loaded command
 *  Info: Stores Command-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
Help_InitializeCommandFn(SCDERoot_t* SCDERootptr)
  {

  // make data root locally available
  SCDERoot = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn = SCDERootptr->SCDEFn;

  SCDEFn->Log3Fn(Help_ProvidedByCommand.commandNameText
		  ,Help_ProvidedByCommand.commandNameTextLen
		  ,3
		  ,"InitializeFn called. Command '%.*s' now useable.\n"
		  ,Help_ProvidedByCommand.commandNameTextLen
		  ,Help_ProvidedByCommand.commandNameText);

  return 0;

  }





/* --------------------------------------------------------------------------------------------------
 *  FName: Help_Command
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
struct headRetMsgMultiple_s
Help_CommandFn (const uint8_t *args
		,const size_t argsLen)
  {

  // prepare STAILQ head for multiple RetMsg storage
  struct headRetMsgMultiple_s headRetMsgMultiple;

  // Initialize the queue
  STAILQ_INIT(&headRetMsgMultiple);

  // set start of possible Help-Arguments
  const uint8_t *helpArgs = args;

  // a seek-counter
  int i = 0;

  // seek to start position of Help-Arguments
  while( (i < argsLen) && (*helpArgs == ' ') ) {i++;helpArgs++;}

   // length of Help-Arguments
  size_t helpArgsLen = argsLen - i;

/*
  // veryfy lengths > 0
  if ( (nameLen) == 0 || (setArgsLen == 0) ) {

	// alloc mem for retMsg
	strTextMultiple_t *retMsg =
		 malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
				,"Error! Could not interpret set command arguments '%.*s'! Usage: set <name> <type dependent arguments>"
			   	,argsLen
				,args);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;

  }

*/




  // build the default return message
  size_t strBufferLen = 0;
  size_t strBufferOffset; //
  strTextMultiple_t *retMsg = NULL;

  // set write buffer start
  char *wBufStart = NULL;

  do {

	// do we have already a requested-buffer-length, then alloc
	if (strBufferLen) {

		// alloc mem for retMsg
		retMsg = malloc(sizeof(strTextMultiple_t));

		// the buffer for the answer
		retMsg->strText = malloc(strBufferLen);
		wBufStart = (char *) retMsg->strText;
		retMsg->strTextLen = strBufferLen;

	}

	// reset write buffer counter
	strBufferOffset = 0;



	// start building intro
	strBufferOffset += snprintf(wBufStart + strBufferOffset
		,strBufferLen
		,"Overview of activated and loaded commands:\r\n\r\n"
		 "Command        Parameter\r\n"
		 "               Description\r\n"
		 "----------------------------------------------------------------------\r\n");

	// loop through  currently stored commands
	command_t *command;

	// loop throug available commands and determine length
	STAILQ_FOREACH(command, &SCDERoot->headCommands, entries) {

//displayattr    <devspec> [attrname] 
//               display attributes 

			strBufferOffset += snprintf(wBufStart + strBufferOffset
				,strBufferLen
				,"%.*s%.*s%.*s\r\n"
				 "%.*s\r\n"
				,(int) command->providedByCommand->commandNameTextLen
				,command->providedByCommand->commandNameText
				,(int) (15 - command->providedByCommand->commandNameTextLen)
				,"               "
				,(int) command->providedByCommand->helpTextLen
				,command->providedByCommand->helpText	
				,(int) command->providedByCommand->helpDetailTextLen
				,command->providedByCommand->helpDetailText);

		}

	// building end-info
	strBufferOffset += snprintf(wBufStart + strBufferOffset
		,strBufferLen
		,"\r\nType help <command> for detailed help.");


	// store reqired len
	strBufferLen = strBufferOffset;

	} while (!retMsg);

//  printf("strBufferOffset %d\r\n"
//	,strBufferOffset);

  // insert retMsg in stail-queue
  STAILQ_INSERT_HEAD(&headRetMsgMultiple, retMsg, entries);

  // temp info
  if (helpArgsLen) {

	printf("got extra args for help cmd '%.*s' for command execution!"
 		,(int) helpArgsLen
		,helpArgs);
  }

  // return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
  return headRetMsgMultiple;

  }









  // alloc mem for response message
//  retMsg = malloc(helpLen);



/*		// response with error text
		asprintf(&retMsg
				,"Could not interpret command '%.*s'! Usage: define <name> <type-name> <type dependent arguments>"
			   	,argsLen
				,args);

		return retMsg;
*/


/*




  // for Fn response msg
  char *retMsg = NULL;

  // set start of possible Name
  const uint8_t *name = args;

  // set start of possible Type-Name
  const uint8_t *typeName = args;

  // a seek-counter
  int i = 0;

  // seek to next space !'\32'
  while( (i < argsLen) && (*typeName != ' ') ) {i++;typeName++;}

  // length of Name
  size_t nameLen = i;

  // seek to start position of Type-Name '\32'
  while( (i < argsLen) && (*typeName == ' ') ) {i++;typeName++;}

  // set start of possible Definition
  const uint8_t *definition = typeName;

  // a 2nd seek-counter
  int j = 0;

  // seek to next space !'\32'
  while( (i < argsLen) && (*definition != ' ') ) {i++,j++;definition++;}

  // length of Type-Name
  size_t typeNameLen = j;

  // start position of Definition
  while( (i < argsLen) && (*definition == ' ') ) {i++;definition++;}

  // length of Type-Name
  size_t definitionLen = argsLen - i;

  // veryfy lengths > 0, definition 0 allowed
  if ( (nameLen) == 0 || (typeNameLen == 0) )

		{

		// response with error text
		asprintf(&retMsg
				,"Could not interpret command '%.*s'! Usage: define <name> <type-name> <type dependent arguments>"
			   	,argsLen
				,args);

		return retMsg;

		}



   // get the module ptr by name
   Module_t* Module = SCDEFn->GetLoadedModulePtrByNameFn(typeName
		   , typeNameLen);

  // do we need to reload Module?
  if (!Module) // NOT FUNCTIONAL!
	Module = SCDEFn->CommandReloadModuleFn(typeName
		   , typeNameLen);

  // alloc mem for modul specific definition structure (Common_Definition_t + X)
  Common_Definition_t *NewCommon_Definition
	= malloc(Module->ProvidedByModule->SizeOfDefinition);

  // zero the struct
  memset(NewCommon_Definition, 0, Module->ProvidedByModule->SizeOfDefinition);

  // prepare default fields in definition

  // copy ptr to associated module
  NewCommon_Definition->module = Module;

  // copy custom name
  NewCommon_Definition->name = malloc(nameLen);
  memcpy(NewCommon_Definition->name, name, nameLen);
  NewCommon_Definition->nameLen = nameLen;

  // store new definition to SCDE-Root
  STAILQ_INSERT_HEAD(&SCDERoot->HeadCommon_Definitions, NewCommon_Definition, entries);

  printf("Defined Name:%.*s TypeName:%.*s\n"
		,NewCommon_Definition->nameLen
  		,NewCommon_Definition->name
  		,NewCommon_Definition->module->ProvidedByModule->typeNameLen
		,NewCommon_Definition->module->ProvidedByModule->typeName);

  // store Definition string in Defp->Definition
  NewCommon_Definition->definition = malloc(definitionLen);
  memcpy(NewCommon_Definition->definition, definition, definitionLen);
  NewCommon_Definition->definitionLen = definitionLen;

  // assign an internal number
  NewCommon_Definition->nr = SCDERoot->DevCount++;

  // do we need initial state? or NULL ? store initial state
  NewCommon_Definition->stateLen = asprintf((char**)&NewCommon_Definition->state, "???");

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

  // call modules DefineFn, if retMsg != NULL -> veto
  retMsg = NewCommon_Definition->module->ProvidedByModule->DefineFn(NewCommon_Definition);

  // do we have an error msg? Interpret it as veto!
  if (retMsg)

	{

	printf("Got error from DefineFN %s \n"
			  ,retMsg);

	//Log 1, "define $def: $ret";
//	DeleteDefinition(Name);	//delete $defs{$name};                            # Veto
//	DeleteAttribute(Name);	//delete $attr{$name};

	}

  // char error* or NULL
  return retMsg;
*/
 // }




//samsung CB6-X5RD-DMPP-8358


/*
Possible commands: 

Command        Parameter 
               Description 
---------------------------------------------------------------------- 
IF             use "help IF" for more help 

apptime        use "help apptime" for more help 

attr           <devspec> <attrname> [<attrval>] 
               set attribute for <devspec> 
attributes     use "help attributes" for more help 

cancel         [<id> [quiet]] 
               list sleepers, cancel sleeper with <id> 
command        use "help command" for more help 

commands       use "help commands" for more help 

createlog      <device> 
               create log/weblink for <device> 
define         <name> <type> <options> 
               define a device 
defmod         <name> <type> <options> 
               define or modify a device 
delete         <devspec> 
               delete the corresponding definition(s) 
deleteattr     <devspec> [<attrname>] 
               delete attribute for <devspec> 
deletereading  <devspec> [<attrname>] 
               delete user defined reading for <devspec> 
devspec        use "help devspec" for more help 

displayattr    <devspec> [attrname] 
               display attributes 
get            <devspec> <type dependent> 
               request data from <devspec> 
global         use "help global" for more help 

help           [<moduleName>] 
               get help (this screen or module dependent docu) 
include        <filename> 
               read the commands from <filenname> 
iowrite        <iodev> <data> 
               write raw data with iodev 
list           [devspec] 
               list definitions and status info 
modify         device <options> 
               modify the definition (e.g. at, notify) 
notice         use "help notice" for more help 

perl           use "help perl" for more help 

reload         <module-name> 
               reload the given module (e.g. 99_PRIV) 
rename         <old> <new> 
               rename a definition 
rereadcfg      [configfile] 
               read in the config after deleting everything 
restore        [list] [<filename|directory>] 
               restore files saved by update 
save           [configfile] 
               write the configfile and the statefile 
set            <devspec> <type dependent> 
               transmit code for <devspec> 
setdefaultattr <attrname> <attrvalue> 
               set attr for following definitions 
setreading     <devspec> <reading> <value> 
               set reading for <devspec> 
setstate       <devspec> <state> 
               set the state shown in the command list 
shutdown       [restart|exitValue] 
               terminate the server 
sleep          <sec> [<id>] [quiet] 
               sleep for sec, 3 decimal places 
trigger        <devspec> <state> 
               trigger notify command 
update         [<fileName>|all|check|force] [http://.../controlfile] 
               update FHEM 
usb            [scan|create] 
               display or create fhem-entries for USB devices 
*/















