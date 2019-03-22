
// smart connected device engine - main




#include "ProjectConfig.h"

#include <esp8266.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"


#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/ledc.h"
#include "lwip/err.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/sockets.h"




#include "vfs.h"



#include "esp_task_wdt.h"


#include "SCDE_s.h"
#include "SCDE_Main.h"
#include "SCDE.h"

#include "Espfs.h"



#include "scde_task.h"








//#include "esp_spi_flash.h"
//#include "esp_partition.h"
//#include "spiffs.h"
//#include "esp_spiffs.h"
//#include "ESP32_Platform_SPIFFS.h"







#include <freertos/FreeRTOS.h>
//#include <freertos/include/freertos/heap_regions.h>
#include <freertos/task.h>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_heap_alloc_caps.h>
#include <nvs_flash.h>

#include "scde_task.h"
#include "logging.h"
#include "sdkconfig.h"


LOG_TAG("SCDE_Main");









/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
 but we only care about one event - are we connected
 to the AP with an IP? */
static const int CONNECTED_BIT = BIT0;

// test for debug
static const char *TAG = "example";


/*
 *  address of espfs file system binary - included by linker
 */
//extern uint32_t _binary_x_img_start;
//extern uint32_t _binary_espfs_img_start;
//extern const uint8_t espfs_img_start[] asm("_binary_espfs_img_start");



#define HTTPD_STACKSIZE 8000//1000
#define HTTPD_MAX_CONNECTIONS 10

/*#include <esp8266.h>
#include "httpd.h"
#include "platform.h"
#include "httpd-platform.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"*/





static int httpPort;
static int httpMaxConnCt;
static xQueueHandle httpdMux;


struct  RtosConnType{
	int fd;
	int needWriteDoneNotif;
	int needsClose;
	int port;
	char ip[4];
};

static RtosConnType rconn[HTTPD_MAX_CONNECTIONS];
































































/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDE_read_rom_uint8
 * Desc: Reads an uint8 from unaligned flash (to prevent exception)
 * Para: const uint8* addr -> prt to adress to read
 * Rets: uint8 -> read from flash
 *--------------------------------------------------------------------------------------------------
 */
