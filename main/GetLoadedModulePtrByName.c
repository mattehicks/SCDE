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
GetLoadedModulePtrByName(const uint8_t *typeName
		, const size_t typeNameLen)
{
  Module_t *Module;

  STAILQ_FOREACH(Module, &SCDERoot.HeadModules, entries) {

	if ( (Module->provided->typeNameLen == typeNameLen) &&
             (!strncasecmp((const char*) Module->provided->typeName,
		 (const char*) typeName, typeNameLen)) ) {
/*
 		#if CORE_SCDE_DBG >= 7
  		LOGD("GetLoadedModulePtrByNameFn(%.*s), got loaded Module ptr.\n"
		  	,typeNameLen
		  	,typeName);
 		#endif
*/
		return Module;
	}
  }
/*
  #if CORE_SCDE_DBG >= 1
  LOGD("GetLoadedModulePtrByNameFn(%.*s), Module name not loaded.\n"
	,typeNameLen
	,typeName);
  #endif
*/
  return NULL;
}

