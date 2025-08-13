//#include "board.h"
#ifndef _Mode_H_
#define _Mode_H_

#ifdef _MODE_C_
#define _MODEDEC_
#else
#define _MODEDEC_ extern
#endif
#include "board.h"

#define bVSyncNegative          BIT0
#define bHSyncNegative          BIT1
#define bHSyncLoss              BIT2
#define bVSyncLoss              BIT3
#define bInterlaceMode          BIT4
#define bUnsupportMode          BIT5
#define bUserMode               BIT6
#define bNativeMode         BIT7
#define bShrinkMode         BIT7



#define HPeriod_Torlance                       (CURRENT_INPUT_IS_DISPLAYPORT()?80:10) // for mode change
#define VTotal_Torlance                                4 // 10 // for mode change
#define VTotal_Delta                                    50
#define HFreq_Torlance                              12 // 15 // 1.5 KHz
#define VFreq_Torlance                              12 // 15 // 1.5 Hz
#define HFREQ_TOLERANCE_ENLARGE      150 // 15KHz
#define VFREQ_TOLERANCE_ENLARGE      100 // 10Hz


#define _3D_TIMING       0// 1

typedef enum
{
    IP_MUX_ADC  = 0,
    IP_MUX_DVI  = 1,
    IP_MUX_HDMI = 2,
    IP_MUX_DP   = 9,
} IP_MUX;

enum
{
    CLK_ADC_BUFF_DIV2,
    CLK_DVI_BUFF_DIV2,
    DP_STRM_CLK,
    CLK_DVI_BUFF,

};

#if ENABLE_VGA_INPUT
#define First_Input_Port    Input_Analog1
#else
#define First_Input_Port    Input_Digital
#endif

typedef enum
{
#if (INPUT_TYPE&INPUT_1A)
    Input_Analog1,
#endif
#if (INPUT_TYPE>=INPUT_1C)
    Input_Digital,
#endif
#if (INPUT_TYPE>=INPUT_2C)
    Input_Digital2,
#endif
#if (INPUT_TYPE>=INPUT_3C)
    Input_Digital3,
#endif
#if (INPUT_TYPE>=INPUT_4C)
    Input_Digital4,
#endif
    Input_Nums,

#if !(INPUT_TYPE&INPUT_1A)
    Input_Analog1,
#endif
    Input_YPbPr,
    Input_Analog2,
    Input_Nothing,
    Input_VGA           = Input_Analog1,
    Input_DVI           = Input_DVI_C1,
    Input_DVI2          = Input_DVI_C2,
    Input_DVI3          = Input_DVI_C3,
    Input_DVI4          = Input_DVI_C4,
    Input_DualDVI       = Input_DualDVI_C1,
    Input_DualDVI2      = Input_DualDVI_C2,
    Input_HDMI          = Input_HDMI_C1,
    Input_HDMI2         = Input_HDMI_C2,
    Input_HDMI3         = Input_HDMI_C3,
    Input_HDMI4         = Input_HDMI_C4,
    Input_Displayport   = Input_Displayport_C1,
    Input_Displayport2  = Input_Displayport_C2,
    Input_Displayport3  = Input_Displayport_C3,
    Input_Displayport4  = Input_Displayport_C4,
    Input_UsbTypeC3     = Input_UsbTypeC_C3,
    Input_UsbTypeC4     = Input_UsbTypeC_C4
} InputPortType;


typedef enum
{
    Input_Priority_Auto,
#if (INPUT_TYPE&INPUT_1A)
    Input_Priority_Analog1,
#endif
#if (INPUT_TYPE>=INPUT_1C)
    Input_Priority_Digital1,
#endif
#if (INPUT_TYPE>=INPUT_2C)
    Input_Priority_Digital2,
#endif
#if (INPUT_TYPE>=INPUT_3C)
    Input_Priority_Digital3,
#endif
#if (INPUT_TYPE>=INPUT_4C)
    Input_Priority_Digital4,
#endif
    Input_Priority_Nums,
} InputPortPriorityType;


// HpVp->3,  HpVn->2, HnVp->1, HnVn->0

#define SyncHpVp        8  // Sync Polarity
#define SyncHpVn        4
#define SyncHnVp        2
#define SyncHnVn        1


