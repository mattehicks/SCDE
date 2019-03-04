// SCDE-Module ESP32_SPI_Module (IO-Device for SPI Hardware)

#ifndef ESP32_SPI_MODULE_S_H
#define ESP32_SPI_MODULE_S_H

// -------------------------------------------------------------------------------------------------

// this Module is made for the Smart-Connected-Device-Engine
#include "SCDE_s.h"

// this Module provides functions for other Modules:
#include "WebIf_Module global types.h"
//#include "WebIf_Module_s.h"

// this Module uses an 1st stage:
// -- no ---

// -------------------------------------------------------------------------------------------------

// whats needed?
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "soc/spi_struct.h"
#include "esp_intr.h"
#include "esp_intr_alloc.h"
#include "rom/lldesc.h"

#include "freertos/ringbuf.h"


// -------------------------------------------------------------------------------------------------



// Maximum amount of bytes that can be put in one DMA descriptor
#define SPI_MAX_DMA_LEN (4096-4)

/**
 * @brief Enum with the three SPI peripherals that are software-accessible in it
 */
typedef enum {
    TFT_SPI_HOST=0,                 ///< SPI1, SPI; Cannot be used in this driver!
    TFT_HSPI_HOST=1,                ///< SPI2, HSPI
    TFT_VSPI_HOST=2                 ///< SPI3, VSPI
} ESP32_SPI_Module_spi_host_device_t;



/**
 * @brief Configuration structure for a SPI bus.
 *
 * Structure to specify the GPIO pins of the bus. Normally, the driver will use the
 * GPIO matrix to route the signals. An exception is made when all signals either can be routed through 
 * the IO_MUX or are -1. In that case, the IO_MUX is used, allowing for > 40MHz speeds.
 */
typedef struct {
    int mosi_io_num;                ///< GPIO pin for Master Out Slave In (=spi_d) signal, or -1 if not used.
    int miso_io_num;                ///< GPIO pin for Master In Slave Out (=spi_q) signal, or -1 if not used.
    int sclk_io_num;                ///< GPIO pin for Spi CLocK signal, or -1 if not used.
    int quadwp_io_num;              ///< GPIO pin for WP (Write Protect) signal which is used as D2 in 4-bit communication modes, or -1 if not used.
    int quadhd_io_num;              ///< GPIO pin for HD (HolD) signal which is used as D3 in 4-bit communication modes, or -1 if not used.
    int max_transfer_sz;            ///< Maximum transfer size, in bytes. Defaults to 4094 if 0.
} ESP32_SPI_Module_spi_bus_config_t;



#define LB_SPI_DEVICE_TXBIT_LSBFIRST (1<<0)  ///< Transmit command/address/data LSB first instead of the default MSB first
#define LB_SPI_DEVICE_RXBIT_LSBFIRST (1<<1)  ///< Receive data LSB first instead of the default MSB first
#define LB_SPI_DEVICE_BIT_LSBFIRST   (SPI_TXBIT_LSBFIRST|SPI_RXBIT_LSBFIRST); ///< Transmit and receive LSB first
#define LB_SPI_DEVICE_3WIRE          (1<<2)  ///< Use spiq for both sending and receiving data
#define LB_SPI_DEVICE_POSITIVE_CS    (1<<3)  ///< Make CS positive during a transaction instead of negative
#define LB_SPI_DEVICE_HALFDUPLEX     (1<<4)  ///< Transmit data before receiving it, instead of simultaneously
#define LB_SPI_DEVICE_CLK_AS_CS      (1<<5)  ///< Output clock on CS line if CS is active

#define SPI_ERR_OTHER_CONFIG 7001

typedef struct ESP32_SPI_Module_spi_transaction_t ESP32_SPI_Module_spi_transaction_t;
typedef void (*ESP32_SPI_Module_spi_transaction_cb_t) (ESP32_SPI_Module_spi_transaction_t *trans);



/**
 * @brief Configuration for an SPI slave device connected to one of the SPI buses.
 */
