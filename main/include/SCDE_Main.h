#ifndef _SCDE_MAIN_H_
#define _SCDE_MAIN_H_


//#define MYTRUE  1
//#define MYFALSE 0
//typedef int MYBOOL;




// Flash access and copy from flash
uint8_t SCDE_read_rom_uint8(const uint8_t* addr);
uint16_t SCDE_read_rom_uint16(const uint16_t* addr);
void SCDE_memcpy_plus(char *dst, const char *src, int len);







#endif /*_SCDE_MAIN_H_*/
