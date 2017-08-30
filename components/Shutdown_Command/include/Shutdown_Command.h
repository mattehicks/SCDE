// SCDE-Command Shutdown

#ifndef SHUTDOWN_COMMAND_H
#define SHUTDOWN_COMMAND_H



#include "SCDE_s.h"



int Shutdown_InitializeCommandFn(SCDERoot_t* SCDERootptr);

struct headRetMsgMultiple_s Shutdown_CommandFn (const uint8_t *args, const size_t argsLen);



#endif /*SHUTDOWN_COMMAND_H*/
