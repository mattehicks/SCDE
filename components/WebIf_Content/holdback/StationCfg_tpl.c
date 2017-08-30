//##################################################################################################
//###  Function: Callback for Station-Configuration (tpl resource)
//###  ESP 8266EX SOC Activities ...
//###  (C) EcSUHA
//###  Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany
//###  MSchulze780@GMAIL.COM
//###  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
//##################################################################################################
#include "ProjectConfig.h"



#include "c_types.h"
#include <string.h>
#include <osapi.h>
#include "user_interface.h"
#include "mem.h"
#include "SCDED.h"
//#include "CGI_StdUi.h"
#include "CGI_WiFi.h"
#include "espmissingincludes.h"
#include "SCDE.h"
//#include "CGI_TiStCfg.h"

#include "CGI_NoAuthErr.h"
#include "CGI_NotFoundErr.h"



#include "StationCfg_tpl.h"



/*
 *--------------------------------------------------------------------------------------------------
 *FName: StationCfg_tpl
 * Desc: Template %X% replacement Code for the StationCfg.htm Page
 * Info: WARNING: CHECK BUFFERSIZE
 * Para: WebIF_HTTPDConnSlotData_t *conn -> ptr to connection slot
 *       char *token -> ptr to string which contains the token to replace
 *       void **arg -> ???
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR 
StationCfg_tpl(WebIF_HTTPDConnSlotData_t *conn, char *token, void **arg) 
{
  char buff[1200];
  if (token==NULL) return;

  switch(token[0])
	{

//--------------------------------------------------------------------------------------------------

	case '?':				// '?' -> Device Name requested!
	{
	// get Devicename from current Service AP settings
	struct softap_config softap_config;
	wifi_softap_get_config (&softap_config);

	os_strcpy(buff,
		(char*) &softap_config.ssid);
	}
	break;

//--------------------------------------------------------------------------------------------------

	case 'A':				// 'A'  -> current rssi
	os_sprintf(buff,
		"%d",
		wifi_station_get_rssi());
	break;

//--------------------------------------------------------------------------------------------------

	case 'B':				// 'B' -> 
	break;

//--------------------------------------------------------------------------------------------------

	case 'C':				// 'C*' -> STATION_IF -> IP Adress
	{
  	struct ip_info ip_info;

  	wifi_get_ip_info(STATION_IF, &ip_info);


	switch(token[1])
		{

		case '1':				// 'C1'  -> IP-Adresse Oktett 1
		os_sprintf(buff,
			"%03d",
			ip_info.ip.addr & 0xff);
		break;

		case '2':				// 'C2'  -> IP-Adresse Oktett 2
		os_sprintf(buff,
			"%03d",
			(ip_info.ip.addr >> 8) & 0xff);
		break;

		case '3':				// 'C3'  -> IP-Adresse Oktett 3
		os_sprintf(buff,
			"%03d",
			(ip_info.ip.addr >> 16) & 0xff);
		break;

		case '4':				// 'C4'  -> IP-Adresse Oktett 4
		os_sprintf(buff,
			"%03d",
			ip_info.ip.addr >> 24);
		break;

		default:				// Not found? Zero terminate!
		buff[0] = 0;
		break;
		}
	}
	break;

//--------------------------------------------------------------------------------------------------

	case 'D':				// 'D*' -> STATION_IF -> Netmask Adress
	{
  	struct ip_info ip_info;

  	wifi_get_ip_info(STATION_IF, &ip_info);

	
	switch(token[1])
		{

		case '1':				// 'D1'  -> Netmask Oktett 1
		os_sprintf(buff,
			"%03d",
			ip_info.netmask.addr & 0xff);
		break;

		case '2':				// 'D2'  -> Netmask Oktett 2
		os_sprintf(buff,
			"%03d",
			(ip_info.netmask.addr >> 8) & 0xff);
		break;

		case '3':				// 'D3'  -> Netmask Oktett 3
		os_sprintf(buff,
			"%03d",
			(ip_info.netmask.addr >> 16) & 0xff);
		break;

		case '4':				// 'D4'  -> Netmask Oktett 4
		os_sprintf(buff,
			"%03d",
			ip_info.netmask.addr >> 24);
		break;

		default:				// Not found? Zero terminate!
		buff[0] = 0;
		break;
		}
	}
	break;

//--------------------------------------------------------------------------------------------------

	case 'E':				// 'E*' -> STATION_IF -> Gateway Adress
	{
  	struct ip_info ip_info;

  	wifi_get_ip_info(STATION_IF, &ip_info);

	switch(token[1])
		{

		case '1':				// 'E1'  -> Gateway Oktett 1
		os_sprintf(buff,
			"%03d",
			ip_info.gw.addr & 0xff);
		break;

		case '2':				// 'E2'  -> Gateway Oktett 2
		os_sprintf(buff,
			"%03d",
			(ip_info.gw.addr >> 8) & 0xff);
		break;

		case '3':				// 'E3'  -> Gateway Oktett 3
		os_sprintf(buff,
			"%03d",
			(ip_info.gw.addr >> 16) & 0xff);
		break;

		case '4':				// 'E4'  -> Gateway Oktett 4
		os_sprintf(buff,
			"%03d",
			ip_info.gw.addr >> 24);
		break;

		default:				// Not found? Zero terminate!
		buff[0] = 0;
		break;
		}
	}
	break;

//--------------------------------------------------------------------------------------------------

	case 'F':				// 'F*' -> STATION_IF -> Mac Adress
	{
 	uint8 macaddr[8];

//	os_memcpy(&macaddr,&SysCfgRamFlashMirrored->Stamacaddr,8);	// woher nehmen?
  	wifi_get_macaddr(STATION_IF, (uint8 *) &macaddr);
	
	switch(token[1])
		{

		case '1':				// 'F1'  -> MAC Oktett 1
		os_sprintf(buff,
			"%02X",
			macaddr[0]);
		break;

		case '2':				// 'F2'  -> MAC Oktett 2
		os_sprintf(buff,
			"%02X",
			macaddr[1]);
		break;

		case '3':				// 'F3'  -> MAC Oktett 3
		os_sprintf(buff,
			"%02X",
			macaddr[2]);
		break;

		case '4':				// 'F4'  -> MAC Oktett 4
		os_sprintf(buff,
			"%02X",
			macaddr[3]);
		break;

		case '5':				// 'F5'  -> MAC Oktett 5
		os_sprintf(buff,
			"%02X",
			macaddr[4]);
		break;

		case '6':				// 'F6'  -> MAC Oktett 6
		os_sprintf(buff,
			"%02X",
			 macaddr[5]);
		break;

		default:				// Not found? Zero terminate!
		buff[0] = 0;
		break;
		}
	}
	break;

//--------------------------------------------------------------------------------------------------

	case 'G':				// 'G' -> List Stations recorded in ESP (0-4)
	  {					// and make Radio Selection Menu, make Station Ap Change
	// get actual AP-Record No (0-4)!
	uint8 AkAPRecord = wifi_station_get_current_ap_id();

	// get current recorded stations (0-4 = max 5)
	struct station_config station_config[5];

	// get number of AP-Records
	uint8 StaRecords = wifi_station_get_ap_info(station_config);

	// may be we have no records
	if (StaRecords == 0)
		{
		os_sprintf(buff,"<h2>Actually no Wireless Station is stored ...</h2>");
		}

	// yes we have records
	else
		{
		int len=0;
		int i=0;

		// create Form
		len += os_sprintf(buff+len,
			"<h2><form method=\"post\" action=\"StationCfg.cgi\">\n");

		// loop all records ..
  		while (i != StaRecords)
			{
			len += os_sprintf(buff+len,
				"<input type=\"radio\" name=\"XE\" value=\"%d\" ",
				i);
			if (i == AkAPRecord) len += os_sprintf(buff+len,
				" checked=\"checked\"");	// Checked?
			len += os_sprintf(buff+len,
				" onChange=\"this.form.submit()\"");	// Auto Transmit Form

			len += os_sprintf(buff+len,
				">Slot %d - SSID: %s - BSSID: "
				MACSTR
				"<br>\n",
				i+1,
				station_config[i].ssid,
				MAC2STR(station_config[i].bssid));
			i++;
			}

		// end created Form
		len += os_sprintf(buff+len, "</form></h2>\n");
		}
	}
	break;

//--------------------------------------------------------------------------------------------------

	case 'H':				// 'H' -> Enable / Disable Soft AP switch -> X2
	RadioA((char*) &buff,
		DisEn,
		(wifi_get_opmode() & 0b01),
		"X2",
		"StationCfg.cgi");
	break;

//--------------------------------------------------------------------------------------------------

	case 'I':				// 'I' -> Enable / Disable DHCP Client -> X8
	RadioA((char*) &buff,
		DisEn,
		wifi_station_dhcpc_status(),
		"X8","StationCfg.cgi");
	break;

//--------------------------------------------------------------------------------------------------

	case 'J':				// 'J' -> Enable / Disable Auto Connect -> X9
	RadioA((char*) &buff,
		DisEn,
		wifi_station_get_auto_connect(),
		"X9","StationCfg.cgi");
	break;

//--------------------------------------------------------------------------------------------------

	case 'K':				// 'K' -> Wifi Physical Mode Selection -> XA
	RadioA((char*) &buff,
		PhMode,
		wifi_get_phy_mode(),
		"XA","StationCfg.cgi");
	break;

//--------------------------------------------------------------------------------------------------

	case 'L':				// 'L' -> Display Actual Connection Status
	os_sprintf(buff,
		"%d",
		wifi_station_get_connect_status());
	break;

//--------------------------------------------------------------------------------------------------

	case 'M':				// 'M' -> Display Actual Access Point (record in Use)
	os_sprintf(buff,
		"%d",
		 wifi_station_get_current_ap_id() +1 );
	break;

//--------------------------------------------------------------------------------------------------

	case 'N':				// 'N' -> Number of Accesspoints stored Selection ->XB
/*	{
	// get current recorded stations (0-4 = max 5)
	struct station_config station_config[5];

	uint8 StaRecords = wifi_station_get_ap_info(station_config);


	SelectA((char*) &buff,
		NrAP,
		StaRecords,
		"XB",
		"StationCfg.cgi");
	}*/
	break;

