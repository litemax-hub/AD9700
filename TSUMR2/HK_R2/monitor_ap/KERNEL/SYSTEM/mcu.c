#define _MCU_C_
#include "board.h"
#include "types.h"
#include "misc.h"
#include "Debug.h"
#include "Common.h"
#include "Ms_rwreg.h"
#include "REG52.H"
#include "Global.h"
#include "mStar.h"
#include "ms_reg.h"
#include "Mcu.h"
#include "MsIRQ.h"
#include "MsOS.h"
#include "Isr.h"
#include "msEread.h"
#include "GPIO_DEF.h"
#include "mdrv_hdmiRx.h"

#if ENABLE_UART_PIU
#include "drvUARTPIU.h"
#endif
#if (ENABLE_USB_TYPEC)
#include "mailbox.h"
#include "usbsw.h"
#endif

//////////////////////////////////
//#include "drvMcu.h"
//#include "halRwreg.h"
/////////////////////////////////
// dummy variables for 8051 internal registers
BYTE EA;
BYTE IT0;
BYTE IE0;
BYTE EX0;
BYTE IT1;
BYTE IE1;
BYTE EX1;
BYTE EX4;
BYTE IEX4;
BYTE ES;
BYTE IP0;
BYTE IP1;
BYTE TMOD;
BYTE TF0;
BYTE ADCON;
BYTE SCON;
BYTE PCON;
BYTE TI;
BYTE RI;
#if 1
BYTE S0RELH;
BYTE S0RELL;
BYTE IEN2;
BYTE TR0;
BYTE ET0;
BYTE TH0;
BYTE TL0;
BYTE TR0;
BYTE ET0;
#endif

void Init_MCU( void )
{
    EA=0;

#if( ENABLE_WATCH_DOG )
    Init_WDT( _ENABLE );
#else
    Init_WDT( _DISABLE );
#endif  // end of #if( ENABLE_WATCH_DOG )

    Init_IOPorts();
    Init_ExternalInterrupt();
    Init_Timer();
    Init_SerialPort();

#if 0//ENABLE_MSTV_UART_DEBUG
#if ENABLE_MSTV_UART_DEBUG_PIU
    mcuSetUartMux(UART_ENGINE_DW_UART, DFT_MUX_Uart0);
#else
    mcuSetUartMux(UART_ENGINE_51_UART0, DFT_MUX_Uart0);
#endif
#endif

#if UART1
    mcuSetUartMux(UART_ENGINE_51_UART1, DFT_MUX_Uart1);
#endif

#if Enable_Cache
    CACHE_ENABLE();
#else
    CACHE_DISABLE();
#endif

    EA=1;
    mcuSetSystemSpeed(SPEED_NORMAL_MODE );
}

/*
#if( ENABLE_WATCH_DOG )
void ClearWDT( void )
{
    old_msWriteBit( REG_3C66, 1, _BIT1 );
}
#endif  // end of #if( ENABLE_WATCH_DOG )
*/

// Initialize I/O setting
void Init_IODrv(void)
{
#if (CHIP_ID == CHIP_MT9700)
    // init all gpio drv to 4mA
    msWriteByte(REG_000495, 0x96);      // 10 01 01 10, GPIO_DPC0, GPIO23, GPIO22, GPIO01,
    msWriteByte(REG_000496, 0x82);      // 10 -- -- 10, GPIO_36  ,   --  ,   --  , GPIO_DPC5
    msWriteByte(REG_000497, 0xAA);      // 10 10 10 10, GPIO02, GPIO00, GPIO_DPC2, GPIO37
    msWriteByte(REG_000498, 0xAA);      // 10 10 10 10, GPIO52, GPIO51, GPIO25, GPIO24
#endif
}

void Init_GPIOTrim(void)
{
    BYTE u8EfuseData;
#if (CHIP_ID == CHIP_MT9700)
    u8EfuseData = msEread_GetDataFromEfuse(0x146);
    msWriteByteMask(REG_000499, (u8EfuseData&(BIT6|BIT5))>>5, BIT1|BIT0);
#elif(CHIP_ID == CHIP_MT9701)
    u8EfuseData = msEread_GetDataFromEfuse(0x14A);
    if(u8EfuseData & BIT(2))
    {
        msWriteByteMask(REG_00049C, (u8EfuseData&(BIT1|BIT0)), BIT1|BIT0);
    }
#endif

}

#if (CHIP_ID == CHIP_MT9701)
static void Init_BandgapRefTrim(void)
{
    BYTE u8EfuseData, u8EfuseACT;

    u8EfuseData = msEread_GetDataFromEfuse(0x13C);
    u8EfuseACT = msEread_GetDataFromEfuse(0x138);

    if( u8EfuseACT & _BIT2 )
    {
        u8EfuseData = (u8EfuseData & 0xF8);
        msWriteByteMask(REG_0003A9, u8EfuseData>>1, 0x7C);
    }
}
#endif

