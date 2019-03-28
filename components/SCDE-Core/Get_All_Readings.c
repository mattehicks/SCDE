#include "ProjectConfig.h"
#include <esp8266.h>
#include "SCDE_s.h"

#include "SCDE.h"






/* --------------------------------------------------------------------------------------------------
 *  FName: GetAllReadings
 *  Desc: Creates a new Define with name "Name", and Module "TypeName" and calls Modules DefFn with
 *        args "Args"
 *  Info: 'Name' is custom definition name [azAZ09._] char[31]
 *        'TypeName' is module name
 *        'Definition+X' is passed to modules DefineFn, and stored in Definition->Definition
 *  Para: Common_Definition_t *Common_Definition -> ptr to definition which readings are requested
 *  Rets: struct headRetMsgMultiple_s -> head from STAILQ, stores multiple (all) readings
 *        from requested Definition, NULL=NONE
 * --------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s
GetAllReadings(Common_Definition_t *Common_Definition)
{
  // prepare STAILQ head for multiple RetMsg storage
  struct headRetMsgMultiple_s headRetMsgMultiple;

  // Initialize the queue head
  STAILQ_INIT(&headRetMsgMultiple);

//---------------------------------------------------------------------------------------------------

  // first the STATE reading
 
	if (Common_Definition->state) {
//	if(defined($val) &&
//     $val ne "unknown" &&
//     $val ne "Initialized" &&
//     $val ne "" &&
//     $val ne "???") {

		// alloc new retMsgMultiple queue element
		strTextMultiple_t *retMsgMultiple =
			malloc(sizeof(strTextMultiple_t));

		// write line to allocated memory and store to queue
		retMsgMultiple->strTextLen = asprintf(&retMsgMultiple->strText
			,"setstate %.*s %.*s\r\n"
			,Common_Definition->nameLen
			,Common_Definition->name
			,Common_Definition->stateLen
			,Common_Definition->state);

		// insert retMsg in stail-queue
		STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsgMultiple, entries);

	}

//---------------------------------------------------------------------------------------------------

	// second the detailed list of readings

  // loop the readings stored for this definition for processing
	xReadingSLTQE_t *currentReadingSLTQE;
	STAILQ_FOREACH(currentReadingSLTQE, &Common_Definition->headReadings, entries) {

		// set current tist, if missing
		if (!currentReadingSLTQE->readingTist) {

			//Log 4, "WriteStatefile $d $c: Missing TIME, using current time";

			time(&currentReadingSLTQE->readingTist);

		}

/*		// set current value, if missing
		if (!currentReadingSLTQE->readingTist) {

			//Log 4, "WriteStatefile $d $c: Missing VAL, setting it to 0";

			currentReadingSLTQE->readingTist = ;

		}*/

		// get reading tist
		struct tm timeinfo;
		localtime_r(&currentReadingSLTQE->readingTist, &timeinfo);

		// alloc new retMsgMultiple queue element
		strTextMultiple_t *retMsgMultiple =
			malloc(sizeof(strTextMultiple_t));

		// write line to allocated memory and store to queue
		retMsgMultiple->strTextLen = asprintf(&retMsgMultiple->strText
			,"setstate %.*s %d-%d-%d %d:%d:%d %.*s %.*s TXT\r\n"
			,Common_Definition->nameLen
			,Common_Definition->name
			,timeinfo.tm_year+1900
			,timeinfo.tm_mon+1
			,timeinfo.tm_mday
			,timeinfo.tm_hour
			,timeinfo.tm_min
			,timeinfo.tm_sec
			,currentReadingSLTQE->nameString.length
			,currentReadingSLTQE->nameString.characters
			,currentReadingSLTQE->valueString.length
			,currentReadingSLTQE->valueString.characters);

/*
		// display for debug
		LOGD("setstate %.*s %d.%d.%d %d:%d:%d %.*s %.*s TXT\r\n"
			,Common_Definition->nameLen
			,Common_Definition->name
			,timeinfo.tm_year+1900
			,timeinfo.tm_mon+1
			,timeinfo.tm_mday
			,timeinfo.tm_hour

			,timeinfo.tm_min
			,timeinfo.tm_sec
			,currentReadingSLTQE->readingNameTextLen
			,currentReadingSLTQE->readingNameText
			,currentReadingSLTQE->readingValueTextLen
			,currentReadingSLTQE->readingValueText);
*/

		// insert retMsg in stail-queue
		STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsgMultiple, entries);

	}

  // return STAILQ head, stores multiple generated lines of text, if STAILQ_EMPTY -> none
  return headRetMsgMultiple;
}



