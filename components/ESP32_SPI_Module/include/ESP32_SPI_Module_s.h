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
// ESP32_SPI_ ... common


//Maximum amount of bytes that can be put in one DMA descriptor
#define SPI_MAX_DMA_LEN (4096-4)



/**
 * Transform unsigned integer of length <= 32 bits to the format which can be
 * sent by the SPI driver directly.
 *
 * E.g. to send 9 bits of data, you can:
 *
 *      uint16_t data = SPI_SWAP_DATA_TX(0x145, 9);
 *
 * Then points tx_buffer to ``&data``.
 *
 * @param data Data to be sent, can be uint8_t, uint16_t or uint32_t. @param
 *  len Length of data to be sent, since the SPI peripheral sends from the MSB,
 *  this helps to shift the data to the MSB.
 */
#define SPI_SWAP_DATA_TX(data, len) __builtin_bswap32((uint32_t)data<<(32-len))



/**
 * Transform received data of length <= 32 bits to the format of an unsigned integer.
 *
 * E.g. to transform the data of 15 bits placed in a 4-byte array to integer:
 *
 *      uint16_t data = SPI_SWAP_DATA_RX(*(uint32_t*)t->rx_data, 15);
 *
 * @param data Data to be rearranged, can be uint8_t, uint16_t or uint32_t.
 * @param len Length of data received, since the SPI peripheral writes from
 *      the MSB, this helps to shift the data to the LSB.
 */
#define SPI_SWAP_DATA_RX(data, len) (__builtin_bswap32(data)>>(32-len))




/**
 * @brief Enum with the three SPI peripherals that are software-accessible in it
 */
typedef enum {
    SPI_HOST=0,                     ///< SPI1, SPI
    HSPI_HOST=1,                    ///< SPI2, HSPI
    VSPI_HOST=2                     ///< SPI3, VSPI
} ESP32_SPI_host_device_t;



/**
 * @brief This is a configuration structure for a SPI bus.
 *
 * You can use this structure to specify the GPIO pins of the bus. Normally, the driver will use the
 * GPIO matrix to route the signals. An exception is made when all signals either can be routed through
 * the IO_MUX or are -1. In that case, the IO_MUX is used, allowing for >40MHz speeds.
 *
 * @note Be advised that the slave driver does not use the quadwp/quadhd lines and fields in spi_bus_config_t refering to these lines will be ignored and can thus safely be left uninitialized.
 */
typedef struct {
    int mosi_io_num;                ///< GPIO pin for Master Out Slave In (=spi_d) signal, or -1 if not used.
    int miso_io_num;                ///< GPIO pin for Master In Slave Out (=spi_q) signal, or -1 if not used.
    int sclk_io_num;                ///< GPIO pin for Spi CLocK signal, or -1 if not used.
    int quadwp_io_num;              ///< GPIO pin for WP (Write Protect) signal which is used as D2 in 4-bit communication modes, or -1 if not used.
    int quadhd_io_num;              ///< GPIO pin for HD (HolD) signal which is used as D3 in 4-bit communication modes, or -1 if not used.
    int max_transfer_sz;            ///< Maximum transfer size, in bytes. Defaults to 4094 if 0.
    uint32_t flags;                 ///< Abilities of bus to be checked by the driver. Or-ed value of ``SPICOMMON_BUSFLAG_*`` flags.
    int intr_flags;    /**< Interrupt flag for the bus to set the priority, and IRAM attribute, see
                         *  ``esp_intr_alloc.h``. Note that the EDGE, INTRDISABLED attribute are ignored
                         *  by the driver. Note that if ESP_INTR_FLAG_IRAM is set, ALL the callbacks of
                         *  the driver, and their callee functions, should be put in the IRAM.
                         */
} ESP32_SPI_bus_config_t;
























// -------------------------------------------------------------------------------------------------
// ESP32_SPI_Master - starts here

/** SPI master clock is divided by 80MHz apb clock. Below defines are example frequencies, and are accurate. Be free to specify a random frequency, it will be rounded to closest frequency (to macros below if above 8MHz).
  * 8MHz
  */
#define SPI_MASTER_FREQ_8M      (APB_CLK_FREQ/10)
#define SPI_MASTER_FREQ_9M      (APB_CLK_FREQ/9)    ///< 8.89MHz
#define SPI_MASTER_FREQ_10M     (APB_CLK_FREQ/8)    ///< 10MHz
#define SPI_MASTER_FREQ_11M     (APB_CLK_FREQ/7)    ///< 11.43MHz
#define SPI_MASTER_FREQ_13M     (APB_CLK_FREQ/6)    ///< 13.33MHz
#define SPI_MASTER_FREQ_16M     (APB_CLK_FREQ/5)    ///< 16MHz
#define SPI_MASTER_FREQ_20M     (APB_CLK_FREQ/4)    ///< 20MHz
#define SPI_MASTER_FREQ_26M     (APB_CLK_FREQ/3)    ///< 26.67MHz
#define SPI_MASTER_FREQ_40M     (APB_CLK_FREQ/2)    ///< 40MHz
#define SPI_MASTER_FREQ_80M     (APB_CLK_FREQ/1)    ///< 80MHz



