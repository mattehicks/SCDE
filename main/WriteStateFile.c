#include "ProjectConfig.h"
#include <esp8266.h>
#include "SCDE_s.h"

#include "SCDE.h"



/* --------------------------------------------------------------------------------------------------
 *  FName: WriteStatefile
 *  Desc: Writes the statefile to filesystem.
 *  Info: 
 *  Para: -/-
 *  Rets: struct headRetMsgMultiple_s -> head from STAILQ, stores multiple (all) readings
 *        from requested Definition, NULL=NONE
 * --------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s
WriteStatefile()
{
  // prepare multiple RetMsg storage
  struct headRetMsgMultiple_s headRetMsgMultiple;

  // Initialize the queue (init STAILQ head)
  STAILQ_INIT(&headRetMsgMultiple);

  // get attribute "global->statefile" value
  strText_t attrStateFNDefName = {(char*) "global", 6};
  strText_t attrStateFNAttrName = {(char*) "statefile", 9};
  strText_t *attrStateFNValueName =
		Get_attrVal_by_defName_and_attrName(&attrStateFNDefName, &attrStateFNAttrName);

	// attribute not found
	if (!attrStateFNValueName) {

		// alloc mem for retMsg
		strTextMultiple_t *retMsg =
			malloc(sizeof(strTextMultiple_t));

		// response with error text
		retMsg->strTextLen = asprintf(&retMsg->strText
			,"Error, Arribute %.*s not found in Definition %.*s !\r\n"
			,attrStateFNAttrName.strTextLen
			,attrStateFNAttrName.strText
			,attrStateFNDefName.strTextLen
			,attrStateFNDefName.strText);

		// insert retMsg in stail-queue
		STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

		// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
		return headRetMsgMultiple;
	}

	// attribute found, but value not assigned
	if (!attrStateFNValueName->strText) {
		
		// dealloc from FN GetAttrValTextByDefTextAttrText
		free(attrStateFNValueName);

		// alloc mem for retMsg
		strTextMultiple_t *retMsg =
			malloc(sizeof(strTextMultiple_t));

		// response with error text
		retMsg->strTextLen = asprintf(&retMsg->strText
			,"Error, arribute %.*s found in Definition %.*s, but no value is assigned !\r\n"
			,attrStateFNAttrName.strTextLen
			,attrStateFNAttrName.strText
			,attrStateFNDefName.strTextLen
			,attrStateFNDefName.strText);

		// insert retMsg in stail-queue
		STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

		// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
		return headRetMsgMultiple;
	}
	
	// my $now = gettimeofday();
	time_t now = TimeNow();
			
//todo: my @t = localtime($now);

//todo: $stateFile = ResolveDateWildcards($stateFile, @t);

	// create statefilename string
	char *stateFile;
	asprintf(&stateFile
			,"/spiffs/%.*s.cfg"
			,attrStateFNValueName->strTextLen
			,attrStateFNValueName->strText);
	 
	// free attribute statefile value
	free (attrStateFNValueName->strText);	 
	free (attrStateFNValueName);
		
	// open statefile
	FILE* sFH = fopen(stateFile, "w");
	if (sFH == NULL) {

		// alloc mem for retMsg
		strTextMultiple_t *retMsg =
			malloc(sizeof(strTextMultiple_t));

		// response with error text
		retMsg->strTextLen = asprintf(&retMsg->strText
			,"Error, could not open $stateFile: %s!\r\n"
			,stateFile);

//   #Log 1, $errormsg; ???

		free(stateFile);

		// insert retMsg in stail-queue
		STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

		// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
		return headRetMsgMultiple;
	}

	// free our prepared filename
	free(stateFile);	//Noch benötigt ? Vorher freigeben?, dann nicht doppelt

	// stores the time		
	struct tm timeinfo;

  // to fill with: "Sat Aug 19 14:16:59 2017"
	char strftime_buf[64];

/*
  // PREPARATIONS OF INTERNAL CLOCK
	localtime_r(&now, &timeinfo);

	// Set timezone to Eastern Standard Time and print local time
	setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
	tzset();
	// END OF PREPARATION
*/

  // get time to struct timeinfo
	localtime_r(&now, &timeinfo);

  // get strftime-text into strftime_buf 
	strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);

  // start statefile with date:-> #Sat Aug 19 14:16:59 2017
  fprintf(sFH,"#%s\r\n", strftime_buf);


			





 







/*
	                                               ## $d ist der Name!!!
 # foreach my $d (sort keys %defs) {
 #   next if($defs{$d}{TEMPORARY});		//temporäre nicht!!
 #   if($defs{$d}{VOLATILE}) {
 #     my $def = $defs{$d}{DEF};
 #     $def =~ s/;/;;/g; # follow-on-for-timer at
 #     print SFH "define $d $defs{$d}{TYPE} $def\n";
 #   }
 #   my @arr = GetAllReadings($d);
 #   print SFH join("\n", @arr)."\n" if(@arr);
 # }
 # return "$attr{global}{statefile}: $!" if(!close(SFH));
 # return "";
		*/	


	// loop the definition for processing
	Common_Definition_t *Common_Definition;
	STAILQ_FOREACH(Common_Definition, &SCDERoot.HeadCommon_Definitions, entries) {
		
//#       next if($defs{$d}{TEMPORARY});		//temporäre nicht!!
		
	/*	LOGD("calling GetAllReadings for:%.*s\n"
			,Common_Definition->nameLen
			,Common_Definition->name);*/

		struct headRetMsgMultiple_s headRetMsgMultipleFromFn =
			GetAllReadings(Common_Definition);

		// if RetMsgMultiple queue not empty -> got readings from definition
		if (!STAILQ_EMPTY(&headRetMsgMultipleFromFn)) {

			// get the queue entries from retMsgMultiple till empty
			while (!STAILQ_EMPTY(&headRetMsgMultipleFromFn)) {

				// get a retMsg element from queue
				strTextMultiple_t *retMsg =
					STAILQ_FIRST(&headRetMsgMultipleFromFn);

			/*	LOGD("store setstate line to File: %.*s\n"
					,retMsg->strTextLen
					,retMsg->strText);*/

				// store setstate line
				fprintf(sFH,"%.*s\n"
					,retMsg->strTextLen
					,retMsg->strText);

				// done, remove this entry
				STAILQ_REMOVE_HEAD(&headRetMsgMultipleFromFn, entries);

				free(retMsg->strText);
				free(retMsg);
			}
		}
	}

	// close statefile
	fclose(sFH);





//filecontent debug
int c;
FILE *file;
file = fopen("/spiffs/state.cfg", "r");
if (file) {
    while ((c = getc(file)) != EOF)
        putchar(c);
    fclose(file);
}


	// return STAILQ head, stores multiple retMsg with readings, if NULL -> none
	return headRetMsgMultiple;

}

