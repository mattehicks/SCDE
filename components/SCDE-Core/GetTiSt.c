#include "ProjectConfig.h"
#include <esp8266.h>
#include "SCDE_s.h"

#include "SCDE.h"



/* Category: SCDE Core Fn
 * --------------------------------------------------------------------------------------------------
 *  FName: GetTiSt
 *  Desc: Returns current SCDE Time Stamp.
 *  Note: MAY DIFFER FROM SYSTEM TIME STAMP, BECAUSE REQUSTED UNIQUE TIME STAMPS MAY ADVANCE TIME
 *        time(&time_t) returns system time, GetTiSt(void) returns current SCDE Time-Stamp !
 *  Para: -/-
 *  Rets: time_t tiST -> SCDE Time Stamp
 * --------------------------------------------------------------------------------------------------
 */
//  static time_t uniqueTiSt; //in SCDE.h! 
time_t
GetTiSt(void)
{
  // for time stamp storage
  time_t nowTiSt;

  // get time stamp
  time(&nowTiSt);

  // only return uniqueTiSt when it is ahead of nowTiSt
  if (SCDERoot.last_timestamp > nowTiSt)
	nowTiSt = SCDERoot.last_timestamp;

  return nowTiSt;
}
