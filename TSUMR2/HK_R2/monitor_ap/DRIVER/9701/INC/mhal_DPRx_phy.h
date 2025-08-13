////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (��MStar Confidential Information��) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MHAL_DPRXPHY_H
#define MHAL_DPRXPHY_H

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "dpCommon.h"

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
typedef enum
{
    mhal_ePM_POWERON,
    mhal_ePM_STANDBY,
    mhal_ePM_POWEROFF,
    mhal_ePM_INVAILD
}mhal_DP_PM_Mode;

typedef enum _DPRx_AUTOEQMODE
{
	DPRx_EQ_MODE_NONE 		= 0,
	DPRx_EQ_UNDER_OVER_MODE = 1,
	DPRx_EQ_UNDER_ONLY_MODE = 2,
	DPRx_EQ_MODE_MAX,
}DPRx_AUTOEQMODE;

typedef enum _DPRx_AUTOEQABAA
{
	DPRx_EQ_ABAA_DENOMIATOR_NONE = 0,
	DPRx_EQ_ABAA_DENOMIATOR_10 	 = 1,
	DPRx_EQ_ABAA_DENOMIATOR_20 	 = 2,
	DPRx_EQ_ABAA_DENOMIATOR_40 	 = 3,
	DPRx_EQ_ABAA_DENOMIATOR_80 	 = 4,
	DPRx_EQ_ABAA_DENOMIATOR_MAX,
}DPRx_AUTOEQABAA;

typedef enum _DPRx_AUTOEQICTRL
{
	DPRx_EQ_ICTRL_NONE = 0,
	DPRx_EQ_ICTRL1 	   = 1,
	DPRx_EQ_ICTRL2 	   = 2,
	DPRx_EQ_ICTRL3 	   = 3,
	DPRx_EQ_ICTRL4 	   = 4,
	DPRx_EQ_ICTRL5 	   = 5,
	DPRx_EQ_ICTRL6 	   = 6,
	DPRx_EQ_ICTRL7 	   = 7,
	DPRx_EQ_ICTRL8 	   = 8,
	DPRx_EQ_ICTRL_MAX,
}DPRx_AUTOEQICTRL;

typedef enum _DPRx_PHASE_LOCK_MODE
{
	DPRx_PHASE_LOCK_NONE 		= 0,
	DPRx_PHASE_LOCK_DIRECTLY 	= 1,
	DPRx_PHASE_LOCK_COMPETITION = 2,
	DPRx_PHASE_LOCK_MAX,
}DPRx_PHASE_LOCK_MODE;

//-------------------------------------------------------------------------------------------------
//  Function Prototype
//-------------------------------------------------------------------------------------------------
void mhal_DPRx_FunctionMuxMode(DPRx_PHY_ID dprx_phy_id, BOOL bEnable);
void mhal_DPRx_DellMode_Enable(DPRx_PHY_ID dprx_phy_id, BOOL bEnable);
void mhal_DPRx_DellMode_FastModeEnable(DPRx_PHY_ID dprx_phy_id, BOOL bEnable);
void mhal_DPRx_DellMode_JmumpModeEnable(DPRx_PHY_ID dprx_phy_id, BOOL bEnable, BYTE swing, BYTE preemphasis, BYTE to_swing, BYTE to_preemphasis);
void mhal_DPRx_DellMode_PKModeEnable(DPRx_PHY_ID dprx_phy_id, BOOL bEnable, BYTE stage, BYTE swing, BYTE preemphasis);
void mhal_DPRx_DellMode_PKforceSetting(DPRx_PHY_ID dprx_phy_id, BYTE swing, BYTE preemphasis);
void mhal_DPRx_DellMode_ACHSetting(DPRx_PHY_ID dprx_phy_id, BYTE swing, BYTE preemphasis);
void mhal_DPRx_PHYInitialSetting(DPRx_ID dprx_id, DPRx_PHY_ID dprx_phy_id);
void mhal_DPRx_EnablePHYInterrupt(DPRx_PHY_ID dprx_phy_id, BOOL bEnable);
void mhal_DPRx_PowerDownEQEnable(DPRx_PHY_ID dprx_phy_id, BOOL bEnable);
void mhal_DPRx_AutoEQModeSetting(DPRx_PHY_ID dprx_phy_id, BYTE EQ_mode);
void mhal_DPRx_AutoEQStartSetting(DPRx_PHY_ID dprx_phy_id, BYTE EQ_Start);
void mhal_DPRx_AutoEQRangeSetting(DPRx_PHY_ID dprx_phy_id, BYTE EQ_Max, BYTE EQ_Min);
void mhal_DPRx_AutoEQABAASetting(DPRx_PHY_ID dprx_phy_id, BYTE ABAA_mode);
void mhal_DPRx_AutoEQictrlSetting(DPRx_PHY_ID dprx_phy_id, BYTE ICTRL_mode);
void mhal_DPRx_PhaseThresholdSetting(DPRx_PHY_ID dprx_phy_id, BYTE Phase_Threshold);
void mhal_DPRx_MuxSelect(DPRx_ID dprx_id, DPRx_AUX_ID dprx_aux_id);
void mhal_DPRx_Select_ACDR_Lane(DPRx_PHY_ID dprx_phy_id , BYTE lane_no);
void mhal_DPRx_Switch_Training_Setting(DPRx_PHY_ID dprx_phy_id , BYTE link_rate);
void mhal_DPRx_SetEQ_Current(DPRx_PHY_ID dprx_phy_id ,BOOL eq_current);
void mhal_DPRx_Switch_Training_Setting_FT(DPRx_PHY_ID dprx_phy_id);
void mhal_DPRx_PHYPowerModeSetting(DP_ePM_Mode dp_pm_Mode, DPRx_ID dprx_id, DPRx_PHY_ID dprx_phy_id);
void mhal_DPRx_SetPHYLanePNSwapEnable(DPRx_PHY_ID dprx_phy_id,  BOOL bEnable, BYTE ubLanePNSwapSelect);
void mhal_DPRx_SetPHYLaneSwapEnable(DPRx_ID dprx_id, DPRx_PHY_ID dprx_phy_id, BOOL bEnable, BYTE ubTargetLaneNumber);
void mhal_DPRx_SetPHYPRBS7PNSwapEnable(DPRx_PHY_ID dprx_phy_id, BYTE ubLanePNSwapSelect );
void mhal_DPRx_PHYCDRDetectEnable(DPRx_PHY_ID dprx_phy_id, BOOL bEnable);
void mhal_DPRx_Check_AGC_Reset(DPRx_PHY_ID dprx_phy_id);
#endif // MHAL_DPRXPHY_H

