// SCDE-Module WebIf (provides web resources and access)

#ifndef MODULEWEBIF_H
#define MODULEWEBIF_H

// -------------------------------------------------------------------------------------------------

// this Module is made for the Smart-Connected-Device-Engine
#include "SCDE_s.h"

// this Module provides functions for other Modules:
// -- no ---

// this Module uses an 1st stage:
// -- no ---

// -------------------------------------------------------------------------------------------------

// stores the Root Data of the Smart Connected Devices Engine - at/for this Module
SCDERoot_t* SCDERoot_at_WebIf_M;

// stores SCDEFn (Functions / callbacks) provided for operation - at/for this Module
SCDEFn_t* SCDEFn_at_WebIf_M;

// -------------------------------------------------------------------------------------------------



// HTTPD authentication definitions
#define HTTP_AUTH_REALM "Protected"
#define AUTH_MAX_USER_LEN 32
#define AUTH_MAX_PASS_LEN 32


// different resource types for authorize
#define AUTH_GENERIC_RESSOURCE	1<<0
#define AUTH_SET_RESSOURCE	1<<1

#define MXHEADERFLDNAMELEN	256
#define MXHEADERFLDVALUELEN	256
#define MXBODYLEN		1024

#define RECV_BUF_SIZE 2400 //2048




typedef struct HTTPDConnSlotPrivData HTTPDConnSlotPrivData;
typedef struct WebIf_HTTPDConnSlotData_s WebIf_HTTPDConnSlotData_t;
typedef struct http_parser http_parser;
typedef struct http_parser_settings_s http_parser_settings_t;
typedef struct HTTPD_InstanceCfg_s HTTPD_InstanceCfg_t;


// callback proc for ... ?
typedef int (*PCallback) (WebIf_HTTPDConnSlotData_t *connData);

// callback proc for header field processing
typedef int (*HdrFldProcCb) (WebIf_HTTPDConnSlotData_t *connData, const char *at, size_t length);










/*
 *  WebIf - Data Table A for Active Resources - The content of the Active Directory - 
 *          PART A (Resource-Data-Row) 
 */
typedef struct WebIf_ActiveResourcesDataA_s {

  uint32_t AllowedMethodsBF;		// Allowed (implemented) Methods for this resource (Bit-Field) (enum http_method from httpD.h)
					// B 3      22      11      87      0
					// I 1      43      65                
					// T 00000000000000000000000000000000 4 Byte Data length
					//   \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\- 00.DELETE
					//    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\- 01.GET		-> Used
					//     \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\- 02.SET		-> Used
					//      \\\\\\\\\\\\\\\\\\\\\\\\\\\\\- 03.POST
					//       \\\\\\\\\\\\\\\\\\\\\\\\\\\\- 04.PUT		-> Used
					//        \\\\\\\\\\\\\\\\\\\\\\\\\\\- 05.CONNECT
					//         \\\\\\\\\\\\\\\\\\\\\\\\\\- 06.OPTIONS
					//          \\\\\\\\\\\\\\\\\\\\\\\\\- 07.TRACE
					//           \\\\\\\\\\\\\\\\\\\\\\\\- 08.COPY
					//            \\\\\\\\\\\\\\\\\\\\\\\- 09.LOCK
					//             \\\\\\\\\\\\\\\\\\\\\\- 01.MKCOL
					//              \\\\\\\\\\\\\\\\\\\\\- 11.MOVE
					//               \\\\\\\\\\\\\\\\\\\\- 12.PROPFIND
					//                \\\\\\\\\\\\\\\\\\\- 13.PROPPATCH
					//                 \\\\\\\\\\\\\\\\\\- 14.SEARCH
					//                  \\\\\\\\\\\\\\\\\- 15.UNLOCK
					//                   \\\\\\\\\\\\\\\\- 16.BIND
					//                    \\\\\\\\\\\\\\\- 17.REBIND
					//                     \\\\\\\\\\\\\\- 18.UNBIND
					//                      \\\\\\\\\\\\\- 19.ACL
					//                       \\\\\\\\\\\\- 20.REPORT
					//                        \\\\\\\\\\\- 21.MKACTIVITY
					//                         \\\\\\\\\\- 22.CHECKOUT
					//                          \\\\\\\\\- 23.vMERGE
					//                           \\\\\\\\- 24.MSEARCH
					//                            \\\\\\\- 25.NOTIFY
					//                             \\\\\\- 26.SUBSCRIBE
					//                              \\\\\- 27.UNSUBSCRIBE
					//                               \\\\- 28.PATCH
					//                                \\\- 29.PURGE
					//                                 \\- 30.MKCALENDAR
					//                                  \- 31.LINK

//---

  uint16_t AllowedDocMimesBF;		// Allowed (implemented) DocMimes Bit-Field for this resource (from SCDE_DocMimes.h)
  uint16_t AllowedSchemesBF;		// Allowed (implemented) Schemes Bit-Field for this resource (from SCDE_Schemes.h)

//---

  uint16_t freeforuse;			//
  uint8_t RPCNo;			// Assigns the RPC-Data-Row (Content Table B) for this resource
  uint8_t EnaByBit;			// Defines the bit (in HttpDDirEnaCtrl) that enables the entry for dir customization; 0 = always ena

//---

  const char *Url;			// Ptr to URL Name of this resource (that is verified and processed)
} WebIf_ActiveResourcesDataA_t;



/*
 *  WebIf - Data Table B for Active Resources - The content of the Active Directory - PART B (Procedure-Call-Data-Row) 
 */
typedef struct WebIf_ActiveResourcesDataB_s {

  const uint32_t RPCFucID; //CgiFucID;	// const int CgiFucID;	Assigns the Cgi-Data-Row (Content Table B) for this resource
//const uint16_t RPCFucID;		// const int CgiFucID;	Assigns the Cgi-Data-Row (Content Table B) for this resource
//const uint16_t AllowedSchemeBF;	// const int CgiFucID;	Assigns the Cgi-Data-Row (Content Table B) for this resource

  PCallback PCb; //cgiSendCallback cgiCb;	// Remote-Procedure-Call Callback which processes this active resource

  const void  *PCArg; //*cgiArg;	// Argument (Data), accessible by the RPC-Callback Procedure for operations
} WebIf_ActiveResourcesDataB_t;












