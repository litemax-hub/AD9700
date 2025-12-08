#include <math.h>
#include "types.h"
#include "board.h"
#include "Global.h"
#include "ms_reg.h"
#include "Ms_rwreg.h"
#include "Power.h"
#include "Debug.h"
#include "Common.h"
#include "misc.h"
#include "Menu.h"
#include "UserPref.h"
#include "msOSD.h"
#include "mStar.h"
#include "menufunc.h"
//#include "msregTV.h"
//#include "Panel.h"
#include "Adjust.h"
#include "Mode.h"
#include "GPIO_DEF.h"
#include "drvOSD.h"
#include "drvADC.h"
#include "AutoFunc.h"
#include "Detect.h"
#ifdef UseInternalDDCRam
#include "ddc.h"
#endif
#include "msACE.h"
#include "MsDLC.h"

#if ENABLE_DAC
#include "drvDAC.h"
#endif

#if ENABLE_HDMI
#include "mdrv_hdmiRx.h"
#endif

#include "mapi_DPRx.h"

#include "mailbox.h"

/////////////////////////////////////
#include "drvmStar.h"
//#include "halRwreg.h"
////////////////////////////////////


#define DETECT_DEBUG    0
#if ENABLE_DEBUG&&DETECT_DEBUG
    #define DETECT_printData(str, value)   printData(str, value)
    #define DETECT_printMsg(str)           printMsg(str)
    #define DETECT_PRINT(format, ...)      printf(format, ##__VA_ARGS__)
#else
    #define DETECT_printData(str, value)
    #define DETECT_printMsg(str)
    #define DETECT_PRINT(format, ...)
#endif

#define DEBUG_HDMI_MHL_CABLE_DETECT     0
#if ENABLE_DEBUG && DEBUG_HDMI_MHL_CABLE_DETECT
    #define DEBUG_HDMI_MHL_CABLE(str, value)   printData(str, value)
#else
    #define DEBUG_HDMI_MHL_CABLE(str, value)
#endif
////////////////////////////////////////////////////////////////////////
#define SyncValidCount_VGA          10
#define SyncLossCount_VGA           20
#define SyncValidCount_DVI          20
#define SyncLossCount_DVI           60
#define SyncValidCount_HDMI         50
#define SyncLossCount_HDMI          250
#define SyncValidCount_DP           50
#define SyncLossCount_DP            200
////////////////////////////////////////////////////////////////////////

#define SyncValidCount_Digital  ((CURRENT_INPUT_IS_DISPLAYPORT())?(SyncValidCount_DP):((CURRENT_INPUT_IS_HDMI())?(SyncValidCount_HDMI):(SyncValidCount_DVI)))
#define SyncLossCount_Digital  ((CURRENT_INPUT_IS_DISPLAYPORT())?(SyncLossCount_DP):((CURRENT_INPUT_IS_HDMI())?(SyncLossCount_HDMI):(SyncLossCount_DVI)))

#define ValidTimingStableCount  ((CURRENT_INPUT_IS_VGA())?(SyncValidCount_VGA):(SyncValidCount_Digital))
#define SyncLossStableCount     ((CURRENT_INPUT_IS_VGA())?(SyncLossCount_VGA):(SyncLossCount_Digital))

#if ENABLE_SPEEDUP_SWITCH
#if ENABLE_HDMI
#define HDMI_STAY_1S_COUNT      50 // unit: 20mS, 50x20=1000mS
#define HDMI_SPEEDUP_SWITCH_PORT()    (HDMICableNotConnect() && (!msAPI_combo_HDMIRx_GetVideoContentInfo(COMBO_VIDEO_CLK_STABLE, SrcInputType)) && (InputTimingStableCounter >= HDMI_STAY_1S_COUNT))
#endif
#if ENABLE_DP_INPUT
#define DP_STAY_1S_COUNT      50 // unit: 20mS, 50x20=1000mS
#define DP_SPEEDUP_SWITCH_PORT()      (DPCableNotConnect() && (InputTimingStableCounter >= DP_STAY_1S_COUNT))
#endif
#if ENABLE_USB_TYPEC
#define DPC_STAY_1S_COUNT      50 // unit: 20mS, 50x20=1000mS
#define DPC_SPEEDUP_SWITCH_PORT()      (DPCCableNotConnect() && (InputTimingStableCounter >= DPC_STAY_1S_COUNT))
#endif
#endif

#define TMDS_TOLERANCE     2

//#define HFreq_Torlance     12 // 15 // 1.5 KHz
//#define VFreq_Torlance     12 // 15 // 1.5 Hz

#if  ENABLE_DP_INPUT || ENABLE_HDMI
#define MaxInputHFreq      ((CURRENT_INPUT_IS_DISPLAYPORT()||CURRENT_INPUT_IS_HDMI())? ((CHIP_ID == CHIP_MT9701)?2200:1000):800)// 800  //FOR 3D
#define MinInputHFreq       ((CURRENT_INPUT_IS_DISPLAYPORT()||CURRENT_INPUT_IS_HDMI())? 100:100)  //for Dp interlace timing  //plz  HY conf spec
#else
#define MaxInputHFreq      800
#define MinInputHFreq       100//300 100831 code test
#endif
#define MaxInputVFreq      ((CHIP_ID == CHIP_MT9701)?1810:800)//750

#define MinInputVFreq       230 // MHL/DP/HDMI

#define DEF_CHECK_AGIAN_ON_THD          5   //times
#define DEF_FAKE_SLEEP_TIME_OUT_THD     10 //sec.

//extern InputModeType code StandardMode[];
extern BYTE xdata InputTimingStableCounter;

extern Bool mStar_FindMode( void );
BYTE mStar_GetInputStatus( void );
Bool IsCableNotConnected( void );
Bool mStar_SyncLossStateDetect( void );
Bool mStar_ValidTimingDetect( void );
#if ENABLE_SPEEDUP_SWITCH
#if ENABLE_HDMI
Bool HDMICableNotConnect(void);
#endif
#if ENABLE_DP_INPUT
Bool DPCableNotConnect(void);
#endif
#if ENABLE_USB_TYPEC
Bool DPCCableNotConnect(void);
#endif
void SpeedupOrExtendSwitchPort(void);
#endif
#if ENABLE_HDMI || ENABLE_DVI || ENABLE_DP_INPUT
extern ST_COMBO_COLOR_FORMAT GetInputCombColorFormat(void);
Bool mStar_MonitorColorSpaceChange(void);
#endif
#if ENABLE_HDMI
Bool mStar_MonitorHDMIAvmute(BOOL bFreeRunModeChk);
#endif

#if ENABLE_YUV420
void mStar_Monitor420Status( BYTE InputPort);
#endif
#if ENABLE_HDMI_ALLM
BOOL mStar_CheckLowLatecyMode(void);
void mStar_MonitorALLMStatus( BYTE InputPort );
#endif

Bool xdata bHDMIFreesyncChk = FALSE;
static WORD g_u16CheckAgainOnThd = DEF_CHECK_AGIAN_ON_THD;
static MS_S16 g_s16FakeSleepTimeOutCnt = -1;
static WORD g_u16FakeSleepTimeOutThd = DEF_FAKE_SLEEP_TIME_OUT_THD;
static BOOL g_bFakeSleepTimeOutEn = TRUE;

