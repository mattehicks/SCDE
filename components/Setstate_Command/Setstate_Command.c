/* #################################################################################################
 *
 *  Function: Setstate Command for SCDE (Smart Connected Device Engine)
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

#include "Setstate_Command.h"

#include <regex.h>



// set default build verbose - if no external override
#ifndef Setstate_Command_DBG  
#define Setstate_Command_DBG  5	// 5 is default
#endif 



// -------------------------------------------------------------------------------------------------

// make data root locally available
static SCDERoot_t* SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for operation
static SCDEFn_t* SCDEFn;

// -------------------------------------------------------------------------------------------------



/*
 * -------------------------------------------------------------------------------------------------
 *  DName: Setstate_Command
 *  Desc: Data 'Provided By Command' for the Setstate Command (functions + infos this command provides
 *        to SCDE)
 *  Data: 
 * -------------------------------------------------------------------------------------------------
 */

// Command Help
const uint8_t Setstate_helpText[] = 
  "Usage: Setstate <definition> <state>, to set the state shown in the command list";
// CommandHelp (detailed)
const uint8_t Setstate_helpDetailText[] = 
  "Usagebwrebwerb: Setstate <definition> <state>, to set the state shown in the command list";


providedByCommand_t Setstate_ProvidedByCommand =
{
   "Setstate"					// Command-Name of command -> libfilename.so !
  ,sizeof("Setstate")-1				// length of cmd
  ,Setstate_InitializeCommandFn			// Initialize Fn
  ,Setstate_CommandFn				// the Fn code
  ,&Setstate_helpText
  ,sizeof(Setstate_helpText)
  ,&Setstate_helpDetailText
  ,sizeof(Setstate_helpDetailText)
};



