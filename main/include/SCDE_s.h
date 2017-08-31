﻿

#ifndef _SCDE_S_H_
#define _SCDE_S_H_

#include <sys/queue.h>



typedef struct Common_Definition_s Common_Definition_t;
typedef struct SCDERoot_s SCDERoot_t;
typedef struct Module_s Module_t;

// -------------------------------------------------------------------------------------------------



// NEW TYPE DEFINITIONS

// GENERAL

// size_t for this system
//typedef size_t uint32_t;



// strText_t holds a text string with length (not zero terminated)
typedef struct strText_s strText_t;

// strTextMultiple_t holds multiple text strings with length (not zero terminated) (linked queue)
typedef struct strTextMultiple_s strTextMultiple_t;



// xString_t holds a text string
typedef struct xString_s xString_t;

// xMultipleString_t holds multiple text strings (linked queue)
typedef struct xMultipleString_s xMultipleString_t;

// -------------------------------------------------------------------------------------------------



/*
 * strText_s (struct)
 * - holds a text string with length (not zero terminated)
 */
struct strText_s {

  uint8_t *strText;				// ptr to allocated mem filled with text
  size_t strTextLen;				// length of the text

};



/*
 * strTextMultiple_s (struct)
 * - holds multiple text strings with length (not zero terminated) (linked queue)
 */
struct strTextMultiple_s {

  STAILQ_ENTRY(strTextMultiple_s) entries;	// Link to next strTextMultiple

  uint8_t *strText;				// ptr to allocated mem filled with text
  size_t strTextLen;				// length of the text

};



/*
 * xString_s (struct)
 * Holds one string ( = character-array[length] - not zero terminated!)
 */
struct xString_s {

  uint8_t *characters;				// ptr to allocated memory filled with an character-array
  size_t length;				// length of the character-array

};



/*
 * xMultipleString_s (struct)
 * Holds multiple strings ( = character-array[length] - not zero terminated!) as an linked queue
 */
struct xMultipleString_s {

  STAILQ_ENTRY(xMultipleString_s) entries;	// Link to next xMultipleString

  xString_t string;				// the xText of this entry

};



// -------------------------------------------------------------------------------------------------







// -------------------------------------------------------------------------------------------------


/*
 * typedefs for Fn provided by SCDE (Fn callbacks)
 * This are functions provided for / accessible by modules (for SCDE operation, +helpers )
 */

//
typedef void (* LogFn_t) (char *Device, int LogLevel, char *Text);

//
typedef void (* Log3Fn_t) (const uint8_t *name, const size_t nameLen, const uint8_t LogLevel, const char *format, ...);

//
typedef void (* Log4Fn_t) (char *text);

// typedef for AnalyzeCommandChainFn - analyzes + processes an configuration file
//typedef char* (* AnalyzeCommandChainFn_t) (const uint8_t *args, const size_t argsLen);

STAILQ_HEAD(headRetMsgMultiple_s, strTextMultiple_s);// headRetMsgMultiplexxx;
STAILQ_HEAD(headxMultipleString_s, xMultipleString_s);
//typedef STAILQ_HEAD (stailhead33, strTextMultiple_s) headRetMsgMultiple (* AnalyzeCommandChainFn_t) (const uint8_t *args, const size_t argsLen);
typedef struct headRetMsgMultiple_s (* AnalyzeCommandChainFn_t) (const uint8_t *args, const size_t argsLen);










// typedef for AnalyzeCommandFn - analyzes + processes one command row
typedef struct headRetMsgMultiple_s (* AnalyzeCommandFn_t) (const uint8_t *args, const size_t argsLen);

//
typedef Module_t* (* GetLoadedModulePtrByNameFn_t)(const uint8_t *typeName, const size_t typeNameLen);

//
typedef Module_t* (* CommandReloadModuleFn_t)(const uint8_t *typeName, const size_t typeNameLen);

