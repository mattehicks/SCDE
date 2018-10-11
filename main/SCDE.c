




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
#include "esp_heap_alloc_caps.h"

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
#include "SCDE_s.h"
#include "SCDE_Main.h"
#include "SCDE.h"





LOG_TAG("scde");




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





/*
 * DName: SCDERoot
 * Desc: Smart Connected Devices Engine - Root Data
 * Data: 
 */
SCDERoot_t SCDERoot;



/*
 * DName: SCDEFn
 * Desc: SCDEFn (functions / callbacks) for operation, provided / made accessible for modules
 * Data: 
 */
SCDEFn_t SCDEFn = {
   Log 
  ,Log3
  ,Log4
  ,AnalyzeCommandChain
  ,AnalyzeCommand
  ,GetLoadedModulePtrByName
  ,CommandReloadModule
//  ,CommandSet
  ,CommandUndefine
  ,readingsBeginUpdate
  ,readingsBulkUpdate
  ,readingsEndUpdate
  ,TimeNow
  ,FmtDateTime
  ,FmtTime
  ,HexDumpOut
  ,ParseKVInputArgs
  ,CallGetFnByDefName
};





/**
 * -------------------------------------------------------------------------------------------------
 *  FName: InitSCDERoot
 *  Desc: Initializes the SCDERoot data structure
 *  Info: 
 *  Para: -/- 
 *  Rets: -/-
 * -------------------------------------------------------------------------------------------------
 */
void
InitSCDERoot(void)
{

  SCDERoot.SCDEFn = &SCDEFn;

  STAILQ_INIT(&SCDERoot.headAttributes);

  STAILQ_INIT(&SCDERoot.headCommands);

  STAILQ_INIT(&SCDERoot.HeadCommon_Definitions);

  STAILQ_INIT(&SCDERoot.HeadModules);

}



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: doGlobalDef
 *  Desc: Initializes the global device
 *  Para: const uint8_t *cfgFileName -> Cfg-File-Name
 *        const size_t cfgFileNameLen -> length of the Cfg-File-Name
 *  Rets: Module_t* -> Pointer to Module / NULL if not found
 * -------------------------------------------------------------------------------------------------
 */
void
doGlobalDef(const uint8_t *cfgFileName
		, const size_t cfgFileNameLen)
  {

  LOGD("doGlobalDef - cfgFileName:%.*s\n"
	,cfgFileNameLen
	,cfgFileName);

  SCDERoot.DevCount = 1;

/*
3326	  $defs{global}{NR}    = $devcount++;
3327	  $defs{global}{TYPE}  = "Global";
3328	  $defs{global}{STATE} = "no definition";
3329	  $defs{global}{DEF}   = "no definition";
3330	  $defs{global}{NAME}  = "global";
3331	
3332	  CommandAttr(undef, "global verbose 3");
3333	  CommandAttr(undef, "global configfile $arg");
3334	  CommandAttr(undef, "global logfile -");
3335	}
3336	
*/


  // start the internal WebIF (built in module)
  // define Name:MyTelnet Module:Telnet -> Args: Port:23
//#define CMD_4_TELNET "define MyTelnet Telnet 23"

  // there should be no return messages - we expect no return messages
  AnalyzeCommand((const uint8_t *) "attr global verbose 3", 21);
  AnalyzeCommand((const uint8_t *) "attr global logfile -", 21);

  return;

  }













/** Category: attr-management helper
 * -------------------------------------------------------------------------------------------------
 *  FName: GetAttrValTextByDefTextAttrText
 *  Desc: Returns the attribute value assigned to an attribute-name, for the definition
 *  Note: DO NOT FORGET TO FREE MEMORY strText_t *attrVal->strText, strText_t *attrVal !
 *  Para: const strText_t *defName -> ptr to the definition name
 *        const strText_t *attrName -> ptr to the attribute name
 *  Rets: strText_t *attrVal -> the attribute value / NULL = not fnd., attrVal->strText = NULL -> empty
 * -------------------------------------------------------------------------------------------------
 */
