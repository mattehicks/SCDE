/**
 * SCDE environmental and task control functions.
 */



#if defined(ESP_PLATFORM)

#include <esp_err.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
//#include <spiffs.h>

#include "lwip/err.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/sockets.h"

//#include "duktape_spiffs.h"
//#include "esp32_specific.h"
#include "sdkconfig.h"

#endif // ESP_PLATFORM

#include <assert.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>

#include "scde_task.h"
#include "logging.h"
#include "SCDE_s.h"
#include "SCDE_Main.h"
#include "SCDE.h"





LOG_TAG("scde_task");



// The Duktape context.
//duk_context *esp32_duk_context;





/**
 * Initialize the duktape environment.
 */
void
scde_init_environment() {


  LOGD("About to create heap");

  // init the SCDE-Engine
  InitSCDERoot();

  Log3( (const uint8_t*)"test!!!"
  		,4 //chck
		,1
		,"Test:%.*s\n"
		,3
		,"aabcd");



 // Activate Help as SCDE built-in Command
  extern ProvidedByCommand_t Help_ProvidedByCommand;
  CommandActivateCommand(&Help_ProvidedByCommand);

}



/**
 * Start the duktape processing.
 *
 * Here we loop waiting for a line of input to be received and, when
 * it arrives, we process it through an eval().  The input comes from
 * a call to readLine() which insulates us from the actual source of
 * the line.
 */
void
scde_task(void *ignore) {


	LOGD(">> duktape_task");


	// Mount the SPIFFS file system.
#if defined(ESP_PLATFORM)
//	esp32_duktape_spiffs_mount();
#endif /* ESP_PLATFORM */


	scde_init_environment();
	// From here on, we have a Duktape context ...

  //SelectQueryTask(NULL);
}




/*
 * --------------------------------------------------------------------------------------------------
 *  FName: Platform_SelectQueryTask
 *  Desc: Task that selects the FD (read,write,exception) and creates read + write + exept. callbacks
 *  Para: void *pvParameters -> ?
 *  Rets: -/-
 * --------------------------------------------------------------------------------------------------
 */
