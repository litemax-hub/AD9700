///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mhal_DPRx.c
/// @author MStar Semiconductor Inc.
/// @brief  DP Rx driver Function
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MHAL_DPRX_C_
#define _MHAL_DPRX_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <string.h>
#include "dpCommon.h"
#include "Ms_rwreg.h"
#include "drvDDC2Bi.h"
#include "mhal_DPRx.h"
#include "mhal_DPRx_phy.h"
#include "msEread.h"

BOOL ubHaveBeenSleeped[DPRx_AUX_ID_MAX] = {0};

#if (ENABLE_DP_INPUT == 0x1)
//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define DPRX_HAL_DEBUG_MESSAGE			0

#if (ENABLE_MSTV_UART_DEBUG && DPRX_HAL_DEBUG_MESSAGE)
#define DPRX_HAL_DPUTSTR(str)			printMsg(str)
#define DPRX_HAL_DPRINTF(str, x)		printData(str, x)
#else
#define DPRX_HAL_DPUTSTR(str)
#define DPRX_HAL_DPRINTF(str, x)
#endif

#define VSC_EXT_SW						1

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
BYTE XDATA ucDPFunctionEnableIndex = 0;
BYTE XDATA ucDPTPCFunctionEnableIndex = 0;

DPRx_stDPAudioInfo XDATA stAudioInfo;
BYTE XDATA AUDIOSTREAM_USED_RX[2] = {DPRx_ID_MAX, DPRx_ID_MAX};

BYTE XDATA glLastRxID = 0xFF;

extern BYTE tCOMBO_HDCP14_BKSV[COMBO_HDCP_BKSV_SIZE];

BYTE glTypeC_MacPNswap[DPRx_ID_MAX] = {0};
BYTE glTypeC_PhyLaneSwap[DPRx_ID_MAX] = {0};

extern BYTE XDATA g_ucDDC_TxRecord;
extern BYTE XDATA dpNullMsg[3];
extern BYTE XDATA u8DDC_RxRecord;

WORD usInfo1 = 0;
BYTE ubInfo2 = 0;

extern BYTE glubRx_XDataQueue[DPRx_ID_MAX][DPRx_XDATAPROGRAM_DPCD_MAX][DPRX_XDataQueue];
DPRx_MCCS_WAKEUP glubMCCS_Wakeup[DPRx_AUX_ID_MAX] = {DPRx_MCCS_MAX};

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_DELAY_NOP()
//  [Description]
//					mhal_DPRx_DELAY_NOP
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_DELAY_NOP(DWORD msNums)
{
	while(msNums--)
    {
		_nop_();
	}

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_DELAY_1ms()
//  [Description]
//					mhal_DPRx_DELAY_1ms
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_DELAY_1ms(void)
{
    DWORD msNums = 31120;

	while(msNums--)
    {
		_nop_();
	}

	return;
}

void ________INIT________(void);
//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_Initial()
//  [Description]
//					mhal_DPRx_Initial
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_Initial(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, DPRx_PHY_ID dprx_phy_id)
{
	if((dprx_id == DPRx_ID_MAX) || (dprx_aux_id == DPRx_AUX_ID_MAX) || (dprx_phy_id == DPRx_PHY_ID_MAX))
	{
		printf("[DPRx][ERROR] initial wrong id!!\r\n");
		return;
	}

    mhal_DPRx_SetPowerDownControl(dprx_id, dprx_aux_id, DP_ePM_POWERON);
    mhal_DPRx_AuxClockEnable(dprx_aux_id);
	mhal_DPRx_AuxInitialSetting(dprx_id, dprx_aux_id);
    mhal_DPRx_PHYInitialSetting(dprx_id, dprx_phy_id);
	mhal_DPRx_ReceiverInitialSetting(dprx_id);
	mhal_DPRx_SetFastTrainingTime(dprx_id, DP_RX_FT_TIMER_A_WAIT_SQ_SIGNAL);
	mhal_DPRx_FastTrainingInitialSetting(dprx_id);
	mhal_DPRx_TopGpInitialSetting();

    #if(DPRX_HDCP2_ENABLE == 0x1)
    mhal_DPRx_HDCP2InitialSetting(dprx_id, TRUE);
    mhal_DPRx_HDCP2SetRxCaps(dprx_id, TRUE);
    #else
    mhal_DPRx_HDCP2SetRxCaps(dprx_id, FALSE);
    #endif

    #if(DPRX_HDCP14_Repeater_ENABLE == 0x1)
	mhal_DPRx_HDCP14RepeaterEnable(dprx_id, dprx_aux_id, TRUE);
    #endif

	mhal_DPRx_OUIInitialSetting(dprx_aux_id);

	mhal_DPRx_SetProgrammableDPCDEnable(dprx_aux_id, TRUE);

    #if DP_XDATA_PROGRAMMABLE_DPCD_MCCS
    mhal_DPRx_EnableMCCSXDATAProgrammableDPCD(dprx_aux_id, FALSE);
    #endif

    mhal_DPRx_EnableAuxInterrupt(dprx_aux_id, TRUE);
    mhal_DPRx_EnableDPAUXRecvInterrupt(dprx_aux_id, FALSE);
	mhal_DPRx_EnablePHYInterrupt(dprx_phy_id, FALSE);

	mhal_DPRx_SetAuxIsel(dprx_aux_id);

    if(ubInfo2 > 1)
    {
        mhal_DPRx_InitialSetting_3(dprx_id, dprx_aux_id);
    }

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_Initial_Decoder()
//  [Description]
//					mhal_DPRx_Initial_Decoder
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_Initial_Decoder(DPRx_DECODER_ID dprx_decoder_id)
{
	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

	mhal_DPRx_PLLInitialSetting(dprx_decoder_id);
	mhal_DPRx_DecodeInitialSetting(dprx_decoder_id);

	mhal_DPRx_UpdateMVIDValue(dprx_decoder_id);
	mhal_DPRx_UpdateMAUDValue(dprx_decoder_id);

	mhal_DPRx_SetAUPLLBigChangeInterrupt(dprx_decoder_id, TRUE);
	mhal_DPRx_SetVPLLBigChangeInterrupt(dprx_decoder_id, TRUE);
	mhal_DPRx_SetSDCInterrupt(dprx_decoder_id, FALSE);
	mhal_DPRx_SetVideoBufferOverflowInterrupt(dprx_decoder_id, FALSE);
	mhal_DPRx_SetMSAChgInterrupt(dprx_decoder_id, TRUE);
	mhal_DPRx_SetVideoMuteInterrupt(dprx_decoder_id, FALSE);
	mhal_DPRx_SetAudioMuteInterrupt(dprx_decoder_id, FALSE);
	mhal_DPRx_SetAudioChanneStatusChgInterrupt(dprx_decoder_id, FALSE);
	mhal_DPRx_SetMISC0ChangeInterrupt(dprx_decoder_id, TRUE);
	mhal_DPRx_SetMISC1ChangeInterrupt(dprx_decoder_id, TRUE);

    mhal_DPRx_DecodeInitialSetting_3(dprx_decoder_id);

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_InitialSetting_3()
//  [Description]
//					mhal_DPRx_InitialSetting_3
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_InitialSetting_3(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id)
{
	WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);

	if((dprx_id == DPRx_ID_MAX) || (dprx_aux_id == DPRx_AUX_ID_MAX))
	{
		return;
	}
    msWriteByteMask(REG_DPRX_AUX_7C_L + usRegOffsetAuxByID, BIT1, BIT1); // SETTING3.09
	msWriteByteMask(REG_DPRX_AUX_7C_L + usRegOffsetAuxByID, BIT2, BIT2); // SETTING3.11

	msWriteByteMask(REG_DPRX_RECEIVER_68_H + usRegOffsetReceiverByID, BIT0, BIT0); // SETTING3.67

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_DecodeInitialSetting_3()
//  [Description]
//					mhal_DPRx_DecodeInitialSetting_3
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_DecodeInitialSetting_3(DPRx_DECODER_ID dprx_decoder_id)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);
	WORD usRegOffsetSdpByID = DP_REG_OFFSET100(dprx_decoder_id);

	msWriteByteMask(REG_DPRX_DECODER_E0_08_L + usRegOffsetDecoderByID, BIT1, BIT1); // SETTING3.15

	msWriteByteMask(REG_DPRX_SDP_E0_00_H + usRegOffsetSdpByID, BIT7, BIT7); // SETTING3.12

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_AuxInitialSetting()
//  [Description]
//					mhal_DPRx_AuxInitialSetting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_AuxInitialSetting(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id)
{
	WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
	WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET000(dprx_id);
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET000(dprx_id);
	DWORD XDATA usTimeOutCount = 0;

	if((dprx_id == DPRx_ID_MAX) || (dprx_aux_id == DPRx_AUX_ID_MAX))
	{
		return;
	}

	// Copy BKSV
	#if (ASIC_Verification == 0x1)
	msWriteByte(REG_DPRX_DPCD1_22_L + usRegOffsetDPCD1ByID, tCOMBO_HDCP14_BKSV[0]); // BKSV // Move to R2
    msWriteByte(REG_DPRX_DPCD1_22_H + usRegOffsetDPCD1ByID, tCOMBO_HDCP14_BKSV[1]); // BKSV
    msWriteByte(REG_DPRX_DPCD1_23_L + usRegOffsetDPCD1ByID, tCOMBO_HDCP14_BKSV[2]); // BKSV
    msWriteByte(REG_DPRX_DPCD1_23_H + usRegOffsetDPCD1ByID, tCOMBO_HDCP14_BKSV[3]); // BKSV
    msWriteByte(REG_DPRX_DPCD1_24_L + usRegOffsetDPCD1ByID, tCOMBO_HDCP14_BKSV[4]); // BKSV
    msWriteByte(REG_DPRX_AUX_DPCD_2A_L + usRegOffsetAuxDPCDByID, tCOMBO_HDCP14_BKSV[0]); // BKSV // Move to R2
    msWriteByte(REG_DPRX_AUX_DPCD_2A_H + usRegOffsetAuxDPCDByID, tCOMBO_HDCP14_BKSV[1]); // BKSV
    msWriteByte(REG_DPRX_AUX_DPCD_2B_L + usRegOffsetAuxDPCDByID, tCOMBO_HDCP14_BKSV[2]); // BKSV
    msWriteByte(REG_DPRX_AUX_DPCD_2B_H + usRegOffsetAuxDPCDByID, tCOMBO_HDCP14_BKSV[3]); // BKSV
    msWriteByte(REG_DPRX_AUX_DPCD_2C_L + usRegOffsetAuxDPCDByID, tCOMBO_HDCP14_BKSV[4]); // BKSV
    #endif

	msWriteByteMask(REG_DPRX_AUX_7E_L + usRegOffsetAuxByID, BIT3, BIT3); // Offline reply setting
    msWriteByteMask(REG_DPRX_AUX_DPCD_7E_L + usRegOffsetAuxDPCDByID, 0x0F, 0x0F);
    msWriteByteMask(REG_DPRX_AUX_54_L + usRegOffsetAuxByID, BIT2|BIT6, BIT2|BIT6); // Enable AUX

   	msWriteByteMask(REG_DPRX_AUX_7D_H + usRegOffsetAuxByID, BIT0, BIT0); // Illegal address reply NACK

	// Illegal I2C address NACK reply, 0: disable -> over 400us, 1: enable -> under 400us
	msWriteByteMask(REG_DPRX_AUX_7D_H + usRegOffsetAuxByID, BIT2, BIT2);
    msWriteByteMask(REG_DPRX_AUX_18_H + usRegOffsetAuxByID, BIT1, BIT0|BIT1); // AUX time-out reply DEFER, 0: ACK, 1: NACK, 2: DEFER

    msWriteByteMask(REG_DPRX_AUX_57_L + usRegOffsetAuxByID, 0, BIT0|BIT1|BIT2); // Disble DP event to PM
    msWriteByteMask(REG_DPRX_AUX_54_H + usRegOffsetAuxByID, 0, BIT4|BIT5); // R-term
    msWriteByteMask(REG_DPRX_AUX_21_L + usRegOffsetAuxByID, BIT0, BIT0); // DPCD 00030 enable

	#if (FPGA_Verification == 0x1)
	msWriteByteMask(REG_DPRX_AUX_DPCD_04_L + usRegOffsetAuxDPCDByID, BIT4|BIT6|BIT7, BIT4|BIT5|BIT6|BIT7); // XTAL clk -> 14.318M
	#else
    msWriteByteMask(REG_DPRX_AUX_DPCD_04_L + usRegOffsetAuxDPCDByID, BIT4|BIT5|BIT7, BIT4|BIT5|BIT6|BIT7); // XTAL clk -> 12M
    #endif

    msWrite2Byte(REG_DPRX_AUX_0A_L + usRegOffsetAuxByID, 0xBB8); // Time out

    msWriteByteMask(REG_DPRX_AUX_01_L + usRegOffsetAuxByID, 0, BIT5);  // DPCD 0000D

    #if (ENABLE_DP_AUDIO_DPCD == 0x1)
    msWriteByteMask(REG_DPRX_AUX_01_H + usRegOffsetAuxByID, BIT3, BIT3); // DPCD 0004[0] NORP = 1 --> support video & audio
    msWriteByteMask(REG_DPRX_AUX_05_L + usRegOffsetAuxByID, BIT2, BIT2); // DPCD 000A[2] ASSOCIATED_TO_PRECEDING_PORT of RECEIVE_PORT1_CAP_0 = 1
    #else
    msWriteByteMask(REG_DPRX_AUX_01_H + usRegOffsetAuxByID, 0, BIT3); // DPCD 0004[0] NORP = 0 --> only support video
    msWriteByteMask(REG_DPRX_AUX_05_L + usRegOffsetAuxByID, 0, BIT2); // DPCD 000A[2] ASSOCIATED_TO_PRECEDING_PORT of RECEIVE_PORT1_CAP_0 = 0
    #endif

    msWriteByteMask(REG_DPRX_AUX_7A_L + usRegOffsetAuxByID, BIT4, BIT4); // Reply timing -> New mode
    msWrite2Byte(REG_DPRX_AUX_7B_L + usRegOffsetAuxByID, DP_AUX_DELAY_NORMAL); // 53us delay reply

    msWriteByteMask(REG_DPRX_DPCD1_48_L + usRegOffsetDPCD1ByID, BIT1, BIT1); // Bcaps
    msWriteByteMask(REG_DPRX_AUX_DPCD_21_L + usRegOffsetAuxDPCDByID, BIT2, BIT2); // PM Bcaps

    msWriteByteMask(REG_DPRX_DPCD0_3F_L + usRegOffsetDPCD0ByID, 0x01, 0x7F); // Sink count
    msWriteByteMask(REG_DPRX_DPCD0_3F_L + usRegOffsetDPCD0ByID, BIT7, BIT7); // CP_ready

	msWriteByteMask(REG_DPRX_DPCD0_3A_L + usRegOffsetDPCD0ByID, 0, BIT1);  // For link CTS1.4, item 5.2.2.5 DPCD Address Range (QD980)
	msWriteByteMask(REG_DPRX_AUX_51_L + usRegOffsetAuxByID, 0, BIT0|BIT1); // For link CTS1.4, item 5.2.2.5 DPCD Address Range (UCD-400)

	// For link CTS1.4, item 5.2.2.5 DPCD Address Range, eDP reserved content should be 0
	msWrite2Byte(REG_DPRX_AUX_DPCD1_10_L + usRegOffsetAuxDPCDByID, 0x0000);
	msWrite2Byte(REG_DPRX_AUX_DPCD1_11_L + usRegOffsetAuxDPCDByID, 0x0000);
	msWrite2Byte(REG_DPRX_AUX_DPCD1_12_L + usRegOffsetAuxDPCDByID, 0x0000);
	msWrite2Byte(REG_DPRX_AUX_DPCD1_13_L + usRegOffsetAuxDPCDByID, 0x0000);
	msWrite2Byte(REG_DPRX_AUX_DPCD1_14_L + usRegOffsetAuxDPCDByID, 0x0000);
	msWrite2Byte(REG_DPRX_AUX_DPCD1_15_L + usRegOffsetAuxDPCDByID, 0x0000);
	msWrite2Byte(REG_DPRX_AUX_DPCD1_16_L + usRegOffsetAuxDPCDByID, 0x0000);
	msWrite2Byte(REG_DPRX_AUX_DPCD1_17_L + usRegOffsetAuxDPCDByID, 0x0000);

    msWriteByteMask(REG_DPRX_AUX_DPCD_03_L + usRegOffsetAuxDPCDByID, 0, BIT6|BIT7); // 2210[5:4] HDR support

    msWriteByteMask(REG_DPRX_DPCD1_7C_L + usRegOffsetDPCD1ByID, 0, BIT0); // For DPCD 004FE ~ 004FF reply

	msWriteByteMask(REG_DPRX_AUX_DPCD_64_H + usRegOffsetAuxDPCDByID, BIT7, BIT7); // Default set 1 to enable some DP1.4 DPCD R/W, ex: DPCD 00060

	msWriteByteMask(REG_DPRX_AUX_7D_L + usRegOffsetAuxByID, 0, BIT2); // reg_aux_rx_oen_set_ov_mode

	msWriteByteMask(REG_DPRX_AUX_DPCD_67_H + usRegOffsetAuxByID, 0, BIT0|BIT1|BIT2|BIT3); // Disable FEC capability

	// PD_VCM_OP
	msWriteByteMask(REG_DPRX_AUX_DPCD_61_L + usRegOffsetAuxDPCDByID, 0, BIT7);
    msWriteByteMask(REG_DPRX_AUX_2A_L + usRegOffsetAuxByID, 0, BIT0);
    msWriteByteMask(REG_DPRX_AUX_DPCD_60_H + usRegOffsetAuxDPCDByID, BIT1, BIT1);
    glubMCCS_Wakeup[dprx_aux_id] = DPRx_MCCS_MAX;

	// Reset async FIFO
	while(usTimeOutCount < DP_AUX_TIMEOUT_CNT)
	{
		usTimeOutCount++;

		if((mhal_DPRx_CheckAuxIdle(dprx_aux_id) == TRUE) && (mhal_DPRx_CheckAuxPhyIdle(dprx_aux_id) == TRUE)) // Aux idle
		{
			break;
		}
	}

	mhal_DPRx_ResetAsyncFIFO(dprx_id, dprx_aux_id); // ResetAsyncFIFO need after clock gating

	mhal_DPRx_OfflinePortLinkTrainingSetting(dprx_aux_id, Faketrain_swing, Faketrain_pre);

	mhal_DPRx_SetOffLine(dprx_id, dprx_aux_id, TRUE); // Multi-port 1-phy init as TRUE to default turn-on offline

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ReceiverInitialSetting()
//  [Description]
//					mhal_DPRx_ReceiverInitialSetting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_ReceiverInitialSetting(DPRx_ID dprx_id)
{
    WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET000(dprx_id);
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);
    WORD usRegOffsetReceiverFECByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	// Initial script by CS
	msWriteByteMask(REG_DPRX_TRANS_CTRL_01_L + usRegOffsetTransCTRLByID, 0x0F, 0x0F); // DP receiver clkgen enable

    msWriteByteMask(REG_DPRX_TRANS_CTRL_12_L + usRegOffsetTransCTRLByID, 0, BIT7); // Check SQH

    //===================================================
    //	EQ initial value patch (DLEV)
    //===================================================
	msWriteByteMask(REG_DPRX_TRANS_CTRL_12_H + usRegOffsetTransCTRLByID, 0, BIT0); // 0: Check SQH or DLEV // 1: Check SQH and DLEV
	msWriteByteMask(REG_DPRX_TRANS_CTRL_12_L + usRegOffsetTransCTRLByID, BIT7, BIT7|BIT6); // [6] Check DLEV // [7] Check SQH

	msWriteByteMask(REG_DPRX_RECEIVER_49_L + usRegOffsetReceiverByID, 0, BIT4); // For link CTS1.4, item 500.1.3 Error Count Test (UCD-400)

    msWriteByteMask(REG_DPRX_RECEIVER_35_H + usRegOffsetReceiverByID, BIT0, BIT0); // Issue: Decode VDE/HDE unstable, auto reset clk_gated_fifo

    // For BK1602_3C[4] = 1 (SW mode)
    // If BK1602_3C[5] = 0 --> Do de-interlaneskew by BS
    // If BK1602_3C[5] = 1 --> Do de-interlaneskew by SR
    // For BK1602_3C[4] = 0 (HW mode)
    // If normal training --> Do de-interlaneskew by BS
    // Others --> Do de-interlaneskew by BS
    msWriteByteMask(REG_DPRX_RECEIVER_3C_L + usRegOffsetReceiverByID, BIT4, BIT4); // reg_change_deinterlaneskew_kcode_to_sr_ov_en

    msWriteByteMask(REG_DPRX_RECEIVER_2F_H + usRegOffsetReceiverByID, 0, BIT1); //TX send idle pattern with MSA will cause Rx descrambler error //reg_en_auto_descrambler_normal_train should always be off
    msWriteByteMask(REG_DPRX_RECEIVER_FEC_30_L + usRegOffsetReceiverFECByID, BIT6, BIT6); // Because of TSUMR2 not support FEC, so we need to bypass FEC decoder

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_FastTrainingInitialSetting()
//  [Description]
//					mhal_DPRx_FastTrainingInitialSetting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_FastTrainingInitialSetting(DPRx_ID dprx_id)
{
	WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_TRANS_CTRL_07_L + usRegOffsetTransCTRLByID, 0, BIT4); // Bypass timer_a

	msWriteByte(REG_DPRX_TRANS_CTRL_08_L + usRegOffsetTransCTRLByID, 0x5); // Time for link rate and lane count switch (us)
	msWriteByte(REG_DPRX_TRANS_CTRL_09_L + usRegOffsetTransCTRLByID, 0xA); // Time for link rate and lane count switch (ms)
	msWriteByteMask(REG_DPRX_TRANS_CTRL_08_H + usRegOffsetTransCTRLByID, 0, BIT2); // Bypass timer_b
	msWriteByteMask(REG_DPRX_TRANS_CTRL_09_H + usRegOffsetTransCTRLByID, 0, BIT2); // Bypass timer_c

	msWriteByte(REG_DPRX_TRANS_CTRL_0A_L + usRegOffsetTransCTRLByID, 0x64); // Time for update final link rate and lane count to DPCD 100/101 (ms)
	msWriteByteMask(REG_DPRX_TRANS_CTRL_0A_H + usRegOffsetTransCTRLByID, 0, BIT2); // Bypass timer_d

	//msWriteByteMask(REG_DPRX_TRANS_CTRL_06_L + usRegOffsetTransCTRLByID, BIT3, BIT3); // Support 10G link rate
    msWriteByteMask(REG_DPRX_TRANS_CTRL_06_L + usRegOffsetTransCTRLByID, 0, BIT4); // Disable 6.75G link rate to speed up fast training
	msWriteByteMask(REG_DPRX_TRANS_CTRL_06_L + usRegOffsetTransCTRLByID, 0, BIT3); // Disable 10G link rate fast training, suggested by JL

	// For FPGA verification
	#if (FPGA_Verification == 0x1)
	msWriteByteMask(REG_DPRX_TRANS_CTRL_06_L + usRegOffsetTransCTRLByID, BIT1, BIT1); // overwrite SQH from PHY enable
	msWriteByteMask(REG_DPRX_TRANS_CTRL_06_L + usRegOffsetTransCTRLByID, BIT6, BIT6); // HPD disconnect fix
	msWriteByteMask(REG_DPRX_TRANS_CTRL_30_L + usRegOffsetTransCTRLByID, 0, BIT3); // Disable auto EQ from MAC
	#endif

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_TopGpInitialSetting()
//  [Description]
//					mhal_DPRx_TopGpInitialSetting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_TopGpInitialSetting(void)
{
    BYTE i = 0;

	for(i = 0; i < 0xB; i++)
	{
		msWrite2Byte(REG_DPRX2_TOP_GP_00_L + i*2, 0xFFFF); // RX2 GP TOP
	}

	msWriteByteMask(REG_DPRX2_TOP_GP_32_L, 0, BIT0); // Unmask decoder IRQ

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_OUIInitialSetting()
//  [Description]
//					mhal_DPRx_OUIInitialSetting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_OUIInitialSetting(DPRx_AUX_ID dprx_aux_id)
{
    DWORD XDATA ulDPCDAddress = 0;
    BYTE XDATA ucDATA;

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    ulDPCDAddress = 0x400;
    ucDATA = IEEE_OUI_FIRST;
    mhal_DPRx_MCUWriteOUIDPCD(dprx_aux_id, ulDPCDAddress, ucDATA);

    ulDPCDAddress = 0x401;
    ucDATA = IEEE_OUI_SECOND;
    mhal_DPRx_MCUWriteOUIDPCD(dprx_aux_id, ulDPCDAddress, ucDATA);

    ulDPCDAddress = 0x402;
    ucDATA = IEEE_OUI_THIRD;
    mhal_DPRx_MCUWriteOUIDPCD(dprx_aux_id, ulDPCDAddress, ucDATA);

    ulDPCDAddress = 0x403;
    ucDATA = DEVICE_STRING_0;
    mhal_DPRx_MCUWriteOUIDPCD(dprx_aux_id, ulDPCDAddress, ucDATA);

    ulDPCDAddress = 0x404;
    ucDATA = DEVICE_STRING_1;
	mhal_DPRx_MCUWriteOUIDPCD(dprx_aux_id, ulDPCDAddress, ucDATA);

    ulDPCDAddress = 0x405;
    ucDATA = DEVICE_STRING_2;
    mhal_DPRx_MCUWriteOUIDPCD(dprx_aux_id, ulDPCDAddress, ucDATA);

    ulDPCDAddress = 0x406;
    ucDATA = DEVICE_STRING_3;
    mhal_DPRx_MCUWriteOUIDPCD(dprx_aux_id, ulDPCDAddress, ucDATA);

    ulDPCDAddress = 0x407;
    ucDATA = DEVICE_STRING_4;
    mhal_DPRx_MCUWriteOUIDPCD(dprx_aux_id, ulDPCDAddress, ucDATA);

    ulDPCDAddress = 0x408;
    ucDATA = DEVICE_STRING_5;
    mhal_DPRx_MCUWriteOUIDPCD(dprx_aux_id, ulDPCDAddress, ucDATA);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_PLLInitialSetting()
//  [Description]
//					mhal_DPRx_PLLInitialSetting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_PLLInitialSetting(DPRx_DECODER_ID dprx_decoder_id)
{
	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

	// Clock enable
	msWrite2Byte(REG_DPRX_PLL_00_L, 0xFFFF);
	msWrite2Byte(REG_DPRX_PLL_01_L, 0xFFFF);
	msWrite2Byte(REG_DPRX_PLL_02_L, 0xFFFF);

	switch(dprx_decoder_id)
	{
		case DPRx_DECODER_ID_0:
			msWriteByte(REG_DPRX_PLL_50_H, 0); // VPLL

			mhal_DPRx_AUPLLSetting();
			break;

		default:
			break;
	}

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_DecodeInitialSetting()
//  [Description]
//					mhal_DPRx_DecodeInitialSetting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_DecodeInitialSetting(DPRx_DECODER_ID dprx_decoder_id)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

	msWrite2Byte(REG_DPRX_DECODER_E0_0D_L + usRegOffsetDecoderByID, 0xFFFF); // Mask interrupt

    msWrite2Byte(REG_DPRX_DECODER_E0_12_L + usRegOffsetDecoderByID, 0x10); // For Maud range
    msWrite2Byte(REG_DPRX_DECODER_E0_13_H + usRegOffsetDecoderByID, 0);

    msWrite2Byte(REG_DPRX_DECODER_E0_16_L + usRegOffsetDecoderByID, 0x10); // For Mvid range
    msWrite2Byte(REG_DPRX_DECODER_E0_17_H + usRegOffsetDecoderByID, 0);

    msWrite2Byte(REG_DPRX_DECODER_E0_1A_L + usRegOffsetDecoderByID, 0x10); // For Naud range
    msWrite2Byte(REG_DPRX_DECODER_E0_1B_H + usRegOffsetDecoderByID, 0);

    msWrite2Byte(REG_DPRX_DECODER_E0_1E_L + usRegOffsetDecoderByID, 0x10); // For Nvid range
    msWrite2Byte(REG_DPRX_DECODER_E0_1F_H + usRegOffsetDecoderByID, 0);

    msWrite2Byte(REG_DPRX_DECODER_E0_58_L + usRegOffsetDecoderByID, 0x0025); // For decoder CRC config

    if(ubInfo2 > 1)
    {
        // Only for TSUMR2 SETTING2
        msWriteByteMask(REG_DPRX_DECODER_E0_28_H + usRegOffsetDecoderByID, BIT1|BIT7, BIT1|BIT7); // Enable adjust the time of MN update when receive MSA
    }

    msWriteByteMask(REG_DPRX_DECODER_E0_2B_H + usRegOffsetDecoderByID, 0, BIT4|BIT3); // [12] reg_vbid6_fall_chk_vbid0 // [11] reg_vbid6_rise_chk_vbid0

    msWriteByte(REG_DPRX_DECODER_E0_0A_H + usRegOffsetDecoderByID, 6); // reg_v_stable_cnt

    msWriteByteMask(REG_DPRX_DECODER_E0_40_L + usRegOffsetDecoderByID, 4, BIT0|BIT1|BIT2); // reg_fr_th

    msWriteByteMask(REG_DPRX_DECODER_E0_61_L + usRegOffsetDecoderByID, BIT1, BIT1); // Fix received HDE < MSA issue

	if(ubInfo2 < 2)
	{
		// Only for TSUMR2
		// Due to HW restriction, we need to enable MN update value
		msWriteByteMask(REG_DPRX_DECODER_E0_28_L + usRegOffsetDecoderByID, BIT7, BIT7);
	}

	msWriteByteMask(REG_DPRX_DECODER_E0_42_L + usRegOffsetDecoderByID, BIT0|BIT4|BIT5, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6); // For HTT learn

	msWriteByteMask(REG_DPRX_DECODER_E0_79_L + usRegOffsetDecoderByID, BIT1, BIT1); // Enable to assert the last Hsync in V-blanking for freesync mode

	msWriteByteMask(REG_DPRX_DECODER_E0_01_H + usRegOffsetDecoderByID, BIT1|BIT2, BIT1|BIT2); // [9] reg_rst_dec_no_stream // [10] reg_sup_se_replaced_by_be_sst_mode

	// For FPGA verification
	msWriteByteMask(REG_DPRX_DECODER_E0_59_L + usRegOffsetDecoderByID, BIT6, BIT5|BIT6); // 4P -> 1P

	#if(ASIC_Verification == 0x1)
    // For real CHIP, decoder 00_H should be set
    msWriteByteMask(REG_DPRX_DECODER_E0_00_H + usRegOffsetDecoderByID, 0, BIT0|BIT1); // 1P mode
    #endif

    msWriteByteMask(REG_DPRX_DECODER_E0_0A_L + usRegOffsetDecoderByID, BIT2, BIT2); // M value determined by MSA M[23:8] & VBID M[7:0]

    msWriteByteMask(REG_DPRX_DECODER_E0_67_L + usRegOffsetDecoderByID, BIT1|BIT0, BIT1|BIT0); // Enable HSP/HSW/VSP/VSW overwrite

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_VersionSetting()
//  [Description]
//					mhal_DPRx_VersionSetting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_VersionSetting(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, BYTE ucVersion)
{
	WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
	WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);

	if((dprx_id == DPRx_ID_MAX) || (dprx_aux_id == DPRx_AUX_ID_MAX) || (ucVersion == DP_VERSION_MAX))
	{
		return;
	}

	// Initialize DPCD 0x2200 ~ 0x2210
	mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_02200, 0x00);
	mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_02201, 0x00);
	mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_02202, 0x00);
	mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_02203, 0x00);
	mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_02204, 0x00);
	mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_02205, 0x00);
	mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_02206, 0x00);
	mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_02207, 0x00);
	mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_02208, 0x00);
	mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_02209, 0x00);
	mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_0220A, 0x00);
	mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_0220B, 0x00);
	mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_0220C, 0x00);
	mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_0220D, 0x00);
	mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_0220E, 0x00);
	mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_0220F, 0x00);
	mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_02210, 0x00);

	msWriteByte(REG_DPRX_AUX_00_L + usRegOffsetAuxByID, ucVersion);	  // DPCD minor revision number
	msWriteByteMask(REG_DPRX_AUX_01_H + usRegOffsetAuxByID, 0, BIT1); // EQ training pattern 3 supported
	msWriteByteMask(REG_DPRX_AUX_7E_H + usRegOffsetAuxByID, 0, BIT2); // EQ training pattern 4 supported
	msWriteByteMask(REG_DPRX_AUX_02_L + usRegOffsetAuxByID, BIT0, BIT0); // OUI support
	msWriteByteMask(REG_DPRX_AUX_02_H + usRegOffsetAuxByID, BIT0, BIT0|BIT7); // DPCD 2200 extended DPCD
	msWriteByteMask(REG_DPRX_AUX_7E_L + usRegOffsetAuxByID, 0, BIT7); // DPCD 2210[3] for VSC
	msWriteByteMask(REG_DPRX_AUX_DPCD_72_L + usRegOffsetAuxDPCDByID, 0, BIT7); 	  // EQ training pattern 4 supported
	msWriteByteMask(REG_DPRX_AUX_DPCD_31_H + usRegOffsetAuxDPCDByID, BIT0, BIT0); // DPCD 2200 sync

	if(ucVersion == DP_VERSION_11)
	{
		printf("========== DP 1.1 ==========\r\n");

		mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_00001, DP_LINKRATE_HBR); // Maximum link rate supported
	}
	else if(ucVersion == DP_VERSION_12)
	{
		printf("========== DP 1.2 ==========\r\n");

	    #if (FPGA_Verification == 0x1)
		mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_00001, DP_LINKRATE_RBR); // Maximum link rate supported
        #else
		// Note: MT9700 default set as max link rate HBR only, SA can change the define for HBR2  ENABLE_DP_RX_MAX_LINKRATE_HBR2
		mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_00001, DP_LINKRATE_HBR); // Maximum link rate supported
	    #endif
		#if (DPRX_SUPPORT_TP3 == 0x1)
		msWriteByteMask(REG_DPRX_AUX_01_H + usRegOffsetAuxByID, BIT1, BIT1); // EQ training pattern 3 supported
		if(ubHaveBeenSleeped[dprx_aux_id])
		{
			msWriteByteMask(REG_DPRX_AUX_DPCD_71_H + usRegOffsetAuxDPCDByID, mhal_DPRx_GetDPCDValueByRIU(dprx_aux_id, DPCD_00002), 0x1F); // Max lane count supported
		}
		else
		{
			msWriteByteMask(REG_DPRX_AUX_DPCD_71_H + usRegOffsetAuxDPCDByID, DP_LANECOUNT_4, 0x1F); // Max lane count supported
		}
		msWriteByteMask(REG_DPRX_AUX_DPCD_71_H + usRegOffsetAuxDPCDByID, BIT6, BIT6); // DPCD 2202 EQ training pattern 3 supported
		#else
		msWriteByteMask(REG_DPRX_AUX_01_H + usRegOffsetAuxByID, 0, BIT1); // EQ training pattern 3 does not support
		if(ubHaveBeenSleeped[dprx_aux_id])
		{
			msWriteByteMask(REG_DPRX_AUX_DPCD_71_H + usRegOffsetAuxDPCDByID, mhal_DPRx_GetDPCDValueByRIU(dprx_aux_id, DPCD_00002), 0x1F); // Max lane count supported
		}
		else
		{
			msWriteByteMask(REG_DPRX_AUX_DPCD_71_H + usRegOffsetAuxDPCDByID, DP_LANECOUNT_4, 0x1F); // Max lane count supported
		}
		msWriteByteMask(REG_DPRX_AUX_DPCD_71_H + usRegOffsetAuxDPCDByID, 0, BIT6); // DPCD 2202 EQ training pattern 3 does not supported
		#endif
        msWriteByteMask(REG_DPRX_AUX_DPCD_71_H + usRegOffsetAuxDPCDByID, BIT7, BIT7); // DPCD 2202[7] Enhanced frame supported
	}
	else if((ucVersion == DP_VERSION_12_14) || (ucVersion == DP_VERSION_14))
	{
		printf("========== DP_VERSION_12_14 ==========\r\n");

		mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_00000, DP_VERSION_12); // DPCD minor revision number
		mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_02200, DP_VERSION_14); // DPCD minor revision number

		//Note: MT9700 default set as max link rate HBR only, SA can change the define for HBR2  ENABLE_DP_RX_MAX_LINKRATE_HBR2
		mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_00001, DP_LINKRATE_HBR); // Maximum link rate supported
		mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_02201, DP_LINKRATE_HBR); // Maximum link rate supported

		#if (DPRX_SUPPORT_TP3 == 0x1)
		msWriteByteMask(REG_DPRX_AUX_01_H + usRegOffsetAuxByID, BIT1, BIT1); // EQ training Pattern 3 supported
		if(ubHaveBeenSleeped[dprx_aux_id])
		{
			msWriteByteMask(REG_DPRX_AUX_DPCD_71_H + usRegOffsetAuxDPCDByID, mhal_DPRx_GetDPCDValueByRIU(dprx_aux_id, DPCD_00002), 0x1F); // Max lane count supported
		}
		else
		{
			msWriteByteMask(REG_DPRX_AUX_DPCD_71_H + usRegOffsetAuxDPCDByID, DP_LANECOUNT_4, 0x1F); // Max lane count supported
		}
		msWriteByteMask(REG_DPRX_AUX_DPCD_71_H + usRegOffsetAuxDPCDByID, BIT6, BIT6); // DPCD 2202 EQ training Pattern 3 supported
		#else
		msWriteByteMask(REG_DPRX_AUX_01_H + usRegOffsetAuxByID, 0, BIT1);
		if(ubHaveBeenSleeped[dprx_aux_id])
		{
			msWriteByteMask(REG_DPRX_AUX_DPCD_71_H + usRegOffsetAuxDPCDByID, mhal_DPRx_GetDPCDValueByRIU(dprx_aux_id, DPCD_00002), 0x1F); // Max lane count supported
		}
		else
		{
			msWriteByteMask(REG_DPRX_AUX_DPCD_71_H + usRegOffsetAuxDPCDByID, DP_LANECOUNT_4, 0x1F); // Max lane count supported
		}
		msWriteByteMask(REG_DPRX_AUX_DPCD_71_H + usRegOffsetAuxDPCDByID, 0, BIT6);
		#endif

        msWriteByteMask(REG_DPRX_AUX_DPCD_71_H + usRegOffsetAuxDPCDByID, BIT7, BIT7); // DPCD 2202[7] Enhanced frame supported

		#if (DPRX_SUPPORT_TP4 == 0x1)
		msWriteByteMask(REG_DPRX_AUX_7E_H + usRegOffsetAuxByID, BIT2, BIT2); // EQ training Pattern 4 supported
		msWriteByteMask(REG_DPRX_AUX_DPCD_72_L + usRegOffsetAuxDPCDByID, BIT7, BIT7); // EQ training Pattern 4 supported
		#else
		msWriteByteMask(REG_DPRX_AUX_7E_H + usRegOffsetAuxByID, 0, BIT2);
		msWriteByteMask(REG_DPRX_AUX_DPCD_72_L + usRegOffsetAuxDPCDByID, 0, BIT7);
		#endif

		mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_02204, 0x01);
		mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_02206, 0x01);
		mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_02207, 0x80);
		mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_02208, 0x02);
		mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_02209, 0x02);
		mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_0220A, 0x06);
		mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_0220B, 0x00);
		mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_0220C, 0x00);
		mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_0220D, 0x00);
		mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_0220F, 0x00);
		mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_02211, 0x00);

		msWriteByteMask(REG_DPRX_AUX_DPCD_31_H + usRegOffsetAuxDPCDByID, 0, BIT0); // DPCD 2200 sync

		#if (DPRX_SUPPORT_DPCD2200 == 0x1)
		msWriteByteMask(REG_DPRX_AUX_02_H + usRegOffsetAuxByID, BIT7, BIT7); // DPCD 2200 extended DPCD
		mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_0220E, 0x81);
		#else
		msWriteByteMask(REG_DPRX_AUX_02_H + usRegOffsetAuxByID, 0, BIT7);
		mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_0220E, 0x01);
		#endif

		#if (DPRX_SUPPORT_VSCSDP == 0x1)
		msWriteByteMask(REG_DPRX_AUX_7E_L + usRegOffsetAuxByID, BIT7, BIT7); // DPCD 2210[3] for VSC
		mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_02210, BIT3);
		#else
		msWriteByteMask(REG_DPRX_AUX_7E_L + usRegOffsetAuxByID, 0, BIT7);
		mhal_DPRx_SetDPCDValueByRIU(dprx_id, dprx_aux_id, DPCD_02210, 0);
		#endif
	}

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetupInputPort()
//  [Description]
//					mhal_DPRx_SetupInputPort
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetupInputPort(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, BOOL bEnable)
{
	if((dprx_id == DPRx_ID_MAX) || (dprx_aux_id == DPRx_AUX_ID_MAX))
	{
		return;
	}

	if(bEnable)
	{
        if(dprx_id == DPRx_ID_3)
        {
            // DPC MUX
            msWriteByteMask(REG_DPRX_PLL_50_L, BIT0, BIT0);
        }
        else
        {
            msWriteByteMask(REG_DPRX_PLL_50_L, 0, BIT0);
        }
	}

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CableGND_Level()
//  [Description]
//					mhal_DPRx_CableGND_Level
//  [Arguments]:
//
//  [Return]:
//					0: Cable connect
//					1: Cable disconnect
//
//**************************************************************************
BOOL mhal_DPRx_CableGND_Level(DPRx_AUX_ID dprx_aux_id)
{
	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

	switch(dprx_aux_id)
	{
		case DPRx_AUX_ID_0:
			if((Input_Displayport_C1 != Input_Nothing) && (DPRx_C1_AUX == AUX_GPIO_31_32_33))
			{
				return hwGNDDP0_Pin;
			}
			else if((Input_Displayport_C2 != Input_Nothing) && (DPRx_C2_AUX == AUX_GPIO_31_32_33))
			{
				return hwGNDDP1_Pin;
			}
			else if((Input_Displayport_C3 != Input_Nothing) && (DPRx_C3_AUX == AUX_GPIO_31_32_33))
			{
				return hwGNDDP2_Pin;
			}
			else if((Input_Displayport_C4 != Input_Nothing) && (DPRx_C4_AUX == AUX_GPIO_31_32_33))
			{
				return hwGNDDP3_Pin;
			}
			break;

		case DPRx_AUX_ID_1:
			if((Input_Displayport_C1 != Input_Nothing) && (DPRx_C1_AUX == AUX_GPIO_52_53_54))
			{
				return hwGNDDP0_Pin;
			}
			else if((Input_Displayport_C2 != Input_Nothing) && (DPRx_C2_AUX == AUX_GPIO_52_53_54))
			{
				return hwGNDDP1_Pin;
			}
			else if((Input_Displayport_C3 != Input_Nothing) && (DPRx_C3_AUX == AUX_GPIO_52_53_54))
			{
				return hwGNDDP2_Pin;
			}
			else if((Input_Displayport_C4 != Input_Nothing) && (DPRx_C4_AUX == AUX_GPIO_52_53_54))
			{
				return hwGNDDP3_Pin;
			}
			break;

		case DPRx_AUX_ID_2:
			if((Input_Displayport_C1 != Input_Nothing) && (DPRx_C1_AUX == AUX_GPIO_DPC_3_4_5))
			{
				return hwGNDDP0_Pin;
			}
			else if((Input_Displayport_C2 != Input_Nothing) && (DPRx_C2_AUX == AUX_GPIO_DPC_3_4_5))
			{
				return hwGNDDP1_Pin;
			}
			else if((Input_Displayport_C3 != Input_Nothing) && (DPRx_C3_AUX == AUX_GPIO_DPC_3_4_5))
			{
				return hwGNDDP2_Pin;
			}
			else if((Input_Displayport_C4 != Input_Nothing) && (DPRx_C4_AUX == AUX_GPIO_DPC_3_4_5))
			{
				return hwGNDDP3_Pin;
			}
			break;

		default:
			break;
	}

	return FALSE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_AUX_N_Level()
//  [Description]
//					mhal_DPRx_AUX_N_Level
//  [Arguments]:
//
//  [Return]:
//					1: Cable connect
//					0: Cable disconnect
//
//**************************************************************************
BOOL mhal_DPRx_AUX_N_Level(DPRx_AUX_ID dprx_aux_id)
{
	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

	switch(dprx_aux_id)
	{
		case DPRx_AUX_ID_0:
			if((Input_Displayport_C1 != Input_Nothing) && (DPRx_C1_AUX == AUX_GPIO_31_32_33))
			{
				return hwSDM_AuxN_DP0Pin_Pin;
			}
			else if((Input_Displayport_C2 != Input_Nothing) && (DPRx_C2_AUX == AUX_GPIO_31_32_33))
			{
				return hwSDM_AuxN_DP1Pin_Pin;
			}
			else if((Input_Displayport_C3 != Input_Nothing) && (DPRx_C3_AUX == AUX_GPIO_31_32_33))
			{
				return hwSDM_AuxN_DP2Pin_Pin;
			}
			else if((Input_Displayport_C4 != Input_Nothing) && (DPRx_C4_AUX == AUX_GPIO_31_32_33))
			{
				return hwSDM_AuxN_DP3Pin_Pin;
			}
			break;

		case DPRx_AUX_ID_1:
			if((Input_Displayport_C1 != Input_Nothing) && (DPRx_C1_AUX == AUX_GPIO_52_53_54))
			{
				return hwSDM_AuxN_DP0Pin_Pin;
			}
			else if((Input_Displayport_C2 != Input_Nothing) && (DPRx_C2_AUX == AUX_GPIO_52_53_54))
			{
				return hwSDM_AuxN_DP1Pin_Pin;
			}
			else if((Input_Displayport_C3 != Input_Nothing) && (DPRx_C3_AUX == AUX_GPIO_52_53_54))
			{
				return hwSDM_AuxN_DP2Pin_Pin;
			}
			else if((Input_Displayport_C4 != Input_Nothing) && (DPRx_C4_AUX == AUX_GPIO_52_53_54))
			{
				return hwSDM_AuxN_DP3Pin_Pin;
			}
			break;

		case DPRx_AUX_ID_2:
			if((Input_Displayport_C1 != Input_Nothing) && (DPRx_C1_AUX == AUX_GPIO_DPC_3_4_5))
			{
				return hwSDM_AuxN_DP0Pin_Pin;
			}
			else if((Input_Displayport_C2 != Input_Nothing) && (DPRx_C2_AUX == AUX_GPIO_DPC_3_4_5))
			{
				return hwSDM_AuxN_DP1Pin_Pin;
			}
			else if((Input_Displayport_C3 != Input_Nothing) && (DPRx_C3_AUX == AUX_GPIO_DPC_3_4_5))
			{
				return hwSDM_AuxN_DP2Pin_Pin;
			}
			else if((Input_Displayport_C4 != Input_Nothing) && (DPRx_C4_AUX == AUX_GPIO_DPC_3_4_5))
			{
				return hwSDM_AuxN_DP3Pin_Pin;
			}
			break;

		default:
			return FALSE;
			break;
	}

	return FALSE;
}

void ________VIDEO________(void);
//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EDIDOffsetSetting()
//  [Description]
//					mhal_DPRx_EDIDOffsetSetting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EDIDOffsetSetting(void)
{
	msWriteByteMask(REG_PM_DDC_7C_L, 0x4, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5); // reg_edid_sram_base_adr_sel_dp0
	msWriteByteMask(REG_PM_DDC_7C_H, 0x8, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5); // reg_edid_sram_base_adr_sel_dp1
	msWriteByteMask(REG_PM_DDC_7D_L, 0xC, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5); // reg_edid_sram_base_adr_sel_dp2

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_Load_EDID()
//  [Description]
//					mhal_DPRx_Load_EDID
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_Load_EDID(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, const BYTE *EdidData)
{
	BYTE ucValue = 0;
	BYTE i = 0;
    WORD uwEDIDSize = 0;
    BYTE ubEDID[512] = {0};
	BYTE ubBlockOffset = 0x0;
	WORD uwEDIDTimeOut = 0x0;
    DWORD ulTimeout = 400; // ~ 550u

    if((dprx_id == DPRx_ID_MAX) || (dprx_aux_id == DPRx_AUX_ID_MAX) || (EdidData == NULL))
    {
        return;
    }

    uwEDIDSize = (EdidData[126] + 1) * 128;

    if((uwEDIDSize < 128) || (uwEDIDSize > 512))
    {
        return;
    }

	memcpy(ubEDID, EdidData, uwEDIDSize);

	// DP0 --> block 0x4 ~ 0x7
	// DP1 --> block 0x8 ~ 0xB
	// DP2 --> block 0xC ~ 0xF
	ubBlockOffset = 0x4 + (dprx_aux_id << 2);

	msWriteByte(REG_PM_DDC_75_H, ubBlockOffset); // Select SRAM base address for CPU read/write

	for(i = 0; i < 128; i++)
	{
		ucValue = ubEDID[i];
        ulTimeout = 400; // ~ 550u

        while(DP_EDID_BUSY() && (ulTimeout > 0))
        {
            ulTimeout--;
        }

		msWriteByte(REG_PM_DDC_25_H, i); // DDC address port for CPU read/write
		msWriteByte(REG_PM_DDC_26_L, ucValue); // DDC data port for CPU write
		msWriteByteMask(REG_PM_DDC_21_H, BIT5, BIT5); // ADC SRAM write data pulse gen when CPU write

		uwEDIDTimeOut = 0x80;

		while((msRead2Byte(REG_PM_DDC_3D_H) & 0x20) && (uwEDIDTimeOut > 0))
		{
			uwEDIDTimeOut--;
		}
	}

    if(uwEDIDSize > 128)
    {
    	msWriteByte(REG_PM_DDC_75_H, (ubBlockOffset + 1)); // Select SRAM base address for CPU read/write

		for(i = 0; i < 128; i++)
		{
			ucValue = ubEDID[i+128];
            ulTimeout = 400; // ~ 550u

            while(DP_EDID_BUSY() && (ulTimeout > 0))
            {
                ulTimeout--;
            }

			msWriteByte(REG_PM_DDC_25_H, i); // DDC address port for CPU read/write
			msWriteByte(REG_PM_DDC_26_L, ucValue); // DDC data port for CPU write
			msWriteByteMask(REG_PM_DDC_21_H, BIT5, BIT5); // ADC SRAM write data pulse gen when CPU write

			uwEDIDTimeOut = 0x80;

			while((msRead2Byte(REG_PM_DDC_3D_H) & 0x20) && (uwEDIDTimeOut > 0))
			{
				uwEDIDTimeOut--;
			}
		}
    }

 	if(uwEDIDSize > 256)
	{
		msWriteByte(REG_PM_DDC_75_H, (ubBlockOffset + 2)); // Select SRAM base address for CPU read/write

		for(i = 0; i < 128; i++)
		{
			ucValue = ubEDID[i+256];
            ulTimeout = 400; // ~ 550u

            while(DP_EDID_BUSY() && (ulTimeout > 0))
            {
                ulTimeout--;
            }

			msWriteByte(REG_PM_DDC_25_H, i); // DDC address port for CPU read/write
			msWriteByte(REG_PM_DDC_26_L, ucValue); // DDC data port for CPU write
			msWriteByteMask(REG_PM_DDC_21_H, BIT5, BIT5); // ADC SRAM write data pulse gen when cpu write

			uwEDIDTimeOut = 0x80;

			while((msRead2Byte(REG_PM_DDC_3D_H) & 0x20) && (uwEDIDTimeOut > 0))
			{
				uwEDIDTimeOut--;
			}
		}
	}

	if(uwEDIDSize > 383)
	{
		msWriteByte(REG_PM_DDC_75_H, (ubBlockOffset + 3)); // Select SRAM base address for CPU read/write

		for(i = 0; i < 128 ; i++)
		{
			ucValue = ubEDID[i+384] ;
            ulTimeout = 400; // ~ 550u

            while(DP_EDID_BUSY() && (ulTimeout > 0))
            {
                ulTimeout--;
            }

			msWriteByte(REG_PM_DDC_25_H, i); // DDC address port for CPU read/write
			msWriteByte(REG_PM_DDC_26_L, ucValue); // DDC data port for CPU write
			msWriteByteMask(REG_PM_DDC_21_H, BIT5, BIT5); // ADC SRAM write data pulse gen when CPU write

			uwEDIDTimeOut = 0x80;

			while((msRead2Byte(REG_PM_DDC_3D_H) & 0x20) && (uwEDIDTimeOut > 0))
			{
				uwEDIDTimeOut--;
			}
		}
	}

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetInternalDPVersion()
//  [Description]
//					mhal_DPRx_SetInternalDPVersion
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetInternalDPVersion(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, BYTE ucValue, BYTE ucDPVersion)
{
    WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET000(dprx_id);
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if((dprx_id == DPRx_ID_MAX) || (dprx_aux_id == DPRx_AUX_ID_MAX))
	{
		return;
	}

	msWriteByteMask(REG_DPRX_DPCD1_09_L + usRegOffsetDPCD0ByID, ucValue, BIT3|BIT2|BIT1|BIT0); // reg_minor_rev
	msWriteByte(REG_DPRX_AUX_50_L + usRegOffsetAuxByID, ucDPVersion); // reg_dpcd_ver_inside

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetDPVersion()
//  [Description]
//					mhal_DPRx_GetDPVersion
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mhal_DPRx_GetDPVersion(DPRx_AUX_ID dprx_aux_id)
{
	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return 0;
	}

	if((mhal_DPRx_GetDPCDValueByRIU(dprx_aux_id, DPCD_0000E) & BIT7) == 0x0)
	{
   		return mhal_DPRx_GetDPCDValueByRIU(dprx_aux_id, DPCD_00000);
	}
	else
	{
		return mhal_DPRx_GetDPCDValueByRIU(dprx_aux_id, DPCD_02200);
	}
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetLinkRate()
//  [Description]
//					mhal_DPRx_GetLinkRate
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
WORD mhal_DPRx_GetLinkRate(DPRx_ID dprx_id)
{
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return 0;
	}

	return (27 * msReadByte(REG_DPRX_DPCD0_20_L + usRegOffsetDPCD0ByID));
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetDPCDLinkRate()
//  [Description]
//					mhal_DPRx_GetDPCDLinkRate
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mhal_DPRx_GetDPCDLinkRate(DPRx_AUX_ID dprx_aux_id)
{
    BYTE ucLinkRate = 0;

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return 0;
	}

    ucLinkRate = mhal_DPRx_GetDPCDValueByRIU(dprx_aux_id, DPCD_00100);

    return ucLinkRate;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetDPLaneCnt()
//  [Description]
//					mhal_DPRx_GetDPLaneCnt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mhal_DPRx_GetDPLaneCnt(DPRx_AUX_ID dprx_aux_id)
{
    BYTE ucLaneCnt = 0;

    if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return 0;
	}

    ucLaneCnt = mhal_DPRx_GetDPCDValueByRIU(dprx_aux_id, DPCD_00101) & 0x0F;

    return ucLaneCnt;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_MCUWriteOUIDPCD()
//  [Description]
//					mhal_DPRx_MCUWriteOUIDPCD
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_MCUWriteOUIDPCD(DPRx_AUX_ID dprx_aux_id, DWORD ulDPCDAddress, BYTE ucDATA)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    msWriteByteMask(REG_DPRX_AUX_DPCD_02_H + usRegOffsetAuxByID, BIT1, BIT1);
    msWriteByte(REG_DPRX_AUX_DPCD_00_L + usRegOffsetAuxByID, ulDPCDAddress & 0xFF);
    msWriteByte(REG_DPRX_AUX_DPCD_00_H + usRegOffsetAuxByID, (ulDPCDAddress >> 0x8) & 0xFF);
    msWriteByte(REG_DPRX_AUX_DPCD_01_L + usRegOffsetAuxByID, (ulDPCDAddress >> 0x10) & 0xF);
    msWriteByte(REG_DPRX_AUX_DPCD_02_L + usRegOffsetAuxByID, ucDATA);
    msWriteByteMask(REG_DPRX_AUX_DPCD_02_H + usRegOffsetAuxByID, BIT0, BIT0);
    mhal_DPRx_DELAY_NOP(100);
    msWriteByteMask(REG_DPRX_AUX_DPCD_02_H + usRegOffsetAuxByID, 0, BIT1);
    mhal_DPRx_DELAY_NOP(100);
    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableGUIDWrite()
//  [Description]
//					mhal_DPRx_EnableGUIDWrite
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableGUIDWrite(DPRx_AUX_ID dprx_aux_id, BOOL bEnable)
{
	WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

	if(bEnable == TRUE)
	{
    	msWriteByteMask(REG_DPRX_AUX_21_L + usRegOffsetAuxByID, BIT0, BIT0);
	}
	else
	{
		msWriteByteMask(REG_DPRX_AUX_21_L + usRegOffsetAuxByID, 0, BIT0);
	}

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetGUIDValue()
//  [Description]
//					mhal_DPRx_GetGUIDValue
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_GetGUIDValue(DPRx_AUX_ID dprx_aux_id, BYTE *pGUID)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
    BYTE XDATA uctemp = 0;

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (pGUID == NULL))
	{
		return;
	}

    for(uctemp = 0; uctemp < DP_GUID_SIZE; uctemp++)
    {
        pGUID[uctemp] = msReadByte(REG_DPRX_AUX_31_L + usRegOffsetAuxByID + uctemp);
    }

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CheckSquelch()
//  [Description]
//					mhal_DPRx_CheckSquelch
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_CheckSquelch(DPRx_ID dprx_id)
{
	WORD usRegOffset40ByID = DP_REG_OFFSET040(dprx_id);
	BOOL XDATA bSquelchFlag = FALSE;

	if(dprx_id == DPRx_ID_MAX)
	{
		return FALSE;
	}

	switch((msReadByte(REG_DPRX_PHY_PM_01_H + usRegOffset40ByID) & (BIT3|BIT2)) >> 2)
	{
	    case 0:
	        bSquelchFlag = msReadByte(REG_DPRX_PHY_PM_11_L + usRegOffset40ByID) & BIT0;
	        break;

	    case 1:
	        bSquelchFlag = (msReadByte(REG_DPRX_PHY_PM_11_L + usRegOffset40ByID) & BIT1) >> 1;
	        break;

	    case 2:
	        bSquelchFlag = (msReadByte(REG_DPRX_PHY_PM_11_L + usRegOffset40ByID) & BIT2) >> 2;
	        break;

	    case 3:
	        bSquelchFlag = (msReadByte(REG_DPRX_PHY_PM_11_L + usRegOffset40ByID) & BIT3) >> 3;
	        break;

	    default:
	        bSquelchFlag = FALSE;
	        break;
	}

    return bSquelchFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ResetAsyncFIFO()
//  [Description]
//					mhal_DPRx_ResetAsyncFIFO
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_ResetAsyncFIFO(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET000(dprx_id);

    if((dprx_id == DPRx_ID_MAX) || (dprx_aux_id == DPRx_AUX_ID_MAX))
	{
		return;
	}

    msWriteByteMask(REG_DPRX_AUX_0F_L + usRegOffsetAuxByID, BIT1, BIT1);
	mhal_DPRx_DELAY_NOP(5);
	msWriteByteMask(REG_DPRX_AUX_0F_L + usRegOffsetAuxByID, 0, BIT1);

	msWriteByteMask(REG_DPRX_DPCD0_1A_L + usRegOffsetDPCD0ByID, BIT5, BIT5);
	mhal_DPRx_DELAY_NOP(5);
	msWriteByteMask(REG_DPRX_DPCD0_1A_L + usRegOffsetDPCD0ByID, 0, BIT5);

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetReferenceClock()
//  [Description]
//					mhal_DPRx_SetReferenceClock
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetReferenceClock(DPRx_DECODER_ID dprx_decoder_id, WORD uwReferClock)
{
	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

	msWrite2Byte(REG_DPRX_PLL_16_L + (dprx_decoder_id * 2), uwReferClock);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_UpdateMVIDValue()
//  [Description]
//					mhal_DPRx_UpdateMVIDValue
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_UpdateMVIDValue(DPRx_DECODER_ID dprx_decoder_id)
{
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

    msWriteByte(REG_DPRX_DECODER_E0_14_L + usRegOffsetDecoderByID, msReadByte(REG_DPRX_DECODER_E0_30_L + usRegOffsetDecoderByID)); // MVID_CENTER[7:0]
    msWriteByte(REG_DPRX_DECODER_E0_14_H + usRegOffsetDecoderByID, msReadByte(REG_DPRX_DECODER_E0_30_H + usRegOffsetDecoderByID)); // MVID_CENTER[15:8]
    msWriteByte(REG_DPRX_DECODER_E0_15_L + usRegOffsetDecoderByID, msReadByte(REG_DPRX_DECODER_E0_31_L + usRegOffsetDecoderByID)); // MVID_CENTER[23:16]

    msWriteByte(REG_DPRX_DECODER_E0_1C_L + usRegOffsetDecoderByID, msReadByte(REG_DPRX_DECODER_E0_32_L + usRegOffsetDecoderByID)); // NVID_CENTER[7:0]
    msWriteByte(REG_DPRX_DECODER_E0_1C_H + usRegOffsetDecoderByID, msReadByte(REG_DPRX_DECODER_E0_32_H + usRegOffsetDecoderByID)); // NVID_CENTER[15:8]
    msWriteByte(REG_DPRX_DECODER_E0_1D_L + usRegOffsetDecoderByID, msReadByte(REG_DPRX_DECODER_E0_33_L + usRegOffsetDecoderByID)); // NVID_CENTER[23:16]

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_UpdateMAUDValue()
//  [Description]
//					mhal_DPRx_UpdateMAUDValue
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_UpdateMAUDValue(DPRx_DECODER_ID dprx_decoder_id)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

	msWrite2Byte(REG_DPRX_DECODER_E0_10_L + usRegOffsetDecoderByID, stAudioInfo.ulMCodeValue & 0xFFFF ); // MAUD_CENTER[7:0]
    msWriteByte(REG_DPRX_DECODER_E0_11_L + usRegOffsetDecoderByID, stAudioInfo.ulMCodeValue >> 16); // MAUD_CENTER[15:8]

    msWrite2Byte(REG_DPRX_DECODER_E0_18_L + usRegOffsetDecoderByID, stAudioInfo.ulNCodeValue & 0xFFFF); // NAUD_CENTER[7:0]
    msWriteByte(REG_DPRX_DECODER_E0_19_L + usRegOffsetDecoderByID, stAudioInfo.ulNCodeValue >> 16); // NAUD_CENTER[15:8]

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetHTT()
//  [Description]
//					mhal_DPRx_GetHTT
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
DWORD mhal_DPRx_GetHTT(DPRx_DECODER_ID dprx_decoder_id)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return 0;
	}

	return (msRead2Byte(REG_DPRX_DECODER_E0_34_L + usRegOffsetDecoderByID) & 0x7FFF);
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetVTT()
//  [Description]
//					mhal_DPRx_GetVTT
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
DWORD mhal_DPRx_GetVTT(DPRx_DECODER_ID dprx_decoder_id)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return 0;
	}

	return (msRead2Byte(REG_DPRX_DECODER_E0_35_L + usRegOffsetDecoderByID) & 0x7FFF);
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetHWidth()
//  [Description]
//					mhal_DPRx_GetHWidth
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
DWORD mhal_DPRx_GetHWidth(DPRx_DECODER_ID dprx_decoder_id)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return 0;
	}

	return (msRead2Byte(REG_DPRX_DECODER_E0_36_L + usRegOffsetDecoderByID) & 0x7FFF);
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetVWidth()
//  [Description]
//					mhal_DPRx_GetVWidth
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
DWORD mhal_DPRx_GetVWidth(DPRx_DECODER_ID dprx_decoder_id)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return 0;
	}

	return (msRead2Byte(REG_DPRX_DECODER_E0_37_L + usRegOffsetDecoderByID) & 0x7FFF);
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetTimingPixelClock()
//  [Description]
//					mhal_DPRx_GetTimingPixelClock
//  [Arguments]:
//
//  [Return]:
//                  Pixel clock = xxx MHz
//**************************************************************************
WORD mhal_DPRx_GetTimingPixelClock(DPRx_ID dprx_id, DPRx_DECODER_ID dprx_decoder_id)
{
	DWORD XDATA ulPixelClock = 0;
    DWORD XDATA ulLSClock = mhal_DPRx_GetLinkRate(dprx_id);
	DWORD XDATA ulBaseMValue = 0;
    DWORD XDATA ulBaseNValue = 0;
	DWORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);
	unsigned long long Temp;
	WORD XDATA usPreviousRange = 0;
	WORD XDATA usTargetRange = 0;
	DWORD XDATA ulMValueTotal = 0;
    BYTE XDATA ubMValueLineCount = 0x64;  // 100 Lines M value
	BYTE XDATA ubMValueLine = 0;

	if((dprx_id == DPRx_ID_MAX) || (dprx_decoder_id == DPRx_DECODER_ID_MAX))
	{
		return 0;
	}

    for(ubMValueLine = 0; ubMValueLine < ubMValueLineCount ; ubMValueLine++)
	{
    	msWriteByteMask(REG_DPRX_DECODER_E0_0A_L + usRegOffsetDecoderByID, BIT0, BIT0); // Receiver 51
		ulBaseMValue = ((DWORD)msReadByte(REG_DPRX_DECODER_E0_31_L + usRegOffsetDecoderByID) << 16) + msRead2Byte(REG_DPRX_DECODER_E0_30_L);
		ulMValueTotal = ulMValueTotal + ulBaseMValue;
		msWriteByteMask(REG_DPRX_DECODER_E0_0A_L + usRegOffsetDecoderByID, 0, BIT0); // Receiver 51
		mhal_DPRx_DELAY_NOP(0xFA);   //250 NOP equal 6.14us longer than 5k60(VTT:2942) one line time
	}

 	ulBaseMValue = ulMValueTotal / ubMValueLineCount;
    ulBaseNValue = ((DWORD)msReadByte(REG_DPRX_DECODER_E0_33_L + usRegOffsetDecoderByID) << 16) + msRead2Byte(REG_DPRX_DECODER_E0_32_L);

    if(ulBaseNValue > 0)
    {
    	Temp = ((unsigned long long)ulBaseMValue * ulLSClock) / ulBaseNValue;
		ulPixelClock = Temp & 0xFFFFFFFF;
    }

#if (DPRX_M_RANGE_NEW_MODE == 1)

    usTargetRange = ulBaseMValue * DPRX_M_RANGE_NEW_MODE_VALUE / 1000;

    if(usTargetRange > 0x40)
    {
        usTargetRange = usTargetRange + 0x100;
    }
#else
	if(ulPixelClock > DPRX_M_DETECT_RANGE_PIXEL_CLK) // When pixel clk > 600MHz (ex: 4K@144), M value range should larger than befoe
    {
       	usTargetRange = 0x130;
    }
	else
	{
        usTargetRange = 0x20;
	}
#endif

    usPreviousRange = msRead2Byte(REG_DPRX_DECODER_E0_16_L + usRegOffsetDecoderByID);
    msWrite2Byte(REG_DPRX_DECODER_E0_16_L + usRegOffsetDecoderByID, (usTargetRange | usPreviousRange));
    msWrite2Byte(REG_DPRX_DECODER_E0_16_L + usRegOffsetDecoderByID, usTargetRange);                     // MVID_Range[15:0]

    return ulPixelClock;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetTimingPixelClock10K()
//  [Description]
//					mhal_DPRx_GetTimingPixelClock10K
//  [Arguments]:
//
//  [Return]:
//                  Pixel clock = xxx MHz
//**************************************************************************
WORD mhal_DPRx_GetTimingPixelClock10K(DPRx_ID dprx_id, DPRx_DECODER_ID dprx_decoder_id)
{
	DWORD XDATA ulPixelClock = 0;
	DWORD XDATA ulLSClock = mhal_DPRx_GetLinkRate(dprx_id);
	DWORD XDATA ulBaseMValue = 0;
    DWORD XDATA ulBaseNValue = 0;
	DWORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);
	unsigned long long Temp;
	WORD XDATA usPreviousRange = 0;
	WORD XDATA usTargetRange = 0;
	DWORD XDATA ulMValueTotal = 0;
    BYTE XDATA ubMValueLineCount = 0x64;  // 100 Lines M value
	BYTE XDATA ubMValueLine = 0;

	if((dprx_id == DPRx_ID_MAX) || (dprx_decoder_id == DPRx_DECODER_ID_MAX))
	{
		return 0;
	}

    for(ubMValueLine = 0; ubMValueLine < ubMValueLineCount ; ubMValueLine++)
	{
    	msWriteByteMask(REG_DPRX_DECODER_E0_0A_L + usRegOffsetDecoderByID, BIT0, BIT0); // Receiver 51
    	ulBaseMValue = ((DWORD)msReadByte(REG_DPRX_DECODER_E0_31_L + usRegOffsetDecoderByID) << 16) + msRead2Byte(REG_DPRX_DECODER_E0_30_L);
		ulMValueTotal = ulMValueTotal + ulBaseMValue;
		msWriteByteMask(REG_DPRX_DECODER_E0_0A_L + usRegOffsetDecoderByID, 0, BIT0); // Receiver 51
		mhal_DPRx_DELAY_NOP(0xFA);  //250 NOP equal 6.14us longer than 5k60(VTT:2942) one line time
	}

	ulBaseMValue = (ulMValueTotal / ubMValueLineCount)*100;
   	ulBaseNValue = ((DWORD)msReadByte(REG_DPRX_DECODER_E0_33_L + usRegOffsetDecoderByID) << 16) + msRead2Byte(REG_DPRX_DECODER_E0_32_L);

    if(ulBaseNValue > 0)
    {
		Temp = ((unsigned long long)ulBaseMValue * ulLSClock) / ulBaseNValue;
		ulPixelClock = Temp & 0xFFFFFFFF;
    }

#if (DPRX_M_RANGE_NEW_MODE == 1)
        usTargetRange = ulBaseMValue / 100 * DPRX_M_RANGE_NEW_MODE_VALUE / 1000;

        if(usTargetRange > 0x40)
        {
            usTargetRange = usTargetRange + 0x100;
        }
#else
        if(ulPixelClock > (DPRX_M_DETECT_RANGE_PIXEL_CLK*100)) // When pixel clk > 600MHz (ex: 4K@144), M value range should larger than befoe
        {
            usTargetRange = 0x130;
        }
        else
        {
            usTargetRange = 0x20;
        }
#endif

        usPreviousRange = msRead2Byte(REG_DPRX_DECODER_E0_16_L + usRegOffsetDecoderByID);
        msWrite2Byte(REG_DPRX_DECODER_E0_16_L + usRegOffsetDecoderByID, (usTargetRange | usPreviousRange));
        msWrite2Byte(REG_DPRX_DECODER_E0_16_L + usRegOffsetDecoderByID, usTargetRange);                     // MVID_Range[15:0]

    return ulPixelClock;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetTimingInformation()
//  [Description]
//					mhal_DPRx_GetTimingInformation
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetTimingInformation(DPRx_DECODER_ID dprx_decoder_id, WORD *usTimingInfo)
{
    BOOL XDATA bInterlace = FALSE;
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if((dprx_decoder_id == DPRx_DECODER_ID_MAX) || (usTimingInfo == NULL))
	{
		return FALSE;
	}

    bInterlace = (msReadByte(REG_DPRX_DECODER_E0_2D_L + usRegOffsetDecoderByID) & BIT2)? TRUE : FALSE;

    usTimingInfo[0] = msRead2Byte(REG_DPRX_DECODER_E0_34_L + usRegOffsetDecoderByID) & 0x7FFF; // H total
    usTimingInfo[1] = msRead2Byte(REG_DPRX_DECODER_E0_35_L + usRegOffsetDecoderByID) & 0x7FFF; // V total
    usTimingInfo[2] = msRead2Byte(REG_DPRX_DECODER_E0_36_L + usRegOffsetDecoderByID) & 0x7FFF; // H width
    usTimingInfo[3] = msRead2Byte(REG_DPRX_DECODER_E0_37_L + usRegOffsetDecoderByID) & 0x7FFF; // V width
    usTimingInfo[4] = msRead2Byte(REG_DPRX_DECODER_E0_38_L + usRegOffsetDecoderByID) & 0x7FFF; // H start
    usTimingInfo[5] = msRead2Byte(REG_DPRX_DECODER_E0_39_L + usRegOffsetDecoderByID) & 0x7FFF; // V start
    usTimingInfo[6] = msRead2Byte(REG_DPRX_DECODER_E0_3A_L + usRegOffsetDecoderByID);		   // H PWS
    usTimingInfo[7] = msRead2Byte(REG_DPRX_DECODER_E0_3B_L + usRegOffsetDecoderByID);		   // V PWS

    return bInterlace;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableAutoInterlace()
//  [Description]
//					mhal_DPRx_EnableAutoInterlace
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableAutoInterlace(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable)
{
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

    msWriteByteMask(REG_DPRX_DECODER_E0_62_L + usRegOffsetDecoderByID, bEnable? BIT4 : 0, BIT4);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetInterlaceFlag()
//  [Description]
//					mhal_DPRx_GetInterlaceFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetInterlaceFlag(DPRx_DECODER_ID dprx_decoder_id)
{
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return FALSE;
	}

    return (msReadByte(REG_DPRX_DECODER_E0_2D_L + usRegOffsetDecoderByID) & BIT2)? TRUE : FALSE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetMISC01()
//  [Description]
//					mhal_DPRx_GetMISC01
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_GetMISC01(DPRx_DECODER_ID dprx_decoder_id, BYTE *usMISCInfo)
{
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if((dprx_decoder_id == DPRx_DECODER_ID_MAX) || (usMISCInfo == NULL))
	{
		return;
	}

    usMISCInfo[1] = msReadByte(REG_DPRX_DECODER_E0_2E_H + usRegOffsetDecoderByID); // MISC1
    usMISCInfo[0] = msReadByte(REG_DPRX_DECODER_E0_2E_L + usRegOffsetDecoderByID); // MISC0

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableDRRFunction()
//  [Description]
//					mhal_DPRx_EnableDRRFunction
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableDRRFunction(DPRx_AUX_ID dprx_aux_id, BOOL bEnable)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
	WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

	if(bEnable == TRUE)
	{
		msWriteByteMask(REG_DPRX_AUX_01_H + usRegOffsetAuxByID, BIT6, BIT6);
		msWriteByteMask(REG_DPRX_AUX_DPCD_74_L + usRegOffsetAuxDPCDByID, BIT6, BIT6); // DPCD 2207[6] MSA_TIMING_PAR_IGNORED
	}
	else
	{
		msWriteByteMask(REG_DPRX_AUX_01_H + usRegOffsetAuxByID, 0, BIT6);
		msWriteByteMask(REG_DPRX_AUX_DPCD_74_L + usRegOffsetAuxDPCDByID, 0, BIT6); // DPCD 2207[6] MSA_TIMING_PAR_IGNORED
	}

	return;
 }

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetDRRFlag()
//  [Description]
//					mhal_DPRx_GetDRRFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetDRRFlag(DPRx_AUX_ID dprx_aux_id)
{
    BOOL bDRRFlag = FALSE;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    if((msReadByte(REG_DPRX_AUX_DPCD_30_L + usRegOffsetAuxByID) & BIT7) == BIT7)
    {
        bDRRFlag = TRUE;
    }

    return bDRRFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableRegenSync()
//  [Description]
//					mhal_DPRx_EnableRegenSync
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableRegenSync(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

    msWriteByteMask(REG_DPRX_DECODER_E0_62_H + usRegOffsetDecoderByID, bEnable? BIT7 : 0, BIT7);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetRegenSync()
//  [Description]
//					mhal_DPRx_SetRegenSync
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetRegenSync(DPRx_ID dprx_id, BYTE Value)
{
	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByte(REG_DPRX2_TOP_GP_48_L + dprx_id, Value);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_RegenTimingInformation()
//  [Description]
//					mhal_DPRx_RegenTimingInformation
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_RegenTimingInformation(DPRx_DECODER_ID dprx_decoder_id, WORD *usTimingInfo)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if((dprx_decoder_id == DPRx_DECODER_ID_MAX) || (usTimingInfo == NULL))
	{
		return;
	}

	// V parameter
	msWrite2Byte(REG_DPRX_DECODER_E0_69_L + usRegOffsetDecoderByID, usTimingInfo[0] & 0x7FFF); // Vsync width
	msWriteByte(REG_DPRX_DECODER_E0_7F_L + usRegOffsetDecoderByID, usTimingInfo[1]); // Vsync front porch width
	msWriteByte(REG_DPRX_DECODER_E0_7F_H + usRegOffsetDecoderByID, usTimingInfo[2]); // Vsync back porch

	// H parameter
	msWrite2Byte(REG_DPRX_DECODER_E0_68_L + usRegOffsetDecoderByID, usTimingInfo[3]); // Hsync width

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_IsInputStreamValid()
//  [Description]
//					mhal_DPRx_IsInputStreamValid
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_IsInputStreamValid(DPRx_DECODER_ID dprx_decoder_id)
{
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

    if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
    {
        return FALSE;
    }

	if((msReadByte(REG_DPRX_DECODER_E0_0A_L + usRegOffsetDecoderByID) & (BIT5|BIT7)) == (BIT5|BIT7))
    {
        return TRUE;
    }

    return FALSE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetHwHtotalLearningStable()
//  [Description]
//					mhal_DPRx_GetHwHtotalLearningStable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetHwHtotalLearningStable(DPRx_AUX_ID dprx_aux_id, DPRx_DECODER_ID dprx_decoder_id)
{
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);
	WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);
	BOOL bRet = FALSE;

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (dprx_decoder_id == DPRx_DECODER_ID_MAX))
    {
        return FALSE;
    }

	if((msReadByte(REG_DPRX_AUX_DPCD_30_L + usRegOffsetAuxDPCDByID) & BIT7) == BIT7) // Freesync on
	{
		if(msReadByte(REG_DPRX_DECODER_E0_0A_L + usRegOffsetDecoderByID) & BIT7)
		{
			bRet = TRUE;
		}
	}
	else
	{
		if((msReadByte(REG_DPRX_DECODER_E0_44_L + usRegOffsetDecoderByID) & BIT2) &&
		   (msRead2Byte(REG_DPRX_DECODER_E0_46_L + usRegOffsetDecoderByID) != 0xFFFF) &&
		   (msReadByte(REG_DPRX_DECODER_E0_0A_L + usRegOffsetDecoderByID) & BIT7))
		{
			bRet = TRUE;
		}
	}

    return bRet;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_OverwriteVBlankingData()
//  [Description]
//					mhal_DPRx_OverwriteVBlankingData
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_OverwriteVBlankingData(DPRx_DECODER_ID dprx_decoder_id, BYTE ucDPColorFormat, BYTE ucDPRangeFormat)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

	switch(ucDPColorFormat)
	{
		case DP_COLOR_FORMAT_RGB:
			msWriteByteMask(REG_DPRX_DECODER_E0_59_H + usRegOffsetDecoderByID, 0, BIT7|BIT6|BIT5|BIT4); // Set R/Cr to 0x0
			msWriteByteMask(REG_DPRX_DECODER_E0_59_H + usRegOffsetDecoderByID, 0, BIT3|BIT2|BIT1|BIT0); // Set G/Y to 0x0
			msWriteByteMask(REG_DPRX_DECODER_E0_59_L + usRegOffsetDecoderByID, 0, BIT3|BIT2|BIT1|BIT0); // Set B/Cb to 0x0
			break;

		case DP_COLOR_FORMAT_YUV_444:
		case DP_COLOR_FORMAT_YUV_422:
		case DP_COLOR_FORMAT_YUV_420:
			if(ucDPRangeFormat == DP_COLOR_RANGE_FULL)
			{
				msWriteByteMask(REG_DPRX_DECODER_E0_59_H + usRegOffsetDecoderByID, BIT7|BIT6|BIT5|BIT4, BIT7|BIT6|BIT5|BIT4); // Set R/Cr to 0xF
				msWriteByteMask(REG_DPRX_DECODER_E0_59_H + usRegOffsetDecoderByID, 0, BIT3|BIT2|BIT1|BIT0); // Set G/Y to 0x0
				msWriteByteMask(REG_DPRX_DECODER_E0_59_L + usRegOffsetDecoderByID, BIT3|BIT2|BIT1|BIT0, BIT3|BIT2|BIT1|BIT0); // Set B/Cb to 0xF
			}
			else
			{
				msWriteByteMask(REG_DPRX_DECODER_E0_59_H + usRegOffsetDecoderByID, BIT7|BIT6|BIT5|BIT4, BIT7|BIT6|BIT5|BIT4); // Set R/Cr to 0xF
				msWriteByteMask(REG_DPRX_DECODER_E0_59_H + usRegOffsetDecoderByID, BIT2|BIT1, BIT3|BIT2|BIT1|BIT0); // Set G/Y to 0x6
				msWriteByteMask(REG_DPRX_DECODER_E0_59_L + usRegOffsetDecoderByID, BIT3|BIT2|BIT1|BIT0, BIT3|BIT2|BIT1|BIT0); // Set B/Cb to 0xF
			}
			break;

		default:
			break;
	}

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetPacketStatus()
//  [Description]
//					mhal_DPRx_GetPacketStatus
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
WORD mhal_DPRx_GetPacketStatus(DPRx_DECODER_ID dprx_decoder_id)
{
    WORD XDATA usPacketStatus = 0;
    WORD usRegOffsetSDPByID = DP_REG_OFFSET000(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return 0;
	}

    usPacketStatus = msRead2Byte(REG_DPRX_SDP_E0_02_L + usRegOffsetSDPByID);

    return usPacketStatus;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ClearPacketStatus()
//  [Description]
//					mhal_DPRx_ClearPacketStatus
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_ClearPacketStatus(DPRx_DECODER_ID dprx_decoder_id, DP_SDP_SEL PacketType)
{
    WORD usRegOffsetSdpByID = DP_REG_OFFSET000(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

    msWrite2Byte(REG_DPRX_SDP_E0_02_L + usRegOffsetSdpByID, BIT0 << PacketType);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetPacketData()
//  [Description]
//                  mhal_DPRx_GetPacketData
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetPacketData(DPRx_DECODER_ID dprx_decoder_id, DP_SDP_SEL PacketType, BYTE *pHeader, BYTE *pPacketData, BYTE ubDataLength)
{
    BYTE i = 0;
	BYTE ucSDPData = 0;
	WORD usRegOffsetSDPByID = DP_REG_OFFSET000(dprx_decoder_id);
	BYTE ubNumberOf32Bytes = 0x0;
	BYTE ubDataArray32Offset = 0x0;

	if((dprx_decoder_id == DPRx_DECODER_ID_MAX) || (PacketType == DP_SDP_SEL_MAX) || (pHeader == NULL) || (pPacketData == NULL))
	{
		return FALSE;
	}

	ucSDPData = msReadByte(REG_DPRX_SDP_E0_08_L + usRegOffsetSDPByID);

    msWriteByteMask(REG_DPRX_SDP_E0_08_L + usRegOffsetSDPByID, PacketType, (BIT0|BIT1|BIT2|BIT3)); // Choose packet type

	if(PacketType == DP_SDP_SEL_PPS)
	{
		if((ubDataLength % DP_SDP_PACKET_MAX_DATA_LENGTH) == 0x0)
		{
			ubNumberOf32Bytes = ubDataLength / DP_SDP_PACKET_MAX_DATA_LENGTH;
		}
		else
		{
			ubNumberOf32Bytes = (ubDataLength / DP_SDP_PACKET_MAX_DATA_LENGTH) + 1;
		}
	}
	else
	{
		ubNumberOf32Bytes = 0x1;

		if(ubDataLength > DP_SDP_PACKET_MAX_DATA_LENGTH)
		{
			ubDataLength = DP_SDP_PACKET_MAX_DATA_LENGTH;
		}
	}

	mhal_DPRx_DELAY_NOP(10);

	ubDataArray32Offset = ubNumberOf32Bytes;

	while(ubNumberOf32Bytes > 0)
	{
		msWriteByteMask(REG_DPRX_SDP_E0_08_H + usRegOffsetSDPByID, ((ubDataArray32Offset - ubNumberOf32Bytes) & 0x03) << 4, BIT4|BIT5);

		mhal_DPRx_DELAY_NOP(20);

		if(ubDataLength > DP_SDP_PACKET_MAX_DATA_LENGTH)
		{
			for(i = 0x0; i < DP_SDP_PACKET_MAX_DATA_LENGTH; i++)
		    {
		        pPacketData[(i + (ubDataArray32Offset - ubNumberOf32Bytes) * 32)] = msReadByte(REG_DPRX_SDP_E0_10_L + usRegOffsetSDPByID + i); // Read data
		    }
			ubDataLength = ubDataLength - DP_SDP_PACKET_MAX_DATA_LENGTH;
		}
		else
		{
		    for(i = 0x0; i < ubDataLength; i++)
		    {
		        pPacketData[(i + (ubDataArray32Offset - ubNumberOf32Bytes) * 32)] = msReadByte(REG_DPRX_SDP_E0_10_L + usRegOffsetSDPByID + i); // Read data
		    }
		}

		ubNumberOf32Bytes--;
	}

    for(i = 0x0; i < 4; i++)
    {
        pHeader[i] = msReadByte(REG_DPRX_SDP_E0_0E_L + usRegOffsetSDPByID + i); // Read header
    }

    msWriteByte(REG_DPRX_SDP_E0_08_L + usRegOffsetSDPByID, ucSDPData);

    return TRUE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetVSCSDP()
//  [Description]
//					mhal_DPRx_GetVSCSDP
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_GetVSCSDP(DPRx_ID dprx_id, BYTE *usMISCInfo)
{
	BYTE ucSDPData = 0;
	WORD usRegOffsetSDPByID = DP_REG_OFFSET000(dprx_id);

	if((dprx_id == DPRx_ID_MAX) || (usMISCInfo == NULL))
	{
		return;
	}

	ucSDPData = msReadByte(REG_DPRX_SDP_E0_08_L + usRegOffsetSDPByID);

	msWriteByteMask(REG_DPRX_SDP_E0_08_L + usRegOffsetSDPByID, DP_SDP_SEL_VSC, BIT0|BIT1|BIT2|BIT3); // Choose VSP

	usMISCInfo[0] = msReadByte(REG_DPRX_SDP_E0_18_L + usRegOffsetSDPByID); // SDP DB16
	usMISCInfo[1] = msReadByte(REG_DPRX_SDP_E0_18_H + usRegOffsetSDPByID); // SDP DB17
	usMISCInfo[2] = msReadByte(REG_DPRX_SDP_E0_19_L + usRegOffsetSDPByID); // SDP DB18

	msWriteByte(REG_DPRX_SDP_E0_08_L + usRegOffsetSDPByID, ucSDPData);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetVSC_EXT()
//  [Description]
//					mhal_DPRx_GetVSC_EXT
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetVSC_EXT(DPRx_ID dprx_id, DP_VSC_EXT_TYPE Type, BYTE *VSC_EXT)
{
	BYTE Count = 0;
	BYTE i = 0, j = 0;
	BYTE Data_Count = 0;
	BYTE Address_Offset = 0;
	WORD usRegOffsetSDPByID = DP_REG_OFFSET000(dprx_id);

	if((dprx_id == DPRx_ID_MAX) || (VSC_EXT == NULL))
	{
		return FALSE;
	}

	#if (VSC_EXT_SW == 0x1)
	BOOL Receive_Bit = 0;
	BOOL bSRAM_Sel = 0;
	#endif

	Data_Count = 0;

	#if (VSC_EXT_SW == 0x1)
	Receive_Bit = (msReadByte(REG_DPRX_SDP_E0_04_L + Address_Offset + usRegOffsetSDPByID) & BIT3)? TRUE : FALSE;
	bSRAM_Sel = msReadByte(REG_DPRX_SDP_E0_05_L + Address_Offset + usRegOffsetSDPByID) & BIT0;

	if(bSRAM_Sel)
	{
		msWriteByteMask(REG_DPRX_SDP_E0_04_L + Address_Offset + usRegOffsetSDPByID, 0, BIT6); // Set read
	}
	else
	{
		msWriteByteMask(REG_DPRX_SDP_E0_04_L + Address_Offset + usRegOffsetSDPByID, 0, BIT7); // Set read
	}

	msWriteByteMask(REG_DPRX_SDP_E0_04_L + Address_Offset + usRegOffsetSDPByID, BIT4, BIT4);

	if(bSRAM_Sel)
	{
		msWriteByteMask(REG_DPRX_SDP_E0_04_L + Address_Offset + usRegOffsetSDPByID, 0, BIT5);
	}
	else
	{
		msWriteByteMask(REG_DPRX_SDP_E0_04_L + Address_Offset + usRegOffsetSDPByID, BIT5, BIT5);
	}
	#endif

	if(Type == CEA)
	{
		Address_Offset = 4;

		msWriteByteMask(REG_DPRX_SDP_E0_08_H + usRegOffsetSDPByID, BIT3, BIT3); // Set VSC_EXT_CEA

		DPRX_HAL_DPRINTF("\r\n Check CEA %x", i);
	}
	else
	{
		msWriteByteMask(REG_DPRX_SDP_E0_08_H + usRegOffsetSDPByID, 0, BIT3); // Set VSC_EXT_VESA

		DPRX_HAL_DPRINTF("\r\n Check VESA %x", i);
	}

	Count = msReadByte(REG_DPRX_SDP_E0_05_H + Address_Offset + usRegOffsetSDPByID) + 1;

	#if (VSC_EXT_SW == 0x1)
	if(Receive_Bit)
	#else
	if(msReadByte(REG_DPRX_SDP_E0_04_L + Address_Offset + usRegOffsetSDPByID) & BIT3)
	#endif
	{
		msWriteByteMask(REG_DPRX_SDP_E0_04_L + Address_Offset + usRegOffsetSDPByID, BIT0, BIT0); // Set read

		for(i = 0; i < Count; i++)
		{
			DPRX_HAL_DPRINTF("\r\n [i] => %x", i);

			msWriteByteMask(REG_DPRX_SDP_E0_04_L + Address_Offset + usRegOffsetSDPByID, BIT2, BIT2); // Read trigger

			for(j = 0; j < 32; j++)
			{
				VSC_EXT[Data_Count] = msReadByte(REG_DPRX_SDP_E0_20_L + j + usRegOffsetSDPByID);

				DPRX_HAL_DPRINTF("\r\n VSC_EXT = %x", VSC_EXT[Data_Count]);

				Data_Count++;
			}
		}
	}
	else
	{
		DPRX_HAL_DPRINTF("\r\n No VSC_EXT = %x", 0);

		return 0;
	}

	msWriteByteMask(REG_DPRX_SDP_E0_04_L + Address_Offset + usRegOffsetSDPByID, BIT1, BIT1); // Set free
	msWriteByteMask(REG_DPRX_SDP_E0_04_L + Address_Offset + usRegOffsetSDPByID, BIT3, BIT3); // Clear

	#if (VSC_EXT_SW == 0x1)
	msWriteByteMask(REG_DPRX_SDP_E0_04_L + Address_Offset + usRegOffsetSDPByID, BIT6|BIT7, BIT6|BIT7); // Set read
	#endif

	return TRUE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetTrainingPatternFlag()
//  [Description]
//					mhal_DPRx_GetTrainingPatternFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetTrainingPatternFlag(DPRx_ID dprx_id)
{
    BOOL XDATA bTrainingFlag = FALSE;
    BYTE XDATA ucTrainingState = 0;
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return FALSE;
	}

    ucTrainingState = msReadByte(REG_DPRX_DPCD0_21_L + usRegOffsetDPCD0ByID) & (BIT0|BIT1);

    if(ucTrainingState != 0)
    {
        bTrainingFlag = TRUE;
    }

    return bTrainingFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CheckReceiveIdlePattern()
//  [Description]
//					mhal_DPRx_CheckReceiveIdlePattern
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_CheckReceiveIdlePattern(DPRx_DECODER_ID dprx_decoder_id)
{
    BOOL XDATA bIdlePatternFlag = FALSE;
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return FALSE;
	}

    bIdlePatternFlag = (msReadByte(REG_DPRX_DECODER_E0_2D_L + usRegOffsetDecoderByID) & BIT3)? TRUE : FALSE;

    return bIdlePatternFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CheckDPCDPowerState()
//  [Description]
//					mhal_DPRx_CheckDPCDPowerState
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_CheckDPCDPowerState(DPRx_AUX_ID dprx_aux_id)
{
    BOOL XDATA bPowerStateFlag = 0;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bPowerStateFlag = ((msReadByte(REG_DPRX_AUX_39_L + usRegOffsetAuxByID) & (BIT0|BIT1)) != BIT1)? TRUE : FALSE;

    return bPowerStateFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CheckCDRLock()
//  [Description]
//					mhal_DPRx_CheckCDRLock
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_CheckCDRLock(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, DPRx_PHY_ID dprx_phy_id, DPRx_ID OnlinePort)
{
	BOOL XDATA bCDRLockFlag = FALSE;
    BYTE XDATA ucLansCount = 0;
    WORD XDATA usLockStatus = 0;
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET000(dprx_id);
	WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET000(dprx_id);
	WORD usRegOffsetPHY0ByID = DP_REG_OFFSET400(dprx_phy_id);

	if((dprx_id == DPRx_ID_MAX) || (dprx_aux_id == DPRx_AUX_ID_MAX) || (dprx_phy_id == DPRx_PHY_ID_MAX))
	{
		return FALSE;
	}

    ucLansCount = msReadByte(REG_DPRX_DPCD0_20_H + usRegOffsetDPCD0ByID) & 0x07;
    usLockStatus = msRead2Byte(REG_DPRX_TRANS_CTRL_17_L + usRegOffsetTransCTRLByID);

    if((ucLansCount == DP_LANECOUNT_1) && (usLockStatus == 0x7))
    {
        bCDRLockFlag = TRUE;
    }
    else if((ucLansCount == DP_LANECOUNT_2) && (usLockStatus == 0x77))
    {
        bCDRLockFlag = TRUE;
    }
    else if((ucLansCount == DP_LANECOUNT_4) && (usLockStatus == 0x7777))
    {
        bCDRLockFlag = TRUE;
    }

    // For one lane at agc state , RTL issue . odinson2 also happen
    if((ucLansCount == DP_LANECOUNT_4) && (usLockStatus != 0x7777))
    {
        if((usLockStatus & 0x7) == 0x7)
        {
            mhal_DPRx_Check_AGC_Reset(dprx_phy_id);
        }
    }

	if(bCDRLockFlag == TRUE)
	{
		msWrite2Byte(REG_DPRX_PHY0_29_L + usRegOffsetPHY0ByID, 0x0028); // 201806, by analog RD's requests

        #if (DPRX_OFFLINE_TO_ONLINE_KEEP_LOCK == 0x1)
        if((mhal_DPRx_IsAuxAtOffLine(dprx_aux_id) == TRUE) && (OnlinePort == dprx_id))
        {
            mhal_DPRx_ResetAsyncFIFO(dprx_id, dprx_aux_id);
		    mhal_DPRx_SetOffLine(dprx_id, dprx_aux_id, FALSE); // For sometimes we will not turn-off offline mode at initial state, ex: for PM -> wake-up, training already lock
        }
        #endif

        // JL suggest turn off this bit when timer_a = 0 to avoid CR lose irq when unplug cable, only turn on this bit during normal training
        #if (DP_RX_FT_TIMER_A_WAIT_SQ_SIGNAL == 0x0)
        msWriteByteMask(REG_DPRX_PHY0_62_L + usRegOffsetPHY0ByID, 0x00, BIT7);
        #endif
	}

    return bCDRLockFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CheckLossCDRStatus()
//  [Description]
//					mhal_DPRx_CheckLossCDRStatus
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_CheckLossCDRStatus(DPRx_AUX_ID dprx_aux_id)
{
    BOOL XDATA bEventFlag = FALSE;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    if((msReadByte(REG_DPRX_AUX_39_L + usRegOffsetAuxByID) & (BIT0|BIT1)) != BIT1)
    {
        bEventFlag = TRUE;
    }

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CheckInterlaneSkewStatus()
//  [Description]
//					mhal_DPRx_CheckInterlaneSkewStatus
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_CheckInterlaneSkewStatus(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id)
{
    BOOL XDATA bEventFlag = FALSE;
	WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
    WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET000(dprx_id);

	if((dprx_id == DPRx_ID_MAX) || (dprx_aux_id == DPRx_AUX_ID_MAX))
	{
		return FALSE;
	}

    if((msReadByte(REG_DPRX_TRANS_CTRL_16_H + usRegOffsetTransCTRLByID) & BIT7) != BIT7)
    {
        if((msReadByte(REG_DPRX_AUX_39_L + usRegOffsetAuxByID) & (BIT0|BIT1)) != BIT1)
        {
            bEventFlag = TRUE;
        }
    }

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CheckLossAlignment()
//  [Description]
//					mhal_DPRx_CheckLossAlignment
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_CheckLossAlignment(DPRx_ID dprx_id)
{
    BOOL XDATA bAlignment = FALSE;
	WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return FALSE;
	}

    if(msReadByte(REG_DPRX_TRANS_CTRL_16_H + usRegOffsetTransCTRLByID) & BIT7)
    {
        bAlignment = TRUE;
    }

    return bAlignment;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CheckVPLLBigChange()
//  [Description]
//					mhal_DPRx_CheckVPLLBigChange
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_CheckVPLLBigChange(DPRx_DECODER_ID dprx_decoder_id)
{
	BOOL XDATA bBigChange = FALSE;
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return FALSE;
	}

	if(msReadByte(REG_DPRX_DECODER_E0_0C_L + usRegOffsetDecoderByID) & BIT1)
	{
	     bBigChange = TRUE;
	}

    return bBigChange;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ClrVideoBigChgFlag()
//  [Description]
//					mhal_DPRx_ClrVideoBigChgFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_ClrVideoBigChgFlag(DPRx_DECODER_ID dprx_decoder_id)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

	msWriteByte(REG_DPRX_DECODER_E0_0F_L + usRegOffsetDecoderByID, BIT1);
	mhal_DPRx_DELAY_NOP(5);
    msWriteByte(REG_DPRX_DECODER_E0_0F_L + usRegOffsetDecoderByID, 0);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableVspHwRegen()
//  [Description]
//					mhal_DPRx_EnableVspHwRegen
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableVspHwRegen(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_DECODER_E0_73_L + usRegOffsetDecoderByID, bEnable? BIT6 : 0, BIT6);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_DecoderOverwrite_Set()
//  [Description]
//                  mhal_DPRx_DecoderOverwrite_Set
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_DecoderOverwrite_Set(DPRx_DECODER_ID dprx_decoder_id, BOOL bOverwriteEnable, WORD usTargetValue)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(bOverwriteEnable == TRUE)
	{
	    msWrite2ByteMask(REG_DPRX_DECODER_E0_67_L + usRegOffsetDecoderByID, usTargetValue, usTargetValue);
	}
	else
	{
        msWrite2ByteMask(REG_DPRX_DECODER_E0_67_L + usRegOffsetDecoderByID, 0, usTargetValue);
	}

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableSEReplaceByBE()
//  [Description]
//					mhal_DPRx_EnableSEReplaceByBE
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableSEReplaceByBE(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

    msWriteByteMask(REG_DPRX_DECODER_E0_01_H + usRegOffsetDecoderByID, bEnable? BIT2 : 0, BIT2);

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_LongDIV()
//  [Description]
//					mhal_DPRx_LongDIV
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
DWORD mhal_DPRx_LongDIV(DWORD M_Value, WORD LinkRate, DWORD N_Value)
{
    struct
	{
        DWORD l0;
        DWORD h0;
    }Div;

    DWORD N;
    WORD uwLinkRate;
	WORD uwCount;
    DWORD d;
	DWORD cc;
	BYTE ubPreDivid; // To make the calculation more fast usage
	BYTE ubMulti1_MSB_BIT = 0; // M_Value
	BYTE ubMulti2_MSB_BIT = 0; // LinkRate

	if((M_Value == 0) || (LinkRate == 0) || (N_Value == 0))
	{
	    return 0;
	}

	ubPreDivid = BIT6; // Note this must be 2/4/6/8...

	// For "A/B", check A & B are both divide by 2/4/6/8..., if yes, take divide first
	while(ubPreDivid != BIT0)
	{
		if(((M_Value > ubPreDivid) & ((M_Value % ubPreDivid) == 0)) &&
		   ((N_Value > ubPreDivid) & ((N_Value % ubPreDivid) == 0)))
		{
			M_Value = M_Value / ubPreDivid;
			N_Value = N_Value / ubPreDivid;
		}

		if(((LinkRate > ubPreDivid) & ((LinkRate % ubPreDivid) == 0)) &&
		   ((N_Value > ubPreDivid) & ((N_Value % ubPreDivid) == 0)))
		{
			LinkRate = LinkRate / ubPreDivid;
			N_Value = N_Value / ubPreDivid;
		}

		ubPreDivid = ubPreDivid >> 1;
	}

	for(uwCount = (sizeof(M_Value)*8 - 1); uwCount > 0; uwCount--) // Search M_Value's MSB bit
	{
		if((M_Value & (((DWORD)BIT0) << uwCount)) != 0)
		{
			ubMulti1_MSB_BIT = uwCount;
			break;
		}
	}

	for(uwCount = (sizeof(LinkRate)*8 - 1); uwCount > 0; uwCount--) // Search LinkRate's MSB bit
	{
		if((LinkRate & (((DWORD)BIT0) << uwCount)) != 0)
		{
			ubMulti2_MSB_BIT = uwCount;
			break;
		}
	}

	// Jugde overflow or not, if not, just reply value
    if((ubMulti1_MSB_BIT + ubMulti2_MSB_BIT) < (sizeof(DWORD)*8 - 1))
    {
        return (((DWORD) M_Value * LinkRate) / N_Value);
    }

	Div.l0 = 0;
    Div.h0 = 0;
	cc = 0;

	// Ex: for 200x5, we hope 200 + 200 + ... + 200, instead of 5 + 5 + 5 + ... + 5
	if(M_Value > LinkRate)
	{
		uwLinkRate = LinkRate;
    	d = M_Value;
	}
	else
	{
		d = LinkRate;
    	uwLinkRate = M_Value;
	}

    N = N_Value;

    for(uwCount = 0 ; uwCount < uwLinkRate ; uwCount++)
    {
        Div.l0 = Div.l0 + d;
        if(((Div.l0) < d) || ( Div.l0 == 0))
        {
            Div.h0 = Div.h0 + 1;
        }
    }

    while(1)
    {
        if(Div.l0 >= N)
        {
            Div.l0 = Div.l0 - N;
            cc = cc + 1;
        }
        else
        {
            if(Div.h0)
            {
                Div.h0 = Div.h0 - 1;
                d = 0xFFFFFFFF - N;
                Div.l0 = d + 1 + Div.l0;
                cc = cc + 1;
            }
            else
            {
                return cc;
            }
        }
    }
}

void ________AUDIO________(void);
//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_AUPLLSetting()
//  [Description]
//					mhal_DPRx_AUPLLSetting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_AUPLLSetting(void)
{
    BYTE Audio_Num = 0;
    BYTE Audio_Function_Num = 1; // modify this number by chip.

    for(Audio_Num = 0; Audio_Num < Audio_Function_Num; Audio_Num++)
    {
	    msWrite2Byte(REG_DPRX_PLL_14_L + DP_REG_OFFSET002(Audio_Num), 0x00D9);
	    msWrite2Byte(REG_DPRX_PLL_30_L + DP_REG_OFFSET020(Audio_Num), 0x4400);
	    msWrite2Byte(REG_DPRX_PLL_31_L + DP_REG_OFFSET020(Audio_Num), 0x0000); // RD wish ictrl as 0
	    msWrite2Byte(REG_DPRX_PLL_32_L + DP_REG_OFFSET020(Audio_Num), 0x8112);
	    msWrite2Byte(REG_DPRX_PLL_3D_L + DP_REG_OFFSET020(Audio_Num), 0x0000);

	    msWrite2Byte(REG_DPRX_ALOCK0_0C_L + DP_REG_OFFSET100(Audio_Num), 0x0008); // Old mode

	    msWrite2Byte(REG_DPRX_ALOCK0_0B_L + DP_REG_OFFSET100(Audio_Num), 0x6565);
	    msWrite2Byte(REG_DPRX_ALOCK0_05_L + DP_REG_OFFSET100(Audio_Num), 0x0F21);
	    msWrite2Byte(REG_DPRX_ALOCK0_06_L + DP_REG_OFFSET100(Audio_Num), 0x0000);
	    msWrite2Byte(REG_DPRX_ALOCK0_07_L + DP_REG_OFFSET100(Audio_Num), 0x0002);
	    msWrite2Byte(REG_DPRX_ALOCK0_51_L + DP_REG_OFFSET100(Audio_Num), 0x0080);
	    msWrite2Byte(REG_DPRX_ALOCK0_0D_L + DP_REG_OFFSET100(Audio_Num), 0x0300);
	    msWrite2Byte(REG_DPRX_ALOCK0_20_L + DP_REG_OFFSET100(Audio_Num), 0x3331);

	    msWrite2Byte(REG_DPRX_ALOCK0_55_L + DP_REG_OFFSET100(Audio_Num), 0xC063); // Auto
	    msWrite2Byte(REG_DPRX_ALOCK0_54_L + DP_REG_OFFSET100(Audio_Num), 0xC00A); // Auto

        msWrite2Byte(REG_DPRX_AUDIO_CTRL0_4B_L + DP_REG_OFFSET100(Audio_Num), 0xE00C);
        msWrite2Byte(REG_DPRX_AUDIO_CTRL0_51_L + DP_REG_OFFSET100(Audio_Num), 0xFFFF);
        msWrite2Byte(REG_DPRX_AUDIO_CTRL0_52_L + DP_REG_OFFSET100(Audio_Num), 0xC000);
        msWriteByteMask(REG_DPRX_AUDIO_CTRL0_53_L + DP_REG_OFFSET100(Audio_Num), 0xC0, BIT7|BIT6);
        msWriteByte(REG_DPRX_AUDIO_CTRL0_54_H + DP_REG_OFFSET100(Audio_Num), 0x18);  //[15] = 1 : Audio left/right channel swap.
        msWriteByte(REG_DPRX_AUDIO_CTRL0_56_L + DP_REG_OFFSET100(Audio_Num), 0x80);
	}

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CheckAUPLLBigChange()
//  [Description]
//					mhal_DPRx_CheckAUPLLBigChange
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_CheckAUPLLBigChange(DPRx_DECODER_ID dprx_decoder_id)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);
	BOOL XDATA bBigChange = FALSE;
    DPRx_stDPAudioInfo XDATA stAudioTemp = {0, 0, 0, 0, 0};

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return FALSE;
	}

    stAudioTemp.ulMCodeValue = msReadByte(REG_DPRX_DECODER_E0_11_L + usRegOffsetDecoderByID); 									  // MAUD_CENTER[23:16]
    stAudioTemp.ulMCodeValue = (stAudioTemp.ulMCodeValue << 16) + msRead2Byte(REG_DPRX_DECODER_E0_10_L + usRegOffsetDecoderByID); // MAUD_CENTER[15:0]
    stAudioTemp.ulNCodeValue = msReadByte(REG_DPRX_DECODER_E0_19_L + usRegOffsetDecoderByID); 									  // NAUD_CENTER[23:16]
    stAudioTemp.ulNCodeValue = (stAudioTemp.ulNCodeValue << 16) + msRead2Byte(REG_DPRX_DECODER_E0_18_L + usRegOffsetDecoderByID); // NAUD_CENTER[15:0]

    stAudioInfo.ulMCodeValue = msReadByte(REG_DPRX_DECODER_E0_05_L + usRegOffsetDecoderByID); 									  // MAUD_CENTER[23:16]
    stAudioInfo.ulMCodeValue = (stAudioInfo.ulMCodeValue << 16) + msRead2Byte(REG_DPRX_DECODER_E0_04_L + usRegOffsetDecoderByID); // MAUD_CENTER[15:0]
    stAudioInfo.ulNCodeValue = msReadByte(REG_DPRX_DECODER_E0_07_L + usRegOffsetDecoderByID); 									  // NAUD_CENTER[23:16]
    stAudioInfo.ulNCodeValue = (stAudioInfo.ulNCodeValue << 16) + msRead2Byte(REG_DPRX_DECODER_E0_06_L + usRegOffsetDecoderByID); // NAUD_CENTER[15:0]

    if(ABS_MINUS_DP(stAudioInfo.ulMCodeValue, stAudioTemp.ulMCodeValue) > 0x10) // Compare M code
    {
        bBigChange = TRUE;
    }

    if(ABS_MINUS_DP(stAudioInfo.ulNCodeValue, stAudioTemp.ulNCodeValue) > 0x10) // Compare M code
    {
        bBigChange = TRUE;
    }

    return bBigChange;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetAUPLLBigChangeInterrupt()
//  [Description]
//					mhal_DPRx_SetAUPLLBigChangeInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetAUPLLBigChangeInterrupt(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_DECODER_E0_0F_L + usRegOffsetDecoderByID, BIT0, BIT0);
    mhal_DPRx_DELAY_NOP(5);
    msWriteByteMask(REG_DPRX_DECODER_E0_0F_L + usRegOffsetDecoderByID, 0, BIT0);

    msWriteByteMask(REG_DPRX_DECODER_E0_0D_L + usRegOffsetDecoderByID, bEnable? 0 : BIT0, BIT0); // [0] Audio MN big change IRQ

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetAudioMuteInterrupt()
//  [Description]
//					mhal_DPRx_SetAudioMuteInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetAudioMuteInterrupt(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_DECODER_E0_0F_L + usRegOffsetDecoderByID, BIT6, BIT6);
    mhal_DPRx_DELAY_NOP(5);
    msWriteByteMask(REG_DPRX_DECODER_E0_0F_L + usRegOffsetDecoderByID, 0, BIT6);

    msWriteByteMask(REG_DPRX_DECODER_E0_0D_L + usRegOffsetDecoderByID, bEnable? 0 : BIT6, BIT6); // [6] Audio mute IRQ

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetAudioChanneStatusChgInterrupt()
//  [Description]
//					mhal_DPRx_SetAudioChanneStatusChgInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetAudioChanneStatusChgInterrupt(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_DECODER_E0_0F_L + usRegOffsetDecoderByID, BIT7, BIT7);
    mhal_DPRx_DELAY_NOP(5);
    msWriteByteMask(REG_DPRX_DECODER_E0_0F_L + usRegOffsetDecoderByID, 0, BIT7);

    msWriteByteMask(REG_DPRX_DECODER_E0_0D_L + usRegOffsetDecoderByID, bEnable? 0 : BIT7, BIT7); // [7] Audio channel status change IRQ

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetMISC0ChangeInterrupt()
//  [Description]
//					mhal_DPRx_SetMISC0ChangeInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetMISC0ChangeInterrupt(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable)
{
	WORD usRegOffsetSdpByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

    msWriteByteMask(REG_DPRX_SDP_E0_47_H + usRegOffsetSdpByID, BIT3, BIT3);
    mhal_DPRx_DELAY_NOP(10);
    msWriteByteMask(REG_DPRX_SDP_E0_47_H + usRegOffsetSdpByID, 0, BIT3);

    msWriteByteMask(REG_DPRX_SDP_E0_45_H + usRegOffsetSdpByID, bEnable? 0 : BIT3, BIT3); // [11] MISC0 change

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetMISC1ChangeInterrupt()
//  [Description]
//					mhal_DPRx_SetMISC1ChangeInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetMISC1ChangeInterrupt(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable)
{
	WORD usRegOffsetSdpByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

    msWriteByteMask(REG_DPRX_SDP_E0_47_H + usRegOffsetSdpByID, BIT4, BIT4);
    mhal_DPRx_DELAY_NOP(10);
    msWriteByteMask(REG_DPRX_SDP_E0_47_H + usRegOffsetSdpByID, 0, BIT4);

    msWriteByteMask(REG_DPRX_SDP_E0_45_H + usRegOffsetSdpByID, bEnable? 0 : BIT4, BIT4); // [12] MISC1 change

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_UpdateAudioMNCode()
//  [Description]
//					mhal_DPRx_UpdateAudioMNCode
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_UpdateAudioMNCode(DPRx_DECODER_ID dprx_decoder_id, DWORD ulAudioMCode, DWORD ulAudioNCode)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

	msWrite2Byte(REG_DPRX_DECODER_E0_10_L + usRegOffsetDecoderByID, ulAudioMCode & 0xFFFF); // MAUD_CENTER[15:0]
    msWriteByte(REG_DPRX_DECODER_E0_11_L + usRegOffsetDecoderByID, ulAudioMCode >> 16); 	// MAUD_CENTER[23:16]
    msWrite2Byte(REG_DPRX_DECODER_E0_18_L + usRegOffsetDecoderByID, ulAudioNCode & 0xFFFF); // NAUD_CENTER[15:0]
    msWriteByte(REG_DPRX_DECODER_E0_19_L + usRegOffsetDecoderByID, ulAudioNCode >> 16); 	// NAUD_CENTER[23:16]

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_AudioChannelCnt()
//  [Description]
//					mhal_DPRx_AudioChannelCnt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mhal_DPRx_AudioChannelCnt(DPRx_DECODER_ID dprx_decoder_id, BYTE *pPacketData)
{
	BYTE ucSDPData = 0;
	WORD usRegOffsetSdpByID = DP_REG_OFFSET100(dprx_decoder_id);

	if((dprx_decoder_id == DPRx_DECODER_ID_MAX) || (pPacketData == NULL))
	{
		return 0;
	}

    ucSDPData = msReadByte(REG_DPRX_SDP_E0_08_L + usRegOffsetSdpByID);

    msWriteByteMask(REG_DPRX_SDP_E0_08_L + usRegOffsetSdpByID, DP_SDP_SEL_AUDIO_STREAM, BMASK(3:0)); // Choose Audio stream packet

    if(msReadByte(REG_DPRX_SDP_E0_0F_H + usRegOffsetSdpByID) >= 2)
    {
		pPacketData[0] = 8;
    }
    else
    {
		pPacketData[0] = 2;
    }

    msWriteByte(REG_DPRX_SDP_E0_08_L + usRegOffsetSdpByID, ucSDPData);

  	return pPacketData[0];
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetAudioMaskCount()
//  [Description]
//                  mhal_DPRx_SetAudioMaskCount
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetAudioMaskCount(DPRx_ID dprx_id, BYTE ucMaskCount)
{
	DPRx_AudioStream DPRX_AUDIO_SELECT = mhal_DPRx_GetAudioStream(dprx_id);
	WORD usRegOffsetAlock0ByID = DP_REG_OFFSET100(DPRX_AUDIO_SELECT);

	if(DPRX_AUDIO_SELECT == DPRx_AUD_STREAM_MAX)
	{
		return;
	}

    msWriteByte(REG_DPRX_ALOCK0_53_H + usRegOffsetAlock0ByID, ucMaskCount);//0x8000(UCD-400 OK / RX-680 fail) //0x4000(UCD-400/QD882 fail)

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CheckAudioAbsent()
//  [Description]
//					mhal_DPRx_CheckAudioAbsent
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_CheckAudioAbsent(DPRx_DECODER_ID dprx_decoder_id, BYTE times)
{
    BOOL XDATA bNoAudio = FALSE;
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);
	WORD usRegOffsetSdpByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return FALSE;
	}

    while(times--)
    {
        if(msReadByte(REG_DPRX_DECODER_E0_2D_L + usRegOffsetDecoderByID) & BIT4) // VBID[4]: AudioMute_Flag
        {
            bNoAudio = TRUE;
        }
        else if((msReadByte(REG_DPRX_SDP_E0_02_L + usRegOffsetSdpByID) & BIT1) == 0) // Check audio status
        {
            bNoAudio = TRUE;
        }
        else
        {
            msWriteByte(REG_DPRX_SDP_E0_02_L + usRegOffsetSdpByID, BIT1);

            return FALSE;
        }
    }

    msWriteByte(REG_DPRX_SDP_E0_02_L + usRegOffsetSdpByID, BIT1);

    return bNoAudio;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetAudioSampleFreq()
//  [Description]
//					mhal_DPRx_GetAudioSampleFreq
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mhal_DPRx_GetAudioSampleFreq(DPRx_DECODER_ID dprx_decoder_id)
{
    DPRx_AudioFreqType XDATA ucFreqVaild = DPRx_AUD_FREQ_ERROR;
    WORD usRegOffsetSdpByID = DP_REG_OFFSET100(dprx_decoder_id);
    BYTE XDATA ucDPAudioFreq;

    if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
    {
        return DPRx_AUD_FREQ_ERROR;
    }

    ucDPAudioFreq = (msReadByte(REG_DPRX_SDP_E0_31_H + usRegOffsetSdpByID)&0xF);

    if(ucDPAudioFreq == 0x03)
    {
        ucFreqVaild = DPRx_AUD_FREQ_32K;
    }
    else if(ucDPAudioFreq == 0x00)
    {
        ucFreqVaild = DPRx_AUD_FREQ_44K;
    }
    else if(ucDPAudioFreq == 0x02)
    {
        ucFreqVaild = DPRx_AUD_FREQ_48K;
    }
    else if(ucDPAudioFreq == 0x08)
    {
        ucFreqVaild = DPRx_AUD_FREQ_88K;
    }
    else if(ucDPAudioFreq == 0x0A)
    {
        ucFreqVaild = DPRx_AUD_FREQ_96K;
    }
    else if(ucDPAudioFreq == 0x0C)
    {
        ucFreqVaild = DPRx_AUD_FREQ_176K;
    }
    else if(ucDPAudioFreq == 0x0E)
    {
        ucFreqVaild = DPRx_AUD_FREQ_192K;
    }
    else
    {
        ucFreqVaild = DPRx_AUD_FREQ_ERROR;
    }

    DPRX_HAL_DPRINTF("\r\n DP ucDPAudioFreq by packet = %d", ucFreqVaild);

    return ucFreqVaild;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CalAudioFreqByIVS()
//  [Description]
//                  mhal_DPRx_CalAudioFreqByIVS
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mhal_DPRx_CalAudioFreqByIVS(DPRx_ID dprx_id)
{
    DPRx_AudioStream DPRX_AUDIO_SELECT = mhal_DPRx_GetAudioStream(dprx_id);
    WORD usRegOffsetAlock0ByID = DP_REG_OFFSET100(DPRX_AUDIO_SELECT);
    DPRx_AudioFreqType XDATA ucFreqVaild = DPRx_AUD_FREQ_ERROR;
    WORD XDATA usDPAudioIVS;
    BYTE XDATA ucDPAudioFreq;

	if(DPRX_AUDIO_SELECT == DPRx_AUD_STREAM_MAX)
    {
        return FALSE;
    }

    usDPAudioIVS = (msRead2Byte(REG_DPRX_ALOCK0_21_L+ usRegOffsetAlock0ByID));

    if(usDPAudioIVS != 0)
    {
        ucDPAudioFreq = (12*1000ul)/usDPAudioIVS;

        DPRX_HAL_DPRINTF("\r\n DP ucDPAudioFreqByIVS %d", ucDPAudioFreq);

        if(diff(ucDPAudioFreq , 32 ) < 2)
        {
            ucFreqVaild = DPRx_AUD_FREQ_32K;
        }
        else if(diff(ucDPAudioFreq , 44 ) < 2)
        {
            ucFreqVaild = DPRx_AUD_FREQ_44K;
        }
        else if(diff(ucDPAudioFreq , 48 ) < 2)
        {
            ucFreqVaild = DPRx_AUD_FREQ_48K;
        }
        else if(diff(ucDPAudioFreq , 88 ) < 2)
        {
            ucFreqVaild = DPRx_AUD_FREQ_88K;
        }
        else if(diff(ucDPAudioFreq , 96 ) < 2)
        {
            ucFreqVaild = DPRx_AUD_FREQ_96K;
        }
        else if(diff(ucDPAudioFreq , 176 ) < 2)
        {
            ucFreqVaild = DPRx_AUD_FREQ_176K;
        }
        else if(diff(ucDPAudioFreq , 192 ) < 2)
        {
            ucFreqVaild = DPRx_AUD_FREQ_192K;
        }
        else
        {
            ucFreqVaild = DPRx_AUD_FREQ_ERROR;
        }
    }

    return ucFreqVaild;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CalAudioFreq()
//  [Description]
//                  mhal_DPRx_CalAudioFreq
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mhal_DPRx_CalAudioFreq(DPRx_ID dprx_id, DPRx_DECODER_ID dprx_decoder_id)
{
    DPRx_AudioFreqType XDATA ucFreqVaild = DPRx_AUD_FREQ_ERROR;
    WORD XDATA wDPAudioFreq;
    DWORD XDATA usLSClock = mhal_DPRx_GetLinkRate(dprx_id);
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);
	WORD ubDivider = BIT7;
	unsigned long long Temp;

	if((dprx_id == DPRx_ID_MAX) || (dprx_decoder_id == DPRx_DECODER_ID_MAX))
	{
		return ucFreqVaild;
	}

    stAudioInfo.ulMCodeValue = msReadByte(REG_DPRX_DECODER_E0_05_L + usRegOffsetDecoderByID); 									  // MAUD_CENTER[23:16]
    stAudioInfo.ulMCodeValue = (stAudioInfo.ulMCodeValue << 16) + msRead2Byte(REG_DPRX_DECODER_E0_04_L + usRegOffsetDecoderByID); // MAUD_CENTER[15:0]
    stAudioInfo.ulNCodeValue = msReadByte(REG_DPRX_DECODER_E0_07_L + usRegOffsetDecoderByID); 									  // NAUD_CENTER[23:16]
    stAudioInfo.ulNCodeValue = (stAudioInfo.ulNCodeValue << 16) + msRead2Byte(REG_DPRX_DECODER_E0_06_L + usRegOffsetDecoderByID); // NAUD_CENTER[15:0]

	while(ubDivider > 1)
	{
		if(((stAudioInfo.ulMCodeValue % ubDivider) == 0x0) &&
			(stAudioInfo.ulMCodeValue > ubDivider) &&
			((stAudioInfo.ulNCodeValue % ubDivider) == 0x0) &&
			(stAudioInfo.ulNCodeValue > ubDivider))
		{
			stAudioInfo.ulMCodeValue = (stAudioInfo.ulMCodeValue / ubDivider);
			stAudioInfo.ulNCodeValue = (stAudioInfo.ulNCodeValue / ubDivider);
		}

		ubDivider = ubDivider / 2;
	}

	if((stAudioInfo.ulMCodeValue != 0) && (stAudioInfo.ulNCodeValue != 0))
    {
		ubDivider = 0x1;

    	if((stAudioInfo.ulMCodeValue % 512) == 0x0)
    	{
			ubDivider = 512;
    	}
		else if((stAudioInfo.ulMCodeValue % 256) == 0x0)
    	{
			ubDivider = 256;
    	}
		else if((stAudioInfo.ulMCodeValue % 128) == 0x0)
    	{
			ubDivider = 128;
    	}
		else if((stAudioInfo.ulMCodeValue % 64) == 0x0)
    	{
			ubDivider = 64;
    	}
		else if((stAudioInfo.ulMCodeValue % 32) == 0x0)
    	{
			ubDivider = 32;
    	}
		else if((stAudioInfo.ulMCodeValue % 16) == 0x0)
    	{
			ubDivider = 16;
    	}
		else if((stAudioInfo.ulMCodeValue % 8) == 0x0)
    	{
			ubDivider = 8;
    	}

		if(ubDivider > 8)
		{
			//wDPAudioFreq = (((usLSClock * 1000ul) + 256ul) * (stAudioInfo.ulMCodeValue / ubDivider)) / (stAudioInfo.ulNCodeValue * (512ul / ubDivider));

			Temp = ((unsigned long long)((usLSClock * 1000ul) + 256ul) * (stAudioInfo.ulMCodeValue / ubDivider)) / (stAudioInfo.ulNCodeValue * (512ul / ubDivider));
			wDPAudioFreq = Temp & 0xFFFF;
		}
		else
		{
			//wDPAudioFreq = ((((usLSClock * 1000ul) + 256ul) / 8) * (stAudioInfo.ulMCodeValue)) / (stAudioInfo.ulNCodeValue * (512ul / 8));

			Temp = ((unsigned long long)(((usLSClock * 1000ul) + 256ul) / 8) * (stAudioInfo.ulMCodeValue)) / (stAudioInfo.ulNCodeValue * (512ul / 8));
			wDPAudioFreq = Temp & 0xFFFF;
		}
    }
    else
    {
		// Roll back MN value
		stAudioInfo.ulMCodeValue = msReadByte(REG_DPRX_DECODER_E0_05_L + usRegOffsetDecoderByID); // REG_DP_OP_40[23:16]: recived audio M value
		stAudioInfo.ulMCodeValue = (stAudioInfo.ulMCodeValue << 16) + msRead2Byte(REG_DPRX_DECODER_E0_04_L + usRegOffsetDecoderByID); // REG_DP_OP_40[15:0]: recived audio M value
		stAudioInfo.ulNCodeValue = msReadByte(REG_DPRX_DECODER_E0_07_L + usRegOffsetDecoderByID); // REG_DP_OP_44[23:16]: recived audio N value
		stAudioInfo.ulNCodeValue = (stAudioInfo.ulNCodeValue << 16) + msRead2Byte(REG_DPRX_DECODER_E0_06_L + usRegOffsetDecoderByID); // REG_DP_OP_44[15:0]: recived audio N value

		ucFreqVaild = DPRx_AUD_FREQ_ERROR;

		return ucFreqVaild;
    }

	// Roll back MN value
   	stAudioInfo.ulMCodeValue = msReadByte(REG_DPRX_DECODER_E0_05_L + usRegOffsetDecoderByID); // REG_DP_OP_40[23:16]: recived audio M value
   	stAudioInfo.ulMCodeValue = (stAudioInfo.ulMCodeValue << 16) + msRead2Byte(REG_DPRX_DECODER_E0_04_L + usRegOffsetDecoderByID); // REG_DP_OP_40[15:0]: recived audio M value
   	stAudioInfo.ulNCodeValue = msReadByte(REG_DPRX_DECODER_E0_07_L + usRegOffsetDecoderByID); // REG_DP_OP_44[23:16]: recived audio N value
   	stAudioInfo.ulNCodeValue = (stAudioInfo.ulNCodeValue << 16) + msRead2Byte(REG_DPRX_DECODER_E0_06_L + usRegOffsetDecoderByID); // REG_DP_OP_44[15:0]: recived audio N value

    if(diff(wDPAudioFreq, 32) < 3)
    {
        ucFreqVaild = DPRx_AUD_FREQ_32K;
    }
    else if(diff(wDPAudioFreq, 44) < 3)
    {
        ucFreqVaild = DPRx_AUD_FREQ_44K;
    }
    else if(diff(wDPAudioFreq, 48) < 3)
    {
        ucFreqVaild = DPRx_AUD_FREQ_48K;
    }
    else if(diff(wDPAudioFreq, 88) < 3)
    {
        ucFreqVaild = DPRx_AUD_FREQ_88K;
    }
    else if(diff(wDPAudioFreq, 96) < 3)
    {
        ucFreqVaild = DPRx_AUD_FREQ_96K;
    }
    else if(diff(wDPAudioFreq, 176) < 3)
    {
        ucFreqVaild = DPRx_AUD_FREQ_176K;
    }
    else if(diff(wDPAudioFreq, 192) < 3)
    {
        ucFreqVaild = DPRx_AUD_FREQ_192K;
    }
    else
    {
        ucFreqVaild = DPRx_AUD_FREQ_ERROR;
    }

    mhal_DPRx_UpdateMAUDValue(dprx_decoder_id);

    return ucFreqVaild;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_AudioCheckStable()
//  [Description]
//					mhal_DPRx_AudioCheckStable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_AudioCheckStable(DPRx_ID dprx_id, BYTE Times)
{
	DPRx_AudioStream DPRX_AUDIO_SELECT = mhal_DPRx_GetAudioStream(dprx_id);
	WORD usRegOffsetAlock0ByID = DP_REG_OFFSET100(DPRX_AUDIO_SELECT);

	if(DPRX_AUDIO_SELECT == DPRx_AUD_STREAM_MAX)
	{
		return FALSE;
	}

    while(Times--)
    {
        if((msRead2Byte(REG_DPRX_ALOCK0_2A_L + usRegOffsetAlock0ByID) == DPRX_AUDIO_CS_STABLE) || ((msReadByte(REG_DPRX_ALOCK0_11_L + usRegOffsetAlock0ByID) <= DPRX_AUDIO_PHASE_DIFF_RANGE) && (msRead2Byte(REG_DPRX_ALOCK0_2A_L + usRegOffsetAlock0ByID) == DPRX_AUDIO_CS_STABLE1)))
        {

        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_AudioCheckFIFOStatus()
//  [Description]
//					mhal_DPRx_AudioCheckFIFOStatus
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_AudioCheckFIFOStatus(DPRx_ID dprx_id)
{
	DPRx_AudioStream DPRX_AUDIO_SELECT = mhal_DPRx_GetAudioStream(dprx_id);
	WORD usRegOffsetAlock0ByID = DP_REG_OFFSET100(DPRX_AUDIO_SELECT);

	if(DPRX_AUDIO_SELECT == DPRx_AUD_STREAM_MAX)
	{
		return;
	}

    if((msRead2Byte(REG_DPRX_ALOCK0_2A_L + usRegOffsetAlock0ByID) == DPRX_AUDIO_CS_STABLE1) && (msReadByte(REG_DPRX_ALOCK0_11_L + usRegOffsetAlock0ByID) >= DPRX_AUDIO_PHASE_DIFF_RESET))
    {
        msWriteByteMask((REG_DPRX_ALOCK0_00_L + usRegOffsetAlock0ByID), 0x0, BIT0);
        Delay1ms(1);
        msWriteByteMask((REG_DPRX_ALOCK0_00_L + usRegOffsetAlock0ByID), BIT0, BIT0);
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetAudioBigChgFlag()
//  [Description]
//					mhal_DPRx_GetAudioBigChgFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetAudioBigChgFlag(DPRx_DECODER_ID dprx_decoder_id)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return FALSE;
	}

	return (msReadByte(REG_DPRX_DECODER_E0_0C_L + usRegOffsetDecoderByID) & BIT0)? TRUE : FALSE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ClrAudioBigChgFlag()
//  [Description]
//					mhal_DPRx_ClrAudioBigChgFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_ClrAudioBigChgFlag(DPRx_DECODER_ID dprx_decoder_id)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

	msWriteByte(REG_DPRX_DECODER_E0_0F_L + usRegOffsetDecoderByID, BIT0);
	mhal_DPRx_DELAY_NOP(5);
    msWriteByte(REG_DPRX_DECODER_E0_0F_L + usRegOffsetDecoderByID, 0);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetAudioPath()
//  [Description]
//                  mhal_DPRx_SetAudioPath
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mhal_DPRx_SetAudioPath(DPRx_ID dprx_id, DPRx_DECODER_ID dprx_decoder_id, DPRx_AudioStream dprx_audio_stream, BOOL ubEnable)
{
    BYTE ubRet = TRUE;

	if((dprx_id == DPRx_ID_MAX) || (dprx_decoder_id == DPRx_DECODER_ID_MAX) || (dprx_audio_stream == DPRx_AUD_STREAM_MAX))
	{
		return FALSE;
	}

    if(dprx_audio_stream < DPRx_AUD_STREAM_MAX)
    {
        if(ubEnable == TRUE)
        {
            AUDIOSTREAM_USED_RX[dprx_audio_stream] = dprx_id;

            if(dprx_id < DPRx_ID_MAX)
            {
                msWriteByteMask(REG_DPRX2_TOP_GP_19_H, (dprx_id << 6), 0xC0);
            }
            else
            {
                ubRet = FALSE;
            }

            if(dprx_decoder_id < DPRx_DECODER_ID_MAX)
            {
                msWriteByteMask(REG_DPRX2_TOP_GP_62_H, dprx_decoder_id, 0x07);
            }
            else
            {
                ubRet = FALSE;
            }
        }
        else
        {
            AUDIOSTREAM_USED_RX[dprx_audio_stream] = DPRx_ID_MAX;
            msWriteByteMask(REG_DPRX2_TOP_GP_19_H, 0xC0, 0xC0);
            msWriteByteMask(REG_DPRX2_TOP_GP_62_H, DPRx_DECODER_ID_MAX, 0x07);
        }
    }
    else
    {
        ubRet = FALSE;
    }

    return ubRet;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetAudioGlobeMute()
//  [Description]
//                  mhal_DPRx_GetAudioGlobeMute
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetAudioGlobeMute(DPRx_ID dprx_id)
{
	DPRx_AudioStream DPRX_AUDIO_SELECT = mhal_DPRx_GetAudioStream(dprx_id);
    WORD usRegOffsetAudioCTRL0ByID = DP_REG_OFFSET100(DPRX_AUDIO_SELECT);

	if(dprx_id == DPRx_ID_MAX)
	{
		return FALSE;
	}

    return (msReadByte(REG_DPRX_AUDIO_CTRL0_4B_L + usRegOffsetAudioCTRL0ByID) & BIT2)? TRUE : FALSE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetNoAudioFlag()
//  [Description]
//					mhal_DPRx_GetNoAudioFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetNoAudioFlag(DPRx_DECODER_ID dprx_decoder_id)
{
	BOOL bNoAudioFlag = 0;
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return FALSE;
	}

	bNoAudioFlag = (msReadByte(REG_DPRX_DECODER_E0_2D_L + usRegOffsetDecoderByID) & BIT4)? TRUE : FALSE;

	return bNoAudioFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetAudioStream()
//  [Description]
//					mhal_DPRx_GetAudioStream
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
DPRx_AudioStream mhal_DPRx_GetAudioStream(DPRx_ID dprx_id)  //need to modify the initial vaule of DPRX_AUDIO_SELECT when IC have more than one audio engine
{
	volatile DPRx_AudioStream DPRX_AUDIO_SELECT = DPRx_AUD_STREAM_0; //DPRx_AUD_STREAM_MAX;

	if(dprx_id == DPRx_ID_MAX)
	{
		return DPRx_AUD_STREAM_MAX;
	}
	/*
    if(AUDIOSTREAM_USED_RX[0] == dprx_id) // Audio_Engine_0
    {
		DPRX_AUDIO_SELECT = DPRx_AUD_STREAM_0;
    }
    else if(AUDIOSTREAM_USED_RX[1] == dprx_id) // Audio_Engine_1
    {
        DPRX_AUDIO_SELECT = DPRx_AUD_STREAM_1;
    }
	*/
    return DPRX_AUDIO_SELECT;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_IsAudioFmtPCM()
//  [Description]
//					mhal_DPRx_IsAudioFmtPCM
//  [Arguments]:
//
//  [Return]:  		0: NON_PCM, 1: PCM
//
//**************************************************************************
DP_AUDIO_FORMAT mhal_DPRx_IsAudioFmtPCM(DPRx_DECODER_ID dprx_decoder_id)
{
	DP_AUDIO_FORMAT states = DP_AUDIO_FMT_PCM;
	WORD usRegOffsetSdpByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return DP_AUDIO_FMT_UNKNOWN;
	}

	if(msReadByte(REG_DPRX_SDP_E0_30_L + usRegOffsetSdpByID) & BIT1)
	{
		states = DP_AUDIO_FMT_NON_PCM;
	}

	return states;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_AudioGlobalMute()
//  [Description]
//                  mhal_DPRx_AudioGlobalMute
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_AudioGlobalMute(DPRx_AudioStream dprx_audio_select, BOOL bEnable)
{
	volatile WORD usRegOffsetAudioCTRL0ByID = DP_REG_OFFSET100(dprx_audio_select);

    if(dprx_audio_select == DPRx_AUD_STREAM_MAX)
    {
        return;
    }

    msWriteByteMask(REG_DPRX_AUDIO_CTRL0_4B_L + usRegOffsetAudioCTRL0ByID, bEnable? BIT2 : 0, BIT2);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_AudioStreamHB3()
//  [Description]
//					mhal_DPRx_AudioStreamHB3
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mhal_DPRx_AudioStreamHB3(DPRx_DECODER_ID dprx_decoder_id)
{
	BYTE ucSDPData = 0;
	BYTE HB3 = 0;
	WORD usRegOffsetSdpByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return 0;
	}

    ucSDPData = msReadByte(REG_DPRX_SDP_E0_08_L + usRegOffsetSdpByID);

    msWriteByteMask(REG_DPRX_SDP_E0_08_L + usRegOffsetSdpByID, DP_SDP_SEL_AUDIO_STREAM, BMASK(3:0)); // Choose Audio stream packet

    HB3 = msReadByte(REG_DPRX_SDP_E0_0F_H + usRegOffsetSdpByID);

    msWriteByte(REG_DPRX_SDP_E0_08_L + usRegOffsetSdpByID, ucSDPData);

  	return HB3;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_AudioAlockReset()
//  [Description]
//                  mhal_DPRx_AudioAlockReset
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_AudioAlockReset(DPRx_AudioStream dprx_audio_select)
{
	volatile WORD usRegOffsetAudioALOCK0ByID = DP_REG_OFFSET100(dprx_audio_select);

    if(dprx_audio_select == DPRx_AUD_STREAM_MAX)
    {
        return;
    }

    msWriteByteMask(REG_DPRX_ALOCK0_00_L + usRegOffsetAudioALOCK0ByID, 0, BIT0);
    mhal_DPRx_DELAY_NOP(15);
    msWriteByteMask(REG_DPRX_ALOCK0_00_L + usRegOffsetAudioALOCK0ByID, BIT0, BIT0);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_AudioFPLLReset()
//  [Description]
//                  mhal_DPRx_AudioFPLLReset
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_AudioFPLLReset(DPRx_AudioStream dprx_audio_select)
{
	volatile WORD usRegOffsetAudioALOCK0ByID = DP_REG_OFFSET100(dprx_audio_select);

    if(dprx_audio_select == DPRx_AUD_STREAM_MAX)
    {
        return;
    }

    msWriteByteMask(REG_DPRX_ALOCK0_54_H + usRegOffsetAudioALOCK0ByID, 0, BIT7);
    msWriteByteMask(REG_DPRX_ALOCK0_0C_L + usRegOffsetAudioALOCK0ByID, 0, BIT3);
    mhal_DPRx_DELAY_NOP(15);
    msWriteByteMask(REG_DPRX_ALOCK0_0C_L + usRegOffsetAudioALOCK0ByID, BIT3, BIT3);
    msWriteByteMask(REG_DPRX_ALOCK0_54_H + usRegOffsetAudioALOCK0ByID, BIT7, BIT7);

    return;
}

void ________INTERRUPT________(void);
//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableAuxInterrupt()
//  [Description]
//					mhal_DPRx_EnableAuxInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableAuxInterrupt(DPRx_AUX_ID dprx_aux_id, BOOL bEnable)
{
    mhal_DPRx_EnableAuxInterruptSetting(dprx_aux_id, bEnable);
    mhal_DPRx_EnableMCCSInterrupt(dprx_aux_id, bEnable);

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableReceiverInterrupt()
//  [Description]
//					mhal_DPRx_EnableReceiverInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableReceiverInterrupt(DPRx_ID dprx_id, BOOL bEnable)
{
    //==================================================================================================
	//========================================   Training IRQ   ========================================
	//==================================================================================================
    mhal_DPRx_SetTrainingPattern1Interrupt(dprx_id, bEnable);
    mhal_DPRx_SetTrainingPattern2Interrupt(dprx_id, bEnable);
    mhal_DPRx_SetTrainingPattern3Interrupt(dprx_id, bEnable);
    mhal_DPRx_SetTrainingPattern4Interrupt(dprx_id, bEnable);
    mhal_DPRx_EnableDPCD270Interrupt(dprx_id, bEnable);
    mhal_DPRx_EnableDPCDTrainingInterrupt(dprx_id, bEnable);
    mhal_DPRx_EnableDPCD102Interrupt(dprx_id, bEnable);

    //==================================================================================================
	//=======================================   Main Link IRQ   ========================================
	//==================================================================================================
    mhal_DPRx_EnableFTInterrupt(dprx_id, bEnable);
    mhal_DPRx_EnableAKSVInterrupt(dprx_id, bEnable);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableMCCSInterrupt()
//  [Description]
//					mhal_DPRx_EnableMCCSInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableMCCSInterrupt(DPRx_AUX_ID dprx_aux_id, BOOL bEnable)
{
	WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_AUX_3C_H + usRegOffsetAuxByID, BIT7, BIT7);
	mhal_DPRx_DELAY_NOP(5);
	msWriteByteMask(REG_DPRX_AUX_3C_H + usRegOffsetAuxByID, 0, BIT7);

	msWriteByteMask(REG_DPRX_AUX_3D_H + usRegOffsetAuxByID, bEnable? 0 : BIT7, BIT7); // MCCS
	msWriteByteMask(REG_DPRX_AUX_4D_L + usRegOffsetAuxByID, BIT1, BIT1); // MCCS reply by MCU

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableAuxInterruptSetting()
//  [Description]
//					mhal_DPRx_EnableAuxInterruptSetting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableAuxInterruptSetting(DPRx_AUX_ID dprx_aux_id, BOOL bEnable)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

    if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

	if(bEnable == TRUE)
	{
		//msWriteByteMask(REG_001DA9, 0, BIT5); // Set dp_aux_int_all mask (mask irq interrupt[13] for hst2)
		msWriteByteMask(REG_DPRX_PM_TOP_04_L, 0, (BIT0 << (dprx_aux_id)));  // Unmask AUX Rx IRQ
		msWriteByteMask(REG_DPRX_AUX_DPCD1_04_L + usRegOffsetAuxByID, 0, (BIT0 << dprx_aux_id));	// Set AUX port interrupt mask
	}
	else
	{
		//msWriteByteMask(REG_001DA9, BIT5, BIT5);
		msWriteByteMask(REG_DPRX_PM_TOP_04_L, (BIT0 << (dprx_aux_id)), (BIT0 << (dprx_aux_id))); // Mask AUX Rx IRQ
		msWriteByteMask(REG_DPRX_AUX_DPCD1_04_L + usRegOffsetAuxByID, (BIT0 << dprx_aux_id), (BIT0 << dprx_aux_id));
	}

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableDPAUXRecvInterrupt()
//  [Description]
//					mhal_DPRx_EnableDPAUXRecvInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableDPAUXRecvInterrupt(DPRx_AUX_ID dprx_aux_id, BOOL bEnable)
{
	WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_AUX_DPCD_4D_H + usRegOffsetAuxByID, BIT7, BIT7);
	mhal_DPRx_DELAY_NOP(5);
	msWriteByteMask(REG_DPRX_AUX_DPCD_4D_H + usRegOffsetAuxByID, 0, BIT7);

	msWriteByteMask(REG_DPRX_AUX_DPCD_49_L + usRegOffsetAuxByID, bEnable? 0 : BIT7, BIT7); // [7] Rx receiver IRQ

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_AuxTimeoutInterruptFlag_Get()
//  [Description]
//					mhal_DPRx_AuxTimeoutInterruptFlag_Get
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_AuxTimeoutInterruptFlag_Get(DPRx_AUX_ID dprx_aux_id)
{
	WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

	if((msReadByte(REG_DPRX_AUX_48_L + usRegOffsetAuxByID) & BIT4) == BIT4)
    {
	    return TRUE;
	}
	else
	{
	    return FALSE;
	}
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_AuxTimeoutInterruptFlag_Clear()
//  [Description]
//					mhal_DPRx_AuxTimeoutInterruptFlag_Clear
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_AuxTimeoutInterruptFlag_Clear(DPRx_AUX_ID dprx_aux_id)
{
	WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_AUX_49_L + usRegOffsetAuxByID, BIT4, BIT4);
	mhal_DPRx_DELAY_NOP(5);
	msWriteByteMask(REG_DPRX_AUX_49_L + usRegOffsetAuxByID, 0, BIT4);

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetCDRLossLockInterrupt()
//  [Description]
//					mhal_DPRx_SetCDRLossLockInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetCDRLossLockInterrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_RECEIVER_61_L + usRegOffsetReceiverByID, BIT0, BIT0);
	mhal_DPRx_DELAY_NOP(5);
    msWriteByteMask(REG_DPRX_RECEIVER_61_L + usRegOffsetReceiverByID, 0, BIT0);

    msWriteByteMask(REG_DPRX_RECEIVER_5D_L + usRegOffsetReceiverByID, bEnable? 0 : BIT0, BIT0); // [0] CR lose lock IRQ

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetMainLinkLossLockInterrupt()
//  [Description]
//					mhal_DPRx_SetMainLinkLossLockInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetMainLinkLossLockInterrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_RECEIVER_61_L + usRegOffsetReceiverByID, BIT1, BIT1);
	mhal_DPRx_DELAY_NOP(5);
	msWriteByteMask(REG_DPRX_RECEIVER_61_L + usRegOffsetReceiverByID, 0, BIT1);

	msWriteByteMask(REG_DPRX_RECEIVER_5D_L + usRegOffsetReceiverByID, bEnable? 0 : BIT1, BIT1); // [1] Main link loss lock IRQ

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetInterlaneSkewLoseInterrupt()
//  [Description]
//					mhal_DPRx_SetInterlaneSkewLoseInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetInterlaneSkewLoseInterrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_RECEIVER_61_L + usRegOffsetReceiverByID, BIT2, BIT2);
	mhal_DPRx_DELAY_NOP(5);
	msWriteByteMask(REG_DPRX_RECEIVER_61_L + usRegOffsetReceiverByID, 0, BIT2);

	msWriteByteMask(REG_DPRX_RECEIVER_5D_L + usRegOffsetReceiverByID, bEnable? 0 : BIT2, BIT2); // [2] Interlane skew done lose IRQ

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetInterlaneSkewDoneInterrupt()
//  [Description]
//					mhal_DPRx_SetInterlaneSkewDoneInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetInterlaneSkewDoneInterrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_RECEIVER_61_L + usRegOffsetReceiverByID, BIT3, BIT3);
	mhal_DPRx_DELAY_NOP(5);
	msWriteByteMask(REG_DPRX_RECEIVER_61_L + usRegOffsetReceiverByID, 0, BIT3);

	msWriteByteMask(REG_DPRX_RECEIVER_5D_L + usRegOffsetReceiverByID, bEnable? 0 : BIT3, BIT3); // [3] Interlane skew done IRQ

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetClkLsBigChangeInterrupt()
//  [Description]
//					mhal_DPRx_SetClkLsBigChangeInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetClkLsBigChangeInterrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_RECEIVER_61_H + usRegOffsetReceiverByID, BIT0, BIT0);
	mhal_DPRx_DELAY_NOP(5);
	msWriteByteMask(REG_DPRX_RECEIVER_61_H + usRegOffsetReceiverByID, 0, BIT0);

	msWriteByteMask(REG_DPRX_RECEIVER_5D_H + usRegOffsetReceiverByID, bEnable? 0 : BIT0, BIT0); // [8] Clk ls big change IRQ

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableFTInterrupt()
//  [Description]
//					mhal_DPRx_EnableFTInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableFTInterrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_RECEIVER_62_L + usRegOffsetReceiverByID, BIT6, BIT6);
	mhal_DPRx_DELAY_NOP(5);
    msWriteByteMask(REG_DPRX_RECEIVER_62_L + usRegOffsetReceiverByID, 0, BIT6);

    msWriteByteMask(REG_DPRX_RECEIVER_5E_L + usRegOffsetReceiverByID, bEnable? 0 : BIT6, BIT6); // [22] Link rate change IRQ in fast training

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetTrainingPattern1Interrupt()
//  [Description]
//					mhal_DPRx_SetTrainingPattern1Interrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetTrainingPattern1Interrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_DPCD1_31_L + usRegOffsetDPCD1ByID, BIT5, BIT5);
	mhal_DPRx_DELAY_NOP(5);
    msWriteByteMask(REG_DPRX_DPCD1_31_L + usRegOffsetDPCD1ByID, 0, BIT5);

    msWriteByteMask(REG_DPRX_DPCD1_2E_L + usRegOffsetDPCD1ByID, bEnable? 0 : BIT5, BIT5); // [5] TPS1 IRQ

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetTrainingPattern2Interrupt()
//  [Description]
//					mhal_DPRx_SetTrainingPattern2Interrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetTrainingPattern2Interrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_DPCD1_31_L + usRegOffsetDPCD1ByID, BIT4, BIT4);
	mhal_DPRx_DELAY_NOP(5);
    msWriteByteMask(REG_DPRX_DPCD1_31_L + usRegOffsetDPCD1ByID, 0, BIT4);

    msWriteByteMask(REG_DPRX_DPCD1_2E_L + usRegOffsetDPCD1ByID, bEnable? 0 : BIT4, BIT4); // [4] TPS2 IRQ

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetTrainingPattern3Interrupt()
//  [Description]
//					mhal_DPRx_SetTrainingPattern3Interrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetTrainingPattern3Interrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_DPCD1_31_L + usRegOffsetDPCD1ByID, BIT3, BIT3);
	mhal_DPRx_DELAY_NOP(5);
    msWriteByteMask(REG_DPRX_DPCD1_31_L + usRegOffsetDPCD1ByID, 0, BIT3);

    msWriteByteMask(REG_DPRX_DPCD1_2E_L + usRegOffsetDPCD1ByID, bEnable? 0 : BIT3, BIT3); // [3] TPS3 IRQ

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetTrainingPattern4Interrupt()
//  [Description]
//					mhal_DPRx_SetTrainingPattern4Interrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetTrainingPattern4Interrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_DPCD1_31_L + usRegOffsetDPCD1ByID, BIT2, BIT2);
	mhal_DPRx_DELAY_NOP(5);
    msWriteByteMask(REG_DPRX_DPCD1_31_L + usRegOffsetDPCD1ByID, 0, BIT2);

    msWriteByteMask(REG_DPRX_DPCD1_2E_L + usRegOffsetDPCD1ByID, bEnable? 0 : BIT2, BIT2); // [2] TPS4 IRQ

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableDPCDTrainingInterrupt()
//  [Description]
//					mhal_DPRx_EnableDPCDTrainingInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableDPCDTrainingInterrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

    msWriteByteMask(REG_DPRX_DPCD1_31_H + usRegOffsetDPCD1ByID, BIT6, BIT6);
    mhal_DPRx_DELAY_NOP(5);
    msWriteByteMask(REG_DPRX_DPCD1_31_H + usRegOffsetDPCD1ByID, 0, BIT6);

    msWriteByteMask(REG_DPRX_DPCD1_2E_H + usRegOffsetDPCD1ByID, bEnable? 0 : BIT6, BIT6); // [14] dpcd_training_irq

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableDPCD102Interrupt()
//  [Description]
//					mhal_DPRx_EnableDPCD102Interrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableDPCD102Interrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET000(dprx_id);

    msWriteByteMask(REG_DPRX_DPCD0_7D_L + usRegOffsetDPCD0ByID, BIT0, BIT0);
    mhal_DPRx_DELAY_NOP(5);
    msWriteByteMask(REG_DPRX_DPCD0_7D_L + usRegOffsetDPCD0ByID, 0, BIT0);

    msWriteByteMask(REG_DPRX_DPCD0_7B_L + usRegOffsetDPCD0ByID, bEnable? 0 : BIT0, BIT0);

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableDPCD103Interrupt()
//  [Description]
//					mhal_DPRx_EnableDPCD103Interrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableDPCD103Interrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_DPCD0_7D_L + usRegOffsetDPCD0ByID, BIT1, BIT1);
    mhal_DPRx_DELAY_NOP(5);
    msWriteByteMask(REG_DPRX_DPCD0_7D_L + usRegOffsetDPCD0ByID, 0, BIT1);

    msWriteByteMask(REG_DPRX_DPCD0_7B_L + usRegOffsetDPCD0ByID, bEnable? 0 : BIT1, BIT1); // [1] DPCD 103h IRQ

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableDPCD202Interrupt()
//  [Description]
//					mhal_DPRx_EnableDPCD202Interrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableDPCD202Interrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_DPCD0_7D_L + usRegOffsetDPCD0ByID, BIT5, BIT5);
    mhal_DPRx_DELAY_NOP(5);
    msWriteByteMask(REG_DPRX_DPCD0_7D_L + usRegOffsetDPCD0ByID, 0, BIT5);

    msWriteByteMask(REG_DPRX_DPCD0_7B_L + usRegOffsetDPCD0ByID, bEnable? 0 : BIT5, BIT5); // [5] DPCD 202h IRQ

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableDPCD270Interrupt()
//  [Description]
//					mhal_DPRx_EnableDPCD270Interrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableDPCD270Interrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_DPCD1_31_L + usRegOffsetDPCD1ByID, BIT6, BIT6);
	mhal_DPRx_DELAY_NOP(5);
    msWriteByteMask(REG_DPRX_DPCD1_31_L + usRegOffsetDPCD1ByID, 0, BIT6);

    msWriteByteMask(REG_DPRX_DPCD1_2E_L + usRegOffsetDPCD1ByID, bEnable? 0 : BIT6, BIT6); // [6] DPCD 270h write pulse IRQ

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetVPLLBigChangeInterrupt()
//  [Description]
//					mhal_DPRx_SetVPLLBigChangeInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetVPLLBigChangeInterrupt(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_DECODER_E0_0F_L + usRegOffsetDecoderByID, BIT1, BIT1);
    mhal_DPRx_DELAY_NOP(5);
    msWriteByteMask(REG_DPRX_DECODER_E0_0F_L + usRegOffsetDecoderByID, 0, BIT1);

    msWriteByteMask(REG_DPRX_DECODER_E0_0D_L + usRegOffsetDecoderByID, bEnable? 0 : BIT1, BIT1); // [1] Video MN big change IRQ

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetVHeightUnstableInterrupt()
//  [Description]
//					mhal_DPRx_SetVHeightUnstableInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetVHeightUnstableInterrupt(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_DECODER_E0_0F_H + usRegOffsetDecoderByID, BIT2, BIT2);
    mhal_DPRx_DELAY_NOP(5);
    msWriteByteMask(REG_DPRX_DECODER_E0_0F_H + usRegOffsetDecoderByID, 0, BIT2);

    msWriteByteMask(REG_DPRX_DECODER_E0_0D_H + usRegOffsetDecoderByID, bEnable? 0 : BIT2, BIT2); // [10] vheight_unstable | (~msa_ignore_en & votal_unstable) | (~decompression_en & hwidth_unstable) IRQ

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetSDCInterrupt()
//  [Description]
//					mhal_DPRx_SetSDCInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetSDCInterrupt(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_DECODER_E0_0F_L + usRegOffsetDecoderByID, BIT2, BIT2);
	mhal_DPRx_DELAY_NOP(5);
	msWriteByteMask(REG_DPRX_DECODER_E0_0F_L + usRegOffsetDecoderByID, 0, BIT2);

    msWriteByteMask(REG_DPRX_DECODER_E0_0D_L + usRegOffsetDecoderByID, bEnable? 0 : BIT2, BIT2); // [2] SDP ECC error IRQ

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetVideoBufferOverflowInterrupt()
//  [Description]
//					mhal_DPRx_SetVideoBufferOverflowInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetVideoBufferOverflowInterrupt(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_DECODER_E0_0F_L + usRegOffsetDecoderByID, BIT3, BIT3);
	mhal_DPRx_DELAY_NOP(5);
	msWriteByteMask(REG_DPRX_DECODER_E0_0F_L + usRegOffsetDecoderByID, 0, BIT3);

    msWriteByteMask(REG_DPRX_DECODER_E0_0D_L + usRegOffsetDecoderByID, bEnable? 0 : BIT3, BIT3); // [3] Video buffer overflow IRQ

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetMSAChgInterrupt()
//  [Description]
//					mhal_DPRx_SetMSAChgInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetMSAChgInterrupt(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_DECODER_E0_0F_L + usRegOffsetDecoderByID, BIT4, BIT4);
    mhal_DPRx_DELAY_NOP(5);
    msWriteByteMask(REG_DPRX_DECODER_E0_0F_L + usRegOffsetDecoderByID, 0, BIT4);

    msWriteByteMask(REG_DPRX_DECODER_E0_0D_L + usRegOffsetDecoderByID, bEnable? 0 : BIT4, BIT4); // [4] MSA change IRQ

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_MSAChgInterruptEvent_Enable()
//  [Description]
//					mhal_DPRx_MSAChgInterruptEvent_Enable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_MSAChgInterruptEvent_Enable(DPRx_DECODER_ID dprx_decoder_id, WORD usMSAChgInterruptEvent)
{
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

    if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
    {
        return;
    }

    msWrite2ByteMask(REG_DPRX_DECODER_E0_2B_L + usRegOffsetDecoderByID, ~usMSAChgInterruptEvent, BIT10|BIT9|BIT8|BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0); // MSA change mode mask

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetVideoMuteInterrupt()
//  [Description]
//					mhal_DPRx_SetVideoMuteInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetVideoMuteInterrupt(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET100(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_DECODER_E0_0F_L + usRegOffsetDecoderByID, BIT5, BIT5);
	mhal_DPRx_DELAY_NOP(5);
	msWriteByteMask(REG_DPRX_DECODER_E0_0F_L + usRegOffsetDecoderByID, 0, BIT5);

    msWriteByteMask(REG_DPRX_DECODER_E0_0D_L + usRegOffsetDecoderByID, bEnable? 0 : BIT5, BIT5); // [5] Video mute IRQ

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_DecoderIsrDetectEnable()
//  [Description]
//					mhal_DPRx_DecoderIsrDetectEnable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_DecoderIsrDetectEnable(DPRx_DECODER_ID dprx_decoder_id, Bool bEnable)
{
    if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
    {
        return;
    }

    if(bEnable)
    {
        //printf("Set decoder ISR enable !!\r\n");
    }
    else
    {
        //printf("Set decoder ISR disable !!\r\n");
    }

    mhal_DPRx_SetAUPLLBigChangeInterrupt(dprx_decoder_id, bEnable);
    mhal_DPRx_SetVPLLBigChangeInterrupt(dprx_decoder_id, bEnable);
    mhal_DPRx_SetMSAChgInterrupt(dprx_decoder_id, bEnable);
    mhal_DPRx_EnableVspHwRegen( dprx_decoder_id, bEnable);
    mhal_DPRx_EnableAutoInterlace(dprx_decoder_id, bEnable);
    mhal_DPRx_SetVideoMuteInterrupt(dprx_decoder_id, bEnable);
    mhal_DPRx_SetAudioChanneStatusChgInterrupt(dprx_decoder_id, bEnable);
    mhal_DPRx_SetAudioMuteInterrupt(dprx_decoder_id, bEnable);
    mhal_DPRx_SetMISC0ChangeInterrupt(dprx_decoder_id, bEnable);
    mhal_DPRx_SetMISC1ChangeInterrupt(dprx_decoder_id, bEnable);

    return;
}

void ________HPD_CONTROL________(void);
//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_HPDControl()
//  [Description]
//					mhal_DPRx_HPDControl
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_HPDControl(DPRx_AUX_ID dprx_aux_id, BOOL bSetHPD)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    if(bSetHPD == TRUE)
    {
		msWriteByteMask(REG_DPRX_AUX_56_H + usRegOffsetAuxByID, BIT4, BIT4|BIT6);
		msWriteByteMask(REG_DPRX_AUX_58_H + usRegOffsetAuxByID, BIT4, BIT4); // HPD value
    }
    else
    {
		msWriteByteMask(REG_DPRX_AUX_56_H + usRegOffsetAuxByID, 0, BIT4|BIT6);
		msWriteByteMask(REG_DPRX_AUX_58_H + usRegOffsetAuxByID, 0, BIT4); // HPD value
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_IsHwHPDControlEnable()
//  [Description]
//					mhal_DPRx_IsHwHPDControlEnable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_IsHwHPDControlEnable(DPRx_AUX_ID dprx_aux_id)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    if(((msReadByte(REG_DPRX_AUX_56_H + usRegOffsetAuxByID) & (BIT3|BIT7)) == (BIT3|BIT7)) && (((msReadByte(REG_DPRX_AUX_57_L + usRegOffsetAuxByID) & BIT3) == BIT3)))
    {
		return TRUE;
    }

    return  FALSE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetHwHPDControlEnable()
//  [Description]
//					mhal_DPRx_SetHwHPDControlEnable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetHwHPDControlEnable(DPRx_AUX_ID dprx_aux_id, BOOL bEnable)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

	switch(dprx_aux_id)
	{
		case DPRx_AUX_ID_0:
		    if((DPRx_C1_AUX == AUX_GPIO_31_32_33) || (DPRx_C2_AUX == AUX_GPIO_31_32_33) || (DPRx_C3_AUX == AUX_GPIO_31_32_33) || (DPRx_C4_AUX == AUX_GPIO_31_32_33))
            {
			    msWriteByteMask(REG_PAD_TOP_0B_H, bEnable, BIT0); // Enable DP RX0 HPD
			}
			break;

		case DPRx_AUX_ID_1:
		    if((DPRx_C1_AUX == AUX_GPIO_52_53_54) || (DPRx_C2_AUX == AUX_GPIO_52_53_54) || (DPRx_C3_AUX == AUX_GPIO_52_53_54) || (DPRx_C4_AUX == AUX_GPIO_52_53_54))
		    {
			    msWriteByteMask(REG_PAD_TOP_0B_H, (bEnable << 1), BIT1); // Enable DP RX1 HPD
			}
			break;

		case DPRx_AUX_ID_2:
		    if((DPRx_C1_AUX == AUX_GPIO_DPC_3_4_5) || (DPRx_C2_AUX == AUX_GPIO_DPC_3_4_5) || (DPRx_C3_AUX == AUX_GPIO_DPC_3_4_5) || (DPRx_C4_AUX == AUX_GPIO_DPC_3_4_5))
		    {
			    msWriteByteMask(REG_PAD_TOP_0B_H, (bEnable << 2), BIT2); // Enable DP RX2 HPD
			}
			break;

		default:
			break;
	}

    if(bEnable == TRUE)
    {
		msWriteByte(REG_DPRX_AUX_52_H + usRegOffsetAuxByID, 0x4B); // 0x44 is about 730us ~ 760us for short HPD

		msWriteByte(REG_DPRX_AUX_4F_L + usRegOffsetAuxByID, 0xEA); // 0xEA is about 650ms for long HPD

		msWriteByteMask(REG_DPRX_AUX_56_H + usRegOffsetAuxByID, BIT7, BIT7); // reg_aux_cmd_period_bypass
		msWriteByteMask(REG_DPRX_AUX_57_L + usRegOffsetAuxByID, BIT3, BIT3); // reg_hpd_hw_en
		msWriteByteMask(REG_DPRX_AUX_56_H + usRegOffsetAuxByID, BIT3, BIT3); // reg_hpd_i_hw_mode
	}
    else
    {
        msWriteByteMask(REG_DPRX_AUX_56_H + usRegOffsetAuxByID, 0, BIT7);
		msWriteByteMask(REG_DPRX_AUX_57_L + usRegOffsetAuxByID, 0, BIT3);
		msWriteByteMask(REG_DPRX_AUX_56_H + usRegOffsetAuxByID, 0, BIT3);
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetShortHPDIsrFlag()
//  [Description]
//					mhal_DPRx_GetShortHPDIsrFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetShortHPDIsrFlag(DPRx_AUX_ID dprx_aux_id)
{
    BOOL bEventFlag = 0;
    DWORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = ((msReadByte(REG_DPRX_AUX_7C_H + usRegOffsetAuxByID) & BIT4) ? TRUE : FALSE);

    if(bEventFlag)
    {
        msWriteByteMask(REG_DPRX_AUX_7C_H + usRegOffsetAuxByID, BIT7, BIT7);
    }

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_InternalOverWriteHPD()
//  [Description]
//					mhal_DPRx_InternalOverWriteHPD
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_InternalOverWriteHPD(DPRx_ID dprx_id, BOOL bEnable)
{
    WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

    msWriteByteMask(REG_DPRX_TRANS_CTRL_06_L + usRegOffsetTransCTRLByID, bEnable? BIT6 : 0, BIT6);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_HwShortHPDTrigger()
//  [Description]
//					mhal_DPRx_HwShortHPDTrigger
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_HwShortHPDTrigger(DPRx_AUX_ID dprx_aux_id)
{
	WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    msWriteByteMask(REG_DPRX_AUX_57_L + usRegOffsetAuxByID, BIT5, BIT5);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CheckHPDBusy()
//  [Description]
//					mhal_DPRx_CheckHPDBusy
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_CheckHPDBusy(DPRx_AUX_ID dprx_aux_id)
{
	WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    if(msReadByte(REG_DPRX_AUX_57_L + usRegOffsetAuxByID) & BIT7)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void ________AUX________(void);
//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_AuxClockEnable()
//  [Description]
//					mhal_DPRx_AuxClockEnable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_AuxClockEnable(DPRx_AUX_ID dprx_aux_id)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

    if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

	// AUX clk enable
	switch(dprx_aux_id)
	{
		case DPRx_AUX_ID_0:
		    if((DPRx_C1_AUX == AUX_GPIO_31_32_33) || (DPRx_C2_AUX == AUX_GPIO_31_32_33) || (DPRx_C3_AUX == AUX_GPIO_31_32_33) || (DPRx_C4_AUX == AUX_GPIO_31_32_33))
		    {
			    msWriteByteMask(REG_PM_48_H, BIT0, BIT0); // AUX clk
			    msWriteByteMask(REG_PM_48_H, BIT4, BIT4); // AUX MCU clk
                msWriteByteMask(REG_DPRX_AUX_DPCD_61_L + usRegOffsetAuxByID, 0, BIT2);
			}
			break;

		case DPRx_AUX_ID_1:
		    if((DPRx_C1_AUX == AUX_GPIO_52_53_54) || (DPRx_C2_AUX == AUX_GPIO_52_53_54) || (DPRx_C3_AUX == AUX_GPIO_52_53_54) || (DPRx_C4_AUX == AUX_GPIO_52_53_54))
		    {
    			msWriteByteMask(REG_PM_48_H, BIT1, BIT1); // AUX clk
    			msWriteByteMask(REG_PM_48_H, BIT5, BIT5); // AUX MCU clk
                msWriteByteMask(REG_DPRX_AUX_DPCD_61_L + usRegOffsetAuxByID, 0, BIT2);
			}
			break;

		case DPRx_AUX_ID_2:
		    if((DPRx_C1_AUX == AUX_GPIO_DPC_3_4_5) || (DPRx_C2_AUX == AUX_GPIO_DPC_3_4_5) || (DPRx_C3_AUX == AUX_GPIO_DPC_3_4_5) || (DPRx_C4_AUX == AUX_GPIO_DPC_3_4_5))
		    {
    			msWriteByteMask(REG_PM_48_H, BIT2, BIT2); // AUX clk
    			msWriteByteMask(REG_PM_48_H, BIT6, BIT6); // AUX MCU clk
                msWriteByteMask(REG_DPRX_AUX_DPCD_61_L + usRegOffsetAuxByID, 0, BIT2);
			}
			break;

	    default:
			break;
	}

	return;
}

//**************************************************************************
//  [Function Name]:
//					mhal_DPRx_SetGPIOforAUX()
//  [Description]
//					mhal_DPRx_SetGPIOforAUX
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetGPIOforAUX(DPRx_AUX_ID dprx_aux_id, BOOL bEnable)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    if(bEnable)
    {
        // Set GPIO for DP AUX used
        msWriteByteMask(REG_DPRX_AUX_DPCD_61_L + usRegOffsetAuxByID, 0, BIT2); // reg_en_gpio
    }
    else
    {
        // Set GPIO for general GPIO used
        msWriteByteMask(REG_DPRX_AUX_DPCD_61_L + usRegOffsetAuxByID, BIT2, BIT2); // reg_en_gpio
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetAuxPMClock()
//  [Description]
//					mhal_DPRx_GetAuxPMClock
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mhal_DPRx_GetAuxPMClock(void)
{
    return msReadByte(REG_PM_49_L) & (BIT3|BIT2);
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetAuxPMClock()
//  [Description]
//					mhal_DPRx_SetAuxPMClock
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetAuxPMClock(BOOL bIsXtalClk)
{
    if(bIsXtalClk == TRUE)
    {
        msWriteByteMask(REG_PM_49_L, BIT2, BIT3|BIT2); // Set PM Aux clock to Xtal
    }
    else
    {
        msWriteByteMask(REG_PM_49_L, 0, BIT3|BIT2); // Set PM Aux clock to FRO
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//					mhal_DPRx_GetAuxValid()
//  [Description]
//					mhal_DPRx_GetAuxValid
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetAuxValid(DPRx_AUX_ID dprx_aux_id)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    return (msReadByte(REG_DPRX_AUX_56_L + usRegOffsetAuxByID) & BIT4)? TRUE : FALSE;
}

//**************************************************************************
//  [Function Name]:
//					mhal_DPRx_SetAuxDelayReply()
//  [Description]
//					mhal_DPRx_SetAuxDelayReply
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetAuxDelayReply(DPRx_AUX_ID dprx_aux_id, WORD usDelayTime)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

	msWrite2Byte(REG_DPRX_AUX_7B_L + usRegOffsetAuxByID, usDelayTime);

    return;
}

//**************************************************************************
//  [Function Name]:
//					mhal_DPRx_GetAuxDelayReply()
//  [Description]
//					mhal_DPRx_GetAuxDelayReply
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
WORD mhal_DPRx_GetAuxDelayReply(DPRx_AUX_ID dprx_aux_id)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

    return msRead2Byte(REG_DPRX_AUX_7B_L + usRegOffsetAuxByID);
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetAuxIsel()
//  [Description]
//					mhal_DPRx_SetAuxIsel
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetAuxIsel(DPRx_AUX_ID dprx_aux_id)
{
	WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);
	WORD usEfuseAddress = (dprx_aux_id == DPRx_AUX_ID_0)? 0x13B : (dprx_aux_id == DPRx_AUX_ID_1)? 0x139 : (dprx_aux_id == DPRx_AUX_ID_2)? 0x138 : 0;

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

	// Project find trim data fail, so RD confirm to use 0x17 only
	msWrite2Byte(REG_DPRX_AUX_DPCD_0F_L + usRegOffsetAuxDPCDByID, 0x170);

	// Efuse setting
	if((msEread_GetDataFromEfuse(usEfuseAddress) & BIT7) == BIT7)
	{
		msWrite2ByteMask(REG_DPRX_AUX_DPCD_0F_L + usRegOffsetAuxDPCDByID, ((msEread_GetDataFromEfuse(usEfuseAddress) & (BIT0|BIT1|BIT2|BIT3|BIT4)) << 4), BIT4|BIT5|BIT6|BIT7|BIT8);
	}

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetOffLine()
//  [Description]
//					mhal_DPRx_SetOffLine
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetOffLine(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, BOOL bEnable)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
	WORD usTrainingDebounce = DP_RX_TRAINING_DEBOUNCE_TIME;

	if(dprx_id == DPRx_ID_MAX || dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    while(usTrainingDebounce != 0)
    {
        usTrainingDebounce--;

        if((mhal_DPRx_CheckAuxIdle(dprx_aux_id) == TRUE) &&
           (mhal_DPRx_CheckAuxPhyIdle(dprx_aux_id) == TRUE) &&
           (mhal_DPRx_TrainingBusy_Check(dprx_id) == FALSE))
        {
            break;
        }

    	mhal_DPRx_DELAY_1ms();
	}

    if(usTrainingDebounce == 0)
    {
        printf("\r\n [DP] Set offline during training!\r\n");
    }

	if(bEnable)
	{
		msWriteByteMask(REG_DPRX_AUX_61_L + usRegOffsetAuxByID, BIT1, BIT1);
	}
	else
	{
	    // When offline -> online switching, reset Aux async FIFO
        mhal_DPRx_ResetAsyncFIFO(dprx_id, dprx_aux_id);

		msWriteByteMask(REG_DPRX_AUX_61_L + usRegOffsetAuxByID, 0, BIT1);
	}

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_IsAuxAtOffLine()
//  [Description]
//					mhal_DPRx_IsAuxAtOffLine
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_IsAuxAtOffLine(DPRx_AUX_ID dprx_aux_id)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    if(msReadByte(REG_DPRX_AUX_61_L + usRegOffsetAuxByID) & BIT1)
    {
        return TRUE;
    }

    return FALSE;
}

//**************************************************************************
//  [Function Name]:
//					mhal_DPRx_CheckAuxPhyIdle()
//  [Description]
//					mhal_DPRx_CheckAuxPhyIdle
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_CheckAuxPhyIdle(DPRx_AUX_ID dprx_aux_id)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    if((mhal_DPRx_GetAuxValid(dprx_aux_id) == FALSE) &&
      ((msReadByte(REG_DPRX_AUX_74_H + usRegOffsetAuxByID) & (BIT3|BIT2|BIT1|BIT0)) == 0)) // AUX state -> idle
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//**************************************************************************
//  [Function Name]:
//					mhal_DPRx_CheckAuxIdle()
//  [Description]
//					mhal_DPRx_CheckAuxIdle
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_CheckAuxIdle(DPRx_AUX_ID dprx_aux_id)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    if((mhal_DPRx_GetAuxValid(dprx_aux_id) == FALSE) &&
      ((msReadByte(REG_DPRX_AUX_74_L + usRegOffsetAuxByID) & (BIT5|BIT4|BIT3)) == BIT3)) // AUX state -> idle
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_PMAux_Reset()
//  [Description]
//                  mhal_DPRx_PMAux_Reset
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_PMAux_Reset(DPRx_AUX_ID dprx_aux_id)
{
	WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET000(dprx_aux_id);
	BYTE ubTimeOutCounter = 100;

	while(ubTimeOutCounter > 0)
	{
		if((msReadByte(REG_DPRX_AUX_56_L  + usRegOffsetAuxByID) & BIT4) == 0x0) // Make sure Aux idle
		{
			msWriteByteMask(REG_DPRX_AUX_2E_L + usRegOffsetAuxByID, 0, BIT0);
			mhal_DPRx_DELAY_NOP(100);
			msWriteByteMask(REG_DPRX_AUX_2E_L + usRegOffsetAuxByID, BIT0, BIT0);

            msWriteByteMask(REG_DPRX_AUX_0F_L + usRegOffsetAuxByID, BIT1, BIT1); // Reset
            mhal_DPRx_DELAY_NOP(10);
            msWriteByteMask(REG_DPRX_AUX_0F_L + usRegOffsetAuxByID, 0, BIT1); // Reset

            if(mhal_DPRx_IsAuxAtOffLine(dprx_aux_id) == FALSE)
            {
                msWriteByteMask(REG_DPRX_DPCD0_1A_L + usRegOffsetDPCD0ByID, BIT5, BIT5); // Reset
                mhal_DPRx_DELAY_NOP(10);
                msWriteByteMask(REG_DPRX_DPCD0_1A_L + usRegOffsetDPCD0ByID, 0, BIT5); // Reset
            }
			break;
		}
		else
		{
			mhal_DPRx_DELAY_NOP(0x200);
		}

		ubTimeOutCounter--;
	}

	return TRUE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_AuxPause_Set()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_AuxPause_Set(DPRx_AUX_ID dprx_aux_id, Bool bEnable)
{
    WORD usRegOffsetAUXByID = DP_REG_OFFSET300(dprx_aux_id);

    if(bEnable == TRUE)
    {
        msWriteByteMask(REG_DPRX_AUX_54_H+usRegOffsetAUXByID, BIT5, BIT5); //PD R-TERM make aux not reply
    }
    else
    {
        msWriteByteMask(REG_DPRX_AUX_54_H+usRegOffsetAUXByID, 0, BIT5);
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CableDisconectResetDPCD()
//  [Description]
//                  mhal_DPRx_CableDisconectResetDPCD
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_CableDisconectResetDPCD(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id)
{
    BOOL usIsOffline = mhal_DPRx_IsAuxAtOffLine(dprx_aux_id);
	BYTE i;
    //reset pm bank
    mhal_DPRx_SetOffLine(dprx_id, dprx_aux_id, TRUE);
	mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00600, 0x1);
	mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00100, 0x6);
	mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00101, 0x1);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00102, 0x0);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00103, 0x0);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00104, 0x0);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00105, 0x0);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00106, 0x0);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00107, 0x0);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00108, 0x1);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00111, 0x0);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00115, 0x0);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00119, 0x0);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00120, 0x0);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00154, 0x0);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00155, 0x0);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00156, 0x0);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00157, 0x0);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00158, 0x0);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00159, 0x0);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_0015A, 0x0);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_0015B, 0x0);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00160, 0x0);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_001A0, 0x0);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_001A1, 0x0);

    for(i = 0; i< 0xD; i++)
	{
		mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, (DPCD_00300 + i), 0x0);
	}
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00310, 0x0);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00311, 0x0);
	for(i = 0; i<= 0xF; i++)
	{
		mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, (DPCD_00030 + i), 0x0);
	}

    //reset npm bank
    if(usIsOffline == FALSE)
    {
        mhal_DPRx_SetOffLine(dprx_id, dprx_aux_id, FALSE);
    	mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_00100, 0x6);
    	mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_00101, 0x1);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_00102, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_00103, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_00104, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_00105, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_00106, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_00108, 0x1);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_0010A, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_0010B, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_0010C, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_0010D, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_0010E, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_00112, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_00113, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_00114, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_00115, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_00118, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_00120, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_00160, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_03051, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_03052, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_03054, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_03055, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_03056, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_03057, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_03058, 0x0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_03059, 0x0);
    }

    mhal_DPRx_SetOffLine(dprx_id, dprx_aux_id, usIsOffline);

	return;
}

void ________DPCD________(void);
//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_MCUWritePMDPCD()
//  [Description]
//                  mhal_DPRx_MCUWritePMDPCD
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_MCUWritePMDPCD(DPRx_AUX_ID dprx_aux_id, DWORD ulDPCDAddress, BYTE ucDATA)
{
    WORD XDATA ubTimeOutLimt = 500;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

	if((msReadByte(REG_DPRX_AUX_54_H + usRegOffsetAuxByID) & BIT5) == BIT5)
	{
	    printf("[DPRXAUX%d] MCU Write PM FAIL because of R-term power down\r\n", dprx_aux_id);
	    return;
	}

    // Address overwrite enable
    msWriteByteMask(REG_DPRX_AUX_67_H + usRegOffsetAuxByID, BIT4, BIT4);

    // 20 bit DPCD address
    msWriteByte(REG_DPRX_AUX_66_L + usRegOffsetAuxByID, ulDPCDAddress & 0xFF);
    msWriteByte(REG_DPRX_AUX_66_H + usRegOffsetAuxByID, (ulDPCDAddress >> 0x8) & 0xFF);
    msWriteByte(REG_DPRX_AUX_67_L + usRegOffsetAuxByID, (ulDPCDAddress >> 0x10) & 0xF);

    msWriteByte(REG_DPRX_AUX_68_H + usRegOffsetAuxByID, ucDATA);

    while(((mhal_DPRx_CheckAuxPhyIdle(dprx_aux_id) == FALSE) || (mhal_DPRx_CheckAuxIdle(dprx_aux_id) == FALSE)) && (ubTimeOutLimt > 0))
    {
        mhal_DPRx_DELAY_NOP(25);
		ubTimeOutLimt--;
    }

    if(ubTimeOutLimt == 0)
    {
        printf("[DPRXAUX%d] MCU Write PM DPCD 0x%x FAIL aux not idle\r\n",dprx_aux_id,ulDPCDAddress);
    }

    msWriteByteMask(REG_DPRX_AUX_67_H + usRegOffsetAuxByID, BIT7, BIT7); // Trigger MUC write

    ubTimeOutLimt = 0x5;

    do
	{
		mhal_DPRx_DELAY_NOP(4);
		ubTimeOutLimt--;
    }while((msReadByte(REG_DPRX_AUX_67_H + usRegOffsetAuxByID) & BIT5) && (ubTimeOutLimt > 0));

	// Address overwrite disable
	msWriteByteMask(REG_DPRX_AUX_67_H + usRegOffsetAuxByID, 0, BIT4);
    mhal_DPRx_DELAY_NOP(100);
    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_MCUReadPMDPCD()
//  [Description]
//                  mhal_DPRx_MCUReadPMDPCD
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mhal_DPRx_MCUReadPMDPCD(DPRx_AUX_ID dprx_aux_id, DWORD ulDPCDAddress)
{
    WORD XDATA ubTimeOutLimt = 500;
    BYTE XDATA Data = 0;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return 0;
	}

	if((msReadByte(REG_DPRX_AUX_54_H + usRegOffsetAuxByID) & BIT5) == BIT5)
	{
	    printf("[DPRXAUX%d] MCU Read PM FAIL because of R-term power down\r\n", dprx_aux_id);
	    return 0;
	}

    // Address overwrite enable
    msWriteByteMask(REG_DPRX_AUX_67_H + usRegOffsetAuxByID, BIT4, BIT4);

    // 20 bit DPCD address
    msWriteByte(REG_DPRX_AUX_66_L + usRegOffsetAuxByID, ulDPCDAddress & 0xFF);
    msWriteByte(REG_DPRX_AUX_66_H + usRegOffsetAuxByID, (ulDPCDAddress >> 0x8) & 0xFF);
    msWriteByte(REG_DPRX_AUX_67_L + usRegOffsetAuxByID, (ulDPCDAddress >> 0x10) & 0xF);

    while(((mhal_DPRx_CheckAuxPhyIdle(dprx_aux_id) == FALSE) || (mhal_DPRx_CheckAuxIdle(dprx_aux_id) == FALSE)) && (ubTimeOutLimt > 0))
    {
        mhal_DPRx_DELAY_NOP(25);
		ubTimeOutLimt--;
    }

    if(ubTimeOutLimt == 0)
    {
        printf("[DPRXAUX%d] MCU READ PM DPCD 0x%x FAIL aux not idle\r\n",dprx_aux_id,ulDPCDAddress);
    }

    msWriteByteMask(REG_DPRX_AUX_67_H+ usRegOffsetAuxByID, BIT6, BIT6); // Trigger MUC read

    ubTimeOutLimt = 0x5;

    do
	{
		mhal_DPRx_DELAY_NOP(4);
		ubTimeOutLimt--;
    }while((msReadByte(REG_DPRX_AUX_67_H + usRegOffsetAuxByID) & BIT5) && (ubTimeOutLimt > 0));

    Data = msReadByte(REG_DPRX_AUX_68_L + usRegOffsetAuxByID);

	// Address overwrite disable
	msWriteByteMask(REG_DPRX_AUX_67_H + usRegOffsetAuxByID, 0, BIT4);
    mhal_DPRx_DELAY_NOP(100);
    return Data;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_MCUWriteNonPMDPCD()
//  [Description]
//                  mhal_DPRx_MCUWriteNonPMDPCD
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_MCUWriteNonPMDPCD(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, DWORD ulDPCDAddress, BYTE ucDATA)
{
    WORD XDATA ubTimeOutLimt = 500;
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET000(dprx_id);
	WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	if((msReadByte(REG_DPRX_AUX_54_H + usRegOffsetAuxByID) & BIT5) == BIT5)
	{
	    printf("[DPRXAUX%d] MCU Write NPM FAIL because of R-term power down\r\n", dprx_aux_id);
	    return;
	}

    // Address overwrite enable
    msWriteByteMask(REG_DPRX_DPCD0_76_H + usRegOffsetDPCD0ByID, BIT4, BIT4);

    // 20 bit DPCD address
    msWriteByte(REG_DPRX_DPCD0_75_L + usRegOffsetDPCD0ByID, ulDPCDAddress & 0xFF);
    msWriteByte(REG_DPRX_DPCD0_75_H + usRegOffsetDPCD0ByID, (ulDPCDAddress >> 0x8) & 0xFF);
    msWriteByte(REG_DPRX_DPCD0_76_L + usRegOffsetDPCD0ByID, (ulDPCDAddress >> 0x10) & 0xF);

    msWriteByte(REG_DPRX_DPCD0_77_H + usRegOffsetDPCD0ByID, ucDATA);

    while(((mhal_DPRx_CheckAuxPhyIdle(dprx_aux_id) == FALSE) || (mhal_DPRx_CheckAuxIdle(dprx_aux_id) == FALSE)) && (ubTimeOutLimt > 0))
    {
        mhal_DPRx_DELAY_NOP(25);
		ubTimeOutLimt--;
    }

    if(ubTimeOutLimt == 0)
    {
        printf("[DPRXAUX%d] MCU Write NPM DPCD 0x%x FAIL aux not idle\r\n",dprx_aux_id,ulDPCDAddress);
    }

    msWriteByteMask(REG_DPRX_DPCD0_76_H + usRegOffsetDPCD0ByID, BIT7, BIT7); // Trigger MUC write

    ubTimeOutLimt = 0x5;

    do
	{
		mhal_DPRx_DELAY_NOP(4);
		ubTimeOutLimt--;
    }while((msReadByte(REG_DPRX_DPCD0_76_H + usRegOffsetDPCD0ByID) & BIT5) && (ubTimeOutLimt > 0));

	// Address overwrite disable
	msWriteByteMask(REG_DPRX_DPCD0_76_H + usRegOffsetDPCD0ByID, 0, BIT4);
    mhal_DPRx_DELAY_NOP(100);
    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_MCUReadNonPMDPCD()
//  [Description]
//                  mhal_DPRx_MCUReadNonPMDPCD
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mhal_DPRx_MCUReadNonPMDPCD(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, DWORD ulDPCDAddress)
{
    WORD XDATA ubTimeOutLimt = 500;
    BYTE XDATA Data = 0;
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET000(dprx_id);
	WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return 0;
	}

	if((msReadByte(REG_DPRX_AUX_54_H + usRegOffsetAuxByID) & BIT5) == BIT5)
	{
	    printf("[DPRXAUX%d] MCU Read NPM FAIL because of R-term power down\r\n", dprx_aux_id);
	    return 0;
	}

    // Address overwrite enable
    msWriteByteMask(REG_DPRX_DPCD0_76_H + usRegOffsetDPCD0ByID, BIT4, BIT4);

    // 20 bit DPCD address
    msWriteByte(REG_DPRX_DPCD0_75_L + usRegOffsetDPCD0ByID, ulDPCDAddress & 0xFF);
    msWriteByte(REG_DPRX_DPCD0_75_H + usRegOffsetDPCD0ByID, (ulDPCDAddress >> 0x8) & 0xFF);
    msWriteByte(REG_DPRX_DPCD0_76_L + usRegOffsetDPCD0ByID, (ulDPCDAddress >> 0x10) & 0xF);

    while(((mhal_DPRx_CheckAuxPhyIdle(dprx_aux_id) == FALSE) || (mhal_DPRx_CheckAuxIdle(dprx_aux_id) == FALSE)) && (ubTimeOutLimt > 0))
    {
        mhal_DPRx_DELAY_NOP(25);
		ubTimeOutLimt--;
    }

    if(ubTimeOutLimt == 0)
    {
        printf("[DPRXAUX%d] MCU Read NPM DPCD 0x%x FAIL aux not idle\r\n",dprx_aux_id,ulDPCDAddress);
    }

    msWriteByteMask(REG_DPRX_DPCD0_76_H + usRegOffsetDPCD0ByID, BIT6, BIT6); // Trigger MUC read

    ubTimeOutLimt = 0x5;

    do
	{
        mhal_DPRx_DELAY_NOP(4);
        ubTimeOutLimt--;
    }while((msReadByte(REG_DPRX_DPCD0_76_H + usRegOffsetDPCD0ByID) & BIT5) && (ubTimeOutLimt > 0));

    Data = msReadByte(REG_DPRX_DPCD0_77_L + usRegOffsetDPCD0ByID);

	// Address overwrite disable
	msWriteByteMask(REG_DPRX_DPCD0_76_H + usRegOffsetDPCD0ByID, 0, BIT4);
    mhal_DPRx_DELAY_NOP(100);
    return Data;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetDPCDValueByRIU()
//  [Description]
//                  mhal_DPRx_GetDPCDValueByRIU
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mhal_DPRx_GetDPCDValueByRIU(DPRx_AUX_ID dprx_aux_id, DWORD ulDPCDAddress)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET000(dprx_aux_id);
    WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET000(dprx_aux_id);
	BYTE offset = 0x0;
	BYTE ubValue;
	BOOL bIsOffline = mhal_DPRx_IsAuxAtOffLine(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return 0;
	}

	switch(ulDPCDAddress)
	{
		case DPCD_00000:
		    return msReadByte(REG_DPRX_AUX_00_L + usRegOffsetAuxByID); // PM
            break;

		case DPCD_00001:
			return msReadByte(REG_DPRX_AUX_00_H + usRegOffsetAuxByID);
			break;

		case DPCD_00002:
		    ubValue = ((msReadByte(REG_DPRX_AUX_01_L + usRegOffsetAuxByID) & BIT7) +
            ((msReadByte(REG_DPRX_AUX_01_H + usRegOffsetAuxByID) & BIT1) << 5) +
			((msReadByte(REG_DPRX_AUX_01_L + usRegOffsetAuxByID) & BIT6) >> 1) +
			(msReadByte(REG_DPRX_AUX_01_L + usRegOffsetAuxByID) & (BIT4|BIT3|BIT2|BIT1|BIT0)));
			return ubValue;
			break;

		case DPCD_00004:
			ubValue = 0x0;
		    ubValue |= ((msReadByte(REG_DPRX_AUX_01_H + usRegOffsetAuxByID) & BIT3) >> 3); // BIT0
		    ubValue |= (msReadByte(REG_DPRX_AUX_03_H + usRegOffsetAuxByID) & (BIT3|BIT4)); // BIT3 ~ BIT4
		    ubValue |= ((msReadByte(REG_DPRX_AUX_03_H + usRegOffsetAuxByID) & (BIT0|BIT1|BIT2)) << 5); // BIT5 ~ BIT7
		    return ubValue;
			break;

		case DPCD_00005:
			return msReadByte(REG_DPRX_AUX_76_H + usRegOffsetAuxByID);
			break;

		case DPCD_0000A:
			ubValue = 0x0;
		    ubValue |= (msReadByte(REG_DPRX_AUX_05_L + usRegOffsetAuxByID) & (BIT1|BIT2));
			break;

		case DPCD_0000E:
			return msReadByte(REG_DPRX_AUX_02_H + usRegOffsetAuxByID);
			break;

		case DPCD_00100:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_70_L + usRegOffsetAuxByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_20_L + usRegOffsetDPCD0ByID); // NPM
		    }
	 		break;

	 	case DPCD_00101:
	 	    if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_70_H + usRegOffsetAuxByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_20_H + usRegOffsetDPCD0ByID); // NPM
		    }
	 		break;

		case DPCD_00111:
			return msReadByte(REG_DPRX_AUX_DPCD_52_L + usRegOffsetAuxByID);
			break;

        case DPCD_00202:
            if(bIsOffline == TRUE)
            {
                return msReadByte(REG_DPRX_AUX_59_L + usRegOffsetAuxByID);
            }
            else
            {
                return msReadByte(REG_DPRX_TRANS_CTRL_17_L + usRegOffsetTransCTRLByID);
            }
		    break;

        case DPCD_00203:
            if(bIsOffline == TRUE)
            {
                return msReadByte(REG_DPRX_AUX_59_H + usRegOffsetAuxByID);
            }
            else
            {
                return msReadByte(REG_DPRX_TRANS_CTRL_17_H + usRegOffsetTransCTRLByID);
            }
		    break;

		case DPCD_02201:
		case DPCD_02202:
		case DPCD_02203:
		case DPCD_02204:
		case DPCD_02205:
		case DPCD_02206:
		case DPCD_02207:
		case DPCD_02208:
		case DPCD_02209:
		case DPCD_0220A:
		case DPCD_0220B:
		case DPCD_0220C:
		case DPCD_0220D:
		case DPCD_0220E:
		case DPCD_0220F:
		case DPCD_02210:
		case DPCD_02211:
			offset = (ulDPCDAddress & 0xFFFFF) - DPCD_02201;
			return msReadByte(REG_DPRX_AUX_DPCD_71_L + usRegOffsetAuxByID + offset);
			break;

		default:
			break;
	}

	return 0;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetDPCDValueByRIU()
//  [Description]
//                  mhal_DPRx_SetDPCDValueByRIU
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetDPCDValueByRIU(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, DWORD ulDPCDAddress, BYTE ubValue)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
	BYTE offset = 0x0;

	if((dprx_id == DPRx_ID_MAX) || (dprx_aux_id == DPRx_AUX_ID_MAX))
	{
		return;
	}

	switch(ulDPCDAddress)
	{
		case DPCD_00000:
            msWriteByte(REG_DPRX_AUX_00_L + usRegOffsetAuxByID, ubValue);
            break;

		case DPCD_00001:
			msWriteByte(REG_DPRX_AUX_00_H + usRegOffsetAuxByID, ubValue);
			break;

		case DPCD_00002:
            msWriteByteMask(REG_DPRX_AUX_01_L + usRegOffsetAuxByID, (ubValue & BIT7), BIT7); // DPCD 00002[7]
			msWriteByteMask(REG_DPRX_AUX_01_H + usRegOffsetAuxByID, (ubValue & BIT6) >> 5, BIT1); // DPCD 00002[6]
			msWriteByteMask(REG_DPRX_AUX_01_L + usRegOffsetAuxByID, (ubValue & BIT5) << 1, BIT6); // DPCD 00002[5]
			msWriteByteMask(REG_DPRX_AUX_01_L + usRegOffsetAuxByID, (ubValue & (BIT4|BIT3|BIT2|BIT1|BIT0)), BIT4|BIT3|BIT2|BIT1|BIT0); // DPCD 00002[4:0]
			break;

        case DPCD_00003:
			msWriteByteMask(REG_DPRX_AUX_7E_H + usRegOffsetAuxByID, (ubValue & BIT7) >> 5, BIT2); // DPCD 00003[7]
			msWriteByteMask(REG_DPRX_AUX_01_H + usRegOffsetAuxByID, (ubValue & BIT6) >> 4, BIT2); // DPCD 00003[6]
			msWriteByteMask(REG_DPRX_AUX_01_H + usRegOffsetAuxByID, (ubValue & BIT0), BIT0); // DPCD 00003[0]
			break;

		case DPCD_00004:
			if(ubValue & BIT0)
			{
				msWriteByteMask(REG_DPRX_AUX_01_H + usRegOffsetAuxByID, BIT3, BIT3);
			}
			else
			{
				msWriteByteMask(REG_DPRX_AUX_01_H + usRegOffsetAuxByID, 0, BIT3);
			}

			msWriteByteMask(REG_DPRX_AUX_03_H + usRegOffsetAuxByID, ubValue & (BIT3|BIT4), BIT3|BIT4);
			msWriteByteMask(REG_DPRX_AUX_03_H + usRegOffsetAuxByID, (ubValue >> 5) & (BIT0|BIT1|BIT2), BIT0|BIT1|BIT2);
			break;

		case DPCD_00005:
			msWriteByte(REG_DPRX_AUX_76_H + usRegOffsetAuxByID, ubValue);
			break;

        case DPCD_00007:
			msWriteByteMask(REG_DPRX_AUX_02_L + usRegOffsetAuxByID, (ubValue & BIT7) >> 7, BIT0); // DPCD 00007[7] OUI Support
			msWriteByteMask(REG_DPRX_AUX_01_H + usRegOffsetAuxByID, (ubValue & BIT6), BIT6); // DPCD 00007[6] MSA_TIMING_PAR_IGNORED
			msWriteByteMask(REG_DPRX_AUX_77_H + usRegOffsetAuxByID, (ubValue & 0x0F), BIT0|BIT1|BIT2|BIT3); // DPCD 00007[3:0] DFP_COUNT
			break;

        case DPCD_00009:
			msWriteByte(REG_DPRX_AUX_04_H + usRegOffsetAuxByID, ubValue);
			break;

		case DPCD_0000A:
			msWriteByteMask(REG_DPRX_AUX_05_L + usRegOffsetAuxByID, ubValue & (BIT1|BIT2), BIT1|BIT2);
			break;

        case DPCD_0000E:
			msWriteByte(REG_DPRX_AUX_02_H + usRegOffsetAuxByID, ubValue);
			break;

		case DPCD_00111:
			// DPCD 111h is one special case located in PM domain
			mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, ulDPCDAddress, ubValue);
			break;

		case DPCD_02200:
			msWriteByte(REG_DPRX_AUX_DPCD_31_L + usRegOffsetAuxByID, ubValue);
			break;

		case DPCD_02201:
		case DPCD_02202:
		case DPCD_02203:
		case DPCD_02204:
		case DPCD_02205:
		case DPCD_02206:
		case DPCD_02207:
		case DPCD_02208:
		case DPCD_02209:
		case DPCD_0220A:
		case DPCD_0220B:
		case DPCD_0220C:
		case DPCD_0220D:
		case DPCD_0220E:
		case DPCD_0220F:
		case DPCD_02210:
		case DPCD_02211:
			offset = (ulDPCDAddress & 0xFFFFF) - DPCD_02201;
			msWriteByte(REG_DPRX_AUX_DPCD_71_L + usRegOffsetAuxByID + offset, ubValue);
			break;

		default:
			if(ulDPCDAddress < DPCD_00100)
			{
				mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, ulDPCDAddress, ubValue);
			}
			else
			{
				mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, ulDPCDAddress, ubValue);
			}
			break;
	}

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_OverWriteDPCD202_203()
//  [Description]
//					mhal_DPRx_OverWriteDPCD202_203
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_OverWriteDPCD202_203(DPRx_ID dprx_id, BOOL bEnable, WORD ucValue)
{
    WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	if(bEnable == TRUE)
	{
		msWrite2Byte(REG_DPRX_TRANS_CTRL_0D_L + usRegOffsetTransCTRLByID, ucValue);

		msWriteByteMask(REG_DPRX_TRANS_CTRL_0C_L + usRegOffsetTransCTRLByID, BIT0|BIT1, BIT0|BIT1);
	}
	else
	{
		msWriteByteMask(REG_DPRX_TRANS_CTRL_0C_L + usRegOffsetTransCTRLByID, 0, BIT0|BIT1);

		msWrite2Byte(REG_DPRX_TRANS_CTRL_0D_L + usRegOffsetTransCTRLByID, 0x0000);
	}

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetFakeTrainingDPCD20x()
//  [Description]
//                  mhal_DPRx_GetFakeTrainingDPCD20x
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mhal_DPRx_GetFakeTrainingDPCD20x(DPRx_AUX_ID dprx_aux_id, DWORD ulDPCDAddress)
{
	WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
	BYTE ubOffset = 0x0;

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return 0;
	}

	if((ulDPCDAddress >= DPCD_00202) && (ulDPCDAddress <= DPCD_00207))
	{
		ubOffset = ulDPCDAddress - DPCD_00202;

		return msReadByte(REG_DPRX_AUX_59_L + usRegOffsetAuxByID + ubOffset);
	}

	return 0;
}

void ________PROG_DPCD________(void);
//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_PutAuxDDCData()
//  [Description]
//					mhal_DPRx_PutAuxDDCData
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_PutAuxDDCData(DPRx_AUX_ID dprx_aux_id, BYTE ucDDCData)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
    WORD usCount = DP_AUX_TIMEOUTCNT;

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    msWriteByte(REG_DPRX_AUX_79_H + usRegOffsetAuxByID, ucDDCData); // [15:8] The write data to TX FIFO

    msWriteByteMask(REG_DPRX_AUX_79_L + usRegOffsetAuxByID, BIT0, BIT0);

    do
    {
        usCount --;
    } while((!(msReadByte(REG_DPRX_AUX_79_L + usRegOffsetAuxByID) & BIT2)) && (usCount > 0));

    msWriteByteMask(REG_DPRX_AUX_79_L + usRegOffsetAuxByID, BIT1, BIT1);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetAuxDDCData()
//  [Description]
//					mhal_DPRx_GetAuxDDCData
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mhal_DPRx_GetAuxDDCData(DPRx_AUX_ID dprx_aux_id)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
    BYTE ucDDCData = 0;

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return 0;
	}

    msWriteByteMask(REG_DPRX_AUX_78_L + usRegOffsetAuxByID, BIT0, BIT0);

    // Must fine tune
    while(!(msReadByte(REG_DPRX_AUX_78_L + usRegOffsetAuxByID) & BIT2)) // Wait
    {};

    msWriteByteMask(REG_DPRX_AUX_78_L + usRegOffsetAuxByID, BIT1, BIT1);

    ucDDCData = msReadByte(REG_DPRX_AUX_3A_H + usRegOffsetAuxByID); // [15:8] MCU read RX FIFO data

    return ucDDCData;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ProgramDPCDEnable()
//  [Description]
//					mhal_DPRx_ProgramDPCDEnable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_ProgramDPCDEnable(DPRx_AUX_ID dprx_aux_id, DPRx_PROGRAM_DPCD_TYPE dp_pDPCD_id, BOOL bEnable)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (dp_pDPCD_id == DPRx_PROGRAM_DPCD_MAX))
	{
		return;
	}

    msWriteByteMask(REG_DPRX_AUX_3E_H + usRegOffsetAuxByID, bEnable? (BIT2 << dp_pDPCD_id) : 0, (BIT2 << dp_pDPCD_id)); // Enable custom address

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetProgramDPCD()
//  [Description]
//					mhal_DPRx_SetProgramDPCD
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetProgramDPCD(BOOL bEnable, DPRx_AUX_ID dprx_aux_id, DPRx_PROGRAM_DPCD_TYPE dp_pDPCD_id, DWORD pDPCD_Addr, BOOL bAddrGroup)
{
    BYTE XDATA pDPCD_id = 0;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (dp_pDPCD_id == DPRx_PROGRAM_DPCD_MAX))
	{
		return;
	}

	if(bEnable == TRUE)
	{

	    if(dp_pDPCD_id < DPRx_PROGRAM_DPCD_4)
	    {
	        msWriteByteMask(REG_DPRX_AUX_3E_H + usRegOffsetAuxByID, (BIT2 << dp_pDPCD_id), (BIT2 << dp_pDPCD_id)); // Enable custom address
	        msWrite2Byte(REG_DPRX_AUX_40_L + DPRX_PROGRAM_DPCD_OFFSET*dp_pDPCD_id + usRegOffsetAuxByID, pDPCD_Addr & 0xFFFF); // Custom address
	        msWriteByteMask(REG_DPRX_AUX_41_L + DPRX_PROGRAM_DPCD_OFFSET*dp_pDPCD_id + usRegOffsetAuxByID, (pDPCD_Addr >> 16), 0x0F); // Custom address
	        msWriteByteMask(REG_DPRX_AUX_41_H + DPRX_PROGRAM_DPCD_OFFSET*dp_pDPCD_id + usRegOffsetAuxByID, bAddrGroup? BIT7 : 0, BIT7); // Enable address block
	    }
	    else if((dp_pDPCD_id >= DPRx_PROGRAM_DPCD_4) && (dp_pDPCD_id < DPRx_PROGRAM_DPCD_8))
	    {
	        pDPCD_id = dp_pDPCD_id - DPRx_PROGRAM_DPCD_4;
	        msWriteByteMask(REG_DPRX_AUX_DPCD_07_H + DPRX_PROGRAM_DPCD_OFFSET*pDPCD_id + usRegOffsetAuxByID, BIT5, BIT5); // Enable custom address
	        msWrite2Byte(REG_DPRX_AUX_DPCD_06_L + DPRX_PROGRAM_DPCD_OFFSET*pDPCD_id + usRegOffsetAuxByID, pDPCD_Addr & 0xFFFF); // Custom address
	        msWriteByteMask(REG_DPRX_AUX_DPCD_07_L + DPRX_PROGRAM_DPCD_OFFSET*pDPCD_id + usRegOffsetAuxByID, (pDPCD_Addr >> 16), 0x0F); // Custom address
	        msWriteByteMask(REG_DPRX_AUX_DPCD_07_H + DPRX_PROGRAM_DPCD_OFFSET*pDPCD_id + usRegOffsetAuxByID, bAddrGroup? BIT7 : 0, BIT7); // Enable address block
	    }
	    else if(dp_pDPCD_id >= DPRx_PROGRAM_DPCD_8)
	    {
	        pDPCD_id = dp_pDPCD_id - DPRx_PROGRAM_DPCD_8;
	        msWrite2Byte(REG_DPRX_AUX_DPCD_32_L + DPRX_PROGRAM_DPCD_OFFSET*pDPCD_id + usRegOffsetAuxByID, pDPCD_Addr & 0xFFFF); // Custom address
	        msWriteByteMask(REG_DPRX_AUX_DPCD_33_L + DPRX_PROGRAM_DPCD_OFFSET*pDPCD_id + usRegOffsetAuxByID, (pDPCD_Addr >> 16), 0x0F); // Custom address
	    }
	}
	else
	{
		if(dp_pDPCD_id < DPRx_PROGRAM_DPCD_4)
	    {
	        msWriteByteMask(REG_DPRX_AUX_3E_H + usRegOffsetAuxByID, 0, (BIT2 << dp_pDPCD_id)); // Enable custom address
	    }
	    else if((dp_pDPCD_id >= DPRx_PROGRAM_DPCD_4) && (dp_pDPCD_id < DPRx_PROGRAM_DPCD_8))
	    {
	        pDPCD_id = dp_pDPCD_id - DPRx_PROGRAM_DPCD_4;
	        msWriteByteMask(REG_DPRX_AUX_DPCD_07_H + DPRX_PROGRAM_DPCD_OFFSET*pDPCD_id + usRegOffsetAuxByID, 0, BIT5); // Enable custom address
	    }
	    else if(dp_pDPCD_id >= DPRx_PROGRAM_DPCD_8)
	    {
	        //No need
	    }
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetProgramDPCDInterrupt()
//  [Description]
//					mhal_DPRx_SetProgramDPCDInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetProgramDPCDInterrupt(DPRx_AUX_ID dprx_aux_id, DPRx_PROGRAM_DPCD_TYPE dp_pDPCD_id, BOOL bEnable)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (dp_pDPCD_id == DPRx_PROGRAM_DPCD_MAX))
	{
		return;
	}

    if(dp_pDPCD_id < DPRx_PROGRAM_DPCD_4)
    {
        msWriteByteMask(REG_DPRX_AUX_49_L + usRegOffsetAuxByID, (BIT0 << dp_pDPCD_id), (BIT0 << dp_pDPCD_id)); // REG_PM_AUX_48[8]: Write 1 to clear touch programmable DPCD address IRQ
        msWriteByteMask(REG_DPRX_AUX_49_L + usRegOffsetAuxByID, 0, (BIT0 << dp_pDPCD_id)); // REG_PM_AUX_48[8]: Write 1 to clear touch programmable DPCD address IRQ
        msWriteByteMask(REG_DPRX_AUX_48_H + usRegOffsetAuxByID, bEnable? 0 : (BIT4 << dp_pDPCD_id), (BIT4 << dp_pDPCD_id)); // REG_PM_AUX_48[12]: programmable DPCD mask
    }
    else if((dp_pDPCD_id >= DPRx_PROGRAM_DPCD_4) && (dp_pDPCD_id < DPRx_PROGRAM_DPCD_8))
    {
        dp_pDPCD_id = dp_pDPCD_id - DPRx_PROGRAM_DPCD_4;
        msWriteByteMask(REG_DPRX_AUX_DPCD_0E_H + usRegOffsetAuxByID, (BIT4 << dp_pDPCD_id), (BIT4 << dp_pDPCD_id)); // REG_DPRX_AUX_DPCD_0E[4]: Write 1 to clear touch programmable DPCD address IRQ
        msWriteByteMask(REG_DPRX_AUX_DPCD_0E_H + usRegOffsetAuxByID, 0, (BIT4 << dp_pDPCD_id)); // REG_DPRX_AUX_DPCD_0E[4]: Write 1 to clear touch programmable DPCD address IRQ
        msWriteByteMask(REG_DPRX_AUX_DPCD_0E_H + usRegOffsetAuxByID, bEnable? 0 : (BIT0 << dp_pDPCD_id), (BIT0 << dp_pDPCD_id)); // REG_DPRX_AUX_DPCD_0E[8]: programmable DPCD mask
    }
    else if(dp_pDPCD_id >= DPRx_PROGRAM_DPCD_8)
    {
        dp_pDPCD_id = dp_pDPCD_id - DPRx_PROGRAM_DPCD_8;
        msWriteByteMask(REG_DPRX_AUX_DPCD_3A_H + usRegOffsetAuxByID, (BIT4 << dp_pDPCD_id), (BIT4 << dp_pDPCD_id)); // REG_DPRX_AUX_DPCD_3A[4]: Write 1 to clear touch programmable DPCD address IRQ
        msWriteByteMask(REG_DPRX_AUX_DPCD_3A_H + usRegOffsetAuxByID, 0, (BIT4 << dp_pDPCD_id)); // REG_DPRX_AUX_DPCD_3A[4]: Write 1 to clear touch programmable DPCD address IRQ
        msWriteByteMask(REG_DPRX_AUX_DPCD_3A_H + usRegOffsetAuxByID, bEnable? 0: (BIT0 << dp_pDPCD_id), (BIT0 << dp_pDPCD_id)); // REG_DPRX_AUX_DPCD_3A[8]: programmable DPCD mask
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetXDATAProgramDPCD()
//  [Description]
//					mhal_DPRx_SetXDATAProgramDPCD
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
#if (XDATA_PROGRAMDPCD == 0x1) // TEST code port 1
void mhal_DPRx_SetXDATAProgramDPCD(BOOL bEnable, DPRx_AUX_ID dprx_aux_id, DPRx_XDATAPROGRAM_DPCD_TYPE dp_pDPCD_id, DWORD pDPCD_Addr, DPRx_Command_TYPE CommandType, BOOL bAddrGroup)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (dp_pDPCD_id == DPRx_XDATAPROGRAM_DPCD_MAX))
	{
		return;
	}

    if(dp_pDPCD_id == DPRx_XDATAPROGRAM_DPCD_0)
    {
        msWriteByteMask(REG_DPRX_AUX_DPCD_57_L + usRegOffsetAuxByID, (bEnable? BIT0 : 0), BIT0);
        msWriteByteMask(REG_DPRX_AUX_DPCD_57_L + usRegOffsetAuxByID, (bAddrGroup? BIT2 : 0), BIT2);
        msWriteByteMask(REG_DPRX_AUX_DPCD_57_L + usRegOffsetAuxByID, CommandType? BIT1 : 0, BIT1);
        msWrite2Byte(REG_DPRX_AUX_DPCD_58_L + usRegOffsetAuxByID, pDPCD_Addr & 0xFFFF); // Custom address
        msWriteByteMask(REG_DPRX_AUX_DPCD_59_L + usRegOffsetAuxByID, (pDPCD_Addr >> 16), 0x0F); // Custom address
        msWriteByteMask(REG_DPRX_AUX_7C_H + usRegOffsetAuxByID, 0, BIT2);

        //I2C 30 reply 0
        glubRx_XDataQueue[dprx_aux_id][0][16] = 0x0;

		msWrite2Byte(REG_DPRX_AUX_DPCD_54_L + usRegOffsetAuxByID, ((DWORD)(&glubRx_XDataQueue[dprx_aux_id][0][0]))&0xFFFC );   //reg_program_xdata_write_address_start_0
		msWrite2Byte(REG_DPRX_AUX_DPCD_55_L + usRegOffsetAuxByID, ((DWORD)(&glubRx_XDataQueue[dprx_aux_id][0][16]))&0xFFFC );   //reg_program_xdata_read_address_start_0
    }
    else if(dp_pDPCD_id == DPRx_XDATAPROGRAM_DPCD_1)
    {
        msWriteByteMask(REG_DPRX_AUX_DPCD_57_L + usRegOffsetAuxByID, (bEnable? BIT4 : 0), BIT4);
        msWriteByteMask(REG_DPRX_AUX_DPCD_57_L + usRegOffsetAuxByID, (bAddrGroup? BIT6 : 0), BIT6);
        msWriteByteMask(REG_DPRX_AUX_DPCD_57_L + usRegOffsetAuxByID, CommandType? BIT5 : 0, BIT5);
        msWrite2Byte(REG_DPRX_AUX_DPCD_7A_L + usRegOffsetAuxByID, pDPCD_Addr & 0xFFFF); // Custom address
        msWriteByteMask(REG_DPRX_AUX_DPCD_7B_L + usRegOffsetAuxByID, (pDPCD_Addr >> 16), 0x0F); // Custom address
        msWriteByteMask(REG_DPRX_AUX_49_H + usRegOffsetAuxByID, 0, BIT2);

		msWrite2Byte(REG_DPRX_AUX_DPCD_7C_L + usRegOffsetAuxByID, ((DWORD)(&glubRx_XDataQueue[dprx_aux_id][1][0]))&0xFFFC );   //reg_program_xdata_write_address_start_1
		msWrite2Byte(REG_DPRX_AUX_DPCD_2E_L + usRegOffsetAuxByID, ((DWORD)(&glubRx_XDataQueue[dprx_aux_id][1][16]))&0xFFFC );   //reg_program_xdata_read_address_start_1
    }

	msWriteByteMask(REG_DPRX_PM_TOP_00_L, 0, BIT7); //XDATA R2 BYPASS

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetXDATAProgramDPCDInterrupt()
//  [Description]
//					mhal_DPRx_SetXDATAProgramDPCDInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetXDATAProgramDPCDInterrupt(DPRx_AUX_ID dprx_aux_id, DPRx_XDATAPROGRAM_DPCD_TYPE dp_pDPCD_id, BOOL bEnable)
{
	WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (dp_pDPCD_id == DPRx_XDATAPROGRAM_DPCD_MAX))
	{
		return;
	}

    if(dp_pDPCD_id == DPRx_XDATAPROGRAM_DPCD_0)
    {
        msWriteByteMask(REG_DPRX_AUX_7C_H + usRegOffsetAuxByID, BIT3, BIT3);			  // REG_PM_AUX_7C[11]: Write 1 to clear touch XDATA programmable DPCD address IRQ
        msWriteByteMask(REG_DPRX_AUX_7C_H + usRegOffsetAuxByID, 0, BIT3);				  // REG_PM_AUX_7C[11]: Write 1 to clear touch XDATA programmable DPCD address IRQ
        msWriteByteMask(REG_DPRX_AUX_7C_H + usRegOffsetAuxByID, bEnable? 0 : BIT2, BIT2); // REG_PM_AUX_7C[10]: XDATA programmable DPCD mask
    }
    else if(dp_pDPCD_id == DPRx_XDATAPROGRAM_DPCD_1)
    {
        msWriteByteMask(REG_DPRX_AUX_4A_H + usRegOffsetAuxByID, BIT7, BIT7);			  // REG_PM_AUX_49[11]: Write 1 to clear touch XDATA programmable DPCD address IRQ
        msWriteByteMask(REG_DPRX_AUX_4A_H + usRegOffsetAuxByID, 0, BIT7);				  // REG_PM_AUX_49[11]: Write 1 to clear touch XDATA programmable DPCD address IRQ
        msWriteByteMask(REG_DPRX_AUX_4A_H + usRegOffsetAuxByID, bEnable? 0 : BIT3, BIT3); // REG_PM_AUX_49[10]: XDATA programmable DPCD mask
    }

    return;
}
#endif

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ProgramDPCD0Proc()
//  [Description]
//					mhal_DPRx_ProgramDPCD0Proc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_ProgramDPCD0Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue) // 2017 DP CTS
{
	BOOL bDPCDWrite = FALSE;
    BYTE uctemp = 0;
    BYTE ucAuxCommand = 0;
    BYTE ucAuxLength = 0;
    DWORD ulAddress = 0x0;
   	WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (ucDataQueue == NULL))
	{
		return FALSE;
	}

    ucAuxCommand = msReadByte(REG_DPRX_AUX_1C_L + usRegOffsetAuxByID) & 0x0F; // REG_PM_AUX_1C[3:0]: DP AUX CH RX command
    ucAuxLength = msReadByte(REG_DPRX_AUX_1D_L + usRegOffsetAuxByID); // REG_PM_AUX_1D[7:0]: DP AUX CH RX length

    ulAddress = msReadByte(REG_DPRX_AUX_1B_L + usRegOffsetAuxByID) & 0x0F; // reg_aux_rx_addr
    ulAddress = (ulAddress << 16) + msRead2Byte(REG_DPRX_AUX_1A_L + usRegOffsetAuxByID);
    ulAddress = ulAddress - DPRX_PROGRAM_DPCD0_ADDERSS;

    msWriteByteMask(REG_DPRX_AUX_18_L + usRegOffsetAuxByID, BIT2, BIT2); // EDID Reply by MCU
    mhal_DPRx_DELAY_NOP(20);

    if((ucAuxCommand == 0x09) || (ucAuxCommand == 0x05) || (ucAuxCommand == 0x01))
    {
        if(ucAuxLength <= DP_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++) // Can't write 16-byte data
            {
                 mhal_DPRx_PutAuxDDCData(dprx_aux_id, ucDataQueue[(ulAddress + uctemp)]);
            }
        }
    }
    else if((ucAuxCommand == 0x08) || (ucAuxCommand == 0x04) || (ucAuxCommand == 0x00))
    {
        if(ucAuxLength <= DP_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++)
            {
                ucDataQueue[uctemp] = mhal_DPRx_GetAuxDDCData(dprx_aux_id);
            }
        }

        bDPCDWrite = TRUE;
    }

    msWrite2Byte(REG_DPRX_AUX_7B_L + usRegOffsetAuxByID, 0);
	mhal_DPRx_DELAY_NOP(5);

	msWriteByteMask(REG_DPRX_AUX_1F_L + usRegOffsetAuxByID, 0x0, BIT0|BIT1);  //reg_aux_tx_cmd    0: Aux ACK, 1: NACK, 2: Defer
	msWriteByteMask(REG_DPRX_AUX_18_L + usRegOffsetAuxByID, 0, BIT2);
	msWrite2Byte(REG_DPRX_AUX_7B_L + usRegOffsetAuxByID, DP_AUX_DELAY_NORMAL); // 53us delay reply

    if((ucAuxCommand == 0x09) || (ucAuxCommand == 0x08))
    {
	    msWriteByteMask(REG_DPRX_AUX_3B_H + usRegOffsetAuxByID, BIT0, BIT0); // Enable MCU reply "Ready"
    }
    else
    {
        msWriteByteMask(REG_DPRX_AUX_3B_H + usRegOffsetAuxByID, 0, BIT0); // Disable MCU reply "Ready"
    }

    msWriteByteMask(REG_DPRX_AUX_49_L + usRegOffsetAuxByID, BIT0, BIT0); // Clear program DPCD 0 IRQ

    return bDPCDWrite;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ProgramDPCD1Proc()
//  [Description]
//					mhal_DPRx_ProgramDPCD1Proc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_ProgramDPCD1Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue, BYTE *ucAUXCMD, BYTE *ucAUXLEN) // 2017 DP CTS
{
	BOOL bDPCDWrite = FALSE;
    BYTE uctemp = 0;
    BYTE ucAuxCommand = 0;
    BYTE ucAuxLength = 0;
    DWORD ulAddress = 0x0;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (ucDataQueue == NULL) || (ucAUXCMD == NULL) || (ucAUXLEN == NULL))
	{
		return FALSE;
	}

    ucAuxCommand = msReadByte(REG_DPRX_AUX_1C_L + usRegOffsetAuxByID) & 0x0F; // REG_PM_AUX_1C[3:0]: DP AUX CH RX command
    ucAuxLength = msReadByte(REG_DPRX_AUX_1D_L + usRegOffsetAuxByID); // REG_PM_AUX_1D[7:0]: DP AUX CH RX length
    *ucAUXCMD = ucAuxCommand;
    *ucAUXLEN = ucAuxLength;

    ulAddress = msReadByte(REG_DPRX_AUX_1B_L + usRegOffsetAuxByID) & 0x0F; // reg_aux_rx_addr
    ulAddress = (ulAddress << 16) + msRead2Byte(REG_DPRX_AUX_1A_L + usRegOffsetAuxByID);
    ulAddress = ulAddress - DPRX_PROGRAM_DPCD1_ADDERSS;

    if((ucAuxCommand == 0x09) || (ucAuxCommand == 0x05) || (ucAuxCommand == 0x01))
    {
		if(ucAuxLength <= DP_AUX_COMMAND_MAX_LENGTH)
		{
		    for(uctemp = 0; uctemp < ucAuxLength; uctemp++) // Can't write 16-byte data
		    {
				mhal_DPRx_PutAuxDDCData(dprx_aux_id, ucDataQueue[(ulAddress + uctemp)]);
		    }
		}
    }
    else if((ucAuxCommand == 0x08) || (ucAuxCommand == 0x04) || (ucAuxCommand == 0x00))
    {
        if(ucAuxLength <= DP_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++)
            {
                ucDataQueue[uctemp] = mhal_DPRx_GetAuxDDCData(dprx_aux_id);
            }
        }

        bDPCDWrite = TRUE;
    }

    msWriteByteMask(REG_DPRX_AUX_1F_L + usRegOffsetAuxByID, 0x0, BIT0|BIT1);  //reg_aux_tx_cmd    0: Aux ACK, 1: NACK, 2: Defer
    msWriteByteMask(REG_DPRX_AUX_3B_H + usRegOffsetAuxByID, BIT0, BIT0); // Set MCU reply "Ready"
    msWriteByteMask(REG_DPRX_AUX_18_L + usRegOffsetAuxByID, 0, BIT1); 	 // AUX reply by HW
    msWriteByteMask(REG_DPRX_AUX_49_L + usRegOffsetAuxByID, BIT1, BIT1); // Clear program DPCD 1 IRQ

    return bDPCDWrite;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ProgramDPCD2Proc()
//  [Description]
//					mhal_DPRx_ProgramDPCD2Proc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_ProgramDPCD2Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue) // 2017 DP CTS
{
	BOOL bDPCDWrite = FALSE;
    BYTE uctemp = 0;
    BYTE ucAuxCommand = 0;
    BYTE ucAuxLength = 0;
    DWORD ulAddress = 0x0;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (ucDataQueue == NULL))
	{
		return FALSE;
	}

	ucAuxCommand = msReadByte(REG_DPRX_AUX_1C_L + usRegOffsetAuxByID) & 0x0F; // REG_PM_AUX_1C[3:0]: DP AUX CH RX command
	ucAuxLength = msReadByte(REG_DPRX_AUX_1D_L + usRegOffsetAuxByID); // REG_PM_AUX_1D[7:0]: DP AUX CH RX length

	ulAddress = msReadByte(REG_DPRX_AUX_1B_L + usRegOffsetAuxByID) & 0x0F; // reg_aux_rx_addr
	ulAddress = (ulAddress << 16) + msRead2Byte(REG_DPRX_AUX_1A_L + usRegOffsetAuxByID);
	ulAddress = ulAddress - DPRX_PROGRAM_DPCD2_ADDERSS;

	msWriteByteMask(REG_DPRX_AUX_18_L + usRegOffsetAuxByID, BIT1, BIT1);

    if(ucAuxCommand == 0x09)
    {
        if(ucAuxLength <= DP_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++) // Can't write 16-byte data
            {
               mhal_DPRx_PutAuxDDCData(dprx_aux_id, ucDataQueue[(ulAddress + uctemp)]);
            }
        }
    }
    else if((ucAuxCommand == 0x08) || (ucAuxCommand == 0x04))
    {
        if(ucAuxLength <= DP_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++)
            {
                ucDataQueue[uctemp] = mhal_DPRx_GetAuxDDCData(dprx_aux_id);
            }
        }

        bDPCDWrite = TRUE;
    }

	msWriteByteMask(REG_DPRX_AUX_1F_L + usRegOffsetAuxByID, 0x0, BIT0|BIT1);  //reg_aux_tx_cmd    0: Aux ACK, 1: NACK, 2: Defer
    msWriteByteMask(REG_DPRX_AUX_3B_H + usRegOffsetAuxByID, BIT0, BIT0); // Set MCU reply "Ready"
    msWriteByteMask(REG_DPRX_AUX_18_L + usRegOffsetAuxByID, 0, BIT1); 	 // AUX reply by HW
    msWriteByteMask(REG_DPRX_AUX_49_L + usRegOffsetAuxByID, BIT2, BIT2); // Clear program DPCD 2 IRQ

	return bDPCDWrite;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ProgramDPCD3Proc()
//  [Description]
//					mhal_DPRx_ProgramDPCD3Proc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_ProgramDPCD3Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue) // 2017 DP CTS
{
	BOOL bDPCDWrite = FALSE;
    BYTE uctemp = 0;
    BYTE ucAuxCommand = 0;
    BYTE ucAuxLength = 0;
    DWORD ulAddress = 0x0;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (ucDataQueue == NULL))
	{
		return FALSE;
	}

    ucAuxCommand = msReadByte(REG_DPRX_AUX_1C_L + usRegOffsetAuxByID) & 0x0F; // REG_PM_AUX_1C[3:0]: DP AUX CH RX command
    ucAuxLength = msReadByte(REG_DPRX_AUX_1D_L + usRegOffsetAuxByID); // REG_PM_AUX_1D[7:0]: DP AUX CH RX length

    ulAddress = msReadByte(REG_DPRX_AUX_1B_L + usRegOffsetAuxByID) & 0x0F;  // reg_aux_rx_addr
    ulAddress = (ulAddress << 16) + msRead2Byte(REG_DPRX_AUX_1A_L + usRegOffsetAuxByID);
    ulAddress = ulAddress - DPRX_PROGRAM_DPCD3_ADDERSS;

    msWriteByteMask(REG_DPRX_AUX_18_L + usRegOffsetAuxByID, BIT1, BIT1);

    if(ucAuxCommand == 0x09)
    {
        if(ucAuxLength <= DP_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++) // Can't write 16-byte data
            {
               mhal_DPRx_PutAuxDDCData(dprx_aux_id, ucDataQueue[(ulAddress + uctemp)]);
            }
        }
    }
    else if((ucAuxCommand == 0x08) || (ucAuxCommand == 0x04))
    {
        if(ucAuxLength <= DP_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++)
            {
                ucDataQueue[uctemp] = mhal_DPRx_GetAuxDDCData(dprx_aux_id);
            }
        }

        bDPCDWrite = TRUE;
    }

	msWriteByteMask(REG_DPRX_AUX_1F_L + usRegOffsetAuxByID, 0x0, BIT0|BIT1);  //reg_aux_tx_cmd    0: Aux ACK, 1: NACK, 2: Defer
    msWriteByteMask(REG_DPRX_AUX_3B_H + usRegOffsetAuxByID, BIT0, BIT0);  // Set MCU reply "Ready"
    msWriteByteMask(REG_DPRX_AUX_18_L + usRegOffsetAuxByID, 0, BIT1); // AUX reply by HW
    msWriteByteMask(REG_DPRX_AUX_49_L + usRegOffsetAuxByID, BIT3, BIT3); // Clear program DPCD 3 IRQ

    return bDPCDWrite;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ProgramDPCD4Proc()
//  [Description]
//					mhal_DPRx_ProgramDPCD4Proc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_ProgramDPCD4Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue) // 2017 DP CTS
{
	BOOL bDPCDWrite = FALSE;
    BYTE uctemp = 0;
    BYTE ucAuxCommand = 0;
    BYTE ucAuxLength = 0;
    DWORD ulAddress = 0x0;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
	WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (ucDataQueue == NULL))
	{
		return FALSE;
	}

    #if (DPRX_SUPPORT_VSCSDP == 0x1)
    ucDataQueue[0] |= BIT3;
    #endif

    #if (DP_VESA_ADAPTIVE_SYNC_SDP_SUPPORT == 0x1)
    ucDataQueue[0] |= BIT1;  //DPRX_SUPPORT_VSCSDP 0x2210[1]
    ucDataQueue[4] |= BIT0;  //DPRX_SUPPORT_VSCSDP 0x2214[0]
    #endif

    ucAuxCommand = msReadByte(REG_DPRX_AUX_1C_L + usRegOffsetAuxByID) & 0x0F; // REG_PM_AUX_1C[3:0]: DP AUX CH RX command
    ucAuxLength = msReadByte(REG_DPRX_AUX_1D_L + usRegOffsetAuxByID); // REG_PM_AUX_1D[7:0]: DP AUX CH RX length

    ulAddress = msReadByte(REG_DPRX_AUX_1B_L + usRegOffsetAuxByID) & 0x0F;  // reg_aux_rx_addr
    ulAddress = (ulAddress << 16) + msRead2Byte(REG_DPRX_AUX_1A_L + usRegOffsetAuxByID);
    ulAddress = ulAddress - DPRX_PROGRAM_DPCD4_ADDERSS;

    msWriteByteMask(REG_DPRX_AUX_18_L + usRegOffsetAuxByID, BIT1, BIT1);

    if(ucAuxCommand == 0x09)
    {
        if(ucAuxLength <= DP_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++) // Can't write 16-byte data
            {
               mhal_DPRx_PutAuxDDCData(dprx_aux_id, ucDataQueue[(ulAddress + uctemp)]);
            }
        }
    }
    else if((ucAuxCommand == 0x08) || (ucAuxCommand == 0x04))
    {
        if(ucAuxLength <= DP_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++)
            {
                ucDataQueue[uctemp] = mhal_DPRx_GetAuxDDCData(dprx_aux_id);
            }
        }

        bDPCDWrite = TRUE;
    }

	msWriteByteMask(REG_DPRX_AUX_1F_L + usRegOffsetAuxByID, 0x0, BIT0|BIT1);  //reg_aux_tx_cmd    0: Aux ACK, 1: NACK, 2: Defer
    msWriteByteMask(REG_DPRX_AUX_3B_H + usRegOffsetAuxByID, BIT0, BIT0);  // Set MCU reply "Ready"
    msWriteByteMask(REG_DPRX_AUX_18_L + usRegOffsetAuxByID, 0, BIT1); // AUX reply by HW
    msWriteByteMask(REG_DPRX_AUX_DPCD_0E_H + usRegOffsetAuxDPCDByID, BIT4, BIT4); // Clear program DPCD 4 IRQ

    return bDPCDWrite;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ProgramDPCD5Proc()
//  [Description]
//					mhal_DPRx_ProgramDPCD5Proc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_ProgramDPCD5Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue, BYTE *ucDataLength) // 2017 DP CTS
{
	BOOL bDPCDWrite = FALSE;
    BYTE uctemp = 0;
    BYTE ucAuxCommand = 0;
    BYTE ucAuxLength = 0;
    DWORD ulAddress = 0x0;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
	WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (ucDataQueue == NULL) || (ucDataLength == NULL))
	{
		return FALSE;
	}

    ucAuxCommand = msReadByte(REG_DPRX_AUX_1C_L + usRegOffsetAuxByID) & 0x0F; // REG_PM_AUX_1C[3:0]: DP AUX CH RX command
    ucAuxLength = msReadByte(REG_DPRX_AUX_1D_L + usRegOffsetAuxByID); // REG_PM_AUX_1D[7:0]: DP AUX CH RX length

    ucDataLength[0] = ucAuxLength; // Return data length

    ulAddress = msReadByte(REG_DPRX_AUX_1B_L + usRegOffsetAuxByID) & 0x0F;  // reg_aux_rx_addr
    ulAddress = (ulAddress << 16) + msRead2Byte(REG_DPRX_AUX_1A_L + usRegOffsetAuxByID);
    ulAddress = ulAddress - DPRX_PROGRAM_DPCD5_ADDERSS;

    msWriteByteMask(REG_DPRX_AUX_18_L + usRegOffsetAuxByID, BIT1, BIT1);

    if(ucAuxCommand == 0x09)
    {
        if(ucAuxLength <= DP_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++) // Can't write 16-byte data
            {
               mhal_DPRx_PutAuxDDCData(dprx_aux_id, ucDataQueue[(ulAddress + uctemp)]);
            }
        }
    }
    else if((ucAuxCommand == 0x08) || (ucAuxCommand == 0x04))
    {
        if(ucAuxLength <= DP_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++)
            {
                ucDataQueue[uctemp] = mhal_DPRx_GetAuxDDCData(dprx_aux_id);
            }
        }

        bDPCDWrite = TRUE;
    }

	msWriteByteMask(REG_DPRX_AUX_1F_L + usRegOffsetAuxByID, 0x0, BIT0|BIT1);  //reg_aux_tx_cmd    0: Aux ACK, 1: NACK, 2: Defer
    msWriteByteMask(REG_DPRX_AUX_3B_H + usRegOffsetAuxByID, BIT0, BIT0);  // Set MCU reply "Ready"
    msWriteByteMask(REG_DPRX_AUX_18_L + usRegOffsetAuxByID, 0, BIT1); // AUX reply by HW
    msWriteByteMask(REG_DPRX_AUX_DPCD_0E_H + usRegOffsetAuxDPCDByID, BIT5, BIT5); // Clear program DPCD 5 IRQ

    return bDPCDWrite;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ProgramDPCD6Proc()
//  [Description]
//					mhal_DPRx_ProgramDPCD6Proc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_ProgramDPCD6Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue) // 2017 DP CTS
{
	BOOL bDPCDWrite = FALSE;
    BYTE uctemp = 0;
    BYTE ucAuxCommand = 0;
    BYTE ucAuxLength = 0;
    DWORD ulAddress = 0x0;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
	WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (ucDataQueue == NULL))
	{
		return FALSE;
	}

    ucAuxCommand = msReadByte(REG_DPRX_AUX_1C_L + usRegOffsetAuxByID) & 0x0F; // REG_PM_AUX_1C[3:0]: DP AUX CH RX command
    ucAuxLength = msReadByte(REG_DPRX_AUX_1D_L + usRegOffsetAuxByID); // REG_PM_AUX_1D[7:0]: DP AUX CH RX length

    ulAddress = msReadByte(REG_DPRX_AUX_1B_L + usRegOffsetAuxByID) & 0x0F; // reg_aux_rx_addr
    ulAddress = (ulAddress << 16) + msRead2Byte(REG_DPRX_AUX_1A_L + usRegOffsetAuxByID);
    ulAddress = ulAddress - DPRX_PROGRAM_DPCD6_ADDERSS;

    msWriteByteMask(REG_DPRX_AUX_18_L + usRegOffsetAuxByID, BIT1, BIT1);

    if(ucAuxCommand == 0x09)
    {
        if(ucAuxLength <= DP_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++) // Can't write 16-byte data
            {
               mhal_DPRx_PutAuxDDCData(dprx_aux_id, ucDataQueue[(ulAddress + uctemp)]);
            }
        }
    }
    else if((ucAuxCommand == 0x08) || (ucAuxCommand == 0x04))
    {
        if(ucAuxLength <= DP_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++)
            {
                ucDataQueue[uctemp] = mhal_DPRx_GetAuxDDCData(dprx_aux_id);
            }
        }

        bDPCDWrite = TRUE;
    }

	msWriteByteMask(REG_DPRX_AUX_1F_L + usRegOffsetAuxByID, 0x0, BIT0|BIT1);  //reg_aux_tx_cmd    0: Aux ACK, 1: NACK, 2: Defer
    msWriteByteMask(REG_DPRX_AUX_3B_H + usRegOffsetAuxByID, BIT0, BIT0); // Set MCU reply "Ready"
    msWriteByteMask(REG_DPRX_AUX_18_L + usRegOffsetAuxByID, 0, BIT1); // AUX reply by HW
    msWriteByteMask(REG_DPRX_AUX_DPCD_0E_H + usRegOffsetAuxDPCDByID, BIT6, BIT6); // Clear program DPCD 6 IRQ

    return bDPCDWrite;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ProgramDPCD7Proc()
//  [Description]
//					mhal_DPRx_ProgramDPCD7Proc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_ProgramDPCD7Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue) // 2017 DP CTS
{
	BOOL bDPCDWrite = FALSE;
    BYTE uctemp = 0;
    BYTE ucAuxCommand = 0;
    BYTE ucAuxLength = 0;
    DWORD ulAddress = 0x0;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
	WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (ucDataQueue == NULL))
	{
		return FALSE;
	}

    ucAuxCommand = msReadByte(REG_DPRX_AUX_1C_L + usRegOffsetAuxByID) & 0x0F; // REG_PM_AUX_1C[3:0]: DP AUX CH RX command
    ucAuxLength = msReadByte(REG_DPRX_AUX_1D_L + usRegOffsetAuxByID); // REG_PM_AUX_1D[7:0]: DP AUX CH RX length

    ulAddress = msReadByte(REG_DPRX_AUX_1B_L + usRegOffsetAuxByID) & 0x0F; // reg_aux_rx_addr
    ulAddress = (ulAddress << 16) + msRead2Byte(REG_DPRX_AUX_1A_L + usRegOffsetAuxByID);
    ulAddress = ulAddress - DPRX_PROGRAM_DPCD7_ADDERSS;

    msWriteByteMask(REG_DPRX_AUX_18_L + usRegOffsetAuxByID, BIT1, BIT1);

    if(ucAuxCommand == 0x09)
    {
        if(ucAuxLength <= DP_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++) // Can't write 16-byte data
            {
               mhal_DPRx_PutAuxDDCData(dprx_aux_id, ucDataQueue[(ulAddress + uctemp)]);
            }
        }
    }
    else if((ucAuxCommand == 0x08) || (ucAuxCommand == 0x04))
    {
        if(ucAuxLength <= DP_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++)
            {
                ucDataQueue[uctemp] = mhal_DPRx_GetAuxDDCData(dprx_aux_id);
            }
        }

        bDPCDWrite = TRUE;
    }

	msWriteByteMask(REG_DPRX_AUX_1F_L + usRegOffsetAuxByID, 0x0, BIT0|BIT1);  //reg_aux_tx_cmd    0: Aux ACK, 1: NACK, 2: Defer
    msWriteByteMask(REG_DPRX_AUX_3B_H + usRegOffsetAuxByID, BIT0, BIT0); // Set MCU reply "Ready"
    msWriteByteMask(REG_DPRX_AUX_18_L + usRegOffsetAuxByID, 0, BIT1); // AUX reply by HW
    msWriteByteMask(REG_DPRX_AUX_DPCD_0E_H + usRegOffsetAuxDPCDByID, BIT7, BIT7); // clear program DPCD 7 IRQ

    return bDPCDWrite;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ProgramDPCD8Proc()
//  [Description]
//					mhal_DPRx_ProgramDPCD8Proc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_ProgramDPCD8Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue) // 2017 DP CTS
{
	WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);

    if((dprx_aux_id == DPRx_AUX_ID_MAX) || (ucDataQueue == NULL))
	{
		return;
	}

    msWriteByteMask(REG_DPRX_AUX_DPCD_3A_H + usRegOffsetAuxDPCDByID, BIT4, BIT4);
	mhal_DPRx_DELAY_NOP(0x30);
    msWriteByteMask(REG_DPRX_AUX_DPCD_3A_H + usRegOffsetAuxDPCDByID, 0, BIT4);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ProgramDPCD9Proc()
//  [Description]
//					mhal_DPRx_ProgramDPCD9Proc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_ProgramDPCD9Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue)
{
	WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (ucDataQueue == NULL))
	{
		return;
	}

	msWriteByteMask(REG_DPRX_AUX_DPCD_3A_H + usRegOffsetAuxDPCDByID, BIT5, BIT5);
	msWriteByteMask(REG_DPRX_AUX_DPCD_3A_H + usRegOffsetAuxDPCDByID, 0, BIT5);

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ProgramDPCD10Proc()
//  [Description]
//					mhal_DPRx_ProgramDPCD10Proc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_ProgramDPCD10Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue)
{
	WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (ucDataQueue == NULL))
	{
		return;
	}

	msWriteByteMask(REG_DPRX_AUX_DPCD_3A_H + usRegOffsetAuxDPCDByID, BIT6, BIT6);
	msWriteByteMask(REG_DPRX_AUX_DPCD_3A_H + usRegOffsetAuxDPCDByID, 0, BIT6);

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ProgramDPCD11Proc()
//  [Description]
//					mhal_DPRx_ProgramDPCD11Proc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_ProgramDPCD11Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue)
{
	WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (ucDataQueue == NULL))
	{
		return;
	}

	msWriteByteMask(REG_DPRX_AUX_DPCD_3A_H + usRegOffsetAuxDPCDByID, BIT7, BIT7);
	msWriteByteMask(REG_DPRX_AUX_DPCD_3A_H + usRegOffsetAuxDPCDByID, 0, BIT7);

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_XDATAProgramDPCD0Proc()
//  [Description]
//					mhal_DPRx_XDATAProgramDPCD0Proc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_XDATAProgramDPCD0Proc(DPRx_AUX_ID dprx_aux_id)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
	WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);
    WORD usTimeout = 500;

    while(((msReadByte(REG_DPRX_AUX_04_L + usRegOffsetAuxByID) & BIT0) != 0) && (usTimeout > 0))
    {
        mhal_DPRx_DELAY_NOP(25);
        usTimeout--;
    };

	msWriteByteMask(REG_DPRX_AUX_DPCD_57_L + usRegOffsetAuxDPCDByID, BIT3, BIT3); // Reset program DPCD 0 XDATA
    msWriteByteMask(REG_DPRX_AUX_7C_H + usRegOffsetAuxByID, BIT3, BIT3);          // Clear XDATA program DPCD 0 IRQ

    return TRUE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_XDATAProgramDPCD1Proc()
//  [Description]
//					mhal_DPRx_XDATAProgramDPCD1Proc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_XDATAProgramDPCD1Proc(DPRx_AUX_ID dprx_aux_id)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
	WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);
    WORD usTimeout = 500;

    while(((msReadByte(REG_DPRX_AUX_04_L+usRegOffsetAuxByID) & BIT0) != 0) && (usTimeout > 0))
    {
        mhal_DPRx_DELAY_NOP(25);
        usTimeout--;
    };

	msWriteByteMask(REG_DPRX_AUX_DPCD_57_L + usRegOffsetAuxDPCDByID, BIT7, BIT7); // Reset program DPCD 1 XDATA
    msWriteByteMask(REG_DPRX_AUX_4A_H + usRegOffsetAuxByID, BIT7, BIT7);          // Clear XDATA program DPCD 1 IRQ

	return TRUE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetProgramDPCD0Flag()
//  [Description]
//					mhal_DPRx_GetProgramDPCD0Flag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetProgramDPCD0Flag(DPRx_AUX_ID dprx_aux_id)
{
	BOOL bEventFlag = 0;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_48_L + usRegOffsetAuxByID) & BIT0)? TRUE : FALSE; // REG_PM_AUX_48[0]: programmable DPCD 0 IRQ

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetProgramDPCD1Flag()
//  [Description]
//					mhal_DPRx_GetProgramDPCD1Flag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetProgramDPCD1Flag(DPRx_AUX_ID dprx_aux_id)
{
	BOOL bEventFlag = 0;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_48_L + usRegOffsetAuxByID) & BIT1)? TRUE : FALSE; // REG_PM_AUX_48[1]: programmable DPCD 1 IRQ

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetProgramDPCD2Flag()
//  [Description]
//					mhal_DPRx_GetProgramDPCD2Flag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetProgramDPCD2Flag(DPRx_AUX_ID dprx_aux_id)
{
	BOOL bEventFlag = 0;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_48_L + usRegOffsetAuxByID) & BIT2)? TRUE : FALSE; // REG_PM_AUX_48[2]: programmable DPCD 2 IRQ

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetProgramDPCD3Flag()
//  [Description]
//					mhal_DPRx_GetProgramDPCD3Flag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetProgramDPCD3Flag(DPRx_AUX_ID dprx_aux_id)
{
	BOOL bEventFlag = 0;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_48_L + usRegOffsetAuxByID) & BIT3)? TRUE : FALSE; // REG_PM_AUX_48[3]: programmable DPCD 3 IRQ

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetProgramDPCD4Flag()
//  [Description]
//					mhal_DPRx_GetProgramDPCD4Flag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetProgramDPCD4Flag(DPRx_AUX_ID dprx_aux_id)
{
	BOOL bEventFlag = 0;
    WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_DPCD_0E_L + usRegOffsetAuxDPCDByID) & BIT0)? TRUE : FALSE; // REG_PM_AUX_DPCD_0E[0]: programmable DPCD 4 IRQ

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetProgramDPCD5Flag()
//  [Description]
//					mhal_DPRx_GetProgramDPCD5Flag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetProgramDPCD5Flag(DPRx_AUX_ID dprx_aux_id)
{
	BOOL bEventFlag = 0;
    WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_DPCD_0E_L + usRegOffsetAuxDPCDByID) & BIT1)? TRUE : FALSE; // REG_PM_AUX_DPCD_0E[1]: programmable DPCD 5 IRQ

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetProgramDPCD6Flag()
//  [Description]
//					mhal_DPRx_GetProgramDPCD6Flag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetProgramDPCD6Flag(DPRx_AUX_ID dprx_aux_id)
{
	BOOL bEventFlag = 0;
    WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_DPCD_0E_L + usRegOffsetAuxDPCDByID) & BIT2)? TRUE : FALSE; // REG_PM_AUX_DPCD_0E[2]: programmable DPCD 6 IRQ

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetProgramDPCD7Flag()
//  [Description]
//					mhal_DPRx_GetProgramDPCD7Flag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetProgramDPCD7Flag(DPRx_AUX_ID dprx_aux_id)
{
	BOOL bEventFlag = 0;
    WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_DPCD_0E_L + usRegOffsetAuxDPCDByID) & BIT3)? TRUE : FALSE; // REG_PM_AUX_DPCD_0E[3]: programmable DPCD 7 IRQ

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetProgramDPCD8Flag()
//  [Description]
//					mhal_DPRx_GetProgramDPCD8Flag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetProgramDPCD8Flag(DPRx_AUX_ID dprx_aux_id)
{
	BOOL bEventFlag = 0;
    WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_DPCD_3A_L + usRegOffsetAuxDPCDByID) & BIT0)? TRUE : FALSE; // REG_PM_AUX_DPCD_3A[0]: programmable DPCD 8 IRQ

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetProgramDPCD9Flag()
//  [Description]
//					mhal_DPRx_GetProgramDPCD9Flag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetProgramDPCD9Flag(DPRx_AUX_ID dprx_aux_id)
{
	BOOL bEventFlag = 0;
    WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_DPCD_3A_L + usRegOffsetAuxDPCDByID) & BIT1)? TRUE : FALSE; // REG_PM_AUX_DPCD_3A[1]: programmable DPCD 9 IRQ

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetProgramDPCD10Flag()
//  [Description]
//					mhal_DPRx_GetProgramDPCD10Flag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetProgramDPCD10Flag(DPRx_AUX_ID dprx_aux_id)
{
	BOOL bEventFlag = 0;
    WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_DPCD_3A_L + usRegOffsetAuxDPCDByID) & BIT2)? TRUE: FALSE; // REG_PM_AUX_DPCD_3A[2]: programmable DPCD 10 IRQ

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetProgramDPCD11Flag()
//  [Description]
//					mhal_DPRx_GetProgramDPCD11Flag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetProgramDPCD11Flag(DPRx_AUX_ID dprx_aux_id)
{
	BOOL bEventFlag = 0;
    WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_DPCD_3A_L + usRegOffsetAuxDPCDByID) & BIT3)? TRUE : FALSE; // REG_PM_AUX_DPCD_3A[3]: programmable DPCD 11 IRQ

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetXDATAProgramDPCD0Flag()
//  [Description]
//					mhal_DPRx_GetXDATAProgramDPCD0Flag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetXDATAProgramDPCD0Flag(DPRx_AUX_ID dprx_aux_id)
{
	BOOL bEventFlag = 0;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_7C_H + usRegOffsetAuxByID) & BIT0)? TRUE : FALSE; // REG_PM_AUX_7C[8]: XDATA programmable DPCD 0 IRQ

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetXDATAProgramDPCD1Flag()
//  [Description]
//					mhal_DPRx_GetXDATAProgramDPCD1Flag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetXDATAProgramDPCD1Flag(DPRx_AUX_ID dprx_aux_id)
{
	BOOL bEventFlag = 0;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_4A_L + usRegOffsetAuxByID) & BIT3)? TRUE : FALSE; // REG_PM_AUX_49[8]: XDATA programmable DPCD 1 IRQ

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetProgrammableDPCDEnable()
//  [Description]
//					mhal_DPRx_SetProgrammableDPCDEnable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_SetProgrammableDPCDEnable(DPRx_AUX_ID dprx_aux_id, BOOL bEnable)
{
	// Program DPCD
	#if (DPRX_PROGRAM_DPCD0_ENABLE == 0x1)
    if(ubInfo2 < 2)
    {
        mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_0, DPRX_PROGRAM_DPCD0_ADDERSS, FALSE);
        mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_0, bEnable);
    }
    else
    {
        mhal_DPRx_SetProgramDPCD(FALSE, dprx_aux_id, DPRx_PROGRAM_DPCD_0, DPRX_PROGRAM_DPCD0_ADDERSS, FALSE);
        mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_0, FALSE);
    }
	#endif

	#if(DPRX_PROGRAM_DPCD1_ENABLE == 0x1)
	mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_1, DPRX_PROGRAM_DPCD1_ADDERSS, TRUE);
	mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_1, bEnable);
	#endif

	#if(DPRX_PROGRAM_DPCD2_ENABLE == 0x1)
	mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_2, DPRX_PROGRAM_DPCD2_ADDERSS, FALSE);
	mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_2, bEnable);
	#endif

	#if(DPRX_PROGRAM_DPCD3_ENABLE == 0x1)
	mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_3, DPRX_PROGRAM_DPCD3_ADDERSS, FALSE);
	mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_3, bEnable);
	#endif

	#if(DPRX_PROGRAM_DPCD4_ENABLE == 0x1)&&(DP_VESA_ADAPTIVE_SYNC_SDP_SUPPORT == 0x0)
	mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_4, DPRX_PROGRAM_DPCD4_ADDERSS, FALSE);
	mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_4, bEnable);
	#endif

	#if(DPRX_PROGRAM_DPCD5_ENABLE == 0x1)
	mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_5, DPRX_PROGRAM_DPCD5_ADDERSS, FALSE);
	mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_5, bEnable);
	#endif

	#if(DPRX_PROGRAM_DPCD6_ENABLE == 0x1)
	mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_6, DPRX_PROGRAM_DPCD6_ADDERSS, FALSE);
	mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_6, bEnable);
	#endif

	#if(DPRX_PROGRAM_DPCD7_ENABLE == 0x1)
	mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_7, DPRX_PROGRAM_DPCD7_ADDERSS, FALSE);
	mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_7, bEnable);
	#endif

	#if(DPRX_PROGRAM_DPCD8_ENABLE == 0x1)
	mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_8, DPRX_PROGRAM_DPCD8_ADDERSS, FALSE);
	mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_8, bEnable);
	#endif

	#if(DPRX_PROGRAM_DPCD9_ENABLE == 0x1)
	mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_9, DPRX_PROGRAM_DPCD9_ADDERSS, FALSE);
	mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_9, bEnable);
	#endif

	#if(DPRX_PROGRAM_DPCD10_ENABLE == 0x1)
	mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_10, DPRX_PROGRAM_DPCD10_ADDERSS, FALSE);
	mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_10, bEnable);
	#endif

	#if(DPRX_PROGRAM_DPCD11_ENABLE == 0x1)
	mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_11, DPRX_PROGRAM_DPCD11_ADDERSS, FALSE);
	mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_11, bEnable);
	#endif

	// XDATA program DPCD
	#if (DPRX_XDATA_PROGRAM_DPCD0_ENABLE == 0x1)
    if(ubInfo2 < 2)
    {
        //offline turn on xdata programable dpcd
        if(bEnable == TRUE)
        {
            mhal_DPRx_SetXDATAProgramDPCD(FALSE, dprx_aux_id, DPRx_XDATAPROGRAM_DPCD_0, DPRX_XDATA_PROGRAM_DPCD0_ADDERSS, DPRx_I2C, FALSE);
            mhal_DPRx_SetXDATAProgramDPCDInterrupt(dprx_aux_id, DPRx_XDATAPROGRAM_DPCD_0, FALSE);
        }
        else
        {
            mhal_DPRx_SetXDATAProgramDPCD(TRUE, dprx_aux_id, DPRx_XDATAPROGRAM_DPCD_0, DPRX_XDATA_PROGRAM_DPCD0_ADDERSS, DPRx_I2C, FALSE);
            mhal_DPRx_SetXDATAProgramDPCDInterrupt(dprx_aux_id, DPRx_XDATAPROGRAM_DPCD_0, TRUE);
        }
    }
    else
    {
        mhal_DPRx_SetXDATAProgramDPCD(FALSE, dprx_aux_id, DPRx_XDATAPROGRAM_DPCD_0, DPRX_XDATA_PROGRAM_DPCD0_ADDERSS, DPRx_I2C, FALSE);
        mhal_DPRx_SetXDATAProgramDPCDInterrupt(dprx_aux_id, DPRx_XDATAPROGRAM_DPCD_0, FALSE);
    }
    #endif

	#if (DPRX_XDATA_PROGRAM_DPCD1_ENABLE == 0x1)
	mhal_DPRx_SetXDATAProgramDPCD(FALSE, dprx_aux_id, DPRx_XDATAPROGRAM_DPCD_1, DPRX_XDATA_PROGRAM_DPCD1_ADDERSS, DPRx_I2C, FALSE);
    mhal_DPRx_SetXDATAProgramDPCDInterrupt(dprx_aux_id, DPRx_XDATAPROGRAM_DPCD_1, FALSE);
    #endif

	return TRUE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableMCCSXDATAProgrammableDPCD()
//  [Description]
//					mhal_DPRx_EnableMCCSXDATAProgrammableDPCD
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableMCCSXDATAProgrammableDPCD(DPRx_AUX_ID dprx_aux_id, BOOL bEnable) // enable/disable xdata programable dpcd0 for mccs
{
    mhal_DPRx_SetXDATAProgramDPCD(bEnable, dprx_aux_id, DPRx_XDATAPROGRAM_DPCD_1, AUX_MCCS_SLAVE_ADDR, DPRx_I2C, FALSE);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CheckMCCSXDATAProgrammableDPCD()
//  [Description]
//					mhal_DPRx_CheckMCCSXDATAProgrammableDPCD
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_CheckMCCSXDATAProgrammableDPCD(DPRx_AUX_ID dprx_aux_id) // check to see if xdata programmable dpcd received mccs command
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
    WORD usRegOffsetAuxDPCDByID = DP_REG_OFFSET300(dprx_aux_id);
    WORD ubTimeout = 500;
    BYTE ubXdataIdx = 0;
    BYTE MCCS_ON[DPRX_AUX_COMMAND_MAX_LENGTH] = {0x51, 0x84, 0x03, 0xD6, 0x00, 0x01, 0x6F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    BYTE MCCS_OFF[DPRX_AUX_COMMAND_MAX_LENGTH] = {0x51, 0x84, 0x03, 0xD6, 0x00, 0x05, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    BYTE MCCS_STANDBY[DPRX_AUX_COMMAND_MAX_LENGTH] = {0x51, 0x84, 0x03, 0xD6, 0x00, 0x04, 0x6A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    BYTE u8AUX_Cmd, u8AUX_Length;

    if((msReadByte(REG_DPRX_AUX_4A_L + usRegOffsetAuxByID) & BIT3))
    {
        ubTimeout = 200;

        while (((msReadByte(REG_DPRX_AUX_04_L + usRegOffsetAuxByID) & BIT0) != 0) && (ubTimeout > 0))
        {
            ubTimeout--;
        };

        u8AUX_Cmd = msReadByte(REG_DPRX_AUX_1C_L + usRegOffsetAuxByID) & 0x0F;
        u8AUX_Length = msReadByte(REG_DPRX_AUX_1D_L + usRegOffsetAuxByID);

        //======================== <Read> PC Read command ========================
        if(u8AUX_Cmd & BIT0) // DP TX handle in ddc parser
        {
        }
        //======================== <Write> PC Write command ========================
        else if((u8AUX_Cmd == 0x00) || (u8AUX_Cmd == 0x04)) // DP RX
        {
            if (u8AUX_Length)
            {
                if(glubRx_XDataQueue[dprx_aux_id][1][0] == 0x51)
                {
                    for(ubXdataIdx = 0; ubXdataIdx < DPRX_AUX_COMMAND_MAX_LENGTH; ubXdataIdx++)
                    {
                        if(MCCS_ON[ubXdataIdx] != glubRx_XDataQueue[dprx_aux_id][1][ubXdataIdx])
                        {
                            break;
                        }
                    }

                    if(ubXdataIdx == DPRX_AUX_COMMAND_MAX_LENGTH)
                    {
                        glubMCCS_Wakeup[dprx_aux_id] = DPRx_MCCS_D1;
                    }

                    for(ubXdataIdx = 0; ubXdataIdx < DPRX_AUX_COMMAND_MAX_LENGTH; ubXdataIdx++)
                    {
                        if(MCCS_OFF[ubXdataIdx] != glubRx_XDataQueue[dprx_aux_id][1][ubXdataIdx])
                        {
                            break;
                        }
                    }

                    if(ubXdataIdx == DPRX_AUX_COMMAND_MAX_LENGTH)
                    {
                        glubMCCS_Wakeup[dprx_aux_id] = DPRx_MCCS_D5;
                    }

                    for(ubXdataIdx = 0; ubXdataIdx < DPRX_AUX_COMMAND_MAX_LENGTH; ubXdataIdx++)
                    {
                        if(MCCS_STANDBY[ubXdataIdx] != glubRx_XDataQueue[dprx_aux_id][1][ubXdataIdx])
                        {
                            break;
                        }
                    }

                    if(ubXdataIdx == DPRX_AUX_COMMAND_MAX_LENGTH)
                    {
                        glubMCCS_Wakeup[dprx_aux_id] = DPRx_MCCS_D4;
                    }
                }

                ubXdataIdx = 0;

    			if(u8DDC_RxRecord == 0)
    			{
    			    u8DDC_RxRecord = TRUE;
    			    rxIndex = 0;
    			    u8AUX_Length--;
                    ubXdataIdx ++;
    			}

    			while(u8AUX_Length--)
    			{

    				if(rxIndex < DDC_BUFFER_LENGTH)
    				{
    					DDCBuffer[rxIndex++] = glubRx_XDataQueue[dprx_aux_id][1][ubXdataIdx];
    				}

                    ubXdataIdx++;
    			}
            }

          	if(u8DDC_RxRecord) // To avoid TX send I2C write cmd without length
            {
    	      	if((u8AUX_Cmd == 0x00) || ((u8AUX_Cmd == 0x04) && (rxIndex > ((DDCBuffer[0] &~ DDC2Bi_CONTROL_STATUS_FLAG) + 1)))) // Mot = 1, but the number of received data bytes > length
    	        {
    				u8DDC_RxRecord = FALSE;
    				g_ucDDC_TxRecord = 0;
    				rxStatus = DDC2B_COMPLETED;
    				txLength = 0;
    				rxInputPort = dprx_aux_id;
                    msWriteByteMask(REG_DPRX_AUX_75_L + usRegOffsetAuxByID, BIT0, BIT0); // Enable HW reply defer
    			}
    		}
        }

        msWriteByteMask(REG_DPRX_AUX_DPCD_57_L + usRegOffsetAuxDPCDByID,BIT7,BIT7); // Reset program DPCD 1 XDATA
        msWriteByteMask(REG_DPRX_AUX_4A_H + usRegOffsetAuxByID, BIT7, BIT7);        // Clear XDATA program DPCD 1 IRQ
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CheckMCCSWakeUpXDATAProgrammableDPCD()
//  [Description]
//					mhal_DPRx_CheckMCCSWakeUpXDATAProgrammableDPCD
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
DPRx_MCCS_WAKEUP mhal_DPRx_CheckMCCSWakeUpXDATAProgrammableDPCD(DPRx_AUX_ID dprx_aux_id) // check to see if xdata programmable dpcd received mccs command
{
    DPRx_MCCS_WAKEUP dprx_mccs_wakeup = glubMCCS_Wakeup[dprx_aux_id];

    glubMCCS_Wakeup[dprx_aux_id] = DPRx_MCCS_MAX;

    return dprx_mccs_wakeup;
}

#if (DPRx_MCCS_SUPPORT == 0x1)
//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetMCCSReceiveFlag()
//  [Description]
//					mhal_DPRx_GetMCCSReceiveFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetMCCSReceiveFlag(DPRx_AUX_ID dprx_aux_id, BYTE ucInputPort)
{
	BOOL bEventFlag = 0;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_3C_L + usRegOffsetAuxByID) & BIT7)? TRUE : FALSE;

    if(bEventFlag)
    {
        mhal_DPRx_MCCSReceiveProc(dprx_aux_id, ucInputPort);

        // Clear IRQ, then trigger HW send ACK and clear DDC buffer
        msRegs(REG_DPRX_AUX_3C_H + usRegOffsetAuxByID) = msRegs(REG_DPRX_AUX_3C_H + usRegOffsetAuxByID) | BIT7;
    }

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_MCCSReceiveProc()
//  [Description]
//					mhal_DPRx_MCCSReceiveProc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_MCCSReceiveProc(DPRx_AUX_ID dprx_aux_id, BYTE ucInputPort)
{
    BYTE u8AUXCH_RxCmd;
    BYTE u8DDC_Length;
    BYTE AuxData;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    u8AUXCH_RxCmd = msRegs(REG_DPRX_AUX_1C_L + usRegOffsetAuxByID) & 0x0F;
    u8DDC_Length = msRegs(REG_DPRX_AUX_1D_L + usRegOffsetAuxByID);

    //======================== <Write> PC read command ========================
    if(u8AUXCH_RxCmd & _BIT0) // DP TX
    {
		if(u8DDC_Length != 0x0)
		{
            if((rxStatus == DDC2B_CLEAR) && txLength)
            {
				while((u8DDC_Length--) && txLength)
				{
					if(g_ucDDC_TxRecord)
					{
						if((g_ucDDC_TxRecord - 1) < DDC_BUFFER_LENGTH)
						{
							msRegs(REG_DPRX_AUX_79_H + usRegOffsetAuxByID) = DDCBuffer[g_ucDDC_TxRecord-1];
						}

					    txLength--;
					}
					else
					{
					    msRegs(REG_DPRX_AUX_79_H + usRegOffsetAuxByID) = DDC2B_DEST_ADDRESS;
					}

					g_ucDDC_TxRecord++;

					msRegs(REG_DPRX_AUX_79_L + usRegOffsetAuxByID) = msRegs(REG_DPRX_AUX_79_L + usRegOffsetAuxByID) | BIT0;

					while(!(((msRegs(REG_DPRX_AUX_79_L + usRegOffsetAuxByID)) & BIT2))) // Wait
					{
					};

					msRegs(REG_DPRX_AUX_79_L + usRegOffsetAuxByID) = msRegs(REG_DPRX_AUX_79_L + usRegOffsetAuxByID) | BIT1;
				}
            }
            else
            {
                while(u8DDC_Length--)
                {
					msRegs(REG_DPRX_AUX_79_H + usRegOffsetAuxByID) = dpNullMsg[g_ucDDC_TxRecord++];
					msRegs(REG_DPRX_AUX_79_L + usRegOffsetAuxByID) = msRegs(REG_DPRX_AUX_79_L + usRegOffsetAuxByID) | BIT0;

					while(!(((msRegs(REG_DPRX_AUX_79_L + usRegOffsetAuxByID)) & BIT2)))	// Wait
					{
					};

					msRegs(REG_DPRX_AUX_79_L + usRegOffsetAuxByID) = msRegs(REG_DPRX_AUX_79_L + usRegOffsetAuxByID) | BIT1;
                }
            }

            u8DDC_RxRecord = FALSE;
        }
    }
    //======================== <Write> PC write command ========================
    else if((u8AUXCH_RxCmd == 0x00) || (u8AUXCH_RxCmd == 0x04)) // DP RX
    {
		//printf("<Write> PC write command\r\n");

		if (u8DDC_Length)
        {
			if(u8DDC_RxRecord == 0)
			{
			    u8DDC_RxRecord = TRUE;
			    rxIndex = 0;

			    msRegs(REG_DPRX_AUX_78_L + usRegOffsetAuxByID) = msRegs(REG_DPRX_AUX_78_L + usRegOffsetAuxByID) | BIT0;

			    while(!(msRegs(REG_DPRX_AUX_78_L + usRegOffsetAuxByID) & BIT2))	 // Wait
			    {};

			    msRegs(REG_DPRX_AUX_78_L + usRegOffsetAuxByID) = msRegs(REG_DPRX_AUX_78_L + usRegOffsetAuxByID) | BIT1;

			    AuxData = msRegs(REG_DPRX_AUX_3A_H + usRegOffsetAuxByID);
			    u8DDC_Length--;
			}

			while(u8DDC_Length--)
			{
			    msRegs(REG_DPRX_AUX_78_L + usRegOffsetAuxByID) = msRegs(REG_DPRX_AUX_78_L + usRegOffsetAuxByID) | BIT0;

			    while(!(msRegs(REG_DPRX_AUX_78_L + usRegOffsetAuxByID) & BIT2)) // Wait
			    {};

			    msRegs(REG_DPRX_AUX_78_L + usRegOffsetAuxByID) = msRegs(REG_DPRX_AUX_78_L + usRegOffsetAuxByID) | BIT1;

				if(rxIndex < DDC_BUFFER_LENGTH)
				{
					DDCBuffer[rxIndex++] = msRegs(REG_DPRX_AUX_3A_H + usRegOffsetAuxByID);
				}
			}
        }

      	if(u8DDC_RxRecord) // To avoid TX send I2C write cmd without length
        {
	      	if((u8AUXCH_RxCmd == 0x00) || ((u8AUXCH_RxCmd == 0x04) && (rxIndex > ((DDCBuffer[0] &~ DDC2Bi_CONTROL_STATUS_FLAG) + 1)))) // Mot = 1, but the number of received data bytes > length
	        {
				u8DDC_RxRecord = FALSE;
				g_ucDDC_TxRecord = 0;
				rxStatus = DDC2B_COMPLETED;
				txLength = 0;
				rxInputPort = ucInputPort;
			}
		}
    }

    msRegs(REG_DPRX_AUX_1F_L + usRegOffsetAuxByID) = 0;
    msRegs(REG_DPRX_AUX_3B_H + usRegOffsetAuxByID) = msRegs(REG_DPRX_AUX_3B_H + usRegOffsetAuxByID) | BIT0; // Trigger

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetMCCSReplyEnable()
//  [Description]
//                  mhal_DPRx_SetMCCSReplyEnable
//  [Arguments]:
//
//  [Return]:
//                  TRUE: Reply MCCS normally
//
//**************************************************************************
BOOL mhal_DPRx_SetMCCSReplyEnable(DPRx_AUX_ID dprx_aux_id, BOOL bEnable)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

    if(bEnable == TRUE)
    {
		msWriteByteMask(REG_DPRX_AUX_1D_H + usRegOffsetAuxByID, AUX_MCCS_SLAVE_ADDR, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6);
    }
	else
	{
		msWriteByteMask(REG_DPRX_AUX_1D_H + usRegOffsetAuxByID, (~AUX_MCCS_SLAVE_ADDR), BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6);
	}

    return TRUE;
}
#endif

void ________TRAINING________(void);
//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_HWFastTrainingEnable()
//  [Description]
//					mhal_DPRx_HWFastTrainingEnable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_HWFastTrainingEnable(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_TRANS_CTRL_06_L + usRegOffsetTransCTRLByID, bEnable? 0 : BIT0, BIT0);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_HWFastTrainingPeriod()
//  [Description]
//					mhal_DPRx_HWFastTrainingPeriod
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_HWFastTrainingPeriod(DPRx_ID dprx_id)
{
	WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return FALSE;
	}

    if((msReadByte(REG_DPRX_TRANS_CTRL_06_H + usRegOffsetTransCTRLByID) & BIT7) == BIT7)
	{
	    return TRUE;
	}
	else
	{
	    return FALSE;
	}
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CheatLinkTrainingSetting()
//  [Description]
//					mhal_DPRx_CheatLinkTrainingSetting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_OfflinePortLinkTrainingSetting(DPRx_AUX_ID dprx_aux_id, BYTE ucSwing, BYTE ucPreEmphasis)
{
	WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_AUX_4E_L + usRegOffsetAuxByID, (ucSwing << 6), BIT7|BIT6); // reg_max_swing_level

	msWriteByteMask(REG_DPRX_AUX_4E_L + usRegOffsetAuxByID, (ucPreEmphasis << 4), BIT5|BIT4); // reg_max_pre_level

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetFastTrainingTime()
//  [Description]
//					mhal_DPRx_SetFastTrainingTime
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetFastTrainingTime(DPRx_ID dprx_id, BYTE ubTime)
{
	WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

    msWriteByteMask(REG_DPRX_TRANS_CTRL_07_L + usRegOffsetTransCTRLByID, ubTime & 0x0F, (BIT0|BIT1|BIT2|BIT3)); // Fast training HW delay time (sec)

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CheckHWFastTrainingLock()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_CheckHWFastTrainingLock(DPRx_ID dprx_id)
{
	WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET000(dprx_id);

    if(msReadByte(REG_DPRX_TRANS_CTRL_06_H + usRegOffsetTransCTRLByID) & BIT7)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CheckHWNormalTrainingDone()
//  [Description]
//                  mhal_DPRx_CheckHWNormalTrainingDone
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_CheckHWNormalTrainingDone(DPRx_ID dprx_id)
{
	WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET000(dprx_id);

    if((msReadByte(REG_DPRX_TRANS_CTRL_20_L + usRegOffsetTransCTRLByID) & (BIT0|BIT1|BIT2)) >= 3)
	{
		return TRUE;
	}

    return FALSE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_TrainingBusy_Check()
//  [Description]
//                  mhal_DPRx_TrainingBusy_Check
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_TrainingBusy_Check(DPRx_ID dprx_id)
{
    WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET000(dprx_id);

    // Training FSM status
    // 0 --> idle
    // 1 --> TP1
    // 2 --> TP2/3/4
    // 3 --> normal done
    // 4 --> link test (CTS)
    if((((msReadByte(REG_DPRX_TRANS_CTRL_20_L + usRegOffsetTransCTRLByID) & (BIT2|BIT1|BIT0)) != 0x1) &&
         ((msReadByte(REG_DPRX_TRANS_CTRL_20_L + usRegOffsetTransCTRLByID) & (BIT2|BIT1|BIT0)) != 0x2) &&
         ((msReadByte(REG_DPRX_TRANS_CTRL_20_L + usRegOffsetTransCTRLByID) & (BIT2|BIT1|BIT0)) != 0x4)))
    {
    	return FALSE;
    }

    return TRUE;
}

void ________HDCP________(void);
//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_combo_LoadHDCPKey()
//  [Description]
//					mhal_DPRx_combo_LoadHDCPKey
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_combo_LoadHDCPKey(BYTE *pHDCPKey, BYTE *pBKSV)
{
	static BOOL bKeyFlag = FALSE;
    WORD ustemp = 0;

	if((pHDCPKey == NULL) || (pBKSV == NULL))
	{
		return;
	}

    msWriteByteMask(REG_PM_57_H, 0, BIT2); // HDCP SRAM enable

    // Load HDCP key
    if(bKeyFlag == FALSE)
    {
       if(pHDCPKey != NULL)
        {
            msWriteByteMask(REG_COMBO_GP_TOP_40_L, 0x0D, 0x0D); // REG_COMBO_GP_TOP_40[0][2][3]: Enable CPU write SRAM by XIU
            msWriteByteMask(REG_HDCPKEY_00_L, 0x00, 0xFF);
            msWriteByteMask(REG_HDCPKEY_00_H, 0x00, 0x03);

            for(ustemp = 0; ustemp < COMBO_HDCP_BKSV_SIZE; ustemp++)
            {
                msWriteByteMask(REG_HDCPKEY_01_L, pBKSV[ustemp], 0xFF); // REG_HDCPKEY_01[7:0]: The data for CPU write into HDCP KEY SRAM through XIU
			}

            for(ustemp = 0; ustemp < 284; ustemp++)
            {
                msWriteByteMask(REG_HDCPKEY_01_L, pHDCPKey[ustemp], 0xFF); // REG_HDCPKEY_01[7:0]: The data for CPU write into HDCP KEY SRAM through XIU
            }

            msWriteByteMask(REG_COMBO_GP_TOP_40_L, 0, 0x0D); // REG_COMBO_GP_TOP_40[0][2][3]: Disable CPU write SRAM by XIU

            bKeyFlag = TRUE;
        }
    }

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_HDCP2SetRxCaps()
//  [Description]
//					mhal_DPRx_HDCP2SetRxCaps
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_HDCP2SetRxCaps(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetHDCP22_2_ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

    #if(DPRX_HDCP2_ENABLE == 0)
    bEnable = FALSE;
    #endif

	if(bEnable)
    {
        // Set Rx Caps 6921Dh
        msWriteByteMask(REG_DPRX_HDCP22_2_0F_L + usRegOffsetHDCP22_2_ByID, BIT1, BIT1); // reg_rx_caps[7:0]
        msWriteByte(REG_DPRX_HDCP22_2_0F_H + usRegOffsetHDCP22_2_ByID, 0); // reg_rx_caps[15:8]
        msWriteByte(REG_DPRX_HDCP22_2_10_L + usRegOffsetHDCP22_2_ByID, 0x02); // reg_rx_caps[23:16]
    }
    else
    {
        msWriteByteMask(REG_DPRX_HDCP22_2_0F_L + usRegOffsetHDCP22_2_ByID, 0, BIT1); // reg_rx_caps[7:0]
        msWriteByte(REG_DPRX_HDCP22_2_0F_H + usRegOffsetHDCP22_2_ByID, 0); // reg_rx_caps[15:8]
        msWriteByte(REG_DPRX_HDCP22_2_10_L + usRegOffsetHDCP22_2_ByID, 0); // reg_rx_caps[23:16]
    }

    return;
}

#if(DPRX_HDCP2_ENABLE == 0x1)
//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_HDCP2InitialSetting()
//  [Description]
//					mhal_DPRx_HDCP2InitialSetting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_HDCP2InitialSetting(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);
    WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	if(bEnable)
    {
        // Avoid toggle ENC result in picture error
        msWriteByteMask(REG_DPRX_RECEIVER_70_H + usRegOffsetReceiverByID, 0, BIT5); // reg_discard_unused_cipher

        // Set limk_integrity_failure, reauth HW mode
        msWriteByteMask(REG_DPRX_HDCP22_4_4F_H + usRegOffsetHDCP22_4_ByID, BIT0|BIT1, BIT0|BIT1); // reg_rx_status_hw_md & reg_reauth_req_hw_md

        // [1] ake_init_irq clr
        // [2] ake_no_stored_km clr
        // [3] ake_stored_km clr
        // [4] locality_check clr
        // [5] ske_irq clr
        msWriteByte(REG_DPRX_HDCP22_4_73_L + usRegOffsetHDCP22_4_ByID, BIT1|BIT2|BIT3|BIT4|BIT5);

        // [11] h_pulum_irq_clr
        msWriteByte(REG_DPRX_HDCP22_4_78_H + usRegOffsetHDCP22_4_ByID, BIT3);

        // [0] r_tx_irq clr
        // [1] tx_caps_irq clr
        // [2] ekh_km_irq clr
        // [3] m_irq clr
        // [4] edkey_ks_irq clr
        // [5] r_iv_irq clr
        msWriteByte(REG_DPRX_HDCP22_4_77_L + usRegOffsetHDCP22_4_ByID, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5);

        // [9] v_received_irq_clr
        //[10] k_received_irq_clr
        msWriteByte(REG_DPRX_HDCP22_4_79_H + usRegOffsetHDCP22_4_ByID, BIT1|BIT2);

        mhal_DPRx_DELAY_NOP(0x20);

        msWriteByte(REG_DPRX_HDCP22_4_73_L + usRegOffsetHDCP22_4_ByID, 0);
        msWriteByte(REG_DPRX_HDCP22_4_78_H + usRegOffsetHDCP22_4_ByID, 0); // [11] h_pulum_irq_clr
        msWriteByte(REG_DPRX_HDCP22_4_77_L + usRegOffsetHDCP22_4_ByID, 0);
        msWriteByte(REG_DPRX_HDCP22_4_79_H + usRegOffsetHDCP22_4_ByID, 0);
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableHDCP2Interrupt()
//  [Description]
//					mhal_DPRx_EnableHDCP2Interrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableHDCP2Interrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	// HDCP22 mask
    if(bEnable == TRUE)
    {
        msWriteByteMask(REG_DPRX_DPCD1_2E_H + usRegOffsetDPCD1ByID, 0, BIT5); // dp_hdcp22_irq_mask
    }
    else
    {
        msWriteByteMask(REG_DPRX_DPCD1_2E_H + usRegOffsetDPCD1ByID, BIT5, BIT5);
    }

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableHDCP2EventMask()
//  [Description]
//					mhal_DPRx_EnableHDCP2EventMask
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableHDCP2EventMask(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	if(bEnable == TRUE)
    {
        // BK_160B, 0x71[1]: ake_init_irq_mask
        // BK_160B, 0x71[2]: ake_no_stored_km_mask
        // BK_160B, 0x71[3]: ake_stored_km_mask
        // BK_160B, 0x71[4]: locality_check_mask
        // BK_160B, 0x71[5]: ske_irq_mask
        msWriteByteMask(REG_DPRX_HDCP22_4_71_L + usRegOffsetHDCP22_4_ByID, 0, BIT1|BIT2|BIT3|BIT4|BIT5);

        // BK_160B, 0x75[0]: reg_r_tx_irq_mask
        // BK_160B, 0x75[1]: reg_tx_caps_irq_mask
        // BK_160B, 0x75[2]: reg_ekh_km_irq_mask
        // BK_160B, 0x75[3]: reg_m_irq_mask
        // BK_160B, 0x75[4]: reg_edkey_ks_irq_mask
        // BK_160B, 0x75[5]: reg_r_iv_irq_mask
        msWriteByteMask(REG_DPRX_HDCP22_4_75_L + usRegOffsetHDCP22_4_ByID, 0, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5);

		msWriteByteMask(REG_DPRX_HDCP22_4_78_H + usRegOffsetHDCP22_4_ByID, 0, BIT1); // BK_160B, 0x78[9]: h_pulum_mask
        msWriteByteMask(REG_DPRX_HDCP22_4_79_L + usRegOffsetHDCP22_4_ByID, 0, BIT5); // BK_160B, 0x79[5]: v mask
        msWriteByteMask(REG_DPRX_HDCP22_4_79_L + usRegOffsetHDCP22_4_ByID, 0, BIT6); // BK_160B, 0x79[6]: k mask
    }
    else
    {
        msWriteByteMask(REG_DPRX_HDCP22_4_71_L + usRegOffsetHDCP22_4_ByID, BIT1|BIT2|BIT3|BIT4|BIT5, BIT1|BIT2|BIT3|BIT4|BIT5);
        msWriteByteMask(REG_DPRX_HDCP22_4_75_L + usRegOffsetHDCP22_4_ByID, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5);

        msWriteByteMask(REG_DPRX_HDCP22_4_78_H + usRegOffsetHDCP22_4_ByID, BIT1, BIT1);
        msWriteByteMask(REG_DPRX_HDCP22_4_79_L + usRegOffsetHDCP22_4_ByID, BIT5, BIT5); // BK_160B, 0x79[5]: v mask
        msWriteByteMask(REG_DPRX_HDCP22_4_79_L + usRegOffsetHDCP22_4_ByID, BIT6, BIT6); // BK_160B, 0x79[6]: k mask
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableHDCP2ReadDownInterrupt()
//  [Description]
//					mhal_DPRx_EnableHDCP2ReadDownInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableHDCP2ReadDownInterrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET000(dprx_id);
    WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

    if(bEnable == TRUE)
    {
        // Clear status avoid turn on mask enter IRQ
        msWriteByte(REG_DPRX_HDCP22_4_78_H + usRegOffsetHDCP22_4_ByID, BIT3); // [11] h_pulum_clr
        msWriteByte(REG_DPRX_DPCD1_31_H + usRegOffsetDPCD1ByID, BIT5);		  // [13] dp_hdcp22_irq_clr
        mhal_DPRx_DELAY_NOP(0x20);
        msWriteByte(REG_DPRX_HDCP22_4_78_H + usRegOffsetHDCP22_4_ByID, 0);
        msWriteByte(REG_DPRX_DPCD1_31_H + usRegOffsetDPCD1ByID, 0);

        msWriteByteMask(REG_DPRX_HDCP22_4_78_H + usRegOffsetHDCP22_4_ByID, 0, BIT1);    // [9] h_pulum unmask
    }
    else
    {
        msWriteByteMask(REG_DPRX_HDCP22_4_78_H + usRegOffsetHDCP22_4_ByID, BIT1, BIT1); // [9] h_pulum mask
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetHDCP2IrqFlag()
//  [Description]
//					mhal_DPRx_GetHDCP2IrqFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetHDCP2IrqFlag(DPRx_ID dprx_id)
{
	BOOL bRet = FALSE;
    WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return FALSE;
	}

    bRet = (msReadByte(REG_DPRX_DPCD1_2D_H + usRegOffsetDPCD1ByID) & BIT5)? TRUE : FALSE; // [13] dp_hdcp22_irq

    if(bRet == TRUE)
    {
        msWriteByte(REG_DPRX_DPCD1_31_H + usRegOffsetDPCD1ByID, BIT5); // [13] dp_hdcp22_irq clear
        mhal_DPRx_DELAY_NOP(0x20);
        msWriteByte(REG_DPRX_DPCD1_31_H + usRegOffsetDPCD1ByID, 0);	   // [13] dp_hdcp22_irq clear
    }

    return bRet;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetHDCP2AKEInitFlag()
//  [Description]
//					mhal_DPRx_GetHDCP2AKEInitFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetHDCP2AKEInitFlag(DPRx_ID dprx_id)
{
	BOOL bRet = FALSE;
    WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET000(dprx_id);
    WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return FALSE;
	}

    bRet = (msReadByte(REG_DPRX_HDCP22_4_70_L + usRegOffsetHDCP22_4_ByID) & BIT1)? TRUE : FALSE; // [1] ake_init_irq

    if(bRet == TRUE)
    {
        msWriteByte(REG_DPRX_HDCP22_4_73_L + usRegOffsetHDCP22_4_ByID, BIT1); // [1] ake_init_irq_clr
        msWriteByte(REG_DPRX_HDCP22_4_77_L + usRegOffsetHDCP22_4_ByID, BIT2|BIT3); // avoid store km status false alarm , [2] ekh_km_irq clr [3] m_irq clr
        mhal_DPRx_DELAY_NOP(0x20);
        msWriteByte(REG_DPRX_HDCP22_4_73_L + usRegOffsetHDCP22_4_ByID, 0);	  // [1] ake_init_irq_clr
        msWriteByte(REG_DPRX_HDCP22_4_77_L + usRegOffsetHDCP22_4_ByID, 0);

        msWriteByteMask(REG_DPRX_RECEIVER_6E_L + usRegOffsetReceiverByID, BIT6, BIT6); // [6] HDCP13 SW reset set
        msWriteByteMask(REG_DPRX_RECEIVER_6E_L + usRegOffsetReceiverByID, 0 , BIT6);   // [6] HDCP13 SW reset clear
    }

    return bRet;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetHDCP2NoStoredKmFlag()
//  [Description]
//					mhal_DPRx_GetHDCP2NoStoredKmFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetHDCP2NoStoredKmFlag(DPRx_ID dprx_id)
{
	BOOL bRet = FALSE;
    WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return FALSE;
	}

    bRet = (msReadByte(REG_DPRX_HDCP22_4_70_L + usRegOffsetHDCP22_4_ByID) & BIT2)? TRUE : FALSE; // [2] ake_no_stored_km_irq

    if(bRet == TRUE)
    {
        msWriteByte(REG_DPRX_HDCP22_4_73_L + usRegOffsetHDCP22_4_ByID, BIT2); // [2] ake_no_stored_km_irq_clr
        mhal_DPRx_DELAY_NOP(0x20);
        msWriteByte(REG_DPRX_HDCP22_4_73_L + usRegOffsetHDCP22_4_ByID, 0);	  // [2] ake_no_stored_km_irq_clr
    }

    return bRet;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetHDCP2StoredKmFlag()
//  [Description]
//					mhal_DPRx_GetHDCP2StoredKmFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetHDCP2StoredKmFlag(DPRx_ID dprx_id)
{
	BOOL bRet = FALSE;
    WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return FALSE;
	}

    bRet = (msReadByte(REG_DPRX_HDCP22_4_70_L + usRegOffsetHDCP22_4_ByID) & BIT3)? TRUE : FALSE; // [3] ake_stored_km_irq

    if(bRet == TRUE)
    {
        msWriteByte(REG_DPRX_HDCP22_4_73_L + usRegOffsetHDCP22_4_ByID, BIT3); // [3] ake_stored_km_irq_clr
        mhal_DPRx_DELAY_NOP(0x20);
        msWriteByte(REG_DPRX_HDCP22_4_73_L + usRegOffsetHDCP22_4_ByID, 0);	  // [3] ake_stored_km_irq_clr
    }

    return bRet;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetHDCP2LCInitFlag()
//  [Description]
//					mhal_DPRx_GetHDCP2LCInitFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetHDCP2LCInitFlag(DPRx_ID dprx_id)
{
	BOOL bRet = FALSE;
    WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET000(dprx_id);
	DWORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_id);
    WORD usTimeout = 0;

	if(dprx_id == DPRx_ID_MAX)
	{
		return FALSE;
	}

    bRet = (msReadByte(REG_DPRX_HDCP22_4_70_L + usRegOffsetHDCP22_4_ByID) & BIT4)? TRUE : FALSE; // [4] locality_check_irq

    if(bRet == TRUE)
    {
        msWriteByte(REG_DPRX_HDCP22_4_73_L + usRegOffsetHDCP22_4_ByID, BIT4); // [4] locality_check_irq_clr
        msWriteByte(REG_DPRX_HDCP22_4_77_L + usRegOffsetHDCP22_4_ByID, BIT4|BIT5); // avoid ske status false alarm , [4] edkey_ks_irq clr [5] r_iv_irq clr
        mhal_DPRx_DELAY_NOP(0x20);
        msWriteByte(REG_DPRX_HDCP22_4_73_L + usRegOffsetHDCP22_4_ByID, 0);	  // [4] locality_check_irq_clr
        msWriteByte(REG_DPRX_HDCP22_4_77_L + usRegOffsetHDCP22_4_ByID, 0);

		usTimeout = 500;
		while ((((msReadByte(REG_DPRX_AUX_74_L + usRegOffsetAuxByID) & 0x38) >> 3) >= 2) && (usTimeout > 0)) //check hw AUX status free
		{
			mhal_DPRx_DELAY_NOP(25);
			usTimeout--;
		};
		msWriteByteMask(REG_DPRX_AUX_75_L + usRegOffsetAuxByID, BIT0, BIT0);  //Enable hw reply defer
    }

    return bRet;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetHDCP2SKEFlag()
//  [Description]
//					mhal_DPRx_GetHDCP2SKEFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetHDCP2SKEFlag(DPRx_ID dprx_id)
{
	BOOL bRet = FALSE;
    WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return FALSE;
	}

    bRet = (msReadByte(REG_DPRX_HDCP22_4_70_L + usRegOffsetHDCP22_4_ByID) & BIT5)? TRUE : FALSE; // [5] ske_irq

    if(bRet == TRUE)
    {
        msWriteByte(REG_DPRX_HDCP22_4_73_L + usRegOffsetHDCP22_4_ByID, BIT5); // [5] ske_irq_clr
        mhal_DPRx_DELAY_NOP(0x20);
        msWriteByte(REG_DPRX_HDCP22_4_73_L + usRegOffsetHDCP22_4_ByID, 0);	  // [5] ske_irq_clr
    }

    return bRet;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetHDCP2HprimeReadDoneFlag()
//  [Description]
//					mhal_DPRx_GetHDCP2HprimeReadDoneFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetHDCP2HprimeReadDoneFlag(DPRx_ID dprx_id)
{
	BOOL bRet = FALSE;
    WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return FALSE;
	}

    bRet = (msReadByte(REG_DPRX_HDCP22_4_78_H + usRegOffsetHDCP22_4_ByID) & BIT0)? TRUE : FALSE; // [8] h_pulum_irq

    if(bRet == TRUE)
    {
        msWriteByte(REG_DPRX_HDCP22_4_78_H + usRegOffsetHDCP22_4_ByID, BIT3); // [11] h_pulum_irq_clr
        mhal_DPRx_DELAY_NOP(0x20);
        msWriteByte(REG_DPRX_HDCP22_4_78_H + usRegOffsetHDCP22_4_ByID, 0);	  // [11] h_pulum_irq_clr
    }

    return bRet;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetHDCP2RepAuthSendAckFlag()
//  [Description]
//					mhal_DPRx_GetHDCP2RepAuthSendAckFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetHDCP2RepAuthSendAckFlag(DPRx_ID dprx_id)
{
	BOOL bRet = FALSE;
    WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return FALSE;
	}

    bRet = (msReadByte(REG_DPRX_HDCP22_4_79_L + usRegOffsetHDCP22_4_ByID) & BIT1)? TRUE : FALSE; // [1] v_received_irq

    if(bRet == TRUE)
    {
        msWriteByte(REG_DPRX_HDCP22_4_79_H + usRegOffsetHDCP22_4_ByID, BIT1); // [9] v_received_irq_clr
        mhal_DPRx_DELAY_NOP(0x20);
        msWriteByte(REG_DPRX_HDCP22_4_79_H + usRegOffsetHDCP22_4_ByID, 0);	  // [9] v_received_irq_clr
    }

    return bRet;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetHDCP2RepAuthStreamManageFlag()
//  [Description]
//					mhal_DPRx_GetHDCP2RepAuthStreamManageFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetHDCP2RepAuthStreamManageFlag(DPRx_ID dprx_id)
{
	BOOL bRet = FALSE;
    WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return FALSE;
	}

    bRet = (msReadByte(REG_DPRX_HDCP22_4_79_L + usRegOffsetHDCP22_4_ByID) & BIT2)? TRUE : FALSE; // [2] k_received_irq

    if(bRet == TRUE)
    {
        msWriteByte(REG_DPRX_HDCP22_4_79_H + usRegOffsetHDCP22_4_ByID, BIT2); // [10] k_received_irq_clr
        mhal_DPRx_DELAY_NOP(0x20);
        msWriteByte(REG_DPRX_HDCP22_4_79_H + usRegOffsetHDCP22_4_ByID, 0);	  // [10] k_received_irq_clr
    }

    return bRet;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_HDCP2GetRxData()
//  [Description]
//					mhal_DPRx_HDCP2GetRxData
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_HDCP2GetRxData(DPRx_ID dprx_id, BYTE ucDataInfo, BYTE *pHDCPData)
{
	BOOL bReceived = FALSE;
    WORD ustemp = 0;
    static WORD u16ContentStreamNum_k;
    WORD usRegOffsetHDCP22_2_ByID = DP_REG_OFFSET000(dprx_id);
    WORD usRegOffsetHDCP22_3_ByID = DP_REG_OFFSET000(dprx_id);
    WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET000(dprx_id);

	if((dprx_id == DPRx_ID_MAX) || (pHDCPData == NULL))
	{
		return FALSE;
	}

    switch(ucDataInfo)
    {
        case DP_HDCP2_INFO_AKE_RTX:
            for(ustemp = 0; ustemp < DP_HDCP2_AKE_RTX_SIZE; ustemp++)
            {
                pHDCPData[ustemp] = msReadByte(REG_DPRX_HDCP22_2_05_L + usRegOffsetHDCP22_2_ByID + ustemp); // reg_r_tx
            }

            bReceived = TRUE;
        	break;

        case DP_HDCP2_INFO_AKE_TXCAPS:
            for(ustemp = 0; ustemp < DP_HDCP2_AKE_TXCAPS_SIZE; ustemp++)
            {
                pHDCPData[ustemp] = msReadByte(REG_DPRX_HDCP22_2_09_L + usRegOffsetHDCP22_2_ByID + ustemp); // reg_tx_caps
            }

            bReceived = TRUE;
        	break;

        case DP_HDCP2_INFO_AKE_EKPUB_KM:
            for(ustemp = 0; ustemp < DP_HDCP2_AKE_EKPUB_KM_SIZE; ustemp++)
            {
                pHDCPData[ustemp] = msReadByte(REG_DPRX_HDCP22_2_11_L + usRegOffsetHDCP22_2_ByID + ustemp); // reg_ekpub_km
            }

            bReceived = TRUE;
        	break;

        case DP_HDCP2_INFO_AKE_EKH_KM_WR:
            for(ustemp = 0; ustemp < DP_HDCP2_AKE_EKH_KM_WR_SIZE; ustemp++)
            {
                pHDCPData[ustemp] = msReadByte(REG_DPRX_HDCP22_2_51_L + usRegOffsetHDCP22_2_ByID + ustemp); // reg_ekh_km_wr
            }

            bReceived = TRUE;
        	break;

        case DP_HDCP2_INFO_AKE_M:
            for(ustemp = 0; ustemp < DP_HDCP2_AKE_M_SIZE; ustemp++)
            {
                pHDCPData[ustemp] = msReadByte(REG_DPRX_HDCP22_2_59_L + usRegOffsetHDCP22_2_ByID + ustemp); // reg_m
            }

            bReceived = TRUE;
        	break;

        case DP_HDCP2_INFO_LC_RN:
            for(ustemp = 0; ustemp < DP_HDCP2_LC_RN_SIZE; ustemp++)
            {
                pHDCPData[ustemp] = msReadByte(REG_DPRX_HDCP22_2_79_L + usRegOffsetHDCP22_2_ByID + ustemp); // reg_r_n
            }

            bReceived = TRUE;
        	break;

        case DP_HDCP2_INFO_SKE_EDKEY_KS:
            for(ustemp = 0; ustemp < DP_HDCP2_SKE_EDKEY_KS_SIZE; ustemp++)
            {
                pHDCPData[ustemp] = msReadByte(REG_DPRX_HDCP22_3_10_L + usRegOffsetHDCP22_3_ByID + ustemp); // reg_edkey_ks
            }

            bReceived = TRUE;
        	break;

        case DP_HDCP2_INFO_SKE_RIV:
            for(ustemp = 0; ustemp < DP_HDCP2_SKE_RIV_SIZE; ustemp++)
            {
                pHDCPData[ustemp] = msReadByte(REG_DPRX_HDCP22_3_18_L + usRegOffsetHDCP22_3_ByID + ustemp); // reg_r_iv
            }

            bReceived = TRUE;
        	break;

        // Repeater
        case DP_HDCP2_INFO_ACK_SEND_ACK_V:
            for(ustemp = 0; ustemp < DP_HDCP2_ACK_SEND_ACK_V_SIZE; ustemp++)
            {
                pHDCPData[ustemp] = msReadByte(REG_DPRX_HDCP22_3_75_L + usRegOffsetHDCP22_3_ByID + ustemp); // reg_v
            }

            bReceived = TRUE;
        	break;

        case DP_HDCP2_INFO_STREAM_SEQ_NUM_M:
            for(ustemp = 0; ustemp < DP_HDCP2_STREAM_SEQ_NUM_M_SIZE; ustemp++)
            {
                pHDCPData[ustemp] = msReadByte(REG_DPRX_HDCP22_3_7D_L + usRegOffsetHDCP22_3_ByID + ustemp); // reg_seq_num_m
            }

            bReceived = TRUE;
        	break;

        case DP_HDCP2_INFO_STREAM_k:
            for(ustemp = 0; ustemp < DP_HDCP2_STREAM_k_SIZE; ustemp++)
            {
                pHDCPData[ustemp] = msReadByte(REG_DPRX_HDCP22_3_7F_L + usRegOffsetHDCP22_3_ByID + ustemp); // reg_k
            }

            u16ContentStreamNum_k = (pHDCPData[0] << 8) | pHDCPData[1];
			if(u16ContentStreamNum_k > 63) //StreamId Type maximum value 126 = 2*k
			{
				u16ContentStreamNum_k = 63;
				printf("HDCP22_ERROR_001");
			}

            bReceived = TRUE;
        	break;

        case DP_HDCP2_INFO_STREAM_STREAM_ID_TYPE:
            for(ustemp = 0; ustemp < (2 * u16ContentStreamNum_k); ustemp++)
            {
                pHDCPData[ustemp] = msReadByte(REG_DPRX_HDCP22_4_00_L + usRegOffsetHDCP22_4_ByID + ustemp); // reg_streamid_type
            }

            bReceived = TRUE;
        	break;

        default:
        	break;
    }

    return bReceived;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_HDCP22ReadRxinfo()
//  [Description]
//					mhal_DPRx_HDCP22ReadRxinfo
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_HDCP22ReadRxinfo(DPRx_ID dprx_id, BYTE *pRxInfo_Hbyte, BYTE *pRxInfo_Lbyte)
{
	WORD usRegOffsetHDCP22_3_ByID = DP_REG_OFFSET000(dprx_id);

	if((dprx_id == DPRx_ID_MAX) || (pRxInfo_Hbyte == NULL) || (pRxInfo_Lbyte == NULL))
	{
		return;
	}

	pRxInfo_Hbyte[0] = msReadByte(REG_DPRX_HDCP22_3_1C_L + usRegOffsetHDCP22_3_ByID);
    pRxInfo_Lbyte[0] = msReadByte(REG_DPRX_HDCP22_3_1C_H + usRegOffsetHDCP22_3_ByID);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_HDCP22GetRepeaterCapbility()
//  [Description]
//					mhal_DPRx_HDCP22GetRepeaterCapbility
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_HDCP22GetRepeaterCapbility(DPRx_ID dprx_id)
{
    BOOL bRet = FALSE;
	WORD usRegOffsetHDCP22_2_ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return FALSE;
	}

    bRet = (msReadByte(REG_DPRX_HDCP22_2_10_L + usRegOffsetHDCP22_2_ByID) & BIT0)? TRUE : FALSE;

    return bRet;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_HDCP22RepeaterEnable()
//  [Description]
//					mhal_DPRx_HDCP22RepeaterEnable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_HDCP22RepeaterEnable(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetHDCP22_2_ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_HDCP22_2_10_L + usRegOffsetHDCP22_2_ByID, bEnable? BIT0 : 0, BIT0);

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_HDCP2SetRxStatus()
//  [Description]
//					mhal_DPRx_HDCP2SetRxStatus
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_HDCP2SetRxStatus(DPRx_ID dprx_id, BYTE ucMaskIndex, BOOL bEnable)
{
	WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	if(bEnable == TRUE)
    {
        msWriteByteMask(REG_DPRX_HDCP22_4_4F_L + usRegOffsetHDCP22_4_ByID, ucMaskIndex, ucMaskIndex); // reg_rx_status
    }
    else
    {
        msWriteByteMask(REG_DPRX_HDCP22_4_4F_L + usRegOffsetHDCP22_4_ByID, 0, ucMaskIndex);
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_HDCP2SetTxData()
//  [Description]
//					mhal_DPRx_HDCP2SetTxData
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_HDCP2SetTxData(DPRx_ID dprx_id, BYTE ucDataInfo, BYTE *pHDCPData)
{
	BOOL bReceived = FALSE;
    WORD ustemp = 0;
    BYTE u8HDCPinfo[2];
    BYTE u8RecvIDList_Size;
    WORD usRegOffsetHDCP22_0_ByID = DP_REG_OFFSET000(dprx_id);
    WORD usRegOffsetHDCP22_1_ByID = DP_REG_OFFSET000(dprx_id);
    WORD usRegOffsetHDCP22_2_ByID = DP_REG_OFFSET000(dprx_id);
    WORD usRegOffsetHDCP22_3_ByID = DP_REG_OFFSET000(dprx_id);
    WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET000(dprx_id);
	DWORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_id);
    WORD usTimeout = 0;

	if((dprx_id == DPRx_ID_MAX) || (pHDCPData == NULL))
	{
		return FALSE;
	}

    switch(ucDataInfo)
    {
        case DP_HDCP2_INFO_AKE_CERTRX:
            for(ustemp = 0; ustemp < DP_HDCP2_AKE_CERTRX_SIZE; ustemp++)
            {
                if(ustemp > 512)
                {
                    msWriteByte(REG_DPRX_HDCP22_2_00_L + usRegOffsetHDCP22_2_ByID + (ustemp - 512), pHDCPData[ustemp]); // reg_cert_rx_512_521
                }
                else if(ustemp > 256)
                {
                    msWriteByte(REG_DPRX_HDCP22_1_00_L + usRegOffsetHDCP22_1_ByID + (ustemp - 256), pHDCPData[ustemp]); // reg_cert_rx_256_511
                }
                else
                {
                    msWriteByte(REG_DPRX_HDCP22_0_00_L + usRegOffsetHDCP22_0_ByID + ustemp, pHDCPData[ustemp]);			// reg_cert_rx_0_255
                }
            }

            bReceived = TRUE;
        	break;

        case DP_HDCP2_INFO_AKE_RRX:
            for(ustemp = 0; ustemp < DP_HDCP2_AKE_RRX_SIZE; ustemp++)
            {
                msWriteByte(REG_DPRX_HDCP22_2_0B_L + usRegOffsetHDCP22_2_ByID + ustemp, pHDCPData[ustemp]); // reg_r_rx
            }

            bReceived = TRUE;
        	break;

        case DP_HDCP2_INFO_AKE_RXCAPS:
            for(ustemp = 0; ustemp < DP_HDCP2_AKE_RXCAPS_SIZE; ustemp++)
            {
                msWriteByte(REG_DPRX_HDCP22_2_0F_L + usRegOffsetHDCP22_2_ByID + ustemp, pHDCPData[ustemp]); // reg_rx_caps
            }

            bReceived = TRUE;
        	break;

        case DP_HDCP2_INFO_AKE_H_PRIME:
            for(ustemp = 0; ustemp < DP_HDCP2_AKE_H_PRIME_SIZE; ustemp++)
            {
                msWriteByte(REG_DPRX_HDCP22_2_61_L + usRegOffsetHDCP22_2_ByID + ustemp, pHDCPData[ustemp]); // reg_h_pulum
            }

            bReceived = TRUE;
        	break;

        case DP_HDCP2_INFO_AKE_EKH_KM_RD:
            for(ustemp = 0; ustemp < DP_HDCP2_AKE_EKH_KM_RD_SIZE; ustemp++)
            {
                msWriteByte(REG_DPRX_HDCP22_2_71_L + usRegOffsetHDCP22_2_ByID + ustemp, pHDCPData[ustemp]); // reg_ekh_km_rd
            }

            bReceived = TRUE;
        	break;

        case DP_HDCP2_INFO_LC_L_PRIME:
            for(ustemp = 0; ustemp < DP_HDCP2_LC_L_PRIME_SIZE; ustemp++)
            {
                msWriteByte(REG_DPRX_HDCP22_3_00_L + usRegOffsetHDCP22_3_ByID + ustemp, pHDCPData[ustemp]); // reg_l_pulum
            }

            usTimeout = 500;
            while ((((msReadByte(REG_DPRX_AUX_74_L + usRegOffsetAuxByID) & 0x38) >> 3) >= 2) && (usTimeout > 0)) //check hw AUX status free
            {
                mhal_DPRx_DELAY_NOP(25);
                usTimeout--;
            };
            msWriteByteMask(REG_DPRX_AUX_75_L + usRegOffsetAuxByID, 0, BIT0);  //Disable hw reply defer

            bReceived = TRUE;
        	break;

        // Repeater
        case DP_HDCP2_INFO_RECVLIST_RXINFO:
            for(ustemp = 0; ustemp < DP_HDCP2_RECVLIST_RXINFO_SIZE; ustemp++)
            {
                msWriteByte(REG_DPRX_HDCP22_3_1C_L + usRegOffsetHDCP22_3_ByID + ustemp, pHDCPData[ustemp]); // reg_rx_info
            }

            bReceived = TRUE;
        	break;

        case DP_HDCP2_INFO_RECVLIST_SEQ_NUM_V:
            for(ustemp = 0; ustemp < DP_HDCP2_RECVLIST_SEQ_NUM_V_SIZE; ustemp++)
            {
                msWriteByte(REG_DPRX_HDCP22_3_1D_L + usRegOffsetHDCP22_3_ByID + ustemp, pHDCPData[ustemp]); // reg_seq_num_v
            }

            bReceived = TRUE;
        	break;

        case DP_HDCP2_INFO_RECVLIST_VPRIME:
            for(ustemp = 0; ustemp < DP_HDCP2_RECVLIST_VPRIME_SIZE; ustemp++)
            {
                msWriteByte(REG_DPRX_HDCP22_3_1F_L + usRegOffsetHDCP22_3_ByID + ustemp, pHDCPData[ustemp]); // reg_v_pulum
            }

            bReceived = TRUE;
        	break;

        case DP_HDCP2_INFO_RECVLIST_RECEIVER_ID_LIST:
            mhal_DPRx_HDCP22ReadRxinfo(dprx_id, &u8HDCPinfo[1], &u8HDCPinfo[0]);

            u8RecvIDList_Size = (((u8HDCPinfo[1] & BIT0) << 4) | ((u8HDCPinfo[0] & 0xF0) >> 4)) * DP_HDCP2_RECEIVER_ID_SIZE;

            for(ustemp = 0; ustemp < u8RecvIDList_Size; ustemp++)
            {
                msWriteByte(REG_DPRX_HDCP22_3_27_L + usRegOffsetHDCP22_3_ByID + ustemp, pHDCPData[ustemp]); // reg_receiver_id_list
            }

            bReceived = TRUE;
        	break;

        case DP_HDCP2_INFO_STREAM_MPRIME:
            for(ustemp = 0; ustemp < DP_HDCP2_STREAM_MPRIME_SIZE; ustemp++)
            {
                msWriteByte(REG_DPRX_HDCP22_4_3F_L + usRegOffsetHDCP22_4_ByID + ustemp, pHDCPData[ustemp]); // reg_m_pulum
            }

            bReceived = TRUE;
        	break;

        default:
        	break;
    }

    return bReceived;
}
#endif

#if (DPRX_HDCP14_Repeater_ENABLE == 0x1)
//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_HDCP14WriteV()
//  [Description]
//					mhal_DPRx_HDCP14WriteV
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_HDCP14WriteV(DPRx_AUX_ID dprx_aux_id, BYTE *VData)
{
    BYTE XDATA i;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    for(i = 0; i < 20; i++)
    {
        msWriteByte(REG_DPRX_AUX_DPCD_10_L + usRegOffsetAuxByID + i , *(VData + i));
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_HDCP14WriteBinfo()
//  [Description]
//					mhal_DPRx_HDCP14WriteBinfo
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_HDCP14WriteBinfo(DPRx_AUX_ID dprx_aux_id, BYTE Binfo_Depth, BYTE Binfo_Devs)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    msWriteByte(REG_DPRX_AUX_DPCD_1A_L + usRegOffsetAuxByID, Binfo_Devs);
    msWriteByte(REG_DPRX_AUX_DPCD_1A_H + usRegOffsetAuxByID, Binfo_Depth);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_HDCP14SetRDY()
//  [Description]
//					mhal_DPRx_HDCP14SetRDY
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_HDCP14SetRDY(DPRx_ID dprx_id, BOOL Enable)
{
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	if(Enable)
    {
        msWriteByteMask(REG_DPRX_DPCD0_67_L + usRegOffsetDPCD0ByID, BIT2, BIT2); // set rdy
    }
    else
    {
        msWriteByte(REG_DPRX_DPCD0_67_L + usRegOffsetDPCD0ByID, BIT3); // Clear rdy
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_HDCP14SetReAuthBit()
//  [Description]
//					mhal_DPRx_HDCP14SetReAuthBit
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_HDCP14SetReAuthBit(DPRx_ID dprx_id, BOOL Enable)
{
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	if(Enable)
    {
        msWriteByteMask(REG_DPRX_DPCD0_67_L + usRegOffsetDPCD0ByID, BIT0, BIT0); // Set reauth bit
    }
    else
    {
        msWriteByte(REG_DPRX_DPCD0_67_L + usRegOffsetDPCD0ByID, BIT1); // Clear reauth bit
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_HDCP14RepeaterEnable()
//  [Description]
//					mhal_DPRx_HDCP14RepeaterEnable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_HDCP14RepeaterEnable(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, BOOL bEnable)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET000(dprx_id);

	if((dprx_id == DPRx_ID_MAX) || (dprx_aux_id == DPRx_AUX_ID_MAX))
	{
		return;
	}

    msWriteByteMask(REG_DPRX_DPCD1_48_L + usRegOffsetDPCD1ByID, bEnable? BIT2 : 0, BIT2);
    msWriteByteMask(REG_DPRX_AUX_DPCD_1E_H + usRegOffsetAuxByID, bEnable? BIT0 : 0, BIT0);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_HDCP14ReadR0()
//  [Description]
//					mhal_DPRx_HDCP14ReadR0
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_HDCP14ReadR0(DPRx_ID dprx_id, BYTE *R0_Data)
{
    BYTE i;
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);

	if((dprx_id == DPRx_ID_MAX) || (R0_Data == NULL))
	{
		return;
	}

    for(i = 0; i < 2; i++)
    {
        *(R0_Data+i) = msRead2Byte(REG_DPRX_RECEIVER_76_L + usRegOffsetReceiverByID + i);
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_HDCP14WriteKSVFIFO()
//  [Description]
//					mhal_DPRx_HDCP14WriteKSVFIFO
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_HDCP14WriteKSVFIFO(DPRx_ID dprx_id, WORD u16length, BYTE *BKSVData)
{
    BYTE XDATA Address = 0;
    BYTE XDATA temp;
    BYTE XDATA ucComplementCount = 0;
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET000(dprx_id);

	if((dprx_id == DPRx_ID_MAX) || (BKSVData == NULL))
	{
		return;
	}

    if((u16length % 15) == 5)
    {
        ucComplementCount = 2;
    }
    else if((u16length % 15) == 10)
    {
        ucComplementCount = 1;
    }

    msWriteByteMask(REG_DPRX_DPCD1_4C_H, BIT7, BIT7); // Reset KSVFIFO offset/data?

    for(temp = 0; temp < (u16length / 5); temp++)
    {
        msWriteByte(REG_DPRX_DPCD1_4C_L + usRegOffsetDPCD1ByID, Address);
        msWriteByte(REG_DPRX_DPCD1_49_L + usRegOffsetDPCD1ByID, BKSVData[0+5*temp]);
        msWriteByte(REG_DPRX_DPCD1_49_H + usRegOffsetDPCD1ByID, BKSVData[1+5*temp]);
        msWriteByte(REG_DPRX_DPCD1_4A_L + usRegOffsetDPCD1ByID, BKSVData[2+5*temp]);
        msWriteByte(REG_DPRX_DPCD1_4A_H + usRegOffsetDPCD1ByID, BKSVData[3+5*temp]);
        msWriteByte(REG_DPRX_DPCD1_4B_L + usRegOffsetDPCD1ByID, BKSVData[4+5*temp]);
        msWriteByteMask(REG_DPRX_DPCD1_4C_L + usRegOffsetDPCD1ByID, BIT7, BIT7);

        Address++;

        mhal_DPRx_DELAY_NOP(50);
    }

    // Patch load KSV FIFO have to 15 Byte one time
    if(ucComplementCount != 0)
    {
        for(temp = 0; temp < ucComplementCount; temp++)
        {
            msWriteByte(REG_DPRX_DPCD1_4C_L + usRegOffsetDPCD1ByID, Address);
            msWriteByte(REG_DPRX_DPCD1_49_L + usRegOffsetDPCD1ByID, 0);
            msWriteByte(REG_DPRX_DPCD1_49_H + usRegOffsetDPCD1ByID, 0);
            msWriteByte(REG_DPRX_DPCD1_4A_L + usRegOffsetDPCD1ByID, 0);
            msWriteByte(REG_DPRX_DPCD1_4A_H + usRegOffsetDPCD1ByID, 0);
            msWriteByte(REG_DPRX_DPCD1_4B_L + usRegOffsetDPCD1ByID, 0);
            msWriteByteMask(REG_DPRX_DPCD1_4C_L + usRegOffsetDPCD1ByID, BIT7, BIT7);

            Address++;

            mhal_DPRx_DELAY_NOP(50);
        }
    }

	return;
}
#endif

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableAKSVInterrupt()
//  [Description]
//					mhal_DPRx_EnableAKSVInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableAKSVInterrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	// AKSV mask
    if(bEnable == TRUE)
    {
        msWriteByteMask(REG_DPRX_DPCD1_2E_L + usRegOffsetDPCD1ByID, 0, BIT7); // dp_hdcp22_irq_mask
    }
    else
    {
        msWriteByteMask(REG_DPRX_DPCD1_2E_L + usRegOffsetDPCD1ByID, BIT7, BIT7);
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetAKSVReceivedFlag()
//  [Description]
//					mhal_DPRx_GetAKSVReceivedFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetAKSVReceivedFlag(DPRx_ID dprx_id)
{
	BOOL bRet = FALSE;
    WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET000(dprx_id);
    WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return FALSE;
	}

    bRet = (msReadByte(REG_DPRX_DPCD1_2D_L + usRegOffsetDPCD1ByID) & BIT7)? TRUE : FALSE; // [7] AKSV reveived

    if(bRet == TRUE)
    {
		msWriteByte(REG_DPRX_DPCD1_31_L + usRegOffsetDPCD1ByID, BIT7); // AKSV received IRQ clear
		mhal_DPRx_DELAY_NOP(0x30);
		msWriteByte(REG_DPRX_DPCD1_31_L + usRegOffsetDPCD1ByID, 0);	   // AKSV received IRQ enable

		msWriteByteMask(REG_DPRX_RECEIVER_6E_L + usRegOffsetReceiverByID, BIT7 , BIT7); // HDCP22 SW reset
		msWriteByteMask(REG_DPRX_RECEIVER_6E_L + usRegOffsetReceiverByID, 0 , BIT7);	// HDCP22 SW reset clear

		msWriteByteMask(REG_DPRX_RECEIVER_70_H + usRegOffsetReceiverByID, 0, BIT4); // HDCP22 dpes/authentication done reset when AKSV received
    }

    return bRet;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CheckHDCPState()
//  [Description]
//                  Check HDCP is 1.4 or 2.2 or no encryption
//  [Arguments]:
//
//  [Return]:
//                  HDCP1.4 or HDCP2.2 or no encryption
//
//**************************************************************************
DP_HDCP_STATE mhal_DPRx_CheckHDCPState(DPRx_ID dprx_id)
{
    DP_HDCP_STATE ucHDCPstatus = DP_HDCP_NO_ENCRYPTION;
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return DP_HDCP_NO_ENCRYPTION;
	}

    if((msReadByte(REG_DPRX_RECEIVER_73_H + usRegOffsetReceiverByID) & 0x07) != 0x00)
    {
	    if((msReadByte(REG_DPRX_RECEIVER_73_H + usRegOffsetReceiverByID) & BIT7) == BIT7)
		{
		    ucHDCPstatus = DP_HDCP_2_2;
		}
		else
		{
			ucHDCPstatus = DP_HDCP_1_4;
		}
	}
	else
	{
	    ucHDCPstatus = DP_HDCP_NO_ENCRYPTION;
	}

    return ucHDCPstatus;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CheckHDCPState()
//  [Description]
//                  Check HDCP is 1.4 or 2.2 or no encryption
//  [Arguments]:
//
//  [Return]:
//                  HDCP1.4 or HDCP2.2 or no encryption
//
//**************************************************************************
DP_HDCP_STATE mhal_DPRx_CheckHDCPEncryption(DPRx_ID dprx_id)
{
    DP_HDCP_STATE ucHDCPstatus = DP_HDCP_NO_ENCRYPTION;
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return DP_HDCP_NO_ENCRYPTION;
	}

    if((msReadByte(REG_DPRX_RECEIVER_6E_H + usRegOffsetReceiverByID) & (BIT3|BIT2)) != 0x0) // Detect main link data encrypt even no authentication
    {
	    if((msReadByte(REG_DPRX_RECEIVER_73_H + usRegOffsetReceiverByID) & BIT7) == BIT7)
		{
		    ucHDCPstatus = DP_HDCP_2_2;
		}
		else
		{
			ucHDCPstatus = DP_HDCP_1_4;
		}
	}
	else
	{
	    ucHDCPstatus = DP_HDCP_NO_ENCRYPTION;
	}

    return ucHDCPstatus;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ForceHDCP13IntegrityFail()
//  [Description]
//                  Force DPCD 0x68029 report link integrity fail
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_ForceHDCP13IntegrityFail(DPRx_ID dprx_id, BOOL bForceEnable)
{
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	if(bForceEnable == TRUE)
	{
		msWriteByteMask(REG_DPRX_DPCD0_1A_L + usRegOffsetDPCD0ByID, BIT2|BIT3, BIT2|BIT3);
	}
	else
	{
		msWriteByteMask(REG_DPRX_DPCD0_1A_L + usRegOffsetDPCD0ByID, 0, BIT2|BIT3);
	}

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ForceHDCP22IntegrityFail()
//  [Description]
//                  Force DPCD 0x68029 report link integrity fail
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_ForceHDCP22IntegrityFail(DPRx_ID dprx_id, BOOL bForceEnable)
{
	WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	if(bForceEnable == TRUE)
	{
		msWriteByteMask(REG_DPRX_HDCP22_4_4F_H + usRegOffsetHDCP22_4_ByID, 0, BIT0); // Turn off reg_rx_status_hw_md
		msWriteByteMask(REG_DPRX_HDCP22_4_4F_L + usRegOffsetHDCP22_4_ByID, BIT4, BIT4);
	}
	else
	{
		msWriteByteMask(REG_DPRX_HDCP22_4_4F_L + usRegOffsetHDCP22_4_ByID, 0, BIT4);
		msWriteByteMask(REG_DPRX_HDCP22_4_4F_H + usRegOffsetHDCP22_4_ByID, BIT0, BIT0); // Turn on reg_rx_status_hw_md
	}

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetHDCPIntegrity()
//  [Description]
//                  mhal_DPRx_GetHDCPIntegrity
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetHDCPIntegrity(DPRx_ID dprx_id)
{
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return FALSE;
	}

	return ((msReadByte(REG_DPRX_RECEIVER_73_H + usRegOffsetReceiverByID) & 0x07) == BIT2)? TRUE : FALSE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ClearHDCPIntegrity()
//  [Description]
//                  mhal_DPRx_ClearHDCPIntegrity
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_ClearHDCPIntegrity(DPRx_ID dprx_id)
{
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_RECEIVER_6F_H + usRegOffsetReceiverByID, BIT7, BIT7);
    msWriteByteMask(REG_DPRX_RECEIVER_6F_H + usRegOffsetReceiverByID, 0, BIT7);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetCPIRQ()
//  [Description]
//					mhal_DPRx_SetCPIRQ
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetCPIRQ(DPRx_AUX_ID dprx_aux_id)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    msWriteByteMask(REG_DPRX_AUX_DPCD_5A_L + usRegOffsetAuxByID, BIT2, BIT2);

    return;
}

//**************************************************************************
//  [Function Name]:
// 				 	mhal_DPRx_HDCPLinkFailReset()
//  [Description]
//					mhal_DPRx_HDCPLinkFailReset
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_HDCPLinkFailReset(DPRx_ID dprx_id)
{
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_RECEIVER_6F_H + usRegOffsetReceiverByID, BIT7, BIT7); // [7]: Link fail reset
    mhal_DPRx_DELAY_NOP(5);
    msWriteByteMask(REG_DPRX_RECEIVER_6F_H + usRegOffsetReceiverByID, 0, BIT7);

    return;
}

//**************************************************************************
//  [Function Name]:
// 				 	mhal_DPRx_HDCP14SoftReset()
//  [Description]
//					mhal_DPRx_HDCP14SoftReset
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_HDCP14SoftReset(DPRx_ID dprx_id)
{
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_RECEIVER_6E_L + usRegOffsetReceiverByID, BIT6, BIT6); // HDCP14 soft reset
    mhal_DPRx_DELAY_NOP(5);
    msWriteByteMask(REG_DPRX_RECEIVER_6E_L + usRegOffsetReceiverByID, 0, BIT6);

    return;
}

//**************************************************************************
//  [Function Name]:
// 				 	mhal_DPRx_HDCP22SoftReset()
//  [Description]
//					mhal_DPRx_HDCP22SoftReset
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_HDCP22SoftReset(DPRx_ID dprx_id)
{
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_RECEIVER_6E_L + usRegOffsetReceiverByID, BIT7, BIT7); // HDCP22 soft reset
    mhal_DPRx_DELAY_NOP(5);
    msWriteByteMask(REG_DPRX_RECEIVER_6E_L + usRegOffsetReceiverByID, 0, BIT7);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetAKSVFlag()
//  [Description]
//					mhal_DPRx_GetAKSVFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetAKSVFlag(DPRx_AUX_ID dprx_aux_id)
{
    BOOL ucFlag = FALSE;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

	if(msReadByte(REG_DPRX_AUX_49_H + usRegOffsetAuxByID) & BIT4)
	{
	    ucFlag = TRUE;
	}

	return ucFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ClearAKSVFlag()
//  [Description]
//					mhal_DPRx_ClearAKSVFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_ClearAKSVFlag(DPRx_AUX_ID dprx_aux_id)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    msWriteByteMask(REG_DPRX_AUX_49_H + usRegOffsetAuxByID, BIT7, BIT7);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_HDCP14CheckAKSVStatus()
//  [Description]
//					mhal_DPRx_HDCP14CheckAKSVStatus
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_HDCP14CheckAKSVStatus(DPRx_AUX_ID dprx_aux_id)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
    BOOL bRet = FALSE;

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bRet = (msReadByte(REG_DPRX_AUX_DPCD_2D_L + usRegOffsetAuxByID) & BIT0)? TRUE : FALSE; // reg_hdcp_aksv_irq

    if(bRet == TRUE)
    {
        msWriteByteMask(REG_DPRX_AUX_DPCD_2D_L + usRegOffsetAuxByID, BIT1, BIT1); // reg_hdcp_aksv_irq_clr
    }

    return bRet;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_HDCP14CheckAnStatus()
//  [Description]
//					mhal_DPRx_HDCP14CheckAnStatus
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_HDCP14CheckAnStatus(DPRx_AUX_ID dprx_aux_id)
{
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
    BOOL bRet = FALSE;

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bRet = (msReadByte(REG_DPRX_AUX_DPCD_2D_L + usRegOffsetAuxByID) & BIT4)? TRUE : FALSE; // reg_hdcp_an_irq

    if(bRet == TRUE)
    {
        msWriteByteMask(REG_DPRX_AUX_DPCD_2D_L + usRegOffsetAuxByID, BIT5, BIT5); // reg_hdcp_an_irq_clr
    }

    return bRet;
}

void ________TYPE_C________(void);
//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_TypeC_AUX_N_Level()
//  [Description]
//					mhal_DPRx_TypeC_AUX_N_Level
//  [Arguments]:
//
//  [Return]:
//					1: Cable connect
//					0: Cable disconnect
//
//**************************************************************************
BOOL mhal_DPRx_TypeC_AUX_N_Level(DPRx_ID dprx_id, BYTE ubPinAssign, BYTE ubCCpinInfo)
{
	if(dprx_id == DPRx_ID_MAX)
	{
		return FALSE;
	}

	#if (DP_TYPE_C_EN == 0x1)
	if(dprx_id == DPRx_ID_3)
	{
		if((ubPinAssign == BIT2)||(ubPinAssign == BIT3)) //Pin Assign C, D case
		{
			if(ubCCpinInfo == 0x1) //Flip plug
			{
				return hwSDM_AuxP_TYPEC4Pin_Pin;
			}
			else if(ubCCpinInfo == 0x0)
			{
				return hwSDM_AuxN_TYPEC4Pin_Pin;
			}
		}
		else if(ubPinAssign == BIT4) // Pin Assign E case
		{
			if(ubCCpinInfo == 0x1) //Flip plug
			{
				return hwSDM_AuxN_TYPEC4Pin_Pin;
			}
			else if(ubCCpinInfo == 0x0)
			{
				return hwSDM_AuxP_TYPEC4Pin_Pin;
			}
		}
	}
    #else
    ubPinAssign = 0x0;
    ubCCpinInfo = 0x0;
	#endif

	return FALSE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_TypeC_CheckSquelch()
//  [Description]
//                  mhal_DPRx_TypeC_CheckSquelch
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_TypeC_CheckSquelch(DPRx_ID dprx_id, BYTE ubPinAssign, BYTE ubCCpinInfo)
{
    BOOL bSquelchFlag = FALSE;

    if(dprx_id == DPRx_ID_MAX)
    {
        return bSquelchFlag;
    }

    // Type C connector swap
    // Lane 0 --> Lane 3
    // Lane 1 --> Lane 0
    // Lane 2 --> Lane 1
    // Lane 3 --> Lane 2
    if((ubPinAssign == BIT2) || (ubPinAssign == BIT3)) // Pin Assign C, D case
    {
        if(ubCCpinInfo == 0x1) // Flip plug
        {
            bSquelchFlag = (msReadByte(REG_1526EF) & BIT6) >> 6;
        }
        else if(ubCCpinInfo == 0x0)
        {
            bSquelchFlag = (msReadByte(REG_1526EF) & BIT7) >> 7;
        }
    }
    else if(ubPinAssign == BIT4) // Pin Assign E case
    {
        if(ubCCpinInfo == 0x1) // Flip plug
        {
            bSquelchFlag = (msReadByte(REG_1526EF) & BIT5) >> 5;
        }
        else if(ubCCpinInfo == 0x0)
        {
            bSquelchFlag = (msReadByte(REG_1526EF) & BIT4) >> 4;
        }
    }

    return bSquelchFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetLaneSwapEnable()
//  [Description]
//					mhal_DPRx_SetLaneSwapEnable
//  [Arguments]:
//					ubTargetLaneNumber: [1:0] for lane0
//										[3:2] for lane1
//										[5:4] for lane2
//										[7:6] for lane3
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_SetLaneSwapEnable(DPRx_ID dprx_id, DPRx_PHY_ID dprx_phy_id, BOOL bEnable, BYTE ubTargetLaneNumber)
{
	if((dprx_id == DPRx_ID_MAX) || (dprx_phy_id == DPRx_PHY_ID_MAX))
	{
		return FALSE;
	}

    ubTargetLaneNumber = ubTargetLaneNumber;

    if(bEnable == TRUE)
	{
	}
	else
	{
	}

	return TRUE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetLanePNSwapEnable()
//  [Description]
//					mhal_DPRx_SetLanePNSwapEnable
//  [Arguments]:
//					ubTargetLaneNumber: [1:0] for Lane0,
//										[3:2] for Lane1,
//										[5:4] for Lane2,
//										[7:6] for Lane3
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_SetLanePNSwapEnable(DPRx_ID dprx_id, BOOL b4LanePNSwap, BYTE ubLanePNSwapSelect)
{
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);
	WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return FALSE;
	}

	if(b4LanePNSwap == TRUE)
	{
		printf("Set 4 lane PN Swap = %x !\r\n", (ubLanePNSwapSelect & 0x0F));
		//MAC pn swap
        msWriteByteMask(REG_DPRX_RECEIVER_06_H + usRegOffsetReceiverByID, (ubLanePNSwapSelect & 0x0F), 0xF);
        msWriteByte(REG_DPRX_TRANS_CTRL_2C_L + usRegOffsetTransCTRLByID, glTypeC_PhyLaneSwap[dprx_id]);
	}
	else
	{
		//MAC pn swap
		msWriteByteMask(REG_DPRX_RECEIVER_06_H , 0, 0xF);
	}

	return TRUE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetAuxPNSwapEnable()
//  [Description]
//					mhal_DPRx_SetAuxPNSwapEnable
//  [Arguments]:
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_SetAuxPNSwapEnable(DPRx_AUX_ID dprx_aux_id, BOOL bAuxPNSwap)
{
	WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

	if(bAuxPNSwap == TRUE)
	{
		printf("Set Aux PN Swap !\r\n");

		msWriteByteMask(REG_DPRX_AUX_18_L + usRegOffsetAuxByID, BIT0, BIT0);
		msWriteByteMask(REG_DPRX_AUX_18_H + usRegOffsetAuxByID, BIT7, BIT7);
	}
	else
	{
		msWriteByteMask(REG_DPRX_AUX_18_L + usRegOffsetAuxByID, 0x0, BIT0);
		msWriteByteMask(REG_DPRX_AUX_18_H + usRegOffsetAuxByID, 0x0, BIT7);
	}

	return TRUE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_MSCHIP_TypeC_PRBS7PNswap()
//  [Description]
//                  mhal_DPRx_MSCHIP_TypeC_PRBS7PNswap
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_MSCHIP_TypeC_PRBS7PNswap(DPRx_ID dprx_id, BYTE ubTypeC_PinAssign)
{
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);
    WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET000(dprx_id);
	WORD usRegOffsetPHY0ByID = DP_REG_OFFSET400(dprx_id);
    BYTE LINK_RATE;

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	LINK_RATE = msReadByte(REG_DPRX_DPCD0_20_L + usRegOffsetDPCD0ByID);

	// For RBR/HBR JTOL PRBS7 setting
	if((LINK_RATE == DP_LINKRATE_HBR3) || (LINK_RATE == DP_LINKRATE_HBR2))
	{
		msWriteByteMask(REG_DPRX_PHY0_0C_L + usRegOffsetPHY0ByID, glTypeC_PhyLaneSwap[dprx_id], BIT0|BIT1|BIT2|BIT3);
		msWriteByteMask(REG_DPRX_RECEIVER_06_H + usRegOffsetReceiverByID, glTypeC_MacPNswap[dprx_id], BIT0|BIT1|BIT2|BIT3);
	}
	else if((LINK_RATE == DP_LINKRATE_HBR) || (LINK_RATE == DP_LINKRATE_RBR))
	{
		if((ubTypeC_PinAssign == BIT2)||(ubTypeC_PinAssign == BIT3)) // pinC D
		{
			msWriteByteMask(REG_DPRX_PHY0_0C_L + usRegOffsetPHY0ByID, 0xC, BIT0|BIT1|BIT2|BIT3);
			msWriteByteMask(REG_DPRX_RECEIVER_06_H + usRegOffsetReceiverByID, 0x0, BIT0|BIT1|BIT2|BIT3);
		}
		else if(ubTypeC_PinAssign == BIT4) // pinE
		{
			msWriteByteMask(REG_DPRX_PHY0_0C_L + usRegOffsetPHY0ByID, 0x3, BIT0|BIT1|BIT2|BIT3);
			msWriteByteMask(REG_DPRX_RECEIVER_06_H + usRegOffsetReceiverByID, 0x0, BIT0|BIT1|BIT2|BIT3);
		}
	}

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_MSCHIP_TypeC_PRBS7PNswapforFT()
//  [Description]
//                  mhal_DPRx_MSCHIP_TypeC_PRBS7PNswapforFT
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_MSCHIP_TypeC_PRBS7PNswapforFT(DPRx_ID dprx_id, BYTE ubTypeC_PinAssign)
{
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);
	WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET000(dprx_id);
	WORD usRegOffsetPHY0ByID = DP_REG_OFFSET400(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	// For RBR/HBR JTOL PRBS7 setting
	// HBR3 / HBR2.5 / HBR2 for fast training
	if(((msReadByte(REG_DPRX_TRANS_CTRL_07_H + usRegOffsetTransCTRLByID) & (BIT4|BIT5|BIT6)) == BIT6) ||
	   ((msReadByte(REG_DPRX_TRANS_CTRL_07_H + usRegOffsetTransCTRLByID) & (BIT4|BIT5|BIT6)) == (BIT4|BIT5)) ||
	   ((msReadByte(REG_DPRX_TRANS_CTRL_07_H + usRegOffsetTransCTRLByID) & (BIT4|BIT5|BIT6)) == BIT5))
	{
		msWriteByteMask(REG_DPRX_PHY0_0C_L + usRegOffsetPHY0ByID, glTypeC_PhyLaneSwap[dprx_id], BIT0|BIT1|BIT2|BIT3);
		msWriteByteMask(REG_DPRX_RECEIVER_06_H + usRegOffsetReceiverByID, glTypeC_MacPNswap[dprx_id], BIT0|BIT1|BIT2|BIT3);
	}
	// HBR / RBR for fast training
	else if(((msReadByte(REG_DPRX_TRANS_CTRL_07_H + usRegOffsetTransCTRLByID) & (BIT4|BIT5|BIT6)) == BIT4) ||
			((msReadByte(REG_DPRX_TRANS_CTRL_07_H + usRegOffsetTransCTRLByID) & (BIT4|BIT5|BIT6)) == 0))
	{
		if((ubTypeC_PinAssign == BIT2) || (ubTypeC_PinAssign == BIT3)) // pinC D
		{
			msWriteByteMask(REG_DPRX_PHY0_0C_L + usRegOffsetPHY0ByID, 0xC, BIT0|BIT1|BIT2|BIT3);
			msWriteByteMask(REG_DPRX_RECEIVER_06_H + usRegOffsetReceiverByID, 0x0, BIT0|BIT1|BIT2|BIT3);
		}
		else if(ubTypeC_PinAssign == BIT4) // pinE
		{
			msWriteByteMask(REG_DPRX_PHY0_0C_L + usRegOffsetPHY0ByID, 0x3, BIT0|BIT1|BIT2|BIT3);
			msWriteByteMask(REG_DPRX_RECEIVER_06_H + usRegOffsetReceiverByID, 0x0, BIT0|BIT1|BIT2|BIT3);
		}
	}

    return;
}

void ________POWER________(void);
//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_Polling600h()
//  [Description]
//					mhal_DPRx_Polling600h
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_Polling600h(DPRx_AUX_ID dprx_aux_id)
{
    BOOL XDATA BStatus = FALSE;
    WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

	if(msReadByte(REG_DPRX_AUX_3C_L + usRegOffsetAuxByID) & BIT6)
	{
		msWriteByte(REG_DPRX_AUX_3C_H + usRegOffsetAuxByID, BIT6);

	    BStatus = TRUE;
	}

    return BStatus;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_PLLOven()
//  [Description]
//					mhal_DPRx_PLLOven
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_PLLOven(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable)
{
	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

	if(bEnable == TRUE)
	{
		msWriteByteMask(REG_DPRX_PLL_51_L + (dprx_decoder_id * 16), BIT5|BIT4, BIT6|BIT5|BIT4);
	}
	else
	{
		msWriteByteMask(REG_DPRX_PLL_51_L + (dprx_decoder_id * 16), 0, BIT6|BIT5|BIT4);
	}

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SymbolEQResetEnable()
//  [Description]
//					mhal_DPRx_SymbolEQResetEnable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SymbolEQResetEnable(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET000(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	if(bEnable)
    {
        msWriteByteMask(REG_DPRX_RECEIVER_01_L + usRegOffsetReceiverByID, BIT6, BIT6);
        msWriteByteMask(REG_DPRX_RECEIVER_01_H + usRegOffsetReceiverByID, BIT1, BIT1);
    }
    else
    {
        msWriteByteMask(REG_DPRX_RECEIVER_01_L + usRegOffsetReceiverByID, 0, BIT6);
        msWriteByteMask(REG_DPRX_RECEIVER_01_H + usRegOffsetReceiverByID, 0, BIT1);
    }

	return;
}
#endif // ENABLE_DP_INPUT

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetAuxClockGating()
//  [Description]
//					mhal_DPRx_SetAuxClockGating
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetAuxClockGating(BOOL bEnable)
{
    if(bEnable == TRUE)
    {
        msWriteByteMask(REG_PM_49_L, BIT0, BIT0); // Enable clock gating
    }
    else
    {
        msWriteByteMask(REG_PM_49_L, 0, BIT0); // Disable clock gating
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetPowerDownControl()
//  [Description]
//					mhal_DPRx_SetPowerDownControl
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetPowerDownControl(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id, DP_ePM_Mode dp_pm_Mode)
{
	WORD usRegOffsetAuxByID = DP_REG_OFFSET300(dprx_aux_id);
	WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET000(dprx_id);

	if(((dprx_id == DPRx_ID_MAX) && (dp_pm_Mode != DP_ePM_POWEROFF_NoUsedPort)) || (dprx_aux_id == DPRx_AUX_ID_MAX) || (dp_pm_Mode == DP_ePM_INVAILD))
	{
        // Power down PHY1 if DPC port is not used
        if(DPRx_C4_AUX == AUX_None)
        {
            mhal_DPRx_PHYPowerModeSetting(DP_ePM_POWEROFF, DPRx_ID_3, DPRx_PHY_ID_1);
        }

		return;
	}

#if (ENABLE_DP_DCOFF_HPD_HIGH == 1)
    if(dp_pm_Mode == DP_ePM_POWEROFF)
    {
        dp_pm_Mode = DP_ePM_STANDBY;
    }
#endif

    msWriteByteMask(REG_DPRX_AUX_57_L + usRegOffsetAuxByID, 0, BIT0|BIT1|BIT2); // Disble DP event to PM, these 3 bits would affect HDMI/VGA wake-up

	switch(dp_pm_Mode)
	{
		case DP_ePM_POWERON:
			msWriteByteMask(REG_DPRX_PLL_30_H, 0, BIT0); // reg_apll_pwdwn_0
			msWriteByteMask(REG_DPRX_PLL_50_H, 0, BIT0); // reg_vpll_pwdwn_0
			msWriteByteMask(REG_DPRX_AUX_54_L + usRegOffsetAuxByID, BIT2, BIT2); // reg_en_aux
			msWriteByteMask(REG_DPRX_AUX_54_H + usRegOffsetAuxByID, 0, BIT5); // reg_pd_aux_rterm
			msWriteByteMask(REG_DPRX_AUX_DPCD_61_L + usRegOffsetAuxByID, 0, BIT7|BIT1); // reg_pd_vcm_op / reg_en_gpio / reg_en_ft_mux

            msWriteByteMask(REG_PM_48_H, (BIT4|BIT0) << dprx_aux_id, (BIT4|BIT0) << dprx_aux_id); // Enable AUX clock [REG_000391]
            mhal_DPRx_SetAuxClockGating(FALSE);

			msWriteByteMask(REG_DPRX_AUX_4D_L + usRegOffsetAuxByID, BIT1, BIT1); // MCCS reply by MCU
			msWriteByteMask(REG_DPRX_AUX_3E_H + usRegOffsetAuxByID, BIT2, BIT2); // Enable programmable DPCD 0

			msWriteByteMask(REG_DPRX_TRANS_CTRL_07_L + usRegOffsetTransCTRLByID, DP_RX_FT_TIMER_A_WAIT_SQ_SIGNAL, BIT0|BIT1|BIT2|BIT3); // Set fast training delay time to 1 sec
			break;

		case DP_ePM_STANDBY:
			msWriteByteMask(REG_DPRX_PLL_30_H, BIT0, BIT0); // reg_apll_pwdwn_0
			msWriteByteMask(REG_DPRX_PLL_50_H, BIT0, BIT0); // reg_vpll_pwdwn_0
			msWriteByteMask(REG_DPRX_AUX_54_L + usRegOffsetAuxByID, BIT2, BIT2); // reg_en_aux
			msWriteByteMask(REG_DPRX_AUX_54_H + usRegOffsetAuxByID, 0, BIT5); // reg_pd_aux_rterm
            msWriteByteMask(REG_DPRX_AUX_DPCD_61_L + usRegOffsetAuxByID, 0, BIT7|BIT1);

            msWriteByteMask(REG_PM_48_H, (BIT4|BIT0) << dprx_aux_id, (BIT4|BIT0) << dprx_aux_id); // Enable AUX clock [REG_000391]
            mhal_DPRx_SetAuxClockGating(FALSE);

			msWriteByteMask(REG_DPRX_AUX_4D_L + usRegOffsetAuxByID, 0, BIT1); // MCCS reply by HW
			msWriteByteMask(REG_DPRX_AUX_3E_H + usRegOffsetAuxByID, 0, BIT2); // Disable programmable DPCD 0

			ubHaveBeenSleeped[dprx_aux_id] = 1;
			break;

		case DP_ePM_POWEROFF:
			msWriteByteMask(REG_DPRX_PLL_30_H, BIT0, BIT0); // reg_apll_pwdwn_0
			msWriteByteMask(REG_DPRX_PLL_50_H, BIT0, BIT0); // reg_vpll_pwdwn_0
			msWriteByteMask(REG_DPRX_AUX_54_L + usRegOffsetAuxByID, 0, BIT2); // reg_en_aux
			msWriteByteMask(REG_DPRX_AUX_54_H + usRegOffsetAuxByID, BIT5, BIT5); // reg_pd_aux_rterm
            msWriteByteMask(REG_DPRX_AUX_DPCD_61_L + usRegOffsetAuxByID, BIT7, BIT7|BIT1);

            msWriteByteMask(REG_PM_48_H, 0, (BIT4|BIT0) << dprx_aux_id); // Enable AUX clock [REG_000391]

            if(msReadByte(REG_PM_48_H) == 0x0) //[REG_000391]
            {
                mhal_DPRx_SetAuxClockGating(TRUE);
            }

			msWriteByteMask(REG_DPRX_AUX_4D_L + usRegOffsetAuxByID, 0, BIT1); // MCCS reply by HW
			msWriteByteMask(REG_DPRX_AUX_3E_H + usRegOffsetAuxByID, 0, BIT2); // Disable programmable DPCD 0

			ubHaveBeenSleeped[dprx_aux_id] = 1;
			break;

		case DP_ePM_POWEROFF_NoUsedPort: // Only for unused AUX (ex: HDMI case)
			msWriteByteMask(REG_DPRX_AUX_54_L + usRegOffsetAuxByID, 0, BIT2); // reg_en_aux
			msWriteByteMask(REG_DPRX_AUX_54_H + usRegOffsetAuxByID, BIT5, BIT5); // reg_pd_aux_rterm
			msWriteByteMask(REG_DPRX_AUX_DPCD_61_L + usRegOffsetAuxByID, BIT7, BIT7|BIT1); // reg_pd_vcm_op / reg_en_gpio / reg_en_ft_mux

			ubHaveBeenSleeped[dprx_aux_id] = 1;
			break;

		default:
			break;
	}

	return;
}
#endif // _mhal_DPRx_C_