#define SPI_DEVICE_TXBIT_LSBFIRST          (1<<0)  ///< Transmit command/address/data LSB first instead of the default MSB first
#define SPI_DEVICE_RXBIT_LSBFIRST          (1<<1)  ///< Receive data LSB first instead of the default MSB first
#define SPI_DEVICE_BIT_LSBFIRST            (SPI_DEVICE_TXBIT_LSBFIRST|SPI_DEVICE_RXBIT_LSBFIRST) ///< Transmit and receive LSB first
#define SPI_DEVICE_3WIRE                   (1<<2)  ///< Use MOSI (=spid) for both sending and receiving data
#define SPI_DEVICE_POSITIVE_CS             (1<<3)  ///< Make CS positive during a transaction instead of negative
#define SPI_DEVICE_HALFDUPLEX              (1<<4)  ///< Transmit data before receiving it, instead of simultaneously
#define SPI_DEVICE_CLK_AS_CS               (1<<5)  ///< Output clock on CS line if CS is active
/** There are timing issue when reading at high frequency (the frequency is related to whether iomux pins are used, valid time after slave sees the clock).
  *     - In half-duplex mode, the driver automatically inserts dummy bits before reading phase to fix the timing issue. Set this flag to disable this feature.
  *     - In full-duplex mode, however, the hardware cannot use dummy bits, so there is no way to prevent data being read from getting corrupted.
  *       Set this flag to confirm that you're going to work with output only, or read without dummy bits at your own risk.
  */
#define SPI_DEVICE_NO_DUMMY                (1<<6)



typedef struct ESP32_SPI_transaction_t ESP32_SPI_transaction_t;
typedef void(*transaction_cb_t)(ESP32_SPI_transaction_t *trans);



/**
 * @brief This is a configuration for a SPI slave device that is connected to one of the SPI buses.
 */
typedef struct {
    uint8_t command_bits;           ///< Default amount of bits in command phase (0-16), used when ``SPI_TRANS_VARIABLE_CMD`` is not used, otherwise ignored.
    uint8_t address_bits;           ///< Default amount of bits in address phase (0-64), used when ``SPI_TRANS_VARIABLE_ADDR`` is not used, otherwise ignored.
    uint8_t dummy_bits;             ///< Amount of dummy bits to insert between address and data phase
    uint8_t mode;                   ///< SPI mode (0-3)
    uint8_t duty_cycle_pos;         ///< Duty cycle of positive clock, in 1/256th increments (128 = 50%/50% duty). Setting this to 0 (=not setting it) is equivalent to setting this to 128.
    uint8_t cs_ena_pretrans;        ///< Amount of SPI bit-cycles the cs should be activated before the transmission (0-16). This only works on half-duplex transactions.
    uint8_t cs_ena_posttrans;       ///< Amount of SPI bit-cycles the cs should stay active after the transmission (0-16)
    int clock_speed_hz;             ///< Clock speed, divisors of 80MHz, in Hz. See ``SPI_MASTER_FREQ_*``.
    int input_delay_ns;             /**< Maximum data valid time of slave. The time required between SCLK and MISO
        valid, including the possible clock delay from slave to master. The driver uses this value to give an extra
        delay before the MISO is ready on the line. Leave at 0 unless you know you need a delay. For better timing
        performance at high frequency (over 8MHz), it's suggest to have the right value.
        */
    int spics_io_num;               ///< CS GPIO pin for this device, or -1 if not used
    uint32_t flags;                 ///< Bitwise OR of SPI_DEVICE_* flags
    int queue_size;                 ///< Transaction queue size. This sets how many transactions can be 'in the air' (queued using spi_device_queue_trans but not yet finished using spi_device_get_trans_result) at the same time
    transaction_cb_t pre_cb;   /**< Callback to be called before a transmission is started.
                                 *
                                 *  This callback is called within interrupt
                                 *  context should be in IRAM for best
                                 *  performance, see "Transferring Speed"
                                 *  section in the SPI Master documentation for
                                 *  full details. If not, the callback may crash
                                 *  during flash operation when the driver is
                                 *  initialized with ESP_INTR_FLAG_IRAM.
                                 */
    transaction_cb_t post_cb;  /**< Callback to be called after a transmission has completed.
                                 *
                                 *  This callback is called within interrupt
                                 *  context should be in IRAM for best
                                 *  performance, see "Transferring Speed"
                                 *  section in the SPI Master documentation for
                                 *  full details. If not, the callback may crash
                                 *  during flash operation when the driver is
                                 *  initialized with ESP_INTR_FLAG_IRAM.
                                 */
} ESP32_SPI_device_interface_config_t;




#define SPI_TRANS_MODE_DIO            (1<<0)  ///< Transmit/receive data in 2-bit mode
#define SPI_TRANS_MODE_QIO            (1<<1)  ///< Transmit/receive data in 4-bit mode
#define SPI_TRANS_USE_RXDATA          (1<<2)  ///< Receive into rx_data member of spi_transaction_t instead into memory at rx_buffer.
#define SPI_TRANS_USE_TXDATA          (1<<3)  ///< Transmit tx_data member of spi_transaction_t instead of data at tx_buffer. Do not set tx_buffer when using this.
#define SPI_TRANS_MODE_DIOQIO_ADDR    (1<<4)  ///< Also transmit address in mode selected by SPI_MODE_DIO/SPI_MODE_QIO
#define SPI_TRANS_VARIABLE_CMD        (1<<5)  ///< Use the ``command_bits`` in ``spi_transaction_ext_t`` rather than default value in ``spi_device_interface_config_t``.
#define SPI_TRANS_VARIABLE_ADDR       (1<<6)  ///< Use the ``address_bits`` in ``spi_transaction_ext_t`` rather than default value in ``spi_device_interface_config_t``.



/**
 * This structure describes one SPI transaction. The descriptor should not be modified until the transaction finishes.
 */
