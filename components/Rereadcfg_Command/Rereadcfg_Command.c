/* #################################################################################################
 *
 *  Function: Rereadcfg Command for SCDE (Smart Connected Device Engine)
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

#include "Rereadcfg_Command.h"



// set default build verbose - if no external override
#ifndef Rereadcfg_Command_DBG  
#define Rereadcfg_Command_DBG  5	// 5 is default
#endif



// -------------------------------------------------------------------------------------------------

// make data root locally available
static SCDERoot_t* SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for operation
static SCDEFn_t* SCDEFn;

// -------------------------------------------------------------------------------------------------



/*
 * -------------------------------------------------------------------------------------------------
 *  DName: Rereadcfg_Command
 *  Desc: Data 'Provided By Command' for the Rereadcfg Command (functions + infos this command provides
 *        to SCDE)
 *  Data: 
 * -------------------------------------------------------------------------------------------------
 */

// Command Help
const uint8_t Rereadcfg_helpText[] =
  "Usage: rereadcfg <configfile>, deletes everything, then reads in the <configfile>";
// CommandHelp (detailed)
const uint8_t Rereadcfg_helpDetailText[] =
  "Usagebwrebwerb: rereadcfg <filename>, to read the commands from <filenname>";


providedByCommand_t Rereadcfg_ProvidedByCommand =
  {
   "Rereadcfg"			// command-name text -> libfilename.so !
  ,9				// command-name text length
  ,Rereadcfg_InitializeCommandFn	// Initialize Fn
  ,Rereadcfg_CommandFn		// the Fn code
  ,&Rereadcfg_helpText
  ,sizeof(Rereadcfg_helpText)
  ,&Rereadcfg_helpDetailText
  ,sizeof(Rereadcfg_helpDetailText)
  };



/* --------------------------------------------------------------------------------------------------
 *  FName: Rereadcfg_Initialize
 *  Desc: Initializion of SCDE Function Callback of an new loaded command
 *  Info: Stores Command-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
Rereadcfg_InitializeCommandFn(SCDERoot_t* SCDERootptr)
  {

  // make data root locally available
  SCDERoot = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn = SCDERootptr->SCDEFn;

  SCDEFn->Log3Fn(Rereadcfg_ProvidedByCommand.commandNameText
		  ,Rereadcfg_ProvidedByCommand.commandNameTextLen
		  ,3
		  ,"InitializeFn called. Command '%.*s' now useable.\n"
		  ,Rereadcfg_ProvidedByCommand.commandNameTextLen
		  ,Rereadcfg_ProvidedByCommand.commandNameText);

  return 0;

  }



/* -------------------------------------------------------------------------------------------------
 *  FName: Rereadcfg_CommandFn
 *  Desc: Opens the fileName, loads and splits the content it in multiple lines by '\r\n',
 *        recreates multi-line text by '//' and executes each line.
 *  Info: 'fileName' is the file-name that should be loaded
 *  Para: const uint8_t *argsText  -> prt to Rereadcfg command args text "fileName"
 *        const size_t argsTextLen -> attr command args text length
 *  Rets: struct headRetMsgMultiple_s -> STAILQ head of multiple retMsg, if NULL -> NO entry
 * -------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s ICACHE_FLASH_ATTR
Rereadcfg_CommandFn (const uint8_t *argsText
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
		,"Could not interpret command '%.*s'! Usage: rereadcfg <configfile>"
		,argsTextLen
		,argsText);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;
  }

// -------------------------------------------------------------------------------------------------

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














  char string[] = 
/*
	"attr esp32ctrl otto maik\r\n"
	"attr esp32c otto maik//\r\n"
	"addthis\r\n"
	"attr esp32cl otto maik\r\n"
	"hallo maik\r\n";
*/
// makerfile
//	"define MyTelnet Telnet 23\r\n"
	"define MyWebIF WebIf 80\r\n"
	"define ESP32Ctrl esp32_control\r\n"
	"attr ESP32Ctrl room Testraum\r\n"

	"define SSR.1.at.GPIO.13 esp32_SwITCH GPIO=13&BLOCK=High_Speed&CHANNEL=0&TIMER=0&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55\r\n"
