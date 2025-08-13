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
    AUDIO_LINE_IN = 1,
    AUDIO_RESERVED = 2,
} AUDIO_SOURCE_SEL;

typedef enum
{
    AUDIO_I2S_FORMAT_I2S_JUSTIFY,
    AUDIO_I2S_FORMAT_LEFT_JUSTIFY,
}AUDIO_I2S_FORMAT_SEL;

typedef enum
{
    AUDIO_I2S_BCK_AU32FS,           // AUPLL 32FS
    AUDIO_I2S_BCK_AU48FS,           // AUPLL 48FS
    AUDIO_I2S_BCK_AU64FS,           // AUPLL 64FS
    AUDIO_I2S_BCK_RESERVED,
}AUDIO_I2S_BCK_SEL;

typedef enum
{
    AUDIO_I2S_MCK_AU64FS,           // AUPLL 64FS
    AUDIO_I2S_MCK_AU128FS,          // AUPLL 128FS
    AUDIO_I2S_MCK_AU256FS,          // AUPLL 256FS
    AUDIO_I2S_MCK_AU384FS,          // AUPLL 384FS
    AUDIO_I2S_MCK_AU512FS,          // AUPLL 512FS
    AUDIO_I2S_MCK_AU768FS,          // AUPLL 768FS

    AUDIO_I2S_MCK_REVERSE,
}AUDIO_I2S_MCK_SEL;


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
extern volatile BOOL audio_EnableAudioAfterSignalLock_Flag;
extern volatile U16 u16TimerAudio;


//-------------------------------------------------------------------------------------------------
//  Function Prototype
//-------------------------------------------------------------------------------------------------
extern void msDrvAudioSourceSel(AUDIO_SOURCE_SEL src);

#if ENABLE_SPDIF
extern void msAPI_AudioSpdifMute(BOOL bEnable);
#endif

extern void msAPI_AudioInit(BOOL bACon);
extern BOOL msAPI_AudioInitDone(void);
extern void msAPI_AudioSetDPGA(AUDIO_CH_SEL eChSelect, WORD u16DPGAGain);
extern void msAPI_AudioSetVolume(AUDIO_CH_SEL eChSelect, short iVolume_Integer, AUDIO_VOLUME_FRACTION_PART Volume_Fraction);
extern void msAPI_AudioSetDpgaMute(Bool enable);
extern void msAPI_AudioSourceSel(void);
extern void msAPI_AudioHandler(void);
extern void msAPI_AudioDACPowerDown(Bool bPowerDN);
extern void msAPI_AudioSineGen(Bool bEnable, AUDIO_PCM_FREQ eSinGenFreq, char u8SinGenGAIN);
extern void msAPI_AudioI2SConfig(AUDIO_I2S_FORMAT_SEL eI2SFormat, AUDIO_I2S_BCK_SEL eI2SBCK, AUDIO_I2S_MCK_SEL eI2SMCK);
extern void msAPI_AudioHDMISwap(BOOL bEnable);
extern void msAPI_AudioSDMSwap(BOOL bEnable);


#endif //_MSDAC_H_

