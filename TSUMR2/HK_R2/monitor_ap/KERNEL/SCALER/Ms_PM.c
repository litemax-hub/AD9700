///////////////////////////////////////////////////////////////////////////////////////////////////
/// file    Ms_PM.c
/// @author MStar Semiconductor Inc.
/// @brief  PM Function
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#define _MS_PM_C_
#include "ms_reg.h"
#include "Ms_rwreg.h"
#include "Power.h"
#include "DDC2Bi.H"
#include "Ms_PM.h"
#include "menufunc.h"
#include "Mcu.h"
#include "mStar.h"
#include "Detect.h"
#include "Keypad.h"     // Provides: Key_GetKeypadStatus()
#include "Keypaddef.h"  // Provides: KeypadMask
#include "SysInit.h"
#include "uartdebug.h"
#include "drvMcu.h"

#if  ENABLE_DP_INPUT
#include "mapi_DPRx.h"
#include "ComboApp.h"
#endif

#if (ENABLE_HDMI || ENABLE_DVI)
#include "mdrv_hdmiRx.h"
#endif
#include "usbsw.h"
#if ENABLE_USB_TYPEC
#include "mailbox.h"
#endif
#if ENABLE_UART_PIU
#include "drvUARTPIU.h"
#endif
#if ENABLE_MSBHK
#include "msAPI_MSBHK.h"
#endif

#include "mapi_eDPTx.h"
#include "system_eDPTx.h"

#if ENABLE_SUPER_RESOLUTION
extern BYTE xdata SRmodeContent;
#endif
extern BYTE xdata GammaContent;

extern BOOL xdata glbIPRxInitHPD_DONE_FLAG;

DWORD  u32FIQ_DET;
DWORD  u32IRQ_DET;
DWORD  u32FIQ_POL;
DWORD  u32IRQ_POL;

BYTE xdata glDP_SQDebounce[DPRx_ID_MAX];
BYTE XDATA glAuxN_L2H_Event = 0x0;  //Each bit stand for different ports

extern void Main_SlowTimerHandler(void);

#if (MS_PM)
extern void msDVISetMux(InputPortType inport);
//extern void msPM_ClearStatus(Bool bResetPM);
extern BYTE Key_GetKeypadStatus(void);

#if  ENABLE_DP_INPUT
extern BYTE XDATA ucDPFunctionEnableIndex;
extern DPRx_AUX_ID _mdrv_DPRx_MSCHIP_DPRxID2AuxID(DPRx_ID dprx_id);
extern DPRx_PHY_ID _mdrv_DPRx_MSCHIP_DPRxID2PhyID(DPRx_ID dprx_id);
extern void mhal_DPRx_MuxSelect(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id);
extern BOOL mhal_DPRx_CheckSquelch(DPRx_ID dprx_id);
extern BOOL mhal_DPRx_TypeC_CheckSquelch(DPRx_ID dprx_id, BYTE ubPinAssign, BYTE ubCCpinInfo);

#if  0
extern void DDC2BI_DP(void);
#if ((CHIP_ID == CHIP_TSUMC)||(CHIP_ID == CHIP_TSUMD)||(CHIP_ID==CHIP_TSUMJ))
#if D2B_XShared_DDCBuffer
extern BYTE *DDCBuffer;
#else
extern BYTE xdata DDCBuffer[DDC_BUFFER_LENGTH];
#endif
#else
extern BYTE xdata DDCBuffer[DDC_BUFFER_LENGTH];
#endif
#endif

#endif

#if ENABLE_HK_CODE_ON_PSRAM
extern void msDrvGetPSramEndInPM(void);
#endif

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define PM_DEBUG    1
#if ENABLE_MSTV_UART_DEBUG && PM_DEBUG
    #define PM_printData(str, value)   printData(str, value)
    #define PM_printMsg(str)           printMsg(str)
#else
    #define PM_printData(str, value)
    #define PM_printMsg(str)
#endif

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------
XDATA sPM_Info  sPMInfo;
XDATA ePMStatus ucWakeupStatus = ePMSTS_INVAID;
XDATA sPM_SARDetect sPMSARKey;
static Bool bPM0WSts = 0;       // 0W wakeup status

//-------------------------------------------------------------------------------------------------
//  External Variables
//-------------------------------------------------------------------------------------------------
#if 0 //!ENABLE_DEBUG
extern BYTE rxIndex;
extern BYTE xdata DDCBuffer[];
#endif

#ifndef PM_SUPPORT_ADC_TIME_SHARE
#define PM_SUPPORT_ADC_TIME_SHARE  0
#endif

extern void InitFirstPowerOnVariables(void);

Bool msPM_IsState_IDLE(void)
{
    if(sPMInfo.ePMState == ePM_IDLE)
        return TRUE;

    return FALSE;
}

//**************************************************************************
//  [Function Name]
//                  msPM_PassWord(BOOL bEnable)
//  [Description]
//                  input  the password for entering pm or turn off XTAL
//**************************************************************************
void msPM_PassWord(BOOL bEnable)
{
    if(bEnable)
    {
        msWriteByte(REG_PM_87,0x55);
        msWriteByte(REG_PM_88,0xAA);
    }
    else
    {
        msWriteByte(REG_PM_87,0x00);
        msWriteByte(REG_PM_88,0x00);
    }
}

void  msPM_Init(void)
{
    sPMInfo.sPMConfig.bHVSync_enable = 0;
#if (PM_SUPPORT_WAKEUP_DVI)
    sPMInfo.sPMConfig.bDVI_enable = 0;
#endif
    sPMInfo.sPMConfig.bDP_enable = 0;
    sPMInfo.sPMConfig.bSOG_enable = 0;
    sPMInfo.sPMConfig.bSAR_enable = 0;
    sPMInfo.sPMConfig.bGPIO_enable = 0;
    sPMInfo.sPMConfig.bMCCS_enable = 0;
    sPMInfo.sPMConfig.bCEC_enable = 0;
    sPMInfo.sPMConfig.bMCUSleep = 0;
    sPMInfo.ucPMMode = ePM_POWERON;
    sPMInfo.ePMState = ePM_IDLE;
}

void msPM_SetFlag_Standby(void)
{
    PM_printMsg(" msPM_SetFlag_Standby");

#if ENABLE_VGA_INPUT
    if (UserPrefInputPriorityType==Input_Priority_Auto||(UserPrefInputPriorityType!=Input_Priority_Auto && (UserPrefInputType == Input_VGA)))
    {
        sPMInfo.sPMConfig.bHVSync_enable = 1;
    }
    else
    {
        sPMInfo.sPMConfig.bHVSync_enable = 0;
    }

    if (UserPrefInputPriorityType==Input_Priority_Auto||(UserPrefInputPriorityType!=Input_Priority_Auto && (UserPrefInputType == Input_VGA)))
    {
        sPMInfo.sPMConfig.bSOG_enable = PM_POWERSAVING_WAKEUP_SOG;
    }
    else
    {
        sPMInfo.sPMConfig.bSOG_enable = 0;
    }
#endif

#if (PM_SUPPORT_WAKEUP_DVI)
    if (UserPrefInputPriorityType==Input_Priority_Auto\
    ||(UserPrefInputPriorityType!=Input_Priority_Auto && ((UserPrefInputType == Input_DVI)||(UserPrefInputType == Input_DVI2)||(UserPrefInputType == Input_DVI3)))\
    ||(UserPrefInputPriorityType!=Input_Priority_Auto && ((UserPrefInputType == Input_HDMI)||(UserPrefInputType == Input_HDMI2)||(UserPrefInputType == Input_HDMI3))))
    {
        sPMInfo.sPMConfig.bDVI_enable =1;
    }
    else
    {
        sPMInfo.sPMConfig.bDVI_enable = 0;
    }
#endif

    if ((UserPrefInputPriorityType==Input_Priority_Auto)||
        ((UserPrefInputPriorityType!=Input_Priority_Auto) &&
        (((Input_Displayport_C1 != Input_Nothing)&&(UserPrefInputType == Input_Displayport_C1)) ||
        ((Input_Displayport_C2 != Input_Nothing)&&(UserPrefInputType == Input_Displayport_C2)) ||
        ((Input_Displayport_C3 != Input_Nothing)&&(UserPrefInputType == Input_Displayport_C3)) ||
        ((Input_Displayport_C4 != Input_Nothing)&&(UserPrefInputType == Input_Displayport_C4)) ||
        ((Input_UsbTypeC_C3 != Input_Nothing)&&(UserPrefInputType == Input_UsbTypeC_C3)) ||
        ((Input_UsbTypeC_C4 != Input_Nothing)&&(UserPrefInputType == Input_UsbTypeC_C4)))
        ))
    {
        sPMInfo.sPMConfig.bDP_enable=PM_SUPPORT_WAKEUP_DP;
    }
    else
    {
        sPMInfo.sPMConfig.bDP_enable = PM_SUPPORT_WAKEUP_DP;
    }

    sPMInfo.sPMConfig.bSAR_enable = PM_POWERSAVING_WAKEUP_SAR;
    sPMInfo.sPMConfig.bGPIO_enable = PM_POWERSAVING_WAKEUP_GPIO;
    sPMInfo.sPMConfig.bMCCS_enable = PM_POWERSAVING_WAKEUP_MCCS;
    sPMInfo.sPMConfig.bCEC_enable = 0;
    sPMInfo.sPMConfig.bMCUSleep = 0;
    if((ENABLE_HDMI_EDID_INTERNAL_DATA == 1) || (ENABLE_VGA_EDID_INTERNAL_DATA == 1))
    {
        sPMInfo.sPMConfig.bEDID_enable = 1;
    }
    else
    {
        sPMInfo.sPMConfig.bEDID_enable = 0;
    }
    sPMInfo.sPMConfig.ePMSARmode = PM_POWERSAVING_SARmode;
#if ENABLE_USB_TYPEC
    sPMInfo.sPMConfig.eTYPECmode = ePMTYPEC_AUTO;
#else
    sPMInfo.sPMConfig.eTYPECmode = ePMTYPEC_OFF;
#endif
    sPMInfo.ucPMMode = ePM_STANDBY;
    sPMInfo.ePMState = ePM_ENTER_PM;

    PM_printData("\r\n sPMInfo.ePMState DPMS %d", sPMInfo.ePMState);
}

void  msPM_SetFlag_PMDCoff(void)
{
    PM_printMsg("msPM_SetFlag_PMDCoff");

#if ENABLE_VGA_INPUT
    sPMInfo.sPMConfig.bHVSync_enable = 0;
    sPMInfo.sPMConfig.bSOG_enable = PM_POWEROFF_WAKEUP_SOG;
#endif

#if (PM_SUPPORT_WAKEUP_DVI)
    sPMInfo.sPMConfig.bDVI_enable = 0;
#endif

    sPMInfo.sPMConfig.bDP_enable = ENABLE_DP_DCOFF_HPD_HIGH;
    sPMInfo.sPMConfig.bSAR_enable = PM_POWEROFF_WAKEUP_SAR;
    sPMInfo.sPMConfig.bGPIO_enable = PM_POWEROFF_WAKEUP_GPIO;
    sPMInfo.sPMConfig.bMCCS_enable = PM_POWEROFF_WAKEUP_MCCS;
    sPMInfo.sPMConfig.bCEC_enable = 0;
    sPMInfo.sPMConfig.bMCUSleep = 0;
    if((ENABLE_HDMI_EDID_INTERNAL_DATA == 1) || (ENABLE_VGA_EDID_INTERNAL_DATA == 1))
    {
        sPMInfo.sPMConfig.bEDID_enable = 1;
    }
    else
    {
        sPMInfo.sPMConfig.bEDID_enable = 0;
    }
    sPMInfo.sPMConfig.ePMSARmode = PM_POWEROFF_SARmode;
#if ENABLE_USB_TYPEC
    if(UserPrefDCOffDischarge == DCOffDisChargerMenuItems_On)
    {
        sPMInfo.sPMConfig.eTYPECmode = ePMTYPEC_AUTO;
    }
    else
    {
        sPMInfo.sPMConfig.eTYPECmode = ePMTYPEC_OFF;
    }
#else
    sPMInfo.sPMConfig.eTYPECmode = ePMTYPEC_OFF;
#endif
    sPMInfo.ucPMMode = ePM_POWEROFF;
    sPMInfo.ePMState = ePM_ENTER_PM;



    PM_printData("\r\n sPMInfo.ePMState DCOFF %d", sPMInfo.ePMState);
}

void msPM_EnableDPDetect(BOOL bEnable)//if not enable can reduce 10mA
{
    msWriteByte(REG_DPRX_TRANS_CTRL_07_L, DP_RX_FT_TIMER_A_WAIT_SQ_SIGNAL);

    #if (PM_SUPPORT_WAKEUP_DP == 0x1)
        #if (ENABLE_DP_OUTPUT == 0x1)
            if(sPMInfo.ucPMMode== ePM_STANDBY)
            {
                mapi_DPTx_MSCHIP_SetPowerSavingMode(sPMInfo.ucPMMode, bEnable);
            }
        #endif
        mapi_DPRx_MSCHIP_EnableDPDetect(sPMInfo.ucPMMode, bEnable);
        glAuxN_L2H_Event = 0;
    #else  //PM_SUPPORT_WAKEUP_DP
        bEnable = 0x0;
    #endif //PM_SUPPORT_WAKEUP_DP
}
#if PM_SUPPORT_WAKEUP_HDMI_SQH
BYTE msPM_HDMI_SQH_Detect(void)
{
    BYTE ucDVI_ACT_Port = DVI_AVT__PORT_NONE;
    BYTE ucStatus, i;

    if(sPMInfo.ucPMMode == ePM_POWEROFF)
    {
        ucDVI_ACT_Port = DVI_AVT__PORT_NONE;
    }
    else
    {
        if(UserPrefInputPriorityType==Input_Priority_Auto)
        {
            for( i = 0 ; i < Input_Nums ; i++ )
            {
                if( !INPUT_IS_TMDS(i) )
                    continue;

                if( INPUT_IS_TMDS0(i) )
                {
                    ucStatus = (msReadByte(REG_DPRX_PHY_PM_11_L)&0x0F);
                }
                else if( INPUT_IS_TMDS1(i) )
                {
                    ucStatus = (msReadByte(REG_DPRX_PHY_PM_31_L)&0x0F);
                }


                if(ucStatus & 0x0E)
                {
                    if( INPUT_IS_TMDS0(i) )
                        ucDVI_ACT_Port |= DVI_AVT_PORT0;
                    else if( INPUT_IS_TMDS1(i) )
                        ucDVI_ACT_Port |= DVI_AVT_PORT1;
                    break;
                }
            }
        }
        else
        {
            if((UserPrefInputPriorityType!=Input_Priority_Auto) && (UserPrefInputType == Input_HDMI))
            {

                ucStatus = (msReadByte(REG_DPRX_PHY_PM_11_L)&0x0F);

                if(ucStatus & 0x0E)
                {
                    if( INPUT_IS_TMDS0(Input_HDMI) )
                        ucDVI_ACT_Port |= DVI_AVT_PORT0;
                }
            }
            else if((UserPrefInputPriorityType!=Input_Priority_Auto) && (UserPrefInputType == Input_HDMI2))
            {
                ucStatus = (msReadByte(REG_DPRX_PHY_PM_31_L)&0x0F);

                if(ucStatus & 0x0E)
                {
                    if( INPUT_IS_TMDS1(Input_HDMI2) )
                        ucDVI_ACT_Port |= DVI_AVT_PORT1;
                }
            }
        }
    }

    return ucDVI_ACT_Port;
}
#endif
#if PM_SUPPORT_WAKEUP_HDMI_SCDC5V
BYTE msPM_HDMI_SCDC5V_Detect(void)
{
    BYTE ucDVI_ACT_Port = DVI_AVT__PORT_NONE;
    BYTE i;
    BYTE ucStatus_SCDC5VP0 = 0;
    BYTE ucStatus_SCDC5VP1 = 0;

    if(sPMInfo.ucPMMode == ePM_POWEROFF)
    {
        ucDVI_ACT_Port = DVI_AVT__PORT_NONE;
    }
    else
    {
        if(UserPrefInputPriorityType==Input_Priority_Auto)
        {
            for( i = 0 ; i < Input_Nums ; i++ )
            {
                if( !INPUT_IS_TMDS(i) )
                    continue;

                if( INPUT_IS_TMDS0(i) )
                {
                    ucStatus_SCDC5VP0 = msReadByte(0x00310E);
                }
                else if( INPUT_IS_TMDS1(i) )
                {
                    ucStatus_SCDC5VP1 = msReadByte(0x00320E);
                }

                if(((ucStatus_SCDC5VP0 & BIT5)==BIT5) || ((ucStatus_SCDC5VP1 & BIT5)==BIT5))
                {
                    if( INPUT_IS_TMDS0(i) )
                        ucDVI_ACT_Port |= DVI_AVT_PORT0;
                    else if( INPUT_IS_TMDS1(i) )
                        ucDVI_ACT_Port |= DVI_AVT_PORT1;
                    break;
                }
            }
        }
        else
        {
            if((UserPrefInputPriorityType!=Input_Priority_Auto) && (UserPrefInputType == Input_HDMI))
            {
                ucStatus_SCDC5VP0 = msReadByte(0x00310E);

                if((ucStatus_SCDC5VP0 & BIT5)==BIT5)
                {
                    if( INPUT_IS_TMDS0(Input_HDMI) )
                        ucDVI_ACT_Port |= DVI_AVT_PORT0;
                }
            }
            else if((UserPrefInputPriorityType!=Input_Priority_Auto) && (UserPrefInputType == Input_HDMI2))
            {
                ucStatus_SCDC5VP1 = msReadByte(0x00320E);

                if((ucStatus_SCDC5VP1 & BIT5)==BIT5)
                {
                    if( INPUT_IS_TMDS1(Input_HDMI2) )
                        ucDVI_ACT_Port |= DVI_AVT_PORT1;
                }
            }
        }
    }

    return ucDVI_ACT_Port;
}
#endif

