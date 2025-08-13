#include "types.h"
#include "board.h"
#include "Global.h"
#include "Power.h"
#include "Debug.h"
#include "DebugDef.h"
#include "Common.h"
#include "DDC2Bi.H"
#include "ms_reg.h"
#include "Ms_rwreg.h"
#include "UserPref.h"
#include "GPIO_DEF.h"
#include "drvADC.h"
#include "Mcu.h"
#include "dpCommon.h"
#include "MsIRQ.h"
#include "mapi_DPRx.h"
#include "mdrv_DPRx.h"
#include "mhal_DPRx.h"
#include "HdcpHandler.h"
#include "HdcpRx.h"
#include "HdcpMbx.h"
#include "drvIMI.h"
#include "drvHDRCommon.h"

//#include "HdcpIMI.h"
#include "drv_Hdcp_IMI.h"
#include "MsOS.h"
#if ENABLE_UART_PIU
#include "drvUARTPIU.h"
#endif

//#include "halRwreg.h"
#if ENABLE_DAC
#include "drvDAC.h"
#endif
#if ENABLE_DPS
#include "msDPS_Setting.h"
#endif

#if (ENABLE_HDMI || ENABLE_DVI)
#include "mdrv_hdmiRx.h"
#endif

#if ENABLE_CEC_INT
#include "apiCEC.h"
#endif
#include "mapi_eDPTx.h"
#include "mdrv_eDPTx.h"
#include "mhal_eDPTx.h"

#if ENABLE_USB_TYPEC
#include "mailbox.h"
#include "usbsw.h"
#endif
#if ENABLE_MSBHK
#include "msAPI_MSBHK.h"
#endif
#if ENABLE_LOCALDIMMING
#include "msLD_Handler.h"
#endif
#if ENABLE_HDMI_BCHErrorIRQ
#include "ComboApp.h"
#endif

#if MS_PM
extern XDATA ePMStatus ucWakeupStatus;
#endif
extern BYTE xdata u8DCRStepTransCnt; // 081205 dcr test
extern volatile ST_HDCP_HANDLER XDATA g_stHdcpHandler[DEF_HDCP_PORT_NUM];

BYTE XDATA glDPRxShortHPDIsrFlag;

#if (ENABLE_DP_INPUT == 0x1)
extern BYTE XDATA glDP_SQDebounce[DPRx_ID_MAX];
#endif

void DPRXISR(void);
void DPRXISR_PM(void);
void DPRXISR_PM(void);
void DPRx_TimerIsrHandler(void);

#if ENABLE_HDMI
extern ST_HDMI_RX_SYSTEM_PARAMETER stHDMIRxInfo;
void mdrv_hdmiRx_TimerHandler(void);
#endif

#if UseINT
void MuteVideoAndAudio(void)
{
#if 0//ENABLE_HDMI
    if(stHDMIRxInfo.stPollingInfo.bHDMIModeFlag)
        hw_SetMute(); // for changed mode have "po-po"
#endif

    if(!DisplayLogoFlag)
    {
        hw_ClrBlacklit();
    }

    Set_InputTimingChangeFlag();
    SrcFlags |= SyncLoss;
}

void IsrModeChangeCheck(void)
{
    MEM_MSWRITE_BYTE(SC00_00, 0); // Scaler sub bank backup in ISR_R2IRQ_NONPM_IRQ

    if (INT_STATUS_A_ISR((MEM_MSREAD_BYTE(SC00_CE))&(BIT4|BIT5|BIT6|BIT7)) || (INT_STATUS_B_ISR((MEM_MSREAD_BYTE(SC00_CF))&(BIT2))))
    {
#if ENABLE_DEBUG_TIMING_CHANGE_POINT
        if(INT_STATUS_A_ISR(BIT7))
            SetTimingChangeFirstPoint(TimingChange_ISR_Vchange);
        else if(INT_STATUS_A_ISR(BIT6))
            SetTimingChangeFirstPoint(TimingChange_ISR_Hchange);
        else if(INT_STATUS_A_ISR(BIT5))
            SetTimingChangeFirstPoint(TimingChange_ISR_Vdisappear);
        else if(INT_STATUS_A_ISR(BIT4))
            SetTimingChangeFirstPoint(TimingChange_ISR_Hdisappear);
        else if(INT_STATUS_B_ISR(BIT2))
            SetTimingChangeFirstPoint(TimingChange_ISR_CLKchange);
#endif

        INT_SCALER_A_ISR(0); //disable INT
        INT_SCALER_B_ISR(0); //disable INT
        INT_STATUS_CLEAR_ISR();

        if (!SyncLossState())
        {
            MuteVideoAndAudio();
        }
    }
    else if (INT_STATUS_A_ISR((MEM_MSREAD_BYTE(SC00_CE))&BIT2))
    {
        MEM_MSWRITE_BYTE_MASK ( SC00_CE, 0 ,BIT2 ); //disable INT
        MEM_MSWRITE_BYTE_MASK ( SC00_CC, 0 ,BIT2 ); //clear status

        g_bOutputVsyncIsrFlag = 1;

        MEM_MSWRITE_BYTE_MASK ( SC00_CE, BIT2 ,BIT2); //enable INT
    }
    else if (INT_STATUS_A_ISR((MEM_MSREAD_BYTE(SC00_CE))&BIT3))
    {
        MEM_MSWRITE_BYTE_MASK ( SC00_CE, 0 ,BIT3 ); //disable INT
        MEM_MSWRITE_BYTE_MASK ( SC00_CC, 0 ,BIT3 ); //clear status
        #if CHIP_ID == CHIP_MT9701 
#if ((ENABLE_DOLBY_HDR || ENABLE_HDR_AUTODETECT) && COMBO_TMDS_PACKET_UPDATED_BY_VSYNC)
        msAPI_combo_IPGetPktReceived_Partial(SrcInputType);
        drvHDRVsyncUpdatePacket(MAIN_WINDOW);
#endif
        if(g_bEnableIsrDisableFlock && (!InputTimingChangeFlag))
        {
            msDrvSetExtTimerCnt(2000);
            MsOS_EnableInterrupt(E_INT_PM_FIQ_EXTIMER0);
            msDrvEnableExtTimer(TRUE);
        }
        #endif
        MEM_MSWRITE_BYTE_MASK ( SC00_CE, BIT3 ,BIT3); //enable INT
    }

}
#endif
#if 0//PANEL_EDP for chip verification

void eDPTx_HPDISR(void)
{


    if((msRegs(REG_EDP_TX0_P0_53_L)&(BIT0|BIT1|BIT2)))
    {
        printMsg("HPD_P0_IRQ");
        if((msRegs(REG_EDP_TX0_P0_53_L)&(BIT2)))
        {
            printMsg("HPD_P0_IRQ_Event");
            msRegs(REG_EDP_TX0_P0_56_L) = (msRegs(REG_EDP_TX0_P0_56_L)|(BIT2));
            msRegs(REG_EDP_TX0_P0_56_L) = (msRegs(REG_EDP_TX0_P0_56_L)&(~BIT2));
        }
        else if((msRegs(REG_EDP_TX0_P0_53_L)&(BIT0)))
        {
            printMsg("HPD_P0_IRQ_Connect");
            msRegs(REG_EDP_TX0_P0_56_L) = (msRegs(REG_EDP_TX0_P0_56_L)|(BIT0));
            msRegs(REG_EDP_TX0_P0_56_L) = (msRegs(REG_EDP_TX0_P0_56_L)&(~BIT0));
        }
        else
        {
            printMsg("HPD_P0_IRQ_Disconnect");
            msRegs(REG_EDP_TX0_P0_56_L) = (msRegs(REG_EDP_TX0_P0_56_L)|(BIT1));
            msRegs(REG_EDP_TX0_P0_56_L) = (msRegs(REG_EDP_TX0_P0_56_L)&(~BIT1));

        }
    }
    return;
}
void eDPTx_AUXISR(void)
{
        if((msRegs(REG_EDP_AUX_TX_P0_02_L)&(BIT3)))
        {
            printMsg("AUX_P0_IRQ_400us timeout");
            msRegs(REG_EDP_AUX_TX_P0_1A_L) = (msRegs(REG_EDP_AUX_TX_P0_1A_L)|(BIT3));
            msRegs(REG_EDP_AUX_TX_P0_1A_L) = (msRegs(REG_EDP_AUX_TX_P0_1A_L)&(~BIT3));
        }
        if((msRegs(REG_EDP_AUX_TX_P0_02_L)&(BIT4)))
        {
            printMsg("AUX_P0_IRQ_receive DPCD data");
            msRegs(REG_EDP_AUX_TX_P0_1A_L) = (msRegs(REG_EDP_AUX_TX_P0_1A_L)|(BIT4));
            msRegs(REG_EDP_AUX_TX_P0_1A_L) = (msRegs(REG_EDP_AUX_TX_P0_1A_L)&(~BIT4));
        }
        if((msRegs(REG_EDP_AUX_TX_P0_02_L)&(BIT5)))
        {
            printMsg("AUX_P0_IRQ_receive DPCD address");
            msRegs(REG_EDP_AUX_TX_P0_1A_L) = (msRegs(REG_EDP_AUX_TX_P0_1A_L)|(BIT5));
            msRegs(REG_EDP_AUX_TX_P0_1A_L) = (msRegs(REG_EDP_AUX_TX_P0_1A_L)&(~BIT5));
        }
        if((msRegs(REG_EDP_AUX_TX_P0_02_L)&(BIT6)))
        {
            printMsg("AUX_P0_IRQ_receive DPCD command");
            msRegs(REG_EDP_AUX_TX_P0_1A_L) = (msRegs(REG_EDP_AUX_TX_P0_1A_L)|(BIT6));
            msRegs(REG_EDP_AUX_TX_P0_1A_L) = (msRegs(REG_EDP_AUX_TX_P0_1A_L)&(~BIT6));
        }
        if((msRegs(REG_EDP_AUX_TX_P0_02_L)&(BIT7)))
        {
            printMsg("AUX_P0_IRQ_receive MCCS");
            msRegs(REG_EDP_AUX_TX_P0_1A_L) = (msRegs(REG_EDP_AUX_TX_P0_1A_L)|(BIT7));
            msRegs(REG_EDP_AUX_TX_P0_1A_L) = (msRegs(REG_EDP_AUX_TX_P0_1A_L)&(~BIT7));
        }
        if((msRegs(REG_EDP_AUX_TX_P0_02_H)&(BIT0)))
        {
            printMsg("AUX_P0_IRQ_receive EDID");
            msRegs(REG_EDP_AUX_TX_P0_1A_H) = (msRegs(REG_EDP_AUX_TX_P0_1A_H)|(BIT0));
            msRegs(REG_EDP_AUX_TX_P0_1A_H) = (msRegs(REG_EDP_AUX_TX_P0_1A_H)&(~BIT0));
        }
        if((msRegs(REG_EDP_AUX_TX_P0_02_H)&(BIT1)))
        {
            printMsg("AUX_P0_IRQ_receive Native AUX");
            msRegs(REG_EDP_AUX_TX_P0_1A_H) = (msRegs(REG_EDP_AUX_TX_P0_1A_H)|(BIT1));
            msRegs(REG_EDP_AUX_TX_P0_1A_H) = (msRegs(REG_EDP_AUX_TX_P0_1A_H)&(~BIT1));
        }
        if((msRegs(REG_EDP_AUX_TX_P0_02_H)&(BIT4)))
        {
            printMsg("AUX_P0_IRQ_receive AUX signal");
            msRegs(REG_EDP_AUX_TX_P0_1A_H) = (msRegs(REG_EDP_AUX_TX_P0_1A_H)|(BIT4));
            msRegs(REG_EDP_AUX_TX_P0_1A_H) = (msRegs(REG_EDP_AUX_TX_P0_1A_H)&(~BIT4));
        }
        return;
}
#endif
#if 1 //Leo-temp
void ms1MsHandler(void)
{
	#if (ENABLE_DP_INPUT == 0x1)
	DPRx_ID dprx_id = DPRx_ID_0;
	#endif

    ms_Counter--;

     //DPRx_TimerIsrHandler();

     #if ENABLE_HDMI
     mdrv_hdmiRx_TimerHandler();
     #endif
if(TimeOutCounter)
        TimeOutCounter--;

#if (CHIP_ID == CHIP_MT9701) && ENABLE_DAC
    if(u16AudioChkTimer)
        u16AudioChkTimer--;
#endif

#if ENABLE_ARC
    if(sAudioArc.u16CECTimeout)
        sAudioArc.u16CECTimeout--;
#endif

#if ENABLE_MSPI_FLASH_ACCESS
    //gsystem_time_ms++;
#endif

#if (ENABLE_DP_INPUT == 0x1)
	for(dprx_id = DPRx_ID_0; dprx_id < DPRx_ID_MAX; dprx_id++)
	{
		if(glDP_SQDebounce[dprx_id] > 0)
		{
			glDP_SQDebounce[dprx_id]--;
		}
	}
#endif

#if ENABLE_DP_OUTPUT
    if(DPTx_time_ms == 0xFFFFFFFF)
    {
        DPTx_time_ms = 0;
    }
    else
    {
        DPTx_time_ms++;
    }
#endif

#if ENABLE_DEBUG
    if( u8ReSyncCounter )
    {
        u8ReSyncCounter--;
        if( u8ReSyncCounter == 0)
        {
            Clr_RxBusyFlag();
            SIORxIndex = 0;
        }
    }
#endif

#if UART1
    if( u8ReSyncCounter1)
    {
        u8ReSyncCounter1--;
        if( u8ReSyncCounter1 == 0 )
        {
            SIORxIndex1 = 0;
        }
    }
#endif

    if( bDelayFlag )
    {
        if( u16DelayCounter > 0 )
            u16DelayCounter--;

        if( u16DelayCounter == 0 )
            bDelayFlag = 0;
    }

    if( bTimeOutCounterFlag )
    {
        if( u16TimeOutCounter > 0 )
            u16TimeOutCounter--;

        if( u16TimeOutCounter == 0 )
            bTimeOutCounterFlag = 0;
    }

#if ENABLE_TIME_MEASUREMENT   //need enable this if test Timer0
    if( g_u16TimeMeasurementCounter )
        g_u16TimeMeasurementCounter--;
#endif

#if Enable_ReducePanelPowerOnTime
    if(g_u16PanelPowerCounter)
        g_u16PanelPowerCounter--;
#endif

#if Enable_PanelHandler
    if(g_s32PanelCustomCounter)
        g_s32PanelCustomCounter--;
    else
        g_s32PanelCounter++;

    if(!g_bPanelHandlerEn)  // main loop handler is not ready, trigger by minimum counter
        Power_PanelCtrlHandler();
    else                    // main loop handler is ready, trigger by maximum counter
        Power_PanelCtrlIsr();
#endif

#if ENABLE_DAC
    if(u16TimerAudio)
        u16TimerAudio--;
#endif

#ifndef TSUMR2_FPGA
    if(!g_bMcuPMClock) // system runs at normal speed
#endif
    {
#if ENABLE_DLC    
        if( u8DCRStepTransCnt )
            u8DCRStepTransCnt --;
#endif        

        if( bPanelOffOnFlag ) //091013
            if( --u16PanelOffOnCounter == 0 )
                bPanelOffOnFlag = 0;
    }
    #if HDMI_HPD_AC_ON_EARLY_PULL_LOW
    if(g_u16HDMIHPDAcOnEarlyPullLowCnt)
    {
        if( --g_u16HDMIHPDAcOnEarlyPullLowCnt == 0 )
        {
            hw_Set_HdcpHpd();
            hw_Set_HdcpHpd2();
            hw_Set_HdcpHpd3();
        }
    }
    #endif

    if(( ms_Counter % 10 ) == 0 )
    {
        Set_ms10Flag();

        if(( ms_Counter % 50 ) == 0 )
        {
            Set_ms50Flag();

            if(( ms_Counter % 200 ) == 0 )
            {
#if _NEW_SOG_DET_
                if(SyncLossState())
                {
                    NEW_SOG_DET_FLUSH();
                }
#endif

#if _NEW_SOG_WAKEUP_DET_
#if MS_PM
                if(ucWakeupStatus == ePMSTS_NON)
                    NEW_SOG_WAKEUP_FLUSH();
#endif
#endif
            }

            if(( ms_Counter % 500 ) == 0 )
            {
                Set_ms500Flag();

                if( ms_Counter == 0 )        //
                {
                    #if ENABLE_FREESYNC
                    Set_ms1000Flag();
                    #endif
                    Set_SecondFlag();
                    ms_Counter = SystemTick;
                }
            }
        }

    }
#if ENABLE_LOCALDIMMING
    msLD_Isr(FALSE);
#endif

    msAPI_combo_IPTimerIsrHandler();
}

