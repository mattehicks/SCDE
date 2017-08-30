/*
 *##################################################################################################
 *  Function: Mimes for SCD-Engine
 *  ESP 8266EX SOC Activities ...
 *  (C) EcSUHA
 *  Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany 
 *  MSchulze780@GMAIL.COM
 *  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
 *##################################################################################################
 */

#include "ProjectConfig.h"
#include <esp8266.h>


//#include <c_types.h>
#include "WebIf_Module_Mimes.h"



/* List of implemented mimes in SCD-Engine -> 0=http(default) not in list!
 * Suffixes - Media type and subtype(s)
 * http://www.sitepoint.com/web-foundations/mime-types-complete-list
 * B 1      87      0
 * I 5                
 * T 0000000000000000     |       THIS DATA MAY BE PROJECT SPECIFIC !         |
 *   \\\\\\\\\\\\\\\\- 00.    = NO FILE MIME ".xxx"
 *    \\\\\\\\\\\\\\\- 01.htm = HTML
 *     \\\\\\\\\\\\\\- 02.cgi = standard gateway interface code
 *      \\\\\\\\\\\\\- 03.jso = json JavaScript Object Notation
 *       \\\\\\\\\\\\- 04.set = settings gateway interface code
 *        \\\\\\\\\\\- 05.prs = parsing  gateway interface code
 *         \\\\\\\\\\- 06.
 *          \\\\\\\\\- 07.
 *           \\\\\\\\- 08.
 *            \\\\\\\- 09.
 *             \\\\\\- 10.
 *              \\\\\- 11.
 *               \\\\- 12.
 *                \\\- 13.
 *                 \\- 14.
 *                  \- 15.
 */
ContentTypes AvailMimes[]=
{
  {"\x00",		NULL}			// #00 -> NO MIME
 ,{"htm",		"text/html"}		// #01 -> standard buildin active web pages
 ,{"cgi",		NULL}			// #02 -> callbacks from standard active web pages
 ,{"jso",		NULL}			// #03 -> java script online data exchange
 ,{"set",		NULL}			// #04 -> special function for home automation-communication 
 ,{"bin",		NULL}			// #05 -> for binary transfers
 ,{"css",		"text/css"}		// #06
 ,{"gif",		"image/gif"}		// #07
 ,{"html",		"text/html"}		// #08
 ,{"js",		"text/javascript"}	// #09
 ,{"txt",		"text/plain"}		// #10 -> for text download
 ,{"jpg",		"image/jpeg"}		// #11
 ,{"jpeg",		"image/jpeg"}		// #12
 ,{"png",		"image/png"}		// #13
 ,{"asp",		NULL}			// #14
 ,{"asp",		NULL}			// #15
// Currently only 0-15 allowed for Active Directory !!!
 ,{NULL,		"text/html"} 		// no implemented mime found -> default value
};