struct ESP32_SPI_transaction_t {
    uint32_t flags;                 ///< Bitwise OR of SPI_TRANS_* flags
    uint16_t cmd;                   /**< Command data, of which the length is set in the ``command_bits`` of spi_device_interface_config_t.
                                      *
                                      *  <b>NOTE: this field, used to be "command" in ESP-IDF 2.1 and before, is re-written to be used in a new way in ESP-IDF 3.0.</b>
                                      *
                                      *  Example: write 0x0123 and command_bits=12 to send command 0x12, 0x3_ (in previous version, you may have to write 0x3_12).
                                      */
    uint64_t addr;                  /**< Address data, of which the length is set in the ``address_bits`` of spi_device_interface_config_t.
                                      *
                                      *  <b>NOTE: this field, used to be "address" in ESP-IDF 2.1 and before, is re-written to be used in a new way in ESP-IDF3.0.</b>
                                      *
                                      *  Example: write 0x123400 and address_bits=24 to send address of 0x12, 0x34, 0x00 (in previous version, you may have to write 0x12340000).
                                      */
    size_t length;                  ///< Total data length, in bits
    size_t rxlength;                ///< Total data length received, should be not greater than ``length`` in full-duplex mode (0 defaults this to the value of ``length``).
    void *user;                     ///< User-defined variable. Can be used to store eg transaction ID.
    union {
        const void *tx_buffer;      ///< Pointer to transmit buffer, or NULL for no MOSI phase
        uint8_t tx_data[4];         ///< If SPI_USE_TXDATA is set, data set here is sent directly from this variable.
    };
    union {
        void *rx_buffer;            ///< Pointer to receive buffer, or NULL for no MISO phase. Written by 4 bytes-unit if DMA is used.
        uint8_t rx_data[4];         ///< If SPI_USE_RXDATA is set, data is received directly to this variable
    };
} ;        //the rx data should start from a 32-bit aligned address to get around dma issue.



/**
 * This struct is for SPI transactions which may change their address and command length.
 * Please do set the flags in base to ``SPI_TRANS_VARIABLE_CMD_ADR`` to use the bit length here.
 */
typedef struct {
    struct ESP32_SPI_transaction_t base;  ///< Transaction data, so that pointer to spi_transaction_t can be converted into spi_transaction_ext_t
    uint8_t command_bits;           ///< The command length in this transaction, in bits.
    uint8_t address_bits;           ///< The address length in this transaction, in bits.
} ESP32_SPI_transaction_ext_t ;



// Handle for a device on a SPI bus (equal to ESP32_SPI_device_t*)
typedef struct ESP32_SPI_device_t* ESP32_SPI_device_handle_t;

typedef struct ESP32_SPI_Definition_s ESP32_SPI_Definition_t;






#define SPICOMMON_BUSFLAG_SLAVE         0          ///< Initialize I/O in slave mode
#define SPICOMMON_BUSFLAG_MASTER        (1<<0)     ///< Initialize I/O in master mode
#define SPICOMMON_BUSFLAG_NATIVE_PINS   (1<<1)     ///< Check using iomux pins. Or indicates the pins are configured through the IO mux rather than GPIO matrix.
#define SPICOMMON_BUSFLAG_SCLK          (1<<2)     ///< Check existing of SCLK pin. Or indicates CLK line initialized.
#define SPICOMMON_BUSFLAG_MISO          (1<<3)     ///< Check existing of MISO pin. Or indicates MISO line initialized.
#define SPICOMMON_BUSFLAG_MOSI          (1<<4)     ///< Check existing of MOSI pin. Or indicates CLK line initialized.
#define SPICOMMON_BUSFLAG_DUAL          (1<<5)     ///< Check MOSI and MISO pins can output. Or indicates bus able to work under DIO mode.
#define SPICOMMON_BUSFLAG_WPHD          (1<<6)     ///< Check existing of WP and HD pins. Or indicates WP & HD pins initialized.
#define SPICOMMON_BUSFLAG_QUAD          (SPICOMMON_BUSFLAG_DUAL|SPICOMMON_BUSFLAG_WPHD)     ///< Check existing of MOSI/MISO/WP/HD pins as output. Or indicates bus able to work under QIO mode.








// -------------------------------------------------------------------------------------------------

typedef struct ESP32_SPI_device_t ESP32_SPI_device_t;

typedef typeof(SPI1.clock) ESP32_SPI_clock_reg_t;

#define NO_CS 3     			// Number of CS pins per SPI host

/// struct to hold private transaction data (like tx and rx buffer for DMA).
typedef struct {
    ESP32_SPI_transaction_t   *trans;
    const uint32_t *buffer_to_send;   	// equals to tx_data, if SPI_TRANS_USE_RXDATA is applied; otherwise if original buffer wasn't in DMA-capable memory, this gets the address of a temporary buffer that is;
                                	// otherwise sets to the original buffer or NULL if no buffer is assigned.
    uint32_t *buffer_to_rcv;    	// similar to buffer_to_send
} ESP32_SPI_trans_priv_t;

typedef struct {
  ESP32_SPI_host_device_t host_device; 	// the spi pheriperal we are using for this 
					// definition(added by maik)

    _Atomic(ESP32_SPI_device_t*) device[NO_CS];
    intr_handle_t intr;
    spi_dev_t *hw;
    ESP32_SPI_trans_priv_t cur_trans_buf;
    int cur_cs;     			// current device doing transaction
    int prev_cs;    			// last device doing transaction, used to avoid re-configure registers if the device not changed
    atomic_int acquire_cs; 		// the device acquiring the bus, NO_CS if no one is doing so.
    bool polling;   			// in process of a polling, avoid of queue new transactions into ISR
    bool isr_free;  			// the isr is not sending transactions
    bool bus_locked;			// the bus is controlled by a device
    lldesc_t *dmadesc_tx;
    lldesc_t *dmadesc_rx;
    uint32_t flags;
    int dma_chan;
    int max_transfer_sz;
    ESP32_SPI_bus_config_t bus_cfg;
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
#endif
} ESP32_SPI_host_t;

typedef struct {
    ESP32_SPI_clock_reg_t reg;
    int eff_clk;
    int dummy_num;
    int miso_delay;
} clock_config_t;

