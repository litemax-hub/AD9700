#define _MAILBOX_C
#include "types.h"
#include "board.h"
#include "misc.h"
#include "Debug.h"
#include "Common.h"
#include "Ms_rwreg.h"
#include "REG52.H"
#include "Global.h"
#include "ms_reg.h"
#include "mailbox.h"
#include "mapi_DPRx.h"
#include "usbsw.h"
#include "Ms_PM.h"
#include "msEread.h"


#define MBX_DEBUG    1
#if ENABLE_MSTV_UART_DEBUG && MBX_DEBUG
#define MBX_printData(str, value)   printData(str, value)
#define MBX_printMsg(str)           printMsg(str)
#else
#define MBX_printData(str, value)
#define MBX_printMsg(str)
#endif

#define ANALOG_JTOL_CTS_TEST   0x0
#if MS_PM
extern XDATA sPM_Info  sPMInfo;
#endif
extern BOOL xdata glbIPRxInitHPD_DONE_FLAG;
extern BYTE XDATA glDPRxShortHPDIsrFlag;

#if ENABLE_USB_TYPEC
ST_MBX_DATA st_mbx;
BYTE XDATA mbx_buffer[MBX_BUF_LEN];
BYTE XDATA mbx_idx_rx; // receive index in ISR
BYTE XDATA mbx_idx_st; // process index in mbx handler
BYTE XDATA mbx_cmd_cnt;

BYTE XDATA glSYS_TypeC_PinAssign[2]; //idx 0:PortA, 1:PortB
BYTE XDATA glSYS_CC_Pin[2]; // idx 0:PortA, 1:PortB
BYTE XDATA glSYS_Attach[2]; // idx 0:PortA, 1:PortB, attached status: 0:detached, 1:attached

BYTE XDATA glSYS_TypeC_PostponeHPDFlag;
BYTE XDATA glSYS_TypeC_TriggerHPDFlag;
BYTE XDATA glSYS_TypeC_PortIndex;
BYTE XDATA glSYS_TypeC_HPD_Value;


#if (ENABLE_USB_TYPEC == 0x1)
BYTE XDATA glReg_PHY1_20_L;
BYTE XDATA glReg_PHY1_20_H;
BYTE XDATA glReg_PHY1_22_L;
BYTE XDATA glReg_PHY1_22_H;
BYTE XDATA glReg_PHY1_24_L;
BYTE XDATA glReg_PHY1_24_H;
BYTE XDATA glReg_PHY1_26_L;
BYTE XDATA glReg_PHY1_26_H;
BYTE XDATA glReg_PHY1_26_H;

BYTE XDATA glReg_PHY1_21_L;
BYTE XDATA glReg_PHY1_23_L;
BYTE XDATA glReg_PHY1_25_L;
BYTE XDATA glReg_PHY1_27_L;

BYTE XDATA glReg_PHY1_0E_L;
BYTE XDATA glReg_PHY1_0E_H;

BYTE XDATA glReg_PHY1_16_L;
BYTE XDATA glReg_PHY1_17_L;
BYTE XDATA glReg_PHY1_18_L;
BYTE XDATA glReg_PHY1_19_L;

BYTE XDATA glReg_PHY1_53_L;
BYTE XDATA glReg_PHY1_53_H;
BYTE XDATA glReg_PHY1_54_L;
BYTE XDATA glReg_PHY1_54_H;

BYTE XDATA glReg_PHY1_4C_H;
BYTE XDATA glReg_PHY1_4D_L;

BYTE XDATA glReg_PHY0_22_L;
BYTE XDATA glReg_PHY0_49_H;
BYTE XDATA glReg_PHY0_4E_H;
BYTE XDATA glReg_PHY0_28_L;
BYTE XDATA glReg_PHY0_4C_L;
BYTE XDATA glReg_PHY0_4C_H;

BYTE XDATA glReg_PHY0_2B_L;
BYTE XDATA glReg_PHY0_2B_H;
BYTE XDATA glReg_PHY0_2C_L;
BYTE XDATA glReg_PHY0_2C_H;
#endif

#define TYPEC_PORT_A_AUX_CTRL1     		0x048C  //Aux_CTRL_1
#define TYPEC_PORT_A_AUX_2GPIO_CTRL  	0x048D
#define TYPEC_PORT_B_AUX_CTRL1     		0x0490  //Aux_CTRL_1
#define TYPEC_PORT_B_AUX_2GPIO_CTRL  	0x0491

void halmbx_set_hkr2_busy_status(BOOL ben)
{
    msWriteByte(REG_MBX_HKR2_BUSY_STATUS, ben ? 0x01: 0x00);
}