/* --------------------------------------------------------------------------------------------------
 *  FName: Setstate_Initialize
 *  Desc: Initializion of SCDE Function Callback of an new loaded command
 *  Info: Stores Command-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
Setstate_InitializeCommandFn(SCDERoot_t* SCDERootptr)
{

  // make data root locally available
  SCDERoot = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn = SCDERootptr->SCDEFn;

  #if Setstate_Command_DBG >= 5
  SCDEFn->Log3Fn(Setstate_ProvidedByCommand.commandNameText
		  ,Setstate_ProvidedByCommand.commandNameTextLen
		  ,5
		  ,"InitializeFn called. Command '%.*s' now useable."
		  ,Setstate_ProvidedByCommand.commandNameTextLen
		  ,Setstate_ProvidedByCommand.commandNameText);
  #endif

  return 0;
}



/* -------------------------------------------------------------------------------------------------
 *  FName: Setstate_CommandFn
 *  Desc: Tries to add an Setstate with optional value to an definition
 *        Calls modules SetstateibuteFn with cmd=add, if retMsg.strText != NULL -> module executes veto
 *  Info: 'defName' is an definition name, for that the Setstateibute should be assigned
 *        'SetstateName' is the Setstateibute name
 *        'SetstateVal' is the OPTIONAL Setstateibute value
 *  Para: const uint8_t *args  -> prt to Setstate command args text "definition reading value"
 *                                                         or "definition timestamp reading value"
 *        const size_t argsLen -> Setstate command args text length
 *  Rets: struct headRetMsgMultiple_s -> STAILQ head of multiple retMsg, if NULL -> no retMsg-entry
 * -------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s
Setstate_CommandFn (const uint8_t *args
		,const size_t argsLen)
{
  // first seek-counter
  int i = 0;
	
  // start seek with new * (seek starts at args *)
  const uint8_t *defName = args;

  // seek * to start of Definition Name '\32' (after space)
  while( (i < argsLen) && (*defName == ' ') ) {i++;defName++;}
	
  // continue seek with new * (seek starts at defName *)
  const uint8_t *tiStReadingValue = defName;

  // second seek-counter
  int j = 0;

  // seek * to end of Definition Name '\32' (after space)
  while( (i < argsLen) && (*tiStReadingValue != ' ') ) {i++;j++;tiStReadingValue++;}

  // length of Definition-Name determined
  size_t defNameLen = j;

  // seek * to start of Timestamp-Reading-Value '\32' (after space)
  while( (i < argsLen) && (*tiStReadingValue == ' ') ) {i++;tiStReadingValue++;}

  // length of Timestamp-Reading-Value determined (assuming: rest of the args)
  size_t tiStReadingValueLen = argsLen - i;

// -------------------------------------------------------------------------------------------------

  // prepare STAILQ head for multiple RetMsg storage
  struct headRetMsgMultiple_s headRetMsgMultiple;

  // Initialize the queue
  STAILQ_INIT(&headRetMsgMultiple);

// -------------------------------------------------------------------------------------------------

  // check usage by verifying lengths
  if ( ( defNameLen == 0 ) || ( tiStReadingValueLen == 0 ) ) {

	// alloc mem for retMsg
	strTextMultiple_t *retMsg =
		 malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Command Setstate could not interpret args '%.*s'!"
		 " Usage: Setstate <devspec> <state-sub-args>"
		,argsLen
		,args);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;
  }

// -------------------------------------------------------------------------------------------------

// create list of definitions which meet devspec here
// and loop them here 
//	{

// -------------------------------------------------------------------------------------------------

  	// search for the Common_Definition for the requested Definition-Name
  	Common_Definition_t *Common_Definition = STAILQ_FIRST(&SCDERoot->HeadCommon_Definitions);

 	 while ( Common_Definition != NULL ) {

		if ( ( Common_Definition->nameLen == defNameLen )
			&& ( !strncasecmp((const char*) Common_Definition->name, (const char*) defName, defNameLen) ) ) {

			// found, break and keep current
			break;
		}

		// get next Common_Definition to process it
		Common_Definition = STAILQ_NEXT(Common_Definition, entries);
 	 }

// -------------------------------------------------------------------------------------------------

	// Definition for the given Definition-Name NOT found
  	if ( Common_Definition == NULL ) {

		// alloc mem for retMsg
		strTextMultiple_t *retMsg =
			 malloc(sizeof(strTextMultiple_t));

		// response with error text
		retMsg->strTextLen = asprintf(&retMsg->strText
			,"Error, could not find <defName>: %.*s!\r\n"
			,defNameLen
			,defName);

		// insert retMsg in stail-queue
		STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

		// -> continue processing 'definitions that meet devspec' loop
  	}

// -------------------------------------------------------------------------------------------------

 	// Definition for the given Definition-Name found
  	else {

		// to store the time components
		struct tm timeComp;

		// to store extracted readings
		char reading[128];				// !!! 64??
		char value[128];				// !!! 64??
		char mime[16];					// !!! 64??

		// create temorary string
		char *tiStReadingValueString;

		asprintf(&tiStReadingValueString
				,"%.*s"
				,tiStReadingValueLen
				,tiStReadingValue);

		// infos needed for calling the StateFn
		time_t readingTiSt;
		uint8_t *readingName;
		size_t readingNameLen;
		uint8_t *readingValue;
		size_t readingValueLen;
		uint8_t *readingMime;
		size_t readingMimeLen;

// -------------------------------------------------------------------------------------------------

		// check if detailed reading with timestamp and MIME can be rebuilt - via regex
		// e.g. definitionname 2017-08-08 22:42:25 myreading active TXT
		if ( 9 == sscanf(tiStReadingValueString
			,"%d-%d-%d %d:%d:%d %s %s %s"
				,&timeComp.tm_year
				,&timeComp.tm_mon
				,&timeComp.tm_mday
				,&timeComp.tm_hour
				,&timeComp.tm_min
				,&timeComp.tm_sec
				,reading
				,value
				,mime ) ) {

				timeComp.tm_year -= 1900;
				timeComp.tm_mon -= 1;

				readingTiSt = mktime(&timeComp);

				if ( readingTiSt == -1 ) {
					printf("Error: unable to make time using mktime\n");

					// use current time
					time(&readingTiSt);
				}


			readingName = &reading;
			readingNameLen = strlen(reading);
			readingValue = &value;
			readingValueLen = strlen(value);
			readingMime = &mime;
			readingMimeLen = strlen(mime);

/*
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
*/

/* 2 optionen
push @ret, "setstate $d $val";
push @ret,"setstate $d $rd->{TIME} $c $val";
*/


