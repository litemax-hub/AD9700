////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (MStar Confidential Information!¡L) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MSAPI_OSD_H
#define MSAPI_OSD_H
#include "datatype.h"
#include "MsTypes.h"
#include "board.h"
#include "msAPI_Font.h"
#include "msAPI_OSD_Resource.h"
#include "apiGFX.h"

//#include "adpGE.h"

#define GE_DRIVER_TEST        0
/********************************************************************************/
/*                                 Macro                                        */
/********************************************************************************/
#define BTN_TEXT_GAP            3//2

/// Types for GEFontFmt.flag: Default
#define GEFONT_FLAG_DEFAULT     0x0000
/// Types for GEFontFmt.flag: Italic
#define GEFONT_FLAG_ITALIC        0x0001
/// Types for GEFontFmt.flag: Scale
#define GEFONT_FLAG_SCALE        0x0002
/// Types for GEFontFmt.flag: Compact
#define GEFONT_FLAG_COMPACT     0x0004
/// Types for GEFontFmt.flag: Background
#define GEFONT_FLAG_BACKGROUND  0x0008
/// Types for GEFontFmt.flag: Blink
#define GEFONT_FLAG_BLINK        0x0010
/// Types for GEFontFmt.flag: Variable width
#define GEFONT_FLAG_VARWIDTH    0x0020  // variable width
/// Types for GEFontFmt.flag: Gap
#define GEFONT_FLAG_GAP            0x0040  // gap (specified by font_tab) between fonts
/// Types for GEFontFmt.flag: Bold
#define GEFONT_FLAG_BOLD        0x0080
/// Types for GEFontFmt.flag: Simple
#define GEFONT_FLAG_SIMPLE      0x0100
/// Types for GEFontFmt.flag: Blink foreground
#define GEFONT_FLAG_BLINK_FG    0x0200
/// Types for GEFontFmt.flag: Blink Background
#define GEFONT_FLAG_BLINK_BG    0x0400
/// Types for GEFontFmt.flag: TRANSL Foreground
#define GEFONT_FLAG_TRANSL_FG   0x0800
/// Types for GEFontFmt.flag: TRANSL Background
#define GEFONT_FLAG_TRANSL_BG   0x1000
/// Types for GEFontFmt.flag: Transparent Foreground
#define GEFONT_FLAG_TRANSP_FG   0x2000
/// Types for GEFontFmt.flag: Transparent Background
#define GEFONT_FLAG_TRANSP_BG   0x4000
/// Types for GEFontFmt.flag: Mirror
#define GEFONT_FLAG_MIRROR   0x8000

///////////////////////////////////////////////////////////////
// color format
///////////////////////////////////////////////////////////////
#define COLOR_TRANSPARENT                               0X000000
#define COLOR_WHITE                                     0xffffff
#define COLOR_BLACK                                     0x0f0f0f
#define COLOR_RED                                       0xe00000
#define COLOR_GREEN                                     0x00FF00
#define COLOR_YELLOW                                    0xffdf00
#define COLOR_LTBLUE                                    0x0080ff
#define COLOR_BLUE                                      0x0000ff
#define COLOR_DKGRAY                                    0x808080
#define COLOR_ASHEN                                         0xffC800
#define COLOR_MAGENTA                                    0xff00ff
#define COLOR_CYAN                                        0x00FFFF
#define COLOR_DKRED                                     0xbf5164
#define COLOR_DKGREEN                                   0x00bf20
#define COLOR_DKBLUE                                    0x0060c0//0x576288
#define COLOR_LTGRAY                                    0xdfdfdf
#define COLOR_GRAY                                      0xa0a0a0
#define COLOR_BRBLUE                                    0x00ffff
#define COLOR_PINK                                      0xffAFAF
#define COLOR_PURPLE                                    0x919EBA
#define COLOR_ORANGE                                    0xff9f00
#define COLOR_BROWN                                     0xa55e02//0xa05f00
#define COLOR_MIDBLUE                                   0x82d7ff
#define COLOR_PURPLERED                                 0x9E103A

