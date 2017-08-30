// SCDE-Command Attr

#ifndef ATTR_COMMAND_H
#define ATTR_COMMAND_H



#include "SCDE_s.h"



int Attr_InitializeCommandFn(SCDERoot_t* SCDERootptr);

struct headRetMsgMultiple_s Attr_CommandFn (const uint8_t *args, const size_t argsLen);



#endif /*ATTR_COMMAND_H*/
