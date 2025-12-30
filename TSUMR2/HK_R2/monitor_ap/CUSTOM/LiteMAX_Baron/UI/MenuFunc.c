#include "types.h"
#include "board.h"
#include "Global.h"
#include "Keypaddef.h"
#include "menudef.h"
#include "Adjust.h"
//#include "msADC.h"
#include "ms_reg.h"
#include "Debug.h"
#include "Common.h"
#include "msOSD.h"
#include "Power.h"
#include "misc.h"
#include "AutoFunc.h"
#include "misc.h"
#include "NVRam.h"
#include "UserPref.h"
#include "mStar.h"
#include "Ms_rwreg.h"
#include "Mcu.h"
#include "MenuStrProp.h"
#include "Gamma.h"
#include "DDC2Bi.H"
#include "MsDLC.h"
#include "msACE.h"
//#include "MsRegTV.h" // 110927 coding reserved
//#include "Panel.h"
#include "OsdDefault.h"
#include "drvGamma.h"
#include "msflash.h"
#include "menufunc.h"
#include "drvOSD.h"
#include "drvADC.h"
#include "ColorPalette.h"
#if ENABLE_DP_INPUT
#include "mapi_DPRx.h"
#include "dpCommon.h"
#endif
#if 0//ENABLE_HDMI
#include "MsHDMI.h"
#endif
#if ENABLE_RTE
#include "msOVD.h"
#endif

#if ENABLE_DPS
#include "msDPS_Setting.h"
#endif
#if ENABLE_3DLUT
#include "drv3DLUT_C.h"
#endif
#include "msScaler.h"
#include "Adjust.h"
#if CHIP_ID==CHIP_MT9701
#include "dri_table_gen_MT9701.h"
#else
#include "dri_table_gen.h"
#endif

#if (MS_PM)
#include "Ms_PM.h"
#endif
#include "GPIO_DEF.h"
////////////////////////////////////
//#include "drvMcu.h"
#include "drvmStar.h"
//#include "halRwreg.h"
///////////////////////////////////
#if ENABLE_DAC
#include "drvDAC.h"
#endif
#if (ENABLE_FREESYNC == 0x1)||(ENABLE_DP_INPUT == 0x1)
#include "ComboApp.h"
#endif

#include "mailbox.h"
#include "CustomEDID.h"
#include "CustomFunc.h"

#if ENABLE_HDR
#include "drvHDR.h"
#endif
#define MenuFunc_DEBUG    0
#if ENABLE_DEBUG&&MenuFunc_DEBUG
    #define MenuFunc_printData(str, value)   printData(str, value)
    #define MenuFunc_printMsg(str)           printMsg(str)
#else
    #define MenuFunc_printData(str, value)
    #define MenuFunc_printMsg(str)
#endif

void ReadWarm1Setting( void );
void ReadWarm2Setting( void );
void ReadNormalSetting( void );
void ReadCool1Setting( void );
void ReadCool2Setting( void );
void ReadSRGBSetting( void );
#if Enable_Expansion
void SetExpansionMode( void );
Bool OSDSetExpansionMode( void );
#endif
WORD DecIncValue( MenuItemActionType action, WORD value, WORD minValue, WORD maxValue, BYTE step );
WORD GetScale100Value( WORD value, WORD minValue, WORD maxValue );
void SetYCMColor( MenuItemActionType action, WORD color, WORD value );
void SetECO( void );
Bool SetBFSize( BYTE ucSize );
void ReadColorTempSetting( void );

#if Enable_Expansion       // For 4:3 Mode BrightnessFrame Position
WORD xdata DispalyWidth;
WORD xdata HStartOffset = 0;
#define DISPLAY_WIDTH   DispalyWidth
#define HSTART_OFFSET   HStartOffset
#else       // Normal Mode
#define DISPLAY_WIDTH   g_sPnlInfo.sPnlTiming.u16Width
#define HSTART_OFFSET   0
#endif

#if MS_PM
extern XDATA sPM_Info  sPMInfo;
#endif

extern void Init_FactorySetting( void );

void ReadUserSetting( void );
//void Save9300KSetting(void);
//void Save6500KSetting(void);
BYTE* code ColorTempPtr[5][5] =
{
#if LiteMAX_Baron_UI
    {&UserPrefBrightnessWarm1, &UserPrefContrastWarm1, &UserPrefRedColorWarm1, &UserPrefGreenColorWarm1, &UserPrefBlueColorWarm1},
    {&UserPrefBrightnessUser, &UserPrefContrastUser, &UserPrefRedColorUser, &UserPrefGreenColorUser, &UserPrefBlueColorUser},
    {&UserPrefBrightnessCool1, &UserPrefContrastCool1, &UserPrefRedColorCool1, &UserPrefGreenColorCool1, &UserPrefBlueColorCool1},
    {&UserPrefBrightnessNormal, &UserPrefContrastNormal, &UserPrefRedColorNormal, &UserPrefGreenColorNormal, &UserPrefBlueColorNormal},
    {&FUserPrefBrightnessSRGB, &FUserPrefContrastSRGB, &UserPrefRedColorSRGB, &UserPrefGreenColorSRGB, &UserPrefBlueColorSRGB},
#else
    {&UserPrefBrightnessCool1, &UserPrefContrastCool1, &UserPrefRedColorCool1, &UserPrefGreenColorCool1, &UserPrefBlueColorCool1},
    {&UserPrefBrightnessNormal, &UserPrefContrastNormal, &UserPrefRedColorNormal, &UserPrefGreenColorNormal, &UserPrefBlueColorNormal},
    {&FUserPrefBrightnessSRGB, &FUserPrefContrastSRGB, &UserPrefRedColorSRGB, &UserPrefGreenColorSRGB, &UserPrefBlueColorSRGB},
    {&UserPrefBrightnessUser, &UserPrefContrastUser, &UserPrefRedColorUser, &UserPrefGreenColorUser, &UserPrefBlueColorUser},
    {&UserPrefBrightnessWarm1, &UserPrefContrastWarm1, &UserPrefRedColorWarm1, &UserPrefGreenColorWarm1, &UserPrefBlueColorWarm1},
#endif
};
//////////////////////////////////////////////////////////////////////////
// Matrix for convert to sRGB space
//////////////////////////////////////////////////////////////////////////
code short tSRGB[3][3] =
{
    { 1096, -43, -28 }, // R  1.0694, -0.0424, -0.0270
    { - 21,   1063, -18 }, // G -0.0204,  1.0376, -0.0172
    { - 1,   34,   991 }  // B -0.0009,  0.0330,  0.9679

};
extern code short tNormalColorCorrectionMatrix[3][3];
/*=
{
    { 1024,    0,       0},
    {     0,    1024,       0},
    {    0,     0,      1024}
};
*/
//DLC Table......
BYTE code t_MWEDLC_Linear_Table[] =
{
    0x07, 0x17, 0x27, 0x37, 0x47, 0x57, 0x67, 0x77, 0x87, 0x97, 0xA7, 0xB7, 0xC7, 0xD7, 0xE7, 0xF7,
    0x87, 0x9B, 0x0F, 0xC2, 0xBD, 0x4E, 0xC6, 0xFD, 0xF3, 0x3E, 0xE9, 0x35, 0x75, 0xED, 0x76, 0x8D,
    0xC1, 0x74, 0xD4, 0x3C, 0x3A, 0x18, 0xE0, 0x0A, 0xD6, 0xE3, 0x33, 0x0C, 0x0A, 0x84, 0xB7, 0xA0,
};

/*
BYTE code t_Normal_ColorSettingTable[] =
{
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x04, 0x00, 0x00, 0x00, 0x00, 0xD4, 0x55, 0xED, 0xDC, 0xEA, 0x20, 0x81, 0x11, 0x1B, 0x01, 0xFE,
    0xDC, 0xC2, 0xAD, 0xDE, 0x23, 0x63, 0x0E, 0x58, 0x10, 0x21, 0xF1, 0xFA, 0x5D, 0x7B, 0xF1, 0x87,
};
*/

//////////////////////////////////////////////////////////////////////////
// Matrix for VIDEO color correcttion
//////////////////////////////////////////////////////////////////////////
//code short tStandardColorCorrectionMatrix[][3]=
#if 0
code short t_Normal_ColorSettingTable[][3]=
{
{0x0400, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, 0x0000, 0x0000},
{0x0400,-0x0369, 0x01AB,-0x051B, 0x0012,-0x01D9, 0x0777,-0x0378},
{-0x0735, 0x017A, 0x019E,-0x026E, 0x0274, 0x0687,-0x07A6, 0x0280},
{-0x0831, 0x0000,-0x0500, 0x0500,-0x0005, 0x0000, 0x0000, 0x0000},
};
#else
code short t_Normal_ColorSettingTable[][3]=
{
 {0x0400, 0x0000, 0x0000},
 {0x0000, 0x0400, 0x0000},
 {0x0000, 0x0000, 0x0400},
};
#endif

extern ST_COMBO_COLOR_FORMAT GetInputCombColorFormat(void);

//================================================================================
WORD GetNonFuncValue( void )
{
    return 50;
}

