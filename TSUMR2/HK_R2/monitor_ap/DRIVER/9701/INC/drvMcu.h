
#ifndef _DRVMCU_H_
#define _DRVMCU_H_

#ifdef _DRVMCU_C_
#define _DRVMCUDEC_
#else
#define _DRVMCUDEC_    extern
#endif

#define _SMOD       1
#define S0REL       (1024-((_SMOD*CRYSTAL_CLOCK+CRYSTAL_CLOCK)/SERIAL_BAUD_RATE)/64)
#define S1REL       (1024-(                      CRYSTAL_CLOCK/SERIAL_BAUD_RATE)/32)

#define    CLOCK_500MHZ         500000000ul
#define    CLOCK_345MHZ         345000000ul
#define    CLOCK_288MHZ         288000000ul
#define    CLOCK_216MHZ         216000000ul
#define    CLOCK_172MHZ         172000000ul
#define    CLOCK_144MHZ         144000000ul
#define    CLOCK_108MHZ         108000000ul
#define    CLOCK_24MHZ          24000000ul
#define    CLOCK_12MHZ          12000000ul
#define    CLOCK_XTAL           CRYSTAL_CLOCK
#define    CLOCK_FRO_12MHZ      CLOCK_12MHZ
#define    CLOCK_4MHZ           4000000ul
#define    CLOCK_R2_SLOW_CLK    24000000ul


//mapping to g_mcuPLLFreqTable[]
#define    IDX_MCU_CLK_500MHZ    0
#define    IDX_MCU_CLK_345MHZ    1
#define    IDX_MCU_CLK_288MHZ    2
#define    IDX_MCU_CLK_216MHZ    3
#define    IDX_MCU_CLK_172MHZ    4
#define    IDX_MCU_CLK_144MHZ    5
#define    IDX_MCU_R2_SLOW_CLK   6
#define    IDX_MCU_CLK_12MHZ     7
#define    IDX_MCU_CLK_XTAL      8
#define    IDX_MCU_CLK_108MHZ    9


/*
#ifndef MCU_SPEED_INDEX
#error "Please define MCU_SPEED_INDEX!!!"
#endif
//Jison, used for decide the cpu clock in compile time
#if MCU_SPEED_INDEX==IDX_MCU_CLK_RESERVED0
#error "Wrong MCU_SPEED_INDEX define!!!"
#endif

#if MCU_SPEED_INDEX==IDX_MCU_CLK_144MHZ||MCU_SPEED_INDEX==IDX_MCU_CLK_216MHZ
#error "Wrong MCU_SPEED_INDEX define!!!"
#endif
*/

// SPI Clock Selection item define
#define    IDX_SPI_CLK_4MHZ     0
#define    IDX_SPI_CLK_12MHZ    1
#define    IDX_SPI_CLK_24MHZ    2 // CLK not support in SPI_MODE_SADD/SPI_MODE_DADD/SPI_MODE_SAQD/SPI_MODE_QAQD
#define    IDX_SPI_CLK_36MHZ    3 // CLK not support in SPI_MODE_SADD/SPI_MODE_DADD/SPI_MODE_SAQD/SPI_MODE_QAQD
#define    IDX_SPI_CLK_43MHZ    4
#define    IDX_SPI_CLK_54MHZ    5
#define    IDX_SPI_CLK_86MHZ    6 // CLK not support in SPI_MODE_SADD/SPI_MODE_DADD/SPI_MODE_SAQD/SPI_MODE_QAQD
#define    IDX_SPI_CLK_SSC      7 // CLK not support in SPI_MODE_SADD/SPI_MODE_DADD/SPI_MODE_SAQD/SPI_MODE_QAQD
#define    IDX_SPI_CLK_XTAL     8


#define    SPI_MODE_NORMAL      0
#define    SPI_MODE_FR          1
#define    SPI_MODE_SADD        2
#define    SPI_MODE_DADD        3
#define    SPI_MODE_SAQD        4
#define    SPI_MODE_QAQD        5


#define EnableExt0Interrupt     1 // nonPM IFQ
#define EnableExt1Interrupt     1 // pm IRQ, [0]nonPM IRQ
#define EnableExt2Interrupt     EXT_TIMER0_1MS
#define EnableExt3Interrupt     ENABLE_WATCH_DOG_INT
#define EnableExt4Interrupt     1 // pm FIQ
#define EnableTime0Interrupt    (!EXT_TIMER0_1MS)


#define WIN0_ADDR_START             (0x8000>>10)  // map to xdata 0x8000~0xEFFF
#define WIN0_ADDR_END               (0xF000>>10)
#define WIN1_ADDR_START             WIN0_ADDR_END // map to xdata 0xF000~0xFFFF
#define WIN1_ADDR_END               (0x10000>>10)

#if 0
////////////////////////////////
///// Struct MUST Sync with 51 /////
typedef struct
{ //MUST CARE 32 bits align & endian (WORD only)
    //////////////////////
    //MSTAR DRIVER AREA//
    WORD    u16PMMode;
    //MSTAR DRIVER AREA//
    //////////////////////

    //////////////////////
    /// CUSTOMER AREA ///
    WORD    u16PMPortIndex;
    WORD    u16PMPortSkipIndex;
    /// CUSTOMER AREA ///
    //////////////////////
}/*__attribute__((packed))*/sMAILBOX_R2;

typedef struct
{ //MUST CARE 32 bits align & endian (WORD only)
    //////////////////////
    //MSTAR DRIVER AREA//
    WORD    u16PMWakeupStatus;
    //MSTAR DRIVER AREA//
    //////////////////////

    //////////////////////
    /// CUSTOMER AREA ///
    WORD    u8PMActivePort;
    /// CUSTOMER AREA ///
    //////////////////////
}/*__attribute__((packed))*/sMAILBOX_51;

