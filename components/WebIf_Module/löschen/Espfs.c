/*
This is a simple read-only implementation of a file system. It uses a block of data coming from the
mkespfsimg tool, and can use that block to do abstracted operations on the files that are in there.
It's written for use with httpd, but doesn't need to be used as such.
*/


//These routines can also be tested by comping them in with the espfstest tool. This
//simplifies debugging, but needs some slightly different headers. The #ifdef takes
//care of that.



//temp till fixed
#define __ets__ //__ESP_ATTR_H__
#define SCDE_FS_DBG  5


#ifdef __ets__
// esp build
#include "ProjectConfig.h"
#include <esp8266.h>
#include <SCDE_Main.h>
//#include <WebIF_Module.h>

//#include "c_types.h"
//#include "user_interface.h"
//#include "espconn.h"
//#include "mem.h"
//#include "osapi.h"
//#include "espmissingincludes.h"
//#include "SCDE.h"
//#include "ProjectConfig.h"
#else
// PC Test build
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define os_malloc malloc
#define os_free free
#define os_memcpy memcpy
#define os_strncmp strncmp
#define os_strcmp strcmp
#define os_strcpy strcpy
#define os_printf printf
#define ICACHE_FLASH_ATTR
//extern char* espFsData;
#endif



#include "../../../espfs/espfsformat.h"
#include "Espfs.h"


#ifdef EFS_HEATSHRINK
#include "heatshrink_decoder.h"
#endif


/*
 *  Pointer to files system data, assigned during init
 */
static char* espFsData = NULL;



/*
 *  Struct of a Fs-File
 */
struct EspFsFile
  {
  EspFsHeader *header;
  char decompressor;
  int32_t posDecomp;
  char *posStart;
  char *posComp;
  void *decompData;
  };



//##################################################################################################
//### FName: espFsInit
//###  Desc: Init the data-adress of FS in flash, ant test
//###  Para: ptr to ESPFS in flash
//###  Rets: result
//##################################################################################################
EspFsInitResult ICACHE_FLASH_ATTR
espFsInit(void *flashAddress)
  {

  # if SCDE_FS_DBG >= 3
  os_printf("Espfs at:%X\n",
	(unsigned int) flashAddress);
  # endif

  # if SCDE_FS_DBG >= 5
  SCDEFn->HexDumpOutFn ("FS-Preview", flashAddress, 1000);
  # endif

/*  // flash access correction - if we are using flash-read-commands (replaced by memcpy)
  if ((uint32_t)flashAddress > 0x40200000)

	{

	flashAddress = (void*)((uint32_t)flashAddress-0x40200000);

	}
*/

  // base address must be aligned to 4 bytes
  if (((int) flashAddress & 3) != 0)

	{

	return ESPFS_INIT_RESULT_BAD_ALIGN;

	}

  // check if there is valid header at address
  EspFsHeader testHeader;

  memcpy((uint32_t *) &testHeader
	,flashAddress
	,sizeof(EspFsHeader));

  // check if its fs
  if (testHeader.magic != ESPFS_MAGIC)

	{

	return ESPFS_INIT_RESULT_NO_IMAGE;

	}

  espFsData = (char *) flashAddress;

  # if SCDE_FS_DBG >= 4
  os_printf("Espfs OK!:\n")
  # endif

  return ESPFS_INIT_RESULT_OK;

  }







