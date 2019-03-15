

#ifndef _SCDE_S_H_
#define _SCDE_S_H_

#include <sys/queue.h>



// -------------------------------------------------------------------------------------------------
// initial type definitions ...


// nach unten verschieben ? doppelt ?
typedef struct Common_Definition_s Common_Definition_t;

// SCDE Root (type) holds the Smart Connected Devices Engine - root data
typedef struct SCDERoot_s SCDERoot_t;

// Module (type) stores information of loaded modules, required for module use and operation
typedef struct Module_s Module_t;

// Provided by Module (type) stores (the common) function callbacks for SCDE module operation
typedef struct ProvidedByModule_s ProvidedByModule_t;

// Command (type) stores commands made available for operation.
typedef struct Command_s Command_t;

// Common_Definition_t stores values (the common part) for operation of an definition
typedef struct Common_Definition_s Common_Definition_t;

// ?
typedef struct Common_StageXCHG_s Common_StageXCHG_t;

// -------------------------------------------------------------------------------------------------




// NEW TYPE DEFINITIONS

// GENERAL

// size_t for this system
//typedef size_t uint32_t;


// ptr zur Fehlermeldung in allokierter strTextMultiple_t WENN NOCH KEIN FEHLER WAR
#define SCDE_OK  NULL


// -------------------------------------------------------------------------------------------------



/*
 * typedefs for string storage and processing
 */


// -------------------------------------------------------------------------------------------------


// String_t holds one string. Characters are at ptr in allocated memory. Length information is included
//  (! the characters are not zero terminated !)
typedef struct String_s String_t;

/*
 * String_s (struct)
 * Holds one string (ptr to characters-array (not zero terminated) and the length of this array)
 */
struct String_s {
  uint8_t* p_char;		// typically ptr to allocated memory filled with an character-array
  size_t len;			// length of the not zero terminated character-array
};


// -------------------------------------------------------------------------------------------------


// Entry_String_t to hold multiple xString_t strings (in an singly linked tail queue)
typedef struct Entry_String_s Entry_String_t;

/*
 * Entry_String_s (struct) 
 * is an singly linked tail queue entry. It stores one String_t string
 * it is used when storing multiple string elements in an singly tail linked queue.
 */
struct Entry_String_s {
  STAILQ_ENTRY(Entry_String_s) entries;		// link to next element of the singly linked tail queue
  String_t string;				// the string of this element
};

// Constructor for the singly linked tail queue (head), which can hold linked string entries
STAILQ_HEAD(Head_String_s, Entry_String_s);


// -------------------------------------------------------------------------------------------------

// OBSOLETE: use String_t
typedef struct xString_s xString_t;

/*
 * xString_s (struct)
 * Holds one string (ptr to characters-array (not zero terminated) and the length of this array)
 */
struct xString_s {
  uint8_t *characters;		// typically ptr to allocated memory filled with an character-array
  size_t length;		// length of the not zero terminated character-array
};

// -------------------------------------------------------------------------------------------------

// OBSOLETE: use xString_t
typedef struct strText_s strText_t;

/* OBSOLETE: use xString_s
 * strText_s (struct)
 * - holds normally a text string with length (not zero terminated)
 * - may be raw data too !
 * - *strText = NULL -> empty, else filled !
 */
struct strText_s {
  char *strText;				// ptr to allocated mem filled with text
  size_t strTextLen;				// length of the text
};

// -------------------------------------------------------------------------------------------------

// OBSOLETE: use  Entry_String_t
typedef struct xMultipleStringSLTQE_s xMultipleStringSLTQE_t;

/*
 * xMultipleStringSLTQE_s (struct) 
 * is an singly linked tail queue element and stores one xString_t string
 * it is used when storing multiple strings (in an singly tail linked queue)
 */
struct xMultipleStringSLTQE_s {
  STAILQ_ENTRY(xMultipleStringSLTQE_s) entries;	// link to next element of the singly linked tail queue
  xString_t string;				// the string of this element
};

/*
 * Constructor for the singly linked tail queue (head), which can hold multiple linked strings
 * SLTQ can be used for an FIFO queue by adding entries at the tail and fetching entries from the head
 * SLTQ is inefficient when removing arbitrary elements
 */
STAILQ_HEAD(xHeadMultipleStringSLTQ_s, xMultipleStringSLTQE_s);
//STAILQ_HEAD(xMultipleStringSLTQH_s, xMultipleStringSLTQE_s);

// -------------------------------------------------------------------------------------------------

// OBSOLETE: use  xMultipleString_t
typedef struct strTextMultiple_s strTextMultiple_t;

/* OBSOLETE: use xMultipleString_s
 * strTextMultiple_s (struct)
 * - holds multiple text strings with length (not zero terminated) (linked queue)
 */
struct strTextMultiple_s {
  STAILQ_ENTRY(strTextMultiple_s) entries;	// Link to next strTextMultiple
  char *strText; 				// ptr to allocated mem filled with text
  size_t strTextLen;				// length of the text
};

