
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "Global.h"
#include "drvDAC.h"
#include "OsdDefault.h"
#include "msEread.h"

#if ENABLE_DAC
//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define AUDIO_DEBUG                     1
#define AUDIO_DELAY_FROCE_THRESHOLD     20  //ms
#define TBL_REG_END                     0x00
#define TBL_REG_DLY                     0xffffffff
#define TBL_REG_TAG                     0xff

#if ENABLE_DEBUG && AUDIO_DEBUG
#define MSAUDIO_PRINT(format, ...)      printf(format, ##__VA_ARGS__)
#else
#define MSAUDIO_PRINT(format, ...)
#endif


//-------------------------------------------------------------------------------------------------
// Global Variable
//-------------------------------------------------------------------------------------------------
volatile U16 u16TimerAudio = 0;
volatile BOOL audio_EnableAudioAfterSignalLock_Flag = 0;


//-------------------------------------------------------------------------------------------------
// Local Variable
//-------------------------------------------------------------------------------------------------
static const RegTblDlyType* pTblAudio   = NULL;
static BOOL bTblAudioForceDelay;
static BOOL bTblAudioDone;
static BYTE u8EfuseIdMask;


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
    {0x112c6e, 0xff, 0x20}, //[15:0]  2nd order synthesizer n.f[23:8]
    {0x112c6f, 0xff, 0x1c},
    {0x112c6c, 0xff, 0x00}, //[7:0]   2nd order synthesizer n.f[7:0]
    {0x112c6d, 0xff, 0xc0}, //[15]    2nd order synthesizer enable, [14] load
    {0x112c06, 0x10, 0x10}, //[4]     Enable reference clock

    //----------------init SRAM -------------------------//
    {0x112c47, 0x80, 0x80}, //[15] AUDIOBAND SRAM init
    {0x112e5e, 0x01, 0x01}, //[0] ini_DATAsram
    {0x112c07, 0x08, 0x08}, //[11] CLK_MAC_CKG
    {TBL_REG_DLY, 0x00, 0x01}, 
    {0x112c07, 0x08, 0x00}, //[11] CLK_MAC_CKG
    {0x112c47, 0x80, 0x00}, //[15] AUDIOBAND SRAM init
    {0x112e5e, 0x01, 0x00}, //[0] ini_DATAsram

    //----------------AUPLL control-------------------------//
    {0x112c60, 0xff, 0xd0},
    {0x112c61, 0xff, 0x12},
    {0x112c66, 0xff, 0x00},
    {0x112c67, 0xff, 0x03},

    //-----------------Input: Channel Source----------------//
    {0x112c05, 0x70, 0x20}, //0: HDMI Rx, 1: ADC0, 2: reserve

    //---------------- Enable CLK----------------------------//
    {0x112c08, 0xff, 0x40}, //[6]  SEL_CLK_DEC
    {0x112c07, 0x80, 0x80}, //[15] CLK_256FS_CHANNEL_IN
    {0x112c07, 0x40, 0x40}, //[14] CLK_ADC
    {0x112c07, 0x20, 0x20}, //[13] CLK_DAC
    {0x112c07, 0x10, 0x10}, //[12] CLK_216MHZ
    {0x112c07, 0x08, 0x08}, //[11] CLK_MAC_CKG
    {0x112c07, 0x04, 0x04}, //[10] CLK_54MHZ
    {0x112c07, 0x02, 0x02}, //[9 ] CLK_DMA_TGEN_256FS
    {0x112c07, 0x01, 0x01}, //[8 ] CLK_I2S_BCK
    {0x112c06, 0x80, 0x80}, //[7 ] CLK_HDMI_IF
    {0x112c06, 0x40, 0x40}, //[6 ] CLK_DEC
    {0x112c06, 0x20, 0x20}, //[5 ] CLK_I2S_TGEN_256FS
    {0x112c06, 0x10, 0x10}, //[4 ] CLK_2ND_ORDER_SYNTH
    {0x112c06, 0x08, 0x08}, //[3 ] CLK_AUPLL_REF_IN
    {0x112c06, 0x04, 0x04}, //[2 ] CLK_I2S_MCK
    {0x112c06, 0x02, 0x02}, //[1 ] CLK_SPDIF

    //----------------haydn reset---------------------------//
    {0x112c00, 0xff, 0xff},
    {0x112c00, 0xff, 0x00},

    //-----------------DPGA---------------------------------//
    {0x112ca4, 0x01, 0x01}, //[0] DPGA enable before reset, revised by jeff-cy.lee
    {0x112c00, 0x80, 0x80}, //0x112c00 [7] toggle DPGA sw-rstz
    {0x112c00, 0x80, 0x00}, //0x112c00 [7] toggle DPGA sw-rstz
    {0x112ca4, 0x70, 0x60}, //[6:4] fading step
    //3'd0:    0.125db for 127+1 samples 
    //3'd1:    0.125db for 63 +1 samples 
    //3'd2:    0.125db for 31 +1 samples 
    //3'd3:    0.125db for 15 +1 samples 
    //3'd4:    0.125db for 7  +1 samples 
    //3'd5:    0.125db for 3  +1 samples 
    //3'd6:    0.125db for 1  +1 samples 
    //3'd7:    0.125db for 0  +1 samples 
    //default: 0.125db for 0  +1 samples 
    {0x112ca3, 0x01, 0x01}, //[8] MUTE_2_ZERO

    // MST9U5 & TSUMR2 DPGA gain = 0dB after reset, mute by SW min gain
    {0x112ca4, 0x02, 0x00}, //[1] disable fading
    {0x112ca6, 0xff, 0x8f}, //0x112ca6[11:0] lef-channel gain
    {0x112ca7, 0x0f, 0x03}, //0x112ca6[11:0] lef-channel gain
    {0x112ca8, 0xff, 0x8f}, //0x112ca8[11:0] right-channel gain
    {0x112ca9, 0x0f, 0x03}, //0x112ca8[11:0] right-channel gain
    // (-480 ~ +911)*-0.125dB = +60dB ~ -113.875dB (mute)
    {0x112ca4, 0x80, 0x00}, //[7] gain trig
    {0x112ca4, 0x80, 0x80}, //[7] gain trig
    {0x112ca4, 0x80, 0x00}, //[7] gain trig
    {0x112ca4, 0x02, 0x02}, //[1] enable fading

    //-----------------AudioBand----------------------------//
    //DAC digital part:
    {0x112c58, 0xff, 0x80}, //DC_OFFSET
    {0x112c59, 0xff, 0x00}, //DC_OFFSET
    //{0x112c57, 0x20, 0x20}, //[13   ] DC_OFFSET_EN
    {0x112c5a, 0x80, 0x80}, //[7    ] DITHER_EN
    {0x112c5a, 0x0c, 0x08}, //[ 3: 2] DITHER_SEL 17L: x0.5
    {0x112c56, 0x10, 0x10}, //[4    ] FIX_MSB_EN
    {0x112c57, 0x01, 0x01}, //[8]   ] SDM_EN

    //DIGITAL OFFSET
    {0x112ea8, 0x01, 0x01}, //RIU Setting MD
    {0x112eab, 0x30, 0x00}, //RIU Setting CH SEL 0
    {0x112eab, 0x40, 0x00}, //RIU Setting CH SEL L
    {0x112eaa, 0xf0, 0x00}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x10}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x20}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x30}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x40}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x50}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x60}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x70}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x80}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x90}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xA0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xB0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xC0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xD0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xE0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xF0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x00}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x10}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x20}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x30}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x40}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x50}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x60}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x70}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x80}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x90}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xA0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xB0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xC0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xD0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xE0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xF0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eab, 0x40, 0x00}, //RIU Setting CH SEL R
    {0x112eaa, 0xf0, 0x00}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x10}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x20}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x30}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x40}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x50}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x60}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x70}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x80}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x90}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xA0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xB0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xC0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xD0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xE0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xF0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x00}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x10}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x20}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x30}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x40}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x50}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x60}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x70}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x80}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x90}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xA0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xB0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xC0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xD0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xE0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xF0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eab, 0x30, 0x10}, //RIU Setting CH SEL 1
    {0x112eab, 0x40, 0x00}, //RIU Setting CH SEL L
    {0x112eaa, 0xf0, 0x00}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x10}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x20}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x30}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x40}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x50}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x60}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x70}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x80}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x90}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xA0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xB0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xC0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xD0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xE0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xF0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x00}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x10}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x20}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x30}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x40}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x50}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x60}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x70}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x80}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x90}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xA0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xB0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xC0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xD0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xE0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xF0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eab, 0x40, 0x00}, //RIU Setting CH SEL R
    {0x112eaa, 0xf0, 0x00}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x10}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x20}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x30}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x40}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x50}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x60}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x70}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x80}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x90}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xA0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xB0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xC0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xD0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xE0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xF0}, //setting offset num
    {0x112eab, 0x01, 0x00}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x00}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x10}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x20}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x30}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x40}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x50}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x60}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x70}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x80}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0x90}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xA0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xB0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xC0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xD0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xE0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112eaa, 0xf0, 0xF0}, //setting offset num
    {0x112eab, 0x01, 0x01}, //setting offset num
    {0x112eac, 0xff, 0x0C},
    {0x112ead, 0xff, 0x02},
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x02, 0x02}, //RIU Setting update
    {0x112ea8, 0x02, 0x00}, //RIU Setting update
    {0x112ea8, 0x01, 0x00}, //RIU Setting MD
    {0x112eab, 0x40, 0x00}, //RIU Setting CH SEL L
    //DIGITAL OFFSET END

    //ADC digital part:
    {0x112c2b, 0x80, 0x80}, //[15] EN_ADC_DITHER

    {0x112e82, 0xff, 0x07},
    {0x112e83, 0xff, 0x00},
    {0x112e84, 0xff, 0x00},
    {0x112e85, 0xff, 0x80},
    {0x112e86, 0xff, 0x07},
    {0x112e87, 0xff, 0x00},
    {0x112e88, 0xff, 0x00},
    {0x112e89, 0xff, 0x80},
    {0x112e92, 0xff, 0x07},
    {0x112e93, 0xff, 0x00},
    {0x112e94, 0xff, 0x00},
    {0x112e95, 0xff, 0x80},
    {0x112e96, 0xff, 0x07},
    {0x112e97, 0xff, 0x00},
    {0x112e98, 0xff, 0x00},
    {0x112e99, 0xff, 0x80},
    {0x112e7a, 0xff, 0x03},
    {0x112e7b, 0xff, 0x00},
    {0x112e7c, 0xff, 0x00},
    {0x112e7d, 0xff, 0xC0},
    {0x112e7e, 0xff, 0x03},
    {0x112e7f, 0xff, 0x00},
    {0x112e80, 0xff, 0x00},
    {0x112e81, 0xff, 0xC0},
    {0x112e8a, 0xff, 0x03},
    {0x112e8b, 0xff, 0x00},
    {0x112e8c, 0xff, 0x00},
    {0x112e8d, 0xff, 0xC0},
    {0x112e8e, 0xff, 0x03},
    {0x112e8f, 0xff, 0x00},
    {0x112e90, 0xff, 0x00},
    {0x112e91, 0xff, 0xC0},

