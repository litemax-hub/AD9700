
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "Global.h"
#include "drvDAC.h"
#include "OsdDefault.h"
#include "msEread.h"
#include "drvBDMA.h"

#if ENABLE_ARC
#include "apiCEC.h"
#endif

#if TTS
#include "mTTS.h"
#endif

#if ENABLE_DAC
//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
//#define MEM_CACHE_BYPASS(u32Addr)       (void*)(0x80000000 | (u32Addr))
#define AUDIO_DEBUG                     1
#define AUDIO_DELAY_FROCE_THRESHOLD     20  //ms
#define TBL_REG_END                     0x00
#define TBL_REG_DLY                     0xffffffff
#define TBL_REG_TAG                     0xff
#define AUDIO_CHK_TIME                  200 // unit: ms
#define AUDIO_DMA_UNIT                  0x10
#define AUDIO_DELAY_VAL                 (TTS?1:0)
#define AUDIO_DELAY_UNIT                DELAY_UNIT_1MS
#define AUDIO_DELAY_DMA_SIZE            ALIGN_16(0xF0400)


#if TTS // need to sync w/ PQ_R2
#define TTS_STR_MAX_SIZE                0x200
#define TTS_BUF_SIZE_PHY                0x202000 // 2M for TTS data, 8K for TTS reader
#else
#define TTS_STR_MAX_SIZE                0
#define TTS_BUF_SIZE_PHY                0
#endif

#if ENABLE_DEBUG && AUDIO_DEBUG
#define MSAUDIO_PRINT(format, ...)      printf(format, ##__VA_ARGS__)
#else
#define MSAUDIO_PRINT(format, ...)
#endif


//-------------------------------------------------------------------------------------------------
// Global Variable
//-------------------------------------------------------------------------------------------------
volatile WORD u16TimerAudio = 0;
volatile BOOL audio_EnableAudioAfterSignalLock_Flag = 0;


//-------------------------------------------------------------------------------------------------
// Local Variable
//-------------------------------------------------------------------------------------------------
static const RegTblDlyType* pTblAudio   = NULL;
static BOOL bTblAudioForceDelay;
static BOOL bTblAudioDone;
static BYTE u8EfuseIdMask;
static BYTE u8AudioInputPort;
static BYTE u8AudioSampleRateKHz;
static BYTE u8SourceSampleRateKHz;
static BOOL bAudioNPCM = FALSE;
static BOOL bAudioFmtChk = FALSE;
static AUDIO_SOURCE_SEL eAudioSrc;

U16 u16AudioChkTimer;

#if ENABLE_ARC
ST_AUDIO_ARC sAudioArc;
#endif

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define AUDIO_VOLUME_ARRAY_NUMBER           115
code WORD AudVolumeTab[AUDIO_VOLUME_ARRAY_NUMBER] =
{
    //   0       1       2       3       4       5       6       7       8       9     OSD   Volume_Range
    0x0000, 0x0008, 0x0010, 0x0018, 0x0020, 0x0028, 0x0030, 0x0038, 0x0040, 0x0048, // 00,    0dB ~  -9dB
    0x0050, 0x0058, 0x0060, 0x0068, 0x0070, 0x0078, 0x0080, 0x0088, 0x0090, 0x0098, // 10,  -10dB ~ -19dB
    0x00A0, 0x00A8, 0x00B0, 0x00B8, 0x00C0, 0x00C8, 0x00D0, 0x00D8, 0x00E0, 0x00E8, // 20,  -20dB ~ -29dB
    0x00F0, 0x00F8, 0x0100, 0x0108, 0x0110, 0x0118, 0x0120, 0x0128, 0x0130, 0x0138, // 30,  -30dB ~ -39dB
    0x0140, 0x0148, 0x0150, 0x0158, 0x0160, 0x0168, 0x0170, 0x0178, 0x0180, 0x0188, // 40,  -40dB ~ -49dB
    0x0190, 0x0198, 0x01A0, 0x01A8, 0x01B0, 0x01B8, 0x01C0, 0x01C8, 0x01D0, 0x01D8, // 50,  -50dB ~ -59dB
    0x01E0, 0x01E8, 0x01F0, 0x01F8, 0x0200, 0x0208, 0x0210, 0x0218, 0x0220, 0x0228, // 60,  -60dB ~ -69dB
    0x0230, 0x0238, 0x0240, 0x0248, 0x0250, 0x0258, 0x0260, 0x0268, 0x0270, 0x0278, // 70,  -70dB ~ -79dB
    0x0280, 0x0288, 0x0290, 0x0298, 0x02A0, 0x02A8, 0x02B0, 0x02B8, 0x02C0, 0x02C8, // 80,  -80dB ~ -89dB
    0x02D0, 0x02D8, 0x02E0, 0x02E8, 0x02F0, 0x02F8, 0x0300, 0x0308, 0x0310, 0x0318, // 90,  -90dB ~ -99dB
    0x0320, 0x0328, 0x0330, 0x0338, 0x0340, 0x0348, 0x0350, 0x0358, 0x0360, 0x0368, // 100,-100dB ~-109dB
    0x0370, 0x0378, 0x0380, 0x0388, 0x0390                                          // 110,-110dB ~-114dB
};


