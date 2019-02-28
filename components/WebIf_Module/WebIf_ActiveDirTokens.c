//##############################################################################
//###  Function: Project specific content for active directory processing
//###  ESP 8266EX SOC Activities ...
//###  (C) EcSUHA
//###  Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany 
//###  MSchulze780@GMAIL.COM
//###  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
//##############################################################################

#include <ProjectConfig.h>
#include <esp8266.h>
#include <Platform.h>

#include <SCDE_s.h>

#include "WebIf_Module.h"

#include "WebIf_ActiveDirTokens.h"


/*
 *--------------------------------------------------------------------------------------------------
 *FName: HTTPD_ADirDevName
 * Desc: check if the Devicename is requested (compare with SysCfgRamFlashMirrored->Ap_ssid)
 * Para: UrlProcHelper *MyUrlProcHelper -> helper for str ptr storage
 *       int NumFeat -> number of features embedded in this project
 * Rets: -1 = compared OK; -3 = not the same
 *--------------------------------------------------------------------------------------------------
 */
/*
int ICACHE_FLASH_ATTR
HTTPD_ADirDevName(UrlProcHelper *MyUrlProcHelper)
  {
  // get Devicename from current Service AP settings
  struct softap_config softap_config;
  wifi_softap_get_config (&softap_config);

  char *s = MyUrlProcHelper->SrcPtr;
  char *d = (char*) &softap_config.ssid;

  while ( (*s == *d) &&
		(*d != '\0') )
		{
		s++;
		d++;
		}

  MyUrlProcHelper->SrcPtr = s;

  if (*d == 0) return -1; // -1 = the same, indicates OK

  else return -3;	  // not the same = -3 -> error not found
  }
*/






















/*
 *--------------------------------------------------------------------------------------------------
 *FName: HTTPD_ADirFeatNr
 * Desc: check if the Feature-No is encoded as number (reads+checks number)
 * Para: UrlProcHelper *MyUrlProcHelper -> helper for str ptr storage
 *       int NumFeat -> number of features embedded in this project
 * Rets: Number of found feature (1-1); -3 if ERROR
 *--------------------------------------------------------------------------------------------------
 */
Common_Definition_t*			//http://192.168.0.56/1/SwITCH.htm
WebIf_ADirFeatureNr(Common_Definition_t* Common_Definition,
	const char** SrcPtr,
	const char** UrlSeekPtr,
	int* tokenExecResult)
{
/*
  char* s = *SrcPtr;

//#if SCDED_DBG >= 9
  printf(">tokenFNr>");
//#endif	

  // for extracted number
  int n = 0;

  // extract number
  while ( ( *s >= '0') && (*s <= '9') ) {

	n = n * 10 + *s - '0';

	s++;
  }

  // check if this number is valid


  // Got number ! Lets get current number of this definition 
  // !! ITS NOT DIRECTLY ASSIGNED - ONLY CURRENT POSITION IN LOOP !!!
  // (count Module->assigned definitions till name match and verify with requested)
  int count = 0;

  // get the Common_Definitions for current module
  Common_Definition_t *analyzeCommon_Definition;

  STAILQ_FOREACH(analyzeCommon_Definition, &SCDERoot->HeadCommon_Definitions, entries) {

	// matching module type
	if (analyzeCommon_Definition->module == Common_Definition->module) {

		// yes a match, count
		count++;

		// if the name matches we have it!
		if ( (Common_Definition->nameLen == analyzeCommon_Definition->nameLen)
			&& (!strncasecmp((const char*) Common_Definition->name,
			   (const char*) analyzeCommon_Definition->name,
			   Common_Definition->nameLen)) ) {

			if ( n == count ) {

				//	#if SCDED_DBG >= 9
				printf(">match!>");
//				#endif	

				// correct ptr to verified position
				*SrcPtr = s;

				return Common_Definition;
			}

			else break;
		}
 	}
  }
*/

  // NO valid + existing number !

//#if SCDED_DBG >= 9
  printf(">no match!");
//#endif	

  *tokenExecResult = -3;

  return NULL;
}



/*
  int n = 0;

  while (( *MyUrlProcHelper->SrcPtr >= '0')
	&& (*MyUrlProcHelper->SrcPtr <= '9'))
	{
	n = n * 10 + *MyUrlProcHelper->SrcPtr++ - '0';
	
	if (n > NumFeat) return -3; //  -3 -> error, out of rang
	}

  if (n == 0) return -3; //  -3 -> error, not found

  n--; // wie die anderen, X-1

  return n;	// value ok & in range
}
*/



/*
 *--------------------------------------------------------------------------------------------------
 *FName: WebIf_ADirFeatureName
 * Desc: check if the Featurename is requested (compare with Name[0-X])
 * Para: UrlProcHelper *MyUrlProcHelper -> helper for str ptr storage
 *       int NumFeat -> number of features embedded in this project
 * Rets: Number of found feature (1-1); -3 if ERROR
 *--------------------------------------------------------------------------------------------------
 */
