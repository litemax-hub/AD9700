#ifndef _MSPI_FLASH_H_
#define _MSPI_FLASH_H_

#ifdef _MSPI_FLASH_C_
    #define _MSPI_FLASH_DEC_
#else
    #define _MSPI_FLASH_DEC_  extern
#endif

#define BIT(_bit_)                  (1 << (_bit_))
#define BIT_(x)                     BIT(x) //[OBSOLETED] //TODO: remove it later
#define BITS(_bits_, _val_)         ((BIT(((1)?_bits_)+1)-BIT(((0)?_bits_))) & (_val_<<((0)?_bits_)))
#define BMASK(_bits_)               (BIT(((1)?_bits_)+1)-BIT(((0)?_bits_)))

#define MSPI_FLASH_WBF_MAX_SIZE				8
#define MSPI_FLASH_RBF_MAX_SIZE				8
#define MSPI_FLASH_PAGE_LENGTH				256

#define MSPI_POLLING_BUSY_FLAG_TIMEOUT 			100		// ms 
#define MSPI_POLLING_TIMEOUT_PAGE_PROGRAME 		1000 	// ms
#define MSPI_POLLING_TIMEOUT_ERASE 				20100 	// ms

// MSPI control register bits 
#define MSPI_ENABLE							BIT0
#define MSPI_NOT_RESET						BIT1
#define MSPI_INTERRUPT_ENABLE				BIT2
#define MSPI_3WIRE_MODE					BIT4
#define MSPI_SAMPLE_AT_TRAILING_EDGE		BIT6	// clock phase
#define MSPI_SCK_IDLE_HIGH					BIT7	// clock polarity

typedef enum
{
	MSPI0,
} EN_MSPI;

typedef enum
{
	MSPI_SLAVE0,
	MSPI_SLAVE1,	
	MSPI_SLAVE2,
	MSPI_SLAVE3,	
	MSPI_SLAVE4,
	MSPI_SLAVE5,	
	MSPI_SLAVE6,	
	MSPI_SLAVE7,	
} EN_MSPI_SLAVE;

typedef enum
{
    MSPI_CKG_27M,
    MSPI_CKG_54M,
    MSPI_CKG_62M,
    MSPI_CKG_72M,
    MSPI_CKG_86M,
    MSPI_CKG_108M,
    MSPI_CKG_123M,
    MSPI_CKG_160M,
    MSPI_CKG_XTAL
} EN_MSPI_CLOCK_GEN;

typedef enum 
{ 
	MSPI_CLK_DIV_2, 
	MSPI_CLK_DIV_4, 
	MSPI_CLK_DIV_8, 
	MSPI_CLK_DIV_16, 
	MSPI_CLK_DIV_32, 
	MSPI_CLK_DIV_64, 
	MSPI_CLK_DIV_128, 
	MSPI_CLK_DIV_256 
} EN_MSPI_CLOCK_DIV;

typedef enum { 
	MSPI_TR_STR_DELAY_1_CYCLE, 
	MSPI_TR_STR_DELAY_2_CYCLE, 
	MSPI_TR_STR_DELAY_16_CYCLE = 0x0F, 
	MSPI_TR_STR_DELAY_256_CYCLE = 0xFF 
} EN_MSPI_TR_STR_DELAY;

typedef enum { 
	MSPI_TR_END_DELAY_1_CYCLE, 
	MSPI_TR_END_DELAY_2_CYCLE, 
	MSPI_TR_END_DELAY_16_CYCLE = 0x0F, 
	MSPI_TR_END_DELAY_256_CYCLE = 0xFF 
} EN_MSPI_TR_END_DELAY;

typedef enum { 
	MSPI_B_TO_B_NO_DELAY, 
	MSPI_B_TO_B_DELAY_1_CYCLE, 
	MSPI_B_TO_B_DELAY_15_CYCLE = 0x0F, 
	MSPI_B_TO_B_DELAY_255_CYCLE = 0xFF 
} EN_MSPI_B_TO_B_DELAY;

typedef enum { 
	MSPI_W_TO_R_NO_DELAY, 
	MSPI_W_TO_R_DELAY_1_CYCLE, 
	MSPI_W_TO_R_DELAY_15_CYCLE = 0x0F, 
	MSPI_W_TO_R_DELAY_255_CYCLE = 0xFF 
} EN_MSPI_W_TO_R_DELAY;

typedef struct
{
    BYTE u8DataByteLength;    // 0~8
    BYTE au8DataBuffer[8];    
} ST_MSPI_TRANSFER_DATA;

