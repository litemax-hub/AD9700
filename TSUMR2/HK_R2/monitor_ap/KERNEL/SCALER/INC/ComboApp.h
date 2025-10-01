#ifndef _ComboApp_H_
#define _ComboApp_H_

#ifdef _ComboApp_C_
#define _ComboApp_
#else
#define _ComboApp_  extern
#endif

#include "board.h"
//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

extern BYTE XDATA glLastInputPort;
////// HDCP 2.2 Key Management //////
#define CRC_SIZE  4 // 4 Bytes CRC for APP layer to judge if key is valid
#define HDCP_KEY_22_RX_1ST_ADDR 0xF00000 //0x078000 -Flash Address Test, 0xFFFFFF ==> no defined
#define HDCP_KEY_22_RX_2ND_ADDR 0xF10000 //0x079000 -Flash Address Test, 0xFFFFFF ==> no defined

#define COMBO_IP_SUPPORT_0PORT          0
#define COMBO_IP_SUPPORT_1PORT          1
#define COMBO_IP_SUPPORT_2PORT          2
#define COMBO_IP_SUPPORT_3PORT          3
#define COMBO_IP_SUPPORT_4PORT          4
#define COMBO_IP_SUPPORT_5PORT          5
#define COMBO_IP_SUPPORT_6PORT          6
#define COMBO_IP_SUPPORT_7PORT          7

#define COMBO_IP_SUPPORT_TYPE           COMBO_IP_SUPPORT_5PORT

#define COMBO_OP_SUPPORT_0PORT          0
#define COMBO_OP_SUPPORT_1PORT          1
#define COMBO_OP_SUPPORT_2PORT          2
#define COMBO_OP_SUPPORT_3PORT          3
#define COMBO_OP_SUPPORT_4PORT          4
#define COMBO_OP_SUPPORT_5PORT          5
#define COMBO_OP_SUPPORT_6PORT          6
#define COMBO_OP_SUPPORT_7PORT          7

#define COMBO_OP_SUPPORT_TYPE           COMBO_OP_SUPPORT_1PORT

// HDCP 2.2
#if (ENABLE_SECU_R2 == 1) && (ENABLE_HDCP22 == 1)
#define DEF_COMBO_HDCP2RX_ISR_MODE      1
#define COMBO_HDCP2_FUNCTION_SUPPORT    1
#define COMBO_HDCP2_DECODE_KEY          1
#define COMBO_HDCP2_HDMI14PORT_EN       1
#define COMBO_HDCP2_HDMI20PORT_EN       1
#define COMBO_HDCP2_INITPROC_NEW_MODE   1// 1
#define COMBO_HDCPTX_BLOCK_ENABLE       0 // 1
#define DEF_HDCP_MANUAL_MODE 			0
    #if (COMBO_HDCP2_INITPROC_NEW_MODE)
    #define COMBO_HDCP2_SECUR2INT_WDT       150 //unit: ms
    #else
    #define COMBO_HDCP2_SECUR2INT_WDT       50 //unit: ms
    #endif
#else
#define COMBO_HDCP2_INITPROC_NEW_MODE   0// 1
#define COMBO_HDCP2_FUNCTION_SUPPORT    0
#define DEF_COMBO_HDCP2RX_ISR_MODE      0
#endif

#define COMBO_HDCP2_AKE_CERTRX_SIZE     522
#define COMBO_HDCP2_ENCODE_KEY_SIZE     1044//1008
#define COMBO_HDCP2_USER_ID_SIZE            8
#define COMBO_HDCP2_ENCODE_TX_KEY_SIZE     580
#define ENABLE_HDMIRX_RTERM_45OHM       0 // 1: efuse value 450hm; 0: efuse value 500hm


