#include "ProjectConfig.h"
#include <esp8266.h>
#include "SCDE_s.h"

#include "SCDE.h"



/* Category: SCDE Core Fn
 * --------------------------------------------------------------------------------------------------
 *  FName: GetUniqueTiSt 
 *  Desc: Returns an UNIQUE SCDE Time Stamp (in most cases the current Time Stamp).
 *  Note: REQUESTING TOO MUCH UNIQUE TIME STAMPS MAY ADVANCE TIME
 *        time(&time_t) returns system time, GetUniqueTiSt(void) returns an current+unique SCDE Time-Stamp !
 *  Para: -/-
 *  Rets: time_t uniqueTiSt -> SCDE Time Stamp (unique)
 * --------------------------------------------------------------------------------------------------
 */
time_t
GetUniqueTiSt(void)
{
  // for time stamp storage
  time_t nowTiSt;

  // get time stamp
  time(&nowTiSt);

  // if lastTiSt is smaller than nowTiSt -> lastTiSt is unique !
  if (SCDERoot.last_timestamp < nowTiSt)
	 SCDERoot.last_timestamp = nowTiSt;

  // else lets make it unique by adding +1
  else SCDERoot.last_timestamp++;

  return SCDERoot.last_timestamp;
}
