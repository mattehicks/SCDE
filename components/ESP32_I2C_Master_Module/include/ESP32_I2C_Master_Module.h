// SCDE-Module ESP32_I2C_MASTER (IO-Device for I2C Master communication for I2C-hardware)

#ifndef ESP32_I2C_MASTER_MODULE_H
#define ESP32_I2C_MASTER_MODULE_H



#include "SCDE_s.h"
#include "WebIf_Module global types.h"

//#include "driver/i2c.h"



/* 
 * ESP32 I2C Controller Hardware Configuration Registers (R / W)
 * ESP32 has 2 I2C Controller units. Base adress is (I2C0) 0x3FF53000 (I2C1) 0x3FF67000
 *
 * Sample to make configuration registers accessible:
 * ESP32_I2C_CONTROLLER_CONF_REG_t *pESP32_I2C_CONTROLLER_CONF_REG =
 *	(ESP32_I2C_CONTROLLER_CONF_REG_t*) 0x3FF57000;
 * Then writeable by: pESP32_I2C_CONTROLLER_CONF_REG[x]->
 */
typedef volatile struct ESP32_I2C_CONTROLLER_CONF_REG_t_s {

  union {

    struct {

      uint32_t scl_low_period:14;             /*This register is used to configure the  low level width of SCL clock.*/
      uint32_t reserved14:    18;
 
    };

    uint32_t val;

  } scl_low_period;

  union {

    struct {

      uint32_t sda_force_out:    1;           /*1：normally output sda data   0: exchange the function of sda_o and sda_oe (sda_o is the original internal output sda signal sda_oe is the enable bit for the internal output sda signal)*/
      uint32_t scl_force_out:    1;           /*1：normally output scl clock  0: exchange the function of scl_o and scl_oe (scl_o is the original internal output scl signal  scl_oe is the enable bit for the internal output scl signal)*/
      uint32_t sample_scl_level: 1;           /*Set this bit to sample data in SCL low level. clear this bit to sample data in SCL high level.*/
      uint32_t reserved3:        1;
      uint32_t ms_mode:          1;           /*Set this bit to configure the module as i2c master  clear this bit to configure the module as i2c slave.*/
      uint32_t trans_start:      1;           /*Set this bit to start sending data in tx_fifo.*/
      uint32_t tx_lsb_first:     1;           /*This bit is used to control the sending mode for  data need to be send. 1：receive data from most significant bit    0：receive data from least significant bit*/
      uint32_t rx_lsb_first:     1;           /*This bit is used to control the storage mode for received data. 1：receive data from most significant bit    0：receive data from least significant bit*/
      uint32_t clk_en:           1;           /*This is the clock gating control bit for reading or writing registers.*/
      uint32_t reserved9:       23;

    };

    uint32_t val;

  } ctr;

  union {

    struct {

      uint32_t ack_rec:             1;        /*This register stores the value of ACK bit.*/
      uint32_t slave_rw:            1;        /*when in slave mode  1：master read slave  0: master write slave.*/
      uint32_t time_out:            1;        /*when I2C takes more than time_out_reg clocks to receive a data then this register changes to high level.*/
      uint32_t arb_lost:            1;        /*when I2C lost control of SDA line  this register changes to high level.*/
      uint32_t bus_busy:            1;        /*1:I2C bus is busy transferring data. 0:I2C bus is in idle state.*/
      uint32_t slave_addressed:     1;        /*when configured as i2c slave  and the address send by master is equal to slave's address  then this bit will be high level.*/
      uint32_t byte_trans:          1;        /*This register changes to high level when one byte is transferred.*/
      uint32_t reserved7:           1;
      uint32_t rx_fifo_cnt:         6;        /*This register represent the amount of data need to send.*/
      uint32_t reserved14:          4;
      uint32_t tx_fifo_cnt:         6;        /*This register stores the amount of received data  in ram.*/
      uint32_t scl_main_state_last: 3;        /*This register stores the value of state machine for i2c module.  3'h0: SCL_MAIN_IDLE  3'h1: SCL_ADDRESS_SHIFT 3'h2: SCL_ACK_ADDRESS  3'h3: SCL_RX_DATA  3'h4 SCL_TX_DATA  3'h5:SCL_SEND_ACK 3'h6:SCL_WAIT_ACK*/
      uint32_t reserved27:          1;
      uint32_t scl_state_last:      3;        /*This register stores the value of state machine to produce SCL. 3'h0: SCL_IDLE  3'h1:SCL_START   3'h2:SCL_LOW_EDGE  3'h3: SCL_LOW   3'h4:SCL_HIGH_EDGE   3'h5:SCL_HIGH  3'h6:SCL_STOP*/
      uint32_t reserved31:          1;

    };

    uint32_t val;

  } status_reg;

  union {

    struct {

      uint32_t tout:      20;                 /*This register is used to configure the max clock number of receiving  a data.*/
      uint32_t reserved20:12;

    };

    uint32_t val;

  } timeout;

  union {

    struct {

      uint32_t addr:       15;                /*when configured as i2c slave  this register is used to configure slave's address.*/
      uint32_t reserved15: 16;
      uint32_t en_10bit:    1;                /*This register is used to enable slave 10bit address mode.*/

   };

   uint32_t val;

  } slave_addr;

  union {

    struct {

      uint32_t rx_fifo_start_addr: 5;         /*This is the offset address of the last receiving data as described in nonfifo_rx_thres_register.*/
      uint32_t rx_fifo_end_addr:   5;         /*This is the offset address of the first receiving data as described in nonfifo_rx_thres_register.*/
      uint32_t tx_fifo_start_addr: 5;         /*This is the offset address of the first  sending data as described in nonfifo_tx_thres register.*/
      uint32_t tx_fifo_end_addr:   5;         /*This is the offset address of the last  sending data as described in nonfifo_tx_thres register.*/
      uint32_t reserved20:        12;

    };

    uint32_t val;

  } fifo_st;

  union {

    struct {

      uint32_t rx_fifo_full_thrhd: 5;
      uint32_t tx_fifo_empty_thrhd:5;         /*Config tx_fifo empty threhd value when using apb fifo access*/
      uint32_t nonfifo_en:         1;         /*Set this bit to enble apb nonfifo access.*/
      uint32_t fifo_addr_cfg_en:   1;         /*When this bit is set to 1 then the byte after address represent the offset address of I2C Slave's ram.*/
      uint32_t rx_fifo_rst:        1;         /*Set this bit to reset rx fifo when using apb fifo access.*/
      uint32_t tx_fifo_rst:        1;         /*Set this bit to reset tx fifo when using apb fifo access.*/
      uint32_t nonfifo_rx_thres:   6;         /*when I2C receives more than nonfifo_rx_thres data  it will produce rx_send_full_int_raw interrupt and update the current offset address of the receiving data.*/
      uint32_t nonfifo_tx_thres:   6;         /*when I2C sends more than nonfifo_tx_thres data  it will produce tx_send_empty_int_raw interrupt and update the current offset address of the sending data.*/
      uint32_t reserved26:         6;

    };

    uint32_t val;

  } fifo_conf;

  union {

    struct {

      uint8_t data;                           /*The register represent the byte  data read from rx_fifo when use apb fifo access*/
      uint8_t reserved[3];

    };

    uint32_t val;

  } fifo_data;

  union {

    struct {

      uint32_t rx_fifo_full:     1;           /*The raw interrupt status bit for rx_fifo full when use apb fifo access.*/
      uint32_t tx_fifo_empty:    1;           /*The raw interrupt status bit for tx_fifo empty when use apb fifo access.*/
      uint32_t rx_fifo_ovf:      1;           /*The raw interrupt status bit for receiving data overflow when use apb fifo access.*/
      uint32_t end_detect:       1;           /*The raw interrupt status bit for end_detect_int interrupt. when I2C deals with  the END command  it will produce end_detect_int interrupt.*/
      uint32_t slave_tran_comp:  1;           /*The raw interrupt status bit for slave_tran_comp_int interrupt. when I2C Slave detects the STOP bit  it will produce slave_tran_comp_int interrupt.*/
      uint32_t arbitration_lost: 1;           /*The raw interrupt status bit for arbitration_lost_int interrupt.when I2C lost the usage right of I2C BUS it will produce arbitration_lost_int interrupt.*/
      uint32_t master_tran_comp: 1;           /*The raw interrupt status bit for master_tra_comp_int interrupt. when I2C Master sends or receives a byte it will produce master_tran_comp_int interrupt.*/
      uint32_t trans_complete:   1;           /*The raw interrupt status bit for trans_complete_int interrupt. when I2C Master finished STOP command  it will produce trans_complete_int interrupt.*/
      uint32_t time_out:         1;           /*The raw interrupt status bit for time_out_int interrupt. when I2C takes a lot of time to receive a data  it will produce  time_out_int interrupt.*/
      uint32_t trans_start:      1;           /*The raw interrupt status bit for trans_start_int interrupt. when I2C sends the START bit it will produce trans_start_int interrupt.*/
      uint32_t ack_err:          1;           /*The raw interrupt status bit for ack_err_int interrupt. when I2C receives a wrong ACK bit  it will produce ack_err_int interrupt..*/
      uint32_t rx_rec_full:      1;           /*The raw interrupt status bit for rx_rec_full_int interrupt. when I2C receives more data  than nonfifo_rx_thres  it will produce rx_rec_full_int interrupt.*/
      uint32_t tx_send_empty:    1;           /*The raw interrupt status bit for tx_send_empty_int interrupt.when I2C sends more data than nonfifo_tx_thres  it will produce tx_send_empty_int interrupt..*/
      uint32_t reserved13:      19;

    };

    uint32_t val;

  } int_raw;

  union {

    struct {

      uint32_t rx_fifo_full:     1;           /*Set this bit to clear the rx_fifo_full_int interrupt.*/
      uint32_t tx_fifo_empty:    1;           /*Set this bit to clear the tx_fifo_empty_int interrupt.*/
      uint32_t rx_fifo_ovf:      1;           /*Set this bit to clear the rx_fifo_ovf_int interrupt.*/
      uint32_t end_detect:       1;           /*Set this bit to clear the end_detect_int interrupt.*/
      uint32_t slave_tran_comp:  1;           /*Set this bit to clear the slave_tran_comp_int interrupt.*/
      uint32_t arbitration_lost: 1;           /*Set this bit to clear the arbitration_lost_int interrupt.*/
      uint32_t master_tran_comp: 1;           /*Set this bit to clear the master_tran_comp interrupt.*/
      uint32_t trans_complete:   1;           /*Set this bit to clear the trans_complete_int interrupt.*/
      uint32_t time_out:         1;           /*Set this bit to clear the time_out_int interrupt.*/
      uint32_t trans_start:      1;           /*Set this bit to clear the trans_start_int interrupt.*/
      uint32_t ack_err:          1;           /*Set this bit to clear the ack_err_int interrupt.*/
      uint32_t rx_rec_full:      1;           /*Set this bit to clear the rx_rec_full_int interrupt.*/
      uint32_t tx_send_empty:    1;           /*Set this bit to clear the tx_send_empty_int interrupt.*/
      uint32_t reserved13:      19;

    };

    uint32_t val;

  } int_clr;

  union {

    struct {

      uint32_t rx_fifo_full:     1;           /*The enable bit for rx_fifo_full_int interrupt.*/
      uint32_t tx_fifo_empty:    1;           /*The enable bit for tx_fifo_empty_int interrupt.*/
      uint32_t rx_fifo_ovf:      1;           /*The enable bit for rx_fifo_ovf_int interrupt.*/
      uint32_t end_detect:       1;           /*The enable bit for end_detect_int interrupt.*/
      uint32_t slave_tran_comp:  1;           /*The enable bit for slave_tran_comp_int interrupt.*/
      uint32_t arbitration_lost: 1;           /*The enable bit for arbitration_lost_int interrupt.*/
      uint32_t master_tran_comp: 1;           /*The enable bit for master_tran_comp_int interrupt.*/
      uint32_t trans_complete:   1;           /*The enable bit for trans_complete_int interrupt.*/
      uint32_t time_out:         1;           /*The enable bit for time_out_int interrupt.*/
      uint32_t trans_start:      1;           /*The enable bit for trans_start_int interrupt.*/
      uint32_t ack_err:          1;           /*The enable bit for ack_err_int interrupt.*/
      uint32_t rx_rec_full:      1;           /*The enable bit for rx_rec_full_int interrupt.*/
      uint32_t tx_send_empty:    1;           /*The enable bit for tx_send_empty_int interrupt.*/
      uint32_t reserved13:      19;
    };

    uint32_t val;

  } int_ena;

  union {

    struct {

      uint32_t rx_fifo_full:     1;            /*The masked interrupt status for rx_fifo_full_int interrupt.*/
      uint32_t tx_fifo_empty:    1;            /*The masked interrupt status for tx_fifo_empty_int interrupt.*/
      uint32_t rx_fifo_ovf:      1;            /*The masked interrupt status for rx_fifo_ovf_int interrupt.*/
      uint32_t end_detect:       1;            /*The masked interrupt status for end_detect_int interrupt.*/
      uint32_t slave_tran_comp:  1;            /*The masked interrupt status for slave_tran_comp_int interrupt.*/
      uint32_t arbitration_lost: 1;            /*The masked interrupt status for arbitration_lost_int interrupt.*/
      uint32_t master_tran_comp: 1;            /*The masked interrupt status for master_tran_comp_int interrupt.*/
      uint32_t trans_complete:   1;            /*The masked interrupt status for trans_complete_int interrupt.*/
      uint32_t time_out:         1;            /*The masked interrupt status for time_out_int interrupt.*/
      uint32_t trans_start:      1;            /*The masked interrupt status for trans_start_int interrupt.*/
      uint32_t ack_err:          1;            /*The masked interrupt status for ack_err_int interrupt.*/
      uint32_t rx_rec_full:      1;            /*The masked interrupt status for rx_rec_full_int interrupt.*/
      uint32_t tx_send_empty:    1;            /*The masked interrupt status for tx_send_empty_int interrupt.*/
      uint32_t reserved13:      19;

    };

  uint32_t val;

  } int_status;

  union {

    struct {

      uint32_t time:        10;                /*This register is used to configure the clock num I2C used to hold the data after the negedge of SCL.*/
      uint32_t reserved10:  22;

    };

    uint32_t val;

  } sda_hold;

  union {

    struct {

      uint32_t time:       10;                 /*This register is used to configure the clock num I2C used to sample data on SDA after the posedge of SCL*/
      uint32_t reserved10: 22;

    };

    uint32_t val;

  } sda_sample;

  union {

    struct {

      uint32_t period:     14;                 /*This register is used to configure the clock num during SCL is low level.*/
      uint32_t reserved14: 18;

    };

    uint32_t val;

  } scl_high_period;

  uint32_t reserved_3c;

  union {

    struct {

      uint32_t time:       10;                /*This register is used to configure the clock num between the negedge of SDA and negedge of SCL for start mark.*/
      uint32_t reserved10: 22;
 
    };

    uint32_t val;

  } scl_start_hold;

  union {

    struct {

      uint32_t time:       10;                /*This register is used to configure the clock num between the posedge of SCL and the negedge of SDA for restart mark.*/
      uint32_t reserved10: 22;

    };

    uint32_t val;

  } scl_rstart_setup;

  union {

    struct {

      uint32_t time:       14;                /*This register is used to configure the clock num after the STOP bit's posedge.*/
      uint32_t reserved14: 18;
  
    };

    uint32_t val;

  } scl_stop_hold;

  union {

    struct {

      uint32_t time:       10;                /*This register is used to configure the clock num between the posedge of SCL and the posedge of SDA.*/
      uint32_t reserved10: 22;

    };

  uint32_t val;

  } scl_stop_setup;

  union {

    struct {

      uint32_t thres:      3;                 /*When input SCL's pulse width is smaller than this register value  I2C ignores this pulse.*/
      uint32_t en:         1;                 /*This is the filter enable bit for SCL.*/
      uint32_t reserved4: 28;

    };

  uint32_t val;

  } scl_filter_cfg;

  union {

    struct {

    uint32_t thres:      3;                 /*When input SCL's pulse width is smaller than this register value  I2C ignores this pulse.*/
    uint32_t en:         1;                 /*This is the filter enable bit for SDA.*/
    uint32_t reserved4: 28;

    };

  uint32_t val;

  } sda_filter_cfg;

  union {

    struct {

      uint32_t byte_num:      8;              /*Byte_num represent the number of data need to be send or data need to be received.*/
      uint32_t ack_en:        1;              /*ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
      uint32_t ack_exp:       1;              /*ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
      uint32_t ack_val:       1;              /*ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
      uint32_t op_code:       3;              /*op_code is the command  0：RSTART   1：WRITE  2：READ  3：STOP . 4:END.*/
      uint32_t reserved14:   17;
      uint32_t done:  1;                      /*When command0 is done in I2C Master mode  this bit changes to high level.*/

    };

  uint32_t val;

  } command[16];

  uint32_t reserved_98;
  uint32_t reserved_9c;
  uint32_t reserved_a0;
  uint32_t reserved_a4;
  uint32_t reserved_a8;
  uint32_t reserved_ac;
  uint32_t reserved_b0;
  uint32_t reserved_b4;
  uint32_t reserved_b8;
  uint32_t reserved_bc;
  uint32_t reserved_c0;
  uint32_t reserved_c4;
  uint32_t reserved_c8;
  uint32_t reserved_cc;
  uint32_t reserved_d0;
  uint32_t reserved_d4;
  uint32_t reserved_d8;
  uint32_t reserved_dc;
  uint32_t reserved_e0;
  uint32_t reserved_e4;
  uint32_t reserved_e8;
  uint32_t reserved_ec;
  uint32_t reserved_f0;
  uint32_t reserved_f4;
  uint32_t date;                                  /**/
  uint32_t reserved_fc;
  uint32_t fifo_start_addr;                       /*This the start address for ram when use apb nonfifo access.*/

} ESP32_I2C_CONTROLLER_CONF_REG_t;