#if CHIP_ID == CHIP_MT9701
void ISR_R2FIQ_PM_EXT_TIMER0(void) // R2 FIQ #0
{
    if(g_bEnableIsrDisableFlock)
    {
        if(!InputTimingChangeFlag)
        {
            MEM_MSWRITE_BYTE_MASK ( SC00_CE, 0 ,BIT3); //disable INT
            msWriteByteMask(SC00_28,BIT3,BIT3);
            msWriteByteMask(REG_LPLL_18,BIT3,BIT3);
            msWriteByteMask(REG_LPLL_1B,BIT1|BIT0,BIT1|BIT0);//enable ivs detect power down
        }
        g_bEnableIsrDisableFlock = 0;
    }
    MsOS_DisableInterrupt(E_INT_PM_FIQ_EXTIMER0);// for one time use
    msDrvEnableExtTimer(FALSE);
}
#endif

void ISR_R2IRQ_AUX_IRQ(void)
{
    mapi_DPRx_PMIsrHandler();

    MsOS_EnableInterrupt(E_INT_PM_IRQ_DP_AUX_ALL);
}

#if(ENABLE_CEC_INT)
void ISR_R2IRQ_PM_CEC_IRQ(void)
{
    api_CEC_ISRHandler();

    MsOS_EnableInterrupt(E_INT_PM_IRQ_CEC0_OUT);
}
#endif
#if ((CHIP_ID == CHIP_MT9701) && (ENABLE_HDMI || ENABLE_DVI))
void ISR_R2IRQ_PM_SCDC_IRQ(void)
{
    mdrv_tmds_SCDC_ISRHandler();
    MsOS_EnableInterrupt(E_INT_PM_IRQ_SCDC);
}
#endif
void ISR_R2IRQ_NONPM_IRQ(void) // Non-PM_IRQ[0]
{
    DWORD u32IRQStatus, u32IRQEXPStatus;
    BYTE ucScBankBK;

    ucScBankBK = MEM_MSREAD_BYTE(SC00_00);
#if (CHIP_ID == CHIP_MT9701)
    u32IRQStatus = msRead4Byte(REG_101938);
    u32IRQEXPStatus = msRead4Byte(REG_10193C);
#else
    u32IRQStatus = msRead4Byte(REG_101978);
    u32IRQEXPStatus = msRead4Byte(REG_10197C);
#endif

#if UseINT
#if ENABLE_HDMI

#if (CHIP_ID == CHIP_MT9701)
    mdrv_hdmiRx_DTOPDEC_IRQ_Mask();
#endif

    if( u32IRQStatus & BIT6 ) // dvi_hdmi_hdcp_int
    {
#if (COMBO_HDCP2_FUNCTION_SUPPORT)
        mdrv_tmds_HDCP2Handler();
#endif

#if (CHIP_ID == CHIP_MT9701)
        mdrv_tmds_TimingChgISR();
#endif

#if ENABLE_HDMI_BCHErrorIRQ
        if(msAPI_combo_HDMIRx_IRQCheckStatus(IRQ_BCHError))
        {
            if(stHDMIRxInfo.stPollingInfo.u16CheckBCHMuteCnt > 0)
            {
                msAPI_combo_HDMIRx_BCHInterrupt();
                if(!SyncLossState())
                {
                    MuteVideoAndAudio();
                }
            }
            else
            {
                mdrv_tmds_HDMIGetBCHErrorStatus(0); // clear bch error
                msAPI_combo_HDMIRx_IRQClearStatus(IRQ_BCHError);    // clear IRQ status
                stHDMIRxInfo.stPollingInfo.u16CheckBCHMuteCnt = 50;
            }
        }
#endif
    }

#if (CHIP_ID == CHIP_MT9701)
    if( u32IRQStatus & BIT11 ) // phy
    {
        mdrv_tmds_PHY_ISRHandler();
    }

        mdrv_hdmiRx_DTOPDEC_IRQ_Unmask();
#endif
        MsOS_EnableInterrupt(E_INT_PM_IRQ_NPM_IRQ_TO_HKR2);
#endif

#if ENABLE_DP_INPUT
    if( u32IRQStatus & BIT4 ) // dp_rx_gp_r2_irq
    {
        mapi_DPRx_RX_IRQ_Handler();
    }

    if( u32IRQStatus & BIT5 ) // dp_rx2_gp_irq
    {
        mapi_DPRx_RX2_IRQ_Handler();
    }
#endif

    if( u32IRQEXPStatus & BIT24 ) // INT_NPM_IRQ_SCINT
    {
        IsrModeChangeCheck();
    }
#if 0//PANEL_EDP for chip verification
    if( u32IRQEXPStatus & BIT0 ) // INT_NPM_IRQ_SCINT
    {
        eDPTx_HPDISR();
    }
    if( u32IRQStatus & BIT30 ) // INT_NPM_IRQ_SCINT
    {
        eDPTx_AUXISR();
    }
#endif
#endif
    MEM_MSWRITE_BYTE(SC00_00, ucScBankBK);
    MsOS_EnableInterrupt(E_INT_PM_IRQ_NPM_IRQ_TO_HKR2);
}

void ISR_R2IRQ_NONPM_FIQ(void) // Non-PM_FIQ[0]
{
#if (CHIP_ID == CHIP_MT9701)
     DWORD u32FIQStatus = msRead4Byte(REG_101918);
    DWORD u32FIQEXPStatus = msRead4Byte(REG_10191C);

    msWrite4Byte(REG_101918, u32FIQStatus); // clear int status
    msWrite4Byte(REG_10191C, u32FIQEXPStatus); // clear int status
#else
    DWORD u32FIQStatus = msRead4Byte(REG_101958);
    DWORD u32FIQEXPStatus = msRead4Byte(REG_10195C);

    msWrite4Byte(REG_101958, u32FIQStatus); // clear int status
    msWrite4Byte(REG_10195C, u32FIQEXPStatus); // clear int status
#endif

    MsOS_EnableInterrupt(E_INT_PM_IRQ_NPM_FIQ_TO_HKR2);
}

#if (CHIP_ID == CHIP_MT9701 && U3_REDRV_PM_MODE)
void ISR_U3_RT(void) //PM_IRQ[28]
{
    bEnterU3State = 1;
	g_bHubU3Status = 1;
    MEM_MSWRITE_BYTE_MASK(REG_1520DB, BIT0, BIT0); // clr INT Status
    MEM_MSWRITE_BYTE_MASK(REG_1523DB, BIT0, BIT0); 
	MsOS_EnableInterrupt(E_INT_PM_IRQ_U3_RT);
	
}