void Init_IOPorts( void )
{
    Init_IODrv();
#if ENABLE_CEC
    Init_GPIOTrim();
#endif

#if (CHIP_ID == CHIP_MT9701)
    Init_BandgapRefTrim();
#endif

    hw_SetDDC_WP();
#if (CHIP_ID!=CHIP_MT9701)
    hw_SetFlashWP();
#else
    #warning "--------------------Flash WP of MT9701 TBD---------------------------"
#endif

#if (MainBoardType == BD_MT9700_LITEMAX)
    Init_PCB_POWER_SW();
    DVI_HDMI_EN();
#endif

    hw_ClrBlacklit();
    hw_ClrPanel();
    hw_ClrGreenLed();
    hw_ClrAmberLed();

    Init_hwDDC_WP_Pin();
    Init_hwFlash_WP_Pin();
    Init_hwBlacklit_Pin();
    Init_hwPanel_Pin();
    Init_hwGreenLed_Pin();
    Init_hwAmberLed_Pin();

    Init_PowerKey();
    Init_hwDSUBCable_Pin();
#if (CHIP_ID==CHIP_TSUMC) || (CHIP_ID==CHIP_TSUMD) || (CHIP_ID == CHIP_TSUMJ) || (CHIP_ID == CHIP_TSUM9)|| (CHIP_ID == CHIP_TSUMF)
    Init_hwDVI0Cable_Pin();
    Init_hwDVI1Cable_Pin();
    Init_hwDVI2Cable_Pin();
#endif

#if ENABLE_DP_INPUT
	Init_hw_DP0_GNDPin();
	Init_hw_DP1_GNDPin();
	Init_hw_DP2_GNDPin();
	Init_hw_DP3_GNDPin();
	Init_hw_DP0_SDMPin();
	Init_hw_DP1_SDMPin();
	Init_hw_DP2_SDMPin();
	Init_hw_DP3_SDMPin();
    Init_hw_DP0_PWRPin();
    Init_hw_DP1_PWRPin();
    Init_hw_DP2_PWRPin();
    Init_hw_DP3_PWRPin();
#if ENABLE_TYPEC_SAR_DET
    Init_hw_TYPEC3_SDMPin();
    Init_hw_TYPEC4_SDMPin();
#endif
#endif

#if HDMI_PRETEST
    Init_hwCEC_Pin();
#endif

#if (CHIP_ID == CHIP_TSUM9||CHIP_ID == CHIP_TSUMF)
    DVI5V_GPIO_Sel();
#endif

#if !USEFLASH
    Set_EEPROM_WP();
    Init_hwI2C_SCL_Pin();
    Init_hwI2C_SDA_Pin();
    Init_hwEEPROM_WP();
#endif

#if AudioFunc
	Init_hwMute_Pin();
	hw_ClrMute();
#endif

#if ENABLE_TOUCH_PANEL || ENABLE_TOUCH_PANEL_DATA_FROM_USB||ENABLE_TOUCH_PANEL_CTRL_OSD
#if ENABLE_TOUCH_PANEL_DATA_FROM_USB
    Init_USB_INT();
#endif
    // keep all output pin at low state before turning on touch panel power
    Clr_TOCUCH_PANEL_POWER();
    Init_TOCUCH_PANEL_POWER_Pin();
    Init_SPI_MISO_Pin();
    Clr_SPI_SCK();
    Init_SPI_SCK_Pin();
    Clr_SPI_MOSI();
    Init_SPI_MOSI_Pin();
    Clr_SPI_SEL();
    Init_SPI_SEL_Pin();
#endif

#if ENABLE_TOUCH_KEY
    SET_CPK_KEYVCC();
    SET_CPK_LEDVCC();
    Init_HW_CPK_KEYVCC_Pin();
    Init_HW_CPK_LEDVCC_Pin();
    Init_SK_INT();
    Init_SK_SCL();
    Init_SK_SDA();
#endif

#if PANEL_VCOM_ADJUST
    HW_CLR_VCOM_I2C_SCL();
    HW_CLR_VCOM_I2C_SDA();
    Init_VCOM_I2C_SCL_Pin();
    Init_VCOM_I2C_SDA_Pin();
#endif

#if ENABLE_USB_TYPEC
    if(!IsUSBDL())
    {
        Init_Hub_Vbus();
    }
	Init_TYPEC_A_AUX_TRI();
	Init_TYPEC_A_AUX_P();
	Init_TYPEC_A_AUX_N();

	Init_TYPEC_B_AUX_TRI();
	Init_TYPEC_B_AUX_P();
	Init_TYPEC_B_AUX_N();
#endif

    #if ENABLE_HDMI && HDMI_HPD_AC_ON_EARLY_PULL_LOW
    Init_hwHDCP_Hpd_Pin();
    Init_hwHDCP_Hpd_Pin2();
    Init_hwHDCP_Hpd_Pin3();
    hw_Clr_HdcpHpd();
    hw_Clr_HdcpHpd2();
    hw_Clr_HdcpHpd3();
    g_u16HDMIHPDAcOnEarlyPullLowCnt = 700;
    #endif
}
/* initialize 8051 CPU timer & interrupt routine */
/* TCON.7(   TF1): Timer 1 overflow flag */
/* TCON.6(   TR1): Timer 1 stop/start bit */
/* TCON.5(   TF0): Timer 0 overflow flag */
/* TCON.4(   TR0): Timer 0 stop/start bit */
/* TCON.3(   IE1): Timer 1 INT1 interrupt flag */
/* TCON.2(   IT1): Timer 1 INT1 interrupt style setup*/
/* TCON.1(   IE0): Timer 0 /INT0 interrupt flag */
/* TCON.0(   IT0): Timer 0 /INT0 interrupt style setup */
void Init_ExternalInterrupt( void )
{
#if (ENABLE_DP_INPUT)
	MsOS_AttachInterrupt(E_INT_PM_IRQ_DP_AUX_ALL, (InterruptCb)ISR_R2IRQ_AUX_IRQ);
    MsOS_EnableInterrupt(E_INT_PM_IRQ_DP_AUX_ALL);
#endif

#if(ENABLE_CEC_INT)
    MsOS_AttachInterrupt(E_INT_PM_IRQ_CEC0_OUT, (InterruptCb)ISR_R2IRQ_PM_CEC_IRQ);
    MsOS_EnableInterrupt(E_INT_PM_IRQ_CEC0_OUT);
#endif

#if ((CHIP_ID == CHIP_MT9701) && (ENABLE_HDMI || ENABLE_DVI))
    msWrite2ByteMask(0x101928,0x00,BIT6|BIT11);

    MsOS_AttachInterrupt(E_INT_PM_IRQ_SCDC, (InterruptCb)ISR_R2IRQ_PM_SCDC_IRQ);
    MsOS_EnableInterrupt(E_INT_PM_IRQ_SCDC);
    mdrv_tmds_SetTimingChgCallBackFunction(MuteVideoAndAudio);

#else
    msWriteByteMask(0x000D6B,0x00,BIT5);
#if ((ENABLE_HDMI_BCHErrorIRQ || DEF_COMBO_HDCP2RX_ISR_MODE) && (ENABLE_HDMI || ENABLE_DVI))
    msWriteByteMask(0x101968,0x00,BIT6); //[6] DVI_HDMI_HDCP_INT
#endif
#endif

#if (CHIP_ID == CHIP_MT9701)
#if (ENABLE_USB_TYPEC && U3_REDRV_PM_MODE)
    MsOS_AttachInterrupt(E_INT_PM_IRQ_U3_RT, (InterruptCb)ISR_U3_RT);
    MsOS_AttachInterrupt(E_INT_PM_IRQ_U3_WAKEUP, (InterruptCb)ISR_U3_WAKEUP);
#endif
    MsOS_AttachInterrupt(E_INT_PM_FIQ_EXTIMER0,(InterruptCb)ISR_R2FIQ_PM_EXT_TIMER0);
#endif


	//enable ISR_R2IRQ_NONPM_IRQ relative mask setting...
    MsOS_AttachInterrupt(E_INT_PM_IRQ_NPM_IRQ_TO_HKR2, (InterruptCb)ISR_R2IRQ_NONPM_IRQ);
    MsOS_AttachInterrupt(E_INT_PM_IRQ_NPM_FIQ_TO_HKR2, (InterruptCb)ISR_R2IRQ_NONPM_FIQ);
    MsOS_AttachInterrupt(E_INT_PM_IRQ_D2B, (InterruptCb)ISR_DDC2Bi);

#if ENABLE_WATCH_DOG_INT
    MsOS_AttachInterrupt(E_INT_PM_FIQ_WDT, (InterruptCb)ISR_WDT);
    MsOS_EnableInterrupt(E_INT_PM_FIQ_WDT);
#endif


    MsOS_EnableInterrupt(E_INT_PM_IRQ_NPM_IRQ_TO_HKR2);
    MsOS_EnableInterrupt(E_INT_PM_IRQ_NPM_FIQ_TO_HKR2);

    //Enable INT_NPM_IRQ_SCINT
    INT_SYNC_CHANGE_TRIGGER_TYPE();
    INT_NPM_IRQ_SCINT_ENABLE(TRUE);

    //Enable INT_NPM_IRQ_DPRX_INT & INT_NPM_IRQ_DPRX2_INT
    INT_NPM_IRQ_DPRX_INT_ENABLE(TRUE);
    INT_NPM_IRQ_DPRX2_INT_ENABLE(TRUE);
}
#if EXT_TIMER0_1MS
void Init_ExtTimerCount( void )
{
    DWORD u32ExtTimerCount;
    u32ExtTimerCount=((DWORD)(EXT_TIMER0_CounterTime)*((msReadByte(REG_PM_BC)&BIT6)?(CLOCK_4MHZ/1000ul):(CLOCK_XTAL/1000ul)));
    msWrite2Byte(REG_002C24, u32ExtTimerCount&0xFFFF);
    msWrite2Byte(REG_002C26, (u32ExtTimerCount>>16)&0xFFFF);
    msWrite2Byte(REG_002C20, 0x0101);
}
#endif
void Init_Timer( void )
{
    WORD clock_period = ClockPeriod;

    /* -------------initialize Timer 0 -----------------------------*/
    g_ucTimer0_TH0 = clock_period >> 8;
    g_ucTimer0_TL0 = clock_period & 0xFF;

    TMOD=0x11; // Timer 0/1 acts as Timer
#if EnableTime0Interrupt
    TH0 = g_ucTimer0_TH0;
    TL0 = g_ucTimer0_TL0; // timer 0 counter
    TF0=0; // timer 1 flag
    TR0=1;  // disable timer 0
    ET0=1;  // enable timer 0 interrupt
#endif
    /* -------------initialize Timer 1 -----------------------------*/
#if EnableTime1Interrupt
    TR1 = 0;
    ET1 = 0;
    TH1 = g_ucTimer0_TH0;
    TL1 = g_ucTimer0_TL0;
    TR1 = 1;
    ET1 = 1;
#endif

#if EXT_TIMER0_1MS
    Init_ExtTimerCount();
#endif
}
/*----------Timer 2 -------------------*/
/* T2CON.7(   TF2): overflow flag */
/* T2CON.6(  EXF2): extern enable flag */
/* T2CON.5(  RCLK): receive clock */
/* T2CON.4(  TCLK): transfer clock */
/* T2CON.3( EXEN2): extern enable flag */
/* T2CON.2(   TR2): stop/start timer 2 */
/* T2CON.1(  C_T2): intern clock(0)/extern counter(1) switch */
/* T2CON.0(CP_RL2): capture flag */
void Init_SerialPort( void )
{
    ADCON |= _BIT7;             // use S0RELH, S0RELL as baudrate generator
    SCON = 0x50;                // mode 1, 8-bit UART, enable receive
    PCON |= _BIT7;
    TI = 0;                     // clear transfer flag
    RI = 0;

  #if ENABLE_MSTV_UART_DEBUG

    ES = 1;                     // enable uart interrupt
    // default baudrate-xtal
    S0RELH = HIBYTE( S0REL );
    S0RELL = LOBYTE( S0REL );

#if EnableSerialPortTXInterrupt
    uart_tx_initial();
#endif

  #else
    ES = 0;                     // disable uart interrupt
    IEN2 &= ~ES1;
  #endif

  #if UART1
    S1CON = (_BIT7 | _BIT4);

    // baudrate
    S1RELH = HIBYTE( S1REL );
    S1RELL = LOBYTE( S1REL );

    S1CON &= ~TI1;    // clear TI1
    S1CON &= ~RI1;    // clear RI1

    TI1_FLAG = 0;
    u8ReSyncCounter1 = 0;
    SIORxIndex1 = 0;

    IEN2 |= ES1;
  #endif

    TI0_FLAG = 0;
    u8ReSyncCounter = 0;
    SIORxIndex = 0;
/*
    g_UartCommand.Index = 0;
    g_bUartDetected=0;

    g_bDebugASCIICommandFlag = _DISABLE;
    CLR_DEBUG_ONLY_FLAG();
*/

#if ENABLE_UART_PIU
    piu_uart_init(CLK_PIU_172M, UART_PIU_BAUDRATE);


#if ENABLE_MSTV_UART_DEBUG_PIU
    MDrv_UART_PIU_DebugInit();
#endif
#endif

}

void Jump2SPI(U32 u32BootAddr)
{
    //set up reset vector base
    msWrite2Byte(0x002B00+0xB4, (U16)(u32BootAddr >> 16));

    __asm__ __volatile__ (
        "\tl.syncwritebuffer\n"
        "\tl.jr     %0\n"
        "\tl.syncwritebuffer\n"
        : : "r" (u32BootAddr));

    while(1);
}