#define I2C_APB_CLK_FREQ  APB_CLK_FREQ /*!< I2C source clock is APB clock, 80MHz */
#define I2C_FIFO_LEN   (32)  /*!< I2C hardware fifo length */

typedef enum {
    I2C_MODE_SLAVE = 0,   /*!< I2C slave mode */
    I2C_MODE_MASTER,      /*!< I2C master mode */
    I2C_MODE_MAX,
} i2c_mode_t;

typedef enum {
    I2C_MASTER_WRITE = 0,   /*!< I2C write data */
    I2C_MASTER_READ,        /*!< I2C read data */
} i2c_rw_t;

typedef enum {
    I2C_DATA_MODE_MSB_FIRST = 0,  /*!< I2C data msb first */
    I2C_DATA_MODE_LSB_FIRST = 1,  /*!< I2C data lsb first */
    I2C_DATA_MODE_MAX
} i2c_trans_mode_t;

typedef enum {
    I2C_CMD_RESTART = 0,   /*!<I2C restart command */
    I2C_CMD_WRITE,         /*!<I2C write command */
    I2C_CMD_READ,          /*!<I2C read command */
    I2C_CMD_STOP,          /*!<I2C stop command */
    I2C_CMD_END            /*!<I2C end command */
} i2c_opmode_t;