//	"define SSR.2.at.GPIO.12 esp32_SwITCH GPIO=12&BLOCK=Low_Speed&CHANNEL=0&TIMER=0&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55\r\n"
//	"define SSR.3.at.GPIO.14 esp32_SwITCH GPIO=14&BLOCK=High_Speed&CHANNEL=1&TIMER=1&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55\r\n"
//	"define SSR.4.at.GPIO.27 esp32_SwITCH GPIO=27&BLOCK=High_Speed&CHANNEL=2&TIMER=1&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55\r\n"
//	"define SSR.5.at.GPIO.26 esp32_SwITCH GPIO=26&BLOCK=High_Speed&CHANNEL=3&TIMER=2&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55\r\n"
//	"define SSR.6.at.GPIO.25 esp32_SwITCH GPIO=25&BLOCK=High_Speed&CHANNEL=4&TIMER=2&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55\r\n"

	"define i2c ESP32_I2C_Master GPIO=25&BLOCK=High_Speed&CHANNEL=4&TIMER=2&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55\r\n"

	;// the end


  char delimiter[] = "\r\n";
  char *ptr;


  // backup old configuration file, for reconstruction
  strText_t oldCfgFile;
  oldCfgFile = SCDERoot->currCfgFile;

  // set temporary to filename from Rereadcfg arg
  SCDERoot->currCfgFile.strText = fileNameText;
  SCDERoot->currCfgFile.strTextLen = fileNameTextLen;

  // clear, reset the global quit-flag
  SCDERoot->globalCtrlRegA &= ~(F_RECEIVED_QUIT);

  // to build our command row and start with empty
  strTextMultiple_t rebuiltCmdRow;
  rebuiltCmdRow.strText = NULL;

  // initialize strtok, start with first line
  ptr = strtok(string, delimiter);

  // loop through the rows
  while (ptr != NULL) {

	// temp
	strTextMultiple_t partialCmdRow;
	partialCmdRow.strText = (char *) ptr;
	partialCmdRow.strTextLen = (size_t) strlen(ptr);

//looping rows input, now in partialCmdRow

	// process if not an empty line ?
	if (partialCmdRow.strTextLen) {

		// if starting with new command row part ...
		if (!rebuiltCmdRow.strText) {

			// init memory allocation for new command row
			rebuiltCmdRow.strText = (char *) malloc(partialCmdRow.strTextLen);

			// copy command to allocated memory
			memcpy(rebuiltCmdRow.strText
				,partialCmdRow.strText
				,partialCmdRow.strTextLen);

			// init length
			rebuiltCmdRow.strTextLen = partialCmdRow.strTextLen;
		}

		// continuing a command row with an part ...
		else {

			// Reallocate memory to new size
			rebuiltCmdRow.strText = (char *) realloc(rebuiltCmdRow.strText
				,rebuiltCmdRow.strTextLen + partialCmdRow.strTextLen);

			// add command-part to allocated memory
			memcpy(rebuiltCmdRow.strText + rebuiltCmdRow.strTextLen
				,partialCmdRow.strText
				,partialCmdRow.strTextLen);

			// save new length
			rebuiltCmdRow.strTextLen += partialCmdRow.strTextLen;
		
		}

		// not ending with //? -> execute
		if ( (rebuiltCmdRow.strTextLen < 2) ||
			(memcmp(rebuiltCmdRow.strText + rebuiltCmdRow.strTextLen - 2, "//", 2)) ) {

			printf("rebuiltCmdRow: %.*s\n"
				,rebuiltCmdRow.strTextLen
				,rebuiltCmdRow.strText);

			// call the AnalyzeCommandChainFn, if retMsg != NULL -> got ret Msgs entries
			struct headRetMsgMultiple_s headRetMsgMultipleFromFn =
				SCDEFn->AnalyzeCommandChainFn((const uint8_t *) rebuiltCmdRow.strText, rebuiltCmdRow.strTextLen);

			// retMsgMultiple stailq filled from Fn ? -> get the entries till empty
			while (!STAILQ_EMPTY(&headRetMsgMultipleFromFn)) {

				// for the retMsg elements
				strTextMultiple_t *retMsg =
					STAILQ_FIRST(&headRetMsgMultipleFromFn);

				printf("AddingRetMsg: %.*s\n"
					,retMsg->strTextLen
					,retMsg->strText);

				// first remove this entry
				STAILQ_REMOVE(&headRetMsgMultipleFromFn, retMsg, strTextMultiple_s, entries);

				// then insert retMsg in stail-queue
				STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

			}

			// release memory for next cycle
			free(rebuiltCmdRow.strText);
			rebuiltCmdRow.strText = NULL;

			// break, if the global quit-flag is set
			if (SCDERoot->globalCtrlRegA & F_RECEIVED_QUIT) break;
	
  		}


		// ending with //? -> more to come, sub 2 chars (//)
		else {

			rebuiltCmdRow.strTextLen -= 2;

		}

	}

	// set pointer to beginning of next line
 	ptr = strtok(NULL, delimiter);

  }

  // there may be an incomplete processed multiline row in memory -> release it
  if (rebuiltCmdRow.strText) free(rebuiltCmdRow.strText);

  // rebuilt current cfg file
  SCDERoot->currCfgFile = oldCfgFile;


  // close file handle
