//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of MStar Software is expressly prohibited, unless prior written
// permission has been granted by MStar.
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>

#ifndef _DEFAULT_OPTION_DEFINE_H_
#define _DEFAULT_OPTION_DEFINE_H_

/*************************************************************************************************************/

#ifndef DEBUG_AUDIO_DETECT_TIME
    #define DEBUG_AUDIO_DETECT_TIME                         0
#endif

#ifndef DTV_SCAN_AUTO_FINE_TUNE_ENABLE
    #define DTV_SCAN_AUTO_FINE_TUNE_ENABLE                  0
#endif

/*
    This function is that user can input any frequency point when DTV manual scan
*/
#ifndef ENABLE_SZ_DTV_OFFSET_FUNCTION
    #define ENABLE_SZ_DTV_OFFSET_FUNCTION                   0
#endif

/*
    This function is that user can input any frequency point when DTV manual scan
*/
#ifndef ENABLE_SZ_DTV_ADDCH_SCAN_FUNCTION
    #define ENABLE_SZ_DTV_ADDCH_SCAN_FUNCTION               0
#endif

#ifndef ENABLE_SZ_FACTORY_PICTURE_CURVE_FUNCTION
    #define ENABLE_SZ_FACTORY_PICTURE_CURVE_FUNCTION        0
#endif

#ifndef ENABLE_SZ_FACTORY_SOUND_CURVE_FUNCTION
    #define ENABLE_SZ_FACTORY_SOUND_CURVE_FUNCTION          0
#endif

#ifndef ENABLE_SZ_FACTORY_OVER_SCAN_FUNCTION
    #define ENABLE_SZ_FACTORY_OVER_SCAN_FUNCTION            0
#endif

#ifndef ENABLE_SZ_NEW_FACTORY_MEMU_FUNCTION
    #define ENABLE_SZ_NEW_FACTORY_MEMU_FUNCTION             0
#endif

#ifndef ENABLE_SZ_FACTORY_SOUND_MODE_FUNCTION
    #define ENABLE_SZ_FACTORY_SOUND_MODE_FUNCTION           0
#endif

#ifndef ENABLE_SZ_FACTORY_WB_ADJUST_FUNCTION
    #define ENABLE_SZ_FACTORY_WB_ADJUST_FUNCTION            0
#endif

#ifndef ENABLE_SZ_FACTORY_OTHER_SETTING_FUNCTION
    #define ENABLE_SZ_FACTORY_OTHER_SETTING_FUNCTION        0
#endif

#ifndef ENABLE_SZ_FACTORY_USB_SAVE_DATABASE_FUNCTION
    #define ENABLE_SZ_FACTORY_USB_SAVE_DATABASE_FUNCTION    1
#endif

#ifndef ENABLE_SZ_FACTORY_OSD_BLENDING_FUNCTION
    #define ENABLE_SZ_FACTORY_OSD_BLENDING_FUNCTION         0
#endif

#ifndef ENABLE_SZ_FACTORY_SOFTWARE_UPDATE_FUNCTION
    #define ENABLE_SZ_FACTORY_SOFTWARE_UPDATE_FUNCTION      0
#endif

#ifndef ENABLE_SZ_FACTORY_EQ_BAND_FUNCTION
    #define ENABLE_SZ_FACTORY_EQ_BAND_FUNCTION              0
#endif

#ifndef ENABLE_SZ_FACTORY_YPBPR_PHASE_FUNCTION
    #define ENABLE_SZ_FACTORY_YPBPR_PHASE_FUNCTION          0
#endif

#ifndef ENABLE_SZ_HDMI_AVMUTE_TEST_FUNCTION
    #define ENABLE_SZ_HDMI_AVMUTE_TEST_FUNCTION             0
#endif

#ifndef ENABLE_SZ_SCAN_LEFT_RIGHT_FUNCTION
    #define ENABLE_SZ_SCAN_LEFT_RIGHT_FUNCTION              0
#endif

#ifndef ENABLE_HIS_LVDS
    #define ENABLE_HIS_LVDS                                 0
#endif

#ifndef PANEL_SWAP_LVDS_POL
    #define PANEL_SWAP_LVDS_POL                             0
#endif


#ifndef ENABLE_HOTEL_MODE_FUNCTION
    #define  ENABLE_HOTEL_MODE_FUNCTION                     0
#endif

#ifndef ENABLE_SZ_BLUESCREEN_FUNCTION
    #define ENABLE_SZ_BLUESCREEN_FUNCTION                   0
#endif

#ifndef ADC_AUTO_GAIN_CABLICATION_WITHOUT_EXT_SIGNAL
    #define ADC_AUTO_GAIN_CABLICATION_WITHOUT_EXT_SIGNAL    0
#endif

#ifndef ENABLE_CH_VOLUME_COMP
    #define ENABLE_CH_VOLUME_COMP                           0
#endif

#ifndef ENABLE_SW_CH_FREEZE_SCREEN
    #define ENABLE_SW_CH_FREEZE_SCREEN                      0
#endif

#ifndef ENABLE_VGA_EIA_TIMING
#define ENABLE_VGA_EIA_TIMING                               1
#endif

#ifndef ENABLE_DMP_POWERON
    #define ENABLE_DMP_POWERON                              0
#endif

#ifndef ENABLE_CH_FORCEVIDEOSTANDARD
    #define ENABLE_CH_FORCEVIDEOSTANDARD                    0
#endif

#ifndef ENABLE_BACKLIGHT_ADJUST
    #define ENABLE_BACKLIGHT_ADJUST             0
#endif