// typedef for readingsBeginUpdateFn - call this before updating readings
typedef int (* readingsBeginUpdateFn_t) (Common_Definition_t *Common_Definition);

// typedef for readingsBulkUpdateFn - call this for every reading (bulk-update)
typedef int (* readingsBulkUpdateFn_t) (Common_Definition_t *Common_Definition, uint8_t *readingNameText, size_t readingNameTextLen, uint8_t *readingValueText, size_t readingValueTextLen);

// typedef for readingsEndUpdateFn - call this to after bulk-update to process readings
typedef int (* readingsEndUpdateFn_t) (Common_Definition_t *Common_Definition);

// typedef for TimeNowFn - returns current time stamp
typedef time_t (* TimeNowFn_t) ();

// typedef for FmtDateTimeFn - returns formated text of Date-Time from tist
typedef strText_t (* FmtDateTimeFn_t) (time_t time);

// typedef for FmtTimeFn - returns formated text of Time from tist
typedef strText_t (* FmtTimeFn_t) (time_t time);

// prints data as Hex-Dump to debug terminal
typedef void (* HexDumpOutFn_t) (char *desc, void *addr, int len);

// typedef for Call GetFn by Def-Name - for 2 stage desings, requests data
typedef int (* CallGetFnByDefNameFn_t) (const uint8_t *nameText, const size_t nameTextLen, Common_Definition_t *sourceCommon_Definition, void *X);


/*
 *  Helper routine that parses Key=Value(@) input arguments into an allocated array
 */
typedef struct kvParseImplementedKeys_s kvParseImplementedKeys_t; // holding the input (IK)

// Data structure that holds the implemented keys and affected readings.
// Used as input for Key=Value input parsing.
struct kvParseImplementedKeys_s {

  const uint32_t affectedReadings;

  const char *implementedKey;

};

typedef struct parsedKVInputArgs_s parsedKVInputArgs_t; // holds the parsed result in allocated mem

// Result structure for http_parser_parse_url().
// Callers should index into field_data[] with UF_* values if field_set
// has the relevant (1 << UF_*) bit set. As a courtesy to clients (and
// because we probably have padding left over), we convert any port to
// a uint32_t.
struct parsedKVInputArgs_s {

  uint64_t keysFoundBF;		// Bit-Field of found keys (1 << XX_IK_*)

  uint64_t requiredKVBF;	// Bit-Field of keys that are required for a successful parse

  uint64_t forbiddenKVBF;	// Bit-Field of keys that are forbidden for a successful parse

  uint32_t affectedReadingsBF;	// Bit-Field of affected readings

  struct keyData_s {

	uint16_t off;		// Offset into buffer in which value-text starts

	uint16_t len;		// Length of value-text in buffer

	uint32_t affectedReadings;

	} keyData_t[];		// XX_IK_MAX

};

// Hepler routine that parses Key=Value(@) input arguments into an allocated array
typedef parsedKVInputArgs_t* (* ParseKVInputArgsFn_t) (int numImplementedKeys, const kvParseImplementedKeys_t *XX_IK, const uint8_t *kvArgsText, const size_t kvArgsTextLen);



/*
 *  ...
 */

// Commands A-Z - will be removed loaded!!
//typedef char* (* CommandDefineFn_t) (const uint8_t *args, const size_t argsLen);
//typedef char* (* CommandSetFn_t) (const uint8_t *args, const size_t argsLen);
typedef strTextMultiple_t* (* CommandUndefineFn_t) (const uint8_t *args, const size_t argsLen);

//--------------------------------------------------------------------------------------------------



/*
 * SCDEFn (Functions / callbacks) for operation, provided / made accessible for modules
 */