/*
 *--------------------------------------------------------------------------------------------------
 *FName: ReadFSDataBlockFromUnalignedFlashA
 * Desc: Reads File-System data block from unaligned flash - proc A -> complete copy
 *       no return of the copied length!
 * Para: char *dst -> prt to destination
 *       char *src -> prt to source data in flash
 *       int len -> max len to copy
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR // replace by SCDE_memcpy_plus
ReadFSDataBlockFromUnalignedFlashA(char *dst
			,char *src
			,int len)
  {

//do we have a flash offset?  src += 0x40200000;

  while (len--) *dst++ = (char) SCDE_read_rom_uint8((uint8_t *) src++);

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: ReadFSDataBlockFromUnalignedFlashB
 * Desc: Reads File-System data block from unaligned flash - proc B -> breaks AFTER '%' and returns
 *       the copied length
 * Para: char *dst -> prt to destination
 *       char *src -> prt to source data in flash
 *       int len -> max len to copy
 * Rets: int -> real copied length
 *--------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR
ReadFSDataBlockFromUnalignedFlashB(char *dst
			,char *src
			,int len)
  {

//do we have a flash offset?    src += 0x40200000;

  int ReadLen = 0;

  while (len--)

	{

	uint8_t  ReadByte = SCDE_read_rom_uint8((uint8_t *) src++);

	*dst++ = (char)ReadByte;

	ReadLen++;

	if (ReadByte == '%') break;

	}

  return ReadLen;

  }




/*

//##################################################################################################
//Copies len bytes over from dst to src, but does it using *only*
//aligned 32-bit reads. Yes, it's no too optimized but it's short and sweet and it works.
//ToDo: perhaps os_memcpy also does unaligned accesses?
#ifdef __ets__
void ICACHE_FLASH_ATTR
//readFlashUnaligned(char *dst, char *src, int len)
ReadFSDataBlockFromUnalignedFlashA(char *dst, char *src, int len)
{
  uint8_t src_offset = ((uint32_t)src) & 3;
  uint32_t src_address = ((uint32_t)src) - src_offset;

  uint32_t tmp_buf[len/4 + 2];
  spi_flash_read((uint32)src_address, (uint32*)tmp_buf, len+src_offset);
  os_memcpy(dst, ((uint8_t*)tmp_buf)+src_offset, len);
}
#else
#define readFlashUnaligned memcpy
#endif


#ifdef __ets__
int ICACHE_FLASH_ATTR
//readFlashUnaligned(char *dst, char *src, int len)
ReadFSDataBlockFromUnalignedFlashB(char *dst, char *src, int len)
{
  uint8_t src_offset = ((uint32_t)src) & 3;
  uint32_t src_address = ((uint32_t)src) - src_offset;

  uint32_t tmp_buf[len/4 + 2];
  spi_flash_read((uint32)src_address, (uint32*)tmp_buf, len+src_offset);
  os_memcpy(dst, ((uint8_t*)tmp_buf)+src_offset, len);
 return len;
}
#else
#define readFlashUnaligned memcpy
#endif

*/

/*oldcode
//##################################################################################################
//### FName: memcpyAligned
//###  Desc: Copies len bytes over from dst to src, but does it using *only*
//###  aligned 32-bit reads. Yes, it's no too optimized but it's short and sweet and it works.
//###  Para: 
//###  Rets: 
//##################################################################################################
void ICACHE_FLASH_ATTR
memcpyAligned(char *dst, char *src, int len)
{
  int x;
  int w, b;
  for (x=0; x<len; x++)
	{
	b=((int)src&3);
	w=*((int *)(src-b));
	if (b==0) *dst=(w>>0);
	if (b==1) *dst=(w>>8);
	if (b==2) *dst=(w>>16);
	if (b==3) *dst=(w>>24);
	dst++; src++;
	}
}
//##################################################################################################
*/








/*
 *--------------------------------------------------------------------------------------------------
 *FName: espFsFlags
 * Desc: Returns the flags of an opened file.
 * Info: 
 * Para: EspFsFile *fh -> pointer to allocated mem for the file handle
 * Rets: int flags -> flags of opened file
 *--------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR
espFsFlags(EspFsFile *fh)
  {

  if (fh == NULL) {

  	# if SCDE_FS_DBG >= 1
	os_printf("\nFh not ready\n");
	# endif

	return -1;

	}

  int8_t flags;

  ReadFSDataBlockFromUnalignedFlashA((char*)&flags
	,(char*)&fh->header->flags
	,1);

  return (int)flags;

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: EspFsFile
 * Desc: Open a file and return a pointer to the file desc struct.
 * Info: 
 * Para: char *fileName -> pointer to filename
 * Rets: EspFsFile* -> pointer to allocated mem for the file desc struct
 *--------------------------------------------------------------------------------------------------
 */