//FOR S1 TRUNK
#define COLOR_MIDBLUE           0x82d7ff
#define COLOR_OPTION_LTBLUE     0x99ccff
#define COLOR_BOTTON_DKBLUE     0x3366cc
#define COLOR_BOTTONFRAME_BLUE  0x00ccff
#define COLOR_MENU_LTOLIVEGREEN    0xD5F565
//#define COLOR_MENU_BBB        0x0060C0
#define COLOR_MENU_BLUE            0x0080FF
#define COLOR_MENU_LTBLUE        0xC0DFE0
#define COLOR_MENU_DKGREEN        0x00BF20
#define COLOR_MENU_LTGREEN        0x00FF00
#define COLOR_MENU_PINK            0xF847F4
//#define COLOR_MENU_HHH        0xFF9F00
#define COLOR_MENU_DKGRAY        0x808080
#define COLOR_MENU_GRAY            0xA0A0A0
#define COLOR_MENU_LTGRAY        0xDFDFDF
//#define COLOR_MENU_LLL        0xA05F00
#define COLOR_MENU_LTYELLOW        0xFFFF80
#define COLOR_MENU_GRID         COLOR_LTBLUE
#define BACKGROUND_COLOR        COLOR_TRANSPARENT


///////////////////////////////////////////////////////////////
// blcok gradient attribute
///////////////////////////////////////////////////////////////
#define CONSTANT_COLOR              0x01
#define GRADIENT_X_COLOR            0x02
#define GRADIENT_Y_COLOR            0x04
#define GRADIENT_X_CENTER_COLOR     0x16
#define GRADIENT_Y_CENTER_COLOR     0x32
#define GRADIENT_XY_COLOR           0x64

//Bitblt Type Flag
#define GEBitbltType_Normal                  0x01
#define GEBitbltType_Bitmap                  0x02
#define GEBitbltType_Font                      0x04
#define GEBitbltType_Has_ScaleInfo     0x08

/********************************************************************************/
/*                                 Enum                                         */
/********************************************************************************/
typedef enum
{
    CHAR_IDX_1BYTE,
    CHAR_IDX_2BYTE
} EN_CHAR_IDX_LENGTH;

typedef enum
{
    EN_ALIGNMENT_DEFAULT,           ///< Alignment default
    EN_ALIGNMENT_LEFT,              ///< Alignment left
    EN_ALIGNMENT_RIGHT,             ///< Alignment right
    EN_ALIGNMENT_CENTER             ///< Alignment center
} EN_ALIGNMENT_TYPE;

typedef enum
{
    EN_STRING_ENDING_NONE,
    EN_STRING_ENDING_1_DOT,
    EN_STRING_ENDING_2_DOT,
    EN_STRING_ENDING_3_DOT,
} EN_STRING_ENDING_TYPE;

#if (ENABLE_ARABIC_OSD || ENABLE_THAI_OSD)
typedef enum _EN_OSDAPI_LANGUAGE
{
    LANGUAGE_OSD_CZECH,
    LANGUAGE_OSD_DANISH,
    LANGUAGE_OSD_GERMAN,
    LANGUAGE_OSD_ENGLISH,
    LANGUAGE_OSD_SPANISH,
    LANGUAGE_OSD_GREEK,
    LANGUAGE_OSD_FRENCH,
    LANGUAGE_OSD_CROATIAN,
    LANGUAGE_OSD_ITALIAN,
    LANGUAGE_OSD_HUNGARIAN,
    LANGUAGE_OSD_DUTCH,
    LANGUAGE_OSD_NORWEGIAN,
    LANGUAGE_OSD_POLISH,
    LANGUAGE_OSD_PORTUGUESE,
    LANGUAGE_OSD_RUSSIAN,
    LANGUAGE_OSD_ROMANIAN,
    LANGUAGE_OSD_SLOVENIAN,
    LANGUAGE_OSD_SERBIAN,
    LANGUAGE_OSD_FINNISH,
    LANGUAGE_OSD_SWEDISH,
    LANGUAGE_OSD_BULGARIAN,
    LANGUAGE_OSD_SLOVAK,
    LANGUAGE_OSD_CHINESE,
    LANGUAGE_OSD_MENU_MAX = LANGUAGE_OSD_CHINESE,        // OSD Menu Language Maximum
    LANGUAGE_OSD_GAELIC,
    LANGUAGE_OSD_WELSH,
    LANGUAGE_OSD_IRISH,
    LANGUAGE_OSD_KOREAN,
    LANGUAGE_OSD_JAPAN,
    LANGUAGE_OSD_HINDI,
    LANGUAGE_OSD_MAORI,
    LANGUAGE_OSD_MANDARIN,
    LANGUAGE_OSD_CANTONESE,
    LANGUAGE_OSD_AUDIO_SUBTITLE_LANGUAGE_MAX_NUM = LANGUAGE_OSD_CANTONESE,      // Audio Language New Zealannd Maximum
    LANGUAGE_OSD_TURKISH,
    LANGUAGE_OSD_NETHERLANDS,
    LANGUAGE_OSD_ARABIC,
    LANGUAGE_OSD_HEBREW,
    LANGUAGE_OSD_KURDISH,
    LANGUAGE_OSD_PARSI,
    LANGUAGE_OSD_THAI,
    LANGUAGE_OSD_QAA,
    LANGUAGE_OSD_UND, //**-- Italy CI Certificate --**//
    LANGUAGE_OSD_UNKNOWN,       //move to hear to avoid haveing same value as spanish.

    LANGUAGE_OSD_AUDIO1,
    LANGUAGE_OSD_AUDIO2,
    LANGUAGE_OSD_AUDIO3,
    LANGUAGE_OSD_AUDIO4,
    LANGUAGE_OSD_AUDIO5,
    LANGUAGE_OSD_AUDIO6,
    LANGUAGE_OSD_ENGX2,// = MENU_LANGUAGE_FONT_ENGX2,
    LANGUAGE_OSD_DEFAULT = LANGUAGE_OSD_ENGLISH,
    LANGUAGE_OSD_NONE = 0xFF,
} EN_OSDAPI_LANGUAGE;
#endif

