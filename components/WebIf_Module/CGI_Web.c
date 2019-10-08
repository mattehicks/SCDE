/*
 *##################################################################################################
 *  Function: The Web CGI (called from WebIf)
 *  ESP 8266EX SOC Activities ...
 *  (C) EcSUHA
 *  Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany 
 *  MSchulze780@GMAIL.COM
 *  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
 *##################################################################################################
 */
#include "ProjectConfig.h"
#include <esp8266.h>





#include <stdio.h>
#include <SCDE_s.h>

#include <WebIf_Module.h>
#include <WebIf_Module global.h>
#include <WebIf_Module global types.h>


//test
void FW_pO(WebIf_HTTPDConnSlotData_t *conn, strText_t out);

/**
 * ---------------------------------------------------------------------------------------------------
 *  DName: WebIf_Provided
 *  Desc: Data 'Provided By Module' for the Web IF module (functions + infos this module provides SCDE)
 *  Data:
 * ---------------------------------------------------------------------------------------------------
 */
static const char *httpNotFoundHeader="HTTP/1.0 404 Not Found\r\nServer: EcSUHA-httpd/0.1\r\nContent-Type: text/plain\r\n\r\nNot Found.\r\n";




/**
 * --------------------------------------------------------------------------------------------------
 *  FName: Web_cgi
 *  Desc: The root Web, CGI called from WebIf
 *  Info: 
 *  Para: WebIf_HTTPDConnSlotData_t *conn -> the connection details
 *  Rets: int -> result (enum xxx)
 * --------------------------------------------------------------------------------------------------
 */
