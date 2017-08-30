//##################################################################################################
//###  Function: Callback for Station-Configuration (cgi resource)
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

#include "StationCfg_cgi.h"
#include "CGI_NoAuthErr.h"
#include "CGI_NotFoundErr.h"



#include "SCDE_CmdParser.h"
#include "StationCfg_cgi.h"


/*
 *--------------------------------------------------------------------------------------------------
 *FName: StationCfg_cgi
 * Desc: Remote Procedure (RP). Should be assinged to resources. Used for incoming requests from
 *       Home-Automation-Systems. Active URL-Path possible (code may use Active-Drectory-ID)  
 *       Resource-Configuration: Methods POST || GET ; Scheme HTTP ; Path opt. with UID ; Mime .cgi|-/- 
 *       Used Header-Fields:
 *       An Finite-State-Machine processes requests and responses with code and message.
 * Info: conn->getArgs -> Query String
 *       conn->postBuff -> reserved for internals and attribute exchange
 * Para: WebIF_HTTPDConnSlotData_t *conn -> ptr to connection slot
 * Rets: int -> cgi result cmd
 *--------------------------------------------------------------------------------------------------
 */

// For: StationCfg_set RPC, Finite State Machine processing State - Stage 2 
enum StationCfg_cgi_state
  {					// #XX for debugging
    S2S_cgi_Check_Stage1_Parser_State = 0// #00 check if parser of stage 1 is in a state that stage 2 can continue work
  , S2S_cgi_Action_S1_Parse_Body_Data	// #01 detected that stage 1 has not parsed the complete message. Processing body data
  , S2S_cgi_Action_S1_Msg_Prsd_Complete	// #02 detected that stage 1 has parsed the complete message including body data
  , S2S_cgi_Check_RPC_Authorization	// #03 Check if request contains the required authorization
  , S2S_cgi_Check_and_Get_ADID		// #04 Check and get the Active-Directory-ID
  , S2S_cgi_Parse_Query_String		// #05 Parse the Query String

  // done - start writing response to TX buff.
  // No Error messages after this State !		 	   
  , S2S_cgi_TX_Response_with_Code	// #06 add response to the TX (response) buffer. TX + Disconn if error code
  , S2S_cgi_TX_Add_HDR_Flds		// #07 add header fields (if any) to the TX (response) buffer
  , S2S_cgi_TX_Add_Body_Data		// #08 add body data (if any) to the TX (response) buffer
  , S2S_cgi_DONE_TX_Response_Buff	// #09 done, incl. response, now TX response buffer and then disconnect
  };

