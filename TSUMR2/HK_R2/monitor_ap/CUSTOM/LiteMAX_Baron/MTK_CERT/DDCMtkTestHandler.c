#include "DDCMtkTestHandler.h"
#include "Menu.h"
#include "menufunc.h"
#include "Mode.h"
#include "Ms_PM.h"
#include "Power.h"

#if 1 && ENABLE_MSTV_UART_DEBUG
#   define MTKTEST_PRINT(format, ...)    printf("[MTK TEST] "format"\n", ##__VA_ARGS__)
#else
#   define MTKTEST_PRINT(format, ...)
#endif

#if ENABLE_MTK_TEST
int iElapsedTime = 0;
BYTE u8AudioStatus[2] = {0};
WORD u16LastValue[4] = {0};

void SetAutoPort(BOOL bEnable)
{
    if (bEnable)
    {
        UserPrefInputPriorityType = Input_Priority_Auto;
        Set_SaveMonitorSettingFlag();
    }
#if (INPUT_TYPE & INPUT_1A)
    else if (Input_Analog1 == UserPrefInputType)
    {
        UserPrefInputPriorityType = Input_Priority_Analog1;
    }
#endif
#if (INPUT_TYPE >= INPUT_1C)
    else if (Input_Digital == SrcInputType)
    {
        UserPrefInputPriorityType = Input_Priority_Digital1;
        Set_SaveMonitorSettingFlag();
    }
#endif
#if (INPUT_TYPE >= INPUT_2C)
    else if (Input_Digital2 == SrcInputType)
    {
        UserPrefInputPriorityType = Input_Priority_Digital2;
        Set_SaveMonitorSettingFlag();
    }
#endif
#if (INPUT_TYPE >= INPUT_3C)
    else if (Input_Digital3 == SrcInputType)
    {
        UserPrefInputPriorityType = Input_Priority_Digital3;
        Set_SaveMonitorSettingFlag();
    }
#endif
}

BYTE SwitchPort(void)
{
    BYTE u8Priority = Input_Nums;
    if (DDCBuffer[3] < Input_Nums)
    {
        u8Priority = DDCBuffer[3];
    }
    else switch (DDCBuffer[3])
    {
#if ENABLE_VGA_INPUT
        case 0x10: u8Priority = Input_VGA; break;
#endif
#if ENABLE_HDMI
        case 0x21:
        {
            if(Input_HDMI != Input_Nothing)
                u8Priority = Input_HDMI;
        }break;
        case 0x22:
        {
            if(Input_HDMI2 != Input_Nothing)
                u8Priority = Input_HDMI2;
        }break;
        case 0x23:
        {
            if(Input_HDMI3 != Input_Nothing)
                u8Priority = Input_HDMI3;
        }break;
#endif
#if ENABLE_DP_INPUT
        case 0x31:
        {
            if(Input_Displayport != Input_Nothing)
                u8Priority = Input_Displayport;
        }break;
        case 0x32:
        {
            if(Input_Displayport2 != Input_Nothing)
                u8Priority = Input_Displayport2;
        }break;
        case 0x33:
        {
            if(Input_Displayport3 != Input_Nothing)
                u8Priority = Input_Displayport3;
        }break;
#endif
        case 0x43:
        {
            if(Input_UsbTypeC3 != Input_Nothing)
                u8Priority = Input_UsbTypeC3;
        }break;
        case 0x44:
        {
            if(Input_UsbTypeC4 != Input_Nothing)
                u8Priority = Input_UsbTypeC4;
        }break;

        case 0x80: //prev port
            u8Priority = (SrcInputType - 1 + Input_Nums) % Input_Nums;
            break;

        case 0x81: //next port
            u8Priority = (SrcInputType + 1) % Input_Nums;
            break;
    }
    if (u8Priority < Input_Nums)
    {
        MTKTEST_PRINT("Switch port: %d", u8Priority);

        SrcInputType=u8Priority;
        UserPrefInputType=SrcInputType;
        Set_SaveMonitorSettingFlag();
#if Enable_PanelHandler
        Power_PanelCtrlOnOff(FALSE, TRUE);
#else
        Power_TurnOffPanel();
#endif

        mStar_SetupInputPort();
        Set_InputTimingChangeFlag();
    }
    return 0;
}

BYTE GetDPIndexByPort(WORD u8Index)
{
    if (u8Index >= Input_Nums || !INPUT_IS_DISPLAYPORT(u8Index))
        return 0;

    BYTE i = 0, n = 1;
    for (i = 0; i < u8Index; ++i)
    {
        if (INPUT_IS_DISPLAYPORT(i))
            ++n;
    }
    return n;
}

BYTE GetAudioStatus(void)
{
    MTKTEST_PRINT("Audio Status: [0x%X, 0x%X]", u8AudioStatus[0], u8AudioStatus[1]);
    DDCBuffer[3] = (u8AudioStatus[0] > DDCBuffer[3]);
    DDCBuffer[4] = (u8AudioStatus[1] > DDCBuffer[4]);
    return 4;
}