struct ESP32_SPI_device_t {
 // ESP32_SPI_Definition_t* ESP32_SPI_Definition; // added by maik - to get the host data but its there. will we need it?
    int id;
    QueueHandle_t trans_queue;
    QueueHandle_t ret_queue;
    ESP32_SPI_device_interface_config_t cfg;
    clock_config_t clk_cfg;
    ESP32_SPI_host_t* host;
    SemaphoreHandle_t semphr_polling;   // semaphore to notify the device it claimed the bus
    bool waiting;			// the device is waiting for the exclusive control of the bus
};


// -------------------------------------------------------------------------------------------------



/** 
 * ESP32_SPI_Definition stores values for operation valid only for the defined instance of an
 * loaded module. Values are initialized by HCTRLD an the loaded module itself.
 */
typedef struct ESP32_SPI_Definition_s {
  Common_Definition_t common;		/*!< ... the common part of the definition */
  WebIf_Provided_t WebIf_Provided;	/*!< provided data for WebIf */
  ESP32_SPI_host_t* p_host;             /*!< the spi bus pheripheral this definition is using */
  ESP32_SPI_bus_config_t* p_bus_config;	/*!< this definitions spi bus configuration (4 all devices??) */
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



/**
 * @brief Try to claim a SPI peripheral
 *
 * Call this if your driver wants to manage a SPI peripheral.
 *
 * @param host Peripheral to claim
 * @param source The caller indentification string.
 *
 * @return True if peripheral is claimed successfully; false if peripheral already is claimed.
 */
bool ESP32_SPI_common_periph_claim(ESP32_SPI_host_device_t host, const char* source);

// The macro is to keep the back-compatibility of IDF v3.2 and before
// In this way we can call spicommon_periph_claim with two arguments, or the host with the source set to the calling function name
// When two arguments (host, func) are given, __spicommon_periph_claim2 is called
// or if only one arguments (host) is given, __spicommon_periph_claim1 is called
#define ESP32_SPI_common_periph_claim(host...) __ESP32_SPI_common_periph_claim(host, 2, 1)
#define __ESP32_SPI_common_periph_claim(host, source, n, ...) __ESP32_SPI_common_periph_claim ## n(host, source)
#define __ESP32_SPI_common_periph_claim1(host, _)    ({ \
    char* warning_str = "calling ESP32_SPI_common_periph_claim without source string is deprecated.";\
    ESP32_SPI_common_periph_claim(host, __FUNCTION__); })

#define __ESP32_SPI_common_periph_claim2(host, func) ESP32_SPI_common_periph_claim(host, func)

/**
 * @brief Check whether the spi periph is in use.
 *
 * @param host Peripheral to check.
 *
 * @return True if in use, otherwise false.
 */
bool ESP32_SPI_common_periph_in_use(ESP32_SPI_host_device_t host);

/**
 * @brief Return the SPI peripheral so another driver can claim it.
 *
 * @param host Peripheral to return
 * @return True if peripheral is returned successfully; false if peripheral was free to claim already.
 */
bool ESP32_SPI_common_periph_free(ESP32_SPI_host_device_t host);


/**
 * @brief Try to claim a SPI DMA channel
 *
 *  Call this if your driver wants to use SPI with a DMA channnel.
 *

 * @param dma_chan channel to claim
 *
 * @return True if success; false otherwise.
 */
bool ESP32_SPI_common_dma_chan_claim(int dma_chan);

/**
 * @brief Check whether the spi DMA channel is in use.
 *
 * @param dma_chan DMA channel to check.
 *
 * @return True if in use, otherwise false.
 */
bool ESP32_SPI_common_dma_chan_in_use(int dma_chan);

/**
 * @brief Return the SPI DMA channel so other driver can claim it, or just to power down DMA.
 *
 * @param dma_chan channel to return
 *
 * @return True if success; false otherwise.
 */
bool ESP32_SPI_common_dma_chan_free(int dma_chan);


// -------------------------------------------------------------------------------------------------


/**
 * @brief Connect a SPI peripheral to GPIO pins
 *
 * This routine is used to connect a SPI peripheral to the IO-pads and DMA channel given in
 * the arguments. Depending on the IO-pads requested, the routing is done either using the
 * IO_mux or using the GPIO matrix.
 *
 * @param host SPI peripheral to be routed
 * @param bus_config Pointer to a spi_bus_config struct detailing the GPIO pins
 * @param dma_chan DMA-channel (1 or 2) to use, or 0 for no DMA.
 * @param flags Combination of SPICOMMON_BUSFLAG_* flags, set to ensure the pins set are capable with some functions:
 *              - ``SPICOMMON_BUSFLAG_MASTER``: Initialize I/O in master mode
 *              - ``SPICOMMON_BUSFLAG_SLAVE``: Initialize I/O in slave mode
 *              - ``SPICOMMON_BUSFLAG_NATIVE_PINS``: Pins set should match the iomux pins of the controller.
 *              - ``SPICOMMON_BUSFLAG_SCLK``, ``SPICOMMON_BUSFLAG_MISO``, ``SPICOMMON_BUSFLAG_MOSI``:
 *                  Make sure SCLK/MISO/MOSI is/are set to a valid GPIO. Also check output capability according to the mode.
 *              - ``SPICOMMON_BUSFLAG_DUAL``: Make sure both MISO and MOSI are output capable so that DIO mode is capable.

 *              - ``SPICOMMON_BUSFLAG_WPHD`` Make sure WP and HD are set to valid output GPIOs.
 *              - ``SPICOMMON_BUSFLAG_QUAD``: Combination of ``SPICOMMON_BUSFLAG_DUAL`` and ``SPICOMMON_BUSFLAG_WPHD``.
 * @param[out] flags_o A SPICOMMON_BUSFLAG_* flag combination of bus abilities will be written to this address.
 *              Leave to NULL if not needed.
 *              - ``SPICOMMON_BUSFLAG_NATIVE_PINS``: The bus is connected to iomux pins.
 *              - ``SPICOMMON_BUSFLAG_SCLK``, ``SPICOMMON_BUSFLAG_MISO``, ``SPICOMMON_BUSFLAG_MOSI``: The bus has
 *                  CLK/MISO/MOSI connected.
 *              - ``SPICOMMON_BUSFLAG_DUAL``: The bus is capable with DIO mode.
 *              - ``SPICOMMON_BUSFLAG_WPHD`` The bus has WP and HD connected.
 *              - ``SPICOMMON_BUSFLAG_QUAD``: Combination of ``SPICOMMON_BUSFLAG_DUAL`` and ``SPICOMMON_BUSFLAG_WPHD``.
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_OK                on success
 */
