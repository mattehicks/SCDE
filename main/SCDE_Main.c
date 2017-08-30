
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






#include "SCDE_s.h"
#include "SCDE_Main.h"
#include "SCDE.h"

#include "Espfs.h"



#include "scde_task.h"








#include "esp_spi_flash.h"
#include "esp_partition.h"
#include "spiffs.h"
#include "esp_spiffs.h"
#include "ESP32_Platform_SPIFFS.h"







#include <freertos/FreeRTOS.h>
#include <freertos/heap_regions.h>
#include <freertos/task.h>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <heap_alloc_caps.h>
#include <nvs_flash.h>

#include "scde_task.h"
#include "logging.h"
#include "sdkconfig.h"


LOG_TAG("scde_main");









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
extern uint32_t _binary_x_img_start;
extern uint32_t _binary_espfs_img_start;
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





























int ICACHE_FLASH_ATTR httpdPlatSendData(ConnTypePtr conn, char *buff, int len) {
	conn->needWriteDoneNotif=1;
	return (write(conn->fd, buff, len)>=0);
}

void ICACHE_FLASH_ATTR httpdPlatDisconnect(ConnTypePtr conn) {
	conn->needsClose=1;
	conn->needWriteDoneNotif=1; //because the real close is done in the writable select code
}




void httpdPlatDisableTimeout(ConnTypePtr conn) {
	//Unimplemented for FreeRTOS
}

//Set/clear global httpd lock.
void ICACHE_FLASH_ATTR httpdPlatLock() {
	xSemaphoreTakeRecursive(httpdMux, portMAX_DELAY);
}

void ICACHE_FLASH_ATTR httpdPlatUnlock() {
	xSemaphoreGiveRecursive(httpdMux);
}


