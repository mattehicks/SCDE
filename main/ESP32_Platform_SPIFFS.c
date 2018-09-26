/* #################################################################################################
 *
 *      Name: SPIFFS VFS
 *  Function: 
 *
 *  ESP 8266EX & ESP32 SoC Activities ...
 *  HoME CoNTROL & Smart Connected Device Engine Activities ...
 *  Copyright by EcSUHA
 *
 *  Created by Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany for EcSUHA.de 
 *
 *  MSchulze780@GMAIL.COM
 *  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
 * #################################################################################################
 */


/**
 * SPIFFs documentation can be found at https://github.com/pellepl/spiffs
 */

#include "ProjectConfig.h"

#include "spiffs.h"
#include "ESP32_Platform_SPIFFS.h"
#include "esp_spiffs.h"

#include <esp_vfs.h>
#include <esp_log.h>
#include <fcntl.h>
#include <errno.h>

#include "sdkconfig.h"


// set ammount of debug information 0 = off 5 = max (recommended)
#ifndef ESP32_SPIFFS_VFS_DBG
#define ESP32_SPIFFS_VFS_DBG 0
#endif


// the mount point for spiffs
#define SCDE_VFS_MOUNTPOINT "/data"

//0x10000 + (1024*512) + (1024*512) + (1024*512) to hex
// ESP32 Platform Memory cfg
#define NVS_PARTITION_START     0x9000
#define NVS_PARTITION_SIZE      0x6000
#define NVS_PHY_START           0xF000
#define NVS_PHY_SIZE            0x1000

#define FACTORY_PARTITION_START 0x10000
#define FACTORY_PARTITION_SIZE  1024*512

#define OTA1_PARTITION_START 	FACTORY_PARTITION_START + FACTORY_PARTITION_SIZE
#define OTA1_PARTITION_SIZE  	FACTORY_PARTITION_SIZE

#define OTA2_PARTITION_START 	OTA1_PARTITION_START + OTA1_PARTITION_SIZE
#define OTA2_PARTITION_SIZE  	FACTORY_PARTITION_SIZE

#define DATA_PARTITION_START 	OTA2_PARTITION_START + OTA2_PARTITION_SIZE
#define DATA_PARTITION_SIZE  	1024*512 //1024*256

#define SECTOR_SIZE             4096
#define PAGE_SIZE               256
#define MEMORY_SIZE             DATA_PARTITION_SIZE
#define MAX_CONCURRENT_FDS      4



// SPIFFS storage areas.
static spiffs fs;
static uint8_t spiffs_work_buf[PAGE_SIZE*2];
static uint8_t spiffs_fds[32*4];
static uint8_t spiffs_cache_buf[(PAGE_SIZE+32)*4];



static char* 
piffsErrorToString( int code ) {

  static char msg[10];

  switch (code) {

  case SPIFFS_OK:
      return "SPIFFS_OK";

  case SPIFFS_ERR_NOT_MOUNTED:
      return "SPIFFS_ERR_NOT_MOUNTED";

  case SPIFFS_ERR_FULL:
      return "SPIFFS_ERR_FULL";

  case SPIFFS_ERR_NOT_FOUND:
      return "SPIFFS_ERR_NOT_FOUND";

  case SPIFFS_ERR_END_OF_OBJECT:
     return "SPIFFS_ERR_END_OF_OBJECT";

  case SPIFFS_ERR_DELETED:
       return "SPIFFS_ERR_DELETED";

  case SPIFFS_ERR_FILE_CLOSED:
      return "SPIFFS_ERR_FILE_CLOSED";

  case SPIFFS_ERR_FILE_DELETED:
      return "SPIFFS_ERR_FILE_DELETED";

  case SPIFFS_ERR_BAD_DESCRIPTOR:
      return "SPIFFS_ERR_BAD_DESCRIPTOR";

  case SPIFFS_ERR_NOT_A_FS:
      return "SPIFFS_ERR_NOT_A_FS";

  case SPIFFS_ERR_FILE_EXISTS:
      return "SPIFFS_ERR_FILE_EXISTS";
  }

  sprintf(msg, "%d", code);

  return msg;

}



