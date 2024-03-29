﻿
/* TO DO:


     OLD NAME          NEW NAME: 
ToDo valName           attrVal
OK   attrName          attrName
OK   defName           defName

*/







#include "ProjectConfig.h"
#include <esp8266.h>
#include "SCDE_s.h"

#include "SCDE.h"


#if defined(ESP_PLATFORM)


#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
/*
#include <esp_err.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_wifi.h>
*/
//#include <spiffs.h>

//#include "lwip/err.h"
//#include "lwip/arch.h"
//#include "lwip/api.h"
//#include "lwip/sockets.h"

//#include "duktape_spiffs.h"
//#include "esp32_specific.h"



//#include <esp8266.h>

//#include "Platform.h"

/*
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
#include "esp_heap_alloc_caps.h"
*/

#include "esp_log.h"

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





// we are in SCDE
LOG_TAG("SCDE");




/*
#include "ProjectConfig.h"
#include <esp8266.h>
#include "Platform.h"
//#include "SCDED_Platform.h"



// SCDE-includes
#include "SCDE_s.h"
#include "SCDE_Main.h"
#include "SCDE.h"

// Modules to include
#include <WebIf_Module.h>
*/




// samples for standards
// Debug Fn entry:
//	LOGD("Fn AnalyzeCommandChain(%.*s,%d) called\n"
//		,argsLen
//		,args
//		,argsLen);


/*
#if not defined SCDE_VERBOSE
	#warning "Building with default verbose !"
	#define SCDE_VERBOSE = 5
#endif
*/






/*
 * DName: SCDERoot
 * Desc: Stores the Root Data for Smart Connected Devices Engine operation
 * Data: 
 */
SCDERoot_t SCDERoot;



/*
 * DName: SCDEFn (SCDE Functions)
 * Desc: Stores Function callbacks provided & made accessible for modules and commands
 *       for operation and helpers
 * Data: 
 */
SCDEFn_t SCDEFn = {
   AnalyzeCommand
  ,AnalyzeCommandChain
  ,CallGetFnByDefName
  ,CommandReloadModule
  ,Devspec2Array
  ,FmtDateTime
  ,FmtTime
  ,GetAllReadings
  ,Get_Def_And_Attr
  ,GetDefinitionPtrByName
  ,Get_Definitions_That_Match_DefSpec_String
  ,GetLoadedModulePtrByName
  ,GetTiSt
  ,GetUniqueTiSt
  ,GoodDeviceName
  ,GoodReadingName
  ,HexDumpOut
  ,Log 
  ,Log3
  ,Log4
  ,MakeDeviceName
  ,MakeReadingName
  ,readingsBeginUpdate
  ,readingsBulkUpdate
  ,readingsBulkUpdate2
  ,readingsEndUpdate
  ,TimeNow
  ,WriteStatefile

// added Fn (Perl -> C)
  ,Get_Attr_Val_By_Def_Name_And_Attr_Name

// not final
  ,CommandUndefine
  ,ParseKVInputArgs
};









































/** Category: attr-management helper
 * -------------------------------------------------------------------------------------------------
 *  FName: SetAttrValTextByDefTextAttrText
 *  Desc: Sets the attribute value assigned to an attribute-name, for the definition
 *  Para: const strText_t *defName -> ptr to the definition name
 *        const strText_t *attrName -> ptr to the attribute name
 *        const strText_t *valName -> ptr to the value name
 *  Rets: strText_t *attrVal -> the attribute value / NULL = not fnd., attrVal->strText = NULL -> empty
 * -------------------------------------------------------------------------------------------------
 */
/*
bool //ICACHE_FLASH_ATTR
//NEWNAME: Set_valName_for_attrName_and_defName
SetAttrValTextByDefTextAttrText(const strText_t *defName
				,const strText_t *attrName
				,const strText_t *valName)  
{

  // search for the Common_Definition for the requested def-name-text
  Common_Definition_t *Common_Definition = STAILQ_FIRST(&SCDERoot.HeadCommon_Definitions);
  while (Common_Definition != NULL) {

	if ( (Common_Definition->nameLen == defName->strTextLen))
		&& (!strncasecmp((const char*) Common_Definition->name, (const char*) defName->strText, defName->strTextLen)) ) {

		// found, break and keep prt
		break;
	}

	// get next Common_Definition for processing
	Common_Definition = STAILQ_NEXT(Common_Definition, entries);
  }

  // Common_Definition for the requested definition-name not found
  if (Common_Definition == NULL) {

  return FALSE;

  }

// -------------------------------------------------------------------------------------------------

  // make the attrValText writeable / changeable for attributeFn
	size_t newAttrValTextLen;
	uint8_t *newAttrValText;

  // we have a value
  if (attrValTextLen) {

		newAttrValTextLen = attrValTextLen;
		newAttrValText = malloc(attrValTextLen);
		memcpy(newAttrValText, attrValText, attrValTextLen);
	}

	// we have no value
	else {

		newAttrValTextLen = 0;
		newAttrValText = NULL;
	}

// -------------------------------------------------------------------------------------------------








  // loop through the assigned attributes and try to find an existing attribute
  attribute_t *attribute = STAILQ_FIRST(&SCDERoot.headAttributes);

  while (true) {

	// no assigned attribute found ?
	if (attribute == NULL) {

	// not found
	attrVal = NULL;

	return attrVal;

	}

	// this attribute already assigned ?
	if ( (attribute->attrAssignedDef->nameLen == defName->strTextLen)
		&& (!strncasecmp((const char*) attribute->attrAssignedDef->name, (const char*) defName->strText, defName->strTextLen))
		&& (attribute->attrNameTextLen == attrName->strTextLen)
		&& (!strncasecmp((const char*) attribute->attrNameText, (const char*) attrName->strText, attrName->strTextLen)) ) {













		// found reserve memory
		attrVal = malloc(sizeof(strText_t));	

		// assigned attrValText has data, make a copy
		if (attribute->attrValText) {

		attrVal->strText = malloc(attribute->attrValTextLen);

		memcpy(attrVal->strText, attribute->attrValText, attribute->attrValTextLen);

		attrVal->strTextLen = attribute->attrValTextLen;

		}

		// assigned attrValText has NO data
		else {

		attrVal->strText = NULL

		}

		if (attribute->attrValText) {

			printf("Got attrVal:%.*s assigned to attrName:%.*s for defName:%.*s\n"
				,attrVal->strTextLen
				,attrVal->strText
				,attribute->attrNameTextLen
				,attribute->attrNameText
				,attribute->attrAssignedDef->nameLen
				,attribute->attrAssignedDef->name);
		}

		else {

			printf("Got an empty attrVal! No value assigned to attrName:%.*s for defName:%.*s\n"
				,attribute->attrNameTextLen
				,attribute->attrNameText
				,attribute->attrAssignedDef->nameLen
				,attribute->attrAssignedDef->name);

		}

		// found, break
		break;

		}

	// get next attribute for processing
	attribute = STAILQ_NEXT(attribute, entries);
  }

  return attrVal;
}
*/



/** attr helper
 * -------------------------------------------------------------------------------------------------
 *  FName: GetAttrPtrByDefTextAttrText
 *  Desc: Initializes the global device
 *  Para: const strText_t *defName -> ptr to the definition name
 *        const strText_t *attrName -> ptr to the attribute name
 *  Rets: attribute_t* -> ptr to the stored attribute / NULL if not found
 * -------------------------------------------------------------------------------------------------
 */
/*
attribute_t* ICACHE_FLASH_ATTR
GetAttrPtrByDefTextAttrText(const strText_t *defName
				,const strText_t *attrName)  
{
}
*/





/*
 * --------------------------------------------------------------------------------------------------
 *  FName: readingsEndUpdate
 *  Desc: Call readingsEndUpdate when you are done updating readings. This optionally calls DoTrigger
 *        to propagate the changes.
 *  Para: const uint8_t *commandTxt ->  ptr to the Command-Text
 *       const size_t commandTxtLen -> Command-Text length
 *       commandFn_t commandFn -> the command Fn
 *       const uint8_t *commandHelp -> ptr the Command-Help text
 *       const size_t commandHelpLen -> Command-Help text length
 *  Rets: -/-
 * --------------------------------------------------------------------------------------------------
 */
/*


Benötigt:

strText_t	GetAttrValTextByDefTextAttrText(strText_t defName, strText_t attrName);
attribute_t	GetAttrPtrByDefTextAttrText(strText_t defName, strText_t attrName);


attribute_t	SetAttrByDefTextAttrTextValText(strText_t defName, strText_t attrName, strText_t attrValue);
*/


