#include "ProjectConfig.h"
#include <esp8266.h>
#include "SCDE_s.h"

#include "SCDE.h"



/** Category: SCDE Core Fn
 * -------------------------------------------------------------------------------------------------
 *  FName: GoodDeviceName
 *  Desc: Checks the given Device Name if it is in compliance with the device name rules:
 *        - normal letters, without special signs (A-Z, a-z)
 *        - numbers (0-9), point (.) and underscore (_)
 *  Info: Device Name rules = definition name rules
 *  Para: const xString_s nameString -> the Device Name string that should be checked
 *  Rets: bool false -> not good ; bool true -> good device name
 * -------------------------------------------------------------------------------------------------
 */
bool
GoodDeviceName(const xString_t nameString)
{

// CODE HERE IS NOT COMPLETE - ONLY FOR DEBUGGING

  return true;
}
