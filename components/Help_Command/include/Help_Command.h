// Command Help for SCDE

#ifndef HELP_COMMAND_H
#define HELP_COMMAND_H


/*
 *  SCDE Typedefs and Structures
 */
#include "SCDE_s.h"



// the initialize Fn
int Help_InitializeCommandFn(SCDERoot_t* SCDERootptr);

// the main command Fn
struct headRetMsgMultiple_s Help_CommandFn (const uint8_t *args, const size_t argsLen);



#endif /*HELP_COMMAND_H*/