strTextMultiple_t* ESP32_SPI_common_bus_initialize_io(ESP32_SPI_Definition_t* ESP32_SPI_Definition, ESP32_SPI_host_device_t host, const ESP32_SPI_bus_config_t* bus_config, int dma_chan, uint32_t flags, uint32_t *flags_o);

/**
 * @brief Free the IO used by a SPI peripheral
 * @deprecated Use spicommon_bus_free_io_cfg instead.
 *
 * @param host SPI peripheral to be freed
 *
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid

 *         - ESP_OK                on success
 */
esp_err_t ESP32_SPI_common_bus_free_io(ESP32_SPI_host_device_t host) __attribute__((deprecated));

/**
 * @brief Free the IO used by a SPI peripheral
 *
 * @param bus_cfg Bus config struct which defines which pins to be used.
 *
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_OK                on success
 */
esp_err_t ESP32_SPI_common_bus_free_io_cfg(const ESP32_SPI_bus_config_t *bus_cfg);

/**
 * @brief Initialize a Chip Select pin for a specific SPI peripheral
 *
 *
 * @param host SPI peripheral
 * @param cs_io_num GPIO pin to route
 * @param cs_num CS id to route
 * @param force_gpio_matrix If true, CS will always be routed through the GPIO matrix. If false,
 *                          if the GPIO number allows it, the routing will happen through the IO_mux.
 */

void ESP32_SPI_common_cs_initialize(ESP32_SPI_host_device_t host, int cs_io_num, int cs_num, int force_gpio_matrix);

/**
 * @brief Free a chip select line
 * @deprecated Use spicommon_cs_io, which inputs the gpio num rather than the cs id instead.
 *
 * @param host SPI peripheral
 * @param cs_num CS id to free
 */
void ESP32_SPI_common_cs_free(ESP32_SPI_host_device_t host, int cs_num) __attribute__((deprecated));

/**
 * @brief Free a chip select line
 *
 * @param cs_gpio_num CS gpio num to free
 */
void ESP32_SPI_common_cs_free_io(int cs_gpio_num);

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
void ESP32_SPI_common_setup_dma_desc_links(lldesc_t *dmadesc, int len, const uint8_t *data, bool isrx);

/**
 * @brief Get the position of the hardware registers for a specific SPI host
 *
 * @param host The SPI host
 *
 * @return A register descriptor stuct pointer, pointed at the hardware registers
 */
spi_dev_t *ESP32_SPI_common_hw_for_host(ESP32_SPI_host_device_t host);

/**
 * @brief Get the IRQ source for a specific SPI host
 *
 * @param host The SPI host
 *
 * @return The hosts IRQ source
 */
int ESP32_SPI_common_irqsource_for_host(ESP32_SPI_host_device_t host);

/**
 * Callback, to be called when a DMA engine reset is completed
*/
typedef void(*dmaworkaround_cb_t)(void *arg);


/**
 * @brief Request a reset for a certain DMA channel
 *
 * @note In some (well-defined) cases in the ESP32 (at least rev v.0 and v.1), a SPI DMA channel will get confused. This can be remedied
 * by resetting the SPI DMA hardware in case this happens. Unfortunately, the reset knob used for thsi will reset _both_ DMA channels, and
 * as such can only done safely when both DMA channels are idle. These functions coordinate this.
 *
 * Essentially, when a reset is needed, a driver can request this using spicommon_dmaworkaround_req_reset. This is supposed to be called

 * with an user-supplied function as an argument. If both DMA channels are idle, this call will reset the DMA subsystem and return true.
 * If the other DMA channel is still busy, it will return false; as soon as the other DMA channel is done, however, it will reset the
 * DMA subsystem and call the callback. The callback is then supposed to be used to continue the SPI drivers activity.
 *
 * @param dmachan DMA channel associated with the SPI host that needs a reset
 * @param cb Callback to call in case DMA channel cannot be reset immediately
 * @param arg Argument to the callback
 *
 * @return True when a DMA reset could be executed immediately. False when it could not; in this
 *         case the callback will be called with the specified argument when the logic can execute
 *         a reset, after that reset.
 */
bool ESP32_SPI_common_dmaworkaround_req_reset(int dmachan, dmaworkaround_cb_t cb, void *arg);


/**
 * @brief Check if a DMA reset is requested but has not completed yet
 *
 * @return True when a DMA reset is requested but hasn't completed yet. False otherwise.
 */
bool ESP32_SPI_common_dmaworkaround_reset_in_progress();


/**
 * @brief Mark a DMA channel as idle.
 *
 * A call to this function tells the workaround logic that this channel will
 * not be affected by a global SPI DMA reset.
 */
void ESP32_SPI_common_dmaworkaround_idle(int dmachan);

/**

 * @brief Mark a DMA channel as active.
 *
 * A call to this function tells the workaround logic that this channel will
 * be affected by a global SPI DMA reset, and a reset like that should not be attempted.
 */