#if ENABLE_HPERIOD_16LINE_MODE
void mStar_HperiodDetModeSwitch(void)
{
    WORD u16HfreqIn, u16HttIn, u16InputStable = 0;
    BOOL bModeEn;
    
    if(!InputTimingStableFlag)
    {
#if ENABLE_HDMI
        if(CURRENT_INPUT_IS_TMDS())
            u16InputStable = msAPI_combo_HDMIRx_GetVideoContentInfo(COMBO_VIDEO_CLK_STABLE, SrcInputType);
#endif
#if ENABLE_DP_INPUT
        if(CURRENT_INPUT_IS_DISPLAYPORT())
            u16InputStable = mapi_DPRx_CheckDPTimingStable(SrcInputType);
#endif

        u16HttIn = msAPI_combo_IPGetGetHTotal();
        if(
#if ENABLE_VGA_INPUT
            (!CURRENT_INPUT_IS_VGA()) && 
#endif
            u16InputStable &&
            u16HttIn)
        {
            u16HfreqIn = (DWORD)(msAPI_combo_IPGetPixelClk() * 1000) / u16HttIn; // unit: KHz
            if(u16HfreqIn > 25) // detect counter will overflow with 16 line mode if Hfreq < 25K
                bModeEn = TRUE; // 16 line mode
            else
                bModeEn = FALSE; // 8 line mode     
        }
        else
            bModeEn = FALSE; // 8 line mode

        if(g_16ModePre != bModeEn)
        {
            g_16ModePre = bModeEn;
            SC0_HPEROID_DETECT_MODE(bModeEn);
            Set_InputTimingChangeFlag();
            DETECT_printData("Hperiod detect mode change: %d\n", g_16ModePre);
        }
    }
}
#endif

//*******************************************************************
// Function Name: mStar_MonitorInputTiming
//
// Decscription: Detect input timing
//
// callee: mStar_SyncLossStateDetect(), mStar_ValidTimingDetect(), mStar_AutoDVI() in detect.c
//         mStar_SetInputPort(), mStar_SetAnalogInputPort() in mstar.c
//
// caller: main() in main.c
//*******************************************************************
void mStar_MonitorInputTiming( void )
{
    if(!PowerOnFlag)
    {
        return ;
    }

    //EDID program when cable not connected
    if( SyncLossState() && IsCableNotConnected() )
    {
        hw_ClrDDC_WP();
    }
    else
    {
        hw_SetDDC_WP();
    }

#if ENABLE_YUV420
    mStar_Monitor420Status(SrcInputType);
#endif
#if ENABLE_HDMI_ALLM
    mStar_MonitorALLMStatus(SrcInputType);
#endif
 
#if ENABLE_HPERIOD_16LINE_MODE
    mStar_HperiodDetModeSwitch();
#endif

    // detect whether input signal is changing
    if( !InputTimingChangeFlag )
    {
        if( SyncLossState() )
        {

            if( !FreeRunModeFlag )
            {
                Set_InputTimingChangeFlag();
            }
            else if( mStar_SyncLossStateDetect() )                // detect mode while no Sync/suspend/standby/disconnect
            {
                Set_InputTimingChangeFlag();
            }
#if !DISABLE_AUTO_SWITCH
            else if( InputTimingStableCounter && ( SwitchPortCntr > Input_Nums ) && (UserPrefInputPriorityType==Input_Priority_Auto)
#if ENABLE_DP_INPUT
            && (!UserPrefMSTOnOff)
#endif
            )
            {
#if ENABLE_SPEEDUP_SWITCH
                SpeedupOrExtendSwitchPort();
#endif
                if( InputTimingStableCounter % SyncLossStableCount == 0 )
                {
                    InputTimingStableCounter = 1; // 071224
                    SrcInputType = ( SrcInputType + 1 ) % Input_Nums;
                    mStar_SetupInputPort();
                    DETECT_printData(" PS_SwitchTo:%d", SrcInputType);
                }
            }
#endif
        }
        else            // valid timing
        {
            // check if input timing is changing
            if( mStar_ValidTimingDetect()
#if ENABLE_HDMI || ENABLE_DVI || ENABLE_DP_INPUT
                ||  mStar_MonitorColorSpaceChange()
#endif
                )
            {
                SrcFlags |= SyncLoss;
                Set_InputTimingChangeFlag();
                if( !DisplayLogoFlag )
                    hw_ClrBlacklit(); // 2006/9/7 8:39PM by Emily for changed mode had garbage
            }

        }
    }
    //=============================================================================
    //=============================================================================
    if( InputTimingChangeFlag )        // input timing is unstable
    {
        if( g_bAccOnOff )
        {
            msAccOnOff( 0 );
        }
        // 2006/7/6 0:8AM by Emily for dec. dot clock
    #if DECREASE_V_SCALING
        DecVScaleValue = 0;
    #endif
        Clr_InputTimingChangeFlag();
        Clr_DoModeSettingFlag();
        Clr_ReadyForChangeFlag();
        BootTimeStamp_Set(AP_LOOP_TS, 1, FALSE); // ap loop timestamp 1

#if ENABLE_DEBUG_TIMING_CHANGE_POINT
        if(TimingChangeFirstPoint)
           printData(" TimingChangeFirstPoint:%d", TimingChangeFirstPoint);
#endif

        bHDMIFreesyncChk = TRUE;
        //g_bHDMIFreesyncEnable = FALSE;

        mStar_PrepareForTimingChange();
    }
    else        // input timing is stable
    {
        InputTimingStableCounter++; // counter timing stable times

        if( InputTimingStableCounter == 0 )
            InputTimingStableCounter = 1;

        if((g_eBootStatus == eBOOT_STATUS_CHECKAGAINON) && (!SyncLossState()) && (InputTimingStableCounter >= g_u16CheckAgainOnThd))
        {
            g_eBootStatus = eBOOT_STATUS_DPMSON;
            DETECT_printMsg("Signal stable over CheckAgainOnThd, exit eBOOT_STATUS_CHECKAGAINON\n");
        }

        if( !InputTimingStableFlag )
        {
#if ENABLE_SPEEDUP_SWITCH
            SpeedupOrExtendSwitchPort();
#endif

#if Enable_ValidTimingStablePanelTurnOn
            if(g_eBootStatus != eBOOT_STATUS_CHECKAGAINON)
            {
                if( !SyncLossState() && InputTimingStableCounter >= 1 )
                {
                    Power_PanelCtrlOnOff(TRUE, FALSE);
                }
            }
#endif

            if(( !SyncLossState() && InputTimingStableCounter >= ValidTimingStableCount ) ||
            ( SyncLossState() && InputTimingStableCounter >= SyncLossStableCount ) )
            {
                Set_InputTimingStableFlag(); // set flag to search for input mode
                Set_DoModeSettingFlag();
                BootTimeStamp_Set(AP_LOOP_TS, 2, TRUE); // ap loop timestamp 2

#if !DISABLE_AUTO_SWITCH
                g_CountSwitchPortTimeFlag=FALSE;
                InputTimingStableCounter = 1;

                if( !SyncLossState() )
                {
                    SwitchPortCntr = 0;
                    g_SwitchSec = DEF_FORCE_DPMS;

#if ENABLE_DEBUG_TIMING_CHANGE_POINT
                    ResetTimingChangeFirstPoint();
#endif
                }

                if( SyncLossState() && (( ++SwitchPortCntr <= Input_Nums ) || (UserPrefPowerSavingEn == PowerSavingMenuItems_Off)) && (UserPrefInputPriorityType==Input_Priority_Auto)
#if ENABLE_DP_INPUT
                && (!UserPrefMSTOnOff)
#endif
                )
                {
                    #if 0//ENABLE_DEBUG
                    if(CURRENT_INPUT_IS_DVI())
                    {
                        old_msWriteByteMask(REG_0280, BIT4, BIT4);
                        while(old_msReadByte(REG_0280)&BIT4);
                    }
                    #endif
                    Clr_InputTimingStableFlag(); // set flag to search for input mode
                    Clr_DoModeSettingFlag();
                    SrcInputType = ( SrcInputType + 1 ) % Input_Nums;
                    mStar_SetupInputPort();
                }
#endif

            }


        }
    }
}

#if ENABLE_HDMI
//*******************************************************************
// Function Name: mStar_MonitorHDMIAvmute
//
// Decscription: Detect input HDMI AVMUTE
//
// callee: msAPI_combo_IPGetAVMuteEnableFlag
//
// caller: mStar_MonitorInputTiming/mStar_ModeHandler
//*******************************************************************
Bool mStar_MonitorHDMIAvmute(BOOL bFreeRunModeChk)
{
	if(( (bFreeRunModeChk) && (!FreeRunModeFlag))|| ((!bFreeRunModeChk) && (FreeRunModeFlag) ))
	{
		return FALSE;
	}
        if( INPUT_IS_TMDS(SrcInputType) && InputTimingStableFlag )
        {
            if( msAPI_combo_IPGetAVMuteEnableFlag(SrcInputType) )
            {
                return TRUE;
            }
        }

    return FALSE;
}
#endif