typedef enum {
    I2C_NUM_0 = 0,  /*!< I2C port 0 */
    I2C_NUM_1 ,     /*!< I2C port 1 */
    I2C_NUM_MAX
} i2c_port_t;

typedef enum {
    I2C_ADDR_BIT_7 = 0,    /*!< I2C 7bit address for slave mode */
    I2C_ADDR_BIT_10,       /*!< I2C 10bit address for slave mode */
    I2C_ADDR_BIT_MAX,
} i2c_addr_mode_t;



/**
 * @brief I2C initialization parameters
 */
typedef struct {
    i2c_mode_t mode;      	  /*!< I2C mode */
    gpio_num_t sda_io_num;        /*!< GPIO number for I2C sda signal */
    gpio_pullup_t sda_pullup_en;  /*!< Internal GPIO pull mode for I2C sda signal*/
    gpio_num_t scl_io_num;        /*!< GPIO number for I2C scl signal */
    gpio_pullup_t scl_pullup_en;  /*!< Internal GPIO pull mode for I2C scl signal*/

    union {
        struct {
            uint32_t clk_speed;     /*!< I2C clock frequency for master mode, (no higher than 1MHz for now) */
        } master;
        struct {
            uint8_t addr_10bit_en;  /*!< I2C 10bit address mode enable for slave mode */
            uint16_t slave_addr;    /*!< I2C address for slave mode */
        } slave;

    };
} i2c_config_t;