uint8_t ICACHE_FLASH_ATTR
SCDE_read_rom_uint8(const uint8_t* addr)
  {

  uint32_t bytes;

  bytes = *(uint32_t*)((uint32_t)addr & ~3);

  return ((uint8_t*)&bytes)[(uint32_t)addr & 3];

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDE_read_rom_uint16
 * Desc: Reads an uint16 from unaligned flash (to prevent exception)
 * Para: const uint16* addr -> prt to adress to read
 * Rets: uint16 -> read from flash
 *--------------------------------------------------------------------------------------------------
 */
uint16_t ICACHE_FLASH_ATTR
SCDE_read_rom_uint16(const uint16_t* addr)
  {

  uint32_t bytes;

  bytes = *(uint32_t*)((uint32_t)addr & ~3);

  return ((uint16_t*)&bytes)[((uint32_t)addr >> 1) & 1];

  }
 

 //   os_printf("%d\r\n", read_rom_uint8(&data[0]));
   // os_printf("%d\r\n", read_rom_uint8(data + 13));
     //os_printf("%d\r\n", read_rom_uint16(&data16[1]));


/*
 *--------------------------------------------------------------------------------------------------
 *FName: os_memcpyplus
 * Desc: memcpy from unaligned flash to ram (to prevent exception)
 * Para: char *dst -> prt to destination
 *       char *src -> prt to source data in flash
 *       int len -> max len to copy
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR
SCDE_memcpy_plus(char *dst
		,const char *src
		,int len)
  {

  while (len--) *dst++ = (char) SCDE_read_rom_uint8((uint8_t*)src++);

  }





























int ICACHE_FLASH_ATTR
httpdPlatSendData(ConnTypePtr conn, char *buff, int len)
{
	conn->needWriteDoneNotif=1;
	return (write(conn->fd, buff, len)>=0);
}



void ICACHE_FLASH_ATTR
httpdPlatDisconnect(ConnTypePtr conn)
{
	conn->needsClose=1;
	conn->needWriteDoneNotif=1; //because the real close is done in the writable select code
}




void 
httpdPlatDisableTimeout(ConnTypePtr conn)
{
	//Unimplemented for FreeRTOS
}



//Set/clear global httpd lock.
void ICACHE_FLASH_ATTR httpdPlatLock() {
	xSemaphoreTakeRecursive(httpdMux, portMAX_DELAY);
}



void ICACHE_FLASH_ATTR 
httpdPlatUnlock()
{
	xSemaphoreGiveRecursive(httpdMux);
}





/**
 * -------------------------------------------------------------------------------------------------
 *  FName: platHttpServerTask
 *  Desc: 
 *  Info: 
 *  Para: -/- 
 *  Rets: -/-
 * -------------------------------------------------------------------------------------------------
 */
#define RECV_BUF_SIZE 2048
static void 
platHttpServerTask(void *pvParameters)
{
  int32_t listenfd;
  int32_t remotefd;
  int32_t len;
  int32_t ret;
  int x;
  int maxfdp = 0;
  char *precvbuf;
  fd_set readset,writeset;
  struct sockaddr name;

  //struct timeval timeout;
  struct sockaddr_in server_addr;
  struct sockaddr_in remote_addr;
	
  httpdMux = xSemaphoreCreateRecursiveMutex();
	
  for (x=0; x<HTTPD_MAX_CONNECTIONS; x++) {

		rconn[x].fd = -1;
  }
	
	// Construct local address structure
	memset(&server_addr, 0, sizeof(server_addr)); // zero out structure
	server_addr.sin_family = AF_INET;							// internet address family
	server_addr.sin_addr.s_addr = INADDR_ANY;			// any incoming interface
	server_addr.sin_len = sizeof(server_addr);  
	server_addr.sin_port = htons(81);//httpPort); // local port

  // Create socket for incoming connections
	do {

		listenfd = socket(AF_INET, SOCK_STREAM, 0);

		if (listenfd == -1) {

			os_printf("platHttpServerTask: failed to create sock!\n");

			vTaskDelay(1000/portTICK_RATE_MS);
		}

  } while (listenfd == -1);

	// bind to the local port
	do {

		ret = bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

		if (ret != 0) {

			os_printf("platHttpServerTask: failed to bind!\n");

			vTaskDelay(1000/portTICK_RATE_MS);
		}

	} while(ret != 0);

	// Listen to the local connection
	do {

		ret = listen(listenfd, HTTPD_MAX_CONNECTIONS);

		if (ret != 0) {

			os_printf("platHttpServerTask: failed to listen!\n");

			vTaskDelay(1000/portTICK_RATE_MS);
		}

	} while(ret != 0);
	
	os_printf("esphttpd: active and listening to connections.\n");

	while (1) {

		// clear fdset, and set the select function wait time
		int socketsFull=1;
		maxfdp = 0;
		FD_ZERO(&readset);
		FD_ZERO(&writeset);
		//timeout.tv_sec = 2;
		//timeout.tv_usec = 0;
		
		for (x=0 ; x<HTTPD_MAX_CONNECTIONS ; x++) {

			if (rconn[x].fd != -1) {

				FD_SET(rconn[x].fd, &readset);

				if (rconn[x].needWriteDoneNotif) FD_SET(rconn[x].fd, &writeset);

				if (rconn[x].fd>maxfdp) maxfdp=rconn[x].fd;

			} else {

				socketsFull = 0;
			}
		}
		
		if (!socketsFull) {
			FD_SET(listenfd, &readset);
			if (listenfd > maxfdp) maxfdp = listenfd;
		}

		// polling all exist client handle,wait until readable/writable
		ret = select(maxfdp+1, &readset, &writeset, NULL, NULL);//&timeout

		if (ret > 0) {

			os_printf("action!\n");

			// see if we need to accept a new connection
			if (FD_ISSET(listenfd, &readset)) {

				len = sizeof(struct sockaddr_in);

				remotefd = accept(listenfd, (struct sockaddr *)&remote_addr, (socklen_t *)&len);

				if (remotefd < 0) {

					os_printf("platHttpServerTask: Huh? Accept failed.\n");

					continue;
				}

				for (x=0 ; x<HTTPD_MAX_CONNECTIONS ; x++) if (rconn[x].fd == -1) break;

				if (x == HTTPD_MAX_CONNECTIONS) {

					os_printf("platHttpServerTask: Huh? Got accept with all slots full.\n");

					continue;
				}

				int keepAlive = 1; // enable keepalive
				int keepIdle = 60; // 60s
				int keepInterval = 5; // 5s
				int keepCount = 3; // retry times
					
				setsockopt(remotefd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepAlive, sizeof(keepAlive));
				setsockopt(remotefd, IPPROTO_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle));
				setsockopt(remotefd, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval));
				setsockopt(remotefd, IPPROTO_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));
				
				rconn[x].fd = remotefd;
				rconn[x].needWriteDoneNotif = 0;
				rconn[x].needsClose = 0;
				
				len = sizeof(name);
				getpeername(remotefd, &name, (socklen_t *) &len);
				struct sockaddr_in *piname = (struct sockaddr_in *) &name;

				rconn[x].port = piname->sin_port;
				memcpy(&rconn[x].ip, &piname->sin_addr.s_addr, sizeof(rconn[x].ip));

				//httpdConnectCb(&rconn[x], rconn[x].ip, rconn[x].port);
				//os_timer_disarm(&connData[x].conn->stop_watch);
				//os_timer_setfn(&connData[x].conn->stop_watch, (os_timer_func_t *)httpserver_conn_watcher, connData[x].conn);
				//os_timer_arm(&connData[x].conn->stop_watch, STOP_TIMER, 0);
//				httpd_printf("httpserver acpt index %d sockfd %d!\n", x, remotefd);
			}
			
			// see if anything happened on the existing connections.
			for (x = 0 ; x < HTTPD_MAX_CONNECTIONS ; x++) {

				// skip empty slots
				if (rconn[x].fd == -1) continue;

				os_printf("aaah\n");

				//Check for write availability first: the read routines may write needWriteDoneNotif while
				//the select didn't check for that.
				if (rconn[x].needWriteDoneNotif && FD_ISSET(rconn[x].fd, &writeset)) {

					rconn[x].needWriteDoneNotif=0; //Do this first, httpdSentCb may write something making this 1 again.
					if (rconn[x].needsClose) {

						//Do callback and close fd.

						//httpdDisconCb(&rconn[x], rconn[x].ip, rconn[x].port);

						close(rconn[x].fd);

						rconn[x].fd=-1;

					} else {

						//httpdSentCb(&rconn[x], rconn[x].ip, rconn[x].port);

					}
				}

				if (FD_ISSET(rconn[x].fd, &readset)) {

					precvbuf = (char*)malloc(RECV_BUF_SIZE);

					if (precvbuf == NULL) {

						os_printf("platHttpServerTask: memory exhausted!\n");

						//httpdDisconCb(&rconn[x], rconn[x].ip, rconn[x].port);

						close(rconn[x].fd);

						rconn[x].fd=-1;
					}

					ret = recv(rconn[x].fd, precvbuf, RECV_BUF_SIZE,0);

					if (ret > 0) {

						//Data received. Pass to httpd.
						//httpdRecvCb(&rconn[x], rconn[x].ip, rconn[x].port, precvbuf, ret);

					} else {

						//recv error,connection close
						//httpdDisconCb(&rconn[x], rconn[x].ip, rconn[x].port);

						close(rconn[x].fd);

						rconn[x].fd=-1;
					}

					if (precvbuf) free(precvbuf);
				}
			}
		}
	}

