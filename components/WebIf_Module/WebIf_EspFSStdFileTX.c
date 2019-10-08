//##################################################################################################
//###  Function: Callback for System-on-Chip Hardware Configuration (jso resource)
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

//#include "espmissingincludes.h"
//#include <string.h>
//#include <osapi.h>
//#include "c_types.h"
//#include "user_interface.h"
///#include "espconn.h"
//#include "mem.h"
//#include "SCDED.h"
//#include "Espfs.h"
//#include "SCDE.h"


#include "WebIf_EspFSStdFileTX.h"
#include "WebIf_Module_Mimes.h"

#include "CGI_NoAuthErr.h"
#include "CGI_NotFoundErr.h"



#define MaxFsReadBlockSize 512

//#define WebIF_EspFSStdFileTX_DBG 5

/**  NEU: WebIF_StdFileTX ?
 * -------------------------------------------------------------------------------------------------
 *  FName: WebIF_EspFSStdFileTX
 *  Desc: WebIf ESP Filesystem standard file transmit - connector to let the WebIf use the
 *        espfs filesystem to serve the standard static files
 *  Para: WebIF_HTTPDConnSlotData_t *conn -> ptr to connection slot
 *  Rets: int -> HTTPD_CGI Result
 * -------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR 
WebIF_EspFSStdFileTX(WebIf_HTTPDConnSlotData_t *conn) 
{

  FILE *file = conn->PCData;

  int len;

  char buff[MaxFsReadBlockSize];

  // Connection aborted? Clean up.
  if (conn->conn == NULL) {

        fclose(file);

	return HTTPD_CGI_DISCONNECT_CONN;
  }

  // Load management ?
  if (SCDED_LoadSerializer(conn)) return HTTPD_CGI_PROCESS_CONN;

// -------------------------------------------------------------------------------------------------
/*
  // Check for valid authorization (AUTH_GENERIC_RESSOURCE)
  if (SCDED_AuthCheck(conn, AUTH_GENERIC_RESSOURCE))

	{

	// set callback for "Not Authorized Error"
	conn->cgi = NoAuthErr_cgi;
	return HTTPD_CGI_REEXECUTE;

	}	
*/
// -------------------------------------------------------------------------------------------------

  // First call to this cgi. Open the file so we can read it, start response ...
  if (file == NULL) {

	// to prepare the filename (path corrections)
	char *fileName;

	asprintf(&fileName
		,"/spiffs/webif%s"
		,conn->url);

	// open the file
	file = fopen(fileName, "r");

	// free prepared name
	free(fileName);

	if (file == NULL) {

		// set callback for "Not Found Error"
		conn->cgi = NotFoundErr_cgi;

		return HTTPD_CGI_REEXECUTE;
	}
	
	conn->PCData = file;

	// Start the response ...
	SCDED_StartRespHeader(conn, 200);

	# if WebIF_EspFSStdFileTX_DBG >= 3	
	os_printf("|URL:%s, MIME:%d>"
		,conn->url
		,SCDED_GetMimeTypeID(conn->url));
	# endif

	SCDED_AddHdrFld(conn
		,"Content-Type",
		AvailMimes[SCDED_GetMimeTypeID(conn->url)].mimetype
		,-1);

	SCDED_AddHdrFld(conn,
		"Cache-Control",
		"max-age=3600, must-revalidate",
		-1);

	SCDED_EndHeader(conn);
  }

  // get the initial number of bytes free in send buff
  int CurrTXBufFree = HTTPD_Send_To_Send_Buffer(conn, NULL, 0);
  int NextTXBufFree = 0;

  // free space in TX buffer?
  while (CurrTXBufFree) {

	// limit current TX buffer
	if (CurrTXBufFree > MaxFsReadBlockSize) 
		CurrTXBufFree = MaxFsReadBlockSize;

	len = fread(&buff, 1, CurrTXBufFree, file);

 	# if WebIF_EspFSStdFileTX_DBG >= 4	
 	os_printf("|read:%d, max:%d,TX>"
		,len
		,CurrTXBufFree);
	# endif

 	# if WebIF_EspFSStdFileTX_DBG >= 5	
 	SCDEFn->HexDumpOutFn ("EspFSStdFileTX-HEX",
		buff,
		len);
	# endif

	// yes we read some data. Send it to TX buf and get new free bytes
	if (len > 0) NextTXBufFree = HTTPD_Send_To_Send_Buffer(conn, buff, len);

	// indicator that file read is complete
//	if (len == 0) //!= CurrTXBufFree) // CurrTXBufFree cant be 0!
	if (len == 0) {

		// We're done.
		fclose(file);

		# if WebIF_EspFSStdFileTX_DBG >= 4	
 		os_printf("|FS read done>");
		# endif

		return HTTPD_CGI_DISCONNECT_CONN;
	}

	// prepare value of free bytes for next cycle
	CurrTXBufFree = NextTXBufFree;	
  }

  // no free bytes in TX-Buf left. continue next time ...
  return HTTPD_CGI_PROCESS_CONN;
}