//-------------------------------------------------------------------------------------------------
//  Local Reg Table
//-------------------------------------------------------------------------------------------------
const RegTblDlyType tblAudioACOn[] =
{
    {TBL_REG_TAG, 0x00, 0xFF},
    //----------------pre-AUPLL-------------------------//
    {0x112d6a, 0xff, 0x20}, //[15:0]  2nd order synthesizer n.f[23:8]
    {0x112d6b, 0xff, 0x1c},
    {0x112d68, 0xff, 0x00}, //[7:0]   2nd order synthesizer n.f[7:0]
    {0x112d69, 0xff, 0xc0}, //[15]    2nd order synthesizer enable, [14] load
    {0x112d69, 0xff, 0x80}, //[15]    2nd order synthesizer enable, [14] load

    //----------------AUPLL control-------------------------//
    {0x112dea, 0xff, 0xd0},
    {0x112deb, 0xff, 0x12},
    {0x112df0, 0xff, 0x00},
    {0x112df1, 0xff, 0x03},

    //----------------pre-earc_tx_pll-------------------------//
    {0x112c1d, 0x07, 0x00}, //
    {0x112c1c, 0xff, 0x01}, //
    {0x112c1d, 0x10, 0x10}, //

    //----------------earc_tx_pll control-------------------------//
    {0x112df7, 0xe0, 0x00},
    {0x112df7, 0x1c, 0x10},
    {0x112df9, 0x07, 0x03},
    {0x112dfa, 0xff, 0x13},
    {0x112dfb, 0xff, 0x60},
    {0x112df6, 0x80, 0x80},
    {0x112df6, 0x07, 0x04},
    {0x112df7, 0x03, 0x02},
    {0x112df6, 0x60, 0x20},
    {0x112df8, 0xc0, 0x40},
    {0x112dfc, 0x8d, 0x00},
    {0x112dfd, 0xff, 0x28},
    {0x112dfe, 0x03, 0x00},

    //-----------------SRC 1---------`----------------------//
    {0x112c5e, 0xff, 0x00}, //SRC DAC CMP0
    {0x112c5f, 0xff, 0x00}, //SRC DAC CMP0
    {0x112c60, 0xff, 0x00}, //SRC DAC CMP0
    {0x112c61, 0xff, 0x3c}, //SRC DAC CMP0
    {0x112c62, 0xff, 0xfe}, //SRC DAC CMP1
    {0x112c63, 0xff, 0x03}, //SRC DAC CMP1
    {0x112c64, 0xff, 0x00}, //SRC DAC CMP1
    {0x112c65, 0xff, 0x90}, //SRC DAC CMP1
    {0x112c66, 0xff, 0xfd}, //SRC DAC CMP2
    {0x112c67, 0xff, 0x01}, //SRC DAC CMP2
    {0x112c68, 0xff, 0x00}, //SRC DAC CMP2
    {0x112c69, 0xff, 0x00}, //SRC DAC CMP2
    {0x112c4e, 0xff, 0x00}, //SRC ADC CMP0
    {0x112c4f, 0xff, 0x00}, //SRC ADC CMP0
    {0x112c50, 0xff, 0x00}, //SRC ADC CMP0
    {0x112c51, 0xff, 0x3c}, //SRC ADC CMP0
    {0x112c52, 0xff, 0xfe}, //SRC ADC CMP1
    {0x112c53, 0xff, 0x03}, //SRC ADC CMP1
    {0x112c54, 0xff, 0x00}, //SRC ADC CMP1
    {0x112c55, 0xff, 0x90}, //SRC ADC CMP1
    {0x112c56, 0xff, 0xfd}, //SRC ADC CMP2
    {0x112c57, 0xff, 0x01}, //SRC ADC CMP2
    {0x112c58, 0xff, 0x00}, //SRC ADC CMP2
    {0x112c59, 0xff, 0x00}, //SRC ADC CMP2
    {0x112c5a, 0xff, 0x3d}, //SRC ADC GAIN
    {0x112c5b, 0xff, 0x01}, //SRC ADC GAIN
    {0x112c5c, 0xff, 0xe4}, //SRC ADC GAIN
    {0x112c5d, 0xff, 0x32}, //SRC ADC GAIN
    {0x112c48, 0x80, 0x80}, //[7] REG_SRC_ADC_AUTO_SYNC_MODE
    {0x112c48, 0x40, 0x40}, //[6] REG_SRC_ADC_POINTER_CYC_ARST_EN
    {0x112c48, 0x20, 0x20}, //[5] REG_SRC_DAC_POINTER_CYC_ARST_EN
    {0x112c48, 0x10, 0x10}, //[4] REG_SRC_ADC_LR_SYNC_EN
    {0x112c48, 0x08, 0x08}, //[3] REG_SRC_DAC_LR_SYNC_EN

    {0x112c4d, 0x80, 0x00}, //[15] EN_DIT
    {0x112c4d, 0x40, 0x40}, //[14] AUTO_RESET
    {0x112c4d, 0x08, 0x08}, //[11] CMP_SEL
    {0x112c4d, 0x02, 0x02}, //[9]  DAC_FIFO_ARST_EN
    {0x112c4d, 0x01, 0x01}, //[8]  DAC_FIFO_NEW_MODE
    {0x112c4c, 0xf0, 0x00}, //[7:4] REG_SRC_DAC_FIFO_RESET
    {0x112c4c, 0xf0, 0xf0}, //[7:4] REG_SRC_DAC_FIFO_RESET
    {0x112c4c, 0xf0, 0x00}, //[7:4] REG_SRC_DAC_FIFO_RESET
    {0x112c49, 0x3f, 0x08}, //[13:8] REG_SRC_CNT_256FS

    //----------------IP Bypass setting-----------------//
    {0x112c05, 0x80, 0x00}, //[15] SRC bypass
    {0x112d01, 0x80, 0x80}, //[15] DMA bypass
    {0x112c87, 0x80, 0x80}, //[15] MIX bypass
    {0x112d3b, 0x80, 0x00}, //[15] DPGA bypass

    //----------------init SRAM-------------------------//
    {0x112c4c, 0xf0, 0x00}, //[7:4] REG_SRC_DAC_FIFO_RESET
    {0x112c48, 0x01, 0x01},
    {0x112c45, 0x80, 0x80},
    {0x112e5e, 0x01, 0x01},
    {0x112c0d, 0x40, 0x40}, //CLK_MAC_SRC_CKG
    {0x112c0d, 0x04, 0x04}, //CLK_MAC_CODEC_CKG
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112c48, 0x01, 0x00},
    {0x112c45, 0x80, 0x00},
    {0x112e5e, 0x01, 0x00},
    {0x112c0d, 0x40, 0x00}, //CLK_MAC_SRC_CKG
    {0x112c0d, 0x04, 0x00}, //CLK_MAC_CODEC_CKG

    //-----------------Input: Channel Source----------------//
    {0x112c05, 0x70, 0x00}, //0:HDMI, 1:SYNTH, 2:I2S Rx 3:SPDIF Rx 4:ADC

    //---------------- Enable CLK----------------------------//
    {0x112c09, 0x80, 0x00}, //[15] CLK_BCK_I2S_DECODER
    {0x112c09, 0x40, 0x00}, //[14] CLK_216MHZ_I2S_SYNTH
    {0x112c09, 0x20, 0x00}, //[13] CLK_7680FS_BCK_MS_TG
    {0x112c09, 0x10, 0x00}, //[12] CLK_216MHZ_SPDIF_RX_SYNTH
    {0x112c09, 0x08, 0x00}, //[11] CLK_128FS_SPDIF_RX
    {0x112c09, 0x04, 0x00}, //[10] CLK_SINGEN
    {0x112c09, 0x02, 0x00}, //[9 ] CLK_DBG_216MHZ
    {0x112c09, 0x01, 0x00}, //[8 ] CLK_DBG_TG_256FS
    {0x112c08, 0x80, 0x80}, //[7 ] CLK_HDMI_RX_128FS
    {0x112c08, 0x40, 0x40}, //[6 ] CLK_HDMI_RX_SYNTH
    {0x112c08, 0x20, 0x20}, //[5 ] CLK_BE_256FS
    {0x112c08, 0x10, 0x10}, //[4 ] CLK_BE_384FS
    {0x112c08, 0x08, 0x08}, //[3 ] CLK_DOWNMIX
    {0x112c08, 0x04, 0x04}, //[2 ] CLK_SPDIF2_FIFO
    {0x112c08, 0x02, 0x02}, //[1 ] CLK_SPDIF2_ENCODER
    {0x112c08, 0x01, 0x01}, //[0 ] CLK_256FS_CHANNEL_IN

    {0x112c0d, 0x80, 0x80}, //[15] CLK_SRC_200_AUDIO
    {0x112c0d, 0x40, 0x40}, //[14] CLK_SRC_MAC
    {0x112c0d, 0x20, 0x20}, //[13] CLK_SRC_252FS
    {0x112c0d, 0x10, 0x10}, //[12] CLK_SRC_256FS
    {0x112c0d, 0x08, 0x08}, //[11] CLK_CODEC_200_AUDIO
    {0x112c0d, 0x04, 0x04}, //[10] CLK_CODEC_MAC
    {0x112c0d, 0x02, 0x02}, //[9 ] CLK_CODEC_ADC
    {0x112c0d, 0x01, 0x01}, //[8 ] CLK_CODEC_DAC
    {0x112c0c, 0x80, 0x80}, //[7 ] CLK_256FS_DMA2_WRITE
    {0x112c0c, 0x40, 0x40}, //[6 ] CLK_256FS_DMA2_READ
    {0x112c0c, 0x20, 0x20}, //[5 ] CLK_DPGA_MAC
    {0x112c0c, 0x08, 0x08}, //[3 ] CLK_I2S_ENCODER_FIFO
    {0x112c0c, 0x04, 0x04}, //[2 ] CLK_I2S_ENCODER_BCK
    {0x112c0c, 0x02, 0x02}, //[1 ] CLK_SPDIF_FIFO
    {0x112c0c, 0x01, 0x01}, //[0 ] CLK_SPDIF_ENCODER

    {0x112c11, 0x02, 0x02}, //[9 ] CLK_ADC_256FS
    {0x112c11, 0x01, 0x01}, //[8 ] CLK_AU_DLY

    //----------------haydn reset---------------------------//
    {0x112c00, 0xff, 0xff},
    {0x112c02, 0xff, 0xff},
    {0x112c00, 0xff, 0x00},
    {0x112c02, 0xff, 0x00},

    //-----------------SRC 2---------`----------------------//
    {0x112c4c, 0x0f, 0x00}, //[3:0] REG_SRC_REQ_MASK
    {0x112c4d, 0x10, 0x10}, //[12] CIC_ENABLE

    //-----------------DMA--------------------------------//
    {0x112a20, 0x08, 0x08}, //[3] L/R swap
    {0x112a40, 0x08, 0x08}, //[3] L/R swap

    //-----------------MIXER--------------------------------//
    {0x112d58, 0xff, 0x10}, //MIX_SEL_DIN
    {0x112d59, 0xff, 0x32}, //MIX_SEL_DIN
    {0x112d5a, 0xff, 0x54}, //MIX_SEL_DIN
    {0x112d5b, 0xff, 0x76}, //MIX_SEL_DIN
    {0x112d5c, 0xff, 0x98}, //MIX_SEL_DIN
    {0x112d5d, 0xff, 0xba}, //MIX_SEL_DIN
    {0x112d5e, 0xff, 0xdc}, //MIX_SEL_DIN
    {0x112d5f, 0xff, 0xfe}, //MIX_SEL_DIN
    {0x112d60, 0x1f, 0x00}, //MIX_GAIN_A
    {0x112d61, 0x1f, 0x00}, //MIX_GAIN_B
    {0x112c87, 0x01, 0x00}, //MIX_GAIN_TRIG
    {0x112c87, 0x01, 0x01}, //MIX_GAIN_TRIG
    {0x112c87, 0x01, 0x00}, //MIX_GAIN_TRIG
    {0x112c87, 0x04, 0x04}, //MIX_EN_DIT
    {0x112c87, 0x08, 0x08}, //MIX_EN

    //-----------------DPGA---------------------------------//
    {0x112d21, 0x70, 0x60}, //[14:12] fading step
    //3'd0:    0.125db for 127+1 samples
    //3'd1:    0.125db for 63 +1 samples
    //3'd2:    0.125db for 31 +1 samples
    //3'd3:    0.125db for 15 +1 samples
    //3'd4:    0.125db for 7  +1 samples
    //3'd5:    0.125db for 3  +1 samples
    //3'd6:    0.125db for 1  +1 samples
    //3'd7:    0.125db for 0  +1 samples
    //default: 0.125db for 0  +1 samples
    {0x112d39, 0x10, 0x10}, //[12] MUTE_2_ZERO
    {0x112d23, 0x40, 0x40}, //[14] enable fading
    {0x112d23, 0x80, 0x80}, //[15] enable

    //-----------------AudioBand----------------------------//
    //ADC digital part:
    {0x112c29, 0x80, 0x80}, //[15]  EN_ADC_DITHER
    {0x112c29, 0x20, 0x00}, //[13]  REG_ADC_SWAP
    {0x112c29, 0x10, 0x00}, //[12]  REG_CMP_SEL
    {0x112c29, 0x08, 0x00}, //[11]  SDM SWAP
    {0x112c29, 0x04, 0x00}, //[10]  SDM INV
    {0x112c29, 0x03, 0x00}, //[9:8] REG_DEC_NUM
    {0x112c28, 0xff, 0xd0}, //[7:0] REG_CIC_CTRL

    //DAC digital part:
    {0x112c2e, 0xff, 0x80}, //DC_OFFSET
    {0x112c2f, 0xff, 0x00}, //DC_OFFSET
    {0x112c2d, 0x80, 0x80}, //[15   ] DC_OFFSET_EN
    {0x112c30, 0x80, 0x80}, //[7    ] DITHER_EN
    {0x112c30, 0x0c, 0x08}, //[ 3: 2] DITHER_SEL 17L: x0.5
    {0x112c2c, 0x10, 0x10}, //[4    ] FIX_MSB_EN
    {0x112c2d, 0x01, 0x01}, //[8]   ] SDM_EN
    {0x112E60, 0x10, 0x10}, //CMP_SEL, decreasing gain of DAC up-sampling filter.
    {0x112E02, 0xff, 0xCD},
    {0x112E03, 0xff, 0x39},
    {0x112E04, 0xff, 0x00},
    {0x112E05, 0xff, 0x00},
    {0x112E06, 0xff, 0x80},
    {0x112E07, 0xff, 0x9D},
    {0x112E08, 0xff, 0xFE},
    {0x112E09, 0xff, 0x03},
    {0x112E0A, 0xff, 0x73},
    {0x112E0B, 0xff, 0x54},
    {0x112E0C, 0xff, 0xEA},
    {0x112E0D, 0xff, 0x01},

	//-----------------DRE----------------------------------//
    //The fastest power detection.
    {0x112E7A, 0xff, 0x00},
    {0x112E7B, 0xff, 0x00},
    {0x112E7C, 0xff, 0x01},
    {0x112E7D, 0xff, 0x00},
    {0x112E7E, 0xff, 0x00},
    {0x112E7F, 0xff, 0x00},
    {0x112E80, 0xff, 0x01},
    {0x112E81, 0xff, 0x00},
    {0x112E82, 0xff, 0x00},
    {0x112E83, 0xff, 0x00},
    {0x112E84, 0xff, 0x01},
    {0x112E85, 0xff, 0x00},
    {0x112E86, 0xff, 0x00},
    {0x112E87, 0xff, 0x00},
    {0x112E88, 0xff, 0x01},
    {0x112E89, 0xff, 0x00},
    {0x112E8A, 0xff, 0x00},
    {0x112E8B, 0xff, 0x00},
    {0x112E8C, 0xff, 0x01},
    {0x112E8D, 0xff, 0x00},
    {0x112E8E, 0xff, 0x00},
    {0x112E8F, 0xff, 0x00},
    {0x112E90, 0xff, 0x01},
    {0x112E91, 0xff, 0x00},
    {0x112E92, 0xff, 0x00},
    {0x112E93, 0xff, 0x00},
    {0x112E94, 0xff, 0x01},
    {0x112E95, 0xff, 0x00},
    {0x112E96, 0xff, 0x00},
    {0x112E97, 0xff, 0x00},
    {0x112E98, 0xff, 0x01},
    {0x112E99, 0xff, 0x00},

    //-----------------De-POP On----------------------------//
    {0x112e61, 0x0f, 0x00}, //DRE disable

    //-----------------Input: ADC---------------------------//
    {0x112cdc, 0x01, 0x00}, //[0]   PD_INMUX0
    {0x112ce3, 0x02, 0x02}, //[9]   EN_CHOP_ADC0
    {0x112ce3, 0x20, 0x20}, //[13]  EN_DIT_ADC0
    {0x112ce8, 0x80, 0x00}, //[9:7] INMUX_GAIN
    {0x112ce9, 0x03, 0x00}, // 0:0dB, 1:-6dB, 2:-3dB(default), 3:3dB, 4:6dB, 5:9dB, 6:12dB
    {0x112ce0, 0xe0, 0x00}, //[7:5] SEL_CH_INMUX
    {0x112cee, 0x18, 0x18}, //[4:3] SEL_DIT_VOL_L_ADC0 SEL_DIT_VOL_R_ADC0

    //-----------------Input: HDMI (only haydn part)--------//
    {0x112b20, 0x1f, 0x00}, //[4:0]  HDMI_CH_MATRIX_CFG_W
    {0x112b21, 0x1f, 0x00}, //[12:8] HDMI_CH_MATRIX_WD
    {0x112b20, 0x1f, 0x01}, //[4:0]  HDMI_CH_MATRIX_CFG_W
    {0x112b21, 0x1f, 0x01}, //[12:8] HDMI_CH_MATRIX_WD
    {0x112b24, 0x7f, 0x7f}, //[6:0]  HDMI RX LAUNCH CFG
    {0x112be2, 0x30, 0x20}, //[5:4]  HDMI status SEL
    {0x112be2, 0x07, 0x00}, //[2:0]  HDMI status channel SEL
    {0x112be2, 0x08, 0x08}, //[3]    HDMI status left justified
    {0x112be3, 0x01, 0x00}, //[8]    HDMI status MSB first
    {0x112be0, 0x3f, 0x03}, //[5:0]  HDMI status block start SEL
    {0x112be4, 0x10, 0x10}, //[4]    HDMI npcm detect byte mode
    {0x112be4, 0x08, 0x08}, //[3]    HDMI npcm detect PA/PB order
    {0x112be4, 0x04, 0x00}, //[2]    HDMI npcm detect fast lock
    {0x112be4, 0x03, 0x00}, //[1:0]  HDMI npcm detect time out SEL

    //-----------------Output: I2S (BCK 64fs, msater)-------//
    //I2S out
    {0x112c82, 0x20, 0x20}, //[5]     BCK invert
    {0x112c81, 0x80, 0x80}, //[15]    I2S_OUT_FMT
    {0x112c81, 0x60, 0x40}, //[14:13] REG_I2S_ENC_WDTH
    {0x112c80, 0x08, 0x00}, //[3]     I2S_OUT_OEZ
    {0x112c80, 0x04, 0x00}, //[2]     I2S_OUT_MUTE_OEZ
    {0x112c82, 0x07, 0x02}, //[2:0]   SEL_CLK_I2S_MCK 256FS

    //-----------------Output: SPDIF TX---------------------//
    {0x112c9a, 0x20, 0x20}, //[5]  REG_EN_AutoBlkStart
    {0x112c9a, 0x40, 0x40}, //[6]  REG_VALIDITY_BIT_SPDIF
    {0x112c9c, 0x04, 0x04}, //[2]  REG_SPDIF_CS_FR_HDMIRX
    {0x112c9c, 0x08, 0x08}, //[3]  REG_SPDIF_CS_VALID_SEL
    {0x112c9b, 0x03, 0x00}, //[9:8]REG_SEL_SPDIF_CH
    {0x112c9c, 0x40, 0x40}, //[6]  REG_SPDIF_OUT_FR_DPGA
    {0x112cb8, 0x20, 0x20}, //[5]  REG_SPDIF_OUT_CS_MODE
    {0x112cb8, 0x80, 0x80}, //[7]  REG_SPDIF_OUT_US_MODE
    {0x112cb9, 0x03, 0x02}, //[9:8]REG_SPDIF_OUT_SEL
    {0x112c99, 0x80, 0x80}, //[15] SPDIF_OUT_OEZ

#if (ENABLE_LINE_OUT || ENABLE_EAR_OUT || ENABLE_LINE_IN)
    {0x112c2d, 0x20, 0x00}, //DC offset disable
    //{0x112e63, 0x1f, 0x00}, // REG_GAIN_IDX_FORCE_VAL_L
    //{0x112e62, 0x1f, 0x00}, // REG_GAIN_IDX_FORCE_VAL_R

    //---------------------------De-POP On---------------------------//
    //{0x112e5e, 0x10, 0x10}, // force register DRE
    //{0x112e63, 0x1f, 0x01}, // REG_GAIN_IDX_FORCE_VAL_L
    //{0x112e62, 0x1f, 0x01}, // REG_GAIN_IDX_FORCE_VAL_R

    // DEPOP ON START

#if ENABLE_LINE_OUT
    {0x112cf2, 0xff, 0x7f}, //GCR_TEST_AUSDM[7:0]=ff
