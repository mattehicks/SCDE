// SCDE-Module ESP32_S0 (S0-Counter)

#ifndef ESP32_S0_MODULE_H
#define ESP32_S0_MODULE_H



#include "SCDE_s.h"
#include "WebIf_Module global types.h"



/* 
 * ESP32 Hardware Counter Pulse Count (RO)
 * ESP32 has 8 counter units. Base adress is 0x3FF57060
 *
 * Sample to make Pulse Count registers accessible:
 * ESP32_S0_CNT_CNT_REG_t[8] *ESP32_S0_CNT_CNT_REG =
 * 	= (ESP32_S0_CNT_CNT_REG_t[8]) 0x3FF57060;
 */
typedef struct ESP32_S0_CNT_CNT_REG_s {

// CNT_CNT_REG (Counter Count Register)
  int32_t  CNT_PULS_CNT : 16; //PULS_CNT	// stores the current pulse count value. (RO)

  uint32_t  CNT_CNT_REG_RESERVED : 16;		// reserved

} ESP32_S0_CNT_CNT_REG_t;



/* 
 * ESP32 Hardware Counter Configuration (R / W)
 * ESP32 has 8 counter units. Base adress is 0x3FF57000
 *
 * Sample to make configuration registers accessible:
 * ESP32_S0_CNT_CONF_REG_t *pESP32_S0_CNT_CONF_REG =
 *	(ESP32_S0_CNT_CONF_REG_t*) 0x3FF57000;
 * Then writeable by: pESP32_S0_CNT_CONF_REG[x]->
 */
typedef struct ESP32_S0_CNT_CONF_REG_s {

// CNT_CONF0_REG (count configuration register 0)
  uint32_t CNT_FILTER_THRES : 10; 		// sets maximum threshold, in APB_CLK cycles, for 
						// the filter. Any pulses lasting shorter than this 
						// will be ignored when the filter is enabled. (R/W)

  uint32_t CNT_FILTER_EN : 1; 			// enable bit for input filter. (R/W)

  uint32_t CNT_THR_ZERO_EN : 1; 		// enable bit for zero comparator. (R/W)

  uint32_t CNT_THR_H_LIM_EN : 1; 		// enable bit for thr_h_lim comparator. (R/W)

  uint32_t CNT_THR_L_LIM_EN : 1; 		// enable bit for thr_l_lim comparator. (R/W)

  uint32_t CNT_THR_THRES0_EN : 1; 		// enable bit for thres0 comparator. (R/W)

  uint32_t CNT_THR_THRES1_EN : 1; 		// enable bit for thres1 comparator. (R/W)

  uint32_t CNT_CH0_NEG_MODE : 2; 		// sets the behaviour when the signal input of
						// channel 0 detects a negative edge. (R/W)
						// 1: Increase the counter; 2: Decrease the counter;
						// 0, 3: No effect on counter

  uint32_t CNT_CH0_POS_MODE : 2;		// sets the behaviour when the signal input of
						// channel 0 detects a positive edge. (R/W) 
						// 1: Increase the counter; 2: Decrease the counter;
						// 0, 3: No effect on counter

  uint32_t CNT_CH0_HCTRL_MODE : 2; 		// configures how the CH0_POS_MODE/CH0_NEG_MODE
						// settings will be modified when the control signal
						// is low. (R/W) 0: No modification; 1: Invert behaviour
						// (increase -> decrease, decrease -> increase);
						// 2, 3: Inhibit counter modification

  uint32_t CNT_CH0_LCTRL_MODE : 2;		// configures how the CH0_POS_MODE/CH0_NEG_MODE
						// settings will be modified when the control signal
						// is low. (R/W) 0: No modification; 1: Invert behaviour
						// (increase -> decrease, decrease -> increase);
						// 2, 3: Inhibit counter modification

  uint32_t CNT_CH1_NEG_MODE : 2; 		// sets the behaviour when the signal input of
						// channel 1 detects a negative edge. (R/W)
						// 1: Increment the counter; 2: Decrement the counter;
						// 0, 3: No effect on counter

  uint32_t CNT_CH1_POS_MODE : 2;		// sets the behaviour when the signal input of
						// channel 1 detects a positive edge. (R/W)
						// 1: Increment the counter; 2: Decrement the counter;
						// 0, 3: No effect on counter

  uint32_t CNT_CH1_HCTRL_MODE : 2; 		// configures how the CH1_POS_MODE/CH1_NEG_MODE
						// settings will be modified when the control signal
						// is low. (R/W) 0: No modification; 1: Invert behaviour
						// (increase -> decrease, decrease -> increase);
						// 2, 3: Inhibit counter modification

  uint32_t CNT_CH1_LCTRL_MODE : 2;		// configures how the CH1_POS_MODE/CH1_NEG_MODE
						// settings will be modified when the control signal
						// is low. (R/W) 0: No modification; 1: Invert behaviour
						// (increase -> decrease, decrease -> increase);
						// 2, 3: Inhibit counter modification

// CNT_CONF1_REG (count configuration register 1)
  uint32_t  CNT_CNT_THRES0 : 16; 		// configures the thres0 value. (R/W)

  uint32_t  CNT_CNT_THRES1 : 16; 		// configures the thres1 value. (R/W)

// CNT_CONF2_REG (count configuration register 2)
  uint32_t  CNT_CNT_H_LIM : 16; 		// configures the thr_h_lim value. (R/W)

  uint32_t  CNT_CNT_L_LIM : 16; 		// configures the thr_l_lim value. (R/W)

} ESP32_S0_CNT_CONF_REG_t;