typedef struct SCDEFn_s
  {

  LogFn_t LogFn;				// Log -> This is the main logging function

  Log3Fn_t Log3Fn;				// Log -> This is the main logging function

  Log4Fn_t Log4Fn;				// Log -> This is the main logging function

  AnalyzeCommandChainFn_t AnalyzeCommandChainFn;// analyzes + processes an configuration file


  AnalyzeCommandFn_t AnalyzeCommandFn;		// analyzes + processes one command row

  GetLoadedModulePtrByNameFn_t GetLoadedModulePtrByNameFn;

  CommandReloadModuleFn_t CommandReloadModuleFn;



  CommandUndefineFn_t CommandUndefineFn;



  readingsBeginUpdateFn_t readingsBeginUpdateFn;// call this before updating readings

  readingsBulkUpdateFn_t readingsBulkUpdateFn;	// call this for every reading (bulk-update)

  readingsEndUpdateFn_t readingsEndUpdateFn;	// call this to after bulk-update to process readings

  TimeNowFn_t TimeNowFn;			// returns current time stamp

  FmtDateTimeFn_t FmtDateTimeFn;		// returns formated text of Date-Time from tist

  FmtTimeFn_t FmtTimeFn;			// returns formated text of Time from tist

  HexDumpOutFn_t HexDumpOutFn;			// prints data as Hex-Dump to debug terminal

  ParseKVInputArgsFn_t ParseKVInputArgsFn;	// parses Key=Value(@) input arguments into array

  CallGetFnByDefNameFn_t CallGetFnByDefNameFn;  // original CallFn

} SCDEFn_t;



//--------------------------------------------------------------------------------------------------



/*
 * typedefs for Module Functions (callbacks) (provided for SCDE - to execute module Fn),
 * should be only called if not NULL.
 *
 * InitializeFn  - returns module information (module_s) required for operation
 *xxxxxxxxxxxxxxxxxxx AttributeFn   - called for attribute changes
 * DefineFn      - define a "device" of this type	// Callback wird beim Define eines Devices/Gerätes aufgerufen
 * DeleteFn      - clean up (delete logfile), called by delete after UndefFn	// wird beim delete aufgerufen um das Device/Gerät endgültig zu löschen
 * ExceptFn      - called if the global select reports an except field
 * GetFn         - get some data from this device
 * NotifyFn      - call this if some device changed its properties
 * ParseFn       - Interpret a raw message
 * ReadFn        - Reading from a Device (see FHZ/WS300)
 * ReadyFn       - check for available data, if no FD
 * RenameFn      - inform the device about its renameing
 * SetFn         - set/activate this device
 * ShutdownFn    - called before shutdown
 * StateFn       - set local info for this device, do not activate anything
 * UndefineFn    - clean up (delete timer, close fd), called by delete and rereadcfg	// Callback wird beim Undefine eines Devices/Gerätes aufgerufen
 */
//typedef Module_t *Module (* InitializeFn_t)(); // sollte eigentlich so sein



/*
 *--------------------------------------------------------------------------------------------------
 *FName: Master_Initialize
 * Desc: Initializion of Home Control Function Callbacks in loaded module
 * Info: Initializion of available Module Function Callbacks for Home Control
 * Para: 
 * Rets: 
 *--------------------------------------------------------------------------------------------------
 */
/*

 *--------------------------------------------------------------------------------------------------
 *FName: Master_Define
 * Desc: Called from command define when a "device" of this type is defined.
 *       Pot modul specific code for define here
 * Info: 
 * Para: Common_Definition *Def -> the new definition for this device, some inits done
 *	 char *Definition -> args?
 * Rets: char* -> error-text-string in allocated mem, or NULL=OK
 *--------------------------------------------------------------------------------------------------
 */



//--------------------------------------------------------------------------------------------------



// ProvidedByModule_t stores function callbacks (Fn) for SCDE module operation
typedef struct ProvidedByModule_s ProvidedByModule_t;

// typedef for AddFn - experimental - provided my module
typedef strTextMultiple_t* (* AddFn_t)(Common_Definition_t *Common_Definition, uint8_t *kvArgs, size_t kvArgsLen);

