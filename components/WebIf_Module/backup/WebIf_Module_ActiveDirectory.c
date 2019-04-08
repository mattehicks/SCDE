#include "ProjectConfig.h"
#include <esp8266.h>
#include "Platform.h"
#include "SCDED_Platform.h"

#include "WebIf_Module.h"

//#include "../WebIF_Content/include/HTools_cgi.h"
#include "HTools_cgi.h"


#include <WebIf_EspFSStdFileTX.h>
#include <WebIf_EspFSAdvFileTX.h>

#include <ServAPCfg_tpl.h>
#include <CGI_Redirect.h>





//http://192.168.0.110/Images/Ecsuha.gif
//http://192.168.0.110/WiFi/ServAPCfg.htm



/*
 *--------------------------------------------------------------------------------------------------
 *DName: BuiltInUrls
 * Desc: Resource-Content-structure of active Directory - PART A (Resource-Row)
 * Data: (struct HTTPDUrls from HttpD.h)
 *       uint32_t AllowedMethodBF -> Allowed (implemented) Method for this resource (Bit-Field)
 *       uint16_t AllowedDocMimeBF -> Allowed (implemented) DocMimes for this resource (Bit-Field)
 *       uint16_t AllowedSchemeBF -> Allowed (implemented) Schemes for this resource (Bit-Field)
 *       uint16_t free -> not used
 *       uint8_t CgiNo -> Assigns the Cgi-Data-Row (Content Table B) for this resource
 *       uint8_t EnaByBit -> Defines the bit (in HttpDDirEnaCtrl) that enables the entry for dir customization; 0 = always ena
 *       const char Url -> Ptr to URL Name of this resource (that is verified and processed)
 *--------------------------------------------------------------------------------------------------
 */
