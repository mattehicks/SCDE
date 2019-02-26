// WebIf_Module structures

#ifndef WEBIFMODULE_S_H
#define WEBIFMODULE_S_H


//#include "SCDE_s.h"

// callback proc for ... ?
typedef int (* PCallback)(WebIf_HTTPDConnSlotData_t *connData);








/*
 *  WebIf - Data Table A for Active Resources - The content of the Active Directory - PART A (Resource-Data-Row) 
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








#endif /*WEBIFMODULE_S_H*/
