// SCDE-Command Sub

#ifndef SUB_COMMAND_H
#define SUB_COMMAND_H



#include "SCDE_s.h"



int Sub_InitializeCommandFn(SCDERoot_t* SCDERootptr);

struct headRetMsgMultiple_s Sub_CommandFn (const uint8_t *args, const size_t argsLen);



#endif /*SUB_COMMAND_H*/
