#define _DRVMCU_C_
#include "Global.h"
#include "drvBDMA.h"
#include "HdcpHandler.h"
#include "drvIMI.h"
#if ENABLE_USB_TYPEC
#include "mailbox.h"
#include "InfoBlock.h"
#include "PDUpdate.h"
#endif
#include "drvUART.h"
#include "risc32_spr.h"
#include "MsOS.h"
#include "SysInit.h"
#include "drvUartDebug.h"
#include "Ms_PM.h"
#include "msflash.h"
#include "asmCPU.h"


#define DRVMCU_DEBUG    1
#if ENABLE_MSTV_UART_DEBUG && DRVMCU_DEBUG
#define DRVMCU_printData(str, value)   printData(str, value)
#define DRVMCU_printMsg(str)           printMsg(str)
#else
#define DRVMCU_printData(str, value)
#define DRVMCU_printMsg(str)
#endif

// dummy variables for 8051 internal registers
BYTE ES;
BYTE S0RELH;
BYTE S0RELL;
BYTE IEN2;
BYTE S1RELH;
BYTE S1RELL;
BYTE TR0;
BYTE ET0;
BYTE TH0;
BYTE TL0;
BYTE TR0;
BYTE ET0;

static DWORD g_u32CpuClock = 0;

void Init_WDT( BYTE bEnable )
{
	#if ENABLE_WATCH_DOG
	u8WDT_Enable = bEnable;
	#endif

    msWriteByteMask( REG_002C04, BIT0,BIT0);  //clear watchdog reset flag

	#if ENABLE_PD_CODE_ON_PSRAM
	InitWDTAck();
	#endif

    if( bEnable )
    {
        DRVMCU_printMsg("=====Enable Watch dog=====\r\n");
#if( ENABLE_WATCH_DOG )
        // initialize the watchdog timer reset interval and interrupt timer
        SetWDTClk(CLK_LIVE_XTAL);
#endif
    }
    else
    {
        msWrite2Byte( REG_002C08, 0x0000 );
        msWrite2Byte( REG_002C0A, 0x0000 );
    }
}

#if( ENABLE_WATCH_DOG )
void ClearWDT( void )
{
#if ENABLE_PD_CODE_ON_PSRAM
    if(IsWDTAck())
    {
        WDT_CLEAR();
        SetWDTAck();
    }
#else
    WDT_CLEAR();
#endif
}

void SetWDTClk(BYTE clk_live_sel)
{
    ClearWDT();

    if (clk_live_sel == CLK_LIVE_RCOSC_4M)
    {
        msWrite4Byte( REG_002C08, u8WDT_Enable?(WATCH_DOG_TIME_RESET * CLOCK_4MHZ):0); // need set from H-byte to L-byte if default is not maximum
#if ENABLE_WATCH_DOG_INT
        msWrite2Byte( REG_002C06, u8WDT_Enable?((WATCH_DOG_TIME_Interrupt * CLOCK_4MHZ)>>16):0 );
#endif
    }
    else // CLK_LIVE_XTAL
    {
        msWrite4Byte( REG_002C08, u8WDT_Enable?(WATCH_DOG_TIME_RESET * CRYSTAL_CLOCK):0 ); // need set from H-byte to L-byte if default is not maximum
#if ENABLE_WATCH_DOG_INT
        msWrite2Byte( REG_002C06, u8WDT_Enable?((WATCH_DOG_TIME_Interrupt * CRYSTAL_CLOCK)>>16):0 );
#endif
    }
}
#endif

/*
REG_0212
[0]: Enable GPIO 40, 41 as Uart-0 function
[1]: Enable GPIO 34, 35 as Uart-0 function
[2]: Enable GPIO 02, 00 as Uart-0 function
[3]: Enable GPIO 00, 01 as Uart-1 function ?
[4]: Enable GPIO 20, 21 as Uart-1 function
[5]: Enable GPIO 32, 33 as Uart-1 function

[7]: Enable GPIO 02, 01 as Uart-0 function
*/
// #if (ENABLE_DEBUG || UART1) //ENABLE_DEBUG:UART 0, UART1: UART 1
#if (DRVMCU_DEBUG || UART1) //DRVMCU_DEBUG:UART 0, UART1: UART 1
void mcuSetUartMux( UART_ENGINE ucUartEngine, UART_GPIO ucUartGPIO )
{
    switch(ucUartGPIO)
    {
        case UART_GPIOX03_04:
            msWriteByteMask(REG_000410, ucUartEngine, 0x07);
            break;
        case UART_GPIOX13_14:
            msWriteByteMask(REG_000410, ucUartEngine<<4, 0x70);
            break;
        case UART_GPIO00_02:
            msWriteByteMask(REG_000414, ucUartEngine, 0x07);
            break;
        case UART_GPIO50_51:
            msWriteByteMask(REG_000414, ucUartEngine<<4, 0x70);
            break;
        case UART_GPIO40_41:
            msWriteByteMask(REG_000415, ucUartEngine, 0x07);
            break;
        default:
            DRVMCU_printMsg("******UART MUX ERROR*****");
            break;
    }
}
#endif

