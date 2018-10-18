/* #################################################################################################
 *
 *  Function: Save Command for SCDE (Smart Connected Device Engine)
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

#include "Save_Command.h"



// -------------------------------------------------------------------------------------------------

// make data root locally available
static SCDERoot_t* SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for operation
static SCDEFn_t* SCDEFn;

// -------------------------------------------------------------------------------------------------



/*
 * -------------------------------------------------------------------------------------------------
 *  DName: Save_Command
 *  Desc: Data 'Provided By Command' for the Save Command (functions + infos this command provides
 *        to SCDE)
 *  Data: 
 * -------------------------------------------------------------------------------------------------
 */
const uint8_t Save_CMDHelpText[] =
	"Usage: save <ConfigFileName>, to write the <ConfigFile> and statefile";	// CommandHelp, Len
const uint8_t Save_CMDHelpDetailText[] =
	"Usagebwrebwerb: save <filename>, to write the <configfile> and statefile";	// CommandHelp, Len


providedByCommand_t Save_ProvidedByCommand =
  {
   "save"			// command-name text -> should be same name as libfilename.so !
  ,4				// command-name text length
  ,Save_InitializeCommandFn	// Initialize Fn
  ,Save_CommandFn		// the Fn code
  ,&Save_CMDHelpText
  ,sizeof(Save_CMDHelpText)
  ,&Save_CMDHelpDetailText
  ,sizeof(Save_CMDHelpDetailText)
  };



/* --------------------------------------------------------------------------------------------------
 *  FName: Save_Initialize
 *  Desc: Initializion of SCDE Function Callback of an new loaded command
 *  Info: Stores Command-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
Save_InitializeCommandFn(SCDERoot_t* SCDERootptr)
  {

  // make data root locally available
  SCDERoot = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn = SCDERootptr->SCDEFn;

  SCDEFn->Log3Fn(Save_ProvidedByCommand.commandNameText
		,Save_ProvidedByCommand.commandNameTextLen
	  ,3
	  ,"InitializeFn called. Command '%.*s' now useable.\n"
	  ,Save_ProvidedByCommand.commandNameTextLen
	  ,Save_ProvidedByCommand.commandNameText);

  return 0;

  }



/* -------------------------------------------------------------------------------------------------
 *  FName: Save_CommandFn
 *  Desc: Opens the fileName, loads and splits the content it in multiple lines by '\r\n',
 *        recreates multi-line text by '//' and executes each line.
 *  Info: 'fileName' is the file-name that should be loaded
 *  Para: const uint8_t *argsText  -> prt to Save command args text "fileName"
 *        const size_t argsTextLen -> attr command args text length
 *  Rets: struct headRetMsgMultiple_s -> STAILQ head of multiple retMsg, if NULL -> NO entry
 * -------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s ICACHE_FLASH_ATTR
Save_CommandFn (const uint8_t *argsText
	,const size_t argsTextLen)
  {

  // prepare STAILQ head for multiple RetMsg storage
  struct headRetMsgMultiple_s headRetMsgMultiple;

  // Initialize the queue
  STAILQ_INIT(&headRetMsgMultiple);

  // set start of possible def-Name-Text
  const uint8_t *fileNameText = argsText;

  // length of def-Name-Text
  size_t fileNameTextLen = argsTextLen;

// -------------------------------------------------------------------------------------------------

  // veryfy lengths > 0, definition 0 allowed
  if (fileNameTextLen == 0) {

	// alloc mem for retMsg
	strTextMultiple_t *retMsg =
		 malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Could not interpret command '%.*s'! Usage: save <ConfigFileName>"
		,argsTextLen
		,argsText);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;
  }

// -------------------------------------------------------------------------------------------------

// Argument ? verarbeiten	
// if($param && $param eq "?") {
//    return "No structural changes." if(!@structChangeHist);
//    return "Last 10 structural changes:\n  ".join("\n  ", @structChangeHist);
//  }
	
// skip save, if autosave is disabled 
//	  if(!$cl && !AttrVal("global", "autosave", 1)) { # Forum #78769
//    Log 4, "Skipping save, as autosave is disabled";
//    return;
//  }

	
// need an restore dir
//my $restoreDir;
// $restoreDir = restoreDir_init("save")
	
// clear structural changes	list
//@structChangeHist = ();

// send saved trigger signal
//DoTrigger("global", "SAVE", 1);
	
	 restoreDir_saveFile($restoreDir, $attr{global}{statefile});
  my $ret = WriteStatefile();
	
	// call the CommandFn, if retMsg != NULL -> error ret Msg
	struct headRetMsgMultiple_s headRetMsgMultipleFromFn
	= SCDEFn->WriteStatefileFn();
	
  return $ret if($ret);
  $ret = "";    # cfgDB_SaveState may return undef
	
	
 /*
  FILE *fp = fopen("/data/x", "w");

  // return msg in case of an error
  if (fp == NULL) {

	// alloc mem for retMsg
	strTextMultiple_t *retMsg =
		 malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
	,"should load: %.*s here!\r\n"
	,fileNameTextLen
	,fileNameText);

  // insert retMsg in stail-queue
  STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

  // return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
  return headRetMsgMultiple;

  }

*/

  // !! correct to default LOG Fn!
  printf("Including %.*s\n"
	,fileNameTextLen
	,fileNameText);


/*

1222	  my @t = localtime();
1223	  my $gcfg = ResolveDateWildcards(AttrVal("global", "configfile", ""), @t);
1224	  my $stf  = ResolveDateWildcards(AttrVal("global", "statefile",  ""), @t);


1225	  if(!$init_done && $arg ne $stf && $arg ne $gcfg) {
1226	    my $nr =  $devcount++;
1227	    $comments{$nr}{TEXT} = "include $arg";
1228	    $comments{$nr}{CFGFN} = $currcfgfile		if($currcfgfile ne $gcfg);
1229	  }
*/

















/*

	// get the Common_Definitions for current module
	Common_Definition_t *Common_Definition;

	STAILQ_FOREACH(Common_Definition, &SCDERoot->HeadCommon_Definitions, entries) {


//		strTextMultiple_t *retMsg =
//			SCDEFn->GetAllReadingsFn(Common_Definition);


		// call the CommandFn, if retMsg != NULL -> error ret Msg
		struct headRetMsgMultiple_s headRetMsgMultipleFromFn
			= SCDEFn->GetAllReadingsFn(Common_Definition);

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



}

*/







  // return STAILQ head, queue stores multiple retMsg entries, if NULL -> no retMsg-entry
  return headRetMsgMultiple;

}





