#if 0
//Deinit code, not used here.
	/*release data connection*/
	for(x=0; x < HTTPD_MAX_CONNECTIONS; x++){
		//find all valid handle 
		if(connData[x].conn == NULL) continue;
		if(connData[x].conn->sockfd >= 0){
			os_timer_disarm((os_timer_t *)&connData[x].conn->stop_watch);
			close(connData[x].conn->sockfd);
			connData[x].conn->sockfd = -1;
			connData[x].conn = NULL;
			if(connData[x].cgi!=NULL) connData[x].cgi(&connData[x]); //flush cgi data
			httpdRetireConn(&connData[x]);
		}
	}
	/*release listen socket*/
	close(listenfd);
	vTaskDelete(NULL);
#endif
}



//Initialize listening socket, do general initialization
void ICACHE_FLASH_ATTR 
httpdPlatInit(int port, int maxConnCt) 
  {
	httpPort=port;
	httpMaxConnCt=maxConnCt;

#ifdef ESP32
	xTaskCreate(platHttpServerTask, (const char *)"esphttpd", HTTPD_STACKSIZE, NULL, 4, NULL);
#else
	xTaskCreate(platHttpServerTask, (const signed char *)"esphttpd", HTTPD_STACKSIZE, NULL, 4, NULL);
#endif
  }







