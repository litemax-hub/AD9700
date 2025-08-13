
#define _DRVOSD_C

#include "Global.h"
#include "types.h"
#include "board.h"
#include "ms_reg.h"
#include "drvOSD.h"
#include "Ms_rwreg.h"
#include "msOSD.h"
#include "halRwreg.h"

#if ENABLE_OSD_ROTATION
#include "LoadPropFont1218.h" //for Get FONT_HEIGHT
#endif

extern  xdata BYTE  OsdFontColor;

#if ENABLE_OSD_ROTATION
WORD drvOSD_GetTxtCAStart(BYTE u8Wnd, BYTE u8XPos, BYTE u8YPos)
{
    BYTE u8LineOffset=msReadByte((u8Wnd<<5)+OSD2_12);
    WORD u16CABase=msRead2Byte((u8Wnd<<5)+OSD2_10);

    BYTE xdata u8Bank=msReadByte(OSD1_00);
    msWriteByte(OSD1_00,0x00);//subbank 0

    if (UserPrefOsdRotateMode==OSDRotateMenuItems_90)
    {
        u16CABase=GET_CABASE_90_0(u16CABase,u8LineOffset);
    }
    else
  #if defined(_OSD_ROTATION_180_)
    if (UserPrefOsdRotateMode==OSDRotateMenuItems_180)
    {
        BYTE u16Height;
        u16Height = (msRead2Byte(( u8Wnd << 5 )+OSD2_0A)- msRead2Byte(( u8Wnd << 5 )+OSD2_06))/(FONT_HEIGHT);
        u16CABase=GET_CABASE_180_0(u16CABase,u8LineOffset, u16Height);
    }
    else
  #endif
  #if defined(_OSD_ROTATION_270_)
    if (UserPrefOsdRotateMode==OSDRotateMenuItems_270)
    {
        BYTE u16Height;
        u16Height = (msRead2Byte((u8Wnd<<5)+OSD2_08)- msRead2Byte((u8Wnd<<5)+OSD2_04))/FONT_HEIGHT;
        u16CABase=GET_CABASE_270_0(u16CABase,u8LineOffset,u16Height);
    }
    else
  #endif
    msWriteByte(OSD1_00,u8Bank);
    return (u16CABase+(WORD)u8YPos*u8LineOffset+u8XPos);
}
#endif

