#include "ProjectConfig.h"
#include <esp8266.h>
#include "SCDE_s.h"

#include "SCDE.h"



//#include <stdarg.h>
/* --------------------------------------------------------------------------------------------------
 *  FName: Log3
 *  Desc: This is the main logging function with 3 infos:
 *        time-stamp, loglevel, creator ant the log-text
 *  Info: Level 0=System; 16=debug
 *        DO NOT FORGET TO FREE char* LogText -> ITS ALLOCATED MEMORY !!!
 *  Para: const uint8_t *name -> the creator name of this log entry
 *        const size_t nameLen -> length of the creator name of this log entry
 *        const uint8_t LogLevel -> the log level of this entry
 *        const char *format -> ptr to the text
 *        ... -> arguments to fill text
 *  Rets: -/-
 * --------------------------------------------------------------------------------------------------
 */
void
Log3 (const uint8_t *name
		,const size_t nameLen
		,const uint8_t LogLevel
		,const char *format
		,...)
{
  // for current time
  time_t nowTist;

  // get current time
  time(&nowTist);

  // create and fill timeinfo struct
  struct tm timeinfo;
  localtime_r(&nowTist, &timeinfo);

  // time,loglevel,name
  printf("Log3|%d.%d.%d %d:%d:%d %d: %.*s: "
		,timeinfo.tm_year+1900
		,timeinfo.tm_mon+1
		,timeinfo.tm_mday
		,timeinfo.tm_hour
		,timeinfo.tm_min
		,timeinfo.tm_sec
		,LogLevel
		,nameLen
		,name);

  // the variable arguments
  va_list list;
  va_start(list, format);
  vprintf(format, list);
  va_end(list);

  // finalize line
  printf("\n");








  //= GetAssignedAttribute("global","verbose");

 // is Device
//stp1: $dev = $dev->{NAME} if(defined($dev) && ref($dev) eq "HASH");

/*stp2:
  if(defined($dev) &&
     defined($attr{$dev}) &&
     defined (my $devlevel = $attr{$dev}{verbose}))
	{
    	return if($loglevel > $devlevel);
	}
  else
	{
	return if($loglevel > $attr{global}{verbose});
	}*/




/*  else
	{
	if ( > GetAttribute("global","verbose")


  char* DevVerbose = GetAttribute(Dev,"verbose");
  char* GlobalVerbose = GetAttribute("global","verbose");
  if ( (GlobalVerbose) &&
*/
  }