void //ICACHE_FLASH_ATTR
InitA()
{

// Zeile 570 , Initialisierung ...

  // for building the summary of ret-msg from cfg + state file including (starts empty)
  strText_t cfgRet= {NULL, 0};

// -------------------------------------------------------------------------------------------------

  // get attribute configfile value (global->configfile)
  String_t attrCfgFNDefName = {(uint8_t*) "global", 6};
  String_t attrCfgFNAttrName = {(uint8_t*) "configfile", 10};
  String_t* attrCfgFNValueName  =
	Get_Attr_Val_By_Def_Name_And_Attr_Name(&attrCfgFNDefName, &attrCfgFNAttrName);

// -------------------------------------------------------------------------------------------------

  // create the args to execute the 'include configfile' command
  strText_t incCFIncludeCommandArgs;

  incCFIncludeCommandArgs.strTextLen =
	asprintf((char**) &incCFIncludeCommandArgs.strText
		,"include %.*s"
		,(int) attrCfgFNValueName->len
		,(char*) attrCfgFNValueName->p_char);

  // call command include to process the initial config-file
  struct headRetMsgMultiple_s incCFHeadRetMsgMultipleFromFn =
		AnalyzeCommandChain((uint8_t*)incCFIncludeCommandArgs.strText,
		(const size_t) incCFIncludeCommandArgs.strTextLen);

 /* // free value from Fn GetAttrValTextByDefTextAttrText
  if (attrCfgFNValueName) {
	  
		if (attrCfgFNValueName->strText) free(attrCfgFNValueName->strText);

		free(attrCfgFNValueName);

	}*/

  free(incCFIncludeCommandArgs.strText);

// -------------------------------------------------------------------------------------------------

  // in case of strTextMultiple_t queue element add leading "configfile:" text to cfgRet.strText
  if (!STAILQ_EMPTY(&incCFHeadRetMsgMultipleFromFn)) {
	  
		// fill cfgRet.strText with leading "configfile:" text
		cfgRet.strTextLen = asprintf((char**) &cfgRet.strText
			,"configfile: ");

	}

  // loop the queue entries of retMsgMultiple and add them to cfgRet.strText
  while (!STAILQ_EMPTY(&incCFHeadRetMsgMultipleFromFn)) {

		// get an retMsg element from queue
		strTextMultiple_t *retMsg =
			STAILQ_FIRST(&incCFHeadRetMsgMultipleFromFn);

		// reallocate cfgRet.strText memory to new size
		cfgRet.strText = (char *) realloc(cfgRet.strText
			,cfgRet.strTextLen + retMsg->strTextLen);

		// add current retMsg->strText to cfgRet.strText
		memcpy(cfgRet.strText + cfgRet.strTextLen
			,retMsg->strText
			,retMsg->strTextLen);

		// save new retMsg->strTextLen
		cfgRet.strTextLen += retMsg->strTextLen;
	
		// done, remove this entry from queue
		STAILQ_REMOVE_HEAD(&incCFHeadRetMsgMultipleFromFn, entries);
//	STAILQ_REMOVE(&headRetMsgMultipleFromFn, retMsg, strTextMultiple_s, entries);

		// free the allocated retMsg->strText
		free(retMsg->strText);

		// and the allocated strTextMultiple_t queue element
		free(retMsg);

	}

// -------------------------------------------------------------------------------------------------

  // debug info ..
  if (cfgRet.strText) {

	LOGD("cfgRet after reading configfile filled: %.*s"
			,cfgRet.strTextLen
			,(char*) cfgRet.strText);

  }
  else {

	LOGD("cfgRet NOT filled!");
  }

// -------------------------------------------------------------------------------------------------

	// get attribute 'statefile' value (from global->statefile)
  String_t attrStateFNDefName = {(uint8_t*) "global", 6};
  String_t attrStateFNAttrName = {(uint8_t*) "statefile", 9};
  String_t *attrStateFNValueName =
		Get_Attr_Val_By_Def_Name_And_Attr_Name(&attrStateFNDefName, &attrStateFNAttrName);

// -------------------------------------------------------------------------------------------------

  // for the args to execute the 'include statefile' command
  strText_t *incSFIncludeCommandArgs = NULL;

  // attr "statefile" found ?
  if (attrStateFNValueName) {

		// +value found ? 
		if (attrStateFNValueName->len) {

			// build strText_t for cmd args
			incSFIncludeCommandArgs =
				malloc(sizeof(strText_t));

			// attribute "statefile" complete, use it to build args
			incSFIncludeCommandArgs->strTextLen =
				asprintf((char**) &incSFIncludeCommandArgs->strText
					,"include %.*s"
					,(int) attrStateFNValueName->len
					,(char*) attrStateFNValueName->p_char);

			//free(attrStateFNValueName->strText);
		}

 // free(attrStateFNValueName);
	}

  // seems that attribute "statefile" is not set, use default to build args
  if (!incSFIncludeCommandArgs) {

		// build strText_t for cmd args
		incSFIncludeCommandArgs =
			malloc(sizeof(strText_t));

		// and fill with cmd-args
		incSFIncludeCommandArgs->strTextLen =
			asprintf((char**) &incSFIncludeCommandArgs->strText
			,"include state");
  }

  // call command include to process the initial state-file
  struct headRetMsgMultiple_s incSFHeadRetMsgMultipleFromFn =
		AnalyzeCommandChain((uint8_t*)incSFIncludeCommandArgs->strText
		,(const size_t) incSFIncludeCommandArgs->strTextLen);

  // free our strText_t that fits the created args
  if (incSFIncludeCommandArgs->strText) 
		free(incSFIncludeCommandArgs->strText);
  if (incSFIncludeCommandArgs)
		free(incSFIncludeCommandArgs);

// -------------------------------------------------------------------------------------------------

  // in case of strTextMultiple_t queue element add leading "statefile:" text to cfgRet.strText
  if (!STAILQ_EMPTY(&incSFHeadRetMsgMultipleFromFn)) {
	  
		// fill cfgRet.strText with leading "statefile:" text
		cfgRet.strTextLen = asprintf((char**) &cfgRet.strText
			,"statefile: ");

	}

  // loop the queue entries of retMsgMultiple and add them to cfgRet.strText
  while (!STAILQ_EMPTY(&incSFHeadRetMsgMultipleFromFn)) {

		// get an retMsg element from queue
		strTextMultiple_t *retMsg =
			STAILQ_FIRST(&incSFHeadRetMsgMultipleFromFn);

		// reallocate cfgRet.strText memory to new size
		cfgRet.strText = (char *) realloc(cfgRet.strText
			,cfgRet.strTextLen + retMsg->strTextLen);

		// add current retMsg->strText to cfgRet.strText
		memcpy(cfgRet.strText + cfgRet.strTextLen
			,retMsg->strText
			,retMsg->strTextLen);

		// save new retMsg->strTextLen
		cfgRet.strTextLen += retMsg->strTextLen;

		// done, remove this entry from queue
		STAILQ_REMOVE_HEAD(&incSFHeadRetMsgMultipleFromFn, entries);
//	STAILQ_REMOVE(&headRetMsgMultipleFromFn, retMsg, strTextMultiple_s, entries);

		// free the allocated retMsg->strText
		free(retMsg->strText);

		// and the allocated strTextMultiple_t queue element
		free(retMsg);

	}

// -------------------------------------------------------------------------------------------------

  // debug info ..
  if (cfgRet.strText) {

	LOGD("cfgRet after reading state filled: %.*s"
			,cfgRet.strTextLen
			,(char*) cfgRet.strText);

  }
  else {

	LOGD("cfgRet NOT filled!");
  }

// -------------------------------------------------------------------------------------------------

  const char cfgErrMsg[] = "Messages collected while initializing FHEM:";


  // if cfgRet text -> set attribute motd to cfgRet text
  if (cfgRet.strText) {

  // build our motd attribute value and assign it
  strText_t motdAttrVal;

  motdAttrVal.strTextLen = asprintf((char**) &motdAttrVal.strText
	,"%s\n%.*s\r\n"
	,cfgErrMsg
	,(int) cfgRet.strTextLen
	,cfgRet.strText);


  strText_t defName = {(char *) "global", 6};
  strText_t attrName = {(char *) "motd", 4};

//  SetAttrValByDefTextAttrTextValText({"global", 6}, {"motd", 10}, motdAttrVal);
//  SetAttrValByDefTextAttrTextValText(&defName, &attrName, motdAttrVal);

	LOGD("motddebug: %.*s"
			,motdAttrVal.strTextLen
			,(char*) motdAttrVal.strText);

  free(motdAttrVal.strText);

//  Log 1, $cfgRet;

  free(cfgRet.strText);

  }

  // if no cfgRet text -> clr attribute motd, but only if holding old cfgRet text (detected by leadin)
  // else clear motd attribute
  else {

//	elsif($attr{global}{motd} && $attr{global}{motd} =~ m/^$cfgErrMsg/) {
//	$attr{global}{motd} = "";

  }



  LOGD("entering6...");
 // vTaskDelay(1000 / portTICK_PERIOD_MS);







/* bereich ist für telnet setup gewesen
// -------------------------------------------------------------------------------------------------

  // try to get the port (for telnet) from value of attribute (global->port)
  strText_t defNameGlobal = {(char *) "global", 6};
  strText_t attrNamePort = {(char *) "port", 4};
  strText_t *valueNamePort =
//	GetAttrValTextByDefTextAttrText( {(uint8_t*) "global", 6}, {(uint8_t*) "port", 4});
	Get_attrVal_by_defName_and_attrName(&defNameGlobal, &attrNamePort);

  // if we have an value for attribute port -> build an telnet definition
  if ( (valueNamePort) && (valueNamePort->strText) ) {

	// create the args to execute the 'define telnet telnet port' command
	strText_t defTelnetCommandArgs;

	defTelnetCommandArgs.strTextLen = asprintf((char**) &defTelnetCommandArgs.strText
		,"define Telnet Telnet %.*s"
		,(int) valueNamePort->strTextLen
		,(char*) valueNamePort->strText);

	// call command include to process the initial config-file
//war	struct headRetMsgMultiple_s headRetMsgMultipleFromFn =
//		AnalyzeCommandChain(defTelnetCommandArgs.strText, defTelnetCommandArgs.strTextLen);

	struct headRetMsgMultiple_s headRetMsgMultipleFromFn =
		AnalyzeCommandChain((uint8_t*)defTelnetCommandArgs.strText,(const size_t) defTelnetCommandArgs.strTextLen);


//	AnalyzeCommandChain((uint8_t*)includeCommandArgs.strText, (const size_t) 

	free(defTelnetCommandArgs.strText);

	// got an retMsg from Fn ? -> log it, delete port attribute
	if (STAILQ_EMPTY(&headRetMsgMultipleFromFn)) {

		// get the entries from retMsgMultiple till empty, if any
		while (!STAILQ_EMPTY(&headRetMsgMultipleFromFn)) {

			// for the retMsg elements
			strTextMultiple_t *retMsg =
				STAILQ_FIRST(&headRetMsgMultipleFromFn);

			LOGD("have to add log RetMsg: %.*s\n"
				,retMsg->strTextLen
				,retMsg->strText);

			// done, remove this entry
			STAILQ_REMOVE_HEAD(&headRetMsgMultipleFromFn, entries);
//			STAILQ_REMOVE(&headRetMsgMultipleFromFn, retMsg, strTextMultiple_s, entries);

			// free the msg-string
			free(retMsg->strText);

			// and the strTextMultiple_t
			free(retMsg);
		}

	LOGD("errors - delete port attribute here...");
	//vTaskDelay(1000 / portTICK_PERIOD_MS);
//	// error occured - delete the port attribute
//	delete($attr{global}{port});

	}

  }

  // warning if we could not find a valid telnet port
  else {

	LOGD("No port attr val for telnet assigned to global..");

  }

  // free value from Fn GetAttrValTextByDefTextAttrText
  if (valueNamePort) {

	if (valueNamePort->strText) free(valueNamePort->strText);

	free(valueNamePort);

  }

// -------------------------------------------------------------------------------------------------

  LOGD("entering7...");
 // vTaskDelay(1000 / portTICK_PERIOD_MS);

*/



}


