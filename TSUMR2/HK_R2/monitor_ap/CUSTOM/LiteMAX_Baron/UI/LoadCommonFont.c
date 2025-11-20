#include "board.h"
#include "types.h"
#include "CommonFont.h"
#include "msOSD.h"
#include "Global.h"
#include "ms_reg.h"
#include "Ms_rwreg.h"
#include "Common.h"

#include "menudef.h"
//#include "extlib.h"
#include "ColorPalette.h"
#if CHIP_ID == CHIP_TSUMU
#include "ColorPalettetbl.h"
#endif
#include "LoadPropFont1218.h"

#include "drvOSD.h"
#include "LoadCommonFont.h"

#define _JUNO_OSD_

#if 0 //LiteMAX_OSD_TEST
extern void LoadMainMenuPropFont(void);
#endif

void LoadCompressColorFont(const BYTE *pu8FontTbl, WORD *pu16MapPtr, BYTE u8Count);
void mStar_LoadCompressedFont(BYTE u8Addr, const WORD *pu16FontPtr, WORD u16Count);

extern xdata BYTE MenuPageIndex;
extern xdata BYTE MenuItemIndex;


void LoadCommonFontUncall(void);

//8 color font raw data
void Osd_Load8ColorFont(BYTE u8Addr, BYTE *pu8Font, WORD u16Num)
{
    DWORD xdata u32Data;
    BYTE xdata u8FontData0,u8FontData1,u8FontData2;
    BYTE xdata u8PixelBit;
    BYTE xdata i;

    msWriteByteMask(OSD1_6D, BIT6,BIT7|BIT6);   //00: for 1/2Bpp; 01: for 3Bpp; 10:for 4Bpp;

    WRITE_CAFSRAM_ADDR();
    msWrite2Byte(PORT_FONT_ADDR, GET_FONT_RAM_ADDR(u8Addr));
    WRITE_FONT();
    // handle 2 lines, 8 pixels each time, 2 lines x 12 pixels = 8 pixels x 3 times
    while(u16Num)
    {
        for (i=0;i<3;i++)
        {
        //    0        1        2        3        4        5        6        7        8
        //11110000 00000000 11111111 00002222 22222222 00000000 22221111 11111111 22222222 //line 0/1
            if (i==0)
            {
                u8FontData0=((*(pu8Font))<<4)+((*(pu8Font+1))>>4);
                u8FontData1=((*(pu8Font))&0xF0)+((*(pu8Font+2))>>4);
                u8FontData2=((*(pu8Font+3))<<4)+((*(pu8Font+4))>>4);
            }
            else if (i==1)
            {
                u8FontData0=((*(pu8Font+1))<<4)+((*(pu8Font+3))>>4);
                u8FontData1=((*(pu8Font+2))<<4)+((*(pu8Font+6))&0x0F);
                u8FontData2=((*(pu8Font+4))<<4)+((*(pu8Font+6))>>4);
            }
            else
            {
                u8FontData0=(*(pu8Font+5));
                u8FontData1=(*(pu8Font+7));
                u8FontData2=(*(pu8Font+8));
            }
            u8PixelBit=BIT7;
            u32Data=0;
            while(u8PixelBit)
            {
                u32Data<<=3;
                if (u8FontData0&u8PixelBit) //color bit0
                    u32Data|=BIT0;
                if (u8FontData1&u8PixelBit) //color bit1
                    u32Data|=BIT1;
                if (u8FontData2&u8PixelBit) //color bit2
                    u32Data|=BIT2;
                u8PixelBit>>=1;
            }
            msWriteByte(PORT_FONT_DATA, u32Data>>16);
            msWriteByte(PORT_FONT_DATA, u32Data>>8);
            msWriteByte(PORT_FONT_DATA, u32Data);
        }
        if (u16Num>=9)
        {
            u16Num-=9;
            pu8Font+=9;
        }
        else
            u16Num=0;
    }

    msWriteByteMask(OSD1_6D, 0x00,BIT7|BIT6);    //00: for 1/2Bpp; 01: for 3Bpp; 10:for 4Bpp;

}

void Osd_LoadCompressColorFont( WORD addr, const BYTE *fontPtr, WORD *mapPtr, BYTE count )
{
    if(( addr & 0x0F00 ) != 0x0000 )
    {
        OSD_FONT_HI_ADDR_SET_BITS(( addr & 0x0F00 )>>8);
    }
    else
    {
        OSD_FONT_HI_ADDR_CLR_TO_0();
    }
    OSD_WRITE_FONT_ADDRESS(LOBYTE(addr));
    LoadCompressColorFont( fontPtr, mapPtr, count );
    OSD_FONT_HI_ADDR_CLR_TO_0();
}