void ESP32_SPI_common_dmaworkaround_transfer_active(int dmachan);









// -------------------------------------------------------------------------------------------------



/**
 * @brief Initialize a SPI bus
 *
 * @warning For now, only supports HSPI and VSPI.
 *
 * @param host SPI peripheral that controls this bus
 * @param bus_config Pointer to a spi_bus_config_t struct specifying how the host should be initialized
 * @param dma_chan Either channel 1 or 2, or 0 in the case when no DMA is required. Selecting a DMA channel
 *                 for a SPI bus allows transfers on the bus to have sizes only limited by the amount of
 *                 internal memory. Selecting no DMA channel (by passing the value 0) limits the amount of
 *                 bytes transfered to a maximum of 32.
 *
 * @warning If a DMA channel is selected, any transmit and receive buffer used should be allocated in
 *          DMA-capable memory.
 *
 * @warning The ISR of SPI is always executed on the core which calls this
 *          function. Never starve the ISR on this core or the SPI transactions will not
 *          be handled.
 *
 * @return
 *         - ESP_ERR_INVALID_ARG   if configuration is invalid
 *         - ESP_ERR_INVALID_STATE if host already is in use
 *         - ESP_ERR_NO_MEM        if out of memory
 *         - ESP_OK                on success

 */
strTextMultiple_t* ESP32_SPI_bus_initialize(ESP32_SPI_Definition_t* ESP32_SPI_Definition, ESP32_SPI_host_device_t host_device, const ESP32_SPI_bus_config_t* bus_config, int dma_chan);

/**
 * @brief Free a SPI bus
 *
 * @warning In order for this to succeed, all devices have to be removed first.
 *
 * @param host SPI peripheral to free
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_ERR_INVALID_STATE if not all devices on the bus are freed
 *         - ESP_OK                on success
 */
strTextMultiple_t* ESP32_SPI_bus_free(ESP32_SPI_Definition_t* ESP32_SPI_Definition);

/**
 * @brief Allocate a device on a SPI bus
 *
 * This initializes the internal structures for a device, plus allocates a CS pin on the indicated SPI master
 * peripheral and routes it to the indicated GPIO. All SPI master devices have three CS pins and can thus control
 * up to three devices.
 *
 * @note While in general, speeds up to 80MHz on the dedicated SPI pins and 40MHz on GPIO-matrix-routed pins are
 *       supported, full-duplex transfers routed over the GPIO matrix only support speeds up to 26MHz.
 *
 * @param host SPI peripheral to allocate device on
 * @param dev_config SPI interface protocol config for the device
 * @param handle Pointer to variable to hold the device handle
 * @return

 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_ERR_NOT_FOUND     if host doesn't have any free CS slots
 *         - ESP_ERR_NO_MEM        if out of memory
 *         - ESP_OK                on success
 */
strTextMultiple_t*  ESP32_SPI_bus_add_device(ESP32_SPI_Definition_t* ESP32_SPI_Definition, const ESP32_SPI_device_interface_config_t *dev_config, ESP32_SPI_device_handle_t *handle);


/**
 * @brief Remove a device from the SPI bus
 *
 * @param handle Device handle to free
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_ERR_INVALID_STATE if device already is freed
 *         - ESP_OK                on success
 */
strTextMultiple_t* ESP32_SPI_bus_remove_device(ESP32_SPI_device_handle_t handle);


/**
 * @brief Queue a SPI transaction for interrupt transaction execution. Get the result by ``spi_device_get_trans_result``.
 *
 * @note Normally a device cannot start (queue) polling and interrupt

 *      transactions simultaneously.
 *
 * @param handle Device handle obtained using spi_host_add_dev
 * @param trans_desc Description of transaction to execute
 * @param ticks_to_wait Ticks to wait until there's room in the queue; use portMAX_DELAY to
 *                      never time out.
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_ERR_TIMEOUT       if there was no room in the queue before ticks_to_wait expired
 *         - ESP_ERR_NO_MEM        if allocating DMA-capable temporary buffer failed
 *         - ESP_ERR_INVALID_STATE if previous transactions are not finished
 *         - ESP_OK                on success
 */
esp_err_t ESP32_SPI_device_queue_trans(ESP32_SPI_device_handle_t handle, ESP32_SPI_transaction_t *trans_desc, TickType_t ticks_to_wait);


/**
 * @brief Get the result of a SPI transaction queued earlier by ``spi_device_queue_trans``.
 *
 * This routine will wait until a transaction to the given device
 * succesfully completed. It will then return the description of the
 * completed transaction so software can inspect the result and e.g. free the memory or
 * re-use the buffers.
 *
 * @param handle Device handle obtained using spi_host_add_dev
 * @param trans_desc Pointer to variable able to contain a pointer to the description of the transaction
        that is executed. The descriptor should not be modified until the descriptor is returned by
        spi_device_get_trans_result.

 * @param ticks_to_wait Ticks to wait until there's a returned item; use portMAX_DELAY to never time

                        out.
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_ERR_TIMEOUT       if there was no completed transaction before ticks_to_wait expired
 *         - ESP_OK                on success
 */
esp_err_t ESP32_SPI_device_get_trans_result(ESP32_SPI_device_handle_t handle, ESP32_SPI_transaction_t **trans_desc, TickType_t ticks_to_wait);


/**
 * @brief Send a SPI transaction, wait for it to complete, and return the result
 *
 * This function is the equivalent of calling spi_device_queue_trans() followed by spi_device_get_trans_result().
 * Do not use this when there is still a transaction separately queued (started) from spi_device_queue_trans() or polling_start/transmit that hasn't been finalized.
 *
 * @note This function is not thread safe when multiple tasks access the same SPI device.
 *      Normally a device cannot start (queue) polling and interrupt
 *      transactions simutanuously.
 *
 * @param handle Device handle obtained using spi_host_add_dev
 * @param trans_desc Description of transaction to execute
 * @return

 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_OK                on success
 */
