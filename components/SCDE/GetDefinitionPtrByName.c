#include "ProjectConfig.h"
#include <esp8266.h>
#include "SCDE_s.h"

#include "SCDE.h"



/* --------------------------------------------------------------------------------------------------
 *  FName: Get Definition pointer by Name
 *  Desc: 
 *  Para: const size_t definitionNameLen -> length of the Definition name
 *        const uint8_t *definitionName -> Definition name
 *  Rets: Common_Definition_t* -> Pointer to Definition / NULL if not found
 * --------------------------------------------------------------------------------------------------
 */
Common_Definition_t*
GetDefinitionPtrByName(const size_t definitionNameLen
		, const uint8_t *definitionName)
{
  Common_Definition_t *Common_Definition;

  STAILQ_FOREACH(Common_Definition, &SCDERoot.HeadCommon_Definitions, entries) {

	if ( (Common_Definition->nameLen == definitionNameLen) &&
             (!strncasecmp((const char*) Common_Definition->name,
			(const char*) definitionName, definitionNameLen)) ) {
/*
 		#if CORE_SCDE_DBG >= 7
  		LOGD("GetDefinitionPtrByNameFn(%.*s), got Definition ptr.\n"
		  	,definitionNameLen
		  	,definitionName);
 		#endif
*/
		return Common_Definition;
	}
  }
/*
  #if CORE_SCDE_DBG >= 1
  LOGD("GetDefinitionPtrByNameFn(%.*s), Definition not found.\n"
	,definitionNameLen
	,definitionName);
  #endif
*/
  return NULL;
}

