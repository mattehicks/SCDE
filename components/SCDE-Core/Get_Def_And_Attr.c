#include "ProjectConfig.h"
#include <esp8266.h>
#include "SCDE_s.h"

#include "SCDE.h"


/* --------------------------------------------------------------------------------------------------
 *  FName: Get_Def_And_Attr
 *  Desc: Returns an SLTQ-head, links to the definition cmd-line and the attribute cmd-lines as
 *        entrys from the given 'entry_definition'
 *  Info: 
 *  Para: Entry_Definition_t* p_entry_definition -> definition for which Def and Attr cmds are requested
 *  Rets: struct Head_String_s -> SLTQ head, stores multiple strings, 
 *                                       the definition CMD line and attribute CMD lines, NULL=NONE
 * --------------------------------------------------------------------------------------------------
 */
struct Head_String_s
Get_Def_And_Attr(Entry_Definition_t* p_entry_definition)
{
  // prepare STAILQ head for multiple retMsg-string storage
  struct Head_String_s head_string;

  // initialize the queue head
  STAILQ_INIT(&head_string);

//---------------------------------------------------------------------------------------------------

  // first get the define cmd-line, but skip global definition - its defined by SCDE on startup
	
  if (! //NOT! skip "global" definition!
	(
	(p_entry_definition->nameLen == 6) &&
	     (memcmp(p_entry_definition->name, "Global", 6) == 0) ) ) {
		
	// alloc new entry string, the retMsg
	Entry_String_t* p_entry_string =
		malloc (sizeof (Entry_String_t));
		
	// check: are definition args stored?
	if(p_entry_definition->definition) {
		
		// write define line with definition args and store it into entry
		p_entry_string->string.len = asprintf(&p_entry_string->string.p_char,
			"define %.*s %.*s %.*s\r\n",
			p_entry_definition->nameLen,
			p_entry_definition->name,
			p_entry_definition->module->provided->typeNameLen,
			p_entry_definition->module->provided->typeName,
			p_entry_definition->definitionLen,
			p_entry_definition->definition);
	}
		
	// no definition args stored ...
	else {

		// write define line without definition args and store it into entry
		p_entry_string->string.len = asprintf(&p_entry_string->string.p_char,
			"define %.*s %.*s\r\n",
			p_entry_definition->nameLen,
			p_entry_definition->name,
			p_entry_definition->module->provided->typeNameLen,
			p_entry_definition->module->provided->typeName);
	}
		
	// insert entry string (the retMsg) into stail-queue
	STAILQ_INSERT_TAIL(&head_string, p_entry_string, entries);		
  }

//---------------------------------------------------------------------------------------------------

  // second the attribute(s)

  // loop the attributes stored for this definition for processing
  Entry_Attr_Value_t* p_entry_attr_value;

  LIST_FOREACH(p_entry_attr_value, &p_entry_definition->head_attr_value, entries) {

	if (
		// is current entry the "global" Definition ? and
		( ( p_entry_definition->nameLen == 6 ) &&
		( memcmp ( p_entry_definition->name, "Global", 6) == 0 ) ) &&

		(

		// is the 'attr-name' "configfile"? or
		( ( p_entry_attr_value->p_entry_attr_name->attr_name.len == 10 ) &&
		( memcmp ( p_entry_attr_value->p_entry_attr_name->attr_name.p_char,
			 "configfile", 10 ) == 0 ) ) ||

		// is the 'attr-name' "version"?
		( ( p_entry_attr_value->p_entry_attr_name->attr_name.len == 7 ) &&
		( memcmp ( p_entry_attr_value->p_entry_attr_name->attr_name.p_char,
			"version", 7 ) == 0 ) ) 
						)
							) {

		// When exec here: current Attribute is "configfile" or "version"
		// Skip, its defined by SCDE on startup
	}

	// standard Attribute, that should be stored ..
	else {
		// alloc new entry string, the retMsg
		Entry_String_t* p_entry_string =
			malloc (sizeof (Entry_String_t));

		// write attr-line to allocated memory and store to into entry
		p_entry_string->string.len = asprintf(&p_entry_string->string.p_char,
			"attr %.*s %.*s %.*s\r\n",
			p_entry_definition->nameLen,
			p_entry_definition->name,
			p_entry_attr_value->p_entry_attr_name->attr_name.len,
			p_entry_attr_value->p_entry_attr_name->attr_name.p_char,
			p_entry_attr_value->attr_value.len,
			p_entry_attr_value->attr_value.p_char);

		// insert entry string (the retMsg) into stail-queue
		STAILQ_INSERT_TAIL(&head_string, p_entry_string, entries);
	}
  }

  // return STAILQ head, stores multiple entries string (the retMsg), if STAILQ_EMPTY -> none
  return head_string;
}