WORD DecIncValue( MenuItemActionType action, WORD value, WORD minValue, WORD maxValue, BYTE step )
{
    if( action == MIA_IncValue || action == MIA_InvDecValue )
    {
        if( value >= maxValue )
        {
            return value;
        }
        minValue = value + step;
        if( minValue > maxValue )
        {
            minValue = maxValue;
        }
        value = minValue;
    }
    else
        // Decrease value
    {
        if( value <= minValue )
        {
            return value;
        }
        #if LiteMAX_Baron_OSD_TEST
        if(value<=step)
            maxValue = 0;
        else
            maxValue = value - step;
        #else
        maxValue = value - step;
        #endif
        if( maxValue < minValue )
        {
            maxValue = minValue;
        }
        value = maxValue;
    }
    return value;
}
WORD GetScale100Value( WORD value, WORD minValue, WORD maxValue )
{
    maxValue = ( maxValue <= minValue ) ? minValue : ( maxValue - minValue );
    //maxValue = maxValue - minValue;
    value = value - minValue;
    if( value > 650 )
    {
        value /= 2;
        maxValue /= 2;
    }
    value = (( WORD )value * 100 + maxValue / 2 ) / maxValue;
    value %= 101;
    return value;
}
extern BYTE xdata PrevMenuItemIndex;
extern BYTE xdata MenuItemIndex;
extern BYTE xdata MenuPageIndex;
#if MWEFunction
void IndependentColorControl( void )
{
    //BYTE code tColorRangeTbl[] = {0x20, 0x28, 0x20, 0x20, 0x28, 0x20, 0x19, 0x29, 0x19};
    BYTE i, j;

    BYTE code tColorControlTbl[][18] = //0x3A~0x4B
    {
        //  3A  3B   3C   3D   3E   3F   40   41   42   43   44   45   46   47   48   49   4A   4B
        {0x08, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //FullEnhanceFlag
        {0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //SkinFlag
        {0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //BlueFlag
        {0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //GreenFlag
        {0x08, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} //ColorAutoDetectFlag
    };

    //for( i = 0; i < 9; i++ ) //0x31~0x39
        //msWriteByte( SC14_08+ i, tColorRangeTbl[i] );

    if( FullEnhanceFlag )
        j = 0;
    else if( SkinFlag )
        j = 1;
    else if( BlueFlag )
        j = 2;
    else if( GreenFlag )
        j = 3;
    else if( ColorAutoDetectFlag )
        j = 4;

    for( i = 0; i < 18; i++ ) //0x3A~0x4B
        msWriteByte( SC14_0E + i, tColorControlTbl[j][i] );
    if( !DemoFlag )
        msWriteByteMask( SC14_02, 0x31, 0x31);
    else
    {
        SetBFSize( UserPrefBFSize ); //SetBFSize();
        msWriteByteMask( SC14_03, 0x31, 0x31 );
    }
}
#if !ENABLE_HDMI
void SetColorBoostWindow( BYTE mode )
{
    if( DemoFlag )
        mode = COLORBOOST_HALF;
    if( mode != COLORBOOST_OFF )
    {
        // 2006/11/10 10:43PM by KK         msAdjustSubBrightness( UserPrefSubBrightness );
        if( DemoFlag )
            mStar_AdjustContrast(DefContrastBase + DefContrast );//msAdjustVideoContrast(MAIN_WINDOW, DefContrastBase + DefContrast );
        else
            mStar_AdjustContrast(DefContrastBase + UserPrefContrast );//msAdjustVideoContrast(MAIN_WINDOW, DefContrastBase + UserPrefContrast );
        if( mode == COLORBOOST_HALF )
        {
            UserPrefBFHSize = g_sPnlInfo.sPnlTiming.u16Width / 2;
            UserPrefBFVSize = g_sPnlInfo.sPnlTiming.u16Height;
            UserPrefBFHStart = 0;
            UserPrefBFVStart = 0;
        }
        else if( mode == COLORBOOST_FULL )
        {
            UserPrefBFHSize = g_sPnlInfo.sPnlTiming.u16Width;
            UserPrefBFVSize = g_sPnlInfo.sPnlTiming.u16Height;
            UserPrefBFHStart = 0;
            UserPrefBFVStart = 0;
        }
        if( DemoFlag )
        {
            msAccSetup( UserPrefBFHStart, UserPrefBFHSize, UserPrefBFVStart, UserPrefBFVSize );
            msAccOnOff( 1 );
        }

    }
    else
        // COLORBOOST_OFF
    {
        msAccOnOff( 0 );
    }
}
#endif
#if !ENABLE_HDMI
void SetColorEnhance( bit enable )
{
    BYTE *pColorTable;
    enable = 1;
    if( UserPrefDcrMode )
    {
        pColorTable = (BYTE *)t_Normal_ColorSettingTable;
        //LoadACETable( pColorTable, DefHue, DefSaturation, 0x80 ); //  UserPrefSubContrast  );
        LoadACETable(MAIN_WINDOW, pColorTable, DefHue, DefSaturation, 0x80 ); //  UserPrefSubContrast  );
        msAccOnOff( 1 );
        msDlcOnOff( 1 );
    }
#if 1 // wait for coding

#else
    else if( FullEnhanceFlag || SkinFlag || BlueFlag || GreenFlag || DemoFlag || ColorAutoDetectFlag )

    {

        if( FullEnhanceFlag || ColorAutoDetectFlag || GreenFlag || BlueFlag || SkinFlag )
            IndependentColorControl();
        else
        {
            old_msWriteByte( BK0_00, REGBANK6 );
            old_msWriteByte( BK6_30, 0x00 );
            old_msWriteByte( BK0_00, REGBANK0 );
            //LoadACETable(pColorTable, DefHue, DefSaturation, DefContrastBase + UserPrefContrast);
        }
        if( GreenFlag || SkinFlag )
        {
            old_msWriteByte( BK0_00, REGBANK3 );
            old_msWriteByte( BK3_72, 0x6A );
            old_msWriteByte( BK3_73, 0x6A );
            old_msWriteByte( BK3_74, 0x6A );
            old_msWriteByte( BK0_00, REGBANK0 );
        }
        else if( ColorAutoDetectFlag )
        {
            old_msWriteByte( BK0_00, REGBANK3 );
            old_msWriteByte( BK3_72, 0x60 );
            old_msWriteByte( BK3_73, 0x60 );
            old_msWriteByte( BK3_74, 0x60 );
            old_msWriteByte( BK0_00, REGBANK0 );
        }
        else
        {
            old_msWriteByte( BK0_00, REGBANK3 );
            old_msWriteByte( BK3_72, 0x70 );
            old_msWriteByte( BK3_73, 0x70 );
            old_msWriteByte( BK3_74, 0x70 );
            old_msWriteByte( BK0_00, REGBANK0 );
        }
    }
    else
    {
        old_msWriteByte( BK0_00, REGBANK6 );
        old_msWriteByte( BK6_30, 0x00 );
        old_msWriteByte( BK0_00, REGBANK0 );
        //LoadACETable(pColorTable, DefHue, DefSaturation, DefContrastBase + UserPrefContrast);
    }
#endif

}
#endif
#if ENABLE_HDMI
void SetColorBoostWin( BYTE mode )
{
    if( DemoFlag )
        mode =  COLORBOOST_HALF;

    if( mode != COLORBOOST_OFF )
    {
        mStar_AdjustContrast(DefContrastBase + UserPrefContrast );//msAdjustVideoContrast(MAIN_WINDOW, DefContrastBase + UserPrefContrast );

        if( mode == COLORBOOST_HALF )
        {
            UserPrefBFHSize = g_sPnlInfo.sPnlTiming.u16Width / 2;
            UserPrefBFVSize = g_sPnlInfo.sPnlTiming.u16Height;
            UserPrefBFHStart = 0;
            UserPrefBFVStart = 0;
        }
        else if( mode == COLORBOOST_FULL )
        {
            UserPrefBFHSize = g_sPnlInfo.sPnlTiming.u16Width ;
            UserPrefBFVSize = g_sPnlInfo.sPnlTiming.u16Height;
            UserPrefBFHStart = 0;
            UserPrefBFVStart = 0;
        }



        if( DemoFlag )
        {
            msAccSetup(UserPrefBFHStart, UserPrefBFHSize, UserPrefBFVStart, UserPrefBFVSize );
            if( IsColorspaceRGBInput() )
                msAccOnOff( 1 );
            else
                msWriteByteMask( SC00_5C, BIT3, BIT3 ); //old_msWriteByte( BK0_5C, old_msReadByte( BK0_5C ) | BIT3 );
        }
        else
        {
            if( IsColorspaceRGBInput() )
                msAccOnOff( 0 );
            else
                msWriteByteMask( SC00_5C, 0x00, BIT3 );  //old_msWriteByte( BK0_5C, old_msReadByte( BK0_5C )&~BIT3 );
        }

    }
    else // COLORBOOST_OFF
    {
        msAccSetup( 0, 0, 0, 0 );
        //msAccOnOff( 0 );
    }

}

void SetColorMatrix( void )
{
    BYTE *pColorTable;

    if( UserPrefDcrMode )
    {
        pColorTable = (BYTE *)t_Normal_ColorSettingTable;
        //LoadACETable( pColorTable, UserPrefHue, UserPrefSaturation, 0x80 ); //  UserPrefSubContrast  );
        LoadACETable( MAIN_WINDOW, pColorTable, UserPrefHue, UserPrefSaturation, 0x80 ); //  UserPrefSubContrast  );
        //msAccOnOff( 1 );
        //msDlcOnOff( 1 );
    }
    else  if( FullEnhanceFlag || SkinFlag || BlueFlag || GreenFlag || DemoFlag || ColorAutoDetectFlag )  //NormalSkin+GreenENhance+BlueEnhance
    {

        if( FullEnhanceFlag || ColorAutoDetectFlag || GreenFlag || BlueFlag || SkinFlag )
            IndependentColorControl();
        else
        {
            //old_msWriteByte( BK0_00, REGBANK6 );
            if( !DemoFlag )
            	msWriteByte( SC14_02, 0x00 );
	    	else
		 		msWriteByte( SC14_03, 0x00 );
            //old_msWriteByte( BK0_00, REGBANK0 );
            //LoadACETable(pColorTable, DefHue, DefSaturation, DefContrastBase + UserPrefContrast);
        }
		if( !DemoFlag )
		{
	        if( GreenFlag || SkinFlag )
	        {
	            msWrite2Byte( SC0F_62, 0x350 );
	            msWrite2Byte( SC0F_64, 0x350 );
	            msWrite2Byte( SC0F_66, 0x350 );
	        }
	        else if( ColorAutoDetectFlag )
	        {
	            msWrite2Byte( SC0F_62, 0x300 );
	            msWrite2Byte( SC0F_64, 0x300 );
	            msWrite2Byte( SC0F_66, 0x300 );
	        }
	        else
	        {
	            msWrite2Byte( SC0F_62, 0x380 );
	            msWrite2Byte( SC0F_64, 0x380 );
	            msWrite2Byte( SC0F_66, 0x380 );
	        }
		}
		else
		{
	        if( GreenFlag || SkinFlag )
	        {
		            msWrite2Byte( SC0F_7E, 0x350 );
		            msWrite2Byte( SC0F_80, 0x350 );
		            msWrite2Byte( SC0F_82, 0x350 );
	        }
	        else if( ColorAutoDetectFlag )
	        {
		            msWrite2Byte( SC0F_7E, 0x300 );
		            msWrite2Byte( SC0F_80, 0x300 );
		            msWrite2Byte( SC0F_82, 0x300 );
	        }
	        else
	        {
		            msWrite2Byte( SC0F_7E, 0x380 );
		            msWrite2Byte( SC0F_80, 0x380 );
		            msWrite2Byte( SC0F_82, 0x380 );
	        }
    	}

		if( !DemoFlag )
			msWriteByteMask(SC0F_71, BIT7, BIT7);
		else
			msWriteByteMask(SC0F_7F, BIT7, BIT7);

    }
    else
    {
		if( !DemoFlag )
		{
			msWriteByte( SC14_02, 0x00 );
			msWriteByteMask(SC0F_71, 0, BIT7);
		}
		else
		{
			msWriteByte( SC14_03, 0x00 );
			msWriteByteMask(SC0F_7F, 0, BIT7);
		}
        //LoadACETable(pColorTable, DefHue, DefSaturation, DefContrastBase + UserPrefContrast);
    }
}

void SetColorSpace( void )
{
#if ENABLE_HDMI
    BYTE XDATA ucDomain;
#endif

#if ENABLE_HDMI
    ucDomain = 0x00;

#if (!ENABLE_FULL_RGB_COLOR_PATH)
    if( !IsColorspaceRGBInput() )
        ucDomain = 0xFF;
#else
   if (msACEGetEnableGlobalHueSat())  //using Y domain function: global hue / saturation
        ucDomain = 0xFF;
#endif
#endif

    if( DemoFlag || BlueFlag || SkinFlag || GreenFlag || ColorAutoDetectFlag || FullEnhanceFlag )
    {
#if ENABLE_HDMI
        if( ucDomain )
        {
            if( DemoFlag )
            {
                msWriteByteMask( SC07_40, BIT0, BIT0); // CSC old_msWriteByte( BK0_5C, old_msReadByte( BK0_5C ) | BIT3 );
            }
            else
            {
                msWriteByteMask( SC07_40, BIT0, BIT0); // CSC old_msWriteByte( BK0_02, old_msReadByte( BK0_02 ) | BIT3 );
                msWriteByteMask( SC00_5C, 0x00, BIT3 );  //old_msWriteByte( BK0_5C, old_msReadByte( BK0_5C )&~BIT3 );
            }
        }
        else
#endif
            if( DemoFlag )
                msAccOnOff( 1 );
    }
    else
    {
#if ENABLE_HDMI
        //if( ucDomain )
            //msWriteByteMask( SC0_5C, 0x00, BIT3 );  //old_msWriteByte( BK0_5C, old_msReadByte( BK0_5C )&~BIT3 );
        //else
#endif
            msAccOnOff( 0 );
#if ENABLE_HDMI
        if( ucDomain )
            msWriteByteMask( SC07_40, BIT0, BIT0); //old_msWriteByte( BK0_02, old_msReadByte( BK0_02 ) | BIT3 );
#endif
    }
}
#endif

//================================================================================
Bool AdjustColorBoostMode( MenuItemActionType action )
{
    action = 0;
    if( !DemoFlag )
    {
        Clr_DemoFlag();
    }
    Clr_SkinFlag();
    Clr_GreenFlag();
    Clr_BlueFlag();
    Clr_ColorAutoDetectFlag();
    Clr_PictureBoostFlag();
    //if (UserPrefDcrMode)
    //SetECO();
    UserPrefDcrMode = 0;
    UserPrefECOMode = ECO_Standard;
    SetECO();
#if ENABLE_HDMI
    msDlcOnOff( _DISABLE );
#endif

    if( FullEnhanceFlag )        // clr
    {
        Clr_FullEnhanceFlag();
#if ENABLE_HDMI
        SetColorBoostWin( COLORBOOST_OFF );
#else
        SetColorBoostWindow( COLORBOOST_OFF );
#endif
    }
    else        // set
    {
        Set_FullEnhanceFlag();
#if ENABLE_HDMI
        SetColorBoostWin( COLORBOOST_FULL );
#else
        SetColorBoostWindow( COLORBOOST_FULL );
#endif
    }
#if ENABLE_HDMI
    SetColorMatrix();
    SetColorSpace();
#else
    SetColorEnhance( 1 );
#endif

#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( ColorFlag ), UserPrefColorFlags );
#else
    UserPref_EnableFlashSaveBit( bFlashSaveMonitorBit );
#endif
    //Benz 2007.4.24   9:50:19      Osd_Draw4Num(7,1,UserPrefColorFlags);
    return TRUE;
}

//================================================================================
Bool AdjustSkinProtectMode( MenuItemActionType action )
{
    action = action;
    if( !DemoFlag )
    {
        Clr_DemoFlag();
    }
    Clr_GreenFlag();
    Clr_FullEnhanceFlag();
    Clr_BlueFlag();
    Clr_ColorAutoDetectFlag();
    Clr_PictureBoostFlag();

    //if (UserPrefDcrMode)
    //SetECO();
    UserPrefDcrMode = 0;
#if 0//Enable_Lightsensor
    UserPrefiCareMode = LightSensorOff;
#endif
    UserPrefECOMode = ECO_Standard;
    SetECO();

#if ENABLE_HDMI
    msDlcOnOff( _DISABLE );
#endif

    if( SkinFlag )
    {
        Clr_SkinFlag();
#if ENABLE_HDMI
        SetColorBoostWin( COLORBOOST_OFF );
#else
        SetColorBoostWindow( COLORBOOST_OFF );
#endif
    }
    else
    {
        Set_SkinFlag();
#if ENABLE_HDMI
        SetColorBoostWin( COLORBOOST_FULL );
#else
        SetColorBoostWindow( COLORBOOST_FULL );
#endif
    }
#if ENABLE_HDMI
    SetColorMatrix();
    SetColorSpace();
#else
    SetColorEnhance( 1 );
#endif

#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( ColorFlag ), UserPrefColorFlags );
#else
    UserPref_EnableFlashSaveBit( bFlashSaveMonitorBit );
#endif
    //Benz 2007.4.24   9:50:24  Osd_Draw4Num(6,1,UserPrefColorFlags);
    return TRUE;
}
Bool AdjustGreenEnhanceMode( MenuItemActionType action )
{
    action = action;
    if( !DemoFlag )
    {
        Clr_DemoFlag();
    }
    Clr_SkinFlag();
    Clr_FullEnhanceFlag();
    Clr_BlueFlag();
    Clr_ColorAutoDetectFlag();
    Clr_PictureBoostFlag();
    //if (UserPrefDcrMode)
    //SetECO();
    UserPrefDcrMode = 0;
#if 0//Enable_Lightsensor
    UserPrefiCareMode = LightSensorOff;
#endif
    UserPrefECOMode = ECO_Standard;
    SetECO();

#if ENABLE_HDMI
    msDlcOnOff( _DISABLE );
#endif

    if( GreenFlag )
    {
        Clr_GreenFlag();
#if ENABLE_HDMI
        SetColorBoostWin( COLORBOOST_OFF );
#else
        SetColorBoostWindow( COLORBOOST_OFF );
#endif
    }
    else
    {
        Set_GreenFlag();
#if ENABLE_HDMI
        SetColorBoostWin( COLORBOOST_FULL );
#else
        SetColorBoostWindow( COLORBOOST_FULL );
#endif
    }
#if ENABLE_HDMI
    SetColorMatrix();
    SetColorSpace();
#else
    SetColorEnhance( 1 );
#endif
#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( ColorFlag ), UserPrefColorFlags );
#else
    UserPref_EnableFlashSaveBit( bFlashSaveMonitorBit );
#endif
    //Benz 2007.4.24   9:50:28      Osd_Draw4Num(5,1,UserPrefColorFlags);
    return TRUE;
}
Bool AdjustBlueEnhanceMode( MenuItemActionType action )
{
    action = action;
    if( !DemoFlag )
    {
        Clr_DemoFlag();
    }
    // color boost menu
    Clr_FullEnhanceFlag();
    Clr_GreenFlag();
    Clr_SkinFlag();
    Clr_ColorAutoDetectFlag();
    // picture boost menu
    Clr_PictureBoostFlag();
    // DCR function
    //if (UserPrefDcrMode)
    //SetECO();
    UserPrefDcrMode = 0;
#if 0//Enable_Lightsensor
    UserPrefiCareMode = LightSensorOff;
#endif
    UserPrefECOMode = ECO_Standard;
    SetECO();

#if ENABLE_HDMI
    msDlcOnOff( _DISABLE );
#endif

    if( BlueFlag )
    {
        Clr_BlueFlag();
#if ENABLE_HDMI
        SetColorBoostWin( COLORBOOST_OFF );
#else
        SetColorBoostWindow( COLORBOOST_OFF );
#endif
    }
    else
    {
        Set_BlueFlag();
#if ENABLE_HDMI
        SetColorBoostWin( COLORBOOST_FULL );
#else
        SetColorBoostWindow( COLORBOOST_FULL );
#endif
    }
#if ENABLE_HDMI
    SetColorMatrix();
    SetColorSpace();
#else
    SetColorEnhance( 1 );
#endif
#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( ColorFlag ), UserPrefColorFlags );
#else
    UserPref_EnableFlashSaveBit( bFlashSaveMonitorBit );
#endif
    //Benz 2007.4.24   9:50:33      Osd_Draw4Num(4,1,UserPrefColorFlags);
    return TRUE;
}

Bool AdjustAutoDetectMode( MenuItemActionType action )
{
    action = action;
    if( !DemoFlag )
        // when demo on, don't clear the demo flag
    {
        Clr_DemoFlag();
    }
    // color boost menu
    Clr_FullEnhanceFlag();
    Clr_GreenFlag();
    Clr_BlueFlag();
    Clr_SkinFlag();
    // picture boost menu
    Clr_PictureBoostFlag();
    // DCR function
    //if (UserPrefDcrMode)
    //SetECO();
    UserPrefDcrMode = 0;
#if 0//Enable_Lightsensor
    UserPrefiCareMode = LightSensorOff;
#endif
    UserPrefECOMode = ECO_Standard;
    SetECO();

#if ENABLE_HDMI
    msDlcOnOff( _DISABLE );
#endif

    if( ColorAutoDetectFlag )
    {
        Clr_ColorAutoDetectFlag();
#if ENABLE_HDMI
        SetColorBoostWin( COLORBOOST_OFF );
#else
        msDlcOnOff( _DISABLE );
        SetColorBoostWindow( COLORBOOST_OFF );
#endif
    }
    else
    {
        Set_ColorAutoDetectFlag();
#if ENABLE_HDMI
        SetColorBoostWin( COLORBOOST_FULL );
#else
        LoadDLCTable( t_MWEDLC_Linear_Table );
        msDlcOnOff( _DISABLE );
        SetColorBoostWindow( COLORBOOST_FULL );
#endif
    }
#if ENABLE_HDMI
    SetColorMatrix();
    SetColorSpace();
#else
    SetColorEnhance( 1 );
#endif
#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( ColorFlag ), UserPrefColorFlags );
#else
    UserPref_EnableFlashSaveBit( bFlashSaveMonitorBit );
#endif
    //Benz 2007.4.24   9:50:40      Osd_Draw4Num(3,1,UserPrefColorFlags);
    return TRUE;
}
Bool AdjustDemoMode( MenuItemActionType action )
{
    action = action;
    Clr_PictureBoostFlag();
    //if (UserPrefDcrMode)
    //SetECO();
    UserPrefDcrMode = 0;
#if 0//Enable_Lightsensor
    UserPrefiCareMode = LightSensorOff;
#endif
    UserPrefECOMode = ECO_Standard;
    UserPrefBFSize = 1;
    SetECO();

#if ENABLE_HDMI
    msDlcOnOff( _DISABLE );
#endif

    if( DemoFlag )
    {
        Clr_DemoFlag();
        if( BlueFlag || SkinFlag || GreenFlag || ColorAutoDetectFlag || FullEnhanceFlag )
#if ENABLE_HDMI
            SetColorBoostWin( COLORBOOST_OFF );
#else
            SetColorBoostWindow( COLORBOOST_OFF );
#endif
        else
#if ENABLE_HDMI
            SetColorBoostWin( COLORBOOST_OFF );
#else
            SetColorBoostWindow( COLORBOOST_OFF );
#endif
    }
    else
    {
        Clr_PictureBoostFlag();
        Set_DemoFlag();
#if ENABLE_HDMI
        SetColorBoostWin( COLORBOOST_HALF );
#else
        SetColorBoostWindow( COLORBOOST_HALF );
#endif
    }
#if ENABLE_HDMI
    SetColorMatrix();
    SetColorSpace();
#else
    SetColorEnhance( 1 );
#endif
#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( ColorFlag ), UserPrefColorFlags );
#else
    UserPref_EnableFlashSaveBit( bFlashSaveMonitorBit );
#endif
    //Benz 2007.4.24   9:50:44      Osd_Draw4Num(2,1,UserPrefColorFlags);
    return TRUE;
}
//-----------------------------------------------------

#if 1  //2006-08-29 Andy
//Bool SetBFSize(void)
Bool SetBFSize( BYTE ucSize )
{
#if ENABLE_HDMI
    BYTE XDATA ucDomain;
#endif

#if ENABLE_HDMI
    ucDomain = 0x00;
    if( !IsColorspaceRGBInput()  )
        ucDomain = 0xFF;
#endif

#if 1
    if( !PictureBoostFlag )
    {
#if !ENABLE_HDMI
        msAccOnOff( 0 );
        msDlcOnOff( 0 );
#endif
        UserPrefBFHSize = 0;
        UserPrefBFVSize = 0;
        UserPrefBFHStart = 0;
        UserPrefBFVStart = 0;
        UserPrefBFSize = ucSize = 1; //Benz 2007.4.14   14:34:23
    }
    else
    {
#if !ENABLE_HDMI
        msAccOnOff( 1 );
#endif
        if( ucSize == 1 ) // H:5 V:5
        {
            UserPrefBFHSize = ( DISPLAY_WIDTH + 4 ) / 8; // / 5;
            UserPrefBFVSize = g_sPnlInfo.sPnlTiming.u16Height / 8; // / 5;
        }
        else if( ucSize == 2 )         // H:5 V:3
        {
            UserPrefBFHSize = DISPLAY_WIDTH / 5;
            UserPrefBFVSize = g_sPnlInfo.sPnlTiming.u16Height / 5; // / 3;
        }
        else if( ucSize == 3 )    //  H4 V:4
        {
            UserPrefBFHSize = DISPLAY_WIDTH / 4;
            UserPrefBFVSize = g_sPnlInfo.sPnlTiming.u16Height / 4;
        }
        else if( ucSize == 4 )      //  H3 V:3
        {
            UserPrefBFHSize = DISPLAY_WIDTH / 3;
            UserPrefBFVSize = g_sPnlInfo.sPnlTiming.u16Height / 3;
        }
        else if( ucSize == 5 )       //  H2 V:2
        {
            UserPrefBFHSize = DISPLAY_WIDTH / 2;
            UserPrefBFVSize = g_sPnlInfo.sPnlTiming.u16Height / 2;
        }
        else if( ucSize == 6 )       //  H2 V:1
        {
            UserPrefBFHSize = DISPLAY_WIDTH / 2;
            UserPrefBFVSize = g_sPnlInfo.sPnlTiming.u16Height;
        }
        else if( ucSize == 7 )      //  Full
        {
            UserPrefBFHSize = DISPLAY_WIDTH;
            UserPrefBFVSize = g_sPnlInfo.sPnlTiming.u16Height;
        }
        UserPrefBFHStart = 0;
        UserPrefBFVStart = 0;
        mStar_AdjustContrast(UserPrefSubContrast );//msAdjustVideoContrast( MAIN_WINDOW, UserPrefSubContrast );
        msAccSetup( UserPrefBFHStart + HSTART_OFFSET, UserPrefBFHSize + HSTART_OFFSET, UserPrefBFVStart, UserPrefBFVSize );
#if ENABLE_HDMI
        msAdjustSubBrightness( MAIN_WINDOW, UserPrefSubBrightness, UserPrefSubBrightness, UserPrefSubBrightness );
#else
        msAccOnOff( 1 );
#endif
    }
#endif

#if ENABLE_HDMI
    if( PictureBoostFlag )
    {
#if ENABLE_HDMI
        if( ucDomain )
        {
            msWriteByteMask(SC00_5C, BIT3, BIT3 ); //old_msWriteByte( BK0_5C, old_msReadByte( BK0_5C ) | BIT3 );
        }
        else
#endif
            msAccOnOff( 1 );
    }
    else
    {
#if ENABLE_HDMI
        if( ucDomain )
            msWriteByteMask(SC00_5C, BIT3, BIT3 );//old_msWriteByte( BK0_5C, old_msReadByte( BK0_5C )&~BIT3 );
        else
#endif
            msAccOnOff( 0 );
#if ENABLE_HDMI
        if( ucDomain )
            msWriteByteMask(SC00_5C, BIT3, BIT3 );//old_msWriteByte( BK0_02, old_msReadByte( BK0_02 ) | BIT3 );
#endif
    }
#endif
    return TRUE;
}

#if ENABLE_HDMI
Bool AdjustBrightFrameSize( BYTE ucSize )
{
    if( ucSize == 1 )         // H:5 V:5
    {
        UserPrefBFHSize = DISPLAY_WIDTH / 8; // / 5;
        UserPrefBFVSize = g_sPnlInfo.sPnlTiming.u16Height / 8; // / 5;
    }
    else if( ucSize == 2 )  // H:5 V:3
    {
        UserPrefBFHSize = DISPLAY_WIDTH / 5;
        UserPrefBFVSize = g_sPnlInfo.sPnlTiming.u16Height / 5; // / 3;
    }
    else if( ucSize == 3 )  //  H4 V:4
    {
        UserPrefBFHSize = DISPLAY_WIDTH / 4;
        UserPrefBFVSize = g_sPnlInfo.sPnlTiming.u16Height / 4;
    }
    else if( ucSize == 4 )  //  H3 V:3
    {
        UserPrefBFHSize = DISPLAY_WIDTH / 3;
        UserPrefBFVSize = g_sPnlInfo.sPnlTiming.u16Height / 3;
    }
    else if( ucSize == 5 )  //  H2 V:2
    {
        UserPrefBFHSize = DISPLAY_WIDTH / 2;
        UserPrefBFVSize = g_sPnlInfo.sPnlTiming.u16Height / 2;
    }
    else if( ucSize == 6 )  //  H2 V:1
    {
        UserPrefBFHSize = DISPLAY_WIDTH / 2;
        UserPrefBFVSize = g_sPnlInfo.sPnlTiming.u16Height;
    }
    else if( ucSize == 7 )  //  Full
    {
        UserPrefBFHSize = DISPLAY_WIDTH;
        UserPrefBFVSize = g_sPnlInfo.sPnlTiming.u16Height;
    }

    UserPrefBFHStart = 0;
    UserPrefBFVStart = 0;
    //msAdjustVideoContrast( UserPrefSubContrast );
    msAccSetup( UserPrefBFHStart + HSTART_OFFSET, UserPrefBFHSize + HSTART_OFFSET, UserPrefBFVStart, UserPrefBFVSize );
    //msAccOnOff( 1 );

    return TRUE;
}
#endif
//---------------------------------------------------------------
Bool AdjustBFSize( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefBFSize, 1, 7, 1 );
    if( tempValue == UserPrefBFSize )
    {
        return FALSE;
    }
    UserPrefBFSize = tempValue;
    //printData("UserPrefBFSize=%d",UserPrefBFSize);
#if ENABLE_HDMI
    AdjustBrightFrameSize( UserPrefBFSize ); //
#else
    SetBFSize( UserPrefBFSize ); //SetBFSize();
#endif
    return TRUE;
}
WORD GetBFSizeValue( void )
{
    return GetScale100Value( UserPrefBFSize, 0, 7 );
}

//---------------------------------------------------------------
Bool AdjustBFHstart( MenuItemActionType action )
{
    WORD tempValue;
    if( UserPrefBFSize == 1 )
    {
        tempValue = DecIncValue( action, UserPrefBFHStart, 0, 7, 1 );
    }
    else if( UserPrefBFSize == 2 )
    {
        tempValue = DecIncValue( action, UserPrefBFHStart, 0, 4, 1 );
    }
    else if( UserPrefBFSize == 3 )
    {
        tempValue = DecIncValue( action, UserPrefBFHStart, 0, 3, 1 );
    }
    else if( UserPrefBFSize == 4 )
    {
        tempValue = DecIncValue( action, UserPrefBFHStart, 0, 2, 1 );
    }
    else if( UserPrefBFSize == 5 || UserPrefBFSize == 6 )
    {
        tempValue = DecIncValue( action, UserPrefBFHStart, 0, 1, 1 );
    }
    else
    {
        tempValue = UserPrefHStart;
    }
    if( tempValue == UserPrefHStart )
    {
        return FALSE;
    }
    //printData("tempValue11=%d",tempValue);

    UserPrefBFHStart = tempValue;
    tempValue = UserPrefBFHStart * UserPrefBFHSize;
    msAccSetup( tempValue + HStartOffset, ( UserPrefBFHSize + tempValue ) + HStartOffset, UserPrefBFVStart * UserPrefBFVSize, ( UserPrefBFVStart * UserPrefBFVSize + UserPrefBFVSize ) );
    return TRUE;
}

WORD GetBFHstartValue( void )
{
    if( UserPrefBFSize == 2 )
    {
        return GetScale100Value( UserPrefBFHStart, 0, 4 );
    }
    else if( UserPrefBFSize == 3 )
    {
        return GetScale100Value( UserPrefBFHStart, 0, 3 );
    }
    else if( UserPrefBFSize == 4 )
    {
        return GetScale100Value( UserPrefBFHStart, 0, 2 );
    }
    else if( UserPrefBFSize == 5 || UserPrefBFSize == 6 )
    {
        return GetScale100Value( UserPrefBFHStart, 0, 1 );
    }
    else if( UserPrefBFSize == 1 )
    {
        return GetScale100Value( UserPrefBFHStart, 0, 7 );
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------
Bool AdjustBFVstart( MenuItemActionType action )
{
    WORD tempValue;
    if( UserPrefBFSize == 6 || UserPrefBFSize == 7 )
    {
        tempValue = 0;
    }
    else if( UserPrefBFSize == 1 )
    {
        tempValue = DecIncValue( action, UserPrefBFVStart, 0, 7, 1 );
    }
    else if( UserPrefBFSize == 2 )
    {
        tempValue = DecIncValue( action, UserPrefBFVStart, 0, 4, 1 );
    }
    else if( UserPrefBFSize == 4 )
    {
        tempValue = DecIncValue( action, UserPrefBFVStart, 0, 2, 1 );
    }
    else if( UserPrefBFSize == 3 )
    {
        tempValue = DecIncValue( action, UserPrefBFVStart, 0, 3, 1 );
    }
    else if( UserPrefBFSize == 5 )
    {
        tempValue = DecIncValue( action, UserPrefBFVStart, 0, 1, 1 );
    }
    /*
    if ( tempValue == UserPrefVStart )
    {
    return FALSE;
    }
     */
    UserPrefBFVStart = tempValue;
    tempValue = UserPrefBFVStart * UserPrefBFVSize;
    msAccSetup( UserPrefBFHStart * UserPrefBFHSize + HStartOffset, UserPrefBFHStart * UserPrefBFHSize + UserPrefBFHSize + HStartOffset, tempValue, ( tempValue + UserPrefBFVSize ) );
    return TRUE;
}
WORD GetBFVstartValue( void )
{
    if( UserPrefBFSize == 1 )
    {
        return GetScale100Value( UserPrefBFVStart, 0, 7 );
    }
    else if( UserPrefBFSize == 2 )
    {
        return GetScale100Value( UserPrefBFVStart, 0, 4 );
    }
    else if( UserPrefBFSize == 4 )
    {
        return GetScale100Value( UserPrefBFVStart, 0, 2 );
    }
    else if( UserPrefBFSize == 3 )
    {
        return GetScale100Value( UserPrefBFVStart, 0, 3 );
    }
    else if( UserPrefBFSize == 5 )
    {
        return GetScale100Value( UserPrefBFVStart, 0, 1 );
    }
    else
    {
        return 0;
    }
}
//================================================================================
Bool AdjustBF_Switch( MenuItemActionType action )
{
    action = action;
    Clr_ColorAutoDetectFlag();
    Clr_FullEnhanceFlag();
    Clr_GreenFlag();
    Clr_BlueFlag();
    Clr_SkinFlag();
    Clr_DemoFlag();
    //if (UserPrefDcrMode)
    //SetECO();
    UserPrefDcrMode = 0;
#if 0//Enable_Lightsensor
    UserPrefiCareMode = LightSensorOff;
#endif
    UserPrefECOMode = ECO_Standard;
    SetECO();

#if ENABLE_HDMI
    msDlcOnOff( _DISABLE );
#endif

    if( PictureBoostFlag )
    {
        Clr_PictureBoostFlag();
#if ENABLE_HDMI
        UserPrefBFSize = 1 ;    // recall default setting=> BFSize=1, position(0,0)
#endif
        SetBFSize( UserPrefBFSize ); //SetBFSize();
    }
    else
    {
#if !ENABLE_HDMI
        //msAdjustSubBrightness( UserPrefSubBrightness );
        //LoadACETable( t_Normal_ColorSettingTable, UserPrefHue, UserPrefSaturation, UserPrefSubContrast );
        msAdjustSubBrightness( MAIN_WINDOW, UserPrefSubBrightness, UserPrefSubBrightness, UserPrefSubBrightness );
        LoadACETable( MAIN_WINDOW, (BYTE *)t_Normal_ColorSettingTable, UserPrefHue, UserPrefSaturation, UserPrefSubContrast );
#endif
        Set_PictureBoostFlag();
#if !ENABLE_HDMI
        UserPrefBFSize = 1; // recall default setting=> BFSize=1, position(0,0)
#endif
        SetBFSize( UserPrefBFSize ); //SetBFSize();
#if !ENABLE_HDMI
        SetColorEnhance( 1 );
#endif
    }
#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( ColorFlag ), UserPrefColorFlags );
#else
    UserPref_EnableFlashSaveBit( bFlashSaveMonitorBit );
#endif
    //Benz 2007.4.24   9:50:48  Osd_Draw4Num(1,1,UserPrefColorFlags);
    return TRUE;
}
//================================================================================
Bool AdjustSubContrast( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefSubContrast, MinSubContrast, MaxSubContrast, 2 );
    if( tempValue == UserPrefSubContrast )
    {
        return FALSE;
    }
    UserPrefSubContrast = tempValue;
    mStar_AdjustContrast(UserPrefSubContrast );//msAdjustVideoContrast( MAIN_WINDOW, UserPrefSubContrast );
    //NVRam_WriteByte( nvrMonitorAddr( SubContrast ), UserPrefSubContrast);
    return TRUE;
}
WORD GetSubContrast100Value( void )
{
    return GetScale100Value( UserPrefSubContrast, MinSubContrast, MaxSubContrast );
}
//================================================================================
Bool AdjustSubBrightness( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefSubBrightness, MinSubBrightness, MaxSubBrightness, 2 );
    if( tempValue == UserPrefSubBrightness )
    {
        return FALSE;
    }
    UserPrefSubBrightness = tempValue;
    msAdjustSubBrightness( MAIN_WINDOW, UserPrefSubBrightness, UserPrefSubBrightness, UserPrefSubBrightness );
    return TRUE;
}
WORD GetSubBrightness100Value( void )
{
    return GetScale100Value( UserPrefSubBrightness, MinSubBrightness, MaxSubBrightness );
}
#endif
#endif
//-----------------------------------------------------
Bool ResetOsdTimer( void )
{
    if( !FactoryModeFlag )
    {
        OsdCounter = UserPrefOsdTime;
    }
    if( MenuPageIndex == MainMenu && !FactoryModeFlag )
    {
        UserPrefLastMenuIndex = MenuItemIndex;
    }
    //OsdCounter=0;
    return TRUE;
}
Bool EnablePowerDownCounter( void )
{
    PowerDownCounter = 3;

    return TRUE;
}

Bool PowerOffSystem( void )
{
    Clr_PowerOnFlag();

    Power_TurnOffLed();

#if ENABLE_HDMI
    //mstar_HDMIAudioMute( 0 );
    hw_SetMute();
#endif


    Clr_FactoryModeFlag();
    Clr_DisplayLogoFlag();
    Clr_DoModeSettingFlag();
    Clr_DoBurninModeFlag(); //2009-10-13  add
    //old_msWriteByte( BK0_32, 0 ); // disable background color function.
    drvOSD_FrameColorEnable(FALSE);
    Clr_PowerSavingFlag(); // 091014

#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( MonitorFlag ), MonitorFlags );
#else
#if (MS_PM)
    SaveMonitorSetting();   //110921 Rick modified - A032
#else
    Set_FlashForceSaveMonitorFlag();
#endif
#endif

#if 0//ENABLE_HDCP//1
    msPullLowHPD2();
#endif


    Power_PowerOffSystem();

    Clr_PowerSavingFlag();

#if (MS_PM)
    msPM_SetFlag_PMDCoff();

#if (ENABLE_DP_INPUT == 0x1)
	if(sPMInfo.ucPMMode == ePM_POWEROFF)
	{
	    #if (ENABLE_DP_DCOFF_HPD_HIGH == 0x0)
		msAPI_combo_DP_InitRxHPD(FALSE);  //Set DP Rx HPD as Low

		#if ENABLE_USB_TYPEC
		if(Input_UsbTypeC_C3 != Input_Nothing)
		{
			drvmbx_send_HPD_Ctrl_CMD(0, 1);  //(u8TypeCPortIdx = 0, for port A)
		}

		if(Input_UsbTypeC_C4 != Input_Nothing)
		{
			drvmbx_send_HPD_Ctrl_CMD(0, 1);  //(u8TypeCPortIdx = 1, for port B)
		}
		#endif
        #endif
	}
#endif

#if (ENABLE_DP_OUTPUT == 0x1)
	if(sPMInfo.ucPMMode == ePM_POWEROFF)
	{
	       mapi_DPTx_MSCHIP_SetPowerSavingMode(sPMInfo.ucPMMode, TRUE);
		mapi_DPTx_TurnOnOff(0, FALSE);
	}
#endif
#endif

    return TRUE;
}

Bool PowerOnSystem( void )
{
    BootTimeStamp_Set(POWER_ON_TS, 0, TRUE); // power on timestamp 0
    Set_PowerOnFlag();
    Clr_InputTimingChangeFlag();

#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( MonitorFlag ), MonitorFlags );
#else
    #if (MS_PM)
    SaveMonitorSetting();
    #else
    Set_FlashForceSaveMonitorFlag();
    #endif
#endif

    BootTimeStamp_Set(POWER_ON_TS, 1, TRUE); // power on timestamp 1
    Power_TurnOnGreenLed();
    Power_PowerOnSystem();
    BootTimeStamp_Set(POWER_ON_TS, 6, TRUE); // power on timestamp 6
    Clr_InputTimingChangeFlag();
    Clr_BurninModeFlag();    //110928 Rick add condition for clear BurninModeFlag while DC On
    Clr_DoBurninModeFlag();
    if( ProductModeFlag || FactoryModeFlag ) //DC On Reset For PE Request
    {
        ResetAllSetting();
        Clr_FactoryModeFlag();
    }
    BootTimeStamp_Set(POWER_ON_TS, 7, TRUE); // power on timestamp 7
    mStar_SetupInputPort();
    BootTimeStamp_Set(POWER_ON_TS, 8, TRUE); // power on timestamp 8
#if DisplayPowerOnLogo
    //if( !ProductModeFlag )
    {
        if(UserPrefDisplayLogoEn)
        {
            Set_DisplayLogoFlag();
        }
        old_msWriteByte( IOSDC2, 0x02 ); //Disable color bit setting
    }
#endif
    mStar_SetupFreeRunMode();
    Set_InputTimingChangeFlag();

#if (ENABLE_DP_OUTPUT == 0x1)
	mapi_DPTx_TurnOnOff(0, TRUE);
#endif
    BootTimeStamp_Set(POWER_ON_TS, 9, TRUE); // power on timestamp 9

    return TRUE;
}
Bool EnterRootMenu( void )
{
    OsdCounter = 0;
    return TRUE;
}
//================================================================================
//================================================================================
//================================================================================
#if ENABLE_DPS
BYTE GetSubWndRealContrast(BYTE ucContrast)
{
#if MWEFunction
    return (UserPrefSubContrast+ucContrast-DefContrast);
#else
    return (UserPrefContrast+ucContrast-DefContrast);
#endif
}
#endif
Bool AdjustContrast( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefContrast, MinContrastValue, MaxContrastValue, 1 );
    if( tempValue == UserPrefContrast )
    {
        return FALSE;
    }
    UserPrefContrast = tempValue;
    mStar_AdjustContrast( UserPrefContrast );
    if( UserPrefColorTemp == CTEMP_USER )
    {
        UserPrefContrastUser = UserPrefContrast;
    }
    else if( UserPrefColorTemp == CTEMP_Cool1 )
    {
        UserPrefContrastCool1 = UserPrefContrast;
        if( FactoryModeFlag )
            FUserPrefContrastCool1 = UserPrefContrast;
    }
    else if( UserPrefColorTemp == CTEMP_Warm1 )
    {
        UserPrefContrastWarm1 = UserPrefContrast;
        if( FactoryModeFlag )
            FUserPrefContrastWarm1 = UserPrefContrast;
    }
    else if( UserPrefColorTemp == CTEMP_Normal )
    {
        UserPrefContrastNormal = UserPrefContrast;
        if( FactoryModeFlag )
            FUserPrefContrastNormal = UserPrefContrast;
    }
    else if( UserPrefColorTemp == CTEMP_SRGB )
    {
        UserPrefContrastSRGB = UserPrefContrast;
    }
#if ENABLE_DPS
    if(!REG_DPS_RUN_Flag)
    {
            mStar_AdjustContrast(tempValue);
        }
#endif

    return TRUE;
}

WORD GetContrastValue( void )
{
    //return GetScale100Value(UserPrefContrast, MinContrastValue, MaxContrastValue);
    WORD temp;
    temp = GetScale100Value( UserPrefContrast, MinContrastValue, MaxContrastValue );
    return temp;
}
//====================================================
Bool AdjustBrightness( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefBrightness, MinBrightnessValue, MaxBrightnessValue, 1 );
    if( tempValue == UserPrefBrightness )
    {
        return FALSE;
    }
    UserPrefBrightness = tempValue;
    mStar_AdjustBrightness( UserPrefBrightness );
#if 0
    if( UserPrefBrightness > 50 )
        mStar_AdjustBrightness( UserPrefBrightness );
    else
    {
        mStar_AdjustBrightness( 0 );
        mStar_AdjustBlackLevel( UserPrefBrightness );
    }
#endif
    if( UserPrefColorTemp == CTEMP_USER )
        UserPrefBrightnessUser = UserPrefBrightness;
    else if( UserPrefColorTemp == CTEMP_Cool1 )
    {
        UserPrefBrightnessCool1 = UserPrefBrightness;
        if( FactoryModeFlag )
            FUserPrefBrightnessCool1 = UserPrefBrightness;
    }
    else if( UserPrefColorTemp == CTEMP_Warm1 )
    {
        UserPrefBrightnessWarm1 = UserPrefBrightness;
        if( FactoryModeFlag )
            FUserPrefBrightnessWarm1 = UserPrefBrightness;
    }
    else if( UserPrefColorTemp == CTEMP_Normal )
    {
        UserPrefBrightnessNormal = UserPrefBrightness;
        if( FactoryModeFlag )
            FUserPrefBrightnessNormal = UserPrefBrightness;
    }
    else if( UserPrefColorTemp == CTEMP_SRGB )
    {
        UserPrefBrightnessSRGB = UserPrefBrightness;
    }

#if ENABLE_DPS
if(!REG_DPS_RUN_Flag)
    mStar_AdjustBrightness(tempValue);
#endif
#if ENABLE_DPS
#if DPS_UserBrightControl_EN
    REG_DPS_UserBrightContlGain = UserPrefBrightness;//(float)USER_PREF_BRIGHTNESS * 256 / 100;
#else
    REG_DPS_UserBrightContlGain = 100;
#endif
#endif
    return TRUE;
}

WORD GetBrightnessValue( void )
{
    return UserPrefBrightness; //GetScale100Value(UserPrefBrightness, 0, 100);
}
//=============================================================================
Bool AdjustFocus( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefPhase, 0, MAX_PHASE_VALUE, 1 );
    if( tempValue == UserPrefPhase )
    {
        return FALSE;
    }
    UserPrefPhase = tempValue;
    //msADC_AdjustPhase( UserPrefPhase );
 #if ENABLE_VGA_INPUT
    drvADC_SetPhaseCode(UserPrefPhase);
 #endif
    return TRUE;
}

