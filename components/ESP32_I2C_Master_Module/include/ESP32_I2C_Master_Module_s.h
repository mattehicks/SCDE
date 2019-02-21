// SCDE-Module ESP32_I2C_MASTER (IO-Device for I2C Master communication for I2C-hardware)

#ifndef ESP32_I2C_MASTER_MODULE_S_H
#define ESP32_I2C_MASTER_MODULE_S_H



#include "SCDE_s.h"
#include "WebIf_Module global types.h"

#include "freertos/ringbuf.h"


// -------------------------------------------------------------------------------------------------



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



// -------------------------------------------------------------------------------------------------



/*
 * included from: esp-idf/blob/master/components/driver/include/driver/i2c.h
 */

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

typedef enum {
    I2C_MASTER_ACK = 0x0,        /*!< I2C ack for each byte read */
    I2C_MASTER_NACK = 0x1,       /*!< I2C nack for each byte read */
    I2C_MASTER_LAST_NACK = 0x2,  /*!< I2C nack for the last byte*/
    I2C_MASTER_ACK_MAX,
} i2c_ack_type_t;



// -------------------------------------------------------------------------------------------------



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



typedef void* i2c_cmd_handle_t;    /*!< I2C command handle  */







// -------------------------------------------------------------------------------------------------



// i2c commands linked list - detailed data for one command
typedef struct {
    uint8_t byte_num;  /*!< cmd byte number */
    uint8_t ack_en;    /*!< ack check enable */
    uint8_t ack_exp;   /*!< expected ack level to get */
    uint8_t ack_val;   /*!< ack value to send */
    uint8_t *data;     /*!< data address */
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
  I2C_STATUS_TIMEOUT,   /*!< I2C bus status error, and operation timeout */
} i2c_status_t;

// ?
typedef struct {
  int type;
} i2c_cmd_evt_t;

// i2c processing object that is kept in RAM
typedef struct {
  int i2c_num;                     /*!< I2C port number */
  int mode;                        /*!< I2C mode, master or slave */
  intr_handle_t intr_handle;       /*!< I2C interrupt handle*/
  int cmd_idx;                     /*!< record current command index, for master mode */
  int status;                      /*!< record current command status, for master mode */
  int rx_cnt;                      /*!< record current read index, for master mode */
  uint8_t data_buf[I2C_FIFO_LEN];  /*!< a buffer to store i2c fifo data */
  i2c_cmd_desc_t cmd_link;         /*!< I2C command link */
  QueueHandle_t cmd_evt_queue;     /*!< I2C command event queue */

#if CONFIG_SPIRAM_USE_MALLOC
  uint8_t *evt_queue_storage;      /*!< The buffer that will hold the items in the queue */
  int intr_alloc_flags;            /*!< Used to allocate the interrupt */
  StaticQueue_t evt_queue_buffer;  /*!< The buffer that will hold the queue structure*/
#endif

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
 * typedefs of ESP32_I2C_Master Function Callbacks
 * This Fn are provided & made accessible for client modules - for operation
 */
// typedef for i2c_cmd_link_createFn - 
typedef i2c_cmd_handle_t (*i2c_cmd_link_createFn_t) ();

// typedef for i2c_cmd_link_deleteFn - 
typedef void (*i2c_cmd_link_deleteFn_t) (i2c_cmd_handle_t cmd_handle);

// typedef for i2c_cmd_link_appendFn - 
typedef esp_err_t (*i2c_cmd_link_appendFn_t) (i2c_cmd_handle_t cmd_handle, i2c_cmd_t *cmd);

// typedef for i2c_master_startFn - 
typedef esp_err_t (*i2c_master_startFn_t) (i2c_cmd_handle_t cmd_handle);

// typedef for i2c_master_stopFn - 
typedef esp_err_t (*i2c_master_stopFn_t) (i2c_cmd_handle_t cmd_handle);

// typedef for i2c_master_writeFn - 
typedef esp_err_t (*i2c_master_writeFn_t) (i2c_cmd_handle_t cmd_handle, uint8_t *data, size_t data_len, bool ack_en);

// typedef for i2c_master_write_byteFn - 
typedef esp_err_t (*i2c_master_write_byteFn_t) (i2c_cmd_handle_t cmd_handle, uint8_t data, bool ack_en);

// typedef for i2c_master_read_byteFn - 
typedef esp_err_t (*i2c_master_read_byteFn_t) (i2c_cmd_handle_t cmd_handle, uint8_t *data, i2c_ack_type_t ack);

// typedef for i2c_master_readFn - 
typedef esp_err_t (*i2c_master_readFn_t) (i2c_cmd_handle_t cmd_handle, uint8_t *data, size_t data_len, i2c_ack_type_t ack);

// typedef for i2c_master_cmd_beginFn - 
typedef esp_err_t (*i2c_master_cmd_beginFn_t) (i2c_obj_t *p_i2c, i2c_cmd_handle_t cmd_handle, TickType_t ticks_to_wait);

/*
 * ESP32_I2C_Master Fn (I2C Functions) typedef
 * Stores function callbacks provided & made accessible for client modules using this module
 */
typedef struct ESP32_I2C_Master_Fn_s {
  i2c_cmd_link_createFn_t i2c_cmd_link_createFn;           // ?
  i2c_cmd_link_deleteFn_t i2c_cmd_link_deleteFn;           // ?
  i2c_cmd_link_appendFn_t i2c_cmd_link_appendFn;           // ?
  i2c_master_startFn_t i2c_master_startFn;                 // ?
  i2c_master_stopFn_t i2c_master_stopFn;                   // ?
  i2c_master_writeFn_t i2c_master_writeFn;                 // ?
  i2c_master_write_byteFn_t i2c_master_write_byteFn;       // ?
  i2c_master_read_byteFn_t i2c_master_read_byteFn;         // ?
  i2c_master_readFn_t i2c_master_readFn;                   // ?
  i2c_master_cmd_beginFn_t i2c_master_cmd_beginFn;         // ?
} ESP32_I2C_Master_Fn_t;



// -------------------------------------------------------------------------------------------------



/** 
 * ESP32_I2C_Master_Definition stores values for operation valid only for the defined instance of an
 * loaded module. Values are initialized by HCTRLD an the loaded module itself.
 */
typedef struct ESP32_I2C_Master_Definition_s {
  Common_Definition_t common;			/*!< ... the common part of the definition */
  WebIf_Provided_t WebIf_Provided;		/*!< provided data for WebIf */
  uint8_t i2c_num;				/*!< the I2C hardware that should be used */
  i2c_config_t i2c_config;			/*!< i2c configuration */
  i2c_obj_t *i2c_obj;				/*!< the current i2c job */
  ESP32_I2C_Master_Fn_t* ESP32_I2C_Master_Fn;	/*!< ESP32_I2C_MasterFn (Functions / callbacks) accessible for client modules */
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



#endif /*ESP32_I2C_MASTER_MODULE_S_H*/