#endif


    {0x112cf0, 0x10, 0x10}, //EN_MUTE_EAR=1
    {0x112ce4, 0x04, 0x04}, //EN_MUTE_LINE=1
    {0x112cf1, 0x18, 0x18}, //EN_CK_DAC[1:0]=11
    {0x112cdb, 0x04, 0x00}, //PD_IBIAS_DAC=0


#if ENABLE_LINE_OUT
    //{0x112cf2, 0x7f, 0x3f}, //GCR_TEST_AUSDM[7:0]=7f
    {0x000b02, 0x02, 0x00}, //EN_VMID2GND_LINE=0
#endif


    // DEPOP ON LINE : 0m //
#if ENABLE_EAR_OUT
    {0x112cef, 0x01, 0x01}, //EN_CHOP_EAR=1
#endif

#if ENABLE_LINE_OUT
    {0x112cef, 0x02, 0x02}, //EN_CHOP_LINE=1
#endif

    {0x112cdd, 0x30, 0x30}, //SEL_CHOP_FREQ_DAC=11
    {0x112cdd, 0x01, 0x01}, //EN_QS_LDO_DAC=1
    {0x112ce1, 0xc0, 0xc0}, //EN_CHOP_PHASE_DAC1&2=1,1


#if ENABLE_EAR_OUT
    {0x112cda, 0x22, 0x00}, //PD_DAC_L0=0, PD_DAC_R0=0
#endif

#if ENABLE_LINE_OUT
    {0x112cda, 0x44, 0x00}, //PD_DAC_L1=0, PD_DAC_R1=0
#endif

    //{0x112cdb, 0x10, 0x00}, //PD_IBIAS_EAR=0
    //{0x112cdb, 0x60, 0x00}, //PD_LT_EAR PD_RT_EAR=0
    //{0x112cdb, 0x01, 0x00}, //PD_IBIAS_LINE=0
    //{0x112cdb, 0x80, 0x00}, //PD_LT_LINE=0


    //{0x112cdb, 0x04, 0x00}, // PD_BIAS_DAC=0
#if ENABLE_EAR_OUT
    {0x112cdb, 0x74, 0x00}, //PD_RT_EAR[13]=0, PD_LT_EAR[14]=0, PD_IBIAS_EAR[12]=0
#endif

#if ENABLE_LINE_OUT
    {0x112cdb, 0x81, 0x00}, //PD_LT_LINE[15]=0, PD_IBIAS_LINE[8]=0
    {0x112cdf, 0x80, 0x00}, //PD_RT_LINE=0
#endif

    {0x112cdc, 0x08, 0x00}, //PD_VI[3]=0
    //{0x000b00, 0x01, 0x00}, //PD_OPLP_EAR[1]=0
    //{0x000b00, 0x08, 0x00}, //PD_OPLP_LINE[2]=0
    {0x112cdc, 0xe0, 0x80}, //PD_LDO_ADC[5]=0, PD_LDO_DAC[6]=0, EN_QS_LDO_ADC[7]=1


#if ENABLE_EAR_OUT
    {0x000b00, 0x01, 0x00}, //PD_OPLP_EAR[1]=0
#endif

#if ENABLE_LINE_OUT
    {0x112ce4, 0x18, 0x08}, //SEL_TC_LINE[1:0]=01
    {0x000b00, 0x08, 0x00}, //PD_OPLP_LINE[2]=0
#endif


#if ENABLE_EAR_OUT
    {0x000b02, 0x01, 0x00}, //EN_VMID2GND_EAR
    {0x112cdf, 0x06, 0x02}, //SEL_IN_EAR[1:0]=01
#endif
    {0x000b03, 0x04, 0x00}, //EN_VREF_DISCH=0

#if ENABLE_LINE_OUT
    {0x112ce2, 0x0c, 0x04}, //SEL_IN_LINE[1:0]=01
    {0x112cf0, 0x80, 0x80}, //EN_STG2_LP_LINE=1
#endif

#if ENABLE_EAR_OUT
    {0x112cf0, 0x10, 0x00}, //EN_MUTE_EAR
    {0x112cf1, 0x01, 0x01}, //EN_STG2_LP=1
#endif

    {0x000b00, 0x02, 0x00}, //PD_VREF=0

    // DEPOP ON LINE  : 1m /
    {TBL_REG_DLY, 0x00, 0x01},

#if ENABLE_LINE_OUT
    {0x112ce4, 0x04, 0x00}, //EN_MUTE_LINE
#endif

    // DEPOP ON LINE  : 2m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x01}, //SEL_DEPOP_LINE[7:0]=16'h01
    // DEPOP ON LINE  : 3m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x23}, //SEL_DEPOP_LINE[7:0]=16'h23
    {0x112cd6, 0xff, 0x08}, //SEL_DEPOP_EAR[11:0]=008
    // DEPOP ON LINE  : 4m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x22}, //SEL_DEPOP_LINE[7:0]=16'h22
    {0x112cd6, 0xff, 0x0c}, //SEL_DEPOP_EAR[11:0]=00c
    // DEPOP ON LINE  : 5m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x26}, //SEL_DEPOP_LINE[7:0]=16'h26
    {0x112cd6, 0xff, 0x0e}, //SEL_DEPOP_EAR[11:0]=00e
    // DEPOP ON LINE  : 6m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x27}, //SEL_DEPOP_LINE[7:0]=16'h27
    //{0x112cd5, 0xff, 0x03}, //SEL_DEPOP_LINE[7:0]=16'h27
    {0x112cd6, 0xff, 0x2e}, //SEL_DEPOP_EAR[11:0]=32e
    {0x112cd7, 0xff, 0x03}, //SEL_DEPOP_EAR[11:0]
    // DEPOP ON LINE  : 7m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112ce9, 0x0c, 0x0c}, //SFTDCH[1:0]=11
    {0x112cd4, 0xff, 0x25}, //SEL_DEPOP_LINE[7:0]=16'h25
    {0x112cd6, 0xff, 0x6e}, //SEL_DEPOP_EAR[11:0]=36e
    // DEPOP ON LINE  : 8m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x24}, //SEL_DEPOP_LINE[7:0]=16'h24
    {0x112cd6, 0xff, 0xee}, //SEL_DEPOP_EAR[11:0]=3ee
    // DEPOP ON LINE  : 9m //
    {TBL_REG_DLY, 0x00, 0x01},

    {0x112cd4, 0xff, 0x2c}, //SEL_DEPOP_LINE[7:0]=16'h2C
    // DEPOP ON LINE  : 10m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x2f}, //SEL_DEPOP_LINE[7:0]=16'h2f
    // DEPOP ON LINE  : 11m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x2e}, //SEL_DEPOP_LINE[7:0]=16'h2e
    // DEPOP ON LINE  : 12m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x2b}, //SEL_DEPOP_LINE[7:0]=16'h2b
    // DEPOP ON LINE  : 13m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x29}, //SEL_DEPOP_LINE[7:0]=16'h29
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x28}, //SEL_DEPOP_LINE[7:0]=16'h28
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x38}, //SEL_DEPOP_LINE[7:0]=16'h38
    {TBL_REG_DLY, 0x00, 0x01},

    // DEPOP ON LINE  : 14m //
    // DEPOP ON LINE  : 17m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x39}, //SEL_DEPOP_LINE[7:0]=16'h39
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x3b}, //SEL_DEPOP_LINE[7:0]=16'h3b
    // DEPOP ON LINE  : 15m //
    // DEPOP ON LINE  : 18m //
    {TBL_REG_DLY, 0x00, 0x01},
    //{0x112cd4, 0xff, 0x3a}, //SEL_DEPOP_LINE[7:0]=16'h3a
    // DEPOP ON LINE  : 16m //
    // DEPOP ON LINE  : 19m //
    {TBL_REG_DLY, 0x00, 0x0a},
    {0x112ce5, 0x80, 0x80}, //EN_MSP=1
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112ce9, 0x0c, 0x08}, //SFTDCH[1:0]=10

    // DEPOP ON STEP 1 : 519m //
    {TBL_REG_DLY, 0x00, 0x03},

    {0x112ce9, 0x0c, 0x00}, //SFTDCH[1:0]=00

    {0x112cd8, 0xff, 0x01}, //EN_VREF_SFTMSP[7:0]=8'b0000,0001
    // DEPOP ON STEP 1 : 517m //
    // DEPOP ON STEP 1 : 569m //

    {TBL_REG_DLY, 0x00, 0x05},

    {0x112ce9, 0x0c, 0x04}, //SFTDCH[1:0]=01
    // DEPOP ON STEP 1 : 537m //
    // DEPOP ON STEP 1 : 589m //

    {TBL_REG_DLY, 0x00, 0xaa},
    {0x112cd8, 0xff, 0x01}, //EN_VREF_SFTMSP[7:0]=8'b0000,0011?
    // DEPOP ON STEP 1 : 557m //
    // DEPOP ON STEP 1 : 607m //

    {TBL_REG_DLY, 0x00, 0x0a},

    {0x112cd8, 0xff, 0x07}, //EN_VREF_SFTMSP[7:0]=8'b0000,0111
    // DEPOP ON STEP 1 : 757m //
    // DEPOP ON STEP 1 : 807m //

    {TBL_REG_DLY, 0x00, 0x03},
    {0x112cd8, 0xff, 0x0f}, //EN_VREF_SFTMSP[7:0]=8'b0000,1111
    // DEPOP ON STEP 1 : 760m //
    // DEPOP ON STEP 1 : 810m //
    {TBL_REG_DLY, 0x00, 0x03},
    {0x112cd8, 0xff, 0x1f}, //EN_VREF_SFTMSP[7:0]=8'b0001,1111
    // DEPOP ON STEP 1 : 763m //
    // DEPOP ON STEP 1 : 813m //
    {TBL_REG_DLY, 0x00, 0x02},
    {0x112cd8, 0xff, 0x3f}, //EN_VREF_SFTMSP[7:0]=8'b0011,1111
    // DEPOP ON STEP 1 : 765m //
    // DEPOP ON STEP 1 : 815m //
    {TBL_REG_DLY, 0x00, 0x03},
    {0x112cd8, 0xff, 0x7f}, //EN_VREF_SFTMSP[7:0]=8'b0111,1111
    // DEPOP ON STEP 1 : 768m //
    // DEPOP ON STEP 1 : 818m //
    {TBL_REG_DLY, 0x00, 0x0a},
    {0x112cd8, 0xff, 0xff}, //EN_VREF_SFTMSP[7:0]=8'b1111,11111
    // DEPOP ON STEP 1 : 778m //
    // DEPOP ON STEP 1 : 828m //

    {TBL_REG_DLY, 0x00, 0x05},
    {0x112cd9, 0xff, 0x01}, //EN_VREF_SFTMSP[8]=1
    // DEPOP ON EAR : 783m //
    // DEPOP ON EAR : 833m //
    {TBL_REG_DLY, 0x00, 0xaf},
    {0x112cd7, 0xff, 0x07}, //SEL_DEPOP_EAR[11:0]
    // DEPOP ON EAR : 958m //
    // DEPOP ON EAR : 1008m //
    {TBL_REG_DLY, 0x00, 0x0a},
    {0x112cd6, 0xff, 0xef}, //SEL_DEPOP_EAR[11:0]=7ef
    // DEPOP ON EAR : 968m //
    // DEPOP ON EAR : 1018m //
    {TBL_REG_DLY, 0x00, 0x05},
    {0x112cd6, 0xff, 0xff}, //SEL_DEPOP_EAR[11:0]=7ff

    // DEPOP ON LINE  : 1209m //
    {TBL_REG_DLY, 0x00, 0x4b},

    {0x112cd5, 0xff, 0x60}, //SEL_DEPOP_LINE[14:8]=16'h60
    // DEPOP ON LINE  : 1160m //
    // DEPOP ON LINE  : 1210m //
    {TBL_REG_DLY, 0x00, 0x03},
    {0x112cd5, 0xff, 0x61}, //SEL_DEPOP_LINE[14:8]=16'h61
    // DEPOP ON LINE  : 1163m //
    // DEPOP ON LINE  : 1213m //
    {TBL_REG_DLY, 0x00, 0x0a},
    {0x112cd5, 0xff, 0x71}, //SEL_DEPOP_LINE[14:8]=16'h71
    // DEPOP ON LINE  : 1173m //
    // DEPOP ON LINE  : 1223m //
    {TBL_REG_DLY, 0x00, 0x02},
    {0x112cd5, 0xff, 0x79}, //SEL_DEPOP_LINE[14:8]=16'h79
    // DEPOP ON LINE  : 1175m //
    // DEPOP ON LINE  : 1225m //
    {TBL_REG_DLY, 0x00, 0x08},
    {0x112cd5, 0xff, 0x7b}, //SEL_DEPOP_LINE[14:8]=16'h7b
    // DEPOP ON LINE  : 1183m //
    // DEPOP ON LINE  : 1233m //
    {TBL_REG_DLY, 0x00, 0x05},
    {0x112cd5, 0xff, 0x7f}, //SEL_DEPOP_LINE[14:8]=16'h7f
    // DEPOP ON LINE  : 1188m //
    // DEPOP ON LINE  : 1238m //
    {TBL_REG_DLY, 0x00, 0x06},
    {0x112cd4, 0xff, 0x3e}, //SEL_DEPOP_LINE[7:0]=16'h3e
    // DEPOP ON LINE  : 1194m //
    // DEPOP ON LINE  : 1244m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x3f}, //SEL_DEPOP_LINE[7:0]=16'h3f
    // DEPOP ON LINE  : 1195m //
    // DEPOP ON LINE  : 1245m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x37}, //SEL_DEPOP_LINE[7:0]=16'h37
    // DEPOP ON STEP 1 : 1196m //
    // DEPOP ON STEP 1 : 1246m //
    {TBL_REG_DLY, 0x00, 0x07},
    {0x112ce5, 0x80, 0x00}, //EN_MSP=0
    {0x112ce9, 0x0c, 0x00}, //SFTDCH[1:0]=00
    {0x112cd8, 0xff, 0x00}, //EN_VREF_SFTMSP[7:0]=0
    {0x112cd9, 0xff, 0x00}, //EN_VREF_SFTMSP[8]=0

    // DEPOP ON LINE  : 1203m //
    // DEPOP ON LINE  : 1253m //
    {TBL_REG_DLY, 0x00, 0x0a},
    {0x112cd4, 0xff, 0xf7}, //SEL_DEPOP_LINE[7:0]=16'hf7

