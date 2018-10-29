// SCDE-Command Rename

#ifndef RENAME_COMMAND_H
#define RENAME_COMMAND_H



#include "SCDE_s.h"



int Rename_InitializeCommandFn(SCDERoot_t* SCDERootptr);

struct headRetMsgMultiple_s Rename_CommandFn (const uint8_t *args, const size_t argsLen);



#endif /*RENAME_COMMAND_H*/
