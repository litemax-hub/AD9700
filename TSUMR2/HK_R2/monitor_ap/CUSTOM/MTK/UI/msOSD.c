#include <stdlib.h>
#include "types.h"
#include "ms_reg.h"
//#include "panel.h"
#include "board.h"
#include "Debug.h"
#include "Common.h"
#include "Ms_rwreg.h"
#include "mStar.h"
#include "misc.h"
#include "Global.h"
#include "menudef.h"
#include "font.h"
//#include "extlib.h"
#include "LoadCommonFont.h"
//#include "msid_v1.h"
#include "drvOSD.h"
#include "msOSD.h"
#include "ColorPalette.h"
#include "LoadPropFont1218.h"


BYTE xdata g_u8OsdWndNo=OSD_MAIN_WND;
BYTE xdata g_u8FontAddrHiBits=0;
BYTE xdata g_u8OsdFontDataHighByte=0;

#define DoubleXSize 0
#define DoubleYSize 0
//==================================================================================
// Local function declaration
//
//void mStar_WriteOSDByte( BYTE address, BYTE value );
BYTE mStar_ReadOSDByte( BYTE address );
void Osd_SetWindowSize( BYTE width, BYTE height );
void Osd_SetPosition( BYTE xPos, BYTE yPos );
//void Osd_DrawChar(BYTE xPos, BYTE yPos, BYTE ch);
void msOSDuncall(void);

extern void mStar_WaitForDataBlanking(void);
//==================================================================================
// variable declaration
//
xdata BYTE OsdWindowWidth = 10;
xdata BYTE OsdWindowHeight = 10;
xdata BYTE OsdFontColor = 0x01;
extern xdata BYTE MenuPageIndex;
extern xdata BYTE MenuItemIndex;
//PropFontType * PropFont_TablePointer;
//WORD * PropFont_MapTablePointer;


//==================================================================================


