
#ifndef _DRVMSTAR_H_
#define _DRVMSTAR_H_

#ifdef _DRVMSTAR_C_
#define _DRVMSTARDEC_
#else
#define _DRVMSTARDEC_   extern
#endif

#ifndef USE_MOD_HW_CAL
#define USE_MOD_HW_CAL        (1)   // coding test// choose hardware(1) or software(0) MOD calibration
#endif

#if ENABLE_AUTOEQ
extern Bool xdata AUTOEQ_FLAG;
#endif

#ifndef I_GEN_CH //use a defined port
  #if PanelDualPort!=0
    #define I_GEN_CH  CH2
  #else
    #if PanelSwapPort!=0
      #define I_GEN_CH  CH6 // need check
    #else
      #define I_GEN_CH  CH2 // need check
    #endif
  #endif
#endif

#if ENABLE_SUPER_RESOLUTION
#define PANEL_SHIFT_DE_V    2
#else
#ifdef _HW_AUTO_NO_SIGNAL_
#define PANEL_SHIFT_DE_V    1
#else
#define PANEL_SHIFT_DE_V    0
#endif
#endif

#ifndef LVDS_XSWING_CH //LVDS double swing ch
#define LVDS_XSWING_CH  0 // Bit0~Bit9 maps to ch0~ch9
#endif

#define HDMI_VCO_LOOP_DIV2              145
#define HDMI_VCO_LOOP_DIV4              72
#define HDMI_VCO_LOOP_DIV8              36
#define HDMI_VCO_CODE_CALCULATE(a)      (a *64 /14.318)
#define HDMI_EQ_VALUE                   0x0A
#define HDMI_R_CHANNEL_EQ               HDMI_EQ_VALUE
#define HDMI_G_CHANNEL_EQ               HDMI_EQ_VALUE
#define HDMI_B_CHANNEL_EQ               HDMI_EQ_VALUE
#define HDMI_CLK_CHANNEL_EQ             HDMI_EQ_VALUE
#define HDMI_PLL_LDO_VALUE              BIT12

_DRVMSTARDEC_ void Init_FRORefTrim(void);
_DRVMSTARDEC_ void Init_USBXCVRRefTrim(void);
_DRVMSTARDEC_ void msInitClockGating(BOOL bEnable);
_DRVMSTARDEC_ void msTMDSInit(void);
//_DRVMSTARDEC_ void TMDSISR(void);
_DRVMSTARDEC_ void drvmStar_Init(Bool bACon );
_DRVMSTARDEC_ void drvmStar_InitPnl( void );
_DRVMSTARDEC_ void mStar_InterlaceModeSetting( void );
_DRVMSTARDEC_ void mStar_SetPanelSSC( BYTE freqMod, BYTE range ); //2011.9.13 18:33 CC
_DRVMSTARDEC_ void mStar_IPPowerControl( void );
#if 0 // no use
_DRVMSTARDEC_ void msTMDSSetMux( InputPortType inport );
#endif
_DRVMSTARDEC_ void msDrvIGenTuning( void );
_DRVMSTARDEC_ void msDrvPowerModCtrl( BYTE ucSwitch );
_DRVMSTARDEC_ BYTE mStar_ScalerDoubleBuffer( Bool u8Enable );
_DRVMSTARDEC_ void mStar_SetScalingFactor( void );
_DRVMSTARDEC_ void msDrvSetDClkPLL( DWORD u32ODCLK_KHZ );
_DRVMSTARDEC_ DWORD msDrvGetIVSPeriod( void );
_DRVMSTARDEC_ DWORD msDrvGetDClkPLLFromIVS( void );
_DRVMSTARDEC_ void mStar_CheckFastClock( WORD u16SCLK, WORD u16DCLK );
#if 0//PANEL_LVDS_4CH
_DRVMSTARDEC_ void mStar_CheckHtotalAlign(void);
#endif
_DRVMSTARDEC_ void mStar_SetScalingFilter(void);
_DRVMSTARDEC_ void drvmStar_SetupFreeRunMode( void );
_DRVMSTARDEC_ void mStar_SetTimingGen( WORD u16HDE_OP21, WORD u16VDE_OP21, BOOL OP21_FreeRun);
_DRVMSTARDEC_ void drvmStar_SetupInputPort_VGA( void );
//_DRVMSTARDEC_ void drvmStar_SetupInputPort_DVI( void );
_DRVMSTARDEC_ void drvmStar_LoadInternalEDID_VGA( void );
_DRVMSTARDEC_ void drvmStar_PowerOnInit( void );
#if ENABLE_HDMI
_DRVMSTARDEC_ void drvmStar_SetupInputPort_HDMI(void);
 #endif