/*
513	my $cfgErrMsg = "Messages collected while initializing FHEM:";
514	my $cfgRet="";

520	  my $ret = CommandInclude(undef, $attr{global}{configfile});
521	  $cfgRet .= "configfile: $ret\n" if($ret);
522	
523	  my $stateFile = $attr{global}{statefile};
524	  if($stateFile) {
525	    my @t = localtime;
526	    $stateFile = ResolveDateWildcards($stateFile, @t);
527	    if(-r $stateFile) {
528	      $ret = CommandInclude(undef, $stateFile);
529	      $cfgRet .= "$stateFile: $ret\n" if($ret);
530	    }
531	  }
533	
534	if($cfgRet) {
535	  $attr{global}{motd} = "$cfgErrMsg\n$cfgRet";
536	  Log 1, $cfgRet;
537	
538	} elsif($attr{global}{motd} && $attr{global}{motd} =~ m/^$cfgErrMsg/) {
539	  $attr{global}{motd} = "";
540	
541	}
542	
543	my $pfn = $attr{global}{pidfilename};
544	if($pfn) {
545	  die "$pfn: $!\n" if(!open(PID, ">$pfn"));
546	  print PID $$ . "\n";
547	  close(PID);
548	}
549	
550	my $gp = $attr{global}{port};
551	if($gp) {
552	  Log 3, "Converting 'attr global port $gp' to 'define telnetPort telnet $gp'";
553	  my $ret = CommandDefine(undef, "telnetPort telnet $gp");
554	  Log 1, "$ret" if($ret);
555	  delete($attr{global}{port});
556	}
557	
558	my $sc_text = "SecurityCheck:";
559	$attr{global}{motd} = "$sc_text\n\n"
560	        if(!$attr{global}{motd} || $attr{global}{motd} =~ m/^$sc_text/);
561	
562	$init_done = 1;
563	$lastDefChange = 1;
564	
565	foreach my $d (keys %defs) {
566	  if($defs{$d}{IODevMissing}) {
567	    Log 3, "No I/O device found for $defs{$d}{NAME}";
568	    delete $defs{$d}{IODevMissing};
569	  }
570	}
571	
572	DoTrigger("global", "INITIALIZED", 1);
573	$fhem_started = time;
574	
575	$attr{global}{motd} .= "Running with root privileges."
576	        if($^O !~ m/Win/ && $<==0 && $attr{global}{motd} =~ m/^$sc_text/);
577	$attr{global}{motd} .=
578	        "\nRestart FHEM for a new check if the problem is fixed,\n".
579	        "or set the global attribute motd to none to supress this message.\n"
580	        if($attr{global}{motd} =~ m/^$sc_text\n\n./);
581	my $motd = $attr{global}{motd};
582	if($motd eq "$sc_text\n\n") {
583	  delete($attr{global}{motd});
584	} else {
585	  if($motd ne "none") {
586	    $motd =~ s/\n/ /g;
587	    Log 2, $motd;
588	  }
589	}
590	
591	my $osuser = "os:$^O user:".(getlogin || getpwuid($<) || "unknown");
592	Log 0, "Featurelevel: $featurelevel";
593	Log 0, "Server started with ".int(keys %defs).
594	        " defined entities ($attr{global}{version} perl:$] $osuser pid:$$)";
595		
*/



/*
 *--------------------------------------------------------------------------------------------------
 *FName: Log
 * Desc: This is the main logging function
 * Info: Level 0=System; 16=debug
 * Para: char *Device -> Device Name string that should be associated to the log
 *       int LogLevel -> Level of information
 *       char *Text -> Text string that should be logged
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void
Log (char *Device, int LogLevel, char *Text)
{

  LOGD("Device:%s, Loglevel:%d, Text:%s",
	Device,
	LogLevel,
	Text);
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: Log
 * Desc: This is the main logging function
 * Info: Level 0=System; 16=debug
 * Para: char *Device -> Device Name string that should be associated to the log
 *       int LogLevel -> Level of information
 *       char *Text -> Text string that should be logged
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void
Log4 (char *text)
{

  printf ("%s", text);
}


















/**
  * --------------------------------------------------------------------------------------------------
  * FName: CommandUndefine -> gets arguments from SET 'Name Args'
  * Desc: Passes args "Args" to the SetFn of the define with name "Name"
  *
  * , and Module "TypeName" and calls Modules SetFn with
  *       args "Args"
  * Info: SET 'Name' 'Args'
  *       'Name' is a valid definition name [azAZ09._] char[31]
  *       'Args' are passed to modules SetFn for processing
  * Para: char* Args -> prt to space seperated txt string "Name TypeName Definition..."
  * Rets: char* -> error-text-string in allocated mem, or NULL=OK
  * --------------------------------------------------------------------------------------------------
  **/
strTextMultiple_t*
CommandUndefine (const uint8_t *args, const size_t argsLen)	//Undefine 'Name' -> Args = 'Name'
{
  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  char *Args = (char*) args;

  int ArgsLen = strlen(Args);

  // did we get any args ?
  if (!ArgsLen) return retMsg;

  // get the Common_Definition by Name
  Common_Definition_t *Common_Definition;

  STAILQ_FOREACH(Common_Definition, &SCDERoot.HeadCommon_Definitions, entries) {

	if (!strcmp((char*)Common_Definition->name, Args))
		break;
  }

  // did we get no Common_Definition?
  if (strcmp((char*)Common_Definition->name,Args)) {

	return retMsg;
  }

  LOGD("\nCalling UndefineFN for Name:%.*s TypeName:%.*s\n"
		  ,Common_Definition->nameLen
		  ,Common_Definition->name
		  ,Common_Definition->module->provided->typeNameLen
		  ,Common_Definition->module->provided->typeName);



  // call modules UndefineFn
  retMsg = Common_Definition->module->provided->UndefineFn(Common_Definition);

  // do we have an error msg?
  if (retMsg) {

	LOGD("Got error from UndefineFN %.*s \n"
		,retMsg->strTextLen
		,retMsg->strText);

  }

  // char error* or NULL
  return retMsg;
}







/*
 *--------------------------------------------------------------------------------------------------
 *FName: CommandLoadCommand //ActivateCommand?
 * Desc: Loads a command (Bultin commands after init, ...)
 * Para: const uint8_t *commandTxt ->  ptr to the Command-Text
 *       const size_t commandTxtLen -> Command-Text length
 *       commandFn_t commandFn -> the command Fn
 *       const uint8_t *commandHelp -> ptr the Command-Help text
 *       const size_t commandHelpLen -> Command-Help text length
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
int
CommandLoadCommand(const uint8_t *commandTxt
		,const size_t commandTxtLen
		,CommandFn_t CommandFn
		,const uint8_t *commandHelp
		,const size_t commandHelpLen)
  {
/*
  Command_t* NewCommand;

  // alloc mem for command specific definition structure (Command_t)
  NewCommand = malloc(sizeof(Command_t));

  // malloc for Command-Text
  NewCommand->commandTxt = malloc(commandTxtLen);

  // copy Command-Text
  memcpy(NewCommand->commandTxt, commandTxt, commandTxtLen);

  // copy Command-Text length
  NewCommand->commandTxtLen = commandTxtLen;

  // store ptr to the command function
  NewCommand->commandFn = commandFn;

  // malloc for Command-Help-Text
  NewCommand->commandHelp = malloc(commandHelpLen);

  // copy Command-Help-Text
  memcpy(NewCommand->commandHelp, commandHelp, commandHelpLen);

  // copy Command-Help-Text length
  NewCommand->commandHelpLen = commandHelpLen;

  // store command to SCDE-Root
  STAILQ_INSERT_HEAD(&SCDERoot.HeadCommands, NewCommand, entries);

//  Log3("main?", 1, "Loaded Command %s\n", commandTxt);

  Log3((const uint8_t*) "root"
	,4
	,1
	,"Loaded command. Command <%.*s> now available for use."
	,commandTxtLen
	,commandTxt);


*/
  return 0;
}



/* = GetAttribute("global","verbose");
 *--------------------------------------------------------------------------------------------------
 *FName: GetAssignedAttribute
 * Desc: Gets the Attribute string for {Definition}{AttributeName} from SCDERoot_s
 * Info: reserved 'global', ...
 * Para: char* Definition ->  string, the name of the definition
 *       char* AttributeName -> string, the name of the attribute
 * Rets: char* -> the Attribute string, NULL if not found
 *--------------------------------------------------------------------------------------------------
 */
/*
char* ICACHE_FLASH_ATTR
GetAttribute(char* Definition
	,char* AttributeName)
  {

  Attribute_t *A1;

  STAILQ_FOREACH(A1, &SCDERoot.HeadAttributes, entries)

	{

	if ( (strcmp(A1->Definition,Definition))
		&& (strcmp(A1->AttributeName,AttributeName)) )
		return A1->Attribute;

	}

  // not found ...
  return NULL;

  }
*/

 
/* = AssignAttribute("global","verbose","6");
 *--------------------------------------------------------------------------------------------------
 *FName: AssignAttribute
 * Desc: Assigns Attribute string to {Definition}{AttributeName} in SCDERoot_s
 * Info: reserved 'global', ...
 * Para: char* Definition ->  string, the name of the definition
 *       char* AttributeName -> string, the name of the attribute
 *       char* Attribute -> string, the attribute value
 * Rets: bool false -> added new attribute ; bool true -> overwritten existing attribute
 *--------------------------------------------------------------------------------------------------
 */
/*
bool ICACHE_FLASH_ATTR
AssignAttribute(char* Definition
	,char* AttributeName
	,char* Attribute)
  {

  Attribute_t *A1;

  STAILQ_FOREACH(A1, &SCDERoot.HeadAttributes, entries)

	{

	if ( (strcmp(A1->Definition,Definition))
		&& (strcmp(A1->AttributeName,AttributeName)) )

		{

		// found, dealloc
		free(A1->Attribute);

		// malloc new	
		A1->Attribute = malloc(strlen(Attribute) + 1);

		// copy in new attribute
		strcpy(A1->Attribute, Attribute);

		// overwritten an existing one
		return true;

		}
	}

  // not found ,create new attribute
  Attribute_t *NewAttribute = 
	malloc(sizeof(Attribute_t));

  // alloc memory for new Definition
  NewAttribute->Definition = 
	malloc(strlen(Definition) + 1);

  // copy in new Definition
  strcpy(NewAttribute->Definition
	,Definition);

  // alloc memory for new AttributeName
  NewAttribute->AttributeName = 
	malloc(strlen(AttributeName) + 1);

  // copy in new AttributeName
  strcpy(NewAttribute->AttributeName
	,AttributeName);

  // alloc memory for new Attribute
  NewAttribute->Attribute = 
	malloc(strlen(Attribute) + 1);

  // copy in new Attribute
  strcpy(NewAttribute->Attribute
	,Attribute);

  // insert new entry at head in queue
  STAILQ_INSERT_HEAD(&SCDERoot.HeadAttributes
	,NewAttribute, entries);

  // created new
  return true;

  }
*/