// typedef for AttributeFn - called in case of attribute changes, to check them - provided my module
typedef strTextMultiple_t* (* AttributeFn_t)(Common_Definition_t* Common_Definition, const uint8_t *attrCmdText, const size_t attrCmdTextLen, const uint8_t *attrNameText, const size_t attrNameTextLen, uint8_t **attrValTextPtr, size_t *attrValTextLenPtr);

// typedef for DefineFn - called to create a new definition of this type - provided my module
typedef strTextMultiple_t* (* DefineFn_t)(Common_Definition_t *Common_Definition);//, const char *Definition);

typedef int (* DeleteFn_t)(Common_Definition_t *Common_Definition);		
typedef int (* ExceptFn_t)(Common_Definition_t *Common_Definition);
typedef int (* FingerprintFn_t)(Common_Definition_t *Common_Definition);

// typedef for GetFn - for 2 stage designs - called to get data from this type - provided my module
typedef int (* GetFn_t)(Common_Definition_t *Common_Definition, Common_Definition_t *sourceCommon_Definition, void *X);

typedef int (* IdleCbFn_t)(Common_Definition_t *Common_Definition);
typedef int (* InitializeFn_t)(SCDERoot_t *SCDERoot);
typedef int (* NotifyFn_t)(Common_Definition_t *Common_Definition);
typedef int (* ParseFn_t)(Common_Definition_t *Common_Definition);
typedef int (* ReadFn_t)(Common_Definition_t *Common_Definition);
typedef int (* ReadyFn_t)(Common_Definition_t *Common_Definition);
typedef int (* RenameFn_t)(Common_Definition_t *Common_Definition);

// typedef for SetFn - called to send data to the device (opposite of Get) - provided my module
typedef strTextMultiple_t* (* SetFn_t)(Common_Definition_t *Common_Definition, uint8_t *setArgs, size_t setArgsLen);

// typedef for ShutdownFn - called to do activities before SCDE shuts down - provided my module
typedef strTextMultiple_t* (* ShutdownFn_t)(Common_Definition_t *Common_Definition);

typedef int (* StateFn_t)(Common_Definition_t *Common_Definition);

// typedef for SubFn - experimental - provided my module
typedef strTextMultiple_t* (* SubFn_t)(Common_Definition_t *Common_Definition, uint8_t *kArgs, size_t kArgsLen);

// typedef for UndefineFn - called when device is deleted, chance to cleanup - provided my module
typedef strTextMultiple_t* (* UndefineFn_t)(Common_Definition_t *Common_Definition);		

typedef int (* DirectReadFn_t)(Common_Definition_t *Common_Definition);
typedef int (* DirectWriteFn_t)(Common_Definition_t *Common_Definition);



/* 
 * ProvidedByModule_s (struct)
 * - stores function callbacks for SCDE module operation
 * - information is sent to SCDE by module when loaded
 * - done by InitializeFn after module the loaded
 */
struct ProvidedByModule_s {

  uint8_t typeName[32];	//typeNameText;	// Type-Name string of module
  size_t typeNameLen;//typeNameTextLen;

  AddFn_t AddFn;		// called when Key=Value Attributes owned by this Module are assigned

  AttributeFn_t AttributeFn;	// called in case of attribute activities / changes, to check them

  DefineFn_t DefineFn;		// called to create a new definition of this type / to finalize setup

  DeleteFn_t DeleteFn;		// clean up (delete logfile), called by delete after UndefFn

  ExceptFn_t ExceptFn;		// called if the global select reports an except field

  GetFn_t GetFn;		// get some data from this device

  IdleCbFn_t IdleCbFn;		// give module an Idle-Callback to process something

  InitializeFn_t InitializeFn;	// returns module information (module_s) required for operation

  NotifyFn_t NotifyFn;		// call this if some device changed its properties

