


//OSD_WIN0 for PG
#define OSD_MAIN_WND        OSD_WIN1
#if 0 //LiteMAX_OSD_TEST
#define MAIN_GD_WIN         OSD_WIN2
#endif
#define OSD_BUTTON_WND      OSD_WIN2
#define OSD_CURSOR_WND      OSD_WIN3
#define OSD_CURSOR_WIN_BIT  OSD_GD_WIN3_EN_BIT
#define OSD_WIN_USED_MAX    OSD_WIN3

#define OSD_MAIN_WND_CA_BASE    0x0001
#define OSD_MAIN_WND_SIZE       1728//(20+20+24)*27
#define OSD_BUTTON_WND_CA_BASE  (OSD_MAIN_WND_CA_BASE+OSD_MAIN_WND_SIZE)
#define OSD_BUTTON_WND_SIZE     60//


extern BYTE xdata g_u8OsdWndNo;
extern BYTE xdata g_u8FontAddrHiBits;
//extern WORD xdata g_u16OsdWndCABase[];
extern BYTE xdata g_u8OsdFontDataHighByte;

#define OSD_SET_WND(u8Wnd)  (g_u8OsdWndNo=u8Wnd)


#define OSD_ENABLE_GRADIENT_COLOR()     //old_msWriteByteMask(OSD1_15, BIT3, BIT3)
#define OSD_DISABLE_GRADIENT_COLOR()    //old_msWriteByteMask(OSD1_15,    0, BIT3)

//Font RAM bits
#define OSD_FONT_HI_ADDR_SET_BITS(u8Addr)     (g_u8FontAddrHiBits = u8Addr)
#define OSD_FONT_HI_ADDR_CLR_TO_0()     (g_u8FontAddrHiBits = 0)

#define OSD_FONT_START_ENTRY        0x1D4   //464(0x1D0) + 2160 entries= 2088 c/a + 720 fonts
#define OSD_FONT_UNIT_SIZE          3       //12x18 font, 1.5x18 = 27 Bytes = 3 x 72 bits = 3 entries
#define GET_FONT_RAM_ADDR(u8Addr)   ((OSD_FONT_START_ENTRY+((((WORD)g_u8FontAddrHiBits)<<8)+(u8Addr))*OSD_FONT_UNIT_SIZE)<<2)


typedef struct
{
    BYTE bButtonOsd:1;
    BYTE bSync2Main:1;
}WrapLayerFlagType;

extern xdata BYTE OsdWindowWidth;
extern xdata BYTE OsdWindowHeight;
//extern BYTE mStar_ReadOSDByte( BYTE address );
//extern void mStar_WriteOSDByte( BYTE address, BYTE value );
extern void Osd_DrawNum( BYTE xPos, BYTE yPos, int value);//WORD value );
extern void Osd_DrawFactoryNum( BYTE xPos, BYTE yPos, int value);//WORD value );
extern void Osd_Draw4Num( BYTE xPos, BYTE yPos, int value);//WORD value );
extern void Osd_DrawHex( BYTE xPos, BYTE yPos, WORD value );
extern void Osd_SetPosition( BYTE xPos, BYTE yPos );
extern void Osd_SetWindowSize( BYTE width, BYTE height );
extern void Osd_SetTextColor( BYTE foreColor, BYTE backColor );
//extern void Osd_DrawChar(BYTE xPos, BYTE yPos, BYTE ch);
extern void Osd_DrawGuage( BYTE ucX, BYTE ucY, BYTE ucLength, BYTE ucValue );
extern void Osd_Show( void );
extern void Osd_Hide( void );
extern void Osd_SetTransparency(BYTE u8Value);
extern void Osd_InitOSD( void );
extern void Osd_DynamicLoadFont( BYTE addr, BYTE *fontPtr, WORD num );
extern void Osd_DrawPropStr( BYTE xPos, BYTE yPos, BYTE *str );
extern void DisableOsdWindow( BYTE winIndex );
extern void MoveOsdWindow( BYTE winIndex, BYTE YPos, BYTE xPosStar, BYTE Xlength, BYTE Ylength, BYTE FgColor, BYTE BgColor );
extern void Osd_DrawCharDirect(BYTE u8XPos, BYTE u8YPos, WORD u8Char);
//extern void Osd_SetText4Color(color);
//extern void Osd_DynamicLoadFont(BYTE addr, BYTE *fontPtr, WORD num);
extern void Osd_DrawPropStr( BYTE xPos, BYTE yPos, BYTE *str );
extern void MonoColorSetting( void );
extern void Osd_SetTextMonoColor( BYTE foreColor, BYTE backColor );
extern void Osd_DrawContinuesChar(BYTE u8XPos, BYTE u8YPos, WORD u8Char, BYTE u8Num);
//extern void mStar_WriteOSDByteMask( BYTE ucReg, BYTE ucValue, BYTE ucMask );

extern BYTE Osd_DoubleBuffer(Bool u8Enable);
extern void Osd_SetWindowEnable(BYTE u8Win_Num,Bool u8Enable);