#include "freertos/ringbuf.h"











typedef void* i2c_cmd_handle_t;    /*!< I2C command handle  */











//XXXXXXXXXXXXXXXXXXXXXXXXXXXXX NEEDED BY CLIENTS THAT PREPARE A LINKED LIST XXXXXXXXXXXXXXXXXXXXXXX

// i2c commands linked list - detailed data for one command
typedef struct {
    uint8_t byte_num;  /*!< cmd byte number */
    uint8_t ack_en;    /*!< ack check enable */
    uint8_t ack_exp;   /*!< expected ack level to get */
    uint8_t ack_val;   /*!< ack value to send */
    uint8_t* data;     /*!< data address */
    uint8_t byte_cmd;  /*!< to save cmd for one byte command mode */
    i2c_opmode_t op_code; /*!< haredware cmd type */
} i2c_cmd_t;



// i2c commands linked list - an element of the linked list
typedef struct i2c_cmd_link{
    i2c_cmd_t cmd;              /*!< command in current cmd link */
    struct i2c_cmd_link *next;  /*!< next cmd link */
} i2c_cmd_link_t;



// i2c commands linked list - head of linked list
typedef struct {
  i2c_cmd_link_t* head;     /*!< head of the command link */
  i2c_cmd_link_t* cur;      /*!< last node of the command link */
  i2c_cmd_link_t* free;     /*!< the first node to free of the command link */
} i2c_cmd_desc_t;