#if !ENABLE_I2S
    //---------------------------De-POP On---------------------------//
    //   I2S do not do DEPOP part which will enable analog engine
    //   because analog and I2S are the same pins
    //---------------------------------------------------------------//

    //---After Power good is ready
#if ENABLE_EAR_OUT
    {0x112ce1, 0x10, 0x10}, //VMID_PULL_LOW_EAR=1([12] EAR_OUT pull low open)
#endif

#if ENABLE_LINE_OUT
    {0x112ce1, 0x20, 0x20}, //VMID_PULL_LOW_LINE=1([13] LINE_OUT pull low open)
#endif

    {0x112cd8, 0xff, 0xff},
    {0x112cd9, 0xff, 0xff},
    {0x112ce8, 0x40, 0x40},
    //this is as much as possible
    {TBL_REG_DLY, 0x01, 0x2C},
    {0x112cd8, 0xff, 0x00},
    {0x112cd9, 0xff, 0x00},
    {0x112ce8, 0x40, 0x00},
    //---

    {0x112e61, 0x0f, 0x00}, //DRE disable
    // DEPOP ON START
    {TBL_REG_TAG, 0x00, 0x04},
#if ENABLE_LINE_OUT
    {0x112cf2, 0x7f, 0x3f}, //GCR_TEST_AUSDM[7:0]=7f