typedef struct {
    uint8_t command_bits;               ///< Amount of bits in command phase (0-16)
    uint8_t address_bits;               ///< Amount of bits in address phase (0-64)
    uint8_t dummy_bits;                 ///< Amount of dummy bits to insert between address and data phase
    uint8_t mode;                       ///< SPI mode (0-3)
    uint8_t duty_cycle_pos;             ///< Duty cycle of positive clock, in 1/256th increments (128 = 50%/50% duty). Setting this to 0 (=not setting it) is equivalent to setting this to 128.
    uint8_t cs_ena_pretrans;            ///< Amount of SPI bit-cycles the cs should be activated before the transmission (0-16). This only works on half-duplex transactions.
    uint8_t cs_ena_posttrans;           ///< Amount of SPI bit-cycles the cs should stay active after the transmission (0-16)
    int clock_speed_hz;                 ///< Clock speed, in Hz
    int spics_io_num;                   ///< CS GPIO pin for this device, handled by hardware; set to -1 if not used
    int spics_ext_io_num;               ///< CS GPIO pin for this device, handled by software (spi_lobo_device_select/spi_lobo_device_deselect); only used if spics_io_num=-1
    uint32_t flags;                     ///< Bitwise OR of LB_SPI_DEVICE_* flags
    ESP32_SPI_Module_spi_transaction_cb_t pre_cb;   ///< Callback to be called before a transmission is started. This callback from 'spi_lobo_transfer_data' function.
    ESP32_SPI_Module_spi_transaction_cb_t post_cb;  ///< Callback to be called after a transmission has completed. This callback from 'spi_lobo_transfer_data' function.
    uint8_t selected;                   ///< **INTERNAL** 1 if the device's CS pin is active
} ESP32_SPI_Module_spi_device_interface_config_t;



#define LB_SPI_TRANS_MODE_DIO            (1<<0)  ///< Transmit/receive data in 2-bit mode
#define LB_SPI_TRANS_MODE_QIO            (1<<1)  ///< Transmit/receive data in 4-bit mode
#define LB_SPI_TRANS_MODE_DIOQIO_ADDR    (1<<2)  ///< Also transmit address in mode selected by SPI_MODE_DIO/SPI_MODE_QIO
#define LB_SPI_TRANS_USE_RXDATA          (1<<3)  ///< Receive into rx_data member of spi_lobo_transaction_t instead into memory at rx_buffer.
#define LB_SPI_TRANS_USE_TXDATA          (1<<4)  ///< Transmit tx_data member of spi_lobo_transaction_t instead of data at tx_buffer. Do not set tx_buffer when using this.



/**
 * Description for one SPI transmission
 */
struct ESP32_SPI_Module_spi_transaction_t {
    uint32_t flags;                 ///< Bitwise OR of LB_SPI_TRANS_* flags
    uint16_t command;               ///< Command data. Specific length was given when device was added to the bus.
    uint64_t address;               ///< Address. Specific length was given when device was added to the bus.
    size_t length;                  ///< Total data length to be transmitted to the device, in bits; if 0, no data is transmitted
    size_t rxlength;                ///< Total data length to be received from the device, in bits; if 0, no data is received
    void *user;                     ///< User-defined variable. Can be used to store eg transaction ID or data to be used by pre_cb and/or post_cb callbacks.
    union {
        const void *tx_buffer;      ///< Pointer to transmit buffer, or NULL for no MOSI phase
        uint8_t tx_data[4];         ///< If SPI_USE_TXDATA is set, data set here is sent directly from this variable.
    };
    union {
        void *rx_buffer;            ///< Pointer to receive buffer, or NULL for no MISO phase
        uint8_t rx_data[4];         ///< If SPI_USE_RXDATA is set, data is received directly to this variable
    };
};



#define NO_CS 3				// Number of CS pins per SPI host
#define NO_DEV 6			// Number of spi devices per SPI host; more than 3 devices can be attached to the same bus if using software CS's
#define SPI_SEMAPHORE_WAIT 2000		// Time in ms to wait for SPI mutex