// status used in i2c commands in the linked list
typedef enum {
  I2C_STATUS_READ,      /*!< read status for current master command */
  I2C_STATUS_WRITE,     /*!< write status for current master command */
  I2C_STATUS_IDLE,      /*!< idle status for current master command */
  I2C_STATUS_ACK_ERROR, /*!< ack error status for current master command */
  I2C_STATUS_DONE,      /*!< I2C command done */
} i2c_status_t;



// i2c processing object
typedef struct {
  int i2c_num;                     /*!< I2C port number */
  int mode;                        /*!< I2C mode, master or slave */
  intr_handle_t intr_handle;       /*!< I2C interrupt handle*/

  int cmd_idx;                     /*!< record current command index, for master mode */
  int status;                      /*!< record current command status, for master mode */
  int rx_cnt;                      /*!< record current read index, for master mode */
  uint8_t data_buf[I2C_FIFO_LEN];  /*!< a buffer to store i2c fifo data */
  i2c_cmd_desc_t cmd_link;         /*!< I2C command link */
  xSemaphoreHandle cmd_sem;        /*!< semaphore to sync command status */
  xSemaphoreHandle cmd_mux;        /*!< semaphore to lock command process */
  size_t tx_fifo_remain;           /*!< tx fifo remain length, for master mode */
  size_t rx_fifo_remain;           /*!< rx fifo remain length, for master mode */
  xSemaphoreHandle slv_rx_mux;     /*!< slave rx buffer mux */
  xSemaphoreHandle slv_tx_mux;     /*!< slave tx buffer mux */
  size_t rx_buf_length;            /*!< rx buffer length */
  RingbufHandle_t rx_ring_buf;     /*!< rx ringbuffer handler of slave mode */
  size_t tx_buf_length;            /*!< tx buffer length */
  RingbufHandle_t tx_ring_buf;     /*!< tx ringbuffer handler of slave mode */
} i2c_obj_t;