#if ENABLE_HDMI || ENABLE_DVI || ENABLE_DP_INPUT
Bool mStar_MonitorColorSpaceChange(void)
{
    ST_COMBO_COLOR_FORMAT stCfPre, stCfCur;

   if((SrcInputType != Input_VGA) && InputTimingStableFlag && (!FreeRunModeFlag))
   {
        stCfPre = GetInputCombColorFormat();
        stCfCur = msAPI_combo_IPGetColorFormat(SrcInputType);
        if(stCfPre.ucColorType != stCfCur.ucColorType)
            return TRUE;
   }
   return FALSE;
}
#endif

//*******************************************************************
// Function Name: mStar_ModeHandler
//
// Decscription: Programming scaler while input timing is changing
//
// callee: mStar_SyncLossStateDetect(), mStar_ValidTimingDetect(), mStar_AutoDVI() in detect.c
//         mStar_SetInputPort(), mStar_SetAnalogInputPort() in mstar.c
//
// caller: main() in main.c
//*******************************************************************
void mStar_CheckFailSafeMode( WORD Checkhfreq, WORD Checkvreq )
{
    if (( Checkhfreq > MaxInputHFreq || Checkhfreq < MinInputHFreq
     ||((!IS_HDMI_FREESYNC()) && Checkvreq > MaxInputVFreq) || ((!IS_HDMI_FREESYNC()) && Checkvreq < MinInputVFreq) )
#if 0 // LiteMax
        ||(StandardModeWidth >= g_sPnlInfo.sPnlTiming.u16Width && StandardModeHeight > g_sPnlInfo.sPnlTiming.u16Height)
        ||(StandardModeWidth > g_sPnlInfo.sPnlTiming.u16Width && StandardModeHeight >= g_sPnlInfo.sPnlTiming.u16Height))
#else
        )
#endif
    {
        SrcFlags |= bUnsupportMode;
    }
}

#if AudioFunc
void mStar_SettingAudioFucntion( void)
{
    if(!SyncLossState() && !UnsupportedModeFlag)
    {
    #if ENABLE_DAC
        if(!audio_EnableAudioAfterSignalLock_Flag)
        {
            audio_EnableAudioAfterSignalLock_Flag = 1;
            
            if ((UserPrefAudioSource == AudioSourceMenuItems_LineIn) && (CURRENT_INPUT_IS_HDMI() || CURRENT_INPUT_IS_DISPLAYPORT()))
                msDrvAudioSourceSel(AUDIO_LINE_IN);
            else
                msAPI_AudioSourceSel();

            #if(CHIP_ID == CHIP_MT9701)
                msAPI_AudioPathSet();
            #endif
        }
    #endif
    
        if(bPanelOnFlag)
        {
            msAPI_AdjustVolume(UserPrefVolume);

        #if (ENABLE_SPDIF && (!ENABLE_ARC))
            msAPI_AudioSpdifMute(FALSE);
        #endif
        }
    }
    else
    {
        msAPI_AdjustVolume(0);
    }
}
#endif // end of #if AudioFunc

void mStar_ModeHandler( void )
{
    if( DisplayLogoFlag )
    {
        Menu_InitAction();
    }
    if(( !DoModeSettingFlag )
#if ENABLE_HDMI && (CHIP_ID == CHIP_MT9700)
        ||(mStar_MonitorHDMIAvmute(TRUE))
#endif
       )
        // while timing change & stable
    {
        return ;
    }

#if ENABLE_DAC
    if(!SyncLossState() && !msAPI_AudioInitDone())
        return;
#endif

    //Clr_DoModeSettingFlag();
    if( !SyncLossState() )
        // no sync
    {
        if(FakeSleepFlag)
        {
            DETECT_printMsg("SIGNAL, EXIT FAKE SLEEP\n");
            Clr_FakeSleepFlag();
            FakeSleepCounter = 0;
        }

        SettingInputColorimetry();
        if( !ReadyForChangeFlag )
        {
            if( !mStar_FindMode() )                // search mode index
            {
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
                SetTimingChangeFirstPoint(TimingChange_FindMode);
#endif
                Set_InputTimingChangeFlag(); // search mode faile
                return ;
            }
            else
                BootTimeStamp_Set(AP_LOOP_TS, 3, TRUE); // ap loop timestamp 3
        }

        if( !UnsupportedModeFlag )
            //supported mode
        {
            if( !ReadyForChangeFlag )
            {
                ReadModeSetting(); // restore user's settings from NVRAM for each mode
                Set_ReadyForChangeFlag();
            }

            if( DisplayLogoFlag )
            {
                return ;
            }

            if( mStar_SetupMode() == FALSE )                // setup scaler
            {
                Set_InputTimingChangeFlag(); // setup mode failed
                mStar_SetupFreeRunMode(); // setup freerun mode
                return ;
            }
            else
                BootTimeStamp_Set(AP_LOOP_TS, 4, TRUE); // ap loop timestamp 4

        }

    }
    Set_ReadyForChangeFlag();

    if( DisplayLogoFlag )
    {
        return ;
    }

    if( SyncLossState() || UnsupportedModeFlag )
    {
        mStar_SetupFreeRunMode(); // setup freerun mode
        Power_TurnOnGreenLed();
    }
    else
    {
        WORD vfreq = 0, hfreq = 0;

        if(IS_DP_FREESYNC())
        {
            WORD u16PixClk, u16Htt;

            u16PixClk = mapi_DPRx_GetPixelClock10K(SrcInputType);//msAPI_combo_IPGetPixelClk();
            u16Htt = msAPI_combo_IPGetGetHTotal();
            if((u16Htt!=0)&&(SrcVTotal!=0))
            {
                hfreq = ((DWORD)u16PixClk * 100 + u16Htt/2)/u16Htt;
                vfreq = ((DWORD)hfreq * 1000 + SrcVTotal/2)/SrcVTotal;
            }
        }
        else
        {
            hfreq = HFreq( SrcHPeriod );
            vfreq = VFreq( hfreq, SrcVTotal );
        }

        if( CURRENT_SOURCE_IS_INTERLACE_MODE() )
            vfreq *= 2;

        //new OOR Spec.
        mStar_CheckFailSafeMode(hfreq, vfreq);    // 110919 Rick add for detect fail safe mode - A025 //110922 Rick Modified

        if( SyncLossState() || UnsupportedModeFlag )
        {
            mStar_SetupFreeRunMode(); // setup freerun mode
            Power_TurnOnGreenLed();
        }

        if( StandardModeHeight <= 400 ||
                ( StandardModeHeight == 480 && ( vfreq > 575 && vfreq < 650 ) && SrcModeIndex != 0 ) )
        {
            UserPrefAutoTimes = 1;
        }
        if( PowerOnFlag )
            // system is on power-off status
        {
            Power_TurnOnGreenLed(); // turn on green led
            //Power_TurnOffAmberLed(); // turn off amber led
        }
    }

    if( !CableNotConnectedFlag || !SyncLossState() )
    {
        if( DoBurninModeFlag )
        {
            //if (!ProductModeFlag)                   //110916 Rick add condition for A027
			Clr_BurninModeFlag();
            Clr_DoBurninModeFlag();
            //old_msWriteByte( BK0_32, 0x00 ); // disable background color function.
            drvOSD_FrameColorEnable(FALSE);
        }
    }
    Clr_DoModeSettingFlag(); //0606

    BootTimeStamp_Set(AP_LOOP_TS, 5, TRUE); // ap loop timestamp 5
#if ENABLE_SUPER_RESOLUTION
    mStar_SetupSuperResolution( UserPrefSuperResolutionMode );
#endif

    BootTimeStamp_Set(AP_LOOP_TS, 6, TRUE); // ap loop timestamp 6
#if ENABLE_SHARPNESS
    msAdjustSharpness( MAIN_WINDOW, UserPrefSharpness, 0 );
#endif
    mStar_AdjustBrightness(UserPrefBrightness);
#if DISABLE_SHORT_LINE_TUNE
    msWriteByteMask(SC00_27,0x00,BIT1);
#endif
    BootTimeStamp_Set(AP_LOOP_TS, 7, TRUE); // ap loop timestamp 7
    SetDisplayOKFlag();

    if(g_eBootStatus != eBOOT_STATUS_CHECKAGAINON)
    {
    #if Enable_PanelHandler
        Power_PanelCtrlOnOff(TRUE, TRUE);
    #else
    #if Enable_ReducePanelPowerOnTime
        Power_ForcePowerOnPanel();
    #else
        Power_TurnOnPanel();    // turn on panel
    #endif
    #endif
    }
    BootTimeStamp_Set(AP_LOOP_TS, 8, TRUE); // ap loop timestamp 8
    Menu_InitAction();      // initial menu OSD state
    BootTimeStamp_Set(AP_LOOP_TS, 9, TRUE); // ap loop timestamp 9   
#if AudioFunc
    #if (CHIP_ID == CHIP_MT9701) && ENABLE_DAC
        msAPI_AudioInputPortSet(SrcInputType);
    #endif
    
    mStar_SettingAudioFucntion();
#endif
    BootTimeStamp_Set(AP_LOOP_TS, 10, TRUE); // ap loop timestamp 10 
}