typedef struct ESP32_SPI_Module_spi_device_t ESP32_SPI_Module_spi_device_t;

typedef struct {
    ESP32_SPI_Module_spi_device_t *device[NO_DEV];
    intr_handle_t intr;
    spi_dev_t *hw;
    //ESP32_SPI_Module_spi_transaction_t *cur_trans;
    int cur_device;
    lldesc_t *dmadesc_tx;
    lldesc_t *dmadesc_rx;
    bool no_gpio_matrix;
    int dma_chan;
    int max_transfer_sz;
    QueueHandle_t ESP32_SPI_Module_spi_bus_mutex;
    ESP32_SPI_Module_spi_bus_config_t cur_bus_config;
} ESP32_SPI_Module_spi_host_t;




struct ESP32_SPI_Module_spi_device_t {
  ESP32_SPI_Module_spi_device_interface_config_t cfg;
  ESP32_SPI_Module_spi_host_t *host;
  ESP32_SPI_Module_spi_bus_config_t bus_config;
  ESP32_SPI_Module_spi_host_device_t host_dev;
};




typedef ESP32_SPI_Module_spi_device_t* ESP32_SPI_Module_spi_device_handle_t;  ///< Handle for a device on a SPI bus
typedef ESP32_SPI_Module_spi_host_t* ESP32_SPI_Module_spi_host_handle_t;
typedef ESP32_SPI_Module_spi_device_interface_config_t* ESP32_SPI_Module_spi_device_interface_config_handle_t;



// -------------------------------------------------------------------------------------------------



//-> internal, nach .s verschieben

/**
 * @brief Add a device. This allocates a CS line for the device, allocates memory for the device structure and hooks
 *        up the CS pin to whatever is specified.
 *
 * This initializes the internal structures for a device, plus allocates a CS pin on the indicated SPI master
 * peripheral and routes it to the indicated GPIO. All SPI master devices have three hw CS pins and can thus control
 * up to three devices. Software handled CS pin can also be used for additional devices on the same SPI bus.
 * 
 * ### If selected SPI host device bus is not yet initialized, it is initialized first with 'bus_config' function ###
 *
 * @note While in general, speeds up to 80MHz on the dedicated SPI pins and 40MHz on GPIO-matrix-routed pins are
 *       supported, full-duplex transfers routed over the GPIO matrix only support speeds up to 26MHz.
 *
 * @param host SPI peripheral to allocate device on (HSPI or VSPI)
 * @param dev_config SPI interface protocol config for the device
 * @param bus_config Pointer to a spi_lobo_bus_config_t struct specifying how the host device bus should be initialized
 * @param handle Pointer to variable to hold the device handle
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_ERR_NOT_FOUND     if host doesn't have any free CS slots
 *         - ESP_ERR_NO_MEM        if out of memory
 *         - ESP_OK                on success
 */
esp_err_t ESP32_SPI_spi_bus_add_device(ESP32_SPI_Module_spi_host_device_t host, ESP32_SPI_Module_spi_bus_config_t *bus_config, ESP32_SPI_Module_spi_device_interface_config_t *dev_config, ESP32_SPI_Module_spi_device_handle_t *handle);

/**
 * @brief Remove a device from the SPI bus. If after removal no other device is attached to the spi bus device, it is freed.
 *
 * @param handle Device handle to free
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_ERR_INVALID_STATE if device already is freed
 *         - ESP_OK                on success
 */
esp_err_t ESP32_SPI_spi_bus_remove_device(ESP32_SPI_Module_spi_device_handle_t handle);

/**
 * @brief Return the actuall SPI bus speed for the spi device in Hz
 *
 * Some frequencies cannot be set, for example 30000000 will actually set SPI clock to 26666666 Hz
 *
 * @param handle Device handle obtained using spi_lobo_bus_add_device
 * 
 * @return 
 *         - actuall SPI clock
 */
uint32_t ESP32_SPI_spi_get_speed(ESP32_SPI_Module_spi_device_handle_t handle);

