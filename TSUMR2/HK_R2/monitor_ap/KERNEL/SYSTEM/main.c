#include "types.h"
#include "board.h"
#include "Global.h"
#include "Mcu.h"
#include "Detect.h"
#include "Menu.h"
#include "ms_reg.h"
#include "Power.h"
#include "Debug.h"
#include "drvUART.h"
#include "drvUartDebug.h"
#include "uartdebug.h"
#include "SysInit.h"
#include "Common.h"
#include "Ms_rwreg.h"
#include "msOSD.h"
#include "misc.h"
#include "NVRam.h"
#include "mStar.h"
#include "UserPref.h"
#include "Keypad.h"
//#include "Panel.h"
#include "msflash.h"
#include "DDC2Bi.H"
#include "GPIO_DEF.h"
#include "ComboApp.h"
#if ENABLE_RTE
#include "drvmsOVD.h"
#endif
#if (MS_PM)
#include "Ms_PM.h"
#endif
#if DDCCI_ENABLE||AudioFunc
#include "Adjust.h"
#endif
#include "MsDLC.h"
#include "msACE.h"
#if ENABLE_DAC
#include "drvDAC.h"
#endif

////////////////////////////////////
//#include "drvMcu.h"
///////////////////////////////////
#if ENABLE_DP_INPUT
#include "mapi_DPRx.h"
#endif

#include "mapi_eDPTx.h"


#include "mdrv_hdmiRx.h"

#if ENABLE_DPS
#include "msDPS_Setting.h"
#endif

#if ENABLE_LOCALDIMMING
#include "msLD_Handler.h"
#endif
#if ENABLE_USB_TYPEC
#include "mailbox.h"
#endif

#if ENABLE_HDR
#include "drvHDRCommon.h"
#endif
#include "usbsw.h"
#include "drvUSBDwld.h"
#if ENABLE_MSBHK
#include "msAPI_MSBHK.h"
#endif

#include "CustomCbAPI.h"

#if ENABLE_MTK_TEST
#include "DDCMtkTestHandler.h"
#endif

#define Main_DEBUG    1
#if ENABLE_DEBUG&&Main_DEBUG
    #define Main_printData(str, value)   printData(str, value)
    #define Main_printMsg(str)           printMsg(str)
#else
    #define Main_printData(str, value)
    #define Main_printMsg(str)
#endif

void Init_Device( void );
void Main_SlowTimerHandler(void);

extern void Init_GlobalVariables( void );
extern void InitFirstPowerOnVariables(void);
extern void i2C_Intial( void );
#if ENABLE_HDCP&&HDCPKEY_IN_Flash
//extern void msInitHDCPProductionKey( void ); //TBD
#endif
extern Bool ResetAllSetting( void );

extern BYTE xdata PowerDownCounter;

extern xdata Bool bHDMIFreesyncChk;

#if ENABLE_INTERNAL_CC
extern XDATA sPM_Info sPMInfo;
#define BUCK_BOOST_CFG_CNT  5// unit: second, used to disable buck-boost while CC is idle 
#endif

BYTE xdata mainloop = 0;

typedef unsigned int  __u16;    // 2 bytes
typedef unsigned char   __u8;     // 1 byte

#ifndef U8
#define U8    BYTE
#endif

#ifndef BOOLEAN
#define BOOLEAN    BYTE
#endif
//extern __u8*  MDrv_Get_HID_Report_Raw_Data(void);
//extern U8 drvUSBHost_HID_Initial(void);
extern U8 drvUSBHost_HID_Initial_Port2(U8 u8Interface);
//extern void MDrv_USB_Init(DWORD USBAdr);
extern void MDrv_USB_Init_Port2(DWORD USBAdr);
extern BOOLEAN  MDrv_Usb_Device_Enum(void);
extern BOOLEAN  MDrv_Usb_Device_Enum_Port2(void);
extern BOOLEAN MDrv_UsbDeviceConnect(void);
extern BOOLEAN MDrv_UsbDeviceConnect_Port2(void);
extern void drvUSBHost_UTMIInitial(void);
extern void  MDrv_Get_Keyboard_Staus(U8 u8Interface);
extern void  MDrv_Get_Mouse_Staus(U8 u8Interface);
extern void  MDrv_Get_Mouse_Staus_Port2(U8 u8Interface);
extern void  MDrv_Get_Keyboard_Staus_Port2(U8 u8Interface);
extern void upll_enable(void);