int ICACHE_FLASH_ATTR
StationCfg_cgi(WebIF_HTTPDConnSlotData_t *conn)
  {

  // Connection aborted? Nothing to clean up.
  if (conn->conn == NULL) 
	return HTTPD_CGI_DISCONNECT_CONN;

//---------------------------------------------------------------------------------------------------

  # if SCDE_DBG >= 4
  os_printf("|StationCfg_cgi>");
  # endif

  // init variables used in finite-state-machine

  // for extracted Active Directory ID
  int16_t ADID = 0; // 0 blodsinn?

  // remember the readings affected by the parsing process
//  uint32_t AffectedReadings = 0;

  // pointer to identified arguments (Query)
  char *Args = NULL; //null blödsinn?

  // Response argument buffer & write position
  char RespArgsBuf[512];
  char* RespArgsWPos = RespArgsBuf;

  // Header Fields buffer & write position
//  char HdrFldsBuf[512];
  char* HdrFldsWPos = NULL;

  // Body-Data buffer & write position
//  char BdyDataBuf[512];
  char* BdyDataWPos = NULL;

// store errors ...
// int stage2_StationCfg_errno = (enum http_errno) 0; //enum?

  int S2_Statex = 0; //test

  // load Stage 2 processing state (initial is 0)
  // enum state S2_State = (enum state) conn->S2_State;
  enum StationCfg_cgi_state S2_State = (enum StationCfg_cgi_state) S2_Statex;

  reexecute:

  // Debug output, Stage 2, FSM State,
  # if SCDE_DBG >= 4
  os_printf("|S2.%d>",
	S2_State);
  # endif

  switch (S2_State)

	{
//--------------------------------------------------------------------------------------------------

	// #00 check if parser of stage 1 is in a state that stage 2 can continue work

	case S2S_cgi_Check_Stage1_Parser_State:

	if (conn->SlotParserState != s_HTTPD_Msg_Prsd_Complete)

		{
		return HTTPD_CGI_WAITRX_CONN;
		}

//	else
//		{
//		// Header parsed, now parsing BodyData - update state
//		S2_State = Step_Msg_Prs_Body_Data;
//		}

	else

		{

		// Message parsed complete - update state
		S2_State = S2S_cgi_Action_S1_Msg_Prsd_Complete;

		}

	break;

//--------------------------------------------------------------------------------------------------

	// #01

	case S2S_cgi_Action_S1_Parse_Body_Data:
	break;

//--------------------------------------------------------------------------------------------------

	// #02 set Arguments according to method, check if arguments

	case S2S_cgi_Action_S1_Msg_Prsd_Complete:

	// check methods
	if (conn->parser_method == HTTP_GET)			// GET Method ?

		{
		Args = conn->getArgs;
		}

	// check methods
	else if (conn->parser_method == HTTP_POST)		// POST Method ?

		{
		Args = conn->postBuff;
		}

	// do we have arguments ? -> Error & break if not
	if (!Args)

		{
		// no arguments error- finnish
		conn->parser_http_errno = HPE_INVALID_URL;

		// goto error;
		S2_State = S2S_cgi_TX_Response_with_Code;
		}

	// else go to next step
	else

		{

		#if SCDE_DBG >= 4
		os_printf("Args:%s>",
			Args);
  		#endif

		// we have args - go to next step
		S2_State = S2S_cgi_Check_RPC_Authorization;

		}

	break;

//--------------------------------------------------------------------------------------------------

	// #03 Check for valid authorization (AUTH_GENERIC_RESSOURCE)

	case S2S_cgi_Check_RPC_Authorization:

	if (SCDED_AuthCheck(conn, AUTH_GENERIC_RESSOURCE))

		{

		// not authorized - finnish with error
		conn->parser_http_errno = HPE_NO_AUTH;

		// goto error;
		S2_State = S2S_cgi_TX_Response_with_Code;

		}

	else

		{

		// authorized - go to next step
		S2_State = S2S_cgi_Parse_Query_String;

		}

	break;

//--------------------------------------------------------------------------------------------------

	// #05 Parse the Query String

	case S2S_cgi_Parse_Query_String:

	{

	// parse the input args for this Feature
	conn->parser_http_errno = SCDE_FeatCmdParser(conn
			,Args
		      //,HdrFldsLinkedList?
		      //,char* Data //BdyData
			,ADID
			,&RespArgsWPos
			,&HdrFldsWPos
			,&BdyDataWPos);

	#if SCDE_DBG >= 4
	if (RespArgsWPos != RespArgsBuf)
		{
		os_printf("Resp:%s>",
			RespArgsBuf);
		}
	#endif




	// parsed query string  - go to next step
	S2_State = S2S_cgi_TX_Response_with_Code;

	}

	break;

//--------------------------------------------------------------------------------------------------

	// #06 add response to the TX (response) buffer. TX + Disconn if error code

	case S2S_cgi_TX_Response_with_Code:

	// start resonse header with code and desc.
	SCDED_StartRespHeader2(conn);

	// >0 -> error, something went wrong, disconn
	if (conn->parser_http_errno)

		{
		// response added to TX buf, code ERROR - NO header fields, NO body data !!!
		return HTTPD_CGI_DISCONNECT_CONN;
		}

	// response added to TX buf, code OK - go to next step
	S2_State = S2S_cgi_TX_Add_HDR_Flds;

	break;

//--------------------------------------------------------------------------------------------------

	// #07 add header fields (if any) to the TX (response) buffer

	case S2S_cgi_TX_Add_HDR_Flds:

	if (RespArgsWPos != RespArgsBuf)
		{
		SCDED_AddHdrFld(conn, "RespMsg", (const char *) RespArgsBuf, RespArgsWPos - RespArgsBuf); //-1);
		}

	// DO NOT FORGET TO ADD CONTENT-LENGHT IF BODY DATA IS PRESENT !!!

	// End header by adding '\r\n'
	SCDED_EndHeader(conn);

	// header fields added - go to next step
	S2_State = S2S_cgi_TX_Add_Body_Data;

	break;

//--------------------------------------------------------------------------------------------------

	// #08 add body data (if any) to the TX (response) buffer

	case S2S_cgi_TX_Add_Body_Data:

	// not implemented


	// body data added - go to next step
	S2_State = S2S_cgi_DONE_TX_Response_Buff;

	break;

//--------------------------------------------------------------------------------------------------

	// #09 done, incl. response, now TX response buffer and then disconnect

	case S2S_cgi_DONE_TX_Response_Buff:

	# if SCDE_DBG >= 4
	os_printf("|Done!>");
	# endif

	return HTTPD_CGI_DISCONNECT_CONN;

	break;

//--------------------------------------------------------------------------------------------------

	default:
	
	// set error number
	conn->parser_http_errno = HPE_UNHANDLED_STATE;

	// error number set - go to next step (TX response with code and disconnect)
	S2_State = S2S_cgi_TX_Response_with_Code;

	break;

//--------------------------------------------------------------------------------------------------

	}

	goto reexecute;
}