void ISR_U3_WAKEUP(void) //PM_IRQ[26]
{
	WORD u16TimeOut = 0xFFFF;
	DWORD u32TimeoutCnt = 0x2FFFF;

	// exit from U3 state
    g_bHubU3Status = 0;

	bIsInU3State = 0;

	MEM_MSWRITE_BYTE(REG_MBX_PD51_TO_HKR2_U3_STATE, 0x00);

	// enable XTAL
	//if(!(MEM_MSREAD_BYTE_EXT1(REG_0003A6) & BIT2))
	#if 1
	{
	    msRegs(REG_0003A6) = msRegs(REG_0003A6) | BIT2; //enalbe xtal
	    msRegs(REG_0003DA) = msRegs(REG_0003DA) | BIT0; //reg_chip_top_xtal_clk_en
	    
        msRegs(REG_00042E) = msRegs(REG_00042E) | BIT0; //enable core poewr
		msRegs(REG_00042C) = msRegs(REG_00042C) | BIT0; //enable core poewr
	

	    while( (!(msRegs(REG_00039C) & BIT1)) && (u32TimeoutCnt--));
	}
    msRegs(REG_0003BC) = msRegs(REG_0003BC) & (~BIT6); //clk_gen rosc 4M
	
    #endif 
	//Turn on 480MHz for UPLL_U3RT and read lock value
	MEM_MSWRITE_BYTE_MASK(REG_151940, BIT7|BIT6, BIT1|BIT7|BIT6 ); // enable 320MHz clock
	MEM_MSWRITE_BYTE(REG_151942, 0xA0); // 
	MEM_MSWRITE_BYTE(REG_151943, 0x14); // reg_upll_loopdiv_second
	while((!(MEM_MSREAD_BYTE(REG_15194A) & BIT0)) && u16TimeOut) //wait UPLL stable
	{
	    u16TimeOut--;
	}

	// change SQH_deglitch clock to upll_480Mhz
	MEM_MSWRITE_BYTE_MASK(REG_152678, BIT0, BIT0); // 1: use upll_480Mh
	MEM_MSWRITE_BYTE_MASK(REG_152778, BIT0, BIT0);
  
	// w1c reg_u3_clear_wakeup_int
	MEM_MSWRITE_BYTE_MASK(REG_152900, BIT2, BIT2); // Reg_u3_clear_wakeup_int
	MEM_MSWRITE_BYTE_MASK(REG_152C00, BIT2, BIT2);
  
	// disable u3_rxdet_fsm
	MEM_MSWRITE_BYTE_MASK(REG_152934, BIT2, BIT2|BIT4);
	MEM_MSWRITE_BYTE_MASK(REG_152C34, BIT2, BIT2|BIT4);

	// clear u3rd_in_u3 state
	MEM_MSWRITE_BYTE_MASK(REG_152900, 0x00, BIT0); // reg_in_u3
	MEM_MSWRITE_BYTE_MASK(REG_152C00, 0x00, BIT0); 

	// Do RX Calibration
	MEM_MSWRITE_BYTE_MASK(REG_15260B, BIT3, BIT3);
	MEM_MSWRITE_BYTE_MASK(REG_15270B, BIT3, BIT3);

	//Power on ATOP_DECAP 
	MEM_MSWRITE_BYTE_MASK(REG_152605, 0x00, BIT7); // reg_nodie_pd_decap
	MEM_MSWRITE_BYTE_MASK(REG_152705, 0x00, BIT7); // reg_nodie_pd_decap
  
    u32TimeoutCnt=0x7FF; //10ms polling timeout
	while(u32TimeoutCnt)
	{
	
		if (MEM_MSREAD_BYTE(REG_1528EE)&BIT5) //C side wakeup
		{
			MEM_MSWRITE_BYTE_MASK(REG_152786, 0x00, BIT4|BIT3); //hub_reg_ov_en_tx_lfps, hub_reg_ov_en_tx_idle
			MEM_MSWRITE_BYTE_MASK(REG_152788, 0x00, BIT4); //hub_reg_ov_en_pd_rt_sstx0
			while(!(MEM_MSREAD_BYTE(REG_152BEE)&BIT5) && (--u32TimeoutCnt));
			DelayUs(5);
			MEM_MSWRITE_BYTE_MASK(REG_152686, 0x00, BIT4|BIT3); //c_reg_ov_en_tx_lfps, c_reg_ov_en_tx_idle
		    if (glSYS_CC_Pin[1]) //lane1
		    {
                MEM_MSWRITE_BYTE_MASK(REG_152688, 0x00, BIT5); //reg_ov_pd_rt_sstx1
		    }
			else			  	 //lane0
			{
				MEM_MSWRITE_BYTE_MASK(REG_152688, 0x00, BIT4); //reg_ov_pd_rt_sstx0
			}
			break;	
		}
		if (MEM_MSREAD_BYTE(REG_152BEE)&BIT5) //Hub side wakeup
		{
			MEM_MSWRITE_BYTE_MASK(REG_152686, 0x00, BIT4|BIT3); //c_reg_ov_en_tx_lfps, c_reg_ov_en_tx_idle
		    if (glSYS_CC_Pin[1]) //lane1
		    {
                MEM_MSWRITE_BYTE_MASK(REG_152688, 0x00, BIT5); //reg_ov_pd_rt_sstx1
		    }
			else			  	 //lane0
			{
				MEM_MSWRITE_BYTE_MASK(REG_152688, 0x00, BIT4); //reg_ov_pd_rt_sstx0
			}
			while(!(MEM_MSREAD_BYTE(REG_1528EE)&BIT5) && (--u32TimeoutCnt));
			DelayUs(5);
			MEM_MSWRITE_BYTE_MASK(REG_152786, 0x00, BIT4|BIT3); //hub_reg_ov_en_tx_lfps, hub_reg_ov_en_tx_idle
			MEM_MSWRITE_BYTE_MASK(REG_152788, 0x00, BIT4); //hub_reg_ov_en_pd_rt_sstx0
			break;	
		}
		u32TimeoutCnt--;

	}
	if(!u32TimeoutCnt)
	{
	    MEM_MSWRITE_BYTE_MASK(REG_152686, 0x00, BIT4|BIT3); //c_reg_ov_en_tx_lfps, c_reg_ov_en_tx_idle
	    MEM_MSWRITE_BYTE_MASK(REG_152786, 0x00, BIT4|BIT3); //hub_reg_ov_en_tx_lfps, hub_reg_ov_en_tx_idle
	    MEM_MSWRITE_BYTE_MASK(REG_152688, 0x00, BIT5); //reg_ov_pd_rt_sstx1
	    MEM_MSWRITE_BYTE_MASK(REG_152688, 0x00, BIT4); //reg_ov_pd_rt_sstx0
	}

	MEM_MSWRITE_BYTE_MASK(REG_152900, BIT2, BIT2); // clr u3 wakeup interrupt status
	MEM_MSWRITE_BYTE_MASK(REG_152C00, BIT2, BIT2);


	//MEM_MSWRITE_BYTE_MASK(REG_000D6B, BIT2, BIT2); // disable U3 wakeup INT to PD51
	MsOS_DisableInterrupt(E_INT_PM_IRQ_U3_WAKEUP);
}

#endif

#if ( ENABLE_WATCH_DOG_INT )
void ISR_WDT(void)
{
            //Init_WDT( 1 );
    	    if(u8WDT_Status == 0x33)
                u8WDT_Status=0x88;
            else
                u8WDT_Status = 0x33;

            MsOS_EnableInterrupt(E_INT_PM_FIQ_WDT);
}
#endif

#else
#if EnableTime0Interrupt
void Time0Interrupt( void )interrupt 1
#else
static void ms1MsHandler(void)
#endif
{
    TH0 = g_ucTimer0_TH0;
    TL0 = g_ucTimer0_TL0;

    ms_Counter--;

     DPRx_TimerIsrHandler();

     #if ENABLE_HDMI
     mdrv_hdmiRx_TimerHandler();
     #endif

#if ENABLE_MSPI_FLASH_ACCESS
    gsystem_time_ms++;
#endif

#if ENABLE_DP_OUTPUT
    if(DPTx_time_ms == 0xFFFFFFFF)
    {
        DPTx_time_ms = 0;
    }
    else
    {
        DPTx_time_ms++;
    }
#endif

#if ENABLE_DEBUG
    if( u8ReSyncCounter )
    {
        u8ReSyncCounter--;
        if( u8ReSyncCounter == 0)
        {
            Clr_RxBusyFlag();
            SIORxIndex = 0;
        }
    }
#endif

#if UART1
    if( u8ReSyncCounter1)
    {
        u8ReSyncCounter1--;
        if( u8ReSyncCounter1 == 0 )
        {
            SIORxIndex1 = 0;
        }
    }
#endif

    if( bDelayFlag )
    {
        if( u16DelayCounter > 0 )
            u16DelayCounter--;

        if( u16DelayCounter == 0 )
            bDelayFlag = 0;
    }

    if( bTimeOutCounterFlag )
    {
        if( u16TimeOutCounter > 0 )
            u16TimeOutCounter--;

        if( u16TimeOutCounter == 0 )
            bTimeOutCounterFlag = 0;
    }


#if ENABLE_TIME_MEASUREMENT   //need enable this if test Timer0
    if( g_u16TimeMeasurementCounter )
        g_u16TimeMeasurementCounter--;
#endif
#if ENABLE_DAC && (CHIP_ID==CHIP_MT9700)
            if( bDAC_DePopCountFlag )
            {
                if( --w16DAC_DePopCounter == 0 )
                {
                    bDAC_DePopCountFlag = 0;
                    bDAC_DePopFlag = 1;
                }
            }
#endif

    if(!g_bMcuPMClock) // system runs at normal speed
    {
        if( u8DCRStepTransCnt )
            u8DCRStepTransCnt --;

        if( bPanelOffOnFlag ) //091013
            if( --u16PanelOffOnCounter == 0 )
                bPanelOffOnFlag = 0;
    }

    if(( ms_Counter % 10 ) == 0 )
    {
        Set_ms10Flag();

        if(( ms_Counter % 50 ) == 0 )
        {
            Set_ms50Flag();

            if(( ms_Counter % 200 ) == 0 )
            {
#if _NEW_SOG_DET_
                if(SyncLossState())
                    NEW_SOG_DET_FLUSH();
#endif

#if _NEW_SOG_WAKEUP_DET_
                if(ucWakeupStatus == ePMSTS_NON)
                    NEW_SOG_WAKEUP_FLUSH();
#endif
            }

            if(( ms_Counter % 500 ) == 0 )
            {
                Set_ms500Flag();

                if( ms_Counter == 0 )        //
                {
                    #if ENABLE_FREESYNC
                    Set_ms1000Flag();
                    #endif
                    Set_SecondFlag();
                    ms_Counter = SystemTick;
                }
            }
        }

    }

    if(TF0)
    {
        TH0 = g_ucTimer0_TH0;
        TL0 = g_ucTimer0_TL0;
    }

    TF0 = 0; // timer 1 flag
}


#if EnableTime1Interrupt
void Time1Interrupt( void )interrupt 3
{
    TH1=g_ucTimer0_TH0;
    TL1=g_ucTimer0_TL0; // timer 1 counter

#if ENABLE_TIME_MEASUREMENT  //need enable this if test Timer1
    if(bTimeMeasurementFlag)
    {
        if(--g_u16TimeMeasurementCounter == 0)
            bTimeMeasurementFlag = 0;
    }
#endif

    g_u16MsCounter1--;
    if (g_u16MsCounter1==0) //
    {
        bSecondTimer1 = TRUE;
        g_u16MsCounter1=1000;
    }
    TF1=0; // timer 1 flag
}
#endif

#if EnableTime2Interrupt
void Time2Interrupt( void )interrupt 5
{
    TF2 = 0;
}
#endif

#if ENABLE_DEBUG
#if EnableSerialPortTXInterrupt

void SerialPortInterrupt( void )interrupt 4
{
    BYTE data sioData;

    if( RI )
    {
        RI = 0;
        sioData = SBUF;
        if( SIORxIndex < _UART_BUFFER_LENGTH_ )
        {
            SIORxBuffer[SIORxIndex++] = sioData;
            if(SIORxIndex < UART_CMD_LENGTH)
                u8ReSyncCounter = ReSyncTick;
            else
                u8ReSyncCounter = 0xFF;
        }
    }

    if(TI)
    {
        TI = 0;
        TI0_FLAG = 1;

        if(!UartPolling)
        {
            if(!check_uart_tx_buffer_empty())
            {
                SBUF = SIOTxBuffer[SIOTxSendIndex];
                uart_tx_increase_send_cnt();
            }
            else
            {
                UART_SEND_DONE = 1;
            }
        }
    }
}

#else
void SerialPortInterrupt( void )interrupt 4
{
    BYTE data sioData;

    if( RI )
    {
        RI = 0;
        sioData = SBUF;
        if( SIORxIndex < _UART_BUFFER_LENGTH_ )
        {
            SIORxBuffer[SIORxIndex++] = sioData;
            if(SIORxIndex < UART_CMD_LENGTH)
                u8ReSyncCounter = ReSyncTick;
            else
                u8ReSyncCounter = 0xFF;
        }
    }
    if( TI )
    {
        TI = 0;
        TI0_FLAG = 1;
    }
}
#endif
#endif

#if UART1
void SerialPort1Interrupt (void)interrupt 16
{
    BYTE data sioData;

    if(S1CON & RI1)
    {
        S1CON &= ~RI1;
        sioData = S1BUF;
        if( SIORxIndex1 < _UART_BUFFER_LENGTH_ )
        {
            SIORxBuffer1[SIORxIndex1++] = sioData;
#if ENABLE_MSBHK
            if(SIORxIndex1 < UART_COM_LEN)
#else
            if(SIORxIndex1 < UART1_CMD_LENGTH)
#endif
                u8ReSyncCounter1 = ReSyncTick;
            else
                u8ReSyncCounter1 = 0xFF;
        }
    }

    if(S1CON & TI1)
    {
        S1CON &= ~TI1;
        TI1_FLAG = 1;
    }
}
#endif


#if EnableExt0Interrupt
void EXT0Interrupt( void )interrupt 0 // non PM FIQ
{
    BYTE data ucStatus_10191E;
#if DEF_HDCP2RX_ISR_MODE
    BYTE data ucStatus_103C1C, ucStatus_002BCC, ucStatus_002BCD, ucStatus_002BCE, ucStatus_002BCF;
#endif

    ucStatus_10191E = MEM_MSREAD_BYTE(REG_10191E);

#if DEF_HDCP2RX_ISR_MODE
    //store current IMI releative setting
    ucStatus_103C1C = MEM_MSREAD_BYTE(REG_103C1C);
    ucStatus_002BCC = MEM_MSREAD_BYTE(REG_002BCC);
    ucStatus_002BCD = MEM_MSREAD_BYTE(REG_002BCD);
    ucStatus_002BCE = MEM_MSREAD_BYTE(REG_002BCE);
    ucStatus_002BCF = MEM_MSREAD_BYTE(REG_002BCF);
#endif

    if(ucStatus_10191E & BIT2) //INT_NPM_FIQ_SECU2HK51
    {
        // Do something here ...
        HDCPHandler_SetR2EventFlag();
    }

    MEM_MSWRITE_BYTE(REG_10191E,ucStatus_10191E);

#if DEF_HDCP2RX_ISR_MODE
        //leave interrupt to recovery IMI releative setting
        MEM_MSWRITE_BYTE(REG_103C1C, ucStatus_103C1C);
        MEM_MSWRITE_BYTE(REG_002BCC, ucStatus_002BCC);
        MEM_MSWRITE_BYTE(REG_002BCD, ucStatus_002BCD);
        MEM_MSWRITE_BYTE(REG_002BCE, ucStatus_002BCE);
        MEM_MSWRITE_BYTE(REG_002BCF, ucStatus_002BCF);
#endif

    IE0 = 0;
}
#endif