WORD GetFocusValue( void )
{
    return (!CURRENT_INPUT_IS_VGA()) ? 50 : GetScale100Value( UserPrefPhase, 0, MAX_PHASE_VALUE );
}

//====================================================
#define MaxClock    (StandardModeHTotal+100)
#define MinClock    (StandardModeHTotal-100)
Bool AdjustClock( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefHTotal, MinClock, MaxClock, 1 );
    if( tempValue == UserPrefHTotal )
    {
        return FALSE;
    }
    UserPrefHTotal = tempValue;
    //msADC_AdjustHTotal( UserPrefHTotal );
    mStar_WaitForDataBlanking();            //110930 Rick add for avoid noise while adjust clock value - C_FOS_5
#if ENABLE_VGA_INPUT
    drvADC_AdjustHTotal(UserPrefHTotal);
#endif
    return TRUE;
}

WORD GetClockValue( void ) // adjust range
{
    return (!CURRENT_INPUT_IS_VGA()) ? 50 : GetScale100Value( UserPrefHTotal, MinClock, MaxClock );
}
//#define MaxHStart   UserPrefAutoHStart+50
//#define MinHStart   UserPrefAutoHStart-50
//====================================================
Bool AdjustHPosition( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefHStart, MinHStart, MaxHStart, 1 );
    if( tempValue == UserPrefHStart )
    {
        return FALSE;
    }
    UserPrefHStart = tempValue;
    mStar_AdjustHPosition( UserPrefHStart );
    return TRUE;
}

WORD GetHPositionValue( void )
{
    return (!CURRENT_INPUT_IS_VGA()) ? 50 : 100 - GetScale100Value( UserPrefHStart, MinHStart, MaxHStart ); //, 0);
}
//====================================================
//#define MaxVStart 2*UserPrefAutoVStart-1
//#define MinVStart 1  // 2006/10/26 4:18PM by Emily  0
//====================================================
Bool AdjustVPosition( MenuItemActionType action )
{
    WORD tempValue;
        tempValue = DecIncValue( action, UserPrefVStart, MinVStart, MaxVStart, 1 );
    if( tempValue == UserPrefVStart )
    {
        return FALSE;
    }
    UserPrefVStart = tempValue;
    mStar_AdjustVPosition( UserPrefVStart );
    return TRUE;
}

WORD GetVPositionValue( void )
{
        return (!CURRENT_INPUT_IS_VGA()) ? 50 : GetScale100Value( UserPrefVStart, MinVStart, MaxVStart );
}
//====================================================
#if AudioFunc
Bool AdjustVolume( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefVolume, MinVolume, MaxVolume, 1 );
    if( tempValue == UserPrefVolume )
    {
        return FALSE;
    }
    UserPrefVolume = tempValue;
    msAPI_AdjustVolume( UserPrefVolume );
    return TRUE;
}
WORD GetVolumeValue( void )
{
    return GetScale100Value( UserPrefVolume, MinVolume, MaxVolume );
}

Bool SetAudioSource(void)
{
    msAPI_AdjustVolume( 0 );

    if (UserPrefAudioSource == MenuItemIndex)
        return FALSE;
    UserPrefAudioSource = MenuItemIndex;

    if (UserPrefAudioSource == AudioSourceMenuItems_Digital)
    {
    #if ENABLE_DAC
        #if ENABLE_HDMI
            if( CURRENT_INPUT_IS_HDMI())
            {
                msDrvAudioSourceSel( AUDIO_DIGITAL );
            }
        #endif
        #if ENABLE_DP_INPUT
            if(CURRENT_INPUT_IS_DISPLAYPORT())
            {
                msDrvAudioSourceSel( AUDIO_DIGITAL );
            }
        #endif
    #endif
    }
    else
    {
    #if ENABLE_DAC
        msDrvAudioSourceSel( AUDIO_LINE_IN );
    #endif
    }

    msAPI_AdjustVolume( UserPrefVolume );
    Set_SaveMonitorSettingFlag();
    return TRUE;
}

#endif
//=====================================================
Bool AdjustOSDHPosition( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefOsdHStart, 0, 100, 5 ); //0729 New Spec
    if( tempValue == UserPrefOsdHStart )
    {
        return FALSE;
    }
    UserPrefOsdHStart = tempValue;
    Osd_SetPosition( UserPrefOsdHStart, UserPrefOsdVStart );
    return TRUE;
}
WORD GetOSDHPositionValue( void )
{
    return GetScale100Value( UserPrefOsdHStart, 0, 100 );
}
//================================================================================
Bool AdjustOSDVPosition( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefOsdVStart, 0, 100, 5 ); //0729 New Spec
    if( tempValue == UserPrefOsdVStart )
    {
        return FALSE;
    }
    UserPrefOsdVStart = tempValue;
    Osd_SetPosition( UserPrefOsdHStart, UserPrefOsdVStart );
    return TRUE;
}
WORD GetOSDVPositionValue( void )
{
    return GetScale100Value( UserPrefOsdVStart, 0, 100 );
}
//================================================================================
Bool AdjustOSDTime( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefOsdTime, OSD_TIME_MIN, OSD_TIME_MAX, 2 );
    if( tempValue == UserPrefOsdTime )
    {
        return FALSE;
    }
    UserPrefOsdTime = tempValue;
    return TRUE;
}
WORD GetOSDTime100Value( void )
{
    return UserPrefOsdTime;
}
//================================================================================
Bool AdjustOSDTransparency( MenuItemActionType action )
{
    WORD tempValue;

    tempValue=DecIncValue( action, UserPrefOsdTransparency, 0, MAX_OSD_TRANSPARENCY, 1 );

    if (tempValue==UserPrefOsdTransparency)
        return FALSE;
    UserPrefOsdTransparency=tempValue;

    Osd_SetTransparency(UserPrefOsdTransparency);

    return TRUE;
}
WORD GetOSDTransparencyValue( void )
{
    return GetScale100Value(UserPrefOsdTransparency,0,MAX_OSD_TRANSPARENCY);
}

//====================================================
Bool AdjustBlueColor( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefBlueColor, MinColorValue, MaxColorValue, (( UserPrefColorTemp == CTEMP_USER ) ? 2 : 1 ) );
    //tempValue = DecIncValue(action, UserPrefBlueColorUser, 0, 100, 1);
    if( tempValue == UserPrefBlueColor )
    {
        return FALSE;
    }
    UserPrefBlueColor = tempValue;
    UserPrefBlueColorUser = UserPrefBlueColor;
#if 0//UsesRGB
    mStar_AdjustContrast( UserPrefContrast );
#else
    msAPI_AdjustRGBColor(MAIN_WINDOW, UserPrefContrast, UserPrefRedColorUser, UserPrefGreenColorUser, UserPrefBlueColorUser);
#endif
    return TRUE;
}

WORD GetBlueColorValue( void )
{
    return GetScale100Value( UserPrefBlueColor, MinColorValue, MaxColorValue );
}
//====================================================
Bool AdjustGreenColor( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefGreenColor, MinColorValue, MaxColorValue, (( UserPrefColorTemp == CTEMP_USER ) ? 2 : 1 ) );
    if( tempValue == UserPrefGreenColor )
    {
        return FALSE;
    }
    UserPrefGreenColor = tempValue;
    UserPrefGreenColorUser = UserPrefGreenColor;
#if 0//UsesRGB
    mStar_AdjustContrast( UserPrefContrast );
#else
    msAPI_AdjustRGBColor(MAIN_WINDOW, UserPrefContrast, UserPrefRedColorUser, UserPrefGreenColorUser, UserPrefBlueColorUser);
#endif
    return TRUE;
}
WORD GetGreenColorValue( void )
{
    return GetScale100Value( UserPrefGreenColor, MinColorValue, MaxColorValue );
}
//====================================================
Bool AdjustRedColor( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefRedColor, MinColorValue, MaxColorValue, (( UserPrefColorTemp == CTEMP_USER ) ? 2 : 1 ) );
	if( tempValue == UserPrefRedColor )
    {
        return FALSE;
    }
    UserPrefRedColor = tempValue;
    UserPrefRedColorUser = UserPrefRedColor;
#if 0//UsesRGB
    mStar_AdjustContrast( UserPrefContrast );
#else
    //msAPI_AdjustRGBColor(MAIN_WINDOW, UserPrefContrast, UserPrefRedColorUser, UserPrefGreenColorUser, UserPrefBlueColorUser);
	mStar_AdjustRedColor( UserPrefRedColorUser, UserPrefContrast );
#endif
    return TRUE;
}
WORD GetRedColorValue( void )
{
    return GetScale100Value( UserPrefRedColor, MinColorValue, MaxColorValue );
}

Bool SetColorFormat(void)
{
    if (UserPrefInputColorFormat == MenuItemIndex)
        return FALSE;
    UserPrefInputColorFormat = MenuItemIndex;
    SetInputColorFormat();
    Set_SaveMonitorSettingFlag();
    return TRUE;
}

//====================================================
Bool SetColorTemp( void )
{
    ReadColorTempSetting();
    if( UserPrefColorTemp == CTEMP_SRGB )
    {
        UserPrefECOMode = ECO_Standard;
    }
    return TRUE;
}
Bool AdjustColorTempMode( MenuItemActionType action )
{
    BYTE temp;
    temp = UserPrefColorTemp;
    if( action == MIA_DecValue )
    {
        UserPrefColorTemp = ( UserPrefColorTemp + CTEMP_Nums - 1 ) % CTEMP_Nums;
    }
    else
    {
        UserPrefColorTemp = ( UserPrefColorTemp + 1 ) % CTEMP_Nums;
    }

    if( UserPrefDcrMode )
    {
        UserPrefDcrMode = 0;
        #if !ENABLE_HDMI
        #if MWEFunction
        if( UserPrefColorFlags )
            msAccOnOff( 1 );
        else
        #endif
            msAccOnOff( 0 );
        #endif
#if ENABLE_DLC
        msDlcOnOff( 0 );
#endif
    }

    if( temp == UserPrefColorTemp )
        return FALSE;

    #if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( ColorTemp ), UserPrefColorTemp );
    #else
    //SaveMonitorSetting();
    Set_FlashSaveMonitorFlag();
    #endif
    //msAccOnOff(0); // 2006/11/9 11:27PM by Emily test


    ReadColorTempSetting();
    //if( UserPrefECOMode != ECO_Standard )        //jeff add in 1112
        //UserPrefECOMode = ECO_Standard;
    return TRUE;
}
Bool AdjustLanguage( MenuItemActionType action )
{
    BYTE temp;
    temp = UserPrefLanguage;
    if( action == MIA_DecValue )
    {
        UserPrefLanguage = ( UserPrefLanguage + LANG_Nums - 1 ) % LANG_Nums;
    }
    else
    {
        UserPrefLanguage = ( UserPrefLanguage + 1 ) % LANG_Nums;
    }
    if( temp == UserPrefLanguage )
        return FALSE;

#if LANGUAGE_TYPE == ASIA
    if( UserPrefLanguage == LANG_TChina )
    {
        LanguageIndex = 1;
        SecondTblAddr = 0x80;
    }
    else if( UserPrefLanguage == LANG_Korea )
    {
        LanguageIndex = 2;
        SecondTblAddr = 0x80; //SecondTblAddr = 0xFF;
    }
    else
    {
        LanguageIndex = 0;
        SecondTblAddr = 0xFF;
    }
#elif LANGUAGE_TYPE == SIXTEEN                   //090330 xiandi.yu
    if( UserPrefLanguage == LANG_SChina )
    {
        LanguageIndex = 1;
        SecondTblAddr = 0x80;
    }
    else if( UserPrefLanguage == LANG_Japanese )
    {
        LanguageIndex = 2;
        SecondTblAddr = 0x80; //SecondTblAddr = 0xFF;
    }
    else
    {
        LanguageIndex = 0;
        SecondTblAddr = 0xFF;
    }
#endif
    //Set_LoadFontFlag();
    return TRUE;
}

Bool ChangeSource( void )
{
    if (UserPrefInputPriorityType == MenuItemIndex)
    {
        PrevMenuItemIndex = MenuItemIndex;
        return FALSE;
    }
    UserPrefInputPriorityType = MenuItemIndex;

    if( UserPrefInputPriorityType == Input_Priority_Auto )
    {
//        SrcInputType=SrcInputType;
        UserPrefInputType=SrcInputType;
    }
#if (INPUT_TYPE & INPUT_1A)
    else if( UserPrefInputPriorityType == Input_Priority_Analog1 )
    {
        SrcInputType=Input_Analog1;
        UserPrefInputType=SrcInputType;
    }
#endif
#if (INPUT_TYPE >= INPUT_1C)
    else if ( UserPrefInputPriorityType == Input_Priority_Digital1 )
    {
        SrcInputType=Input_Digital;
        UserPrefInputType=SrcInputType;
    }
#endif
#if (INPUT_TYPE >= INPUT_2C)
    else if ( UserPrefInputPriorityType == Input_Priority_Digital2 )
    {
        SrcInputType=Input_Digital2;
        UserPrefInputType=SrcInputType;
    }
#endif
#if (INPUT_TYPE >= INPUT_3C)
    else if ( UserPrefInputPriorityType == Input_Priority_Digital3 )
    {
        SrcInputType=Input_Digital3;
        UserPrefInputType=SrcInputType;
    }
#endif
#if (INPUT_TYPE >= INPUT_4C)
    else if ( UserPrefInputPriorityType == Input_Priority_Digital4 )
    {
        SrcInputType=Input_Digital4;
        UserPrefInputType=SrcInputType;
    }
#endif

    Set_SaveMonitorSettingFlag();
#if Enable_PanelHandler
    Power_PanelCtrlOnOff(FALSE, TRUE);
#else
    Power_TurnOffPanel();
#endif

    mStar_SetupInputPort();
    Set_InputTimingChangeFlag();
    //SrcFlags |= SyncLoss;
    //if( !ProductModeFlag )
    //    Set_ShowInputInfoFlag();
    return TRUE;
}

void DDC_ChangeSource( void )
{
    if(UserPrefInputPriorityType == (UserPrefInputType+1))
        return;
    UserPrefInputPriorityType = UserPrefInputType+1;
    SrcInputType=UserPrefInputType;
#if Enable_PanelHandler
    Power_PanelCtrlOnOff(FALSE, TRUE);
#else
    Power_TurnOffPanel();
#endif

    mStar_SetupInputPort();
    Set_InputTimingChangeFlag();
}

Bool ChangeDPVersion( void )
{
	#if ENABLE_DP_INPUT
    BYTE ucVersion = 0x00, DP_port;

    if (UserPrefDPVersion == MenuItemIndex)
        return FALSE;
    UserPrefDPVersion = MenuItemIndex;
    MenuFunc_printData("UserPrefDPVersion = %d\n", UserPrefDPVersion);

    if (UserPrefDPVersion == DPVersionMenuItems_DP1_1)
        ucVersion = DP_VERSION_11;
    else if (UserPrefDPVersion == DPVersionMenuItems_DP1_2)
        ucVersion = DP_VERSION_12;
    else if (UserPrefDPVersion == DPVersionMenuItems_DP1_4)
        ucVersion = DP_VERSION_14;

    if(CURRENT_INPUT_IS_DISPLAYPORT())
    {
#if Enable_PanelHandler
        Power_PanelCtrlOnOff(FALSE, TRUE);
#else
        Power_TurnOffPanel();
#endif
    }

    for(DP_port = Input_Digital; DP_port < Input_Nums; DP_port++)
    {
        if((ENABLED_DISPLAYPORT>>DP_port)&BIT0)
        {
        	msAPI_combo_IPControlHPD(DP_port,FALSE);
            mapi_DPRx_VersionSetting(DP_port, ucVersion);

			#if (ENABLE_DP_RX_MAX_LINKRATE_HBR2 == 0x1)
			if((ucVersion == DP_VERSION_14)||(ucVersion == DP_VERSION_12))
			{
				/*
				DP_LINKRATE_RBR = 0x6,
				DP_LINKRATE_HBR = 0xA,
				DP_LINKRATE_HBR2 = 0x14,
				DP_LINKRATE_HBR25 = 0x19,
				DP_LINKRATE_HBR3 = 0x1E
				*/
				mapi_DPRx_MaxLinkRate_Set(DP_port, 0x14);  //Note: Change Max link rate need after the API "mapi_DPRx_VersionSetting"
			}
			#endif

			CustomEDID_UpdateEDID(DP_port);

            MenuFunc_printData("DP_port : %d", DP_port);
            MenuFunc_printData("ucVersion : %d\n", ucVersion);
        }
    }

    ForceDelay1ms(650);

    for(DP_port = Input_Digital; DP_port < Input_Nums; DP_port++)
    {
        if((ENABLED_DISPLAYPORT>>DP_port)&BIT0)
        {
            msAPI_combo_IPControlHPD(DP_port,TRUE);
        }
    }

    Set_SaveMonitorSettingFlag();
	#endif //#if ENABLE_DP_INPUT
    return TRUE;
}

Bool ChangeMSTOnOff( void )
{
	#if ENABLE_DP_INPUT
    BYTE DP_port;

    if (UserPrefMSTOnOff== MenuItemIndex)
        return FALSE;
    UserPrefMSTOnOff = MenuItemIndex;

    if (CURRENT_INPUT_IS_DISPLAYPORT())
    {
        SetMSTOnOff();
    }
    else
    {
        if (!UserPrefMSTOnOff)
        {
            for(DP_port = Input_Digital; DP_port < Input_Nums; DP_port++)
            {
                if((ENABLED_DISPLAYPORT>>DP_port)&BIT0)
                {
                	msAPI_combo_IPControlHPD(DP_port,FALSE);
                }
            }

            ForceDelay1ms(650);

            for(DP_port = Input_Digital; DP_port < Input_Nums; DP_port++)
            {
                if((ENABLED_DISPLAYPORT>>DP_port)&BIT0)
                {
                    msAPI_combo_IPControlHPD(DP_port,TRUE);
                }
            }
        }
    }

    Set_SaveMonitorSettingFlag();
	#endif //#if ENABLE_DP_INPUT
    return TRUE;
}

Bool SetMSTOff( void )
{
	#if ENABLE_DP_INPUT
    BYTE DP_port;

    for(DP_port = Input_Digital; DP_port < Input_Nums; DP_port++)
    {
        if((ENABLED_DISPLAYPORT>>DP_port)&BIT0)
        {

        }
    }
	#endif //#if ENABLE_DP_INPUT
    return TRUE;
}

