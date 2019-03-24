#ifndef PROJECTCONFIG_H
#define PROJECTCONFIG_H


#define FREERTOS
#define ESP32




// new debugging verbose setting starts here
//                                      0 no logging
//                                        1 fatal message
//                                          2 very important
//                                            3 important
//                                              4 good to know
//                                                5 all necessary (default)
//                                                  6 debug builds
//                                                    7 developers debug builds
//                                                      8 for detail debugging
//                                                        9  bug searching with all available

// Platform logging verbose - override  0 1 2 3 4 5 6 7 8 9
#define CORE_PLATFORM_DBG                             7

// SCDE-CORE logging verbose - override 0 1 2 3 4 5 6 7 8 9
#define CORE_SCDE_DBG                                 7
#define Helpers_SCDE_DBG                              7

// Command logging verbose - override   0 1 2 3 4 5 6 7 8 9
#define Add_Command_DBG                               7
#define Attr_Command_DBG                                  9
#define Define_Command_DBG                            7
#define Deleteattr_Command_DBG                            9
#define Delete_Command_DBG                            7
#define Help_Command_DBG                              7
#define Include_Command_DBG                           7
#define List_Command_DBG                              7
#define Rename_Command_DBG                            7
#define Rereadcfg_Command_DBG                         7
#define Save_Command_DBG                              7
#define Set_Command_DBG                               7
#define Setstate_Command_DBG                          7
#define Shutdown_Command_DBG                          7
#define Sub_Command_DBG                               7
#define IOWrite_Command_DBG                           7

// Module logging verbose - override    0 1 2 3 4 5 6 7 8 9
#define BH1750_Module_DBG                             7
#define BMP180_Module_DBG                             7
#define ESP32_DeVICE_Module_DBG                       7
#define ESP32_I2C_Master_Module_DBG                   7
#define ESP32_S0_Module_DBG                           7
#define ESP32_SwITCH_Module_DBG                       7
#define SSD1306_Module_DBG                            7
#define Telnet_Module_DBG                                 9
#define Webif_Module_DBG                                  9


// Used X logging verbose - override    0 1 2 3 4 5 6 7 8 9
#define HTTPD_DBG                                         9













//old debugging verbose starts here


// OLD verbose + debug info cfg         0 1 2 3 4 5 6 7 8 9
#define WebIF_EspFSStdFileTX_DBG                4
#define WebIF_EspFSAdvFileTX_DBG                4
#define SCDED_DBG                                 5
#define SCDEH_DBG                                 5
#define SCDE_FS_DBG                     0


//old module
//#define ESP32_S0_DBG                              5
//#define ESP32_SwITCH_DBG                          5
//#define ESP32_CONTROL_DBG              0
//#define ESP32_BH1770_DBG                          5
#define ESP32_SPIFFS_VFS_DBG                      5




#endif /*PROJECTCONFIG_H*/
