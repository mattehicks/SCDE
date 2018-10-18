// SCDE-Module ESP32_SwITCH (switch GPIOs by PWM-hardware)

#ifndef ESP32_SwITCH_MODULE_H
#define ESP32_SwITCH_MODULE_H



#include "SCDE_s.h"
#include "WebIf_Module global types.h"



/* 
 * Struct for ESP-32 PWM hardware timer adressing - they are the clock source for the PWM signal
 * ESP32 has 4 high speed + 4 low speed timers. Base adress is
 * 0x3FF59140 for high speed timer 0 / 0x3FF59160 for low speed timer 0
 *
 *  // pointer to make the hardware timers accessible
 *  ESP32_PWM_Timer_t *pPWM_Timer =
 *	(ESP32_PWM_Timer_t*) (0x3FF59140 + (0x08 * newTimer));
 *  // access low speed timers (instead of high speed timers)?
 *  if (newBlock) pPWM_Timer =
 *	(ESP32_PWM_Timer_t*) ( (uint8_t*) pPWM_Timer + 0x20);
 */
typedef struct ESP32_PWM_Timer_s {

  // LEDC_TIMER_CONF_REG
  uint32_t RESOLUTION : 5;			// controls the bit-resolution of the counter. The counter range is
						// [0,2**reg_hstimerx_lim], the maximum bit width is 20. (R/W)

  uint32_t DIV_FACTOR : 18;			// Sets the division factor to generate the counter
						// value from the selected clock source.
						// The least significant eight bits are the fractional part. (R/W)
  uint32_t PAUSE : 1;				// Suspends the counter in high-speed timer. (R/W)

  uint32_t RESET : 1;				// Resets the counter value to ’zero’. (R/W)

  uint32_t TICK_SOURCE : 1;			// Selects clock source - depends on HS / LS timer. (R/W)
						// High-speed block -> 1: APB_CLK / 0: REF_TICK
						// Low-speed block -> 1: SLOW_CLK / 0: REF_TICK

  uint32_t LOW_SPEED_UPDATE : 1; 		// To take new values - Low speed timer channels only !
						// reserved for high speed timers

  uint32_t TIMER_CONF_REG_RESERVED : 5;		// reserved
// not needed
  // LEDC_HSTIMER_VALUE_REG // 0x3FF59194 for t0
  uint32_t LEDC_HSTIMER_CNT : 20; 		// register holds the current counter value of the high-speed timer (RO)

  uint32_t LEDC_HSTIMER_VALUE_REG : 6;		// reserved

} ESP32_PWM_Timer_t;



/*
 * Struct for ESP-32 PWM hardware channel adressing - they are generating the PWM signal
 * ESP32 has 4 high speed + 4 low speed pwm-channels. Base adress is
 * 0x3FF59000 for high speed channel 0 / 0x3FF590A0 for low speed channel 0
 *
 *  // pointer to make the hardware counters accessible
 *  ESP32_PWM_Channel_t *pPWM_Channel =
 *	(ESP32_PWM_Channel_t*) (0x3FF59000 + (0x14 * newChannel));
 *  // access low speed channels (instead of high speed channels)?
 *  if (newBlock) pPWM_Channel =
 *	(ESP32_PWM_Channel_t*) ( (uint8_t*) pPWM_Channel + 0xa0);
 */