void Osd_InitOSD( void )
{
//    BYTE u8Retry=10;

    msWriteByteMask(REG_120F86, 0x08, 0x0F);  //OSD output clk

/*
    if(!u8Retry)
    {
        drvOSD_uncall();
        msOSDuncall();
    }
*/
    msWriteByteMask(OSD1_00, 0x00, 0x03); //Set to OSD1 BK0

    drvOSD_ClrOSD_SRAM();  //erase sram

    msWriteByteMask(OSD1_03, BIT2|BIT0, BIT2|BIT0); //[2]:OSD wr priority is over display for both cafsram/psram
    msWriteByteMask(OSD1_10, 0x04, 0x1F); //OSD VS extend:4 liens, for ML to trig during OSD_blanking.


    msWrite2Byte(OSD1_0A, 0x0FFF); // 4 color start
    msWrite2Byte(OSD1_0C, 0x0FFF); // 8 color start
    msWrite2Byte(OSD1_0E, 0x0FFF); // 16 color start

    msWrite2Byte(OSD1_04, g_sPnlInfo.sPnlTiming.u16Width);         //set osd h range size
    msWrite2Byte(OSD1_06, g_sPnlInfo.sPnlTiming.u16Height);        //set osd v range size
    //msWriteByteMask(OSD1_02,BIT1,BIT1);         //force blending
    //Init cafsram
    msWrite2Byte(OSD1_08, OSD_FONT_START_ENTRY);               //set font data entry base,
    msWriteByteMask(OSD1_0B, (OSD_FONT_UNIT_SIZE-1)<<4, BIT4|BIT5);  //Set unit size to 3 entries, [5:4] value+1 entries

    drvOSD_SetWndCABaseAddr(OSD_WIN0,0x0000);
    drvOSD_SetWndShiftPixel(OSD_WIN0, 0, 0);    //avoid compiler warning
    drvOSD_SetWndFixColor(OSD_WIN0, 0, 0, 0);   //avoid compiler warning

    //init OSD_MAIN_WND
    drvOSD_SetWndCtrl(OSD_MAIN_WND, OSD_WND_CTRL0,
                      (OWC0_FP_MODE|
                       OWC0_GROUP_SEL0|
                       OWC0_LV1_ALPHA_SRC(OSD_A_FIX)|
                       OWC0_LV2_ALPHA_SRC(OSD_A_FIX)|
                       OWC0_OUT_ALPHA_SRC(OSD_A_FIX)),
                       OWC0MASK_ALL);
    drvOSD_SetWndCtrl(OSD_MAIN_WND, OSD_WND_CTRL1,
                     (OWC1_1BP_44MODE_EN|
                      OWC1_44MODE_TRS_EN|
                      OWC1_FONT_HEIGHT(18)),
                      OWC1MASK_ALL);
    drvOSD_SetWndCtrl(OSD_MAIN_WND, OSD_WND_CTRL2,
                     (OWC2_GD_COLOR_R(OSD_COLOR_FIX)|
                      OWC2_GD_COLOR_G(OSD_COLOR_FIX)|
                      OWC2_GD_COLOR_B(OSD_COLOR_FIX)),
                      OWC2MASK_ALL);

    drvOSD_SetWndFixAlpha(OSD_MAIN_WND,OSD_ALPHA_LV2,0x00); //PG   //(1-a)*low_lvl+a*top_lvl
    drvOSD_SetWndFixAlpha(OSD_MAIN_WND,OSD_ALPHA_LV1,0x3F); //FG
    drvOSD_SetWndFixAlpha(OSD_MAIN_WND,OSD_ALPHA_OUT,0x3F); //OSD
    drvOSD_SetWndCABaseAddr(OSD_MAIN_WND,OSD_MAIN_WND_CA_BASE);

    //init OSD_BUTTON_WND
    drvOSD_SetWndCtrl(OSD_BUTTON_WND, OSD_WND_CTRL0,
                     (OWC0_FP_MODE|
                      OWC0_GROUP_SEL0|
                      OWC0_LV1_ALPHA_SRC(OSD_A_FIX)|
                      OWC0_LV2_ALPHA_SRC(OSD_A_FIX)|
                      OWC0_OUT_ALPHA_SRC(OSD_A_FIX)),
                      OWC0MASK_ALL);
    drvOSD_SetWndCtrl(OSD_BUTTON_WND, OSD_WND_CTRL1,
                     (OWC1_1BP_44MODE_EN|
                      OWC1_44MODE_TRS_EN|
                      OWC1_FONT_HEIGHT(18)),
                      OWC1MASK_ALL);
    drvOSD_SetWndCtrl(OSD_BUTTON_WND, OSD_WND_CTRL2,
                     (OWC2_GD_COLOR_R(OSD_COLOR_FIX)|
                      OWC2_GD_COLOR_G(OSD_COLOR_FIX)|
                      OWC2_GD_COLOR_B(OSD_COLOR_FIX)),
                      OWC2MASK_ALL);

    drvOSD_SetWndFixAlpha(OSD_BUTTON_WND,OSD_ALPHA_LV2,0x00); //PG   //(1-a)*low_lvl+a*top_lvl
    drvOSD_SetWndFixAlpha(OSD_BUTTON_WND,OSD_ALPHA_LV1,0x3F); //FG
    drvOSD_SetWndFixAlpha(OSD_BUTTON_WND,OSD_ALPHA_OUT,0x3F); //OSD
    drvOSD_SetWndCABaseAddr(OSD_BUTTON_WND,OSD_BUTTON_WND_CA_BASE);

    //init OSD_CURSOR_WND
    drvOSD_SetWndCtrl(OSD_CURSOR_WND, OSD_WND_CTRL0,
                     (OWC0_FP_MODE|
                      OWC0_GROUP_SEL0|
                      OWC0_LV1_ALPHA_SRC(OSD_A_FIX)|
                      OWC0_LV2_ALPHA_SRC(OSD_A_FIX)|
                      OWC0_OUT_ALPHA_SRC(OSD_A_FIX)),
                      OWC0MASK_ALL);
    drvOSD_SetWndCtrl(OSD_CURSOR_WND, OSD_WND_CTRL1,
                     (OWC1_1BP_44MODE_EN|
                      OWC1_44MODE_TRS_EN|
                      OWC1_FONT_HEIGHT(18)),
                      OWC1MASK_ALL);
    drvOSD_SetWndCtrl(OSD_CURSOR_WND, OSD_WND_CTRL2,
                     (OWC2_GD_COLOR_R(OSD_GD_EG3)|
                      OWC2_GD_COLOR_G(OSD_GD_EG4)|
                      OWC2_GD_COLOR_B(OSD_GD_EG5)),
                      OWC2MASK_ALL);

    drvOSD_SetWndFixAlpha(OSD_CURSOR_WND,OSD_ALPHA_LV2,0x00); //PG   //(1-a)*low_lvl+a*top_lvl
    drvOSD_SetWndFixAlpha(OSD_CURSOR_WND,OSD_ALPHA_LV1,0x00); //PG
    drvOSD_SetWndFixAlpha(OSD_CURSOR_WND,OSD_ALPHA_OUT,0x3F); //OSD

    OSD_SET_WND(OSD_MAIN_WND);
    Osd_SetPosition(50, 50); // set osd position
    Osd_SetTransparency(UserPrefOsdTransparency);
    LoadCommonFont();

}