typedef void *espconn_handle;
/** A callback prototype to inform about events for a espconn */
typedef void (* espconn_connect_callback)(void *arg);
typedef void (* espconn_reconnect_callback)(void *arg, int8_t err); //sint8 err);
typedef void (* espconn_recv_callback)(void *arg, char *pdata, unsigned short len);
typedef void (* espconn_sent_callback)(void *arg);




/* Definitions for error constants. */
#define ESPCONN_OK          0    /* No error, everything OK. */
#define ESPCONN_MEM        -1    /* Out of memory error.     */
#define ESPCONN_TIMEOUT    -3    /* Timeout.                 */
#define ESPCONN_RTE        -4    /* Routing problem.         */
#define ESPCONN_INPROGRESS  -5    /* Operation in progress    */
#define ESPCONN_MAXNUM		-7	 /* Total number exceeds the set maximum*/
#define ESPCONN_ABRT       -8    /* Connection aborted.      */
#define ESPCONN_RST        -9    /* Connection reset.        */
#define ESPCONN_CLSD       -10   /* Connection closed.       */
#define ESPCONN_CONN       -11   /* Not connected.           */
#define ESPCONN_ARG        -12   /* Illegal argument.        */
#define ESPCONN_IF		   -14	 /* UDP send error			 */
#define ESPCONN_ISCONN     -15   /* Already connected.       */
#define ESPCONN_HANDSHAKE  -28   /* ssl handshake failed	 */
#define ESPCONN_SSL_INVALID_DATA  -61   /* ssl application invalid	 */


/** Protocol family and type of the espconn */
enum espconn_type {
    ESPCONN_INVALID    = 0,
    /* ESPCONN_TCP Group */
    ESPCONN_TCP        = 0x10,
    /* ESPCONN_UDP Group */
    ESPCONN_UDP        = 0x20,
};

/** Current state of the espconn. Non-TCP espconn are always in state ESPCONN_NONE! */
enum espconn_state {
    ESPCONN_NONE,
    ESPCONN_WAIT,
    ESPCONN_LISTEN,
    ESPCONN_CONNECT,
    ESPCONN_WRITE,
    ESPCONN_READ,
    ESPCONN_CLOSE
};



typedef struct _esp_tcp {
  int remote_port;
  int local_port;
  uint8_t local_ip[4];
  uint8_t remote_ip[4];
  espconn_connect_callback connect_callback;
  espconn_reconnect_callback reconnect_callback;
  espconn_connect_callback disconnect_callback;
//  espconn_connect_callback write_finish_fn;
} esp_tcp;

typedef struct _esp_udp {
  int remote_port;
  int local_port;
  uint8_t local_ip[4];
  uint8_t remote_ip[4];
} esp_udp;


typedef struct _remot_info {
	enum espconn_state state;
	int remote_port;
	uint8_t remote_ip[4];
} remot_info;





enum espconn_option{
	ESPCONN_START = 0x00,
	ESPCONN_REUSEADDR = 0x01,
	ESPCONN_NODELAY = 0x02,
	ESPCONN_COPY = 0x04,
	ESPCONN_KEEPALIVE = 0x08,
	ESPCONN_END
};

enum espconn_level{
	ESPCONN_KEEPIDLE,
	ESPCONN_KEEPINTVL,
	ESPCONN_KEEPCNT
};

enum {
	ESPCONN_IDLE = 0,
	ESPCONN_CLIENT,
	ESPCONN_SERVER,
	ESPCONN_BOTH,
	ESPCONN_MAX
};







/* 
 * Entry WebIf Definition (struct) stores values for operation valid only for the defined instance of an
 * loaded module. Values are initialized by SCDE an the loaded module itself.
 */
typedef struct Entry_WebIf_Definition_s {

  Entry_Definition_t common;		// ... the common part of the definition

//---

  enum espconn_type type;		// type of the espconn (TCP, UDP)
  enum espconn_state state;		// current state of the espconn
  union {
	esp_tcp *tcp;			// connection details IP,Ports, ...
	esp_udp *udp;
  } proto;
    
  espconn_recv_callback recv_callback;	// A callback function for event: receive-data

  espconn_sent_callback send_callback;	// a callback function for event: send-data

  uint8_t link_cnt;

  void* reverse;			// the reverse link to application-conn-slot-data

  int WebIf_CtrlRegA;			// WebIf Control-Reg-A (enum WebIf_CtrlRegA from WEBIF.h)

  HTTPD_InstanceCfg_t* HTTPD_InstCfg;	// link to configuration of this HTTPD-Instance

  uint8_t slot_no;			// slot number in this instance

} Entry_WebIf_Definition_t;













/* 
 * Definition typedef stores values for operation valid only for the defined instance of an
 * loaded module. Values are initialized by HCTRLD an the loaded module itself.
 */
