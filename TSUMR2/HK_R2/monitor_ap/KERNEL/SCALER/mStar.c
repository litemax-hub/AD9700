#define _MSTAR_C_
#include <math.h>
#include "types.h"
#include "board.h"
#include "ms_reg.h"
#include "Global.h"
#include "Mcu.h"
//#include "panel.h"
#include "Adjust.h"
#include "drvDeltaE.h"
//#include "msADC.h"
#include "Ms_rwreg.h"
#include "Debug.h"
#include "Common.h"
#include "misc.h"
#include "DDC.h"
#include "Power.h"
#include "Detect.h"
#include "msOSD.h"
#include "Gamma.h"
//#include "extlib.h"
#include "Menu.h"
#include "menudef.h"
#include "menufunc.h"
#include "msflash.h"
//#include "msid_v1.h"
#include "drvGPIO.h"
#include "drv_adctbl.h"
#include "drvADC.h"
#include "msEread.h"
#if ENABLE_RTE
#include "msOVD.h"
#endif
#if (MS_PM)
#include "Ms_PM.h"
#endif
#include "MsDLC.h"
#include "msACE.h"
#include "UserPref.h"
#include "Panel.h"

#if ENABLE_LOCALDIMMING
#include "msLD_Handler.h"
#endif

#if ENABLE_DAC
#include "drvDAC.h"
#endif
#if ENABLE_3DLUT
#include "drv3DLUT_C.h"
#endif
#include "mapi_DPRx.h"
#include "mdrv_hdmiRx.h"
#include "dpCommon.h"
#include "ComboApp.h"
#include "mapi_eDPTx.h"
#if (CHIP_ID == CHIP_MT9701)
#include "system_eDPTx.h"
#endif

#if ENABLE_USB_TYPEC
#include "mailbox.h"
#endif
#include "drvBDMA.h"

#include "usbsw.h"

#if ENABLE_MENULOAD || ENABLE_MENULOAD_2 || ENABLE_PQ_R2
#include "msMenuload.h"
#endif

///////////////////////////////////////////
//#include "drvMcu.h"
#include "drvmStar.h"
//#include "appmStar.h"
//#include "halRwreg.h"
//////////////////////////////////////////
#if ENABLE_HDR
#include "drvHDRCommon.h"
#include "drvHDR.h"
#endif

#if (CHIP_ID == CHIP_MT9700)
#include "dri_table_gen.h"
#else//(CHIP_ID == CHIP_MT9701)
#include "dri_table_gen_MT9701.h"
#endif

#include "CustomCbAPI.h"

// INTERNAL
#include "mStar.h"
#define MSTAR_DEBUG    0
#if ENABLE_MSTV_UART_DEBUG && MSTAR_DEBUG
#define MST_printData(str, value)   printData(str, value)
#define MST_printMsg(str)           printMsg(str)
#else
#define MST_printData(str, value)
#define MST_printMsg(str)
#endif
//*******************************************************************
//
// Constant Definition
//
//*******************************************************************
#define ADC2Sets    0//0// 1 for chip with 2 ADC(9x5x & 9x4x)
#define OutDClk1    108
#define OutDClk2    135
#if PanelLVDS
//14.318 * 15 * 8 / 3.5 = 490.9, 490.9<<19=257374477.16,   490.9<<3=8=3927
#define DClkFactor  257374477ul
//#define DClkFactor      257650102// use 215 as base
#else
//14.318 * 15 * 8 / 4 = 429.54, 429.54<<19=225202667.52,   429.54<<3=8=3436.32
#define DClkFactor  225202667ul
//#define DClkFactor      257650102// use 215 as base
#endif
#define FreeRunDClk (DWORD)DClkFactor/g_sPnlInfo.sPnlTiming.u16DClk

//*******************************************************************
//
// extern declaration
//
//*******************************************************************
//*******************************************************************
//
// local declaration
//
//*******************************************************************
xdata ST_COMBO_COLOR_FORMAT _colorFormat;
static BOOL bForceDeOnlyMode = FALSE;
static BOOL bDeOnlyModeEn = FALSE;

void mStar_SetupInputPort( void );
//void mStar_SetupFreeRunMode( void );
void mStar_SetupADC( void );
Bool mStar_SetCaptureWindow( void );
//void mStar_SetScalingFactor( void );
//void mStar_SetupPattern( void );
//Bool mStar_SetPanelTiming( void );
void mStar_InitADC( void );
//void mStar_InitTCON( void );
//void mStar_SetScalingFilter( void );
void mStar_SetUserPref( void );
//void mStar_ResetDClkPLL( void );
void SetPWMFreq( WORD freq );

void ScalerReset(BYTE ucParam)
{
    if( ucParam == RST_ALL )
    {
        // lpll on first for scaler rst
        msWriteByteMask(REG_140506, 0, BIT5); // reg_lpll1_pd

        SC0_SUB_BANK_SEL(0);//old_msWriteByte( SC0_00, 0x00 );
        SC0_SCALER_RESET(0);//old_msWriteByte( SC0_F1, 0x00 );
        //mStar_ScalerDoubleBuffer(FALSE);
        // power down contorl 0xF0~0xF1
        SC0_SCALER_RESET(0);//old_msWriteByte( SC0_F1, 0x00 ); //20081024 RD suggest
        SC0_SCALER_POWER_DOWN(0, 0xFF);//old_msWriteByte( SC0_F0, 0x00 ); //|BIT6); // power up chip
        SC0_SCALER_RESET(RST_ALL);//old_msWriteByte( SC0_F1, 0x7F ); // software reset // DDCCI enable bit6
        ForceDelay1ms(2);
        SC0_SCALER_RESET(0);//old_msWriteByte( SC0_F1, 0x00 ); // software reset
        SC0_NORMAL_MODE();//old_msWriteByte( SC0_F8, 0x00 ); // ???? important
    }
    else if( ucParam & RST_IP )
    {
        BYTE ucBackup = msReadByte(REG_120F85);

        SC0_SCALER_RESET(ucParam);
        ForceDelay1ms(2);
        msWriteByte(REG_120F85, 0x0C);  // switch idclk to odclk
        ForceDelay1ms(2);
        msWriteByte(REG_120F85, ucBackup);
        SC0_SCALER_RESET(0);
    }
    else
    {
        SC0_SCALER_RESET(ucParam);
        ForceDelay1ms(2);
        SC0_SCALER_RESET(0);
    }
}

void GPIOPWMSelect( void )
{
/*
    BYTE u8Retry=10;
    if(!u8Retry)
    {
        drvGPIO_uncall();
    }
*/
    msAPIPWMClkSel(PWM_CLK_XTAL);
    msAPIDispPWMMappingConfig(DISP_PWM_MAPPING_TYPE);
#if AudioFunc
    msAPIPWMConfig(VOLUME_PWM, VOLUME_PWM_FREQ, VOLUME_PWM_DUTY, VOLUME_PWM_CONFIG, VOLUME_PWM_VS_ALG_NUM ,0);
    msDrvPWMEnableSwitch(VOLUME_PWM, TRUE, VOLUME_PWM_CONFIG);
#endif
    msAPIPWMConfig(BACKLIGHT_PWM, BACKLIGHT_PWM_FREQ, BACKLIGHT_PWM_DUTY, BACKLIGHT_PWM_CONFIG, BACKLIGHT_PWM_VS_ALG_NUM ,0);
    msDrvPWMEnableSwitch(BACKLIGHT_PWM, TRUE, BACKLIGHT_PWM_CONFIG);
}

#if ENABLE_3DLUT
extern void drvGammaOnOff(BYTE u8Switch, BYTE u8Window);
#endif

void mStar_ACOnInit(void)
{
#ifndef TSUMR2_FPGA
    Init_FRORefTrim();
    #if 0//CHIP_ID == CHIP_MT9701
    Init_FROcalibrationByTsensor();
    #endif
#endif

#if ENABLE_FLASH_CURRENT_ADJUSTMENT
    msWriteByteMask(REG_000412, FLASH_DEFAULT_CURRENT, BIT1|BIT0); // SPI_CK driving current
    msWriteByteMask(REG_000412, FLASH_DEFAULT_CURRENT<<2, BIT3|BIT2); // SPI_DI driving current
    msWriteByteMask(REG_000412, FLASH_DEFAULT_CURRENT<<4, BIT5|BIT4); // SPI_DO driving current
    msWriteByteMask(REG_000412, FLASH_DEFAULT_CURRENT<<6, BIT7|BIT6); // SPI_CZ driving current
    msWriteByteMask(REG_000413, FLASH_DEFAULT_CURRENT, BIT1|BIT0); // GPIO10(SPI_WP)/GPIO11(SPI_HOLD) driving current
    msWriteByteMask(REG_000413, FLASH_DEFAULT_CURRENT<<2, BIT3|BIT2); // GPIO51~55 MSPI driving current
#endif



    msDrvDPCInit();

#if ENABLE_USB_TYPEC
    MBX_init();             // must be initialized before PD 51 wakeup
#endif

#if ENABLE_PD_CODE_ON_PSRAM
    msDrvMcuPD51Run();
#endif

    mStar_SetupInputPort();
}

