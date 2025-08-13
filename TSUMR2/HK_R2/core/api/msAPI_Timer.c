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
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
/// @file msAPI_Timer.h
/// MStar Application Interface for Timer
/// @brief API for Timer functions
/// @author MStar Semiconductor, Inc.
//
////////////////////////////////////////////////////////////////////////////////

#define MSAPI_TIMER_C

/******************************************************************************/
/*                            Header Files                                    */
/******************************************************************************/
#include "datatype.h"
#include "board.h"
#include "SW_Config.h"
#include "default_option_define.h"
#include "sysinfo.h"

#include "SysInit.h"
#include "drvCPU.h"
//#include "msAPI_MailBox.h"
#include "drvGlobal.h"
#include "msAPI_Timer.h"
#include "MsCommon.h"
#include "MsIRQ.h"
#include "MsOS.h"
#if OBA2
#include "msAPI_DrvInit.h"
#include "msAPI_APEngine.h"
#endif

#if MHEG5_ENABLE
#include "apiXC.h"
#include "apiXC_Adc.h"
#include "msAPI_Global.h"
#include "MApp_GlobalSettingSt.h"
#include "msAPI_MHEG5.h"
#endif

/******************************************************************************/
/*                               Functions                                    */
/******************************************************************************/
static BOOLEAN fSetSystemTime = 1;
#if defined(MIPS_CHAKRA)
extern void MsOS_DelayTask(U32 u32milisec);
#endif

static S32 gS32NextTimeOffset = 0;
static U32 gU32TimeOfChange = 0;
//extern S32 volatile gS32OffsetTime;
S32 volatile gS32OffsetTime;

/******************************************************************************/
/// API for check if system time has been set
/******************************************************************************/
BOOLEAN msAPI_Timer_IsSystemTimeValid(void)
{
    return (BOOLEAN)(fSetSystemTime);
}
extern U32 volatile gSystemTimeCount;
extern U32 volatile gTimerCount0;
extern U32 volatile gTimerDownCount0;
extern U32 volatile gTimerDownCount1;
extern U8  gu8100msTH;
extern U16 g_u16ElapsedTimeSecond;
void msAPI_Timer_DisableSystemTime(void)
{
    gSystemTimeCount = 0;
    fSetSystemTime = 0;
}

#define PM_RTC_UPDATE_TIMEOUT           1000 //ms

void msAPI_Timer_SyncToPmRTC(void)
{
    #if 1
    //static U32 preSyncTime = 0;

    U32 u32PmRTC_Time;

    //if( msAPI_Timer_DiffTimeFromNow(preSyncTime) > PM_RTC_UPDATE_TIMEOUT )
    {
        u32PmRTC_Time = 0; //No RTC in MST9U //MDrv_PM_RTCGetSystemTime();
        gSystemTimeCount=u32PmRTC_Time;
        //preSyncTime = msAPI_Timer_GetTime0();
    }
    #endif
}


/******************************************************************************/
/// API for Get system time ticks
/// @return -Current system time counts, the unit is system default.
/******************************************************************************/
U32 msAPI_Timer_GetTime0(void)
{
    return MsOS_GetSystemTime();
}
static MS_U32 _TimerTarget=0;
U32 msAPI_Timer_GetDownTimer0(void)
{
    U32 ret;
    if(msAPI_Timer_GetTime0()>=_TimerTarget)
    {
        _TimerTarget=0;
        ret = 0 ;
    }
    else
    {
        ret = _TimerTarget-msAPI_Timer_GetTime0() ;
    }

    return ret;
}
void msAPI_Timer_SetDownTimer0(U32 u32Timer)
{
    _TimerTarget = msAPI_Timer_GetTime0() +u32Timer;
}

U32 msAPI_Timer_GetSystemTime(void)
{
    return gSystemTimeCount;
}

S32 msAPI_Timer_GetOffsetTime(void)
{
    if ((gU32TimeOfChange!=0) && (gSystemTimeCount>=gU32TimeOfChange))
    {
        if (gS32OffsetTime!=gS32NextTimeOffset)
        {
            gS32OffsetTime = gS32NextTimeOffset;
        }
    }

    return gS32OffsetTime;
}
extern U32 volatile gWakeupSystemTime;//Need to delete extern
U32 msAPI_Timer_GetWakeupTime(void)
{
    return gWakeupSystemTime;
}
void msAPI_Timer_SetWakeupTime(U32 u32Time)
{
    gWakeupSystemTime = u32Time;
}

/******************************************************************************/
/// API for Calculate the difference of two time values. This function will handle overflow problem.
/// @param -u32Timer \b IN  First time value to calculate
/// @param -u32TaskTimer \b IN  2nd time value to calculate
/// @return -Time difference of 2 time values.
/******************************************************************************/
U32 msAPI_Timer_DiffTime(U32 u32Timer, U32 u32TaskTimer)
{
    if (u32TaskTimer >= u32Timer)
    {
        //return 0;
        return (u32TaskTimer - u32Timer);
    }
    else
    {
        //return MsOS_Timer_DiffTime(u32Timer, u32TaskTimer);
        return (u32Timer - u32TaskTimer);
    }
}