//http://www.thegeekstuff.com/2012/05/c-mutex-examples/

static xQueueHandle SCDEMux;






/*
 * --------------------------------------------------------------------------------------------------
 *  FName: Platform_IdleCbTask
 *  Desc: Task that provides the Idle-Callbacks
 *  Para: void *pvParameters -> ?
 *  Rets: -/-
 * --------------------------------------------------------------------------------------------------
 */
static void
IdleCbTask(void *pvParameters)
{
  Entry_Definition_t* p_entry_definition;

  // endless loop - blocked by an delay
  while(1) {

	printf("\nNow IdleCbs>");

	// loop through definitions
	STAILQ_FOREACH(p_entry_definition, &SCDERoot.HeadCommon_Definitions, entries) {

		// check if F_WANTS_IDLE_TASK is set and IdleCbFn Fn is installed
		if ( (p_entry_definition->module->provided->IdleCbFn)
			&& (p_entry_definition->Common_CtrlRegA & F_WANTS_IDLE_TASK) ) {

			 os_printf("SCDE: Exec IdleCbFn!! Type-Name:%.*s Def-Name:%.*s FD:%d\n",
				p_entry_definition->module->provided->typeNameLen,
				p_entry_definition->module->provided->typeName,
				p_entry_definition->nameLen,
				p_entry_definition->name,
				p_entry_definition->fd);

			// clear Flag F_WANTS_WRITE, should be set again when more data should be sended
			p_entry_definition->Common_CtrlRegA &= ~F_WANTS_IDLE_TASK;

			// execute the idle callback function
			p_entry_definition->module->provided->IdleCbFn(p_entry_definition);
		}
	}

	vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}



/*
 * --------------------------------------------------------------------------------------------------
 *  FName: Platform_SelectQueryTask
 *  Desc: Task that selects the FD (read,write,exception) and creates read + write + exept. callbacks
 *  Para: void *pvParameters -> ?
 *  Rets: -/-
 * --------------------------------------------------------------------------------------------------
 */
static void
SelectQueryTask(void *pvParameters)
{
  Entry_Definition_t* p_entry_definition;
  int sd;

  // helper to finding highest file descriptor
  int max_fdp;

  // read socket descriptors set
  fd_set readfds;

  // write socket descriptors set
  fd_set writefds;

  // exception socket descriptors set
  fd_set exceptfds;

  os_printf("debug1:%d  2:%d.\n",
	FD_SETSIZE,
	sizeof(readfds) );

  struct timeval timeout;
  // Initialize the timeout data structure
    timeout.tv_sec = 1;//seconds;
    timeout.tv_usec = 0;

  // ???
  SCDEMux = xSemaphoreCreateRecursiveMutex();

  // ??
  int ret;
	
  os_printf("SCDE: startig global select loop.\n");

  // endless loop - blocked by select (depends on sets)
  while(1) {

	// list currently stored modules
	printf("Selectloop: heap:%d\n",heap_caps_get_free_size(MALLOC_CAP_8BIT));

	Entry_Definition_t *Common_Definition;
	STAILQ_FOREACH(Common_Definition, &SCDERoot.HeadCommon_Definitions, entries) {
		printf("L Module:%.*s Name:%.*s FD:%d\n",
			Common_Definition->module->provided->typeNameLen,
			Common_Definition->module->provided->typeName,
			Common_Definition->nameLen,
			Common_Definition->name,
			Common_Definition->fd);
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
	STAILQ_FOREACH(p_entry_definition, &SCDERoot.HeadCommon_Definitions, entries) {

		// file descriptor / socket descriptor of this definition
		sd = p_entry_definition->fd;

		// if valid socket descriptor then may be we need to add to sets
		if (sd != -1) {

			 os_printf("SCDE: adding %.*s fd:%d\n",
				p_entry_definition->nameLen,
				p_entry_definition->name,
				p_entry_definition->fd);

			// select for reading, but only if an Fn is installed
			if (p_entry_definition->module->provided->DirectReadFn) {

				FD_SET(sd , &readfds);
			}

			// select for writing (if wants to write), but only if an Fn is installed
			if  ( (p_entry_definition->Common_CtrlRegA & F_WANTS_WRITE)
				&& (p_entry_definition->module->provided->DirectWriteFn) ) {

				FD_SET(sd , &writefds);
			}

			// select for exceptions, but only if an Fn is installed
			if (p_entry_definition->module->provided->ExceptFn) {

				FD_SET(sd , &exceptfds);
			}

			// find highest file descriptor number, need it for the select function
			// calculation after each FD_SET?
			if (sd > max_fdp) max_fdp = sd;
		}
	}

	// execute the selection
	ret = select(max_fdp + 1, &readfds, &writefds, &exceptfds, &timeout);

	 os_printf("|SCDE: passed global select>");

	// did we get any?
	if (ret > 0) {

		 os_printf("SCDE: got any fd from select\n");
/*problem mit gelöschter definition?
		// loop through definitions with a valid socket descriptor to check and execute callbacks
		STAILQ_FOREACH(p_entry_definition, &SCDERoot.HeadCommon_Definitions, entries) {

			 os_printf("SCDE: ok fnd select\n");

			// file descriptor / socket descriptor of this definition
			sd = p_entry_definition->fd;

			// if valid socket descriptor then check and execute callbacks
			if (sd != -1) {

				// first check for write availability, but only if 'F_WANTS_WRITE' and 'DirectWriteFn' is installed
				if ( ( p_entry_definition->Common_CtrlRegA & F_WANTS_WRITE ) &&
					( p_entry_definition->module->provided->DirectWriteFn ) &&
					( FD_ISSET(sd , &writefds) ) ) {

					// clear Flag F_WANTS_WRITE, should be set again when more data should be sended
					p_entry_definition->Common_CtrlRegA &= ~F_WANTS_WRITE;

					// execute the write function
					p_entry_definition->module->provided->DirectWriteFn(p_entry_definition);
				}

				// second check for read availability. But only if a function is installed
				if ( (p_entry_definition->module->provided->DirectReadFn) &&
					(FD_ISSET(sd , &readfds)) ) {

					// execute the read function
					p_entry_definition->module->provided->DirectReadFn(p_entry_definition);
				}

				// third check for exceptions. But only if a function is installed
				if ( (p_entry_definition->module->provided->ExceptFn) &&
					(FD_ISSET(sd , &exceptfds)) ) {

					// execute the exception function
					p_entry_definition->module->provided->ExceptFn(p_entry_definition);
				}
			}
		}
*/
// --------------------------------------------------------------------------------------------------

		Entry_Definition_t* p_next_entry_definition;

		// first the write availability

		p_entry_definition = STAILQ_FIRST(&SCDERoot.head_definition);

		while (p_entry_definition != NULL) {

			// load next, because current may be deleted during call
			p_next_entry_definition = STAILQ_NEXT(p_entry_definition, entries);

			os_printf("SCDE: select writeset?\n");

			// file descriptor / socket descriptor of this definition
			sd = p_entry_definition->fd;

			// if valid socket descriptor then check and execute callbacks
			if (sd != -1) {

				// first check for write availability, but only if 'F_WANTS_WRITE' and 'DirectWriteFn' is installed
				if ( ( p_entry_definition->Common_CtrlRegA & F_WANTS_WRITE ) &&
					( p_entry_definition->module->provided->DirectWriteFn ) &&
					( FD_ISSET(sd , &writefds) ) ) {

					// clear Flag F_WANTS_WRITE, should be set again when more data should be sended
					p_entry_definition->Common_CtrlRegA &= ~F_WANTS_WRITE;

					// execute the write function
					p_entry_definition->module->provided->DirectWriteFn(p_entry_definition);
				}

			}

			esp_task_wdt_feed();

			// nexi is already buffered
			p_entry_definition = p_next_entry_definition;
		}


		// second the read availability

		p_entry_definition = STAILQ_FIRST(&SCDERoot.head_definition);

		while (p_entry_definition != NULL) {

			// load next, because current may be deleted during call
			p_next_entry_definition = STAILQ_NEXT(p_entry_definition, entries);

			os_printf("SCDE: select writeset?\n");

			// file descriptor / socket descriptor of this definition
			sd = p_entry_definition->fd;

			// if valid socket descriptor then check and execute callbacks
			if (sd != -1) {

				// second check for read availability. But only if a function is installed
				if ( (p_entry_definition->module->provided->DirectReadFn) &&
					(FD_ISSET(sd , &readfds)) ) {

					// execute the read function
					p_entry_definition->module->provided->DirectReadFn(p_entry_definition);
				}


			}

			esp_task_wdt_feed();

			// nexi is already buffered
			p_entry_definition = p_next_entry_definition;
		}

		// third the write availability

		p_entry_definition = STAILQ_FIRST(&SCDERoot.head_definition);

		while (p_entry_definition != NULL) {

			// load next, because current may be deleted during call
			p_next_entry_definition = STAILQ_NEXT(p_entry_definition, entries);

			os_printf("SCDE: select writeset?\n");

			// file descriptor / socket descriptor of this definition
			sd = p_entry_definition->fd;

			// if valid socket descriptor then check and execute callbacks
			if (sd != -1) {

				// third check for exceptions. But only if a function is installed
				if ( (p_entry_definition->module->provided->ExceptFn) &&

					(FD_ISSET(sd , &exceptfds)) ) {

					// execute the exception function
					p_entry_definition->module->provided->ExceptFn(p_entry_definition);
				}

			}

			esp_task_wdt_feed();

			// nexi is already buffered
			p_entry_definition = p_next_entry_definition;
		}

// --------------------------------------------------------------------------------------------------

	}
  }
}



































/*
 *  Main entry point for ESP32 into the application.
 */
int
app_main(void)
{
  LOGD("Free heap at start: %d", esp_get_free_heap_size());

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector    

 // LOGD("Free IRAM: %d",  xPortGetFreeHeapSizeTagged(MALLOC_CAP_32BIT));

//--------------------------------------------------------------------------------------------------

  //Initialize NVS
  esp_err_t ret = nvs_flash_init();

  // Error ? -> init nvs
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {

	ESP_ERROR_CHECK(nvs_flash_erase());

	ret = nvs_flash_init();
  }

  ESP_ERROR_CHECK(ret);

//--------------------------------------------------------------------------------------------------

  // init the SCDE-Engine (ROOT data)
  InitSCDERoot();

  // Now we can use the LogFn, say hello
  Log3( (const uint8_t*)"test!!!"
  		,4 //chck
		,1
		,"Test:%.*s\n"
		,3
		,TAG);

  // --------------------------------------------------------------------------------------------------
































#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"






// -------------------------------------------------------------------------------------------------
// initialize SPIFFS

  // initialize SPIFFS at path ??? 
  ESP_LOGI(TAG, "Initializing SPIFFS");
    
  esp_vfs_spiffs_conf_t conf = {
 	.base_path = "/spiffs",
	.partition_label = NULL,
	.max_files = 5,
	.format_if_mount_failed = true
  };

  // register SPIFFS
  //esp_err_t ret = esp_vfs_spiffs_register(&conf);
  ret = esp_vfs_spiffs_register(&conf);
  if (ret != ESP_OK) {
	if (ret == ESP_FAIL) {

		ESP_LOGE(TAG, "Failed to mount or format filesystem");

	} else if (ret == ESP_ERR_NOT_FOUND) {

		ESP_LOGE(TAG, "Failed to find SPIFFS partition");

	} else {

		ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));

	}

	return;
  }

  // get SPIFFS infos
  size_t total = 0, used = 0;

  ret = esp_spiffs_info(NULL, &total, &used);

  if (ret != ESP_OK) {

	ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
  } else {

  ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);

  }

