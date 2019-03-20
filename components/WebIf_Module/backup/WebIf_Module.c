/*
 *##################################################################################################
 *  Function: HTTP Daemon
 *  ESP 8266EX SOC Activities ...
 *  (C) EcSUHA
 *  Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany 
 *  MSchulze780@GMAIL.COM
 *  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
 *##################################################################################################
 */

#include "ProjectConfig.h"
#include <esp8266.h>
#include "WebIf_Module.h"
#include "WebIf_Module_Global.h"
#include "Platform.h"
#include "SCDED_Platform.h"

#include <SCDE_Main.h>





//----------------------- new stuff ------------------------


#include <stdio.h>
#include <SCDE_s.h>

#include <WebIf_Module.h>
#include <WebIf_Module global.h>
#include <WebIf_Module global types.h>

#include "lwip/sockets.h"



//----------------------- new stuff ------------------------



#include <WebIf_EspFSStdFileTX.h>
//#include <WebIf_EspFSAdvFileTX.h>

//#include <ServAPCfg_tpl.h>

#include <WebIf_ActiveDirTokens.h>



#define SYS_CPU_80MHz 80
#define SYS_CPU_160MHz 160


// helper ...
#define CHECK_BIT(var,pos) ((var) & (1<<(0)))



// (S)mart (C)onnected (D)evice Engine Version
#define HTTP_PARSER_VERSION_MAJOR 2
#define HTTP_PARSER_VERSION_MINOR 6
#define HTTP_PARSER_VERSION_PATCH 1



// Compile with -D HTTP_PARSER_STRICT=0 to make less checks (code runs faster)
#ifndef HTTP_PARSER_STRICT
# define HTTP_PARSER_STRICT 0
#endif


/*
// HTTPD authentication definitions
#define HTTP_AUTH_REALM "Protected"
#define AUTH_MAX_USER_LEN 32
#define AUTH_MAX_PASS_LEN 32
*/



// Max allowed connection inactivity at HTTPD at normal state (no longpoll)
#define HTTPD_TIMEOUT 60 //30


// Max post buffer len
//#define MAX_POST 1460 // Max Empfangslänge 1024

// Max send buffer len
#define MAX_SENDBUFF_LEN 2048



// Maximium header size allowed. If the macro is not defined
// before including this header then the default is used. To
// change the maximum header size, define the macro in the build
// environment (e.g. -DHTTP_MAX_HEADER_SIZE=<value>). To remove
// the effective limit on the size of the header, define the macro
// to a very large number (e.g. -DHTTP_MAX_HEADER_SIZE=0x7fffffff) !!!war (80*1024)!!!
#ifndef HTTP_MAX_HEADER_SIZE
# define HTTP_MAX_HEADER_SIZE (1024)
#endif






/*
listen www-https
    bind :443 ssl crt /etc/haproxy/ssl.pem  no-tls-tickets ciphers ECDHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-RSA-RC4-SHA:AES128-GCM-SHA256:AES256-GCM-SHA384:AES128-SHA256:AES128-SHA:AES256-SHA256:AES256-SHA:RC4-SHA
    rspadd Strict-Transport-Security:\ max-age=31536000
    reqadd X-Forwarded-Proto:\ https
    compression algo gzip
    compression type text/html text/plain text/javascript application/javascript application/xml text/css
    default_backend apache
*/





#include "WebIf_Module_Mimes.h"
#include "WebIf_Module_Schemes.h"
#include "WebIf_Module_ActiveDirectory.h"
#include "WebIf_ActiveDirTokens.h"
#include "Base64.h"
#include "sha1.h"

//#include "SCDE.h"
//#include "Espfs.h"
//#include "HttpDespfs.h"
//#include "CGI_WebSocket.h"
//#include "utils.h"
//#include "sntp.h"

#include "CGI_NoAuthErr.h"
#include "CGI_NotFoundErr.h"
//#include "CGI_Redirect.h"



// NOTE: SCDED directory content enable control - if bit 0-31 is set, its equivalent content is  enabled!
// SCDED_DirConEnaCtrl should be assigned by define in Project Config!
//static uint32_t SCDED_DirConEnaCtrl = 0;




//--------------------------------------------------------------------------------------------------

// List of implemented header fields
// http:// list to all fields
HdrFlds AvailHdrFlds[]=
{
  {"Authorization"			,NULL	},			// #00 ->
  {"Host"				,HTTPDHdrFldHostFnCb	},	// #01 ->
  {"Sec-WebSocket-Key"			,NULL			},	// #02 -> 
  {NULL					,NULL},				// not found!
};

//--------------------------------------------------------------------------------------------------

// SCDE Event-TX-Connect-Control - State (enum) Monitoring
enum ETXCC_State
  { s_idle = 0					// #000 idle state, !! no connection control !!
  , s_delay_then_idle = 8*10-1			// #079 connection control - delay idle state
  , s_dns_lookup_failed	= 80			// #080 connection control - DNS lookup failed state
  , s_monitoring_dns_lookup = 80+8*10-1		// #159 connection control - monitoring DNS lookup to get IP adress
  , s_conn_proc_IP_failed = 160			// #160 connection control - connection process to IP failed state
  , s_monitoring_conn_proc_IP = 160+8*10-1	// #239 connection control - monitoring connection process to an IP adress
  };

// SCDE Event-TX-Connect-Control State
  static int ETXCC_State = s_delay_then_idle;	// delay after boot up ...

// SCDE Event-TX - STATISTICS
  int SCDEETX_ConnCNT = 0;	// connections made counter
  int SCDEETX_MsgCNT = 0;	// messages sent from queue counter
  int SCDEETX_RcvOKCNT = 0;	// messages OK sent (determinated by response code)
  int SCDEETX_RcvNOKCNT = 0;	// Error counter: retrys, ... ?

//--------------------------------------------------------------------------------------------------






































/*
//##############################################################################
//### FName: HTTPDGetHeader
//###  Desc: Copys the content of a header field indicated in *header 
//###        to *ret with maxlen retlen
//###  Para: WebIf_HTTPDConnSlotData_t *conn
//###	     char *header -> Ptr to header start
//###        char *ret -> ptr to reserved mem-space to write result found
//###        int retLen -> length of result in bytes
//###  Rets: Int:Bool: FALSE=0= not found / TRUE=1= found ok
//##############################################################################
//Get the value of a certain header in the HTTP client head
//Returns true when found, false when not found.
int ICACHE_FLASH_ATTR 
HTTPDGetHeader(WebIf_HTTPDConnSlotData_t *conn, char *header, char *ret, int retLen)
{
  char *p = conn->priv->head;			// to beginning of header 

  p=p+strlen(p)+1;				// skip ROW 1: GET/POST part
  p=p+strlen(p)+1;				// skip ROW 2: HTTP part

  while (p<(conn->priv->head+conn->priv->headPos))
	{
	while (*p <= 32 && *p != 0) p++; // skip crap at start

	//See if this is the header
	if (strncmp(p, header, strlen(header)) == 0 && p[strlen(header)] == ':')
		{
		// Skip 'key:' bit of header line
		p = p + strlen(header)+1;

		// Skip past spaces after the colon
		while (*p == ' ') p++;

		// Copy from p to end
		while (*p != 0 && *p != '\r' && *p != '\n' && retLen > 1)
			{
			*ret++ = *p++;
			retLen--;
			}

		// Zero-terminate string
		*ret = 0;

		// All done :)
		return 1;
		}
	p += strlen(p)+1; // Skip past end of string and \0 terminator
	}
  return 0;
}
*/







//  httpdFindArgHexVal(connData->postBuff,	// Test for arg and copy sting
//	 "passwd", (char*)stconf.password, sizeof(stconf.password));


//  Mark job in load serializer as finnished (by deleting bit)
//  LoadSerializer &= ~(1<<(conn->SlotNo));
























#include <ctype.h>

double ICACHE_FLASH_ATTR
atofown(char *s)
{
	double a = 0.0;
	int e = 0;
	int c;
	while ((c = *s++) != '\0' && isdigit(c)) {
		a = a*10.0 + (c - '0');
	}
	if (c == '.') {
		while ((c = *s++) != '\0' && isdigit(c)) {
			a = a*10.0 + (c - '0');
			e = e-1;
		}
	}
	if (c == 'e' || c == 'E') {
		int sign = 1;
		int i = 0;
		c = *s++;
		if (c == '+')
			c = *s++;
		else if (c == '-') {
			c = *s++;
			sign = -1;
		}
		while (isdigit(c)) {
			i = i*10 + (c - '0');
			c = *s++;
		}
		e += i*sign;
	}
	while (e > 0) {
		a *= 10.0;
		e--;
	}
	while (e < 0) {
		a *= 0.1;
		e++;
	}
	return a;
}

/*
char * ICACHE_FLASH_ATTR
StringToLower(char *string)
{
	int i;
	int len = strlen(string);
	for(i=0; i<len; i++) {
		if(string[i] >= 'A' && string[i] <= 'Z') {
			string[i] += 32;
		}
	}
	return string;
}
*/

/*
char ICACHE_FLASH_ATTR
CharToLower(char *string)
{
	int i;
	int len = strlen(string);
	for(i=0; i<len; i++) {
		if(string[i] >= 'A' && string[i] <= 'Z') {
			string[i] += 32;
		}
	}
	return string;
}
*/

//int strncasecmp(const char *s1, const char *s2, size_t n);




//2/* Compare S1 and S2, ignoring case, returning less than, equal to or
//43   greater than zero if S1 is lexicographically less than,
//44   equal to or greater than S2.  */ returns 0 if equal must be zero terminated

int ICACHE_FLASH_ATTR
strcasecmp (const char *s1, const char *s2)
{

  const unsigned char *p1 = (const unsigned char *) s1;
  const unsigned char *p2 = (const unsigned char *) s2;

  int result;

  if (p1 == p2)
  return 0;

  while ((result = tolower (*p1) - tolower (*p2++)) == 0)
	{
	if (*p1++ == '\0') break;
	}

  return result;
}

int ICACHE_FLASH_ATTR
strncasecmp (const char *s1, const char *s2, size_t length)
{

  const unsigned char *p1 = (const unsigned char *) s1;
  const unsigned char *p2 = (const unsigned char *) s2;

  int result=0;

  if (p1 == p2)
  return 0;

  for (; length != 0; length--)
	{
	if ( (result = tolower (*p1++) - tolower (*p2++) ) != 0) break;
	}
  return result;
}





/*
 *----------------------------------------------------------------------
 *
 * strcasecmp --
 *
 *	Compares two strings, ignoring case differences.
 *
 * Results:
 *	Compares two null-terminated strings s1 and s2, returning -1, 0,
 *	or 1 if s1 is lexicographically less than, equal to, or greater
 *	than s2.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */
/*
int
strcasecmp(s1, s2)
    CONST char *s1;			 First string. /
    CONST char *s2;			/ Second string. /
{
    unsigned char u1, u2;

    for ( ; ; s1++, s2++) {
	u1 = (unsigned char) *s1;
	u2 = (unsigned char) *s2;
	if ((u1 == '\0') || (charmap[u1] != charmap[u2])) {
	    break;
	}
    }
    return charmap[u1] - charmap[u2];
}
*/
/*
 *----------------------------------------------------------------------
 *
 * strncasecmp --
 *
 *	Compares two strings, ignoring case differences.
 *
 * Results:
 *	Compares up to length chars of s1 and s2, returning -1, 0, or 1
 *	if s1 is lexicographically less than, equal to, or greater
 *	than s2 over those characters.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */
/*
int
strncasecmp(s1, s2, length)
    CONST char *s1;		* First string. *
    CONST char *s2;		* Second string. *
    size_t length;		* Maximum number of characters to compare
				 * (stop earlier if the end of either string
				 * is reached). *
{
    unsigned char u1, u2;

  for (; length != 0; length--, s1++, s2++)
	{
	u1 = (unsigned char) *s1;
	u2 = (unsigned char) *s2;
	if (charmap[u1] != charmap[u2])
		{
		return charmap[u1] - charmap[u2];
		}
	if (u1 == '\0')
		{
		return 0;
		}
	}
  return 0;
}
*/
































/*
 *--------------------------------------------------------------------------------------------------
 *FName: HTTPDFindStrFromKey
 * Desc: Hepler routine that searches and extracts an string value matching the given key. 
 *       Source data is an key=value pair string, seperated by an "&" from an query string
 * Para: char *query -> pointer to source string from query
 *       char *key -> pointer to key-string we are searching for
 *       char *buff-> pointer where the string will be be stored
 *       int buffLen -> maximum allowed result length

 *       char *dest-> destination pointer where the string will be be stored
 *       int destLen -> maximum allowed result length storeable in destination

 * Rets: int -> false or number of chars copied to destination
 *--------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR 
HTTPDFindStrFromKey(char *query, char *key, char *buff, int buffLen) 
{
  char *p; // start ptr
  char *e; // end ptr 
  int keylen = strlen(key); // lenght of key

  p = query;

  while (p!=NULL && *p!='\n' && *p!='\r' && *p!=0)
  	{

	//printf("Find Key:\"%s\" in query:\"%s\"\n",
	//	key,
	//	p);

	// matches value? and is "=" after value?
	if ( (strncmp(p, key, strlen(key))==0) && (p[keylen]=='=') ) 
  		{
		// move p to start of value
		p += keylen+1;

		// move e to end of value
		e = (char*) strstr(p, "&");

		// or is it last Key-Value pair?
		if (e==NULL) e = p + strlen(p);

		
		//printf("Key:fnd!,val:\"%s\",len %d\n",
		//	p,
		//	(e-p));

		return SCDED_UrlDecode(p, (e-p), buff, buffLen);
		}

	// jump after next "&"
	p=(char*)strstr(p, "&");
	if (p!=NULL) p+=1;
	}


//printf("Key:\"%s\" in \"%s\"- not fnd!\n",
//	key,
//	query);


  // not found
  return false;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: HTTPDFindSpecialStrFromKey
 * Desc: Hepler routine that searches and extracts an SPECIAL string value matching the given key. 
 *       Source data is an key=value pair string, seperated by an "&" from an query string
 *       WARNING: FOR FIRST and LAST CHAR ONLY ALPHA OR NUMMERICALAL IS ALLOWED !	  
 * Para: char *query -> pointer to source string from query
 *       char *key -> pointer to key-string we are searching for
 *       char *dest-> destination pointer where the string will be be stored
 *       int destLen -> maximum allowed result length storeable in destination
 *       int allowed -> 
 *                      case 1 = [azAZ09.-] Alpha+Num+".-" for Domain Name
 *                      case 2 = [azAZ09._-] Alpha+Num+"._-" for Feature Name
 * Rets: int -> false or number of chars copied to destination
 *--------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR
HTTPDFindSpecialStrFromKey(char *query, char *key, void *dest, int destLen, int allowed)
{
  uint8_t decBuf[destLen];

  if ( HTTPDFindStrFromKey(query, key, (char*) &decBuf, destLen) )
	{

//  printf("gotback:\"%s\"",
//	decBuf);

	char* xptr = NULL;
	switch (allowed)
		{

		// #1 = CharStr (Alpha+Num+".-") -> return length (incl. \0) and write str to dest if found
		case 1:
		xptr = "-"; //".-";	// ??????????   Domain Name allowed chars
		break;

		// #2 = CharStr (Alpha+Num+"_.") -> return length (incl. \0) and write str to dest if found
		case 2:
		xptr = "_.";	// Feature Name allowed chars
		break;

		// #3 = CharStr (Alpha+Num+"³") -> return length (incl. \0) and write str to dest if found
		case 3:
		xptr = "³";	// Unit Name allowed chars
		break;
		}

	// now allowed for allowed chars
	uint8_t *pinput = decBuf;

	// check first char
	if ( isalpha(*pinput) || isalnum(*pinput) )
		{

		++pinput;

		if (*pinput != '\0')
			{
			while (*pinput) 
				{
				if (isalpha(*pinput) || isalnum(*pinput) || strchr(xptr, *pinput) ) 
				++pinput;
				// unacceptable character 
				else break;
				}
			}

		// Sting compatible till \0 ?
		if (*pinput == '\0')
			{

			--pinput;
			// check last char
			if (isalpha(*pinput) || isalnum(*pinput) )
				{
    				strcpy((char*)dest, (char*)&decBuf);

				// printf("dec:\"%s\"",
				//	(char*) dest);

    				return strlen(dest); //destLen;
				}
			}
		}
//?
	//return destLen; // OK OBWOHL FALSCHE ZEICHEN?
	}

  return false;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: HTTPDTestKey
 * Desc: Hepler routine that searches if the given key is available. 
 *       Source data is an key=value pair string, seperated by an "&" from an query string
 * Para: char *query -> pointer to source string from query
 *       char *key -> pointer to key-string we are searching for
 *       char *buff-> pointer where the string will be be stored
 *       int buffLen -> maximum allowed result length
 * Rets: int -> false or number of chars copied to buffer
 *--------------------------------------------------------------------------------------------------
 */