Bool SetMSTOnOff( void )
{
	#if ENABLE_DP_INPUT
    BYTE DP_port;

    for(DP_port = Input_Digital; DP_port < Input_Nums; DP_port++)
    {
        if((ENABLED_DISPLAYPORT>>DP_port)&BIT0)
        {
            if(SrcInputType == DP_port)
            {
            	msAPI_combo_IPControlHPD(DP_port,FALSE);

				MenuFunc_printData("DP_port : %d\n", DP_port);
                MenuFunc_printData("UserPrefMSTOnOff : %d\n", UserPrefMSTOnOff);
            }
            else
            {
            	msAPI_combo_IPControlHPD(DP_port,FALSE);
            }

        }
    }

	ForceDelay1ms(650);

    for(DP_port = Input_Digital; DP_port < Input_Nums; DP_port++)
    {
        if((ENABLED_DISPLAYPORT>>DP_port)&BIT0)
        {
            msAPI_combo_IPControlHPD(DP_port,TRUE);
        }
    }
	#endif //#if ENABLE_DP_INPUT

    return TRUE;
}
#if ENABLE_DP_INPUT
Bool SetDPVersion( BYTE DPVersion )
{
	#if ENABLE_DP_INPUT
    BYTE ucVersion = 0x00, DP_port;

    if (DPVersion == DPVersionMenuItems_DP1_1)
        ucVersion = DP_VERSION_11;
    else if (DPVersion == DPVersionMenuItems_DP1_2)
        ucVersion = DP_VERSION_12;
    else if (DPVersion == DPVersionMenuItems_DP1_4)
        ucVersion = DP_VERSION_14;

    
    for(DP_port = Input_Digital; DP_port < Input_Nums; DP_port++)
    {
        if((ENABLED_DISPLAYPORT>>DP_port)&BIT0)
        {
            mapi_DPRx_VersionSetting(DP_port, ucVersion);

			#if (ENABLE_DP_RX_MAX_LINKRATE_HBR2 == 0x1)
			if((ucVersion == DP_VERSION_14)||(ucVersion == DP_VERSION_12))
			{
				/*
				DP_LINKRATE_RBR = 0x6,
				DP_LINKRATE_HBR = 0xA,
				DP_LINKRATE_HBR2 = 0x14,
				DP_LINKRATE_HBR25 = 0x19,
				DP_LINKRATE_HBR3 = 0x1E
				*/
				mapi_DPRx_MaxLinkRate_Set(DP_port, 0x14);  //Note: Change Max link rate need after the API "mapi_DPRx_VersionSetting"
			}
			#endif
        }

        CustomEDID_UpdateEDID(DP_port);
    }
    
	#endif
    return TRUE;
}

void SetDPOfflinePortMSToff(BYTE InputType)
{
    MenuFunc_printData("DP_port: %d\n", InputType);
    msAPI_combo_IPControlHPD(InputType,FALSE);
    ForceDelay1ms(650);
    msAPI_combo_IPControlHPD(InputType,TRUE);
}
#endif

#if DDCCI_ENABLE
Bool AdjustDDCCI( MenuItemActionType action )
{
    action = action;
    if( DDCciFlag )
        Clr_DDCciFlag();
    else
        Set_DDCciFlag();
    return TRUE;
}
#endif


Bool ResetAllSetting( void )
{
#if !USEFLASH
    ModeInfoType xdata modeInfo;
#endif
    WORD hFreq, vFreq;
    hFreq = HFreq( SrcHPeriod );
    vFreq = VFreq( hFreq, SrcVTotal );

#if Enable_PanelHandler
    Power_PanelCtrlOnOff(FALSE, TRUE);
#else
    mStar_BlackWhiteScreenCtrl(BW_SCREEN_BLACK);//Power_TurnOffPanel();
#endif

    /*          //remove down
    if(ProductModeFlag)
    {
        Power_TurnOnAmberLed();
    }
    */
    // if (!FactoryModeFlag)
    {
        UserPrefOsdHStart = DEF_OSD_H_POSITION;
        UserPrefOsdVStart = DEF_OSD_V_POSITION;
    }
    UserPrefVolume = DEF_VOLUME; //0603 request by zhong
#if AudioFunc
    msAPI_AdjustVolume( UserPrefVolume );
#endif

    UserPrefRedColor = DefColorUser;
    UserPrefGreenColor = DefColorUser;
    UserPrefBlueColor = DefColorUser;
    UserPrefRedColorUser = DefColorUser;
    UserPrefGreenColorUser = DefColorUser;
    UserPrefBlueColorUser = DefColorUser;

#if DDCCI_ENABLE
    UserprefRedBlackLevel = 50;
    UserprefGreenBlackLevel = 50;
    UserprefBlueBlackLevel = 50;
#endif

    UserPrefLastMenuIndex = 0;
    UserPrefBrightnessUser = DefBrightness;
    UserPrefContrastUser = DefContrast;
    UserPrefBrightness = DefBrightness;
    UserPrefContrast = DefContrast;
    UserPrefBrightnessWarm1 = DefBrightness;//FUserPrefBrightnessWarm1;
    UserPrefContrastWarm1 = DefContrast; // FUserPrefContrastWarm1;
    UserPrefBrightnessNormal = DefBrightness;//FUserPrefBrightnessNormal;
    UserPrefContrastNormal = DefContrast;//FUserPrefContrastNormal;
    UserPrefBrightnessCool1 = DefBrightness; // FUserPrefBrightnessCool1;
    UserPrefContrastCool1 = DefContrast;//FUserPrefContrastCool1;
    UserPrefBrightnessUser = DefBrightness;
    UserPrefContrastUser = DefContrast;
    //UserPrefSavedModeIndex = NumberOfMode;
    UserPrefOsdTime = 10;
    UserPrefOsdTransparency = DEF_OSD_TRANSPARENCY;
    Osd_SetTransparency(UserPrefOsdTransparency);
	#if LiteMAX_Baron_OSD_TEST
	UserPrefColorTemp = CTEMP_Warm1;
	#else
    UserPrefColorTemp = CTEMP_USER;
	#endif
    SetColorTemp(); //9300K()
    UserPrefECOMode = ECO_Standard;
#if ENABLE_OSD_ROTATION
    UserPrefOsdRotateMode = OSDRotateMenuItems_Off;
#endif
#if ENABLE_SHARPNESS
    UserPrefSharpness= DefSharpness;
    msAdjustSharpness( MAIN_WINDOW, UserPrefSharpness, 0 );
#endif
#if Enable_Gamma
    UserPrefGamaMode = GAMA1;
    mStar_SetupGamma( UserPrefGamaMode );
#endif
#if ENABLE_DPS
    UserprefDPSMode = 0;
    msDPS_Off_Setting( );
#endif
#if ENABLE_DLC
    UserprefDLCMode = 0;
    g_bDLCOnOff = 0;
#endif

#if Enable_Gamma
    UserPrefGamaOnOff=FALSE;
    drvGammaOnOff(UserPrefGamaOnOff, MAIN_WINDOW );
#endif
#if ENABLE_RTE
    UserprefOverDriveSwitch = 1;
    UserprefOverDriveWeighting = DEF_OD_Weighting;
    msAPI_OverDriveEnable(UserprefOverDriveSwitch);
#endif

#if ENABLE_FREESYNC
    UserprefFreeSyncMode = FreeSyncMenuItems_Off;
    msAPI_combo_IPEnableDDRFlag(UserprefFreeSyncMode);
#endif

    UserPrefInputPriorityType=DEF_INPUT_PRIORITY;
#if ENABLE_DP_INPUT
    UserPrefMSTOnOff=0;
    SetMSTOff();
    UserPrefDPVersion=DPVersionMenuItems_DP1_2;
    SetDPVersion(UserPrefDPVersion);
#endif

#if ENABLE_SUPER_RESOLUTION
    UserPrefSuperResolutionMode = SRMODE_MIDDLE;
    mStar_SetupSuperResolution( UserPrefSuperResolutionMode );
#endif
#if ENABLE_DeBlocking
    UserPrefDeBlocking= 0;
    //msDPS_Off_Setting( );
#endif
    UserPrefColorMode = ColorModeMenuItems_Off;
    SetColorModeOFF();
    UserPrefInputColorFormat = INPUTCOLOR_AUTO;
    UserPrefInputColorRange = INPUTCOLOR_FULL;
    SetECO();

    Set_DDCciFlag();
    if( UserPrefDcrMode )
    {
        UserPrefDcrMode = 0;
#if !ENABLE_HDMI
        msAccOnOff( 0 );
#endif
#if ENABLE_DLC
        msDlcOnOff( 0 );
#endif
        SetECO();
    }

    UserPrefHue = DefHue;
    UserPrefSaturation = DefSaturation;
    if(UserPrefColorTemp==CTEMP_USER)
    {
        msAdjustHSC(MAIN_WINDOW, UserPrefHue, UserPrefSaturation, UserPrefContrast);
    }
    UserPrefIndependentHueR = DefIndependentH;
    UserPrefIndependentHueG = DefIndependentH;
    UserPrefIndependentHueB = DefIndependentH;
    UserPrefIndependentHueC = DefIndependentH;
    UserPrefIndependentHueM = DefIndependentH;
    UserPrefIndependentHueY = DefIndependentH;

    UserPrefIndependentSaturationR = DefIndependentS;
    UserPrefIndependentSaturationG = DefIndependentS;
    UserPrefIndependentSaturationB = DefIndependentS;
    UserPrefIndependentSaturationC = DefIndependentS;
    UserPrefIndependentSaturationM = DefIndependentS;
    UserPrefIndependentSaturationY = DefIndependentS;

    UserPrefIndependentBrightnessR = DefIndependentY;
    UserPrefIndependentBrightnessG = DefIndependentY;
    UserPrefIndependentBrightnessB = DefIndependentY;
    UserPrefIndependentBrightnessC = DefIndependentY;
    UserPrefIndependentBrightnessM = DefIndependentY;
    UserPrefIndependentBrightnessY = DefIndependentY;

    AdjustAllIndependentHSYValue();

#if MWEFunction
    UserPrefColorFlags = 0;
    UserPrefSubContrast = DefSubContrast;
    UserPrefSubBrightness = DefSubBrightness;
    UserPrefBFSize = 1;
    msAdjustSubBrightness( MAIN_WINDOW, UserPrefSubBrightness, UserPrefSubBrightness, UserPrefSubBrightness );
    mStar_AdjustContrast(DefContrastBase + UserPrefContrast );//msAdjustVideoContrast( MAIN_WINDOW, DefContrastBase + UserPrefContrast );
    SetBFSize( UserPrefBFSize );
#endif
    UserprefHistogram1 = 0x40;
    UserprefHistogram2 = 0xB0;
    UserprefALha = 50;
    UserprefBata = 50;
    UserPrefPowerSavingEn = PowerSavingMenuItems_On;
	
	UserprefPowerKeyEnable = 1;

    if(CURRENT_INPUT_IS_VGA())//(( SrcInputType < Input_Digital ) )
    {
        if(( !FactoryModeFlag ) && !ProductModeFlag )   //for nw aoc factorymode
        {
            //mStar_AutoGeomtry();
            //UserPrefAutoTimes=0;
            if( mStar_AutoGeomtry() == TRUE )
                //0707
            {
                UserPrefAutoTimes = 1;
                UserPrefAutoHStart = UserPrefHStart;
                UserPrefAutoVStart = UserPrefVStart;
            }
        }
        // UserPrefAutoHStart = UserPrefHStart;
        {
            BYTE modeIndex, flag;
            flag = SrcFlags; //UserModeFlag
            modeIndex = SaveIndex; //SrcModeIndex;
            SrcFlags &= ~bUserMode;
#if !USEFLASH
            for( SaveIndex = 0; SaveIndex < NumberOfMode; SaveIndex++ )
            {
                NVRam_WriteByte( nvrModeAddr( CheckSum ), 0 );
            }
            for( SaveIndex = 0; SaveIndex < NumberOfMode; SaveIndex++ )
            {
                modeInfo.HerFreq = 0;
                modeInfo.VerFreq = 0;
                modeInfo.VTotal = 0;
                modeInfo.Flags = 0;
                NVRam_WriteTbl( nvrModeInfoAddr( SaveIndex ), ( BYTE* ) &modeInfo, ModeInfoSize );
            }
            SaveIndex = 0;
            SrcFlags = flag;
            modeInfo.HerFreq = hFreq;
            modeInfo.VerFreq = vFreq;
            modeInfo.VTotal = SrcVTotal;
            modeInfo.Flags = SrcFlags & 0x13;
            NVRam_WriteTbl( nvrModeInfoAddr( SaveIndex ), ( BYTE* ) &modeInfo, ModeInfoSize );
            NVRam_WriteByte( nvrModeAddr( CheckSum ), 0 );
            NVRam_WriteByte( nvrMonitorAddr( SavedModeIndex ), UserPrefSavedModeIndex );
            SaveModeSetting();
#else
            SaveIndex = SrcModeIndex;//SaveIndex = 0;
            SrcFlags = flag;
            Flash_ClearModeSet();
            SaveModeSetting();
#endif
        }
    }
    else
    {
        Delay1ms( 500 );
    }
    // 100805 for when reset all if you change the OSD postion to x=0,y=0 it will show the OSD window on center
    //    Osd_SetPosition(UserPrefOsdHStart, UserPrefOsdVStart);
#if Enable_Expansion            //111221 modified Expansion mode condition - B42048, B42029
    Osd_Hide();
    UserprefExpansionMode = DefExpansion;
    OverScanSetting.AspRatio = UserprefExpansionMode;
    SetExpansionMode();//This Function will mute when reset all setting.
#endif

    if( FactoryModeFlag )
    {
#if USEFLASH
        UserprefBacklighttime = BlacklitTime = 0;
        SaveMonitorSetting2();
#else
        BlacklitTime = 0;
        SaveBlacklitTime();
#endif
    }
    Clr_BurninModeFlag();
    Clr_DoBurninModeFlag(); // 091014 coding
    SaveMonitorSetting(); //1020
    if( ProductModeFlag )
    {
#if USEFLASH
        FactoryProductModeValue = ProductModeOffValue;
        SaveFactorySetting();
#else
        WORD temp = ProductModeOffValue;
        NVRam_WriteTbl( ProductModeAddr, ( BYTE* )( &temp ), 2 );
#endif
    }

    if( ProductModeFlag )
    {
        //Power_TurnOnAmberLed();canceled PE Request
        Clr_ProductModeFlag();//20091022
    }
#if 0//DECREASE_V_SCALING       //111103 Rick modified for avoid auto fail after factory reset - A070
    DecVScaleValue = 0;
#endif

    mStar_BlackWhiteScreenCtrl(BW_SCREEN_OFF);

    return TRUE;
}
Bool AutoConfig( void )
{
    Bool result;
    result = mStar_AutoGeomtry();
    //UserPrefAutoHStart=UserPrefHStart;
    if( result )
    {
        UserPrefAutoHStart = UserPrefHStart;
        UserPrefAutoVStart = UserPrefVStart;
        UserPrefAutoTimes = 1;
        SaveModeSetting();
    }
#if ENABLE_VGA_INPUT
    else
    {
        UserPrefHTotal = StandardModeHTotal;
        UserPrefHStart = StandardModeHStart;
        UserPrefVStart = StandardModeVStart;
        UserPrefAutoHStart = UserPrefHStart;
        UserPrefAutoVStart = UserPrefVStart;
        UserPrefPhase = 18;
        UserPrefAutoTimes = 0;
        drvADC_AdjustHTotal( UserPrefHTotal );
        drvADC_SetPhaseCode( UserPrefPhase );
        mStar_AdjustHPosition(UserPrefHStart);//old_msWrite2Byte( SC0_07, UserPrefHStart );
        mStar_AdjustVPosition(UserPrefAutoVStart);
    }
#endif
    return result;
}

#if 1
/*
Bool ResetBriConValue(void)
{
UserPrefBrightness=UserPrefBrightness6500K;
UserPrefContrast=UserPrefContrast6500K;
mStar_AdjustBrightness(UserPrefBrightness);
mStar_AdjustContrast(UserPrefContrast);
SaveMonitorSetting();
return TRUE;
}
 */
Bool ResetGeometry( void )
{
    UserPrefHStart = StandardModeHStart;
    mStar_AdjustHPosition( UserPrefHStart );
    //NVRam_WriteByte(nvrModeAddr(HStart), UserPrefHStart);
    UserPrefVStart = StandardModeVStart;
    mStar_AdjustVPosition( UserPrefVStart );
    //NVRam_WriteByte(nvrModeAddr(VStart), UserPrefVStart);
    UserPrefHTotal = StandardModeHTotal;
    drvADC_AdjustHTotal( UserPrefHTotal );
    //NVRam_WriteByte(nvrModeAddr(HTotal), UserPrefHTotal);
    UserPrefPhase = 0;
    drvADC_SetPhaseCode( UserPrefPhase );
    //NVRam_WriteByte(nvrModeAddr(Phase), UserPrefPhase);
    AutoConfig();
    return TRUE;
}
#endif
/*void SaveUserPref(void)
{
    // 2006/10/24 7:5PM by Emily      printData("SaveUserPref", 0);
    SaveMonitorSetting();
    if (SrcInputType < Input_Digital)
    {
        SaveModeSetting();
    }
}*/

#if !USEFLASH
void SaveFactorySettingByItem( BYTE itemIndex )
{
    if( itemIndex == 2 )
    {
        NVRam_WriteByte( nvrFactoryAddr( AdcRedGain ), UserPrefAdcRedGain );
    }
    else if( itemIndex == 4 )
    {
        NVRam_WriteByte( nvrFactoryAddr( AdcGreenGain ), UserPrefAdcGreenGain );
    }
    else if( itemIndex == 6 )
    {
        NVRam_WriteByte( nvrFactoryAddr( AdcBlueGain ), UserPrefAdcBlueGain );
    }
    else if( itemIndex == 8 )
    {
        NVRam_WriteByte( nvrFactoryAddr( AdcRedOffset ), UserPrefAdcRedOffset );
    }
    else if( itemIndex == 10 )
    {
        NVRam_WriteByte( nvrFactoryAddr( AdcGreenOffset ), UserPrefAdcGreenOffset );
    }
    else if( itemIndex == 12 )
    {
        NVRam_WriteByte( nvrFactoryAddr( AdcBlueOffset ), UserPrefAdcBlueOffset );
    }
    else if( itemIndex == 14 )
    {
        NVRam_WriteByte( nvrFactoryAddr( RedColorCool1 ), UserPrefRedColorCool1 );
    }
    else if( itemIndex == 16 )
    {
        NVRam_WriteByte( nvrFactoryAddr( GreenColorCool1 ), UserPrefGreenColorCool1 );
    }
    else if( itemIndex == 18 )
    {
        NVRam_WriteByte( nvrFactoryAddr( BlueColorCool1 ), UserPrefBlueColorCool1 );
    }
    else if( itemIndex == 20 )
    {
        NVRam_WriteByte( nvrFactoryAddr( FBrightnessCool1 ), FUserPrefBrightnessCool1 );
    }
    else if( itemIndex == 22 )
    {
        NVRam_WriteByte( nvrFactoryAddr( FContrastCool1 ), FUserPrefContrastCool1 );
    }
    else if( itemIndex == 24 )
    {
        NVRam_WriteByte( nvrFactoryAddr( RedColorNormal ), UserPrefRedColorNormal );
    }
    else if( itemIndex == 26 )
    {
        NVRam_WriteByte( nvrFactoryAddr( GreenColorNormal ), UserPrefGreenColorNormal );
    }
    else if( itemIndex == 28 )
    {
        NVRam_WriteByte( nvrFactoryAddr( BlueColorNormal ), UserPrefBlueColorNormal );
    }
    else if( itemIndex == 30 )
    {
        NVRam_WriteByte( nvrFactoryAddr( FBrightnessNormal ), FUserPrefBrightnessNormal );
    }
    else if( itemIndex == 32 )
    {
        NVRam_WriteByte( nvrFactoryAddr( FContrastNormal ), FUserPrefContrastNormal );
    }
    else if( itemIndex == 34 )
    {
        NVRam_WriteByte( nvrFactoryAddr( RedColorWarm1 ), UserPrefRedColorWarm1 );
    }
    else if( itemIndex == 36 )
    {
        NVRam_WriteByte( nvrFactoryAddr( GreenColorWarm1 ), UserPrefGreenColorWarm1 );
    }
    else if( itemIndex == 38 )
    {
        NVRam_WriteByte( nvrFactoryAddr( BlueColorWarm1 ), UserPrefBlueColorWarm1 );
    }
    else if( itemIndex == 40 )
    {
        NVRam_WriteByte( nvrFactoryAddr( FBrightnessWarm1 ), FUserPrefBrightnessWarm1 );
    }
    else if( itemIndex == 42 )
    {
        NVRam_WriteByte( nvrFactoryAddr( FContrastWarm1 ), FUserPrefContrastWarm1 );
    }
    else if( itemIndex == 44 )
        NVRam_WriteByte( nvrFactoryAddr( RedColorsRGB ), UserPrefRedColorSRGB );
    else if( itemIndex == 46 )
        NVRam_WriteByte( nvrFactoryAddr( GreenColorsRGB ), UserPrefGreenColorSRGB );
    else if( itemIndex == 48 )
        NVRam_WriteByte( nvrFactoryAddr( BlueColorsRGB ), UserPrefBlueColorSRGB );
    else if( itemIndex == 50 )
    {
        NVRam_WriteByte( nvrFactoryAddr( FBrightnesssRGB ), FUserPrefBrightnessSRGB );
    }
    else if( itemIndex == 52 )
    {
        NVRam_WriteByte( nvrFactoryAddr( FContrastsRGB ), FUserPrefContrastSRGB );
    }
    //else if (itemIndex == 54)
    //{
    //    NVRam_WriteByte(nvrMonitorAddr(MonitorFlag), MonitorFlags);
    //}
}
#endif

//=======================================================================================
// For Factory alignment
#if 1
Bool AutoColor( void )
{
#if ENABLE_VGA_INPUT
    Bool result;
    if(!CURRENT_INPUT_IS_VGA())//( SrcInputType == Input_Digital || SrcInputType == Input_Digital2 )
        return TRUE;
    result = mStar_AutoColor();
    if( result )
    {
        //Osd_DrawStr(10, 2, PassText());
        SaveFactorySetting();
    }
    return result;
#else
    return 0;
#endif
}
#if !USEFLASH
Bool EraseAllEEPROMCell( void )
{
    if( UserPrefBrightness == 0 && UserPrefContrast == MinContrastValue )
    {
        WORD  i;
#ifdef EEPRom_24C04
        for( i = 0; i < 0x1ff; i++ )
#else
        for( i = 0; i < 0x7ff; i++ )
#endif
            NVRam_WriteByte( i, 0xFF );
        Power_TurnOnAmberLed();
    }
    return TRUE;
}
#else
Bool EraseFlashUserData(void) //2011.9.13 14:54 CC - A022
{

    if( UserPrefBrightness != 0|| UserPrefContrast!= MinContrastValue )
    {   //printMsg("EraseFlashUserData(1)");
        Delay1ms( 200 ); // for Factory check delay,It's can reduce
        return FALSE;
    }

    if( UserPrefBrightness == 0 && UserPrefContrast == MinContrastValue )
    {   //printMsg("EraseFlashUserData(2)");
        FlashSectorErase( TRUE, FLASH_FactorySettingAddr );
        FlashSectorErase( TRUE, FLASH_MonitorSettingAddr );
        FlashSectorErase( TRUE, FLASH_MonitorSetting2Addr );
        FlashSectorErase( TRUE, FLASH_TimingModeAddr );
        FlashSectorErase( TRUE, FLASH_FreeBufferAddr);
        Power_TurnOnAmberLed();
        Delay1ms( 1000 );
    }

    return TRUE;
}
#endif
#if 1
//====================================================
Bool AdjustRedGain( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefAdcRedGain, 0, 0xFFF, 1 );
    if( tempValue == UserPrefAdcRedGain )
    {
        return FALSE;
    }
    UserPrefAdcRedGain = tempValue;
    //msADC_AdjustAdcRedGain( UserPrefAdcRedGain );
#if ENABLE_VGA_INPUT
    drvADC_SetRedGainCode(UserPrefAdcRedGain);
#endif
    return TRUE;
}
WORD GetRedGainValue( void )
{
    return UserPrefAdcRedGain;
}
Bool AdjustGreenGain( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefAdcGreenGain, 0, 0xFFF, 1 );
    if( tempValue == UserPrefAdcGreenGain )
    {
        return FALSE;
    }
    UserPrefAdcGreenGain = tempValue;
    //msADC_AdjustAdcGreenGain( UserPrefAdcGreenGain );
#if ENABLE_VGA_INPUT
    drvADC_SetGreenGainCode(UserPrefAdcGreenGain);
#endif
    return TRUE;
}
WORD GetGreenGainValue( void )
{
    return UserPrefAdcGreenGain;
}
Bool AdjustBlueGain( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefAdcBlueGain, 0, 0xFFF, 1 );
    if( tempValue == UserPrefAdcBlueGain )
    {
        return FALSE;
    }
    UserPrefAdcBlueGain = tempValue;
    //msADC_AdjustAdcBlueGain( UserPrefAdcBlueGain );