// -------------------------------------------------------------------------------------------------



/* 
 * ESP32_I2C_Master_Definition stores values for operation valid only for the defined instance of an
 * loaded module. Values are initialized by HCTRLD an the loaded module itself.
 */
typedef struct ESP32_I2C_Master_Definition_s {
  Common_Definition_t common;		// ... the common part of the definition
  WebIf_Provided_t WebIf_Provided;	// provided data for WebIf
  uint8_t i2c_num;			// the I2C hardware that should be used
  i2c_config_t i2c_config;		// i2c configuration
  i2c_obj_t i2c_obj;			// the current i2c job
} ESP32_I2C_Master_Definition_t;



// -------------------------------------------------------------------------------------------------



/* 
 * ESP32_I2C_Master_StageXCHG stores values for operation of 2. stage design Modules (stage exchange)
 */
typedef struct ESP32_I2C_Master_StageXCHG_s {

  Common_StageXCHG_t common;		// ... the common part of the StageXCHG

  // module specific here 

} ESP32_I2C_Master_StageXCHG_t;



// -------------------------------------------------------------------------------------------------



/*
 *  Functions provided to SCDE by Module - for type operation (A-Z)
 */
strTextMultiple_t* ESP32_I2C_Master_Attribute(Common_Definition_t* Common_Definition, const uint8_t *attrCmdText, const size_t attrCmdTextLen, const uint8_t *attrNameText, const size_t attrNameTextLen, uint8_t **attrValTextPtr, size_t *attrValTextLenPtr);