_DRVMCUDEC_ XDATA sMAILBOX_R2 g_sMailBoxR2;
_DRVMCUDEC_ XDATA sMAILBOX_51 g_sMailBox51;
#endif

// use reserved 16th semaphore to record.
#define NUM_MAILBOX         0x60
#define IsMailBoxValid()  (msReadByte(REG_10181E)==0x02) // 1:HK51, 2:PD51
#define SetMailBoxValid() (msWriteByte(REG_10181E, 0x00),msWriteByte(REG_10181E, 0x01)) // 1:HK51, 2:PD51
// use reserved 15th semaphore to record.
#define InitWDTAck()       (msWriteByte(REG_MBX_WDT_ACK, 0x02)) // 1:HK51, 2:PD51
#define IsWDTAck()        (msReadByte(REG_MBX_WDT_ACK)==0x02) // 1:HK51, 2:PD51
#define SetWDTAck()       (msWriteByte(REG_MBX_WDT_ACK, 0x01)) // 1:HK51, 2:PD51
// use reserved 14th semaphore to record.
#define IsUSBDL()        (msReadByte(REG_10181A)==0x01) // 1:Enable USB download
#define SetUSBDL()       (msWriteByte(REG_10181A, 0x00),msWriteByte(REG_10181A, 0x01))

#if ENABLE_DP_INPUT
    #if (SPI_SPEED_INDEX < IDX_SPI_CLK_54MHZ) || (SPI_SPEED_INDEX >= IDX_SPI_CLK_SSC)
        #error "--------------------default SPI_SPEED_INDEX need to be set to IDX_SPI_CLK_54MHZ or higher--------------------"
    #endif
    #if SPI_MODE < SPI_MODE_SADD
        #error "--------------------default SPI_MODE need to be set to SPI_MODE_DADD or higher--------------------"
    #endif
#endif


enum
{
    CLK_LIVE_XTAL,
    CLK_LIVE_RCOSC_4M
};

typedef enum
{
    SPEED_12MHZ_MODE,
    SPEED_XTAL_MODE,
    SPEED_SLOWCLK_MODE,
    SPEED_NORMAL_MODE,
} SystemSpeedMode;

typedef enum
{
    UART_GPIOX03_04,
    UART_GPIOX13_14,
    UART_GPIO00_02 = 3,
    UART_GPIO50_51,
    UART_GPIO40_41,
    UART_GPIO_RESERVED
}UART_GPIO;

typedef enum // UART Engine
{
    UART_ENGINE_R2_HK,
    UART_ENGINE_R2_SECU,
    UART_ENGINE_PD51_UART0,
    UART_ENGINE_PD51_UART1,
    UART_ENGINE_DW_UART,
    UART_ENGINE_R2_PQ,
    RESERVED
}UART_ENGINE;

typedef enum
{
    _HK51_SPI,
    _HK51_DRAM,
    _HK51_PSRAM,
    _PD51_PSRAM,
} _CodeType;

extern void Init_WDT( BYTE bEnable );
extern void mcuSetUartMux( UART_ENGINE ucUartEngine, UART_GPIO ucUartGPIO );
extern void mcuSetSpiSpeed( BYTE ucIndex );
extern void mcuSetSpiMode( BYTE ucMode );
extern BYTE mcuGetSpiMode(void);
extern void mcuSetMcuSpeed( BYTE ucSpeedIdx );
extern DWORD mcuGetMcuSpeed(void);
extern void mcuSetSystemSpeed(BYTE u8Mode );
extern void SetSPI_Quad_En(BYTE ucEnable);

extern DWORD mcuDMACRC32(BYTE channel,BYTE SourceType,DWORD dwSourceAddr, DWORD dwByteCount, DWORD dwPolynimial, DWORD dwSeed);
extern void mcuDMADownloadCode(BYTE SourceType,BYTE DestinType,DWORD dwSourceAddr,DWORD dwDestinAddr,DWORD dwByteCount);
extern void mcuArrangeCodeAddr(void);
extern void mcuInitXdataMapToDRAM(void);
extern void mcu40kXdataMapToDRAMIMI(DWORD dwADDR, BYTE ucDestSel);
extern void mcu4kXdataMapToDRAMIMI(DWORD dwADDR, BYTE ucDestSel);
extern void ClearWDT( void );
extern void SetWDTClk( BYTE clk_live_sel );
extern void mcuSetSlowClkSpeed(BYTE u8SlowClkDiv, BYTE u8SlowClkDivPD51);
extern void mcuSetCPUClock(DWORD u32CpuClock, BYTE ucSpeedIdx);
extern void DelayUs(DWORD u32Delayus);
extern void msDrvSetExtTimerCnt(DWORD u32UsCnt);
extern void msDrvEnableExtTimer(BOOL bEnable);

#if ENABLE_HK_CODE_ON_PSRAM
extern void msDrvMcuSetCodeRangeOnPSram(DWORD dwAddrStart, DWORD dwAddrEnd);
#endif
#if ENABLE_PD_CODE_ON_PSRAM
extern void msDrvMcuPD51Run(void);
#endif

#if 0
extern void msDrvMcuMailBoxRead(void);
extern void msDrvMcuMailBoxWrite(void);
extern void msDrvMcuMailBoxClear(void);
#endif
extern BYTE msDrvMcuIsPD51Alive(void);
extern void msDrvMcuTrigIntToPD51(void);
extern void msDrvMcuTrigIntToSECUR2(void);
#endif

