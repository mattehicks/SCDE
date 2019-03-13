#include "ProjectConfig.h"
#include <esp8266.h>
#include "SCDE_s.h"

#include "SCDE.h"



/* Category: SCDE Core Fn
 * --------------------------------------------------------------------------------------------------
 *  FName: CommandReload
 *  Desc: Initially loads or executes a reload of an Module of given Type-Name
 *  Info:  
 *  Para: const uint8_t typeName -> Text string of Modules Type-Name that should be reloaded 
 *        const size_t typeNameLen -> length of the typeName
 *  Rets: Module_t* -> Pointer to Module / NULL on error
 *--------------------------------------------------------------------------------------------------
 */
Module_t*
CommandReloadModule(const uint8_t* p_typeame
	,const size_t typeNameLen)
{
  //  printf ("Command Reload called. Loading Module Type: %s", text);
  Log("HCTRL",16,"Loading Module");


  return NULL;
  // we need ptr to Module_t. Contains function callbacks for module operation
  ProvidedByModule_t* ProvidedByNEWModule;

  ProvidedByNEWModule = (ProvidedByModule_t*) malloc(sizeof(ProvidedByModule_t));
  // search List of embedded Modules by TypeName



//  ProvidedByNEWModule =

/*  // loading external Module
  else
	{

	// handle for the lib
	void *LibHandle;

	// lib handle 
	LibHandle = my_load_dyn (TypeName);

	// Get ptr to ProvidedByModule_t. We need the function callbacks for module operation
	ProvidedByModule_t* ProvidedByNEWModule;
	printf ("Die Werte sind nicht gleich\n");
	ProvidedByNEWModule = dlsym(LibHandle, "ProvidedByModule");

	}
*/


  Log("HCTRL",16,"Module loaded/n");
/*
  if (LoadedModule == NULL)

		{

		printf ("Fehler bei dlsym() kiadmodule: %s\n", dlerror ());
		exit (EXIT_FAILURE);

		}
*/


  // Call the Initialize Function
//  ProvidedByNEWModule->InitializeFn(&SCDEFn);

  // prepare new module and store 
  Module_t* NewModule;
  NewModule = (Module_t*) malloc(sizeof(Module_t));
  NewModule->provided = ProvidedByNEWModule;
//  NewModule->LibHandle = LibHandle;
  STAILQ_INSERT_HEAD(&SCDERoot.HeadModules, NewModule, entries);

  Log("HCTRL",16,"Module initializzed/n");

/*
  Log3((const uint8_t*) Name
	,sizeof(Name)
	,1
	,"CommandAttr called. Creating for definition <%s> a new AttrName <%s> with AttrValue <%s>"
	,Name
	,AttrName
	,AttrValue);
*/


 // debug: list currently stored modules
  Module_t *Module;
  STAILQ_FOREACH(Module, &SCDERoot.HeadModules, entries)
	{
	printf("LM Name:\"%.*s\"\n"
		,Module->provided->typeNameLen
		,Module->provided->typeName);
	}

  return NewModule;
}

