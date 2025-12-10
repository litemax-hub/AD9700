#include "types.h"
#include "board.h"
#include "Global.h"
#include "Debug.h"
#include "Common.h"
#if (LANGUAGE_TYPE==ASIA)||(LANGUAGE_TYPE==SIXTEEN)
#include "menudef.h"
#endif
#include "MsDLC.h"
#include "msACE.h"
#include "UserPref.h"
#if ENABLE_DPS
#include "msDPS_Setting.h"
#endif
#include "COLOR_VERIFY.H"
#include "msEread.h"
#include "Detect.h"
#if ENABLE_DAC
#include "drvDAC.h"
#endif
#include "menufunc.h"
#include "hwi2c.h"
#include "mspi_flash.h"
#include "mailbox.h"
#include "usbsw.h"
#if ENABLE_UART_PIU
#include "drvUARTPIU.h"
#endif
#include "drvHDRCommon.h"
#include "drvUSBDwld.h"
#include "DDCColorHandler.h"
#include "msAPI_MSBHK.h"
#include "reserve.h"
#include "PDUpdate.h"

XDATA eBOOT_STATUS_TYPE g_eBootStatus = eBOOT_STATUS_ACON;

volatile BOOL g_bEnableIsrDisableFlock;
volatile BOOL g_bOutputVsyncIsrFlag;

#if ENABLE_MENULOAD || ENABLE_MENULOAD_2
BYTE xdata u8MenuLoadFlag[ML_DEV_NUM] = {0};
BOOL bMLEnable = FALSE;
#endif

#if ENABLE_DYNAMICSCALING
BYTE u8DSFlag = 0;
#endif

#if ENABLE_WATCH_DOG
BYTE xdata u8WDT_Enable=0;
#endif

#if ENABLE_WATCH_DOG_INT
BYTE xdata u8WDT_Status=0;
#endif

#if EnableTime1Interrupt
WORD XDATA g_u16MsCounter1 = 1000; // for System timer
BOOL XDATA bSecondTimer1 = FALSE;
BYTE XDATA u8SecondTIMER1 = 0;
#endif

#if 0//(CHIP_ID == CHIP_TSUM9||CHIP_ID == CHIP_TSUMF)
BOOL g_bFROTrimResult;
#endif

//#if ENABLE_DPS
BOOL idata bRunToolFlag = 0;
//#endif
BYTE data g_ucTimer0_TH0;
BYTE data g_ucTimer0_TL0;
BYTE xdata g_u8SystemSpeedMode=0;
BYTE xdata g_SwitchSec=DEF_FORCE_DPMS;
BYTE xdata g_CountSwitchPortTimeFlag=FALSE;

bit g_bSlowClkDetEnabled = FALSE;
bit g_bSlowClkDetForceDisabled = FALSE;
BYTE g_u8SlowClkTimer = 0;

#if ENABLE_MSPI_FLASH_ACCESS
//DWORD gsystem_time_ms=0;
#endif

#if ENABLE_BOOT_TIME_PROFILING
MS_U32 g_u32TimeStamp[MAX_TS][_TIME_STAMP_NUM_MAX_];
#endif

DWORD DPTx_time_ms=0;

volatile bit g_bMcuPMClock = 0;
bit SecondFlag = 0;
bit ms10Flag = 0;
bit ms50Flag = 0;
bit ms500Flag = 0; // 120815 coding, set to bit data type for isr usage
bit ms1000Flag = 0;

bit InputTimingChangeFlag = 0; // 120815 coding, set to bit data type for isr usage
bit g_bDisplayOK = 0;
bit g_bInputSOGFlag = 0;
BYTE xdata bInputVGAisYUV = 0;   // (0) VGA is RGB    (1) VGA is YUV //20150121

volatile WORD data ms_Counter = SystemTick; // for System timer
BYTE xdata ModeDetectCounter = 20;
volatile WORD xdata u16PanelOffOnCounter = 0;
volatile bit bPanelOffOnFlag = 0;

bit bPanelOnFlag = 0; // 100811 coding addition to indicate panel power status
BYTE xdata BackLightActiveFlag;
BYTE xdata KeypadButton = 0;

BYTE xdata DDCWPActiveFlag; //cc 2011.09.09 15:50

BYTE xdata Second;

BYTE xdata TPDebunceCounter = 0;
bit StartScanKeyFlag = 0;
bit bKeyReadyFlag = 0;
#if ENABLE_TOUCH_KEY
#if TOUCH_KEY_POWER_KEY_DEBOUNCE
bit bPowerKeyPressed = 0;
BYTE xdata PowerKeyDebounceCntr = 0;
#endif
#endif

