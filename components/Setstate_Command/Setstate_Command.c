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


providedByCommand_t Setstate_ProvidedByCommand = {
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
 *  Desc: Sets the state of an reading


Die X_State-Funktion wird durch fhem.pl aufgerufen, sobald über den Befehl setstate versucht wird ein Wert für ein Reading oder den Status ($hash->{STATE}) einer Gerätedefinition zu setzen. Dieser Befehl wird primär beim Starten von FHEM aufgerufen sobald das State-File eingelesen wird


An complete State with Date+Time+Name+Value+Mime '2017-08-08 22:42:25 myreading active TXT'

Wenn via setstate ein Reading gesetzt wird, kann die X_State-Funktion das Setzen dieses Readings durch die Rückgabe einer aussagekräftigen Fehlermeldung unterbinden. Sofern undef zurückgegeben wird, wird das entsprechende Reading auf den übergebenen Status gesetzt.

State without Name+Mime is assumed as the Definitions STATE '2017-08-08 22:42:25 active' (Date+Time+Value, has fixed Mime 'TXT')
State without Date Time Name Mime 'active' (Value, sets current Date+Time and has fixed Mime 'TXT')

Wenn via setstate der Definitionsstatus gesetzt wird, wird die X_State-Funktion erst nach dem Setzen des Status aufgerufen. Man kann dabei zwar eine Fehlermeldung zurückgeben, der Status wird aber dennoch übernommen. Die Fehlermeldung wird lediglich dem Nutzer angezeigt.




 *  Info: 'devspecString' is an Devicespecification String, for that the Setstate should be executed
 *        'SetstateName' is the Setstateibute name
 *        'SetstateVal' is the OPTIONAL Setstateibute value

 *  Para: const uint8_t *args  -> prt to Setstate command args text "definition reading value"
 *                                                         or "definition timestamp reading value"
 *        const size_t argsLen -> Setstate command args text length
 *  Rets: struct headRetMsgMultiple_s -> STAILQ head of multiple retMsg, if NULL -> no retMsg-entry









 *  Para: const xString_t argsString -> Setstate arguments string "devspec reading value"
 *                                   -> or "devspec timestamp reading value"
 *  Rets: struct xHeadMultipleStringSLTQ_s -> singly linked tail queue head to store multiple strings
 *                                            loop string entrys till STAILQ_EMPTY
 * -------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s  //new xHeadMultipleStringSLTQ_s
Setstate_CommandFn (const uint8_t *args
		,const size_t argsLen) //const xString_s argsString)
{

//-----------------------------------

// temporary creation to make ready -> const xString_s argsString
  xString_t argsString;
  argsString.characters = args;
  argsString.length = argsLen;

//-----------------------------------

  // first seek-counter
  int i = 0;

  // stores extracted Devicespecification-String
  xString_t devspecString;

  // start seeking at Arguments-String *
  devspecString.characters = 
	argsString.characters;

  // seek * to start of Devicespecification-String (after space  '\32')
  while( (i < argsString.length) && (*devspecString.characters == ' ') )
	{ i++ ; devspecString.characters++ ; }

  // stores extracted Time-Stamp-Reading-Value-String
  xString_t tiStReadingValueString;

  // start seeking at beginning of Devicespecification-String *
  tiStReadingValueString.characters = 
	devspecString.characters;

  // second seek-counter
  int j = 0;

  // seek * to the end of Devicespecification-String (before space '\32' !)
  while( (i < argsString.length) && (*tiStReadingValueString.characters != ' ') )
	{ i++ ; j++ ; tiStReadingValueString.characters++ ; }

  // length of Devicespecification-String determined
  devspecString.length = j;

  // seek * to start of Timestamp-Reading-Value-String (after space  '\32')
  while( (i < argsString.length) && (*tiStReadingValueString.characters == ' ') )
	{ i++ ; tiStReadingValueString.characters++ ; }

  // length of Timestamp-Reading-Value-String determined (assuming: rest of the args)
  tiStReadingValueString.length = argsString.length - i;

// -------------------------------------------------------------------------------------------------

  // prepare singly linked tail queue head for multiple Return-Message storage
  struct xHeadMultipleStringSLTQ_s retMsgHeadMultipleStringSLTQ;

  // Initialize the SLTQ
  STAILQ_INIT(&retMsgHeadMultipleStringSLTQ);

// -------------------------------------------------------------------------------------------------

  // check usage by verifying lengths
  if ( ( devspecString.length == 0 ) || ( tiStReadingValueString.length == 0 ) ) {

	// alloc mem for retMsg
	xMultipleStringSLTQE_t *retMsgMultipleStringSLTQE =
	malloc(sizeof(xMultipleStringSLTQE_t));

	// response with error text
	retMsgMultipleStringSLTQE->string.length = 
		asprintf(&retMsgMultipleStringSLTQE->string.characters
			,"Command Setstate could not interpret args '%.*s'!"
			 " Usage: Setstate <devspec> <state-sub-args>"
			,argsString.length
			,argsString.characters);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&retMsgHeadMultipleStringSLTQ, retMsgMultipleStringSLTQE, entries);

  	// return singly linked tail queue head, which holds multiple linked retMsg strings

//-------
struct headRetMsgMultiple_s x;
STAILQ_INIT(&x);
x.stqh_first = retMsgHeadMultipleStringSLTQ.stqh_first;
x.stqh_last = retMsgHeadMultipleStringSLTQ.stqh_last;

  	return x; // retMsgHeadMultipleStringSLTQ;   
//-------
  }

// -------------------------------------------------------------------------------------------------

  // get all Definitions which meet devspec in an SLTQ
  struct xHeadMultipleStringSLTQ_s definitionHeadMultipleStringSLTQ =
  	SCDEFn->Devspec2ArrayFn(devspecString);

  // Queue empty? No Definitions for the given devspec NOT found
  if (STAILQ_EMPTY(&definitionHeadMultipleStringSLTQ)) {

	// alloc mem for retMsg
	xMultipleStringSLTQE_t *retMsgMultipleStringSLTQE =
		malloc(sizeof(xMultipleStringSLTQE_t));

	// response with error retMsg
	retMsgMultipleStringSLTQE->string.length = 
		asprintf(&retMsgMultipleStringSLTQE->string.characters
			,"Please define <%.*s> first!"
			,devspecString.length
			,devspecString.characters);

	// insert retMsg in SLTQ
	STAILQ_INSERT_TAIL(&retMsgHeadMultipleStringSLTQ, retMsgMultipleStringSLTQE, entries);

//-------
struct headRetMsgMultiple_s x;
STAILQ_INIT(&x);
x.stqh_first = retMsgHeadMultipleStringSLTQ.stqh_first;
x.stqh_last = retMsgHeadMultipleStringSLTQ.stqh_last;

  	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
  	return x; // retMsgHeadMultipleString;   
//-------
  }

// -------------------------------------------------------------------------------------------------

 // for processing the definition queue elements
  xMultipleStringSLTQE_t *definitionMultipleStringSLTQE;

  // now loop through the found Definitions - till the queue is empty
  while ( !STAILQ_EMPTY ( &definitionHeadMultipleStringSLTQ ) ) {

// -------------------------------------------------------------------------------------------------

	// get first definition queue element
	definitionMultipleStringSLTQE = 
		STAILQ_FIRST(&definitionHeadMultipleStringSLTQ);

  	// search for the Common_Definition for the requested Definition-Name
  	Common_Definition_t *Common_Definition = 
		STAILQ_FIRST(&SCDERoot->HeadCommon_Definitions);

 	 while ( Common_Definition != NULL ) {

		if ( ( Common_Definition->nameLen == definitionMultipleStringSLTQE->string.length )
			&& ( !strncasecmp((const char*) Common_Definition->name
				,(const char*) definitionMultipleStringSLTQE->string.characters
				, definitionMultipleStringSLTQE->string.length) ) ) {

			// found matching Definition, break and keep current
			break;
		}

		// get next Common_Definition to process it
		Common_Definition = STAILQ_NEXT(Common_Definition, entries);
 	 }

// -------------------------------------------------------------------------------------------------

	// Definition for the given Definition-Name NOT found
  	if ( Common_Definition == NULL ) {

		// alloc mem for retMsg
		xMultipleStringSLTQE_t *retMsgMultipleStringSLTQE =
			malloc(sizeof(xMultipleStringSLTQE_t));

		// response with error retMsg
		retMsgMultipleStringSLTQE->string.length = 
			asprintf(&retMsgMultipleStringSLTQE->string.characters
				,"Please define <defName>: %.*s first!"
				,definitionMultipleStringSLTQE->string.length
				,definitionMultipleStringSLTQE->string.characters);

		// insert retMsg in stail-queue
		STAILQ_INSERT_TAIL(&retMsgHeadMultipleStringSLTQ, retMsgMultipleStringSLTQE, entries);

		// -> continue processing the 'definitions that meet devspec' loop
  	}

// -------------------------------------------------------------------------------------------------

 	// Definition for the given Definition-Name found
  	else {

		// to store the time, date components
		struct tm timeComp;

		// to store extracted values by current method sscanf (reading, value and mime)
		char reading[128];				// !!! 64??
		char value[128];				// !!! 64??
		char mime[16];					// !!! 64??

// -------------------------------------------------------------------------------------------------

		// create temorary string because we need temp zero terminated string for sscanf
		char *tiStReadingValueStringZT;
		asprintf(&tiStReadingValueStringZT
			,"%.*s"
			,tiStReadingValueString.length
			,tiStReadingValueString.characters);

		// final data we needed for calling the StateFn
		time_t readingTiSt;
		xString_t stateNameString;
		xString_t stateValueString;
		xString_t stateMimeString;

// -------------------------------------------------------------------------------------------------

		// check if a detailed Reading with Timestamp and MIME can be rebuilt - via regex
		// e.g. '2017-08-08 22:42:25 myreading active TXT'
		if ( 9 == sscanf(tiStReadingValueStringZT
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

			// correct the time
			timeComp.tm_year -= 1900;
			timeComp.tm_mon -= 1;

			// use extracted time
			readingTiSt = mktime(&timeComp);

			// time ok?
			if ( readingTiSt == -1 ) {
				printf("Error: unable to make time using mktime\n");

				// use current time
				time(&readingTiSt);
			}

	

			// build the data of an detailed reading
			stateNameString.length = asprintf(&stateNameString.characters
				,"%.*s"
				,strlen(reading)
				,(char*) &reading);

			stateValueString.length = asprintf(&stateValueString.characters
				,"%.*s"
				,strlen(value)
				,(char*) &value);

			stateMimeString.length = asprintf(&stateMimeString.characters
				,"%.*s"
				,strlen(mime)
				,(char*) &mime);

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

// -------------------------------------------------------------------------------------------------

			// NULL = start with no veto for reading preperation
			xMultipleStringSLTQE_t *retMsgMultipleStringSLTQE = NULL;

			// if provided by Type -> call State Fn to set the state
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
					,stateNameString.length
					,stateNameString.characters
					,stateValueString.length
					,stateValueString.characters
					,stateMimeString.length
					,stateMimeString.characters
					,strText.strTextLen
					,strText.strText);

				// free TiSt from LogFn
				free(strText.strText);
				#endif

				// call Modules StateFn. Interpret retMsgMultipleStringSLTQE != NULL as veto !
				retMsgMultipleStringSLTQE =
					Common_Definition->module->ProvidedByModule->StateFn(Common_Definition
						,readingTiSt
						,stateNameString
						,stateValueString
						,stateMimeString);
				}

			// return message? in singly linked tail queue element? VETO + Insert it in retMsg-queue.
			if (retMsgMultipleStringSLTQE) {

				STAILQ_INSERT_TAIL(&retMsgHeadMultipleStringSLTQ
					,retMsgMultipleStringSLTQE, entries);

				// free memory
				if (stateNameString.characters) 
					free(stateNameString.characters);

				if (stateValueString.characters) 
					free(stateValueString.characters);

				if (stateMimeString.characters) 
					free(stateMimeString.characters);
			}

			// else no return message - NO VETO. Create / Update Reading
			else {

				// Loop through old readings and
				// try to find an old reading and replace it.
				// Or add the new reading.
				xReadingsSLTQE_t *oldReadingsSLTQE = 
					STAILQ_FIRST(&Common_Definition->headReadings);

				while (true) {

					// loop at end - no old Reading found. Create new one !
					if (oldReadingsSLTQE == NULL) {

						// alloc mem for new Reading 
						xReadingsSLTQE_t *newReadingsSLTQE
							= malloc(sizeof(xReadingsSLTQE_t));

						// zero the struct
						memset(newReadingsSLTQE, 0, sizeof(xReadingsSLTQE_t));

						// fill Reading
						newReadingsSLTQE->readingTist = readingTiSt;
						newReadingsSLTQE->nameString = stateNameString;
						newReadingsSLTQE->valueString = stateValueString;

						// debug output
						#if Setstate_Command_DBG >= 7
						// prepare TiSt for LogFn
						strText_t strText =
  							SCDEFn->FmtDateTimeFn(readingTiSt);

						SCDEFn->Log3Fn(Setstate_ProvidedByCommand.commandNameText
		  					,Setstate_ProvidedByCommand.commandNameTextLen
							,7
							,"Created Reading '%.*s' with Value '%.*s',"
							 " Mime '%.*s', TimeStamp '%.*s'."
							,newReadingsSLTQE->nameString.length
							,newReadingsSLTQE->nameString.characters
							,newReadingsSLTQE->valueString.length
							,newReadingsSLTQE->valueString.characters
							,stateMimeString.length
							,stateMimeString.characters
							//,newReadingsSLTQE->mimeString.length
							//,newReadingsSLTQE->mimeString.characters
							,strText.strTextLen
							,strText.strText);

						// free TiSt from LogFn
						free(strText.strText);
						#endif


if (stateMimeString.characters) 
	free(stateMimeString.characters);

						// add new Reading at tail
						STAILQ_INSERT_TAIL(&Common_Definition->headReadings
							,newReadingsSLTQE, entries);

						// added new Reading, break
						break;
					}

					// looped - check if this existing Reading matches. Overwrite the old value.
					if ( (oldReadingsSLTQE->nameString.length == stateNameString.length)
						&& (!strncmp((const char*) oldReadingsSLTQE->nameString.characters,
						(const char*) stateNameString.characters,
						stateNameString.length)) ) {

						// replace / free old name & value for this reading
						if (oldReadingsSLTQE->nameString.characters) 
							free(oldReadingsSLTQE->nameString.characters);

						if (oldReadingsSLTQE->valueString.characters) 
							free(oldReadingsSLTQE->valueString.characters);

						// fill Reading
						oldReadingsSLTQE->readingTist = readingTiSt;
						oldReadingsSLTQE->nameString = stateNameString;
						oldReadingsSLTQE->valueString = stateValueString;

						#if Setstate_Command_DBG >= 7
						// prepare TiSt for LogFn
						strText_t strText =
  							SCDEFn->FmtDateTimeFn(readingTiSt);

						// debug output
						SCDEFn->Log3Fn(Setstate_ProvidedByCommand.commandNameText
		  					,Setstate_ProvidedByCommand.commandNameTextLen
							,7
							,"Created Reading '%.*s' with Value '%.*s',"
							 " Mime '%.*s', TimeStamp '%.*s'."
							,oldReadingsSLTQE->nameString.length
							,oldReadingsSLTQE->nameString.characters
							,oldReadingsSLTQE->valueString.length
							,oldReadingsSLTQE->valueString.characters
							,stateMimeString.length
							,stateMimeString.characters
							//,newReadingsSLTQE->mimeString.length
							//,newReadingsSLTQE->mimeString.characters
							,strText.strTextLen
							,strText.strText);

						// free TiSt from LogFn
						free(strText.strText);
						#endif

if (stateMimeString.characters) 
	free(stateMimeString.characters);

			/*LOGD("Updated old reading - readingName:%.*s, readingValue:%.*s\n"
				,oldReading->readingNameTextLen
				,oldReading->readingNameText
				,oldReading->readingValueTextLen
				,oldReading->readingValueText);*/

						// found, break
						break;
					}

					// get next reading for processing
					oldReadingsSLTQE = STAILQ_NEXT(oldReadingsSLTQE, entries);
				}
			}
		}

// -------------------------------------------------------------------------------------------------

		// seems that an detailed reading could NOT be rebuilt
		// else assuming this is the general, required STATE reading with fixed Mime 'TXT'
		else {

			// we are dealing with STATE reading here
			stateNameString.characters = (uint8_t *) "STATE";
			stateNameString.length = sizeof("STATE")-1;

			// we are dealing with STATE reading here, always MIME TXT
			stateMimeString.characters = (uint8_t *) "TXT";
			stateMimeString.length = sizeof("TXT")-1;

			// check if a STATE Reading with Timestamp can be rebuilt ?
			// e.g. '2017-08-08 22:42:25 active'
			if ( 7 == sscanf(tiStReadingValueStringZT
				,"%d-%d-%d %d:%d:%d %s"
				,&timeComp.tm_year
				,&timeComp.tm_mon
				,&timeComp.tm_mday
				,&timeComp.tm_hour
				,&timeComp.tm_min
				,&timeComp.tm_sec
				,value ) ) {

				// correct the time
				timeComp.tm_year -= 1900;
				timeComp.tm_mon -= 1;

				// use extracted time
				readingTiSt = mktime(&timeComp);

				// time ok?
				if ( readingTiSt == -1 ) {
					printf("Error: unable to make time using mktime\n");

					// use current time
					time(&readingTiSt);
				}

				// the extracted data of the value
				stateValueString.characters = &value;
				stateValueString.length = strlen(value);
			}

			// seems that an STATE Reading with Timestamp could NOT be rebuilt
			// else check if a STATE Reading without Timestamp can be rebuilt ?
			// e.g. 'active'
			else if ( 1 == sscanf(tiStReadingValueStringZT
				,"%s"
				,value ) ) {

				// use current time
				time(&readingTiSt);

				// the extracted data of the value
				stateValueString.characters = &value;
				stateValueString.length = strlen(value);
			}

			else {

				// could NOT rebuilt -> results in value zero length
				stateValueString.length = 0;
			}

// -------------------------------------------------------------------------------------------------

			// if provided by Type -> call State Fn to set the state
			if (Common_Definition->module->ProvidedByModule->StateFn) {

				#if Setstate_Command_DBG >= 7
				// prepare TiSt for LogFn
				strText_t strText =
  					SCDEFn->FmtDateTimeFn(readingTiSt);

				SCDEFn->Log3Fn(Setstate_ProvidedByCommand.commandNameText
		  			,Setstate_ProvidedByCommand.commandNameTextLen
					,7
					,"Calling StateFn of Module '%.*s' for Definition '%.*s'. "
					 "Reading '%.*s' gets new Value '%.*s', Mime '%.*s', TimeStamp '%.*s'."
					,Common_Definition->module->ProvidedByModule->typeNameLen
					,Common_Definition->module->ProvidedByModule->typeName
					,Common_Definition->nameLen
					,Common_Definition->name
					,stateNameString.length
					,stateNameString.characters
					,stateValueString.length
					,stateValueString.characters
					,stateMimeString.length
					,stateMimeString.characters
					,strText.strTextLen
					,strText.strText);

				// free TiSt from LogFn
				free(strText.strText);
				#endif

				// call Modules StateFn. Interpret retMsgMultipleStringSLTQE != NULL as veto !
				xMultipleStringSLTQE_t *retMsgMultipleStringSLTQE =
					Common_Definition->module->ProvidedByModule->StateFn(Common_Definition
						,readingTiSt
						,stateNameString
						,stateValueString
						,stateMimeString);

				// got an singly linked tail queue element holding an return message? Insert it in queue.
				if (retMsgMultipleStringSLTQE) 
					STAILQ_INSERT_TAIL(&retMsgHeadMultipleStringSLTQ
						,retMsgMultipleStringSLTQE, entries);

				// no veto possible here - STATE will get state
			}

			// we have a value for 'STATE' ?
			if (stateValueString.length) {

				// do not overwrite some states like "opened" or "initialized"
				if ( ( SCDERoot->globalCtrlRegA | F_INIT_DONE ) || 
				     ( ( Common_Definition->stateLen == 3 ) && 
				       ( strncmp((char*)Common_Definition->state, "???", 3 ) ) ) ) {

					// free old value, if any
					if (Common_Definition->state)
						free(Common_Definition->state);

					// store new value
					Common_Definition->stateLen = 
						asprintf((char**) &Common_Definition->state
							,"%.*s"
							,stateValueString.length
							,stateValueString.characters);

					// overwrite with new tiST
					Common_Definition->stateTiSt = readingTiSt;
				}
			}
		}

// -------------------------------------------------------------------------------------------------

		// free the temp string tiSt-Reading-Value-String
		free(tiStReadingValueStringZT);
	}

// -------------------------------------------------------------------------------------------------

  	// remove head in definition(s) STAIL Queue
        STAILQ_REMOVE_HEAD(&definitionHeadMultipleStringSLTQ, entries);

	// free charancters in xString_t
	free(definitionMultipleStringSLTQE->string.characters);

	// free STAIL Queue element xMultipleStringSLTQE_t
	free(definitionMultipleStringSLTQE);
  }

// -------------------------------------------------------------------------------------------------

//-------
struct headRetMsgMultiple_s x;
STAILQ_INIT(&x);
x.stqh_first = retMsgHeadMultipleStringSLTQ.stqh_first;
x.stqh_last = retMsgHeadMultipleStringSLTQ.stqh_last;

  	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
  	return x; // retMsgHeadMultipleStringSLTQ;   
}





