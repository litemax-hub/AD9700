
#include "types.h"
#include "board.h"
#include "Global.h"
#include "Detect.h"
#include "msACE.h"
#include "mdrv_hdmiRx.h"
#include "mapi_DPRx.h"
#include "menufunc.h"
#include "CustomEDID.h"

#define CUSTOM_CB_DEBUG    1
#if ENABLE_MSTV_UART_DEBUG && CUSTOM_CB_DEBUG
#define CUSTOM_CB_printData(str, value)   printData(str, value)
#define CUSTOM_CB_printMsg(str)           printMsg(str)
#else
#define CUSTOM_CB_printData(str, value)
#define CUSTOM_CB_printMsg(str)
#endif

/// @brief [Custom Cb function] Init IP related customize option like EDID/DP version/... 
/// @param  
/// @return
void CustomCb_CustomizeIPOption_Init(void)
{
/*
Please don't add every customize init setting in this function
There's mStar_SetUserPref function can be used
*/
    BYTE u8InputPort = 0;

    CUSTOM_CB_printMsg("CustomCb_CustomizeIPOption_Init");

    #if ENABLE_HDMI && ENABLE_MSTV_UART_DEBUG
        #if (CInput_HDMI_C1 != CInput_Nothing)
        if(hwHDCP_Hpd_Pin())
        {
            printMsg("<WARNING> HDMI 1 HPD pull high before init done. Please check & reduce init functions !!! \r\n");
        }
        #endif

        #if (CInput_HDMI_C2 != CInput_Nothing)
        if(hwHDCP_Hpd2_Pin())
        {
            printMsg("<WARNING> HDMI 2 HPD pull high before init done. Please check & reduce init functions !!! \r\n");
        }
        #endif
        
        #if (CInput_HDMI_C3 != CInput_Nothing)
        if(hwHDCP_Hpd3_Pin())
        {
            printMsg("<WARNING> HDMI 3 HPD pull high before init done. Please check & reduce init functions !!! \r\n");
        }
        #endif

    #endif 

    
    for(u8InputPort = 0; u8InputPort < Input_Nums; u8InputPort++)
    {
        #if ENABLE_FREESYNC
        //Load HDMI EDID part move to CustomEDID_InitInternalEDID
        #if ENABLE_DP_INPUT
        if(INPUT_IS_DISPLAYPORT(u8InputPort))
        {
             mapi_DPRx_EnableDRRFunction(u8InputPort, UserprefFreeSyncMode);
        }
        #endif
        #endif
    }
    
#if ENABLE_DP_INPUT
    //Keep DPCD part of SetDPVersion, EDID part move to CustomEDID_InitInternalEDID
    BYTE ucVersion = 0x00, DP_port;

    if (UserPrefDPVersion == DPVersionMenuItems_DP1_1)
        ucVersion = DP_VERSION_11;
    else if (UserPrefDPVersion == DPVersionMenuItems_DP1_2)
        ucVersion = DP_VERSION_12;
    else if (UserPrefDPVersion == DPVersionMenuItems_DP1_4)
        ucVersion = DP_VERSION_14;

    for(DP_port = Input_Digital; DP_port < Input_Nums; DP_port++)
    {
        if((ENABLED_DISPLAYPORT>>DP_port)&BIT0)
        {
            mapi_DPRx_VersionSetting(DP_port, ucVersion);
			
			#if (ENABLE_DP_RX_MAX_LINKRATE_HBR2 == 0x1)
			if((ucVersion == DP_VERSION_14)||(ucVersion == DP_VERSION_12))
			{
				/*
				DP_LINKRATE_RBR = 0x6,
				DP_LINKRATE_HBR = 0xA,
				DP_LINKRATE_HBR2 = 0x14,
				DP_LINKRATE_HBR25 = 0x19,
				DP_LINKRATE_HBR3 = 0x1E
				*/
				mapi_DPRx_MaxLinkRate_Set(DP_port, 0x14);  //Note: Change Max link rate need after the API "mapi_DPRx_VersionSetting"
			}
			#endif
        }
    }
#endif
    CustomEDID_InitInternalEDID();
}

/// @brief [Custom Cb function] Setting customize FBL condition
/// @param
/// @return True: System prefer FBL, False: System prefer FB
Bool CustomCb_CustomizeFblCondition(void)
{
    Bool SetFBL = FRAME_BFF_SEL;

    #if ((CHIP_ID == CHIP_MT9701) && (PanelType == PanelM270HAN03_0_eDP))
    if( (!msAPI_combo_IPGetDDRFlag()) &&
        ((SC0_READ_AUTO_WIDTH()>=1920) && (SC0_READ_AUTO_HEIGHT()>=1080) && (SrcVFreq>=1200))
    )
    {
        SetFBL = FRAME_BFFLESS;
    }
    #endif

    CUSTOM_CB_printData("CustomCb_CustomizeFblCondition: %d\n", SetFBL);
    
    return SetFBL;
}

