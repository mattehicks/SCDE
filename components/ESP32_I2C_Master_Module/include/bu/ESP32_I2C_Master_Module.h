// SCDE-Module ESP32_I2C_MASTER (IO-Device for I2C Master communication for I2C-hardware)

#ifndef ESP32_I2C_MASTER_MODULE_H
#define ESP32_I2C_MASTER_MODULE_H



#include "SCDE_s.h"
#include "WebIf_Module global types.h"

//#include "driver/i2c.h"











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
    i2c_mode_t mode;       /*!< I2C mode */
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















// entscheiden welche intern sind !!!!!!!!!!



typedef struct {
    uint8_t byte_num;  /*!< cmd byte number */
    uint8_t ack_en;    /*!< ack check enable */
    uint8_t ack_exp;   /*!< expected ack level to get */
    uint8_t ack_val;   /*!< ack value to send */
    uint8_t* data;     /*!< data address */
    uint8_t byte_cmd;  /*!< to save cmd for one byte command mode */
    i2c_opmode_t op_code; /*!< haredware cmd type */
}i2c_cmd_t;

typedef struct i2c_cmd_link{
    i2c_cmd_t cmd;              /*!< command in current cmd link */
    struct i2c_cmd_link *next;  /*!< next cmd link */
} i2c_cmd_link_t;

typedef struct {
    i2c_cmd_link_t* head;     /*!< head of the command link */
    i2c_cmd_link_t* cur;      /*!< last node of the command link */
    i2c_cmd_link_t* free;     /*!< the first node to free of the command link */
} i2c_cmd_desc_t;

typedef enum {
    I2C_STATUS_READ,      /*!< read status for current master command */
    I2C_STATUS_WRITE,     /*!< write status for current master command */
    I2C_STATUS_IDLE,      /*!< idle status for current master command */
    I2C_STATUS_ACK_ERROR, /*!< ack error status for current master command */
    I2C_STATUS_DONE,      /*!< I2C command done */
} i2c_status_t;



#include "freertos/ringbuf.h"


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







/* 
 * ESP32_I2C_Master_Definition stores values for operation valid only for the defined instance of an
 * loaded module. Values are initialized by HCTRLD an the loaded module itself.
 */
typedef struct ESP32_I2C_Master_Definition_s {

  Common_Definition_t common;		// ... the common part of the definition

  WebIf_Provided_t WebIf_Provided;	// provided data for WebIf

  uint8_t i2c_master_port;		// the I2C hardware port that should be used

  i2c_config_t i2c_config;		// i2c configuration structure

  i2c_obj_t i2c_obj;			// the i2c object structure

} ESP32_I2C_Master_Definition_t;



/*
 *  Functions provided to SCDE by module for type operation
 */
strTextMultiple_t* ESP32_I2C_Master_Attribute(Common_Definition_t* Common_Definition, const uint8_t *attrCmdText, const size_t attrCmdTextLen, const uint8_t *attrNameText, const size_t attrNameTextLen, uint8_t **attrValTextPtr, size_t *attrValTextLenPtr);
strTextMultiple_t* ESP32_I2C_Master_Define(Common_Definition_t *Common_Definition);
int                ESP32_I2C_Master_IdleCb(Common_Definition_t *Common_Definition);
int                ESP32_I2C_Master_Initialize(SCDERoot_t* SCDERoot);
strTextMultiple_t* ESP32_I2C_Master_Set(Common_Definition_t* Common_Definition, uint8_t *setArgs, size_t setArgsLen);
strTextMultiple_t* ESP32_I2C_Master_Shutdown(Common_Definition_t* Common_Definition);
strTextMultiple_t* ESP32_I2C_Master_Undefine(Common_Definition_t* Common_Definition);



/*
 *  helpers provided to module for type operation
 */
bool ESP32_I2C_Master_ProcessKVInputArgs(ESP32_I2C_Master_Definition_t* ESP32_I2C_Master_Definition, parsedKVInputArgs_t *parsedKVInput, uint8_t *argsText, size_t argsTextLen);
bool ESP32_I2C_Master_SetAffectedReadings(ESP32_I2C_Master_Definition_t* ESP32_I2C_Master_Definition, uint64_t affectedReadings);












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
esp_err_t i2c_param_config(i2c_port_t i2c_num, const i2c_config_t* i2c_conf);



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
esp_err_t i2c_set_pin(i2c_port_t i2c_num, gpio_num_t sda_io_num, gpio_num_t scl_io_num,
                      gpio_pullup_t sda_pullup_en, gpio_pullup_t scl_pullup_en, i2c_mode_t mode);


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