// ucIndex ( PLL clock selection, whenever change, please make sure reg_ckg_spi[5]=0;)
//          0: 04   MHz
//          1: 12   MHz
//          2: 24   MHz
//          3: 36   MHz
//          4: 43   MHz
//          5: 54   MHz
//          6: 86   MHz
//          7: SSC clock
//          8: Crystal clock

void mcuSetSpiSpeed( BYTE ucIndex )
{
    DRVMCU_printData("spi speed = %x\n", ucIndex);

    if( ucIndex == IDX_SPI_CLK_XTAL)
    {
        msWriteByteMask( REG_000AE0, 0, _BIT2 );
    }
    else
    {
        msWriteByteMask( REG_000AE0, 0, _BIT2 ); //switch to xtal first for glitch free

        if(ucIndex == IDX_SPI_CLK_SSC)
        {
            msWriteByteMask(REG_0003B4, SPI_SSC_DIVIDER_A, 0x0F);
            msWriteByteMask(REG_0003B4, SPI_SSC_DIVIDER_B<<4, 0xF0);
            msWriteByteMask(REG_0003B5, SPI_SSC_DIVIDER_C, 0x0F);
            msWriteByteMask(REG_0003B5, SPI_SSC_DIVIDER_D<<4, 0xF0);
            msWriteByteMask(REG_0003B6, BIT0, BIT0);
        }
        
        msWriteByteMask( REG_000AE0, ucIndex << 3, _BIT5 | _BIT4 | _BIT3);
        msWriteByteMask( REG_000AE0, _BIT2, _BIT2 );
    }
}