static const char *
typeToString(spiffs_obj_type type) {

  switch(type) {

  case SPIFFS_TYPE_DIR:
      return "Dir";

  case SPIFFS_TYPE_FILE:
      return "File";

  case SPIFFS_TYPE_HARD_LINK:
      return "Hard link";

  case SPIFFS_TYPE_SOFT_LINK:
      return "Soft link";

  default:
      return "Unknown";

  }
}














//#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
/*
static int spiffsErrMap(spiffs *fs) {
	int errorCode = SPIFFS_errno(fs);
	switch (errorCode) {
		case SPIFFS_ERR_FULL:
			return ENOSPC;
		case SPIFFS_ERR_NOT_FOUND:
			return ENOENT;
		case SPIFFS_ERR_FILE_EXISTS:
			return EEXIST;
		case SPIFFS_ERR_NOT_A_FILE:
			return EBADF;
		case SPIFFS_ERR_OUT_OF_FILE_DESCS:
			return ENFILE;
		default: {
			ESP_LOGE(tag, "We received SPIFFs error code %d but didn't know how to map to an errno", errorCode);
			return ENOMSG;
		}
	}
} // spiffsErrMap
*/















/**
 * Debug-Log the flags that are specified in an open() call.
 */
static void 
logFlags(int flags) {

  #if ESP32_SPIFFS_VFS_DBG >= 3 
  printf("flags:");;
  #endif

  if (flags & O_APPEND) {

      #if ESP32_SPIFFS_VFS_DBG >= 3 
      printf("- O_APPEND");
      #endif
  }

  if (flags & O_CREAT) {

      #if ESP32_SPIFFS_VFS_DBG >= 3 
      printf("- O_CREAT");
      #endif
  }


  if (flags & O_TRUNC) {

      #if ESP32_SPIFFS_VFS_DBG >= 3 
      printf("- O_TRUNC");
      #endif
  }


  if (flags & O_RDONLY) {

      #if ESP32_SPIFFS_VFS_DBG >= 3 
      printf("- O_RDONLY");
      #endif
  }


  if (flags & O_WRONLY) {

      #if ESP32_SPIFFS_VFS_DBG >= 3 
      printf("- O_WRONLY");
      #endif
  }


  if (flags & O_RDWR) {

      #if ESP32_SPIFFS_VFS_DBG >= 3 
      printf("- O_RDWR");
      #endif
  }

}



/**
 * VFS connector to SPIFFS for the write Fn
 */
static size_t 
vfs_write(void *ctx, int fd, const void *data, size_t size) {

  #if ESP32_SPIFFS_VFS_DBG >= 3 
  printf("|vfs_write, fd=%d, data=0x%X, size=%i>"
      ,fd
      ,(int32_t) data
      ,(int32_t) size);
  #endif

  spiffs *fs = (spiffs *) ctx;

  size_t retSize = SPIFFS_write(fs, (spiffs_file) fd, (void *) data, size);

  return retSize;

}



/**
 * VFS connector to SPIFFS for the lseek Fn
 */
static off_t
vfs_lseek(void *ctx, int fd, off_t offset, int whence) {

  #if ESP32_SPIFFS_VFS_DBG >= 3 
  printf("|vfs_lseek, fd=%d, offset=%d, whence=%d>"
      ,fd
      ,(int) offset
      ,whence);
  #endif

  spiffs *fs = (spiffs *) ctx;

  SPIFFS_lseek(fs, (spiffs_file) fd, offset, whence);

  return 0;

}



/**
 * VFS connector to SPIFFS for the read Fn
 */
static ssize_t
vfs_read(void *ctx, int fd, void *dst, size_t size) {

  #if ESP32_SPIFFS_VFS_DBG >= 3 
  printf("|vfs_read, read ctx=%X, fd=%d, dst=0x%X, size=%i>"
      ,(int32_t) ctx
      ,fd
      ,(int32_t) dst
      ,(int32_t) size);
  #endif

  spiffs *fs = (spiffs *) ctx;

  ssize_t retSize = SPIFFS_read(fs, (spiffs_file) fd, dst, size);

  return retSize;

}



