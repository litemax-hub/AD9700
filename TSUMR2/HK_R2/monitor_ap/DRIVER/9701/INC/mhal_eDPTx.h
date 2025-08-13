///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   mhal_eDPTx.h
/// @author MStar Semiconductor Inc.
/// @brief  DP driver Function
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MHAL_EDPTX_H_
#define _MHAL_EDPTX_H_
//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define eDPTX_DEBUG 1
#define eDPTXHBR3       0x1E
#define eDPTXHBR25      0x19
#define eDPTXHBR2       0x14
#define eDPTXHBR        0x0A
#define eDPTXRBR        0x06
#define eDPTX4Lanes  4
#define eDPTX2Lanes  2
#define eDPTX1Lanes  1
#define eDPTX_Efuse_En  1
#define eDPTX_AUX_efuse_en  1
#define eDPTX_RTERM_efuse_en  1


#define eDPTX_Port_Offset_100 0x100
#define eDPTX_Port_Offset_200 0x200
#define eDPTX_Port_Offset_300 0x300

#define eDPTX_AUX_Offset        0x100
#define eDPTX_TRANS_Offset      0x100
#define eDPTX_PHY_Offset        0x100
#define eDPTX_ENCODER_Offset        0x200

//Aux
#define AUX_CMD_I2C_W_MOT0      0x00
#define AUX_CMD_I2C_R_MOT0      0x01
#define AUX_CMD_I2C_W           0x04
#define AUX_CMD_I2C_R           0x05
#define AUX_CMD_NATIVE_W        0x08
#define AUX_CMD_NATIVE_R        0x09
#define AUX_EDID_SLAVE_ADDR     0x50
#define AUX_EDID_SEGMENT_ADDR   0x30
#define AUX_MCCS_SLAVE_ADDR     0x37
#define AUX_NEW_MODE_EN     0


#define eDPTXHPD_IRQEvent           (BIT2)//4P_mode : (BIT8)
#define eDPTXHPD_IRQDisConnect      (BIT1)//4P_mode : (BIT6)
#define eDPTXHPD_IRQConnect         (BIT0)//4P_mode : (BIT7)



#define DP_HDCP_ADDR_Bksv       0x68000
#define DP_HDCP_ADDR_R0_        0x68005 //R0'
#define DP_HDCP_ADDR_Aksv       0x68007
#define DP_HDCP_ADDR_An         0x6800C
#define DP_HDCP_ADDR_V_H0       0x68014 //V'Ho
#define DP_HDCP_ADDR_V_H1       0x68018 //V'H1
#define DP_HDCP_ADDR_V_H2       0x6801C //V'H2
#define DP_HDCP_ADDR_V_H3       0x68020 //V'H3
#define DP_HDCP_ADDR_V_H4       0x68024 //V'H4
#define DP_HDCP_ADDR_Bcaps      0x68028
#define DP_HDCP_ADDR_Bstatus    0x68029
#define DP_HDCP_ADDR_Binfo      0x6802A
#define DP_HDCP_ADDR_KSV_FIFO   0x6802C
#define DP_HDCP_ADDR_Ainfo      0x6803B

#define eDPMCCSADDR 0x000037

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

typedef enum _eDPTx_ID{
    eDPTx_ID_0 = 0x0,
    eDPTx_ID_1 = 0x1,
    eDPTx_ID_2 = 0x2,
    eDPTx_ID_3 = 0x3,
    eDPTx_ID_MAX,
}eDPTx_ID;

typedef struct
{
    BYTE ucCV;
    BOOL bCID;

 }eDP_SYSINFO,*PDP_SYSINFO;


typedef enum {
    eDPTx_Color_RGB = 0x00,
    eDPTx_Color_YUV422 = 0x01,
    eDPTx_Color_YUV444 =0x02,
    eDPTx_Color_YUV420 =0x03,
}eDPTx_ColorFormat;

typedef struct
{
    BYTE                        u8TotalPorts;
    BYTE                        u8PortSelect;
    BYTE                        u8LinkRate;
    BYTE                        u8LaneCount;
    BYTE                        u8AuxPNSwap;
    BYTE                        u8LanePNSwap;
    BYTE                        u8LaneSwap;
    BYTE                        u8PortSWAP;
    BYTE                        u8LaneSkewSwap;
    BYTE                        u8SSCModulation;
    BYTE                        u8SSCPercentage;
    BYTE                        u8ColorDepth;
    Bool                        bEnhanceFrameMode;
    Bool                        bASSR;
}ST_EDPTX_INFO;