void SetSPI_Quad_En(BYTE ucEnable)
{
    hw_ClrFlashWP();
    Delay1ms( 1 );
    //    WREG(0x09C0) = 0x06;   //SPI Flash WREN Command
    //    WREG(0x09C1) = 0x01;   //SPI Flash Write Command
    msWrite2Byte(REG_0009C0, 0x0106);

#if 1 //Flash MX25L1636E .. 1 Bytes SR, QE in SR-1[6]
    //    WREG(0x09C2) = 0x40;
    if (ucEnable==TRUE)
        msWriteByte(REG_0009C2, 0x40);
    else
        msWriteByte(REG_0009C2, 0x00);
    //    WREG(0x09C3) = 0x05;   //SPI Flash RDSR Command
    msWriteByte(REG_0009C3, 0x05);
    //    WREG(0x09D4) = 0x21;   //Write length: 1 Bytes / 2 Bytes / 1 Bytes
    //    WREG(0x09D5) = 0x01;
    msWrite2Byte(REG_0009D4, 0x0121);
#else //Flash W25Q128 .. 2 Bytes SR, QE in SR-2[1]
    //    WREG(0x09C2) = 0x00;
    //    WREG(0x09C3) = 0x02;
    if (ucEnable==TRUE)
        msWrite2Byte(REG_0009C2, 0x0200);
    else
        msWrite2Byte(REG_0009C2, 0x0000);
    //    WREG(0x09C4) = 0x05;   //SPI Flash RDSR Command
    msWrite2Byte(REG_0009C4, 0x0005);
    //    WREG(0x09D4) = 0x31;   //Write length: 1 Bytes / 3 Bytes / 1 Bytes
    //    WREG(0x09D5) = 0x01;
    msWrite2Byte(REG_0009D4, 0x0131);

#endif
    //    WREG(0x09D6) = 0x00;   //Read length: 0 Bytes / 0 Bytes / 1 Bytes
    //    WREG(0x09D7) = 0x01;
    msWrite2Byte(REG_0009D6, 0x0100);

    //    WREG(0x09D9) = 0xf0;   //enable second/third commands and auto check status
    //    WREG(0x09D8) = 0x07;  //fsp enable and interrupt enable
    msWrite2Byte(REG_0009D8, 0xf007);

    //    WREG(0x09DA) = 0x01;        //Trigger
    msWrite2Byte(REG_0009DA, 0x0001);

    //Done = RREG(0x09DC, Bit(0))  //Check FSP done flag
    //WREG(0x09DE, Bit(0)) = 1     //Clear FSP done flag
    //    while(!(WREG(0x09DC)&BIT0))
    SetTimOutConter(10);

    while( (!(msReadByte(REG_0009DC)&BIT0)) && bTimeOutCounterFlag );

    if( !bTimeOutCounterFlag )
    {
        DRVMCU_printMsg("SetSPI_Quad_En Timeout !!!");
    }

    msWrite2Byte(REG_0009DE, (msReadByte(REG_0009DE)|BIT0));     //Clear FSP done flag
    hw_SetFlashWP();
}
/*
SPI model select.
0x0: Normal mode, (SPI command is 0x03)
0x1: Enable fast read mode, (SPI command is 0x0B)
0x2: Enable address single & data dual mode, (SPI command is 0x3B)
0x3: Enable address dual & data dual mode, (SPI command is 0xBB)
0xa: Enable address single & data quad mode, (SPI command is 0x6B)
0xb: Enable address quad & data quad mode, (SPI command is 0xEB)
*/
void mcuSetSpiMode( BYTE ucMode )
{
    BYTE ucValue;

    switch( ucMode )
    {
        case SPI_MODE_FR:
            ucValue = 0x01;
            FlashFSPEnable(TRUE);
            break;

        case SPI_MODE_SADD:
            ucValue = 0x02;
            FlashFSPEnable(FALSE);
            break;

        case SPI_MODE_DADD:
            ucValue = 0x03;
            FlashFSPEnable(FALSE);
            break;

        case SPI_MODE_SAQD:
            ucValue = 0x0A;
            FlashFSPEnable(FALSE);
            break;

        case SPI_MODE_QAQD:
            ucValue = 0x0B;
            FlashFSPEnable(FALSE);
            msWriteByteMask( REG_000405, _BIT3, _BIT3 );
            break;

        case SPI_MODE_NORMAL:
            ucValue = 0x00;
            FlashFSPEnable(TRUE);
            break;
            
        default:
            ucValue = 0x00;
            break;
    }

    if ((ucMode == SPI_MODE_SAQD)||(ucMode == SPI_MODE_QAQD))
    {
        msWriteByte( REG_000AE4, SPI_MODE_FR ); // For FSP in SetSPI_Quad_En()
        SetSPI_Quad_En(TRUE);
        msWriteByteMask( REG_000405, _BIT3, _BIT3 ); //spi_quad_en
    }
    else
    {
        if(msReadByte(REG_000405)&_BIT3) // SetSPI_Quad_En might need update by each flash spec
        {
            msWriteByte( REG_000AE4, SPI_MODE_FR ); // For FSP in SetSPI_Quad_En()
            SetSPI_Quad_En(FALSE);
        }
        msWriteByteMask( REG_000405, 0x00, _BIT3 ); //spi_quad_en
    }
    msWriteByte( REG_000AE4, ucValue ); // SPI model select


    DRVMCU_printData( "SPI Mode = %d ", ucMode );
}
BYTE mcuGetSpiMode(void)
{
    BYTE ucMode;

    switch( msReadByte( REG_000AE4 ) )
    {
        case 0x01:
            ucMode = SPI_MODE_FR;
            break;

        case 0x02:
            ucMode = SPI_MODE_SADD;
            break;

        case 0x03:
            ucMode = SPI_MODE_DADD;
            break;

        case 0x0A:
            ucMode = SPI_MODE_SAQD;
            break;

        case 0x0B:
            ucMode = SPI_MODE_QAQD;
            break;

        case 0x00:
        default:
            ucMode = SPI_MODE_NORMAL;
    }

    return ucMode;
}

DWORD code g_mcuPLLFreqTable[] =
{
    CLOCK_500MHZ,          // 0,
    CLOCK_345MHZ,          // 1,
    CLOCK_288MHZ,          // 2,
    CLOCK_216MHZ,          // 3,
    CLOCK_172MHZ,          // 4,
    CLOCK_144MHZ,          // 5,
    CLOCK_R2_SLOW_CLK,     // 6,
    CLOCK_12MHZ,           // 7,
    CLOCK_XTAL,            // 8,    
    CLOCK_108MHZ,          // 9,
};