#if ENABLE_LINE_OUT
    {0x112ce8, 0x02, 0x02}, //EN_STG2_AB_LINE=1
#endif

    {TBL_REG_DLY, 0x00, 0x14},

#if ENABLE_EAR_OUT
    {0x112ce8, 0x01, 0x01}, //EN_STG2_AB_EAR=1
#endif

    //{TBL_REG_DLY, 0x00, 0x01},
    //{0x112ce4, 0x80, 0x00}, //SEL_TC_LINE
    //{0x112ce5, 0x01, 0x01}, //SEL_TC_LINE

    //{0x112ce5, 0x03, 0x02}, //EN_ISEL_LINE
    //{TBL_REG_DLY, 0x00, 0x0a},
    //{0x000b02, 0x02, 0x00}, //VMID_PULL_LOW_LINE=0

    {0x112cdc, 0x80, 0x00}, //EN_QS_LDO_ADC=0
    {0x112cdd, 0x01, 0x00}, //EN_QS_LDO_DAC=0
    //{0x112e5e, 0x10, 0x00}, // force register DRE

#if ENABLE_LINE_OUT
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112ce4, 0x18, 0x00}, //SEL_TC_LINE[1:0]=00

    //{TBL_REG_DLY, 0x00, 0x02},
    //{0x112cf2, 0x3f, 0x7f}, //GCR_TEST_AUSDM[7:0]

    {TBL_REG_DLY, 0x00, 0x02},
    {0x112cf2, 0x3f, 0x3f}, //GCR_TEST_AUSDM[7:0]
    {TBL_REG_DLY, 0x00, 0x02},
    {0x112cf2, 0x3f, 0x1f}, //GCR_TEST_AUSDM[7:0]
    {TBL_REG_DLY, 0x00, 0x04},
    {0x112cf2, 0x3f, 0x0f}, //GCR_TEST_AUSDM[7:0]
    {TBL_REG_DLY, 0x00, 0x04},
    {0x112cf2, 0x3f, 0x07}, //GCR_TEST_AUSDM[7:0]
    {TBL_REG_DLY, 0x00, 0x04},
    {0x112cf2, 0xff, 0x03}, //GCR_TEST_AUSDM[7:0]
    {TBL_REG_DLY, 0x00, 0x04},
    {0x112cf2, 0xff, 0x01}, //GCR_TEST_AUSDM[7:0]
    {TBL_REG_DLY, 0x00, 0x04},
    {0x112cf2, 0xff, 0x00}, //GCR_TEST_AUSDM[7:0]
#endif


    // DEPOP ON LINE  : 1213m //
    // DEPOP ON LINE  : 1298m //
    // depop on finish................

#if ENABLE_LINE_IN
    //-----------------Input: ADC---------------------------//
    {0x112cdc, 0x01, 0x00}, //[0]   PD_INMUX0
    {0x112cda, 0x01, 0x00}, //PD_ADC0
    {0x112ce3, 0x02, 0x02}, //[9]   EN_CHOP_ADC0
    {0x112ce3, 0x20, 0x20}, //[13]  EN_DIT_ADC0
    {0x112ce8, 0x80, 0x00}, //[9:7] INMUX_GAIN
    {0x112ce9, 0x03, 0x00}, //0:0dB, 1:-6dB, 2:-3dB(default), 3:3dB, 4:6dB, 5:9dB, 6:12dB
    {0x112ce0, 0xe0, 0x00}, //[7:5] SEL_CH_INMUX
    {0x112cee, 0x18, 0x18}, //[4:3] SEL_DIT_VOL_L_ADC0 SEL_DIT_VOL_R_ADC0
#endif

    {0x112e61, 0x0f, 0x0f}, //DRE enable

#endif // (ENABLE_LINE_OUT || ENABLE_EAR_OUT || ENABLE_LINE_IN)

    //end of table
    {TBL_REG_END, 0x00, 0x00}
};


const RegTblDlyType tblAudioDCOff[] =
{
    {TBL_REG_TAG, 0x00, 0xFF},

    {0x112bfc, 0x30, 0x30}, //0x112ca0 [5] DPGA SW mute mode, [4] DPGA SW mute

#if (ENABLE_LINE_OUT || ENABLE_EAR_OUT || ENABLE_LINE_IN)
    //-----------------De-POP Off----------------------------//
    // DEPOP  OFF  : 12m //
    {0x112e61, 0x0f, 0x00}, //DRE disable

    //{TBL_REG_DLY, 0x00, 0x14},
    //{0x112e63, 0x1f, 0x01}, // REG_GAIN_IDX_FORCE_VAL_L
    //{0x112e62, 0x1f, 0x01}, // REG_GAIN_IDX_FORCE_VAL_R

#if ENABLE_LINE_OUT
    {TBL_REG_DLY, 0x00, 0x02},
    {0x112cf2, 0xff, 0x01}, //GCR_TEST_AUSDM[7:0]
    {TBL_REG_DLY, 0x00, 0x02},
    {0x112cf2, 0xff, 0x03}, //GCR_TEST_AUSDM[7:0]
    {TBL_REG_DLY, 0x00, 0x02},
    {0x112cf2, 0xff, 0x07}, //GCR_TEST_AUSDM[7:0]
    {TBL_REG_DLY, 0x00, 0x02},
    {0x112cf2, 0xff, 0x0f}, //GCR_TEST_AUSDM[7:0]
    {TBL_REG_DLY, 0x00, 0x02},
    {0x112cf2, 0xff, 0x1f}, //GCR_TEST_AUSDM[7:0]
    {TBL_REG_DLY, 0x00, 0x02},
    {0x112cf2, 0xff, 0x3f}, //GCR_TEST_AUSDM[7:0]
    {TBL_REG_DLY, 0x00, 0x02},
    {0x112cf2, 0xff, 0x7f}, //GCR_TEST_AUSDM[7:0]
#endif


    {0x112cdc, 0x80, 0x80}, //EN_QS_LDO_ADC=1
    {0x112cdd, 0x01, 0x01}, //EN_QS_LDO_DAC=1

    {TBL_REG_DLY, 0x00, 0x02},
    // DEPOP  OFF  : 0m //
    {0x112cd4, 0xff, 0xff}, //SEL_DEPOP_LINE[7:0]=16'hff
    {0x112cd5, 0xff, 0x7f}, //SEL_DEPOP_LINE[14:8]=16'h7f
    // DEPOP  OFF  : 0m //
    // DEPOP  OFF  : 14m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0xfe}, //SEL_DEPOP_LINE[7:0]=16'hfe
    // DEPOP  OFF  : 1m //
    // DEPOP  OFF  : 15m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0xfa}, //SEL_DEPOP_LINE[7:0]=16'hfa
    // DEPOP  OFF  : 2m //
    // DEPOP  OFF  : 16m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd5, 0xff, 0x3f}, //SEL_DEPOP_LINE[14:8]=16'h3f
    // DEPOP  OFF  : 3m //
    // DEPOP  OFF  : 17m //
    {TBL_REG_DLY, 0x00, 0x01},
    //{0x112ce8, 0x02, 0x00}, //EN_STG2_AB_LINE=0
    //{0x112ce8, 0x01, 0x00}, //EN_STG2_AB_EAR=0
    {0x112ce8, 0x03, 0x00}, //EN_STG2_AB_LINE=0, EN_STG2_AB_EAR=0
    {0x112cd6, 0xff, 0xef}, //SEL_DEPOP_EAR[11:0]=4ef
    {0x112cd7, 0xff, 0x04}, //SEL_DEPOP_EAR

    // DEPOP  OFF  : 4m //
    // DEPOP  OFF  : 18m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd5, 0xff, 0x3b}, //SEL_DEPOP_LINE[14:8]=16'h3b
    // DEPOP  OFF  : 5m //
    // DEPOP  OFF  : 19m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd5, 0xff, 0x39}, //SEL_DEPOP_LINE[14:8]=16'h39
    // DEPOP  OFF  : 6m //
    // DEPOP  OFF  : 20m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x1a}, //SEL_DEPOP_LINE[7:0]=16'h1a
    // DEPOP  OFF  : 7m //
    // DEPOP  OFF  : 21m //

    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x5a}, //SEL_DEPOP_LINE[7:0]=16'h5a
    // DEPOP  OFF  : 8m //
    // DEPOP  OFF  : 22m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd5, 0xff, 0x01}, //SEL_DEPOP_LINE[14:8]=16'h01


    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd5, 0xff, 0x00}, //SEL_DEPOP_LINE[14:8]=16'h00
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd5, 0xff, 0x08}, //SEL_DEPOP_LINE[14:8]=16'h08

    //{TBL_REG_DLY, 0x00, 0x0a},
    //{0x112e5e 0x10 0x10}, // force register DRE
    //{0x112e63 0x1f 0x0e}, // REG_GAIN_IDX_FORCE_VAL_L
    //{0x112e62 0x1f 0x0e}, // REG_GAIN_IDX_FORCE_VAL_R
    //{TBL_REG_DLY, 0x00, 0x05},

    // DEPOP OFF  : 9m //
    // DEPOP OFF  : 23m //

    {TBL_REG_DLY, 0x00, 0x05},
    {0x112ce9, 0x0c, 0x0c}, //SFTDCH[1:0]=11
    // DEPOP OFF  : 14m //
    // DEPOP OFF  : 28m //

    {TBL_REG_DLY, 0x00, 0x05},
    {0x0b00, 0x02, 0x02}, //PD_VREF=1
    // DEPOP OFF  : 19m //
    // DEPOP OFF  : 33m //
    {TBL_REG_DLY, 0x00, 0x05},
    {0x000b03, 0x04, 0x04}, //EN_VREF_DISCH=1
    // DEPOP OFF  : 24m //
    // DEPOP OFF  : 38m //
    {TBL_REG_DLY, 0x00, 0x05},
    {0x112ce9, 0x0c, 0x08}, //SFTDCH[1:0]=10


    // DEPOP OFF  : 43m //
    {TBL_REG_DLY, 0x00, 0x05},

    {0x112ce9, 0x0c, 0x00}, //SFTDCH[1:0]=00
    // DEPOP OFF  : 34m //
    // DEPOP OFF  : 143m //

    {TBL_REG_DLY, 0x00, 0x05},

    // DEPOP OFF  : 163m //
    {TBL_REG_DLY, 0x00, 0x05},


    {0x112cd8, 0xff, 0x01}, //EN_VREF_SFTMSP[7:0]=8'b0000,0001
    // DEPOP OFF  : 254m //
    // DEPOP OFF  : 313m //
    {TBL_REG_DLY, 0x00, 0x05},
    {0x112cd8, 0xff, 0x03}, //EN_VREF_SFTMSP[7:0]=8'b0000,0011
    // DEPOP OFF  : 259m //
    // DEPOP OFF  : 318m //
    {TBL_REG_DLY, 0x00, 0x05},
    {0x112cd8, 0xff, 0x07}, //EN_VREF_SFTMSP[7:0]=8'b0000,0111
    // DEPOP OFF  : 264m //
    // DEPOP OFF  : 323m //

    {TBL_REG_DLY, 0x00, 0x05},
    {0x112cd8, 0xff, 0x0f}, //EN_VREF_SFTMSP[7:0]=8'b0000,1111
    // DEPOP OFF  : 269m //
    // DEPOP OFF  : 328m //
    {TBL_REG_DLY, 0x00, 0x05},
    {0x112cd8, 0xff, 0x1f}, //EN_VREF_SFTMSP[7:0]=8'b0001,1111
    // DEPOP OFF  : 274m //
    // DEPOP OFF  : 333m //
    {TBL_REG_DLY, 0x00, 0x0a},
    {0x112cd8, 0xff, 0x3f}, //EN_VREF_SFTMSP[7:0]=8'b0011,1111
    // DEPOP OFF  : 284m //
    // DEPOP OFF  : 343m //


    {TBL_REG_DLY, 0x00, 0x0a},


    {0x112cd8, 0xff, 0x7f}, //EN_VREF_SFTMSP[7:0]=8'b0111,1111
    // DEPOP OFF  : 304m //
    // DEPOP OFF  : 363m //

    {TBL_REG_DLY, 0x00, 0x0a},

    {0x112cd8, 0xff, 0xff}, //EN_VREF_SFTMSP[7:0]=8'b1111,11111
    // DEPOP OFF  : 324m //
    // DEPOP OFF  : 383m //

    {TBL_REG_DLY, 0x01, 0x5e}, // 420 // 390
    {TBL_REG_DLY, 0x01, 0x5e},

    {0x112cd5, 0xff, 0x08}, //SEL_DEPOP_LINE[14:8]=16'h08
    // DEPOP  OFF  : 1614m //
    // DEPOP  OFF  : 1773m //
    {TBL_REG_DLY, 0x00, 0x32}, // 40
    {0x112cd5, 0xff, 0x18}, //SEL_DEPOP_LINE[14:8]=16'h18
    // DEPOP  OFF  : 1654m //
    // DEPOP  OFF  : 1813m //
    {TBL_REG_DLY, 0x00, 0x2d}, // 40
    {0x112cd4, 0xff, 0xde}, //SEL_DEPOP_LINE[7:0]=16'hde
    // DEPOP  OFF  : 1694m //
    // DEPOP  OFF  : 1853m //

    {TBL_REG_DLY, 0x00, 0x02},
    {0x112cd4, 0xff, 0xdf}, //SEL_DEPOP_LINE[7:0]=16'hdf
    // DEPOP  OFF  : 1696m //
    // DEPOP  OFF  : 1855m //
    {TBL_REG_DLY, 0x00, 0x02},
    {0x112cd4, 0xff, 0xdd}, //SEL_DEPOP_LINE[7:0]=16'hdd
    // DEPOP  OFF  : 1698m //
    // DEPOP  OFF  : 1857m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd5, 0xff, 0x00}, //SEL_DEPOP_LINE[14:8]=16'h00
    {0x112cd8, 0xff, 0x00}, //EN_VREF_SFTMSP[7:0]=8'b0000,0000
    {0x112cd9, 0xff, 0x00}, //EN_VREF_SFTMSP[8]=0

    // DEPOP  OFF  : 1699m //
    // DEPOP  OFF  : 1858m //
    {TBL_REG_DLY, 0x00, 0x02},
    {0x112cd4, 0xff, 0x9d}, //SEL_DEPOP_LINE[7:0]=16'h9d
    // DEPOP  OFF  : 1701m //
    // DEPOP  OFF  : 1860m //

    {TBL_REG_DLY, 0x00, 0x02},
    {0x112cd4, 0xff, 0x1d}, //SEL_DEPOP_LINE[7:0]=16'h1d
    // DEPOP OFF EAR : 1703m //
    // DEPOP OFF EAR : 1862m //

    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd6, 0xff, 0xcd}, //SEL_DEPOP_EAR[11:0]=4cd
    // DEPOP  OFF  : 1704m //
    // DEPOP  OFF  : 1863m //
    {TBL_REG_DLY, 0x00, 0x02},
    {0x112cd4, 0xff, 0x1e}, //SEL_DEPOP_LINE[7:0]=16'h1e
    // DEPOP  OFF  : 1706m //
    // DEPOP  OFF  : 1865m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x1a}, //SEL_DEPOP_LINE[7:0]=16'h1a
    {0x112cd6, 0xff, 0xc1}, //SEL_DEPOP_EAR[11:0]=4c1
    // DEPOP  OFF  : 1707m //
    // DEPOP  OFF  : 1866m //
    {TBL_REG_DLY, 0x00, 0x02},
    {0x112cd4, 0xff, 0x1b}, //SEL_DEPOP_LINE[7:0]=16'h1b
    {0x112cd6, 0xff, 0x01}, //SEL_DEPOP_EAR[11:0]=401
    // DEPOP  OFF  : 1709m //
    // DEPOP  OFF  : 1868m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x19}, //SEL_DEPOP_LINE[7:0]=16'h19
    // DEPOP  OFF  : 1710m //
    // DEPOP  OFF  : 1869m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x18}, //SEL_DEPOP_LINE[7:0]=16'h18
    // DEPOP  OFF  : 1711m //
    // DEPOP  OFF  : 1870m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x08}, //SEL_DEPOP_LINE[7:0]=16'h08
    // DEPOP  OFF  : 1712m //
    // DEPOP  OFF  : 1871m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x0c}, //SEL_DEPOP_LINE[7:0]=16'h0c
    // DEPOP  OFF  : 1713m //
    // DEPOP  OFF  : 1872m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x04}, //SEL_DEPOP_LINE[7:0]=16'h04
    {0x112cd6, 0xff, 0x00}, //SEL_DEPOP_EAR[11:0]=400
    // DEPOP  OFF  : 1714m //
    // DEPOP  OFF  : 1873m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd4, 0xff, 0x00}, //SEL_DEPOP_LINE[7:0]=16'h00

    // DEPOP OFF  : 1715m //
    // DEPOP OFF  : 1874m //

    {TBL_REG_DLY, 0x00, 0x04},
    {0x112ce9, 0x0c, 0x00}, //SFTDCH[1:0]=00
    {0x112cd7, 0xff, 0x00}, //SEL_DEPOP_EAR[11:0]=000
    // DEPOP OFF EAR : 1719m //
    // DEPOP OFF EAR : 1878m //
    {TBL_REG_DLY, 0x00, 0x05},
    {0x000b00, 0x01, 0x01}, //PD_OPLP_EAR=1

