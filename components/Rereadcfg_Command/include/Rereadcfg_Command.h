// SCDE-Command Rereadcfg

#ifndef REREADCFG_COMMAND_H
#define REREADCFG_COMMAND_H



#include "SCDE_s.h"



int Rereadcfg_InitializeCommandFn(SCDERoot_t* SCDERootptr);

struct headRetMsgMultiple_s Rereadcfg_CommandFn(const uint8_t *argsText, const size_t argsTextLen);



#endif /*REREADCFG_COMMAND_H*/