void mcuSetSlowClkSpeed(BYTE u8SlowClkDiv, BYTE u8SlowClkDivPD51)
{    
    #define IDX_MCU_PD51_SLOW_CLK   1

    BYTE u8ClkSel;
    BYTE u8SpeedIdx;
    BYTE u8Div;
    BYTE u8ClkSelPD51;
    BYTE u8SpeedIdxPD51;
    BYTE u8DivPD51;
    BOOL bXTALStsBK = TRUE;

    u8ClkSel = (msReadByte(REG_0003B3) & BIT7);
    u8SpeedIdx = (msReadByte(REG_0003B3) & 0x07); // HKR2 clk select
    u8Div = (msReadByte(REG_000372) & 0x1F); // HKR2 clock divide counter

    u8ClkSelPD51 = (msReadByte(REG_0003BC) & BIT0);
    u8SpeedIdxPD51 = (msReadByte(REG_0003BB) & 0x07);
    u8DivPD51 = ((msReadByte(REG_0003BB) & 0xF8) >> 3);

    if((!u8ClkSel) || (u8SpeedIdx != IDX_MCU_R2_SLOW_CLK) || (u8Div != u8SlowClkDiv)
        || (!u8ClkSelPD51) || (u8SpeedIdxPD51 != IDX_MCU_PD51_SLOW_CLK) || (u8DivPD51 != u8SlowClkDivPD51))
    {    
        //DRVMCU_printData("SLOW CLOCK = %d MHz \r\n", (CLOCK_R2_SLOW_CLK /1000000 /(u8SlowClkDiv + 1)));
        
        if((msReadByte(REG_PM_A6) & BIT2) == 0)
        {
            bXTALStsBK = FALSE;
            msWriteByteMask(REG_PM_A6, BIT2, BIT2);
        }

        mcuSetCPUClock((CLOCK_R2_SLOW_CLK /(u8SlowClkDiv + 1)), IDX_MCU_R2_SLOW_CLK);
		
        // set clk of PD51 and RIU
        msWriteByteMask(REG_0003BC, 0x00, BIT0);
        msWriteByteMask(REG_0003BB, u8SlowClkDivPD51 << 3, 0xF8);    
        msWriteByteMask(REG_0003BB, 0x01, 0x07);
        msWriteByteMask(REG_0003BC, BIT0, BIT0);

        if(bXTALStsBK == FALSE)
            msWriteByteMask(REG_PM_A6, 0x00, BIT2);

        if(!g_bSlowClkDetEnabled) // only first time enter PM to set uart with recovery clock
        {
        #if ENABLE_MSTV_UART_DEBUG
            MDrv_UART_Init(UART_ENGINE_R2_HK,SERIAL_BAUD_RATE_PM);
            MDrv_UART_DebugInit(DEFAULT_UART_DEV);
        #else
            mdrv_uart_close((MS_U32)mdrv_uart_open(DEFAULT_UART_DEV));
        #endif
        }
    }
    else
    {
        // To reset timer    
        mcuSetCPUClock((CLOCK_R2_SLOW_CLK /(u8SlowClkDiv + 1)), IDX_MCU_R2_SLOW_CLK);
        //DRVMCU_printData("SLOW CLOCK RECOVER = %d MHz \r\n", (CLOCK_R2_SLOW_CLK /1000000 /(u8SlowClkDiv + 1)));
    }
}

void mcuSetMcuSpeed( BYTE ucSpeedIdx )
{
    BOOL bXTALStsBK = TRUE;

    g_u32CpuClock = g_mcuPLLFreqTable[ucSpeedIdx];

#if ENABLE_USB_TYPEC
    if(msDrvMcuIsPD51Alive())
    {
        drvmbx_send_MCU_Speed_Update_CMD((ucSpeedIdx < IDX_MCU_R2_SLOW_CLK) ? 2 : ((ucSpeedIdx == IDX_MCU_R2_SLOW_CLK)? 1 : 11));
    }
#endif

    if(ucSpeedIdx == IDX_MCU_R2_SLOW_CLK)
    {
        msPM_SlowClkDetEnable(FALSE, HK_R2_RECOVERY_CLOCK, PD_51_RECOVERY_CLOCK);
    }
    else
    {
        mcuSetCPUClock(g_u32CpuClock, ucSpeedIdx);

#if ENABLE_MSTV_UART_DEBUG
        if(ucSpeedIdx < IDX_MCU_R2_SLOW_CLK)            
            MDrv_UART_Init(UART_ENGINE_R2_HK,SERIAL_BAUD_RATE);            
        else
            MDrv_UART_Init(UART_ENGINE_R2_HK,SERIAL_BAUD_RATE_PM);    
        
        MDrv_UART_DebugInit(DEFAULT_UART_DEV);
#else
        mdrv_uart_close((MS_U32)mdrv_uart_open(DEFAULT_UART_DEV));
#endif       

        if((msReadByte(REG_PM_A6) & _BIT2) == 0)
        {
            bXTALStsBK = FALSE;
            msWriteByteMask(REG_PM_A6, _BIT2, _BIT2);
        }

        //set clk of PD51 and RIU
        if(ucSpeedIdx < IDX_MCU_R2_SLOW_CLK)
        {
            msWriteByteMask( REG_0003BC, 0x00, _BIT0);
            msWriteByteMask( REG_0003BB, 0x02, 0x07); // 216M
            msWriteByteMask( REG_0003BC, _BIT0, _BIT0);
        }
        else
        {
            msWriteByteMask( REG_0003BC, 0x00, _BIT0); //XTAL
        }
        
        if(bXTALStsBK == FALSE)
            msWriteByteMask(REG_PM_A6, 0x00, _BIT2);

        DRVMCU_printData("R2_CLK = %d MHz \r\n", (g_u32CpuClock /1000 /1000));
    }    
}