strText_t*
GetAttrValTextByDefTextAttrText(const strText_t *defName
				,const strText_t *attrName)  
{

  strText_t *attrVal;

  // loop through the assigned attributes and try to find the existing attribute
  attribute_t *attribute = STAILQ_FIRST(&SCDERoot.headAttributes);

  while (true) {

	// no assigned attribute found ?
	if (attribute == NULL) {

	// not found
	attrVal = NULL;

	return attrVal;

	}

	// is this the attribute we are searching for?
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

			attrVal->strText = NULL;

		}

		if (attribute->attrValText) {

			LOGD("Got attrVal:%.*s assigned to attrName:%.*s for defName:%.*s\n"
				,attrVal->strTextLen
				,attrVal->strText
				,attribute->attrNameTextLen
				,attribute->attrNameText
				,attribute->attrAssignedDef->nameLen
				,attribute->attrAssignedDef->name);
		}

		else {

			LOGD("Got an empty attrVal! No value assigned to attrName:%.*s for defName:%.*s\n"
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
	
  const char cfgErrMsg[] = "Messages collected while initializing FHEM:";

  // Goal: include the config file (574-581)
	
  // for building the summary of ret-msg from cfg file including (starts empty)
  strText_t cfgRet = {NULL, 0};
	
  // get initial config-filename from value of attribute (global->configfile)
  strText_t defName = {(char*) "global", 6};
  strText_t attrName = {(char*) "configfile", 10};
  strText_t *valueName = GetAttrValTextByDefTextAttrText(&defName, &attrName);

// -------------------------------------------------------------------------------------------------

  // create the args to execute the initial 'include cfg' command
  strText_t includeCommandArgs;

  includeCommandArgs.strTextLen =
	asprintf((char**) &includeCommandArgs.strText
		,"include %.*s"
		,(int) valueName->strTextLen
		,(char*) valueName->strText);

  // call command include to process the initial config-file
  struct headRetMsgMultiple_s headRetMsgMultipleFromFn =
	AnalyzeCommandChain((uint8_t*)includeCommandArgs.strText, (const size_t) includeCommandArgs.strTextLen);
//??	AnalyzeCommand(includeCommandArgs.strText, includeCommandArgs.strTextLen);
//	AnalyzeCommandChain(includeCommandArgs.strText, includeCommandArgs.strTextLen);

  // free value from Fn GetAttrValTextByDefTextAttrText
  if (valueName) {
	  
	    if (valueName->strText) free(valueName->strText);

	    free(valueName);

  }

  free(includeCommandArgs.strText);

// -------------------------------------------------------------------------------------------------

  // if entries from retMsgMultiple add "configfile:" text to cfgRet
  if (!STAILQ_EMPTY(&headRetMsgMultipleFromFn)) {
	  
	cc   strText_t includeCommandArgs;
	  
		// if starting: build text header and start adding first ret-msg ...
	if (!cfgRet.strText) {

		// create header text
		cfgRet.strTextLen = asprintf((char**) &cfgRet.strText
			,"configfile: %.*s"
			,(int) retMsg->strTextLen
			,(char*) retMsg->strText);
	}
	  
	  
	  
	  
	  
	  const char cfgErrMsg[] = "Messages collected while initializing FHEM:";  
	  
	// Reallocate memory to new size
	cfgRet.strText = (char *) realloc(cfgRet.strText
		,cfgRet.strTextLen + retMsg->strTextLen);

	// add command-part to allocated memory
	memcpy(cfgRet.strText + cfgRet.strTextLen
		,retMsg->strText
		,retMsg->strTextLen);

	// save new length
	cfgRet.strTextLen += retMsg->strTextLen;
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  	
  // get the entries from retMsgMultiple till empty, and add to cfgRet text
  while (!STAILQ_EMPTY(&headRetMsgMultipleFromFn)) {

	// for processing the retMsg elements
	strTextMultiple_t *retMsg =
		STAILQ_FIRST(&headRetMsgMultipleFromFn);

	// Reallocate memory to new size
	cfgRet.strText = (char *) realloc(cfgRet.strText
		,cfgRet.strTextLen + retMsg->strTextLen);

	// add command-part to allocated memory
	memcpy(cfgRet.strText + cfgRet.strTextLen
		,retMsg->strText
		,retMsg->strTextLen);

	// save new length
	cfgRet.strTextLen += retMsg->strTextLen;
	
	// done, remove this entry
	STAILQ_REMOVE_HEAD(&headRetMsgMultipleFromFn, entries);
//	STAILQ_REMOVE(&headRetMsgMultipleFromFn, retMsg, strTextMultiple_s, entries);

	// free the msg-string
	free(retMsg->strText);

	// and the strTextMultiple_t
	free(retMsg);
  }

// -------------------------------------------------------------------------------------------------

  // debug info ..
  if (cfgRet.strText) {

	LOGD("cfgRet filled: %.*s"
			,cfgRet.strTextLen
			,(char*) cfgRet.strText);

  }
  else {

	LOGD("cfgRet NOT filled!");
  }

// -------------------------------------------------------------------------------------------------

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








// -------------------------------------------------------------------------------------------------

  // try to get the port (for telnet) from value of attribute (global->port)
  strText_t defNameGlobal = {(char *) "global", 6};
  strText_t attrNamePort = {(char *) "port", 4};
  strText_t *valueNamePort =
//	GetAttrValTextByDefTextAttrText( {(uint8_t*) "global", 6}, {(uint8_t*) "port", 4});
	GetAttrValTextByDefTextAttrText(&defNameGlobal, &attrNamePort);

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

//#include <stdarg.h>
/* --------------------------------------------------------------------------------------------------
 *  FName: Log3
 *  Desc: This is the main logging function with 3 infos: definition, loglevel and log-text
 *  Info: Level 0=System; 16=debug | DO NOT FORGET TO FREE char* LogText -> ITS ALLOCATED MEMORY !!!
 *  Para: Common_Definition_t* Common_Definition -> ptr to the Device-Definition which wants a log entry
 *        uint8_t LogLevel -> Level of information - to decide if it should be finally logged
 *        char* LogText -> ptr to allocated mem, containing text-string that should be logged
 *  Rets: -/-
 * --------------------------------------------------------------------------------------------------
 */
void //IRAM_ATTR
Log3 (const uint8_t *name
		,const size_t nameLen
		,const uint8_t LogLevel
		,const char *format
	,...)
  {

// 2016.10.12 07:26:16 LogPrioNr: Define->customname:

	// generate into Log3Fn|2016.10.12 07:26:16 2: LiGHT.2_F4B64:
  LOGD("Log3Fn|2016.10.12 07:26:16 %d: %.*s: "
	,LogLevel
	,nameLen
	,name);


  time_t now;
      struct tm timeinfo;
      time(&now);
      localtime_r(&now, &timeinfo);

      char strftime_buf[64];

      // Set timezone to Eastern Standard Time and print local time
         setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
         tzset();
         localtime_r(&now, &timeinfo);
         strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
         LOGD("New York is: %s"
        	,strftime_buf);


  va_list list;
  va_start(list, format);
  vprintf(format, list);
  va_end(list);



  //= GetAssignedAttribute("global","verbose");

 // is Device
//stp1: $dev = $dev->{NAME} if(defined($dev) && ref($dev) eq "HASH");

/*stp2:
  if(defined($dev) &&
     defined($attr{$dev}) &&
     defined (my $devlevel = $attr{$dev}{verbose}))
	{
    	return if($loglevel > $devlevel);
	}
  else
	{
	return if($loglevel > $attr{global}{verbose});
	}*/




/*  else
	{
	if ( > GetAttribute("global","verbose")


  char* DevVerbose = GetAttribute(Dev,"verbose");
  char* GlobalVerbose = GetAttribute("global","verbose");
  if ( (GlobalVerbose) &&
*/
  }



/* --------------------------------------------------------------------------------------------------
 *  FName: CommandActivateCommand
 *  Desc: Activates a Command which is included in this build (internal module) by adding it to
 *        SCDE Root (Commands)
 *  Info: Should be used if loading is not possible ...  
 *  Para: ProvidedByCommand_t* ProvidedByCommand -> ptr to in built included Command
 *  Rets: Command_t* -> Pointer to Command / NULL on error
 * --------------------------------------------------------------------------------------------------
 */
command_t*
CommandActivateCommand (providedByCommand_t* providedByNEWCommand)
  {

  // Call the Initialize Function
  providedByNEWCommand->initializeCommandFn(&SCDERoot);

  // prepare new Command and store in SCDE root
  command_t* newCommand;
  newCommand = (command_t*) malloc(sizeof(command_t));
  newCommand->providedByCommand = providedByNEWCommand;

  //  NewModule->LibHandle = LibHandle;
  STAILQ_INSERT_HEAD(&SCDERoot.headCommands, newCommand, entries);

  Log("HCTRL",16,"Command xx activated by main Fn\n");

  return newCommand;

  }



/* --------------------------------------------------------------------------------------------------
 *  FName: CommandActivateModule
 *  Desc: Activates a Module which is included in this build (internal module) by adding it to
 *        SCDE Root (Modules)
 *  Info: Should be used if loading is not possible ...  
 *  Para: ProvidedByModule_t* ProvidedByModule -> ptr to in built included module
 *  Rets: Module_t* -> Pointer to Module / NULL on error
 * --------------------------------------------------------------------------------------------------
 */
Module_t*
CommandActivateModule (ProvidedByModule_t* ProvidedByNEWModule)
  {

  // Call the Initialize Function
  ProvidedByNEWModule->InitializeFn(&SCDERoot);

  // prepare new module and store in SCDE root
  Module_t* NewModule;
  NewModule = (Module_t*) malloc(sizeof(Module_t));
  NewModule->ProvidedByModule = ProvidedByNEWModule;

  //  NewModule->LibHandle = LibHandle;
  STAILQ_INSERT_HEAD(&SCDERoot.HeadModules, NewModule, entries);

  Log("HCTRL",16,"Module xx activated by main Fn\n");

  return NewModule;

  }




/* --------------------------------------------------------------------------------------------------
 *  FName: CommandReload
 *  Desc: Initially loads or executes a reload of an Module of given Type-Name
 *  Info:  
 *  Para: const uint8_t typeName -> Text string of Modules Type-Name that should be reloaded 
 *        const size_t typeNameLen -> length of the typeName
 *  Rets: Module_t* -> Pointer to Module / NULL on error
 *--------------------------------------------------------------------------------------------------
 */
Module_t*
CommandReloadModule(const uint8_t *typeName
	,const size_t typeNameLen)
  {

  //  printf ("Command Reload called. Loading Module Type: %s", text);
  Log("HCTRL",16,"Loading Module/n");

  // we need ptr to Module_t. Contains function callbacks for module operation
  ProvidedByModule_t* ProvidedByNEWModule;

  ProvidedByNEWModule = (ProvidedByModule_t*) malloc(sizeof(ProvidedByModule_t));
  // search List of embedded Modules by TypeName



//  ProvidedByNEWModule =

/*  // loading external Module
  else
	{

	// handle for the lib
	void *LibHandle;

	// lib handle 
	LibHandle = my_load_dyn (TypeName);

	// Get ptr to ProvidedByModule_t. We need the function callbacks for module operation
	ProvidedByModule_t* ProvidedByNEWModule;
	printf ("Die Werte sind nicht gleich\n");
	ProvidedByNEWModule = dlsym(LibHandle, "ProvidedByModule");

	}
*/


  Log("HCTRL",16,"Module loaded/n");
/*
  if (LoadedModule == NULL)

		{

		printf ("Fehler bei dlsym() kiadmodule: %s\n", dlerror ());
		exit (EXIT_FAILURE);

		}
*/


  // Call the Initialize Function
//  ProvidedByNEWModule->InitializeFn(&SCDEFn);

  // prepare new module and store 
  Module_t* NewModule;
  NewModule = (Module_t*) malloc(sizeof(Module_t));
  NewModule->ProvidedByModule = ProvidedByNEWModule;
//  NewModule->LibHandle = LibHandle;
  STAILQ_INSERT_HEAD(&SCDERoot.HeadModules, NewModule, entries);

  Log("HCTRL",16,"Module initializzed/n");

/*
  Log3((const uint8_t*) Name
	,sizeof(Name)
	,1
	,"CommandAttr called. Creating for definition <%s> a new AttrName <%s> with AttrValue <%s>"
	,Name
	,AttrName
	,AttrValue);
*/


 // debug: list currently stored modules
  Module_t *Module;
  STAILQ_FOREACH(Module, &SCDERoot.HeadModules, entries)
	{
	LOGD("LM Name:\"%.*s\"\n"
		,Module->ProvidedByModule->typeNameLen
		,Module->ProvidedByModule->typeName);
	}

  return NewModule;

  }



/* --------------------------------------------------------------------------------------------------
 *  FName: CallGetFnByDefName
 *  Desc: xxx Creates a new Define with name "Name", and Module "TypeName" and calls Modules DefFn with
 *        args "Args"
 *  Info: 'Name' is custom definition name [azAZ09._] char[31]
 *        'TypeName' is module name
 *        'Definition+X' is passed to modules DefineFn, and stored in Definition->Definition
 *  Para: const uint8_t *args  -> prt to space seperated command text string "Command ..."
 *        const size_t argsLen -> length of args
 *  Rets: struct headRetMsgMultiple_s -> head from STAILQ, stores multiple RetMsg (errors), NULL=OK
 * --------------------------------------------------------------------------------------------------
 */
int
CallGetFnByDefName(const uint8_t *nameText
	,const size_t nameTextLen
	,Common_Definition_t *sourceCommon_Definition
	, void *X)
{

  int retInt = 0;

  // get the Common_Definition by Name
  Common_Definition_t *Common_Definition;

  STAILQ_FOREACH(Common_Definition, &SCDERoot.HeadCommon_Definitions, entries) {

      if ( (Common_Definition->nameLen == nameTextLen)
          && (!strncasecmp((const char*) Common_Definition->name, (const char*) nameText, nameTextLen)) ) {

              if (Common_Definition->module->ProvidedByModule->GetFn) {

                  retInt = Common_Definition->module->ProvidedByModule->GetFn(Common_Definition, sourceCommon_Definition, X);

              }

              else {


              }

      }
  }

  return retInt;
}




/* --------------------------------------------------------------------------------------------------
 *  FName: AnalyzeCommandChain
 *  Desc: Creates a new Define with name "Name", and Module "TypeName" and calls Modules DefFn with
 *        args "Args"
 *  Info: 'Name' is custom definition name [azAZ09._] char[31]
 *        'TypeName' is module name
 *        'Definition+X' is passed to modules DefineFn, and stored in Definition->Definition
 *  Para: const uint8_t *args  -> prt to space seperated command text string "Command ..."
 *        const size_t argsLen -> length of args
 *  Rets: struct headRetMsgMultiple_s -> head from STAILQ, stores multiple RetMsg (errors), NULL=OK
 * --------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s
AnalyzeCommandChain (const uint8_t *args
		, const size_t argsLen)
{

  // prepare STAILQ head for multiple RetMsg storage
  struct headRetMsgMultiple_s headRetMsgMultiple;

  // Initialize the queue
  STAILQ_INIT(&headRetMsgMultiple);

  // set start of possible Command
  const uint8_t *commandName = args;

  // set start of possible Command-Arguments
  const uint8_t *commandArgs = args;

  // a seek-counter
  int i = 0;

  // seek to next space !'\32'
  while( (i < argsLen) && (*commandArgs != ' ') ) {i++;commandArgs++;}

  // length of Name
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
  command_t *command;

  // search for the command
  STAILQ_FOREACH(command, &SCDERoot.headCommands, entries) {

	LOGD("testing: %.*s\n"
		,command->providedByCommand->commandNameTextLen
		,command->providedByCommand->commandNameText);

	if ( (command->providedByCommand->commandNameTextLen == commandLen)
		&& (!strncasecmp((const char*) command->providedByCommand->commandNameText, (const char*) commandName, commandLen)) ) {

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
 


/* --------------------------------------------------------------------------------------------------
 *  FName: AnalyzeCommand
 *  Desc: Creates a new Define with name "Name", and Module "TypeName" and calls Modules DefFn with
 *        args "Args"
 *  Info: 'Name' is custom definition name [azAZ09._] char[31]
 *        'TypeName' is module name
 *        'Definition+X' is passed to modules DefineFn, and stored in Definition->Definition
 *  Para: const uint8_t *args  -> prt to space seperated command text string "Command ..."
 *        const size_t argsLen -> length of args
 *  Rets: struct headRetMsgMultiple_s -> head from STAILQ, stores multiple RetMsg (errors), NULL=OK
 * --------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s
AnalyzeCommand (const uint8_t *args
		, const size_t argsLen)
{

  // prepare STAILQ head for multiple RetMsg storage
  struct headRetMsgMultiple_s headRetMsgMultiple;

  // Initialize the queue
  STAILQ_INIT(&headRetMsgMultiple);

//  // for Fn response msg
//  strTextMultiple_t *retMsg;

  // set start of possible Command
  const uint8_t *commandName = args;

  // set start of possible Command-Arguments
  const uint8_t *commandArgs = args;

  // a seek-counter
  int i = 0;

  // seek to next space !'\32'
  while( (i < argsLen) && (*commandArgs != ' ') ) {i++;commandArgs++;}

  // length of Name
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
  command_t *command;

  // search for the command
  STAILQ_FOREACH(command, &SCDERoot.headCommands, entries) {

	LOGD("testing: %.*s\n"
		,command->providedByCommand->commandNameTextLen
		,command->providedByCommand->commandNameText);

	if ( (command->providedByCommand->commandNameTextLen == commandLen)
		&& (!strncasecmp((const char*) command->providedByCommand->commandNameText, (const char*) commandName, commandLen)) ) {

		// call the CommandFn, if retMsg != NULL -> error ret Msg
		struct headRetMsgMultiple_s headRetMsgMultipleFromFn
			= command->providedByCommand->commandFn(commandArgs, commandArgsLen);

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



/* --------------------------------------------------------------------------------------------------
 *  FName: CommandDefine
 *  Desc: Creates a new common define with name "Name", and Module "Type-Name", prevills some common
 *        values and calls Modules DefFn with for further module-specific initialization.
 *  Info: 'Name' is custom definition name. Allowed: [azAZ09._], uint8_t[32]
 *        'Type-Name' is module name
 *        'Definition ...' is custom and passed to modules DefineFn, stored in Definition->Definition
 *  Para: const uint8_t *args  -> prt to space seperated define text string "Name Type-Name Definition ..."
 *        const size_t argsLen -> length of args
 *  Rets: char* -> error-text-string in allocated mem, or NULL = OK
 * --------------------------------------------------------------------------------------------------
 */
/*
char* ICACHE_FLASH_ATTR
CommandDefine (const uint8_t *args
		, const size_t argsLen)
  {

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
		asprintf(char**) &retMsg
				,"Could not interpret command '%.*s'! Usage: define <name> <type-name> <type dependent arguments>"
			   	,argsLen
				,args);

		return retMsg;

		}
*/
 /*
   if(defined($defs{$name}));
   asprintf(char**) &RetMsg, "%s already defined, delete it first\n", name);
   return RetMsg;

   if($name !~ m/^[a-z0-9.:_]*$/i);
   asprintf(char**) &RetMsg, "Invalid characters in name (not A-Za-z0-9._): %s\n", name);
   return RetMsg;
  */
/*

   // get the module ptr by name
   Module_t* Module = GetLoadedModulePtrByName(typeName
		   , typeNameLen);

  // do we need to reload Module?
  if (!Module) // NOT FUNCTIONAL!
	Module = CommandReload(typeName
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
  STAILQ_INSERT_HEAD(&SCDERoot.HeadCommon_Definitions, NewCommon_Definition, entries);

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
  NewCommon_Definition->nr = SCDERoot.DevCount++;

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

  }
*/


/**
  * --------------------------------------------------------------------------------------------------
  * FName: CommandSet -> gets arguments from SET 'Name Args'
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

/*
char* ICACHE_FLASH_ATTR
CommandSet(const uint8_t *args, const size_t argsLen)	//SET 'Name Args' -> Args = 'Name Args'
  {

	char *Args = (char*) args;

  int ArgsLen = strlen(Args);

  // did we get any args ?
  if (!ArgsLen) return NULL;

  // copy args to ram for modifications
  char* Input = malloc (ArgsLen + 1);
  strcpy(Input, Args);

  // prepare the sting* containing the Name
  char* Name = Input;

  // if ' ' is found next steps -> zero terminate Name*
  // + use ptr as start of sting* for the Args*
  char* SetArgs = strchr(Input, ' ');

  if (SetArgs != NULL)

	{

	*SetArgs++ = '\0';

	}

  else

	// we need SetArgs ! -> error
	{

	free(Input);

	return NULL;

	}

   // optimize error msg
   //-> my @a = split("[ \t][ \t]*", $param);
   //->return "Usage: set <name> <type-dependent-options>\n $namedef" if(int(@a)<1);


  // get the Common_Definition by Name
  Common_Definition_t *Common_Definition;

  STAILQ_FOREACH(Common_Definition, &SCDERoot.HeadCommon_Definitions, entries)

	{

	if (!strcmp((char*)Common_Definition->name,Name))
		break;

	}

  // did we get no Common_Definition?
  if (strcmp((char*)Common_Definition->name,Name))

  	  {

		free(Input);

		return NULL;

  	  }

  printf("\nCalling SetFN for Name:%.*s TypeName:%.*s Set-args:%s"
		  ,Common_Definition->nameLen
		  ,Common_Definition->name
		  ,Common_Definition->module->ProvidedByModule->typeNameLen
		  ,Common_Definition->module->ProvidedByModule->typeName
		  ,SetArgs);

  // call modules SetFn with set-args
  char* ret = Common_Definition->module->ProvidedByModule->SetFn(Common_Definition, SetArgs);

  // free resources
  free(Input);

  // do we have an error msg?
  if (ret)

	{

	printf("Got error from SetFN %s \n"
			  ,ret);


	}

  // char error* or NULL
  return ret;

  }
*/


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

  STAILQ_FOREACH(Common_Definition, &SCDERoot.HeadCommon_Definitions, entries)

	{

	if (!strcmp((char*)Common_Definition->name, Args))
		break;

	}

  // did we get no Common_Definition?
  if (strcmp((char*)Common_Definition->name,Args))

  	  {

		return retMsg;

  	  }

  LOGD("\nCalling UndefineFN for Name:%.*s TypeName:%.*s\n"
		  ,Common_Definition->nameLen
		  ,Common_Definition->name
		  ,Common_Definition->module->ProvidedByModule->typeNameLen
		  ,Common_Definition->module->ProvidedByModule->typeName);



  // call modules UndefineFn
  retMsg = Common_Definition->module->ProvidedByModule->UndefineFn(Common_Definition);

  // do we have an error msg?
  if (retMsg)

	{

	LOGD("Got error from UndefineFN %.*s \n"
			  ,retMsg->strTextLen
				,retMsg->strText);

	}

  // char error* or NULL
  return retMsg;

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: CommandAttr
 * Desc: Creates for definition with Name <name> a new AttrName <attrname> with AttrValue <attrvalue>
 *  
 * Info: 'Name' is custom definition name [azAZ09._] char[31]
 *       'TypeName' is module name
 *       'Definition+X' is passed to modules DefineFn, and stored in Definition->Definition
 * Para: char* Args -> prt to space seperated txt string "Name TypeName Definition..."
 * Rets: char* -> error-text-string in allocated mem, or NULL=OK
 *--------------------------------------------------------------------------------------------------
 */
char*
CommandAttr (char* Args)
  {

  // prepare the sting* containing the Name
  char* Name = Args;

  // prepare the sting* containing the AttrName
  char* AttrName = strchr(Args, ' ');

  if (AttrName != NULL)
	*AttrName++ = '\0';
  else return (NULL);

  // prepare the sting* containing the AttrValue
  char* AttrValue = strchr(AttrName, ' ');
  if (AttrValue != NULL)
	*AttrValue++ = '\0';
  //else no Definition  (may be ok)

  // required: Name+AttrName
  if (!((Name) && (AttrName)) )
	return "Usage: attr <name> <attrname> [<attrvalue>] $namedef";

  Log3((const uint8_t*) Name
	,sizeof(Name)
	,1
	,"CommandAttr called. Creating for definition <%s> a new AttrName <%s> with AttrValue <%s>"
	,Name
	,AttrName
	,AttrValue);




  return NULL;

  }






/* --------------------------------------------------------------------------------------------------
 *  FName: GetLoadedModulePtrByName
 *  Desc: Returns the ptr to an already loaded Module
 *  Para: const uint8_t *typeName -> Type-Name
 *        const size_t typeNameLen -> length of the Type-Name
 *  Rets: Module_t* -> Pointer to Module / NULL if not found
 * --------------------------------------------------------------------------------------------------
 */
Module_t*
GetLoadedModulePtrByName(const uint8_t *typeName
		, const size_t typeNameLen)
  {

  Module_t *Module;

  STAILQ_FOREACH(Module, &SCDERoot.HeadModules, entries) {

/*  	  printf("chk a:%.*s ax:%d, b:%.*s bx:%d,\n"
  			,typeNameLen
  			,typeName
			,typeNameLen
  			,Module->ProvidedByModule->typeNameLen
  			,Module->ProvidedByModule->typeName
			,Module->ProvidedByModule->typeNameLen);*/

	if ( (Module->ProvidedByModule->typeNameLen == typeNameLen)
		&& (!strncasecmp((const char*) Module->ProvidedByModule->typeName, (const char*) typeName, typeNameLen)) ) {

		LOGD("Type-Name:%.*s, got loaded module*.\n"
		  	,typeNameLen
		  	,typeName);

		return Module;

	}
  }

  LOGD("Type-Name:%.*s, module* NOT loaded!\n"
	,typeNameLen
	,typeName);

  return NULL;

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
		,commandFn_t commandFn
		,const uint8_t *commandHelp
		,const size_t commandHelpLen)
  {
/*
  command_t* NewCommand;

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


/*
 * --------------------------------------------------------------------------------------------------
 *  FName: readingsBeginUpdateFn
 *  Desc: Call readingsBeginUpdate before you start updating readings. The updated readings will all
 *       get the same timestamp, which is the time when you called this subroutine.
 *  Para: const uint8_t *commandTxt ->  ptr to the Command-Text
 *       const size_t commandTxtLen -> Command-Text length
 *       commandFn_t commandFn -> the command Fn
 *       const uint8_t *commandHelp -> ptr the Command-Help text
 *       const size_t commandHelpLen -> Command-Help text length
 *  Rets: -/-
 * --------------------------------------------------------------------------------------------------
 */
int
readingsBeginUpdate(Common_Definition_t *Common_Definition)
  {

  // check if bulk update is not already called
  if (Common_Definition->bulkUpdateReadings) {

	Log3(Common_Definition->module->ProvidedByModule->typeName
		  ,Common_Definition->module->ProvidedByModule->typeNameLen
		  ,1
		  ,"Error! readingsEndUpdateFn not called in Def-Name: '%.*s'. Can not begin new update.\n"
		  ,Common_Definition->name
		  ,Common_Definition->nameLen);

	return 0;

  }

  // alloc mem for reading structure (bulkUpdateReadings_t)
  Common_Definition->bulkUpdateReadings =
	(bulkUpdateReadings_t *) malloc(sizeof(bulkUpdateReadings_t));

  // zero the struct
  memset(Common_Definition->bulkUpdateReadings, 0, sizeof(bulkUpdateReadings_t));

  // clear bulk-update tail-queue
  STAILQ_INIT(&Common_Definition->bulkUpdateReadings->headReadings);






  // assign bulk update time stamp
  Common_Definition->bulkUpdateReadings->bulkUpdateTist = TimeNow();

  strText_t  strText =
	FmtDateTime(Common_Definition->bulkUpdateReadings->bulkUpdateTist);

  LOGD("HelloTest tist:%.*s"
	,strText.strTextLen
	,strText.strText);

  free(strText.strText);






  // assign bulk update time stamp
  time(&Common_Definition->bulkUpdateReadings->bulkUpdateTist);

  // display for debug
  struct tm timeinfo;
  localtime_r(&Common_Definition->bulkUpdateReadings->bulkUpdateTist, &timeinfo);

  printf("readingsBeginUpdate tist: %d.%d.%d, %d:%d:%d\n"
	,timeinfo.tm_mday
	,timeinfo.tm_mon+1
	,timeinfo.tm_year+1900
	,timeinfo.tm_hour
	,timeinfo.tm_min
	,timeinfo.tm_sec);


  

/*
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



  LOGD("readingsBeginUpdate called, Type-Name:%.*s, Def-Name:%.*s\n"
	,Common_Definition->module->ProvidedByModule->typeNameLen
	,Common_Definition->module->ProvidedByModule->typeName
	,Common_Definition->nameLen
	,Common_Definition->name);

  return 0;
  }



/*
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
int
readingsBulkUpdate(Common_Definition_t *Common_Definition
		,uint8_t *readingNameText
		,size_t readingNameTextLen
		,uint8_t *readingValueText
		,size_t readingValueTextLen)
  {

  // check if bulk update begin was called
  if (!Common_Definition->bulkUpdateReadings) {

	Log3(Common_Definition->module->ProvidedByModule->typeName
		  ,Common_Definition->module->ProvidedByModule->typeNameLen
		  ,1
		  ,"Error! readingsBulkUpdateFn called without calling readingsBeginUpdateFn first in Def-Name: '%.*s'.\n"
		  ,Common_Definition->name
		  ,Common_Definition->nameLen);

	return 0;

  }


  // alloc mem for reading structure (reading_t)
  reading_t *newReading
	= malloc(sizeof(reading_t));

  // zero the struct
  memset(newReading, 0, sizeof(reading_t));

  // copy reading data
  newReading->readingNameText =
	readingNameText;
  newReading->readingNameTextLen =
	readingNameTextLen;
  newReading->readingValueText =
	readingValueText;
  newReading->readingValueTextLen =
	readingValueTextLen;

  // store new reading to SCDE-Root
  STAILQ_INSERT_HEAD(&Common_Definition->bulkUpdateReadings->headReadings, newReading, entries);

  LOGD("readingsBulkUpdate called for reading:%.*s value:%.*s\n"
  			,readingNameTextLen
  			,readingNameText
  			,readingValueTextLen
  			,readingValueText);


  // list currently added readings stored for processing
  reading_t *reading;
  STAILQ_FOREACH(reading, &Common_Definition->bulkUpdateReadings->headReadings, entries) {
	LOGD("L readingName:%.*s, readingValue:%.*s\n"
		,reading->readingNameTextLen
		,reading->readingNameText
		,reading->readingValueTextLen
		,reading->readingValueText);
  }

  return 0;

  }



/*http://sancho.ccd.uniroma2.it/cgi-bin/man/man2html?STAILQ_INIT+3
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
int
readingsEndUpdate(Common_Definition_t *Common_Definition)
  {

  // check if bulk update begin was called
  if (!Common_Definition->bulkUpdateReadings) {

	Log3(Common_Definition->module->ProvidedByModule->typeName
		  ,Common_Definition->module->ProvidedByModule->typeNameLen
		  ,1
		  ,"Error! readingsEndUpdateFn called without calling readingsBeginUpdateFn first in Def-Name: '%.*s'.\n"
		  ,Common_Definition->name
		  ,Common_Definition->nameLen);

	return 0;

  }

  printf("readingsEndUpdate called. Now processing:\n");



  // loop through the bulk-update-readings
  reading_t *reading = STAILQ_FIRST(&Common_Definition->bulkUpdateReadings->headReadings);
  reading_t *nextReading;
  while (reading != NULL) {

	nextReading = STAILQ_NEXT(reading, entries);

	// set common tist
	reading->readingTist =
		Common_Definition->bulkUpdateReadings->bulkUpdateTist;

	LOGD("Now proc. readingName:%.*s, readingValue:%.*s\n"
		,reading->readingNameTextLen
		,reading->readingNameText
		,reading->readingValueTextLen
		,reading->readingValueText);

	// loop through old readings and try to find an old reading and replace it. Or add the new reading.
	reading_t *oldReading = STAILQ_FIRST(&Common_Definition->headReadings);
	while (true) {

		// no old reading found ?
		if (oldReading == NULL) {

			// add new reading at tail
			STAILQ_INSERT_HEAD(&Common_Definition->headReadings, reading, entries);

			LOGD("Added new reading - readingName:%.*s, readingValue:%.*s\n"
				,reading->readingNameTextLen
				,reading->readingNameText
				,reading->readingValueTextLen
				,reading->readingValueText);

			// added new, break
			break;
		}

		// is this an old value for this reading
		if ( (oldReading->readingNameTextLen == reading->readingNameTextLen)
			&& (!strncmp((const char*) oldReading->readingNameText, (const char*) reading->readingNameText, reading->readingNameTextLen)) ) {

			// replace old value for this reading
			if (oldReading->readingNameText) free(oldReading->readingNameText);
			if (oldReading->readingValueText) free(oldReading->readingValueText);
			oldReading->readingNameTextLen = reading->readingNameTextLen;
			oldReading->readingNameText = reading->readingNameText;
			oldReading->readingValueTextLen = reading->readingValueTextLen;
			oldReading->readingValueText = reading->readingValueText;
			oldReading->readingTist =
				Common_Definition->bulkUpdateReadings->bulkUpdateTist;

			// we have taken the data - free reading
			free(reading);

			LOGD("Updated old reading - readingName:%.*s, readingValue:%.*s\n"
				,oldReading->readingNameTextLen
				,oldReading->readingNameText
				,oldReading->readingValueTextLen
				,oldReading->readingValueText);

			// found, break
			break;
		}

		// get next reading for processing
		oldReading = STAILQ_NEXT(oldReading, entries);

	}

	// goto next reading for processing
	reading = nextReading;
  }

  printf("readingsEndUpdate finnished. Current readings for this definiton:\n");

  // clear bulk-update tail-queue
//  STAILQ_INIT(&Common_Definition->bulkUpdateReadings->headReadings);


  // dealloc and clear bulk-update data
  free(Common_Definition->bulkUpdateReadings);
  Common_Definition->bulkUpdateReadings = NULL;















/*
  // list currently added readings stored for processing
  reading_t *reading;
  STAILQ_FOREACH(reading, &Common_Definition->bulkUpdateReadings->headReadings, entries) {

	printf("Processing readingName:%.*s, readingValue:%.*s\n"
		,reading->readingNameTextLen
		,reading->readingNameText
		,reading->readingValueTextLen
		,reading->readingValueText);

	// loop and find old reading for this new reading ?
	reading_t *oldReading;
	STAILQ_FOREACH(oldReading, &Common_Definition->headReadings, entries) {

		// is this an old value for this reading
		if ( (oldReading->readingNameTextLen == reading->readingNameTextLen)
			&& (!strncmp((const char*) oldReading->readingNameText, (const char*) reading->readingNameText, reading->readingNameTextLen)) ) {

		// replace old value for this reading
		if (oldReading->readingNameText) free(oldReading->readingNameText);
		if (oldReading->readingValueText) free(oldReading->readingValueText);
		oldReading->readingNameTextLen = reading->readingNameTextLen;
		oldReading->readingNameText = reading->readingNameText;
		oldReading->readingValueTextLen = reading->readingValueTextLen;
		oldReading->readingValueText = reading->readingValueText;

		printf("Updated old reading - readingName:%.*s, readingValue:%.*s\n"
			,oldReading->readingNameTextLen
			,oldReading->readingNameText
			,oldReading->readingValueTextLen
			,oldReading->readingValueText);

		}

		// no old value for this reading found
		else if (oldReading->entries == NULL) {

			// store new reading to SCDE-Root
			STAILQ_INSERT_AFTER(&Common_Definition->bulkUpdateReadings->headReadings, newReading, entries);

		}
	



	}

  }
*/

  // list readings stored for definition after processing
  reading_t *readingNow;
  STAILQ_FOREACH(readingNow, &Common_Definition->headReadings, entries) {
	LOGD("L readingName:%.*s, readingValue:%.*s\n"
		,readingNow->readingNameTextLen
		,readingNow->readingNameText
		,readingNow->readingValueTextLen
		,readingNow->readingValueText);
  }



  return 0;

  }







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

















/* helper
 * --------------------------------------------------------------------------------------------------
 *  FName: TimeNow
 *  Desc: Returns the current time stamp
 *  Para: -/-
 *  Rets: time_t -> current time-stamp
 * --------------------------------------------------------------------------------------------------
 */
time_t
TimeNow()
{

  time_t timeNow;

  // assign time stamp
  time(&timeNow);

  return timeNow;

}



/* helper
 * --------------------------------------------------------------------------------------------------
 *  FName: FmtDateTime 
 *  Desc: Creates formated date-time-text (uint8_t style - in this case zero terminated) from given
 *        time-stamp. Returned in msgText_t (text in allocated memory + length information)
 *  Note: DO NOT FORGET TO FREE MEMORY !! 
 *  Para: time_t tiSt -> the time-stamp that should be used
 *  Rets: strText_t -> formated date-time-text data
 * --------------------------------------------------------------------------------------------------
 */
strText_t
FmtDateTime(time_t tiSt)
{

  // our msg-text data packet
  strText_t strText;

  // get timeinfo for time-stamp
  struct tm timeinfo;
  localtime_r(&tiSt, &timeinfo);

  // prepare formated-time-string in allocated memory
  strText.strTextLen = asprintf((char**) &strText.strText
	,"%04d-%02d-%02d %02d:%02d:%02d"
	,timeinfo.tm_year+1900
	,timeinfo.tm_mday
	,timeinfo.tm_mon+1
	,timeinfo.tm_hour
	,timeinfo.tm_min
	,timeinfo.tm_sec);

  return strText;

}



/* helper
 * --------------------------------------------------------------------------------------------------
 *  FName: FmtTime
 *  Desc: Creates formated time-text (uint8_t style - in this case zero terminated) from given
 *        time-stamp. Returned in msgText_t (text in allocated memory + length information)
 *  Note: DO NOT FORGET TO FREE MEMORY !! 
 *  Para: time_t tiSt -> the time-stamp that should be used
 *  Rets: strText_t -> formated time-text data
 * --------------------------------------------------------------------------------------------------
 */
strText_t
FmtTime(time_t tiSt)
{

  // our msg-text data packet
  strText_t strText;

  // get timeinfo for time-stamp
  struct tm timeinfo;
  localtime_r(&tiSt, &timeinfo);

  // prepare formated-time-string in allocated memory
  strText.strTextLen = asprintf((char**) &strText.strText
	,"%02d:%02d:%02d"
	,timeinfo.tm_hour
	,timeinfo.tm_min
	,timeinfo.tm_sec);

  return strText;

}



/* helper
 * -------------------------------------------------------------------------------------------------
 *  FName: HexDumpOut
 *  Desc: Prints data as Hex-Dump to debug terminal
 *  Info:  
 *  Para: char *desc -> ptr to leading description
 *        void *addr -> ptr to beginning of data data to dump
 *        int len -> length of the data to dump 
 *  Rets: -/-
 * -------------------------------------------------------------------------------------------------
 */
void
HexDumpOut (char *desc, void *addr, int len) 
{

  int i;
  unsigned char buff[17];
  unsigned char *pc = (unsigned char*)addr;

  // Output description if given.
  if (desc != NULL) printf ("%s:\n", desc);

  // Process every byte in the data.
  for (i = 0; i < len; i++) {

        // Multiple of 16 means new line (with line offset).
        if ((i % 16) == 0) {

		// Just don't print ASCII for the zeroth line.
            	if (i != 0) printf ("  %s\n", buff);

		// Output the offset.
		printf ("  %04x ", i);

	}

	// Now the hex code for the specific character.
	printf (" %02x", pc[i]);

	// And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) buff[i % 16] = '.';

        else buff[i % 16] = pc[i];

        buff[(i % 16) + 1] = '\0';

  }

  // Pad out last line if not exactly 16 characters.
  while ((i % 16) != 0) {

	printf ("   ");

	i++;

  }

  // And print the final ASCII bit.
  printf ("  %s\n", buff);

}




























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