typedef enum
{
    COMBO_COLOR_FORMAT_RGB         = 0,     ///< HDMI RGB 444 Color Format
    COMBO_COLOR_FORMAT_YUV_422     = 1,     ///< HDMI YUV 422 Color Format
    COMBO_COLOR_FORMAT_YUV_444     = 2,     ///< HDMI YUV 444 Color Format
    COMBO_COLOR_FORMAT_YUV_420     = 3,     ///< HDMI YUV 420 Color Format
    COMBO_COLOR_FORMAT_RESERVED    = 4,     ///< Reserve
    COMBO_COLOR_FORMAT_DEFAULT     = COMBO_COLOR_FORMAT_RGB,///< Default setting
    COMBO_COLOR_FORMAT_UNKNOWN     = 15,    ///< Unknow Color Format
}EN_COLOR_FORMAT_TYPE;

typedef enum
{
    COMBO_COLOR_DEPTH_8BIT = 0,
    COMBO_COLOR_DEPTH_10BIT,
    COMBO_COLOR_DEPTH_12BIT,
    COMBO_COLOR_DEPTH_16BIT,
    COMBO_COLOR_DEPTH_6BIT, //for DP
    COMBO_COLOR_DEPTH_NONE,
}EN_COLOR_DEPTH_TYPE;

typedef enum
{
    COMBO_COLOR_RANGE_DEFAULT    = 0, //depend on video format;
    COMBO_COLOR_RANGE_LIMIT      = 1,
    COMBO_COLOR_RANGE_FULL       = 2,
    COMBO_COLOR_RANGE_RESERVED   = 3,
}EN_COLOR_RANGE_TYPE;

typedef enum
{
    COMBO_COLORIMETRY_xvYCC601             = 0,    ///< HDMI xvYCC601 Colorimetry
    COMBO_COLORIMETRY_xvYCC709            = 1,    ///< HDMI xvYCC709 Colorimetry
    COMBO_COLORIMETRY_sYCC601             = 2,    ///< HDMI sYCC601 colorimetry
    COMBO_COLORIMETRY_ADOBEYCC601         = 3,    ///< HDMI Adobe YCC 601 colorimetry
    COMBO_COLORIMETRY_ADOBERBG            = 4,    ///< HDMI Adobe RGB colorimetry
    COMBO_COLORIMETRY_BT2020YcCbcCrc      = 5,    ///< HDMI BT2020 YcCbcCrc colorimetry
    COMBO_COLORIMETRY_BT2020RGBYCbCr      = 6,    ///< HDMI BT2020 RGB or YCbCr colorimetry
    COMBO_COLORIMETRY_ITU601              = 7,    ///< HDMI ITU601 Colorimetry
    COMBO_COLORIMETRY_ITU709              = 8,    ///< HDMI ITU709 Colorimetry
    COMBO_COLORIMETRY_DCI_P3_RGB_D65      = 9,
    COMBO_COLORIMETRY_DCI_P3_RGB_Theater  = 10,
    COMBO_COLORIMETRY_AdditionalCE        = 11,
    COMBO_COLORIMETRY_NONE                = 15,
}EN_COLORIMETRY_TYPE;

typedef enum
{
    COMBO_YUV_COLORIMETRY_ITU601=0,
    COMBO_YUV_COLORIMETRY_ITU709,
    COMBO_YUV_COLORIMETRY_NoData,
    COMBO_YUV_COLORIMETRY_EC,
    COMBO_YUV_COLORIMETRY_Max,
} EN_YPBPR_COLORIMETRY_TYPE;

typedef enum
{
    COMBO_COLORIMETRY_Additional_DCI_P3_RGB_D65 = 0,
    COMBO_COLORIMETRY_Additional_DCI_P3_RGB_Theater,
    COMBO_COLORIMETRY_Additional_NoData,
}EN_EXTENDED_ADDITIONAL_COLORIMETRY_TYPE;

typedef enum
{
    COMBO_KEY_RX = 0,
    COMBO_KEY_TX = 1,
}EN_COMBO_KEY_TYPE;