void MonoColorSetting( void )
{
    Osd_LoadColorPalette(); // load osd color
}

// setup osd window size
void Osd_SetWindowSize( BYTE width, BYTE height )
{
    OsdWindowWidth = width;
    OsdWindowHeight = height;

  drvOSD_SetWndLineOffset(OSD_MAIN_WND,OsdWindowWidth); //for OSD_MAIN_WND only
  #if ENABLE_OSD_ROTATION //reset CABase addr after line offset was set
    if (UserPrefOsdRotateMode==OSDRotateMenuItems_90)
    {
        drvOSD_SetWndCABaseAddr(OSD_MAIN_WND, GET_CABASE_0_90(OSD_MAIN_WND_CA_BASE, OsdWindowWidth));
    }
    else
    #if defined(_OSD_ROTATION_180_)
    if (UserPrefOsdRotateMode==OSDRotateMenuItems_180)
    {
        drvOSD_SetWndCABaseAddr(OSD_MAIN_WND, GET_CABASE_0_180(OSD_MAIN_WND_CA_BASE,OsdWindowWidth,OsdWindowHeight));
    }
    else
    #endif
    #if defined(_OSD_ROTATION_270_)
    if (UserPrefOsdRotateMode==OSDRotateMenuItems_270)
    {
        drvOSD_SetWndCABaseAddr(OSD_MAIN_WND, GET_CABASE_0_270(OSD_MAIN_WND_CA_BASE,OsdWindowWidth,OsdWindowHeight));
    }
    else
    #endif
        drvOSD_SetWndCABaseAddr(OSD_MAIN_WND, OSD_MAIN_WND_CA_BASE);
  #endif

}