BYTE msPM_DVIClockDetect(void)
{
    BYTE ucDVI_ACT_Port = DVI_AVT__PORT_NONE;
    BYTE ucStatus, i, ucReCheck;

    if(sPMInfo.ucPMMode == ePM_POWEROFF)
    {
        ucDVI_ACT_Port = DVI_AVT__PORT_NONE;
    }
    else
    {
        if(UserPrefInputPriorityType==Input_Priority_Auto)
        {
            for( i = 0 ; i < Input_Nums ; i++ )
            {
                if( !INPUT_IS_TMDS(i) )
                    continue;
        #if (ENABLE_HDMI || ENABLE_DVI)
                    mdrv_hdmiRx_PMSwitchDVIDetect(i);
        #endif

                ucReCheck = 3;
                Delay1ms(1);

                ucStatus = msReadByte(REG_DVI_RAW_CLK);

                while(((ucStatus & DVI_RAW_CLK_DET) == 0) && (ucReCheck > 0))
                {
                    Delay1ms(1);
                    ucStatus = msReadByte(REG_DVI_RAW_CLK);
                    ucReCheck--;
                }

                if(ucStatus & DVI_RAW_CLK_DET)
                {
                    if( INPUT_IS_TMDS0(i) )
                        ucDVI_ACT_Port |= DVI_AVT_PORT0;
                    else if( INPUT_IS_TMDS1(i) )
                        ucDVI_ACT_Port |= DVI_AVT_PORT1;
                #if(CHIP_ID == CHIP_MT9700) //hdmi port  MT9700:3 MT9701:2
                    else if( INPUT_IS_TMDS2(i) )
                        ucDVI_ACT_Port |= DVI_AVT_PORT2;
                #endif
                    break;
                }
            }
        }
        else
        {
            if((UserPrefInputPriorityType!=Input_Priority_Auto) && (UserPrefInputType == Input_HDMI))
            {
        #if (ENABLE_HDMI || ENABLE_DVI)
                    mdrv_hdmiRx_PMSwitchDVIDetect(Input_HDMI);
        #endif

                ucReCheck = 3;
                Delay1ms(1);

                ucStatus = msReadByte(REG_DVI_RAW_CLK);

                while(((ucStatus & DVI_RAW_CLK_DET) == 0) && (ucReCheck > 0))
                {
                    Delay1ms(1);
                    ucStatus = msReadByte(REG_DVI_RAW_CLK);
                    ucReCheck--;
                }

                if(ucStatus & DVI_RAW_CLK_DET)
                {
                    if( INPUT_IS_TMDS0(Input_HDMI) )
                        ucDVI_ACT_Port |= DVI_AVT_PORT0;
                }
            }
            else if((UserPrefInputPriorityType!=Input_Priority_Auto) && (UserPrefInputType == Input_HDMI2))
            {
        #if (ENABLE_HDMI || ENABLE_DVI)
                    mdrv_hdmiRx_PMSwitchDVIDetect(Input_HDMI2);
        #endif

                ucReCheck = 3;
                Delay1ms(1);

                ucStatus = msReadByte(REG_DVI_RAW_CLK);

                while(((ucStatus & DVI_RAW_CLK_DET) == 0) && (ucReCheck > 0))
                {
                    Delay1ms(1);
                    ucStatus = msReadByte(REG_DVI_RAW_CLK);
                    ucReCheck--;
                }

                if(ucStatus & DVI_RAW_CLK_DET)
                {
                    if( INPUT_IS_TMDS1(Input_HDMI2) )
                        ucDVI_ACT_Port |= DVI_AVT_PORT1;
                }
            }
        #if(CHIP_ID == CHIP_MT9700) //hdmi port  MT9700:3 MT9701:2
            else if((UserPrefInputPriorityType!=Input_Priority_Auto) && (UserPrefInputType == Input_HDMI3))
            {
        #if (ENABLE_HDMI || ENABLE_DVI)
                    mdrv_hdmiRx_PMSwitchDVIDetect(Input_HDMI3);
        #endif

                ucReCheck = 3;
                Delay1ms(1);

                ucStatus = msReadByte(REG_DVI_RAW_CLK);

                while(((ucStatus & DVI_RAW_CLK_DET) == 0) && (ucReCheck > 0))
                {
                    Delay1ms(1);
                    ucStatus = msReadByte(REG_DVI_RAW_CLK);
                    ucReCheck--;
                }

                if(ucStatus & DVI_RAW_CLK_DET)
                {
                    if( INPUT_IS_TMDS2(Input_HDMI3) )
                        ucDVI_ACT_Port |= DVI_AVT_PORT2;
                }
            }
        #endif
        }
    }

    return ucDVI_ACT_Port;
}

void msPM_Enable_EDID_READ(BOOL bEnable)
{
    if(bEnable)
    {
        msWriteByteMask(REG_003E49, (ENABLE_VGA_EDID_INTERNAL_DATA?BIT7:0), BIT7); // enable DDC function for ADC_0
        msWriteByteMask(REG_003E45, (ENABLE_HDMI_EDID_INTERNAL_DATA?BIT7:0), BIT7); // enable DDC function for DVI_0
        msWriteByteMask(REG_003E4D, (ENABLE_HDMI_EDID_INTERNAL_DATA?BIT7:0), BIT7); // enable DDC function for DVI_1
        msWriteByteMask(REG_003E58, (ENABLE_HDMI_EDID_INTERNAL_DATA?BIT7:0), BIT7); // enable DDC function for DVI_2
        msWriteByteMask(REG_003EA8, (ENABLE_HDMI_EDID_INTERNAL_DATA?BIT7:0), BIT7); // enable DDC function for DVI_3
        msWriteByteMask(REG_003F44, (ENABLE_HDMI_EDID_INTERNAL_DATA?BIT7:0), BIT7); // enable DDC function for DVI_4
        msWriteByteMask(REG_003E60, (ENABLE_HDMI_EDID_INTERNAL_DATA?BIT7:0), BIT7); // reply ACK while source accesses A0_EDID with address is over 128
    }
    else
    {
        msWriteByteMask(REG_003E49, 0x00, BIT7); // disable  DDC function for ADC_0
        msWriteByteMask(REG_003E45, 0x00, BIT7); // disable  DDC function for DVI_0
        msWriteByteMask(REG_003E4D, 0x00, BIT7); // disable  DDC function for DVI_1
        msWriteByteMask(REG_003E58, 0x00, BIT7); // disable  DDC function for DVI_2
        msWriteByteMask(REG_003EA8, 0x00, BIT7); // disable  DDC function for DVI_3
        msWriteByteMask(REG_003F44, 0x00, BIT7); // disable  DDC function for DVI_4
    }
}

void msPM_EnableHVSyncDetect(BOOL bEnable)
{
    if(bEnable)
    {
        msWriteByteMask(REG_SYNC_DET, EN_SYNC_DET_SET, EN_SYNC_DET_MASK);
    }
    else
    {
        msWriteByteMask(REG_SYNC_DET, 0, EN_SYNC_DET_MASK);
    }
}

#if PM_SUPPORT_SOG_TIME_SHARE
//**************************************************************************
//  [Function Name]:
//                  msPM_EnableDVIClockAmp(BOOL bEnable)
//  [Description]
//                  Enable DVI clock amplifier control
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msPM_EnableDVIClockAmp(Bool bEnable)
{
    if(bEnable)
    {
        msWrite2ByteMask(REG_DVI_CTRL, EN_DVI_CTRL_SET|EN_DVI_CHEN_AUTO, EN_DVI_CTRL_MASK ); // enable DVI clock amplifier
    }
    else
    {
        msWrite2ByteMask(REG_DVI_CTRL, EN_DVI_ALWAYS_ON, EN_DVI_CTRL_MASK ); // disable DVI clock amplifier time sharing
    }
}
#endif

//**************************************************************************
//  [Function Name]:
//                  msPM_EnableSOGDetect(BOOL bEnable)
//  [Description]
//                  Enable SOG Detect: need power ADC VREF & BANGAP
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msPM_EnableSOGDetect(BOOL bEnable)
{
    if(bEnable)
    {
        #if(PM_SUPPORT_SOG_TIME_SHARE)
        msWriteBit(REG_0003E9, TRUE, _BIT7); // enable SOG time sharing option of VREF/BGAP to save power of SOG wakeup
        msPM_EnableDVIClockAmp(TRUE); // enable DVI clock amplifier control, because SOG time sharing requires DVI clock amplifier control.
        #else
        msWriteBit(REG_0003E9, FALSE, _BIT7); // disable SOG time sharing option of VREF/BGAP to save power of SOG wakeup
        #endif

        #if _NEW_SOG_WAKEUP_DET_
        NEW_SOG_WAKEUP_FLUSH();
        #endif

        msWriteByteMask(REG_SYNC_DET, EN_SOG_DET, EN_SOG_DET);
    }
    else
    {
        msWriteBit(REG_0003E9, FALSE, _BIT7); // disable SOG time sharing option of VREF/BGAP to save power of SOG wakeup
        msWriteByteMask(REG_SYNC_DET, 0, EN_SOG_DET);
    }
}

void msPM_EnableSARDetect(BOOL bEnable)
{
    sPMSARKey.u8SAR_KeyMask = 0x00;
    sPMSARKey.u8SAR_CmpLvl = 0x00;

    if (bEnable)
    {
        msWriteByteMask(REG_SARADC, BIT5, BIT4|BIT5|BIT6);
        msWriteByteMask(REG_SAR_ANYKEY, 0x00, BIT7);

        switch(sPMInfo.sPMConfig.ePMSARmode)
        {
            case ePMSAR_SAR0:
                msWriteByteMask(_REG_GPIOSAR_OEZ, BIT0, BIT0);
                msWriteByteMask(REG_SAR_CH_EN, BIT0, BIT0);
                sPMSARKey.u8SAR_KeyMask = BIT0;
                sPMSARKey.u8SAR_CmpLvl = EN_SAR_14V;
            break;

            case ePMSAR_SAR1:
                msWriteByteMask(_REG_GPIOSAR_OEZ, BIT1, BIT1);
                msWriteByteMask(REG_SAR_CH_EN, BIT1, BIT1);
                sPMSARKey.u8SAR_KeyMask = BIT1;
                sPMSARKey.u8SAR_CmpLvl = EN_SAR_14V;
            break;

            case ePMSAR_SAR2:
                msWriteByteMask(_REG_GPIOSAR_OEZ, BIT2, BIT2);
                msWriteByteMask(REG_SAR_CH_EN, BIT2, BIT2);
                sPMSARKey.u8SAR_KeyMask = BIT2;
                sPMSARKey.u8SAR_CmpLvl = EN_SAR_05V;
            break;

            case ePMSAR_SAR3:
                msWriteByteMask(_REG_GPIOSAR_OEZ, BIT3, BIT3);
                msWriteByteMask(REG_SAR_CH_EN, BIT3, BIT3);
                sPMSARKey.u8SAR_KeyMask = BIT3;
                sPMSARKey.u8SAR_CmpLvl = EN_SAR_14V;
            break;

            case ePMSAR_SAR12:
                msWriteByteMask(_REG_GPIOSAR_OEZ, BIT1|BIT2, BIT1|BIT2);
                msWriteByteMask(REG_SAR_CH_EN, BIT1|BIT2, BIT1|BIT2);
                sPMSARKey.u8SAR_KeyMask = BIT1|BIT2;
                sPMSARKey.u8SAR_CmpLvl = EN_SAR_14V;
             break;

            case ePMSAR_SAR123:
                msWriteByteMask(_REG_GPIOSAR_OEZ, BIT1|BIT2|BIT3, BIT1|BIT2|BIT3);
                msWriteByteMask(REG_SAR_CH_EN, BIT1|BIT2|BIT3, BIT1|BIT2|BIT3);
                sPMSARKey.u8SAR_KeyMask = BIT1|BIT2|BIT3;
                sPMSARKey.u8SAR_CmpLvl = EN_SAR_14V;
            break;

            case ePMSAR_SAR01:
                msWriteByteMask(_REG_GPIOSAR_OEZ, BIT0|BIT1, BIT0|BIT1);
                msWriteByteMask(REG_SAR_CH_EN, BIT0|BIT1, BIT0|BIT1);
                sPMSARKey.u8SAR_KeyMask = BIT0|BIT1;
                sPMSARKey.u8SAR_CmpLvl = EN_SAR_14V;
            break;

            case ePMSAR_SAR012:
                msWriteByteMask(_REG_GPIOSAR_OEZ, BIT0|BIT1|BIT2, BIT0|BIT1|BIT2);
                msWriteByteMask(REG_SAR_CH_EN, BIT0|BIT1|BIT2, BIT0|BIT1|BIT2);
                sPMSARKey.u8SAR_KeyMask = BIT0|BIT1|BIT2;
                sPMSARKey.u8SAR_CmpLvl = EN_SAR_14V;
            break;
        }

        msWriteByteMask(REG_003A62, sPMInfo.sPMConfig.ePMSARmode, sPMInfo.sPMConfig.ePMSARmode);
    }
    else
    {
        msWriteByteMask(REG_SARADC, BIT6, BIT6);
    }

#if ENABLE_VBUS_DETECT_SAR
    msWriteByteMask(REG_003A62, VBUS_DET_SAR_CH, VBUS_DET_SAR_CH);
#endif
}

#define SARTHR_25V      ((255ul*250)/330)
#define SARTHR_14V      ((255ul*140)/330)
#define SARTHR_05V      ((255ul*50)/330)
BOOL msPM_CompareSARChannel(void)
{
    BYTE u8CmpThreshold = 0;
    BOOL bSAR_Result = FALSE;

    if(u8CmpThreshold == 0)
    {
        if(sPMSARKey.u8SAR_CmpLvl == EN_SAR_25V)
            u8CmpThreshold = SARTHR_25V;
        else if(sPMSARKey.u8SAR_CmpLvl == EN_SAR_14V)
            u8CmpThreshold = SARTHR_14V;
        else if(sPMSARKey.u8SAR_CmpLvl == EN_SAR_05V)
            u8CmpThreshold = SARTHR_05V;
    }

    if((_bit0_(sPMSARKey.u8SAR_KeyMask)) && (KEYPAD_SAR00 <= u8CmpThreshold))
        bSAR_Result = TRUE;
    else if((_bit1_(sPMSARKey.u8SAR_KeyMask)) && (KEYPAD_SAR01 <= u8CmpThreshold))
        bSAR_Result = TRUE;
    else if((_bit2_(sPMSARKey.u8SAR_KeyMask)) && (KEYPAD_SAR02 <= u8CmpThreshold))
        bSAR_Result = TRUE;
    else if((_bit3_(sPMSARKey.u8SAR_KeyMask)) && (KEYPAD_SAR03 <= u8CmpThreshold))
        bSAR_Result = TRUE;
    return bSAR_Result;
}

#if PM_CABLEDETECT_USE_GPIO
void msPM_CableDetectStates()
{
    WORD INV_GPIO_POL_SET_Temp = INV_GPIO_POL_SET;
#if !(CABLE_DETECT_VGA_USE_SAR)
    if(hwDSUBCable_Pin)
        INV_GPIO_POL_SET_Temp = (INV_GPIO_POL_SET_Temp)&~PM_VGACBL_DET;
#endif
#if !(CABLE_DETECT_VGA_USE_SAR)
    if(hwDVICable_Pin)
        INV_GPIO_POL_SET_Temp = (INV_GPIO_POL_SET_Temp)&~PM_DVICBL_DET;
#endif
#if 0
    if(DET_HDMIA)
        INV_GPIO_POL_SET_Temp=(INV_GPIO_POL_SET_Temp)&~PM_HDMIACBL_DET;

    if(DET_HDMIB)
        INV_GPIO_POL_SET_Temp=(INV_GPIO_POL_SET_Temp)&~PM_HDMIBCBL_DET;

    if(DET_HDMIC)
        INV_GPIO_POL_SET_Temp=(INV_GPIO_POL_SET_Temp)&~PM_HDMICCBL_DET;
#endif

    old_msWrite2ByteMask(REG_PM_62,INV_GPIO_POL_SET_Temp, INV_GPIO_POL_MASK);

}
#endif

void msPM_EnableGPIODetect(BOOL bEnable)
{
    if(bEnable)
    {
        msWrite2ByteMask(REG_PM_60 , EN_GPIO_DET_SET , EN_GPIO_DET_MASK);
        msWrite2ByteMask(REG_PM_62, INV_GPIO_POL_SET, INV_GPIO_POL_MASK);

        #if PM_CABLEDETECT_USE_GPIO
        msPM_CableDetectStates();
        #endif
    }
    else
    {
        msWrite2ByteMask(REG_PM_60 , 0x00, EN_GPIO_DET_MASK);
        msWrite2ByteMask(REG_PM_62, 0x00, INV_GPIO_POL_MASK);
    }
}

