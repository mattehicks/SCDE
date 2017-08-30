
//SPIFFS (SPI Flash File System)

#ifndef ESP32_Platform_SPIFFS_H_
#define ESP32_Platform_SPIFFS_H_


#include "spiffs.h"


//
int ESP32_Platform_SPIFFSRegisterVFS(const char *mountPoint, spiffs *fs);

//
int ESP32_Platform_MountSPIFFS();

#endif /* ESP32_Platform_SPIFFS_H_ */