strTextMultiple_t* ESP32_I2C_Master_Define(Common_Definition_t *Common_Definition);

//strTextMultiple_t* ESP32_I2C_Master_DirectWrite(Common_Definition_t *Common_Definition_Stage1, Common_Definition_t *Common_Definition_Stage2, Common_StageXCHG_t *Common_StageXCHG);

int ESP32_I2C_Master_IdleCb(Common_Definition_t *Common_Definition);

int ESP32_I2C_Master_Initialize(SCDERoot_t* SCDERoot);

strTextMultiple_t* ESP32_I2C_Master_Set(Common_Definition_t* Common_Definition, uint8_t *setArgs, size_t setArgsLen);

strTextMultiple_t* ESP32_I2C_Master_Shutdown(Common_Definition_t* Common_Definition);

strTextMultiple_t* ESP32_I2C_Master_Undefine(Common_Definition_t* Common_Definition);



// -------------------------------------------------------------------------------------------------



/*
 *  helpers provided to module for type operation
 */
strTextMultiple_t* ESP32_I2C_Master_ProcessKVInputArgs(ESP32_I2C_Master_Definition_t* ESP32_I2C_Master_Definition, parsedKVInputArgs_t *parsedKVInput, uint8_t *argsText, size_t argsTextLen);
bool ESP32_I2C_Master_SetAffectedReadings(ESP32_I2C_Master_Definition_t* ESP32_I2C_Master_Definition, uint64_t affectedReadings);



// -------------------------------------------------------------------------------------------------



/**
 * @brief I2C parameter initialization
 *
 * @param i2c_num I2C port number
 * @param i2c_conf pointer to I2C parameter settings
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
strTextMultiple_t* i2c_param_config(i2c_port_t i2c_num, const i2c_config_t* i2c_conf);

/**
 * @brief Configure GPIO signal for I2C sck and sda
 *
 * @param i2c_num I2C port number
 * @param sda_io_num GPIO number for I2C sda signal
 * @param scl_io_num GPIO number for I2C scl signal
 * @param sda_pullup_en Whether to enable the internal pullup for sda pin
 * @param scl_pullup_en Whether to enable the internal pullup for scl pin
 * @param mode I2C mode
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
strTextMultiple_t* i2c_set_pin(i2c_port_t i2c_num, gpio_num_t sda_io_num, gpio_num_t scl_io_num,
                      gpio_pullup_t sda_pullup_en, gpio_pullup_t scl_pullup_en, i2c_mode_t mode);


/**
 * @brief reset I2C tx hardware fifo
 *
 * @param i2c_num I2C port number
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_reset_tx_fifo(i2c_port_t i2c_num);



/**
 * @brief reset I2C rx fifo
 *
 * @param i2c_num I2C port number
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_reset_rx_fifo(i2c_port_t i2c_num);



/**
 * @brief Create and init I2C command link
 *        @note
 *        Before we build I2C command link, we need to call i2c_cmd_link_create() to create
 *        a command link.
 *        After we finish sending the commands, we need to call i2c_cmd_link_delete() to
 *        release and return the resources.
 *
 * @return i2c command link handler
 */
i2c_cmd_handle_t i2c_cmd_link_create();



