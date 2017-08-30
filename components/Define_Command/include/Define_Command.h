// SCDE-Command Define

#ifndef DEFINE_COMMAND_H
#define DEFINE_COMMAND_H



#include "SCDE_s.h"



int Define_InitializeCommandFn(SCDERoot_t* SCDERootptr);

struct headRetMsgMultiple_s Define_CommandFn (const uint8_t *args, const size_t argsLen);



#endif /*DEFINE_COMMAND_H*/
