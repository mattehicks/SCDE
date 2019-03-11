// SCDE-Module ESP32_TouchGUI1_Module (IO-Device for SPI Hardware)

#ifndef ESP32_TouchGUI1_MODULE_S_H
#define ESP32_TouchGUI1_MODULE_S_H

// -------------------------------------------------------------------------------------------------

// this Module is made for the Smart-Connected-Device-Engine
#include "SCDE_s.h"

// this Module provides functions for other Modules:
#include "WebIf_Module global types.h"
//#include "WebIf_Module_s.h"

// this Module uses an 1st stage:
#include "ESP32_SPI_Module_s.h"

// -------------------------------------------------------------------------------------------------

// whats needed?
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "soc/spi_struct.h"
#include "esp_intr.h"
#include "esp_intr_alloc.h"
#include "rom/lldesc.h"

#include "freertos/ringbuf.h"
//#include "tft.h"
// -------------------------------------------------------------------------------------------------

// ptr zur Fehlermeldung in allokierter strTextMultiple_t WENN NOCH KEIN FEHLER WAR
#define SCDE_OK  NULL



// -------------------------------------------------------------------------------------------------

// display details
typedef struct DisplayConfig_s {
  uint8_t tft_disp_type;  	///< Display type, DISP_TYPE_ILI9488 or DISP_TYPE_ILI9341
} DisplayConfig_t;



// -------------------------------------------------------------------------------------------------



/*
 * typedefs of ESP32_SPI_Module Function Callbacks
 * This Fn are provided & made accessible for client modules - for operation
 */
/*
// typedef for ESP32_SPI_Module_spi_bus_add_deviceFn - 
typedef esp_err_t (*ESP32_SPI_spi_bus_add_deviceFn_t) (ESP32_SPI_Module_spi_host_device_t host, ESP32_SPI_Module_spi_bus_config_t *bus_config, ESP32_SPI_Module_spi_device_interface_config_t *dev_config, ESP32_SPI_Module_spi_device_handle_t *handle);
*/


/*
 * ESP32 SPI provided Fn - table
 * Stores common + custom functions this Module provides to the SCDE (and client Modules)
 */
typedef struct ESP32_TouchGUI1_ProvidedByModule_s {
// --- first the provided common module functions ---
  ProvidedByModule_t common;							// the common fn
// --- now the provided custom module fuctions ---
  // none!									// ?
} ESP32_TouchGUI1_ProvidedByModule_t;



// -------------------------------------------------------------------------------------------------



/** 
 * ESP32_TouchGUI1_Definition stores values for operation valid only for the defined instance of an
 * loaded module. Values are initialized by HCTRLD an the loaded module itself.
 */
typedef struct ESP32_TouchGUI1_Definition_s {
  Common_Definition_t common;			/*!< ... the common part of the definition */
  WebIf_Provided_t WebIf_Provided;		/*!< provided data for WebIf */


  size_t stage1definitionNameLen;	// Stange 1 name assigned by def
  uint8_t *stage1definitionName;	// Stange 1 nameLen assigned by def

  ESP32_SPI_Definition_t* ESP32_SPI_Definition; // stage 1 link -> wirklich speichern?

 time_t time_last;


  // Display dimensions
//  int _width;
//  int _height;

  // Converts colors to grayscale if set to 1
//  uint8_t gray_scale;

  // the display configuration
  DisplayConfig_t display_config;

  // globals for tft painting?
  TFTGlobals_t TFT_globals;

  // Spi device configuration + handle - for display
  ESP32_SPI_device_interface_config_t disp_interface_config;
  ESP32_SPI_device_handle_t disp_handle;

  // Spi device configuration + handle - for touch screen
  ESP32_SPI_device_interface_config_t ts_interface_config;
  ESP32_SPI_device_handle_t ts_handle;

//  uint8_t i2c_num;				/*!< the I2C hardware that should be used */
//  i2c_config_t i2c_config;			/*!< i2c configuration */
//  i2c_obj_t *i2c_obj;				/*!< the current i2c job */
 // ESP32_TouchGUI1_Fn_t* ESP32_TouchGUI1_Fn;	/*!< ESP32_I2C_MasterFn (Functions / callbacks) accessible for client modules */
} ESP32_TouchGUI1_Definition_t;



// -------------------------------------------------------------------------------------------------



/* 
 * ESP32_TouchGUI1_StageXCHG stores values for operation of 2. stage design Modules (stage exchange)
 */
typedef struct ESP32_TouchGUI1_StageXCHG_s {
  Common_StageXCHG_t common;		// ... the common part of the StageXCHG
  // module specific here 

} ESP32_TouchGUI1_StageXCHG_t;



// -------------------------------------------------------------------------------------------------



#endif /*ESP32_TouchGUI1_MODULE_S_H*/