EspFsFile* ICACHE_FLASH_ATTR
espFsOpen(char *fileName) 
  {

  char *p = espFsData;
  char *hpos;
  char namebuf[256];
  EspFsHeader h;
  EspFsFile *r;

  // Strip initial slashes
  while (fileName[0] == '/') fileName++;

  // loop through files and find that file!
  while(1)

	{

	hpos = p;

	// Grab the next file header.
	memcpy(&h, p, sizeof(EspFsHeader));
//opt   spi_flash_read((uint32)p, (uint32*)&h, sizeof(EspFsHeader));

	if (h.magic != 0x73665345)

		{

		# if SCDE_FS_DBG >= 1
		os_printf("|FS IMAGE ERROR!>");
		# endif

		return NULL;

		}


	if (h.flags & FLAG_LASTFILE)

		{

		# if SCDE_FS_DBG >= 1
		os_printf("|End of image.>");
		# endif

		return NULL;

		}

	// Grab the name of the file.
	p += sizeof(EspFsHeader); 

	memcpy(&namebuf, p, sizeof(namebuf));
//opt	spi_flash_read((uint32)p, (uint32*)&namebuf, sizeof(namebuf));

	# if SCDE_FS_DBG >= 4
	os_printf("|Found file '%s'. Namelen=%x fileLenComp=%x, compr=%d flags=%d>", 
		namebuf,
		(unsigned int)h.nameLen,
		(unsigned int)h.fileLenComp,
		h.compression,
		h.flags);
	# endif

	if (strcmp(namebuf, fileName) == 0)

		{

		// Yay, this is the file we need!
		p += h.nameLen; // Skip to content.

		// Alloc file desc mem
		r = (EspFsFile *) malloc(sizeof(EspFsFile));

		if (r == NULL) return NULL;

		r->header = (EspFsHeader *)hpos;
		r->decompressor = h.compression;
		r->posComp = p;
		r->posStart = p;
		r->posDecomp = 0;

//--------------------------------------------------------------------------------------------------

		if (h.compression == COMPRESS_NONE)

			{

			r->decompData = NULL;
#ifdef EFS_HEATSHRINK
			} 

//--------------------------------------------------------------------------------------------------

		else if (h.compression==COMPRESS_HEATSHRINK)

			{

			// File is compressed with Heatshrink.
			char parm;

			heatshrink_decoder *dec;

			// Decoder params are stored in 1st byte.
//oldcode			memcpyAligned(&parm, r->posComp, 1);
			ReadFSDataBlockFromUnalignedFlashA(&parm, r->posComp, 1);

			r->posComp++;

			# if SCDE_FS_DBG >= 4
			os_printf("Heatshrink compressed file; dec parms = %x\n",
				parm);
			# endif

			dec = heatshrink_decoder_alloc(16, (parm >> 4) & 0xf, parm & 0xf);

			r->decompData = dec;
#endif
			}

//--------------------------------------------------------------------------------------------------

		 else

			{

			# if SCDE_FS_DBG >= 1
			os_printf("Invalid compression: %d\n"
				,h.compression);
			# endif

			return NULL;

			}

//--------------------------------------------------------------------------------------------------

		return r;

		}

	// We don't need this file. Skip name and file
	p += h.nameLen + h.fileLenComp;

	if ((int)p&3) p+=4-((int)p&3); // align to next 32bit val

	}
  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: espFsClose
 * Desc: Close the file (frees mem filehandle and decomp-data)
 * Para: EspFsFile *fh -> ptr to the file handle
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR
espFsClose(EspFsFile *fh)
  {
	if (fh == NULL) return;

#ifdef EFS_HEATSHRINK
	if (fh->decompressor == COMPRESS_HEATSHRINK)

		{

		// free mem used for decompression
		heatshrink_decoder *dec = (heatshrink_decoder *)fh->decompData;
		heatshrink_decoder_free(dec);

		}

#endif

  // free mem for file handle
  free(fh);

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: espFsRead //SoCFS_ReadLen
 * Desc: Read len bytes from the given file into buff.
 * Info: len may be variable. Ideal to fill the TX-Buffer completly. 
 * Para: EspFsFile *fh -> ptr to the file handle
 *       char *buff -> ptr to the destination buffer
 *       int len -> size of buffer
 * Rets: int -> Returns the actual amount of bytes read
 *--------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR
espFsRead(EspFsFile *fh, char *buff, int len)
  {

  // file length
  int flen;

  // file decompressed length
  int fdlen;

  if (fh == NULL) return 0;

  // read file length from flash to 'int flen'
  ReadFSDataBlockFromUnalignedFlashA( (char*)&flen
	,(char*)&fh->header->fileLenComp
	,4);
//oldcode  memcpyAligned((char*)&flen, (char*)&fh->header->fileLenComp, 4);

//---------------------------------------------------------------------------------------------------

  // read uncompressed ?
  if (fh->decompressor == COMPRESS_NONE)

	{

	int toRead;

	// clc rest length to read -> toRead
	toRead = flen-(fh->posComp-fh->posStart);

	// rest of file shorter than read len?
	if (len > toRead) len = toRead;

	#if SCDE_FS_DBG >= 4
	os_printf("|reading, limit:%d, range:%d -",
		len,
		(unsigned int)fh->posComp);
	#endif

	// copy data, till '%' or len reached
	len = ReadFSDataBlockFromUnalignedFlashB(buff, fh->posComp, len);

	fh->posDecomp += len;
	fh->posComp += len;

	#if SCDE_FS_DBG >= 4
	os_printf("%d, real len:%d>"
		,(unsigned int)fh->posComp
		,len);
	#endif

	return len;

#ifdef EFS_HEATSHRINK

	}

//---------------------------------------------------------------------------------------------------

  // read HEATSHRINK-compressed ?
  else if (fh->decompressor == COMPRESS_HEATSHRINK)

	{

	// copy/read files decompressed length from flash to 'int fdlen'
	ReadFSDataBlockFromUnalignedFlashA((char*)&fdlen
		,(char*)&fh->header->fileLenDecomp
		,4);

	// decoded bytes counter
	int decoded = 0;

	// rest-length of encoded data to do, 16 byte blocks processed, 0 = finnished
	unsigned int elen;

	// resulting decompressed read length
	unsigned int rlen;

	// our decompressor feed buffer
	char ebuff[16];

	heatshrink_decoder *dec = (heatshrink_decoder *)fh->decompData;

	// are we already finnished ?
	if (fh->posDecomp == fdlen)

		{

		return 0;

		}

	// file decode an poll (read) loop, 
	// breaks if length-limit reached, char '%', or file read finnished
	while (decoded < len)

		{

		// Feed data into the decompressor
		// ToDo: Check ret val of heatshrink fns for errors
		elen = flen - (fh->posComp - fh->posStart);

		// is there something to encode ?
		if (elen > 0)

			{

			// read a file-system data block from unaligned flash
			ReadFSDataBlockFromUnalignedFlashA(ebuff
				,fh->posComp
				,16);

			// decode data
			heatshrink_decoder_sink(dec
				,(uint8_t *)ebuff
				,(elen>16)?16:elen
				,&rlen);

			// store ne pos in compressed file
			fh->posComp += rlen;

			}


		// poll decompressed data and put as much as allowed by len into buff 
		heatshrink_decoder_poll(dec
			,(uint8_t *)buff
			,len-decoded
			,&rlen);

		// check for special character to stop reading for token processing
		bool stop = false;
		int rlenOK = 0;

		while (rlenOK < rlen)

			{

			rlenOK++;

			if (*buff++ == '%')

				{

				stop = true;
			//	break;

				} 
			}

		// correct ptrs
		fh->posDecomp += rlenOK;
		decoded += rlenOK;

		#if SCDE_FS_DBG >= 5
		os_printf("\n|Elen:%d,rlen:%d,decbytes:%d,posDecomp:%d,fdlen:%d>"
			,elen           // curent position in encoded file
			,rlenOK         // resulting, valid bytes from one decompressing cycle
			,decoded        // this call decompressed bytes counter
			,fh->posDecomp  // current position in decompressed file
			,fdlen);	// file decompressed length
		#endif


		// We must ensure that whole file is decompressed and written to output buffer.
		// This means even when there is no input data (elen==0) try to poll decoder until
		// posDecomp equals decompressed file length. Then file is complete.
		if (elen == 0)

			{

			// are we finnished with file ?
			if (fh->posDecomp == fdlen)

				{

				#if SCDE_FS_DBG >= 5
				os_printf("|Dec finnished>");
				#endif

				heatshrink_decoder_finish(dec);

				}

			return decoded;

			}

		// special character found? then break loop
		if (stop == true) break;

		}

	return decoded;

//---------------------------------------------------------------------------------------------------

#endif
	}

  return 0;

  }

/*
 01f0  0a 20 20 3c 70 3e 55 53 45 20 46 49 52 45 50 52  .  <p>USE FIREPR
|Elen:173,rlen:1,d:1,pd:2455,fdl:2830>
|Elen:173,rlen:18,d:19,pd:2473,fdl:2830>
|Elen:157,rlen:29,d:48,pd:2502,fdl:2830>
|Elen:141,rlen:29,d:77,pd:2531,fdl:2830>
|Elen:125,rlen:16,d:93,pd:2547,fdl:2830>
|Elen:109,rlen:44,d:137,pd:2591,fdl:2830>
|Elen:93,rlen:52,d:189,pd:2643,fdl:2830>
|Elen:77,rlen:14,d:203,pd:2657,fdl:2830>
|Elen:61,rlen:54,d:257,pd:2711,fdl:2830>
|Elen:45,rlen:54,d:311,pd:2765,fdl:2830>
|Elen:29,rlen:23,d:334,pd:2788,fdl:2830>
|Elen:13,rlen:42,d:376,pd:2830,fdl:2830>
|Elen:0,rlen:0,d:376,pd:2830,fdl:2830>
|Dec finnished>
|read:376, max:512,TX>EspFsStdRead-HEX:
  0000  4f 4f 46 20 48 4f 55 53 49 4e 47 3c 2f 70 3e 0d  OOF HOUSING</p>.
*/