// -------------------------------------------------------------------------------------------------
// embedd commands A-Z

  // Activate Add as SCDE built-in Command
  extern ProvidedByCommand_t Add_ProvidedByCommand;
  CommandActivateCommand(&Add_ProvidedByCommand);

  // Activate Attr as SCDE built-in Command
  extern ProvidedByCommand_t Attr_ProvidedByCommand;
  CommandActivateCommand(&Attr_ProvidedByCommand);

  // Activate Define as SCDE built-in Command
  extern ProvidedByCommand_t Define_ProvidedByCommand;
  CommandActivateCommand(&Define_ProvidedByCommand);

  // Activate Deleteattr as SCDE built-in Command
  extern ProvidedByCommand_t Deleteattr_ProvidedByCommand;
  CommandActivateCommand(&Deleteattr_ProvidedByCommand);

  // Activate Delete as SCDE built-in Command
  extern ProvidedByCommand_t Delete_ProvidedByCommand;
  CommandActivateCommand(&Delete_ProvidedByCommand);

  // Activate Help as SCDE built-in Command
  extern ProvidedByCommand_t Help_ProvidedByCommand;
  CommandActivateCommand(&Help_ProvidedByCommand);

  // Activate Include as SCDE built-in Command
  extern ProvidedByCommand_t Include_ProvidedByCommand;
  CommandActivateCommand(&Include_ProvidedByCommand);

  // Activate IOWrite as SCDE built-in Command
  extern ProvidedByCommand_t IOWrite_ProvidedByCommand;
  CommandActivateCommand(&IOWrite_ProvidedByCommand);

  // Activate List as SCDE built-in Command
  extern ProvidedByCommand_t List_ProvidedByCommand;
  CommandActivateCommand(&List_ProvidedByCommand);

  // Activate Rename as SCDE built-in Command
  extern ProvidedByCommand_t Rename_ProvidedByCommand;
  CommandActivateCommand(&Rename_ProvidedByCommand);

  // Activate Rereadcfg as SCDE built-in Command
  extern ProvidedByCommand_t Rereadcfg_ProvidedByCommand;
  CommandActivateCommand(&Rereadcfg_ProvidedByCommand);

  // Activate Save as SCDE built-in Command
  extern ProvidedByCommand_t Save_ProvidedByCommand;
  CommandActivateCommand(&Save_ProvidedByCommand);

  // Activate Set as SCDE built-in Command
  extern ProvidedByCommand_t Set_ProvidedByCommand;
  CommandActivateCommand(&Set_ProvidedByCommand);

  // Activate Setstate as SCDE built-in Command
  extern ProvidedByCommand_t Setstate_ProvidedByCommand;
  CommandActivateCommand(&Setstate_ProvidedByCommand);

  // Activate Shutdown as SCDE built-in Command
  extern ProvidedByCommand_t Shutdown_ProvidedByCommand;
  CommandActivateCommand(&Shutdown_ProvidedByCommand);

  // Activate Sub as SCDE built-in Command
  extern ProvidedByCommand_t Sub_ProvidedByCommand;
  CommandActivateCommand(&Sub_ProvidedByCommand);