bit TI0_FLAG; // 120815 coding, set to bit data type for isr usage
BYTE xdata SIORxBuffer[_UART_BUFFER_LENGTH_];
volatile BYTE xdata SIORxIndex;
volatile BYTE xdata u8ReSyncCounter;

#if EnableSerialPortTXInterrupt
WORD idata SIOTxBufferIndex;
WORD idata SIOTxSendIndex;
bit idata UART_SEND_DONE;
bit idata UartPolling;
BYTE xdata SIOTxBuffer[_UART_TX_BUFFER_LENGTH_];
#endif

#if UART1
bit TI1_FLAG;
BYTE xdata SIORxBuffer1[_UART_BUFFER_LENGTH_];
volatile WORD xdata SIORxIndex1;
volatile BYTE u8ReSyncCounter1;
#endif

BYTE xdata OsdCounter = 0;
BYTE xdata HotKeyCounter = 0;
BYTE xdata PowerDownCounter = 0;
BYTE xdata InputTimingStableCounter = 0;

volatile WORD xdata TimeOutCounter = 0;

DWORD xdata BlacklitTime = 0;
bit SaveBlacklitTimeFlag = 0;
//BYTE LanguageState=0;
//BYTE CurrentLanguage=0;   //wumaozhong add 20050615
//BYTE ShortcutLuminance=0;
//BYTE SaveIndex=0; // saveIndex
WORD xdata SystemFlags = 0; // for system status flags
WORD xdata System2Flags = 0;
BYTE xdata System3Flags = 0;  // Rex 100701 for 3D
//BYTE xdata ChipVer = 0;
BYTE xdata SaveIndex = 0; // saveIndex
BYTE xdata PatternNo = 0;
BYTE xdata BrightnessTemp = 0;
BYTE xdata PrevHistoValue = 0;

#if ENABLE_TIME_MEASUREMENT
WORD volatile idata g_u16TimeMeasurementCounter = 0;
bit bTimeMeasurementFlag=0;
WORD idata g_u16Time1;
WORD idata g_u16Time2;
#endif

bit gBoolVisualKey = 0;
BYTE xdata gByteVisualKey = 0xff;
BYTE xdata LanguageIndex;
BYTE xdata SecondTblAddr;
#if ENABLE_HDCP
BYTE xdata HDCPFlag = 0;
#endif
#if ENABLE_HDCP&&HDCPKEY_IN_Flash
BYTE xdata HDCPDataBuffer[6]; //[6];
BYTE code g_u8SecureStormAESKey[16] =
{
    'M', 'S', 't', 'a', 'r', '_', 'M', 'S', 'T', '9', 'U', '4', '-', 'V', '1', '0'
};
#endif
#if ENABLE_USB_TYPEC
BYTE xdata g_u8TypeCPMDBCCnt; // PM TypeC Auto Mode Debounce Counter
#endif
#if ENABLE_SECU_R2
BYTE xdata g_u8SecuR2Alive=0; // SecuR2 status. 1:alive, 0:dead
#endif

BYTE xdata InputColorFormat = 0; // indicate input color space is RGB or YUV
BYTE xdata InputColorRange = 0; // indicate input color space is Full or Limit

bit DebugOnlyFlag = 0;
#if 1//Enable_Expansion
bit bExpansionBit = 0;
#endif
bit SaveFactorySettingFlag = 0;

volatile WORD data u16DelayCounter = 0;
volatile bit bDelayFlag = 0;

volatile U32 u16TimeOutCounter = 0; // 120510 coding, addition for checking register status
volatile bit bTimeOutCounterFlag = 0;


bit g_bServiceEDIDUnLock = 0;

OverScanType  xdata OverScanSetting;

PanelSettingType xdata PanelSetting;
InputTimingType xdata InputTiming;
MonitorSettingType xdata MonitorSetting; //Benz 2007.4.15   19:44:07idata - >xdata
#if USEFLASH
MonitorSettingType2 xdata  MonitorSetting2;
#endif
ModeSettingType xdata ModeSetting;
FactorySettingType xdata  FactorySetting;
//==========================for AutoBacklight=====
BYTE xdata HistogramNow = 0;
BYTE xdata HistogramPrev = 0;
BYTE xdata BacklightNow = 0;
BYTE xdata BacklightPrev = 0;
BYTE xdata HistogramFlag = 1;

//xdata BYTE xfr_regs[256] _at_ 0xC000;
//xdata BYTE g_FlashID = 0;