// -------------------------------------------------------------------------------------------------




//warum hier?

// OBSOLETE: use xHeadMultipleString_s
STAILQ_HEAD(headxMultipleString_s, xMultipleString_s);
// OBSOLETE: use xHeadMultipleString_s
STAILQ_HEAD(headRetMsgMultiple_s, strTextMultiple_s);




/*
 *  ...
 */

// Commands A-Z - will be removed loaded!!
//typedef char* (* CommandDefineFn_t) (const uint8_t *args, const size_t argsLen);
//typedef char* (* CommandSetFn_t) (const uint8_t *args, const size_t argsLen);
typedef strTextMultiple_t* (* CommandUndefineFn_t) (const uint8_t *args, const size_t argsLen);



// -------------------------------------------------------------------------------------------------



/*
 *  Helper routine that parses Key=Value(@) input arguments into an allocated array
 */
// holding the input (IK)
typedef struct kvParseImplementedKeys_s kvParseImplementedKeys_t;

// Data structure that holds the implemented keys and affected readings.
// Used as input for Key=Value input parsing.
struct kvParseImplementedKeys_s {

  const uint32_t affectedReadings;

  const char *implementedKey;
};

 // holds the parsed result in allocated mem
typedef struct parsedKVInputArgs_s parsedKVInputArgs_t;

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



//--------------------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------------------



/*
 * typedefs of Function Callbacks
 * This are provided & made accessible for modules and commands - for operation and helpers
 */
// typedef for AnalyzeCommandFn - analyzes + processes one command row
typedef struct headRetMsgMultiple_s (*AnalyzeCommandFn_t) (const uint8_t *args, const size_t argsLen);

// typedef for AnalyzeCommandChainFn - analyzes + processes an configuration file
typedef struct headRetMsgMultiple_s (*AnalyzeCommandChainFn_t) (const uint8_t *args, const size_t argsLen);

// typedef for Call GetFn by Def-Name - for 2 stage desings, requests data
typedef int (* CallGetFnByDefNameFn_t) (const uint8_t *nameText, const size_t nameTextLen, Common_Definition_t *sourceCommon_Definition, void *X);

//
typedef Module_t* (*CommandReloadModuleFn_t)(const uint8_t *typeName, const size_t typeNameLen);

// typedef for Devspec2ArrayFn - returns all definitions that match the given devicespecification (devspec)
typedef struct xHeadMultipleStringSLTQ_s (*Devspec2ArrayFn_t) (const xString_t devspecString);

// typedef for FmtDateTimeFn - returns formated text of Date-Time from tist
typedef strText_t (*FmtDateTimeFn_t) (time_t time);

// typedef for FmtTimeFn - returns formated text of Time from tist
typedef strText_t (*FmtTimeFn_t) (time_t time);

// typedef for Call Get All Readings Fn by Def-Name
typedef struct headRetMsgMultiple_s (*GetAllReadingsFn_t) (Common_Definition_t *Common_Definition);

// typedef for Call Get Definition and Attributes setup lines Fn by Def-Name
typedef struct headRetMsgMultiple_s (*GetDefAndAttrFn_t) (Common_Definition_t *Common_Definition);

//
typedef Common_Definition_t* (*GetDefinitionPtrByNameFn_t) (const size_t definitionNameLen, const uint8_t *definitionName);

// Returns a STAILQ head that stores entries of all 'dev_spec' matching definitions
typedef struct Head_Definitions_s (*Get_Definitions_That_Match_DefSpec_String_Fn_t) (const String_t dev_spec);

//
typedef Module_t* (*GetLoadedModulePtrByNameFn_t)(const uint8_t *typeName, const size_t typeNameLen);

// returns current SCDE Time Stamp
typedef time_t (*GetTiStFn_t)(void);

// returns an UNIQUE SCDE Time Stamp (in most cases the current Time Stamp)
typedef time_t (*GetUniqueTiStFn_t)(void);

// typedef for GoodDeviceNameFn - checks the given Device Name for device name rules
typedef bool (*GoodDeviceNameFn_t) (const xString_t nameString);

// typedef for GoodReadingNameFn - checks the given Reading Name for reading name rules
typedef bool (*GoodReadingNameFn_t) (const xString_t nameString);

// prints data as Hex-Dump to debug terminal
typedef void (* HexDumpOutFn_t) (char *desc, void *addr, int len);

//
typedef void (*LogFn_t) (char *Device, int LogLevel, char *Text);

//
typedef void (*Log3Fn_t) (const uint8_t *name, const size_t nameLen, const uint8_t LogLevel, const char *format, ...);

//
typedef void (*Log4Fn_t) (char *text);

// typedef for MakeDeviceNameFn - corrects the given Device Name (overwrites illegal chars with '_')
typedef void (*MakeDeviceNameFn_t) (const xString_t nameString);

// typedef for MakeReadingNameFn - corrects the given Reading Name (overwrites illegal chars with '_')
typedef void (*MakeReadingNameFn_t) (const xString_t nameString);

