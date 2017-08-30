// SCDE-Command Include

#ifndef INCLUDE_COMMAND_H
#define INCLUDE_COMMAND_H



#include "SCDE_s.h"



int Include_InitializeCommandFn(SCDERoot_t* SCDERootptr);

struct headRetMsgMultiple_s Include_CommandFn(const uint8_t *argsText, const size_t argsTextLen);



#endif /*INCLUDE_COMMAND_H*/