void msPM_EnableMCCSDetect(BOOL bEnable)
{
    if(bEnable)
    {
        msWriteByte(REG_003E0A, 0xB7);  // enable DDC2Bi for A0
        msWriteByte(REG_003E0C, 0xB7);  // enable DDC2Bi for D0
        msWriteByte(REG_003E0D, 0xB7);  // enable DDC2Bi for D1
        msWriteByte(REG_003E52, 0xB7);  // enable DDC2Bi for D2
    #if(CHIP_ID == CHIP_MT9700)
        msWriteByte(REG_003EA2, 0xB7);  // enable DDC2Bi for D3
    #endif
        msWriteByteMask(REG_003E09, 0, (BIT1|BIT0)); //disable No ACK ,Hold CLK
        msWriteByteMask(REG_003EC1, BIT7|BIT6, BIT7|BIT6); // enable A0, D0
        msWriteByteMask(REG_003EEF, BIT7|BIT6, BIT7|BIT6); // enable D1, D2
    #if(CHIP_ID == CHIP_MT9700)
        msWriteByteMask(REG_003EB4, BIT4, BIT4); // enable D3
    #endif
    }
    else
    {
        msWriteBit(REG_003E0A, 0, BIT7);  // disable DDC2Bi for A0
        msWriteBit(REG_003E0C, 0, BIT7);  // disable DDC2Bi for D0
        msWriteBit(REG_003E0D, 0, BIT7);  // disable DDC2Bi for D1
        msWriteBit(REG_003E52, 0, BIT7);  // disable DDC2Bi for D2
    #if(CHIP_ID == CHIP_MT9700)
        msWriteBit(REG_003EA2, 0, BIT7);  // enable DDC2Bi for D3
    #endif
        msWriteByteMask(REG_003EC1, 0, BIT7|BIT6); // disable A0, D0
        msWriteByteMask(REG_003EEF, 0, BIT7|BIT6); // disable D1, D2
    #if(CHIP_ID == CHIP_MT9700)
        msWriteByteMask(REG_003EB4, 0, BIT4); // enable D3
    #endif
    }
}

void msPM_MCCSReset(void)
{
    msWriteByte(REG_003EC0, 0xFF); // clear status of A0, D0
    msWriteByte(REG_003EC0, 0x00);
    msWriteByte(REG_003EEE, 0xFF); // clear status of D1, D2
    msWriteByte(REG_003EEE, 0x00);

#if (CHIP_ID == CHIP_MT9700)
    msWriteByteMask(REG_003EB4, 0x0F, 0x0F); // clear status of D3
    msWriteByteMask(REG_003EB4, 0x00, 0x0F);
#endif
}

void msPM_ClearStatus(Bool bResetPM)
{
    msWriteBit(REG_PM_81, TRUE, BIT7);
    msWriteBit(REG_PM_81, 0, BIT7);

    //*********************************************************
    //Software reset PM //Sky110719
    //
    //Some wake up event just report a pulse    ____| |___,
    //if wake up event keep happen after Wake up status Clear,
    //wake up report register will not update status.
    //software reset can solve it
    //*********************************************************
    if(bResetPM)
        msWriteBit(REG_PM_83, TRUE, BIT1);

    msWriteBit(REG_PM_83, 0, BIT1);
}

void msPM_OutputTriState(void)
{

}

void msPM_ClockGating(void)
{

    if (sPMInfo.ucPMMode == ePM_POWERON)
    {
        msWriteByteMask(REG_PM_DA, BIT2|BIT1|BIT0, BIT2|BIT1|BIT0);//Xtal , FRO12, 24, mcu clk enable
        msWriteByteMask(REG_PM_8C, 0x00, BIT0); //ckg_uart0_pm
        msWriteByteMask(REG_PM_8D, 0x00, BIT0); //ckg_uart1_pm
    #if(CHIP_ID == CHIP_MT9700)
        msWriteByteMask(REG_PM_96, 0x00, BIT2|BIT0); //ckg_cec0 & ckg_imi
    #elif(CHIP_ID == CHIP_MT9701)
        msWriteByteMask(REG_PM_96, 0x00, BIT0); //ckg_cec0
    #endif
        msWriteByteMask(REG_PM_B0, 0x00, BIT4|BIT0); //ckg_scdc0 &scdc1
        msWriteByteMask(REG_PM_B1, 0x00, BIT4|BIT0); //ckg_scdc2 & ckg_miic0_pm
        msWriteByteMask(REG_PM_B2, 0x00, BIT4|BIT0); //ckg_miic1_pm & ckg_hotplug0
        msWriteByteMask(REG_PM_76, 0x00, BIT0); //ckg_ocp_sar
    }
    else if(sPMInfo.ucPMMode == ePM_STANDBY)
    {
        msWriteByteMask(REG_PM_8C, BIT0, BIT0); //ckg_uart0_pm
        msWriteByteMask(REG_PM_8D, BIT0, BIT0); //ckg_uart1_pm
     #if(CHIP_ID == CHIP_MT9700)
        msWriteByteMask(REG_PM_96, BIT2, BIT2); //ckg_imi
    #endif
        msWriteByteMask(REG_PM_B2, BIT0, BIT0); //ckg_miic1_pm
        msWriteByteMask(REG_PM_76, BIT0, BIT0); //ckg_ocp_sar

#if !ENABLE_USB_TYPEC
        msWriteByteMask(REG_PM_B1, BIT4, BIT4); //ckg_miic0_pm
        msWriteByteMask(REG_PM_DA, 0x00, BIT2|BIT1|BIT0);//Xtal , FRO12, 24, mcu clk enable
#endif

    }
    else //ePM_POWEROFF
    {
        msWriteByteMask(REG_PM_8C, BIT0, BIT0); //ckg_uart0_pm
        msWriteByteMask(REG_PM_8D, BIT0, BIT0); //ckg_uart1_pm
    #if(CHIP_ID == CHIP_MT9700)
        msWriteByteMask(REG_PM_96, BIT2|BIT0, BIT2|BIT0); //ckg_cec0 & ckg_imi
    #elif(CHIP_ID == CHIP_MT9701)
        msWriteByteMask(REG_PM_96, BIT0, BIT0); //ckg_cec0
    #endif
        msWriteByteMask(REG_PM_B0, BIT4|BIT0, BIT4|BIT0); //ckg_scdc0 &scdc1
        msWriteByteMask(REG_PM_B1, BIT0, BIT0); //ckg_scdc2
        msWriteByteMask(REG_PM_B2, BIT4|BIT0, BIT4|BIT0); //ckg_miic1_pm & ckg_hotplug0
        msWriteByteMask(REG_PM_76, BIT0, BIT0); //ckg_ocp_sar

#if !ENABLE_USB_TYPEC
        msWriteByteMask(REG_PM_B1, BIT4, BIT4); //ckg_miic0_pm
        msWriteByteMask(REG_PM_DA, 0x00, BIT2|BIT1|BIT0);//Xtal , FRO12, 24, mcu clk enable
#endif
    }
}

void msPM_InterruptEnable(Bool benable)
{
    INT_NPM_IRQ_SCINT_ENABLE(benable);
}

void msPM_PowerDownMacro(void)
{
    msPM_InterruptEnable(FALSE);

#if ENABLE_VGA_INPUT
    //---ADC
    if(sPMInfo.sPMConfig.bHVSync_enable || sPMInfo.sPMConfig.bSOG_enable)
    {
        msWriteByteMask(REG_002508, 0x00, BIT1|BIT2);
#if MS_VGA_SOG_EN
        msWriteByteMask(REG_002509, 0x00, BIT1|BIT2);
        #if _NEW_SOG_DET_
        msWriteByteMask(REG_00254E, BIT7, BIT7);
        #endif
        msWriteByteMask(REG_002560, 0x00, BIT0);
#endif
        msWriteByteMask(REG_0025B4, 0x00, BIT5|BIT6|BIT7);
	#if _NEW_SOG_DET_
        msWriteByteMask(REG_0025F0, 0x02, 0x0F);
        msWrite2ByteMask(REG_0025F2, 0x00C8, 0x0FFF);
        msWrite2ByteMask(REG_0025F0, 0x9280, 0xFFF0);
        msWriteByteMask(REG_0025F3, 0x10, 0xF0);
        msWriteByteMask(REG_0025F4, BIT2, BIT1|BIT2|BIT3);
	#endif
    }
    else
    {
        msWriteByteMask(REG_002508, BIT1|BIT2, BIT1|BIT2);
        msWriteByteMask(REG_002509, BIT1|BIT2, BIT1|BIT2);
        #if _NEW_SOG_DET_
        msWriteByteMask(REG_00254E, 0x00, BIT7);
        #endif
        msWriteByteMask(REG_002560, BIT0, BIT0);
        msWriteByteMask(REG_0025B4, BIT5|BIT6|BIT7, BIT5|BIT6|BIT7);
	#if _NEW_SOG_DET_
        msWrite2Byte(REG_0025F0, 0x0000);
        msWrite2Byte(REG_0025F2, 0x0000);
        msWriteByteMask(REG_0025F4, BIT1, BIT1|BIT2|BIT3);
	#endif
    }
#endif

    /*//---DVI
    //confirming with RD Ocasi
    #if (PM_SUPPORT_WAKEUP_DVI)
    if(sPMInfo.sPMConfig.bDVI_enable)
    {
        old_msWriteByteMask(REG_01C2,BIT5 ,BIT6|BIT5);
        old_msWriteByteMask(REG_01C3,0x00 ,BIT7|BIT5|BIT4);
        old_msWriteByteMask(REG_01CD,0x00 ,BIT0);
        old_msWriteByteMask(REG_01CC,0x00 ,BIT2|BIT1|BIT0);
        old_msWriteByteMask(REG_01CE, BIT3 ,BIT3|BIT2|BIT1|BIT0);    //diff
        old_msWriteByteMask(REG_17BE, BIT0 ,BIT0);
        old_msWriteByteMask(REG_17C0, 0xA7 ,BIT7|BIT5|BIT2|BIT1|BIT0);   //diff
        old_msWriteByteMask(REG_17C1, 0x3F ,BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
    }
    else
    #endif
    {
        old_msWriteByteMask(REG_01C2,0x00 ,BIT6|BIT5);
        old_msWriteByteMask(REG_01C3,0x0B ,BIT7|BIT5|BIT4);
        old_msWriteByteMask(REG_01CD,0x00 ,BIT0);
        old_msWriteByteMask(REG_01CC,0x00 ,BIT2|BIT1|BIT0);
        old_msWriteByteMask(REG_01CE, 0x0F ,BIT3|BIT2|BIT1|BIT0);     //Diff
        old_msWriteByteMask(REG_17BE, BIT0 ,BIT0);
        old_msWriteByteMask(REG_17C0, 0xA7 ,BIT7|BIT5|BIT2|BIT1|BIT0);    //Diff
        old_msWriteByteMask(REG_17C1, 0x3F ,BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
    }*/

    //---MOD ATOP
    //old_msWriteByteMask(REG_30F0, BIT0, BIT0);
    //old_msWriteByteMask(REG_30EE, 0x00, BIT2|BIT1|BIT0);
    //old_msWriteByteMask(REG_30DA, 0x00, 0xFF);
    //old_msWriteByteMask(REG_308A,  BIT5|BIT4, BIT5|BIT4);

    #if CHIP_ID == CHIP_MT9701
    msWriteByteMask(REG_1405D8, BIT3, BIT3); // reg_mpll_pd
    msWriteByteMask(REG_1405DF, BIT0, BIT0); // reg_mpll_digclk_pd
    msWriteByteMask(REG_1405D8, BIT4, BIT4); // reg_mpll_clk_adc_vco_div2_pd
    msWriteByteMask(REG_140506, BIT5, BIT5); // reg_lpll1_pd
    msWriteByteMask(REG_140566, BIT5, BIT5); // reg_lpll2_pd
    #endif

    //---SAR
    if(sPMInfo.sPMConfig.bSAR_enable)
    {
        msWriteByteMask(REG_003A60,0x00, BIT6);
    }
    else
    {
        msWriteByteMask(REG_003A60,BIT6, BIT6); //SAR PD
    }

    msPM_ClockGating();

	#if (ENABLE_DP_INPUT == 0x1)
    //---DP
    mapi_DPRx_SetPowerDownControl(Input_Displayport_C1, sPMInfo.ucPMMode);
    mapi_DPRx_SetPowerDownControl(Input_Displayport_C2, sPMInfo.ucPMMode);
    mapi_DPRx_SetPowerDownControl(Input_Displayport_C3, sPMInfo.ucPMMode);
    mapi_DPRx_SetPowerDownControl(Input_Displayport_C4, sPMInfo.ucPMMode);
    mapi_DPRx_SetPowerDownControl(Input_UsbTypeC_C4, sPMInfo.ucPMMode);
    mapi_DPRx_SetPhyPowerDownControl(Input_Displayport_C1, sPMInfo.ucPMMode);
    mapi_DPRx_SetPhyPowerDownControl(Input_Displayport_C2, sPMInfo.ucPMMode);
    mapi_DPRx_SetPhyPowerDownControl(Input_Displayport_C3, sPMInfo.ucPMMode);
    mapi_DPRx_SetPhyPowerDownControl(Input_Displayport_C4, sPMInfo.ucPMMode);
    mapi_DPRx_SetPhyPowerDownControl(Input_UsbTypeC_C4, sPMInfo.ucPMMode);
    #endif

    if(sPMInfo.sPMConfig.eTYPECmode == ePMTYPEC_ON)
    {
        #if ENABLE_SECU_R2
        if(g_u8SecuR2Alive) // After the 1st PowerDown the IMI might be lack of miu clock.
        {
            msAPI_combo_SECU_PowerDown(TRUE);
            g_u8SecuR2Alive = 0;
        }
        #endif

        //---CLK Gating
        msInitClockGating(TRUE);
    }

#if PANEL_EDP
    if( g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE )
    {
        System_eDPTx_PowerDown();
    }
#endif

#if ENABLE_HPERIOD_16LINE_MODE
    SC0_HPEROID_DETECT_MODE(FALSE); // set to 8 line mode to prevent counter overflow if core pwr is always on during PM mode
#endif


#if ENABLE_PQ_R2
    MDrv_Sys_ResetPQ();
#endif
    //---MPLL_PD
    //old_msWriteByteMask(REG_1ED1,BIT0, BIT0); // [4]: MPLL power DOWN //move to msPM_SetPMClock()
    old_msWriteByteMask(REG_1EDC, BIT5|BIT4, BIT5|BIT4); //[5][4]:Power Down MPLL 216/432
}

void msPM_PowerUpMacro(void)
{
    //follow power down list to update it
    msPM_ClockGating();

#if ENABLE_VGA_INPUT
    //---ADC
    msWriteByteMask(REG_0003E9, 0x00, BIT7);
    msWriteByteMask(REG_002508, 0x00, BIT1|BIT2);
    msWriteByteMask(REG_002509, 0x00, BIT1|BIT2);
    #if _NEW_SOG_DET_
    msWriteByteMask(REG_00254E, 0x00, BIT7);
    #endif
    msWriteByteMask(REG_002560, 0x00, BIT0);
    msWriteByteMask(REG_0025B4, 0x00, BIT5|BIT6|BIT7);
    #if _NEW_SOG_DET_
    msWriteByteMask(REG_0025F0, 0x02, 0x0F);
    msWrite2ByteMask(REG_0025F2, 0x00C8, 0x0FFF);
    msWrite2ByteMask(REG_0025F0, 0x9280, 0xFFF0);
    msWriteByteMask(REG_0025F3, 0x10, 0xF0);
    msWriteByteMask(REG_0025F4, BIT2, BIT1|BIT2|BIT3);
    #endif
#endif

    /*//---DVI ATOP
    old_msWriteByteMask(REG_01C2,BIT5,BIT6|BIT5);
    old_msWriteByteMask(REG_01C3,0x00 ,BIT7|BIT5|BIT4);
    old_msWriteByteMask(REG_01CD,0x00 ,BIT0);
    old_msWriteByteMask(REG_01CC,0x00 ,BIT2|BIT1|BIT0);
    old_msWriteByteMask(REG_01CE,0x00 ,BIT3|BIT2|BIT1|BIT0);      //Diff
    old_msWriteByteMask(REG_17BE, 0x00 ,BIT0);
    old_msWriteByteMask(REG_17C0, 0x00 ,BIT7|BIT5|BIT2|BIT1|BIT0);    //Diff
    old_msWriteByteMask(REG_17C1, 0x00 ,BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);*/

    //---MOD ATOP
    //old_msWriteByteMask(REG_30F0, 0x00, BIT0);
    //old_msWriteByteMask(REG_30EE, BIT2|BIT1|BIT0, BIT2|BIT1|BIT0);
    //old_msWriteByteMask(REG_30DA, 0xFF, 0xFF);
    old_msWriteByteMask(REG_308A,  0x00, BIT5|BIT4);

    //---KeyPad SAR
    msWriteByteMask(REG_003A60,0x00, BIT6); //[6]SAR_PD

    //---MPLL_PD
    //old_msWriteByteMask(REG_1ED1,BIT0, BIT0); // [4]: MPLL power DOWN //move to msPM_SetPMClock()
    old_msWriteByteMask(REG_1EDC, 0x00, BIT5|BIT4); //[5][4]:Power Down MPLL 216/432

    //MPLL_POWER_UP(_ENABLE);
    mcuSetSystemSpeed(SPEED_NORMAL_MODE );
    #if ENABLE_UART_PIU
    piu_uart_init(CLK_PIU_172M, UART_PIU_BAUDRATE);
    #endif

    // after mpll on
#if (ENABLE_HDMI || ENABLE_DVI)
        //---DVI
        mdrv_hdmiRx_PMEnableDVIDetect(HDMI_PM_POWERON);
#endif

#if ENABLE_USB_TYPEC
#if CHIP_ID == CHIP_MT9701
    if(!g_u8Usb2Alive)
        msDrvUsb2Init(); // For usb2 device(e.g. BB ... etc)
    if(!sPMInfo.sPMConfig.bTYPEC_enable)
        msDrvUsbSwitchInit();
#else //MT9700
	msDrvUsb2Init(); // For usb2 device(e.g. BB ... etc)
#endif
#endif

    Init_ExternalInterrupt();
    ScalerReset(RST_ALL);
    mStar_Init(FALSE); // remove it to speed up powering on time????
    DDC2Bi_Init();
    msPM_InterruptEnable(TRUE);

    #if (ENABLE_DP_INPUT == 0x1)
    //---DP
    mapi_DPRx_SetPowerDownControl(Input_Displayport_C1, sPMInfo.ucPMMode);
    mapi_DPRx_SetPowerDownControl(Input_Displayport_C2, sPMInfo.ucPMMode);
    mapi_DPRx_SetPowerDownControl(Input_Displayport_C3, sPMInfo.ucPMMode);
    mapi_DPRx_SetPowerDownControl(Input_Displayport_C4, sPMInfo.ucPMMode);
    mapi_DPRx_SetPowerDownControl(Input_UsbTypeC_C4, sPMInfo.ucPMMode);
    mapi_DPRx_SetPhyPowerDownControl(Input_Displayport_C1, sPMInfo.ucPMMode);
    mapi_DPRx_SetPhyPowerDownControl(Input_Displayport_C2, sPMInfo.ucPMMode);
    mapi_DPRx_SetPhyPowerDownControl(Input_Displayport_C3, sPMInfo.ucPMMode);
    mapi_DPRx_SetPhyPowerDownControl(Input_Displayport_C4, sPMInfo.ucPMMode);
    mapi_DPRx_SetPhyPowerDownControl(Input_UsbTypeC_C4, sPMInfo.ucPMMode);
    #endif

#if (PANEL_EDP && (CHIP_ID == CHIP_MT9700))
    if( g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE )
        mapi_eDPTx_PowerDown(FALSE);
#endif
}