Common_Definition_t*			//http://192.168.0.56/SSR.1.at.GPIO.13.htm
WebIf_ADirFeatureName(Common_Definition_t* Common_Definition,
	const char** SrcPtr,
	const char** UrlSeekPtr,
	int* tokenExecResult)
{
  char* s = *SrcPtr;
  char* d = (char *) Common_Definition->name;
  int dLen = Common_Definition->nameLen;

//	#if SCDED_DBG >= 9
	printf(">tokenFName>");
//	#endif	

  while ( (*s == *d) &&
//	(  *d != '\0') ) {
	( dLen-- ) ) {

//	#if SCDED_DBG >= 9
	printf("%c", *s);
//	#endif	

	s++;
	d++;
  }

  // full length match ?
 // if (*d == 0) {
  if (!dLen) {

//	#if SCDED_DBG >= 9
	printf(">match!>");
//	#endif	

	// correct ptr to verified position
	*SrcPtr = s;

	return Common_Definition;
  }

 // NO full length match !
//#if SCDED_DBG >= 9
  printf(">no match!");
//#endif	

  *tokenExecResult = -3;

  return NULL;
}


/*



  while ( 0 < NumFeat--) {

	char *s = MyUrlProcHelper->SrcPtr;
	char *d = NULL;//SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[NumFeat].IB01_Name;

	while ( (*s == *d) &&
		(*d != '\0') ) {

		s++;
		d++;
	}

	if (*d == 0) {

		MyUrlProcHelper->SrcPtr = s;
		return Common_Definition; // n+1 found
	}

  }
  return (Common_Definition_t*) -3; // -3=Error
}

http://192.168.0.56/SSR.1.at.GPIO.13.htm
*/



/*
 *--------------------------------------------------------------------------------------------------
 *FName: WebIf_ExecActiveDirToken
 * Desc: Check for special character (token) at UrlSeekPtr that indicates active content and process
 * Para: WebIf_HTTPDConnSlotData *conn -> ptr to current connection slot
 *       Common_Definition_t* Common_Definition -> Definition that is checked for a match
 *       const char* SrcPtr -> ptr to the source -> requested url
 *       const char* UrlSeekPtr -> ptr to the url in Active-Dir-Url-Res-Data
 *       int* tokenExecResult -> place where the result should be stored
 * Rets: bool -> true will break Url match loop
 *--------------------------------------------------------------------------------------------------
 */
bool
WebIf_ExecActiveDirToken(WebIf_HTTPDConnSlotData_t* conn,
	Common_Definition_t* Common_Definition,
	const char** SrcPtr,
	const char** UrlSeekPtr,
	int* tokenExecResult)
{
  // ActiveID init for processing,
  // -3 error not found,
  // -2 empty and no error,
  // -1 found without value,
  // 0-x value found 
  // in case =>-1 forcing alternative Filename !!!

  // Check if the current token at UrlSeekPtr is in an 'Active Directory Content Token'
  if ( (**UrlSeekPtr >= ADC_DevName) && (**UrlSeekPtr < ADC_LAST) ) {

	// yes, load token as 'special function' and go to next char ptr++
	char SpecFunc = **UrlSeekPtr;
	*UrlSeekPtr = *UrlSeekPtr+1;

	// Active Directory Content Functions. For all projects

	// Device Name ?
//	if      (SpecFunc == ADC_DevName) conn->ActiveDirID = 
//		HTTPD_ADirDevName(MyUrlProcHelper);		// -3 in case of ERROR, -1 OK

	// Force alternative file name from file system ?
	if (SpecFunc == ADC_ForceAFN) {

		*tokenExecResult = -1;	// -1 -> force alternative filename

//		#if SCDED_DBG >= 9
		printf(">tokenAFN>");
//		#endif
	}

	// all others: active content xxxxxxxwith 'No. of Features Information'
	else {	

/*		// read: 'No. of Features Information'
		uint8_t HiByte = **UrlSeekPtr;
		uint8_t LoByte = **UrlSeekPtr;
		*UrlSeekPtr = *UrlSeekPtr+1;
		*UrlSeekPtr = *UrlSeekPtr+1
		int NumFeat = HiByte<<8 | LoByte;
*/

		// check: active directory content token -> Feature Number ?
		// results: tokenExecResult -3 in case of ERROR
		if (SpecFunc == ADC_FeatNr) conn->activeDirFndDefiniton =
			WebIf_ADirFeatureNr(Common_Definition, SrcPtr, UrlSeekPtr, tokenExecResult);

		// check: active directory content token -> Feature Name ?
		// results: tokenExecResult -3 in case of ERROR
		if (SpecFunc == ADC_FeatureName) conn->activeDirFndDefiniton =
			WebIf_ADirFeatureName(Common_Definition, SrcPtr, UrlSeekPtr, tokenExecResult);
		}

//	if (conn->ActiveDirID >= 0) conn->ActiveDirID++; // Name 0 is 1 to the user    äNDERN?????????

	// succesfull, in range, false continues loop in httpd.c, (if ADID is not -3)
	return false;
	}

  // not successful, true breaks loop in httpd.c
  else return true;
}


