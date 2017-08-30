#ifndef CGI_REDIRECT_H
#define CGI_REDIRECT_H

#include "WebIf_Module.h"



int cgiRedirect(WebIf_HTTPDConnSlotData_t *Conn);
int cgiRedirectToHostname(WebIf_HTTPDConnSlotData_t *Conn);
int cgiRedirectApClientToHostname(WebIf_HTTPDConnSlotData_t *Conn);


//void HTTPDRedirect(WebIF_HTTPDConnSlotData_t *Conn, char *newUrl);


#endif /* CGI_REDIRECT_H */

