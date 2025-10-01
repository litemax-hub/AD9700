#define _ComboAPP_C_
#if 1
#include <math.h>
#include "types.h"
#include "board.h"
#include "ms_reg.h"
#include "Global.h"
#include "Mcu.h"
//#include "panel.h"
#include "Adjust.h"
//#include "msADC.h"
#include "Ms_rwreg.h"
#include "Debug.h"
#include "Common.h"
#include "misc.h"
#include "DDC.h"
#include "Power.h"
#include "Detect.h"
#include "msOSD.h"
#include "Gamma.h"
//#include "extlib.h"
#include "Menu.h"
#include "menudef.h"
#include "menufunc.h"
#include "msflash.h"
//#include "msid_v1.h"
#include "drvGPIO.h"
#include "drv_adctbl.h"
#include "drvADC.h"
#include "msEread.h"
#if ENABLE_RTE
#include "msOVD.h"
#endif
//#if ENABLE_FREESYNC
//#include "drvDPRxApp.h"
//#endif
#if (MS_PM)
#include "Ms_PM.h"
#endif
#include "MsDLC.h"
#include "msACE.h"
#include "UserPref.h"
#include "Panel.h"
#if ENABLE_3DLUT
#include "drv3DLUT_C.h"
#endif

#include "mapi_DPRx.h"
#include "HdcpMbx.h"
#include "HdcpHandler.h"
#include "drv_Hdcp_IMI.h"
#include "mdrv_hdmiRx.h"
#include "drvIMI.h"
#include "HdcpIMI.h"
#include "HdcpCommon.h"
#include "mdrv_hdmiRx.h"
#include "apiCEC.h"
#include "ComboApp.h"
#include "drvmStar.h"
#include "mailbox.h"
#include <string.h>
#include "mdrv_hdmiRx.h"


#if((HDCP_KEY_22_RX_1ST_ADDR==0xF00000) || (HDCP_KEY_22_RX_2ND_ADDR==0xF10000))
    #warning "<<< ENABLE_HDCP22_RX_KEY_STORAGE - Key Address using default value >>>"
#endif

#define  Combo_DEBUG  1

#if (Combo_DEBUG&&ENABLE_DEBUG)
#define Combo_printData(str, value)   printData(str, value)
#define Combo_printMsg(str)           printMsg(str)
#else
#define Combo_printData(str, value)
#define Combo_printMsg(str)
#endif

#if(COMBO_HDCP2_FUNCTION_SUPPORT)
static WORD gu16SecR2TimeoutCnt = 0;
//static Bool g_bUseCustomerAESKey = FALSE;
//static Bool bHdcp1xTxKeyInit = FALSE;
//static Bool bHdcp22TxKeyInit = FALSE;
BYTE *u8HDCP_KEY = (BYTE*)IMIGetAddr(DEF_IMI_HDCP14TxKey_OFFSETADDR);
static Bool bHDCP22InitProcDone = FALSE;
static MS_BOOL bSecRomCodeDone = FALSE;
BYTE code ucKeyEncodeString[COMBO_HDCP2_ENCODE_KEY_CHECK_SIZE] =
{
    0x4D, 0x53, 0x54, 0x41, 0x52, 0x5F, 0x53, 0x45, 0x43, 0x55, 0x52, 0x45, 0x5F, 0x53, 0x54, 0x4F, 0x52, 0x45, 0x5F, 0x46, 0x49, 0x4C, 0x45, 0x5F, 0x4D, 0x41, 0x47, 0x49, 0x43, 0x5F, 0x49, 0x44
};
#endif
static Bool bSendCmdFlag = FALSE;
static MS_BOOL bSecuStoreDone = FALSE;
BYTE *pHDCPTxData_CertRx;
BYTE XDATA glLastInputPort = 0xFF;
BOOL XDATA bHDCP2RxKeyDecodeDone;
BOOL XDATA bHDCP2TxKeyDecodeDone;
BOOL XDATA bHDCP1TxKeyDecodeDone;
BOOL bHDCP1RxKeyDecodeDone = FALSE;//marked for MT9700 to reload key after entering pm mode and then waking up system.
BYTE *tCOMBO_HDCP2_CERTRX_TABLE = (BYTE*)IMIGetAddr(DEF_IMI_CERT_OFFSETADDR); //for SecureStorm



extern BOOL xdata glbIPRxInitHPD_DONE_FLAG;
extern BYTE XDATA glSYS_TypeC_PinAssign[2];

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
Bool msAPI_combo_FetchSecureStormResult(Bool bIsRxKey, BYTE* pu8DataBuf, WORD u16BufSize, WORD* pu16RetSize);

#if(COMBO_HDCP2_FUNCTION_SUPPORT)
static Bool bHDCP22TimerClr = FALSE;

//**************************************************************************
//  [Function Name]:
//                  mapi_combo_HDCP2Initial()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void mapi_combo_HDCP2Initial(void)
{
    mdrv_combo_HDCP2Initial(); //declare hdcp2.2 capability

    WORD u16InputPortTypeMap = 0;
	WORD u16OutputPortTypeMap = 0;

	//for(ucComboSelect = (EN_COMBO_IP_SELECT)COMBO_IP_SUPPORT_0PORT; ucComboSelect < (EN_COMBO_IP_SELECT)COMBO_IP_SUPPORT_TYPE; ucComboSelect++)
	{
		u16InputPortTypeMap = ((u16InputPortTypeMap << 1) | BIT0);
	}
	//for(ucComboSelect = (EN_COMBO_IP_SELECT)COMBO_OP_SUPPORT_0PORT; ucComboSelect < (EN_COMBO_IP_SELECT)COMBO_OP_SUPPORT_TYPE; ucComboSelect++)
	{
		u16OutputPortTypeMap = ((u16OutputPortTypeMap << 1) | BIT0);
	}
    HDCPHandler_Init(u16InputPortTypeMap, u16OutputPortTypeMap, FALSE);
    #if(COMBO_HDCP2_DECODE_KEY)
    //HDCPHandler_DecodeRxKey(tCOMBO_HDCP2_CERTRX_TABLE);
    #endif
    #if 1
    //mdrv_combo_SetHDCP2CallBackFunction(0, mdrv_tmds_HDCP2TxEventProc);
	//mdrv_combo_SetHDCP2CallBackFunction(1, mapi_DPRx_HDCP2TxEventProc);
    #else
    for(ucComboSelect = COMBO_IP_SELECT_PORT0; ucComboSelect < COMBO_IP_SELECT_MASK; ucComboSelect++)
    {
        ucInputPort = mapi_combo_GetComboInputPort(ucComboSelect);
        ucComboIPType = mapi_combo_GetComboType(ucComboSelect);

        switch(ucComboIPType)
        {
            case COMBO_IP_TYPE_DVI:
            case COMBO_IP_TYPE_DUAL:
            case COMBO_IP_TYPE_HDMI:
            case COMBO_IP_TYPE_MHL:
				#if((CHIP_ID == MST9U3) || (CHIP_ID == MST9U4))
				{
					mdrv_combo_SetHDCP2CallBackFunction(ucInputPort, mapi_tmds_HDCP2TxEventProc);
				}
				#elif((CHIP_ID != MST9U) && (CHIP_ID != MST9U2))  //CHIP after MST9U5
				{
					mdrv_combo_SetHDCP2CallBackFunction(ucInputPort, mapi_tmds_HDCP2TxEventProc);   //T.B.D: need modify callback func return type
				}
				#endif

                #if(DEF_COMBO_HDCP2RX_ISR_MODE == 1)
					#if(CHIP_ID == MST9U3)
					{
						drv_HDCPRx_AttachFetchMsgCBFunc(ucInputPort, mapi_tmds_HDCP2FetchMsg);
					}
					#elif(CHIP_ID == MST9U4)
					{
						HDCPHandler_AttachFetchMsgCBFunc(ucInputPort, mapi_tmds_HDCP2FetchMsg);
					}
					#endif
                #endif
                break;

            case COMBO_IP_TYPE_DP:
				#if((CHIP_ID == MST9U3) || (CHIP_ID == MST9U4))
				{
					mdrv_combo_SetHDCP2CallBackFunction(ucInputPort, mapi_DP_HDCP2TxEventProc);
				}
				#elif((CHIP_ID != MST9U) && (CHIP_ID != MST9U2)) //CHIP after MST9U5
				{
					mdrv_combo_SetHDCP2CallBackFunction(ucInputPort, mapi_DPRx_HDCP2TxEventProc);
					mdrv_combo_SetHDCPRxWriteX74Function(ucInputPort, mapi_DPRx_HDCP14MapDPCDWrite);
					mdrv_combo_SetHDCPRepKSVFIFOFunction(ucInputPort, mapi_DPRx_HDCP14MapDPCDKSVList);
					#if (ENABLE_DPTX_OUTPUT)
					mdrv_combo_SetHDCPRxReadX74Function(ucInputPort, mapi_DPTx_HDCP14MapDPCDRead);
					#endif

				}
				#endif

                #if(DEF_COMBO_HDCP2RX_ISR_MODE == 1)
					#if(CHIP_ID == MST9U3)
					{
						drv_HDCPRx_AttachFetchMsgCBFunc(ucInputPort, mapi_DP_HDCP2FetchMsg);
					}
					#elif(CHIP_ID == MST9U4)
					{
						HDCPHandler_AttachFetchMsgCBFunc(ucInputPort, mapi_DP_HDCP2FetchMsg);
					}
					#endif
                #endif
                break;

            default:

                break;
        }
    }
    #endif
}
#if 0
Bool msAPI_combo_ProcessHdcp1TxKey(void)
{
    printf("====bHdcp1xTxKeyInit======\n");//[MT9700]
    if (bHDCP22TimerClr == FALSE)
    {
        bHDCP22TimerClr = TRUE;
        gu16SecR2TimeoutCnt = 0;
    }

    if (gu16SecR2TimeoutCnt <= COMBO_HDCP2_SECUR2INT_WDT)
    {
        if(mdrv_combo_HDCP2RomCodeDone())
        {
            bSecRomCodeDone = TRUE;
        }

        if  (bSendCmdFlag == FALSE && ((bHDCP22TimerClr == TRUE) &&  bSecRomCodeDone)) //only do once; trigger decode TX key command
        {
            bSendCmdFlag = TRUE;
            HDCPMBX_Init();
            //HDCPHandler_DecodeHdcp1TxKey(tCOMBO_HDCP2_CERTRX_TABLE);
            HDCPHandler_DecodeHdcp1TxKey(u8HDCP_KEY);
        }

        if (bSecRomCodeDone == TRUE)
        {
            bHdcp1xTxKeyInit = HDCPHandler_GetR2EventFlag();
        }
    }
    else
    {
        bHdcp1xTxKeyInit = TRUE;
        printf("**Combo decode hdcp1x Tx key timeout\r\n");
    }

    bHDCP22TimerClr = (bHdcp1xTxKeyInit) ? FALSE : TRUE; //reset timer;
    printf("====bHdcp1xTxKeyInit=== is %d\n",bHdcp1xTxKeyInit);//[MT9700]
    return bHdcp1xTxKeyInit;
}

