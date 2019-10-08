//##################################################################################################
//###  Function: EspFS_cgi Code for Filesytem Acess
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


/*
#include "espmissingincludes.h"
#include <string.h>
#include <osapi.h>
#include "c_types.h"
#include "user_interface.h"
#include "espconn.h"
#include "mem.h"
#include "ProjectConfig.h"
#include "SCDED.h"
#include "Espfs.h"
#include "SCDE.h"
*/


#include "WebIf_EspFSAdvFileTX.h"
#include "WebIf_Module_Mimes.h"

#include "CGI_NoAuthErr.h"
#include "CGI_NotFoundErr.h"

#include "Device_WebIfTokens.h"


#define MaxFsReadBlockSize 512


// type definition for procedure callback
typedef void (* TplCallback)(WebIf_HTTPDConnSlotData_t *conn, char *token, void **arg);

// Storage for token processing from file
typedef struct {
  FILE* file;
  void *tplArg;
  char token[64];	// max allowed len for replacement-token!
  int tokenPos;
} TplData;



//#define WebIF_EspFSAdvFileTX_DBG 5



/** Neu: WebIF_AdvFileTX ?
 * --------------------------------------------------------------------------------------------------
 *  FName: WebIF_EspFSAdvFileTX
 *  Desc: Connector to let SCDED use the espfs filesystem to serve the files,
 *        !WITH! template %X% processing
 *  Info: 
 *  Para: WebIF_HTTPDConnSlotData_t *conn -> ptr to connection slot
 *  Rets: int -> cgi result cmd
 * --------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR 
WebIf_EspFSAdvFileTX(WebIf_HTTPDConnSlotData_t *conn) 
// dies ist eine cgi ! rename !EspFsTemplate_cgi
{
  // RPCData stores pointer to template processing data
  TplData* tpd = conn->PCData;

  int len;
  int x, sp = 0;
  char *e = NULL;
  char buff[MaxFsReadBlockSize];

   // Connection aborted? Clean up.
  if ( conn->conn == NULL ) {

	// inform cgi (NULL) about clean up
	( (TplCallback) (conn->PCArg) ) (conn, NULL, &tpd->tplArg);

        fclose (tpd->file);

	// free memory reserved for template processing
	free (tpd);

	return HTTPD_CGI_DISCONNECT_CONN;
  }

  // Load management ?
  if ( SCDED_LoadSerializer(conn) ) return HTTPD_CGI_PROCESS_CONN;

//--------------------------------------------------------------------------------------------------
/*
  // Check for valid authorization (AUTH_GENERIC_RESSOURCE)
  if ( SCDED_AuthCheck (conn, AUTH_GENERIC_RESSOURCE) ) {

	// set callback for "Not Authorized Error"
	conn->cgi = NoAuthErr_cgi;

	return HTTPD_CGI_REEXECUTE;
  }	
*/
//--------------------------------------------------------------------------------------------------	

  // First call to this cgi. Open the file so we can read it.
  if ( tpd == NULL ) {

	// alloc memory for template processing data
	tpd = (TplData *) malloc (sizeof (TplData));

	// to prepare the filename (path corrections)
	char* fileName;

	// Option to use alternative file from filesystem	
	if ( conn->AltFSFile != NULL ) {

		asprintf (&fileName,
			"/spiffs/webif%s",
			conn->AltFSFile);
	}

	else	{

		asprintf (&fileName,
			"/spiffs/webif%s",
			conn->url);
	}

	// open the file
	tpd->file = fopen (fileName, "r");

	// free prepared name
	free (fileName);

	tpd->tplArg = NULL;
	tpd->tokenPos = -1;

	// file not found ?
	if ( tpd->file == NULL ) {

		// free memory for template processing data
		free (tpd);

		// set callback for "Not Found Error"
		conn->cgi = NotFoundErr_cgi;

		return HTTPD_CGI_REEXECUTE;
	}

	// continue processing ...		
	conn->PCData = tpd;

	SCDED_StartRespHeader (conn, 200);

	SCDED_AddHdrFld(conn, "Content-Type",
		AvailMimes[SCDED_GetMimeTypeID(conn->url)].mimetype, -1);

	SCDED_EndHeader(conn);
  }

