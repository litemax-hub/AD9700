#define EEPROM_24C16    1
#define EEPROM_24C32    2
#define EEPROM_24C64    3
#define EEPROM_24C128   4
#define EEPROM_24C256   5

#define EEPROM_TYPE EEPROM_24C256

extern void i2c_WriteTBL( BYTE deviceID, WORD addr, BYTE *buffer, WORD count );
extern void i2c_ReadTBL( BYTE deviceID, WORD addr, BYTE *buffer, WORD count );