// typedef for readingsBeginUpdateFn - call this before updating readings
typedef int (*readingsBeginUpdateFn_t) (Common_Definition_t *Common_Definition);

// typedef for readingsBulkUpdateFn - call this for every reading (bulk-update)
typedef int (*readingsBulkUpdateFn_t) (Common_Definition_t *Common_Definition, uint8_t *readingNameText, size_t readingNameTextLen, uint8_t *readingValueText, size_t readingValueTextLen);

// call this to add an Reading to the running update of Readings
typedef int (*readingsBulkUpdate2Fn_t) (Common_Definition_t *Common_Definition, const size_t readingNameStringLength, const uint8_t *readingNameStringCharacters, const size_t readingValueStringLengthconst, const uint8_t *readingValueStringCharacters, const bool changed);

// typedef for readingsEndUpdateFn - call this to after bulk-update to process readings
typedef int (*readingsEndUpdateFn_t) (Common_Definition_t *Common_Definition, bool doTrigger);

// typedef for TimeNowFn - returns current time stamp
typedef time_t (*TimeNowFn_t) ();

// typedef for WriteStatefileFn - 
typedef struct headRetMsgMultiple_s (*WriteStatefileFn_t) ();

// ----------------

// added Fn (Perl -> C)
typedef strText_t* (*Get_attrVal_by_defName_and_attrNameFn_t) (const strText_t *defName
				,const strText_t *attrName);



/*
 * SCDEFn (SCDE Functions) typedef
 * Stores Function callbacks provided & made accessible for modules and commands - for operation and helpers
 */
typedef struct SCDEFn_s {
  AnalyzeCommandFn_t AnalyzeCommandFn;                     // analyzes + processes one command row
  AnalyzeCommandChainFn_t AnalyzeCommandChainFn;           // analyzes + processes an configuration file
  CallGetFnByDefNameFn_t CallGetFnByDefNameFn;             // original CallFn
  CommandReloadModuleFn_t CommandReloadModuleFn;           //
  Devspec2ArrayFn_t Devspec2ArrayFn;			   // returns all definitions that match devspec
  FmtDateTimeFn_t FmtDateTimeFn;                           // returns formated text of Date-Time from tist
  FmtTimeFn_t FmtTimeFn;                                   // returns formated text of Time from tist
  GetAllReadingsFn_t GetAllReadingsFn;                     // returns all readings of an definition
  GetDefAndAttrFn_t GetDefAndAttrFn;                       //
  GetDefinitionPtrByNameFn_t GetDefinitionPtrByNameFn;     //
  Get_Definitions_That_Match_DefSpec_String_Fn_t Get_Definitions_That_Match_DefSpec_String_Fn;
  GetLoadedModulePtrByNameFn_t GetLoadedModulePtrByNameFn; //
  GetTiStFn_t GetTiStFn;                                   // returns current SCDE Time Stamp
  GetUniqueTiStFn_t GetUniqueTiStFn;			   // returns an UNIQUE SCDE Time Stamp
  GoodDeviceNameFn_t GoodDeviceNameFn;			   // checks the given Device Name for device name rules
  GoodReadingNameFn_t GoodReadingNameFn;		   // checks the given Reading Name for device name rules
  HexDumpOutFn_t HexDumpOutFn;                             // prints data as Hex-Dump to debug terminal
  LogFn_t LogFn;                                           // Log -> This is the main logging function
  Log3Fn_t Log3Fn;                                         // Log -> This is the main logging function
  Log4Fn_t Log4Fn;                                         // Log -> This is the main logging function
  MakeDeviceNameFn_t MakeDeviceNameFn;			   // corrects the given Device Name
  MakeReadingNameFn_t MakeReadingNameFn;		   // corrects the given Reading Name   
  readingsBeginUpdateFn_t readingsBeginUpdateFn;           // call this before updating readings
  readingsBulkUpdateFn_t readingsBulkUpdateFn;	           // call this for every reading (bulk-update)
  readingsBulkUpdate2Fn_t readingsBulkUpdate2Fn;	   // call this to add an Reading to the running update
  readingsEndUpdateFn_t readingsEndUpdateFn;               // call this to after bulk-update to process readings



  TimeNowFn_t TimeNowFn;                                   // returns current system time (no TiSt)
  WriteStatefileFn_t WriteStatefileFn;                     // 
// added Fn (Perl -> C)
  Get_attrVal_by_defName_and_attrNameFn_t Get_attrVal_by_defName_and_attrNameFn;
// not final
  CommandUndefineFn_t CommandUndefineFn;                   //
  ParseKVInputArgsFn_t ParseKVInputArgsFn;                 // parses Key=Value(@) input arguments into array
} SCDEFn_t;



//--------------------------------------------------------------------------------------------------



