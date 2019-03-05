/* #################################################################################################
 *
 *  Function: Deleteattr Command for SCDE (Smart Connected Device Engine)
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

#include "Deleteattr_Command.h"



// set default build verbose - if no external override
#ifndef Deleteattr_Command_DBG  
#define Deleteattr_Command_DBG  5	// 5 is default
#endif



// -------------------------------------------------------------------------------------------------

// make data root locally available
static SCDERoot_t* SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for operation
static SCDEFn_t* SCDEFn;

// -------------------------------------------------------------------------------------------------



/*
 * -------------------------------------------------------------------------------------------------
 *  DName: Deleteattr_Command
 *  Desc: Data 'Provided By Command' for the Deleteattr Command (functions + infos this command provides
 *        to SCDE)
 *  Data: 
 * -------------------------------------------------------------------------------------------------
 */

// Command Help
const uint8_t Deleteattr_helpText[] = 
  "Usage: Deleteattr <devspec> <attrName>, to delete attribute for <devspec>";
// CommandHelp (detailed)
const uint8_t Deleteattr_helpDetailText[] = 
  "Usagebwrebwerb: DeleteAttr <name> <type> <options>, to Define a device";


ProvidedByCommand_t Deleteattr_ProvidedByCommand = {
  "Deleteattr",					// Command-Name of command -> libfilename.so !
  10,						// length of cmd
  Deleteattr_InitializeCommandFn,		// Initialize Fn
  Deleteattr_CommandFn,				// the Fn code
  { &Deleteattr_helpText, sizeof(Deleteattr_helpText) },
  { &Deleteattr_helpDetailText, sizeof(Deleteattr_helpDetailText) }
};



/* --------------------------------------------------------------------------------------------------
 *  FName: Deleteattr_Initialize
 *  Desc: Initializion of SCDE Function Callback of an new loaded command
 *  Info: Stores Command-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
Deleteattr_InitializeCommandFn(SCDERoot_t* SCDERootptr)
  {

  // make data root locally available
  SCDERoot = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn = SCDERootptr->SCDEFn;

// --------------------------------------------------------------------------------------------------

  #if Deleteattr_Command_DBG >= 3
  SCDEFn->Log3Fn(Deleteattr_ProvidedByCommand.commandNameText
	,Deleteattr_ProvidedByCommand.commandNameTextLen
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
 *  FName: Deleteattr_CommandFn
 *  Desc: Tries to delete an attribute from an definition
 *        Calls modules AttributeFn with cmd=del, retMsg.strText != NULL -> module executes veto
 *  Info: 'defName' is definition name, for that the attribute should be deleted
 *        'attrName' is the attribute name
 *  Para: const uint8_t *argsText  -> prt to attr command args text "defName attrName"
 *        const size_t argsTextLen -> attr command args text length
 *  Rets: struct headRetMsgMultiple_s -> STAILQ head of multiple retMsg, if NULL -> no retMsg-entry
 * -------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s
Deleteattr_CommandFn (const uint8_t *argsText
		,const size_t argsTextLen)
{
  #if Deleteattr_Command_DBG >= 7
  SCDEFn->Log3Fn(Deleteattr_ProvidedByCommand.commandNameText
	,Deleteattr_ProvidedByCommand.commandNameTextLen
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
  const uint8_t *defNameText = argsText;

  // set start of possible attr-Name-Text
  const uint8_t *attrNameText = argsText;

  // a seek-counter
  int i = 0;

  // seek to next space '\32'
  while( (i < argsTextLen) && (*attrNameText != ' ') ) {i++;attrNameText++;}

  // length of def-Name-Text
  size_t defNameTextLen = i;

  // seek to start position of attr-Name-Text '\32'
  while( (i < argsTextLen) && (*attrNameText == ' ') ) {i++;attrNameText++;}

  // length of attr-Name-Text
  size_t attrNameTextLen = argsTextLen - i;

// -------------------------------------------------------------------------------------------------

  // veryfy lengths > 0, definition 0 allowed
  if ( (defNameTextLen == 0) || (attrNameTextLen == 0) ) {

	// alloc mem for retMsg
	strTextMultiple_t *retMsg =
		 malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Could not interpret command '%.*s'! Usage: Deleteattr <devspec> <attrName>"
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

  // Common_Definition for the requested def-name-text not found
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

  // Common_Definition for the requested definition-name found
  else {

// -------------------------------------------------------------------------------------------------

	// call Attribute Fn to notify changes - if provided by Type
	if (Common_Definition->module->provided->AttributeFn) {

		// prepare a dummy for attributeFn (required?)
		size_t dummyAttrValTextLen = 0;
		uint8_t *dummyAttrValText = NULL;

		// build attribute command text
		uint8_t *attrCmdText;
		size_t attrCmdTextLen = (size_t) asprintf((char **) &attrCmdText
			,"del");

		printf("Calling AttributeFN of typeName:%.*s for defName:%.*s -> attrCmd:%.*s attrName:%.*s\n"
			,Common_Definition->module->provided->typeNameLen
			,Common_Definition->module->provided->typeName
			,Common_Definition->nameLen
			,Common_Definition->name
			,attrCmdTextLen
			,attrCmdText
  			,attrNameTextLen
			,attrNameText);

		// call modules AttributeFn, if retMsg != NULL -> interpret as veto
		strTextMultiple_t *retMsg = 
			Common_Definition->module->provided->AttributeFn(Common_Definition
			,attrCmdText
			,attrCmdTextLen
			,attrNameText
			,attrNameTextLen
			,&dummyAttrValText
			,&dummyAttrValTextLen);

		// got an error msg?
		if (retMsg) {

			// insert retMsg in stail-queue
			STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);
		}

		free(attrCmdText);
	}

// -------------------------------------------------------------------------------------------------

	// veto from Types attributeFn? Do NOT del attribute
	if (!STAILQ_EMPTY(&headRetMsgMultiple)) {

		// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
		return headRetMsgMultiple;
	}

// -------------------------------------------------------------------------------------------------

	// loop through assigned attributes and try to find the existing attribute and delete it. Or add error-msg if not found.
	attribute_t *attribute = STAILQ_FIRST(&SCDERoot->headAttributes);
	while (true) {

		// no old attribute found ?
		if (attribute == NULL) {

			// alloc mem for retMsg
			strTextMultiple_t *retMsg =
				malloc(sizeof(strTextMultiple_t));

			// response with error text
			retMsg->strTextLen = asprintf(&retMsg->strText
				,"Error - attrName:%.*s not found!\r\n"
				,attrNameTextLen
				,attrNameText);

			// insert retMsg in stail-queue
			STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

			// added error-text, break
			break;
		}

		// is this an old value for this attribute?
		if ( (attribute->attrAssignedDef == Common_Definition)
			&& (attribute->attrNameTextLen == attrNameTextLen)
			&& (!strncmp((const char*) attribute->attrNameText, (const char*) attrNameText, attrNameTextLen)) ) {

			printf("Deleted attribute - defName:%.*s, attrName:%.*s, attrVal:%.*s\n"
				,attribute->attrAssignedDef->nameLen
				,attribute->attrAssignedDef->name
				,attribute->attrNameTextLen
				,attribute->attrNameText
				,attribute->attrValTextLen
				,attribute->attrValText);

			// remove attribute
			STAILQ_REMOVE(&SCDERoot->headAttributes, attribute, attribute_s, entries);

			// free attribute name
			if (attribute->attrNameText) free(attribute->attrNameText);
	
			// free attribute value
			if (attribute->attrValText) free(attribute->attrValText);

			// free attribute
			if (attribute) free(attribute);

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



