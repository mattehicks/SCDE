/* #################################################################################################
 *
 *  Function: Attr Command for SCDE (Smart Connected Device Engine)
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

#include "Attr_Command.h"



// set default build verbose - if no external override
#ifndef Attr_Command_DBG  
#define Attr_Command_DBG  5	// 5 is default
#endif



// -------------------------------------------------------------------------------------------------

// make data root locally available
static SCDERoot_t* SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for operation
static SCDEFn_t* SCDEFn;

// -------------------------------------------------------------------------------------------------



/*
 * -------------------------------------------------------------------------------------------------
 *  DName: Attr_Command
 *  Desc: Data 'Provided By Command' for the Attr Command (functions + infos this command provides
 *        to SCDE)
 *  Data: 
 * -------------------------------------------------------------------------------------------------
 */

// Command Help
const uint8_t Attr_helpText[] = 
  "Usage: Attr <devspec> <attrName> [<attrVal>], to add attribute for <devspec>";
// CommandHelp (detailed)
const uint8_t Attr_helpDetailText[] = 
  "Usagebwrebwerb: Attr <name> <type> <options>, to Define a device";


ProvidedByCommand_t Attr_ProvidedByCommand = {
  "Attr",					// Command-Name of command -> libfilename.so !
  4,						// length of cmd
  Attr_InitializeCommandFn,			// Initialize Fn
  Attr_CommandFn,				// the Fn code
  { &Attr_helpText, sizeof(Attr_helpText) },
  { &Attr_helpDetailText, sizeof(Attr_helpDetailText) }
};



/* --------------------------------------------------------------------------------------------------
 *  FName: Attr_Initialize
 *  Desc: Initializion of SCDE Function Callback of an new loaded command
 *  Info: Stores Command-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
Attr_InitializeCommandFn(SCDERoot_t* SCDERootptr)
{
  // make data root locally available
  SCDERoot = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn = SCDERootptr->SCDEFn;

// --------------------------------------------------------------------------------------------------

  #if Attr_Command_DBG >= 3
  SCDEFn->Log3Fn(Attr_ProvidedByCommand.commandNameText
	,Attr_ProvidedByCommand.commandNameTextLen
	,3
	,"InitializeFn called. Now useable.");
  #endif

// --------------------------------------------------------------------------------------------------

  return 0;
}

/*
Device specification (devspec)

The commands attr, deleteattr, displayattr, delete, get, list, set, setreading, setstate, trigger can take a more complex device specification as argument, which will be expanded to a list of devices. A device specification (short devspec) can be:
a single device name. This is the most common case.
a list of devices, separated by comma (,)
a regular expression
a NAME=VALUE pair, where NAME can be an internal value like TYPE, a Reading-Name or an attribute. VALUE is a regexp. To negate the comparison, use NAME!=VALUE. To restrict the search, use i: as prefix for internal values, r: for readings and a: for attributes. See the example below.
if the spec is followed by the expression :FILTER=NAME=VALUE, then the values found in the first round are filtered by the second expression.
Examples:
set lamp1 on
set lamp1,lamp2,lamp3 on
set lamp.* on
set room=kitchen off
set room=kitchen:FILTER=STATE=on off
set room=kitchen:FILTER=STATE!=off off
*/



/* -------------------------------------------------------------------------------------------------
 *  FName: Attr_CommandFn
 *  Desc: Tries to add an attribute with optional value to an definition
 *        Calls modules AttributeFn with cmd=add, if retMsg.strText != NULL -> module executes veto
 *  Info: 'defName' is an definition name, for that the attribute should be assigned
 *        'attrName' is the attribute name
 *        'attrVal' is the OPTIONAL attribute value
 *  Para: const uint8_t *argsText  -> prt to attr command args text "defName attrName attrVal"
 *        const size_t argsTextLen -> attr command args text length
 *  Rets: struct headRetMsgMultiple_s -> STAILQ head of multiple retMsg, if NULL -> no retMsg-entry
 * -------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s ICACHE_FLASH_ATTR
Attr_CommandFn (const uint8_t *argsText
		,const size_t argsTextLen)
{
  #if Attr_Command_DBG >= 7
  SCDEFn->Log3Fn(Attr_ProvidedByCommand.commandNameText
	,Attr_ProvidedByCommand.commandNameTextLen
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
		,"Could not interpret command '%.*s'! Usage: Attr <devspec> <attrName> [<attrVal>]"
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
	if (Common_Definition->module->provided->AttributeFn) {

		// build attribute command text
		uint8_t *attrCmdText;
		size_t attrCmdTextLen = (size_t) asprintf((char **) &attrCmdText
			,"add");

		printf("Calling AttributeFN of typeName:%.*s for defName:%.*s -> attrCmd:%.*s attrName:%.*s attrVal:%.*s\n"
			,Common_Definition->module->provided->typeNameLen
			,Common_Definition->module->provided->typeName
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
			Common_Definition->module->provided->AttributeFn(Common_Definition
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



