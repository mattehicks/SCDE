//##################################################################################################
//###  Function: Callback for System-on-Chip Hardware Configuration (jso resource)
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
#include "sntp.h"
#include "SCDED.h"
#include "espmissingincludes.h"
#include "SCDE.h"
#include "ip_addr.h"
#include "CGI_WiFi.h"

//#include "CGI_TiStCfg.h"
#include "CGI_NoAuthErr.h"
#include "CGI_NotFoundErr.h"



#include "SoCHWCfg_jso.h"



//##################################################################################################
//### FName: SoCHWCfg_jso
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
// 04-29-> free
// 30   -> "jso Lead Out"
// 31   -> "BWS assigned"
/*# Data Sample for one Switch.X.jso
{"STATUS":{"MEMORY":"21456","TIME":"1474703052"}
,{"WSAP":{
"name":"xxxx","WSAP_Password":"EcSUHAde","WSAP_RF_Channel":"1","WSAP_Maximal_Connections":"4","WSAP_Authentication_Method":"WPA_WPA2_PSK","WSAP_SSID":"Visible","WSAP_Beacon_Interval":"100","WSAP_IP_Adress":"000.000.000.000","WSAP_Netmask":"000.000.000.000","WSAP_Gateway_Adress":"000.000.000.000","WSAP_DHCPS":"Enabled","WSAP_DHCPS_Lease_Start_IP":"000.000.000.000","WSAP_DHCPS_Lease_End_IP":"000.000.000.000","WSAP_MAC_Adress":"1A.FE.34.FE.4A.11.00.00","WSAP":"Disabled"]
}
}


{"STATUS":{"MEMORY":"21456","TIME":"1474703052",
"SwITCH":[
"name":"xxxx","WSAP_Password":"EcSUHAde","WSAP_RF_Channel":"1","WSAP_Maximal_Connections":"4","WSAP_Authentication_Method":"WPA_WPA2_PSK","WSAP_SSID":"Visible","WSAP_Beacon_Interval":"100","WSAP_IP_Adress":"000.000.000.000","WSAP_Netmask":"000.000.000.000","WSAP_Gateway_Adress":"000.000.000.000","WSAP_DHCPS":"Enabled","WSAP_DHCPS_Lease_Start_IP":"000.000.000.000","WSAP_DHCPS_Lease_End_IP":"000.000.000.000","WSAP_MAC_Adress":"1A.FE.34.FE.4A.11.00.00","WSAP":"Disabled"]
}
}
*/

int ICACHE_FLASH_ATTR 
SoCHWCfg_jso(WebIF_HTTPDConnSlotData_t *conn) 
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
		cmpTIST = SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].IB01_X_TiSt;
		if (FeatTIST < cmpTIST)
			FeatTIST = cmpTIST;
		// TiSt 
		cmpTIST = SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].IB02_X_TiSt;
		if (FeatTIST < cmpTIST)
			FeatTIST = cmpTIST;
		// TiSt 
		cmpTIST = SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].IB03_X_TiSt;
		if (FeatTIST < cmpTIST)
			FeatTIST = cmpTIST;
		// TiSt 
		cmpTIST = SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].IB04_X_TiSt;
		if (FeatTIST < cmpTIST)
			FeatTIST = cmpTIST;
		// TiSt 
		cmpTIST = SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].IB05_X_TiSt;
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
	BWS = 0b11000000000000000000000000001111;

	}

// Create Answer according to Buffer-Write-Strategy to save ram-memory
  int len;
  char buff[512];

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

	struct rst_info *rst_i;

	// get reset information
	rst_i = system_get_rst_info();

	len = os_sprintf(buff, "{\"STATUS\":{\"Mem\":\"%d\",\"TiSt\":\"%u\",\"SDK\":\"%s\",\"Chip_ID\":\"%X\",\"Boot\":\"V 1.%d\",\"FW\":\"V %u.%u, Build %u\",\"Boot_Partition\":\"%X\",\"Flash_Chip_ID\":\"%X\",\"Uptime\":\"%d\",\"a\":\"%d\",\"b\":\"%d\",\"s\":\"%d\",\"Reset_Info\":\"reason: %08X - exccause: %08X, epc1: %08X, epc2: %08X, epc3: %08X, excvaddr: %08X, depc: %08X\"},\n\"SoCHWCfg\":{"
		,system_get_free_heap_size()
		,GetTIST()

		,system_get_sdk_version()
		,system_get_chip_id()
		,system_get_boot_version()

		,(unsigned int) SysCfgRamFlashMirrored->Version_major
		,(unsigned int) SysCfgRamFlashMirrored->Version_minor
		,(unsigned int) SysCfgRamFlashMirrored->build

		,system_upgrade_userbin_check()
		,spi_flash_get_id()

		, 666

		,user_rf_cal_sector_set()
		,SCDE_GetSysCfgRamFlashMirroredSec()
		,wifi_fpm_get_sleep_type()
		,rst_i->reason
		,rst_i->exccause
		,rst_i->epc1
		,rst_i->epc2
		,rst_i->epc3
		,rst_i->excvaddr
		,rst_i->depc

		);

	if (!SCDED_Send(conn, buff, len)) return HTTPD_CGI_PROCESS_CONN;

	}