#if ENABLE_EAR_OUT
    {0x000b02, 0x01, 0x01}, //EN_VMID2GND_EAR=1
#endif

    {0x112cdb, 0x60, 0x60}, //PD_LT_EAR=1 PD_RT_EAR=1
    {0x112cf1, 0x01, 0x00}, //EN_STG2_LP_EAR=0

    // DEPOP  OFF  : 1724m //
    // DEPOP  OFF  : 1883m //


    {TBL_REG_DLY, 0x00, 0x0a},

    {0x000b03, 0x04, 0x00}, //EN_VREF_DISCH=0
    // DEPOP  OFF  : 1729m //
    // DEPOP  OFF  : 1933m //
    //{0x112cdb, 0x01, 0x01}, //PD_IBIAS_LINE=1
    //{0x112cdb, 0x80, 0x80}, //PD_LT_LINE=1
    {0x112cdb, 0x81, 0x81},

    {0x000b00, 0x08, 0x08}, //PD_OPLP_LINE=1

    //{0x112cdc, 0x0c, 0x0c},

#if ENABLE_LINE_OUT
    {0x000b02, 0x02, 0x02}, //EN_VMID2GND_LINE=1
#endif

    {0x112cdf, 0x80, 0x80}, //PD_RT_LINE=1
    {0x112cf0, 0x80, 0x00}, //EN_STG2_LP_LINE=0
    // DEPOP  OFF  : 1729m //
    // DEPOP  OFF  : 1938m //
    {TBL_REG_DLY, 0x00, 0x05},
    {0x112ce4, 0x04, 0x04}, //EN_MUTE_LINE=1
    {0x112cdb, 0x10, 0x10}, //PD_IBIAS_EAR
    {0x112cf0, 0x10, 0x10}, //EN_MUTE_EAR
    {0x112cdc, 0x08, 0x08}, //PD_VI=1

    {0x112cdc, 0x80, 0x00}, //EN_QS_LDO_ADC=0
    {0x112cdd, 0x01, 0x00}, //EN_QS_LDO_DAC=0


#if ENABLE_LINE_OUT
    //{TBL_REG_DLY, 0x00, 0x02},
    //{0x112cf2, 0xff, 0x7f}, //GCR_TEST_AUSDM[7:0]
    {TBL_REG_DLY, 0x00, 0x02},
    {0x112cf2, 0x3f, 0x3f}, //GCR_TEST_AUSDM[7:0]
    {TBL_REG_DLY, 0x00, 0x02},
    {0x112cf2, 0x3f, 0x1f}, //GCR_TEST_AUSDM[7:0]
    {TBL_REG_DLY, 0x00, 0x04},
    {0x112cf2, 0x3f, 0x0f}, //GCR_TEST_AUSDM[7:0]
    {TBL_REG_DLY, 0x00, 0x04},
    {0x112cf2, 0x3f, 0x07}, //GCR_TEST_AUSDM[7:0]
    {TBL_REG_DLY, 0x00, 0x04},
    {0x112cf2, 0xff, 0x03}, //GCR_TEST_AUSDM[7:0]
    {TBL_REG_DLY, 0x00, 0x04},
    {0x112cf2, 0xff, 0x01}, //GCR_TEST_AUSDM[7:0]
    {TBL_REG_DLY, 0x00, 0x04},
    {0x112cf2, 0xff, 0x00}, //GCR_TEST_AUSDM[7:0]
#endif
    //total 1962m


    //Depop off final (return to default value..)
    {0x112cda, 0x67, 0xff},
    {0x112cdb, 0xf7, 0xff},
    {0x112cdc, 0xff, 0x7f},
    {0x112cdd, 0xff, 0x3e},
    {0x112cde, 0xff, 0x00},
    {0x112cdf, 0xff, 0x80},
    {0x112ce0, 0xff, 0x00},
    //{0x000b02, 0x03, 0x03},


#if ENABLE_EAR_OUT
    {0x000b02, 0x01, 0x01}, //EN_VMID_PULL_LOW_EAR=1
#endif

#if ENABLE_LINE_OUT
    {0x000b02, 0x02, 0x02}, //EN_VMID_PULL_LOW_LINE=1
#endif


    {0x000b02, 0xfc, 0xc0},

    {0x112ce2, 0x7f, 0x00},
    {0x112ce3, 0xff, 0x00},
    {0x112ce4, 0xff, 0x00},
    {0x112ce5, 0xff, 0x00},
    {0x112ce6, 0xfb, 0x00},
    {0x112ce7, 0xff, 0x00},
    {0x112ce8, 0xff, 0x00},
    {0x112ce9, 0xff, 0x01},
    {0x112cea, 0xff, 0x00},
    {0x112ceb, 0xff, 0x00},
    {0x112cec, 0xff, 0x00},
    {0x112ced, 0xff, 0x00},
    {0x112cee, 0x7f, 0x00},
    {0x112cef, 0x0f, 0x00},
    {0x112cf0, 0xff, 0x00},
    {0x112cf1, 0x1f, 0x00},
    {0x112cf2, 0xff, 0x00},
    {0x112cf3, 0x7f, 0x00},
    {0x000b00, 0xff, 0x0b},

    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd8, 0xff, 0xff},
    {0x112cd9, 0xff, 0xff},
    {0x000b03, 0x04, 0x04}, //EN_VREF_DISCH=1
    {TBL_REG_DLY, 0x00, 0x01},
    //{0x112e5e, 0x10, 0x00}, // force register DRE
    //{0x112e63, 0x1f, 0x00}, // REG_GAIN_IDX_FORCE_VAL_L
    //{0x112e62, 0x1f, 0x00}, // REG_GAIN_IDX_FORCE_VAL_R
#endif // !ENABLE_I2S

    //end of table
    {TBL_REG_END, 0x00, 0x00}
};


//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
///
///           AUDIO DRV FUNCTION
///
////////////////////////////////////////////////////////////////////////////////
//******************************************************************************
//------------------------------------------------------------------------------
/// @brief \b Function \b Name:  msDrvAudioTblProc()
/// @brief \b Function \b Description: Load Audio Table
/// @param <IN>        \b pTbl : table
/// @param <IN>        \b bFroceDelay : [TRUE] force delay [FALSE] using main loop timer
/// @return <OUT>      \b
//------------------------------------------------------------------------------
static void msDrvAudioTblProc(void)
{
    U16 u16Delay;
    static U8 u8TagIdMask = 0xFF;

    if(!pTblAudio || u16TimerAudio)
        return;

    while(1)
    {
        if(pTblAudio->u32Reg == TBL_REG_TAG)
        {
            u8TagIdMask = pTblAudio->u8Value;
            pTblAudio++;
        }

        if(u8TagIdMask & u8EfuseIdMask)
        {
            if(pTblAudio->u32Reg == TBL_REG_END)
            {
                //end of table
                pTblAudio = NULL;
                bTblAudioDone = TRUE;
                bAudioFmtChk = TRUE;
                MSAUDIO_PRINT("TblAudioDone\r\n");
                break;
            }
            else if(pTblAudio->u32Reg == TBL_REG_DLY)
            {
            #if EnableDACDePop
                //delay
                u16Delay = ((U16)pTblAudio->u8Mask << 8) | pTblAudio->u8Value;
            #else
                u16Delay = 0;
            #endif

                if(bTblAudioForceDelay || (u16Delay < AUDIO_DELAY_FROCE_THRESHOLD))
                {
                    ForceDelay1ms(u16Delay);
                }
                else
                {
                    MSAUDIO_PRINT("TblAudio TimerDelay:%d\r\n", u16Delay);
                    u16TimerAudio = u16Delay;
                    pTblAudio++;
                    break;
                }
            }
            else
            {
                msWriteByteMask(pTblAudio->u32Reg, pTblAudio->u8Value, pTblAudio->u8Mask);
            }
        }

        pTblAudio++;
    }
}