#if EnableExt1Interrupt
void EXT1Interrupt( void )interrupt 2 // pm IRQ, [0]nonPM IRQ
{
    BYTE data ucStatus_002B38, ucStatus_10193F, ucStatus_101938;
    BYTE data ucStatus_002B39;
    BYTE data ucScBankBK;

    ucStatus_002B38 = MEM_MSREAD_BYTE(REG_002B38);
    ucStatus_002B39 = MEM_MSREAD_BYTE(REG_002B39);

    if( ucStatus_002B38 & BIT2 ) // INT_PM_IRQ_D2B
    {
        ISR_DDC2Bi();
    }

    if( ucStatus_002B39 & BIT5 ) // INT_PM_IRQ_
    {
         DPRXISR_PM();
    }

    if(ucStatus_002B38 & BIT0) // INT_PM_IRQ_NPMIRQ
    {
        ucScBankBK = MEM_MSREAD_BYTE(SC00_00); // put after P2=0 for XFR access correctly on function calls.

       DPRXISR();
       #if PANEL_EDP
        /*
        if(msReadByte(REG_EDP_TX0_P0_53_L)&(BIT2|BIT1|BIT0))
        {
            mapi_eDPTx_HPDIRQ_Handler(0);
        }
        */
        eDPTx_HPDISR();
        eDPTx_AUXISR();
       #endif

        ucStatus_101938 = MEM_MSREAD_BYTE(REG_101938);
        ucStatus_10193F = MEM_MSREAD_BYTE(REG_10193F);

        #if ENABLE_UART_PIU
        if( ucStatus_101938 & BIT0 ) // INT_NPM_IRQ_UART
        {
            piu_uart_isr();
        }
        #endif

        #if UseINT
        if( ucStatus_10193F & BIT0 ) // INT_NPM_IRQ_SCINT
        {
            IsrModeChangeCheck();
        }
        #endif

        #if (ENABLE_DP_OUTPUT == 0x1)
        mapi_DPTx_TX_IRQ_Handler();
        #endif

        MEM_MSWRITE_BYTE(SC00_00, ucScBankBK);
    }

    IE1 = 0;
}

#endif

#if EnableExt2Interrupt
void EXT2Interrupt( void )interrupt 9 // ext timer int
{
    ms1MsHandler();
    IEX2 = 0;
}
#endif

#if EnableExt3Interrupt
void EXT3Interrupt( void )interrupt 10 // WDT interrupt
{
    u8WDT_Status = 0x33;
    IEX3 = 0;
}
#endif

#if EnableExt4Interrupt
void EXT4Interrupt( void )interrupt 11 // pm FIQ
{
    BYTE data ucStatus_002B18, ucStatus_002B19;

    ucStatus_002B18 = MEM_MSREAD_BYTE_EXT4(REG_002B18);
    ucStatus_002B19 = MEM_MSREAD_BYTE_EXT4(REG_002B19);

    #if ENABLE_USB_TYPEC
    if(ucStatus_002B19 & BIT6) //INT_PM_FIQ_PD512HK51
    {
        // Do something here ...
        drvmbx_receive_cmd_isr();
    }
    #endif

    MEM_MSWRITE_BYTE(REG_002B18,ucStatus_002B18);
    MEM_MSWRITE_BYTE(REG_002B19,ucStatus_002B19);

    IEX4 = 0;
}
#endif

