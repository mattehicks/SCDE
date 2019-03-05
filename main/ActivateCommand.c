#include "ProjectConfig.h"
#include <esp8266.h>
#include "SCDE_s.h"

#include "SCDE.h"



/* --------------------------------------------------------------------------------------------------
 *  FName: CommandActivateCommand
 *  Desc: Activates a Command which is included in this build (internal module) by adding it to
 *        SCDE Root (Commands)
 *  Info: Should be used if loading is not possible ...  
 *  Para: ProvidedByCommand_t* ProvidedByCommand -> ptr to in built included Command
 *  Rets: Command_t* -> Pointer to Command / NULL on error
 * --------------------------------------------------------------------------------------------------
 */
Command_t*
CommandActivateCommand (ProvidedByCommand_t* providedByNEWCommand)
{
  // Call the Initialize Function
  providedByNEWCommand->InitializeCommandFn(&SCDERoot);

  // prepare new Command and store in SCDE root
  Command_t* newCommand;
  newCommand = (Command_t*) malloc(sizeof(Command_t));
  newCommand->provided = providedByNEWCommand;

  //  NewModule->LibHandle = LibHandle;
  STAILQ_INSERT_HEAD(&SCDERoot.headCommands, newCommand, entries);

//  Log("HCTRL",16,"Command xx activated by main Fn\n");

  return newCommand;
}