#endif
    {TBL_REG_TAG, 0x00, 0xFF},		
    {0x112cf0, 0x10, 0x10}, //EN_MUTE_EAR=1
    {0x112ce4, 0x04, 0x04}, //EN_MUTE_LINE=1

#if ENABLE_EAR_OUT
    {0x112cf1, 0x08, 0x08},
#endif

#if ENABLE_LINE_OUT
    {0x112cf1, 0x10, 0x10},
#endif
    {0x112cdb, 0x04, 0x00}, //PD_IBIAS_DAC=0


    {TBL_REG_TAG, 0x00, 0x04},
#if ENABLE_LINE_OUT
    {0x112ce1, 0x20, 0x00}, //EN_VMID2GND_LINE=0
#endif


    {TBL_REG_TAG, 0x00, 0xFF},
    // DEPOP ON LINE : 0m //
#if ENABLE_EAR_OUT
    {0x112cef, 0x01, 0x01}, //EN_CHOP_EAR=1
#endif

#if ENABLE_LINE_OUT
    {0x112cef, 0x02, 0x02}, //EN_CHOP_LINE=1
#endif

    {0x112cdd, 0x33, 0x33}, //SEL_CHOP_FREQ_DAC=11
    {0x112ce1, 0xc0, 0xc0}, //EN_CHOP_PHASE_DAC1&2=1,1


    {TBL_REG_TAG, 0x00, 0x04},
    {0x112cda, 0x01, 0x01}, //PD_ADC0=1


    {TBL_REG_TAG, 0x00, 0xFF},
#if ENABLE_EAR_OUT
    {0x112cda, 0x22, 0x00}, //PD_ADC0, PD_DAC, PD_IBIAS_LINE, PD_BIAS_DAC,PD_IBIAS_EAR, PD_RT_EAR, PD_LT_EAR, PD_LT_LINE
#endif

#if ENABLE_LINE_OUT
    {0x112cda, 0x44, 0x00}, //PD_ADC0, PD_DAC, PD_IBIAS_LINE, PD_BIAS_DAC,PD_IBIAS_EAR, PD_RT_EAR, PD_LT_EAR, PD_LT_LINE
#endif

    //{0x112cdb, 0x10, 0x00}, //PD_IBIAS_EAR=0
    //{0x112cdb, 0x60, 0x00}, //PD_LT_EAR PD_RT_EAR=0
    //{0x112cdb, 0x01, 0x00}, //PD_IBIAS_LINE=0
    //{0x112cdb, 0x80, 0x00}, //PD_LT_LINE=0


    {TBL_REG_TAG, 0x00, 0x03},
#if ENABLE_EAR_OUT
    {0x112cdb, 0x70, 0x00}, //PD_LT_LINE=1
#endif

#if ENABLE_LINE_OUT
    {0x112cdb, 0x81, 0x00}, //PD_LT_LINE=0
#endif

    //{0x112cdc, 0x08, 0x00}, //PD_VI[3]=0
    //{0x112cdc, 0x02, 0x00}, //PD_OPLP_EAR[1]=0
    //{0x112cdc, 0x04, 0x00}, //PD_OPLP_LINE[2]=0
    //{0x112cdc, 0xe0, 0x80}, //PD_LDO_ADC[5]=0, PD_LDO_DAC[6]=0, EN_QS_LDO_ADC[7]=1

    {0x112cdc, 0xe8, 0x80},


    {TBL_REG_TAG, 0x00, 0x04},	
    //{0x112cdb, 0x04, 0x00}, //PD_BIAS_DAC=0
#if ENABLE_EAR_OUT
    {0x112cdb, 0x74, 0x00}, //PD_RT_EAR[13]=0, PD_LT_EAR[14]=0, PD_IBIAS_EAR[12]=0, PD_BIAS_DAC[10]=0
#endif

#if ENABLE_LINE_OUT
    {0x112cdb, 0x85, 0x00}, //PD_LT_LINE[15]=0, PD_IBIAS_LINE[8]=0, PD_BIAS_DAC[10]=0
#endif

    //{0x112cdc, 0x08, 0x00}, //PD_VI[3]=0
    //{0x112cdc, 0x02, 0x00}, //PD_OPLP_EAR[1]=0
    //{0x112cdc, 0x04, 0x00}, //PD_OPLP_LINE[2]=0
    //{0x112cdc, 0xe0, 0x80}, //PD_LDO_ADC[5]=0, PD_LDO_DAC[6]=0, EN_QS_LDO_ADC[7]=1

    {0x112cdc, 0xe8, 0x81}, //PD_INMUX=0, PD_VI=0, SEL_CHOP_FREQ_DAC=01


    {TBL_REG_TAG, 0x00, 0xFF},
#if ENABLE_EAR_OUT
    {0x112cdc, 0x02, 0x00},
#endif

#if ENABLE_LINE_OUT
    {0x112cdc, 0x04, 0x00},
#endif

    {0x112cdd, 0x01, 0x01}, //EN_QS_LDO_DAC=1