//*******************************************************************
// Function Name: mStar_Init(void)
//
// Description: Initialize mStar chip while 1st power on system
//
// Caller: mStar_InitADC(), mStar_InitTCON, mStar_SetupFreeRunMode()
//         mStar_SetupInputPort() in mStar.c
//         old_msWriteByte(), mStar_WriteWord() in ms_rwreg.c
//         mStar_WriteDDC1(), mStar_WriteDDC2() in ddc.c
//         mStar_InitGamma() in gamma.c
//         Osd_InitOSD() in osd.c
// Callee: Init_Device() in main.c
//*******************************************************************
void mStar_Init( BOOL bACon )
{
//    BYTE NOUSE=0;
    BootTimeStamp_Set(MSTAR_INIT_TS, 0, TRUE); // mstar init timestamp 0
    drvmStar_InitPnl();

#if (CHIP_ID == CHIP_MT9701)
    mapi_eDPTx_VariableInit();
#endif

#if (CHIP_ID == CHIP_TSUMC)||(CHIP_ID == CHIP_TSUMD)||(CHIP_ID == CHIP_TSUMJ)||(CHIP_ID == CHIP_TSUM9)||(CHIP_ID == CHIP_TSUMF)

#if USE_VCTRL
    old_msWriteByteMask(REG_01A5,BIT1,BIT1);
    ForceDelay1ms(2);
    old_msWriteByteMask(REG_0183,0x10,0x70);
    ForceDelay1ms(2);
    old_msWriteByteMask(REG_0183,0x20,0x70);
    ForceDelay1ms(2);
    old_msWriteByteMask(REG_0183,0x30,0x70);
    ForceDelay1ms(2);
    old_msWriteByteMask(REG_0183,0x40,0x70);
    ForceDelay1ms(2);
    old_msWriteByteMask(REG_0183,0x50,0x70);
#elif USE_EXTERNAL_LDO
    old_msWriteByteMask(REG_01A5,0,BIT1);
    old_msWriteByteMask(REG_0183,0x50,0x70);
#else
#if CHIP_ID==CHIP_TSUMF
    if (msEread_GetDataFromEfuse(0x07)<228)
    {
        old_msWriteByteMask(REG_0183,0x70,0x70);
    }
    else
#endif
    {
        old_msWriteByteMask(REG_0183,0x00,0x70);
    }

#endif

#endif

    msDrvDPCRtermTrimInitNonPM(); // Needs init for every pm wakeup
    msDPCRtermModeInit();
#if ENABLE_INTERNAL_CC
    msDrvInternalCcTrimInitNonPM();
#endif
    msDrvUsbSwitchRtermEnDP(TRUE); // R-term Init for DP
    Init_USBXCVRRefTrim();

#if ENABLE_MENULOAD || ENABLE_MENULOAD_2 || ENABLE_DYNAMICSCALING || ENABLE_PQ_R2
    msMLoad_DS_Init();
#endif

#if ENABLE_MENULOAD || ENABLE_MENULOAD_2
    msMLoad_Init();
#endif

#if ENABLE_DOLBY_HDR
    API_Dolby_Addr_Init();
#endif

#if ENABLE_DYNAMICSCALING
    msDS_Init();
#endif
    BootTimeStamp_Set(MSTAR_INIT_TS, 1, TRUE); // mstar init timestamp 1
    drvmStar_Init(bACon);
    BootTimeStamp_Set(MSTAR_INIT_TS, 2, TRUE); // mstar init timestamp 2
    GPIOPWMSelect();

    //msDrvPowerModCtrl(_DISABLE); //Jison 110421 disable panel data out

    mStar_InitADC();
    mStar_SetupFreeRunMode();

    if( g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE )
    {
        System_eDPTx_Training();
    }

#ifndef TSUMR2_FPGA
    #if ENABLE_HDCP
    //msDisableHDCP();
    //msInitHDCPProductionKey(); //TBD
    #endif
#endif

    BootTimeStamp_Set(MSTAR_INIT_TS, 3, TRUE); // mstar init timestamp 3
#if ENABLE_DAC
        msAPI_AudioInit(bACon); // Audio DAC init
#endif
    BootTimeStamp_Set(MSTAR_INIT_TS, 4, TRUE); // mstar init timestamp 4
#if ENABLE_RTE
    msAPI_OverDriveInit();
#endif

    BootTimeStamp_Set(MSTAR_INIT_TS, 5, TRUE); // mstar init timestamp 5
#if Enable_ReducePanelPowerOnTime
    if( !bPanelOnFlag )
        g_ePanelStatus = ePANEL_STATUS_TurnOnData;

    if(bACon)
        Power_TurnOnPanel();
#endif

#if Enable_PanelHandler
    if( !bPanelOnFlag )
        g_ePanelState = ePANEL_STATE_0;

    if(bACon)
        Power_PanelCtrlOnOff(TRUE, FALSE);
#endif
    BootTimeStamp_Set(MSTAR_INIT_TS, 6, TRUE); // mstar init timestamp 6

    Osd_InitOSD();

#if DisplayPowerOnLogo
    if( DisplayLogoFlag )
    {
        Menu_InitAction();
    }
#endif

    BootTimeStamp_Set(MSTAR_INIT_TS, 7, TRUE); // mstar init timestamp 7
#if Enable_Gamma
    mStar_SetupGamma( UserPrefGamaMode );
  // mStar_SetupGamma( GAMA3 );
#endif
    BootTimeStamp_Set(MSTAR_INIT_TS, 8, TRUE); // mstar init timestamp 8
    InitialPeaking();
#if ENABLE_SUPER_RESOLUTION
    msInitalSRSetting();
#endif

#if ENABLE_SUPER_RESOLUTION
    mStar_SetupSuperResolution( UserPrefSuperResolutionMode );
#endif
#if CHIP_ID == CHIP_TSUMV       //111006 Rick modified  - A045
    msAdjustSharpness( MAIN_WINDOW, 0x22, 0 );
#elif ((CHIP_ID==CHIP_MT9700)|| (CHIP_ID==CHIP_MT9701))
  #if ENABLE_SHARPNESS
    msAdjustSharpness( MAIN_WINDOW, UserPrefSharpness, 0 );
  #endif
#else
    msAdjustSharpness( MAIN_WINDOW, 0x09, 0 );
#endif

    InitACEVar(); // Initialize the variable at ACE.c
    #if (USEFLASH && COLORDATA_USEFLASH && ENABLE_DeltaE)
    msAPI_appAdjust_FlashInit();
    #endif
    #if ENABLE_ColorMode_Preload_Function
    mdrv_DeltaE_CbInit();
    #endif
    #if 0 //fix coverity error-DEADCODE
    if(NOUSE)
    {
        InitialPeaking();
        msAdjustSharpness( MAIN_WINDOW, 0x10, 0 );
        InitDLCVar();
        msAdjustSubBrightness( MAIN_WINDOW, 0x80, 0x80, 0x80 );
        msAdjustVideoContrast( MAIN_WINDOW, 0x80 );
        msAdjustVideoHue( MAIN_WINDOW, DefHue );
        msAdjustVideoSaturation( MAIN_WINDOW, 0x80 );
    }
    #endif

    //old_msWriteByteMask(SC7_D0,BIT6,BIT6);//Main window Y noise-masking dither enable
    //old_msWriteByteMask(SC7_D2,BIT6,BIT6);//Sub window Y noise-masking dither enable

#if ENABLE_3DLUT
    mStar_Setup3DLUT();
    // disable the other color engine
    drvGammaOnOff(_DISABLE, MAIN_WINDOW );
#endif

#if ENABLE_DeBlocking
    msInitDeBlocking( );
#endif
    mStar_SetUserPref();

#if DISABLE_SHORT_FRAME
    msWriteByteMask(SC00_63, BIT2, BIT2);
    msWriteByteMask(SC00_63, 0, BIT3);
#endif

    MENU_LOAD_ACTIVE();
    BootTimeStamp_Set(MSTAR_INIT_TS, 9, TRUE); // mstar init timestamp 9
}

#if UseINT //Enable mode change INT
void mStar_EnableModeChangeINT(Bool u8Enable)
{
    if (u8Enable)
    {
        INT_STATUS_CLEAR();

        if (CURRENT_INPUT_IS_VGA())//(SrcInputType == Input_Analog1)
        {
            INT_SCALER_A(0xB8);
            INT_SCALER_B(0x00); // clock change
        }
        else
        {
#if ENABLE_FREESYNC
            if(IS_DP_FREESYNC())
                INT_SCALER_A(0x2C);
            else if(IS_HDMI_FREESYNC())
                INT_SCALER_A(0x1C);
            else
#endif
                INT_SCALER_A(0x3C); // for YCbCr

            INT_SCALER_B(BIT2); // clock change
        }
    }
    else
    {
        INT_SCALER_A(0x00);
        INT_SCALER_B(0x00);
        INT_STATUS_CLEAR();
    }
}
#endif

void mStar_EnableAutoNoSignal(Bool bEnable)
{
    BYTE u8SC00_BF = msReadByte(SC00_BF);
    BYTE u8SC00_E6 = msReadByte(SC00_E6);
    BYTE u8SC00_E7 = msReadByte(SC00_E7);
    BYTE u8SC00_F8 = msReadByte(SC00_F8);
    BYTE u8SC00_FA = msReadByte(SC00_FA);

    u8SC00_E6 &= ~0x3F;
    u8SC00_E7 &= ~0x1F;
    u8SC00_BF &= ~(_BIT5|_BIT4);
    u8SC00_F8 &= ~(_BIT7);
    u8SC00_FA &= ~(_BIT1);

    u8SC00_E6 |= (bEnable?((HPeriod_Torlance)&0x3F):3);
    u8SC00_E7 |= (bEnable?(BIT4|((VTotal_Torlance)&0x0F)):0);
    if(bEnable)
    {
        if(!IS_DP_FREESYNC())
        {
        #ifdef _HW_AUTO_NO_SIGNAL_
            u8SC00_F8 |= _BIT7;// hsync loss en(overflow)
        #if (CHIP_ID == CHIP_MT9701)
            u8SC00_BF |= _BIT4;// htt chg en
        #endif
        #endif
        }

        if(!IS_HDMI_FREESYNC())
        {
        #ifdef _HW_AUTO_NO_SIGNAL_
            u8SC00_FA |= _BIT1; // vsync width chg en
        #if (CHIP_ID == CHIP_MT9701)
            u8SC00_BF |= _BIT5; // vtt chg en
        #endif
        #endif
        }
    }
    msWriteByte(SC00_E6, u8SC00_E6); // [6:0] H peroid tolerance
    msWriteByte(SC00_F8, u8SC00_F8); // [7] hsync loss en(overflow)
    msWriteByte(SC00_FA, u8SC00_FA); // [1] vsync width chg en
#if (CHIP_ID == CHIP_MT9701)
    msWriteByte(SC00_BF, u8SC00_BF); // [5]vtt chg en, [4] htt chg en
#endif
    msWriteByte(SC00_E7, u8SC00_E7); // [4]enable auto no signal, [3:0] V total tolerance
}

Bool mStar_SetPanelTiming(void)
{
    if(appmStar_SetPanelTiming() == FALSE)
        return FALSE;

    return TRUE;
}

//*******************************************************************
// Function Name: mStar_SetupMode
//
// Decscription: setup registers for input timing,
// return      : TRUE,
// caller: mStar_SetupADC(), mStar_SetupCaptureWindow(),
//         mStar_SetScalingFactor(), mStar_SetPanelTiming(),
//         mStar_SetUserPref() in mstar.c
//         mStar_FineTuneDVIPhase(), mStar_ValidTimingDetect() in detect.c
//         mSar_WriteByte(), old_msReadByte() in ms_rwreg.c
// callee: mStar_ModeHandler() in detect.c
//*******************************************************************
Bool mStar_SetupMode( void )
{
    Clr_FreeRunModeFlag();
    Clr_BackToUnsupportFlag();
    Clr_BackToStandbyFlag();
#if ENABLE_VGA_INPUT
    mStar_SetupADC(); // setup ADC block, including polarity & htotal, phase, vco
#endif
    if( mStar_SetCaptureWindow() == FALSE )        // setup capture window
    {
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
        SetTimingChangeFirstPoint(TimingChange_CaptureWindow);
#endif
        return FALSE;
    }

    mStar_SetupPath();

    if( mStar_SetPanelTiming() == FALSE )        // set output dclk
    {
        MST_printMsg( "Dot clock --> not support" );
        SrcFlags |= bUnsupportMode;
        return TRUE;
    }

    if( InputTimingChangeFlag || mStar_ValidTimingDetect() )        // check if input timing has changed
    {
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
        SetTimingChangeFirstPoint(TimingChange_SetupMode);
#endif
        return FALSE;
    }

    // enable double buffer
    //mStar_ScalerDoubleBuffer(TRUE);

    // use interrupt to speedup mode changing while input timing is changing
#if UseINT
    mStar_EnableModeChangeINT(TRUE);
#endif
#ifdef _HW_AUTO_NO_SIGNAL_
     mStar_EnableAutoNoSignal(TRUE);
#endif
    mStar_InterlaceModeSetting();

    #if ENABLE_RTE
        msAPI_OverDriveEnable( UserprefOverDriveSwitch );
    #endif

#if BrightFreqByVfreq
    SetFourtimesPWMFreq(); // due to pwm period is not fixed, needs to set freq before period
#endif

    mStar_SetUserPref(); // restore user setting // 2006/10/16 12:38AM by Emily
    SetInputColorFormat();
    return TRUE;
}

//*******************************************************************
// Function Name: mStar_SetupFreeRunMode
//
// Decscription: setup registers for free run mode without any input timing,
//
// caller: mStar_ResetDClkPLL() in mstar.c
//         mSar_WriteByte(), old_msReadByte(), mStar_WriteWord() in ms_rwreg.c
// callee: mStar_ModeHandler() in detect.c
//*******************************************************************
void mStar_SetupFreeRunMode( void )
{
    drvmStar_SetupFreeRunMode();
    Set_FreeRunModeFlag();

#if BrightFreqByVfreq
    //SetPWM4Vfreq();
    SetPWMFreq( 240 );
#endif

#if ENABLE_FACTORY_SSCADJ // 2011-09-16 19:46 CC
    mStar_SetPanelSSC(FactorySetting.SSCModulation, FactorySetting.SSCPercentage);
#else
    if( g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_NONE )
        mStar_SetPanelSSC(PANEL_SSC_MODULATION_DEF, PANEL_SSC_PERCENTAGE_DEF);
    else
        mStar_SetPanelSSC(PANEL_SSC_MODULATION_DEF_VX1, PANEL_SSC_PERCENTAGE_DEF_VX1);
#endif

}


void mStar_ForceDeOnlyMode(BOOL bForce, BOOL bEn)
{
    bForceDeOnlyMode = bForce;
    bDeOnlyModeEn = bEn;

}