  ParseFn_t ParseFn;		// Interpret a new message

  ReadFn_t ReadFn;		// Reading / receiving from a Device

  ReadyFn_t ReadyFn;		// check for available data, if no FD

  RenameFn_t RenameFn;		// inform the device about its renameing

  SetFn_t SetFn;		// set/activate this device

  ShutdownFn_t ShutdownFn;	// called before shutdown

  StateFn_t StateFn;		// set local info for this device, do not activate anything

  SubFn_t SubFn;		// called when Attribute-Keys owned by this Module are deleted

  UndefineFn_t UndefineFn;	// clean up (delete timer, close fd), called by delete and rereadcfg

  DirectReadFn_t DirectReadFn;	// ? called by select loop

  DirectWriteFn_t DirectWriteFn;// ? called by select loop

// uint32_t *FnProvided;	// Fn Provided for other Modules

  int SizeOfDefinition;		// Size of modul specific definition structure (Common_Definition_t + X)

};



//--------------------------------------------------------------------------------------------------



// Module_t stores information of loaded modules, Fns required for module use and operation
typedef struct Module_s Module_t;



/*
 * Module_s (struct)
 * - stores information of loaded modules, Fns required for module use and operation
 * - data is associated when module is loaded
 * - done in InitializeFn (after module load)
 */
struct Module_s {

  STAILQ_ENTRY (Module_s) entries;	// Link to next loaded Module

  ProvidedByModule_t *ProvidedByModule;	// Ptr to Provided by Module Info

  void *LibHandle;			// Handle to this loaded Module

};



// -------------------------------------------------------------------------------------------------



// bulkUpdateReadings_t stores bulk update information and the link to updated readings
typedef struct bulkUpdateReadings_s bulkUpdateReadings_t;



/* 
 * bulkUpdateReadings_s (struct)
 * - stores bulk update information and the link to updated readings
 */
struct bulkUpdateReadings_s {

  STAILQ_HEAD (stailhead7, reading_s) headReadings;	// Link to assigned reading

  time_t bulkUpdateTist;		// timestamp of bulk update

};



// -------------------------------------------------------------------------------------------------



// reading_t stores one reading
typedef struct reading_s reading_t;



/* 
 * reading_s (struct)
 * - stores one reading
 */
struct reading_s {

  STAILQ_ENTRY(reading_s) entries;	// link to next reading

  uint8_t *readingNameText;		// text of reading-name, in allocated mem
  size_t readingNameTextLen;		// text-len of reading-name

  uint8_t *readingValueText;		// text of reading-value, in allocated mem
  size_t readingValueTextLen;		// text-len of reading-value

  time_t readingTist;			// timestamp of reading

};



// -------------------------------------------------------------------------------------------------



// Common_Definition_t stores values for operation of an definition (device)
typedef struct Common_Definition_s Common_Definition_t;



/* 
 * Common_Definition_s (struct)
 * - stores values for operation of an definition (device), common part,  valid only for the defined
 * - instance of an loaded module. Values are initialized by the SCDE and finalized by the
 *   loaded module itself (defineFn).
 */
struct Common_Definition_s {
 
  STAILQ_HEAD (stailhead8, common_Attribute_s) headAttributes;	// Link to assigned Attributes

  uint32_t *link;		// link to next struct

  STAILQ_ENTRY(Common_Definition_s) entries; // Link to next Definition


  uint8_t *name;//nameText;	// Ptr to the Name string (assigned by users declaration - in allocated mem)
  size_t nameLen;//nameTextLen

  Module_t *module;		// Ptr to the Module_t (assigned when module is loaded)

  uint8_t *state;//stateText	// Ptr to allocated memory filled with oneliner-string describing its state
  size_t stateLen;//stateTextLen

  uint8_t *definition;//definitionText	// Ptr to allocated memory filled with users definition string
  size_t definitionLen;//definitionTextLen

