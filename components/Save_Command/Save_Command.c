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



// set default build verbose - if no external override
#ifndef Save_Command_DBG  
#define Save_Command_DBG  5	// 5 is default
#endif 



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

// Command Help
const uint8_t Save_helpText[] =
  "Usage: save <ConfigFileName>, to write the <ConfigFile> and statefile";
// CommandHelp (detailed)
const uint8_t Save_helpDetailText[] =
  "Usagebwrebwerb: save <filename>, to write the <configfile> and statefile";


ProvidedByCommand_t Save_ProvidedByCommand = {
  "Save",			// command-name text -> libfilename.so !
  sizeof("Save")-1,		// command-name text length
  Save_InitializeCommandFn,	// Initialize Fn
  Save_CommandFn,		// the Fn code
  { &Save_helpText, sizeof(Save_helpText) },
  { &Save_helpDetailText, sizeof(Save_helpDetailText) }
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

// --------------------------------------------------------------------------------------------------

  #if Save_Command_DBG >= 5
  SCDEFn->Log3Fn(Save_ProvidedByCommand.commandNameText
	,Save_ProvidedByCommand.commandNameTextLen
	,5
	,"InitializeFn called. Now useable.");
  #endif

// --------------------------------------------------------------------------------------------------

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
  #if Save_Command_DBG >= 7
  SCDEFn->Log3Fn(Save_ProvidedByCommand.commandNameText
	,Save_ProvidedByCommand.commandNameTextLen
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
  const uint8_t *fileNameText = argsText;

  // length of def-Name-Text
  size_t fileNameTextLen = argsTextLen;

// -------------------------------------------------------------------------------------------------

/* valid Filename/content check ?
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
*/
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
	
// Unklar
//restoreDir_saveFile($restoreDir, $attr{global}{statefile});
	
/*
  // !! correct to default LOG Fn!
  printf("DebugA %.*s\n"
	,fileNameTextLen
	,fileNameText);
*/


	// call WriteStatefileFn
	struct headRetMsgMultiple_s headRetMsgMultipleFromFn
		= SCDEFn->WriteStatefileFn();
	
  // if RetMsgMultiple queue filled -> error
  if (!STAILQ_EMPTY(&headRetMsgMultipleFromFn)) {

		// get the queue entries from retMsgMultiple till empty
		while (!STAILQ_EMPTY(&headRetMsgMultipleFromFn)) {

			// get a retMsg element
			strTextMultiple_t *retMsg =
				STAILQ_FIRST(&headRetMsgMultipleFromFn);
/*
			printf("moving RetMsgFromFn: %.*s\n"
				,retMsg->strTextLen
				,retMsg->strText);
*/
			// insert retMsg element in main ret msg returning queue
			STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

			// done, remove this entry
			STAILQ_REMOVE_HEAD(&headRetMsgMultipleFromFn, entries);

		}

 	 // return STAILQ head, stores multiple retMsg entries
 	 return headRetMsgMultiple;

	}

	// state file written, now continue with config file

	// verify NameTextLen > 0, if still 0 -> try to get it from Attribute

		// get attribute "global->configfile" value
		strText_t attrConfigFNDefName = {(char*) "global", 6};
		strText_t attrConfigFNAttrName = {(char*) "configfile", 10};
		strText_t *attrConfigFNValueName =
			SCDEFn->Get_attrVal_by_defName_and_attrNameFn(&attrConfigFNDefName, &attrConfigFNAttrName);

		// veryfy length > 0, NameTextLen still 0 -> get it from Attribute
		if (fileNameTextLen == 0) {
	
			// assigned Attribute found ?
			if (attrConfigFNValueName) {

				// with Value ?
				if (attrConfigFNValueName->strText) {

					// and its not an empty Value (has >0 chars) ?
					if (attrConfigFNValueName->strTextLen) {

						// case: configfile attribute set, use it
						fileNameTextLen =
							attrConfigFNValueName->strTextLen;
						fileNameText =
							(uint8_t *) attrConfigFNValueName->strText;
					}
				}
			}
		}







	

	// case: no configfile attribute set and no command argument specified (Len = 0)

	  // veryfy length > 0, NameTextLen still 0 -> not allowed
		if (fileNameTextLen == 0) {

			// alloc mem for retMsg
			strTextMultiple_t *retMsg =
				 malloc(sizeof(strTextMultiple_t));

			// response with error text
			retMsg->strTextLen = asprintf(&retMsg->strText
				,"No configfile attribute set and no argument specified");

			// insert retMsg in stail-queue
			STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

			// free attribute-data from Get_attrVal_by_defName_and_attrNameFn
			if (attrConfigFNValueName->strText) free(attrConfigFNValueName->strText);	 
			if (attrConfigFNValueName) free(attrConfigFNValueName);
 
			// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
			return headRetMsgMultiple;
		}

//#restoreDir_saveFile($restoreDir, $param);

	// create configfilename string
	char *configFile;
	asprintf(&configFile
			,"/spiffs/%.*s.cfg"
			,fileNameTextLen
			,fileNameText);

	// open configfile
	FILE* cFH = fopen(configFile, "w");
	if (cFH == NULL) {

		// alloc mem for retMsg
		strTextMultiple_t *retMsg =
			malloc(sizeof(strTextMultiple_t));

		// response with error text
		retMsg->strTextLen = asprintf(&retMsg->strText
			,"Error, could not open $configFile: %s!\r\n"
			,configFile);

//   #Log 1, $errormsg; ???

		// free filename-data
		free(configFile);

		// free attribute-data from Get_attrVal_by_defName_and_attrNameFn
		if (attrConfigFNValueName->strText) free(attrConfigFNValueName->strText);	 
		if (attrConfigFNValueName) free (attrConfigFNValueName);

		// insert retMsg in stail-queue
		STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

		// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
		return headRetMsgMultiple;
	}

	// free filename-data
	free(configFile);







/*
	// stores the time
	struct tm timeinfo;

 time_t timeNow;

 // assign time stamp
  time(&timeNow);

  // get time
	localtime_r(&timeNow, &timeinfo);

  // to fill with: "Sat Aug 19 14:16:59 2017"
	char strftime_buf[64];

 // get strftime-text into strftime_buf 
	strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);

  // start statefile with date:-> #Sat Aug 19 14:16:59 2017
  fprintf(cFH,"#%s\r\n", strftime_buf);
*/






  // loop through definitions unique number to process them in order of creation
  for (uint32_t i = 0 ; i < SCDERoot->DevCount ; i++) {




	// its a comment, add it
//#if(!defined($d)) {
//#     print $fh $h->{TEXT},"\n";
//#      next;
//#    }





		Common_Definition_t *Common_Definition;

		// loop through definitions to find matching unique nr
		STAILQ_FOREACH(Common_Definition, &SCDERoot->HeadCommon_Definitions, entries) {


			// if this Definition matches current unique number -> process it
			if (
				(Common_Definition->nr == i) &&												// the current unique number
				(!(Common_Definition->defCtrlRegA & F_TEMPORARY)) &&	// and NO temporary definition
				(!(Common_Definition->defCtrlRegA & F_VOLATILE))			// and NO volatile definition
				 ) {
/*
				// store Definition and Attribute
				printf("calling GetDefAndAttr for:%.*s\n"
					,Common_Definition->nameLen
					,Common_Definition->name);
*/
				// get .CFG lines for definition and attribute setup
				struct headRetMsgMultiple_s headRetMsgMultipleFromFn =
					SCDEFn->GetDefAndAttrFn(Common_Definition);

				// if RetMsgMultiple queue not empty -> got lines to add to .CFG
				if (!STAILQ_EMPTY(&headRetMsgMultipleFromFn)) {

					// get the queue entries from retMsgMultiple till empty
					while (!STAILQ_EMPTY(&headRetMsgMultipleFromFn)) {

						// get a retMsg element from queue
						strTextMultiple_t *retMsg =
							STAILQ_FIRST(&headRetMsgMultipleFromFn);
/*
						printf("store def or attr line to File: %.*s\n"
							,retMsg->strTextLen
							,retMsg->strText);
*/
						// store def or attr line to file
						fprintf(cFH,"%.*s\n"
							,retMsg->strTextLen
							,retMsg->strText);

						// done, remove this entry
						STAILQ_REMOVE_HEAD(&headRetMsgMultipleFromFn, entries);

						free(retMsg->strText);
						free(retMsg);
					}
				}
			}
		}
  }

	// close configfile
	fclose(cFH);

	// show filecontent on debug term
	int c;
	FILE *file;
	file = fopen("/spiffs/maker.cfg", "r");
	if (file) {
    while ((c = getc(file)) != EOF)
        putchar(c);
    fclose(file);
	}

	// finnish cmd save with confirmation string

		// alloc mem for an retMsg
		strTextMultiple_t *retMsg =
			malloc(sizeof(strTextMultiple_t));

		// response with error text
		retMsg->strTextLen = asprintf(&retMsg->strText
			,"Wrote configuration to %.*s!\r\n"
			,fileNameTextLen
			,fileNameText);

		// insert retMsg in queue
		STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

		// free attribute-data from Get_attrVal_by_defName_and_attrNameFn
		if (attrConfigFNValueName->strText) free(attrConfigFNValueName->strText);	 
		if (attrConfigFNValueName) free (attrConfigFNValueName);

	// return STAILQ head, stores multiple retMsg, if STAILQ_EMPTY -> no retMsg-entries
	return headRetMsgMultiple;
}

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






