/**
 * @brief Set the new clock speed for the device, return the actuall SPI bus speed set, in Hz
 *        This function can be used after the device is initialized
 *
 * Some frequencies cannot be set, for example 30000000 will actually set SPI clock to 26666666 Hz
 *
 * @param handle Device handle obtained using spi_lobo_bus_add_device
 * @param speed  New device spi clock to be set in Hz
 * 
 * @return 
 *         - actuall SPI clock
 *         - 0 if speed cannot be set
 */
uint32_t ESP32_SPI_spi_set_speed(ESP32_SPI_Module_spi_device_handle_t handle, uint32_t speed);

/**
 * @brief Select spi device for transmission
 *
 * It configures spi bus with selected spi device parameters if previously selected device was different than the current
 * If device's spics_io_num=-1 and spics_ext_io_num > 0 'spics_ext_io_num' pin is set to active state (low)
 * 
 * spi bus device's semaphore is taken before selecting the device
 *
 * @param handle Device handle obtained using spi_lobo_bus_add_device
 * @param force  configure spi bus even if the previous device was the same
 * 
 * @return 
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_OK                on success
 */
esp_err_t ESP32_SPI_spi_device_select(ESP32_SPI_Module_spi_device_handle_t handle, int force);

/**
 * @brief De-select spi device
 *
 * If device's spics_io_num=-1 and spics_ext_io_num > 0 'spics_ext_io_num' pin is set to inactive state (high)
 * 
 * spi bus device's semaphore is given after selecting the device
 * 
 * @param handle Device handle obtained using spi_lobo_bus_add_device
 * 
 * @return 
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_OK                on success
 */
esp_err_t ESP32_SPI_spi_device_deselect(ESP32_SPI_Module_spi_device_handle_t handle);

/**
 * @brief Check if spi bus uses native spi pins
 *
 * @param handle Device handle obtained using spi_lobo_bus_add_device
 * 
 * @return 
 *         - true        if native spi pins are used
 *         - false       if spi pins are routed through gpio matrix
 */
bool ESP32_SPI_spi_uses_native_pins(ESP32_SPI_Module_spi_device_handle_t handle);

/**
 * @brief Get spi bus native spi pins
 *
 * @param handle Device handle obtained using spi_lobo_bus_add_device
 * 
 * @return 
 *         places spi bus native pins in provided pointers
 */
void ESP32_SPI_spi_get_native_pins(int host, int *sdi, int *sdo, int *sck);

/**
 * @brief Transimit and receive data to/from spi device based on transaction data
 * 
 * TRANSMIT 8-bit data to spi device from 'trans->tx_buffer' or 'trans->tx_data' (trans->lenght/8 bytes)
 * and RECEIVE data to 'trans->rx_buffer' or 'trans->rx_data' (trans->rx_length/8 bytes)
 * Lengths must be 8-bit multiples!
 * If trans->rx_buffer is NULL or trans->rx_length is 0, only transmits data
 * If trans->tx_buffer is NULL or trans->length is 0, only receives data
 * If the device is in duplex mode (LB_SPI_DEVICE_HALFDUPLEX flag NOT set), data are transmitted and received simultaneously.
 * If the device is in half duplex mode (LB_SPI_DEVICE_HALFDUPLEX flag IS set), data are received after transmission
 * 'address', 'command' and 'dummy bits' are transmitted before data phase IF set in device's configuration
 *   and IF 'trans->length' and 'trans->rx_length' are NOT both 0
 * If device was not previously selected, it will be selected before transmission and deselected after transmission.
 *
 * @param handle Device handle obtained using spi_lobo_bus_add_device
 * 
 * @param trans Pointer to variable containing the description of the transaction that is executed
 *
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP error code        if device cannot be selected
 *         - ESP_OK                on success
 *
 */
esp_err_t ESP32_SPI_spi_transfer_data(ESP32_SPI_Module_spi_device_handle_t handle, ESP32_SPI_Module_spi_transaction_t *trans);

