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
//#include "CGI_StdUi.h"
#include "CGI_SwITCH.h"
//#include "Control.h"
#include "espmissingincludes.h"
#include "SCDE.h"
//#include "TransmitQueue.h"
#include "sntp.h"
//#include "CGI_TiStCfg.h"
#include "SCDED.h"

#include "CGI_SCDEETX.h"
#include "CGI_NoAuthErr.h"
#include "CGI_NotFoundErr.h"
#include "CGI_Redirect.h"*/

// Answer in case NOT FOUND 
static const char *httpNotFoundHeader="HTTP/1.0 404 Not Found\r\nServer: EcSUHA-httpd/0.1\r\nContent-Type: text/plain\r\n\r\nNot Found.\r\n";


//##################################################################################################
//### FName: NotFoundErr_cgi
//###  Desc: Cgi to send not found error and than disconnect ...
//###  Para: HTTPDConnSlotData *conn -> ptr to connection slot
//###  Rets: NONE
//##################################################################################################
int ICACHE_FLASH_ATTR 
NotFoundErr_cgi(WebIf_HTTPDConnSlotData_t *conn)
{
  // Connection aborted? Nothing to clean up.
  if (conn->conn == NULL) return HTTPD_CGI_DISCONNECT_CONN;

//--------------------------------------------------------------------------------------------------

  # if SCDED_DBG >= 3
  printf("HTTPD Conn %p, slot %d CGI_NOT_FOUND for %s. TX 404!\n",
		conn->conn,
		conn->slot_no,
		conn->url);
  # endif

  // Write response "not found" to send buf
  HTTPD_Send_To_Send_Buffer(conn, httpNotFoundHeader, -1);

//--------------------------------------------------------------------------------------------------

  // finnished, send rest of data, disconnect
  return HTTPD_CGI_DISCONNECT_CONN;
}


