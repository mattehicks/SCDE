# SCDE
Smart Connected Device Engine - Generation 2


Commands
Add
Attr
Define
Delete
Deleteattr
Help
Include
List
Set
Shutdown
Sub


Embedded Modules
Global
Telnet

Modules
ESP32_BH1750
ESP32_Control
ESP32_I2C_Master
ESP32_S0
ESP32_SwITCH



make menuconfig

Make and flash the example.

make all && make flash


Prepare SPIFFS image

It is not required to prepare the image, as the spiffs will be automatically formated on first use, but it might be convenient.

SFPIFFS image can be prepared on host and flashed to ESP32.

NEW: Tested and works under Linux, Windows and Mac OS

Copy the files to be included on spiffs into components/spiffs_image/image/ directory. Subdirectories can also be added.

Execute:

make makefs

to create spiffs image in build directory without flashing to ESP32

Execute:

make flashfs

to create spiffs image in build directory and flash it to ESP32
