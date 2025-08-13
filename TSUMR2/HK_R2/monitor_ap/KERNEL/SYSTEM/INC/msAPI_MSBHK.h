#ifndef _MSAPI_MSBHK_H_
#define _MSAPI_MSBHK_H_

#include "Ms_PM.h"

#ifdef _MSAPI_MSBHK_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

#define UART_WAIT_TIME     5000 //ms
#define UART_CMD_LEN        600

#define UART_COM_LEN0     SIORxBuffer1[0]
#define UART_COM_LEN1     SIORxBuffer1[1]
#define UART_COM_LEN     ((UART_COM_LEN0 <<8) |UART_COM_LEN1)
#define UART_COM_SEQ     SIORxBuffer1[2]
#define UART_COM_CLASS     SIORxBuffer1[3]
#define UART_COM_SUBCLASS    SIORxBuffer1[4]
#define UART_COM_CKSH     SIORxBuffer1[UART_COM_LEN-1]

///// Need to Sync with MSBHK //////
typedef enum _APICommadEnumType
{
    API_DEVICE_CLASS = 0x00,
    API_SECURE_CLASS = 0x10,
    API_CONFIG_CLASS = 0x20,
    API_IMAGE_CLASS = 0x30,
    API_AUDIO_CLASS = 0x40,
    API_HDMI_CLASS = 0x50,
    API_DP_CLASS = 0x60,
    API_INFO_CLASS = 0x70,
    API_NOTICE_CLASS = 0xF0,
    API_DEBUG_CLASS = 0xFF
}APICommadEnumType;

typedef enum _APIDeviceClassEnumType
{
    DEV_Reset = 0x00,
    DEV_PMMode = 0x01,
    DEV_WDT = 0x02,
    DEV_GetCMD = 0xFF
}APIDeviceClassEnumType;

typedef enum _APISecureClassEnumType
{
    SECU_RxEDID = 0x00,
    SECU_RxEDIDMode = 0x01,
    SECU_RxEDIDTbl = 0x02,
    SECU_RxEDIDTblCfg = 0x03,
    SECU_RxHDCPMode = 0x04,
    SECU_TxHDCPMode = 0x05,
    SECU_LoadHDCPKey = 0x06,
    SECU_GetCMD = 0xFF
}APISecureClassEnumType;

typedef enum _APIConfigClassEnumType
{
    CFG_IOMUX = 0x00,
    CFG_INPORT = 0x01,
    CFG_OUTPORT = 0x02,
    CFG_HPD = 0x03,
    CFG_RPT = 0x04,
    CFG_GPIOMode = 0x05,
    CFG_GPIOValue = 0x06,
    CFG_UART = 0x07,
//    CFG_SgnChk = 0x08,
    CFG_AEQ = 0x09,
    CFG_EQCap = 0x0A,
    CFG_UARTBR = 0x0B,
    CFG_PkgCmd = 0x0C,
    CFG_GetCMD = 0xFF
}APIConfigClassEnumType;

typedef enum _APIImageClassEnumType
{
    IMG_TXPTNGEN = 0x00,
    IMG_GetCMD = 0xFF
}APIImageClassEnumType;

typedef enum _APIAudioClassEnumType
{
    AUD_I2SMUX = 0x00,
    AUD_MUXSEL = 0x01,
    AUD_GetCMD = 0xFF
}APICAudioClassEnumType;

typedef enum _APIHDMIClassEnumType
{
    HDMI_HDR = 0x00,
    HDMI_TXSW = 0x01,
    HDMI_TXTEM = 0x02,
    HDMI_ARC = 0x03,
    HDMI_CEC = 0x04,
    HDMI_GetCMD = 0xFF
}APIHDMIClassEnumType;

typedef enum _APIDPClassEnumType
{
    DP_VER = 0x00,
    DP_MST = 0x01,
    DP_DSCPORT = 0x02,
    DP_DSCMODE = 0x03,
    DP_OUI = 0x04,
    DP_MSTEDID = 0x05,
    DP_GetCMD = 0xFF
}APIDPClassEnumType;

typedef enum _APIInfoClassEnumType
{
    IFO_GetCMD = 0xFF
}APIInfoClassEnumType;

typedef enum _APINoticeClassEnumType
{
    NTE_SgnDetected = 0x00,
    NTE_BtCmp = 0x01,
    NTE_CECPwrOn = 0x02,
    NTE_GPIODetected = 0x03,
    NTE_MCCSDetected = 0x04
}APINoticeClassEnumType;

typedef enum _APIDebugClassEnumType
{
    DBG_SetUserData = 0x00,
    DBG_UserDataSaveFlag = 0x01,
    DBG_UserDataCmt = 0x02,
    DBG_SetReg = 0x03,
    DBG_GetCMD = 0xFF
}APIDebugClassEnumType;

typedef enum _APIResponseEnumType
{
    NOACK = 0,
    MATCHCMD = 0x10,
    CHECKSUM_FAIL = 0x20,
    NOMATCHCLASS = 0x30,
    NOMATCHSUBCLASS = 0x40,
    NOMATCHCMD = 0x50,
    RESEREVERESPONSE = 0xFF
}APIResponseEnumType;