Bool msAPI_combo_ProcessHdcp2TxKey(BOOL bUseCustomizeKey)
{
    if (bHDCP22TimerClr == FALSE)
    {
        bHDCP22TimerClr = TRUE;
        bSendCmdFlag = FALSE;
        gu16SecR2TimeoutCnt = 0;
    }

    if (gu16SecR2TimeoutCnt <= COMBO_HDCP2_SECUR2INT_WDT)
    {
        if  (bSendCmdFlag == FALSE && ((bHDCP22TimerClr == TRUE) &&  bHdcp1xTxKeyInit)) //only do once; trigger decode TX key command
        {
            bSendCmdFlag = TRUE;
            HDCPHandler_DecodeHdcp2TxKey(tCOMBO_HDCP2_CERTRX_TABLE, bUseCustomizeKey);
        }

        if (bSecRomCodeDone == TRUE)
        {
            bHdcp22TxKeyInit = HDCPHandler_GetR2EventFlag();
        }
    }
    else
    {
        bHdcp22TxKeyInit = TRUE;
        COMBO_DRV_DPUTSTR("**Combo decode Tx key timeout\r\n");
    }

    bHDCP22TimerClr = (bHdcp22TxKeyInit) ? FALSE : TRUE; //reset timer;
    return bHdcp22TxKeyInit;
}

#endif

#if(COMBO_HDCP2_INITPROC_NEW_MODE)
void msAPI_HDCP2_reset_static_variable(void)
{
    bSendCmdFlag = FALSE;
#if(COMBO_HDCP2_FUNCTION_SUPPORT)
    bHDCP22TimerClr = FALSE;
#endif
#if(COMBO_HDCP2_FUNCTION_SUPPORT)
    gu16SecR2TimeoutCnt = 0;
    bHDCP22InitProcDone=FALSE;
#endif
    bSecuStoreDone = FALSE;
    msWriteByte(0x10054C, msReadByte(0x10054c)&(~BIT1));

    HDCPHandler_ClrRomR2FirstEventFlag();
    mdrv_combo_HDCP2ResetRomCodeflag();
}

//**************************************************************************
//  [Function Name]:
//                  mapi_combo_HDCP2Initial()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
ENUM_HDCP_KEY_INIT_STATUS msAPI_combo_HDCP2InitHandler(BOOL bUseCustomizeKey)
{
    Bool bFlag;
    ENUM_HDCP_KEY_INIT_STATUS enInitStatus = EN_HDCP_KEY_INIT_STATUS_NONE;

    bFlag = bUseCustomizeKey;

    if (bHDCP22TimerClr == FALSE)
    {
        bSendCmdFlag = FALSE;
        bHDCP22TimerClr = TRUE;
        gu16SecR2TimeoutCnt = 0;
    }

    if (gu16SecR2TimeoutCnt <= COMBO_HDCP2_SECUR2INT_WDT)
    {
        if(mdrv_combo_HDCP2RomCodeDone())
        {

			#if (COMBO_HDCPTX_BLOCK_ENABLE == 0)
			HDCPMBX_Init();
			#endif
            if (bSendCmdFlag == FALSE)
            {
                bSecRomCodeDone = TRUE;
                mapi_combo_HDCP2Initial();
                if (HDCPHandler_DecodeHdcp2RxKey(tCOMBO_HDCP2_CERTRX_TABLE, bUseCustomizeKey) == EN_HDCP_ERR_NONE)
                {
                    bSendCmdFlag = TRUE;
                }
            }
        }

        #if(COMBO_HDCP2_DECODE_KEY)
        if(mdrv_combo_HDCP2KeyDecodeDone() && bSecRomCodeDone)
        {
            WORD i = 0;
            for ( i = 0; i < sizeof(tCOMBO_HDCP2_CERTRX_TABLE); i++ ) //update from DRAM
            {
                tCOMBO_HDCP2_CERTRX_TABLE[i] = msMemReadByte((ULONG)tCOMBO_HDCP2_CERTRX_TABLE + i);
                #if 0
                printf("0x%02X ", tCOMBO_HDCP2_CERTRX_TABLE[i]);
                if ((i+1) % 16 == 0)
                    printf("\r\n");
                #endif
            }

            bHDCP22InitProcDone = TRUE;
            bSecuStoreDone = TRUE;
            enInitStatus = EN_HDCP_KEY_INIT_STATUS_PASS;
        }
        if(bSecuStoreDone == TRUE)
        {
                mdrv_tmds_SetHDCP2CallBackFunction(mdrv_combo_HDCP2RxEventProc, (BYTE*)((ULONG)tCOMBO_HDCP2_CERTRX_TABLE + COMBO_HDCP2_ENCODE_KEY_SIZE));
                #if (ENABLE_DP_INPUT == 0x1)
				    mapi_DPRx_SetHDCP2CallBackFunction(mdrv_combo_HDCP2RxEventProc, (BYTE*)((ULONG)tCOMBO_HDCP2_CERTRX_TABLE + COMBO_HDCP2_ENCODE_KEY_SIZE));
                #endif
				//mapi_DPRx_SetHDCP2ConfigRepeaterCallBackFunction(mdrv_combo_ConfigRepeaterPair);
                #if ((COMBO_HDCPTX_BLOCK_ENABLE == 1)&&(COMBO_DPTX_BLOCK_ENABLE == 1))
					mapi_DPTx_InitHDCPCBFunc(HDCPHandler_EnableHdcpTxOnDP);
					mdrv_combo_SetHDCPTxIOCBFunction(ST_DPTX_OUTPUT_PORT0, mapi_DPTx_HDCPAuxCMD);
					mdrv_combo_SetHDCPTxCompareRiFunction(ST_DPTX_OUTPUT_PORT0, mapi_DPTx_HDCP14_CheckRi);
					mdrv_combo_SetHDCPTxGetDPTXR0Function(ST_DPTX_OUTPUT_PORT0, mapi_DPTx_HDCP14_GetDPTXR0);
					mapi_DPRx_HDCP14ChkVPrimeCallBackFunction(mdrv_combo_HDCP14CheckVPrimePrepared);
					mapi_DPRx_SetHDCP14AKSVReceivedCallBackFunction(mdrv_combo_HDCP14ReadBKSV);
                #endif
        }
        #endif //#if(COMBO_HDCP2_DECODE_KEY)
    }
    else
    {
        if (bHDCP22InitProcDone == FALSE)
        {
            bHDCP22InitProcDone = TRUE;
            enInitStatus = EN_HDCP_KEY_INIT_STATUS_FAIL;
            printf("Wait SECU R2 Init Process Timeout!!\r\n");
        }
    }

    return enInitStatus;
}

#endif //#if (COMBO_HDCP2_INITPROC_NEW_MODE)

//**************************************************************************
//  [Function Name]:
//                  mapi_combo_HDCP2Initial()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
MS_BOOL mapi_combo_HDCP2GetRecvID(BYTE *pucDataBuf)
{
    MS_BOOL bRet = FALSE;

    if ((bHDCP22InitProcDone == TRUE) && (pucDataBuf != NULL))
    {
        memcpy(pucDataBuf, tCOMBO_HDCP2_CERTRX_TABLE, sizeof(BYTE)*5);
        bRet = TRUE;
    }

    return bRet;
}

#endif //#if(COMBO_HDCP2_FUNCTION_SUPPORT)

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------