BYTE xdata LastMenuPage = 0;
BYTE xdata FakeSleepCounter = 0;
//bit PowerGoodExistFlag = 0;
#if ENABLE_HPERIOD_16LINE_MODE
BOOL g_16ModePre = FALSE;
#endif

BYTE  xdata g_BurninChangePatternSec = DEF_BurninChangePatternTime;

extern void msDebugDummy(void);
extern void msDPSSettingDummy(void);
extern void msDPSHandlerDummy(void);
extern void msGammaDummy(void);
extern void msPMDummy(void);
extern void msDACDummy(void);
extern void mStarDummy(void);
extern void msapmStarDummy(void);
extern void msMainDummy(void);
extern void msAdjustDummy(void);
//extern void msDDCColorHandlerDummy(void);
extern void msAutoGammaDummy(void);

BYTE xdata SwitchPortCntr = 0;
//bit g_bBacklightOn = 0;

//BYTE code UserDataCode[];// =//Set UserDataCode To Bank3 Addr:A000

#if DECREASE_V_SCALING
WORD xdata DecVScaleValue = 0;
#endif

#if !ENABLE_RTE
void msOverDriveDummy(void);
#endif
#if 1//!ENABLE_RTE
void drvmsOVDDummy(void);
#endif
#if  !ENABLE_3DLUT
 void ms3DLUT_Dummy(void);
#endif

#if Enable_ReducePanelPowerOnTime
volatile WORD g_u16PanelPowerCounter = 0;
ePANEL_STATUS_TYPE g_ePanelStatus = ePANEL_STATUS_NONE;
#endif

#if Enable_PanelHandler
BOOL g_bPanelISREn                              = 1;                // enable Power_PanelCtrlIsr
BOOL g_bPanelHandlerEn                          = 0;                // enable Power_PanelCtrlHandler
BOOL g_bPanelStateStopFlag[ePANEL_STATE_MAX]    = {0};              // 0: enter state 1 denied, 1: available
ePANEL_STATE g_ePanelState                      = ePANEL_STATE_0;
BOOL g_bCurrentPanelCtrlOn                      = 0;                // set by Power_PanelCtrlOnOff
BOOL g_bPreviousPanelCtrlOn                     = 0;                // set by ISR / handler
PANEL_POWER_TIMING_INFO g_sPanelPowerTimingInfo =
{
    .state1_2_Min   = PanelOnTiming1,
    .state1_2_Max   = -1,
    .state2_3_Min   = PanelOnTiming2,
    .state2_3_Max   = -1,               // recommended set as -1
    .state3_2_Min   = PanelOffTiming1,
    .state3_2_Max   = -1,               // recommended set as -1
    .state2_1_Min   = PanelOffTiming2,
    .state2_1_Max   = -1,
    .paneloffon_Min = 1200,
};
volatile MS_S32 g_s32PanelCounter       = 0;
volatile MS_S32 g_s32PanelMinCounter    = 0;
volatile MS_S32 g_s32PanelMaxCounter    = -1;
volatile MS_S32 g_s32PanelCustomCounter = 0;
#endif

//This flag is for power on(AC/DC/DPMS) 1 time use
BOOL g_bFirstPowerOn[eFIRST_POWER_ON_INDEX_MAX];

#if (HDMI_HPD_AC_ON_EARLY_PULL_LOW == 1)
volatile MS_U16 g_u16HDMIHPDAcOnEarlyPullLowCnt = 0;
#endif
#if ENABLE_TOUCH_KEY
BYTE xdata TouchKeyRestortCounter; // unit: 50mS
    #if ENABLE_LOW_CONTACT
    BYTE xdata TouchKeyLowContactDelay; // unit: 50mS
    BYTE xdata TouchKeyLowContactDetectCntr; // unit: 1S, use to detect low contact peroid
    #endif
    #if 0//TOUCH_KEY_CTRL_LED
    BYTE xdata TouchKeySetLEDStatus;
    #endif
#else
void TouchKeyDummy(void);
#endif

extern void UserDataSpaceDummy(void);
extern void PDUpdateReservedDummy(void);