/*
 * typedefs for Module Functions (callbacks) (provided for SCDE - to execute module Fn),
 * should be only called if not NULL.
 *
 * InitializeFn  - returns module information (module_s) required for operation
 * DeleteFn      - clean up (delete logfile), called by delete after UndefFn	// wird beim delete aufgerufen um das Device/Gerät endgültig zu löschen
 * ExceptFn      - called if the global select reports an except field
 * GetFn         - get some data from this device
 * NotifyFn      - call this if some device changed its properties
 * ParseFn       - Interpret a raw message
 * ReadFn        - Reading from a Device (see FHZ/WS300)
 * ReadyFn       - check for available data, if no FD
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



// typedef for AddFn - experimental - provided my module
typedef strTextMultiple_t* (* AddFn_t)(Common_Definition_t *Common_Definition, uint8_t *kvArgs, size_t kvArgsLen);

// typedef for AttributeFn - called in case of attribute changes for this definition, to check them - provided my module
typedef strTextMultiple_t* (* AttributeFn_t)(Common_Definition_t* Common_Definition, const uint8_t *attrCmdText, const size_t attrCmdTextLen, const uint8_t *attrNameText, const size_t attrNameTextLen, uint8_t **attrValTextPtr, size_t *attrValTextLenPtr);

// typedef for DefineFn - called to create a new definition of this type - provided my module
typedef strTextMultiple_t* (* DefineFn_t)(Common_Definition_t *Common_Definition);

//
typedef int (* DeleteFn_t)(Common_Definition_t *Common_Definition);		


//
typedef int (* DirectReadFn_t)(Common_Definition_t *Common_Definition);

// typedef for DirectWriteFn - for 2 stage designs - called to give write job to 1st stage - provided my module
/*typedef strTextMultiple_t* (* DirectWriteFn_t)(Common_Definition_t *Common_Definition_Stage1, Common_Definition_t *Common_Definition_Stage2, Common_StageXCHG_t *Common_StageXCHG);*/
typedef int (* DirectWriteFn_t)(Common_Definition_t *Common_Definition);

//
typedef int (* ExceptFn_t)(Common_Definition_t *Common_Definition);

//
typedef int (* FingerprintFn_t)(Common_Definition_t *Common_Definition);

// typedef for GetFn - for 2 stage designs - called to get data from this type - provided my module
typedef int (* GetFn_t)(Common_Definition_t *Common_Definition, Common_Definition_t *sourceCommon_Definition, void *X);
//typedef int (* GetFn_t)(Common_StageXCHG_t *Common_StageXCHG);

//
typedef int (* IdleCbFn_t)(Common_Definition_t *Common_Definition);

//
typedef int (* InitializeFn_t)(SCDERoot_t *SCDERoot);

//
typedef int (* NotifyFn_t)(Common_Definition_t *Common_Definition);

// typedef for ParseFn - for 2 stage designs - called to give job data to 2nd stage - provided my module
typedef strTextMultiple_t* (* ParseFn_t)(Common_StageXCHG_t *Common_StageXCHG);

//
typedef int (* ReadFn_t)(Common_Definition_t *Common_Definition);

//
typedef int (* ReadyFn_t)(Common_Definition_t *Common_Definition);

// typedef for RenameFn - called to inform the definition about its renameing - provided my module
typedef strTextMultiple_t* (* RenameFn_t)(Common_Definition_t *Common_Definition, uint8_t *newName, size_t newNameLen, uint8_t *oldName, size_t oldNameLen);

// typedef for SetFn - called to send data to the definition (opposite of Get) - provided my module
typedef strTextMultiple_t* (* SetFn_t)(Common_Definition_t *Common_Definition, uint8_t *setArgs, size_t setArgsLen);

// typedef for ShutdownFn - called to do activities before SCDE shuts down - provided my module
typedef strTextMultiple_t* (* ShutdownFn_t)(Common_Definition_t *Common_Definition);

// typedef for StateFn - called to set an state for this definition e.g. called from setstate cmd for recovery from save
//typedef strTextMultiple_t* (* StateFn_t)(Common_Definition_t *Common_Definition, time_t readingTiSt,
// uint8_t *readingName, size_t readingNameLen, uint8_t *readingValue, size_t readingValueLen,
// uint8_t *readingMime, size_t readingMimeLen);

// typedef for StateFn - called to set an state for this definition.
// Normally called from setstate cmd when recovering states from save from save
typedef xMultipleStringSLTQE_t* (* StateFn_t) (Common_Definition_t *Common_Definition, const time_t stateTiSt, const xString_t stateNameString, const xString_t stateValueString, const xString_t stateMimeString);

// typedef for SubFn - experimental - provided my module
typedef strTextMultiple_t* (* SubFn_t)(Common_Definition_t *Common_Definition, uint8_t *kArgs, size_t kArgsLen);

// typedef for UndefineFn - called when an definition is deleted, chance to cleanup - provided my module
typedef strTextMultiple_t* (* UndefineFn_t)(Common_Definition_t *Common_Definition);		

// typedef for WriteFn - called to write data to the definition
typedef xMultipleStringSLTQE_t* (* WriteFn_t) (Common_Definition_t *Common_Definition, xString_t data);