void drvOSD_DrawRealChar(BYTE u8Wnd, BYTE u8XPos, BYTE u8YPos, BYTE u8Char)
{
    WORD u16TempValue;
    BYTE u8DrawCode = (!(u8YPos & BIT7));

    u8YPos &= 0x7F;

    u16TempValue=GET_TXT_CASTART(u8Wnd,u8XPos,u8YPos); // get real address
    WRITE_CAFSRAM_ADDR();
    msWrite2Byte(PORT_ATTR_ADDR, u16TempValue); // display font attribute
    WRITE_ATTRIBUTE();
    msWriteByte(PORT_ATTR_DATA, OsdFontColor);
    if (u8DrawCode)
    {
        WRITE_CAFSRAM_ADDR();
        msWrite2Byte(PORT_CODE_ADDR, u16TempValue); // dispaly font code
        WRITE_CODE();
        msWriteByte(PORT_CODE_DATA, u8Char);
    }

}
void drvOSD_DrawContinuousChar(BYTE u8Wnd,BYTE u8XPos, BYTE u8YPos, BYTE u8Char, BYTE u8Num)
{
    WORD u16TempValue;
    BYTE i;
    BYTE    u8DrawCode = (!(u8YPos & BIT7));

    u8YPos &= 0x7F;
    u16TempValue=GET_TXT_CASTART(u8Wnd,u8XPos,u8YPos); // get real address
    WRITE_CAFSRAM_ADDR();
    msWrite2Byte(PORT_ATTR_ADDR, u16TempValue);
    WRITE_ATTRIBUTE();
    for(i=0;i<u8Num;i++)
    {
        msWriteByte(PORT_ATTR_DATA, OsdFontColor);
    }
    if (u8DrawCode)
    {
        WRITE_CAFSRAM_ADDR();
        msWrite2Byte(PORT_CODE_ADDR, u16TempValue);
        WRITE_CODE();
        for(i=0;i<u8Num;i++)
        {
            msWriteByte(PORT_CODE_DATA, u8Char);
        }
    }
}
#if defined(_MANHATTAN_OSD_)
void drvOSD_DrawVContinuousChar(BYTE u8Wnd, BYTE u8XPos, BYTE u8YPos, BYTE u8Char, BYTE u8Num)
{
    WORD u16TempValue;
    BYTE i;
    BYTE u8DrawCode = (!(u8YPos & BIT7));

    u8YPos &= 0x7F;

    u16TempValue=GET_TXT_CASTART(u8Wnd,u8XPos,u8YPos); // get real address
    #define LINE_OFFSET u8Wnd
    LINE_OFFSET=msReadByte((u8Wnd<<5)+OSD2_12);
    for (i=0;i<u8Num;i++)
    {
        WRITE_CAFSRAM_ADDR();
        msWrite2Byte(PORT_ATTR_ADDR, u16TempValue);
        WRITE_ATTRIBUTE();
        msWriteByte(PORT_ATTR_DATA, OsdFontColor);
        u16TempValue+=LINE_OFFSET;
    }
    if (u8DrawCode)
    {
        for(i=0;i<u8Num;i++)
        {
            u16TempValue-=LINE_OFFSET;
            WRITE_CAFSRAM_ADDR();
            msWrite2Byte(PORT_CODE_ADDR, u16TempValue);
            WRITE_CODE();
            msWriteByte(PORT_CODE_DATA, u8Char);
        }
    }
    #undef LINE_OFFSET
}
#endif
void drvOSD_DrawBlankPlane(BYTE u8Wnd,BYTE u8XPos,BYTE u8YPos,BYTE u8Width,BYTE u8Height)
{
    BYTE i,j;
    WORD u16TempValue;
    BYTE IsDrawCode=(!(u8YPos & BIT7));

    u8YPos&=0x7F;
    u16TempValue=GET_TXT_CASTART(u8Wnd,u8XPos,u8YPos); // get real address
    #define LINE_OFFSET u8Wnd
    LINE_OFFSET=msReadByte((u8Wnd<<5)+OSD2_12);
    for (i = 0; i < u8Height; i++)
    {
        WRITE_CAFSRAM_ADDR();
        msWrite2Byte(PORT_ATTR_ADDR, u16TempValue);
        WRITE_ATTRIBUTE();
        for(j=0;j<u8Width;j++)
        {
            msWriteByte(PORT_ATTR_DATA, OsdFontColor);
        }
        u16TempValue+=LINE_OFFSET;
    }
    if (!IsDrawCode)
        return;
    u16TempValue-=(LINE_OFFSET*u8Height); // recover real address
    for (i = 0; i < u8Height; i++)
    {
        WRITE_CAFSRAM_ADDR();
        msWrite2Byte(PORT_CODE_ADDR, u16TempValue);
        WRITE_CODE();
        for(j=0;j<u8Width;j++)
        {
            msWriteByte(PORT_CODE_DATA, SPACE_FONT);
        }
        u16TempValue+=LINE_OFFSET;
    }
    #undef LINE_OFFSET

}