/******************************************************************************/
/// API for Calculating the difference between time in parameter and current time.
/// @param -u32TaskTimer \b IN  2nd time value to calculate
/// @return -Time difference between current time and task time
/******************************************************************************/
U32 msAPI_Timer_DiffTimeFromNow(U32 u32TaskTimer) //unit = ms
{
    return MsOS_Timer_DiffTimeFromNow(u32TaskTimer);
}

/******************************************************************************/
/// API for Delay function
/// @param -u32DelayTime \b IN  required time period to delay.
/******************************************************************************/
void msAPI_Timer_Delayms(U32 u32DelayTime) //unit = ms
{
    MsOS_DelayTask(u32DelayTime);
}
/******************************************************************************/
/// API for Set RTC Match time.
/// @param -u32SystemTime \b IN  RTC Match time to set
///
/// The unit is depends on the "period" which is setting by USER_TIMER_PERIOD
/******************************************************************************/
void msAPI_Timer_SetRTCWakeUpTime(U32 u32SystemTime) //unit = sec
{
#if (ENABLE_RTC)
    MDrv_PM_RTCSetMatchTime(u32SystemTime);
#else
    u32SystemTime = u32SystemTime;
#endif
}

U32 msAPI_Timer_GetRTCWakeUpTime(void) //unit = sec
{
#if (ENABLE_RTC)
    return MDrv_PM_RTCGetMatchTime();
#else
    return 0;
#endif
}

void msAPI_Timer_EnableRTCWakeUp(BOOLEAN bEnable)
{
#if (ENABLE_RTC)
    MDrv_PM_RTCEnableInterrupt(bEnable);
#else
    bEnable = bEnable;
#endif
}

U32 msAPI_Timer_GetRTCSystemTime(void) //unit = sec
{
#if (ENABLE_RTC)
    return MDrv_PM_RTCGetSystemTime();
#else
    return 0;
#endif
}
#ifdef MSOS_TYPE_LINUX
#include <time.h>
void msAPI_Timer_SetOSSystemTime(U32 u32SystemTime)
{
    time_t timep = u32SystemTime;
    time_t offset_time = ((365*10)+2)*24*60*60; // from 1970/1/1 to 1980/1/1

    timep = timep + offset_time;
    stime(&timep);
}
#endif
/******************************************************************************/
/// API for Set system time.
/// @param -u32SystemTime \b IN  System time to set
///
/// The unit is depends on the "period" which is setting by USER_TIMER_PERIOD
/******************************************************************************/
void msAPI_Timer_SetSystemTime(U32 u32SystemTime) //unit = sec
{
    gSystemTimeCount=u32SystemTime;
    //-when set system time 0, it also sets RTC counter at the same time.
#if (ENABLE_RTC)
    MDrv_PM_RTCSetSystemTime(u32SystemTime);
#endif
#ifdef MSOS_TYPE_LINUX
    msAPI_Timer_SetOSSystemTime(u32SystemTime);
#endif
}

void msAPI_Timer_SetOffsetTime(S32 s32OffsetTime) //unit = sec
{
#if MHEG5_ENABLE
    if((IsDTVInUse()) &&(msAPI_Timer_GetOffsetTime() != s32OffsetTime)&&msAPI_MHEG5_GetBinStatus())
    {
        msAPI_MHEG5_SystemTime_Transmit(msAPI_Timer_GetSystemTime(),s32OffsetTime);
    }
#endif
#if defined (DVB_SYSTEM)
    if(s32OffsetTime != msAPI_Timer_GetOffsetTime())
    {
        stGenSetting.g_Time.s32Offset_Time = s32OffsetTime;
        gS32OffsetTime = s32OffsetTime;
    }
#else
    gS32OffsetTime = s32OffsetTime;
#endif
}


S32 msAPI_Timer_Load_OffsetTime(void)
{
    S32 s32OffsetTime = 0;
#if defined (DVB_SYSTEM)
    s32OffsetTime = stGenSetting.g_Time.s32Offset_Time;
#endif

    gS32OffsetTime = s32OffsetTime;

    return s32OffsetTime;
}


void msAPI_Timer_ResetWDT(void)
{
    MDrv_Sys_ClearWatchDog();
}

void msAPI_Timer_SetTimeOfChange(U32 u32TimeOfChg) //unit = sec
{
    gU32TimeOfChange = u32TimeOfChg;
}
void msAPI_Timer_SetNextTimeOffset(S32 s32OffsetTime) //unit = sec
{
    gS32NextTimeOffset = s32OffsetTime;
}

#undef MSAPI_TIMER_C