//note : returned MCU speed is not consider slow clock divider
DWORD mcuGetMcuSpeed(void)
{
    return g_u32CpuClock;
}

void mcuSetCPUClock(DWORD u32CpuClock, BYTE ucSpeedIdx)
{
    if(msReadByte(REG_PM_A6) & BIT2)
        msWriteByteMask( REG_0003B3, 0x00, _BIT7);

    MsOS_CPU_SetClock(u32CpuClock, ucSpeedIdx);

    if(ucSpeedIdx != IDX_MCU_CLK_XTAL)
        msWriteByteMask( REG_0003B3, _BIT7, _BIT7);
}


//=========================================================
void mcuSetSystemSpeed(BYTE u8Mode )
{
#ifdef TSUMR2_FPGA
    u8Mode = SPEED_XTAL_MODE;
    return;
#endif

    if (g_u8SystemSpeedMode!=u8Mode)
    { //MCU speed >= SPI speed
        switch(u8Mode)
        {
            case SPEED_12MHZ_MODE:

                if (g_u8SystemSpeedMode>SPEED_12MHZ_MODE)
                {
                    mcuSetSpiSpeed( IDX_SPI_CLK_12MHZ );
                    mcuSetSpiMode( SPI_MODE_NORMAL );
                    mcuSetMcuSpeed( IDX_MCU_CLK_12MHZ );
                }
                else
                {
                    mcuSetMcuSpeed( IDX_MCU_CLK_12MHZ );
                    mcuSetSpiMode( SPI_MODE_NORMAL );
                    mcuSetSpiSpeed( IDX_SPI_CLK_12MHZ );
                }
                g_bMcuPMClock = 1; // 120925 coding addition
            break;
            case SPEED_XTAL_MODE:
                if (g_u8SystemSpeedMode>SPEED_XTAL_MODE)
                {
                    mcuSetSpiSpeed( IDX_SPI_CLK_XTAL );
                    mcuSetSpiMode( SPI_MODE_NORMAL );
                    mcuSetMcuSpeed( IDX_MCU_CLK_XTAL );
                }
                else
                {
                    mcuSetMcuSpeed( IDX_MCU_CLK_XTAL );
                    mcuSetSpiMode( SPI_MODE_NORMAL );
                    mcuSetSpiSpeed( IDX_SPI_CLK_XTAL );
                }
                g_bMcuPMClock = 1; // 120925 coding addition
            break;
            case SPEED_SLOWCLK_MODE:
                if (g_u8SystemSpeedMode>SPEED_SLOWCLK_MODE)
                {
                    mcuSetSpiSpeed( IDX_SPI_CLK_12MHZ );
                    mcuSetSpiMode( SPI_MODE_FR );
                    mcuSetMcuSpeed( IDX_MCU_R2_SLOW_CLK );
                }
                else
                {
                    mcuSetMcuSpeed( IDX_MCU_R2_SLOW_CLK );
                    mcuSetSpiMode( SPI_MODE_FR );
                    mcuSetSpiSpeed( IDX_SPI_CLK_12MHZ );
                }
                g_bMcuPMClock = 1; // 120925 coding addition
            break;
            default: //normal
                mcuSetSpiMode( SPI_MODE );
                msWriteByte( REG_101ED0, 0x92 ); // power on mpll and set output-divider /2
                msWriteByte( REG_101ED1, 0x54 );
                msWriteByte( REG_101EDC, 0x00 );
                msWriteByte( REG_101EDD, 0x00 );
                MDrv_SyninpllR2Init();
                ForceDelay1ms(10);
                mcuSetMcuSpeed( MCU_SPEED_INDEX );
                mcuSetSpiSpeed( SPI_SPEED_INDEX );
                g_bMcuPMClock = 0; // 120925 coding addition
            break;
        }
        g_u8SystemSpeedMode=u8Mode;
        //SetForceDelayLoop();
    }
}

#if 1
void mcuInitXdataMapToDRAM(void)
{
    /*
    Initial XDATA on DRAM.
    Win0: MCU view 0x8000~0xEFFF(28KB)
    Win1: MCU view 0xF000~0xFFFF(4KB)
    */
    msWriteByte(REG_002BC6, WIN0_ADDR_START); // unit is K Byte
    msWriteByte(REG_002BC7, WIN0_ADDR_END);

    msWriteByte(REG_002BCA, WIN1_ADDR_START);
    msWriteByte(REG_002BCB, WIN1_ADDR_END);

    msWriteBit(REG_002BC4, _ENABLE, _BIT2);   //  enable
}