//---------------------------------------------------------------------------------------------------
  // do "jso Main Data Rows" - we need the Remote TiSt!
/*
	int32 RmtTIST;
	if (HTTPDFindValueDecInt32FromKey(conn->postBuff,
		"TIST", &RmtTIST))
  os_printf("tmp Rmt.TIST:%d\n",RmtTIST);
	else RmtTIST = -1;


  // do "jso Main Data -> IB01_X_TiSt" ?
  if (BWS & 1<<5)

	{

	BWS &= ~(1<<5);

	// DO TIST Output Check, output row if older, or no TIST...
	if (RmtTIST == -1 ||
		RmtTIST < SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].IB01_X_TiSt)

		// TIST-Check result: Output this ROW
		{

		// get current Service AP settings
		struct softap_config softap_config;

		wifi_softap_get_config (&softap_config);

		len = os_sprintf(buff,
			"%s"	// first row - no comma
			 "\"name\":\"%s\""
			",\"WSAP_Password\":\"%s\""
			",\"WSAP_RF_Channel\":\"%u\""
			",\"WSAP_Maximal_Connections\":\"%u\""
			",\"WSAP_Authentication_Method\":\"%s\""
			",\"WSAP_SSID\":\"%s\""
			",\"WSAP_Beacon_Interval\":\"%u\""
			,(BWS & 1<<4)?"":","
			,(char*) &softap_config.ssid
			,(char*) &softap_config.password
			,softap_config.channel
			,softap_config.max_connection
			,SCDE_GetDesc(Auth_M,softap_config.authmode)
			,SCDE_GetDesc(SSID_H,softap_config.ssid_hidden)
			,softap_config.beacon_interval);

		// delete "first row - no comma" flag
		BWS &= ~(1<<4);

		// send row
		if (!SCDED_Send(conn, buff, len)) return HTTPD_CGI_PROCESS_CONN;

		}
	}

  // do "jso Main Data -> IB02_X_TiSt" ?
  if (BWS & 1<<6)

	{

	BWS &= ~(1<<6);

	// DO TIST Output Check, output row if older, or no TIST...
	if (RmtTIST == -1 ||
		RmtTIST < SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].IB02_X_TiSt)

		// TIST-Check result: Output this ROW
		{

		// get current Service AP IP-Settings
		struct ip_info AP_ip_info;

		wifi_get_ip_info(SOFTAP_IF, &AP_ip_info);

		len = os_sprintf(buff,
			"%s"	// first row - no comma
			 "\"WSAP_IP_Adress\":\"%03d.%03d.%03d.%03d\""
			",\"WSAP_Netmask\":\"%03d.%03d.%03d.%03d\""
			",\"WSAP_Gateway_Adress\":\"%03d.%03d.%03d.%03d\""
			,(BWS & 1<<4)?"":","
			,AP_ip_info.ip.addr & 0xff
			,(AP_ip_info.ip.addr >> 8) & 0xff
			,(AP_ip_info.ip.addr >> 16) & 0xff
			,AP_ip_info.ip.addr >> 24
			,AP_ip_info.netmask.addr & 0xff
			,(AP_ip_info.netmask.addr >> 8) & 0xff
			,(AP_ip_info.netmask.addr >> 16) & 0xff
			,AP_ip_info.netmask.addr >> 24
			,AP_ip_info.gw.addr & 0xff
			,(AP_ip_info.gw.addr >> 8) & 0xff
			,(AP_ip_info.gw.addr >> 16) & 0xff
			,AP_ip_info.gw.addr >> 24);

		// delete "first row - no comma" flag
		BWS &= ~(1<<4);

		// send row
		if (!SCDED_Send(conn, buff, len)) return HTTPD_CGI_PROCESS_CONN;

		}
	}

  // do "jso Main Data -> IB03_X_TiSt" ?
  if (BWS & 1<<7)

	{

	BWS &= ~(1<<7);

	// DO TIST Output Check, output row if older, or no TIST...
	if (RmtTIST == -1 ||
		RmtTIST < SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].IB03_X_TiSt)

		// TIST-Check result: Output this ROW
		{

		// get current Service AP DHCP-Settings
		struct dhcps_lease dhcps_lease;

		wifi_softap_get_dhcps_lease(&dhcps_lease);

		int WifiSoftAPDHCPSStatus = wifi_softap_dhcps_status();

		len = os_sprintf(buff,
			"%s"	// first row - no comma
			 "\"WSAP_DHCPS\":\"%s\""
			",\"WSAP_DHCPS_Lease_Start_IP\":\"%03d.%03d.%03d.%03d\""
			",\"WSAP_DHCPS_Lease_End_IP\":\"%03d.%03d.%03d.%03d\""
			,(BWS & 1<<4)?"":","
			,SCDE_GetDesc(DisEn, WifiSoftAPDHCPSStatus)
			, dhcps_lease.start_ip.addr & 0xff
			,(dhcps_lease.start_ip.addr >> 8) & 0xff
			,(dhcps_lease.start_ip.addr >> 16) & 0xff
			, dhcps_lease.start_ip.addr >> 24
			, dhcps_lease.end_ip.addr & 0xff
			,(dhcps_lease.end_ip.addr >> 8) & 0xff
			,(dhcps_lease.end_ip.addr >> 16) & 0xff
			, dhcps_lease.end_ip.addr >> 24);

		// delete "first row - no comma" flag
		BWS &= ~(1<<4);

		// send row
		if (!SCDED_Send(conn, buff, len)) return HTTPD_CGI_PROCESS_CONN;

		}
	}

  // do "jso Main Data -> IB04_X_TiSt" ?
  if (BWS & 1<<8)

	{

	BWS &= ~(1<<8);

	// DO TIST Output Check, output row if older, or no TIST...
	if (RmtTIST == -1 ||
		RmtTIST < SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].IB04_X_TiSt)

		// TIST-Check result: Output this ROW
		{

 		// get current Service AP MAC Adress
		uint8 WifiSoftAPMACAddr[8];

		wifi_get_macaddr(SOFTAP_IF, (uint8 *) &WifiSoftAPMACAddr);

		len = os_sprintf(buff,
			"%s"	// first row - no comma
			 "\"WSAP_MAC_Adress\":\"%02X.%02X.%02X.%02X.%02X.%02X.%02X.%02X\""
			,(BWS & 1<<4)?"":","
			,WifiSoftAPMACAddr[0]
			,WifiSoftAPMACAddr[1]
			,WifiSoftAPMACAddr[2]
			,WifiSoftAPMACAddr[3]
			,WifiSoftAPMACAddr[4]
			,WifiSoftAPMACAddr[5]
			,WifiSoftAPMACAddr[6]
			,WifiSoftAPMACAddr[7]);

		// delete "first row - no comma" flag
		BWS &= ~(1<<4);

		// send row
		if (!SCDED_Send(conn, buff, len)) return HTTPD_CGI_PROCESS_CONN;

		}
	}

  // do "jso Main Data -> IB05_X_TiSt" ?
  if (BWS & 1<<9)

	{

	BWS &= ~(1<<9);

	// DO TIST Output Check, output row if older, or no TIST...
	if (RmtTIST == -1 ||
		RmtTIST < SysCfgRamNoMirror->MySCDE_FeatCfgRamNoMirror[0].IB05_X_TiSt)

		// TIST-Check result: Output this ROW
		{

		// get current Service AP Operating Mode
		int WifiApOpMode = (wifi_get_opmode() >> 1);

		len = os_sprintf(buff,
			"%s"	// first row - no comma
			 "\"WSAP\":\"%s\""
			,(BWS & 1<<4)?"":","
			,SCDE_GetDesc(DisEn, WifiApOpMode) );

		// delete "first row - no comma" flag
		BWS &= ~(1<<4);

		// send row
		if (!SCDED_Send(conn, buff, len)) return HTTPD_CGI_PROCESS_CONN;

		}
	}
*/
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




