//##################################################################################################
//###  Function: Callback for System-on-Chip Hardware Configuration (tpl resource)
//###  ESP 8266EX SOC Activities ...
//###  (C) EcSUHA
//###  Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany 
//###  MSchulze780@GMAIL.COM
//###  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
//##################################################################################################
#include "ProjectConfig.h"
#include <esp8266.h>
#include "WebIf_Module.h"
#include "Platform.h"



//#include "c_types.h"
//#include <string.h>
//#include <osapi.h>
//#include "user_interface.h"
//#include "mem.h"
//#include "SCDED.h"
//#include "espmissingincludes.h"

//#include "CGI_SoCCfg.h"
#include "CGI_NoAuthErr.h"
#include "CGI_NotFoundErr.h"



#include "Device_WebIfTokens.h"




/*
 *--------------------------------------------------------------------------------------------------
 *FName: Device_GeneralWEBIF_tpl
 * Desc: Devicespecific general WEB-IF Template %X% replacement Code for all active Page
 * Info: Called before page/resource specific 'Template %X% replacement Code'
 * Para: HTTPDConnSlotData *conn -> ptr to connection slot
 *       char *token -> ptr to string which contains the token to replace
 *       void **arg -> ???
 * Rets: int bytes -> free bytes in send buff, -1 = nothing replaced / added to sent buff
 *--------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR 
Device_WEBIFtokens(WebIf_HTTPDConnSlotData_t *conn, char *token, void **arg) 
{
  // bytes free in send buff, -1 indicates that nothing was sent!
  int BytesFree = -1;

  switch(token[0])

	{

//--------------------------------------------------------------------------------------------------

	case '?':				// '?' -> Device Name: Custom Name

	{

/*	// get Devicename from current Service AP settings
	struct softap_config softap_config;

	wifi_softap_get_config (&softap_config);

	BytesFree = HTTPD_Send_To_Send_Buffer(conn
		,(char*) &softap_config.ssid
		,-1);*/

	// get Devicename from current Service AP settings
	wifi_ap_config_t wifi_ap_config;

	esp_wifi_get_config (WIFI_IF_AP, &wifi_ap_config);

	BytesFree = HTTPD_Send_To_Send_Buffer(conn,
		(char*) &wifi_ap_config.ssid, -1);
	}

	break;

//--------------------------------------------------------------------------------------------------

	case '!':				// '!' -> Device Info: WeCallIt

	{
/*spz
	BytesFree = HTTPD_Send_To_Send_Buffer(conn
		,SysCfgRamFlashMirrored->InfoWeCallIt
		,-1);
*/
	}

	break;

//--------------------------------------------------------------------------------------------------

	case '=':				// '§' -> Device Info: EmbeddedFeatures

	{
/*spz
	BytesFree = HTTPD_Send_To_Send_Buffer(conn
		,SysCfgRamFlashMirrored->InfoEmbeddedFeat
		,-1);
*/
	}

	break;

//--------------------------------------------------------------------------------------------------

	case '$':				// '$' -> Device WEB-IF: Menu

	{

	static const char WEBIFMenu[] ICACHE_RODATA_ATTR = 
	"<li><a href=\"/CoNTROL_8S-1C-1ADC.htm\">Device Feature Ctrl</a></li>"
	"<li><a href=\"/SwITCH.htm\">SwITCH Feature Ctrl</a></li>"
	"<li><a href=\"/1/SwITCH.htm\">/X/SwITCH Res Ctrl</a></li>"
	"<li><a href=\"/ClIMA.htm\">ClIMA Feature Ctrl</a></li>"
	"<li><a href=\"/1/ClIMA.htm\">/X/ClIMA Res Ctrl</a></li>"
	"<li><a href=\"/ADC.htm\">ADC Feature Ctrl</a></li>"
	"<li><a href=\"/1/ADC.htm\">/X/ADC Res Ctrl</a></li>"
	"<li><a href=\"/BasServ.htm\">Basic  Services</a></li>"
	"<li><a href=\"/AdvServ.htm\">Advanced Services</a></li>"
	"<li><a href=\"/GenAttr.htm\">General Attributes</a></li>"
	"<li><a href=\"/SpcAttr.htm\">Special Attributes</a></li>"
	"<li><a href=\"/DeviceCfg.htm\">Device General Cfg</a></li>"
	"<li><a href=\"/SoCHWCfg.htm\">SoC Hardware Cfg</a></li>"
	"<li><a href=\"/WiFi/StationCfg.htm\">WLAN Station Cfg</a></li>"
	"<li><a href=\"/WiFi/QConnect.htm\">WLAN Station QConn</a></li>"
	"<li><a href=\"/WiFi/ServAPCfg.htm\">WLAN Service AP Cfg</a></li>"
	"<li><a href=\"/TiStCfg.htm\">Time Stamp Cfg</a></li>"
	"<li><a href=\"/Firmware.htm\">Firmware Update</a></li>"
	"<li><a href=\"http://www.ecsuha.de/boards/topic/4/control-8s-1c-1adc-version-ssr-und-relay\">Get Web Support</a></li>"
	"<li><a href=\"/UserManual.htm\">Read User Manual</a></li>"
	"<li><a href=\"/RESTAPIRef.htm\">REST API Reference</a></li>"
	"<li><a href=\"/SafetyWarning.htm\">Safety Warnings</a></li>";
	BytesFree = HTTPD_Send_To_Send_Buffer(conn
		,(const char *)&WEBIFMenu
		,sizeof(WEBIFMenu));
	}

	break;

//--------------------------------------------------------------------------------------------------

	case '_':				// '_' -> Device WEB-IF: Footer

	{

	static const char WEBIFFooter[] ICACHE_RODATA_ATTR =

	"<div id=\"fuss\">"
	"<a href=\"#\">Impressum</a> ::: "
	"<a href=\"http://www.ecsuha.de/login\">Sign in</a> ::: "
	"<a href=\"http://www.ecsuha.de/customer/info\">ME@EcSUHA.de</a> ::: "
	"<a href=\"http://www.ecsuha.de/privatemessages\">My Messages</a> ::: "
	"<a href=\"http://www.ecsuha.de/wishlist\">My Wishlist</a>"
	"</div>";

	BytesFree = HTTPD_Send_To_Send_Buffer(conn,
		(const char *)&WEBIFFooter,
		sizeof(WEBIFFooter));
	}

	break;

//--------------------------------------------------------------------------------------------------

	default:				// Not found? Zero terminate!
	break;

//--------------------------------------------------------------------------------------------------

	}

  return BytesFree;
}



