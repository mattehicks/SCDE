/* #################################################################################################
 *
 *  Function: Deleteattr Command - for SCDE (Smart Connected Device Engine)
 *            Deletes Attributes assigned to Definitions
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
static SCDERoot_t* p_SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for operation
static SCDEFn_t* p_SCDEFn;

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
  "Usage: Deleteattr <def-spec> <attr-name>, to delete attributes from definitions";
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
 *  FName: Deleteattr - Initialize Command Funktion
 *  Desc: Initializion of an (new loaded) SCDE-Command. Init p_SCDERoot and p_SCDE Function Callbacks.
 *  Info: Called only once befor use!
 *  Para: SCDERoot_t* p_SCDERoot_from_core -> ptr to SCDE Data Root from SCDE-Core
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
Deleteattr_InitializeCommandFn(SCDERoot_t* p_SCDERoot_from_core)
{
  // make data root locally available
  p_SCDERoot = p_SCDERoot_from_core;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  p_SCDEFn = p_SCDERoot->SCDEFn;

// --------------------------------------------------------------------------------------------------

  #if Deleteattr_Command_DBG >= 3
  p_SCDEFn->Log3Fn(Deleteattr_ProvidedByCommand.commandNameText
	,Deleteattr_ProvidedByCommand.commandNameTextLen
	,3
	,"InitializeFn called. Now useable.");
  #endif

// --------------------------------------------------------------------------------------------------

  return 0;
}



/* -------------------------------------------------------------------------------------------------
 *  FName: Deleteattr - The Command main Fn
 *  Desc: Tries to delete an attributes from definition-specification matching Definitions.
 *        Then calls modules AttributeFn with cmd=del, retMsg.strText != NULL -> module sends veto.
 *  Info: 'def_spec' is the definition specification which is the input for the definition names
 *         matching query. For all matching Definitions this attribute will be deleted.
 *  Para: const uint8_t* p_args  -> space seperated command args text string "def_spec"
 *        const size_t args_len -> command args text length
 * NPara: const String_t args -> space seperated command args string "def_spec attr_name"
 *  Rets: struct headRetMsgMultiple_s -> STAILQ head of multiple retMsg, if NULL -> no retMsg
 * -------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s ICACHE_FLASH_ATTR //struct  Head_String_s
Deleteattr_CommandFn (const uint8_t* p_args
		,const size_t args_len)
{

  // temporary conversion to make ready -> const String_t args
  String_t args;
  args.p_char = p_args;
  args.len = args_len;

// -------------------------------------------------------------------------------------------------

  #if Deleteattr_Command_DBG >= 7
  p_SCDEFn->Log3Fn(Deleteattr_ProvidedByCommand.commandNameText
	,Deleteattr_ProvidedByCommand.commandNameTextLen
	,7
	,"CommandFn called with args '%.*s'"
	,args_len
	,p_args);
  #endif

// --------------------------------------------------------------------------------------------------

  // prepare STAILQ head to store multiple 'ret_msg' elements
  struct Head_String_s head_ret_msg;

  // Initialize the queue
  STAILQ_INIT(&head_ret_msg);

// --------------------------------------------------------------------------------------------------

  // expected argument #1
  String_t def_spec;

  // set * to start of possible def-spec text (seek-start-pos)
  def_spec.p_char = args.p_char;

  // the total seek-counter
  int i = 0;
	
  // seek * to start of  def-spec text ('\32' after space)
  while( ( i < args.len ) && ( *def_spec.p_char == ' ' ) ) { i++ ; def_spec.p_char++ ; }

  // 1 @ finnished

  // expected argument #2
  String_t attr_name;

  // set * to start of possible attr-name text (seek-start-pos)
  attr_name.p_char = def_spec.p_char;

  // an element seek-counter
  int j = 0;

  // seek to next space '\32'
  while( ( i < args.len ) && ( *attr_name.p_char != ' ' ) ) { i++, j++ ; attr_name.p_char++ ; }

  // length of def-spec text determined
  def_spec.len = j;

  // seek * to start of attr-name text ('\32' after space)
  while( ( i < args.len ) && ( *attr_name.p_char == ' ' ) ) { i++ ; attr_name.p_char++ ; }

  // 2 @ finnished

  // no further arguments expected - searching the end of text
  String_t end_of_text;
	
  // set start * of possible 'end of text' seek-start-pos
  end_of_text.p_char = attr_name.p_char;
	
  // clear element seek-counter
  j = 0;

  // seek to next space '\32'
  while( ( i < args.len ) && ( *end_of_text.p_char != ' ' ) ) { i++ , j++ ; end_of_text.p_char++ ; }

  // length of attr-Val text determined
  attr_name.len = j;

  // @ 'p_end_of_text' ! No further processing ...

// -------------------------------------------------------------------------------------------------

  // verify lengths > 0, ATTR_VAL_LEN = 0 ALLOWED!
  if ( ( def_spec.len == 0 ) || ( attr_name.len == 0 ) ) {

	// alloc mem for retMsg
	Entry_String_t* p_entry_ret_msg =
		 malloc(sizeof(Entry_String_t));

	// response with error text
	p_entry_ret_msg->string.len = asprintf(&p_entry_ret_msg->string.p_char,
		"Error! Could not interpret '%.*s'! Usage: Deleteattr <def-spec> <attr-name>",
		args.len,
		args.p_char);

	// insert ret_msg as entry in stail-queue
	STAILQ_INSERT_TAIL(&head_ret_msg, p_entry_ret_msg, entries);

  	// return head of singly linked tail queue, which holds 'ret_msg' elements


// temporary conversion to make ready ->  head_ret_msg
struct headRetMsgMultiple_s x;
STAILQ_INIT(&x);
x.stqh_first =  head_ret_msg.stqh_first;
x.stqh_last =  head_ret_msg.stqh_last;
  	return x; // head_ret_msg;
  }

// -------------------------------------------------------------------------------------------------

  // get all 'definition' that match 'def_spec'. Result returned as SLTQ head.
  struct Head_Definitions_s head_dev_spec_matching_definitions =
	p_SCDEFn->Get_Definitions_That_Match_DefSpec_String_Fn(def_spec);

// -------------------------------------------------------------------------------------------------

  // start processing, load first returned entry 'definition'
  Entry_Definitions_t* p_entry_dev_spec_matching_definitions = 
	STAILQ_FIRST(&head_dev_spec_matching_definitions);

  String_t attr_value = {(uint8_t*) NULL, 0};

  // no entry for definitions that match requested 'def_spec' ? We should throw out an error!
  if ( p_entry_dev_spec_matching_definitions == NULL ) {

	// alloc mem for retMsg
	Entry_String_t* p_entry_ret_msg =
		 malloc(sizeof(Entry_String_t));

	// response with error text
	p_entry_ret_msg->string.len = asprintf(&p_entry_ret_msg->string.p_char,
		"Could not find an <def-spec> matching definition! Check '%.*s'!",
		def_spec.len,
		def_spec.p_char);

	// insert ret_msg as entry in stail-queue
	STAILQ_INSERT_TAIL(&head_ret_msg, p_entry_ret_msg, entries);

  // temporary conversion to make ready ->  head_ret_msg
struct headRetMsgMultiple_s x;
STAILQ_INIT(&x);
x.stqh_first =  head_ret_msg.stqh_first;
x.stqh_last =  head_ret_msg.stqh_last;
  	return x; // head_ret_msg;
  }

// -------------------------------------------------------------------------------------------------

  // loop through the found 'definition' entrys - till the queue has no more entrys
  while ( p_entry_dev_spec_matching_definitions != NULL ) {

// -------------------------------------------------------------------------------------------------

	// get the ptr to the current 'definition'
	Common_Definition_t* p_entry_definition = 
		p_entry_dev_spec_matching_definitions->p_entry_definition;

	// to store the ret_msg from AttributeFn
	Entry_String_t* p_entry_ret_msg = NULL;

	// call 'module' 'Attribute_Fn' for this 'definition' to notify - if provided by Type
	if (p_entry_definition->module->provided->Attribute_Fn) {

		// build 'attr_command'
		String_t attr_command;
		attr_command.len = asprintf((char **) &attr_command.p_char
			,"del");

		#if Deleteattr_Command_DBG >= 7
		p_SCDEFn->Log3Fn(Deleteattr_ProvidedByCommand.commandNameText,
			Deleteattr_ProvidedByCommand.commandNameTextLen,
			7,
			"Calling AttributeFN for definition '%.*s' (modul '%.*s') with args "
			"attr_command '%.*s' attr_name '%.*s' attr_value '%.*s'",
			p_entry_definition->nameLen,
			p_entry_definition->name,
			p_entry_definition->module->provided->typeNameLen,
			p_entry_definition->module->provided->typeName,
			attr_command.len,
			attr_command.p_char,
  			attr_name.len,
			attr_name.p_char);
		#endif

		// call 'module' 'Attribute_Fn', if ret_msg != NULL -> an entry, interpret as veto
		p_entry_ret_msg =
			p_entry_definition->module->provided->Attribute_Fn(p_entry_definition,
			attr_command,
			attr_name,
			attr_value);

		// free 'attr_command'
		free (attr_command.p_char);
	}

	else {

		#if Deleteattr_Command_DBG >= 7
		p_SCDEFn->Log3Fn(Deleteattr_ProvidedByCommand.commandNameText,
			Deleteattr_ProvidedByCommand.commandNameTextLen,
			7,
			"No AttributeFN available for definition '%.*s' (modul '%.*s') "
			" attr_name '%.*s' with attr_value '%.*s' will be assigned without check.",
			p_entry_definition->nameLen,
			p_entry_definition->name,
			p_entry_definition->module->provided->typeNameLen,
			p_entry_definition->module->provided->typeName,
  			attr_name.len,
			attr_name.p_char,
			attr_value.len,
			attr_value.p_char);
		#endif
	}

// -------------------------------------------------------------------------------------------------

	// 'ret_msg' as entry ? => veto from attributeFn! Add 'ret_msg' entry to queue, do NOT delete attribute !
	if (p_entry_ret_msg) {

		// insert ret_msg as entry in stail-queue
		STAILQ_INSERT_TAIL(&head_ret_msg, p_entry_ret_msg, entries);

		#if Deleteattr_Command_DBG >= 7
		p_SCDEFn->Log3Fn(Deleteattr_ProvidedByCommand.commandNameText,
			Deleteattr_ProvidedByCommand.commandNameTextLen,
			7,
			"Got an veto msg from AttributeFN for definition '%.*s' (modul '%.*s'). "
			"Attribute attr_name '%.*s' with attr_value '%.*s' NOT deleted!",
			p_entry_definition->nameLen,
			p_entry_definition->name,
			p_entry_definition->module->provided->typeNameLen,
			p_entry_definition->module->provided->typeName,
  			attr_name.len,
			attr_name.p_char,
			attr_value.len,
			attr_value.p_char);
		#endif

		// load next entry of 'def_spec' matching definitions
		p_entry_dev_spec_matching_definitions = 
			STAILQ_NEXT(p_entry_dev_spec_matching_definitions, entries);

		// to continue loop without adding attributes
		continue;
	}

// -------------------------------------------------------------------------------------------------

	// get first entry from our 'attr_name' list
  	Entry_Attr_Name_t* p_entry_attr_name = 
		LIST_FIRST(&p_SCDERoot->head_attr_name);

	// get first entry from our 'attr_value' list
  	Entry_Attr_Value_t* p_entry_attr_value = 
		LIST_FIRST(&p_entry_definition->head_attr_value);

  	// search the lists 'attr_name' entries for the requested 'attr_name'
 	while ( p_entry_attr_name != NULL ) {

		// compare no + length of characters. Matching list entry with requested 'attr_name' ?
		if ( ( p_entry_attr_name->attr_name.len == attr_name.len )
			&& ( !strncasecmp((const char*) p_entry_attr_name->attr_name.p_char,
				(const char*) attr_name.p_char,
				attr_name.len) ) ) {

			// found matching 'attr_name', keep, break loop! 
			break;
		}

		// load next 'attr_name' entry to process it
		p_entry_attr_name = LIST_NEXT(p_entry_attr_name, entries);
 	 }

// -------------------------------------------------------------------------------------------------

	// We have an entry for requested 'attr_name' ? Search assigned 'attr_value' !
  	if ( !p_entry_attr_name == NULL ) {

  		// search this 'definition' list of assigned 'attr_value' entries
 		while ( p_entry_attr_value != NULL ) {

			// check, if this entry 'attr_value' is for requested entry 'attr_name'
			if ( p_entry_attr_name == p_entry_attr_value->p_entry_attr_name ) {

				// found assigned 'attr_value', keep, break loop! 
				break;
			}

			// load next 'attr_value' entry to process it
			p_entry_attr_value = LIST_NEXT(p_entry_attr_value, entries);
		}
 	 }

// -------------------------------------------------------------------------------------------------

	// no entry for requested 'attr_name' or 'attr_value' ? -> NO delete!
  	if ( p_entry_attr_name == NULL || p_entry_attr_value == NULL ) {

		// alloc mem for retMsg
		Entry_String_t* p_entry_ret_msg =
			 malloc(sizeof(Entry_String_t));

		// response with error text
		p_entry_ret_msg->string.len = asprintf(&p_entry_ret_msg->string.p_char,
			"Deleting attr_value '%.*s' for definition '%.*s' (modul '%.*s') "
			"failed! It is NOT assigned!",
			attr_name.len,
			attr_name.p_char,
			p_entry_definition->nameLen,
			p_entry_definition->name,
			p_entry_definition->module->provided->typeNameLen,
			p_entry_definition->module->provided->typeName);

		// insert ret_msg as entry in stail-queue
		STAILQ_INSERT_TAIL(&head_ret_msg, p_entry_ret_msg, entries);


		// load next entry of 'def_spec' matching definitions
		p_entry_dev_spec_matching_definitions = 
			STAILQ_NEXT(p_entry_dev_spec_matching_definitions, entries);

		// to continue loop without adding attributes
		continue;
	}

// -------------------------------------------------------------------------------------------------

	else {
		// data is still there - log now ...

		#if Deleteattr_Command_DBG >= 7
		p_SCDEFn->Log3Fn(Deleteattr_ProvidedByCommand.commandNameText,
			Deleteattr_ProvidedByCommand.commandNameTextLen,
			7,
			"Will delete attr_name '%.*s' with attr_value '%.*s' "
			"from definition '%.*s' (modul '%.*s').",
			p_entry_attr_name->attr_name.len,
			p_entry_attr_name->attr_name.p_char,
			p_entry_attr_value->attr_value.len,
			p_entry_attr_value->attr_value.p_char,
			p_entry_definition->nameLen,
			p_entry_definition->name,
			p_entry_definition->module->provided->typeNameLen,
			p_entry_definition->module->provided->typeName);
		#endif

		// remove 'attr_value' entry from the 'attr_value' list
		LIST_REMOVE(p_entry_attr_value, entries);

		// free 'attr_value'.p_char
		if (p_entry_attr_value->attr_value.p_char) 
			free(p_entry_attr_value->attr_value.p_char);

		// free old entry 'p_entry_attr_value'
		if (p_entry_attr_value) 
			free(p_entry_attr_value);

// -------------------------------------------------------------------------------------------------

		// check if current 'attr_name' entry is no longer in use (can be removed)
	
		// get first entry from our 'definition' list
  		Common_Definition_t* p_entry_definition = 
			STAILQ_FIRST(&p_SCDERoot->HeadCommon_Definitions);

 		// search the lists 'definition' entries for the requested 'definition'
 		while ( p_entry_definition != NULL ) {

			// get first entry from our 'attr_value' list
  			Entry_Attr_Value_t* p_entry_attr_value = 
				LIST_FIRST(&p_entry_definition->head_attr_value);

  			// search the lists 'attr_value' entries for the requested 'attr_name'
 			while ( p_entry_attr_value != NULL ) {

				// 'attr_name' used in this 'definition' 'attr_value' list ? -> in use!
				if ( p_entry_attr_name == p_entry_attr_value->p_entry_attr_name ) {

					// 'attr_name'in use, keep, break loop! 
					break;
				}

				// load next 'attr_value' entry to process it
				p_entry_attr_value = LIST_NEXT(p_entry_attr_value, entries);
			}

			// load next 'definition' entry to process it
			p_entry_definition = STAILQ_NEXT(p_entry_definition, entries);
 		 }

		// no entry for requested 'attr_name' or 'attr_value' ? 
		// -> delete 'attr_name' entry, its not longer in use - fee it
  		if ( p_entry_attr_value == NULL || p_entry_definition == NULL ) {

			// remove 'attr_name' entry from the 'attr_name' list
			LIST_REMOVE(p_entry_attr_name, entries);

			// free 'attr_name'.p_char
			free(p_entry_attr_name->attr_name.p_char);

			// free old entry 'p_entry_attr_name'
			free(p_entry_attr_name);
		}
	}

// -------------------------------------------------------------------------------------------------

  	// load next entry of def_spec matching definitions
  	p_entry_dev_spec_matching_definitions = 
		STAILQ_NEXT(p_entry_dev_spec_matching_definitions, entries);
  }

// -------------------------------------------------------------------------------------------------


// temporary conversion to make ready ->  head_ret_msg
struct headRetMsgMultiple_s x;
STAILQ_INIT(&x);
x.stqh_first =  head_ret_msg.stqh_first;
x.stqh_last =  head_ret_msg.stqh_last;

  return x; // head_ret_msg;
}