#if ENABLE_VGA_INPUT
    drvADC_SetBlueGainCode(UserPrefAdcBlueGain);
#endif
    return TRUE;
}
WORD GetBlueGainValue( void )
{
    return UserPrefAdcBlueGain;
}
//====================================================
//====================================================
Bool AdjustAdcRedOffset( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefAdcRedOffset, 0, 0x7FF, 1 );
    if( tempValue == UserPrefAdcRedOffset )
    {
        return FALSE;
    }
    UserPrefAdcRedOffset = tempValue;
    //msADC_AdjustAdcRedOffset( UserPrefAdcRedOffset );
#if ENABLE_VGA_INPUT
    drvADC_SetRedOffsetCode(UserPrefAdcRedOffset);
#endif
    return TRUE;
}
WORD GetAdcRedOffsetValue( void )
{
    return UserPrefAdcRedOffset;
}
Bool AdjustAdcGreenOffset( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefAdcGreenOffset, 0, 0x7FF, 1 );
    if( tempValue == UserPrefAdcGreenOffset )
    {
        return FALSE;
    }
    UserPrefAdcGreenOffset = tempValue;
    //msADC_AdjustAdcGreenOffset( UserPrefAdcGreenOffset );
#if ENABLE_VGA_INPUT
    drvADC_SetGreenOffsetCode(UserPrefAdcGreenOffset);
#endif
    return TRUE;
}
WORD GetAdcGreenOffsetValue( void )
{
    return UserPrefAdcGreenOffset;
}
Bool AdjustAdcBlueOffset( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefAdcBlueOffset, 0, 0x7FF, 1 );
    if( tempValue == UserPrefAdcBlueOffset )
    {
        return FALSE;
    }
    UserPrefAdcBlueOffset = tempValue;
    //msADC_AdjustAdcBlueOffset( UserPrefAdcBlueOffset );
#if ENABLE_VGA_INPUT
    drvADC_SetBlueOffsetCode(UserPrefAdcBlueOffset);
#endif
    return TRUE;
}
WORD GetAdcBlueOffsetValue( void )
{
    return UserPrefAdcBlueOffset;
}
#endif

void GetColorTempRGB(ColorType *pstColor)
{
    pstColor->u8Red     =*(ColorTempPtr[UserPrefColorTemp][2]);
    pstColor->u8Green   =*(ColorTempPtr[UserPrefColorTemp][3]);
    pstColor->u8Blue    =*(ColorTempPtr[UserPrefColorTemp][4]);
}

void ReadColorTempSetting( void )
{
//   LoadStatus loadStatus = LoadStatus_SUCCESS;

    if (UserPrefECOMode == ECO_Standard)
    {
        UserPrefContrast =  *ColorTempPtr[UserPrefColorTemp][1];
        UserPrefBrightness =  *ColorTempPtr[UserPrefColorTemp][0];
    }
	
/*    switch(UserPrefColorTemp)
    {  

        case CTEMP_Cool1: //9300K
            printMsg("CURRENT_COLOR_TEMP: 9300K");
             loadStatus = IDXLoadDeltaEColorMode(148);
            break;
        case CTEMP_Normal: //6500K
            printMsg("CURRENT_COLOR_TEMP: 6500K");
            loadStatus = IDXLoadDeltaEColorMode(20);
            break;
        case CTEMP_Warm1: //5700K
            printMsg("CURRENT_COLOR_TEMP: 5000K");
            loadStatus = IDXLoadDeltaEColorMode(146);
            break;
        case CTEMP_SRGB:
            printMsg("sRGB");
            loadStatus = IDXLoadDeltaEColorMode(0); 
            break;
	
    }

   if(loadStatus== LoadStatus_SUCCESS)
   	return;
   */
    switch(UserPrefColorTemp)
    {
    #if (COLOR_ACCURACY == COLOR_10_BIT)
        case CTEMP_Cool1: //9300K
            UserPrefRedColorCool1 = 232;
            UserPrefGreenColorCool1 = 240;
            UserPrefBlueColorCool1 = 255;
            break;
        case CTEMP_Normal:
            UserPrefRedColorNormal = 244;
            UserPrefGreenColorNormal = 255;
            UserPrefBlueColorNormal = 244;
            break;
        case CTEMP_Warm1: //6500K
            UserPrefRedColorWarm1 = 255;
            UserPrefGreenColorWarm1 = 232;
            UserPrefBlueColorWarm1 = 240;
            break;
        case CTEMP_SRGB:
            UserPrefRedColorSRGB = 244;
            UserPrefGreenColorSRGB = 255;
            UserPrefBlueColorSRGB = 244;
            break;
    #else
        case CTEMP_Cool1: //9300K
            UserPrefRedColorCool1 = DefCool_RedColor;
            UserPrefGreenColorCool1 = DefCool_GreenColor;
            UserPrefBlueColorCool1 = DefCool_BlueColor;
            break;
        case CTEMP_Normal:
            UserPrefRedColorNormal = DefNormal_RedColor;
            UserPrefGreenColorNormal = DefNormal_GreenColor;
            UserPrefBlueColorNormal = DefNormal_BlueColor;
            break;
        case CTEMP_Warm1: //6500K
            UserPrefRedColorWarm1 = DefWarm_RedColor;
            UserPrefGreenColorWarm1 = DefWarm_GreenColor;
            UserPrefBlueColorWarm1 = DefWarm_BlueColor;
            break;
        case CTEMP_SRGB:
            UserPrefRedColorSRGB = DefsRGB_RedColor;
            UserPrefGreenColorSRGB = DefsRGB_GreenColor;
            UserPrefBlueColorSRGB = DefsRGB_BlueColor;
            break;
    #endif
    }
    UserPrefRedColor =  *ColorTempPtr[UserPrefColorTemp][2];
    UserPrefGreenColor =  *ColorTempPtr[UserPrefColorTemp][3];
    UserPrefBlueColor =  *ColorTempPtr[UserPrefColorTemp][4];
    CheckColorValueRange();
    UserPrefBrightness %= 101;
    // 2006/11/10 11:13PM by KK   if(!UserPrefDcrMode) // for under DCR mode, the picture show filter
    mStar_AdjustBrightness( UserPrefBrightness );
    mStar_AdjustContrast( UserPrefContrast );
}

void SetECO( void )
{
    SetColorTemp();
    if( UserPrefECOMode == ECO_Text )
    {
        UserPrefBrightness = 20;
        UserPrefContrast = MinContrastValue + ( MaxContrastValue - MinContrastValue ) * 0.5;
    }
    else if( UserPrefECOMode == ECO_Internet )
    {
        UserPrefBrightness = 40;
        UserPrefContrast = MinContrastValue + ( MaxContrastValue - MinContrastValue ) * 0.5;
    }
    else if( UserPrefECOMode == ECO_Game )
    {
        UserPrefBrightness = 60;
        UserPrefContrast = MinContrastValue + ( MaxContrastValue - MinContrastValue ) * 0.5;
    }
    else if( UserPrefECOMode == ECO_Movie )
    {
        UserPrefBrightness = 80;
        UserPrefContrast = MinContrastValue + ( MaxContrastValue - MinContrastValue ) * 0.5;
    }
    else if( UserPrefECOMode == ECO_Sports )
    {
        UserPrefBrightness = 100;
        UserPrefContrast = MinContrastValue + ( MaxContrastValue - MinContrastValue ) * 0.5;
    }
    mStar_AdjustBrightness( UserPrefBrightness );
    mStar_AdjustContrast( UserPrefContrast );
}
Bool AdjustECOMode( MenuItemActionType action )
{
#if 0
    if( UserPrefColorTemp == CTEMP_SRGB )
    {
        UserPrefECOMode = ECO_Standard;
        return TRUE;
    }
#else       //111110 Rick modified adjust ECO condition - B40139
    if(PushECOHotKeyFlag)
    {
        if(UserPrefColorTemp == CTEMP_SRGB)
            UserPrefColorTemp=CTEMP_Warm1;
        Clr_PushECOHotKeyFlag();
    }
#endif
    UserPrefDcrMode = 0;
#if MWEFunction
    Clr_GreenFlag();
    Clr_SkinFlag();
    Clr_BlueFlag();
    Clr_ColorAutoDetectFlag();
    Clr_FullEnhanceFlag();
    Clr_DemoFlag();
    Clr_PictureBoostFlag();
    SetBFSize( UserPrefBFSize );
#endif
    //Benz 2007.4.23   15:35:12
    //Benz 2007.4.23   16:40:20     Osd_Draw4Num(1,1,UserPrefECOMode);
    if( action == MIA_DecValue )
    {
        UserPrefECOMode = ( UserPrefECOMode + ECO_Nums - 1 ) % ECO_Nums; //Benz have some bug issue
    }
    else
    {
        UserPrefECOMode = ( UserPrefECOMode + 1 ) % ECO_Nums;
    }
    //Benz 2007.4.23   16:40:10     Osd_Draw4Num(2,1,UserPrefECOMode);
#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( ECOMode ), UserPrefECOMode );
#if MWEFunction
    NVRam_WriteByte( nvrMonitorAddr( ColorFlag ), UserPrefColorFlags );
#endif
#else
    //SaveMonitorSetting();
    Set_FlashSaveMonitorFlag();
#endif
    SetECO();
    return TRUE;
}
#if Enable_Gamma
Bool AdjustGamaMode( MenuItemActionType action )
{
    //if (UserPrefColorTemp==CTEMP_SRGB)  //0617
    //  return FALSE;
    if( action == MIA_DecValue )
    {
        UserPrefGamaMode = ( UserPrefGamaMode + GAMA_Nums - 1 ) % GAMA_Nums;
    }
    else
    {
        UserPrefGamaMode = ( UserPrefGamaMode + 1 ) % GAMA_Nums;
    }
#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( GamaMode ), UserPrefGamaMode );
#else
    //SaveMonitorSetting();
    Set_FlashSaveMonitorFlag();
#endif

    // 2006/11/9 11:44PM by Emily     if(!BlueFlag||!SkinFlag||!GreenFlag||!ColorAutoDetectFlag)
    {
        //old_msWriteByte( BK0_00, REGBANK3 );
       // old_msWriteByte( BK3_40, 0x00 );
       // old_msWriteByte( BK0_00, REGBANK0 );
        mStar_SetupGamma( UserPrefGamaMode );
#if MWEFunction
        if( UserPrefColorFlags != 0 || UserPrefDcrMode )
#else
        if( UserPrefDcrMode )
#endif
        {
#if !ENABLE_HDMI
            msAccOnOff( 1 );
#endif
           // old_msWriteByte( BK0_00, REGBANK3 );
           // old_msWriteByte( BK3_40, 0x01 );
           // old_msWriteByte( BK0_00, REGBANK0 );
        }
    }
    return TRUE;
}
#endif

Bool AdjustDcrMode( MenuItemActionType action )
{
#if 0//ENABLE_HDMI
    BYTE XDATA ucDomain;
#endif

    action = action;

#if 0//ENABLE_HDMI
    ucDomain = 0x00;
    if( gScInfo.InputColor != INPUT_RGB )
        ucDomain = 0xFF;
#endif

#if 1

    if( UserPrefDcrMode )
    {
        UserPrefDcrMode = 0;
        SetECO();
    }
    else
    {
        UserPrefDcrMode = 1;
        UserPrefColorTemp = CTEMP_Warm1;

        UserPrefECOMode = ECO_Standard;
        ReadColorTempSetting();

    #if MWEFunction
    #if 0//!ENABLE_HDMI // wait for coding
        //msAdjustSubBrightness( UserPrefSubBrightness );
        msAdjustSubBrightness( MAIN_WINDOW, UserPrefSubBrightness, UserPrefSubBrightness, UserPrefSubBrightness );
        msAdjustVideoContrast( DefContrastBase + UserPrefContrast );
    #endif
        Clr_PictureBoostFlag();
        Clr_ColorAutoDetectFlag();
        Clr_FullEnhanceFlag();
        Clr_SkinFlag();
        Clr_BlueFlag();
        Clr_GreenFlag();
        Clr_DemoFlag();
    #endif

    }
#if ENABLE_DLC
    msDCROnOff(UserPrefDcrMode, MAIN_WINDOW);
#endif
#else

    if( UserPrefDcrMode )
    {
        UserPrefDcrMode = 0;
        //old_msWriteByte( BK0_00, REGBANK0 );
        mStar_AdjustBrightness( UserPrefBrightness );
#if !ENABLE_HDMI
#if MWEFunction
        if( UserPrefColorFlags )
            msAccOnOff( 1 );
        else
#endif
            msAccOnOff( 0 );
#endif
        msDlcOnOff( 0 );
        SetECO();
    }
    else
    {

        UserPrefDcrMode = 1;

        //if ( UserPrefColorTemp == CTEMP_SRGB ) // 100817
        UserPrefColorTemp = CTEMP_Warm1;

        UserPrefECOMode = ECO_Standard;
        ReadColorTempSetting();
        #if MWEFunction
        #if !ENABLE_HDMI
        //msAdjustSubBrightness( UserPrefSubBrightness );
        msAdjustSubBrightness( MAIN_WINDOW, UserPrefSubBrightness, UserPrefSubBrightness, UserPrefSubBrightness );
        msAdjustVideoContrast( DefContrastBase + UserPrefContrast );
        #endif
        Clr_PictureBoostFlag(); // 2006/10/20 6:21PM by Emily for aoc request
        Clr_ColorAutoDetectFlag();
        Clr_FullEnhanceFlag();
        Clr_SkinFlag();
        Clr_BlueFlag();
        Clr_GreenFlag();
        Clr_DemoFlag();
#endif

        //old_msWriteByte( BK0_00, REGBANK3 );
        //old_msWriteByte( BK3_72, 0x70 );
        //old_msWriteByte( BK3_73, 0x70 );
        //old_msWriteByte( BK3_74, 0x70 );
        //old_msWriteByte( BK0_00, REGBANK0 );

        LoadACETable(MAIN_WINDOW, (BYTE *)t_Normal_ColorSettingTable, DefHue, DefSaturation, 0x80 ); // /UserPrefSubContrast
        msAccSetup( 0, PanelWidth, 0, PanelHeight );
#if !ENABLE_HDMI
        msAccOnOff( 1 );
#endif
        msDlcInit( PanelWidth, PanelHeight );
        msSetDlcStrength( g_LowStrength, g_HighStrength );
        LoadDLCTable( t_MWEDLC_Linear_Table );
#if !ENABLE_HDMI
        msDlcOnOff( 1 );
#endif
    }
#if ENABLE_HDMI
#if MWEFunction
    if( UserPrefDcrMode || UserPrefColorFlags )
#else
    if( UserPrefDcrMode )
#endif
    {
#if ENABLE_HDMI
        if( ucDomain )
        {
            SetMainWinVideoDomain( ucDomain );      //SetMainWinColorControl(ucDomain);
            mStar_WriteByteMask( BK0_5C, BIT3, BIT3 ); //old_msWriteByte( BK0_5C, old_msReadByte( BK0_5C ) | BIT3 );
        }
        else
#endif
            msAccOnOff( _ENABLE );

        if( UserPrefDcrMode )
            msDlcOnOff( _ENABLE );
    }
    else
    {
#if ENABLE_HDMI
        if( ucDomain )
            mStar_WriteByteMask( BK0_5C, 0x00, BIT3 ); //old_msWriteByte( BK0_5C, old_msReadByte( BK0_5C )&~BIT3 );
        else
#endif
            msAccOnOff( 0 );

        msDlcOnOff( _DISABLE );

#if ENABLE_HDMI
        SetMainWinVideoDomain( 0 ); //SetMainWinColorControl(0);
        if( ucDomain )
            mStar_WriteByteMask( BK0_02, BIT3, BIT3 ); //old_msWriteByte( BK0_02, old_msReadByte( BK0_02 ) | BIT3 );
#endif
    }
#endif

    msDCROnOff(UserPrefDcrMode, 0);
#endif


#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( DcrMode ), UserPrefDcrMode );
    NVRam_WriteByte( nvrMonitorAddr( ECOMode ), UserPrefECOMode );
#else
    //SaveMonitorSetting();
    Set_FlashSaveMonitorFlag();
#endif
    return TRUE;
}
Bool SetDCRmode(void)
{
    if (UserPrefDcrMode == MenuItemIndex)
        return FALSE;
    UserPrefDcrMode = MenuItemIndex;
#if ENABLE_DLC
    msDCROnOff(UserPrefDcrMode, MAIN_WINDOW);
#endif
    Set_SaveMonitorSettingFlag();
    return TRUE;
}

#if ENABLE_DPS
Bool AdjustDPSMode( MenuItemActionType action )
{
    action=action;

    if( UserprefDPSMode )
    {
        UserprefDPSMode = 0;
        msDPS_Off_Setting();
    }
    else
    {
        UserprefDPSMode = 1;
        msDPS_On_Setting();
    }

    #if !USEFLASH
        NVRam_WriteByte( nvrMonitorAddr( DPSMode ), UserprefDPSMode );
    #else
        Set_FlashSaveMonitorFlag();
    #endif
    return TRUE;
}
Bool SetDPSmode(void)
{
    if (UserprefDPSMode == MenuItemIndex)
        return FALSE;
    UserprefDPSMode = MenuItemIndex;
    //printData("========UserprefDPSMod====%x",UserprefDPSMode);
    if( UserprefDPSMode )
    {
        msDPS_On_Setting();
    }
    else
    {
        msDPS_Off_Setting();
    }
    Set_SaveMonitorSettingFlag();
    return TRUE;
}
#endif
#if ENABLE_DLC
Bool AdjustDLCMode( MenuItemActionType action )
{
    action=action;

    if( UserprefDLCMode )
    {
        UserprefDLCMode = 0;
        g_bDLCOnOff=0;

        msDlcOnOff(1);
    }
    else
    {
        UserprefDLCMode = 1;
        g_bDLCOnOff=0;
        msDlcOnOff(0);
    }

    #if !USEFLASH
        NVRam_WriteByte( nvrMonitorAddr( DLCMode ), UserprefDLCMode );
    #else
        Set_FlashSaveMonitorFlag();
    #endif
    return TRUE;
}
Bool SetDLCmode(void)
{
    if (UserprefDLCMode == MenuItemIndex)
        return FALSE;
    UserprefDLCMode = MenuItemIndex;
    //printData("========UserprefDLCMode====%x",UserprefDLCMode);
    if( UserprefDLCMode )
    {
        g_bDLCOnOff=0;
        msDlcOnOff(1);
    }
    else
    {
        //UserprefDLCMode = 1;
        g_bDLCOnOff=0;
        msDlcOnOff(0);
    }
    Set_SaveMonitorSettingFlag();
    return TRUE;
}
#endif

#if 0//ENABLE_3DLUT
Bool Adjust3DLUTMode( MenuItemActionType action )
{
    action=action;

    UserPref3DLUTmode=MenuItemIndex;
    ms3DLutEnable(MAIN_WINDOW, UserPref3DLUTmode);

    #if !USEFLASH
        NVRam_WriteByte( nvrMonitorAddr( RGB3DLUTMode ), UserPref3DLUTmode );
    #else
        Set_FlashSaveMonitorFlag();
    #endif
    return TRUE;
}
#endif

#if ENABLE_SUPER_RESOLUTION
Bool AdjustSuperResolutionMode( MenuItemActionType action )
{
    if( action == MIA_DecValue )
    {
        UserPrefSuperResolutionMode = ( UserPrefSuperResolutionMode + SRMODE_Nums - 1 ) % SRMODE_Nums;
    }
    else
    {
        UserPrefSuperResolutionMode = ( UserPrefSuperResolutionMode + 1 ) % SRMODE_Nums;
    }

#if !USEFLASH
    NVRam_WriteByte( nvrMonitorAddr( SuperResolutionMode ), UserPrefSuperResolutionMode );
#else
    Set_FlashSaveMonitorFlag();
#endif
    mStar_SetupSuperResolution( UserPrefSuperResolutionMode );
    return TRUE;
}
Bool SetSuperResolutionMode(void)
{
    if (UserPrefSuperResolutionMode== MenuItemIndex)
        return FALSE;
    UserPrefSuperResolutionMode = MenuItemIndex;
    mStar_SetupSuperResolution( UserPrefSuperResolutionMode );
    Set_SaveMonitorSettingFlag();
    return TRUE;
}
#endif

#if ENABLE_DeBlocking
Bool AdjustDeBlockingMode( MenuItemActionType action )
{
    action=action;

    // temp, TNR always on/off with deblocking function.
    AdjustTNR(0);

    if( UserPrefDeBlocking)
        UserPrefDeBlocking = 0;
    else
        UserPrefDeBlocking = 1;

    #if !USEFLASH
        NVRam_WriteByte( nvrMonitorAddr( DeBlocking ), UserPrefDeBlocking );
    #else
        Set_FlashSaveMonitorFlag();
    #endif

    msDeBlockingOnOff(UserPrefDeBlocking, MAIN_WINDOW);

    return TRUE;
}
#endif

//=====================================================================
#if 1
Bool SetFactoryColorTempCool1( void )
{
    UserPrefColorTemp = CTEMP_Cool1;
    ReadColorTempSetting(); // 2006/11/10 6:58PM by KK ReadCool1Setting();
    //mStar_AdjustBrightness( UserPrefBrightness );
    //mStar_AdjustContrast( UserPrefContrast );
    return TRUE;
}
Bool SetFactoryColorTempWarm1( void )
{
    UserPrefColorTemp = CTEMP_Warm1;
    ReadColorTempSetting(); // 2006/11/10 6:57PM by KK  ReadWarm1Setting();
    //mStar_AdjustBrightness( UserPrefBrightness );
    //mStar_AdjustContrast( UserPrefContrast );
    return TRUE;
}
Bool SetFactoryColorTempNormal( void )
{
    UserPrefColorTemp = CTEMP_Normal;
    ReadColorTempSetting(); // 2006/11/10 6:58PM by KK ReadNormalSetting();
    //mStar_AdjustBrightness( UserPrefBrightness );
    //mStar_AdjustContrast( UserPrefContrast );
    return TRUE;
}
Bool SetFactoryColorTempSRGB( void )
{
    UserPrefColorTemp = CTEMP_SRGB;
    ReadColorTempSetting(); // 2006/11/10 6:58PM by KK ReadSRGBSetting();
    //mStar_AdjustBrightness( UserPrefBrightness );
    //mStar_AdjustContrast( UserPrefContrast );
    return TRUE;
}

#endif
#if 1

//====================================================
Bool AdjustRedColorCool1( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefRedColorCool1, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefRedColorCool1 )
    {
        return FALSE;
    }
    UserPrefRedColorCool1 = tempValue;
#if UsesRGB
    mStar_AdjustContrast( UserPrefContrast );
#else
    mStar_AdjustRedColor( UserPrefRedColorCool1, UserPrefContrastCool1 );
#endif
    return TRUE;
}
Bool AdjustGreenColorCool1( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefGreenColorCool1, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefGreenColorCool1 )
    {
        return FALSE;
    }
    UserPrefGreenColorCool1 = tempValue;
#if UsesRGB
    mStar_AdjustContrast( UserPrefContrast );
#else
    mStar_AdjustGreenColor( UserPrefGreenColorCool1, UserPrefContrastCool1 );
#endif
    return TRUE;
}
Bool AdjustBlueColorCool1( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefBlueColorCool1, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefBlueColorCool1 )
    {
        return FALSE;
    }
    UserPrefBlueColorCool1 = tempValue;
#if UsesRGB
    mStar_AdjustContrast( UserPrefContrast );
#else
    mStar_AdjustBlueColor( UserPrefBlueColorCool1, UserPrefContrastCool1 );
#endif
    return TRUE;
}
Bool AdjustBrightnessCool1( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefBrightnessCool1, 0, 100, 1 );
    if( tempValue == UserPrefBrightnessCool1 )
    {
        return FALSE;
    }
    UserPrefBrightnessCool1 = tempValue;
    UserPrefBrightness = UserPrefBrightnessCool1;
    if( FactoryModeFlag )
        FUserPrefBrightnessCool1 = UserPrefBrightnessCool1;
    mStar_AdjustBrightness( UserPrefBrightnessCool1 );
    return TRUE;
}
Bool AdjustContrastCool1( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefContrastCool1, MinContrastValue, MaxContrastValue, 1 );
    if( tempValue == UserPrefContrastCool1 )
    {
        return FALSE;
    }
    UserPrefContrastCool1 = tempValue;
    UserPrefContrast = UserPrefContrastCool1;
    if( FactoryModeFlag )
        FUserPrefContrastCool1 = UserPrefContrastCool1;
    mStar_AdjustContrast( UserPrefContrast );
    return TRUE;
}
WORD GetRColorCool1Value( void )
{
    return UserPrefRedColorCool1;
}
WORD GetGColorCool1Value( void )
{
    return UserPrefGreenColorCool1;
}
WORD GetBColorCool1Value( void )
{
    return UserPrefBlueColorCool1;
}
WORD GetBrightnessCool1Value( void )
{
    return FUserPrefBrightnessCool1 % 101;
}
WORD GetContrastCool1Value( void ) // WMZ 050819
{
    WORD temp;
    temp = GetScale100Value( FUserPrefContrastCool1, MinContrastValue, MaxContrastValue );
    return temp;
}
//====================================================
Bool AdjustRedColorWarm1( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefRedColorWarm1, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefRedColorWarm1 )
    {
        return FALSE;
    }
    UserPrefRedColorWarm1 = tempValue;