/// @brief [Custom Cb function] Setting customize Fake sleep condition
/// @param  
/// @return TRUE: Enter Fake Sleep Mode, FALSE: Don't Enter Fake Sleep Mode
Bool CustomCb_FakeSleepCondition(void)
{
#if DISABLE_DPMS
    return TRUE;
#elif DISABLE_DP_FAKESLEEP
    return FALSE;
#else
    Bool bResult = FALSE;
    Bool bFakeSleepTimeOutSleepEn = TRUE;

//Fake Sleep Condition1:
 #if ENABLE_DP_INPUT
    BYTE DP_port=0;

    if((UserPrefInputPriorityType == Input_Priority_Auto) && (!UserPrefMSTOnOff))    //auto port and sst mode
    {
        for(DP_port = Input_Digital; DP_port < Input_Nums; DP_port++)
        {
            if((ENABLED_DISPLAYPORT >> DP_port) & BIT0)
            {
                if(mapi_DPRx_IsSystemGoRealSleep(DP_port) == FALSE)
                {
                    CUSTOM_CB_printData("fake sleep: auto port 0x%x", DP_port);
                    bResult = TRUE;
                    break;
                }
            }
        }
    }
    else if(CURRENT_INPUT_IS_DISPLAYPORT()) //fix port
    {
        if(mapi_DPRx_IsSystemGoRealSleep(SrcInputType) == FALSE)
        {
            CUSTOM_CB_printData("fake sleep: fix port 0x%x", DP_port);
            bResult = TRUE;
        }
    }

    //Set Fake Sleep Condition 1 Time Out Enable
    if(bResult)
    {
        bFakeSleepTimeOutSleepEn &= FALSE;
    }
#endif

//Fake Sleep Condition2:
    if(g_eBootStatus == eBOOT_STATUS_CHECKAGAINON)
    {
        CUSTOM_CB_printMsg("fake sleep, check again to fake sleep");
        bResult = TRUE;
    }

    //Set Fake Sleep Condition 2 Time Out Enable
    if(bResult)
    {
        bFakeSleepTimeOutSleepEn &= TRUE;
    }

//Fake Sleep Time Out En Setting
    msAPI_FakeSleepTimeOutEn_Set(bFakeSleepTimeOutSleepEn);
    
//Fake Sleep Time Out Setting
    if((msAPI_FakeSleepTimeOutEn_Get() == TRUE) && (bResult == TRUE) && (msAPI_FakeSleepTimeOutCnt_Get() == -1))
    {
        CUSTOM_CB_printMsg("Start to count Down fake sleep time out \n");
        msAPI_FakeSleepTimeOutCnt_Set(msAPI_FakeSleepTimeOutThd_Get());
    }
    else if(msAPI_FakeSleepTimeOutCnt_Get() == 0)
    {
        CUSTOM_CB_printMsg("fake sleep time out, enter real sleep");
        bResult = FALSE;
    }

    return bResult;
}

/// @brief [Custom Cb function] Setting customize window color condition
/// @param  
/// @return E_WIN_COLOR_FORMAT
BYTE CustomCb_WinColorCondition(void)
{
    BYTE u8Result = WIN_COLOR_RGB;

    if(IsColorspaceRGBInput())
    {
        u8Result = WIN_COLOR_RGB;
    }
    else
    {
        u8Result = WIN_COLOR_YUV;
    }

    if(u8Result >= WIN_COLOR_MAX)
    {
        CUSTOM_CB_printMsg("Win color setting error, reset to RGB\n");
        u8Result = WIN_COLOR_RGB;
    }

    return u8Result;
#endif
}

/// @brief This function is used to attach callback functions to the specified appmStar callback index.
/// @param  
void CustomCb_appmStar_Init(void)
{
    BYTE u8Idx;

    for( u8Idx = 0 ; u8Idx < eCB_appmStar_CBMaxNum ; u8Idx++ )
    {
        appmStar_CbAttach(u8Idx, NULL);
    }

    #if CUSTOMIZE_IP_OPTION
    appmStar_CbAttach(eCB_appmStar_CustomizeIPOptionInit, CustomCb_CustomizeIPOption_Init);
    #endif

    appmStar_CbAttach(eCB_appmStar_CustomizeFakeSleepCondition, CustomCb_FakeSleepCondition);
    #if (CHIP_ID == CHIP_MT9701)
    appmStar_CbAttach(eCB_appmStar_CustomizeWinColorCondition, CustomCb_WinColorCondition);
    #endif

    appmStar_CbSetInitFlag(TRUE);
}

/// @brief custom call back function attachment
/// @param  
void CustomCb_CbFunnction_Init(void)
{
    #if (CHIP_ID == CHIP_MT9701)
    mStar_FblCBAttach(CustomCb_CustomizeFblCondition);// FBL/FB can only be slected in MT9701
    #endif

    CustomCb_appmStar_Init();
}