#define PORT
#define PORT2
#define HOST20_INTERFACE_NUM_MAX    5
extern U8 xdata PORT_DEVICE[HOST20_INTERFACE_NUM_MAX];//support 5 interface
extern U8 xdata PORT2_DEVICE[HOST20_INTERFACE_NUM_MAX];
#if (ENABLE_USB_TYPEC == 0x1)
extern BYTE XDATA glSYS_TypeC_TriggerHPDFlag;
extern BYTE XDATA glSYS_TypeC_PostponeHPDFlag;
#endif
BYTE code msMainNullData[] = {0};
BOOL xdata glbIPRxInitHPD_DONE_FLAG;

void msMainDummy(void)
{
    BYTE xdata i = msMainNullData[0];
    i = i;
}

int main( void )
{
    //BYTE UsbConnect = 0, UsbConnect_Port2 = 0;
    InitFirstPowerOnVariables();
    BootTimeStamp_Clr();
#if ENABLE_BOOT_TIME_PROFILING
    MDrv_UART_SetIsPrint(FALSE);
#endif
    BootTimeStamp_Set(AP_INIT_TS, 0, TRUE); // ap init timestamp 0
#if 0//(CHIP_ID == CHIP_TSUM9 ||CHIP_ID == CHIP_TSUMF)
    drvmStar_PowerOnInit();
#if (CHIP_ID == CHIP_TSUMF)
    MPLL_CLOCK_ADC(TRUE);
#endif
#else
    #if (MS_PM)
    msPM_Exit();    // for WDT reset in PM mode
    #endif
#endif

    BootTimeStamp_Set(AP_INIT_TS, 1, TRUE); // ap init timestamp 1
    Init_MCU();
    BootTimeStamp_Set(AP_INIT_TS, 2, TRUE); // ap init timestamp 2
#if( ENABLE_WATCH_DOG )
    ClearWDT();
#endif

#if ENABLE_MSTV_UART_DEBUG
    dbgVersionMessage();
    MDrv_UART_DebugInit(DEFAULT_UART_DEV);
#else
    mdrv_uart_close((MS_U32)mdrv_uart_open(DEFAULT_UART_DEV));
#endif

    BootTimeStamp_Set(AP_INIT_TS, 3, TRUE); // ap init timestamp 3
    ScalerReset(RST_ALL);
    ForceDelay1ms( 100 );
    BootTimeStamp_Set(AP_INIT_TS, 4, TRUE); // ap init timestamp 4
    g_ucFlashID = ReadFlashID();

    g_dwSpiDuelImageOffset = msRead4Byte(REG_002D3C);//0x6000ul

#if FLASH_READ_BYTE_BY_CODE_POINTER
    g_xfr_FlashCode = (BYTE volatile *)(((DWORD)(USER_DATA_BANK)<<16)-g_dwSpiDuelImageOffset);
#endif

#if !USEFLASH
    i2C_Intial();
#endif

    BootTimeStamp_Set(AP_INIT_TS, 5, TRUE); // ap init timestamp 5
#ifdef TSUMR2_FPGA
    Init_MonitorSetting();
#else
    ReadMonitorSetting();
#endif
//UserPrefInputPriorityType = Input_Priority_Auto;
    CustomCb_CbFunnction_Init();

    BootTimeStamp_Set(AP_INIT_TS, 6, TRUE); // ap init timestamp 6
    mStar_Init(TRUE);
    BootTimeStamp_Set(AP_INIT_TS, 7, TRUE); // ap init timestamp 7
    mStar_ACOnInit();

    BootTimeStamp_Set(AP_INIT_TS, 8, TRUE); // ap init timestamp 8
#if ENABLE_USB_DEVICE
    if( !ENABLE_USB_TYPEC || IsUSBDL() )
        msDrvUsb2Init();
#endif

#if (MS_PM)
    msPM_Init();
#endif

    CheckFactoryKeyStatus();
    Init_GlobalVariables();
    Menu_InitVariable();
    Init_Device();
    DDC2Bi_Init();
    FlashBinFileSizeCheck();
#if ENABLE_MTK_TEST
    InitMtkTest();
#endif
    BootTimeStamp_Set(AP_INIT_TS, 9, TRUE); // ap init timestamp 9
    while( 1 )
    {


#if (MS_PM)
        msPM_Handler();
        if(!msPM_IsState_IDLE())
            continue;
#endif
       BootTimeStamp_Set(AP_LOOP_TS, 0, TRUE); // ap loop timestamp 0

       if((glbIPRxInitHPD_DONE_FLAG == FALSE)&&( !DisplayLogoFlag ) )
    	{
    		#if(ENABLE_DP_INPUT == 0x1)
			glbIPRxInitHPD_DONE_FLAG = TRUE;

			msAPI_combo_DP_InitRxHPD(TRUE);

				#if (ENABLE_USB_TYPEC == 0x1)
				if(glSYS_TypeC_PostponeHPDFlag == TRUE)
				{
					glSYS_TypeC_TriggerHPDFlag = TRUE;
				}
				#endif
			#endif
    	}

        Main_SlowTimerHandler();

#if UART1
    #if ENABLE_MSBHK
        MSBHK_RxHandler();
    #else
        UART1_Handler();
    #endif
#endif

#if( ENABLE_WATCH_DOG )
        ClearWDT();
#endif

#if (ENABLE_MSTV_UART_DEBUG)
        if ( msAPI_UART_DecodeCommand() )
        {
            continue; // stop main loop for debug
        }
#endif
		
		DDC2Bi_CommandHandler();
        if(bRunToolFlag)
            continue;

#if ENABLE_DP_INPUT
        mapi_DPRx_Handler();
#endif

#if PANEL_EDP
        if( g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE )
            mapi_eDPTx_Handle();
#endif

#if ENABLE_USB_DEVICE
    if( !ENABLE_USB_TYPEC || IsUSBDL())
        USB_Handler();
#endif
        msAPI_combo_Handler();

#if ENABLE_HDCP&&HDCPKEY_IN_Flash
        if( WriteHDCPcodeFlag )
            continue;
#endif

#if ENABLE_HDCP&&HDCPKEY_IN_Flash
        if( LoadHDCPKeyFlag )
        {
            //msInitHDCPProductionKey(); // TBD
            Clr_LoadHDCPKeyFlag();
        }
#endif

        Power_PowerHandler();
#if Enable_PanelHandler
        g_bPanelHandlerEn = 1;
        Power_PanelCtrlHandler();
#endif
#if (MS_PM)
        if(!msPM_IsState_IDLE())
            continue;
#endif

        if( !ModeDetectCounter || InputTimingChangeFlag )
        {
            mStar_MonitorInputTiming();
            ModeDetectCounter = 20;
        }

        mStar_ModeHandler();
#if ENABLE_DPS
	msDPSHandler();
#endif

#if ENABLE_DLC
        msDlcHandler();
#endif

#if (ENABLE_DAC)
        msAPI_AudioHandler();
#endif

#if ENABLE_USB_TYPEC
        MBX_handler();
#endif
        Menu_OsdHandler();

#if (MS_PM)
        if(!msPM_IsState_IDLE())
            continue;
#endif

#if MWEFunction
        if(( ColorAutoDetectFlag || UserPrefDcrMode ) && PowerOnFlag && !SyncLossState() && InputTimingStableFlag && ( !DisplayLogoFlag ) )
#else
        if(( UserPrefDcrMode ) && PowerOnFlag && !SyncLossState() && InputTimingStableFlag && ( !DisplayLogoFlag ) )
#endif
        {
#if ENABLE_DLC        
            msDlcHandler();
            msDCRHandler();
#endif            
        }
#if ENABLE_LOCALDIMMING
        msLDHandler();
#endif

#if ENABLE_TOUCH_PANEL||ENABLE_TOUCH_PANEL_CTRL_OSD
        TouchPanelHandler();
#endif

#if USEFLASH
        UserPref_FlashDataSaving();
#endif
    }

    return 0;
}