  int nr;			// Sequential number assigned to device by HCTRLD

  int fd;			// FileDescriptor. Used by selectlist / readyfnlist (-1 = not assigned)

  int Common_CtrlRegA;		// Common Control Reg A (enum Common_CtrlRegA from WEBIF.h)

  xSemaphoreHandle def_mux; // 

  bulkUpdateReadings_t *bulkUpdateReadings;
  STAILQ_HEAD (stailhead6, reading_s) headReadings;	// Link to assigned Attributes

  /* todo
#Special values in %defs:
# READINGS- The readings. Each value has a "VAL" and a "TIME" component.
# IODev   - attached to io device
# CHANGED - Currently changed attributes of this device. Used by NotifyFn
# VOLATILE- Set if the definition should be saved to the "statefile"
# NOTIFYDEV - if set, the notifyFn will only be called for this device
*/

};



// Information Flags - for Connection Control
enum Common_CtrlRegA {
    F_WANTS_WRITE		= 1 << 0	// indicates: define (FD) wants to write data (call DirectWriteFn), if ready
  , F_WANTS_IDLE_TASK		= 1 << 1	// indicates: define (FD) wants idle-task to be called (call IdleTaskFn)
  , F_xRESXX			= 1 << 2	//   
  , F_xRESXZ			= 1 << 3	// 
  , F_xRESXA			= 1 << 4
  , F_xRESXB			= 1 << 5
  , F_xRESXC			= 1 << 6
  , F_xRESXD			= 1 << 7
  , F_xRESXE			= 1 << 8
  , F_xRESXF			= 1 << 9
  , F_xRESXG			= 1 << 10
  , F_xRESXH			= 1 << 11
  , F_xRESXI			= 1 << 12
  , F_xRESXJ			= 1 << 13
  , F_xRESXK			= 1 << 14
  , F_xRESXL			= 1 << 15
};



// -------------------------------------------------------------------------------------------------



/*
 * typedef for generic, loadable Command-Callbacks
 * They provide User-Executable, loadable functions for SCDE
 */             //strTextMultiple_t

//
typedef int (* initializeCommandFn_t)(SCDERoot_t* SCDERoot);
//
typedef struct headRetMsgMultiple_s (* commandFn_t) (const uint8_t *args, const size_t argsLen);


// providedByCommand_t stores function callbacks and information for command operation
typedef struct providedByCommand_s providedByCommand_t;



/* 
 * providedByCommand_s (struct)
 * - stores function callbacks and information for command operation
 * - nformation is associated when the command is loaded
 * - (done in InitializeFn after command load)
 */
struct providedByCommand_s {

  uint8_t commandNameText[32];			// name text of command
  size_t commandNameTextLen;

  initializeCommandFn_t initializeCommandFn;	// returns module information (module_s) required for operation

  commandFn_t commandFn;			// the command Fn

  const uint8_t *helpText;			// string of help
  const size_t helpTextLen;

  const uint8_t *helpDetailText;		// string of detailed help
  const size_t helpDetailTextLen;

};



//--------------------------------------------------------------------------------------------------



// command_t stores commands made available for operation.
typedef struct command_s command_t;



/* 
 * commands_s (struct)
 * - stores commands made available for operation.
 * - some are buildin and initialized after start, others are loaded by user
 */
struct command_s {

  STAILQ_ENTRY (command_s) entries;		// link to next loaded command

  providedByCommand_t *providedByCommand;	// ptr to provided-by-command Info

};



// -------------------------------------------------------------------------------------------------



// attribute_t holds attributes assigned to definitions, for operation and customization.
typedef struct attribute_s attribute_t;



/*
 * attribute_s (struct)
 * - stores attributes assigned to definitions, for operation and customization.
 */
struct attribute_s {

  STAILQ_ENTRY(attribute_s) entries;	// link to next attribute

  Common_Definition_t *attrAssignedDef;	// ptr to the definition the attribute is assigned to

