#ifndef _SCDE_H_
#define _SCDE_H_

// -------------------------------------------------------------------------------------------------

// set default build verbose - if no external override
#ifndef CORE_SCDE_DBG  
#define CORE_SCDE_DBG  5	// 5 is default
#endif 

// set default build verbose - if no external override
#ifndef Helpers_SCDE_DBG  
#define Helpers_SCDE_DBG  5	// 5 is default
#endif 

// -------------------------------------------------------------------------------------------------


// stores the Root Data of the Smart Connected Devices Engine
SCDERoot_t SCDERoot;

// stores SCDEFn (Functions / callbacks) for operation (provided / made accessible for modules)
SCDEFn_t SCDEFn;


// inits the Root Data of the Smart Connected Devices Engine
void InitSCDERoot(void);

// ?
void InitA();


// -------------------------------------------------------------------------------------------------



/*
 * Functions provided by SCDE (Fn callbacks) 
 * AND made accessible for calling by modules and commands (for SCDE operation and helpers )
 */

// analyzes + processes one command row
struct headRetMsgMultiple_s AnalyzeCommand(const uint8_t *args, const size_t argsLen);

// analyzes + processes an configuration file
struct headRetMsgMultiple_s AnalyzeCommandChain(const uint8_t *args, const size_t argsLen);

//
int CallGetFnByDefName(const uint8_t *nameText, const size_t nameTextLen, Common_Definition_t *sourceCommon_Definition, void *X);

//
Module_t* CommandReloadModule(const uint8_t *typeName, const size_t typeNameLen);

// returns all definitions that match devspec
struct xHeadMultipleStringSLTQ_s Devspec2Array(const xString_t devspecString);

// returns formated text of Date-Time from tist
strText_t FmtDateTime(time_t tiSt);

// returns formated text of Time from tist
strText_t FmtTime(time_t tiSt);

// returns ALL Readings of the Definition
struct headRetMsgMultiple_s GetAllReadings (Common_Definition_t *Common_Definition);

// returns the Definition line and the Attribute line
struct headRetMsgMultiple_s GetDefAndAttr(Common_Definition_t *Common_Definition);

//
Common_Definition_t* GetDefinitionPtrByName(const size_t definitionNameLen, const uint8_t *definitionName);

// Returns a STAILQ head that stores entries of all 'dev_spec' matching definitions
struct Head_Definitions_s Get_Definitions_That_Match_DefSpec_String(const String_t dev_spec);

//
Module_t* GetLoadedModulePtrByName(const uint8_t *typeName, const size_t typeNameLen);

// returns current SCDE Time Stamp
time_t GetTiSt(void);

// returns an UNIQUE SCDE Time Stamp (in most cases the current Time Stamp)
time_t GetUniqueTiSt(void);

// checks the given Device Name for device name rules
bool GoodDeviceName(const xString_t nameString);

// checks the given Reading Name for device name rules
bool GoodReadingName(const xString_t nameString);

// prints data as Hex-Dump to debug terminal
void HexDumpOut (char *desc, void *addr, int len);

//
void Log(char *Device, int LogLevel, char *Text);

//
void Log3(const uint8_t *name, const size_t nameLen, const uint8_t LogLevel, const char *format, ...);

//
void Log4(char *text);

// corrects the given Device Name
void MakeDeviceName(const xString_t nameString);

// corrects the given Reading Name
void MakeReadingName(const xString_t nameString);

// call this before updating readings
int readingsBeginUpdate(Common_Definition_t *Common_Definition);

// call this to add an Reading to the running update of Readings
int readingsBulkUpdate(Common_Definition_t *Common_Definition, uint8_t *readingNameText, size_t readingNameTextLen, uint8_t *readingValueText, size_t readingValueTextLen);

// call this to add an Reading to the running update of Readings
int readingsBulkUpdate2(Common_Definition_t *Common_Definition, const size_t readingNameStringLength, const uint8_t *readingNameStringCharacters, const size_t readingValueStringLength, const uint8_t *readingValueStringCharacters,const bool changed);

// call this to after bulk-update to process readings
int readingsEndUpdate(Common_Definition_t *Common_Definition, bool doTrigger);

// returns current system time. (IT IS NOT AN TIME STAMP)
time_t TimeNow();

//
struct headRetMsgMultiple_s WriteStatefile();



// -------------------------------------------------------------------------------------------------



/*
 * Functions provided by SCDE (Fn callbacks) 
 * AND NOT made accessible for calling by modules and commands (internal Fn)
 */

//
Module_t* CommandActivateModule (ProvidedByModule_t* ProvidedByNEWModule);

//
Command_t* CommandActivateCommand (ProvidedByCommand_t* providedByNEWCommand);

//
int CommandLoadCommand(const uint8_t *commandTxt, const size_t commandTxtLen, CommandFn_t CommandFn, const uint8_t *commandHelp, const size_t commandHelpLen);


// internal Fn, but made available to Modules and Commands

//deleted Common_Definition_t* AddDefineForModuleWithName(Module_t* ModulePtr, const char* Name);

//Common_Definition_t*      AddDefineForModuleWithName(Module_t* ModulePtr, char* Name);




//char* CommandDefine (const uint8_t *args, const size_t argsLen);
//char* CommandSet (const uint8_t *args, const size_t argsLen);
strTextMultiple_t*          CommandUndefine (const uint8_t *args, const size_t argsLen);
// Attribute Management
char*                       GetAttribute(char* Definition, char* AttributeName);
bool                        AssignAttribute(char* Definition, char* AttributeName, char* Attribute);
bool                        DeleteAttribute(char* Definition, char* AttributeName);

// Initializes the global device
void doGlobalDef(const uint8_t *cfgFileName, const size_t cfgFileNameLen);

//
String_t* Get_Attr_Val_By_Def_Name_And_Attr_Name(const String_t* p_def_name, const String_t* p_attr_name);






// prints data as Hex-Dump to debug terminal
// doppeltvoid HexDumpOut (char *desc, void *addr, int len);

parsedKVInputArgs_t* ParseKVInputArgs(int numImplementedKeys, const kvParseImplementedKeys_t *XX_IK, const uint8_t *kvArgsText, const size_t kvArgsTextLen);




#endif /*_SCDE_H_*/