//--------------------------------------------------------------------------------------------------

	case 'O':				// 'O' -> 'O*' -> active station_config
	{
	struct station_config station_config;

	wifi_station_get_config(&station_config);
	
	switch(token[1])
		{

		case '1':				// 'O1'  -> SSID / aka Devicename
		os_strcpy(buff,
			(char*)station_config.ssid);
		break;

		case '2':				// 'O2'  -> Password
		os_strcpy(buff,
			(char*)station_config.password);
		break;

//!!!!!!!!!!!!!!!!!!!!!

		case '3':				// 'O3'  -> Ena/Disa bssid
		os_strcpy(buff,
			(char*)station_config.password);
		break;

//!!!!!!!!!!!!!!!!!!!!!
		case '4':				// 'O4'  -> bssid Oktett 1
		os_sprintf(buff,
			"%02X",
			station_config.bssid[0]);
		break;

		case '5':				// 'O5'  -> bssid Oktett 2
		os_sprintf(buff,
			"%02X",
			station_config.bssid[1]);

		case '6':				// 'O6'  -> bssid Oktett 3
		os_sprintf(buff,
			"%02X",
			station_config.bssid[2]);
		break;

		case '7':				// 'O7'  -> bssid Oktett 4
		os_sprintf(buff,
			"%02X",
			station_config.bssid[3]);
		break;

		case '8':				// 'O8'  -> bssid Oktett 5
		os_sprintf(buff,
			"%02X",
			station_config.bssid[4]);
		break;

		case '9':				// 'O9'  -> bssid Oktett 6
		os_sprintf(buff,
			"%02X",
			station_config.bssid[5]);
		break;

		default:				// Not found? Zero terminate!
		buff[0] = 0;
		break;
		}
	}
	break;

//--------------------------------------------------------------------------------------------------

	default:				// Not found? Zero terminate!
	buff[0] = 0;
	break;

//--------------------------------------------------------------------------------------------------
	}

  SCDED_Send(conn, buff, -1);
}