void mStar_PrepareForTimingChange( void )
{

    #if 0//DEBUG_PRINT_ENABLE
    printData( "PrepareForTimingChange SrcInputType=%d", SrcInputType );
    #endif

#if ENABLE_DAC
    audio_EnableAudioAfterSignalLock_Flag=0;
    msAPI_AudioSetDpgaMute(TRUE);

#if ENABLE_SPDIF
    msAPI_AudioSpdifMute(TRUE);
#endif

#if (TTS && (CHIP_ID == CHIP_MT9701))
    msAPI_AudioTTSMixEnable(FALSE);
#endif

#endif

    #if ENABLE_HDMI // wait for checking
    /*
    if( gScInfo.InputTmdsType == TMDS_HDMI )
        mstar_HDMIAudioMute( HDMI_AUDIO_OFF );
    mstar_HDMIInitialVariable();
   //mstar_HDMIResetColorSpace();
    mstar_HDMIRst(Reset_HDMIStatus);
    */
    #endif
    Clr_ForcePowerSavingFlag();
    PowerDownCounter = 0;
    InputColorFormat = INPUTCOLOR_RGB;
    if( !DisplayLogoFlag )
    {
        if( PowerSavingFlag )
        {
            Power_PowerOnSystem();
            mStar_SetupFreeRunMode();
        }
        else
        {
            mStar_SetupFreeRunMode(); // keep clock output then turn off panel
#if Enable_PanelHandler
            if(Power_PanelCtrlState_Get() == (ePANEL_STATE_MAX-1))
            {
                Power_PanelCtrlOnOff(FALSE, TRUE);
            }
#else
            Power_TurnOffPanel();
#endif
        }
        Osd_Hide();

  #if ENABLE_DVI_DUAL_LINK //Jison 110429 reset to single to speed up dvi dual-->single timing stable
            if (CURRENT_INPUT_IS_DVI() && SyncLossState())
                msDVIDualLinkMode(FALSE);
  #endif

    }

#if UseINT
    mStar_EnableModeChangeINT(FALSE);
#endif
#ifdef _HW_AUTO_NO_SIGNAL_
    mStar_EnableAutoNoSignal(FALSE);
#endif
    if( InputTimingStableFlag )
    {
        ScalerReset(RST_IP);
    }
    Clr_InputTimingStableFlag(); // reset input timing stable and wait for timing stable
    InputTimingStableCounter = 0;
    SrcFlags &= ~( bUnsupportMode | bUserMode | bNativeMode );
    //g_bInputSOGFlag = 1; // set to 1 for timing change switch to separate HV first
    //old_msWriteByte(SC0_01, 0x00);        // disable double bufer.
    //mStar_ScalerDoubleBuffer(FALSE);
    //mStar_WriteByte(REGBK, REGBANKADC);
    //old_msWriteByte(SC0_01, 0x00);        // enable ADC's double bufer.
    //mStar_WriteByte(REGBK, REGBANKSCALER);
    msWriteByteMask(SC00_E8,0,(BIT0|BIT1));
    msWriteByteMask(SC00_E9,0,BIT0);
    #if CHIP_ID == CHIP_MT9701
    msWriteByte(REG_LPLL_19, 0x01); //[3:0] input frame div for frame sync, [7:4] output frame div for frame sync
    msWriteByte(REG_LPLL_1C, 0x00); //[3:0] input frame div[7:4], [7:4] output frame div [7:4]
    msWriteByteMask(REG_LPLL_18, 0, BIT3);
    msWriteByteMask(REG_LPLL_E1, 0x00, BIT7|BIT3);//fast lock mode disable
    msWriteByteMask(REG_LPLL_1B, 0, BIT1|BIT0);//disable ivs detect no signal/stable power down
    #endif
#ifdef _HW_AUTO_NO_SIGNAL_
#if (CHIP_ID == CHIP_MT9701)
    msWriteByteMask(SC00_28,0,(BIT5|BIT4)); // [5]:reg_freesync_en(vcnt freeze), [4]:reg_no_signal_freesync_off
#endif
#endif
}
//==========================================================

#define DISALE_CABLEDETECT  (INPUT_TYPE!=INPUT_1A)//101215 Modify
Bool mStar_SyncLossStateDetect( void )
{
    BYTE fStatus;
    Bool result = FALSE;

    fStatus = mStar_GetInputStatus();
    //printData("status %xH", fStatus);//old_msReadByte(INSTA));
    //printData("stable counter %d", InputTimingStableCounter);
    if( fStatus & SyncLoss )
    {
#if ENABLE_VGA_INPUT
        if(CURRENT_INPUT_IS_VGA())
        {
            if( InputTimingStableCounter && (InputTimingStableCounter % ValidTimingStableCount == 0) )
                mStar_SetAnalogInputPort((g_bInputSOGFlag?FALSE:TRUE));

            fStatus = mStar_GetInputStatus();
            if( !( fStatus & SyncLoss ) )
                goto SyncActive;
        }
#endif

        if( CableNotConnectedFlag )  // no Cable
        {
            if( !IsCableNotConnected() )
            {
                Clr_CableNotConnectedFlag();
#if DISALE_CABLEDETECT
#else
                result = TRUE;
#endif
            }
        }
        else if( IsCableNotConnected() ) // no Sync
        {
            Set_CableNotConnectedFlag();
#if DISALE_CABLEDETECT
#else
            //Power_TurnOnGreenLed();
            Power_TurnOnGreenLed();
            result = TRUE;
#endif
        }

        if( PowerSavingFlag )
        {
            if(CURRENT_INPUT_IS_TMDS())
            {
#if ((CHIP_ID == CHIP_TSUMC) || (CHIP_ID == CHIP_TSUMD) || (CHIP_ID == CHIP_TSUMJ)|| (CHIP_ID == CHIP_TSUM9)|| (CHIP_ID == CHIP_TSUMF)||(CHIP_ID == CHIP_MT9700))
                if(DVI_CLK_STABLE())
#else
                if(!DVI_CLOCK_IS_MISSING())//(!(old_msReadByte(SC0_CA) & 0x40)) // coding check stable or exist
#endif
                {
                    //Delay1ms(150);
                    Delay1ms( 50 );
                    mStar_PowerUp();
                    Delay1ms( 50 );
                    fStatus = mStar_GetInputStatus();
                    if( fStatus & SyncLoss )
                    {
                        mStar_PowerDown();
                    }
                    else
                    {
                        result = TRUE;
                    }
                }
            }
            /********** wmz 20051117
            else if (SrcInputType==Input_YPbPr && old_msReadByte(BK0_E1)&0x50)
            { if (abs(HFreq(mStar_ReadWord(BK0_E5))-156)<20)
            { fStatus&=~SyncLoss;
            result=TRUE;
            }
            }
             ***************/
        }
    }
    else
    {
#if ENABLE_VGA_INPUT
    SyncActive:
#endif
        result = TRUE;
    }
    if( result )
    {
        SrcFlags = ( SrcFlags & 0xF0 ) | fStatus;
    } // clear others flags, except sync informat (polarity & w/o)
    return result;
}

