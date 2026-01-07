#include "types.h"
#include "board.h"
#include "Global.h"
#include "ms_reg.h"
//#include "panel.h"
#include "misc.h"
#include "Ms_rwreg.h"
#include "mStar.h"
#include "Debug.h"
#include "Common.h"
#include "msOSD.h"
#include "Adjust.h"
//#include "msADC.h"
//#include "drvadc.h"
#include "drvmsOVD.h"
#include "GPIO_DEF.h"
#include "Power.h"
#include "Detect.h"
#include "misc.h"
#include "asmCPU.h"
#if (MS_PM)
#include "Ms_PM.h"
#endif
#include "mapi_eDPTx.h"
#include "system_eDPTx.h"

////////////////////////////////////////////////////
//#include "drvPower.h"
//#include "drvmStar.h"
///////////////////////////////////////////////////
#if ENABLE_DPS
#include "msDPS_Setting.h"
#endif

#define POWER_DEBUG    0
#if ENABLE_MSTV_UART_DEBUG && POWER_DEBUG
    #define POWER_printData(str, value)   printData(str, value)
    #define POWER_printMsg(str)           printMsg(str)
#else
    #define POWER_printData(str, value)
    #define POWER_printMsg(str)
#endif
#define TurnOffOSD      1
void Power_TurnOffLed( void );
#if Enable_PanelHandler
    #define PANEL_HANDLER_TIMEOUT   10000 // 10 sec
#else
void Power_TurnOffPanel( void );
#endif
void Power_PowerOffSystem( void );

void Power_TurnOnAmberLed( void );
extern Bool PowerOffSystem( void );

//=============================================
// Control system power status
// caller : main()
//---------------------------------------------
void Power_PowerHandler( void )
{
    if( eDPTXAutoTest )
        return ;

    if( ForcePowerSavingFlag )
    {
        Clr_ForcePowerSavingFlag(); // clear force power saving flag

        if(msCheckFakeSleep())
        {
            Set_FakeSleepFlag();
            POWER_printMsg("ENTER FAKE SLEEP\n");
            FakeSleepCounter = 2;
#if Enable_PanelHandler
            Power_PanelCtrlOnOff(FALSE, TRUE);
#else
            Power_TurnOffPanel();
#endif
            Clr_InputTimingChangeFlag();
            OsdCounter = 0;
            PowerDownCounter = 0;
            Clr_OsdTimeoutFlag();
            g_CountSwitchPortTimeFlag = FALSE;
        }
        else
        {
            Clr_FakeSleepFlag();
 			POWER_printMsg("POWER, EXIT FAKE SLEEP\n");

            if(msAPI_FakeSleepTimeOutEn_Get())
            {
                //Reset Fake Sleep Time Out cnt to Default
                msAPI_FakeSleepTimeOutCnt_Set(-1);
                msAPI_FakeSleepTimeOutEn_Set(FALSE);
            }

            if( !PowerSavingFlag ) //not DPMS
            {
                if( PowerOnFlag )
                {
                    Power_PowerOffSystem();
                    #if (MS_PM)
                    msPM_SetFlag_Standby();
                    #endif
                    Power_TurnOnAmberLed();
                }
                else //for dc off -> ac off -> ac on then will enter DC OFF mode
                {
                    PowerOffSystem();
                }
            }
        }
    }
}

extern void _mdrv_DPRx_LongHPD(DPRx_ID dprx_id);
void Power_PowerOnSystem( void )
{
#if (ENABLE_DP_INPUT == 0x1)
    DPRx_ID dprx_id = DPRx_ID_MAX;
#endif
    BootTimeStamp_Set(POWER_ON_TS, 2, TRUE); // power on timestamp 2
    Set_ShowInputInfoFlag();
    gBoolVisualKey = 0;
    gByteVisualKey = 0xff;
    
    //Disable turn on panel when MTK internal test case
    //The panel on function is controled by other sw flow
    #if Enable_PanelHandler
    //Wake up and stop at turn on backlight
    Power_PanelCtrlStateStopFlag_Set(ePANEL_STATE_3);
    Power_PanelCtrlOnOff(TRUE, FALSE);
    #else
        Power_TurnOnPanel();
    #endif
    BootTimeStamp_Set(POWER_ON_TS, 3, TRUE); // power on timestamp 3
    mStar_PowerUp();
    BootTimeStamp_Set(POWER_ON_TS, 4, TRUE); // power on timestamp 4
    Clr_PowerSavingFlag();
    Set_InputTimingChangeFlag();
    //Clr_ForcePowerDownFlag();
    SrcFlags |= SyncLoss;
    SwitchPortCntr = 0;
#if (ENABLE_DP_INPUT == 0x1)
    for(dprx_id = DPRx_ID_0; dprx_id < DPRx_ID_MAX; dprx_id++)
	{
		_mdrv_DPRx_LongHPD(dprx_id);
	}
#endif
    mStar_SetupInputPort();
    BootTimeStamp_Set(POWER_ON_TS, 5, TRUE); // power on timestamp 5
}