#if UsesRGB
    mStar_AdjustContrast( UserPrefContrast );
#else
//    mStar_AdjustRedColor( UserPrefRedColorWarm1, UserPrefContrastWarm1 );
    msAPI_AdjustRGBColor(MAIN_WINDOW, UserPrefContrast, UserPrefRedColorWarm1, UserPrefGreenColorWarm1, UserPrefBlueColorWarm1);
#endif
    return TRUE;
}
Bool AdjustGreenColorWarm1( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefGreenColorWarm1, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefGreenColorWarm1 )
    {
        return FALSE;
    }
    UserPrefGreenColorWarm1 = tempValue;
#if UsesRGB
    mStar_AdjustContrast( UserPrefContrast );
#else
//    mStar_AdjustGreenColor( UserPrefGreenColorWarm1, UserPrefContrastWarm1 );
    msAPI_AdjustRGBColor(MAIN_WINDOW, UserPrefContrast, UserPrefRedColorWarm1, UserPrefGreenColorWarm1, UserPrefBlueColorWarm1);
#endif
    return TRUE;
}
Bool AdjustBlueColorWarm1( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefBlueColorWarm1, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefBlueColorWarm1 )
    {
        return FALSE;
    }
    UserPrefBlueColorWarm1 = tempValue;
#if UsesRGB
    mStar_AdjustContrast( UserPrefContrast );
#else
//    mStar_AdjustBlueColor( UserPrefBlueColorWarm1, UserPrefContrastWarm1 );
    msAPI_AdjustRGBColor(MAIN_WINDOW, UserPrefContrast, UserPrefRedColorWarm1, UserPrefGreenColorWarm1, UserPrefBlueColorWarm1);
#endif
    return TRUE;
}
Bool AdjustBrightnessWarm1( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefBrightnessWarm1, 0, 100, 1 );
    if( tempValue == UserPrefBrightnessWarm1 )
    {
        return FALSE;
    }
    UserPrefBrightnessWarm1 = tempValue;
    UserPrefBrightness = UserPrefBrightnessWarm1;
    if( FactoryModeFlag )
        FUserPrefBrightnessWarm1 = UserPrefBrightnessWarm1;
    mStar_AdjustBrightness( UserPrefBrightnessWarm1 );
    return TRUE;
}
Bool AdjustContrastWarm1( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefContrastWarm1, MinContrastValue, MaxContrastValue, 1 );
    if( tempValue == UserPrefContrastWarm1 )
    {
        return FALSE;
    }
    UserPrefContrastWarm1 = tempValue;
    UserPrefContrast = UserPrefContrastWarm1;
    if( FactoryModeFlag )
        FUserPrefContrastWarm1 = UserPrefContrastWarm1;
    mStar_AdjustContrast( UserPrefContrast );
    return TRUE;
}
WORD GetRColorWarm1Value( void )
{
    return UserPrefRedColorWarm1;
}
WORD GetGColorWarm1Value( void )
{
    return UserPrefGreenColorWarm1;
}
WORD GetBColorWarm1Value( void )
{
    return UserPrefBlueColorWarm1;
}
WORD GetBrightnessWarm1Value( void )
{
    return FUserPrefBrightnessWarm1 % 101;
}
WORD GetContrastWarm1Value( void ) // WMZ 050819
{
    WORD temp;
    temp = GetScale100Value( FUserPrefContrastWarm1, MinContrastValue, MaxContrastValue );
    return temp;
}
//===============================================================
Bool AdjustRedColorNormal( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefRedColorNormal, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefRedColorNormal )
    {
        return FALSE;
    }
    UserPrefRedColorNormal = tempValue;
#if UsesRGB
    mStar_AdjustContrast( UserPrefContrast );
#else
    mStar_AdjustRedColor( UserPrefRedColorNormal, UserPrefContrastNormal );
#endif
    return TRUE;
}
Bool AdjustGreenColorNormal( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefGreenColorNormal, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefGreenColorNormal )
    {
        return FALSE;
    }
    UserPrefGreenColorNormal = tempValue;
#if UsesRGB
    mStar_AdjustContrast( UserPrefContrast );
#else
    mStar_AdjustGreenColor( UserPrefGreenColorNormal, UserPrefContrastNormal );
#endif
    return TRUE;
}
Bool AdjustBlueColorNormal( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefBlueColorNormal, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefBlueColorNormal )
    {
        return FALSE;
    }
    UserPrefBlueColorNormal = tempValue;
#if UsesRGB
    mStar_AdjustContrast( UserPrefContrast );
#else
    mStar_AdjustBlueColor( UserPrefBlueColorNormal, UserPrefContrastNormal );
#endif
    return TRUE;
}
Bool AdjustBrightnessNormal( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefBrightnessNormal, 0, 100, 1 );
    if( tempValue == UserPrefBrightnessNormal )
    {
        return FALSE;
    }
    UserPrefBrightnessNormal = tempValue;
    UserPrefBrightness = UserPrefBrightnessNormal;
    if( FactoryModeFlag )
        FUserPrefBrightnessNormal = UserPrefBrightnessNormal;
    mStar_AdjustBrightness( UserPrefBrightnessNormal );
    return TRUE;
}
Bool AdjustContrastNormal( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefContrastNormal, MinContrastValue, MaxContrastValue, 1 );
    if( tempValue == UserPrefContrastNormal )
    {
        return FALSE;
    }
    UserPrefContrastNormal = tempValue;
    UserPrefContrast = UserPrefContrastNormal;
    if( FactoryModeFlag )
        FUserPrefContrastNormal = UserPrefContrastNormal;
    mStar_AdjustContrast( UserPrefContrast );
    return TRUE;
}
WORD GetRColorNormalValue( void )
{
    return UserPrefRedColorNormal;
}
WORD GetGColorNormalValue( void )
{
    return UserPrefGreenColorNormal;
}
WORD GetBColorNormalValue( void )
{
    return UserPrefBlueColorNormal;
}
WORD GetBrightnessNormalValue( void )
{
    //   return FUserPrefBrightnessNormal % 101;
    return FUserPrefBrightnessNormal % 101;
}
WORD GetContrastNormalValue( void ) // WMZ 050819
{
    WORD temp;
    temp = GetScale100Value( FUserPrefContrastNormal, MinContrastValue, MaxContrastValue );
    return temp;
}
//====================================================
Bool AdjustRedColorSRGB( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefRedColorSRGB, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefRedColorSRGB )
    {
        return FALSE;
    }
    UserPrefRedColorSRGB = tempValue;
#if UsesRGB
    mStar_AdjustContrast( UserPrefContrast );
#else
    mStar_AdjustRedColor( UserPrefRedColorSRGB, UserPrefContrastSRGB );
#endif
    return TRUE;
}
Bool AdjustGreenColorSRGB( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefGreenColorSRGB, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefGreenColorSRGB )
    {
        return FALSE;
    }
    UserPrefGreenColorSRGB = tempValue;
#if UsesRGB
    mStar_AdjustContrast( UserPrefContrast );
#else
    mStar_AdjustGreenColor( UserPrefGreenColorSRGB, UserPrefContrastSRGB );
#endif
    return TRUE;
}
Bool AdjustBlueColorSRGB( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, UserPrefBlueColorSRGB, MinColorValue, MaxColorValue, 1 );
    if( tempValue == UserPrefBlueColorSRGB )
    {
        return FALSE;
    }
    UserPrefBlueColorSRGB = tempValue;
#if UsesRGB
    mStar_AdjustContrast( UserPrefContrast );
#else
    mStar_AdjustBlueColor( UserPrefBlueColorSRGB, UserPrefContrastSRGB );
#endif
    return TRUE;
}
Bool AdjustBrightnessSRGB( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, FUserPrefBrightnessSRGB, 0, 100, 1 );
    if( tempValue == FUserPrefBrightnessSRGB )
    {
        return FALSE;
    }
    FUserPrefBrightnessSRGB = tempValue;
    UserPrefBrightness = FUserPrefBrightnessSRGB;
    mStar_AdjustBrightness( UserPrefBrightness );
    return TRUE;
}
Bool AdjustContrastSRGB( MenuItemActionType action )
{
    WORD tempValue;
    tempValue = DecIncValue( action, FUserPrefContrastSRGB, MinContrastValue, MaxContrastValue, 1 );
    if( tempValue == FUserPrefContrastSRGB )
    {
        return FALSE;
    }
    FUserPrefContrastSRGB = tempValue;
    UserPrefContrast = FUserPrefContrastSRGB;
    mStar_AdjustContrast( UserPrefContrast );
    return TRUE;
}
WORD GetRColorSRGBValue( void )
{
    return UserPrefRedColorSRGB;
}
WORD GetGColorSRGBValue( void )
{
    return UserPrefGreenColorSRGB;
}
WORD GetBColorSRGBValue( void )
{
    return UserPrefBlueColorSRGB;
}
WORD GetBrightnessSRGBValue( void )
{
    return FUserPrefBrightnessSRGB % 101;
}
WORD GetContrastSRGBValue( void ) // WMZ 050819
{
    WORD temp;
    temp = GetScale100Value( FUserPrefContrastSRGB, MinContrastValue, MaxContrastValue );
    return temp;
}
#endif
//======================================================================================

Bool EnableBurninMode( void )
{
    PatternNo = 0;
    //old_msWriteByte( BK0_32, BIT0 );
    drvOSD_FrameColorEnable(TRUE);
    return TRUE;
}
#if 1
WORD GetBurninValue( void )
{
    return ( BurninModeFlag ) ? ( 1 ) : ( 0 );
}
Bool AdjustBurnin( void )
{
    if( BurninModeFlag )
    {
        Clr_BurninModeFlag();
        Clr_DoBurninModeFlag();     //110928 Rick modified for do burnin mode while burnin on - A027
    }
    else
    {
        Set_BurninModeFlag();
        Set_DoBurninModeFlag();      //110928 Rick modified for do burnin mode while burnin on - A027
	PatternNo = 7;
    }
#if USEFLASH
    //SaveMonitorSetting();
    Set_FlashForceSaveMonitorFlag();
#else
    NVRam_WriteByte( nvrMonitorAddr( MonitorFlag ), MonitorFlags );
#endif
    Delay1ms( 200 );
    return TRUE;
}
#endif
//====================================================
WORD GetProductionValue( void )
{
    return ( ProductModeFlag) ? ( 1 ) : ( 0 );
}
Bool AdjustProduction( MenuItemActionType action )
{
    action = action;
    if( UserPrefBrightness == 0 && UserPrefContrast == MinContrastValue )
    {
        if( ProductModeFlag )
        {
            #if USEFLASH
            FactoryProductModeValue = ProductModeOffValue;
            #else
            WORD u16TempValue = ProductModeOffValue;
            NVRam_WriteTbl( ProductModeAddr, ( BYTE* )( &u16TempValue ), 2 );
            #endif
            Clr_ProductModeFlag();
        }
        else
        {
            #if USEFLASH
            FactoryProductModeValue = ProductModeOnValue;
            #else
            WORD u16TempValue = ProductModeOnValue;
            NVRam_WriteTbl( ProductModeAddr, ( BYTE* )( &u16TempValue ), 2 );
            #endif
            Set_ProductModeFlag();
        }
        #if USEFLASH
        //SaveMonitorSetting();
        Set_FlashForceSaveMonitor2Flag();
        #else
        NVRam_WriteByte( nvrMonitorAddr( MonitorFlag ), System2Flags );
        #endif
        Delay1ms( 200 );
     }
    return TRUE;
}

WORD GetDDCWPActiveValue( void )
{
    return (( DDCWPActiveFlag) ? ( 1 ) : ( 0 ));
}
Bool AdjustDDCWP( MenuItemActionType action )
{
    action = action;
    if( DDCWPActiveFlag )
    {
        g_bServiceEDIDUnLock = 1;
        hw_ClrDDC_WP();
    }
    else
    {
       g_bServiceEDIDUnLock = 0;
       hw_SetDDC_WP();
    }
    Delay1ms( 200 );
    return TRUE;
}
//====================================================
Bool AdjustBankNo( MenuItemActionType action )
{
    if( action == MIA_IncValue )
    {
        MSBankNo = ( MSBankNo + 1 ) % 3;
    }
    else
    {
        MSBankNo = ( MSBankNo + 2 ) % 3;
    }
    return TRUE;
}
Bool AdjustAddr( MenuItemActionType action )
{
    if( action == MIA_IncValue )
    {
        MSAddr = ( MSAddr + 1 ) % 0x100;
    }
    else
    {
        MSAddr = ( MSAddr + 255 ) % 0x100;
    }
    return TRUE;
}
WORD GetAddrValue( void )
{
    return MSAddr;
}
Bool AdjustRegValue( MenuItemActionType action )
{
    if( action == MIA_IncValue )
    {
        MSValue = ( MSValue + 1 ) % 0x100;
    }
    else
    {
        MSValue = ( MSValue + 255 ) % 0x100;
    }
    return TRUE;
}
WORD GetAddrRegValue( void )
{
    if( MSBankNo == 0 )        // scaler
    {
        MSValue = old_msReadByte( MSAddr );
    }
    else if( MSBankNo == 1 )        // adc
    {
        // wait for coding
        //old_msWriteByte( BK0_00, REGBANK1 );
        MSValue = old_msReadByte( MSAddr );
        //old_msWriteByte( BK0_00, REGBANK0 );
    }
    else if( MSBankNo == 2 )        // TCON
    {
        // wait for coding
        //old_msWriteByte( BK0_00, REGBANK2 );
        MSValue = old_msReadByte( MSAddr );
        //old_msWriteByte( BK0_00, REGBANK0 );
    }
    return MSValue;
}
WORD GetRegValue( void )
{
    return MSValue;
}
Bool WriteRegValue( void )
{
    if( MSBankNo == 0 )        // scaler
    {
        old_msWriteByte( MSAddr, MSValue );
    }
    else if( MSBankNo == 1 )        // adc
    {
        // wait for coding
        //old_msWriteByte( BK0_00, REGBANK1 );
        old_msWriteByte( MSAddr, MSValue );
        //old_msWriteByte( BK0_00, REGBANK0 );
    }
    else if( MSBankNo == 2 )        // TCON
    {
        // wait for coding
        //old_msWriteByte( BK0_00, REGBANK2 );
        old_msWriteByte( MSAddr, MSValue );
        //old_msWriteByte( BK0_00, REGBANK0 );
    }
    return TRUE;
}
Bool EnterDisplayLogo( void )
{
    //printMsg("load");
    //Osd_LoadLogoFontCP();
#if Enable_PanelHandler
    BYTE i = 0;
    for(i = 0; i < ePANEL_STATE_MAX; i++)
    {
        Power_PanelCtrlStateStopFlag_Clr(i);
    }
    Power_PanelCtrlOnOff(TRUE, FALSE);
#else
    #if Enable_ReducePanelPowerOnTime
    Power_ForcePowerOnPanel();
    #else
    Power_TurnOnPanel();
    #endif
#endif
    OsdCounter = 3;
    //printMsg("turn on");
    return TRUE;
}
Bool OsdCountDown( void )
{
    OsdCounter = UserPrefOsdTime; //3;
    return TRUE;
}

#endif
WORD GetPanelOnTimeValue( void )
{
    WORD hour;
    if( BlacklitTime > 235926000ul )
        // 655365*3600
        hour = 0xFFFF;
    else
        hour = ( DWORD )BlacklitTime / 3600;
    return hour;
}

/*
WORD GetSSCStepValue( void )
{
    WORD StepValue;
    StepValue=old_msRead2Byte(BK0_D8);
    return StepValue;
}
WORD GetSSCSpanValue( void )
{
    WORD SpanValue;
    SpanValue=old_msRead2Byte(BK0_DA);
    return SpanValue;
}
*/
#if ENABLE_FACTORY_SSCADJ
Bool AdjustFactorySSCModulation( MenuItemActionType action )
{
    WORD xdata tempValue;

    if( g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE )
        tempValue = DecIncValue( action, FactorySetting.SSCModulation, 0, PANEL_SSC_MODULATION_MAX_EDP, 1 );
    else if( g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_NONE )
        tempValue = DecIncValue( action, FactorySetting.SSCModulation, 0, PANEL_SSC_MODULATION_MAX, 1 );
    else
        tempValue = DecIncValue( action, FactorySetting.SSCModulation, 0, PANEL_SSC_MODULATION_MAX_VX1, 1 );

    if( tempValue == FactorySetting.SSCModulation )
        return FALSE;

    FactorySetting.SSCModulation = tempValue;

    mStar_SetPanelSSC(FactorySetting.SSCModulation, FactorySetting.SSCPercentage);

    #if USEFLASH
    Set_FlashForceSaveFactoryFlag();
    #else
    NVRam_WriteByte( nvrFactoryAddr( SSCModulation ), FactorySetting.SSCModulation );
    #endif

    return TRUE;
}

WORD GetFactorySSCModulationValue( void )
{
    return FactorySetting.SSCModulation;
}

Bool AdjustFactorySSCPercentage( MenuItemActionType action )
{
    WORD xdata tempValue;

    if( g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE )
        tempValue = DecIncValue( action, FactorySetting.SSCPercentage, 0, PANEL_SSC_PERCENTAGE_MAX_EDP, 1 );
    else if( g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_NONE )
        tempValue = DecIncValue( action, FactorySetting.SSCPercentage, 0, PANEL_SSC_PERCENTAGE_MAX, 1 );
    else
        tempValue = DecIncValue( action, FactorySetting.SSCPercentage, 0, PANEL_SSC_PERCENTAGE_MAX_VX1, 1 );

    if( tempValue == FactorySetting.SSCPercentage )
        return FALSE;

    FactorySetting.SSCPercentage = tempValue;

    mStar_SetPanelSSC(FactorySetting.SSCModulation, FactorySetting.SSCPercentage);
    #if USEFLASH
    Set_FlashForceSaveFactoryFlag();
    #else
    NVRam_WriteByte( nvrFactoryAddr( SSCPercentage ), FactorySetting.SSCPercentage );
    #endif
    return TRUE;
}

WORD GetFactorySSCPercentageValue( void )
{
    return FactorySetting.SSCPercentage;
}
#endif

#if Enable_Expansion
void SetExpansionMode( void )
{
    {
        #if UseINT
        mStar_EnableModeChangeINT(FALSE);
        #endif
		#if DECREASE_V_SCALING
		DecVScaleValue = 0;
		#endif
        mStar_SetupMode();
        #if UseINT
        mStar_EnableModeChangeINT(TRUE);
        #endif
    }
}
Bool OSDSetExpansionMode( void )
{

    if (UserprefExpansionMode== MenuItemIndex)
        return FALSE;
    UserprefExpansionMode = MenuItemIndex;

    OverScanSetting.AspRatio = UserprefExpansionMode; //20150728
    mStar_BlackWhiteScreenCtrl(BW_SCREEN_BLACK);
    SetExpansionMode();
    mStar_BlackWhiteScreenCtrl(BW_SCREEN_OFF);
    Set_SaveMonitorSettingFlag();
    return TRUE;
}
#endif
#if 0//ENABLE_RTE
WORD GetOverDriveValue( void )
{
    return ( OverDriveOnFlag ) ? ( 1 ) : ( 0 );
}
Bool AdjustOverDrive( MenuItemActionType action )
{
    action = action;
    if( OverDriveOnFlag )
    {
        Clr_OverDriveOnFlag();
        msOverDriveOnOff( FALSE );
    }
    else
    {
        Set_OverDriveOnFlag();
        msOverDriveOnOff( TRUE );
    }
#if USEFLASH
    //SaveMonitorSetting();
    Set_FlashSaveMonitorFlag();
#else
    NVRam_WriteByte( nvrMonitorAddr( MonitorFlag ), MonitorFlags );
#endif
    Delay1ms( 200 );
    return TRUE;
}
#endif
#if ENABLE_RTE
Bool SetOverDriveOnOffmode(void)
{
    if (UserprefOverDriveSwitch == MenuItemIndex)
        return FALSE;
    UserprefOverDriveSwitch = MenuItemIndex;
    if( UserprefOverDriveSwitch )
    {
        msAPI_OverDriveEnable(TRUE);
    }
    else
    {
        msAPI_OverDriveEnable(FALSE);
    }
    Set_SaveMonitorSettingFlag();
    return TRUE;
}
#endif

#if PanelRSDS
WORD GetVCOMValue( void )
{
    return UserPrefVcomValue;
}

Bool AdjustVCOMValue( MenuItemActionType action )
{
    BYTE tempBank = old_msReadByte( BK0_00 );
    old_msWriteByte( BK0_00, REGBANK0 );

    if( action == MIA_IncValue )
    {
        UserPrefVcomValue = UserPrefVcomValue + 1;
    }
    else
    {
        UserPrefVcomValue = UserPrefVcomValue + 255;
    }
    old_msWriteByte( BK0_C5, UserPrefVcomValue );

    old_msWriteByte( BK0_00, tempBank );
    return TRUE;
}
#endif

#if ENABLE_DeBlocking
void msDeBlockingOnOff( BYTE ucSwitch, BYTE win)
{
    win=win;
	ucSwitch=ucSwitch;
    //old_msWriteByteMask(SC2_20,  (ucSwitch)?(BIT0):(0), BIT0);  //BIT0:enable
}

void msInitDeBlocking( void )
{
    BYTE i=0;
    //SC2_60[3]=1: motion debug mode
    //SC2_E0[3]=0:SPF all bypass
    //old_msWriteByteMask(SC2_E0, 0x00        ,BIT3|BIT2|BIT1|BIT0);
    //old_msWriteByteMask(SC2_E0, 0x00        ,BIT7);

    //old_msWriteByteMask(SC2_80, BIT0        ,BIT7|BIT6|BIT0);  //BIT7:debug mode ,BIT6: iir mode BIT0:blockiness_en_f2
    //old_msWriteByteMask(SC2_80,   BIT3|BIT2   ,BIT3|BIT2);  //De-blocking coarse detect step F2
    //old_msWriteByteMask(SC2_81, 0x30    ,0xFF);  //De-blocking coarse active threshold F2

    MenuFunc_printMsg("msInitDeBlocking");
}
#endif
#if ENABLE_SHARPNESS
//=============================================================================
#define MaxSharpness    0x1F   //user sharpness adjust gain: 0x10~0x1F==>1.0~ 1.9
#define MinSharpness    0x00     //user sharpness adjust gain  0x00~0x0F==>0.0~ 0.9

Bool AdjustSharpness( MenuItemActionType action )
{
    WORD tempValue;

    tempValue = DecIncValue( action, UserPrefSharpness, MinSharpness, MaxSharpness, 1 );
    if( tempValue == UserPrefSharpness )
    {
        return FALSE;
    }
    UserPrefSharpness = tempValue;

#if ENABLE_SUPER_RESOLUTION
    mStar_SetupSuperResolution(UserPrefSuperResolutionMode);
#endif
    msAdjustSharpness( MAIN_WINDOW, UserPrefSharpness, 0);

    return TRUE;
}
WORD GetSharpnessValue( void )
{
    return GetScale100Value( UserPrefSharpness, MinSharpness, MaxSharpness );
}
#endif

void MenuFuncuncall(void)
{
    AdjustDcrMode(0);
    AdjustGamaMode(0);
#if ENABLE_DPS
    AdjustDPSMode(0);
#endif
#if ENABLE_DLC
    AdjustDLCMode(0);
#endif
#if ENABLE_SUPER_RESOLUTION
    AdjustSuperResolutionMode(0);
#endif
    AdjustLanguage(0);
#if ENABLE_SHARPNESS
//    GetSharpnessValue();
#endif

#if ENABLE_FREESYNC
    GetSrcFreesyncFPS();
#endif
}


#if ENABLE_OSD_ROTATION
Bool SetOSDRotateMode(void)
{
    if (UserPrefOsdRotateMode == MenuItemIndex)
        return FALSE;
    UserPrefOsdRotateMode = MenuItemIndex;

    if (UserPrefOsdRotateMode!=0)
        SET_MENU_ROTATION_FLAG();
    else
        CLR_MENU_ROTATION_FLAG();

//Marked for OSD rotate garbage
/*
    if ( UserPrefOsdRotateMode==OSDRotateMenuItems_90 )
    {
        drvOSD_SetWndCABaseAddr ( OSD_MAIN_WND, GET_CABASE_0_90 ( OSD_MAIN_WND_CA_BASE, OsdWindowWidth ) );
        Osd_SetPosition ( UserPrefHStart, UserPrefVStart );
    }
    #if defined (_OSD_ROTATION_180_)
    if ( UserPrefOsdRotateMode==OSDRotateMenuItems_180 )
    {
        drvOSD_SetWndCABaseAddr ( OSD_MAIN_WND, GET_CABASE_0_180 ( OSD_MAIN_WND_CA_BASE, OsdWindowWidth, OsdWindowHeight ) );
        Osd_SetPosition ( UserPrefHStart, UserPrefVStart );
    }
    else
    #endif
    #if defined (_OSD_ROTATION_270_)
    if ( UserPrefOsdRotateMode==OSDRotateMenuItems_270 )
    {
        drvOSD_SetWndCABaseAddr ( OSD_MAIN_WND, GET_CABASE_0_270 ( OSD_MAIN_WND_CA_BASE, OsdWindowWidth, OsdWindowHeight ) );
        Osd_SetPosition ( UserPrefHStart, UserPrefVStart );
    }
    else
    #endif
    {
        drvOSD_SetWndCABaseAddr ( OSD_MAIN_WND, OSD_MAIN_WND_CA_BASE );
        Osd_SetPosition ( UserPrefOsdHStart, UserPrefOsdVStart);
    }
*/
    Set_SaveMonitorSettingFlag();
    return TRUE;
}
#endif