  uint8_t *attrNameText;		// ptr to the attribute name text (text in allocated mem!)
  size_t   attrNameTextLen;

  uint8_t *attrValText;			// ptr to the attribute value text (text in allocated mem!)
  size_t   attrValTextLen;

};



// -------------------------------------------------------------------------------------------------



// SCDERoot_t holds the Smart Connected Devices Engine - root data
typedef struct SCDERoot_s SCDERoot_t;



/* 
 * SCDERoot_s (struct)
 * - Smart Connected Devices Engine - root data
 */
struct SCDERoot_s {

  SCDEFn_t* SCDEFn;					// SCDEFn (Functions / callbacks) for faster operation
//use vars qw($auth_refresh);
//use vars qw($cmdFromAnalyze);   # used by the warnings-sub
//use vars qw($cvsid);            # used in 98_version.pm
  strText_t currCfgFile;
  uint32_t DevCount;					// Maximum device number, used for storing
  uint32_t FeatureLevel;				// for version management
  uint32_t globalCtrlRegA;				// global flags A
//use vars qw($fhem_started);     # used for uptime calculation
//use vars qw($init_done);        #
//use vars qw($internal_data);    # FileLog/DbLog -> SVG data transport
//use vars qw($lastDefChange);    # number of last def/attr change
//use vars qw($nextat);           # Time when next timer will be triggered.
//use vars qw($readytimeout);     # Polling interval. UNIX: device search only
//use vars qw($reread_active);
//use vars qw($selectTimestamp);  # used to check last select exit timestamp
//use vars qw($winService);       # the Windows Service object
  STAILQ_HEAD (stailhead3, attribute_s) headAttributes;	// Link to assigned attributes
  STAILQ_HEAD (stailhead4, command_s) headCommands;	// Link to available commands
//use vars qw(%data);             # Hash for user data
//use vars qw(%defaultattr);      # Default attributes, used by FHEM2FHEM
  STAILQ_HEAD (stailhead2, Common_Definition_s) HeadCommon_Definitions;// Link to Definitions (device, button, ...)
//use vars qw(%inform);           # Used by telnet_ActivateInform
//use vars qw(%intAt);            # Internal at timer hash, global for benchmark
//use vars qw(%logInform);        # Used by FHEMWEB/Event-Monitor
  STAILQ_HEAD (stailhead1, Module_s) HeadModules;	// Link to loaded Modules
//use vars qw(%ntfyHash);         # hash of devices needed to be notified.
//use vars qw(%oldvalue);         # Old values, see commandref.html
//use vars qw(%readyfnlist);      # devices which want a "readyfn"
//use vars qw(%selectlist);       # devices which want a "select"
//use vars qw(%value);            # Current values, see commandref.html
//use vars qw(@authenticate);     # List of authentication devices
//use vars qw(@authorize);        # List of authorization devices
//use vars qw(@structChangeHist); # Contains the last 10 structural changes

};


// global - information flags - for Connection Control
enum Global_CtrlRegA
  { F_RECEIVED_QUIT		= 1 << 0	// indicates: quit the include processing
  , F_INIT_DONE			= 1 << 1	// indicates: the SCDE has fiinished the init process
  , F_RESGM			= 1 << 2	//   
  , F_RESGN			= 1 << 3	// 
  , F_RESGA			= 1 << 4
  , F_RESGB			= 1 << 5
  , F_RESGC			= 1 << 6
  , F_RESGD			= 1 << 7
  , F_RESGE			= 1 << 8
  , F_RESGF			= 1 << 9
  , F_RESGG			= 1 << 10
  , F_RESGH			= 1 << 11
  , F_RESGI			= 1 << 12
  , F_RESGJ			= 1 << 13
  , F_RESGK			= 1 << 14
  , F_RESGL			= 1 << 15
  };

#endif /*_SCDE_S_H_*/
