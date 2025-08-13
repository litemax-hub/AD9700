///////////////////////////////////////////////////////////////////////////////
/// @file ColorPalette.h
/// @brief
/// @author MStarSemi Inc.
///
/// Functions to load Color Palette
///
/// Features
///  -
///////////////////////////////////////////////////////////////////////////////

#ifndef _COLORPALETTE_H
#define _COLORPALETTE_H

#ifdef _COLORPALETTE_C
#define INTERFACE
#else
#define INTERFACE   extern
#endif

typedef enum
{ CP_LOGO_BLACK,
#ifndef _AW_OSD_STYLE_
  CP_LOGO_BLUE,
#else
  CP_LOGO_GRAY,
#endif
  CP_LOGO_WHITE,
  CP_LOGO_RBLUE,
  CP_LOGO_WBLUE,   //ENABLE_ENERGY_STAR_LOGO, Steven100206

  CP_LOGO_MAX
} CP_LogoColor;

#if 1

#define Color_2  0x01
#define Color_4  0x02
#define Color_8  0x03
#define Color_2T  0x04    //Transparent
#define Color_4T  0x05    //Transparent
#define Color_8T  0x06    //Transparent

typedef enum
{
#if CHIP_ID>=CHIP_TSUMV
    CP_Tran,
#endif
    CP_WhiteColor,
    CP_RedColor,
    CP_LightBlue,
    CP_BlueColor,
    CP_WhiteColor1,
    CP_GrayColor,
    CP_LightColor,
    CP_BlackColor,
    CP_MaxColor
} CP_ColorType;


#else
//For S0
typedef enum
{
#if CHIP_ID>=CHIP_TSUMV
  CP_TRANSPARENT_COLOR,
#else
  CP_BLACK,         // 0
#endif
  CP_GRAY6,         // 1    //mask
  CP_WHITE,         // 2
  CP_CURSOR_TOP,    // 3

  CP_CURSOR_DOWN,   // 4
  CP_BK_COLOR_XXX0, // 5
  CP_BK_COLOR_XXX1, // 6
  CP_BK_COLOR_XXX2, // 7

  CP_GREEN_TICK,    // 8
  CP_GRAY3,         // 9
  CP_GRAY4,         // A
  CP_YELLOW,        // B 255, 255, 000

  CP_BLUE,          // C 000, 000, 255
  CP_GREEN,         // D 000, 255, 000
  CP_RED,           // E 255, 000, 000
#if CHIP_ID>=CHIP_TSUMV
  CP_BLACK,         // F
#else
  CP_TRANSPARENT_COLOR,
#endif

  CP_MAX_COLOR,

  CP_MASK_COLOR     =0x01,     // always be 1
  CP_BK_COLOR       =CP_BLUE,
  CP_BK_COLOR_H     =CP_RED,  //Steven110519
  CP_CLEAR          =CP_TRANSPARENT_COLOR,
//Menu L0/L1/L2 Color
  CP_BK_COLOR_L0    =CP_BLACK,
  CP_BK_COLOR_L1    =CP_BLACK,
  CP_BK_COLOR_L2    =CP_BLACK,

//Hightlight Color
  CP_HL_COLOR_L0    =CP_BK_COLOR_L1,
  CP_HL_COLOR_L1    =CP_BK_COLOR_L2,
//Saperation Line Color
  CP_SL_COLOR_L1    =CP_WHITE,
  CP_SL_COLOR_L2    =CP_BK_COLOR_L0,
  CP_FG_COLOR_L2    =CP_WHITE,      //L2 Text Color while highlight L1
#if 1  //Steven110523
  CP_ADJ_COLOR_L2   =CP_RED,      //L2 Text Color while highlight L2
#else
  CP_ADJ_COLOR_L2   =CP_BLACK,      //L2 Text Color while highlight L2
#endif
//Button OSD Saperation Line Color
  CP_SL_COLOR_BO    =CP_GRAY6,
  CP_MODEL_COLOR    =CP_WHITE,
  CP_GRAY_OUT_L0    =CP_GRAY6,
  CP_GRAY_OUT_L1    =CP_BK_COLOR_L0,
  CP_GRAY_OUT_L2    =CP_GRAY6,
  CP_HOT_MENU_CURSOR=CP_BK_COLOR_L2,

  //CP_BK_BC_ORG_L1L2 =CP_GRAY6,   //FY12_BC  //Steven110519
  CP_MSG_BK         =CP_BK_COLOR_L1, //FY12_MSG
#if 1  //Steven110523
  CP_MSG_BK_H       =CP_RED
#else
  CP_MSG_BK_H       =CP_GRAY3
#endif
} CP_ColorType;
#endif