#if ENABLE_LINE_OUT
    {0x112cdf, 0x80, 0x00}, //PD_RT_LINE=0
#endif

    {0x112ce1, 0x10, 0x00}, //EN_VMID2GND_EAR
    {0x112ce2, 0x0c, 0x04}, //SEL_IN_LINE[1:0]=01
    {0x112cdf, 0x06, 0x02}, //SEL_IN_EAR[1:0]=01

    {0x112ce8, 0x40, 0x00}, //EN_VREF_DISCH=0

#if ENABLE_LINE_OUT
    {0x112cf0, 0x80, 0x80}, //EN_STG2_LP_LINE=1
#endif

    //{0x112cf0, 0x10, 0x00}, //EN_MUTE_EAR
    {0x112cf0, 0x90, 0x80}, //EN_MUTE_EAR

    {0x112cf1, 0x01, 0x01}, //EN_STG2_LP=1
    {0x000b00, 0x02, 0x00}, //PD_VREF=0

    // DEPOP ON LINE  : 1m /
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112ce4, 0x04, 0x00}, //EN_MUTE_LINE

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
    {0x112cd7, 0xff, 0x03},
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

    {0x112ce5, 0x80, 0x80}, //EN_MSP=1

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
    {0x112cd4, 0xff, 0x3a}, //SEL_DEPOP_LINE[7:0]=16'h3a
    // DEPOP ON LINE  : 16m //
    // DEPOP ON LINE  : 19m //
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112ce9, 0x0c, 0x08}, //SFTDCH[1:0]=10


    {TBL_REG_TAG, 0x00, 0x03},
    // DEPOP ON STEP 1 : 17m //
    {TBL_REG_DLY, 0x01, 0xF4},


    {TBL_REG_TAG, 0x00, 0x04},

    // DEPOP ON STEP 1 : 519m //
    {TBL_REG_DLY, 0x00, 0x03},	


    {TBL_REG_TAG, 0x00, 0xFF},
    {0x112ce9, 0x0c, 0x00}, //SFTDCH[1:0]=00
    {TBL_REG_TAG, 0x00, 0x04},
    {0x112cd8, 0xff, 0x01}, //EN_VREF_SFTMSP[7:0]=8'b0000,0001
    // DEPOP ON STEP 1 : 517m //
    // DEPOP ON STEP 1 : 569m //
    {TBL_REG_TAG, 0x00, 0x03},
    {TBL_REG_DLY, 0x00, 0x14},
    {TBL_REG_TAG, 0x00, 0x04},
    {TBL_REG_DLY, 0x00, 0x05},
    {TBL_REG_TAG, 0x00, 0xFF},
    {0x112ce9, 0x0c, 0x04}, //SFTDCH[1:0]=01
    // DEPOP ON STEP 1 : 537m //
    // DEPOP ON STEP 1 : 589m //
    {TBL_REG_TAG, 0x00, 0x03},
    {TBL_REG_DLY, 0x00, 0x14},
    {0x112cd8, 0xff, 0x01}, //EN_VREF_SFTMSP[7:0]=8'b0000,0001
    {TBL_REG_TAG, 0x00, 0x04},
    {TBL_REG_DLY, 0x00, 0xAA},
    // DEPOP ON STEP 1 : 557m //
    // DEPOP ON STEP 1 : 607m //
    {TBL_REG_TAG, 0x00, 0x03},    
    {TBL_REG_DLY, 0x00, 0xC8},
    {TBL_REG_TAG, 0x00, 0x04},
    {TBL_REG_DLY, 0x00, 0x0A},
    {TBL_REG_TAG, 0x00, 0xFF},
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
    {TBL_REG_DLY, 0x00, 0x0A},
    {0x112cd8, 0xff, 0xff}, //EN_VREF_SFTMSP[7:0]=8'b1111,11111
    // DEPOP ON STEP 1 : 778m //
    // DEPOP ON STEP 1 : 828m //

    {TBL_REG_DLY, 0x00, 0x05},
    {0x112cd9, 0xff, 0x01}, //EN_VREF_SFTMSP[8]=1
    // DEPOP ON EAR : 783m //
    // DEPOP ON EAR : 833m //
    {TBL_REG_DLY, 0x00, 0xAF},
    {0x112cd7, 0xff, 0x07},
    // DEPOP ON EAR : 958m //
    // DEPOP ON EAR : 1008m //
    {TBL_REG_DLY, 0x00, 0x0A},
    {0x112cd6, 0xff, 0xef}, //SEL_DEPOP_EAR[11:0]=7ef
    // DEPOP ON EAR : 968m //
    // DEPOP ON EAR : 1018m //
    {TBL_REG_DLY, 0x00, 0x05},
    {0x112cd6, 0xff, 0xff}, //SEL_DEPOP_EAR[11:0]=7ff


    {TBL_REG_TAG, 0x00, 0x03},
    // DEPOP ON EAR : 973m //
    {TBL_REG_DLY, 0x00, 0x2E},
    {0x112ce8, 0x01, 0x01}, //EN_STG2_AB_EAR=1


    // DEPOP ON LINE  : 1019m //
    {TBL_REG_DLY, 0x00, 0x8D},


    {TBL_REG_TAG, 0x00, 0x04}, 
    // DEPOP ON LINE  : 1209m //
    {TBL_REG_DLY, 0x00, 0x07},


    {TBL_REG_TAG, 0x00, 0xFF}, 
    {0x112cd5, 0xff, 0x60}, //SEL_DEPOP_LINE[14:8]=16'h60
    // DEPOP ON LINE  : 1160m //
    // DEPOP ON LINE  : 1210m //
    {TBL_REG_DLY, 0x00, 0x03},
    {0x112cd5, 0xff, 0x61}, //SEL_DEPOP_LINE[14:8]=16'h61
    // DEPOP ON LINE  : 1163m //
    // DEPOP ON LINE  : 1213m //
    {TBL_REG_DLY, 0x00, 0x0A},
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
    {TBL_REG_DLY, 0x00, 0x0A},
    {0x112cd4, 0xff, 0xf7}, //SEL_DEPOP_LINE[7:0]=16'hf7
    {0x112ce8, 0x02, 0x02}, //EN_STG2_AB_LINE=1
    
    {TBL_REG_TAG, 0x00, 0x04}, 
    {TBL_REG_DLY, 0x00, 0x14}, //{TBL_REG_DLY, 0x00, 0x0A},
    {0x112ce8, 0x01, 0x01}, //EN_STG2_AB_EAR=1
    
    //{TBL_REG_DLY, 0x00, 0x01},
    //{0x112ce4, 0x80, 0x00}, //SEL_TC_LINE
    //{0x112ce5, 0x01, 0x01}, //SEL_TC_LINE

    //{0x112ce5, 0x03, 0x02}, //EN_ISEL_LINE
    //{TBL_REG_DLY, 0x00, 0x0A},
    //{0x112ce1, 0x20, 0x00}, //VMID_PULL_LOW_LINE=0

    {TBL_REG_TAG, 0x00, 0x0FF},
    {0x112cdc, 0x80, 0x00}, //EN_QS_LDO_ADC=0
    {0x112cdd, 0x01, 0x00}, //EN_QS_LDO_DAC=0

    {TBL_REG_TAG, 0x00, 0x04},
