///////////////////////////////////////////////////////////////////////////////
/// @file ColorPalette.c
/// @brief
/// @author MStarSemi Inc.
///
/// Functions to load color palette.
///
/// Features
///  -
///////////////////////////////////////////////////////////////////////////////
#define _COLORPALETTE_C

#include "types.h"
#include "board.h"
#include "ms_reg.h"
#include "Global.h"
#include "menudef.h"
#include "ColorPalette.h"
#include "ColorPaletteTbl.h"
//#include "debugMsg.h"
#include "Ms_rwreg.h"
//#include "mstar.h"
#include "drvOSD.h"
#include "msOSD.h"

#include "Common.h"
//for Factory
ColorPaletteType code tOSDColorPalette0[] =
{
#if CHIP_ID>=CHIP_TSUMV
	{0xFF, 0xFF, 0xFF,	  CP_COLORKEY_F},  // Color_Tran
#endif
	{0xFF, 0x00, 0x00,	  CP_COLORKEY_F},  // Palette 0 // Black
	{0xFF, 0xFF, 0xFF,	  CP_COLORKEY_F},  // Palette 1 // White
	{0xFF, 0x00, 0x00,	  CP_COLORKEY_F},  // Palette 2 // Red
	{0x00, 0xFF, 0x00,	  CP_COLORKEY_F},  // Palette 3 // Green
	{0x00, 0x00, 0xFF,	  CP_COLORKEY_F},  // Palette 4 // Blue
	{0xFF, 0xFF, 0x00,	  CP_COLORKEY_F},  // Palette 5 // Yellow
	{0xc0, 0xc0, 0xc0,	  CP_COLORKEY_F},  // Palette 6 // Gray
	{0x38, 0x80, 0x98,	  CP_COLORKEY_F},  // Palette 7 // Background
	{0x00, 0x00, 0x80,	  CP_COLORKEY_F},  // Palette 4 // Deep Blue
};

