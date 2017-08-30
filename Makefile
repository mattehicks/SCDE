#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := SCDE_ESP32

include $(IDF_PATH)/make/project.mk




#
# Build the files for shipping.
#
ship: images #ship_build all
	echo "+---------------+"
	echo "| Building ship |"
	echo "+---------------+"	
	mkdir -p build/ship
#	cp build/esp32-duktape.bin build/ship
#	cp build/partitions_singleapp.bin build/ship
	cp build/bootloader/bootloader.bin build/ship
	cp build/spiffs.img build/ship
	cp build/espfs.img build/ship
#	cp data/install_binaries.sh build/ship
#	cd build/ship; tar -cvzf ../../esp32-duktape-$(shell date "+%Y-%m-%d").tar.gz .
	echo "+---------+"
	echo "| Results |"
	echo "+---------+"	
	echo "Created output: SCDE_ESP32 - $(shell date "+%Y-%m-%d").tar.gz"




# Build the ESP-FS Imagefile.
ESPFSImage:
	echo "+--------------------+"
	echo "| Building espfs.img |"
	echo "+--------------------+"
	cd filesystem; find . -print | ../bin/mkespfsimage -c 0 > ../build/espfs.img



# Build the SPI Flash File System Imagefile.
SPIFFSImage:
	echo "+---------------------+"
	echo "| Building spiffs.img, 512KB max size |"
	echo "+---------------------+"
#	./bin/mkspiffs -c filesystem -b 65536 -p 256 -s 524288 build/spiffs.img
	./bin/mkspiffs -c filesystem -b 4096 -p 256 -s 524288 build/spiffs.img



#
#  Perform flashing of both ESPFS and SPIFFS to ESP32
#
flashdata: ESPFSImage SPIFFSImage
	echo "Flashing both ESPFS and SPIFFS to ESP32"
	$(ESPTOOLPY_WRITE_FLASH) --compress 0x300000 build/espfs.img 0x180000 build/spiffs.img


#
#  Perform flashing of both ESPFS and SPIFFS to ESP32
#
flashdatax: SPIFFSImage
	echo "Flashing SPIFFS to ESP32"
#	$(ESPTOOLPY_WRITE_FLASH) --compress 0x180000 build/spiffs.img
	$(ESPTOOLPY_WRITE_FLASH) --compress 0x190000 build/spiffs.img



#
#  Build all, flash app & flash both ESPFS and SPIFFS to ESP32
#
flashall: flash flashdata


#
#  Help Text
#
what:
	echo "duktape_configure - Configure Duktape."
	echo "duktape_install   - Install latest Duktape."
	echo "flash             - Flash the ESP32 application."
	echo "flashall          - Flash the ESP32 application and file systems data."
	echo "flashdata         - Flash the file systems data."



