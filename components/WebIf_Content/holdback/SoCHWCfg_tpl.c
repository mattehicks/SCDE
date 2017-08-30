//##################################################################################################
//###  Function: Callback for System-on-Chip Hardware Configuration (tpl resource)
//###  ESP 8266EX SOC Activities ...
//###  (C) EcSUHA
//###  Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany 
//###  MSchulze780@GMAIL.COM
//###  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
//##################################################################################################
#include "ProjectConfig.h"



#include "c_types.h"
#include <string.h>
#include <osapi.h>
#include "user_interface.h"
#include "mem.h"
#include "SCDED.h"
#include "SCDE.h"
#include "espmissingincludes.h"

//#include "CGI_SoCCfg.h"
#include "CGI_NoAuthErr.h"
#include "CGI_NotFoundErr.h"



#include "SoCHWCfg_tpl.h"



// HTML GUI ELEMENTS
SelectAData SEL_SOCGenOpt[]={  //ID, Text MAX CGI LEN BEACHTEN!!!
  {1,"Enable 160 MHZ Speedup"},
  {0, NULL}
  };

/*
 *--------------------------------------------------------------------------------------------------
 *FName: SoCHWCfg_tpl
 * Desc: Template %X% replacement Code for the SOCHWCfg.htm Page
 * Info: WARNING: CHECK BUFFERSIZE
 * Para: WebIF_HTTPDConnSlotData_t *conn -> ptr to connection slot
 *       char *token -> ptr to string which contains the token to replace
 *       void **arg -> ???
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR 
SoCHWCfg_tpl(WebIF_HTTPDConnSlotData_t *conn, char *token, void **arg) 
{
  char buff[600];	
  if (token==NULL) return;

  switch(token[0])
	{

//--------------------------------------------------------------------------------------------------

	case 'A':				// 'A' -> Version of SDK
	os_sprintf(buff,
		"%s",
		system_get_sdk_version());
	break;

//--------------------------------------------------------------------------------------------------

	case 'B':				// 'B' -> Chip ID
	os_sprintf(buff,
		"%X",
		system_get_chip_id());
	break;

//--------------------------------------------------------------------------------------------------

	case 'C':				// 'C' -> Boot Version
	os_sprintf(buff,
		"%d",
		system_get_boot_version());
	break;

//--------------------------------------------------------------------------------------------------

	case 'D':				// 'D' -> Firmware Version
	os_sprintf(buff,
		"V %u.%u, Build %u",
		(unsigned int) SysCfgRamFlashMirrored->Version_major,
		(unsigned int) SysCfgRamFlashMirrored->Version_minor,
		(unsigned int) SysCfgRamFlashMirrored->build);
	break;

//--------------------------------------------------------------------------------------------------

	case 'E':				// 'E' -> Boot Partition
	os_sprintf(buff,
		"%X",
		system_upgrade_userbin_check());
	break;

//--------------------------------------------------------------------------------------------------

	case 'F':				// 'F' -> FlashChipID
	os_sprintf(buff,
		"%X",
		spi_flash_get_id());
	break;

//--------------------------------------------------------------------------------------------------

	case 'G':				// 'G' -> last Reset Information
	{
	struct rst_info *rst_i;

	rst_i = system_get_rst_info();

	os_sprintf(buff, "reason: %08X - exccause: %08X epc1: %08X - epc2: %08X - epc3: %08X excvaddr: %08X - depc: %08X",
		rst_i->reason,rst_i->exccause,
		rst_i->epc1,
		rst_i->epc2,
		rst_i->epc3,
		rst_i->excvaddr,
		rst_i->depc);
	}
	break;

//--------------------------------------------------------------------------------------------------

	case 'H':				// 'H' -> SoC General Options ->X1 Result
	CheckboxA((char*) &buff,
		SEL_SOCGenOpt,
		SysCfgRamFlashMirrored->SoC_GenOpt,
		"X1",
		"SOCHWCfg.cgi");
	break;

//--------------------------------------------------------------------------------------------------

	default:				// Not found? Zero terminate!
	buff[0] = 0;
	break;

//--------------------------------------------------------------------------------------------------
	}

  SCDED_Send(conn, buff, -1);
}