/********************************************************************************/
/*                             Structure type                                   */
/********************************************************************************/
/// GE RGB Color
typedef struct
{
#ifdef WORDS_BIGENDIAN
    U8 a;               ///< Alpha
    U8 r;               ///< Red
    U8 g;               ///< Green
    U8 b;               ///< Blue
#else
    U8 b;
    U8 g;
    U8 r;
    U8 a;
#endif
}MSAPI_GERGBColor;

/// GE Font format
typedef struct
{
    U16 flag;                    ///< scale or italic
    U16 width;                  ///< width
    U16 height;                 ///< height
    U8 dis;                     ///< distance
    MSAPI_GERGBColor clr;       ///< GE RGB Color
    U8 blinkfg;                 ///< blink foreground
    U8 blinkbg;                 ///< blink background
    U8 ifont_gap;               ///< inter-font gap, only valid for GEFONT_FLAG_GAP
} MSAPI_GEFontFmt;

/// OSD Button structure
typedef struct
{
    U16 x;                                  ///< x
    U16 y;                                  ///< y
    U16 width;                              ///< width
    U16 height;                             ///< height
    U16 radius;                             ///< radius
    U32 f_clr;                              ///< frame color
    U32 t_clr;                              ///< text_color
    U32 b_clr;                              ///< background color
    U32 g_clr;                              ///< gradient color
    U8 u8Gradient;                          ///< gradient
    EN_CHAR_IDX_LENGTH bStringIndexWidth;   ///< string index width
    EN_ALIGNMENT_TYPE enTextAlign;          ///< text align
    BOOLEAN fHighLight;                     ///< hight light or not
    U8 bFrameWidth;                         ///< frame width
    MSAPI_GEFontFmt Fontfmt;                ///< Font format
} OSDClrBtn;



typedef struct

{

    U16               sb_pit;       ///< source pitch in bytes

    U16               db_pit;       ///< destination pitch in bytes

    U32               sb_base;      ///< source buffer base in bytes

    U32               db_base;      ///< destination buffer base in bytes

}GEPitBaseInfo;

/// GE Color information
#if 1
typedef struct

{

#ifdef WORDS_BIGENDIAN

    U8 a;   ///< alpha

    U8 r;   ///< Red

    U8 g;   ///< Green

    U8 b;   ///< Blue

#else

    U8 b;   ///< Blue

    U8 g;   ///< Green

    U8 r;   ///< Red

    U8 a;   ///< alpha

#endif

} GERGBColor;
#endif

/// GE Color Range
typedef struct
{
    GERGBColor color_s;     ///< start color
    GERGBColor color_e;     ///< end color
} GEColorRange;