void Power_PowerOffSystem( void )
{
#if Enable_PanelHandler
    Power_PanelCtrlOnOff(FALSE, TRUE);
#else
    Power_TurnOffPanel();
#endif
    mStar_PowerDown();
    Set_PowerSavingFlag();
    Clr_InputTimingChangeFlag();
    OsdCounter = 0;
    PowerDownCounter = 0;
    Clr_OsdTimeoutFlag();
}

void Power_TurnOffLed( void )
{
#if ENABLE_TOUCH_KEY
    #if TOUCH_KEY_CTRL_LED
    //TouchKeySetLEDStatus = TouchKeyLEDStatus_OFF;
    TouchKey_CtrlLED(TouchKeyLEDStatus_OFF);
    #endif
#else
    hw_ClrGreenLed();
    hw_ClrAmberLed();
#endif
}

void Power_TurnOnGreenLed( void )
{
#if ENABLE_TOUCH_KEY
    #if TOUCH_KEY_CTRL_LED
    //TouchKeySetLEDStatus = TouchKeyLEDStatus_GREEN_ON;
    TouchKey_CtrlLED(TouchKeyLEDStatus_GREEN_ON);
    #endif
#else
    hw_SetGreenLed();
    hw_ClrAmberLed();
#endif
}

void Power_TurnOnAmberLed( void )
{
#if ENABLE_TOUCH_KEY
    #if TOUCH_KEY_CTRL_LED
    //TouchKeySetLEDStatus = TouchKeyLEDStatus_AMBER_ON;
    TouchKey_CtrlLED(TouchKeyLEDStatus_AMBER_ON);
    #endif
#else
    hw_ClrGreenLed();
    hw_SetAmberLed();
#endif
}

#if Enable_PanelHandler
PANEL_POWER_TIMING_INFO Power_PanelCtrlCounter_Get(void)
{
    return g_sPanelPowerTimingInfo;
}

void Power_PanelCtrlCounter_Set(PANEL_POWER_TIMING_INFO sPanelPowerTimingInfo)
{
    g_sPanelPowerTimingInfo = sPanelPowerTimingInfo;
}

ePANEL_STATE Power_PanelCtrlState_Get(void)
{
    return g_ePanelState;
}

void Power_PanelCtrlState_ForceJump(ePANEL_STATE eState, MS_S32 CustomCounter)
{
    ePANEL_STATE eCurrentState = Power_PanelCtrlState_Get();

    /* panel on force jump */
    if(g_bCurrentPanelCtrlOn)
    {
        // force jump state warning message
        if(eState >= eCurrentState)
        {
            POWER_printMsg("panel on force jump state must smaller than previous state");
            return;
        }

        // set force jump state stop flag
        Power_PanelCtrlStateStopFlag_Set(eState-1);

        // set force jump reverse direction
        g_bCurrentPanelCtrlOn = FALSE;

        // start force jump state
        while(Power_PanelCtrlState_Get() != eState)
            Power_PanelCtrlHandler();

        // wait for custom counter and resume to previous state
        g_s32PanelCustomCounter = CustomCounter;
        POWER_printMsg("CustomCounter Set");
        Power_PanelCtrlOnOff(TRUE, FALSE);

        // clr force jump state stop flag
        Power_PanelCtrlStateStopFlag_Clr(eState-1);
    }

    /* panel off force jump */
    else
    {
        // force jump state warning message
        if(eState <= eCurrentState)
        {
            POWER_printMsg("panel off force jump state must larger than previous state");
            return;
        }

        // set force jump state stop flag
        Power_PanelCtrlStateStopFlag_Set(eState+1);

        // set force jump reverse direction
        g_bCurrentPanelCtrlOn = TRUE;

        // start force jump state
        while(Power_PanelCtrlState_Get() != eState)
            Power_PanelCtrlHandler();

        // wait for custom counter and resume to previous state
        g_s32PanelCustomCounter = CustomCounter;
        POWER_printMsg("CustomCounter Set");
        Power_PanelCtrlOnOff(TRUE, FALSE);

        // clr force jump state stop flag
        Power_PanelCtrlStateStopFlag_Clr(eState+1);
    }
}