typedef struct
{
    EN_COLOR_FORMAT_TYPE        ucColorType;
	EN_COLOR_DEPTH_TYPE         ucColorDepth;
    EN_COLOR_RANGE_TYPE         ucColorRange;
    EN_COLORIMETRY_TYPE         ucColorimetry;
    EN_YPBPR_COLORIMETRY_TYPE   ucYuvColorimetry;
    EN_EXTENDED_ADDITIONAL_COLORIMETRY_TYPE ucExtAddiColoirty;
}ST_COMBO_COLOR_FORMAT;

typedef enum
{
    COMBO_PACKET_NULL                       = 0x00, //depend on video format;
    COMBO_PACKET_AUDIO_CLOCK_REGENERATION   = 0x01,
    COMBO_PACKET_AUDIO_SAMPLE               = 0x02,
    COMBO_PACKET_GENERAL_CONTROL            = 0x03,
    COMBO_PACKET_ACP_PACKET                 = 0x04,
    COMBO_PACKET_ISRC1_PACKET               = 0x05,
    COMBO_PACKET_ISRC2_PACKET               = 0x06,
    COMBO_PACKET_ONE_BIT_AUDIO_SAMPLE       = 0x07,
    COMBO_PACKET_DST_AUDIO                  = 0x08,
    COMBO_PACKET_HBR_AUDIO_STREAM           = 0x09,
    COMBO_PACKET_GAMUT_METADATA             = 0x0A,
    COMBO_PACKET_VSC_EXT_VESA_INFOFRAME     = 0x20,
    COMBO_PACKET_VSC_EXT_CEA_INFOFRAME      = 0x21,
    COMBO_PACKET_ADS_INFOFRAME              = 0x22,
    COMBO_PACKET_VS_INFOFRAME               = 0x81,
    COMBO_PACKET_AVI_INFOFRAME              = 0x82,
    COMBO_PACKET_SPD_INFOFRAME              = 0x83,
    COMBO_PACKET_AUD_INFOFRAME              = 0x84,
    COMBO_PACKET_MPEG_INFOFRAME             = 0x85,
    COMBO_PACKET_VBI_INFOFRAME              = 0x86,
    COMBO_PACKET_HDR_INFOFRAME              = 0x87,
    COMBO_PACKET_EXTENDED_METADATA          = 0x7F,
    COMBO_AUDIO_CHANNEL_STATUS              = 0xE0, // not packet type value
    COMBO_PACKET_DP_MISC                    = 0xFF,
}EN_PACKET_DEFINE_TYPE;


typedef enum
{
    COMBO_AUDIO_FMT_NON_PCM   = 0,
    COMBO_AUDIO_FMT_PCM       = 1,
    COMBO_AUDIO_FMT_UNKNOWN   = 2,
} EN_TMDS_AUDIO_FORMAT;

typedef enum
{
    COMBO_IP_SWAP_TYPE_NONE = 0,
    COMBO_IP_SWAP_TYPE_PN,
    COMBO_IP_SWAP_TYPE_RB,
}EN_COMBO_IP_SWAP_TYPE;
//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
typedef enum        // Define HDCP Key Type
{
    HDCP_KEY_14_RX_1ST,
    HDCP_KEY_14_TX_1ST,
    HDCP_KEY_22_RX_1ST,
    HDCP_KEY_22_TX_1ST,
    HDCP_KEY_14_RX_2ND,
    HDCP_KEY_14_TX_2ND,
    HDCP_KEY_22_RX_2ND,
    HDCP_KEY_22_TX_2ND,
}EN_HDCP_KEY_TYPE;

typedef enum
{
    COMBO_HDCP_NO_ENCRYPTION = 0,
    COMBO_HDCP_1_4 = 1,
    COMBO_HDCP_2_2 = 2,
}EN_HDCP_STATE;

typedef enum
{
    COMBO_AUDIO_FREQUENCY_32K       = 32,
    COMBO_AUDIO_FREQUENCY_44K       = 44,
    COMBO_AUDIO_FREQUENCY_48K       = 48,
    COMBO_AUDIO_FREQUENCY_88K       = 88,
    COMBO_AUDIO_FREQUENCY_96K       = 96,
    COMBO_AUDIO_FREQUENCY_176K      = 176,
    COMBO_AUDIO_FREQUENCY_192K      = 192,
    COMBO_AUDIO_FREQUENCY_UNVALID   = 0,

} EN_COMBO_AUDIO_FREQUENCY_INDEX;


