/* #################################################################################################
 *
 *  Function: Setstate Command for SCDE (Smart Connected Device Engine)
 *
 *  ESP 8266EX & ESP32 SOC Activities ...
 *  Copyright by EcSUHA
 *
 *  Created by Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, G push @ret, "setstate $d $val"; push @ret, "setstate $d $val";ermany for EcSUHA.de 
 *
 *  MSchulze780@GMAIL.COM
 *  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
 * #################################################################################################
 */

/* 2 optionen
push @ret, "setstate $d $val";
push @ret,"setstate $d $rd->{TIME} $c $val";
*/

#include <ProjectConfig.h>
#include <esp8266.h>
#include <Platform.h>

#include <SCDE_s.h>

#include "Setstate_Command.h"



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
  ,8						// length of cmd
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

  SCDEFn->Log3Fn(Setstate_ProvidedByCommand.commandNameText
		  ,Setstate_ProvidedByCommand.commandNameTextLen
		  ,3
		  ,"InitializeFn called. Command '%.*s' now useable."
		  ,Setstate_ProvidedByCommand.commandNameTextLen
		  ,Setstate_ProvidedByCommand.commandNameText);

  return 0;
}



/* -------------------------------------------------------------------------------------------------
 *  FName: Setstate_CommandFn
 *  Desc: Tries to add an Setstateibute with optional value to an definition
 *        Calls modules SetstateibuteFn with cmd=add, if retMsg.strText != NULL -> module executes veto
 *  Info: 'defName' is an definition name, for that the Setstateibute should be assigned
 *        'SetstateName' is the Setstateibute name
 *        'SetstateVal' is the OPTIONAL Setstateibute value
 *  Para: const uint8_t *argsText  -> prt to Setstate command args text "defName SetstateName SetstateVal"
 *        const size_t argsTextLen -> Setstate command args text length
 *  Rets: struct headRetMsgMultiple_s -> STAILQ head of multiple retMsg, if NULL -> no retMsg-entry
 * -------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s
Setstate_CommandFn (const uint8_t *argsText
		,const size_t argsTextLen)
{

  // prepare STAILQ head for multiple RetMsg storage
  struct headRetMsgMultiple_s headRetMsgMultiple;

  // Initialize the queue
  STAILQ_INIT(&headRetMsgMultiple);

  // a seek-counter
  int i = 0;
	
  // set start * of possible def-Name-Text seek-start-pos
  const uint8_t *defNameText = argsText;
	
  // seek to start * of attr-Name-Text '\32' after space
  while( (i < argsTextLen) && (*defNameText == ' ') ) {i++;defNameText++;}
	
  // set start * of possible attr-Name-Text seek-start-pos
  const uint8_t *attrNameText = defNameText;

  // seek to next space '\32'
  while( (i < argsTextLen) && (*attrNameText != ' ') ) {i++;attrNameText++;}

  // length of def-Name-Text determined
  size_t defNameTextLen = i;

  // seek to start * of attr-Name-Text '\32' after space
  while( (i < argsTextLen) && (*attrNameText == ' ') ) {i++;attrNameText++;}

  // set start * of possible attr-Val seek-start-pos
  const uint8_t *attrValText = attrNameText;

  // a second seek-counter
  int j = 0;

  // seek to next space '\32'
  while( (i < argsTextLen) && (*attrValText != ' ') ) {i++,j++;attrValText++;}

  // length of attr-Name-Text determined
  size_t attrNameTextLen = j;

  // start * of attr-Val-Text '\32' after space
  while( (i < argsTextLen) && (*attrValText == ' ') ) {i++;attrValText++;}
		
	 // set start * of possible attr-Va-end seek-start-pos
  const uint8_t *attrValTextEnd = attrValText;
	
	// a third seek-counter
  int k = 0;

  // seek to next space '\32'
  while( (i < argsTextLen) && (*attrValTextEnd != ' ') ) {i++,k++;attrValTextEnd++;}

  // length of attr-Val-Text determined
  size_t attrValTextLen = k;

// -------------------------------------------------------------------------------------------------

  // veryfy lengths > 0, definition 0 allowed
  if ( (defNameTextLen == 0) || (attrNameTextLen == 0) ) {

	// alloc mem for retMsg
	strTextMultiple_t *retMsg =
		 malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Could not interpret command '%.*s'! Usage: Setstate <devspec> <attrName> [<attrVal>]"
		,argsTextLen
		,argsText);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;
  }

// -------------------------------------------------------------------------------------------------

  // search for the Common_Definition for the requested def-name-text
  Common_Definition_t *Common_Definition = STAILQ_FIRST(&SCDERoot->HeadCommon_Definitions);
  while (Common_Definition != NULL) {

	if ( (Common_Definition->nameLen == defNameTextLen)
		&& (!strncasecmp((const char*) Common_Definition->name, (const char*) defNameText, defNameTextLen)) ) {

		// found, break and keep prt
		break;
	}

	// get next Common_Definition for processing
	Common_Definition = STAILQ_NEXT(Common_Definition, entries);
  }

// -------------------------------------------------------------------------------------------------

  // Common_Definition for the requested definition-name not found
  if (Common_Definition == NULL) {

	// alloc mem for retMsg
	strTextMultiple_t *retMsg =
		 malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Error, could not find <defName>: %.*s!\r\n"
		,defNameTextLen
		,defNameText);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;
  }

  // Common_Definition for the requested def-name-text found
  else {


















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

	// call Attribute Fn to notify changes - if provided by Type
	if (Common_Definition->module->ProvidedByModule->AttributeFn) {

		// build attribute command text
		uint8_t *attrCmdText;
		size_t attrCmdTextLen = (size_t) asprintf((char **) &attrCmdText
			,"add");

		printf("Calling AttributeFN of typeName:%.*s for defName:%.*s -> attrCmd:%.*s attrName:%.*s attrVal:%.*s\n"
			,Common_Definition->module->ProvidedByModule->typeNameLen
			,Common_Definition->module->ProvidedByModule->typeName
			,Common_Definition->nameLen
			,Common_Definition->name
			,attrCmdTextLen
			,attrCmdText
  			,attrNameTextLen
			,attrNameText
			,newAttrValTextLen
			,newAttrValText);

		// call modules AttributeFn, if retMsg != NULL -> interpret as veto
		strTextMultiple_t *retMsg =  
			Common_Definition->module->ProvidedByModule->AttributeFn(Common_Definition
			,attrCmdText
			,attrCmdTextLen
			,attrNameText
			,attrNameTextLen
			,&newAttrValText
			,&newAttrValTextLen);

		// got an error msg?
		if (retMsg) {

			// insert retMsg in stail-queue
			STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);
		}

		free(attrCmdText);
	}

// -------------------------------------------------------------------------------------------------

	// veto from Types attributeFn? Do NOT add attribute
	if (!STAILQ_EMPTY(&headRetMsgMultiple)) {

		if (newAttrValText) free(newAttrValText);

		// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
		return headRetMsgMultiple;
	}

// -------------------------------------------------------------------------------------------------

	// loop through assigned attributes and try to find the existing attribute and replace it. Or add it as new attribute.
	attribute_t *attribute = STAILQ_FIRST(&SCDERoot->headAttributes);
	while (true) {

		// no old attribute found ?
		if (attribute == NULL) {

			// alloc mem for attribute structure (attribute_t)
			attribute
				= malloc(sizeof(attribute_t));

//			// zero the struct
//			memset(attribute, 0, sizeof(attribute_t));

			// the def
			attribute->attrAssignedDef = Common_Definition;

			// the name
			attribute->attrNameTextLen = attrNameTextLen;
			attribute->attrNameText = malloc(attrNameTextLen);
			memcpy(attribute->attrNameText, attrNameText, attrNameTextLen);

			// the value - already in allocated mem
			attribute->attrValTextLen = newAttrValTextLen;
			attribute->attrValText = newAttrValText;
	
			// add new attribute at tail
			STAILQ_INSERT_HEAD(&SCDERoot->headAttributes, attribute, entries);

			printf("Added new attribute - defName:%.*s, attrName:%.*s, attrVal:%.*sx\n"
				,attribute->attrAssignedDef->nameLen
				,attribute->attrAssignedDef->name
				,attribute->attrNameTextLen
				,attribute->attrNameText
				,attribute->attrValTextLen
				,attribute->attrValText);

			// added new, break
			break;
		}

		// is this an old value for this attribute?
		if ( (attribute->attrAssignedDef == Common_Definition)
			&& (attribute->attrNameTextLen == attrNameTextLen)
			&& (!strncmp((const char*) attribute->attrNameText, (const char*) attrNameText, attrNameTextLen)) ) {

			// free old value for this attribute
			if (attribute->attrValText) free(attribute->attrValText);

			// the new value
			attribute->attrValTextLen = newAttrValTextLen;
			attribute->attrValText = newAttrValText;

			printf("Updated old attribute - defName:%.*s, attrName:%.*s, attrVal:%.*s\n"
				,attribute->attrAssignedDef->nameLen
				,attribute->attrAssignedDef->name
				,attribute->attrNameTextLen
				,attribute->attrNameText
				,attribute->attrValTextLen
				,attribute->attrValText);

			// found, break
			break;
		}

		// get next attribute for processing
		attribute = STAILQ_NEXT(attribute, entries);
	}

	 // return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;

  }

}