void mStar_SetIPMux(IP_MUX eIPMux)
{
    if( eIPMux == IP_MUX_ADC )
    {
        msWriteByteMask(SC00_03, 0, _BIT7); // YUV422 issue
        msWriteByteMask(SC00_A8, 0x00, _BIT1|_BIT0);

        msWriteByteMask(SC00_E8, 0, _BIT5|_BIT4|_BIT3|_BIT2); //user define HV polarity(bit5,bit3)
        msWriteByte(SC00_04, VSE_B);
        msWriteByteMask(REG_120F84, 0, _BIT6); //DE only mode for SC_TOP //recover
        msWriteByteMask( SC00_A4, 0, _BIT2); // disable Narrow Hsync

        msWriteByte(SC00_6F, 0x00); // Interlace mode line shift
        msWriteByteMask(SC00_E7, _BIT5, _BIT5); // Htt Filter
        msWriteByte(SC00_ED, 0x01); // enable ADC coast
        msWriteByte(SC00_EE, 0x03); // enable coast window start
        msWriteByte(SC00_EF, 0x01); // enable coast window end
        msWriteByte(SC00_F3, 0x00); // disable DE glitch removal function & more tolerance for DE
        msWriteByte(SC00_02, NIS_B|ISEL_ANALOG1);

        msWriteByteMask(REG_102E02, IP_MUX_ADC<<4, 0xF0);
        msWriteByteMask(REG_120F85, 0x00, 0x1F); // reg_ckg_idclk

    }
    else if( eIPMux == IP_MUX_DVI )
    {
        msWriteByteMask(SC00_03, 0, _BIT7); // YUV422 issue
        msWriteByteMask(SC00_A8, 0x00, _BIT1|_BIT0);
        msWriteByteMask(SC00_E8, 0, _BIT5|_BIT4|_BIT3|_BIT2); //user define HV polarity(bit5,bit3)

        if(bForceDeOnlyMode)
        {
            if(bDeOnlyModeEn)
            {
                msWriteByte( SC00_04, _BIT3|_BIT7|_BIT6);
                msWriteByteMask( REG_120F84, _BIT6, _BIT6); // enable DE-only mode
                msWriteByteMask( SC00_A4, 0, _BIT2); // disable Narrow Hsync
            }
            else
            {
                msWriteByte( SC00_04, _BIT3|_BIT7);
                msWriteByteMask( REG_120F84, 0, _BIT6); // disable DE-only mode
                msWriteByteMask( SC00_A4, _BIT2, _BIT2); // enable Narrow Hsync
            }
        }
        else
        {
            if(_TMDS_DE_MODE_EN_)
            {
                msWriteByte( SC00_04, _BIT3|_BIT7|_BIT6);
                msWriteByteMask( REG_120F84, _BIT6, _BIT6); // enable DE-only mode
                msWriteByteMask( SC00_A4, 0, _BIT2); // disable Narrow Hsync
            }
            else
            {
                msWriteByte( SC00_04, _BIT3|_BIT7);
                msWriteByteMask( REG_120F84, 0, _BIT6); // disable DE-only mode
                msWriteByteMask( SC00_A4, _BIT2, _BIT2); // enable Narrow Hsync
            }
        }
        msWriteByteMask(SC00_E7, _BIT5, _BIT5); // Htt Filter
        msWriteByte(SC00_EE, 0x01); // enable coast window start
        msWriteByte(SC00_EF, 0x00); // enable coast window end
        msWriteByte(SC00_F3, 0xF0); // enable DE glitch removal function & more tolerance for DE
        msWriteByte(SC00_02, NIS_B|ISEL_DVI|SCALER_STYPE_HV);

        msWriteByteMask(REG_102E02, IP_MUX_DVI<<4, 0xF0);
        msWriteByteMask(REG_120F85, 0x04, 0x1F); // reg_ckg_idclk
    }
    else if( eIPMux == IP_MUX_HDMI )
    {
    #if (CHIP_ID == CHIP_MT9701)
        msWriteByteMask(SC00_03, _BIT7, _BIT7);  // CHIP_MT9701 transfer YUV422 to YUV444 in HDMI
    #else
        msWriteByteMask(SC00_03, 0, _BIT7);  // YUV422 issue
    #endif

        msWriteByteMask(SC00_A8, 0x00, _BIT1|_BIT0);
        msWriteByteMask(SC00_E8, 0, _BIT5|_BIT4|_BIT3|_BIT2); //user define HV polarity(bit5,bit3)
        msWriteByteMask(SC00_7A, _BIT7, _BIT7); // [7]: HDMI 444 pixel repetition mode enable

        if(bForceDeOnlyMode)
        {
            if(bDeOnlyModeEn)
            {
                msWriteByte( SC00_04, _BIT3|_BIT7|_BIT6);
                msWriteByteMask( REG_120F84, _BIT6, _BIT6); // enable DE-only mode
                msWriteByteMask( SC00_A4, 0, _BIT2); // disable Narrow Hsync
            }
            else
            {
                msWriteByte( SC00_04, _BIT3|_BIT7);
                msWriteByteMask( REG_120F84, 0, _BIT6); // disable DE-only mode
                msWriteByteMask( SC00_A4, _BIT2, _BIT2); // enable Narrow Hsync
            }
        }
        else
        {
            if(_TMDS_DE_MODE_EN_&&(!ENABLE_FREESYNC))
            {
                msWriteByte( SC00_04, _BIT3|_BIT7|_BIT6);
                msWriteByteMask( REG_120F84, _BIT6, _BIT6); // enable DE-only mode
                msWriteByteMask( SC00_A4, 0, _BIT2); // disable Narrow Hsync
            }
            else
            {
                msWriteByte( SC00_04, _BIT3|_BIT7);
                msWriteByteMask( REG_120F84, 0, _BIT6); // disable DE-only mode
                msWriteByteMask( SC00_A4, _BIT2, _BIT2); // enable Narrow Hsync
            }
        }

        msWriteByteMask(SC00_E7, _BIT5, _BIT5); // Htt Filter
        msWriteByte(SC00_EE, 0x01); // enable coast window start
        msWriteByte(SC00_EF, 0x00); // enable coast window end
        msWriteByte(SC00_F3, 0xF0); // enable DE glitch removal function & more tolerance for DE
        msWriteByte(SC00_02, NIS_B|ISEL_HDMI_444|SCALER_STYPE_HV);

        msWriteByteMask(REG_102E02, IP_MUX_DVI<<4, 0xF0);
        msWriteByteMask(REG_120F85, 0x04, 0x1F); // reg_ckg_idclk
    }
    else // eIPMux == IP_MUX_DP
    {
        msWriteByte(SC00_02, NIS_B|SCALER_STYPE_HV|SCALER_ISEL_DP); // Set HV seperate
        msWriteByteMask(SC00_03, _BIT7, _BIT7);                                                                         // Force Input Clock Divide Function.
        msWriteByte( SC00_04, _BIT3|_BIT4|_BIT7);                                                                       //  Set HV mode
        msWriteByteMask(SC00_A6, 0, _BIT1);                                                                               // Dual in
        msWriteByteMask(SC00_A8, _BIT1|_BIT0, _BIT1|_BIT0);                                                     //Interlace method
        //msWriteByteMask(SC00_E6, _BIT6, _BIT6);   // DE Follow mode
        msWriteByteMask(SC00_E7, _BIT5, _BIT5);                                                                        // Htt Filter
        msWriteByteMask(SC00_E8, _BIT5|_BIT4|_BIT3|_BIT2, _BIT5|_BIT4|_BIT3|_BIT2); // Override H/H sync
        msWriteByte(SC00_EE, 0x00); // enable coast window start
        msWriteByte(SC00_EF, 0x00); // enable coast window end
        msWriteByte(SC00_F3, 0xF0);

        msWriteByteMask(REG_120F84, 0,_BIT6);
        msWriteByteMask( SC00_A4, _BIT2, _BIT2); // enable Narrow Hsync
        msWriteByteMask(REG_102E02, IP_MUX_DP<<4, 0xF0);
        msWriteByteMask(REG_120F85, 0x08, 0x1F);// reg_ckg_idclk
    }
}
//*******************************************************************
// Function Name: mStar_SetAnalogInputPort
//
// Decscription: setup registers for Separate Sync/Composite Sync/SOG,
//
// caller: mSar_WriteByte(), old_msReadByte() in ms_rwreg.c
//
// callee: mStar_MonitorInputTiming() in detect.c
//*******************************************************************
void mStar_SetAnalogInputPort( Bool ToSOGPort )
{
    BYTE regValue = SC0_READ_INPUT_SETTING();//old_msReadByte(SC0_02);

#if MS_VGA_SOG_EN
   if(!ToSOGPort)
    {
        regValue = regValue & 0x8F;
        g_bInputSOGFlag=0;
       if(bInputVGAisYUV) //20150121
        {
            //MST_printMsg(">>SetAnaPort>> ADC_TABLE_SOURCE_YUV_HV");
            drvADC_SetADCSource(ADC_TABLE_SOURCE_YUV_HV);  // input is YUV
        }
        else
        {
            //MST_printMsg(">>SetAnaPort>> ADC_TABLE_SOURCE_RGB");
            drvADC_SetADCSource(ADC_TABLE_SOURCE_RGB);   // input is RGB
        }
        SC0_ADC_COAST_ENABLE(0x01);//old_msWriteByte(SC0_ED, 0x01);   // enable ADC coast
        SC0_ADC_COAST_START(0x03);//old_msWriteByte(SC0_EE, 0x03); //0x00);   // enable coast window start
        SC0_ADC_COAST_END(0x01);//old_msWriteByte(SC0_EF, 0x01); //0x00);   // enable coast window end
        SC0_GLITCH_REMOVAL_ENABLE(0);//old_msWriteByte(SC0_F3, 0x00 ); //RD suggest 20081008
        ADC_PLL_LOCKING_EDGE(0);//old_msWriteByteMask(REG_ADC_DTOP_07_L,0,BIT5); // 0:Hsync leading edge; 1: Hsync trailing edge
        MST_printMsg("InputPort_____VGA");
    }
    else
    {
        regValue = regValue | 0x70;
        g_bInputSOGFlag=1;

        if(bInputVGAisYUV) //20150121
        {
            //MST_printMsg(">>SetAnaPort>> ADC_TABLE_SOURCE_YUV");
            drvADC_SetADCSource(ADC_TABLE_SOURCE_YUV);  // input is YUV
        }
        else
        {
            //MST_printMsg(">>SetAnaPort>> ADC_TABLE_SOURCE_SOG");
            drvADC_SetADCSource(ADC_TABLE_SOURCE_SOG);   // input is RGB
        }
        SC0_ADC_COAST_ENABLE(0x21);//old_msWriteByte(SC0_ED, 0x21);   // enable ADC coast
        SC0_ADC_COAST_START(0x0A);//old_msWriteByte(SC0_EE, 0x08);   // enable coast window start  //Al050814 Micro version need adjust this value
        SC0_ADC_COAST_END(0x0A);//old_msWriteByte(SC0_EF, 0x08 ); //0x05);   // enable coast window end //Jison 110317 follow CHIP_TSUMT
        SC0_GLITCH_REMOVAL_ENABLE(0);//old_msWriteByte( SC0_F3,  0x00 );
        ADC_PLL_LOCKING_EDGE(1);//old_msWriteByteMask(REG_ADC_DTOP_07_L,BIT5,BIT5); // 0:Hsync leading edge; 1: Hsync trailing edge
        MST_printMsg("InputPort_____SOG");
    }
#else
    ToSOGPort=ToSOGPort;
    regValue = regValue & 0x8F;
    g_bInputSOGFlag = 0;
    drvADC_SetADCSource(ADC_TABLE_SOURCE_RGB);
#endif

    SC0_RECOVER_INPUT_SETTING(regValue);//old_msWriteByte(SC0_02, regValue);

    ScalerReset(RST_IP);

    Delay1ms(80); // delay over 1 frame time to wait for status register is ready

}