/*
Bool mStar_IsStatusChanged( void )
{
    // prevent input status change when setup mode.
    if( old_msReadByte( SC0_E1 ) &INTM_B )
    {
        if( !( SrcFlags & bInterlaceMode ) )
        {
            Set_InputTimingChangeFlag();
            return TRUE;
        }
    }
    else if( SrcFlags & bInterlaceMode )
    {
        Set_InputTimingChangeFlag();
        return TRUE;
    }
    return FALSE;
}
*/

////////////////////////////////////////////////////////////
//  1080i@50  CEA standard format_39: SrcVTotal = 1250, pixel clock = 72MHz //
////////////////////////////////////////////////////////////
void NewModeInterlacedDetect(void)
{
    static BOOL _NewModeInterlacedDetect = FALSE;
    WORD tempWidth = 0, tempHeight = 0;

    if(CURRENT_INPUT_IS_TMDS())
    {
        tempWidth = SC0_READ_AUTO_WIDTH();
        tempHeight = SC0_READ_AUTO_HEIGHT();
        if((tempWidth > 1915) && (tempWidth < 1925)
            && (tempHeight > 535) && (tempHeight < 545)
            && (SrcVTotal > 620) && (SrcVTotal < 630)
            && (SrcVFreq > 498) && (SrcVFreq < 504)
            && (!CURRENT_SOURCE_IS_INTERLACE_MODE()))
        {
            // enable new mode interlaced detect
            msWriteByteMask(SC00_E9, (BIT0|BIT1), (BIT0|BIT1)); //h74[8] Interlace Field Invert
                                                              //h74[9] 1: Use the relationship of VSYNC and HSYNC to judge
            _NewModeInterlacedDetect = TRUE;
            SrcFlags |= bInterlaceMode;
            Delay1ms(100);
        }
        else if(_NewModeInterlacedDetect
            && (!((SrcVFreq > 498) && (SrcVFreq < 504)
            && (SrcVTotal > 1245) && (SrcVTotal < 1255)
            && (CURRENT_SOURCE_IS_INTERLACE_MODE()))))
        {
            // disable new mode interlaced detect
            msWriteByteMask(SC00_E9, 0, (BIT0|BIT1));
            _NewModeInterlacedDetect = FALSE;
        }
    }
    else
    {
        msWriteByteMask(SC00_E9, 0, (BIT0|BIT1));
    }
}

Bool mStar_ValidTimingDetect( void )
{
    BYTE fStatus;
    BOOL bIs16Line = SC0_HPERIOD_16LINE_MODE();

    fStatus = mStar_GetInputStatus();
    if( fStatus & SyncLoss )        // no sync
    {
        SrcFlags = ( SrcFlags & 0xF0 ) | fStatus; // clear others flags, except sync informat (polarity & w/o)

        DETECT_printMsg( "Valid Timing : Sync Loss" );

        return TRUE;
    }
    else
    {
#if ENABLE_HDMI && (CHIP_ID == CHIP_MT9700)
    	if(mStar_MonitorHDMIAvmute(FALSE))
    	{
	        return TRUE;
    	}
#endif
        if( SyncPolarity( SrcFlags ) != SyncPolarity( fStatus ) )            // Sync polarity changed
        {
            SrcFlags = ( SrcFlags & 0xF0 ) | fStatus; // clear others flags, except sync informat (polarity & w/o)

            DETECT_printMsg( "Valid Timing : Sync polarity changed" );

            return TRUE;
        }
        else
        {
            WORD tempPeriod;
            SrcFlags = ( SrcFlags & 0xF0 ) | fStatus; // clear others flags, except sync informat (polarity & w/o)
            tempPeriod = SC0_READ_HPEROID();//old_msRead2Byte(SC0_E4) & MASK_13BIT;
            if(tempPeriod == 0x1FFF)
            {
                DETECT_printMsg( "Valid Timing : H period overflow" );
            }
            
            if(bIs16Line)
                tempPeriod = (tempPeriod + 8) / 16;
            else
                tempPeriod = (tempPeriod + 4) / 8;
            
            if( InputTimingStableCounter == 0 )
            {
                SrcHPeriod = tempPeriod;
            }

            if(!IS_DP_FREESYNC())
            {
                #if DEBUG_PRINT_ENABLE
                // printData("HPeriod1 %d", tempPeriod);
                #endif
                if( labs( (DWORD)tempPeriod - SrcHPeriod ) > HPeriod_Torlance )
                    // HPeriod changed
                {
                    tempPeriod = SC0_READ_HPEROID();//old_msRead2Byte(SC0_E4) & MASK_13BIT;

                    if(bIs16Line)
                        tempPeriod = (tempPeriod + 8) / 16;
                    else
                        tempPeriod = (tempPeriod + 4) / 8;
                    
                    if( labs( (DWORD)tempPeriod - SrcHPeriod ) > HPeriod_Torlance )
                    {
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
                        SetTimingChangeFirstPoint(TimingChange_SrcHPeriod);
#endif
                    return TRUE;
                    }
                }
            }

#if ENABLE_DP_INPUT
            if( CURRENT_INPUT_IS_DISPLAYPORT() )
            {
                if(msAPI_combo_DP_GetMSAChg( SrcInputType ) )
                {
                    msAPI_combo_DP_ClrMSAChg( SrcInputType );
                    return TRUE;
                }
            }
#endif

            tempPeriod = SC0_READ_VTOTAL();//old_msRead2Byte(SC0_E2) & MASK_11BIT;
            if( InputTimingStableCounter == 0 )
            {
                // 100902 coding addition
                Delay1ms( 100 );
                tempPeriod = SC0_READ_VTOTAL();//old_msRead2Byte(SC0_E2) & MASK_11BIT;
                SrcVTotal = tempPeriod;
            }
            
            NewModeInterlacedDetect();

            if(!IS_HDMI_FREESYNC())
            {
                #if DEBUG_PRINT_ENABLE
                // printData("  VTotal1 %d", tempPeriod);
                #endif
                if( abs( tempPeriod - SrcVTotal ) > VTotal_Torlance )
                // vtotal changed
                {
#if ENABLE_HDMI // 100902 coding addition
                    Delay1ms( 10 );
                    tempPeriod = SC0_READ_VTOTAL();//old_msRead2Byte(SC0_E2) & MASK_11BIT;
                    if( labs( (DWORD)tempPeriod - SrcVTotal ) > VTotal_Torlance ) // vtotal changed
#endif
                    {
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
                        SetTimingChangeFirstPoint(TimingChange_SrcVTotal);
#endif
                        return TRUE;
                    }
                }
            }

#if ENABLE_FREESYNC
            if(IS_DP_FREESYNC())
            {
                tempPeriod = msAPI_combo_IPGetGetHTotal();

                if( InputTimingStableCounter == 0 )
                {
                    tempPeriod = msAPI_combo_IPGetGetHTotal();

                    SrcHTotalMSA = tempPeriod;
                }
                if( tempPeriod != SrcHTotalMSA)//MSA Htotal change
                {
                    return TRUE;
                }
            }
#endif
#if ENABLE_CHECK_TMDS_DE
            if( CURRENT_INPUT_IS_TMDS() )
            {
                if(!InputTimingStableFlag && (InputTimingStableCounter >= (ValidTimingStableCount-1)))
                {
                    SrcTMDSWidth = SC0_READ_AUTO_WIDTH();
                    SrcTMDSHeight = SC0_READ_AUTO_HEIGHT();
                    SrcTMDSHStart = SC0_READ_AUTO_START_H();
                    SrcTMDSVStart = SC0_READ_AUTO_START_V();
                }

                if(InputTimingStableFlag)
                {
                    tempPeriod = SC0_READ_AUTO_WIDTH();
                    if( labs( (DWORD)tempPeriod - SrcTMDSWidth ) > TMDS_TOLERANCE )
                    {
                        tempPeriod = SC0_READ_AUTO_WIDTH();
                        if( labs( (DWORD)tempPeriod - SrcTMDSWidth ) > TMDS_TOLERANCE )
                        {
                            DETECT_printData( "SrcTMDSWidth changed%d", tempPeriod );

#if ENABLE_DEBUG_TIMING_CHANGE_POINT
                            SetTimingChangeFirstPoint(TimingChange_SrcTMDSWidth);
#endif
                            return TRUE;
                        }
                    }

                    tempPeriod = SC0_READ_AUTO_HEIGHT();
                    if( labs( (DWORD)tempPeriod - SrcTMDSHeight ) > TMDS_TOLERANCE )
                    {
#if ENABLE_HDMI
                        Delay1ms( 20 );
                        tempPeriod = SC0_READ_AUTO_HEIGHT();
                        if( labs( (DWORD)tempPeriod - SrcTMDSHeight ) > TMDS_TOLERANCE )
#endif
                        {
                            DETECT_printData( "SrcTMDSHeight changed%d", tempPeriod );
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
                            SetTimingChangeFirstPoint(TimingChange_SrcTMDSHeight);
#endif
                            return TRUE;
                        }
                    }

                    tempPeriod = SC0_READ_AUTO_START_H();//old_msRead2Byte( SC0_80 ) & MASK_12BIT;
                    if( labs( (DWORD)tempPeriod - SrcTMDSHStart ) > TMDS_TOLERANCE )
                    {
                        DETECT_printData( "SrcTMDSHStart changed%d", tempPeriod );
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
                        SetTimingChangeFirstPoint(TimingChange_SrcTMDSHStart);
#endif
                        return TRUE;
                    }

                    tempPeriod = SC0_READ_AUTO_START_V();//old_msRead2Byte( SC0_7E ) & MASK_11BIT;
                    if( labs( (DWORD)tempPeriod - SrcTMDSVStart ) > TMDS_TOLERANCE )
                    {
                        DETECT_printData( "SrcTMDSVStart changed%d", tempPeriod );
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
                        SetTimingChangeFirstPoint(TimingChange_SrcTMDSVStart);
#endif
                        return TRUE;
                    }
                }

            }
#endif

        }
    }
    return FALSE;
}

