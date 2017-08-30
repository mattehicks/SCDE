//##################################################################################################
//###  Function: Callback for Station-Configuration (jso resource)
//###  ESP 8266EX SOC Activities ...
//###  (C) EcSUHA
//###  Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany
//###  MSchulze780@GMAIL.COM
//###  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
//##################################################################################################
#include "ProjectConfig.h"
#include <esp8266.h>


/*
#include "c_types.h"
#include <string.h>
#include <osapi.h>
#include "user_interface.h"
#include "mem.h"
#include "sntp.h"
#include "SCDED.h"
#include "espmissingincludes.h"
#include "SCDE.h"
#include "ip_addr.h"
*/

//#include "CGI_TiStCfg.h"
#include "CGI_NoAuthErr.h"
#include "CGI_NotFoundErr.h"

//#include "CGI_WiFi.h"



#include "StationCfg_jso.h"



//##################################################################################################
//### FName: StationCfg_jso
//###  Desc: Creates DATA for client side Java Script and sends it (JSON)
//###        Req.Args: valid "conn->ActiveDirID" or args "fSwITCH + lSwITCH"
//###        no tist = full output!!! else:
//###        1st "ALL TIST CHECK" to detect anychange -> if not = Longpoll
//###        -> if yes output, but changed (NEWER TIST) only
//###  Para: PTR to WebIF_HTTPDConnSlotData_t 
//###  Rets: -/-
//##################################################################################################
// Buffer-Write-Strategy defined by BIT-SET stored in conn->cgiData (initially zero):
// 00	-> HTTPDStartResponse
// 01   -> HTTPDHeader
// 02   -> HTTPDEndHeaders
// 03   -> "jso Lead In"
// 04   -> "first row - no comma" flag
// 05   -> data for IB06_X_TiSt
// 06   -> data for IB07_X_TiSt
// 07   -> data for IB08_X_TiSt
// 08   -> data for IB09_X_TiSt
// 09   -> data for IB10_X_TiSt
// 10   -> data for IB11_X_TiSt
// 11   -> data for RB01_X_TiSt
// 12-29-> free
// 30   -> "jso Lead Out"
// 31   -> "BWS assigned"
/*# Data Sample for one Switch.X.jso
{"STATUS":{"MEMORY":"21760","TIME":"1474674752",
"SwITCH":[
"Station_IP_Adress":"192.168.000.068","Station_Netmask":"255.255.255.000","Station_Gateway_Adress":"192.168.000.001","Station":"Enabled","Station_DHCP":"Enabled","Station_Auto_Connect":"Enabled","Station_Physical_Mode":"802.11n","Station_MAC_Adress":"18.FE.34.FE.4A.11.FF.00","Station_RSSI":"-78"]
}
}
*/