Bool msPM_GetPMStatus(void)
{
    BOOL bResault = FALSE;

    BYTE ucStatus_85;
    BYTE ucStatus_86;
    BYTE ucDVI_ACT_Port = 0;
#if (PM_SUPPORT_WAKEUP_HDMI_SQH && ENABLE_HDMI)
    BYTE ucHDMI_SQH_ACT_Port;
#endif

#if(CHIP_ID == CHIP_MT9701)
#if (PM_SUPPORT_WAKEUP_HDMI_SCDC5V && (ENABLE_HDMI || ENABLE_DVI))
    BYTE ucHDMI_SCDC5V_ACT_Port;
#endif
#endif

    volatile WORD ucStatus_GPIO;
#if (ENABLE_DP_INPUT)
    BYTE u8ActivePort = 0;
#endif
    BYTE ucStatus_0C;

    ucWakeupStatus = ePMSTS_NON;

    ucStatus_85   = msReadByte(REG_EVENT1);
    ucStatus_86   = msReadByte(REG_EVENT2);
    ucStatus_GPIO = msRead2Byte(REG_PM_64);
    ucStatus_0C   = msReadByte(REG_EVENT4);

#if (PM_SUPPORT_WAKEUP_DVI && (ENABLE_HDMI|| ENABLE_DVI))
    ucDVI_ACT_Port = msPM_DVIClockDetect();
#endif

#if (PM_SUPPORT_WAKEUP_HDMI_SQH && ENABLE_HDMI)
    ucHDMI_SQH_ACT_Port = msPM_HDMI_SQH_Detect();
#endif

#if(CHIP_ID == CHIP_MT9701)
#if (PM_SUPPORT_WAKEUP_HDMI_SCDC5V && (ENABLE_HDMI || ENABLE_DVI))
    ucHDMI_SCDC5V_ACT_Port = msPM_HDMI_SCDC5V_Detect();
#endif
#endif

#if PM_POWERkEY_GETVALUE
    if(PowerKey == 0)
    {
        KeypadButton = BTN_Nothing;                //110914 Rick add for enable OSDLock function while PM On - A023
        Key_ScanKeypad();
        ucWakeupStatus =ePMSTS_POWERGPIO_ACT;
        bResault = TRUE;
        PM_printData("ePMSTS_POWERGPIO_ACT:%d", ePMSTS_POWERGPIO_ACT);
    }
    else
#endif

#if ENABLE_MSBHK
    if(ucMSBHK_WakeupStatus)
    {
        ucWakeupStatus = ucMSBHK_WakeupStatus;
        ucMSBHK_WakeupStatus = ePMSTS_NON;
        bResault = TRUE;
    }
    else
#endif

#if PM_CABLE_DETECT_USE_SAR
    if((abs(sPMInfo.bCABLE_SAR_VALUE-CABLE_DET_SAR)>5)&&(PowerOnFlag))
    {
              ucWakeupStatus =ePMSTS_CABLESAR_ACT;
        bResault = TRUE;
        PM_printData("ePMSTS_CABLESAR_ACT:%d", ePMSTS_CABLESAR_ACT);
    }
    else
#endif

    if((ucStatus_85 & (HSYNC_DET_0|VSYNC_DET_0|SOG_DET_0|HV_DET_0)) || (ucStatus_86 & CS_DET))
    {
        ucWakeupStatus = ePMSTS_VGA_ACT;
        SrcInputType = Input_Analog1;
        bResault = TRUE;
        PM_printData("ePMSTS_VGA_ACT:%d", ePMSTS_VGA_ACT);
    }
    else if(ucStatus_GPIO & EN_GPIO_DET_MASK)
    {
        ucWakeupStatus = ePMSTS_GPIO_ACT;
        bResault = TRUE;
        PM_printData("ePMSTS_GPIO_ACT:%d", ePMSTS_GPIO_ACT);
    }
    else
    {

        if(ucStatus_0C & CEC_WAKEUP)
        {
            ucWakeupStatus = ePMSTS_CEC_ACT;
            bResault = TRUE;
            PM_printData("ePMSTS_CEC_ACT:%d", ePMSTS_CEC_ACT);
        }
        else if(ucStatus_86 & D2B_WAKEUP)
        {
            volatile BYTE mccsWakeUpStatus = 0 ;
            if(msReadByte(REG_003EC2)&BIT2) // A0
            {
                mccsWakeUpStatus = msReadByte(REG_003EC4);
                msWriteBit(REG_003EC0, TRUE, BIT6);//Clr
            }
            else if(msReadByte(REG_003EC2)&BIT3) // D0
            {
                mccsWakeUpStatus = msReadByte(REG_003EC5);
                msWriteBit(REG_003EC0, TRUE, BIT7);//Clr
            }
            else if(msReadByte(REG_003EF0)&BIT2) // D1
            {
                mccsWakeUpStatus = msReadByte(REG_003EF2);
                msWriteBit(REG_003EEE, TRUE, BIT6);//Clr
            }
            else if(msReadByte(REG_003EF0)&BIT3) // D2
            {
                mccsWakeUpStatus = msReadByte(REG_003EF3);
                msWriteBit(REG_003EEE, TRUE, BIT7);//Clr
            }
            else if(msReadByte(REG_003EB6)&BIT6) // D3
            {
                mccsWakeUpStatus = msReadByte(REG_003EB9);
                msWriteBit(REG_003EB4, TRUE, BIT3);//Clr
            }
            else if(msReadByte(REG_003EC2)&BIT3) // D4
                mccsWakeUpStatus = msReadByte(REG_003EC4);

            if(mccsWakeUpStatus == 0x05)
                ucWakeupStatus = ePMSTS_MCCS05_ACT;
            else if(mccsWakeUpStatus == 0x04)
                ucWakeupStatus = ePMSTS_MCCS04_ACT;
            else
                ucWakeupStatus = ePMSTS_MCCS01_ACT;
            bResault = TRUE;

            PM_printData("D2B_WAKEUP:%d", ucWakeupStatus);
        }
        else if(msPM_CompareSARChannel())
        {
            ucWakeupStatus = ePMSTS_SAR_ACT;
            Set_SARwakeupFlag();
            bResault = TRUE;
            PM_printData("ePMSTS_SAR_ACT:%d", ePMSTS_SAR_ACT);
        }
        //================================================================================
        //  Move DP wake up checking first before DVI, because DVI clock detected bit would report 1 when DP plug in.
        //================================================================================
#if (ENABLE_DP_INPUT)
        else if(sPMInfo.sPMConfig.bDP_enable && ((u8ActivePort = msPM_GetActiveDPPort(0x1F)) != 0xFF)) // Normal Trining Wake up
//        else if((u8ActivePort = msPM_GetActiveDPPort(0x1F)) != 0xFF) // Normal Trining Wake up
        {
            #if(PM_SUPPORT_WAKEUP_DP)
            ucWakeupStatus = ePMSTS_DP_ACT;
            #endif

            bResault = TRUE;
            PM_printData("u8ActivePort:%x \r\n", u8ActivePort);
            #if(PM_SUPPORT_WAKEUP_DP)
            PM_printData("ePMSTS_DP_ACT NormalTraining:%x\r\n", ePMSTS_DP_ACT);
            #endif
        }
#endif
#if (PM_SUPPORT_WAKEUP_DVI)
        else if((ucDVI_ACT_Port & DVI_AVT_PORT0)&&((Input_HDMI_C1 != Input_Nothing)||(Input_DVI_C1 != Input_Nothing)))
        {
            if(Input_HDMI != Input_Nothing)
                SrcInputType = Input_HDMI;
            else
                SrcInputType = Input_DVI;

            ucWakeupStatus = ePMSTS_DVI_0_ACT;
            bResault = TRUE;
            PM_printData("ePMSTS_DVI_0_ACT:%d", ePMSTS_DVI_0_ACT);
        }
        else if((ucDVI_ACT_Port & DVI_AVT_PORT1)&&((Input_HDMI_C2 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)))
        {
            if(Input_HDMI2 != Input_Nothing)
                SrcInputType = Input_HDMI2;
            else
                SrcInputType = Input_DVI2;

            ucWakeupStatus = ePMSTS_DVI_1_ACT;
            bResault = TRUE;
            PM_printData("ePMSTS_DVI_1_ACT:%d", ePMSTS_DVI_1_ACT);
        }
        else if((ucDVI_ACT_Port & DVI_AVT_PORT2)&&((Input_HDMI_C3 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing)))
        {
            if(Input_HDMI3 != Input_Nothing)
                SrcInputType = Input_HDMI3;
            else
                SrcInputType = Input_DVI3;

            ucWakeupStatus = ePMSTS_DVI_2_ACT;
            bResault = TRUE;
            PM_printData("ePMSTS_DVI_2_ACT:%d", ePMSTS_DVI_2_ACT);
        }
        else if((ucDVI_ACT_Port & DVI_AVT_PORT3)&&((Input_HDMI_C4 != Input_Nothing)||(Input_DVI_C4 != Input_Nothing)))
        {
            if(Input_HDMI4 != Input_Nothing)
                SrcInputType = Input_HDMI4;
            else
                SrcInputType = Input_DVI4;

            ucWakeupStatus = ePMSTS_DVI_3_ACT;
            bResault = TRUE;
            PM_printData("ePMSTS_DVI_3_ACT:%d", ePMSTS_DVI_3_ACT);
        }
#endif
#if (PM_SUPPORT_WAKEUP_HDMI_SQH && ENABLE_HDMI && PM_SUPPORT_WAKEUP_DVI)
        else if((ucHDMI_SQH_ACT_Port & DVI_AVT_PORT0)&&((Input_HDMI_C1 != Input_Nothing)||(Input_DVI_C1 != Input_Nothing)))
        {
            if(Input_HDMI != Input_Nothing)
                SrcInputType = Input_HDMI;
            else
                SrcInputType = Input_DVI;

            ucWakeupStatus = ePMSTS_DVI_0_ACT;
            bResault = TRUE;
            PM_printData("[HDMI][SQH_Wakeup]ePMSTS_DVI_0_ACT:%d", ePMSTS_DVI_0_ACT);
        }
        else if((ucHDMI_SQH_ACT_Port & DVI_AVT_PORT1)&&((Input_HDMI_C2 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)))
        {
            if(Input_HDMI2 != Input_Nothing)
                SrcInputType = Input_HDMI2;
            else
                SrcInputType = Input_DVI2;

            ucWakeupStatus = ePMSTS_DVI_1_ACT;
            bResault = TRUE;
            PM_printData("[HDMI][SQH_Wakeup]ePMSTS_DVI_1_ACT:%d", ePMSTS_DVI_1_ACT);
        }
#endif
#if (CHIP_ID == CHIP_MT9701)
#if (PM_SUPPORT_WAKEUP_HDMI_SCDC5V && (ENABLE_HDMI || ENABLE_DVI))
        else if((ucHDMI_SCDC5V_ACT_Port & DVI_AVT_PORT0)&&((Input_HDMI_C1 != Input_Nothing)||(Input_DVI_C1 != Input_Nothing)))
        {
            if(Input_HDMI != Input_Nothing)
                SrcInputType = Input_HDMI;
            else
                SrcInputType = Input_DVI;

            ucWakeupStatus = ePMSTS_DVI_0_ACT;
            msWriteByteMask(0x003119, 0x02, 0x06);
            msWriteByteMask(0x003219, 0x02, 0x06);
            msWriteByteMask(0x00310F, 0xFF, 0xFF);
            msWriteByteMask(0x00320F, 0xFF, 0xFF);
            msWriteByteMask(0x00310F, 0x00, 0xFF);
            msWriteByteMask(0x00320F, 0x00, 0xFF);
            bResault = TRUE;
            PM_printData("[HDMI][SCDC5V_Wakeup]ePMSTS_DVI_0_ACT:%d", ePMSTS_DVI_0_ACT);
        }
        else if((ucHDMI_SCDC5V_ACT_Port & DVI_AVT_PORT1)&&((Input_HDMI_C2 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)))
        {
            if(Input_HDMI2 != Input_Nothing)
                SrcInputType = Input_HDMI2;
            else
                SrcInputType = Input_DVI2;

            ucWakeupStatus = ePMSTS_DVI_1_ACT;
            msWriteByteMask(0x003119, 0x02, 0x06);
            msWriteByteMask(0x003219, 0x02, 0x06);
            msWriteByteMask(0x00310F, 0xFF, 0xFF);
            msWriteByteMask(0x00320F, 0xFF, 0xFF);
            msWriteByteMask(0x00310F, 0x00, 0xFF);
            msWriteByteMask(0x00320F, 0x00, 0xFF);
            bResault = TRUE;
            PM_printData("[HDMI][SCDC5V_Wakeup]ePMSTS_DVI_1_ACT:%d", ePMSTS_DVI_1_ACT);
        }
#endif
#endif
    }

#if ENABLE_USB_TYPEC
    if( (bResault == FALSE) // check after all wakeup events
      &&(sPMInfo.sPMConfig.eTYPECmode == ePMTYPEC_AUTO) )
    {
        if( ((sPMInfo.sPMConfig.bTYPEC_enable==0) && (glSYS_Attach[0] || glSYS_Attach[1]))
          ||((sPMInfo.sPMConfig.bTYPEC_enable==1) && !(glSYS_Attach[0] || glSYS_Attach[1]) && (g_u8TypeCPMDBCCnt==0)) )
        {
            ucWakeupStatus = ePMSTS_TYPECAUTO_CHG;
            bResault = TRUE;
        #if (CHIP_ID == CHIP_MT9701)
            if(sPMInfo.sPMConfig.bTYPEC_enable==0)
            {
                glSYS_Attach[0] = 0; // PD side CC open.
                glSYS_Attach[1] = 0; // PD side CC open.
            }
        #endif
        }
#if ((CHIP_ID == CHIP_MT9701) && U3_REDRV_PM_MODE)
		else if(g_bRedrU3Status != g_bHubU3Status)
		{
			ucWakeupStatus = ePMSTS_U3STATE_CHG;
			bResault = TRUE;
		}
#endif

    }
#endif

    return  bResault;
}

//**************************************************************************
//  [Function Name]:
//                  _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber()
//  [Description]
//					_msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(DPRx_ID dprx_id)
{
    if(dprx_id == DPRx_ID_MAX)
    {
        return Input_Nothing;
    }

    switch(dprx_id)
    {
        case DPRx_ID_0:
            return Input_Displayport_C1;
            break;

        case DPRx_ID_1:
            return Input_Displayport_C2;
            break;

#if(CHIP_ID == CHIP_MT9701)

        case DPRx_ID_2:
            if(Input_Displayport_C3 != Input_Nothing)
            {
                return Input_Displayport_C3;
            }
            else
            {
                return Input_UsbTypeC_C3;
            }
            break;
#else

        case DPRx_ID_2:
            return Input_Displayport_C3;
            break;

        case DPRx_ID_3:
            if(Input_Displayport_C4 != Input_Nothing)
            {
                return Input_Displayport_C4;
            }
            else
            {
                return Input_UsbTypeC_C4;
            }
            break;
#endif

        default:
            return Input_Nothing;
            break;
    }

    return Input_Nothing;
}