/* = DeleteAttribute("global","verbose");
 *--------------------------------------------------------------------------------------------------
 *FName: DeleteAttribute
 * Desc: Deletes the Attribute string for {Definition}{AttributeName} in SCDERoot_s
 * Info: 
 * Para: char* Definition ->  string, the name of the definition
 *       char* AttributeName -> string, the name of the attribute
 * Rets: bool false -> deleted attribute ; bool true -> attribute not found
 *--------------------------------------------------------------------------------------------------
 */
/*
bool ICACHE_FLASH_ATTR
DeleteAttribute(char* Definition
	,char* AttributeName)
  {

  Attribute_t *A1;

  STAILQ_FOREACH(A1, &SCDERoot.HeadAttributes, entries)

	{

	if ( (strcmp(A1->Definition,Definition))
		&& (strcmp(A1->AttributeName,AttributeName)) )
		{
		// found, remove from queue
		STAILQ_REMOVE(&SCDERoot.HeadAttributes, A1, Attribute_s, entries);

		// dealloc
		free(A1->Definition);
		free(A1->AttributeName);
		free(A1->Attribute);
		free(A1);

		// found + deleted
		return false;

		}
	}

  // not found ...
  return true;

  }
*/






		
		
		
		
		
		
		
		
		
		
		
		
		
//#################################################################################################
//#################################################################################################
//#####################################  FN GROSS GESCHRIEBEN #####################################
//#################################################################################################
//#################################################################################################		
//#################################################################################################	
		
		
		
		
		
		
	



		

	
		
	






		
	/*  lan fürs loggen den loglevel vatiabel ermitteln?  Log GetLogLevel($name,4)
 *--------------------------------------------------------------------------------------------------
 *FName: GetLogLevel
 * Desc: This is the main logging function with 3 infos: definition, loglevel and log-text
 * Info: Level 0=System; 16=debug | DO NOT FORGET TO FREE char* LogText -> ITS ALLOCATED MEMORY !!!
 * Para: Common_Definition_t* Common_Definition -> ptr to the Device-Definition which wants a log entry
 *       uint8_t LogLevel -> Level of information - to decide if it should be finally logged
 *       char* LogText -> ptr to allocated mem, containing text-string that should be logged
 * Rets: char* -> text which should contain the log level ; NULL = NotFound/Error
 *--------------------------------------------------------------------------------------------------
 */
char*
GetLogLevel (char *Device
	,uint8_t LogLevel)
  {

//  my ($dev,$deflev) = @_;
//  my $df = defined($deflev) ? $deflev : 2;
//
//  return $df if(!defined($dev));
//  return $attr{$dev}{loglevel}  if(defined($attr{$dev}) && defined($attr{$dev}{loglevel}));
//  return $df;


//  if (!GetDefinitionByName(DefinitionName)) return df;



  return NULL;
}

/*
No output
 Error
Warning
Info
 Debug
 Verbose


*/
		
		
		


		
		






		
		
		
		
		
		
		
	



/* valid time check ?
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 10;
    while(timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
*/


		
/*
 * --------------------------------------------------------------------------------------------------
 *  FName: readingsBeginUpdateFn
 *  Desc: Call readingsBeginUpdate before you start updating Readings. The updated Readings will all
 *       get the same timestamp, which is the time when you called this subroutine.
 *  Para: Common_Definition_t *Common_Definition -> the definition which wants to update readings
 *  Rets: ?
 * --------------------------------------------------------------------------------------------------
 */
/*
Die Funktion readingsBeginUpdate() bereitet die Definition mit dem Hash $hash auf ein Update von Readings vor. Dies betrifft insbesondere das Setzen von Umgebungsvariablen sowie dem aktuellen Zeitstempel als Änderungszeitpunkt. Der Aufruf dieser Funktion ist notwendig um eigentliche Updates mit der Funktion readingsBulkUpdate() auf der gewünschten Definition durchführen zu können.*/
int
readingsBeginUpdate(Common_Definition_t *Common_Definition)
  {

  // check if bulk update is already called
  if (Common_Definition->bulkUpdateReadings) {

	#if CORE_SCDE_DBG >= 1
	Log3(Common_Definition->module->provided->typeName
		,Common_Definition->module->provided->typeNameLen
		,1
		,"readingsBeginUpdateFn was already called for Definition '%.*s'. "
		 "Can not begin new update! Error!"
		,Common_Definition->name
		,Common_Definition->nameLen);
	#endif

	return 0;
  }

  // alloc mem for bulk update structure (bulkUpdateReadings_t)
  Common_Definition->bulkUpdateReadings =
	(bulkUpdateReadings_t *) malloc(sizeof(bulkUpdateReadings_t));

  // zero the bulk update structure (bulkUpdateReadings_t)
  memset(Common_Definition->bulkUpdateReadings, 0, sizeof(bulkUpdateReadings_t));

  // bulk-update single-linked-tail queue - init head
  STAILQ_INIT(&Common_Definition->bulkUpdateReadings->readingsSLTQH);

  // assign bulk update time stamp
  Common_Definition->bulkUpdateReadings->bulkUpdateTist = GetUniqueTiSt();

  #if CORE_SCDE_DBG >= 7
  // prepare time for debug
  struct tm timeinfo;
  localtime_r(&Common_Definition->bulkUpdateReadings->bulkUpdateTist, &timeinfo);

  Log3(Common_Definition->module->provided->typeName
	,Common_Definition->module->provided->typeNameLen
	,7
	,"readingsBeginUpdateFn called for Definition '%.*s'. "
	 "Readings will get TiSt '%d.%d.%d, %d:%d:%d'."
	,Common_Definition->nameLen
	,Common_Definition->name
	,timeinfo.tm_mday
	,timeinfo.tm_mon+1
	,timeinfo.tm_year+1900
	,timeinfo.tm_hour
	,timeinfo.tm_min
	,timeinfo.tm_sec);
  #endif

  return 0;
}



/*OBSOLETE USE readingsBulkUpdateFn2!
 * --------------------------------------------------------------------------------------------------
 *  FName: readingsBulkUpdateFn
 *  Desc: Call readingsBulkUpdate to update the reading.
 *        Example: readingsUpdate($hash,"temperature",$value);
 *  Para: const uint8_t *commandTxt ->  ptr to the Command-Text
 *       const size_t commandTxtLen -> Command-Text length
 *       commandFn_t commandFn -> the command Fn
 *       const uint8_t *commandHelp -> ptr the Command-Help text
 *       const size_t commandHelpLen -> Command-Help text length
 *  Rets: -/-
 * --------------------------------------------------------------------------------------------------
 */
int //OBSOLETE FN use 2
readingsBulkUpdate(Common_Definition_t *Common_Definition
		,uint8_t *readingNameText
		,size_t readingNameTextLen
		,uint8_t *readingValueText
		,size_t readingValueTextLen)
{
  // check if bulk update begin was called
  if (!Common_Definition->bulkUpdateReadings) {

	Log3(Common_Definition->module->provided->typeName
		  ,Common_Definition->module->provided->typeNameLen
		  ,1
		  ,"Error! readingsBulkUpdateFn called without calling readingsBeginUpdateFn first in Def-Name: '%.*s'."
		  ,Common_Definition->name
		  ,Common_Definition->nameLen);

	return 0;
  }

  // alloc mem for reading structure (reading_t)
  xReadingSLTQE_t *newReadingsSLTQE
	= malloc(sizeof(xReadingSLTQE_t));

  // zero the struct
  memset(newReadingsSLTQE, 0, sizeof(xReadingSLTQE_t));

  // copy reading data
  newReadingsSLTQE->nameString.length = readingNameTextLen;
  newReadingsSLTQE->nameString.characters = readingNameText;
  newReadingsSLTQE->valueString.length = readingValueTextLen;
  newReadingsSLTQE->valueString.characters = readingValueText;

  // store new reading to SCDE-Root
  STAILQ_INSERT_HEAD(&Common_Definition->bulkUpdateReadings->readingsSLTQH, newReadingsSLTQE, entries);

  LOGD("readingsBulkUpdate called for reading:%.*s value:%.*s\n"
  	,readingNameTextLen
  	,readingNameText
  	,readingValueTextLen
  	,readingValueText);

  // list currently added readings stored for processing
  xReadingSLTQE_t *currentReadingSLTQE;
  STAILQ_FOREACH(currentReadingSLTQE, &Common_Definition->bulkUpdateReadings->readingsSLTQH, entries) {

	LOGD("L readingName:%.*s, readingValue:%.*s\n"
		,currentReadingSLTQE->nameString.length
		,currentReadingSLTQE->nameString.characters
		,currentReadingSLTQE->valueString.length
		,currentReadingSLTQE->valueString.characters);
  }

  return 0;
}



/*
 * --------------------------------------------------------------------------------------------------
 *  FName: readingsBulkUpdate2Fn
 *  Desc: Call readingsBulkUpdate to add an Reading to the running update of Readings
 *  NOTE: Call readingsBeginUpdateFn first!
 *  Para: Common_Definition_t *Common_Definition -> the Definition that requests updated readings
 *        const size_t readingNameStringLength -> length of the name string
 *        const uint8_t *readingNameStringCharacters -> ptr to the name string
 *        const size_t readingValueStringLength -> length of the value string
 *        const uint8_t *readingValueStringCharacters) -> ptr to the value string
 *  Rets: ?
 * --------------------------------------------------------------------------------------------------
 */
/*Die Funktion readingsBulkUpdate() führt ein Update eines einzelnen Readings für die Definition $hash durch. Dabei wird das Readings $reading auf den Wert $value gesetzt. Bevor diese Funktion benutzt werden kann, muss readingsBeginUpdate() zuvor aufgerufen werden, ansonsten werden keine Updates durchgeführt.

changed optional
Flag, ob ein Event für dieses Update erzeugt werden soll (Wert: 1). Oder ob definitiv kein Event erzeugt werden soll (Wert: 0). Wenn nicht gesetzt, wird aufgrund entsprechender Attribute in der Definition von $hash entschieden, ob ein Event zu erzeugen ist, oder nicht (Attribute: event-on-change-reading, event-on-update-reading, event-min-interval, ...)

Rückgabewert $rv
Zeichenkette bestehend aus Reading und Wert getrennt durch einen Doppelpunkt, welcher anzeigt, auf welchen Wert das Reading tatsächlich gesetzt wurde.*/