typedef struct _APISystemCommad
{
    WORD MSBHK_Ver_H;
    WORD MSBHK_Ver_L;
    BYTE WDT_Enable;
    BYTE WDT_Time100ms;
    BYTE UART_Port[3];
    BYTE I2S_Mux;
}APISYSTEMCommad;

typedef enum _UARTResponseEnumType
{
    UART_INISTATUS = 0,
    UART_CHECKSUM_OK = 0x10,
    UART_CHECKSUM_FAIL = 0x20,
    UART_IRESEREVERESPONSE = 0xFF
}UARTResponseEnumType;

#define APIRxPortNum   APIDPIRxPortNum+APIHDMIRxPortNum
#define APIDPIRxPortNum    3
#define APIHDMIRxPortNum   6
#define APIHDMITxPortNum   5

#if 1//MSTAR_INTERNAL
#define APITxPortNum   	   8    // 3DP+5HDMI
#define APIAudioTxPortNum  5    // 3DP+2HDMI
#else
#define APITxPortNum       6    // 1DP+5HDMI
#define APIAudioTxPortNum  3    // DP+2HDMI
#endif

typedef struct _APIComboCommad
{
    BYTE MuxSet[APITxPortNum][9];
    BYTE AudioMuxSet[APIAudioTxPortNum][3];
    BYTE RxPortEnable[APIRxPortNum];
    BYTE TxPortEnable[APITxPortNum];
    BYTE TXPatternGEN[APITxPortNum][2];
    BYTE RxHPD[APIRxPortNum];
    BYTE RepeaterMode[2][4];
    BYTE RxEQMode[APIRxPortNum];
    BYTE RxEQCap[APIRxPortNum];
    BYTE RxEDIDMode[APIRxPortNum];
    BYTE RxEDIDTable[APIRxPortNum];
    BYTE RxHDCPMode[APIRxPortNum];
    BYTE TxHDCPMode[APITxPortNum];
}APICOMBOCommad;

typedef struct _APIHDMICommad
{
    BYTE TxHDREnable[APIHDMIRxPortNum];
    BYTE TxSwing[APIHDMIRxPortNum];
    BYTE TxTemination[APIHDMIRxPortNum];
    BYTE ARC;
    BYTE CEC;
    BYTE SgnCheck[2][2][4];
}APIHDMICommad;

typedef struct _APIDPCommad
{
    BYTE RxVersion[APIDPIRxPortNum];
    BYTE RxMSTEnable[APIDPIRxPortNum];
    BYTE RxDSCMode[APIDPIRxPortNum];
    BYTE RxDSCPort;
}APIDPCommad;

typedef struct _APICommadType
{
    APISYSTEMCommad stSystemCommand;
    APICOMBOCommad stComboCommand;
    APIHDMICommad stHDMICommand;
    APIDPCommad stDPCommand;
}APICommadType;

typedef enum
{
    POWER_ON = 0,
    POWER_DPMS,
    POWER_DCOFF
}EN_PM_MODE;

typedef enum
{
    BR_9600   =  0,
    BR_38400  =  1,
    BR_57600  =  2,
    BR_115200 =  3
}EN_UART_BAUD_RATE;

/////////////////////////////////////////////////////////

INTERFACE XDATA APICommadType g_ST_APICommand;
#if MS_PM
INTERFACE XDATA ePMStatus ucMSBHK_WakeupStatus;
#endif

INTERFACE BYTE MSBHK_RxHandler(void);
INTERFACE BYTE msAPI_MSBHK_SetCmd( BYTE Class, BYTE SubClass, BYTE Cmd, WORD DataSize, BYTE* Data, WORD* ReplyDataSize, BYTE* ReplyData);
INTERFACE BYTE msAPI_MSBHK_GetVersion(WORD* Ver_H, WORD* Ver_L);
INTERFACE void msAPI_MSBHK_SetPMMode(EN_PM_MODE Mode, WORD Port);
INTERFACE BYTE msdrv_MSBHK_ParsingCommand( BYTE Class, BYTE SubClass, BYTE Cmd, WORD DataSize, BYTE* RxData, BYTE* ReturnData);
INTERFACE BYTE msdrv_MSBHK_ParsingNoticeCommand(BYTE SubClass, BYTE Cmd, WORD DataSize, BYTE* RxData);
INTERFACE BYTE msdrv_MSBHK_UartRxDecode(WORD RxIndex, BYTE* RxFIFO, BYTE* RxClass, BYTE* RxSubClass, BYTE* RxCmd, WORD* RxDataSize, BYTE* RxData );
INTERFACE BYTE msdrv_MSBHK_UartTxEncode(BYTE Seq, BYTE Class, BYTE SubClass, BYTE Cmd, WORD Data_Size, BYTE* Data, WORD* TxFIFOSize, BYTE* TxFIFO);
INTERFACE void msdrv_MSBHK_UartTxSend(BYTE *TxFIFO, WORD TxFIFOSize);
INTERFACE void MSBHKeDummy(void);

#undef INTERFACE

#endif