//------------------------------------------------------------------------------
/// @brief \b Function \b Name:  msDrvAudioTblLoad()
/// @brief \b Function \b Description: Set Audio Table for Loading, AC on / DC off
/// @param <IN>        \b pTbl : table
/// @param <IN>        \b bFroceDelay : [TRUE] force delay [FALSE] using main loop timer
/// @return <OUT>      \b
//------------------------------------------------------------------------------
static void msDrvAudioTblLoad(const RegTblDlyType *pTbl, BOOL bFroceDelay)
{
    pTblAudio = pTbl;
    bTblAudioForceDelay = bFroceDelay;
    bTblAudioDone = FALSE;
    u16TimerAudio = 0;

    msDrvAudioTblProc();
}


static BOOL msDrvAudioNPCMFlagGet(void)
{
    BYTE u8Rtn = FALSE;

    if(eAudioSrc == AUDIO_DIGITAL)
        u8Rtn = (msReadByte(REG_112BE5) & BIT7);

    //MSAUDIO_PRINT("bAudioNPCM = 0x%x\n", u8Rtn);

    return u8Rtn;
}


static BYTE msDrvAudioSampleRateKHzGet(BOOL bSrc)
{
    if(bSrc)
        return u8SourceSampleRateKHz; // From source
    else
        return u8AudioSampleRateKHz; // Use for Haydn
}


static void msDrvAudioSampleRateKHzSet(BYTE u8InputPort)
{
    u8SourceSampleRateKHz = (BYTE)msAPI_combo_IPGetAudioFrequency(u8InputPort);
    u8AudioSampleRateKHz = (BYTE)AUD_SamplleRate_48K;

    MSAUDIO_PRINT("Source sample rate = %d\n", u8SourceSampleRateKHz);
    MSAUDIO_PRINT("Audio sample rate = %d\n", u8AudioSampleRateKHz);
}


static void msDrvAudioSRCBypass(BOOL bEnable)
{
    MSAUDIO_PRINT("AUDIO SRC Bypass = 0x%x\n", bEnable);
    msWrite2ByteMask(REG_112C04, (bEnable << 15), BIT15);
}


static void msDrvAudioDMABypass(BOOL bEnable)
{
    MSAUDIO_PRINT("AUDIO DMA bypass = 0x%x\n", bEnable);
    msWrite2ByteMask(REG_112D00, (bEnable << 15), BIT15);
}


static void msDrvAudioMixerBypass(BOOL bEnable)
{
    MSAUDIO_PRINT("AUDIO Mixer bypass = 0x%x\n", bEnable);
    msWrite2ByteMask(REG_112C86, (bEnable << 15), BIT15);
}


static void msDrvAudioDPGABypass(BOOL bEnable)
{
    MSAUDIO_PRINT("AUDIO DPGA bypass = 0x%x\n", bEnable);
    msWrite2ByteMask(REG_112D3A, (bEnable << 15), BIT15); // DPGA bypass
}


//------------------------------------------------------------------------------
/// @brief \b Function \b Name:  msDrvAudioPadSetting()
/// @brief \b Function \b Description: Pad Output Setting
/// @return <OUT>      \b
//------------------------------------------------------------------------------
static void msDrvAudioPadSetting(void)
{
    Init_Pin_I2S();
    Init_Pin_SPDIF();
}


//------------------------------------------------------------------------------
/// @brief \b Function \b Name:  msDrvAudioDPGASetVolTrig(void)
/// @brief \b Function \b Description: Trigger DPGA Setting
/// @param <IN>        \b
/// @return <OUT>      \b
//------------------------------------------------------------------------------
static void msDrvAudioDPGASetVolTrig(void)
{
    msWriteByteMask(REG_112D21, 0x00, BIT7);
    msWriteByteMask(REG_112D21, BIT7, BIT7);
    msWriteByteMask(REG_112D21, 0x00, BIT7);
}

//------------------------------------------------------------------------------
/// @brief \b Function \b Name:  msDrvAudioDPGASetVolume()
/// @brief \b Function \b Description: Set Audio DPGA Volume
/// @param <IN>        \b eChSelect: Channel selection
/// @param <IN>        \b u16DPGAGain: DPGA gain (0~0xFFF)
/// @return <OUT>      \b
//------------------------------------------------------------------------------
static void msDrvAudioDPGASet(AUDIO_CH_SEL eChSelect, WORD u16DPGAGain)
{

#if 1

    MSAUDIO_PRINT("Channel Selection=%x\r\n", eChSelect);
    MSAUDIO_PRINT("DPGA Set=%x\r\n",u16DPGAGain);
#endif

    switch(eChSelect)
    {
        case E_AUDIO_LEFT_CH:
            msWrite2ByteMask(REG_112D20, u16DPGAGain, 0x0FFF);
            break;

        case E_AUDIO_RIGHT_CH:
            msWrite2ByteMask(REG_112D22, u16DPGAGain, 0x0FFF);
            break;

        case E_AUDIO_LEFT_RIGHT_CH:
            msWrite2ByteMask(REG_112D20, u16DPGAGain, 0x0FFF);
            msWrite2ByteMask(REG_112D22, u16DPGAGain, 0x0FFF);
            break;

        default:
            break;
    }
    
    msDrvAudioDPGASetVolTrig();
}

//------------------------------------------------------------------------------
/// @brief \b Function \b Name:  msDrvAudioDPGASetVolume()
/// @brief \b Function \b Description: Set Audio DPGA Volume
/// @param <IN>        \b eChSelect: Channel selection
/// @param <IN>        \b Volume_Integer: Volume Integer Part from 0 ~ -113dB
/// @param <IN>        \b Volume_Fraction: Volume Fraction Part, -0.125dB per unit
///                    \b        sel in AUDIO_VOLUME_FRACTION_PART
/// @return <OUT>      \b
//------------------------------------------------------------------------------
static void msDrvAudioDPGASetVolume(AUDIO_CH_SEL eChSelect, signed short iVolume_Integer, AUDIO_VOLUME_FRACTION_PART Volume_Fraction)
{
    //BYTE u8Offset = eDPGA*8;
    BYTE   voltab_index;
    WORD  reg16_LCHVolume,reg16_RCHVolume;

    if (iVolume_Integer > 0)
    {
        iVolume_Integer = 0;   //maximum is 0dB
    }
    else if (iVolume_Integer <= -113)   //(+911) * -0.125dB = -113.875dB
    {
        iVolume_Integer = -113;   //minimum
    }

    voltab_index = abs(iVolume_Integer);
    reg16_RCHVolume = AudVolumeTab[voltab_index]+Volume_Fraction;
    reg16_LCHVolume = AudVolumeTab[voltab_index]+Volume_Fraction;

#if 1
    MSAUDIO_PRINT("volume_Integer=%d, \r\n", iVolume_Integer);
    MSAUDIO_PRINT("voltab_index= %d\r\n", voltab_index);

    MSAUDIO_PRINT("Vol Ing =%x, \r\n", AudVolumeTab[voltab_index]);
    MSAUDIO_PRINT("Vol Fra=%x\r\n", Volume_Fraction);

    MSAUDIO_PRINT("L Channel Register Vol=%x\r\n", reg16_LCHVolume);
    MSAUDIO_PRINT("R Channel Register Vol=%x\r\n",reg16_RCHVolume);
#endif

    switch(eChSelect)
    {
        case E_AUDIO_LEFT_CH:
            msWrite2ByteMask(REG_112D20, reg16_LCHVolume, 0x0FFF);
            break;

        case E_AUDIO_RIGHT_CH:
            msWrite2ByteMask(REG_112D22, reg16_RCHVolume, 0x0FFF);
            break;

        case E_AUDIO_LEFT_RIGHT_CH:
            msWrite2ByteMask(REG_112D20, reg16_LCHVolume, 0x0FFF);
            msWrite2ByteMask(REG_112D22, reg16_RCHVolume, 0x0FFF);
            break;

        default:
            break;
    }

    msDrvAudioDPGASetVolTrig();
}


//**************************************************************************
//  [Function Name]:
//                  msDrvAudioSetDpgaMute()
//  [Description]
//                  DPGA mute
//  [Arguments]:
//
//  [Return]:
//
//**************************************************************************
void  msDrvAudioSetDpgaMute(BOOL enable)
{
    //SW mode: BIT5 = 1, HW mode: BIT5 = 0
    if(enable)
        msWriteByteMask(REG_112BFC, BIT5|BIT4, BIT5|BIT4);
    else if(((msReadByte(REG_112C05) & 0x70) >> 4) == AUDIO_LINE_IN)
        msWriteByteMask(REG_112BFC, BIT5, BIT5|BIT4);
    else
        msWriteByteMask(REG_112BFC, 0x00, BIT5|BIT4);

#if TTS
    mDrvAudioTTSSysMute(msReadByte(REG_112BFC) & (BIT5|BIT4));
#endif
}


//**************************************************************************
//  [Function Name]:
//                  msDrvAudioSourceSel()
//  [Description]
//                  Select Line-out source
//  [Arguments]:
//                  src: Line-in0, Line-in1, DAC
//  [Return]:
//
//**************************************************************************
void msDrvAudioSourceSel(AUDIO_SOURCE_SEL src)
{
    MSAUDIO_PRINT("msDrvAudioSourceSel:%d\r\n",src);
    msWrite2ByteMask(REG_112C04, (BYTE)src << 12, BIT14|BIT13|BIT12);
}


//------------------------------------------------------------------------------
/// @brief \b Function \b Name:  msDrvAudioI2STxConfig()
/// @brief \b Function \b Description: Audio I2S configuration
/// @param <IN>        \b
/// @return <OUT>      \b
//------------------------------------------------------------------------------
static void msDrvAudioI2STxConfig(AUDIO_I2S_FORMAT_SEL eI2SFormat, AUDIO_I2S_BCK_SEL eI2SBCK, AUDIO_I2S_MCK_SEL eI2SMCK)
{
    msWrite2ByteMask(REG_112C80, (eI2SFormat<<15)|(eI2SBCK<<13), BIT15|BIT14|BIT13);
    msWrite2ByteMask(REG_112C82, eI2SMCK, BIT2|BIT1|BIT0);
}


static void msDrvAudioI2SSwap(BOOL bEnable)
{
    msWriteByteMask(REG_112C83, bEnable, BIT0);
}


static void msDrvAudioHDMISwap(BOOL bEnable)
{
    msWriteByteMask(REG_112B20, 0x00, 0x1F);
    msWriteByteMask(REG_112B21, bEnable?0x01:0x00, 0x1F);
    DelayUs(1);

    msWriteByteMask(REG_112B20, 0x01, 0x1F);
    msWriteByteMask(REG_112B21, bEnable?0x00:0x01, 0x1F);
    DelayUs(1);
}


static void msDrvAudioADCSwap(BOOL bEnable)
{
    msWriteByteMask(REG_112C29, (bEnable << 5), BIT5);
}


#if ENABLE_SPDIF
static void msDrvAudioSpdifSwap(BOOL bEnable)
{
    msWriteByteMask(REG_112C9A, (bEnable << 7), BIT7);
}


static void msDrvAudioSpdifMute(BOOL bEnable)
{
    MSAUDIO_PRINT("Disable SPDIF output = %d\n", bEnable);
    msWriteByteMask(REG_112C99, (bEnable << 7), BIT7);
}


void msAPI_AudioSpdifSwap(BOOL bEnable) // SPDIF TX L/R swap
{
    msDrvAudioSpdifSwap(bEnable);
}


void msAPI_AudioSpdifMute(BOOL bEnable)
{
    msDrvAudioSpdifMute(bEnable);
}
#endif


#if ENABLE_ARC
static void msDrvAudioArcStsSet(AUDIO_ARC_STS eArcSts)
{
    static AUDIO_ARC_STS ePreArcSts = AUDIO_ARC_STS_OFF;

    if(ePreArcSts != eArcSts)
    {
        ePreArcSts = eArcSts;
        sAudioArc.eStatus = eArcSts;
        MSAUDIO_PRINT("Audio ARC status = %d\n", sAudioArc.eStatus);
    }
}


static void msDrvAudioArcPllSet(BYTE u8SampleRateKHz)
{
    static BYTE u8SampleRate_BK = AUD_SamplleRate_48K;
    WORD u16TimeOut = 20;

    MSAUDIO_PRINT("Set ARC PLL, u8SampleRateKHz = %d\n", u8SampleRateKHz);

    if(u8SampleRateKHz == u8SampleRate_BK)
        return;

    u8SampleRate_BK = u8SampleRateKHz;

    if(u8SampleRateKHz <= AUD_SamplleRate_96K) // 32K ~ 96K
    {
        msWriteByteMask(REG_112DF7, 0x02, 0x03); // GCR_AUPLL_KP1[9:8]
        msWriteByteMask(REG_112DF8, 0x40, 0xc0); // GCR_AUPLL_LOOPDIV_FIRST[7:6]
        msWriteByteMask(REG_112DFB, 0x60, 0xff); // TEST_AUPLL[15:8]

        if(u8SampleRateKHz <= AUD_SamplleRate_48K)
        {
            msWriteByteMask(REG_112DF6, 0x80, 0x80); // GCR_AUPLL_KP1[7]
            msWriteByteMask(REG_112DF6, 0x20, 0x60); // GCR_AUPLL_KP0[6:5]
            msWriteByteMask(REG_112DFD, 0x28, 0xff); // GCR_AUPLL_LOOPDIV_SECOND[15:8]
        }
        else if(u8SampleRateKHz <= AUD_SamplleRate_96K)
        {
            msWriteByteMask(REG_112DF6, 0x00, 0x80); // GCR_AUPLL_KP1[7]
            msWriteByteMask(REG_112DF6, 0x00, 0x60); // GCR_AUPLL_KP0[6:5]
            msWriteByteMask(REG_112DFD, 0x14, 0xff); // GCR_AUPLL_LOOPDIV_SECOND[15:8]
        }
    }
    else // 176.4K ~ 192K
    {
        msWriteByteMask(REG_112DF7, 0x01, 0x03); // GCR_AUPLL_KP1[9:8]
        msWriteByteMask(REG_112DF6, 0x80, 0x80); // GCR_AUPLL_KP1[7]
        msWriteByteMask(REG_112DF6, 0x60, 0x60); // GCR_AUPLL_KP0[6:5]
        msWriteByteMask(REG_112DF8, 0x00, 0xc0); // GCR_AUPLL_LOOPDIV_FIRST[7:6]
        msWriteByteMask(REG_112DFB, 0x80, 0xff); // TEST_AUPLL[15:8]
        msWriteByteMask(REG_112DFD, 0x14, 0xff); // GCR_AUPLL_LOOPDIV_SECOND[15:8]
    }

    while((!(msReadByte(REG_112DF4) & BIT1)) && u16TimeOut--)
    {
        Delay1ms(1);
    }

    if(!(msReadByte(REG_112DF4) & BIT1))
        MSAUDIO_PRINT("Set ARC PLL failed\n");
}


