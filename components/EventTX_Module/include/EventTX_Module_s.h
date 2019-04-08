// SCDE-Module EVENTTX_Module (IO-Device for SPI Hardware)

#ifndef EVENTTX_MODULE_S_H
#define EVENTTX_MODULE_S_H

// -------------------------------------------------------------------------------------------------

// this Module is made for the Smart-Connected-Device-Engine
#include "SCDE_s.h"

// this Module provides functions for other Modules:
#include "WebIf_Module global types.h"
//#include "WebIf_Module_s.h"

// this Module uses an 1st stage:
// -- no ---

// -------------------------------------------------------------------------------------------------










// -------------------------------------------------------------------------------------------------


/* 
 * Entry EventTX Definition (struct) stores values for operation valid only for the defined instance of an
 * loaded module. Values are initialized by SCDE an the loaded module itself.
 */
typedef struct Entry_EventTX_Definition_s {

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

  int EventTX_CtrlRegA;			// EventTX Control-Reg-A (enum EventTX_CtrlRegA from WEBIF.h)

  HTTPD_InstanceCfg_t* HTTPD_InstCfg;	// link to configuration of this HTTPD-Instance
 
  uint8_t slot_no;			// slot number in this instance

} Entry_EventTX_Definition_t;


// -------------------------------------------------------------------------------------------------

/* 
 * Definition typedef stores values for operation valid only for the defined instance of an
 * loaded module. Values are initialized by HCTRLD an the loaded module itself.
 */
typedef struct EventTX_HTTPDConnSlotData_s { //HTTPD_Conn_Slot

  Entry_WebIf_Definition_t* conn;	// Link to lower level connection management

//- V V V V V V V V V V V V V V V V V V // TX-Helper

  char* p_send_buffer;//uint8_t*	// Pointer to current allocated send buffer w. size [MAX_SB_LEN] (NULL=NO Send-Buffer)
  int send_buffer_write_pos; //uint16_t	// Send buffer, current write position (offset), !> 0 = its allocated!
  char* p_trailing_buffer;//uint8_t*	// An 'trailing_buffer' in case of 'send_buffer' overflow. Its prio for next transmission!
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

//  union { //parsing / processing
// temporary data - during header field parsing
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

  STAILQ_HEAD (stailheadx, Entry_Header_Field_s) head_header_field;// REQUEST & RESPONSE - Stores Header-Fields in an SLTQ

//-

  char* url;				// FROM REQUEST ONLY - Ptr to allocated memory filled with PATH extracted from URI
  char* AltFSFile;			// FROM REQUEST ONLY - (NULL = Filename from Path!) else Ptr. to alternative filename in FS
  char* DestUrl;//MatchedUrl		// FROM REQUEST ONLY - Ptr to BuildInURL that matches (Active URL with Tokens for reconstruction)
  char* getArgs;//Query			// FROM REQUEST ONLY - Ptr to allocated memory filled with QUERY (GETARGS) extracted from URI, if any
  char* hostName;			// FROM REQUEST ONLY - content of host name field

//-

  char* p_body_data;//uint8_t*		// REQUEST & RESPONSE - Pointer to BdyData Buffer
  int body_data_len;//uint16_t		// REQUEST & RESPONSE - length of stored body data
//int postPos;	//BodyPos benötigt?	// counter for post position (contains whole post data len, not only buffer!)
} EventTX_HTTPDConnSlotData_t;



// --------------------------------------------------------------------------------------------------



/*
 * typedefs of ESP32_SPI_Module Function Callbacks
 * This Fn are provided & made accessible for client modules - for operation
 */
// typedef for ESP32_SPI_Module_spi_bus_add_deviceFn - adds an device to the definitions host
//typedef strTextMultiple_t*  (*ESP32_SPI_bus_add_deviceFn_t) (ESP32_SPI_Definition_t* ESP32_SPI_Definition, const ESP32_SPI_device_interface_config_t *dev_config, ESP32_SPI_device_handle_t *handle);



/*
 * ESP32 SPI provided Fn - table
 * Stores common + custom functions this Module provides to the SCDE (and client Modules)

 */
typedef struct EventTX_ProvidedByModule_s {
// --- first the provided common module functions ---
  ProvidedByModule_t common;							// the common fn
// --- now the provided custom module fuctions ---
//  ESP32_SPI_bus_add_deviceFn_t ESP32_SPI_bus_add_deviceFn;			// adds an device to the definitions host
} EventTX_ProvidedByModule_t;



#endif /*EVENTTX_MODULE_S_H*/

