
#ifndef WEBIFMODULEGLOBAL_TYPES_H
#define WEBIFMODULEGLOBAL_TYPES_H


//#include "WebIf_Module.h"




// typedefs for WebIf_FnProvided (Functions (callbacks) provided by WebIf for Modules - to execute implemented WebIf Fn)
//typedef void (* LogFnx_t) (char *Device, int LogLevel, char *Text);
//typedef void (* Log3Fnx_t) (const uint8_t *name, const size_t nameLen, const uint8_t LogLevel, const char *format, ...);

/*
 * WebIf_FnProvided (Functions / callbacks) for operation, provided / made accessible by WebIf for modules
 */
typedef struct WebIf_FnProvided_s
  {

//  LogFnx_t LogFnx;					// Log -> This is the main logging function

  Log3Fn_t Log3Fn;					// Log -> This is the main logging function

} WebIf_FnProvided_t;


/* 
 * WebIf_Definition stores values for operation valid only for the defined instance of an
 * loaded module. Values are initialized by HCTRLD an the loaded module itself.
 */
typedef struct WebIf_Provided_s
  {

  // FnProvider (Functions / callbacks) for operation
  WebIf_FnProvided_t *WebIf_FnProvided;

  uint32_t *link_nextProvided;

  // Pointer to ActiveResourcesDataA, set at init time.
  WebIf_ActiveResourcesDataA_t *ActiveResourcesDataA;
  // Pointer to ActiveResourcesDataB, set at init time.
  WebIf_ActiveResourcesDataB_t *ActiveResourcesDataB;

  } WebIf_Provided_t;


#endif /*WEBIFMODULEGLOBAL_TYPES_H*/

