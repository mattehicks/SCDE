#include "ProjectConfig.h"
#include <esp8266.h>
#include "SCDE_s.h"

#include "SCDE.h"



/** Category: attribute management, helper
 * -------------------------------------------------------------------------------------------------
 *  FName: Get_attrVal_by_defName_and_attrName   //GetAttrValTextByDefTextAttrText
 *  Desc: Returns the attribute value assigned to an attribute-name, for the definition
 *  Note: DO NOT FORGET TO FREE MEMORY !!!
 *        if (attrVal->strText) free (attrVal->strText) !
 *        if (attrVal) free (attrVal) !
 *  Para: const strText_t *defName -> ptr to the definition name we are searching for
 *        const strText_t *attrName -> ptr to the attribute name we are searching for
 *  Rets: strText_t *attrVal = NULL -> not found / assigned)
 *               or *attrVal != NULL & attrVal->strText = NULL -> found, but no value assigned
 *             else *attrVal -> the assigned attribute value text an textLen
 * -------------------------------------------------------------------------------------------------
 */
strText_t*
Get_attrVal_by_defName_and_attrName(const strText_t *defName
				,const strText_t *attrName)  
{

  strText_t *attrVal;

  attribute_t *attribute =
		STAILQ_FIRST(&SCDERoot.headAttributes);

	// loop through the assigned attributes and try to find the attribute by name
  while (true) {

		// no assigned attribute found ?
		if (attribute == NULL) {

			// not found
			attrVal = NULL;

			return attrVal;

		}

		// is this the attribute we are searching for?
		if ( (attribute->attrAssignedDef->nameLen == defName->strTextLen)
			&& (!strncasecmp((const char*) attribute->attrAssignedDef->name, (const char*) defName->strText, defName->strTextLen))
			&& (attribute->attrNameTextLen == attrName->strTextLen)
			&& (!strncasecmp((const char*) attribute->attrNameText, (const char*) attrName->strText, attrName->strTextLen)) ) {

			// found, reserve memory for the attrVal
			attrVal = malloc(sizeof(strText_t));	

			// assigned attrValText has data, make a copy
			if (attribute->attrValText) {

				attrVal->strText = malloc(attribute->attrValTextLen);

				memcpy(attrVal->strText, attribute->attrValText, attribute->attrValTextLen);

				attrVal->strTextLen = attribute->attrValTextLen;
			}

			// assigned attrValText has NO data
			else {

				attrVal->strText = NULL;
			}

			// Debug output ... 

		// attribute found ? -> yes / no ?
		if (attrVal) {

			// yes attribute found -> has text or not ?
			if (attrVal->strText) {

				// has text
				LOGD("Got attrVal:%.*s assigned to attrName:%.*s for defName:%.*s\n"
					,attrVal->strTextLen
					,attrVal->strText
					,attribute->attrNameTextLen
					,attribute->attrNameText
					,attribute->attrAssignedDef->nameLen
					,attribute->attrAssignedDef->name);
			}

			else {

				// yes but has NO text
				LOGD("No value assigned to attrName:%.*s for defName:%.*s\n"
					,attribute->attrNameTextLen
					,attribute->attrNameText
					,attribute->attrAssignedDef->nameLen
					,attribute->attrAssignedDef->name);
			}
		}

		else {

				// NOT found
				LOGD("Assigned attrName:%.*s for defName:%.*s NOT found!\n"
					,attribute->attrNameTextLen
					,attribute->attrNameText
					,attribute->attrAssignedDef->nameLen
					,attribute->attrAssignedDef->name);
			}

		// found, break
		break;
		}

	// get next attribute for processing
	attribute = STAILQ_NEXT(attribute, entries);
  }

  return attrVal;
}