//  fclose(fp);

  // return STAILQ head, queue stores multiple retMsg entries, if NULL -> no retMsg-entry
  return headRetMsgMultiple;

}









































/*

	printf("Abschnitt gefunden: %s\n", ptr);

	// call the AnalyzeCommandChainFn, if retMsg != NULL -> head of the ret Msgs
	struct headRetMsgMultiple_s headRetMsgMultipleFromFn =
		SCDEFn->AnalyzeCommandChainFn((uint8_t *) ptr, (size_t) strlen(ptr));

	// retMsgMultiple stailq filled from Fn ? -> get the entries till empty
	while (!STAILQ_EMPTY(&headRetMsgMultipleFromFn)) {

		// for the retMsg elements
		strTextMultiple_t *retMsg =
			STAILQ_FIRST(&headRetMsgMultipleFromFn);

		printf("AddingRetMsg: %.*s\n"
			,retMsg->strTextLen
			,retMsg->strText);

		// first remove this entry
		STAILQ_REMOVE(&headRetMsgMultipleFromFn, retMsg, strTextMultiple_s, entries);

		// then insert retMsg in stail-queue
		STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

	}

	// break, if the global quit-flag is set
	if (SCDERoot->globalCtrlRegA & F_RECEIVED_QUIT) break;

	// loop through the next lines
 	ptr = strtok(NULL, delimiter);

  }

  // return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
  return headRetMsgMultiple;

}






*/






 /*
 // for the retMsg elements
  strTextMultiple_t *retMsg;

  STAILQ_FOREACH(retMsg, &headRetMsgMultiple, entries) {

		printf("added: %.*s\n"
			,retMsg->strTextLen
			,retMsg->strText);
  }

*/








/*

  // max row length
  char buffer[256];
  char *pbuff;
  int value;

  // parse file
  while (1) {

	// end of file reached?
	if (!fgets(buffer, sizeof buffer, stdin)) break;

	printf("Line contains");

	pbuff = buffer;

	// search for end of line
	while (1) {

	// found ?
	if (*pbuff == '\n') break;

	value = strtol(pbuff, &pbuff, 10);

	printf(" %d", value);

	}
  }


fprintf(fp, "Hello!");
*/


/*
  uint8_t testcmd[] = "attr esp32ctrl hallo maik";
  size_t testcmdlen = 25;

  struct headRetMsgMultiple_s headRetMsgMultiplex;

   // get the module ptr by name
   headRetMsgMultiplex = SCDEFn->AnalyzeCommandChainFn(testcmd
		   , testcmdlen);

  if (!STAILQ_EMPTY(&headRetMsgMultiplex)) {

	// get the multiple retMsg
	strTextMultiple_t *retMsgx;

	// Read2 (remove).
	while (!STAILQ_EMPTY(&headRetMsgMultiplex)) {

		retMsgx = STAILQ_FIRST(&headRetMsgMultiplex);

		printf("Type-Name:%.*s, module* NOT loaded!\n"
			,retMsgx->strTextLen
			,retMsgx->strText);

		STAILQ_REMOVE(&headRetMsgMultiplex, retMsgx, strTextMultiple_s, entries);

		free(retMsgx->strText);

		free(retMsgx);
	}
  }



//----

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

//----


  // return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
  return headRetMsgMultiple;
*/
 










//https://www.freebsd.org/cgi/man.cgi?query=queue&sektion=3



//https://github.com/stockrt/queue.h/blob/master/sample.c



















