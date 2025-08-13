#if  CHIP_ID==CHIP_MT9700
#include "Panel.h"

#ifndef     _MSFB_H
#define     _MSFB_H


#ifdef _MS_FB_C
#define INTERFACE
#else
#define INTERFACE   extern
#endif

#define ALIGN_4K(_x_)           (((_x_) + 4095) & ~4095)

/* Xdata Allocation - total 16KB in CHIP_MT9700 (0x4000 - 0x7FFF)
    -------------------
    |      Start      | 0x4000
    |      SWDB       | 0x4100
    |    DDCBuffer    | 0x4200
    |       END       | 0x7FFF
    -------------------
*/
#define XDATA_SIZE              0x4000
#define XDATA_ADDR_START        0x4000 // keep 0x4000.
#define XDATA_SWDB_ADDR_START   0x4100 // SWDB - allocate maximum 256 Bytes >=  sizeof(StuSWDBCtrl)
//#define XDATA_DDC_ADDR_START    0x4200 // DDC max = 256B*5=0x500, 0X4C00-0x0500=0x4700, In current driver code, maximum 256 Bytes needed.
//#define XDATA_DDC_TX_ADDR_START XDATA_DDC_ADDR_START + D2B_FIFO_XdataSize
#define XDATA_ADDR_END          0x7FFF

/* SPI BIN Allocation - HK51+SECUR2+PD51
    -------------------
    |      HK51       | 0x00-0000 (13 Banks) - Customized according to Keilc project.
    |     SECUR2      | 0x0D-0000 (Fixed 1 Bank)
    |      PD51       | 0x0E-0000 (Fixed 2 Bank)(option)
    |      END        | 0x10-0000
    -------------------
*/
#define HK51_SPI_BIN_START     (0x000000UL)
#define HK51_SPI_BIN_SIZE      (0x0D0000UL) // 13 Banks . Customized according to Keilc project.
#define SECUR2_SPI_BIN_START   (HK51_SPI_BIN_START+HK51_SPI_BIN_SIZE)
#define SECUR2_SPI_BIN_SIZE    (0x010000UL) // Fixed 1 Bank, 64KB
#define PD51_SPI_BIN_START     (SECUR2_SPI_BIN_START+SECUR2_SPI_BIN_SIZE)
#define PD51_SPI_BIN_SIZE      (0x020000UL) // Fixed 2 Bank, 128KB
#define TOTAL_BIN_SIZE         (PD51_SPI_BIN_START+PD51_SPI_BIN_SIZE-HK51_SPI_BIN_START)


/*****************************************/
/*     DDR parameter                     */
/*****************************************/
#define DRAM_SIZE           (0x800000UL)

#define ENABLE_DDR_SSC      0

#define DDR_SSC_MODULATION_DEF      30 // unit: 1KHz, range 0~30 means 0~30KHz
#define DDR_SSC_PERCENTAGE_DEF      10 //unit: 0.1%, range 0~10 means 0~1%
#define DDR_SSC_MODULATION_MAX      30 // unit: 1KHz, range 0~30 means 0~30KHz
#define DDR_SSC_PERCENTAGE_MAX      10 // unit: 0.1%, range 0~10 means 0~1%
#if (ENABLE_DDR_SSC)
#define DDR_SSC_DEVIATION           DDR_SSC_PERCENTAGE_DEF
#else
#define DDR_SSC_DEVIATION           0
#endif

#define BIST_BASE_UNIT      (0x4000)
#define MIU_UNIT            (0x20)

/*PSRAM(16KB) supports common bank only*/
#if (ENABLE_HK_CODE_ON_PSRAM)
#define HK_PSRAM_ADDR                (0UL+0x10000UL*g_u8DIBankOffset)
#define HK_PSRAM_SIZE                (0x00004000UL)
#define MCU_ON_PSRAM_START_ADDR      (0x00000000UL)
#define MCU_ON_PSRAM_END_ADDR        (0x00003FFFUL)
// For DMA load
#define DMA_TO_PSRAM_SOURCE_ADDR     HK_PSRAM_ADDR              // flash start addr
#define DMA_TO_PSRAM_DESTIN_ADDR     MCU_ON_PSRAM_START_ADDR    // psram start addr
#define DMA_TO_PSRAM_BYTE_COUNT      HK_PSRAM_SIZE              // flash to psram size
#endif


/*****************************************/
/*     Memory Allocation                 */
/*****************************************/
/* Memory Allocation
    -------------------
    |  HK CODE        |
    |  HK DATA        |
    |  MENULOAD       |
    |  AUTOLOAD       |
    |  OD             |
    |  FB             |
    -------------------
*/