BOOL Power_PanelCtrlStateStopFlag_Get(ePANEL_STATE eState)
{
    if((eState>ePANEL_STATE_0) && (eState<(ePANEL_STATE_MAX-1)))
        return g_bPanelStateStopFlag[eState];
    return FALSE;
}

void Power_PanelCtrlStateStopFlag_Set(ePANEL_STATE eState)
{
    if((eState>ePANEL_STATE_0) && (eState<(ePANEL_STATE_MAX-1)))
        g_bPanelStateStopFlag[eState] = TRUE;
}

void Power_PanelCtrlStateStopFlag_Clr(ePANEL_STATE eState)
{
    if((eState>ePANEL_STATE_0) && (eState<(ePANEL_STATE_MAX-1)))
        g_bPanelStateStopFlag[eState] = FALSE;
}

void Power_PanelCtrlStateMachineChange(BOOL ISRTrigger)
{
#if !(ENABLE_MSTV_UART_DEBUG && POWER_DEBUG)
    UNUSED(ISRTrigger);
#endif

    /* state machine stop by custom counter */
    if(g_s32PanelCustomCounter)
        return;

    /* check state machine change */
    // direction change
    if(g_bPreviousPanelCtrlOn != g_bCurrentPanelCtrlOn)
    {
        g_bPreviousPanelCtrlOn = g_bCurrentPanelCtrlOn;
    }
    // direction not change
    else
    {
        if(g_bCurrentPanelCtrlOn && (g_ePanelState!=ePANEL_STATE_MAX-1))
        {
            // avoid early turn on panel w/ garbage
            if((g_ePanelState==ePANEL_STATE_2) && !(DisplayLogoFlag || g_bDisplayOK))
                return;
            if(Power_PanelCtrlStateStopFlag_Get(g_ePanelState+1))
                return;
            g_ePanelState++;
        }
        if(!g_bCurrentPanelCtrlOn && (g_ePanelState!=ePANEL_STATE_0))
        {
            if(Power_PanelCtrlStateStopFlag_Get(g_ePanelState-1))
                return;
            g_ePanelState--;
        }
    }

    /* execute state machine event */
    switch(g_ePanelState)
    {
        // panel not valid
        case ePANEL_STATE_0:
            // turn on panel start flow
            if(g_bCurrentPanelCtrlOn)
            {
                // rst maximum, minimum, counter
                g_s32PanelMinCounter = 0;
                g_s32PanelMaxCounter = -1;
                g_s32PanelCounter = 0;
                POWER_printData("[Panel %d] ePANEL_STATE_0 : turn on panel start", ISRTrigger);
            }
            break;

        // turn on/off panel
        case ePANEL_STATE_1:
            // turn on panel power flow
            if(g_bCurrentPanelCtrlOn)
            {
                mStar_BlackWhiteScreenCtrl(BW_SCREEN_BLACK);
                hw_SetPanel();

                // rst maximum, minimum, counter
                g_s32PanelMinCounter = g_sPanelPowerTimingInfo.state1_2_Min;
                g_s32PanelMaxCounter = g_sPanelPowerTimingInfo.state1_2_Max;
                g_s32PanelCounter = 0;
                POWER_printData("[Panel %d] ePANEL_STATE_1 : turn on panel power", ISRTrigger);
                break;
            }
            // turn off panel power flow
            if(!g_bCurrentPanelCtrlOn)
            {
                hw_ClrPanel();
                bPanelOnFlag = 0;

                // rst maximum, minimum, counter
                g_s32PanelMinCounter = g_sPanelPowerTimingInfo.paneloffon_Min;
                g_s32PanelMaxCounter = -1;
                g_s32PanelCounter = 0;
                POWER_printData("[Panel %d] ePANEL_STATE_1 : turn off panel power", ISRTrigger);
            }
            break;

        // turn on/off MOD / training / data ready
        case ePANEL_STATE_2:
            // turn on MOD -> training -> data ready flow
            if(g_bCurrentPanelCtrlOn)
            {
#if (CHIP_ID == CHIP_MT9700)
                if( g_sPnlInfo.ePnlTypeLvds != EN_PNL_LVDS_NONE && !iGenTuningFinished)
                {
                    msDrvMODBiasConCal();
                }
#endif
                msDrvPowerModCtrl(_ENABLE);
                if( g_sPnlInfo.ePnlTypeVby1 != EN_PNL_VBY1_NONE )
                {
                    msDrvCheckVBY1LockN();
                }
                if( g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE )
                {
                    System_eDPTx_PowerOnCheck();
                }
                // rst maximum, minimum, counter
                g_s32PanelMinCounter = g_sPanelPowerTimingInfo.state2_3_Min;
                g_s32PanelMaxCounter = g_sPanelPowerTimingInfo.state2_3_Max;
                g_s32PanelCounter = 0;
                POWER_printData("[Panel %d] ePANEL_STATE_2 : turn on MOD -> training -> data ready", ISRTrigger);
                break;
            }
            // turn off MOD flow
            if(!g_bCurrentPanelCtrlOn)
            {
                mStar_WaitForDataBlanking();
#ifndef _PANEL_NORMALLY_BLACK_
                mStar_BlackWhiteScreenCtrl(BW_SCREEN_WHITE);
#else
                mStar_BlackWhiteScreenCtrl(BW_SCREEN_BLACK);
#endif
                MAsm_CPU_DelayMs(80);
                mStar_BlackWhiteScreenCtrl(BW_SCREEN_OFF);
                msDrvPowerModCtrl(_DISABLE);

                // rst maximum, minimum, counter
                g_s32PanelMinCounter = g_sPanelPowerTimingInfo.state2_1_Min;
                g_s32PanelMaxCounter = g_sPanelPowerTimingInfo.state2_1_Max;
                g_s32PanelCounter = 0;
                POWER_printData("[Panel %d] ePANEL_STATE_2 : turn off MOD", ISRTrigger);
            }
            break;

        // turn on/off backlight
        case ePANEL_STATE_3:
            // turn on backlight flow
            if(g_bCurrentPanelCtrlOn)
            {
                mStar_WaitForDataBlanking();
                mStar_BlackWhiteScreenCtrl(BW_SCREEN_OFF);
                hw_SetBlacklit();
                bPanelOnFlag = 1;

                // rst maximum, minimum, counter
                g_s32PanelMinCounter = 0;
                g_s32PanelMaxCounter = -1;
                g_s32PanelCounter = 0;
                POWER_printData("[Panel %d] ePANEL_STATE_3 : turn on backlight", ISRTrigger);
                break;
            }
            // turn off backlight flow
            if(!g_bCurrentPanelCtrlOn)
            {
#if TurnOffOSD
                Osd_Hide();
                Delay4us();
#endif
                hw_ClrBlacklit();

                // rst maximum, minimum, counter
                g_s32PanelMinCounter = g_sPanelPowerTimingInfo.state3_2_Min;
                g_s32PanelMaxCounter = g_sPanelPowerTimingInfo.state3_2_Max;
                g_s32PanelCounter = 0;
                POWER_printData("[Panel %d] ePANEL_STATE_3 : turn off backlight", ISRTrigger);
            }
            break;

        // panel success turn on
        case ePANEL_STATE_4:
            // turn off panel start flow
            if(!g_bCurrentPanelCtrlOn)
            {
                // rst maximum, minimum, counter
                g_s32PanelMinCounter = 0;
                g_s32PanelMaxCounter = -1;
                g_s32PanelCounter = 0;
                POWER_printData("[Panel %d] ePANEL_STATE_4 : turn off panel start", ISRTrigger);
            }
            break;

        default:
           break;
    }
}

