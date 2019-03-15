#include "ProjectConfig.h"
#include <esp8266.h>
#include "SCDE_s.h"

#include "SCDE.h"



/* Category: SCDE Core Fn
 * -------------------------------------------------------------------------------------------------
 *  FName: doGlobalDef
 *  Desc: Initializes the global device
 *  Para: const uint8_t *cfgFileName -> Cfg-File-Name
 *        const size_t cfgFileNameLen -> length of the Cfg-File-Name
 *  Rets: Module_t* -> Pointer to Module / NULL if not found
 * -------------------------------------------------------------------------------------------------
 */
void
doGlobalDef(const uint8_t *cfgFileName
		, const size_t cfgFileNameLen)
{

/*  LOGD("doGlobalDef - cfgFileName:%.*s\n"
	,cfgFileNameLen
	,cfgFileName);*/

  SCDERoot.device_count = 1;

/*
3326	  $defs{global}{NR}    = $devcount++;
3327	  $defs{global}{TYPE}  = "Global";
3328	  $defs{global}{STATE} = "no definition";
3329	  $defs{global}{DEF}   = "no definition";
3330	  $defs{global}{NAME}  = "global";
3331	
3332	  CommandAttr(undef, "global verbose 3");
3333	  CommandAttr(undef, "global configfile $arg");
3334	  CommandAttr(undef, "global logfile -");
3335	}
3336	
*/


  // start the internal WebIF (built in module)
  // define Name:MyTelnet Module:Telnet -> Args: Port:23
//#define CMD_4_TELNET "define MyTelnet Telnet 23"

  // there should be no return messages - we expect no return messages
  AnalyzeCommand((const uint8_t *) "attr global verbose 3", 21);
  AnalyzeCommand((const uint8_t *) "attr global logfile -", 21);

  return;
}	
		