void Init_Device()
{
#if ENABLE_TOUCH_KEY
    TouchKeyRestortCounter = TouchKey_Init();
#endif

#if ENABLE_TOUCH_PANEL||ENABLE_TOUCH_PANEL_CTRL_OSD
    TPL_InitSetting();
#endif

    if( PowerOnFlag )
    {
        Power_TurnOnGreenLed();
    }
    else
    {
        Power_TurnOffLed();
    }
}

void Main_SlowTimerHandler(void)
{
#if ((CHIP_ID == CHIP_MT9701) && U3_REDRV_PM_MODE)
	msUSB3Handler();
#endif
    if(ms10Flag)
    {
        if(ModeDetectCounter)
        {
            if(ModeDetectCounter > 10)
                ModeDetectCounter -= 10;
            else
                ModeDetectCounter = 0;
        }

        if(TPDebunceCounter)
        {
            if(TPDebunceCounter > 10)
            {
                TPDebunceCounter -= 10;
            }
            else
            {
                TPDebunceCounter = 0;
                Set_StartScanKeyFlag();
            }
        }

        if(!FreeRunModeFlag)
        {
#if ENABLE_TOUCH_PANEL || ENABLE_TOUCH_PANEL_CTRL_OSD
            if(TPL_PollingCounter)
            {
                if(TPL_PollingCounter > 10)
                    TPL_PollingCounter -= 10;
                else
                    TPL_PollingCounter = 0;
            }
#endif
        }
#if ENABLE_DOLBY_HDR
    DVPQR2Event_Handler();
#endif
#if ENABLE_HDR_AUTODETECT
        AutodetectHDR_Handler();
#endif
#if ENABLE_MTK_TEST
        MtkTestUpdateHandler();
#endif
        Clr_ms10Flag();
    }

    if(ms50Flag)
    {

#if ENABLE_TOUCH_KEY
        if(TouchKeyRestortCounter)
        {
            TouchKeyRestortCounter--;
            if(!TouchKeyRestortCounter && (TouchKey_InitStatus() == PRE_RESTORT))
                TouchKey_Restort_Reg();

        }

#if ENABLE_LOW_CONTACT
        if(TouchKeyLowContactDelay)
        {
            TouchKeyLowContactDelay--;
            if(!TouchKeyLowContactDelay && (TouchKey_InitStatus() == PRE_RESTORT))
            {
                TouchKey_Restort_Reg();
                TouchKeyLowContactDetectCntr = 0;
            }
        }
#endif

#if TOUCH_KEY_POWER_KEY_DEBOUNCE
        if( bPowerKeyPressed )
        {
            if( PowerKeyDebounceCntr < POWER_KEY_DEBOUNCE_PEROID )
                PowerKeyDebounceCntr++;
        }
#endif
#endif

        if(IS_HDMI_FREESYNC())
        {
            if(bHDMIFreesyncChk)
                msWriteByte(SC00_E8,(msReadByte(SC00_E8)&(~BIT1))|BIT0);    //Enable force P mode
            bHDMIFreesyncChk = FALSE;
        }

        Clr_ms50Flag();
    }

    if(SecondFlag)
    {


#if ENABLE_WATCH_DOG_INT
        Main_printData("u8WDT_Status==%x",u8WDT_Status);
#endif
        Second++;

        if(g_u8SlowClkTimer)
            g_u8SlowClkTimer--;

#if USEFLASH
        UserPref_FlashSaveFlagCheck();
#endif

        if( PowerOnFlag && !PowerSavingFlag )
        {
            if( BlacklitTime < 0xFFFFFFFFul )
            {
                BlacklitTime += 1;
            }
            else
                BlacklitTime = 0;

            if( BlacklitTime > 0 && ( BlacklitTime % 1800 ) == 0 )
                SaveBlacklitTimeFlag = 1;
        }

        if( OsdCounter )
        {
            if( --OsdCounter == 0 )
            {
                Set_OsdTimeoutFlag();
            }
        }

#if HotInputSelect
        if( HotKeyCounter )
        {
            if( --HotKeyCounter == 0 )
            {
                if( PressExitFlag )
                    Set_EnableShowAutoFlag();
            }
        }
#endif

#if DISABLE_POWERSAVING
        //Do not enter PowerSaving
#else
        if(FakeSleepFlag && FakeSleepCounter)
        {
            if(--FakeSleepCounter == 0)
            {
                if(msCheckFakeSleep())
                {
                    FakeSleepCounter = 2;
                    //Main_printMsg("FAKE SLEEP");
                    g_CountSwitchPortTimeFlag = FALSE;
                    PowerDownCounter = 0;
                }
                else
                {
                    Set_ForcePowerSavingFlag();
                }
            }
        }
#endif

        if((msAPI_FakeSleepTimeOutEn_Get() == TRUE) && (msAPI_FakeSleepTimeOutCnt_Get() > 0))
        {
            msAPI_FakeSleepTimeOutCnt_Set((msAPI_FakeSleepTimeOutCnt_Get()-1));

            if(msAPI_FakeSleepTimeOutCnt_Get() == 0)
            {
                Main_printMsg("Fake sleep time out\n");
            }
        }
#if (NEW_MTK_UI == 1)
	if( PowerDownCounter )
        {
            if( --PowerDownCounter == 0 )
            {
                Set_ForcePowerSavingFlag();
            }
        }
#else
	#if DISABLE_POWERSAVING
        //Do not enter PowerSaving
	#else
	        if( PowerDownCounter )
	        {
	            if( --PowerDownCounter == 0 )
	            {
	                Set_ForcePowerSavingFlag();
	            }
	        }
	#endif
#endif

	    if(--g_BurninChangePatternSec==0)
	    {
	        g_BurninChangePatternSec = DEF_BurninChangePatternTime;
	        Set_ChangePatternFlag();
	    }

        if (g_SwitchSec && g_CountSwitchPortTimeFlag && (!DoBurninModeFlag))
        {
        	#if (NEW_MTK_UI == 1)
			if(UserPrefPowerSavingEn == PowerSavingMenuItems_On)
            {
                if( --g_SwitchSec == 0 )
                {
					g_CountSwitchPortTimeFlag=FALSE;
                    g_SwitchSec=DEF_FORCE_DPMS;
                    Set_ForcePowerSavingFlag();
                }
            }
			#else
            if( --g_SwitchSec == 0 )
            {
            	#if DISABLE_POWERSAVING
        		//Do not enter PowerSaving
				#else
                g_CountSwitchPortTimeFlag=FALSE;
                g_SwitchSec=DEF_FORCE_DPMS;
                Set_ForcePowerSavingFlag();
				#endif
            }
			#endif
        }

#if ENABLE_TOUCH_KEY

#if ENABLE_LOW_CONTACT
        if(TouchKeyLowContactDetectCntr)
            TouchKeyLowContactDetectCntr--;
#endif

        if((Second%2) == 0)
        {
            if(!TouchKeyRestortCounter) // 120417 coding addition
                TouchKeyRestortCounter = TouchKey_Check_ESD();
        }
#endif
        //if((Second%5) == 0)
        //    printData("~~Second: %d\n", Second);

#if ENABLE_USB_TYPEC
        if(g_u8TypeCPMDBCCnt)
            g_u8TypeCPMDBCCnt--;
#endif

#if ENABLE_INTERNAL_CC
        if((sPMInfo.ePMState == ePM_WAIT_EVENT) && ((Second%BUCK_BOOST_CFG_CNT) == 0))
        {
            BYTE u8IsCCIdle;

            drvmbx_send_CC_IDLE_RCMD(&u8IsCCIdle);
            if(u8IsCCIdle)
                drvmbx_send_BuBst_Config_CMD(BIT1|BIT0); // disable buck-boost when detach or power sink
        }
#endif

        Clr_SecondFlag();
    }

    if( SaveBlacklitTimeFlag )
    {
        SaveBlacklitTimeFlag = 0;
#if USEFLASH
        UserprefBacklighttime = BlacklitTime;
        UserPref_EnableFlashSaveBit( bFlashForceSaveMonitor2Bit );
#else
        SaveBlacklitTime();
#endif
    }

#if 0//ENABLE_TOUCH_KEY
#if TOUCH_KEY_CTRL_LED
    if(TouchKey_GetLEDStatus() != TouchKeySetLEDStatus)
        TouchKey_CtrlLED(TouchKeySetLEDStatus);
#endif
#endif

}
