//#################################################################################################
//###  Function: CGI for "not found error"
//###  ESP 8266EX SOC Activities ...
//###  (C) EcSUHA
//###  Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany 
//###  MSchulze780@GMAIL.COM
//###  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
//##################################################################################################
#include "ProjectConfig.h"
#include <esp8266.h>
#include "WebIf_Module.h"


/*
//#include "c_types.h"
#include <string.h>
#include <osapi.h>
#include "user_interface.h"
#include "mem.h"
#include "SCDED.h"
#include "CGI_SwITCH.h"
#include "espmissingincludes.h"
#include "SCDE.h"
#include "sntp.h"

#include "CGI_SCDEETX.h"*/

#include "CGI_NoAuthErr.h"
#include "CGI_NotFoundErr.h"
#include "CGI_Redirect.h"





//##################################################################################################
//### FName: cgiRedirect
//###  Desc: cgi to redirect to an other URL (Path char* is stored in conn->cgiArg)
//###  Para: HTTPDConnSlotData *ConnSlotData ->
//###  Rets: int -> recommended value HTTPD_CGI_DISCONNECT_CONN
//##################################################################################################
int ICACHE_FLASH_ATTR
cgiRedirect(WebIf_HTTPDConnSlotData_t *Conn) 
  {

  if (Conn->conn == NULL) {

	// Connection aborted. Clean up.
	return HTTPD_CGI_DISCONNECT_CONN;

	}

  // execute the redirection with procedure Callback arg
  SCDED_Redirect(Conn, (char*) Conn->PCArg);

  return HTTPD_CGI_DISCONNECT_CONN;

  }



//##################################################################################################
//### FName: cgiRedirectToHostname
//###  Desc: 
//###  Para: HTTPDConnSlotData *ConnSlotData ->
//###  Rets: value HTTPD_CGI_DISCONNECT_CONN
//##################################################################################################
//This CGI function redirects to a fixed url of http://[hostname]/ if hostname field of request isn't
//already that hostname. Use this in combination with a DNS server that redirects everything to the
//ESP in order to load a HTML page as soon as a phone, tablet etc connects to the ESP. Watch out:
//this will also redirect connections when the ESP is in STA mode, potentially to a hostname that is not
//in the 'official' DNS and so will fail.
int ICACHE_FLASH_ATTR
cgiRedirectToHostname(WebIf_HTTPDConnSlotData_t *conn)
  {

  static const char hostFmt[] ICACHE_RODATA_ATTR = "http://%s/";
  char *buff;
  int isIP = 0;
  int x;

  if (conn->conn == NULL) {

	// Connection aborted. Clean up.
	return HTTPD_CGI_DISCONNECT_CONN;

	}

  if (conn->hostName == NULL) {

	# if SCDED_DBG >= 3
	os_printf("Err!No Hostname\n");
	# endif

	// set callback for "Not Found Error"
	conn->cgi = NotFoundErr_cgi;
	return HTTPD_CGI_REEXECUTE;

	}

  // Quick and dirty code to see if host is an IP
  if (strlen(conn->hostName) > 8) {

	isIP = 1;

	for (x = 0; x < strlen(conn->hostName); x++) {

		if (conn->hostName[x] != '.' && (conn->hostName[x] < '0' || conn->hostName[x] > '9')) isIP = 0;

		}
	}

  if (isIP) {

	// set callback for "Not Found Error"
	conn->cgi = NotFoundErr_cgi;

	return HTTPD_CGI_REEXECUTE;

	}

  // Check hostname; pass on if the same
  if (strcmp(conn->hostName, (char*)conn->PCArg) == 0) {

	// set callback for "Not Found Error"
	conn->cgi = NotFoundErr_cgi;

	return HTTPD_CGI_REEXECUTE;

	}

  // Not the same. Redirect to real hostname.
  buff = malloc(strlen((char*)conn->PCArg)+sizeof(hostFmt));

  sprintf(buff, hostFmt, (char*)conn->PCArg);

  # if SCDED_DBG >= 3
  os_printf("Redir.to:%s\n",
		buff);
  # endif

  SCDED_Redirect(conn, buff);

  free(buff);

  return HTTPD_CGI_DISCONNECT_CONN;

  }



//##################################################################################################
//### FName: cgiRedirectApClientToHostname
//###  Desc: cgi to redirect only Access-Point clients (same netmask) to new Host Name
//###        Clients connected to the STA interface will not be redirected to nowhere.
//###  Para: HTTPDConnSlotData *ConnSlotData -> current connection slot
//###  Rets: value HTTPD_CGI_DISCONNECT_CONN
//##################################################################################################
int ICACHE_FLASH_ATTR
cgiRedirectApClientToHostname(WebIf_HTTPDConnSlotData_t *conn)
  {

  wifi_mode_t Mode;

  // Check if the softap interface is activated, else return not found
  esp_wifi_get_mode(&Mode);

  if (Mode != 2 && Mode != 3) {

	// set callback for "Not Found Error"
	conn->cgi = NotFoundErr_cgi;

	return HTTPD_CGI_REEXECUTE;

	}

  // Check if current connection goes to the softap interface, yes redirect, else return not found
  uint32_t *remadr;

  tcpip_adapter_ip_info_t ap_ip_info;
  tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP, &ap_ip_info);

  // get connections remote adress
  remadr = (uint32_t *) conn->conn->proto.tcp->remote_ip;

  // connection from AP -> redirect
  if ((*remadr & ap_ip_info.netmask.addr) == (ap_ip_info.ip.addr & ap_ip_info.netmask.addr)) {

	return cgiRedirectToHostname(conn);

	}

  // connection from STA -> Not found error
  else	{

	// set callback for "Not Found Error"
	conn->cgi = NotFoundErr_cgi;

	return HTTPD_CGI_REEXECUTE;

	}
  }


/* woanders?
//##################################################################################################
//### FName: HTTPDRedirect
//###  Desc: Sends HTTP Content that redirects Browser to new location
//###  Para: HTTPDConnSlotData *conn, char *newUrl
//###  Rets: -/-
//##################################################################################################
void ICACHE_FLASH_ATTR 
HTTPDRedirect(WebIf_HTTPDConnSlotData_t *conn, char *newUrl) 
  {

  char buff[1024];

  int l;

  l=os_sprintf(buff, "HTTP/1.1 302 Found\r\nLocation: %s\r\n\r\nMoved to %s\r\n", newUrl, newUrl);

  HTTPDSend(conn, buff, l);
  }
*/
