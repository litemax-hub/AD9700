///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   msDAC.h
/// @author MStar Semiconductor Inc.
/// @brief  Audio DAC Function
///////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------------------
//NOTE
//WS:48khz , BCK:16BIT:1536khz,24BIT:2304khz,32BIT:3072khz ,MCK:0: 64fs 1:128fs 2:256fs 3:384fs  4:512fs 5:768fs
//-------------------------------------------------------------------------------------------------

#ifndef _MSDAC_H_
#define _MSDAC_H_


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
// Audio DPGA selection
typedef enum
{
    AUDIO_DPGA0                     = 0,
    AUDIO_DPGA1                     = 1,
    AUDIO_DPGA2                     = 2,
    AUDIO_DPGA3                     = 3,
    AUDIO_DPGA_ALL                  = 4,
} AUDIO_DPGA_SEL;


// Audio Volume Fraction Part
typedef enum
{
    VOLUME_FRACTION_MINUS_dot0dB    = 0,
    VOLUME_FRACTION_MINUS_dot125dB  = 1,
    VOLUME_FRACTION_MINUS_dot25dB   = 2,
    VOLUME_FRACTION_MINUS_dot375dB  = 3,
    VOLUME_FRACTION_MINUS_dot5dB    = 4,
    VOLUME_FRACTION_MINUS_dot625dB  = 5,
    VOLUME_FRACTION_MINUS_dot75dB   = 6,
    VOLUME_FRACTION_MINUS_dot875dB  = 7,
} AUDIO_VOLUME_FRACTION_PART;


// Audio channel selection
typedef enum _AUDIO_CH_SEL
{
    E_AUDIO_LEFT_CH = 0,
    E_AUDIO_RIGHT_CH = 1,
    E_AUDIO_LEFT_RIGHT_CH = 2
} AUDIO_CH_SEL;


//Internal PCM generator
typedef enum
{
    PCM_250Hz,
    PCM_500HZ,
    PCM_1KHZ,
    PCM_1500HZ,
    PCM_2KHZ,
    PCM_3KHZ,
    PCM_4KHZ,
    PCM_6KHZ,
    PCM_8KHZ,
    PCM_12KHZ,
    PCM_16KHZ
} AUDIO_PCM_FREQ;

typedef enum
{
    AUDIO_DIGITAL = 0,
    AUDIO_RESERVED = 1,    
    AUDIO_LINE_IN = 4,
} AUDIO_SOURCE_SEL;


typedef enum
{
    AUDIO_I2S_FORMAT_I2S_JUSTIFY,
    AUDIO_I2S_FORMAT_LEFT_JUSTIFY,
} AUDIO_I2S_FORMAT_SEL;


typedef enum
{
    AUDIO_I2S_BCK_AU32FS, // AUPLL 32FS
    AUDIO_I2S_BCK_AU48FS, // AUPLL 48FS
    AUDIO_I2S_BCK_AU64FS, // AUPLL 64FS
    AUDIO_I2S_BCK_RESERVED,
} AUDIO_I2S_BCK_SEL;


typedef enum
{
    AUDIO_I2S_MCK_AU128FS,
    AUDIO_I2S_MCK_AU192FS,
    AUDIO_I2S_MCK_AU256FS,
    AUDIO_I2S_MCK_AU384FS,
    AUDIO_I2S_MCK_REVERSE,
} AUDIO_I2S_MCK_SEL;


typedef enum
{
    AUD_SamplleRate_32K = 32,
    AUD_SamplleRate_44K = 44,
    AUD_SamplleRate_48K = 48,
    AUD_SamplleRate_88K = 88,
    AUD_SamplleRate_96K = 96,
    AUD_SamplleRate_176K = 176,
    AUD_SamplleRate_192K = 192,
    AUD_SamplleRate_ERROR = 0,
} EN_AUD_SamplleRate_INDEX;


typedef enum
{
    DELAY_UNIT_1MS = 0,
    DELAY_UNIT_DOT_5MS = 1,
    DELAY_UNIT_DOT_25MS = 2,
    DELAY_UNIT_DOT_125MS = 3,
} AUDIO_DELAY_MS;


