#include "i2c.h"
#define NVRAM_DEVICE    0xA0
extern void NVRam_WriteByte( WORD addr, BYTE value );
extern void NVRam_ReadByte( WORD addr, BYTE *value );
extern void NVRam_ReadWord( WORD addr, WORD *value );
extern void NVRam_WriteTbl( WORD addr, BYTE *buffer, WORD count );
extern void NVRam_ReadTbl( WORD addr, BYTE *buffer, WORD count );