/* 
 * ESP32_S0_Definition stores values for operation valid only for the defined instance of an
 * loaded module. Values are initialized by HCTRLD an the loaded module itself.
 */
typedef struct ESP32_S0_Definition_s {

  Common_Definition_t common;		// ... the common part of the definition

  WebIf_Provided_t WebIf_Provided;	// provided data for WebIf


  //uint32_t Capabilities;		// caps -> capabilities of this Devices S0 Feature - HARDCODED!
//  char IB01_Name[31];			// name -> custom name of the LiGHT = MAX 31+1 byte

  float IB01_AverageFactor;   		// avf -> averageXXmin X average-factor = value at average display (+ average unit) 

  char IB01_AverageUnit[3];		// avu -> resulting average-unit (e.g. kWh) MAX 3+1!

  float IB01_AlltimeTotalFactor;  	// atf -> alltime-total-tics X alltime-total-factor => alltime Value at display of meter (+ alltime total unit) 

  char IB01_AlltimeTotalUnit[3];	// atu -> resulting total-unit (e.g. kWh) MAX 3+1!

  uint32_t IB01_Debounce;		// deb -> GPIO debounce value (1-255)

  uint32_t IB01_SettingsBF;		// XXX -> Settings-Bit-Field (32 Bits)

  uint8_t unit;				// the assigned ESP32 hardware counter unit 
  uint8_t unused1;
  uint8_t unused2;
  uint8_t unused3;
  uint8_t pulseCH0GPIO;			// connected GPIO for pulse input channel 0
  uint8_t controlCH0GPIO;		// connected GPIO for control input channel 0 (if used)
  uint8_t pulseCH1GPIO;			// connected GPIO for pulse input channel 1 (if used)
  uint8_t controlCH1GPIO;		// connected GPIO for control input channel 1 (if used)			// the assigned ESP32 hardware counter unit 

 // verschiben ins ram only??????????????
  //--- Buffer for calculations (to big for RTC Backup) ---
  uint32_t ATTShiftBuf[16];		// Alltime-Total-Tics NOW + 15 Alltime-Total-Tics Shiftbuffer 		// löschen TotalTics[16];		


  uint32_t RB01_AlltimeTotalTics;	// att -> actual alltime-total-tics value

  //float RB01_AlltimeTotalValue	// atv -> actual alltime-total-value (its a calculated reading!)


  float RB01_Average01Min;		// av01 -> calculated average last 01 minute

  float RB01_Average05Min;		// av05 -> calculated Average last 05 Minutes

  float RB01_Average15Min;		// av15 -> calculated Average last 15 Minutes

  int32_t IB01_X_TiSt;			// Timestamp for Internals-Block-1 sync (last change)

  int32_t RB01_X_TiSt;			// Timestamp for Readings-Block-1 sync (last change)
 


  } ESP32_S0_Definition_t;