int // will be renamed to readingsBulkUpdate
readingsBulkUpdate2(Common_Definition_t *Common_Definition
		,const size_t readingNameStringLength
		,const uint8_t *readingNameStringCharacters
		,const size_t readingValueStringLength
		,const uint8_t *readingValueStringCharacters
		,const bool changed)
{
  // check if bulk update begin was called
  if (!Common_Definition->bulkUpdateReadings) {

	#if CORE_SCDE_DBG >= 1
	Log3(Common_Definition->module->provided->typeName
		,Common_Definition->module->provided->typeNameLen
		,1
		,"readingsBulkUpdateFn called for Definition '%.*s' "
		 "without calling readingsBeginUpdateFn first! Error!"
		,Common_Definition->name
		,Common_Definition->nameLen);
	#endif

	return 0;
  }

 #if CORE_SCDE_DBG >= 7
  Log3(Common_Definition->module->provided->typeName
	,Common_Definition->module->provided->typeNameLen
	,7
	,"readingsBulkUpdateFn is adding Reading Name: '%.*s' "
	 "Value: '%.*s' for update."
	,readingNameStringLength
	,readingNameStringCharacters
	,readingValueStringLength
 	,readingValueStringCharacters);
  #endif

  // alloc mem for reading
  xReadingSLTQE_t *newReadingSLTQE
	= malloc(sizeof(xReadingSLTQE_t));

  // zero the struct ??? notwendig ???
  memset(newReadingSLTQE, 0, sizeof(xReadingSLTQE_t));

  // fill Reading with Name
  newReadingSLTQE->nameString.length =
	asprintf((char **) &newReadingSLTQE->nameString.characters
	,"%.*s"
	,readingNameStringLength
	,readingNameStringCharacters);

  // fill Reading with Value
  newReadingSLTQE->valueString.length =
	asprintf((char **) &newReadingSLTQE->valueString.characters
	,"%.*s"
	,readingValueStringLength
	,readingValueStringCharacters);

  // add new Reading to definitions bulk-Update-Readings queue
  STAILQ_INSERT_TAIL(&Common_Definition->bulkUpdateReadings->readingsSLTQH
	,newReadingSLTQE, entries);

  return 0;
}



/*http://sancho.ccd.uniroma2.it/cgi-bin/man/man2html?STAILQ_INIT+3
 * --------------------------------------------------------------------------------------------------
 *  FName: readingsEndUpdate
 *  Desc: Call readingsEndUpdate when you are done updating readings. This optionally calls DoTrigger
 *        to propagate the changes.
 *  Para: Common_Definition_t *Common_Definition -> the Definition that requests updated readings
 *        bool doTrigger -> triggering readings ? false / true
 *  Rets: -/-
 * --------------------------------------------------------------------------------------------------
 */
/*Die Funktion readingsEndUpdate() beendet den Bulk-Update Prozess durch die Funktionen readingsBeginUpdate() & readingsBulkUpdate() und triggert optional die entsprechenden Events sämtlicher erzeugter Readings für die Definition $hash. Desweiteren werden nachgelagerte Tasks wie bspw. die Erzeugung von User-Readings (Attribut: userReadings), sowie die Erzeugung des STATE aufgrund des Attributs stateFormat durchgeführt. Sofern $do_trigger gesetzt ist, werden alle anstehenden Events nach Abschluss getriggert.*/
int
readingsEndUpdate(Common_Definition_t *Common_Definition,
	bool doTrigger)
{
  // check if bulk update begin was called
  if (!Common_Definition->bulkUpdateReadings) {

	#if CORE_SCDE_DBG >= 1
	Log3(Common_Definition->module->provided->typeName
		,Common_Definition->module->provided->typeNameLen
		,1
		,"readingsEndUpdateFn called for Definition '%.*s' "
		 "without calling readingsBeginUpdateFn first! Error!"
		,Common_Definition->name
		,Common_Definition->nameLen);
	#endif

	return 0;
  }

  // loop through the bulk-update-readings - start with first entry
  xReadingSLTQE_t *currentBUReadingSLTQE = 
	STAILQ_FIRST(&Common_Definition->bulkUpdateReadings->readingsSLTQH);

  while (currentBUReadingSLTQE != NULL) {

	// set common tist to BU reading element
	currentBUReadingSLTQE->readingTist =
		Common_Definition->bulkUpdateReadings->bulkUpdateTist;

/*	LOGD("Now proc. readingName:%.*s, readingValue:%.*s\n"
		,currentBUReadingSLTQE->nameString.length
		,currentBUReadingSLTQE->nameString.characters
		,currentBUReadingSLTQE->valueString.length
		,currentBUReadingSLTQE->valueString.characters);*/

	// SLTQ Element to loop trough the old readings
	xReadingSLTQE_t *existingReadingSLTQE = 
		STAILQ_FIRST(&Common_Definition->headReadings);

	// loop through the existing readings
	while (true) {

		// no old reading found ? Add new reading at tail of Readings-SLTQ
		if (existingReadingSLTQE == NULL) {

			// remove from Bulk-Update-Readings-SLTQ
			STAILQ_REMOVE(&Common_Definition->bulkUpdateReadings->readingsSLTQH,
				currentBUReadingSLTQE, xReadingSLTQE_s, entries);

			// add new reading at tail of Readings-SLTQ
			STAILQ_INSERT_TAIL(&Common_Definition->headReadings,
				currentBUReadingSLTQE, entries);

/*			LOGD("Added new reading - readingName:%.*s, readingValue:%.*s\n"
				,currentBUReadingSLTQE->nameString.length
				,currentBUReadingSLTQE->nameString.characters
				,currentBUReadingSLTQE->valueString.length
				,currentBUReadingSLTQE->valueString.characters);*/

			// added new, break
			break;
		}

		// is there an existing reading name that matches? Then replace old value. 
		if ( (existingReadingSLTQE->nameString.length == 
			currentBUReadingSLTQE->nameString.length)
			&& (!strncmp((const char*) existingReadingSLTQE->nameString.characters,
				(const char*) currentBUReadingSLTQE->nameString.characters,
				currentBUReadingSLTQE->nameString.length)) ) {

			// upd. existing reading, free existing valueString (take current valueString)
			if (existingReadingSLTQE->valueString.characters) 
				free(existingReadingSLTQE->valueString.characters);

			// upd. existing reading, move current valueString length
			existingReadingSLTQE->valueString.length = 
				currentBUReadingSLTQE->valueString.length;

			// upd. existing reading, move current valueString
			existingReadingSLTQE->valueString.characters = 
				currentBUReadingSLTQE->valueString.characters;

			// upd. existing reading, free current nameString (keep existing nameString)
			if (currentBUReadingSLTQE->nameString.characters) 
				free(currentBUReadingSLTQE->nameString.characters);

			// updating existing reading - move Time Stamp
			existingReadingSLTQE->readingTist =
				Common_Definition->bulkUpdateReadings->bulkUpdateTist;

			// remove from Bulk-Update-Readings-SLTQ
			STAILQ_REMOVE(&Common_Definition->bulkUpdateReadings->readingsSLTQH,
				currentBUReadingSLTQE, xReadingSLTQE_s, entries);

			// we have taken the data - free current bulk update reading SLTQE
			free(currentBUReadingSLTQE);

/*			LOGD("Updated old reading - readingName:%.*s, readingValue:%.*s\n"
				,existingReadingSLTQE->nameString.length
				,existingReadingSLTQE->nameString.characters
				,existingReadingSLTQE->valueString.length
				,existingReadingSLTQE->valueString.characters);*/

			// found, break
			break;
		}

		// get next existing reading for processing
		existingReadingSLTQE = STAILQ_NEXT(existingReadingSLTQE, entries);

	}

	// continue loop - get next first bulk update reading
	currentBUReadingSLTQE = 
		STAILQ_FIRST(&Common_Definition->bulkUpdateReadings->readingsSLTQH);
  }

  // dealloc bulk-update data
  free(Common_Definition->bulkUpdateReadings);

  // clear bulk-update data (mark as not started)
  Common_Definition->bulkUpdateReadings = NULL;

/*printf("readingsEndUpdate finnished. Current readings for this definiton:\n");

  // list readings stored for definition after processing
//  xReadingSLTQE_t *currentBUReadingSLTQE;
  STAILQ_FOREACH(currentBUReadingSLTQE, &Common_Definition->headReadings, entries) {

	LOGD("L readingName:%.*s, readingValue:%.*s\n"
		,currentBUReadingSLTQE->nameString.length
		,currentBUReadingSLTQE->nameString.characters
		,currentBUReadingSLTQE->valueString.length
		,currentBUReadingSLTQE->valueString.characters);
  }*/

  return 0;
}





























/*
 * --------------------------------------------------------------------------------------------------
 *  FName: readingsBeginUpdateFn
 *  Desc: Call readingsBeginUpdate before you start updating Readings. The updated Readings will all
 *       get the same timestamp, which is the time when you called this subroutine.
 *  Para: Common_Definition_t *Common_Definition -> the definition which wants to update readings
 *  Rets: ?
 * --------------------------------------------------------------------------------------------------
 */