// handler control
void Power_PanelCtrlHandler(void)
{
    if(PanelHandlerFlag)
    {
        g_bPanelISREn = FALSE;  // turn off Power_PanelCtrlIsr
        Power_PanelCtrlStateMachineChange(FALSE);
        g_bPanelISREn = TRUE;   // resume Power_PanelCtrlIsr
    }
}

// ISR control
void Power_PanelCtrlIsr(void)
{
    if(g_bPanelISREn && (g_s32PanelMaxCounter!=-1) && PanelISRFlag)
        Power_PanelCtrlStateMachineChange(TRUE);
}

BOOL Power_PanelCtrlOnOff(BOOL bOn, BOOL bForce)
{
    BOOL bResult = TRUE;
    ePANEL_STATE state;

    // set panel on/off direction
    g_bCurrentPanelCtrlOn = bOn;

    // fast on/off flow will execute ISR immediately
    if(g_bPreviousPanelCtrlOn != bOn)
    {
        if(bOn && (g_ePanelState==ePANEL_STATE_1))
        {
            POWER_printMsg("[Panel] fast off/on panel power : wait for timing limitation");
            bResult = FALSE;
        }
        else if(bOn && (g_ePanelState==ePANEL_STATE_2))
        {
            POWER_printMsg("[Panel] fast off/on MOD / training : may cause data fail!");
            bResult = FALSE;
        }
        else
        {
            g_s32PanelMaxCounter = 0;
        }
    }

    // force delay panel on/off
    if(bForce)
    {
        // clr all stop flag avoid daed lock
        for(state=0; state<ePANEL_STATE_MAX; state++)
            Power_PanelCtrlStateStopFlag_Clr(state);

        SetTimOutConter(PANEL_HANDLER_TIMEOUT);
        while(bTimeOutCounterFlag)
        {
            // force panel on done
            if( g_bCurrentPanelCtrlOn && (g_ePanelState>=ePANEL_STATE_MAX-1))
                break;
            // force panel off done
            if(!g_bCurrentPanelCtrlOn && (g_ePanelState<=ePANEL_STATE_1))
                break;
            // force panel on check timing change
            if( g_bCurrentPanelCtrlOn && (g_ePanelState==ePANEL_STATE_2) && (!DisplayLogoFlag && InputTimingChangeFlag))
            {
                POWER_printMsg("detect timing change when turn on panel!");
                g_bCurrentPanelCtrlOn = FALSE;
            }

            Power_PanelCtrlHandler();
        }

        if(!bTimeOutCounterFlag)
        {
            POWER_printMsg("[Warning] Panel handler timeout!!!");
        }
    }
    return bResult;
}
#else
void Power_TurnOnPanel( void )
{
    if(bPanelOnFlag  && (!InputTimingChangeFlag || DisplayLogoFlag))
    {
        if(!BackLightActiveFlag)
        {
            hw_SetBlacklit();
        }
        return;
    }

    while( bPanelOffOnFlag )
    {
        if(InputTimingChangeFlag && !DisplayLogoFlag)
            return;
    }

#if Enable_ReducePanelPowerOnTime
    if(g_ePanelStatus == ePANEL_STATUS_DataRdy)
    {
        g_ePanelStatus = ePANEL_STATUS_TurnOnBL;

        if(g_u16PanelPowerCounter)
        {
            ForceDelay1ms(g_u16PanelPowerCounter);
            g_u16PanelPowerCounter = 0;
        }
    }
    else
#endif
    {
        mStar_WaitForDataBlanking();

        mStar_BlackWhiteScreenCtrl(BW_SCREEN_BLACK);

        hw_SetPanel();

        ForceDelay1ms( PanelOnTiming1 );

#if PANEL_VCOM_ADJUST
        HW_SET_VCOM_I2C_SDA();
        HW_SET_VCOM_I2C_SCL();
#endif

#if (CHIP_ID == CHIP_MT9700)
        if( g_sPnlInfo.ePnlTypeLvds != EN_PNL_LVDS_NONE && !iGenTuningFinished)
        {
            msDrvMODBiasConCal();
        }
#endif
        msDrvPowerModCtrl(_ENABLE);
        if( g_sPnlInfo.ePnlTypeVby1 != EN_PNL_VBY1_NONE )
        {
            msDrvCheckVBY1LockN();
        }
        if( g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE )
        {
            System_eDPTx_PowerOnCheck();
        }

#if Enable_ReducePanelPowerOnTime
        if(g_ePanelStatus == ePANEL_STATUS_TurnOnData)
        {
            g_u16PanelPowerCounter = PanelOnTiming2;
            g_ePanelStatus = ePANEL_STATUS_DataRdy;
            return;
        }
#else
        ForceDelay1ms( PanelOnTiming2 );
#endif
    }

    bPanelOnFlag = 1;
    mStar_WaitForDataBlanking();
    mStar_BlackWhiteScreenCtrl(BW_SCREEN_OFF);

    if( InputTimingChangeFlag && !DisplayLogoFlag )
    {// for mode change
        Power_TurnOffPanel();
    }
    else
    {
        hw_SetBlacklit();
    }

    POWER_printData("  Power_TurnOnPanel", 1);
}