int 
Web_cgi(WebIf_HTTPDConnSlotData_t *conn)
{
   // Connection aborted? Nothing to clean up.
  if (conn->conn == NULL) return HTTPD_CGI_DISCONNECT_CONN;

/*
define apiWEB FHEMWEB 8088 global
attr apiWEB column Alarms: Apartment: Living: Bedroom: Kitchen: Sonos: Residents: Weather: Bathroom: Logs: Statistics: DashboardRoom: System: hidden: all:
attr apiWEB hiddenroom input,detail,save,Unsorted,Everything,CUL_HM,FS20,Commandref,style,Edit files,Select style,Logfile,Floorplans,Remote doc,FileLogs,Apartment,Bathroom,Bedroom,Kitchen,Living,Residents,System,Weather,Event monitor,NEW
attr apiWEB room hidden
attr apiWEB webname webhook
*/

// strText_t test;

 //   FW_pO "<div id=\"menu\">$FW_detail details</div>";

  //  FW_pO "</table>";
 //   FW_pO "</td></tr>";

strText_t test = {"xx",2};
 FW_pO(conn,test);



//--------------------------------------------------------------------------------------------------

  # if SCDED_DBG >= 3
  os_printf("HTTPD Conn %p, slot %d CGI_NOT_FOUND for %s. TX 404!\n",
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


/** newName Web_pO (Web_PrintOut?)
 * --------------------------------------------------------------------------------------------------
 *  FName: FW_pO
 *  Desc: Print (append to output)
 *  Info: allocated memory in strText_t is freed!
 *  Para: WebIf_HTTPDConnSlotData_t *conn -> connection slot
 *        strText_t out -> content to print to output
 *  Rets: void
 * --------------------------------------------------------------------------------------------------
 */
void
FW_pO(WebIf_HTTPDConnSlotData_t *conn, strText_t out)
{
  // empty? -> No write
  if (!out.strText) return;

  // Write content to send buf
  HTTPD_Send_To_Send_Buffer(conn, out.strText, out.strTextLen);

  // Write "\n"
  HTTPD_Send_To_Send_Buffer(conn, "\n", 1);

  // free allocated
  free (out.strText);

  return;
}



/** newName FW_pH (Web_PrintHref?)
 * --------------------------------------------------------------------------------------------------
 *  FName: FW_pH
 *  Desc: Print href (append to output / or return)
 *  Info: - in case 'doRet  =TRUE' (return as strText_t) do not forget to free the allocated memory!
 *        - strText_t link initialized, please
 *        - strText_t txt initialized, please
 *        - strText_t class initialized, please
 *  Para: strText_t link  -> build href with this link
 *        strText_t txt   -> build href with this text
 *        bool td         -> FALSE = build not in ... / TRUE = in <TD></TD>
 *	  strText_t class -> class of the div tag
 *	  bool doRet      -> do return, FALSE = append to output / TRUE = return as strText_t
 *	  bool nonl       -> non line, FALSE = not a line, build in "<div $class>$ret</div>"
 *                           its a line TRUE = keep "$ret"
 *  Rets: strText_t       -> content, only if bool doRet is TRUE
 * --------------------------------------------------------------------------------------------------
 */

/*
strText_t
FW_pH(strText_t link
	,strText_t txt
	,bool td
	,strText_t class
	,bool doRet
	,bool nonl)
{

  // our result, will be filled in every case
  strText_t ret;


// correct the link in case of case #1
//  $link .= $FW_CSRF if($link =~ m/cmd/ &&
//                       $link !~m/cmd=style%20(list|select|eventMonitor)/);

// correct the link in case of case #2
//  $link = ($link =~ m,^/,) ? $link : "$FW_ME$FW_subdir?$link";


// Using onclick, as href starts safari in a webapp.
// Known issue: the pointer won't change

// build href, based on flags $FW_ss || $FW_tp
  if ($FW_ss || $FW_tp) {

	// build new
	ret.strTextLen = asprintf(&ret.strText
		,"<a onClick=\"location.href='%.*s'\">%.*s</a>"
		,link.strTextLen
		,link.strText
		,text.strTextLen
		,text.strText);
  } else {

	// build new
	ret.strTextLen = asprintf(&ret.strText
		,"<a href=\"%.*s\">%.*s</a>"
		,link.strTextLen
		,link.strText
		,text.strTextLen
		,text.strText);
  }


  // if an class for div tag is given, build it as " class="$class""
  if (class.strText) {

	// backup
	strText_t oldClass = class;

	// build new
	class.strTextLen = asprintf(&class.strText
		," class=\"%.*s\""
		,oldclass.strTextLen
		,oldclass.strText);

	// free old
	free(oldClass.strText);
  } // else class.strTextLen = 0; ??? should we care?


  // nonl FALSE? = not a line, build in "<div $class>$ret</div>"
  if (!nonl) {

	// backup
	strText_t oldRet = ret;

	// build new
	ret.strTextLen = asprintf(&ret.strText
		,"<div %.*s>%.*s</div>"
		,class.strTextLen
		,class.strText
		,oldRet.strTextLen
		,oldRet.strText);

	// free old
	free(oldRet.strText);
  }


 // wo wird link gefreed ?
  if (link.strText) free(class.strText);

 // wo wird txt gefreed ?
  if (txt.strText) free(class.strText);

  // wo wird class gefreed ?
  if (class.strText) free(class.strText);


  // td TRUE = build in <TD></TD>
  if (td) {

	// backup
	strText_t oldRet = ret;

	// build new
	ret.strTextLen = asprintf(&ret.strText
		,"<td>%.*s</td>"
		,oldRet.strTextLen
		,oldRet.strText);

	// free old
	free(oldRet.strText);
  }


  // doRet TRUE = return as strText_t
  if (doRet) return ret;

  // doRet FALSE = append to output
  FW_pO(ret);

  return;

}



*/
















/*

 vfprintf example 
#include <stdio.h>
#include <stdarg.h>

void WriteFormatted (FILE * stream, const char * format, ...)
{
  va_list args;
  va_start (args, format);
  vfprintf (stream, format, args);
  va_end (args);
}

int main ()
{
   FILE * pFile;

   pFile = fopen ("myfile.txt","w");

   WriteFormatted (pFile,"Call with %d variable argument.\n",1);
   WriteFormatted (pFile,"Call with %d variable %s.\n",2,"arguments");

   fclose (pFile);

   return 0;



// writes to sendbuffer
int
FW_p0 (WebIf_HTTPDConnSlotData_t* conn, const char * format, ...)
{
  va_list args;
  va_start (args, format);
  vfprintf (conn, format, args);
  va_end (args);
}

}























*/










