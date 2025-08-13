#include "types.h"
#include "i2cdef.h"
#include "i2c.h"
#include "board.h"
#include "Global.h"
#include "mStar.h"
#include "ms_reg.h"
#include "Ms_rwreg.h"
#include "GPIO_DEF.h"
#include "NVRam.h"
#if !USEFLASH
void NVRam_WriteByte( WORD addr, BYTE value )
{
    Clr_EEPROM_WP(); //wumaozhong 20050331
    i2c_WriteTBL( NVRAM_DEVICE, addr, &value, 1 );
    Set_EEPROM_WP(); //wumaozhong 20050331
}
void NVRam_ReadByte( WORD addr, BYTE *value )
{
    i2c_ReadTBL( NVRAM_DEVICE, addr, value, 1 );
}
void NVRam_ReadWord( WORD addr, WORD *value )
{
    i2c_ReadTBL( NVRAM_DEVICE, addr, (BYTE*)value, 2 );
}

void NVRam_WriteTbl( WORD addr, BYTE *buffer, WORD count )
{
    Clr_EEPROM_WP();
    i2c_WriteTBL( NVRAM_DEVICE, addr, buffer, count );
    Set_EEPROM_WP();
}

void NVRam_ReadTbl( WORD addr, BYTE *buffer, WORD count )
{
    i2c_ReadTBL( NVRAM_DEVICE, addr, buffer, count );
}

#endif