/**
 * VFS connector to SPIFFS for the open Fn
 * Open the file specified by path.  The flags contain the instructions
 * on how the file is to be opened.
 *
 * For example:
 *
 * O_CREAT  - Create the named file.
 * O_TRUNC  - Truncate (empty) the file.
 * O_RDONLY - Open the file for reading only.
 * O_WRONLY - Open the file for writing only.
 * O_RDWR   - Open the file for reading and writing.
 * O_APPEND - Append to the file.
 *
 * The mode are access mode flags.
 */
static int
vfs_open(void *ctx, const char *path, int flags, int accessMode) {

  #if ESP32_SPIFFS_VFS_DBG >= 3 
  printf("|vfs_open, path=%s, flags=0x%x, accessMode=0x%X>"
      ,path
      ,flags
      ,accessMode);
  #endif

  // The openMode is encoded in the 1st 2 bits as:
  // 0 - 00 - RDONLY
  // 1 - 01 - WRONLY
  // 2 - 10 - RDWR
  // 3 - 11 - Undefined

  // debug-log flags in extra Fn
  logFlags(flags);

  spiffs *fs = (spiffs *) ctx;

  int spiffsFlags = 0;

  int openMode = (flags & 3);

  if (openMode == O_RDONLY) {
      spiffsFlags |= SPIFFS_O_RDONLY;
  }

  else if (openMode == O_RDWR) {
      spiffsFlags |= SPIFFS_O_RDWR;
  }

  else if (openMode == O_WRONLY) {

      spiffsFlags |= SPIFFS_O_WRONLY;
  }

  if (flags & O_CREAT) {

      spiffsFlags |= SPIFFS_O_CREAT;
  }

  if (flags & O_TRUNC) {

      spiffsFlags |= SPIFFS_O_TRUNC;
  }

  if (flags & O_APPEND) {

      spiffsFlags |= SPIFFS_O_APPEND;
  }

  int rc = SPIFFS_open(fs, path, spiffsFlags, accessMode);

  return rc;

}



/**
 * VFS connector to SPIFFS for the close Fn
 */
static int
vfs_close(void *ctx, int fd) {

  #if ESP32_SPIFFS_VFS_DBG >= 3 
  printf("|vfs_close, fd=%d>"
      ,fd);
  #endif

  spiffs *fs = (spiffs *) ctx;

  int rc = SPIFFS_close(fs, (spiffs_file) fd);

  return rc;

}



/**
 * VFS connector to SPIFFS for the fstat Fn
 */
static int
vfs_fstat(void *ctx, int fd, struct stat *st) {

  #if ESP32_SPIFFS_VFS_DBG >= 3 
  printf("|vfs_fstat, fd=%d, st=0x%X>"
      ,fd
      ,(uint32_t) st);
  #endif

  spiffs_stat spiffsStat;

  spiffs *fs = (spiffs *) ctx;

  SPIFFS_fstat(fs, fd, &spiffsStat);

  st->st_size = spiffsStat.size;

  return 1;
 
}



/**
 * VFS connector to SPIFFS for the stat Fn
 */
static int
vfs_stat(void *ctx, const char *path, struct stat *st) {

  #if ESP32_SPIFFS_VFS_DBG >= 3 
  printf("|vfs_stat path=%s>"
      ,path);
  #endif

  spiffs_stat spiffsStat;

  spiffs *fs = (spiffs *)ctx;

  SPIFFS_stat(fs, path, &spiffsStat);

  st->st_size = spiffsStat.size;

  return 1;

}



/**
 * VFS connector to SPIFFS for the unlink Fn
 */
static int 
vfs_unlink(void *ctx, const char *path) {

  #if ESP32_SPIFFS_VFS_DBG >= 3 
  printf("|vfs_unlink, path=%s>"
      ,path);
  #endif

  spiffs *fs = (spiffs *)ctx;

  SPIFFS_remove(fs, path);

  return 0;

}