BYTE msPM_GetActiveDPPort(BYTE u8InputPortl)
{
#if (ENABLE_DP_INPUT == 0x1)
    DPRx_ID dprx_id;
    BYTE XDATA ubSystemPortNum[DPRx_ID_MAX];
    BYTE ubDPFixPort_Detect = 0;
    WORD usRegOffsetAuxByID = 0;
    ePM_MCCS_WAKEUP dprx_mccs_wakeup = ePM_MCCS_MAX;
	DPRx_AUX_ID dprx_aux_id = DPRx_AUX_ID_MAX;

#if(CHIP_ID == CHIP_MT9701)
    ubSystemPortNum[DPRx_ID_0] = Input_Displayport_C1;
    ubSystemPortNum[DPRx_ID_1] = Input_Displayport_C2;

    if(Input_Displayport_C3 != Input_Nothing)
    {
        ubSystemPortNum[DPRx_ID_2] = Input_Displayport_C3;
    }
    else if(Input_UsbTypeC_C3 != Input_Nothing)
    {
        ubSystemPortNum[DPRx_ID_2] = Input_UsbTypeC_C3;
    }
#else
    ubSystemPortNum[DPRx_ID_0] = Input_Displayport_C1;
    ubSystemPortNum[DPRx_ID_1] = Input_Displayport_C2;
    ubSystemPortNum[DPRx_ID_2] = Input_Displayport_C3;

    if(Input_Displayport_C4 != Input_Nothing)
    {
        ubSystemPortNum[DPRx_ID_3] = Input_Displayport_C4;
    }
    else if(Input_UsbTypeC_C4 != Input_Nothing)
    {
        ubSystemPortNum[DPRx_ID_3] = Input_UsbTypeC_C4;
    }
#endif

#if 1
    if(UserPrefInputPriorityType != Input_Priority_Auto)
    {
        if(SrcInputType == Input_Displayport)
        {
            ubDPFixPort_Detect = BIT0;
        }
        else if(SrcInputType == Input_Displayport2)
        {
            ubDPFixPort_Detect = BIT1;
        }
        else if((SrcInputType == Input_Displayport3) || (SrcInputType == Input_UsbTypeC3))
        {
            ubDPFixPort_Detect = BIT2;
        }
        else if((SrcInputType == Input_Displayport4) || (SrcInputType == Input_UsbTypeC4))
        {
            ubDPFixPort_Detect = BIT3;
        }
        else
        {
            ubDPFixPort_Detect = 0;
        }
    }
#endif
    for(dprx_id = DPRx_ID_0; dprx_id < DPRx_ID_MAX; dprx_id++)
    {
#if 1
        if((UserPrefInputPriorityType != Input_Priority_Auto) && (!((ubDPFixPort_Detect) & (BIT0 << dprx_id))))
        {
            continue;
        }
#endif
		dprx_aux_id = _mdrv_DPRx_MSCHIP_DPRxID2AuxID(dprx_id);

        #if (CHIP_ID == CHIP_MT9701)
		usRegOffsetAuxByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
        #else
        usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
        #endif

		if(dprx_aux_id != DPRx_AUX_ID_MAX)
		{
			mhal_DPRx_MuxSelect(dprx_id, dprx_aux_id);
		}
		else
		{
			continue;
		}

        if(ubSystemPortNum[dprx_id] == Input_Nothing)
        {
            continue; // Not DP port or Type C port case
        }
        else
        {
            if(mapi_DPRx_AUX_N_Level(ubSystemPortNum[dprx_id]) == 0x0) // Make sure Aux N as Low in sleep mode
            {
                glAuxN_L2H_Event |= (BIT0 << dprx_id);
            }
        }

        if(u8InputPortl & (BIT0 << dprx_id)) // DP port start from Input_Digital
        {
            if(sPMInfo.ucPMMode == ePM_STANDBY)
            {
                //==================== DP Aux_N wake-up detect ====================
                #if(CHIP_ID == CHIP_MT9701)
                if(ubSystemPortNum[dprx_id] == Input_UsbTypeC_C3)
                #else
                if(ubSystemPortNum[dprx_id] == Input_UsbTypeC_C4)
                #endif
                {
                }
                else
                {
                    if((mapi_DPRx_AUX_N_Level(ubSystemPortNum[dprx_id]) == 0x1) && ((glAuxN_L2H_Event & (BIT0 << dprx_id)) != 0x0))
                    {
                        mapi_DPRx_HPDControl(ubSystemPortNum[dprx_id], FALSE);

                        glAuxN_L2H_Event = 0x0;

                        PM_printData("\r\n**AUXN wake up at DPRx ID %d", dprx_id);
                        PM_printData("\r\n**AUXN wake up at system ID %d", _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id));

                        return _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id);
                    }
                }

                #if(ENABLE_USB_TYPEC == 1)
                #if(CHIP_ID == CHIP_MT9701)
                if(ubSystemPortNum[dprx_id] == Input_UsbTypeC_C3)
                #else
                if(ubSystemPortNum[dprx_id] == Input_UsbTypeC_C4)
                #endif
                {
                    if((glSYS_TypeC_PinAssign[glSYS_TypeC_PortIndex] == BIT2) || (glSYS_TypeC_PinAssign[glSYS_TypeC_PortIndex] == BIT4)) // u3rt sqh clk setting decide by PD51 when PinD
                    {
                        msWriteByteMask(REG_152678, 0, BIT0|BIT7); // Make sure u3rt [0] sqh clk set to FRO and [7] close deglitch to prevent Ugreen Pin E noise when source side cable unplug
                        msWriteByteMask(REG_15267B, 0, BIT0|BIT7);
                    }

                    if((glSYS_TypeC_PinAssign[glSYS_TypeC_PortIndex] == BIT2) || (glSYS_TypeC_PinAssign[glSYS_TypeC_PortIndex] == BIT3)) // PinC/D
                    {
                        if(glSYS_CC_Pin[glSYS_TypeC_PortIndex] == 0x1) // Flip plug
                        {
                            #if(CHIP_ID == CHIP_MT9701)
                            if(msReadByte(REG_1526EF) & BIT6) // MT9701 dpc sqh tie0 use u3rt sqh
                            #else
                            if(mhal_DPRx_TypeC_CheckSquelch(dprx_id, glSYS_TypeC_PinAssign[glSYS_TypeC_PortIndex], glSYS_CC_Pin[glSYS_TypeC_PortIndex]) == TRUE)
                            #endif
                            {
                                if(glDP_SQDebounce[dprx_id] == 0)
                                {
                                    PM_printData("\r\n**TPC PM PHY PinCD Flip SQ wake up at DPRx ID %d", dprx_id);
                                    PM_printData("\r\n**TPC PM PHY PinCD Flip SQ wake up at system ID %d", _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id));

                                    return _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id);
                                }
                            }
                            else
                            {
                                glDP_SQDebounce[dprx_id] = 50; // Unit: 1ms
                            }
                        }
                        else if(glSYS_CC_Pin[glSYS_TypeC_PortIndex] == 0x0)
                        {
                            #if(CHIP_ID == CHIP_MT9701)
                            if(msReadByte(REG_1526EF) & BIT7) // MT9701 dpc sqh tie0 use u3rt sqh
                            #else
                            if(mhal_DPRx_TypeC_CheckSquelch(dprx_id, glSYS_TypeC_PinAssign[glSYS_TypeC_PortIndex], glSYS_CC_Pin[glSYS_TypeC_PortIndex]) == TRUE)
                            #endif
                            {
                                if(glDP_SQDebounce[dprx_id] == 0)
                                {
                                    PM_printData("\r\n**TPC PM PHY PinCD non-Flip SQ wake up at DPRx ID %d", dprx_id);
                                    PM_printData("\r\n**TPC PM PHY PinCD non-Flip SQ wake up at system ID %d", _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id));

                                    return _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id);
                                }
                            }
                            else
                            {
                                glDP_SQDebounce[dprx_id] = 50; // Unit: 1ms
                            }
                        }
                    }
                    else if(glSYS_TypeC_PinAssign[glSYS_TypeC_PortIndex] == BIT4) // PinE
                    {
                        if(glSYS_CC_Pin[glSYS_TypeC_PortIndex] == 0x1) // Flip plug
                        {
                            #if(CHIP_ID == CHIP_MT9701)
                            if(msReadByte(REG_1526EF) & BIT5) // MT9701 dpc sqh tie0 use u3rt sqh
                            #else
                            if(mhal_DPRx_TypeC_CheckSquelch(dprx_id, glSYS_TypeC_PinAssign[glSYS_TypeC_PortIndex], glSYS_CC_Pin[glSYS_TypeC_PortIndex]) == TRUE)
                            #endif
                            {
                                if(glDP_SQDebounce[dprx_id] == 0)
                                {
                                    PM_printData("\r\n**TPC PM PHY PinE Flip SQ wake up at DPRx ID %d", dprx_id);
                                    PM_printData("\r\n**TPC PM PHY PinE Flip SQ wake up at system ID %d", _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id));

                                    return _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id);
                                }
                            }
                            else
                            {
                                glDP_SQDebounce[dprx_id] = 50; // Unit: 1ms
                            }
                        }
                        else if(glSYS_CC_Pin[glSYS_TypeC_PortIndex] == 0x0)
                        {
                            #if(CHIP_ID == CHIP_MT9701)
                            if(msReadByte(REG_1526EF) & BIT4) // MT9701 dpc sqh tie0 use u3rt sqh
                            #else
                            if(mhal_DPRx_TypeC_CheckSquelch(dprx_id, glSYS_TypeC_PinAssign[glSYS_TypeC_PortIndex], glSYS_CC_Pin[glSYS_TypeC_PortIndex]) == TRUE)
                            #endif
                            {
                                if(glDP_SQDebounce[dprx_id] == 0)
                                {
                                    PM_printData("\r\n**TPC PM PHY PinE non-Flip SQ wake up at DPRx ID %d", dprx_id);
                                    PM_printData("\r\n**TPC PM PHY PinE non-Flip SQ wake up at system ID %d", _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id));

                                    return _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id);
                                }
                            }
                            else
                            {
                                glDP_SQDebounce[dprx_id] = 50; // Unit: 1ms
                            }
                        }
                    }
                }
                else
                #endif

                //==================== DP squelch wake-up detect ====================
                #if(SET_DPC_PORT_AS_DP_PORT == 1)
                #if(CHIP_ID == CHIP_MT9701)
                msWriteByteMask(REG_152678, 0, BIT0|BIT7); // Make sure u3rt [0] sqh clk set to FRO and [7] close deglitch to prevent Ugreen Pin E noise when source side cable unplug
                msWriteByteMask(REG_15267B, 0, BIT0|BIT7);

                if(dprx_id == DPRx_ID_2)
                {
    				if(msReadByte(REG_1526EF) & BIT7) // MT9701 dpc sqh tie0 use u3rt sqh
                    {
                        if(glDP_SQDebounce[dprx_id] == 0)
                        {
                            PM_printData("\r\n**SQ wake up at DPRx ID %d", dprx_id);
                            PM_printData("\r\n**SQ wake up at system ID %d", _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id));

                            return _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id);
                        }
                    }
                    else
                    {
                        glDP_SQDebounce[dprx_id] = 50; // Unit: 1ms
                    }
                }
                #else
                if(dprx_id == DPRx_ID_3)
                {
                    if(msReadByte(REG_DPRX_PHY_PM_0B_L) &  BIT3)
                    {
                        PM_printData("\r\n**SQ wake up at DPRx ID %d", dprx_id);
                        PM_printData("\r\n**SQ wake up at system ID %d", _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id));

                        return _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id);
                    }
                    else
                    {
                        glDP_SQDebounce[dprx_id] = 50; // Unit: 1ms
                    }
                }
                #endif
                else
                #endif
#if 1
                {
    				if(mhal_DPRx_CheckSquelch(dprx_id) == TRUE)
                    {
                        if(glDP_SQDebounce[dprx_id] == 0)
                        {
                            PM_printData("\r\n**SQ wake up at DPRx ID %d", dprx_id);
                            PM_printData("\r\n**SQ wake up at system ID %d", _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id));

                            return _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id);
                        }
                    }
                    else
                    {
                        glDP_SQDebounce[dprx_id] = 50; // Unit: 1ms
                    }
                }
#endif
                //==================== DPCD 600h wake-up detect ====================
                #if(CHIP_ID == CHIP_MT9701)
                /* for verification
    			if((mapi_DPRx_AUX_N_Level(ubSystemPortNum[dprx_id]) == TRUE) &&
                   ((msReadByte(REG_DPRX_AUX_PM0_39_L + usRegOffsetAuxByID) & (BIT2|BIT1|BIT0)) == 0x1))
                {
                    PM_printData("\r\n**DPCD 600h wake up at DPRx ID %d", dprx_id);
                    PM_printData("\r\n**DPCD 600h wake up at system ID %d", _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id));

                    return _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id);
                }
    			*/
                #endif
            }
            //==================== DP MCCS wake-up detect ====================
            dprx_mccs_wakeup = mapi_DPRx_CheckMCCSWakeUpXDATAProgrammableDPCD(ubSystemPortNum[dprx_id]);

            #if (CHIP_ID == CHIP_MT9701)
            if((msReadByte(REG_DPRX_AUX_PM0_4D_L + usRegOffsetAuxByID) & BIT1) == 0 || dprx_mccs_wakeup != ePM_MCCS_MAX)
            {
                if ((msReadByte(REG_DPRX_AUX_PM0_2D_L + usRegOffsetAuxByID) & BIT1) != 0 || dprx_mccs_wakeup == ePM_MCCS_D5) // D6 05
                {
                    ucWakeupStatus = ePMSTS_MCCS05_ACT;
                    msWriteByte(REG_DPRX_AUX_PM0_2D_L + usRegOffsetAuxByID, (msReadByte(REG_DPRX_AUX_PM0_2D_L + usRegOffsetAuxByID)));  // Clear status

                    PM_printData("\r\n**DDCCI ACT D6 05 wake up at DPRx ID %d", dprx_id);
                    PM_printData("\r\n**DDCCI ACT D6 05 wake up at system ID %d", _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id));

                    return _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id);
                }
                else if((msReadByte(REG_DPRX_AUX_PM0_2D_L + usRegOffsetAuxByID) & BIT2) != 0 || dprx_mccs_wakeup == ePM_MCCS_D4) // D6 04
                {
                    ucWakeupStatus = ePMSTS_MCCS04_ACT;
                    msWriteByte(REG_DPRX_AUX_PM0_2D_L + usRegOffsetAuxByID, (msReadByte(REG_DPRX_AUX_PM0_2D_L + usRegOffsetAuxByID)));  // Clear status

                    PM_printData("\r\n**DDCCI ACT D6 04 wake up at DPRx ID %d", dprx_id);
                    PM_printData("\r\n**DDCCI ACT D6 04 wake up at system ID %d", _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id));

                    return _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id);
                }
                else if((msReadByte(REG_DPRX_AUX_PM0_2D_L + usRegOffsetAuxByID) & BIT0) != 0 || dprx_mccs_wakeup == ePM_MCCS_D1) // D6 01
                {
                    ucWakeupStatus = ePMSTS_MCCS01_ACT;
                    msWriteByte(REG_DPRX_AUX_PM0_2D_L + usRegOffsetAuxByID, (msReadByte(REG_DPRX_AUX_PM0_2D_L + usRegOffsetAuxByID)));  // Clear status

                    PM_printData("\r\n**DDCCI ACT D6 01 wake up at DPRx ID %d", dprx_id);
                    PM_printData("\r\n**DDCCI ACT D6 01 wake up at system ID %d", _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id));

                    return _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id);
                }
            }
            #else

			if((msReadByte(REG_DPRX_AUX_4D_L + usRegOffsetAuxByID) & BIT1) == 0 || dprx_mccs_wakeup != ePM_MCCS_MAX)
            {
                if ((msReadByte(REG_DPRX_AUX_2D_L + usRegOffsetAuxByID) & BIT1) != 0 || dprx_mccs_wakeup == ePM_MCCS_D5) // D6 05
                {
                    ucWakeupStatus = ePMSTS_MCCS05_ACT;
                    msWriteByte(REG_DPRX_AUX_2D_L + usRegOffsetAuxByID, (msReadByte(REG_DPRX_AUX_2D_L + usRegOffsetAuxByID)));  // Clear status

                    PM_printData("\r\n**DDCCI ACT D6 05 wake up at DPRx ID %d", dprx_id);
                    PM_printData("\r\n**DDCCI ACT D6 05 wake up at system ID %d", _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id));

                    return _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id);
                }
                else if((msReadByte(REG_DPRX_AUX_2D_L + usRegOffsetAuxByID) & BIT2) != 0 || dprx_mccs_wakeup == ePM_MCCS_D4) // D6 04
                {
                    ucWakeupStatus = ePMSTS_MCCS04_ACT;
                    msWriteByte(REG_DPRX_AUX_2D_L + usRegOffsetAuxByID, (msReadByte(REG_DPRX_AUX_2D_L + usRegOffsetAuxByID)));  // Clear status

                    PM_printData("\r\n**DDCCI ACT D6 04 wake up at DPRx ID %d", dprx_id);
                    PM_printData("\r\n**DDCCI ACT D6 04 wake up at system ID %d", _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id));

                    return _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id);
                }
                else if((msReadByte(REG_DPRX_AUX_2D_L + usRegOffsetAuxByID) & BIT0) != 0  || dprx_mccs_wakeup == ePM_MCCS_D1) // D6 01
                {
                    ucWakeupStatus = ePMSTS_MCCS01_ACT;
                    msWriteByte(REG_DPRX_AUX_2D_L + usRegOffsetAuxByID, (msReadByte(REG_DPRX_AUX_2D_L + usRegOffsetAuxByID)));  // Clear status

                    PM_printData("\r\n**DDCCI ACT D6 01 wake up at DPRx ID %d", dprx_id);
                    PM_printData("\r\n**DDCCI ACT D6 01 wake up at system ID %d", _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id));

                    return _msPM_mdrv_DPRx_MSCHIP_DPRxID2PortNumber(dprx_id);
                }
            }
            #endif
        }
    }