void InitMtkTest(void)
{
    msWriteByteMask(REG_112C95, 0x05, 0x0f);
}

void MtkTestUpdateHandler(void) //update every 10ms
{
    iElapsedTime += 10;
    if (iElapsedTime >= 1000)
    {
        iElapsedTime = 0;
        u8AudioStatus[0] = 0;
        u8AudioStatus[1] = 0;
    }

    WORD u16NowValue = msRead2Byte(REG_112D80);
    if (0xffff == u16NowValue || 0x0001 == u16NowValue)
    {
        u16NowValue = 0;
    }
    if (u16LastValue[0] != u16NowValue)
    {
        ++u8AudioStatus[1];
        u16LastValue[0] = u16NowValue;
    }

    u16NowValue = msRead2Byte(REG_112D82);
    if (0xffff == u16NowValue || 0x0001 == u16NowValue)
    {
        u16NowValue = 0;
    }
    if (u16LastValue[1] != u16NowValue)
    {
        ++u8AudioStatus[1];
        u16LastValue[1] = u16NowValue;
    }
}
#if MS_PM
extern XDATA sPM_Info  sPMInfo;
#endif
BYTE MtkTestCmdHandler(void)
{
    switch (DDCBuffer[2])
    {
        case MTK_TEST_GET_IP_NUM:
            DDCBuffer[0] = 0x83;
            DDCBuffer[3] = Input_Nums;
            return 3;

        case MTK_TEST_GET_IP_STATUS:
            return 0;//GetIPStatus(u8Source);

        case MTK_TEST_GET_SC_IN_STATUS:
            return 0;//GetScalerInStatus(u8Source);

        case MTK_TEST_GET_SC_OUT_STATUS:
            return 0;//GetScalerOutStatus(u8Source);

        case MTK_TEST_GET_DW_STATUS:
            DDCBuffer[0] = 0x83;
            if(g_bDisplayOK && (!SyncLossState()))
                DDCBuffer[3] = 1;
            else
                DDCBuffer[3] = 0;
            return 3;//GetWindowStatus(u8Source);
#if MS_PM
        case MTK_TEST_GET_PM_STATUS:
        {
            DDCBuffer[0] = 0x84;
            DDCBuffer[3] = sPMInfo.ucPMMode;
            DDCBuffer[4] = sPMInfo.ePMState; //ePM_IDLE
            return 4;
        }    
#endif
        //case MTK_TEST_GET_USBC_PD_STATUS:
        //case MTK_TEST_GET_TIMING_INFO:

        case MTK_TEST_GET_AUDIO_STATUS:
            return GetAudioStatus();

        case MTK_TEST_GET_HDCP_CAP:
            return 0;//GetHDCPCap(u8Source);

        case MTK_TEST_GET_MST_STATUS:
            return 0;//GetMST(u8Source);

        case MTK_TEST_SWITCH_PORT:
            return SwitchPort();

        case MTK_TEST_SWITCH_POWER_DC:
            if (!DDCBuffer[3] && PowerOnFlag)
            {
                PowerOffSystem();
            }
            else if (DDCBuffer[3] && !PowerOnFlag)
            {
                PowerOnSystem();
            }
            return 0;

        case MTK_TEST_SWITCH_HDCP_CAP:
            return 0;//SetHDCPCap(u8Source);

        case MTK_TEST_SWITCH_MST:
            return 0;//SetMST(u8Source);

        case MTK_TEST_TIMING_CHANGE:
            {
                Set_InputTimingChangeFlag();
            }
            return 0;

        case MTK_TEST_GET_FLAG_POWER_SAVING:
            DDCBuffer[0] = 0x83;
            DDCBuffer[3] = 1;
            return 3;

        case MTK_TEST_SET_FLAG_POWER_SAVING:
            //Set_g_u8MenuTempValue(DDCBuffer[3]);
            //mAPI_MenuFunc_SetPowerSaveEn();
            return 0;

        case MTK_TEST_GET_FLAG_PORT_SCAN:
            DDCBuffer[0] = 0x83;
            DDCBuffer[3] = (Input_Priority_Auto == UserPrefInputPriorityType);
            return 3;

        case MTK_TEST_SET_FLAG_PORT_SCAN:
            SetAutoPort(DDCBuffer[3]);
            return 0;

        case MTK_TEST_GET_FLAG_AUDIO_MUTE:
            DDCBuffer[0] = 0x83;
            DDCBuffer[3] = UserPrefVolume <= MinVolume;
            return 3;

        case MTK_TEST_SET_FLAG_AUDIO_MUTE:
            UserPrefVolume = DDCBuffer[3] ? 0 : (MinVolume + MaxVolume) / 2;
            msAPI_AdjustVolume(UserPrefVolume);
            return 0;
    }
    return 0;
}
#endif