void Power_TurnOffPanel( void )
{
    if( !bPanelOnFlag )
        return;


#if TurnOffOSD
    //Osd_DoubleBuffer(FALSE);
    Osd_Hide();
    //mStar_ScalerDoubleBuffer(FALSE);
    Delay4us();
#endif

    hw_ClrBlacklit();

    ForceDelay1ms( PanelOffTiming1 );

    mStar_WaitForDataBlanking();
#ifndef _PANEL_NORMALLY_BLACK_
    mStar_BlackWhiteScreenCtrl(BW_SCREEN_WHITE);
#else
    mStar_BlackWhiteScreenCtrl(BW_SCREEN_BLACK);
#endif
    ForceDelay1ms( 80 );
    mStar_BlackWhiteScreenCtrl(BW_SCREEN_OFF);

    msDrvPowerModCtrl(_DISABLE);

#if PANEL_VCOM_ADJUST
    HW_CLR_VCOM_I2C_SCL();
    HW_CLR_VCOM_I2C_SDA();
#endif

    ForceDelay1ms( PanelOffTiming2 );

    hw_ClrPanel();

#if TurnOffOSD
    //Osd_DoubleBuffer(TRUE);
#endif
    SetPanelOffOnCounter(1200);
    //u16PanelOffOnCounter = 1200;
    //bPanelOffOnFlag = 1;

    bPanelOnFlag = 0;

    POWER_printData("Power_TurnOffPanel", 1);
}