typedef struct WebIf_HTTPDConnSlotData_s { //HTTPD_Conn_Slot

  Entry_WebIf_Definition_t* conn;	// Link to lower level connection management

//- V V V V V V V V V V V V V V V V V V // TX-Helper

  char* send_buffer;//uint8_t*		// Pointer to current allocated send buffer w. size [MAX_SB_LEN] (NULL=NO Send-Buffer)
  int send_buffer_write_pos; //uint16_t	// Send buffer, current write position (offset), !> 0 = its allocated!
  char* trailing_buffer;//uint8_t*	// An 'trailing_buffer' in case of 'send_buffer' overflow. Its prio for next transmission!
  int trailing_buffer_len; //uint16_t	// The 'trailing_buffer' length of (allocated & stored) data, !> 0 = its allocated!


//- V V V V V V V V V V V V V V V V V V // Helper

  HTTPDConnSlotPrivData* priv;		// Opaque pointer to data for internal httpd housekeeping (currently unused)

//- V V V V V V V V V V V V V V V V V V // Procedure Callback Data

  PCallback cgi;			// Assigned Procedure Callback for Processing
  const void* PCArg;			// Argument for Procedure-Callback-Processing
  void* PCData;				// Data for PC Procedure-Callback-Operations (counting...)

//- V V V V V V V V V V V V V V V V V V // General Connection Control

  uint16_t ConnCtrlFlags;		// Connection Control Register (enum ConnCtrlFlags from httpD.h)
  int16_t InactivityTimer;		// 10HZ Timer counting up after any Callback to detect broken connections

//- V V V V V V V V V V V V V V V V V V // ?




//- union 32Bit connspecific possible 

  uint8_t slot_no;			// helper to show slot number
  unsigned int SlotCgiState : 4;	// enum SlotCgiState
  unsigned int SlotParserState : 4;	// enum SlotParserState
  uint16_t LP10HzCounter;		// LongPoll 100ms/ 10Hz Counter (0.1-6553.6 Sec) for this conn

//-

//union
//	{ routine verschieben!
//	RPCCallback cgi; //soll rpc //alt:cgiSendCallback cgi;	// Assigned RPC Processing Callback
//	HdrFldProcCb HdrFldFnCb;// Header Field processing Function Callback
// int32_t HdrFldId;


// temporary fields for header field extraction
  char* p_hdr_fld_name_buff;		// current Header Field Name Buffer ... during parsing
  int hdr_fld_name_len;			// current HdrFldNameBuff length ... during parsing
  char* p_hdr_fld_value_buff;		// Pointer to Header Field Value Buffer ... during parsing
  int hdr_fld_value_len;		// current HdrFldValueBuff length ... during parsing

//	};




//- V V V V V V V V V V V V V V V V V V // SCD-Engine STAGE 1 processing -> parsing HTTP

  uint32_t parser_nread;          	// # bytes read in various scenarios
  uint64_t parser_content_length;	// Content-Length of Body from Headerfield ; -1 = not specified ; (counts down while parsed?)

//-

  unsigned int parser_type : 2;         // Parser type cfg  AND result after parse (enum http_parser_type from httpD.h)
  unsigned int parser_flags : 8;        // Extracted information Flags F_* (enum flags from httpD.h)
					// BIT 0 = F_CHUNKED
					// BIT 1 = F_CONNECTION_KEEP_ALIVE
					// BIT 2 = F_CONNECTION_CLOSE
					// BIT 3 = F_CONNECTION_UPGRADE
					// BIT 4 = F_TRAILING
					// BIT 5 = F_UPGRADE
					// BIT 6 = F_SKIPBODY
					// BIT 7 = F_CONTENTLENGTH -> Content Length is set
  unsigned int parser_state : 7;        // State of parser finite state machine (enum state from http_parser.c)
  unsigned int parser_header_state : 7; // State of parser_header finite state machine (enum header_state from http_parser.c)
  unsigned int parser_index : 7;        // index into current matcher
  unsigned int parser_lenient_http_headers : 1;

//-

  uint16_t parser_http_major;		// FROM RESPONSE ONLY - extracted major version code (0-999 valid)
  uint16_t parser_http_minor;		// FROM RESPONSE ONLY - extracted minor version code (0-999 valid)

//-	

  uint16_t parser_status_code;		// FROM RESPONSE ONLY - extracted status code (0-999 valid)		// unsigned int parser_status_code : 16;
  unsigned int parser_method : 8;	// FROM REQUEST ONLY - extracted method (enum http_method from httpD.h)
  unsigned int parser_http_errno : 7;	// internal error number in various operations
  unsigned int parser_upgrade : 1;	// FROM REQUEST ONLY - upgrade header was present. Check when http_parser_execute() returns

//- Extracted values in case of an request ...

  unsigned int parser_scheme : 4;	// FROM REQUEST ONLY - scheme extracted by uri-parsing (AvailSchemes[] from ps.h)
  unsigned int parser_mime : 4;		// FROM REQUEST ONLY - mime extracted by uri-parsing (AvailContentTypes[] from ps.h)
  unsigned int freereserved1 : 8;	// 
  int16_t AxctiveDirID;			// FROM REQUEST ONLY - Active Directory ID (ADID) extracted by uri-parsing (0-32767 valid,-1,-2,-3
  Entry_Definition_t* p_entry_active_dir_matching_definition;	// FROM REQUEST ONLY - Active Directory ID (ADID) extracted by uri-parsing (0-32767 valid,-1,-2,-3 indicate special cases)

//-

  STAILQ_HEAD (stailhead, Entry_Header_Field_s) head_header_field;// REQUEST & RESPONSE - Stores Header-Fields in an SLTQ

//-

  char* url;				// FROM REQUEST ONLY - Ptr to allocated memory filled with PATH extracted from URI
  char* AltFSFile;			// FROM REQUEST ONLY - (NULL = Filename from Path!) else Ptr. to alternative filename in FS
  char* DestUrl;//MatchedUrl		// FROM REQUEST ONLY - Ptr to BuildInURL that matches (Active URL with Tokens for reconstruction)
  char* getArgs;//Query			// FROM REQUEST ONLY - Ptr to allocated memory filled with QUERY (GETARGS) extracted from URI, if any
  char* hostName;			// FROM REQUEST ONLY - content of host name field

//-

  char* body_data;//postBuff; //BodyData		// REQUEST & RESPONSE - Pointer to BdyData Buffer
  int body_data_len;//postLen;	//BodyLen		// REQUEST & RESPONSE - length of stored body data
//int postPos;	//BodyPos benötigt?	// counter for post position (contains whole post data len, not only buffer!)
} WebIf_HTTPDConnSlotData_t;



// --------------------------------------------------------------------------------------------------



/*
 *  Functions provided to SCDE by Module - for type operation (A-Z)
 */

//
strTextMultiple_t* WebIf_Add(Common_Definition_t* Common_Definition, uint8_t *kvArgs, size_t kvArgsLen);

//
strTextMultiple_t* WebIf_Define(Common_Definition_t *Common_Definition);

//
int WebIf_Direct_Read(Common_Definition_t*  p_entry_definition);

//
int WebIf_Direct_Write(Common_Definition_t*  p_entry_definition);

//
int WebIf_IdleCbX(Common_Definition_t *Common_Definition);

//
int WebIf_Initialize(SCDERoot_t* SCDERoot);

//
strTextMultiple_t* WebIf_Set(Common_Definition_t* Common_Definition, uint8_t *setArgs, size_t setArgsLen);

//
strTextMultiple_t* WebIf_Sub(Common_Definition_t* Common_Definition , uint8_t *kArgs, size_t kArgsLen);

//
strTextMultiple_t* WebIf_Undefine(Common_Definition_t* Common_Definition);



// --------------------------------------------------------------------------------------------------



/*
 *  helpers provided to module for type operation
 */
void WebIf_disconnect(Entry_WebIf_Definition_t *WebIf_Definition);
void espconn_regist_connectcb(Entry_WebIf_Definition_t *conn, espconn_connect_callback connectCb);
void espconn_regist_disconcb(Entry_WebIf_Definition_t *conn, espconn_connect_callback DisconCb);
void espconn_regist_sentcb(Entry_WebIf_Definition_t *conn, espconn_sent_callback SentCb);
void espconn_regist_recvcb(Entry_WebIf_Definition_t *conn, espconn_recv_callback RecvCb);
void espconn_regist_reconcb(Entry_WebIf_Definition_t *conn, espconn_reconnect_callback ReconCb);
int  WebIf_sent(Entry_WebIf_Definition_t* p_entry_webif_definition, uint8_t* send_buffer, uint send_buffer_len);

