// SCDE-Command Save

#ifndef SAVE_COMMAND_H
#define SAVE_COMMAND_H



#include "SCDE_s.h"



int Save_InitializeCommandFn(SCDERoot_t* SCDERootptr);

struct headRetMsgMultiple_s Save_CommandFn(const uint8_t *argsText, const size_t argsTextLen);



#endif /*SAVE_COMMAND_H*/