void Osd_Write4ColorFontStartAddr( WORD wAddr )
{
	msWrite2ByteMask(OSD1_0A,wAddr,0x0FFF); // 4 color start
}

void Osd_Write8ColorFontStartAddr( WORD wAddr )
{
	msWrite2ByteMask(OSD1_0C,wAddr,0x0FFF); // 8 color start
}

#if DisplayLogo!=NoBrand
void Osd_LoadLogoFontCP( void )
{
	Osd_Write4ColorFontStartAddr( Logo_4C );

	Osd_LoadCompressColorFont( Logo_4C, tLogo_4C, NULL, Logo_Size);
}
#endif
void Osd_LoadFacoryFontCP( void )
{
    MonoColorSetting();
    mStar_LoadCompressedFont( (1), tFactoryFont0, 0 ); // load osd fonts
}

void LoadFfont( void )
{
    mStar_LoadCompressedFont( FfontStart, tFontF, 0 ); // load osd fonts
}

#if 0 //LiteMAX_OSD_TEST
void InitGradualColor(void)
{
	//init MAIN_GD_WIN
    drvOSD_SetWndCtrl(MAIN_GD_WIN, OSD_WND_CTRL0,
                      (OWC0_FP_MODE|
                       OWC0_GROUP_SEL0|
                       OWC0_LV1_ALPHA_SRC(OSD_A_FIX)|
                       OWC0_LV2_ALPHA_SRC(OSD_A_FIX)|
                       OWC0_OUT_ALPHA_SRC(OSD_A_FIX)),
                       OWC0MASK_ALL);
    drvOSD_SetWndCtrl(MAIN_GD_WIN, OSD_WND_CTRL1,
                     (OWC1_1BP_44MODE_EN|
                      OWC1_44MODE_TRS_EN|
                      OWC1_FONT_HEIGHT(18)),
                      OWC1MASK_ALL);
    drvOSD_SetWndCtrl(MAIN_GD_WIN, OSD_WND_CTRL2,
                     (OWC2_GD_COLOR_R(OSD_COLOR_FIX)|
                      OWC2_GD_COLOR_G(OSD_COLOR_FIX)|
                      OWC2_GD_COLOR_B(OSD_COLOR_FIX)),
                      OWC2MASK_ALL);

    drvOSD_SetWndFixAlpha(MAIN_GD_WIN,OSD_ALPHA_LV2,0x00); //PG   //(1-a)*low_lvl+a*top_lvl
    drvOSD_SetWndFixAlpha(MAIN_GD_WIN,OSD_ALPHA_LV1,0x3F); //FG
    drvOSD_SetWndFixAlpha(MAIN_GD_WIN,OSD_ALPHA_OUT,0x3F); //OSD
    drvOSD_SetWndCABaseAddr(MAIN_GD_WIN,OSD_MAIN_WND_CA_BASE);
}
#endif