ColorPaletteType code tOSDColorPalette256[]=
{
  // 0
  {0x00, 0x00, 0x00,    CP_COLORKEY_F},           // 0
  {0xF0, 0xF0, 0xF0,    CP_COLORKEY_F},           // 1
  {0xF0, 0x00, 0x00,    CP_COLORKEY_F},           // 2
  {0x40, 0x40, 0x40,    CP_COLORKEY_F},           // 3
  // 1
  {0xEC, 0x94, 0x30,    CP_COLORKEY_F},           // P // Mediatek Gold
  {0xFF, 0xFF, 0x00,    CP_COLORKEY_F},           // 5
  {0x00, 0xFF, 0x00,	CP_COLORKEY_F}, 		  // 6
  {0x00, 0x00, 0xFF,	CP_COLORKEY_F}, 		  // 7
  // 2
  {NULL, NULL, NULL,    CP_COLORKEY_F},           // 8
  {NULL, NULL, NULL,    CP_COLORKEY_F},           // 9
  {NULL, NULL, NULL,    CP_COLORKEY_F},           // A
  {NULL, NULL, NULL,    CP_COLORKEY_F},           // B

  {NULL, NULL, NULL,    CP_COLORKEY_F},           // C
  {NULL, NULL, NULL,    CP_COLORKEY_F},           // D
  {NULL, NULL, NULL,    CP_COLORKEY_F},           // E
  #if LiteMAX_Baron_OSD_TEST
  {0x38, 0x80, 0x98,    CP_COLORKEY_F},  		  // F
  #else
  {0x00, 0x00, 0x00,    CP_COLORKEY_F},           // F
  #endif
  // 4 0x10
  {0x00, 0x00, 0x00,    CP_COLORKEY_F},           // 0
  {0xEC, 0x94, 0x30,    CP_COLORKEY_F},           // 1
  {0xEC, 0x94, 0x30,    CP_COLORKEY_F},           // 2
  {0xFF, 0xFF, 0xFF,    CP_COLORKEY_F},           // 3
  // 5 0x10 Frame // Cursor Palette Hightlight (Brightness and Contrast First Line)
  {0x38, 0x80, 0x98,    CP_COLORKEY_F}, //{RGB_CURSOR_BLACK,    CP_COLORKEY_F},
  {0xFF, 0xFF, 0xFF,    CP_COLORKEY_F}, //{RGB_CURSOR_WHITE,    CP_COLORKEY_F},
  {0x60, 0x60, 0x60,    CP_COLORKEY_F}, //{RGB_CURSOR_TOP,      CP_COLORKEY_F},
  {0x40, 0x40, 0x40,    CP_COLORKEY_F}, //{RGB_BK_COLOR_H,      CP_COLORKEY_F},
  // 6 0x14 Icon Normal // Cursor Palette Hightlight (AutoAdjust First Line)
  {0x38, 0x80, 0x98,    CP_COLORKEY_F}, //{RGB_CURSOR_BLACK,    CP_COLORKEY_F},
  {0xFF, 0xFF, 0x00,    CP_COLORKEY_F}, //{RGB_CURSOR_WHITE,    CP_COLORKEY_F},
  {0x00, 0xFF, 0xFF,    CP_COLORKEY_F}, //{RGB_CURSOR_TOP,      CP_COLORKEY_F},
  {0x00, 0x00, 0x00,    CP_COLORKEY_F}, //{0x00, 0x00, 0x00,    CP_COLORKEY_F},
  // 7 0x18 Audio Icon Normal // Cursor Palette Hightlight (Common Second Line) && <<gauge 4 color 1>>
  {0x38, 0x80, 0x98,    CP_COLORKEY_F}, //{RGB_CURSOR_BLACK,    CP_COLORKEY_F},
  {0xFF, 0xFF, 0x00,    CP_COLORKEY_F}, //{RGB_CURSOR_WHITE,    CP_COLORKEY_F},
  {0xFF, 0x00, 0x00,    CP_COLORKEY_F}, //{RGB_CURSOR_TOP,      CP_COLORKEY_F},
  {0x00, 0x00, 0x00,    CP_COLORKEY_F}, //{RGB_CURSOR_DOWN,     CP_COLORKEY_F},
  // 8 0x1C Color Icon Normal // Cursor Palette Hightlight (Common Third Line)
  {0x38, 0x80, 0x98,    CP_COLORKEY_F}, //{0x00, 0x00, 0x00,    CP_COLORKEY_F},
  {0xFF, 0x00, 0x00,    CP_COLORKEY_F}, //{RGB_CURSOR_WHITE,    CP_COLORKEY_F},
  {0x00, 0xFF, 0x00,    CP_COLORKEY_F}, //{RGB_CURSOR_DOWN,     CP_COLORKEY_F},
  {0x00, 0x00, 0xFF,    CP_COLORKEY_F}, //{RGB_CURSOR_BLACK,    CP_COLORKEY_F},
  // 9 0x20 Sharpness Icon Normal // Cursor Palette Hightlight (L1 First Line)
  {0x38, 0x80, 0x98,    CP_COLORKEY_F}, //{RGB_CURSOR_BLACK,    CP_COLORKEY_F},
  {0x00, 0x00, 0x00,    CP_COLORKEY_F}, //{RGB_CURSOR_WHITE,    CP_COLORKEY_F},
  {0xFF, 0xFF, 0x00,    CP_COLORKEY_F}, //{RGB_CURSOR_TOP,      CP_COLORKEY_F},
  {0x00, 0x00, 0x00,    CP_COLORKEY_F}, //{0x00, 0x00, 0x00,    CP_COLORKEY_F},
  // 10 0x24 Icon Select // Cursor Palette Hightlight (L1 Common Third Line)
  {0x00, 0x00, 0xC0,    CP_COLORKEY_F}, //{0x00, 0x00, 0x00,    CP_COLORKEY_F},
  {0xFF, 0xFF, 0x00,    CP_COLORKEY_F}, //{RGB_CURSOR_WHITE,    CP_COLORKEY_F},
  {0x00, 0xFF, 0xFF,    CP_COLORKEY_F}, //{RGB_CURSOR_DOWN,     CP_COLORKEY_F},
  {0x00, 0x00, 0x00,    CP_COLORKEY_F}, //{RGB_CURSOR_BLACK,    CP_COLORKEY_F},
  // 11 0x28 Audio Icon Select // Cursor Palette Hightlight (L1 /L2 Common Third Line buttom)
  {0x00, 0x00, 0xC0,    CP_COLORKEY_F}, //{RGB_BK_COLOR_H,      CP_COLORKEY_F},
  {0xFF, 0xFF, 0x00,    CP_COLORKEY_F}, //{RGB_CURSOR_WHITE,    CP_COLORKEY_F},
  {0xFF, 0x00, 0x00,    CP_COLORKEY_F}, //{RGB_CURSOR_DOWN,     CP_COLORKEY_F},
  {0x00, 0x00, 0x00,    CP_COLORKEY_F}, //{RGB_CURSOR_BLACK,    CP_COLORKEY_F},
  // 12 0x2C Color Icon Select // 3rd Line of MM ICON(4color) highlight
  {0x00, 0x00, 0xC0,    CP_COLORKEY_F}, //{0x00, 0x00, 0x00,    CP_COLORKEY_F},
  {0xFF, 0x00, 0x00,    CP_COLORKEY_F}, //{RGB_BLACK,           CP_COLORKEY_F},
  {0x00, 0xFF, 0x00,    CP_COLORKEY_F}, //{RGB_CURSOR_DOWN,     CP_COLORKEY_F},
  {0x00, 0x00, 0xFF,    CP_COLORKEY_F}, //{RGB_CURSOR_BLACK,    CP_COLORKEY_F},
  // 13 0x30 Sharpness Icon Select // for Menu Logo
  {0x00, 0x00, 0xC0,    CP_COLORKEY_F}, //{0x00, 0x00, 0x00,    CP_COLORKEY_F},
  {0x00, 0x00, 0x00,    CP_COLORKEY_F}, //{0xFF, 0xFF, 0xFF,    CP_COLORKEY_F},
  {0xFF, 0xFF, 0x00,    CP_COLORKEY_F}, //{0xFF, 0x00, 0x00,    CP_COLORKEY_F},
  {0x00, 0x00, 0x00,    CP_COLORKEY_F}, //{0xFF, 0xFF, 0xFF,    CP_COLORKEY_F},
  // 14 0x34 Stealth Logo Color
  {0x00, 0x00, 0x00,    CP_COLORKEY_F},           // 0
  {0x40, 0x40, 0x40,    CP_COLORKEY_F},           // 1
  {0xFF, 0xFF, 0xFF,    CP_COLORKEY_F},           // 2
  {0xE4, 0x1E, 0x26,    CP_COLORKEY_F},           // 3
  // 15 0x38 Icon Disable Color
  {0x38, 0x80, 0x98,    CP_COLORKEY_F},           // 0
  {0x40, 0x40, 0x40,    CP_COLORKEY_F},           // 1
  {0x40, 0x40, 0x40,    CP_COLORKEY_F},           // 2
  {0x40, 0x40, 0x40,    CP_COLORKEY_F},           // 3
  // 16 0x3C PowerKeyLock Normal
  {0x38, 0x80, 0x98,    CP_COLORKEY_F}, //{0x00, 0x00, 0x00,    CP_COLORKEY_F},
  {  29,   64,   77,    CP_COLORKEY_F}, //{RGB_WHITE,           CP_COLORKEY_F},
  {  11,   19,   23,    CP_COLORKEY_F}, //{0x00, 0x00, 0x00,    CP_COLORKEY_F},
  {   4,    1,    1,    CP_COLORKEY_F}, //{0x00, 0x00, 0x00,    CP_COLORKEY_F},
  // 17 0x40 PowerKeyLock Select
  {0x00, 0x00, 0xC0,    CP_COLORKEY_F}, //{RGB_CURSOR_TOP,      CP_COLORKEY_F},
  {  29,   64,   77,    CP_COLORKEY_F}, //{RGB_BK_COLOR_H,      CP_COLORKEY_F},
  {  11,   19,   23,    CP_COLORKEY_F}, //{RGB_CURSOR_DOWN,     CP_COLORKEY_F},
  {   4,    1,    1,    CP_COLORKEY_F}, //{0x00, 0x00, 0x00,    CP_COLORKEY_F},
};