void DPRXISR_PM(void)
{
    DPRx_ID idata dprx_id;
    WORD idata OffsetAux;

    for(dprx_id = DPRx_ID_0; dprx_id < DPRx_ID_MAX; dprx_id++)
    {
        if(!(GET_DPRx_FUNCTION_ENABLE_PORT(dprx_id)))
        {
            continue;
        }


       OffsetAux = DP_REG_OFFSETAUX(dprx_id);

    //=======================================================================
        //     DP MST
        //=======================================================================
        if(msRegs[REG_DPRX_AUX_DPCD_05_L + OffsetAux]&(BIT3))   // MST
        {
            msRegs[REG_DPRX_AUX_DPCD_05_L+OffsetAux] |= BIT0;

            #if (DP_MST_FUNCTION_SUPPORT == 0x1)
            mapi_DPMST_IsrHandler(dprx_id);
            #endif
        }

        if(msRegs[REG_DPRX_AUX_7C_H + OffsetAux]&(BIT4))   // short HPD isr mode
        {
            //This would be used at Type C MST mode !
            msRegs[REG_DPRX_AUX_7C_H+OffsetAux] |= BIT7;
            glDPRxShortHPDIsrFlag = TRUE;
        }
        //=======================================================================
        //     DP MCCS
        //=======================================================================
        if(msRegs[REG_DPRX_AUX_3C_L+OffsetAux]&(BIT7))   // MCCS
        {
            #if (ENABLE_DP_INPUT == 0x1)
            DDC2BI_DP(dprx_id);
            #endif

            msRegs[REG_DPRX_AUX_3C_H+OffsetAux] = BIT7;
            msRegs[REG_DPRX_AUX_3C_H+OffsetAux] = 0;

         }
        #if 0
        //=======================================================================
        //      Programable DPCD_0
        //=======================================================================
        if(msRegs[REG_DPRX_AUX_48_L+OffsetAux]&(BIT0))   //Programable DPCD_0
        {
        BYTE XDATA uctemp = 0;
        BYTE XDATA ucAuxCommand = 0;
        BYTE XDATA ucAuxLength = 0;
        BYTE XDATA ubTimeOutLimt = 100;
        BYTE XDATA Reply_Data_21[16] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0x06,0x00,0x00,0x00,0x80,0x00};
        DWORD XDATA DPCD_Address=0;
        WORD XDATA OffSet_Address=0;
        WORD XDATA OffSet9_Address=0;
        BYTE XDATA DPCD_Offset = 0;

        OffSet_Address = OffsetAux;
        DPCD_Address = msRegs[REG_DPRX_AUX_1B_L+OffSet_Address]&0x0F;  // reg_aux_rx_addr
        DPCD_Address = (DPCD_Address << 16) + (msRegs[REG_DPRX_AUX_1A_H+OffSet_Address] << 8) + msRegs[REG_DPRX_AUX_1A_L+OffSet_Address];
        DPCD_Offset =  DPCD_Address - DPRX_PROGRAM_DPCD0_ADDERSS;

        ucAuxCommand = msRegs[REG_DPRX_AUX_1C_L+OffSet_Address]&0x0F;
        ucAuxLength = msRegs[REG_DPRX_AUX_1D_L+OffSet_Address];

        msRegs[REG_DPRX_AUX_18_L+OffSet_Address] = msRegs[REG_DPRX_AUX_18_L+OffSet_Address] |BIT1;  //reply by MCU

        if(ucAuxCommand&BIT0) // TX Read
        {

        for (uctemp =0; uctemp <ucAuxLength ;uctemp++)
        {
            msRegs[REG_DPRX_AUX_79_H + OffSet_Address] =Reply_Data_21[DPCD_Offset + uctemp];  //DPCD Value
            msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] = msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] |BIT0;
            do
            {
            ubTimeOutLimt --;
            } while((!(msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] & BIT2))  && (ubTimeOutLimt > 0));

            msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] = msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] |BIT1;
        }
            msRegs[REG_DPRX_AUX_1F_L+OffSet_Address] = 0x00;  //reply ack
            msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] = msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] |BIT0; //trigger
        }
        else // TX Write
        {
            msRegs[REG_DPRX_AUX_1F_L+OffSet_Address] = 0;  //reply ack
            msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] = msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] |BIT0; //trigger
        }
        msRegs[REG_DPRX_AUX_18_L+ OffSet_Address] = msRegs[REG_DPRX_AUX_18_L+ OffSet_Address] & (~BIT1);  //reply by MCU

        msRegs[REG_DPRX_AUX_49_L+OffSet_Address] = msRegs[REG_DPRX_AUX_DPCD_4D_H+OffSet_Address] |BIT0;

         }
        #endif
        #if 0
        //=======================================================================
        //      Programable DPCD_1   0x2200
        //=======================================================================
        if(msRegs[REG_DPRX_AUX_48_L+OffsetAux]&(BIT1))   //Programable DPCD_1
        {
            BYTE XDATA uctemp = 0;
            BYTE XDATA ucAuxCommand = 0;
            BYTE XDATA ucAuxLength = 0;
            BYTE XDATA ubTimeOutLimt = 100;
            BYTE XDATA  Reply_Data_22[16] = {0x51,0x52,0x53,0x54,0x55,0x66,0x77,0x88,0x99,0xaa,0x06,0x00,0x00,0x00,0x80,0x00};
            WORD XDATA DPCD_Address=0;
            WORD XDATA OffSet_Address=0;
            WORD XDATA OffSet9_Address=0;

            OffSet_Address = OffsetAux;

            DPCD_Address = (msRegs[REG_DPRX_AUX_1A_H+OffSet_Address] << 8) + msRegs[REG_DPRX_AUX_1A_L+OffSet_Address];
            ucAuxCommand = msRegs[REG_DPRX_AUX_1C_L+OffSet_Address]&0x0F;
            ucAuxLength = msRegs[REG_DPRX_AUX_1D_L+OffSet_Address];

            msRegs[REG_DPRX_AUX_18_L+OffSet_Address] = msRegs[REG_DPRX_AUX_18_L+OffSet_Address] |BIT1;  //reply by MCU
            if(ucAuxCommand&BIT0) // TX Read
            {
                for (uctemp =0; uctemp <ucAuxLength ;uctemp++)
                 {
                    msRegs[REG_DPRX_AUX_79_H + OffSet_Address] =Reply_Data_22[DPCD_Address + uctemp];  //DPCD Value
                    msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] = msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] |BIT0;
                    do
                    {
                        ubTimeOutLimt --;
                    } while((!(msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] & BIT2))  && (ubTimeOutLimt > 0));

                    msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] = msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] |BIT1;
                }
                msRegs[REG_DPRX_AUX_1F_L+OffSet_Address] = 0x00;  //reply ack
                msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] = msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] |BIT0; //trigger
            }
            else // TX Write
            {
                 msRegs[REG_DPRX_AUX_1F_L+OffSet_Address] = 0;  //reply ack
                msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] = msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] |BIT0; //trigger
            }

            msRegs[REG_DPRX_AUX_18_L+ OffSet_Address] = msRegs[REG_DPRX_AUX_18_L+ OffSet_Address] & (~BIT1);  //reply by MCU

            msRegs[REG_DPRX_AUX_49_L+OffSet_Address] = msRegs[REG_DPRX_AUX_DPCD_4D_H+OffSet_Address] |BIT1;

        }
        #endif
        #if 0
         //=======================================================================
         //      Programable DPCD_2  //0x2210
         //=======================================================================
         if(msRegs[REG_DPRX_AUX_48_L+OffsetAux]&(BIT2))   //Programable DPCD_2
         {
             BYTE XDATA uctemp = 0;
             BYTE XDATA ucAuxCommand = 0;
             BYTE XDATA ucAuxLength = 0;
             BYTE XDATA ubTimeOutLimt = 100;
             BYTE XDATA Reply_Data_22[16] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0x06,0x00,0x00,0x00,0x80,0x00};
             WORD XDATA DPCD_Address=0;
             WORD XDATA OffSet_Address=0;
             WORD XDATA OffSet9_Address=0;
             BYTE XDATA DPCD_Offset = 0;

             OffSet_Address = OffsetAux;

             DPCD_Address = (msRegs[REG_DPRX_AUX_1A_H+OffSet_Address] << 8) + msRegs[REG_DPRX_AUX_1A_L+OffSet_Address];
             ucAuxCommand = msRegs[REG_DPRX_AUX_1C_L+OffSet_Address]&0x0F;
             ucAuxLength = msRegs[REG_DPRX_AUX_1D_L+OffSet_Address];
             DPCD_Offset =  DPCD_Address - DPRX_PROGRAM_DPCD1_ADDERSS;

             msRegs[REG_DPRX_AUX_18_L+OffSet_Address] = msRegs[REG_DPRX_AUX_18_L+OffSet_Address] |BIT1;  //reply by MCU
             if(ucAuxCommand&BIT0) // TX Read
             {

                for (uctemp =0; uctemp <ucAuxLength ;uctemp++)
                {
                msRegs[REG_DPRX_AUX_79_H + OffSet_Address] =Reply_Data_22[DPCD_Offset + uctemp];  //DPCD Value
                msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] = msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] |BIT0;
                do
                {
                    ubTimeOutLimt --;
                } while((!(msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] & BIT2))  && (ubTimeOutLimt > 0));

                msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] = msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] |BIT1;
                }
                 msRegs[REG_DPRX_AUX_1F_L+OffSet_Address] = 0;  //reply ack
                 msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] = msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] |BIT0; //trigger
             }
             else // TX Write.
             {
                msRegs[REG_DPRX_AUX_1F_L+OffSet_Address] = 0;  //reply ack
                 msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] = msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] |BIT0; //trigger
             }
             msRegs[REG_DPRX_AUX_18_L+ OffSet_Address] = msRegs[REG_DPRX_AUX_18_L+ OffSet_Address] & (~BIT1);  //reply by MCU

             msRegs[REG_DPRX_AUX_49_L+OffSet_Address] = msRegs[REG_DPRX_AUX_DPCD_4D_H+OffSet_Address] |BIT2;

         }
         #endif
        //=======================================================================
        //      Programable DPCD_8    DPCD100h Use
        //=======================================================================
        if(msRegs[REG_DPRX_AUX_DPCD_3A_L+OffsetAux]&(BIT0))   //Programable DPCD_8
        {
            WORD XDATA OffSet_Address=0;
            BYTE XDATA E_Data = 0;
            DWORD usTimeOutCount = 0;

            OffSet_Address = OffsetAux;

            msRegs[REG_DPRX_AUX_DPCD_3A_H+OffSet_Address] = msRegs[REG_DPRX_AUX_DPCD_3A_H+OffSet_Address] |BIT4;
            msRegs[REG_DPRX_AUX_DPCD_3A_H+OffSet_Address] = msRegs[REG_DPRX_AUX_DPCD_3A_H+OffSet_Address] & (~BIT4);

         }
        //=======================================================================
        //      Programable DPCD_9    DPCD68005h Use
        //=======================================================================
        if(msRegs[REG_DPRX_AUX_DPCD_3A_L+OffsetAux]&(BIT1))   //Programable DPCD_9
        {
            g_stHdcpHandler[DEF_HDCPRX_DP_PORT].bPrepare2SetRdyBit = TRUE;  //r0 read done rx prepare to set rdy bit
            gDPRxInfo[dprx_id].bHDCP14R0beRead = TRUE;

            msRegs[REG_DPRX_AUX_DPCD_3A_H + OffsetAux] = msRegs[REG_DPRX_AUX_DPCD_3A_H + OffsetAux] |BIT5;
            msRegs[REG_DPRX_AUX_DPCD_3A_H + OffsetAux] = msRegs[REG_DPRX_AUX_DPCD_3A_H + OffsetAux] & (~BIT5);

         }

        //=======================================================================
        //      Programable DPCD_10    DPCD68014h Use
        //=======================================================================
        if(msRegs[REG_DPRX_AUX_DPCD_3A_L+OffsetAux]&(BIT2))   //Programable DPCD_10
        {
            gDPRxInfo[dprx_id].bHDCP14VprimebeRead = TRUE;

            msRegs[REG_DPRX_AUX_DPCD_3A_H + OffsetAux] = msRegs[REG_DPRX_AUX_DPCD_3A_H + OffsetAux] |BIT6;
            msRegs[REG_DPRX_AUX_DPCD_3A_H + OffsetAux] = msRegs[REG_DPRX_AUX_DPCD_3A_H + OffsetAux] & (~BIT6);

         }

        #if XDATA_PROGRAMDPCD  //TEST code Port 1
        //=======================================================================
        //     //XDATA Program DPCD 0
        //=======================================================================
        if(msRegs[REG_DPRX_AUX_7C_H+OffsetAux]&(BIT0))   //XDATA Program DPCD
        {
            WORD XDATA OffSet_Address=0;
            BYTE XDATA E_Data = 0;
            DWORD usTimeOutCount = 0;
            BYTE u8PortIdx=0;

            OffSet_Address = OffsetAux;
            msRegs[REG_DPRX_AUX_7F_L]  +=1;
            while(!((msRegs[REG_DPRX_AUX_74_L+OffsetAux]&0x38)==BIT3))
            {
                   if(++usTimeOutCount > 0xFFFF0000)
                   {
                        break;
                   }
            }
            msRegs[REG_DPRX_AUX_DPCD_57_L+OffSet_Address] |=BIT3;
            msRegs[REG_DPRX_AUX_7C_H+OffSet_Address] = msRegs[REG_DPRX_AUX_7C_H+OffSet_Address] |BIT3;
            msRegs[REG_DPRX_AUX_7C_H+OffSet_Address] = 0;

        }
        //=======================================================================
          //     //XDATA Program DPCD 1
          //=======================================================================
          if(msRegs[REG_DPRX_AUX_49_H+OffsetAux]&(BIT0))   //XDATA Program DPCD
          {
              WORD XDATA OffSet_Address=0;
              BYTE XDATA E_Data = 0;
              DWORD usTimeOutCount = 0;
              BYTE u8PortIdx=0;

              OffSet_Address = OffsetAux;
              msRegs[REG_DPRX_AUX_7F_H]  +=1;
             while(!((msRegs[REG_DPRX_AUX_74_L+OffsetAux]&0x38)==BIT3))
            {
                   if(++usTimeOutCount > 0xFFFF0000)
                   {
                        break;
                   }
            }
              msRegs[REG_DPRX_AUX_DPCD_57_L+OffSet_Address] |=BIT7;
              msRegs[REG_DPRX_AUX_49_H+OffSet_Address] = msRegs[REG_DPRX_AUX_49_H+OffSet_Address] |BIT3;
              msRegs[REG_DPRX_AUX_49_H+OffSet_Address] = 0;

          }
         #endif
         #if 0
         //=======================================================================
         //      AUX CMD
         //=======================================================================
         if(msRegs[REG_DPRX_AUX_DPCD_50_L+DPRX_PHY_REG_OFFSET(dprx_id)]&(BIT3))   //Programable DPCD_0
         {
             BYTE XDATA uctemp = 0;
             BYTE XDATA ucAuxCommand = 0;
             BYTE XDATA ucAuxLength = 0;
             BYTE XDATA ubTimeOutLimt = 100;
             BYTE XDATA Reply_Data_21[16] = {0};
             WORD XDATA DPCD_Address=0;
             WORD XDATA OffSet_Address=0;
             WORD XDATA OffSet9_Address=0;

             OffSet_Address = DPRX_PHY_REG_OFFSET(dprx_id);
             OffSet9_Address = DPRX_PHY9_REG_OFFSET(dprx_id);

              gDPRxInfo[dprx_id].bDPAUXVALID = TRUE;

             DPCD_Address = (msRegs[REG_DPRX_AUX_1A_H+OffSet_Address] << 8) + msRegs[REG_DPRX_AUX_1A_L+OffSet_Address];
             ucAuxCommand = msRegs[REG_DPRX_AUX_1C_L+OffSet_Address]&0x0F;
             ucAuxLength = msRegs[REG_DPRX_AUX_1D_L+OffSet_Address];

             msRegs[REG_0016E0] =  msRegs[REG_0016E0] +1;

            // msRegs[REG_DPRX_AUX_3C_H+OffSet_Address] = msRegs[REG_DPRX_AUX_3C_H+OffSet_Address] |BIT4;
           //  msRegs[REG_DPRX_AUX_3C_H+OffSet_Address] = msRegs[REG_DPRX_AUX_3C_H+OffSet_Address] & (~BIT4);

             msRegs[REG_DPRX_AUX_SBM_72_H] = msRegs[REG_DPRX_AUX_SBM_72_H] |(BIT0 <<dprx_id);
             while(count--)
             {
                 _nop_();
             }
             msRegs[REG_DPRX_AUX_SBM_72_H] = msRegs[REG_DPRX_AUX_SBM_72_H] & (~ (BIT0 << dprx_id));

             msRegs[REG_DPRX_AUX_DPCD_4D_H+OffSet_Address] = msRegs[REG_DPRX_AUX_DPCD_4D_H+OffSet_Address] |BIT7;
             msRegs[REG_DPRX_AUX_DPCD_4D_H+OffSet_Address] = msRegs[REG_DPRX_AUX_DPCD_4D_H+OffSet_Address] & (~BIT7);
         }


         //=======================================================================
         //      Programable DPCD_0
         //=======================================================================
         if(msRegs[REG_DPRX_AUX_48_L+OffsetAux]&(BIT0))   //Programable DPCD_0
         {
             BYTE XDATA uctemp = 0;
             BYTE XDATA ucAuxCommand = 0;
             BYTE XDATA ucAuxLength = 0;
             BYTE XDATA ubTimeOutLimt = 100;   //  TP3 = 02h[6]  = 0xC4 ( TP3 On / TP3 off 0x84) // , TP4 = 03h[7]  =  0x81 ( TP4 On)  / 0x01 (TP4 off)
             BYTE XDATA Reply_Data_21[16] = {0x12,0x14,0xC4,0x81,0x01,0x00,0x01,0x00,0x02,0x02,0x06,0x00,0x00,0x00,0x80,0x00};
             DWORD XDATA DPCD_Address=0;
             WORD XDATA OffSet_Address=0;
             WORD XDATA OffSet9_Address=0;
             BYTE XDATA DPCD_Offset = 0;

             OffSet_Address = OffsetAux;

             DPCD_Address = msRegs[REG_DPRX_AUX_1B_L+OffSet_Address]&0x0F;  // reg_aux_rx_addr
             DPCD_Address = (DPCD_Address << 16) + (msRegs[REG_DPRX_AUX_1A_H+OffSet_Address] << 8) + msRegs[REG_DPRX_AUX_1A_L+OffSet_Address];
             DPCD_Offset =  DPCD_Address - DPRX_PROGRAM_DPCD0_ADDERSS;

             ucAuxCommand = msRegs[REG_DPRX_AUX_1C_L+OffSet_Address]&0x0F;
             ucAuxLength = msRegs[REG_DPRX_AUX_1D_L+OffSet_Address];

             msRegs[REG_DPRX_AUX_18_L+OffSet_Address] = msRegs[REG_DPRX_AUX_18_L+OffSet_Address] |BIT1;  //reply by MCU

             if(ucAuxCommand&BIT0) // TX Read
             {

                  for (uctemp =0; uctemp <ucAuxLength ;uctemp++)
                  {
                     msRegs[REG_DPRX_AUX_79_H + OffSet_Address] =Reply_Data_21[DPCD_Offset + uctemp];  //DPCD Value
                     msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] = msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] |BIT0;
                     do
                     {
                         ubTimeOutLimt --;
                     } while((!(msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] & BIT2))  && (ubTimeOutLimt > 0));

                     msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] = msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] |BIT1;
                 }
                 msRegs[REG_DPRX_AUX_1F_L+OffSet_Address] = 0x00;  //reply ack
                 msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] = msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] |BIT0; //trigger
             }
             else // TX Write
             {
                 msRegs[REG_DPRX_AUX_1F_L+OffSet_Address] = 0;  //reply ack
                 msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] = msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] |BIT0; //trigger
             }
             msRegs[REG_DPRX_AUX_18_L+ OffSet_Address] = msRegs[REG_DPRX_AUX_18_L+ OffSet_Address] & (~BIT1);  //reply by MCU

             msRegs[REG_DPRX_AUX_49_L+OffSet_Address] = msRegs[REG_DPRX_AUX_DPCD_4D_H+OffSet_Address] |BIT0;

         }

         //=======================================================================
         //      Programable DPCD_1   0x2200
         //=======================================================================
         if(msRegs[REG_DPRX_AUX_48_L+OffsetAux]&(BIT1))   //Programable DPCD_1
         {
             BYTE XDATA uctemp = 0;
             BYTE XDATA ucAuxCommand = 0;
             BYTE XDATA ucAuxLength = 0;
             BYTE XDATA ubTimeOutLimt = 100;
             BYTE XDATA ucDPCD_0x220x[0x10] =  {0x14, 0x14, 0xC4, 0x01, 0x01, 0x00, 0x01, 0x00,0x02, 0x02, 0x06, 0x00, 0x00, 0x00, 0x80, 0x00};
             WORD XDATA DPCD_Address=0;
             WORD XDATA OffSet_Address=0;
             WORD XDATA OffSet9_Address=0;

             OffSet_Address = OffsetAux;

             DPCD_Address = (msRegs[REG_DPRX_AUX_1A_H+OffSet_Address] << 8) + msRegs[REG_DPRX_AUX_1A_L+OffSet_Address];
             ucAuxCommand = msRegs[REG_DPRX_AUX_1C_L+OffSet_Address]&0x0F;
             ucAuxLength = msRegs[REG_DPRX_AUX_1D_L+OffSet_Address];

             msRegs[REG_DPRX_AUX_18_L+OffSet_Address] = msRegs[REG_DPRX_AUX_18_L+OffSet_Address] |BIT1;  //reply by MCU
             if(ucAuxCommand&BIT0) // TX Read
             {
                 for (uctemp =0; uctemp <ucAuxLength ;uctemp++)
                  {
                     msRegs[REG_DPRX_AUX_79_H + OffSet_Address] =ucDPCD_0x220x[DPCD_Address + uctemp];  //DPCD Value
                     msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] = msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] |BIT0;
                     do
                     {
                         ubTimeOutLimt --;
                     } while((!(msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] & BIT2))  && (ubTimeOutLimt > 0));

                     msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] = msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] |BIT1;
                 }
                 msRegs[REG_DPRX_AUX_1F_L+OffSet_Address] = 0x00;  //reply ack
                 msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] = msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] |BIT0; //trigger
             }
             else // TX Write
             {
                  msRegs[REG_DPRX_AUX_1F_L+OffSet_Address] = 0;  //reply ack
                 msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] = msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] |BIT0; //trigger
             }

             msRegs[REG_DPRX_AUX_18_L+ OffSet_Address] = msRegs[REG_DPRX_AUX_18_L+ OffSet_Address] & (~BIT1);  //reply by MCU

             msRegs[REG_DPRX_AUX_49_L+OffSet_Address] = msRegs[REG_DPRX_AUX_DPCD_4D_H+OffSet_Address] |BIT1;

         }
            //=======================================================================
         //      Programable DPCD_2  //0x2210
         //=======================================================================
         if(msRegs[REG_DPRX_AUX_48_L+OffsetAux]&(BIT2))   //Programable DPCD_2
         {
             BYTE XDATA uctemp = 0;
             BYTE XDATA ucAuxCommand = 0;
             BYTE XDATA ucAuxLength = 0;
             BYTE XDATA ubTimeOutLimt = 100;
             BYTE XDATA ucDPCD_0x2210 = 0x08;
             WORD XDATA DPCD_Address=0;
             WORD XDATA OffSet_Address=0;
             WORD XDATA OffSet9_Address=0;

             OffSet_Address = OffsetAux;

             DPCD_Address = (msRegs[REG_DPRX_AUX_1A_H+OffSet_Address] << 8) + msRegs[REG_DPRX_AUX_1A_L+OffSet_Address];
             ucAuxCommand = msRegs[REG_DPRX_AUX_1C_L+OffSet_Address]&0x0F;
             ucAuxLength = msRegs[REG_DPRX_AUX_1D_L+OffSet_Address];

             msRegs[REG_DPRX_AUX_18_L+OffSet_Address] = msRegs[REG_DPRX_AUX_18_L+OffSet_Address] |BIT1;  //reply by MCU
             if(ucAuxCommand&BIT0) // TX Read
             {

                     msRegs[REG_DPRX_AUX_79_H + OffSet_Address] = ucDPCD_0x2210;
                     msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] = msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] |BIT0;
                     do
                     {
                         ubTimeOutLimt --;
                     } while((!(msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] & BIT2))  && (ubTimeOutLimt > 0));

                     msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] = msRegs[REG_DPRX_AUX_79_L+ OffSet_Address] |BIT1;

                 msRegs[REG_DPRX_AUX_1F_L+OffSet_Address] = 0;  //reply ack
                 msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] = msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] |BIT0; //trigger
             }
             else // TX Write.
             {
                msRegs[REG_DPRX_AUX_1F_L+OffSet_Address] = 0;  //reply ack
                 msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] = msRegs[REG_DPRX_AUX_3B_H+OffSet_Address] |BIT0; //trigger
             }
             msRegs[REG_DPRX_AUX_18_L+ OffSet_Address] = msRegs[REG_DPRX_AUX_18_L+ OffSet_Address] & (~BIT1);  //reply by MCU

             msRegs[REG_DPRX_AUX_49_L+OffSet_Address] = msRegs[REG_DPRX_AUX_DPCD_4D_H+OffSet_Address] |BIT2;

         }
          //=======================================================================
         //      Programable DPCD_3
         //=======================================================================
         if(msRegs[REG_DPRX_AUX_48_L+OffsetAux]&(BIT3))   //Programable DPCD_3
         {
             BYTE XDATA uctemp = 0;
             BYTE XDATA ucAuxCommand = 0;
             BYTE XDATA ucAuxLength = 0;
             BYTE XDATA ubTimeOutLimt = 100;
             BYTE XDATA Test[16] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x78};
             WORD XDATA OffSet_Address=0;

             OffSet_Address = OffsetAux;

         }
         //=======================================================================
         //      Programable DPCD_4
         //=======================================================================
         if(msRegs[REG_DPRX_AUX_DPCD_0E_L+OffsetAux]&(BIT0))   //Programable DPCD_0
         {
             BYTE XDATA uctemp = 0;
             BYTE XDATA ucAuxCommand = 0;
             BYTE XDATA ucAuxLength = 0;
             BYTE XDATA ubTimeOutLimt = 100;
             BYTE XDATA Test[16] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x78};
             WORD XDATA OffSet_Address=0;

             OffSet_Address = OffsetAux;

             ucAuxCommand = msRegs[REG_DPRX_AUX_1C_L+OffSet_Address]&0x0F;
             ucAuxLength = msRegs[REG_DPRX_AUX_1D_L+OffSet_Address];

             msRegs[REG_DPRX_AUX_3C_H+OffSet_Address] = msRegs[REG_DPRX_AUX_3C_H+OffSet_Address] |BIT4;
             msRegs[REG_DPRX_AUX_3C_H+OffSet_Address] = msRegs[REG_DPRX_AUX_3C_H+OffSet_Address] & (~BIT4);

             msRegs[REG_DPRX_AUX_DPCD_4F_L+OffSet_Address] = msRegs[REG_DPRX_AUX_DPCD_4F_L+OffSet_Address] |BIT4;

         }
         //=======================================================================
         //      Programable DPCD_5
         //=======================================================================
         if(msRegs[REG_DPRX_AUX_DPCD_0E_L+OffsetAux]&(BIT1))   //Programable DPCD_0
         {
             BYTE XDATA uctemp = 0;
             BYTE XDATA ucAuxCommand = 0;
             BYTE XDATA ucAuxLength = 0;
             BYTE XDATA ubTimeOutLimt = 100;
             BYTE XDATA Test[16] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x78};
             WORD XDATA OffSet_Address=0;

             OffSet_Address = OffsetAux;

             ucAuxCommand = msRegs[REG_DPRX_AUX_1C_L+OffSet_Address]&0x0F;
             ucAuxLength =msRegs[REG_DPRX_AUX_1D_L+OffSet_Address];

             msRegs[REG_DPRX_AUX_3C_H+OffSet_Address] = msRegs[REG_DPRX_AUX_3C_H+OffSet_Address] |BIT4;
             msRegs[REG_DPRX_AUX_3C_H+OffSet_Address] = msRegs[REG_DPRX_AUX_3C_H+OffSet_Address] & (~BIT4);


             msRegs[REG_DPRX_AUX_DPCD_0E_H+OffSet_Address] = msRegs[REG_DPRX_AUX_DPCD_4F_L+OffSet_Address] |BIT5;
         }


         //=======================================================================
         //      Programable DPCD_9
         //=======================================================================
         if(msRegs[REG_DPRX_AUX_DPCD_51_H+OffsetAux]&(BIT5))   //Programable DPCD_9
         {
             WORD XDATA OffSet_Address=0;

             OffSet_Address = OffsetAux;

             msRegs[REG_DPRX_AUX_3C_H+OffSet_Address] = msRegs[REG_DPRX_AUX_3C_H+OffSet_Address] |BIT4;
             msRegs[REG_DPRX_AUX_3C_H+OffSet_Address] = msRegs[REG_DPRX_AUX_3C_H+OffSet_Address] & (~BIT4);


             msRegs[REG_DPRX_AUX_DPCD_4F_H+OffSet_Address] = msRegs[REG_DPRX_AUX_DPCD_4F_H+OffSet_Address] |BIT1;
             msRegs[REG_DPRX_AUX_DPCD_4F_H+OffSet_Address] = msRegs[REG_DPRX_AUX_DPCD_4F_H+OffSet_Address] & (~BIT1);
         }
        #endif
    }

}
void DPRXISR(void)
{
    #if 1

        DPRx_ID idata dprx_id;
        DPRx_ID idata dprx_id_Scan;
        DPRx_DECODER_ID idata dprx_decoder_id;
        WORD idata usOffsetByID;
        BYTE idata ubMaskEnable;
        #if DEF_HDCP2RX_ISR_MODE
        BYTE idata ucStatus_103C1C, ucStatus_002BCC, ucStatus_002BCD, ucStatus_002BCE, ucStatus_002BCF;
        #endif

        dprx_id = mapi_DPRx_InputPortToAuxPort(SrcInputType);

        for(dprx_decoder_id = DPRx_DECODER_ID_0; dprx_decoder_id < DPRx_DECODER_ID_MAX; dprx_decoder_id++)
        {
            usOffsetByID = DP_REG_OFFSET100(dprx_decoder_id);
            //=======================================================================
            //     // Audio MN Change
            //=======================================================================
            ubMaskEnable = msRegs[REG_DPRX_DECODER_E0_0D_L + usOffsetByID];
            if((msRegs[REG_DPRX_DECODER_E0_0C_L + usOffsetByID]&(~ubMaskEnable)) != 0x0)
            {
                if(msRegs[REG_DPRX_DECODER_E0_0C_L + usOffsetByID]&(BIT0))   //
                {
                    gDPRxInfo[dprx_id].bDPAUPLLBIGChange[dprx_decoder_id] = TRUE;

                    msRegs[REG_DPTX_ENCODER_E0_1_7D_H] |= BIT1;  //toggle this bit to trigger audio SRAM reset.
                    _nop_();
                    _nop_();
                    _nop_();
                    _nop_();
                    _nop_();
                    msRegs[REG_DPTX_ENCODER_E0_1_7D_H] &= (~BIT1);  //toggle this bit to trigger audio SRAM reset.


                    msRegs[REG_DPRX_DECODER_E0_0F_L + usOffsetByID] = BIT0;
                    _nop_();
                    msRegs[REG_DPRX_DECODER_E0_0F_L + usOffsetByID] = 0;
                }
                //=======================================================================
                //     // Video MN Change
                //=======================================================================

                if(msRegs[REG_DPRX_DECODER_E0_0C_L + usOffsetByID]&(BIT1))   //
                {
                    //gDPRx_2DPTxInfo[dprx_id].TimingChange_RT = TRUE;
                    gDPRxInfo[dprx_id].bDPVPLLBIGChange[dprx_decoder_id] = TRUE;

                    msRegs[REG_DPRX_DECODER_E0_0F_L + usOffsetByID] = BIT1;
                    _nop_();
                    msRegs[REG_DPRX_DECODER_E0_0F_L + usOffsetByID] = 0;
                }

                //=======================================================================
                //     // SDC ECC errir Change
                //=======================================================================

                if(msRegs[REG_DPRX_DECODER_E0_0C_L + usOffsetByID]&(BIT2))   //
                {
                    msRegs[REG_DPRX_DECODER_E0_0F_L + usOffsetByID] = BIT2;
                    _nop_();
                    msRegs[REG_DPRX_DECODER_E0_0F_L + usOffsetByID] = 0;
                }
                //=======================================================================
                //     // Video Buffer overflow
                //=======================================================================
                if(msRegs[REG_DPRX_DECODER_E0_0C_L + usOffsetByID]&(BIT3))   //
                {
                    msRegs[REG_DPRX_DECODER_E0_0F_L + usOffsetByID]=BIT3;
                    _nop_();
                    msRegs[REG_DPRX_DECODER_E0_0F_L + usOffsetByID]=0;
                }
                //=======================================================================
                //     // MSA Change
                //=======================================================================

                if(msRegs[REG_DPRX_DECODER_E0_0C_L + usOffsetByID]&(BIT4))   //
                {
                    gDPRxInfo[dprx_id].bDPVPLLBIGChange[dprx_decoder_id] = TRUE;

                    #if(DP_MST_FUNCTION_SUPPORT == 0x1)
                        if((gDPRxInfo[dprx_id].ubDPVersion > DP_VERSION_11)&&
                            (gDPRxInfo[dprx_id].ubDP_SystemMode == DPRX_SYS_MST_MODE))
                        {
                            if(glDPMSTInfo.bDPMST_StreamPayloadID_CheckFlag == FALSE)
                            {
                                glDPMSTInfo.bDPMST_StreamPayloadID_CheckFlag = TRUE;
                                glDPMSTInfo.usMstCheckStreamValidCounter = DPMST_WAIT_STREAM_VALID_INTERVAL + glDPMSTInfo.ubDPMST_MaxLinkCountTotal*DPMST_STREAM_VALID_TIME_POSITION_UNIT;
                            }
                        }
                    #endif


                    msRegs[REG_DPRX_DECODER_E0_0F_L + usOffsetByID]=BIT4;
                    _nop_();
                    msRegs[REG_DPRX_DECODER_E0_0F_L + usOffsetByID]=0;
                }

                //=======================================================================
                //     // Video Mute
                //=======================================================================
                if(msRegs[REG_DPRX_DECODER_E0_0C_L + usOffsetByID]&(BIT5))   //
                {
                    #if CTS_SINKSTATUS
                    msRegs[REG_DPRX_DPCD1_4D_H + usOffsetByID] &=(~BIT1);
                    #endif
                    msRegs[REG_DPRX_DECODER_E0_0F_L + usOffsetByID]=(BIT5);
                    _nop_();
                    msRegs[REG_DPRX_DECODER_E0_0F_L + usOffsetByID]=0;
                }

                //=======================================================================
                //     // Audio Mute
                //=======================================================================
                if(msRegs[REG_DPRX_DECODER_E0_0C_L + usOffsetByID]&(BIT6))   //
                {
                    #if CTS_SINKSTATUS
                    msRegs[REG_DPRX_DPCD1_4D_H + usOffsetByID] &=(~BIT2);
                    #endif
                    msRegs[REG_DPRX_DECODER_E0_0F_L + usOffsetByID]=(BIT6);
                    _nop_();
                    msRegs[REG_DPRX_DECODER_E0_0F_L + usOffsetByID]=0;
                }
                //=======================================================================
                //    //  audio channel status change
                //=======================================================================

                if(msRegs[REG_DPRX_DECODER_E0_0C_L + usOffsetByID]&(BIT7))   //
                {
                    msRegs[REG_DPRX_DECODER_E0_0F_L + usOffsetByID]=(BIT7);
                    _nop_();
                    msRegs[REG_DPRX_DECODER_E0_0F_L + usOffsetByID]=0;
                }
            }
        }


            //=======================================================================
            //      TP1
            //=======================================================================
            if(msRegs[REG_DPRX_DPCD1_2D_L+DP_REG_OFFSET000(dprx_id)]&(BIT5))   //
            {
                BYTE data DPRx_PhyID = gDPRxInfo[dprx_id].ubComboPhyID;

                usOffsetByID = DP_REG_OFFSET300(DPRx_PhyID);

                msRegs[REG_DPRX_PHY0_29_L + usOffsetByID] = 0x08; //201806, request by analog RD

                gDPRxInfo[dprx_id].bDPTrainingTPS1 = TRUE;
                gDPRxInfo[dprx_id].bDPTrainingFlag = TRUE;
                //=======================================
                msRegs[REG_DPRX_DPCD1_31_L+DP_REG_OFFSET000(dprx_id)]=BIT5;
                msRegs[REG_DPRX_DPCD1_31_L+DP_REG_OFFSET000(dprx_id)]=0;

                #if 1 //0223
                 msRegs[REG_DPRX_RECEIVER_68_H+DP_REG_OFFSET000(dprx_id)] |=BIT0;
                #endif

                #if (DP_MST_FUNCTION_SUPPORT == 0x1)
                if((gDPRxInfo[dprx_id].ubDPVersion > DP_VERSION_11)&&
                    (gDPRxInfo[dprx_id].ubDP_SystemMode == DPRX_SYS_MST_MODE))
                {
                    glDPMSTInfo.bDPMST_ModeDetectEnFlag = TRUE;
                    glDPMSTInfo.usDPMST_ModeDetectCntDown = DPMST_SST_MST_MODE_DETECT_TIME;
                    glDPMSTInfo.bDPMST_SBM_Receive1stSbmFlag = FALSE;

                    mdrv_DPRx_ISR_TPS1ResetRxPayload(dprx_id);
                }
                #endif
            }
            //=======================================================================
            //      TP2
            //=======================================================================
            if(msRegs[REG_DPRX_DPCD1_2D_L + DP_REG_OFFSET000(dprx_id)] & (BIT4))
            {
                gDPRxInfo[dprx_id].bDPTrainingTPS2 = TRUE;
                gDPRxInfo[dprx_id].bDPTrainingFlag = TRUE;
                gDPRxInfo[dprx_id].uwSinkStatusCnt = 100; // For link CTS1.4, item 5.2.2.8 ESI Field Mapping

                // For link CTS1.4, item 5.2.2.8 ESI Field Mapping, overwrite DPCD 205 and 200F
                msRegs[REG_DPRX_DPCD1_4D_H + DP_REG_OFFSET000(dprx_id)] |= BIT0|BIT1|BIT2;

                msRegs[REG_DPRX_DPCD1_31_L + DP_REG_OFFSET000(dprx_id)] = BIT4;
                msRegs[REG_DPRX_DPCD1_31_L + DP_REG_OFFSET000(dprx_id)] = 0;
            }

            //=======================================================================
            //      TP3
            //=======================================================================
            if(msRegs[REG_DPRX_DPCD1_2D_L + DP_REG_OFFSET000(dprx_id)] & (BIT3))
            {
                gDPRxInfo[dprx_id].uwSinkStatusCnt = 100; // For link CTS1.4, item 5.2.2.8 ESI Field Mapping

                // For link CTS1.4, item 5.2.2.8 ESI Field Mapping, overwrite DPCD 205 and 200F
                msRegs[REG_DPRX_DPCD1_4D_H + DP_REG_OFFSET000(dprx_id)] |= BIT0|BIT1|BIT2;

                msRegs[REG_DPRX_DPCD1_31_L + DP_REG_OFFSET000(dprx_id)] = BIT3;
                msRegs[REG_DPRX_DPCD1_31_L + DP_REG_OFFSET000(dprx_id)] = 0;
            }
            //=======================================================================
            //      TP4
            //=======================================================================
            if(msRegs[REG_DPRX_DPCD1_2D_L+DP_REG_OFFSET000(dprx_id)] & (BIT2))
            {
                gDPRxInfo[dprx_id].uwSinkStatusCnt = 100; // For link CTS1.4, item 5.2.2.8 ESI Field Mapping

                // For link CTS1.4, item 5.2.2.8 ESI Field Mapping, overwrite DPCD 205 and 200F
                msRegs[REG_DPRX_DPCD1_4D_H + DP_REG_OFFSET000(dprx_id)] |= BIT0|BIT1|BIT2;

                msRegs[REG_DPRX_DPCD1_31_L + DP_REG_OFFSET000(dprx_id)] = BIT2;
                msRegs[REG_DPRX_DPCD1_31_L + DP_REG_OFFSET000(dprx_id)] = 0;
            }

            //=======================================================================
            //      CDR Loss Lock
            //=======================================================================
            if(msRegs[REG_DPRX_RECEIVER_63_L + DP_REG_OFFSET000(dprx_id)] & (BIT0))
            {
                #if CTS_SINKSTATUS
                msRegs[REG_DPRX_DPCD1_4D_H + DP_REG_OFFSET000(dprx_id)] &= (~(BIT1|BIT2));
                #endif

                gDPRxInfo[dprx_id].bDPLoseCDRLockIRQ = TRUE;
                gDPRxInfo[dprx_id].uwCDRHPDCnt = 30; // For link CTS1.4, item 5.3.2.1 IRQ HPD Pulse Due to Loss of Symbol Lock and Clock Recovery Lock

                msRegs[REG_DPRX_RECEIVER_61_L + DP_REG_OFFSET000(dprx_id)] = BIT0;
                msRegs[REG_DPRX_RECEIVER_61_L + DP_REG_OFFSET000(dprx_id)] = 0;

            }
             //=======================================================================
            //     FAST TRAINING  INT
            //=======================================================================
            if(msRegs[REG_DPRX_RECEIVER_64_L+DP_REG_OFFSET000(dprx_id)]&(BIT6))
            {

                #if 1 //0223
                 msRegs[REG_DPRX_RECEIVER_68_H+DP_REG_OFFSET000(dprx_id)]&=(~BIT0);
                #endif
                msRegs[REG_DPRX_RECEIVER_62_L+DP_REG_OFFSET000(dprx_id)]=BIT6;
                msRegs[REG_DPRX_RECEIVER_62_L+DP_REG_OFFSET000(dprx_id)]=0;

            }
            //=======================================================================
            //      InterlaneLoss
            //=======================================================================
            if(msRegs[REG_DPRX_RECEIVER_63_L+DP_REG_OFFSET000(dprx_id)]&(BIT2))
            {

                gDPRxInfo[dprx_id].bDPInterlaneSkewIRQ = TRUE;
                gDPRxInfo[dprx_id].uwSkewHPDCnt = 1000;
                gDPRxInfo[dprx_id].bLinkRate_FastTraining = 0;

                msRegs[REG_DPRX_RECEIVER_61_L+DP_REG_OFFSET000(dprx_id)]=BIT2;
                msRegs[REG_DPRX_RECEIVER_61_L+DP_REG_OFFSET000(dprx_id)]=0;

            }
              //=======================================================================
            //      InterlaneDone
            //=======================================================================
            if(msRegs[REG_DPRX_RECEIVER_63_L+DP_REG_OFFSET000(dprx_id)]&(BIT3))
            {

                msRegs[REG_DPRX_RECEIVER_61_L+DP_REG_OFFSET000(dprx_id)]=BIT3;
                msRegs[REG_DPRX_RECEIVER_61_L+DP_REG_OFFSET000(dprx_id)]=0;
            }

           //=======================================================================
           //      MST Mode Change (DPCD 0x111 bit0)
           //=======================================================================
           if(msRegs[REG_DPRX_DPCD1_2D_H+DP_REG_OFFSET000(dprx_id)]&(BIT2))    //
           {

               msRegs[REG_DPRX_DPCD1_31_H+DP_REG_OFFSET000(dprx_id)] = BIT2;

               gDPRxInfo[dprx_id].bDPCD111UpdateFlag = TRUE;

               msRegs[REG_DPRX_DPCD1_31_H+DP_REG_OFFSET000(dprx_id)] = 0;
           }
            //=======================================================================
            //      270h
            //=======================================================================
            if(msRegs[REG_DPRX_DPCD1_2D_L+DP_REG_OFFSET000(dprx_id)]&(BIT6))   //
            {
                if(msRegs[REG_DPRX_DPCD0_4B_L+DP_REG_OFFSET000(dprx_id)]&BIT7)
                {
                    msRegs[REG_DPRX_TRANS_CTRL_20_L] =   msRegs[REG_DPRX_TRANS_CTRL_20_L] |BIT5;
                    gDPRxInfo[dprx_id].bDPAutoTestEn = 1;
                }
                else
                {
                    msRegs[REG_DPRX_TRANS_CTRL_20_L] =   msRegs[REG_DPRX_TRANS_CTRL_20_L] &(~BIT5);
                    gDPRxInfo[dprx_id].bDPAutoTestEn = 0;
                }
                //=======================================
                msRegs[REG_DPRX_DPCD1_31_L+DP_REG_OFFSET000(dprx_id)]=BIT6;
                msRegs[REG_DPRX_DPCD1_31_L+DP_REG_OFFSET000(dprx_id)]=0;
                //=======================================
                // msRegs[REG_0016E0] =  msRegs[REG_0016E0] +1;
                //=======================================
            }

            for(dprx_id_Scan = DPRx_ID_0; dprx_id_Scan < DPRx_ID_MAX; dprx_id_Scan++)
           {
                BYTE data DPRx_PhyID = gDPRxInfo[dprx_id_Scan].ubComboPhyID;

                usOffsetByID = DP_REG_OFFSET300(DPRx_PhyID);

                if(!(GET_DPRx_FUNCTION_ENABLE_PORT(dprx_id_Scan)))
                continue;


               //=======================================================================
               //      Lane 0 AEQ Done
               //=======================================================================
               if(msRegs[REG_DPRX_PHY2_44_H + usOffsetByID]&(BIT7))   //
               {
                   //=======================================
                   //=======================================
                   msRegs[REG_DPRX_PHY2_47_H + usOffsetByID] = BIT7;
                   _nop_();
                   msRegs[REG_DPRX_PHY2_47_H + usOffsetByID] = 0;
               }

           }


             //=======================================================================
            //     HDCP14  INT
            //=======================================================================

            if(msRegs[REG_DPRX_DPCD1_2D_L+DP_REG_OFFSET000(dprx_id)]&(BIT7))   //BK_1606, 0x2d[7]: aksv received
            {
                msRegs[REG_DPRX_DPCD1_31_L+DP_REG_OFFSET000(dprx_id)] = BIT7;   //BK_1606, 0x31[7]: clr aksv irq
                msRegs[REG_DPRX_DPCD1_31_L+DP_REG_OFFSET000(dprx_id)] = 0;

                msRegs[REG_DPRX_DPCD0_67_L+DP_REG_OFFSET000(dprx_id)] = BIT3;   //BK_1605, 0x67[3]: clr rdy
                msRegs[REG_DPRX_DPCD0_67_L+DP_REG_OFFSET000(dprx_id)] = BIT1;   //BK_1605, 0x67[1]: clr reauth bit

                msRegs[REG_DPRX_DPCD0_1A_L + DP_REG_OFFSET000(dprx_id)] &= (~(BIT2|BIT3)); //turn off "force HDCP14 link integrity fail:

                msRegs[REG_DPRX_HDCP22_4_4F_L + DP_REG_OFFSET000(dprx_id)] &= (~(BIT4)); //turn off "force HDCP22 link integrity fail
                msRegs[REG_DPRX_HDCP22_4_4F_H + DP_REG_OFFSET000(dprx_id)] |= BIT0;


                //Rx Recv AKSV Clear Rx Mailbox active bit and polling flag
                msRegs[DEF_HDCPMBX_BANK + (DEF_HDCPMBX_CMD_LEN*DEF_HDCPRX_DP_PORT + DEF_HDCPMBX_CMD_BASE)] &= (~BIT7);
                g_stHdcpHandler[DEF_HDCPRX_DP_PORT].bPolling = FALSE; //clear polling flag;

                #if (DPRX_HDCP14_Repeater_ENABLE)
                    //fix r0 be read faster result in mistake clear bPrepare2SetRdyBit in handle
                    g_stHdcpHandler[DEF_HDCPRX_DP_PORT].bPrepare2SetRdyBit = FALSE;
                    gDPRxInfo[dprx_id].bHDCP14R0beRead = FALSE;

                    if(bHDCP14RepPairEnable == TRUE)
                    {
                        g_bHdcp14TxStartAuth = TRUE;
                    }
                #endif
            }

             //=======================================================================
            //     HDCP22  INT
            //=======================================================================

            if(msRegs[REG_DPRX_DPCD1_2D_H+DP_REG_OFFSET000(dprx_id)]&(BIT5))
            {

                //BK_1106, 0x61[13]: dp_hdcp22_irq_clr
                msRegs[REG_DPRX_DPCD1_31_H+DP_REG_OFFSET000(dprx_id)] = BIT5;
                msRegs[REG_DPRX_DPCD1_31_H+DP_REG_OFFSET000(dprx_id)] = 0;

                msRegs[REG_DPRX_DPCD0_1A_L + DP_REG_OFFSET000(dprx_id)] &= (~(BIT2|BIT3)); //turn off "force HDCP14 link integrity fail:

                msRegs[REG_DPRX_HDCP22_4_4F_L + DP_REG_OFFSET000(dprx_id)] &= (~(BIT4)); //turn off "force HDCP22 link integrity fail
                msRegs[REG_DPRX_HDCP22_4_4F_H + DP_REG_OFFSET000(dprx_id)] |= BIT0;


                //HDCP22 Client 1 interrupt Event
                if(msRegs[REG_DPRX_HDCP22_4_70_L+DP_REG_OFFSET000(dprx_id)]&(BIT1))       // BK_160B, 0x70[1]: ake_init_irq_status
                {
                    msRegs[REG_DPRX_HDCP22_4_73_L+DP_REG_OFFSET000(dprx_id)] = BIT1;     // BK_160B, 0x73[1]: ake_init_irq_clr
                    msRegs[REG_DPRX_HDCP22_4_73_L+DP_REG_OFFSET000(dprx_id)] = 0;


                    g_bHdcpRxMsgID = 2;
                    g_bHdcpRxMsgParser = TRUE;

                    #if ((ENABLE_DPTX_OUTPUT) && (DPRX_HDCP2_ENABLE))
                    //trigger repeater tx start Auth to sink
                    if(bHDCP22RepPairEnable == TRUE)
                    {
                        g_bHdcp22TxStartAuth = TRUE;
                    }
                    #endif

                }
                else if(msRegs[REG_DPRX_HDCP22_4_70_L+DP_REG_OFFSET000(dprx_id)]&(BIT2) )       // BK_160B, 0x70[2]:  ake_no_stored_km_status
                {
                    msRegs[REG_DPRX_HDCP22_4_73_L+DP_REG_OFFSET000(dprx_id)] = BIT2;            // BK_160B, 0x73[2]: ake_no_stored_km_clr
                    msRegs[REG_DPRX_HDCP22_4_73_L+DP_REG_OFFSET000(dprx_id)] = 0;

                    g_bHdcpRxMsgID = 4;
                    g_bHdcpRxMsgParser = TRUE;

                }
                else if(msRegs[REG_DPRX_HDCP22_4_70_L+DP_REG_OFFSET000(dprx_id)]&(BIT3))       // BK_160B, 0x70[3]:  ake_stored_km_status
                {
                    msRegs[REG_DPRX_HDCP22_4_73_L+DP_REG_OFFSET000(dprx_id)] = BIT3;            // BK_160B, 0x73[3]: ake_stored_km_clr
                    msRegs[REG_DPRX_HDCP22_4_73_L+DP_REG_OFFSET000(dprx_id)] = 0;

                    g_bHdcpRxMsgID = 5;
                    g_bHdcpRxMsgParser = TRUE;

                }
                else if(msRegs[REG_DPRX_HDCP22_4_70_L+DP_REG_OFFSET000(dprx_id)]&(BIT4))       // BK_160B, 0x70[4]: locality_check_status
                {
                    msRegs[REG_DPRX_HDCP22_4_73_L+DP_REG_OFFSET000(dprx_id)] = BIT4;            // BK_160B, 0x73[4]: locality_check_clr
                    msRegs[REG_DPRX_HDCP22_4_73_L+DP_REG_OFFSET000(dprx_id)] = 0;

                    g_bHdcpRxMsgID = 9;
                    g_bHdcpRxMsgParser = TRUE;

                }
                else if(msRegs[REG_DPRX_HDCP22_4_70_L+DP_REG_OFFSET000(dprx_id)]&(BIT5) )       // BK_160B, 0x70[5]: ske_irq
                {
                    msRegs[REG_DPRX_HDCP22_4_73_L+DP_REG_OFFSET000(dprx_id)] = BIT5;            // BK_160B, 0x73[5]: ske_irq_clr
                    msRegs[REG_DPRX_HDCP22_4_73_L+DP_REG_OFFSET000(dprx_id)] = 0;

                    g_bHdcpRxMsgID = 11;
                    g_bHdcpRxMsgParser = TRUE;
                }
                else if(msRegs[REG_DPRX_HDCP22_4_79_L+DP_REG_OFFSET000(dprx_id)]&(BIT0))       // BK_160B, 0x79[0]: h_pulum_status
                {
                    msRegs[REG_DPRX_HDCP22_4_79_H+DP_REG_OFFSET000(dprx_id)] = BIT0;            // BK_160B, 0x79[8]: h_pulum_clr
                    msRegs[REG_DPRX_HDCP22_4_79_H+DP_REG_OFFSET000(dprx_id)] = 0;

                    g_bHdcpRxMsgID = 0;
                    g_bHdcpRxMsgParser = TRUE;

                }
                else if(msRegs[REG_DPRX_HDCP22_4_79_L+DP_REG_OFFSET000(dprx_id)]&(BIT1))  //v interrupt
                {
                    msRegs[REG_DPRX_HDCP22_4_79_H+DP_REG_OFFSET000(dprx_id)] = BIT1;            // BK_160B, 0x79[9]: v clr
                    msRegs[REG_DPRX_HDCP22_4_79_H+DP_REG_OFFSET000(dprx_id)] = 0;

                    g_bHdcpRxMsgID = 15;
                    g_bHdcpRxMsgParser = TRUE;
                }
                else if(msRegs[REG_DPRX_HDCP22_4_79_L+DP_REG_OFFSET000(dprx_id)]&(BIT2))  //k interrupt
                {
                    msRegs[REG_DPRX_HDCP22_4_79_H+DP_REG_OFFSET000(dprx_id)] = BIT2;            // BK_160B, 0x79[10]: k clr
                    msRegs[REG_DPRX_HDCP22_4_79_H+DP_REG_OFFSET000(dprx_id)] = 0;

                    g_bHdcpRxMsgID = 16;
                    g_bHdcpRxMsgParser = TRUE;
                }
                else
                {
                    g_bHdcpRxMsgID = 0;
                    g_bHdcpRxMsgParser = FALSE;
                }
#if DEF_HDCP2RX_ISR_MODE
                //store current IMI releative setting
                ucStatus_103C1C = MEM_MSREAD_BYTE(REG_103C1C);

                ucStatus_002BCC = MEM_MSREAD_BYTE(REG_002BCC);
                ucStatus_002BCD = MEM_MSREAD_BYTE(REG_002BCD);
                ucStatus_002BCE = MEM_MSREAD_BYTE(REG_002BCE);
                ucStatus_002BCF = MEM_MSREAD_BYTE(REG_002BCF);

                HDCPHandler_ISR_Hdcp2MsgDispatcher(DEF_HDCPRX_DP_PORT);

                //leave interrupt to recovery IMI releative setting
                MEM_MSWRITE_BYTE(REG_103C1C, ucStatus_103C1C);

                MEM_MSWRITE_BYTE(REG_002BCC, ucStatus_002BCC);
                MEM_MSWRITE_BYTE(REG_002BCD, ucStatus_002BCD);
                MEM_MSWRITE_BYTE(REG_002BCE, ucStatus_002BCE);
                MEM_MSWRITE_BYTE(REG_002BCF, ucStatus_002BCF);
#endif
            }

    #endif


}