/*
 * SPI transactions uses the semaphore (taken in select function) to protect the transfer
 */
esp_err_t ESP32_SPI_spi_device_TakeSemaphore(ESP32_SPI_Module_spi_device_handle_t handle);

void ESP32_SPI_spi_device_GiveSemaphore(ESP32_SPI_Module_spi_device_handle_t handle);

/**
 * @brief Setup a DMA link chain
 *
 * This routine will set up a chain of linked DMA descriptors in the array pointed to by
 * ``dmadesc``. Enough DMA descriptors will be used to fit the buffer of ``len`` bytes in, and the
 * descriptors will point to the corresponding positions in ``buffer`` and linked together. The
 * end result is that feeding ``dmadesc[0]`` into DMA hardware results in the entirety ``len`` bytes
 * of ``data`` being read or written.
 *
 * @param dmadesc Pointer to array of DMA descriptors big enough to be able to convey ``len`` bytes
 * @param len Length of buffer
 * @param data Data buffer to use for DMA transfer
 * @param isrx True if data is to be written into ``data``, false if it's to be read from ``data``.
 */
void ESP32_SPI_spi_setup_dma_desc_links(lldesc_t *dmadesc, int len, const uint8_t *data, bool isrx);

/**
 * @brief Check if a DMA reset is requested but has not completed yet
 *
 * @return True when a DMA reset is requested but hasn't completed yet. False otherwise.
 */
bool ESP32_SPI_spi_dmaworkaround_reset_in_progress();

/**
 * @brief Mark a DMA channel as idle.
 *
 * A call to this function tells the workaround logic that this channel will
 * not be affected by a global SPI DMA reset.
 */
void ESP32_SPI_spi_dmaworkaround_idle(int dmachan);

/**
 * @brief Mark a DMA channel as active.
 *
 * A call to this function tells the workaround logic that this channel will
 * be affected by a global SPI DMA reset, and a reset like that should not be attempted.
 */
void ESP32_SPI_spi_dmaworkaround_transfer_active(int dmachan);



// -------------------------------------------------------------------------------------------------



/*
 * typedefs of ESP32_SPI_Module Function Callbacks
 * This Fn are provided & made accessible for client modules - for operation
 */
// typedef for ESP32_SPI_Module_spi_bus_add_deviceFn - 
typedef esp_err_t (*ESP32_SPI_spi_bus_add_deviceFn_t) (ESP32_SPI_Module_spi_host_device_t host, ESP32_SPI_Module_spi_bus_config_t *bus_config, ESP32_SPI_Module_spi_device_interface_config_t *dev_config, ESP32_SPI_Module_spi_device_handle_t *handle);

// typedef for ESP32_SPI_spi_bus_remove_deviceFn - 
typedef esp_err_t (*ESP32_SPI_spi_bus_remove_deviceFn_t) (ESP32_SPI_Module_spi_device_handle_t handle);

// typedef for ESP32_SPI_spi_get_speedFn - 
typedef uint32_t (*ESP32_SPI_spi_get_speedFn_t) (ESP32_SPI_Module_spi_device_handle_t handle);

// typedef for ESP32_SPI_spi_set_speedFn - 
typedef uint32_t (*ESP32_SPI_spi_set_speedFn_t) (ESP32_SPI_Module_spi_device_handle_t handle, uint32_t speed);

// typedef for ESP32_SPI_spi_device_selectFn - 
typedef esp_err_t (*ESP32_SPI_spi_device_selectFn_t) (ESP32_SPI_Module_spi_device_handle_t handle, int force);

// typedef for ESP32_SPI_spi_device_deselectFn - 
typedef esp_err_t (*ESP32_SPI_spi_device_deselectFn_t) (ESP32_SPI_Module_spi_device_handle_t handle);

// typedef for ESP32_SPI_spi_uses_native_pinsFn - 
typedef bool (*ESP32_SPI_spi_uses_native_pinsFn_t) (ESP32_SPI_Module_spi_device_handle_t handle);