/// GE Bitmap format
typedef struct
{
    BOOLEAN bBmpColorKeyEnable; ///< colorkey enable
    GEColorRange clrrange;  ///< color range
    BOOLEAN bScale; ///< scaling enable
    U16 width; ///< bitmap width
    U16 height; ///< bitmap height
} GEBitmapFmt;


/// GE pointer coordinate
typedef struct
{
    U16 x; ///< x coordinate of point
    U16 y; ///< y coordinate of point
} GEPoint;


/// OSD line structure
typedef struct
{
    U16 x1;                     ///< start x
    U16 y1;                     ///< start y
    U16 x2;                     ///< end x
    U16 y2;                     ///< end y
    U32 u32LineColor;        ///< Line color
    U8     u8LineWidth;            ///< line width
} OSDClrLine;

typedef struct
{
    U16 width;
    U16 height;
    U8 rowdata[1];//row data
} BMPHDR;


typedef struct
{
    MSAPI_GERGBColor color_s;
    MSAPI_GERGBColor color_e;
} MSAPI_GEColorRange;

typedef struct
{
    BOOLEAN bBmpColorKeyEnable;
    MSAPI_GEColorRange clrrange;
    BOOLEAN bScale;
      U16 width;
    U16 height;
} MSAPI_GEBitmapFmt;

/// OSD Region
typedef struct
{
    U16 x;          ///< x
    U16 y;          ///< y
    U16 width;      ///< width
    U16 height;     ///< height
    U8  fbID;       ///< frame buffer ID
} MSAPI_OSDRegion;

typedef struct
{
    U16 x0;
    U16 y0;
    U16 x1;
    U16 y1;
} MSAPI_ClipWindow;

typedef struct
{
	U16 	x;
	U16 	y;
	U16		width;
	U16		height;
}MSAPI_OSDRange;


typedef struct
{
	U8 u8GWinID;
	#if 1 // PANEL_8K
	U8 u8GWin2ID;
	#endif
	U8 u8MainFbID;
	U8 u8SubFbID;
	MSAPI_OSDRange	Rect;
} MSAPI_FontGWin;

typedef enum
{
	GWIN_LAYER_BTM = 0,
	GWIN_LAYER_TOP = 1,
	GWIN_LAYER_MAX = 2,
}MSAPI_GWINLAYER;

typedef struct
{
    U16  v0_x;  //destination x
    U16  v0_y; //destination y
    U16  width;    // same when drawing line (destination width)
    U16  height;  // same when drawing line (destination height)
    //U16  v1_x;
    //U16  v1_y;
    U16  v2_x;  //source x
    U16  v2_y;  //source y
    U8    direction; // direction[0] = x direction, direction[1] = y direction
}GECoordinate;

//==============================================================================
// Scaling factor info
//==============================================================================
/// Define the scaling facter for X & Y direction.
typedef struct
{
    U32 u32DeltaX;
    U32 u32DeltaY;
    U32 u32InitDelatX;
    U32 u32InitDelatY;
}GE_SCALE_INFO;


///@internal GE Bitmap information, For future use
typedef struct
{
    U8                 BitBltTypeFlag;      ///< bitblt type
    U8                 BmpFlag;             ///< Option for drawing effect(italic, mirror, rotate...etc)
    U8                 dst_fm;              ///< destination format
    U8                 src_fm;              ///< source format
    BMPHANDLE          bmphandle;           ///< handle for bitmap
    U16                src_width;           ///< source width (pixel)
    U16                src_height;          ///< source height (pixel)
    GECoordinate BitbltCoordinate;
    GE_SCALE_INFO ScaleInfo;
} GEBitBltInfo;

#ifdef MSAPI_OSD_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

// Variable
INTERFACE MSAPI_ClipWindow ClipWindow;
INTERFACE MSAPI_ClipWindow ClipWindowBackup;