//For S1
#if 0 //Disable, S0 S1 use the same mono color palette
typedef enum
{
  CP1_BLACK,
  CP1_WHITE,
  CP1_GRAY6,
  CP1_CURSOR_TOP,

  CP1_CURSOR_DOWN,
  CP1_BK_COLOR_L0,
  CP1_BK_COLOR_L1,
  CP1_BK_COLOR_L2,

  CP1_GREEN_TICK,
  CP1_GRAY3,
  CP1_GRAY4,
  CP1_YELLOW,	// 255, 255, 000

  CP1_BLUE,      // A 000, 000, 255
  CP1_GREEN,	    // B 000, 255, 000
  CP1_RED,		// C 255, 000, 000
  CP1_TRANSPARENT_COLOR,

  CP1_MAX,
  // 4 color
  // 8 color
  CP1_8_GAUGE           =20,
  CP1_4_HM_SAPERATION   =38
} CP1_ColorType;
#endif
#define CP1_4_HM_SAPERATION 45
//Color Select
//-----------------------
#define CS_NORMAL           0
#define CS_HIGHLIGHT        1
#define CS_SELECTED         2
//-----------------------
#define MONO_COLOR(FColor,BkColor)  ((FColor)<<4|(BkColor))
#define FOUR_COLOR(ColorGroup)      ((ColorGroup)<<2)
#define EIGHT_COLOR(ColorGroup)     ((ColorGroup)<<3)

//INTERFACE void LoadLogoCP(void);
//INTERFACE void LoadNormalCP(void);

INTERFACE void Osd_LoadColorPalette( void );
INTERFACE void Osd_LoadColorPalette256( void );

#if 1  //Steven110523
#define CURSOR_TOP_R            0x00
#define CURSOR_TOP_G            0x00
#define CURSOR_TOP_B            0x00
#define CURSOR_COLOR_DELTA_R    0x00
#define CURSOR_COLOR_DELTA_G    0x00
#define CURSOR_COLOR_DELTA_B    0x00

#define CURSOR_DOWN_R           (CURSOR_TOP_R-CURSOR_COLOR_DELTA_R)
#define CURSOR_DOWN_G           (CURSOR_TOP_G-CURSOR_COLOR_DELTA_G)
#define CURSOR_DOWN_B           (CURSOR_TOP_B-CURSOR_COLOR_DELTA_B)
#else
#if 1 //FY12_CURSOR
#define CURSOR_TOP_R            0x00//0x07
#define CURSOR_TOP_G            0xA0//0xA7
#define CURSOR_TOP_B            0xC0//0xC4
#define CURSOR_COLOR_DELTA_R    0x00
#define CURSOR_COLOR_DELTA_G    0x20
#define CURSOR_COLOR_DELTA_B    0x30
#else
#define CURSOR_TOP_R            0xE0
#define CURSOR_TOP_G            0xC0
#define CURSOR_TOP_B            0x50
#define CURSOR_COLOR_DELTA_R    0x00
#define CURSOR_COLOR_DELTA_G    0x20
#define CURSOR_COLOR_DELTA_B    0x30
#endif
#define CURSOR_DOWN_R           (CURSOR_TOP_R-CURSOR_COLOR_DELTA_R)
#define CURSOR_DOWN_G           (CURSOR_TOP_G-CURSOR_COLOR_DELTA_G)
#define CURSOR_DOWN_B           (CURSOR_TOP_B-CURSOR_COLOR_DELTA_B)
#endif

typedef struct
{
    BYTE u8Red;
    BYTE u8Green;
    BYTE u8Blue;
  #if CHIP_ID>=CHIP_TSUMV
    BYTE u8ColorKey;
  #endif
} ColorPaletteType;

#undef INTERFACE
#endif ///