/*
Die Funktion readingsBeginUpdate() bereitet die Definition mit dem Hash $hash auf ein Update von Readings vor. Dies betrifft insbesondere das Setzen von Umgebungsvariablen sowie dem aktuellen Zeitstempel als Änderungszeitpunkt. Der Aufruf dieser Funktion ist notwendig um eigentliche Updates mit der Funktion readingsBulkUpdate() auf der gewünschten Definition durchführen zu können.*/
int
readingsBeginUpdate2(Common_Definition_t* Common_Definition)
{
  // check if bulk update is already called
  if (Common_Definition->bulkUpdateReadings2) {

	#if CORE_SCDE_DBG >= 1
	Log3(Common_Definition->module->provided->typeName
		,Common_Definition->module->provided->typeNameLen
		,1
		,"readingsBeginUpdate2Fn was already called for Definition '%.*s'. "
		 "Can not begin new update! Error!"
		,Common_Definition->name
		,Common_Definition->nameLen);
	#endif

	return 0;
  }

  // alloc mem for bulk update structure (bulkUpdateReadings2_t)
  Common_Definition->bulkUpdateReadings2 =
	(bulkUpdateReadings2_t *) malloc(sizeof(bulkUpdateReadings2_t));

  // zero the bulk update structure (bulkUpdateReadings_t)
  memset(Common_Definition->bulkUpdateReadings2, 0, sizeof(bulkUpdateReadings2_t));

  // bulk-update single-linked-tail queue - init head
  STAILQ_INIT(&Common_Definition->bulkUpdateReadings2->readings2SLTQH);

  // assign bulk update time stamp
  Common_Definition->bulkUpdateReadings->bulkUpdateTist = GetUniqueTiSt();

  #if CORE_SCDE_DBG >= 7
  // prepare time for debug
  struct tm timeinfo;
  localtime_r(&Common_Definition->bulkUpdateReadings2->bulkUpdateTist, &timeinfo);

  Log3(Common_Definition->module->provided->typeName
	,Common_Definition->module->provided->typeNameLen
	,7
	,"readingsBeginUpdate2Fn called for Definition '%.*s'. "
	 "Readings will get TiSt '%d.%d.%d, %d:%d:%d'."
	,Common_Definition->nameLen
	,Common_Definition->name
	,timeinfo.tm_mday
	,timeinfo.tm_mon+1
	,timeinfo.tm_year+1900
	,timeinfo.tm_hour
	,timeinfo.tm_min
	,timeinfo.tm_sec);
  #endif

  return 0;
}


/*
 * --------------------------------------------------------------------------------------------------
 *  FName: readingsBulkUpdate3Fn
 *  Desc: Call readingsBulkUpdate to add an Reading to the running update of Readings
 *  NOTE: Call readingsBeginUpdateFn first!
 *  Para: Common_Definition_t *Common_Definition -> the Definition that requests updated readings
 *        const size_t readingNameStringLength -> length of the name string
 *        const uint8_t *readingNameStringCharacters -> ptr to the name string
 *        const size_t readingValueStringLength -> length of the value string

 *        const uint8_t *readingValueStringCharacters) -> ptr to the value string
 *  Rets: ?
 * --------------------------------------------------------------------------------------------------
 */
/*Die Funktion readingsBulkUpdate() führt ein Update eines einzelnen Readings für die Definition $hash durch. Dabei wird das Readings $reading auf den Wert $value gesetzt. Bevor diese Funktion benutzt werden kann, muss readingsBeginUpdate() zuvor aufgerufen werden, ansonsten werden keine Updates durchgeführt.

changed optional
Flag, ob ein Event für dieses Update erzeugt werden soll (Wert: 1). Oder ob definitiv kein Event erzeugt werden soll (Wert: 0). Wenn nicht gesetzt, wird aufgrund entsprechender Attribute in der Definition von $hash entschieden, ob ein Event zu erzeugen ist, oder nicht (Attribute: event-on-change-reading, event-on-update-reading, event-min-interval, ...)



Rückgabewert $rv
Zeichenkette bestehend aus Reading und Wert getrennt durch einen Doppelpunkt, welcher anzeigt, auf welchen Wert das Reading tatsächlich gesetzt wurde.*/

int
readingsBulkUpdate3(Common_Definition_t* Common_Definition,
		    const Reading2_t* reading,
		    const bool changed)

/*		,const size_t readingNameStringLength
		,const uint8_t *readingNameStringCharacters
		,const size_t readingValueStringLength
		,const uint8_t *readingValueStringCharacters
		,const bool changed)*/
{
  // check if bulk update begin was called
  if (!Common_Definition->bulkUpdateReadings) {

	#if CORE_SCDE_DBG >= 1
	Log3(Common_Definition->module->provided->typeName
		,Common_Definition->module->provided->typeNameLen
		,1
		,"readingsBulkUpdateFn called for Definition '%.*s' "
		 "without calling readingsBeginUpdateFn first! Error!"
		,Common_Definition->name
		,Common_Definition->nameLen);
	#endif

	return 0;
  }

 #if CORE_SCDE_DBG >= 7
  Log3(Common_Definition->module->provided->typeName
	,Common_Definition->module->provided->typeNameLen
	,7
	,"readingsBulkUpdateFn is adding Reading Name: '%.*s' "
	 "Value: '%.*s' for update."
	,reading->nameString.length
	,reading->nameString.characters
	,reading->valueString.length
	,reading->valueString.characters);
  #endif

  // alloc mem for reading
  xReading2SLTQE_t *newReading2SLTQE
	= malloc( sizeof(xReading2SLTQE_t) );

  // zero the struct ??? notwendig ???
  memset(newReading2SLTQE, 0, sizeof(xReading2SLTQE_t));


  // fill Reading with Reading
  newReading2SLTQE->reading = reading;

  // fill Reading with Definition - the Reading belongs to
  newReading2SLTQE->definition = Common_Definition;

  // add new Reading to definitions bulk-Update-Readings queue
  STAILQ_INSERT_TAIL(&Common_Definition->bulkUpdateReadings2->readings2SLTQH
	,newReading2SLTQE, entries);

  return 0;
}



/*http://sancho.ccd.uniroma2.it/cgi-bin/man/man2html?STAILQ_INIT+3
 * --------------------------------------------------------------------------------------------------
 *  FName: readingsEndUpdate
 *  Desc: Call readingsEndUpdate when you are done updating readings. This optionally calls DoTrigger
 *        to propagate the changes.
 *  Para: Common_Definition_t *Common_Definition -> the Definition that requests updated readings

 *        bool doTrigger -> triggering readings ? false / true
 *  Rets: -/-
 * --------------------------------------------------------------------------------------------------
 */
/*Die Funktion readingsEndUpdate() beendet den Bulk-Update Prozess durch die Funktionen readingsBeginUpdate() & readingsBulkUpdate() und triggert optional die entsprechenden Events sämtlicher erzeugter Readings für die Definition $hash. Desweiteren werden nachgelagerte Tasks wie bspw. die Erzeugung von User-Readings (Attribut: userReadings), sowie die Erzeugung des STATE aufgrund des Attributs stateFormat durchgeführt. Sofern $do_trigger gesetzt ist, werden alle anstehenden Events nach Abschluss getriggert.*/
int
readingsEndUpdate2(Common_Definition_t *Common_Definition,
	bool doTrigger)
{
  // check if bulk update begin was called
  if (!Common_Definition->bulkUpdateReadings) {

	#if CORE_SCDE_DBG >= 1
	Log3(Common_Definition->module->provided->typeName
		,Common_Definition->module->provided->typeNameLen
		,1
		,"readingsEndUpdateFn called for Definition '%.*s' "
		 "without calling readingsBeginUpdateFn first! Error!"
		,Common_Definition->name
		,Common_Definition->nameLen);
	#endif

	return 0;
  }

  // loop through the bulk-update-readings - start with first entry
  xReadingSLTQE_t *currentBUReadingSLTQE = 
	STAILQ_FIRST(&Common_Definition->bulkUpdateReadings->readingsSLTQH);

  while (currentBUReadingSLTQE != NULL) {

	// set common tist to BU reading element
	currentBUReadingSLTQE->readingTist =
		Common_Definition->bulkUpdateReadings->bulkUpdateTist;

/*	LOGD("Now proc. readingName:%.*s, readingValue:%.*s\n"
		,currentBUReadingSLTQE->nameString.length

		,currentBUReadingSLTQE->nameString.characters
		,currentBUReadingSLTQE->valueString.length
		,currentBUReadingSLTQE->valueString.characters);*/

	// SLTQ Element to loop trough the old readings
	xReadingSLTQE_t *existingReadingSLTQE = 
		STAILQ_FIRST(&Common_Definition->headReadings);

	// loop through the existing readings
	while (true) {

		// no old reading found ? Add new reading at tail of Readings-SLTQ
		if (existingReadingSLTQE == NULL) {

			// remove from Bulk-Update-Readings-SLTQ
			STAILQ_REMOVE(&Common_Definition->bulkUpdateReadings->readingsSLTQH,
				currentBUReadingSLTQE, xReadingSLTQE_s, entries);

			// add new reading at tail of Readings-SLTQ
			STAILQ_INSERT_TAIL(&Common_Definition->headReadings,
				currentBUReadingSLTQE, entries);

/*			LOGD("Added new reading - readingName:%.*s, readingValue:%.*s\n"
				,currentBUReadingSLTQE->nameString.length

				,currentBUReadingSLTQE->nameString.characters
				,currentBUReadingSLTQE->valueString.length
				,currentBUReadingSLTQE->valueString.characters);*/

			// added new, break
			break;
		}

		// is there an existing reading name that matches? Then replace old value. 
		if ( (existingReadingSLTQE->nameString.length == 
			currentBUReadingSLTQE->nameString.length)
			&& (!strncmp((const char*) existingReadingSLTQE->nameString.characters,
				(const char*) currentBUReadingSLTQE->nameString.characters,
				currentBUReadingSLTQE->nameString.length)) ) {

			// upd. existing reading, free existing valueString (take current valueString)
			if (existingReadingSLTQE->valueString.characters) 
				free(existingReadingSLTQE->valueString.characters);

			// upd. existing reading, move current valueString length
			existingReadingSLTQE->valueString.length = 
				currentBUReadingSLTQE->valueString.length;

			// upd. existing reading, move current valueString
			existingReadingSLTQE->valueString.characters = 
				currentBUReadingSLTQE->valueString.characters;

			// upd. existing reading, free current nameString (keep existing nameString)
			if (currentBUReadingSLTQE->nameString.characters) 
				free(currentBUReadingSLTQE->nameString.characters);

			// updating existing reading - move Time Stamp
			existingReadingSLTQE->readingTist =
				Common_Definition->bulkUpdateReadings->bulkUpdateTist;

			// remove from Bulk-Update-Readings-SLTQ
			STAILQ_REMOVE(&Common_Definition->bulkUpdateReadings->readingsSLTQH,
				currentBUReadingSLTQE, xReadingSLTQE_s, entries);

			// we have taken the data - free current bulk update reading SLTQE
			free(currentBUReadingSLTQE);

/*			LOGD("Updated old reading - readingName:%.*s, readingValue:%.*s\n"
				,existingReadingSLTQE->nameString.length
				,existingReadingSLTQE->nameString.characters
				,existingReadingSLTQE->valueString.length
				,existingReadingSLTQE->valueString.characters);*/

			// found, break
			break;
		}

		// get next existing reading for processing
		existingReadingSLTQE = STAILQ_NEXT(existingReadingSLTQE, entries);

	}

	// continue loop - get next first bulk update reading
	currentBUReadingSLTQE = 
		STAILQ_FIRST(&Common_Definition->bulkUpdateReadings->readingsSLTQH);
  }

  // dealloc bulk-update data
  free(Common_Definition->bulkUpdateReadings);

  // clear bulk-update data (mark as not started)
  Common_Definition->bulkUpdateReadings = NULL;

/*printf("readingsEndUpdate finnished. Current readings for this definiton:\n");

  // list readings stored for definition after processing
//  xReadingSLTQE_t *currentBUReadingSLTQE;
  STAILQ_FOREACH(currentBUReadingSLTQE, &Common_Definition->headReadings, entries) {


	LOGD("L readingName:%.*s, readingValue:%.*s\n"
		,currentBUReadingSLTQE->nameString.length
		,currentBUReadingSLTQE->nameString.characters
		,currentBUReadingSLTQE->valueString.length
		,currentBUReadingSLTQE->valueString.characters);
  }*/

  return 0;
}






