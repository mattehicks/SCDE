// SCDE-Command Add

#ifndef ADD_COMMAND_H
#define ADD_COMMAND_H



#include "SCDE_s.h"



int Add_InitializeCommandFn(SCDERoot_t* SCDERootptr);

struct headRetMsgMultiple_s Add_CommandFn (const uint8_t *args, const size_t argsLen);



#endif /*ADD_COMMAND_H*/
