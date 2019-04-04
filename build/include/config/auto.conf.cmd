deps_config := \
	/home/maikschulze/esp/esp-idf/components/app_trace/Kconfig \
	/home/maikschulze/esp/esp-idf/components/aws_iot/Kconfig \
	/home/maikschulze/esp/esp-idf/components/bt/Kconfig \
	/home/maikschulze/esp/esp-idf/components/driver/Kconfig \
	/home/maikschulze/esp/esp-idf/components/efuse/Kconfig \
	/home/maikschulze/esp/esp-idf/components/esp32/Kconfig \
	/home/maikschulze/esp/esp-idf/components/esp_adc_cal/Kconfig \
	/home/maikschulze/esp/esp-idf/components/esp_event/Kconfig \
	/home/maikschulze/esp/esp-idf/components/esp_http_client/Kconfig \
	/home/maikschulze/esp/esp-idf/components/esp_http_server/Kconfig \
	/home/maikschulze/esp/esp-idf/components/esp_https_ota/Kconfig \
	/home/maikschulze/esp/esp-idf/components/espcoredump/Kconfig \
	/home/maikschulze/esp/esp-idf/components/ethernet/Kconfig \
	/home/maikschulze/esp/esp-idf/components/fatfs/Kconfig \
	/home/maikschulze/esp/esp-idf/components/freemodbus/Kconfig \
	/home/maikschulze/esp/esp-idf/components/freertos/Kconfig \
	/home/maikschulze/esp/esp-idf/components/heap/Kconfig \
	/home/maikschulze/esp/esp-idf/components/libsodium/Kconfig \
	/home/maikschulze/esp/esp-idf/components/log/Kconfig \
	/home/maikschulze/esp/esp-idf/components/lwip/Kconfig \
	/home/maikschulze/esp/esp-idf/components/mbedtls/Kconfig \
	/home/maikschulze/esp/esp-idf/components/mdns/Kconfig \
	/home/maikschulze/esp/esp-idf/components/mqtt/Kconfig \
	/home/maikschulze/esp/esp-idf/components/nvs_flash/Kconfig \
	/home/maikschulze/esp/esp-idf/components/openssl/Kconfig \
	/home/maikschulze/esp/esp-idf/components/pthread/Kconfig \
	/home/maikschulze/esp/esp-idf/components/spi_flash/Kconfig \
	/home/maikschulze/esp/esp-idf/components/spiffs/Kconfig \
	/home/maikschulze/esp/esp-idf/components/tcpip_adapter/Kconfig \
	/home/maikschulze/esp/esp-idf/components/unity/Kconfig \
	/home/maikschulze/esp/esp-idf/components/vfs/Kconfig \
	/home/maikschulze/esp/esp-idf/components/wear_levelling/Kconfig \
	/home/maikschulze/esp/esp-idf/components/app_update/Kconfig.projbuild \
	/home/maikschulze/esp/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/maikschulze/esp/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/maikschulze/esp/SCDE/main/Kconfig.projbuild \
	/home/maikschulze/esp/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/maikschulze/esp/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(IDF_TARGET)" "esp32"
include/config/auto.conf: FORCE
endif
ifneq "$(IDF_CMAKE)" "n"
include/config/auto.conf: FORCE
endif

$(deps_config): ;
