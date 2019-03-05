#include "ProjectConfig.h"
#include <esp8266.h>
#include "SCDE_s.h"

#include "SCDE.h"



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: InitSCDERoot
 *  Desc: Initializes the SCDERoot data structure
 *  Info: 
 *  Para: -/- 
 *  Rets: -/-
 * -------------------------------------------------------------------------------------------------
 */
void
InitSCDERoot(void) {
  SCDERoot.SCDEFn = &SCDEFn;

  STAILQ_INIT(&SCDERoot.headAttributes);

  STAILQ_INIT(&SCDERoot.headCommands);

  STAILQ_INIT(&SCDERoot.HeadCommon_Definitions);

  STAILQ_INIT(&SCDERoot.HeadModules);
}
