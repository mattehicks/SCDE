// Command Help for SCDE

#ifndef SET_COMMAND_H
#define SET_COMMAND_H


/*
 *  SCDE Typedefs and Structures
 */
#include "SCDE_s.h"



// the initialize Fn
int Set_InitializeCommandFn(SCDERoot_t* SCDERootptr);

// the main command Fn
struct headRetMsgMultiple_s Set_CommandFn (const uint8_t *args, const size_t argsLen);



#endif /*SET_COMMAND_H*/
