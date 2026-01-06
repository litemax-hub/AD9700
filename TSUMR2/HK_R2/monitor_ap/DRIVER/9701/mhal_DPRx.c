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
#include "mailbox.h"

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

extern BYTE XDATA glDP_SQDebounce[DPRx_ID_MAX];
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
    DWORD msNums = 45303;

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
    mhal_DPRx_SetGPIOforAUX(dprx_aux_id, TRUE);
	mhal_DPRx_AuxInitialSetting(dprx_id, dprx_aux_id);
    mhal_DPRx_PHYInitialSetting(dprx_id, dprx_phy_id);
	mhal_DPRx_ReceiverInitialSetting(dprx_id);
	mhal_DPRx_HWFastTrainingEnable(dprx_id, FALSE);
	mhal_DPRx_SetFastTrainingTime(dprx_id, DP_RX_FT_TIMER_A_WAIT_SQ_SIGNAL);
	mhal_DPRx_FastTrainingInitialSetting(dprx_id);
	mhal_DPRx_HWFastTrainingEnable(dprx_id, TRUE);
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

	#if(FunctionMuxMode_HW == 0x1)
	mhal_DPRx_FunctionMuxMode(dprx_phy_id, TRUE);
	#else
	mhal_DPRx_FunctionMuxMode(dprx_phy_id, FALSE);
	#endif

	#if(DellMode_EN == 0x1)
	mhal_DPRx_DellMode_Enable(dprx_phy_id, TRUE);
	#else
	mhal_DPRx_DellMode_Enable(dprx_phy_id, FALSE);
	#endif

	mhal_DPRx_OUIInitialSetting(dprx_aux_id);

	mhal_DPRx_SetProgrammableDPCDEnable(dprx_aux_id, TRUE);

    #if (DPRX_HDCP2_ENABLE == 1)
    mhal_DPRx_SetXDATAProgrammableDPCDEnable(dprx_aux_id, DPRx_XDATAPROGRAM_DPCD_0, TRUE, FALSE);
    #else
    mhal_DPRx_SetXDATAProgrammableDPCDEnable(dprx_aux_id, DPRx_XDATAPROGRAM_DPCD_0, TRUE, TRUE);
    #endif

    #if (DP_XDATA_PROGRAMMABLE_DPCD_MCCS == 1)
    mhal_DPRx_SetXDATAProgrammableDPCDEnable(dprx_aux_id, DPRx_XDATAPROGRAM_DPCD_1, FALSE, FALSE);
    #else
    mhal_DPRx_SetXDATAProgrammableDPCDEnable(dprx_aux_id, DPRx_XDATAPROGRAM_DPCD_1, TRUE, TRUE);
    #endif
    mhal_DPRx_EnableAuxInterrupt(dprx_aux_id, TRUE);
    mhal_DPRx_EnableDPAUXRecvInterrupt(dprx_aux_id, FALSE);
	mhal_DPRx_EnablePHYInterrupt(dprx_phy_id, FALSE);
	mhal_DPRx_EnableDPCD103Interrupt(dprx_id, FALSE);
	mhal_DPRx_EnableDPCD202Interrupt(dprx_id, FALSE); // For debug

	mhal_DPRx_SetAuxIsel(dprx_aux_id);

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
	WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    WORD usRegOffsetAuxPM1ByID = DP_REG_OFFSET_AUX_PM1(dprx_aux_id);
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);
    WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET_DPCD0(dprx_id);
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET_DPCD1(dprx_id);
    DWORD XDATA ulTimeOutCount = 0;

	msWriteByteMask(REG_DPRX_AUX_PM0_7E_L + usRegOffsetAuxPM0ByID, BIT3, BIT3); // Offline Reply setting
    msWriteByteMask(REG_DPRX_AUX_PM0_7E_H + usRegOffsetAuxPM0ByID, BIT3, BIT3); // Power saving usage
    msWriteByteMask(REG_DPRX_AUX_PM2_7E_L + usRegOffsetAuxPM2ByID, 0x0F, 0x0F); // [2] reg_en_aux // [6] reg_gpio_auxn_oen // [13] reg_pd_aux_rterm
    msWriteByteMask(REG_DPRX_AUX_PM0_54_L + usRegOffsetAuxPM0ByID, BIT6|BIT2, BIT6|BIT2); // Enable AUX

    msWriteByteMask(REG_DPRX_AUX_PM0_54_H + usRegOffsetAuxPM0ByID, 0, BIT5); // Disable power down AUX R-term
    msWriteByteMask(REG_DPRX_AUX_PM2_04_L + usRegOffsetAuxPM2ByID, BIT7|BIT5|BIT4, BIT7|BIT6|BIT5|BIT4); // Xtal frequency

    // DP version setting
    msWriteByteMask(REG_DPRX_AUX_PM2_64_H + usRegOffsetAuxPM2ByID, BIT7, BIT7); // DP1.4 DPCD enable
    msWriteByteMask(REG_DPRX_DPCD1_60_L + usRegOffsetDPCD1ByID, BIT7, BIT7); // DP1.4 non-PM DPCD enable

	// Illegal I2C address NACK reply, 0: disable -> over 400us, 1: enable -> under 400us
	msWriteByteMask(REG_DPRX_AUX_PM0_7D_H + usRegOffsetAuxPM0ByID, BIT2, BIT2);
    msWriteByteMask(REG_DPRX_AUX_PM0_7D_H + usRegOffsetAuxPM0ByID, BIT0, BIT0); // Illegal address reply NACK
    msWriteByteMask(REG_DPRX_AUX_PM0_18_H + usRegOffsetAuxPM0ByID, BIT1, BIT1|BIT0); // AUX time out reply DEFER, 0: ACK, 1: NACK, 2: DEFER

    #if FPGA_Verification
    msWrite2Byte(REG_DPRX_AUX_PM0_0A_L + usRegOffsetAuxPM0ByID, 0x1FFF); // AUX time out threshold
    #else
    msWrite2Byte(REG_DPRX_AUX_PM0_0A_L + usRegOffsetAuxPM0ByID, 0xBB8); // AUX time out threshold
    #endif
    msWriteByteMask(REG_DPRX_AUX_PM0_7A_L + usRegOffsetAuxPM0ByID, BIT4, BIT4); // AUX reply timing mode select, 0: old mode (60 ~ 70us), 1: new mode (60 ~ 300us)

    mhal_DPRx_SetAuxDelayReply(dprx_aux_id, DP_AUX_DELAY_NORMAL);  // 53us delay reply

    #if (ENABLE_DP_AUDIO_DPCD == 0x1)
    msWriteByteMask(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID, BIT3, BIT3); // DPCD 0004[0] NORP = 1 --> support video & audio
    msWriteByteMask(REG_DPRX_AUX_PM0_05_L + usRegOffsetAuxPM0ByID, BIT2, BIT2); // DPCD 000A[2] ASSOCIATED_TO_PRECEDING_PORT of RECEIVE_PORT1_CAP_0 = 1
    #else
    msWriteByteMask(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID, 0, BIT3); // DPCD 0004[0] NORP = 0 --> only support video
    msWriteByteMask(REG_DPRX_AUX_PM0_05_L + usRegOffsetAuxPM0ByID, 0, BIT2); // DPCD 000A[2] ASSOCIATED_TO_PRECEDING_PORT of RECEIVE_PORT1_CAP_0 = 0
    #endif

    msWriteByteMask(REG_DPRX_AUX_PM0_01_L + usRegOffsetAuxPM0ByID, 0, BIT5);  // DPCD 0000D
    msWriteByteMask(REG_DPRX_AUX_PM0_21_L + usRegOffsetAuxPM0ByID, BIT0, BIT0); // DPCD 30 ~ 3F GUID data write enable
    msWriteByteMask(REG_DPRX_DPCD0_3F_L + usRegOffsetDPCD0ByID, DPRX_SINK_COUNT_DEFAULT, 0x7F); // DPCD 200 // Sink count
    msWriteByteMask(REG_DPRX_DPCD0_3F_L + usRegOffsetDPCD0ByID, BIT7, BIT7); // DPCD 200 // CP ready
    msWriteByteMask(REG_DPRX_DPCD1_7C_L + usRegOffsetDPCD1ByID, 0, BIT0); // For DPCD 40F ~ 4FF reply
    msWriteByteMask(REG_DPRX_AUX_PM2_03_L + usRegOffsetAuxPM2ByID, 0, BIT7|BIT6); // DPCD 2210[5:4] HDR support
    msWriteByte(REG_DPRX_AUX_PM0_51_L + usRegOffsetAuxPM0ByID, 0); // Disable DPCD 2E (only for eDP used)
    msWriteByteMask(REG_DPRX_DPCD0_3A_L + usRegOffsetDPCD0ByID, 0, BIT1); // Disable DPCD 700 ~ 733 (only for eDP used)

    msWriteByteMask(REG_DPRX_AUX_PM2_31_H + usRegOffsetAuxPM2ByID, 0, BIT0); // Disable DPCD 0000 ~ 0011 and 2200 ~ 2211 sync

	msWriteByteMask(REG_DPRX_AUX_PM2_21_L + usRegOffsetAuxPM2ByID, BIT2, BIT2); // PM HDCP Bcaps
    msWriteByteMask(REG_DPRX_DPCD1_48_L + usRegOffsetDPCD1ByID, BIT1, BIT1); // HDCP Bcaps

	msWriteByteMask(REG_DPRX_AUX_PM0_7D_L + usRegOffsetAuxPM0ByID, 0, BIT2); //reg_aux_rx_oen_set_ov_mode

	// PD_VCM_OP
	msWriteByteMask(REG_DPRX_AUX_PM2_61_L + usRegOffsetAuxPM2ByID, 0, BIT7);
    msWriteByteMask(REG_DPRX_AUX_PM0_2A_L + usRegOffsetAuxPM0ByID, 0, BIT0);
    msWriteByteMask(REG_DPRX_AUX_PM2_60_H + usRegOffsetAuxPM2ByID, BIT1, BIT1);

    // For Odinson2 fake train SW2pre0 will better than SW2pre1
    mhal_DPRx_OfflinePortLinkTrainingSetting(dprx_aux_id, Faketrain_swing, Faketrain_pre);

    // Patch for EDID read error HW bug
    msWrite2Byte(REG_DPRX_AUX_PM1_00_L + usRegOffsetAuxPM1ByID, 0x3838);
    msWrite2Byte(REG_DPRX_AUX_PM1_01_L + usRegOffsetAuxPM1ByID, 0x3838);
    msWrite2Byte(REG_DPRX_AUX_PM1_02_L + usRegOffsetAuxPM1ByID, 0x3838);
    msWrite2Byte(REG_DPRX_AUX_PM1_03_L + usRegOffsetAuxPM1ByID, 0x3838);
    msWrite2Byte(REG_DPRX_AUX_PM1_04_L + usRegOffsetAuxPM1ByID, 0x3838);
    msWriteByteMask(REG_DPRX_AUX_PM3_33_L + usRegOffsetAuxPM3ByID, 0, BIT7);
    msWriteByteMask(REG_DPRX_AUX_PM0_3E_L + usRegOffsetAuxPM0ByID, BIT7, BIT7);

    mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00090, 0x00, DPRx_RIU_DPCD_Auto); // Initial disable FEC capable

    // Fake training setting
    msWriteByteMask(REG_DPRX_AUX_PM3_7A_H + usRegOffsetAuxPM3ByID, BIT1, BIT1); // Enable fake training valid mode (guarantee request sw2/pre1)

    // DPCD 0x61 reg default value = 0x12, we hope init as 0 because DSC maybe not be enabled.
    mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00061, 0, DPRx_RIU_DPCD_Auto);
    glubMCCS_Wakeup[dprx_aux_id] = DPRx_MCCS_MAX;

    // Online reset async FIFO
	while(ulTimeOutCount < DP_AUX_TIMEOUT_CNT)
	{
		ulTimeOutCount++;

		if(mhal_DPRx_CheckAuxIdle(dprx_aux_id) == TRUE) // Aux idle
		{
			break;
		}
	}

    mhal_DPRx_ResetAsyncFIFO(dprx_id, dprx_aux_id); // ResetAsyncFIFO need after clock gating

	mhal_DPRx_SetOffLine(dprx_id, dprx_aux_id, TRUE);

    #if FPGA_Verification // 5.2.1.8 for FPGA stage
    msWriteByteMask(REG_DPRX_AUX_PM0_19_L + usRegOffsetAuxPM0ByID, BIT7, BIT7);
    #endif

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
    WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET_TRANS_CTRL(dprx_id);
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);
    WORD usRegOffsetReceiverFECByID = DP_REG_OFFSET_RECEIVER_FEC(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_TRANS_CTRL_06_L + usRegOffsetTransCTRLByID, 0, BIT1); //monitor case need to disable overwrite SQH from PHY due to default setting for Tcon is enable
	msWriteByteMask(REG_DPRX_TRANS_CTRL_30_H + usRegOffsetTransCTRLByID, BIT3, BIT3); //reset auto eq when FLT link rate chnage
	msWriteByteMask(REG_DPRX_TRANS_CTRL_04_H + usRegOffsetTransCTRLByID, BIT3|BIT1|BIT0, 0x1F); //Leger3 is 12MHZ: set X-1 =11
	msWrite2Byte(REG_DPRX_TRANS_CTRL_25_L + usRegOffsetTransCTRLByID, 0); // sqh/dlev refer in NLT CR stage, 0:lane count, 1:only lane 0

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

    msWriteByteMask(REG_DPRX_RECEIVER_34_L + usRegOffsetReceiverByID, 0, BIT0); // reg_train_ctrl_sel

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
    WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET_TRANS_CTRL(dprx_id);

    if(dprx_id == DPRx_ID_MAX)
    {
        return;
    }

    msWriteByteMask(REG_DPRX_TRANS_CTRL_07_H + usRegOffsetTransCTRLByID, 0, BIT3); // Bypass timer_a

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

    msWrite2Byte(REG_DPRX2_TOP_GP_2C_L, 0xFFFF); // reg_dp_clkgen_en_ext
    msWrite2Byte(REG_DPRX2_TOP_GP_2D_L, 0xFFFF); // reg_dp_clkgen_en_ext
    msWrite2Byte(REG_DPRX2_TOP_GP_2E_L, 0xFFFF); // reg_dp_clkgen_en_ext

    msWrite2Byte(REG_DPRX2_TOP_GP_3C_L, 0xFFFF); // reg_dp_clkicg_en
    msWrite2Byte(REG_DPRX2_TOP_GP_3D_L, 0xFFFF); // reg_dp_clkicg_en

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
    switch(dprx_decoder_id)
    {
        case DPRx_DECODER_ID_0:
            #if (FPGA_Verification == 0)
            //VPLL setting from VPLL APN
            // [4:0] reg_pll_clkgen_en_vpll
            msWrite2Byte(REG_DPRX_PLL_TOP0_30_L, 0x001F);

            //  [0] reg_pd_vpll
            //  [1] reg_pd_vpll_kf0
            //  [2] reg_pd_vpll_kp0
            //  [3] reg_pd_vpll_kp1
            //  [4] reg_pd_vpll_kpdiv
            //  [8] reg_en_vpll_rst
            //  [9] reg_en_vpll_test
            // [10] reg_en_vpll_xtal
            // [11] reg_en_vpll_prdt
            // [12] reg_en_vpll_ov_cp_sw
            msWrite2ByteMask(REG_DPRX_PLL_TOP0_32_L, 0, BIT0|BIT3);

            //   [7:0] reg_gcr_vpll_loopdiv_second
            //   [9:8] reg_gcr_vpll_loopdiv_first
            // [14:12] reg_gcr_vpll_output_div
            msWrite2Byte(REG_DPRX_PLL_TOP0_34_L, 0x3104);

            // [5] reg_dprx_vpll_en
            msWriteByteMask(REG_DPRX_PLL_TOP0_38_L, BIT5, BIT5);
            // [10:8] reg_vpll_m_shift
            msWriteByteMask(REG_DPRX_PLL_TOP0_38_H, BIT1, BIT1);
            #endif

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
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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

    msWriteByteMask(REG_DPRX_DECODER_E0_28_H + usRegOffsetDecoderByID, BIT1|BIT7, BIT1|BIT7); // Enable adjust the time of MN update when receive MSA

    msWriteByteMask(REG_DPRX_DECODER_E0_2B_H + usRegOffsetDecoderByID, 0, BIT4|BIT3); // [12] reg_vbid6_fall_chk_vbid0 // [11] reg_vbid6_rise_chk_vbid0

    msWriteByte(REG_DPRX_DECODER_E0_0A_H + usRegOffsetDecoderByID, 6); // reg_v_stable_cnt

    msWriteByteMask(REG_DPRX_DECODER_E0_40_L + usRegOffsetDecoderByID, 4, BIT0|BIT1|BIT2); // reg_fr_th

    msWriteByteMask(REG_DPRX_DECODER_E0_61_L + usRegOffsetDecoderByID, BIT1, BIT1); // Fix received HDE < MSA issue

	msWriteByteMask(REG_DPRX_DECODER_E0_42_L + usRegOffsetDecoderByID, BIT0|BIT4|BIT5, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6); // For HTT learn

	msWriteByteMask(REG_DPRX_DECODER_E0_79_L + usRegOffsetDecoderByID, BIT1, BIT1); // Enable to assert the last Hsync in V-blanking for freesync mode

	msWriteByteMask(REG_DPRX_DECODER_E0_01_H + usRegOffsetDecoderByID, BIT1|BIT2, BIT1|BIT2); // [9] reg_rst_dec_no_stream // [10] reg_sup_se_replaced_by_be_sst_mode

    // Mengkun suggest set 0 because this new mode cause video output is not gated when DP receive no video stream flag in VB_ID //sync 9u6
    msWriteByteMask(REG_DPRX_DECODER_E0_6F_H + usRegOffsetDecoderByID, 0, BIT2); //reg_video_mute_latch_new_mode , only latch vbid[3] in vblanking

    // For FPGA verification
    msWriteByteMask(REG_DPRX_DECODER_E0_59_L + usRegOffsetDecoderByID, BIT6, BIT5|BIT6); // 4P -> 1P

    #if(ASIC_Verification == 0x1)
    // For real CHIP, decoder 00_H should be set
    msWriteByteMask(REG_DPRX_DECODER_E0_00_H + usRegOffsetDecoderByID, 0, BIT0|BIT1); // 1P mode
    #endif

    msWriteByteMask(REG_DPRX_DECODER_E0_0A_L + usRegOffsetDecoderByID, BIT2, BIT2); // M value determined by MSA M[23:8] & VBID M[7:0]

    msWriteByteMask(REG_DPRX_DECODER_E0_67_L + usRegOffsetDecoderByID, BIT1|BIT0, BIT1|BIT0); // Enable HSP/HSW/VSP/VSW overwrite

    msWriteByteMask(REG_DPRX_DECODER_E0_5E_H + usRegOffsetDecoderByID, BIT4|BIT5|BIT6|BIT7, BIT4|BIT5|BIT6|BIT7); // mute info for XC, CR done, stream enable, vblking, vbid[3]

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
void mhal_DPRx_VersionSetting(DPRx_AUX_ID dprx_aux_id, BYTE ucVersion)
{
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
	WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);
    WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET_DPCD1(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (ucVersion == DP_VERSION_MAX))
	{
		return;
	}

    // Initial DPCD 2200~2210
	msWriteByteMask(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID, 0, BIT1); // DPCD 0002[6] TPS3_SUPPORTED
	msWriteByteMask(REG_DPRX_AUX_PM0_7E_H + usRegOffsetAuxPM0ByID, 0, BIT2); // DPCD 0003[7] TPS4_SUPPORTED
	msWriteByteMask(REG_DPRX_AUX_PM0_02_H + usRegOffsetAuxPM0ByID, 0, BIT7); // DPCD 000E[7] EXTENDED_RECEIVER_CAPABILITY_FIELD_PRESENT
	msWriteByteMask(REG_DPRX_AUX_PM0_02_H + usRegOffsetAuxPM0ByID, BIT0, BIT0); // DPCD 000E[6:0] TRAINING_AUX_RD_INTERVAL
	msWriteByteMask(REG_DPRX_AUX_PM0_7E_L + usRegOffsetAuxPM0ByID, 0, BIT7); // DPCD 2210[3] VSC_SDP_EXTENSION_FOR_COLORIMETRY_SUPPORTED
	msWriteByteMask(REG_DPRX_AUX_PM2_72_L + usRegOffsetAuxPM2ByID, 0, BIT7); // DPCD 2203[7] TPS4_SUPPORTED

    // Initial set DPCD 2200 ~ 2210 to 0
	msWriteByteMask(REG_DPRX_AUX_PM0_02_H + usRegOffsetAuxPM2ByID, 0, BIT7); // DPCD 000E[7] EXTENDED_RECEIVER_CAPABILITY_FIELD_PRESENT
	mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02200, 0x00, DPRx_RIU_DPCD_Auto);
	mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02201, 0x00, DPRx_RIU_DPCD_Auto);
	mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02202, 0x00, DPRx_RIU_DPCD_Auto);
	mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02203, 0x00, DPRx_RIU_DPCD_Auto);
	mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02204, 0x00, DPRx_RIU_DPCD_Auto);
	mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02206, 0x00, DPRx_RIU_DPCD_Auto);
	mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02207, 0x00, DPRx_RIU_DPCD_Auto);
	mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02208, 0x00, DPRx_RIU_DPCD_Auto);
	mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02209, 0x00, DPRx_RIU_DPCD_Auto);
	mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_0220A, 0x00, DPRx_RIU_DPCD_Auto);
	mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_0220B, 0x00, DPRx_RIU_DPCD_Auto);
	mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_0220C, 0x00, DPRx_RIU_DPCD_Auto);
	mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_0220D, 0x00, DPRx_RIU_DPCD_Auto);
	mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_0220E, 0x00, DPRx_RIU_DPCD_Auto);
	mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_0220F, 0x00, DPRx_RIU_DPCD_Auto);
	mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02210, 0x00, DPRx_RIU_DPCD_Auto);
    mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02216, 0x00, DPRx_RIU_DPCD_Auto);
    mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00010, 0x00, DPRx_RIU_DPCD_Auto);
    mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00011, 0x00, DPRx_RIU_DPCD_Auto);
    mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00012, 0x00, DPRx_RIU_DPCD_Auto);
    mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00013, 0x00, DPRx_RIU_DPCD_Auto);
    mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00014, 0x00, DPRx_RIU_DPCD_Auto);
    mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00015, 0x00, DPRx_RIU_DPCD_Auto);
    mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00016, 0x00, DPRx_RIU_DPCD_Auto);
    mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00017, 0x00, DPRx_RIU_DPCD_Auto);
    mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00018, 0x00, DPRx_RIU_DPCD_Auto);
    mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00019, 0x00, DPRx_RIU_DPCD_Auto);
    mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_0001A, 0x00, DPRx_RIU_DPCD_Auto);
    mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_0001B, 0x00, DPRx_RIU_DPCD_Auto);
    mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_0001C, 0x00, DPRx_RIU_DPCD_Auto);
    mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_0001D, 0x00, DPRx_RIU_DPCD_Auto);
    mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_0001E, 0x00, DPRx_RIU_DPCD_Auto);
    mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_0001F, 0x00, DPRx_RIU_DPCD_Auto);
    mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00700, 0x00, DPRx_RIU_DPCD_Auto);

	if(ucVersion == DP_VERSION_11)
	{
		printf("========== DP 1.1 ==========\r\n");

		//msWriteByte(REG_DPRX_AUX_PM0_50_L + usRegOffsetAuxByID, DP_VERSION_11); // DPCD version for reading // Enable for IC verification of DP version related items

        msWriteByteMask(REG_DPRX_AUX_PM0_04_L + usRegOffsetAuxPM0ByID, 0, BIT6); // DPCD 0006[1] MAIN_LINK_CHANNEL_CODING_CAP - 128b/132b supported, sync with DPCD2206

        mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00000, DP_VERSION_11, DPRx_RIU_DPCD_Auto); // DPCD 0000[3:0] Minor Revision Number
		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00001, DP_LINKRATE_HBR, DPRx_RIU_DPCD_Auto); // DPCD 0001[7:0] MAX_LINK_RATE
        mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02205, 0x00, DPRx_RIU_DPCD_Auto);

		// DPCD version for non-pm HW, for DP1.1 PHY auto test issue, error count report would be affect by this setting
		// We think default set MAX version would be fine and this setting is MP for many DP1.2/1.4 Monitors
		msWriteByte(REG_DPRX_DPCD1_09_L + usRegOffsetDPCD1ByID, DP_VERSION_11);
	}
	else if(ucVersion == DP_VERSION_12)
	{
		printf("========== DP 1.2 ==========\r\n");

        //msWriteByte(REG_DPRX_AUX_PM0_50_L + usRegOffsetAuxByID, DP_VERSION_12); // DPCD version for reading // Enable for IC verification of DP version related items

		msWriteByteMask(REG_DPRX_AUX_PM0_04_L + usRegOffsetAuxPM0ByID, 0, BIT6); // DPCD 0006[1] MAIN_LINK_CHANNEL_CODING_CAP - 128b/132b supported, sync with DPCD2206

        mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00000, DP_VERSION_12, DPRx_RIU_DPCD_Auto); // DPCD 0000[3:0] Minor Revision Number
		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00001, DP_LINKRATE_HBR2, DPRx_RIU_DPCD_Auto); // DPCD 0001[7:0] MAX_LINK_RATE

		#if (DPRX_SUPPORT_TP3 == 0x1)
		msWriteByteMask(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID, BIT1, BIT1); // DPCD 0002[6] TPS3_SUPPORTED
		msWriteByteMask(REG_DPRX_AUX_PM2_71_H + usRegOffsetAuxPM2ByID, DP_LANECOUNT_4, 0x1F); // DPCD 2202[4:0] MAX_LANE_COUNT
		msWriteByteMask(REG_DPRX_AUX_PM2_71_H + usRegOffsetAuxPM2ByID, BIT7|BIT6, BIT7|BIT6); // DPCD 2202[6] TPS3_SUPPORTED
		#else
		msWriteByteMask(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID, 0, BIT1); // DPCD 0002[6] TPS3_SUPPORTED
		msWriteByteMask(REG_DPRX_AUX_PM2_71_H + usRegOffsetAuxPM2ByID, DP_LANECOUNT_4, 0x1F); // DPCD 2202[4:0] MAX_LANE_COUNT
		msWriteByteMask(REG_DPRX_AUX_PM2_71_H + usRegOffsetAuxPM2ByID, BIT7, BIT7|BIT6); // DPCD 2202[6] TPS3_SUPPORTED
		#endif

		// DPCD version for non-pm HW, for DP1.1 PHY auto test issue, error count report would be affect by this setting
		// We think default set MAX version would be fine and this setting is MP for many DP1.2/1.4 Monitors
		msWriteByte(REG_DPRX_DPCD1_09_L + usRegOffsetDPCD1ByID, DP_VERSION_12);
        mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02205, 0x00, DPRx_RIU_DPCD_Auto);
	}
	else if((ucVersion == DP_VERSION_12_14) || (ucVersion == DP_VERSION_14))
	{
		printf("========== DP_VERSION_14 ==========\r\n");

        //msWriteByte(REG_DPRX_AUX_PM0_50_L + usRegOffsetAuxByID, DP_VERSION_14); // DPCD version for reading // Enable for IC verification of DP version related items

		msWriteByteMask(REG_DPRX_AUX_PM0_04_L + usRegOffsetAuxPM0ByID, 0, BIT6); // DPCD 0006[1] MAIN_LINK_CHANNEL_CODING_CAP - 128b/132b supported, sync with DPCD2206

		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00000, DP_VERSION_12, DPRx_RIU_DPCD_Auto); // DPCD 0000[3:0] Minor Revision Number
		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02200, DP_VERSION_14, DPRx_RIU_DPCD_Auto); // DPCD minor revision number
		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00001, DP_LINKRATE_HBR2, DPRx_RIU_DPCD_Auto); // DPCD 0001[7:0] MAX_LINK_RATE
		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02201, DP_LINKRATE_HBR2, DPRx_RIU_DPCD_Auto); // DPCD 2201[7:0] MAX_LINK_RATE

		#if (DPRX_SUPPORT_TP3 == 0x1)
		msWriteByteMask(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID, BIT1, BIT1); // DPCD 0002[6] TPS3_SUPPORTED
		msWriteByteMask(REG_DPRX_AUX_PM2_71_H + usRegOffsetAuxPM2ByID, DP_LANECOUNT_4, 0x1F); // DPCD 2202[4:0] MAX_LANE_COUNT
		msWriteByteMask(REG_DPRX_AUX_PM2_71_H + usRegOffsetAuxPM2ByID, BIT7|BIT6, BIT7|BIT6); // DPCD 2202[6] TPS3_SUPPORTED
		#else
		msWriteByteMask(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID, 0, BIT1); // DPCD 0002[6] TPS3_SUPPORTED
		msWriteByteMask(REG_DPRX_AUX_PM2_71_H + usRegOffsetAuxPM2ByID, DP_LANECOUNT_4, 0x1F); // DPCD 2202[4:0] MAX_LANE_COUNT
		msWriteByteMask(REG_DPRX_AUX_PM2_71_H + usRegOffsetAuxPM2ByID, BIT7, BIT7|BIT6); // DPCD 2202[6] TPS3_SUPPORTED
		#endif

		#if (DPRX_SUPPORT_TP4 == 0x1)
		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00003, 0x81, DPRx_RIU_DPCD_Auto); // DPCD 0003[7] TPS4_SUPPORTED // DPCD 0003[0] MAX_DOWNSPREAD
		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02203, 0x81, DPRx_RIU_DPCD_Auto); // DPCD 2203[7] TPS4_SUPPORTED // DPCD 2203[0] MAX_DOWNSPREAD
		#else
		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00003, 0x01, DPRx_RIU_DPCD_Auto); // DPCD 0003[7] TPS4_SUPPORTED // DPCD 0003[0] MAX_DOWNSPREAD
		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02203, 0x01, DPRx_RIU_DPCD_Auto); // DPCD 2203[7] TPS4_SUPPORTED // DPCD 2203[0] MAX_DOWNSPREAD
		#endif

        if(msReadByte(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID) & BIT6) // DPCD 0007[6] MSA_TIMING_PAR_IGNORED
        {
    		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00007, 0xC0, DPRx_RIU_DPCD_Auto); // DPCD 0007[7] OUI Support // DPCD 0007[3:0] DFP_COUNT
    		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02207, 0xC0, DPRx_RIU_DPCD_Auto); // DPCD 2207[7] OUI Support // DPCD 2207[3:0] DFP_COUNT
        }
        else
        {
    		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00007, 0x80, DPRx_RIU_DPCD_Auto); // DPCD 0007[7] OUI Support // DPCD 0007[3:0] DFP_COUNT
    		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02207, 0x80, DPRx_RIU_DPCD_Auto); // DPCD 2207[7] OUI Support // DPCD 2207[3:0] DFP_COUNT
        }

		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02204, 0x01, DPRx_RIU_DPCD_Auto); // DPCD 2204[0] Number of Receiver Ports, 0: 1 receiver port, 1: two or more receiver ports
		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02206, 0x01, DPRx_RIU_DPCD_Auto); // DPCD 2206[1] 128b/132b_SUPPORTED // DPCD 2206[0] 8b/10b_SUPPORTED
		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02208, 0x02, DPRx_RIU_DPCD_Auto); // DPCD 2208[1] LOCAL_EDID_PRESENT for RECEIVE_PORT0_CAP_0
		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_00009, 0x02, DPRx_RIU_DPCD_Auto); // DPCD 0009[7:0] BUFFER_SIZE for RECEIVE_PORT0_CAP_1
		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02209, 0x02, DPRx_RIU_DPCD_Auto); // DPCD 2209[7:0] BUFFER_SIZE for RECEIVE_PORT0_CAP_1
		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_0220A, 0x06, DPRx_RIU_DPCD_Auto); // DPCD 220A[2] ASSOCIATED_TO_PRECEDING_PORT // DPCD 220A[1] LOCAL_EDID_PRESENT for RECEIVE_PORT1_CAP_0
		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_0220B, 0x00, DPRx_RIU_DPCD_Auto); // DPCD 220B[7:0] BUFFER_SIZE for RECEIVE_PORT1_CAP_1
		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_0220C, 0x00, DPRx_RIU_DPCD_Auto); // DPCD 220C I2C Speed Control Capabilities Bit Map
		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_0220D, 0x00, DPRx_RIU_DPCD_Auto); // DPCD 220D eDP_CONFIGURATION_CAP
		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_0220F, 0x00, DPRx_RIU_DPCD_Auto); // DPCD 220F ADAPTER_CAP
		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_02211, 0x00, DPRx_RIU_DPCD_Auto); // DPCD 220F EXTENDED_DPRX_SLEEP_WAKE_TIMEOUT_REQUEST

		#if (DPRX_SUPPORT_DPCD2200 == 0x1)
		msWriteByteMask(REG_DPRX_AUX_PM0_02_H + usRegOffsetAuxPM0ByID, BIT7, BIT7); // DPCD 000E[7] EXTENDED_RECEIVER_CAPABILITY_FIELD_PRESENT
		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_0220E, 0x81, DPRx_RIU_DPCD_Auto); // DPCD 220E[7] EXTENDED_RECEIVER_CAPABILITY_FIELD_PRESENT // DPCD 220E[6:0] TRAINING_AUX_RD_INTERVAL
		#else
		msWriteByteMask(REG_DPRX_AUX_PM0_02_H + usRegOffsetAuxPM0ByID, 0, BIT7); // DPCD 000E[7] EXTENDED_RECEIVER_CAPABILITY_FIELD_PRESENT
		mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, DPCD_0220E, 0x01, DPRx_RIU_DPCD_Auto); // DPCD 220E[7] EXTENDED_RECEIVER_CAPABILITY_FIELD_PRESENT // DPCD 220E[6:0] TRAINING_AUX_RD_INTERVAL
		#endif

		#if (DPRX_SUPPORT_VSCSDP == 0x1)
		msWriteByteMask(REG_DPRX_AUX_PM0_7E_L + usRegOffsetAuxPM0ByID, BIT7, BIT7); // DPCD 2210[3] VSC_SDP_EXTENSION_FOR_COLORIMETRY_SUPPORTED
		mhal_DPRx_SetDPCDValueByRIU_Mask(dprx_aux_id, DPCD_02210, BIT3, BIT3, DPRx_RIU_DPCD_Auto); // DPCD 2210[3] VSC_SDP_EXTENSION_FOR_COLORIMETRY_SUPPORTED
		#else
		msWriteByteMask(REG_DPRX_AUX_PM0_7E_L + usRegOffsetAuxPM0ByID, 0, BIT7); // DPCD 2210[3] VSC_SDP_EXTENSION_FOR_COLORIMETRY_SUPPORTED
		mhal_DPRx_SetDPCDValueByRIU_Mask(dprx_aux_id, DPCD_02210, 0, BIT3, DPRx_RIU_DPCD_Auto); // DPCD 2210[3] VSC_SDP_EXTENSION_FOR_COLORIMETRY_SUPPORTED
		#endif

		// DPCD version for non-pm HW, for DP1.1 PHY auto test issue, error count report would be affect by this setting
		// We think default set MAX version would be fine and this setting is MP for many DP1.2/1.4 Monitors
		msWriteByte(REG_DPRX_DPCD1_09_L + usRegOffsetDPCD1ByID, DP_VERSION_14);
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

	bEnable = FALSE;

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
				return hwGNDDP0_Pin;
			break;

		case DPRx_AUX_ID_1:
				return hwGNDDP1_Pin;
			break;

		case DPRx_AUX_ID_2:
				return hwGNDDP2_Pin;
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
			return hwSDM_AuxN_DP0Pin_Pin;
			break;

		case DPRx_AUX_ID_1:
			return hwSDM_AuxN_DP1Pin_Pin;
			break;

		case DPRx_AUX_ID_2:
			return hwSDM_AuxN_DP2Pin_Pin;
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
//                  mhal_DPRx_ReadInternalEDID()
//  [Description]
//					mhal_DPRx_ReadInternalEDID
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mhal_DPRx_ReadInternalEDID(BYTE u8Block, BYTE u8Addr)
{
	WORD u16TimeOut = 0xFFFF;
    BYTE u8Value = 0;

    msWriteByteMask(REG_PM_DDC_75_H, u8Block, 0x3F); // Select SRAM base address for CPU read/write
    msWriteByte(REG_PM_DDC_25_H, u8Addr);			 // DDC address port for CPU read/write
    msWriteByteMask(REG_PM_DDC_21_H, BIT4, BIT4);	 // ADC SRAM read data pulse gen when CPU read

    for(u8Value = 0; u8Value < 10; u8Value++) // Need wait 2T to update data
	{
		_nop_();
	}

    while((msReadByte(REG_PM_DDC_3D_H) & BIT4) && (--u16TimeOut));

    u8Value = msReadByte(REG_PM_DDC_07_L); // DDC data port for CPU read

    return u8Value;
}

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
	msWriteByteMask(REG_PM_DDC_7C_L, 0x0, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5); // reg_edid_sram_base_adr_sel_dp0
	msWriteByteMask(REG_PM_DDC_7C_H, 0x4, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5); // reg_edid_sram_base_adr_sel_dp1
	msWriteByteMask(REG_PM_DDC_7D_L, 0x8, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5); // reg_edid_sram_base_adr_sel_dp2

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
    WORD j = 0;
    WORD uwEDIDSize = 0;
    BYTE ubEDID[DP_EDID_MAX_SIZE] = {0};
	BYTE ubBlockOffset = 0x0;
	WORD uwEDIDTimeOut = 0x0;
    DWORD ulTimeout = 400; // ~ 550u

    if((dprx_id == DPRx_ID_MAX) || (dprx_aux_id == DPRx_AUX_ID_MAX) || (EdidData == NULL))
    {
        return;
    }

    uwEDIDSize = (EdidData[126] + 1) * 128;

    if((uwEDIDSize < 128) || (uwEDIDSize > DP_EDID_MAX_SIZE))
    {
        return;
    }

	memcpy(ubEDID, EdidData, uwEDIDSize);

	// DP0 --> block 0x0 ~ 0x3
	// DP1 --> block 0x4 ~ 0x7
	// DP2 --> block 0x8 ~ 0xB
	ubBlockOffset = 0x0 + (dprx_aux_id << 2);

    for(j = 0; j < uwEDIDSize; j+= 128)
    {
        msWriteByte(REG_PM_DDC_75_H, (ubBlockOffset + (j/128))); // Select SRAM base address for CPU read/write

        for(i = 0; i < 128; i++)
    	{
    		ucValue = ubEDID[i+j];
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

			mhal_DPRx_DELAY_NOP(10);
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
    WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET_DPCD0(dprx_id);
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if((dprx_id == DPRx_ID_MAX) || (dprx_aux_id == DPRx_AUX_ID_MAX))
	{
		return;
	}

	msWriteByteMask(REG_DPRX_DPCD1_09_L + usRegOffsetDPCD0ByID, ucValue, BIT3|BIT2|BIT1|BIT0); // reg_minor_rev
	msWriteByte(REG_DPRX_AUX_PM0_50_L + usRegOffsetAuxPM0ByID, ucDPVersion); // reg_dpcd_ver_inside

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_DPCDVersion_Get()
//  [Description]
//					mhal_DPRx_DPCDVersion_Get
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE mhal_DPRx_DPCDVersion_Get(DPRx_AUX_ID dprx_aux_id)
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
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

    if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    msWriteByteMask(REG_DPRX_AUX_PM2_02_H + usRegOffsetAuxPM2ByID, BIT1, BIT1);
    msWriteByte(REG_DPRX_AUX_PM2_00_L + usRegOffsetAuxPM2ByID, ulDPCDAddress & 0xFF);
    msWriteByte(REG_DPRX_AUX_PM2_00_H + usRegOffsetAuxPM2ByID, (ulDPCDAddress >> 0x8) & 0xFF);
    msWriteByte(REG_DPRX_AUX_PM2_01_L + usRegOffsetAuxPM2ByID, (ulDPCDAddress >> 0x10) & 0xF);
    msWriteByte(REG_DPRX_AUX_PM2_02_L + usRegOffsetAuxPM2ByID, ucDATA);
    msWriteByteMask(REG_DPRX_AUX_PM2_02_H + usRegOffsetAuxPM2ByID, BIT0, BIT0);
    mhal_DPRx_DELAY_NOP(100);
    msWriteByteMask(REG_DPRX_AUX_PM2_02_H + usRegOffsetAuxPM2ByID, 0, BIT1);
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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

    if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    if(bEnable == TRUE)
    {
        msWriteByteMask(REG_DPRX_AUX_PM0_21_L + usRegOffsetAuxPM0ByID, BIT0, BIT0);
    }
    else
    {
        msWriteByteMask(REG_DPRX_AUX_PM0_21_L + usRegOffsetAuxPM0ByID, 0, BIT0);
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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    BYTE XDATA uctemp = 0;

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (pGUID == NULL))
	{
		return;
	}

    for(uctemp = 0; uctemp < DP_GUID_SIZE; uctemp++)
    {
        pGUID[uctemp] = msReadByte(REG_DPRX_AUX_PM0_31_L + usRegOffsetAuxPM0ByID + uctemp);
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

    if(dprx_id == DPRx_ID_2)
	{
		return mhal_DPRx_GetDPCLane0SQH(FALSE);
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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET_DPCD0(dprx_id);

    if((dprx_id == DPRx_ID_MAX) || (dprx_aux_id == DPRx_AUX_ID_MAX))
	{
		return;
	}

    msWriteByteMask(REG_DPRX_AUX_PM0_0F_L + usRegOffsetAuxPM0ByID, BIT1, BIT1); // Reset
    mhal_DPRx_DELAY_NOP(10);
    msWriteByteMask(REG_DPRX_AUX_PM0_0F_L + usRegOffsetAuxPM0ByID, 0, BIT1); // Reset

    msWriteByteMask(REG_DPRX_DPCD0_1A_L + usRegOffsetDPCD0ByID, BIT5, BIT5); // Reset
    mhal_DPRx_DELAY_NOP(10);
    msWriteByteMask(REG_DPRX_DPCD0_1A_L + usRegOffsetDPCD0ByID, 0, BIT5); // Reset

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
	// TBD
	UNUSED(dprx_decoder_id);
    UNUSED(uwReferClock);
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
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
    DWORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);
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
    	ulBaseMValue = ((DWORD)msReadByte(REG_DPRX_DECODER_E0_31_L + usRegOffsetDecoderByID) << 16) + msRead2Byte(REG_DPRX_DECODER_E0_30_L + usRegOffsetDecoderByID);
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

    msWriteByteMask(REG_DPRX_DECODER_E0_0A_L + usRegOffsetDecoderByID, 0, BIT0); // Receiver 51

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
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
    usTimingInfo[6] = msRead2Byte(REG_DPRX_DECODER_E0_3A_L + usRegOffsetDecoderByID);          // H PWS
    usTimingInfo[7] = msRead2Byte(REG_DPRX_DECODER_E0_3B_L + usRegOffsetDecoderByID);          // V PWS

    return bInterlace;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableLastHsyncAssert()
//  [Description]
//					mhal_DPRx_EnableLastHsyncAssert
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableLastHsyncAssert(DPRx_DECODER_ID dprx_decoder_id, BOOL bEnable)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

    msWriteByteMask(REG_DPRX_DECODER_E0_79_L + usRegOffsetDecoderByID, bEnable? BIT1 : 0, BIT1);

	return;
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
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
void mhal_DPRx_GetMISC01(DPRx_DECODER_ID dprx_decoder_id, WORD *usMISCInfo)
{
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
	WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

	if(bEnable == TRUE)
	{
		msWriteByteMask(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID, BIT6, BIT6);
		msWriteByteMask(REG_DPRX_AUX_PM2_74_L + usRegOffsetAuxPM2ByID, BIT6, BIT6); // DPCD 2207[6] MSA_TIMING_PAR_IGNORED
	}
	else
	{
		msWriteByteMask(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID, 0, BIT6);
		msWriteByteMask(REG_DPRX_AUX_PM2_74_L + usRegOffsetAuxPM2ByID, 0, BIT6); // DPCD 2207[6] MSA_TIMING_PAR_IGNORED
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
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    if((msReadByte(REG_DPRX_AUX_PM2_30_L + usRegOffsetAuxPM2ByID) & BIT7) == BIT7)
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
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);
    BOOL bRet = FALSE;

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (dprx_decoder_id == DPRx_DECODER_ID_MAX))
    {
        return FALSE;
    }

    if((msReadByte(REG_DPRX_AUX_PM2_30_L + usRegOffsetAuxPM2ByID) & BIT7) == BIT7) // Freesync on
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
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
    WORD usRegOffsetSDPByID = DP_REG_OFFSET_SDP(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return 0;
	}

    usPacketStatus = msRead2Byte(REG_DPRX_SDP_E0_02_L + usRegOffsetSDPByID);

    return usPacketStatus;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetPacketStatus_ext()
//  [Description]
//					mhal_DPRx_GetPacketStatus_ext
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
WORD mhal_DPRx_GetPacketStatus_ext(DPRx_DECODER_ID dprx_decoder_id)
{
	WORD usRegOffsetSDPByID = DP_REG_OFFSET_SDP(dprx_decoder_id);
	WORD XDATA usPacketStatus = 0;

    usPacketStatus = msRead2Byte(REG_DPRX_SDP_E0_7D_L + usRegOffsetSDPByID);

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
void mhal_DPRx_ClearPacketStatus(DPRx_DECODER_ID dprx_decoder_id, DP_SDP_SEL ucPacketType)
{
    WORD usRegOffsetSdpByID = DP_REG_OFFSET_SDP(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
	{
		return;
	}

    msWrite2Byte(REG_DPRX_SDP_E0_02_L + usRegOffsetSdpByID, BIT0 << ucPacketType);

    return;
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
void mhal_DPRx_ClearPacketStatus_ext(DPRx_DECODER_ID dprx_decoder_id, DP_SDP_SEL ucPacketType)
{
	WORD usRegOffsetSdpByID = DP_REG_OFFSET_SDP(dprx_decoder_id);

    msWrite2Byte(REG_DPRX_SDP_E0_7D_L + usRegOffsetSdpByID, (BIT0 << ucPacketType)); // Packet status

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
BOOL mhal_DPRx_GetPacketData(DPRx_DECODER_ID dprx_decoder_id, DP_SDP_SEL ucPacketType, BYTE *ucHeader, BYTE *ucPacketData, BYTE ucDataLength)
{
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);
	WORD usRegOffsetSdpByID = DP_REG_OFFSET_SDP(dprx_decoder_id);
	BYTE i = 0;
	BYTE ucSDPData = 0;
	BYTE ucNumberOf32Bytes = 0x0;
	BYTE ucDataArray32Offset = 0x0;

	ucSDPData = msReadByte(REG_DPRX_SDP_E0_08_L + usRegOffsetSdpByID);

    msWriteByteMask(REG_DPRX_SDP_E0_08_L + usRegOffsetDecoderByID, ucPacketType, BIT4|BIT3|BIT2|BIT1|BIT0); // Choose packet type

	if(ucPacketType == DP_SDP_SEL_PPS)
	{
		// PPS packet data are 128 bytes
		if(ucDataLength > DP_DSC_SDP_PACKET_MAX_DATA_LENGTH)
		{
			ucDataLength = DP_DSC_SDP_PACKET_MAX_DATA_LENGTH;
			ucNumberOf32Bytes = 0x4;
		}
		else
		{
			if((ucDataLength % DP_SDP_PACKET_MAX_DATA_LENGTH) == 0x0)
			{
				ucNumberOf32Bytes = ucDataLength / DP_SDP_PACKET_MAX_DATA_LENGTH;
			}
			else
			{
				ucNumberOf32Bytes = (ucDataLength / DP_SDP_PACKET_MAX_DATA_LENGTH) + 1;
			}
		}
	}
	else
	{
		ucNumberOf32Bytes = 0x1;

		if(ucDataLength > DP_SDP_PACKET_MAX_DATA_LENGTH)
		{
			ucDataLength = DP_SDP_PACKET_MAX_DATA_LENGTH;
		}
	}

	mhal_DPRx_DELAY_NOP(10);

	ucDataArray32Offset = ucNumberOf32Bytes;

    DPRX_HAL_DPUTSTR("data: ");

	while(ucNumberOf32Bytes > 0)
	{
		msWriteByteMask(REG_DPRX_SDP_E0_08_H + usRegOffsetDecoderByID, ((ucDataArray32Offset - ucNumberOf32Bytes) & 0x03) << 4, BIT5|BIT4);

		mhal_DPRx_DELAY_NOP(20);

		if(ucDataLength > DP_SDP_PACKET_MAX_DATA_LENGTH)
		{
			for(i = 0x0; i < DP_SDP_PACKET_MAX_DATA_LENGTH; i++)
		    {
		        ucPacketData[(i + (ucDataArray32Offset - ucNumberOf32Bytes) * 32)] = msReadByte(REG_DPRX_SDP_E0_10_L + usRegOffsetDecoderByID + i); // Read data
		    }
			ucDataLength = ucDataLength - DP_SDP_PACKET_MAX_DATA_LENGTH;
		}
		else
		{
		    for(i = 0x0; i < ucDataLength; i++)
		    {
		        ucPacketData[(i + (ucDataArray32Offset - ucNumberOf32Bytes) * 32)] = msReadByte(REG_DPRX_SDP_E0_10_L + usRegOffsetDecoderByID + i); // Read data
		        DPRX_HAL_DPRINTF("%x ", ucPacketData[(i + (ucDataArray32Offset - ucNumberOf32Bytes) * 32)] );
		    }
		}

		ucNumberOf32Bytes--;
	}

    DPRX_HAL_DPUTSTR("\r\n ucHeader");

    for(i = 0x0; i < 4; i++)
    {
        ucHeader[i] = msReadByte(REG_DPRX_SDP_E0_0E_L + usRegOffsetDecoderByID + i); // Read header
        DPRX_HAL_DPRINTF("%x ", ucHeader[i]);
    }

    DPRX_HAL_DPUTSTR("\r\n");

	if(msRead2Byte(REG_DPRX_SDP_E0_02_L + usRegOffsetSdpByID) & (BIT0 << ucPacketType))
	{
		msWriteByte(REG_DPRX_SDP_E0_08_L + usRegOffsetSdpByID, ucSDPData);

		return TRUE;
	}
	else
	{
		msWriteByte(REG_DPRX_SDP_E0_08_L + usRegOffsetSdpByID, ucSDPData);

		return FALSE;
	}
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
void mhal_DPRx_GetVSCSDP(DPRx_DECODER_ID dprx_decoder_id, WORD *usMISCInfo)
{
	WORD usRegOffsetSdpByID = DP_REG_OFFSET_SDP(dprx_decoder_id);
	BYTE ucSDPData = 0;

	ucSDPData = msReadByte(REG_DPRX_SDP_E0_08_L + usRegOffsetSdpByID);

	msWriteByteMask(REG_DPRX_SDP_E0_08_L + usRegOffsetSdpByID, DP_SDP_SEL_VSC, BMASK(3:0)); // Choose VSP

	usMISCInfo[0] = msReadByte(REG_DPRX_SDP_E0_18_L + usRegOffsetSdpByID); // SDP DB16
	usMISCInfo[1] = msReadByte(REG_DPRX_SDP_E0_18_H + usRegOffsetSdpByID); // SDP DB17
	usMISCInfo[2] = msReadByte(REG_DPRX_SDP_E0_19_L + usRegOffsetSdpByID); // SDP DB18

	msWriteByte(REG_DPRX_SDP_E0_08_L + usRegOffsetSdpByID, ucSDPData);

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
BOOL mhal_DPRx_GetVSC_EXT(DPRx_DECODER_ID dprx_decoder_id, DP_VSC_EXT_TYPE ucType, BYTE *ucVSC_EXT)
{
	WORD usRegOffsetSdpByID = DP_REG_OFFSET_SDP(dprx_decoder_id);
	BYTE ucCount = 0;
    BYTE ucDataCount = 0;
    BYTE ucAddressOffset = 0;
    BYTE i = 0;
    BYTE j = 0;
	#if (VSC_EXT_SW == 0x1)
    BOOL bReceive_Bit = 0;
	BOOL bSRAM_Sel = 0;
	#endif

    if(ucType == CEA)
    {
        ucAddressOffset = 4;
    }
    else
	{
	    ucAddressOffset = 0;
    }

	#if (VSC_EXT_SW == 0x1)
  	{
	    bReceive_Bit = (msReadByte(REG_DPRX_SDP_E0_04_L + ucAddressOffset + usRegOffsetSdpByID) & BIT3)? TRUE : FALSE;
	    bSRAM_Sel = msReadByte(REG_DPRX_SDP_E0_05_L + ucAddressOffset + usRegOffsetSdpByID) & BIT0;

        if(bSRAM_Sel)
        {
            msWriteByteMask(REG_DPRX_SDP_E0_04_L + ucAddressOffset + usRegOffsetSdpByID, 0, BIT6); // Set read
        }
        else
        {
            msWriteByteMask(REG_DPRX_SDP_E0_04_L + ucAddressOffset + usRegOffsetSdpByID, 0, BIT7); // Set read
        }

        msWriteByteMask(REG_DPRX_SDP_E0_04_L + ucAddressOffset + usRegOffsetSdpByID, BIT4, BIT4);

        if(bSRAM_Sel)
        {
            msWriteByteMask(REG_DPRX_SDP_E0_04_L + ucAddressOffset + usRegOffsetSdpByID, 0, BIT5);
        }
        else
        {
            msWriteByteMask(REG_DPRX_SDP_E0_04_L + ucAddressOffset + usRegOffsetSdpByID, BIT5, BIT5);
        }
	}
	#endif

	if(ucType == CEA)
	{
        msWriteByteMask(REG_DPRX_SDP_E0_08_H + usRegOffsetSdpByID, BIT3, BIT3); // Set VSC_EXT_CEA

        DPRX_HAL_DPRINTF("\r\n Check CEA %x", i);
	}
	else
	{
        msWriteByteMask(REG_DPRX_SDP_E0_08_H + usRegOffsetSdpByID, 0, BIT3); // Set VSC_EXT_VESA

        DPRX_HAL_DPRINTF("\r\n Check VESA %x", i);
	}

	ucCount = msReadByte(REG_DPRX_SDP_E0_05_H + ucAddressOffset + usRegOffsetSdpByID) + 1;

	#if (VSC_EXT_SW == 0x1)
    if(bReceive_Bit)
	#else
    if(msReadByte(REG_DPRX_SDP_E0_04_L + ucAddressOffset + usRegOffsetSdpByID) & BIT3)
	#endif
    {
        msWriteByteMask(REG_DPRX_SDP_E0_04_L + ucAddressOffset + usRegOffsetSdpByID, BIT0, BIT0); // Set read

        for(i = 0; i < ucCount; i++)
        {
			DPRX_HAL_DPRINTF("\r\n [i] => %x", i);

			msWriteByteMask(REG_DPRX_SDP_E0_04_L + ucAddressOffset + usRegOffsetSdpByID, BIT2, BIT2); // Read trigger

			for(j = 0; j < 32; j++)
			{
				ucVSC_EXT[ucDataCount] = msReadByte(REG_DPRX_SDP_E0_20_L + j + usRegOffsetSdpByID);

				DPRX_HAL_DPRINTF("\r\n VSC_EXT = %x", ucVSC_EXT[ucDataCount]);

				ucDataCount++;
			}
        }
    }
    else
    {
		DPRX_HAL_DPRINTF("\r\n No VSC_EXT = %x", 0);

		return 0;
    }

    msWriteByteMask(REG_DPRX_SDP_E0_04_L + ucAddressOffset + usRegOffsetSdpByID, BIT1, BIT1); // Set free
    msWriteByteMask(REG_DPRX_SDP_E0_04_L + ucAddressOffset + usRegOffsetSdpByID, BIT3, BIT3); // Clear

    #if (VSC_EXT_SW == 0x1)
    msWriteByteMask(REG_DPRX_SDP_E0_04_L + ucAddressOffset + usRegOffsetSdpByID, BIT7|BIT6, BIT7|BIT6); // Set read
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
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET_DPCD0(dprx_id);

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
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bPowerStateFlag = ((msReadByte(REG_DPRX_AUX_PM0_39_L + usRegOffsetAuxPM0ByID) & (BIT0|BIT1)) != BIT1)? TRUE : FALSE;

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
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET_DPCD0(dprx_id);
	WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET_TRANS_CTRL(dprx_id);
	WORD usRegOffsetPHY0ByID = DP_REG_OFFSET_PHY0(dprx_phy_id);

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

        mhal_DPRx_SetAuxDelayReply(dprx_aux_id, DP_AUX_DELAY_NORMAL);
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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    if((msReadByte(REG_DPRX_AUX_PM0_39_L + usRegOffsetAuxPM0ByID) & (BIT0|BIT1)) != BIT1)
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
	WORD usRegOffsetAuxByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
	WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET_TRANS_CTRL(dprx_id);

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
	WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET_TRANS_CTRL(dprx_id);

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
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_CRC_Get()
//  [Description]
//                  mhal_DPRx_CRC_Get
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_CRC_Get(DPRx_DECODER_ID dprx_decoder_id, WORD *usCRCValue)
{
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

    usCRCValue[0] = msRead2Byte(REG_DPRX_DECODER1_E0_18_L + usRegOffsetDecoderByID);
    usCRCValue[1] = msRead2Byte(REG_DPRX_DECODER1_E0_19_L + usRegOffsetDecoderByID);
    usCRCValue[2] = msRead2Byte(REG_DPRX_DECODER1_E0_1A_L + usRegOffsetDecoderByID);

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
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

    msWriteByteMask(REG_DPRX_DECODER_E0_01_H + usRegOffsetDecoderByID, bEnable? BIT2 : 0, BIT2);

	return;
}

#if(DPRX_AUDIO_NOVIDEO == 0x1)
//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_IsVBIDVideoMute()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_IsVBIDVideoMute(DPRx_DECODER_ID dprx_decoder_id, BYTE times)
{
    Bool XDATA bVideoMuteFlag = FALSE;
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
    {
        return bVideoMuteFlag;
    }

    while(times--)
    {
        if(msReadByte(REG_DPRX_DECODER_E0_2D_L+usRegOffsetDecoderByID)&BIT3) // REG_DP_OP_7A[3]: Read Bit3:1 NoVideoStrm
        {
            bVideoMuteFlag = TRUE;
            break;
        }

		mhal_DPRx_DELAY_NOP(20);
    }

    return bVideoMuteFlag;

}
#endif

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

    #if (FPGA_Verification == 0)
    // AUPLL setting from APLL APN
    msWrite2Byte(REG_DPRX_PLL_TOP0_10_L, 0x001F);// [4:0] reg_pll_clkgen_en_apll[5] reg_pll_clkgen_en_alock

    //  [0] reg_pd_aupll
    //  [1] reg_pd_aupll_kf0
    //  [2] reg_pd_aupll_kp0
    //  [3] reg_pd_aupll_kp1
    //  [4] reg_pd_aupll_kpdiv
    //  [8] reg_en_aupll_rst
    //  [9] reg_en_aupll_test
    // [10] reg_en_aupll_xtal
    // [11] reg_en_aupll_prdt
    // [12] reg_en_aupll_ov_cp_sw
    msWrite2ByteMask(REG_DPRX_PLL_TOP0_12_L, 0, BIT0|BIT3);

    //   [7:0] reg_gcr_aupll_loopdiv_second
    //   [9:8] reg_gcr_aupll_loopdiv_first
    // [14:12] reg_gcr_aupll_output_div
    msWrite2Byte(REG_DPRX_PLL_TOP0_14_L, 0x4104);
    #endif

    // Reference by MST9U6
    for(Audio_Num = 0; Audio_Num < Audio_Function_Num; Audio_Num++)
    {
	    msWrite2Byte(REG_DPRX_ALOCK0_0C_L + DP_REG_OFFSET_ALOCK(Audio_Num), 0x0008); // Old mode

	    msWrite2Byte(REG_DPRX_ALOCK0_0B_L + DP_REG_OFFSET_ALOCK(Audio_Num), 0x6565);
	    msWrite2Byte(REG_DPRX_ALOCK0_05_L + DP_REG_OFFSET_ALOCK(Audio_Num), 0x0F21);
	    msWrite2Byte(REG_DPRX_ALOCK0_06_L + DP_REG_OFFSET_ALOCK(Audio_Num), 0x0000);
	    msWrite2Byte(REG_DPRX_ALOCK0_07_L + DP_REG_OFFSET_ALOCK(Audio_Num), 0x0002);
	    msWrite2Byte(REG_DPRX_ALOCK0_51_L + DP_REG_OFFSET_ALOCK(Audio_Num), 0x0080);
	    msWrite2Byte(REG_DPRX_ALOCK0_0D_L + DP_REG_OFFSET_ALOCK(Audio_Num), 0x0301); // reg_force_prd_stable = 1 // RD George suggest // QD88E CTS issue confirm by Bruce
	    msWrite2Byte(REG_DPRX_ALOCK0_20_L + DP_REG_OFFSET_ALOCK(Audio_Num), 0x3331);

	    msWrite2Byte(REG_DPRX_ALOCK0_55_L + DP_REG_OFFSET_ALOCK(Audio_Num), 0xC063); // Auto
	    msWrite2Byte(REG_DPRX_ALOCK0_54_L + DP_REG_OFFSET_ALOCK(Audio_Num), 0xC00A); // Auto
        msWrite2Byte(REG_DPRX_AUDIO_CTRL0_51_L + DP_REG_OFFSET_ALOCK(Audio_Num), 0xFFFF);
        msWrite2Byte(REG_DPRX_AUDIO_CTRL0_4B_L + DP_REG_OFFSET_ALOCK(Audio_Num), 0xE00C);
        msWrite2Byte(REG_DPRX_AUDIO_CTRL0_52_L + DP_REG_OFFSET_ALOCK(Audio_Num), 0xC000);
        msWriteByteMask(REG_DPRX_AUDIO_CTRL0_53_L + DP_REG_OFFSET_ALOCK(Audio_Num), 0xC0, BIT7|BIT6);
        msWriteByte(REG_DPRX_AUDIO_CTRL0_54_H + DP_REG_OFFSET_ALOCK(Audio_Num), 0x18);  // [15] = 1: Audio left/right channel swap.
        msWriteByte(REG_DPRX_AUDIO_CTRL0_56_L + DP_REG_OFFSET_ALOCK(Audio_Num), 0x80);

        msWriteByte(REG_DPRX_SDP_E0_00_H + DP_REG_OFFSET_ALOCK(Audio_Num), 0x80); // Audio CTS 5443 ECO
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
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);
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
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
	WORD usRegOffsetSdpByID = DP_REG_OFFSET_SDP(dprx_decoder_id);

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
	WORD usRegOffsetSdpByID = DP_REG_OFFSET_SDP(dprx_decoder_id);

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
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
    WORD usRegOffsetSdpByID = DP_REG_OFFSET_SDP(dprx_decoder_id);

    if((dprx_decoder_id == DPRx_DECODER_ID_MAX) || (pPacketData == NULL))
    {
        return 0;
    }

    ucSDPData = msReadByte(REG_DPRX_SDP_E0_08_L + usRegOffsetSdpByID);

    msWriteByteMask(REG_DPRX_SDP_E0_08_L + usRegOffsetSdpByID, DP_SDP_SEL_AUDIO_STREAM, BMASK(4:0)); // Choose Audio stream packet

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
	WORD usRegOffsetAlock0ByID = DP_REG_OFFSET_ALOCK(DPRX_AUDIO_SELECT);

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
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);
    WORD usRegOffsetSdpByID = DP_REG_OFFSET_SDP(dprx_decoder_id);

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
    WORD usRegOffsetSdpByID = DP_REG_OFFSET_SDP(dprx_decoder_id);
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
    WORD usRegOffsetAlock0ByID = DP_REG_OFFSET_ALOCK(DPRX_AUDIO_SELECT);
    DPRx_AudioFreqType XDATA ucFreqVaild = DPRx_AUD_FREQ_ERROR;
    WORD XDATA usDPAudioIVS;
    BYTE XDATA ucDPAudioFreq;

    if(DPRX_AUDIO_SELECT == DPRx_AUD_STREAM_MAX)
    {
        return FALSE;
    }

    usDPAudioIVS = (msRead2Byte(REG_DPRX_ALOCK0_21_L + usRegOffsetAlock0ByID));

    if(usDPAudioIVS != 0)
    {
        ucDPAudioFreq = (12 * 1000ul) / usDPAudioIVS;

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
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);
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
	WORD usRegOffsetAlock0ByID = DP_REG_OFFSET_ALOCK(DPRX_AUDIO_SELECT);

    while(Times--)
    {
        if((msRead2Byte(REG_DPRX_ALOCK0_2A_L + usRegOffsetAlock0ByID) == DPRX_AUDIO_CS_STABLE) ||
          ((msReadByte(REG_DPRX_ALOCK0_11_L + usRegOffsetAlock0ByID) <= DPRX_AUDIO_PHASE_DIFF_RANGE) && (msRead2Byte(REG_DPRX_ALOCK0_2A_L + usRegOffsetAlock0ByID) == DPRX_AUDIO_CS_STABLE1)))
        {
            // Do nothing
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
	WORD usRegOffsetAlock0ByID = DP_REG_OFFSET_ALOCK(DPRX_AUDIO_SELECT);

    if((msRead2Byte(REG_DPRX_ALOCK0_2A_L + usRegOffsetAlock0ByID) == DPRX_AUDIO_CS_STABLE1) && (msReadByte(REG_DPRX_ALOCK0_11_L + usRegOffsetAlock0ByID) >= DPRX_AUDIO_PHASE_DIFF_RESET))
    {
        msWriteByteMask((REG_DPRX_ALOCK0_00_L + usRegOffsetAlock0ByID), 0, BIT0);
        mhal_DPRx_DELAY_1ms();
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
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

    msWriteByte(REG_DPRX_DECODER_E0_0F_L + usRegOffsetDecoderByID, BIT0);
	mhal_DPRx_DELAY_NOP(10);
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
    WORD usRegOffsetAudioCTRL0ByID = DP_REG_OFFSET_AUDIO_CTRL(DPRX_AUDIO_SELECT);

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
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);
    BOOL bNoAudioFlag = FALSE;

	bNoAudioFlag = (msReadByte(REG_DPRX_DECODER_E0_2D_L + usRegOffsetDecoderByID) & BIT4)? TRUE : FALSE;

	return bNoAudioFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetAudioChannelStatus()
//  [Description]
//					mhal_DPRx_GetAudioChannelStatus
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetAudioChannelStatus(DPRx_DECODER_ID dprx_decoder_id, BYTE ucPacketLength, BYTE *ucPacketData)
{
    WORD usRegOffsetSdpByID = DP_REG_OFFSET_SDP(dprx_decoder_id);
    BYTE i;

    if(ucPacketLength > 24)
    {
        ucPacketLength = 24;
    }

    for(i = 0; i < ucPacketLength ; i++)
    {
        if(i < 5)
        {
            *(ucPacketData + i) = msReadByte(REG_DPRX_SDP_E0_30_L + i + usRegOffsetSdpByID);
        }
        else
        {
            *(ucPacketData + i) = 0;
        }
    }

    return TRUE;
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
DPRx_AudioStream mhal_DPRx_GetAudioStream(DPRx_ID dprx_id) // Need to modify the initial vaule of DPRX_AUDIO_SELECT when IC have more than one audio engine
{
	volatile DPRx_AudioStream DPRX_AUDIO_SELECT = DPRx_AUD_STREAM_0;

	if(dprx_id == DPRx_ID_MAX)
	{
		return DPRx_AUD_STREAM_MAX;
	}

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
	WORD usRegOffsetSdpByID = DP_REG_OFFSET_SDP(dprx_decoder_id);

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
	volatile WORD usRegOffsetAudioCTRL0ByID = DP_REG_OFFSET_AUDIO_CTRL(dprx_audio_select);

    if(dprx_audio_select == DPRx_AUD_STREAM_MAX)
    {
        return;
    }

    msWriteByteMask(REG_DPRX_AUDIO_CTRL0_4B_L + usRegOffsetAudioCTRL0ByID, bEnable? BIT2 : 0, BIT2);

    if(bEnable == FALSE)
    {
        // Pacth for clear mute event
        msWriteByteMask(REG_DPRX_AUDIO_CTRL0_52_L + usRegOffsetAudioCTRL0ByID, BIT2, BIT2);
        mhal_DPRx_DELAY_NOP(10);
        msWriteByteMask(REG_DPRX_AUDIO_CTRL0_52_L + usRegOffsetAudioCTRL0ByID, 0, BIT2);
    }

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
	WORD usRegOffsetSdpByID = DP_REG_OFFSET_SDP(dprx_decoder_id);

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
	volatile WORD usRegOffsetAudioALOCK0ByID = DP_REG_OFFSET_ALOCK(dprx_audio_select);

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
	volatile WORD usRegOffsetAudioALOCK0ByID = DP_REG_OFFSET_ALOCK(dprx_audio_select);

    if(dprx_audio_select == DPRx_AUD_STREAM_MAX)
    {
        return;
    }

    msWriteByteMask(REG_DPRX_ALOCK0_54_H + usRegOffsetAudioALOCK0ByID, 0, BIT7); //reg_auto_fpll_en
    msWriteByteMask(REG_DPRX_ALOCK0_0C_L + usRegOffsetAudioALOCK0ByID, 0, BIT3); //reg_frame_lpll_en
    mhal_DPRx_DELAY_NOP(15);
    msWriteByteMask(REG_DPRX_ALOCK0_0C_L + usRegOffsetAudioALOCK0ByID, BIT3, BIT3);
    msWriteByteMask(REG_DPRX_ALOCK0_54_H + usRegOffsetAudioALOCK0ByID, BIT7, BIT7);

    return;
}

#if(DPRX_AUDIO_NOVIDEO == 0x1)
//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_IsVBIDAudioMute()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool mhal_DPRx_IsVBIDAudioMute(DPRx_DECODER_ID dprx_decoder_id, BYTE times)
{
    Bool XDATA bAudioMuteFlag = FALSE;
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

	if(dprx_decoder_id == DPRx_DECODER_ID_MAX)
    {
        return bAudioMuteFlag;
    }

    while(times--)
    {
        if(msReadByte(REG_DPRX_DECODER_E0_2D_L + usRegOffsetDecoderByID) & BIT4) // REG_DP_OP_7A[4]: Read Bit4:1 NoAudioStrm
        {
            bAudioMuteFlag = TRUE;
            break;
        }

		mhal_DPRx_DELAY_NOP(20);
    }

    return bAudioMuteFlag;

}
#endif

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
    if(bEnable)
    {
	    mhal_DPRx_EnableAuxInterruptSetting(dprx_aux_id, TRUE);
        mhal_DPRx_EnableMCCSInterrupt(dprx_aux_id, TRUE);
	}
	else
	{
	    mhal_DPRx_EnableAuxInterruptSetting(dprx_aux_id, FALSE);
        mhal_DPRx_EnableMCCSInterrupt(dprx_aux_id, FALSE);
	}

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
    mhal_DPRx_EnableDPCD100Interrupt(dprx_id, bEnable);
    mhal_DPRx_EnableDPCD270Interrupt(dprx_id, bEnable);
    mhal_DPRx_EnableDPCD270bit1Interrupt(dprx_id, bEnable);
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
	WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_AUX_PM0_3C_H + usRegOffsetAuxPM0ByID, BIT7, BIT7);
	mhal_DPRx_DELAY_NOP(5);
	msWriteByteMask(REG_DPRX_AUX_PM0_3C_H + usRegOffsetAuxPM0ByID, 0, BIT7);

	msWriteByteMask(REG_DPRX_AUX_PM0_3D_H + usRegOffsetAuxPM0ByID, bEnable? 0 : BIT7, BIT7); // MCCS
	msWriteByteMask(REG_DPRX_AUX_PM0_4D_L + usRegOffsetAuxPM0ByID, BIT1, BIT1); // MCCS reply by MCU

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
    if(bEnable == TRUE)
	{
        msWriteByteMask(REG_PM_IRQ_14_H, 0, BIT5); // Unmask dp_aux_int_all (hst0 interrupt[13])
        msWriteByteMask(REG_DPRX_PM_TOP_04_H, 0, (BIT0 << (dprx_aux_id + 1)));  // Unmask AUX Rx IRQ
	}
	else
	{
        msWriteByteMask(REG_PM_IRQ_14_H, BIT5, BIT5); // Mask dp_aux_int_all (hst0 interrupt[13])
        msWriteByteMask(REG_DPRX_PM_TOP_04_H, (BIT0 << (dprx_aux_id + 1)), (BIT0 << (dprx_aux_id + 1))); // Mask AUX Rx IRQ
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
	WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_AUX_PM2_4D_H + usRegOffsetAuxPM2ByID, BIT7, BIT7);
	mhal_DPRx_DELAY_NOP(5);
	msWriteByteMask(REG_DPRX_AUX_PM2_4D_H + usRegOffsetAuxPM2ByID, 0, BIT7);

	msWriteByteMask(REG_DPRX_AUX_PM2_49_L + usRegOffsetAuxPM2ByID, bEnable? 0 : BIT7, BIT7); // [7] Rx receiver IRQ

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
	WORD usRegOffsetAuxByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    if((msReadByte(REG_DPRX_AUX_PM0_48_L + usRegOffsetAuxByID) & BIT4) == BIT4)
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
	WORD usRegOffsetAuxByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_AUX_PM0_49_L + usRegOffsetAuxByID, BIT4, BIT4);
	mhal_DPRx_DELAY_NOP(5);
	msWriteByteMask(REG_DPRX_AUX_PM0_49_L + usRegOffsetAuxByID, 0, BIT4);

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
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);

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
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);

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
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);

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
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);

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
//                  mhal_DPRx_EnableLinkClockBigChangeInterrupt()
//  [Description]
//					mhal_DPRx_EnableLinkClockBigChangeInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableLinkClockBigChangeInterrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);

	msWriteByteMask(REG_DPRX_RECEIVER_61_H + usRegOffsetReceiverByID, BIT0, BIT0);
	mhal_DPRx_DELAY_NOP(10);
	msWriteByteMask(REG_DPRX_RECEIVER_61_H + usRegOffsetReceiverByID, 0, BIT0);

	msWriteByteMask(REG_DPRX_RECEIVER_5D_H + usRegOffsetReceiverByID, bEnable? 0 : BIT0, BIT0); // [8] Big changing of link clock

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableHDCPdpesInterrupt()
//  [Description]
//					mhal_DPRx_EnableHDCPdpesInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableHDCPdpesInterrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);

	msWriteByteMask(REG_DPRX_RECEIVER_61_H + usRegOffsetReceiverByID, BIT6, BIT6);
	mhal_DPRx_DELAY_NOP(10);
	msWriteByteMask(REG_DPRX_RECEIVER_61_H + usRegOffsetReceiverByID, 0, BIT6);

	msWriteByteMask(REG_DPRX_RECEIVER_5D_H + usRegOffsetReceiverByID, bEnable? 0 : BIT6, BIT6); // HDCP dpes

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableNoHDCPdpesInterrupt()
//  [Description]
//					mhal_DPRx_EnableNoHDCPdpesInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableNoHDCPdpesInterrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);

	msWriteByteMask(REG_DPRX_RECEIVER_61_H + usRegOffsetReceiverByID, BIT7, BIT7);
	mhal_DPRx_DELAY_NOP(10);
	msWriteByteMask(REG_DPRX_RECEIVER_61_H + usRegOffsetReceiverByID, 0, BIT7);

	msWriteByteMask(REG_DPRX_RECEIVER_5D_H + usRegOffsetReceiverByID, bEnable? 0 : BIT7, BIT7); // ~ HDCP dpes

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableNoLinkClockInterrupt()
//  [Description]
//					mhal_DPRx_EnableNoLinkClockInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableNoLinkClockInterrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);

	msWriteByteMask(REG_DPRX_RECEIVER_61_H + usRegOffsetReceiverByID, BIT1, BIT1);
	mhal_DPRx_DELAY_NOP(10);
	msWriteByteMask(REG_DPRX_RECEIVER_61_H + usRegOffsetReceiverByID, 0, BIT1);

	msWriteByteMask(REG_DPRX_RECEIVER_5D_H + usRegOffsetReceiverByID, bEnable? 0 : BIT1, BIT1); // [9] No link clock

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
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);

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
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);

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
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET_DPCD1(dprx_id);

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
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET_DPCD1(dprx_id);

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
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET_DPCD1(dprx_id);

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
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET_DPCD1(dprx_id);

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
//                  mhal_DPRx_EnableDPCD100PMInterrupt()
//  [Description]
//					mhal_DPRx_EnableDPCD100PMInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableDPCD100PMInterrupt(DPRx_AUX_ID dprx_aux_id, BOOL bEnable)
{
	volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);

    msWriteByteMask(REG_DPRX_AUX_PM3_3B_L + usRegOffsetAuxPM3ByID, BIT1, BIT1);
	mhal_DPRx_DELAY_NOP(10);
    msWriteByteMask(REG_DPRX_AUX_PM3_3B_L + usRegOffsetAuxPM3ByID, 0, BIT1);

    msWriteByteMask(REG_DPRX_AUX_PM3_35_L + usRegOffsetAuxPM3ByID, bEnable? 0 : BIT1, BIT1);

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableDPCD101PMInterrupt()
//  [Description]
//					mhal_DPRx_EnableDPCD101PMInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableDPCD101PMInterrupt(DPRx_AUX_ID dprx_aux_id, BOOL bEnable)
{
	volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);

    msWriteByteMask(REG_DPRX_AUX_PM3_3B_L + usRegOffsetAuxPM3ByID, BIT0, BIT0);
	mhal_DPRx_DELAY_NOP(10);
    msWriteByteMask(REG_DPRX_AUX_PM3_3B_L + usRegOffsetAuxPM3ByID, 0, BIT0);

    msWriteByteMask(REG_DPRX_AUX_PM3_35_L + usRegOffsetAuxPM3ByID, bEnable? 0 : BIT0, BIT0);

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableDPCD108PMInterrupt()
//  [Description]
//					mhal_DPRx_EnableDPCD108PMInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableDPCD108PMInterrupt(DPRx_AUX_ID dprx_aux_id, BOOL bEnable)
{
	volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);

    msWriteByteMask(REG_DPRX_AUX_PM3_3A_H + usRegOffsetAuxPM3ByID, BIT7, BIT7);
	mhal_DPRx_DELAY_NOP(10);
    msWriteByteMask(REG_DPRX_AUX_PM3_3A_H + usRegOffsetAuxPM3ByID, 0, BIT7);

    msWriteByteMask(REG_DPRX_AUX_PM3_34_H + usRegOffsetAuxPM3ByID, bEnable? 0 : BIT7, BIT7);

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
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET_DPCD0(dprx_id);

    msWriteByteMask(REG_DPRX_DPCD0_7D_L + usRegOffsetDPCD0ByID, BIT1, BIT1);
    mhal_DPRx_DELAY_NOP(10);
    msWriteByteMask(REG_DPRX_DPCD0_7D_L + usRegOffsetDPCD0ByID, 0, BIT1);

    msWriteByteMask(REG_DPRX_DPCD0_7B_L + usRegOffsetDPCD0ByID, bEnable? 0 : BIT1, BIT1);

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
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET_DPCD0(dprx_id);

    msWriteByteMask(REG_DPRX_DPCD0_7D_L + usRegOffsetDPCD0ByID, BIT5, BIT5);
    mhal_DPRx_DELAY_NOP(10);
    msWriteByteMask(REG_DPRX_DPCD0_7D_L + usRegOffsetDPCD0ByID, 0, BIT5);

    msWriteByteMask(REG_DPRX_DPCD0_7B_L + usRegOffsetDPCD0ByID, bEnable? 0 : BIT5, BIT5);

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
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET_DPCD1(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_DPCD1_31_L + usRegOffsetDPCD1ByID, BIT6, BIT6);
    mhal_DPRx_DELAY_NOP(10);
    msWriteByteMask(REG_DPRX_DPCD1_31_L + usRegOffsetDPCD1ByID, 0, BIT6);

    msWriteByteMask(REG_DPRX_DPCD1_2E_L + usRegOffsetDPCD1ByID, bEnable? 0 : BIT6, BIT6); // [6] DPCD 270h write pulse IRQ

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
void mhal_DPRx_EnableDPCD270bit1Interrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET_DPCD1(dprx_id);

    msWriteByteMask(REG_DPRX_DPCD1_31_L + usRegOffsetDPCD1ByID, BIT1, BIT1);
    mhal_DPRx_DELAY_NOP(10);
    msWriteByteMask(REG_DPRX_DPCD1_31_L + usRegOffsetDPCD1ByID, 0, BIT1);

    msWriteByteMask(REG_DPRX_DPCD1_2E_L + usRegOffsetDPCD1ByID, bEnable? 0 : BIT1, BIT1); // [1] DPCD 270h[0] test sink start

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableDPCD100Interrupt()
//  [Description]
//					mhal_DPRx_EnableDPCD100Interrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableDPCD100Interrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET_DPCD1(dprx_id);

    msWriteByteMask(REG_DPRX_DPCD1_3D_L + usRegOffsetDPCD1ByID, BIT6, BIT6);
    mhal_DPRx_DELAY_NOP(10);
    msWriteByteMask(REG_DPRX_DPCD1_3D_L + usRegOffsetDPCD1ByID, 0, BIT6);

    msWriteByteMask(REG_DPRX_DPCD1_3B_L + usRegOffsetDPCD1ByID, bEnable? 0 : BIT6, BIT6); // [22] dpcd_00100h_irq

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
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
	WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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
    WORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);

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

    mhal_DPRx_SetAUPLLBigChangeInterrupt(dprx_decoder_id, bEnable);
    mhal_DPRx_SetVPLLBigChangeInterrupt(dprx_decoder_id, bEnable);
    mhal_DPRx_SetMSAChgInterrupt(dprx_decoder_id, bEnable);
    mhal_DPRx_EnableAutoInterlace(dprx_decoder_id, bEnable);
    mhal_DPRx_SetVideoMuteInterrupt(dprx_decoder_id, bEnable);
    mhal_DPRx_SetAudioChanneStatusChgInterrupt(dprx_decoder_id, bEnable);
    mhal_DPRx_SetAudioMuteInterrupt(dprx_decoder_id, bEnable);
    mhal_DPRx_SetMISC0ChangeInterrupt(dprx_decoder_id, bEnable);
    mhal_DPRx_SetMISC1ChangeInterrupt(dprx_decoder_id, bEnable);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableDPCD11AWriteInterrupt()
//  [Description]
//					mhal_DPRx_EnableDPCD11AWriteInterrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableDPCD11AWriteInterrupt(DPRx_AUX_ID dprx_aux_id, BOOL bEnable)
{
	WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);

    msWriteByteMask(REG_DPRX_AUX_PM3_3B_L + usRegOffsetAuxPM3ByID, BIT4, BIT4);
	mhal_DPRx_DELAY_NOP(10);
    msWriteByteMask(REG_DPRX_AUX_PM3_3B_L + usRegOffsetAuxPM3ByID, 0, BIT4);

    msWriteByteMask(REG_DPRX_AUX_PM3_35_L + usRegOffsetAuxPM3ByID, bEnable? 0 : BIT4, BIT4);

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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    if(bSetHPD == TRUE)
    {
		msWriteByteMask(REG_DPRX_AUX_PM0_56_H + usRegOffsetAuxPM0ByID, BIT4, BIT4);
		msWriteByteMask(REG_DPRX_AUX_PM0_58_H + usRegOffsetAuxPM0ByID, BIT4, BIT4); // HPD value
    }
    else
    {
		msWriteByteMask(REG_DPRX_AUX_PM0_56_H + usRegOffsetAuxPM0ByID, 0, BIT4);
		msWriteByteMask(REG_DPRX_AUX_PM0_58_H + usRegOffsetAuxPM0ByID, 0, BIT4); // HPD value
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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    if(((msReadByte(REG_DPRX_AUX_PM0_56_H + usRegOffsetAuxPM0ByID) & (BIT3|BIT7)) == (BIT3|BIT7)) &&
        (((msReadByte(REG_DPRX_AUX_PM0_57_L + usRegOffsetAuxPM0ByID) & BIT3) == BIT3)))
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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

	switch(dprx_aux_id)
	{
		case DPRx_AUX_ID_0:
			    msWriteByteMask(REG_PAD_TOP_0B_H, bEnable, BIT0); // Enable DP RX0 HPD
			break;

		case DPRx_AUX_ID_1:
			    msWriteByteMask(REG_PAD_TOP_0B_H, (bEnable << 1), BIT1); // Enable DP RX1 HPD
			break;

		case DPRx_AUX_ID_2:
			    msWriteByteMask(REG_PAD_TOP_0B_H, (bEnable << 2), BIT2); // Enable DP RX2 HPD
			break;

		default:
			break;
	}

    msWriteByteMask(REG_DPRX_AUX_PM0_56_H + usRegOffsetAuxPM0ByID, 0, BIT6); // hpd_oen hpd gpio output mode

    if(bEnable == TRUE)
    {
		msWriteByte(REG_DPRX_AUX_PM0_52_H + usRegOffsetAuxPM0ByID, 0x4B); // 0x44 is about 730us ~ 760us for short HPD

		msWriteByte(REG_DPRX_AUX_PM0_4F_L + usRegOffsetAuxPM0ByID, 0xEA); // 0xEA is about 650ms for long HPD

		msWriteByteMask(REG_DPRX_AUX_PM0_56_H + usRegOffsetAuxPM0ByID, BIT7, BIT7); // reg_aux_cmd_period_bypass
		msWriteByteMask(REG_DPRX_AUX_PM0_57_L + usRegOffsetAuxPM0ByID, BIT3, BIT3); // reg_hpd_hw_en
		msWriteByteMask(REG_DPRX_AUX_PM0_56_H + usRegOffsetAuxPM0ByID, BIT3, BIT3); // reg_hpd_i_hw_mode
	}
    else
    {
        msWriteByteMask(REG_DPRX_AUX_PM0_56_H + usRegOffsetAuxPM0ByID, 0, BIT7);
		msWriteByteMask(REG_DPRX_AUX_PM0_57_L + usRegOffsetAuxPM0ByID, 0, BIT3);
		msWriteByteMask(REG_DPRX_AUX_PM0_56_H + usRegOffsetAuxPM0ByID, 0, BIT3);
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
    DWORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = ((msReadByte(REG_DPRX_AUX_PM0_7C_H + usRegOffsetAuxPM0ByID) & BIT4) ? TRUE : FALSE);

    if(bEventFlag)
    {
        msWriteByteMask(REG_DPRX_AUX_PM0_7C_H + usRegOffsetAuxPM0ByID, BIT7, BIT7);
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
    WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET_TRANS_CTRL(dprx_id);

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
	WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    msWriteByteMask(REG_DPRX_AUX_PM0_57_L + usRegOffsetAuxPM0ByID, BIT5, BIT5);

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
	WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    if(msReadByte(REG_DPRX_AUX_PM0_57_L + usRegOffsetAuxPM0ByID) & BIT7)
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
#if FPGA_Verification
    UNUSED(dprx_aux_id);
#else
    msWriteByteMask(REG_PM_48_H, (BIT4|BIT0) << dprx_aux_id , (BIT4|BIT0) << dprx_aux_id); // AUX clk enable [REG_000391]
#endif

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
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    if(bEnable)
    {
        // Set GPIO for DP AUX used
        msWriteByteMask(REG_DPRX_AUX_PM2_61_L + usRegOffsetAuxPM2ByID, 0, BIT2); // reg_en_gpio
    }
    else
    {
        // Set GPIO for general GPIO used
        msWriteByteMask(REG_DPRX_AUX_PM2_61_L + usRegOffsetAuxPM2ByID, BIT2, BIT2); // reg_en_gpio
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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    return (msReadByte(REG_DPRX_AUX_PM0_56_L + usRegOffsetAuxPM0ByID) & BIT4)? TRUE : FALSE;
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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    WORD usPreDelay;

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    usPreDelay = (msRead2Byte(REG_DPRX_AUX_PM0_7B_L + usRegOffsetAuxPM0ByID)&0x1FFF);

    if(usPreDelay != usDelayTime)
    {
        msWrite2Byte(REG_DPRX_AUX_PM0_7B_L + usRegOffsetAuxPM0ByID, usDelayTime);
    }

    if(usPreDelay > usDelayTime)
    {
        msWriteByteMask(REG_DPRX_AUX_PM0_08_L + usRegOffsetAuxPM0ByID, BIT6, BIT6);
    }

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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

    return msRead2Byte(REG_DPRX_AUX_PM0_7B_L + usRegOffsetAuxPM0ByID);
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
    #if (FPGA_Verification == 0)
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);
	WORD usEfuseAddress = (dprx_aux_id == DPRx_AUX_ID_0)? 0x130 : (dprx_aux_id == DPRx_AUX_ID_1)? 0x131 : (dprx_aux_id == DPRx_AUX_ID_2)? 0x132 : 0;

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

	// Project find trim data fail, so RD confirm to use 0x17 only
	msWrite2Byte(REG_DPRX_AUX_PM2_0F_L + usRegOffsetAuxPM2ByID, 0x170);

	// Efuse setting
	if((msEread_GetDataFromEfuse(usEfuseAddress) & BIT7) == BIT7)
	{
		msWrite2ByteMask(REG_DPRX_AUX_PM2_0F_L + usRegOffsetAuxPM2ByID, ((msEread_GetDataFromEfuse(usEfuseAddress) & (BIT0|BIT1|BIT2|BIT3|BIT4)) << 4), BIT4|BIT5|BIT6|BIT7|BIT8);
	}
    #else
    UNUSED(dprx_aux_id);
    #endif

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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    WORD usTrainingDebounce = DP_RX_TRAINING_DEBOUNCE_TIME;

	if(dprx_id == DPRx_ID_MAX || dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    while(usTrainingDebounce != 0)
    {
        usTrainingDebounce--;

        if((mhal_DPRx_CheckAuxIdle(dprx_aux_id) == TRUE)  &&
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

    if(bEnable) // Offline
	{
		msWriteByteMask(REG_DPRX_AUX_PM0_61_L + usRegOffsetAuxPM0ByID, BIT1, BIT1);
	}
	else // Online
	{
	    // When offline -> online switching, reset Aux async FIFO
        mhal_DPRx_ResetAsyncFIFO(dprx_id, dprx_aux_id);

		msWriteByteMask(REG_DPRX_AUX_PM0_61_L + usRegOffsetAuxPM0ByID, 0, BIT1);
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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    if(msReadByte(REG_DPRX_AUX_PM0_61_L + usRegOffsetAuxPM0ByID) & BIT1)
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
    WORD usRegOffsetAuxByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    if((mhal_DPRx_GetAuxValid(dprx_aux_id) == FALSE) &&
      ((msReadByte(REG_DPRX_AUX_PM0_74_H + usRegOffsetAuxByID) & (BIT3|BIT2|BIT1|BIT0)) == 0)) // AUX state -> idle
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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    if((mhal_DPRx_GetAuxValid(dprx_aux_id) == FALSE) &&
       (((msReadByte(REG_DPRX_AUX_PM0_74_L + usRegOffsetAuxPM0ByID) & (BIT5|BIT4|BIT3)) == BIT3) || // AUX state - > AUX_RECEIVE
       ((msReadByte(REG_DPRX_AUX_PM0_74_L + usRegOffsetAuxPM0ByID) & (BIT5|BIT4|BIT3)) == 0))) // AUX state -> IDLE
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
	WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
	BYTE ubTimeOutCounter = 100;
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET_DPCD0(dprx_aux_id);

    if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

	while(ubTimeOutCounter > 0)
	{
		if((msReadByte(REG_DPRX_AUX_PM0_56_L  + usRegOffsetAuxPM0ByID) & BIT4) == 0x0) // Make sure Aux idle
		{
			msWriteByteMask(REG_DPRX_AUX_PM0_2E_L + usRegOffsetAuxPM0ByID, 0, BIT0);
			mhal_DPRx_DELAY_NOP(100);
			msWriteByteMask(REG_DPRX_AUX_PM0_2E_L + usRegOffsetAuxPM0ByID, BIT0, BIT0);

			msWriteByteMask(REG_DPRX_AUX_PM0_0F_L + usRegOffsetAuxPM0ByID, BIT1, BIT1); // Reset
            mhal_DPRx_DELAY_NOP(10);
            msWriteByteMask(REG_DPRX_AUX_PM0_0F_L + usRegOffsetAuxPM0ByID, 0, BIT1); // Reset

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
    WORD usRegOffsetAUXByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);

    if(bEnable == TRUE)
    {
        msWriteByteMask(REG_DPRX_AUX_PM3_2A_H + usRegOffsetAUXByID, BIT3|BIT1, BIT3|BIT1); // aux will not reply
    }
    else
    {
        msWriteByteMask(REG_DPRX_AUX_PM3_2A_H + usRegOffsetAUXByID, 0, BIT3|BIT1);
    }
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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    // Address overwrite enable
    msWriteByteMask(REG_DPRX_AUX_PM0_67_H + usRegOffsetAuxPM0ByID, BIT4, BIT4);

    // 20 bit DPCD address
    msWriteByte(REG_DPRX_AUX_PM0_66_L + usRegOffsetAuxPM0ByID, ulDPCDAddress & 0xFF);
    msWriteByte(REG_DPRX_AUX_PM0_66_H + usRegOffsetAuxPM0ByID, (ulDPCDAddress >> 0x8) & 0xFF);
    msWriteByte(REG_DPRX_AUX_PM0_67_L + usRegOffsetAuxPM0ByID, (ulDPCDAddress >> 0x10) & 0xF);

    msWriteByte(REG_DPRX_AUX_PM0_68_H + usRegOffsetAuxPM0ByID, ucDATA);

    while((mhal_DPRx_CheckAuxIdle(dprx_aux_id) == FALSE) && (ubTimeOutLimt > 0))
    {
        mhal_DPRx_DELAY_NOP(25);
		ubTimeOutLimt--;
    }

    if(ubTimeOutLimt == 0)
    {
        printf("[DPRXAUX%d] MCU Write PM DPCD 0x%x FAIL aux not idle\r\n",dprx_aux_id,ulDPCDAddress);
    }

    msWriteByteMask(REG_DPRX_AUX_PM0_67_H + usRegOffsetAuxPM0ByID, BIT7, BIT7); // Trigger MUC write

    ubTimeOutLimt = 0x5;
    do
	{
		mhal_DPRx_DELAY_NOP(4);
		ubTimeOutLimt--;
    }while((msReadByte(REG_DPRX_AUX_PM0_67_H + usRegOffsetAuxPM0ByID) & BIT5) && (ubTimeOutLimt > 0));

	// Address overwrite disable
	msWriteByteMask(REG_DPRX_AUX_PM0_67_H + usRegOffsetAuxPM0ByID, 0, BIT4);
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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return 0;
	}

    // Address overwrite enable
    msWriteByteMask(REG_DPRX_AUX_PM0_67_H + usRegOffsetAuxPM0ByID, BIT4, BIT4);

    // 20 bit DPCD address
    msWriteByte(REG_DPRX_AUX_PM0_66_L + usRegOffsetAuxPM0ByID, ulDPCDAddress & 0xFF);
    msWriteByte(REG_DPRX_AUX_PM0_66_H + usRegOffsetAuxPM0ByID, (ulDPCDAddress >> 0x8) & 0xFF);
    msWriteByte(REG_DPRX_AUX_PM0_67_L + usRegOffsetAuxPM0ByID, (ulDPCDAddress >> 0x10) & 0xF);

    while((mhal_DPRx_CheckAuxIdle(dprx_aux_id) == FALSE) && (ubTimeOutLimt > 0))
    {
        mhal_DPRx_DELAY_NOP(25);
		ubTimeOutLimt--;
    }

    if(ubTimeOutLimt == 0)
    {
        printf("[DPRXAUX%d] MCU READ PM DPCD 0x%x FAIL aux not idle\r\n",dprx_aux_id,ulDPCDAddress);
    }

    msWriteByteMask(REG_DPRX_AUX_PM0_67_H+ usRegOffsetAuxPM0ByID, BIT6, BIT6); // Trigger MUC read

    ubTimeOutLimt = 0x5;

    do
	{
		mhal_DPRx_DELAY_NOP(4);
		ubTimeOutLimt--;
    }while((msReadByte(REG_DPRX_AUX_PM0_67_H + usRegOffsetAuxPM0ByID) & BIT5) && (ubTimeOutLimt > 0));

    Data = msReadByte(REG_DPRX_AUX_PM0_68_L + usRegOffsetAuxPM0ByID);

	// Address overwrite disable
	msWriteByteMask(REG_DPRX_AUX_PM0_67_H + usRegOffsetAuxPM0ByID, 0, BIT4);
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
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET_DPCD0(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

    // Address overwrite enable
    msWriteByteMask(REG_DPRX_DPCD0_76_H + usRegOffsetDPCD0ByID, BIT4, BIT4);

    // 20 bit DPCD address
    msWriteByte(REG_DPRX_DPCD0_75_L + usRegOffsetDPCD0ByID, ulDPCDAddress & 0xFF);
    msWriteByte(REG_DPRX_DPCD0_75_H + usRegOffsetDPCD0ByID, (ulDPCDAddress >> 0x8) & 0xFF);
    msWriteByte(REG_DPRX_DPCD0_76_L + usRegOffsetDPCD0ByID, (ulDPCDAddress >> 0x10) & 0xF);

    msWriteByte(REG_DPRX_DPCD0_77_H + usRegOffsetDPCD0ByID, ucDATA);

    while((mhal_DPRx_CheckAuxIdle(dprx_aux_id) == FALSE) && (ubTimeOutLimt > 0))
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
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET_DPCD0(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return 0;
	}

    // Address overwrite enable
    msWriteByteMask(REG_DPRX_DPCD0_76_H + usRegOffsetDPCD0ByID, BIT4, BIT4);

    // 20 bit DPCD address
    msWriteByte(REG_DPRX_DPCD0_75_L + usRegOffsetDPCD0ByID, ulDPCDAddress & 0xFF);
    msWriteByte(REG_DPRX_DPCD0_75_H + usRegOffsetDPCD0ByID, (ulDPCDAddress >> 0x8) & 0xFF);
    msWriteByte(REG_DPRX_DPCD0_76_L + usRegOffsetDPCD0ByID, (ulDPCDAddress >> 0x10) & 0xF);

    while((mhal_DPRx_CheckAuxIdle(dprx_aux_id) == FALSE) && (ubTimeOutLimt > 0))
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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    WORD usRegOffsetAuxPM1ByID = DP_REG_OFFSET_AUX_PM1(dprx_aux_id);
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);
    WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
    WORD usRegOffsetAuxPM4ByID = DP_REG_OFFSET_AUX_PM4(dprx_aux_id);
    WORD usRegOffsetAuxPMTopByID = DP_REG_OFFSET_AUX_PM_TOP(dprx_aux_id);
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET_DPCD0(dprx_aux_id);
    WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET_DPCD1(dprx_aux_id);
    WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET_TRANS_CTRL(dprx_aux_id);
	BYTE ucRegValue = 0x0;
	BOOL bIsOffline = mhal_DPRx_IsAuxAtOffLine(dprx_aux_id);

    switch(ulDPCDAddress)
    {
		case DPCD_00000:
		    return msReadByte(REG_DPRX_AUX_PM0_00_L + usRegOffsetAuxPM0ByID); // PM
            break;

		case DPCD_00001:
			return msReadByte(REG_DPRX_AUX_PM0_00_H + usRegOffsetAuxPM0ByID);
			break;

		case DPCD_00002:
		    ucRegValue = ((msReadByte(REG_DPRX_AUX_PM0_01_L + usRegOffsetAuxPM0ByID) & BIT7) +
            ((msReadByte(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID) & BIT1) << 5) +
			((msReadByte(REG_DPRX_AUX_PM0_01_L + usRegOffsetAuxPM0ByID) & BIT6) >> 1) +
			(msReadByte(REG_DPRX_AUX_PM0_01_L + usRegOffsetAuxPM0ByID) & (BIT4|BIT3|BIT2|BIT1|BIT0)));
			return ucRegValue;
			break;

		case DPCD_00003:
			ucRegValue = (((msReadByte(REG_DPRX_AUX_PM0_7E_H + usRegOffsetAuxPM0ByID) & BIT2) << 5) +
			((msReadByte(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID) & BIT2) << 4) +
			((msReadByte(REG_DPRX_AUX_PM0_02_L + usRegOffsetAuxPM0ByID) & BIT2) >> 1) +
			(msReadByte(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID) & BIT0));
			return ucRegValue;
			break;

		case DPCD_00004:
			ucRegValue = ((msReadByte(REG_DPRX_AUX_PM0_03_H + usRegOffsetAuxPM0ByID) & (BIT2|BIT1|BIT0)) << 5) +
			(msReadByte(REG_DPRX_AUX_PM0_03_H + usRegOffsetAuxPM0ByID) & (BIT4|BIT3)) +
			((msReadByte(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID) & BIT3) >> 3);
			return ucRegValue;
            break;

		case DPCD_00005:
			return msReadByte(REG_DPRX_AUX_PM0_76_H + usRegOffsetAuxPM0ByID);
			break;

		case DPCD_00006:
			return ((msReadByte(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID) & BIT7) >> 7); // DPCD 00006[0]
			break;

		case DPCD_00007:
			ucRegValue = ((msReadByte(REG_DPRX_AUX_PM0_02_L + usRegOffsetAuxPM0ByID) & BIT0) << 7) +
			(msReadByte(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID) & BIT6) +
			(msReadByte(REG_DPRX_AUX_PM0_77_H + usRegOffsetAuxPM0ByID) & 0x0F);
			return ucRegValue;
			break;

		case DPCD_00008:
			ucRegValue = ((msReadByte(REG_DPRX_AUX_PM2_62_L + usRegOffsetAuxPM2ByID) & (BIT2|BIT1|BIT0)) << 3) +
			(msReadByte(REG_DPRX_AUX_PM0_04_L + usRegOffsetAuxPM0ByID) & (BIT2|BIT1));
			return ucRegValue;
			break;

		case DPCD_00009:
			return msReadByte(REG_DPRX_AUX_PM0_04_H + usRegOffsetAuxPM0ByID);
			break;

		case DPCD_0000A:
			ucRegValue = (msReadByte(REG_DPRX_AUX_PM0_7C_L + usRegOffsetAuxPM0ByID) & (BIT7|BIT6|BIT5|BIT4|BIT3|BIT0)) +
			(msReadByte(REG_DPRX_AUX_PM0_05_L + usRegOffsetAuxPM0ByID) & (BIT2|BIT1));
			return ucRegValue;
			break;

		case DPCD_0000B:
			return msReadByte(REG_DPRX_AUX_PM0_05_H + usRegOffsetAuxPM0ByID);
			break;

		case DPCD_0000C:
			return msReadByte(REG_DPRX_AUX_PM0_08_H + usRegOffsetAuxPM0ByID);
			break;

		case DPCD_0000D:
			ucRegValue = ((msReadByte(REG_DPRX_AUX_PM0_01_L + usRegOffsetAuxPM0ByID) & BIT5) >> 2) +
			((msReadByte(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID) & BIT5) >> 4) +
			((msReadByte(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID) & BIT4) >> 4);
			return ucRegValue;
			break;

		case DPCD_0000E:
			return msReadByte(REG_DPRX_AUX_PM0_02_H + usRegOffsetAuxPM0ByID);
			break;

		case DPCD_0000F:
			ucRegValue = ((msReadByte(REG_DPRX_AUX_PM0_77_H + usRegOffsetAuxPM0ByID) & BIT5) >> 4) +
			((msReadByte(REG_DPRX_AUX_PM0_77_H + usRegOffsetAuxPM0ByID) & BIT6) >> 6);
			return ucRegValue;
			break;

		case DPCD_00010:
		    return msReadByte(REG_DPRX_AUX_PM1_10_L + usRegOffsetAuxPM1ByID);
		    break;

		case DPCD_00011:
		    return msReadByte(REG_DPRX_AUX_PM1_10_H + usRegOffsetAuxPM1ByID);
		    break;

		case DPCD_00012:
		    return msReadByte(REG_DPRX_AUX_PM1_11_L + usRegOffsetAuxPM1ByID);
		    break;

		case DPCD_00013:
		    return msReadByte(REG_DPRX_AUX_PM1_11_H + usRegOffsetAuxPM1ByID);
		    break;

		case DPCD_00014:
		    return msReadByte(REG_DPRX_AUX_PM1_12_L + usRegOffsetAuxPM1ByID);
		    break;

		case DPCD_00015:
		    return msReadByte(REG_DPRX_AUX_PM1_12_H + usRegOffsetAuxPM1ByID);
		    break;

		case DPCD_00016:
		    return msReadByte(REG_DPRX_AUX_PM1_13_L + usRegOffsetAuxPM1ByID);
		    break;

		case DPCD_00017:
		    return msReadByte(REG_DPRX_AUX_PM1_13_H + usRegOffsetAuxPM1ByID);
		    break;

        case DPCD_00018:
            return msReadByte(REG_DPRX_AUX_PM1_14_L + usRegOffsetAuxPM1ByID);
		    break;

	    case DPCD_00019:
	        return msReadByte(REG_DPRX_AUX_PM1_14_H + usRegOffsetAuxPM1ByID);
		    break;

	    case DPCD_0001A:
	        return msReadByte(REG_DPRX_AUX_PM1_15_L + usRegOffsetAuxPM1ByID);
		    break;

	    case DPCD_0001B:
	        return msReadByte(REG_DPRX_AUX_PM1_15_H + usRegOffsetAuxPM1ByID);
		    break;

	    case DPCD_0001C:
	        return msReadByte(REG_DPRX_AUX_PM1_16_L + usRegOffsetAuxPM1ByID);
		    break;

	    case DPCD_0001D:
	        return msReadByte(REG_DPRX_AUX_PM1_16_H + usRegOffsetAuxPM1ByID);
		    break;

	    case DPCD_0001E:
	        return msReadByte(REG_DPRX_AUX_PM1_17_L + usRegOffsetAuxPM1ByID);
		    break;

	    case DPCD_0001F:
		    return msReadByte(REG_DPRX_AUX_PM1_17_H + usRegOffsetAuxPM1ByID);
		    break;

		case DPCD_00020:
		    if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM3_00_L + usRegOffsetAuxPM3ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_03_L + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_00021:
            if(bIsOffline == TRUE)
            {
		        return ((msReadByte(REG_DPRX_AUX_PM0_77_H + usRegOffsetAuxPM0ByID) & BIT4) >> 4);  // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_03_H + usRegOffsetDPCD0ByID); // NPM
		    }
			break;

        case DPCD_00022:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM0_03_L + usRegOffsetAuxPM0ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_04_L + usRegOffsetDPCD0ByID); // NPM
		    }
			break;

	    case DPCD_00023:
		    return msReadByte(REG_DPRX_DPCD0_04_H + usRegOffsetDPCD0ByID);
            break;

		case DPCD_00024:
		    return msReadByte(REG_DPRX_DPCD0_05_L + usRegOffsetDPCD0ByID);
            break;

		case DPCD_00025:
		    return msReadByte(REG_DPRX_DPCD0_05_H + usRegOffsetDPCD0ByID);
            break;

		case DPCD_00026:
		    return msReadByte(REG_DPRX_DPCD0_06_L + usRegOffsetDPCD0ByID);
            break;

		case DPCD_00027:
		    return msReadByte(REG_DPRX_DPCD0_06_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_00028:
            return msReadByte(REG_DPRX_DPCD0_07_L + usRegOffsetDPCD0ByID);
            break;

	    case DPCD_00029:
	        return msReadByte(REG_DPRX_DPCD0_07_H + usRegOffsetDPCD0ByID);
            break;

	    case DPCD_0002A:
	        return msReadByte(REG_DPRX_DPCD0_08_L + usRegOffsetDPCD0ByID);
            break;

	    case DPCD_0002B:
	        return msReadByte(REG_DPRX_DPCD0_08_H + usRegOffsetDPCD0ByID);
            break;

	    case DPCD_0002C:
	        return msReadByte(REG_DPRX_DPCD0_09_L + usRegOffsetDPCD0ByID);
            break;

	    case DPCD_0002D:
		    return msReadByte(REG_DPRX_DPCD0_09_H + usRegOffsetDPCD0ByID);
            break;

		case DPCD_0002E:
			return msReadByte(REG_DPRX_AUX_PM0_51_L + usRegOffsetAuxPM0ByID);
			break;

        case DPCD_0002F:
			return msReadByte(REG_DPRX_AUX_PM0_51_H + usRegOffsetAuxPM0ByID);
			break;

	    case DPCD_00030:
			return msReadByte(REG_DPRX_AUX_PM0_31_L + usRegOffsetAuxPM0ByID);
			break;

	    case DPCD_00031:
			return msReadByte(REG_DPRX_AUX_PM0_31_H + usRegOffsetAuxPM0ByID);
			break;

		case DPCD_00032:
			return msReadByte(REG_DPRX_AUX_PM0_32_L + usRegOffsetAuxPM0ByID);
			break;

	    case DPCD_00033:
			return msReadByte(REG_DPRX_AUX_PM0_32_H + usRegOffsetAuxPM0ByID);
			break;

		case DPCD_00034:
			return msReadByte(REG_DPRX_AUX_PM0_33_L + usRegOffsetAuxPM0ByID);
			break;

	    case DPCD_00035:
			return msReadByte(REG_DPRX_AUX_PM0_33_H + usRegOffsetAuxPM0ByID);
			break;

		case DPCD_00036:
			return msReadByte(REG_DPRX_AUX_PM0_34_L + usRegOffsetAuxPM0ByID);
			break;

	    case DPCD_00037:
			return msReadByte(REG_DPRX_AUX_PM0_34_H + usRegOffsetAuxPM0ByID);
			break;

		case DPCD_00038:
			return msReadByte(REG_DPRX_AUX_PM0_35_L + usRegOffsetAuxPM0ByID);
			break;

		case DPCD_00039:
			return msReadByte(REG_DPRX_AUX_PM0_35_H + usRegOffsetAuxPM0ByID);
			break;

	    case DPCD_0003A:
			return msReadByte(REG_DPRX_AUX_PM0_36_L + usRegOffsetAuxPM0ByID);
			break;

		case DPCD_0003B:
			return msReadByte(REG_DPRX_AUX_PM0_36_H + usRegOffsetAuxPM0ByID);
			break;

	    case DPCD_0003C:
			return msReadByte(REG_DPRX_AUX_PM0_37_L + usRegOffsetAuxPM0ByID);
			break;

		case DPCD_0003D:
			return msReadByte(REG_DPRX_AUX_PM0_37_H + usRegOffsetAuxPM0ByID);
			break;

	    case DPCD_0003E:
			return msReadByte(REG_DPRX_AUX_PM0_38_L + usRegOffsetAuxPM0ByID);
			break;

	    case DPCD_0003F:
			return msReadByte(REG_DPRX_AUX_PM0_38_H + usRegOffsetAuxPM0ByID);
			break;

		case DPCD_00040:
			return msReadByte(REG_DPRX_AUX_PM3_01_L + usRegOffsetAuxPM3ByID);
			break;

	    case DPCD_00041:
			return msReadByte(REG_DPRX_AUX_PM3_01_H + usRegOffsetAuxPM3ByID);
			break;

		case DPCD_00042:
			return msReadByte(REG_DPRX_AUX_PM3_02_L + usRegOffsetAuxPM3ByID);
			break;

	    case DPCD_00043:
			return msReadByte(REG_DPRX_AUX_PM3_02_H + usRegOffsetAuxPM3ByID);
			break;

		case DPCD_00044:
			return msReadByte(REG_DPRX_AUX_PM3_03_L + usRegOffsetAuxPM3ByID);
			break;

	    case DPCD_00045:
			return msReadByte(REG_DPRX_AUX_PM3_03_H + usRegOffsetAuxPM3ByID);
			break;

		case DPCD_00046:
			return msReadByte(REG_DPRX_AUX_PM3_04_L + usRegOffsetAuxPM3ByID);
			break;

	    case DPCD_00047:
			return msReadByte(REG_DPRX_AUX_PM3_04_H + usRegOffsetAuxPM3ByID);
			break;

		case DPCD_00048:
			return msReadByte(REG_DPRX_AUX_PM3_05_L + usRegOffsetAuxPM3ByID);
			break;

		case DPCD_00049:
			return msReadByte(REG_DPRX_AUX_PM3_05_H + usRegOffsetAuxPM3ByID);
			break;

	    case DPCD_0004A:
			return msReadByte(REG_DPRX_AUX_PM3_06_L + usRegOffsetAuxPM3ByID);
			break;

		case DPCD_0004B:
			return msReadByte(REG_DPRX_AUX_PM3_06_H + usRegOffsetAuxPM3ByID);
			break;

	    case DPCD_0004C:
			return msReadByte(REG_DPRX_AUX_PM3_07_L + usRegOffsetAuxPM3ByID);
			break;

		case DPCD_0004D:
			return msReadByte(REG_DPRX_AUX_PM3_07_H + usRegOffsetAuxPM3ByID);
			break;

	    case DPCD_0004E:
			return msReadByte(REG_DPRX_AUX_PM3_08_L + usRegOffsetAuxPM3ByID);
			break;

	    case DPCD_0004F:
			return msReadByte(REG_DPRX_AUX_PM3_08_H + usRegOffsetAuxPM3ByID);
			break;

		case DPCD_00060:
            ucRegValue = ((msReadByte(REG_DPRX_AUX_PM0_3F_L + usRegOffsetAuxPM0ByID) & (BIT6|BIT5|BIT4)) >> 3) +
            ((msReadByte(REG_DPRX_AUX_PM2_62_L + usRegOffsetAuxPM2ByID) & BIT3) >> 3);
            return ucRegValue;
            break;

		case DPCD_00061:
            ucRegValue = ((msReadByte(REG_DPRX_AUX_PM2_62_L + usRegOffsetAuxPM2ByID) & 0xF0) >> 4) +
            ((msReadByte(REG_DPRX_AUX_PM2_62_H + usRegOffsetAuxPM2ByID) & 0x0F) << 4);
            return ucRegValue;
            break;

		case DPCD_00062:
			return ((msReadByte(REG_DPRX_AUX_PM2_62_H + usRegOffsetAuxPM2ByID) & (BIT5|BIT4)) >> 4);
            break;

		case DPCD_00063:
            return msReadByte(REG_DPRX_AUX_PM2_63_L + usRegOffsetAuxPM2ByID);
            break;

		case DPCD_00064:
            return msReadByte(REG_DPRX_AUX_PM2_63_H + usRegOffsetAuxPM2ByID);
            break;

		case DPCD_00065:
            return (msReadByte(REG_DPRX_AUX_PM2_64_L + usRegOffsetAuxPM2ByID) & (BIT3|BIT2|BIT1|BIT0));
            break;

		case DPCD_00066:
            ucRegValue = ((msReadByte(REG_DPRX_AUX_PM0_3F_L + usRegOffsetAuxPM0ByID) & BIT7) >> 6) +
            ((msReadByte(REG_DPRX_AUX_PM2_62_H + usRegOffsetAuxPM2ByID) & BIT6) >> 6);
            return ucRegValue;
            break;

		case DPCD_00067:
            ucRegValue = ((msReadByte(REG_DPRX_AUX_PM2_64_L + usRegOffsetAuxPM2ByID) & 0xF0) >> 4) +
            ((msReadByte(REG_DPRX_AUX_PM2_64_H + usRegOffsetAuxPM2ByID) & 0x0F) << 4);
            return ucRegValue;
            break;

		case DPCD_00068:
            return msReadByte(REG_DPRX_AUX_PM2_65_L + usRegOffsetAuxPM2ByID);
            break;

		case DPCD_00069:
            return msReadByte(REG_DPRX_AUX_PM2_65_H + usRegOffsetAuxPM2ByID);
            break;

		case DPCD_0006A:
            return ((msReadByte(REG_DPRX_AUX_PM2_64_H + usRegOffsetAuxPM2ByID) & (BIT6|BIT5|BIT4)) >> 3);
            break;

		case DPCD_0006B:
            return msReadByte(REG_DPRX_AUX_PM2_66_L + usRegOffsetAuxPM2ByID);
            break;

		case DPCD_0006C:
            return msReadByte(REG_DPRX_AUX_PM2_66_H + usRegOffsetAuxPM2ByID);
            break;

		case DPCD_0006D:
            return (msReadByte(REG_DPRX_AUX_PM2_67_L + usRegOffsetAuxPM2ByID) & (BIT2|BIT1|BIT0));
            break;

		case DPCD_0006E:
            return msReadByte(REG_DPRX_AUX_PM3_13_H + usRegOffsetAuxPM3ByID);
            break;

		case DPCD_0006F:
            return msReadByte(REG_DPRX_AUX_PM3_14_L + usRegOffsetAuxPM3ByID);
            break;

		case DPCD_00090:
		    ucRegValue = ((msReadByte(REG_DPRX_AUX_PM0_62_L + usRegOffsetAuxPM0ByID) & 0x0F) << 4) +
			(msReadByte(REG_DPRX_AUX_PM2_67_H + usRegOffsetAuxPM2ByID) & 0x0F);
			return ucRegValue;
			break;

		case DPCD_00091:
            return msReadByte(REG_DPRX_AUX_PM3_09_H + usRegOffsetAuxPM3ByID);
            break;

		case DPCD_000A0:
		    return msReadByte(REG_DPRX_AUX_PM3_14_H + usRegOffsetAuxPM3ByID);
            break;

		case DPCD_000A1:
		    return msReadByte(REG_DPRX_AUX_PM3_15_L + usRegOffsetAuxPM3ByID);
            break;

		case DPCD_000A2:
		    return msReadByte(REG_DPRX_AUX_PM3_15_H + usRegOffsetAuxPM3ByID);
            break;

		case DPCD_000A3:
		    return msReadByte(REG_DPRX_AUX_PM3_16_L + usRegOffsetAuxPM3ByID);
            break;

		case DPCD_000A4:
		    return msReadByte(REG_DPRX_AUX_PM3_16_H + usRegOffsetAuxPM3ByID);
            break;

		case DPCD_000A5:
		    return msReadByte(REG_DPRX_AUX_PM3_17_L + usRegOffsetAuxPM3ByID);
            break;

		case DPCD_000A6:
		    return msReadByte(REG_DPRX_AUX_PM3_17_H + usRegOffsetAuxPM3ByID);
            break;

		case DPCD_000A7:
		    return msReadByte(REG_DPRX_AUX_PM3_18_L + usRegOffsetAuxPM3ByID);
            break;

        case DPCD_000A8:
            return msReadByte(REG_DPRX_AUX_PM3_18_H + usRegOffsetAuxPM3ByID);
            break;

        case DPCD_000A9:
            return msReadByte(REG_DPRX_AUX_PM3_19_L + usRegOffsetAuxPM3ByID);
            break;

        case DPCD_000AA:
            return msReadByte(REG_DPRX_AUX_PM3_19_H + usRegOffsetAuxPM3ByID);
            break;

        case DPCD_000AB:
            return msReadByte(REG_DPRX_AUX_PM3_20_L + usRegOffsetAuxPM3ByID);
            break;

        case DPCD_000AC:
            return msReadByte(REG_DPRX_AUX_PM3_20_H + usRegOffsetAuxPM3ByID);
            break;

        case DPCD_000AD:
            return msReadByte(REG_DPRX_AUX_PM3_21_L + usRegOffsetAuxPM3ByID);
            break;

        case DPCD_000AE:
            return msReadByte(REG_DPRX_AUX_PM3_21_H + usRegOffsetAuxPM3ByID);
            break;

        case DPCD_000B0:
            return msReadByte(REG_DPRX_AUX_PM3_10_L + usRegOffsetAuxPM3ByID);
            break;

        case DPCD_000B1:
            return msReadByte(REG_DPRX_AUX_PM3_10_H + usRegOffsetAuxPM3ByID);
            break;

        case DPCD_000B2:
            return msReadByte(REG_DPRX_AUX_PM3_11_L + usRegOffsetAuxPM3ByID);
            break;

        case DPCD_000B3:
            return msReadByte(REG_DPRX_AUX_PM3_11_H + usRegOffsetAuxPM3ByID);
            break;

        case DPCD_000B4:
            return msReadByte(REG_DPRX_AUX_PM3_12_L + usRegOffsetAuxPM3ByID);
            break;

        case DPCD_00100:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM0_70_L + usRegOffsetAuxPM0ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_20_L + usRegOffsetDPCD0ByID); // NPM
		    }
	 		break;

	 	case DPCD_00101:
	 	    if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM0_70_H + usRegOffsetAuxPM0ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_20_H + usRegOffsetDPCD0ByID); // NPM
		    }
	 		break;

	 	case DPCD_00102:
	 	    if(bIsOffline == TRUE)
            {
		        ucRegValue = (msReadByte(REG_DPRX_AUX_PM0_71_L + usRegOffsetAuxPM0ByID) & BIT5) +
    			(msReadByte(REG_DPRX_AUX_PM0_71_L + usRegOffsetAuxPM0ByID) & BIT4) +
    			(msReadByte(REG_DPRX_AUX_PM0_71_L + usRegOffsetAuxPM0ByID) & (BIT1|BIT0));
    			return ucRegValue;
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_21_L + usRegOffsetDPCD0ByID); // NPM
		    }
			break;

		case DPCD_00103:
		    if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM0_71_H + usRegOffsetAuxPM0ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_21_H + usRegOffsetDPCD0ByID); // NPM
		    }
	 		break;

	 	case DPCD_00104:
	 	    if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM0_72_L + usRegOffsetAuxPM0ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_22_L + usRegOffsetDPCD0ByID); // NPM
		    }
	 		break;

	 	case DPCD_00105:
	 	    if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM0_72_H + usRegOffsetAuxPM0ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_22_H + usRegOffsetDPCD0ByID); // NPM
		    }
	 		break;

	 	case DPCD_00106:
	 	    if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM0_73_L + usRegOffsetAuxPM0ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_23_L + usRegOffsetDPCD0ByID); // NPM
		    }
	 		break;

	 	case DPCD_00107:
			return msReadByte(REG_DPRX_AUX_PM2_30_L + usRegOffsetAuxPM2ByID);
	 		break;

	 	case DPCD_00108:
	 	    if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM0_3F_H + usRegOffsetAuxPM0ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD1_4E_H + usRegOffsetDPCD1ByID); // NPM
		    }

	 		break;

	 	case DPCD_00109:
			return msReadByte(REG_DPRX_DPCD0_24_H + usRegOffsetDPCD0ByID);
	 		break;

	 	case DPCD_0010A:
			return msReadByte(REG_DPRX_DPCD0_25_L + usRegOffsetDPCD0ByID);
	 		break;

	 	case DPCD_0010B:
			return msReadByte(REG_DPRX_DPCD1_46_L + usRegOffsetDPCD1ByID);
	 		break;

	 	case DPCD_0010C:
			return msReadByte(REG_DPRX_DPCD1_46_H + usRegOffsetDPCD1ByID);
	 		break;

	 	case DPCD_0010D:
			return msReadByte(REG_DPRX_DPCD1_47_L + usRegOffsetDPCD1ByID);
	 		break;

	 	case DPCD_0010E:
			return msReadByte(REG_DPRX_DPCD1_47_H + usRegOffsetDPCD1ByID);
	 		break;

	 	case DPCD_0010F:
			return msReadByte(REG_DPRX_DPCD0_27_H + usRegOffsetDPCD0ByID);
	 		break;

        case DPCD_00110:
			return msReadByte(REG_DPRX_DPCD0_28_L + usRegOffsetDPCD0ByID);
	 		break;

        case DPCD_00111:
			return msReadByte(REG_DPRX_AUX_PM2_52_L + usRegOffsetAuxPM2ByID);
	 		break;

	 	case DPCD_00112:
			return msReadByte(REG_DPRX_DPCD0_29_L + usRegOffsetDPCD0ByID);
	 		break;

	 	case DPCD_00113:
			return msReadByte(REG_DPRX_DPCD0_29_H + usRegOffsetDPCD0ByID);
	 		break;

	 	case DPCD_00114:
			return msReadByte(REG_DPRX_DPCD0_2A_L + usRegOffsetDPCD0ByID);
	 		break;

	 	case DPCD_00115:
            ucRegValue = ((msReadByte(REG_DPRX_AUX_PM2_6E_L + usRegOffsetAuxPM2ByID) & BIT1) << 3) +
            ((msReadByte(REG_DPRX_AUX_PM2_6E_L + usRegOffsetAuxPM2ByID) & BIT2) << 1);
            return ucRegValue;
            break;

        case DPCD_00117:
			return msReadByte(REG_DPRX_DPCD1_51_H + usRegOffsetDPCD1ByID);
	 		break;

	 	case DPCD_00118:
			return msReadByte(REG_DPRX_DPCD0_2A_H + usRegOffsetDPCD0ByID);
	 		break;

        case DPCD_00119:
			return msReadByte(REG_DPRX_AUX_PM2_30_H + usRegOffsetAuxPM2ByID);
	 		break;

	 	case DPCD_00120:
	 	    if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM0_64_L + usRegOffsetAuxPM0ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_2B_L + usRegOffsetDPCD0ByID); // NPM
		    }
	 		break;

	 	case DPCD_00121:
			return msReadByte(REG_DPRX_DPCD0_2B_H + usRegOffsetDPCD0ByID);
	 		break;

	 	case DPCD_00122:
			return msReadByte(REG_DPRX_DPCD0_2C_L + usRegOffsetDPCD0ByID);
	 		break;

	 	case DPCD_00123:
			return msReadByte(REG_DPRX_DPCD0_2C_H + usRegOffsetDPCD0ByID);
	 		break;

	 	case DPCD_00124:
			return msReadByte(REG_DPRX_DPCD0_2D_L + usRegOffsetDPCD0ByID);
	 		break;

	 	case DPCD_00125:
			return msReadByte(REG_DPRX_DPCD0_2D_H + usRegOffsetDPCD0ByID);
	 		break;

        case DPCD_00160:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM0_64_H + usRegOffsetAuxPM0ByID); // PM
		    }
		    else
		    {
                return (msReadByte(REG_DPRX_DPCD1_60_L + usRegOffsetDPCD1ByID) & BIT0); // NPM
		    }
	 		break;

	 	case DPCD_00161:
	 	    if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM3_22_L + usRegOffsetAuxPM3ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_2B_L + usRegOffsetDPCD0ByID); // NPM
		    }
	 		break;

	 	case DPCD_001A0:
			return msReadByte(REG_DPRX_AUX_PM2_52_H + usRegOffsetAuxPM2ByID);
	 		break;

	 	case DPCD_001A1:
			return msReadByte(REG_DPRX_AUX_PM2_53_L + usRegOffsetAuxPM2ByID);
	 		break;

        case DPCD_00200:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM0_77_L + usRegOffsetAuxPM0ByID); // PM
		    }
		    else
		    {
                ucRegValue = ((msReadByte(REG_DPRX_DPCD0_3F_L + usRegOffsetDPCD0ByID) & BIT6) << 1) +
                ((msReadByte(REG_DPRX_DPCD0_3F_L + usRegOffsetDPCD0ByID) & BIT7) >> 1) +
                (msReadByte(REG_DPRX_DPCD0_3F_L + usRegOffsetDPCD0ByID) & (BIT5|BIT4|BIT3|BIT2|BIT1|BIT0));
                return ucRegValue;
		    }
            break;

        case DPCD_00202:
            if(bIsOffline == TRUE)
            {
                return msReadByte(REG_DPRX_AUX_PM0_59_L + usRegOffsetAuxPM0ByID);
            }
            else
            {
                return msReadByte(REG_DPRX_TRANS_CTRL_17_L + usRegOffsetTransCTRLByID);
            }
		    break;

	    case DPCD_00203:
            if(bIsOffline == TRUE)
            {
	            return msReadByte(REG_DPRX_AUX_PM0_59_H + usRegOffsetAuxPM0ByID);
            }
            else
            {
                return msReadByte(REG_DPRX_TRANS_CTRL_17_H + usRegOffsetTransCTRLByID);
            }
		    break;

		case DPCD_00204:
            return msReadByte(REG_DPRX_AUX_PM0_5A_L + usRegOffsetAuxPM0ByID);
		    break;

		case DPCD_00205:
            return msReadByte(REG_DPRX_AUX_PM0_5A_H + usRegOffsetAuxPM0ByID);
		    break;

		case DPCD_00206:
            return msReadByte(REG_DPRX_AUX_PM0_5B_L + usRegOffsetAuxPM0ByID);
		    break;

		case DPCD_00207:
            return msReadByte(REG_DPRX_AUX_PM0_5B_H + usRegOffsetAuxPM0ByID);
		    break;

        case DPCD_0020C:
            return msReadByte(REG_DPRX_DPCD0_40_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_0020D:
            return msReadByte(REG_DPRX_DPCD0_41_L + usRegOffsetDPCD0ByID);
            break;

        case DPCD_0020E:
            return msReadByte(REG_DPRX_DPCD0_41_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_00218:
            return msReadByte(REG_DPRX_DPCD1_10_L + usRegOffsetDPCD1ByID);
            break;

        case DPCD_00219:
            return msReadByte(REG_DPRX_DPCD1_10_H + usRegOffsetDPCD1ByID);
            break;

        case DPCD_00220:
            return (msReadByte(REG_DPRX_DPCD1_11_L + usRegOffsetDPCD1ByID) & (BIT4|BIT3|BIT2|BIT1|BIT0));
            break;

        case DPCD_00221:
            return msReadByte(REG_DPRX_DPCD1_11_H + usRegOffsetDPCD1ByID);
            break;

        case DPCD_00222:
            return msReadByte(REG_DPRX_DPCD1_12_H + usRegOffsetDPCD1ByID);
            break;

        case DPCD_00223:
            return msReadByte(REG_DPRX_DPCD1_12_L + usRegOffsetDPCD1ByID);
            break;

        case DPCD_00224:
            return msReadByte(REG_DPRX_DPCD1_13_H + usRegOffsetDPCD1ByID);
            break;

        case DPCD_00225:
            return msReadByte(REG_DPRX_DPCD1_13_L + usRegOffsetDPCD1ByID);
            break;

        case DPCD_00226:
            return msReadByte(REG_DPRX_DPCD1_14_H + usRegOffsetDPCD1ByID);
            break;

        case DPCD_00227:
            return msReadByte(REG_DPRX_DPCD1_14_L + usRegOffsetDPCD1ByID);
            break;

        case DPCD_00228:
            return msReadByte(REG_DPRX_DPCD1_15_H + usRegOffsetDPCD1ByID);
            break;

        case DPCD_00229:
            return msReadByte(REG_DPRX_DPCD1_15_L + usRegOffsetDPCD1ByID);
            break;

        case DPCD_0022A:
            return msReadByte(REG_DPRX_DPCD1_16_L + usRegOffsetDPCD1ByID);
            break;

        case DPCD_0022B:
            return msReadByte(REG_DPRX_DPCD1_16_H + usRegOffsetDPCD1ByID);
            break;

        case DPCD_0022C:
            return msReadByte(REG_DPRX_DPCD1_17_L + usRegOffsetDPCD1ByID);
            break;

        case DPCD_0022D:
            return msReadByte(REG_DPRX_DPCD1_17_H + usRegOffsetDPCD1ByID);
            break;

        case DPCD_0022E:
            return msReadByte(REG_DPRX_DPCD1_18_H + usRegOffsetDPCD1ByID);
            break;

        case DPCD_0022F:
            return msReadByte(REG_DPRX_DPCD1_18_L + usRegOffsetDPCD1ByID);
            break;

        case DPCD_00230:
            return msReadByte(REG_DPRX_DPCD1_19_H + usRegOffsetDPCD1ByID);
            break;

        case DPCD_00231:
            return msReadByte(REG_DPRX_DPCD1_19_L + usRegOffsetDPCD1ByID);
            break;

        case DPCD_00232:
            return msReadByte(REG_DPRX_DPCD1_1A_L + usRegOffsetDPCD1ByID);
            break;

        case DPCD_00233:
            return (msReadByte(REG_DPRX_DPCD1_1A_H + usRegOffsetDPCD1ByID) & (BIT1|BIT0));
            break;

        case DPCD_00234:
            return msReadByte(REG_DPRX_DPCD1_1B_L + usRegOffsetDPCD1ByID);
            break;

        case DPCD_00248:
            return (msReadByte(REG_DPRX_DPCD1_1F_H + usRegOffsetDPCD1ByID) & (BIT2|BIT1|BIT0));
            break;

        case DPCD_00249:
            return msReadByte(REG_DPRX_DPCD0_43_L + usRegOffsetDPCD0ByID);
            break;

        case DPCD_0024A:
            return msReadByte(REG_DPRX_DPCD0_44_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_0024B:
            return msReadByte(REG_DPRX_DPCD0_44_L + usRegOffsetDPCD0ByID);
            break;

        case DPCD_00250:
            return msReadByte(REG_DPRX_DPCD0_44_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_00251:
            return msReadByte(REG_DPRX_DPCD0_45_L + usRegOffsetDPCD0ByID);
            break;

        case DPCD_00252:
            return msReadByte(REG_DPRX_DPCD0_45_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_00253:
            return msReadByte(REG_DPRX_DPCD0_46_L + usRegOffsetDPCD0ByID);
            break;

        case DPCD_00254:
            return msReadByte(REG_DPRX_DPCD0_46_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_00255:
            return msReadByte(REG_DPRX_DPCD0_47_L + usRegOffsetDPCD0ByID);
            break;

        case DPCD_00256:
            return msReadByte(REG_DPRX_DPCD0_47_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_00257:
            return msReadByte(REG_DPRX_DPCD0_48_L + usRegOffsetDPCD0ByID);
            break;

        case DPCD_00258:
            return msReadByte(REG_DPRX_DPCD0_48_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_00259:
            return msReadByte(REG_DPRX_DPCD0_49_L + usRegOffsetDPCD0ByID);
            break;

        case DPCD_0025A:
            return (msReadByte(REG_DPRX_DPCD1_2C_H + usRegOffsetDPCD1ByID) & BIT0);
            break;

        case DPCD_0025B:
            ucRegValue = ((msReadByte(REG_DPRX_DPCD1_2C_H + usRegOffsetDPCD1ByID) & (BIT5|BIT4)) >> 3) +
            ((msReadByte(REG_DPRX_DPCD1_2C_H + usRegOffsetDPCD1ByID) & BIT1) >> 1);
            return ucRegValue;
            break;

        case DPCD_00260:
            return msReadByte(REG_DPRX_DPCD0_49_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_00261:
            return msReadByte(REG_DPRX_DPCD0_4A_L + usRegOffsetDPCD0ByID);
            break;

        case DPCD_00262:
            return msReadByte(REG_DPRX_DPCD0_4A_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_00270:
            return msReadByte(REG_DPRX_DPCD0_4B_L + usRegOffsetDPCD0ByID);
            break;

        case DPCD_00271:
            return msReadByte(REG_DPRX_DPCD0_1B_L + usRegOffsetDPCD0ByID);
            break;

        case DPCD_00272:
            return msReadByte(REG_DPRX_DPCD0_1B_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_00273:
            return (msReadByte(REG_DPRX_DPCD0_1C_L + usRegOffsetDPCD0ByID) & 0x0F);
            break;

        case DPCD_00274:
            return ((msReadByte(REG_DPRX_DPCD0_1C_L + usRegOffsetDPCD0ByID) & 0xF0) >> 4);
            break;

        case DPCD_00275:
            return (msReadByte(REG_DPRX_DPCD0_1C_H + usRegOffsetDPCD0ByID) & 0x0F);
            break;

        case DPCD_00276:
            return ((msReadByte(REG_DPRX_DPCD0_1C_H + usRegOffsetDPCD0ByID) & 0xF0) >> 4);
            break;

        case DPCD_00277:
            return (msReadByte(REG_DPRX_DPCD0_1D_L + usRegOffsetDPCD0ByID) & 0x0F);
            break;

        case DPCD_00278:
            return ((msReadByte(REG_DPRX_DPCD0_1D_L + usRegOffsetDPCD0ByID) & 0xF0) >> 4);
            break;

        case DPCD_00279:
            return (msReadByte(REG_DPRX_DPCD0_1D_H + usRegOffsetDPCD0ByID) & 0x0F);
            break;

        case DPCD_0027A:
            return ((msReadByte(REG_DPRX_DPCD0_1D_H + usRegOffsetDPCD0ByID) & 0xF0) >> 4);
            break;

        case DPCD_00281:
            return msReadByte(REG_DPRX_DPCD0_4C_L + usRegOffsetDPCD0ByID);
            break;

        case DPCD_00282:
            return msReadByte(REG_DPRX_DPCD0_4C_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_00300:
            return msReadByte(REG_DPRX_AUX_PM2_42_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_00301:
            return msReadByte(REG_DPRX_AUX_PM2_42_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_00302:
            return msReadByte(REG_DPRX_AUX_PM2_43_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_00303:
            return msReadByte(REG_DPRX_AUX_PM2_43_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_00304:
            return msReadByte(REG_DPRX_AUX_PM2_44_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_00305:
            return msReadByte(REG_DPRX_AUX_PM2_45_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_00306:
            return msReadByte(REG_DPRX_AUX_PM2_46_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_00307:
            return msReadByte(REG_DPRX_AUX_PM2_46_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_00308:
            return msReadByte(REG_DPRX_AUX_PM2_47_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_00309:
            return msReadByte(REG_DPRX_AUX_PM2_47_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_0030A:
            return msReadByte(REG_DPRX_AUX_PM2_48_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_0030B:
            return msReadByte(REG_DPRX_AUX_PM2_48_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_00314:
            return msReadByte(REG_DPRX_AUX_PM2_35_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_00320:
            return msReadByte(REG_DPRX_AUX_PM1_1C_L + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00321:
            return msReadByte(REG_DPRX_AUX_PM1_1C_H + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00340:
            return msReadByte(REG_DPRX_AUX_PM1_20_L + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00341:
            return msReadByte(REG_DPRX_AUX_PM1_20_H + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00342:
            return msReadByte(REG_DPRX_AUX_PM1_21_L + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00343:
            return msReadByte(REG_DPRX_AUX_PM1_21_H + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00344:
            return msReadByte(REG_DPRX_AUX_PM1_22_L + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00345:
            return msReadByte(REG_DPRX_AUX_PM1_22_H + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00346:
            return msReadByte(REG_DPRX_AUX_PM1_23_L + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00347:
            return msReadByte(REG_DPRX_AUX_PM1_23_H + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00348:
            return msReadByte(REG_DPRX_AUX_PM1_24_L + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00349:
            return msReadByte(REG_DPRX_AUX_PM1_24_H + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_0034A:
            return msReadByte(REG_DPRX_AUX_PM1_25_L + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_0034B:
            return msReadByte(REG_DPRX_AUX_PM1_25_H + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_0034C:
            return msReadByte(REG_DPRX_AUX_PM1_26_L + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_0034D:
            return msReadByte(REG_DPRX_AUX_PM1_26_H + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_0034E:
            return msReadByte(REG_DPRX_AUX_PM1_27_L + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_0034F:
            return msReadByte(REG_DPRX_AUX_PM1_27_H + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00350:
            return msReadByte(REG_DPRX_AUX_PM1_28_L + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00351:
            return msReadByte(REG_DPRX_AUX_PM1_28_H + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00352:
            return msReadByte(REG_DPRX_AUX_PM1_29_L + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00353:
            return msReadByte(REG_DPRX_AUX_PM1_29_H + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00354:
            return msReadByte(REG_DPRX_AUX_PM1_2A_L + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00355:
            return msReadByte(REG_DPRX_AUX_PM1_2A_H + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00356:
            return msReadByte(REG_DPRX_AUX_PM1_2B_L + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00357:
            return msReadByte(REG_DPRX_AUX_PM1_2B_H + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00358:
            return msReadByte(REG_DPRX_AUX_PM1_2C_L + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00359:
            return msReadByte(REG_DPRX_AUX_PM1_2C_H + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00381:
            return msReadByte(REG_DPRX_AUX_PM1_2D_L + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00382:
            return msReadByte(REG_DPRX_AUX_PM1_2D_H + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00383:
            return msReadByte(REG_DPRX_AUX_PM1_2E_L + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_00384:
            return msReadByte(REG_DPRX_AUX_PM1_2E_H + usRegOffsetAuxPM1ByID);
            break;

	    case DPCD_0040F:
			return ((msReadByte(REG_DPRX_AUX_PM2_35_L + usRegOffsetAuxPM2ByID) & BIT5) >> 5);
			break;

	    case DPCD_00410:
            return msReadByte(REG_DPRX_PM_TOP_68_L + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00411:
            return msReadByte(REG_DPRX_PM_TOP_68_H + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00412:
            return msReadByte(REG_DPRX_PM_TOP_69_L + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00413:
            return msReadByte(REG_DPRX_PM_TOP_69_H + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00414:
            return msReadByte(REG_DPRX_PM_TOP_6A_L + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00415:
            return msReadByte(REG_DPRX_PM_TOP_6A_H + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00416:
            return msReadByte(REG_DPRX_PM_TOP_6B_L + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00417:
            return msReadByte(REG_DPRX_PM_TOP_6B_H + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00418:
            return msReadByte(REG_DPRX_PM_TOP_6C_L + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00419:
            return msReadByte(REG_DPRX_PM_TOP_6C_H + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_0041A:
            return msReadByte(REG_DPRX_PM_TOP_6D_L + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_0041B:
            return msReadByte(REG_DPRX_PM_TOP_6D_H + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_0041C:
            return msReadByte(REG_DPRX_PM_TOP_6E_L + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_0041D:
            return msReadByte(REG_DPRX_PM_TOP_6E_H + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_0041E:
            return msReadByte(REG_DPRX_PM_TOP_6F_L + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00420:
            return msReadByte(REG_DPRX_PM_TOP_70_L + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00421:
            return msReadByte(REG_DPRX_PM_TOP_70_H + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00422:
            return msReadByte(REG_DPRX_PM_TOP_71_L + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00423:
            return msReadByte(REG_DPRX_PM_TOP_71_H + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00424:
            return msReadByte(REG_DPRX_PM_TOP_72_L + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00425:
            return msReadByte(REG_DPRX_PM_TOP_72_H + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00426:
            return msReadByte(REG_DPRX_PM_TOP_73_L + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00427:
            return msReadByte(REG_DPRX_PM_TOP_73_H + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00428:
            return msReadByte(REG_DPRX_PM_TOP_74_L + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00429:
            return msReadByte(REG_DPRX_PM_TOP_74_H + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_0042A:
            return msReadByte(REG_DPRX_PM_TOP_75_L + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_0042B:
            return msReadByte(REG_DPRX_PM_TOP_75_H + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_0042C:
            return msReadByte(REG_DPRX_PM_TOP_76_L + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_0042D:
            return msReadByte(REG_DPRX_PM_TOP_76_H + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_0042E:
            return msReadByte(REG_DPRX_PM_TOP_77_L + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_0042F:
            return msReadByte(REG_DPRX_PM_TOP_77_H + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00430:
            return msReadByte(REG_DPRX_PM_TOP_78_L + usRegOffsetAuxPMTopByID);
            break;

		case DPCD_00431:
		    return msReadByte(REG_DPRX_PM_TOP_78_H + usRegOffsetAuxPMTopByID);
            break;

		case DPCD_00432:
		    return msReadByte(REG_DPRX_PM_TOP_79_L + usRegOffsetAuxPMTopByID);
            break;

		case DPCD_00433:
		    return msReadByte(REG_DPRX_PM_TOP_79_H + usRegOffsetAuxPMTopByID);
            break;

		case DPCD_00434:
		    return msReadByte(REG_DPRX_PM_TOP_7A_L + usRegOffsetAuxPMTopByID);
            break;

		case DPCD_00435:
		    return msReadByte(REG_DPRX_PM_TOP_7A_H + usRegOffsetAuxPMTopByID);
            break;

		case DPCD_00436:
		    return msReadByte(REG_DPRX_PM_TOP_7B_L + usRegOffsetAuxPMTopByID);
            break;

		case DPCD_00437:
		    return msReadByte(REG_DPRX_PM_TOP_7B_H + usRegOffsetAuxPMTopByID);
            break;

		case DPCD_00438:
		    return msReadByte(REG_DPRX_PM_TOP_7C_L + usRegOffsetAuxPMTopByID);
            break;

		case DPCD_00439:
		    return msReadByte(REG_DPRX_PM_TOP_7C_H + usRegOffsetAuxPMTopByID);
            break;

		case DPCD_0043A:
		    return msReadByte(REG_DPRX_PM_TOP_7D_L + usRegOffsetAuxPMTopByID);
            break;

		case DPCD_0043B:
		    return msReadByte(REG_DPRX_PM_TOP_7D_H + usRegOffsetAuxPMTopByID);
            break;

		case DPCD_0043C:
            return msReadByte(REG_DPRX_PM_TOP_7E_L + usRegOffsetAuxPMTopByID);
            break;

	    case DPCD_0043E:
			return msReadByte(REG_DPRX_PM_TOP_7F_L + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00440:
            return msReadByte(REG_DPRX_PM_TOP_67_L + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00441:
            return msReadByte(REG_DPRX_PM_TOP_67_H + usRegOffsetAuxPMTopByID);
            break;

        case DPCD_00600:
            return (msReadByte(REG_DPRX_AUX_PM0_39_L + usRegOffsetAuxPM0ByID) & (BIT2|BIT1|BIT0));
            break;

        case DPCD_00700:
            return msReadByte(REG_DPRX_AUX_PM1_18_L + usRegOffsetAuxPM1ByID);
            break;

        case DPCD_02002:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM0_77_L + usRegOffsetAuxPM0ByID); // PM
		    }
		    else
		    {
                ucRegValue = ((msReadByte(REG_DPRX_DPCD0_3F_L + usRegOffsetDPCD0ByID) & BIT6) << 1) +
                ((msReadByte(REG_DPRX_DPCD0_3F_L + usRegOffsetDPCD0ByID) & BIT7) >> 1) +
                (msReadByte(REG_DPRX_DPCD0_3F_L + usRegOffsetDPCD0ByID) & (BIT5|BIT4|BIT3|BIT2|BIT1|BIT0));
                return ucRegValue;
		    }
            break;

        case DPCD_02200:
            if(bIsOffline == TRUE)
            {
                return msReadByte(REG_DPRX_AUX_PM2_31_L + usRegOffsetAuxPM2ByID); // PM
		    }
		    else
		    {
		        return msReadByte(REG_DPRX_DPCD1_32_H + usRegOffsetDPCD1ByID);
		    }
            break;

        case DPCD_02201:
            return msReadByte(REG_DPRX_AUX_PM2_71_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_02202:
            return msReadByte(REG_DPRX_AUX_PM2_71_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_02203:
            return msReadByte(REG_DPRX_AUX_PM2_72_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_02204:
            return msReadByte(REG_DPRX_AUX_PM2_72_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_02205:
            return msReadByte(REG_DPRX_AUX_PM2_73_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_02206:
            return msReadByte(REG_DPRX_AUX_PM2_73_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_02207:
            return msReadByte(REG_DPRX_AUX_PM2_74_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_02208:
            return msReadByte(REG_DPRX_AUX_PM2_74_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_02209:
            return msReadByte(REG_DPRX_AUX_PM2_75_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_0220A:
            return msReadByte(REG_DPRX_AUX_PM2_75_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_0220B:
            return msReadByte(REG_DPRX_AUX_PM2_76_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_0220C:
            return msReadByte(REG_DPRX_AUX_PM2_76_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_0220D:
            return msReadByte(REG_DPRX_AUX_PM2_77_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_0220E:
            return msReadByte(REG_DPRX_AUX_PM2_77_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_0220F:
            return msReadByte(REG_DPRX_AUX_PM2_78_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_02210:
            return msReadByte(REG_DPRX_AUX_PM2_78_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_02211:
            return msReadByte(REG_DPRX_AUX_PM2_79_L + usRegOffsetAuxPM2ByID);
            break;

		case DPCD_02212:
            return msReadByte(REG_DPRX_AUX_PM2_68_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_02213:
            return msReadByte(REG_DPRX_AUX_PM2_68_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_02214:
            return msReadByte(REG_DPRX_AUX_PM0_52_L + usRegOffsetAuxPM0ByID);
            break;

        case DPCD_02215:
            return msReadByte(REG_DPRX_AUX_PM3_00_H + usRegOffsetAuxPM3ByID);
            break;

        case DPCD_02216:
            return msReadByte(REG_DPRX_AUX_PM3_09_L + usRegOffsetAuxPM3ByID);
            break;

		case DPCD_02217:
			return msReadByte(REG_DPRX_AUX_PM3_67_H + usRegOffsetAuxPM3ByID);
			break;

		case DPCD_02230:
            return msReadByte(REG_DPRX_DPCD0_0A_L + usRegOffsetDPCD0ByID);
            break;

        case DPCD_02231:
            return msReadByte(REG_DPRX_DPCD0_0A_H + usRegOffsetDPCD0ByID);
            break;

		case DPCD_02232:
		    return msReadByte(REG_DPRX_DPCD0_0B_L + usRegOffsetDPCD0ByID);
            break;

		case DPCD_02233:
		    return msReadByte(REG_DPRX_DPCD0_0B_H + usRegOffsetDPCD0ByID);
            break;

		case DPCD_02234:
		    return msReadByte(REG_DPRX_DPCD0_0C_L + usRegOffsetDPCD0ByID);
            break;

		case DPCD_02235:
		    return msReadByte(REG_DPRX_DPCD0_0C_H + usRegOffsetDPCD0ByID);
            break;

		case DPCD_02236:
		    return msReadByte(REG_DPRX_DPCD0_0D_L + usRegOffsetDPCD0ByID);
            break;

		case DPCD_02237:
		    return msReadByte(REG_DPRX_DPCD0_0D_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_02238:
            return msReadByte(REG_DPRX_DPCD0_0E_L + usRegOffsetDPCD0ByID);
            break;

	    case DPCD_02239:
	        return msReadByte(REG_DPRX_DPCD0_0E_H + usRegOffsetDPCD0ByID);
            break;

	    case DPCD_0223A:
	        return msReadByte(REG_DPRX_DPCD0_0F_L + usRegOffsetDPCD0ByID);
            break;

	    case DPCD_0223B:
	        return msReadByte(REG_DPRX_DPCD0_0F_H + usRegOffsetDPCD0ByID);
            break;

	    case DPCD_0223C:
	        return msReadByte(REG_DPRX_DPCD0_10_L + usRegOffsetDPCD0ByID);
            break;

	    case DPCD_0223D:
	        return msReadByte(REG_DPRX_DPCD0_10_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_0223E:
            return msReadByte(REG_DPRX_DPCD0_11_L + usRegOffsetDPCD0ByID);
            break;

	    case DPCD_0223F:
	        return msReadByte(REG_DPRX_DPCD0_11_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_02240:
            return msReadByte(REG_DPRX_DPCD0_12_L + usRegOffsetDPCD0ByID);
            break;

        case DPCD_02241:
            return msReadByte(REG_DPRX_DPCD0_12_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_02242:
            return msReadByte(REG_DPRX_DPCD0_13_L + usRegOffsetDPCD0ByID);
            break;

        case DPCD_02243:
            return msReadByte(REG_DPRX_DPCD0_13_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_02244:
            return msReadByte(REG_DPRX_DPCD0_14_L + usRegOffsetDPCD0ByID);
            break;

        case DPCD_02245:
            return msReadByte(REG_DPRX_DPCD0_14_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_02246:
            return msReadByte(REG_DPRX_DPCD0_15_L + usRegOffsetDPCD0ByID);
            break;

        case DPCD_02247:
            return msReadByte(REG_DPRX_DPCD0_15_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_02248:
            return msReadByte(REG_DPRX_DPCD0_16_L + usRegOffsetDPCD0ByID);
            break;

        case DPCD_02249:
            return msReadByte(REG_DPRX_DPCD0_16_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_0224A:
            return msReadByte(REG_DPRX_DPCD0_17_L + usRegOffsetDPCD0ByID);
            break;

        case DPCD_0224B:
            return msReadByte(REG_DPRX_DPCD0_17_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_0224C:
            return msReadByte(REG_DPRX_DPCD0_18_L + usRegOffsetDPCD0ByID);
            break;

        case DPCD_0224D:
            return msReadByte(REG_DPRX_DPCD0_18_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_0224E:
            return msReadByte(REG_DPRX_DPCD0_19_L + usRegOffsetDPCD0ByID);
            break;

        case DPCD_0224F:
            return msReadByte(REG_DPRX_DPCD0_19_H + usRegOffsetDPCD0ByID);
            break;

        case DPCD_02250:
		    return msReadByte(REG_DPRX_DPCD0_25_H + usRegOffsetDPCD0ByID);
            break;

		case DPCD_02251:
			return msReadByte(REG_DPRX_AUX_PM3_13_L + usRegOffsetAuxPM3ByID);
			break;

	    case DPCD_02260:
			return ((msReadByte(REG_DPRX_AUX_PM2_62_L + usRegOffsetAuxPM2ByID) & BIT3) >> 3);
			break;

	    case DPCD_02261:
			ucRegValue = ((msReadByte(REG_DPRX_AUX_PM2_62_L + usRegOffsetAuxPM2ByID) & 0xF0) >> 4) +
            ((msReadByte(REG_DPRX_AUX_PM2_62_H + usRegOffsetAuxPM2ByID) & 0x0F) << 4);
            return ucRegValue;
			break;

        case DPCD_02262:
            return ((msReadByte(REG_DPRX_AUX_PM2_62_H + usRegOffsetAuxPM2ByID) & (BIT5|BIT4)) >> 4);
            break;

		case DPCD_02263:
            return msReadByte(REG_DPRX_AUX_PM2_63_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_02264:
            return msReadByte(REG_DPRX_AUX_PM2_63_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_02265:
            return (msReadByte(REG_DPRX_AUX_PM2_64_L + usRegOffsetAuxPM2ByID) & 0x0F);
            break;

        case DPCD_02266:
            return ((msReadByte(REG_DPRX_AUX_PM2_62_H + usRegOffsetAuxPM2ByID) & BIT6) >> 6);
            break;

        case DPCD_02267:
            ucRegValue = ((msReadByte(REG_DPRX_AUX_PM2_64_L + usRegOffsetAuxPM2ByID) & 0xF0) >> 4) +
            ((msReadByte(REG_DPRX_AUX_PM2_64_H + usRegOffsetAuxPM2ByID) & 0x0F) << 4);
            return ucRegValue;
            break;

        case DPCD_02268:
            return msReadByte(REG_DPRX_AUX_PM2_65_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_02269:
            return msReadByte(REG_DPRX_AUX_PM2_65_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_0226A:
            return msReadByte(REG_DPRX_AUX_PM3_27_H + usRegOffsetAuxPM3ByID);
            break;

        case DPCD_0226B:
            return msReadByte(REG_DPRX_AUX_PM2_66_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_0226C:
            return msReadByte(REG_DPRX_AUX_PM2_66_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_0226D:
            return (msReadByte(REG_DPRX_AUX_PM2_67_L + usRegOffsetAuxPM2ByID) & (BIT2|BIT1|BIT0));
            break;

        case DPCD_0226E:
            return msReadByte(REG_DPRX_AUX_PM3_29_H + usRegOffsetAuxPM3ByID);
            break;

        case DPCD_0226F:
            return ((msReadByte(REG_DPRX_AUX_PM2_67_L + usRegOffsetAuxPM2ByID) & (BIT6|BIT5|BIT4)) >> 4);
            break;

        case DPCD_02270:
			return ((msReadByte(REG_DPRX_AUX_PM2_62_L + usRegOffsetAuxPM2ByID) & BIT3) >> 3);
			break;

        case DPCD_02271:
            ucRegValue = ((msReadByte(REG_DPRX_AUX_PM2_62_L + usRegOffsetAuxPM2ByID) & 0xF0) >> 4) +
            ((msReadByte(REG_DPRX_AUX_PM2_62_H + usRegOffsetAuxPM2ByID) & 0x0F) << 4);
            return ucRegValue;
            break;

        case DPCD_02272:
            return ((msReadByte(REG_DPRX_AUX_PM2_62_H + usRegOffsetAuxPM2ByID) & (BIT5|BIT4)) >> 4);
            break;

		case DPCD_02273:
            return msReadByte(REG_DPRX_AUX_PM2_63_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_02300:
            return msReadByte(REG_DPRX_AUX_PM4_12_L + usRegOffsetAuxPM4ByID);
            break;

        case DPCD_02301:
            return msReadByte(REG_DPRX_AUX_PM4_12_H + usRegOffsetAuxPM4ByID);
            break;

		case DPCD_02302:
		    return msReadByte(REG_DPRX_AUX_PM4_13_L + usRegOffsetAuxPM4ByID);
            break;

		case DPCD_02303:
		    return msReadByte(REG_DPRX_AUX_PM4_13_H + usRegOffsetAuxPM4ByID);
            break;

		case DPCD_02304:
		    return msReadByte(REG_DPRX_AUX_PM4_14_L + usRegOffsetAuxPM4ByID);
            break;

		case DPCD_02305:
		    return msReadByte(REG_DPRX_AUX_PM4_14_H + usRegOffsetAuxPM4ByID);
            break;

		case DPCD_02306:
		    return msReadByte(REG_DPRX_AUX_PM4_15_L + usRegOffsetAuxPM4ByID);
            break;

		case DPCD_02307:
		    return msReadByte(REG_DPRX_AUX_PM4_15_H + usRegOffsetAuxPM4ByID);
            break;

        case DPCD_02308:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM4_1B_H + usRegOffsetAuxPM4ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD1_3E_L + usRegOffsetDPCD1ByID); // NPM
		    }
            break;

		case DPCD_02309:
		    if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM4_1C_L + usRegOffsetAuxPM4ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD1_3E_H + usRegOffsetDPCD1ByID); // NPM
		    }
            break;

        case DPCD_0230A:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM4_1C_H + usRegOffsetAuxPM4ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD1_3F_L + usRegOffsetDPCD1ByID); // NPM
		    }
            break;

		case DPCD_0230B:
		    if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM4_1D_L + usRegOffsetAuxPM4ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD1_3F_H + usRegOffsetDPCD1ByID); // NPM
		    }
            break;

        case DPCD_0230C:
            if(bIsOffline == TRUE)
            {
                return msReadByte(REG_DPRX_AUX_PM4_16_L + usRegOffsetAuxPM4ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_68_H + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_0230D:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM4_16_H + usRegOffsetAuxPM4ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_69_L + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_0230E:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM4_17_L + usRegOffsetAuxPM4ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_69_H + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_0230F:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM4_17_H + usRegOffsetAuxPM4ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_6A_L + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_02310:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM4_18_L + usRegOffsetAuxPM4ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_6A_H + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_02311:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM4_18_H + usRegOffsetAuxPM4ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_6B_L + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_02312:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM4_19_L + usRegOffsetAuxPM4ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_6B_H + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_02313:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM4_19_H + usRegOffsetAuxPM4ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_6C_L + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_02314:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM4_1A_L + usRegOffsetAuxPM4ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_6C_H + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_02315:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM4_1A_H + usRegOffsetAuxPM4ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_6D_L + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_02316:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM4_1B_L + usRegOffsetAuxPM4ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_6D_H + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_03000:
            return msReadByte(REG_DPRX_AUX_PM2_5C_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_03001:
            return msReadByte(REG_DPRX_AUX_PM2_5C_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_03030:
            return msReadByte(REG_DPRX_DPCD1_41_L + usRegOffsetDPCD1ByID);
            break;

        case DPCD_03031:
            return msReadByte(REG_DPRX_DPCD1_41_H + usRegOffsetDPCD1ByID);
            break;

        case DPCD_03032:
            return msReadByte(REG_DPRX_DPCD1_42_L + usRegOffsetDPCD1ByID);
            break;

        case DPCD_03033:
            return msReadByte(REG_DPRX_DPCD1_42_H + usRegOffsetDPCD1ByID);
            break;

        case DPCD_03034:
            return msReadByte(REG_DPRX_DPCD1_43_L + usRegOffsetDPCD1ByID);
            break;

        case DPCD_03050:
		    return msReadByte(REG_DPRX_DPCD1_43_H + usRegOffsetDPCD1ByID);
            break;

        case DPCD_03051:
            return msReadByte(REG_DPRX_DPCD1_44_L + usRegOffsetDPCD1ByID);
            break;

        case DPCD_03052:
		    return msReadByte(REG_DPRX_DPCD1_44_H + usRegOffsetDPCD1ByID);
            break;

        case DPCD_03054:
            return msReadByte(REG_DPRX_DPCD1_61_L + usRegOffsetDPCD1ByID);
            break;

        case DPCD_03055:
            return msReadByte(REG_DPRX_DPCD1_61_H + usRegOffsetDPCD1ByID);
            break;

        case DPCD_03056:
            return msReadByte(REG_DPRX_DPCD1_62_L + usRegOffsetDPCD1ByID);
            break;

        case DPCD_03057:
		    return msReadByte(REG_DPRX_DPCD1_62_H + usRegOffsetDPCD1ByID);
            break;

        case DPCD_03058:
            return msReadByte(REG_DPRX_DPCD1_63_L + usRegOffsetDPCD1ByID);
            break;

        case DPCD_03059:
		    return msReadByte(REG_DPRX_DPCD1_63_H + usRegOffsetDPCD1ByID);
            break;

        case DPCD_68000:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM2_2A_L + usRegOffsetAuxPM2ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD1_22_L + usRegOffsetDPCD1ByID); // NPM
		    }
            break;

        case DPCD_68001:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM2_2A_H + usRegOffsetAuxPM2ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD1_22_H + usRegOffsetDPCD1ByID); // NPM
		    }
            break;

		case DPCD_68002:
		    if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM2_2B_L + usRegOffsetAuxPM2ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD1_23_L + usRegOffsetDPCD1ByID); // NPM
		    }
            break;

		case DPCD_68003:
		    if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM2_2B_H + usRegOffsetAuxPM2ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD1_23_H + usRegOffsetDPCD1ByID); // NPM
		    }
            break;

		case DPCD_68004:
		    if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM2_2C_L + usRegOffsetAuxPM2ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD1_24_L + usRegOffsetDPCD1ByID); // NPM
		    }
            break;

	    case DPCD_68005:
            return msReadByte(REG_DPRX_AUX_PM2_29_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_68006:
            return msReadByte(REG_DPRX_AUX_PM2_29_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_68007:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM2_22_L + usRegOffsetAuxPM2ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_60_H + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_68008:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM2_22_H + usRegOffsetAuxPM2ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_61_L + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_68009:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM2_23_L + usRegOffsetAuxPM2ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_61_H + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_6800A:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM2_23_H + usRegOffsetAuxPM2ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_62_L + usRegOffsetDPCD0ByID); // NPM
		    }

            break;

        case DPCD_6800B:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM2_24_L + usRegOffsetAuxPM2ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_62_H + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_6800C:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM2_25_L + usRegOffsetAuxPM2ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_63_L + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_6800D:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM2_25_H + usRegOffsetAuxPM2ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_63_H + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_6800E:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM2_26_L + usRegOffsetAuxPM2ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_64_L + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_6800F:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM2_26_H + usRegOffsetAuxPM2ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_64_H + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_68010:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM2_27_L + usRegOffsetAuxPM2ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_65_L + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_68011:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM2_27_H + usRegOffsetAuxPM2ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_65_H + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_68012:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM2_28_L + usRegOffsetAuxPM2ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_66_L + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_68013:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM2_28_H + usRegOffsetAuxPM2ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD0_66_H + usRegOffsetDPCD0ByID); // NPM
		    }
            break;

        case DPCD_68014:
            return msReadByte(REG_DPRX_AUX_PM2_10_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_68015:
            return msReadByte(REG_DPRX_AUX_PM2_10_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_68016:
            return msReadByte(REG_DPRX_AUX_PM2_11_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_68017:
            return msReadByte(REG_DPRX_AUX_PM2_11_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_68018:
            return msReadByte(REG_DPRX_AUX_PM2_12_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_68019:
            return msReadByte(REG_DPRX_AUX_PM2_12_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_6801A:
            return msReadByte(REG_DPRX_AUX_PM2_13_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_6801B:
            return msReadByte(REG_DPRX_AUX_PM2_13_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_6801C:
            return msReadByte(REG_DPRX_AUX_PM2_14_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_6801D:
            return msReadByte(REG_DPRX_AUX_PM2_14_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_6801E:
            return msReadByte(REG_DPRX_AUX_PM2_15_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_6801F:
            return msReadByte(REG_DPRX_AUX_PM2_15_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_68020:
            return msReadByte(REG_DPRX_AUX_PM2_16_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_68021:
            return msReadByte(REG_DPRX_AUX_PM2_16_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_68022:
            return msReadByte(REG_DPRX_AUX_PM2_17_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_68023:
            return msReadByte(REG_DPRX_AUX_PM2_17_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_68024:
            return msReadByte(REG_DPRX_AUX_PM2_18_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_68025:
            return msReadByte(REG_DPRX_AUX_PM2_18_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_68026:
            return msReadByte(REG_DPRX_AUX_PM2_19_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_68027:
            return msReadByte(REG_DPRX_AUX_PM2_19_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_68028:
            if(bIsOffline == TRUE)
            {
		        ucRegValue = ((msReadByte(REG_DPRX_AUX_PM2_1E_H + usRegOffsetAuxPM2ByID) & BIT0) << 1) +
			    ((msReadByte(REG_DPRX_AUX_PM2_21_L + usRegOffsetAuxPM2ByID) & BIT2) >> 2);
			    return ucRegValue;
		    }
		    else
		    {
                return ((msReadByte(REG_DPRX_DPCD1_48_L + usRegOffsetDPCD1ByID) &  (BIT2|BIT1)) >> 1); // NPM
		    }
            break;

        case DPCD_68029:
            if(bIsOffline == TRUE)
            {
		        ucRegValue = (msReadByte(REG_DPRX_AUX_PM2_20_L + usRegOffsetAuxPM2ByID) & BIT3) +
    			((msReadByte(REG_DPRX_AUX_PM2_21_L + usRegOffsetAuxPM2ByID) & BIT1) << 1) +
    			((msReadByte(REG_DPRX_AUX_PM2_21_L + usRegOffsetAuxPM2ByID) & BIT0) << 1) +
    			((msReadByte(REG_DPRX_AUX_PM2_20_L + usRegOffsetAuxPM2ByID) & BIT6) >> 6);
    			return ucRegValue;
		    }
		    else
		    {
                ucRegValue = ((msReadByte(REG_DPRX_DPCD0_67_L + usRegOffsetDPCD0ByID) & BIT0) << 3) +
                ((msReadByte(REG_DPRX_DPCD0_67_L + usRegOffsetDPCD0ByID) & BIT2) >> 2);
                return ucRegValue;
		    }
            break;

        case DPCD_6802A:
            return msReadByte(REG_DPRX_AUX_PM2_1A_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_6802B:
            return msReadByte(REG_DPRX_AUX_PM2_1A_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_E0000:
            return msReadByte(REG_DPRX_AUX_PM2_6A_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_E0001:
            return msReadByte(REG_DPRX_AUX_PM2_6A_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_E0002:
            return msReadByte(REG_DPRX_AUX_PM2_6B_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_E0003:
            return msReadByte(REG_DPRX_AUX_PM2_6B_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_E0004:
            return msReadByte(REG_DPRX_AUX_PM2_6C_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_E0005:
            return msReadByte(REG_DPRX_AUX_PM2_6C_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_E0006:
            return msReadByte(REG_DPRX_AUX_PM2_6D_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_E0007:
            return msReadByte(REG_DPRX_AUX_PM2_6D_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_E0008:
            return msReadByte(REG_DPRX_AUX_PM2_6E_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_E0009:
            return msReadByte(REG_DPRX_AUX_PM2_6E_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_E000B:
            return msReadByte(REG_DPRX_AUX_PM2_6F_H + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_E000D:
            return (msReadByte(REG_DPRX_AUX_PM3_33_L + usRegOffsetAuxPM3ByID) & (BIT6|BIT0));
            break;

        case DPCD_F0000:
            ucRegValue = ((msReadByte(REG_DPRX_AUX_PM2_7A_L + usRegOffsetAuxPM2ByID) & 0x0F) << 4) +
            ((msReadByte(REG_DPRX_AUX_PM2_7E_H + usRegOffsetAuxPM2ByID) & 0xF0) >> 4);
            return ucRegValue;
            break;

        case DPCD_F0001:
            return msReadByte(REG_DPRX_AUX_PM0_00_H + usRegOffsetAuxPM0ByID);
            break;

        case DPCD_F0002:
            return msReadByte(REG_DPRX_AUX_PM0_7F_H + usRegOffsetAuxPM0ByID);
            break;

        case DPCD_F0003:
            return msReadByte(REG_DPRX_AUX_PM0_7A_H + usRegOffsetAuxPM0ByID);
            break;

        case DPCD_F0004:
            return msReadByte(REG_DPRX_AUX_PM2_69_L + usRegOffsetAuxPM2ByID);
            break;

        case DPCD_FFF00:
            return ((msReadByte(REG_DPRX_AUX_PM0_75_L + usRegOffsetAuxPM0ByID) & BIT1) >> 1);
            break;

        case DPCD_FFF01:
            return (msReadByte(REG_DPRX_AUX_PM0_76_L + usRegOffsetAuxPM0ByID) & BIT0);
            break;

        case DPCD_FFFFC:
            return msReadByte(REG_DPRX_AUX_PM0_16_L + usRegOffsetAuxPM0ByID);
            break;

        case DPCD_FFFFD:
            return msReadByte(REG_DPRX_AUX_PM0_16_H + usRegOffsetAuxPM0ByID);
            break;

        case DPCD_FFFFE:
            return msReadByte(REG_DPRX_AUX_PM0_17_L + usRegOffsetAuxPM0ByID);
            break;

        case DPCD_FFFFF:
            if(bIsOffline == TRUE)
            {
		        return msReadByte(REG_DPRX_AUX_PM0_17_H + usRegOffsetAuxPM0ByID); // PM
		    }
		    else
		    {
                return msReadByte(REG_DPRX_DPCD1_0F_L + usRegOffsetDPCD1ByID); // NPM
		    }
            break;

		default:
		    mhal_DPRx_MCUReadPMDPCD(dprx_aux_id, ulDPCDAddress);
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
void mhal_DPRx_SetDPCDValueByRIU(DPRx_AUX_ID dprx_aux_id, DWORD ulDPCDAddress, BYTE ucValue, DPRx_RIU_DPCD_TYPE dp_riu_DPCD_type_id)
{
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    WORD usRegOffsetAuxPM1ByID = DP_REG_OFFSET_AUX_PM1(dprx_aux_id);
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);
    WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
    WORD usRegOffsetAuxPM4ByID = DP_REG_OFFSET_AUX_PM4(dprx_aux_id);
    WORD usRegOffsetAuxPMTopByID = DP_REG_OFFSET_AUX_PM_TOP(dprx_aux_id);
    WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET_DPCD0(dprx_aux_id);
    WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET_DPCD1(dprx_aux_id);

    switch(ulDPCDAddress)
    {
		case DPCD_00000:
            msWriteByte(REG_DPRX_AUX_PM0_00_L + usRegOffsetAuxPM0ByID, ucValue);
            break;

		case DPCD_00001:
			msWriteByte(REG_DPRX_AUX_PM0_00_H + usRegOffsetAuxPM0ByID, ucValue);
			break;

		case DPCD_00002:
            msWriteByteMask(REG_DPRX_AUX_PM0_01_L + usRegOffsetAuxPM0ByID, (ucValue & BIT7), BIT7); // DPCD 00002[7]
			msWriteByteMask(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID, (ucValue & BIT6) >> 5, BIT1); // DPCD 00002[6]
			msWriteByteMask(REG_DPRX_AUX_PM0_01_L + usRegOffsetAuxPM0ByID, (ucValue & BIT5) << 1, BIT6); // DPCD 00002[5]
			msWriteByteMask(REG_DPRX_AUX_PM0_01_L + usRegOffsetAuxPM0ByID, (ucValue & (BIT4|BIT3|BIT2|BIT1|BIT0)), BIT4|BIT3|BIT2|BIT1|BIT0); // DPCD 00002[4:0]
			break;

		case DPCD_00003:
			msWriteByteMask(REG_DPRX_AUX_PM0_7E_H + usRegOffsetAuxPM0ByID, (ucValue & BIT7) >> 5, BIT2); // DPCD 00003[7]
			msWriteByteMask(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID, (ucValue & BIT6) >> 4, BIT2); // DPCD 00003[6]
			msWriteByteMask(REG_DPRX_AUX_PM0_02_L + usRegOffsetAuxPM0ByID, (ucValue & BIT1) << 1, BIT2); // DPCD 00003[1]
			msWriteByteMask(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID, (ucValue & BIT0), BIT0); // DPCD 00003[0]
			break;

		case DPCD_00004:
			msWriteByteMask(REG_DPRX_AUX_PM0_03_H + usRegOffsetAuxPM0ByID, (ucValue & (BIT7|BIT6|BIT5)) >> 5, (BIT2|BIT1|BIT0)); // DPCD 00004[7:5]
			msWriteByteMask(REG_DPRX_AUX_PM0_03_H + usRegOffsetAuxPM0ByID, (ucValue & (BIT4|BIT3)), (BIT4|BIT3)); // DPCD 00004[4:3]
			msWriteByteMask(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID, (ucValue & BIT0) << 3, BIT3); // DPCD 00004[0]
			break;

		case DPCD_00005:
			msWriteByte(REG_DPRX_AUX_PM0_76_H + usRegOffsetAuxPM0ByID, ucValue);
			break;

		case DPCD_00006:
			msWriteByteMask(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID, (ucValue & BIT0) << 7, BIT7); // DPCD 00006[0]
			break;

		case DPCD_00007:
			msWriteByteMask(REG_DPRX_AUX_PM0_02_L + usRegOffsetAuxPM0ByID, (ucValue & BIT7) >> 7, BIT0); // DPCD 00007[7] OUI Support
			msWriteByteMask(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID, (ucValue & BIT6), BIT6); // DPCD 00007[6] MSA_TIMING_PAR_IGNORED
			msWriteByteMask(REG_DPRX_AUX_PM0_77_H + usRegOffsetAuxPM0ByID, (ucValue & 0x0F), BIT3|BIT2|BIT1|BIT0); // DPCD 00007[3:0] DFP_COUNT
			break;

		case DPCD_00008:
			msWriteByteMask(REG_DPRX_AUX_PM2_62_L + usRegOffsetAuxPM2ByID, (ucValue & (BIT5|BIT4|BIT3)) >> 3, (BIT2|BIT1|BIT0)); // DPCD 00008[5:3]
			msWriteByteMask(REG_DPRX_AUX_PM0_04_L + usRegOffsetAuxPM0ByID, (ucValue & (BIT2|BIT1)), (BIT2|BIT1)); // DPCD 00008[2:1]
			break;

		case DPCD_00009:
			msWriteByte(REG_DPRX_AUX_PM0_04_H + usRegOffsetAuxPM0ByID, ucValue);
			break;

		case DPCD_0000A:
			msWriteByteMask(REG_DPRX_AUX_PM0_7C_L + usRegOffsetAuxPM0ByID, (ucValue & (BIT7|BIT6|BIT5|BIT4|BIT3|BIT0)), (BIT7|BIT6|BIT5|BIT4|BIT3|BIT0)); // DPCD 0000A[7:3],[0]
			msWriteByteMask(REG_DPRX_AUX_PM0_05_L + usRegOffsetAuxPM0ByID, (ucValue & BIT2), BIT2); // DPCD 0000A[2]
			msWriteByteMask(REG_DPRX_AUX_PM0_05_L + usRegOffsetAuxPM0ByID, (ucValue & BIT1), BIT1); // DPCD 0000A[1]
			break;

		case DPCD_0000B:
			msWriteByte(REG_DPRX_AUX_PM0_05_H + usRegOffsetAuxPM0ByID, ucValue);
			break;

		case DPCD_0000C:
			msWriteByte(REG_DPRX_AUX_PM0_08_H + usRegOffsetAuxPM0ByID, ucValue);
			break;

		case DPCD_0000D:
			msWriteByteMask(REG_DPRX_AUX_PM0_01_L + usRegOffsetAuxPM0ByID, (ucValue & BIT3) << 2, BIT5); // DPCD 0000D[3]
			msWriteByteMask(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID, (ucValue & BIT1) << 4, BIT5); // DPCD 0000D[1]
			msWriteByteMask(REG_DPRX_AUX_PM0_01_H + usRegOffsetAuxPM0ByID, (ucValue & BIT0) << 4, BIT4); // DPCD 0000D[0]
			break;

		case DPCD_0000E:
			msWriteByte(REG_DPRX_AUX_PM0_02_H + usRegOffsetAuxPM0ByID, ucValue);
			break;

		case DPCD_0000F:
			msWriteByteMask(REG_DPRX_AUX_PM0_77_H + usRegOffsetAuxPM0ByID, (ucValue & BIT1) << 4, BIT5); // DPCD 0000F[1]
			msWriteByteMask(REG_DPRX_AUX_PM0_77_H + usRegOffsetAuxPM0ByID, (ucValue & BIT0) << 6, BIT6); // DPCD 0000F[0]
			break;

		case DPCD_00010:
			msWriteByte(REG_DPRX_AUX_PM1_10_L + usRegOffsetAuxPM1ByID, ucValue);
			break;

		case DPCD_00011:
			msWriteByte(REG_DPRX_AUX_PM1_10_H + usRegOffsetAuxPM1ByID, ucValue);
			break;

		case DPCD_00012:
			msWriteByte(REG_DPRX_AUX_PM1_11_L + usRegOffsetAuxPM1ByID, ucValue);
			break;

		case DPCD_00013:
			msWriteByte(REG_DPRX_AUX_PM1_11_H + usRegOffsetAuxPM1ByID, ucValue);
			break;

		case DPCD_00014:
			msWriteByte(REG_DPRX_AUX_PM1_12_L + usRegOffsetAuxPM1ByID, ucValue);
			break;

		case DPCD_00015:
			msWriteByte(REG_DPRX_AUX_PM1_12_H + usRegOffsetAuxPM1ByID, ucValue);
			break;

		case DPCD_00016:
			msWriteByte(REG_DPRX_AUX_PM1_13_L + usRegOffsetAuxPM1ByID, ucValue);
			break;

		case DPCD_00017:
			msWriteByte(REG_DPRX_AUX_PM1_13_H + usRegOffsetAuxPM1ByID, ucValue);
			break;

        case DPCD_00018:
        	msWriteByte(REG_DPRX_AUX_PM1_14_L + usRegOffsetAuxPM1ByID, ucValue);
			break;

	    case DPCD_00019:
	    	msWriteByte(REG_DPRX_AUX_PM1_14_H + usRegOffsetAuxPM1ByID, ucValue);
			break;

	    case DPCD_0001A:
	    	msWriteByte(REG_DPRX_AUX_PM1_15_L + usRegOffsetAuxPM1ByID, ucValue);
			break;

	    case DPCD_0001B:
	    	msWriteByte(REG_DPRX_AUX_PM1_15_H + usRegOffsetAuxPM1ByID, ucValue);
			break;

	    case DPCD_0001C:
	    	msWriteByte(REG_DPRX_AUX_PM1_16_L + usRegOffsetAuxPM1ByID, ucValue);
			break;

	    case DPCD_0001D:
	    	msWriteByte(REG_DPRX_AUX_PM1_16_H + usRegOffsetAuxPM1ByID, ucValue);
			break;

	    case DPCD_0001E:
	    	msWriteByte(REG_DPRX_AUX_PM1_17_L + usRegOffsetAuxPM1ByID, ucValue);
			break;

	    case DPCD_0001F:
			msWriteByte(REG_DPRX_AUX_PM1_17_H + usRegOffsetAuxPM1ByID, ucValue);
			break;

		case DPCD_00020:
		    if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_Auto)
            {
                msWriteByte(REG_DPRX_AUX_PM3_00_L + usRegOffsetAuxPM3ByID, ucValue); // PM
                msWriteByte(REG_DPRX_DPCD0_03_L + usRegOffsetDPCD0ByID, ucValue); // NPM
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForcePM)
            {
                msWriteByte(REG_DPRX_AUX_PM3_00_L + usRegOffsetAuxPM3ByID, ucValue);
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForceNPM)
            {
                msWriteByte(REG_DPRX_DPCD0_03_L + usRegOffsetDPCD0ByID, ucValue);
            }
            break;

        case DPCD_00021:
            if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_Auto)
            {
                msWriteByteMask(REG_DPRX_AUX_PM0_77_H + usRegOffsetAuxPM0ByID, (ucValue & BIT0) << 4, BIT4); // DPCD 00021[0]
                msWriteByte(REG_DPRX_DPCD0_03_H + usRegOffsetDPCD0ByID, ucValue);
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForcePM)
            {
                msWriteByteMask(REG_DPRX_AUX_PM0_77_H + usRegOffsetAuxPM0ByID, (ucValue & BIT0) << 4, BIT4); // DPCD 00021[0]
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForceNPM)
            {
                msWriteByte(REG_DPRX_DPCD0_03_H + usRegOffsetDPCD0ByID, ucValue);
            }
			break;

        case DPCD_00022:
            if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_Auto)
            {
                msWriteByte(REG_DPRX_AUX_PM0_03_L + usRegOffsetAuxPM0ByID, ucValue); // PM
                msWriteByte(REG_DPRX_DPCD0_04_L + usRegOffsetDPCD0ByID, ucValue); // NPM
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForcePM)
            {
                msWriteByte(REG_DPRX_AUX_PM0_03_L + usRegOffsetAuxPM0ByID, ucValue);
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForceNPM)
            {
                msWriteByte(REG_DPRX_DPCD0_04_L + usRegOffsetDPCD0ByID, ucValue);
            }

			break;

        case DPCD_00023:
            msWriteByte(REG_DPRX_DPCD0_04_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_00024:
            msWriteByte(REG_DPRX_DPCD0_05_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_00025:
            msWriteByte(REG_DPRX_DPCD0_05_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_00026:
            msWriteByte(REG_DPRX_DPCD0_06_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_00027:
            msWriteByte(REG_DPRX_DPCD0_06_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_00028:
            msWriteByte(REG_DPRX_DPCD0_07_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_00029:
            msWriteByte(REG_DPRX_DPCD0_07_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_0002A:
            msWriteByte(REG_DPRX_DPCD0_08_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_0002B:
            msWriteByte(REG_DPRX_DPCD0_08_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_0002C:
            msWriteByte(REG_DPRX_DPCD0_09_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_0002D:
            msWriteByte(REG_DPRX_DPCD0_09_H + usRegOffsetDPCD0ByID, ucValue);
            break;

		case DPCD_0002E:
			msWriteByte(REG_DPRX_AUX_PM0_51_L + usRegOffsetAuxPM0ByID, ucValue);
			break;

        case DPCD_0002F:
			msWriteByte(REG_DPRX_AUX_PM0_51_H + usRegOffsetAuxPM0ByID, ucValue);
			break;

		case DPCD_00060:
            msWriteByteMask(REG_DPRX_AUX_PM0_3F_L + usRegOffsetAuxPM0ByID, (ucValue & (BIT3|BIT2|BIT1)) << 3, (BIT6|BIT5|BIT4));
            msWriteByteMask(REG_DPRX_AUX_PM2_62_L + usRegOffsetAuxPM2ByID, (ucValue & BIT0) << 3, BIT3);
            break;

		case DPCD_00061:
            msWriteByteMask(REG_DPRX_AUX_PM2_62_L + usRegOffsetAuxPM2ByID, (ucValue & 0x0F) << 4, BIT7|BIT6|BIT5|BIT4);
            msWriteByteMask(REG_DPRX_AUX_PM2_62_H + usRegOffsetAuxPM2ByID, (ucValue & 0xF0) >> 4, BIT3|BIT2|BIT1|BIT0);
            break;

		case DPCD_00062:
			msWriteByteMask(REG_DPRX_AUX_PM2_62_H + usRegOffsetAuxPM2ByID, (ucValue & (BIT1|BIT0)) << 4, BIT5|BIT4);
            break;

		case DPCD_00063:
            msWriteByte(REG_DPRX_AUX_PM2_63_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_00064:
            msWriteByte(REG_DPRX_AUX_PM2_63_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_00065:
            msWriteByteMask(REG_DPRX_AUX_PM2_64_L + usRegOffsetAuxPM2ByID, ucValue & (BIT3|BIT2|BIT1|BIT0), BIT3|BIT2|BIT1|BIT0);
            break;

		case DPCD_00066:
            msWriteByteMask(REG_DPRX_AUX_PM0_3F_L + usRegOffsetAuxPM0ByID, (ucValue & BIT1) << 6, BIT7);
            msWriteByteMask(REG_DPRX_AUX_PM2_62_H + usRegOffsetAuxPM2ByID, (ucValue & BIT0) << 6, BIT6);
            break;

		case DPCD_00067:
            msWriteByteMask(REG_DPRX_AUX_PM2_64_L + usRegOffsetAuxPM2ByID, (ucValue & 0x0F) << 4, BIT7|BIT6|BIT5|BIT4);
            msWriteByteMask(REG_DPRX_AUX_PM2_64_H + usRegOffsetAuxPM2ByID, (ucValue & 0xF0) >> 4, BIT3|BIT2|BIT1|BIT0);
            break;

		case DPCD_00068:
            msWriteByte(REG_DPRX_AUX_PM2_65_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_00069:
            msWriteByte(REG_DPRX_AUX_PM2_65_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_0006A:
            msWriteByteMask(REG_DPRX_AUX_PM2_64_H + usRegOffsetAuxPM2ByID, (ucValue & (BIT3|BIT2|BIT1)) << 3, BIT6|BIT5|BIT4);
            break;

		case DPCD_0006B:
            msWriteByte(REG_DPRX_AUX_PM2_66_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_0006C:
            msWriteByte(REG_DPRX_AUX_PM2_66_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_0006D:
            msWriteByteMask(REG_DPRX_AUX_PM2_67_L + usRegOffsetAuxPM2ByID, ucValue & (BIT2|BIT1|BIT0), BIT2|BIT1|BIT0);
            break;

		case DPCD_0006E:
            msWriteByte(REG_DPRX_AUX_PM3_13_H + usRegOffsetAuxPM3ByID, ucValue);
            break;

		case DPCD_0006F:
		    //msWriteByteMask(REG_DPRX_AUX_PM2_67_L + usRegOffsetAuxByID, (ucValue & (BIT2|BIT1|BIT0)) << 4, BIT6|BIT5|BIT4);
            msWriteByte(REG_DPRX_AUX_PM3_14_L + usRegOffsetAuxPM3ByID, ucValue);
            break;

        case DPCD_00080:
		case DPCD_00081:
		case DPCD_00082:
		case DPCD_00083:
		case DPCD_00084:
		case DPCD_00085:
		case DPCD_00086:
		case DPCD_00087:
        case DPCD_00088:
        case DPCD_00089:
        case DPCD_0008A:
        case DPCD_0008B:
        case DPCD_0008C:
        case DPCD_0008D:
        case DPCD_0008E:
        case DPCD_0008F:
            mhal_DPRx_MCUWriteOUIDPCD(dprx_aux_id, ulDPCDAddress, ucValue);
			break;

		case DPCD_00090:
		    msWriteByteMask(REG_DPRX_AUX_PM0_62_L + usRegOffsetAuxPM0ByID, ((ucValue & 0xF0) >> 4), BIT3|BIT2|BIT1|BIT0);
			msWriteByteMask(REG_DPRX_AUX_PM2_67_H + usRegOffsetAuxPM2ByID, (ucValue & 0x0F), BIT3|BIT2|BIT1|BIT0);
			break;

		case DPCD_00091:
            msWriteByte(REG_DPRX_AUX_PM3_09_H + usRegOffsetAuxPM3ByID, ucValue);
            break;

		case DPCD_000A0:
		    msWriteByte(REG_DPRX_AUX_PM3_14_H + usRegOffsetAuxPM3ByID, ucValue);
            break;

		case DPCD_000A1:
		    msWriteByte(REG_DPRX_AUX_PM3_15_L + usRegOffsetAuxPM3ByID, ucValue);
            break;

		case DPCD_000A2:
		    msWriteByte(REG_DPRX_AUX_PM3_15_H + usRegOffsetAuxPM3ByID, ucValue);
            break;

		case DPCD_000A3:
		    msWriteByte(REG_DPRX_AUX_PM3_16_L + usRegOffsetAuxPM3ByID, ucValue);
            break;

		case DPCD_000A4:
		    msWriteByte(REG_DPRX_AUX_PM3_16_H + usRegOffsetAuxPM3ByID, ucValue);
            break;

		case DPCD_000A5:
		    msWriteByte(REG_DPRX_AUX_PM3_17_L + usRegOffsetAuxPM3ByID, ucValue);
            break;

		case DPCD_000A6:
		    msWriteByte(REG_DPRX_AUX_PM3_17_H + usRegOffsetAuxPM3ByID, ucValue);
            break;

		case DPCD_000A7:
		    msWriteByte(REG_DPRX_AUX_PM3_18_L + usRegOffsetAuxPM3ByID, ucValue);
            break;

        case DPCD_000A8:
            msWriteByte(REG_DPRX_AUX_PM3_18_H + usRegOffsetAuxPM3ByID, ucValue);
            break;

        case DPCD_000A9:
            msWriteByte(REG_DPRX_AUX_PM3_19_L + usRegOffsetAuxPM3ByID, ucValue);
            break;

        case DPCD_000AA:
            msWriteByte(REG_DPRX_AUX_PM3_19_H + usRegOffsetAuxPM3ByID, ucValue);
            break;

        case DPCD_000AB:
            msWriteByte(REG_DPRX_AUX_PM3_20_L + usRegOffsetAuxPM3ByID, ucValue);
            break;

        case DPCD_000AC:
            msWriteByte(REG_DPRX_AUX_PM3_20_H + usRegOffsetAuxPM3ByID, ucValue);
            break;

        case DPCD_000AD:
            msWriteByte(REG_DPRX_AUX_PM3_21_L + usRegOffsetAuxPM3ByID, ucValue);
            break;

        case DPCD_000AE:
            msWriteByte(REG_DPRX_AUX_PM3_21_H + usRegOffsetAuxPM3ByID, ucValue);
            break;

        case DPCD_000B0:
            msWriteByte(REG_DPRX_AUX_PM3_10_L + usRegOffsetAuxPM3ByID, ucValue);
            break;

        case DPCD_000B1:
            msWriteByte(REG_DPRX_AUX_PM3_10_H + usRegOffsetAuxPM3ByID, ucValue);
            break;

        case DPCD_000B2:
            msWriteByte(REG_DPRX_AUX_PM3_11_L + usRegOffsetAuxPM3ByID, ucValue);
            break;

        case DPCD_000B3:
            msWriteByte(REG_DPRX_AUX_PM3_11_H + usRegOffsetAuxPM3ByID, ucValue);
            break;

        case DPCD_000B4:
            msWriteByte(REG_DPRX_AUX_PM3_12_L + usRegOffsetAuxPM3ByID, ucValue);
            break;

        case DPCD_00200:
            if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_Auto)
		    {
		        msWriteByte(REG_DPRX_AUX_PM0_77_L + usRegOffsetAuxPM0ByID, ucValue); // PM
		        msWriteByteMask(REG_DPRX_DPCD0_3F_L + usRegOffsetDPCD0ByID, (ucValue & BIT7) >> 1, BIT6);
                msWriteByteMask(REG_DPRX_DPCD0_3F_L + usRegOffsetDPCD0ByID, (ucValue & BIT6) << 1, BIT7);
                msWriteByteMask(REG_DPRX_DPCD0_3F_L + usRegOffsetDPCD0ByID, (ucValue & (BIT5|BIT4|BIT3|BIT2|BIT1|BIT0)), BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		    }
		    else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForcePM)
		    {
                msWriteByte(REG_DPRX_AUX_PM0_77_L + usRegOffsetAuxPM0ByID, ucValue);
		    }
		    else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForceNPM)
		    {
                msWriteByteMask(REG_DPRX_DPCD0_3F_L + usRegOffsetDPCD0ByID, (ucValue & BIT7) >> 1, BIT6);
                msWriteByteMask(REG_DPRX_DPCD0_3F_L + usRegOffsetDPCD0ByID, (ucValue & BIT6) << 1, BIT7);
                msWriteByteMask(REG_DPRX_DPCD0_3F_L + usRegOffsetDPCD0ByID, (ucValue & (BIT5|BIT4|BIT3|BIT2|BIT1|BIT0)), BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		    }
            break;

        case DPCD_0020C:
            msWriteByte(REG_DPRX_DPCD0_40_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_0020D:
            msWriteByte(REG_DPRX_DPCD0_41_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_0020E:
            msWriteByte(REG_DPRX_DPCD0_41_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_00218:
            msWriteByte(REG_DPRX_DPCD1_10_L + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_00219:
            msWriteByte(REG_DPRX_DPCD1_10_H + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_00220:
            msWriteByteMask(REG_DPRX_DPCD1_11_L + usRegOffsetDPCD1ByID, (ucValue & (BIT4|BIT3|BIT2|BIT1|BIT0)), BIT4|BIT3|BIT2|BIT1|BIT0);
            break;

        case DPCD_00221:
            msWriteByte(REG_DPRX_DPCD1_11_H + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_00222:
            msWriteByte(REG_DPRX_DPCD1_12_H + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_00223:
            msWriteByte(REG_DPRX_DPCD1_12_L + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_00224:
            msWriteByte(REG_DPRX_DPCD1_13_H + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_00225:
            msWriteByte(REG_DPRX_DPCD1_13_L + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_00226:
            msWriteByte(REG_DPRX_DPCD1_14_H + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_00227:
            msWriteByte(REG_DPRX_DPCD1_14_L + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_00228:
            msWriteByte(REG_DPRX_DPCD1_15_H + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_00229:
            msWriteByte(REG_DPRX_DPCD1_15_L + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_0022A:
            msWriteByte(REG_DPRX_DPCD1_16_L + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_0022B:
            msWriteByte(REG_DPRX_DPCD1_16_H + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_0022C:
            msWriteByte(REG_DPRX_DPCD1_17_L + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_0022D:
            msWriteByte(REG_DPRX_DPCD1_17_H + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_0022E:
            msWriteByte(REG_DPRX_DPCD1_18_H + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_0022F:
            msWriteByte(REG_DPRX_DPCD1_18_L + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_00230:
            msWriteByte(REG_DPRX_DPCD1_19_H + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_00231:
            msWriteByte(REG_DPRX_DPCD1_19_L + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_00232:
            msWriteByte(REG_DPRX_DPCD1_1A_L + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_00233:
            msWriteByteMask(REG_DPRX_DPCD1_1A_H + usRegOffsetDPCD1ByID, (ucValue & (BIT1|BIT0)), (BIT1|BIT0));
            break;

        case DPCD_00234:
            msWriteByte(REG_DPRX_DPCD1_1B_L + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_00248:
            msWriteByteMask(REG_DPRX_DPCD1_1F_H + usRegOffsetDPCD1ByID, (ucValue & (BIT2|BIT1|BIT0)), (BIT2|BIT1|BIT0));
            break;

        case DPCD_00249:
            //msWriteByte(REG_DPRX_DPCD0_43_L + usRegOffsetDPCD0ByID, ucValue); read only in MT9701
            break;

        case DPCD_0024A:
            msWriteByte(REG_DPRX_DPCD0_43_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_0024B:
            msWriteByte(REG_DPRX_DPCD0_44_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_00250:
            msWriteByte(REG_DPRX_DPCD0_44_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_00251:
            msWriteByte(REG_DPRX_DPCD0_45_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_00252:
            msWriteByte(REG_DPRX_DPCD0_45_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_00253:
            msWriteByte(REG_DPRX_DPCD0_46_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_00254:
            msWriteByte(REG_DPRX_DPCD0_46_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_00255:
            msWriteByte(REG_DPRX_DPCD0_47_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_00256:
            msWriteByte(REG_DPRX_DPCD0_47_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_00257:
            msWriteByte(REG_DPRX_DPCD0_48_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_00258:
            msWriteByte(REG_DPRX_DPCD0_48_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_00259:
            msWriteByte(REG_DPRX_DPCD0_49_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_0025A:
            msWriteByteMask(REG_DPRX_DPCD1_2C_H + usRegOffsetDPCD1ByID, (ucValue & BIT0), BIT0);
            break;

        case DPCD_00262:
            msWriteByte(REG_DPRX_DPCD0_4A_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_00271:
            msWriteByte(REG_DPRX_DPCD0_1B_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_00272:
            msWriteByte(REG_DPRX_DPCD0_1B_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_00273:
            msWriteByteMask(REG_DPRX_DPCD0_1C_L + usRegOffsetDPCD0ByID, (ucValue & (BIT3|BIT2|BIT1|BIT0)), BIT3|BIT2|BIT1|BIT0);
            break;

        case DPCD_00274:
            msWriteByteMask(REG_DPRX_DPCD0_1C_L + usRegOffsetDPCD0ByID, (ucValue & (BIT3|BIT2|BIT1|BIT0)) << 4, BIT7|BIT6|BIT5|BIT4);
            break;

        case DPCD_00275:
            msWriteByteMask(REG_DPRX_DPCD0_1C_H + usRegOffsetDPCD0ByID, (ucValue & (BIT3|BIT2|BIT1|BIT0)), BIT3|BIT2|BIT1|BIT0);
            break;

        case DPCD_00276:
            msWriteByteMask(REG_DPRX_DPCD0_1C_H + usRegOffsetDPCD0ByID, (ucValue & (BIT3|BIT2|BIT1|BIT0)) << 4, BIT7|BIT6|BIT5|BIT4);
            break;

        case DPCD_00277:
            msWriteByteMask(REG_DPRX_DPCD0_1D_L + usRegOffsetDPCD0ByID, (ucValue & (BIT3|BIT2|BIT1|BIT0)), BIT3|BIT2|BIT1|BIT0);
            break;

        case DPCD_00278:
            msWriteByteMask(REG_DPRX_DPCD0_1D_L + usRegOffsetDPCD0ByID, (ucValue & (BIT3|BIT2|BIT1|BIT0)) << 4, BIT7|BIT6|BIT5|BIT4);
            break;

        case DPCD_00279:
            msWriteByteMask(REG_DPRX_DPCD0_1D_H + usRegOffsetDPCD0ByID, (ucValue & (BIT3|BIT2|BIT1|BIT0)), BIT3|BIT2|BIT1|BIT0);
            break;

        case DPCD_0027A:
            msWriteByteMask(REG_DPRX_DPCD0_1D_H + usRegOffsetDPCD0ByID, (ucValue & (BIT3|BIT2|BIT1|BIT0)) << 4, BIT7|BIT6|BIT5|BIT4);
            break;

        case DPCD_00281:
            msWriteByte(REG_DPRX_DPCD0_4C_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_00282:
            msWriteByte(REG_DPRX_DPCD0_4C_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_00314:
            msWriteByte(REG_DPRX_AUX_PM2_35_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_00320:
            msWriteByte(REG_DPRX_AUX_PM1_1C_L + usRegOffsetAuxPM1ByID, ucValue);
            break;

        case DPCD_00321:
            msWriteByte(REG_DPRX_AUX_PM1_1C_H + usRegOffsetAuxPM1ByID, ucValue);
            break;

        case DPCD_00340:
            msWriteByte(REG_DPRX_AUX_PM1_20_L + usRegOffsetAuxPM1ByID, ucValue);
            break;

        case DPCD_00341:
            msWriteByte(REG_DPRX_AUX_PM1_20_H + usRegOffsetAuxPM1ByID, ucValue);
            break;

        case DPCD_00342:
            msWriteByte(REG_DPRX_AUX_PM1_21_L + usRegOffsetAuxPM1ByID, ucValue);
            break;

        case DPCD_00343:
            msWriteByte(REG_DPRX_AUX_PM1_21_H + usRegOffsetAuxPM1ByID, ucValue);
            break;

        case DPCD_00381:
            msWriteByte(REG_DPRX_AUX_PM1_2D_L + usRegOffsetAuxPM1ByID, ucValue);
            break;

        case DPCD_00400:
		case DPCD_00401:
		case DPCD_00402:
		case DPCD_00403:
		case DPCD_00404:
		case DPCD_00405:
		case DPCD_00406:
		case DPCD_00407:
		case DPCD_00408:
		case DPCD_00409:
		case DPCD_0040A:
		case DPCD_0040B:
			mhal_DPRx_MCUWriteOUIDPCD(dprx_aux_id, ulDPCDAddress, ucValue);
			break;

	    case DPCD_0040F:
			msWriteByteMask(REG_DPRX_AUX_PM2_35_L + usRegOffsetAuxPM2ByID, (ucValue & BIT0) << 5, BIT5);
			break;

        case DPCD_00414:
            msWriteByte(REG_DPRX_PM_TOP_6A_L + usRegOffsetAuxPMTopByID, ucValue);
            break;

        case DPCD_00430:
            msWriteByte(REG_DPRX_PM_TOP_78_L + usRegOffsetAuxPMTopByID, ucValue);
            break;

		case DPCD_00431:
		    msWriteByte(REG_DPRX_PM_TOP_78_H + usRegOffsetAuxPMTopByID, ucValue);
            break;

		case DPCD_00432:
		    msWriteByte(REG_DPRX_PM_TOP_79_L + usRegOffsetAuxPMTopByID, ucValue);
            break;

		case DPCD_00433:
		    msWriteByte(REG_DPRX_PM_TOP_79_H + usRegOffsetAuxPMTopByID, ucValue);
            break;

		case DPCD_00434:
		    msWriteByte(REG_DPRX_PM_TOP_7A_L + usRegOffsetAuxPMTopByID, ucValue);
            break;

		case DPCD_00435:
		    msWriteByte(REG_DPRX_PM_TOP_7A_H + usRegOffsetAuxPMTopByID, ucValue);
            break;

		case DPCD_00436:
		    msWriteByte(REG_DPRX_PM_TOP_7B_L + usRegOffsetAuxPMTopByID, ucValue);
            break;

		case DPCD_00437:
		    msWriteByte(REG_DPRX_PM_TOP_7B_H + usRegOffsetAuxPMTopByID, ucValue);
            break;

		case DPCD_00438:
		    msWriteByte(REG_DPRX_PM_TOP_7C_L + usRegOffsetAuxPMTopByID, ucValue);
            break;

		case DPCD_00439:
		    msWriteByte(REG_DPRX_PM_TOP_7C_H + usRegOffsetAuxPMTopByID, ucValue);
            break;

		case DPCD_0043A:
		    msWriteByte(REG_DPRX_PM_TOP_7D_L + usRegOffsetAuxPMTopByID, ucValue);
            break;

		case DPCD_0043B:
		    msWriteByte(REG_DPRX_PM_TOP_7D_H + usRegOffsetAuxPMTopByID, ucValue);
            break;

		case DPCD_0043C:
            msWriteByte(REG_DPRX_PM_TOP_7E_L + usRegOffsetAuxPMTopByID, ucValue);
            break;

	    case DPCD_0043E:
			msWriteByte(REG_DPRX_PM_TOP_7F_L + usRegOffsetAuxPMTopByID, ucValue);
            break;

        case DPCD_00500:
		case DPCD_00501:
		case DPCD_00502:
		case DPCD_00503:
		case DPCD_00504:
		case DPCD_00505:
		case DPCD_00506:
		case DPCD_00507:
		case DPCD_00508:
		case DPCD_00509:
		case DPCD_0050A:
		case DPCD_0050B:
			mhal_DPRx_MCUWriteOUIDPCD(dprx_aux_id, ulDPCDAddress, ucValue);
			break;

        case DPCD_00700:
            msWriteByte(REG_DPRX_AUX_PM1_18_L + usRegOffsetAuxPM1ByID, ucValue);
            break;

        case DPCD_02002:
            if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_Auto)
		    {
		        msWriteByte(REG_DPRX_AUX_PM0_77_L + usRegOffsetAuxPM0ByID, ucValue); // PM
		        msWriteByteMask(REG_DPRX_DPCD0_3F_L + usRegOffsetDPCD0ByID, (ucValue & BIT7) >> 1, BIT6);
                msWriteByteMask(REG_DPRX_DPCD0_3F_L + usRegOffsetDPCD0ByID, (ucValue & BIT6) << 1, BIT7);
                msWriteByteMask(REG_DPRX_DPCD0_3F_L + usRegOffsetDPCD0ByID, (ucValue & (BIT5|BIT4|BIT3|BIT2|BIT1|BIT0)), BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		    }
		    else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForcePM)
		    {
                msWriteByte(REG_DPRX_AUX_PM0_77_L + usRegOffsetAuxPM0ByID, ucValue);
		    }
		    else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForceNPM)
		    {
                msWriteByteMask(REG_DPRX_DPCD0_3F_L + usRegOffsetDPCD0ByID, (ucValue & BIT7) >> 1, BIT6);
                msWriteByteMask(REG_DPRX_DPCD0_3F_L + usRegOffsetDPCD0ByID, (ucValue & BIT6) << 1, BIT7);
                msWriteByteMask(REG_DPRX_DPCD0_3F_L + usRegOffsetDPCD0ByID, (ucValue & (BIT5|BIT4|BIT3|BIT2|BIT1|BIT0)), BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
		    }
            break;

		case DPCD_02200:
		    if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_Auto)
		    {
		        msWriteByte(REG_DPRX_AUX_PM2_31_L + usRegOffsetAuxPM2ByID, ucValue); // PM
		        msWriteByte(REG_DPRX_DPCD1_32_H + usRegOffsetDPCD1ByID, ucValue); // NPM
		    }
		    else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForcePM)
		    {
                msWriteByte(REG_DPRX_AUX_PM2_31_L + usRegOffsetAuxPM2ByID, ucValue);
		    }
		    else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForceNPM)
		    {
                msWriteByte(REG_DPRX_DPCD1_32_H + usRegOffsetDPCD1ByID, ucValue);
		    }
            break;

		case DPCD_02201:
			msWriteByte(REG_DPRX_AUX_PM2_71_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_02202:
		    msWriteByte(REG_DPRX_AUX_PM2_71_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_02203:
			msWriteByte(REG_DPRX_AUX_PM2_72_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_02204:
			msWriteByte(REG_DPRX_AUX_PM2_72_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_02205:
			msWriteByte(REG_DPRX_AUX_PM2_73_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_02206:
			msWriteByte(REG_DPRX_AUX_PM2_73_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_02207:
			msWriteByte(REG_DPRX_AUX_PM2_74_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_02208:
			msWriteByte(REG_DPRX_AUX_PM2_74_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_02209:
			msWriteByte(REG_DPRX_AUX_PM2_75_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_0220A:
			msWriteByte(REG_DPRX_AUX_PM2_75_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_0220B:
			msWriteByte(REG_DPRX_AUX_PM2_76_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_0220C:
			msWriteByte(REG_DPRX_AUX_PM2_76_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_0220D:
			msWriteByte(REG_DPRX_AUX_PM2_77_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_0220E:
			msWriteByte(REG_DPRX_AUX_PM2_77_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_0220F:
			msWriteByte(REG_DPRX_AUX_PM2_78_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_02210:
			msWriteByte(REG_DPRX_AUX_PM2_78_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_02211:
			msWriteByte(REG_DPRX_AUX_PM2_79_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

		case DPCD_02212:
            msWriteByte(REG_DPRX_AUX_PM2_68_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_02213:
            msWriteByte(REG_DPRX_AUX_PM2_68_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_02214:
            msWriteByte(REG_DPRX_AUX_PM0_52_L + usRegOffsetAuxPM0ByID, ucValue);
            break;

        case DPCD_02215:
            msWriteByte(REG_DPRX_AUX_PM3_00_H + usRegOffsetAuxPM3ByID, ucValue);
            break;

        case DPCD_02216:
            msWriteByte(REG_DPRX_AUX_PM3_09_L + usRegOffsetAuxPM3ByID, ucValue);
            break;

		case DPCD_02217:
			msWriteByte(REG_DPRX_AUX_PM3_67_H + usRegOffsetAuxPM3ByID, ucValue);
			break;

	    case DPCD_02230:
            msWriteByte(REG_DPRX_DPCD0_0A_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_02231:
            msWriteByte(REG_DPRX_DPCD0_0A_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_02232:
            msWriteByte(REG_DPRX_DPCD0_0B_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_02233:
            msWriteByte(REG_DPRX_DPCD0_0B_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_02234:
            msWriteByte(REG_DPRX_DPCD0_0C_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_02235:
            msWriteByte(REG_DPRX_DPCD0_0C_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_02236:
            msWriteByte(REG_DPRX_DPCD0_0D_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_02237:
            msWriteByte(REG_DPRX_DPCD0_0D_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_02238:
            msWriteByte(REG_DPRX_DPCD0_0E_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_02239:
            msWriteByte(REG_DPRX_DPCD0_0E_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_0223A:
            msWriteByte(REG_DPRX_DPCD0_0F_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_0223B:
            msWriteByte(REG_DPRX_DPCD0_0F_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_0223C:
            msWriteByte(REG_DPRX_DPCD0_10_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_0223D:
            msWriteByte(REG_DPRX_DPCD0_10_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_0223E:
            msWriteByte(REG_DPRX_DPCD0_11_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_0223F:
            msWriteByte(REG_DPRX_DPCD0_11_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_02240:
            msWriteByte(REG_DPRX_DPCD0_12_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_02241:
            msWriteByte(REG_DPRX_DPCD0_12_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_02242:
            msWriteByte(REG_DPRX_DPCD0_13_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_02243:
            msWriteByte(REG_DPRX_DPCD0_13_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_02244:
            msWriteByte(REG_DPRX_DPCD0_14_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_02245:
            msWriteByte(REG_DPRX_DPCD0_14_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_02246:
            msWriteByte(REG_DPRX_DPCD0_15_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_02247:
            msWriteByte(REG_DPRX_DPCD0_15_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_02248:
            msWriteByte(REG_DPRX_DPCD0_16_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_02249:
            msWriteByte(REG_DPRX_DPCD0_16_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_0224A:
            msWriteByte(REG_DPRX_DPCD0_17_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_0224B:
            msWriteByte(REG_DPRX_DPCD0_17_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_0224C:
            msWriteByte(REG_DPRX_DPCD0_18_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_0224D:
            msWriteByte(REG_DPRX_DPCD0_18_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_0224E:
            msWriteByte(REG_DPRX_DPCD0_19_L + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_0224F:
            msWriteByte(REG_DPRX_DPCD0_19_H + usRegOffsetDPCD0ByID, ucValue);
            break;

        case DPCD_02250:
            msWriteByte(REG_DPRX_DPCD0_25_H + usRegOffsetDPCD0ByID, ucValue);
            break;

		case DPCD_02251:
			msWriteByte(REG_DPRX_AUX_PM3_13_L + usRegOffsetAuxPM3ByID, ucValue);
			break;

	    case DPCD_02260:
			msWriteByteMask(REG_DPRX_AUX_PM2_62_L + usRegOffsetAuxPM2ByID, (ucValue & BIT0) << 3, BIT3);
			break;

	    case DPCD_02261:
			msWriteByteMask(REG_DPRX_AUX_PM2_62_L + usRegOffsetAuxPM2ByID, (ucValue & 0x0F) << 4, BIT7|BIT6|BIT5|BIT4);
            msWriteByteMask(REG_DPRX_AUX_PM2_62_H + usRegOffsetAuxPM2ByID, (ucValue & 0xF0) >> 4, BIT3|BIT2|BIT1|BIT0);
			break;

        case DPCD_02262:
            msWriteByteMask(REG_DPRX_AUX_PM2_62_H + usRegOffsetAuxPM2ByID, (ucValue & (BIT1|BIT0)) << 4, BIT5|BIT4);
            break;

		case DPCD_02263:
            msWriteByte(REG_DPRX_AUX_PM2_63_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_02264:
            msWriteByte(REG_DPRX_AUX_PM2_63_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_02265:
            msWriteByteMask(REG_DPRX_AUX_PM2_64_L + usRegOffsetAuxPM2ByID, (ucValue & 0x0F), BIT3|BIT2|BIT1|BIT0);
            break;

        case DPCD_02266:
            msWriteByteMask(REG_DPRX_AUX_PM2_62_H + usRegOffsetAuxPM2ByID, (ucValue & BIT0) << 6, BIT6);
            break;

        case DPCD_02267:
            msWriteByteMask(REG_DPRX_AUX_PM2_64_L + usRegOffsetAuxPM2ByID, (ucValue & 0x0F) << 4, BIT7|BIT6|BIT5|BIT4);
            msWriteByteMask(REG_DPRX_AUX_PM2_64_H + usRegOffsetAuxPM2ByID, (ucValue & 0xF0) >> 4, BIT3|BIT2|BIT1|BIT0);
            break;

        case DPCD_02268:
            msWriteByte(REG_DPRX_AUX_PM2_65_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_02269:
            msWriteByte(REG_DPRX_AUX_PM2_65_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_0226A:
            msWriteByte(REG_DPRX_AUX_PM3_27_H + usRegOffsetAuxPM3ByID, ucValue);
            break;

        case DPCD_0226B:
            msWriteByte(REG_DPRX_AUX_PM2_66_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_0226C:
            msWriteByte(REG_DPRX_AUX_PM2_66_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_0226D:
            msWriteByteMask(REG_DPRX_AUX_PM2_67_L + usRegOffsetAuxPM2ByID, (ucValue & (BIT2|BIT1|BIT0)), BIT2|BIT1|BIT0);
            break;

        case DPCD_0226E:
            msWriteByte(REG_DPRX_AUX_PM3_29_H + usRegOffsetAuxPM3ByID, ucValue);
            break;

        case DPCD_0226F:
            msWriteByteMask(REG_DPRX_AUX_PM2_67_L + usRegOffsetAuxPM2ByID, (ucValue & (BIT2|BIT1|BIT0)) << 4, BIT6|BIT5|BIT4);
            break;

        case DPCD_02270:
			msWriteByteMask(REG_DPRX_AUX_PM2_62_L + usRegOffsetAuxPM2ByID, (ucValue & BIT0) << 3, BIT3);
			break;

        case DPCD_02271:
            msWriteByteMask(REG_DPRX_AUX_PM2_62_L + usRegOffsetAuxPM2ByID, (ucValue & 0x0F) << 4, BIT7|BIT6|BIT5|BIT4);
            msWriteByteMask(REG_DPRX_AUX_PM2_62_H + usRegOffsetAuxPM2ByID, (ucValue & 0xF0) >> 4, BIT3|BIT2|BIT1|BIT0);
            break;

        case DPCD_02272:
            msWriteByteMask(REG_DPRX_AUX_PM2_62_H + usRegOffsetAuxPM2ByID, (ucValue & (BIT1|BIT0)) << 4, BIT5|BIT4);
            break;

		case DPCD_02273:
            msWriteByte(REG_DPRX_AUX_PM2_63_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_02300:
            msWriteByte(REG_DPRX_AUX_PM4_12_L + usRegOffsetAuxPM4ByID, ucValue);
            break;

        case DPCD_02301:
            msWriteByte(REG_DPRX_AUX_PM4_12_H + usRegOffsetAuxPM4ByID, ucValue);
            break;

		case DPCD_02302:
            msWriteByte(REG_DPRX_AUX_PM4_13_L + usRegOffsetAuxPM4ByID, ucValue);
            break;

		case DPCD_02303:
            msWriteByte(REG_DPRX_AUX_PM4_13_H + usRegOffsetAuxPM4ByID, ucValue);
            break;

		case DPCD_02304:
            msWriteByte(REG_DPRX_AUX_PM4_14_L + usRegOffsetAuxPM4ByID, ucValue);
            break;

		case DPCD_02305:
            msWriteByte(REG_DPRX_AUX_PM4_14_H + usRegOffsetAuxPM4ByID, ucValue);
            break;

		case DPCD_02306:
            msWriteByte(REG_DPRX_AUX_PM4_15_L + usRegOffsetAuxPM4ByID, ucValue);
            break;

		case DPCD_02307:
            msWriteByte(REG_DPRX_AUX_PM4_15_H + usRegOffsetAuxPM4ByID, ucValue);
            break;

        case DPCD_0230C:
            if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_Auto)
            {
                msWriteByte(REG_DPRX_AUX_PM4_16_L + usRegOffsetAuxPM4ByID, ucValue); // PM
                //msWriteByte(REG_DPRX_DPCD0_68_H + usRegOffsetDPCD0ByID, ucValue); // NPM read only in MT9701
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForcePM)
            {
                msWriteByte(REG_DPRX_AUX_PM4_16_L + usRegOffsetAuxPM4ByID, ucValue);
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForceNPM)
            {
                //msWriteByte(REG_DPRX_DPCD0_68_H + usRegOffsetDPCD0ByID, ucValue); // NPM read only in MT9701
            }
            break;

        case DPCD_0230D:
            if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_Auto)
            {
                msWriteByte(REG_DPRX_AUX_PM4_16_H + usRegOffsetAuxPM4ByID, ucValue); // PM
                //msWriteByte(REG_DPRX_DPCD0_69_L + usRegOffsetDPCD0ByID, ucValue); // NPM read only in MT9701
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForcePM)
            {
                msWriteByte(REG_DPRX_AUX_PM4_16_H + usRegOffsetAuxPM4ByID, ucValue);
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForceNPM)
            {
                //msWriteByte(REG_DPRX_DPCD0_69_L + usRegOffsetDPCD0ByID, ucValue); // NPM read only in MT9701
            }
            break;

        case DPCD_0230E:
            if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_Auto)
            {
                msWriteByte(REG_DPRX_AUX_PM4_17_L + usRegOffsetAuxPM4ByID, ucValue); // PM
                //msWriteByte(REG_DPRX_DPCD0_69_H + usRegOffsetDPCD0ByID, ucValue); // NPM read only in MT9701
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForcePM)
            {
                msWriteByte(REG_DPRX_AUX_PM4_17_L + usRegOffsetAuxPM4ByID, ucValue);
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForceNPM)
            {
                //msWriteByte(REG_DPRX_DPCD0_69_H + usRegOffsetDPCD0ByID, ucValue); // NPM read only in MT9701
            }
            break;

        case DPCD_0230F:
            if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_Auto)
            {
                msWriteByte(REG_DPRX_AUX_PM4_17_H + usRegOffsetAuxPM4ByID, ucValue); // PM
                //msWriteByte(REG_DPRX_DPCD0_6A_L + usRegOffsetDPCD0ByID, ucValue); // NPM read only in MT9701
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForcePM)
            {
                msWriteByte(REG_DPRX_AUX_PM4_17_H + usRegOffsetAuxPM4ByID, ucValue);
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForceNPM)
            {
                //msWriteByte(REG_DPRX_DPCD0_6A_L + usRegOffsetDPCD0ByID, ucValue); // NPM read only in MT9701
            }
            break;

        case DPCD_02310:
            if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_Auto)
            {
                msWriteByte(REG_DPRX_AUX_PM4_18_L + usRegOffsetAuxPM4ByID, ucValue); // PM
                //msWriteByte(REG_DPRX_DPCD0_6A_H + usRegOffsetDPCD0ByID, ucValue); // NPM read only in MT9701
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForcePM)
            {
                msWriteByte(REG_DPRX_AUX_PM4_18_L + usRegOffsetAuxPM4ByID, ucValue);
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForceNPM)
            {
                //msWriteByte(REG_DPRX_DPCD0_6A_H + usRegOffsetDPCD0ByID, ucValue); // NPM read only in MT9701
            }
            break;

        case DPCD_02311:
            if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_Auto)
            {
                msWriteByte(REG_DPRX_AUX_PM4_18_H + usRegOffsetAuxPM4ByID, ucValue); // PM
                //msWriteByte(REG_DPRX_DPCD0_6B_L + usRegOffsetDPCD0ByID, ucValue); // NPM read only in MT9701
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForcePM)
            {
                msWriteByte(REG_DPRX_AUX_PM4_18_H + usRegOffsetAuxPM4ByID, ucValue);
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForceNPM)
            {
                //msWriteByte(REG_DPRX_DPCD0_6B_L + usRegOffsetDPCD0ByID, ucValue); // NPM read only in MT9701
            }
            break;

        case DPCD_02312:
            if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_Auto)
            {
                msWriteByte(REG_DPRX_AUX_PM4_19_L + usRegOffsetAuxPM4ByID, ucValue); // PM
                //msWriteByte(REG_DPRX_DPCD0_6B_H + usRegOffsetDPCD0ByID, ucValue); // NPM read only in MT9701
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForcePM)
            {
                msWriteByte(REG_DPRX_AUX_PM4_19_L + usRegOffsetAuxPM4ByID, ucValue);
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForceNPM)
            {
                //msWriteByte(REG_DPRX_DPCD0_6B_H + usRegOffsetDPCD0ByID, ucValue); // NPM read only in MT9701
            }
            break;

        case DPCD_02313:
            if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_Auto)
            {
                msWriteByte(REG_DPRX_AUX_PM4_19_H + usRegOffsetAuxPM4ByID, ucValue); // PM
                //msWriteByte(REG_DPRX_DPCD0_6C_L + usRegOffsetDPCD0ByID, ucValue); // NPM read only in MT9701
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForcePM)
            {
                msWriteByte(REG_DPRX_AUX_PM4_19_H + usRegOffsetAuxPM4ByID, ucValue);
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForceNPM)
            {
                //msWriteByte(REG_DPRX_DPCD0_6C_L + usRegOffsetDPCD0ByID, ucValue); // NPM read only in MT9701
            }
            break;

        case DPCD_02314:
            if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_Auto)
            {
                msWriteByte(REG_DPRX_AUX_PM4_1A_L + usRegOffsetAuxPM4ByID, ucValue); // PM
                //msWriteByte(REG_DPRX_DPCD0_6C_H + usRegOffsetDPCD0ByID, ucValue); // NPM read only in MT9701
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForcePM)
            {
                msWriteByte(REG_DPRX_AUX_PM4_1A_L + usRegOffsetAuxPM4ByID, ucValue);
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForceNPM)
            {
                //msWriteByte(REG_DPRX_DPCD0_6C_H + usRegOffsetDPCD0ByID, ucValue); // NPM read only in MT9701
            }
            break;

        case DPCD_02315:
            if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_Auto)
            {
                msWriteByte(REG_DPRX_AUX_PM4_1A_H + usRegOffsetAuxPM4ByID, ucValue); // PM
                //msWriteByte(REG_DPRX_DPCD0_6D_L + usRegOffsetDPCD0ByID, ucValue); // NPM read only in MT9701
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForcePM)
            {
                msWriteByte(REG_DPRX_AUX_PM4_1A_H + usRegOffsetAuxPM4ByID, ucValue);
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForceNPM)
            {
                //msWriteByte(REG_DPRX_DPCD0_6D_L + usRegOffsetDPCD0ByID, ucValue); // NPM read only in MT9701
            }
            break;

        case DPCD_02316:
            if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_Auto)
            {
                msWriteByte(REG_DPRX_AUX_PM4_1B_L + usRegOffsetAuxPM4ByID, ucValue); // PM
                //msWriteByte(REG_DPRX_DPCD0_6D_H + usRegOffsetDPCD0ByID, ucValue); // NPM read only in MT9701
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForcePM)
            {
                msWriteByte(REG_DPRX_AUX_PM4_1B_L + usRegOffsetAuxPM4ByID, ucValue);
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForceNPM)
            {
                //msWriteByte(REG_DPRX_DPCD0_6D_H + usRegOffsetDPCD0ByID, ucValue); // NPM read only in MT9701
            }
            break;

        case DPCD_03000:
            msWriteByte(REG_DPRX_AUX_PM2_5C_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_03030:
            msWriteByte(REG_DPRX_DPCD1_41_L + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_03031:
            msWriteByte(REG_DPRX_DPCD1_41_H + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_03032:
            msWriteByte(REG_DPRX_DPCD1_42_L + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_03033:
            msWriteByte(REG_DPRX_DPCD1_42_H + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_03034:
            msWriteByte(REG_DPRX_DPCD1_43_L + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_03050:
            msWriteByte(REG_DPRX_DPCD1_43_H + usRegOffsetDPCD1ByID, ucValue);
            break;

        case DPCD_68000:
            if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_Auto)
            {
                msWriteByte(REG_DPRX_AUX_PM2_2A_L + usRegOffsetAuxPM2ByID, ucValue); // PM
                msWriteByte(REG_DPRX_DPCD1_22_L + usRegOffsetDPCD1ByID, ucValue); // NPM
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForcePM)
            {
                msWriteByte(REG_DPRX_AUX_PM2_2A_L + usRegOffsetAuxPM2ByID, ucValue);
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForceNPM)
            {
                msWriteByte(REG_DPRX_DPCD1_22_L + usRegOffsetDPCD1ByID, ucValue);
            }
            break;

        case DPCD_68001:
            if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_Auto)
            {
                msWriteByte(REG_DPRX_AUX_PM2_2A_H + usRegOffsetAuxPM2ByID, ucValue); // PM
                msWriteByte(REG_DPRX_DPCD1_22_H + usRegOffsetDPCD1ByID, ucValue); // NPM
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForcePM)
            {
                msWriteByte(REG_DPRX_AUX_PM2_2A_H + usRegOffsetAuxPM2ByID, ucValue);
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForceNPM)
            {
                msWriteByte(REG_DPRX_DPCD1_22_H + usRegOffsetDPCD1ByID, ucValue);
            }
            break;

		case DPCD_68002:
		    if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_Auto)
            {
                msWriteByte(REG_DPRX_AUX_PM2_2B_L + usRegOffsetAuxPM2ByID, ucValue); // PM
                msWriteByte(REG_DPRX_DPCD1_23_L + usRegOffsetDPCD1ByID, ucValue); // NPM
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForcePM)
            {
                msWriteByte(REG_DPRX_AUX_PM2_2B_L + usRegOffsetAuxPM2ByID, ucValue);
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForceNPM)
            {
                msWriteByte(REG_DPRX_DPCD1_23_L + usRegOffsetDPCD1ByID, ucValue);
            }
            break;

		case DPCD_68003:
		    if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_Auto)
            {
                msWriteByte(REG_DPRX_AUX_PM2_2B_H + usRegOffsetAuxPM2ByID, ucValue); // PM
                msWriteByte(REG_DPRX_DPCD1_23_H + usRegOffsetDPCD1ByID, ucValue); // NPM
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForcePM)
            {
                msWriteByte(REG_DPRX_AUX_PM2_2B_H + usRegOffsetAuxPM2ByID, ucValue);
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForceNPM)
            {
                msWriteByte(REG_DPRX_DPCD1_23_H + usRegOffsetDPCD1ByID, ucValue);
            }
            break;

		case DPCD_68004:
		    if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_Auto)
            {
                msWriteByte(REG_DPRX_AUX_PM2_2C_L + usRegOffsetAuxPM2ByID, ucValue); // PM
                msWriteByte(REG_DPRX_DPCD1_24_L + usRegOffsetDPCD1ByID, ucValue); // NPM
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForcePM)
            {
                msWriteByte(REG_DPRX_AUX_PM2_2C_L + usRegOffsetAuxPM2ByID, ucValue);
            }
            else if(dp_riu_DPCD_type_id == DPRx_RIU_DPCD_ForceNPM)
            {
                msWriteByte(REG_DPRX_DPCD1_24_L + usRegOffsetDPCD1ByID, ucValue);
            }
            break;

	    case DPCD_68005:
            msWriteByte(REG_DPRX_AUX_PM2_29_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_68006:
            msWriteByte(REG_DPRX_AUX_PM2_29_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_68014:
            msWriteByte(REG_DPRX_AUX_PM2_10_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_68015:
            msWriteByte(REG_DPRX_AUX_PM2_10_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_68016:
            msWriteByte(REG_DPRX_AUX_PM2_11_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_68017:
            msWriteByte(REG_DPRX_AUX_PM2_11_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_68018:
            msWriteByte(REG_DPRX_AUX_PM2_12_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_68019:
            msWriteByte(REG_DPRX_AUX_PM2_12_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_6801A:
            msWriteByte(REG_DPRX_AUX_PM2_13_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_6801B:
            msWriteByte(REG_DPRX_AUX_PM2_13_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_6801C:
            msWriteByte(REG_DPRX_AUX_PM2_14_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_6801D:
            msWriteByte(REG_DPRX_AUX_PM2_14_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_6801E:
            msWriteByte(REG_DPRX_AUX_PM2_15_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_6801F:
            msWriteByte(REG_DPRX_AUX_PM2_15_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_68020:
            msWriteByte(REG_DPRX_AUX_PM2_16_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_68021:
            msWriteByte(REG_DPRX_AUX_PM2_16_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_68022:
            msWriteByte(REG_DPRX_AUX_PM2_17_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_68023:
            msWriteByte(REG_DPRX_AUX_PM2_17_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_68024:
            msWriteByte(REG_DPRX_AUX_PM2_18_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_68025:
            msWriteByte(REG_DPRX_AUX_PM2_18_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_68026:
            msWriteByte(REG_DPRX_AUX_PM2_19_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_68027:
            msWriteByte(REG_DPRX_AUX_PM2_19_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_68028:
            msWriteByteMask(REG_DPRX_AUX_PM2_1E_H + usRegOffsetAuxPM2ByID, (ucValue & BIT1) >> 1, BIT0); // DPCD 68028[1]
			msWriteByteMask(REG_DPRX_AUX_PM2_21_L + usRegOffsetAuxPM2ByID, (ucValue & BIT0) << 2, BIT2); // DPCD 68028[0]
            break;

        case DPCD_68029:
            msWriteByteMask(REG_DPRX_AUX_PM2_20_L + usRegOffsetAuxPM2ByID, (ucValue & BIT3), BIT3); // DPCD 68029[3]
			msWriteByteMask(REG_DPRX_AUX_PM2_21_L + usRegOffsetAuxPM2ByID, (ucValue & BIT2) >> 1, BIT1); // DPCD 68029[2]
			msWriteByteMask(REG_DPRX_AUX_PM2_21_L + usRegOffsetAuxPM2ByID, (ucValue & BIT1) >> 1, BIT0); // DPCD 68029[1]
			msWriteByteMask(REG_DPRX_AUX_PM2_20_L + usRegOffsetAuxPM2ByID, (ucValue & BIT0) << 6, BIT6); // DPCD 68029[0]
            break;

        case DPCD_6802A:
            msWriteByte(REG_DPRX_AUX_PM2_1A_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_6802B:
            msWriteByte(REG_DPRX_AUX_PM2_1A_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_E0000:
            msWriteByte(REG_DPRX_AUX_PM2_6A_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_E0001:
            msWriteByte(REG_DPRX_AUX_PM2_6A_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_E0002:
            msWriteByte(REG_DPRX_AUX_PM2_6B_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_E0003:
            msWriteByte(REG_DPRX_AUX_PM2_6B_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_E0004:
            msWriteByte(REG_DPRX_AUX_PM2_6C_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_E0005:
            msWriteByte(REG_DPRX_AUX_PM2_6C_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_E0006:
            msWriteByte(REG_DPRX_AUX_PM2_6D_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_E0007:
            msWriteByte(REG_DPRX_AUX_PM2_6D_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_E0008:
            msWriteByte(REG_DPRX_AUX_PM2_6E_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_E0009:
            msWriteByte(REG_DPRX_AUX_PM2_6E_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_E000B:
            msWriteByte(REG_DPRX_AUX_PM2_6F_H + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_E000D:
            msWriteByteMask(REG_DPRX_AUX_PM3_33_L + usRegOffsetAuxPM3ByID, (ucValue & (BIT6|BIT0)), BIT6|BIT0);
            break;

        case DPCD_F0000:
            msWriteByteMask(REG_DPRX_AUX_PM2_7A_L + usRegOffsetAuxPM2ByID, (ucValue & 0xF0) >> 4, BIT3|BIT2|BIT1|BIT0);
            msWriteByteMask(REG_DPRX_AUX_PM2_7E_H + usRegOffsetAuxPM2ByID, (ucValue & 0x0F) << 4, BIT7|BIT6|BIT5|BIT4);
            break;

        case DPCD_F0001:
            msWriteByte(REG_DPRX_AUX_PM0_00_H + usRegOffsetAuxPM0ByID, ucValue);
            break;

        case DPCD_F0002:
            msWriteByte(REG_DPRX_AUX_PM0_7F_H + usRegOffsetAuxPM0ByID, ucValue);
            break;

        case DPCD_F0004:
            msWriteByte(REG_DPRX_AUX_PM2_69_L + usRegOffsetAuxPM2ByID, ucValue);
            break;

        case DPCD_FFF00:
            msWriteByteMask(REG_DPRX_AUX_PM0_75_L + usRegOffsetAuxPM0ByID, (ucValue & BIT0) << 1, BIT1); // DPCD FFF04[0]
            break;

		default:
			break;
	}

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetDPCDValueByRIU_Mask()
//  [Description]
//                  mhal_DPRx_SetDPCDValueByRIU_Mask
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetDPCDValueByRIU_Mask(DPRx_AUX_ID dprx_aux_id, DWORD ulDPCDAddress, BYTE ucValue, BYTE ucMask, DPRx_RIU_DPCD_TYPE dp_riu_DPCD_type_id)
{
	BYTE ucDPCDTemp = mhal_DPRx_GetDPCDValueByRIU(dprx_aux_id, ulDPCDAddress);

	ucDPCDTemp &= (~ucMask);
	ucDPCDTemp |= (ucValue & ucMask);

	mhal_DPRx_SetDPCDValueByRIU(dprx_aux_id, ulDPCDAddress, ucDPCDTemp, dp_riu_DPCD_type_id);

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
    WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET_TRANS_CTRL(dprx_id);

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
//                  mhal_DPRx_OverWriteDPCD246()
//  [Description]
//					mhal_DPRx_OverWriteDPCD246
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_OverWriteDPCD246(DPRx_AUX_ID dprx_aux_id, BOOL bEnable, WORD usValue)
{
    volatile WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET_DPCD0(dprx_aux_id);

    if(bEnable == TRUE)
    {
        msWriteByte(REG_DPRX_DPCD0_3F_H + usRegOffsetDPCD0ByID, usValue);

        msWriteByteMask(REG_DPRX_DPCD0_3A_L + usRegOffsetDPCD0ByID, BIT2, BIT2);
    }
    else
    {
        msWriteByte(REG_DPRX_DPCD0_3F_H + usRegOffsetDPCD0ByID, 0); // Write to Reg default value

        msWriteByteMask(REG_DPRX_DPCD0_3A_L + usRegOffsetDPCD0ByID, 0, BIT2);
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
	WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
	BYTE ubOffset = 0x0;

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return 0;
	}

	if((ulDPCDAddress >= DPCD_00202) && (ulDPCDAddress <= DPCD_00207))
	{
		ubOffset = ulDPCDAddress - DPCD_00202;

		return msReadByte(REG_DPRX_AUX_PM0_59_L + usRegOffsetAuxPM0ByID + ubOffset);
	}

	return 0;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetDPCD100PMFlag()
//  [Description]
//					mhal_DPRx_GetDPCD100PMFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetDPCD100PMFlag(DPRx_AUX_ID dprx_aux_id)
{
	volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
	volatile BOOL bEventFlag = FALSE;

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM3_3E_L + usRegOffsetAuxPM3ByID) & BIT1)? TRUE : FALSE;

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetDPCD101PMFlag()
//  [Description]
//					mhal_DPRx_GetDPCD101PMFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetDPCD101PMFlag(DPRx_AUX_ID dprx_aux_id)
{
	volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
	volatile BOOL bEventFlag = FALSE;

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM3_3E_L + usRegOffsetAuxPM3ByID) & BIT0)? TRUE : FALSE;

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetDPCD108PMFlag()
//  [Description]
//					mhal_DPRx_GetDPCD108PMFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetDPCD108PMFlag(DPRx_AUX_ID dprx_aux_id)
{
	volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
	volatile BOOL bEventFlag = FALSE;

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM3_3D_H + usRegOffsetAuxPM3ByID) & BIT7)? TRUE : FALSE;

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetDPCD11AWriteFlag()
//  [Description]
//					mhal_DPRx_GetDPCD11AWriteFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetDPCD11AWriteFlag(DPRx_AUX_ID dprx_aux_id)
{
	volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
	volatile BOOL bEventFlag = FALSE;

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM3_3E_L + usRegOffsetAuxPM3ByID) & BIT4)? TRUE : FALSE;

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_DPCD100PMProc()
//  [Description]
//					mhal_DPRx_DPCD100PMProc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_DPCD100PMProc(DPRx_AUX_ID dprx_aux_id)
{
    volatile WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
	volatile DWORD ulAddress = 0x0;
	volatile BYTE ucAuxLength = 0;

    msWriteByteMask(REG_DPRX_AUX_PM3_3B_L + usRegOffsetAuxPM3ByID, BIT1, BIT1);
	mhal_DPRx_DELAY_NOP(10);
    msWriteByteMask(REG_DPRX_AUX_PM3_3B_L + usRegOffsetAuxPM3ByID, 0, BIT1);

	ulAddress = msReadByte(REG_DPRX_AUX_PM0_1B_L + usRegOffsetAuxPM0ByID) & 0x0F; // reg_aux_rx_addr
    ulAddress = (ulAddress << 16) + msRead2Byte(REG_DPRX_AUX_PM0_1A_L + usRegOffsetAuxPM0ByID);

	ucAuxLength = msReadByte(REG_DPRX_AUX_PM0_1D_L + usRegOffsetAuxPM0ByID); // REG_PM_AUX_1D[7:0]: DP AUX CH RX length

	if((ulAddress == 0xFF) && (ucAuxLength == 1))
	{
	    // Do nothing to fix HW bug that DPCD_100 is triggerred by one byte DPCD_FF R/W
	}
	else
	{

	}

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_DPCD101PMProc()
//  [Description]
//					mhal_DPRx_DPCD101PMProc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_DPCD101PMProc(DPRx_AUX_ID dprx_aux_id)
{
    volatile WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
	volatile DWORD ulAddress = 0x0;
	volatile BYTE ucAuxLength = 0;

    msWriteByteMask(REG_DPRX_AUX_PM3_3B_L + usRegOffsetAuxPM3ByID, BIT0, BIT0);
	mhal_DPRx_DELAY_NOP(10);
    msWriteByteMask(REG_DPRX_AUX_PM3_3B_L + usRegOffsetAuxPM3ByID, 0, BIT0);

	ulAddress = msReadByte(REG_DPRX_AUX_PM0_1B_L + usRegOffsetAuxPM0ByID) & 0x0F; // reg_aux_rx_addr
    ulAddress = (ulAddress << 16) + msRead2Byte(REG_DPRX_AUX_PM0_1A_L + usRegOffsetAuxPM0ByID);

	ucAuxLength = msReadByte(REG_DPRX_AUX_PM0_1D_L + usRegOffsetAuxPM0ByID); // REG_PM_AUX_1D[7:0]: DP AUX CH RX length

	if((ulAddress == 0x100) && (ucAuxLength == 1))
	{
	    // Do nothing to fix HW bug that DPCD_101 is triggerred by one byte DPCD_100 R/W
	}
	else
	{

	}

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_DPCD108PMProc()
//  [Description]
//					mhal_DPRx_DPCD108PMProc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_DPCD108PMProc(DPRx_AUX_ID dprx_aux_id)
{
    volatile WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
	volatile DWORD ulAddress = 0x0;
	volatile BYTE ucAuxLength = 0;

    msWriteByteMask(REG_DPRX_AUX_PM3_3A_H + usRegOffsetAuxPM3ByID, BIT7, BIT7);
	mhal_DPRx_DELAY_NOP(10);
    msWriteByteMask(REG_DPRX_AUX_PM3_3A_H + usRegOffsetAuxPM3ByID, 0, BIT7);

	ulAddress = msReadByte(REG_DPRX_AUX_PM0_1B_L + usRegOffsetAuxPM0ByID) & 0x0F; // reg_aux_rx_addr
    ulAddress = (ulAddress << 16) + msRead2Byte(REG_DPRX_AUX_PM0_1A_L + usRegOffsetAuxPM0ByID);

	ucAuxLength = msReadByte(REG_DPRX_AUX_PM0_1D_L + usRegOffsetAuxPM0ByID); // REG_PM_AUX_1D[7:0]: DP AUX CH RX length

	if((ulAddress == 0x107) && (ucAuxLength == 1))
	{
	    // Do nothing to fix HW bug that DPCD_108 is triggerred by one byte DPCD_107 R/W
	}
	else
	{

	}

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_DPCD11AProc()
//  [Description]
//					mhal_DPRx_DPCD11AProc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_DPCD11AProc(DPRx_AUX_ID dprx_aux_id)
{
    volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);

    msWriteByteMask(REG_DPRX_AUX_PM3_3B_L + usRegOffsetAuxPM3ByID, BIT4, BIT4);
	mhal_DPRx_DELAY_NOP(10);
    msWriteByteMask(REG_DPRX_AUX_PM3_3B_L + usRegOffsetAuxPM3ByID, 0, BIT4);

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
    volatile WORD usRegOffsetPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    volatile WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET_DPCD0(dprx_aux_id);
    BOOL usIsOffline = mhal_DPRx_IsAuxAtOffLine(dprx_aux_id);
	BYTE i;

    //reset pm bank
    mhal_DPRx_SetOffLine(dprx_id, dprx_aux_id, TRUE);
    msWriteByteMask(REG_DPRX_AUX_PM0_49_L + usRegOffsetPM0ByID, BIT7, BIT7);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00108, 0x1);
    mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_02217, 0);
	mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00600, 0x1);
	mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00100, 0x6);
	mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, DPCD_00101, 0x1);

	for(i = 0; i<= 0xF; i++)
	{
		mhal_DPRx_MCUWritePMDPCD(dprx_aux_id, (DPCD_00030 + i), 0x0);
	}

    //reset npm bank
    if(usIsOffline == FALSE)
    {
        mhal_DPRx_SetOffLine(dprx_id, dprx_aux_id, FALSE);
        msWriteByteMask(REG_DPRX_DPCD0_1A_L + usRegOffsetDPCD0ByID, BIT6, BIT6);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_00108, 0x1);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_00110, 0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_00121, 0);
        mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_00160, 0);
    	mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_00100, 0x6);
    	mhal_DPRx_MCUWriteNonPMDPCD(dprx_id, dprx_aux_id, DPCD_00101, 0x1);
    }

    mhal_DPRx_SetOffLine(dprx_id, dprx_aux_id, usIsOffline);

	return;
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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    WORD usCount = DP_AUX_TIMEOUTCNT;

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    msWriteByte(REG_DPRX_AUX_PM0_79_H + usRegOffsetAuxPM0ByID, ucDDCData); // [15:8] The write data to TX FIFO

    msWriteByteMask(REG_DPRX_AUX_PM0_79_L + usRegOffsetAuxPM0ByID, BIT0, BIT0);

    do
    {
        usCount --;
    } while((!(msReadByte(REG_DPRX_AUX_PM0_79_L + usRegOffsetAuxPM0ByID) & BIT2)) && (usCount > 0));

    msWriteByteMask(REG_DPRX_AUX_PM0_79_L + usRegOffsetAuxPM0ByID, BIT1, BIT1);

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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    BYTE ucDDCData = 0;

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return 0;
	}

    msWriteByteMask(REG_DPRX_AUX_PM0_78_L + usRegOffsetAuxPM0ByID, BIT0, BIT0);

    // Must fine tune
    while(!(msReadByte(REG_DPRX_AUX_PM0_78_L + usRegOffsetAuxPM0ByID) & BIT2)) // Wait
    {};

    msWriteByteMask(REG_DPRX_AUX_PM0_78_L + usRegOffsetAuxPM0ByID, BIT1, BIT1);

    ucDDCData = msReadByte(REG_DPRX_AUX_PM0_3A_H + usRegOffsetAuxPM0ByID); // [15:8] MCU read RX FIFO data

    return ucDDCData;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetAuxDDCData_BurstMode()
//  [Description]
//					mhal_DPRx_GetAuxDDCData_BurstMode
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_GetAuxDDCData_BurstMode(DPRx_AUX_ID dprx_aux_id, DWORD ulAddr, BYTE ucByteLength, BYTE *ucDDCData)
{
    volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
	volatile WORD usCount = DPRX_PROG_DPCD_BURSTMODE_TIMEOUT;
	volatile BYTE uctemp = 0;

    msWriteByteMask(REG_DPRX_AUX_PM3_64_H + usRegOffsetAuxPM3ByID, BIT1, BIT1); // Rx burst FIFO enable

	// Must fine tune
	while((!(msReadByte(REG_DPRX_AUX_PM3_64_H + usRegOffsetAuxPM3ByID) & BIT3)) && (usCount > 0)) // Rx FIFO burst done
	{
		usCount--;
	};

	msWriteByteMask(REG_DPRX_AUX_PM3_64_H + usRegOffsetAuxPM3ByID, BIT5, BIT5); // Rx FIFO burst done clear

	for(uctemp = 0; uctemp < ucByteLength; uctemp++)
    {
		ucDDCData[ulAddr + uctemp] = msReadByte(REG_DPRX_AUX_PM3_5C_H + uctemp + usRegOffsetAuxPM3ByID); // MCU read RX FIFO data
	}

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_PutAuxDDCData_BurstMode()
//  [Description]
//					mhal_DPRx_PutAuxDDCData_BurstMode
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_PutAuxDDCData_BurstMode(DPRx_AUX_ID dprx_aux_id, DWORD ulAddr, BYTE ucByteLength, BYTE *ucDDCData)
{
    volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
	volatile WORD usCount = DPRX_PROG_DPCD_BURSTMODE_TIMEOUT;
	volatile BYTE uctemp = 0;

	if(ucByteLength == 1) // HW bug for burst mode Read 1 byte, RD patch code
	{
	 	msWriteByteMask(REG_DPRX_AUX_PM3_7A_H + usRegOffsetAuxPM3ByID, 0, BIT2);    // reg_tx_fifo_length_hw
		msWriteByteMask(REG_DPRX_AUX_PM3_5B_H + usRegOffsetAuxPM3ByID, BIT1, BIT1); // reg_tx_fifo_length
	}

    for(uctemp = 0; uctemp < ucByteLength; uctemp++)
    {
		if(uctemp == 0)
		{
			msWriteByteMask(REG_DPRX_AUX_PM3_53_L + usRegOffsetAuxPM3ByID, (ucDDCData[ulAddr] << 4), 0xF0); // REG_DPRX_AUX_PM3_53[11:4]
			msWriteByteMask(REG_DPRX_AUX_PM3_53_H + usRegOffsetAuxPM3ByID, (ucDDCData[ulAddr] >> 4), 0xF);
		}
		else
		{
			msWriteByte(REG_DPRX_AUX_PM3_54_L + (uctemp - 1) + usRegOffsetAuxPM3ByID, ucDDCData[ulAddr + uctemp]);
		}
    }

	msWriteByteMask(REG_DPRX_AUX_PM3_64_H + usRegOffsetAuxPM3ByID, BIT2, BIT2); // Tx burst FIFO enable

	while((!(msReadByte(REG_DPRX_AUX_PM3_64_H + usRegOffsetAuxPM3ByID) & BIT4)) && (usCount > 0)) // Tx FIFO burst done
	{
		usCount--;
	}

	msWriteByteMask(REG_DPRX_AUX_PM3_64_H + usRegOffsetAuxPM3ByID, BIT6, BIT6); // Tx FIFO burst done clear

    return;
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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (dp_pDPCD_id == DPRx_PROGRAM_DPCD_MAX))
	{
		return;
	}

    msWriteByteMask(REG_DPRX_AUX_PM0_3E_H + usRegOffsetAuxPM0ByID, bEnable? (BIT2 << dp_pDPCD_id) : 0, (BIT2 << dp_pDPCD_id)); // Enable custom address

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetProgDPCDDetectCmdType()
//  [Description]
//                  mhal_DPRx_SetProgDPCDDetectCmdType
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SetProgDPCDDetectCmdType(DPRx_AUX_ID dprx_aux_id, BYTE ucAuxCommandType, DPRx_PROGRAM_DPCD_TYPE dp_pDPCD_id)
{
	volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);

	if(ucAuxCommandType == DPRX_I2C_AND_NATIVE_AUX)
	{
		msWrite2ByteMask(REG_DPRX_AUX_PM3_70_L + usRegOffsetAuxPM3ByID, 0x0, (BIT0 << dp_pDPCD_id));
	}
	else
	{
		msWrite2ByteMask(REG_DPRX_AUX_PM3_70_L + usRegOffsetAuxPM3ByID, (BIT0 << dp_pDPCD_id), (BIT0 << dp_pDPCD_id)); // Only programmable DPCD 0 ~ 7 can detect native aux or I2C cmd type

	    if(ucAuxCommandType == DPRX_NATIVE_AUX_ONLY)
		{
			msWrite2ByteMask(REG_DPRX_AUX_PM3_7C_L + usRegOffsetAuxPM3ByID, 0x0, (BIT0 << dp_pDPCD_id));
		}
		else if(ucAuxCommandType == DPRX_I2C_ONLY)
		{
			msWrite2ByteMask(REG_DPRX_AUX_PM3_7C_L + usRegOffsetAuxPM3ByID, (BIT0 << dp_pDPCD_id), (BIT0 << dp_pDPCD_id));
		}
	}

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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);
    WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (dp_pDPCD_id == DPRx_PROGRAM_DPCD_MAX))
	{
		return;
	}

	if(bEnable == TRUE)
	{
	    if(dp_pDPCD_id < DPRx_PROGRAM_DPCD_4)
	    {
	        msWriteByteMask(REG_DPRX_AUX_PM0_3E_H + usRegOffsetAuxPM0ByID, (BIT2 << dp_pDPCD_id), (BIT2 << dp_pDPCD_id)); // Enable custom address
	        msWrite2Byte(REG_DPRX_AUX_PM0_40_L + DPRX_PROGRAM_DPCD_OFFSET*dp_pDPCD_id + usRegOffsetAuxPM0ByID, pDPCD_Addr & 0xFFFF); // Custom address
	        msWriteByteMask(REG_DPRX_AUX_PM0_41_L + DPRX_PROGRAM_DPCD_OFFSET*dp_pDPCD_id + usRegOffsetAuxPM0ByID, (pDPCD_Addr >> 16), 0x0F); // Custom address
	        msWriteByteMask(REG_DPRX_AUX_PM0_41_H + DPRX_PROGRAM_DPCD_OFFSET*dp_pDPCD_id + usRegOffsetAuxPM0ByID, bAddrGroup? BIT7 : 0, BIT7); // Enable address block
	    }
	    else if((dp_pDPCD_id >= DPRx_PROGRAM_DPCD_4) && (dp_pDPCD_id < DPRx_PROGRAM_DPCD_8))
	    {
	        pDPCD_id = dp_pDPCD_id - DPRx_PROGRAM_DPCD_4;
	        msWriteByteMask(REG_DPRX_AUX_PM2_07_H + DPRX_PROGRAM_DPCD_OFFSET*pDPCD_id + usRegOffsetAuxPM2ByID, BIT5, BIT5); // Enable custom address
	        msWrite2Byte(REG_DPRX_AUX_PM2_06_L + DPRX_PROGRAM_DPCD_OFFSET*pDPCD_id + usRegOffsetAuxPM2ByID, pDPCD_Addr & 0xFFFF); // Custom address
	        msWriteByteMask(REG_DPRX_AUX_PM2_07_L + DPRX_PROGRAM_DPCD_OFFSET*pDPCD_id + usRegOffsetAuxPM2ByID, (pDPCD_Addr >> 16), 0x0F); // Custom address
	        msWriteByteMask(REG_DPRX_AUX_PM2_07_H + DPRX_PROGRAM_DPCD_OFFSET*pDPCD_id + usRegOffsetAuxPM2ByID, bAddrGroup? BIT7 : 0, BIT7); // Enable address block
	    }
	    else if((dp_pDPCD_id >= DPRx_PROGRAM_DPCD_8) && (dp_pDPCD_id < DPRx_PROGRAM_DPCD_12))
	    {
	        pDPCD_id = dp_pDPCD_id - DPRx_PROGRAM_DPCD_8;
	        msWrite2Byte(REG_DPRX_AUX_PM2_32_L + DPRX_PROGRAM_DPCD_OFFSET*pDPCD_id + usRegOffsetAuxPM2ByID, pDPCD_Addr & 0xFFFF); // Custom address
	        msWriteByteMask(REG_DPRX_AUX_PM2_33_L + DPRX_PROGRAM_DPCD_OFFSET*pDPCD_id + usRegOffsetAuxPM2ByID, (pDPCD_Addr >> 16), 0x0F); // Custom address
	    }
		else if(dp_pDPCD_id >= DPRx_PROGRAM_DPCD_12)
        {
            pDPCD_id = dp_pDPCD_id - DPRx_PROGRAM_DPCD_12;
            msWrite2Byte(REG_DPRX_AUX_PM3_4C_L + DPRX_PROGRAM_DPCD_OFFSET*pDPCD_id + usRegOffsetAuxPM3ByID, pDPCD_Addr & 0xFFFF); // Custom address
            msWriteByteMask(REG_DPRX_AUX_PM3_4D_L + DPRX_PROGRAM_DPCD_OFFSET*pDPCD_id + usRegOffsetAuxPM3ByID, (pDPCD_Addr >> 16), 0x0F); // Custom address
        }
	}
	else
	{
		if(dp_pDPCD_id < DPRx_PROGRAM_DPCD_4)
	    {
	        msWriteByteMask(REG_DPRX_AUX_PM0_3E_H + usRegOffsetAuxPM0ByID, 0, (BIT2 << dp_pDPCD_id)); // Enable custom address
	    }
	    else if((dp_pDPCD_id >= DPRx_PROGRAM_DPCD_4) && (dp_pDPCD_id < DPRx_PROGRAM_DPCD_8))
	    {
	        pDPCD_id = dp_pDPCD_id - DPRx_PROGRAM_DPCD_4;
	        msWriteByteMask(REG_DPRX_AUX_PM2_07_H + DPRX_PROGRAM_DPCD_OFFSET*pDPCD_id + usRegOffsetAuxPM2ByID, 0, BIT5); // Enable custom address
	    }
	    else if(dp_pDPCD_id >= DPRx_PROGRAM_DPCD_8)
	    {
	        // No need
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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);
    WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (dp_pDPCD_id == DPRx_PROGRAM_DPCD_MAX))
	{
		return;
	}

    if(dp_pDPCD_id < DPRx_PROGRAM_DPCD_4)
    {
        msWriteByteMask(REG_DPRX_AUX_PM0_49_L + usRegOffsetAuxPM0ByID, (BIT0 << dp_pDPCD_id), (BIT0 << dp_pDPCD_id)); // REG_PM_AUX_48[8]: Write 1 to clear touch programmable DPCD address IRQ
        mhal_DPRx_DELAY_NOP(0xA);
        msWriteByteMask(REG_DPRX_AUX_PM0_49_L + usRegOffsetAuxPM0ByID, 0, (BIT0 << dp_pDPCD_id)); // REG_PM_AUX_48[8]: Write 1 to clear touch programmable DPCD address IRQ
        msWriteByteMask(REG_DPRX_AUX_PM0_48_H + usRegOffsetAuxPM0ByID, bEnable? 0 : (BIT4 << dp_pDPCD_id), (BIT4 << dp_pDPCD_id)); // REG_PM_AUX_48[12]: programmable DPCD mask
    }
    else if((dp_pDPCD_id >= DPRx_PROGRAM_DPCD_4) && (dp_pDPCD_id < DPRx_PROGRAM_DPCD_8))
    {
        dp_pDPCD_id = dp_pDPCD_id - DPRx_PROGRAM_DPCD_4;
        msWriteByteMask(REG_DPRX_AUX_PM2_0E_H + usRegOffsetAuxPM2ByID, (BIT4 << dp_pDPCD_id), (BIT4 << dp_pDPCD_id)); // REG_DPRX_AUX_DPCD_0E[4]: Write 1 to clear touch programmable DPCD address IRQ
        mhal_DPRx_DELAY_NOP(0xA);
        msWriteByteMask(REG_DPRX_AUX_PM2_0E_H + usRegOffsetAuxPM2ByID, 0, (BIT4 << dp_pDPCD_id)); // REG_DPRX_AUX_DPCD_0E[4]: Write 1 to clear touch programmable DPCD address IRQ
        msWriteByteMask(REG_DPRX_AUX_PM2_0E_H + usRegOffsetAuxPM2ByID, bEnable? 0 : (BIT0 << dp_pDPCD_id), (BIT0 << dp_pDPCD_id)); // REG_DPRX_AUX_DPCD_0E[8]: programmable DPCD mask
    }
    else if((dp_pDPCD_id >= DPRx_PROGRAM_DPCD_8)  && (dp_pDPCD_id < DPRx_PROGRAM_DPCD_12))
    {
        dp_pDPCD_id = dp_pDPCD_id - DPRx_PROGRAM_DPCD_8;
        msWriteByteMask(REG_DPRX_AUX_PM2_3A_H + usRegOffsetAuxPM2ByID, (BIT4 << dp_pDPCD_id), (BIT4 << dp_pDPCD_id)); // REG_DPRX_AUX_DPCD_3A[4]: Write 1 to clear touch programmable DPCD address IRQ
        mhal_DPRx_DELAY_NOP(0xA);
        msWriteByteMask(REG_DPRX_AUX_PM2_3A_H + usRegOffsetAuxPM2ByID, 0, (BIT4 << dp_pDPCD_id)); // REG_DPRX_AUX_DPCD_3A[4]: Write 1 to clear touch programmable DPCD address IRQ
        msWriteByteMask(REG_DPRX_AUX_PM2_3A_H + usRegOffsetAuxPM2ByID, bEnable? 0: (BIT0 << dp_pDPCD_id), (BIT0 << dp_pDPCD_id)); // REG_DPRX_AUX_DPCD_3A[8]: programmable DPCD mask
    }
    else if(dp_pDPCD_id >= DPRx_PROGRAM_DPCD_12)
    {
        dp_pDPCD_id = dp_pDPCD_id - DPRx_PROGRAM_DPCD_12;
        msWriteByteMask(REG_DPRX_AUX_PM3_3A_L + usRegOffsetAuxPM3ByID, (BIT0 << dp_pDPCD_id), (BIT0 << dp_pDPCD_id));               // REG_DPRX_AUX_PM2_3A[0]: Write 1 to clear touch programmable DPCD address IRQ
        mhal_DPRx_DELAY_NOP(0xA);
        msWriteByteMask(REG_DPRX_AUX_PM3_3A_L + usRegOffsetAuxPM3ByID, 0, (BIT0 << dp_pDPCD_id));                                   // REG_DPRX_AUX_PM2_3A[0]: Write 1 to clear touch programmable DPCD address IRQ
        msWriteByteMask(REG_DPRX_AUX_PM3_34_L + usRegOffsetAuxPM3ByID, bEnable? 0 : (BIT0 << dp_pDPCD_id), (BIT0 << dp_pDPCD_id));  // REG_DPRX_AUX_PM2_34[0]: Programmable DPCD mask
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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (dp_pDPCD_id == DPRx_XDATAPROGRAM_DPCD_MAX))
	{
		return;
	}

    msWriteByteMask(REG_PIU_MISC0_25_H, 0, BIT0); //25[8] reg_dqm_dma_mux_sel

    if(dp_pDPCD_id == DPRx_XDATAPROGRAM_DPCD_0)
    {
        msWriteByteMask(REG_DPRX_AUX_PM2_57_L + usRegOffsetAuxPM2ByID, (bEnable? BIT0 : 0), BIT0);
        msWriteByteMask(REG_DPRX_AUX_PM2_57_L + usRegOffsetAuxPM2ByID, (bAddrGroup? BIT2 : 0), BIT2);
        msWriteByteMask(REG_DPRX_AUX_PM2_57_L + usRegOffsetAuxPM2ByID, CommandType? BIT1 : 0, BIT1);
        msWrite2Byte(REG_DPRX_AUX_PM2_58_L + usRegOffsetAuxPM2ByID, pDPCD_Addr & 0xFFFF); // Custom address
        msWriteByteMask(REG_DPRX_AUX_PM2_59_L + usRegOffsetAuxPM2ByID, (pDPCD_Addr >> 16), 0x0F); // Custom address
        msWriteByteMask(REG_DPRX_AUX_PM0_7C_H + usRegOffsetAuxPM0ByID, 0, BIT2);

        glubRx_XDataQueue[dprx_aux_id][0][16] = 0x02; //initial read value
        glubRx_XDataQueue[dprx_aux_id][0][17] = 0x00;
        glubRx_XDataQueue[dprx_aux_id][0][18] = 0x02;
		msWrite2Byte(REG_DPRX_AUX_PM2_54_L + usRegOffsetAuxPM2ByID, ((DWORD)(&glubRx_XDataQueue[dprx_aux_id][0][0]))&0xFFFC );   //reg_program_xdata_write_address_start_0
		msWrite2Byte(REG_DPRX_AUX_PM2_55_L + usRegOffsetAuxPM2ByID, ((DWORD)(&glubRx_XDataQueue[dprx_aux_id][0][16]))&0xFFFC );   //reg_program_xdata_read_address_start_0
    }
    else if(dp_pDPCD_id == DPRx_XDATAPROGRAM_DPCD_1)
    {
        msWriteByteMask(REG_DPRX_AUX_PM2_57_L + usRegOffsetAuxPM2ByID, (bEnable? BIT4 : 0), BIT4);
        msWriteByteMask(REG_DPRX_AUX_PM2_57_L + usRegOffsetAuxPM2ByID, (bAddrGroup? BIT6 : 0), BIT6);
        msWriteByteMask(REG_DPRX_AUX_PM2_57_L + usRegOffsetAuxPM2ByID, CommandType? BIT5 : 0, BIT5);
        msWrite2Byte(REG_DPRX_AUX_PM2_7A_L + usRegOffsetAuxPM2ByID, pDPCD_Addr & 0xFFFF); // Custom address
        msWriteByteMask(REG_DPRX_AUX_PM2_7B_L + usRegOffsetAuxPM2ByID, (pDPCD_Addr >> 16), 0x0F); // Custom address
        msWriteByteMask(REG_DPRX_AUX_PM0_49_H + usRegOffsetAuxPM0ByID, 0, BIT2);

        //glubRx_XDataQueue[dprx_aux_id][1][16] = 0xbb; //initial read value

		msWrite2Byte(REG_DPRX_AUX_PM2_7C_L + usRegOffsetAuxPM2ByID, ((DWORD)(&glubRx_XDataQueue[dprx_aux_id][1][0]))&0xFFFC );   //reg_program_xdata_write_address_start_1
		msWrite2Byte(REG_DPRX_AUX_PM2_2E_L + usRegOffsetAuxPM2ByID, ((DWORD)(&glubRx_XDataQueue[dprx_aux_id][1][16]))&0xFFFC );   //reg_program_xdata_read_address_start_1
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
	WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (dp_pDPCD_id == DPRx_XDATAPROGRAM_DPCD_MAX))
	{
		return;
	}

    if(dp_pDPCD_id == DPRx_XDATAPROGRAM_DPCD_0)
    {
        msWriteByteMask(REG_DPRX_AUX_PM0_7C_H + usRegOffsetAuxPM0ByID, BIT3, BIT3);			  // REG_PM_AUX_7C[11]: Write 1 to clear touch XDATA programmable DPCD address IRQ
        msWriteByteMask(REG_DPRX_AUX_PM0_7C_H + usRegOffsetAuxPM0ByID, 0, BIT3);				  // REG_PM_AUX_7C[11]: Write 1 to clear touch XDATA programmable DPCD address IRQ
        msWriteByteMask(REG_DPRX_AUX_PM0_7C_H + usRegOffsetAuxPM0ByID, bEnable? 0 : BIT2, BIT2); // REG_PM_AUX_7C[10]: XDATA programmable DPCD mask
    }
    else if(dp_pDPCD_id == DPRx_XDATAPROGRAM_DPCD_1)
    {
        msWriteByteMask(REG_DPRX_AUX_PM0_4A_H + usRegOffsetAuxPM0ByID, BIT7, BIT7);			  // REG_PM_AUX_49[11]: Write 1 to clear touch XDATA programmable DPCD address IRQ
        msWriteByteMask(REG_DPRX_AUX_PM0_4A_H + usRegOffsetAuxPM0ByID, 0, BIT7);				  // REG_PM_AUX_49[11]: Write 1 to clear touch XDATA programmable DPCD address IRQ
        msWriteByteMask(REG_DPRX_AUX_PM0_4A_H + usRegOffsetAuxPM0ByID, bEnable? 0 : BIT3, BIT3); // REG_PM_AUX_49[10]: XDATA programmable DPCD mask
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
BOOL mhal_DPRx_ProgramDPCD0Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue)
{
    volatile WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
    volatile BOOL bDPCDWrite = FALSE;

    #if(DPRX_PROGRAM_DPCD0_FUNC_SELECT != DPRX_POWERFUL1BYTE)
    volatile BYTE ucAuxCommand = 0;
    volatile BYTE ucAuxLength = 0;
    volatile DWORD ulAddress = 0x0;
    #endif

    #if((DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD) && (DPRX_PROGRAM_DPCD0_FUNC_SELECT != DPRX_POWERFUL1BYTE))
    volatile BYTE uctemp = 0;
	#endif

	#if(DPRX_PROGRAM_DPCD0_FUNC_SELECT == DPRX_POWERFUL1BYTE)
        ucDataQueue[0] = msReadByte(REG_DPRX_AUX_PM3_65_H + usRegOffsetAuxPM3ByID);
	#else
        ucAuxCommand = msReadByte(REG_DPRX_AUX_PM0_1C_L + usRegOffsetAuxPM0ByID) & 0x0F; // REG_PM_AUX_1C[3:0]: DP AUX CH RX command
        ucAuxLength = msReadByte(REG_DPRX_AUX_PM0_1D_L + usRegOffsetAuxPM0ByID); // REG_PM_AUX_1D[7:0]: DP AUX CH RX length

        ulAddress = msReadByte(REG_DPRX_AUX_PM0_1B_L + usRegOffsetAuxPM0ByID) & 0x0F; // reg_aux_rx_addr
        ulAddress = (ulAddress << 16) + msRead2Byte(REG_DPRX_AUX_PM0_1A_L + usRegOffsetAuxPM0ByID);
        ulAddress = ulAddress - DPRX_PROGRAM_DPCD0_ADDERSS;

        msWriteByteMask(REG_DPRX_AUX_PM0_18_L + usRegOffsetAuxPM0ByID, BIT1, BIT1); // reg_aux_reply_mcu

        if((ucAuxCommand == AUX_CMD_NATIVE_R) || (ucAuxCommand == AUX_CMD_I2C_R) || (ucAuxCommand == AUX_CMD_I2C_R_MOT0))
        {
            if(ucAuxLength <= DPRX_AUX_COMMAND_MAX_LENGTH)
            {
            	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
                for(uctemp = 0; uctemp < ucAuxLength; uctemp++) // Can't write 16-byte data
                {
                    mhal_DPRx_PutAuxDDCData(dprx_aux_id, ucDataQueue[(ulAddress + uctemp)]);
                }
    			#elif (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST)
    			mhal_DPRx_PutAuxDDCData_BurstMode(dprx_aux_id, ulAddress, ucAuxLength, ucDataQueue);
    			#endif
            }
        }
        else if((ucAuxCommand == AUX_CMD_NATIVE_W) || (ucAuxCommand == AUX_CMD_I2C_W) || (ucAuxCommand == AUX_CMD_I2C_W_MOT0))
        {
            if(ucAuxLength <= DPRX_AUX_COMMAND_MAX_LENGTH)
            {
            	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
                for(uctemp = 0; uctemp < ucAuxLength; uctemp++)
                {
                    ucDataQueue[(ulAddress + uctemp)] = mhal_DPRx_GetAuxDDCData(dprx_aux_id);
                }
    			#elif (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST)
    			mhal_DPRx_GetAuxDDCData_BurstMode(dprx_aux_id, ulAddress, ucAuxLength, ucDataQueue);
    			#endif
            }

            bDPCDWrite = TRUE;
        }

    	msWriteByteMask(REG_DPRX_AUX_PM0_1F_L + usRegOffsetAuxPM0ByID, 0x0, BIT1|BIT0); // reg_aux_tx_cmd // 0: ACK, 1: NACK, 2: DEFER

        msWriteByteMask(REG_DPRX_AUX_PM0_3B_H + usRegOffsetAuxPM0ByID, BIT0, BIT0); // Set MCU reply "Ready"
        msWriteByteMask(REG_DPRX_AUX_PM0_18_L + usRegOffsetAuxPM0ByID, 0, BIT1); 	 // AUX reply by HW
        msWriteByteMask(REG_DPRX_AUX_PM0_49_L + usRegOffsetAuxPM0ByID, BIT0, BIT0); // Clear program DPCD 0 IRQ

    	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST) // HW bug for burst mode Read 1 byte, RD patch code
    	msWriteByteMask(REG_DPRX_AUX_PM3_7A_H + usRegOffsetAuxPM3ByID, BIT2, BIT2);
    	msWriteByteMask(REG_DPRX_AUX_PM3_5B_H + usRegOffsetAuxPM3ByID, 0, BIT1);
    	#endif
	#endif

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
BOOL mhal_DPRx_ProgramDPCD1Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue)
{
	volatile WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
    volatile BOOL bDPCDWrite = FALSE;
    volatile BYTE ucAuxCommand = 0;
    volatile BYTE ucAuxLength = 0;
    volatile DWORD ulAddress = 0x0;

	#if(DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
	volatile BYTE uctemp = 0;
	#endif

	ucAuxCommand = msReadByte(REG_DPRX_AUX_PM0_1C_L + usRegOffsetAuxPM0ByID) & 0x0F; // REG_PM_AUX_1C[3:0]: DP AUX CH RX command
	ucAuxLength = msReadByte(REG_DPRX_AUX_PM0_1D_L + usRegOffsetAuxPM0ByID); // REG_PM_AUX_1D[7:0]: DP AUX CH RX length

	ulAddress = msReadByte(REG_DPRX_AUX_PM0_1B_L + usRegOffsetAuxPM0ByID) & 0x0F; // reg_aux_rx_addr
	ulAddress = (ulAddress << 16) + msRead2Byte(REG_DPRX_AUX_PM0_1A_L + usRegOffsetAuxPM0ByID);
	ulAddress = ulAddress - DPRX_PROGRAM_DPCD1_ADDERSS;

	msWriteByteMask(REG_DPRX_AUX_PM0_18_L + usRegOffsetAuxPM0ByID, BIT1, BIT1);

    if(ucAuxCommand == AUX_CMD_NATIVE_R)
    {
    	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
        if(ucAuxLength <= DPRX_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++) // Can't write 16-byte data
            {
               mhal_DPRx_PutAuxDDCData(dprx_aux_id, ucDataQueue[(ulAddress + uctemp)]);
            }
        }
		#elif (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST)
		mhal_DPRx_PutAuxDDCData_BurstMode(dprx_aux_id, ulAddress, ucAuxLength, ucDataQueue);
		#endif
    }
    else if((ucAuxCommand == AUX_CMD_NATIVE_W) || (ucAuxCommand == AUX_CMD_I2C_W))
    {
    	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
        if(ucAuxLength <= DPRX_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++)
            {
                ucDataQueue[(ulAddress + uctemp)] = mhal_DPRx_GetAuxDDCData(dprx_aux_id);
            }
        }
		#elif (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST)
		mhal_DPRx_GetAuxDDCData_BurstMode(dprx_aux_id, ulAddress, ucAuxLength, ucDataQueue);
		#endif

        bDPCDWrite = TRUE;
    }

	msWriteByteMask(REG_DPRX_AUX_PM0_1F_L + usRegOffsetAuxPM0ByID, 0x0, BIT1|BIT0); // reg_aux_tx_cmd // 0: ACK, 1: NACK, 2: DEFER

    msWriteByteMask(REG_DPRX_AUX_PM0_3B_H + usRegOffsetAuxPM0ByID, BIT0, BIT0); // Set MCU reply "Ready"
    msWriteByteMask(REG_DPRX_AUX_PM0_18_L + usRegOffsetAuxPM0ByID, 0, BIT1); 	 // AUX reply by HW
    msWriteByteMask(REG_DPRX_AUX_PM0_49_L + usRegOffsetAuxPM0ByID, BIT1, BIT1); // Clear program DPCD 1 IRQ

	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST) // HW bug for burst mode Read 1 byte, RD patch code
	msWriteByteMask(REG_DPRX_AUX_PM3_7A_H + usRegOffsetAuxPM3ByID, BIT2, BIT2);
	msWriteByteMask(REG_DPRX_AUX_PM3_5B_H + usRegOffsetAuxPM3ByID, 0, BIT1);
	#endif

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
BOOL mhal_DPRx_ProgramDPCD2Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue)
{
	volatile WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
    volatile BOOL bDPCDWrite = FALSE;
    volatile BYTE ucAuxCommand = 0;
    volatile BYTE ucAuxLength = 0;
    volatile DWORD ulAddress = 0x0;

	#if(DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
	volatile BYTE uctemp = 0;
	#endif

	ucAuxCommand = msReadByte(REG_DPRX_AUX_PM0_1C_L + usRegOffsetAuxPM0ByID) & 0x0F; // REG_PM_AUX_1C[3:0]: DP AUX CH RX command
	ucAuxLength = msReadByte(REG_DPRX_AUX_PM0_1D_L + usRegOffsetAuxPM0ByID); // REG_PM_AUX_1D[7:0]: DP AUX CH RX length

	ulAddress = msReadByte(REG_DPRX_AUX_PM0_1B_L + usRegOffsetAuxPM0ByID) & 0x0F; // reg_aux_rx_addr
	ulAddress = (ulAddress << 16) + msRead2Byte(REG_DPRX_AUX_PM0_1A_L + usRegOffsetAuxPM0ByID);
	ulAddress = ulAddress - DPRX_PROGRAM_DPCD2_ADDERSS;

	msWriteByteMask(REG_DPRX_AUX_PM0_18_L + usRegOffsetAuxPM0ByID, BIT1, BIT1);

    if(ucAuxCommand == AUX_CMD_NATIVE_R)
    {
    	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
        if(ucAuxLength <= DPRX_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++) // Can't write 16-byte data
            {
               mhal_DPRx_PutAuxDDCData(dprx_aux_id, ucDataQueue[(ulAddress + uctemp)]);
            }
        }
		#elif (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST)
		mhal_DPRx_PutAuxDDCData_BurstMode(dprx_aux_id, ulAddress, ucAuxLength, ucDataQueue);
		#endif
    }
    else if((ucAuxCommand == AUX_CMD_NATIVE_W) || (ucAuxCommand == AUX_CMD_I2C_W))
    {
    	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
        if(ucAuxLength <= DPRX_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++)
            {
                ucDataQueue[(ulAddress + uctemp)] = mhal_DPRx_GetAuxDDCData(dprx_aux_id);
            }
        }
		#elif (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST)
		mhal_DPRx_GetAuxDDCData_BurstMode(dprx_aux_id, ulAddress, ucAuxLength, ucDataQueue);
		#endif

        bDPCDWrite = TRUE;
    }

	msWriteByteMask(REG_DPRX_AUX_PM0_1F_L + usRegOffsetAuxPM0ByID, 0x0, BIT1|BIT0); // reg_aux_tx_cmd // 0: ACK, 1: NACK, 2: DEFER

    msWriteByteMask(REG_DPRX_AUX_PM0_3B_H + usRegOffsetAuxPM0ByID, BIT0, BIT0); // Set MCU reply "Ready"
    msWriteByteMask(REG_DPRX_AUX_PM0_18_L + usRegOffsetAuxPM0ByID, 0, BIT1); 	 // AUX reply by HW
    msWriteByteMask(REG_DPRX_AUX_PM0_49_L + usRegOffsetAuxPM0ByID, BIT2, BIT2); // Clear program DPCD 2 IRQ

	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST) // HW bug for burst mode Read 1 byte, RD patch code
	msWriteByteMask(REG_DPRX_AUX_PM3_7A_H + usRegOffsetAuxPM3ByID, BIT2, BIT2);
	msWriteByteMask(REG_DPRX_AUX_PM3_5B_H + usRegOffsetAuxPM3ByID, 0, BIT1);
	#endif

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
BOOL mhal_DPRx_ProgramDPCD3Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue)
{
	volatile WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
    volatile BOOL bDPCDWrite = FALSE;
    volatile BYTE ucAuxCommand = 0;
    volatile BYTE ucAuxLength = 0;
    volatile DWORD ulAddress = 0x0;

	#if(DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
	volatile BYTE uctemp = 0;
	#endif

	ucAuxCommand = msReadByte(REG_DPRX_AUX_PM0_1C_L + usRegOffsetAuxPM0ByID) & 0x0F; // REG_PM_AUX_1C[3:0]: DP AUX CH RX command
	ucAuxLength = msReadByte(REG_DPRX_AUX_PM0_1D_L + usRegOffsetAuxPM0ByID); // REG_PM_AUX_1D[7:0]: DP AUX CH RX length

	ulAddress = msReadByte(REG_DPRX_AUX_PM0_1B_L + usRegOffsetAuxPM0ByID) & 0x0F; // reg_aux_rx_addr
	ulAddress = (ulAddress << 16) + msRead2Byte(REG_DPRX_AUX_PM0_1A_L + usRegOffsetAuxPM0ByID);
	ulAddress = ulAddress - DPRX_PROGRAM_DPCD3_ADDERSS;

	msWriteByteMask(REG_DPRX_AUX_PM0_18_L + usRegOffsetAuxPM0ByID, BIT1, BIT1);

    if(ucAuxCommand == AUX_CMD_NATIVE_R)
    {
    	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
        if(ucAuxLength <= DPRX_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++) // Can't write 16-byte data
            {
               mhal_DPRx_PutAuxDDCData(dprx_aux_id, ucDataQueue[(ulAddress + uctemp)]);
            }
        }
		#elif (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST)
		mhal_DPRx_PutAuxDDCData_BurstMode(dprx_aux_id, ulAddress, ucAuxLength, ucDataQueue);
		#endif
    }
    else if((ucAuxCommand == AUX_CMD_NATIVE_W) || (ucAuxCommand == AUX_CMD_I2C_W))
    {
    	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
        if(ucAuxLength <= DPRX_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++)
            {
                ucDataQueue[(ulAddress + uctemp)] = mhal_DPRx_GetAuxDDCData(dprx_aux_id);
            }
        }
		#elif (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST)
		mhal_DPRx_GetAuxDDCData_BurstMode(dprx_aux_id, ulAddress, ucAuxLength, ucDataQueue);
		#endif

        bDPCDWrite = TRUE;
    }

	msWriteByteMask(REG_DPRX_AUX_PM0_1F_L + usRegOffsetAuxPM0ByID, 0x0, BIT1|BIT0); // reg_aux_tx_cmd // 0: ACK, 1: NACK, 2: DEFER

    msWriteByteMask(REG_DPRX_AUX_PM0_3B_H + usRegOffsetAuxPM0ByID, BIT0, BIT0); // Set MCU reply "Ready"
    msWriteByteMask(REG_DPRX_AUX_PM0_18_L + usRegOffsetAuxPM0ByID, 0, BIT1); 	 // AUX reply by HW
    msWriteByteMask(REG_DPRX_AUX_PM0_49_L + usRegOffsetAuxPM0ByID, BIT3, BIT3); // Clear program DPCD 3 IRQ

	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST) // HW bug for burst mode Read 1 byte, RD patch code
	msWriteByteMask(REG_DPRX_AUX_PM3_7A_H + usRegOffsetAuxPM3ByID, BIT2, BIT2);
	msWriteByteMask(REG_DPRX_AUX_PM3_5B_H + usRegOffsetAuxPM3ByID, 0, BIT1);
	#endif

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
BOOL mhal_DPRx_ProgramDPCD4Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue)
{
	volatile WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
	volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
	volatile BOOL bDPCDWrite = FALSE;
    volatile BYTE ucAuxCommand = 0;
    volatile BYTE ucAuxLength = 0;
    volatile DWORD ulAddress = 0x0;

	#if(DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
	volatile BYTE uctemp = 0;
	#endif

    ucAuxCommand = msReadByte(REG_DPRX_AUX_PM0_1C_L + usRegOffsetAuxPM0ByID) & 0x0F; // REG_PM_AUX_1C[3:0]: DP AUX CH RX command
    ucAuxLength = msReadByte(REG_DPRX_AUX_PM0_1D_L + usRegOffsetAuxPM0ByID); // REG_PM_AUX_1D[7:0]: DP AUX CH RX length

    ulAddress = msReadByte(REG_DPRX_AUX_PM0_1B_L + usRegOffsetAuxPM0ByID) & 0x0F; // reg_aux_rx_addr
    ulAddress = (ulAddress << 16) + msRead2Byte(REG_DPRX_AUX_PM0_1A_L + usRegOffsetAuxPM0ByID);
    ulAddress = ulAddress - DPRX_PROGRAM_DPCD4_ADDERSS;

    msWriteByteMask(REG_DPRX_AUX_PM0_18_L + usRegOffsetAuxPM0ByID, BIT1, BIT1);

    if(ucAuxCommand == AUX_CMD_NATIVE_R)
    {
    	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
        if(ucAuxLength <= DPRX_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++) // Can't write 16-byte data
            {
               mhal_DPRx_PutAuxDDCData(dprx_aux_id, ucDataQueue[(ulAddress + uctemp)]);
            }
        }
		#elif (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST)
		mhal_DPRx_PutAuxDDCData_BurstMode(dprx_aux_id, ulAddress, ucAuxLength, ucDataQueue);
		#endif
    }
    else if((ucAuxCommand == AUX_CMD_NATIVE_W) || (ucAuxCommand == AUX_CMD_I2C_W))
    {
    	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
        if(ucAuxLength <= DPRX_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++)
            {
                ucDataQueue[(ulAddress + uctemp)] = mhal_DPRx_GetAuxDDCData(dprx_aux_id);
            }
        }
		#elif (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST)
		mhal_DPRx_GetAuxDDCData_BurstMode(dprx_aux_id, ulAddress, ucAuxLength, ucDataQueue);
		#endif

        bDPCDWrite = TRUE;
    }

	msWriteByteMask(REG_DPRX_AUX_PM0_1F_L + usRegOffsetAuxPM0ByID, 0x0, BIT1|BIT0); // reg_aux_tx_cmd // 0: ACK, 1: NACK, 2: DEFER

    msWriteByteMask(REG_DPRX_AUX_PM0_3B_H + usRegOffsetAuxPM0ByID, BIT0, BIT0); // Set MCU reply "Ready"
    msWriteByteMask(REG_DPRX_AUX_PM0_18_L + usRegOffsetAuxPM0ByID, 0, BIT1); // AUX reply by HW
    msWriteByteMask(REG_DPRX_AUX_PM2_0E_H + usRegOffsetAuxPM0ByID, BIT4, BIT4); // Clear program DPCD 4 IRQ

	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST) // HW bug for burst mode Read 1 byte, RD patch code
	msWriteByteMask(REG_DPRX_AUX_PM3_7A_H + usRegOffsetAuxPM3ByID, BIT2, BIT2);
	msWriteByteMask(REG_DPRX_AUX_PM3_5B_H + usRegOffsetAuxPM3ByID, 0, BIT1);
	#endif

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
BOOL mhal_DPRx_ProgramDPCD5Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue)
{
	volatile WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
	volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
	volatile BOOL bDPCDWrite = FALSE;
    volatile BYTE ucAuxCommand = 0;
    volatile BYTE ucAuxLength = 0;
    volatile DWORD ulAddress = 0x0;

	#if(DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
	volatile BYTE uctemp = 0;
	#endif

    ucAuxCommand = msReadByte(REG_DPRX_AUX_PM0_1C_L + usRegOffsetAuxPM0ByID) & 0x0F; // REG_PM_AUX_1C[3:0]: DP AUX CH RX command
    ucAuxLength = msReadByte(REG_DPRX_AUX_PM0_1D_L + usRegOffsetAuxPM0ByID); // REG_PM_AUX_1D[7:0]: DP AUX CH RX length

    ulAddress = msReadByte(REG_DPRX_AUX_PM0_1B_L + usRegOffsetAuxPM0ByID) & 0x0F; // reg_aux_rx_addr
    ulAddress = (ulAddress << 16) + msRead2Byte(REG_DPRX_AUX_PM0_1A_L + usRegOffsetAuxPM0ByID);
    ulAddress = ulAddress - DPRX_PROGRAM_DPCD5_ADDERSS;

    msWriteByteMask(REG_DPRX_AUX_PM0_18_L + usRegOffsetAuxPM0ByID, BIT1, BIT1);

    if(ucAuxCommand == AUX_CMD_NATIVE_R)
    {
    	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
        if(ucAuxLength <= DPRX_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++) // Can't write 16-byte data
            {
               mhal_DPRx_PutAuxDDCData(dprx_aux_id, ucDataQueue[(ulAddress + uctemp)]);
            }
        }
		#elif (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST)
		mhal_DPRx_PutAuxDDCData_BurstMode(dprx_aux_id, ulAddress, ucAuxLength, ucDataQueue);
		#endif
    }
    else if((ucAuxCommand == AUX_CMD_NATIVE_W) || (ucAuxCommand == AUX_CMD_I2C_W))
    {
    	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
        if(ucAuxLength <= DPRX_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++)
            {
                ucDataQueue[(ulAddress + uctemp)] = mhal_DPRx_GetAuxDDCData(dprx_aux_id);
            }
        }
		#elif (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST)
		mhal_DPRx_GetAuxDDCData_BurstMode(dprx_aux_id, ulAddress, ucAuxLength, ucDataQueue);
		#endif

        bDPCDWrite = TRUE;
    }

	msWriteByteMask(REG_DPRX_AUX_PM0_1F_L + usRegOffsetAuxPM0ByID, 0x0, BIT1|BIT0); // reg_aux_tx_cmd // 0: ACK, 1: NACK, 2: DEFER

    msWriteByteMask(REG_DPRX_AUX_PM0_3B_H + usRegOffsetAuxPM0ByID, BIT0, BIT0); // Set MCU reply "Ready"
    msWriteByteMask(REG_DPRX_AUX_PM0_18_L + usRegOffsetAuxPM0ByID, 0, BIT1); // AUX reply by HW
    msWriteByteMask(REG_DPRX_AUX_PM2_0E_H + usRegOffsetAuxPM0ByID, BIT5, BIT5); // Clear program DPCD 5 IRQ

	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST) // HW bug for burst mode Read 1 byte, RD patch code
	msWriteByteMask(REG_DPRX_AUX_PM3_7A_H + usRegOffsetAuxPM3ByID, BIT2, BIT2);
	msWriteByteMask(REG_DPRX_AUX_PM3_5B_H + usRegOffsetAuxPM3ByID, 0, BIT1);
	#endif

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
BOOL mhal_DPRx_ProgramDPCD6Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue)
{
	volatile WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
	volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
	volatile BOOL bDPCDWrite = FALSE;
    volatile BYTE ucAuxCommand = 0;
    volatile BYTE ucAuxLength = 0;
    volatile DWORD ulAddress = 0x0;

	#if(DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
	volatile BYTE uctemp = 0;
	#endif

    ucAuxCommand = msReadByte(REG_DPRX_AUX_PM0_1C_L + usRegOffsetAuxPM0ByID) & 0x0F; // REG_PM_AUX_1C[3:0]: DP AUX CH RX command
    ucAuxLength = msReadByte(REG_DPRX_AUX_PM0_1D_L + usRegOffsetAuxPM0ByID); // REG_PM_AUX_1D[7:0]: DP AUX CH RX length

    ulAddress = msReadByte(REG_DPRX_AUX_PM0_1B_L + usRegOffsetAuxPM0ByID) & 0x0F; // reg_aux_rx_addr
    ulAddress = (ulAddress << 16) + msRead2Byte(REG_DPRX_AUX_PM0_1A_L + usRegOffsetAuxPM0ByID);
    ulAddress = ulAddress - DPRX_PROGRAM_DPCD6_ADDERSS;

    msWriteByteMask(REG_DPRX_AUX_PM0_18_L + usRegOffsetAuxPM0ByID, BIT1, BIT1);

    if(ucAuxCommand == AUX_CMD_NATIVE_R)
    {
    	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
        if(ucAuxLength <= DPRX_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++) // Can't write 16-byte data
            {
               mhal_DPRx_PutAuxDDCData(dprx_aux_id, ucDataQueue[(ulAddress + uctemp)]);
            }
        }
		#elif (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST)
		mhal_DPRx_PutAuxDDCData_BurstMode(dprx_aux_id, ulAddress, ucAuxLength, ucDataQueue);
		#endif
    }
    else if((ucAuxCommand == AUX_CMD_NATIVE_W) || (ucAuxCommand == AUX_CMD_I2C_W))
    {
    	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
        if(ucAuxLength <= DPRX_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++)
            {
                ucDataQueue[(ulAddress + uctemp)] = mhal_DPRx_GetAuxDDCData(dprx_aux_id);
            }
        }
		#elif (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST)
		mhal_DPRx_GetAuxDDCData_BurstMode(dprx_aux_id, ulAddress, ucAuxLength, ucDataQueue);
		#endif

        bDPCDWrite = TRUE;
    }

	msWriteByteMask(REG_DPRX_AUX_PM0_1F_L + usRegOffsetAuxPM0ByID, 0x0, BIT1|BIT0); // reg_aux_tx_cmd // 0: ACK, 1: NACK, 2: DEFER

    msWriteByteMask(REG_DPRX_AUX_PM0_3B_H + usRegOffsetAuxPM0ByID, BIT0, BIT0); // Set MCU reply "Ready"
    msWriteByteMask(REG_DPRX_AUX_PM0_18_L + usRegOffsetAuxPM0ByID, 0, BIT1); // AUX reply by HW
    msWriteByteMask(REG_DPRX_AUX_PM2_0E_H + usRegOffsetAuxPM0ByID, BIT6, BIT6); // Clear program DPCD 6 IRQ

	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST) // HW bug for burst mode Read 1 byte, RD patch code
	msWriteByteMask(REG_DPRX_AUX_PM3_7A_H + usRegOffsetAuxPM3ByID, BIT2, BIT2);
	msWriteByteMask(REG_DPRX_AUX_PM3_5B_H + usRegOffsetAuxPM3ByID, 0, BIT1);
	#endif

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
BOOL mhal_DPRx_ProgramDPCD7Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue)
{
	volatile WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
	volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
	volatile BOOL bDPCDWrite = FALSE;
    volatile BYTE ucAuxCommand = 0;
    volatile BYTE ucAuxLength = 0;
    volatile DWORD ulAddress = 0x0;

	#if(DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
	volatile BYTE uctemp = 0;
	#endif

    ucAuxCommand = msReadByte(REG_DPRX_AUX_PM0_1C_L + usRegOffsetAuxPM0ByID) & 0x0F; // REG_PM_AUX_1C[3:0]: DP AUX CH RX command
    ucAuxLength = msReadByte(REG_DPRX_AUX_PM0_1D_L + usRegOffsetAuxPM0ByID); // REG_PM_AUX_1D[7:0]: DP AUX CH RX length

    ulAddress = msReadByte(REG_DPRX_AUX_PM0_1B_L + usRegOffsetAuxPM0ByID) & 0x0F; // reg_aux_rx_addr
    ulAddress = (ulAddress << 16) + msRead2Byte(REG_DPRX_AUX_PM0_1A_L + usRegOffsetAuxPM0ByID);
    ulAddress = ulAddress - DPRX_PROGRAM_DPCD7_ADDERSS;

    msWriteByteMask(REG_DPRX_AUX_PM0_18_L + usRegOffsetAuxPM0ByID, BIT1, BIT1);

    if(ucAuxCommand == AUX_CMD_NATIVE_R)
    {
    	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
        if(ucAuxLength <= DPRX_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++) // Can't write 16-byte data
            {
               mhal_DPRx_PutAuxDDCData(dprx_aux_id, ucDataQueue[(ulAddress + uctemp)]);
            }
        }
		#elif (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST)
		mhal_DPRx_PutAuxDDCData_BurstMode(dprx_aux_id, ulAddress, ucAuxLength, ucDataQueue);
		#endif
    }
    else if((ucAuxCommand == AUX_CMD_NATIVE_W) || (ucAuxCommand == AUX_CMD_I2C_W))
    {
    	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
        if(ucAuxLength <= DPRX_AUX_COMMAND_MAX_LENGTH)
        {
            for(uctemp = 0; uctemp < ucAuxLength; uctemp++)
            {
                ucDataQueue[(ulAddress + uctemp)] = mhal_DPRx_GetAuxDDCData(dprx_aux_id);
            }
        }
		#elif (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST)
		mhal_DPRx_GetAuxDDCData_BurstMode(dprx_aux_id, ulAddress, ucAuxLength, ucDataQueue);
		#endif

        bDPCDWrite = TRUE;
    }

	msWriteByteMask(REG_DPRX_AUX_PM0_1F_L + usRegOffsetAuxPM0ByID, 0x0, BIT1|BIT0); // reg_aux_tx_cmd // 0: ACK, 1: NACK, 2: DEFER

    msWriteByteMask(REG_DPRX_AUX_PM0_3B_H + usRegOffsetAuxPM0ByID, BIT0, BIT0); // Set MCU reply "Ready"
    msWriteByteMask(REG_DPRX_AUX_PM0_18_L + usRegOffsetAuxPM0ByID, 0, BIT1); // AUX reply by HW
    msWriteByteMask(REG_DPRX_AUX_PM2_0E_H + usRegOffsetAuxPM0ByID, BIT7, BIT7); // Clear program DPCD 7 IRQ

	#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST) // HW bug for burst mode Read 1 byte, RD patch code
	msWriteByteMask(REG_DPRX_AUX_PM3_7A_H + usRegOffsetAuxPM3ByID, BIT2, BIT2);
	msWriteByteMask(REG_DPRX_AUX_PM3_5B_H + usRegOffsetAuxPM3ByID, 0, BIT1);
	#endif

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
void mhal_DPRx_ProgramDPCD8Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue)
{
	WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

    if((dprx_aux_id == DPRx_AUX_ID_MAX) || (ucDataQueue == NULL))
	{
		return;
	}

    msWriteByteMask(REG_DPRX_AUX_PM2_3A_H + usRegOffsetAuxPM2ByID, BIT4, BIT4);
	mhal_DPRx_DELAY_NOP(0x30);
    msWriteByteMask(REG_DPRX_AUX_PM2_3A_H + usRegOffsetAuxPM2ByID, 0, BIT4);

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
	WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

    if((dprx_aux_id == DPRx_AUX_ID_MAX) || (ucDataQueue == NULL))
	{
		return;
	}

    msWriteByteMask(REG_DPRX_AUX_PM2_3A_H + usRegOffsetAuxPM2ByID, BIT5, BIT5);
	mhal_DPRx_DELAY_NOP(0x30);
    msWriteByteMask(REG_DPRX_AUX_PM2_3A_H + usRegOffsetAuxPM2ByID, 0, BIT5);

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
	WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

    if((dprx_aux_id == DPRx_AUX_ID_MAX) || (ucDataQueue == NULL))
	{
		return;
	}

    msWriteByteMask(REG_DPRX_AUX_PM2_3A_H + usRegOffsetAuxPM2ByID, BIT6, BIT6);
	mhal_DPRx_DELAY_NOP(0x30);
    msWriteByteMask(REG_DPRX_AUX_PM2_3A_H + usRegOffsetAuxPM2ByID, 0, BIT6);

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
	WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

    if((dprx_aux_id == DPRx_AUX_ID_MAX) || (ucDataQueue == NULL))
	{
		return;
	}

    msWriteByteMask(REG_DPRX_AUX_PM2_3A_H + usRegOffsetAuxPM2ByID, BIT7, BIT7);
	mhal_DPRx_DELAY_NOP(0x30);
    msWriteByteMask(REG_DPRX_AUX_PM2_3A_H + usRegOffsetAuxPM2ByID, 0, BIT7);

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ProgramDPCD12Proc()
//  [Description]
//					mhal_DPRx_ProgramDPCD12Proc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_ProgramDPCD12Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue)
{
	WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (ucDataQueue == NULL))
	{
		return;
	}

	msWriteByteMask(REG_DPRX_AUX_PM3_3A_L + usRegOffsetAuxPM3ByID, BIT0, BIT0);
	mhal_DPRx_DELAY_NOP(0x30);
	msWriteByteMask(REG_DPRX_AUX_PM3_3A_L + usRegOffsetAuxPM3ByID, 0, BIT0);

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ProgramDPCD13Proc()
//  [Description]
//					mhal_DPRx_ProgramDPCD13Proc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_ProgramDPCD13Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue)
{
	volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (ucDataQueue == NULL))
	{
		return;
	}

	msWriteByteMask(REG_DPRX_AUX_PM3_3A_L + usRegOffsetAuxPM3ByID, BIT1, BIT1);
	mhal_DPRx_DELAY_NOP(0x30);
	msWriteByteMask(REG_DPRX_AUX_PM3_3A_L + usRegOffsetAuxPM3ByID, 0, BIT1);

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ProgramDPCD14Proc()
//  [Description]
//					mhal_DPRx_ProgramDPCD14Proc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_ProgramDPCD14Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue)
{
	volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (ucDataQueue == NULL))
	{
		return;
	}

	msWriteByteMask(REG_DPRX_AUX_PM3_3A_L + usRegOffsetAuxPM3ByID, BIT2, BIT2);
	mhal_DPRx_DELAY_NOP(0x30);
	msWriteByteMask(REG_DPRX_AUX_PM3_3A_L + usRegOffsetAuxPM3ByID, 0, BIT2);

	return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_ProgramDPCD15Proc()
//  [Description]
//					mhal_DPRx_ProgramDPCD15Proc
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_ProgramDPCD15Proc(DPRx_AUX_ID dprx_aux_id, BYTE *ucDataQueue)
{
	volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);

	if((dprx_aux_id == DPRx_AUX_ID_MAX) || (ucDataQueue == NULL))
	{
		return;
	}

	msWriteByteMask(REG_DPRX_AUX_PM3_3A_L + usRegOffsetAuxPM3ByID, BIT3, BIT3);
	mhal_DPRx_DELAY_NOP(0x30);
	msWriteByteMask(REG_DPRX_AUX_PM3_3A_L + usRegOffsetAuxPM3ByID, 0, BIT3);

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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

	msWriteByteMask(REG_DPRX_AUX_PM2_57_L + usRegOffsetAuxPM2ByID, BIT3, BIT3); // Reset program DPCD 0 XDATA
    msWriteByteMask(REG_DPRX_AUX_PM0_7C_H + usRegOffsetAuxPM0ByID, BIT3, BIT3); // Clear XDATA program DPCD 0 IRQ

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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

    #if (DP_XDATA_PROGRAMMABLE_DPCD_MCCS == 1)
    BYTE ubXdataIdx = 0;
    BYTE MCCS_ON[DPRX_AUX_COMMAND_MAX_LENGTH] = {0x51, 0x84, 0x03, 0xD6, 0x00, 0x01, 0x6F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    BYTE MCCS_OFF[DPRX_AUX_COMMAND_MAX_LENGTH] = {0x51, 0x84, 0x03, 0xD6, 0x00, 0x05, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    BYTE MCCS_STANDBY[DPRX_AUX_COMMAND_MAX_LENGTH] = {0x51, 0x84, 0x03, 0xD6, 0x00, 0x04, 0x6A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    BYTE u8AUX_Cmd, u8AUX_Length;

    u8AUX_Cmd = msReadByte(REG_DPRX_AUX_PM0_1C_L + usRegOffsetAuxPM0ByID) & 0x0F;
    u8AUX_Length = msReadByte(REG_DPRX_AUX_PM0_1D_L + usRegOffsetAuxPM0ByID);
    //======================== <Read> PC Read command ========================
    if(u8AUX_Cmd & BIT0) // DP TX
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
                msWriteByteMask(REG_DPRX_AUX_PM0_75_L + usRegOffsetAuxPM0ByID, BIT0, BIT0); // Enable HW reply defer
			}
		}
    }
    #endif

	msWriteByteMask(REG_DPRX_AUX_PM2_57_L + usRegOffsetAuxPM2ByID, BIT7, BIT7); // Reset program DPCD 1 XDATA
    msWriteByteMask(REG_DPRX_AUX_PM0_4A_H + usRegOffsetAuxPM0ByID, BIT7, BIT7); // Clear XDATA program DPCD 1 IRQ

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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM0_48_L + usRegOffsetAuxPM0ByID) & BIT0)? TRUE : FALSE; // REG_PM_AUX_48[0]: programmable DPCD 0 IRQ

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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM0_48_L + usRegOffsetAuxPM0ByID) & BIT1)? TRUE : FALSE; // REG_PM_AUX_48[1]: programmable DPCD 1 IRQ

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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM0_48_L + usRegOffsetAuxPM0ByID) & BIT2)? TRUE : FALSE; // REG_PM_AUX_48[2]: programmable DPCD 2 IRQ

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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM0_48_L + usRegOffsetAuxPM0ByID) & BIT3)? TRUE : FALSE; // REG_PM_AUX_48[3]: programmable DPCD 3 IRQ

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
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM2_0E_L + usRegOffsetAuxPM2ByID) & BIT0)? TRUE : FALSE; // REG_PM_AUX_DPCD_0E[0]: programmable DPCD 4 IRQ

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
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM2_0E_L + usRegOffsetAuxPM2ByID) & BIT1)? TRUE : FALSE; // REG_PM_AUX_DPCD_0E[1]: programmable DPCD 5 IRQ

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
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM2_0E_L + usRegOffsetAuxPM2ByID) & BIT2)? TRUE : FALSE; // REG_PM_AUX_DPCD_0E[2]: programmable DPCD 6 IRQ

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
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM2_0E_L + usRegOffsetAuxPM2ByID) & BIT3)? TRUE : FALSE; // REG_PM_AUX_DPCD_0E[3]: programmable DPCD 7 IRQ

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
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM2_3A_L + usRegOffsetAuxPM2ByID) & BIT0)? TRUE : FALSE; // REG_PM_AUX_DPCD_3A[0]: programmable DPCD 8 IRQ

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
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM2_3A_L + usRegOffsetAuxPM2ByID) & BIT1)? TRUE : FALSE; // REG_PM_AUX_DPCD_3A[1]: programmable DPCD 9 IRQ

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
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM2_3A_L + usRegOffsetAuxPM2ByID) & BIT2)? TRUE : FALSE; // REG_PM_AUX_DPCD_3A[2]: programmable DPCD 10 IRQ

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
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM2_3A_L + usRegOffsetAuxPM2ByID) & BIT3)? TRUE : FALSE; // REG_PM_AUX_DPCD_3A[3]: programmable DPCD 11 IRQ

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetProgramDPCD12Flag()
//  [Description]
//					mhal_DPRx_GetProgramDPCD12Flag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetProgramDPCD12Flag(DPRx_AUX_ID dprx_aux_id)
{
	WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
	BOOL bEventFlag = 0;

    if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM3_3D_L + usRegOffsetAuxPM3ByID) & BIT0)? TRUE : FALSE;

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetProgramDPCD13Flag()
//  [Description]
//					mhal_DPRx_GetProgramDPCD13Flag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetProgramDPCD13Flag(DPRx_AUX_ID dprx_aux_id)
{
	WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
	BOOL bEventFlag = 0;

    if(dprx_aux_id == DPRx_AUX_ID_MAX)
    {
        return FALSE;
    }

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM3_3D_L + usRegOffsetAuxPM3ByID) & BIT1)? TRUE : FALSE;

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetProgramDPCD14Flag()
//  [Description]
//					mhal_DPRx_GetProgramDPCD14Flag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetProgramDPCD14Flag(DPRx_AUX_ID dprx_aux_id)
{
	WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
	BOOL bEventFlag = 0;

    if(dprx_aux_id == DPRx_AUX_ID_MAX)
    {
        return FALSE;
    }

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM3_3D_L + usRegOffsetAuxPM3ByID) & BIT2)? TRUE : FALSE;

    return bEventFlag;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetProgramDPCD15Flag()
//  [Description]
//					mhal_DPRx_GetProgramDPCD15Flag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetProgramDPCD15Flag(DPRx_AUX_ID dprx_aux_id)
{
	WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
	BOOL bEventFlag = 0;

    if(dprx_aux_id == DPRx_AUX_ID_MAX)
    {
        return FALSE;
    }

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM3_3D_L + usRegOffsetAuxPM3ByID) & BIT3)? TRUE : FALSE;

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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM0_7C_H + usRegOffsetAuxPM0ByID) & BIT0)? TRUE : FALSE; // REG_PM_AUX_7C[8]: XDATA programmable DPCD 0 IRQ

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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM0_4A_L + usRegOffsetAuxPM0ByID) & BIT3)? TRUE : FALSE; // REG_PM_AUX_49[8]: XDATA programmable DPCD 1 IRQ

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
	volatile WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    volatile WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);

    #if(DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST)
    msWriteByteMask(REG_DPRX_AUX_PM3_64_H + usRegOffsetAuxPM3ByID, BIT0, BIT0); // Burst mode
    #endif

    bEnable = bEnable; // For compile warning

    // Programmable DPCD 0
    #if (DPRX_PROGRAM_DPCD0_FUNC_SELECT == DPRX_POWERFUL1BYTE) // Powerful 1 byte
    msWriteByteMask(REG_DPRX_AUX_PM3_64_H + usRegOffsetAuxPM3ByID, BIT7, BIT7); // HW mode
    msWrite2Byte(REG_DPRX_AUX_PM3_66_L + usRegOffsetAuxPM3ByID, DPRX_PROGRAM_DPCD0_ADDERSS & 0xFFFF);
    msWriteByteMask(REG_DPRX_AUX_PM3_67_L + usRegOffsetAuxPM3ByID, DPRX_PROGRAM_DPCD0_ADDERSS >> 16, 0xF);
    msWriteByte(REG_DPRX_AUX_PM3_65_L + usRegOffsetAuxPM3ByID, 0xAA);    // Set value involved by SW
    msWrite2Byte(REG_DPRX_AUX_PM0_40_L + usRegOffsetAuxPM0ByID, 0xFFFF); // Use powerful mode should set programmable DPCD address of old mode to be an address that is not used
    msWriteByte(REG_DPRX_AUX_PM0_41_L + usRegOffsetAuxPM0ByID, 0xF);     // Use powerful mode should set programmable DPCD address of old mode to be an address that is not used
    #else
        UNUSED(usRegOffsetAuxPM0ByID);
    	#if(DPRX_PROGRAM_DPCD0_ENABLE == 0x1)
            mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_0, DPRX_PROGRAM_DPCD0_ADDERSS, FALSE);
            mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_0, bEnable);
    		#if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST) // Burst mode
                mhal_DPRx_SetProgDPCDDetectCmdType(dprx_aux_id, DPRX_PROGRAM_DPCD0_FUNC_SELECT, DPRx_PROGRAM_DPCD_0);
        	#endif
    	#endif
    #endif

    // Programmable DPCD 1
    #if(DPRX_PROGRAM_DPCD1_ENABLE == 0x1)
    mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_1, DPRX_PROGRAM_DPCD1_ADDERSS, FALSE);
    mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_1, bEnable);
	    #if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST) // Burst mode
        mhal_DPRx_SetProgDPCDDetectCmdType(dprx_aux_id, DPRX_PROGRAM_DPCD1_FUNC_SELECT, DPRx_PROGRAM_DPCD_1);
        #endif
    #endif

    // Programmable DPCD 2
    #if(DPRX_PROGRAM_DPCD2_ENABLE == 0x1)
    mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_2, DPRX_PROGRAM_DPCD2_ADDERSS, FALSE);
    mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_2, bEnable);
	    #if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST) // Burst mode
        mhal_DPRx_SetProgDPCDDetectCmdType(dprx_aux_id, DPRX_PROGRAM_DPCD2_FUNC_SELECT, DPRx_PROGRAM_DPCD_2);
        #endif
    #endif

    // Programmable DPCD 3
    #if(DPRX_PROGRAM_DPCD3_ENABLE == 0x1)
    mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_3, DPRX_PROGRAM_DPCD3_ADDERSS, FALSE);
    mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_3, bEnable);
	    #if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST) // Burst mode
        mhal_DPRx_SetProgDPCDDetectCmdType(dprx_aux_id, DPRX_PROGRAM_DPCD3_FUNC_SELECT, DPRx_PROGRAM_DPCD_3);
        #endif
    #endif

    // Programmable DPCD 4
    #if(DPRX_PROGRAM_DPCD4_ENABLE == 0x1)
    mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_4, DPRX_PROGRAM_DPCD4_ADDERSS, FALSE);
    mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_4, bEnable);
	    #if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST) // Burst mode
        mhal_DPRx_SetProgDPCDDetectCmdType(dprx_aux_id, DPRX_PROGRAM_DPCD4_FUNC_SELECT, DPRx_PROGRAM_DPCD_4);
        #endif
    #endif

    // Programmable DPCD 5
    #if(DPRX_PROGRAM_DPCD5_ENABLE == 0x1)
    mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_5, DPRX_PROGRAM_DPCD5_ADDERSS, FALSE);
    mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_5, bEnable);
	    #if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST) // Burst mode
        mhal_DPRx_SetProgDPCDDetectCmdType(dprx_aux_id, DPRX_PROGRAM_DPCD5_FUNC_SELECT, DPRx_PROGRAM_DPCD_5);
        #endif
    #endif

    // Programmable DPCD 6
    #if(DPRX_PROGRAM_DPCD6_ENABLE == 0x1)
    mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_6, DPRX_PROGRAM_DPCD6_ADDERSS, FALSE);
    mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_6, bEnable);
	    #if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST) // Burst mode
        mhal_DPRx_SetProgDPCDDetectCmdType(dprx_aux_id, DPRX_PROGRAM_DPCD6_FUNC_SELECT, DPRx_PROGRAM_DPCD_6);
        #endif
    #endif

    // Programmable DPCD 7
    #if(DPRX_PROGRAM_DPCD7_ENABLE == 0x1)
        mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_7, DPRX_PROGRAM_DPCD7_ADDERSS, FALSE);
        mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_7, bEnable);
	    #if (DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_BURST) //Burst mode
        mhal_DPRx_SetProgDPCDDetectCmdType(dprx_aux_id, DPRX_PROGRAM_DPCD7_FUNC_SELECT, DPRx_PROGRAM_DPCD_7);
	    #endif
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

    #if(DPRX_PROGRAM_DPCD12_ENABLE == 0x1)
    mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_12, DPRX_PROGRAM_DPCD12_ADDERSS, FALSE);
    mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_12, bEnable);
    #endif

    #if(DPRX_PROGRAM_DPCD13_ENABLE == 0x1)
    mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_13, DPRX_PROGRAM_DPCD13_ADDERSS, FALSE);
    mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_13, bEnable);
    #endif

    #if(DPRX_PROGRAM_DPCD14_ENABLE == 0x1)
    mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_14, DPRX_PROGRAM_DPCD14_ADDERSS, FALSE);
    mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_14, bEnable);
    #endif

    #if(DPRX_PROGRAM_DPCD15_ENABLE == 0x1)
    mhal_DPRx_SetProgramDPCD(bEnable, dprx_aux_id, DPRx_PROGRAM_DPCD_15, DPRX_PROGRAM_DPCD15_ADDERSS, FALSE);
    mhal_DPRx_SetProgramDPCDInterrupt(dprx_aux_id, DPRx_PROGRAM_DPCD_15, bEnable);
    #endif

	return TRUE;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SetXDATAProgrammableDPCDEnable()
//  [Description]
//					mhal_DPRx_SetXDATAProgrammableDPCDEnable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_SetXDATAProgrammableDPCDEnable(DPRx_AUX_ID dprx_aux_id, DPRx_XDATAPROGRAM_DPCD_TYPE dprx_xdata_hw_id, BOOL bHwEnable, BOOL bIsrEnable)
{
	DWORD ulTargetAddress = 0xFFFFFF; // Initial as invalid address
	DPRx_Command_TYPE rxCommandType = DPRx_NATIVE_AUX;
	BOOL bAddrGroup = FALSE; // Block mode or not

	switch(dprx_xdata_hw_id)
	{
		case DPRx_XDATAPROGRAM_DPCD_0:
			#if(DPRX_XDATA_PROGRAM_DPCD0_ENABLE == 0x1)
			ulTargetAddress = DPRX_XDATA_PROGRAM_DPCD0_ADDERSS;
		    #endif
			break;

		case DPRx_XDATAPROGRAM_DPCD_1:
			#if(DPRX_XDATA_PROGRAM_DPCD1_ENABLE == 0x1)
			ulTargetAddress = DPRX_XDATA_PROGRAM_DPCD1_ADDERSS;
			rxCommandType = DPRx_I2C;
			#endif
			break;

		case DPRx_XDATAPROGRAM_DPCD_MAX:
			break;
	};

	if(ulTargetAddress == 0xFFFFFF)
	{
		return FALSE;
	}

	mhal_DPRx_SetXDATAProgramDPCD(bHwEnable, dprx_aux_id, dprx_xdata_hw_id, ulTargetAddress, rxCommandType, bAddrGroup);
    mhal_DPRx_SetXDATAProgramDPCDInterrupt(dprx_aux_id, dprx_xdata_hw_id, bIsrEnable);

    return TRUE;
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
DPRx_MCCS_WAKEUP mhal_DPRx_CheckMCCSWakeUpXDATAProgrammableDPCD(DPRx_AUX_ID dprx_aux_id) // Check to see if xdata programmable dpcd received mccs command
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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bEventFlag = (msReadByte(REG_DPRX_AUX_PM0_3C_L + usRegOffsetAuxPM0ByID) & BIT7)? TRUE : FALSE;

    if(bEventFlag)
    {
        mhal_DPRx_MCCSReceiveProc(dprx_aux_id, ucInputPort);

        // Clear IRQ, then trigger HW send ACK and clear DDC buffer
        msWriteByteMask(REG_DPRX_AUX_PM0_3C_H + usRegOffsetAuxPM0ByID, BIT7, BIT7);
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
#if(DPRX_PROGRAMMABLE_DPCD_MODE == DPRX_PROG_DPCD_MODE_OLD)
void mhal_DPRx_MCCSReceiveProc(DPRx_AUX_ID dprx_aux_id, BYTE ucInputPort)
{
    BYTE u8AUXCH_RxCmd;
    BYTE u8DDC_Length;
    BYTE AuxData;
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    u8AUXCH_RxCmd = msRegs(REG_DPRX_AUX_PM0_1C_L + usRegOffsetAuxPM0ByID) & 0x0F;
    u8DDC_Length = msRegs(REG_DPRX_AUX_PM0_1D_L + usRegOffsetAuxPM0ByID);

    //======================== <Read> PC read command ========================
    if(u8AUXCH_RxCmd & BIT0) // DP TX
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
							msRegs(REG_DPRX_AUX_PM0_79_H + usRegOffsetAuxPM0ByID) = DDCBuffer[g_ucDDC_TxRecord-1];
						}

					    txLength--;
					}
					else
					{
					    msRegs(REG_DPRX_AUX_PM0_79_H + usRegOffsetAuxPM0ByID) = DDC2B_DEST_ADDRESS;
					}

					g_ucDDC_TxRecord++;

					msRegs(REG_DPRX_AUX_PM0_79_L + usRegOffsetAuxPM0ByID) = msRegs(REG_DPRX_AUX_PM0_79_L + usRegOffsetAuxPM0ByID) | BIT0;

					while(!(((msRegs(REG_DPRX_AUX_PM0_79_L + usRegOffsetAuxPM0ByID)) & BIT2))) // Wait
					{
					};

					msRegs(REG_DPRX_AUX_PM0_79_L + usRegOffsetAuxPM0ByID) = msRegs(REG_DPRX_AUX_PM0_79_L + usRegOffsetAuxPM0ByID) | BIT1;
				}
            }
            else
            {
                while(u8DDC_Length--)
                {
					msRegs(REG_DPRX_AUX_PM0_79_H + usRegOffsetAuxPM0ByID) = dpNullMsg[g_ucDDC_TxRecord++];
					msRegs(REG_DPRX_AUX_PM0_79_L + usRegOffsetAuxPM0ByID) = msRegs(REG_DPRX_AUX_PM0_79_L + usRegOffsetAuxPM0ByID) | BIT0;

					while(!(((msRegs(REG_DPRX_AUX_PM0_79_L + usRegOffsetAuxPM0ByID)) & BIT2)))	// Wait
					{
					};

					msRegs(REG_DPRX_AUX_PM0_79_L + usRegOffsetAuxPM0ByID) = msRegs(REG_DPRX_AUX_PM0_79_L + usRegOffsetAuxPM0ByID) | BIT1;
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

			    msRegs(REG_DPRX_AUX_PM0_78_L + usRegOffsetAuxPM0ByID) = msRegs(REG_DPRX_AUX_PM0_78_L + usRegOffsetAuxPM0ByID) | BIT0;

			    while(!(msRegs(REG_DPRX_AUX_PM0_78_L + usRegOffsetAuxPM0ByID) & BIT2))	 // Wait
			    {};

			    msRegs(REG_DPRX_AUX_PM0_78_L + usRegOffsetAuxPM0ByID) = msRegs(REG_DPRX_AUX_PM0_78_L + usRegOffsetAuxPM0ByID) | BIT1;

			    AuxData = msRegs(REG_DPRX_AUX_PM0_3A_H + usRegOffsetAuxPM0ByID);
			    u8DDC_Length--;
			}

			while(u8DDC_Length--)
			{
			    msRegs(REG_DPRX_AUX_PM0_78_L + usRegOffsetAuxPM0ByID) = msRegs(REG_DPRX_AUX_PM0_78_L + usRegOffsetAuxPM0ByID) | BIT0;

			    while(!(msRegs(REG_DPRX_AUX_PM0_78_L + usRegOffsetAuxPM0ByID) & BIT2)) // Wait
			    {};

			    msRegs(REG_DPRX_AUX_PM0_78_L + usRegOffsetAuxPM0ByID) = msRegs(REG_DPRX_AUX_PM0_78_L + usRegOffsetAuxPM0ByID) | BIT1;

				if(rxIndex < DDC_BUFFER_LENGTH)
				{
					DDCBuffer[rxIndex++] = msRegs(REG_DPRX_AUX_PM0_3A_H + usRegOffsetAuxPM0ByID);
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

    msRegs(REG_DPRX_AUX_PM0_1F_L + usRegOffsetAuxPM0ByID) = 0;
    msRegs(REG_DPRX_AUX_PM0_3B_H + usRegOffsetAuxPM0ByID) = msRegs(REG_DPRX_AUX_PM0_3B_H + usRegOffsetAuxPM0ByID) | BIT0; // Trigger

	return;
}
#else
void mhal_DPRx_MCCSReceiveProc(DPRx_AUX_ID dprx_aux_id, BYTE ucInputPort)
{
	WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    WORD usRegOffsetAuxPM3ByID = DP_REG_OFFSET_AUX_PM3(dprx_aux_id);
	BYTE u8AUXCH_RxCmd;
    BYTE u8DDC_Length;
    BYTE ucAuxData;
    BYTE uctemp = 0;
    BYTE ucDDCBuf[16] = {0};
    BYTE ucDataIndex = 0;
    BYTE ucdpNullMsg = 0;
	WORD usCount = DPRX_PROG_DPCD_BURSTMODE_TIMEOUT;

    u8AUXCH_RxCmd = msRegs(REG_DPRX_AUX_PM0_1C_L + usRegOffsetAuxPM0ByID) & 0x0F;
    u8DDC_Length = msRegs(REG_DPRX_AUX_PM0_1D_L + usRegOffsetAuxPM0ByID);

    //======================== <Read> PC read command ========================
    if(u8AUXCH_RxCmd & BIT0) // DP TX
    {
		if(u8DDC_Length != 0x0)
		{
            if((rxStatus == DDC2B_CLEAR) && txLength)
            {
				while((u8DDC_Length--) && (txLength))
				{
					if(g_ucDDC_TxRecord)
					{
						if((g_ucDDC_TxRecord-1) < DDC_BUFFER_LENGTH)
						{
                    		if(ucDataIndex == 0)
                    		{
                    			msWriteByteMask(REG_DPRX_AUX_PM3_53_L + usRegOffsetAuxPM3ByID, ((DDCBuffer[g_ucDDC_TxRecord-1]) << 4), 0xF0); // REG_DPRX_AUX_PM3_53[11:4]
                    			msWriteByteMask(REG_DPRX_AUX_PM3_53_H + usRegOffsetAuxPM3ByID, ((DDCBuffer[g_ucDDC_TxRecord-1])  >> 4), 0xF);
                    		}
                    		else
                    		{
                    			msWriteByte(REG_DPRX_AUX_PM3_54_L + (ucDataIndex - 1) + usRegOffsetAuxPM3ByID, DDCBuffer[g_ucDDC_TxRecord-1]);
                    		}

                    		ucDataIndex++;
						}

					    txLength--;
					}
					else
					{
					    if(ucDataIndex == 0)
                		{
                			msWriteByteMask(REG_DPRX_AUX_PM3_53_L + usRegOffsetAuxPM3ByID, (BYTE)(DDC2B_DEST_ADDRESS << 4), 0xF0); // REG_DPRX_AUX_PM3_53[11:4]
                			msWriteByteMask(REG_DPRX_AUX_PM3_53_H + usRegOffsetAuxPM3ByID, (BYTE)(DDC2B_DEST_ADDRESS >> 4), 0xF);
                		}
                		else
                		{
                			msWriteByte(REG_DPRX_AUX_PM3_54_L + (ucDataIndex - 1) + usRegOffsetAuxPM3ByID, DDC2B_DEST_ADDRESS);
                		}

                		ucDataIndex++;
					}

					g_ucDDC_TxRecord++;
				}

				msWriteByteMask(REG_DPRX_AUX_PM3_64_H + usRegOffsetAuxPM3ByID, BIT2, BIT2); // Tx burst FIFO enable

            	while((!(msReadByte(REG_DPRX_AUX_PM3_64_H + usRegOffsetAuxPM3ByID) & BIT4)) && (usCount > 0)) // Tx FIFO burst done
            	{
            		usCount--;
            	}

            	msWriteByteMask(REG_DPRX_AUX_PM3_64_H + usRegOffsetAuxPM3ByID, BIT6, BIT6); // Tx FIFO burst done clear
            }
            else
            {
                while(u8DDC_Length--)
                {
					ucdpNullMsg = dpNullMsg[g_ucDDC_TxRecord++];

					if(ucDataIndex == 0)
            		{
            			msWriteByteMask(REG_DPRX_AUX_PM3_53_L + usRegOffsetAuxPM3ByID, ucdpNullMsg << 4, 0xF0); // REG_DPRX_AUX_PM3_53[11:4]
            			msWriteByteMask(REG_DPRX_AUX_PM3_53_H + usRegOffsetAuxPM3ByID, ucdpNullMsg >> 4, 0xF);
            		}
            		else
            		{
            			msWriteByte(REG_DPRX_AUX_PM3_54_L + (ucDataIndex - 1) + usRegOffsetAuxPM3ByID, ucdpNullMsg);
            		}

            		ucDataIndex++;
                }

                msWriteByteMask(REG_DPRX_AUX_PM3_64_H + usRegOffsetAuxPM3ByID, BIT2, BIT2); // Tx burst FIFO enable

            	while((!(msReadByte(REG_DPRX_AUX_PM3_64_H + usRegOffsetAuxPM3ByID) & BIT4)) && (usCount > 0)) // Tx FIFO burst done
            	{
            		usCount--;
            	}

            	msWriteByteMask(REG_DPRX_AUX_PM3_64_H + usRegOffsetAuxPM3ByID, BIT6, BIT6); // Tx FIFO burst done clear
            }
        }
    }
    //======================== <Write> PC write command ========================
    else if((u8AUXCH_RxCmd == 0x00) || (u8AUXCH_RxCmd == 0x04)) // DP RX
    {
		if (u8DDC_Length)
        {
            msWriteByteMask(REG_DPRX_AUX_PM3_64_H + usRegOffsetAuxPM3ByID, BIT1, BIT1);  //Rx burst FIFO enable

            // Must fine tune
            while((!(msReadByte(REG_DPRX_AUX_PM3_64_H + usRegOffsetAuxPM3ByID) & BIT3)) && (usCount > 0)) // Rx FIFO burst done
            {
                usCount--;
            };

            msWriteByteMask(REG_DPRX_AUX_PM3_64_H + usRegOffsetAuxPM3ByID, BIT5, BIT5); //Rx FIFO burst done clear

            for(uctemp = 0; uctemp < u8DDC_Length; uctemp++)
            {
        		ucDDCBuf[uctemp] = msReadByte(REG_DPRX_AUX_PM3_5C_H + uctemp + usRegOffsetAuxPM3ByID); // MCU read RX FIFO data
        	}

			if(u8DDC_RxRecord == 0)
			{
			    u8DDC_RxRecord = TRUE;
			    rxIndex = 0;

                ucAuxData = ucDDCBuf[ucDataIndex];
                ucDataIndex++;
			    u8DDC_Length--;
			}

			while(u8DDC_Length--)
			{
            	if(rxIndex < DDC_BUFFER_LENGTH)
				{
					DDCBuffer[rxIndex++] = ucDDCBuf[ucDataIndex];
                    ucDataIndex++;
				}
			}
        }

        if(u8DDC_RxRecord)//To avoid TX send I2C write cmd without length
        {
            if((u8AUXCH_RxCmd == 0x00) ||
              ((u8AUXCH_RxCmd == 0x04) && (rxIndex > ((DDCBuffer[0] &~ DDC2Bi_CONTROL_STATUS_FLAG) + 1)))) // Mot = 1, but the number of received data bytes > length
            {
                u8DDC_RxRecord = FALSE;
                g_ucDDC_TxRecord = 0;
                rxStatus = DDC2B_COMPLETED;
                txLength = 0;

                // Note that rxInputPort need to set as system scan port order instead of combo port
                rxInputPort = ucInputPort;
            }
        }
    }

    msWriteByteMask(REG_DPRX_AUX_PM0_1F_L + usRegOffsetAuxPM0ByID, 0x0, BIT1|BIT0); // reg_aux_tx_cmd // 0: ACK, 1: NACK, 2: DEFER

    msWriteByteMask(REG_DPRX_AUX_PM0_3B_H + usRegOffsetAuxPM0ByID, BIT0, BIT0); // Set MCU reply "Ready"
    msWriteByteMask(REG_DPRX_AUX_PM0_18_L + usRegOffsetAuxPM0ByID, 0, BIT1);    // AUX reply by HW

	return;
}
#endif

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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

    if(bEnable == TRUE)
    {
		msWriteByteMask(REG_DPRX_AUX_PM0_1D_H + usRegOffsetAuxPM0ByID, AUX_MCCS_SLAVE_ADDR, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6);
    }
	else
	{
		msWriteByteMask(REG_DPRX_AUX_PM0_1D_H + usRegOffsetAuxPM0ByID, (~AUX_MCCS_SLAVE_ADDR), BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6);
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
	WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET_TRANS_CTRL(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_TRANS_CTRL_06_L + usRegOffsetTransCTRLByID, bEnable? 0 : BIT0, BIT0);

    return;
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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

	msWriteByteMask(REG_DPRX_AUX_PM0_4E_L + usRegOffsetAuxPM0ByID, (ucSwing << 6), BIT7|BIT6); // reg_max_swing_level
	msWriteByteMask(REG_DPRX_AUX_PM0_4E_L + usRegOffsetAuxPM0ByID, (ucPreEmphasis << 4), BIT5|BIT4); // reg_max_pre_level

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
    WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET_TRANS_CTRL(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

    msWriteByteMask(REG_DPRX_TRANS_CTRL_07_L + usRegOffsetTransCTRLByID, ubTime & 0x0F, (BIT0|BIT1|BIT2|BIT3)); // Fast training HW delay time (sec)

    return;
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_HWFastTrainingBusy_Get()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_HWFastTrainingBusy_Get(DPRx_ID dprx_id)
{
	WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET_TRANS_CTRL(dprx_id);

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
	WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET_TRANS_CTRL(dprx_id);

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
    WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET_TRANS_CTRL(dprx_id);

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

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_GetLane0SQH()
//  [Description]
//					mhal_DPRx_GetLane0SQH
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_GetDPCLane0SQH(BOOL bCustomLane0)
{
    #if (ENABLE_USB_TYPEC == 0x1) && (SET_DPC_PORT_AS_DP_PORT == 0x0)
    if((glSYS_TypeC_PinAssign[glSYS_TypeC_PortIndex] == BIT2) || (glSYS_TypeC_PinAssign[glSYS_TypeC_PortIndex] == BIT3)) // PinC/D
    {
        if(glSYS_CC_Pin[glSYS_TypeC_PortIndex] == 0x1) // Flip plug
        {
            if(bCustomLane0)
            {
                return 0; // TBD for customize lane0
            }
            else
            {
                return (msReadByte(REG_1526EF) & BIT6);
            }
        }
        else if(glSYS_CC_Pin[glSYS_TypeC_PortIndex] == 0x0)
        {
            if(bCustomLane0)
            {
                return 0; // TBD for customize lane0
            }
            else
            {
                return (msReadByte(REG_1526EF) & BIT7);
            }
        }
    }
    else if(glSYS_TypeC_PinAssign[glSYS_TypeC_PortIndex] == BIT4) // PinE
    {
        if(glSYS_CC_Pin[glSYS_TypeC_PortIndex] == 0x1) // Flip plug
        {
            if(bCustomLane0)
            {
                return 0; // TBD for customize lane0
            }
            else
            {
                return (msReadByte(REG_1526EF) & BIT5);
            }
        }
        else if(glSYS_CC_Pin[glSYS_TypeC_PortIndex] == 0x0)
        {
            if(bCustomLane0)
            {
                return 0; // TBD for customize lane0
            }
            else
            {
                return (msReadByte(REG_1526EF) & BIT4);
            }
        }
    }
    #endif

    // For fsDP
    if(bCustomLane0)
    {
        return 0; // TBD for customize lane0
    }
    else
    {
        return (msReadByte(REG_1526EF) & BIT7);
    }
}

//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_SQHOverwrite()
//  [Description]
//					mhal_DPRx_SQHOverwrite
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_SQHOverwrite(DPRx_ID dprx_id, BOOL bDebounce)
{
    BOOL bCustomLane0 = FALSE; // Customize lane 0

    if (dprx_id == DPRx_ID_2)
    {
        if(mhal_DPRx_GetDPCLane0SQH(bCustomLane0)) // MT9701 dpc sqh tie0 use u3rt sqh
        {
            if(glDP_SQDebounce[dprx_id] == 0 || bDebounce == FALSE)
            {
                msWriteByte(REG_DPRX_PHY_PM_7C_L, 0x0f);
                return;
            }
        }
        else
        {
            glDP_SQDebounce[dprx_id] = 50; // Unit: 1ms
            msWriteByte(REG_DPRX_PHY_PM_7C_L, 0x00);
            return;
        }
    }
    else
    {
        glDP_SQDebounce[dprx_id] = 50; // Unit: 1ms
        msWriteByte(REG_DPRX_PHY_PM_7C_L, 0x00);
        return;
    }

    return;
}

void ________HDCP________(void);
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
	WORD usRegOffsetHDCP22_2_ByID = DP_REG_OFFSET_DPCD_HDCP22_2(dprx_id);

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
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);
    WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET_DPCD_HDCP22_4(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	if(bEnable)
    {
        // Avoid toggle ENC result in picture error
        msWriteByteMask(REG_DPRX_RECEIVER_70_H + usRegOffsetReceiverByID, 0, BIT5); // reg_discard_unused_cipher

        // Set link_integrity_failure, reauth HW mode
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
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET_DPCD1(dprx_id);

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
	WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET_DPCD_HDCP22_4(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	if(bEnable == TRUE)
    {
        // BK_160F, 0x71[1]: ake_init_irq_mask
        // BK_160F, 0x71[2]: ake_no_stored_km_mask
        // BK_160F, 0x71[3]: ake_stored_km_mask
        // BK_160F, 0x71[4]: locality_check_mask
        // BK_160F, 0x71[5]: ske_irq_mask
        msWriteByteMask(REG_DPRX_HDCP22_4_71_L + usRegOffsetHDCP22_4_ByID, 0, BIT1|BIT2|BIT3|BIT4|BIT5);

        // BK_160F, 0x75[0]: reg_r_tx_irq_mask
        // BK_160F, 0x75[1]: reg_tx_caps_irq_mask
        // BK_160F, 0x75[2]: reg_ekh_km_irq_mask
        // BK_160F, 0x75[3]: reg_m_irq_mask
        // BK_160F, 0x75[4]: reg_edkey_ks_irq_mask
        // BK_160F, 0x75[5]: reg_r_iv_irq_mask
        msWriteByteMask(REG_DPRX_HDCP22_4_75_L + usRegOffsetHDCP22_4_ByID, 0, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5);

		msWriteByteMask(REG_DPRX_HDCP22_4_78_H + usRegOffsetHDCP22_4_ByID, 0, BIT1); // BK_160F, 0x78[9]: h_pulum_mask
        msWriteByteMask(REG_DPRX_HDCP22_4_79_L + usRegOffsetHDCP22_4_ByID, 0, BIT5); // BK_160F, 0x79[5]: v mask
        msWriteByteMask(REG_DPRX_HDCP22_4_79_L + usRegOffsetHDCP22_4_ByID, 0, BIT6); // BK_160F, 0x79[6]: k mask
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
	WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET_DPCD_HDCP22_4(dprx_id);
    WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET_DPCD1(dprx_id);

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
    WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET_DPCD1(dprx_id);

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
    WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET_DPCD_HDCP22_4(dprx_id);
    WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);

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
    WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET_DPCD_HDCP22_4(dprx_id);

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
    WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET_DPCD_HDCP22_4(dprx_id);

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
    WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET_DPCD_HDCP22_4(dprx_id);
	DWORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_id);
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
		while ((((msReadByte(REG_DPRX_AUX_PM0_74_L + usRegOffsetAuxPM0ByID) & 0x38) >> 3) >= 2) && (usTimeout > 0)) //check hw AUX status free
		{
			mhal_DPRx_DELAY_NOP(25);
			usTimeout--;
		};

		msWriteByteMask(REG_DPRX_AUX_PM0_75_L + usRegOffsetAuxPM0ByID, BIT0, BIT0);  //Enable hw reply defer
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
    WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET_DPCD_HDCP22_4(dprx_id);

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
    WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET_DPCD_HDCP22_4(dprx_id);

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
    WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET_DPCD_HDCP22_4(dprx_id);

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
    WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET_DPCD_HDCP22_4(dprx_id);

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
    WORD usRegOffsetHDCP22_2_ByID = DP_REG_OFFSET_DPCD_HDCP22_2(dprx_id);
    WORD usRegOffsetHDCP22_3_ByID = DP_REG_OFFSET_DPCD_HDCP22_3(dprx_id);
    WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET_DPCD_HDCP22_4(dprx_id);

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
	WORD usRegOffsetHDCP22_3_ByID = DP_REG_OFFSET_DPCD_HDCP22_3(dprx_id);

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
	WORD usRegOffsetHDCP22_2_ByID = DP_REG_OFFSET_DPCD_HDCP22_2(dprx_id);

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
	WORD usRegOffsetHDCP22_2_ByID = DP_REG_OFFSET_DPCD_HDCP22_2(dprx_id);

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
	WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET_DPCD_HDCP22_4(dprx_id);

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
    WORD usRegOffsetHDCP22_0_ByID = DP_REG_OFFSET_DPCD_HDCP22_0(dprx_id);
    WORD usRegOffsetHDCP22_1_ByID = DP_REG_OFFSET_DPCD_HDCP22_1(dprx_id);
    WORD usRegOffsetHDCP22_2_ByID = DP_REG_OFFSET_DPCD_HDCP22_2(dprx_id);
    WORD usRegOffsetHDCP22_3_ByID = DP_REG_OFFSET_DPCD_HDCP22_3(dprx_id);
    WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET_DPCD_HDCP22_4(dprx_id);
	DWORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_id);
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
            while ((((msReadByte(REG_DPRX_AUX_PM0_74_L + usRegOffsetAuxPM0ByID) & 0x38) >> 3) >= 2) && (usTimeout > 0)) //check hw AUX status free
            {
                mhal_DPRx_DELAY_NOP(25);
                usTimeout--;
            };
            msWriteByteMask(REG_DPRX_AUX_PM0_75_L + usRegOffsetAuxPM0ByID, 0, BIT0);  //Disable hw reply defer

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
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    for(i = 0; i < 20; i++)
    {
        msWriteByte(REG_DPRX_AUX_PM2_10_L + usRegOffsetAuxPM2ByID + i , *(VData + i));
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
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    msWriteByte(REG_DPRX_AUX_PM2_1A_L + usRegOffsetAuxPM2ByID, Binfo_Devs);
    msWriteByte(REG_DPRX_AUX_PM2_1A_H + usRegOffsetAuxPM2ByID, Binfo_Depth);

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
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET_DPCD0(dprx_id);

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
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET_DPCD0(dprx_id);

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
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET_DPCD1(dprx_id);

	if((dprx_id == DPRx_ID_MAX) || (dprx_aux_id == DPRx_AUX_ID_MAX))
	{
		return;
	}

    msWriteByteMask(REG_DPRX_DPCD1_48_L + usRegOffsetDPCD1ByID, bEnable? BIT2 : 0, BIT2);
    msWriteByteMask(REG_DPRX_AUX_PM2_1E_H + usRegOffsetAuxPM2ByID, bEnable? BIT0 : 0, BIT0);

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
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);

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
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET_DPCD1(dprx_id);

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
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET_DPCD1(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

	// AKSV mask
    if(bEnable == TRUE)
    {
        msWriteByte(REG_DPRX_DPCD1_31_L + usRegOffsetDPCD1ByID, BIT7); // AKSV received IRQ clear
        mhal_DPRx_DELAY_NOP(0x30);
        msWriteByte(REG_DPRX_DPCD1_31_L + usRegOffsetDPCD1ByID, 0);    // AKSV received IRQ enable

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
    WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET_DPCD1(dprx_id);
    WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);

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
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);

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
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);

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
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET_DPCD0(dprx_id);

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
	WORD usRegOffsetHDCP22_4_ByID = DP_REG_OFFSET_DPCD_HDCP22_4(dprx_id);

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
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return FALSE;
	}

	return ((msReadByte(REG_DPRX_RECEIVER_73_H + usRegOffsetReceiverByID) & (BIT0|BIT1|BIT2)) == BIT2)? TRUE : FALSE;
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
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);

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
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    msWriteByteMask(REG_DPRX_AUX_PM2_5A_L + usRegOffsetAuxPM2ByID, BIT2, BIT2);

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
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);

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
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);

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
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);

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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

	if(msReadByte(REG_DPRX_AUX_PM0_49_H + usRegOffsetAuxPM0ByID) & BIT4)
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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return;
	}

    msWriteByteMask(REG_DPRX_AUX_PM0_49_H + usRegOffsetAuxPM0ByID, BIT7, BIT7);

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
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);
    BOOL bRet = FALSE;

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bRet = (msReadByte(REG_DPRX_AUX_PM2_2D_L + usRegOffsetAuxPM2ByID) & BIT0)? TRUE : FALSE; // reg_hdcp_aksv_irq

    if(bRet == TRUE)
    {
        msWriteByteMask(REG_DPRX_AUX_PM2_2D_L + usRegOffsetAuxPM2ByID, BIT1, BIT1); // reg_hdcp_aksv_irq_clr
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
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);
    BOOL bRet = FALSE;

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

    bRet = (msReadByte(REG_DPRX_AUX_PM2_2D_L + usRegOffsetAuxPM2ByID) & BIT4)? TRUE : FALSE; // reg_hdcp_an_irq

    if(bRet == TRUE)
    {
        msWriteByteMask(REG_DPRX_AUX_PM2_2D_L + usRegOffsetAuxPM2ByID, BIT5, BIT5); // reg_hdcp_an_irq_clr
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
	if(dprx_id == DPRx_ID_2)
	{
		if((ubPinAssign == BIT2)||(ubPinAssign == BIT3)) //Pin Assign C, D case
		{
			if(ubCCpinInfo == 0x1) //Flip plug
			{
				return hwSDM_AuxP_TYPEC3Pin_Pin;
			}
			else if(ubCCpinInfo == 0x0)
			{
				return hwSDM_AuxN_TYPEC3Pin_Pin;
			}
		}
		else if(ubPinAssign == BIT4) // Pin Assign E case
		{
			if(ubCCpinInfo == 0x1) //Flip plug
			{
				return hwSDM_AuxN_TYPEC3Pin_Pin;
			}
			else if(ubCCpinInfo == 0x0)
			{
				return hwSDM_AuxP_TYPEC3Pin_Pin;
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
//                  mhal_DPRx_SetAuxPNSwapEnable()
//  [Description]
//					mhal_DPRx_SetAuxPNSwapEnable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_SetAuxPNSwapEnable(DPRx_AUX_ID dprx_aux_id, BOOL bEnable)
{
	WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

    if(bEnable == TRUE)
	{
	    printf("[DP Aux %d] Set Aux PN Swap !\r\n", dprx_aux_id);

		msWriteByteMask(REG_DPRX_AUX_PM0_18_L + usRegOffsetAuxPM0ByID, BIT0, BIT0);
		msWriteByteMask(REG_DPRX_AUX_PM0_18_H + usRegOffsetAuxPM0ByID, BIT7, BIT7);
	}
	else
	{
		msWriteByteMask(REG_DPRX_AUX_PM0_18_L + usRegOffsetAuxPM0ByID, 0, BIT0);
		msWriteByteMask(REG_DPRX_AUX_PM0_18_H + usRegOffsetAuxPM0ByID, 0, BIT7);
	}

	return TRUE;
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
//                  mhal_DPRx_LanePNSwapEnable_Set()
//  [Description]
//					mhal_DPRx_LanePNSwapEnable_Set
//  [Arguments]:
//					ubTargetLaneNumber: [1:0] for Lane0,
//										[3:2] for Lane1,
//										[5:4] for Lane2,
//										[7:6] for Lane3
//  [Return]:
//
//**************************************************************************
BOOL mhal_DPRx_LanePNSwapEnable_Set(DPRx_ID dprx_id, BYTE ubLanePNSwapSelect)
{
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return FALSE;
	}

    msWriteByteMask(REG_DPRX_RECEIVER_06_H + usRegOffsetReceiverByID, (ubLanePNSwapSelect & 0x0F), 0xF);

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
    UNUSED(dprx_id);
    UNUSED(ubTypeC_PinAssign);
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
    UNUSED(dprx_id);
    UNUSED(ubTypeC_PinAssign);
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
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);

	if(dprx_aux_id == DPRx_AUX_ID_MAX)
	{
		return FALSE;
	}

	if(msReadByte(REG_DPRX_AUX_PM0_3C_L + usRegOffsetAuxPM0ByID) & BIT6)
	{
		msWriteByte(REG_DPRX_AUX_PM0_3C_H + usRegOffsetAuxPM0ByID, BIT6);

	    BStatus = TRUE;
	}

    return BStatus;
}
//**************************************************************************
//  [Function Name]:
//                  mhal_DPRx_EnableDPCD600Interrupt()
//  [Description]
//					mhal_DPRx_EnableDPCD600Interrupt
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mhal_DPRx_EnableDPCD600Interrupt(DPRx_ID dprx_id, BOOL bEnable)
{
	WORD usRegOffsetDPCD1ByID = DP_REG_OFFSET_DPCD1(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return;
	}

    msWriteByteMask(REG_DPRX_DPCD1_31_L + usRegOffsetDPCD1ByID, BIT0, BIT0);
    mhal_DPRx_DELAY_NOP(10);
    msWriteByteMask(REG_DPRX_DPCD1_31_L + usRegOffsetDPCD1ByID, 0, BIT0);

    msWriteByteMask(REG_DPRX_DPCD1_2E_L + usRegOffsetDPCD1ByID, bEnable? 0 : BIT0, BIT0); // [0] dpcd_power_ctrl == 2

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
	WORD usRegOffsetReceiverByID = DP_REG_OFFSET_RECEIVER(dprx_id);

    if(bEnable)
    {
        msWriteByteMask(REG_DPRX_RECEIVER_01_L + usRegOffsetReceiverByID, BIT6, BIT6); // symbol_lock_eq_done
        msWriteByteMask(REG_DPRX_RECEIVER_01_H + usRegOffsetReceiverByID, BIT1, BIT1); // ctrl_symbol_detect
    }
    else
    {
        msWriteByteMask(REG_DPRX_RECEIVER_01_L + usRegOffsetReceiverByID, 0, BIT6);
        msWriteByteMask(REG_DPRX_RECEIVER_01_H + usRegOffsetReceiverByID, 0, BIT1);
    }

	return;
}
#endif // ENABLE_DP_INPUT

BOOL ubHaveBeenSleeped[DPRx_AUX_ID_MAX] = {0};

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
	WORD usRegOffsetDPCD0ByID = DP_REG_OFFSET_DPCD0(dprx_id);

	if(dprx_id == DPRx_ID_MAX)
	{
		return 0;
	}

	return (27 * msReadByte(REG_DPRX_DPCD0_20_L + usRegOffsetDPCD0ByID));
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
    DWORD usRegOffsetDecoderByID = DP_REG_OFFSET_DECODER(dprx_decoder_id);
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
    	ulBaseMValue = ((DWORD)msReadByte(REG_DPRX_DECODER_E0_31_L + usRegOffsetDecoderByID) << 16) + msRead2Byte(REG_DPRX_DECODER_E0_30_L + usRegOffsetDecoderByID);
		ulMValueTotal = ulMValueTotal + ulBaseMValue;
		msWriteByteMask(REG_DPRX_DECODER_E0_0A_L + usRegOffsetDecoderByID, 0, BIT0); // Receiver 51
		mhal_DPRx_DELAY_NOP(0xFA);   //250 NOP equal 6.14us longer than 5k60(VTT:2942) one line time
	}

	ulBaseMValue = ulMValueTotal / ubMValueLineCount;
    ulBaseNValue = ((DWORD)msReadByte(REG_DPRX_DECODER_E0_33_L + usRegOffsetDecoderByID) << 16) + msRead2Byte(REG_DPRX_DECODER_E0_32_L);

    ulBaseMValue = ulBaseMValue * 100;

    if(ulBaseNValue > 0)
    {
        Temp = ((unsigned long long)ulBaseMValue * ulLSClock) / ulBaseNValue;
        ulPixelClock = Temp & 0xFFFFFFFF;
    }

    msWriteByteMask(REG_DPRX_DECODER_E0_0A_L + usRegOffsetDecoderByID, 0, BIT0); // Receiver 51

#if (DPRX_M_RANGE_NEW_MODE == 1)
    usTargetRange = ulBaseMValue / 100 * DPRX_M_RANGE_NEW_MODE_VALUE / 1000;

    if(usTargetRange > 0x40)
    {
        usTargetRange = usTargetRange + 0x100;
    }
#else
    if(ulPixelClock > (DPRX_M_DETECT_RANGE_PIXEL_CLK * 100)) // When pixel clk > 600MHz (ex: 4K@144), M value range should larger than befoe
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
    msWrite2Byte(REG_DPRX_DECODER_E0_16_L + usRegOffsetDecoderByID, usTargetRange); 					 // MVID_Range[15:0]

    return ulPixelClock;
}

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
#if (FPGA_Verification == 0x0)
    WORD usRegOffsetAuxPM0ByID = DP_REG_OFFSET_AUX_PM0(dprx_aux_id);
    WORD usRegOffsetAuxPM2ByID = DP_REG_OFFSET_AUX_PM2(dprx_aux_id);
    WORD usRegOffsetTransCTRLByID = DP_REG_OFFSET_TRANS_CTRL(dprx_id);

    if(((dprx_id == DPRx_ID_MAX) && (dp_pm_Mode != DP_ePM_POWEROFF_NoUsedPort)) || (dprx_aux_id == DPRx_AUX_ID_MAX) || (dp_pm_Mode == DP_ePM_INVAILD))
    {
        // Power down PHY1 if DPC port is not used
        if(DPRx_C3_AUX == AUX_None)
        {
            mhal_DPRx_PHYPowerModeSetting(DP_ePM_POWEROFF, DPRx_ID_2, DPRx_PHY_ID_1);
        }

        return;
    }

#if (ENABLE_DP_DCOFF_HPD_HIGH == 1)
    if(dp_pm_Mode == DP_ePM_POWEROFF)
    {
        dp_pm_Mode = DP_ePM_STANDBY;
    }
#endif

    msWriteByteMask(REG_DPRX_AUX_PM0_57_L + usRegOffsetAuxPM0ByID, 0, BIT0|BIT1|BIT2); // Disble DP event to PM, these 3 bits would affect HDMI/VGA wake-up

    switch(dp_pm_Mode)
    {
        case DP_ePM_POWERON:
            //APLL
            msWriteByteMask(REG_DPRX_PLL_TOP0_12_L, 0, BIT0);    // reg_pd_aupll
            msWriteByteMask(REG_DPRX_PLL_TOP0_12_L, 0, BIT1);    // reg_pd_aupll_kf0
            msWriteByteMask(REG_DPRX_PLL_TOP0_12_L, 0, BIT2);    // reg_pd_aupll_kp0
            msWriteByteMask(REG_DPRX_PLL_TOP0_12_L, 0, BIT3);    // reg_pd_aupll_kp1
            msWriteByteMask(REG_DPRX_PLL_TOP0_12_L, 0, BIT4);    // reg_pd_aupll_kpdiv
            //VPLL
            msWriteByteMask(REG_DPRX_PLL_TOP0_32_L, 0, BIT0);    // reg_pd_aupll
            msWriteByteMask(REG_DPRX_PLL_TOP0_32_L, 0, BIT1);    // reg_pd_aupll_kf0
            msWriteByteMask(REG_DPRX_PLL_TOP0_32_L, 0, BIT2);    // reg_pd_aupll_kp0
            msWriteByteMask(REG_DPRX_PLL_TOP0_32_L, 0, BIT3);    // reg_pd_aupll_kp1
            msWriteByteMask(REG_DPRX_PLL_TOP0_32_L, 0, BIT4);    // reg_pd_aupll_kpdiv
            //AUX_ATOP
            msWriteByteMask(REG_DPRX_AUX_PM0_54_L + usRegOffsetAuxPM0ByID, BIT2, BIT2); // reg_en_aux
            msWriteByteMask(REG_DPRX_AUX_PM2_61_L + usRegOffsetAuxPM2ByID, 0, BIT1|BIT2|BIT7); // reg_en_gpio, en_ftmux, pd_vcm_op
            msWriteByteMask(REG_DPRX_AUX_PM0_54_H + usRegOffsetAuxPM0ByID, 0, BIT5); // pd_aux_rterm

            msWriteByteMask(REG_PM_48_H, (BIT4|BIT0) << dprx_aux_id, (BIT4|BIT0) << dprx_aux_id); // Enable AUX clock [REG_000391]
             mhal_DPRx_SetAuxClockGating(FALSE);
            msWriteByteMask(REG_DPRX_AUX_PM0_4D_L + usRegOffsetAuxPM0ByID, BIT1, BIT1); // MCCS reply by MCU

            msWriteByteMask(REG_DPRX_TRANS_CTRL_07_L + usRegOffsetTransCTRLByID, DP_RX_FT_TIMER_A_WAIT_SQ_SIGNAL, BIT0|BIT1|BIT2|BIT3); // Set fast training delay time to 1 sec
            break;

        case DP_ePM_STANDBY:
            //APLL
            msWriteByteMask(REG_DPRX_PLL_TOP0_12_L, BIT0, BIT0);    // reg_pd_aupll
            msWriteByteMask(REG_DPRX_PLL_TOP0_12_L, BIT1, BIT1);    // reg_pd_aupll_kf0
            msWriteByteMask(REG_DPRX_PLL_TOP0_12_L, BIT2, BIT2);    // reg_pd_aupll_kp0
            msWriteByteMask(REG_DPRX_PLL_TOP0_12_L, BIT3, BIT3);    // reg_pd_aupll_kp1
            msWriteByteMask(REG_DPRX_PLL_TOP0_12_L, BIT4, BIT4);    // reg_pd_aupll_kpdiv
            //VPLL
            msWriteByteMask(REG_DPRX_PLL_TOP0_32_L, BIT0, BIT0);    // reg_pd_aupll
            msWriteByteMask(REG_DPRX_PLL_TOP0_32_L, BIT1, BIT1);    // reg_pd_aupll_kf0
            msWriteByteMask(REG_DPRX_PLL_TOP0_32_L, BIT2, BIT2);    // reg_pd_aupll_kp0
            msWriteByteMask(REG_DPRX_PLL_TOP0_32_L, BIT3, BIT3);    // reg_pd_aupll_kp1
            msWriteByteMask(REG_DPRX_PLL_TOP0_32_L, BIT4, BIT4);    // reg_pd_aupll_kpdiv
            //AUX_ATOP
            msWriteByteMask(REG_DPRX_AUX_PM0_54_L + usRegOffsetAuxPM0ByID, BIT2, BIT2); // reg_en_aux
            msWriteByteMask(REG_DPRX_AUX_PM2_61_L + usRegOffsetAuxPM2ByID, 0, BIT1|BIT2|BIT7); // reg_en_gpio, en_ftmux, pd_vcm_op
            msWriteByteMask(REG_DPRX_AUX_PM0_54_H + usRegOffsetAuxPM0ByID, 0, BIT5); // pd_aux_rterm

            msWriteByteMask(REG_PM_48_H, (BIT4|BIT0) << dprx_aux_id, (BIT4|BIT0) << dprx_aux_id); // Enable AUX clock [REG_000391]
             mhal_DPRx_SetAuxClockGating(FALSE);

            msWriteByteMask(REG_DPRX_AUX_PM0_4D_L + usRegOffsetAuxPM0ByID, 0, BIT1); // MCCS reply by MCU

            ubHaveBeenSleeped[dprx_aux_id] = 1;
            break;

        case DP_ePM_POWEROFF:
            //APLL
            msWriteByteMask(REG_DPRX_PLL_TOP0_12_L, BIT0, BIT0);    // reg_pd_aupll
            msWriteByteMask(REG_DPRX_PLL_TOP0_12_L, BIT1, BIT1);    // reg_pd_aupll_kf0
            msWriteByteMask(REG_DPRX_PLL_TOP0_12_L, BIT2, BIT2);    // reg_pd_aupll_kp0
            msWriteByteMask(REG_DPRX_PLL_TOP0_12_L, BIT3, BIT3);    // reg_pd_aupll_kp1
            msWriteByteMask(REG_DPRX_PLL_TOP0_12_L, BIT4, BIT4);    // reg_pd_aupll_kpdiv
            //VPLL
            msWriteByteMask(REG_DPRX_PLL_TOP0_32_L, BIT0, BIT0);    // reg_pd_aupll
            msWriteByteMask(REG_DPRX_PLL_TOP0_32_L, BIT1, BIT1);    // reg_pd_aupll_kf0
            msWriteByteMask(REG_DPRX_PLL_TOP0_32_L, BIT2, BIT2);    // reg_pd_aupll_kp0
            msWriteByteMask(REG_DPRX_PLL_TOP0_32_L, BIT3, BIT3);    // reg_pd_aupll_kp1
            msWriteByteMask(REG_DPRX_PLL_TOP0_32_L, BIT4, BIT4);    // reg_pd_aupll_kpdiv
            //AUX_ATOP
            msWriteByteMask(REG_DPRX_AUX_PM0_54_L + usRegOffsetAuxPM0ByID, 0, BIT2); // reg_en_aux
            msWriteByteMask(REG_DPRX_AUX_PM2_61_L + usRegOffsetAuxPM2ByID, BIT7, BIT1|BIT2|BIT7); // reg_en_gpio, en_ftmux, pd_vcm_op
            msWriteByteMask(REG_DPRX_AUX_PM0_54_H + usRegOffsetAuxPM0ByID, BIT5, BIT5); // pd_aux_rterm

            msWriteByteMask(REG_PM_48_H, 0, (BIT4|BIT0) << dprx_aux_id); // Enable AUX clock [REG_000391]

            if(msReadByte(REG_PM_48_H) == 0x0) //[REG_000391]
            {
                 mhal_DPRx_SetAuxClockGating(TRUE);
            }

            msWriteByteMask(REG_DPRX_AUX_PM0_4D_L + usRegOffsetAuxPM0ByID, 0, BIT1); // MCCS reply by MCU

            ubHaveBeenSleeped[dprx_aux_id] = 1;
            break;

        case DP_ePM_POWEROFF_NoUsedPort: // Only for unused AUX (ex: HDMI case)
            //AUX_ATOP
            msWriteByteMask(REG_DPRX_AUX_PM0_54_L + usRegOffsetAuxPM0ByID, 0, BIT2); // reg_en_aux
            msWriteByteMask(REG_DPRX_AUX_PM2_61_L + usRegOffsetAuxPM2ByID, BIT7, BIT1|BIT2|BIT7); // reg_en_gpio, en_ftmux, pd_vcm_op
            msWriteByteMask(REG_DPRX_AUX_PM0_54_H + usRegOffsetAuxPM0ByID, BIT5, BIT5); // pd_aux_rterm

            msWriteByteMask(REG_PM_48_H, 0, (BIT4|BIT0) << dprx_aux_id); // Enable AUX clock [REG_000391]

            if(msReadByte(REG_PM_48_H) == 0x0) //[REG_000391]
            {
                msWriteByteMask(REG_PM_49_L, BIT0, BIT0); // Enable AUX clock gating if all AUX is not used [REG_000392]
            }

            ubHaveBeenSleeped[dprx_aux_id] = 1;
            break;

        default:
            break;
    }
#else
    UNUSED(dprx_id);
    UNUSED(dprx_aux_id);
    UNUSED(dp_pm_Mode);
#endif

    return;
}
#endif // _mhal_DPRx_C_