// -------------------------------------------------------------------------------------------------

  // get the initial number of bytes free in send buff (1 - x)
  int CurrTXBufFree = HTTPD_Send_To_Send_Buffer(conn, NULL, 0);
  int NextTXBufFree = 0;

  // free space in TX buffer?
  while (CurrTXBufFree) {

	// limit current TX buffer
	if (CurrTXBufFree > MaxFsReadBlockSize) 
		CurrTXBufFree = MaxFsReadBlockSize;

	// read till next % (>=1), or max CurrTXBufFree len
	len = fread(buff, 1, CurrTXBufFree, tpd->file);

 	# if WebIF_EspFSAdvFileTX_DBG >= 4	
 	os_printf("|read:%d, max:%d,TX>"
		,len
		,CurrTXBufFree);
	# endif

 	# if WebIF_EspFSAdvFileTX_DBG >= 5	
 	SCDEFn->HexDumpOutFn ("EspFsTSRead-HEX",//TS=TokenStream?
		buff,
		len);
	# endif

	// data to send counter 'always' to prevent sending of rest ...
	sp = 0;

	// parse the data and replace tokens
	if (len > 0) {

		e = buff;

		for (x=0; x < len; x++) {

			// in ordinary text (tpd->tokenPos == -1), not in token ...
			if (tpd->tokenPos == -1) {

				// found token beginning ...
				if (buff[x] == '%') {

					// Send raw data up to now
					if (sp != 0) NextTXBufFree = HTTPD_Send_To_Send_Buffer(conn, e, sp);

					sp = 0;

					// Go collect token chars.
					tpd->tokenPos = 0;
				}

				// not a token beginning ...
				else	{
		
					sp++;
				}
			}

			// Inside token ...(tpd->tokenPos == 0)
			else {

				if (buff[x] == '%') {

					if (tpd->tokenPos == 0) {

						// This is the second % of a %% escape string.
						// Send a single % and resume with the normal program flow.
						NextTXBufFree = HTTPD_Send_To_Send_Buffer(conn, "%", 1);
					}

					// token end '%' detected, executen token replacement
					else	{

						// This is an actual token.
						tpd->token[tpd->tokenPos++] = 0; // zero-terminate token

						NextTXBufFree = Device_WEBIFtokens(conn
							,tpd->token
							,&tpd->tplArg);

						// no replacement happened (-1) ?, then continue with specific template
						if (NextTXBufFree == -1) {

							((TplCallback)(conn->PCArg))(conn
								,tpd->token
								,&tpd->tplArg);

							NextTXBufFree = HTTPD_Send_To_Send_Buffer(conn, NULL, 0);
						}
					}

					// continue, now outside token in ordinary text ...
					e = &buff[x+1];

					tpd->tokenPos = -1;
				}

				// token continuing, copy out token to token buffer
		 		else {

					if (tpd->tokenPos < (sizeof(tpd->token)-1))
						tpd->token[tpd->tokenPos++] = buff[x];
				}
			}
		}
	}

	// Send remaining data ..
	if (sp != 0) NextTXBufFree = HTTPD_Send_To_Send_Buffer(conn, e, sp);

	// indicator that file read is complete (read len != requested len)
//	if (len == 0) //!= CurrTXBufFree) // CurrTXBufFree cant be 0!
	if (len == 0) {

		// We're done - inform cgi (NULL) about clean up
		((TplCallback)(conn->PCArg))(conn, NULL, &tpd->tplArg);

		// We're done.
		fclose(tpd->file);

		// free memory for template processing data
		free(tpd);

		# if WebIF_EspFSAdvFileTX_DBG >= 4	
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


