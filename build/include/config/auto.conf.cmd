deps_config := \
	/home/maikschulze/esp/esp-idf/components/bt/Kconfig \
	/home/maikschulze/esp/esp-idf/components/esp32/Kconfig \
	/home/maikschulze/esp/esp-idf/components/espfs/Kconfig \
	/home/maikschulze/esp/esp-idf/components/ethernet/Kconfig \
	/home/maikschulze/esp/esp-idf/components/freertos/Kconfig \
	/home/maikschulze/esp/esp-idf/components/log/Kconfig \
	/home/maikschulze/esp/esp-idf/components/lwip/Kconfig \
	/home/maikschulze/esp/esp-idf/components/mbedtls/Kconfig \
	/home/maikschulze/esp/esp-idf/components/spi_flash/Kconfig \
	/home/maikschulze/esp/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/maikschulze/esp/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/maikschulze/esp/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/maikschulze/esp/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