#if ENABLE_FREESYNC
BYTE GetSrcFreesyncFPS(void)
{
    BYTE xdata u8Vfreq = 0xFF;
    WORD xdata u16TimeOutCnt;
    DWORD xdata u32IvsPeriod;
    #if CHIP_ID != CHIP_MT9701
    DWORD xdata u8DivN;
    WORD xdata u16IvsPrd_L;
    BYTE xdata u8IvsPrd_H,u8DivN_L,u8DivN_H;
    #endif

    u16TimeOutCnt = 600;
    while(u16TimeOutCnt!=0)
    {
        u16TimeOutCnt--;
        #if CHIP_ID == CHIP_MT9701
        u32IvsPeriod = msDrvGetIVSPeriod();
        #else
        u16IvsPrd_L = msRead2Byte(REG_LPLL_42);
        u8IvsPrd_H = msReadByte(REG_LPLL_44);

        u8DivN_L = msReadByte(REG_LPLL_19);
        u8DivN_H = msReadByte(REG_LPLL_1C);
        u8DivN=(DWORD)(((u8DivN_H&BIT0)<<4)|(u8DivN_L&0x0F));
        u32IvsPeriod = ((DWORD)u8IvsPrd_H<<16)|u16IvsPrd_L;
        #endif
        if(u32IvsPeriod!=0)
        {
            u8Vfreq = (((CRYSTAL_CLOCK)+(u32IvsPeriod>>1))/u32IvsPeriod);
            break;
        }
    }
    return u8Vfreq;
}
Bool SetDRRFunction(void)
{
    if (UserprefFreeSyncMode == MenuItemIndex)
        return FALSE;
    UserprefFreeSyncMode = MenuItemIndex;
    Set_SaveMonitorSettingFlag();

    msAPI_combo_IPEnableDDRFlag(UserprefFreeSyncMode);
    return TRUE;
}
#endif
Bool OSD_SetColorTemp(void)
{
    if (UserPrefColorTemp== MenuItemIndex)
        return FALSE;
    UserPrefColorTemp = MenuItemIndex;

    SetColorTemp();

    Set_SaveMonitorSettingFlag();
    return TRUE;
}

Bool AdjustVideoSaturation(MenuItemActionType enAction)
{
    WORD u16TempValue;

    u16TempValue = DecIncValue(enAction, UserPrefSaturation, 0, 0xFF, 1);



    if (u16TempValue == UserPrefSaturation)
        return FALSE;

    UserPrefSaturation = u16TempValue;
#if (GLOBL_HSL_IP_SEL == GLOBL_HSL_IP_HSY)
	msAPI_Adjust_Sat(0, GetHSYMappingValue(1, (UserPrefSaturation*UserPrefIndependentSaturationR)/128));
	msAPI_Adjust_Sat(1, GetHSYMappingValue(1, (UserPrefSaturation*UserPrefIndependentSaturationG)/128));
	msAPI_Adjust_Sat(2, GetHSYMappingValue(1, (UserPrefSaturation*UserPrefIndependentSaturationB)/128));
	msAPI_Adjust_Sat(3, GetHSYMappingValue(1, (UserPrefSaturation*UserPrefIndependentSaturationC)/128));
	msAPI_Adjust_Sat(4, GetHSYMappingValue(1, (UserPrefSaturation*UserPrefIndependentSaturationM)/128));
	msAPI_Adjust_Sat(5, GetHSYMappingValue(1, (UserPrefSaturation*UserPrefIndependentSaturationY)/128));
#else
    AdjustVideoHueSaturation();
#endif
    return TRUE;
}
WORD GetSaturationValue( void )
{
    WORD temp;
    temp = GetScale100Value( UserPrefSaturation, 0x00, 0xFF );
    return temp;
}


Bool AdjustVideoHUE(MenuItemActionType enAction)
{
    WORD u16TempValue;

    u16TempValue = DecIncValue(enAction, UserPrefHue, 0, 100, 1);

    if (u16TempValue == UserPrefHue)
        return FALSE;

    UserPrefHue = u16TempValue;
#if (GLOBL_HSL_IP_SEL == GLOBL_HSL_IP_HSY)
	msAPI_Adjust_Hue(0, GetHSYMappingValue(0, (UserPrefHue+UserPrefIndependentHueR)/2));
	msAPI_Adjust_Hue(1, GetHSYMappingValue(0, (UserPrefHue+UserPrefIndependentHueG)/2));
	msAPI_Adjust_Hue(2, GetHSYMappingValue(0, (UserPrefHue+UserPrefIndependentHueB)/2));
	msAPI_Adjust_Hue(3, GetHSYMappingValue(0, (UserPrefHue+UserPrefIndependentHueC)/2));
	msAPI_Adjust_Hue(4, GetHSYMappingValue(0, (UserPrefHue+UserPrefIndependentHueM)/2));
	msAPI_Adjust_Hue(5, GetHSYMappingValue(0, (UserPrefHue+UserPrefIndependentHueY)/2));
#else
    AdjustVideoHueSaturation();
#endif

    return TRUE;
}
WORD GetHueValue( void )
{
    WORD temp;
    temp = GetScale100Value( UserPrefHue, 0x00, 100 );
    return temp;
}

Bool SetGammaOnOff(void)
{
    if (UserPrefGamaOnOff == MenuItemIndex)
        return FALSE;
    UserPrefGamaOnOff = MenuItemIndex;

	mStar_WaitForDataBlanking();//avoid noise

//TBD
#if (CHIP_ID == CHIP_MT9701)
mdrv_Adjust_DS_Start();
if(UserPrefGamaOnOff)
{
	extern code BYTE *tAllPostGammaTbl[][3];

    msAPI_GammaLoadTbl_1024E_12B_N(MAIN_WINDOW,  (BYTE**)tAllPostGammaTbl[0]);
}
    drvGammaOnOff(UserPrefGamaOnOff, MAIN_WINDOW);
    drvGammaOnOff(UserPrefGamaOnOff, SUB_WINDOW);
mdrv_Adjust_DS_End();
#else
    drvGammaOnOff(UserPrefGamaOnOff, MAIN_WINDOW);
    drvGammaOnOff(UserPrefGamaOnOff, SUB_WINDOW);
#endif
    Set_SaveMonitorSettingFlag();
    return TRUE;
}

Bool AdjustIndependentHue_R(MenuItemActionType enAction)
{
    WORD u16TempValue;
    int u32Temp;

    u16TempValue = DecIncValue(enAction, UserPrefIndependentHueR, 0, 100, 1);

    if (u16TempValue == UserPrefIndependentHueR)
        return FALSE;

    UserPrefIndependentHueR = u16TempValue;

#if ((INDEP_HSL_IP_SEL == INDEP_HSL_IP_HSY) && (GLOBL_HSL_IP_SEL == GLOBL_HSL_IP_HSY))
		u32Temp = GetHSYMappingValue(0, (UserPrefHue+UserPrefIndependentHueR)/2);
#else
	#if (INDEP_HSL_IP_SEL == INDEP_HSL_IP_HSY)
    	u32Temp = GetHSYMappingValue(0, u16TempValue);
	#else
		u32Temp = GetHSYMappingValue(0, 100-u16TempValue);
	#endif
#endif

    msAPI_Adjust_Hue(0, u32Temp);
    return TRUE;
}
WORD GetIndependentHueRValue( void )
{
    WORD temp;
    temp = GetScale100Value( UserPrefIndependentHueR, 0x00, 100 );
    return temp;
}

Bool AdjustIndependentHue_G(MenuItemActionType enAction)
{
    WORD u16TempValue;
    int u32Temp;

    u16TempValue = DecIncValue(enAction, UserPrefIndependentHueG, 0, 100, 1);

    if (u16TempValue == UserPrefIndependentHueG)
        return FALSE;

    UserPrefIndependentHueG = u16TempValue;

#if ((INDEP_HSL_IP_SEL == INDEP_HSL_IP_HSY) && (GLOBL_HSL_IP_SEL == GLOBL_HSL_IP_HSY))
		u32Temp = GetHSYMappingValue(0, (UserPrefHue+UserPrefIndependentHueG)/2);
#else
	#if (INDEP_HSL_IP_SEL == INDEP_HSL_IP_HSY)
    u32Temp = GetHSYMappingValue(0, u16TempValue);
	#else
			u32Temp = GetHSYMappingValue(0, 100-u16TempValue);
	#endif
#endif

    msAPI_Adjust_Hue(1, u32Temp);
    return TRUE;
}
WORD GetIndependentHueGValue( void )
{
    WORD temp;
    temp = GetScale100Value( UserPrefIndependentHueG, 0x00, 100 );
    return temp;
}

Bool AdjustIndependentHue_B(MenuItemActionType enAction)
{
    WORD u16TempValue;
    int u32Temp;

    u16TempValue = DecIncValue(enAction, UserPrefIndependentHueB, 0, 100, 1);

    if (u16TempValue == UserPrefIndependentHueB)
        return FALSE;

    UserPrefIndependentHueB = u16TempValue;

#if ((INDEP_HSL_IP_SEL == INDEP_HSL_IP_HSY) && (GLOBL_HSL_IP_SEL == GLOBL_HSL_IP_HSY))
		u32Temp = GetHSYMappingValue(0, (UserPrefHue+UserPrefIndependentHueB)/2);
#else
	#if (INDEP_HSL_IP_SEL == INDEP_HSL_IP_HSY)
    u32Temp = GetHSYMappingValue(0, u16TempValue);
	#else
			u32Temp = GetHSYMappingValue(0, 100-u16TempValue);
	#endif
#endif

	msAPI_Adjust_Hue(2, u32Temp);

    return TRUE;
}
WORD GetIndependentHueBValue( void )
{
    WORD temp;
    temp = GetScale100Value( UserPrefIndependentHueB, 0x00, 100 );
    return temp;
}

Bool AdjustIndependentHue_C(MenuItemActionType enAction)
{
    WORD u16TempValue;
    int u32Temp;

    u16TempValue = DecIncValue(enAction, UserPrefIndependentHueC, 0, 100, 1);

    if (u16TempValue == UserPrefIndependentHueC)
        return FALSE;

    UserPrefIndependentHueC = u16TempValue;

#if ((INDEP_HSL_IP_SEL == INDEP_HSL_IP_HSY) && (GLOBL_HSL_IP_SEL == GLOBL_HSL_IP_HSY))
			u32Temp = GetHSYMappingValue(0, (UserPrefHue+UserPrefIndependentHueC)/2);
#else
    u32Temp = GetHSYMappingValue(0, u16TempValue);
#endif

	msAPI_Adjust_Hue(3, u32Temp);
    return TRUE;
}
WORD GetIndependentHueCValue( void )
{
    WORD temp;
    temp = GetScale100Value( UserPrefIndependentHueC, 0x00, 100 );
    return temp;
}

Bool AdjustIndependentHue_M(MenuItemActionType enAction)
{
    WORD u16TempValue;
    int u32Temp;

    u16TempValue = DecIncValue(enAction, UserPrefIndependentHueM, 0, 100, 1);

    if (u16TempValue == UserPrefIndependentHueM)
        return FALSE;

    UserPrefIndependentHueM = u16TempValue;

#if ((INDEP_HSL_IP_SEL == INDEP_HSL_IP_HSY) && (GLOBL_HSL_IP_SEL == GLOBL_HSL_IP_HSY))
				u32Temp = GetHSYMappingValue(0, (UserPrefHue+UserPrefIndependentHueM)/2);
#else
    u32Temp = GetHSYMappingValue(0, u16TempValue);
#endif


    msAPI_Adjust_Hue(4, u32Temp);
    return TRUE;
}
WORD GetIndependentHueMValue( void )
{
    WORD temp;
    temp = GetScale100Value( UserPrefIndependentHueM, 0x00, 100 );
    return temp;
}

Bool AdjustIndependentHue_Y(MenuItemActionType enAction)
{
    WORD u16TempValue;
    int u32Temp;

    u16TempValue = DecIncValue(enAction, UserPrefIndependentHueY, 0, 100, 1);

    if (u16TempValue == UserPrefIndependentHueY)
        return FALSE;

    UserPrefIndependentHueY = u16TempValue;

#if ((INDEP_HSL_IP_SEL == INDEP_HSL_IP_HSY) && (GLOBL_HSL_IP_SEL == GLOBL_HSL_IP_HSY))
				u32Temp = GetHSYMappingValue(0, (UserPrefHue+UserPrefIndependentHueY)/2);
#else
    u32Temp = GetHSYMappingValue(0, u16TempValue);
#endif

    msAPI_Adjust_Hue(5, u32Temp);
    return TRUE;
}
WORD GetIndependentHueYValue( void )
{
    WORD temp;
    temp = GetScale100Value( UserPrefIndependentHueY, 0x00, 100 );
    return temp;
}

Bool AdjustIndependentSaturation_R(MenuItemActionType enAction)
{
    WORD u16TempValue;
    int u32Temp;

    u16TempValue = DecIncValue(enAction, UserPrefIndependentSaturationR, 0, 0xFF, 1);

    if (u16TempValue == UserPrefIndependentSaturationR)
        return FALSE;

    UserPrefIndependentSaturationR = u16TempValue;
#if (GLOBL_HSL_IP_SEL == GLOBL_HSL_IP_HSY)
    u32Temp = GetHSYMappingValue(1, (UserPrefSaturation*u16TempValue)/128);
#else
    u32Temp = GetHSYMappingValue(1, u16TempValue);
#endif

    msAPI_Adjust_Sat(0, u32Temp);
    return TRUE;
}
WORD GetIndependentSaturationRValue( void )
{
    WORD temp;
    temp = GetScale100Value( UserPrefIndependentSaturationR, 0x00, 0xFF );
    return temp;
}

Bool AdjustIndependentSaturation_G(MenuItemActionType enAction)
{
    WORD u16TempValue;
    int u32Temp;

    u16TempValue = DecIncValue(enAction, UserPrefIndependentSaturationG, 0, 0xFF, 1);

    if (u16TempValue == UserPrefIndependentSaturationG)
        return FALSE;

    UserPrefIndependentSaturationG = u16TempValue;

#if (GLOBL_HSL_IP_SEL == GLOBL_HSL_IP_HSY)
		u32Temp = GetHSYMappingValue(1, (UserPrefSaturation*u16TempValue)/128);
#else
    u32Temp = GetHSYMappingValue(1, u16TempValue);
#endif

	msAPI_Adjust_Sat(1, u32Temp);
    return TRUE;
}
WORD GetIndependentSaturationGValue( void )
{
    WORD temp;
    temp = GetScale100Value( UserPrefIndependentSaturationG, 0x00, 0xFF );
    return temp;
}

Bool AdjustIndependentSaturation_B(MenuItemActionType enAction)
{
    WORD u16TempValue;
    int u32Temp;

    u16TempValue = DecIncValue(enAction, UserPrefIndependentSaturationB, 0, 0xFF, 1);

    if (u16TempValue == UserPrefIndependentSaturationB)
        return FALSE;

    UserPrefIndependentSaturationB = u16TempValue;

#if (GLOBL_HSL_IP_SEL == GLOBL_HSL_IP_HSY)
		u32Temp = GetHSYMappingValue(1, (UserPrefSaturation*u16TempValue)/128);
#else
    u32Temp = GetHSYMappingValue(1, u16TempValue);
#endif

	msAPI_Adjust_Sat(2, u32Temp);
    return TRUE;
}
WORD GetIndependentSaturationBValue( void )
{
    WORD temp;
    temp = GetScale100Value( UserPrefIndependentSaturationB, 0x00, 0xFF );
    return temp;
}

Bool AdjustIndependentSaturation_C(MenuItemActionType enAction)
{
    WORD u16TempValue;
    int u32Temp;

    u16TempValue = DecIncValue(enAction, UserPrefIndependentSaturationC, 0, 0xFF, 1);

    if (u16TempValue == UserPrefIndependentSaturationC)
        return FALSE;

    UserPrefIndependentSaturationC = u16TempValue;

#if (GLOBL_HSL_IP_SEL == GLOBL_HSL_IP_HSY)
		u32Temp = GetHSYMappingValue(1, (UserPrefSaturation*u16TempValue)/128);
#else
    u32Temp = GetHSYMappingValue(1, u16TempValue);
#endif

	msAPI_Adjust_Sat(3, u32Temp);
    return TRUE;
}
WORD GetIndependentSaturationCValue( void )
{
    WORD temp;
    temp = GetScale100Value( UserPrefIndependentSaturationC, 0x00, 0xFF );
    return temp;
}

Bool AdjustIndependentSaturation_M(MenuItemActionType enAction)
{
    WORD u16TempValue;
    int u32Temp;

    u16TempValue = DecIncValue(enAction, UserPrefIndependentSaturationM, 0, 0xFF, 1);

    if (u16TempValue == UserPrefIndependentSaturationM)
        return FALSE;

    UserPrefIndependentSaturationM = u16TempValue;

#if (GLOBL_HSL_IP_SEL == GLOBL_HSL_IP_HSY)
		u32Temp = GetHSYMappingValue(1, (UserPrefSaturation*u16TempValue)/128);
#else
    u32Temp = GetHSYMappingValue(1, u16TempValue);
#endif

	msAPI_Adjust_Sat(4, u32Temp);
    return TRUE;
}
WORD GetIndependentSaturationMValue( void )
{
    WORD temp;
    temp = GetScale100Value( UserPrefIndependentSaturationM, 0x00, 0xFF );
    return temp;
}

Bool AdjustIndependentSaturation_Y(MenuItemActionType enAction)
{
    WORD u16TempValue;
    int u32Temp;

    u16TempValue = DecIncValue(enAction, UserPrefIndependentSaturationY, 0, 0xFF, 1);

    if (u16TempValue == UserPrefIndependentSaturationY)
        return FALSE;

    UserPrefIndependentSaturationY = u16TempValue;

#if (GLOBL_HSL_IP_SEL == GLOBL_HSL_IP_HSY)
		u32Temp = GetHSYMappingValue(1, (UserPrefSaturation*u16TempValue)/128);
#else
    u32Temp = GetHSYMappingValue(1, u16TempValue);
#endif

	msAPI_Adjust_Sat(5, u32Temp);
    return TRUE;
}
WORD GetIndependentSaturationYValue( void )
{
    WORD temp;
    temp = GetScale100Value( UserPrefIndependentSaturationY, 0x00, 0xFF );
    return temp;
}

Bool AdjustIndependentBrightness_R(MenuItemActionType enAction)
{
    WORD u16TempValue;
    int u32Temp;

    u16TempValue = DecIncValue(enAction, UserPrefIndependentBrightnessR, 0, 100, 1);

    if (u16TempValue == UserPrefIndependentBrightnessR)
        return FALSE;

    UserPrefIndependentBrightnessR = u16TempValue;

    u32Temp = GetHSYMappingValue(2, u16TempValue);
    msAPI_Adjust_Bri(0, u32Temp);
    return TRUE;
}
WORD GetIndependentBrightnessRValue( void )
{
    WORD temp;
    temp = GetScale100Value( UserPrefIndependentBrightnessR, 0x00, 100 );
    return temp;
}

Bool AdjustIndependentBrightness_G(MenuItemActionType enAction)
{
    WORD u16TempValue;
    int u32Temp;

    u16TempValue = DecIncValue(enAction, UserPrefIndependentBrightnessG, 0, 100, 1);

    if (u16TempValue == UserPrefIndependentBrightnessG)
        return FALSE;

    UserPrefIndependentBrightnessG = u16TempValue;

    u32Temp = GetHSYMappingValue(2, u16TempValue);

    msAPI_Adjust_Bri(1, u32Temp);
    return TRUE;
}
WORD GetIndependentBrightnessGValue( void )
{
    WORD temp;
    temp = GetScale100Value( UserPrefIndependentBrightnessG, 0x00, 100 );
    return temp;
}

Bool AdjustIndependentBrightness_B(MenuItemActionType enAction)
{
    WORD u16TempValue;
    int u32Temp;

    u16TempValue = DecIncValue(enAction, UserPrefIndependentBrightnessB, 0, 100, 1);

    if (u16TempValue == UserPrefIndependentBrightnessB)
        return FALSE;

    UserPrefIndependentBrightnessB = u16TempValue;

    u32Temp = GetHSYMappingValue(2, u16TempValue);

    msAPI_Adjust_Bri(2, u32Temp);
    return TRUE;
}
WORD GetIndependentBrightnessBValue( void )
{
    WORD temp;
    temp = GetScale100Value( UserPrefIndependentBrightnessB, 0x00, 100 );
    return temp;
}

Bool AdjustIndependentBrightness_C(MenuItemActionType enAction)
{
    WORD u16TempValue;
    int u32Temp;

    u16TempValue = DecIncValue(enAction, UserPrefIndependentBrightnessC, 0, 100, 1);

    if (u16TempValue == UserPrefIndependentBrightnessC)
        return FALSE;

    UserPrefIndependentBrightnessC = u16TempValue;

    u32Temp = GetHSYMappingValue(2, u16TempValue);

	msAPI_Adjust_Bri(3, u32Temp);
    return TRUE;
}
WORD GetIndependentBrightnessCValue( void )
{
    WORD temp;
    temp = GetScale100Value( UserPrefIndependentBrightnessC, 0x00, 100 );
    return temp;
}

Bool AdjustIndependentBrightness_M(MenuItemActionType enAction)
{
    WORD u16TempValue;
    int u32Temp;

    u16TempValue = DecIncValue(enAction, UserPrefIndependentBrightnessM, 0, 100, 1);

    if (u16TempValue == UserPrefIndependentBrightnessM)
        return FALSE;

    UserPrefIndependentBrightnessM = u16TempValue;

    u32Temp = GetHSYMappingValue(2, u16TempValue);

	msAPI_Adjust_Bri(4, u32Temp);
    return TRUE;
}
WORD GetIndependentBrightnessMValue( void )
{
    WORD temp;
    temp = GetScale100Value( UserPrefIndependentBrightnessM, 0x00, 100 );
    return temp;
}

Bool AdjustIndependentBrightness_Y(MenuItemActionType enAction)
{
    WORD u16TempValue;
    int u32Temp;

    u16TempValue = DecIncValue(enAction, UserPrefIndependentBrightnessY, 0, 100, 1);

    if (u16TempValue == UserPrefIndependentBrightnessY)
        return FALSE;

    UserPrefIndependentBrightnessY = u16TempValue;

    u32Temp = GetHSYMappingValue(2, u16TempValue);

    msAPI_Adjust_Bri(5, u32Temp);
    return TRUE;
}
WORD GetIndependentBrightnessYValue( void )
{
    WORD temp;
    temp = GetScale100Value( UserPrefIndependentBrightnessY, 0x00, 100 );
    return temp;
}

Bool SetColorRange(void)
{
    if (UserPrefInputColorRange == MenuItemIndex)
        return FALSE;
    UserPrefInputColorRange = MenuItemIndex;

    printf("UserPrefInputColorRange : %d\n", UserPrefInputColorRange);

#if CHIP_ID == CHIP_MT9701
    msACESetWinColorRange(TRUE, (E_WIN_COLOR_RANGE)UserPrefInputColorRange);
#else
#if ENABLE_FULL_RGB_COLOR_PATH
    msACECSCControl();
#endif
    //mStar_AdjustContrast( UserPrefContrast );
    msACESetWinColorRange(TRUE, (E_WIN_COLOR_RANGE)UserPrefInputColorRange);
#endif
	 
    Set_SaveMonitorSettingFlag();
    return TRUE;
}

Bool SetColorMode(void)
{
    if (UserPrefColorMode == MenuItemIndex)
        return FALSE;
    UserPrefColorMode = MenuItemIndex;

    if (UserPrefColorMode == ColorModeMenuItems_Enhance)
        SetColorModeEnhance();
    else if (UserPrefColorMode == ColorModeMenuItems_Demo)
        SetColorModeDemo();
    else
        SetColorModeOFF();

    Set_SaveMonitorSettingFlag();
    return TRUE;
}
Bool SetColorModeOFF(void)
{
    //printf("Color Mode: 0FF\n");
    //Disable Gamma
    //msWriteBit(SC0B_20, TRUE, _BIT7);//TRUE for disable MWE function (2D peaking line-buffer sram active)
    drvGammaOnOff(FALSE, MAIN_WINDOW);
    drvGammaOnOff(FALSE, SUB_WINDOW);

    //Disable Color Temp.
    //mStar_AdjustBrightness(0x80);
    mStar_AdjustContrast(0x80);

    #if (CHIP_ID == CHIP_MT9701)
    //Disable Sub Win (MWE)
    mdrv_ACE_MweEnable_Set(FALSE);
    #endif
    //msWriteBit(SC00_27, TRUE, _BIT7); // enable auto ouput V sync
    return TRUE;
}