void halmbx_set_pd51_busy_status(BOOL ben)
{
    msWriteByte(REG_MBX_PD51_BUSY_STATUS, ben ? 0x01: 0x00);
}

BYTE halmbx_get_to_pd51_busy_status(void)
{
    WORD XDATA u16Count=0;
    while(msReadByte(REG_MBX_PD51_BUSY_STATUS))
    {
        ForceDelay1ms(1);
        if(u16Count++ >2000) //implicitly wait PD51 booting done.
            break;
    }

    return (msReadByte(REG_MBX_PD51_BUSY_STATUS));
}

void MBX_init(void)
{
    BYTE xdata i;

    for(i=0; i<0xff; i++)// 0xff is reserved for WDT, Init_WDT function will init its value
    {
        msWriteByte(REG_MBX_PD51_TO_HKR2_CMD_ID+i, 0x00);
    }
}

void drvmbx_receive_cmd_isr(void)
{
    BYTE data i, size;

    size = MEM_MSREAD_BYTE(REG_MBX_PD51_TO_HKR2_CMD_LEN);
    if(size < MBX_CMD_MAX_LEN)
    {
        mbx_buffer[mbx_idx_rx] = MEM_MSREAD_BYTE(REG_MBX_PD51_TO_HKR2_CMD_ID);
        mbx_buffer[(mbx_idx_rx+1)%MBX_BUF_LEN] = size;
        for(i=0; i<size;i++)
        {
            mbx_buffer[(mbx_idx_rx+2+i)%MBX_BUF_LEN] = MEM_MSREAD_BYTE(REG_MBX_PD51_TO_HKR2_CMD_DATA_START+i);
        }

        if( ((mbx_idx_st > mbx_idx_rx)&&((mbx_idx_st-mbx_idx_rx/*-size-2*/)>=MBX_CMD_MAX_LEN/*+2*/)) // roughly keep enough buffer size for next cmd
          ||((mbx_idx_st <= mbx_idx_rx)&&((MBX_BUF_LEN-mbx_idx_rx+mbx_idx_st)>=MBX_CMD_MAX_LEN)))
        {
            MEM_MSWRITE_BYTE(REG_MBX_HKR2_BUSY_STATUS, 0x00);
        }

        mbx_idx_rx = (mbx_idx_rx+2+size)%MBX_BUF_LEN;
        mbx_cmd_cnt++;
    }
}

BOOL drvmbx_send_cmd(EN_MBX_COMMAND_IDX ucCMD, BYTE ucCMDSize, BYTE *pu8CMDData)
{
    BYTE xdata i;
    BOOL bRet = TRUE;

    if(halmbx_get_to_pd51_busy_status())
    {
        MBX_printMsg("[MBX]TO PD51 MBX busy");
        bRet =FALSE;
    }
    if(ucCMDSize>MBX_CMD_MAX_LEN)
    {
        MBX_printMsg("[MBX]MBX_SEND_ERR CMD size over buffer");
        return FALSE;
    }

    halmbx_set_pd51_busy_status(TRUE);

    msWriteByte(REG_MBX_HKR2_TO_PD51_CMD_ID, ucCMD);
    msWriteByte(REG_MBX_HKR2_TO_PD51_CMD_LEN, ucCMDSize);
    for(i=0; i<ucCMDSize; ++i)
    {
        msWriteByte(REG_MBX_HKR2_TO_PD51_CMD_DATA_START+i, pu8CMDData[i] );
    }
    msDrvMcuTrigIntToPD51();
    return bRet;
}

BOOL drvmbx_send_rcmd(EN_MBX_COMMAND_IDX ucCMD, BYTE ucCMDSize, BYTE *pu8CMDData, BYTE *pu8ReadData)
{
    BYTE xdata i;
    WORD xdata u16Count=0;
    BOOL bRet = TRUE;

    msWriteByte(REG_MBX_HKR2_TO_PD51_CMD_READ_SIZE, 0);
    bRet = drvmbx_send_cmd(ucCMD, ucCMDSize, pu8CMDData);

    while(!msReadByte(REG_MBX_HKR2_TO_PD51_CMD_READ_SIZE))
    {
        ForceDelay1ms(1);
        if(u16Count++ >2000) //implicitly wait PD51 booting done.
        {
            MBX_printMsg("[MBX]MBX_SEND_RCMD timeout!");
            return FALSE;
        }
    }

    u16Count = msReadByte(REG_MBX_HKR2_TO_PD51_CMD_READ_SIZE);
    for(i=0; i<u16Count; i++)
    {
        pu8ReadData[i] = msReadByte(REG_MBX_HKR2_TO_PD51_CMD_READ_DATA+i);
    }

    return bRet;
}