//*******************************************************************
// Function Name: mStar_SetupInputPort
//
// Decscription: setup input port registers for
//               Analog/Digital/YCbCr(Video) input
//
// caller: mSar_WriteByte(), old_msReadByte() in ms_rwreg.c
// callee: mStar_MonitorInputTiming() in detect.c
//*******************************************************************
void mStar_SetupInputPort( void )
{
#if ENABLE_DP_INPUT
    static BYTE u8PreInputPort = Input_Nums;
#endif
#if (ENABLE_HDMI || ENABLE_DVI)
    static BYTE u8HDMIPreInputPort = Input_Nums;
    BOOL bIsHDMIPort_Chg = FALSE;
#endif
    MST_printData("SetupInputPort : %d", SrcInputType);
    g_CountSwitchPortTimeFlag=TRUE;

#if CHIP_ID == CHIP_TSUMU
    if(!g_SetupPathInfo.bOverrideSCFmtIn)
        g_SetupPathInfo.ucSCFmtIn = SC_FMT_IN_NORMAL;
    g_SetupPathInfo.ucIPLRSel = IP_3D_LR_FRAME_DET;
#elif CHIP_ID == CHIP_TSUM2
    if(!g_SetupPathInfo.bOverrideSCFmtIn)
        g_SetupPathInfo.ucSCFmtIn = SC_FMT_IN_NORMAL;
#endif




    Set_ShowInputInfoFlag();

    mStar_IPPowerControl();
#if (ENABLE_DVI_HDMI_SWITCH)
    if(SrcInputType == Input_Digital)
        SELECT_DVI();
    else
        SELECT_HDMI();
#endif
#if (CHIP_ID == CHIP_MT9700)
    msAPI_combo_IPPowerControl(SrcInputType);
#endif

#if (ENABLE_HDMI || ENABLE_DVI)
#if ENABLE_HPD_REPLACE_MODE
    if((!(CURRENT_INPUT_IS_DVI()))&&(!(CURRENT_INPUT_IS_HDMI())))
    {
        msAPI_combo_HDMIRx_MAC_HDCP_Enable(FALSE);//TMP
        msAPI_combo_HDMIRx_SCDC_config(u8HDMIPreInputPort,TRUE);
        msAPI_combo_HDMIRx_SCDC_Clr(u8HDMIPreInputPort);
        msAPI_combo_HDMIRx_DataRtermControl(u8HDMIPreInputPort,FALSE);

        #if (CHIP_ID==CHIP_MT9701)
            #if ENABLE_DVI
                msAPI_combo_HDMIRx_IRQ_Enable(COMBO_HDMI_IRQ_PHY, Input_DVI, HDMI_ACTIVE_CABLE_CR_LOCK, FALSE);
                msAPI_combo_HDMIRx_IRQ_Enable(COMBO_HDMI_IRQ_PM_SCDC, Input_DVI, 0x3, FALSE);
            #endif

            #if ENABLE_HDMI
                msAPI_combo_HDMIRx_IRQ_Enable(COMBO_HDMI_IRQ_PHY, Input_HDMI, HDMI_ACTIVE_CABLE_CR_LOCK, FALSE);
                msAPI_combo_HDMIRx_IRQ_Enable(COMBO_HDMI_IRQ_PM_SCDC, Input_HDMI, 0x3, FALSE);

                if (Input_HDMI2 != Input_Nothing) {
                    msAPI_combo_HDMIRx_IRQ_Enable(COMBO_HDMI_IRQ_PHY, Input_HDMI2, HDMI_ACTIVE_CABLE_CR_LOCK, FALSE);
                    msAPI_combo_HDMIRx_IRQ_Enable(COMBO_HDMI_IRQ_PM_SCDC, Input_HDMI2, 0x3, FALSE);
                }
            #endif
        #endif

    }
    else
    {
        #if (CHIP_ID==CHIP_MT9701)
            #if ENABLE_DVI
                if (SrcInputType == Input_DVI) {
                    msAPI_combo_HDMIRx_IRQ_Enable(COMBO_HDMI_IRQ_PHY, Input_DVI, HDMI_ACTIVE_CABLE_CR_LOCK, TRUE);
                    msAPI_combo_HDMIRx_IRQ_Enable(COMBO_HDMI_IRQ_PM_SCDC, Input_DVI, 0x3, TRUE);
                }
                else {
                    msAPI_combo_HDMIRx_IRQ_Enable(COMBO_HDMI_IRQ_PHY, Input_DVI, HDMI_ACTIVE_CABLE_CR_LOCK, FALSE);
                    msAPI_combo_HDMIRx_IRQ_Enable(COMBO_HDMI_IRQ_PM_SCDC, Input_DVI, 0x3, FALSE);
                }
            #endif

            #if ENABLE_HDMI
                if (SrcInputType == Input_HDMI) {
                    msAPI_combo_HDMIRx_IRQ_Enable(COMBO_HDMI_IRQ_PHY, Input_HDMI, HDMI_ACTIVE_CABLE_CR_LOCK, TRUE);
                    msAPI_combo_HDMIRx_IRQ_Enable(COMBO_HDMI_IRQ_PM_SCDC, Input_HDMI, 0x3, TRUE);
                }
                else {
                    msAPI_combo_HDMIRx_IRQ_Enable(COMBO_HDMI_IRQ_PHY, Input_HDMI, HDMI_ACTIVE_CABLE_CR_LOCK, FALSE);
                    msAPI_combo_HDMIRx_IRQ_Enable(COMBO_HDMI_IRQ_PM_SCDC, Input_HDMI, 0x3, FALSE);
                }
                if (SrcInputType == Input_HDMI2) {
                    msAPI_combo_HDMIRx_IRQ_Enable(COMBO_HDMI_IRQ_PHY, Input_HDMI2, HDMI_ACTIVE_CABLE_CR_LOCK, TRUE);
                    msAPI_combo_HDMIRx_IRQ_Enable(COMBO_HDMI_IRQ_PM_SCDC, Input_HDMI2, 0x3, TRUE);
                }
                else {
                    msAPI_combo_HDMIRx_IRQ_Enable(COMBO_HDMI_IRQ_PHY, Input_HDMI2, HDMI_ACTIVE_CABLE_CR_LOCK, FALSE);
                    msAPI_combo_HDMIRx_IRQ_Enable(COMBO_HDMI_IRQ_PM_SCDC, Input_HDMI2, 0x3, FALSE);
                }
            #endif
        #endif
    }
#else
    msAPI_combo_HDMIRx_DDCControl(Input_HDMI, FALSE);
    msAPI_combo_HDMIRx_DDCControl(Input_HDMI2, FALSE);
    msAPI_combo_HDMIRx_DDCControl(Input_HDMI3, FALSE);
    msAPI_combo_HDMIRx_DDCControl(Input_DVI, FALSE);
    msAPI_combo_HDMIRx_DDCControl(Input_DVI2, FALSE);
    msAPI_combo_HDMIRx_DDCControl(Input_DVI3, FALSE);
    msAPI_combo_HDMIRx_DataRtermControl(u8HDMIPreInputPort,FALSE);
#endif
    #if ENABLE_HDMI_BCHErrorIRQ
    msAPI_combo_HDMIRx_EnableHDMIRx_PKT_ParsrIRQ(FALSE);
    #endif
    //MST_printData("=== PreInputPort = 0x%x",u8HDMIPreInputPort);
    if(u8HDMIPreInputPort == SrcInputType)
    {
        bIsHDMIPort_Chg = FALSE;
    }
    else
    {
        bIsHDMIPort_Chg = TRUE;
    }
    u8HDMIPreInputPort = SrcInputType;
    //MST_printData("=== CurInputPort = 0x%x",u8HDMIPreInputPort);
#endif

#if ((CHIP_ID == CHIP_TSUMC) || (CHIP_ID == CHIP_TSUMD)||(CHIP_ID == CHIP_TSUM9)||(CHIP_ID == CHIP_TSUMF))
    if(!INPUT_IS_TMDS())
    {
         old_msWriteByteMask(REG_175D, 0x20, 0x20);//Mask HDMI IRQ
#if DHDMI_SIMPLAYHD_PATCH // SimplayHD CTS 8-18
         old_msWriteByteMask(REG_1509, 0x84, 0x84);
#endif
    }
    else
    {
        old_msWriteByteMask(REG_175D, 0x00, 0x20);
    }
#endif
#if ENABLE_VGA_INPUT
    if(CURRENT_INPUT_IS_VGA())//( SrcInputType < Input_Digital )
    {
        drvmStar_SetupInputPort_VGA();
        mStar_SetIPMux(IP_MUX_ADC);

#if (CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMD || CHIP_ID==CHIP_TSUM9|| CHIP_ID==CHIP_TSUMF)
        ComboInputControl(COMBO_INPUT_ANALOG);
#endif
    }
#endif


    #if (ENABLE_DP_INPUT == 0x1)
    mapi_DPRx_Switch_Port_Check(); // offline keep lock need check before hdmi
    #endif


#if ENABLE_HDMI
    if(CURRENT_INPUT_IS_DVI())//( SrcInputType == Input_Digital )
    {
#if ENABLE_HPD_REPLACE_MODE
        mdrv_hdmi_SetupInputPort(SrcInputType);
    #if (CHIP_ID==CHIP_MT9701)
        if(bIsHDMIPort_Chg)
    #endif
        {
            mdrv_hdmiRx_Software_Reset(SrcInputType, HDMI_SW_RESET_HDCP);
        }
#else
        msAPI_combo_IPControlHPD(SrcInputType,FALSE);
        mdrv_hdmi_SetupInputPort(SrcInputType);
        ForceDelay1ms(100);
        msAPI_combo_HDMIRx_ClockRtermControl(SrcInputType,FALSE);
        msAPI_combo_HDMIRx_DataRtermControl(SrcInputType,FALSE);
        mdrv_hdmiRx_Software_Reset(SrcInputType, HDMI_SW_RESET_HDCP);
        //drvmStar_SetupInputPort_DVI();
        ForceDelay1ms(500);
        msAPI_combo_HDMIRx_ClockRtermControl(SrcInputType,TRUE);
        msAPI_combo_HDMIRx_DataRtermControl(SrcInputType,TRUE);
        ForceDelay1ms(100);
        msAPI_combo_IPControlHPD(SrcInputType,TRUE);
#endif
        mStar_SetIPMux(IP_MUX_DVI);
    #if (CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMD || CHIP_ID==CHIP_TSUM9|| CHIP_ID==CHIP_TSUMF)
        ComboInputControl(COMBO_INPUT_DIGITAL);
    #endif
    }

    else if(CURRENT_INPUT_IS_HDMI())//(SrcInputType == Input_HDMI)
    {
#if ENABLE_HPD_REPLACE_MODE
        mdrv_hdmi_SetupInputPort(SrcInputType);
    #if (CHIP_ID==CHIP_MT9701)
        if(bIsHDMIPort_Chg)
    #endif
        {
            mdrv_hdmiRx_Software_Reset(SrcInputType, HDMI_SW_RESET_HDCP);
        }
#if(COMBO_HDCP2_FUNCTION_SUPPORT)
        mdrv_combo_SetHDCP2CallBackFunction(0, mdrv_tmds_HDCP2TxEventProc);
#endif
#else
        msAPI_combo_IPControlHPD(SrcInputType,FALSE);
        mdrv_hdmi_SetupInputPort(SrcInputType);
        ForceDelay1ms(100);
        msAPI_combo_HDMIRx_ClockRtermControl(SrcInputType,FALSE);
        msAPI_combo_HDMIRx_DataRtermControl(SrcInputType,FALSE);
        mdrv_hdmiRx_Software_Reset(SrcInputType, HDMI_SW_RESET_HDCP);
#if(COMBO_HDCP2_FUNCTION_SUPPORT)
        mdrv_combo_SetHDCP2CallBackFunction(0, mdrv_tmds_HDCP2TxEventProc);
#endif
        ForceDelay1ms(500);
        msAPI_combo_HDMIRx_ClockRtermControl(SrcInputType,TRUE);
        msAPI_combo_HDMIRx_DataRtermControl(SrcInputType,TRUE);
        ForceDelay1ms(100);
        msAPI_combo_IPControlHPD(SrcInputType,TRUE);

#endif
#if ENABLE_HDMI_BCHErrorIRQ
        msAPI_combo_HDMIRx_EnableHDMIRx_PKT_ParsrIRQ(TRUE);
#endif
        mStar_SetIPMux(IP_MUX_HDMI);
    #if (CHIP_ID==CHIP_TSUMC || CHIP_ID==CHIP_TSUMD || CHIP_ID==CHIP_TSUM9|| CHIP_ID==CHIP_TSUMF)
        ComboInputControl(COMBO_INPUT_DIGITAL);
    #endif
    }
#endif

    if(CURRENT_INPUT_IS_DISPLAYPORT())
    {
#if ENABLE_DP_INPUT
        mapi_DPRx_SetupInputPort_DisplayPort(SrcInputType);
        mdrv_combo_SetHDCP2CallBackFunction(0, mapi_DPRx_HDCP2TxEventProc);

        if(UserPrefMSTOnOff && (UserPrefDPVersion != DPVersionMenuItems_DP1_1))
        {
            SetMSTOnOff();
        }
#endif

        mStar_SetIPMux(IP_MUX_DP);
    }
#if ENABLE_DP_INPUT
    else
    {
        mapi_DPRx_Not_DisplayPort(SrcInputType);
    }
#endif

#if ENABLE_DP_INPUT
    u8PreInputPort = SrcInputType;
    //MST_printData("********************************* u8PreInputPort: %d\n", u8PreInputPort);
#endif

#if (DPC_USB_ERROR_COUNT_CONFIG_EN == 0x1)
    if((SrcInputType == Input_UsbTypeC3)||(SrcInputType == Input_UsbTypeC4))
    {
        //Following API must put after mapi_DPRx_SetupInputPort_DisplayPort
        MBX_TPC_EnterDPAltModeCheck();
    }
#endif

    ScalerReset(RST_IP);

#if ENABLE_HDCP
    //if(CURRENT_INPUT_IS_TMDS())//( SrcInputType == Input_Digital || SrcInputType == Input_Digital2 )
    //    msEnableHDCP(); //TBD
#endif

#if 0
    drvmbx_send_Video_TypeC_Port_CMD(CURRENT_INPUT_IS_USBTYPEC2()?0:(CURRENT_INPUT_IS_USBTYPEC3()?1:2));
#endif
}