//**************************************************************************
//  [Function Name]:
//                  mdrv_DPRx_TimerHandler()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void DPRx_TimerIsrHandler(void)
{
    DPRx_ID dprx_id;

    for(dprx_id = DPRx_ID_0; dprx_id < DPRx_ID_MAX; dprx_id++)
    {
        if(GET_DPRx_FUNCTION_ENABLE_PORT(dprx_id))
        {
            if(gDPRxInfo[dprx_id].uwDPRxStableTimeout > 0)
            {
                gDPRxInfo[dprx_id].uwDPRxStableTimeout--;
            }

            if(gDPRxInfo[dprx_id].uwCDRHPDCnt > 0)
            {
                gDPRxInfo[dprx_id].uwCDRHPDCnt--;
            }

            if(gDPRxInfo[dprx_id].uwSkewHPDCnt > 0)
            {
                gDPRxInfo[dprx_id].uwSkewHPDCnt--;
            }

            if(gDPRxInfo[dprx_id].uwSinkStatusCnt > 0)
            {
                if(gDPRxInfo[dprx_id].uwSinkStatusCnt < 2)
                {
                    // For link CTS1.4, item 5.2.2.8 ESI Field Mapping, disable overwrite DPCD 205 and 200F
                    msRegs[REG_DPRX_DPCD1_4D_H + DP_REG_OFFSET000(dprx_id)] &= (~(BIT0|BIT1|BIT2));
                }

                gDPRxInfo[dprx_id].uwSinkStatusCnt--;
            }

            if(gDPRxInfo[dprx_id].uwHDCPCnt > 0)
            {
                gDPRxInfo[dprx_id].uwHDCPCnt--;
            }

            if(gDPRxInfo[dprx_id].ubContDownTimer > 0)
            {
                gDPRxInfo[dprx_id].ubContDownTimer--;
            }

            if(gDPRxInfo[dprx_id].ubContDownTimer2 > 0)
            {
                gDPRxInfo[dprx_id].ubContDownTimer2--;
            }

            if(gDPRxInfo[dprx_id].uwDPStableCount > 0)
            {
                gDPRxInfo[dprx_id].uwDPStableCount--;
            }

            if(gDPRxInfo[dprx_id].ucInterlaceCount> 0)
            {
                gDPRxInfo[dprx_id].ucInterlaceCount--;
            }

            if(gDPRxInfo[dprx_id].bAudioDelayUnmuteCnt> 0)
            {
                gDPRxInfo[dprx_id].bAudioDelayUnmuteCnt--;
            }

            if(gDPRxInfo[dprx_id].bCableDisConnectCnt> 0)
            {
                gDPRxInfo[dprx_id].bCableDisConnectCnt--;
            }

            if(gDPRxInfo[dprx_id].bCableConnectCnt> 0)
            {
                gDPRxInfo[dprx_id].bCableConnectCnt--;
            }

            if(gDPRxInfo[dprx_id].ucHDCP14LongHPDTimer> 0)
            {
                gDPRxInfo[dprx_id].ucHDCP14LongHPDTimer--;
            }


        }

    }

    #if (DP_MST_FUNCTION_SUPPORT == 0x1)
    if(glDPMSTInfo.usDPMST_ModeDetectCntDown > 0)
    {
        glDPMSTInfo.usDPMST_ModeDetectCntDown--;
    }

    if(glDPMSTInfo.usMstSBMBusyCounter > 0)
    {
        glDPMSTInfo.usMstSBMBusyCounter--;
    }

    if(glDPMSTInfo.usMstCheckStreamValidCounter > 0)
    {
        glDPMSTInfo.usMstCheckStreamValidCounter--;
    }

    if(glDPMSTInfo.usMstCheckingNotifyCountDown > 0)
    {
        glDPMSTInfo.usMstCheckingNotifyCountDown--;
    }
    #endif

    return;
}

#endif  //Leo-temp

#if ENABLE_HDMI
//**************************************************************************
//  [Function Name]:
//                  mdrv_hdmiRx_TimerHandler()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mdrv_hdmiRx_TimerHandler(void)
{
    if(stHDMIRxInfo.uc10msCounter > 0)
    {
        stHDMIRxInfo.uc10msCounter--;
    }
    else
    {
        stHDMIRxInfo.uc10msCounter = HDMI_POLLING_INTERVAL;
        stHDMIRxInfo.b10msTimerFlag = TRUE;
    }
}
#endif


#if ENABLE_HK_CODE_ON_PSRAM
// Used for getting end address of DP-related functions located in common bank
void msDrvGetPSramEndInPM(void)
{

}
#endif