/* 
 * Provided by Module (struct)
 * - stores (the common) function callbacks for SCDE module operation
 * - information is sent to SCDE by module, when loaded
 * - done by InitializeFn after module the loaded
 */
struct ProvidedByModule_s {
  uint8_t typeName[32];		// Type-Name = Module Name
  size_t typeNameLen;

  AddFn_t AddFn;		//
  AttributeFn_t AttributeFn;	// verifyAttributeFn , called in case of attribute changes, to check them
  DefineFn_t DefineFn;		// defineDefinitionFn, called to create a new definition of this type
  DeleteFn_t DeleteFn;		// deleteDefinitionFn, to cleanup (delete log), called by delete after UndefFn
  DirectReadFn_t DirectReadFn;	// readDirectFn      , called from select loop to read
  DirectWriteFn_t DirectWriteFn;// writeDirectFn     , called from select loop to write
  ExceptFn_t ExceptFn;		//                   , called if the global select reports an except field
  GetFn_t GetFn;		// get some data from this device
  IdleCbFn_t IdleCbFn;		//                   , give module an Idle-Callback to process something
  InitializeFn_t InitializeFn;	// initializeModuleFn, set up module for operation (after load)
  NotifyFn_t NotifyFn;		//                   , call this if some device changed its properties
  ParseFn_t ParseFn;		//                   , Interpret a new message
  ReadFn_t ReadFn;		//                   , Reading / receiving from a Device
  ReadyFn_t ReadyFn;		//                   , check for available data, if no FD
  RenameFn_t RenameFn;		// renameDefinitionFn, called to inform the definition about its renameing
  SetFn_t SetFn;		// setDefinitionFn?  , set/activate this device
  ShutdownFn_t ShutdownFn;	//                   ,called before shutdown
  StateFn_t StateFn;		//                   ,set local info for this device, do not activate anything
  SubFn_t SubFn;		//                   ,called when Attribute-Keys owned by this Module are deleted
  UndefineFn_t UndefineFn;	//                   ,clean up (delete timer, close fd), called by delete and rereadcfg
  WriteFn_t WriteFn;		//
  void* CustomFn;		// ... provided by this Module (non-standard Fn). For other Modules.
  int SizeOfDefinition;//sizeOf..// Size of modul specific definition structure (Common_Definition_t + X)
};



//--------------------------------------------------------------------------------------------------



/*
 * Module (struct)
 * - stores information of loaded modules, required for module use and operation
 * - data is associated when module is loaded
 * - done in InitializeFn (after module load)
 */
struct Module_s {
  STAILQ_ENTRY (Module_s) entries;		// Link to next loaded Module
  ProvidedByModule_t *provided;				// Ptr to Provided by Module Info
  void *LibHandle;				// Handle to this loaded Module

 // place  Provided  FNS here direct ?
};



// -------------------------------------------------------------------------------------------------



// xReadingsSLTQE_t - Singly Linked Tail Queue Element to hold multiple Readings
typedef struct xReadingSLTQE_s xReadingSLTQE_t;

/*
 * xReadingsSLTQE_s (struct) 
 * - is an singly linked tail queue element and stores one Reading
 * - it is used when storing multiple Readings (in an singly tail linked queue)
 */
struct xReadingSLTQE_s {
  STAILQ_ENTRY(xReadingSLTQE_s) entries;	// link to next xReadingsSLTQE_s element
  time_t readingTist;				// timestamp of reading
  xString_t nameString;				// text of reading-name, in allocated mem
  xString_t valueString;			// text of reading-value, in allocated mem
};

/*
 * Constructor for the singly linked tail queue head, which may hold multiple linked Readings
 * SLTQ can be used for an FIFO queue by adding entries at the tail and fetching entries from the head
 * SLTQ is inefficient when removing arbitrary elements
 */
STAILQ_HEAD(readingsSLTQH_s, xReadingSLTQE_s);



// -------------------------------------------------------------------------------------------------



// bulkUpdateReadings_t - stores bulk update information and the readings SLTQH
typedef struct bulkUpdateReadings_s bulkUpdateReadings_t;

/* 
 * bulkUpdateReadings_s (struct)
 * - stores bulk update information and the readings singly linked tail queue head
 */
struct bulkUpdateReadings_s {
  struct readingsSLTQH_s readingsSLTQH;		// head of multiple updated Readings
  time_t bulkUpdateTist;			// timestamp of bulk update
};



// -------------------------------------------------------------------------------------------------



// Reading2 (type) - stores the content of an reading
typedef struct Reading2_s Reading2_t;

/*
 * Reading2 (struct) 
 * - stores the content of an reading
 * - it is used when storing multiple Readings (in an singly linked tail queue)
 */
typedef struct Reading2_s {
  time_t readingTist;				// timestamp of reading
  xString_t nameString;				// text of reading-name, in allocated mem
  xString_t valueString;			// text of reading-value, in allocated mem
} Reading2_t;





// -------------------------------------------------------------------------------------------------