#if Enable_Expansion
void IsSupportedAspRatioMode(WORD u16width, WORD u16height)
{
    WORD xdata M, N;
    WORD xdata u16OutputImgH, u16OutputImgV;

    //Frame buffer less mode
    if (CURRENT_IS_FBL())
    {   //4:3 & 16:9
        {
            M = 4;
            N = 3;
            if( ((DWORD)g_sPnlInfo.sPnlTiming.u16Width*N/M) < g_sPnlInfo.sPnlTiming.u16Height )
            {
                u16OutputImgH = g_sPnlInfo.sPnlTiming.u16Width;
                u16OutputImgV = ((DWORD)g_sPnlInfo.sPnlTiming.u16Width*N/M);
            }
            else
            {
                u16OutputImgH = ((DWORD)g_sPnlInfo.sPnlTiming.u16Height*M/N);
                u16OutputImgV = g_sPnlInfo.sPnlTiming.u16Height;
            }
            if(abs(u16OutputImgV-g_sPnlInfo.sPnlTiming.u16Height)>= 5)
            {
                Clr_Asp4_3EnableFlag();
                MST_printData(">>>>>>>--------------------Clr_Asp4_3EnableFlag==%x \r\n",0);
            }
            else
            {
                Set_Asp4_3EnableFlag();
                MST_printData(">>>>>>>--------------------Set_Asp4_3EnableFlag==%x \r\n",0);
            }
            M = 16;
            N = 9;
            if( ((DWORD)g_sPnlInfo.sPnlTiming.u16Width*N/M) < g_sPnlInfo.sPnlTiming.u16Height )
            {
                u16OutputImgH = g_sPnlInfo.sPnlTiming.u16Width;
                u16OutputImgV = ((DWORD)g_sPnlInfo.sPnlTiming.u16Width*N/M);
            }
            else
            {
                u16OutputImgH = ((DWORD)g_sPnlInfo.sPnlTiming.u16Height*M/N);
                u16OutputImgV = g_sPnlInfo.sPnlTiming.u16Height;
            }
            if(abs(u16OutputImgV-g_sPnlInfo.sPnlTiming.u16Height)>= 5)
            {
                Clr_Asp16_9EnableFlag();
                MST_printData(">>>>>>>--------------------Clr_Asp16_9EnableFlag==%x \r\n",0);

            }
            else
            {
                Set_Asp16_9EnableFlag();
                MST_printData(">>>>>>>--------------------Set_Asp16_9EnableFlag==%x \r\n",0);
            }
        }
        //1:1
        {
            u16OutputImgH = u16width;
            u16OutputImgV = u16height;
            if(abs(u16OutputImgV-g_sPnlInfo.sPnlTiming.u16Height)>= 5)
            {
                Clr_Asp1_1EnableFlag();
                MST_printData(">>>>>>>--------------------Clr_Asp1_1EnableFlag==%x \r\n",0);

            }
            else
            {
                Set_Asp1_1EnableFlag();
                MST_printData(">>>>>>>--------------------Set_Asp1_1EnableFlag==%x \r\n",0);
            }
        }
        //Fixed input ratio
        {
            if( (DWORD)g_sPnlInfo.sPnlTiming.u16Width*u16height >= (DWORD)g_sPnlInfo.sPnlTiming.u16Height*u16width )
            {
                u16OutputImgH = ((DWORD)u16width * g_sPnlInfo.sPnlTiming.u16Height / u16height);
                u16OutputImgV = g_sPnlInfo.sPnlTiming.u16Height;
            }
            else
            {
                u16OutputImgH = g_sPnlInfo.sPnlTiming.u16Width;
                u16OutputImgV = ((DWORD)u16height * g_sPnlInfo.sPnlTiming.u16Width / u16width);
            }
            if(abs(u16OutputImgV-g_sPnlInfo.sPnlTiming.u16Height)>= 5)
            {
                Clr_AspFixedInputRatioEnableFlag();
                MST_printData(">>>>>>>--------------------Clr_AspFixedInputRatioEnableFlag==%x \r\n",0);

            }
            else
            {
                Set_AspFixedInputRatioEnableFlag();
                MST_printData(">>>>>>>--------------------Set_AspFixedInputRatioEnableFlag==%x \r\n",0);
            }
        }
    }
    else
    {
        OverScanSetting.AspRatioEnableFlag=0x0F;
    };
    //MST_printData(">>>>>>>--------------------width==%x \r\n",u16width);
    //MST_printData(">>>>>>>--------------------height==%x \r\n",u16height);
    //MST_printData(">>>>>>>--------------------u16OutputImgH = %x \r\n", u16OutputImgH);
    //MST_printData(">>>>>>>--------------------u16OutputImgV = %x \r\n", u16OutputImgV);
}
#endif
void OverScanCheck(WORD CapWinHStart ,WORD CapWinVStart,WORD width, WORD height)
{
    WORD xdata OverScanHDisp, OverScanVDisp, OverScanHCap, OverScanVCap;
    WORD xdata M, N;

    if( (OverScanSetting.AspRatio == OVERSCAN_4_3)
     || (OverScanSetting.AspRatio == OVERSCAN_16_9) )
    {
        if(OverScanSetting.AspRatio == OVERSCAN_4_3)
        {
            M = 4;
            N = 3;
        }
        else //if(OverScanSetting.AspRatio == OVERSCAN_16_9)
        {
            M = 16;
            N = 9;
        }
        if( ((DWORD)g_sPnlInfo.sPnlTiming.u16Width*N/M) < g_sPnlInfo.sPnlTiming.u16Height )
        {
            OverScanSetting.OverScanH = g_sPnlInfo.sPnlTiming.u16Width;
            OverScanSetting.OverScanV = ((DWORD)g_sPnlInfo.sPnlTiming.u16Width*N/M);
        }
        else
        {
            OverScanSetting.OverScanH = ((DWORD)g_sPnlInfo.sPnlTiming.u16Height*M/N);
            OverScanSetting.OverScanV = g_sPnlInfo.sPnlTiming.u16Height;
        }
        OverScanHDisp = OverScanSetting.OverScanH + OverScanSetting.ExtH;
        OverScanVDisp = OverScanSetting.OverScanV + OverScanSetting.ExtV;
        if( OverScanSetting.ExtH )
        {
            OverScanHCap = ((DWORD)width * OverScanSetting.OverScanH / OverScanHDisp);
            CapWinHStart = UserPrefHStart + (width - OverScanHCap) / 2 / PANEL_H_DIV;
            width = OverScanHCap;
        }

        if( OverScanSetting.ExtV )
        {
            OverScanVCap = ((DWORD)height * OverScanSetting.OverScanV / OverScanVDisp);
            CapWinVStart = UserPrefVStart + (height - OverScanVCap) / 2;
            height = OverScanVCap;
        }
    }
    else if( OverScanSetting.AspRatio == OVERSCAN_1_1 )
    {
        OverScanSetting.OverScanH = width;
        OverScanSetting.OverScanV = height;
    }
    else if(OverScanSetting.AspRatio == OVERSCAN_FIXED) //Fixed Input Aspect Ratio
    {
        if( (DWORD)g_sPnlInfo.sPnlTiming.u16Width*height >= (DWORD)g_sPnlInfo.sPnlTiming.u16Height*width )
        {
            OverScanSetting.OverScanH = ((DWORD)width * g_sPnlInfo.sPnlTiming.u16Height / height);
            OverScanSetting.OverScanV = g_sPnlInfo.sPnlTiming.u16Height;
        }
        else
        {
            OverScanSetting.OverScanH = g_sPnlInfo.sPnlTiming.u16Width;
            if(width)
            {
                OverScanSetting.OverScanV = ((DWORD)height * g_sPnlInfo.sPnlTiming.u16Width / width);
            }
            else
            {
                MST_printMsg("width invalid!!");
            }
        }
    }
    else if( OverScanSetting.AspRatio == OVERSCAN_FIXED_RATIO )
    {
        OverScanSetting.OverScanH = (g_sPnlInfo.sPnlTiming.u16Width > width)?( width + ((DWORD)(g_sPnlInfo.sPnlTiming.u16Width - width) * OverScanSetting.ImageRatio) / 100 ):g_sPnlInfo.sPnlTiming.u16Width ;
        OverScanSetting.OverScanV = (g_sPnlInfo.sPnlTiming.u16Height > height)?( height + ((DWORD)(g_sPnlInfo.sPnlTiming.u16Height - height) * OverScanSetting.ImageRatio) / 100 ):g_sPnlInfo.sPnlTiming.u16Height ;
        OverScanHCap = ((DWORD)width * OverScanSetting.ScanRatio / 100);
        CapWinHStart = UserPrefHStart + (width - OverScanHCap) / 2 / PANEL_H_DIV;
        width = OverScanHCap;

        OverScanVCap = ((DWORD)height * OverScanSetting.ScanRatio / 100);
        CapWinVStart = UserPrefVStart + (height - OverScanVCap) / 2;
        height = OverScanVCap;
    }
    else if( OverScanSetting.AspRatio == OVERSCAN_FULL )
    {
        OverScanSetting.OverScanV=g_sPnlInfo.sPnlTiming.u16Height;
        OverScanSetting.OverScanH=g_sPnlInfo.sPnlTiming.u16Width;
    }

    #if (FRAME_BFF_SEL == FRAME_BFFLESS)
    //if output image V size is smaller than panelHight with FBL mode , set output image size to panel size.
    if (CURRENT_IS_FBL())
    {
        if(abs(OverScanSetting.OverScanV-g_sPnlInfo.sPnlTiming.u16Height)>= 5)
        {
            OverScanSetting.OverScanH = g_sPnlInfo.sPnlTiming.u16Width;
            OverScanSetting.OverScanV = g_sPnlInfo.sPnlTiming.u16Height;
        }
    }
    #endif
    width &= (~BIT0);
    OverScanSetting.FinalCapWinHST=CapWinHStart;
    OverScanSetting.FinalCapWinVST=CapWinVStart;
    OverScanSetting.FinalCapWinWidth=width;
    OverScanSetting.FinalCapWinHeight=height;

    MST_printData("--------------------CapWinHStart = %x", CapWinHStart);
    MST_printData("--------------------CapWinVStart = %x", CapWinVStart);
    MST_printData("--------------------width = %x", width);
    MST_printData("--------------------height = %x", height);
}
//*******************************************************************
// Function Name: mStar_SetCaptureWindow
//
// Decscription: setup input capture window for display
//
// caller: mSar_WriteWord(), mStar_ReadWord() in ms_rwreg.c
///
// callee: mStar_SetupMode() in mstar.c
//*******************************************************************
Bool mStar_SetCaptureWindow( void )
{
    WORD xdata width, height;
    MST_printMsg(">>>>>>>>mStar_SetCaptureWindow >>>>>>>>.");
    if(
        CURRENT_INPUT_IS_TMDS()//SrcInputType == Input_Digital || SrcInputType == Input_Digital2
#if 1 //ENABLE_DP_INPUT
        || CURRENT_INPUT_IS_DISPLAYPORT()//SrcInputType == Input_Displayport
#endif
    )
    {
#if ENABLE_CHECK_TMDS_DE
        if( CURRENT_INPUT_IS_TMDS() )
        {
            UserPrefHStart = SrcTMDSHStart; // get hsync DE start
            UserPrefVStart = SrcTMDSVStart; // get vsync DE start
        }
        else
        {
            UserPrefHStart = SC0_READ_AUTO_START_H();//old_msRead2Byte(SC0_80); // get hsync DE start
            UserPrefVStart = SC0_READ_AUTO_START_V();//old_msRead2Byte(SC0_7E); // get vsync DE start
        }
#else
        UserPrefHStart = SC0_READ_AUTO_START_H();//old_msRead2Byte(SC0_80); // get hsync DE start
        UserPrefVStart = SC0_READ_AUTO_START_V();//old_msRead2Byte(SC0_7E); // get vsync DE start
#endif

        width = GetImageWidth(); // get DE width
        height = GetImageHeight(); // get DE Height

         if(CURRENT_SOURCE_IS_INTERLACE_MODE())
            {
                if(SC0_DE_ONLY_MODE() == TRUE)//(old_scReadByte(SC0_04)&BIT6) //DE only mode
                {
                    if(SC0_VIDEO_FIELD_INVERSION() == FALSE)//((old_scReadByte(SC0_E9)&BIT3)==0) //video field invert
                        height++;  //height=(Vend �V Vstart + 2);
                    else
                        height-=3; //height=(Vend �V Vstart - 2);
                }
                else
                {
                    height=SC0_READ_AUTO_END_V()-SC0_READ_AUTO_START_V();

                    if( msReadByte(SC00_E9)&BIT1)
                    {
                    #if (CHIP_ID == CHIP_MT9701)
                        height+=2;
                        UserPrefVStart = SC0_READ_AUTO_START_V();//msRead2Byte(SC0_7E); // get vsync DE start
                    #else
                        UserPrefVStart = SC0_READ_AUTO_START_V()|BIT0;//msRead2Byte(SC0_7E); // get vsync DE start
                    #endif
                    }
                    else
                    {
                        UserPrefVStart = SC0_READ_AUTO_START_V()|BIT0;//msRead2Byte(SC0_7E); // get vsync DE start
                    }
                }

            }
    }
    else
    {
        // input 1360x768 display on 1366 panel,
        // don't do sacle and get 1366 for hori. data
        if( g_sPnlInfo.sPnlTiming.u16Width == 1366 )
        {
            if( StandardModeGroup == Res_1360x768 )
                width = 1366;
            else
                width = StandardModeWidth;
        }
        else
        {
            width = StandardModeWidth;
        }


        height = SC0_READ_AUTO_HEIGHT();//old_msRead2Byte(SC0_82)-old_msRead2Byte(SC0_7E)+1; // get DE Height

        if( StandardModeGroup == Res_1152x864 )
        {
            height = 864;
        }
        else if((( StandardModeGroup == Res_720x480 ) && ( abs( StandardModeVFreq - 599 ) < 10 ) && ( abs( StandardModeHFreq - 157 ) < 10 ) )
                || (( StandardModeGroup == Res_720x576 ) && ( abs( StandardModeVFreq - 500 ) < 10 ) && ( abs( StandardModeHFreq - 156 ) < 10 ) )
                || (( StandardModeGroup == Res_1920x1080 ) && ( abs( StandardModeVFreq - 600 ) < 10 ) && ( abs( StandardModeHFreq - 337 ) < 10 ) )
                || (( StandardModeGroup == Res_1920x1080 ) && ( abs( StandardModeVFreq - 500 ) < 10 ) && ( abs( StandardModeHFreq - 281 ) < 10 ) ) ) // for VGA InterlaceMode
        {
            height = StandardModeHeight + 4;
        }
        else
        {
            height = StandardModeHeight;
        }
    }
#if DECREASE_V_SCALING
    //if(!OverScanFlag)
        height += DecVScaleValue;
#endif

    OverScanCheck(UserPrefHStart ,UserPrefVStart,width, height);

    if( width == g_sPnlInfo.sPnlTiming.u16Width && height == g_sPnlInfo.sPnlTiming.u16Height ) // set native mode flag
        SrcFlags |= bNativeMode;

    mStar_AdjustHPosition(OverScanSetting.FinalCapWinHST);
    mStar_AdjustVPosition(OverScanSetting.FinalCapWinVST);

    SC0_SET_IMAGE_HEIGHT(OverScanSetting.FinalCapWinHeight);
    SC0_SET_IMAGE_WIDTH(OverScanSetting.FinalCapWinWidth/(1+Input420Flag));

    return TRUE;
}
//*******************************************************************
// Function Name: mStar_SetupClampingByMode
//
// Decscription: When input timing is reduced blanking, we have to reduce the
//   duration of ADC calibration to avoid dark display.
//    The amount of calibration duration must be smaller than back-porch.
//    Currently, we set the calibration duration to H_Back_Porch/2.
// caller: mSar_WriteByte(), old_msReadByte() in ms_rwreg.c
///
// callee: mStar_SetupMode() in mstar.c
//*******************************************************************
void mStar_SetupClampingByMode(void)
{


    if (CURRENT_INPUT_IS_VGA())//(SrcInputType< Input_Digital)
    {
        if(g_bInputSOGFlag)  //Steven101221
        {
            switch(SrcModeIndex)
            {
                case MD_720x480I_60_YCbCr:
          case MD_720x480I_60_YCbCr_1:
                {
                    drvADC_AdjustCalDuring(15);  //Back-porch=57 pixel
                    break;
                }
                #if 0  //Steven110608, disable, Update TSUMT_ADC_Driver table 0.8 to fix some timing clamping issue.
                case MD_720X576I_50:
                {
                    drvADC_AdjustCalDuring(31);  //Back-porch=69 pixel
                    break;
                }
                case MD_720X480P_60:
                {
                    drvADC_AdjustCalDuring(60/2); //Back-porch=60 pixel
                    break;
                }
                case MD_640X480P_60:
                {
                    drvADC_AdjustCalDuring(48/2); //Back-porch=48 pixel
                    break;
                }
                case MD_720X576P_50:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=68 pixel
                    break;
                }
                #endif
                //================
                case MD_640x480_60_VESA:
                {
                    drvADC_AdjustCalDuring(7); //Back-porch=25 pixel
                    break;
                }
                case MD_640x350_70_IBM:
                {
                    drvADC_AdjustCalDuring(7); //Back-porch=48 pixel
                    break;
                }
                case MD_720x400_70:
                {
                    drvADC_AdjustCalDuring(15); //Back-porch=48 pixel
                    break;
                }
                #if 0  //Steven110608, disable, Update TSUMT_ADC_Driver table 0.8 to fix some timing clamping issue.
                case MD_800X600_72:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=64 pixel
                    break;
                }
                #endif
                case MD_1280x768R_60_CVT:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=80 pixel
                    break;
                }
                case MD_1280x800R_60_CVT:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=80 pixel
                    break;
                }
                case MD_1440x900R_60_CVT:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=80 pixel
                    break;
                }
                //#if PANEL_WIDTH==1600&&PANEL_HEIGHT==900
                case MD_1600X900R_60_VESA:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=80 pixel
                    break;
                }
                //#endif
                case MD_1680x1050R_60_CVT:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=80 pixel
                    break;
                }
                case MD_1920x1200R_60_CVT:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=80 pixel
                    break;
                }

        #if  0
         case MD_1920X1200_50R:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=80 pixel
                    break;
                }
          case MD_1600X1000_60R:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=80 pixel
                    break;
                }
                case MD_2048X1152_60R:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=80 pixel
                    break;
                }
                 case MD_1920X1080_60R:
                {
                    drvADC_AdjustCalDuring(31); //Back-porch=80 pixel
                    break;
                }
        #endif

            }
        }
    }
}
//*******************************************************************
// Function Name: mStar_SetupADC
//
// Decscription: setup ADC bandwidth/filter, clock, phase for sampling input data
//               and R/G/B gains, offsets
// caller: mSar_WriteByte(), old_msReadByte() in ms_rwreg.c
///
// callee: mStar_SetupMode() in mstar.c
//*******************************************************************
void mStar_SetupADC( void )
{
    WORD tempValue;

    if(!CURRENT_INPUT_IS_VGA())//(SrcInputType > Input_Analog1)
        return;

    tempValue = HFreq( SrcHPeriod );//(( DWORD )MST_CLOCK_MHZ * 10 + SrcHPeriod / 2 ) / SrcHPeriod; //calculate hfreq: round 5
    tempValue = (( DWORD )tempValue * UserPrefHTotal + 5000 ) / 10000; //dclk= hfreq * htotal

#if DADCPLLPROTECTION
    drvADC_SetModewithPLLProtection(g_bInputSOGFlag ? ADC_INPUTSOURCE_YPBPR : ADC_INPUTSOURCE_RGB, tempValue, UserPrefHTotal);
#else
    drvADC_AdjustHTotal(UserPrefHTotal);

    drvADC_SetADCModeSetting((g_bInputSOGFlag ? ADC_INPUTSOURCE_YPBPR : ADC_INPUTSOURCE_RGB), tempValue);
#endif
    //drvADC_SetPhaseCode((UPPHASE_GET_REAL_VALUE()+1)%MAX_PHASE_VALUE); //Jison 100818 patch for phase state machine reset
    drvADC_SetPhaseCode(UserPrefPhase); //Jison 100818
    drvADC_SetupHsyncPolarity((SrcFlags & bHSyncNegative)); //Jison 100818

    // wait for checking
    drvADC_SetRGBGainCode(UserPrefAdcRedGain, UserPrefAdcGreenGain, UserPrefAdcBlueGain);
    drvADC_SetRGBOffsetCode(UserPrefAdcRedOffset, UserPrefAdcGreenOffset, UserPrefAdcBlueOffset);
    //drvADC_AdjustCalDuring(7);

    mStar_SetupClampingByMode(); //Jison 110106
    //msADC_AdjustCalDuring( 80/2 );  // unit: pixel // coding temp setting
}