// setup
#define MainMenuOSD_Height  20
#define OSD_TRANSPARENT_SHIFT     7
#define OSD_WINDOW(Addr)    ((OSD_MAIN_WND<<5)+Addr)
void Osd_SetPosition( BYTE xPos, BYTE yPos )
{
    BYTE u8xPos, u8yPos;
    WORD posValue;
    WORD u16Hight;
    SINT xdata  xOffset = 0, yOffset = 0;
    WORD preMainHStart, preMainVStart;
#if ENABLE_OSD_ROTATION
    BYTE bInvertPosition = FALSE;
#endif

    MENU_LOAD_START();

    preMainHStart = drvOSD_GetWndInfo ( OSD_MAIN_WND, OSD_WND_INFO_H_START );
    preMainVStart = drvOSD_GetWndInfo ( OSD_MAIN_WND, OSD_WND_INFO_V_START );

#if ENABLE_OSD_ROTATION
    if ( UserPrefOsdRotateMode==OSDRotateMenuItems_90 )
    {
        bInvertPosition = TRUE;
        u8xPos = 100 - yPos;
        u8yPos = 100 - xPos;
    }
    else
    #ifdef _OSD_ROTATION_180_
    if ( UserPrefOsdRotateMode==OSDRotateMenuItems_180 )
    {
        bInvertPosition = FALSE;
        u8xPos = 100 - xPos;
        u8yPos = yPos;
    }
    else
    #endif
    #ifdef _OSD_ROTATION_270_
    if ( UserPrefOsdRotateMode==OSDRotateMenuItems_270 )
    {
        bInvertPosition = TRUE;
        u8xPos = yPos;
        u8yPos = xPos;
    }
    else
    #endif
#endif
    {
#if ENABLE_OSD_ROTATION
        bInvertPosition = FALSE;
#endif
        u8xPos = xPos;
        u8yPos = 100 - yPos;
    }
#if (ENABLE_OSD_ROTATION)
    if ( bInvertPosition )
    {
        posValue = ( ( DWORD ) u8xPos * ( g_sPnlInfo.sPnlTiming.u16Width - ( WORD ) ((OsdWindowHeight) * 18) ) ) / 100;
    }
    else
#endif
    {
        posValue = ( ( DWORD ) u8xPos * ( g_sPnlInfo.sPnlTiming.u16Width - ( WORD ) ((OsdWindowWidth) * 12) ) ) / 100;
    }
    xOffset = posValue - preMainHStart;
#if !ENABLE_MENULOAD
    mStar_WaitForDataBlanking();
#endif
    DB_W2B ( ( OSD_WIN1 << 5 ) + OSD2_04, posValue );
#if ENABLE_OSD_ROTATION
    if ( bInvertPosition )
    {
        DB_W2B ( ( OSD_WIN1 << 5 ) + OSD2_08, posValue + ( WORD ) ((OsdWindowHeight) * 18) );
    }
    else
#endif
    {
        DB_W2B ( ( OSD_WIN1 << 5 ) + OSD2_08, posValue + ( WORD ) ((OsdWindowWidth) * 12) );
    }

#if (ENABLE_OSD_ROTATION)
    u16Hight = g_sPnlInfo.sPnlTiming.u16Height ;
    if ( bInvertPosition )
    {
        posValue = ( ( DWORD ) u8yPos * ( u16Hight - ( WORD ) (((OsdWindowWidth)) * 12) ) / 100 );
    }
    else
#endif
    {
        posValue = ( ( DWORD ) u8yPos * ( u16Hight - ( WORD ) (((OsdWindowHeight)) * 18) ) ) / 100;
    }
    yOffset = posValue - preMainVStart;
    DB_W2B ( ( OSD_WIN1 << 5 ) + OSD2_06, posValue );
#if ENABLE_OSD_ROTATION
    if ( bInvertPosition )
    {
        DB_W2B ( ( OSD_WIN1 << 5 ) + OSD2_0A, posValue + ( WORD ) ((OsdWindowWidth) * 12) );
    }
    else
#endif
    {
        DB_W2B ( ( OSD_WIN1 << 5 ) + OSD2_0A, posValue + ( WORD ) ((OsdWindowHeight )* 18) );
    }

    MENU_LOAD_END();
}

void Osd_SetTextMonoColor( BYTE foreColor, BYTE backColor )
{
    OsdFontColor = ( foreColor & 0xF ) << 4 | ( backColor & 0xF );
}
void Osd_SetTextColor( BYTE foreColor, BYTE backColor )
{
    // OsdFontColor=(foreColor&0xF)<<4|(backColor&0xF);
    if( backColor == Color_2 )
        OsdFontColor = foreColor << 1;
    else if( backColor == Color_4 )
        OsdFontColor = foreColor << 2;
    else if( backColor == Color_8 )
        OsdFontColor = foreColor << 3;
    else if( backColor == Color_2T )
        OsdFontColor = foreColor << 1 | 0x01;
    else if( backColor == Color_4T )
        OsdFontColor = foreColor << 2 | 0x01;
    else// (backColor==Color_8T)
        OsdFontColor = foreColor << 3 | 0x01;
}

//Character Direct Write DisplayCode
void Osd_DrawCharDirect(BYTE u8XPos, BYTE u8YPos, WORD u8Char)
{
    if(( u8Char & 0x0F00 ) != 0x0000 )
    {
        OSD_TEXT_HI_ADDR_SET_BITS(( u8Char & 0x0F00 )>>8);
    }
    else
    {
        OSD_TEXT_HI_ADDR_CLR_TO_0();
    }

    drvOSD_DrawRealChar(g_u8OsdWndNo, u8XPos, u8YPos, u8Char);

    OSD_TEXT_HI_ADDR_CLR_TO_0();

}
void Osd_DrawContinuesChar(BYTE u8XPos, BYTE u8YPos, WORD u8Char, BYTE u8Num)
{
    if(( u8Char & 0x0F00 ) != 0x0000 )
    {
        OSD_TEXT_HI_ADDR_SET_BITS(( u8Char & 0x0F00 )>>8);
    }
    else
    {
        OSD_TEXT_HI_ADDR_CLR_TO_0();
    }

    drvOSD_DrawContinuousChar(g_u8OsdWndNo,u8XPos,u8YPos,u8Char,u8Num);

    OSD_TEXT_HI_ADDR_CLR_TO_0();

}