#else
    u8InputPortl = u8InputPortl;
#endif

    return 0xFF;
}

void msPM_SetPMClockLive(BYTE clk_sel)
{
    if (clk_sel == CLK_LIVE_RCOSC_4M)
    {
        msWriteByteMask(REG_PM_BC, BIT6, BIT6); //clk Live select ROSC
        #if (ENABLE_WATCH_DOG)
        SetWDTClk(CLK_LIVE_RCOSC_4M);
        #endif
    }
    else
    {
        #if (ENABLE_WATCH_DOG)
        SetWDTClk(CLK_LIVE_XTAL);
        #endif
        msWriteByteMask(REG_PM_BC, 0, BIT6); //clk Live select XTAL
    }
#if EXT_TIMER0_1MS
    Init_ExtTimerCount();
#endif
}

void msPM_SlowClkDetEvntSet(void)
{
    u32FIQ_DET = u32IRQ_DET = u32FIQ_POL = u32IRQ_POL = 0;

#if ENABLE_MSTV_UART_DEBUG //ENABLE_DEBUG
    u32FIQ_DET |= FIQ_HKR2_0_UART_DET;
#endif

#if ENABLE_USB_TYPEC
    u32FIQ_DET |= (FIQ_PD51_1_UART_DET|FIQ_PD51_0_UART_DET);
#if ENABLE_INTERNAL_CC
    u32IRQ_DET |= (IRQ_PD_CC_SWITCH_INT|IRQ_U31_WAKEUP_INT_UP);
    u32IRQ_POL |= IRQ_PD_CC_SWITCH_INT;
#else
#if(CHIP_ID == CHIP_MT9701)
    u32IRQ_DET |= IRQ_U31_WAKEUP_INT_UP;
#else
    u32IRQ_DET |= (IRQ_GPIO_C_WKUP_2|IRQ_U31_WAKEUP_INT_UP);
#endif
    u32IRQ_POL |= IRQ_GPIO_C_WKUP_2;
#endif
#endif

}

#if(CHIP_ID == CHIP_MT9700)
void msPM_SlowClkDetEnable(BOOL bEnable, BYTE u8SlowClkDiv)
{
    g_bSlowClkDetEnabled = bEnable;

    //mask all
    msWrite4Byte(REG_000366, 0xFFFFFFFF);
    msWrite4Byte(REG_00036A, 0xFFFFFFFF);

    mcuSetSlowClkSpeed(u8SlowClkDiv);

    if(bEnable)
    {
        //polarity
        msWrite4Byte(REG_00036E, u32FIQ_POL);
        msWrite4Byte(REG_0003DC, u32IRQ_POL);
        //mask
        msWrite4Byte(REG_000366, (u32FIQ_DET ^ 0xFFFFFFFF));
        msWrite4Byte(REG_00036A, (u32IRQ_DET ^ 0xFFFFFFFF));
    }
}
#else
void msPM_SlowClkDetEnable(BOOL bEnable, BYTE u8SlowClkDiv, BYTE u8SlowClkDivPD51)
{
    g_bSlowClkDetEnabled = bEnable;

    //mask all
    msWrite4Byte(REG_000366, 0xFFFFFFFF);
    msWrite4Byte(REG_00036A, 0xFFFFFFFF);

    mcuSetSlowClkSpeed(u8SlowClkDiv, u8SlowClkDivPD51);

    if(bEnable)
    {
        //polarity
        msWrite4Byte(REG_00036E, u32FIQ_POL);
        msWrite4Byte(REG_0003DC, u32IRQ_POL);
        //mask
        msWrite4Byte(REG_000366, (u32FIQ_DET ^ 0xFFFFFFFF));
        msWrite4Byte(REG_00036A, (u32IRQ_DET ^ 0xFFFFFFFF));
    }
}
#endif

void msPM_SlowClkDetForceDisable(BOOL bDisable)
{
    if(bDisable)
    {
        g_bSlowClkDetForceDisabled = TRUE;

    #if(CHIP_ID == CHIP_MT9700)
        msPM_SlowClkDetEnable(FALSE, HK_R2_RECOVERY_CLOCK);
    #else
        msPM_SlowClkDetEnable(FALSE, HK_R2_RECOVERY_CLOCK, PD_51_RECOVERY_CLOCK);
    #endif
    }
    else
    {
        g_bSlowClkDetForceDisabled = FALSE;
        g_u8SlowClkTimer = SLOW_CLK_TIMER;
    }
}

BOOL msPM_SlowClkChkReady(void)
{
    BYTE XDATA u8Rtn = FALSE;
    BYTE XDATA u8IsCCIdle = FALSE;

    if(g_u8SlowClkTimer == 0)
    {
        g_u8SlowClkTimer = SLOW_CLK_TIMER;
#if ENABLE_USB_TYPEC
        drvmbx_send_CC_IDLE_RCMD(&u8IsCCIdle);
        if(u8IsCCIdle)
           u8Rtn = TRUE;
#else
        u8IsCCIdle = u8IsCCIdle;
        u8Rtn = TRUE;
#endif
    }

    return u8Rtn;
}

#if(CHIP_ID == CHIP_MT9700)
void msPM_SlowClkHandler(void)
{
    BYTE u8SlowClkDiv;

    if(!g_bSlowClkDetForceDisabled && (PM_CLOCK == SLOW_CLK) && (HK_R2_SLOW_CLOCK != HK_R2_RECOVERY_CLOCK))
    {
        if(!g_bSlowClkDetEnabled && msPM_SlowClkChkReady())
        {
            msPM_SlowClkDetEnable(TRUE, HK_R2_SLOW_CLOCK);
        }

        //check if clock is recovered or not
        u8SlowClkDiv = (msReadByte(REG_000372) & 0x1F);
        if(g_bSlowClkDetEnabled && (u8SlowClkDiv == HK_R2_RECOVERY_CLOCK))
        {
            msPM_SlowClkDetEnable(FALSE, HK_R2_RECOVERY_CLOCK);
            g_u8SlowClkTimer = SLOW_CLK_TIMER;
        }

    }
}
#else
void msPM_SlowClkHandler(void)
{
    BYTE u8SlowClkDiv;
    BYTE u8SlowClkDivPD51;

    static BOOL bClkDivEn = ((HK_R2_SLOW_CLOCK != HK_R2_RECOVERY_CLOCK)?TRUE:FALSE);
#if ENABLE_USB_TYPEC
    static BOOL bClkDivEnPD51 = ((PD_51_SLOW_CLOCK != PD_51_RECOVERY_CLOCK)?TRUE:FALSE);
#else
    static BOOL bClkDivEnPD51 = FALSE;
#endif

    if((!g_bSlowClkDetForceDisabled) && (PM_CLOCK == SLOW_CLK) && (bClkDivEn || bClkDivEnPD51))
    {
        if((!g_bSlowClkDetEnabled) && msPM_SlowClkChkReady())
        {
            msPM_SlowClkDetEnable(TRUE, HK_R2_SLOW_CLOCK, PD_51_SLOW_CLOCK);
        }

        u8SlowClkDiv = (msReadByte(REG_000372) & 0x1F);
        u8SlowClkDivPD51 = ((msReadByte(REG_0003BB) & 0xF8) >> 3);

        //check if clock is recovered or not
        if(g_bSlowClkDetEnabled && ((bClkDivEn && (u8SlowClkDiv == HK_R2_RECOVERY_CLOCK)) || (bClkDivEnPD51 && (u8SlowClkDivPD51 == PD_51_RECOVERY_CLOCK))))
        {
            msPM_SlowClkDetEnable(FALSE, HK_R2_RECOVERY_CLOCK, PD_51_RECOVERY_CLOCK);
            g_u8SlowClkTimer = SLOW_CLK_TIMER;
        }
    }
}
#endif

void msPM_SlowClkInit(void)
{
    msWriteByteMask(REG_000372, (HK_R2_RECOVERY_CLOCK << 7), BIT7); //set HK_R2 clock recovery freq

#if(CHIP_ID == CHIP_MT9700)
    msWriteByteMask(REG_0003BA, HK_R2_RECOVERY_CLOCK, BIT0); //set PD51 clock recovery freq as HK_R2
#else
    msWriteByteMask(REG_0003BA, PD_51_RECOVERY_CLOCK, BIT0); //set PD51 clock recovery freq
#endif

    mcuSetSystemSpeed(SPEED_SLOWCLK_MODE);
    msPM_SlowClkDetEvntSet();
    g_u8SlowClkTimer = SLOW_CLK_TIMER;
}

void msPM_SetPMClock(BYTE clk_sel)
{
    if (clk_sel == XTAL)
    {
        PM_printMsg(" \r\n***PM Clock =>XTAL ");
        mcuSetSystemSpeed(SPEED_XTAL_MODE );
        msPM_SetPMClockLive(CLK_LIVE_XTAL);
        msAPIPWMClkSel(PWM_CLK_XTAL);

        msWriteByteMask(REG_PM_76, _BIT2, _BIT3|_BIT2); //select OCP SAR clk XTAL
        msWriteByteMask(REG_PM_90, _BIT3, _BIT5|_BIT4|_BIT3|_BIT2); //select SAR and DDC clk XTAL
        msWriteByteMask(REG_PM_B0, 0x00, _BIT7|_BIT6|_BIT3|_BIT2);  //select SCDC0 and SCDC1 clk XTAL
        msWriteByteMask(REG_PM_B1, 0x00, _BIT7|_BIT6|_BIT3|_BIT2); //select SCDC2 and MIIC0 clk XTAL
        msWriteByteMask(REG_PM_B2, 0x00, _BIT7|_BIT6|_BIT3|_BIT2); //select MIIC1 and Hotplug0 clk XTAL

        #if (ENABLE_DP_INPUT == 0x1)
        mapi_DPRx_SetAuxPMClock(TRUE); //select aux clk XTAL
        #endif
    }
    else
    {
        if (clk_sel == RCOSC)
        {
            PM_printMsg(" \r\n***PM Clock =>FRO 12M ");
            mcuSetSystemSpeed(SPEED_12MHZ_MODE);
        }
        else //slow clock
        {
            PM_printMsg(" \r\n***PM Clock =>SLOW CLOCK ");
            msPM_SlowClkInit();
        }

#if((CHIP_ID == CHIP_MT9701) && ENABLE_USB_TYPEC)
        if(!sPMInfo.sPMConfig.bTYPEC_enable || g_bRedrU3Status)
        {
            msPM_SetPMClockLive(CLK_LIVE_RCOSC_4M);
        }
#else
        msPM_SetPMClockLive(CLK_LIVE_RCOSC_4M);
#endif
        msAPIPWMClkSel(PWM_CLK_FRO_12M);
        msWriteByteMask(REG_PM_76, _BIT3, _BIT3|_BIT2); //select OCP SAR clk FRO12
        msWriteByteMask(REG_PM_90, _BIT4|_BIT2, _BIT5|_BIT4|_BIT3|_BIT2); //select SAR and DDC clk FRO12
        msWriteByteMask(REG_PM_B0, _BIT6|_BIT2, _BIT7|_BIT6|_BIT3|_BIT2); //select SCDC0 and SCDC1 clk FRO12
        msWriteByteMask(REG_PM_B1, _BIT6|_BIT2, _BIT7|_BIT6|_BIT3|_BIT2); //select SCDC2 and MIIC0 clk FRO12
        msWriteByteMask(REG_PM_B2, _BIT6|_BIT2, _BIT7|_BIT6|_BIT3|_BIT2); //select MIIC1 and Hotplug0 clk FRO12

        #if (ENABLE_DP_INPUT == 0x1)
        mapi_DPRx_SetAuxPMClock(FALSE); //select aux clk FRO12
        #endif
    }

    Delay1ms(1);

    //old_msWriteByteMask(REG_1ED1,_BIT4|_BIT0 , _BIT4|_BIT0); // power down LPLL and MPLL
    //old_msWriteByteMask(SC0_F0, 0, BIT1|BIT0);
    //old_msWriteByte(SC0_F1, 0x01);
    //old_msWriteByte(SC0_F1, 0x00);
    //old_msWriteByteMask(SC0_F0, BIT4|BIT1|BIT0, BIT4|BIT1|BIT0);
}

void msPM_SetPMMode(void)
{
#if (CHIP_ID == CHIP_MT9701)
    BOOL bCorePowerCtrl = TRUE;
#endif

#if PM_SUPPORT_ADC_TIME_SHARE
    msWriteBit(REG_PM_FF, 1, BIT2);
    msWrite2ByteMask(REG_DVI_CHEN, 0xE0|DVI_CTRL_PERIOD, 0xE7);
#else
    msWriteBit(REG_PM_FF,0,BIT2);
    msWrite2ByteMask(REG_DVI_CHEN, 0x00|DVI_CTRL_PERIOD, 0xE7);
#endif

    //---HVSync
    if (sPMInfo.sPMConfig.bHVSync_enable)
    {
        msPM_EnableHVSyncDetect(TRUE);
    }
    else
    {
        msPM_EnableHVSyncDetect(FALSE);
    }

    //---SOG
    if (sPMInfo.sPMConfig.bSOG_enable)
    {
        msPM_EnableSOGDetect(TRUE);
    }
    else
    {
        msPM_EnableSOGDetect(FALSE);
    }

    //---DP
    if (sPMInfo.sPMConfig.bDP_enable)
    {
        msPM_EnableDPDetect(TRUE);
    }
    else
    {
        msPM_EnableDPDetect(FALSE);
    }

    //---DP/TypeC R-term
    if((sPMInfo.ucPMMode==ePM_POWEROFF))
    {
        msDrvUsbSwitchRtermEnDP(FALSE); // R-term for DP
    }
    else
    {
        msDrvUsbSwitchRtermEnDP(TRUE); // R-term for DP
    }

#if 0
    if( (!sPMInfo.sPMConfig.bTYPEC_enable) && (sPMInfo.sPMConfig.eTYPECmode!=ePMTYPEC_PDON) )
    {
        msDrvUsbSwitchRtermEnTypeC(FALSE); // R-term for TypeC
    }
#endif

    //---DVI
#if (ENABLE_DVI || ENABLE_HDMI)
    #if ENABLE_HDMI_BCHErrorIRQ
    msAPI_combo_HDMIRx_EnableHDMIRx_PKT_ParsrIRQ(FALSE);
    #endif
    #if (PM_SUPPORT_WAKEUP_DVI)
    if (sPMInfo.sPMConfig.bDVI_enable)
    {
        mdrv_hdmiRx_PMEnableDVIDetect(HDMI_PM_STANDBY);
    }
    else
    {
        mdrv_hdmiRx_PMEnableDVIDetect(HDMI_PM_POWEROFF);
    }
    #endif
#endif

#if ENABLE_MSBHK
    //---Power Key
    if (sPMInfo.ucPMMode == ePM_POWEROFF)
    {
        msAPI_MSBHK_SetPMMode(POWER_DCOFF, 0);
        while (((Key_GetKeypadStatus()^KeypadMask)&KeypadMask)==KEY_POWER); //Wait power key released,avoid power off and the power on again
    }
    else
    {
        msAPI_MSBHK_SetPMMode(POWER_DPMS, 0xFFFF); // detect all ports
        //TBD fix port
    }
#else
    //---Power Key
    if (sPMInfo.ucPMMode == ePM_POWEROFF)
    {
        while (((Key_GetKeypadStatus()^KeypadMask)&KeypadMask)==KEY_POWER); //Wait power key released,avoid power off and the power on again
    }
    else if (sPMInfo.ucPMMode == ePM_POWEROFF_0W)
    {
        PM_printMsg("Enter 0W Mode");
        msWriteByte(REG_00038B, 0xA5);
        msWriteByte(REG_00038B, 0x58);
        while(1);
    }
#endif

    //---GPIO
    if (sPMInfo.sPMConfig.bGPIO_enable)
    {
        msPM_EnableGPIODetect(TRUE);
    }
    else
    {
        msPM_EnableGPIODetect(FALSE);
    }

    //---SAR
    if (sPMInfo.sPMConfig.bSAR_enable)
    {
        msPM_EnableSARDetect(TRUE);
    }
    else
    {
        msPM_EnableSARDetect(FALSE);
    }

    //---MCCS
    if (sPMInfo.sPMConfig.bMCCS_enable)
    {
        msPM_EnableMCCSDetect(TRUE);
    }
    else
    {
        msPM_EnableMCCSDetect(FALSE);
    }

    msPM_MCCSReset();

    //---EDID
    if (sPMInfo.sPMConfig.bEDID_enable)
    {
        msPM_Enable_EDID_READ(TRUE);
    }
    else
    {
        msPM_Enable_EDID_READ(FALSE);
    }

    if (sPMInfo.ucPMMode == ePM_POWERON) return;

    msPM_PassWord(TRUE);
    msPM_PowerDownMacro();
    msPM_OutputTriState();
    msPM_ClearStatus(FALSE);

#if ((CHIP_ID == CHIP_MT9701) && U3_REDRV_PM_MODE)
    if(g_bRedrU3Status)
    {
        msSetUSBEnterU3State(glSYS_CC_Pin[1]);
        PM_printMsg("\r\nEnter USB U3 mode");
    }
#endif

    if (sPMInfo.sPMConfig.bMCUSleep)
    {
        msWriteByteMask(REG_PM_A6, 0, _BIT0);
        msPM_SetPMClock(RCOSC);
        msWriteByteMask(REG_PM_AA, _BIT1, _BIT1);
        msWriteByteMask(REG_PM_A6, _BIT7, _BIT7|_BIT6); // Enable HW PM mode (Enable mcu gating)
        //Set MCU sleep mode
        PM_printMsg("Enter HW PM Mode");
        CACHE_DISABLE();
        msWriteByte(REG_PM_80, 0xA5);
        msWriteByte(REG_PM_80, 0x56);
    }
    else
    {
        msPM_SetPMClock(PM_CLOCK);

        msWriteByteMask(REG_PM_A6, 0, _BIT0);
        msWriteByteMask(REG_PM_A6, _BIT0, _BIT0); //Enter SW PM Mode
#if ENABLE_USB_TYPEC
    #if(CHIP_ID == CHIP_MT9701)
        if((PM_CLOCK == XTAL) || (sPMInfo.sPMConfig.bTYPEC_enable && (!g_bRedrU3Status)))
    #else
        if(ENABLE_INTERNAL_CC || (PM_CLOCK == XTAL) || (sPMInfo.sPMConfig.bTYPEC_enable && (!IsInU3State())))
    #endif
#else
        if(PM_CLOCK == XTAL)
#endif
        {
            msWriteByteMask(REG_PM_A6, BIT2, BIT2);
        }
        else
        {
            msWriteByteMask(REG_PM_A6, BIT2, BIT2);
            ForceDelay1ms(1);
            msWriteByteMask(REG_PM_A6, 0, BIT2); //disable XTAL
        }
#if (CHIP_ID == CHIP_MT9701)

#if ENABLE_USB_TYPEC
		if((!sPMInfo.sPMConfig.bTYPEC_enable) || ((glSYS_TypeC_PinAssign[1] == BIT4) || (glSYS_TypeC_PinAssign[1] == BIT2))) // pinC, pinE
		{
            bCorePowerCtrl = FALSE;
		}
#if U3_REDRV_PM_MODE
		else if(g_bRedrU3Status)
		{
            bCorePowerCtrl = FALSE;
		}
#endif
		else
		{
			bCorePowerCtrl = TRUE;
		}
#else
        bCorePowerCtrl = FALSE;
#endif

	if (!bCorePowerCtrl)
	{
		WORD u16TimeoutCnt = 0x7FF;

        Disable_PM_Power(); //disable core power
		while((msReadByte(REG_00039C) & BIT1)&&(u16TimeoutCnt--)); //wait core power off with time out count
#if ENABLE_USB_TYPEC
        g_u8Usb2Alive = 0;
#endif
#if ENABLE_SECU_R2
        g_u8SecuR2Alive = 0;
#endif
#if U3_REDRV_PM_MODE
        if(g_bRedrU3Status)
            MsOS_EnableInterrupt(E_INT_PM_IRQ_U3_WAKEUP);
#endif
	}

#else //(CHIP_ID == CHIP_MT9700)
#if 0
        if(!sPMInfo.sPMConfig.bTYPEC_enable)
#endif
        {
            Disable_PM_Power(); //disable core power
#if ENABLE_INTERNAL_CC
            ForceDelay1ms(2);
            Enable_PM_Power();
            WAIT_CORE_POWER_READY(2000);
#endif
            #if ENABLE_SECU_R2
            g_u8SecuR2Alive = 0;
            #endif
        }
#endif
        PM_printMsg("Enter SW PM Mode");
        sPMInfo.ePMState = ePM_WAIT_EVENT;

        #if PM_CABLE_DETECT_USE_SAR
        sPMInfo.bCABLE_SAR_VALUE = CABLE_DET_SAR;
        #endif
    }
}