//----------------- old stuff ------------------------





// Max allowed connections to SCDED. (Limit is 31+1)
// +1 is the SCDE-Event-TX connection, its conn[0]
#define MAX_SCDED_CONN 10 +1



/*
// get current state hlper macro for stage 2 processing FSM
#define S2_CURRENT_STATE() S2_State

// update current state hlper macro for stage 2 processing FSM
#define S2_UPDATE_STATE(V) S2_State = (enum state) (V)

// set error code helper macro for stage 2 processing FSM
#define SET_S2_ERRNO(e)							\
do									\
	{								\
	conn->parser_http_errno = (e);					\
	} while(0)
*/
/*// set error code helper macro for stage 2 processing FSM
#define SET_S2_ERRNO(e)							\
do									\
	{								\
	stage2_scde_errno = (e);					\
	} while(0)
*/












// CGI-Result CMD OLD OLD	!!!!!		// Commands:
#define HTTPD_CGI_REEXECUTE 1			// CMD: re-execute procedure call (allows pc-switching)
#define HTTPD_CGI_DISCONNECT_CONN 2		// disconnect and retire connection
#define HTTPD_CGI_F_DISCONNECT_CONN 3		// ETX forces a real disconnect (override keep alive func)
#define HTTPD_CGI_PROCESS_CONN 4		// continue processing connection incl. callback (IdleCb)
#define HTTPD_CGI_WAITRX_CONN 5			// wait connection for further RX, keep conn


// CGI-Abnormal-Execution-markers		// Markers:
#define CGI_FINNISHED ( (void *) 0)		// Used to indicate clean up and retire proc
#define CGI_UPGRADE_WEBSOCKET ( (void *) 1)	// 
#define CGI_NOT_ASSIGNED ( (void *) 2)		// used to RX more data, till CGI can be assigned


/*
// Possible result CMDs of the connections assigned Procedure-Call
#define HTTPD_PCR_CLEANEDUP 1			// INFO: cleanup-done (resonse to ?)
#define HTTPD_PCR_REEXECUTE_PC 1		// CMD: re-execute procedure call (allows PC-switching)
#define HTTPD_PCR_DISCONNECT_CONN_RETIRE 2	// CMD: PC done, disconnect, retire this connection
#define HTTPD_PCR_CONN_DONE 3			// CMD: PC done, retire connection, and hold or disconnect (depends on flags?)
#define HTTPD_PCR_WAIT_CONN_GET_IDLE_CB 4	// CMD: wait for connection activity (RX and keep conn) + get one callback (any Callback, or next idle Callback)
#define HTTPD_PCR_WAIT_CONN 5			// CMD: wait for connection activity (RX and keep conn)

// Procedure Call can be used as a marker for the following events
#define PC_FINNISHED_CLEANUP ( (void *) 0)	// Marker: This PC is finnished, please clean up
#define PC_UPGRADE_WEBSOCKET ( (void *) 1)	// Marker: upgrade conn to Websocket + and install PC for Websocket
#define PC_NOT_ASSIGNED ( (void *) 2)		// Marker: PC could not be assigned from the currently received data (no execution - RX more data)
*/












// SCDE Event-TX - STATISTICS
  extern int SCDEETX_ConnCNT;	// connections made counter
  extern int SCDEETX_MsgCNT;	// messages sent from queue counter
  extern int SCDEETX_RcvOKCNT;	// messages OK sent (determinated by response code)
  extern int SCDEETX_RcvNOKCNT;	// Error counter: retrys, ... ?


//----------


/* 
 * Entry_Header_Field (struct)
 * - stores the value of an header field
 */
typedef struct Entry_Header_Field_s {
  STAILQ_ENTRY(Entry_Header_Field_s) entries;	// link to next header field (SLTQ)
  int8_t hdr_fld_id;  				// stores header field ID - from HTTPD Table
  char* p_hdr_fld_val;				// stores the assigned value in allocated mem
} Entry_Header_Field_t;

/*
 * Constructor for the singly linked tail queue (head), which can hold multiple linked strings
 * SLTQ can be used for an FIFO queue by adding entries at the tail and fetching entries from the head
 * SLTQ is inefficient when removing arbitrary elements
 */
STAILQ_HEAD(Head_Header_Field_s, Entry_Header_Field_s);


//----------


// Information Flags - for Connection Control
enum ConnCtrlFlags
  { F_GENERATE_IDLE_CALLBACK	= 1 << 0	// Force one further callback (may be by generating one idle callback)
  , F_CONN_IS_AUTHENTICATED	= 1 << 1	// this connection is authenticated
  , F_TXED_CALLBACK_PENDING	= 1 << 2 //data_sent_cb_pending	// something was sent. Sent Callback is pending. Do not send again.  
  , F_CALLED_BY_RXED_CALLBACK	= 1 << 3 //in_recv_cb	// processing was invoked by Received Callback
  , F_CALLED_BY_TXED_CALLBACK	= 1 << 4 //IN_sent_Cb
  , F_RESB			= 1 << 5
  , F_RESC			= 1 << 6
  , F_RESD			= 1 << 7
  , F_RESE			= 1 << 8
  , F_RESF			= 1 << 9
  , F_RESG			= 1 << 10
  , F_RESH			= 1 << 11
  , F_RESI			= 1 << 12
  , F_RESJ			= 1 << 13
  , F_RESK			= 1 << 14
  , F_RESL			= 1 << 15
  };



// Information Flags - for Connection Control
enum WebIf_CtrlRegA
  { F_THIS_IS_SERVERSOCKET	= 1 << 0	// indicates this is Server Socket (listens for new conn)
  , F_NEEDS_CLOSE		= 1 << 1	// 
  , F_RESXX			= 1 << 2	//   
  , F_RESXZ			= 1 << 3	// 
  , F_RESXA			= 1 << 4
  , F_RESXB			= 1 << 5
  , F_RESXC			= 1 << 6
  , F_RESXD			= 1 << 7
  , F_RESXE			= 1 << 8
  , F_RESXF			= 1 << 9
  , F_RESXG			= 1 << 10
  , F_RESXH			= 1 << 11
  , F_RESXI			= 1 << 12
  , F_RESXJ			= 1 << 13
  , F_RESXK			= 1 << 14
  , F_RESXL			= 1 << 15
  };











