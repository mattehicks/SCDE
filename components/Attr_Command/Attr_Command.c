/* #################################################################################################
 *
 *  Function: Attr Command - for SCDE (Smart Connected Device Engine)
 *            Assigns Attributes to Definitions
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
static SCDERoot_t* p_SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for operation
static SCDEFn_t* p_SCDEFn;

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
  "Usage: Attr <def-spec> <attr-name> [<attr-val>], to add attributes to definitions";
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
 *  FName: Attr - Initialize Command Funktion
 *  Desc: Initializion of an (new loaded) SCDE-Command. Init p_SCDERoot and p_SCDE Function Callbacks.
 *  Info: Called only once befor use!
 *  Para: SCDERoot_t* p_SCDERoot_from_core -> ptr to SCDE Data Root from SCDE-Core
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR
Attr_InitializeCommandFn(SCDERoot_t* p_SCDERoot_from_core)
{
  // make data root locally available
  p_SCDERoot = p_SCDERoot_from_core;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  p_SCDEFn = p_SCDERoot->SCDEFn;

// --------------------------------------------------------------------------------------------------

  #if Attr_Command_DBG >= 3
  p_SCDEFn->Log3Fn(Attr_ProvidedByCommand.commandNameText
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
 *  FName: Attr - The Command main Fn
 *  Desc: Tries to add Attributes to definition-specification matching Definitions, with optional Value.
 *        Then calls modules AttributeFn with cmd=add, if retMsg.strText != NULL -> module sends veto.
 *  Info: 'def_spec' is the definition specification which is the input for the definition names
 *                    matching query. All matching Definitions will get this attribute assigned.
 *        'attr_name' is the attribute name
 *        'attr_value' is the OPTIONAL attribute value (NULL IF NO VALUE ASSIGNED!)
 *  Para: const uint8_t* p_args  -> space seperated command args text string "def_spec attr_name attr_val"
 *        const size_t args_len -> command args text length
 * NPara: const String_t args -> space seperated command args string "def_spec attr_name attr_val"
 *  Rets: struct headRetMsgMultiple_s -> STAILQ head of multiple retMsg, if NULL -> no retMsg
 * -------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s ICACHE_FLASH_ATTR //struct  Head_String_s
Attr_CommandFn (const uint8_t* p_args
		,const size_t args_len)
{

  // temporary conversion to make ready -> const String_t args
  String_t args;
  args.p_char = p_args;
  args.len = args_len;

// -------------------------------------------------------------------------------------------------

  #if Attr_Command_DBG >= 7
  p_SCDEFn->Log3Fn(Attr_ProvidedByCommand.commandNameText
	,Attr_ProvidedByCommand.commandNameTextLen
	,7
	,"CommandFn called with args '%.*s'"
	,args.len
	,args.p_char);
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

  // expected argument #3
  String_t attr_value;

  // set * to start of possible attr-val text (seek-start-pos)
  attr_value.p_char = attr_name.p_char;

  // clear element seek-counter
  j = 0;

  // seek to next space '\32'
  while( ( i < args.len ) && ( *attr_value.p_char != ' ' ) ) { i++ , j++ ; attr_value.p_char++ ; }

  // length of attr-Name-Text determined
  attr_name.len = j;

  // seek * to start of 'end of text' ('\32' after space)
  while( ( i < args.len ) && ( *attr_value.p_char == ' ' ) ) { i++ ; attr_value.p_char++ ; }

  // 3 @ finnished

  // no further arguments expected - searching the end of text
  String_t end_of_text;
	
  // set start * of possible 'end of text' seek-start-pos
  end_of_text.p_char = attr_value.p_char;
	
  // clear element seek-counter
  j = 0;

  // seek to next space '\32'
  while( ( i < args.len ) && ( *end_of_text.p_char != ' ' ) ) { i++ , j++ ; end_of_text.p_char++ ; }

  // length of attr-Val text determined
  attr_value.len = j;

  // @ 'p_end_of_text' ! No further processing ...

// -------------------------------------------------------------------------------------------------

  // verify lengths > 0, ATTR_VAL_LEN = 0 ALLOWED!
  if ( ( def_spec.len == 0 ) || ( attr_name.len == 0 ) ) {

	// alloc mem for retMsg
	Entry_String_t* p_entry_ret_msg =
		 malloc(sizeof(Entry_String_t));

	// response with error text
	p_entry_ret_msg->string.len = asprintf(&p_entry_ret_msg->string.p_char,
		"Error! Could not interpret '%.*s'! Usage: Attr <def-spec> <attr-name> [<attr-val>]",
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

  // get all Definitions that match def-spec. Result as SLTQ Head.
  struct Head_Definitions_s head_dev_spec_matching_definitions =
	p_SCDEFn->Get_Definitions_That_Match_DefSpec_String_Fn(def_spec);
printf("maik1");
// -------------------------------------------------------------------------------------------------

  // start processing the returned SLTQ elements, filled with entrys (definitions)
  Entry_Definitions_t* p_entry_dev_spec_matching_definitions = 
	STAILQ_FIRST(&head_dev_spec_matching_definitions);

  // loop through the found Definitions - till the queue has no more entrys
  while ( p_entry_dev_spec_matching_definitions != NULL ) {

// -------------------------------------------------------------------------------------------------
printf("maik2");
	// get the ptr to the current definition
	Common_Definition_t* p_definition = 
		p_entry_dev_spec_matching_definitions->p_entry_definition;

	// to store the ret_msg from AttributeFn
	Entry_String_t* p_entry_ret_msg = NULL;

	// call Modules Attribute Fn for this definition to notify about upcoming changes - if provided by Type
	if (p_definition->module->provided->AttributeFn) {
printf("maik3");
		// build 'attr_command'
		String_t attr_command;
		attr_command.len = asprintf((char **) &attr_command.p_char
			,"add");

		printf("Calling AttributeFN of typeName:%.*s for defName:%.*s -> attrCmd:%.*s attrName:%.*s attrVal:%.*s\n"
			,p_definition->module->provided->typeNameLen
			,p_definition->module->provided->typeName
			,p_definition->nameLen
			,p_definition->name
			,attr_command.len
			,attr_command.p_char
  			,attr_name.len
			,attr_name.p_char
			,attr_value.len
			,attr_value.p_char);

		// call modules AttributeFn, if retMsg != NULL -> interpret as veto
		p_entry_ret_msg =  (Entry_String_t*) //tEMPORARY casting!!!!!!!!!!!!!!!!!!!!1
			p_definition->module->provided->AttributeFn(p_definition,
			attr_command.p_char,
			attr_command.len,
			attr_name.p_char,
			attr_name.len,
			attr_value.p_char,
			attr_value.len);

		// free 'attr_command'
		free(attr_command.p_char);
	}

// -------------------------------------------------------------------------------------------------

	// ret_msg = veto from attributeFn? Store as entry in queue + do NOT add attribute !
	if (p_entry_ret_msg) {
printf("maik4");
		// insert ret_msg as entry in stail-queue
		STAILQ_INSERT_TAIL(&head_ret_msg, p_entry_ret_msg, entries);

 		printf("veto attr AttributeFN of typeName:%.*s for defName:%.*s -> attrName:%.*s attrVal:%.*s\n"
			,p_definition->module->provided->typeNameLen
			,p_definition->module->provided->typeName
			,p_definition->nameLen
			,p_definition->name
  			,attr_name.len
			,attr_name.p_char
			,attr_value.len
			,attr_value.p_char);

		// get next entry of def_spec matching definitions
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
		LIST_FIRST(&p_definition->head_attr_value);
printf("maik5");
  	// search the list filled with 'attr_name' entries if the requested 'attr_name' already exists
 	while ( p_entry_attr_name != NULL ) {
printf("maik6");
		// compare 'attr_name' character length & characters. Match ?
		if ( ( p_entry_attr_name->attr_name.len == attr_name.len )
			&& ( !strncasecmp((const char*) p_entry_attr_name->attr_name.p_char,
				(const char*) attr_name.p_char,
				attr_name.len) ) ) {

			// found matching 'attr_name', break loop and keep current
			break;
		}

		// get next 'attr_name' entry to process it
		p_entry_attr_name = LIST_NEXT(p_entry_attr_name, entries);
 	 }
printf("maik7");
// -------------------------------------------------------------------------------------------------

	// 'attr_name' currently not assigned ?
  	if ( p_entry_attr_name == NULL ) {
printf("maik8");
		// alloc mem for 'attr_name' storage (Entry_Attr_Name_s)
		p_entry_attr_name
			= malloc(sizeof(Entry_Attr_Name_t));

		// store the 'attr_name'
		p_entry_attr_name->attr_name.len = attr_name.len;
		p_entry_attr_name->attr_name.p_char = malloc(attr_name.len);
		memcpy(p_entry_attr_name->attr_name.p_char, attr_name.p_char, attr_name.len);
	
		// add new 'attr_name' entry to the attr_name list
		LIST_INSERT_HEAD(&p_SCDERoot->head_attr_name, p_entry_attr_name, entries);
printf("maik9");
		// debug msg created
	}

// -------------------------------------------------------------------------------------------------

	else {
printf("maik10");
  		// search the list filled with 'attr_value' entries if the requested 'attr_value' already exists
 		while ( p_entry_attr_value != NULL ) {

			// 1 st. check if 'attr_value' entry is for this matching definition + this entrys 'attr_name'
			if ( ( p_definition == p_entry_attr_value->p_entry_definition ) &&
				( p_entry_attr_name == p_entry_attr_value->p_entry_attr_name ) ) {

				// found assigned 'attr_name', break loop and keep current
				break;
			}
		}
printf("maik11");
		// get next 'attr_value' entry to process it
		p_entry_attr_value = LIST_NEXT(p_entry_attr_value, entries);
 	 }

// -------------------------------------------------------------------------------------------------
printf("maik12");
	// 'attr_value' currently not assigned ?
  	if ( p_entry_attr_value == NULL ) {

		// alloc mem for 'attr_value' storage (Entry_Attr_Value_s)
		p_entry_attr_value
			= malloc(sizeof(Entry_Attr_Value_t));


		// store the 'attr_value'
		p_entry_attr_value->attr_value.len = attr_value.len;

		// may be without value ! Length 0!
		if (attr_value.len) {

			p_entry_attr_value->attr_value.p_char = malloc(attr_value.len);
			memcpy(p_entry_attr_value->attr_value.p_char, attr_value.p_char, attr_value.len);
		}
printf("maik13");
		// add new 'attr_value' entry to the attr_value list
		LIST_INSERT_HEAD(&p_definition->head_attr_value, p_entry_attr_value, entries);
printf("maik14");
		// debug msg created
	}

// -------------------------------------------------------------------------------------------------

	else {

		// free old value
		if (p_entry_attr_value->attr_value.p_char) 
			free(p_entry_attr_value->attr_value.p_char);

		// store the 'attr_value'
		p_entry_attr_value->attr_value.len = attr_value.len;

		// may be without value ! Length 0!
		if (attr_value.len) {

			p_entry_attr_value->attr_value.p_char = malloc(attr_value.len);
			memcpy(p_entry_attr_value->attr_value.p_char, attr_value.p_char, attr_value.len);
		}


	}
printf("maik15");
  // get next entry of def_spec matching definitions
  p_entry_dev_spec_matching_definitions = 
	STAILQ_NEXT(p_entry_dev_spec_matching_definitions, entries);
  }



// -------------------------------------------------------------------------------------------------
printf("maik16");

// temporary conversion to make ready ->  head_ret_msg
struct headRetMsgMultiple_s x;
STAILQ_INIT(&x);
x.stqh_first =  head_ret_msg.stqh_first;
x.stqh_last =  head_ret_msg.stqh_last;

  return x; // head_ret_msg;
}