/**
 * @brief Free I2C command link
 *        @note
 *        Before we build I2C command link, we need to call i2c_cmd_link_create() to create
 *        a command link.
 *        After we finish sending the commands, we need to call i2c_cmd_link_delete() to
 *        release and return the resources.
 *
 * @param cmd_handle I2C command handle
 */
void i2c_cmd_link_delete(i2c_cmd_handle_t cmd_handle);



/**
 * @brief Queue command for I2C master to generate a start signal
 *        @note
 *        Only call this function in I2C master mode
 *        Call i2c_master_cmd_begin() to send all queued commands
 *
 * @param cmd_handle I2C cmd link
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_master_start(i2c_cmd_handle_t cmd_handle);



/**
 * @brief Queue command for I2C master to write one byte to I2C bus
 *        @note
 *        Only call this function in I2C master mode
 *        Call i2c_master_cmd_begin() to send all queued commands
 *
 * @param cmd_handle I2C cmd link
 * @param data I2C one byte command to write to bus
 * @param ack_en enable ack check for master
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_master_write_byte(i2c_cmd_handle_t cmd_handle, uint8_t data, bool ack_en);



/**
 * @brief Queue command for I2C master to write buffer to I2C bus
 *        @note
 *        Only call this function in I2C master mode
 *        Call i2c_master_cmd_begin() to send all queued commands
 *
 * @param cmd_handle I2C cmd link
 * @param data data to send
 * @param data_len data length
 * @param ack_en enable ack check for master
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_master_write(i2c_cmd_handle_t cmd_handle, uint8_t* data, size_t data_len, bool ack_en);



/**
 * @brief Queue command for I2C master to read one byte from I2C bus
 *        @note
 *        Only call this function in I2C master mode
 *        Call i2c_master_cmd_begin() to send all queued commands
 *
 * @param cmd_handle I2C cmd link
 * @param data pointer accept the data byte
 * @param ack ack value for read command
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_master_read_byte(i2c_cmd_handle_t cmd_handle, uint8_t* data, int ack);



/**
 * @brief Queue command for I2C master to read data from I2C bus
 *        @note
 *        Only call this function in I2C master mode
 *        Call i2c_master_cmd_begin() to send all queued commands
 *
 * @param cmd_handle I2C cmd link
 * @param data data buffer to accept the data from bus
 * @param data_len read data length
 * @param ack ack value for read command
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_master_read(i2c_cmd_handle_t cmd_handle, uint8_t* data, size_t data_len, int ack);



/**
 * @brief Queue command for I2C master to generate a stop signal
 *        @note
 *        Only call this function in I2C master mode
 *        Call i2c_master_cmd_begin() to send all queued commands
 *
 * @param cmd_handle I2C cmd link
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_master_stop(i2c_cmd_handle_t cmd_handle);



/**
 * @brief I2C master send queued commands.
 *        This function will trigger sending all queued commands.
 *        The task will be blocked until all the commands have been sent out.
 *        The I2C APIs are not thread-safe, if you want to use one I2C port in different tasks,
 *        you need to take care of the multi-thread issue.
 *        @note
 *        Only call this function in I2C master mode
 *
 * @param i2c_num I2C port number
 * @param cmd_handle I2C command handler
 * @param ticks_to_wait maximum wait ticks.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
 *     - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
 *     - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
 */
esp_err_t i2c_master_cmd_begin(i2c_obj_t* p_i2c, i2c_cmd_handle_t cmd_handle, portBASE_TYPE ticks_to_wait);



/**
 * @brief I2C isr handler register
 *
 * @param i2c_num I2C port number
 * @param fn isr handler function
 * @param arg parameter for isr handler function
 * @param intr_alloc_flags Flags used to allocate the interrupt. One or multiple (ORred)
 *            ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info.
 * @param handle handle return from esp_intr_alloc.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_isr_register(i2c_port_t i2c_num, void (*fn)(void*), void * arg, int intr_alloc_flags, intr_handle_t *handle);



/**
 * @brief to delete and free I2C isr.
 *
 * @param handle handle of isr.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2c_isr_free(intr_handle_t handle);






#endif /*ESP32_I2C_MASTER_MODULE_H*/