typedef struct ESP32_PWM_Channel_s {

  // CONF0_REG
  uint32_t TIMER_SEL : 2; 			// selects the timer LS/HS (0-3) that should be connected as source.

  uint32_t SIG_OUT_EN : 1; 			// output enable control bit for this channel. (R/W)

  uint32_t IDLE_LV : 1; 			// controls the output level when this channel is inactive. (R/W)

  uint32_t PARA_UP : 1;				// update register LEDC_LSCHn_HPOINT and LEDC_LSCHn_DUTY 
						// (for low-speed channels only !!!)  (R/W)
  
  uint32_t CONF0_REG_RESERVED : 26; 		// reserved

  uint32_t CLK_EN : 1; 				// This bit is clock gating control signal. 
						// when software configure LED_PWM internal registers
                                                // it controls the register clock.

  // HPOINT_REG
  uint32_t HPOINT : 20;				// The output value changes to high when htimerx(x=[0,3]), selected by LS/HS
						// channel n, has reached reg_hpoint_hschn[19:0]. (R/W)

  uint32_t HPOINT_REG_RESERVED : 12; 		// reserved


  // DUTY_REG
  uint32_t DUTY_fract_bits : 4;

  uint32_t DUTY : 21; 				// control the output duty. When hstimerx(x=[0,3]) has reached reg_lpoint_hschn,
						// ? the output signal changes to low. (R/W)
						// ? reg_lpoint_hschn=(reg_hpoint_hschn[19:0]+reg_duty_hschn[24:4]) (1)
						// ? reg_lpoint_hschn=(reg_hpoint_hschn[19:0]+reg_duty_hschn[24:4] +1) (2)

  uint32_t DUTY_REG_RESERVED : 7; 		// reserved

  // CONF1_REG
  uint32_t DUTY_SCALE : 10; 			// increase or decrease the step scale for LS/HS channel. (R/W)

  uint32_t DUTY_CYCLE : 10;			// increase or decrease the duty cycle every time REG_DUTY_CYCLE_HSCH cycles. (R/W)

  uint32_t DUTY_NUM : 10; 			// control the number of times the duty cycle is increased or decreased. (R/W)

  uint32_t DUTY_INC : 1; 			// increase or decrease the duty of output signal. (R/W)

  uint32_t DUTY_START : 1; 			// When REG_DUTY_NUM_HSCH, REG_DUTY_CYCLE_HSCH and REG_DUTY_SCALE_HSCHn has been
						// configured, these vales will not take effect until REG_DUTY_START_HSCHn is set.
						// This bit is automatically cleared by hardware. (R/W)
// we dont need this
  // DUTY_R_REG
  uint32_t DUTY_CYCLEX : 25;			// This register represents the current duty cycle of the output signal. (RO)

  uint32_t DUTY_R_REG_RESERVED : 7; 		// reserved 

} ESP32_PWM_Channel_t;



/* 
 * ESP32_SwITCH_Definition stores values for operation valid only for the defined instance of an
 * loaded module. Values are initialized by HCTRLD an the loaded module itself.
 */
typedef struct ESP32_SwITCH_Definition_s {

  Common_Definition_t common;		// ... the common part of the definition

  WebIf_Provided_t WebIf_Provided;	// provided data for WebIf


  //uint32_t Capabilities;		// caps -> capabilities of this Devices S0 Feature - HARDCODED!
//  char IB01_Name[31];			// name -> custom name of the LiGHT = MAX 31+1 byte

  float IB01_AverageFactor;   		// avf -> averageXXmin X average-factor = value at average display (+ average unit) 

  char  IB01_AverageUnit[3];		// avu -> resulting average-unit (e.g. kWh) MAX 3+1!

  float IB01_AlltimeTotalFactor;  	// atf -> alltime-total-tics X alltime-total-factor => alltime Value at display of meter (+ alltime total unit) 

  char  IB01_AlltimeTotalUnit[3];	// atu -> resulting total-unit (e.g. kWh) MAX 3+1!

  uint32_t IB01_Debounce;		// deb -> GPIO debounce value (1-255)

  uint32_t IB01_SettingsBF;		// XXX -> Settings-Bit-Field (32 Bits)

//  uint8_t GPIO;				// 
  uint8_t unused2;
  uint8_t unused3;
  uint8_t counterUnit;			// assigned ESP32 hardware counter unit 

 // verschiben ins ram only??????????????
  //--- Buffer for calculations (to big for RTC Backup) ---
  uint32_t ATTShiftBuf[16];		// Alltime-Total-Tics NOW + 15 Alltime-Total-Tics Shiftbuffer 		// löschen TotalTics[16];		


  uint32_t RB01_AlltimeTotalTics;	// att -> actual alltime-total-tics value

  //float RB01_AlltimeTotalValue	// atv -> actual alltime-total-value (its a calculated reading!)


  float RB01_Average01Min;		// av01 -> calculated average last 01 minute

  float RB01_Average05Min;		// av05 -> calculated Average last 05 Minutes

  float RB01_Average15Min;		// av15 -> calculated Average last 15 Minutes

//  int32_t IB01_X_TiSt;			// Timestamp for Internals-Block-1 sync (last change)

 // int32_t RB01_X_TiSt;			// Timestamp for Readings-Block-1 sync (last change)
 







 //--- Internals - Block 1 ---

  uint8_t GPIO;				// conntect to GPIO-No
  uint8_t Block : 1;			// High-Speed 0 / Low-Speed 1
  uint8_t Reserved1 : 7;		// reserved
  uint8_t Channel;			// 0-7 available
  uint8_t Timer;			// 0-3 available


//uint32_t Capabilities;		// =CAPS, capabilities of Devices S0 Feature - HARDCODED!
  char IB01_Name[31];			// name -> custom name of the SwITCH = MAX 31+1 byte

  uint32_t IB01_SwValMin;		// XXXX -> SwITCH allowed min value (for slider)
  uint32_t IB01_SwValMax;		// XXXX -> SwITCH allowed max value (for slider)



  uint32_t RB01_Val;			// val -> switch value = 1 byte

  int32_t IB01_X_TiSt;			// tist -> for Internals-Block-01 sync
  int32_t RB01_X_TiSt;			// tist -> for Readings-Block-01 sync

  } ESP32_SwITCH_Definition_t;