void LoadCommonFont( void )
{
	#if LiteMAX_OSD_TEST
	#if ENABLE_DEBUG
	printData( "LoadCommonFont \r\n", 0);
	#endif
    //InitGradualColor();

	Osd_Write4ColorFontStartAddr( _4ColorFontStart );
	Osd_Write8ColorFontStartAddr( 0x0FFF );

	//Load 2 COLOR Font
    //Static Mono Font
    mStar_LoadCompressedFont(COMMON_FONT_START, tCommonArea, 0);/// Load Common font
    #if 1 //LiteMAX_Baron_OSD_TEST
    mStar_LoadCompressedFont(COMMON_FONT_2ND_START, tCommonArea2nd, 0);/// Load Common font
    #endif
    mStar_LoadCompressedFont(SM_CHECK_2C_FONT_START, tCheckMark2ColorIconFont, 0);
	#if 0
    LoadMainMenuPropFont();
	#endif
	//Load 4 COLOR Font
	// Frame
	Osd_LoadCompressColorFont( _4ColorFontStart, t4ColorFrame, NULL, Frame_Size);
	// FrameNew
	Osd_LoadCompressColorFont( FrameNew4CFontStart, t4ColorFrameNew, NULL, FrameNew_Size);
	// Main Icon
	Osd_LoadCompressColorFont( MainIcon4C_0_MainMenuIcon, t4ColorMainIcon, NULL, MainIcon4C_0_MainMenuIcon_Size);
	Osd_LoadCompressColorFont( MainIcon4C_1_BrightnessSub, t4ColorBrightnessSub, NULL, MainIcon4C_1_BrightnessSub_Size);
	Osd_LoadCompressColorFont( MainIcon4C_2_AudioSub, t4ColorAudioSub, NULL, MainIcon4C_2_AudioSub_Size);
	Osd_LoadCompressColorFont( MainIcon4C_3_ColorSub, t4ColorColorSub, NULL, MainIcon4C_3_ColorSub_Size);

	Osd_LoadCompressColorFont( MainIcon4C_4_ImageSub, t4ColorImageSub, NULL, MainIcon4C_4_ImageSub_Size);
	Osd_LoadCompressColorFont( MainIcon4C_5_OtherSub, t4ColorOtherSub, NULL, MainIcon4C_5_OtherSub_Size);
	#if LiteMAX_Baron_OSD_TEST
	Osd_LoadCompressColorFont( MainIcon4C_PowerKeyLock, t4ColorPowerKeyLock, NULL, MainIcon4C_PowerKeyLock_Size);
	Osd_LoadCompressColorFont( MainIcon4C_LoadDefaultSub, t4ColorLoadDefaultSub, NULL, MainIcon4C_LoadDefaultSub_Size);
	#endif
	//Osd_LoadCompressColorFont( MainIcon4C_PPMode, t4ColorPPMode, NULL, MainIcon4C_PPMode_Size);

	Osd_LoadColorPalette256(); // load osd color
	#else //#if LiteMAX_OSD_TEST
#if 0 // fix coverity error-deadcode
    BYTE i = 0;
    if(i)
    {
        LoadCommonFontUncall();
    }
#endif

    //Load 2 COLOR Font
#if ENABLE_DEBUG
    printData( "LoadCommonFont", 0);
#endif

	Osd_Write4ColorFontStartAddr( _4ColorFontStart );
	Osd_Write8ColorFontStartAddr( 0x0FFF );

    mStar_LoadCompressedFont( MonoFontStart, tCommonFont_2C, 0 );
    mStar_LoadCompressedFont( CursorFontStart, tCursorFont_2C, 0 );
    mStar_LoadCompressedFont( NumberFontStart, tNumberFont_2C, 0 );

    if( FactoryModeFlag )
        mStar_LoadCompressedFont( (FfontStart), tFontF, 0 ); // load osd fonts

    //Load 4 COLOR Font
	Osd_LoadCompressColorFont( SmallLogo_4C, tSmallLogo_4C, NULL, SmallLogo_Size);

    Osd_LoadColorPalette256(); // load osd color
    #endif //#if LiteMAX_OSD_TEST

}

void LoadCommonFontUncall(void)
{
    //OSDLoadFontData(0, 0);
   // Osd_Load291Font(0, 0, 0);

    Osd_Load8ColorFont(0,0,0);
}