BOOL drvmbx_send_HPD_Ctrl_CMD(BYTE u8Ctrl, BYTE u8TypeCPortIdx)
{
    BYTE u8CmdData[2];

	if((glbIPRxInitHPD_DONE_FLAG == FALSE)&&
		((u8Ctrl &BIT0) != 0x0)) //for SYSTEMIP_HPD_HIGH, SYSTEMIP_HPD_IRQ_HPDHIGH
	{
		glSYS_TypeC_PostponeHPDFlag = TRUE;
		glSYS_TypeC_HPD_Value = u8Ctrl;
		glSYS_TypeC_PortIndex = u8TypeCPortIdx;
		return TRUE;
	}

	if((u8TypeCPortIdx == 0) || (u8TypeCPortIdx == 1))
	{
		if((u8Ctrl == 0x0) || (u8Ctrl == 0x1))
		{
		    #if (CHIP_ID == CHIP_MT9701)
			mapi_DPRx_HPDControl(Input_UsbTypeC3, u8Ctrl);
            #else
			mapi_DPRx_HPDControl(Input_UsbTypeC4, u8Ctrl);
            #endif
		}
	}


    u8CmdData[0] = u8Ctrl;
    u8CmdData[1] = u8TypeCPortIdx;
    MBX_printData("[MBX]--Set HPD 0x%x--", u8Ctrl);
    MBX_printData("[MBX]--TypeCPort %d--", u8TypeCPortIdx);
    return drvmbx_send_cmd(EN_DISPLAYPORT_to_PD_CMD_HPD_CTRL, 2, &u8CmdData[0]);
}

BOOL drvmbx_send_MCU_Speed_Update_CMD(BYTE u8SpeedIdx)
{
    BYTE u8CmdData[1];
    BYTE u8Rdata[1];

    u8CmdData[0] = u8SpeedIdx;

    MBX_printMsg("[MBX]--Set MCU Speed Update Ack--");
    return drvmbx_send_rcmd(EN_HK_to_PD_CMD_MCU_SPEED_UPDATE, 1, &u8CmdData[0], u8Rdata); // rcmd as ack to make sure the CPU speed synced.
}

BOOL drvmbx_send_PM_Status_Update_CMD(BYTE u8PMStatus, BYTE u8TypeCMode)
{
    BYTE u8CmdData[2];
    u8CmdData[0] = u8PMStatus;
    u8CmdData[1] = u8TypeCMode;

    MBX_printData("[MBX]--Set PM Status %d--", u8PMStatus);
    MBX_printData("[MBX]--Set PM TypeC %d--", u8TypeCMode);
    return drvmbx_send_cmd(EN_HK_to_PD_CMD_PM_STATUS_UPDATE, 2, &u8CmdData[0]);
}

BOOL drvmbx_send_Multi_Func_Config_CMD(BYTE u8Enable)
{
    MBX_printData("[MBX]--Set MultiFunc Cfg %d--", u8Enable);
    return drvmbx_send_cmd(EN_HK_to_PD_CMD_MULTI_FUNC_CONFIG, 1, &u8Enable);
}

BOOL drvmbx_send_Video_TypeC_Port_CMD(BYTE u8TypeCPortIdx)
{
    //MBX_printData("[MBX]--Set Acvite TypeC Port %d--", u8TypeCPortIdx);
    return drvmbx_send_cmd(EN_HK_to_PD_CMD_VIDEO_TYPEC_PORT, 1, &u8TypeCPortIdx);
}

BOOL drvmbx_send_Upll_Off(BYTE u8Off)
{
    MBX_printData("[MBX]--Set Upll off %d--", u8Off);
    return drvmbx_send_cmd(EN_HK_to_PD_CMD_UPLL_OFF, 1, &u8Off);
}

// pu8CcIdle: 1: CC is in idle state, 0: otherwise
BOOL drvmbx_send_CC_IDLE_RCMD(BYTE *pu8CcIdle)
{
    BYTE u8Rtn = 0;
    BYTE u8Rdata[1] = {0};

    u8Rtn = drvmbx_send_rcmd(EN_HK_to_PD_RCMD_CC_IDLE, 0, 0, u8Rdata);
    *pu8CcIdle = u8Rdata[0];
    return u8Rtn;
}

BOOL drvmbx_send_BuBst_Config_CMD(BYTE u8Config)
{
    MBX_printData("[MBX]--Set buck-boost Cfg 0x%x--", u8Config);
    return drvmbx_send_cmd(EN_HK_to_PD_CMD_BUBST_CONFIG, 1, &u8Config);
}

BOOL drvmbx_send_BB_Disconnect_CMD(BYTE u8Enable)
{
    return drvmbx_send_cmd(EN_HK_to_PD_CMD_BB_DISCONNECT, 1, &u8Enable);
}

