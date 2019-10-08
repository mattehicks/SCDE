deps_config := \
	/home/maikschulze/ESP32/esp-idf/components/app_trace/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/aws_iot/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/bt/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/driver/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/efuse/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/esp32/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/esp_adc_cal/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/esp_event/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/esp_http_client/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/esp_http_server/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/esp_https_ota/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/espcoredump/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/ethernet/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/fatfs/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/freemodbus/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/freertos/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/heap/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/libsodium/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/log/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/lwip/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/mbedtls/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/mdns/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/mqtt/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/nvs_flash/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/openssl/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/pthread/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/spi_flash/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/spiffs/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/tcpip_adapter/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/unity/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/vfs/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/wear_levelling/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/wifi_provisioning/Kconfig \
	/home/maikschulze/ESP32/esp-idf/components/app_update/Kconfig.projbuild \
	/home/maikschulze/ESP32/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/maikschulze/ESP32/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/maikschulze/ESP32/SCDE/main/Kconfig.projbuild \
	/home/maikschulze/ESP32/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/maikschulze/ESP32/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(IDF_TARGET)" "esp32"
include/config/auto.conf: FORCE
endif
ifneq "$(IDF_CMAKE)" "n"
include/config/auto.conf: FORCE
endif

$(deps_config): ;
