#include "ProjectConfig.h"
#include <esp8266.h>
#include "SCDE_s.h"

#include "SCDE.h"



/* --------------------------------------------------------------------------------------------------
 *  FName: CommandActivateModule
 *  Desc: Activates a Module which is included in this build (internal module) by adding it to
 *        SCDE Root (Modules)
 *  Info: Should be used if loading is not possible ...  
 *  Para: ProvidedByModule_t* ProvidedByModule -> ptr to in built included module
 *  Rets: Module_t* -> Pointer to Module / NULL on error
 * --------------------------------------------------------------------------------------------------
 */
Module_t*
CommandActivateModule (ProvidedByModule_t* ProvidedByNEWModule)
{
  // Call the Initialize Function
  ProvidedByNEWModule->InitializeFn(&SCDERoot);

  // prepare new module and store in SCDE root
  Module_t* NewModule;
  NewModule = (Module_t*) malloc(sizeof(Module_t));
  NewModule->provided = ProvidedByNEWModule;

  //  NewModule->LibHandle = LibHandle;
  STAILQ_INSERT_HEAD(&SCDERoot.HeadModules, NewModule, entries);

//  Log("HCTRL",16,"Module xx activated by main Fn\n");

  return NewModule;
}