#if ENABLE_LINE_OUT
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


    {TBL_REG_TAG, 0x00, 0xFF},
    // DEPOP ON LINE  : 1213m //
    // DEPOP ON LINE  : 1298m //
    // depop on finish................
#endif

#if ENABLE_LINE_IN
    //-----------------Input: ADC---------------------------//
    {0x112cdc, 0x01, 0x00}, //[0]   PD_INMUX0
    {0x112cda, 0x01, 0x00}, //PD_ADC0, PD_DAC, PD_IBIAS_LINE, PD_BIAS_DAC,PD_IBIAS_EAR, PD_RT_EAR, PD_LT_EAR, PD_LT_LINE
    {0x112ce3, 0x02, 0x02}, //[9]   EN_CHOP_ADC0
    {0x112ce3, 0x20, 0x20}, //[13]  EN_DIT_ADC0
    {0x112ce8, 0x80, 0x00}, //[9:7] INMUX_GAIN
    {0x112ce9, 0x03, 0x00}, //0:0dB, 1:-6dB, 2:-3dB(default), 3:3dB, 4:6dB, 5:9dB, 6:12dB
    {0x112ce0, 0xe0, 0x00}, //[7:5] SEL_CH_INMUX
    {0x112cee, 0x18, 0x18}, //[4:3] SEL_DIT_VOL_L_ADC0 SEL_DIT_VOL_R_ADC0
#endif

#if ENABLE_I2S
    {0x112e69, 0x01, 0x01}, //Disable SRC dither
    
    //-----------------Output: I2S (BCK 64fs, msater)-------//
    //decimation filt. gain compensation
    {0x112d40, 0x01, 0x01}, //[0] Enable gain compensation
    {0x112d42, 0xff, 0x12}, //L ch gain 0.7dB(0x115), 0.6dB(0x112), 0.5dB(0x10f), 0.4dB(0x10c), 0.3dB(0x108), 0.2dB(0x105), 0.1dB(0x102)
    {0x112d43, 0xff, 0x01}, //
    {0x112d44, 0xff, 0x12}, //R ch gain
    {0x112d45, 0xff, 0x01}, //
    {0x112d40, 0x02, 0x02}, //[1] toggle to update gain
    {0x112d40, 0x02, 0x00}, //[1] toggle to update gain
    //I2S out
    {0x112c09, 0x01, 0x00}, //[8]     I2S BCK select 0:PLL 1:TGEN
    {0x112c08, 0x20, 0x00}, //[5]     I2S TGEN clock select 0:PLL 1:CH_IN
    {0x112c81, 0x80, 0x80}, //[15]    I2S_OUT_FMT
    {0x112c81, 0x60, 0x40}, //[14:13] I2S_BCK_SEL & SEL_CLK_I2S_BCK_PLL
    {0x112c80, 0x08, 0x00}, //[3]     I2S_OUT_OEZ
    {0x112c80, 0x04, 0x00}, //[2]     I2S_OUT_MUTE_OEZ
    {0x112c82, 0x07, 0x02}, //[2:0]   SEL_CLK_I2S_MCK 256FS