void drvOSD_SetWndLineOffset(BYTE u8Wnd, BYTE u8LineOffset)
{
    msWriteByte((u8Wnd<<5)+OSD2_12,u8LineOffset);
}
void drvOSD_SetWndHPosition(BYTE u8Wnd, WORD u16HStart, WORD u16HEnd)
{
    u8Wnd<<=5;
    DB_W2B((u8Wnd)+OSD2_04,u16HStart);
    DB_W2B((u8Wnd)+OSD2_08,u16HEnd);
}
void drvOSD_SetWndVPosition(BYTE u8Wnd, WORD u16VStart, WORD u16VEnd)
{
    u8Wnd<<=5;
    DB_W2B((u8Wnd)+OSD2_06,u16VStart);
    DB_W2B((u8Wnd)+OSD2_0A,u16VEnd);
}
WORD drvOSD_GetWndInfo(BYTE u8Wnd,BYTE u8Info)
{
    u8Wnd<<=5;
    if (u8Info==OSD_WND_INFO_H_START)
        return  msRead2Byte((u8Wnd)+OSD2_04);
    if (u8Info==OSD_WND_INFO_V_START)
        return msRead2Byte((u8Wnd)+OSD2_06);
    if (u8Info==OSD_WND_INFO_V_END)
        return msRead2Byte((u8Wnd)+OSD2_0A);
    if (u8Info==OSD_WND_INFO_LINE_OFFSET)
        return msReadByte((u8Wnd)+OSD2_12);
    if (u8Info==OSD_WND_INFO_CABASE)
        return msRead2Byte((u8Wnd)+OSD2_10);
    return 0;
}
void drvOSD_SetWndOn(BYTE u8Wnd,Bool u8On)
{
    msWriteByteMask((u8Wnd<<5)+OSD2_00,(u8On?BIT0:0),BIT0);
}
void drvOSD_SetWndCtrl(BYTE u8Wnd, BYTE u8CtrlType, WORD u16Value,WORD u16Mask)
{
    u8Wnd<<=5;
    if (u8CtrlType==OSD_WND_CTRL0)
        msWrite2ByteMask((u8Wnd)+OSD2_00,u16Value,u16Mask);
    else if (u8CtrlType==OSD_WND_CTRL1)
        msWrite2ByteMask((u8Wnd)+OSD2_02,u16Value,u16Mask);
    else if (u8CtrlType==OSD_WND_CTRL2)
        msWrite2ByteMask((u8Wnd)+OSD2_14,u16Value,u16Mask);
}
void drvOSD_SetWndFixAlpha(BYTE u8Wnd,BYTE u8Lvl,BYTE u8Value)
{
    u8Wnd<<=5;
    //inverse color key define
    msWriteByteMask((u8Wnd)+OSD2_15,BIT5,BIT5);
    if (u8Lvl==OSD_ALPHA_OUT)
        msWriteByteMask((u8Wnd)+OSD2_0C,u8Value,0x3F);
    else if (u8Lvl==OSD_ALPHA_LV1)
        msWriteByteMask((u8Wnd)+OSD2_0D,u8Value,0x3F);
    else if (u8Lvl==OSD_ALPHA_LV2)
        msWriteByteMask((u8Wnd)+OSD2_19,u8Value,0x3F);
}
void drvOSD_SetWndFixColor(BYTE u8Wnd,BYTE u8Red,BYTE u8Green,BYTE u8Blue)
{
    u8Wnd<<=5;
    msWriteByte((u8Wnd)+OSD2_16,u8Red);
    msWriteByte((u8Wnd)+OSD2_17,u8Green);
    msWriteByte((u8Wnd)+OSD2_18,u8Blue);
}
void drvOSD_SetWndShiftPixel(BYTE u8Wnd, BYTE u8X, BYTE u8Y)
{
    u8Wnd<<=5;
    msWriteByte((u8Wnd)+OSD2_0E,u8X);
    msWriteByte((u8Wnd)+OSD2_0F,u8Y);
}
void drvOSD_SetWndCABaseAddr(BYTE u8Wnd,WORD u16Addr)
{
    msWrite2Byte((u8Wnd<<5)+OSD2_10,u16Addr);
}
void drvOSD_SetWndFixAttr(BYTE u8Wnd,BYTE u8Enable,BYTE u8Attr)
{
    u8Wnd<<=5;
    msWriteByte((u8Wnd)+OSD2_1A,u8Attr);
    msWriteByteMask((u8Wnd)+OSD2_15,(u8Enable?BIT7:0),BIT7);
}
void drvOSD_SetGDEngineEnableWnd(BYTE u8GDEngine, WORD u16EnableFlags,WORD u16Mask)
{
    BYTE xdata u8Bank=old_msReadByte(OSD1_00);
    WORD xdata u16Addr;
    old_msWriteByte(OSD1_00,0x01);//subbank 1
    if (u8GDEngine<OSD_GD_EG4)
        u16Addr=OSD1_44+u8GDEngine;
    else
        u16Addr=OSD1_EC+(u8GDEngine-OSD_GD_EG4);// gd4/gd5 //110627
    if (u16EnableFlags&0xFF) //for wnd
    {
        old_msWriteByteMask(u16Addr,(u16EnableFlags&0xFF),(u16Mask&0xFF));
    }
    u16EnableFlags&=0x0F00;
    u16Mask&=0x0F00;
    if (u16Mask) //for timing gen
    {
        if (u8GDEngine&0x01) //GD1/GD3/GD5
        {
            u16EnableFlags>>=4;
            u16Mask>>=4;
        }
        else //GD0/GD2/GD4
        {
            u16EnableFlags>>=8;
            u16Mask>>=8;
        }
        old_msWriteByteMask(OSD1_EE+(u8GDEngine>>1),u16EnableFlags,u16Mask); //110627
    }

    old_msWriteByte(OSD1_00,u8Bank);
}

#if (CHIP_ID == CHIP_MT9701)
static WORD _HorSt = 0;
static WORD _HorEd = 0;
static WORD _FrameColorR = 0;
static WORD _FrameColorG = 0;
static WORD _FrameColorB = 0;
void drvOSD_FrameColor_Set(BYTE u8DispWin, BOOL bEnable, WORD R, WORD G, WORD B)
{
    //Backup and set pattern 
    WORD horSt = 0, horEd = 0;
    _FrameColorR = R;
    _FrameColorG = G;
    _FrameColorB = B;

    if (bEnable)
    {
        horSt = msRead2Byte(SC00_18);
        horEd = msRead2Byte(SC00_1C);
        if (horSt != 0 || horEd !=0)
        {
            _HorSt = horSt;
            _HorEd = horEd;
            msWrite2Byte(SC00_18, 0x00);
            msWrite2Byte(SC00_1C, 0x00);
        }
        /*if (SyncLossState() && FreeRunModeFlag)
        {
            if(u8DispWin == MAIN_WINDOW)
            {
                msWrite2Byte(SC65_E2,R);
                msWrite2Byte(SC65_E4,G);
                msWrite2Byte(SC65_E6,B);
            }
            else
            {
                msWrite2Byte(SC65_EE,R);
                msWrite2Byte(SC65_F0,G);
                msWrite2Byte(SC65_F2,B);
            }
        }
        else*/
        {
            if(u8DispWin == MAIN_WINDOW)
            {
                msWrite2Byte(SC65_E8,R);
                msWrite2Byte(SC65_EA,B);
                msWrite2Byte(SC65_EC,G);
            }
            else
            {
                msWrite2Byte(SC65_F4,R);
                msWrite2Byte(SC65_F6,B);
                msWrite2Byte(SC65_F8,G);
            }
        }
    }
    else// if(!(SyncLossState() && FreeRunModeFlag))
    {
        if(_HorSt != 0 || _HorEd != 0)
        {
            msWrite2Byte(SC00_18, _HorSt);
            msWrite2Byte(SC00_1C, _HorEd);
        }
    }

    /*if (SyncLossState() && FreeRunModeFlag)
        msWriteByteMask(SC65_32, (bEnable ? 0: BIT4), BIT4);
    else*/
        msWriteByteMask(SC65_32, (bEnable ? BIT4: 0), BIT4);
}
#endif