#if  1//MS_VGA_SOG_EN
BYTE GetVSyncWidth(void)
{
    BYTE u8VSyncWidth;
    SC0_VSYNC_WIDTH_REPORT(TRUE);//old_msWriteByteMask(SC0_FA,BIT0,BIT0);
    u8VSyncWidth=SC0_READ_VSYNC_WIDTH();//old_msReadByte(SC0_E2);
    SC0_VSYNC_WIDTH_REPORT(FALSE);//old_msWriteByteMask(SC0_FA,0,BIT0);
    //PRINT_DATA("u8VSyncWidth=%d",u8VSyncWidth);
    return u8VSyncWidth;
}
#endif

BYTE mStar_GetInputStatus( void )
{
    BYTE fStatus = 0;
    WORD inputValue = 0;
    BYTE status;

    if(!IS_DP_FREESYNC())
    {
        inputValue = SC0_READ_HPEROID();//old_msRead2Byte(SC0_E4) & MASK_13BIT;
        if( inputValue == 0x1FFF )
        {
            fStatus |= bHSyncLoss;
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
            SetTimingChangeFirstPoint(TimingChange_Hperoid);
#endif
        }

        if(SC0_HPERIOD_16LINE_MODE())
            inputValue = (inputValue + 8) / 16;
        else
            inputValue = (inputValue + 4) / 8;
        
        if(inputValue < 10)
        {
            fStatus |= bHSyncLoss;
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
            SetTimingChangeFirstPoint(TimingChange_Hperoid);
#endif
        }
    }

    if(!IS_HDMI_FREESYNC())
    {
        inputValue = SC0_READ_VTOTAL();//old_msRead2Byte(SC0_E2) & MASK_11BIT;
        if( inputValue == 0x1FFF || inputValue < 200 )
        {
            fStatus |= bVSyncLoss;
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
            SetTimingChangeFirstPoint(TimingChange_Vtotal);
#endif
        }
    }

    status = SC0_READ_SYNC_STATUS();//old_msReadByte(SC0_E1);

    fStatus |= ( status & 0x03 ); // Get input timing polarity

    if(CURRENT_INPUT_IS_TMDS())    // input timing is valid while current state is no sync
    {
        #if ENABLE_HDMI
        if(msAPI_combo_HDMIRx_GetVideoContentInfo(COMBO_VIDEO_DE_STABLE, SrcInputType) == FALSE)
        {
            if(msAPI_combo_HDMIRx_GetVideoContentInfo(COMBO_VIDEO_DE_STABLE, SrcInputType) == FALSE)
            {
                fStatus |= SyncLoss;
                #if DETECT_DEBUG
                if((InputTimingStableCounter%5)==0)
                DETECT_printData("DVI DE Unstable", 1);
                #endif

                #if ENABLE_DEBUG_TIMING_CHANGE_POINT
                SetTimingChangeFirstPoint(TimingChange_TMDS_DE);
                #endif
            }
        }
        #endif
    }
    else if(CURRENT_INPUT_IS_DISPLAYPORT())    // input timing is valid while current state is no sync
    {
        #if ENABLE_DP_INPUT
        if(mapi_DPRx_CheckDPTimingStable(SrcInputType) == FALSE)
        {
            fStatus |= SyncLoss;
            DETECT_printMsg("DP Unstable\r\n");
        }
        #endif
    }
    else if( SyncLossState() && !( fStatus & SyncLoss ) )        // input timing is valid while current state is no sync
    {
        Delay1ms( 20 );
        if( InputTimingChangeFlag )
        {
            return fStatus;
        }
        status = status & SC0_READ_SYNC_STATUS();//old_msReadByte(SC0_E1);
        if(( status & 3 ) != ( fStatus & 3 ) )
            // polarity is stable
        {
            fStatus |= SyncLoss;
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
            SetTimingChangeFirstPoint(TimingChange_SyncPol);
#endif
        }
        else if( status & 0x30 )
            // SOG or CSync input
        {
            Delay1ms( 20 );
            if( InputTimingChangeFlag )
            {
                return fStatus;
            }
            status = SC0_READ_SYNC_STATUS();//old_msReadByte(SC0_E1);           // Check if SOG/CSYNC is valid
            if( labs( (DWORD)SC0_READ_VTOTAL() - inputValue ) > 2 )//( abs( (old_msRead2Byte( SC0_E2 )&MASK_11BIT) - inputValue ) > 2 )
            {
                fStatus |= SyncLoss;
            }
#if  MS_VGA_SOG_EN

else if (status&SOGD_B)
            {
                #if 1
                //if(!SyncLossState())
                 {
                    //BYTE i;
                    //for (i=0;i<10;i++)
                    {
                        inputValue=GetVSyncWidth();
                        if (inputValue>15||inputValue==0)
                        {
                            //printMsg("SOG Det Err");
                            fStatus |= SyncLoss;
                            //break;
                        }
                        //ForceDelay1ms(2);
                    }
                }
                #else
                {
                    BYTE u8Tmp0=SC0_READ_POWER_DOWN_STATUS();//old_msReadByte(SC0_F0);
                    BYTE u8Tmp1=SC0_READ_ADC_COAST_ENABLE_VALUE();//old_msReadByte(SC0_ED);
                    BYTE u8Tmp2=SC0_READ_ADC_COAST_START_VALUE();//old_msReadByte(SC0_EE);
                    BYTE u8Tmp3=SC0_READ_ADC_COAST_END_VALUE();//old_msReadByte(SC0_EF);
                    BYTE i;
                    //for get stable VSyncWidth
                    SC0_SCALER_POWER_DOWN(0, BIT0|BIT1); //to get vsyncWidth
                    SC0_ADC_COAST_ENABLE(0x21);//old_msWriteByte(SC0_ED, 0x21); //_NEW_SOG_WAKEUP_DET_
                    SC0_ADC_COAST_START(0x08);//old_msWriteByte(SC0_EE, 0x08);
                    SC0_ADC_COAST_END(0x05);//old_msWriteByte(SC0_EF, 0x05);
                    Delay1ms(20);
                    inputValue=0;
                    for (i=0;i<10;i++)
                    {
                        inputValue=GetVSyncWidth();
                        if (inputValue>15||inputValue=0)
                        {
                      //printMsg("SOG Det Err");
                            fStatus |= SyncLoss;
                            break;
                        }
                        ForceDelay1ms(2);
                    }
                    SC0_ADC_COAST_ENABLE(u8Tmp1);//old_msWriteByte(SC0_ED, u8Tmp1);
                    SC0_ADC_COAST_START(u8Tmp2);//old_msWriteByte(SC0_EE, u8Tmp2);
                    SC0_ADC_COAST_END(u8Tmp3);//old_msWriteByte(SC0_EF, u8Tmp3);
                    SC0_SCALER_POWER_DOWN(u8Tmp0, 0xFF);
                }
                #endif
            }

#endif
        }
    }
    return fStatus;
}