esp_err_t ESP32_SPI_device_transmit(ESP32_SPI_device_handle_t handle, ESP32_SPI_transaction_t *trans_desc);


/**
 * @brief Immediately start a polling transaction.
 *
 * @note Normally a device cannot start (queue) polling and interrupt
 *      transactions simutanuously. Moreover, a device cannot start a new polling
 *      transaction if another polling transaction is not finished.
 *
 * @param handle Device handle obtained using spi_host_add_dev
 * @param trans_desc Description of transaction to execute
 * @param ticks_to_wait Ticks to wait until there's room in the queue;
 *              currently only portMAX_DELAY is supported.
 *
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_ERR_TIMEOUT       if the device cannot get control of the bus before ``ticks_to_wait`` expired
 *         - ESP_ERR_NO_MEM        if allocating DMA-capable temporary buffer failed
 *         - ESP_ERR_INVALID_STATE if previous transactions are not finished
 *         - ESP_OK                on success
 */
esp_err_t ESP32_SPI_device_polling_start(ESP32_SPI_device_handle_t handle, ESP32_SPI_transaction_t *trans_desc, TickType_t ticks_to_wait);


/**
 * @brief Poll until the polling transaction ends.
 *
 * This routine will not return until the transaction to the given device has
 * succesfully completed. The task is not blocked, but actively busy-spins for
 * the transaction to be completed.
 *
 * @param handle Device handle obtained using spi_host_add_dev
 * @param ticks_to_wait Ticks to wait until there's a returned item; use portMAX_DELAY to never time
                        out.
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_ERR_TIMEOUT       if the transaction cannot finish before ticks_to_wait expired
 *         - ESP_OK                on success
 */
esp_err_t ESP32_SPI_device_polling_end(ESP32_SPI_device_handle_t handle, TickType_t ticks_to_wait);


/**
 * @brief Send a polling transaction, wait for it to complete, and return the result
 *
 * This function is the equivalent of calling spi_device_polling_start() followed by spi_device_polling_end().
 * Do not use this when there is still a transaction that hasn't been finalized.
 *
 * @note This function is not thread safe when multiple tasks access the same SPI device.
 *      Normally a device cannot start (queue) polling and interrupt

 *      transactions simutanuously.
 *
 * @param handle Device handle obtained using spi_host_add_dev
 * @param trans_desc Description of transaction to execute
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_OK                on success
 */
esp_err_t ESP32_SPI_device_polling_transmit(ESP32_SPI_device_handle_t handle, ESP32_SPI_transaction_t *trans_desc);


/**
 * @brief Occupy the SPI bus for a device to do continuous transactions.
 *
 * Transactions to all other devices will be put off until ``spi_device_release_bus`` is called.
 *
 * @note The function will wait until all the existing transactions have been sent.
 *
 * @param device The device to occupy the bus.
 * @param wait Time to wait before the the bus is occupied by the device. Currently MUST set to portMAX_DELAY.
 *
 * @return
 *      - ESP_ERR_INVALID_ARG : ``wait`` is not set to portMAX_DELAY.
 *      - ESP_OK : Success.
 */
esp_err_t ESP32_SPI_device_acquire_bus(ESP32_SPI_device_handle_t device, TickType_t wait);

/**
 * @brief Release the SPI bus occupied by the device. All other devices can start sending transactions.
 *
 * @param dev The device to release the bus.

 */
void ESP32_SPI_device_release_bus(ESP32_SPI_device_handle_t dev);


/**
 * @brief Calculate the working frequency that is most close to desired frequency, and also the register value.
 *
 * @param fapb The frequency of apb clock, should be ``APB_CLK_FREQ``.
 * @param hz Desired working frequency
 * @param duty_cycle Duty cycle of the spi clock
 * @param reg_o Output of value to be set in clock register, or NULL if not needed.
 * @return Actual working frequency that most fit.
 */
int ESP32_SPI_cal_clock(int fapb, int hz, int duty_cycle, uint32_t* reg_o);

/**
  * @brief Calculate the timing settings of specified frequency and settings.
  *
  * @param gpio_is_used True if using GPIO matrix, or False if iomux pins are used.
  * @param input_delay_ns Input delay from SCLK launch edge to MISO data valid.
  * @param eff_clk Effective clock frequency (in Hz) from spi_cal_clock.
  * @param dummy_o Address of dummy bits used output. Set to NULL if not needed.
  * @param cycles_remain_o Address of cycles remaining (after dummy bits are used) output.

  *         - -1 If too many cycles remaining, suggest to compensate half a clock.
  *         - 0 If no remaining cycles or dummy bits are not used.
  *         - positive value: cycles suggest to compensate.
  *
  * @note If **dummy_o* is not zero, it means dummy bits should be applied in half duplex mode, and full duplex mode may not work.
  */
void ESP32_SPI_get_timing(bool gpio_is_used, int input_delay_ns, int eff_clk, int* dummy_o, int* cycles_remain_o);

/**
  * @brief Get the frequency limit of current configurations.
  *         SPI master working at this limit is OK, while above the limit, full duplex mode and DMA will not work,
  *         and dummy bits will be aplied in the half duplex mode.
  *
  * @param gpio_is_used True if using GPIO matrix, or False if native pins are used.
  * @param input_delay_ns Input delay from SCLK launch edge to MISO data valid.
  * @return Frequency limit of current configurations.
  */
int ESP32_SPI_get_freq_limit(bool gpio_is_used, int input_delay_ns);






// -------------------------------------------------------------------------------------------------



/*
 * typedefs of ESP32_SPI_Module Function Callbacks
 * This Fn are provided & made accessible for client modules - for operation
 */