#if (ENABLE_OSDW_GOP)
BOOLEAN msAPI_OSD_FontGWinInit(MSAPI_FontGWin* FontGwin,U16 FbFmt,MSAPI_GWINLAYER Layer);
INTERFACE BOOLEAN msAPI_OSD_FontGWinTerminate(MSAPI_GWINLAYER Layer);
INTERFACE void msAPI_OSD_SetFontGWinHWPinpon(MSAPI_GWINLAYER Layer,BOOLEAN enable);
INTERFACE void msAPI_OSD_FontGWinOpen(MSAPI_GWINLAYER Layer);
INTERFACE void msAPI_OSD_SetGwinPosition(MSAPI_GWINLAYER Layer, U16 u16x, U16 u16y);
INTERFACE U32 msAPI_OSD_GetFbAddr(U8 u8FbIndex);
INTERFACE U16 msAPI_OSD_FBSwitch(MSAPI_GWINLAYER Layer);
INTERFACE void msAPI_OSD_PreFBSwitch(MSAPI_GWINLAYER Layer);
INTERFACE void msAPI_OSD_PostFBSwitch(MSAPI_GWINLAYER Layer);
#endif
INTERFACE void msAPI_OSD_Free_resource(void);
INTERFACE void msAPI_OSD_GET_resource(void);

INTERFACE U16 msAPI_OSD_u16Strlen(U16 *pu16Str);
#if 1//(KEEP_UNUSED_FUNC == 1 || defined(ZUI))
INTERFACE void msAPI_OSD_u16Strcpy(U16 *pu16Dest, U16 *pu16Src);
INTERFACE void msAPI_OSD_u16Strcat(U16 *pu16Dest, U16 *pu16Src);
#endif
#if (KEEP_UNUSED_FUNC == 1)
INTERFACE S8 msAPI_OSD_u16Strcmp(U16 *u16strA,U16 *u16strB);
#endif
INTERFACE U16 msAPI_OSD_GetStrLength(U8 *pu8Str, EN_CHAR_IDX_LENGTH u8NoOfBytesPerChar);
INTERFACE U16 msAPI_OSD_GetStrWidth(FONTHANDLE font_handle, U8 *pu8Str, OSDClrBtn *pclrBtn);
INTERFACE U8 msAPI_OSD_GetFontHeight(FONTHANDLE font_handle);
#if 1
INTERFACE void msAPI_OSD_DrawLine(OSDClrLine *p_clr_line);
#else
INTERFACE void msAPI_OSD_DrawLine(OSDClrBtn *pclrBtn);
#endif
INTERFACE void msAPI_OSD_DrawText(FONTHANDLE font_handle, U8 *pu8Str, OSDClrBtn *pclrBtn);
INTERFACE void msAPI_OSD_DrawText_I2(FONTHANDLE font_handle, U8 *pu8Str, OSDClrBtn *pclrBtn);
INTERFACE void msAPI_OSD_DrawPunctuatedString(FONTHANDLE font_handle, U16 *pu16Str, OSDClrBtn *pclrBtn, U8 max_row_num);
INTERFACE void msAPI_OSD_DrawPunctuatedString_S1(FONTHANDLE font_handle, U8 *pu8Str, OSDClrBtn *pclrBtn, U8 max_row_num);
INTERFACE void msAPI_OSD_DrawPunctuatedString_S2(FONTHANDLE font_handle, U8 *pu8Str, OSDClrBtn *pclrBtn, U8 max_row_num);
INTERFACE void msAPI_OSD_DrawClippedString(FONTHANDLE font_handle, U16 *pu16Str, OSDClrBtn *pclrBtn, EN_STRING_ENDING_TYPE enEndingType);

INTERFACE void msAPI_OSD_DrawFrame(OSDClrBtn *clrBtn);
INTERFACE void msAPI_OSD_DrawBlock(OSDClrBtn *pclrBtn);
INTERFACE void msAPI_OSD_DrawMMIBlock(OSDClrBtn *pclrBtn);
INTERFACE void msAPI_OSD_DrawButton(FONTHANDLE font_handle, U8 *pu8Str, OSDClrBtn *pclrBtn);

INTERFACE void msAPI_OSD_DrawRoundFrame (OSDClrBtn *pclrBtn);
INTERFACE void msAPI_OSD_DrawRoundBlock (OSDClrBtn *pclrBtn);
INTERFACE void msAPI_OSD_DrawRoundButton(FONTHANDLE font_handle, U8 *pu8Str, OSDClrBtn *pclrBtn);