static void msDrvAudioArcInit(BOOL bEnable, AUDIO_ARC_STS eStatus)
{
    BYTE u8CECBuf[1] = {0};

    MSAUDIO_PRINT("ARC init = %d\n", bEnable);

    if(bEnable)
    {
        api_CEC_SendMessage(CEC_LA_AUD_SYSTEM, CEC_OPCODE_REPORT_ARC_INIT, u8CECBuf, 0);
    }
    else
    {
        msDrvAudioSpdifMute(TRUE);
        api_CEC_SendMessage(CEC_LA_AUD_SYSTEM, CEC_OPCODE_REPORT_ARC_TERMINATE, u8CECBuf, 0);
    }

    msDrvAudioArcStsSet(eStatus);
}


static void msDrvAudioArcEnable(Bool bEnable)
{
    BYTE u8CECBuf[1] = {0};

    if(bEnable)
    {
        if(sAudioArc.eStatus < AUDIO_ARC_STS_INIT) // ARC may already init by receiving init arc cmd from device
        {
            MSAUDIO_PRINT("Send ARC REQ init cmd\n");
            api_CEC_SendMessage(CEC_LA_AUD_SYSTEM, CEC_OPCODE_REQ_ARC_INIT, u8CECBuf, 0);
            sAudioArc.u16CECTimeout = 1500;
            msDrvAudioArcStsSet(AUDIO_ARC_STS_REQ_INIT);
        }
    }
    else
    {
        msDrvAudioSpdifMute(TRUE);
        MSAUDIO_PRINT("Send ARC REQ terminate cmd\n");
        api_CEC_SendMessage(CEC_LA_AUD_SYSTEM, CEC_OPCODE_REQ_ARC_TERMINATE, u8CECBuf, 0);
        sAudioArc.u16CECTimeout = 1500;
        msDrvAudioArcStsSet(AUDIO_ARC_STS_REQ_TERMINATE);
    }
}


static void msDrvAudioArcHandler(void)
{
    AUDIO_ARC_STS eStatus;

    if((sAudioArc.eStatus != AUDIO_ARC_STS_OFF) && (sAudioArc.eStatus != AUDIO_ARC_STS_5V_LOST)
       && (msAPI_combo_IPCable_5V_Detect(ARC_PORT) == FALSE))
    {
        if(sAudioArc.eStatus == AUDIO_ARC_STS_REQ_TERMINATE)
            eStatus = AUDIO_ARC_STS_OFF;
        else
            eStatus = AUDIO_ARC_STS_5V_LOST;

        msDrvAudioArcInit(FALSE, eStatus);
    }
    else if(sAudioArc.eStatus == AUDIO_ARC_STS_INIT)
    {
        if(bPanelOnFlag)
        {
            msDrvAudioArcPllSet(msDrvAudioSampleRateKHzGet(bAudioNPCM)); // ARC need to be muted before setting PLL
            msDrvAudioSpdifMute(FALSE);
            msDrvAudioArcStsSet(AUDIO_ARC_STS_ARC);
        }
    }
    else if(((sAudioArc.eStatus == AUDIO_ARC_STS_REQ_INIT) || (sAudioArc.eStatus == AUDIO_ARC_STS_REQ_TERMINATE))
            && (sAudioArc.u16CECTimeout == 0))
    {
        msDrvAudioArcInit(FALSE, AUDIO_ARC_STS_OFF);
    }

    if((sAudioArc.eStatus == AUDIO_ARC_STS_5V_LOST) && (msAPI_combo_IPCable_5V_Detect(ARC_PORT) == TRUE))
        msDrvAudioArcEnable(TRUE);
}


void msAPI_AudioArcInit(BOOL bEnable, AUDIO_ARC_STS eStatus)
{
    msDrvAudioArcInit(bEnable, eStatus);
}


void msAPI_AudioArcEnable(BOOL bEnable)
{
    if(bEnable && (msAPI_combo_IPCable_5V_Detect(ARC_PORT) == FALSE))
    {
        MSAUDIO_PRINT("ARC port lost 5V\n");
        msDrvAudioArcStsSet(AUDIO_ARC_STS_5V_LOST);
    }
    else
    {
        msDrvAudioArcEnable(bEnable);
    }
}
#endif


static BOOL msDrvAudioFmtChange(void)
{
    BOOL bRtn = FALSE;
    BOOL bSampleRateChg = FALSE;

    if((BYTE)msAPI_combo_IPGetAudioFrequency(u8AudioInputPort) != msDrvAudioSampleRateKHzGet(TRUE))
    {
        if(bAudioNPCM
    #if ENABLE_ARC
            || (sAudioArc.eStatus != AUDIO_ARC_STS_OFF)
    #endif
        )
        {
            bSampleRateChg = TRUE;
        }
        else // Just update audio info, no need to re-setup path
        {
            msDrvAudioSampleRateKHzSet(u8AudioInputPort);
        }
    }

    if((msDrvAudioNPCMFlagGet() != bAudioNPCM) || bSampleRateChg)
    {
        bRtn = TRUE;
        MSAUDIO_PRINT("Audio format change\n");
    }

    return bRtn;
}


static void msDrvAudioFmtCheck(void)
{
    if(bAudioFmtChk && (!u16AudioChkTimer)
       && ((SC0_READ_INPUT_SETTING() & BIT7) == 0) && InputTimingStableFlag
       && (!SyncLossState()) && (!UnsupportedModeFlag))
    {
        if(msDrvAudioFmtChange())
        {
            msAPI_AudioSetDpgaMute(TRUE);

        #if ENABLE_ARC
            if(sAudioArc.eStatus == AUDIO_ARC_STS_ARC)
            {
                msDrvAudioSpdifMute(TRUE);
                msDrvAudioArcStsSet(AUDIO_ARC_STS_INIT);
            }
        #endif

        #if TTS
            msAPI_AudioTTSMixEnable(FALSE);
        #endif

            msAPI_AudioPathSet();

        #if TTS
            msAPI_AudioTTSMixEnable(TRUE);
        #endif

            msAPI_AudioSetDpgaMute(FALSE);
        }

        u16AudioChkTimer = AUDIO_CHK_TIME;
    }
}


__attribute__((unused)) static void msDrvAudioDMAInit(AUDIO_DMA_ENGINE eEngine, DWORD u32BaseAddr, DWORD u32Size, AUDIO_DMA_BIT_MODE eDMABitMode)
{
    BYTE u8Offset = eEngine *0x20;

    MSAUDIO_PRINT("Audio DMA %d init, base addr = 0x%x, size = 0x%x, u8Offset = 0x%x\n", eEngine, u32BaseAddr, u32Size, u8Offset);

    // TTS Reader Init
    msWriteByteMask(REG_112A01 + u8Offset, (eDMABitMode << 1), BIT1);
    msWrite4Byte(REG_112A02 + u8Offset, (DWORD)(u32BaseAddr /AUDIO_DMA_UNIT)); // base addr
    msWrite2Byte(REG_112A06 + u8Offset, (WORD)(u32Size /AUDIO_DMA_UNIT)); // size
}


__attribute__((unused)) static void msDrvAudioDMAThrdSet(AUDIO_DMA_ENGINE eEngine, DWORD u32Thrd)
{
    BYTE u8Offset = eEngine *0x20;

    MSAUDIO_PRINT("Audio DMA %d thrd set, u32Thrd = 0x%x, u8Offset = 0x%x\n", eEngine, u32Thrd, u8Offset);

    msWrite2Byte(REG_112A0A + u8Offset, (u32Thrd /AUDIO_DMA_UNIT));
}


static void msDrvAudioDMAReset(AUDIO_DMA_ENGINE eEngine)
{
    BYTE u8Offset = eEngine *0x20;

    MSAUDIO_PRINT("Audio DMA %d reset, u8Offset = 0x%x\n", eEngine, u8Offset);

    msWriteByteMask(REG_112A01 + u8Offset, BIT7, BIT7);
    msWriteByteMask(REG_112A01 + u8Offset, 0, BIT7);
}


static void msDrvAudioDMAAllReset(void)
{
    msWriteByteMask(REG_112C00, BIT6, BIT6);
    msWriteByteMask(REG_112C00, 0, BIT6);
}


static void msDrvAudioDMAEnable(AUDIO_DMA_ENGINE eEngine, BOOL bEnable, BOOL bFreeRun)
{
    BYTE u8Offset = eEngine *0x20;

    BYTE u8Val = 0;

    MSAUDIO_PRINT("Audio DMA %d enable, bEnable = %d, bFreeRun = %d, u8Offset = 0x%x\n", eEngine, bEnable, bFreeRun, u8Offset);

    if(bEnable)
        u8Val = 0x03;

    if(bFreeRun)
        u8Val |= 0x04;

    if(!bEnable)
    {
        msWriteByteMask(REG_112A00 + u8Offset, 0, BIT1|BIT0);
        msDrvAudioDMAReset(eEngine);
    }
    else
    {
        msWriteByteMask(REG_112A00 + u8Offset, u8Val, BIT2|BIT1|BIT0);
    }
}


__attribute__((unused)) static void msDrvAudioDMAMCURWSizeSet(AUDIO_DMA_ENGINE eEngine, DWORD u32Size)
{
    BYTE u8Offset = eEngine *0x20;

    msWrite2Byte(REG_112A08 + u8Offset, (u32Size /AUDIO_DMA_UNIT));
}


__attribute__((unused)) static void msDrvAudioDMAMCURWTrig(AUDIO_DMA_ENGINE eEngine)
{
    BYTE u8Offset = eEngine *0x20;

    msWriteByteMask(REG_112A00 + u8Offset, BIT4, BIT4);
    msWriteByteMask(REG_112A00 + u8Offset, 0, BIT4);
}


static void msDrvAudioDelayEnable(BOOL bEnable, WORD u16DelayVal, AUDIO_DELAY_MS eDelayUnit)
{
#if TTS
    if(bEnable == FALSE)
    {
        bEnable = TRUE;
        u16DelayVal = 1;
    }
#endif

    MSAUDIO_PRINT("Audio delay enable = %d\n", bEnable);

    msDrvAudioDMABypass(TRUE);

#if TTS
    msDrvAudioDMAEnable(AUDIO_DMA_RD_TTS, FALSE, FALSE);
#endif

    msDrvAudioDMAEnable(AUDIO_DMA_RD, FALSE, FALSE);
    msDrvAudioDMAEnable(AUDIO_DMA_WR, FALSE, FALSE);
    msWriteBit(REG_112B40, FALSE, BIT0);
    msDrvAudioDMAAllReset();

    if(bEnable)
    {
        msDrvAudioDMABypass(FALSE);
        msWriteByteMask(REG_112B40, (eDelayUnit << 4), 0x30);
        msWrite2Byte(REG_112B42, u16DelayVal);
        msWriteBit(REG_112B40, u16DelayVal?TRUE:FALSE, BIT0);
        msDrvAudioDMAEnable(AUDIO_DMA_RD, TRUE, TRUE);
        msDrvAudioDMAEnable(AUDIO_DMA_WR, TRUE, TRUE);
    }
}


void msAPI_AudioDelayEnable(BOOL bEnable, WORD u16DelayVal, AUDIO_DELAY_MS eDelayUnit)
{
    msDrvAudioDelayEnable(bEnable, u16DelayVal, eDelayUnit);
}


//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
///
///           AUDIO API FUNCTION
///
////////////////////////////////////////////////////////////////////////////////
//******************************************************************************
BYTE msAPI_AudioInputPortGet(void)
{
    return u8AudioInputPort;
}


void msAPI_AudioInputPortSet(BYTE u8InputPort)
{
    u8AudioInputPort = u8InputPort;
}


//------------------------------------------------------------------------------
/// @brief \b Function \b Name: msAPI_AudioI2SConfig()
/// @brief \b Function \b Description: Audio I2S configuration
/// @param <IN>        \b
/// @return <OUT>      \b
//------------------------------------------------------------------------------
void msAPI_AudioI2SConfig(AUDIO_I2S_FORMAT_SEL eI2SFormat, AUDIO_I2S_BCK_SEL eI2SBCK, AUDIO_I2S_MCK_SEL eI2SMCK)
{
    msDrvAudioI2STxConfig(eI2SFormat, eI2SBCK, eI2SMCK);
}


