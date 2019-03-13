#include "ProjectConfig.h"
#include <esp8266.h>
#include "SCDE_s.h"

#include "SCDE.h"



/* --------------------------------------------------------------------------------------------------
 *  FName: Get loaded Module pointer by Name
 *  Desc: Returns the ptr to an already loaded Module
 *  Para: const uint8_t *typeName -> Type-Name
 *        const size_t typeNameLen -> length of the Type-Name
 *  Rets: Module_t* -> Pointer to Module / NULL if not found
 * --------------------------------------------------------------------------------------------------
 */
Module_t*
GetLoadedModulePtrByName(const uint8_t* p_type_name
		, const size_t type_name_len)
{
  Module_t* p_module;

  STAILQ_FOREACH(p_module, &SCDERoot.HeadModules, entries) {

	if ( (p_module->provided->typeNameLen == type_name_len) &&
             (!strncasecmp((const char*) p_module->provided->typeName,
		 (const char*) p_type_name, type_name_len)) ) {
/*
 		#if CORE_SCDE_DBG >= 7
  		LOGD("GetLoadedModulePtrByNameFn(%.*s), got loaded Module ptr.\n"
		  	,typeNameLen
		  	,typeName);
 		#endif
*/
		return p_module;
	}
  }
/*
  #if CORE_SCDE_DBG >= 1
  LOGD("GetLoadedModulePtrByNameFn(%.*s), Module name not loaded.\n"
	,typeNameLen
	,typeName);
  #endif
*/
  // pointer for given Module (Type-Name) not found!
  return NULL;
}