//------------------------------------------------------------------------------------------------------
// Function Name:  mcu40kXdataMapToDRAMorIMI
//
// Description: using window0(64K alignment) for mpping xdata(accessed by HK51) to DRAM or IMI(8KB)
//
// Parameter:
//         dwADDR: start address of DRAM or IMI which xdata mapping to
//         ucDestSel: DRAM or IMI slelection, 0: DRAM, 1: IMI
//
// The low byte address to access xdata from MIU.
// The granularity is 64k bytes.
// The actual address[26:0] to miu would be {reg_sdr_xd_map[10:8],reg_sdr_xd_map[7:0],xdata_addr[15:0]},
// where xdata_addr[15:0] is mcu xdata address of 64k bytes.
//------------------------------------------------------------------------------------------------------
void mcu40kXdataMapToDRAMIMI(DWORD dwADDR, BYTE ucDestSel)
{
    DWORD dwADDRCmp;

    if(ucDestSel)
    {
        DRVMCU_printMsg(" Window 0(64K align) cann't mapping xdata to IMI!! ");
        return;
    }

    msWrite2Byte(REG_002BC8, dwADDR>>16);
    msWriteByte(REG_103C1C, 0x24);

    do
    {
        dwADDRCmp = (DWORD)(((DWORD)msRead2Byte(REG_002BC8))<<16);
    }while( dwADDRCmp!=(dwADDR&0xFFFF0000) );
}

//------------------------------------------------------------------------------------------------------
// Function Name:  mcuXdataMapToDRAMorIMI
//
// Description: using window1(4K alignment) for mpping xdata(accessed by HK51) to DRAM or IMI(8KB)
//
// Parameter:
//         dwADDR: start address of DRAM or IMI which xdata mapping to
//         ucDestSel: DRAM or IMI slelection, 0: DRAM, 1: IMI
//
//------------------------------------------------------------------------------------------------------
void mcu4kXdataMapToDRAMIMI(DWORD dwADDR, BYTE ucDestSel)
{
    DWORD dwADDRCmp;

    msWriteByte(REG_103C1C, (ucDestSel ? 0x25 : 0x24));
    msWrite2Byte(REG_002BCC, dwADDR>>12);
    msWrite2Byte(REG_002BCE, dwADDR>>28);

    do
    {
        dwADDRCmp = (DWORD)(((DWORD)msRead2Byte(REG_002BCE)<<28) | ((DWORD)msRead2Byte(REG_002BCC))<<12);
    }while( dwADDRCmp!=(dwADDR&0xFFFFF000) );
}
#endif

//------------------------------------------------------------------------------------------------------
// Function Name:  mcuDMACRC32
//
//  Description: BDMA do check CRC value
//
// Parameter:
//          channel:  can be set to CHANNEL_AUTO / CHANNEL_0 / CHANNEL_1.We can assign BDMA channel by using  CHANNEL_0 and CHANNEL_1
//                           if we set to CHANNEL_AUTO, BDMA_Operation function will auto switch to chnnel1 if channel0 is busy.
//          SourceType: you can choose device source, please refer to BDMA_SOURCE_TYPE structure
//          dwSourceAddr:   set Check CRC Start Address
//          dwByteCount:    set Check CRC  size
//          dwPolynimial: set Polynomial
//          dwSeed: set seed
//          dwPattern:  return 4 byte value
//
//------------------------------------------------------------------------------------------------------
DWORD mcuDMACRC32(BYTE channel,BYTE SourceType,DWORD dwSourceAddr, DWORD dwByteCount, DWORD dwPolynimial, DWORD dwSeed)
{
    return BDMA_Operation(channel, SourceType, DEST_CRC32, dwSourceAddr, dwSourceAddr, dwByteCount,dwPolynimial, dwSeed);
}