// Content:   AllowedMethodBF          | AllowedDocMimeBF | AllowedSchemeBF  |free|CgiNo| EnaByBit | Url
const WebIf_ActiveResourcesDataA_t BuiltInUrlsX[] = //ICACHE_RODATA_ATTR =
{
// Device Home Page
  // Device-Feature Control
  { 0b00000000000000000000000000001010, 0b0000000000000010, 0b0000000000000001,  0,  0, 0b00000000, "/CoNTROL_8S-1C-1ADC"}
  // RENAMED Device-Feature Control
 ,{ 0b00000000000000000000000000001010, 0b0000000000000010, 0b0000000000000001,  0,  0, 0b00000000, "/\xf0\x00 CoNTROL_8S-1C-1ADC.htm"}	// ADC_DevName

  // Device-Feature Control - Home Automation SCDE:// access Resources
 ,{ 0b00000000000000000000000000000110, 0b0000000000010001, 0b0000000000000010,  0,  0, 0b00000000, "/CoNTROL_8S-1C-1ADC"}
  // RENAMED Device-Feature Control - Home Automation SCDE:// access Resources
 ,{ 0b00000000000000000000000000000110, 0b0000000000010001, 0b0000000000000010,  0,  0, 0b00000000, "/\xf0"}	// ADC_DevName

  // Switch-Feature Control
 ,{ 0b00000000000000000000000000001010, 0b0000000000011110, 0b0000000000000001,  0,  1, 0b00000000, "/\xf1\x00\x08/SwITCH\x00 SwITCHX.htm"} // ADC_FeatNr
  // RENAMED Switch-Feature Control
 ,{ 0b00000000000000000000000000001010, 0b0000000000011110, 0b0000000000000001,  0,  1, 0b00000000, "/\xf3\x00\x08\x00 SwITCHX.htm"} // ADC_SwITCHFN
  // Multi Switch(es)-Feature Control
 ,{ 0b00000000000000000000000000001010, 0b0000000000001010, 0b0000000000000001,  0,  2, 0b00000000, "/SwITCH"}

  // Switch-Feature Control - Home Automation Scheme SwITCH:// access Resources
 ,{ 0b00000000000000000000000000000110, 0b0000000000010001, 0b0000000000001000,  0,  1, 0b00000000, "/\xf1\x00\x08"} // ADC_FeatNr
  // RENAMED Switch-Feature Control - Home Automation SwITCH:// access Resources
 ,{ 0b00000000000000000000000000000110, 0b0000000000010001, 0b0000000000001000,  0,  1, 0b00000000, "/\xf3\x00\x08"} // ADC_SwITCHFN

  // Clima-Feature Control
 ,{ 0b00000000000000000000000000001010, 0b0000000000011110, 0b0000000000000001,  0,  3, 0b00000000, "/\xf1\x00\x01/ClIMA\x00 ClIMAX.htm"} // ADC_FeatNr
  // RENAMED Clima-Feature Control
 ,{ 0b00000000000000000000000000001010, 0b0000000000011110, 0b0000000000000001,  0,  3, 0b00000000, "/\xf4\x00\x01\x00 ClIMAX.htm"} // ADC_ClIMAFN
  // Multi ClIMA(s)-Feature Control
 ,{ 0b00000000000000000000000000001010, 0b0000000000001010, 0b0000000000000001,  0,  4, 0b00000000, "/ClIMA"}

  // ClIMA-Feature Control - Home Automation Scheme ClIMA:// access Resources
 ,{ 0b00000000000000000000000000000110, 0b0000000000010001, 0b0000000000010000,  0,  3, 0b00000000, "/\xf1\x00\x01"} // ADC_FeatNr
  // RENAMED ClIMA-Feature Control - Home Automation ClIMA:// access Resources
 ,{ 0b00000000000000000000000000000110, 0b0000000000010001, 0b0000000000010000,  0,  3, 0b00000000, "/\xf4\x00\x01"} // ADC_ClIMAFN	

  // ADC Feature-Control
 ,{ 0b00000000000000000000000000001010, 0b0000000000011110, 0b0000000000000001,  0,  5, 0b00000000, "/\xf1\x00\x01/ADC\x00 ADCX.htm"} // ADC_FeatNr
  // RENAMED ADC-Feature Control
 ,{ 0b00000000000000000000000000001010, 0b0000000000011110, 0b0000000000000001,  0,  5, 0b00000000, "/\xf6\x00\x01\x00 ADCX.htm"} // ADC_ADCFN
  // Multi ADC(s)-Feature Control
 ,{ 0b00000000000000000000000000001010, 0b0000000000001010, 0b0000000000000001,  0,  6, 0b00000000, "/ADC"}

  // ADC-Feature Control - Home Automation Scheme ADC:// access Resources
 ,{ 0b00000000000000000000000000000110, 0b0000000000010001, 0b0000000001000000,  0,  5, 0b00000000, "/\xf1\x00\x01"} // ADC_FeatNr
  // RENAMED ADC-Feature Control - Home Automation ADC:// access Resources
 ,{ 0b00000000000000000000000000000110, 0b0000000000010001, 0b0000000001000000,  0,  5, 0b00000000, "/\xf6\x00\x01"} // ADC_ADCFN

  // SERVICE PAGES
 ,{ 0b00000000000000000000000000001010, 0b0000000000000110, 0b0000000000000001,  0,  7, 0b00000000, "/BasServ"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000000110, 0b0000000000000001,  0,  8, 0b00000000, "/AdvServ"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000000110, 0b0000000000000001,  0,  9, 0b00000000, "/SpcAttr"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000000110, 0b0000000000000001,  0, 10, 0b00000000, "/GenAttr"}

  // Device Cfg
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0, 11, 0b00000000, "/DeviceCfg"}

  // SOC Hardware Cfg
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0, 12, 0b00000000, "/SoCHWCfg"}

  // WiFi Stations Cfg
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0, 13, 0b00000000, "/WiFi/StationCfg"}

  // WIFI Q-Connect PAGES
 ,{ 0b00000000000000000000000000001010, 0b0000000000000010, 0b0000000000000001,  0, 14, 0b00000000, "/WiFi/QConnect"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000001000, 0b0000000000000001,  0, 15, 0b00000000, "/WiFi/WiFiScan"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000000100, 0b0000000000000001,  0, 16, 0b00000000, "/WiFi/Connect"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000000100, 0b0000000000000001,  0, 17, 0b00000000, "/WiFi/Setmode"}

  // Service Access Point Cfg
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0, 18, 0b00000000, "/WiFi/ServAPCfg"}

  // TimeStamp Cfg
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0, 19, 0b00000000, "/TiStCfg"}

  // Firmware Update
 ,{ 0b00000000000000000000000000001010, 0b0000000000001110, 0b0000000000000001,  0, 20, 0b00000000, "/Firmware"}

  // Redirects
 ,{ 0b00000000000000000000000000001010, 0b0000000000000001, 0b0000000000000001,  0, 21, 0b00000000, "/WiFi"}	// nomime
 ,{ 0b00000000000000000000000000001010, 0b0000000000000001, 0b0000000000000001,  0, 21, 0b00000000, "/WiFi/"}	// nomime
 ,{ 0b00000000000000000000000000001010, 0b0000000000000001, 0b0000000000000001,  0, 22, 0b00000000, "/"}	// nomime
 ,{ 0b00000000000000000000000000001010, 0b0000000000000010, 0b0000000000000001,  0, 22, 0b00000000, "/index"}	// .htm

  // Secret services
 ,{ 0b00000000000000000000000000001010, 0b0000000000100000, 0b0000000000000001,  0, 23, 0b00000000, "/32MBitFlash"}
 ,{ 0b00000000000000000000000000001010, 0b0000000000100000, 0b0000000000000001,  0, 24, 0b00000000, "/wfs"}

 ,{0,0,0,0,0,0,"*"}
};



