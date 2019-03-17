#include "ProjectConfig.h"
#include <esp8266.h>
#include "SCDE_s.h"

#include "SCDE.h"



/** Category: attribute management, helper
 * -------------------------------------------------------------------------------------------------
 *  FName: Get 'attr_val' (as String_t*) by 'def_name' and 'attr_name'
 *  Desc: Returns the Attribute Value assigned to an Attribute-Name and Definition
 *  Note: DO NOT FORGET TO FREE MEMORY !!!
 *        if (attrVal->strText) free (attrVal->strText) !
 *        if (attrVal) free (attrVal) !
 *  Para: const String_t* def_name -> the definition name we are searching for
 *        const String_t* attr_name -> the attribute name we are searching for
 *  Rets: String_t *attr_value = NULL -> not found / assigned)
 *            else *attr_value -> the assigned attribute value
 *           note: *attr_value != NULL & attr_value.len = 0 and .p_char = NULL -> found, no value assigned
 * -------------------------------------------------------------------------------------------------
 */
String_t*
Get_Attr_Val_By_Def_Name_And_Attr_Name(const String_t* p_def_name
				,const String_t* p_attr_name)  
{
  String_t* p_attr_value = NULL;

  // get first entry from our 'attr_name' list
  Entry_Attr_Name_t* p_entry_attr_name = 
	LIST_FIRST (&SCDERoot.head_attr_name);

  // loop through the assigned attr_names and try to find an matching attribute by name
  while ( p_entry_attr_name != NULL ) {

	// compare 'attr_name' character length & characters
	if ( ( p_entry_attr_name->attr_name.len == p_attr_name->len ) && 
		( !strncasecmp((const char*) p_entry_attr_name->attr_name.p_char,
		(const char*) p_attr_name->p_char, p_attr_name->len) ) ) {

		// get first entry from our 'definition' list
		Common_Definition_t* p_entry_definition = 
			STAILQ_FIRST(&SCDERoot.HeadCommon_Definitions);

		while ( p_entry_definition != NULL ) {

			// compare 'definition_name' character length & characters. Match ?
			if ( ( p_entry_definition->nameLen == p_def_name->len )
				&& ( !strncasecmp((const char*) p_entry_definition->name,
				(const char*) p_def_name->p_char,
				p_def_name->len) ) ) {

				// get first entry from definitions 'attr_value' list
  				Entry_Attr_Value_t* p_entry_attr_value = 
					LIST_FIRST(&p_entry_definition->head_attr_value);

				while ( p_entry_attr_value != NULL ) {
	
					// compare if this 'attr_value' is the requested 'attr_name' ?
					if ( p_entry_attr_value->p_entry_attr_name == p_entry_attr_name ) {

						// Match ! This is attr_value for attr_name & definition!
						p_attr_value = &p_entry_attr_value->attr_value;

						return p_attr_value;
					}

					// get next 'attr_value' entry to process it
					p_entry_attr_value = LIST_NEXT(p_entry_attr_value, entries);
				}
 		 	}

			// get next 'definition' entry to process it
			p_entry_definition = STAILQ_NEXT(p_entry_definition, entries);
		}
  	}

	// get next 'attr_name' entry to process it
	p_entry_attr_name = LIST_NEXT(p_entry_attr_name, entries);
  }

  // No match! -> Return NULL
  return p_attr_value;
}