int ICACHE_FLASH_ATTR 
StationCfg_jso(WebIF_HTTPDConnSlotData_t *conn) 
{
   // Connection aborted? Nothing to clean up.
  if (conn->conn == NULL)
	return HTTPD_CGI_DISCONNECT_CONN;

//---------------------------------------------------------------------------------------------------

  // Check if Parser is ready
  if (conn->SlotParserState != s_HTTPD_Msg_Prsd_Complete)
	return HTTPD_CGI_WAITRX_CONN;

//---------------------------------------------------------------------------------------------------

  // Check for valid authorization (AUTH_GENERIC_RESSOURCE)
  if (SCDED_AuthCheck(conn, AUTH_GENERIC_RESSOURCE))

	{

	// set callback for "Not Authorized Error"
	conn->cgi = NoAuthErr_cgi;

	return HTTPD_CGI_REEXECUTE;

	}	

//---------------------------------------------------------------------------------------------------	

  char *Args;

  if (conn->parser_method == HTTP_GET)		// GET Method ?

	{

	Args = conn->getArgs;

	}

  else //if (conn->parser_method == HTTP_POST)	// POST Method ?

	{

	// all post data buffered? If not continue receiving all post data to buffer ...
	if (conn->parser_content_length) return HTTPD_CGI_PROCESS_CONN;

	Args = conn->postBuff;

	}

//--------------------------------------------------------------------------------------------------

  # if SwITCH_DBG >= 3
  if (Args == NULL)
	{
	os_printf(".jso->NO Args!");
	}
  else
	{
	os_printf(".jso->Args:%s\n",Args);
	}
  # endif

//--------------------------------------------------------------------------------------------------

  // Storage for BufferWriteStrategy
  int BWS = (int ) conn->PCData;

  // fresh connection without Buffer-Write-Strategy ?
  if (!BWS)	//  if (!conn->cgiData)
	{

	# if SwITCH_DBG >= 3
	os_printf("BWS,Start\n");
	# endif

	// Check if remote sent "TIST" timestamp, so that we can do longpoll in case of no new values
	int32 RmtTIST = -1;
	if (HTTPDFindValueDecInt32FromKey(Args,
		"TIST", &RmtTIST))

		{

		// find oldest Timestamp from Features to check for longpoll
		int32 FeatTIST = 0;
		int32 cmpTIST;

		// TiSt 
		cmpTIST = SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].IB06_X_TiSt;
		if (FeatTIST < cmpTIST)
			FeatTIST = cmpTIST;

		// TiSt 
		cmpTIST = SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].IB07_X_TiSt;
		if (FeatTIST < cmpTIST)
			FeatTIST = cmpTIST;

		// TiSt 
		cmpTIST = SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].IB08_X_TiSt;
		if (FeatTIST < cmpTIST)
			FeatTIST = cmpTIST;

		// TiSt 
		cmpTIST = SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].IB09_X_TiSt;
		if (FeatTIST < cmpTIST)
			FeatTIST = cmpTIST;

		// TiSt 
		cmpTIST = SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].IB10_X_TiSt;
		if (FeatTIST < cmpTIST)
			FeatTIST = cmpTIST;

		// TiSt 
		cmpTIST = SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].IB11_X_TiSt;
		if (FeatTIST < cmpTIST)
			FeatTIST = cmpTIST;

		// TiSt 
		cmpTIST = SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].RB01_X_TiSt;
		if (FeatTIST < cmpTIST)
			FeatTIST = cmpTIST;


		// check if remote data is complete up to date, if yes do longpoll ... else renew specific or all rows
		if (FeatTIST <= RmtTIST)

			{

			#if SwITCH_DBG >= 4
			os_printf("Rmt.TIST:%d, Feat.TIST:%d, Real TIST: %d, LP!->%d Sec\n",
				RmtTIST,
				FeatTIST,
				GetTIST(),
				conn->LP10HzCounter);
			#endif

			// do longpoll, limit time (30 sec)
			if (conn->LP10HzCounter < 30*10)

				{

				conn->LP10HzCounter++;

				return HTTPD_CGI_PROCESS_CONN;

				}
			}
		}

	// define Buffer-Write-Strategy (BWS) for answer
	BWS = 0b11000000000000000000111111111111;

	}

// Create Answer according to Buffer-Write-Strategy to save ram-memory
  int len;
  char buff[256];

//---------------------------------------------------------------------------------------------------

  // do HTTPDStartResponse ?
  if (BWS & 1<<0)

	{

	BWS &= ~(1<<0);

	//if (!HTTPDStartRespHeader(conn, 200)) return HTTPD_CGI_PROCESS_CONN;
	if (!SCDED_StartRespHeader2(conn)) return HTTPD_CGI_PROCESS_CONN;

	}

//---------------------------------------------------------------------------------------------------

  // do HTTPDHeader ?
  if (BWS & 1<<1)

	{

	BWS &= ~(1<<1);
	if (!SCDED_AddHdrFld(conn, "Content-Type", "text/json", -1)) return HTTPD_CGI_PROCESS_CONN;

	}

//---------------------------------------------------------------------------------------------------

  // do HTTPDEndHeaders ?
  if (BWS & 1<<2)

	{

	BWS &= ~(1<<2);
	if (!SCDED_EndHeader(conn)) return HTTPD_CGI_PROCESS_CONN;

	}

//---------------------------------------------------------------------------------------------------

  // do "jso Lead In" ?
  if (BWS & 1<<3)

	{

	BWS &= ~(1<<3);

	len = os_sprintf(buff, "{\"STATUS\":{\"Mem\":\"%d\",\"TiSt\":\"%u\"},\n\"StationCfg\":{"
		,system_get_free_heap_size()
		,GetTIST());

	if (!SCDED_Send(conn, buff, len)) return HTTPD_CGI_PROCESS_CONN;

	}