//------ PANEL RELATED ---------------------------------------------------------
#ifndef PANEL_PDP_10BIT
    #define PANEL_PDP_10BIT                     0
#endif

#ifndef LVDS_PN_SWAP_L
    #define LVDS_PN_SWAP_L                      0x00
#endif

#ifndef LVDS_PN_SWAP_H
    #define LVDS_PN_SWAP_H                      0x00
#endif

#ifndef PANEL_SWAP_LVDS_CH
    #define PANEL_SWAP_LVDS_CH                  0x00
#endif

#ifndef BD_LVDS_CONNECT_TYPE
    #define BD_LVDS_CONNECT_TYPE                0x00
#endif

//------ HDMI RELATED ---------------------------------------------------------
#ifndef HDCP_HPD_INVERSE
    #define HDCP_HPD_INVERSE                    ENABLE
#endif

#ifndef ENABLE_FACTORY_POWER_ON_MODE
    #define ENABLE_FACTORY_POWER_ON_MODE        DISABLE
#endif

#ifndef ENABLE_FACTORY_PANEL_SEL
    #define ENABLE_FACTORY_PANEL_SEL            DISABLE
#endif

//------ PACKAGE RELATED ---------------------------------------------------------
#ifndef SHARE_GND
    #define SHARE_GND                           ENABLE//DISABLE
#endif

//------  Board Related -----------------------------------------------------------
#ifndef SCART_ID_SEL
    #define SCART_ID_SEL                        0
#endif

#ifndef SCART2_ID_SEL
    #define SCART2_ID_SEL                       0
#endif

// ------ Demo Fine tune -----------------------------------------------------------
#ifndef ENABLE_DEMO_FINE_TUNE
    #define ENABLE_DEMO_FINE_TUNE               0
#endif

// ------ Flash control -----------------------------------------------------
#ifndef FLASH_WP_PIN_CONTROL
    #define FLASH_WP_PIN_CONTROL                DISABLE
#endif
// ATV vtotal change patch
#ifndef PATCH_FOR_V_RANGE
    #define PATCH_FOR_V_RANGE                   DISABLE
#endif

#ifndef ENABLE_ATV_MODE_SHOW_NO_SIGNAL
    #define ENABLE_ATV_MODE_SHOW_NO_SIGNAL      0
#endif

#ifndef _AutoNR_EN_
    #define _AutoNR_EN_                         0
#endif

#ifndef ENABLE_NEW_AUTO_NR
    #define ENABLE_NEW_AUTO_NR                  0
#endif

#ifndef ENABLE_TTX_SHOW_OFF_SIGNAL
    #define ENABLE_TTX_SHOW_OFF_SIGNAL          0
#endif

#ifndef ENABLE_EPGTIMER_RECORDER_TURNOFFPANEL
    #define ENABLE_EPGTIMER_RECORDER_TURNOFFPANEL           0
#endif

#ifndef ENABLE_SCART_YC_INPUT
    #define ENABLE_SCART_YC_INPUT               0
#endif

#ifndef MPEG_HD_PRE_SCAL_DOWN
     #define MPEG_HD_PRE_SCAL_DOWN              DISABLE
#endif

#ifndef ENABLE_FRANCE_DTV
     #define ENABLE_FRANCE_DTV                  DISABLE
#endif

#ifndef ENABLE_TIMESHIT
     #define ENABLE_TIMESHIT                    DISABLE
#endif

#ifndef ENABLE_SMOOTH_PANEL_OUTPUT
     #define ENABLE_SMOOTH_PANEL_OUTPUT         DISABLE
#endif

#ifndef ENABLE_BACKLIGHT_PWM_SYNC_WITH_FRAMERATE
     #define ENABLE_BACKLIGHT_PWM_SYNC_WITH_FRAMERATE       DISABLE
#endif

#define RATIO_OF_BACKLIGHT_FREQ_OVER_FRAMERATE              2

#ifndef ENABLE_SZ_FACTORY_USB_SAVE_MAP_FUNCTION
    #define ENABLE_SZ_FACTORY_USB_SAVE_MAP_FUNCTION         DISABLE
#endif

#ifndef ENABLE_CHECK_WSS_INFO_CHANGE
#define ENABLE_CHECK_WSS_INFO_CHANGE            0
#endif


#ifndef COMPRESS_BIN_AUDIO_DEC
    #define COMPRESS_BIN_AUDIO_DEC              0
#endif

#ifndef DTV_COUNT_DOWN_TIMER_PATCH
    #define DTV_COUNT_DOWN_TIMER_PATCH          0
#endif

#ifndef ENABLE_DMP_DLC
    #define ENABLE_DMP_DLC                      0
#endif

#ifndef LOAD_CCFONT_ONCE
    #define LOAD_CCFONT_ONCE                    0
#endif

#ifndef ENABLE_HDMI_4K_2K
    #define ENABLE_HDMI_4K_2K                   0
#endif

#ifndef MHEG5_ENABLE
    #define MHEG5_ENABLE                        0
#endif

#ifndef ENABLE_RTC
    #define ENABLE_RTC                          0
#endif

#ifndef ENABLE_PVR
    #define ENABLE_PVR                          0
#endif

#ifndef ENABLE_CARDREADER
    #define ENABLE_CARDREADER                   0
#endif

#ifndef ENABLE_USB_DOWNLOAD_BIN
    #define ENABLE_USB_DOWNLOAD_BIN             0
#endif

#ifndef ENABLE_6M30_3D_PROCESS
    #define ENABLE_6M30_3D_PROCESS              0
#endif

/*************************************************************************************************************/
#endif //#ifndef _DEFAULT_OPTION_DEFINE_H_