/*
 *--------------------------------------------------------------------------------------------------
 *DName: BuiltInActivePages
 * Desc: Resource-Content-structure of active Directory - PART B (Cgi-Data-Row) 
 * Data: (struct HTTPDActivePages from HttpD.h)
 *       const uint32_t CgiFucID -> 	
 *       cgiSendCallback cgiCb -> Callback Procedure which processes this active resource
 *       const void *cgiArg -> Argument (Data), passed to / accessible by the Callback Procedure
 *--------------------------------------------------------------------------------------------------
 */
// CgiFucID=(No.<<16)+AllowedSchemeBF |      cgi            |     cgi_data
const WebIf_ActiveResourcesDataB_t BuiltInActiveResourcesX[] = //ICACHE_RODATA_ATTR =
  {
  // Device-Feature Control
  {(0<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	Device_tpl	}
 ,{(0<<16) +	0b0000000000010001,	NULL,NULL}//	SCDE_set,		NULL		}//Device_set,

  // Switch-Feature Control
 ,{(1<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	SwITCH_tpl	}
 ,{(1<<16) +	0b0000000000000100,	NULL,NULL}//	SwITCH_cgi,		NULL		}
 ,{(1<<16) +	0b0000000000001000,	NULL,NULL}//	SwITCH_jso,		NULL		}
 ,{(1<<16) +	0b0000000000010001,	NULL,NULL}//	SwITCH_set,		NULL		}

  // Multi Switch(es)-Feature Control
 ,{(2<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	SwITCH_tpl	}
 ,{(2<<16) +	0b0000000000001000,	NULL,NULL}//	SwITCH_jso,		NULL		}

  // Clima-Feature Control
 ,{(3<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	ClIMA_tpl	}
 ,{(3<<16) +	0b0000000000000100,	NULL,NULL}//	ClIMA_cgi,		NULL		}
 ,{(3<<16) +	0b0000000000001000,	NULL,NULL}//	ClIMA_jso,		NULL		}
 ,{(3<<16) +	0b0000000000010001,	NULL,NULL}//	ClIMA_set,		NULL		}

  // Multi ClIMA(s)-Feature Control
 ,{(4<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	ClIMA_tpl	}
 ,{(4<<16) +	0b0000000000001000,	NULL,NULL}//	ClIMA_jso,		NULL		}

  // ADC-Feature Control
 ,{(5<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	ADC_tpl		}
 ,{(5<<16) +	0b0000000000000100,	NULL,NULL}//	ADC_cgi,		NULL		}
 ,{(5<<16) +	0b0000000000001000,	NULL,NULL}//	ADC_jso,		NULL		}
 ,{(5<<16) +	0b0000000000010001,	NULL,NULL}//	ADC_set,		NULL		}

  // Multi ADC(s)-Feature Control
 ,{(6<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	ADC_tpl		}
 ,{(6<<16) +	0b0000000000001000,	NULL,NULL}//	ADC_jso,		NULL		}

  // ### SERVICE PAGES ###
 ,{(7<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	BasServ_tpl	}
 ,{(7<<16) +	0b0000000000000100,	NULL,NULL}//	BasServ_cgi,		NULL		}

 ,{(8<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	AdvServ_tpl	}
 ,{(8<<16) +	0b0000000000000100,	NULL,NULL}//	AdvServ_cgi,		NULL		}

 ,{(9<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	SpcAttr_tpl	}
 ,{(9<<16) +	0b0000000000000100,	NULL,NULL}//	SpcAttr_cgi,		NULL		}

 ,{(10<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	GenAttr_tpl	}
 ,{(10<<16) +	0b0000000000000100,	NULL,NULL}//	GenAttr_cgi,		NULL		}

  // ### Device Cfg ###
 ,{(11<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	DeviceCfg_tpl	}
 ,{(11<<16) +	0b0000000000000100,	NULL,NULL}//	DeviceCfg_cgi,		NULL		}
 ,{(11<<16) +	0b0000000000001000,	NULL,NULL}//	DeviceCfg_jso,		NULL		}

  // ### SOC Hardware Cfg ###
 ,{(12<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	SoCHWCfg_tpl	}
 ,{(12<<16) +	0b0000000000000100,	NULL,NULL}//	SoCHWCfg_cgi,		NULL		}
 ,{(12<<16) +	0b0000000000001000,	NULL,NULL}//	SoCHWCfg_jso,		NULL		}

  // ### WiFi Station Cfg ###
 ,{(13<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	StationCfg_tpl	}
 ,{(13<<16) +	0b0000000000000100,	NULL,NULL}//	StationCfg_cgi,		NULL		}
 ,{(13<<16) +	0b0000000000001000,	NULL,NULL}//	StationCfg_jso,		NULL		}

  // ### WIFI Q-Connect PAGES ###
 ,{(14<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	WifiQConnect_tpl}
 ,{(15<<16) +	0b0000000000001000,	NULL,NULL}//	WiFiScan_jso,		NULL		}
 ,{(16<<16) +	0b0000000000000100,	NULL,NULL}//	WiFiConnect_cgi,	NULL		}
 ,{(17<<16) +	0b0000000000000100,	NULL,NULL}//	WifiSetMode_cgi,	NULL		}	

  // ### Service Access Point Cfg ###
 ,{(18<<16) +	0b0000000000000010,	WebIf_EspFSAdvFileTX,	ServAPCfg_tpl	}
 ,{(18<<16) +	0b0000000000000100,	NULL,NULL}//	ServAPCfg_cgi,		NULL		}
 ,{(18<<16) +	0b0000000000001000,	NULL,NULL}//	ServAPCfg_jso,		NULL		}

  // ### TimeStamp Cfg ###
 ,{(19<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	TiStCfg_tpl	}
 ,{(19<<16) +	0b0000000000000100,	NULL,NULL}//	TiStCfg_cgi,		NULL		}
 ,{(19<<16) +	0b0000000000001000,	NULL,NULL}//	TiStCfg_jso,		NULL		}

  // ### Firmware Update ###
 ,{(20<<16) +	0b0000000000000010,	NULL,NULL}//	EspFsTemplate_cgi,	FirmwareUd_tpl	}
 ,{(20<<16) +	0b0000000000000100,	NULL,NULL}//	FirmwareUd_cgi,		NULL		}
 ,{(20<<16) +	0b0000000000001000,	NULL,NULL}//	FirmwareUd_jso,		NULL		}

  // ### Redirects ###
 ,{(21<<16) +	0b0000000000000001,	cgiRedirect,	"/WiFi/QConnect.htm"		}
 ,{(22<<16) +	0b0000000000000001,	cgiRedirect,	"/CoNTROL_8S-1C-1ADC.htm"	}
 ,{(22<<16) +	0b0000000000000010,	cgiRedirect,	"/CoNTROL_8S-1C-1ADC.htm"	}

  // ### Secret services ###
 ,{(23<<16) +	0b0000000000100000,	ReadFullFlash_cgi,	NULL		}		//.bin
 ,{(24<<16) +	0b0000000000100000,	NULL,NULL}//	WriteFileSystem_cgi,	NULL		}		//.bin
  };