//https://regexr.com/
//https://gist.github.com/ianmackinnon/3294587
//http://pubs.opengroup.org/onlinepubs/009695399/functions/regcomp.html

// typedef for StateFn - called to set an state for this definition e.g. called from setstate cmd for recovery from save
//typedef strTextMultiple_t* (* StateFn_t)(Common_Definition_t *Common_Definition, time_t readingTiSt, uint8_t *readingName, size_t readingNameLen, uint8_t *readingValue, size_t readingValueLen);

	/*		char aa[] = "maik";
			char ab[] = "otto";

			// Detailed state with timestamp ?
			readingTiSt = 99;
			readingName = (uint8_t *) &aa;
			readingNameLen = 4;
			readingValue = (uint8_t *) &ab;
			readingValueLen = 4;
			readingMime = (uint8_t *) &ab;
			readingMimeLen = 4;*/



/*
        char inputStr2[100]="12:34:04";
        char inputStr[100]="12:34";

	// for regex processing
	regex_t regex;
        int reti;
        char msgbuf[100];

	// compile regular expression
        reti = regcomp(&regex, "^(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}) +([^ ].*)$", 0);

        if( reti ){

	        fprintf(stderr, "Could not compile regex\n");
        }
*/






/*

	char *source = "___ abc123def ___ ghi456 ___";

 	char *regexString = "^(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}) +([^ ].*)$";
	size_t maxMatches = 8;
	size_t maxGroups = 2;
  
	regex_t regexCompiled;
	regmatch_t groupArray[maxGroups];
	unsigned int m;
	char *cursor;

	if (regcomp(&regexCompiled, regexString, REG_EXTENDED)) {

		printf("Could not compile regular expression.\n");

//		return 1;
	};



	// match cnt for loop
	m = 0;

	// seek *
	cursor = source;

	for ( m = 0 ; m < maxMatches ; m ++ ) {

		if (regexec(&regexCompiled, cursor, maxGroups, groupArray, 0))
		        break;  // No more matches

		// group cnt for loop
		unsigned int g = 0;

		unsigned int offset = 0;

		for ( g = 0 ; g < maxGroups ; g++ ) {

			if ( groupArray[g].rm_so == (size_t)-1 )
            			break;  // No more groups

			if ( g == 0 )
 				offset = groupArray[g].rm_eo;

			// build a copy
		//	char cursorCopy[strlen(cursor) + 1];
		//	strcpy(cursorCopy, cursor);
		//	cursorCopy[groupArray[g].rm_eo] = 0;

			printf("RegEx Match %u, Group %u: [%2u-%2u]: '%.*s'\n"
				,m
				,g
				,(unsigned int) groupArray[g].rm_so
				,(unsigned int) groupArray[g].rm_eo
		//		,cursorCopy + groupArray[g].rm_so);

				,(int) groupArray[g].rm_eo
				,cursor); 
		}

		cursor += offset;
	}

	regfree(&regexCompiled);

*/











/*
	// execute regular expression
	printf("%s is the string\n",inputStr);
	reti = regexec(&regex, inputStr, 0, NULL, 0);

	// MATCH - contains tiSt and nameValue
	if( !reti ) {

		puts("Match");

		my ($sname, $sval) = split(" ", $nameval, 2);
    		 $sval = "" if(!defined($sval));*/


/*
exe    		 my $ret = CallFn($sdev, "StateFn", $d, $tim, $sname, $sval);

	}

	// NO MATCH - contains NO tiSt and nameValue
        else if ( reti == REG_NOMATCH ) {

                puts("No match");
        }

	// error, failed ...
        else {
                regerror(reti, &regex, msgbuf, sizeof(msgbuf));
                fprintf(stderr, "Regex match failed: %s\n", msgbuf);
        }

	// free compiled regular expression
	regfree(&regex);

*/



			// detect wrong chars in reading
 /*   		 Log3 $d, 3, "WARNING: unsupported character in reading $sname ".
            	 "(not A-Za-z/\\d_\\.-), notify the $d->{TYPE} module maintainer."
       		 if(!goodReadingName($sname));
     		 if(!defined($d->{READINGS}{$sname}) ||
      		   !defined($d->{READINGS}{$sname}{TIME}) ||
      		   $d->{READINGS}{$sname}{TIME} lt $tim) {
     		   $d->{READINGS}{$sname}{VAL} = $sval;
      		  $d->{READINGS}{$sname}{TIME} = $tim; } 
*/