#if ENABLE_CABLE_5V_EDID
_DRVMSTARDEC_ void InsertEDIDData(BYTE ucAddress, BYTE ucData);
_DRVMSTARDEC_ void drvmStar_LoadInternalEDID_VGA( void );
_DRVMSTARDEC_ void drvmStar_LoadInternalEDID_HDMI(BYTE *pEdid);
 #endif

_DRVMSTARDEC_ void msDrvMODInit(void);

typedef enum
{
    DVI_POWER_ON,
    DVI_POWER_STANDBY,
    DVI_POWER_DOWN
} DVI_PowerCtrlType;

typedef enum
{
    COMBO_INPUT_OFF,
    COMBO_INPUT_POWERSAVING,
    COMBO_INPUT_ANALOG,
    COMBO_INPUT_DIGITAL,
    COMBO_INPUT_DIGITAL_A,
    COMBO_INPUT_DIGITAL_B,
    COMBO_INPUT_DIGITAL_C,
} ComboInputType;

//_DRVMSTARDEC_ void ComboInputControl(ComboInputType ctrl);
_DRVMSTARDEC_ void drvDVI_PowerCtrl(DVI_PowerCtrlType ctrl);
//_DRVMSTARDEC_ void drvDVI_PortMuxControl(BYTE ucPort);

extern  XDATA WORD wHF0_L0, wHF0_L1, wHF1_L0, wHF1_L1, wHF2_L0, wHF2_L1;

#define SSC_STEP        0x00B2//0x0124
#define SSC_SPAN        0x013A//0x0050
#define LPLL_SSC        _DISABLE
#define IVS_DEFAULT     22//19 //12

#define FRAMEPLL_TIMEOUT    4000 // unit: mS
#define WaitOutputFpllStable()  {SetTimOutConter(FRAMEPLL_TIMEOUT);while(((msRead2Byte(REG_LPLL_54)&0x1F4) !=0x1F4 )&& (bTimeOutCounterFlag)&&(!InputTimingChangeFlag));}//if(!DDCCI_MCCS_CMD_FLAG) {g_u16u16DelayCounter=5000;while(!(msRead2Byte(REG_3854)==0x1F4) && (g_u16u16DelayCounter));}
#define WaitOutputFastLockStable()  {SetTimOutConter(FRAMEPLL_TIMEOUT);while(((msReadByte(REG_LPLL_54)&0xF4) !=0xF4 )&& (bTimeOutCounterFlag)&&(!InputTimingChangeFlag));}//if(!DDCCI_MCCS_CMD_FLAG) {g_u16u16DelayCounter=5000;while(!(msRead2Byte(REG_3854)==0x1F4) && (g_u16u16DelayCounter));}

#define PANEL_SHIFT_DE_FB_V 1

typedef struct _RegTblType
{
    WORD wReg;
    BYTE ucValue;
    BYTE ucMask;
}RegTblType;

typedef struct _RegTbl2Type
{
    DWORD dwReg;
    WORD wValue;
    WORD wMask;
}RegTbl2Type;


typedef struct _RegTblDlyType
{
    //refer to scripts format, reg, mask, value
    DWORD u32Reg;   //0x00 -> end of table, 0xffffffff -> delay time = (mask<<8|value)
    BYTE u8Mask;
    BYTE u8Value;
}RegTblDlyType;