// xReading2SLTQE (type) - Singly Linked Tail Queue Element (try2) to hold multiple Readings
typedef struct xReading2SLTQE_s xReading2SLTQE_t;

/*
 * Reading singly linked tail queue element  (struct) 
 * - stores information for one Reading
 * - it is used when storing multiple Readings (in an singly linked tail queue)
 */

struct xReading2SLTQE_s {
  STAILQ_ENTRY(xReading2SLTQE_s) entries;	// link to next Reading2SLTQE element
  Reading2_t* reading;				// ptr to the reading data
  Common_Definition_t* definition;		// the Definition the reading belongs to
};

/*
 * Constructor for the singly linked tail queue head, which may hold multiple linked Readings
 * SLTQ can be used for an FIFO queue by adding entries at the tail and fetching entries from the head
 * SLTQ is inefficient when removing arbitrary elements
 */
STAILQ_HEAD(readings2SLTQH_s, xReading2SLTQE_s);



// -------------------------------------------------------------------------------------------------



// bulkUpdateReadings2_t - stores bulk update information and the readings SLTQH
typedef struct bulkUpdateReadings2_s bulkUpdateReadings2_t;

/* 
 * bulkUpdateReadings2_s (struct)
 * - stores bulk update information and the readings singly linked tail queue head
 */
struct bulkUpdateReadings2_s {
  struct readings2SLTQH_s readings2SLTQH;	// head of multiple updated Readings
  time_t bulkUpdateTist;			// timestamp of bulk update
};



// -------------------------------------------------------------------------------------------------





/* 
 * Common Definition (struct)
 * - stores values for operation of an definition (device), common part,  valid only for the defined
 * - instance of an loaded module. Values are initialized by the SCDE and finalized by the
 *   loaded module itself (defineFn).
 */
struct Common_Definition_s {
  STAILQ_ENTRY(Common_Definition_s) entries;			// Link to next Definition
 
  STAILQ_HEAD (stailhead8, common_Attribute_s) headAttributes;	//p_head_attribute	// Link to assigned Attributes

//neu:
  LIST_HEAD (Head_Attr_Value_s, Entry_Attr_Value_s) head_attr_value;	// List of assigned / in use attr_names

 //? uint32_t* link;						//link to next struct

  // strText_t name; NEU
  uint8_t* name; //p_name;					// Ptr to the Name string (assigned by users declaration - in allocated mem)
  size_t nameLen; //name_len

  Module_t* module;						// Ptr to the Module_t (assigned when module is loaded)

// the 'STATE' reading						// the STATE reading is obligatory for every definition !
// strText_t reading_state_value; NEU
  uint8_t* state; //p_reading_state_value				// Ptr to allocated memory filled with oneliner-text describing its STATE
  size_t stateLen; //reading_state_value_len;			// and the length of the oneliner-text
  time_t stateTiSt; //reading_state_timestamp			// SCDE has also a timestamp for STATE

// strText_t definition; NEU
  uint8_t* definition;// p_definition				// Ptr to allocated memory filled with users definition string
  size_t definitionLen;// definition_len

  uint32_t nr;							// An unique sequential number assigned to definition
								// to rebuild /save in the order of definition-creation

  int fd;							// FileDescriptor. Used by selectlist / readyfnlist 
								// (-1 = not assigned)

  int Common_CtrlRegA; //common_control_register_a		// Common Control Reg A (enum Common_CtrlRegA from WEBIF.h)

  int defCtrlRegA; //common_definition_control_register_a	// Definition Control Reg A (enum Common_DefCtrlRegA)

  xSemaphoreHandle def_mux; //definition_mux 

  bulkUpdateReadings_t *bulkUpdateReadings;
  STAILQ_HEAD (stailhead6, reading_s) headReadings;//head_readings	// Link to assigned Attributes

  bulkUpdateReadings2_t *bulkUpdateReadings2;
  STAILQ_HEAD (stailhead7, reading2_s) headReadings2;//head_readings2	// Link to assigned Attributes

  // Pointer to ActiveResourcesDataA, set at init time.
  void* ActiveResourcesDataA; //active_resource_data_a
  // Pointer to ActiveResourcesDataB, set at init time.
  void* ActiveResourcesDataB; //active_resource_data_b

  /* todo
#Special values in %defs:
# READINGS- The readings. Each value has a "VAL" and a "TIME" component.
# IODev   - attached to io device
# CHANGED - Currently changed attributes of this device. Used by NotifyFn
# VOLATILE- Set if the definition should be saved to the "statefile"
# NOTIFYDEV - if set, the notifyFn will only be called for this device
*/
};

// Information Flags, stored in Common_Definition_s - for Definition Control
enum Common_DefCtrlRegA { //common_definition_control_register_a
//hinzu CDCRA_Flag
    F_TEMPORARY		= 1 << 0	// indicates:Definition is temporary e.g. WEBIf connection
  , F_VOLATILE		= 1 << 1	// indicates:Definition is volatile -> saved to statefile
  , CDCRA_F_weitere	= 1 << 2	//   
};