/**
 * VFS connector to SPIFFS for the link Fn
 */
static int
vfs_link(void *ctx, const char *oldPath,  char *newPath) {

  #if ESP32_SPIFFS_VFS_DBG >= 3 
  printf("|vfs_link, !!!NOT_IMPLEMENTED!!!, oldPath=%s, newPath=%s>"
      ,oldPath
      ,newPath);
  #endif

  return 0;

}



/**
 * VFS connector to SPIFFS for the rename Fn
 */
static int
vfs_rename(void *ctx, const char *oldPath,  char *newPath) {

  #if ESP32_SPIFFS_VFS_DBG >= 3 
  printf("|vfs_rename, oldPath=%s, newPath=%s>"
      ,oldPath
      ,newPath);
  #endif

  spiffs *fs = (spiffs *) ctx;

  int rc = SPIFFS_rename(fs, oldPath, newPath);

  return rc;

}



/**
 * Register the VFS at the specified mount point.
 * The callback functions are registered to handle the
 * different functions that may be requested against the
 * VFS.
 */
int
ESP32_Platform_SPIFFSRegisterVFS(const char *mountPoint, spiffs *fs) {

  esp_vfs_t vfs;
  int ret;

  vfs.flags = ESP_VFS_FLAG_CONTEXT_PTR;
  vfs.write_p  = vfs_write;
  vfs.lseek_p  = vfs_lseek;
  vfs.read_p   = vfs_read;
  vfs.open_p   = vfs_open;
  vfs.close_p  = vfs_close;
  vfs.fstat_p  = vfs_fstat;
  vfs.stat_p   = vfs_stat;
  vfs.link_p   = vfs_link;
  vfs.unlink_p = vfs_unlink;
  vfs.rename_p = vfs_rename;

  ret = esp_vfs_register(mountPoint, &vfs, (void *)fs);

  if (ret) {

  #if ESP32_SPIFFS_VFS_DBG >= 1 
  printf("|SPIFFS_RegisterVFS, err=%i>"
      ,ret);
  #endif

  }

  else {

  #if ESP32_SPIFFS_VFS_DBG >= 3 
  printf("|SPIFFS_RegisterVFS, OK (%i)>"
      ,ret);
  #endif

  }

  return ret;

}









/**
 * Mount the spiffs file system.  Once mounted, register it as a
 * virtual file system.
 */
int
ESP32_Platform_MountSPIFFS() {

  spiffs_config cfg;

  int ret;

  cfg.phys_size = MEMORY_SIZE;			// physical size used in SPI Flash
  cfg.phys_addr = DATA_PARTITION_START;		// physical offset in spi flash, must be on block boundary
  cfg.phys_erase_block = SECTOR_SIZE;		// allowed flash erase size
  cfg.log_block_size = SECTOR_SIZE;     	// logical size of a block, must be on physical block size
                                       	 	// boundary and must never be less than a physical block
  cfg.log_page_size = PAGE_SIZE;     		// logical size of a page, at least log_block_size/8

  cfg.hal_read_f = esp32_spi_flash_read;
  cfg.hal_write_f = esp32_spi_flash_write;
  cfg.hal_erase_f = esp32_spi_flash_erase;

  #if ESP32_SPIFFS_VFS_DBG >= 3 
  printf("|Loading SPIFFS at address 0x%x>"
      ,cfg.phys_addr);
  #endif


  ret = SPIFFS_mount(&fs
      ,&cfg
      ,spiffs_work_buf
      ,spiffs_fds
      ,sizeof(spiffs_fds)
      ,spiffs_cache_buf
      ,sizeof(spiffs_cache_buf)
      ,0);

  if (ret) {

      #if ESP32_SPIFFS_VFS_DBG >= 3 
      printf("|Error: rc from spiffs mount = %d>"
          ,ret);
      #endif

  }

  // mount succesful?
  if (!ret) {

      ESP32_Platform_SPIFFSRegisterVFS(SCDE_VFS_MOUNTPOINT, &fs);

  }

  return ret;

}