/*
void
SelectQueryTask(void *ignore) {

  Common_Definition_t *Def;
  int sd;

  // helper to finding highest file descriptor
  int max_fdp;

  // read socket descriptors set
  fd_set readfds;

  // write socket descriptors set
  fd_set writefds;

  // exception socket descriptors set
  fd_set exceptfds;

  LOGD("debug1:%d  2:%d.\n"
      ,FD_SETSIZE, sizeof(readfds) );

  struct timeval timeout;
  // Initialize the timeout data structure
  timeout.tv_sec = 1;//seconds;
  timeout.tv_usec = 0;

  // ???
//  SCDEMux = xSemaphoreCreateRecursiveMutex();

  // ??
  int ret;
	
  LOGD("SCDE: startig global select loop.\n");

  // endless loop - blocked by select (depends on sets)
  while(1) {

      // list currently stored modules

      Common_Definition_t *Common_Definition;
      STAILQ_FOREACH(Common_Definition, &SCDERoot.HeadCommon_Definitions, entries) {

         LOGD("L Module:%.*s Name:%.*s FD:%d\n"
            ,Common_Definition->module->ProvidedByModule->typeNameLen
            ,Common_Definition->module->ProvidedByModule->typeName
            ,Common_Definition->nameLen
            ,Common_Definition->name
            ,Common_Definition->fd);
         }


	// clear highest file descriptor
	max_fdp = 0;

	// clear the read socket set
	FD_ZERO(&readfds);

	// clear the write socket set
	FD_ZERO(&writefds);

	// clear the exception socket set
	FD_ZERO(&exceptfds);
   

	// loop through definitions with a valid socket descriptor to add to FD to sets
	STAILQ_FOREACH(Def, &SCDERoot.HeadCommon_Definitions, entries) {

		// file descriptor / socket descriptor of this definition
		sd = Def->fd;

		// if valid socket descriptor then may be we need to add to sets
		if (sd != -1) {

			 LOGD("SCDE: adding %.*s fd:%d\n",Def->nameLen,Def->name,Def->fd);

			// select for reading, but only if an Fn is installed
			if (Def->module->ProvidedByModule->DirectReadFn) {

				FD_SET(sd , &readfds);

				}

			// select for writing (if wants to write), but only if an Fn is installed
			if  ( (Def->Common_CtrlRegA & F_WANTS_WRITE)
				&& (Def->module->ProvidedByModule->DirectWriteFn) ) {

				FD_SET(sd , &writefds);

				}

			// select for exceptions, but only if an Fn is installed
			if (Def->module->ProvidedByModule->ExceptFn) {

				FD_SET(sd , &exceptfds);

				}

			// find highest file descriptor number, need it for the select function
			// calculation after each FD_SET?
			if (sd > max_fdp) max_fdp = sd;

			}
		}

	// execute the selection
	ret = select(max_fdp + 1, &readfds, &writefds, &exceptfds, &timeout);

	LOGD("|SCDE: passed global select>");

	// did we get any?
	if (ret > 0) {

		LOGD("SCDE: got any fd from select\n");

		// loop through definitions with a valid socket descriptor to check and execute callbacks
		STAILQ_FOREACH(Def, &SCDERoot.HeadCommon_Definitions, entries) {

			   LOGD("SCDE: ok fnd select\n");

			// file descriptor / socket descriptor of this definition
			sd = Def->fd;

			// if valid socket descriptor then check and execute callbacks
			if (sd != -1) {

				// first check for write availability, but only if 'F_WANTS_WRITE' and 'DirectWriteFn' is installed
				if ( (Def->Common_CtrlRegA & F_WANTS_WRITE)
					&& (Def->module->ProvidedByModule->DirectWriteFn)
					&& (FD_ISSET(sd , &writefds)) ) {

					// clear Flag F_WANTS_WRITE, should be set again when more data should be sended
					Def->Common_CtrlRegA &= ~F_WANTS_WRITE;

					// execute the write function
					Def->module->ProvidedByModule->DirectWriteFn((Common_Definition_t*) Def);
					
					}

				// second check for read availability. But only if a function is installed
				if ( (Def->module->ProvidedByModule->DirectReadFn)
					&& (FD_ISSET(sd , &readfds)) ) {

					// execute the read function
					Def->module->ProvidedByModule->DirectReadFn((Common_Definition_t*) Def);

					}

				// third check for exceptions. But only if a function is installed
				if ( (Def->module->ProvidedByModule->ExceptFn)
					&& (FD_ISSET(sd , &exceptfds)) ) {

					// execute the exception function
					Def->module->ProvidedByModule->ExceptFn((Common_Definition_t*) Def);

					}

				}
			}
		}
	}
  }
*/


/*
 * --------------------------------------------------------------------------------------------------
 *  FName: Platform_IdleCbTask
 *  Desc: Task that provides the Idle-Callbacks
 *  Para: void *pvParameters -> ?
 *  Rets: -/-
 * --------------------------------------------------------------------------------------------------
 */
/*
void
IdleCbTask(void *ignore) {

  Common_Definition_t *Common_Definition;

  // endless loop - blocked by an delay
  while(1) {

	LOGD("\nNow IdleCbs>");

	// loop through definitions
	STAILQ_FOREACH(Common_Definition, &SCDERoot.HeadCommon_Definitions, entries) {

		// check if F_WANTS_IDLE_TASK is set and IdleCbFn Fn is installed
		if ( (Common_Definition->module->ProvidedByModule->IdleCbFn)
			&& (Common_Definition->Common_CtrlRegA & F_WANTS_IDLE_TASK) ) {

			 LOGD("SCDE: Exec IdleCbFn!! Type-Name:%.*s Def-Name:%.*s FD:%d\n"
				,Common_Definition->module->ProvidedByModule->typeNameLen
				,Common_Definition->module->ProvidedByModule->typeName
				,Common_Definition->nameLen
				,Common_Definition->name
				,Common_Definition->fd);

			// clear Flag F_WANTS_WRITE, should be set again when more data should be sended
			Common_Definition->Common_CtrlRegA &= ~F_WANTS_IDLE_TASK;


			// execute the idle callback function
			Common_Definition->module->ProvidedByModule->IdleCbFn((Common_Definition_t*) Common_Definition);

			}

		}

#if defined(ESP_PLATFORM)
        vTaskDelay(100 / portTICK_PERIOD_MS);
#endif // ESP_PLATFORM

	}

}
*/