extern xdata BYTE g_u8FontAddrHiBits;
void mStar_LoadCompressedFont(BYTE u8Addr, const WORD *pu16FontPtr, WORD u16Count)
{
    BYTE XDATA u8DupTimes, u8LineIndex;//, i;
    WORD XDATA u16Line1Data, u16Line2Data;
    BYTE XDATA u8LastTimes=0;
    #if defined(_ROSEN_OSD_)
    u16Line1Data=msReadByte(OSD1_09);
    u16Line1Data<<=8;
    u16Line1Data+=msReadByte(OSD1_08); //get font base entry
    u16Line1Data=(u16Line1Data+((((WORD)g_u8FontAddrHiBits)<<8)+u8Addr)*(((msReadByte(OSD1_0B)>>4)&0x03)+1))<<2; //get real address in cafram, ((font base entry)+unit*(unit size))*4 addr/entry
    msWriteByte(OSD1_66, (BYTE)u16Line1Data);
    msWriteByte(OSD1_67, (BYTE)(u16Line1Data>>8));
    #elif defined(_JUNO_OSD_)
    u16Line1Data=msReadByte(OSD1_09);
    u16Line1Data<<=8;
    u16Line1Data+=msReadByte(OSD1_08); //get font base entry
    u16Line1Data=(u16Line1Data+((((WORD)g_u8FontAddrHiBits)<<8)+u8Addr)*(((msReadByte(OSD1_0B)>>4)&0x03)+1))<<2; //get real address in cafram, ((font base entry)+unit*(unit size))*4 addr/entry
    u8LineIndex=msReadByte(OSD1_6D);
    msWriteByte(OSD1_6D,u8LineIndex&(~(BIT6|BIT7)));
    msWriteByte(OSD1_68,0x00);

    msWriteByte(OSD1_66, (BYTE)u16Line1Data);
    msWriteByte(OSD1_67, (BYTE)(u16Line1Data>>8));
    #else
    msWriteByte(OSD2_A3, u8Addr);
    #endif
    msWriteByte(OSD1_68,0x03);

    #define i   u8Addr
    u8LineIndex=0;
    while (*pu16FontPtr)
    {
        u8DupTimes=((*pu16FontPtr)>>12)&0xF;
        u16Line1Data=(*pu16FontPtr)&0xFFF;

        u8DupTimes-=u8LastTimes;
        u8LastTimes=0;

        if(u8DupTimes>=2)
        {
            u16Line2Data=u16Line1Data;
            for (i=0; i<(u8DupTimes>>1); i++)
            {
            #if defined(_ROSEN_OSD_)
                //xxxx0000 00001111
                //xxxx1111 22222222
                msWriteByte(OSD1_6C, (BYTE)(u16Line1Data>>4) );
                msWriteByte(OSD1_6C, ((u16Line1Data&0x0F)<<4)+(u16Line2Data>>8));
                msWriteByte(OSD1_6C, (u16Line2Data&0xFF));
            #elif defined(_JUNO_OSD_)
                //xxxx0000 00001111
                //xxxx1111 22222222
                msWriteByte(OSD1_66, (BYTE)(u16Line1Data>>4) );
                msWriteByte(OSD1_66, ((u16Line1Data&0x0F)<<4)+(u16Line2Data>>8));
                msWriteByte(OSD1_66, (u16Line2Data&0xFF));
            #else
                msWriteByte(OSD2_A4, ((u16Line2Data>>4)&0xF0)|((u16Line1Data>>8)&0xF));
                msWriteByte(OSD2_A4, u16Line1Data&0xFF);
                msWriteByte(OSD2_A4, u16Line2Data&0xFF);
            #endif
            }
            u8LineIndex+=(u8DupTimes&0xFE);
            u8DupTimes &= 1;
        }


        if (u8DupTimes&1) // must wait for even line
        {
            if (!(*(pu16FontPtr+1)))
                break;
            u16Line2Data=(*(pu16FontPtr+1))&0xFFF;
            u8LastTimes=1;
            #if defined(_ROSEN_OSD_)
            msWriteByte(OSD1_6C, (BYTE)(u16Line1Data>>4) );
            msWriteByte(OSD1_6C, ((u16Line1Data&0x0F)<<4)+(u16Line2Data>>8));
            msWriteByte(OSD1_6C, (u16Line2Data&0xFF));
            #elif defined(_JUNO_OSD_)
            msWriteByte(OSD1_66, (BYTE)(u16Line1Data>>4) );
            msWriteByte(OSD1_66, ((u16Line1Data&0x0F)<<4)+(u16Line2Data>>8));
            msWriteByte(OSD1_66, (u16Line2Data&0xFF));
            #else
            msWriteByte(OSD2_A4, ((u16Line2Data>>4)&0xF0)|((u16Line1Data>>8)&0xF));
            msWriteByte(OSD2_A4, u16Line1Data&0xFF);
            msWriteByte(OSD2_A4, u16Line2Data&0xFF);
            #endif
            u8LineIndex+=2;
        }

        pu16FontPtr++;

        if (u16Count)
        {
            if (u8LineIndex==18)
            {
                u16Count--;
                u8LineIndex=0;
                if (u16Count==0)
                    break;
            }
        }
    }
    #undef i
    //_msWriteByte(OSD2_AE, 0x00);
}