// typedef for ESP32_SPI_Module_spi_bus_add_deviceFn - adds an device to the definitions host
typedef strTextMultiple_t*  (*ESP32_SPI_bus_add_deviceFn_t) (ESP32_SPI_Definition_t* ESP32_SPI_Definition, const ESP32_SPI_device_interface_config_t *dev_config, ESP32_SPI_device_handle_t *handle);

// typedef for ESP32_SPI_spi_bus_remove_deviceFn - removes an device from the definitions host
typedef strTextMultiple_t*  (*ESP32_SPI_bus_remove_deviceFn_t) (ESP32_SPI_device_handle_t handle);

// typedef for ESP32_SPI_device_queue_transFn - to queue an interrupt driven transaction
typedef esp_err_t (*ESP32_SPI_device_queue_transFn_t) (ESP32_SPI_device_handle_t handle, ESP32_SPI_transaction_t *trans_desc, TickType_t ticks_to_wait);

// typedef for ESP32_SPI_device_get_trans_resultFn - to get result of an interrupt driven transaction
typedef esp_err_t (*ESP32_SPI_device_get_trans_resultFn_t) (ESP32_SPI_device_handle_t handle, ESP32_SPI_transaction_t **trans_desc, TickType_t ticks_to_wait);

// typedef for ESP32_SPI_device_transmitFn - to send transaction, waits to complete, return result
typedef esp_err_t (*ESP32_SPI_device_transmitFn_t) (ESP32_SPI_device_handle_t handle, ESP32_SPI_transaction_t *trans_desc);

// typedef for ESP32_SPI_device_polling_startFn - to immediately start a polling transaction
typedef esp_err_t (*ESP32_SPI_device_polling_startFn_t) (ESP32_SPI_device_handle_t handle, ESP32_SPI_transaction_t *trans_desc, TickType_t ticks_to_wait);

// typedef for ESP32_SPI_device_polling_endFn - to poll until the transaction ends
typedef esp_err_t (*ESP32_SPI_device_polling_endFn_t) (ESP32_SPI_device_handle_t handle, TickType_t ticks_to_wait);

// typedef for ESP32_SPI_device_polling_transmitFn - send polling transaction, wait, return result
typedef esp_err_t (*ESP32_SPI_device_polling_transmitFn_t) (ESP32_SPI_device_handle_t handle, ESP32_SPI_transaction_t *trans_desc);

// typedef for ESP32_SPI_device_acquire_busFn - to occupy the SPI bus for transactions on a device
typedef esp_err_t (*ESP32_SPI_device_acquire_busFn_t) (ESP32_SPI_device_handle_t device, TickType_t wait);

// typedef for ESP32_SPI_device_release_busFn - to release the occupied SPI bus
typedef void (*ESP32_SPI_device_release_busFn_t) (ESP32_SPI_device_handle_t dev);

// typedef for ESP32_SPI_cal_clockFn - to calculate the working frequency
typedef esp_err_t (*ESP32_SPI_cal_clockFn_t) (int fapb, int hz, int duty_cycle, uint32_t* reg_o);

// typedef for ESP32_SPI_get_timingFn - to calculate the timing settings of specified freq.
typedef void (*ESP32_SPI_get_timingFn_t) (bool gpio_is_used, int input_delay_ns, int eff_clk, int* dummy_o, int* cycles_remain_o);

// typedef for ESP32_SPI_get_freq_limitFn - to get the frequency limit of current configuration
typedef esp_err_t (*ESP32_SPI_get_freq_limitFn_t) (bool gpio_is_used, int input_delay_ns);



/*
 * ESP32 SPI provided Fn - table
 * Stores common + custom functions this Module provides to the SCDE (and client Modules)

 */
typedef struct ESP32_SPI_ProvidedByModule_s {
// --- first the provided common module functions ---
  ProvidedByModule_t common;							// the common fn
// --- now the provided custom module fuctions ---
  ESP32_SPI_bus_add_deviceFn_t ESP32_SPI_bus_add_deviceFn;			// adds an device to the definitions host
  ESP32_SPI_bus_remove_deviceFn_t ESP32_SPI_bus_remove_deviceFn;		// removes an device from the definitions host
  ESP32_SPI_device_queue_transFn_t ESP32_SPI_device_queue_transFn;		// to queue an interrupt driven transaction
  ESP32_SPI_device_get_trans_resultFn_t ESP32_SPI_device_get_trans_resultFn;	// to get result of an interrupt driven transaction
  ESP32_SPI_device_transmitFn_t ESP32_SPI_device_transmitFn;			// to send transaction, waits to complete, return result
  ESP32_SPI_device_polling_startFn_t ESP32_SPI_device_polling_startFn;		// to immediately start a polling transaction
  ESP32_SPI_device_polling_endFn_t ESP32_SPI_device_polling_endFn;		// to poll until the transaction ends
  ESP32_SPI_device_polling_transmitFn_t ESP32_SPI_device_polling_transmitFn;	// send polling transaction, wait, return result
  ESP32_SPI_device_acquire_busFn_t ESP32_SPI_device_acquire_busFn;		// to occupy the SPI bus for transactions on a device
  ESP32_SPI_device_release_busFn_t ESP32_SPI_device_release_busFn;		// to release the occupied SPI bus 
  ESP32_SPI_cal_clockFn_t ESP32_SPI_cal_clockFn;				// to calculate the working frequency
  ESP32_SPI_get_timingFn_t ESP32_SPI_get_timingFn;				// to calculate the timing settings of specified freq.
  ESP32_SPI_get_freq_limitFn_t ESP32_SPI_get_freq_limitFn;			// to get the frequency limit of current configuration
} ESP32_SPI_ProvidedByModule_t;



#endif /*ESP32_SPI_MODULE_S_H*/