#if CHIP_ID == CHIP_MT9701
void PowerDownDDR(void)
{
     //DDR ATOP
     msWriteByteMask(REG_122154, 0x70, 0x70);
     msWriteBit(REG_122184, BIT3, BIT3);
     msWriteBit(REG_12218B, BIT1, BIT1);
     msWriteByteMask(REG_122133, 0xC0, 0xC0);
     msWriteBit(REG_122160, BIT1, BIT1);
}
#endif

//*******************************************************************
// Function Name: mStar_PowerUp
//
// Decscription: Power on chip from power down mode
//
// callee: old_msWriteByte() in ms_rwreg.c
//
// caller: Power_PowerOnSystem() in power.c
//*******************************************************************
void mStar_PowerUp( void )
{
#if !(MS_PM)
    MPLL_POWER_UP(TRUE);
    LPLL_POWER_UP(TRUE);
    MPLL_CLOCK_ADC(TRUE);
    mcuSetSystemSpeed(SPEED_NORMAL_MODE);
#endif
    SC0_SCALER_POWER_DOWN(BIT6, 0xFF); // power on chip from power down mode

    ScalerReset(RST_IP);

#if ENABLE_DAC // wait for coding
    msAPI_AudioDACPowerDown( FALSE );
#endif

    // 120229 coding check tsum2
    WRITE_POWER_ON_TABLE();

    mStar_IPPowerControl();

    MST_printMsg("PowerUp");

}