void LoadCompressColorFont(const BYTE *pu8FontTbl, WORD *pu16MapPtr, BYTE u8Count)
{
    WORD XDATA u16FontIndex; //<<
    WORD XDATA u16FontData1;
    WORD XDATA u16FontData2=0;
    const BYTE *pu8FontPtr;
    BYTE XDATA u8RepeatLine;
    BYTE XDATA u8LineCount;
    #if 1//defined(_ROSEN_OSD_) || defined(_JUNO_OSD_)
    //BYTE XDATA i; //<<
    DWORD XDATA u32Data;
    #endif
    pu8FontPtr=pu8FontTbl;


#if defined(_JUNO_OSD_)
    u8RepeatLine=msReadByte(OSD1_6D);
    msWriteByte(OSD1_6D,(u8RepeatLine&(~(BIT7|BIT6)))); //set to 2bpp mode
    msWriteByte(OSD1_68,0x03);//WRITE_FONT
#endif

    //for (i=0; i<u8Count; i++)
    while(u8Count--)
    {
        u16FontIndex=0;
        if (pu16MapPtr!=NULL)
        {
            u16FontIndex=*pu16MapPtr;
            pu8FontPtr=pu8FontTbl+(u16FontIndex&0x7FFF);
            pu16MapPtr++;
        }
        u8LineCount=18;

        //printData("font %d", i);
        while (u8LineCount)
        {
            if (u16FontIndex&0x8000 || pu16MapPtr==NULL) // compress
            {
              #if 0  //Don't use this in MST9U R2 due to data alignment issue (can't cast from BYTE to WORD)!
                u16FontData1=*(((WORD*)pu8FontPtr));
                u16FontData2=*(((WORD*)pu8FontPtr)+1);
              #else
                u16FontData1=(WORD)((*(pu8FontPtr))<<8)  + (((WORD)(*(pu8FontPtr+1))));
                u16FontData2=(WORD)((*(pu8FontPtr+2))<<8)+ (((WORD)(*(pu8FontPtr+3))));
              #endif
                u8RepeatLine=u16FontData1>>12;
                u8LineCount-=u8RepeatLine;
                u16FontData1&=0xFFF;

                if (u16FontData2&0xF000) // line with same data 1/2
                {
                    u16FontData2=u16FontData1;
                    pu8FontPtr+=2;
                }
                else// if (lineCount)
                    pu8FontPtr+=4;
            #if 1//defined(_ROSEN_OSD_) || defined(_JUNO_OSD_)
                u32Data=0; //init to 0
                u16FontIndex=BIT11;\
                while(u16FontIndex)
                {
                    u32Data<<=2;
                    if (u16FontData1&u16FontIndex) //color bit0
                        u32Data|=BIT0;
                    if (u16FontData2&u16FontIndex) //color bit1
                        u32Data|=BIT1;
                    u16FontIndex>>=1;
                }
            #endif
                while (u8RepeatLine--)
                {
                    #if defined(_ROSEN_OSD_)
                    msWriteByte(OSD1_6C, u32Data>>16);
                    msWriteByte(OSD1_6C, u32Data>>8);
                    msWriteByte(OSD1_6C, u32Data);
                    #elif defined(_JUNO_OSD_)
                    msWriteByte(OSD1_66, u32Data>>16);
                    msWriteByte(OSD1_66, u32Data>>8);
                    msWriteByte(OSD1_66, u32Data);
                    #else
                    msWriteByte(OSD2_A4, ((u16FontData2>>4)&0xF0)|((u16FontData1>>8)&0xF));
                    msWriteByte(OSD2_A4, u16FontData1&0xFF);
                    msWriteByte(OSD2_A4, u16FontData2&0xFF);
                    #endif
                }
            }
            else
            {
                #if 1//defined(_ROSEN_OSD_) || defined(_JUNO_OSD_)
                for (u8RepeatLine=0; u8RepeatLine<54; u8RepeatLine+=3)
                {
                    u16FontData1=(((*(pu8FontPtr+u8RepeatLine))&0x0F)<<8)+(*(pu8FontPtr+u8RepeatLine+1));
                    u16FontData2=(((*(pu8FontPtr+u8RepeatLine))&0xF0)<<4)+(*(pu8FontPtr+u8RepeatLine+2));
                    u32Data=0; //init to 0
                    u16FontIndex=BIT11;
                    while(u16FontIndex)
                    {
                        u32Data<<=2;
                        if (u16FontData1&u16FontIndex) //color bit0
                            u32Data|=BIT0;
                        if (u16FontData2&u16FontIndex) //color bit1
                            u32Data|=BIT1;
                        u16FontIndex>>=1;
                    }
                    #if defined(_ROSEN_OSD_)
                    msWriteByte(OSD1_6C, u32Data>>16);
                    msWriteByte(OSD1_6C, u32Data>>8);
                    msWriteByte(OSD1_6C, u32Data);
                    #elif defined(_JUNO_OSD_)
                    msWriteByte(OSD1_66, u32Data>>16);
                    msWriteByte(OSD1_66, u32Data>>8);
                    msWriteByte(OSD1_66, u32Data);
                    #endif
                }
                #else
                for (u8RepeatLine=0; u8RepeatLine<54; u8RepeatLine++)
                {
                    msWriteByte(OSD2_A4, *(pu8FontPtr+u8RepeatLine));
                }
                #endif
                u8LineCount=0;
            }
        }
    }
}