INTERFACE void msAPI_OSD_DrawBitmap(BMPHANDLE Handle, U16 u16StartX, U16 u16StartY, GEBitmapFmt bmpfmt);
INTERFACE void msAPI_OSD_DrawBitmap_Subtitle(BMPHANDLE Handle, U16 u16StartX, U16 u16StartY, GEBitmapFmt bmpfmt);
INTERFACE void msAPI_OSD_SetClipWindow(U16 U16x0, U16 U16y0, U16 U16x1, U16 U16y1);
INTERFACE void msAPI_OSD_RestoreBackupClipWindow(void);
INTERFACE void msAPI_OSD_GetClipWindow(U16 *pX0, U16 *pY0, U16 *pX1, U16 *pY1);
INTERFACE void msAPI_OSD_BackupClipWindow(void);
INTERFACE void msAPI_OSD_SetDither(BOOLEAN bEnable);
INTERFACE void msAPI_OSD_SetRotation(U8 locrotate,U8 glorotate);

#if (ENABLE_ARABIC_OSD || ENABLE_THAI_OSD)
INTERFACE void msAPI_OSD_SetOSDLanguage(EN_OSDAPI_LANGUAGE eLanguage);
INTERFACE void msAPI_OSD_EnterDivxSubtitleMode(BOOLEAN bDivxSubtitle);
INTERFACE BOOLEAN msAPI_OSD_GetDivxSubtitleMode(void);
//INTERFACE void msAPI_OSD_DisplayDivxSubtitleText(BOOLEAN bTextMode);
INTERFACE void msAPI_OSD_SetHebrewLangMixedMode(BOOLEAN bHebrewMixed);
#endif

#undef INTERFACE

// Optional special drawing function

#define MSAPI_OSD_AQUA

#ifdef MSAPI_OSD2_C
#define INTERFACE
#else
#define INTERFACE extern
#endif
INTERFACE U8 msAPI_OSD_CopyRegion(MSAPI_OSDRegion *src, MSAPI_OSDRegion *dst);

#ifdef MSAPI_OSD_AQUA
INTERFACE void msAPI_OSD_DrawGradientRoundBlock(OSDClrBtn *pclrBtn);

#endif

INTERFACE void msAPI_OSD_DrawColorKeyBitmap(BMPHANDLE Handle, U16 u16StartX, U16 u16StartY, MSAPI_GEBitmapFmt bmpfmt);

#if 1//def ZUI
INTERFACE void msAPI_OSD_GetPunctuatedStringHeight(FONTHANDLE font_handle, U16 *pu16Str, OSDClrBtn *pclrBtn, U8 max_row_num,
    U8 * pu8rows, U16 * pu16height);
#endif

#define msAPI_GE_BitBlt(_BitbltInfo, _PitBaseInfo) MDrv_GE_BitBlt(_BitbltInfo, _PitBaseInfo)
#define MDrv_GE_SetDC_CSC_FMT(mode, yuv_out_range, uv_in_range, srcfmt, dstfmt) \
        MApi_GFX_SetDC_CSC_FMT(mode, yuv_out_range, uv_in_range, srcfmt, dstfmt)

#define msAPI_GE_SetDC_CSC_FMT(mode, yuv_out_range, uv_in_range, srcfmt, dstfmt) \
        MApi_GFX_SetDC_CSC_FMT(mode, yuv_out_range, uv_in_range, srcfmt, dstfmt)
#define msAPI_GE_SetNearestMode(enable) MApi_GFX_SetNearestMode(enable)
#define msAPI_GE_SetPatchMode(repeat) MApi_GFX_SetPatchMode(repeat)

// Wrapper
INTERFACE BMPHANDLE MDrv_GE_LoadBitmap(MS_U32 addr, MS_U32 u32len, MS_U16 u16width, MS_U16 u16height, MS_U8 dstloc);
INTERFACE void MDrv_GE_DrawBitmap(BMPHANDLE handle, GEPoint *ppoint, GEBitmapFmt *pbmpfmt);
INTERFACE void MDrv_GE_BitBlt(GEBitBltInfo *BitbltInfo, GEPitBaseInfo *PitBaseInfo);

#define msAPI_GE_SetMirror(MirrorX, MirrorY) MApi_GFX_SetMirror(MirrorX, MirrorY)
#define msAPI_GE_SetRotate(Rotate) MApi_GFX_SetRotate(Rotate)

#define MDrv_GE_ClearFrameBuffer(a,b,c)  MApi_GFX_ClearFrameBuffer(a,b,c)
#define MDrv_GE_ClearFrameBufferByWord(a,b,c)  MApi_GFX_ClearFrameBufferByWord(a,b,c)

#undef INTERFACE

// end Optional special drawing function

#endif /* MSAPI_OSD_H */