void Osd_DrawPropStr(BYTE u8XPos, BYTE u8YPos, BYTE *pu8Str)
{
    WORD  xdata  u16TempValue;
    Bool  xdata  IsDrawCode = TRUE;

    if (pu8Str==NULL || (*(pu8Str + 1))==0) //Jison
        return;
    if (u8YPos & BIT7)
        IsDrawCode = FALSE;

    if( u8YPos == 2 || u8XPos == 0 )
    {
        u8XPos=( OsdWindowWidth - (*(pu8Str + 1)) ) / 2 + 1;
    }

    u8YPos &= 0x7F; //~(BIT7|BIT6);
#define DISP_CHAR   u8XPos
#define DISP_PTR    u8YPos

    u16TempValue=GET_TXT_CASTART(g_u8OsdWndNo,u8XPos,u8YPos); // get real address
    DISP_PTR = 0;
    WRITE_CAFSRAM_ADDR();
    msWrite2Byte(PORT_ATTR_ADDR, u16TempValue);
    WRITE_ATTRIBUTE();
    while (DISP_PTR++ < *(pu8Str + 1))
        msWriteByte(PORT_ATTR_DATA, OsdFontColor);

    if (IsDrawCode == TRUE)
    {
        DISP_PTR = 0;
        WRITE_CAFSRAM_ADDR();
        msWrite2Byte(PORT_CODE_ADDR, u16TempValue);
        WRITE_CODE();
        while (DISP_PTR < *(pu8Str + 1))
        {
            msWriteByte(PORT_CODE_DATA, (*pu8Str) + DISP_PTR);
            DISP_PTR++;
        }
    }

#undef DISP_CHAR
#undef DISP_PTR

}
void DrawNum( BYTE xPos, BYTE yPos, char len, int value)
{
    char _minus = 0;
    BYTE ucshift = 1;

    if (value<10)
        ucshift=3;
    else if (value<100)
        ucshift=2;

    if( value < 0 )
    {
        value = 0 - value;
        _minus = 1;
    }

    if (Layer1_MenuPage||Layer2_MenuPage)
        ;
    else
    {
        Osd_DrawCharDirect( xPos + 2, yPos, ( BYTE )0x01 );
        Osd_DrawCharDirect( xPos + 1, yPos, ( BYTE )0x01 );
        Osd_DrawCharDirect( xPos + 0, yPos, ( BYTE )0x01 );
    }

    if( value == 0 )
    {
        Osd_DrawCharDirect( xPos + ( len-- )-ucshift, yPos, ( BYTE )( value + NumberFontStart ) );
    }
    else
    {
        while( value && len )            // translate integer to string
        {
            Osd_DrawCharDirect( xPos + ( len-- )-ucshift, yPos, ( BYTE )(( value % 10 ) + NumberFontStart ) );
            value /= 10;
        }
    }

    if( _minus && len >= 0 )            // add sign
    {
        Osd_DrawCharDirect( xPos + ( len-- )-ucshift, yPos, ( BYTE )'-' );
    }
    _minus = ( _minus ) ? ( 0 ) : ( 1 );
    for( ; _minus <= len; _minus++ )
    {
        Osd_DrawCharDirect( xPos + _minus-ucshift, yPos, ( BYTE )0x01 );
    }
}
void Osd_DrawNum( BYTE xPos, BYTE yPos, int value )
{
    DrawNum( xPos, yPos, 3, value );
}