// Data structure for Select A
typedef struct
  {
  uint8_t ID;
  const char *Desc;
  } SelectAData;



// Helper for active URL processing
typedef struct {
  const char *UrlSeekPtr;
  char *SrcPtr;
} UrlProcHelper_t;



// List of supported header fields, and callback to process fielddata
typedef struct {
	const char *FldName;
	HdrFldProcCb FldFnCb;
} HdrFlds;







// Information Flags - for response to open connection processing
enum RespFlags
  { F_WAIT_CONN			= 1 << 0 // KEEP_CONN_ALIVE// wait for conn activity
  , F_WAIT_CONN_GET_IDLE_CB	= 1 << 1 // WANTS_IDLE_CB  // wait for conn activity + conn requests an callback - any Callback, or next idle Callback.
  , F_KEEP_CONN_ALIVE     	= 1 << 2 // reset parser + keep connection alive (limited time) for new requests on this connection
  , F_REEXECUTE_PC		= 1 << 3 // reexecute procedure call of this conn	-> not used currently
  , F_F_DISCONNECT_CONN		= 1 << 4 // forces a real disconnect of the ETX Slot conn
  , F_xxxx			= 1 << 5 // cxxx
  , F_e				= 1 << 6
  , F_f				= 1 << 7
  };


// current SlotCgiState (enum) 4BIT!
enum SlotCgiState
  { s_awaiting_request_can_TX = 0	// #00 default at start
  , s_TXed_awaiting_response		// #01
  , s_reserveda				// #02
  , s_reservedb				// #03
  };


// current slot-parser-state (enum) 4BIT!
enum SlotParserState
  { s_just_connected_awaiting_rx = 0	// #00 default at start
  , s_body_is_final			// #01

  , s_HTTPD_Msg_Begin			// #02
  , s_HTTPD_Url_Found			// #03
  , s_HTTPD_Header_Prsd_Complete	// #04
  , s_HTTPD_Fnd_Body_Data		// #05
  , s_HTTPD_Msg_Prsd_Complete		// #06

  , s_ETX_ready_to_TX_request		// #07 default start on ETX Slot
 // , s_ETX_request_TXed_awaiting_resp	// #08

  };





/*
 *   Data structure for implemented query keys and affected readings
 *   used as input for Key=Value parsing, proc SCDEH_ParseStrToQueryResultKF
 */
typedef struct SCDE_QueryKeys_s
  {
  const uint32_t AffectedReadings;
  const char *QueryKey;
  } SCDE_QueryKeys_t;



/*
 *   Result structure for http_parser_parse_url().
 *   Callers should index into field_data[] with UF_* values if field_set
 *   has the relevant (1 << UF_*) bit set. As a courtesy to clients (and
 *   because we probably have padding left over), we convert any port to
 *   a uint32_t.
 */
typedef struct SCDE_XX_QueryResultKF_s
  {
  uint64_t KeyField_Set;		// Bitmask of found keys (1 << XX_QF_*)

  struct KeyField_Data_s
	{
	uint16_t off;			// Offset into buffer in which field starts
	uint16_t len;			// Length of run in buffer
	uint32_t AffectedReadings;	// Key affects capabilities-bits
	} KeyField_Data_t[];		// XX_QF_MAX
  } SCDE_XX_QueryResultKF_t;






// information extraction helpers
int SCDEH_GetSpecialStrVal(const uint8_t *Buf, uint16_t Len, char *Dest, int DestLen, int Type);
bool SCDEH_GetHexUInt32Val(const uint8_t *buf, uint16_t len, uint32_t* value);
bool SCDEH_GetDecUInt8Val(const uint8_t *buf, uint16_t len, uint8_t* value);		// opti
bool SCDEH_GetDecUInt16Val(const uint8_t *buf, uint16_t len, uint16_t* value);	// opti
bool SCDEH_GetDecUInt32Val(const uint8_t *buf, uint16_t len, uint32_t* value);
bool SCDEH_GetDecInt8Val(const uint8_t *buf, uint16_t len, int8_t* value);		// opti
bool SCDEH_GetDecInt16Val(const uint8_t *buf, uint16_t len, int16_t* value);	// opti
bool SCDEH_GetDecInt32Val(const uint8_t *buf, uint16_t len, int32_t* value);
bool SCDEH_GetFloatVal(const uint8_t *buf, uint16_t len, double* value);
bool SCDEH_GetDecInt32Val(const uint8_t *buf, uint16_t len, int32_t* value);
bool SCDEH_GetQueryKeyID(const uint8_t *buf, uint16_t len, uint8_t* Value, SelectAData* ValueTab);
char* SCDEH_AddCmdToWritePos(char* RespArgsWPos, const SCDE_QueryKeys_t* XX_QueryKeys, uint32_t CmdNr);
SCDE_XX_QueryResultKF_t* SCDEH_ParseStrToQueryResultKF(int num, const SCDE_QueryKeys_t *XX_QueryKeys, const char *QueryStr);

// unsorted
int HTTPDGetHeader(WebIf_HTTPDConnSlotData_t *conn, char *header, char *ret, int retLen);



// Value from Key extract helpers
int HTTPDFindStrFromKey(char *query, char *key, char *buff, int buffLen);
int HTTPDFindSpecialStrFromKey(char *query, char *key, void *dest, int destLen, int check);
bool HTTPDTestKey(char *query, char *key);
bool HTTPDFindValueDecInt32FromKey(char *query, char *key, int *buff);
bool HTTPDFindValueDecUInt8FromKey(char *query, char *key, uint8_t *buff);
bool HTTPDFindValueDecUInt16FromKey(char *query, char *key, uint16_t *buff);
bool HTTPDFindValueDecUInt32FromKey(char *query, char *key, uint32_t *buff);
bool HTTPDFindValueHexUInt8FromKey(char *query, char *key, uint8_t *buff);
bool HTTPDFindValueHexUInt32FromKey(char *query, char *key, uint32_t *buff);
bool HTTPDFindValueFloatFromKey(char *query, char *key, float *buff);
bool HTTPDFindValueHexUInt32ColFromKey(char *query, char *key, uint32_t *buff);

//old, to replace
int  httpdFindArgUint8Sel(char *line, char *arg, uint8_t *buff);
//int httpdFindArg(char *line, char *arg, char *buff, int buffLen);