#endif

    //-----------------Input: HDMI (only haydn part)--------//
    {0x112d60, 0x10, 0x00}, //[4]    HDMI_CH_MATRIX_CFG_W
    {0x112d60, 0x07, 0x00}, //[2:0]  HDMI_CH_MATRIX_WD
    {0x112d60, 0x10, 0x10}, //[4]    HDMI_CH_MATRIX_CFG_W
    {0x112d60, 0x07, 0x01}, //[2:0]  HDMI_CH_MATRIX_WD
    {0x112d61, 0x7f, 0x7f}, //[14:8] HDMI RX LAUNCH CFG
    {0x112d62, 0x0c, 0x08}, //[3:2]  HDMI status SEL
    {0x112d62, 0x30, 0x00}, //[6:4]  HDMI status channel SEL
    {0x112d62, 0x01, 0x01}, //[0]    HDMI status left justified
    {0x112d62, 0x02, 0x00}, //[1]    HDMI status MSB first
    {0x112d63, 0x3f, 0x03}, //[13:8] HDMI status block start SEL
    {0x112d6e, 0x10, 0x10}, //[4]    HDMI npcm detect byte mode
    {0x112d6e, 0x08, 0x08}, //[3]    HDMI npcm detect PA/PB order
    {0x112d6e, 0x04, 0x00}, //[2]    HDMI npcm detect fast lock
    {0x112d6e, 0x03, 0x00}, //[1:0]  HDMI npcm detect time out SEL
    {0x112d6c, 0x01, 0x01}, //[0]    HDMI vld re-gen

    //-----------------Output: SPDIF TX---------------------//
    {0x112c88, 0x20, 0x20}, //0x112c88 [5]  REG_EN_AutoBlkStart
    {0x112c88, 0x40, 0x00}, //0x112c88 [6]  REG_VALIDITY_BIT_SPDIF
    {0x112c89, 0x03, 0x00}, //0x112c88 [9:8]REG_SEL_SPDIF_CH
    {0x112c89, 0x08, 0x08}, //0x112c88 [11] REG_SPDIF_OUT_CS_MODE
    {0x112c89, 0x20, 0x20}, //0x112c88 [13] REG_SPDIF_OUT_US_MODE
    {0x112c8a, 0x10, 0x00}, //0x112c8a [4]  SPDIF_SEL_DEBUG   0:from CH_IN 1:from I2S source
    {0x112c08, 0x02, 0x00}, //0x112c08 [1]  REG_SEL_CLK_SPDIF 0:from CH_IN 1:from AUPLL
    {0x112c8b, 0x02, 0x02}, //0x112c8a [9]  REG_SPDIF_CS_FR_HDMIRX
    {0x112c8b, 0x04, 0x04}, //0x112c8a [10] REG_SPDIF_CS_VALID_SEL
    {0x112c8a, 0x01, 0x01}, //0x112c8a [0]  SPDIF_OUT_OEZ

    //end of table
    {TBL_REG_END, 0x00, 0x00}
};


const RegTblDlyType tblAudioDCOff[] =
{
	{TBL_REG_TAG, 0x00, 0xFF},
#if !ENABLE_I2S
    //-----------------De-POP Off----------------------------//
    //DPGA SW mute
    {0x112ca1, 0x08, 0x08}, //0x112ca0 [11] DPGA SW mute mode
    {0x112ca1, 0x04, 0x00}, //0x112ca0 [10] DPGA SW mute =0
    {0x112ca1, 0x04, 0x04}, //0x112ca0 [10] DPGA SW mute =1


    {TBL_REG_TAG, 0x00, 0x04},
#if ENABLE_LINE_OUT
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
    //{TBL_REG_DLY, 0x00, 0x02},
    //{0x112cf2, 0xff, 0x7f}, //GCR_TEST_AUSDM[7:0]
#endif    


    {TBL_REG_TAG, 0x00, 0xFF},
    {0x112cdc, 0x80, 0x80}, //EN_QS_LDO_ADC=1
    {0x112cdd, 0x01, 0x01}, //EN_QS_LDO_DAC=1

    // DEPOP  OFF  : 12m //

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
    {0x112ce8, 0x03, 0x00},
    {0x112cd6, 0xff, 0xef}, //SEL_DEPOP_EAR[11:0]=4ef
    {0x112cd7, 0xff, 0x04},

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

    {TBL_REG_TAG, 0x00, 0x04}, 
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd5, 0xff, 0x00}, //SEL_DEPOP_LINE[14:8]=16'h00
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd5, 0xff, 0x08}, //SEL_DEPOP_LINE[14:8]=16'h08
    
    {TBL_REG_TAG, 0x00, 0xFF},     
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
    {0x112ce8, 0x40, 0x40}, //EN_VREF_DISCH=1
    // DEPOP OFF  : 24m //
    // DEPOP OFF  : 38m //
    {TBL_REG_DLY, 0x00, 0x05},
    {0x112ce9, 0x0c, 0x08}, //SFTDCH[1:0]=10


    {TBL_REG_TAG, 0x00, 0x03},
    // DEPOP OFF  : 29m //
    {TBL_REG_DLY, 0x00, 0x05},


    {TBL_REG_TAG, 0x00, 0x04},
    // DEPOP OFF  : 43m //
    {TBL_REG_DLY, 0x00, 0x05},


    {TBL_REG_TAG, 0x00, 0xFF},
    {0x112ce9, 0x0c, 0x00}, //SFTDCH[1:0]=00
    // DEPOP OFF  : 34m //
    // DEPOP OFF  : 143m //

    {TBL_REG_TAG, 0x00, 0x03},
    {TBL_REG_DLY, 0x00, 0x14},
    //{0x112ce9, 0x0c, 0x04}, //SFTDCH[1:0]=01
    {TBL_REG_TAG, 0x00, 0x04},
    {TBL_REG_DLY, 0x00, 0x05},

    {TBL_REG_TAG, 0x00, 0x03},
    // DEPOP OFF  : 54m //
    
#if QUICK_DEPOP_OFF
    {TBL_REG_DLY, 0x00, 0x00},
#else
    {TBL_REG_DLY, 0x00, 0xC8},
#endif

    {TBL_REG_TAG, 0x00, 0x04},
    // DEPOP OFF  : 163m //
    {TBL_REG_DLY, 0x00, 0x05},


    {TBL_REG_TAG, 0x00, 0xFF},
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
    {TBL_REG_DLY, 0x00, 0x0A},
    {0x112cd8, 0xff, 0x3f}, //EN_VREF_SFTMSP[7:0]=8'b0011,1111
    // DEPOP OFF  : 284m //
    // DEPOP OFF  : 343m //
    {TBL_REG_TAG, 0x00, 0x03},
    {TBL_REG_DLY, 0x00, 0x14},
    {TBL_REG_TAG, 0x00, 0x04},
    {TBL_REG_DLY, 0x00, 0x0A},
    {TBL_REG_TAG, 0x00, 0xFF},
    {0x112cd8, 0xff, 0x7f}, //EN_VREF_SFTMSP[7:0]=8'b0111,1111
    // DEPOP OFF  : 304m //
    // DEPOP OFF  : 363m //
    {TBL_REG_TAG, 0x00, 0x03},
    {TBL_REG_DLY, 0x00, 0x14},
    {TBL_REG_TAG, 0x00, 0x04},
    {TBL_REG_DLY, 0x00, 0x0A},
    {TBL_REG_TAG, 0x00, 0xFF},
    {0x112cd8, 0xff, 0xff}, //EN_VREF_SFTMSP[7:0]=8'b1111,11111
    // DEPOP OFF  : 324m //
    // DEPOP OFF  : 383m //
    
    {TBL_REG_TAG, 0x00, 0x03},
    
