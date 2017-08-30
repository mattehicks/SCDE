// SCDE-Command Deleteattr

#ifndef DELETEATTR_COMMAND_H
#define DELETEATTR_COMMAND_H



#include "SCDE_s.h"



int Deleteattr_InitializeCommandFn(SCDERoot_t* SCDERootptr);

struct headRetMsgMultiple_s Deleteattr_CommandFn (const uint8_t *args, const size_t argsLen);



#endif /*DELETEATTR_COMMAND_H*/