void MBX_TPC_EnterDPAltModeCheck(void)
{
	#if (ENABLE_USB_TYPEC == 0x1)
    BYTE xdata u8PortIdx = 0;
    BYTE xdata ubTypeC_InputType = Input_UsbTypeC4;

    if(SrcInputType == Input_UsbTypeC3)
    {
        u8PortIdx = st_mbx.u8CmdData[2];
        ubTypeC_InputType = Input_UsbTypeC3; //PortA
    }
    #if (CHIP_ID==CHIP_MT9700)
    else if(SrcInputType == Input_UsbTypeC4)
    {
        u8PortIdx = st_mbx.u8CmdData[2];
        ubTypeC_InputType = Input_UsbTypeC4; //PortB

		if((glSYS_TypeC_PinAssign[u8PortIdx] == BIT2)||(glSYS_TypeC_PinAssign[u8PortIdx] == BIT3))//Pin Assign C, D case
        {
			if(glSYS_CC_Pin[u8PortIdx] == 0x1)  //Flip plug
			{
                //STEP 1 for USB TPC issue
                if(glSYS_TypeC_PinAssign[u8PortIdx] == BIT2)
                {
					//mapi_DPRx_SetTypeCPowerDownControl(ubTypeC_Port, 0xE4, TRUE);
                    //mapi_DPRx_DELAY_NOP(ubPhy_OnOff_Delaytime);
                }
                else if(glSYS_TypeC_PinAssign[u8PortIdx] == BIT3)
                {
					//mapi_DPRx_SetTypeCPowerDownControl(ubTypeC_Port, 0xE4, TRUE);
                }



				mapi_DPRx_SetLaneSwapEnable(ubTypeC_InputType,  TRUE,  0x39);
                #if(ENABLE_INTERNAL_CC == 1)
				mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, FALSE,  TRUE,  0x4);
                #else
				mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, TRUE,  TRUE,  0x4);
                #endif


			}
			else if(glSYS_CC_Pin[u8PortIdx] == 0x0)
			{
				//STEP 1 for USB TPC issue
                if(glSYS_TypeC_PinAssign[u8PortIdx] == BIT2)
                {
                    //mapi_DPRx_SetTypeCPowerDownControl(ubTypeC_Port, 0x1B, TRUE);
                }
                else if(glSYS_TypeC_PinAssign[u8PortIdx] == BIT3)
                {
                    //mapi_DPRx_SetTypeCPowerDownControl(ubTypeC_Port, 0x1B, TRUE);
                }


				mapi_DPRx_SetLaneSwapEnable(ubTypeC_InputType,  TRUE,  0x6C);
                #if(ENABLE_INTERNAL_CC == 1)
                mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, TRUE,  TRUE,  0x2);
                #else
                mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, FALSE,  TRUE,  0x2);
                #endif

			}


			#if(ANALOG_JTOL_CTS_TEST == 0x1)
			MBX_printMsg("-------Analog JTOL Test stop !!------- \r\n");
			while(1);
			#endif


        }
        else if(glSYS_TypeC_PinAssign[u8PortIdx] == BIT4) // Pin Assign E case
        {


            if(glSYS_CC_Pin[u8PortIdx] == 0x1)  //Flip plug
            {
				mapi_DPRx_SetLaneSwapEnable(ubTypeC_InputType, TRUE, 0x93);
                #if(ENABLE_INTERNAL_CC == 1)
				mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, TRUE,  TRUE,  0xE);
                #else
				mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, FALSE,  TRUE,  0xE);
                #endif
            }
            else if(glSYS_CC_Pin[u8PortIdx] == 0x0)
            {
				mapi_DPRx_SetLaneSwapEnable(ubTypeC_InputType, TRUE, 0xC6);
                #if (ENABLE_INTERNAL_CC == 1)
                mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, FALSE,  TRUE,  0x7);
                #else
                mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, TRUE,  TRUE,  0x7);
                #endif
            }
        }

    }
    #endif
    else
    {
        if(INPUT_IS_DISPLAYPORT(SrcInputType) )
        {
            #if (CHIP_ID==CHIP_MT9700)
            mapi_DPRx_SetLaneSwapEnable(ubTypeC_InputType, FALSE, 0xE4);
            #elif (CHIP_ID==CHIP_MT9701)
            mapi_DPRx_SetPinAssignSettings(ubTypeC_InputType, FALSE, 0xE4);
            #endif
            mapi_DPRx_SetLanePNSwapEnable(SrcInputType, FALSE, FALSE,  FALSE,  0x0);
            mapi_DPRx_SetLaneCount(SrcInputType, 0x4);  // 4 lane
        }
    }
    // T.B.D
	#endif

    return;
}