#if QUICK_DEPOP_OFF
    {TBL_REG_DLY, 0x00, 0x00},
#else
    {TBL_REG_DLY, 0x05, 0x6E},
#endif

    {TBL_REG_TAG, 0x00, 0x04},
    {TBL_REG_DLY, 0x01, 0xA4}, // {TBL_REG_DLY, 0x01, 0x86},
    {TBL_REG_TAG, 0x00, 0xFF},

    {0x112cd5, 0xff, 0x08}, //SEL_DEPOP_LINE[14:8]=16'h08
    // DEPOP  OFF  : 1614m //
    // DEPOP  OFF  : 1773m //
    {TBL_REG_DLY, 0x00, 0x32}, //{TBL_REG_DLY, 0x00, 0x28},
    {0x112cd5, 0xff, 0x18}, //SEL_DEPOP_LINE[14:8]=16'h18
    // DEPOP  OFF  : 1654m //
    // DEPOP  OFF  : 1813m //
    {TBL_REG_DLY, 0x00, 0x2D}, //{TBL_REG_DLY, 0x00, 0x28},
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
    {0x112cdc, 0x02, 0x02}, //PD_OPLP_EAR=1

#if ENABLE_EAR_OUT
    {0x112ce1, 0x10, 0x10}, //EN_VMID2GND_EAR=1
#endif

    {0x112cdb, 0x60, 0x60}, //PD_LT_EAR=1 PD_RT_EAR=1
    {0x112cf1, 0x01, 0x00}, //EN_STG2_LP_EAR=0

    // DEPOP  OFF  : 1724m //
    // DEPOP  OFF  : 1883m //
    {TBL_REG_TAG, 0x00, 0x03}, 
    {TBL_REG_DLY, 0x00, 0x32},
    {TBL_REG_TAG, 0x00, 0x04}, 
    {TBL_REG_DLY, 0x00, 0x0A},
    {TBL_REG_TAG, 0x00, 0xFF}, 
    {0x112ce8, 0x40, 0x00}, //EN_VREF_DISCH=0
    // DEPOP  OFF  : 1729m //
    // DEPOP  OFF  : 1933m //
    //{0x112cdb, 0x01, 0x01}, //PD_IBIAS_LINE=1
    //{0x112cdb, 0x80, 0x80}, //PD_LT_LINE=1
    {0x112cdb, 0x81, 0x81},

    {0x112cdc, 0x04, 0x04}, //PD_OPLP_LINE=1

    //{0x112cdc, 0x0c, 0x0c},

#if ENABLE_LINE_OUT
    {0x112ce1, 0x20, 0x20}, //EN_VMID2GND_LINE=1
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


    {TBL_REG_TAG, 0x00, 0x04},
#if ENABLE_LINE_OUT
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


    {TBL_REG_TAG, 0x00, 0xFF},
    //Depop off final (return to default value..)
    {0x112cda, 0x67, 0xff},
    {0x112cdb, 0xf7, 0xff},
    {0x112cdc, 0xff, 0x7f},
    {0x112cdd, 0xff, 0x3e},
    {0x112cde, 0xff, 0x00},
    {0x112cdf, 0xff, 0x80},
    {0x112ce0, 0xff, 0x00},
    //{0x112ce1, 0xff, 0x00},


    {TBL_REG_TAG, 0x00, 0x04},
#if ENABLE_EAR_OUT
    {0x112ce1, 0xff, 0x10}, //
#endif

#if ENABLE_LINE_OUT
    {0x112ce1, 0xff, 0x20}, //
#endif


    {TBL_REG_TAG, 0x00, 0xFF},
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
    
    {TBL_REG_TAG, 0x00, 0x04},
    {TBL_REG_DLY, 0x00, 0x01},
    {0x112cd8, 0xff, 0xff},
    {0x112cd9, 0xff, 0xff},
    {0x112ce8, 0x40, 0x40},
    {TBL_REG_DLY, 0x00, 0x01},
    {TBL_REG_TAG, 0x00, 0xFF},
#endif

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


//------------------------------------------------------------------------------
/// @brief \b Function \b Name:  msDrvDPGAMuteConfig()
/// @brief \b Function \b Description: DPGA mute by audio mute and AV mute in HW mode
/// @return <OUT>      \b
//------------------------------------------------------------------------------
static void msDrvDPGAMuteConfig(void)
{
    msWrite2ByteMask(REG_112CA0, BIT0|BIT1|BIT2, BIT0|BIT1|BIT2);
}


#if ENABLE_SPDIF
static void msDrvAudioSpdifMute(BOOL bEnable)
{
    MSAUDIO_PRINT("Disable SPDIF output = %d\n", bEnable);
    msWriteByteMask(REG_112C8A, bEnable, BIT0);
}