//---------------------------------------------------------------------------------------------------
  // do "jso Main Data Rows" - we need the Remote TiSt!

	int32 RmtTIST;
	if (HTTPDFindValueDecInt32FromKey(conn->postBuff,
		"TIST", &RmtTIST))
  os_printf("tmp Rmt.TIST:%d\n",RmtTIST);
	else RmtTIST = -1;


  // do "jso Main Data -> IB06_X_TiSt" ?
  if (BWS & 1<<5)

	{

	BWS &= ~(1<<5);

	// DO TIST Output Check, output row if older, or no TIST...
	if (RmtTIST == -1 ||
		RmtTIST < SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].IB06_X_TiSt)

		// TIST-Check result: Output this ROW
		{

		// get current Station IP Settings
		struct ip_info STA_ip_info;

		wifi_get_ip_info(STATION_IF, &STA_ip_info);

		len = os_sprintf(buff,
			"%s"	// first row - no comma
			 "\"Station_IP_Adress\":\"%03d.%03d.%03d.%03d\""
			",\"Station_Netmask\":\"%03d.%03d.%03d.%03d\""
			",\"Station_Gateway_Adress\":\"%03d.%03d.%03d.%03d\""
			,(BWS & 1<<4)?"":","
			,STA_ip_info.ip.addr & 0xff
			,(STA_ip_info.ip.addr >> 8) & 0xff
			,(STA_ip_info.ip.addr >> 16) & 0xff
			,STA_ip_info.ip.addr >> 24
			,STA_ip_info.netmask.addr & 0xff
			,(STA_ip_info.netmask.addr >> 8) & 0xff
			,(STA_ip_info.netmask.addr >> 16) & 0xff
			,STA_ip_info.netmask.addr >> 24
			,STA_ip_info.gw.addr & 0xff
			,(STA_ip_info.gw.addr >> 8) & 0xff
			,(STA_ip_info.gw.addr >> 16) & 0xff
			,STA_ip_info.gw.addr >> 24);

		// delete "first row - no comma" flag
		BWS &= ~(1<<4);

		// send row
		if (!SCDED_Send(conn, buff, len)) return HTTPD_CGI_PROCESS_CONN;

		}
	}

  // do "jso Main Data -> IB07_X_TiSt" ?
  if (BWS & 1<<6)

	{

	BWS &= ~(1<<6);

	// DO TIST Output Check, output row if older, or no TIST...
	if (RmtTIST == -1 ||
		RmtTIST < SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].IB07_X_TiSt)

		// TIST-Check result: Output this ROW
		{

		// get current Station Operating Mode
		int WifiStaOpMode = (wifi_get_opmode() & 0b01);

		len = os_sprintf(buff,
			"%s"	// first row - no comma
			 "\"Station\":\"%s\""
			,(BWS & 1<<4)?"":","
			,SCDE_GetDesc(DisEn, WifiStaOpMode) );

		// delete "first row - no comma" flag
		BWS &= ~(1<<4);

		// send row
		if (!SCDED_Send(conn, buff, len)) return HTTPD_CGI_PROCESS_CONN;

		}
	}

  // do "jso Main Data -> IB08_X_TiSt" ?
  if (BWS & 1<<7)

	{

	BWS &= ~(1<<7);

	// DO TIST Output Check, output row if older, or no TIST...
	if (RmtTIST == -1 ||
		RmtTIST < SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].IB08_X_TiSt)

		// TIST-Check result: Output this ROW
		{

		// get current Station DHCP-Status
		int WifiStationDHCPStatus = wifi_station_dhcpc_status();

		len = os_sprintf(buff,
			"%s"	// first row - no comma
			 "\"Station_DHCP\":\"%s\""
			,(BWS & 1<<4)?"":","
			,SCDE_GetDesc(DisEn, WifiStationDHCPStatus));

		// delete "first row - no comma" flag
		BWS &= ~(1<<4);

		// send row
		if (!SCDED_Send(conn, buff, len)) return HTTPD_CGI_PROCESS_CONN;

		}
	}

  // do "jso Main Data -> IB09_X_TiSt" ?
  if (BWS & 1<<8)

	{

	BWS &= ~(1<<8);

	// DO TIST Output Check, output row if older, or no TIST...
	if (RmtTIST == -1 ||
		RmtTIST < SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].IB09_X_TiSt)

		// TIST-Check result: Output this ROW
		{

		// get current Station Auto-Connect-Status
		int WifiStationAutoConnectStatus = wifi_station_get_auto_connect();

		len = os_sprintf(buff,
			"%s"	// first row - no comma
			 "\"Station_Auto_Connect\":\"%s\""
			,(BWS & 1<<4)?"":","
			,SCDE_GetDesc(DisEn, WifiStationAutoConnectStatus));

		// delete "first row - no comma" flag
		BWS &= ~(1<<4);

		// send row
		if (!SCDED_Send(conn, buff, len)) return HTTPD_CGI_PROCESS_CONN;

		}
	}

  // do "jso Main Data -> IB10_X_TiSt" ?
  if (BWS & 1<<9)

	{

	BWS &= ~(1<<9);

	// DO TIST Output Check, output row if older, or no TIST...
	if (RmtTIST == -1 ||
		RmtTIST < SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].IB10_X_TiSt)

		// TIST-Check result: Output this ROW
		{

		// get current Station Physical Mode
		int WifiStationPhyMode = wifi_get_phy_mode();

		len = os_sprintf(buff,
			"%s"	// first row - no comma
			 "\"Station_Physical_Mode\":\"%s\""
			,(BWS & 1<<4)?"":","
			,SCDE_GetDesc(PhMode, WifiStationPhyMode));

		// delete "first row - no comma" flag
		BWS &= ~(1<<4);

		// send row
		if (!SCDED_Send(conn, buff, len)) return HTTPD_CGI_PROCESS_CONN;

		}
	}

  // do "jso Main Data -> IB11_X_TiSt" ?
  if (BWS & 1<<10)

	{

	BWS &= ~(1<<10);

	// DO TIST Output Check, output row if older, or no TIST...
	if (RmtTIST == -1 ||
		RmtTIST < SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].IB11_X_TiSt)

		// TIST-Check result: Output this ROW
		{

		// get current Station MAC Adress
 		uint8 WifiStationMACAddr[8];

	 	wifi_get_macaddr(STATION_IF, (uint8 *) &WifiStationMACAddr);

		len = os_sprintf(buff,
			"%s"	// first row - no comma
			 "\"Station_MAC_Adress\":\"%02X.%02X.%02X.%02X.%02X.%02X.%02X.%02X\""
			,(BWS & 1<<4)?"":","
			,WifiStationMACAddr[0]
			,WifiStationMACAddr[1]
			,WifiStationMACAddr[2]
			,WifiStationMACAddr[3]
			,WifiStationMACAddr[4]
			,WifiStationMACAddr[5]
			,WifiStationMACAddr[6]
			,WifiStationMACAddr[7]);

		// delete "first row - no comma" flag
		BWS &= ~(1<<4);

		// send row
		if (!SCDED_Send(conn, buff, len)) return HTTPD_CGI_PROCESS_CONN;

		}
	}

  // do "jso Main Data -> RB01_X_TiSt" ?
  if (BWS & 1<<11)

	{

	BWS &= ~(1<<11);

	// DO TIST Output Check, output row if older, or no TIST...
	if (RmtTIST == -1 ||
		RmtTIST < SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].RB01_X_TiSt)

		// TIST-Check result: Output this ROW
		{

		len = os_sprintf(buff,
			"%s"	// first row - no comma
			 "\"Station_RSSI\":\"%d\""
			,(BWS & 1<<4)?"":","
			,wifi_station_get_rssi());

		// delete "first row - no comma" flag
		BWS &= ~(1<<4);

		// send row
		if (!SCDED_Send(conn, buff, len)) return HTTPD_CGI_PROCESS_CONN;

		}
	}

//--------------------------------------------------------------------------------------------------

  // do "jso Lead Out" ?
  if (BWS & 1<<30)

	{

	BWS &= ~(1<<30);

	len = os_sprintf(buff, "}\n}");

	if (!SCDED_Send(conn, buff, len)) return HTTPD_CGI_PROCESS_CONN;

	}

//--------------------------------------------------------------------------------------------------

  return HTTPD_CGI_DISCONNECT_CONN;

  }