//*******************************************************************
// Function Name: mStar_PowerDown
//
// Decscription: Power down chip
//
// callee: old_msWriteByte() in ms_rwreg.c
//
// caller: Power_PowerOffSystem() in power.c
//*******************************************************************
void mStar_PowerDown( void )
{
#if ENABLE_RTE // wait for coding
    msAPI_OverDriveEnable( FALSE); // disable OD function , or you can set BK4_90h= 0 directly.
#endif

#if ENABLE_DAC
    msAPI_AudioDACPowerDown( TRUE );
#endif

#if ENABLE_VGA_INPUT
    drvADC_PowerCtrl((PowerOnFlag ? ADC_POWER_STANDBY : ADC_POWER_DOWN));
#endif
    drvDVI_PowerCtrl((PowerOnFlag ? DVI_POWER_STANDBY : DVI_POWER_DOWN));

  #if UseINT
    mStar_EnableModeChangeINT(FALSE); //Jison: for DDCCI,DVI input, reduced power off,it wake up immediately.
  #endif

#if (MS_PM)
    SC0_SCALER_POWER_DOWN( PDDS_B | BIT5 | BIT2 | 0x2 | BIT6, 0xFF ); // power down chip except mode detection
#else
    SC0_SCALER_POWER_DOWN( PDDS_B | BIT5 | BIT2 | 0x3 | BIT6, 0xFF ); // power down chip except mode detection
    //Jison, Don't enable if use PM mode, it will cause sometime DDCCI_ID is disabled by int 0
    //if (DDCCI_FORCE_POWER_SAVE_FLAG && (SRC_INPUT_TYPE==INPUT_DVI || SRC_INPUT_TYPE==INPUT_HDMI))
    //old_msWriteByte(SC0_CF, BIT2); //Enable DVI clock change int 090710 // 110928 coding check with Jison
#endif

#if CHIP_ID == CHIP_MT9701
    //PowerDownDDR();
#endif

#if !(MS_PM)
    ///mcuSetSystemSpeed(SPEED_XTAL_MODE);     //111012 Modify,
    MPLL_POWER_UP(FALSE);
    LPLL_POWER_UP(FALSE);
    MPLL_CLOCK_ADC(FALSE);
#endif

    MST_printMsg("PowerDown");
}
//*******************************************************************
// Function Name: mStar_InitADC
//
// Decscription: Initialize ADC bank registers
//
// callee: old_msWriteByte() in ms_rwreg.c
//
// caller: mStar_Init() in mStar.c
//*******************************************************************
void mStar_InitADC( void )
{
#if ENABLE_VGA_INPUT
    msADC_init();
#endif
}


//==================================================================
extern BYTE code t_MWEDLC_Linear_Table[];
//extern BYTE code t_Normal_ColorSettingTable[];
extern code short t_Normal_ColorSettingTable[][3];

//*******************************************************************
// Function Name: mStar_SetUserPref
//
// Decscription: Restore user setting from NVRAM
//
// callee: old_msWriteByte() in ms_rwreg.c
//
// caller: mStar_SetupMode() in mStar.c
//*******************************************************************
void mStar_SetUserPref( void )
{
    if( UserPrefDcrMode )
    {
        UserPrefECOMode = ECO_Standard;
        ReadColorTempSetting();

        msAccSetup( 0, g_sPnlInfo.sPnlTiming.u16Width, 0, g_sPnlInfo.sPnlTiming.u16Height );
#if ENABLE_DLC
        msDlcInit( g_sPnlInfo.sPnlTiming.u16Width, g_sPnlInfo.sPnlTiming.u16Height );
        msSetDlcStrength( g_LowStrength, g_HighStrength );
        LoadDLCTable( t_MWEDLC_Linear_Table );
#endif
        msAccOnOff( _DISABLE );     //111021 Rick check - B39434
#if ENABLE_DLC
        msDlcOnOff( _ENABLE );
#endif
    }
#if DDCCI_ENABLE
    mStar_AdjustUserPrefBlacklevel( UserprefRedBlackLevel, UserprefGreenBlackLevel, UserprefBlueBlackLevel );
#endif

#if ENABLE_LOCALDIMMING
    msLDInit();
#endif

    LoadACETable( MAIN_WINDOW, (BYTE *)t_Normal_ColorSettingTable, DefHue, DefSaturation, UserPrefContrast); // /UserPrefSubContrast
    LoadACETable( SUB_WINDOW, (BYTE *)t_Normal_ColorSettingTable, DefHue, DefSaturation, UserPrefContrast); // /UserPrefSubContrast
#if ENABLE_DLC
    msDCROnOff(UserPrefDcrMode, MAIN_WINDOW);
#endif
    SetECO(); //zhifeng.wu

    //Set Color Mode
    if (UserPrefColorMode == ColorModeMenuItems_Enhance)
        SetColorModeEnhance();
    else if (UserPrefColorMode == ColorModeMenuItems_Demo)
        SetColorModeDemo();
    else
    {
        SetColorModeOFF();
        UserPrefGamaMode= FALSE;
    }

    //Enable HSY
    #if HSY_MODE
    #if (CHIP_ID == CHIP_MT9700)
    msAPI_Adjust_ICC_Init();
    #else//(CHIP_ID == CHIP_MT9701)
    msAPI_HSY_inital(GEN_BY_HW);
    #endif
    #else
    msAPI_CustomHueEnable(MAIN_WINDOW, TRUE);
    msAPI_CustomSatEnable(MAIN_WINDOW, TRUE);
    msAPI_CustomBriEnable(MAIN_WINDOW, TRUE);
    #endif

    #if HSY_MODE
    //Set HSY
    AdjustAllIndependentHSYValue();
    #endif

    //Color Temp
    if(UserPrefColorTemp==CTEMP_USER)
    {
        msAPI_AdjustRGBColor(MAIN_WINDOW, UserPrefContrast, UserPrefRedColorUser, UserPrefGreenColorUser, UserPrefBlueColorUser);
        msAdjustHSC(MAIN_WINDOW, UserPrefHue, UserPrefSaturation, UserPrefContrast);

    }
    else
    {
        ReadColorTempSetting();
    }
}

void SetPWMFreq(WORD freq)
{
    drvGPIO_SetPWMFreq(BrightnessPWM, freq);
}
#if BrightFreqByVfreq
void SetFourtimesPWMFreq( void )
{
    WORD outVfreq;

#if CHIP_ID == CHIP_TSUMU || CHIP_ID == CHIP_TSUM2 || CHIP_ID == CHIP_TSUMD
    if(g_SetupPathInfo.bFBMode)
    {
        outVfreq = (WORD)GetVfreq()*4;
    }
    else
#endif
    {
        outVfreq = V_FREQ_IN*4;
    }

    SetPWMFreq( outVfreq );

    MST_printData("  BrightFreqByVfreq:%d", outVfreq);
}
#endif

void mStar_BlackWhiteScreenCtrl(BYTE u8Ctrl)
{
    if (u8Ctrl==BW_SCREEN_WHITE)
        SC0_WHITE_SCREEN_ENABLE();//old_msWriteByteMask(SC0_43,BIT5,(BIT4|BIT5));
    else if (u8Ctrl==BW_SCREEN_BLACK)
        SC0_BLACK_SCREEN_ENABLE();//old_msWriteByteMask(SC0_43,BIT4,(BIT4|BIT5));
    else
        SC0_BLACK_WHITE_SCREEN_DISABLE();//old_msWriteByteMask(SC0_43,0,(BIT4|BIT5));
}

void SetInputCombColorFormat(ST_COMBO_COLOR_FORMAT cf)
{
    _colorFormat = cf;
}

ST_COMBO_COLOR_FORMAT GetInputCombColorFormat(void)
{
    return _colorFormat;
}
BYTE IsColorspaceRGBInput()
{
    BYTE  ColorspaceRGB=FALSE;
    xdata ST_COMBO_COLOR_FORMAT cf;
    SetInputCombColorFormat(msAPI_combo_IPGetColorFormat(SrcInputType)); //msAPI_combo_IPGetColorFormat((EN_COMBO_IP_SELECT)(msGetHDRPortIPMuxByWin(u8WinIdx)));
    cf = GetInputCombColorFormat();
    if(cf.ucColorType == COMBO_COLOR_FORMAT_RGB)
    ColorspaceRGB = TRUE;

    if(UserPrefInputColorFormat == INPUTCOLOR_YUV)
	 ColorspaceRGB=FALSE;
    else if (UserPrefInputColorFormat == INPUTCOLOR_RGB)
	 ColorspaceRGB=TRUE;

#if  0//ENABLE_DP_INPUT
    //if(CURRENT_INPUT_IS_DISPLAYPORT()&&(( gDPInfo.ucDPColorFormat  )!=InputColor_RGB))
    if(CURRENT_INPUT_IS_DISPLAYPORT()&&mapi_DPRx_GetColorSpace(0)!=DP_COLOR_FORMAT_RGB )
        ColorspaceRGB=FALSE;
#endif
#if 0//ENABLE_HDMI
    //if(SrcInputType==Input_HDMI&&(gScInfo.InputColor!=InputColor_RGB))
#if PANEL_3D_PASSIVE_4M
    if(CURRENT_INPUT_IS_HDMI())//(SrcInputType==Input_Digital)
#else
    if(CURRENT_INPUT_IS_TMDS() && (stHDMIRxInfo.stHDRInfo.ucColorFormat!=HDMI_COLOR_FORMAT_RGB444))
#endif
        ColorspaceRGB=FALSE;
#endif

#if (CHIP_ID==CHIP_TSUMC||CHIP_ID==CHIP_TSUMD)
    msWriteByteMask(SC0B_A0, (g_SetupPathInfo.bMRW422==1 || ColorspaceRGB==FALSE)?(BIT0|BIT4):(0), BIT0|BIT4);
#elif (CHIP_ID==CHIP_TSUMJ||CHIP_ID==CHIP_TSUM9||CHIP_ID==CHIP_TSUMF)
    if ((msReadByte(SC07_40) & BIT0) || (msReadByte(SC07_40) & BIT4))
    {
        msWriteByteMask(SC0B_A0, BIT0|BIT4, BIT0|BIT4);
    }
    else
    {
        msWriteByteMask(SC0B_A0, (ColorspaceRGB==FALSE)?(BIT0|BIT4):(0), BIT0|BIT4);
    }
#elif ((CHIP_ID==CHIP_MT9700 || CHIP_ID==CHIP_MT9701) && ENABLE_FULL_RGB_COLOR_PATH)
	msWriteByteMask(SC0B_A0, BIT0|BIT4, BIT0|BIT4);
#endif

    return ColorspaceRGB;
}

BYTE IsColorspaceRGB(void)
{
    BYTE ColorspaceRGB = TRUE;

    ColorspaceRGB = IsColorspaceRGBInput();

    return  ColorspaceRGB;
}