// -------------------------------------------------------------------------------------------------
// embedd modules A-Z (we have no file system to load)

  // --- the required module

  // Activate Global as SCDE built-in Module
  extern ProvidedByModule_t Global_ProvidedByModule;
  CommandActivateModule(&Global_ProvidedByModule);

  // --- now the optional modules

  // Activate BH1750 as SCDE built-in Module
  extern ProvidedByModule_t BH1750_ProvidedByModule;
  CommandActivateModule(&BH1750_ProvidedByModule);

  // Activate BMP180 as SCDE built-in Module
  extern ProvidedByModule_t BMP180_ProvidedByModule;
  CommandActivateModule(&BMP180_ProvidedByModule);

  // Activate ESP32_DeVICE as SCDE built-in Module
  extern ProvidedByModule_t ESP32_DeVICE_ProvidedByModule;
  CommandActivateModule(&ESP32_DeVICE_ProvidedByModule);

  // Activate ESP32_I2C_Master as SCDE built-in Module
  extern ProvidedByModule_t ESP32_I2C_Master_ProvidedByModule;
  CommandActivateModule(&ESP32_I2C_Master_ProvidedByModule);

  // Activate ESP32_S0 as SCDE built-in Module
  extern ProvidedByModule_t ESP32_S0_ProvidedByModule;
  CommandActivateModule(&ESP32_S0_ProvidedByModule);

  // Activate ESP32_SPI as SCDE built-in Module
  extern ProvidedByModule_t ESP32_SPI_ProvidedByModule;
  CommandActivateModule(&ESP32_SPI_ProvidedByModule);

  // Activate ESP32_SwITCH as SCDE built-in Module
  extern ProvidedByModule_t ESP32_SwITCH_ProvidedByModule;
  CommandActivateModule(&ESP32_SwITCH_ProvidedByModule);

  // Activate ESP32_TouchGUI1 as SCDE built-in Module
  extern ProvidedByModule_t ESP32_TouchGUI1_ProvidedByModule;
  CommandActivateModule(&ESP32_TouchGUI1_ProvidedByModule);

  // Activate SSD1306 as SCDE built-in Module
  extern ProvidedByModule_t SSD1306_ProvidedByModule;
  CommandActivateModule(&SSD1306_ProvidedByModule);

  // Activate Telnet as SCDE built-in Module
  extern ProvidedByModule_t Telnet_ProvidedByModule;
  CommandActivateModule(&Telnet_ProvidedByModule);

  // Activate WebIF as SCDE built-in Module
  extern ProvidedByModule_t WebIf_ProvidedByModule;
  CommandActivateModule(&WebIf_ProvidedByModule);