void Load256ColorPalette(BYTE u8Group, BYTE u8Addr, const ColorPaletteType *colorPtr, WORD u16Num)
{
    WORD i;

    //write start addr of PSRAM
    if (u8Group==0)
        WRITE_PSRAM0_ADDR();
    else
        WRITE_PSRAM1_ADDR();
    msWriteByte(OSD1_66, u8Addr);

    //write R/G/B
    if (u8Group==0)
        WRITE_PSRAM0_COLOR();
    else
        WRITE_PSRAM1_COLOR();
    for (i=0;i<u16Num;i++)
    {
        msWriteByte(OSD1_66, ((colorPtr+i)->u8Blue));
        msWriteByte(OSD1_66, ((colorPtr+i)->u8Green));
        msWriteByte(OSD1_66, ((colorPtr+i)->u8Red));
    }

    //write start addr of PSRAM
    if (u8Group==0)
        WRITE_PSRAM0_ADDR();
    else
        WRITE_PSRAM1_ADDR();
    msWriteByte(OSD1_66, u8Addr);

    //write colorkey
    if (u8Group==0)
        WRITE_PSRAM0_COLOR_KEY();
    else
        WRITE_PSRAM1_COLOR_KEY();
    while (u16Num--)
    {
        if(colorPtr->u8ColorKey)
            msWriteByteMask(OSD1_66, BIT0,BIT0);
        else
            msWriteByteMask(OSD1_66, 0,BIT0);
        colorPtr++;
    }

}

void Osd_LoadColorPalette( void )
{

    Load256ColorPalette(0, 0, tOSDColorPalette0, sizeof( tOSDColorPalette0 )/sizeof(ColorPaletteType) );
}

void Osd_LoadColorPalette256( void )
{

    Load256ColorPalette(0, 0, tOSDColorPalette256, sizeof( tOSDColorPalette256 )/sizeof(ColorPaletteType) );
}