extern ST_EDPTX_INFO g_eDPTxInfo;
//-------------------------------------------------------------------------------------------------
//  Function Prototype
//-------------------------------------------------------------------------------------------------
void mhal_eDPTx_CheckIDInfo(void);
void mhal_eDPTx_TU_Overwrite(WORD us100TimesTU);
void mhal_eDPTx_SetHPDOV(eDPTx_ID edptx_id);
void mhal_eDPTx_SetHPDOVHigh(eDPTx_ID edptx_id);
void mhal_eDPTx_SetAtop(eDPTx_ID edptx_id);
void mhal_eDPTx_AuxPNSwap(eDPTx_ID edptx_id);
void mhal_eDPTx_AuxClkDebunce(eDPTx_ID edptx_id);
void mhal_eDPTx_PHYInit(eDPTx_ID edptx_id);
void mhal_eDPTx_OVLaneFunction(void);
void mhal_eDPTx_MACInit(eDPTx_ID edptx_id);
void mhal_eDPTx_LanePNSwap(eDPTx_ID edptx_id);
void mhal_eDPTx_LaneSwap(eDPTx_ID edptx_id, BOOL bDataLaneSwapEn, BOOL bSkewSwapEn);
void mhal_eDPTx_PatternSkew(eDPTx_ID edptx_id, BOOL bENABLE);
void mhal_eDPTx_ConfigLinkRate(BYTE LinkRate, eDPTx_ID edptx_id);
void mhal_eDPTx_HBR2ConfigLinkRate(BYTE LinkRate, eDPTx_ID edptx_id);
void mhal_eDPTx_SSCEnable(eDPTx_ID edptx_id, BYTE LinkRate, BYTE bEnable);
void mhal_eDPTx_ColorMISC(eDPTx_ID edptx_id);
void mhal_eDPTx_ColorDepth(eDPTx_ID edptx_id);
void mhal_eDPTx_SetFreeSyncOn(eDPTx_ID edptx_id);
void mhal_eDPTx_FineTuneTU_Disable(eDPTx_ID edptx_id);
void mhal_eDPTx_FineTuneTU(eDPTx_ID edptx_id, DWORD eDPTX_OUTBL_PixRateKhz);
void mhal_eDPTx_TimingSet(eDPTx_ID edptx_id);
void mhal_eDPTx_VideoPG(eDPTx_ID edptx_id);
void mhal_eDPTx_PGEnable( eDPTx_ID edptx_id, BOOL bEnable, BYTE u8Pattern);
void mhal_eDPTx_FsyncSet(void);
void mhal_eDPTx_AuxInit(eDPTx_ID edptx_id);
void mhal_eDPTx_AuxClearIRQ(eDPTx_ID edptx_id);
void mhal_eDPTx_MuxSwitch(eDPTx_ID edptx_id);
void mhal_eDPTx_AuxReadCMDByte(eDPTx_ID edptx_id, DWORD DPCDADDR);
void mhal_eDPTx_AuxReadCMDBytes(eDPTx_ID edptx_id, BYTE Length, DWORD DPCDADDR);
Bool mhal_eDPTx_CheckWritePoint(eDPTx_ID edptx_id);
Bool mhal_eDPTx_CheckAuxRxFull(eDPTx_ID edptx_id);
Bool mhal_eDPTx_AuxRxReplyCMD(eDPTx_ID edptx_id);
void mhal_eDPTx_TxComplete(eDPTx_ID edptx_id);
void mhal_eDPTx_TrigerReadPulse0(eDPTx_ID edptx_id);
void mhal_eDPTx_TrigerReadPulse1(eDPTx_ID edptx_id);
BYTE mhal_eDPTx_AuxReadData(eDPTx_ID edptx_id);
void mhal_eDPTx_AuxWriteCMDByte(eDPTx_ID edptx_id, DWORD DPCDADDR, BYTE wData);
void mhal_eDPTx_AuxWriteCMDBytes(eDPTx_ID edptx_id, DWORD DPCDADDR);
Bool mhal_eDPTx_AuxRXCompleteInterrupt(eDPTx_ID edptx_id);
Bool mhal_eDPTx_AuxWriteCheckComplete(eDPTx_ID edptx_id);
void mhal_eTPTx_AUXTXWriteBuffer(eDPTx_ID edptx_id, BYTE LengthCnt ,BYTE *pTxBuf);
void mhal_eTPTx_AUXTXWriteData(eDPTx_ID edptx_id, BYTE Length);
void mhal_eTPTx_AUXTXWriteDataBytes(eDPTx_ID edptx_id, BYTE Length, BYTE EOF);
void mhal_eDPTx_AuxEDIDReadNLCMD(eDPTx_ID edptx_id, DWORD DPCDADDR);
void mhal_eDPTx_AuxEDIDWriteByteCMD(eDPTx_ID edptx_id, DWORD DPCDADDR, BYTE wData);
void mhal_eDPTx_AuxEDIDReadBytesCMD(eDPTx_ID edptx_id, DWORD DPCDADDR, BYTE Length);
void mhal_eDPTx_AuxMCCSReadBytesCMD( eDPTx_ID edptx_id, BYTE Length, BYTE EOF);
void mhal_eDPTx_AuxMCCSWriteBytesCMD( eDPTx_ID edptx_id, BYTE EOF);
void mhal_eDPTx_HPDInitSet(eDPTx_ID edptx_id);
void mhal_eDPTx_MLSignalDisable(eDPTx_ID edptx_id);
void mhal_eDPTx_MLSignalEnable(eDPTx_ID edptx_id);
void mhal_eDPTx_PowerDown(eDPTx_ID edptx_id);
void mhal_eDPTx_ASSR(eDPTx_ID edptx_id, BOOL bENABLE);
void mhal_eDPTx_AFR(eDPTx_ID edptx_id, BOOL bENABLE);
void mhal_eDPTx_EnhanceFrameMode(eDPTx_ID edptx_id, BOOL bENABLE);
void mhal_eDPTx_SetLaneCnt(eDPTx_ID edptx_id, BYTE LaneCount);
void mhal_eDPTx_LaneDataOverWriteEnable(eDPTx_ID edptx_id, BOOL bENABLE);
void mhal_eDPTx_SetPHY_IdlePattern(eDPTx_ID edptx_id, BOOL bENABLE);
void mhal_eDPTx_TrainingPattern_Select(BYTE TP_value, eDPTx_ID edptx_id);
void mhal_eDPTx_TBCFIFO_Overwrite(void);
void mhal_eDPTx_FMTOutputEnable(void);
WORD mhal_eDPTx_HPDStatus(WORD HPDStatus, eDPTx_ID edptx_id);
Bool mhal_eDPTx_HPDValues(eDPTx_ID edptx_id);
void mhal_eDPTx_HPDClearIRQBitAll(eDPTx_ID edptx_id);
void mhal_eDPTx_HPDClearIRQConnect(eDPTx_ID edptx_id);
void mhal_eDPTx_HPDClearIRQDisconnect(eDPTx_ID edptx_id);
void mhal_eDPTx_HPDClearIRQEvent(eDPTx_ID edptx_id);
void mhal_eDPTx_HBR2DRVMAINTune(BYTE LaneNum, BYTE SetValue, eDPTx_ID edptx_id);
void mhal_eDPTx_HBR2DRVPRETune(BYTE LaneNum, BYTE SetValue, eDPTx_ID edptx_id);
void mhal_eDPTx_HBR2PREDRVMAINTune(BYTE LaneNum, BYTE SetValue, eDPTx_ID edptx_id);
void mhal_eDPTx_HBR2PREDRVPRETune(BYTE LaneNum, BYTE SetValue, eDPTx_ID edptx_id);
void mhal_eDPTx_MODSwingTune(BYTE LaneNum, BYTE SwingValue, BYTE edptx_id);
void mhal_eDPTx_MODPreEmphasisTune(BYTE LaneNum, BYTE PremphasisValue, BYTE edptx_id);
void mhal_eDPTx_PRBS7Disable(eDPTx_ID edptx_id);
void mhal_eDPTx_PRBS7Enable(eDPTx_ID edptx_id);
void mhal_eDPTx_EyePatternDisable(eDPTx_ID edptx_id);
void mhal_eDPTx_ProgramPatternEnable(eDPTx_ID edptx_id);
void mhal_eDPTx_EyePatternEnable(eDPTx_ID edptx_id);
DWORD mhal_eDPTx_CheckHPD(eDPTx_ID edptx_id);
WORD mhal_eDPTx_CheckHPDIRQ(eDPTx_ID edptx_id);
void mhal_eDPTx_PGSyncRst(void);
void mhal_eDPTx_VDClkRst(void);
void mhal_eDPTx_HPDIRQRst(eDPTx_ID edptx_id);
void mhal_eDPTx_EfuseAuxTrimSetting(eDPTx_ID edptx_id);
void mhal_eDPTx_EfuseRtermTrimSetting(eDPTx_ID edptx_id);
void mhal_eDPTx_EfuseIbiasTrimSetting(eDPTx_ID edptx_id);
void  mhal_eDPTx_ReadEFuseFlow(void);
void  mhal_eDPTx_HDCPAnCode(eDPTx_ID edptx_id, BYTE i, BYTE ubAnValue);
void  mhal_eDPTx_HDCPInCode(eDPTx_ID edptx_id, BYTE i, BYTE Lm);
void  mhal_eDPTx_HDCPInSeed(eDPTx_ID edptx_id, BYTE temp);
void  mhal_eDPTx_HDCPCipherAuth(eDPTx_ID edptx_id);
void  mhal_eDPTx_HDCPr0Available(eDPTx_ID edptx_id);
BYTE  mhal_eDPTx_HDCPr0L(eDPTx_ID edptx_id);
BYTE  mhal_eDPTx_HDCPr0H(eDPTx_ID edptx_id);
void  mhal_eDPTx_HDCPFrameEn(eDPTx_ID edptx_id);

#endif