// Information Flags, stored in Common_Definition_s - for Connection Control
enum Common_CtrlRegA {	//common_control_register_a
//hinzu CCRA_Flag
    F_WANTS_WRITE		= 1 << 0	// indicates: define (FD) wants to write data (call DirectWriteFn), if ready
  , F_WANTS_IDLE_TASK		= 1 << 1	// indicates: define (FD) wants idle-task to be called (call IdleTaskFn)
  , CCRA_F_weitere		= 1 << 2	//   
};



// -------------------------------------------------------------------------------------------------



// oder Common_DirectWriteJob_t
// Common_StageXCHG_t stores values for operation of 2. stage design Modules
typedef struct Common_StageXCHG_s Common_StageXCHG_t;



/* 
 * Common_StageXCHG_s (struct)
 * - stores values for operation of 2. stage design Modules (stage exchange)
 * - the definitions for callbacks
 */
struct Common_StageXCHG_s {

  Common_Definition_t *definitionStage1;	// ptr to the definition that is 1 st stage
  Common_Definition_t *definitionStage2;	// ptr to the definition that is 2 nd stage

  int stageCtrlRegA;		// Stage processing Control Register A (enum Common_stageCtrlRegA)
};

// Information Flags, stored in Common_Definition_s - for Connection Control
enum stageCtrlRegA {
    SCRA_F_JOB_DONE			= 1 << 0	// indicates: the job is done
  , SCRA_F_ERROR			= 1 << 1	// indicates: ?
  , SCRA_F_weitere			= 1 << 2	// indicates: ?
};



// -------------------------------------------------------------------------------------------------
// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ typedefs and structs for Command operation ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓



// typedef for InitializeCommandFn - called to initialize an command (once called after loading it)
typedef int (* InitializeCommandFn_t)(SCDERoot_t* SCDERoot);

// typedef for CommandFn - when SCDE finds matching command name, this Fn will be called - to exec.
typedef struct headRetMsgMultiple_s (* CommandFn_t) (const uint8_t *args, const size_t argsLen);

// Provided by Command (type) - stores function callbacks and information needed for cmd operation
typedef struct ProvidedByCommand_s ProvidedByCommand_t;

//--------------------------------------------------------------------------------------------------

/* 
 * Provided by Command (struct)
 * - stores function callbacks and information for a modular command
 * - information is associated when the command is loaded
 * - (done in InitializeFn after command load)
 */
struct ProvidedByCommand_s {

// strText_t nameString; NEU name_string		// name text of command
  uint8_t commandNameText[32];	//command_name_text		// name text of command
  size_t commandNameTextLen; //command_name_text_len

  InitializeCommandFn_t InitializeCommandFn;	// returns module information (module_s)
  CommandFn_t CommandFn;			// the command Fn
  const xString_t helpString; //help_string	// help text
  const xString_t helpDetailString; //help_detail_string	// detailed help text
};

//--------------------------------------------------------------------------------------------------

/* 
 * Commands (struct)
 * - stores commands made available for operation.
 * - some are buildin and initialized after start, others are loaded by user
 */
struct Command_s {
  STAILQ_ENTRY (Command_s) entries;		// link to next loaded command
  ProvidedByCommand_t* provided; // p_provided		// ptr to provided-by-command Info
};



// -------------------------------------------------------------------------------------------------
// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ typedefs and structs for Attribute operation ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓



// attribute_t holds attributes assigned to definitions, for operation and customization.
typedef struct attribute_s attribute_t;

/*
 * attribute_s (struct)
 * - stores attributes assigned to definitions, for operation and customization.
 */
struct attribute_s { //Attribute_s

  STAILQ_ENTRY(attribute_s) entries;	// link to next attribute

//  Common_Definition_t* assignedDef; NEU definition	
  Common_Definition_t* attrAssignedDef;	//p_definition // ptr to the definition the attribute is assigned to

// strText_t name; NEU name
  uint8_t* attrNameText; //p_name_text		// ptr to the attribute name text (text in allocated mem!)
  size_t   attrNameTextLen; //name_text_len

// strText_t value; NEU value
  uint8_t* attrValText; //p_value_text		// ptr to the attribute value text (text in allocated mem!)
  size_t   attrValTextLen; //value_len
};



// -------------------------------------------------------------------------------------------------


















// -------------------------------------------------------------------------------------------------

// Entry_Definitions_s to hold the result of ...multiple xString_t strings (in an singly linked tail queue)
typedef struct Entry_Definitions_s Entry_Definitions_t;

/*
 * Entry_Definitions_s (struct) 
 * - is an singly linked tail queue entry and stores the link to one Common_Definition_t
 * it is used when storing multiple strings (in an singly tail linked queue)
 */
struct Entry_Definitions_s {
  STAILQ_ENTRY(Entry_Definitions_s) entries;	// link to next element of the singly linked tail queue
  Common_Definition_t* p_entry_definition;	// the definition of this element
};

