// Command IOWRITE for SCDE

#ifndef IOWRITE_COMMAND_H
#define IOWRITE_COMMAND_H


/*
 *  SCDE Typedefs and Structures
 */
#include "SCDE_s.h"



// the initialize Fn
int IOWrite_InitializeCommandFn(SCDERoot_t* SCDERootptr);

// the main command Fn
struct headRetMsgMultiple_s IOWrite_CommandFn (const uint8_t *args, const size_t argsLen);



#endif /*IOWRITE_COMMAND_H*/