#if (defined(COMBO_HDCP2_INITPROC_NEW_MODE) && (COMBO_HDCP2_INITPROC_NEW_MODE == 1))
typedef enum
{
    EN_HDCP_KEY_INIT_STATUS_FAIL = 0,
    EN_HDCP_KEY_INIT_STATUS_PASS,
    EN_HDCP_KEY_INIT_STATUS_NONE,
} ENUM_HDCP_KEY_INIT_STATUS;
#endif

typedef enum
{
    COMBO_VIDEO_CONTENT_HDE = 0,
    COMBO_VIDEO_CONTENT_VDE,
    COMBO_VIDEO_CONTENT_HTT,
    COMBO_VIDEO_CONTENT_VTT,
    COMBO_VIDEO_CLK_STABLE,
    COMBO_VIDEO_CLK_COUNT,
    COMBO_VIDEO_DE_STABLE,
    COMBO_VIDEO_COLOR_FROMAT,
    COMBO_VIDEO_COLOR_DEPTH,
    COMBO_VIDEO_COLOR_RANGE,
    COMBO_VIDEO_HDR_FLAG,
    COMBO_VIDEO_FREE_SYNC_FLAG,
    COMBO_VIDEO_AVMUTE_FLAG,
    COMBO_VIDEO_VRR_FLAG,
    COMBO_VIDEO_ALLM_FLAG,
    COMBO_VIDEO_HDMI_MODE_FLAG,
}EN_COMBO_VIDEO_CONTENT_INFO;

typedef enum
{
    COMBO_HDMI_IRQ_PHY,
    COMBO_HDMI_IRQ_MAC,
    COMBO_HDMI_IRQ_PKT_QUE,
    COMBO_HDMI_IRQ_PM_SQH_ALL_WK,
    COMBO_HDMI_IRQ_PM_SCDC,
    COMBO_HDMI_FIQ_CLK_DET_0,
    COMBO_HDMI_FIQ_CLK_DET_1,
    COMBO_HDMI_FIQ_CLK_DET_2,
    COMBO_HDMI_FIQ_CLK_DET_3,
    COMBO_HDMI_INT_N,
}EN_COMBO_HDMIRX_INT;


typedef enum 
{
	COMBO_DP_VERSION_11 = 0,
	COMBO_DP_VERSION_12,
	COMBO_DP_VERSION_14,
	COMBO_DP_VERSION_20,
	COMBO_DP_VERSION_UNKNOW,
}EN_COMBO_DP_VERSION;


typedef enum
{

//===========Naming Rules==========
/*	
COMBO_RX_INFO_( Data transfer by variable or structure )_( Feature name )_ ( available for read or write )

Available for read or write:
Available for read only: RO
Available for write only: WO
Available for both read & write: RW

Data transfer by variable or structure:
Transfer by variable: VAR
Transfer by structure: ST
*/


    //=========DP used only==============
   
   
    //=========HDMI used only==============
    COMBO_RX_INFO_VAR_V_POLARITY_RO = 0x50, //HDMI used only start
   
    
    //=========IP Common used only==============
    COMBO_RX_INFO_VAR_HTT_RO = 0xA0,   //DP RX: MSA HTT
    COMBO_RX_INFO_VAR_PIXEL_CLOCK_10KHZ,
    
    //=========Union Structure Common Used only==============
    
	COMBO_RX_VAR_WAKEUPSTATUS_WO = 0x121,
	//=========Union Structure DP Used only==============
	
	

	//=========Union Structure HDMI Used only==============
}EN_COMBO_RX_INFO_SELECT;