Bool SetColorModeEnhance(void)
{
    //printf("Color Mode: Enhance\n");
    //Gamma + Color Temp. D65
    //msWriteBit(SC0B_20, TRUE, _BIT7);//TRUE for disable MWE function (2D peaking line-buffer sram active)
    mStar_AdjustBrightness(UserPrefBrightness);
    mStar_AdjustContrast(UserPrefContrast);

    mStar_WaitForDataBlanking();

    drvGammaOnOff(_ENABLE, MAIN_WINDOW );
    drvGammaOnOff(_DISABLE, SUB_WINDOW);

    #if (CHIP_ID == CHIP_MT9701)
    //Disable Sub Win (MWE)
    mdrv_ACE_MweEnable_Set(FALSE);
    #endif
    //msWriteBit(SC00_27, TRUE, _BIT7); // enable auto ouput V sync
    return TRUE;
}

int GetHSYMappingValue(BYTE u8Mode, WORD u16Value)
{
    int temp = 0;
    #if HSY_MODE
        switch(u8Mode)
        {
            case 0x00: //Hue (-1023~0~1023)
            {
                if(u16Value < 50)
                    temp = (((50-u16Value)*1023/50)&0x3FF)*-1;
                else if(u16Value >= 50)
                    temp = ((u16Value-50)*1023/50)&0x3FF;
            }
            break;
            case 0x01: //Saturation (0~128~255)
            {
                temp = u16Value&0xFF;
            }
             break;
            case 0x02: //Brightness (-1023~0~1023)
            {
                if(u16Value < 50)
                    temp = (((50-u16Value)*1023/50)&0x3FF)*-1;
                else if(u16Value >= 50)
                    temp = ((u16Value-50)*1023/50)&0x3FF;
            }
            break;
        }
    #else
        //UNUSED(u8Mode);
        if(u8Mode == 0x01)
            u16Value = u16Value*100/255;

        if(u16Value < 50)
        {
            // 2's complement for negative value
            temp = ((50-u16Value)*63/50);
            temp = 0x40 + temp;
        }
        else if(u16Value >= 50)
            temp = ((u16Value-50)*63/50);

    #endif

    return temp;
}

void AdjustAllIndependentHSYValue(void)
{
    #if HSY_MODE
	#if (CHIP_ID==CHIP_MT9700)
    msAPI_Adjust_ICC_Value_Reset();
	#else //(CHIP_ID==CHIP_MT9701)
	msAPI_HSY_Reset(GEN_BY_HW);
	#endif
    #endif

	int u32Temp=0;

#if (CHIP_ID!=CHIP_MT9701)
#if ((INDEP_HSL_IP_SEL == INDEP_HSL_IP_HSY) && (GLOBL_HSL_IP_SEL == GLOBL_HSL_IP_HSY))
	u32Temp = GetHSYMappingValue(0, (UserPrefHue+UserPrefIndependentHueR)/2);
    msAPI_Adjust_Hue(0, u32Temp);
    u32Temp = GetHSYMappingValue(0, (UserPrefHue+UserPrefIndependentHueG)/2);
    msAPI_Adjust_Hue(1, u32Temp);
    u32Temp = GetHSYMappingValue(0, (UserPrefHue+UserPrefIndependentHueB)/2);
    msAPI_Adjust_Hue(2, u32Temp);
    u32Temp = GetHSYMappingValue(0, (UserPrefHue+UserPrefIndependentHueC)/2);
    msAPI_Adjust_Hue(3, u32Temp);
    u32Temp = GetHSYMappingValue(0, (UserPrefHue+UserPrefIndependentHueM)/2);
    msAPI_Adjust_Hue(4, u32Temp);
    u32Temp = GetHSYMappingValue(0, (UserPrefHue+UserPrefIndependentHueY)/2);
    msAPI_Adjust_Hue(5, u32Temp);
#else
	#if (INDEP_HSL_IP_SEL == INDEP_HSL_IP_HSY)
    u32Temp = GetHSYMappingValue(0, UserPrefIndependentHueR);
    msAPI_Adjust_Hue(0, u32Temp);
    u32Temp = GetHSYMappingValue(0, UserPrefIndependentHueG);
    msAPI_Adjust_Hue(1, u32Temp);
    u32Temp = GetHSYMappingValue(0, UserPrefIndependentHueB);
    msAPI_Adjust_Hue(2, u32Temp);
    u32Temp = GetHSYMappingValue(0, UserPrefIndependentHueC);
    msAPI_Adjust_Hue(3, u32Temp);
    u32Temp = GetHSYMappingValue(0, UserPrefIndependentHueM);
    msAPI_Adjust_Hue(4, u32Temp);
    u32Temp = GetHSYMappingValue(0, UserPrefIndependentHueY);
    msAPI_Adjust_Hue(5, u32Temp);
	#else
	u32Temp = GetHSYMappingValue(0, 100-UserPrefIndependentHueR);
    msAPI_Adjust_Hue(0, u32Temp);
    u32Temp = GetHSYMappingValue(0, 100-UserPrefIndependentHueG);
    msAPI_Adjust_Hue(1, u32Temp);
    u32Temp = GetHSYMappingValue(0, 100-UserPrefIndependentHueB);
    msAPI_Adjust_Hue(2, u32Temp);
    u32Temp = GetHSYMappingValue(0, UserPrefIndependentHueC);
    msAPI_Adjust_Hue(3, u32Temp);
    u32Temp = GetHSYMappingValue(0, UserPrefIndependentHueM);
    msAPI_Adjust_Hue(4, u32Temp);
    u32Temp = GetHSYMappingValue(0, UserPrefIndependentHueY);
    msAPI_Adjust_Hue(5, u32Temp);
	#endif
#endif

#if ((INDEP_HSL_IP_SEL == INDEP_HSL_IP_HSY) && (GLOBL_HSL_IP_SEL == GLOBL_HSL_IP_HSY))
    u32Temp = GetHSYMappingValue(1, (UserPrefSaturation*UserPrefIndependentSaturationR)/128);
    msAPI_Adjust_Sat(0, u32Temp);
    u32Temp = GetHSYMappingValue(1, (UserPrefSaturation*UserPrefIndependentSaturationG)/128);
    msAPI_Adjust_Sat(1, u32Temp);
    u32Temp = GetHSYMappingValue(1, (UserPrefSaturation*UserPrefIndependentSaturationB)/128);
    msAPI_Adjust_Sat(2, u32Temp);
    u32Temp = GetHSYMappingValue(1, (UserPrefSaturation*UserPrefIndependentSaturationC)/128);
    msAPI_Adjust_Sat(3, u32Temp);
    u32Temp = GetHSYMappingValue(1, (UserPrefSaturation*UserPrefIndependentSaturationM)/128);
    msAPI_Adjust_Sat(4, u32Temp);
    u32Temp = GetHSYMappingValue(1, (UserPrefSaturation*UserPrefIndependentSaturationY)/128);
    msAPI_Adjust_Sat(5, u32Temp);
#else
    u32Temp = GetHSYMappingValue(1, UserPrefIndependentSaturationR);
    msAPI_Adjust_Sat(0, u32Temp);
    u32Temp = GetHSYMappingValue(1, UserPrefIndependentSaturationG);
    msAPI_Adjust_Sat(1, u32Temp);
    u32Temp = GetHSYMappingValue(1, UserPrefIndependentSaturationB);
    msAPI_Adjust_Sat(2, u32Temp);
    u32Temp = GetHSYMappingValue(1, UserPrefIndependentSaturationC);
    msAPI_Adjust_Sat(3, u32Temp);
    u32Temp = GetHSYMappingValue(1, UserPrefIndependentSaturationM);
    msAPI_Adjust_Sat(4, u32Temp);
    u32Temp = GetHSYMappingValue(1, UserPrefIndependentSaturationY);
    msAPI_Adjust_Sat(5, u32Temp);
#endif
    u32Temp = GetHSYMappingValue(2, UserPrefIndependentBrightnessR);
    msAPI_Adjust_Bri(0, u32Temp);
    u32Temp = GetHSYMappingValue(2, UserPrefIndependentBrightnessG);
    msAPI_Adjust_Bri(1, u32Temp);
    u32Temp = GetHSYMappingValue(2, UserPrefIndependentBrightnessB);
    msAPI_Adjust_Bri(2, u32Temp);
    u32Temp = GetHSYMappingValue(2, UserPrefIndependentBrightnessC);
    msAPI_Adjust_Bri(3, u32Temp);
    u32Temp = GetHSYMappingValue(2, UserPrefIndependentBrightnessM);
    msAPI_Adjust_Bri(4, u32Temp);
    u32Temp = GetHSYMappingValue(2, UserPrefIndependentBrightnessY);
    msAPI_Adjust_Bri(5, u32Temp);
#else//MT9701
	u32Temp = GetHSYMappingValue(0, UserPrefIndependentHueR);
	msAPI_Adjust_Hue(0, u32Temp);
	u32Temp = GetHSYMappingValue(0, UserPrefIndependentHueG);
	msAPI_Adjust_Hue(1, u32Temp);
	u32Temp = GetHSYMappingValue(0, UserPrefIndependentHueB);
	msAPI_Adjust_Hue(2, u32Temp);
	u32Temp = GetHSYMappingValue(0, UserPrefIndependentHueC);
	msAPI_Adjust_Hue(3, u32Temp);
	u32Temp = GetHSYMappingValue(0, UserPrefIndependentHueM);
	msAPI_Adjust_Hue(4, u32Temp);
	u32Temp = GetHSYMappingValue(0, UserPrefIndependentHueY);
	msAPI_Adjust_Hue(5, u32Temp);

    u32Temp = GetHSYMappingValue(1, UserPrefIndependentSaturationR);
    msAPI_Adjust_Sat(0, u32Temp);
    u32Temp = GetHSYMappingValue(1, UserPrefIndependentSaturationG);
    msAPI_Adjust_Sat(1, u32Temp);
    u32Temp = GetHSYMappingValue(1, UserPrefIndependentSaturationB);
    msAPI_Adjust_Sat(2, u32Temp);
    u32Temp = GetHSYMappingValue(1, UserPrefIndependentSaturationC);
    msAPI_Adjust_Sat(3, u32Temp);
    u32Temp = GetHSYMappingValue(1, UserPrefIndependentSaturationM);
    msAPI_Adjust_Sat(4, u32Temp);
    u32Temp = GetHSYMappingValue(1, UserPrefIndependentSaturationY);
    msAPI_Adjust_Sat(5, u32Temp);

    u32Temp = GetHSYMappingValue(2, UserPrefIndependentBrightnessR);
    msAPI_Adjust_Bri(0, u32Temp);
    u32Temp = GetHSYMappingValue(2, UserPrefIndependentBrightnessG);
    msAPI_Adjust_Bri(1, u32Temp);
    u32Temp = GetHSYMappingValue(2, UserPrefIndependentBrightnessB);
    msAPI_Adjust_Bri(2, u32Temp);
    u32Temp = GetHSYMappingValue(2, UserPrefIndependentBrightnessC);
    msAPI_Adjust_Bri(3, u32Temp);
    u32Temp = GetHSYMappingValue(2, UserPrefIndependentBrightnessM);
    msAPI_Adjust_Bri(4, u32Temp);
    u32Temp = GetHSYMappingValue(2, UserPrefIndependentBrightnessY);
    msAPI_Adjust_Bri(5, u32Temp);
#endif
}

enum {
 SDTV_LIMITED = 0,
 SDTV_FULL = 1,
 HDTV_LIMITED =2,
 HDTV_FULL =3,
};


void AdjustVideoHueSaturation(void)
{
/*    if(UserPrefHue==DefHue && UserPrefSaturation==DefSaturation)
    {
#if ENABLE_FULL_RGB_COLOR_PATH
        msSetVideoColorMatrix_Contrast(MAIN_WINDOW, UserPrefContrast);
		msSetVideoColorMatrix_Contrast(SUB_WINDOW, UserPrefContrast);
#else
        if (IsColorspaceRGBInput() && (GetInputCombColorFormat().ucColorRange != COMBO_COLOR_RANGE_LIMIT))  //RGB Full//restore to pure RGB
        {
			msAdjustPCContrast(MAIN_WINDOW, UserPrefContrast);
			msAdjustPCContrast(SUB_WINDOW, UserPrefContrast);
            msWriteByteMask(SC07_40, FALSE, BIT4|BIT0);
			msACESetRGBMode(TRUE);
		msWriteByteMask(SC0F_AE, 0, BIT6|BIT7);
        }
        else
        {
	    	msAdjustVideoHue(MAIN_WINDOW, UserPrefHue);
	    	msAdjustVideoHue(SUB_WINDOW, UserPrefHue);
            msAdjustVideoSaturation(MAIN_WINDOW, UserPrefSaturation);
            msAdjustVideoSaturation(SUB_WINDOW, UserPrefSaturation);
            msACESetRGBMode(FALSE);

			if (IsColorspaceRGBInput())     // RGB + Limit
			{
				msWriteByteMask(SC07_40, BIT4|BIT0, BIT4|BIT0);
				msWriteByteMask(SC07_41, FALSE, BIT4|BIT0);
			}
			else //YUV
			{
				msWriteByteMask(SC07_40, FALSE, BIT4|BIT0);
			}
            msWriteByteMask(SC0F_AE, BIT6|BIT7, BIT6|BIT7);
        }
#endif
    }
    else*/
    {
#if CHIP_ID == CHIP_MT9701
        if(msACEGetWinColor() == WIN_COLOR_YUV)
#endif            
        {
        msAdjustVideoHue(MAIN_WINDOW, UserPrefHue);
		msAdjustVideoHue(SUB_WINDOW, UserPrefHue);
        msAdjustVideoSaturation(MAIN_WINDOW, UserPrefSaturation);
        msAdjustVideoSaturation(SUB_WINDOW, UserPrefSaturation);
        }


#if ((CHIP_ID == CHIP_MT9700) && !ENABLE_FULL_RGB_COLOR_PATH)
        msAPI_VideoHueSaturation();

#endif
    }
}


/////////////////////////////////////
//For Leger3 SQC test modifications//
/////////////////////////////////////

/// @brief Setting DC off Discharge enable/disable
/// @param  void, control by MenuItemIndex
/// @return TRUE -> Setting success, FALSE -> Setting fail or same setting
Bool MenuFunc_DCOffDischarge_Set(void)
{
    if (UserPrefDCOffDischarge == MenuItemIndex)
        return FALSE;
    UserPrefDCOffDischarge = MenuItemIndex;

    return TRUE;
}
/// @brief Setting System would goto PM mode or not 
/// @param  void, control by MenuItemIndex
/// @return TRUE -> Setting success, FALSE -> Setting fail or same setting
Bool MenuFunc_PowerSavingEn_Set(void)
{
    if (UserPrefPowerSavingEn == MenuItemIndex)
        return FALSE;
    UserPrefPowerSavingEn = MenuItemIndex;

    return TRUE;
}

/// @brief 
/// @param  
/// @return 
Bool MenuFunc_DisplayLogoEn_Set(void)
{
    if (UserPrefDisplayLogoEn == MenuItemIndex)
        return FALSE;
    UserPrefDisplayLogoEn = MenuItemIndex;

    SaveMonitorSetting();
    return TRUE;
}

/// @brief sync all OSD v freq calculation in one function
/// @param  
/// @return V freq(Hz)
WORD MenuFunc_GetVfreq(void)
{
    WORD u16Vfreq = 0;
    #if (CHIP_ID != CHIP_MT9701)
    WORD u16Hfreq = 0;;
    #endif
#if ENABLE_FREESYNC
    if(msAPI_combo_IPGetDDRFlag())
    {
        u16Vfreq = GetSrcFreesyncFPS();
        MenuFunc_printData("Freesync vfreq = %d\n", u16Vfreq);
    }
    else
#endif
    {
        
        #if (CHIP_ID == CHIP_MT9701)
        u16Vfreq = V_FREQ_IN;
        #else
        u16Hfreq = HFreq( SrcHPeriod );
        if( CURRENT_SOURCE_IS_INTERLACE_MODE() )
        {
            u16Hfreq *= 2;
        }
        u16Vfreq = (( DWORD )u16Hfreq * 1000 ) / SrcVTotal;
        u16Vfreq = (u16Vfreq+5)/10;
        #endif
        MenuFunc_printData("Non-Freesync vfreq = %d\n", u16Vfreq);
    }

    return u16Vfreq;
}

Bool AdjustPowerKey( void )
{
    if(UserprefPowerKeyEnable)
        UserprefPowerKeyEnable = 0;
    else
        UserprefPowerKeyEnable = 1;    

    Set_SaveMonitorSettingFlag();
    return TRUE;
}

//======== FACTORY MENU FUNCTION ===============//
Bool AdjustFBrightness_0( MenuItemActionType action )
{
    WORD xdata tempValue;

    tempValue = DecIncValue( action, FUserPrefBrightness_0, 0, FUserPrefBrightness_25, 1 );
    FUserPrefBrightness_0 = tempValue;
    mStar_FAdjustBrightness( FUserPrefBrightness_0 );

    return TRUE;
}
WORD GetFBrightness_0Value( void )
{
    return FUserPrefBrightness_0;
}
Bool AdjustFBrightness_25( MenuItemActionType action )
{
    WORD xdata tempValue;

    tempValue = DecIncValue( action, FUserPrefBrightness_25, FUserPrefBrightness_0, FUserPrefBrightness_50, 1 );

    FUserPrefBrightness_25 = tempValue;
    mStar_FAdjustBrightness( FUserPrefBrightness_25 );
    return TRUE;
}
WORD GetFBrightness_25Value( void )
{
    return FUserPrefBrightness_25;
}
Bool AdjustFBrightness_50( MenuItemActionType action )
{
    WORD xdata tempValue;

    tempValue = DecIncValue( action, FUserPrefBrightness_50, FUserPrefBrightness_25, FUserPrefBrightness_75, 1 );
    FUserPrefBrightness_50 = tempValue;
    mStar_FAdjustBrightness( FUserPrefBrightness_50 );
    return TRUE;
}
WORD GetFBrightness_50Value( void )
{
    return FUserPrefBrightness_50;
}
Bool AdjustFBrightness_75( MenuItemActionType action )
{
    WORD xdata tempValue;

    tempValue = DecIncValue( action, FUserPrefBrightness_75, FUserPrefBrightness_50, FUserPrefBrightness_100, 1 );
    FUserPrefBrightness_75 = tempValue;
    mStar_FAdjustBrightness( FUserPrefBrightness_75 );
    return TRUE;
}
WORD GetFBrightness_75Value( void )
{
    return FUserPrefBrightness_75;
}
Bool AdjustFBrightness_100( MenuItemActionType action )
{
    WORD xdata tempValue;

    tempValue = DecIncValue( action, FUserPrefBrightness_100, FUserPrefBrightness_75, DEF_FAC_BRIGHTNESS_100, 1 );

    FUserPrefBrightness_100 = tempValue;
    mStar_FAdjustBrightness( FUserPrefBrightness_100 );
    return TRUE;
}
WORD GetFBrightness_100Value( void )
{
    return FUserPrefBrightness_100;
}

//#if FAC_BrightnessAutoCalculate || ENABLE_BOE_NEW_SZ_DDCCI_SPEC
#if ENABLE_BOE_NEW_SZ_DDCCI_SPEC
static void calculateBrightnessLevels(BYTE brightness0, BYTE brightness100)
{
/*先拿掉make看看
    float range = (float)(brightness100 - brightness0);
    USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_25 = (BYTE)round(brightness0 + range * 0.25f);
    USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_50 = (BYTE)round(brightness0 + range * 0.50f);
    USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_75 = (BYTE)round(brightness0 + range * 0.75f);
*/
}
#endif

#if ENABLE_BOE_NEW_SZ_DDCCI_SPEC
Bool DDCCI_AdjustLID_Mode(BYTE DDC_Data) //for LiteMax test
{
/*先拿掉make看看
	if (DDC_Data == USER_PREF_FAC_LIDMODE)
        return FALSE;

	USER_PREF_FAC_LIDMODE = DDC_Data;
	if (GetBacklightModeValue() == EN_BL_MODE_OSD) {
		if (USER_PREF_FAC_LIDMODE == BL_LID_MODE_PWM) {
				msAPIPWMFreq(BACKLIGHT_PWM,DIMMING_PWM_HZ);
		}
		else {
				msAPIPWMFreq(BACKLIGHT_PWM,DIMMING_DC_HZ);
		}
        #if EnablePQSettingsByPort
			msAPI_AdjustBrightness(USER_PREF_BRIGHTNESS(SrcInputPortM));
        #else
			msAPI_AdjustBrightness(USER_PREF_BRIGHTNESS);
        #endif
	}
	else if (GetBacklightModeValue() == EN_BL_MODE_LS) {
		if (USER_PREF_FAC_LIDMODE == BL_LID_MODE_PWM) {
			msAPIPWMFreq(BACKLIGHT_PWM,DIMMING_PWM_HZ);
		}
		else {
				msAPIPWMFreq(BACKLIGHT_PWM,DIMMING_DC_HZ);
		}
			msAPI_AdjustBrightness(LS_BacklightPrev);
	}
	else if (GetBacklightModeValue() == EN_BL_MODE_VR) {
		if (USER_PREF_FAC_LIDMODE == BL_LID_MODE_PWM) {
			msAPIPWMFreq(BACKLIGHT_PWM,DIMMING_PWM_HZ);
		}
		else {
				msAPIPWMFreq(BACKLIGHT_PWM,DIMMING_DC_HZ);
		}
		
		msAPI_AdjustBrightness(LS_BacklightPrev );
	}
	SetLidMode(DDC_Data);
	
	//SaveFactorySetting(); //FactoryBlockSave();

	return TRUE;
*/
}

void DDCCI_AdjDCCurve0(BYTE DDC_Data) //for LiteMax test
{
/*先拿掉make看看
	USER_PREF_FAC_LIDMODE = BL_LID_MODE_DC;
    USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_0 = DDC_Data;
    msAPI_FAdjustBrightness(USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_0);
*/
}
void DDCCI_AdjDCCurve25(BYTE DDC_Data) //for LiteMax test
{
/*先拿掉make看看
	USER_PREF_FAC_LIDMODE = BL_LID_MODE_DC;
    USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_25 = DDC_Data;
    msAPI_FAdjustBrightness(USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_25);
*/
}
void DDCCI_AdjDCCurve50(BYTE DDC_Data) //for LiteMax test
{
/*先拿掉make看看
	USER_PREF_FAC_LIDMODE = BL_LID_MODE_DC;
    USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_50 = DDC_Data;
    msAPI_FAdjustBrightness(USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_50);
*/
}
void DDCCI_AdjDCCurve75(BYTE DDC_Data) //for LiteMax test
{
/*先拿掉make看看
	USER_PREF_FAC_LIDMODE = BL_LID_MODE_DC;
    USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_75 = DDC_Data;
    msAPI_FAdjustBrightness(USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_75);
*/
}
void DDCCI_AdjDCCurve100(BYTE DDC_Data) //for LiteMax test
{
/*先拿掉make看看
	USER_PREF_FAC_LIDMODE = BL_LID_MODE_DC;
    USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_100 = DDC_Data;
   	msAPI_FAdjustBrightness(USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_100);
*/
}
void DDCCI_AdjPWMCurve75(BYTE DDC_Data) //for LiteMax test
{
/*
	USER_PREF_FAC_LIDMODE = BL_LID_MODE_PWM;
    USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_75 = DDC_Data;
    msAPI_FAdjustBrightness(USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_75);
*/
	if (DDC_Data >= 	FUserPrefBrightness_100)
		DDC_Data = FUserPrefBrightness_100;
	else if (DDC_Data <= FUserPrefBrightness_50)
		DDC_Data = FUserPrefBrightness_50;

	FUserPrefBrightness_75 = DDC_Data;
	mStar_FAdjustBrightness(FUserPrefBrightness_75);
}
void DDCCI_AdjPWMCurve0AutoCalculate(BYTE DDC_Data) //for LiteMax test
{
/*先拿掉make看看
	USER_PREF_FAC_LIDMODE = BL_LID_MODE_PWM;
	USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_0 = DDC_Data;
	calculateBrightnessLevels(USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_0, USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_100);
    msAPI_FAdjustBrightness(USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_0);
*/
}
void DDCCI_AdjPWMCurve100AutoCalculate(BYTE DDC_Data) //for LiteMax test
{
/*先拿掉mark看看
	USER_PREF_FAC_LIDMODE = BL_LID_MODE_PWM;
	USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_100 = DDC_Data;
	calculateBrightnessLevels(USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_0, USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_100);
    msAPI_FAdjustBrightness(USER_PREF_FAC_BRIGHTNESS_CURVE[USER_PREF_FAC_LIDMODE].u16OSD_100);
*/
}
#endif

Bool FactoryReset(void)
{
	Init_FactorySetting();
	SaveFactorySetting();
	Init_MonitorSetting();
	SaveMonitorSetting();

	Set_FactoryModeFlag();
	ResetAllSetting();

	return TRUE;
}