typedef enum _FSyncMode
{
    FLM_FBL,
    FLM_FB_FPLL_NOLOCK,
    FLM_FB_FPLL,
    FLM_FBL_FRAMEPLL,
    FLM_FB_FAST_LOCK,
    FLM_FB_FAST_LOCK_AUTO_TGEN,
    FLM_FB_FPLL_FLOCK_RESET,
    FLM_FB_FPLL_FLOCK_RESET_AUTO_TGEN
}FSyncMode;

enum DVI_INPUT_PORT_TYPE
{
    DVI_INPUT_NONE = 0,
    DVI_INPUT_PORT0,
    DVI_INPUT_PORT1,
    DVI_INPUT_PORT2,
    DVI_INPUT_ALL,
    DVI_INPUT_PORT_MASK,
};

typedef enum RatioOfInputVAndOutput
{
    RatioOfInputVAndOutputVIs0205 =0,
    RatioOfInputVAndOutputVIs0512,
    RatioOfInputVAndOutputVIs0102,
    RatioOfInputVAndOutputVIs0506,
    RatioOfInputVAndOutputVIs1415,
    RatioOfInputVAndOutputVIs6566,
    RatioOfInputVAndOutputVIs0101,
    RatioOfInputVAndOutputVIs6766,
    RatioOfInputVAndOutputVIs1312,
    RatioOfInputVAndOutputVIs1110,
    RatioOfInputVAndOutputVIs6760,
    RatioOfInputVAndOutputVIs0706,
    RatioOfInputVAndOutputVIs0504,
    RatioOfInputVAndOutputVIs0605,
    RatioOfInputVAndOutputVIsNumEnd,
}RatioOfInputVAndOutput;

typedef enum
{
    MFT_NoChg,
    MFT_LR2OE,
    MFT_OE2LR,
    MFT_LR24B,
}MFT_MODE;

_DRVMSTARDEC_ void msWritePowerOnTble(void);
_DRVMSTARDEC_ BYTE msDrvMapInputToCombo(BYTE u8Input);
//_DRVMSTARDEC_ BYTE msDrvMapInputToComboIsr(BYTE u8Input);
_DRVMSTARDEC_ BYTE msDrvMapComboToPortIndex(BYTE u8Combo);
_DRVMSTARDEC_ void msSetFrameSyncMode(FSyncMode ucFrameLockMode, WORD height, WORD u16HTotal, DWORD u32ODCLK_KHZ);
_DRVMSTARDEC_ BYTE GetVfreq( void );
_DRVMSTARDEC_ void msDrvSetIVS( void );
_DRVMSTARDEC_ BOOL msDrvSetFrameDivider( BYTE vfreq_N );
_DRVMSTARDEC_ void msDrvCheckVBY1LockN(void);
_DRVMSTARDEC_ void msDrv_forceIP1VsyncPolarityEnable (void);
_DRVMSTARDEC_ void msDrvTsensorEnable(BOOL bEnable);
_DRVMSTARDEC_ WORD msDrvReadTemperature(void);
//_DRVMSTARDEC_ void Init_FROcalibrationByTsensor(void);
_DRVMSTARDEC_ void drvmStar_TimingGenInit( void );
_DRVMSTARDEC_ void drvmStar_PnlTimingSet(ST_PANELTIMING_INFO *sPnlTimingReSetup);
_DRVMSTARDEC_ ST_PANEL_INFO g_sPnlInfo;
_DRVMSTARDEC_ ST_PANEL_INFO g_sPnlInfoInit;
_DRVMSTARDEC_ BYTE msDrvGetLinkRateLoopGain(BYTE u8LinkRate);
_DRVMSTARDEC_ BYTE msDrvGetLinkRateLoopDiv(BYTE u8LinkRate);
_DRVMSTARDEC_ WORD msDrvOutputHttAlign(WORD u16Htt);
_DRVMSTARDEC_ void msDrvOutputAutoHttAlign(void);
_DRVMSTARDEC_ void msDrvResetTGen( void );
_DRVMSTARDEC_ void msDrvSetFrameRateOptimize(BOOL bEnable);

#undef _DRVMSTARDEC_
#endif