typedef struct
{
    DWORD ulValue;

    union{

		//COMBO_RX_ST_VIDEO_DE_TOTAL_RO
        struct{
        	DWORD HDE;
            DWORD VDE;
            DWORD HTT;
            DWORD VTT;
        } ST_COMBO_VIDEO_DE_TOTAL;


		//COMBO_RX_ST_DP_VERSION_INFO_RO
		struct{
			EN_COMBO_DP_VERSION port_dp_version_info;
		}ST_COMBO_DP_VERSION_INFO;
    };
} ST_COMBO_RX_INFO_UNION;


//-------------------------------------------------------------------------------------------------
//  Function Prototype
//-------------------------------------------------------------------------------------------------

extern Bool msAPI_combo_IPGetDDRFlag_ISR(void);
extern Bool msAPI_combo_IPGetDDRFlag(void);
extern void msAPI_combo_IPEnableDDRFlag(Bool Enable);
extern WORD msAPI_combo_IPGetPixelClk(void);
extern WORD msAPI_combo_IPGetGetHTotal(void);
void msAPI_combo_IPPHYSetting(void);
BOOL msAPI_combo_InitHandler(BOOL bUseCustomizeKey);
Bool msAPI_combo_ProcessHdcp2TxKey(BOOL bUseCustomizeKey);
BOOL msAPI_combo_ProcessHdcp1TxKey(void);
void msAPI_combo_SECU_PowerDown(BOOL bSetPowerDown);
BOOL msAPI_combo_CheckSecuStoreDone(void);
void msAPI_combo_IPTimerIsrHandler(void);
void msAPI_combo_Handler(void);
void msAPI_combo_IPTimerIsrHandler(void);
#if ENABLE_HDMI_BCHErrorIRQ
void msAPI_combo_HDMIRx_BCHInterrupt(void);
#endif
//void msAPI_combo_HDCPInitialRebootValue(void);
extern Bool msAPI_combo_IPGetInterlace(void);
BOOL msAPI_combo_IPControlHPD(BYTE ucInputPort, Bool bSetHPD);
void msAPI_combo_HDMIRx_ClockRtermControl(BYTE ucInputPort, Bool bPullHighFlag);
void msAPI_combo_HDMIRx_DataRtermControl(BYTE ucInputPort, Bool bPullHighFlag);
WORD msAPI_combo_HDMIRx_GetVideoContentInfo(EN_COMBO_VIDEO_CONTENT_INFO enVideoContentInfo, BYTE ucPortSelect);
void msAPI_combo_DP_InitRxHPD(BOOL bEnableHigh);
BYTE msAPI_combo_DP_GetDPCDLinkRate(BYTE ucInputPort);
BYTE msAPI_combo_DP_GetDPLaneCnt(BYTE ucInputPort);
BOOL msAPI_combo_DP_SetMCCSReplyEnable(BYTE ucInputPort, BOOL bEnable);
Bool msAPI_combo_DP_GetMSAChg(BYTE ucInputPort);
void msAPI_combo_DP_ClrMSAChg(BYTE ucInputPort);
void msAPI_combo_DP_SetFastTrainingTime(BYTE ucInputPort, BYTE ubTime);
BOOL msAPI_combo_DP_CheckDPTimingStable(BYTE ucInputPort);
BOOL msAPI_combo_DP_GetVideoStreamCRC(BYTE ucInputPort, BYTE* pCRC_Data);
BYTE msAPI_combo_GetComboPortIndex(BYTE ucInputPort);
void msAPI_combo_HDMIRx_DDCControl(BYTE ucInputPort, Bool bEnable);
void msAPI_combo_HDMIRx_EDIDLoad(BYTE ucInputPort, BYTE *EdidData);
void msAPI_combo_HDMIRx_IRQ_Enable(EN_COMBO_HDMIRX_INT e_int, BYTE enInputPortSelect, MS_U16 bit_msk, Bool bEnableIRQ);
#if ENABLE_HPD_REPLACE_MODE
void msAPI_combo_HDMIRx_MAC_HDCP_Enable(Bool bEnable);
void msAPI_combo_HDMIRx_SCDC_Clr(BYTE ucInputPort);
void msAPI_combo_HDMIRx_SCDC_config(BYTE enInputPortSelect, BYTE bDDC);
#endif
void msAPI_combo_IPInitial(Bool bACon);

