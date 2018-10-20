// Command Delete for SCDE

#ifndef DELETE_COMMAND_H
#define DELETE_COMMAND_H


/*
 *  SCDE Typedefs and Structures
 */
#include "SCDE_s.h"



// the initialize Fn
int Delete_InitializeCommandFn(SCDERoot_t* SCDERootptr);

// the main command Fn
struct headRetMsgMultiple_s Delete_CommandFn (const uint8_t *args, const size_t argsLen);



#endif /*DELETE_COMMAND_H*/
