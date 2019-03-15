#include "ProjectConfig.h"
#include <esp8266.h>
#include "SCDE_s.h"

#include "SCDE.h"



/* SCDE helper
 * --------------------------------------------------------------------------------------------------
 *  FName: Get Definitions that match 'def_spec' string
 *  Desc: Returns a STAILQ head that stores entries of all 'def_spec.' matching definitions.
 *  Para: const String_t def_spec. -> the 'def_spec' string (definition specification)
 *  Rets: Head_Definitions_s -> Pointer to STAILQ head / STAILQ_EMPTY -> no matching definitions
 * --------------------------------------------------------------------------------------------------
 */
struct Head_Definitions_s
Get_Definitions_That_Match_DefSpec_String(const String_t def_spec)
{
  // prepare STAILQ head to store entries of 'def_spec.' matching definitions
  struct Head_Definitions_s head_def_spec_matching_definitions;

  // Initialize the queue head
  STAILQ_INIT(&head_def_spec_matching_definitions);

//---------------------------------------------------------------------------------------------------

// CODE HERE IS NOT COMPLETE - ONLY FOR DEBUGGING

 // assume 'def_spec.' = 'definition_name'
  Common_Definition_t* p_definition;

  STAILQ_FOREACH(p_definition, &SCDERoot.HeadCommon_Definitions, entries) {

	if ( (p_definition->nameLen == def_spec.len) &&
             (!strncasecmp((const char*) p_definition->name,
			(const char*) def_spec.p_char, def_spec.len)) ) {

		// alloc an definitionMultipleString queue element
		Entry_Definitions_t* p_entry_def_spec_matching_definition =
			malloc(sizeof(Entry_Definitions_t));

		// store ptr to definition into queue entry 
		p_entry_def_spec_matching_definition->p_entry_definition = 
			p_definition;
	
		// insert 'def-spec' matching queue entry into stail-queue
		STAILQ_INSERT_TAIL(&head_def_spec_matching_definitions,
			p_entry_def_spec_matching_definition, entries);
	}
  }

// else NULL -> none found

// CODE HERE IS NOT COMPLETE - ONLY FOR DEBUGGING

//---------------------------------------------------------------------------------------------------

  // return STAILQ head, stored are multiple (all) matching definition entries,
  // if STAILQ_EMPTY -> no matching definitions
  return head_def_spec_matching_definitions;
}