void DummyCall(void)
{
    //BYTE i = 0;
#if 0
    if(i)
    {
    #if ENABLE_UART_PIU
        piu_uart_putc((char)'\n');
    #endif
        putSIOChar((char)'\n');
        (WORD)i = GetStandardModeVTotal();
        UserDataSpaceDummy();
    #if 1//ENABLE_DEBUG
        mStarDummy();
        msapmStarDummy();
        msMainDummy();
        msAdjustDummy();
    #endif

    #if !ENABLE_DEBUG
        msDebugDummy();
    #endif

    #if !ENABLE_DeltaE
        msDDCColorHandlerDummy();
        msAutoGammaDummy();
    #endif

    #if !ENABLE_DPS
        msDPSSettingDummy();
        msDPSHandlerDummy();
    #endif

    #if !Enable_Gamma
        msGammaDummy();
    #endif

    #if (!ENABLE_DEBUG) || (!MS_PM)
        msPMDummy();
    #endif

    #if !ENABLE_RTE
        msOverDriveDummy();
    #endif

    #if 1//!ENABLE_RTE
        drvmsOVDDummy();
    #endif

    #if !(ENABLE_DEBUG&&COLORVERIFY_DEBUG)
        msColorVerifyDummy();
    #endif

    #if  !ENABLE_3DLUT
        ms3DLUT_Dummy();
    #endif

    #if !ENABLE_TOUCH_KEY
        TouchKeyDummy();
    #endif

    #if !ENABLE_DAC
        msDACDummy();
    #endif

    #if !ENABLE_USB_TYPEC
        MBXDummy();
    #else
        drvmbx_send_Multi_Func_Config_CMD(0);
    #endif

    #if !ENABLE_WINISP
        WinISPDummy();
    #endif

        USBSWDummy();
        DetectDummy();
        UserPrefDummy();
        DualImageDummy();
        msDISetBankOffset(0);
        msDIGetBankOffset();
        GetPow2(0);
        GetLog(0);
        GetImageHeight();
        GetImageWidth();
		mcuGetSpiMode();
        MSBHKeDummy();
        RedundanceFuc();

    #if defined(TSUMR2_FPGA) || !(CHIP_ID==CHIP_TSUMC ||CHIP_ID==CHIP_TSUMD)
    {
        BYTE bHDCPKeyInEfuse,ucValue,u8ADCBandgapTrimVal;
        bHDCPKeyInEfuse = msEread_IsHDCPKeyInEfuse();
        ucValue = msEread_GetHDCPKeyFromEfuse(0);
        msEread_CompareHDCPChecksumInEfuse(0);
        u8ADCBandgapTrimVal = msEread_GetADCBandgapTrimValFromEfuse();
        msEread_SetComboInitialValue();
        msEread_SetHDMIInitialValue();
    }
    #endif

	#if (MS_PM)
        msPM_StartRCOSCCal();
    #endif

    #if (CHIP_ID == CHIP_TSUM9 ||CHIP_ID == CHIP_TSUMF)
        drvGPIO_PWMAlignVSyncDelay(0, 0);
        drvGPIO_PWMInverse(0, 0);
    #endif

    #if ENABLE_DAC
        msAPI_AudioSineGen(0,0,0);
    #endif

        GetHSyncWidth();
        GetNonFuncValue();

        mcuDMACRC32(0,0,0,0,0,0);
        ///////////////////FPGA////////////////////
        old_msWrite3Byte(0, 0);
        scReadByte(0);
        scWriteByte(0, 0);
        msEread_SetMHLInitialValue();

        msDrvWriteInternalEDID(0, 0, 0);
        msDrvReadInternalEDID(0, 0);
       // drv_CEC_DummyFunction();

        hw_i2c_uncall();
        mcu40kXdataMapToDRAMIMI(0, 0);
        msDrvMcuIsPD51Alive();

       #if ENABLE_MSPI_FLASH_ACCESS
   	{
        BYTE u8Temp=0,buff[5];
        MDrv_MSPI_Flash_Init(0,0,0,0,0,0,0);
        u8Temp = MDrv_MSPI_Flash_Read_ID((EN_MSPI)0x00, 0x00);

        MDrv_MSPI_Flash_Erase_All((EN_MSPI)0x00, 0x00);
        MDrv_MSPI_Flash_Write((EN_MSPI)0x00, 0x00, 0x00, 0x00, buff);
        MDrv_MSPI_Flash_Read((EN_MSPI)0x00, 0x00, 0x00, 0x00, buff);

        MDrv_MSPI_Flash_Sector_Erase(0, 0, 0);
        MDrv_MSPI_Flash_32KB_Block_Erase(0, 0, 0);
        MDrv_MSPI_Flash_64KB_Block_Erase(0, 0, 0);

        MDrv_MSPI_Flash_Read_Status_Register2(0, 0);
        MDrv_MSPI_Flash_Write_Status_Register(0, 0, 0);
        MDrv_MSPI_Flash_Write_Enable(0, 0);
        MDrv_MSPI_Flash_Write_Disable(0, 0);
   	}
       #endif

        #if ENABLE_HDR
        msSetHDREnable(0, 0);
        #endif
    	usb_connect();
        ForceDelay1ms_Pure(0);
        PDUpdateDummy();
        PDUpdateReservedDummy();
    }
#endif

}