#if (ENABLE_HK_CODE_ON_DRAM || ENABLE_HK_CODE_ON_PSRAM || ENABLE_PD_CODE_ON_PSRAM)
void mcuDMADownloadCode(BYTE SourceType, BYTE DestinType, DWORD dwSourceAddr, DWORD dwDestinAddr, DWORD dwByteCount)
{
    if((SourceType == SOURCE_SPI) && (DestinType == DEST_MIU0))
    {
        BDMA_Operation(CHANNEL_AUTO, SOURCE_SPI, DEST_MIU0, dwSourceAddr, dwDestinAddr, dwByteCount, 0, 0);
        msMiuProtectCtrl(MIU_PROTECT_0, _ENABLE, MIU_ID_PM_51, HK_CODE_ADDR, (HK_CODE_ADDR + HK_CODE_SIZE));
    }
    else if((DestinType == DEST_SECR2_IQMEM) || (DestinType == DEST_SECR2_DQMEM))
    {
        msWriteByteMask(REG_100ED7,BIT7, BIT7); // stall R2
        msWriteByte(REG_100E80,0x07); //  enablesecu_r2 reset
        msWriteByteMask(REG_100EC8,BIT4,BIT4); // sel bdma
        BDMA_Operation(CHANNEL_AUTO,SourceType,DestinType,dwSourceAddr,dwDestinAddr,dwByteCount,0,0);
        msWriteByte(REG_100E80,0x00);
        msWriteByteMask(REG_100EC8,0,BIT4);
        msWriteByteMask(REG_100ED7, 0, BIT7);
    }
    else
    {
        BDMA_Operation(CHANNEL_AUTO,SourceType,DestinType,dwSourceAddr,dwDestinAddr,dwByteCount,0,0);
    }
}

#if (ENABLE_HK_CODE_ON_DRAM) || (ENABLE_HK_CODE_ON_PSRAM)
void mcuArrangeCodeAddr(void)
{
#if (ENABLE_HK_CODE_ON_DRAM)
    // dram address range, full code on DRAM first
    msWrite2Byte(REG_002B80, (HK_CODE_ADDR >> 16));

    msWrite2Byte(REG_00100C, (WORD)(MCU_ON_DRAM_START_ADDR));
    msWrite2Byte(REG_001008, (WORD)(MCU_ON_DRAM_START_ADDR>>16));

    msWrite2Byte(REG_00100E, (WORD)((HK_CODE_SIZE-1)));
    msWrite2Byte(REG_00100A, (WORD)((HK_CODE_SIZE-1)>>16));

    // dram enable
    msWriteByteMask(REG_001018, _BIT2, _BIT2|_BIT1);

    // spi address range
    msWrite2Byte(REG_001014, (WORD)(MCU_ON_SPI_START_ADDR));
    msWrite2Byte(REG_001010, (WORD)(MCU_ON_SPI_START_ADDR>>16));

    msWrite2Byte(REG_001016, (WORD)(MCU_ON_SPI_END_ADDR));
    msWrite2Byte(REG_001012, (WORD)(MCU_ON_SPI_END_ADDR>>16));

    // dram address end
    msWrite2Byte(REG_00100E, (WORD)(MCU_ON_DRAM_END_ADDR));
    msWrite2Byte(REG_00100A, (WORD)(MCU_ON_DRAM_END_ADDR>>16));

    // dram, spi enable
    msWriteByteMask(REG_001018, _BIT2|_BIT1, _BIT2|_BIT1);
#endif

#if (ENABLE_HK_CODE_ON_PSRAM)
    // set psram address range and enable
    msDrvMcuSetCodeRangeOnPSram(MCU_ON_PSRAM_START_ADDR, MCU_ON_PSRAM_END_ADDR);
#endif

}
#endif

#if ENABLE_HK_CODE_ON_PSRAM
//Note: Switching PSRAM on/off in common bank is strictly forbidden!!
void msDrvMcuCodeOnPSramEnable(BYTE bEnable)
{
    if(bEnable)
        msRegs(REG_0010E7) |= BIT7;
    else
        msRegs(REG_0010E7) &= (~BIT7);
}

//Note: Aligning start and end address is unnecessary, but must keep in the range of PSRAM.
void msDrvMcuSetCodeRangeOnPSram(DWORD dwAddrStart, DWORD dwAddrEnd)
{
    msDrvMcuCodeOnPSramEnable(FALSE);

    msWrite2Byte(REG_001004, (WORD)(dwAddrStart));
    msWrite2Byte(REG_001000, (WORD)(dwAddrStart>>16));
    msWrite2Byte(REG_001006, (WORD)(dwAddrEnd));
    msWrite2Byte(REG_001002, (WORD)(dwAddrEnd>>16));

    msDrvMcuCodeOnPSramEnable(TRUE);
}
#endif

#if ENABLE_PD_CODE_ON_PSRAM
#define PD51_BIN_ADDR_OVERWRITE		0 //set offset in flash of PD51.bin, 0 means using pd51_binfo.B_FAddr

void mcuWakeUpPD51(void)
{
    DRVMCU_printMsg("HKR2 Wakeup PD51.");
    msWrite2Byte(REG_001B00, 0x0101); // [0]:1:reset PD51, [8]:1:enable PSRAM -- [8]:no loading in TSUMG
    msWrite2Byte(REG_000C00, 0x0000); // PSRAM Range: 0x000000~0x020000, 128K
    msWrite2Byte(REG_000C04, 0x0000);
    msWrite2Byte(REG_000C02, 0x0002);
    msWrite2Byte(REG_000C06, 0x0000);
    msWrite2Byte(REG_000C18, 0x0001); //[0]:PSRAM enable, [1]:SPI disable, [2]:DRAM disable
    msWriteByte(REG_001B00, 0x00); // [0]:0:release PD51
}