// -------------------------------------------------------------------------------------------------
// initialize the global definition

  // for response msg from AnalyzeCommand
  struct headRetMsgMultiple_s headRetMsgMultiple;

  #define CMD_4_GLOBAL "define Global Global"
  headRetMsgMultiple = AnalyzeCommand((const uint8_t*) CMD_4_GLOBAL, 20);//sizeof(CMD_4_TELNET));

  // retMsgMultiple stailq filled ?
  if (!STAILQ_EMPTY(&headRetMsgMultiple)) {

	// for the retMsg elements
	strTextMultiple_t *retMsg;

	// get the entries till empty
	while (!STAILQ_EMPTY(&headRetMsgMultiple)) {

		retMsg = STAILQ_FIRST(&headRetMsgMultiple);

		Log3( (const uint8_t*)"xxx"
			 ,sizeof("xxx")
			 ,1
			 ,"result is:%.*s\n"
			 ,retMsg->strTextLen
			,retMsg->strText);

		// done, remove this entry
		STAILQ_REMOVE(&headRetMsgMultiple, retMsg, strTextMultiple_s, entries);

		// free the msg-string
		free(retMsg->strText);

		// and the strTextMultiple_t
		free(retMsg);
	}
  }

//--------------------------------------------------------------------------------------------------

  // vTaskDelay(1000 / portTICK_PERIOD_MS);
  Log("Plat_ESP32",10,"MainFn inits SCDE.");

  // Init / start SCDE Core ...
  InitA();

//  vTaskDelay(1000 / portTICK_PERIOD_MS);

//--------------------------------------------------------------------------------------------------

  Log("Plat_ESP32",10,"MainFn starts SelectQueryTask.");

  // Start SCDE Select-Query loop
  xTaskCreatePinnedToCore(&SelectQueryTask
	,"SelQuery"
	,8048
	,NULL
	,4
	,NULL
	,0);

  // start idle-callbacks loop
  xTaskCreatePinnedToCore(&IdleCbTask
	,"IdleCb"
	,8048
	,NULL
	,5
	,NULL
	,0);

 /*
  xTaskCreate(SelectQueryTask
	,(const char *)"SelQuery"
	,HTTPD_STACKSIZE
	,NULL
	,4
	,NULL);
*/

  Log("Plat_ESP32",10,"MainFn reaches while(0).");

  // dont now what to do here ...
  while (true) {
	vTaskDelay(300 / portTICK_PERIOD_MS);
  }

  return 0;
}