// typedef for ESP32_SPI_get_native_pinsFn - 
typedef void (*ESP32_SPI_get_native_pinsFn_t) (int host, int *sdi, int *sdo, int *sck);
void ESP32_SPI_get_native_pins(int host, int *sdi, int *sdo, int *sck);

// typedef for ESP32_SPI_spi_transfer_dataFn - 
typedef esp_err_t (*ESP32_SPI_spi_transfer_dataFn_t) (ESP32_SPI_Module_spi_device_handle_t handle, ESP32_SPI_Module_spi_transaction_t *trans);

// typedef for ESP32_SPI_spi_device_TakeSemaphoreFn - 
typedef esp_err_t (*ESP32_SPI_spi_device_TakeSemaphoreFn_t) (ESP32_SPI_Module_spi_device_handle_t handle);

// typedef for ESP32_SPI_spi_device_GiveSemaphoreFn - 
typedef void (*ESP32_SPI_spi_device_GiveSemaphoreFn_t) (ESP32_SPI_Module_spi_device_handle_t handle);

// typedef for ESP32_SPI_spi_setup_dma_desc_linksFn - 
typedef void (*ESP32_SPI_spi_setup_dma_desc_linksFn_t) (lldesc_t *dmadesc, int len, const uint8_t *data, bool isrx);

// typedef for ESP32_SPI_spi_dmaworkaround_reset_in_progressFn - 
typedef bool (*ESP32_SPI_spi_dmaworkaround_reset_in_progressFn_t) ();

// typedef for ESP32_SPI_Module_spi_dmaworkaround_idleFn - 
typedef void (*ESP32_SPI_spi_dmaworkaround_idleFn_t) (int dmachan);

// typedef for ESP32_SPI_Module_spi_dmaworkaround_transfer_activeFn - 
typedef void (*ESP32_SPI_spi_dmaworkaround_transfer_activeFn_t) (int dmachan);



/*
 * ESP32_SPI_Master Fn (SPI Functions) - table
 * Stores function callbacks provided & made accessible for client modules using this module
 */
typedef struct ESP32_SPI_Fn_s {
  ESP32_SPI_spi_bus_add_deviceFn_t ESP32_SPI_spi_bus_add_deviceFn;		// ?
  ESP32_SPI_spi_bus_remove_deviceFn_t ESP32_SPI_spi_bus_remove_deviceFn;	// ?
  ESP32_SPI_spi_get_speedFn_t ESP32_SPI_spi_get_speedFn;			// ?
  ESP32_SPI_spi_set_speedFn_t ESP32_SPI_spi_set_speedFn;			// ?
  ESP32_SPI_spi_device_selectFn_t ESP32_SPI_spi_device_selectFn;		// ?
  ESP32_SPI_spi_device_deselectFn_t ESP32_SPI_spi_device_deselectFn;		// ?
  ESP32_SPI_spi_uses_native_pinsFn_t ESP32_SPI_spi_uses_native_pinsFn;		// ?
  ESP32_SPI_get_native_pinsFn_t ESP32_SPI_get_native_pinsFn;			// ?
  ESP32_SPI_spi_transfer_dataFn_t ESP32_SPI_spi_transfer_dataFn;		// ?
  ESP32_SPI_spi_device_TakeSemaphoreFn_t ESP32_SPI_spi_device_TakeSemaphoreFn;	// ?
  ESP32_SPI_spi_device_GiveSemaphoreFn_t ESP32_SPI_spi_device_GiveSemaphoreFn;	// ?
  ESP32_SPI_spi_setup_dma_desc_linksFn_t ESP32_SPI_spi_setup_dma_desc_linksFn;	// ?
  ESP32_SPI_spi_dmaworkaround_reset_in_progressFn_t ESP32_SPI_spi_dmaworkaround_reset_in_progressFn;	// ?
  ESP32_SPI_spi_dmaworkaround_idleFn_t ESP32_SPI_spi_dmaworkaround_idleFn;	// ?
  ESP32_SPI_spi_dmaworkaround_transfer_activeFn_t ESP32_SPI_spi_dmaworkaround_transfer_activeFn;	// ?
} ESP32_SPI_Fn_t;