// HTTP cgi functions (build-in) 
//int cgiRedirect(WebIf_HTTPDConnSlotData_t *Conn);
//int cgiRedirectToHostname(WebIf_HTTPDConnSlotData_t *ConnSlotData);
//int cgiRedirectApClientToHostname(WebIf_HTTPDConnSlotData_t *ConnSlotData);


// Get Time Stamp functions for device
//int GetTIST(void);
//int GetUniqueTIST(void);


//---------------------


// HTTPD-Helpers

// Returns the ID of the header field (if in built in table).
int HTTPD_Get_Hdr_Fld_ID(const char* p_at, size_t length);

//Returns the header field value (char* to string) from ID (if stored)
char* HTTPD_Get_Hdr_Val_From_ID(WebIf_HTTPDConnSlotData_t* p_conn, int hdr_fld_id);

int SCDED_UrlDecode(char *val, int valLen, char *dst, int dstLen);
void SCDED_UrlDecode2(char *dst, const char *src);
int SCDED_HexVal(char c);
int SCDED_ParseMimetype(char *ext);
int SCDED_GetMimeTypeID(char *url);
int SCDED_StartRespHeader(WebIf_HTTPDConnSlotData_t *conn, int code);
int SCDED_StartRespHeader2(WebIf_HTTPDConnSlotData_t *conn);
//int SCDED_AddHdrFld(WebIf_HTTPDConnSlotData_t *conn, const char *field, const char *val);
int SCDED_AddHdrFld(WebIf_HTTPDConnSlotData_t *conn, const char *Field, const char *Value, int ValueLength);
int SCDED_EndHeader(WebIf_HTTPDConnSlotData_t *conn);
int SCDED_Redirect(WebIf_HTTPDConnSlotData_t *conn, char *newUrl);
void SCDED_RetireConn(WebIf_HTTPDConnSlotData_t *conn);
WebIf_HTTPDConnSlotData_t * SCDED_FindConnSlot(void *arg);//struct espconn *pespconn); //void *arg);
int SCDED_Send(WebIf_HTTPDConnSlotData_t *conn, const char *data, int len);
void SCDED_TransmitSendBuff(WebIf_HTTPDConnSlotData_t *conn);
bool SCDED_LoadSerializer(WebIf_HTTPDConnSlotData_t *conn);
void SCDED_ProcHdrFldAuth(WebIf_HTTPDConnSlotData_t *conn);
void SCDED_CheckHdrFldAuth(WebIf_HTTPDConnSlotData_t *conn, char* UserInfo, int len);
bool SCDED_AuthCheck(WebIf_HTTPDConnSlotData_t *conn, int ResAuthCheckEna);
void SCDED_SetConCtrl(uint32_t CtrlBitfld);
void SCDED_RespToOpenConn(WebIf_HTTPDConnSlotData_t *conn);




void SCDED_Init(WebIf_ActiveResourcesDataA_t *fixedUrls, WebIf_ActiveResourcesDataB_t *fixedActiveResources, int Port);

void SCDEETX_Connect(void);
void SCDEETX_DNSLookup(void *arg);
void SCDEETX_DNSLookupTimeout(void *arg);
void SCDEETX_IPConnect(const char *name, ip_addr_t *ip, void *arg);
void SCDEETX_InconnTimeoutCb(void *arg);
void SCDEETX_DisconnTimeoutCb(void *arg);
//void HTTPDParseHeader(char *h, WebIf_HTTPDConnSlotData_t *conn);

// no longer required
void SCDED_IdleCbGen(void);

// Platform dependent code should call these ...
void SCDED_IdleCb(void *arg);
void WebIf_SentCb(void *arg);
void WebIf_RecvCb(void *arg, char *recvdata, unsigned short recvlen);
void WebIf_ReconCb(void *arg, int8_t err);
void WebIf_DisconCb(void *arg);
void WebIf_ConnCb(void *arg);

int SCDED_ETXSlotConnCb(void *arg);



const char* SCDE_GetDesc(SelectAData *SAD, uint8_t SelID);






























// Callbacks should return non-zero to indicate an error. The parser will
// then halt execution.
//
// The one exception is on_headers_complete. In a HTTP_RESPONSE parser
// returning '1' from on_headers_complete will tell the parser that it
// should not expect a body. This is used when receiving a response to a
// HEAD request which may contain 'Content-Length' or 'Transfer-Encoding:
// chunked' headers that indicate the presence of a body.
//
// http_data_cb does not return data chunks. It will be called arbitrarily
// many times for each string. E.G. you might get 10 callbacks for "on_url"
// each providing just a few characters more data.
typedef int (*http_data_cb) (WebIf_HTTPDConnSlotData_t *conn, const char *at, size_t length);
typedef int (*http_cb) (WebIf_HTTPDConnSlotData_t *conn);


 
// Possible SCD-Engine Request Methods
#define HTTP_METHOD_MAP(XX)         \
  XX(0,  DELETE,      DELETE)       \
  XX(1,  GET,         GET)          \
  XX(2,  SET,         SET)          \
  XX(3,  POST,        POST)         \
  XX(4,  PUT,         PUT)          \
  XX(5,  CONNECT,     CONNECT)      \
  XX(6,  OPTIONS,     OPTIONS)      \
  XX(7,  TRACE,       TRACE)        \
  XX(8,  COPY,        COPY)         \
  XX(9,  LOCK,        LOCK)         \
  XX(10, MKCOL,       MKCOL)        \
  XX(11, MOVE,        MOVE)         \
  XX(12, PROPFIND,    PROPFIND)     \
  XX(13, PROPPATCH,   PROPPATCH)    \
  XX(14, SEARCH,      SEARCH)       \
  XX(15, UNLOCK,      UNLOCK)       \
  XX(16, BIND,        BIND)         \
  XX(17, REBIND,      REBIND)       \
  XX(18, UNBIND,      UNBIND)       \
  XX(19, ACL,         ACL)          \
  XX(20, REPORT,      REPORT)       \
  XX(21, MKACTIVITY,  MKACTIVITY)   \
  XX(22, CHECKOUT,    CHECKOUT)     \
  XX(23, MERGE,       MERGE)        \
  XX(24, MSEARCH,     M-SEARCH)     \
  XX(25, NOTIFY,      NOTIFY)       \
  XX(26, SUBSCRIBE,   SUBSCRIBE)    \
  XX(27, UNSUBSCRIBE, UNSUBSCRIBE)  \
  XX(28, PATCH,       PATCH)        \
  XX(29, PURGE,       PURGE)        \
  XX(30, MKCALENDAR,  MKCALENDAR)   \
  XX(31, LINK,        LINK)         \