// end of detailed rebuilt
		}




// -------------------------------------------------------------------------------------------------

		// an detailed reading could NOT be rebuilt
		// assuming this is the general, required STATE reading
		else {

			// we are dealing with STATE reading here
			readingName = (uint8_t *) "STATE";
			readingNameLen = sizeof("STATE")-1;

			// we are dealing with STATE reading here, always MIME TXT
			readingMime = (uint8_t *) "TXT";
			readingMimeLen = sizeof("TXT")-1;

			// STATE reading with date ?
			// e.g. definitionname 2017-08-08 22:42:25 active
			if ( 7 == sscanf(tiStReadingValueString
				,"%d-%d-%d %d:%d:%d %s"
				,&timeComp.tm_year
				,&timeComp.tm_mon
				,&timeComp.tm_mday
				,&timeComp.tm_hour
				,&timeComp.tm_min
				,&timeComp.tm_sec
				,value ) ) {

				timeComp.tm_year -= 1900;
				timeComp.tm_mon -= 1;

				readingTiSt = mktime(&timeComp);

				if ( readingTiSt == -1 ) {
					printf("Error: unable to make time using mktime\n");

					// use current time
					time(&readingTiSt);
				}
			}

			// STATE reading without date ?
			// e.g. definitionname active
			else if ( 1 == sscanf(tiStReadingValueString
					,"%s"
					,value ) ) {

					// use current time
					time(&readingTiSt);
			}

			else {

			// no value will be detected in length zero

			}

			readingValue = &value;
			readingValueLen = strlen(value);

			// we have a value for 'STATE'
			if (readingValueLen) {

				// do not overwrite state like "opened" or "initialized"
				if ( ( SCDERoot->globalCtrlRegA | F_INIT_DONE ) || 
				     ( ( Common_Definition->stateLen == 3 ) && 
				       ( strncmp((char*)Common_Definition->state, "???", 3 ) ) ) ) {

					// free old value
					if (Common_Definition->state)
						free(Common_Definition->state);

					// store new value
					Common_Definition->stateLen = 
						asprintf((char**) &Common_Definition->state
							,"%.*s"
							,readingValueLen
							,readingValue);

					// store new tiST
					Common_Definition->stateTiSt = readingTiSt;
				}
			}
		}

// -------------------------------------------------------------------------------------------------

		// call State Fn to execute the update - if provided by Type
		if (Common_Definition->module->ProvidedByModule->StateFn) {

			#if Setstate_Command_DBG >= 7
			// prepare TiSt for LogFn
			strText_t strText =
  				SCDEFn->FmtDateTimeFn(readingTiSt);

			SCDEFn->Log3Fn(Setstate_ProvidedByCommand.commandNameText
		  		,Setstate_ProvidedByCommand.commandNameTextLen
				,5
				,"Calling StateFn of Module '%.*s' for Definition '%.*s'. "
				 "Reading '%.*s' gets new Value '%.*s', Mime '%.*s', TimeStamp '%.*s'."
				,Common_Definition->module->ProvidedByModule->typeNameLen
				,Common_Definition->module->ProvidedByModule->typeName
				,Common_Definition->nameLen
				,Common_Definition->name
				,readingNameLen
				,readingName
				,readingValueLen
				,readingValue
				,readingMimeLen
				,readingMime
				,strText.strTextLen
				,strText.strText);

			// free TiSt from LogFn
			free(strText.strText);
			#endif

			// call modules StateFn, if retMsg != NULL -> interpret as veto
			strTextMultiple_t *retMsg =
				Common_Definition->module->ProvidedByModule->StateFn(Common_Definition
					,readingTiSt
					,readingName
					,readingNameLen
					,readingValue
					,readingValueLen
					,readingMime
					,readingMimeLen);

			// got an error msg? Insert retMsg in stail-queue
			if (retMsg) STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

		}


		// free this temp string
		free(tiStReadingValueString);

// -------------------------------------------------------------------------------------------------

	}

// end of loop for list of definitions which meet devspec here
//}

  // return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
  return headRetMsgMultiple;
}