void Osd_DrawFactoryNum( BYTE xPos, BYTE yPos, int value )
{
    DrawNum( xPos, yPos, 4, value );
}
void Osd_Draw4Num( BYTE xPos, BYTE yPos, int value )
{
    DrawNum( xPos, yPos, 4, value );
}
//===================================================================================
#if !Multi_Gauge
#define GuageFontStart  ColorGuageFontStar  //0x56
#define GuageFont0_4    GuageFontStart
#define GuageFont1_4    GuageFontStart+2
#define GuageFont2_4    GuageFontStart+4
#define GuageFont3_4    GuageFontStart+6
#define GuageFont4_4    GuageFontStart+8
void Osd_DrawGuage( BYTE ucX, BYTE ucY, BYTE ucLength, BYTE ucValue )
{

    BYTE pLead;
    BYTE ucLoop;

    Osd_DrawCharDirect( ucX , ucY, ColorGuageEdgeStar ); // Draw guage edge 100729
    Osd_DrawCharDirect( ucX + 8 , ucY, ColorGuageEdgeStar+2 ); // Draw guage edge 100729

    OSD_TEXT_HI_ADDR_SET_BIT8(); //enable bit 8

    ucLength -= 2;

    // bar length should small than 42 ( ucLength < 43 )
    pLead = ( (WORD) ucValue * ucLength * 4 ) / 100;   // resver 0.1 // fill bar ucLength 10 times
    if( ucValue != 0 && pLead == 0 )
        pLead++;

    for(ucLoop=1; ucLoop<=(pLead/4); ucLoop++)
        Osd_DrawCharDirect(ucX+ucLoop, ucY, GuageFont4_4);

    if(pLead < ucLength*4)
        Osd_DrawCharDirect(ucX+ucLoop, ucY, GuageFont0_4+( pLead%4 )*2);

    for(ucLoop=(pLead/4+2); ucLoop<=ucLength; ucLoop++)
        Osd_DrawCharDirect(ucX + ucLoop, ucY, GuageFont0_4);


    OSD_TEXT_HI_ADDR_CLR_TO_0();


}
#endif
void Osd_Show( void )
{
    mStar_WaitForDataBlanking();
    drvOSD_SetWndOn(OSD_MAIN_WND, TRUE);
}
void Osd_Hide( void )
{
    drvOSD_SetWndOn(OSD_MAIN_WND, FALSE);
}
void Osd_SetTransparency(BYTE u8Value)
{
    u8Value=0x3F-(u8Value*8);
    drvOSD_SetWndFixAlpha(OSD_MAIN_WND, OSD_ALPHA_OUT, u8Value);
    drvOSD_SetWndFixAlpha(OSD_BUTTON_WND, OSD_ALPHA_OUT, u8Value);
    drvOSD_SetWndFixAlpha(OSD_CURSOR_WND, OSD_ALPHA_OUT, u8Value);
}
// ============================
// Draw Hex for Debug
BYTE Hex2ASCII( BYTE Num )
{
    return ( Num + 3 ); //(Num<10)?(Num+3):(Num+3);   //3// 3 -->0   10+3-->A
}
void DrawHex( BYTE xPos, BYTE yPos, WORD value )
{
    char len = 3;
    //  Osd_DrawChar(xPos+4, yPos, (BYTE)'H');
    while( value && len >= 0 )
    {
        //Osd_DrawChar(xPos+(len--), yPos, (BYTE)Hex2ASCII(value%0x10));
        Osd_DrawCharDirect( xPos + ( len-- ), yPos, ( BYTE )Hex2ASCII( value % 0x10 ) );
        value /= 0x10;
    }
    if( len > 0 )
    {
        for( value = 0; value <= len; value++ )
            //Osd_DrawChar(xPos+value, yPos, (BYTE)'0');
        {
            Osd_DrawCharDirect( xPos + value, yPos, ( BYTE )3 );
        }
    }
}
void Osd_DrawHex( BYTE xPos, BYTE yPos, WORD value )
{
    DrawHex( xPos, yPos, value );
}
void Osd_DynamicLoadFont( BYTE addr, BYTE *fontPtr, WORD num )
{

    #if PropFontUseCommonArea
    if ( UserPrefLanguage == LANG_TChina )
    {
        SecondTblAddr = 0x80;
        LanguageIndex = 1;
    }
    else if ( UserPrefLanguage == LANG_Korea )
    {
        SecondTblAddr = 0x80;
        LanguageIndex = 2;
    }
    else
    {
        SecondTblAddr = 0xFF;
        LanguageIndex = 0;
    }
    #else
    pstPropFontSet1218=tPropFontSet;
    #endif
    g_u8PropFontFlags=SPACE1PIXEL;

    LoadPropFonts1218(addr, fontPtr, num, NULL, 0, 0, 0);

}

/*
BYTE Osd_DoubleBuffer(Bool u8Enable)
{
    u8Enable=0;
    return u8Enable;
}
*/

void msOSDuncall(void)
{
    drvOSD_SetWndHPosition(0, 0, 0);
    drvOSD_SetWndVPosition(0, 0, 0);
}