/* HK Code, allocate in 64KB unit */
#define HK_CODE_ADDR                (0UL)
#if (ENABLE_HK_CODE_ON_DRAM)
#define HK_CODE_SIZE                (0xD0000)       // dram reserved size 13 bank
// For MCU read, need to align with msfb.h, MCU_CODE_SIZE
#define MCU_ON_DRAM_START_ADDR      (0x00000000UL)
#define MCU_ON_DRAM_END_ADDR        (0x0006FFFFUL)
#define MCU_ON_SPI_START_ADDR       (0x00070000UL)  /* bank 7 on SPI for flash data read */
#define MCU_ON_SPI_END_ADDR         (0x000CFFFFUL)
// For DMA load
#define DMA_TO_DRAM_SOURCE_ADDR     MCU_ON_DRAM_START_ADDR      // flash start addr
#define DMA_TO_DRAM_DESTIN_ADDR     HK_CODE_ADDR                // dram start addr
#define DMA_TO_DRAM_BYTE_COUNT      HK_CODE_SIZE                // flash to dram size
#else
#define HK_CODE_SIZE                (0UL)
#endif

/* HK XDATA, allocate in 64KB boundary */
#if ENABLE_HK_XDATA_ON_DRAM
#define HK_XDATA_SIZE               (0x10000)   // 64 k
#else
#define HK_XDATA_SIZE               (0UL)
#endif

#if ENABLE_MENULOAD
#if ENABLE_MENULOAD_2
#define MIU_MENULOAD_SIZE           (0x400)
#define MIU_MENULOAD_2_SIZE         (0x400)
#else
#define MIU_MENULOAD_SIZE           (0x800)
#define MIU_MENULOAD_2_SIZE         (0UL)
#endif
#else
#define MIU_MENULOAD_SIZE           (0UL)
#define MIU_MENULOAD_2_SIZE         (0UL)
#endif

#if ENABLE_AUTOLOAD
#define MIU_AUTOLOAD_SIZE           0x10000//(0x5000)
#else
#define MIU_AUTOLOAD_SIZE           (0UL)
#endif

#define MIU_OD_SIZE                 (0UL) // use IMI

#if ENABLE_ENG_USB_MODE
#define MIU_USBBUF_SIZE              0x1000 // 4KB
#else
#define MIU_USBBUF_SIZE              (0UL)
#endif

#define HK_XDATA_ADDR_START         (HK_CODE_ADDR + HK_CODE_SIZE)
#define MIU_MENULOAD_ADDR_START     ALIGN_4K(HK_XDATA_ADDR_START + HK_XDATA_SIZE) // needs 4K alignment for using mcu4kXdataMapToDRAMIMI()
#define MIU_MENULOAD_2_ADDR_START   ALIGN_4K(MIU_MENULOAD_ADDR_START + MIU_MENULOAD_SIZE) // needs 4K alignment for using mcu4kXdataMapToDRAMIMI()
#define MIU_AUTOLOAD_ADDR_START     ALIGN_4K(MIU_MENULOAD_2_ADDR_START + MIU_MENULOAD_2_SIZE) // needs 4K alignment for using mcu4kXdataMapToDRAMIMI()
#define MIU_USBBUF_ADDR_START       ALIGN_4K(MIU_AUTOLOAD_ADDR_START + MIU_AUTOLOAD_SIZE) // needs 4K alignment for using mcu4kXdataMapToDRAMIMI()
#define MIU_OD_ADDR_START           (MIU_USBBUF_ADDR_START + MIU_USBBUF_SIZE)
#define MIU_OD_ADDR_END             (MIU_OD_ADDR_START + MIU_OD_SIZE)
#define MIU_FB_ADDR_START           MIU_OD_ADDR_END
#define MIU_FB_ADDR_END             DRAM_SIZE

enum
{
    MIU_PROTECT_0 = BIT0,
    MIU_PROTECT_1 = BIT1,
    MIU_PROTECT_2 = BIT2,
    MIU_PROTECT_3 = BIT3,
};

enum
{
    MIU_ID_CMD      = 0x00,
    MIU_ID_HAYDN    = 0x01,
    MIU_ID_SECU_R2  = 0x03,
    MIU_ID_AESDMA   = 0x04,
    MIU_ID_USB      = 0x05,
    MIU_ID_PM_51    = 0x06,
    MIU_ID_BDMA     = 0x0E,

    MIU_ID_MRW_W    = 0x20,
    MIU_ID_MRW_R    = 0x21,
    MIU_ID_OD_W     = 0x22,
    MIU_ID_OD_R     = 0x23,
    MIU_ID_ADL      = 0x24,
    MIU_ID_ML       = 0x25,
    MIU_ID_DMA      = 0x26,
};

///////////////////////////////////////////////////////////////////////////
// Subroutines
///////////////////////////////////////////////////////////////////////////
INTERFACE void msInitMemory(void);
INTERFACE Bool msMemoryBist(void);
INTERFACE void msMiuProtectCtrl(BYTE ucGroup, BOOL bCtrl, WORD ucID, DWORD wAddrStart, DWORD wAddrEnd);
#undef INTERFACE

#endif

#endif