#if ENABLE_HDMI_BCHErrorIRQ
void msAPI_combo_HDMIRx_EnableHDMIRx_PKT_ParsrIRQ(Bool bEnable);
void msAPI_combo_HDMIRx_IRQMaskEnable(WORD enIRQType, Bool bEnable);
void msAPI_combo_HDMIRx_IRQClearStatus(WORD enIRQType);
Bool msAPI_combo_HDMIRx_IRQCheckStatus(WORD enIRQType);
#endif

void msAPI_combo_IPPowerControl(BYTE ucInputPort);
#if(COMBO_TMDS_PACKET_UPDATED_BY_VSYNC)
void msAPI_combo_IPGetPktReceived_Partial(BYTE ucInputPort);
BYTE msAPI_combo_HDMI_GetErrorStatus_Partial(BYTE ucInputPort);
#endif
BOOL msAPI_combo_IPGetPacketContent(BYTE ucInputPort, EN_PACKET_DEFINE_TYPE ucPacketType, BYTE ucPacketLength, BYTE *pPacketData);
Bool msAPI_combo_HDCP2CheckEncodeKeyValid(BYTE *pKeyTable);
Bool msAPI_combo_Hdcp2InsertKeyWithCutomizeKey(Bool bExternalKey, BYTE *pKeyTable, Bool bIsRxKey, Bool bUseCustomizeKey, BYTE* pCustomizeKey);
Bool msAPI_combo_IPInsertHDCP22Key(Bool bExternalKey, BYTE *pKeyTable, Bool bIsRxKey, BOOL bUseNewKey);
Bool msAPI_combo_FetchSecureStormResult(Bool bIsRxKey, BYTE* pu8DataBuf, WORD u16BufSize, WORD* pu16RetSize);

BOOL mapi_combo_RxInfo_Get(BYTE ucInputPort, ST_COMBO_RX_INFO_UNION* rxInfoUnion, EN_COMBO_RX_INFO_SELECT rxInfoSelect);
BOOL mapi_combo_RxInfo_Set(BYTE ucInputPort, ST_COMBO_RX_INFO_UNION* rxInfoUnion, EN_COMBO_RX_INFO_SELECT rxInfoSelect);


BOOL msAPI_combo_IPGetHDRPacket(BYTE ucInputPort , BYTE *pHeader, BYTE *pPacketData);
ST_COMBO_COLOR_FORMAT msAPI_combo_IPGetColorFormat(BYTE ucInputPort);
EN_TMDS_AUDIO_FORMAT msAPI_combo_HDMIRx_IsAudioFmtPCM(BYTE ucPortIndex);
BOOL msAPI_combo_IPGetAVMuteEnableFlag(BYTE ucInputPort);
EN_HDCP_STATE msAPI_combo_IPCheckHDCPState(BYTE ucInputPort);
EN_COMBO_AUDIO_FREQUENCY_INDEX msAPI_combo_IPGetAudioFrequency(BYTE ucInputPort);
BOOL msAPI_combo_IPCable_5V_Detect(BYTE ucInputPort);
void msAPI_combo_HDMIRx_SCDC_Field_Set(BYTE ucInputPort, BYTE u8Offset, BYTE u8Size, BYTE *u8SetValue);
BOOL msAPI_combo_IPSet_Swap(BYTE ucInputPort, EN_COMBO_IP_SWAP_TYPE enComboIPSwapType, BOOL bSwapEnable);
#if (COMBO_HDCP2_INITPROC_NEW_MODE)
ENUM_HDCP_KEY_INIT_STATUS msAPI_combo_HDCP2InitHandler(BOOL bUseCustomizeKey);
void msAPI_HDCP2_reset_static_variable(void);
#endif
#endif

