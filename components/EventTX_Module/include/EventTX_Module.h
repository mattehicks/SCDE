// SCDE-Module EventTX_Module (IO-Device for SPI-Hardware)

#ifndef EVENTTX_MODULE_H
#define EVENTTX_MODULE_H

// -------------------------------------------------------------------------------------------------

// this Module is made for the Smart-Connected-Device-Engine
#include "SCDE_s.h"

// this Module provides functions for other Modules:
#include "WebIf_Module global types.h"
//#include "WebIf_Module_s.h"

// this Module uses an 1st stage:
// -- no ---

// -------------------------------------------------------------------------------------------------

// stores the Root Data of the Smart Connected Devices Engine - at/for this Module
SCDERoot_t* SCDERoot_at_EventTX_M;

// stores SCDEFn (Functions / callbacks) provided for operation - at/for this Module
SCDEFn_t* SCDEFn_at_EventTX_M;

// -------------------------------------------------------------------------------------------------


















// -------------------------------------------------------------------------------------------------



/*
 *  Functions provided to SCDE by Module - for type operation (A-Z)
 */

//
strTextMultiple_t* EventTX_Add(Common_Definition_t* Common_Definition, uint8_t *kvArgs, size_t kvArgsLen);

//
strTextMultiple_t* EventTX_Define(Common_Definition_t *Common_Definition);

//
int EventTX_Direct_Read(Common_Definition_t*  p_entry_definition);

//
int EventTX_Direct_Write(Common_Definition_t*  p_entry_definition);

//
int EventTX_IdleCb(Common_Definition_t* Common_Definition);

//
int EventTX_Initialize(SCDERoot_t* SCDERoot);

//
strTextMultiple_t* EventTX_Set(Common_Definition_t* Common_Definition, uint8_t *setArgs, size_t setArgsLen);

//
strTextMultiple_t* EventTX_Sub(Common_Definition_t* Common_Definition , uint8_t *kArgs, size_t kArgsLen);

//
strTextMultiple_t* EventTX_Undefine(Common_Definition_t* Common_Definition);



// -------------------------------------------------------------------------------------------------





/*
 *  helpers provided to module for type operation
 */
/*
strTextMultiple_t* ESP32_SPI_ProcessKVInputArgs(ESP32_SPI_Definition_t* ESP32_SPI_Definition, parsedKVInputArgs_t *parsedKVInput, uint8_t *argsText, size_t argsTextLen);

bool ESP32_SPI_SetAffectedReadings(ESP32_SPI_Definition_t* ESP32_SPI_Definition, uint64_t affectedReadings);
*/







/*
 *  helpers provided to module for type operation
 */
//void WebIf_disconnect(Entry_WebIf_Definition_t *WebIf_Definition);
void EventTX_Regist_Connect_Cb(Entry_EventTX_Definition_t* conn, espconn_connect_callback p_connect_cb);
void EventTX_Regist_Disconn_Cb(Entry_EventTX_Definition_t* conn, espconn_connect_callback p_disconn_cb);
void EventTX_Regist_Sent_Cb(Entry_EventTX_Definition_t* conn, espconn_sent_callback p_sent_cb);
void EventTX_Regist_Recv_Cb(Entry_EventTX_Definition_t* conn, espconn_recv_callback p_recv_cb);
void EventTX_Regist_Recon_Cb(Entry_EventTX_Definition_t* conn, espconn_reconnect_callback reconn_cb);
//int  WebIf_sent(Entry_WebIf_Definition_t* p_entry_webif_definition, uint8_t* send_buffer, uint send_buffer_len);

//----------------- old stuff ------------------------










// -------------------------------------------------------------------------------------------------

/*
 * Platform dependent code should call these ...
 */

//
void EventTX_Connect_Cb (void* arg);

//
void EventTX_Disconnect_Cb (void* arg);

//
void EventTX_Reconnect_Cb (void* arg, int8_t err);

//
void EventTX_Idle_Cb (void* arg);

//
void EventTX_Sent_Cb (void* arg);

//
void EventTX_Received_Cb (void* arg, char* p_recv_data, unsigned short recv_data_len);


// -------------------------------------------------------------------------------------------------













#endif /*EVENTTX_MODULE_H*/


