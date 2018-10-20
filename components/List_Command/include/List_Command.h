// Command List for SCDE

#ifndef LIST_COMMAND_H
#define LIST_COMMAND_H


/*
 *  SCDE Typedefs and Structures
 */
#include "SCDE_s.h"



// the initialize Fn
int List_InitializeCommandFn(SCDERoot_t* SCDERootptr);

// the main command Fn
struct headRetMsgMultiple_s List_CommandFn (const uint8_t *args, const size_t argsLen);



#endif /*LIST_COMMAND_H*/