void msDrvMcuPD51Run(void)
{
    BININFO pd51_binfo = {
        .B_ID = 0,
        .B_FAddr = 0,
        .B_Len = 0,
        .B_IsComp = 0
    };
	DWORD dwSrcAddr = 0;

#if ENABLE_INTERNAL_CC
    pd51_binfo.B_ID = BIN_ID_CODE_PD51_ICC;
#else
	pd51_binfo.B_ID = BIN_ID_CODE_PD51;
#endif
    if (!Get_BinInfo(&pd51_binfo))
    {
        DRVMCU_printData("Get PD51 BINFO (B_ID=0x%x) fail..\n", pd51_binfo.B_ID);
        return;
    }

#if ENABLE_PD_FW_AUTO_UPDATE
    msAPI_PDUpdateAuto((PD51_BIN_ADDR_OVERWRITE?PD51_BIN_ADDR_OVERWRITE:pd51_binfo.B_FAddr), pd51_binfo.B_Len, PDSA_MCU, PDSA_PCB);
#endif

	dwSrcAddr = (PD51_BIN_ADDR_OVERWRITE ? PD51_BIN_ADDR_OVERWRITE : pd51_binfo.B_FAddr);
    mcuDMADownloadCode(SOURCE_SPI, DEST_PD51_PSRAM, dwSrcAddr, 0x0, 0x20000); // size of PD51.bin is 128KB
    mcuWakeUpPD51();
}

#endif
#endif

#if 0
void msDrvMcuMailBoxRead(void)
{
    BYTE i;
    BYTE *pu8Tmp    = (BYTE *)(&g_sMailBoxR2);
    BYTE u8MBSize   = sizeof(sMAILBOX_R2);

    if(IsMailBoxValid())
    {
        for( i=0 ; i<u8MBSize ; i+=2 )
        {
            *(pu8Tmp+i+1) = msReadByte(REG_103380+i);
            *(pu8Tmp+i)   = msReadByte(REG_103380+i+1);
        }
    }

}

void msDrvMcuMailBoxWrite(void)
{
    BYTE i;
    BYTE *pu8Tmp    = (BYTE *)(&g_sMailBox51);
    BYTE u8MBSize   = sizeof(sMAILBOX_51);

    msDrvMcuMailBoxClear();

    for( i=0 ; i<u8MBSize ; i+=2 )
    {
        msWriteByte(REG_103380+i+1, *(pu8Tmp+i));
        msWriteByte(REG_103380+i  , *(pu8Tmp+i+1));
    }

    SetMailBoxValid();
}

void msDrvMcuMailBoxClear(void)
{
    BYTE i;

    for( i=0 ; i<NUM_MAILBOX ; i++ )
    {
        msWriteByte(REG_103380+i, 0x00);
    }
}
#endif

void DelayUs(DWORD u32Delayus)
{
    MsOS_DelayTaskUs(u32Delayus);
}

// Timer0 32-bit counter
#define US_UNIT     (CRYSTAL_CLOCK/1000000)
void msDrvSetExtTimerCnt(DWORD u32UsCnt)
{
    msWrite4Byte(REG_002C24, u32UsCnt*US_UNIT);
}
#undef US_UNIT

void msDrvEnableExtTimer(BOOL bEnable)
{
    msWrite2ByteMask(REG_002C20, bEnable?BIT1|BIT8:0, BIT1|BIT8);//count start
}

BYTE msDrvMcuIsPD51Alive(void)
{
    return ((msReadByte(REG_001B00)&BIT0)?0:1);
}

void msDrvMcuTrigIntToPD51(void) // could be further speed up by changing to MACRO
{
#if(CHIP_ID == CHIP_MT9701)
    MEM_MSWRITE_BYTE(REG_002A40, 0); // only BIT0 will be used. To speed up w/o WBmask
    MEM_MSWRITE_BYTE(REG_002A40, BIT0);
#else
    MEM_MSWRITE_BYTE(REG_002A00, 0); // only BIT0 will be used. To speed up w/o WBmask
    MEM_MSWRITE_BYTE(REG_002A00, BIT0);
#endif
}

void msDrvMcuTrigIntToSECUR2(void) // could be further speed up by changing to MACRO
{
    MEM_MSWRITE_BYTE(REG_100540, 0);
    MEM_MSWRITE_BYTE(REG_100540, BIT2);
}