void Init_GlobalVariables( void )
{
/////////////////////////////////////////////////
// add for prevent complier warning
    DummyCall();
//////////////////////////////////////////////////
#if Enable_Expansion
    OverScanSetting.Enable= 1;
    OverScanSetting.AspRatio= UserprefExpansionMode;
    OverScanSetting.ScanRatio= 98;
    OverScanSetting.ImageRatio= 100;
#endif

    g_bOutputVsyncIsrFlag = 0;

    InputTimingStableCounter = 0;
    Second = 0;
    OsdCounter = 0;
    PowerDownCounter = 0;
    bPanelOffOnFlag = 0;
    bPanelOnFlag = 0;
    Clr_StartScanKeyFlag();
    TPDebunceCounter = SKPollingInterval;

#if USEFLASH
    BlacklitTime = UserprefBacklighttime;
#endif

#if ENABLE_TOUCH_KEY
    TouchKeyRestortCounter = 0;
    #if ENABLE_LOW_CONTACT
    TouchKeyLowContactDetectCntr = TOUCH_KEY_LOW_CONTACT_DETECT_PEROID;
    TouchKeyLowContactDelay = 0;
    #endif
    #if 0//TOUCH_KEY_CTRL_LED
    TouchKeySetLEDStatus = TouchKeyLEDStatus_OFF;
    #endif
#endif

    SrcFlags = SyncLoss;
    SrcModeIndex = 0x00;
    Input420Flag = 0;

    if( BurninModeFlag )
    {
        Set_DoBurninModeFlag();
    }
    else
    {
        Clr_DoBurninModeFlag();
    }

    if( !( ProductModeFlag || FactoryModeFlag ) )
        Set_ShowInputInfoFlag();

    if( PowerOnFlag )
    {
        Set_InputTimingChangeFlag();
#if DisplayPowerOnLogo
        if(UserPrefDisplayLogoEn)
        {        
            Set_DisplayLogoFlag();
        }
#else
        Clr_DisplayLogoFlag();  //2009-10-13
#endif
    }
    else
    {
        SystemFlags |= bForcePowerSavingBit;
        Clr_DisplayLogoFlag();
    }

#if LANGUAGE_TYPE == ASIA
    if( UserPrefLanguage == LANG_TChina )
    {
        LanguageIndex = 1;
        SecondTblAddr = 0x80;
    }
    else if( UserPrefLanguage == LANG_Korea )
    {
        LanguageIndex = 2;
        SecondTblAddr = 0x80; //SecondTblAddr = 0xFF;
    }
    else
    {
        LanguageIndex = 0;
        SecondTblAddr = 0xFF;
    }
#elif LANGUAGE_TYPE == SIXTEEN                    //090330 xiandi.yu
    if( UserPrefLanguage == LANG_SChina )
    {
        LanguageIndex = 1;
        SecondTblAddr = 0x80;
    }
    else if( UserPrefLanguage == LANG_Japanese )
    {
        LanguageIndex = 2;
        SecondTblAddr = 0x80; //SecondTblAddr = 0xFF;
    }
    else
    {
        LanguageIndex = 0;
        SecondTblAddr = 0xFF;
    }
#elif LANGUAGE_TYPE == EUROPE
    LanguageIndex = 0;
    SecondTblAddr = 0xFF;
#endif

#if DECREASE_V_SCALING
    DecVScaleValue = 0;
#endif

#if ENABLE_DPS
#if DPS_UserBrightControl_EN
    REG_DPS_UserBrightContlGain = UserPrefBrightness;//(float)USER_PREF_BRIGHTNESS * 256 / 100;
#else
    REG_DPS_UserBrightContlGain = 100;
#endif
#endif
}

void InitFirstPowerOnVariables(void)
{
    BYTE i = 0;
    for(i=0; i<eFIRST_POWER_ON_INDEX_MAX; i++)
    {
        g_bFirstPowerOn[i] = TRUE;
    }
}

BYTE GetPow2(BYTE expre)
{
    if( expre == 0 )
        return 1;
    else
        return (0x02<<(expre-1));
}

BYTE GetLog(DWORD value)
{
    BYTE i = 0;

    if( value == 0 )
        return 0;

    while(value)
    {
        value >>= 1;
        i++;
    }

    return (i-1);
}


BYTE TransNumToBit(BYTE value)
{
    XDATA BYTE i=1;
    if(value == 0)
        return 1;

    while(value)
    {
        i <<= 1;
        value--;
    }
    return i;
}