//IBXX_ -> Internals timestamp sharing block NO XX, for settings
//ABXX_ -> Attributes timestamp sharing block NO XX, for user settings
//RBXX_ -> Readings timestamp sharing block NO XX, for features current readings
/*                Stored in Ram (No Mirror) -+ -> for fast access, reset clears struct
 *           Stored in Ram (RTC-Mirrored) -+ | -> for keeping values alive after reset/crash
 *       Stored in Ram (Flash Mirrored) -+ | | -> for fast access ?
 *               Stored only in Flash -+ | | | -> for memory efficient, but slow programming
 *                                     | | | |
 *          S0_Feature                 | | | |
 *                                     | | | |
 *    --- Internals - Block 01 ---     | | | |
 * IB01_Name char[31]                  | X | |  name -> custom name of the LiGHT = MAX 31+1 byte
 * float IB01_AverageFactor            | X | |  avf -> averageXXmin X average-factor = value at average display (+ average unit) 
 * char IB01_AverageUnit[3]            | X | |  avu -> resulting average-unit (e.g. kWh) MAX 3+1!
 * float IB01_AlltimeTotalFactor       | X | |  atf -> alltime-total-tics X alltime-total-factor => alltime Value at display of meter (+ alltime total unit) 
 * char IB01_AlltimeTotalUnit[3]       | X | |  atu -> resulting total-unit (e.g. kWh) MAX 3+1!
 * unsigned int IB01_Debounce          | X | |  deb -> GPIO debounce value (1-255)
 * unsigned int IB01_SettingsBF        | X | |  XXX -> Settings-Bit-Field (32 Bits)
 * int32_t IB01_X_TiSt                 | | | X  tist -> for Internals-Block-01 sync
 *                                     | | | |
 *   --- Attributes - Block 01 ---     | | | |  
 *                                     | | | |
 *    --- Readings - Block 01 ---      | | | |
 * uint32_t RB01_AlltimeTotalTics      | | X |  att -> actual alltime-total-tics value
 * float RB01_Average01Min             | | | X  av01 -> calculated average last 01 minute
 * float RB01_Average05Min             | | | X  av05 -> calculated Average last 05 Minutes
 * float RB01_Average15Min             | | | X  av15 -> calculated Average last 15 Minutes
 * int32_t RB01_X_TiSt                 | | | X  tist -> for Readings-Block-01 sync
 *
 */








/*
 *  Functions provided to SCDE by module for type operation
 */
strTextMultiple_t* ESP32_S0_Attribute(Common_Definition_t* Common_Definition, const uint8_t *attrCmdText, const size_t attrCmdTextLen, const uint8_t *attrNameText, const size_t attrNameTextLen, uint8_t **attrValTextPtr, size_t *attrValTextLenPtr);
strTextMultiple_t* ESP32_S0_Define(Common_Definition_t *Common_Definition);
int                ESP32_S0_IdleCb(Common_Definition_t *Common_Definition);
int                ESP32_S0_Initialize(SCDERoot_t* SCDERoot);
strTextMultiple_t* ESP32_S0_Rename(Common_Definition_t *Common_Definition, uint8_t *newName, size_t newNameLen, uint8_t *oldName, size_t oldNameLen);
strTextMultiple_t* ESP32_S0_Set(Common_Definition_t* Common_Definition, uint8_t *setArgs, size_t setArgsLen);
strTextMultiple_t* ESP32_S0_Shutdown(Common_Definition_t* Common_Definition);
strTextMultiple_t* ESP32_S0_Undefine(Common_Definition_t* Common_Definition);




/*
 *  helpers provided to module for type operation
 */
bool ESP32_S0_ProcessKVInputArgs(ESP32_S0_Definition_t* ESP32_S0_Definition, parsedKVInputArgs_t *parsedKVInput, uint8_t *argsText, size_t argsTextLen);
bool ESP32_S0_SetAffectedReadings(ESP32_S0_Definition_t* ESP32_S0_Definition, uint64_t affectedReadings);
void ESP32_S0_CntProcessing1M(ESP32_S0_Definition_t* ESP32_S0_Definition);


#endif /*ESP32_S0_MODULE_H*/