/* SCDE Limit is 32 methods */      \
  XX(32, UNLINK,      UNLINK)       \
  XX(33,  HEAD,        HEAD)        \



enum http_method
  {
#define XX(num, name, string) HTTP_##name = num,
	HTTP_METHOD_MAP(XX)
#undef XX
  };



// operating modes for parser
enum http_parser_type { HTTP_REQUEST, HTTP_RESPONSE, HTTP_BOTH };



// Information Flags - Set while header-parsing (encoded in conn->parser_flags)
enum flags
  { F_CHUNKED               = 1 << 0
  , F_CONNECTION_KEEP_ALIVE = 1 << 1
  , F_CONNECTION_CLOSE      = 1 << 2
  , F_CONNECTION_UPGRADE    = 1 << 3
  , F_TRAILING              = 1 << 4
  , F_UPGRADE               = 1 << 5
  , F_SKIPBODY              = 1 << 6
  , F_CONTENTLENGTH         = 1 << 7
  };


// Table for Enum internal Error Number to external Error-Code + Error-Description
// that will be send in the Response. Maximum 64 chars in the description !
// XX(internal_error_desc, ext.error code, "external description")
#define HTTP_ERRNO_MAP(XX)									\
  /* No error */										\
  XX(OK,			200,	"OK")							\
												\
  /* Callback-related errors */									\
  XX(CB_message_begin,		500,	"the on_message_begin callback failed")			\
  XX(CB_url,			500,	"the on_url callback failed")				\
  XX(CB_header_field,		500,	"the on_header_field callback failed")			\
  XX(CB_header_value,		500,	"the on_header_value callback failed")			\
  XX(CB_headers_complete,	500,	"the on_headers_complete callback failed")		\
  XX(CB_body,			500,	"the on_body callback failed")				\
  XX(CB_message_complete,	500,	"the on_message_complete callback failed")		\
  XX(CB_status,			500,	"the on_status callback failed")			\
  XX(CB_chunk_header,		500,	"the on_chunk_header callback failed")			\
  XX(CB_chunk_complete,		500,	"the on_chunk_complete callback failed")		\
												\
  /* Parsing-related errors */ 									\
  XX(INVALID_EOF_STATE,		500,	"stream ended at an unexpected time")			\
  XX(HEADER_OVERFLOW,		500,	"too many header bytes seen; overflow detected")	\
  XX(CLOSED_CONNECTION,		500,	"data received after completed connection: close message")      \
  XX(INVALID_VERSION,		500,	"invalid HTTP version")					\
  XX(INVALID_STATUS,		500,	"invalid HTTP status code")				\
  XX(INVALID_METHOD,		500,	"invalid HTTP method")					\
  XX(INVALID_URL,		500,	"invalid URL")						\
  XX(INVALID_HOST,		500,	"invalid host")						\
  XX(INVALID_PORT,		500,	"invalid port")						\
  XX(INVALID_PATH,		500,	"invalid path")						\
  XX(INVALID_QUERY_STRING,	500,	"invalid query string")					\
  XX(INVALID_FRAGMENT,		500,	"invalid fragment")					\
  XX(LF_EXPECTED,		500,	"LF character expected")				\
  XX(INVALID_HEADER_TOKEN,	500,	"invalid character in header")				\
  XX(INVALID_CONTENT_LENGTH,	500,	"invalid character in content-length header")		\
  XX(UNEXPECTED_CONTENT_LENGTH, 500,	"unexpected content-length header")			\
  XX(INVALID_CHUNK_SIZE,	500,	"invalid character in chunk size header")		\
  XX(INVALID_CONSTANT,		500,	"invalid constant string")				\
  XX(INVALID_INTERNAL_STATE,	500,	"encountered unexpected internal state")		\
  XX(STRICT,			500,	"strict mode assertion failed")				\
  XX(PAUSED,			500,	"parser is paused")					\
  XX(UNKNOWN,			500,	"an unknown error occurred")				\
  /* Stage 2 specific errors */									\
  XX(HINVALID_URL,		500,	"parser is paused")					\
  XX(NO_AUTH,			500,	"no authorization")					\
  XX(ADID_AOR,			500,	"adid out of range")					\
  XX(UNHANDLED_STATE,		500,	"parser is paused")					\
  XX(INVALID_URLb,		500,	"parser is paused")					\
  XX(INVALID_URLc,		500,	"parser is paused")                                     

/* Define HPE_* values for each errno value above */
#define HTTP_ERRNO_GEN(n ,x ,s) HPE_##n,
enum http_errno {
  HTTP_ERRNO_MAP(HTTP_ERRNO_GEN)
};
#undef HTTP_ERRNO_GEN


// Get an http_errno value from current parsing
#define HTTP_PARSER_ERRNO(p)	((enum http_errno) (p)->parser_http_errno)


// Settings for the HTTP Parser
struct http_parser_settings_s
  {
  http_cb      on_message_begin;
  http_data_cb on_url;
  http_data_cb on_status;
  http_data_cb on_header_field;
  http_data_cb on_header_value;
  http_cb      on_headers_complete;
  http_data_cb on_body;
  http_cb      on_message_complete;
  /* When on_chunk_header is called, the current chunk length is stored
   * in parser->content_length.
   */
  http_cb      on_chunk_header;
  http_cb      on_chunk_complete;
  };



/*
 * WebIf Definition settings (gid auch für childs?!!) //?Information for an HTTPD-Instance?
 */
typedef struct HTTPD_InstanceCfg_s {
  // NOTE: SCDED directory content enable control - if bit 0-31 is set, its equivalent content is  enabled!
  // SCDED_DirConEnaCtrl should be assigned by define in Project Config!
  uint32_t DirConEnaCtrlFB;

  // Load-Serializer-Bitfield, if ANY bit for Slot 1-32 is set by Slot-Cb, its marked
  // as a heavy load task -> it can be used to block other heavy load tasks
  uint32_t LoadSerializer; //LoadSerializerBF

  // Slot Control Register Bitfield, if a bit 1-32 is set, a slot 1-32 is in use!
  uint32_t SlotCtrlRegBF;

  // Pointer to BuildInURls set at init time.
  WebIf_ActiveResourcesDataA_t *BuiltInUrls;
  WebIf_ActiveResourcesDataB_t *BuiltInActiveResources;

  // the http_parser settings for this session
  http_parser_settings_t  HTTPDparser_settings;
} HTTPD_InstanceCfg_t;