#ifndef CABLE_DET_SAR
#define CABLE_DET_SAR       0
#endif

#if (INPUT_TYPE == INPUT_1A && CABLE_DETECT_VGA_USE_SAR)
Bool IsCableNotConnected( void )
{
    BYTE retry_Key, temp, temp1;
    retry_Key = 3;
    while( retry_Key )
    {
        temp = CABLE_DET_SAR;
        Delay1ms( 2 );
        temp1 = CABLE_DET_SAR;
        if( abs( temp - temp1 ) < 3 )
            break;
        retry_Key--;
    }

    DETECT_printData( "CABEL ADC =%d", temp );

    if( temp > 190 )
        return TRUE;
    else
        return FALSE;
}
#elif (INPUT_TYPE == INPUT_1A && !CABLE_DETECT_VGA_USE_SAR)
Bool IsCableNotConnected( void )
{
#if CABLE_DETECT_DEBUG_ENABLE
    DETECT_printData( "hwDSUBCable_Pin %d", hwDSUBCable_Pin );
    DETECT_printData( "hwDVICable_Pin %d", hwDVICable_Pin );
#endif

    #if INPUT_TYPE==INPUT_1A
    return ( hwDSUBCable_Pin );
    #else
    #if (CHIP_ID == CHIP_TSUMJ) || (CHIP_ID == CHIP_TSUM9) || (CHIP_ID == CHIP_TSUMF)
    BYTE temp;
    temp = hwDSUBCable_Pin | hwDVI0Cable_Pin;
    if (temp && 0xFF)
        return TRUE;
    else
        return FALSE;
    #else
    return ( hwDSUBCable_Pin && hwDVICable_Pin );
    #endif
    #endif
}
#elif (LiteMAX_Baron_UI == 1)
Bool IsCableNotConnected( void )
{
	return hwSDM_AuxN_DP1Pin_Pin;
}
#else
#warning "no coding for  IsCableNotConnected"
Bool IsCableNotConnected( void )
{
    // wait for coding
    return FALSE;
}
#endif

#if ENABLE_SPEEDUP_SWITCH

#if ENABLE_HDMI
Bool HDMICableNotConnect(void)
{
    bit bresult = FALSE; // false means HDMI cable connect

#define IS_HDMI_PORT0_CABLE_CONNECT()     (!hwDVI0Cable_Pin) 
#define IS_HDMI_PORT1_CABLE_CONNECT()     (!hwDVI1Cable_Pin) 
#define IS_HDMI_PORT2_CABLE_CONNECT()     (!hwDVI2Cable_Pin) 

#if (CInput_HDMI_C1 != CInput_Nothing)
    if(CURRENT_INPUT_IS_HDMI0())
    {
        if(!IS_HDMI_PORT0_CABLE_CONNECT())
        {
            bresult = TRUE;
            DETECT_printData( "HDMI0 CableNotConnect:%x", hwDVI0Cable_Pin );
        }
        else
        {
            DETECT_printData( "HDMI0 CableConnect:%x", hwDVI0Cable_Pin );
        }
    }
#endif

#if (CInput_HDMI_C2 != CInput_Nothing)
    if(CURRENT_INPUT_IS_HDMI1())
    {
        if(!IS_HDMI_PORT1_CABLE_CONNECT())
        {
            bresult = TRUE;
            DETECT_printData( "HDMI1 CableNotConnect:%x", hwDVI1Cable_Pin );
        }
        else
        {
            DETECT_printData( "HDMI1 CableConnect:%x", hwDVI1Cable_Pin );
        }
    }
#endif

#if (CInput_HDMI_C3 != CInput_Nothing)
    if(CURRENT_INPUT_IS_HDMI2())
    {
        if(!IS_HDMI_PORT2_CABLE_CONNECT())
        {
            bresult = TRUE;
            DETECT_printData( "HDMI2 CableNotConnect:%x", hwDVI2Cable_Pin );
        }
        else
        {
            DETECT_printData( "HDMI2 CableConnect:%x", hwDVI2Cable_Pin );
        }
    }
#endif


    return bresult;
}
#endif

#if ENABLE_DP_INPUT
Bool DPCableNotConnect(void)
{
    bit bresult = FALSE; // false means DP cable connect

#if (CInput_Displayport_C1 != CInput_Nothing)
    if(CURRENT_INPUT_IS_DISPLAYPORT0())
    {
        if(!mapi_DPRx_GetCableDetectPort(CInput_Displayport_C1))
        {
            bresult = TRUE;
            DETECT_printData( "DP0 CableNotConnect:%x", mapi_DPRx_GetCableDetectPort(CInput_Displayport_C1) );
        }
        else
        {
            DETECT_printData( "DP0 CableConnect:%x", mapi_DPRx_GetCableDetectPort(CInput_Displayport_C1) );
        }
    }
#endif

#if (CInput_Displayport_C2 != CInput_Nothing)
    if(CURRENT_INPUT_IS_DISPLAYPORT1())
    {
        if(!mapi_DPRx_GetCableDetectPort(CInput_Displayport_C2))
        {
            bresult = TRUE;
            DETECT_printData( "DP1 CableNotConnect:%x", mapi_DPRx_GetCableDetectPort(CInput_Displayport_C2) );
        }
        else
        {
            DETECT_printData( "DP1 CableConnect:%x", mapi_DPRx_GetCableDetectPort(CInput_Displayport_C2) );
        }
    }
#endif

#if (CInput_Displayport_C3 != CInput_Nothing)
    if(CURRENT_INPUT_IS_DISPLAYPORT2())
    {
        if(!mapi_DPRx_GetCableDetectPort(CInput_Displayport_C3))
        {
            bresult = TRUE;
            DETECT_printData( "DP2 CableNotConnect:%x", mapi_DPRx_GetCableDetectPort(CInput_Displayport_C3) );
        }
        else
        {
            DETECT_printData( "DP2 CableConnect:%x", mapi_DPRx_GetCableDetectPort(CInput_Displayport_C3) );
        }
    }
#endif

#if (CInput_Displayport_C4 != CInput_Nothing)
    if(CURRENT_INPUT_IS_DISPLAYPORT3())
    {
        if(!mapi_DPRx_GetCableDetectPort(CInput_Displayport_C4))
        {
            bresult = TRUE;
            DETECT_printData( "DP3 CableNotConnect:%x", mapi_DPRx_GetCableDetectPort(CInput_Displayport_C4) );
        }
        else
        {
            DETECT_printData( "DP3 CableConnect:%x", mapi_DPRx_GetCableDetectPort(CInput_Displayport_C4) );
        }
    }
#endif

    return bresult;
}
#endif