/* this code has problems when no value is present
bool ICACHE_FLASH_ATTR 
HTTPDTestKey(char *query, char *key)
{
  char decBuf[1];

  if ( HTTPDFindStrFromKey(query, key, (char*) &decBuf, 1) )
	{
	return true;
	}

  else return false;
}*/
bool ICACHE_FLASH_ATTR 
HTTPDTestKey(char *query, char *key)
{
  char *p; // start ptr
  char *e; // end ptr 
  int keylen = strlen(key); // lenght of key

  p = query;

  while (p!=NULL && *p!='\n' && *p!='\r' && *p!=0)
  	{

	//printf("Find Key:\"%s\" in query:\"%s\"\n",
	//	key,
	//	p);

	// matches value? and is "=" after value?
	if ( (strncmp(p, key, strlen(key))==0) && (p[keylen]=='=') ) 
  		{
		// move p to start of value
		p += keylen+1;

		// move e to end of value
		e = (char*) strstr(p, "&");

		// or is it last Key-Value pair?

		if (e==NULL) e = p + strlen(p);

		//printf("Key:fnd!,val:\"%s\",len %d\n",
		//	p,
		//	(e-p));

		return true;
		}

	// jump after next "&"
	p=(char*)strstr(p, "&");
	if (p!=NULL) p+=1;
	}

//printf("Key:\"%s\" in \"%s\"- not fnd!\n",
//	key,
//	query);


  // not found
  return false;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: HTTPDFindValueDecInt32FromKey
 * Desc: Hepler routine that searches and extracts an Dec Int 32 Value matching the given key. 
 *       Source data is an key=value pair string, seperated by an "&" from an query string
 * Para: char *query -> pointer to source string from query
 *       char *key -> pointer to key-string we are searching for
 *       int32 *buff-> pointer where the number will be be stored
 * Rets: bool -> true / false + filled buffer
 *--------------------------------------------------------------------------------------------------
 */
bool ICACHE_FLASH_ATTR
HTTPDFindValueDecInt32FromKey(char *query, char *key, int *buff) 
{
  char decBuf[12];

  if ( HTTPDFindStrFromKey(query, key, (char*) &decBuf, 12) )
	{
	*buff = atoi(decBuf);
	return true;
	}

  else return false;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: HTTPDFindValueDecUInt8FromKey
 * Desc: Hepler routine that searches and extracts an Dec UInt 8 Value matching the given key. 
 *       Source data is an key=value pair string, seperated by an "&" from an query string
 * Para: char *query -> pointer to source string from query
 *       char *key -> pointer to key-string we are searching for
 *       uint8 *buff-> pointer where the number will be be stored
 * Rets: bool -> true / false + filled buffer
 *--------------------------------------------------------------------------------------------------
 */
bool ICACHE_FLASH_ATTR 
HTTPDFindValueDecUInt8FromKey(char *query, char *key, uint8_t *buff) 
{
  char decBuf[12];

  if ( HTTPDFindStrFromKey(query, key, (char*) &decBuf, 12) )
	{
	*buff = atoi(decBuf);

	//printf("FA!DecUint8Val:%d\n",
	//	*buff);

	return true;
	}

  else return false;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: HTTPDFindValueDecUInt16FromKey
 * Desc: Hepler routine that searches and extracts an Dec UInt 16 Value matching the given key. 
 *       Source data is an key=value pair string, seperated by an "&" from an query string
 * Para: char *query -> pointer to source string from query
 *       char *key -> pointer to key-string we are searching for
 *       uint16 *buff-> pointer where the number will be be stored
 * Rets: bool -> true / false + filled buffer
 *--------------------------------------------------------------------------------------------------
 */
bool ICACHE_FLASH_ATTR 
HTTPDFindValueDecUInt16FromKey(char *query, char *key, uint16_t *buff) 
{
  char decBuf[12];

  if ( HTTPDFindStrFromKey(query, key, (char*) &decBuf, 12) )
	{
	*buff = atoi(decBuf);

	//printf("FA!DecUint16Val:%d\n",
	//	*buff);

	return true;
	}

  else return false;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: HTTPDFindValueDecUInt32FromKey
 * Desc: Hepler routine that searches and extracts an Dec UInt 32 Value matching the given key. 
 *       Source data is an key=value pair string, seperated by an "&" from an query string
 * Para: char *query -> pointer to source string from query
 *       char *key -> pointer to key-string we are searching for
 *       uint32 *buff-> pointer where the number will be be stored
 * Rets: bool -> true / false + filled buffer
 *--------------------------------------------------------------------------------------------------
 */
bool ICACHE_FLASH_ATTR
HTTPDFindValueDecUInt32FromKey(char *query, char *key, uint32_t *buff) 
{
  char decBuf[12];
  if ( HTTPDFindStrFromKey(query, key, (char*) &decBuf, 12) )
	{
	*buff = atoi(decBuf);

	//printf("FA!DecUint32Val:%d\n",
	//	*buff);

	return true;
	}
  else return false;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: HTTPDFindValueHexUInt8FromKey
 * Desc: Hepler routine that searches and extracts an Hex UInt 8 Value matching the given key. 
 *       Source data is an key=value pair string, seperated by an "&" from an query string
 * Para: char *query -> pointer to source string from query
 *       char *key -> pointer to key-string we are searching for
 *       uint8 *buff-> pointer where the number will be be stored
 * Rets: bool -> true / false + filled buffer
 *--------------------------------------------------------------------------------------------------
 */
bool ICACHE_FLASH_ATTR
HTTPDFindValueHexUInt8FromKey(char *query, char *key, uint8_t *buff) 
{
  char decBuf[12];
  if ( HTTPDFindStrFromKey(query, key, (char*) &decBuf, 12) )
	{
	*buff = strtol(decBuf,NULL,16);

	//printf("FA!HexUint8Val:%d\n",
	//	*buff);

	return true;
	}
  else return false;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: HTTPDFindValueHexUInt32FromKey
 * Desc: Hepler routine that searches and extracts an Hex UInt 32 Value matching the given key. 
 *       Source data is an key=value pair string, seperated by an "&" from an query string
 * Para: char *query -> pointer to source string from query
 *       char *key -> pointer to key-string we are searching for
 *       uint32 *buff-> pointer where the number will be be stored
 * Rets: bool -> true / false + filled buffer
 *--------------------------------------------------------------------------------------------------
 */
bool ICACHE_FLASH_ATTR
HTTPDFindValueHexUInt32FromKey(char *query, char *key, uint32_t *buff) 
{
  char decBuf[12];

  if ( HTTPDFindStrFromKey(query, key, (char*) &decBuf,12) )
	{
	*buff = strtol(decBuf,NULL,16);

	//printf("FA!HexUint32Val:%d\n",
	//	*buff);

	return true;

	}

  else return false;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: HTTPDFindValueFloatFromKey
 * Desc: Hepler routine that searches and extracts an Float Value matching the given key. 
 *       Source data is an key=value pair string, seperated by an "&" from an query string
 * Para: char *query -> pointer to source string from query
 *       char *key -> pointer to key-string we are searching for
 *       float *buff-> pointer where the number will be be stored
 * Rets: bool -> true / false + filled buffer
 *--------------------------------------------------------------------------------------------------
 */
bool ICACHE_FLASH_ATTR
HTTPDFindValueFloatFromKey(char *query, char *key, float *buff)
{
  char decBuf[32];

  if ( HTTPDFindStrFromKey(query, key, (char*) &decBuf,32) )

	{

	*buff = atofown(decBuf);

	//printf("FA!HexUint32Val:%f\n",
	//	*buff);

	return true;

	}

  else return false;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: HTTPDFindValueHexUInt32ColFromKey
 * Desc: Hepler routine that searches and extracts an HEX Color Value matching the given key. 
 *       Source data is an key=value pair string, seperated by an "&" from an query string
 * Para: char *query -> pointer to source string from query
 *       char *key -> pointer to key-string we are searching for
 *       uint32 *buff-> pointer where the 32 Bit number will be stored
 * Rets: bool -> true / false + filled buffer
 *--------------------------------------------------------------------------------------------------
 */
bool ICACHE_FLASH_ATTR
HTTPDFindValueHexUInt32ColFromKey(char *query, char *key, uint32_t *buff) 
{
  char decBuf[12];

  if ( HTTPDFindStrFromKey(query, key, (char*) &decBuf,12) )

	{

	*buff = strtol(decBuf+1,NULL,16);	// jump #

	//printf("Fnd!HexUint32Col:%d\n",
	//	*buff);

	return true;

	}

  else return false;
}










































/*
//##############################################################################
//###  FunctionName : HTTPDFindArgUint8Sel
//###  Description  : Hepler routine to build uint8 Value from Selection Data of get- or post-data
//###  Parameters   : char *line -> pointer to source string of get- or post-data
//### 		      char *arg -> pointer to Text-Arg we are searching for
//### 		      int *buff-> pointer where the 32 Bit number should be stored
//###  Returns      : bool -> true / false
//##############################################################################
int ICACHE_FLASH_ATTR 
HTTPDFindArgUint8Sel(char *line, char *arg, uint8 *buff) 
{
  char decBuf[12];
  if ( HTTPDFindStrFromKey(line, arg, &decBuf) )
	{
	*buff = atoi(decBuf);
	return true;
	}
  else return false;
}
*/



//##############################################################################
//###  FunctionName : httpdFindArgUint8Sel
//###  Description  : Hepler routine to build uint8 Value from Selection Data of get- or post-data
//###  Parameters   : char *line -> pointer to source string of get- or post-data
//### 		      char *arg -> pointer to Text-Arg we are searching for
//### 		      int *buff-> pointer where the 32 Bit number should be stored
//###  Returns      : bool -> true / false
//##############################################################################
int ICACHE_FLASH_ATTR 
httpdFindArgUint8Sel(char *line, char *arg, uint8_t *buff) 
  {

  char *p, *e;
  uint8_t SelVal = 0;
  if (line==NULL) return 0;
  p=line;

  while(p!=NULL && *p!='\n' && *p!='\r' && *p!=0)

  	{
 
//	printf("findArg: %s\n", p);

	if ( (strncmp(p, arg, strlen(arg))==0) && p[strlen(arg)]=='=')	// if Argument found, followed by =? 
  		{
		p+=strlen(arg)+1;	// move p to start of value
		e=(char*)strstr(p, "&");	// e= pointer to val
		if (e==NULL) e=p+strlen(p);

		//printf("findArg: val %s len %d\n", p, (e-p));

//		char tmpbuff[12];	
//		HTTPDUrlDecode(p, (e-p), (char*)&tmpbuff, 10); //buffLen);

		//printf("xxxx %d\n", atoi(p));

		SelVal = SelVal + atoi(p);

//		printf("yyyyyy %d\n", *buff);



//		return true;
		}
	p=(char*)strstr(p, "&");
	if (p!=NULL) p+=1;

	}

	*buff = SelVal;	// save result

//	printf("Finding %s in %s: Not found :/\n", arg, line);

	return true;//false; // not found  BUG gibt nie falsew°!!!!!!!!!!!!!1
}


/* original!!!!!!!1
//##############################################################################
//###  FunctionName : httpdFindArg
//###  Description  : 
//###  Parameters   : char *line -> (source string of get- or post-data),
//### 		      char *arg -> (Text-Arg to find),
//### 		      char *buff-> zero terminated copy of result,
//###		      int buffLen) -> maximum result length
//###  Returns      : length of the result, or -1 if the value wasn't found.
//##############################################################################
int ICACHE_FLASH_ATTR 
httpdFindArg(char *line, char *arg, char *buff, int buffLen) 
{
  char *p, *e;
  if (line==NULL) return 0;
  p=line;
  while(p!=NULL && *p!='\n' && *p!='\r' && *p!=0)
  	{
 
	# if SCDED_DBG >= 1
	printf("findArg: %s\n", p);
	#endif

	if (strncmp(p, arg, strlen(arg))==0 && p[strlen(arg)]=='=') 
  		{
		p+=strlen(arg)+1; //move p to start of value
		e=(char*)strstr(p, "&");
		if (e==NULL) e=p+strlen(p);

 		# if SCDED_DBG >= 1
		printf("findArg: val %s len %d\n", p, (e-p));
		#endif


		return HTTPDUrlDecode(p, (e-p), buff, buffLen);
		}
	p=(char*)strstr(p, "&");
	if (p!=NULL) p+=1;
	}

 	# if SCDED_DBG >= 1
	printf("Finding %s in %s: Not found :/\n", arg, line);
	#endif

	return -1; //not found
}
*/




















/*	if (SCDEH_GetSpecialStrVal((char*)&SysCfgRamFlashMirrored->MySwitchFeatureCfgRamFlashMirrored[ADID].IB1_Name
		,sizeof(SysCfgRamFlashMirrored->MySwitchFeatureCfgRamFlashMirrored[ADID].IB1_Name)
		,Args+QueryResultKF->KeyField_Data_t[XX_QF_DIM].off
		,QueryResultKF->KeyField_Data_t[XX_QF_DIM].len
		,2) )
*/

// remove spaces  
//  while ( len && isspace (*buf) )
//                --len, ++buf;



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDEH_GetSpecialStrVal
 * Desc: Hepler routine that checks and extracts char[len] from buffer. 
 *       Checks data for restrictions of type. Keeps old value in case of wrong content.
 * Para: const uint8_t *Buf -> pointer to source string
 *       uint16_t Len ->  length of NOT zero terminated source string
 *       void *Dest ->
 *       int DestLen ->
 *       int Type -> 
 * Rets: int -> false or number of chars copied to destination
 *--------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR
SCDEH_GetSpecialStrVal(const uint8_t *Buf
			, uint16_t Len
			, char *Dest
			, int DestLen
			, int Type)
  {

  char* xptr = NULL;
  int MinLen = 0;

  switch (Type)
	{

	// #1 = CharStr (Alpha+Num+".-") -> return length (incl. \0) and write str to dest if found
	case 1:
	xptr = "-"; //".-";	// ??????????  allowed chars
	break;

	// #2 = CharStr (Alpha+Num+"_.") -> return length (incl. \0) and write str to dest if found
	case 2:
	xptr = "_.";	// 'Feature Name' & 'Device Name' allowed chars
	MinLen = 1;	// minimum length 1 char
	break;

	// #3 = CharStr (Alpha+Num+"³") -> return length (incl. \0) and write str to dest if found
	case 3:
	xptr = "³";	// Unit Name allowed chars
	break;

	}

  // check for minimum length
  if (MinLen > Len) return false;

  // check for maximum length
  if (Len > DestLen) return false;

  // ptr to destination
  char *POutput = Dest;

  // special (allowd) case: Len = 0 -> skip all (empty) and zero terminated
  if (Len)
	{

	// check first charcter for alpha + alphanum
	if (! (isalpha(*Buf) || isalnum(*Buf) ) ) return false;

	// check last charcter for alpha + alphanum
	if (! (isalpha(Buf[Len-1]) || isalnum(Buf[Len-1]) ) ) return false;

	const uint8_t *PInput;
	int LenToGo;

	PInput = Buf;
	LenToGo = Len;

	// check loop
	while (LenToGo--)

		{

		// check character
		if (isalpha(*PInput) || isalnum(*PInput) || strchr(xptr, *PInput) ) 
			PInput++;

		// unacceptable character 
		else return false;

		}

	PInput = Buf;
	LenToGo = Len;

	// copy loop
	while (LenToGo--)

		{

		*POutput++ = *PInput++;

		}
	}

  // terminate string
  *POutput = '\0';

  #if SCDEH_DBG >= 5
  printf("||X-SpecialStr:%s>"
	,Dest);		
  #endif

  return Len;

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDEH_GetHexUInt32Val
 * Desc: Hepler routine that extracts an hexadecimal unsigned int 32 value from buffer.
 *       Expects [09afAF] only !
* Para: const uint8_t *buf -> pointer to source string
 *       uint16_t len -> length of NOT zero terminated source data at buf*
 *       uint32_t* value -> extracted value will be stored here
 * Rets: bool -> true if extracted a valid value in types range
 *--------------------------------------------------------------------------------------------------
 */
bool ICACHE_FLASH_ATTR
SCDEH_GetHexUInt32Val(const uint8_t *buf
			, uint16_t len
			, uint32_t* value)
  {

  uint32_t n = 0;

  while (len--)

	{

	if (isdigit(*buf)) n = (n * 16) + *buf++ - '0';
	else if (*buf >='a' && *buf <='f') n = (n * 16) + *buf++ - ('a'-10);
	else if (*buf >='A' && *buf <='F') n = (n * 16) + *buf++ - ('A'-10);

	}

  # if SCDEH_DBG >= 5
  printf("|X-uint32:%X>"
	,n);		
  # endif

  *value = n;

  return true;

  }

 

/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDEH_GetDecUInt8Val
 * Desc: Hepler routine that extracts an decimal unsigned int 8 value from buffer.
 *       Expects digits only !
* Para:  const uint8_t *buf -> pointer to source string
 *       uint16_t len -> length of NOT zero terminated source data at buf*
 *       uint8_t* value -> extracted value will be stored here
 * Rets: bool -> true if extracted a valid value
 *--------------------------------------------------------------------------------------------------
 */
bool ICACHE_FLASH_ATTR
SCDEH_GetDecUInt8Val(const uint8_t *buf
			,uint16_t len
			,uint8_t* value)
  {

  int n = 0;

  while (len--)
	n = (n * 10) + *buf++ - '0';

  # if SCDEH_DBG >= 5
  printf("|X-uint8:%u>"
	,n);		
  # endif

  *value = n;

  return true;

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDEH_GetDecUInt16Val
 * Desc: Hepler routine that extracts an decimal unsigned int 16 value from buffer.
 *       Expects digits only !
* Para: const uint8_t *buf -> pointer to source string
 *       uint16_t len -> length of NOT zero terminated source data at buf*
 *       uint16_t* value -> extracted value will be stored here
 * Rets: bool -> true if extracted a valid value
 *--------------------------------------------------------------------------------------------------
 */
bool ICACHE_FLASH_ATTR
SCDEH_GetDecUInt16Val(const uint8_t *buf
	,uint16_t len
	,uint16_t* value)
  {

  int n = 0;

  while (len--)
	  n = (n * 10) + *buf++ - '0';

  # if SCDEH_DBG >= 5
  printf("|X-uint16:%u>"
	,n);		
  # endif

  *value = n;

  return true;

}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDEH_GetDecUInt32Val
 * Desc: Hepler routine that extracts an decimal unsigned int 32 value from buffer.
 *       Expects digits only !
* Para: const uint8_t *buf -> pointer to source string
 *       uint16_t len -> length of NOT zero terminated source data at buf*
 *       uint32_t* value -> extracted value will be stored here
 * Rets: bool -> true if extracted a valid value
 *--------------------------------------------------------------------------------------------------
 */
bool ICACHE_FLASH_ATTR
SCDEH_GetDecUInt32Val(const uint8_t *buf
	,uint16_t len
	,uint32_t* value)
  {

  int n = 0;

  while (len--)
  n = (n * 10) + *buf++ - '0';

  # if SCDEH_DBG >= 5
  printf("|X-uint32:%u>"
	,n);		
  # endif

  *value = n;

  return true;

 }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDEH_GetDecUInt32Val
 * Desc: Hepler routine that extracts an decimal unsigned int 32 value from buffer.
 *       Expects digits only !
 * Para: const uint8_t *buf -> pointer to source string
 *       uint16_t len -> length of NOT zero terminated source data at buf*
 *       float* value -> extracted value will be stored here
 * Rets: bool -> true if extracted a valid value
 *--------------------------------------------------------------------------------------------------
 */
/*
bool ICACHE_FLASH_ATTR
SCDEH_GetFloatVal(const uint8_t *buf
	,uint16_t len
	,float* value)
  {

  int n = 0;

  while (len--)
	  n = (n * 10) + *buf++ - '0';

  # if SCDEH_DBG >= 5
  printf("|X-float:%u>"
	,n);		
  # endif

  *value = n;

  return true;

 }*/

/*
bool ICACHE_FLASH_ATTR
SCDEH_GetFloatVal(const uint8_t *buf
	,uint16_t len
	,float* value)
  {

  float rez = 0, fact = 1;

  if (*buf == '-' && (len) )

	{

	buf++;
	len--;
	fact = -1;

	};

  for (int point_seen = 0; len; buf++, len--)

	{

	if (*buf == '.')

		{

		point_seen = 1; 
		continue;

		};

	int d = *buf - '0';

	if (d >= 0 && d <= 9)

		{

		if (point_seen) fact /= 10.0f;
		rez = rez * 10.0f + (float)d;

		};

	else return false;

	};

  *value = rez * fact;

  return true;

  }

*/

bool ICACHE_FLASH_ATTR
SCDEH_GetFloatVal(const uint8_t *buf
	,uint16_t len
	,double* value)
  {
  double a = 0.0;
  int e = 0;
  int c = 0;

  if (!(len)) return false;				// 1.1  ch3

  while (len && isdigit(c = *buf++) )			// .1    ch2 

	{

	len--;
	a = a * 10.0 + (c - '0');

	}

  if (c == '.') 					// MERKER '.',  1    ch2

	{

	len--;
	while (len && isdigit(c = *buf++) )

		{
		len--;
		a = a * 10.0 + (c - '0');
		e = e - 1;

		}
	}

  if (len)	//e-1   2    1

	{

	if ( (c == 'e' || c == 'E') && len >= 2 )

		{

		int sign = 1;
		int i = 0;
		c = *buf++;
		len--;

		if (c == '+')

			{

			c = *buf++;

			}

		else if (c == '-')

			{

			c = *buf++;
			sign = -1;

			}

		else return false;


		while (isdigit(c) && len)

			{

			len--;
			i = i*10 + (c - '0');
			c = *buf++;

			}

		e += i*sign;
		}

	else return false;
	}

  while (e > 0)

	{

	a *= 10.0;
	e--;

	}

  while (e < 0)

	{

	a *= 0.1;
	e++;

	}

  *value = a;

  return true;

  }










/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDEH_GetDecInt8Val
 * Desc: Hepler routine that extracts an decimal signed int 16 value from buffer.
 *       Expects digits with optional trailing '+-'
 * Para: const uint8_t *buf -> pointer to source string
 *       uint16_t len -> length of NOT zero terminated source data at buf*
 *       int8_t* value -> extracted value will be stored here
 * Rets: bool -> true if extracted a valid value
 *--------------------------------------------------------------------------------------------------
 */
bool ICACHE_FLASH_ATTR
SCDEH_GetDecInt8Val(const uint8_t *buf
	, uint16_t len
	,int8_t* value)
  {

  int32_t n = 0;
  int32_t sign = 1;

  if (len) switch(*buf)

	{

	case '-':	sign=-1;        \
	case '+':	--len, ++buf;

	}

  while ( len-- && isdigit(*buf) )
	n = n*10 + *buf++ -'0';

  # if SCDEH_DBG >= 5
  printf("|X-int8:%d>"
	,n);		
  # endif

  *value = n * sign;

  return true;

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDEH_GetDecInt32Val
 * Desc: Hepler routine that extracts an decimal signed int 16 value from buffer.
 *       Expects digits with optional trailing '+-'
 * Para: const uint8_t *buf -> pointer to source string
 *       uint16_t len -> length of NOT zero terminated source data at buf*

 *       int16_t* value -> extracted value will be stored here
 * Rets: bool -> true if extracted a valid value
 *--------------------------------------------------------------------------------------------------
 */
bool ICACHE_FLASH_ATTR
SCDEH_GetDecInt16Val(const uint8_t *buf
	, uint16_t len
	,int16_t* value)
  {

  int32_t n = 0;
  int32_t sign = 1;

  if (len) switch(*buf)

	{

	case '-':	sign=-1;        \
	case '+':	--len, ++buf;

	}

  while ( len-- && isdigit(*buf) )
	n = n*10 + *buf++ -'0';

  # if SCDEH_DBG >= 5
  printf("|X-int16:%d>"
	,n);		
  # endif

  *value = n * sign;

  return true;

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDEH_GetDecInt32Val
 * Desc: Hepler routine that extracts an decimal signed int 32 value from buffer.
 *       Expects digits with optional trailing '+-'
 * Para: const uint8_t *buf -> pointer to source string
 *       uint16_t len -> length of NOT zero terminated source data at buf*
 *       int32_t* value -> extracted value will be stored here
 * Rets: bool -> true if extracted a valid value
 *--------------------------------------------------------------------------------------------------
 */
bool ICACHE_FLASH_ATTR
SCDEH_GetDecInt32Val(const uint8_t *buf
	, uint16_t len
	,int32_t* value)
  {

  int32_t n = 0;
  int32_t sign = 1;

  if (len) switch(*buf)

	{

	case '-':	sign=-1;        \
	case '+':	--len, ++buf;

	}

  while ( len-- && isdigit(*buf) )
	n = n*10 + *buf++ -'0';

  # if SCDEH_DBG >= 5
  printf("|X-int32:%d>"
	,n);		
  # endif

  *value = n * sign;

  return true;

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDE_GetDesc
 * Desc: Hepler to get assigned string to SelID
 * Para: SelectAData *SAD -> pointer to data table
 *       uint8 SelID -> the Selected ID
 * Rets: const char* -> the assigned text
 *--------------------------------------------------------------------------------------------------
 */
const char* ICACHE_FLASH_ATTR
SCDE_GetDesc(SelectAData *SAD, uint8_t SelID)
  {

  int i=0;

  while ( (SAD[i].Desc != NULL) && (SAD[i].ID != SelID) ) i++;

  return SAD[i].Desc;

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDEH_GetQueryKeyID
 * Desc: Hepler routine that extracts the Query-Key-ID from buffer.
 *       Expects a  that meet ?digits with optional trailing '+-'
 * Para: const uint8_t *buf -> pointer to source string
 *       uint16_t len ->  length of NOT zero terminated source data int32_t[len]
 *       int32_t* Value -> ptr to beginning of value string
 *       SelectAData* ValueTab -> table with values and IDs
 * Rets: int header field id (-1 if error / not found)
 *--------------------------------------------------------------------------------------------------
 */
bool ICACHE_FLASH_ATTR
SCDEH_GetQueryKeyID(const uint8_t *buf
	,uint16_t len
	,uint8_t *Value
	,SelectAData *ValueTab)
{

  int i = 0;

  // seek through values, till desc == 0
  while (ValueTab[i].Desc != 0) {

	if ( (strlen(ValueTab[i].Desc) == len)
		&& (!strncasecmp((const char*) ValueTab[i].Desc, (const char*) buf, len)) ) {

		*Value = ValueTab[i].ID;

		# if SCDEH_DBG >= 5
		printf("|X-QKID:%d>"
			,ValueTab[i].ID);		
		# endif

		return true;
	}




/* old code
	if (memcmp(buf, ValueTab[i].Desc, len) != 0) i++;

	else if (len == strlen(ValueTab[i].Desc) ) {

		*Value = ValueTab[i].ID;

		# if SCDEH_DBG >= 5
		printf("|X-QKID:%d>"
			,ValueTab[i].ID);		
		# endif

		return true;

	}
*/
	else i++;
  }

  # if SCDEH_DBG >= 5
  printf("|X-QKID:NF!>");		
  # endif

  return false;

}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDEH_AddCmdToWritePos
 * Desc: Hepler routine that adds executed command description to string 'KEY=VALUE" 
 *       Expects digits with optional trailing '+-'
 * Para: char* RespArgsWPos -> pointer to write position in string
 *       const SCDE_QueryKeys_t* XX_QueryKeys ->  table with associated QueryKeys
 *       uint32_t CmdNr -> eqivalent command-number from command to add 
 * Rets: char* RespArgsWPos -> pointer to new write position
 *--------------------------------------------------------------------------------------------------
 */
char* ICACHE_FLASH_ATTR
SCDEH_AddCmdToWritePos(char* RespArgsWPos,
			const SCDE_QueryKeys_t* XX_QueryKeys,
			uint32_t CmdNr)
  {

  strcpy(RespArgsWPos,"cmd=");

  RespArgsWPos += 4;

  strcpy(RespArgsWPos, XX_QueryKeys[CmdNr].QueryKey);

  RespArgsWPos += strlen(XX_QueryKeys[CmdNr].QueryKey);

  return RespArgsWPos;

  }



/*// query keys dürfen keine encoded zeichen enthalten!
 *--------------------------------------------------------------------------------------------------
 *FName: SCDEH_ParseStrToQueryResultKF
 * Desc: Hepler routine that searches and extracts an SPECIAL string value matching the given key. 
 *       Source data is an key=value pair string, seperated by an "&" from an query string
 *       WARNING: FOR FIRST and LAST CHAR ONLY ALPHA OR NUMMERICALAL IS ALLOWED !	  
 * Para: int num -> number of implemented KEYs for this query-type
 *       const QueryKeys *XX_QueryKeys -> list of implemented KEYs for this query-type
 *       const char *QueryStr -> ptr to zero-teriminated KEY=VALUE string from query, seperated by '&'
 * Rets: struct SCDE_XX_QueryResultKF *QueryResultKF -> allocad struct, filled with data
 *       DONT FORGET TO FREE MEMORY !
 *--------------------------------------------------------------------------------------------------
 */
SCDE_XX_QueryResultKF_t* ICACHE_FLASH_ATTR
SCDEH_ParseStrToQueryResultKF(int num,
			const SCDE_QueryKeys_t *XX_QueryKeys,
			const char *QueryStr)
  {
  // alloc memory for struct XX_QueryResultKF - length is variable !
  int memlen =  8 + (num * (2+2+4));

  #if SCDEH_DBG >= 5
  printf("|SCDEH-PSTQRKF-malloc:%d, input:\"%s\">"
	,memlen
	,QueryStr);
  #endif

  SCDE_XX_QueryResultKF_t *QueryResultKF =
	(struct SCDE_XX_QueryResultKF_s *) malloc(memlen);

  // clear Key Field Set
  QueryResultKF->KeyField_Set = 0;

  // implemented Key-Fields loop
  int i;
  for (i=0; i<num; i++)
	{

	// get ptr to current key
	const char * key = XX_QueryKeys[i].QueryKey;

	#if SCDEH_DBG >= 5
	printf("|chk for KEY:%d-\"%s\", "
		,i
		,key);
	#endif

	const char *p = QueryStr;	// start ptr
	const char *e;			// end ptr 
	int keylen = strlen(key);	// lenght of key

//	while ( (p != NULL) && (*p != '\n') && (*p != '\r') && (*p != 0) )
	while ( (p != NULL) && (*p != '\n') && (*p != '\r') && (*p != 0) )
  		{

		# if SCDEH_DBG >= 5
		printf("cmp:\"%s\"",
			p);
		# endif

		// matches value? and is "=" after value?
		if ( (strncmp(p, key, keylen) ==0 ) && (p[keylen] == '=') ) 
  			{
			// move p to start of the VALUE (from this KEY)
			p += keylen+1;

			// move e to end of the VALUE (from this KEY)
			e = (char*) strstr(p, "&");

			// or is it last Key-Value pair?
			if (e == NULL) e = p + strlen(p);

 			# if SCDEH_DBG >= 5
			printf(", next cmp:\"%s\"-len:%d"
				,p
				,(e-p));
			# endif

			// store positive result -> do not forget to url-encode !
			QueryResultKF->KeyField_Set |= (uint64_t)1<<i;
			QueryResultKF->KeyField_Data_t[i].off = (p-QueryStr); // or better store ptr?
			QueryResultKF->KeyField_Data_t[i].len = (e-p);
			QueryResultKF->KeyField_Data_t[i].AffectedReadings = 
				XX_QueryKeys[i].AffectedReadings;

			// KEY found
			# if SCDEH_DBG >= 5
			printf(", found!>");
			# endif

			break;
			}

		// jump after next "&"
		p = (char*) strstr(p, "&");
		if (p != NULL) p += 1;
		}

	// KEY not found
	# if SCDEH_DBG >= 5
	printf(">");
	# endif

	}

  // Query parsed complete
  # if SCDEH_DBG >= 5
  SCDEFn_at_WebIf_M->HexDumpOutFn ("|QueryResultKF-HEX",
	QueryResultKF,
	memlen);
  # endif

  return QueryResultKF;
  }






























  static int UniqueTIST;
//##################################################################################################
//### FName: GetTIST
//###  Desc: Get the current Time Stamp for operations (warning: may not be the real time!)
//###  Para: None
//###  Rets: TimeStamp
//##################################################################################################
int ICACHE_FLASH_ATTR
GetTIST(void)
{
  int NowTIST = 0; //sntp_get_current_timestamp();
  if (UniqueTIST < NowTIST) UniqueTIST = NowTIST;
  return UniqueTIST;
}



//##################################################################################################
//### FName: GetUniqueTIST
//###  Desc: Gets an Unique Time Stamp for operations (warning: may not be the real time! BUT ITS UNIQUE)
//###  Para: None
//###  Rets: TimeStamp
//##################################################################################################
int ICACHE_FLASH_ATTR
GetUniqueTIST(void)
{
  int NowTIST = 0;//spz sntp_get_current_timestamp();
  if (UniqueTIST < NowTIST) UniqueTIST = NowTIST;
  else UniqueTIST++;
  return UniqueTIST;
}















/* loschen
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_UrlDecode
 * Desc: Decodes a string at *val with length valLen, which contains a percent-encoded value 
 *       (from query - Url Decode) to a temporary buffer at *dstStr with max lenght dstLen.
 *       Returns the amount of bytes decoded to dstStr. The decoded string is \0 terminated.
 * Para: 
 *       char *val -> ptr to encoded value string
 *       int valLen -> length of encoded value string
 *       char *dstStr -> ptr to decoded string buffer
 *       int dstLen -> buffer length maximal available for decoded string
 * Rets: int -> decoded string length at *dstStr
 *--------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR
SCDED_UrlDecode(char *val, int valLen, char *dst, int dstLen) 

  {

  int s=0, d=0;

  int esced=0, escVal=0;

  while ( (s < valLen) && (d < dstLen) )

  	{

	if (esced==1)

		{

		escVal = SCDED_HexVal(val[s])<<4;
		esced = 2;

		} 

	else if (esced==2) 

		{

		escVal += SCDED_HexVal(val[s]);
		dst[d++] = escVal;
		esced = 0;

		} 

	else if (val[s] == '%')

		{

		esced = 1;

		}

	else if (val[s] == '+')

		{

		dst[d++] = ' ';

		}

	else

		{

		dst[d++] = val[s];

		}

	s++;

	}

  if (d < dstLen) dst[d] = 0;

  return d;
  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_UrlDecode2 wofür ???
 * Desc: RETURN AN EXTRACTED %ENCODED NUMBER FROM STRING
 *       .
 *       
 * Para: 
 *       char *val -> ptr to encoded value string
 *       int valLen -> length of encoded value string
 *       char *dstStr -> ptr to decoded string buffer
 *       int dstLen -> buffer length maximal available for decoded string
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR 
SCDED_UrlDecode2(char *dst, const char *src)

  {

  uint8_t a, b;

  while (*src)

	{

	if ((*src == '%') && ((a = src[1]) &&
		(b = src[2])) && (isxdigit(a) && isxdigit(b)))

		{

		if (a >= 'a')
			a -= 'a'-'A';

		if (a >= 'A')
			a -= ('A' - 10);
		else
			a -= '0';

		if (b >= 'a')
			b -= 'a'-'A';

		if (b >= 'A')
			b -= ('A' - 10);
		else
			b -= '0';

		*dst++ = 16*a+b;
		src += 3;

		}

	else

		{

		*dst++ = *src++;

		}

	}

	*dst++ = '\0';
  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_GetHdrFldID
 * Desc: Returns the ID of the header field at char* with specified length (NOT zero terminated!)
 * Para: char *url -> ptr to source string
 *       size_t length -> number of chars
 * Rets: int header field id (-1 if error / not found)
 *--------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR
SCDED_GetHdrFldID(const char *at,
			size_t length)
  {

  int i = 0;

  while (AvailHdrFlds[i].FldName != NULL)

	{

	if (memcmp(at, AvailHdrFlds[i].FldName, length) != 0) 

		{

		i++;

		}

	else 

		{

		return i;

		}

	}

  return -1;

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_GetHdrValFromID
 * Desc: Returns the header field value (char* to string) from ID 
 * Para: WebIf_HTTPDConnSlotData_t *conn -> prt to connection slot
 *       int HdrFldID -> header field id
 * Rets: char* -> header field value / NULL on error
 *--------------------------------------------------------------------------------------------------
 */
char* ICACHE_FLASH_ATTR
SCDED_GetHdrValFromID(WebIf_HTTPDConnSlotData_t *conn,
			int HdrFldID)
  {

  HeaderFieldInfo_t *n1;

  STAILQ_FOREACH(n1, &conn->head, entries)

	{

	if (n1->HdrFldID == HdrFldID) return n1->HdrFldVal;

	}

  return NULL;

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_HexVal
 * Desc: Returns the value of a HEX char
 * Para: char c -> input HEX char
 * Rets: int -> value of HEX char
 *--------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR 
SCDED_HexVal(char c)

  {

  if (c>='0' && c<='9') return c-'0';

  if (c>='A' && c<='F') return c-'A'+10;

  if (c>='a' && c<='f') return c-'a'+10;

  return 0;

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_GetMimeTypeID
 * Desc: Returns the ID of the Mime Type for a given url str at char *
 * Para: char *url -> given url str at char *
 * Rets: int -> ID of Mime from table
 *--------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR
SCDED_GetMimeTypeID(char *url)

  {

  int i=1;

  // Go find the extension
  char *ext = url + (strlen(url)-1);

  while ( (ext != url) && (*ext != '.') ) ext--;

  // if we have no extension -> return value '0'
  if (ext == url) return 0;

  ext++; //if (*ext=='.') ext++;

  while ( (AvailMimes[i].ext != NULL) && (strcmp(ext, AvailMimes[i].ext) !=0) ) i++;

  return i;

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_StartRespHeader
 * Desc: Adds start of an Response Header to TX Buffer
 * Para: WebIf_HTTPDConnSlotData_t *conn -> prt to connection slot
 *	 int code -> Response code
 * Rets: int -> bytes free in sendbuff
 *--------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR 
SCDED_StartRespHeader(WebIf_HTTPDConnSlotData_t *conn
			,int code) 
  {

  char buff[128];

  int l;

  l = sprintf(buff, "HTTP/1.0 %d OK\r\nSCD-Engine/0.1\r\n", code);

  return (SCDED_Send(conn, buff, l));

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_StartRespHeader2
 * Desc: Adds start of an Response Header to TX Buffer. It includes external code and description.
 *       The code stored in 'conn->parser_http_errno' is used.
 * Para: WebIf_HTTPDConnSlotData_t *conn -> ptr to current connection slot
 * Rets: int -> amount of bytes free in sendbuff
 *--------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR 
SCDED_StartRespHeader2(WebIf_HTTPDConnSlotData_t *conn)

  {

  char buff[128];

  int l;

  l = sprintf(buff, "HTTP/1.0 %d %s\r\nSCD-Engine/0.1\r\n"
	,HTTPD_ErrnoCode(conn->parser_http_errno)
	,HTTPD_ErrnoDescription(conn->parser_http_errno));

  return (SCDED_Send(conn, buff, l));

  }



/*alt:HTTPDHeader
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_AddHdrFld
 * Desc: Adds a Header-Field with value to TX Buffer
 * Para: WebIf_HTTPDConnSlotData_t *conn -> ptr to connection slot
 *       const char *Field -> ptr to field str
 *       const char *Value -> ptr to value str
 *       int ValueLength -> length of value str (0 = unknown /autodetect)
 * Rets: int -> bytes free in sendbuff
 *--------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR 
SCDED_AddHdrFld(WebIf_HTTPDConnSlotData_t *conn
		,const char *Field
		,const char *Value
		,int ValueLength) 
  {

  SCDED_Send(conn, Field, -1);

  SCDED_Send(conn, ": ", 2);

  SCDED_Send(conn,  Value, ValueLength);

  return (SCDED_Send(conn, "\r\n", 2));

  }



/*alt:HTTPDHeaders
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_EndHeader
 * Desc: Adds "end header mark" to TX buffer
 * Para: WebIf_HTTPDConnSlotData_t *conn -> ptr to connection slot
 * Rets: int -> bytes free in sendbuff
 *--------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR 
SCDED_EndHeader(WebIf_HTTPDConnSlotData_t *conn)

  {

  return (SCDED_Send(conn, "\r\n", 2));

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_Redirect
 * Desc: Sends HTTP content that redirects browser to new location
 * Para: WebIf_HTTPDConnSlotData_t *conn -> ptr to connection slot
 *       char *newUrl -> new url string redirect destination
 * Rets: int -> bytes free in sendbuff
 *--------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR 
SCDED_Redirect(WebIf_HTTPDConnSlotData_t *conn
		,char *newUrl) 
  {

  char buff[512];

  int l;

  l = sprintf(buff, "HTTP/1.1 302 Found\r\nLocation: %s\r\n\r\nMoved to %s\r\n",
	newUrl,
	newUrl);

  return (SCDED_Send(conn, buff, l));

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_Send
 * Desc: Helper function to add data to the send buffer.
 *       If len is -1 the data is seen as a C-string. Len will be determined.
 * Info: CAN SEND FROM FLASH. DO NOT USE LENGTH AUTO-DETECTION IF SENDING DATA DIRECT FROM FLASH !!!
 * Para: WebIf_HTTPDConnSlotData_t *conn -> ptr to connection slot
 *       const char *data -> ptr to the data to send 
 *       int len -> length of data. If -1 the data is seen as a C-string and len will be determined.
 * Rets: int bytes free in sendbuff
 *--------------------------------------------------------------------------------------------------
 */
/*
int ICACHE_FLASH_ATTR 
SCDED_Send(WebIf_HTTPDConnSlotData_t *conn
		,const char *data
		,int len)
  {
  // if len is -1 the data is seen as a C-string. Length will be determined ..
  if (len < 0) len = strlen(data);

  // will data fit in sendbuff ?
  if (conn->sendBuffLen + len <= MAX_SENDBUFF_LEN)

	// if data fits in sendbuff ...
	{

	// copy to sendbuf
	SCDE_memcpy_plus(conn->sendBuff + conn->sendBuffLen, data, len);
	conn->sendBuffLen += len;

	// return free bytes in sendbuff
	return (MAX_SENDBUFF_LEN - conn->sendBuffLen);

	}

  else

	 // if data does NOT fit in sendbuff ...
	{

	// Step 1: copy data to sendbuff till full
	int SendBufFree = MAX_SENDBUFF_LEN - conn->sendBuffLen;

	if (SendBufFree)

		{

		// copy to sendbuf
		SCDE_memcpy_plus(conn->sendBuff + conn->sendBuffLen, data, SendBufFree);
		conn->sendBuffLen += SendBufFree;

		}

	// Step 2: create/add rest of data to TrailingBuff
	int NewTrailingBuffLen;

	if (conn->TrailingBuff == NULL) NewTrailingBuffLen = len - SendBufFree;

	else NewTrailingBuffLen =
		conn->TrailingBuffLen + len - SendBufFree;

	// alloc new TrailingBuff
	char *NewTrailingBuff = (char*)malloc(NewTrailingBuffLen+1);

	// Copy old TrailingBuff to new TrailingBuff, and dealloc old
	if (conn->TrailingBuff != NULL)

		{

		// copy to sendbuf
		memcpy (NewTrailingBuff, conn->TrailingBuff, conn->TrailingBuffLen);
		free(conn->TrailingBuff);

		}

  else  conn->TrailingBuffLen =0; ///test


	// Add new data to TrailingBuff and save
	SCDE_memcpy_plus(NewTrailingBuff + conn->TrailingBuffLen, data + SendBufFree, len - SendBufFree);

	# if SCDED_DBG >= 3
	printf("HTTPD %d Bytes to TrailingBuff\n",
		NewTrailingBuffLen);
	#endif

	// Store 
	conn->TrailingBuff = NewTrailingBuff;
	conn->TrailingBuffLen = NewTrailingBuffLen;

	}

  // 0 bytes free in sendbuff
  return 0;

  }
*/


/* --------------------------------------------------------------------------------------------------
 *  FName: SCDED_SendToSendBuff
 *  Desc: Sends / adds data to the Send-Buffer, without doing the real transmission.
 *        May create (alloc) an send buffer. May also create an manage an Trailing Buffer if SB is full.
 *        If len is -1 the data is seen as a C-string. Len will be determined.
 *  Info: CAN SEND FROM FLASH. DO NOT USE LENGTH AUTO-DETECTION IF SENDING DATA DIRECT FROM FLASH !!!
 *  Para: WebIf_HTTPDConnSlotData_t *conn -> ptr to the connection slot
 *        const char *data -> ptr to the data to send 
 *        int len -> length of data. If -1 the data is seen as a C-string and len will be determined.
 *  Rets: int bytes free in sendbuff
 * --------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR 
SCDED_Send(WebIf_HTTPDConnSlotData_t *conn
//SCDED_SendToSendBuff(HTTPDConnSlotData_t *conn
		,const char *data
		,int len)
  {

  // if len is -1, the data is seen as a C-string. Determine length ..
  if (len < 0) len = strlen(data);

  // in case we have nothing to send -> return free bytes in sendbuff
  if (!len) return (MAX_SENDBUFF_LEN - conn->sendBuffLen);

  // WE WILL DEFINITIVLY SEND DATA HERE - MAKE SEND REQUEST
  conn->conn->common.Common_CtrlRegA |= F_WANTS_WRITE;	//!!!!!!!!!!!!!!!!!!!!!!!problem!

  // alloc Send-Buffer, if not already done
  if (conn->sendBuff == NULL)
	conn->sendBuff = (char*) malloc(MAX_SENDBUFF_LEN);

  // will data fit in Send-Buffer? Then simply copy ...
  if (conn->sendBuffLen + len <= MAX_SENDBUFF_LEN)

	// data fits, copy to Send-Buffer ...
	{

	// copy to Send-Buffer
	SCDE_memcpy_plus(conn->sendBuff + conn->sendBuffLen, data, len);
	conn->sendBuffLen += len;

	// return free bytes in Send-Buffer
	return (MAX_SENDBUFF_LEN - conn->sendBuffLen);

	}

  else

	 // else data does NOT fit in Send-Buffer ...
	{

	// Step 1: copy data to Send-Buffer till full
	int SendBufFree = MAX_SENDBUFF_LEN - conn->sendBuffLen;

	if (SendBufFree)

		{

		// copy to Send-Buffer
		SCDE_memcpy_plus(conn->sendBuff + conn->sendBuffLen, data, SendBufFree);

		conn->sendBuffLen += SendBufFree;

		}

	// Step 2: create / add the rest of data to Trailing-Buffer
	int NewTrailingBuffLen;

	// if there is no Trailing-Buffer / empty ... -> get new length
	if (conn->TrailingBuff == NULL) NewTrailingBuffLen = len - SendBufFree;

	// else if there was a Trailing-Buffer ... -> get new length
	else NewTrailingBuffLen =
		conn->TrailingBuffLen + len - SendBufFree;

	// alloc new Trailing-Buffer
	char *NewTrailingBuff = (char*) malloc(NewTrailingBuffLen+1);

	// copy old Trailing-Buffer to new Trailing-Buffer, and dealloc old
	if (conn->TrailingBuff != NULL)

		{

		// copy old Trailing-Buffer to new Trailing-Buffer
		memcpy (NewTrailingBuff, conn->TrailingBuff, conn->TrailingBuffLen);

		// dealloc old Trailing-Buffer
		free(conn->TrailingBuff);

		}

	// add the rest of the new data to the Trailing Buffer and save
	SCDE_memcpy_plus(NewTrailingBuff + conn->TrailingBuffLen, data + SendBufFree, len - SendBufFree);

	# if SCDED_DBG >= 3
	printf("|note: adding %d bytes to TrailingBuff>",
		NewTrailingBuffLen);
	#endif

	// Store 
	conn->TrailingBuff = NewTrailingBuff;
	conn->TrailingBuffLen = NewTrailingBuffLen;

	}

  // here 0 bytes free in sendbuff
  return 0;

  }





/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_XmitSendBuff
 * Desc: Helper function to send any data in conn->sendBuff
 * Para: WebIf_HTTPDConnSlotData_t *conn -> ptr to connection slot
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
/*
void ICACHE_FLASH_ATTR 
SCDED_XmitSendBuff(WebIf_HTTPDConnSlotData_t *conn)

  {

  if (conn->sendBuffLen != 0)

	{

	# if SCDED_DBG >= 3
	printf("\n|XmitSendBuff, slot %d, to remote:%d.%d.%d.%d:%d from local port:%d, len=%d. mem:%d>",
		conn->SlotNo,
		conn->conn->proto.tcp->remote_ip[0],
		conn->conn->proto.tcp->remote_ip[1],
		conn->conn->proto.tcp->remote_ip[2],
		conn->conn->proto.tcp->remote_ip[3],
		conn->conn->proto.tcp->remote_port,
		conn->conn->proto.tcp->local_port,
		conn->sendBuffLen,
		system_get_free_heap_size());
	# endif

	# if SCDED_DBG >= 5
 	 SCDEFn_at_WebIf_M->HexDumpOutFn ("\nTX-HEX",
		conn->sendBuff,
		conn->sendBuffLen);
	# endif

	int8_t Result = WebIf_sent(conn->conn,
		(uint8_t*) conn->sendBuff,
		conn->sendBuffLen);

	// show error on debug term...
 	if (Result)

		{

		# if SCDED_DBG >= 1
		printf("\n|TX-Err:%d!>"
			,Result);
		# endif

		}

	// We sent data. We are not allowed to send again till SentCb is fired.
	// Indicate this by F_TXED_CALLBACK_PENDING in ConCtrl
	conn->ConnCtrlFlags |= F_TXED_CALLBACK_PENDING;

	}

  else

	{

	# if SCDED_DBG >= 3
	printf("|NO TX!>");
	# endif

	}
  }
*/



/* --------------------------------------------------------------------------------------------------
 *  FName: SCDED_TransmitSendBuff
 *  Desc: Function to finally transmit the data stored in the Send-Buffer (conn->sendBuff, if any)
 *
 *  Para: SCDED_DConnSlotData_t *conn -> ptr to connection slot
 *  Rets: -/-
 * --------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR 
SCDED_TransmitSendBuff(WebIf_HTTPDConnSlotData_t *conn) 
{

  // do we have an allocated Send-Buffer -> there is something to send in the Send-Buffer !
  if (conn->sendBuff) {

	# if SCDED_DBG >= 3
	os_printf("|TX SendBuff, slot %d, to remote:%d.%d.%d.%d:%d from local port:%d, len=%d. mem:%d>",
		conn->SlotNo,
		conn->conn->proto.tcp->remote_ip[0],
		conn->conn->proto.tcp->remote_ip[1],
		conn->conn->proto.tcp->remote_ip[2],
		conn->conn->proto.tcp->remote_ip[3],
		conn->conn->proto.tcp->remote_port,
		conn->conn->proto.tcp->local_port,
		conn->sendBuffLen,
		system_get_free_heap_size());
	# endif

	# if SCDED_DBG >= 5
 	 SCDEFn_at_WebIf_M->HexDumpOutFn ("\nTX-SendBuff",
		conn->sendBuff,
		conn->sendBuffLen);
	# endif

	int8_t Result = WebIf_sent(conn->conn,
		(uint8_t*)conn->sendBuff,
		(uint16_t)conn->sendBuffLen);

	// show error on debug term...
 	if (Result) {

		# if SCDED_DBG >= 1
		os_printf("\n|TX-Err:%d!>"
			,Result);
		# endif
	}

	// free Send-Buffer
	free(conn->sendBuff);

	// indicate -> no Send Buffer
	conn->sendBuff = NULL;

	// init length for next usage
	conn->sendBuffLen = 0;

	// incompatibility? alter code ?
	// We sent data. We are not allowed to send again till SentCb is fired.
	// Indicate this by F_TXED_CALLBACK_PENDING in ConCtrl
	conn->ConnCtrlFlags |= F_TXED_CALLBACK_PENDING;
  }

  else {

	# if SCDED_DBG >= 3
	os_printf("|no allocated SendBuff, no TX!>");
	# endif
  }
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_LoadSerializer
 * Desc: To prevent heavy cpu and ram usage caused by to much new connections this is a try
 *       to serialize heavy load tasks. It is controlled by "uint32 LoadSerializer"
 *       If NO bit for connSlot 1-32 is set, it sets SlotBit and process one heavy load tasks.
 *       If ANY bit for connSlot 1-32 is set, it delays this heavy load tasks.
 * Para: WebIf_HTTPDConnSlotData_t *conn -> ptr to connection slot
 * Rets: bool -> FALSE= Process conn task now / TRUE= Process conn task later (load serialization active)
 *--------------------------------------------------------------------------------------------------
 */
bool ICACHE_FLASH_ATTR 
SCDED_LoadSerializer(WebIf_HTTPDConnSlotData_t *conn)
{

  // if no load, mark this connection as "process"
  if (!(conn->conn->HTTPD_InstCfg->LoadSerializer &= ~(1<<(conn->SlotNo))) ) {

	conn->conn->HTTPD_InstCfg->LoadSerializer |= 1 << (conn->SlotNo);

	#if SCDED_DBG >= 3
	printf("|LS:%u, proc Slot %d>",
		conn->conn->HTTPD_InstCfg->LoadSerializer,
		conn->SlotNo);
	#endif

	return false;
  }

  else  {

	#if SCDED_DBG >= 3
	printf("LS:%u, Slot %d delayed\n",
		conn->conn->HTTPD_InstCfg->LoadSerializer,
		conn->SlotNo);
	#endif

	return true;
  }
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_ProcHdrFldAuth
 * Desc: Function searches for header field "Authorization" and parses it.
 *       Func will extract UI and call HTTPDCheckAuth with User Information and check Authorization
 *       Should be called after header-parsing is finnished.
 * Para: WebIf_HTTPDConnSlotData_t *conn -> ptr to connection slot
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR
SCDED_ProcHdrFldAuth(WebIf_HTTPDConnSlotData_t *conn)

  {

  HeaderFieldInfo_t *n1;

  STAILQ_FOREACH(n1, &conn->head, entries)

	{

	if (n1->HdrFldID == 0)

		{

		char *at = n1->HdrFldVal;

		size_t length = strlen(n1->HdrFldVal);

		// Field beginning with "Basic " ?
		if (strncmp(at, "Basic ", 6) == 0)

			{

			char userpass[AUTH_MAX_USER_LEN+AUTH_MAX_PASS_LEN+2];
			int r;

			// decode
			r = mybase64_decode(length-6,
				at+6, sizeof(userpass),
				(unsigned char *)userpass);

 			// clean out string on decode error
			if (r<0) r=0;
 
			// zero-terminate user:pass string
			userpass[r]=0;

			// Now check the Auth
			SCDED_CheckHdrFldAuth(conn,
				(char*) &userpass,
				strlen(userpass));

 			// always OK
			return;

			}
		}
	}

  // no valid authorization found
  # if SCDED_DBG >= 4
  printf("|NoHdrFldAuthFnd>");
  # endif

  return;	
  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_CheckHdrFldAuth
 * Desc: Checks the user information string for "Basic Authorization", Sets F_CONN_IS_AUTHENTICATED
 *       Should be called during header field processing (->Auth field) or during URI parsing (UserInfo)
 * Para: WebIf_HTTPDConnSlotData_t *conn -> ptr to connection slot
 *       char* UserInfo -> pointer to user info str XXX:YYYY, NOT zero terminated! 
 *       int len -> length of user info
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR
SCDED_CheckHdrFldAuth(WebIf_HTTPDConnSlotData_t *conn
		,char* UserInfo
		,int len)
 {

  # if SCDED_DBG >= 4
  printf("|ChkUI4Auth:\"%s\" len:%d,",
	UserInfo,
	len);
  # endif
/* spz
  // Complete Check of UI ...
  if (len == strlen(SysCfgRamFlashMirrored->InfrastructurePwd) + strlen(SysCfgRamFlashMirrored->DevicePwd) + 1 &&

	// first part check &&
	strncmp(UserInfo, SysCfgRamFlashMirrored->InfrastructurePwd,
		 strlen(SysCfgRamFlashMirrored->InfrastructurePwd)) == 0 &&

	// ":" check &&
	UserInfo[strlen(SysCfgRamFlashMirrored->InfrastructurePwd)] == ':' &&

	// second part check
	strncmp(UserInfo+strlen(SysCfgRamFlashMirrored->InfrastructurePwd) + 1,
		SysCfgRamFlashMirrored->DevicePwd,
		strlen(SysCfgRamFlashMirrored->DevicePwd)) == 0)

	{

	// -> this connection is authenticated! -> Set F_CONN_IS_AUTHENTICATED flag for this connection
	conn->ConnCtrlFlags |= F_CONN_IS_AUTHENTICATED;

	# if SCDED_DBG >= 4
	printf("PASS>");
	# endif

	}

  else

	{

	# if SCDED_DBG >= 4
	printf("NOK!>");
	# endif

	}
*/
  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_AuthCheck
 * Desc: Checks authorization for ressource-type passed by "SecurityResCheck" -
 *       If auth. is enabled for ressource-type in register "HTTPDSecurityCfg" AND
 *       header parsing has found valid authorization information -> FALSE
 *       else -> TRUE ; Decision for further resource processing or "return HTTPD_CGI_NOAUTH_CONN"
 *       will continue
 * Para: WebIf_HTTPDConnSlotData_t *conn -> ptr to connection slot data
 *       int SecurityResCheck -> Bit of the ressource to do enabled check
 *                               (corresponting register is "HTTPDSecurityCfg")
 * Rets: bool -> valid=FALSE, else TRUE
 *--------------------------------------------------------------------------------------------------
 */
bool ICACHE_FLASH_ATTR 
SCDED_AuthCheck(WebIf_HTTPDConnSlotData_t *connData
		,int ResAuthCheckEna)
  {

  // check if the security auth. for resource type is enabled (in HTTPDSecurityCfg)
//spz  if ( (SysCfgRamFlashMirrored->HTTPDSecurityCfg & ResAuthCheckEna) &&
  if ( (1) &&
	// and auth. verification passed indicated by flag F_CONN_IS_AUTHENTICATED set during header processing?
		(!(connData->ConnCtrlFlags & F_CONN_IS_AUTHENTICATED)) )

	{

	// OK, send false to allow the resource
	return true;

	}

  else

	{

	// OK, send true to block the resource
	return false;

	}
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_SetConCtrl
 * Desc: SCDED Directory Content Enable Control - allows to enable and disable directory content by
 *       changing register "SCDED_DirConEnaCtrl". Must be specified in directory-data.
 *       Used to change Device behaviour
 * Para: uint32 CtrlBitfld -> new Control Bitfield
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
/* no longer used
void ICACHE_FLASH_ATTR 
SCDED_SetConCtrl(uint32_t CtrlBitfld)
  {

  SCDED_DirConEnaCtrl = CtrlBitfld;

  }*/




/* ------------------------------------------------------------------------------------------------
 *  @brief WebIF Respond to open connection
 *         Main element of SCDE. Responses to slots open connection, calls the processing callback
 *         procedure and holds or closes the connection, if finished
 *
 *  @param WebIf_HTTPDConnSlotData_t *conn : provide connection slot data
 *  @return -/-
 * ------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR 
SCDED_RespToOpenConn(WebIf_HTTPDConnSlotData_t *conn)
  {

  // Reset Inactivity Timer because we had an callback ...
  conn->InactivityTimer = 0;

  // Clear / Init Information Flags F_* (enum RespFlags from httpD.h) for response processing.
  uint32_t ConnRespFlags = 0;

// -------------------------------------------------------------------------------------------------

  // first: If Trailing-Buffer is filled, copy it to sendbuff, its prio to send it ...
  if (conn->TrailingBuff != NULL) {

	int TransmitLen;
	char *NewTrailingBuff = NULL;
	int NewTrailingBuffLen;

	// Step 1: Determine transmitlen.If more data then MAX_SENDBUFF_LEN -> save for next TX-Cb
	if (conn->TrailingBuffLen <= MAX_SENDBUFF_LEN) {

		TransmitLen = conn->TrailingBuffLen;

		}

	// more data to transmit then send buff can take ...
	else {

		TransmitLen = MAX_SENDBUFF_LEN;
		NewTrailingBuffLen = conn->TrailingBuffLen - MAX_SENDBUFF_LEN;

		// alloc new TrailingBuff
		char *NewTrailingBuff = (char*)malloc(NewTrailingBuffLen+1);

		// add the rest of the data to the TrailingBuff
		memcpy (NewTrailingBuff, conn->TrailingBuff + MAX_SENDBUFF_LEN, NewTrailingBuffLen);
		}

	# if SCDED_DBG >= 3
	printf("|TX %d bytes from TrailingBuff>",
		TransmitLen);
	#endif

	// Step 2: send data from the TrailingBuff, but max MAX_SENDBUFF_LEN
	SCDED_Send(conn, conn->TrailingBuff, TransmitLen);
//SCENARIO: MAX_SENDBUFF_LEN reached here?

	// free old TrailingBuff, save new NewTrailingBuff
	free(conn->TrailingBuff);

	if (NewTrailingBuff) {

		conn->TrailingBuff = NewTrailingBuff;
		conn->TrailingBuffLen = NewTrailingBuffLen;

		}

	else conn->TrailingBuff = NULL;

	}

// -------------------------------------------------------------------------------------------------

  // Scenario 1: The event 'PC_FINNISHED_CLEANUP' 
  // The cgi of the requested ressource is marked as finished
  // -> disconnection may happen automatic without flags ...
  if (conn->cgi == CGI_FINNISHED) {

	# if SCDED_DBG >= 3
	printf("|Conn %p, slot %d, PC_FINNISHED_CLEANUP>",
		conn->conn,
		conn->SlotNo);
	# endif

	}

// -------------------------------------------------------------------------------------------------

  // Scenario 2: The event 'PC_NOT_ASSIGNED' 
  // The cgi of the requested ressource in currently not assigned
  // -> expected to RX more data? Keep alive, without IdleCb !
  else if (conn->cgi == CGI_NOT_ASSIGNED) {

	# if SCDED_DBG >= 3
	printf("|Conn %p, slot %d, CGI_NOT_ASSIGNED>",
		conn->conn,
		conn->SlotNo);
	# endif

	// set F_WAIT_CONN flag, expecting more RX
	ConnRespFlags |= F_WAIT_CONN;

	}

// -------------------------------------------------------------------------------------------------

  // Scenario 3: The event 'CGI_UPGRADE_WEBSOCKET'
  // The cgi of the requested ressource wants to use WS -> install websockets protocol!
  else if (conn->cgi == CGI_UPGRADE_WEBSOCKET) {

	# if SCDED_DBG >= 3
	printf("SCDED Conn %p, slot %d, Upgr.Req for WS res:%s\n",
		conn->conn,
		conn->SlotNo,
		conn->url);
	# endif

	// get ptr to zero terminated key from header field "Sec-WebSocket-Accept" (ID02)
	char *SecWebSocketAcceptVal = SCDED_GetHdrValFromID(conn,02);

	if (SecWebSocketAcceptVal)
	// initiate Web Socket Connection

		{
		// prepare answer value for field "Sec-WebSocket-Accept"
		char buff[256];

		// copy this key to beginning of answer-buffer
		strcpy(buff, SecWebSocketAcceptVal);
		// add Globally Unique Identifier to answer-buffer
		strcat(buff, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
		// generate sha1 from string ...
		sha1nfo s;
		sha1_init(&s);
		sha1_write(&s, buff, strlen(buff));
		// base-encode the 20 byte sha1 result
		xbase64_encode(20, sha1_result(&s), sizeof(buff), buff);

		//Reply with the right headers.
		SCDED_StartRespHeader(conn, 101);
		SCDED_AddHdrFld(conn, "Upgrade", "WebSocket", -1);
		SCDED_AddHdrFld(conn, "Connection", "Upgrade", -1);
		SCDED_AddHdrFld(conn, "Sec-WebSocket-Accept", buff, -1);
		//HTTPDHeader(conn, "Sec-WebSocket-Protocol", "chat");
		//HTTPDHeader(conn, "Access-Control-Allow-Origin", "http://www.websocket.org");
		SCDED_EndHeader(conn);
/*
		// Alloc structs for WebSocket connection and stores it in conn->PCData
		conn->PCData = malloc(sizeof(struct Websock_s));
		memset(conn->PCData, 0, sizeof(struct Websock_s));
		Websock_t *ws = (Websock_t *)conn->PCData;
		ws->priv = malloc(sizeof(struct WebsockPriv_s));
		memset(ws->priv, 0, sizeof(struct WebsockPriv_s));
		ws->conn = conn;

		// Set data receive handler for WebSocket connection
		conn->recvHdl = cgiWebSocketRecv;
		// Inform CGI function we have a connection
		WsConnectedCb connCb = conn->PCArg;
		connCb(ws);
		// Insert ws into linked list
		if (llStart==NULL)
			{
			llStart=ws;
			}
		else
			{
			Websock_t *lw=llStart;
			while (lw->priv->next) lw=lw->priv->next;
			lw->priv->next=ws;
			}
*/
		}

	// Web Socket Connection failed
	else	{

		// Reply with the right headers.
		SCDED_StartRespHeader(conn, 500);
		SCDED_EndHeader(conn);

		// mark as finnished (next Response invoked by SentCB will disconnect)
		conn->cgi = CGI_FINNISHED;

		}
	}

// -------------------------------------------------------------------------------------------------

  // Scenario 4 - Default CGI execution this time ...
  else	{

	int r;

	do	{

		# if SCDED_DBG >= 3
		printf("|Conn %p, slot %d, Exec cgi at %p with PC-Arg: %p>",
			conn->conn,
			conn->SlotNo,
			conn->cgi,
			conn->PCArg);
		# endif

		// execute cgi fn.
		r = conn->cgi(conn);

		}

	// Possible cgi result 1: HTTPD_CGI_REEXECUTE, for cgi switching
	while (r == HTTPD_CGI_REEXECUTE);

// -------------------------------------------------------------------------------------------------

	// Possible cgi result 2: HTTPD_CGI_DISCONNECT_CONN, finnished, send rest of data, disconnect
	if (r == HTTPD_CGI_DISCONNECT_CONN) {

//call cleanup cgi?

		// mark as finnished (next response invoked by SentCB will disconnect, if data to send)
		conn->cgi = CGI_FINNISHED;

		}

// -------------------------------------------------------------------------------------------------

	// Possible cgi result 2: HTTPD_CGI_F_DISCONNECT_CONN - finnished, send rest of data, force a real disconnect (ETX-ONLY)
	else if (r == HTTPD_CGI_F_DISCONNECT_CONN) {

//call cleanup cgi?

		// mark as finnished (next Response invoked by SentCB will disconnect, if data to send)
		conn->cgi = CGI_FINNISHED;

		// set 'ETX_DISCONNECT_CONN' flag to force real disconnection for ETX-Slot
		ConnRespFlags |= F_F_DISCONNECT_CONN;

		}

// -------------------------------------------------------------------------------------------------

	// Possible cgi result 3: HTTPD_CGI_PROCESS_CONN, not finnished, enable one callback (worst case an idle Cb)
	else if (r == HTTPD_CGI_PROCESS_CONN) {

		// set 'F_WAIT_CONN_GET_IDLE_CB' flag + 'keep connection alive' flag
		ConnRespFlags |= F_WAIT_CONN_GET_IDLE_CB + F_KEEP_CONN_ALIVE;

		}

// -------------------------------------------------------------------------------------------------

	// Possible cgi result 4: HTTPD_CGI_WAITRX_CONN, not finnished, keep conn + wait for further RX
	else if (r == HTTPD_CGI_WAITRX_CONN) {

		// set 'keep connection alive' flag
		ConnRespFlags |= F_KEEP_CONN_ALIVE;

		}
	}

// -------------------------------------------------------------------------------------------------

  // Now possible actions: 
  // Prio 1: If data is in send buff
  // -> Transmit, SentCb will be fired soon for further activities ...
  if (conn->sendBuffLen) {

	SCDED_TransmitSendBuff(conn);

	// Keep connection only for 10 Sec
//	espconn_regist_time(conn->conn,10,1);	// MAX 10 Sec for answer (HTTPD_TIMEOUT ?)

	}

  // Prio 2: NO data in send buff, conn wants further processing, enable fire of idle Cb in ConnCtrl
  // -> one IdleCb will be fired soon ...
  else if (ConnRespFlags & F_WAIT_CONN_GET_IDLE_CB) {

	// Force one callback (may be by generating one idle callback)
	conn->ConnCtrlFlags |= F_GENERATE_IDLE_CALLBACK;

	// WE WILL DEFINITIVLY SEND DATA HERE - MAKE SEND REQUEST
	conn->conn->common.Common_CtrlRegA |= F_WANTS_IDLE_TASK;


	// Keep connection if idle for 2h
//	espconn_regist_time(conn->conn,257,1);	// MAX 256 Sec Longpoll

	}

  // Prio 3 - NO data in send buff, conn wants NO further processing. Check if KEEP_CONN_ALIVE is requested?
  else if (ConnRespFlags & F_KEEP_CONN_ALIVE) {

	// if yes do nothing ...

	}

  // Prio 4 - NO data in send buff, conn wants NO further processing. No keep alive. Disconnect connection !
  else {
	
	// disconnect -> will fire DisconCb to free resources
	WebIf_disconnect(conn->conn);

	}
  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_RespToOpenConn
 * Desc: Idle Callback Generator. Checks if conn slots requires idle Callback, and provides it.
 *       THIS GENERATOR CODE SHOULD BE CALLED FROM 10HZ TIMER
 *       Idle Cb request bit (BIT SET in ConnCtrl) must be renewed in every cycle to get an idleCb !!! 
 * Para: -/-
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR 
SCDED_IdleCbGen(void)
  {

/*
  # if SCDED_DBG >= 4
  printf("\nSCDE IdleCbs now! mem:%d>"
	,system_get_free_heap_size());
  # endif
*/

//--------------------------------------------------------------------------------------------------
/*spz
  // check Inactivity-Timer in all connection slots. Disconnect or increase
  int i;

  for (i=0; i<MAX_SCDED_CONN; i++)

	{

	// check only if Slot is allocated ...
	if (ConnSlots[i] != NULL)

		{

		// increase Inactivity Timer
		ConnSlots[i]->InactivityTimer++;

		// has InactivityTimer reached timeout-limit ? Then disconnect
		if (ConnSlots[i]->InactivityTimer == 300)

			{

			# if SCDED_DBG >= 1
			printf("\nSCDED Slot:%d, Timeout! Disconn!\n",
				i);
			# endif

			//espconn_disconnect(ConnSlots[i]->conn);

			}

		// has InactivityTimer reached death-limit ? Then retire death connection
		else if (ConnSlots[i]->InactivityTimer > 900)

			{

			// this indicates that connection is gone
			ConnSlots[i]->conn = NULL;

			// indicate cgi to clean up and dealloc
			if (ConnSlots[i]->cgi != NULL) ConnSlots[i]->cgi(ConnSlots[i]);

			# if SCDED_DBG >= 1
			printf("\nSCDED Slot:%d, death! force retire!\n",
				i);
			# endif

			SCDED_RetireConn(ConnSlots[i]);

			}
		}
	}

//--------------------------------------------------------------------------------------------------

  // check all connection slot for an request of an Idle Callback and execute it if required
  for (i=0; i<MAX_SCDED_CONN; i++)

	{

	// do idle callback if Slot is
	// +  allocated
        // + has a conn (!= NULL)
	// + F_GENERATE_IDLE_CALLBACK is set in ConnCtrl ...
	if ( (ConnSlots[i] != NULL) &&
		(ConnSlots[i]->conn != NULL) && //überflüssig ?
		(ConnSlots[i]->ConnCtrlFlags & F_GENERATE_IDLE_CALLBACK) )

		{

		// call it with current connection
		SCDED_IdleCb(ConnSlots[i]->conn);

		}
	}
*/
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: WebIf_IdleCb
 * Desc: Idle-Callback is triggered when connection has requested an Idle-Callback by ConnCtrl register.
 *       (if F_GENERATE_IDLE_CALLBACK in ConnCtrl is set -> request must be renewed in every cycle!!!)  
 * Para: void *arg -> struct espconn *conn
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR 
WebIf_IdleCb(void *arg)
  {

  // the arg is a ptr to the platform specific conn
  //struct espconn   *platform_conn = arg;	// ESP 8266 NonOS
  WebIf_Definition_t *platform_conn = arg;	// ESP 32 RTOS

  // get assigned HTTPD-Connection-Slot-Data
  WebIf_HTTPDConnSlotData_t *conn
	= platform_conn->reverse;

  # if SCDED_DBG >= 3
  printf("\nWebIf IdleCb, slot:%d, remote:%d.%d.%d.%d:%d, local port:%d, iat:%d, mem:%d>"
	,conn->SlotNo
	,conn->conn->proto.tcp->remote_ip[0]
	,conn->conn->proto.tcp->remote_ip[1]
	,conn->conn->proto.tcp->remote_ip[2]
	,conn->conn->proto.tcp->remote_ip[3]
	,conn->conn->proto.tcp->remote_port
	,conn->conn->proto.tcp->local_port
	,conn->InactivityTimer
	,system_get_free_heap_size());
  #endif

//--------------------------------------------------------------------------------------------------

  // Connection-Control Flags management for Idle-Callback
  // - CLEAR: F_CALLED_BY_RXED_CALLBACK, because this is not RX-Callback ...
  // - CLEAR: F_GENERATE_IDLE_CALLBACK, because now we have Idle Callback and it is needed once ...
  // clr flags
  conn->ConnCtrlFlags &= ~(F_CALLED_BY_RXED_CALLBACK + F_GENERATE_IDLE_CALLBACK);

//--------------------------------------------------------------------------------------------------

  // Response to open connection...
  SCDED_RespToOpenConn(conn);

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_SentCb
 * Desc: Data was sent callback is triggered when data was sent to a client conn of SCDED
 * Para: WebIf_Definition_t* WebIfDef ->ptr to the WebIf_Definition
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR 
WebIf_SentCb (void* arg)
{
  // the arg is a ptr to the platform specific conn
  //struct espconn   *platform_conn = arg;	// ESP 8266 NonOS
  WebIf_Definition_t* platform_conn = arg;	// ESP 32 RTOS

  // get assigned HTTPD-Connection-Slot-Data
  WebIf_HTTPDConnSlotData_t *conn = platform_conn->reverse;

  # if SCDED_DBG >= 3
  printf("\nWebIf SentCb, slot:%d, remote:%d.%d.%d.%d:%d, local port:%d, mem:%d>"
	,conn->SlotNo
	,conn->conn->proto.tcp->remote_ip[0]
	,conn->conn->proto.tcp->remote_ip[1]
	,conn->conn->proto.tcp->remote_ip[2]
	,conn->conn->proto.tcp->remote_ip[3]
	,conn->conn->proto.tcp->remote_port
	,conn->conn->proto.tcp->local_port
	,system_get_free_heap_size());
  #endif

//--------------------------------------------------------------------------------------------------

  # if SCDED_DBG >= 1
  if (! (conn->ConnCtrlFlags & F_TXED_CALLBACK_PENDING) ) {
	printf("|Err! TXedCbFlag missing>");
  }
  #endif

  // Connection-Control Flags management for Sent-Callback
  // - CLEAR: F_TXED_CALLBACK_PENDING, because SentCb is fired. TXED_CALLBACK is no longer pending ...
  // - CLEAR: F_CALLED_BY_RXED_CALLBACK, because this is not RX-Callback ...
  // - CLEAR: F_GENERATE_IDLE_CALLBACK, because now we have a callback. No Idle Callback is needed ...
  conn->ConnCtrlFlags &= ~(F_TXED_CALLBACK_PENDING + F_CALLED_BY_RXED_CALLBACK + F_GENERATE_IDLE_CALLBACK);

//--------------------------------------------------------------------------------------------------

  // Respond to open connection...
  SCDED_RespToOpenConn(conn);
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_RecvCb
 * Desc: Received callback is triggered when a data block is received from an client conn of SCDED
 * Para: void *arg -> struct espconn *conn
 *       char *recvdata -> ptr to received data
 *       unsigned short recvlen -> length of received data 
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR
WebIf_RecvCb(void *arg,
		char *recvdata,
		unsigned short recvlen) 
  {

  // the arg is a ptr to the platform specific conn
  //struct espconn   *platform_conn = arg;	// ESP 8266 NonOS
  WebIf_Definition_t *platform_conn = arg;	// ESP 32 RTOS

  // get assigned HTTPD-Connection-Slot-Data
  WebIf_HTTPDConnSlotData_t *conn
	= platform_conn->reverse;

  # if SCDED_DBG >= 3
  printf("\nWebIf RecvCb, slot %d, remote:%d.%d.%d.%d:%d,local port:%d, len:%d, mem:%d>"
	,platform_conn->SlotNo
	,platform_conn->proto.tcp->remote_ip[0]
	,platform_conn->proto.tcp->remote_ip[1]
	,platform_conn->proto.tcp->remote_ip[2]
	,platform_conn->proto.tcp->remote_ip[3]
	,platform_conn->proto.tcp->remote_port
	,platform_conn->proto.tcp->local_port
	,recvlen
	,system_get_free_heap_size());

  #endif

  # if SCDED_DBG >= 5
  SCDEFn_at_WebIf_M->HexDumpOutFn ("RX-HEX", recvdata, recvlen);
  # endif

//--------------------------------------------------------------------------------------------------

  // speed up cpu
//  system_update_cpu_freq(SYS_CPU_160MHz);

  // Connection-Control Flags management for Received-Callback
  // - CLEAR: F_GENERATE_IDLE_CALLBACK, because now we have Idle Callback and it is needed once ...
  // - SET: F_CALLED_BY_RXED_CALLBACK, because this is RX-Callback ... ...
  // clr flags
  conn->ConnCtrlFlags &= ~(F_GENERATE_IDLE_CALLBACK);
  // set flags
  conn->ConnCtrlFlags |= (F_CALLED_BY_RXED_CALLBACK);

//--------------------------------------------------------------------------------------------------

  // always parse received data	
  size_t nparsed = http_parser_execute (conn
			,&conn->conn->HTTPD_InstCfg->HTTPDparser_settings
			,recvdata
			,(size_t) recvlen);

  # if SCDED_DBG >= 5
  printf("|Msg parsed, (%d from %d)>",
	nparsed,
	recvlen);
  # endif

//---------------------------------------------------------------------------------------------------

  // continue or error ?
  if (nparsed != (size_t)recvlen)
	{

	# if SCDED_DBG >= 1
	printf("|Err: %u (%s)>"
		,HTTPD_ErrnoCode(conn->parser_http_errno)
		,HTTPD_ErrnoDescription(conn->parser_http_errno));
	# endif

	// give status about error to client
	SCDED_StartRespHeader2(conn);

	// stop processing more data
	conn->cgi = CGI_FINNISHED;

	}

//---------------------------------------------------------------------------------------------------

  // Upgrade connection to websockets?
  else if (conn->parser_upgrade == 1)

	{

	# if SCDED_DBG >= 3
	printf("|Upgrade conn>");
	# endif

	// can not answer..
	conn->cgi = CGI_UPGRADE_WEBSOCKET;

	}

//---------------------------------------------------------------------------------------------------

  // Response to open connection now	
  SCDED_RespToOpenConn(conn);

  // slow down cpu
//  system_update_cpu_freq(SYS_CPU_80MHz);

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: WebIf_ReconCb
 * Desc: Reconnect Info callback is triggered when the connection to client conn of SCDED is broken
 *       its unclear what to do in this cases ...
 * Para: void *arg -> struct espconn *conn
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR 
WebIf_ReconCb(void *arg
		,int8_t error)
  {

  // the arg is a ptr to the platform specific conn
  //struct espconn   *platform_conn = arg;	// ESP 8266 NonOS
  WebIf_Definition_t *platform_conn = arg;	// ESP 32 RTOS

  // get assigned HTTPD-Connection-Slot-Data
  WebIf_HTTPDConnSlotData_t *conn
	= platform_conn->reverse;

  # if SCDED_DBG >= 3
  printf("\nWebIf ReconCb, slot %d, remote:%d.%d.%d.%d:%d,local port:%d, error:%d, mem:%d>"
	,platform_conn->SlotNo
	,platform_conn->proto.tcp->remote_ip[0]
	,platform_conn->proto.tcp->remote_ip[1]
	,platform_conn->proto.tcp->remote_ip[2]
	,platform_conn->proto.tcp->remote_ip[3]
	,platform_conn->proto.tcp->remote_port
	,platform_conn->proto.tcp->local_port
	,error
	,system_get_free_heap_size());

  #endif

//---------------------------------------------------------------------------------------------------

  // process error information here! 
/*
sint8 err : error code
ESCONN_TIMEOUT - Timeout
ESPCONN_ABRT - TCP connection aborted
ESPCONN_RST - TCP connection abort
ESPCONN_CLSD - TCP connection closed
ESPCONN_CONN - TCP connection
ESPCONN_HANDSHAKE - TCP SSL handshake fail
ESPCONN_PROTO_MSG - SSL application invalid
*/

  WebIf_DisconCb(arg);

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: WebIf_DisconCb
 * Desc: Disconnected callback -> conn is disconnected -> clean up, free memory
 *       its unclear what to do in this cases ...
 * Para: WebIf_Definition_t* WebIfDef ->ptr to the WebIf_Definition
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR
WebIf_DisconCb(void *arg)
  {

  // the arg is a ptr to the platform specific conn
//struct espconn     *platform_conn = arg;	// ESP 8266 NonOS
  WebIf_Definition_t *platform_conn = arg;	// ESP 32 RTOS

  // get assigned HTTPD-Connection-Slot-Data
  WebIf_HTTPDConnSlotData_t *conn
	= platform_conn->reverse;

 # if SCDED_DBG >= 3
  printf("\nWebIf DisconCb, slot %d, remote:%d.%d.%d.%d:%d, local port:%d, mem:%d>"
	,platform_conn->SlotNo
	,platform_conn->proto.tcp->remote_ip[0]
	,platform_conn->proto.tcp->remote_ip[1]
	,platform_conn->proto.tcp->remote_ip[2]
	,platform_conn->proto.tcp->remote_ip[3]
	,platform_conn->proto.tcp->remote_port
	,platform_conn->proto.tcp->local_port
	,system_get_free_heap_size());
  #endif

//---------------------------------------------------------------------------------------------------

  // mark connections job in load serializer as finnished (by deleting bit)
  conn->conn->HTTPD_InstCfg->LoadSerializer &= ~(1<<(conn->SlotNo));

  // remove the conn / mark that the connection is gone
  conn->conn = NULL;

  // !!! CONN IS GONE FROM THIS POINT !!!

  // indicate cgi to clean up and dealloc
  if (conn->cgi != NULL) conn->cgi(conn);

  // free allocated memory for the Send-Buffer, if any
  if (conn->sendBuff != NULL) free(conn->sendBuff);

  // free allocated memory for Trailing Buff, if any
  if (conn->TrailingBuff != NULL) free(conn->TrailingBuff);

  // nicht schön
  // init http parser for new conn (goal is to free allocated memory!)
  HTTPD_ParserInit(conn, HTTP_BOTH);

  // finally free allocated memory for this WebIf_HTTPDConnSlotData_t struct
  free(conn);

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: WebIf_ConnCb
 * Desc: Connected callback is triggered in case of new connections to WebIf-HTTP-Daemon
 * Info:
 * Para: WebIf_Definition_t* WebIfDef -> ptr to the WebIf_Definition
 * Rets: int ->
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR 
WebIf_ConnCb(void *arg)
  {
  // the arg is a ptr to the platform specific conn
  //struct espconn   *platform_conn = arg;	// ESP 8266 NonOS
  WebIf_Definition_t *platform_conn = arg;	// ESP 32 RTOS

//--------------------------------------------------------------------------------------------------

// # if SCDED_DBG >= 3
  printf("\nWebIf ConCb, gets slot %d of %d, from remote:%d.%d.%d.%d:%d to local port:%d, mem:%d>"
	,platform_conn->SlotNo
	,MAX_SCDED_CONN
	,platform_conn->proto.tcp->remote_ip[0]
	,platform_conn->proto.tcp->remote_ip[1]
	,platform_conn->proto.tcp->remote_ip[2]
	,platform_conn->proto.tcp->remote_ip[3]
	,platform_conn->proto.tcp->remote_port
	,platform_conn->proto.tcp->local_port
	,system_get_free_heap_size());

//  #endif

//--------------------------------------------------------------------------------------------------

  // alloc conn-slot memory
  WebIf_HTTPDConnSlotData_t *conn
	= (WebIf_HTTPDConnSlotData_t*) malloc(sizeof(WebIf_HTTPDConnSlotData_t));

  // zero conn-slot memory
  memset(conn, 0, sizeof(WebIf_HTTPDConnSlotData_t));

  // store current connection in created conn slot
  conn->conn = platform_conn;

 // Write slot number for identification
  conn->SlotNo = conn->conn->SlotNo;

  // store reverse reference
  platform_conn->reverse = conn;

  // Register data received callback
  espconn_regist_recvcb(platform_conn, WebIf_RecvCb);

  // Register data reconnection / error info callback
  espconn_regist_reconcb(platform_conn, WebIf_ReconCb);

  // Register disconnected callback
  espconn_regist_disconcb(platform_conn, WebIf_DisconCb);

  // Register data sent callback
  espconn_regist_sentcb(platform_conn, WebIf_SentCb);

//--------------------------------------------------------------------------------------------------

  // init http parser for the new conn
  HTTPD_ParserInit(conn, HTTP_BOTH);

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_ETXSlotConnCb
 * Desc: Event-TX slot connected callback is triggered in case of a new connection to SCDE Daemon (E-TX slot)
 *       (special connection slot 0 is used for SCDE-Event-TX connection, slot 1-X are default slots)
 * Para: void *arg -> struct espconn *conn
 * Rets: int ->
 *--------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR 
SCDED_ETXSlotConnCb(void *arg)

  {
/*
 //spz struct espconn *pespconn = arg;

//--------------------------------------------------------------------------------------------------

  # if SCDED_DBG >= 3
  printf("\nSCDED ETXSlot-ConCb pespcon:%p, slot:0!, remote:%d.%d.%d.%d:%d, local port:%d, mem:%d>"
	,pespconn
	,pespconn->proto.tcp->remote_ip[0]
	,pespconn->proto.tcp->remote_ip[1]
	,pespconn->proto.tcp->remote_ip[2]
	,pespconn->proto.tcp->remote_ip[3]
	,pespconn->proto.tcp->remote_port
	,pespconn->proto.tcp->local_port
	,system_get_free_heap_size());
  # endif

//--------------------------------------------------------------------------------------------------

  // prepare new / or retire -> alloc + zero memory for struct ConnSlots
  if (ConnSlots[0] == NULL)

	// real new connection because structure is not allocated ...
	{

	ConnSlots[0] = malloc(sizeof(struct WebIf_HTTPDConnSlotData_t));

	// in case of activated Docked-Mode
if(1) //spz	if (SysCfgRamFlashMirrored->MySCDEETX_Cfg.ETX_GenOpt & F_TX_KEEP_CONN_ALIVE)

		// resync & clear ETX Queue. Force resync
		{

		SCDEETXQ_DeleteAllJobs();
AcETXQueueData = NULL;	// warum nicht immer aus der queue?
		SCDEETX_ResetSyncCb();

		}
	}

  // get pointer to conn-Slot for SCDE-Event-TX conn
  WebIf_HTTPDConnSlotData_t *conn = ConnSlots[0];

  // clear slot
  memset(conn, 0, sizeof(struct WebIf_HTTPDConnSlotData_t));

  // store current connection
//SPZ  conn->conn = pespconn;

  // store back reference
//spz  pespconn->reverse = conn;

  // Write slot number for identification
//  conn->SlotNo = (uint8) 0; // its cleared!

  // Register data received callback
//spz  espconn_regist_recvcb(pespconn, SCDED_RecvCb);

  // Register data reconnection / error info callback
//spz  espconn_regist_reconcb(pespconn, SCDED_ReconCb);

  // Register disconnected callback
//spz  espconn_regist_disconcb(pespconn, SCDED_DisconCb);

  // Register data sent callback
//spz  espconn_regist_sentcb(pespconn, SCDED_SentCb);

//--------------------------------------------------------------------------------------------------

  // init http parser for new conn
  HTTPD_ParserInit(conn, HTTP_BOTH);

  // set callback for SCDE-Event-TX
  conn->cgi = SCDEETX_ETXPCb;

  // set current slot-activity status
  conn->SlotParserState = s_ETX_ready_to_TX_request;

  // force idle callback for Event-TX processing (F_GENERATE_IDLE_CALLBACK)
  conn->ConnCtrlFlags |= F_GENERATE_IDLE_CALLBACK;
*/
  return 1;

  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_Init
 * Desc: Smart-Connected-Device-Engine-Daemon - General INIT
 * Para: HTTPDUrls *fixedUrls -> ptr to urls resources data
 *       SCDE_RPCActiveRes *fixedActiveResources -> ptr to active resources data
 *       int Port -> used port
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR 
SCDED_Init(WebIf_ActiveResourcesDataA_t *fixedUrls
		,WebIf_ActiveResourcesDataB_t *fixedActiveResources
		,int Port) 
  {
/*
  // Init *ConnSlots[MAX_HTTPD_CONN] to NULL
  memset(&ConnSlots, 0, sizeof (*ConnSlots[MAX_SCDED_CONN]));

  // set content... Hard Coding it?
  BuiltInUrls = fixedUrls;
  BuiltInActiveResources = fixedActiveResources;

  // Init HTTPD Parser settings
  http_parser_settings_init(&HTTPDparser_settings);

  // Set HTTPD Parser Fn callbacks
  HTTPDparser_settings.on_message_begin = HTTPDMsgBeginCb;
  HTTPDparser_settings.on_url = HTTPDUrlFoundCb;
//HTTPDparser_settings.on_status ???
  HTTPDparser_settings.on_header_field = HTTPDHeaderFldCb;
  HTTPDparser_settings.on_header_value = HTTPDHeaderValCb;
  HTTPDparser_settings.on_headers_complete = HTTPDHeaderPrsdCompleteCb;
  HTTPDparser_settings.on_body = HTTPDFndBodyDataCb;
  HTTPDparser_settings.on_message_complete = HTTPDMsgPrsdCompleteCb;
//HTTPDparser_settings.on_chunk_header ???
//HTTPDparser_settings.on_chunk_complete ???

  // Init Load Serializer
  LoadSerializer = 0;

  // platform specific initializion of the scded
  SCDED_PlatformInit(Port, MAX_SCDED_CONN);

*/
/* new platform strategy
  # if SCDED_DBG >= 2
  printf("SCDED init at Port=%d, Espconn=%p, Esp_tcp=%p, ConnSlots=%d\n"
	,Port

	,&HTTPDConn
	,&HTTPDTcp
	,MAX_SCDED_CONN);
  # endif	

  // Init Esp_tcp for HTTPD-Server
  HTTPDTcp.local_port = Port;

  // Init Espconn for HTTPD-Server
  HTTPDConn.type = ESPCONN_TCP;
  HTTPDConn.state = ESPCONN_NONE;
  HTTPDConn.proto.tcp = &HTTPDTcp;

  // setup connect cb for HTTP Daemon connection
  espconn_regist_connectcb(&HTTPDConn,
	SCDED_DefaultSlotConnCb);

  // start listening for connections
  espconn_accept(&HTTPDConn);

  // setup maximum HTTP Daemon connections
  espconn_tcp_set_max_con_allow(&HTTPDConn,
	MAX_HTTPD_CONN);

  // setup default connection timeout
  espconn_regist_time(&HTTPDConn, 0, 0);	// HTTPD_TIMEOUT, 0); // No Timeout

 # if SCDED_DBG >= 4
  printf("SCDED now %d slots\n",
	espconn_tcp_get_max_con_allow(&HTTPDConn));
 # endif

  // setup defaults for conn 
  espconn_set_opt(&HTTPDConn,0b11);
*/
  }




/*
// SCDE Event-TX-Connect-Control - State (enum) Monitoring
enum ETXCC_State
  { s_idle = 0					// #000 idle state, !! no connection control !!
  , s_delay_then_idle = 8*10-1			// #079 connection control - delay idle state
  , s_dns_lookup_failed	= 80			// #080 connection control - DNS lookup failed state
  , s_monitoring_dns_lookup = 80+8*10-1		// #159 connection control - monitoring DNS lookup to get IP adress
  , s_conn_proc_IP_failed = 160			// #160 connection control - connection process to IP failed state
  , s_monitoring_conn_proc_IP = 160+8*10-1	// #239 connection control - monitoring connection process to an IP adress
  };
*/


/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDEETX_Connect
 * Desc: Connects the SCDE-Event TX connection (should be called from 10Hz timer !)
 *       conditions:
 *       + SCDE-ETX enabled && not connected
 *       + Queue filled || "TX Keep Alive to RX" is activated
 *       includes Connect Control by status
 * Para: -/-
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR
SCDEETX_Connect(void)
  {
/*
  // is the SCDE-Event TX activated ?
  if (1) //SPZ if (SysCfgRamFlashMirrored->MySCDEETX_Cfg.ETX_GenOpt & F_ENABLE_SCDE_EVENT_TX)
	{

	// is the SCDE-Event TX Slot (0) NOT connected ?
  	if (ConnSlots[0] == NULL)
		{

		// is SCDE-E-TX-Queue filled or SCDE-E-TX "TX Keep Alive to RX" activated -> we need  a connection
if (1)  //SPZ		if ( (SCDEETXQ_GetNumJobs() != 0) || (SysCfgRamFlashMirrored->MySCDEETX_Cfg.ETX_GenOpt & F_TX_KEEP_CONN_ALIVE) )

			{

			# if SCDEETX_DBG >= 5
			printf("\nSCDEETX CCS:%d",
				ETXCC_State);
			# endif

			switch (ETXCC_State)

				{

//---------------------------------------------------------------------------------------------------

				// is E-TX-Connect-Control State > s_idle? -> Connect Control not active (do make new connection try)
				case s_idle:

				# if SCDEETX_DBG >= 3
				printf("SCDEETX ConnTry\n");
				# endif

				# if SCDEETX_DBG >= 5
				// Display SCDE-Event-TX Queue for debugging
				SCDEETXQ_DisplayJobs();

				printf("SCDEETX CCS:%d. Statistics:Conn=%d; Msg=%d;RcvOK=%d;RcvNOK=%d\n",
					ETXCC_State,
					SCDEETX_ConnCNT,
					SCDEETX_MsgCNT,
					SCDEETX_RcvOKCNT,
					SCDEETX_RcvNOKCNT);
  				# endif

				// Try to extract IP ...
	if (1) /spz			if (UTILS_StrToIP((int8_t*) SysCfgRamFlashMirrored->MySCDEETX_Cfg.Domain,
					&SysCfgRamFlashMirrored->MySCDEETX_Cfg.ip))/

					// we have IP! skipping DNS lookup...
					{

	/ spz				SCDEETX_IPConnect((char *)SysCfgRamFlashMirrored->MySCDEETX_Cfg.Domain,
						(ip_addr_t*) &SysCfgRamFlashMirrored->MySCDEETX_Cfg.ip.addr,
						&SCDEETXConn);/

					}

  				else

					// we have NO IP! we need DNS lookup to get IP
					{

					SCDEETX_DNSLookup(&SCDEETXConn);

					}

				break;

//---------------------------------------------------------------------------------------------------

				// is E-TX-Connect-Control State > s_dns_lookup_failed? -> Connect Control detects DNS lookup failure
				case s_dns_lookup_failed:

				# if SCDEETX_DBG >= 1
				printf("SCDEETX DNS-Lookup TIMEOUT!\n");
				# endif

				// restart
				ETXCC_State = s_idle;

				break;

//---------------------------------------------------------------------------------------------------

				// is E-TX-Connect-Control State > s_conn_proc_IP_failed? -> Connect Control detects connection to IP failed
				case s_conn_proc_IP_failed:

				# if SCDEETX_DBG >= 1
				printf("SCDEETX ConnCb TIMEOUT!\n");
				# endif

				// restart
				ETXCC_State = s_idle;

				break;

//---------------------------------------------------------------------------------------------------

				default:
				ETXCC_State--;

				}
			}
		}

	}
*/
}



//##################################################################################################
//### FName: SCDEETX_DNSLookup
//###  Desc: Make an DNS Lookup, that CALLBACKs SCDEETX_IPConnect with IP
//###  Para: void *arg -> espconn *conn
//###  Rets: NONE
//##################################################################################################
void ICACHE_FLASH_ATTR
SCDEETX_DNSLookup(void *arg)
  {
//spz  struct espconn *conn = (struct espconn *)arg;
/*spz
  err_t Result = espconn_gethostbyname(conn,
	(char *)SysCfgRamFlashMirrored->MySCDEETX_Cfg.Domain,
	&SysCfgRamFlashMirrored->MySCDEETX_Cfg.ip,
	SCDEETX_IPConnect);*/

  # if SCDEETX_DBG >= 3
  printf("SCDEETX DNS lookup: %s err:%d\n",
	SysCfgRamFlashMirrored->MySCDEETX_Cfg.Domain,
	Result);
  # endif

  if (1) //spz if (Result == ESPCONN_ARG)
	{

	# if SCDEETX_DBG >= 1
	printf("SCDEETX DNSErr!\n");
	# endif

	// Event-TX-Connect-Control new state: s_delay_then_idle (because DNS-lookup failed)
	ETXCC_State = s_delay_then_idle;

	return;

	}

  // Event-TX-Connect-Control new state: s_monitoring_dns_lookup (because we want to monitor DNSLookup func success)
  ETXCC_State = s_monitoring_dns_lookup;

  }



//##################################################################################################
//### FName: SCDEETX_IPConnect
//###  Desc: Connect with IP (->DNS Lookup complete) - starting Connection
//###  Para: const char *name -> const char *DomainNameDestination
//###        ip_addr_t *ip    -> ip_addr_t *IPDestination
//###	     void *espconn    -> struct espconn *espconn
//###  Rets: NONE
//##################################################################################################
void ICACHE_FLASH_ATTR
SCDEETX_IPConnect(const char *name, ip_addr_t *ip, void *arg)
  {

//spz  static esp_tcp tcp;

  //struct espconn *pespconn = (struct espconn *)arg;

  if (ip == NULL)
	{

	# if SCDEETX_DBG >= 1
	printf("SCDEETX dest. IP inv.!\n");
	# endif

	// Event-TX-Connect-Control new state: s_delay_then_idle (because IP we got is invalid)
	ETXCC_State = s_delay_then_idle;

	return;
	}

  # if SCDEETX_DBG >= 3
  printf("SCDEETX conn. to remote:%d.%d.%d.%d:%d from local port:%d\n",
	*((uint8 *)&ip->addr),
	*((uint8 *)&ip->addr + 1),
	*((uint8 *)&ip->addr + 2),
	*((uint8 *)&ip->addr + 3),
	SysCfgRamFlashMirrored->MySCDEETX_Cfg.Dest_Port,
	SysCfgRamFlashMirrored->MySCDEETX_Cfg.ESP_Port);
  # endif

  // prepare connection structure
/*spz  pespconn->type = ESPCONN_TCP;
  pespconn->state = ESPCONN_NONE;
  pespconn->proto.tcp = &tcp;
  pespconn->proto.tcp->local_port = SysCfgRamFlashMirrored->MySCDEETX_Cfg.ESP_Port;
  pespconn->proto.tcp->remote_port = SysCfgRamFlashMirrored->MySCDEETX_Cfg.Dest_Port;
  memcpy(pespconn->proto.tcp->remote_ip, &ip->addr, 4);

  espconn_regist_connectcb(pespconn, SCDED_PlatformETXSlotConnCb);

  int8_t Result = espconn_connect(pespconn);

  if (Result)
	{

	# if SCDEETX_DBG >= 1
	printf("SCDEETX ConErr:%d!\n",
		Result);
	# endif

	// Event-TX-Connect-Control new state: s_delay_then_idle (because connect func gives us an error)
	ETXCC_State = s_delay_then_idle;

	return;
	}

  // setup defaults for this conn 
  espconn_set_opt(pespconn,0b11); // free imediatly+disable nagle
//espconn_set_opt(pespconn,0b1011); // free imediatly+disable nagle+TCP keep alive

  // Event-TX-Connect-Control new state: s_waked_up_monitoring_conn_proc_IP #02 (monitoring event conncb)
  ETXCC_State = s_monitoring_conn_proc_IP;*/

  }






/*
General Function

                                                             Connection-Response-Flags S=SET C=CLEAR
                                                                                                                  W H
                                                                                                                  A O
                                                                                                                  N L
                                                                                                                  T D
                                                                                                                  S   
                                                                                                                    C
                                                                                                                  C O
                                                                                                                  B N
                                                                                                                    N
HTTPD_RespondToOpenConnection                     
1. send trailingbuff (if any)                     // |0|0|0|0|0|0|0|0|0|0|1|1|1|1|1|1|1|1|1|1|2|2|2|2|2|2|2|2|2|2|3|3|
                                                  // |0|1|2|3|4|5|6|7|8|9|0|1|2|3|4|5|6|7|8|9|0|1|2|3|4|5|6|7|8|9|0|1|
2. preparing PC                                                                                                     
|                                                 // | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
 \- Event 'PC_FINNISHED_CLEANUP' ?                                                                                 
|                                           Set/Clr->| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
 \- Event 'PC_NOT_ASSIGNED' ?                                                                                      
|                                           Set/Clr->| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |S|
 \- Event 'CGI_UPGRADE_WEBSOCKET' ?                                                                                 
|                                           Set/Clr->| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
 \- Default PC execution ?                                                                                         
  |
   \
   3. Processing PC-Result
   |                                        Set/Clr->| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
    \- Event 'HTTPD_CGI_DISCONNECT_CONN' ?                                                                                 
   |                                        Set/Clr->| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
    \- Event 'HTTPD_CGI_PROCESS_CONN' ?                                                                                 
   |                                        Set/Clr->| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |S|S|
    \- Event 'HTTPD_CGI_WAITRX_CONN' ?                                                                                 
   |                                        Set/Clr->| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |S|
    \- Event ' ' ?                                                                                 
   |                                        Set/Clr->| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
   \- Event ' ' ?                                                                                 

4.
|                                                                                               
 \- Send Data, if any
|                                                                                               
 \- ASSIGN IDLE CB                               if->| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |X| |  ?       
|                                                                                               
 \- ASSIGN IDLE CB                               if->| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |X| |  ?      



*/











// tokenExecResult -> special states		// Result:
#define TOKEN_ERROR_NO_MATCH		 0	// 00 Token Error! No Match!
#define STATE_FOUND_WITHOUT_VALUE	 -1	// 00 found without value
#define STATE_EMPTY_AND_NO_ERROR	-2	// -2 empty and no error
#define STATE_ERROR_NOT_FOUND		-3	// -3 error not found

// Query Resource processing steps (enum)
enum step				// #XX for debugging
  { s_cmp_with_built_in_resources= 0	// #00 compare with built in resources
  , s_chk_if_method_is_avail_for_res	// #01 check if method is available for the resource
  , s_chk_if_scheme_is_avail_for_res	// #02 check if scheme is available for the resource
  , s_chk_if_mime_is_avail_for_res	// #03 check if mime is available for the resource
  , s_chk_if_the_res_row_is_enabled	// #04 check if the resource row is enabled
  , s_get_match_in_table_b		// #05 Get the Cgi-Data-Row number for this Resource-Row
  , s_prepare_the_callback_data		// #06 prepare the callback data
  , s_return_callback_is_prep		// #07 return, callback is prepared
  , s_load_resource_not_found_cb	// #08 load resource not found callback
};

#define CURRENT_STEP() NextStep
#define UPDATE_STEP(V) NextStep = (enum step) (V);
#define STEP_UPDATED() goto stepupdated;   

/*
 *--------------------------------------------------------------------------------------------------
 *FName: HTTPD_ParseUrl
 * Desc: Analyzes the path extracted by HTTP-Parser and tries to find + load a matching build in
 *       resource including callback data. Installs the filesystem callback if BI resource not found.
 *       Installs callback for 'not found' error 
 * Para: WebIf_HTTPDConnSlotData_t* conn -> ptr to connection slot
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void
HTTPD_ParseUrl(WebIf_HTTPDConnSlotData_t* conn)
{
  int rowCnt_B;				// to adress row in table Active-Dir-Url-Res-Data B
  int DocMime;				// for mime matching requirements

  const char* SrcPtr;			// Ptr to the source -> requested url
  const char* UrlSeekPtr;		// Ptr to the url in Active-Dir-Url-Res-Data

  enum step NextStep = s_cmp_with_built_in_resources;

  #if SCDED_DBG >= 4
  printf("\n|Checking if Active-Dir-Url-Res-Data is available for path '%s'>",
	conn->url);
  #endif	

//--------------------------------------------------------------------------------------------------

  // holds current Definitions Active-Directory-Url-Resource-Data A
  WebIf_ActiveResourcesDataA_t* ActiveDirUrlResDataA;

  // holds current Definitions Active-Directory-Url-Resource-Data b
  WebIf_ActiveResourcesDataB_t* ActiveDirUrlResDataB;	

//--------------------------------------------------------------------------------------------------

  // Current Job: Check if path is empty
  // matched? -> go to state "load_resource_not_found_cb", will load 'resource not found' cb & ret
  // else     -> continue ...
  if ( conn->url == NULL ) UPDATE_STEP (s_load_resource_not_found_cb);
  
//--------------------------------------------------------------------------------------------------

  // Current Job: Loop through Common_Definitions to get WebIf provided data
  Common_Definition_t* Common_Definition;

  STAILQ_FOREACH (Common_Definition, &SCDERoot_at_WebIf_M->HeadCommon_Definitions, entries) {

  // Current Job: Check if current Definition has Active-Dir-Url-Res-Data
  // yes? -> continue - load them to loop the rows
  // no?  -> do nothing -> results in load next definition (STAILQ_FOREACH)
  if (Common_Definition->ActiveResourcesDataA) {

	// load Active-Resources-Data of current Definition
	ActiveDirUrlResDataA =
		(WebIf_ActiveResourcesDataA_t*) Common_Definition->ActiveResourcesDataA;
	ActiveDirUrlResDataB =
		(WebIf_ActiveResourcesDataB_t*) Common_Definition->ActiveResourcesDataB;

		# if SCDED_DBG >= 4
		printf("\n|Definition '%.*s' has Active-Dir-Url-Res-Data. Looping Rows>\n"
			,Common_Definition->nameLen
			,Common_Definition->name);
  		# endif

	// Before loop: restart row count in table Active-Dir-Url-Res-Data A
	int rowCnt_A = 0;

//--------------------------------------------------------------------------------------------------

	// Current Job: Loop built in resources till we are at the end of the current definitions
	//              "built in resources" list
	// Matched -> go to state "cmp_with_built_in_resources", will compare with built in resources
	// Else -> end UrlResource-Row matching loop ("*" endmark reached)
	while ( ActiveDirUrlResDataA[rowCnt_A].Url[0] != '*' ) {

		UPDATE_STEP(s_cmp_with_built_in_resources);

//--------------------------------------------------------------------------------------------------

		// restart here, if the processing step is updated ...
		stepupdated:

 		# if SCDED_DBG >= 4
		printf("|S%d>",
			NextStep);
  		# endif

  		switch (NextStep) {

//--------------------------------------------------------------------------------------------------

			// State: Check if the path of the requested url (excluding mime) 
			//        matches this active Resource-Row
			case s_cmp_with_built_in_resources:
			;
			// stores + inits the result of token execution
			int tokenExecResult = 0;	// RESULT 00 Token Error! No Match!

			// ActiveID -> starting with 'we have no Definition'
			conn->activeDirFndDefiniton = NULL;
	
			// An ptr to the source -> requested url
			SrcPtr = conn->url;

			// An ptr to the url in Active-Dir-Url-Res-Data
			UrlSeekPtr = ActiveDirUrlResDataA[rowCnt_A].Url;

			#if SCDED_DBG >= 4
			printf("\n|R-Row %d CMP:", rowCnt_A);
			#endif

			// Current Job: Check if Url from Active-Dir-Url-Res-Data matches 
			// (\0 = string-end = 100% compared) !OR! is different. 
			// If its different check + execute possible token for active content, too.
			while (*UrlSeekPtr != '\0') {

				// Inner compare loop. Loops till source str is zero or different. 
				while ( (*SrcPtr == *UrlSeekPtr) &&
					(*SrcPtr != '\0') ) {

					#if SCDED_DBG >= 4
					printf("%c", *UrlSeekPtr);
					#endif

					SrcPtr++;
					UrlSeekPtr++;
				}

				// Its different here -> Maybe next char at UrlSeekPtr is a token
				// for active content. Call ExecActiveDirToken to check + execute.
				// If Active-Content not matching -> Break the loop to stop compare
				if (WebIf_ExecActiveDirToken(conn, Common_Definition,
					&SrcPtr, &UrlSeekPtr, &tokenExecResult )) break;

				// Matched! Continue compare loop!
				// conn->activeDirFndDefiniton, SrcPtr, UrlSeekPtr, tokenExecResult updated now !
				// conn->activeDirFndDefiniton MAY contain an matching Definition
				// tokenExecResult MUST contain result code of token execution
				// RESULT CODES:
				// #00 Token Error! No Match!
				// ??alllowed?? -2 init and no match no error //empty and no error
				// -3 no match error from ExecActiveDirToken -> Start next Res-Row CMP
				// -1 token executed ok, continue, no Definition, alt.filename requested
			}

			// Current job: Analyze result of previous Url-match-check. If UrlSeekPtr
			// is at string-end (\0) and 'tokenExecResult' holds NOT an error/no match (0)   
			// Matched ->  continue Mime check at state: s_chk_if_mime_is_avail_for_res
			// Else -> End UrlResource-Row matching loop (no match)
			if ( (*UrlSeekPtr == '\0') && (!tokenExecResult) ) {

				# if SCDED_DBG >= 4
				if (tokenExecResult)
					printf("|Url cmp result:%d>", tokenExecResult);
				
				// the matching Definition name
				if (conn->activeDirFndDefiniton) 
					printf(">Definition '%.*s' matched. Its Module '%.*s'>",
					conn->activeDirFndDefiniton->nameLen,
					conn->activeDirFndDefiniton->name,
					conn->activeDirFndDefiniton->module->provided->typeNameLen,
					conn->activeDirFndDefiniton->module->provided->typeName);
				# endif

				UPDATE_STEP(s_chk_if_mime_is_avail_for_res);
				STEP_UPDATED()
			}

			// url (incl.tokens) not matching, break .
			break;

// -------------------------------------------------------------------------------------------------

			// State: check if the requested url ends with an availabe mime
			case s_chk_if_mime_is_avail_for_res:

			// stores the identified mime. 0 = no mime
			DocMime = 0;

			// is there a mime? If not keep 0 -> no mime
			if (*SrcPtr == '.') {

				// skip the '.'
				SrcPtr++;

				// seek table - till found, or NULL (at least at pos 16)
				while ( (AvailMimes[DocMime].ext != NULL) &&
					(strcmp(SrcPtr, AvailMimes[DocMime].ext) !=0) )
					DocMime++;

				// no valid doc mime found ?, break
				if (AvailMimes[DocMime].ext == NULL) {

					// valid mime not found, break ..
					break;
				}

				SrcPtr += strlen(AvailMimes[DocMime].ext);
			}

			# if SCDED_DBG >= 4
			printf("|DocMimeBit:%d>",
				DocMime);
			# endif

			// Current Job: Check if a VALID (<16) mime is available for this resource
			//              and check if SrcPtr is 100% compared
			// Matched -> continue at state: s_chk_scheme_avail_for_cont
			// Else -> End UrlResource-Row matching loop (no match)
			if ( (DocMime < 16) &&
				(ActiveDirUrlResDataA[rowCnt_A].AllowedDocMimesBF &
				 1 << DocMime) && (*SrcPtr == '\0') ) {	

				UPDATE_STEP(s_chk_if_method_is_avail_for_res);
				STEP_UPDATED()	
			}

			// mime not available, break ..
			break;

//--------------------------------------------------------------------------------------------------

			// State: Check if the requested method is availabe
			case s_chk_if_method_is_avail_for_res:

			// Current Job: Check if method is available
			// Matched -> continue at state: s_chk_if_scheme_is_avail_for_res
			// Else -> End UrlResource-Row matching loop (no match)
			if ( (ActiveDirUrlResDataA[rowCnt_A].AllowedMethodsBF &
				(1 << conn->parser_method) ) && (conn->parser_method <= 31) ) {

				UPDATE_STEP(s_chk_if_scheme_is_avail_for_res);
				STEP_UPDATED()	
			}

			// requested method not available, break ..
			break;

//--------------------------------------------------------------------------------------------------

			// State: Check if the requested scheme type is availabe
			case s_chk_if_scheme_is_avail_for_res:

			// Current Job: Check if scheme is available
			// Matched -> continue at state: s_chk_if_the_res_row_is_enabled
			// Else -> End UrlResource-Row matching loop (no match)
			if ( (ActiveDirUrlResDataA[rowCnt_A].AllowedSchemesBF &
				 1 << conn->parser_scheme) && (conn->parser_scheme <= 15) ) {

				UPDATE_STEP(s_chk_if_the_res_row_is_enabled);
				STEP_UPDATED()	
			}

			// requested scheme not available, break ..
			break;

//--------------------------------------------------------------------------------------------------

			// State: Check if this Resource-Row is enabled
			case s_chk_if_the_res_row_is_enabled:

			// Current Job: Check if Row is enabled
			// Matched -> continue at state: s_get_match_in_table_b
			// Else -> End UrlResource-Row matching loop (no match)
			if ( (!ActiveDirUrlResDataA[rowCnt_A].EnaByBit) ||
				(conn->conn->HTTPD_InstCfg->DirConEnaCtrlFB &
				(1 << (ActiveDirUrlResDataA[rowCnt_A].EnaByBit - 1) ) ) ) {

				UPDATE_STEP(s_get_match_in_table_b);
				STEP_UPDATED()
			}

			// resource row is not enabled, break ..
			break;

//--------------------------------------------------------------------------------------------------

			// State: Get the Cgi-Data-Row number for this Resource-Row
			case s_get_match_in_table_b:

			// check if Active Page Data is found -> continue at state: 
			//     s_prepare_the_callback_data
			; //need this

			// temp value for ???
			uint32_t RPCFucIDMask = 0xffff0000 | (1 << DocMime); // any docmime allowed is ok

			// temp value for ???
			int RPCFucID = ( (ActiveDirUrlResDataA[rowCnt_A].RPCNo << 16) |
				(1 << DocMime) );

			// find and assign data in Active-Dir-Url-Res-Data B. We expect an match!
			while ( (ActiveDirUrlResDataB[rowCnt_B].RPCFucID &
				 RPCFucIDMask) != RPCFucID) rowCnt_B++;

			# if SCDED_DBG >= 4
			printf("|PC-Row:%d>", rowCnt_B);
			# endif

			UPDATE_STEP(s_prepare_the_callback_data);

			STEP_UPDATED()

			// no exec here ...
			break;

//--------------------------------------------------------------------------------------------------

			// State: Prepare the callback data from Active-Dir-Url-Res-Data
			case s_prepare_the_callback_data:

			// Clear cgi Data
			conn->PCData = NULL;

			// Correct DestUrl pointer to real Filesystem url to get the right template "Switch.htm" ??????????? //conn->url;
			conn->DestUrl = (char* volatile) conn->conn->HTTPD_InstCfg->BuiltInUrls[rowCnt_A].Url;	// DESTURL BENÖTIGT????????????????????????

			// Current Job: Check if tokenExecResult >= -1. Then an alternative filename
			//              is assigned to load content from standard file system
			// Matched -> find and use alternative filename for content load stored after the path "/\xfe/S0\x00 S0X.htm"
			// Else -> use requested filename for content load
			if (tokenExecResult >= -1) {

				// prepare conn->AltFSFile -> Alternative-File-System-File
				UrlSeekPtr++; // \x00 to ' '
				UrlSeekPtr++; //  ' ' to beginning of Alternative-File-System-File name
				conn->AltFSFile = (char* volatile) UrlSeekPtr; // e.g. "SwITCH.htm";

				#if SCDED_DBG >= 4
				printf("File-Name alternative: %s\n", conn->AltFSFile);
				#endif

			}

			// use requested filename (if no alternative is assigned) for content load...
			else conn->AltFSFile = conn->url;

			// Store cgi CB Function
			conn->cgi = ActiveDirUrlResDataB[rowCnt_B].PCb;

			// Store cgi CB data
			conn->PCArg = ActiveDirUrlResDataB[rowCnt_B].PCArg;

			UPDATE_STEP(s_return_callback_is_prep);

			STEP_UPDATED()

			// no exec here ...
			break;

//--------------------------------------------------------------------------------------------------

			// State: Callback prepared, callback data loaded, return
			case s_return_callback_is_prep:

			return;

			// no exec here ...
			break;

//--------------------------------------------------------------------------------------------------

			// Processing finished, case resource not found callback loaded, return
			case s_load_resource_not_found_cb:

			#if SCDED_DBG >= 1
			printf("|404 Cb!>");
			#endif

			// set callback for "Not Found Error"
			conn->cgi = NotFoundErr_cgi;

			return;

			// no exec here ...
			break;

//--------------------------------------------------------------------------------------------------

		}

		// next urlResource-Row of current Definition
		rowCnt_A++;	

	} // end of urlResource-Row matching loop (loops till "*" endmark)

	#if SCDED_DBG >= 4
	printf("\n|*\n");
	#endif

  } // end of has ActiveResourcesData
  } // end of Definitions-matching-loop

//--------------------------------------------------------------------------------------------------

  // No matching Active-Dir-Url-Res-Data found in Definitions. Load file system callback and return 

  #if SCDED_DBG >= 4
//  printf("\n|*, no matching Active-Dir-Url-Res-Data found, loading standard file-system cb\n");
  printf("\n|Looped all Definitions. No match found. Loading file-system cb.\n");
  #endif
	
  conn->cgi = WebIF_EspFSStdFileTX;	// load callback Fn for standard file tx
  conn->PCArg = NULL;			// data for callback Fn

  // Clear cgi Data
  conn->PCData = NULL;
 
  return;
}














































 













//#include <assert.h>
//#define assert(ignore)((void) 0)



#ifndef ULLONG_MAX
# define ULLONG_MAX ((uint64_t) -1) /* 2^64-1 */
#endif

#ifndef MIN
# define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef ARRAY_SIZE
# define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

#ifndef BIT_AT
# define BIT_AT(a, i)                                                \
  (!!((unsigned int) (a)[(unsigned int) (i) >> 3] &                  \
   (1 << ((unsigned int) (i) & 7))))
#endif

#ifndef ELEM_AT
# define ELEM_AT(a, i, v) ((unsigned int) (i) < ARRAY_SIZE(a) ? (a)[(i)] : (v))
#endif

#define SET_ERRNO(e)                                                 \
do {                                                                 \
  conn->parser_http_errno = (e);                                          \
} while(0)


#define CURRENT_STATE() p_state
#define UPDATE_STATE(V) p_state = (enum state) (V);
#define RETURN(V)                                                    \
do {                                                                 \
  conn->parser_state = CURRENT_STATE();                                   \
  return (V);                                                        \
} while (0);
#define REEXECUTE()                                                  \
  goto reexecute;                                           



#ifdef __GNUC__
# define LIKELY(X) __builtin_expect(!!(X), 1)
# define UNLIKELY(X) __builtin_expect(!!(X), 0)
#else
# define LIKELY(X) (X)
# define UNLIKELY(X) (X)
#endif





/* Run the notify callback FOR, returning ER if it fails */
#define CALLBACK_NOTIFY_(FOR, ER)                                    \
do {                                                                 \
  assert(HTTP_PARSER_ERRNO(conn) == HPE_OK);                       \
                                                                     \
  if (LIKELY(settings->on_##FOR)) {                                  \
    conn->parser_state = CURRENT_STATE();                                 \
    if (UNLIKELY(0 != settings->on_##FOR(conn))) {                 \
      SET_ERRNO(HPE_CB_##FOR);                                       \
    }                                                                \
    UPDATE_STATE(conn->parser_state);                                     \
                                                                     \
    /* We either errored above or got paused; get out */             \
    if (UNLIKELY(HTTP_PARSER_ERRNO(conn) != HPE_OK)) {             \
      return (ER);                                                   \
    }                                                                \
  }                                                                  \
} while (0)

/* Run the notify callback FOR and consume the current byte */
#define CALLBACK_NOTIFY(FOR)            CALLBACK_NOTIFY_(FOR, p - data + 1)

/* Run the notify callback FOR and don't consume the current byte */
#define CALLBACK_NOTIFY_NOADVANCE(FOR)  CALLBACK_NOTIFY_(FOR, p - data)

/* Run data callback FOR with LEN bytes, returning ER if it fails */
#define CALLBACK_DATA_(FOR, LEN, ER)                                 \
do {                                                                 \
  assert(HTTP_PARSER_ERRNO(conn) == HPE_OK);                       \
                                                                     \
  if (FOR##_mark) {                                                  \
    if (LIKELY(settings->on_##FOR)) {                                \
      conn->parser_state = CURRENT_STATE();                               \
      if (UNLIKELY(0 !=                                              \
                   settings->on_##FOR(conn, FOR##_mark, (LEN)))) { \
        SET_ERRNO(HPE_CB_##FOR);                                     \
      }                                                              \
      UPDATE_STATE(conn->parser_state);                                   \
                                                                     \
      /* We either errored above or got paused; get out */           \
      if (UNLIKELY(HTTP_PARSER_ERRNO(conn) != HPE_OK)) {           \
        return (ER);                                                 \
      }                                                              \
    }                                                                \
    FOR##_mark = NULL;                                               \
  }                                                                  \
} while (0)
  
/* Run the notify callback FOR and consume the current byte */
#define CALLBACK_NOTIFY(FOR)            CALLBACK_NOTIFY_(FOR, p - data + 1)

/* Run the notify callback FOR and don't consume the current byte */
#define CALLBACK_NOTIFY_NOADVANCE(FOR)  CALLBACK_NOTIFY_(FOR, p - data)


/* Run data callback FOR with LEN bytes, returning ER if it fails */
#define CALLBACK_DATA_(FOR, LEN, ER)                                 \
do {                                                                 \
  assert(HTTP_PARSER_ERRNO(conn) == HPE_OK);                       \
                                                                     \
  if (FOR##_mark) {                                                  \
    if (LIKELY(settings->on_##FOR)) {                                \
      conn->parser_state = CURRENT_STATE();                               \
      if (UNLIKELY(0 !=                                              \
                   settings->on_##FOR(conn, FOR##_mark, (LEN)))) { \
        SET_ERRNO(HPE_CB_##FOR);                                     \
      }                                                              \
      UPDATE_STATE(conn->parser_state);                                   \
                                                                     \
      /* We either errored above or got paused; get out */           \
      if (UNLIKELY(HTTP_PARSER_ERRNO(conn) != HPE_OK)) {           \
        return (ER);                                                 \
      }                                                              \
    }                                                                \
    FOR##_mark = NULL;                                               \
  }                                                                  \
} while (0)
  
/* Run the data callback FOR and consume the current byte */
#define CALLBACK_DATA(FOR)						\
    CALLBACK_DATA_(FOR, p - FOR##_mark, p - data + 1)

/* Run the data callback FOR and don't consume the current byte */
#define CALLBACK_DATA_NOADVANCE(FOR)					\
    CALLBACK_DATA_(FOR, p - FOR##_mark, p - data)

/* Set the mark FOR; non-destructive if mark is already set */
#define MARK(FOR)							\
  do 									\
	{								\
	if (!FOR##_mark)						\
		{							\
		FOR##_mark = p;						\
		}							\
	} while (0)



// Don't allow the total size of the HTTP headers (including the status
// line) to exceed HTTP_MAX_HEADER_SIZE.  This check is here to protect
// embedders against denial-of-service attacks where the attacker feeds
// us a never-ending header that the embedder keeps buffering.
//
// This check is arguably the responsibility of embedders but we're doing
// it on the embedder's behalf because most won't bother and this way we
// make the web a little safer.  HTTP_MAX_HEADER_SIZE is still far bigger
// than any reasonable request or response so this should never affect
// day-to-day operation.
#define COUNT_HEADER_SIZE(V)						\
  do									\
	{								\
  conn->parser_nread += (V);						\
	if (UNLIKELY(conn->parser_nread > (HTTP_MAX_HEADER_SIZE))) {	\
	SET_ERRNO(HPE_HEADER_OVERFLOW);					\
	goto error;							\
	}								\
  } while (0)



#define PROXY_CONNECTION "proxy-connection"
#define CONNECTION "connection"
#define CONTENT_LENGTH "content-length"
#define TRANSFER_ENCODING "transfer-encoding"
#define UPGRADE "upgrade"
#define CHUNKED "chunked"
#define KEEP_ALIVE "keep-alive"
#define CLOSE "close"


static const char *method_strings[] =
  {
#define XX(num, name, string) #string,
  HTTP_METHOD_MAP(XX)
#undef XX
  };



// Tokens as defined by rfc 2616. Also lowercases them.
//        token       = 1*<any CHAR except CTLs or separators>
//     separators     = "(" | ")" | "<" | ">" | "@"
//                    | "," | ";" | ":" | "\" | <">
//                    | "/" | "[" | "]" | "?" | "="
//                    | "{" | "}" | SP | HT
//static const char tokens[256] ICACHE_RODATA_ATTR = {
static const char tokens[256] = {
//   0 nul    1 soh    2 stx    3 etx    4 eot    5 enq    6 ack    7 bel
        0,       0,       0,       0,       0,       0,       0,       0,
//   8 bs     9 ht    10 nl    11 vt    12 np    13 cr    14 so    15 si
        0,       0,       0,       0,       0,       0,       0,       0,
//  16 dle   17 dc1   18 dc2   19 dc3   20 dc4   21 nak   22 syn   23 etb
        0,       0,       0,       0,       0,       0,       0,       0,
//  24 can   25 em    26 sub   27 esc   28 fs    29 gs    30 rs    31 us
        0,       0,       0,       0,       0,       0,       0,       0,
//  32 sp    33  !    34  "    35  #    36  $    37  %    38  &    39  '
        0,      '!',      0,      '#',     '$',     '%',     '&',    '\'',
//  40  (    41  )    42  *    43  +    44  ,    45  -    46  .    47  / 
        0,       0,      '*',     '+',      0,      '-',     '.',      0,
//  48  0    49  1    50  2    51  3    52  4    53  5    54  6    55  7 
       '0',     '1',     '2',     '3',     '4',     '5',     '6',     '7',
//  56  8    57  9    58  :    59  ;    60  <    61  =    62  >    63  ? 
       '8',     '9',      0,       0,       0,       0,       0,       0,
//  64  @    65  A    66  B    67  C    68  D    69  E    70  F    71  G  
        0,      'a',     'b',     'c',     'd',     'e',     'f',     'g',
//  72  H    73  I    74  J    75  K    76  L    77  M    78  N    79  O 
       'h',     'i',     'j',     'k',     'l',     'm',     'n',     'o',
//  80  P    81  Q    82  R    83  S    84  T    85  U    86  V    87  W  
       'p',     'q',     'r',     's',     't',     'u',     'v',     'w',
//  88  X    89  Y    90  Z    91  [    92  \    93  ]    94  ^    95  _ 
       'x',     'y',     'z',      0,       0,       0,      '^',     '_',
//  96  `    97  a    98  b    99  c   100  d   101  e   102  f   103  g  
       '`',     'a',     'b',     'c',     'd',     'e',     'f',     'g',
// 104  h   105  i   106  j   107  k   108  l   109  m   110  n   111  o 
       'h',     'i',     'j',     'k',     'l',     'm',     'n',     'o',
// 112  p   113  q   114  r   115  s   116  t   117  u   118  v   119  w  
       'p',     'q',     'r',     's',     't',     'u',     'v',     'w',
// 120  x   121  y   122  z   123  {   124  |   125  }   126  ~   127 del 
       'x',     'y',     'z',      0,      '|',      0,      '~',       0 };


//static const int8_t unhex[256] =
static const int32_t unhex[256] ICACHE_RODATA_ATTR =
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
  , 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,-1,-1,-1,-1,-1,-1
  ,-1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
  };


#if HTTP_PARSER_STRICT
# define T(v) 0
#else
# define T(v) v
#endif

static const uint8_t normal_url_char[32] = {
/*   0 nul    1 soh    2 stx    3 etx    4 eot    5 enq    6 ack    7 bel  */
        0    |   0    |   0    |   0    |   0    |   0    |   0    |   0,
/*   8 bs     9 ht    10 nl    11 vt    12 np    13 cr    14 so    15 si   */
        0    | T(2)   |   0    |   0    | T(16)  |   0    |   0    |   0,
/*  16 dle   17 dc1   18 dc2   19 dc3   20 dc4   21 nak   22 syn   23 etb */
        0    |   0    |   0    |   0    |   0    |   0    |   0    |   0,
/*  24 can   25 em    26 sub   27 esc   28 fs    29 gs    30 rs    31 us  */
        0    |   0    |   0    |   0    |   0    |   0    |   0    |   0,
/*  32 sp    33  !    34  "    35  #    36  $    37  %    38  &    39  '  */
        0    |   2    |   4    |   0    |   16   |   32   |   64   |  128,
/*  40  (    41  )    42  *    43  +    44  ,    45  -    46  .    47  /  */
        1    |   2    |   4    |   8    |   16   |   32   |   64   |  128,
/*  48  0    49  1    50  2    51  3    52  4    53  5    54  6    55  7  */
        1    |   2    |   4    |   8    |   16   |   32   |   64   |  128,
/*  56  8    57  9    58  :    59  ;    60  <    61  =    62  >    63  ?  */
        1    |   2    |   4    |   8    |   16   |   32   |   64   |   0,
/*  64  @    65  A    66  B    67  C    68  D    69  E    70  F    71  G  */
        1    |   2    |   4    |   8    |   16   |   32   |   64   |  128,
/*  72  H    73  I    74  J    75  K    76  L    77  M    78  N    79  O  */
        1    |   2    |   4    |   8    |   16   |   32   |   64   |  128,
/*  80  P    81  Q    82  R    83  S    84  T    85  U    86  V    87  W  */
        1    |   2    |   4    |   8    |   16   |   32   |   64   |  128,
/*  88  X    89  Y    90  Z    91  [    92  \    93  ]    94  ^    95  _  */
        1    |   2    |   4    |   8    |   16   |   32   |   64   |  128,
/*  96  `    97  a    98  b    99  c   100  d   101  e   102  f   103  g  */
        1    |   2    |   4    |   8    |   16   |   32   |   64   |  128,
/* 104  h   105  i   106  j   107  k   108  l   109  m   110  n   111  o  */
        1    |   2    |   4    |   8    |   16   |   32   |   64   |  128,
/* 112  p   113  q   114  r   115  s   116  t   117  u   118  v   119  w  */
        1    |   2    |   4    |   8    |   16   |   32   |   64   |  128,
/* 120  x   121  y   122  z   123  {   124  |   125  }   126  ~   127 del */
        1    |   2    |   4    |   8    |   16   |   32   |   64   |   0, };
#undef T


// parser processing status (enum)
enum state
  { s_dead = 1			// #01 important that this is > 0

  // response		 	   #XX for debugging
  , s_start_req_or_res		// #02 before identifying if msg is request or response
  , s_res_or_resp_H		// #03 
  , s_req_HE_or_resp_H		// #04 Identified (H)xxx, may be H(E)AD or request response H(t)tp/1.1 200 OK

  // response	
  , s_start_res			// #05
  , s_res_H			// #06
  , s_res_HT			// #07
  , s_res_HTT			// #08
  , s_res_HTTP			// #09
  , s_res_first_http_major	// #10
  , s_res_http_major		// #11
  , s_res_first_http_minor	// #12
  , s_res_http_minor		// #13
  , s_res_first_status_code	// #14
  , s_res_status_code		// #15
  , s_res_status_start		// #16
  , s_res_status		// #17
  , s_res_line_almost_done	// #18

  //  request			   #XX for debugging
  , s_start_req			// #19 identified as request, because H is missing (H)ttp/1.1 200 OK
  , s_req_method		// #20
  , s_req_spaces_before_url	// #21
  , s_req_schema		// #22
  , s_req_schema_slash		// #23
  , s_req_schema_slash_slash	// #24
  , s_req_server_start		// #25
  , s_req_server		// #26
  , s_req_server_with_at	// #27
  , s_req_path			// #28
  , s_req_query_string_start	// #29
  , s_req_query_string		// #30
  , s_req_fragment_start	// #31
  , s_req_fragment		// #32
  , s_req_http_start		// #33
  , s_req_http_H		// #34
  , s_req_http_HT		// #35
  , s_req_http_HTT		// #36
  , s_req_http_HTTP		// #37
  , s_req_first_http_major	// #38
  , s_req_http_major		// #39
  , s_req_first_http_minor	// #40
  , s_req_http_minor		// #41
  , s_req_line_almost_done	// #42

  , s_header_field_start	// #43
  , s_header_field		// #44
  , s_header_value_discard_ws	// #45
  , s_header_value_discard_ws_almost_done	// #46
  , s_header_value_discard_lws	// #47
  , s_header_value_start	// #48
  , s_header_value		// #49
  , s_header_value_lws		// #50

  , s_header_almost_done	// #51

  , s_chunk_size_start		// #52
  , s_chunk_size		// #53
  , s_chunk_parameters		// #54
  , s_chunk_size_almost_done	// #55

  , s_headers_almost_done	// #56
  , s_headers_done		// #57

  /* Important: 's_headers_done' must be the last 'header' state. All
   * states beyond this must be 'body' states. It is used for overflow
   * checking. See the PARSING_HEADER() macro.
   */

  , s_chunk_data		// #58
  , s_chunk_data_almost_done	// #59
  , s_chunk_data_done		// #60

  , s_body_identity		// #61
  , s_body_identity_eof		// #62

  , s_message_done		// #63
  };

#define PARSING_HEADER(state) (state <= s_headers_done)


// current header status (enum)
enum header_states
  { h_general = 0			// #00
  , h_C					// #01
  , h_CO				// #02
  , h_CON				// #03

  , h_matching_connection		// #04
  , h_matching_proxy_connection		// #05
  , h_matching_content_length		// #06
  , h_matching_transfer_encoding	// #07
  , h_matching_upgrade			// #08

  , h_connection			// #09
  , h_content_length			// #10
  , h_transfer_encoding			// #11
  , h_upgrade				// #12

  , h_matching_transfer_encoding_chunked// #13
  , h_matching_connection_token_start	// #14
  , h_matching_connection_keep_alive	// #15
  , h_matching_connection_close		// #16
  , h_matching_connection_upgrade	// #17
  , h_matching_connection_token		// #18

  , h_transfer_encoding_chunked		// #19
  , h_connection_keep_alive		// #20
  , h_connection_close			// #21
  , h_connection_upgrade		// #22
  };



// host processing status (enum)
enum http_host_state
  {
    s_http_host_dead = 1		// #01
  , s_http_userinfo_start		// #02
  , s_http_userinfo			// #03
  , s_http_host_start			// #04
  , s_http_host_v6_start		// #05
  , s_http_host				// #06
  , s_http_host_v6			// #07
  , s_http_host_v6_end			// #08
  , s_http_host_v6_zone_start		// #09
  , s_http_host_v6_zone			// #10
  , s_http_host_port_start		// #11
  , s_http_host_port			// #12
};


/* Macros for character classes; depends on strict-mode  */
#define CR                  '\r'
#define LF                  '\n'
#define LOWER(c)            (unsigned char)(c | 0x20)
#define IS_ALPHA(c)         (LOWER(c) >= 'a' && LOWER(c) <= 'z')
#define IS_NUM(c)           ((c) >= '0' && (c) <= '9')
#define IS_ALPHANUM(c)      (IS_ALPHA(c) || IS_NUM(c))
#define IS_HEX(c)           (IS_NUM(c) || (LOWER(c) >= 'a' && LOWER(c) <= 'f'))
#define IS_MARK(c)          ((c) == '-' || (c) == '_' || (c) == '.' || \
  (c) == '!' || (c) == '~' || (c) == '*' || (c) == '\'' || (c) == '(' || \
  (c) == ')')
#define IS_USERINFO_CHAR(c) (IS_ALPHANUM(c) || IS_MARK(c) || (c) == '%' || \
  (c) == ';' || (c) == ':' || (c) == '&' || (c) == '=' || (c) == '+' || \
  (c) == '$' || (c) == ',')

#define STRICT_TOKEN(c)     (tokens[(unsigned char)c])

#if HTTP_PARSER_STRICT
// do more checks
#define TOKEN(c)            (tokens[(unsigned char)c])
#define IS_URL_CHAR(c)      (BIT_AT(normal_url_char, (unsigned char)c))
#define IS_HOST_CHAR(c)     (IS_ALPHANUM(c) || (c) == '.' || (c) == '-')
// do less checks
#else
#define TOKEN(c)            ((c == ' ') ? ' ' : tokens[(unsigned char)c])
#define IS_URL_CHAR(c)      (BIT_AT(normal_url_char, (unsigned char)c) || ((c) & 0x80))
#define IS_HOST_CHAR(c)     (IS_ALPHANUM(c) || (c) == '.' || (c) == '-' || (c) == '_')
#endif




// Verify that a char is a valid visible (printable) US-ASCII character or %x80-FF
#define IS_HEADER_CHAR(ch)	(ch == CR || ch == LF || ch == 9 || (ch > 31 && ch != 127))

#define start_state		(conn->parser_type == HTTP_REQUEST ? s_start_req : s_start_res)


#if HTTP_PARSER_STRICT
// do more checks
# define STRICT_CHECK(cond)                                          \
do {                                                                 \
  if (cond) {                                                        \
    SET_ERRNO(HPE_STRICT);                                           \
    goto error;                                                      \
  }                                                                  \
} while (0)
# define NEW_MESSAGE() (http_should_keep_alive(conn) ? start_state : s_dead)
#else
// do less checks
# define STRICT_CHECK(cond)
# define NEW_MESSAGE() start_state
#endif



// Map assigned http-error-numbers and end-user-description to table 'http_strerror_tab'
#define HTTP_STRERROR_GEN(n, x, s) { x,  s },
static struct
  {
  const unsigned int code;
  const char *description;
  } http_strerror_tab[] = {
  HTTP_ERRNO_MAP(HTTP_STRERROR_GEN)
};
#undef HTTP_STRERROR_GEN



int http_message_needs_eof(const WebIf_HTTPDConnSlotData_t *conn);








//##################################################################################################
//### FName: parse_url_char
//###  Desc: char processing func for url/uri parsing,
//###        This is designed to be shared by http_parser_execute() for URL validation,
//###        hence it has a state transition + byte-for-byte interface. In addition, it
//###        is meant to be embedded in http_parser_parse_url(), which does the dirty
//###        work of turning state transitions URL components for its API.
//###
//###        This function should only be invoked with non-space characters. It is
//###        assumed that the caller cares about (and can detect) the transition between
//###        URL and non-URL states by looking for these.
//###  Para: enum state s -> old parser state
//###        const char ch -> the char that should be processed
//###  Rets: static enum state -> new parser state
//##################################################################################################
static enum state ICACHE_FLASH_ATTR
parse_url_char(enum state s,
		const char ch)
{
  if (ch == ' ' || ch == '\r' || ch == '\n')
	{
	return s_dead;
	}

#if HTTP_PARSER_STRICT
  if (ch == '\t' || ch == '\f')
	{
	return s_dead;
	}
#endif

  switch (s) 
	{
//-------------------------------------------------------------------------------
	case s_req_spaces_before_url:

		// Proxied requests are followed by scheme of an absolute URI (alpha).
		// All methods except CONNECT are followed by '/' or '*'.
		if (ch == '/' || ch == '*')
			{
			return s_req_path;
			}

		// 1. char scheme = 1st ALPHA ,2nd ( ALPHA / DIGIT / "+" / "-" / "." )
		if (IS_ALPHA(ch))
			{
			return s_req_schema;
			}
	break;
//-------------------------------------------------------------------------------	
	case s_req_schema:
		// 2. char scheme = 1st ALPHA ,2nd ( ALPHA / DIGIT / "+" / "-" / "." )
		if (IS_ALPHANUM(ch)) //+-. not implemented
			{
			return s;
			}

		if (ch == ':')
			{
			return s_req_schema_slash;
			}
	break;
//-------------------------------------------------------------------------------
	case s_req_schema_slash:
		if (ch == '/')
			{
			return s_req_schema_slash_slash;
			}
	break;
//-------------------------------------------------------------------------------
	case s_req_schema_slash_slash:
	if (ch == '/')
		{
		return s_req_server_start;
		}
	break;
//-------------------------------------------------------------------------------
	case s_req_server_with_at:
		if (ch == '@')
			{
			return s_dead;
			}
//-------------------------------------------------------------------------------
	// FALLTHROUGH
	case s_req_server_start:
	case s_req_server:
		if (ch == '/')
			{
			return s_req_path;
			}

		if (ch == '?')
			{
			return s_req_query_string_start;
			}

		if (ch == '@')
			{
			return s_req_server_with_at;
			}

		if (IS_USERINFO_CHAR(ch) || ch == '[' || ch == ']')
			{
			return s_req_server;
			}
	break;
//-------------------------------------------------------------------------------
	case s_req_path:
		if (IS_URL_CHAR(ch))
			{
			return s;
			}

		switch (ch)
			{
			case '?':
			return s_req_query_string_start;

			case '#':
			return s_req_fragment_start;
			}

	break;
//-------------------------------------------------------------------------------
	case s_req_query_string_start:
	case s_req_query_string:
		if (IS_URL_CHAR(ch))
			{
			return s_req_query_string;
			}

		switch (ch)
			{
			case '?':
			// allow extra '?' in query string
			return s_req_query_string;

			case '#':
			return s_req_fragment_start;
			}

	break;
//-------------------------------------------------------------------------------
	case s_req_fragment_start:
		if (IS_URL_CHAR(ch))
			{
			return s_req_fragment;
			}

		switch (ch)
			{
			case '?':
			return s_req_fragment;

			case '#':
			return s;
			}

	break;
//-------------------------------------------------------------------------------
	case s_req_fragment:
		if (IS_URL_CHAR(ch))
			{
			return s;
			}

		switch (ch)
			{
			case '?':
			case '#':
			return s;
			}
	break;
//-------------------------------------------------------------------------------
	default:
	break;
//-------------------------------------------------------------------------------
  }

  // We should never fall out of the switch above unless there's an error
  return s_dead;
}



//##############################################################################
//### FName: http_parser_execute
//###  Desc:  
//###  Para: WebIf_HTTPDConnSlotData_t *conn ->	ptr to slot data
//###      : const http_parser_settings *settings -> ptr to parser settings
//###      : const char *data -> ptr to data to process
//###      : size_t len -> length of data to process
//###  Rets: -/-
//##############################################################################
size_t  ICACHE_FLASH_ATTR
http_parser_execute (WebIf_HTTPDConnSlotData_t *conn,
			const http_parser_settings_t *settings,
			const char *data,
			size_t len)
{
  char c, ch;
  int8_t unhex_val;
  const char *p = data;
  const char *header_field_mark = 0;
  const char *header_value_mark = 0;
  const char *url_mark = 0;
  const char *body_mark = 0;
  const char *status_mark = 0;

  enum state p_state = (enum state) conn->parser_state;

  const unsigned int lenient = conn->parser_lenient_http_headers;

  // We're in an error state. Don't bother doing anything
  if (HTTP_PARSER_ERRNO(conn) != HPE_OK)
	{
	return 0;
	}

  if (len == 0)
	{
	switch (CURRENT_STATE())
		{

		// Use of CALLBACK_NOTIFY() here would erroneously return 1 byte read if we got paused.
		case s_body_identity_eof:
 		CALLBACK_NOTIFY_NOADVANCE(message_complete);
		return 0;

		case s_dead:

      		case s_start_req_or_res:

		case s_start_res:

		case s_start_req:
			return 0;

		default:
		SET_ERRNO(HPE_INVALID_EOF_STATE);
		return 1;
		}
	}

  // continue header field ?
  if (CURRENT_STATE() == s_header_field)
	header_field_mark = data;

  // continue header value ?
  if (CURRENT_STATE() == s_header_value)
	header_value_mark = data;

  switch (CURRENT_STATE())
	{
	case s_req_path:

	case s_req_schema:

	case s_req_schema_slash:

	case s_req_schema_slash_slash:

	case s_req_server_start:

	case s_req_server:

	case s_req_server_with_at:

	case s_req_query_string_start:

	case s_req_query_string:

	case s_req_fragment_start:

	case s_req_fragment:
	url_mark = data;
	break;

	case s_res_status:
	status_mark = data;
	break;

	default:
	break;
	}
 	
  # if SCDED_DBG >= 4
  printf("HTTPD Slt:%d,StrtPrs>",
	conn->SlotNo);
  # endif

  for (p = data; p != data + len; p++)
	{
	ch = *p;

	if (PARSING_HEADER(CURRENT_STATE()))
		COUNT_HEADER_SIZE(1);

	reexecute:

 	# if SCDED_DBG >= 4
	printf("|P%d-H%d>",
		CURRENT_STATE(),
		conn->parser_header_state);
  	# endif

//----------------------------------------------------------------------------------------------------

	switch (CURRENT_STATE())
		{

        	// this state is used after a 'Connection: close' message
       		// the parser will error out if it reads another message
		case s_dead:
        	if (LIKELY(ch == CR || ch == LF))
          		break;

        	SET_ERRNO(HPE_CLOSED_CONNECTION);
        	goto error;

//----------------------------------------------------------------------------------------------------

		// identify if this is an request or an response
		case s_start_req_or_res:
		{
		if (ch == CR || ch == LF)
			break;

		conn->parser_flags = 0;

		conn->parser_content_length = ULLONG_MAX;

		if (ch == 'H')
			{
			UPDATE_STATE(s_res_or_resp_H);

			CALLBACK_NOTIFY(message_begin);
			}
		else
			{

			conn->parser_type = HTTP_REQUEST;

			UPDATE_STATE(s_start_req);

			REEXECUTE();
			}
		break;
		}

//----------------------------------------------------------------------------------------------------

		case s_res_or_resp_H:
		if (ch == 'T')
			{
			conn->parser_type = HTTP_RESPONSE;

			UPDATE_STATE(s_res_HT);
			}
		else
			{
			if (UNLIKELY(ch != 'E'))
				{
				SET_ERRNO(HPE_INVALID_CONSTANT);
				goto error;
				}

			conn->parser_type = HTTP_REQUEST;
			conn->parser_method = HTTP_HEAD;
			conn->parser_index = 2;
			UPDATE_STATE(s_req_method);
			}

        	break;

//----------------------------------------------------------------------------------------------------

		case s_start_res:
		{
		conn->parser_flags = 0;
		conn->parser_content_length = ULLONG_MAX;

		switch (ch)
			{

			case 'H':
			UPDATE_STATE(s_res_H);
			break;

			case CR:

			case LF:
			break;

			default:
			SET_ERRNO(HPE_INVALID_CONSTANT);
			goto error;

			}

		CALLBACK_NOTIFY(message_begin);

		break;
		}
//----------------------------------------------------------------------------------------------------

		case s_res_H:
		STRICT_CHECK(ch != 'T');
		UPDATE_STATE(s_res_HT);
		break;

//----------------------------------------------------------------------------------------------------

		case s_res_HT:
		STRICT_CHECK(ch != 'T');
		UPDATE_STATE(s_res_HTT);
		break;

//----------------------------------------------------------------------------------------------------

		case s_res_HTT:
		STRICT_CHECK(ch != 'P');
		UPDATE_STATE(s_res_HTTP);
		break;

//----------------------------------------------------------------------------------------------------

		case s_res_HTTP:
		STRICT_CHECK(ch != '/');
		UPDATE_STATE(s_res_first_http_major);
		break;

//----------------------------------------------------------------------------------------------------

		case s_res_first_http_major:
		if (UNLIKELY(ch < '0' || ch > '9'))
			{
			SET_ERRNO(HPE_INVALID_VERSION);
			goto error;
			}

		conn->parser_http_major = ch - '0';
		UPDATE_STATE(s_res_http_major);

		break;

//----------------------------------------------------------------------------------------------------

		// major HTTP version or dot
		case s_res_http_major:
			{
			if (ch == '.')
				{
				UPDATE_STATE(s_res_first_http_minor);
				break;
				}

			if (!IS_NUM(ch))
				{
				SET_ERRNO(HPE_INVALID_VERSION);
				goto error;
				}

		conn->parser_http_major *= 10;
		conn->parser_http_major += ch - '0';

		if (UNLIKELY(conn->parser_http_major > 999))
			{
			SET_ERRNO(HPE_INVALID_VERSION);
			goto error;
			}

		break;
		}

//----------------------------------------------------------------------------------------------------

		// first digit of minor HTTP version
		case s_res_first_http_minor:
		if (UNLIKELY(!IS_NUM(ch)))
			{
			SET_ERRNO(HPE_INVALID_VERSION);
			goto error;
			}

		conn->parser_http_minor = ch - '0';

		UPDATE_STATE(s_res_http_minor);

		break;

//----------------------------------------------------------------------------------------------------

		// minor HTTP version or end of request line
		case s_res_http_minor:
		{ //!
		if (ch == ' ')
			{
			UPDATE_STATE(s_res_first_status_code);
			break;
			}

		if (UNLIKELY(!IS_NUM(ch)))
			{
			SET_ERRNO(HPE_INVALID_VERSION);
			goto error;
			}

		conn->parser_http_minor *= 10;
		conn->parser_http_minor += ch - '0';
	
		if (UNLIKELY(conn->parser_http_minor > 999))
			{
			SET_ERRNO(HPE_INVALID_VERSION);
			goto error;
			}

		break;
		}

//----------------------------------------------------------------------------------------------------

		case s_res_first_status_code:
		{ //!
		if (!IS_NUM(ch))
			{
			if (ch == ' ')
				{
				break;
				}

			SET_ERRNO(HPE_INVALID_STATUS);

			goto error;
			}

		conn->parser_status_code = ch - '0';

		UPDATE_STATE(s_res_status_code);

		break;
		}

//----------------------------------------------------------------------------------------------------

		case s_res_status_code:
		{ //!
		if (!IS_NUM(ch))
			{
			switch (ch)
				{

				case ' ':
				UPDATE_STATE(s_res_status_start);
				break;

				case CR:
				UPDATE_STATE(s_res_line_almost_done);
				break;

				case LF:
				UPDATE_STATE(s_header_field_start);
				break;

				default:
				SET_ERRNO(HPE_INVALID_STATUS);
				goto error;

				}
			break;
			}

		conn->parser_status_code *= 10;
		conn->parser_status_code += ch - '0';

		if (UNLIKELY(conn->parser_status_code > 999))
			{
			SET_ERRNO(HPE_INVALID_STATUS);

			goto error;
			}

		break;
		}

//----------------------------------------------------------------------------------------------------

		case s_res_status_start:
		{ //!
		if (ch == CR)
			{
			UPDATE_STATE(s_res_line_almost_done);
			break;
			}

		if (ch == LF)
			{
			UPDATE_STATE(s_header_field_start);
			break;
			}

		MARK(status);

		UPDATE_STATE(s_res_status);

		conn->parser_index = 0;

 		break;
		}
//----------------------------------------------------------------------------------------------------

		case s_res_status:
		if (ch == CR)
			{
			UPDATE_STATE(s_res_line_almost_done);

			CALLBACK_DATA(status);

			break;
			}

		if (ch == LF)
			{
			UPDATE_STATE(s_header_field_start);

			CALLBACK_DATA(status);

			break;
			}

		break;

//----------------------------------------------------------------------------------------------------

		case s_res_line_almost_done:
		STRICT_CHECK(ch != LF);
		UPDATE_STATE(s_header_field_start);
		break;

//----------------------------------------------------------------------------------------------------

		case s_start_req:
		{//!
		if (ch == CR || ch == LF)
			{
			break;
			}

		conn->parser_flags = 0;
		conn->parser_content_length = ULLONG_MAX;

		if (UNLIKELY(!IS_ALPHA(ch)))
			{
			SET_ERRNO(HPE_INVALID_METHOD);
			goto error;
			}

		conn->parser_method = (enum http_method) 0;
		conn->parser_index = 1;

		switch (ch)
			{
			// ACL
			case 'A':
			conn->parser_method = HTTP_ACL;
			break;

			// BIND
			case 'B':
			conn->parser_method = HTTP_BIND;
			break;

			// CONNECT -> COPY | CHECKOUT 
			case 'C':
			conn->parser_method = HTTP_CONNECT;
			break;

			// DELETE
			case 'D':
			conn->parser_method = HTTP_DELETE;
			break;

			// GET
			case 'G':
			conn->parser_method = HTTP_GET;
			break;

			// HEAD
			case 'H':
			conn->parser_method = HTTP_HEAD;
			break;

			// LOCK -> LINK
			case 'L':
			conn->parser_method = HTTP_LOCK;
			break;

			// MKCOL -> MOVE | MKACTIVITY | MERGE | M-SEARCH | MKCALENDAR 
			case 'M':
			conn->parser_method = HTTP_MKCOL;
			break;

			// NOTIFY
			case 'N':
			conn->parser_method = HTTP_NOTIFY;
			break;

			// OPTIONS
			case 'O':
			conn->parser_method = HTTP_OPTIONS;
			break;

			// POST -> PROPFIND | PROPPATCH | PUT | PATCH | PURGE
			case 'P':
			conn->parser_method = HTTP_POST;
			break;

			// REPORT -> REBIND
			case 'R':
			conn->parser_method = HTTP_REPORT;
			break;

			// SUBSCRIBE -> SEARCH | SET
			case 'S':
			conn->parser_method = HTTP_SUBSCRIBE;
			break;

			// TRACE
			case 'T':
			conn->parser_method = HTTP_TRACE;
			break;

			// UNLOCK -> UNSUBSCRIBE | UNBIND | UNLINK
			case 'U':
			conn->parser_method = HTTP_UNLOCK;
			break;

			default:
			SET_ERRNO(HPE_INVALID_METHOD);
			goto error;

			}

		UPDATE_STATE(s_req_method);

		CALLBACK_NOTIFY(message_begin);

		break;
		}

//----------------------------------------------------------------------------------------------------

		case s_req_method:
		{
		const char *matcher;

		if (UNLIKELY(ch == '\0'))
			{
			SET_ERRNO(HPE_INVALID_METHOD);
			goto error;
			}

		matcher = method_strings[conn->parser_method];

		if (ch == ' ' && matcher[conn->parser_index] == '\0')
			{
			UPDATE_STATE(s_req_spaces_before_url);
			}

		else if (ch == matcher[conn->parser_index])
			{
			; /* nada */
			}

		// CONNECT -> CHECKOUT | COPY
		else if (conn->parser_method == HTTP_CONNECT)
			{
			if (conn->parser_index == 1 && ch == 'H')
				{
				conn->parser_method = HTTP_CHECKOUT;
				}

			else if (conn->parser_index == 2  && ch == 'P')
				{
				conn->parser_method = HTTP_COPY;
				}
			else
				{
				SET_ERRNO(HPE_INVALID_METHOD);
				goto error;
				}
			}

		// MKCOL -> MOVEMERGE | MSEARCH | MKACTIVITY | MKCALENDAR
		else if (conn->parser_method == HTTP_MKCOL)
			{
			if (conn->parser_index == 1 && ch == 'O')
				{
				conn->parser_method = HTTP_MOVE;
				}

			else if (conn->parser_index == 1 && ch == 'E')
				{
				conn->parser_method = HTTP_MERGE;
				}

			else if (conn->parser_index == 1 && ch == '-')
				{
				conn->parser_method = HTTP_MSEARCH;
				}

			else if (conn->parser_index == 2 && ch == 'A')
				{
				conn->parser_method = HTTP_MKACTIVITY;
				}

			else if (conn->parser_index == 3 && ch == 'A')
				{
				conn->parser_method = HTTP_MKCALENDAR;
				}

			else
				{
				SET_ERRNO(HPE_INVALID_METHOD);
				goto error;
				}
			}

		// SUBSCRIBE -> SEARCH | SET
		else if (conn->parser_method == HTTP_SUBSCRIBE)
			{
			if (conn->parser_index == 1 && ch == 'E')
				{
				conn->parser_method = HTTP_SEARCH; /* or HTTP_SET */
				}
			else
				{
				SET_ERRNO(HPE_INVALID_METHOD);
				goto error;
				}
			}

		// REPORT -> REBIND
		else if (conn->parser_method == HTTP_REPORT)
			{
			if (conn->parser_index == 2 && ch == 'B')
				{
				conn->parser_method = HTTP_REBIND;
				}
			else
				{
				SET_ERRNO(HPE_INVALID_METHOD);
				goto error;
				}
			}

		// POST -> PROPFIND | PROPPATCH | PUT | PURGE | PATCH
		else if (conn->parser_index == 1)
			{
			if (conn->parser_method == HTTP_POST)
				{
				if (ch == 'R')
					{
					conn->parser_method = HTTP_PROPFIND; /* or HTTP_PROPPATCH */
					}

				else if (ch == 'U')
					{
					conn->parser_method = HTTP_PUT; /* or HTTP_PURGE */
					}

				else if (ch == 'A')
					{
					conn->parser_method = HTTP_PATCH;
					}
				else
					{
					SET_ERRNO(HPE_INVALID_METHOD);
					goto error;
					}
				}

			// LOCK -> LINK
			else if (conn->parser_method == HTTP_LOCK)
				{
				if (ch == 'I')
					{
					conn->parser_method = HTTP_LINK;
					}
				else
					{
					SET_ERRNO(HPE_INVALID_METHOD);
					goto error;
					}
				}

			}

		// PUT -> PURGE
		else if (conn->parser_index == 2)
			{
			if (conn->parser_method == HTTP_PUT)
				{
				if (ch == 'R')
					{
					conn->parser_method = HTTP_PURGE;
					}

				else
					{
					SET_ERRNO(HPE_INVALID_METHOD);
					goto error;
					}
				}

			// UNLOCK -> UNSUBSCRIBE | UNBIND
			else if (conn->parser_method == HTTP_UNLOCK)
				{
				if (ch == 'S')
					{
					conn->parser_method = HTTP_UNSUBSCRIBE;
					}

				else if(ch == 'B')
					{
					conn->parser_method = HTTP_UNBIND;
					}

				else
					{
					SET_ERRNO(HPE_INVALID_METHOD);
					goto error;
					}
				}

			// SEARCH -> SET
			else if (conn->parser_method == HTTP_SEARCH)
				{
				if (ch == 'T')
					{
					conn->parser_method = HTTP_SET;
					}

				else
					{
					SET_ERRNO(HPE_INVALID_METHOD);
					goto error;
					}
				}

			else
				{
				SET_ERRNO(HPE_INVALID_METHOD);
				goto error;
				}
			}

		else if (conn->parser_index == 4 && conn->parser_method == HTTP_PROPFIND && ch == 'P')
			{
			conn->parser_method = HTTP_PROPPATCH;
			}

		else if (conn->parser_index == 3 && conn->parser_method == HTTP_UNLOCK && ch == 'I')
			{
			conn->parser_method = HTTP_UNLINK;
			}
		else
			{
			SET_ERRNO(HPE_INVALID_METHOD);
			goto error;
			}

		++conn->parser_index;

		break;
		}

//----------------------------------------------------------------------------------------------------

		case s_req_spaces_before_url:
		{//!
		if (ch == ' ')
			{
			break;
			}

		MARK(url);

		if (conn->parser_method == HTTP_CONNECT)
			{
			UPDATE_STATE(s_req_server_start);
			}

		UPDATE_STATE(parse_url_char(CURRENT_STATE(), ch));

		if (UNLIKELY(CURRENT_STATE() == s_dead))
			{
			SET_ERRNO(HPE_INVALID_URL);

			goto error;
			}

		break;
		}

//----------------------------------------------------------------------------------------------------

		case s_req_schema:

//----------------------------------------------------------------------------------------------------

		case s_req_schema_slash:

//----------------------------------------------------------------------------------------------------

		case s_req_schema_slash_slash:

//----------------------------------------------------------------------------------------------------

		case s_req_server_start:
		{//!
		switch (ch)
			{

			/* No whitespace allowed here */
			case ' ':

			case CR:

			case LF:
			SET_ERRNO(HPE_INVALID_URL);
			goto error;

			default:
			UPDATE_STATE(parse_url_char(CURRENT_STATE(), ch));

			if (UNLIKELY(CURRENT_STATE() == s_dead))
				{
				SET_ERRNO(HPE_INVALID_URL);
				goto error;
				}
			}

		break;
		}

//----------------------------------------------------------------------------------------------------

		case s_req_server:

//----------------------------------------------------------------------------------------------------

		case s_req_server_with_at:

//----------------------------------------------------------------------------------------------------

		case s_req_path:

//----------------------------------------------------------------------------------------------------

		case s_req_query_string_start:

//----------------------------------------------------------------------------------------------------

		case s_req_query_string:

//----------------------------------------------------------------------------------------------------

		case s_req_fragment_start:

//----------------------------------------------------------------------------------------------------

		case s_req_fragment:
		{//!
		switch (ch)
			{

			case ' ':
			UPDATE_STATE(s_req_http_start);
			CALLBACK_DATA(url);
			break;

			case CR:

			case LF:
			conn->parser_http_major = 0;
			conn->parser_http_minor = 9;

			UPDATE_STATE((ch == CR) ?
				s_req_line_almost_done :
				s_header_field_start);

			CALLBACK_DATA(url);
			break;

			default:
			UPDATE_STATE(parse_url_char(CURRENT_STATE(), ch));
			if (UNLIKELY(CURRENT_STATE() == s_dead))
				{
				SET_ERRNO(HPE_INVALID_URL);
				goto error;
				}
			}
		break;
		}

//----------------------------------------------------------------------------------------------------
//cont
      case s_req_http_start:
        switch (ch) {
          case 'H':
            UPDATE_STATE(s_req_http_H);
            break;
          case ' ':
            break;
          default:
            SET_ERRNO(HPE_INVALID_CONSTANT);
            goto error;
        }
        break;

      case s_req_http_H:
        STRICT_CHECK(ch != 'T');
        UPDATE_STATE(s_req_http_HT);
        break;

      case s_req_http_HT:
        STRICT_CHECK(ch != 'T');
        UPDATE_STATE(s_req_http_HTT);
        break;

      case s_req_http_HTT:
        STRICT_CHECK(ch != 'P');
        UPDATE_STATE(s_req_http_HTTP);
        break;

      case s_req_http_HTTP:
        STRICT_CHECK(ch != '/');
        UPDATE_STATE(s_req_first_http_major);
        break;

      /* first digit of major HTTP version */
      case s_req_first_http_major:
        if (UNLIKELY(ch < '1' || ch > '9')) {
          SET_ERRNO(HPE_INVALID_VERSION);
          goto error;
        }

        conn->parser_http_major = ch - '0';
        UPDATE_STATE(s_req_http_major);
        break;

      /* major HTTP version or dot */
      case s_req_http_major:
      {
        if (ch == '.') {
          UPDATE_STATE(s_req_first_http_minor);
          break;
        }

        if (UNLIKELY(!IS_NUM(ch))) {
          SET_ERRNO(HPE_INVALID_VERSION);
          goto error;
        }

        conn->parser_http_major *= 10;
        conn->parser_http_major += ch - '0';

        if (UNLIKELY(conn->parser_http_major > 999)) {
          SET_ERRNO(HPE_INVALID_VERSION);
          goto error;
        }

        break;
      }

      /* first digit of minor HTTP version */
      case s_req_first_http_minor:
        if (UNLIKELY(!IS_NUM(ch))) {
          SET_ERRNO(HPE_INVALID_VERSION);
          goto error;
        }

        conn->parser_http_minor = ch - '0';
        UPDATE_STATE(s_req_http_minor);
        break;

      /* minor HTTP version or end of request line */
      case s_req_http_minor:
      {
        if (ch == CR) {
          UPDATE_STATE(s_req_line_almost_done);
          break;
        }

        if (ch == LF) {
          UPDATE_STATE(s_header_field_start);
          break;
        }

        /* XXX allow spaces after digit? */

        if (UNLIKELY(!IS_NUM(ch))) {
          SET_ERRNO(HPE_INVALID_VERSION);
          goto error;
        }

        conn->parser_http_minor *= 10;
        conn->parser_http_minor += ch - '0';

        if (UNLIKELY(conn->parser_http_minor > 999)) {
          SET_ERRNO(HPE_INVALID_VERSION);
          goto error;
        }

        break;
      }

      /* end of request line */
      case s_req_line_almost_done:
      {
        if (UNLIKELY(ch != LF)) {
          SET_ERRNO(HPE_LF_EXPECTED);
          goto error;
        }

        UPDATE_STATE(s_header_field_start);
        break;
      }

	case s_header_field_start:
		{
		if (ch == CR)
			{
			UPDATE_STATE(s_headers_almost_done);
			break;
			}

		if (ch == LF)
			{
			// they might be just sending \n instead of \r\n so this would be
			// the second \n to denote the end of headers
			UPDATE_STATE(s_headers_almost_done);
			REEXECUTE();
			}

		c = TOKEN(ch);

		if (UNLIKELY(!c))
			{
			SET_ERRNO(HPE_INVALID_HEADER_TOKEN);
			goto error;
			}

		MARK(header_field);

		conn->parser_index = 0;
		UPDATE_STATE(s_header_field);

		switch (c)
			{
			case 'c':
				conn->parser_header_state = h_C;
				break;

			case 'p':
				conn->parser_header_state = h_matching_proxy_connection;
				break;

			case 't':
				conn->parser_header_state = h_matching_transfer_encoding;
				break;

			case 'u':
				conn->parser_header_state = h_matching_upgrade;
				break;

			default:
				 conn->parser_header_state = h_general;
			break;
			}
		break;
		}

      case s_header_field:
      {
        const char* start = p;
        for (; p != data + len; p++) {
          ch = *p;
          c = TOKEN(ch);

          if (!c)
            break;

          switch (conn->parser_header_state) {
            case h_general:
              break;

            case h_C:
              conn->parser_index++;
              conn->parser_header_state = (c == 'o' ? h_CO : h_general);
              break;

            case h_CO:
              conn->parser_index++;
              conn->parser_header_state = (c == 'n' ? h_CON : h_general);
              break;

            case h_CON:
              conn->parser_index++;
              switch (c) {
                case 'n':
                  conn->parser_header_state = h_matching_connection;
                  break;
                case 't':
                  conn->parser_header_state = h_matching_content_length;
                  break;
                default:
                  conn->parser_header_state = h_general;
                  break;
              }
              break;

            /* connection */

            case h_matching_connection:
              conn->parser_index++;
              if (conn->parser_index > sizeof(CONNECTION)-1
                  || c != CONNECTION[conn->parser_index]) {
                conn->parser_header_state = h_general;
              } else if (conn->parser_index == sizeof(CONNECTION)-2) {
                conn->parser_header_state = h_connection;
              }
              break;

            /* proxy-connection */

            case h_matching_proxy_connection:
              conn->parser_index++;
              if (conn->parser_index > sizeof(PROXY_CONNECTION)-1
                  || c != PROXY_CONNECTION[conn->parser_index]) {
                conn->parser_header_state = h_general;
              } else if (conn->parser_index == sizeof(PROXY_CONNECTION)-2) {
                conn->parser_header_state = h_connection;
              }
              break;

            /* content-length */

            case h_matching_content_length:
              conn->parser_index++;
              if (conn->parser_index > sizeof(CONTENT_LENGTH)-1
                  || c != CONTENT_LENGTH[conn->parser_index]) {
                conn->parser_header_state = h_general;
              } else if (conn->parser_index == sizeof(CONTENT_LENGTH)-2) {
                if (conn->parser_flags & F_CONTENTLENGTH) {
                  SET_ERRNO(HPE_UNEXPECTED_CONTENT_LENGTH);
                  goto error;
                }
                conn->parser_header_state = h_content_length;
                conn->parser_flags |= F_CONTENTLENGTH;
              }
              break;

            /* transfer-encoding */

            case h_matching_transfer_encoding:
              conn->parser_index++;
              if (conn->parser_index > sizeof(TRANSFER_ENCODING)-1
                  || c != TRANSFER_ENCODING[conn->parser_index]) {
                conn->parser_header_state = h_general;
              } else if (conn->parser_index == sizeof(TRANSFER_ENCODING)-2) {
                conn->parser_header_state = h_transfer_encoding;
              }
              break;

            /* upgrade */

            case h_matching_upgrade:
              conn->parser_index++;
              if (conn->parser_index > sizeof(UPGRADE)-1
                  || c != UPGRADE[conn->parser_index]) {
                conn->parser_header_state = h_general;
              } else if (conn->parser_index == sizeof(UPGRADE)-2) {
                conn->parser_header_state = h_upgrade;
              }
              break;

            case h_connection:
            case h_content_length:
            case h_transfer_encoding:
            case h_upgrade:
              if (ch != ' ') conn->parser_header_state = h_general;
              break;

            default:
              assert(0 && "Unknown header_state");
              break;
          }
        }

        COUNT_HEADER_SIZE(p - start);

        if (p == data + len) {
          --p;
          break;
        }

        if (ch == ':') {
          UPDATE_STATE(s_header_value_discard_ws);
          CALLBACK_DATA(header_field);
          break;
        }

        SET_ERRNO(HPE_INVALID_HEADER_TOKEN);
        goto error;
      }

      case s_header_value_discard_ws:
        if (ch == ' ' || ch == '\t') break;

        if (ch == CR) {
          UPDATE_STATE(s_header_value_discard_ws_almost_done);
          break;
        }

        if (ch == LF) {
          UPDATE_STATE(s_header_value_discard_lws);
          break;
        }

        /* FALLTHROUGH */

	case s_header_value_start:	// #48
	{
	MARK(header_value);

	UPDATE_STATE(s_header_value);
	conn->parser_index = 0;

	c = LOWER(ch);

        switch (conn->parser_header_state)
	{
//--------------------------------------------------------------------------------------------------
	case h_upgrade:
		conn->parser_flags |= F_UPGRADE;
		conn->parser_header_state = h_general;
	break;
//--------------------------------------------------------------------------------------------------
	case h_transfer_encoding:
	// looking for 'Transfer-Encoding: chunked'
		if ('c' == c)
			{
			conn->parser_header_state = h_matching_transfer_encoding_chunked;
			}
		else
			{
			conn->parser_header_state = h_general;
			}
	break;
//--------------------------------------------------------------------------------------------------
	case h_content_length:
		if (UNLIKELY(!IS_NUM(ch)))
			{
			SET_ERRNO(HPE_INVALID_CONTENT_LENGTH);
			goto error;
			}

		conn->parser_content_length = ch - '0';
	break;
//--------------------------------------------------------------------------------------------------
	case h_connection:
		// looking for 'Connection: keep-alive'
		if (c == 'k')
			{
			conn->parser_header_state = h_matching_connection_keep_alive;
			}

		// looking for 'Connection: close'
		else if (c == 'c')
			{
			conn->parser_header_state = h_matching_connection_close;
			}

		else if (c == 'u')
			{
			conn->parser_header_state = h_matching_connection_upgrade;
			}
		else
			{
			conn->parser_header_state = h_matching_connection_token;
			}

	break;
//--------------------------------------------------------------------------------------------------
	// Multi-value `Connection` header
	case h_matching_connection_token_start:
	break;
//--------------------------------------------------------------------------------------------------
	default:
		conn->parser_header_state = h_general;
	break;
//--------------------------------------------------------------------------------------------------
	}
	break;
      }

      case s_header_value:	// #49
      {
        const char* start = p;
        enum header_states h_state = (enum header_states) conn->parser_header_state;
        for (; p != data + len; p++) {
          ch = *p;
          if (ch == CR) {
            UPDATE_STATE(s_header_almost_done);
            conn->parser_header_state = h_state;
            CALLBACK_DATA(header_value);
            break;
          }

          if (ch == LF) {
            UPDATE_STATE(s_header_almost_done);
            COUNT_HEADER_SIZE(p - start);
            conn->parser_header_state = h_state;
            CALLBACK_DATA_NOADVANCE(header_value);
            REEXECUTE();
          }

          if (!lenient && !IS_HEADER_CHAR(ch)) {
            SET_ERRNO(HPE_INVALID_HEADER_TOKEN);
            goto error;
          }

          c = LOWER(ch);

	# if SCDED_DBG >= 4
	printf("!P%d-H%d-X%d>",
		CURRENT_STATE(),
		conn->parser_header_state,
		h_state);
  	# endif


          switch (h_state) {
            case h_general:
            {
              const char* p_cr;
              const char* p_lf;
              size_t limit = data + len - p;

              limit = MIN(limit, HTTP_MAX_HEADER_SIZE);

              p_cr = (const char*) memchr(p, CR, limit);
              p_lf = (const char*) memchr(p, LF, limit);
              if (p_cr != NULL) {
                if (p_lf != NULL && p_cr >= p_lf)
                  p = p_lf;
                else
                  p = p_cr;
              } else if (UNLIKELY(p_lf != NULL)) {
                p = p_lf;
              } else {
                p = data + len;
              }
              --p;

              break;
            }

            case h_connection:
            case h_transfer_encoding:
              assert(0 && "Shouldn't get here.");
              break;
//--------------------------------------------------------------------------------------------------
	// are we in content_length value? -> parse value char by char till ' '
	case h_content_length:
		{
		uint64_t t;

		if (ch == ' ') break;

		if (UNLIKELY(!IS_NUM(ch)))
			{
			SET_ERRNO(HPE_INVALID_CONTENT_LENGTH);
			conn->parser_header_state = h_state;
			goto error;
			}

		t = conn->parser_content_length;
		t *= 10;
		t += ch - '0';

		// Overflow? Test against a conservative limit for simplicity
		if (UNLIKELY((ULLONG_MAX - 10) / 10 < conn->parser_content_length))
			{
			SET_ERRNO(HPE_INVALID_CONTENT_LENGTH);
			conn->parser_header_state = h_state;
			goto error;
			}

		conn->parser_content_length = t;

//printf("|!!!Content length extracted:%llu>",
//	conn->parser_content_length);
		break;
		}
//--------------------------------------------------------------------------------------------------

            /* Transfer-Encoding: chunked */
            case h_matching_transfer_encoding_chunked:
              conn->parser_index++;
              if (conn->parser_index > sizeof(CHUNKED)-1
                  || c != CHUNKED[conn->parser_index]) {
                h_state = h_general;
              } else if (conn->parser_index == sizeof(CHUNKED)-2) {
                h_state = h_transfer_encoding_chunked;
              }
              break;

            case h_matching_connection_token_start:
              /* looking for 'Connection: keep-alive' */
              if (c == 'k') {
                h_state = h_matching_connection_keep_alive;
              /* looking for 'Connection: close' */
              } else if (c == 'c') {
                h_state = h_matching_connection_close;
              } else if (c == 'u') {
                h_state = h_matching_connection_upgrade;
              } else if (STRICT_TOKEN(c)) {
                h_state = h_matching_connection_token;
              } else if (c == ' ' || c == '\t') {
                /* Skip lws */
              } else {
                h_state = h_general;
              }
              break;

            /* looking for 'Connection: keep-alive' */
            case h_matching_connection_keep_alive:
              conn->parser_index++;
              if (conn->parser_index > sizeof(KEEP_ALIVE)-1
                  || c != KEEP_ALIVE[conn->parser_index]) {
                h_state = h_matching_connection_token;
              } else if (conn->parser_index == sizeof(KEEP_ALIVE)-2) {
                h_state = h_connection_keep_alive;
              }
              break;

            /* looking for 'Connection: close' */
            case h_matching_connection_close:
              conn->parser_index++;
              if (conn->parser_index > sizeof(CLOSE)-1 || c != CLOSE[conn->parser_index]) {
                h_state = h_matching_connection_token;
              } else if (conn->parser_index == sizeof(CLOSE)-2) {
                h_state = h_connection_close;
              }
              break;

            /* looking for 'Connection: upgrade' */
            case h_matching_connection_upgrade:
              conn->parser_index++;
              if (conn->parser_index > sizeof(UPGRADE) - 1 ||
                  c != UPGRADE[conn->parser_index]) {
                h_state = h_matching_connection_token;
              } else if (conn->parser_index == sizeof(UPGRADE)-2) {
                h_state = h_connection_upgrade;
              }
              break;

            case h_matching_connection_token:
              if (ch == ',') {
                h_state = h_matching_connection_token_start;
                conn->parser_index = 0;
              }
              break;

            case h_transfer_encoding_chunked:
              if (ch != ' ') h_state = h_general;
              break;

            case h_connection_keep_alive:
            case h_connection_close:
            case h_connection_upgrade:
              if (ch == ',') {
                if (h_state == h_connection_keep_alive) {
                  conn->parser_flags |= F_CONNECTION_KEEP_ALIVE;
                } else if (h_state == h_connection_close) {
                  conn->parser_flags |= F_CONNECTION_CLOSE;
                } else if (h_state == h_connection_upgrade) {
                  conn->parser_flags |= F_CONNECTION_UPGRADE;
                }
                h_state = h_matching_connection_token_start;
                conn->parser_index = 0;
              } else if (ch != ' ') {
                h_state = h_matching_connection_token;
              }
              break;

            default:
              UPDATE_STATE(s_header_value);
              h_state = h_general;
              break;
          }
        }
        conn->parser_header_state = h_state;

        COUNT_HEADER_SIZE(p - start);

        if (p == data + len)
          --p;
        break;
      }
//----------------------------------------------------------------------------------------------------
      case s_header_almost_done:	// #51
      {
        if (UNLIKELY(ch != LF)) {
          SET_ERRNO(HPE_LF_EXPECTED);
          goto error;
        }

        UPDATE_STATE(s_header_value_lws);
        break;
      }
//----------------------------------------------------------------------------------------------------
      case s_header_value_lws:	// #50
      {
        if (ch == ' ' || ch == '\t') {
          UPDATE_STATE(s_header_value_start);
          REEXECUTE();
        }

        /* finished the header */
        switch (conn->parser_header_state) {
          case h_connection_keep_alive:
            conn->parser_flags |= F_CONNECTION_KEEP_ALIVE;
            break;
          case h_connection_close:
            conn->parser_flags |= F_CONNECTION_CLOSE;
            break;
          case h_transfer_encoding_chunked:
            conn->parser_flags |= F_CHUNKED;
            break;
          case h_connection_upgrade:
            conn->parser_flags |= F_CONNECTION_UPGRADE;
            break;
          default:
            break;
        }

        UPDATE_STATE(s_header_field_start);
        REEXECUTE();
      }
//----------------------------------------------------------------------------------------------------
      case s_header_value_discard_ws_almost_done:
      {
        STRICT_CHECK(ch != LF);
        UPDATE_STATE(s_header_value_discard_lws);
        break;
      }
//----------------------------------------------------------------------------------------------------
      case s_header_value_discard_lws:	// #47
      {
        if (ch == ' ' || ch == '\t') {
          UPDATE_STATE(s_header_value_discard_ws);
          break;
        } else {
          switch (conn->parser_header_state) {
            case h_connection_keep_alive:
              conn->parser_flags |= F_CONNECTION_KEEP_ALIVE;
              break;
            case h_connection_close:
              conn->parser_flags |= F_CONNECTION_CLOSE;
              break;
            case h_connection_upgrade:
              conn->parser_flags |= F_CONNECTION_UPGRADE;
              break;
            case h_transfer_encoding_chunked:
              conn->parser_flags |= F_CHUNKED;
              break;
            default:
              break;
          }

          /* header value was empty */
          MARK(header_value);
          UPDATE_STATE(s_header_field_start);
          CALLBACK_DATA_NOADVANCE(header_value);
          REEXECUTE();
        }
      }
//----------------------------------------------------------------------------------------------------
	case s_headers_almost_done:	// #56
		{
		STRICT_CHECK(ch != LF);

		if (conn->parser_flags & F_TRAILING)
			{
			// End of a chunked request
			UPDATE_STATE(s_message_done);
			CALLBACK_NOTIFY_NOADVANCE(chunk_complete);
			REEXECUTE();
			}

		// Cannot use chunked encoding and a content-length header together
		// per the HTTP specification.
		if ((conn->parser_flags & F_CHUNKED) &&
			(conn->parser_flags & F_CONTENTLENGTH))
			{
			SET_ERRNO(HPE_UNEXPECTED_CONTENT_LENGTH);
			goto error;
			}

		UPDATE_STATE(s_headers_done);

		// Set this here so that on_headers_complete() callbacks can see it
		conn->parser_upgrade =
			((conn->parser_flags & (F_UPGRADE | F_CONNECTION_UPGRADE)) ==
			(F_UPGRADE | F_CONNECTION_UPGRADE) ||
		conn->parser_method == HTTP_CONNECT);

        /* Here we call the headers_complete callback. This is somewhat
         * different than other callbacks because if the user returns 1, we
         * will interpret that as saying that this message has no body. This
         * is needed for the annoying case of recieving a response to a HEAD
         * request.
         *
         * We'd like to use CALLBACK_NOTIFY_NOADVANCE() here but we cannot, so
         * we have to simulate it by handling a change in errno below.
         */
        if (settings->on_headers_complete)
		{
 		switch (settings->on_headers_complete(conn))
			{
			case 0:
			break;

			case 1:
			conn->parser_flags |= F_SKIPBODY;
			break;

 			default:
			SET_ERRNO(HPE_CB_headers_complete);
			RETURN(p - data); /* Error */
			}
		}

        if (HTTP_PARSER_ERRNO(conn) != HPE_OK)
		{
		RETURN(p - data);
		}

	REEXECUTE();
	}
//----------------------------------------------------------------------------------------------------
	case s_headers_done:	// #57
		{
        	int hasBody;
		STRICT_CHECK(ch != LF);

		conn->parser_nread = 0;

		hasBody = conn->parser_flags & F_CHUNKED ||
			(conn->parser_content_length > 0 && conn->parser_content_length != ULLONG_MAX);

		if (conn->parser_upgrade && (conn->parser_method == HTTP_CONNECT ||
			(conn->parser_flags & F_SKIPBODY) || !hasBody))
			{
			// Exit, the rest of the message is in a different protocol
			UPDATE_STATE(NEW_MESSAGE());
			CALLBACK_NOTIFY(message_complete);
			RETURN((p - data) + 1);
			}

		if (conn->parser_flags & F_SKIPBODY)
			{
			UPDATE_STATE(NEW_MESSAGE());
			CALLBACK_NOTIFY(message_complete);
			}

		else if (conn->parser_flags & F_CHUNKED)
			{
			// chunked encoding - ignore Content-Length header
			UPDATE_STATE(s_chunk_size_start);
			}

		else
			{
			if (conn->parser_content_length == 0)
				{
				// Content-Length header given but zero: Content-Length: 0\r\n
				UPDATE_STATE(NEW_MESSAGE());
				CALLBACK_NOTIFY(message_complete);
				}
			else if (conn->parser_content_length != ULLONG_MAX)
				{
				// Content-Length header given and non-zero
				UPDATE_STATE(s_body_identity);
				}

			else
				{
				if (!http_message_needs_eof(conn))
					{
					// Assume content-length 0 - read the next
					UPDATE_STATE(NEW_MESSAGE());
					CALLBACK_NOTIFY(message_complete);
					}

			else
					{
					// Read body until EOF
					UPDATE_STATE(s_body_identity_eof);
					}
				}
			}

		break;
		}
//----------------------------------------------------------------------------------------------------
	case s_body_identity:	// #61
		{
		uint64_t to_read = MIN(conn->parser_content_length,
			(uint64_t) ((data + len) - p));

		assert(conn->parser_content_length != 0
			&& conn->parser_content_length != ULLONG_MAX);

		/* The difference between advancing content_length and p is because
		* the latter will automaticaly advance on the next loop iteration.
		* Further, if content_length ends up at 0, we want to see the last
		* byte again for our message complete callback.
		*/
		MARK(body);
		conn->parser_content_length -= to_read;
		p += to_read - 1;

		if (conn->parser_content_length == 0)
			{
			UPDATE_STATE(s_message_done);

			/* Mimic CALLBACK_DATA_NOADVANCE() but with one extra byte.
			*
			* The alternative to doing this is to wait for the next byte to
			* trigger the data callback, just as in every other case. The
			* problem with this is that this makes it difficult for the test
			* harness to distinguish between complete-on-EOF and
			* complete-on-length. It's not clear that this distinction is
			* important for applications, but let's keep it for now.
			*/
			CALLBACK_DATA_(body, p - body_mark + 1, p - data);
			REEXECUTE();
			}

	break;
	}
//----------------------------------------------------------------------------------------------------
      /* read until EOF */
      case s_body_identity_eof:	// #62
        MARK(body);
        p = data + len - 1;

        break;
//----------------------------------------------------------------------------------------------------
      case s_message_done:	// #63
        UPDATE_STATE(NEW_MESSAGE());
        CALLBACK_NOTIFY(message_complete);
        if (conn->parser_upgrade) {
          /* Exit, the rest of the message is in a different protocol. */
          RETURN((p - data) + 1);
        }
        break;
//----------------------------------------------------------------------------------------------------
      case s_chunk_size_start:	// #52
      {
        assert(conn->parser_nread == 1);
        assert(conn->parser_flags & F_CHUNKED);

        unhex_val = unhex[(unsigned char)ch];
        if (UNLIKELY(unhex_val == -1)) {
          SET_ERRNO(HPE_INVALID_CHUNK_SIZE);
          goto error;
        }

        conn->parser_content_length = unhex_val;
        UPDATE_STATE(s_chunk_size);
        break;
      }
//----------------------------------------------------------------------------------------------------
      case s_chunk_size:	// #53
      {
        uint64_t t;

        assert(conn->parser_flags & F_CHUNKED);

        if (ch == CR) {
          UPDATE_STATE(s_chunk_size_almost_done);
          break;
        }

        unhex_val = unhex[(unsigned char)ch];

        if (unhex_val == -1) {
          if (ch == ';' || ch == ' ') {
            UPDATE_STATE(s_chunk_parameters);
            break;
          }

          SET_ERRNO(HPE_INVALID_CHUNK_SIZE);
          goto error;
        }

        t = conn->parser_content_length;
        t *= 16;
        t += unhex_val;

        /* Overflow? Test against a conservative limit for simplicity. */
        if (UNLIKELY((ULLONG_MAX - 16) / 16 < conn->parser_content_length)) {
          SET_ERRNO(HPE_INVALID_CONTENT_LENGTH);
          goto error;
        }

        conn->parser_content_length = t;
        break;
      }
//----------------------------------------------------------------------------------------------------
      case s_chunk_parameters:	// #54
      {
        assert(conn->parser_flags & F_CHUNKED);
        /* just ignore this shit. TODO check for overflow */
        if (ch == CR) {
          UPDATE_STATE(s_chunk_size_almost_done);
          break;
        }
        break;
      }
//----------------------------------------------------------------------------------------------------
      case s_chunk_size_almost_done:	// #55
      {
        assert(conn->parser_flags & F_CHUNKED);
        STRICT_CHECK(ch != LF);

        conn->parser_nread = 0;

        if (conn->parser_content_length == 0) {
          conn->parser_flags |= F_TRAILING;
          UPDATE_STATE(s_header_field_start);
        } else {
          UPDATE_STATE(s_chunk_data);
        }
        CALLBACK_NOTIFY(chunk_header);
        break;
      }
//----------------------------------------------------------------------------------------------------
      case s_chunk_data:	// #58
      {
        uint64_t to_read = MIN(conn->parser_content_length,
                               (uint64_t) ((data + len) - p));

        assert(conn->parser_flags & F_CHUNKED);
        assert(conn->parser_content_length != 0
            && conn->parser_content_length != ULLONG_MAX);

        /* See the explanation in s_body_identity for why the content
         * length and data pointers are managed this way.
         */
        MARK(body);
        conn->parser_content_length -= to_read;
        p += to_read - 1;

        if (conn->parser_content_length == 0) {
          UPDATE_STATE(s_chunk_data_almost_done);
        }

        break;
      }
//----------------------------------------------------------------------------------------------------
      case s_chunk_data_almost_done:	// #59
//spz        assert(parser->flags & F_CHUNKED);
//spz        assert(parser->content_length == 0);
        STRICT_CHECK(ch != CR);
        UPDATE_STATE(s_chunk_data_done);
        CALLBACK_DATA(body);
        break;
//----------------------------------------------------------------------------------------------------
      case s_chunk_data_done:	// #60
//spz        assert(conn->parser_flags & F_CHUNKED);
        STRICT_CHECK(ch != LF);
        conn->parser_nread = 0;
        UPDATE_STATE(s_chunk_size_start);
        CALLBACK_NOTIFY(chunk_complete);
        break;
//----------------------------------------------------------------------------------------------------
      default:
        assert(0 && "unhandled state");
        SET_ERRNO(HPE_INVALID_INTERNAL_STATE);
        goto error;
    }
  }

  /* Run callbacks for any marks that we have leftover after we ran our of
   * bytes. There should be at most one of these set, so it's OK to invoke
   * them in series (unset marks will not result in callbacks).
   *
   * We use the NOADVANCE() variety of callbacks here because 'p' has already
   * overflowed 'data' and this allows us to correct for the off-by-one that
   * we'd otherwise have (since CALLBACK_DATA() is meant to be run with a 'p'
   * value that's in-bounds).
   */

  assert(((header_field_mark ? 1 : 0) +
          (header_value_mark ? 1 : 0) +
          (url_mark ? 1 : 0)  +
          (body_mark ? 1 : 0) +
          (status_mark ? 1 : 0)) <= 1);

  CALLBACK_DATA_NOADVANCE(header_field);
  CALLBACK_DATA_NOADVANCE(header_value);
  CALLBACK_DATA_NOADVANCE(url);
  CALLBACK_DATA_NOADVANCE(body);
  CALLBACK_DATA_NOADVANCE(status);

  RETURN(len);

error:
  if (HTTP_PARSER_ERRNO(conn) == HPE_OK)
	{
	SET_ERRNO(HPE_UNKNOWN);
	}

  RETURN(p - data);
}







//##################################################################################################
//### FName: http_message_needs_eof
//###  Desc: Does the parser need to see an EOF to find the end of the message?
//###  Para: 
//###  Rets: -/-
//##################################################################################################
int ICACHE_FLASH_ATTR
http_message_needs_eof (const WebIf_HTTPDConnSlotData_t *conn)
{
  if (conn->parser_type == HTTP_REQUEST)
	{
	return 0;
	}

  // See RFC 2616 section 4.4
  if (conn->parser_status_code / 100 == 1 ||	// 1xx e.g. Continue
      conn->parser_status_code == 204 ||	// No Content
      conn->parser_status_code == 304 ||	// Not Modified
      conn->parser_flags & F_SKIPBODY)
	{
	// response to a HEAD request
	return 0;
	}

  if ((conn->parser_flags & F_CHUNKED) ||
	conn->parser_content_length != ULLONG_MAX)
	{
	return 0;
	}

  return 1;
}



//##################################################################################################
//### FName: http_should_keep_alive
//###  Desc: Checks 
//###  Para: 
//###  Rets: -/-
//##################################################################################################
int ICACHE_FLASH_ATTR
http_should_keep_alive (const WebIf_HTTPDConnSlotData_t *conn)
{
  if (conn->parser_http_major > 0 && conn->parser_http_minor > 0)
	{
	// HTTP/1.1
	if (conn->parser_flags & F_CONNECTION_CLOSE)
		{
		return 0;
		}
	}
  else
	{
	// HTTP/1.0 or earlier
	if (!(conn->parser_flags & F_CONNECTION_KEEP_ALIVE))
		{
		return 0;
		}
	}

  return !http_message_needs_eof(conn);
}



//##################################################################################################
//### FName: http_method_str
//###  Desc: Return ptr to http method string
//###  Para: 
//###  Rets: -/-
//##################################################################################################
const char * ICACHE_FLASH_ATTR
http_method_str (enum http_method m)
{
  return ELEM_AT(method_strings, m, "<unknown>");
}



// umbenennen in ParserManageBuffers?
//##################################################################################################
//### FName: HTTPD_ParserInit
//###  Desc: Inits the HTTPD-Parser struct to be ready for a new message (Request or Response)
//###        frees also allocated memory !
//###  Para: WebIf_HTTPDConnSlotData_t *conn -> connection slot to init
//###        enum http_parser_type t -> set parser functionality (Request / Response / Both)
//###  Rets: -/-
//##################################################################################################
void ICACHE_FLASH_ATTR
HTTPD_ParserInit (WebIf_HTTPDConnSlotData_t *conn,
			enum http_parser_type pType)
{
 // free allocated memory for stored header fields
  HeaderFieldInfo_t *n1;
  while (!STAILQ_EMPTY(&conn->head))
	{
        n1 = STAILQ_FIRST(&conn->head);
	# if SCDED_DBG >= 3
	printf("|DeallocFldVal:\"%s\" (ID:%d)>",
		n1->HdrFldVal,
		n1->HdrFldID);
	# endif
        STAILQ_REMOVE_HEAD(&conn->head, entries);
        free(n1);
	}

  // free allocated memory for Path
  if (conn->url != NULL) 
	{
	free(conn->url);
	conn->url = NULL;
	}

  // free allocated memory for Query
  if (conn->getArgs != NULL)
	{
	free(conn->getArgs);
	conn->getArgs = NULL;
	}

  // free allocated memory for Post Buff
  if (conn->postBuff != NULL)
	{
	free(conn->postBuff);
	conn->postBuff = NULL;
	}


//temp this both, because code should be improved:
  // free allocated memory for Header Field Name Buffer (only in case of error)
  if (conn->HdrFldNameBuff != NULL)
	{
	free(conn->HdrFldNameBuff);
	conn->HdrFldNameBuff = NULL;
	}

  // free allocated memory for Header Field Value Buffer (only in case of error)
  if (conn->HdrFldValueBuff != NULL)
	{
	free(conn->HdrFldValueBuff);
	conn->HdrFldValueBuff = NULL;
	}


  // Step #1 Clear only
  // NOW ZERO !PARTS! OF STRUCT
//  memset(&conn->PCData+4, 0, 4+4+8+2+2+4+4+(7*4)+4); // CLEARLENGTH =
 
  memset(conn + offsetof(WebIf_HTTPDConnSlotData_t, parser_nread),
	0,
	sizeof(WebIf_HTTPDConnSlotData_t) - offsetof(WebIf_HTTPDConnSlotData_t, parser_nread) ); // CLEARLENGTH =

/*  // Step #1 Clear only
  conn->parser_flags = 0;
  conn->parser_header_state = 0;
  conn->parser_index = 0;
  conn->parser_lenient_http_headers = 0;
  conn->parser_nread = 0;
  conn->parser_content_length = 0;

  conn->parser_status_code = 0;
  conn->parser_method = 0;
*/
//  conn->parser_dataX = data;

  // Step #2 Reinit values
  conn->parser_type = pType;
  conn->parser_state = (pType == HTTP_REQUEST ? s_start_req : (pType == HTTP_RESPONSE ? s_start_res : s_start_req_or_res));
  conn->parser_http_errno = HPE_OK;
}


/*
//##################################################################################################
//### FName: http_parser_init
//###  Desc: Inits the parser to be ready for a new message
//###  Para: WebIf_HTTPDConnSlotData_t *conn -> connection slot to init
//###        enum http_parser_type t -> set parser functionality
//###  Rets: -/-
//##################################################################################################
void ICACHE_FLASH_ATTR
http_parser_init (WebIf_HTTPDConnSlotData_t *conn,
			enum http_parser_type pType)
{
//  void *data = conn->parser_dataX;  preserve application data  raus??????????

//aaahhhh  memset(parser, 0, sizeof(*parser));!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


  // Step #1 Clear only
  conn->parser_flags = 0;
  conn->parser_header_state = 0;
  conn->parser_index = 0;
  conn->parser_lenient_http_headers = 0;

  conn->parser_nread = 0;
  conn->parser_content_length = 0;

  conn->parser_status_code = 0;
  conn->parser_method = 0;

//  conn->parser_dataX = data;

  // Step #2 Reinit values
  conn->parser_type = pType;
  conn->parser_state = (pType == HTTP_REQUEST ? s_start_req : (pType == HTTP_RESPONSE ? s_start_res : s_start_req_or_res));
  conn->parser_http_errno = HPE_OK;
}
*/


//##################################################################################################
//### FName: http_parser_settings_init
//###  Desc: Inits the settings structure (valid for all connection slots)
//###  Para: http_parser_settings *settings -> ptr to allocated memory for settings 
//###  Rets: -/-
//##################################################################################################
void ICACHE_FLASH_ATTR
http_parser_settings_init(http_parser_settings_t *settings)
{
  memset(settings, 0, sizeof(http_parser_settings_t)); //*settings));
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: HTTPD_ErrnoCode
 * Desc: Get pointer to the external code of current error 
 * Para: enum http_errno err -> current error number (0=OK)
 * Rets: const char * -> ptr to description string
 *--------------------------------------------------------------------------------------------------
 */
const unsigned int ICACHE_FLASH_ATTR
HTTPD_ErrnoCode(enum http_errno err)
  {
  return http_strerror_tab[err].code;
  }



/*
 *--------------------------------------------------------------------------------------------------
 *FName: HTTPD_ErrnoDescription
 * Desc: Get pointer to the external description of current error 
 * Para: enum http_errno err -> current error number (0=OK)
 * Rets: const char * -> ptr to description string
 *--------------------------------------------------------------------------------------------------
 */
const char * ICACHE_FLASH_ATTR
HTTPD_ErrnoDescription(enum http_errno err)
  {
  return http_strerror_tab[err].description;
  }



//##################################################################################################
//### FName: http_parse_host_char
//###  Desc: 
//###  Para: -/-
//###  Rets: static enum http_host_state -> ?
//##################################################################################################
static enum http_host_state ICACHE_FLASH_ATTR
http_parse_host_char(enum http_host_state s,
			const char ch)
{
  switch(s)
	{
//-------------------------------------------------------------------------------
	case s_http_userinfo:
	case s_http_userinfo_start:
		if (ch == '@')
			{
			return s_http_host_start;
			}

		if (IS_USERINFO_CHAR(ch))
			{
			return s_http_userinfo;
			}
	break;
//-------------------------------------------------------------------------------
	case s_http_host_start:
		if (ch == '[')
			{
			return s_http_host_v6_start;
			}

		if (IS_HOST_CHAR(ch))
			{
			return s_http_host;
			}
//-------------------------------------------------------------------------------
	break;

	case s_http_host:
		if (IS_HOST_CHAR(ch))
			{
			return s_http_host;
			}
//-------------------------------------------------------------------------------
	// FALLTHROUGH
	case s_http_host_v6_end:
		if (ch == ':')
			{
			return s_http_host_port_start;
			}

	break;
//-------------------------------------------------------------------------------
	case s_http_host_v6:
		if (ch == ']')
			{
			return s_http_host_v6_end;
			}
//-------------------------------------------------------------------------------
	// FALLTHROUGH
	case s_http_host_v6_start:
		if (IS_HEX(ch) || ch == ':' || ch == '.')
			{
			return s_http_host_v6;
			}

		if (s == s_http_host_v6 && ch == '%')
			{
        		return s_http_host_v6_zone_start;	
			}
	break;
//-------------------------------------------------------------------------------
	case s_http_host_v6_zone:
		if (ch == ']')
			{
			return s_http_host_v6_end;
			}
//-------------------------------------------------------------------------------
	// FALLTHROUGH
	case s_http_host_v6_zone_start:
		// RFC 6874 Zone ID consists of 1*( unreserved / pct-encoded)
		if (IS_ALPHANUM(ch) || ch == '%' || ch == '.' || ch == '-' || ch == '_' ||
			ch == '~')
			{
			return s_http_host_v6_zone;
			}
	break;
//-------------------------------------------------------------------------------
	case s_http_host_port:
	case s_http_host_port_start:
		if (IS_NUM(ch))
			{
			return s_http_host_port;
			}

		break;
//-------------------------------------------------------------------------------
	default:
	break;
	}
  return s_http_host_dead;
}



//##################################################################################################
//### FName: http_parse_host
//###  Desc: Parses HOST field extraxted from URL by func. http_parser_parse_url
//###  Para: -/-
//###  Rets: static int -> ?
//##################################################################################################
static int ICACHE_FLASH_ATTR
http_parse_host(const char * buf,
	struct http_parser_url *u,
	int found_at)
{
	assert(u->field_set & (1 << UF_HOST));
	enum http_host_state s;

	const char *p;
	size_t buflen = u->field_data[UF_HOST].off + u->field_data[UF_HOST].len;

	u->field_data[UF_HOST].len = 0;

	s = found_at ? s_http_userinfo_start : s_http_host_start;

	for (p = buf + u->field_data[UF_HOST].off; p < buf + buflen; p++)
		{
		enum http_host_state new_s = http_parse_host_char(s, *p);

		if (new_s == s_http_host_dead)
			{
			return 1;
			}

 	# if SCDED_DBG >= 4
	printf("|PH%d>",
		new_s);
  	#endif

	switch(new_s)
		{
//-------------------------------------------------------------------------------
		case s_http_host:
			if (s != s_http_host)
				{
				u->field_data[UF_HOST].off = p - buf;
				}
			u->field_data[UF_HOST].len++;
		break;
//-------------------------------------------------------------------------------
		case s_http_host_v6:
			if (s != s_http_host_v6)
				{
				u->field_data[UF_HOST].off = p - buf;
				}
			u->field_data[UF_HOST].len++;
		break;
//-------------------------------------------------------------------------------
		case s_http_host_v6_zone_start:
		case s_http_host_v6_zone:
			u->field_data[UF_HOST].len++;
		break;
//-------------------------------------------------------------------------------
		case s_http_host_port:
			if (s != s_http_host_port)
				{
				u->field_data[UF_PORT].off = p - buf;
				u->field_data[UF_PORT].len = 0;
				u->field_set |= (1 << UF_PORT);
				}
			u->field_data[UF_PORT].len++;
		break;
//-------------------------------------------------------------------------------
		case s_http_userinfo:
			if (s != s_http_userinfo)
				{
				u->field_data[UF_USERINFO].off = p - buf ;
				u->field_data[UF_USERINFO].len = 0;
				u->field_set |= (1 << UF_USERINFO);
				}
			u->field_data[UF_USERINFO].len++;
		break;
//-------------------------------------------------------------------------------
		default:
		break;
//-------------------------------------------------------------------------------
		}
	s = new_s;
	}

  // Make sure we don't end somewhere unexpected

  switch (s)
	{
	case s_http_host_start:
	case s_http_host_v6_start:
	case s_http_host_v6:
	case s_http_host_v6_zone_start:
	case s_http_host_v6_zone:
	case s_http_host_port_start:
	case s_http_userinfo:
	case s_http_userinfo_start:
 		return 1;
	default:
	break;
	}

  return 0;
}



//##################################################################################################
//### FName: http_parser_url_init
//###  Desc: Prepares struct http_parser_url for URL parsing (by zeroing data)
//###  Para: struct http_parser_url *u -> ptr to reserved memory
//###  Rets: -/-
//##################################################################################################
void ICACHE_FLASH_ATTR
http_parser_url_init(struct http_parser_url *u)
{
  memset(u, 0, sizeof(*u));
}



//##################################################################################################
//### FName: http_parser_parse_url
//###  Desc: Parses URL and extracts field data to struct http_parser_url
//###        !All http_parser_url members must be initialized to 0! (by http_parser_url_init)
//###  Para: const char *buf -> ptr to data in buffer
//###        size_t buflen -> data length in buffer
//###        int is_connect -> ??????? something to do with CONNECT requests can only contain "hostname:port"
//###        struct http_parser_url *u -> ptr to initialized (zeroed) struct http_parser_url
//###  Rets: Callbacks must return 0 on success. Returning a non-zero value indicates error to the
//###        parser, making it exit immediately.
//##################################################################################################
int ICACHE_FLASH_ATTR
http_parser_parse_url(const char *buf,
			size_t buflen,
			int is_connect,
			struct http_parser_url *u)
{
  enum state s;
  const char *p;
  enum http_parser_url_fields uf, old_uf;
  int found_at = 0;

  u->port = u->field_set = 0;
  s = is_connect ? s_req_server_start : s_req_spaces_before_url;
  old_uf = UF_MAX;

  for (p = buf; p < buf + buflen; p++)
	{
	s = parse_url_char(s, *p);

 	# if SCDED_DBG >= 4
	printf("|PUC%d>",
		s);
  	#endif

	// Figure out the next field that we're operating on
	switch (s)
		{
		case s_dead:
			return 1;
//-------------------------------------------------------------------------------
		// Skip delimeters
		case s_req_schema_slash:
		case s_req_schema_slash_slash:
		case s_req_server_start:
		case s_req_query_string_start:
		case s_req_fragment_start:
			continue;
//-------------------------------------------------------------------------------
 		case s_req_schema:
			uf = UF_SCHEMA;
			break;
//-------------------------------------------------------------------------------
		case s_req_server_with_at:
			found_at = 1;
//-------------------------------------------------------------------------------
		// FALLTROUGH
		case s_req_server:
 			uf = UF_HOST;
			break;
//-------------------------------------------------------------------------------
		case s_req_path:
			uf = UF_PATH;
			break;
//-------------------------------------------------------------------------------
		case s_req_query_string:
			uf = UF_QUERY;
			break;
//-------------------------------------------------------------------------------
		case s_req_fragment:
			uf = UF_FRAGMENT;
			break;
//-------------------------------------------------------------------------------
		default:
		assert(!"Unexpected state");
		return 1;
		}
//-------------------------------------------------------------------------------
	// Nothing's changed; soldier on
	if (uf == old_uf)
		{
		u->field_data[uf].len++;
		continue;
		}

	u->field_data[uf].off = p - buf;
	u->field_data[uf].len = 1;

	# if SCDED_DBG >= 4
	printf("|URI part:%.*s,Len:%d is uf:%d>"
		,1
		,p
		,u->field_data[uf].len
		,uf);
  	#endif

	u->field_set |= (1 << uf);
	old_uf = uf;
	}
//-------------------------------------------------------------------------------
// final compliance checks ...

/* not in SCD engine
  // host must be present if there is a schema
  // parsing http:///toto will fail
  if ((u->field_set & (1 << UF_SCHEMA)) &&
      (u->field_set & (1 << UF_HOST)) == 0)
	{
	return 1;
	}
*/

  // if host is present parse it ...
  if (u->field_set & (1 << UF_HOST))
	{
	if (http_parse_host(buf, u, found_at) != 0)
		{
		return 1;
		}
	}

  // CONNECT requests can only contain "hostname:port"
  if (is_connect && u->field_set != ((1 << UF_HOST)|(1 << UF_PORT)))
	{
	return 1;
	}

  // if port is present -> extract it
  if (u->field_set & (1 << UF_PORT))
	{
	// Don't bother with endp; we've already validated the string
	unsigned long v = strtoul(buf + u->field_data[UF_PORT].off, NULL, 10);

	// Ports have a max value of 2^16
	if (v > 0xffff)
		{
		return 1;
		}

	u->port = (uint16_t) v;
	}

  return 0;
}



//##################################################################################################
//### FName: http_parser_pause
//###  Desc: Puts parser in pause (only for debug reasons)
//###  Para: WebIf_HTTPDConnSlotData_t *conn -> connection slot that should be paused
//###  Rets: -/-
//##################################################################################################
void ICACHE_FLASH_ATTR
http_parser_pause(WebIf_HTTPDConnSlotData_t *conn,
			int paused)
{
  // Users should only be pausing/unpausing a parser that is not in an error
  // state. In non-debug builds, there's not much that we can do about this
  // other than ignore it.
  if (HTTP_PARSER_ERRNO(conn) == HPE_OK ||
	HTTP_PARSER_ERRNO(conn) == HPE_PAUSED)
	{
	SET_ERRNO((paused) ? HPE_PAUSED : HPE_OK);
	}
  else
	{
	assert(0 && "Attempting to pause parser in error state");
	}
}



//##################################################################################################
//### FName: http_body_is_final
//###  Desc: Call this to indicate that body processing is finnished
//###  Para: WebIf_HTTPDConnSlotData_t *conn -> connection slot in process
//###  Rets: parser_state -> new parser state
//##################################################################################################
int ICACHE_FLASH_ATTR
http_body_is_final(const WebIf_HTTPDConnSlotData_t *conn)
{
 # if SCDED_DBG >= 4
  printf("|BdyFinalCb\n");
  #endif

//  // set current slot-activity status
//  conn->SlotParserState = s_body_is_final;

  return conn->parser_state == s_message_done;
}



//##################################################################################################
//### FName: http_parser_version
//###  Desc: Returns the version number of the parser
//###  Para: -/-
//###  Rets: unsigned long -> coded version number
//##################################################################################################
unsigned long ICACHE_FLASH_ATTR
http_parser_version(void)
{
  return HTTP_PARSER_VERSION_MAJOR * 0x10000 |
         HTTP_PARSER_VERSION_MINOR * 0x00100 |
         HTTP_PARSER_VERSION_PATCH * 0x00001;
}



//##################################################################################################
//### FName: HTTPDMsgBeginCb
//###  Desc: Callback fired by HTTPDParser when starting parsing new incoming message from fresh connection
//###  Para: 
//###  Rets: Callbacks must return 0 on success. Returning a non-zero value indicates error to the
//###        parser, making it exit immediately.
//##################################################################################################
int ICACHE_FLASH_ATTR
HTTPDMsgBeginCb(WebIf_HTTPDConnSlotData_t *conn)
{

  # if SCDED_DBG >= 4
  printf("|MsgBegCb>");
  # endif

  // set current slot-activity status
  conn->SlotParserState = s_HTTPD_Msg_Begin;

  return 0;
}



//##################################################################################################
//### FName: HTTPDUrlFoundCb
//###  Desc: Callback fired by HTTPDParser when url is found, used for parsing URI into elements
//###  Para: 
//###  Rets: Callbacks must return 0 on success. Returning a non-zero value indicates error to the
//###        parser, making it exit immediately.
//##################################################################################################
int ICACHE_FLASH_ATTR
HTTPDUrlFoundCb(WebIf_HTTPDConnSlotData_t *conn,
			const char *at,
			size_t length)
{
  // set current slot-activity status
  conn->SlotParserState = s_HTTPD_Url_Found;

  // New URI? Start with cleared F_CONN_IS_AUTHENTICATED flag in ConnCtrl
  conn->ConnCtrlFlags &= ~F_CONN_IS_AUTHENTICATED;

  // alloc memory for struct http_parser_url
  struct http_parser_url *http_parser_url =
	(struct http_parser_url *) malloc(sizeof(struct http_parser_url));

  // Zero struct http_parser_url
  http_parser_url_init(http_parser_url);

  // Parse url data to struct http_parser_url
  http_parser_parse_url(at,
			length,
			0, // its not a connect request
			http_parser_url);

  # if SCDED_DBG >= 4
  printf("|UrlFndCb,Parsed URI,BF:%d",	// URI Elements Bitfield
	http_parser_url->field_set);
  #endif
//--------------------------------------------------------------------------------------------------
  if (http_parser_url->field_set & (1 << UF_SCHEMA))
	{
	# if SCDED_DBG >= 4
	printf(",SCHEMA:\"%s\",len:%d",
		at+http_parser_url->field_data[UF_SCHEMA].off,
		http_parser_url->field_data[UF_SCHEMA].len);
	# endif
	}
//--------------------------------------------------------------------------------------------------
  // have we found a SCHEMA in the URI ?
  if (http_parser_url->field_set & (1 << UF_SCHEMA))
	{
	// get pos and len of NOT zero terminated SCHEMA from uri
	const char *pos = at+http_parser_url->field_data[UF_SCHEMA].off;
	int len = http_parser_url->field_data[UF_SCHEMA].len;

	int i = 0;
	while ( (AvailSchemes[i].SchemeName != NULL) &&
		(strncasecmp(pos, AvailSchemes[i].SchemeName, len) != 0) )
		{
		i++;
		}
	conn->parser_scheme = i;

	# if SCDED_DBG >= 4
	printf(",SCHEMAX:\"%s\"",
		pos);
	# endif

	}

	# if SCDED_DBG >= 4
	printf(",SCHEME:%d",
		conn->parser_scheme);
	# endif
//--------------------------------------------------------------------------------------------------
  if (http_parser_url->field_set & (1 << UF_USERINFO))
	{
	# if SCDED_DBG >= 4
	printf(",UInfo:\"%s\",len:%d",
		at+http_parser_url->field_data[UF_USERINFO].off,
		http_parser_url->field_data[UF_USERINFO].len);
	# endif

	// Check User Info
	SCDED_CheckHdrFldAuth(conn,
		(char *) at+http_parser_url->field_data[UF_USERINFO].off,
		http_parser_url->field_data[UF_USERINFO].len);
	}
//--------------------------------------------------------------------------------------------------
  if (http_parser_url->field_set & (1 << UF_HOST))
	{
	# if SCDED_DBG >= 4
	printf(",HOST:\"%s\",len:%d",
		at+http_parser_url->field_data[UF_HOST].off,
		http_parser_url->field_data[UF_HOST].len);
	# endif
	}
//--------------------------------------------------------------------------------------------------
  if (http_parser_url->field_set & (1 << UF_PORT))
	{
	# if SCDED_DBG >= 4
	printf(",PORT:%d",
		http_parser_url->port);
	# endif
	}
//--------------------------------------------------------------------------------------------------
  // have we found a PATH in the URI ?
  if (http_parser_url->field_set & (1 << UF_PATH))
	{
	// get pos and len of NOT zero terminated path from uri
	const char *pos = at+http_parser_url->field_data[UF_PATH].off;
	int len = http_parser_url->field_data[UF_PATH].len;

	// store the path
	conn->url = malloc(len+1);
	memcpy(conn->url, pos, len);
	conn->url[len] = '\0';	// zero terminate

	# if SCDED_DBG >= 4
	printf(",PATH:\"%s\"",
		conn->url);
	# endif

	// do not forget to dealloc !!
	}
//--------------------------------------------------------------------------------------------------
  // have we found a QUERY in the URI ?
  if (http_parser_url->field_set & (1 << UF_QUERY))
	{
	// get pos and len of NOT zero terminated query from uri
	const char *pos = at+http_parser_url->field_data[UF_QUERY].off;
	int len = http_parser_url->field_data[UF_QUERY].len;

	// store the query
	conn->getArgs = malloc(len+1);
	memcpy(conn->getArgs, pos, len);
	conn->getArgs[len] = '\0';	// zero terminate

	# if SCDED_DBG >= 4
	printf(",QUERY:\"%s\"",
		conn->getArgs);
	# endif

	// do not forget to dealloc !!
	}
//--------------------------------------------------------------------------------------------------
  // free memory reserved for struct http_parser_url
  free(http_parser_url);



  // fehler beim parsen? ->1 ?
  return 0;
}








































/*

int ICACHE_FLASH_ATTR 
HTTPDSend(WebIf_HTTPDConnSlotData_t *conn, const char *data, int len)
{
  // if len is -1 the data is seen as a C-string. Length will be determined ..
  if (len < 0) len = strlen(data);

  // will data fit in sendbuff ?
  if (conn->sendBuffLen + len <= MAX_SENDBUFF_LEN)

	// if data fits in sendbuff ...
	{
	memcpy (conn->sendBuff + conn->sendBuffLen, data, len); // copy to sendbuf
	conn->sendBuffLen += len;

	// return free bytes in sendbuff
	return (MAX_SENDBUFF_LEN - conn->sendBuffLen);
	}
  else
	 // if data does NOT fit in sendbuff ...
	{
	// Step 1: copy data to sendbuff till full
	int SendBufFree = MAX_SENDBUFF_LEN - conn->sendBuffLen;
	if (SendBufFree)
		{
		memcpy (conn->sendBuff + conn->sendBuffLen, data, SendBufFree); // copy to sendbuf
		conn->sendBuffLen += SendBufFree;
		}


	// Step 2: create/add rest of data to TrailingBuff
	int NewTrailingBuffLen;
	if (conn->TrailingBuff == NULL) NewTrailingBuffLen = len - SendBufFree;
	else NewTrailingBuffLen =
		conn->TrailingBuffLen + len - SendBufFree;

	// alloc new TrailingBuff
	char *NewTrailingBuff = (char*)malloc(NewTrailingBuffLen+1);

	// Copy old TrailingBuff to new TrailingBuff, and dealloc old
	if (conn->TrailingBuff != NULL)
		{
		memcpy (NewTrailingBuff, conn->TrailingBuff, conn->TrailingBuffLen); // copy to sendbuf
		free(conn->TrailingBuff);
		}
 else  conn->TrailingBuffLen =0; ///test


	// Add new data to TrailingBuff and save
	memcpy (NewTrailingBuff + conn->TrailingBuffLen, data + SendBufFree, len - SendBufFree); // copy to sendbuf

	# if SCDED_DBG >= 3
	printf("HTTPD %d Bytes to TrailingBuff\n",
		NewTrailingBuffLen);
	#endif

	// Store 
	conn->TrailingBuff = NewTrailingBuff;
	conn->TrailingBuffLen = NewTrailingBuffLen;
	}
  return 0; // 0 bytes free in sendbuff
}

*/
































//##################################################################################################
//### FName: HTTPDHeaderFldCb
//###  Desc: Callback fired by HTTPDParser when (part of!) a header field name is found.
//###        values are stored/added to: conn->HdrFldNameBuff
//###  Para: WebIf_HTTPDConnSlotData_t *conn ->
//###        const char *at, -> ptr to field name string data (to add)
//###        size_t length -> data length of header field (to add)
//###  Rets: Callbacks must return 0 on success. Returning a non-zero value indicates error to the
//###        parser, making it exit immediately.
//##################################################################################################
int ICACHE_FLASH_ATTR
HTTPDHeaderFldCb(WebIf_HTTPDConnSlotData_t *conn,
			const char *at,
			size_t length)
{
  // Process header field name+value that is already stored?
  HTTPDProcHeaderFld(conn);

  // continue, check head field name data for max allowed length
  if (conn->HdrFldNameBuff == NULL)
	{
	if (length > MXHEADERFLDNAMELEN) return 1; // error
	}
  else
	{
	if (strlen(conn->HdrFldNameBuff) + length > MXHEADERFLDNAMELEN) return 1; // error
	}

  // store header field name data
  conn->HdrFldNameBuff = HTTPDStoreAddData(conn->HdrFldNameBuff,(const uint8_t*) at,length);

  # if SCDED_DBG >= 4
  printf("|HdrFldNameCb,Prs:\"%s\">",
	conn->HdrFldNameBuff);
  #endif

  return 0;
}






/* 2. implem
{
  // get header field ID (if implemented)
  int i = HTTPDGetHdrFldID(at,length);

  # if SCDED_DBG >= 4
  printf("|HdrFldCb%s,id:%d,len:%d>",
	at,
	i,
	length);
  #endif

	conn->HdrFldId = i; //-1 do not store 

 return 0;
*/




/* 1. implem
  // if implemented load callback
  if (i >= 0)
	{
	conn->HdrFldFnCb = AvailHdrFlds[i].FldFnCb;
	}
  //  unknown, NULL prevents execution
  else
	{
	conn->HdrFldFnCb = NULL;
	}
 return 0;
*/






//##################################################################################################
//### FName: HTTPDHeaderValCb
//###  Desc: Callback fired by HTTPDParser when (part of!) a header field value is found.
//###        values are stored/added to: conn->HdrFldValueBuff
//###  Para: WebIf_HTTPDConnSlotData_t *conn ->
//###        const char *at, -> ptr to field value string data (to add)
//###        size_t length -> data length of header field value (to add)
//###  Rets: Callbacks must return 0 on success. Returning a non-zero value indicates error to the
//###        parser, making it exit immediately.
//##################################################################################################
int ICACHE_FLASH_ATTR
HTTPDHeaderValCb(WebIf_HTTPDConnSlotData_t *conn,
			const char *at,
			size_t length)
{
  // check header field value for max allowed data length
  if (conn->HdrFldValueBuff == NULL)
	{
	if (length > MXHEADERFLDVALUELEN) return 1; // error
	}
  else
	{
	if (strlen(conn->HdrFldValueBuff) + length > MXHEADERFLDVALUELEN) return 1; // error
	}

  // store header field data
  conn->HdrFldValueBuff = HTTPDStoreAddData(conn->HdrFldValueBuff,(const uint8_t*) at,length);

  # if SCDED_DBG >= 5
  printf("|HdrFldValCb,Prs:\"%s\" for Name:\"%s\">",
	conn->HdrFldValueBuff,
	conn->HdrFldNameBuff);
	#endif

  return 0;
}




/*
int ICACHE_FLASH_ATTR
HTTPDHeaderValCb(WebIf_HTTPDConnSlotData_t *conn,
			const char *at,
			size_t length)
{
  // data in header field buffer ?
  if (conn->HdrFldBuff != NULL)
	{
	// get header field ID (if implemented)
	int HdrFldId = HTTPDGetHdrFldID(conn->HdrFldBuff,strlen(conn->HdrFldBuff));

	# if SCDED_DBG >= 4
	printf("|HdrFldCb%s,id:%d,len:%d>",
		at,
		HdrFldId,
		length);
	#endif

	// should we store value?
	if (HdrFldId >= 0)
		{
		HeaderFieldInfo_t *n1;
		// store int8_t ID+valuestring+zero Termination
		n1 = malloc(4+4+length+1);//4+1+!!!!!
 		n1->HdrFldID = conn->HdrFldId;
		memcpy(&n1->HdrFldVal, at, length);
		n1->HdrFldVal[length] = '\0';	// zero terminate
		STAILQ_INSERT_HEAD(&conn->head, n1, entries);

		# if SCDED_DBG >= 4
		printf("|StorFldVal:%s,id:%d>",
			n1->HdrFldVal,
			n1->HdrFldID);
		#endif
		}
	else
		{
		# if SCDED_DBG >= 4
		printf("|SkpFldVal>");
		#endif
		}

	// dealloc header field buffer
	free(conn->HdrFldBuff);
	}

  // if no data in header field buffer something went wrong ...
  else
	{
	# if SCDED_DBG >= 4
	printf("|ERROR!HFB>");
	#endif
	return 1;
	}

  return 0;	// should return 1 f to many field-data or fields that are a no go!
}
*/



/* working second ..
{
  if (conn->HdrFldId >= 0)
	{
	HeaderFieldInfo_t *n1;
	// store int8_t ID+valuestring+zero Termination
	n1 = malloc(4+4+length+1);//4+1+!!!!!
        n1->HdrFldID = conn->HdrFldId;
	memcpy(&n1->HdrFldVal, at, length);
	n1->HdrFldVal[length] = '\0';	// zero terminate
        STAILQ_INSERT_HEAD(&conn->head, n1, entries);

	# if SCDED_DBG >= 4
	printf("|StorFldVal:%s,id:%d>",
		n1->HdrFldVal,
		n1->HdrFldID);
	#endif
	}
  else
	{
	# if SCDED_DBG >= 4
	printf("|SkpFldVal>");
	#endif
	}

  return 0;	// should return 1 f to many field-data or fields that are a no go!
}
*/







/*
		next_station = STAILQ_NEXT(station,next);
		free(station);
		station = next_station;
*/

/*
        while (!STAILQ_EMPTY(&head)) {
                n1 = STAILQ_LAST(&head, stailq_entry, entries);
                STAILQ_REMOVE(&head, n1, stailq_entry, entries);
                printf ("n2: %d\n", n1->value);
                free(n1);

# if SCDED_DBG >= 4
	HeaderFieldInfo_t *n1;
	while (!STAILQ_EMPTY(&conn->head))
		{
		n1 = STAILQ_LAST(&conn->head, stailq_entry, entries);
		printf("|StorFldVal:%s,id:%d",
	n1->HdrFldVal,
	n1->HdrFldID);
              }
#endif

*/


/*
	HeaderFieldInfo_t *n1;
	while (!STAILQ_EMPTY(&conn->head))
		{
		n1 = STAILQ_LAST(&conn->head, HeaderFieldInfo_s, entries);

        	n1 = STAILQ_FIRST(&conn->head);
		printf("|DeallocFldVal:%s,id:%d",
			n1->HdrFldVal,
			n1->HdrFldID);
        STAILQ_REMOVE_HEAD(&conn->head, entries);
        free(n1);
              }

*/





// to implement error if to much
/*
  int r = 0; // default -> no error

  if (conn->HdrFldFnCb)
	// if != NULL, Execute header field value processing function
	{
	r = conn->HdrFldFnCb(conn,
			at,
			length);
	}

  return r;
}
*/


//##################################################################################################
//### FName: HTTPDProcHeaderFld
//###  Desc: Process Header field name and value (Store it for later use, if ID is >0)
//###        And deallocs buffers !!!
//###  Para: WebIf_HTTPDConnSlotData_t *conn ->
//###  Rets: -/-
//##################################################################################################
void ICACHE_FLASH_ATTR
HTTPDProcHeaderFld(WebIf_HTTPDConnSlotData_t *conn)
{
  // do we need to process last header field name and value ?
  if (conn->HdrFldNameBuff != NULL)
	{
	// do we have a value too?
	if (conn->HdrFldValueBuff != NULL)
		{
		// get header field ID (if implemented)
		int HdrFldId = SCDED_GetHdrFldID(conn->HdrFldNameBuff,strlen(conn->HdrFldNameBuff));

		# if SCDED_DBG >= 4
		printf("|HdrFldProc,name:\"%s\"(ID:%d),val:\"%s\"-",
			conn->HdrFldNameBuff,
			HdrFldId,
			conn->HdrFldValueBuff);
		#endif

		// should we store value? (>0)
		if (HdrFldId >= 0)
			{
			HeaderFieldInfo_t *n1;
			// store int8_t ID+valuestring+zero Termination
			n1 = malloc(4+4+strlen(conn->HdrFldValueBuff)+1);//4+1+!!!!!
 			n1->HdrFldID = HdrFldId;
			strcpy((char*) &n1->HdrFldVal, (char*) conn->HdrFldValueBuff);
			STAILQ_INSERT_HEAD(&conn->head, n1, entries);

			# if SCDED_DBG >= 4
			printf("(StoreFld)>");
			#endif
			}
		else
			{
			# if SCDED_DBG >= 4
			printf("(SkipFld)>");
			#endif
			}

		// dealloc header field value buffer
		free(conn->HdrFldValueBuff);
		conn->HdrFldValueBuff = NULL;
		}

	// dealloc header field name buffer
	free(conn->HdrFldNameBuff);
	conn->HdrFldNameBuff = NULL;
	}

// done, buffers freed
}


/*
//##################################################################################################
//### FName: HTTPDHdrFldAuthFnCb
//###  Desc: Callback is fired when a value for header field "Authorization" is found, to parse it
//###  Para: 
//###  Rets: Callbacks must return 0 on success. Returning a non-zero value indicates error to the
//###        parser, making it exit immediately.
//##################################################################################################
int ICACHE_FLASH_ATTR
HTTPDHdrFldAuthFnCb(WebIf_HTTPDConnSlotData_t *conn,
			const char *at,
			size_t length)
{
  // Field beginning with "Basic " ?
  if (strncmp(at, "Basic ", 6) == 0)
	{

	char *user = "user";
	char *pass = "pass";

	char userpass[AUTH_MAX_USER_LEN+AUTH_MAX_PASS_LEN+2];
	int r;

	// decode
	r = mybase64_decode(length-6,
		at+6, sizeof(userpass),
		(unsigned char *)userpass);

 	// clean out string on decode error
	if (r<0) r=0;
 
	// zero-terminate user:pass string
	userpass[r]=0;

	# if SCDED_DBG >= 4
	printf("|HdrFldAuthFnCb:%s,",
		userpass);
	#endif

	// Check devices user/pass against extracted from header fied
	if (strlen(userpass) == strlen(user) + strlen(pass)+1 &&
		strncmp(userpass, user, strlen(user))==0 &&
		userpass[strlen(user)]==':' &&
		strcmp(userpass+strlen(user)+1, pass)==0)
		{
		// Authenticated! -> Set Authenticated Bit (1) in ConnCtrl
		conn->ConnCtrl |= (1<<1);

		# if SCDED_DBG >= 4
		printf("OK>");
		#endif
		}
	else
		{
		# if SCDED_DBG >= 4
		printf("NOK!>");
		#endif
		}
	}
  // always OK
  return 0;	
}
*/





//##################################################################################################
//### FName: HTTPDHdrFldHostFnCb
//###  Desc: Callback is fired when a value for header field "Host" is found, to parse it
//###  Para: 
//###  Rets: Callbacks must return 0 on success. Returning a non-zero value indicates error to the
//###        parser, making it exit immediately.
//##################################################################################################
int ICACHE_FLASH_ATTR
HTTPDHdrFldHostFnCb(WebIf_HTTPDConnSlotData_t *conn,
			const char *at,
			size_t length)
{
  # if SCDED_DBG >= 4
  printf("|HdrFldHostFnCb:%s,len:%d>",
	at,
	length);
  #endif

  # if SCDED_DBG >= 4
  printf("test:%.*s\n",
	(int) length,
	at);
  #endif

  return 0;
}






//##################################################################################################
//### FName: HTTPDHeaderPrsdCompleteCb
//###  Desc: Callback fired by HTTPDParser when Header is completely parsed
//###  Para: WebIf_HTTPDConnSlotData_t *conn ->
//###  Rets: Callbacks must return 0 on success. Returning a non-zero value indicates error to the
//###        parser, making it exit immediately.
//##################################################################################################
int ICACHE_FLASH_ATTR
HTTPDHeaderPrsdCompleteCb(WebIf_HTTPDConnSlotData_t *conn)
{
  # if SCDED_DBG >= 4
  printf("HeComplCb>");
  #endif

  // set current slot-activity status
  conn->SlotParserState = s_HTTPD_Header_Prsd_Complete;
//--------------------------------------------------------------------------------------------------
  // Process last header field name+value that may be already stored?
  HTTPDProcHeaderFld(conn);
//--------------------------------------------------------------------------------------------------
  // list stored header fields, if debug
  # if SCDED_DBG >= 4
  printf("\nHTTPD stored HdrFlds:\n");

  HeaderFieldInfo_t *n1;
  STAILQ_FOREACH(n1, &conn->head, entries)
	{
	printf("ID:%d,Val:\"%s\"\n",
		n1->HdrFldID,
		n1->HdrFldVal);
	}
  # endif
//--------------------------------------------------------------------------------------------------
  // list result of parsing process, if debug
  # if SCDED_DBG >= 2
  printf("HTTPD Parsed: V%d.%d,StCd:%d,Method:%d,Scheme:%d,Mime:%d,Flags:%d,Err:%d,Ud:%d,PostLen:%llu\n",
	conn->parser_http_major,
	conn->parser_http_minor,
	conn->parser_status_code,
	conn->parser_method,
	conn->parser_scheme,
	conn->parser_mime,
	conn->parser_flags,
	conn->parser_http_errno,
	conn->parser_upgrade,
	conn->parser_content_length);
  	# endif
//--------------------------------------------------------------------------------------------------
  // is it an request ? Then do preprocessing ...
  if (conn->parser_type == HTTP_REQUEST)
	{
	// Authenticate remote for request
	SCDED_ProcHdrFldAuth(conn);

	// Analyze the path from HTTP-Parser and try to find + load a matching build in Resource
	HTTPD_ParseUrl(conn);
	}
//--------------------------------------------------------------------------------------------------

  return 0;
}



//##################################################################################################
//### FName: HTTPDFndBodyDataCb
//###  Desc: Callback fired by HTTPDParser when parser has found body-data
//###  Para: WebIf_HTTPDConnSlotData_t *conn ->
//###        const char *at, -> ptr to body data (to add)
//###        size_t length -> data length of body data (to add)
//###  Rets: Callbacks must return 0 on success. Returning a non-zero value indicates error to the
//###        parser, making it exit immediately.
//##################################################################################################
int ICACHE_FLASH_ATTR
HTTPDFndBodyDataCb(WebIf_HTTPDConnSlotData_t *conn,
			const char *at,
			size_t length)
{
  // set current slot-activity status
  conn->SlotParserState = s_HTTPD_Fnd_Body_Data;
//--------------------------------------------------------------------------------------------------

  // check BodyBuff for max allowed data length
  if (conn->postBuff == NULL)
	{
	if (length > MXBODYLEN) return 1; // error
	}
  else
	{
	if (conn->postLen + length > MXBODYLEN) return 1; // error
	}

  // store body data
  conn->postBuff = HTTPDStoreAddDataB(conn->postBuff,&conn->postLen,(const uint8_t*) at,length);

  # if SCDED_DBG >= 4
  printf("|FndBdyDataCb,len:%d,nread:%u,cont-len:%llu  Now Buffered:%d>",
	length,
	(unsigned int) conn->parser_nread,
	conn->parser_content_length,
	conn->postLen);
  # endif

//--------------------------------------------------------------------------------------------------
  return 0;
}


//##################################################################################################
//### FName: HTTPDMsgPrsdCompleteCb
//###  Desc: Callback fired by HTTPDParser when message parsing is complete (after body data)
//###  Para: WebIf_HTTPDConnSlotData_t *conn ->
//###  Rets: Callbacks must return 0 on success. Returning a non-zero value indicates error to the
//###        parser, making it exit immediately.
//##################################################################################################
int ICACHE_FLASH_ATTR
HTTPDMsgPrsdCompleteCb(WebIf_HTTPDConnSlotData_t *conn)
{
  # if SCDED_DBG >= 4
  printf("MsgPrsdComplCb>");
  # endif

  // set current slot-activity status
  conn->SlotParserState = s_HTTPD_Msg_Prsd_Complete;
//--------------------------------------------------------------------------------------------------

  return 0;
}













/*


  // Prepare memory for post buffer (max size = MAX_POST)
  conn->postBuff = (char*)malloc(length+1);
  conn->postPos = 0;

  // copy data
  memcpy(conn->postBuff, at, length);
  conn->postBuff[length] = '\0';	// zero terminate
*/
/*
 // Parse header field Content Length
  else if (strncmp(h, "Content-Length: ", 16) == 0)
	{
	i = 0;
	while (h[i] != ' ') i++;


	int PLen = atoi(h+i+1);     
	conn->postLen = PLen;
	if (PLen > MAX_POST) PLen = MAX_POST;

	# if SCDED_DBG >= 4
	printf("HTTPD Content-Len: %d\n", PLen);
	#endif

	// Prepare post buffer (max size = MAX_POST)
	conn->postBuff = (char*)malloc(PLen+1);
	conn->priv->postPos = 0;

  conn->postBuff = (char *) at;
  conn->postBuff[length] = '\0';	// zero terminate
  conn->postLen = length;
*/

/*
  # if SCDED_DBG >= 4
  printf("|BdyCb,len:%d,nread:%u,cont-len:%llu>",
	length,
	(unsigned int) conn->parser_nread,
	conn->parser_content_length);
  #endif

  return 0;
}

*/

/*
HTTPD Parsed: V1.1,StCd:0,Method:3,Flags:130,Err:0,Ud:0,PostLen:28
|No-Auth>
HTTPD URI prs fnd PATH:"/S0.jso", DocMimeBit is:3
S:0,@ 0 ?->/|ERR!
S:0,@ 1 ?->/|ERR!
S:0,@ 2 ?->/|ERR!
S:0,@ 3 ?->/|ERR!
S:0,@ 4 ?->/S0S:1,.MOKS:2,-ENA!S:3,,ap:8 xx:-2
S:4,S:5,|P57-H0>|P61-H0>|BdyCb,len:28,nread:0,cont-len:0>|P63-H0>X0
X1
X2
HTTPD Conn 0x3fff5310, slot 0, Exec cgi at 0x402293e4 with RPCArg: 0x00000000
findArg: TIST=1460300394&fS0=1&lS0=10
*/





//##################################################################################################
//### FName: HTTPDStoreAddData
//###  Desc: Stores data (allocates a buffer for it), or adds data to allocated Buffer (with realloc)
//###        !! Dont forget to dealloc !!
//###  Para: char *Buff -> ptr to allocated space (NULL if empty)
//###        const char *at, -> ptr to data (to add) (not zero terminated!)
//###        size_t length -> data length of data (to add)
//###  Rets: char *Buff -> ptr to allocated space
//##################################################################################################
char * ICACHE_FLASH_ATTR
HTTPDStoreAddData(char *Buff,
			const uint8_t *data,
			size_t length)
{
  // determinate Buff len
  int BuffLen;
  if (Buff == NULL) BuffLen = 0;
  else BuffLen = strlen(Buff);

  // determinate new Buff len
  int NewBuffLen;
  if (Buff == NULL) NewBuffLen = length;
  else NewBuffLen =

		BuffLen + length;

  // alloc new Buff
  char *NewBuff = (char*)malloc(NewBuffLen+1);

  // Copy old Buff to new Buff, and dealloc old
  if (Buff != NULL)
	{
	memcpy (NewBuff, Buff, BuffLen); // copy to sendbuf
	free(Buff);
	}

  // Add new data to TrailingBuff and save
  memcpy (NewBuff + BuffLen, data, length); // copy to sendbuf

  // zero terminate, func is for string only !
  NewBuff[NewBuffLen] = '\0';

  return NewBuff;
}



//##################################################################################################
//### FName: HTTPDStoreAddData
//###  Desc: Stores data (allocates a buffer for it), or adds data to allocated Buffer (with realloc)
//###        !! Dont forget to dealloc !!
//###  Para: char *Buff -> ptr to allocated space (NULL if empty)
//###        const char *at, -> ptr to data (to add) (not zero terminated!)
//###        size_t length -> data length of data (to add)
//###  Rets: char *Buff -> ptr to allocated space
//##################################################################################################
char * ICACHE_FLASH_ATTR
HTTPDStoreAddDataB(char *Buff,
			int *Len,
			const uint8_t *data,
			size_t length)
{
  // determinate New Buff Len
  int NewBuffLen;
  if (Buff == NULL) NewBuffLen = length;
  else NewBuffLen =
		Len[0] + length;

  // alloc New Buff
  char *NewBuff = (char*)malloc(NewBuffLen+1);

  // Copy old Buff to New Buff, and dealloc old
  if (Buff != NULL)
	{
	memcpy (NewBuff, Buff, Len[0]);
	free(Buff);
	}

  // Add new data to New Buff
  memcpy (NewBuff + Len[0], data, length);

  // better zero terminate, may be a string !
  NewBuff[NewBuffLen] = '\0';

  // Store new len
  Len[0] = NewBuffLen;

  return NewBuff;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SCDED_build_select_input_A
 * Desc: Creates %token% replacement chars - HTML Select Element - (Type A)
 * Para: -/-
 *       char *buff -> write buffer
 *       SelectAData *SAD -> id/desc translation table 
 *       uint8 SelID -> current selected ID
 *       char *name = name of the select element
 *       char *id = id of the select element
 *       char *label -> build <label> element, or not (NULL)
 * Rets: int -> length of data written to buffer
 *--------------------------------------------------------------------------------------------------
 */
/* sample data - keep buffer size in mind !!!
<label>Authentication Method:
<select name="WSAP_Authentication_Method" id="E" onchange="AJAXPost('E')">
<option value="OPEN">Open</option>
<option value="WEP">WEP</option>
<option value="WPA_PSK">WPA PSK</option>
<option value="WPA2_PSK">WPA2 PSK</option>
<option value="WPA_WPA2_PSK">WPA WPA2 PSK</option>
<option value="MAX">MAX</option>
</select>
</label>
*/
int ICACHE_FLASH_ATTR
SCDED_build_select_input_A(char *buff
			,SelectAData *SAD
			,uint8_t SelID
			,char *name
			,char *id
			,char *label) 
  {

  int i=0;
  int len=0;

  // optional: build <label>
  if (label != NULL) len +=
	sprintf(buff + len, "<label>%s\n", label);

  // build <select> leadin
  len += sprintf(buff + len
	,"<select name=\"%s\" id=\"%s\" onchange=\"AJAXPost('%s')\">\n"
	,name, id, id);

  // cycle: build options
  while (SAD[i].Desc != NULL)

	{

	if (SelID != SAD[i].ID) len += sprintf(buff + len
		,"<option value=\"%s\">%s</option>\n"
		,SAD[i].Desc
		,SAD[i].Desc);

	else len += sprintf(buff + len
		,"<option value=\"%s\" selected>%s</option>\n"
		,SAD[i].Desc
		,SAD[i].Desc);

	i++;

	}

  // finnish </select>
  len += sprintf(buff + len,
	"</select>\n");

  // optional: finnish </label>
  if (label != NULL) len +=
	sprintf(buff + len, "</label>\n");

  #if SCDED_DBG >= 4
  printf("SelA:Fits %d in buff?\n"
	,len);
  #endif

  return len;

  }



//##################################################################################################
//### FName: SelectA
//###  Desc: Creates HTML Select Element
//###  Para: char *CGI = CGI name; CGI != NULL -> creates Form
//###        uint8 SelID = current selected ID
//###        SelectAData *SAD = pointer to Data
//###        char *buff = buffer to write to
//###  	     char *Name = Name of the Select Element -> When Name="X*"=> auto submit !
//###  Rets: buffer with created data
//##################################################################################################
/* sample data - keep buffer size in mind !!!
<select id="leaveCode" name="leaveCode">
  <option value="0">testA</option>
  <option value="1">testB</option>
  <option value="2" selected>testC</option>
  <option value="3">testD</option>
</select>
*/
void ICACHE_FLASH_ATTR
SelectA(char *buff, SelectAData *SAD, uint8_t SelID, char *Name, char *CGI) 
{
  int i=0;
  int len=0;
  if (CGI != NULL) len += sprintf(buff+len, "<form method=\"post\" action=\"%s\">\n",CGI); // create Form ?!
  if (Name[0] == 'X') len += sprintf(buff+len, "<select name=\"%s\" onChange=\"this.form.submit()\">\n",Name); //instant Post by Java-Script
  else len += sprintf(buff+len, "<select name=\"%s\">",Name);	//no instant Post by Java-Script
  // cycle Table
  while (SAD[i].Desc != NULL)
	{
	if (SelID != SAD[i].ID) len += sprintf(buff+len, "<option value=\"%d\">%s</option>\n",SAD[i].ID,SAD[i].Desc);
	else len += sprintf(buff+len, "<option value=\"%d\" selected>%s</option>\n",SAD[i].ID,SAD[i].Desc);
	i++;
	}
  len += sprintf(buff+len, "</select>\n");
  if (CGI != NULL) len += sprintf(buff+len, "</form>\n");	// created Form ?!
  # if SCDED_DBG >= 4
  printf("SelA:Fits %d in buff?\n",len);
  #endif
}



//##################################################################################################
//### FName: RadioA
//###  Desc: Creates HTML input type radio Element
//###  Para: char *CGI = CGI name; CGI != NULL -> creates Form
//###        uint8 SelID = current selected ID
//###        SelectAData *SAD = pointer to Data
//###        char *buff = buffer to write to
//###  	     char *Name = Name of the Element -> When Name="X*"then auto submit !
//###  Rets: buffer with created data
//##################################################################################################
/* sample data - keep buffer size in mind !!!
<form method="post" action="genattr.cgi">
<input type="radio" name="X2" value="1" onChange="this.form.submit()">aaaaY<br>
<input type="radio" name="X2" value="2" onChange="this.form.submit()">bbY<br>
<input type="radio" name="X2" value="3" checked="checked" onChange="this.form.submit()">ccccccY<br>
<input type="radio" name="X2" value="4" onChange="this.form.submit()">ddddddY<br>
<input type="radio" name="X2" value="5" onChange="this.form.submit()">eeeeY<br>
</form>
*/
void ICACHE_FLASH_ATTR
RadioA(char *buff, SelectAData *SAD, uint8_t SelID, char *Name, char *CGI) 
{
  int i=0;
  int len=0;
 if (CGI != NULL) len += sprintf(buff+len, "<form method=\"post\" action=\"%s\">\n",CGI); // create Form ?!
  // cycle Table
  while (SAD[i].Desc != NULL)
	{
	len += sprintf(buff+len, "<input type=\"radio\" name=\"%s\" value=\"%d\"",Name,SAD[i].ID);
	if (SelID == SAD[i].ID) len += sprintf(buff+len," checked=\"checked\"");			// Checked?
	if (Name[0] == 'X') len += sprintf(buff+len," onChange=\"this.form.submit()\"");		// Auto Transmit Form?
	len += sprintf(buff+len,">%s<br>\n",SAD[i].Desc);
	i++;
	}
  if (CGI != NULL) len += sprintf(buff+len, "</form>\n");	// created Form ?!
  # if SCDED_DBG >= 4
  printf("SelA:Fits %d in buff?\n",len);
  #endif
}



//##################################################################################################
//### FName: CheckboxA
//###  Desc: Creates HTML input type checkbox Element
//###  Para: char *CGI = CGI name; CGI != NULL -> creates Form
//###        uint8 SelID = current selected ID
//###        SelectAData *SAD = pointer to Data
//###        char *buff = buffer to write to
//###        char *Name = Name of the Element -> When Name="X*" then auto submit !
//###  Rets: buffer with created data
//##################################################################################################
/* sample data - keep buffer size in mind !!!
<form method="post" action="genattr.cgi">
<input type="checkbox" name="X1" value="1" checked="checked" onChange="this.form.submit()">aaaaa<br>
<input type="checkbox" name="X1" value="2" checked="checked" onChange="this.form.submit()">bbb<br>
<input type="checkbox" name="X1" value="4" onChange="this.form.submit()">ccccccc<br>
<input type="checkbox" name="X1" value="8" onChange="this.form.submit()">ddddddd<br>
<input type="checkbox" name="X1" value="16" onChange="this.form.submit()">eeeee<br>
</form>

*/
void ICACHE_FLASH_ATTR
CheckboxA(char *buff, SelectAData *SAD, uint8_t SelID, char *Name, char *CGI) // SelID= 8 Bit for 8 Choices !!!
{
  int i=0;
  int len=0;
  if (CGI != NULL) len += sprintf(buff+len, "<form method=\"post\" action=\"%s\">\n",CGI); // create Form ?!
  len += sprintf(buff+len, "<input type=\"hidden\" name=\"%s\" value=\"0\">\n",Name);	// trick to send FORM when nothing is checked
  // cycle Table
  while ( (SAD[i].Desc != NULL) & (i<8) )
	{
	len += sprintf(buff+len, "<input type=\"checkbox\" name=\"%s\" value=\"%d\"",Name,SAD[i].ID);
	if (SelID & (1 << i) ) len += sprintf(buff+len," checked=\"checked\"");			// Checked?
	if (Name[0] == 'X') len += sprintf(buff+len," onChange=\"this.form.submit()\"");		// Auto Transmit Form?
	len += sprintf(buff+len,">%s<br>\n",SAD[i].Desc);
	i++;
	}
  if (CGI != NULL) len += sprintf(buff+len, "</form>\n");	// created Form ?!
  # if SCDED_DBG >= 4
  printf("SelA:Fits %d in buff?\n",len);
  #endif
}


















































/**
 * ---------------------------------------------------------------------------------------------------
 *  DName: WebIf_Provided
 *  Desc: Data 'Provided By Module' for the Web IF module (functions + infos this module provides SCDE)
 *  Data:
 * ---------------------------------------------------------------------------------------------------
 */
WebIf_Provided_t WebIf_Provided = {

  NULL	//,WebIf_DirectWrite	// DirectWrite

  };




/**
 * ---------------------------------------------------------------------------------------------------
 *  DName: ModuleWebIf
 *  Desc: Data 'Provided By Module' for the Web IF module (functions + infos this module provides SCDE)
 *  Data:
 * ---------------------------------------------------------------------------------------------------
 */
const ProvidedByModule_t WebIf_ProvidedByModule = { // A-Z order
   "WebIf"			  	// Type-Name of module -> on Linux libfilename.so !
  ,5					// size of Type-Name

  ,WebIf_Add				// Add
  ,NULL					// Attribute
  ,WebIf_Define				// Define
  ,NULL					// Delete
  ,WebIf_Direct_Read			// Direct_Read
  ,WebIf_Direct_Write			// Direct_Write
  ,NULL					// Except
  ,NULL					// Get
  ,WebIf_IdleCbX			// IdleCb
  ,WebIf_Initialize			// Initialize
  ,NULL					// Notify
  ,NULL					// Parse
  ,NULL					// Read
  ,NULL					// Ready
  ,NULL					// Rename
  ,NULL	//WebIf_Set			// Set
  ,NULL					// Shutdown
  ,NULL					// State
  ,WebIf_Sub				// Sub
  ,WebIf_Undefine			// Undefine
  ,NULL					// Write
  ,NULL					// FnProvided
  ,sizeof(WebIf_Definition_t)		// Modul specific Size (Common_Definition_t + X)
};



/**
 * --------------------------------------------------------------------------------------------------
 *  FName: WebIf_Add
 *  Desc: Processes the device-specific command line arguments from the set command
 *  Info: Invoked by cmd-line 'Set Telnet_Definition.common.Name setArgs'
 *  Para: Telnet_Definition_t *Telnet_Definition -> WebIF Definition that should get a set cmd
 *        uint8_t *setArgs -> the setArgs
 *        size_t setArgsLen -> length of the setArgs
 *  Rets: char * -> response text in allocated memory, NULL=no text
 * --------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* 
WebIf_Add(Common_Definition_t* Common_Definition
	,uint8_t *kvArgs
	,size_t kvArgsLen)
  {

  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  #if SCDEH_DBG >= 5
  printf("\n|WebIf_Add, Name:%.*s, got key value args:%.*s>"
	,Common_Definition->nameLen
	,Common_Definition->name
	,kvArgsLen
	,kvArgs);
  #endif

  // alloc mem for retMsg
  retMsg = malloc(sizeof(strTextMultiple_t));

  // response with error text
  retMsg->strTextLen = asprintf(&retMsg->strText
	,"WebIf_Add, Name:%.*s, got key value args:%.*s"
	,Common_Definition->nameLen
	,Common_Definition->name
	,kvArgsLen
	,kvArgs);

  return retMsg;

  }



/*
 * --------------------------------------------------------------------------------------------------
 *  FName: WebIf_IdleCb	
 *  Desc: Finalizes the defines a new "device" of 'WebIf' type. Contains devicespecific init code.
 *  Info: 
 *  Para: WebIf_Definition_t *WebIf_Definition -> prefilled WebIf Definition
 *        char *Definition -> the last part of the CommandDefine arg* 
 *  Rets: char* -> response text NULL=no text
 * --------------------------------------------------------------------------------------------------
 */
int //feedModuleTask
WebIf_IdleCbX(Common_Definition_t *Common_Definition)
  {

  #if SCDEH_DBG >= 5
  printf("\n|WebIf_IdleCb Entering..., Name:%.*s>"
	,Common_Definition->nameLen
	,Common_Definition->name); //(char*)
  #endif

  // make common ptr to modul specific ptr
  WebIf_Definition_t* WebIf_Definition = (WebIf_Definition_t*) Common_Definition;


/*
	// do idle callback if Slot is
	// +  allocated
        // + has a conn (!= NULL)
	// + F_GENERATE_IDLE_CALLBACK is set in ConnCtrl ...
	if ( (ConnSlots[i] != NULL) &&
		(ConnSlots[i]->conn != NULL) && //überflüssig ?
		(ConnSlots[i]->ConnCtrlFlags & F_GENERATE_IDLE_CALLBACK) )

		{

		// call it with current connection
		WebIf_IdleCb(ConnSlots[i]->conn);

		}
*/

 if (WebIf_Definition->reverse) {

  // get assigned HTTPD-Connection-Slot-Data
  WebIf_HTTPDConnSlotData_t *conn
	= WebIf_Definition->reverse;

  // execute disconnection (indicated by NEEDS_CLOSE flag) or send more data ...
  if (conn->ConnCtrlFlags & F_GENERATE_IDLE_CALLBACK)

	{

	// execute Idle Callback
	WebIf_IdleCb(WebIf_Definition);

	}

}

  return 0;

  }













/*
 * --------------------------------------------------------------------------------------------------
 *  FName: WebIf_sent (espconn_sent compatible Fn)
 *  Desc: Platform conn - Send data Fn
 *  Para: WebIf_Definition_t *WebIf_Definition -> WebIf Definition
 *        uint8_t *Buff -> buffer with data to send
 *        uint Len -> length of data to send
 *  Rets: int -> written data
 * --------------------------------------------------------------------------------------------------
 */
int ICACHE_FLASH_ATTR
WebIf_sent(WebIf_Definition_t *WebIf_Definition
	,uint8_t *Buff
	,uint Len)
{
  # if SCDED_DBG >= 5
  os_printf("\n|WebIf_Sent len:%d!>"
	,Len);
  # endif

  // select for want writing (F_WANTS_WRITE), because maybe we have more data to send ...
  WebIf_Definition->common.Common_CtrlRegA |= F_WANTS_WRITE;

  return (write(WebIf_Definition->common.fd, Buff, Len) >= 0);
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: WebIf_disconnect (espconn_disconnect compatible Fn)
 * Desc: Platform conn - Disconnect Fn
 * Info: 
 * Para: WebIf_Definition_t *WebIf_Definition -> WebIf Definition
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR
WebIf_disconnect(WebIf_Definition_t *WebIf_Definition)
{
  // select for disconnecting (F_NEEDS_CLOSE)
  WebIf_Definition->WebIf_CtrlRegA |= F_NEEDS_CLOSE;

  // select for want writing (F_WANTS_WRITE), because the real close is done in the write select of code
  WebIf_Definition->common.Common_CtrlRegA |= F_WANTS_WRITE;
}













/*
 *--------------------------------------------------------------------------------------------------
 *FName: WebIf_UndefineRaw
 * Desc: internal cleanup and WebIf_Definition delete
 * Para: WebIf_Definition_t *WebIf_Definition -> WebIf Definition that should be deleted
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
int 
WebIf_UndefineRaw(WebIf_Definition_t* WebIf_Definition)
{
  // connection close
  close(WebIf_Definition->common.fd);

  // FD is gone ...
//WebIf_Definition->common.FD = -1;

// --- dealloc? non master conns?


  // remove WebIf Definition
  STAILQ_REMOVE(&SCDERoot_at_WebIf_M->HeadCommon_Definitions
	,(Common_Definition_t*) WebIf_Definition
	,Common_Definition_s
	,entries);

  // mark slot as unused
  WebIf_Definition->HTTPD_InstCfg->SlotCtrlRegBF &= 
	~(1<<WebIf_Definition->SlotNo);

  // free Name
  free(WebIf_Definition->common.name);

  // free TCP struct
  free(WebIf_Definition->proto.tcp);

  // free WebIf_Definition
  free(WebIf_Definition);

  return 0;
}

























/*
 *--------------------------------------------------------------------------------------------------
 *FName: WebIf_Define
 * Desc: Finalizes the defines a new "device" of 'WebIf' type. Contains devicespecific init code.
 * Info: 
 * Para: WebIf_Definition_t *WebIf_Definition -> prefilled WebIf Definition
 *       char *Definition -> the last part of the CommandDefine arg* 
 * Rets: strTextMultiple_t* -> response text NULL=no text
 *--------------------------------------------------------------------------------------------------
 */
strTextMultiple_t*
WebIf_Define(Common_Definition_t *Common_Definition)//, const char *Definition)
  {

  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // make common ptr to modul specific ptr
	WebIf_Definition_t* WebIf_Definition =
		  (WebIf_Definition_t*) Common_Definition;

  #if SCDEH_DBG >= 5
  printf("\n|WebIf_Def, Def:%.*s>"
	,WebIf_Definition->common.definitionLen
	,WebIf_Definition->common.definition);
  #endif

//---

  // alloc memory for the HTTPD-Instance-Configuration
  WebIf_Definition->HTTPD_InstCfg = (HTTPD_InstanceCfg_t*) malloc(sizeof(HTTPD_InstanceCfg_t));

  // memclr the HTTPD-Instance-Configuration
  memset(WebIf_Definition->HTTPD_InstCfg, 0, sizeof (HTTPD_InstanceCfg_t));

  // HTTPD-Instance-Configuration -> set content
  WebIf_Definition->HTTPD_InstCfg->BuiltInUrls = (WebIf_ActiveResourcesDataA_t *) &BuiltInUrlsX;
  WebIf_Definition->HTTPD_InstCfg->BuiltInActiveResources = (WebIf_ActiveResourcesDataB_t *) &BuiltInActiveResourcesX;

  // HTTPD-Instance-Configuration -> Set HTTPD Parser Fn callbacks
  http_parser_settings_t* HTTPDparser_settings = &WebIf_Definition->HTTPD_InstCfg->HTTPDparser_settings;

  // doppelt ?? HTTPD-Instance-Configuration -> assign the HTTPD Parser the settings
  http_parser_settings_init(HTTPDparser_settings);

  HTTPDparser_settings->on_message_begin = HTTPDMsgBeginCb;
  HTTPDparser_settings->on_url = HTTPDUrlFoundCb;
//HTTPDparser_settings->on_status ???
  HTTPDparser_settings->on_header_field = HTTPDHeaderFldCb;
  HTTPDparser_settings->on_header_value = HTTPDHeaderValCb;
  HTTPDparser_settings->on_headers_complete = HTTPDHeaderPrsdCompleteCb;
  HTTPDparser_settings->on_body = HTTPDFndBodyDataCb;
  HTTPDparser_settings->on_message_complete = HTTPDMsgPrsdCompleteCb;
//HTTPDparser_settings->on_chunk_header ???
//HTTPDparser_settings->on_chunk_complete ???


  // ?? how to init !!!!!!!!!!!
//WebIf_Definition->HTTPD_InstCfg->SCDED_DirConEnaCtrl = 0; cleared by memset ...

  // reset Load-Serializer-Bitfield -> no heavy load tasks now!
//WebIf_Definition->HTTPD_InstCfg->LoadSerializer = 0; cleared by memset ...

  //reset Slot Control Register Bitfield -> no connections yet!
//WebIf_Definition->HTTPD_InstCfg->SlotCtrlRegBF = 0; cleared by memset ...

//--------------------------------------------------------------------------------------------------

  // mark this as the server-socket
  WebIf_Definition->WebIf_CtrlRegA |= F_THIS_IS_SERVERSOCKET;



 // later from definition
  int Port = 80;

  // ???
  int opt = true;

  int ret;

  // master socket or listening fd
  int listenfd;

  // server address structure
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_len = sizeof(server_addr);
  server_addr.sin_port = htons(Port);

  // Create socket for incoming connections
  do {

	listenfd = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);

	// socked created or error?
	if (listenfd < 0)

		{

		SCDEFn_at_WebIf_M->Log3Fn(WebIf_Definition->common.name
				,WebIf_Definition->common.nameLen
				,1
				,"WebIf_Define ERROR: failed to create sock! retriing\n");

		vTaskDelay(1000/portTICK_RATE_MS);

		}

	} while(listenfd < 0);


  // set master socket to allow multiple connections , this is just a good habit, it will work without this
  ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt) );
  if (ret < 0 )

	{

	SCDEFn_at_WebIf_M->Log3Fn(WebIf_Definition->common.name
			,WebIf_Definition->common.nameLen
			,1
			,"WebIf_Define ERROR: 'setsockopt' failed! error:%d\n"
			,ret);

	}

  // bind the socket to the local port
  do {

	ret = bind(listenfd, (struct sockaddr *) &server_addr, sizeof(server_addr));

	if (ret != 0)

		{

		SCDEFn_at_WebIf_M->Log3Fn(WebIf_Definition->common.name
				,WebIf_Definition->common.nameLen
				,1
				,"WebIf_Define ERROR: 'bind' failed! retriing\n");

		vTaskDelay(1000/portTICK_RATE_MS);
		}

	} while(ret != 0);


//  # if SCDED_DBG >= 3
 /* printf("|local at:%u.%u.%u.%u:%u,port:%u>"
	,(uint8_t*) &server_addr.sin_addr.s_addr
	,(uint8_t*) &server_addr.sin_addr.s_addr+1
	,(uint8_t*) &server_addr.sin_addr.s_addr+2
	,(uint8_t*) &server_addr.sin_addr.s_addr+3
	,(uint16_t*) &server_addr.sin_port);*/

//  #endif




#define HTTPD_MAX_CONNECTIONS 16

  // listen to the local port
  do	{

	ret = listen(listenfd, HTTPD_MAX_CONNECTIONS);

	if (ret != 0)

		{

		SCDEFn_at_WebIf_M->Log3Fn(WebIf_Definition->common.name
				,WebIf_Definition->common.nameLen
				,1
				,"WebIf_Define ERROR: 'listen' failed! retriing\n");

		vTaskDelay(1000/portTICK_RATE_MS);
		}

	} while(ret != 0);

  // store FD to Definition. Will than be processed in global loop ...
  WebIf_Definition->common.fd = listenfd;

  // using TCP, create, fill and store struct
  esp_tcp *tcp = malloc (sizeof(esp_tcp));
//  tcp->local_ip = server_addr.sin_addr.s_addr;
  tcp->local_port = server_addr.sin_port;
  WebIf_Definition->proto.tcp = tcp;

  WebIf_Definition->type = ESPCONN_TCP;
  WebIf_Definition->state = ESPCONN_NONE;

  // register the connect-callback - used ...
  espconn_regist_connectcb(WebIf_Definition,
	WebIf_ConnCb);

  SCDEFn_at_WebIf_M->Log3Fn(WebIf_Definition->common.name
		  ,WebIf_Definition->common.nameLen
		  ,1
		  ,"Defined a WebIf at X.X.X.X:YYYY, FD is:%d\n"
		  ,listenfd);

  return retMsg;
}



/*
 * -------------------------------------------------------------------------------------------------
 *  FName: WebIf_DirectRead
 *  Desc: Called from the global select-loop when FD is in read-set
 *  Info: 
 *  Para: Common_Definition_t* p_entry_definition -> the FD owners definition
 *  Rets: ? - unused
 * -------------------------------------------------------------------------------------------------
 */
int 
WebIf_Direct_Read(Common_Definition_t* p_entry_definition)
{
  // make common ptr to modul specific ptr
  WebIf_Definition_t* p_entry_webif_definition = 
	(WebIf_Definition_t*) p_entry_definition;

// -------------------------------------------------------------------------------------------------

  #if WebIf_Module_DBG >= 7
  SCDEFn_at_WebIf_M->Log3Fn(p_entry_webif_definition->common.name,
	p_entry_webif_definition->common.nameLen,
	7,
	"Direct Read Fn (module '%.*s'), entering.",
	p_entry_webif_definition->common.module->provided->typeNameLen,
	p_entry_webif_definition->common.module->provided->typeName);
  #endif

// ------------------------------------------------------------------------------------------------

  // ptr to receive-buffer
  char* recv_buffer;

  int32_t len;

  // FD of the new client connection
  int32_t NewClientFD;

  // sockaddr_in, to get info about new client, contains port, ip-addr, family
  struct sockaddr_in remote_addr;
  struct sockaddr_in local_addr;
  
  // sockaddr, to get info about new client
  struct sockaddr peerName;

  // sockaddr, to get info about server
  struct sockaddr sockName;

// -------------------------------------------------------------------------------------------------

  // Check Flag 'THIS_IS_SERVERSOCKET' in WebIf_CtrlRegA. This indicates a server-socket.
  // -> Manage a new connection.
  if (p_entry_webif_definition->WebIf_CtrlRegA & F_THIS_IS_SERVERSOCKET) {

// -------------------------------------------------------------------------------------------------

	// check slot availiability, get a slot no. or RETURN, mark slot as 'in use'
	uint32_t SlotCtrlRegBF = 
		p_entry_webif_definition->HTTPD_InstCfg->SlotCtrlRegBF;

	uint8_t NewSlotNo;

	// MAX_SLOTS_PER_INSTANCE -> uint32_t BF used in code -> 32 64?
	for ( NewSlotNo = 0 ; NewSlotNo < 32 ; NewSlotNo++ ) {

		if ( ! ( SlotCtrlRegBF & ( 0b1 << NewSlotNo ) ))
			break;
	}

	// Check if we got a free slot? -> 'no slots free' error
	if ( NewSlotNo >= 32 ) {

		SCDEFn_at_WebIf_M->Log3Fn(p_entry_webif_definition->common.name
			,p_entry_webif_definition->common.nameLen
			,1
			,"WebIf_DirectRead Error no slots free...\n");

		// do not accept new connection
		return 0;//??
	}

	// mark found slot as used in Slot-Control-Register-Bitfield
	SlotCtrlRegBF |= ( 0b1 << NewSlotNo );

	// store Slot-Control-Register-Bitfield
	p_entry_webif_definition->HTTPD_InstCfg->SlotCtrlRegBF = SlotCtrlRegBF;

// -------------------------------------------------------------------------------------------------

	#if SCDEH_DBG >= 5
	printf("|accept a new conn>");
	#endif

	len = sizeof(struct sockaddr_in);

	// get FD from new connection and store remote address
	NewClientFD = accept(p_entry_webif_definition->common.fd,
		(struct sockaddr *) &remote_addr, (socklen_t *) &len);

	// check for error
	if ( NewClientFD < 0 ) {

		SCDEFn_at_WebIf_M->Log3Fn(p_entry_webif_definition->common.name
				,p_entry_webif_definition->common.nameLen
				,1
				,"WebIf_DirectRead Error! accept failed...\n");

		// error, process next slot?
		return 0;//??
	}

// -------------------------------------------------------------------------------------------------

	// create a new WebIf Definition
	WebIf_Definition_t* p_new_entry_webif_definition;

	// alloc mem for modul specific definition structure (Common_Definition_t + X)
	p_new_entry_webif_definition = 
		(WebIf_Definition_t*) malloc(sizeof(WebIf_Definition_t));

	// zero the struct
	memset(p_new_entry_webif_definition, 0, sizeof (WebIf_Definition_t));

// -------------------------------------------------------------------------------------------------
				
	// set parameters for new connection 
	int keepAlive = 1;	//enable keepalive
 	int keepIdle = 60;	//60s
 	int keepInterval = 5;	//5s
 	int keepCount = 3;	//retry times

 	setsockopt(NewClientFD, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepAlive, sizeof(keepAlive));
  	setsockopt(NewClientFD, IPPROTO_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle));
  	setsockopt(NewClientFD, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval));
  	setsockopt(NewClientFD, IPPROTO_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));
			
 	// store clients FD to new WebIf Definition
 	p_new_entry_webif_definition->common.fd = NewClientFD;

	// copy link to HTTPD-Instance-Configuration
	p_new_entry_webif_definition->HTTPD_InstCfg
		= p_entry_webif_definition->HTTPD_InstCfg;

	// copy ptr to associated module (this module)
	p_new_entry_webif_definition->common.module
		= p_entry_webif_definition->common.module;

	// store Slot-Number
	p_new_entry_webif_definition->SlotNo = NewSlotNo;

	// clear Flag 'WANTS_WRITE' in new WebIf Definition
 	p_new_entry_webif_definition->common.Common_CtrlRegA
		&= ~F_WANTS_WRITE;

	// clear Flag 'NEEDS_CLOSE' in new WebIf Definition
 	p_new_entry_webif_definition->WebIf_CtrlRegA
		&= ~F_NEEDS_CLOSE;

	// get info about new client-conn (port, ip, ...)	
 	socklen_t peerSockAddrLen = sizeof(peerName);
  	getpeername(NewClientFD, &peerName, (socklen_t *) &peerSockAddrLen);
  	struct sockaddr_in *peerSockAddr_in = (struct sockaddr_in *) &peerName;

	// get info about our server-conn (port, ip, ...)	
 	socklen_t sockSockAddrLen = sizeof(sockName);
  	getsockname(NewClientFD, &sockName, (socklen_t *) &sockSockAddrLen);
  	struct sockaddr_in *sockSockAddr_in = (struct sockaddr_in *) &sockName;

	// using TCP, create struct
	esp_tcp *tcp = (esp_tcp*) malloc (sizeof(esp_tcp));

	// using TCP, fill struct
	tcp->local_port = ntohs(sockSockAddr_in->sin_port);	// server port
	tcp->remote_port = ntohs(peerSockAddr_in->sin_port);	// client port
	memcpy(&tcp->remote_ip				// client IP
		, &peerSockAddr_in->sin_addr.s_addr
		, sizeof(tcp->remote_ip));

	// store TCP struct
	p_new_entry_webif_definition->proto.tcp = tcp;

	// give definition a new unique name
	p_new_entry_webif_definition->common.nameLen = asprintf((char**)&p_new_entry_webif_definition->common.name
		,"%.*s.%d.%d.%d.%d.%u"
		,p_entry_webif_definition->common.nameLen
		,p_entry_webif_definition->common.name
		,p_new_entry_webif_definition->proto.tcp->remote_ip[0]
		,p_new_entry_webif_definition->proto.tcp->remote_ip[1]
		,p_new_entry_webif_definition->proto.tcp->remote_ip[2]
		,p_new_entry_webif_definition->proto.tcp->remote_ip[3]
		,p_new_entry_webif_definition->proto.tcp->remote_port);

  	// assign an unique number
 	 p_new_entry_webif_definition->common.nr = SCDERoot_at_WebIf_M->device_count++;

  	// make this definition temporary
	p_new_entry_webif_definition->common.defCtrlRegA |= F_TEMPORARY;

	// store new WebIf Definition
	STAILQ_INSERT_HEAD(&SCDERoot_at_WebIf_M->HeadCommon_Definitions
		,(Common_Definition_t*) p_new_entry_webif_definition
		, entries);

	#if Telnet_Module_DBG >= 7
	SCDEFn_at_WebIf_M->Log3Fn(p_entry_webif_definition->common.name,
		p_entry_webif_definition->common.nameLen,
		7,
		"Direct Read Fn (module '%.*s'), F_THIS_IS_SERVERSOCKET set for "
		"this conn. Accepting a new conn. Using slot '%d', FD '%d'. "
		"Creating a 'definition' with F_TEMPORARY set. Exec Conn_Cb Fn",
		p_entry_webif_definition->common.module->provided->typeNameLen,
		p_entry_webif_definition->common.module->provided->typeName,
		p_new_entry_webif_definition->SlotNo,
		p_new_entry_webif_definition->common.fd);
	#endif

 	// execute WebIf Connect Callback to init
	WebIf_ConnCb(p_new_entry_webif_definition);
  }

// --------------------------------------------------------------------------------------------------

  // Flag 'THIS_IS_SERVERSOCKET' cleared. This indicates that this is not the server-socket.
  // -> Manage old connection.
  else	{

 	#if SCDEH_DBG >= 5
	printf("|recv>");
	#endif

	// malloc our receive buffer
	recv_buffer = (char*) malloc(RECV_BUF_SIZE);

	// got no buffer ? Close / Cleanup connection
	if ( recv_buffer == NULL ) {

		os_printf("platHttpServerTask: memory exhausted!\n");

		WebIf_DisconCb(p_entry_webif_definition);

		close(p_entry_webif_definition->common.fd);

		p_entry_webif_definition->common.fd = -1;
	}

	// receive the expected data
	int32_t recv_len = recv(p_entry_webif_definition->common.fd,
		recv_buffer, RECV_BUF_SIZE, 0);

	// process, if received data
	if ( recv_len > 0 ) {

		// execute Received Callback
		WebIf_RecvCb(p_entry_webif_definition, recv_buffer, recv_len);
	}

	// or has remote closed the connection ?
	else if ( recv_len == 0 ) {

		// execute Disconnect Callback
		WebIf_DisconCb(p_entry_webif_definition);

		// undefinde this p_entry_webif_definition
		WebIf_UndefineRaw(p_entry_webif_definition);
	}


	// else we got an error ...
	else {

		// execute Error Callback
		WebIf_ReconCb(p_entry_webif_definition, recv_len);

		// undefinde this p_entry_webif_definition
		WebIf_UndefineRaw(p_entry_webif_definition);
	}

 	// free our receive buffer
  	free(recv_buffer);
  }

  return 0;
}



/*
 * --------------------------------------------------------------------------------------------------
 *  FName: WebIf_DirectWrite
 *  Desc: Called from the global select-loop when FD is in write-set
 *  Info: But ONLY if Flag 'Want_Write' in Common_CtrlRegA is set !!!
 *  Para: Common_Definition_t* p_entry_definition -> the FD owners definition
 *  Rets: ? - unused
 * --------------------------------------------------------------------------------------------------
 */
int 
WebIf_Direct_Write(Common_Definition_t* p_entry_definition)
{
  // make common ptr to modul specific ptr
  WebIf_Definition_t* p_entry_webif_definition = 
	(WebIf_Definition_t*) p_entry_definition;

// -------------------------------------------------------------------------------------------------

  #if WebIf_Module_DBG >= 7
  SCDEFn_at_WebIf_M->Log3Fn(p_entry_webif_definition->common.name,
	p_entry_webif_definition->common.nameLen,
	7,
	"Direct Write Fn (module '%.*s'), entering.",
	p_entry_webif_definition->common.module->provided->typeNameLen,
	p_entry_webif_definition->common.module->provided->typeName);
  #endif

// ------------------------------------------------------------------------------------------------

  // clear Flag F_WANT_WRITE, will be set again when more data is send
//  p_entry_webif_definition->common.Common_CtrlRegA &= ~F_WANTS_WRITE;	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! in mainfn

// --------------------------------------------------------------------------------------------------

  // execute disconnection (indicated by NEEDS_CLOSE flag) or send more data ...
  if ( p_entry_webif_definition->WebIf_CtrlRegA & F_NEEDS_CLOSE ) {

	#if WebIf_Module_DBG >= 8
  	SCDEFn_at_WebIf_M->Log3Fn(p_entry_webif_definition->common.name,
		p_entry_webif_definition->common.nameLen,
		8,
		"Direct Write Fn (module '%.*s'), F_NEEDS_CLOSE set for "
		"this conn. Exec Disconn_Cb Fn and Undefine_Raw Fn.",
		p_entry_webif_definition->common.module->provided->typeNameLen,
		p_entry_webif_definition->common.module->provided->typeName);
 	 #endif

	// execute Disconnect Callback
	WebIf_DisconCb(p_entry_webif_definition);

	// undefinde this WebIf_Definition
	WebIf_UndefineRaw(p_entry_webif_definition);

	// definition gone here ...
  }

// --------------------------------------------------------------------------------------------------

  // chance to send more data by SendCb ...
  else	{

	#if WebIf_Module_DBG >= 8
  	SCDEFn_at_WebIf_M->Log3Fn(p_entry_webif_definition->common.name,
		p_entry_webif_definition->common.nameLen,
		8,
		"Direct Write Fn (module '%.*s'), this conn is ready to write, "
		"exec Send_Cb Fn. "
		p_entry_webif_definition->common.module->provided->typeNameLen,
		p_entry_webif_definition->common.module->provided->typeName);
 	 #endif

	// execute Sent Callback
	WebIf_SentCb(p_entry_webif_definition);
  }

  return 0;
}


/* rename
Disconn_Cb
Undefine_Raw

*/

/*
 * --------------------------------------------------------------------------------------------------
 *  FName: WebIf_Initialize
 *  Desc: Initializion of SCDE Function Callbacks of an new loaded module
 *  Info: Stores Module-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 * --------------------------------------------------------------------------------------------------

 */
int 
WebIf_Initialize(SCDERoot_t* SCDERootptr)
{
  // make data root locally available
  SCDERoot_at_WebIf_M = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn_at_WebIf_M = SCDERootptr->SCDEFn;

  SCDEFn_at_WebIf_M->Log3Fn(WebIf_ProvidedByModule.typeName
		  ,WebIf_ProvidedByModule.typeNameLen
		  ,3
		  ,"InitializeFn called. Type '%.*s' now useable.\n"
		  ,WebIf_ProvidedByModule.typeNameLen
		  ,WebIf_ProvidedByModule.typeName);

  return 0;
}



/**
 * --------------------------------------------------------------------------------------------------
 *  FName: WebIf_Set
 *  Desc: Processes the device-specific command line arguments from the set command
 *  Info: Invoked by cmd-line 'Set Telnet_Definition.common.Name setArgs'
 *  Para: Telnet_Definition_t *Telnet_Definition -> WebIF Definition that should get a set cmd
 *        uint8_t *setArgs -> the setArgs
 *        size_t setArgsLen -> length of the setArgs
 *  Rets: strTextMultiple_t* -> response text in allocated memory, NULL=no text
 * --------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* 
WebIf_Set(Common_Definition_t* Common_Definition
	,uint8_t *setArgs
	,size_t setArgsLen)
{
  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // make common ptr to modul specific ptr
  WebIf_Definition_t* WebIf_Definition =
	(WebIf_Definition_t*) Common_Definition;

  #if SCDEH_DBG >= 5
  printf("\n|WebIf_Set, Name:%.*s, got args:%.*s>"
	,WebIf_Definition->common.nameLen
	,WebIf_Definition->common.name
	,setArgsLen
	,setArgs);
  #endif


  // response with error text
  retMsg = malloc(sizeof(strTextMultiple_t));

  // response with error text
  retMsg->strTextLen = asprintf(&retMsg->strText
	,"WebIf_Set, Name:%.*s, got args:%.*s"
	,WebIf_Definition->common.nameLen
	,WebIf_Definition->common.name
	,setArgsLen
	,setArgs);

  return retMsg;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: WebIf_Undefine
 * Desc: Removes the define of an "device" of 'WebIf' type. Contains devicespecific init code.
 * Info: Invoked by cmd-line 'Undefine WebIf_Definition.common.Name'
 * Para: WebIf_Definition_t *WebIf_Definition -> WebIf Definition that should be removed
 * Rets: strTextMultiple_t* -> response text NULL=no text
 *--------------------------------------------------------------------------------------------------
 */
strTextMultiple_t*
WebIf_Undefine(Common_Definition_t* Common_Definition)
{
  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  // make common ptr to modul specific ptr
  WebIf_Definition_t* WebIf_Definition = (WebIf_Definition_t*) Common_Definition;


  #if SCDEH_DBG >= 5
  printf("\n|WebIf_UnDef, Name:%s>"
	,(char*)WebIf_Definition->common.name);
  #endif

  return retMsg;
}



/**
 * --------------------------------------------------------------------------------------------------
 *  FName: WebIf_Sub
 *  Desc: Processes the device-specific command line arguments from the set command
 *  Info: Invoked by cmd-line 'Set Telnet_Definition.common.Name setArgs'
 *  Para: Telnet_Definition_t *Telnet_Definition -> WebIF Definition that should get a set cmd
 *        uint8_t *setArgs -> the setArgs
 *        size_t setArgsLen -> length of the setArgs
 *  Rets: strTextMultiple_t* -> response text in allocated memory, NULL=no text
 * --------------------------------------------------------------------------------------------------
 */
strTextMultiple_t* 
WebIf_Sub(Common_Definition_t* Common_Definition
	,uint8_t *kArgs
	,size_t kArgsLen)
{
  // for Fn response msg
  strTextMultiple_t *retMsg = NULL;

  #if SCDEH_DBG >= 5
  printf("\n|WebIf_Sub, Name:%.*s, got key args:%.*s>"
	,Common_Definition->nameLen
	,Common_Definition->name
	,kArgsLen
	,kArgs);
  #endif


  // response with error text
	// alloc mem for retMsg
  retMsg = malloc(sizeof(strTextMultiple_t));

  // response with error text
  retMsg->strTextLen = asprintf(&retMsg->strText
	,"WebIf_Sub, Name:%.*s, got key args:%.*s"
	,Common_Definition->nameLen
	,Common_Definition->name
	,kArgsLen
	,kArgs);

  return retMsg;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: espconn_regist_recvcb
 * Desc: Platform conn - Register data received callback
 * Para: 
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR
espconn_regist_recvcb(WebIf_Definition_t *conn
			, espconn_recv_callback recv_callback)
{
  conn->recv_callback = recv_callback;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: espconn_regist_Connectcb
 * Desc: Platform conn - Register disconnected callback
 * Para: 
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR
espconn_regist_connectcb(WebIf_Definition_t *conn
		, espconn_connect_callback connect_callback)
{
 conn->proto.tcp->connect_callback = connect_callback;
}



/*
 *--------------------------------------------------------------------------------------------------

 *FName: espconn_regist_reconcb
 * Desc: Platform conn - Register error info callback
 * Para: 
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR
espconn_regist_reconcb(WebIf_Definition_t *conn
		, espconn_reconnect_callback reconnect_callback)
{
  conn->proto.tcp->reconnect_callback = reconnect_callback;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: espconn_regist_disconcb
 * Desc: Platform conn - Register disconnected callback
 * Para: 
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR
espconn_regist_disconcb(WebIf_Definition_t *conn
		, espconn_connect_callback disconnect_callback)
{
 conn->proto.tcp->disconnect_callback = disconnect_callback;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: espconn_regist_sentcb
 * Desc: Platform conn - Register data sent callback
 * Para: 
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void ICACHE_FLASH_ATTR
espconn_regist_sentcb(WebIf_Definition_t *conn
		, espconn_sent_callback send_callback)
{
  conn->send_callback = send_callback;
}



/*
 * --------------------------------------------------------------------------------------------------
 *  FName: WebIf_Connect
 *  Desc: Called from XXX_DirectRead when a new connection at the SERVERSOCKET is detected 
 *  Info: 
 *  Para: Common_Definition* Common_Definition -> XXX_Definition of the FD owners definition
 *  Rets: ? - unused
 * --------------------------------------------------------------------------------------------------
 */
/*
int 
WebIf_Connect(Common_Definition_t* Common_Definition)
{

  // make common ptr to modul specific ptr
  WebIf_Definition_t* WebIf_Definition = (WebIf_Definition_t*) Common_Definition;

  // unused
  return 0;
}
*/