void SettingInputColorimetry(void)
{
    xdata ST_COMBO_COLOR_FORMAT cf;
    SetInputCombColorFormat(msAPI_combo_IPGetColorFormat(SrcInputType)); //msAPI_combo_IPGetColorFormat((EN_COMBO_IP_SELECT)(msGetHDRPortIPMuxByWin(u8WinIdx)));
    cf = GetInputCombColorFormat();

    MST_printData("Pixel Format(RGB/422/444/420) = %d", cf.ucColorType);
    MST_printData("Color Range(Dft/Limit/Full) = %d", cf.ucColorRange);
    MST_printData("Yuv Colorimetry(601/709/Nodata/EC) = %d", cf.ucYuvColorimetry);
    MST_printData("Extend Colorimetry(601/709/s601/adobe601/adobeRGB/2020CL/2020NCL) = %d", cf.ucColorimetry);

#if ENABLE_FULL_RGB_COLOR_PATH
    msACECSCControl( );
    msACESetHDTVMode(ACE_YUV_TO_RGB_MATRIX_HDTV_LIMIT);
#else


    if(cf.ucColorType == COMBO_COLOR_FORMAT_RGB) // force to YUV709 when input source is RGB
        cf.ucYuvColorimetry = COMBO_YUV_COLORIMETRY_ITU709;

    switch (cf.ucYuvColorimetry)
        {
            default:

            case COMBO_YUV_COLORIMETRY_ITU709:
            {
                    MST_printData("1: Colorimetry %d use HDTV Matrix\n",cf.ucColorimetry);
                    if(cf.ucColorRange == COMBO_COLOR_RANGE_LIMIT ||(cf.ucColorType == COMBO_COLOR_FORMAT_RGB) )
                    {
                        #if ENABLE_HDR
                        if(mapi_Adjust_EnableNonStdCSC_Get() && (msGetHDRStatus(MAIN_WINDOW) == HDR_OFF))
                        #else
                        if(mapi_Adjust_EnableNonStdCSC_Get())
                        #endif
                        {
                            msACESetHDTVMode(ACE_YUV_TO_RGB_MATRIX_HDTV_LIMIT_NONSTD);//msACESelectYUVtoRGBMatrix(ucWinIndex, ACE_YUV_TO_RGB_MATRIX_HDTV_LIMIT_NONSTD, 0);
                        }
                        else
                        {
                            msACESetHDTVMode(ACE_YUV_TO_RGB_MATRIX_HDTV_LIMIT);//msACESelectYUVtoRGBMatrix(ucWinIndex, ACE_YUV_TO_RGB_MATRIX_HDTV_LIMIT, 0);
                        }
                    }
                    else
                    {
                        msACESetHDTVMode(ACE_YUV_TO_RGB_MATRIX_HDTV);//msACESelectYUVtoRGBMatrix(ucWinIndex, ACE_YUV_TO_RGB_MATRIX_HDTV, 0);
                    }
           }
           break;

            case COMBO_YUV_COLORIMETRY_ITU601:
           {
                    MST_printData("0: Colorimetry %d use SDTV  Matrix\n",cf.ucColorimetry);
                    if(cf.ucColorRange == COMBO_COLOR_RANGE_LIMIT)
                    {
                        #if ENABLE_HDR
                        if(mapi_Adjust_EnableNonStdCSC_Get() && (msGetHDRStatus(MAIN_WINDOW) == HDR_OFF))
                        #else
                        if(mapi_Adjust_EnableNonStdCSC_Get())
                        #endif
                        {
                            msACESetHDTVMode(ACE_YUV_TO_RGB_MATRIX_SDTV_LIMIT_NONSTD);//msACESelectYUVtoRGBMatrix(ucWinIndex, ACE_YUV_TO_RGB_MATRIX_SDTV_LIMIT_NONSTD, 0);
                        }
                        else
                        {
                            msACESetHDTVMode(ACE_YUV_TO_RGB_MATRIX_SDTV_LIMIT);//msACESelectYUVtoRGBMatrix(ucWinIndex, ACE_YUV_TO_RGB_MATRIX_SDTV_LIMIT, 0);
                        }
                    }
                    else
                    {
                        msACESetHDTVMode(ACE_YUV_TO_RGB_MATRIX_SDTV);//msACESelectYUVtoRGBMatrix(ucWinIndex, ACE_YUV_TO_RGB_MATRIX_SDTV, 0);
                    }
           }
           break;

            case COMBO_YUV_COLORIMETRY_EC:
            {
                    MST_printData("1: ucYuvColorimetry %d use NoData  Matrix\n",cf.ucYuvColorimetry);
                    if(cf.ucColorimetry == COMBO_COLORIMETRY_xvYCC601)
                    {
                        if(cf.ucColorRange == COMBO_COLOR_RANGE_LIMIT)
                        {
                            #if ENABLE_HDR
                            if(mapi_Adjust_EnableNonStdCSC_Get() && (msGetHDRStatus(MAIN_WINDOW) == HDR_OFF))
                            #else
                            if(mapi_Adjust_EnableNonStdCSC_Get())
                            #endif
                            {
                                msACESetHDTVMode(ACE_YUV_TO_RGB_MATRIX_SDTV_LIMIT_NONSTD);//msACESelectYUVtoRGBMatrix(ucWinIndex, ACE_YUV_TO_RGB_MATRIX_SDTV_LIMIT_NONSTD, 0);
                            }
                            else
                            {
                                msACESetHDTVMode(ACE_YUV_TO_RGB_MATRIX_SDTV_LIMIT);
                            }
                        }
                        else
                            msACESetHDTVMode(ACE_YUV_TO_RGB_MATRIX_SDTV);
                     }
                     else if(cf.ucColorimetry == COMBO_COLORIMETRY_xvYCC709)
                     {
                        if(cf.ucColorRange == COMBO_COLOR_RANGE_LIMIT)
                        {
                            #if ENABLE_HDR
                            if(mapi_Adjust_EnableNonStdCSC_Get() && (msGetHDRStatus(MAIN_WINDOW) == HDR_OFF))
                            #else
                            if(mapi_Adjust_EnableNonStdCSC_Get())
                            #endif
                            {
                                msACESetHDTVMode(ACE_YUV_TO_RGB_MATRIX_HDTV_LIMIT_NONSTD);//msACESelectYUVtoRGBMatrix(ucWinIndex, ACE_YUV_TO_RGB_MATRIX_HDTV_LIMIT_NONSTD, 0);
                            }
                            else
                            {
                                msACESetHDTVMode(ACE_YUV_TO_RGB_MATRIX_HDTV_LIMIT);
                            }
                        }
                        else
                            msACESetHDTVMode(ACE_YUV_TO_RGB_MATRIX_HDTV);
                      }
             }
             break;
        }
#endif
}

BYTE code mStarNullData[] = {0};
void mStarDummy(void)
{
    BYTE xdata i = mStarNullData[0];
    i = i;
}

#if 0//((CHIP_ID==CHIP_TSUM9 ||CHIP_ID==CHIP_TSUMF)&& !defined(TSUMR2_FPGA))
void msTrimFROFromEfuse(void)
{
    BYTE uctemp;

    uctemp = msEread_GetDataFromEfuse(0x0004);

    if((uctemp & BIT7) == BIT7)  // Trimming flag
    {
        old_msWriteByteMask(REG_018A, uctemp, 0x7F);
        g_bFROTrimResult = TRUE;

    }
    else
    {
        g_bFROTrimResult = FALSE;

    }
}

#if (ENABLE_XTAL_LESS)
//*******************************************************
//  RCOSC calibration counter.
//  If HIGH_RESOLUTION_EN = 0
//  >> TARGET frequency = REF_freq * COUNT_REPORT/ 512
//         => COUNT_REPORT = Target * 512  / REF_Freg = (OSC432M/16) * 512 / FRO12M  = 0x480
//  If HIGH_RESOLUTION_EN = 1
//  >> TARGET frequency = REF_freq * COUNT_REPORT/ 2048
//         => COUNT_REPORT = Target * 2048  / REF_Freg = (OSC432M/32) * 2048 / FRO12M = 0x900
//*******************************************************
Bool msStartRCOSC432MCal(void)
{
    #define WRITE_CAL_VALUE(A)    (old_msWriteByteMask(REG_1ECB, A,0xFF))
    #define RCOSC_HIGH_RESOLUTION_EN    0//BIT3
    #define RCOSC_COUNTER_MASK          0xFFF
    #if RCOSC_HIGH_RESOLUTION_EN
    #if CHIP_ID==CHIP_TSUMF
    #define OSC432_TARGET               0x900   //0x7C0 for 372M, 0x900 for 432M
    #else
    #define OSC432_TARGET               0x7C0   //0x7C0 for 372M, 0x900 for 432M
    #endif
    #else
    #if CHIP_ID==CHIP_TSUMF
    #define OSC432_TARGET               0x480   //0x3E0 for 372M, 0x480 for 432M
    #else
    #define OSC432_TARGET               0x3E0   //0x3E0 for 372M, 0x480 for 432M
    #endif
    #endif
    WORD ucCounter;
    BYTE i=0;

    old_msWriteByte(REG_1ECA,0x20);//0x1E65[7:0] = 0010_0000
    old_msWriteByteMask(REG_01B2, 0, BIT5|BIT6|BIT7);//0x0159[7:5] = 000 (default)
    old_msWriteBit(REG_1EEB, TRUE, BIT6);//0x1E75[14] = 1 //Enable test bus output.
    old_msWriteByteMask(REG_1EE8, 0x13, 0x1F);//0x1E74[4:0] = 10011 // Select TEST_CLK_OUT source.
    old_msWriteByteMask(REG_1EEB, 0x01, 0x1F);//0x1E75[12:8] = 00001 //ckg_tstclk
    #if RCOSC_HIGH_RESOLUTION_EN
    old_msWriteByteMask(REG_1EEC,0x05,0x07);//0x1E76[2:0] = 101
    #else
    old_msWriteByteMask(REG_1EEC,0x04,0x07);//0x1E76[2:0] = 100
    #endif
    Delay1ms(99);
    do
    {
        WRITE_CAL_VALUE(i++);
        Delay1ms(1);
        old_msWriteByte(REG_3A80, 0x80|RCOSC_HIGH_RESOLUTION_EN);//osc soft reset
        old_msWriteByte(REG_3A80, 0x03|RCOSC_HIGH_RESOLUTION_EN);// calculate & counter one time mode enable
        while( !( old_msReadByte(REG_3A83) & BIT4 ) );
        ucCounter = old_msRead2Byte(REG_3A82)&RCOSC_COUNTER_MASK;
    }
    while(( ucCounter < OSC432_TARGET ) &&  (i < 0xF0 ));
    old_msWriteBit(REG_1EEB, FALSE, BIT6); //close test bus
    return (BOOL)(ucCounter >= OSC432_TARGET);

    #undef WRITE_CAL_VALUE
    #undef RCOSC_HIGH_RESOLUTION_EN
    #undef RCOSC_COUNTER_MASK
    #undef OSC432_TARGET

}
#endif
#endif

WORD GetHSyncWidth(void)
{
    WORD u16HsyncWidth;
    WORD u16AutoHDEWidth = SC0_READ_AUTO_WIDTH();
    volatile BYTE u8BP_SC0_7A = msReadByte(SC00_7A);
    DWORD u32sclk=0;
    WORD u16hPeriod;

    if(CURRENT_INPUT_IS_VGA())
    {
        u16hPeriod = SC0_READ_HPEROID();
        u32sclk = (( DWORD )UserPrefHTotal * (SC0_HPERIOD_16LINE_MODE() ? MST_HPeriod_UINT_16 : MST_HPeriod_UINT)) / (u16hPeriod );
    }

    msWriteByteMask(SC00_7A ,BIT3 ,BIT3);
    u16HsyncWidth = (msReadByte(SC00_EB)&0x0F)<<8|msReadByte(SC00_EA);
    msWriteByte(SC00_7A, u8BP_SC0_7A);


    if(CURRENT_INPUT_IS_VGA())
    {
    u16HsyncWidth=(((float)u16HsyncWidth*u32sclk+XTAL_CLOCK_KHZ/2)/XTAL_CLOCK_KHZ); //return clock count, DVI by idclk domain, ADC by xtal clk domain
    }

    u16HsyncWidth = (u16HsyncWidth>u16AutoHDEWidth)?(mSTar_GetInputHTotal()-u16HsyncWidth):(u16HsyncWidth);

    return u16HsyncWidth;  //return clock count, DVI by idclk domain, ADC by xtal clk domain
}

void SetInputColorFormat( void )
{
#if ENABLE_VGA_INPUT
    if(g_bInputSOGFlag)
    {
        if(UserPrefInputColorFormat == INPUTCOLOR_YUV) //20150121
        {
            drvADC_SetADCSource(ADC_TABLE_SOURCE_YUV);  // input is YUV
        }
        else
        {
            drvADC_SetADCSource(ADC_TABLE_SOURCE_SOG);   // input is RGB
        }
    }
    else
    {
        if(UserPrefInputColorFormat == INPUTCOLOR_YUV) //20150121
        {
            drvADC_SetADCSource(ADC_TABLE_SOURCE_YUV_HV);  // input is YUV
        }
        else
        {
            drvADC_SetADCSource(ADC_TABLE_SOURCE_RGB);   // input is RGB
        }
    }
#endif

#if ENABLE_HDR
    msSetHDREnable(MAIN_WINDOW, HDR_OFF);
#endif

    //msACECSCControl();
#if CHIP_ID == CHIP_MT9701
    E_WIN_COLOR_FORMAT eWinColor;
    if(appmStar_CbGetInitFlag() && appmStar_CB_FuncList[eCB_appmStar_CustomizeWinColorCondition]!=NULL)
    {
        eWinColor = (E_WIN_COLOR_FORMAT)((fbappmstarCustomizeWinColorConditionCb)appmStar_CB_FuncList[eCB_appmStar_CustomizeWinColorCondition])();
    }
    else
    {
        if(IsColorspaceRGBInput())
            eWinColor = WIN_COLOR_RGB;  //should be customized by user preference
        else
            eWinColor = WIN_COLOR_YUV;
    }

    msACESetWinColor(TRUE, eWinColor);
    msACESetWinColorRange(TRUE,  UserPrefInputColorFormat);
    if (TRUE == mdrv_ACE_MweEnable_Get())
    {
        msACESetWinColor(FALSE, WIN_COLOR_YUV);
        msACESetWinColorRange(FALSE,  UserPrefInputColorFormat);
    }
#else
#if ENABLE_FULL_RGB_COLOR_PATH
    msACECSCControl();
#else
    AdjustVideoHueSaturation();
#endif
#endif
}