BYTE msPM_Reset(void)
{
    BYTE ucStatus = ePMSTS_INVAID;

    #if 0//Enable_Cache
    CACHE_ENABLE();//MCU_EnableCache(_ENABLE);
    #endif
    msPM_PassWord(FALSE);

    msPM_Init();
    msPM_ClearStatus(TRUE);

    msPM_MCCSReset();
    PM_printData("Enter PM ePM_PowerON mode!",0);

    return ucStatus;
}

#define RECHECK_COUNT     50
Bool IsHVSyncActive(BYTE count)
{
    Bool u8Rlt=FALSE;
    WORD u16InputValue;
    BYTE check_cnt;

    for(check_cnt=0; check_cnt<count; check_cnt++)
    {
#if CHIP_ID == CHIP_TSUMC
        TMDS_Config_For_PM();
#endif
        ForceDelay1ms(20);

        u16InputValue = SC0_READ_HPEROID();//old_msRead2Byte(SC0_E4) & 0x1FFF;
        if (u16InputValue == 0x1FFF || u16InputValue < 20)
           continue;
        if(labs((DWORD)u16InputValue-(SC0_READ_HPEROID()))>4)
            continue;

        u16InputValue = SC0_READ_VTOTAL();
        if (u16InputValue == 0xFFF || u16InputValue < 200)
            continue;

        u8Rlt=TRUE;


        break;
    }
    return u8Rlt;
}

#if MS_VGA_SOG_EN
extern BYTE GetVSyncWidth(void);
#endif

Bool msPM_CheckAnalogSyncActive(void)
{
    Bool u8Rlt=TRUE;
    BYTE SyncTYPE = 0;
    #if MS_VGA_SOG_EN
    BYTE ucStatus;
    BYTE w_VSyncWidth;
    #endif

    msWriteByte(SC00_F3, 0x00);                         //for SOG+video timing can't wake up(no v count)
    msWriteByte(SC00_F0,0x00);
    msWriteByte(REG_102E02,0x00);
    msWriteByte(REG_002508,0x00);
    msWriteByte(REG_002509,0x00);
    //old_msWriteByteMask(REG_1EDC, 0, _BIT5|_BIT4);    //Power On MPLL 216MHz/432MHz
    //old_msWriteByteMask(REG_1ED1,0, _BIT4|_BIT0);     //Power on LPLL and MPLL
    old_msWriteByteMask(REG_1E47, _BIT2, 0x0F);         //Select clk source
    mStar_SetupInputPort();
    old_msWriteByteMask(REG_1E3E, 0, BIT0);             //Sky110702 Modify for SOG can't Get V Sync

    for (SyncTYPE = 0; SyncTYPE < 2; SyncTYPE++)
    {
#if ENABLE_VGA_INPUT
        mStar_SetAnalogInputPort(SyncTYPE);
#endif
        msWriteByteMask(SC00_ED,0,BIT5);
        u8Rlt = IsHVSyncActive(RECHECK_COUNT);
        if(u8Rlt)
        {
            #if MS_VGA_SOG_EN
                    ucStatus = SC0_READ_SYNC_STATUS(); //old_msReadByte(SC0_E1);
            if (ucStatus&(SOGP_B) && ucStatus&(CSP_B))
            {
                if (ucStatus & SOGD_B)
                {
                    w_VSyncWidth = GetVSyncWidth();
                    if (w_VSyncWidth > 15 || w_VSyncWidth == 0) //check SOG pulse width if bigger than 15  //djyang 20080605
                    {
                        return FALSE;
                    }
                    msWriteByteMask(SC00_ED,1,BIT5);
                }
                return TRUE;
            }
            #endif

            return u8Rlt;
        }
    }

    return FALSE;
}

extern void mStar_SetIPMux(IP_MUX eIPMux);
Bool msPM_CheckDVISyncActive(void)
{
    Bool u8Rlt = FALSE;
#if (ENABLE_HDMI || ENABLE_DVI)//ENABLE_SYNC_CHECK_AGAIN
    BYTE u8Combo_InputPort = 0xFF;
    Bool bHPDFlag = FALSE;

    if(ucWakeupStatus == ePMSTS_DVI_0_ACT)
    {
        u8Combo_InputPort = HDMI_INPUT_PORT0;
    }
    else if(ucWakeupStatus == ePMSTS_DVI_1_ACT)
    {
        u8Combo_InputPort = HDMI_INPUT_PORT1;
    }
    else if(ucWakeupStatus == ePMSTS_DVI_2_ACT)
    {
        u8Combo_InputPort = HDMI_INPUT_PORT2;
    }
    else if(ucWakeupStatus == ePMSTS_DVI_3_ACT)
    {
        u8Combo_InputPort = HDMI_INPUT_PORT3;
    }

#if ENABLE_HPD_REPLACE_MODE
    bHPDFlag = mdrv_hdmiRx_Initial(1);
    //bHPDFlag do nothing here.
#else
    bHPDFlag = mdrv_hdmiRx_Initial(1);
    msAPI_combo_HDMIRx_ClockRtermControl(SrcInputType, TRUE);
    msAPI_combo_HDMIRx_DataRtermControl(SrcInputType, TRUE);
#endif
    mdrv_hdmiRx_SetPowerDown(SrcInputType, FALSE);  // power down once when AC on

    if(CURRENT_INPUT_IS_DVI())//( SrcInputType == Input_Digital )
    {
        PM_printMsg("msPM_CheckDVI");
        mdrv_hdmi_SetupInputPort(SrcInputType);
        mStar_SetIPMux(IP_MUX_DVI);
    }
    else if(CURRENT_INPUT_IS_HDMI())//(SrcInputType == Input_HDMI)
    {
        PM_printMsg("msPM_CheckHDMI");
        mdrv_hdmi_SetupInputPort(SrcInputType);
        mStar_SetIPMux(IP_MUX_HDMI);
    }

    u8Rlt = IsHVSyncActive(RECHECK_COUNT);
#if PM_DEBUG
    //PM_printData("PM_A2:%x", old_msReadByte(REG_PM_A2));
    //PM_printData("PM_A4:%x", old_msReadByte(REG_PM_A4));
    if(u8Rlt == TRUE)
    {
        PM_printData("msPM_CheckDVISyncActive:%d", SrcInputType);
    }
#endif
#endif
    return u8Rlt;

}

#if   0//  sahdow_Need test
Bool msPM_CheckDPMCCSActive(void)
{
    Bool bExitPM = TRUE;
     if(msRegs[REG_1FE6]&_BIT7)   //MCCS Interrupt
        {
                //DDC2BI_DP() ;
            if(DDCBuffer[2]==PowerMode)
            {
                if((DDCBuffer[4] == 0x01)
                &&(sPMInfo.ucPMMode == ePM_POWEROFF || sPMInfo.ucPMMode == ePM_STANDBY))
                    ;
                else  if((DDCBuffer[4] == 0x05) && (sPMInfo.ucPMMode == ePM_STANDBY))
                {
                    msPM_Reset();
                    PowerOffSystem();
                    bExitPM=FALSE;
                }
                else  if((DDCBuffer[4] == 0x05) && (sPMInfo.ucPMMode == ePM_POWEROFF))
                {

                }
                else
                    bExitPM=FALSE;
            }

        }

 return bExitPM;


}
#endif

Bool msPM_CheckMCCSActive(void)
{
    Bool bExitPM = FALSE;

    if(((ucWakeupStatus == ePMSTS_MCCS01_ACT) && (sPMInfo.ucPMMode == ePM_POWEROFF || sPMInfo.ucPMMode == ePM_STANDBY))
      ||((ucWakeupStatus == ePMSTS_MCCS05_ACT) && (sPMInfo.ucPMMode == ePM_POWEROFF)))
    {
        bExitPM = TRUE;
    }
    else  if((ucWakeupStatus == ePMSTS_MCCS05_ACT) && (sPMInfo.ucPMMode == ePM_STANDBY))
    {
        msPM_Reset();
        PowerOffSystem();
    }

    return bExitPM;
}

Bool msPM_CheckPowerKeyActive(void)
{
    Bool bExitPM = FALSE;

    #if PM_POWERkEY_GETVALUE
    {
        if(sPMInfo.ucPMMode == ePM_STANDBY)
        {
            msPM_Reset();
            PowerOffSystem();
        }
        else if(sPMInfo.ucPMMode == ePM_POWEROFF)
        {
            bExitPM = TRUE;
        }
    }
    #else

    WORD ucStatus_GPIO;

    ucStatus_GPIO = msRead2Byte(REG_PMGPIO_STS);

    if (ucStatus_GPIO & (PM_POWERKEY_INT)) //power key
    {
        if(sPMInfo.ucPMMode == ePM_STANDBY)
        {
            //msPM_Reset();
            PowerOffSystem();
        }
        else if(sPMInfo.ucPMMode == ePM_POWEROFF)
            bExitPM = TRUE;
    }
    #endif

    return bExitPM;
}

BOOL msPM_Checkagain(void)
{
    Bool bExitPM = TRUE;

    //volatile WORD ucStatus_GPIO;
    //old_msWriteByte(SC0_F1, 0);
    //old_msWriteByte(REG_1ED1, 0x4);           //MPLL function enable
    //old_msWriteByte(SC0_F0, 0);

    Enable_PM_Power();                          //enable core power
    msWriteByteMask(REG_PM_A6, _BIT2, _BIT2);//SW XTAL on
    msWriteByteMask(REG_PM_DA, _BIT0, _BIT0);//Xtal clk enable

    ForceDelay1ms(10);
    WAIT_CORE_POWER_READY(2000);
    msPM_SetPMClock(XTAL);
    msWriteByteMask(REG_PM_A6, 0, _BIT7|_BIT0); // PM SW power down mode(MCU no die mode)
    //old_msWriteByte(SC0_06, 0x00);            //power up idclk and odclk

    #if 0//Enable_Cache
    CACHE_ENABLE();//MCU_EnableCache(_ENABLE);
    #endif

    if(ucWakeupStatus == ePMSTS_VGA_ACT)
    {
        old_msWriteByteMask(REG_1ED1, 0, BIT0);
        if(msPM_CheckAnalogSyncActive())
            bExitPM = TRUE;//wakeup
        else
            bExitPM = FALSE;
        old_msWriteByteMask(REG_1ED1, BIT0, BIT0);
    }

#if (PM_SUPPORT_WAKEUP_DVI)
    else if(ucWakeupStatus == ePMSTS_DVI_0_ACT||ucWakeupStatus ==ePMSTS_DVI_1_ACT || ucWakeupStatus == ePMSTS_DVI_2_ACT||ucWakeupStatus ==ePMSTS_DVI_3_ACT)
    {
   #if(CHIP_ID == CHIP_MT9700)
        if(msPM_CheckDVISyncActive())
            bExitPM = TRUE;//wakeup
       else
            bExitPM = FALSE;
   #else
            bExitPM = TRUE;
   #endif
    }
#endif

    else if(ucWakeupStatus == ePMSTS_GPIO_ACT)
    {
        #if !(PM_POWERkEY_GETVALUE)
        if(msPM_CheckPowerKeyActive())
        {
            bExitPM = TRUE; //wakeup
        }
        else
        #endif

        #if PM_CABLEDETECT_USE_GPIO && ENABLE_DP_INPUT
        if (!(DP_CABLE_NODET))
        {
            SrcInputType = Input_Displayport;
            mStar_SetupInputPort();
            bExitPM = TRUE;
        }
        else
        #endif

        {
            bExitPM = FALSE;
        }
    }
    else if(ucWakeupStatus == ePMSTS_SAR_ACT)
    {
        bExitPM = TRUE;
    }
    else if(ucWakeupStatus == ePMSTS_MCCS05_ACT||ucWakeupStatus == ePMSTS_MCCS04_ACT||ucWakeupStatus == ePMSTS_MCCS01_ACT)
    {
        if(msPM_CheckMCCSActive())
            bExitPM = TRUE;//wakeup
        else
            bExitPM = FALSE;
    }

#if ENABLE_DP_INPUT&&PM_SUPPORT_WAKEUP_DP
    else if(ucWakeupStatus == ePMSTS_DP_ACT)
    {
     //mStar_SetupInputPort();   // For HDCP wake up issue. Aksv not ready in Rx
    #if  0 //  sahdow_Need test
    if(msPM_CheckDPMCCSActive())
        bExitPM = TRUE;
    else
        bExitPM = FALSE;
    #else
    bExitPM = TRUE;
    #endif
    }
#endif

#if (PM_POWERkEY_GETVALUE)
    else if(ucWakeupStatus == ePMSTS_POWERGPIO_ACT)
    {
        if(msPM_CheckPowerKeyActive())
        {
            bExitPM = TRUE;//wakeup
            KeypadButton=BTN_Repeat;      //110914 Rick modified
        }
        else
            bExitPM = FALSE;
    }
#endif

#if (PM_CABLE_DETECT_USE_SAR)
    else if(ucWakeupStatus ==ePMSTS_CABLESAR_ACT)
    {
        if(CABLE_DET_SAR<sPMInfo.bCABLE_SAR_VALUE)
            bExitPM = TRUE;
        else
            bExitPM = FALSE;
    }
#endif
#if ENABLE_USB_TYPEC
    else if(ucWakeupStatus == ePMSTS_TYPECAUTO_CHG)
    {
        bExitPM = FALSE;
    }
#if ((CHIP_ID == CHIP_MT9701) && U3_REDRV_PM_MODE)
	else if(ucWakeupStatus == ePMSTS_U3STATE_CHG)
	{
		bExitPM = FALSE;
		PM_printMsg("ePMSTS_U3STATE_CHG.....");
	}
#endif
#endif
#if ENABLE_CEC
    else if(ucWakeupStatus == ePMSTS_CEC_ACT)
    {
        bExitPM = TRUE;
    }
#endif
    else
    {
        bExitPM = FALSE;
    }
    //bExitPM = TRUE;//wakeup

    return bExitPM;
}

