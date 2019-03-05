#include "ProjectConfig.h"
#include <esp8266.h>
#include "SCDE_s.h"

#include "SCDE.h"



/* helper
 * --------------------------------------------------------------------------------------------------
 *  FName: Format Date Time 
 *  Desc: Creates formated date-time-text (uint8_t style - in this case zero terminated) from given
 *        time-stamp. Returned in msgText_t (text in allocated memory + length information)
 *  Note: DO NOT FORGET TO FREE MEMORY !! 
 *  Para: time_t tiSt -> the time-stamp that should be used
 *  Rets: strText_t -> formated date-time-text data
 * --------------------------------------------------------------------------------------------------
 */
strText_t
FmtDateTime(time_t tiSt)
{
  // our msg-text data packet
  strText_t strText;

  // get timeinfo for time-stamp
  struct tm timeinfo;
  localtime_r(&tiSt, &timeinfo);

  // prepare formated-time-string in allocated memory
  strText.strTextLen = asprintf((char**) &strText.strText
	,"%04d-%02d-%02d %02d:%02d:%02d"
	,timeinfo.tm_year+1900
	,timeinfo.tm_mon+1
	,timeinfo.tm_mday
	,timeinfo.tm_hour
	,timeinfo.tm_min
	,timeinfo.tm_sec);

  return strText;
}

