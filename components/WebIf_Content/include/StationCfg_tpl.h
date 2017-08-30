#ifndef StationCfg_tpl_H
#define StationCfg_tpl_H

#include "WebIF_Module.h"


extern SelectAData DisEn[];
extern SelectAData MaxConn[];
extern SelectAData Channel[];
extern SelectAData Auth_M[];
extern SelectAData SSID_H[];
extern SelectAData NrAP[];
extern SelectAData PhMode[];



void StationCfg_tpl(WebIF_HTTPDConnSlotData_t *connData, char *token, void **arg);



#endif /* StationCfg_tpl_H */
