/*
 *##################################################################################################
 *  Function: Schemes for SCD-Engine
 *  ESP 8266EX SOC Activities ...
 *  (C) EcSUHA
 *  Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany 
 *  MSchulze780@GMAIL.COM
 *  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
 *##################################################################################################
 */

#include "ProjectConfig.h"
#include <esp8266.h>
#include "WebIf_Module.h"
#include "Platform.h"


#include "WebIf_Module_Schemes.h"



/* List of implemented schemes in SCD-Engine -> dynamically assigned
 * B 1      87      0
 * I 5                
 * T 0000000000000000     |       THIS DATA MAY BE PROJECT SPECIFIC !         |
 *   \\\\\\\\\\\\\\\\- 01.http  = default, hypertext transport protocol
 *    \\\\\\\\\\\\\\\- 02.scde  = generic scde for general device control
 *     \\\\\\\\\\\\\\- ??.light = generic light for switching light sources
 *      \\\\\\\\\\\\\- ??.switch= generic switch for switching switches
 *       \\\\\\\\\\\\- ??.clima = generic clima for clima sensors
 *        \\\\\\\\\\\- ??.s0    = generic s0 for s0 counting
 *         \\\\\\\\\\- ??.adc   = generic adc for adc sensors
 *          \\\\\\\\\- ??.
 *           \\\\\\\\- ??.
 *            \\\\\\\- ??.
 *             \\\\\\- ??.
 *              \\\\\- ??.
 *               \\\\- ??.
 *                \\\- ??.
 *                 \\- ??.
 *                  \- ??.
 */

/* NEU
Schemes AvailSchemes[]=
{				// BIT SET (1<<XX) - MAX 16 = uint16_t Bitfield
  {"http"		}	// #01 -> http or no scheme=http
//---
  #if PROJECT_HAS_SCDE == TRUE
 ,{"scde"
  #endif		}	// #02 -> generic scde for general device control
//---
  #if PROJECT_HAS_LIGHT == TRUE
 ,{"light"		}	// #?? -> generic light for switching light sources
  #endif
//---
  #if PROJECT_HAS_SWITCH == TRUE
 ,{"switch"		}	// #?? -> generic switch for switching switches
  #endif
//---
  #if PROJECT_HAS_CLIMA == TRUE
 ,{"clima"		}	// #?? -> generic clima for clima sensors
  #endif
//---
  #if PROJECT_HAS_S0 == TRUE
 ,{"s0"			}	// #?? -> generic s0 for s0 counting
  #endif
//---
  #if PROJECT_HAS_ADC == TRUE
 ,{"adc"		}	// #?? -> generic adc for adc sensors
  #endif
//---
  #if PROJECT_HAS_FX == TRUE
 ,{"fx"			}	// #?? -> generic adc for adc sensors
  #endif
//---
  #if PROJECT_HAS_RFID == TRUE
 ,{"rfid"		}	// #?? -> generic rfid for RF-ID reading / writing
  #endif
//---
 ,{NULL			}	// #17 -> end of table mark, not found!, unknown scheme! 
*/

// alt
Schemes AvailSchemes[] = {
				// BIT SET (1<<XX) - MAX 16 = uint16_t Bitfield
  {"http"		}	// #01 -> http or no scheme=http
 ,{"scde"		}	// #02 -> generic scde for general device control
 ,{"light"		}	// #03 -> generic light for switching light sources
 ,{"switch"		}	// #04 -> generic switch for switching switches
 ,{"clima"		}	// #05 -> generic clima for clima sensors
 ,{"s0"			}	// #06 -> generic s0 for s0 counting
 ,{"adc"		}	// #07 -> generic adc for adc sensors
 ,{"fx"			}	// #08 -> generic adc for adc sensors
 ,{"rfid"		}	// #09 -> generic rfid for RF-ID reading / writing
 ,{NULL			}	// #16 -> end of table mark, not found!, unknown scheme! 
};




