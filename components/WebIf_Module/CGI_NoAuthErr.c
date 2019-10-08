//#################################################################################################
//###  Function: CGI for "not authorized error"
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
//#include "CGI_TiStCfg.h"*/



#include "CGI_NoAuthErr.h"
#include "CGI_NotFoundErr.h"
//#include "CGI_Redirect.h"



//##################################################################################################
//### FName: NoAuthErr_cgi
//###  Desc: Cgi to send not authorized error and than disconnect ...
//###  Para: HTTPDConnSlotData *conn -> ptr to connection slot
//###  Rets: NONE
//##################################################################################################
int ICACHE_FLASH_ATTR 
NoAuthErr_cgi(WebIf_HTTPDConnSlotData_t *conn)
{
   // Connection aborted? Nothing to clean up.
  if (conn->conn == NULL) return HTTPD_CGI_DISCONNECT_CONN;

//--------------------------------------------------------------------------------------------------

  # if SCDED_DBG >= 3
  os_printf("HTTPD Conn %p, slot %d CGI_NOAUTH for %s. TX 401!\n",
	conn->conn,
	conn->slot_no,
	conn->url);
  # endif

  // missing or wrong Authentication happened,
  // write response:  we need WWW-Authentication - this will force log in screen
  const char *forbidden = "401 Forbidden.";
  SCDED_StartRespHeader(conn, 401);
  SCDED_AddHdrFld(conn, "Content-Type", "text/plain",-1);
  SCDED_AddHdrFld(conn, "WWW-Authenticate", "Basic realm=\""HTTP_AUTH_REALM"\"",-1);
  SCDED_EndHeader(conn);
  HTTPD_Send_To_Send_Buffer(conn, forbidden, -1);

//--------------------------------------------------------------------------------------------------

  // finnished, send rest of data, disconnect
  return HTTPD_CGI_DISCONNECT_CONN;
}