//NEW
/*
 * ESP32_SPI_Master Fn (SPI Functions) - table
 * Stores function callbacks provided & made accessible for client modules using this module
 */
typedef struct ESP32_SPI_provided_fn_s {
// --- first the common module functions ---
  ProvidedByModule_t ESP32_SPI_common_fn;	// the common functions ...
// --- now the custom module fuctions ---
  ESP32_SPI_spi_bus_add_deviceFn_t ESP32_SPI_spi_bus_add_deviceFn;		// ?
  ESP32_SPI_spi_bus_remove_deviceFn_t ESP32_SPI_spi_bus_remove_deviceFn;	// ?
  ESP32_SPI_spi_get_speedFn_t ESP32_SPI_spi_get_speedFn;			// ?
  ESP32_SPI_spi_set_speedFn_t ESP32_SPI_spi_set_speedFn;			// ?
  ESP32_SPI_spi_device_selectFn_t ESP32_SPI_spi_device_selectFn;		// ?
  ESP32_SPI_spi_device_deselectFn_t ESP32_SPI_spi_device_deselectFn;		// ?
  ESP32_SPI_spi_uses_native_pinsFn_t ESP32_SPI_spi_uses_native_pinsFn;		// ?
  ESP32_SPI_get_native_pinsFn_t ESP32_SPI_get_native_pinsFn;			// ?
  ESP32_SPI_spi_transfer_dataFn_t ESP32_SPI_spi_transfer_dataFn;		// ?
  ESP32_SPI_spi_device_TakeSemaphoreFn_t ESP32_SPI_spi_device_TakeSemaphoreFn;	// ?
  ESP32_SPI_spi_device_GiveSemaphoreFn_t ESP32_SPI_spi_device_GiveSemaphoreFn;	// ?
  ESP32_SPI_spi_setup_dma_desc_linksFn_t ESP32_SPI_spi_setup_dma_desc_linksFn;	// ?
  ESP32_SPI_spi_dmaworkaround_reset_in_progressFn_t ESP32_SPI_spi_dmaworkaround_reset_in_progressFn;	// ?
  ESP32_SPI_spi_dmaworkaround_idleFn_t ESP32_SPI_spi_dmaworkaround_idleFn;	// ?
  ESP32_SPI_spi_dmaworkaround_transfer_activeFn_t ESP32_SPI_spi_dmaworkaround_transfer_activeFn;	// ?
} ESP32_SPI_provided_Fn_t;

// -------------------------------------------------------------------------------------------------



/** 
 * ESP32_SPI_Definition stores values for operation valid only for the defined instance of an
 * loaded module. Values are initialized by HCTRLD an the loaded module itself.
 */
typedef struct ESP32_SPI_Definition_s {
  Common_Definition_t common;			/*!< ... the common part of the definition */
  WebIf_Provided_t WebIf_Provided;		/*!< provided data for WebIf */
//  uint8_t i2c_num;				/*!< the I2C hardware that should be used */
//  i2c_config_t i2c_config;			/*!< i2c configuration */
//  i2c_obj_t *i2c_obj;				/*!< the current i2c job */
  ESP32_SPI_Fn_t* ESP32_SPI_Fn;	/*!< ESP32_I2C_MasterFn (Functions / callbacks) accessible for client modules */
} ESP32_SPI_Definition_t;



// -------------------------------------------------------------------------------------------------



/* 
 * ESP32_SPI_StageXCHG stores values for operation of 2. stage design Modules (stage exchange)
 */
typedef struct ESP32_SPI_StageXCHG_s {
  Common_StageXCHG_t common;		// ... the common part of the StageXCHG
  // module specific here 

} ESP32_SPI_StageXCHG_t;



// -------------------------------------------------------------------------------------------------



#endif /*ESP32_SPI_MODULE_S_H*/