void msAPI_AudioSpdifMute(BOOL bEnable)
{
    msDrvAudioSpdifMute(bEnable);
}
#endif


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
    msWriteByteMask(REG_112CA4, 0x00, BIT7);
    msWriteByteMask(REG_112CA4, BIT7, BIT7);
    msWriteByteMask(REG_112CA4, 0x00, BIT7);
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
            msWrite2ByteMask(REG_112CA6, u16DPGAGain, 0x0FFF);
            break;

        case E_AUDIO_RIGHT_CH:
            msWrite2ByteMask(REG_112CA8, u16DPGAGain, 0x0FFF);
            break;

        case E_AUDIO_LEFT_RIGHT_CH:
            msWrite2ByteMask(REG_112CA6, u16DPGAGain, 0x0FFF);
            msWrite2ByteMask(REG_112CA8, u16DPGAGain, 0x0FFF);
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
            msWrite2ByteMask(REG_112CA6, reg16_LCHVolume, 0x0FFF);
            break;

        case E_AUDIO_RIGHT_CH:
            msWrite2ByteMask(REG_112CA8, reg16_RCHVolume, 0x0FFF);
            break;

        case E_AUDIO_LEFT_RIGHT_CH:
            msWrite2ByteMask(REG_112CA6, reg16_LCHVolume, 0x0FFF);
            msWrite2ByteMask(REG_112CA8, reg16_RCHVolume, 0x0FFF);
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
void  msDrvAudioSetDpgaMute(Bool enable)
{
    //SW mode: BIT11 = 1, HW mode: BIT11 = 0
    if(enable)
        msWrite2ByteMask(REG_112CA0, BIT11|BIT10, BIT11|BIT10);
    else if(((msReadByte(REG_112C05) & 0x70) >> 4) == AUDIO_LINE_IN)
        msWrite2ByteMask(REG_112CA0, BIT11, BIT11|BIT10);
    else
        msWrite2ByteMask(REG_112CA0, 0x00, BIT11|BIT10);
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

    switch(src)
    {
        case AUDIO_LINE_IN:
            msWrite2ByteMask(REG_112C04, src<<12, BIT14|BIT13|BIT12);   //inmux switch
            break;

        case AUDIO_DIGITAL:
            msWrite2ByteMask(REG_112C04, src<<12, BIT14|BIT13|BIT12);   //inmux switch
            break;

        default:
            msWrite2ByteMask(REG_112C04, src<<12, BIT14|BIT13|BIT12);
            break;
    }
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


static void msDrvAudioHDMISwap(BOOL bEnable)
{
    msWriteByteMask(REG_112D60, 0x00, BIT4); // OutCh_1
    msWriteByteMask(REG_112D60, (bEnable?0x01:0x00), BIT2|BIT1|BIT0);
    DelayUs(1);

    msWriteByteMask(REG_112D60, BIT4, BIT4); // OutCh_2
    msWriteByteMask(REG_112D60, (bEnable?0x00:0x01), BIT2|BIT1|BIT0);
    DelayUs(1);
}


static void msDrvAudioSDMSwap(BOOL bEnable)
{
    msWriteByteMask(REG_112C53, (bEnable << 7), BIT7);
}


//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
///
///           AUDIO API FUNCTION
///
////////////////////////////////////////////////////////////////////////////////
//******************************************************************************
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


void msAPI_AudioHDMISwap(BOOL bEnable)
{
    msDrvAudioHDMISwap(bEnable);
}


void msAPI_AudioSDMSwap(BOOL bEnable) // EAR_OUT and LINE_OUT L/R swap
{
    msDrvAudioSDMSwap(bEnable);
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
        msAPI_AudioI2SConfig(AUDIO_I2S_FORMAT_LEFT_JUSTIFY,AUDIO_I2S_BCK_AU48FS,AUDIO_I2S_MCK_AU256FS);
        msDrvAudioPadSetting();
        msDrvDPGAMuteConfig();
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
        msAPI_AudioI2SConfig(AUDIO_I2S_FORMAT_LEFT_JUSTIFY,AUDIO_I2S_BCK_AU48FS,AUDIO_I2S_MCK_AU256FS);
        msDrvAudioPadSetting();
        msDrvDPGAMuteConfig();
    }
}


//------------------------------------------------------------------------------
/// @brief \b Function \b Name:  msAPI_AudioSetDpgaMute()
/// @brief \b Function \b Description: Mute from DPGA hardware block
/// @param <IN>        \b enable
/// @return <OUT>      \b
//------------------------------------------------------------------------------
void msAPI_AudioSetDpgaMute(Bool enable)
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
    AUDIO_SOURCE_SEL eAudioSrc;

    if (CURRENT_INPUT_IS_VGA() && ENABLE_LINE_IN)
        eAudioSrc = AUDIO_LINE_IN;
    else if(CURRENT_INPUT_IS_DVI())
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
    BYTE u8SinGenGAINSel = (~(u8SinGenGAIN/6))+1;

    if(bEnable)
    {
        u8BackupSourceSEL = ((msReadByte(REG_112C05) & 0x70) >> 4);
        u8BackupMute = (((msRead2Byte(REG_112CA0) & 0x0C00) == 0x0C00)?TRUE: FALSE);
        msWrite2ByteMask(REG_112CA0, BIT11, BIT11|BIT10); //SW unmute to avoid HDMI source mute
    }
    else
    {
        msAPI_AudioSetDpgaMute(u8BackupMute);
    }

    //sine gen path select
    msWrite2ByteMask(REG_112C04, ((bEnable)?(AUDIO_RESERVED<<12):(u8BackupSourceSEL<<12)), BIT14|BIT13|BIT12); //inmux switch
    msWrite2ByteMask(REG_112C1C, ((bEnable)?(BIT15):(0)), BIT15|BIT14);   //[15] REG_DMA_SYNTH_ENABLE; [14] REG_DMA_SYNTH_CHANGE_FREQ

    //frequency & gain
    msWrite2ByteMask(REG_112C14, (eSinGenFreq<<4)|(eSinGenFreq), BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);
    msWrite2ByteMask(REG_112C16, (u8SinGenGAINSel<<8)|(u8SinGenGAINSel), BIT12|BIT11|BIT10|BIT9|BIT8|BIT4|BIT3|BIT2|BIT1|BIT0);

    //enable
    msWrite2ByteMask(REG_112C14, ((bEnable)?(BIT15|BIT14|BIT13):(0)), BIT15|BIT14|BIT13);   //[15] Sin Gen Enable, [14]/[13] R/L channel Sin Gen Path Selection, [12] Sin Gen Dither Enable
}
#endif