// Field Sets generated by http_parser_parse_url().
// will be stored in struct http_parser_url
enum http_parser_url_fields
{   //				   #XX for debugging
    UF_SCHEMA           = 0	// #00
  , UF_HOST             = 1	// #01
  , UF_PORT             = 2	// #02
  , UF_PATH             = 3	// #03
  , UF_QUERY            = 4	// #04
  , UF_FRAGMENT         = 5	// #05
  , UF_USERINFO         = 6	// #06
  , UF_MAX              = 7	// #07
};



// Result structure for http_parser_parse_url().
// Callers should index into field_data[] with UF_* values if field_set
// has the relevant (1 << UF_*) bit set. As a courtesy to clients (and
// because we probably have padding left over), we convert any port to
// a uint16_t.
struct http_parser_url
{
  uint16_t field_set;		// Bitmask of (1 << UF_*) values
  uint16_t port;		// Converted UF_PORT string

  struct
	{
	uint16_t off;		// Offset into buffer in which field starts
	uint16_t len;		// Length of run in buffer
	} field_data[UF_MAX];
};

















/* Returns the library version. Bits 16-23 contain the major version number,
 * bits 8-15 the minor version number and bits 0-7 the patch level.
 * Usage example:
 *
 *   unsigned long version = http_parser_version();
 *   unsigned major = (version >> 16) & 255;
 *   unsigned minor = (version >> 8) & 255;
 *   unsigned patch = version & 255;
 *   printf("http_parser v%u.%u.%u\n", major, minor, patch);*/
unsigned long http_parser_version(void);

//void http_parser_init(WebIf_HTTPDConnSlotData_t *conn,
//	enum http_parser_type type);

// Inits the Parser for an new Request only / Response only / or both 
void HTTPD_ParserInit (WebIf_HTTPDConnSlotData_t *conn,
			enum http_parser_type pType);

/* Initialize http_parser_settings members to 0 */
void http_parser_settings_init(http_parser_settings_t *settings);

/* Executes the parser. Returns number of parsed bytes. Sets
 * `parser->http_errno` on error. */
size_t http_parser_execute(WebIf_HTTPDConnSlotData_t *conn,
	const http_parser_settings_t *settings,
	const char *data,
	size_t len);

/* If http_should_keep_alive() in the on_headers_complete or
 * on_message_complete callback returns 0, then this should be
 * the last message on the connection.
 * If you are the server, respond with the "Connection: close" header.
 * If you are the client, close the connection.*/
int http_should_keep_alive(const WebIf_HTTPDConnSlotData_t *conn);

/* Returns a string version of the HTTP method. */
const char *http_method_str(enum http_method m);

/* Return the associated external code to the given internal error-no */
const unsigned int HTTPD_ErrnoCode(enum http_errno err);

/* Return the associated description to the given internal error-no */
const char* HTTPD_ErrnoDescription(enum http_errno err);




/* Initialize all http_parser_url members to 0 */
void http_parser_url_init(struct http_parser_url *u);

/* Parse a URL; return nonzero on failure */
int http_parser_parse_url(const char *buf,
	size_t buflen,
	int is_connect,
	struct http_parser_url *u);

/* Pause or un-pause the parser; a nonzero value pauses */
void http_parser_pause(WebIf_HTTPDConnSlotData_t *conn,
	int paused);

/* Checks if this is the final chunk of the body. */
int http_body_is_final(const WebIf_HTTPDConnSlotData_t *conn);

// Analyzes the path from HTTP-Parser and tries to find + load a matching build in Resource
void HTTPD_ParseUrl(WebIf_HTTPDConnSlotData_t *conn);



//--------------



// HTTPD parser integrated callbacks (installed in init) + Procs

// Callback fired by HTTPDParser when message begins
int HTTPD_On_Message_Begin_Cb(WebIf_HTTPDConnSlotData_t *conn);

// Callback fired by HTTPDParser when url is found, used for parsing URI into elements
int HTTPD_On_Url_Cb(WebIf_HTTPDConnSlotData_t *conn, const char *at, size_t length);

// Callback fired by HTTPDParser when a header field is found
int HTTPD_On_Header_Field_Cb(WebIf_HTTPDConnSlotData_t* p_conn, const char* p_at, size_t length);

// Callback fired by HTTPDParser when a header value is found
int HTTPD_On_Header_Value_Cb(WebIf_HTTPDConnSlotData_t* p_conn, const char* p_at, size_t length);

// Proc to process stored header field (name+value)
void HTTPD_Store_Header_Field(WebIf_HTTPDConnSlotData_t *conn);

// Callback fired by HTTPDParser when header is complete parsed
int HTTPD_On_Headers_Complete_Cb(WebIf_HTTPDConnSlotData_t *conn);

// Callback fired by HTTPDParser when body-data is found
int HTTPD_On_Body_Cb(WebIf_HTTPDConnSlotData_t *conn, const char *at, size_t length);

// Callback fired by HTTPDParser when message parsing is complete (after body data)
int HTTPD_On_Message_Complete_Cb(WebIf_HTTPDConnSlotData_t *conn);



//--------------



// Helper
char * HTTPDStoreAddDataB(char *Buff,int *Len,const uint8_t *data,size_t length);

char* HTTPDStoreAddData(char *Buff, const uint8_t *data, size_t length);


int SCDED_build_select_input_A(char *buff, SelectAData *SAD, uint8_t SelID, char *name, char *id, char *label);
void SelectA(char buff[128], SelectAData *SAD, uint8_t SelID, char *Name, char *CGI);
void RadioA(char *buff, SelectAData *SAD, uint8_t SelID, char *Name, char *CGI);
void CheckboxA(char *buff, SelectAData *SAD, uint8_t SelID, char *Name, char *CGI);


// Header field parse callbacks
// Callback is fired when a value for header field "Authorization" is found, to parse it
int HTTPDHdrFldAuthFnCb(WebIf_HTTPDConnSlotData_t *conn, const char *at, size_t length);
// Callback is fired when a value for header field "Host" is found, to parse it
int HTTPDHdrFldHostFnCb(WebIf_HTTPDConnSlotData_t *conn, const char *at, size_t length);



#endif /*MODULEWEBIF_H*/