//IBXX_ -> Internals timestamp sharing block NO XX, for settings
//ABXX_ -> Attributes timestamp sharing block NO XX, for user settings
//RBXX_ -> Readings timestamp sharing block NO XX, for features current readings



/*                Stored in Ram (No Mirror) -+ -> for fast access, reset clears struct
 *           Stored in Ram (RTC-Mirrored) -+ | -> for keeping values alive after reset/crash
 *       Stored in Ram (Flash Mirrored) -+ | | -> for fast access ?
 *               Stored only in Flash -+ | | | -> for memory efficient, but slow programming
 *                                     | | | |
 *      LiGHT_Feature - for Mode 2     | | | |
 *                                     | | | |
 *    --- Internals - Block 01 ---     | | | |
 * IB01_Name char[31]                  | X | |  name -> custom name of the SwITCH = MAX 31+1 byte
 * uint32_t IB01_SwValMin;             | X | |  XXXX -> SwITCH allowed min value (for slider)
 * uint32_t IB01_SwValMax;             | X | |  XXXX -> SwITCH allowed max value (for slider)
 * int32_t IB01_X_TiSt                 | | | X  tist -> for Internals-Block-01 sync
 *                                     | | | |
 *   --- Attributes - Block 01 ---     | | | |  
 *                                     | | | |
 *    --- Readings - Block 01 ---      | | | |
 * uint32_t RB01_Val                   | | X |  val -> SwITCH value = 1 byte
 * int32_t RB01_X_TiSt                 | | | X  tist -> for Readings-Block-01 sync
 *
 */



/*
 *  Functions provided to SCDE by module for type operation
 */
strTextMultiple_t* ESP32_SwITCH_Attribute(Common_Definition_t* Common_Definition, const uint8_t *attrCmdText, const size_t attrCmdTextLen, const uint8_t *attrNameText, const size_t attrNameTextLen, uint8_t **attrValTextPtr, size_t *attrValTextLenPtr);
strTextMultiple_t* ESP32_SwITCH_Define(Common_Definition_t *Common_Definition);
int                ESP32_SwITCH_IdleCb(Common_Definition_t *Common_Definition);
int                ESP32_SwITCH_Initialize(SCDERoot_t* SCDERoot);
strTextMultiple_t* ESP32_SwITCH_Set(Common_Definition_t* Common_Definition, uint8_t *setArgs, size_t setArgsLen);
strTextMultiple_t* ESP32_SwITCH_Shutdown(Common_Definition_t* Common_Definition);
strTextMultiple_t* ESP32_SwITCH_Undefine(Common_Definition_t* Common_Definition);




/*
 *  helpers provided to module for type operation
 */
bool ESP32_SwITCH_ProcessKVInputArgs(ESP32_SwITCH_Definition_t* ESP32_SwITCH_Definition, parsedKVInputArgs_t *parsedKVInput, uint8_t *argsText, size_t argsTextLen);
bool ESP32_SwITCH_SetAffectedReadings(ESP32_SwITCH_Definition_t* ESP32_SwITCH_Definition, uint64_t affectedReadings);



#endif /*ESP32_SwITCH_MODULE_H*/