// please refer to the serial flash datasheet
#define SPI_CMD_WREN                BITS(7:0, 0x06)	// write enable
#define SPI_CMD_WRDI                BITS(7:0, 0x04)	// write disable
#define SPI_CMD_RDSR            	BITS(7:0, 0x05)	// read status register-1
#define SPI_CMD_RDSR2           	BITS(7:0, 0x35) // read status register-2, support for new WinBond Flash
#define SPI_CMD_WRSR            	BITS(7:0, 0x01) // write status register
#define SPI_CMD_PP                  BITS(7:0, 0x02)	// page program
#define SPI_CMD_QPP                 BITS(7:0, 0x02)	// quad page program
#define SPI_CMD_READ                BITS(7:0, 0x03)	// read date
#define SPI_CMD_FASTREAD            BITS(7:0, 0x0B)	// fast read
#define SPI_CMD_RDID                BITS(7:0, 0x9F)	// read manufacturer ID and 2-byte device ID
#define SPI_CMD_SE                  BITS(7:0, 0x20)	// sector erase
#define SPI_CMD_32BE                BITS(7:0, 0x52)	// 32KB block erase
#define SPI_CMD_64BE                BITS(7:0, 0xD8) // 64KB block erase
#define SPI_CMD_CE                  BITS(7:0, 0xC7)	// chip erase

_MSPI_FLASH_DEC_ void MDrv_MSPI_Flash_Init(EN_MSPI, BYTE, EN_MSPI_CLOCK_DIV, EN_MSPI_TR_STR_DELAY, EN_MSPI_TR_END_DELAY, EN_MSPI_B_TO_B_DELAY, EN_MSPI_W_TO_R_DELAY);
_MSPI_FLASH_DEC_ void MDrv_MSPI_Flash_Write_Read_Once(EN_MSPI, EN_MSPI_SLAVE, ST_MSPI_TRANSFER_DATA*, ST_MSPI_TRANSFER_DATA*);
_MSPI_FLASH_DEC_ BYTE  MDrv_MSPI_Flash_Read_Status_Register(EN_MSPI, EN_MSPI_SLAVE);
_MSPI_FLASH_DEC_ BYTE  MDrv_MSPI_Flash_Read_Status_Register2(EN_MSPI, EN_MSPI_SLAVE);
_MSPI_FLASH_DEC_ void MDrv_MSPI_Flash_Write_Status_Register(EN_MSPI, EN_MSPI_SLAVE, BYTE);
_MSPI_FLASH_DEC_ void MDrv_MSPI_Flash_Write_Enable(EN_MSPI, EN_MSPI_SLAVE);
_MSPI_FLASH_DEC_ void MDrv_MSPI_Flash_Write_Disable(EN_MSPI, EN_MSPI_SLAVE);
_MSPI_FLASH_DEC_ DWORD  MDrv_MSPI_Flash_Read_ID(EN_MSPI, EN_MSPI_SLAVE);
_MSPI_FLASH_DEC_ void MDrv_MSPI_Flash_Sector_Erase(EN_MSPI, EN_MSPI_SLAVE, DWORD);
_MSPI_FLASH_DEC_ void MDrv_MSPI_Flash_32KB_Block_Erase(EN_MSPI, EN_MSPI_SLAVE, DWORD);
_MSPI_FLASH_DEC_ void MDrv_MSPI_Flash_64KB_Block_Erase(EN_MSPI, EN_MSPI_SLAVE, DWORD);
_MSPI_FLASH_DEC_ void MDrv_MSPI_Flash_Chip_Erase(EN_MSPI, EN_MSPI_SLAVE);
_MSPI_FLASH_DEC_ BOOL MDrv_MSPI_Flash_Erase_All(EN_MSPI, EN_MSPI_SLAVE);
_MSPI_FLASH_DEC_ BOOL MDrv_MSPI_Flash_Page_Program(EN_MSPI, EN_MSPI_SLAVE, DWORD, DWORD, const BYTE *);
_MSPI_FLASH_DEC_ BOOL MDrv_MSPI_Flash_Write(EN_MSPI, EN_MSPI_SLAVE, DWORD, DWORD, const BYTE *);
_MSPI_FLASH_DEC_ BOOL MDrv_MSPI_Flash_Read(EN_MSPI, EN_MSPI_SLAVE, DWORD, DWORD, BYTE *);

#endif	/* _MSPI_FLASH_H_ */