typedef enum
{
    AUDIO_DMA_WR,
    AUDIO_DMA_RD,
    AUDIO_DMA_RD_TTS,
} AUDIO_DMA_ENGINE;


typedef enum
{
    AUDIO_DMA_16_BIT = 0,
    AUDIO_DMA_24_BIT,
} AUDIO_DMA_BIT_MODE;


#if ENABLE_ARC
typedef enum
{
    AUDIO_ARC_STS_OFF,
    AUDIO_ARC_STS_5V_LOST,
    AUDIO_ARC_STS_REQ_TERMINATE,    
    AUDIO_ARC_STS_REQ_INIT,
    AUDIO_ARC_STS_INIT, // ARC connected
    AUDIO_ARC_STS_ARC, // ARC set done
} AUDIO_ARC_STS;


typedef struct
{
    AUDIO_ARC_STS   eStatus;
    WORD            u16CECTimeout;
}ST_AUDIO_ARC;
#endif


extern WORD u16AudioChkTimer;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
extern volatile BOOL audio_EnableAudioAfterSignalLock_Flag;
extern volatile WORD u16TimerAudio;


//-------------------------------------------------------------------------------------------------
//  Function Prototype
//-------------------------------------------------------------------------------------------------
extern void msDrvAudioSourceSel(AUDIO_SOURCE_SEL src);

#if ENABLE_SPDIF
extern void msAPI_AudioSpdifSwap(BOOL bEnable); // SPDIF TX L/R swap
extern void msAPI_AudioSpdifMute(BOOL bEnable);
#endif

#if ENABLE_ARC
extern ST_AUDIO_ARC sAudioArc;

extern void msAPI_AudioArcInit(BOOL bEnable, AUDIO_ARC_STS eStatus);
extern void msAPI_AudioArcEnable(BOOL bEnable);
#endif

#if TTS
extern void msDrvAudioTTSInit(void);

extern BOOL msAPI_AudioTTSPlay(char *strText);
extern void msAPI_AudioTTSMixScale(BYTE u8Audio, BYTE u8TTS);
extern void msAPI_AudioTTSSpeed(WORD u16Speed);
extern void msAPI_AudioTTSMixEnable(BOOL bEnable);
extern void msAPI_AudioTTSUserEnable(BOOL bEnable);
#endif

extern BYTE msAPI_AudioInputPortGet(void);
extern void msAPI_AudioInputPortSet(BYTE u8InputPort);
extern void msAPI_AudioInit(BOOL bACon);
extern BOOL msAPI_AudioInitDone(void);
extern void msAPI_AudioSetDPGA(AUDIO_CH_SEL eChSelect, WORD u16DPGAGain);
extern void msAPI_AudioSetVolume(AUDIO_CH_SEL eChSelect, short iVolume_Integer, AUDIO_VOLUME_FRACTION_PART Volume_Fraction);
extern void msAPI_AudioSetDpgaMute(Bool enable);
extern void msAPI_AudioSourceSel(void);
extern void msAPI_AudioHandler(void);
extern void msAPI_AudioDACPowerDown(Bool bPowerDN);
extern void msAPI_AudioDelayEnable(BOOL bEnable, WORD u16DelayVal, AUDIO_DELAY_MS eDelayUnit);
extern void msAPI_AudioPathSet(void);
extern void msAPI_AudioSineGen(Bool bEnable, AUDIO_PCM_FREQ eSinGenFreq, char u8SinGenGAIN);
extern void msAPI_AudioI2SConfig(AUDIO_I2S_FORMAT_SEL eI2SFormat, AUDIO_I2S_BCK_SEL eI2SBCK, AUDIO_I2S_MCK_SEL eI2SMCK);
extern void msAPI_AudioI2SSwap(BOOL bEnable); // I2S TX L/R swap
//extern void msAPI_AudioSDMSwap(BOOL bEnable); // LINE_OUT and EAR_OUT L/R swap. Cannot use because DRE is enabled. Please use RX(HDMI and ADC) to swap L/R.
extern void msAPI_AudioHDMISwap(BOOL bEnable); // HDMI(DP) RX L/R swap
extern void msAPI_AudioADCSwap(BOOL bEnable); // LINE_IN L/R swap


#endif //_MSDAC_H_