/*


  // prepare STAILQ head for multiple RetMsg storage
  struct headRetMsgMultiple_s headRetMsgMultiple;

  // Initialize the queue
  STAILQ_INIT(&headRetMsgMultiple);










   FILE *fp;
   char str[60];

   // opening file for reading 
   fp = fopen("file.txt" , "r");

   if(fp == NULL) {
      perror("Error opening file");
      return(-1);

   }

   if( fgets (str, 60, fp) != NULL ) {
      // writing content to stdout 
      puts(str);
   }

   fclose(fp);






char line[256];
char com[3];

char separators[] = " ";
fgets(line, sizeof(line), fp);

//add or copy
strncpy(&com, p, sizeof(com));    // copy the command string in com







  char input[1024];  /buffer
  char *string[256]; /onerow           // 1) 3 is dangerously small,256 can hold a while;-) 

                                // You may want to dynamically allocate the pointers
                                // in a general, robust case. 
  char delimit[]=" \t\r\n\v\f"; // 2) POSIX whitespace characters
  int i = 0, j = 0;

  if (fgets(input, sizeof input, stdin)) // 3) fgets() returns NULL on error.
                                        // 4) Better practice to use sizeof 
                                        //    input rather hard-coding size 
  {                                        
    string[i]=strtok(input,delimit);    // 5) Make use of i to be explicit 
    while(string[i]!=NULL)                    
    {
      printf("string [%d]=%s\n",i,string[i]);
      i++;
      string[i]=strtok(NULL,delimit);
    }

    for (j=0;j<i;j++)
    printf("%s", string[i]);
  }

  return 0;
  }







  char string[] = "Kurt,Kanns;555678;DE";
  char delimiter[] = "\r\n";
  char *ptr;

  // clear, reset global rcvdquit-flag
  clear rcvdquit-flag here

  // initialize strtok, start with first line
  ptr = strtok(string, delimiter);

  while(ptr != NULL) {

	printf("Abschnitt gefunden: %s\n", ptr);




	// call the AnalyzeCommandChainFn, if retMsg != NULL -> head of the ret Msgs
	struct headRetMsgMultiple_s headRetMsgMultipleFromFn
		AnalyzeCommandChain(ptr, strlen(ptr));

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

	// break if global rcvdquit-flag is set
	if (rcvdquit-flag) break;

	// loop through the next lines
 	ptr = strtok(NULL, delimiter);

  }

  // return STAILQ head, it stores multiple RetMsg, if NULL -> no RetMsg-entries
  return headRetMsgMultiple;















seek through file, split lines by '\r\n'
identify multiline commands and add them 
call AnalyzeCommandChain with the identified line
push answer(if any) to multiple msg
loop through lines, break if rcvdquit-flag is set

1209	#####################################
1210	sub
1211	CommandInclude($$)
1212	{
1213	  my ($cl, $arg) = @_;			// arg = filename 
1214	  my $fh;
1215	  my @ret;
1216	  my $oldcfgfile;
1217	
1218	  if(!open($fh, $arg)) {		// arg = filename ��en + fehlermeldung + abrechen
1219	    return "Can't open $arg: $!";
1220	  }


1221	  Log 1, "Including $arg";



1222	  my @t = localtime();
1223	  my $gcfg = ResolveDateWildcards(AttrVal("global", "configfile", ""), @t);
1224	  my $stf  = ResolveDateWildcards(AttrVal("global", "statefile",  ""), @t);


1225	  if(!$init_done && $arg ne $stf && $arg ne $gcfg) {
1226	    my $nr =  $devcount++;
1227	    $comments{$nr}{TEXT} = "include $arg";
1228	    $comments{$nr}{CFGFN} = $currcfgfile		if($currcfgfile ne $gcfg);
1229	  }



1230	  $oldcfgfile  = $currcfgfile;
1231	  $currcfgfile = $arg;
1232	

1233	  my $bigcmd = "";
1234	  $rcvdquit = 0;
1235	  while(my $l = <$fh>) {

		// search for linereturn
1236	  	$l =~ s/[\r\n]//g;
1237
		// add multiline commands	
1238	        if ($l =~ m/^(.*)\\ *$/) {       # Multiline commands
1239	      		$bigcmd .= "$1\n";
1240	        }

		else {
1241	      		my $tret = AnalyzeCommandChain($cl, $bigcmd . $l);
1242	     		push @ret, $tret if(defined($tret));
1243	      		$bigcmd = "";
1244	        }

1245	    last if($rcvdquit);
1246	
1247	  }


1248	  $currcfgfile = $oldcfgfile;
1249	  close($fh);
1250	  return join("\n", @ret) if(@ret);
1251	  return undef;
1252	}


*/



