#if Enable_ReducePanelPowerOnTime
void Power_ForcePowerOnPanel( void )
{
    if(bPanelOnFlag  && (!InputTimingChangeFlag || DisplayLogoFlag))
    {
        if(!BackLightActiveFlag)
        {
            hw_SetBlacklit();
        }
        return;
    }

    while( bPanelOffOnFlag )
    {
        if(InputTimingChangeFlag && !DisplayLogoFlag)
        {
            return;
        }
    }

    if(g_ePanelStatus == ePANEL_STATUS_DataRdy)
    {
        g_ePanelStatus = ePANEL_STATUS_TurnOnBL;

        if(g_u16PanelPowerCounter)
        {
            ForceDelay1ms(g_u16PanelPowerCounter);
            g_u16PanelPowerCounter = 0;
        }
    }
    else
    {
        mStar_WaitForDataBlanking();

        mStar_BlackWhiteScreenCtrl(BW_SCREEN_BLACK);

        hw_SetPanel();

        ForceDelay1ms( PanelOnTiming1 );

#if PANEL_VCOM_ADJUST
        HW_SET_VCOM_I2C_SDA();
        HW_SET_VCOM_I2C_SCL();
#endif

#if (CHIP_ID == CHIP_MT9700)
        if( g_sPnlInfo.ePnlTypeLvds != EN_PNL_LVDS_NONE && !iGenTuningFinished)
        {
            msDrvMODBiasConCal();
        }
#endif
        msDrvPowerModCtrl(_ENABLE);
        if( g_sPnlInfo.ePnlTypeVby1 != EN_PNL_VBY1_NONE )
        {
            msDrvCheckVBY1LockN();
        }
        if( g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE )
        {
            System_eDPTx_PowerOnCheck();
        }
        ForceDelay1ms( PanelOnTiming2 );

        g_ePanelStatus = ePANEL_STATUS_TurnOnBL;
    }

    bPanelOnFlag = 1;
    mStar_WaitForDataBlanking();
    mStar_BlackWhiteScreenCtrl(BW_SCREEN_OFF);

    if( InputTimingChangeFlag && !DisplayLogoFlag )
    {// for mode change
        Power_TurnOffPanel();
    }
    else
    {
        hw_SetBlacklit();
    }

    POWER_printData("  Power_ForceTurnOnPanel", 1);
}
#endif
#endif