#if 0
typedef enum
{
    Res_640x350,
    Res_640x400,
    Res_720x400,
    Res_640x480,
    Res_800x600,
    Res_832x624,
    Res_884x930, //0909
    Res_1024x768,
    Res_1280x1024,
    Res_1600x1200,
    Res_1152x864,
    Res_1152x870,
    Res_1280x720,
    Res_1280x960,
    Res_720x480,
    Res_800x1209,
    Res_1440x900, // for Wide panel
    Res_1400x1050,
    Res_1680x1050,

    Max_Res
} ResolutionType;
#endif

typedef enum
{
    Res_640x350,
    Res_640x400,
    Res_720x400,
    Res_640x480,
    //Res_720x483,
    Res_700x570,
    Res_720x576,
    Res_800x600,
    Res_832x624,
    Res_884x930, //0909
    Res_1024x768,
    Res_1280x1024,
    Res_1600x1200,
    Res_1152x864,
    Res_1152x870,
    Res_1152x900,
    Res_1280x720,
    Res_1280x960,
    Res_720x480,
    Res_800x1209,
    Res_1792x1344,
    Res_1400x1050,
    Res_1920x1080,
    Res_1920x1200,

    Res_848x480,   //2005-07-26
    Res_1280x768,  //2005-07-26
    Res_1360x768,
    Res_1366x768,
    Res_1440x900,

    Res_1680x1050,
    Res_1280x800,
    Res_1600x900,
    Res_1856x1392,
    Res_1920x1440,
    Res_2048x1536,
    Res_1280x600,
    Res_1440x1050,
    Res_2560x1600,
#if _3D_TIMING
    Res_1920x2160,
    Res_1280x720P_FP,
#endif
    Max_Res
} ResolutionType;
typedef enum
{
MD_Dummy,   //=====IMPORTANT    DON'T DELETE IT
MD_848X480_60_VESA,     //0
MD_848X480_75_CVT,      //1
MD_1280x768R_60_CVT,    //2
MD_1280x768_60_CVT,     //3
MD_1280x768_75_CVT,     //4
MD_1280x768_85_CVT,     //5
MD_1280x960R_60_VESA,   //6
MD_1280x960_60_VESA,    //7
MD_1280x960_70_FORPC,   //8
MD_1280x960_72_FORPC,   //9
MD_1280x960_75_CVT,     //10
MD_1280x960_85_VESA,    //11
MD_1360x768_60,         //12
MD_1440x900R_60_CVT,    //13
MD_1440x900_60_CVT,     //14
MD_1440x900_75_CVT,     //15
MD_1440x900_85_CVT,     //16
MD_1680x1050R_60_CVT,   //17
MD_1680x1050_60_CVT,    //18
MD_1680x1050_57_CVT,    //19
//===============================================
//===== Normal Mode Start ============
//===============================================
MD_640x350_70_IBM,      //20
MD_640x350_85_VESA,     //21
MD_720x400_70,          //22
MD_720x400_85_VESA,     //23
MD_640x400_56_NEC,      //24
MD_640x400_70_IBM,      //25
//===============================================
//===>Conflict with WSVGA(848x480@60Hz HpVp) ====
//===============================================
MD_640x480_60_VESA,     //26
MD_640x480_66_MAC,      //27
MD_640x480_67,          //28
MD_640x480_72_VESA,     //29
MD_640x480_75_VESA,     //30
MD_640x480_85_VESA,     //31
MD_800x600_56_VESA,     //32
MD_800x600_60_VESA,     //33
MD_800x600_72_VESA,     //34
MD_800x600_75_VESA,     //35
MD_800x600_85_VESA,     //36
MD_832x624_75_MAC,      //37
//===============================================
//===>Conflict with WXVGA(1280x768@60Hz HpVp;HnVp) ====
//===============================================
MD_1024x768_60_VESA,    //38
MD_1024x768R_60_VESA,   //39
MD_1024x768_70_VESA,    //40
//===============================================
//===>Conflict with WXVGA(1280x768@75Hz HnVp) ===
//===============================================
MD_1024x768_75_VESA,    //41
MD_1024x768_75_MAC,     //42
//===============================================
//===>Conflict with WXVGA(1280x768@85Hz HnVp) ===
//===============================================
MD_1024x768_85_VESA,    //43
MD_1152x864_60_FORPC,   //44
MD_1152x864_70_FORPC,   //45
MD_1152x864_60_GRAPHICSCARD,//(For ATI Card)    //46
MD_1152x864_75_VESA,    //47
MD_1152x864_85_VESA,    //48
MD_1152x870_75_MAC,     //49
MD_1280x720_60_CVT,     //50
MD_1280x720_70,         //51
MD_1280x720_75,         //52
//========================================================
//===>Conflict with SXVGA+(1400x1050@60Hz HpVn,HnVP ) ====
//========================================================
MD_1280x1024_60_VESA,   //53
MD_1280x1024_75_VESA,   //54
MD_1280x1024_85_VESA,   //55
MD_1600x1200_60_VESA,   //56
MD_1600x1200_65_VESA,   //57
MD_1600x1200_70_VESA,   //58
MD_1600x1200_75_VESA,   //59
MD_1400x1050R_60_CVT,   //60
MD_1400x1050_60_CVT,    //61
MD_1400x1050_75_CVT,    //62
MD_1400x1050_85_CVT,    //63
MD_1792x1344_60_CVT,    //64
MD_1920x1200_60_CVT,    //65
MD_1920x1200R_60_CVT,   //66
#if 1
MD_720x480_50_YCbCr,    //67
#endif
MD_1024x768_72,         //68
MD_1280x1024_72,        //69
MD_1280x1024_70,        //70
MD_1152x900_66,         //71
MD_720x480I_60_YCbCr,   //72  Start of YPbPr
MD_1920x1080I_50_YCbCr, //73
MD_1920x1080I_50_R_YCbCr,   //74
MD_1920x1080I_60_NTSC_YCbCr,//75
MD_700x570_50_YCbCr,        //76(For India TV Box)
MD_1152x870_60_MAC_YCbCr,   //77
MD_720x576_50_YCbCr,        //78
MD_720x576_60_YCbCr,        //79
MD_720x576_75_YCbCr,        //80
MD_720x576I_50_YCbCr,       //81
MD_720x480_60_YCbCr,        //82
MD_720x480I_60_YCbCr_1,     //83
MD_1920x1080_50_YCbCr,      //84
MD_1920x1080_60_YCbCr,      //85
MD_832x624_75_MAC_YCbCr,    //86
MD_1280x800R_60_CVT,        //87
MD_1280x800_60_CVT,         //88
MD_1280x800_60,             //89
MD_1280x800_60_GRAPHICSCARD,//90 NVIDIA
MD_1280x800_75_CVT,         //91
MD_1280x800_76,             //92
MD_1920x1080_60_138M,       //93    // 138.5M
MD_1920x1080_60_148M,       //94    //148.5M

MD_1920x1080_24,            //95
MD_1920x1080_25,            //96
MD_1920x1080_30,            //97
#if 1//PANEL_WIDTH==1600&&PANEL_HEIGHT==900
MD_1600X900_60_VESA,        //98
MD_1600X900_60_CVT,         //99
MD_1600X900R_60_VESA,       //100
#endif
#if 1//PANEL_WIDTH==1366&&PANEL_HEIGHT==768
MD_1366x768_60,             //101
MD_1360x768_60_VESA,        //102
#endif
MD_1920x1080_60,            //103
MD_1600x900_75,             //104
MD_1680x1050_60_GTF,        //105
MD_1280x768_75_GTF,         //106
MD_1920x1080_75,            //107
MD_1856x1392_60,            //108
MD_1280x768_GTF,            //109
MD_1856x1392_75_VESA_DMT,   //110
MD_1920x1080_85,            //111
MD_1920x1200_85_VESA_GTF,   //112
MD_1920x1400_85,            //113
MD_2048x1536_75,            //114
MD_640x480_120,             //115
MD_800x600_85,              //116
MD_1024x768_100,            //117
MD_1024x768_120_VES_GTF,	//118
MD_1280x960_85,             //119
MD_1280x960_120,            //120
MD_1600x1200_100,           //121
MD_1680x1200_100,           //122
MD_1280x960_75,             //123
MD_1920x1440_75,            //124
MD_640x480_100,             //125
MD_800x600_120,             //126
MD_640x480_70_VESA_GTF,     //127
MD_1280x600_60,             //128
MD_1792x1344_75_VESA_GTF,   //129
MD_800x600_100,             //130
MD_1152x864_85,             //131
MD_1152x864_100,            //132
MD_1280x720_85,             //133
MD_1280x720_100,            //134
MD_1280x720_120,            //135
MD_1280x800_85,             //136
MD_1280x800_120,            //137
MD_1280x1024_85,            //138
MD_1280x1024_100,           //139
MD_1280x1024_120,           //140
MD_1360x768_120,            //141
MD_1400x1050_85,            //142
MD_1400x1050_120,           //143
MD_1440x900_85,             //144
MD_1440x900_120,            //145
MD_1600x900_85_VESA_GTF,    //146
MD_1600x900_100,            //147
MD_1600x900_120,            //148
MD_1600x1200_85,            //149
MD_1680x1050_120,           //150
MD_1920x1080_100,           //151
MD_1600x1200_120,           //152
MD_1024x768_85,             //153
MD_800x600R_120,            //154
MD_1024x768R_120,           //155
MD_1280x768R_120,           //156
MD_1280x800R_120,           //157
MD_1280x960R_120,           //158
MD_1280x1024R_120,          //159
MD_1360x768R_120,           //160
MD_1440x1050R_120,          //161
MD_1440x900R_120,           //162
MD_1680x1050R_120,          //163
MD_1366x768R_60,            //164
MD_1600x900R_60_VESA_DMT,   //165
MD_640x480_50,              //166
MD_800x600_50,              //167
MD_1280x1024_50,            //168
MD_1280x768_50,             //169
MD_848x480_50,              //170
MD_1360x768_50,             //171
MD_1360x768_75,             //172
MD_1920x1080R_60,           //173
MD_1280x960_50,             //174
MD_1400x1050_50,            //175
MD_1280x768_120_VESA_GTF,   //176
MD_2560x1600_60,            //177
MD_1280x960_60_VESA_DMT,    //178
MD_1400x1050_60_VESA_GTF,   //179
MD_1600x900_60_VESA_GTF,    //180
MD_1600x1200_85_VESA_DMT,   //181
MD_1680x1050_60_VESA_GFT,   //182
MD_1680x1050_75_VESA_GFT,   //183
MD_1920x1200_75_VESA_GTF,   //184
MD_1920x1440_60_VESA_DMT,   //185
MD_1920x1080_50_CVT,        //186
#if _3D_TIMING
Mod_1920x1080_24FP,
Mod_1280x720p_60_FP,
Mod_1280x720p_50_FP,
#endif
MD_MAX
} StandardModeType;
typedef struct
{
    BYTE fFlags;
    BYTE InputType;
    WORD HPeiod;
    WORD HTotalMSA;
    WORD VTotal;
    BYTE ModeIndex;
    WORD VFreq;
#if ENABLE_CHECK_TMDS_DE
    WORD TMDSWidth;
    WORD TMDSHeight;
    WORD TMDSHStart;
    WORD TMDSVStart;
#endif
    BYTE YUV420Flag;
    BYTE ALLMFlag;
    DWORD IVSPeriod;
} InputTimingType;
typedef struct
{
    WORD DispWidth;
    WORD DispHeight;
} InputResolutionType;
typedef struct
{
    BYTE Flags;
    ResolutionType ResIndex;
    WORD HFreq;
    WORD VFreq;
    WORD HTotal;
    WORD VTotal;
    WORD HStart;
    WORD VStart;
} InputModeType;
_MODEDEC_ WORD GetStandardModeWidth( void );
_MODEDEC_ WORD GetStandardModeHeight( void );
_MODEDEC_ WORD GetStandardModeGroup( void );
_MODEDEC_ WORD GetStandardModeHStart( void );
_MODEDEC_ WORD GetStandardModeVStart( void );
_MODEDEC_ WORD GetStandardModeHTotal( void );
_MODEDEC_ WORD GetStandardModeVTotal( void );
_MODEDEC_ WORD GetStandardModeHFreq( void);
_MODEDEC_ WORD GetStandardModeVFreq( void );
#endif