void drvOSD_FrameColorEnable(Bool bEnable)
{
#if (CHIP_ID == CHIP_MT9701)
        drvOSD_FrameColor_Set(MAIN_WINDOW, bEnable, _FrameColorR, _FrameColorG, _FrameColorB);
#else
    static WORD u16HstatBk = 0;

    if(bEnable)
    {
        u16HstatBk = msRead2Byte(SC00_18);
        msWrite2ByteMask(SC00_18, 0x00, 0xFFF);
    }
    else
    {
        msWrite2ByteMask(SC00_18, u16HstatBk, 0xFFF); //image H. start
    }
    msWriteByteMask(SC10_32, (bEnable ? BIT4 : 0), BIT4);
#endif
}
void drvOSD_FrameColorRGB(BYTE u8Red,BYTE u8Green,BYTE u8Blue)
{
    msWriteByte(SC10_33, u8Red);
    msWriteByte(SC10_34, u8Green);
    msWriteByte(SC10_35, u8Blue);
}

void drvOSD_WaitOsdDeEndInterrupt()
{
    g_bOutputVsyncIsrFlag = 0;
    TimeOutCounter = 50;
    while (TimeOutCounter && (msReadByte(OSD1_A0)&BIT1) && (!InputTimingChangeFlag));

    TimeOutCounter = 50;
    while (TimeOutCounter && !(msReadByte(OSD1_A0)&BIT1) && (!InputTimingChangeFlag) && (!g_bOutputVsyncIsrFlag));
}

void drvOSD_ClrOSD_SRAM(void)
{
    XDATA BYTE u8Retry;

    for (u8Retry=0;u8Retry<=10;u8Retry++)
    {
        CAFSRAM_ERASE_TRIG();
        SET_OSD_CTRL_REG_WR_PORT(_ENABLE);
        ForceDelay1ms(3);
        if (!(msReadByte(OSD1_65)&BIT1))
            break;
    }
    for (u8Retry=0;u8Retry<=10;u8Retry++)
    {
        PSRAM0_ERASE_TRIG();
        SET_OSD_CTRL_REG_WR_PORT(_ENABLE);
        ForceDelay1ms(3);
        if (!(msReadByte(OSD1_75)&BIT1))
            break;
    }
    for (u8Retry=0;u8Retry<=10;u8Retry++)
    {
        PSRAM1_ERASE_TRIG();
        SET_OSD_CTRL_REG_WR_PORT(_ENABLE);
        ForceDelay1ms(3);
        if (!(msReadByte(OSD1_85)&BIT1))
            break;
    }
}

// wait for coding
//extern void Osd_LoadColorPalette( void );

void drvOSD_uncall(void)
{
    drvOSD_DrawRealChar(0, 0, 0, 0);
    drvOSD_DrawContinuousChar(0, 0, 0, 0, 0);
    drvOSD_DrawBlankPlane(0, 0, 0, 0, 0);
    drvOSD_GetWndInfo(0, 0);
    drvOSD_SetWndOn(0, 0);
    drvOSD_SetWndCtrl(0, 0, 0, 0);
    drvOSD_SetWndFixAlpha(0, 0, 0);
    drvOSD_SetWndFixColor(0, 0, 0, 0);
    drvOSD_SetWndShiftPixel(0, 0, 0);
    drvOSD_SetWndCABaseAddr(0, 0);
    drvOSD_SetWndFixAttr(0, 0, 0);
    drvOSD_SetGDEngineEnableWnd(0, 0, 0);
    drvOSD_SetWndLineOffset(0, 0);
    drvOSD_WaitOsdDeEndInterrupt();
    //Osd_LoadColorPalette();
}