#if ENABLE_USB_TYPEC
Bool DPCCableNotConnect(void)
{
    bit bresult = FALSE; // false means DPC cable connect

    if(CURRENT_INPUT_IS_USBTYPEC2() || CURRENT_INPUT_IS_USBTYPEC3())
    {
        if(!mapi_CC_GetAttachStatus(1)) 
        {
            bresult = TRUE;
            DETECT_printData( "DPC CableNotConnect:%x", mapi_CC_GetAttachStatus(1));
        }
        else
        {
            DETECT_printData( "DPC CableConnect:%x",mapi_CC_GetAttachStatus(1));
        }
    }

    return bresult;
}
#endif

void SpeedupOrExtendSwitchPort(void)
{
#if ENABLE_HDMI
    if(HDMI_SPEEDUP_SWITCH_PORT())
    {
        DETECT_printMsg("HDMI speed up switch port \r\n");
        InputTimingStableCounter = SyncLossStableCount;
    }
#endif

#if ENABLE_DP_INPUT
    if(DP_SPEEDUP_SWITCH_PORT())
    {
        DETECT_printMsg("DP speed up switch port \r\n");
        InputTimingStableCounter = SyncLossStableCount;
    }
#endif

#if ENABLE_USB_TYPEC
    if(DPC_SPEEDUP_SWITCH_PORT())
    {
        DETECT_printMsg("DPC speed up switch port \r\n");
        InputTimingStableCounter = SyncLossStableCount;
    }
#endif

}
#endif
WORD GetImageWidth(void)
{
    WORD u16ImageWidth=0;
#if ENABLE_DP_INPUT
    WORD unused;
#endif
    if( CURRENT_INPUT_IS_VGA() )
    {
        u16ImageWidth=StandardModeWidth;
    }
#if ENABLE_DP_INPUT
    else if( CURRENT_INPUT_IS_DISPLAYPORT() )
    {
        mapi_DPRx_GetHVDEInformation(  SrcInputType,   &u16ImageWidth,  &unused );

    }
#endif
#if  ENABLE_CHECK_TMDS_DE
    else if( CURRENT_INPUT_IS_TMDS() )
    {
        u16ImageWidth=SrcTMDSWidth;
    }
#endif
    else
    {
        u16ImageWidth=SC0_READ_AUTO_WIDTH();
    }
    return u16ImageWidth;
}
WORD GetImageHeight(void)
{
    WORD u16ImageHeight=0;
#if ENABLE_DP_INPUT
        WORD unused;
#endif


    if( CURRENT_INPUT_IS_VGA() )
    {
        u16ImageHeight=StandardModeHeight;
    }
#if ENABLE_DP_INPUT
    else if( CURRENT_INPUT_IS_DISPLAYPORT() )
    {
        mapi_DPRx_GetHVDEInformation( SrcInputType,   &unused,  &u16ImageHeight );
    }
#endif
#if  ENABLE_CHECK_TMDS_DE
    else if( CURRENT_INPUT_IS_TMDS() )
    {
        u16ImageHeight=SrcTMDSHeight;
    }
#endif
    else
    {
        u16ImageHeight=SC0_READ_AUTO_HEIGHT();
    }
    return u16ImageHeight;
}

Bool msCheckFakeSleep(void)
{
    Bool bResult = FALSE;

//Fake Sleep Condition comes from custom cb function
    if(appmStar_CbGetInitFlag() && appmStar_CB_FuncList[eCB_appmStar_CustomizeFakeSleepCondition]!=NULL)
    {
        bResult = ((fbappmstarCustomizeFakeSleepConditionCb)appmStar_CB_FuncList[eCB_appmStar_CustomizeFakeSleepCondition])();
    }

    return bResult;
}

BYTE code DetectNullData[] = {0};
void DetectDummy(void)
{
    BYTE xdata i = DetectNullData[0];
    i = i;
}

#if ENABLE_YUV420
void mStar_Monitor420Status( BYTE InputPort )
{
    EN_COLOR_FORMAT_TYPE eInputColorType = msAPI_combo_IPGetColorFormat(InputPort).ucColorType;
    BOOL bInput420 = (eInputColorType == COMBO_COLOR_FORMAT_YUV_420)?TRUE:FALSE;

    if(Input420Flag != bInput420)
    {
        DETECT_printData( "Input420Flag changed : %d", bInput420);
        Input420Flag = bInput420;
        Set_InputTimingChangeFlag();
    }
}
#endif

#if ENABLE_HDMI_ALLM
BOOL mStar_CheckLowLatecyMode(void)
{
    BOOL bLowLatecy = FALSE;

    if((FRAME_BFFLESS == mStar_FblOv())||IS_HDMI_FREESYNC()||IS_DP_FREESYNC())
        bLowLatecy = TRUE;

    return bLowLatecy;
}

void mStar_MonitorALLMStatus( BYTE InputPort )
{
    Bool bALLMTemp = msAPI_combo_HDMIRx_GetVideoContentInfo(COMBO_VIDEO_ALLM_FLAG, InputPort);

    if((FreeRunModeFlag || g_bDisplayOK) && (InputALLMFlag != bALLMTemp))
    {
        DETECT_PRINT( "ALLMTemp changed : %d, %d\n",bALLMTemp, InputALLMFlag );
        InputALLMFlag = bALLMTemp;
        if(g_bDisplayOK && (!mStar_CheckLowLatecyMode()))
        {
            ClrDisplayOKFlag();
            Set_DoModeSettingFlag();
#if ENABLE_DAC
            audio_EnableAudioAfterSignalLock_Flag=0;
            msAPI_AudioSetDpgaMute(TRUE);
#if ENABLE_SPDIF
            msAPI_AudioSpdifMute(TRUE);
#endif
#if (TTS && (CHIP_ID == CHIP_MT9701))
            msAPI_AudioTTSMixEnable(FALSE);
#endif
#endif
            if( !DisplayLogoFlag )
            {
                hw_ClrBlacklit();
            }
        }
    }

}
#endif

/// @brief Set Exit eBOOT_STATUS_CHECKAGAINON state cnt, the unit is same as SyncValidCount/SyncLossCount
/// @param u16CheckAgainOnThd 
void msAPI_CheckAgainOnThd_Set(WORD u16CheckAgainOnThd)
{
    g_u16CheckAgainOnThd = u16CheckAgainOnThd;
}

/// @brief Get CheckAgainOn Threshold
/// @param  
/// @return g_u16CheckAgainOnThd
WORD msAPI_CheckAgainOnThd_Get(void)
{
    return g_u16CheckAgainOnThd;
}

/// @brief Set Fake Sleep Time Out Cnt, -1 is init value
/// @param u16TimeOutCnt 
void msAPI_FakeSleepTimeOutCnt_Set(MS_S16 s16TimeOutCnt)
{
    g_s16FakeSleepTimeOutCnt = s16TimeOutCnt;
}

/// @brief Get Fake Sleep Time Out Cnt
/// @param  
/// @return g_s16FakeSleepTimeOutCnt
MS_S16 msAPI_FakeSleepTimeOutCnt_Get(void)
{
    return g_s16FakeSleepTimeOutCnt;
}

/// @brief Set Fake Sleep Time Out Threshold
/// @param u16TimeOutThd 
void msAPI_FakeSleepTimeOutThd_Set(WORD u16TimeOutThd)
{
    g_u16FakeSleepTimeOutThd = u16TimeOutThd;
}

/// @brief Set Fake Sleep Time Out Threshold
/// @param  
/// @return g_u16FakeSleepTimeOutThd
WORD msAPI_FakeSleepTimeOutThd_Get(void)
{
    return g_u16FakeSleepTimeOutThd;
}

/// @brief Set Fake Sleep Time Out Enable
/// @param bEn, TRUE: Enable Fake SLeep Time Out, FALSE: Disable Fake Sleep Time Out 
void msAPI_FakeSleepTimeOutEn_Set(BOOL bEn)
{
    g_bFakeSleepTimeOutEn = bEn;
}

/// @brief Set Fake Sleep Time Out Enable
/// @param  
/// @return g_bFakeSleepTimeOutEn
BOOL msAPI_FakeSleepTimeOutEn_Get(void)
{
    return g_bFakeSleepTimeOutEn;
}