void MBX_handler(void)
{
    BYTE xdata i, idx_rx_tmp, u8PortIdx;
	BYTE xdata ubTypeC_InputType;
	BYTE xdata ucHPDValue = 0x0;

	#if (ENABLE_USB_TYPEC == 0x1)
	static BOOL bAcOnInitDone = FALSE;

	if(bAcOnInitDone == FALSE)
	{
		bAcOnInitDone = TRUE;

		// T.B.D
	}

	if(glSYS_TypeC_PostponeHPDFlag  == TRUE)
	{
		if(glSYS_TypeC_TriggerHPDFlag == TRUE)
		{
			drvmbx_send_HPD_Ctrl_CMD(glSYS_TypeC_HPD_Value, glSYS_TypeC_PortIndex);
			glSYS_TypeC_TriggerHPDFlag = FALSE;
			glSYS_TypeC_PostponeHPDFlag = FALSE;
            //MBX_printMsg("-------[MBX]Rx HPD pstpone set H");
		}
	}

    if((glDPRxShortHPDIsrFlag == TRUE)||(mapi_DPRx_GetTypeCShortHPDEvent(SrcInputType) == TRUE))
    {
		if((SrcInputType == Input_UsbTypeC3)&&(glSYS_CC_Pin[1] != 0x2)) //port index = 0x0 for port A
		{
			drvmbx_send_HPD_Ctrl_CMD(SYSTEMIP_HPD_IRQ_HPDHIGH, 1);

		}
		else if((SrcInputType == Input_UsbTypeC4)&&(glSYS_CC_Pin[1] != 0x2)) //port index = 0x1 for port B
		{
			drvmbx_send_HPD_Ctrl_CMD(SYSTEMIP_HPD_IRQ_HPDHIGH, 1);
		}
        mapi_DPRx_ClrTypeCShortHPDEvent(SrcInputType);
		glDPRxShortHPDIsrFlag = FALSE;
        MBX_printMsg("-----[MBX]Rx Short HPD");
    }


	if(mapi_DPRx_GetTypeCHPDEvent(SrcInputType, &ucHPDValue) == TRUE)
	{
		if((SrcInputType == Input_UsbTypeC3)&&(glSYS_CC_Pin[1] != 0x2)) //port index = 0x0 for port A
		{
			if(ucHPDValue == 0x0)
			{
				drvmbx_send_HPD_Ctrl_CMD(SYSTEMIP_HPD_LOW, 1);
			}
			else
			{
				drvmbx_send_HPD_Ctrl_CMD(SYSTEMIP_HPD_HIGH, 1);
			}
		}
		else if((SrcInputType == Input_UsbTypeC4)&&(glSYS_CC_Pin[1] != 0x2)) //port index = 0x1 for port B
		{
			if(ucHPDValue == 0x0)
			{
				drvmbx_send_HPD_Ctrl_CMD(SYSTEMIP_HPD_LOW, 1);
			}
			else
			{
				drvmbx_send_HPD_Ctrl_CMD(SYSTEMIP_HPD_HIGH, 1);
			}
		}

		mapi_DPRx_ClrTypeCHPDEvent(SrcInputType);

        MBX_printData("-------[MBX]Rx IP Ctrl HPD%d ", ucHPDValue);
	}

	#endif

    #if (ANALOG_JTOL_CTS_TEST == 0x0)
    if(mbx_cmd_cnt)
    #endif
    {
        MBX_CNT_MINUS_1(mbx_cmd_cnt);

        st_mbx.u8CommandId = mbx_buffer[mbx_idx_st];
        st_mbx.u8Length = mbx_buffer[(mbx_idx_st+1)%MBX_BUF_LEN];
        if(st_mbx.u8Length>MBX_CMD_MAX_LEN)
        {
            MBX_printMsg("[MBX]MBX_ERR CMD size over buffer");
            return;
        }
        for(i=0; i<st_mbx.u8Length;i++)
        {
            st_mbx.u8CmdData[i] = mbx_buffer[(mbx_idx_st+2+i)%MBX_BUF_LEN];
            //MBX_printData("[MBX]data: 0x%x", st_mbx.u8CmdData[i]);
        }

        st_mbx.u8CmdEmpty=FALSE;

        idx_rx_tmp = mbx_idx_rx; // for ISR value change
        if( ((mbx_idx_st > idx_rx_tmp)&&((mbx_idx_st-idx_rx_tmp/*-size-2*/)>=MBX_CMD_MAX_LEN/*+2*/)) // roughly keep enough buffer size for next cmd
          ||((mbx_idx_st <= idx_rx_tmp)&&((MBX_BUF_LEN-idx_rx_tmp+mbx_idx_st)>=MBX_CMD_MAX_LEN)))
        {
            halmbx_set_hkr2_busy_status(FALSE);
        }
        mbx_idx_st = (mbx_idx_st+2+st_mbx.u8Length)%MBX_BUF_LEN;

        #if (ANALOG_JTOL_CTS_TEST == 0x1)
        st_mbx.u8CommandId = EN_PD_to_DISPLAYPORT_CMD_SET_PIN_ASSGN;
        st_mbx.u8CmdData[1] = BIT2;
        st_mbx.u8CmdData[0] = 1;
        #endif

        switch(st_mbx.u8CommandId)
        {
            case EN_PD_to_DISPLAYPORT_CMD_SET_PIN_ASSGN:
            {
                MBX_printData("[MBX]CC pin =%x \r\n", st_mbx.u8CmdData[0]);
                MBX_printData("[MBX]Pin Assignment=%x (0x4: pin C, 0x8: pin D, 0x10: pinE), \r\n", st_mbx.u8CmdData[1]);
                MBX_printData("[MBX]Port A/B =%x \r\n", st_mbx.u8CmdData[2]);

			    #if 0//xunru msdbg temp for DPC

			    msWriteByte(0x152673 , 0x11);
			    msWriteByte(0x15267b , 0x89);
			    msWriteByte(0x152678 , 0x89);
			    msWriteByteMask(0x0020D8 , 0, BIT2);
			    msWriteByteMask(0x002011 , 0, BIT0|BIT1|BIT2|BIT3);
			    msWriteByteMask(0x002010 , BIT1, BIT0|BIT1|BIT2);
			    msWriteByteMask(0x002013 , BIT0, BIT0);
			    msWriteByteMask(0x002010 , BIT4|BIT5, BIT4|BIT5);

				msWrite2ByteMask(REG_DPRX_PHY0_10_L + 0x400, 0x0180,0x180); // Initial script by OG
				msWrite2ByteMask(REG_DPRX_PHY0_4D_L + 0x400, 0x07FF,0x07FF); // Initial script by OG
			    #endif

                u8PortIdx = st_mbx.u8CmdData[2];
                glSYS_TypeC_PortIndex = st_mbx.u8CmdData[2];
                glSYS_CC_Pin[u8PortIdx] = st_mbx.u8CmdData[0];
                glSYS_TypeC_PinAssign[u8PortIdx] = st_mbx.u8CmdData[1];
#if((CHIP_ID == CHIP_MT9701) && U3_REDRV_PM_MODE)
                if(glSYS_CC_Pin[u8PortIdx] == 0x2) // typeC port with USB3
                {
                    g_bRedrU3Status = g_bHubU3Status = 0;
                }
#endif

                // Dynamically Config DPC R-term
                //msDrvDPCRtermTrimCfg(glSYS_CC_Pin[u8PortIdx], glSYS_TypeC_PinAssign[u8PortIdx]);
                msDPCPadSwitchCfg(glSYS_CC_Pin[u8PortIdx], glSYS_TypeC_PinAssign[u8PortIdx]);
                msDPCRtermCfg(glSYS_CC_Pin[u8PortIdx], glSYS_TypeC_PinAssign[u8PortIdx]);

                #if (CHIP_ID==CHIP_MT9701)
				ubTypeC_InputType = Input_UsbTypeC3; //init
                #else
				ubTypeC_InputType = Input_UsbTypeC4; //init
                #endif

				mapi_DPRx_SetTypeCInfo(ubTypeC_InputType, glSYS_CC_Pin[u8PortIdx], glSYS_TypeC_PinAssign[u8PortIdx]);
                #if (CHIP_ID==CHIP_MT9701)
				mapi_DPRx_SetPinAssignSettings(ubTypeC_InputType, TRUE, 0xFF);
                #endif

                if(st_mbx.u8CmdData[1]==0x8)//PinD
                {
                    mapi_DPRx_SetEQ_Current(ubTypeC_InputType ,true);
                }
                else//PinC/E
                {
                    mapi_DPRx_SetEQ_Current(ubTypeC_InputType, false);
                }
                if((glSYS_TypeC_PinAssign[u8PortIdx] == 0x0)||(glSYS_CC_Pin[u8PortIdx] == 0x2))
                {
                    drvmbx_send_HPD_Ctrl_CMD(SYSTEMIP_HPD_LOW, u8PortIdx);
                    MBX_printMsg("[MBX]Set HPD 0 \r\n");

					Init_TYPEC_B_AUX_TRI();
					Init_TYPEC_B_AUX_P();
					Init_TYPEC_B_AUX_N();

                    //mapi_DPRx_SetTypeCPowerDownControl(ubTypeC_Port, 0x0, FALSE);
					mapi_DPRx_TypeC_CableDisconect(ubTypeC_InputType);
                }
                else if((glSYS_TypeC_PinAssign[u8PortIdx] == BIT2)||(glSYS_TypeC_PinAssign[u8PortIdx] == BIT3))//Pin Assign C, D case
                {
					if(glSYS_CC_Pin[u8PortIdx] == 0x1)  //Flip plug
					{
						hw_Clr_TYPEC_B_AUX_P();
						hw_Set_TYPEC_B_AUX_N();
						hw_Set_TYPEC_B_AUX_TRI();

                        //STEP 1 for USB TPC issue
                        #if (CHIP_ID==CHIP_MT9700)
                        if(glSYS_TypeC_PinAssign[u8PortIdx] == BIT2)
                        {
                            if(sPMInfo.ucPMMode == ePM_POWERON )
                            {
                                mapi_DPRx_SetPhyPowerDownControl(ubTypeC_InputType, sPMInfo.ucPMMode);
                            }
                            //mapi_DPRx_DELAY_NOP(ubPhy_OnOff_Delaytime);
                        }
                        else if(glSYS_TypeC_PinAssign[u8PortIdx] == BIT3)
                        {
                            if(sPMInfo.ucPMMode == ePM_POWERON )
                            {
                                mapi_DPRx_SetPhyPowerDownControl(ubTypeC_InputType, sPMInfo.ucPMMode);
                            }
                        }

						if(SrcInputType == Input_UsbTypeC4)
						{
    						mapi_DPRx_SetLaneSwapEnable(ubTypeC_InputType,  TRUE,  0x39);
                            #if(ENABLE_INTERNAL_CC == 1)
                            mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, FALSE,  TRUE,  0x4);
                            #else
	    					mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, TRUE,  TRUE,  0x4);
                            #endif
						}
						else
						{
						    #if(ENABLE_INTERNAL_CC == 1)
                            mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, FALSE,  FALSE,  0x4);
                            #else
							mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, TRUE,  FALSE,  0x4);
                            #endif
						}
                        #endif

					}
					else if(glSYS_CC_Pin[u8PortIdx] == 0x0)
					{

						hw_Set_TYPEC_B_AUX_P();
						hw_Clr_TYPEC_B_AUX_N();
						hw_Set_TYPEC_B_AUX_TRI();

                        //STEP 1 for USB TPC issue
                        #if (CHIP_ID==CHIP_MT9700)
                        if(glSYS_TypeC_PinAssign[u8PortIdx] == BIT2)
                        {
                            if(sPMInfo.ucPMMode == ePM_POWERON )
                            {
                                mapi_DPRx_SetPhyPowerDownControl(ubTypeC_InputType, sPMInfo.ucPMMode);
                            }
                        }
                        else if(glSYS_TypeC_PinAssign[u8PortIdx] == BIT3)
                        {
                            if(sPMInfo.ucPMMode == ePM_POWERON )
                            {
                                mapi_DPRx_SetPhyPowerDownControl(ubTypeC_InputType, sPMInfo.ucPMMode);
                            }
                        }

						if(SrcInputType == Input_UsbTypeC4)
						{
    						mapi_DPRx_SetLaneSwapEnable(ubTypeC_InputType,  TRUE,  0x6C);
                            #if(ENABLE_INTERNAL_CC == 1)
	                        mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, TRUE,  TRUE,  0x2);
                            #else
	                        mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, FALSE,  TRUE,  0x2);
                            #endif
						}
						else
						{
						    #if(ENABLE_INTERNAL_CC == 1)
	                        mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, TRUE,  FALSE,  0x2);
                            #else
						    mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, FALSE,  FALSE,  0x2);
                            #endif
						}
                        #endif
					}

                    if(sPMInfo.ucPMMode == ePM_POWEROFF)
                    {
                        MBX_printMsg("[MBX]DCOff HPD Keeping @ pinCD \r\n");
                    }
                    else
                    {
                        MBX_printMsg("[MBX]Set HPD pin CD \r\n");
                        drvmbx_send_HPD_Ctrl_CMD(SYSTEMIP_HPD_HIGH, u8PortIdx);
                    }

                    //---------------------------------------
    				//For pin Assign D, we need to declare DP 2 lane only
    				//---------------------------------------
    				if(glSYS_TypeC_PinAssign[u8PortIdx] == 0x8)
    				{
    					mapi_DPRx_SetLaneCount(ubTypeC_InputType, 0x2); // 2 lane
    				}
    				else
    				{
    					mapi_DPRx_SetLaneCount(ubTypeC_InputType, 0x4);  // 4 lane
    				}

					#if(ANALOG_JTOL_CTS_TEST == 0x1)
					MBX_printMsg("-------Analog JTOL Test stop !!------- \r\n");
					while(1);
					#endif
                }
                else if(glSYS_TypeC_PinAssign[u8PortIdx] == BIT4) // Pin Assign E case
                {


                    if(glSYS_CC_Pin[u8PortIdx] == 0x1)  //Flip plug
                    {
						hw_Set_TYPEC_B_AUX_P();
						hw_Clr_TYPEC_B_AUX_N();
						hw_Set_TYPEC_B_AUX_TRI();

                        #if (CHIP_ID==CHIP_MT9700)
                        if(sPMInfo.ucPMMode == ePM_POWERON )
                        {
                            mapi_DPRx_SetPhyPowerDownControl(ubTypeC_InputType, sPMInfo.ucPMMode);
                        }

						if(SrcInputType == Input_UsbTypeC4)
						{
    						mapi_DPRx_SetLaneSwapEnable(ubTypeC_InputType, TRUE, 0x93);
                            #if(ENABLE_INTERNAL_CC == 1)
    						mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, TRUE,  TRUE,  0xE);
                            #else
    						mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, FALSE,  TRUE,  0xE);
                            #endif
						}
						else
						{
						    #if(ENABLE_INTERNAL_CC == 1)
    						mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, TRUE,  FALSE,  0xE);
                            #else
							mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, FALSE,  FALSE,  0xE);
                            #endif
						}
                        #endif
                    }
                    else if(glSYS_CC_Pin[u8PortIdx] == 0x0)
                    {

						hw_Clr_TYPEC_B_AUX_P();
						hw_Set_TYPEC_B_AUX_N();
						hw_Set_TYPEC_B_AUX_TRI();

                        #if (CHIP_ID==CHIP_MT9700)
                        if(sPMInfo.ucPMMode == ePM_POWERON )
                        {
                            mapi_DPRx_SetPhyPowerDownControl(ubTypeC_InputType, sPMInfo.ucPMMode);
                        }

						if(SrcInputType == Input_UsbTypeC4)
						{
                            mapi_DPRx_SetLaneSwapEnable(ubTypeC_InputType, TRUE, 0xC6);
                            #if(ENABLE_INTERNAL_CC == 1)
                            mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, FALSE,  TRUE,  0x7);
                            #else
                            mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, TRUE,  TRUE,  0x7);
                            #endif
						}
						else
						{
						    #if(ENABLE_INTERNAL_CC == 1)
                            mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, FALSE,  FALSE,  0x7);
                            #else
							mapi_DPRx_SetLanePNSwapEnable(ubTypeC_InputType, TRUE, TRUE,  FALSE,  0x7);
                            #endif
						}
                        #endif

                    }

                    if(sPMInfo.ucPMMode == ePM_POWEROFF)
                    {
                        MBX_printMsg("[MBX]DCOff HPD Keeping @ pinE \r\n");
                    }
                    else
                    {
                        MBX_printMsg("[MBX]Set HPD pinE \r\n");
                        drvmbx_send_HPD_Ctrl_CMD(SYSTEMIP_HPD_HIGH, u8PortIdx);
                    }

                    mapi_DPRx_SetLaneCount(ubTypeC_InputType, 0x4);  // 4 lane

                }

				MBX_printMsg("[MBX]Set GPIO X40X41_PortB\r\n");
            }
            	break; // case EN_PD_to_DISPLAYPORT_CMD_SET_PIN_ASSGN:

            case EN_PD_to_HK_CMD_ATTACH_STATUS:
                {
                    MBX_printData("[MBX]Attach =%x \r\n", st_mbx.u8CmdData[1]);
                    MBX_printData("[MBX]Port A/B =%x \r\n", st_mbx.u8CmdData[0]);
                    glSYS_Attach[st_mbx.u8CmdData[0]] = st_mbx.u8CmdData[1];
                }
                break; // case EN_PD_to_HK_CMD_ATTACH_STATUS:

            case EN_PD_to_HK_CMD_CUSTOM:
                // Custom command handler here.
                MBX_printMsg("[MBX]Custom cmd.");
                break;

            default:
                MBX_printData("\r\n[MBX]CMD is not defined = %x", st_mbx.u8CommandId);
                break;
        }
        //MBX_printMsg("[MBX]PD to HK mbx!!!!!!\n");
    }
}
BOOL mapi_CC_GetAttachStatus(BYTE u8TypeCPortIdx)
{
    //idx 0:PortA, 1:PortB, attached status: 0:detached, 1:attached
    return ((u8TypeCPortIdx <= 1) ? glSYS_Attach[u8TypeCPortIdx] : 0);

}
#endif