void msAPI_AudioI2SSwap(BOOL bEnable) // I2S TX L/R swap
{
    msDrvAudioI2SSwap(bEnable);
}


/*void msAPI_AudioSDMSwap(BOOL bEnable) // LINE_OUT and EAR_OUT L/R swap
{
    // Cannot use because DRE is enabled. Please use RX(HDMI and ADC) to swap L/R.
}*/


void msAPI_AudioHDMISwap(BOOL bEnable) // HDMI(DP) RX L/R swap
{
    msDrvAudioHDMISwap(bEnable);
}


void msAPI_AudioADCSwap(BOOL bEnable) // Line in L/R swap
{
    msDrvAudioADCSwap(bEnable);
}


//------------------------------------------------------------------------------
/// @brief \b Function \b Name:  msAPI_AudioInit()
/// @brief \b Function \b Description: Audio Init
/// @param <IN>        \b
/// @return <OUT>      \b
//------------------------------------------------------------------------------
void msAPI_AudioInit(BOOL bACon)
{
    WORD u16Arg1;
    BYTE u8Arg2;

    msEread_Init(&u16Arg1, &u8Arg2);
    u8EfuseIdMask = 1 << u8Arg2;

    if(bACon)
    {
        msDrvAudioTblLoad(tblAudioACOn, FALSE);
        msAPI_AudioI2SConfig(AUDIO_I2S_FORMAT_LEFT_JUSTIFY, AUDIO_I2S_BCK_AU48FS, AUDIO_I2S_MCK_AU256FS);
        msDrvAudioPadSetting();

    #if TTS
        msDrvAudioTTSInit();
    #endif

#if AUDIO_DELAY_VAL
    #if (AUDIO_DELAY_DMA_SIZE <= MIU_AUDIO_SIZE)
        msDrvAudioDMAInit(AUDIO_DMA_WR, (DWORD)MIU_AUDIO_ADDR_START, (DWORD)(AUDIO_DELAY_DMA_SIZE), AUDIO_DMA_24_BIT);
        msDrvAudioDMAInit(AUDIO_DMA_RD, (DWORD)MIU_AUDIO_ADDR_START, (DWORD)AUDIO_DELAY_DMA_SIZE, AUDIO_DMA_24_BIT);
    #endif
#endif
    }
}


//------------------------------------------------------------------------------
/// @brief \b Function \b Name:  msAPI_AudioInit()
/// @brief \b Function \b Description: Audio Init
/// @param <IN>        \b
/// @return <OUT>      \b
//------------------------------------------------------------------------------
BOOL msAPI_AudioInitDone(void)
{
    return bTblAudioDone;
}


//------------------------------------------------------------------------------
/// @brief \b Function \b Name:  msAPI_AudioDACPowerDown()
/// @brief \b Function \b Description: Power Down / Up setting
/// @param <IN>        \b
/// @return <OUT>      \b
//------------------------------------------------------------------------------
void msAPI_AudioDACPowerDown(Bool bPowerDN)
{
    if(bPowerDN)
    {
        msDrvAudioTblLoad(tblAudioDCOff, TRUE);
    }
    else
    {
        msDrvAudioTblLoad(tblAudioACOn, FALSE);
        msAPI_AudioI2SConfig(AUDIO_I2S_FORMAT_LEFT_JUSTIFY, AUDIO_I2S_BCK_AU48FS, AUDIO_I2S_MCK_AU256FS);
        msDrvAudioPadSetting();

    #if TTS
        msDrvAudioTTSInit();
    #endif

#if AUDIO_DELAY_VAL
    #if (AUDIO_DELAY_DMA_SIZE <= MIU_AUDIO_SIZE)
        msDrvAudioDMAInit(AUDIO_DMA_WR, (DWORD)MIU_AUDIO_ADDR_START, (DWORD)(AUDIO_DELAY_DMA_SIZE), AUDIO_DMA_24_BIT);
        msDrvAudioDMAInit(AUDIO_DMA_RD, (DWORD)MIU_AUDIO_ADDR_START, (DWORD)AUDIO_DELAY_DMA_SIZE, AUDIO_DMA_24_BIT);
    #endif
#endif
    }
}


void msAPI_AudioPathSet(void)
{
    BOOL bMixerBypass = TRUE;

    bAudioNPCM = msDrvAudioNPCMFlagGet();

#if TTS
    if((!bAudioNPCM) && mDrvAudioTTSIsSupport())
        bMixerBypass = FALSE;
#endif

    msDrvAudioSRCBypass(bAudioNPCM);
    msDrvAudioDPGABypass(bAudioNPCM);
    msDrvAudioSampleRateKHzSet(u8AudioInputPort);
    msDrvAudioMixerBypass(bMixerBypass);

#if AUDIO_DELAY_VAL
    msDrvAudioDelayEnable(TRUE, AUDIO_DELAY_VAL, AUDIO_DELAY_UNIT);
#endif

#if TTS
    msAPI_AudioTTSMixEnable(TRUE);
#endif
}


//------------------------------------------------------------------------------
/// @brief \b Function \b Name:  msAPI_AudioSetDpgaMute()
/// @brief \b Function \b Description: Mute from DPGA hardware block
/// @param <IN>        \b enable
/// @return <OUT>      \b
//------------------------------------------------------------------------------
void msAPI_AudioSetDpgaMute(BOOL enable)
{
    MSAUDIO_PRINT("msAPI_AudioSetDpgaMute:%d\r\n", enable);
    msDrvAudioSetDpgaMute(enable);
}

//------------------------------------------------------------------------------
/// @brief \b Function \b Name: msAPI_AudioSetVolume()
/// @brief \b Function \b Description: Set DPGA gain
/// @param <IN>        \b eChSelect: LR channel selection
///                      u16DPGAGain: DPGA Set(0~ 0xFFF)
/// @return <OUT>      \b
//------------------------------------------------------------------------------
void msAPI_AudioSetDPGA(AUDIO_CH_SEL eChSelect, WORD u16DPGAGain)
{
    msDrvAudioDPGASet(eChSelect, u16DPGAGain);
}
//------------------------------------------------------------------------------
/// @brief \b Function \b Name: msAPI_AudioSetVolume()
/// @brief \b Function \b Description: Set DPGA gain
/// @param <IN>        \b chsel: LR channel selection
///                      volume: volume control(0~ -113.875dB)
/// @return <OUT>      \b
//------------------------------------------------------------------------------
void msAPI_AudioSetVolume(AUDIO_CH_SEL eChSelect, short iVolume_Integer, AUDIO_VOLUME_FRACTION_PART Volume_Fraction)
{
    msDrvAudioDPGASetVolume(eChSelect, iVolume_Integer, Volume_Fraction);
    msDrvAudioSetDpgaMute(FALSE);
}


//------------------------------------------------------------------------------
/// @brief \b Function \b Name: msAPI_AudioSourceSel()
/// @brief \b Function \b Description: Select audio source
/// @param <IN>        \b
/// @return <OUT>      \b
//------------------------------------------------------------------------------
void msAPI_AudioSourceSel(void)
{
#if ENABLE_SPDIF
    #if ENABLE_ARC
    if(sAudioArc.eStatus == AUDIO_ARC_STS_ARC)
    {
        msDrvAudioSpdifMute(TRUE);
        msDrvAudioArcStsSet(AUDIO_ARC_STS_INIT);
    }
    #else
        msDrvAudioSpdifMute(TRUE);
    #endif
#endif

    if(CURRENT_INPUT_IS_DVI() && ENABLE_LINE_IN)
        eAudioSrc = AUDIO_LINE_IN;
    else if(CURRENT_INPUT_IS_HDMI())
        eAudioSrc = AUDIO_DIGITAL;
    else if(CURRENT_INPUT_IS_DISPLAYPORT())
        eAudioSrc = AUDIO_DIGITAL;
    else if(CURRENT_INPUT_IS_USBTYPEC())
        eAudioSrc = AUDIO_DIGITAL;
    else
        eAudioSrc = AUDIO_RESERVED;

    MSAUDIO_PRINT("eAudioSrc:%d\r\n", eAudioSrc);

    msDrvAudioSourceSel(eAudioSrc);
}


//------------------------------------------------------------------------------
/// @brief \b Function \b Name:  msAPI_AudioHandler()
/// @brief \b Function \b Description: Audio handler
/// @param <IN>        \b
/// @return <OUT>      \b
//------------------------------------------------------------------------------
void msAPI_AudioHandler(void)
{
    msDrvAudioTblProc();

#if TTS
    msAPI_AudioTTSPlay("\0");
#endif

    msDrvAudioFmtCheck();

#if ENABLE_ARC
    msDrvAudioArcHandler();
#endif
}


//------------------------------------------------------------------------------
/// @brief \b Function \b Name: msDrvAudioSineGen()
/// @brief \b Function \b Description: Audio internal sine gen
/// @param <IN>        \b bEnable, u8SinGenFreq(250Hz~12000Hz), u8SinGenGAIN(-90~0db)
/// @return <OUT>      \b
//------------------------------------------------------------------------------
void msAPI_AudioSineGen(Bool bEnable, AUDIO_PCM_FREQ eSinGenFreq, char u8SinGenGAIN) //high nibble for LCH,low nibble for RCH
{
    static BYTE u8BackupSourceSEL, u8BackupMute;
    static BOOL bInit = FALSE;
    BYTE u8SinGenGAINSel = (~(u8SinGenGAIN/6))+1;

    if(bInit == FALSE)
    {
		bInit = TRUE;
        msWrite2ByteMask(REG_112C1A, 0x1e00, 0x7fff);
        msWriteByteMask(REG_112C1D, BIT7|BIT6, BIT7|BIT6);
        msWriteByteMask(REG_112C1D, BIT7, BIT7|BIT6);
        msWriteByteMask(REG_112C09, 0x07, 0x07);
    }

    if(bEnable)
    {
        u8BackupSourceSEL = ((msReadByte(REG_112C05) & 0x70) >> 4);
        u8BackupMute = (((msReadByte(REG_112BFC) & 0x30) == 0x30)?TRUE: FALSE);
        msWriteByteMask(REG_112BFC, BIT5, BIT5|BIT4); // SW unmute to avoid HDMI source mute

        //sine gen path select
        msWriteByteMask(REG_112C05, (AUDIO_RESERVED << 4), 0x70);

        //frequency & gain
        msWrite2Byte(REG_112C16, (eSinGenFreq << 12) | (eSinGenFreq << 8) | (u8SinGenGAINSel << 4) | (u8SinGenGAINSel));

        //enable
        msWrite2Byte(REG_112C14, 0x301);
    }
    else
    {
        msAPI_AudioSetDpgaMute(u8BackupMute);
        msWriteByteMask(REG_112C05, (u8BackupSourceSEL << 4), 0x70);

        //enable
        msWrite2Byte(REG_112C14, 0x0000);
    }
}


#if TTS
void msDrvAudioTTSInit(void)
{
    ST_AUDIO_TTS_ADDR sTTSAddr;

    sTTSAddr.u32BufAddrSt = (MIU_AUDIO_ADDR_START + AUDIO_DELAY_DMA_SIZE);
    sTTSAddr.u32BufAddrEnd = (sTTSAddr.u32BufAddrSt + TTS_BUF_SIZE_PHY);
    sTTSAddr.u32InfoAddr = ALIGN_4(sTTSAddr.u32BufAddrEnd + (TTS_STR_MAX_SIZE *2));

    if(sTTSAddr.u32InfoAddr + sizeof(ST_AUDIO_TTS_INFO) <= (MIU_AUDIO_ADDR_START + MIU_AUDIO_SIZE))
    {
        sTTSAddr.sMbxReg.u32BufSt = MBX_TTS_BUF_START;
        sTTSAddr.sMbxReg.u32BufEnd = MBX_TTS_BUF_END;
        sTTSAddr.sMbxReg.u32StrSts = MBX_TTS_STR_STATUS;

        mDrvAudioTTSInfoToPQR2Init(sTTSAddr, TTS_STR_MAX_SIZE);

        if(mDrvAudioTTSIsSupport())
            msDrvAudioMixerBypass(FALSE);
    }
    else
    {
        MSAUDIO_PRINT("TTS size over MIU_AUDIO_SIZE\n");
    }
}


BOOL msAPI_AudioTTSPlay(char *strText)
{
    BOOL bRtn = FALSE;

    if(msAPI_AudioInitDone())
    {
        bRtn = mDrvAudioTTSPlay(strText);
    }

    return bRtn;
}


void msAPI_AudioTTSMixScale(BYTE u8Audio, BYTE u8TTS)
{
    // u8Audio: x/16, x = 0~16
    // u8TTS: x/16, x = 0~15

    mDrvAudioTTSMixScale(u8Audio, u8TTS);
}


void msAPI_AudioTTSSpeed(WORD u16Speed)
{
    mDrvAudioTTSSpeed(u16Speed);
}


void msAPI_AudioTTSMixEnable(BOOL bEnable)
{
    mDrvAudioTTSMixEnable(bEnable);
}


void msAPI_AudioTTSUserEnable(BOOL bEnable)
{
    mDrvAudioTTSUserEnable(bEnable);
}
#endif // TTS

#endif