#define RECV_BUF_SIZE 2048
static void platHttpServerTask(void *pvParameters) {
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
	
	httpdMux=xSemaphoreCreateRecursiveMutex();
	
	for (x=0; x<HTTPD_MAX_CONNECTIONS; x++) {
		rconn[x].fd=-1;
	}
	
	/* Construct local address structure */
	memset(&server_addr, 0, sizeof(server_addr)); /* Zero out structure */
	server_addr.sin_family = AF_INET;			/* Internet address family */
	server_addr.sin_addr.s_addr = INADDR_ANY;   /* Any incoming interface */
	server_addr.sin_len = sizeof(server_addr);  
	server_addr.sin_port = htons(81);//httpPort); /* Local port */

	/* Create socket for incoming connections */
	do{
		listenfd = socket(AF_INET, SOCK_STREAM, 0);
		if (listenfd == -1) {
			os_printf("platHttpServerTask: failed to create sock!\n");
			vTaskDelay(1000/portTICK_RATE_MS);
		}
	} while(listenfd == -1);

	/* Bind to the local port */
	do{
		ret = bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
		if (ret != 0) {
			os_printf("platHttpServerTask: failed to bind!\n");
			vTaskDelay(1000/portTICK_RATE_MS);
		}
	} while(ret != 0);

	do{
		/* Listen to the local connection */
		ret = listen(listenfd, HTTPD_MAX_CONNECTIONS);
		if (ret != 0) {
			os_printf("platHttpServerTask: failed to listen!\n");
			vTaskDelay(1000/portTICK_RATE_MS);
		}
		
	} while(ret != 0);
	
	os_printf("esphttpd: active and listening to connections.\n");
	while(1){
		// clear fdset, and set the select function wait time
		int socketsFull=1;
		maxfdp = 0;
		FD_ZERO(&readset);
		FD_ZERO(&writeset);
		//timeout.tv_sec = 2;
		//timeout.tv_usec = 0;
		
		for(x=0; x<HTTPD_MAX_CONNECTIONS; x++){
			if (rconn[x].fd!=-1) {
				FD_SET(rconn[x].fd, &readset);
				if (rconn[x].needWriteDoneNotif) FD_SET(rconn[x].fd, &writeset);
				if (rconn[x].fd>maxfdp) maxfdp=rconn[x].fd;
			} else {
				socketsFull=0;
			}
		}
		
		if (!socketsFull) {
			FD_SET(listenfd, &readset);
			if (listenfd>maxfdp) maxfdp=listenfd;
		}



		//polling all exist client handle,wait until readable/writable
		ret = select(maxfdp+1, &readset, &writeset, NULL, NULL);//&timeout

		if(ret > 0){
			os_printf("action!\n");
			//See if we need to accept a new connection
			if (FD_ISSET(listenfd, &readset)) {
				len=sizeof(struct sockaddr_in);
				remotefd = accept(listenfd, (struct sockaddr *)&remote_addr, (socklen_t *)&len);
				if (remotefd<0) {
					os_printf("platHttpServerTask: Huh? Accept failed.\n");
					continue;
				}
				for(x=0; x<HTTPD_MAX_CONNECTIONS; x++) if (rconn[x].fd==-1) break;
				if (x==HTTPD_MAX_CONNECTIONS) {
					os_printf("platHttpServerTask: Huh? Got accept with all slots full.\n");
					continue;
				}
				int keepAlive = 1; //enable keepalive
				int keepIdle = 60; //60s
				int keepInterval = 5; //5s
				int keepCount = 3; //retry times
					
				setsockopt(remotefd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepAlive, sizeof(keepAlive));
				setsockopt(remotefd, IPPROTO_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle));
				setsockopt(remotefd, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval));
				setsockopt(remotefd, IPPROTO_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));
				
				rconn[x].fd=remotefd;
				rconn[x].needWriteDoneNotif=0;
				rconn[x].needsClose=0;
				
				len=sizeof(name);
				getpeername(remotefd, &name, (socklen_t *)&len);
				struct sockaddr_in *piname=(struct sockaddr_in *)&name;

				rconn[x].port=piname->sin_port;
				memcpy(&rconn[x].ip, &piname->sin_addr.s_addr, sizeof(rconn[x].ip));

				//httpdConnectCb(&rconn[x], rconn[x].ip, rconn[x].port);
				//os_timer_disarm(&connData[x].conn->stop_watch);
				//os_timer_setfn(&connData[x].conn->stop_watch, (os_timer_func_t *)httpserver_conn_watcher, connData[x].conn);
				//os_timer_arm(&connData[x].conn->stop_watch, STOP_TIMER, 0);
//				httpd_printf("httpserver acpt index %d sockfd %d!\n", x, remotefd);
			}
			
			//See if anything happened on the existing connections.
			for(x=0; x < HTTPD_MAX_CONNECTIONS; x++){
				//Skip empty slots
				if (rconn[x].fd==-1) continue;

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
					precvbuf=(char*)malloc(RECV_BUF_SIZE);
					if (precvbuf==NULL) {
						os_printf("platHttpServerTask: memory exhausted!\n");
						//httpdDisconCb(&rconn[x], rconn[x].ip, rconn[x].port);
						close(rconn[x].fd);
						rconn[x].fd=-1;
					}
					ret=recv(rconn[x].fd, precvbuf, RECV_BUF_SIZE,0);
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

  Common_Definition_t *Common_Definition;

  // endless loop - blocked by an delay
  while(1) {

	printf("\nNow IdleCbs>");

	// loop through definitions
	STAILQ_FOREACH(Common_Definition, &SCDERoot.HeadCommon_Definitions, entries) {

		// check if F_WANTS_IDLE_TASK is set and IdleCbFn Fn is installed
		if ( (Common_Definition->module->ProvidedByModule->IdleCbFn)
			&& (Common_Definition->Common_CtrlRegA & F_WANTS_IDLE_TASK) ) {

			 os_printf("SCDE: Exec IdleCbFn!! Type-Name:%.*s Def-Name:%.*s FD:%d\n"
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

  os_printf("debug1:%d  2:%d.\n"
		  ,FD_SETSIZE, sizeof(readfds) );

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

	Common_Definition_t *Common_Definition;
	STAILQ_FOREACH(Common_Definition, &SCDERoot.HeadCommon_Definitions, entries) {
		printf("L Module:%.*s Name:%.*s FD:%d\n"
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

			 os_printf("SCDE: adding %.*s fd:%d\n",Def->nameLen,Def->name,Def->fd);

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

	 os_printf("|SCDE: passed global select>");

	// did we get any?
	if (ret > 0) {

		 os_printf("SCDE: got any fd from select\n");

		// loop through definitions with a valid socket descriptor to check and execute callbacks
		STAILQ_FOREACH(Def, &SCDERoot.HeadCommon_Definitions, entries) {

			 os_printf("SCDE: ok fnd select\n");

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





#include "apps/sntp/sntp.h"

static void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();
}





static void obtain_time(void)
{
    initialize_sntp();

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 10;
    while(timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }
}














/*
 * --------------------------------------------------------------------------------------------------
 *  FName: ESP32_WiFiEventHandler
 *  Desc: Wifi event handler
 *  Para: System_Event_t *event -> struct with incoming event information for wifi
 *  Rets: -/-
 * --------------------------------------------------------------------------------------------------
 */
static esp_err_t
ESP32_WiFiEventHandler(void *ctx, system_event_t *event)
  {

   switch (event->event_id)

	{

//--------------------------------------------------------------------------------------------------

	// ESP32 WiFi ready
	case SYSTEM_EVENT_WIFI_READY:

	break;

//--------------------------------------------------------------------------------------------------

	//ESP32 finish scanning AP
	case SYSTEM_EVENT_SCAN_DONE:

	// struct contains:
  	// uint32_t status;          /**< status of scanning APs */
  	// uint8_t  number;
  	// uint8_t  scan_id;
	// system_event_sta_scan_done_t;

	break;

//--------------------------------------------------------------------------------------------------

	//ESP32 station start
	case SYSTEM_EVENT_STA_START:

	// station started - connect to station
	esp_wifi_connect();

	break;

//--------------------------------------------------------------------------------------------------

	//ESP32 station stop
	case SYSTEM_EVENT_STA_STOP:

	break;

//--------------------------------------------------------------------------------------------------

	//ESP32 station connected to AP
	case SYSTEM_EVENT_STA_CONNECTED:

	// struct contains:
   	// uint8_t ssid[32];         /**< SSID of connected AP */
   	// uint8_t ssid_len;         /**< SSID length of connected AP */
   	// uint8_t bssid[6];         /**< BSSID of connected AP*/
   	// uint8_t channel;          /**< channel of connected AP*/
   	// wifi_auth_mode_t authmode;
	// system_event_sta_connected_t;

	Log3( (const uint8_t*)"xxx"
		, sizeof("xxx")
		,1
		,"Devices station connected to an access-point. SSID:%s, channel:%d\n"
		,event->event_info.connected.ssid
		,event->event_info.connected.channel);

	break;

//--------------------------------------------------------------------------------------------------

	//ESP32 station disconnected from AP
	case SYSTEM_EVENT_STA_DISCONNECTED:

	// struct contains:
   	// uint8_t ssid[32];         /**< SSID of disconnected AP */
   	// uint8_t ssid_len;         /**< SSID length of disconnected AP */
   	// uint8_t bssid[6];         /**< BSSID of disconnected AP */
   	// uint8_t reason;           /**< reason of disconnection */
	// system_event_sta_disconnected_t;

	// This is a workaround as ESP32 WiFi libs don't currently auto-reassociate
	esp_wifi_connect();
	xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);

	Log3( (const uint8_t*)"xxx"
		, sizeof("xxx")
		,1
		,"Devices station disconnected from an access-point. SSID:%s, reason:%d\n"
		,event->event_info.disconnected.ssid
		,event->event_info.disconnected.reason);

	break;

//--------------------------------------------------------------------------------------------------

	//the auth mode of AP connected by ESP32 station changed
	case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:

	// struct contains:
   	// wifi_auth_mode_t old_mode;         /**< the old auth mode of AP */
    	// wifi_auth_mode_t new_mode;         /**< the new auth mode of AP */
	// system_event_sta_authmode_change_t;

	Log3( (const uint8_t*)"xxx"
		, sizeof("xxx")
		,1
		,"Devices station - Authmode changed from %d to %d\n",
		event->event_info.auth_change.old_mode,
		event->event_info.auth_change.new_mode);

	break;

//--------------------------------------------------------------------------------------------------

	//ESP32 station got IP from connected AP
	case SYSTEM_EVENT_STA_GOT_IP:

	// struct contains:
    	// tcpip_adapter_ip_info_t ip_info;
	// system_event_sta_got_ip_t;

	// ??
	xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);

	Log3( (const uint8_t*)"xxx"
		, sizeof("xxx")
		,1
		,"Devices station got IP from access point:"IPSTR ", mask:"IPSTR", gw:"IPSTR"\n",
		IP2STR(&event->event_info.got_ip.ip_info.ip),
		IP2STR(&event->event_info.got_ip.ip_info.netmask),
		IP2STR(&event->event_info.got_ip.ip_info.gw));

	break;

//--------------------------------------------------------------------------------------------------

	//ESP32 soft-AP start
	case SYSTEM_EVENT_AP_START:

	break;

//--------------------------------------------------------------------------------------------------

	//ESP32 soft-AP stop
	case SYSTEM_EVENT_AP_STOP:

	break;

//--------------------------------------------------------------------------------------------------

	//a station connected to ESP32 soft-AP
	case SYSTEM_EVENT_AP_STACONNECTED:

	// struct contains:
 	// uint8_t mac[6];           /**< MAC address of the station connected to ESP32 soft-AP */
 	// uint8_t aid;              /**< the aid that ESP32 soft-AP gives to the station connected to  */
	// system_event_ap_staconnected_t;
	
	Log3( (const uint8_t*)"xxx"
		, sizeof("xxx")
		,1
		,"I:WSAP Device conn.: "MACSTR ", AID = %d\n",
		MAC2STR(event->event_info.sta_connected.mac),
		event->event_info.sta_connected.aid);

	break;

//--------------------------------------------------------------------------------------------------

	// a station disconnected from ESP32 soft-AP
	case SYSTEM_EVENT_AP_STADISCONNECTED:

	// struct contains:
   	// uint8_t mac[6];           /**< MAC address of the station disconnects to ESP32 soft-AP */
   	// uint8_t aid;              /**< the aid that ESP32 soft-AP gave to the station disconnects to  */
	// system_event_ap_stadisconnected_t;

	Log3( (const uint8_t*)"xxx"
		, sizeof("xxx")
		,1
		,"WSAP - Device disconnected: "MACSTR ", AID = %d\n",
		MAC2STR(event->event_info.sta_disconnected.mac),
		event->event_info.sta_disconnected.aid);

	break;

//--------------------------------------------------------------------------------------------------

	// Receive probe request packet in soft-AP interface
	case SYSTEM_EVENT_AP_PROBEREQRECVED:

	// struct contains:
 	// int rssi;                 /**< Received probe request signal strength */
 	// uint8_t mac[6];           /**< MAC address of the station which send probe request */
	// system_event_ap_probe_req_rx_t;

	Log3( (const uint8_t*)"xxx"
		, sizeof("xxx")
		,1
		,"WSAP - ProbeReq.Rcved:%d\n"
		,event->event_info.ap_probereqrecved.rssi);

	break;

//--------------------------------------------------------------------------------------------------

	default:
	break;

//--------------------------------------------------------------------------------------------------

	}

  return ESP_OK;

  }



























/*
 * --------------------------------------------------------------------------------------------------
 *  FName: ESP32_WiFiInit
 *  Desc: Init wifi
 *  Para: -/-
 *  Rets: -/-
 * --------------------------------------------------------------------------------------------------
 */
static void 
ESP32_WiFiInit(void) {

  tcpip_adapter_init();

  wifi_event_group = xEventGroupCreate();

  // install wifi event handler
  ESP_ERROR_CHECK(esp_event_loop_init(ESP32_WiFiEventHandler, NULL) );

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );

  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );

  wifi_config_t sta_config = {
        .sta = {
            .ssid = "SF4 AP",
            .password = "pcmcia91",
            .bssid_set = false
        }
  };

  ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );

  ESP_ERROR_CHECK( esp_wifi_start() );

  ESP_ERROR_CHECK( esp_wifi_connect() );

}





//esp_err_t event_handler(void *ctx, system_event_t *event)
//{
//    return ESP_OK;
//}


/*
 *  Main entry point for ESP32 into the application.
 */
int
app_main(void)
{

  LOGD("Free heap at start: %d", esp_get_free_heap_size());

  LOGD("Free IRAM: %d",  xPortGetFreeHeapSizeTagged(MALLOC_CAP_32BIT));

  nvs_flash_init();

  system_init();

  LOGD("WiFi init");
  ESP32_WiFiInit();



  ESP_LOGI(TAG, "... hello\r\n");


  // mount the SPIFFS, make VFS useable ...
  ESP32_Platform_MountSPIFFS();

  ESP_LOGI(TAG, "... VFS accessible\r\n");

//--------------------------------------------------------------------------------------------------

  // init the SCDE-Engine
  InitSCDERoot();

  Log3( (const uint8_t*)"test!!!"
  		,4 //chck
		,1
		,"Test:%.*s\n"
		,3
		,TAG);


  // --------------------------------------------------------------------------------------------------

  // wait till wifi is connected
  xEventGroupWaitBits(wifi_event_group
		  , CONNECTED_BIT
		  ,false
		  ,true
		  ,portMAX_DELAY);

  // sync the rtc-time
  initialize_sntp();



  // Set adress of EspFs binary
  espFsInit(&_binary_x_img_start);
  espFsInit(&_binary_espfs_img_start);
// espFsInit(&TAG);

// -------------------------------------------------------------------------------------------------

  // embedded commands A-Z

  // Activate Attr as SCDE built-in Command
  extern providedByCommand_t Attr_ProvidedByCommand;
  CommandActivateCommand(&Attr_ProvidedByCommand);

  // Activate Add as SCDE built-in Command
  extern providedByCommand_t Add_ProvidedByCommand;
  CommandActivateCommand(&Add_ProvidedByCommand);

  // Activate Define as SCDE built-in Command
  extern providedByCommand_t Define_ProvidedByCommand;
  CommandActivateCommand(&Define_ProvidedByCommand);

  // Activate Delete as SCDE built-in Command
  extern providedByCommand_t Delete_ProvidedByCommand;
  CommandActivateCommand(&Delete_ProvidedByCommand);

  // Activate Deleteattr as SCDE built-in Command
  extern providedByCommand_t Deleteattr_ProvidedByCommand;
  CommandActivateCommand(&Deleteattr_ProvidedByCommand);

  // Activate Help as SCDE built-in Command
  extern providedByCommand_t Help_ProvidedByCommand;
  CommandActivateCommand(&Help_ProvidedByCommand);

  // Activate Include as SCDE built-in Command
  extern providedByCommand_t Include_ProvidedByCommand;
  CommandActivateCommand(&Include_ProvidedByCommand);

  // Activate List as SCDE built-in Command
  extern providedByCommand_t List_ProvidedByCommand;
  CommandActivateCommand(&List_ProvidedByCommand);

  // Activate Set as SCDE built-in Command
  extern providedByCommand_t Set_ProvidedByCommand;
  CommandActivateCommand(&Set_ProvidedByCommand);

  // Activate Shutdown as SCDE built-in Command
  extern providedByCommand_t Shutdown_ProvidedByCommand;
  CommandActivateCommand(&Shutdown_ProvidedByCommand);

  // Activate Sub as SCDE built-in Command
  extern providedByCommand_t Sub_ProvidedByCommand;
  CommandActivateCommand(&Sub_ProvidedByCommand);

// -------------------------------------------------------------------------------------------------

  // embedded modules A-Z (we have no file system to load)

  // Activate Global as SCDE built-in Module
  extern ProvidedByModule_t Global_ProvidedByModule;
  CommandActivateModule(&Global_ProvidedByModule);

// --- now the optional modules

  // Activate ESP32_BH1750 as SCDE built-in Module
  extern ProvidedByModule_t ESP32_BH1750_ProvidedByModule;
  CommandActivateModule(&ESP32_BH1750_ProvidedByModule);

  // Activate ESP32Control as SCDE built-in Module
  extern ProvidedByModule_t ESP32_Control_ProvidedByModule;
  CommandActivateModule(&ESP32_Control_ProvidedByModule);

  // Activate ESP32_I2C_Master as SCDE built-in Module
  extern ProvidedByModule_t ESP32_I2C_Master_ProvidedByModule;
  CommandActivateModule(&ESP32_I2C_Master_ProvidedByModule);

  // Activate ESP32_S0 as SCDE built-in Module
  extern ProvidedByModule_t ESP32_S0_ProvidedByModule;
  CommandActivateModule(&ESP32_S0_ProvidedByModule);

  // Activate ESP32_SwITCH as SCDE built-in Module
  extern ProvidedByModule_t ESP32_SwITCH_ProvidedByModule;
  CommandActivateModule(&ESP32_SwITCH_ProvidedByModule);

  // Activate Telnet as SCDE built-in Module
  extern ProvidedByModule_t Telnet_ProvidedByModule;
  CommandActivateModule(&Telnet_ProvidedByModule);

  // Activate WebIF as SCDE built-in Module
  extern ProvidedByModule_t WebIf_ProvidedByModule;
  CommandActivateModule(&WebIf_ProvidedByModule);

// -------------------------------------------------------------------------------------------------

  // for response msg from AnalyzeCommand
  struct headRetMsgMultiple_s headRetMsgMultiple;

  // initialize the global definition


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

  vTaskDelay(1000 / portTICK_PERIOD_MS);

  InitA();

  vTaskDelay(1000 / portTICK_PERIOD_MS);


//--------------------------------------------------------------------------------------------------

//  doGlobalDef("cfgfilename", 11);


//--------------------------------------------------------------------------------------------------

  Log3( (const uint8_t*)"xxx"
  		, sizeof("xxx")
		,1
		,"Commands loaded. Continue with Defines\n");



/*  // for Fn response msg
  strTextMultiple_t retMsg;
  retMsg.strText = NULL;*/

/*
  // start the internal WebIF (built in module)
  // define Name:MyTelnet Module:Telnet -> Args: Port:23
#define CMD_4_TELNET "define MyTelnet Telnet 23"
 ret = SCDEFn.CommandDefineCommandFn((const uint8_t*) CMD_4_TELNET, 18);//sizeof(CMD_4_TELNET));
 if (ret)
 	 {
	 Log3( (const uint8_t*)"xxx"
			 ,sizeof("xxx")
			 ,1
			 ,"result is:%s\n"
			 ,ret);
	 free(ret);
 	 }
*/



//--------------------------------------------------------------------------------------------------
/*
  // start the internal Telnet (built in module)
  // define Name:MyTelnet Module:Telnet -> Args: Port:23
#define CMD_4_TELNET "define MyTelnet Telnet 23"
  headRetMsgMultiple = AnalyzeCommand((const uint8_t*) CMD_4_TELNET, 25);//sizeof(CMD_4_TELNET));

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

 vTaskDelay(1000 / portTICK_PERIOD_MS);
*/
//--------------------------------------------------------------------------------------------------




//--------------------------------------------------------------------------------------------------

  // start the internal WebIF (built in module)
  // define Name:WebIF Module:WebIF -> Args: Port:80


//  char *WebIfDef = "MyWebIF WebIF 80";
//  char* Args = malloc (strlen(WebIfDef)+1);
//  strcpy(Args,WebIfDef);
//  SCDE_memcpy_plus
//  printf("Dcheck1:%p\n"
//   		,Args);

//spz  CommandDefine("MyWebIF WebIF 80"); // goes to external flash
   // start the internal Telnet (built in module)
  // define Name:MyTelnet Module:Telnet -> Args: Port:23
/*
#define CMD_4_WEBIF "define MyWebIF WebIf 80"
 headRetMsgMultiple = AnalyzeCommand((const uint8_t*) CMD_4_WEBIF, 23);//sizeof(CMD_4_TELNET));

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

 vTaskDelay(1000 / portTICK_PERIOD_MS);
*/
//--------------------------------------------------------------------------------------------------






//--------------------------------------------------------------------------------------------------
/*
#define CMD_4_ESP32CONTROL "define ESP32Ctrl esp32control"
  headRetMsgMultiple = AnalyzeCommand((const uint8_t*) CMD_4_ESP32CONTROL, 29);//sizeof(CMD_4_TELNET));

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

  vTaskDelay(1000 / portTICK_PERIOD_MS);
*/
//--------------------------------------------------------------------------------------------------









 /*   char *WebIfDef = "MyWebIF";
    char* Args = malloc (25);
  //  strcpy(Args,WebIfDef);
    SCDE_memcpy_plus(Args,WebIfDef,25);
  //  printf("Dcheck1:%p\n"
  //   		,Args);
    CommandUndefine(Args); // goes to external flash*/


/*spz
  Log3("main?", 1, "calling a set fn for test!\n");
  CommandSet("MyWebIF hellotest"); // goes to external flash

  Log3("main?", 1, "calling a undefine fn for test!\n");
  CommandUndefine("MyWebIF"); // goes to external flash

  Log3("main?", 1, "wtests finnished\n");
*/
/*  char* ccc;
  int xyz = asprintf(&ccc, "xxx%d",5);*/

//  static const char *TAG = "example";
//  ESP_LOGI(TAG, "... allocated socket\r\n");

//--------------------------------------------------------------------------------------------------

// httpdPlatInit(int port, int maxConnCt) 
//  httpdPlatInit(81, 10);


// start Defines-Select-Query loop
  xTaskCreatePinnedToCore(&SelectQueryTask
		  	 , "SelQuery"
			 , 8048
			 , NULL
			 , 4
			 , NULL
			 , 0);

// start Defines-Select-Query loop
  xTaskCreatePinnedToCore(&IdleCbTask
		  	 , "IdleCb"
			 , 8048
			 , NULL
			 , 5
			 , NULL
			 , 0);




 /*
  xTaskCreate(SelectQueryTask
	,(const char *)"SelQuery"
	,HTTPD_STACKSIZE
	,NULL
	,4
	,NULL);
*/
  // Test of logging
  Log("HCTRL",16,"MainFn Reached\n");












/*
 // Perform the tests on the VFS
    registerTestVFS("/data");
//    ESP_LOGI(tag, "vfs registered");

    FILE *file = fopen("/data/x", "w");

    if (file == NULL) {
 //   	ESP_LOGE(tag, "failed to open file");
//    	return 0;
    }


fprintf(file, "Hello!");
*/






  char tag[] = "test_memory_vfs()";
  printf("Memory test starting...\n");

  int ret;
/*
  // fill the SPIFFS cfg struct 
  ConfigSPIFFS();

  // mount the filesystem
  ESP_LOGI(tag, "Mounting SPIFFS");

//  bool forceFormat = true;
  bool forceFormat = false;

  // mount the SPIFFS
  ret = SPIFFS_mount(&fs
      ,&cfg
      ,spiffs_work_buf
      ,spiffs_fds
      ,sizeof (spiffs_fds)
      ,spiffs_cache_buf
      ,sizeof (spiffs_cache_buf)
      ,NULL);

  if (ret != SPIFFS_OK || forceFormat) {
      ESP_LOGI(tag, "Mounting SPIFFS failed with result=%i", ret);


      ESP_LOGI(tag, "Unmounting SPIFFS...");
      SPIFFS_unmount(&fs);


      ESP_LOGI(tag, "Formatting SPIFFS...");
      ret = SPIFFS_format(&fs);
      ESP_LOGI(tag, "   result=%i", ret);

      ESP_LOGI(tag, "Mounting SPIFFS...");
      ret = SPIFFS_mount(&fs, &cfg, spiffs_work_buf, spiffs_fds, sizeof(spiffs_fds), spiffs_cache_buf, sizeof(spiffs_cache_buf), NULL);

      ESP_LOGI(tag, "   result=%i", ret);
  }

  else if (ret == SPIFFS_OK) {

      ESP_LOGI(tag, "SPIFFS mounted");
  }
*/

  char fname[] = "/data/makerfilex.cfg";    //myfile";

//  SPIFFS_RegisterVFS("/data", &fs);

//  ESP_LOGI(tag, "vfs registered");


  FILE* file = fopen(fname, "w");
  if (file <= NULL) {

      ESP_LOGE(tag, "failed to open file '%s'. result=0x%X",fname, (uint32_t)file);

      return 0;
  }

  else {
	int nbytes;


      ESP_LOGE(tag, "Opened file '%s'. file=0x%X", fname, (uint32_t)file);

      char teststring[]="Hello!";
      nbytes = fwrite(teststring, 1, 6, file);

      if(nbytes==-1) {
          ESP_LOGE(tag, "Failed to write to file. result=%i", nbytes);
      }
 
      else {

          ESP_LOGI(tag, "Wrote %i bytes to file from buffer at 0x%X", nbytes, (uint32_t)teststring);

          //ret = fflush(file);
          //ESP_LOGI(tag, "fflush=%i", ret);

          ret = fclose(file);
          ESP_LOGI(tag, "fclose=%i", ret);
	}



 char   fname2[] = "/data/makerfile.cfg";    //myfile";
          FILE* file2 = fopen(fname2, "r+");

          if (file2 > NULL){
          //ret = fseek(file2, 0, SEEK_SET);
          //ESP_LOGI(tag, "fseek=%i", ret);

          char *buf;
          buf = (char*)malloc(64);
          memset(buf, 0, 64);

          ESP_LOGI(tag, "Attempting to read 6 bytes from file 0x%X into buffer at 0x%X.", (uint32_t)file2, (uint32_t)buf);
          nbytes = fread(buf,1,6,file2);
          ESP_LOGI(tag, "Read %i bytes from file 0x%X into buffer at 0x%X: '%s', ", nbytes, (uint32_t)file2, (uint32_t)buf, buf);
          fclose(file2);
          free(buf);
      }

      else {

          ESP_LOGE(tag, "Failed to open file!");
      }




          FILE* file3 = fopen(fname, "r");

          if (file3 > NULL){
          //ret = fseek(file2, 0, SEEK_SET);
          //ESP_LOGI(tag, "fseek=%i", ret);

          char *buf;
          buf = (char*)malloc(64);
          memset(buf, 0, 64);

          ESP_LOGI(tag, "Attempting to read 6 bytes from file 0x%X into buffer at 0x%X.", (uint32_t)file3, (uint32_t)buf);
          nbytes = fread(buf,1,6,file3);
          ESP_LOGI(tag, "Read %i bytes from file 0x%X into buffer at 0x%X: '%s', ", nbytes, (uint32_t)file3, (uint32_t)buf, buf);
          fclose(file3);
          free(buf);
      }

      else {

          ESP_LOGE(tag, "Failed to open file!");
      }


    }
  //}

  printf("Memory test complete!\n");

















  gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
   int level = 0;

    while (true) {
        gpio_set_level(GPIO_NUM_4, level);
        level = !level;
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }

    return 0;
}