//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPGetColorFormat()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
ST_COMBO_COLOR_FORMAT msAPI_combo_IPGetColorFormat(BYTE ucInputPort)
{
#if ENABLE_HDMI
    ST_HDMI_RX_COLOR_FORMAT ucHDMIColorimetry;
#endif
    ST_COMBO_COLOR_FORMAT ucColorFormat = {COMBO_COLOR_FORMAT_DEFAULT, COMBO_COLOR_DEPTH_NONE, COMBO_COLOR_RANGE_DEFAULT, COMBO_COLORIMETRY_NONE, COMBO_YUV_COLORIMETRY_ITU601, COMBO_COLORIMETRY_Additional_NoData};

    if(INPUT_IS_DISPLAYPORT(ucInputPort)||INPUT_IS_USBTYPEC(ucInputPort))
    {
#if ENABLE_DP_INPUT
        ucColorFormat.ucColorType = mapi_DPRx_GetColorSpace(ucInputPort);
        ucColorFormat.ucColorDepth = mapi_DPRx_GetColorDepthInfo(ucInputPort);
        ucColorFormat.ucColorRange = mapi_DPRx_GetColorRange(ucInputPort);
        ucColorFormat.ucColorimetry = mapi_DPRx_GetColorimetry(ucInputPort);
        ucColorFormat.ucYuvColorimetry = mapi_DPRx_GetYuvColorimetry(ucInputPort);
#endif
    }
    else if(INPUT_IS_HDMI(ucInputPort) || INPUT_IS_DVI(ucInputPort))
    {
#if ENABLE_HDMI
        ucColorFormat.ucColorType = msAPI_combo_HDMIRx_GetVideoContentInfo(COMBO_VIDEO_COLOR_FROMAT, ucInputPort);
        ucColorFormat.ucColorDepth = msAPI_combo_HDMIRx_GetVideoContentInfo(COMBO_VIDEO_COLOR_DEPTH, ucInputPort);
        ucColorFormat.ucColorRange = msAPI_combo_HDMIRx_GetVideoContentInfo(COMBO_VIDEO_COLOR_RANGE, ucInputPort);
        ucHDMIColorimetry = mdrv_hdmiRx_GetColorimetry(ucInputPort);
        ucColorFormat.ucYuvColorimetry = ucHDMIColorimetry.ucColorimetry;
        ucColorFormat.ucColorimetry = ucHDMIColorimetry.ucExtColorimetry;
        ucColorFormat.ucExtAddiColoirty = ucHDMIColorimetry.ucExtAddColorimetry;
#endif
    }

    return ucColorFormat;
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_HDMIRx_IsAudioFmtPCM()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
EN_TMDS_AUDIO_FORMAT msAPI_combo_HDMIRx_IsAudioFmtPCM(BYTE ucPortIndex)
{
    return mdrv_hdmiRx_IsAudioFmtPCM(ucPortIndex);
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPGetColorFormat()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL msAPI_combo_IPGetHDRPacket(BYTE ucInputPort , BYTE *pHeader, BYTE *pPacketData)
{
    BOOL PactketRdy = FALSE;

    pHeader = pHeader;

    if(INPUT_IS_DISPLAYPORT(ucInputPort)||INPUT_IS_USBTYPEC(ucInputPort))
    {
#if ENABLE_DP_INPUT
        PactketRdy = mapi_DPRx_GetPacketData(ucInputPort, DP_HDR_PACKET, pPacketData, DP_SDP_PACKET_MAX_DATA_LENGTH); // T.B.D
#endif

    }
    else if(INPUT_IS_HDMI(ucInputPort))
    {
#if ENABLE_HDMI
        PactketRdy = mdrv_hdmiRx_GetPacketContent(PKT_HDR, 30, pPacketData, ucInputPort);
#endif
    }

    return PactketRdy;
}
//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPControlHPD()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL msAPI_combo_IPControlHPD(BYTE ucInputPort, Bool bSetHPD)
{
#if (ENABLE_HDMI || ENABLE_DVI)
    BYTE ComboPortIndex;
#endif
    #if (ENABLE_USB_TYPEC == 0x1)

    BYTE u8TypeCPort = 1;

    if(INPUT_IS_USBTYPEC(ucInputPort) &&
       !((glSYS_TypeC_PinAssign[u8TypeCPort] == 0x0)||(glSYS_CC_Pin[u8TypeCPort] == 0x2)) )
    {
        if(bSetHPD == 0x0)
        {
            drvmbx_send_HPD_Ctrl_CMD(SYSTEMIP_HPD_LOW, u8TypeCPort);
        }
        else
        {
            drvmbx_send_HPD_Ctrl_CMD(SYSTEMIP_HPD_HIGH, u8TypeCPort);
        }
    }
    #endif

    if(INPUT_IS_DISPLAYPORT(ucInputPort))
    {
#if ENABLE_DP_INPUT
        if(glbIPRxInitHPD_DONE_FLAG == TRUE)
        {
        	return mapi_DPRx_HPDControl(ucInputPort, bSetHPD);
        }
#endif
    }
    else if(INPUT_IS_HDMI(ucInputPort) || INPUT_IS_DVI(ucInputPort))
    {
#if (ENABLE_HDMI || ENABLE_DVI)
        ComboPortIndex = msDrvMapInputToCombo(ucInputPort);

        mdrv_hdmiRx_HPDControl(ComboPortIndex, bSetHPD);
#endif
    }

	return TRUE;
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_HDMIRx_ClockRtermControl()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAPI_combo_HDMIRx_ClockRtermControl(BYTE ucInputPort, Bool bPullHighFlag)
{
    BYTE ComboPortIndex;
    ComboPortIndex = msDrvMapInputToCombo(ucInputPort);

    if(INPUT_IS_HDMI(ucInputPort) || INPUT_IS_DVI(ucInputPort))
    {
        mdrv_hdmiRx_ClockRtermControl(ComboPortIndex, bPullHighFlag);
    }
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_HDMIRx_DataRtermControl()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAPI_combo_HDMIRx_DataRtermControl(BYTE ucInputPort, Bool bPullHighFlag)
{
    BYTE ComboPortIndex;
    ComboPortIndex = msDrvMapInputToCombo(ucInputPort);

    if(INPUT_IS_HDMI(ucInputPort) || INPUT_IS_DVI(ucInputPort))
    {
        mdrv_hdmiRx_DataRtermControl(ComboPortIndex, bPullHighFlag);
    }
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_HDMIRx_GetVideoContentInfo()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
WORD msAPI_combo_HDMIRx_GetVideoContentInfo(EN_COMBO_VIDEO_CONTENT_INFO enVideoContentInfo, BYTE ucPortSelect)
{
    WORD usReturnValue = 0;
#if (ENABLE_HDMI || ENABLE_DVI)
    if(INPUT_IS_HDMI(ucPortSelect) || INPUT_IS_DVI(ucPortSelect))
    {
        switch(enVideoContentInfo)
        {
            case COMBO_VIDEO_CONTENT_HDE:
                usReturnValue = mdrv_hdmiRx_GetVideoContentInfo(HDMI_VIDEO_CONTENT_HDE, ucPortSelect);
                break;

            case COMBO_VIDEO_CONTENT_VDE:
                usReturnValue = mdrv_hdmiRx_GetVideoContentInfo(HDMI_VIDEO_CONTENT_VDE, ucPortSelect);
                break;

            case COMBO_VIDEO_CONTENT_HTT:
                usReturnValue = mdrv_hdmiRx_GetVideoContentInfo(HDMI_VIDEO_CONTENT_HTT, ucPortSelect);
                break;

            case COMBO_VIDEO_CONTENT_VTT:
                usReturnValue = mdrv_hdmiRx_GetVideoContentInfo(HDMI_VIDEO_CONTENT_VTT, ucPortSelect);
                break;

            case COMBO_VIDEO_CLK_STABLE:
                usReturnValue = mdrv_hdmiRx_GetVideoContentInfo(HDMI_VIDEO_CLK_STABLE, ucPortSelect);
                break;

            case COMBO_VIDEO_CLK_COUNT:
                usReturnValue = mdrv_hdmiRx_GetVideoContentInfo(HDMI_VIDEO_CLK_COUNT, ucPortSelect);
                break;
            case COMBO_VIDEO_DE_STABLE:
                usReturnValue = mdrv_hdmiRx_GetVideoContentInfo(HDMI_VIDEO_DE_STABLE, ucPortSelect);
                break;

            case COMBO_VIDEO_COLOR_FROMAT:
                usReturnValue = mdrv_hdmiRx_GetVideoContentInfo(HDMI_VIDEO_COLOR_FROMAT, ucPortSelect);
                break;

            case COMBO_VIDEO_COLOR_DEPTH:
                usReturnValue = mdrv_hdmiRx_GetVideoContentInfo(HDMI_VIDEO_COLOR_DEPTH, ucPortSelect);
                break;

            case COMBO_VIDEO_COLOR_RANGE:
                usReturnValue = mdrv_hdmiRx_GetVideoContentInfo(HDMI_VIDEO_COLOR_RANGE, ucPortSelect);
                break;

            case COMBO_VIDEO_HDR_FLAG:
                usReturnValue = mdrv_hdmiRx_GetVideoContentInfo(HDMI_VIDEO_HDR_FLAG, ucPortSelect);
                break;

            case COMBO_VIDEO_FREE_SYNC_FLAG:
                usReturnValue = mdrv_hdmiRx_GetVideoContentInfo(HDMI_VIDEO_FREE_SYNC_FLAG, ucPortSelect);
                break;

            case COMBO_VIDEO_AVMUTE_FLAG:
                usReturnValue = mdrv_hdmiRx_GetVideoContentInfo(HDMI_VIDEO_AVMUTE_FLAG, ucPortSelect);
                break;

            case COMBO_VIDEO_VRR_FLAG:
                usReturnValue = mdrv_hdmiRx_GetVideoContentInfo(HDMI_VIDEO_VRR_FLAG, ucPortSelect);
                break;

            case COMBO_VIDEO_ALLM_FLAG:
                usReturnValue = mdrv_hdmiRx_GetVideoContentInfo(HDMI_VIDEO_ALLM_FLAG, ucPortSelect);
                break;
            case COMBO_VIDEO_HDMI_MODE_FLAG:
                usReturnValue =  mdrv_hdmiRx_GetVideoContentInfo(HDMI_VIDEO_HDMI_MODE_FLAG, ucPortSelect);
                break;
            default:
                break;
        }
    }
#else
    UNUSED(enVideoContentInfo);
    UNUSED(ucPortSelect);
#endif
    return usReturnValue;
}

#if 0
//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_HDCPInitialRebootValue()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAPI_combo_HDCPInitialRebootValue(void)
{
    bSecRomCodeDone = FALSE;
    bSecuStoreDone = FALSE;
    bSendCmdFlag = FALSE;
    bHDCP2RxKeyDecodeDone = FALSE;
    bHDCP2TxKeyDecodeDone = FALSE;
    bHDCP1TxKeyDecodeDone = FALSE;

    HDCPHandler_RebootInitValue();

    return;
}
#endif
//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_SECU_PowerDown()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAPI_combo_SECU_PowerDown(BOOL bSetPowerDown)
{

    HDCPHandler_SECU_PowerDown(bSetPowerDown);

    return;
}


//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPInitial()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAPI_combo_IPInitial(Bool bACon)
{
    Combo_printMsg("-Combo--IP Init---\r\n");
    glLastInputPort = 0xFF;
    Bool bHPDFlag = FALSE;

#if ENABLE_DP_INPUT
    mapi_DPRx_Initial();
#endif

#if (ENABLE_HDMI || ENABLE_DVI)
    bHPDFlag = mdrv_hdmiRx_Initial(!bACon);
#else
    UNUSED(bACon);
#endif

#if (ENABLE_LOAD_KEY_VIA_EFUSE)
    HDCPHandler_LoadKeyViaEfuse();
#endif

#ifndef TSUMR2_FPGA
    HDCPHandler_LoadHDCP14RxBksv(NULL, NULL, FALSE);

    if(bHDCP1RxKeyDecodeDone == FALSE)
    {
        HDCPHandler_LoadHDCP14RxKey(NULL, NULL, FALSE);
        Combo_printMsg("<<<TBD:HDCPHandler_LoadHDCP14RxKey>>\r\n");
        bHDCP1RxKeyDecodeDone = TRUE;
    }
#endif

    Combo_printMsg("<<<TBD:mdrv_hdmiRx_Initial>>\r\n");

    if(bHPDFlag)
    {
#if (ENABLE_HDMI || ENABLE_DVI)
        mdrv_hdmiRx_HPD_Resume();
#endif
    }

#if ENABLE_CEC
    api_CEC_init();
#endif

    glbIPRxInitHPD_DONE_FLAG = FALSE;

return;
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPPHYSetting()
//  [Description]
//                  msAPI_combo_IPPHYSetting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAPI_combo_IPPHYSetting(void)
{
    //Bool ucflag = FALSE;

    if(CURRENT_INPUT_IS_DISPLAYPORT())
    {

    }
    else if(CURRENT_INPUT_IS_HDMI())
    {

    }

    return;
}


//**************************************************************************
//  [Function Name]:
//                  msApi_combo_HDCP2Initial()
//  [Description]
//                  msApi_combo_HDCP2Initial
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msApi_combo_HDCP2Initial(void)
{
    WORD u16InputPortTypeMap = EN_HDCPMBX_PORT_TYPE_SINK;
    WORD u16OutputPortTypeMap = EN_HDCPMBX_PORT_TYPE_SOURCE;

    //HDCPHandler_Init(u16InputPortTypeMap, u16OutputPortTypeMap);
    HDCPHandler_Init(u16InputPortTypeMap, u16OutputPortTypeMap, FALSE);

    //memset(tCOMBO_HDCP2_CERTRX_TABLE, 0x0, (1 + DEF_CUSTOMIZE_KEY_SIZE + COMBO_HDCP2_ENCODE_KEY_SIZE + COMBO_HDCP2_AKE_CERTRX_SIZE));

}


#if 0
//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_InitHandler()
//  [Description]
//                  msAPI_combo_InitHandler
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL msAPI_combo_InitHandler(BOOL bUseCustomizeKey)
{
    BYTE uncall = 0;
    //BOOL bRet = FALSE;

#if ENABLE_SECU_R2
    if(!bSecuStoreDone)
    {
        if(HDCPHandler_RomCodeDone())
        {

            if(!bSendCmdFlag)
            {

                HDCPMBX_Init();
                msApi_combo_HDCP2Initial();

                #if (ENABLE_HDCP22)
                {
                    HDCPHandler_LoadHDCP22RxKey(tCOMBO_HDCP2_CERTRX_TABLE);
                    if (HDCPHandler_DecodeHdcp2RxKey(((ULONG)DEF_IMI_CERT_OFFSETADDR), bUseCustomizeKey) == EN_HDCP_ERR_NONE)
                    {
                        bSendCmdFlag = TRUE;
                    }
                }
                #else
                {
                    HDCPHandler_LoadHDCP14TxKey(NULL, FALSE);
                    if(HDCPHandler_DecodeHdcp1TxKey(DEF_IMI_HDCP14TxKey_OFFSETADDR) == EN_HDCP_ERR_NONE)
                    {
                        bSendCmdFlag = TRUE;
                        bHDCP1TxKeyDecodeDone = TRUE;
                    }
                }
                #endif

            }
            bSecRomCodeDone = TRUE;
        }


        if(HDCPHandler_KeyDecodeDone() && bSecRomCodeDone)
        {
            bSecuStoreDone = TRUE;

            #if (ENABLE_HDCP22)
            {
                bHDCP2RxKeyDecodeDone = TRUE;
                mcu4kXdataMapToDRAMIMI(DEF_HK51_IMI_BANK0_SETTING, TRUE);
                mapi_DPRx_HDCP2SetCertRx((BYTE*)((ULONG)(DEF_HK_IMI_BASE_ADDR+DEF_IMI_CERT_OFFSETADDR + COMBO_HDCP2_ENCODE_KEY_SIZE)));
            }
            #endif
            return TRUE;
        }
    }
#endif

    if(uncall)
    {
        bUseCustomizeKey= 0;
    }

    return FALSE;
}


BOOL msAPI_combo_ProcessHdcp2TxKey(void)
{
    if(bHDCP2TxKeyDecodeDone == FALSE)
    {
        msAPI_combo_Hdcp2InsertKeyWithCutomizeKey(FALSE, NULL, FALSE, FALSE, NULL);
        //IMI_WriteByte(DEF_IMI_CERT_OFFSETADDR + COMBO_HDCP2_ENCODE_TX_KEY_SIZE, 0x0);
        HDCPHandler_LoadHDCP22TxKey(tCOMBO_HDCP2_CERTRX_TABLE);
        if (HDCPHandler_DecodeHdcp2TxKey(DEF_IMI_CERT_OFFSETADDR, FALSE) == EN_HDCP_ERR_NONE)
        {
            bHDCP2TxKeyDecodeDone = TRUE;
            //printMsg("**HDCPHandler_DecodeHdcp2TxKey\r\n");
        }
    }

    if(HDCPHandler_KeyDecodeDone() && bSecRomCodeDone)
    {
        return TRUE;
    }
    return FALSE;
}

BOOL msAPI_combo_ProcessHdcp1TxKey(void)
{
    if(bHDCP1TxKeyDecodeDone == FALSE)
    {
        HDCPHandler_LoadHDCP14TxKey(NULL, FALSE);
        if(HDCPHandler_DecodeHdcp1TxKey(DEF_IMI_HDCP14TxKey_OFFSETADDR) == EN_HDCP_ERR_NONE)
        {
            bHDCP1TxKeyDecodeDone = TRUE;
        }
    }

    if(HDCPHandler_KeyDecodeDone() && bSecRomCodeDone)
    {
        return TRUE;
    }

    return FALSE;
}
#endif
BOOL msAPI_combo_CheckSecuStoreDone(void)
{
#if (ENABLE_HDCP22)
    if((bSecuStoreDone) && (bHDCP2RxKeyDecodeDone) && (bHDCP2TxKeyDecodeDone) && (bHDCP1TxKeyDecodeDone))
    {
        Combo_printMsg("**Secu Store done done done\r\n");
        return TRUE;
    }
    else
    {
        Combo_printMsg("\033[1;31m SECU STOR FAIL!!!\033[m\r\n");
        return FALSE;
    }
#else
    if((bSecuStoreDone) && (bHDCP1TxKeyDecodeDone))
    {
        Combo_printMsg("**Secu Store done done done\r\n");
        return TRUE;
    }
    else
    {
        Combo_printMsg("\033[1;31m SECU STOR FAIL!!!\033[m\r\n");
        return FALSE;
    }
#endif
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPPHYSetting()
//  [Description]
//                  msAPI_combo_IPPHYSetting
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAPI_combo_Handler(void)
{
#if ((COMBO_HDCP2_FUNCTION_SUPPORT) && (!COMBO_HDCP2_INITPROC_NEW_MODE))
    #if (COMBO_HDCP2_DECODE_KEY)
		#if (COMBO_HDCPTX_BLOCK_ENABLE == 1)
		if (bHdcp22TxKeyInit == TRUE)
		#endif
		{
			if ((bHDCP22TimerClr == FALSE) && (bSendCmdFlag == FALSE))
			{
				gu16SecR2TimeoutCnt = 0;
				bHDCP22TimerClr = TRUE;
				if (HDCPHandler_DecodeHdcp2RxKey(tCOMBO_HDCP2_CERTRX_TABLE, g_bUseCustomerAESKey) == EN_HDCP_ERR_NONE)
					bSendCmdFlag = TRUE;
			}
		}
    #endif //(COMBO_HDCP2_DECODE_KEY)
    if ((gu16SecR2TimeoutCnt <= COMBO_HDCP2_SECUR2INT_WDT) && (bSecuStoreDone != TRUE))
    {
        if(mdrv_combo_HDCP2RomCodeDone())
        {
            bSecRomCodeDone = TRUE;
            mapi_combo_HDCP2Initial();
        }
        #if(COMBO_HDCP2_DECODE_KEY)
        if(mdrv_combo_HDCP2KeyDecodeDone() && bSecRomCodeDone)
        {
            bHDCP22InitProcDone = TRUE;
            bSecuStoreDone = TRUE;
        }
        #endif
        #if(COMBO_HDCP2_DECODE_KEY)
        if(bSecuStoreDone == TRUE)
        {
				mapi_combo_HDCP2Initial();
                mdrv_tmds_SetHDCP2CallBackFunction(mdrv_combo_HDCP2RxEventProc, (BYTE*)((ULONG)tCOMBO_HDCP2_CERTRX_TABLE + COMBO_HDCP2_ENCODE_KEY_SIZE));
				mdrv_tmds_SetHDCP2CallBackFunction(mdrv_combo_HDCP2RxEventProc, (BYTE*)((ULONG)tCOMBO_HDCP2_CERTRX_TABLE + COMBO_HDCP2_ENCODE_KEY_SIZE));
				#if 0 //((COMBO_HDCPTX_BLOCK_ENABLE == 1)&&(COMBO_DPTX_BLOCK_ENABLE == 1))
					mapi_DPTx_InitHDCPCBFunc(HDCPHandler_EnableHdcpTxOnDP);
					mdrv_combo_SetHDCPTxIOCBFunction((BYTE)COMBO_IP_TYPE_DP, mapi_DPTx_HDCPAuxCMD);
				#endif
        }
        #endif
    }
    else
    {
        if (bHDCP22InitProcDone == FALSE)
        {
            bHDCP22InitProcDone = TRUE;
            printf("\033[1;36mWait SECU R2 Init Process Timeout!!\033[0m\r\n");
        }
    }
#endif //#if (COMBO_HDCP2_FUNCTION_SUPPORT) && (!COMBO_HDCP2_INITPROC_NEW_MODE)

#if (COMBO_HDCP2_FUNCTION_SUPPORT)
    if ((bSecRomCodeDone) && (bSecuStoreDone))
    {
        mdrv_combo_HDCP2Handler();
    }
#endif

#if (ENABLE_HDMI || ENABLE_DVI)
mdrv_hdmiRx_Handler();
#endif

#if ENABLE_CEC
     api_CEC_Handler();
#endif

    return;
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPTimerIsrHandler()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAPI_combo_IPTimerIsrHandler(void)
{
#if ENABLE_DP_INPUT
    mapi_DPRx_TimerHandler();
#endif

#if(COMBO_HDCP2_FUNCTION_SUPPORT)
        if (gu16SecR2TimeoutCnt <= COMBO_HDCP2_SECUR2INT_WDT)
            gu16SecR2TimeoutCnt++;
#endif

#if (ENABLE_HDMI || ENABLE_DVI)
    mdrv_hdmiRx_TimerInterrupt();
#endif

    return;
}

#if ENABLE_HDMI_BCHErrorIRQ
//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_HDMIRx_BCHInterrupt()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAPI_combo_HDMIRx_BCHInterrupt(void)
{
    mdrv_hdmiRx_BCHInterrupt();
}
#endif

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPGetDDRFlag_ISR()
//  [Description]
//                  msAPI_combo_IPGetDDRFlag_ISR
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool msAPI_combo_IPGetInterlace()
{
    Bool ucflag= FALSE;

    if(CURRENT_INPUT_IS_DISPLAYPORT())
    {
        #if ENABLE_DP_INPUT
        ucflag = mapi_DPRx_GetInterlaceFlag(SrcInputType);
        #endif
    }
    else if(CURRENT_INPUT_IS_HDMI())
    {

    }

    return ucflag;
}
//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_HDMIRx_DDCControl()
//  [Description]
//                  msAPI_combo_HDMIRx_DDCControl
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAPI_combo_HDMIRx_DDCControl(BYTE ucInputPort, Bool bEnable)
{
    mdrv_hdmiRx_DDCControl(ucInputPort, bEnable);
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_HDMIRx_SCDC_Field_Set()
//  [Description]
//                  write scdc data
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAPI_combo_HDMIRx_SCDC_Field_Set(BYTE ucInputPort, BYTE u8Offset, BYTE u8Size, BYTE *u8SetValue)
{
    BYTE ComboPortIndex = msDrvMapInputToCombo(ucInputPort);

    if(ComboPortIndex >= HDMI_INPUT_PORT_END)
    {
        Combo_printMsg("** ComboPortIndex >= HDMI_INPUT_PORT_END\r\n");
        return;
    }
    else
    {
        mdrv_hdmiRx_SetSCDCValue(ComboPortIndex, u8Offset, u8Size, u8SetValue);
    }
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_HDMIRx_EDIDLoad()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAPI_combo_HDMIRx_EDIDLoad(BYTE ucInputPort, BYTE *EdidData)
{
    BYTE ComboPortIndex = msDrvMapInputToCombo(ucInputPort);

    if(ComboPortIndex >= HDMI_INPUT_PORT_END)
    {
        Combo_printMsg("** ComboPortIndex >= HDMI_INPUT_PORT_END\r\n");
        return;
    }
    else
    {
        mdrv_HDMIRx_LoadEDID(ComboPortIndex, EdidData);
    }
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_HDMIRx_IRQ_Enable()
//  [Description]
//                  msAPI_combo_HDMIRx_IRQ_Enable
//  [Arguments]:
//                  bEnable : Enable phy irq.
//  [Return]:
//
//**************************************************************************
void msAPI_combo_HDMIRx_IRQ_Enable(EN_COMBO_HDMIRX_INT e_int, BYTE enInputPortSelect, MS_U16 bit_msk, Bool bEnableIRQ)
{
#if (CHIP_ID == CHIP_MT9701)
    BYTE ComboPortIndex = msDrvMapInputToCombo(enInputPortSelect);
    BYTE i = 0;
    if(ComboPortIndex >= HDMI_INPUT_PORT_END)
    {
        Combo_printMsg("** ComboPortIndex >= HDMI_INPUT_PORT_END\r\n");
        if((e_int == COMBO_HDMI_IRQ_PM_SCDC) && (bEnableIRQ == FALSE))
        {
            for(i = HDMI_INPUT_PORT0; i < HDMI_INPUT_PORT_END; i++)
            {
                mdrv_hdmiRx_IRQ_Enable(e_int, i, bit_msk, bEnableIRQ);
            }
        }
        return;
    }
    else
    {
        if((e_int == COMBO_HDMI_IRQ_PM_SCDC) && (bEnableIRQ == FALSE))
        {
            for(i = HDMI_INPUT_PORT0; i < HDMI_INPUT_PORT_END; i++)
            {
                mdrv_hdmiRx_IRQ_Enable(e_int, i, bit_msk, bEnableIRQ);
            }
        }
        else
        {
            mdrv_hdmiRx_IRQ_Enable(e_int, ComboPortIndex, bit_msk, bEnableIRQ);
        }
    }

#elif(CHIP_ID == CHIP_MT9700)
    UNUSED(e_int);
    UNUSED(enInputPortSelect);
    UNUSED(bit_msk);
    UNUSED(bEnableIRQ);
#endif
}

#if ENABLE_HPD_REPLACE_MODE
//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_HDMIRx_MAC_HDCP_Enable()
//  [Description]
//                  msAPI_combo_HDMIRx_MAC_HDCP_Enable
//  [Arguments]:
//                  bEnable : Enable Mac HDCP ack.
//  [Return]:
//
//**************************************************************************
void msAPI_combo_HDMIRx_MAC_HDCP_Enable(Bool bEnable)
{
    mdrv_hdmiRx_MAC_HDCP_Enable(bEnable);
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_HDMIRx_SCDC_Clr()
//  [Description]
//                  msAPI_combo_HDMIRx_SCDC_Clr
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAPI_combo_HDMIRx_SCDC_Clr(BYTE ucInputPort)
{
    mdrv_hdmiRx_SCDC_Clr(ucInputPort);
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_HDMIRx_SCDC_config()
//  [Description]
//                  msAPI_combo_HDMIRx_SCDC_config
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAPI_combo_HDMIRx_SCDC_config(BYTE enInputPortSelect, BYTE bDDC)
{
    mdrv_hdmiRx_SCDC_config(enInputPortSelect, bDDC);
}
#endif

#if ENABLE_HDMI_BCHErrorIRQ
//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_HDMIRx_EnableHDMIRx_PKT_ParsrIRQ()
//  [Description]
//                  msAPI_combo_HDMIRx_EnableHDMIRx_PKT_ParsrIRQ
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAPI_combo_HDMIRx_EnableHDMIRx_PKT_ParsrIRQ(Bool bEnable)
{
    mdrv_hdmiRx_EnableHDMIRx_PKT_ParsrIRQ(bEnable);
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_HDMIRx_IRQMaskEnable()
//  [Description]
//                  msAPI_combo_HDMIRx_IRQMaskEnable
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAPI_combo_HDMIRx_IRQMaskEnable(WORD enIRQType, Bool bEnable)
{
    mdrv_hdmiRx_IRQMaskEnable(enIRQType, bEnable);
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_HDMIRx_IRQClearStatus()
//  [Description]
//                  msAPI_combo_HDMIRx_IRQClearStatus
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAPI_combo_HDMIRx_IRQClearStatus(WORD enIRQType)
{
    mdrv_hdmiRx_IRQClearStatus(enIRQType);
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_HDMIRx_IRQCheckStatus()
//  [Description]
//                  msAPI_combo_HDMIRx_IRQCheckStatus
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool msAPI_combo_HDMIRx_IRQCheckStatus(WORD enIRQType)
{
    return mdrv_hdmiRx_IRQCheckStatus(enIRQType);
}
#endif

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPPowerControl()
//  [Description]
//                  msAPI_combo_IPPowerControl
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAPI_combo_IPPowerControl(BYTE ucInputPort)
{
	BYTE i = 0;
	BYTE ComboPortIndex = msDrvMapInputToCombo(ucInputPort);
	BYTE PHY_Id = 0;

	if(glLastInputPort == ucInputPort)
	{
		return; // No need to control the same port
	}
	else
	{
		glLastInputPort = ucInputPort;
	}

	if((ComboPortIndex == 2) || (ComboPortIndex == 3))
	{
		PHY_Id = 2;
	}
	else if(ComboPortIndex == 4) // Dual port
	{
		PHY_Id = 4;
	}
	else
	{
		PHY_Id = ComboPortIndex;
	}

	for(i = 0; i < 3; i++) // POWER OFF
	{
		if(i == PHY_Id)
		{
			if(ComboPortIndex == 2)
			{
    		    #if (ENABLE_HDMI || ENABLE_DVI)
				if((Input_HDMI_C4 != Input_Nothing) || (Input_DVI_C4 != Input_Nothing))
				{
					// Power off port 2B
					mdrv_hdmiRx_DataRtermControl(3, FALSE);
					mdrv_hdmiRx_ClockRtermControl(3, FALSE);
				}
			    #endif
			}
			else if(ComboPortIndex == 3)
			{
    		    #if (ENABLE_HDMI || ENABLE_DVI)
				if((Input_HDMI_C3 != Input_Nothing) || (Input_DVI_C3 != Input_Nothing))
				{
					// Power off port 2A
					mdrv_hdmiRx_DataRtermControl(2, FALSE);
					mdrv_hdmiRx_ClockRtermControl(2, FALSE);
				}
			    #endif
			}

			continue;
		}

		if((PHY_Id == 4) && ((i == 0x0) || (i == 0x1)))
		{
			continue;
		}

		Combo_printData("====Power OFF=== %d", i);

		msWriteByteMask(REG_DPRX_PHY_PM_02_L + (0x40 * i), BIT1, BIT1); // BK19_42[1]
		msWriteByteMask(REG_DPRX_PHY_PM_02_L + (0x40 * i), BIT0, BIT0); // BK19_42[0]

		if(i < 2) // PHY 0/1: PD_R-term
		{
			msWriteByteMask(REG_DPRX_PHY_PM_6B_H, 0, (BIT5 << i)); // BK19_6B[13:12]
		}
		else // PHY 2: PD_R-term
		{
            #if (ENABLE_HDMI || ENABLE_DVI)
			if((Input_HDMI_C3 != Input_Nothing) || (Input_DVI_C3 != Input_Nothing))
			{
				mdrv_hdmiRx_DataRtermControl(2, FALSE);
				mdrv_hdmiRx_ClockRtermControl(2, FALSE);
			}
			if((Input_HDMI_C4 != Input_Nothing) || (Input_DVI_C4 != Input_Nothing))
			{
				mdrv_hdmiRx_DataRtermControl(3, FALSE);
				mdrv_hdmiRx_ClockRtermControl(3, FALSE);
			}
		    #endif
		}

		msWriteByteMask(REG_DPRX_PHY_PM_6B_H, 0, BIT4|BIT3|BIT2); // BK19_6B[12:10]
	}

	for(i = 0; i < 3; i++) // POWER ON
	{
		if(PHY_Id == 4)
		{
			if(i >= 2)
			{
				continue;
			}
		}
		else
		{
			if(i != PHY_Id)
			{
				continue;
			}
		}

		Combo_printData("====Power On === %d", i);

		msWriteByteMask(REG_DPRX_PHY_PM_02_L + (0x40 * i), BIT1, BIT1);
		msWriteByteMask(REG_DPRX_PHY_PM_02_L + (0x40 * i), 0, BIT0);

		if(i < 2) // PHY 0/1: PD_R-term
		{
		}
		else // PHY 2: PD_R-term
		{
			if(INPUT_IS_HDMI(ucInputPort) || INPUT_IS_DVI(ucInputPort))
			{
				// Enable R-term in HDMI IP
			}
		}
	}

	return;
}

#if(COMBO_TMDS_PACKET_UPDATED_BY_VSYNC)
//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPGetPktReceived_Partial()
//  [Description]:
//                  use to update packet received status.
//  [Arguments]:
//                  enInputPort : combo port
//  [Return]:
//                  none.
//
//**************************************************************************
void msAPI_combo_IPGetPktReceived_Partial(BYTE ucInputPort)
{
    if(INPUT_IS_HDMI(ucInputPort) || INPUT_IS_DVI(ucInputPort))
    {
        mdrv_tmds_HDMI_GetPktReceived_Partial(ucInputPort);
    }
    else if(INPUT_IS_DISPLAYPORT(ucInputPort) || INPUT_IS_USBTYPEC(ucInputPort))
    {
        mapi_DPRx_GetPacketReceived_Partial(ucInputPort);
    }
}

//**************************************************************************
//  [Function Name]:
//                  mapi_tmds_HDMIGetErrorStatus_Partial()
//  [Description]:
//                  use to get HDMI err status
//  [Arguments]:
//                  enInputPort : combo port
//                  ucPacketType: indicate the type of the packet which is going to be extracted
//  [Return]:
//                  packet checksum error status.
//
//**************************************************************************
BYTE msAPI_combo_HDMI_GetErrorStatus_Partial(BYTE ucInputPort)
{
    return mdrv_tmds_HDMIGetErrorStatus_Partial(ucInputPort);
}
#endif

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPGetPacketContent()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool msAPI_combo_IPGetPacketContent(BYTE ucInputPort, EN_PACKET_DEFINE_TYPE ucPacketType, BYTE ucPacketLength, BYTE *pPacketData)
{
    Bool bPktReceived = FALSE;

    if(ucPacketLength > 0)
    {
        if(INPUT_IS_DISPLAYPORT(ucInputPort)||INPUT_IS_USBTYPEC(ucInputPort))
        {
            if(ucPacketType == COMBO_PACKET_DP_MISC)
            {
                bPktReceived = mapi_DPRx_Get_Misc(ucInputPort, pPacketData);
            }
            else if(ucPacketType == COMBO_PACKET_AUD_INFOFRAME)
            {
                bPktReceived = mapi_DPRx_GetPacketData(ucInputPort, DP_AUDIO_TIMESREAM, pPacketData, ucPacketLength);
            }
            else if(ucPacketType == COMBO_PACKET_HDR_INFOFRAME)
            {
                bPktReceived = mapi_DPRx_GetPacketData(ucInputPort, DP_HDR_PACKET, pPacketData, ucPacketLength);
            }
            else if(ucPacketType == COMBO_PACKET_SPD_INFOFRAME)
            {
                bPktReceived = mapi_DPRx_GetPacketData(ucInputPort, DP_SPD_PACKET, pPacketData, ucPacketLength);
            }
            #if(CHIP_ID >= CHIP_MT9701)
            else if(ucPacketType == COMBO_PACKET_ACP_PACKET)
            {
                bPktReceived = mapi_DPRx_GetPacketData(ucInputPort, DP_EXT_PACKET, pPacketData, ucPacketLength);
            }
            else if(ucPacketType == COMBO_PACKET_ISRC1_PACKET)
            {
                bPktReceived = mapi_DPRx_GetPacketData(ucInputPort, DP_ACM_PACKET, pPacketData, ucPacketLength);
            }
            else if(ucPacketType == COMBO_PACKET_ISRC2_PACKET)
            {
                bPktReceived = mapi_DPRx_GetPacketData(ucInputPort, DP_ISRC_PACKET, pPacketData, ucPacketLength);
            }
            else if(ucPacketType == COMBO_PACKET_ONE_BIT_AUDIO_SAMPLE)
            {
                bPktReceived = mapi_DPRx_GetPacketData(ucInputPort, DP_VSC_PACKET, pPacketData, ucPacketLength);
            }
            else if(ucPacketType == COMBO_PACKET_VS_INFOFRAME)
            {
                bPktReceived = mapi_DPRx_GetPacketData(ucInputPort, DP_VSP_PACKET, pPacketData, ucPacketLength);
            }
            else if(ucPacketType == COMBO_PACKET_AVI_INFOFRAME)
            {
                bPktReceived = mapi_DPRx_GetPacketData(ucInputPort, DP_AVI_PACKET, pPacketData, ucPacketLength);
            }
            else if(ucPacketType == COMBO_PACKET_MPEG_INFOFRAME)
            {
                bPktReceived = mapi_DPRx_GetPacketData(ucInputPort, DP_MPEG_PACKET, pPacketData, ucPacketLength);
            }
            else if(ucPacketType == COMBO_PACKET_VBI_INFOFRAME)
            {
                bPktReceived = mapi_DPRx_GetPacketData(ucInputPort, DP_NTSC_PACKET, pPacketData, ucPacketLength);
            }
            else if(ucPacketType == COMBO_AUDIO_CHANNEL_STATUS)
            {
                bPktReceived = mapi_DPRx_GetPacketData(ucInputPort, DP_AUDIO_CHANNEL_STATUS, pPacketData, ucPacketLength);
            }
            else if(ucPacketType == COMBO_PACKET_VSC_EXT_VESA_INFOFRAME)
            {
                bPktReceived = mapi_DPRx_GetPacketData(ucInputPort, DP_VSC_EXT_VESA_PACKET, pPacketData, ucPacketLength);
            }
            else if(ucPacketType == COMBO_PACKET_VSC_EXT_CEA_INFOFRAME)
            {
                bPktReceived = mapi_DPRx_GetPacketData(ucInputPort, DP_VSC_EXT_CEA_PACKET, pPacketData, ucPacketLength);
            }
            else if(ucPacketType == COMBO_PACKET_ADS_INFOFRAME)
            {
                bPktReceived = mapi_DPRx_GetPacketData(ucInputPort, DP_ADS_PACKET, pPacketData, ucPacketLength);
            }
            #endif
        }
        else if(INPUT_IS_HDMI(ucInputPort))
        {
            if(ucPacketType == COMBO_PACKET_AVI_INFOFRAME)
            {
                bPktReceived = mdrv_hdmiRx_GetPacketContent(PKT_AVI, ucPacketLength, pPacketData, ucInputPort);
            }
            else if(ucPacketType == COMBO_PACKET_MPEG_INFOFRAME)
            {
                bPktReceived = mdrv_hdmiRx_GetPacketContent(PKT_MPEG, ucPacketLength, pPacketData, ucInputPort);
            }
            else if(ucPacketType == COMBO_PACKET_AUD_INFOFRAME)
            {
                bPktReceived = mdrv_hdmiRx_GetPacketContent(PKT_AUI, ucPacketLength, pPacketData, ucInputPort);
            }
            else if(ucPacketType == COMBO_PACKET_SPD_INFOFRAME)
            {
                bPktReceived = mdrv_hdmiRx_GetPacketContent(PKT_SPD, ucPacketLength, pPacketData, ucInputPort);
            }
            else if(ucPacketType == COMBO_PACKET_GENERAL_CONTROL)
            {
                bPktReceived = mdrv_hdmiRx_GetPacketContent(PKT_GC, ucPacketLength, pPacketData, ucInputPort);
            }
            else if(ucPacketType == COMBO_PACKET_VS_INFOFRAME)
            {
                bPktReceived = mdrv_hdmiRx_GetPacketContent(PKT_VS, ucPacketLength, pPacketData, ucInputPort);
            }
            else if(ucPacketType == COMBO_PACKET_ISRC1_PACKET)
            {
                bPktReceived = mdrv_hdmiRx_GetPacketContent(PKT_ISRC1, ucPacketLength, pPacketData, ucInputPort);
            }
            else if(ucPacketType == COMBO_PACKET_ISRC2_PACKET)
            {
                bPktReceived = mdrv_hdmiRx_GetPacketContent(PKT_ISRC2, ucPacketLength, pPacketData, ucInputPort);
            }
            else if(ucPacketType == COMBO_PACKET_ACP_PACKET)
            {
                bPktReceived = mdrv_hdmiRx_GetPacketContent(PKT_ACP, ucPacketLength, pPacketData, ucInputPort);
            }
            else if(ucPacketType == COMBO_PACKET_HDR_INFOFRAME)
            {
                bPktReceived = mdrv_hdmiRx_GetPacketContent(PKT_HDR, ucPacketLength, pPacketData, ucInputPort);
            }
            else if(ucPacketType == COMBO_AUDIO_CHANNEL_STATUS)
            {
                bPktReceived = mdrv_hdmiRx_GetPacketContent(PKT_CHANNEL_STATUS, ucPacketLength, pPacketData, ucInputPort);
            }
        }
    }

    return bPktReceived;
}
#if(COMBO_HDCP2_FUNCTION_SUPPORT)
//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_HDCP2CheckEncodeKeyValid()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool msAPI_combo_HDCP2CheckEncodeKeyValid(BYTE *pKeyTable)
{
    Bool bKeyValidFlag = TRUE;
    BYTE uctemp = 0;

    for(uctemp = 0; uctemp < COMBO_HDCP2_ENCODE_KEY_CHECK_SIZE; uctemp++)
    {
        if(pKeyTable[uctemp] != ucKeyEncodeString[uctemp])
        {
            bKeyValidFlag = FALSE;

            break;
        }
    }

    return bKeyValidFlag;
}
#endif
//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_Hdcp2InsertKeyWithCutomizeKey()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool msAPI_combo_Hdcp2InsertKeyWithCutomizeKey(Bool bExternalKey, BYTE *pKeyTable, Bool bIsRxKey, Bool bUseCustomizeKey, BYTE* pCustomizeKey)
{
#if ENABLE_HDCP22
    Bool bRet = FALSE;
    WORD u16KeyLen = 0;
    WORD u16Cnt = 0;

    do
    {
        //avoid use External key but no load key result error use internal key
        if(bExternalKey == TRUE)
        {
            if (bIsRxKey == FALSE)
            {
                tCOMBO_HDCP2_CERTRX_TABLE[COMBO_HDCP2_ENCODE_TX_KEY_SIZE] = 1;
            }
            else
            {
                tCOMBO_HDCP2_CERTRX_TABLE[COMBO_HDCP2_ENCODE_KEY_SIZE + COMBO_HDCP2_AKE_CERTRX_SIZE] = 1;
            }
        }
        if ((bUseCustomizeKey == TRUE) && (pCustomizeKey == NULL))
        {
            Combo_printMsg("\033[1;31mUsing Customized AES key, but content is NULL!!!\033[m\r\n");
            bRet = FALSE;
            break;
        }

        if ((bExternalKey == TRUE) && (pKeyTable == NULL))
        {
            Combo_printMsg("\033[1;31mUsing External key, but content is NULL!!!\033[m\r\n");
            bRet = FALSE;
            break;
        }

        //g_bUseCustomerAESKey = bUseCustomizeKey;

        if (bUseCustomizeKey == FALSE) //without customerize key;
        {
            bRet = msAPI_combo_IPInsertHDCP22Key(bExternalKey, pKeyTable, bIsRxKey, FALSE);
            break;
        }

        if (bExternalKey == FALSE) //internal key;
        {
            Combo_printMsg("\033[1;31m Use Internal Key!!!\033[m\r\n");
            if (bIsRxKey == FALSE)
            {
                tCOMBO_HDCP2_CERTRX_TABLE[COMBO_HDCP2_ENCODE_TX_KEY_SIZE] = 0;
            }
            else
            {
                tCOMBO_HDCP2_CERTRX_TABLE[COMBO_HDCP2_ENCODE_KEY_SIZE + COMBO_HDCP2_AKE_CERTRX_SIZE] = 0;
            }

            bRet = TRUE;
            break;
        }

        if (msAPI_combo_HDCP2CheckEncodeKeyValid(&pKeyTable[32]) == FALSE)
        {
            Combo_printMsg("\033[1;31mInvalid Encoded Key Format!!!\033[m\r\n");
            bRet = FALSE;
            break;
        }

        u16KeyLen = bIsRxKey ? COMBO_HDCP2_ENCODE_KEY_SIZE : COMBO_HDCP2_ENCODE_TX_KEY_SIZE;

        for(u16Cnt = 0; u16Cnt < u16KeyLen; u16Cnt++)
        {
            tCOMBO_HDCP2_CERTRX_TABLE[u16Cnt] = pKeyTable[u16Cnt];
        }

        if (bIsRxKey == FALSE)
        {
            tCOMBO_HDCP2_CERTRX_TABLE[COMBO_HDCP2_ENCODE_TX_KEY_SIZE] = 1;

            for (u16Cnt = 0; u16Cnt < DEF_CUSTOMIZE_KEY_SIZE; u16Cnt++)
            {
                tCOMBO_HDCP2_CERTRX_TABLE[COMBO_HDCP2_ENCODE_TX_KEY_SIZE + 1 + u16Cnt] = pCustomizeKey[u16Cnt];
            }
        }
        else
        {
            tCOMBO_HDCP2_CERTRX_TABLE[COMBO_HDCP2_ENCODE_KEY_SIZE + COMBO_HDCP2_AKE_CERTRX_SIZE] = 1;

            for (u16Cnt = 0; u16Cnt < DEF_CUSTOMIZE_KEY_SIZE; u16Cnt++)
            {
                tCOMBO_HDCP2_CERTRX_TABLE[COMBO_HDCP2_ENCODE_KEY_SIZE + COMBO_HDCP2_AKE_CERTRX_SIZE + 1 + u16Cnt] = pCustomizeKey[u16Cnt];
            }
        }

        bRet = TRUE;

    } while(FALSE);

    return bRet;

#else
    UNUSED(bExternalKey);
    UNUSED(pKeyTable);
    UNUSED(bIsRxKey);
    UNUSED(bUseCustomizeKey);
    UNUSED(pCustomizeKey);
    return FALSE;
#endif
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPInsertHDCP22Key()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool msAPI_combo_IPInsertHDCP22Key(Bool bExternalKey, BYTE *pKeyTable, Bool bIsRxKey, BOOL bUseNewKey)
{
#if ENABLE_HDCP22
    Bool bDoneFlag = FALSE;
    WORD ustemp = 0;
    WORD u16KeyLen = 0;

    bUseNewKey = bUseNewKey; //fix compile warning;

    do
    {
        if (bExternalKey == TRUE)
        {
            if (pKeyTable == NULL)
            {
                Combo_printMsg("\033[1;31mUsing External key, but content is NULL!!!\033[m\r\n");
                bDoneFlag = FALSE;
                break;
            }

            if (msAPI_combo_HDCP2CheckEncodeKeyValid(&pKeyTable[32])== FALSE)
            {
                Combo_printMsg("\033[1;31mInvalid Encoded Key Format!!!\033[m\r\n");
                bDoneFlag = FALSE;
                break;
            }

            u16KeyLen = bIsRxKey ? COMBO_HDCP2_ENCODE_KEY_SIZE : COMBO_HDCP2_ENCODE_TX_KEY_SIZE;

            for(ustemp = 0; ustemp < u16KeyLen; ustemp++)
            {
                tCOMBO_HDCP2_CERTRX_TABLE[ustemp] = pKeyTable[ustemp];
            }

            ustemp = (bIsRxKey == TRUE) ? (COMBO_HDCP2_ENCODE_KEY_SIZE + COMBO_HDCP2_AKE_CERTRX_SIZE) : COMBO_HDCP2_ENCODE_TX_KEY_SIZE;
            tCOMBO_HDCP2_CERTRX_TABLE[ustemp] = 1;

            bDoneFlag = TRUE;
        }
        else //bExternalKey = false
        {
            ustemp = (bIsRxKey == TRUE) ? (COMBO_HDCP2_ENCODE_KEY_SIZE + COMBO_HDCP2_AKE_CERTRX_SIZE) : COMBO_HDCP2_ENCODE_TX_KEY_SIZE;
            tCOMBO_HDCP2_CERTRX_TABLE[ustemp] = 0;

            bDoneFlag = TRUE;
        }
    } while(FALSE);

    bSendCmdFlag = FALSE;

    return bDoneFlag;
#else
    UNUSED(bExternalKey);
    UNUSED(pKeyTable);
    UNUSED(bIsRxKey);
    UNUSED(bUseNewKey);
    return FALSE;
#endif
}

Bool msAPI_combo_FetchSecureStormResult(Bool bIsRxKey, BYTE* pu8DataBuf, WORD u16BufSize, WORD* pu16RetSize)
{
    Bool bRet = FALSE;

    do
    {
        if ((pu8DataBuf == NULL) || (pu16RetSize == NULL))
        {
            Combo_printMsg("NULL buffer pointer\r\n");
            bRet = FALSE;
            break;
        }

        if ((bHDCP2TxKeyDecodeDone == FALSE) && (bIsRxKey == FALSE))
        {
            Combo_printMsg("Tx Key is NOT Finish yet!\r\n");
            bRet = FALSE;
            break;
        }

        if (bSecuStoreDone == FALSE)
        {
            Combo_printMsg("SecureStorm is NOT Finish yet!\r\n");
            bRet = FALSE;
            break;
        }

        *pu16RetSize = bIsRxKey ? COMBO_HDCP2_ENCODE_KEY_SIZE : COMBO_HDCP2_ENCODE_TX_KEY_SIZE;

        if (u16BufSize != *pu16RetSize)
        {
            Combo_printMsg("Invaild Buffer Size!\r\n");
            bRet = FALSE;
            break;
        }

        mcu4kXdataMapToDRAMIMI(DEF_HK51_IMI_BANK0_SETTING, TRUE);
        IMI_ReadBytes(DEF_IMI_CERT_OFFSETADDR, (void*)tCOMBO_HDCP2_CERTRX_TABLE, DEF_HDCP2_RX_ENCODE_KEY_SIZE);

        memcpy((void*)pu8DataBuf, (void*)tCOMBO_HDCP2_CERTRX_TABLE, *pu16RetSize);
        bRet = TRUE;
    } while(FALSE);

    return bRet;
}


/**
 *  @brief      ReSet HPD low duration time
 *  @param      eInputPortMux -- combo port enum value
 *  @param      rxInfoUnion -- specific structure for information exchange
 *  @param      rxInfoSelect -- specific enum for information exchange
 *  @return     BOOL -- The operation work or not
*/
BOOL mapi_combo_RxInfo_Get(BYTE ucInputPort, ST_COMBO_RX_INFO_UNION* rxInfoUnion, EN_COMBO_RX_INFO_SELECT rxInfoSelect)
{
	BOOL bReturnValue = FALSE;

	do
	{
		if(INPUT_IS_DISPLAYPORT(ucInputPort)||INPUT_IS_USBTYPEC(ucInputPort))
		{
			bReturnValue = mapi_DPRx_PortInfo_Get((BYTE)ucInputPort, rxInfoUnion, rxInfoSelect);
		   	break;
		}

		if(INPUT_IS_HDMI(ucInputPort)||INPUT_IS_DVI(ucInputPort))
		{
		   bReturnValue = mdrv_hdmiRx_RxInfo_Get(ucInputPort, rxInfoUnion, rxInfoSelect);
           break;
		}
	} while(FALSE);



	return bReturnValue;
}

/**
 *  @brief      ReSet HPD low duration time
 *  @param      eInputPortMux -- combo port enum value
 *  @param      rxInfoUnion -- specific structure for information exchange
 *  @param      rxInfoSelect -- specific enum for information exchange
 *  @return     BOOL -- The operation work or not
*/
BOOL mapi_combo_RxInfo_Set(BYTE ucInputPort, ST_COMBO_RX_INFO_UNION* rxInfoUnion, EN_COMBO_RX_INFO_SELECT rxInfoSelect)
{
	BOOL bReturnValue = FALSE;

	
	do
	{
		if(INPUT_IS_DISPLAYPORT(ucInputPort)||INPUT_IS_USBTYPEC(ucInputPort))
		{
			bReturnValue = mapi_DPRx_PortInfo_Set((BYTE)ucInputPort, rxInfoUnion, rxInfoSelect);
		   	break;
		}

		if(INPUT_IS_HDMI(ucInputPort)||INPUT_IS_DVI(ucInputPort))
		{
		   
		}
	} while(FALSE);

	return bReturnValue;
}



void ________HDMI_ONLY________(void);

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPGetAVMuteEnableFlag()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL msAPI_combo_IPGetAVMuteEnableFlag(BYTE ucInputPort)
{
    BOOL bIsAVMuteEnabled = FALSE;
    do
    {
        if(INPUT_IS_DISPLAYPORT(ucInputPort)||INPUT_IS_USBTYPEC(ucInputPort) || INPUT_IS_DVI(ucInputPort))
        {
            break;
        }

        if (INPUT_IS_HDMI(ucInputPort))
        {
            bIsAVMuteEnabled = (msAPI_combo_HDMIRx_GetVideoContentInfo(COMBO_VIDEO_AVMUTE_FLAG, ucInputPort) == 0x01) ? TRUE : FALSE;
        }
    } while(FALSE);


    return bIsAVMuteEnabled;
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPCheckHDCPState()
//  [Description]:
//                  check HDCP is 1.4 or 2.2 or no encryption
//  [Arguments]:
//                  enInputPort : combo port
//  [Return]:
//                  HDCP1.4 or HDCP2.2 or no encryption
//**************************************************************************
EN_HDCP_STATE msAPI_combo_IPCheckHDCPState(BYTE ucInputPort)
{
    EN_HDCP_STATE enHDCPState = COMBO_HDCP_NO_ENCRYPTION;

    if(INPUT_IS_DISPLAYPORT(ucInputPort)||INPUT_IS_USBTYPEC(ucInputPort))
    {
#if ENABLE_DP_INPUT
        enHDCPState = (EN_HDCP_STATE)mapi_DPRx_CheckHDCPState(ucInputPort);
#endif
    }
    else if(INPUT_IS_HDMI(ucInputPort) || INPUT_IS_DVI(ucInputPort))
    {
        enHDCPState = (EN_HDCP_STATE)mdrv_hdmiRx_CheckHDCPState();
    }


    return enHDCPState;
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPGetAudioFrequency()
//  [Description]:
//                  msAPI_combo_IPGetAudioFrequency
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
EN_COMBO_AUDIO_FREQUENCY_INDEX msAPI_combo_IPGetAudioFrequency(BYTE ucInputPort)
{
    EN_COMBO_AUDIO_FREQUENCY_INDEX enTMDSAudioFrequency = COMBO_AUDIO_FREQUENCY_UNVALID;

    if(INPUT_IS_DISPLAYPORT(ucInputPort))
    {
#if ENABLE_DP_INPUT
        enTMDSAudioFrequency = (EN_COMBO_AUDIO_FREQUENCY_INDEX)mapi_DPRx_GetAudioFreq(ucInputPort);
#endif
    }
#if ENABLE_HDMI
    else if(INPUT_IS_HDMI(ucInputPort))
    {
#if (CHIP_ID == CHIP_MT9701)
        enTMDSAudioFrequency = (EN_COMBO_AUDIO_FREQUENCY_INDEX)mdrv_hdmiRx_GetAudioFrequency(ucInputPort);
#endif
    }
#endif
    return enTMDSAudioFrequency;
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPCable_5V_Detect()
//  [Description]:
//                  msAPI_combo_IPCable_5V_Detect
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL msAPI_combo_IPCable_5V_Detect(BYTE ucInputPort)
{
    BOOL bIsCableConnect = FALSE;
    if(INPUT_IS_DISPLAYPORT(ucInputPort))
    {
        return FALSE;
    }
#if (ENABLE_HDMI || ENABLE_DVI)
    else if(INPUT_IS_HDMI(ucInputPort) || INPUT_IS_DVI(ucInputPort))
    {
        bIsCableConnect = mdrv_hdmiRx_Cable_5V_Detect(ucInputPort);
    }
#endif
    return bIsCableConnect;
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPSet_Swap()
//  [Description]:
//                  This function is used for pn/rb swap; in further, it can be added more swap setting.
//  [Arguments]:
//                  BYTE ucInputPort: port index
//                  EN_COMBO_IP_SWAP_TYPE enComboIPSwapType
//                     COMBO_IP_SWAP_TYPE_NONE: nothing swap
//                     COMBO_IP_SWAP_TYPE_PN: pn swap
//                     COMBO_IP_SWAP_TYPE_RB: rb swap
//
//  [Return]:
//                  BOOL: swap done or not.
//**************************************************************************
BOOL msAPI_combo_IPSet_Swap(BYTE ucInputPort, EN_COMBO_IP_SWAP_TYPE enComboIPSwapType, BOOL bSwapEnable)
{
    BOOL bSwapDone = FALSE;

    if(INPUT_IS_DISPLAYPORT(ucInputPort))
    {
        return FALSE;
    }
#if (ENABLE_HDMI || ENABLE_DVI)
    else if(INPUT_IS_HDMI(ucInputPort) || INPUT_IS_DVI(ucInputPort))
    {
        bSwapDone = mdrv_hdmiRx_Set_RB_PN_Swap(ucInputPort, enComboIPSwapType, bSwapEnable);
    }
#endif

    return bSwapDone;
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPClearAVMuteEnableFlag()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL msAPI_combo_IPClearAVMuteEnableFlag(BYTE ucInputPort)
{
    BOOL bIsAVMuteEnabled = FALSE;
    do
    {
        if(INPUT_IS_DISPLAYPORT(ucInputPort)||INPUT_IS_USBTYPEC(ucInputPort) || INPUT_IS_DVI(ucInputPort))
        {
            break;
        }

        if (INPUT_IS_HDMI(ucInputPort))
        {
        }
    } while(FALSE);


    return bIsAVMuteEnabled;
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPGetDDRFlag_ISR()
//  [Description]
//                  msAPI_combo_IPGetDDRFlag_ISR
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool msAPI_combo_IPGetDDRFlag_ISR(void)
{
    Bool ucflag= FALSE;

    if(CURRENT_INPUT_IS_DISPLAYPORT())
    {
        #if (ENABLE_DP_INPUT)
          ucflag = mapi_DPRx_GetDRRFlag(SrcInputType);
        #endif
    }
    else if(CURRENT_INPUT_IS_HDMI())
    {
        #if (ENABLE_HDMI)
        ucflag = msAPI_combo_HDMIRx_GetVideoContentInfo(COMBO_VIDEO_FREE_SYNC_FLAG, SrcInputType);
        #endif
    }

    return ucflag;
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPGetDDRFlag()
//  [Description]
//                  msAPI_combo_IPGetDDRFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool msAPI_combo_IPGetDDRFlag(void)
{
    Bool ucflag= FALSE;

    if(CURRENT_INPUT_IS_DISPLAYPORT())
    {
        #if (ENABLE_DP_INPUT)
         ucflag = mapi_DPRx_GetDRRFlag(SrcInputType);
        #endif
    }
    else if(CURRENT_INPUT_IS_HDMI())
    {
        #if (ENABLE_HDMI)
        ucflag = (msAPI_combo_HDMIRx_GetVideoContentInfo(COMBO_VIDEO_FREE_SYNC_FLAG, SrcInputType) || msAPI_combo_HDMIRx_GetVideoContentInfo(COMBO_VIDEO_VRR_FLAG, SrcInputType) );
        #endif
    }

    return ucflag;
}
//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPGetDDRFlag()
//  [Description]
//                  msAPI_combo_IPGetDDRFlag
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************

void msAPI_combo_IPEnableDDRFlag(Bool Enable)
{

    #if (ENABLE_DP_INPUT)
    //0x6 is the total of DP related enum
    BYTE ubSysPort[6] = {Input_Displayport, Input_Displayport2, Input_Displayport3, Input_Displayport4, Input_UsbTypeC3, Input_UsbTypeC4};
    BYTE i;

    for(i = 0; i < 6; i++)
    {
        if(ubSysPort[i] == Input_Nothing)
        {
            continue;
        }
        mapi_DPRx_EnableDRRFunction(ubSysPort[i],Enable);
        msAPI_combo_IPControlHPD(ubSysPort[i],FALSE);
        ForceDelay1ms( 550 );
        msAPI_combo_IPControlHPD(ubSysPort[i],TRUE);
    }
    #endif

    if(CURRENT_INPUT_IS_HDMI())
    {
        Combo_printMsg("============2");
        Set_InputTimingChangeFlag(); // setup mode failed
        mStar_SetupFreeRunMode(); // setup freerun mode
    #if(CHIP_ID < CHIP_MT9701)
        #if ENABLE_HDMI_DRR_MCCS
        msAPI_combo_IPControlHPD(SrcInputType,FALSE);
        ForceDelay1ms(100);
        msAPI_combo_HDMIRx_ClockRtermControl(SrcInputType,FALSE);
        msAPI_combo_HDMIRx_DataRtermControl(SrcInputType,FALSE);
        ForceDelay1ms(500);
        msAPI_combo_HDMIRx_ClockRtermControl(SrcInputType,TRUE);
        msAPI_combo_HDMIRx_DataRtermControl(SrcInputType,TRUE);
        ForceDelay1ms(100);
        msAPI_combo_IPControlHPD(SrcInputType,TRUE);
        #else
        #if ENABLE_CABLE_5V_EDID && ENABLE_HDMI && ENABLE_FREESYNC
        // reload HDMI EDID & HPD
        #endif
        #endif
    #else
        #if ENABLE_HDMI && ENABLE_FREESYNC
        BYTE ComboPortIndex = 0;
        ComboPortIndex = msDrvMapInputToCombo(SrcInputType);

        if(ComboPortIndex >= HDMI_INPUT_PORT_END)
        {
            Combo_printMsg("** ComboPortIndex >= HDMI_INPUT_PORT_END\r\n");
            return;
        }
        else
        {
            mdrv_hdmiRx_HPDControl_By_Duration(ComboPortIndex, 700);
            mdrv_HDMIRx_SetFreeSyncEDID(ComboPortIndex, Enable);
        }
        #endif
    #endif
    }
}
//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPGetPixelClk()
//  [Description]
//                  msAPI_combo_IPGetPixelClk
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
WORD msAPI_combo_IPGetPixelClk(void)
{
    WORD Pixel_Clock=0;

    if(CURRENT_INPUT_IS_DISPLAYPORT())
    {
        #if (ENABLE_DP_INPUT)
        Pixel_Clock = mapi_DPRx_GetPixelClock(SrcInputType);
        #endif
    }
    else if(CURRENT_INPUT_IS_HDMI() || CURRENT_INPUT_IS_DVI())
    {
        #if (ENABLE_HDMI || ENABLE_DVI)
        Pixel_Clock = msAPI_combo_HDMIRx_GetVideoContentInfo(COMBO_VIDEO_CLK_COUNT, SrcInputType);
        #endif
    }

    return Pixel_Clock;
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPGetPixelClk()
//  [Description]
//                  msAPI_combo_IPGetPixelClk
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
WORD msAPI_combo_IPGetGetHTotal(void)
{
    WORD Htt = 0;
    #if (ENABLE_DP_INPUT)
    WORD Vtt = 0;
    #endif

    if(CURRENT_INPUT_IS_DISPLAYPORT())
    {
        #if (ENABLE_DP_INPUT)
        mapi_DPRx_GetHVInformation( SrcInputType,  &Htt, &Vtt );
        #endif
    }
    else if(CURRENT_INPUT_IS_HDMI() || CURRENT_INPUT_IS_DVI())
    {
        #if (ENABLE_HDMI || ENABLE_DVI)
        Htt = msAPI_combo_HDMIRx_GetVideoContentInfo(COMBO_VIDEO_CONTENT_HTT, SrcInputType);
        #endif
    }

    return Htt;
}

void ______COMBO_DP_ONLY________(void );
//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_GetDPCDLinkRate()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE msAPI_combo_DP_GetDPCDLinkRate(BYTE ucInputPort)
{
    BYTE ucLinkRate = 0x0;

    if(INPUT_IS_HDMI(ucInputPort) || INPUT_IS_DVI(ucInputPort))
    {
        return FALSE;
    }

    if(INPUT_IS_DISPLAYPORT(ucInputPort) || INPUT_IS_USBTYPEC(ucInputPort))
    {
        ucLinkRate = mapi_DPRx_GetDPCDLinkRate(ucInputPort);
    }

    return ucLinkRate;
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_GetDPCDLinkRate()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BYTE msAPI_combo_DP_GetDPLaneCnt(BYTE ucInputPort)
{
    BYTE ucLaneCnt = 0x0;

    if(INPUT_IS_HDMI(ucInputPort) || INPUT_IS_DVI(ucInputPort))
    {
        return FALSE;
    }

    if(INPUT_IS_DISPLAYPORT(ucInputPort) || INPUT_IS_USBTYPEC(ucInputPort))
    {
        ucLaneCnt = mapi_DPRx_GetDPLaneCnt(ucInputPort);
    }

    return ucLaneCnt;
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_DP_SetMCCSReplyEnable()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL msAPI_combo_DP_SetMCCSReplyEnable(BYTE ucInputPort, BOOL bEnable)
{
    if(INPUT_IS_HDMI(ucInputPort) || INPUT_IS_DVI(ucInputPort))
    {
        return FALSE;
    }

    if(INPUT_IS_DISPLAYPORT(ucInputPort) || INPUT_IS_USBTYPEC(ucInputPort))
    {
        return mapi_DPRx_SetMCCSReplyEnable(ucInputPort, bEnable);
    }

    return FALSE;
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_DP_GetMSAChg()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
Bool msAPI_combo_DP_GetMSAChg(BYTE ucInputPort)
{
    Bool bMSAChgFlag = FALSE;

	if(INPUT_IS_HDMI(ucInputPort) || INPUT_IS_DVI(ucInputPort))
    {
        return FALSE;
    }

    if(INPUT_IS_DISPLAYPORT(ucInputPort) || INPUT_IS_USBTYPEC(ucInputPort))
    {
        bMSAChgFlag = mapi_DPRx_GetMSAChgFlag(ucInputPort);
    }

    return bMSAChgFlag;
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_DP_ClrMSAChg()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAPI_combo_DP_ClrMSAChg(BYTE ucInputPort)
{
    if(INPUT_IS_HDMI(ucInputPort) || INPUT_IS_DVI(ucInputPort))
    {
        return;
    }

    if(INPUT_IS_DISPLAYPORT(ucInputPort) || INPUT_IS_USBTYPEC(ucInputPort))
    {
        mapi_DPRx_ClrMSAChgFlag(ucInputPort);
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_DP_InitRxHPD()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAPI_combo_DP_InitRxHPD(BOOL bEnableHigh)
{
	BYTE i;
	BYTE ubPortIndex;

    //0x6 is the total of DP related enum
    BYTE ubSysPort[0x6] = {Input_Displayport, Input_Displayport2, Input_Displayport3, Input_Displayport4, Input_UsbTypeC3, Input_UsbTypeC4};

    for(i = 0; i < 6; i++)
    {
        if(ubSysPort[i] == Input_Nothing)
        {
            continue;
        }

        ubPortIndex = ubSysPort[i];

		if(INPUT_IS_DISPLAYPORT(ubPortIndex)&&(INPUT_IS_USBTYPEC(ubPortIndex) == FALSE))
	    {
	        if(i < 4)
            {
	    	    Combo_printData("-IP port %x DP-", i);
            }
            else
            {
                Combo_printData("-IP port %x DP-", (i-2));
            }

	    	#if (ENABLE_DP_INPUT == 0x1)
			mapi_DPRx_HPDControl(ubPortIndex, bEnableHigh);
			#else
			bEnableHigh = 0x0;
			#endif
	    }
		else if(INPUT_IS_USBTYPEC(ubPortIndex))
		{
		    if(i < 4)
            {
			    Combo_printData("-IP port %x TypeC-", i);
            }
            else
            {
                Combo_printData("-IP port %x TypeC-", (i-2));
            }

		}
    }

	return;
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_DP_SetFastTrainingTime()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void msAPI_combo_DP_SetFastTrainingTime(BYTE ucInputPort, BYTE ubTime)
{
    if(INPUT_IS_HDMI(ucInputPort) || INPUT_IS_DVI(ucInputPort))
    {
        return;
    }

    if(INPUT_IS_DISPLAYPORT(ucInputPort) || INPUT_IS_USBTYPEC(ucInputPort))
    {
        mapi_DPRx_SetFastTrainingTime(ucInputPort, ubTime);
    }

    return;
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_DP_CheckDPTimingStable()
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL msAPI_combo_DP_CheckDPTimingStable(BYTE ucInputPort)
{
	BOOL bStableFlag = FALSE;

	if(INPUT_IS_HDMI(ucInputPort) || INPUT_IS_DVI(ucInputPort))
    {
        return FALSE;
    }

    if(INPUT_IS_DISPLAYPORT(ucInputPort) || INPUT_IS_USBTYPEC(ucInputPort))
    {
        bStableFlag = mapi_DPRx_CheckDPTimingStable(ucInputPort);
    }

    return bStableFlag;
}

//**************************************************************************
//  [Function Name]:
//                  msAPI_combo_IPDPGetVideoStreamCRC(),  for SST stream only !!!
//  [Description]
//
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
BOOL msAPI_combo_DP_GetVideoStreamCRC(BYTE ucInputPort, BYTE* pCRC_Data)
{
    Bool ucflag= FALSE;

	if(INPUT_IS_HDMI(ucInputPort) || INPUT_IS_DVI(ucInputPort))
    {
        ucflag = FALSE;
    }

    if(INPUT_IS_DISPLAYPORT(ucInputPort) || INPUT_IS_USBTYPEC(ucInputPort))
    {
        ucflag = mapi_DPRx_GetVideoStreamCRC(ucInputPort, pCRC_Data);
    }

    return ucflag;
}

#endif