/*
 * Constructor for the singly linked tail queue (head), which can hold multiple linked strings
 * SLTQ can be used for an FIFO queue by adding entries at the tail and fetching entries from the head
 * SLTQ is inefficient when removing arbitrary elements
 */
STAILQ_HEAD(Head_Definitions_s, Entry_Definitions_s);


// -------------------------------------------------------------------------------------------------


// Entry_Attr_Name_t holds the name of an assigned attribute (attr_name). (linked list from queue.h)
typedef struct Entry_Attr_Name_s Entry_Attr_Name_t;

/*
 * Entry_Attr_Name_s (struct)
 * - holds the name of an assigned attribute (attr_name). (linked list from queue.h)
 */
struct Entry_Attr_Name_s {
  LIST_ENTRY(Entry_Attr_Name_s) entries;		// links to next list entries
  String_t attr_name;					// an assigned Attribute Name
};


// -------------------------------------------------------------------------------------------------


// Entry_Attr_Value_t holds assigned attribute-values. (linked list from queue.h)
typedef struct Entry_Attr_Value_s Entry_Attr_Value_t;

/*
 * Entry_Attr_Value_s (struct)
 * - holds assigned attribute-values. (linked list from queue.h)
 */
struct Entry_Attr_Value_s {
  LIST_ENTRY(Entry_Attr_Value_s) entries;	// links to next list entries
  Common_Definition_t* p_entry_definition;	// ptr to the definition-entry the attribute is assigned to
  Entry_Attr_Name_t* p_entry_attr_name;		// ptr to the attribute-name-entry the attribute is assigned to
  String_t attr_value;				// the assigned value MAY BE NULL IF NO VALUE!
};


// -------------------------------------------------------------------------------------------------


/* 
 * SCDERoot (struct)
 * - Smart Connected Devices Engine - root data
 */
struct SCDERoot_s {
  SCDEFn_t* SCDEFn; //p_scde_fn		// SCDEFn (Functions / callbacks) for faster operation
//use vars qw($auth_refresh);
//use vars qw($cmdFromAnalyze);   # used by the warnings-sub
//use vars qw($cvsid);            # used in 98_version.pm
  strText_t current_config_file;	//current_config_file
  uint32_t device_count;		// Is used to store Definitions in the right order. 1st=1
  uint32_t feature_level;		// for version management
  uint32_t global_control_register_a;	// -> 'enum global_control_register_a'
  time_t last_timestamp;		// to check if requested TiSt is unique (higher than last time)
//use vars qw($fhem_started);     # used for uptime calculation
//use vars qw($init_done);        #
//use vars qw($internal_data);    # FileLog/DbLog -> SVG data transport
//use vars qw($lastDefChange);    # number of last def/attr change
//use vars qw($nextat);           # Time when next timer will be triggered.
//use vars qw($readytimeout);     # Polling interval. UNIX: device search only
//use vars qw($reread_active);
//use vars qw($selectTimestamp);  # used to check last select exit timestamp
//use vars qw($winService);       # the Windows Service object
  STAILQ_HEAD (stailhead3, attribute_s) headAttributes;//head_attribute	// Link to assigned attributes
  STAILQ_HEAD (stailhead4, Command_s) headCommands;//head_command	// Link to available commands
//use vars qw(%data);             # Hash for user data
//use vars qw(%defaultattr);      # Default attributes, used by FHEM2FHEM
  STAILQ_HEAD (stailhead2, Common_Definition_s) HeadCommon_Definitions;//head_common_definition// Link to Definitions (device, button, ...)
//use vars qw(%inform);           # Used by telnet_ActivateInform
//use vars qw(%intAt);            # Internal at timer hash, global for benchmark
//use vars qw(%logInform);        # Used by FHEMWEB/Event-Monitor
  STAILQ_HEAD (stailhead1, Module_s) HeadModules;//head_module	// Link to loaded Modules
//use vars qw(%ntfyHash);         # hash of devices needed to be notified.
//use vars qw(%oldvalue);         # Old values, see commandref.html
//use vars qw(%readyfnlist);      # devices which want a "readyfn"
//use vars qw(%selectlist);       # devices which want a "select"
//use vars qw(%value);            # Current values, see commandref.html
//use vars qw(@authenticate);     # List of authentication devices
//use vars qw(@authorize);        # List of authorization devices
//use vars qw(@structChangeHist); # Contains the last 10 structural changes

  LIST_HEAD (Head_Attr_Name_s, Entry_Attr_Name_s) head_attr_name;	// List of assigned attr_names
};

// global control register - some global flags - e.g. for Connection Control
enum global_control_register_a
{
//NEU: GCRA_F lag ändern
    F_RECEIVED_QUIT		= 1 << 0	// indicates: quit the include processing
  , F_INIT_DONE			= 1 << 1	// indicates: the SCDE has fiinished the init process
  , GCRA_F_weitere		= 1 << 2	//   
};



#endif /*_SCDE_S_H_*/