void msPM_ScanADCCh(void)
{
    BYTE channel;

    if(!ModeDetectCounter)
    {
        channel = msReadByte(REG_0003FF)&0x03;
#if MS_VGA_SOG_EN
        channel = (++channel) >= 3 ? 0 : channel;
#else
	    channel = (++channel) >= 2 ? 0 : channel;
#endif
        msWriteByteMask(REG_0003FF,channel,0x03);
        ModeDetectCounter = 20;
    }
}

void msPM_WaitingEvent(void)
{
    //msPM_ClearStatus(TRUE); // 120209 coding reserved. PM sw mode, after disable xtal, it should not reset PM
    PM_printMsg("msPM_WaitingEvent.....");

    while(1)
    {
        msPM_SlowClkHandler();

        #if ENABLE_WATCH_DOG
        ClearWDT();
        #endif
        
        #if Enable_PanelHandler
        g_bPanelHandlerEn = 1;
        Power_PanelCtrlHandler();
        #endif

        Main_SlowTimerHandler();

#ifdef UseVGACableReadWriteAllPortsEDID
        //if ((!g_bServiceEDIDUnLock && !WriteDeviceFlag) && (!ProductModeFlag))
#endif
        {
            if(SyncLossState() && IsCableNotConnected())
            {
                hw_ClrDDC_WP();
            }
            else
            {
                hw_SetDDC_WP();
            }
        }
#if ENABLE_USB_TYPEC
#if(!ENABLE_INTERNAL_CC)
        if((PM_CLOCK != XTAL) && (msReadByte(REG_PM_A6)&BIT2) && IsInU3State())
        {
            msWriteByteMask(REG_PM_A6, 0, BIT2); //disable XTAL
        }
#endif
        MBX_handler();
#endif

#if UART1
    #if ENABLE_MSBHK
        MSBHK_RxHandler();
    #else
        UART1_Handler();
    #endif
#endif

#if ENABLE_MSTV_UART_DEBUG
    if ( msAPI_UART_DecodeCommand() )
    {
        continue; // stop main loop for debug
    }
#endif

#if(CHIP_ID == CHIP_MT9700)
        msPM_ScanADCCh();
#endif

#if ENABLE_DP_INPUT
        mapi_DPRx_MSCHIP_DP_PMHandler();
#endif


		DDC2Bi_CommandHandler();

        if(msPM_GetPMStatus())
        {
            BootTimeStamp_Clr();
            BootTimeStamp_Set(PM_INIT_TS, 0, TRUE); // pm init timestamp 0
        	PM_printData("[Break msPM_WaitingEvent]bRunToolFlag:%d\n",bRunToolFlag);
            break;
        }
    }

    PM_printData("\r\nPM Wakeup Event1 (%d) !", ucWakeupStatus);
}

void msPM_UpdatePowerStatus(void)
{
    #define IS_DVI_ACT()    ((ucWakeupStatus == ePMSTS_DVI_0_ACT) || (ucWakeupStatus == ePMSTS_DVI_1_ACT) || (ucWakeupStatus == ePMSTS_DVI_2_ACT) || (ucWakeupStatus == ePMSTS_DVI_3_ACT))

    if((ucWakeupStatus == ePMSTS_POWERGPIO_ACT) || ((ucWakeupStatus == ePMSTS_MCCS01_ACT) && !PowerOnFlag) || (ucWakeupStatus == ePMSTS_MCCS05_ACT)) // DC ON
    {
        PM_printMsg("[PowerStatus] DC ON \n");
        g_eBootStatus = eBOOT_STATUS_DCON;
    }
    #if (ENABLE_DP_INPUT == 0x1)
    else if((ucWakeupStatus == ePMSTS_DP_ACT) || IS_DVI_ACT())
    #else
    else if(IS_DVI_ACT())
    #endif
    {
        PM_printMsg("[PowerStatus] DPMS WAKE UP, Check Again\n");
        g_eBootStatus = eBOOT_STATUS_CHECKAGAINON;
    }
    else //DPMS Wake up
    {
        PM_printMsg("[PowerStatus] DPMS WAKE UP \n");
        g_eBootStatus = eBOOT_STATUS_DPMSON;
    }

    #undef IS_DVI_ACT
}

void msPM_TypeCConfig(void)
{
#if ENABLE_USB_TYPEC
    if((sPMInfo.sPMConfig.eTYPECmode == ePMTYPEC_OFF)
        ||(sPMInfo.sPMConfig.eTYPECmode == ePMTYPEC_PDON))
    {
        sPMInfo.sPMConfig.bTYPEC_enable = 0;
    }
    else if(sPMInfo.sPMConfig.eTYPECmode == ePMTYPEC_ON)
    {
        sPMInfo.sPMConfig.bTYPEC_enable = 1;
    }
    else if(sPMInfo.sPMConfig.eTYPECmode == ePMTYPEC_AUTO)
    {
        if(ucWakeupStatus == ePMSTS_TYPECAUTO_CHG)
        {
            sPMInfo.sPMConfig.bTYPEC_enable = !sPMInfo.sPMConfig.bTYPEC_enable;
            if(sPMInfo.sPMConfig.bTYPEC_enable)
            {
                //mcuSetSystemSpeed(SPEED_NORMAL_MODE );
                #if(CHIP_ID == CHIP_MT9701)
                #if U3_REDRV_PM_MODE
            	msSetUSBExitU3State();
				g_bHubU3Status = 0;
				g_bRedrU3Status = g_bHubU3Status;
                #endif
                msDrvUsbSwitchInit();
                msDrvUsb2Init(); // For usb2 device(e.g. BB ... etc)
                #endif

                // miu clock toggle for OTG bank access MT9700 TBD
                //msWriteByte(REG_120F06, BIT2);
                //msWriteByte(REG_120F06, 0x00);

                #if (ENABLE_DP_INPUT == 0x1)
                mapi_DPRx_SetPowerDownControl(Input_UsbTypeC_C4, sPMInfo.ucPMMode);
                #endif
                msInitClockGating(TRUE);
                g_u8TypeCPMDBCCnt = 3; // At least 3s to re-enter ePMSTS_TYPECAUTO_CHG
            }
        }
#if ((CHIP_ID == CHIP_MT9701) && U3_REDRV_PM_MODE)
		else if(ucWakeupStatus == ePMSTS_U3STATE_CHG)
		{
			g_bRedrU3Status = g_bHubU3Status;

			if(g_bRedrU3Status)
				g_u8TypeCPMDBCCnt = 0;
		}
#endif
        else
        {
#if ((CHIP_ID == CHIP_MT9701) && U3_REDRV_PM_MODE)
            g_bRedrU3Status = g_bHubU3Status;
#endif
            sPMInfo.sPMConfig.bTYPEC_enable = (glSYS_Attach[0] || glSYS_Attach[1])?1:0;
            if(sPMInfo.sPMConfig.bTYPEC_enable)
            {
                #if ENABLE_SECU_R2
                if(g_u8SecuR2Alive) // After the 1st PowerDown the IMI might be lack of miu clock.
                {
                    msAPI_combo_SECU_PowerDown(TRUE);
                    g_u8SecuR2Alive = 0;
                }
                #endif
                #if (ENABLE_DP_INPUT == 0x1)
                mapi_DPRx_SetPowerDownControl(Input_UsbTypeC_C4, ePM_POWEROFF);
                #endif
                msInitClockGating(TRUE);
            }
        }
    }
    else
    {
        PM_printMsg("[PM] Error !!! Invalid TypeC Mode !!!!! \n");
    }
#else
    sPMInfo.sPMConfig.bTYPEC_enable = 0;
#endif
}

#if ENABLE_USB_TYPEC
// Maps to PD51 TypeC Mode
BYTE msPM_TransTypeCMode(BYTE u8TypeCMode)
{
#if(CHIP_ID == CHIP_MT9701)
    if(u8TypeCMode==ePMTYPEC_AUTO)
#if U3_REDRV_PM_MODE
        u8TypeCMode += ((g_bRedrU3Status || (glSYS_TypeC_PinAssign[1] == BIT4) || (glSYS_TypeC_PinAssign[1] == BIT2) )? 0 : sPMInfo.sPMConfig.bTYPEC_enable);
#else
        u8TypeCMode += sPMInfo.sPMConfig.bTYPEC_enable;
#endif
    else
#else // MT9700
#if 0
    if(u8TypeCMode==ePMTYPEC_AUTO)
        u8TypeCMode += sPMInfo.sPMConfig.bTYPEC_enable;
    else
#endif
#endif
    if(u8TypeCMode==ePMTYPEC_PDON)
        u8TypeCMode += 1;

    return u8TypeCMode;
}
#endif

void  msPM_Handler(void)
{
    switch(sPMInfo.ePMState)
    {
        case ePM_ENTER_PM:
            #if (ENABLE_WATCH_DOG)
            Init_WDT(_DISABLE);
            #endif
            msPM_TypeCConfig();
            #if ENABLE_CEC
            drv_CEC_ConfigWakeUp();

            #endif
            #if ENABLE_USB_TYPEC
            #if ENABLE_VBUS_DETECT_SAR
            drvmbx_send_BuBst_Config_CMD(BIT1|BIT0); // disable buck-boost when detach or power sink
            #endif
            drvmbx_send_PM_Status_Update_CMD(sPMInfo.ucPMMode, msPM_TransTypeCMode(sPMInfo.sPMConfig.eTYPECmode));
            drvmbx_send_Upll_Off(1);
            #endif
            #if ENABLE_HK_CODE_ON_PSRAM
            {
                void (*FPTR_PSRAM_END_ADDR_PM)(void) = msDrvGetPSramEndInPM;
                WORD wEndAddr = (WORD)(FPTR_PSRAM_END_ADDR_PM) - 1;

                if(wEndAddr > MCU_ON_PSRAM_END_ADDR)
                    wEndAddr = MCU_ON_PSRAM_END_ADDR;

                msDrvMcuSetCodeRangeOnPSram(MCU_ON_PSRAM_START_ADDR, wEndAddr);
            }
            #endif

            msPM_SetPMMode();
        break;

        case ePM_WAIT_EVENT:
            msPM_WaitingEvent();
            if(msPM_Checkagain())
            {
                BootTimeStamp_Set(PM_INIT_TS, 1, TRUE); // pm init timestamp 1
                sPMInfo.ePMState = ePM_EXIT_PM;
            }
            else
                sPMInfo.ePMState = ePM_ENTER_PM;
        break;

        case ePM_EXIT_PM:
            #if ENABLE_USB_TYPEC
            drvmbx_send_Upll_Off(0); // MUST execute before msPM_PowerUpMacro()
            #endif
            #if ENABLE_SUPER_RESOLUTION
            SRmodeContent = SRMODE_Nums;
            #endif
            GammaContent = GAMA_Nums;

            msPM_UpdatePowerStatus();
            msPM_Reset();
            BootTimeStamp_Set(PM_INIT_TS, 2, TRUE); // pm init timestamp 2
            msPM_PowerUpMacro();

            BootTimeStamp_Set(PM_INIT_TS, 3, TRUE); // pm init timestamp 3
            if(PowerOnFlag) //wakeup from standby
            {
                Power_PowerOnSystem();
                if((ucWakeupStatus == ePMSTS_VGA_ACT)
                    ||(ucWakeupStatus == ePMSTS_DVI_0_ACT)
                    ||(ucWakeupStatus == ePMSTS_DVI_1_ACT)
                    ||(ucWakeupStatus == ePMSTS_DVI_2_ACT)
                    ||(ucWakeupStatus ==ePMSTS_DVI_3_ACT))
                    SrcFlags &= ~SyncLoss;
            }
            else //wakeup from dc off
            {
                PowerOnSystem();
            }
            sPMInfo.ePMState = ePM_IDLE;
            g_SwitchSec = DEF_FORCE_DPMS;

            BootTimeStamp_Set(PM_INIT_TS, 4 ,TRUE); // pm init timestamp 4
            #if (ENABLE_WATCH_DOG)
            Init_WDT(_ENABLE);
            ClearWDT();
            #endif
            #if ENABLE_USB_TYPEC
            #if ENABLE_VBUS_DETECT_SAR
            drvmbx_send_BuBst_Config_CMD(0);
            #endif
            drvmbx_send_PM_Status_Update_CMD(sPMInfo.ucPMMode, msPM_TransTypeCMode(sPMInfo.sPMConfig.eTYPECmode));
            #endif
            #if ENABLE_HK_CODE_ON_PSRAM
            msDrvMcuSetCodeRangeOnPSram(MCU_ON_PSRAM_START_ADDR, MCU_ON_PSRAM_END_ADDR);
            #endif
            #if ENABLE_MSBHK
            msAPI_MSBHK_SetPMMode(POWER_ON, 0);
            #endif
            BootTimeStamp_Set(PM_INIT_TS, 5 ,TRUE); // pm init timestamp 5
            InitFirstPowerOnVariables();
        break;

        case ePM_IDLE:
        default:
        break;
    }
}

//**************************************************************************
//  [Function Name]:
//                  msPM_StartRCOSCCal()
//  [Description]
//                  msPM_StartRCOSCCal
//  [Arguments]:
//
//  [Return]:
//  RCOSC = XTAL * Counter / 512 => Counter = RCOSC *512/XTAL = 12M*512/12M =>0x200
//  this chip has high resolution calibration function, so the formula will be changed as below
//  RCOSC = XTAL * Counter / 2048 => Counter = RCOSC *2048/XTAL = 12M*2048/12M =>0x800
//**************************************************************************
Bool msPM_StartRCOSCCal(void)
{
    #define RCOSC_HIGH_RESOLUTION_EN  BIT3
    #define RCOSC_COUNTER_MASK  0xFFF
    #define RCOSC_FRO_TUNE  0x7F
    #define WRITE_CAL_VALUE(A)    {(msWriteByteMask(REG_00038A, A,  RCOSC_FRO_TUNE));}

    WORD u16Counter;
    BYTE u8i;

    msWriteByte(REG_0003A6, 0x0E);                                  //[0]reg_sw_pm_mode  //[1]reg_sw_power_fence

    msPM_SetPMClockLive(CLK_LIVE_XTAL);

    WRITE_CAL_VALUE(0x00);
    msWriteByteMask(REG_103A80, RCOSC_HIGH_RESOLUTION_EN ,RCOSC_HIGH_RESOLUTION_EN|BIT0);
    msWriteByteMask(REG_103A80, RCOSC_HIGH_RESOLUTION_EN|BIT0 ,RCOSC_HIGH_RESOLUTION_EN|BIT0);//RCOSC calculate enable
    MDrv_SysDelayUs(5);
    while((msReadByte(REG_103A83) & _BIT4)==0x00);           // one time counter flag
    u16Counter = msRead2Byte(REG_103A82)&RCOSC_COUNTER_MASK; // one time counter report

    if(u16Counter < RCOSC_TARGET)
    {
        u8i = 0;
        do
        {
            WRITE_CAL_VALUE(++u8i);
            msWriteByteMask(REG_103A80, RCOSC_HIGH_RESOLUTION_EN ,RCOSC_HIGH_RESOLUTION_EN|BIT0);
            msWriteByteMask(REG_103A80, RCOSC_HIGH_RESOLUTION_EN|BIT0 ,RCOSC_HIGH_RESOLUTION_EN|BIT0);//RCOSC calculate enable
            MDrv_SysDelayUs(5);
            while((msReadByte(REG_103A83) & _BIT4)==0x00);
            u16Counter = msRead2Byte(REG_103A82)&RCOSC_COUNTER_MASK;

        } while((u8i < RCOSC_FRO_TUNE/2) && (u16Counter < RCOSC_TARGET));
        return (BOOL)(u16Counter >= RCOSC_TARGET);
    }
    else if(u16Counter > RCOSC_TARGET)
    {
        u8i = RCOSC_FRO_TUNE;
        do
        {
            WRITE_CAL_VALUE(--u8i);
            msWriteByteMask(REG_103A80, RCOSC_HIGH_RESOLUTION_EN ,RCOSC_HIGH_RESOLUTION_EN|BIT0);
            msWriteByteMask(REG_103A80, RCOSC_HIGH_RESOLUTION_EN|BIT0 ,RCOSC_HIGH_RESOLUTION_EN|BIT0);//RCOSC calculate enable
            MDrv_SysDelayUs(5);
            while((msReadByte(REG_103A83) & _BIT4)==0x00);
            u16Counter = msRead2Byte(REG_103A82)&RCOSC_COUNTER_MASK;

        } while ((u8i > (RCOSC_FRO_TUNE+1)/2) &&(u16Counter > RCOSC_TARGET));
        return (BOOL)(u16Counter <= RCOSC_TARGET);
    }
    return TRUE;
#undef WRITE_CAL_VALUE
#undef RCOSC_HIGH_RESOLUTION_EN
#undef RCOSC_COUNTER_MASK
}

void msPM_Exit(void)
{
    bPM0WSts = (msReadByte(REG_PM_8A) & BIT7) ? TRUE : FALSE;

    msWriteByte(REG_PM_A6, 0x0E);// BIT3: no use, BIT2: xtal on, BIT1: power fence, BIT0: SW PM mode
    Enable_PM_Power(); //enable core power
    WAIT_CORE_POWER_READY(2000);
    msPM_PassWord(FALSE);
}

Bool msPM_0WStatus(void)
{
    return bPM0WSts;
}
#else
#define PM_DEBUG    0
#endif

#if 0 //(!ENABLE_DEBUG) || (!MS_PM)
BYTE code msPMNullData[] = {0};
void msPMDummy(void)
{
    BYTE xdata i = msPMNullData[0];
    i = i;
}
#endif