/*

		// loop through  currently stored modules
		Module_t* p_Module;

		STAILQ_FOREACH(p_Module, &SCDERoot->HeadModules, entries) {

			// get the Common_Definitions for current module
			Common_Definition_t* p_Common_Definition;

			STAILQ_FOREACH(p_Common_Definition, &SCDERoot->HeadCommon_Definitions, entries) {

				if (Common_Definition->module == Module) {


				}
			}
		}

*/




























/*
  // list currently added readings stored for processing
  reading_t *reading;
  STAILQ_FOREACH(reading, &Common_Definition->bulkUpdateReadings->readingsSLTQH, entries) {

	printf("Processing readingName:%.*s, readingValue:%.*s\n"
		,reading->readingNameTextLen
		,reading->readingNameText
		,reading->readingValueTextLen
		,reading->readingValueText);

	// loop and find old reading for this new reading ?
	reading_t *existingReadingSLTQE;
	STAILQ_FOREACH(existingReadingSLTQE, &Common_Definition->headReadings, entries) {

		// is this an old value for this reading
		if ( (existingReadingSLTQE->readingNameTextLen == reading->readingNameTextLen)
			&& (!strncmp((const char*) existingReadingSLTQE->readingNameText, (const char*) reading->readingNameText, reading->readingNameTextLen)) ) {

		// replace old value for this reading
		if (existingReadingSLTQE->readingNameText) free(existingReadingSLTQE->readingNameText);
		if (existingReadingSLTQE->readingValueText) free(existingReadingSLTQE->readingValueText);
		existingReadingSLTQE->readingNameTextLen = reading->readingNameTextLen;
		existingReadingSLTQE->readingNameText = reading->readingNameText;
		existingReadingSLTQE->readingValueTextLen = reading->readingValueTextLen;
		existingReadingSLTQE->readingValueText = reading->readingValueText;

		printf("Updated old reading - readingName:%.*s, readingValue:%.*s\n"
			,existingReadingSLTQE->readingNameTextLen
			,existingReadingSLTQE->readingNameText
			,existingReadingSLTQE->readingValueTextLen
			,existingReadingSLTQE->readingValueText);

		}

		// no old value for this reading found
		else if (existingReadingSLTQE->entries == NULL) {

			// store new reading to SCDE-Root
			STAILQ_INSERT_AFTER(&Common_Definition->bulkUpdateReadings->readingsSLTQH, newReadingsSLTQE, entries);

		}
	



	}

  }
*/









/*
#
# Call readingsBeginUpdate before you start updating readings.
# The updated readings will all get the same timestamp,
# which is the time when you called this subroutine.
#
sub 
readingsBeginUpdate($)
{
  my ($hash)= @_;
  my $name = $hash->{NAME};
  
  if(!$name) {
    Log 1, "ERROR: empty name in readingsBeginUpdate";
    stacktrace();
    return;
  }

  # get timestamp
  my $now = gettimeofday();
  my $fmtDateTime = FmtDateTime($now);
  $hash->{".updateTime"} = $now; # in seconds since the epoch
  $hash->{".updateTimestamp"} = $fmtDateTime;

  my $attrminint = AttrVal($name, "event-min-interval", undef);
  if($attrminint) {
    my @a = split(/,/,$attrminint);
    $hash->{".attrminint"} = \@a;
  }
  
  my $attraggr = AttrVal($name, "event-aggregator", undef);
  if($attraggr) {
    my @a = split(/,/,$attraggr);
    $hash->{".attraggr"} = \@a;
  }

  my $attreocr= AttrVal($name, "event-on-change-reading", undef);
  if($attreocr) {
    my @a = split(/,/,$attreocr);
    $hash->{".attreocr"} = \@a;
  }
  
  my $attreour= AttrVal($name, "event-on-update-reading", undef);
  if($attreour) {
    my @a = split(/,/,$attreour);
    $hash->{".attreour"} = \@a;
  }

  my $attrtocr= AttrVal($name, "timestamp-on-change-reading", undef);
  if($attrtocr) {
    my @a = split(/,/,$attrtocr);
    $hash->{".attrtocr"} = \@a;
  }


  $hash->{CHANGED}= () if(!defined($hash->{CHANGED}));
  return $fmtDateTime;
}

sub
evalStateFormat($)
{
  my ($hash) = @_;

  my $name = $hash->{NAME};

  ###########################
  # Set STATE
  my $sr = AttrVal($name, "stateFormat", undef);
  my $st = $hash->{READINGS}{state};
  if(!$sr) {
    $st = $st->{VAL} if(defined($st));

  } elsif($sr =~ m/^{(.*)}$/s) {
    $st = eval $1;
    if($@) {
      $st = "Error evaluating $name stateFormat: $@";
      Log 1, $st;
    }

  } else {
    # Substitute reading names with their values, leave the rest untouched.
    $st = $sr;
    my $r = $hash->{READINGS};
    $st =~ s/\b([A-Za-z\d_\.-]+)\b/($r->{$1} ? $r->{$1}{VAL} : $1)/ge;

  }
  $hash->{STATE} = ReplaceEventMap($name, $st, 1) if(defined($st));
}

#
# Call readingsEndUpdate when you are done updating readings.
# This optionally calls DoTrigger to propagate the changes.
#
sub
readingsEndUpdate($$)
{
  my ($hash,$dotrigger)= @_;
  my $name = $hash->{NAME};

  $hash->{".triggerUsed"} = 1 if(defined($hash->{".triggerUsed"}));

  # process user readings
  if(defined($hash->{'.userReadings'})) {
    foreach my $userReading (@{$hash->{'.userReadings'}}) {

      my $trigger = $userReading->{trigger};
      if(defined($trigger)) {
        my @fnd = grep { $_ && $_ =~ m/^$trigger$/ } @{$hash->{CHANGED}};
        next if(!@fnd);
      }

      my $reading= $userReading->{reading};
      my $modifier= $userReading->{modifier};
      my $perlCode= $userReading->{perlCode};
      my $oldvalue= $userReading->{value};
      my $oldt= $userReading->{t};
      #Debug "Evaluating " . $reading;
      $cmdFromAnalyze = $perlCode;      # For the __WARN__ sub
      my $NAME = $name; # no exceptions, #53069
      my $value= eval $perlCode;
      $cmdFromAnalyze = undef;
      my $result;
      # store result
      if($@) {
        $value = "Error evaluating $name userReading $reading: $@";
        Log 1, $value;
        $result= $value;
      } elsif($modifier eq "none") {
        $result= $value;
      } elsif($modifier eq "difference") {
        $result= $value - $oldvalue if(defined($oldvalue));
      } elsif($modifier eq "differential") {
        my $deltav= $value - $oldvalue if(defined($oldvalue));
        my $deltat= $hash->{".updateTime"} - $oldt if(defined($oldt));
        if(defined($deltav) && defined($deltat) && ($deltat>= 1.0)) {
          $result= $deltav/$deltat;
        }
      } elsif($modifier eq "integral") {
        if(defined($oldt) && defined($oldvalue)) {
          my $deltat= $hash->{".updateTime"} - $oldt if(defined($oldt));
          my $avgval= ($value + $oldvalue) / 2;
          $result = ReadingsVal($name,$reading,$value);
          if(defined($deltat) && $deltat>= 1.0) {
            $result+= $avgval*$deltat;
          }
        }
      } elsif($modifier eq "offset") {
        $oldvalue = $value if( !defined($oldvalue) );
        $result = ReadingsVal($name,$reading,0);
        $result += $oldvalue if( $value < $oldvalue );
      } elsif($modifier eq "monotonic") {
        $oldvalue = $value if( !defined($oldvalue) );
        $result = ReadingsVal($name,$reading,$value);
        $result += $value - $oldvalue if( $value > $oldvalue );
      } 
      readingsBulkUpdate($hash,$reading,$result,1) if(defined($result));
      # store value
      $userReading->{TIME}= $hash->{".updateTimestamp"};
      $userReading->{t}= $hash->{".updateTime"};
      $userReading->{value}= $value;
    }
  }
  evalStateFormat($hash);

  # turn off updating mode
  delete $hash->{".updateTimestamp"};
  delete $hash->{".updateTime"};
  delete $hash->{".attreour"};
  delete $hash->{".attreocr"};
  delete $hash->{".attraggr"};
  delete $hash->{".attrminint"};
  delete $hash->{".attrtocr"};


  # propagate changes
  if($dotrigger && $init_done) {
    DoTrigger($name, undef, 0) if(!$readingsUpdateDelayTrigger);
  } else {
    if(!defined($hash->{INTRIGGER})) {
      delete($hash->{CHANGED});
      delete($hash->{CHANGEDWITHSTATE})
    }
  }
  
  return undef;
}

#
# Call readingsBulkUpdate to update the reading.
# Example: readingsUpdate($hash,"temperature",$value);
#
sub
readingsBulkUpdate($$$@)
{
  my ($hash,$reading,$value,$changed)= @_;
  my $name= $hash->{NAME};

  return if(!defined($reading) || !defined($value));
  # sanity check
  if(!defined($hash->{".updateTimestamp"})) {
    Log 1, "readingsUpdate($name,$reading,$value) missed to call ".
                "readingsBeginUpdate first.";
    return;
  }
  
  # shorthand
  my $readings = $hash->{READINGS}{$reading};

  if(!defined($changed)) {
    $changed = (substr($reading,0,1) ne "."); # Dont trigger dot-readings
  }
  $changed = 0 if($hash->{".ignoreEvent"});

  # if reading does not exist yet: fake entry to allow filtering
  $readings = { VAL => "" } if( !defined($readings) );

  my $update_timestamp = 1;
  if($changed) {
  
    # these flags determine if any of the "event-on" attributes are set
    my $attreocr = $hash->{".attreocr"};
    my $attreour = $hash->{".attreour"};

    # determine whether the reading is listed in any of the attributes
    my $eocr = $attreocr &&
               ( my @eocrv = grep { my $l = $_; $l =~ s/:.aaaaaaaaaaaaaaaaaaaaahstern//;
                   ($reading=~ m/^$l$/) ? $_ : undef} @{$attreocr});
    my $eour = $attreour && grep($reading =~ m/^$_$/, @{$attreour});

    # check if threshold is given
    my $eocrExists = $eocr;
    if( $eocr
        && $eocrv[0] =~ m/.*:(.*)/ ) {
      my $threshold = $1;
      my $ov = $value;

      $value =~ s/[^\d\.\-eE]//g; # We expect only numbers here.
      my $isNum = looks_like_number($value);
      if(!$isNum) {   # Forum #41083
        $value = $ov;
        $value =~ s/[^\d\.\-]//g;
        $isNum = looks_like_number($value);
      }
      if($isNum) {
        my $last_value = $hash->{".attreocr-threshold$reading"};
        if( !defined($last_value) ) {
          $hash->{".attreocr-threshold$reading"} = $value;
        } elsif( abs($value-$last_value) < $threshold ) {
          $eocr = 0;
        } else {
          $hash->{".attreocr-threshold$reading"} = $value;
        }
      }
      $value = $ov;
    }

    # determine if an event should be created:
    # always create event if no attribute is set
    # or if the reading is listed in event-on-update-reading
    # or if the reading is listed in event-on-change-reading...
    # ...and its value has changed...
    # ...and the change greater then the threshold
    $changed= !($attreocr || $attreour)
              || $eour  
              || ($eocr && ($value ne $readings->{VAL}));
    #Log 1, "EOCR:$eocr EOUR:$eour CHANGED:$changed";

    my @v = grep { my $l = $_;
                   $l =~ s/:.//;
                   ($reading=~ m/^$l$/) ? $_ : undef} @{$hash->{".attrminint"}};
    if(@v) {
      my (undef, $minInt) = split(":", $v[0]);
      my $now = $hash->{".updateTime"};
      my $le = $hash->{".lastTime$reading"};
      if($le && $now-$le < $minInt) {
        if(!$eocr || ($eocr && $value eq $readings->{VAL})){
          $changed = 0;
        } else {
          $hash->{".lastTime$reading"} = $now;
        }
      } else {
        $hash->{".lastTime$reading"} = $now;
        $changed = 1 if($eocrExists);
      }
    }

    if( $attreocr ) {
      if( my $attrtocr = $hash->{".attrtocr"} ) {
        $update_timestamp = $changed
                if( $attrtocr && grep($reading =~ m/^$_$/, @{$attrtocr}) );
      }
    }

  }

  if($changed) {
    #Debug "Processing $reading: $value";
    my @v = grep { my $l = $_;
                  $l =~ s/:.//;
                  ($reading=~ m/^$l$/) ? $_ : undef} @{$hash->{".attraggr"}};
    if(@v) {
      # e.g. power:20:linear:avg
      my (undef,$duration,$method,$function,$holdTime) = split(":", $v[0], 5);
      my $ts;
      if(defined($readings->{".ts"})) {
        $ts= $readings->{".ts"};
      } else {
        require "TimeSeries.pm";
        $ts= TimeSeries->new( { method => $method, 
                                autoreset => $duration,
                                holdTime => $holdTime } );
        $readings->{".ts"}= $ts;
        # access from command line:
        # { $defs{"myClient"}{READINGS}{"myValue"}{".ts"}{max} }
        #Debug "TimeSeries created.";
      }
      my $now = $hash->{".updateTime"};
      my $val = $value; # save value
      $changed = $ts->elapsed($now);
      $value = $ts->{$function} if($changed);
      $ts->add($now, $val); 
    } else {
      # If no event-aggregator attribute, then remove stale series if any.
      delete $readings->{".ts"};
    }
  }
  
  
  setReadingsVal($hash, $reading, $value, $hash->{".updateTimestamp"})
        if($update_timestamp); 
  
  my $rv = "$reading: $value";
  if($changed) {
    if($reading eq "state") {
      $rv = $value;
      $hash->{CHANGEDWITHSTATE} = [];
    }
    addEvent($hash, $rv);
  }
  return $rv;
}

#
# this is a shorthand call
#
sub
readingsSingleUpdate($$$$)
{
  my ($hash,$reading,$value,$dotrigger)= @_;
  readingsBeginUpdate($hash);
  my $rv = readingsBulkUpdate($hash,$reading,$value);
  readingsEndUpdate($hash,$dotrigger);
  return $rv;
}
*/













	
		
//#################################################################################################
//#################################################################################################
//#####################################  added Fn (Perl -> C) #####################################
//#################################################################################################
//#################################################################################################		
//#################################################################################################




	
		
		
		
		
		
		
		
		































/*
 * Find the first occurrence of find in s, where the search is limited to the
 * first slen characters of s.
 */
char*
strnstr(s, find, slen)
	const char *s;
	const char *find;
	size_t slen;
{
	char c, sc;
	size_t len;

	if ((c = *find++) != '\0') {
		len = strlen(find);
		do {
			do {
				if ((sc = *s++) == '\0' || slen-- < 1)
					return (NULL);
			} while (sc != c);
			if (len > slen)
				return (NULL);
		} while (strncmp(s, find, len) != 0);
		s--;
	}
	return ((char *)s);
}



/**
 *--------------------------------------------------------------------------------------------------
 *FName: ParseKVInputArgs Parse Key=Value@ input Arguments
 * Desc: Hepler routine that parses Key=Value(@) input arguments into an allocated array.
 *
 *       . searches and extracts an SPECIAL string value matching the given key. 
 *       Source data is an key=value pair string, seperated by an "&" from an query string
 *       WARNING: FOR FIRST and LAST CHAR ONLY ALPHA OR NUMMERICALAL IS ALLOWED !	
 * Note:  kvArgsText // query keys dürfen keine encoded zeichen enthalten!
 * Para: int num -> number of implemented KEYs for this query-type
 *       const implementedKeys *XX_IK -> list of implemented KEYs for this query-type
 *       const char *QueryStr -> ptr to zero-teriminated KEY=VALUE string from query, seperated by '&'
 * Rets: struct SCDE_XX_parsedKVInput *parsedKVInput -> allocad struct, filled with data
 *       DONT FORGET TO FREE MEMORY !
 *--------------------------------------------------------------------------------------------------
 */
//TO DO:parse eines doppelten keys erkennen-> NULL zurück
//      parse unbekannter keys erkennen -> NULL zurück
//      FEHLER -> NULL zurück
parsedKVInputArgs_t*
ParseKVInputArgs(int numImplementedKeys
			,const kvParseImplementedKeys_t *XX_IK
			,const uint8_t *kvArgsText
			,const size_t kvArgsTextLen)
{

  // alloc memory for ESP32_S0_parsedKVInput_t -> length is variable !
  int memlen =  8 + 8 + 8 + 4 + (numImplementedKeys * (2+2+4));

  #if ESP32_S0_DBG >= 5
  LOGD("|PKVInput-malloc:%d, input:\"%.*s\">"
	,memlen
	,kvArgsTextLen
	,kvArgsText);
  #endif

  parsedKVInputArgs_t *parsedKVInputArgs =
	(struct parsedKVInputArgs_s *) malloc(memlen);

  // zero the bulk update structure (bulkUpdateReadings_t)
 // memset(parsedKVInputArgs, 0, memlen);

  // clear keys-found bitfield -> later user can detect what keys are found
  parsedKVInputArgs->keysFoundBF = 0;

  // reset affected Readings -> option for user to set affected readings, start cleared
  parsedKVInputArgs->affectedReadingsBF = 0;

  // implemented keys loop
  int i;
  for ( i = 0 ; i < numImplementedKeys ; i++ ) {

	// get ptr to current key
	const char *key = XX_IK[i].implementedKey;

	#if ESP32_S0_DBG >= 5
	LOGD("|chk for KEY:%d-\"%s\", "
		,i
		,key);
	#endif

	const uint8_t *p = kvArgsText;		// kv-args start ptr
	const uint8_t *ep = kvArgsText + kvArgsTextLen;	// kv-args end ptr
	const uint8_t *e;			// end ptr 
	int keylen = strlen(key);		// lenght of implemented key

//	while ( (p != NULL) && (*p != '\n') && (*p != '\r') && (*p != 0) )

	while ( (p != NULL) && (p < ep) ) {

		# if ESP32_S0_DBG >= 5
		LOGD("cmp:\"%.*s\""
			,keylen
			,p);
		# endif

		// matches value? and is "=" after value?
		if ( (strncasecmp((char*) p, key, keylen) == 0 ) && (p[keylen] == '=') ) {

			// move p to start of the VALUE (from this KEY)
			p += keylen + 1;

			// move e to end of the VALUE (from this KEY)
			e = (uint8_t *) strnstr((char*) p, "&", ep - p);

			// or is it last Key-Value pair?
			if (e == NULL) e = ep;

 			# if ESP32_S0_DBG >= 5
			LOGD(", next cmp:\"%.*s\""
				,(e-p)
				,p);
			# endif

			// store positive result -> do not forget to url-encode !
			parsedKVInputArgs->keysFoundBF |= (uint64_t) 1<<i;
			parsedKVInputArgs->keyData_t[i].off = (p-kvArgsText); // or better store ptr?
			parsedKVInputArgs->keyData_t[i].len = (e-p);
			parsedKVInputArgs->keyData_t[i].affectedReadings = 
				XX_IK[i].affectedReadings;

			// KEY found
			# if ESP32_S0_DBG >= 5
			LOGD(", found!>");
			# endif

			break;

			}

		// jump after next "&"
		p = (uint8_t*) strnstr((char*) p, "&", ep - p);

		if (p != NULL) p += 1;

		}

	// KEY not found
	# if ESP32_S0_DBG >= 5
	printf(">");
	# endif

	}

  // Query parsed complete
  # if SCDEH_DBG >= 5
  HexDumpOut ("|parsedKVInputArgs-HEX",
	parsedKVInputArgs,
	memlen);
  # endif

  return parsedKVInputArgs;

  }





