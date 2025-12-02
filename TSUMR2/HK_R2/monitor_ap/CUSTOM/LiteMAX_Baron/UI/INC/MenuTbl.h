#include "MenuFontTbl.h"
#include "MenuTblDef.h"

#define SUB_TEXT_XPOS           12
#define SUB_TEXT_YPOS           6

#define SUB_TEXTINFO_YPOS   12

#define TestXPosition           12
#define NumXposion          25
#define GaugeXPosition          34

//=================================================================
RadioTextType code BriteContMenuRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  NULL, Layer2XPos, (LayerYPos+2*BriteContMenuItems_Brightness), BrightnessText},
    {  NULL, Layer2XPos, (LayerYPos+2*BriteContMenuItems_Contrast), ContrastText},
    #if ENABLE_DLC
    {  NULL, Layer2XPos, (LayerYPos+2*BriteContMenuItems_DLC), DLCText},
    #endif
    #if ENABLE_DPS
    {  NULL, Layer2XPos, (LayerYPos+2*BriteContMenuItems_DPS), DPSText},
    #endif
    #if ENABLE_DCR
    {  NULL, Layer2XPos, (LayerYPos+2*BriteContMenuItems_DCR), DCRText},
    #endif
#if (ENABLE_VGA_INPUT)
    {  dwiVGAItem, Layer2XPos, (LayerYPos+2*BriteContMenuItems_AutoColor), ADCAutoColorText},
#endif
    {NULL, Layer2XPos, (LayerYPos+2*BriteContMenuItems_Return), ReturnText},
    #if ENABLE_DLC
    {  NULL, Layer3XPos, (LayerYPos+2*BriteContMenuItems_DLC), DLCStatusText},
    #endif
    #if ENABLE_DPS
    {  NULL, Layer3XPos, (LayerYPos+2*BriteContMenuItems_DPS), DPSStatusText},
    #endif
    #if ENABLE_DCR
    {NULL, Layer3XPos, (LayerYPos+2*BriteContMenuItems_DCR), DCRStatusText},
    #endif
    { dwiEnd, Layer3XPos, (LayerYPos+2*BriteContMenuItems_Max), NullText},
};
DrawRadioGroupType code DrawBriteContMenuRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, BriteContMenuRatioText}
};

GaugeType code BriteContMenuGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {  NULL, GaugeXPos, (LayerYPos+2*BriteContMenuItems_Brightness), GetBrightnessValue},
    {dwiEnd, GaugeXPos, (LayerYPos+2*BriteContMenuItems_Contrast), GetContrastValue},
};
DrawGuageType code DrawBriteContMenutGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {CPC_White, CPC_Black,    9,  BriteContMenuGuage}
};

NumberType code BriteContMenuNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {  NULL, NumberXPos, (LayerYPos+2*BriteContMenuItems_Brightness), GetBrightnessValue},
    {dwiEnd, NumberXPos, (LayerYPos+2*BriteContMenuItems_Contrast), GetContrastValue},
};
DrawNumberType code DrawBriteContMenuNumber[] =
{
    // ForeColor, BackColor,  GetValue
    {CPC_White, CPC_Black,  BriteContMenuNumber}
};
//--------------------------------------------------------------------------
RadioTextType code ColorSettingsMenuRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  NULL, Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_ColorTemp), ColorTempText},
    {  dwiUserColor, Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_Red), RedText},
    {  dwiUserColor, Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_Green), GreenText},
    {  dwiUserColor, Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_Blue), BlueText},
    {  dwiUserColor, Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_Hue), HueText},
    {  dwiUserColor, Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_Saturation), SaturationText},
#if ENABLE_SUPER_RESOLUTION
    {  NULL, Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_SuperResolution), SuperResolutionText},
#endif
#if ENABLE_COLORMODE_DEMO
    {  NULL, Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_ColorMode), ColorModeText},
#endif
    {  NULL, Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_ColorFormat), ColorFormatText},
    {  NULL, Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_Return), ReturnText},

    {  NULL, Layer3XPos, (LayerYPos+2*ColorSettingsMenuItems_ColorTemp), ColorTempStatusText},
#if ENABLE_SUPER_RESOLUTION
    {  NULL, Layer3XPos, (LayerYPos+2*ColorSettingsMenuItems_SuperResolution), SuperResolutionStatusText},
#endif
#if ENABLE_COLORMODE_DEMO
    {  NULL, Layer3XPos, (LayerYPos+2*ColorSettingsMenuItems_ColorMode), ColorModeStatusText},
#endif
    {dwiEnd, Layer3XPos, (LayerYPos+2*ColorSettingsMenuItems_ColorFormat), ColorFormatStatusText},


};
DrawRadioGroupType code DrawColorSettingsMenuRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, ColorSettingsMenuRatioText}
};

GaugeType code ColorSettingsMenuGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {  dwiUserColor, GaugeXPos, (LayerYPos+2*ColorSettingsMenuItems_Red), GetRedColorValue},
    {  dwiUserColor, GaugeXPos, (LayerYPos+2*ColorSettingsMenuItems_Green), GetGreenColorValue},
    {  dwiUserColor, GaugeXPos, (LayerYPos+2*ColorSettingsMenuItems_Blue), GetBlueColorValue},
    {  dwiUserColor, GaugeXPos, (LayerYPos+2*ColorSettingsMenuItems_Hue), GetHueValue},
    {  dwiUserColor|dwiEnd, GaugeXPos, (LayerYPos+2*ColorSettingsMenuItems_Saturation), GetSaturationValue},
};
DrawGuageType code DrawColorSettingsMenuGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {CPC_White, CPC_Black,    9,  ColorSettingsMenuGuage}
};

NumberType code ColorSettingsMenuNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {  dwiUserColor, NumberXPos, (LayerYPos+2*ColorSettingsMenuItems_Red), GetRedColorValue},
    {  dwiUserColor, NumberXPos, (LayerYPos+2*ColorSettingsMenuItems_Green), GetGreenColorValue},
    {  dwiUserColor, NumberXPos, (LayerYPos+2*ColorSettingsMenuItems_Blue), GetBlueColorValue},
    {  dwiUserColor, NumberXPos, (LayerYPos+2*ColorSettingsMenuItems_Hue), GetHueValue},
    {  dwiUserColor|dwiEnd, NumberXPos, (LayerYPos+2*ColorSettingsMenuItems_Saturation), GetSaturationValue},
};
DrawNumberType code DrawColorSettingsMenuNumber[] =
{
    // ForeColor, BackColor,  GetValue
    {CPC_White, CPC_Black,  ColorSettingsMenuNumber}
};

//--------------------------------------------------------------------------
RadioTextType code ExtColorSettingsMenuRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  NULL, Layer2XPos, (LayerYPos+2*ExtColorSettingsMenuItems_ColorRange), ColorRangeText},
    {  NULL, Layer2XPos, (LayerYPos+2*ExtColorSettingsMenuItems_IndependentHue), IndependentHueText},
    {  NULL, Layer2XPos, (LayerYPos+2*ExtColorSettingsMenuItems_IndependentSaturation), IndependentSaturationText},
    {  NULL, Layer2XPos, (LayerYPos+2*ExtColorSettingsMenuItems_IndependentBrightness), IndependentBrightnessText},
    {  NULL, Layer2XPos, (LayerYPos+2*ExtColorSettingsMenuItems_Return), ReturnText},

    {  dwiEnd, Layer3XPos, (LayerYPos+2*ExtColorSettingsMenuItems_ColorRange), ColorRangeStatusText},
};
DrawRadioGroupType code DrawExtColorSettingsMenuRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, ExtColorSettingsMenuRatioText}
};

//--------------------------------------------------------------------------
RadioTextType code InputSourceMenuRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  NULL, Layer2XPos, (LayerYPos+2*InputSourceMenuItems_SourceSelect), SourceSelectText},
    {  NULL, Layer2XPos, (LayerYPos+2*InputSourceMenuItems_Return), ReturnText},
    {  NULL, Layer3XPos, (LayerYPos+2*InputSourceMenuItems_SourceSelect), SourceSelectStatusText},
#if ENABLE_DP_INPUT
    {  NULL, Layer2XPos, (LayerYPos+2*InputSourceMenuItems_DPVersion), DPVersionText},
    {  dwiEnd, Layer3XPos, (LayerYPos+2*InputSourceMenuItems_DPVersion), DPVersionStatusText},
#else
    { dwiEnd, Layer2XPos, (LayerYPos+2*InputSourceMenuItems_Max), NullText},
#endif
};
DrawRadioGroupType code DrawInputSourceMenuRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, InputSourceMenuRatioText}
};

//--------------------------------------------------------------------------
GaugeType code DisplaySettingsMenuGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
#if (ENABLE_VGA_INPUT)
    {  dwiVGAItem, GaugeXPos, (LayerYPos+2*DisplaySettingsMenuItems_HPos), GetHPositionValue},
    {  dwiVGAItem, GaugeXPos, (LayerYPos+2*DisplaySettingsMenuItems_VPos), GetVPositionValue},
#endif
#if ENABLE_SHARPNESS
    {  NULL, GaugeXPos, (LayerYPos+2*DisplaySettingsMenuItems_Sharpness), GetSharpnessValue},
#endif
#if (ENABLE_VGA_INPUT)
    {  dwiVGAItem, GaugeXPos, (LayerYPos+2*DisplaySettingsMenuItems_Clock), GetClockValue},
    {dwiVGAItem|dwiEnd, GaugeXPos, (LayerYPos+2*DisplaySettingsMenuItems_Phase), GetFocusValue},
#endif
#if ENABLE_SHARPNESS
    {  dwiEnd, GaugeXPos, (LayerYPos+2*DisplaySettingsMenuItems_Sharpness), GetSharpnessValue},
#endif
    {dwiNULL, 0, 0, 0},
};
DrawGuageType code DrawDisplaySettingsMenuGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {CPC_White, CPC_Black,    9,  DisplaySettingsMenuGuage}
};

NumberType code DisplaySettingsMenuNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
#if (ENABLE_VGA_INPUT)
    {  dwiVGAItem, NumberXPos, (LayerYPos+2*DisplaySettingsMenuItems_HPos), GetHPositionValue},
    {  dwiVGAItem, NumberXPos, (LayerYPos+2*DisplaySettingsMenuItems_VPos), GetVPositionValue},
#endif
#if ENABLE_SHARPNESS
    {  NULL, NumberXPos, (LayerYPos+2*DisplaySettingsMenuItems_Sharpness), GetSharpnessValue},
#endif
#if (ENABLE_VGA_INPUT)
    {  dwiVGAItem, NumberXPos, (LayerYPos+2*DisplaySettingsMenuItems_Clock), GetClockValue},
    {dwiVGAItem|dwiEnd, NumberXPos, (LayerYPos+2*DisplaySettingsMenuItems_Phase), GetFocusValue},
#endif
#if ENABLE_SHARPNESS
    {  dwiEnd, NumberXPos, (LayerYPos+2*DisplaySettingsMenuItems_Sharpness), GetSharpnessValue},
#endif
    {dwiNULL, 0, 0, 0},
};
DrawNumberType code DrawDisplaySettingsMenuNumber[] =
{
    // ForeColor, BackColor,  GetValue
    {CPC_White, CPC_Black,  DisplaySettingsMenuNumber}
};

RadioTextType code DisplaySettingsMenuRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  NULL, Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_Gamma), GammaText},
#if (ENABLE_VGA_INPUT)
    {  dwiVGAItem, Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_HPos), HPositionText},
    {  dwiVGAItem, Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_VPos), VPositionText},
#endif
    #if ENABLE_SHARPNESS
    {  NULL, Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_Sharpness), SharpnessText},
    #endif
#if (ENABLE_VGA_INPUT)
    {  dwiVGAItem, Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_Clock), ClockText},
    {  dwiVGAItem, Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_Phase), PhaseText},
#endif
    #if Enable_Expansion
    {  NULL, Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_Aspect), AspectRatioText},
    #endif
    #if ENABLE_RTE
    {  NULL, Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_OD), OverdriveText},
    #endif
#if (ENABLE_VGA_INPUT)
    {  dwiVGAItem, Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_AutoAdjust), AutoAdjustText},
#endif
    {  NULL, Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_Return), ReturnText},

    {  NULL, Layer3XPos, (LayerYPos+2*DisplaySettingsMenuItems_Gamma), GammaStatusText},
    #if Enable_Expansion
    {  NULL, Layer3XPos, (LayerYPos+2*DisplaySettingsMenuItems_Aspect), AspectRatioStatusText},
    #endif
    #if ENABLE_RTE
    {  NULL, Layer3XPos, (LayerYPos+2*DisplaySettingsMenuItems_OD), OverdriveStatusText},
    #endif
    {dwiEnd, Layer3XPos, (LayerYPos+2*DisplaySettingsMenuItems_Max), NullText},
};
DrawRadioGroupType code DrawDisplaySettingsMenuRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, DisplaySettingsMenuRatioText}
};

//--------------------------------------------------------------------------
GaugeType code OtherSettingsMenuGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    #if AudioFunc
    {  NULL, GaugeXPos, (LayerYPos+2*OtherSettingsMenuItems_Volume), GetVolumeValue},
    #endif
    {  NULL, GaugeXPos, (LayerYPos+2*OtherSettingsMenuItems_OSDTrans), GetOSDTransparencyValue},
    {  NULL, GaugeXPos, (LayerYPos+2*OtherSettingsMenuItems_OSDHPos), GetOSDHPositionValue},
    {  NULL, GaugeXPos, (LayerYPos+2*OtherSettingsMenuItems_OSDVPos), GetOSDVPositionValue},
    {dwiEnd, GaugeXPos, (LayerYPos+2*OtherSettingsMenuItems_OSDTimeout), GetOSDTime100Value},
};
DrawGuageType code DrawOtherSettingsMenuGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {CPC_White, CPC_Black,    9,  OtherSettingsMenuGuage}
};

NumberType code OtherSettingsMenuNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    #if AudioFunc
    {  NULL, NumberXPos, (LayerYPos+2*OtherSettingsMenuItems_Volume), GetVolumeValue},
    #endif
    {  NULL, NumberXPos, (LayerYPos+2*OtherSettingsMenuItems_OSDTrans), GetOSDTransparencyValue},
    {  NULL, NumberXPos, (LayerYPos+2*OtherSettingsMenuItems_OSDHPos), GetOSDHPositionValue},
    {  NULL, NumberXPos, (LayerYPos+2*OtherSettingsMenuItems_OSDVPos), GetOSDVPositionValue},
    {dwiEnd, NumberXPos, (LayerYPos+2*OtherSettingsMenuItems_OSDTimeout), GetOSDTime100Value},
};
DrawNumberType code DrawOtherSettingsMenuNumber[] =
{
    // ForeColor, BackColor,  GetValue
    {CPC_White, CPC_Black,  OtherSettingsMenuNumber}
};

RadioTextType code OtherSettingsMenuRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    #if AudioFunc
    {  NULL, Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_Volume), VolumeText},
    {  NULL, Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_AudioSource), AudioSourceText},
    {  NULL, Layer3XPos, (LayerYPos+2*OtherSettingsMenuItems_AudioSource), AudioSourceValue},
    #endif
    {  NULL, Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_OSDTrans), OSDTransparencyText},
    {  NULL, Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_OSDHPos), OSDHPositionText},
    {  NULL, Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_OSDVPos), OSDVPositionText},
    {  NULL, Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_OSDTimeout), OSDTimeoutText},
#if ENABLE_OSD_ROTATION
    {  NULL, Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_OSDRotate), OSDRotateText},
    {  NULL, Layer3XPos, (LayerYPos+2*OtherSettingsMenuItems_OSDRotate), OSDRotateStatusText},
#endif
#if ENABLE_FREESYNC
    {  NULL, Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_FreeSync), FreeSyncModeText},
    {  NULL, Layer3XPos, (LayerYPos+2*OtherSettingsMenuItems_FreeSync), FreeSyncModeStatusText},
#endif
//    {  NULL, Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_DisplayLogo), DisplayLogoText},
//    {  NULL, Layer3XPos, (LayerYPos+2*OtherSettingsMenuItems_DisplayLogo), DisplayLogoStatusText},
    {NULL, Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_Return), ReturnText},
    {dwiEnd, Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_Reset), FactoryResetText},
};
DrawRadioGroupType code DrawOtherSettingsMenuRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, OtherSettingsMenuRatioText}
};

RadioTextType code PowerManagerMenuRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
#if ENABLE_DCOFF_CHARGE
    { NULL, Layer2XPos, (LayerYPos+2*PowerManagerMenuItems_DCOffDischarge), PowerManagerDCOffDischargeText},
#endif
    { NULL, Layer2XPos, (LayerYPos+2*PowerManagerMenuItems_Return), ReturnText},
    {dwiEnd, Layer2XPos, (LayerYPos+2*PowerManagerMenuItems_PowerSaving), PowerManagerPowerSavingText},
};
DrawRadioGroupType code DrawPowerManagerMenuRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, PowerManagerMenuRatioText}
};

//=========================================================
#if LiteMAX_Baron_OSD_TEST
MenuItemType code MainMenuItems[] =
{
    // 0 LuminanceMenu
    {
        2, MainMenusStr_Y_Start, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // SelForeColor, SelBackColor;
        BrightnessMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        BrightnessText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL // DrawRadioGroupType
        },
        NULL, // Font
        mibSelectable // Flags
    },
	#if ENABLE_SHARPNESS
    // 1 SharpnessMenu
    {
        7, MainMenusStr_Y_Start,//8, 6, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // SelForeColor, SelBackColor;
        SharpnessMenu, // NextMenuPage;
        DWI_Nothing,//DWI_Icon, // DrawMenuItemType;//David add at 20241029 for remove sharpness
        SharpnessText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL, // DrawRadioGroupType
        },
        NULL, // Font
        mibInvisible//mibSelectable // Flags//David add at 20241029 for remove sharpness
    },
    #endif
    // 1 ColorMenu
    {
        9, MainMenusStr_Y_Start, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // SelForeColor, SelBackColor;
        ColorTempMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        ColorText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL, // DrawRadioGroupType
        },
        NULL, // Font
        mibSelectable // Flags
    },
    // 2 ToolMenu
    {
        15, MainMenusStr_Y_Start, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // SelForeColor, SelBackColor;
        DefaultMenu,// NextMenuPage;
        DWI_Icon,// DrawMenuItemType;
        DefaultText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,   // AdjustFunction
            NULL,   // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL, // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 3 PowerLockMenu
    {
        20, MainMenusStr_Y_Start, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // SelForeColor, SelBackColor;
        RootMenu,// NextMenuPage;
        DWI_Icon,// DrawMenuItemType;
        PowerKeyEnableText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL,   // AdjustFunction
            AdjustPowerKey   // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL, // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 4 Exit
    {
        27, MainMenusStr_Y_Start, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // SelForeColor, SelBackColor;
        RootMenu,// NextMenuPage;
        DWI_Icon,// DrawMenuItemType;
        ExitText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,   // AdjustFunction
            NULL,   // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL, // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

MenuItemType code LuminanceMenuItems[] =
{
    // 0 Brightness
    {
        11,5,//6, 5, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // SelForeColor, SelBackColor;
        BrightnessMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        BrightnessText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
};
MenuItemType code SignalMenuItems[] =
{
#if 1 // remove signal "AUTO"
    // 0 Auto
    {
        4,10,//6, 5, // XPos, YPos;
        CPC_Yellow, CPC_Black, // ForeColor, BackColor;
        CPC_Yellow, CPC_Blue, // SelForeColor, SelBackColor;
        RootMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        AutoSelectText, //AutoText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            ChangeSource, //SwitchSignal, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
#endif
    // 1 VGA
    {
        9,10,//8,10,//6, 5, // XPos, YPos;
        CPC_Yellow, CPC_Black, // ForeColor, BackColor;
        CPC_Yellow, CPC_Blue, // SelForeColor, SelBackColor;
        RootMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        VGAText,//DefaultText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            ChangeSource, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    #if 0
    // 2 DVI
    {
        9,10,//12,10,//14, 5, // XPos, YPos;
        CPC_Yellow, CPC_Black, // ForeColor, BackColor;
        CPC_Yellow, CPC_Blue, // SelForeColor, SelBackColor;
        RootMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        DigitalPort0Text, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            ChangeSource, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    #endif
    // 3 HDMI
    {
        14,10,//16,10,//24, 5, // XPos, YPos;
        CPC_Yellow, CPC_Black, // ForeColor, BackColor;
        CPC_Yellow, CPC_Blue, // SelForeColor, SelBackColor;
        RootMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        DigitalPort0Text, //HDMIText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            ChangeSource, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 4 DP
    {
        19,10,//20,10,//14, 5, // XPos, YPos;
        CPC_Yellow, CPC_Black, // ForeColor, BackColor;
        CPC_Yellow, CPC_Blue, // SelForeColor, SelBackColor;
        RootMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        DigitalPort1Text, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            ChangeSource, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 5 Exit
    {
        24,10,//24, 5, // XPos, YPos;
        CPC_Yellow, CPC_Black, // ForeColor, BackColor;
        CPC_Yellow, CPC_Blue, // SelForeColor, SelBackColor;
        MainMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        ExitText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
};
MenuItemType code SoundMenuItems[] =
{
    // 0 SoundAdjust
    {
        11,5,//6, 5, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // SelForeColor, SelBackColor;
        SoundAdjustMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//C9300KText,//DefaultText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 1 SoundMute
    {
        18,5,//14, 5, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // SelForeColor, SelBackColor;
        ColorSettingsMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//CUserText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
    #if AudioFunc // jason 20200507
            NULL, //Temp SetSoundMute, // ExecFunction
    #else
            NULL, // ExecFunction
    #endif
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 2 Exit
    {
        25,5,//24, 5, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // SelForeColor, SelBackColor;
        MainMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//ReturnText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
};
MenuItemType code ColorMenuItems[] =
{
	// 0 6500K
	{
		6, 5, // XPos, YPos;
		CPC_White, CPC_Black, // ForeColor, BackColor;
		CPC_White, CPC_Black, // SelForeColor, SelBackColor;
		ColorTempMenu, // NextMenuPage;
		DWI_Icon, // DrawMenuItemType;
		DefaultText, // DisplayText;
		NaviExecKeyEvent,
		{
			NULL, // AdjustFunction
			OSD_SetColorTemp, // ExecFunction
		},
		{
			NULL, // DrawNumberType
			NULL, // DrawGuageType
			NULL  // DrawMenuRadioGroup,
		},
		NULL, //Font
		mibSelectable // Flags
	},
	// 1 CTEMP_USER
	{
		14, 5, // XPos, YPos;
		CPC_White, CPC_Black, // ForeColor, BackColor;
		CPC_White, CPC_Black, // SelForeColor, SelBackColor;
		ColorSettingsMenu, // NextMenuPage;
		DWI_Icon, // DrawMenuItemType;
		CUserText, // DisplayText;
		NaviExec2KeyEvent,
		{
			NULL, // AdjustFunction
			OSD_SetColorTemp, // ExecFunction
		},
		{
			NULL, // DrawNumberType
			NULL, // DrawGuageType
			NULL  // DrawMenuRadioGroup,
		},
		NULL, //Font
		mibSelectable // Flags
	},
	// 2 Return
	{
		24, 5, // XPos, YPos;
		CPC_White, CPC_Black, // ForeColor, BackColor;
		CPC_White, CPC_Black, // SelForeColor, SelBackColor;
		MainMenu, // NextMenuPage;
		DWI_Icon, // DrawMenuItemType;
		ReturnText, // DisplayText;
		NaviKeyEvent,
		{
			NULL, // AdjustFunction
			NULL, // ExecFunction
		},
		{
			NULL, // DrawNumberType
			NULL, // DrawGuageType
			NULL  // DrawMenuRadioGroup,
		},
		NULL, //Font
		mibSelectable // Flags
	},
};
MenuItemType code ImageMenuItems[] =
{
    // 0 AutoAdjust
    {
        4,5,//6, 5, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // SelForeColor, SelBackColor;
        RootMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//C6500KText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
#if 0 // jason 20190617
            AutoConfig,
#else
            NULL, //Temp ShowVGAAutoMessage, // ExecFunction
#endif
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 1 Clock
    {
        NULL,NULL,//6, 5, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        ImageClockMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//C6500KText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 2 Phase
    {
        NULL,NULL,//6, 5, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        ImagePhaseMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//C6500KText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 3 ImageHPos
    {
        NULL,NULL,//6, 5, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        ImageHPosMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//C6500KText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 4 ImageVPos
    {
        NULL,NULL,//6, 5, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        ImageVPosMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//C6500KText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 5 Exit
    {
        25,5,//24, 5, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // SelForeColor, SelBackColor;
        MainMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//ReturnText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
};
MenuItemType code ToolMenuItems[] =
{
    // 0 OSD Control
    {
        NULL,NULL,//6, 5, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        OSDControlMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//C6500KText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 1 Factory_Reset
    {
        NULL,NULL,//6, 5, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        RootMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//C9300KText,//DefaultText, // DisplayText;
        NaviKeyEvent, //Temp NaviExec2KeyEvent,
        {
            NULL, // AdjustFunction
            ResetAllSetting, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 2 Sharpness
    {
        NULL,NULL,//14, 5, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        RootMenu, //Temp SharpnessMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//CUserText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 3 Exit
    {
        NULL,NULL,//24, 5, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        MainMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//ReturnText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
};
#endif


#if LiteMAX_Baron_OSD_TEST
//============ Red MenuItem==============
NumberType code UserRedNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {  dwiEnd, RGBGaugeXPos+RGBGaugeLens+1, 2, GetRedColorValue},
};//GaugeXPos+GaugeLens-5
DrawNumberType code DrawUserRedNumber[] =
{
    // ForeColor, BackColor,  GetValue
    { CPC_White, CPC_Black,     UserRedNumber},
};
GaugeType code UserRedGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    { dwiEnd, RGBGaugeXPos, 2, GetRedColorValue},
};
DrawGuageType code DrawUserRedGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    { CPC_Red,  CPC_Black,    RGBGaugeLens,  UserRedGuage},
};
//============ Green MenuItem==============
NumberType code UserGreenNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {  dwiEnd, RGBGaugeXPos+RGBGaugeLens+1, 4, GetGreenColorValue},
};//GaugeXPos+GaugeLens-5
DrawNumberType code DrawUserGreenNumber[] =
{
    // ForeColor, BackColor,  GetValue
    { CPC_White, CPC_Black,     UserGreenNumber},
};
GaugeType code UserGreenGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    { dwiEnd, RGBGaugeXPos, 4, GetGreenColorValue},
};
DrawGuageType code DrawUserGreenGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    { CPC_Green,  CPC_Black,    RGBGaugeLens,  UserGreenGuage},
};
//============ Blue MenuItem==============
NumberType code UserBlueNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    { dwiEnd, RGBGaugeXPos+RGBGaugeLens+1, 6, GetBlueColorValue},
};//GaugeXPos+GaugeLens-5
DrawNumberType code DrewUserBlueNumber[] =
{
    // ForeColor, BackColor,  GetValue
    { CPC_White, CPC_Black,     UserBlueNumber},
};
GaugeType code UserBlueGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    { dwiEnd, RGBGaugeXPos, 6, GetBlueColorValue},
};
DrawGuageType code DrawUserBlueGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    { CPC_Blue,  CPC_Black,    RGBGaugeLens,  UserBlueGuage},
};

//============ User Color MenuItem==============
MenuItemType code ColorSettingsMenuItems[] =
{
    // 0 red
    {
        8, 2, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // SelForeColor, SelBackColor;
        ColorTempMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        RText, // DisplayText;
        AdjusterItemKeyEvent,
        {
            AdjustRedColor,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawUserRedNumber, // DrawNumberType
            DrawUserRedGuage, // DrawGuageType
            NULL,//DrawColorSettingsMenuStatusRatioGroup, // DrawRadioGroupType
        },
        NULL, // Font
        mibSelectable // Flags
    },
    // 1 green
    {
        8, 4, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // SelForeColor, SelBackColor;
        ColorTempMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        GText, // DisplayText;
        AdjusterItemKeyEvent,
        {
            AdjustGreenColor,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawUserGreenNumber, // DrawNumberType
            DrawUserGreenGuage, // DrawGuageType
            NULL,//DrawColorSettingsMenuStatusRatioGroup, // DrawRadioGroupType
        },
        NULL, // Font
        mibSelectable // Flags
    },
    // 2 blue
    {
        8, 6, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // SelForeColor, SelBackColor;
        ColorTempMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        BText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustBlueColor,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrewUserBlueNumber, // DrawNumberType
            DrawUserBlueGuage, // DrawGuageType
            NULL,//DrawColorSettingsMenuStatusRatioGroup, // DrawRadioGroupType
        },
        NULL, // Font
        mibSelectable // Flags
    },
    // icon
    {
        2, 5, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // SelForeColor, SelBackColor;
        ColorTempMenu,// NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        CUserText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL, // DrawRadioGroupType
        },
        NULL, // Font
        mibDrawValue // Flags
    },
};
#endif

MenuItemType code DCOffDischargeMenuItems[] =
{
    //Off
    {
        Layer3XPos, (LayerYPos+2*PowerSavingMenuItems_Off), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        NULL,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        PowerManagerOffText,// DisplayText;
        NaviExecFuncGotoPrevKeyEvent,
        {
            NULL,// AdjustFunction
            MenuFunc_DCOffDischarge_Set,// ExecFunction
        },
        {
            NULL,   // DrawNumberType
            NULL,   // DrawGuageType
            NULL,   // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable
    },
    //On
    {
        Layer3XPos, (LayerYPos+2*PowerSavingMenuItems_On), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        NULL,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        PowerManagerOnText,// DisplayText;
        NaviExecFuncGotoPrevKeyEvent,
        {
            NULL,// AdjustFunction
            MenuFunc_DCOffDischarge_Set,// ExecFunction
        },
        {
            NULL,   // DrawNumberType
            NULL,   // DrawGuageType
            NULL,   // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable
    },
};

MenuItemType code PowerSavingMenuItems[] =
{
    //Off
    {
        Layer3XPos, (LayerYPos+2*DCOffDisChargerMenuItems_Off), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        NULL,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        PowerManagerOffText,// DisplayText;
        NaviExecFuncGotoPrevKeyEvent,
        {
            NULL,// AdjustFunction
            MenuFunc_PowerSavingEn_Set,// ExecFunction
        },
        {
            NULL,   // DrawNumberType
            NULL,   // DrawGuageType
            NULL,   // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable
    },
    //On
    {
        Layer3XPos, (LayerYPos+2*DCOffDisChargerMenuItems_On), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        NULL,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        PowerManagerOnText,// DisplayText;
        NaviExecFuncGotoPrevKeyEvent,
        {
            NULL,// AdjustFunction
            MenuFunc_PowerSavingEn_Set,// ExecFunction
        },
        {
            NULL,   // DrawNumberType
            NULL,   // DrawGuageType
            NULL,   // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable
    },
};
NumberType code HotBrightnessNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, 14, 2, GetBrightnessValue},
};
DrawNumberType code DrawHotBrightnessNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {CPC_White, CPC_Black,     HotBrightnessNumber}
};

GaugeType code HotBrightnessGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, 2, 4, GetBrightnessValue},
};
DrawGuageType code DrawHotBrightnessGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {CPC_Yellow, CPC_Black,  17,  HotBrightnessGuage}
};

//---------------------------------------------------------------
//============ Contrast MenuItem==============
#if LiteMAX_Baron_OSD_TEST 
NumberType code ContrastNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {  dwiEnd, GaugeXPos+GaugeLens-5, SubMenuValue_Y_Start, GetContrastValue},
};//GaugeXPos+GaugeLens-5
DrawNumberType code DrawContrastNumber[] =
{
    // ForeColor, BackColor,  GetValue
    {CPC_White, CPC_Black,     ContrastNumber}
};
GaugeType code ContrastGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, SubMenuBar_Y_Start, GetContrastValue},
};
DrawGuageType code DrawContrastGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {CPC_Yellow,  CPC_Black,    GaugeLens,  ContrastGuage}
};
MenuItemType code ContrastMenuItems[] =
{
    // 0 Contrast
    {
        NULL, NULL, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        LuminanceMenu,// NextMenuPage;
        DWI_Icon,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustContrast,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawContrastNumber,// DrawNumberType
            DrawContrastGuage,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};
#endif
//============ Brightness MenuItem==============
#if LiteMAX_Baron_OSD_TEST
NumberType code BrightnessNumber[] =
{
	// Flags,	XPos,	YPos,	GetValue
	{  dwiEnd, GaugeXPos+GaugeLens-5, 3, GetBrightnessValue},
};
DrawNumberType code DrawBrightnessNumber[] =
{
	// ForeColor, BackColor,  GetValue
	{ CPC_White, CPC_Black,	   BrightnessNumber},
};
GaugeType code BrightnessGuage[] =
{
	// Flags, ForeColor,	BackColor,	Length, 	GetValue
	{ dwiEnd, GaugeXPos, 4, GetBrightnessValue},
};
DrawGuageType code DrawBrightnessGuage[] =
{
	//	ForeColor,	BackColor,	Length,,	GetValue
	{ CPC_Yellow,  CPC_Black,	GaugeLens,	BrightnessGuage},
};
MenuItemType code BrightnessMenuItems[] =
{
    // 0 Brightness
    {
        3, 5, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // SelForeColor, SelBackColor;
        MainMenu,// NextMenuPage;
        DWI_Icon,// DrawMenuItemType;
        BrightnessText, // DisplayText;
        AdjusterKeyEvent,
        {
           AdjustBrightness,// AdjustFunction
           NULL,// ExecFunction
        },
        {
           DrawBrightnessNumber,// DrawNumberType
           DrawBrightnessGuage,// DrawGuageType
           NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};
#endif

#if LiteMAX_Baron_OSD_TEST
//============ Load Default MenuItem==============
MenuItemType code LoadDefaultMenuItems[] =
{
    // 0 YES
    {
        13, 3, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // SelForeColor, SelBackColor;
        RootMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        LoadDefaultText, // DisplayText;
        NaviExec2KeyEvent,
        {
            NULL, // AdjustFunction
            ResetAllSetting, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 1 NO
    {
        13, 3, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // SelForeColor, SelBackColor;
        MainMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        LoadDefaultText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//============ SoundAdjust MenuItem==============
#if AudioFunc
NumberType code SoundAdjustNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {  dwiEnd, GaugeXPos+GaugeLens-5, SubMenuValue_Y_Start, GetVolumeValue},
};//GaugeXPos+GaugeLens-5
DrawNumberType code DrawSoundAdjustNumber[] =
{
    // ForeColor, BackColor,  GetValue
    {CPC_White, CPC_Black,     SoundAdjustNumber}
};
GaugeType code SoundAdjustGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, SubMenuBar_Y_Start, GetVolumeValue},
};
DrawGuageType code DrawSoundAdjustGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {CPC_Yellow,  CPC_Black,    GaugeLens,  SoundAdjustGuage}
};
#endif
MenuItemType code SoundAdjustMenuItems[] =
{
    // 0 
    {
        NULL, NULL, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        SoundMenu,// NextMenuPage;
        DWI_Icon,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
#if AudioFunc // jason 20200507
        {
            AdjustVolume,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawSoundAdjustNumber,// DrawNumberType
            DrawSoundAdjustGuage,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
#else
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
#endif
        NULL, //Font
        mibSelectable // Flags
    },
};

MenuItemType code OSDControlMenuItems[] =
{
    // 0 OSD time
    {
        NULL,NULL,//6, 5, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        OSDTimeMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//C6500KText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 1 OSD HPos
    {
        NULL,NULL,//6, 5, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        OSDHPosMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//C6500KText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 2 OSD VPos
    {
        NULL,NULL,//6, 5, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        OSDVPosMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//C6500KText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
#if ENABLE_OSD_ROTATION
    // 3 OSD Rotation
    {
        NULL,NULL,//6, 5, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        OSDRotationMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//C6500KText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
#endif
    // 4 Exit
    {
        NULL,NULL,//6, 5, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        ToolMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//C6500KText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//============ ImageClock MenuItem==============
NumberType code ImageClockNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {  dwiEnd, GaugeXPos+GaugeLens-5, SubMenuValue_Y_Start, GetClockValue},
};//GaugeXPos+GaugeLens-5
DrawNumberType code DrawImageClockNumber[] =
{
    // ForeColor, BackColor,  GetValue
    {CPC_White, CPC_Black,     ImageClockNumber}
};
GaugeType code ImageClockGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, SubMenuBar_Y_Start, GetClockValue},
};
DrawGuageType code DrawImageClockGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {CPC_Yellow,  CPC_Black,    GaugeLens,  ImageClockGuage}
};
MenuItemType code ImageClockMenuItems[] =
{
    // 0
    {
        NULL, NULL, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        ImageMenu,// NextMenuPage;
        DWI_Icon,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustClock,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawImageClockNumber,// DrawNumberType
            DrawImageClockGuage,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//============ ImagePhase MenuItem==============
NumberType code ImagePhaseNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {  dwiEnd, GaugeXPos+GaugeLens-5, SubMenuValue_Y_Start, GetFocusValue},
};//GaugeXPos+GaugeLens-5
DrawNumberType code DrawImagePhaseNumber[] =
{
    // ForeColor, BackColor,  GetValue
    {CPC_White, CPC_Black,     ImagePhaseNumber}
};
GaugeType code ImagePhaseGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, SubMenuBar_Y_Start, GetFocusValue},
};
DrawGuageType code DrawImagePhaseGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {CPC_Yellow,  CPC_Black,    GaugeLens,  ImagePhaseGuage}
};
MenuItemType code ImagePhaseMenuItems[] =
{
    // 0
    {
        NULL, NULL, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        ImageMenu,// NextMenuPage;
        DWI_Icon,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustFocus,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawImagePhaseNumber,// DrawNumberType
            DrawImagePhaseGuage,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//============ ImageHPos MenuItem==============
NumberType code ImageHPosNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {  dwiEnd, GaugeXPos+GaugeLens-5, SubMenuValue_Y_Start, GetHPositionValue},
};//GaugeXPos+GaugeLens-5
DrawNumberType code DrawImageHPosNumber[] =
{
    // ForeColor, BackColor,  GetValue
    {CPC_White, CPC_Black,     ImageHPosNumber}
};
GaugeType code ImageHPosGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, SubMenuBar_Y_Start, GetHPositionValue},
};
DrawGuageType code DrawImageHPosGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {CPC_Yellow,  CPC_Black,    GaugeLens,  ImageHPosGuage}
};
MenuItemType code ImageHPosMenuItems[] =
{
    // 0
    {
        NULL, NULL, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        ImageMenu,// NextMenuPage;
        DWI_Icon,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustHPosition,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawImageHPosNumber,// DrawNumberType
            DrawImageHPosGuage,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//============ ImageVPos MenuItem==============
NumberType code ImageVPosNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {  dwiEnd, GaugeXPos+GaugeLens-5, SubMenuValue_Y_Start, GetVPositionValue},
};//GaugeXPos+GaugeLens-5
DrawNumberType code DrawImageVPosNumber[] =
{
    // ForeColor, BackColor,  GetValue
    {CPC_White, CPC_Black,     ImageVPosNumber}
};
GaugeType code ImageVPosGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, SubMenuBar_Y_Start, GetVPositionValue},
};
DrawGuageType code DrawImageVPosGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {CPC_Yellow,  CPC_Black,    GaugeLens,  ImageVPosGuage}
};
MenuItemType code ImageVPosMenuItems[] =
{
    // 0
    {
        NULL, NULL, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        ImageMenu,// NextMenuPage;
        DWI_Icon,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustVPosition,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawImageVPosNumber,// DrawNumberType
            DrawImageVPosGuage,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//============ OSDTime MenuItem==============
NumberType code OSDTimeNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {  dwiEnd, GaugeXPos+GaugeLens-5, SubMenuValue_Y_Start, GetOSDTime100Value},
};//GaugeXPos+GaugeLens-5
DrawNumberType code DrawOSDTimeNumber[] =
{
    // ForeColor, BackColor,  GetValue
    {CPC_White, CPC_Black,     OSDTimeNumber}
};
GaugeType code OSDTimeGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, SubMenuBar_Y_Start, GetOSDTime100Value},
};
DrawGuageType code DrawOSDTimeGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {CPC_Yellow,  CPC_Black,    GaugeLens,  OSDTimeGuage}
};
MenuItemType code OSDTimeMenuItems[] =
{
    // 0
    {
        NULL, NULL, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        OSDControlMenu,// NextMenuPage;
        DWI_Icon,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustOSDTime,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawOSDTimeNumber,// DrawNumberType
            DrawOSDTimeGuage,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//============ OSDHPos MenuItem==============
NumberType code OSDHPosNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {  dwiEnd, GaugeXPos+GaugeLens-5, SubMenuValue_Y_Start, GetOSDHPositionValue},
};//GaugeXPos+GaugeLens-5
DrawNumberType code DrawOSDHPosNumber[] =
{
    // ForeColor, BackColor,  GetValue
    {CPC_White, CPC_Black,     OSDHPosNumber}
};
GaugeType code OSDHPosGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, SubMenuBar_Y_Start, GetOSDHPositionValue},
};
DrawGuageType code DrawOSDHPosGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {CPC_Yellow,  CPC_Black,    GaugeLens,  OSDHPosGuage}
};
MenuItemType code OSDHPosMenuItems[] =
{
    // 0
    {
        NULL, NULL, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        OSDControlMenu,// NextMenuPage;
        DWI_Icon,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustOSDHPosition,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawOSDHPosNumber,// DrawNumberType
            DrawOSDHPosGuage,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//============ OSDVPos MenuItem==============
NumberType code OSDVPosNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {  dwiEnd, GaugeXPos+GaugeLens-5, SubMenuValue_Y_Start, GetOSDVPositionValue},
};//GaugeXPos+GaugeLens-5
DrawNumberType code DrawOSDVPosNumber[] =
{
    // ForeColor, BackColor,  GetValue
    {CPC_White, CPC_Black,     OSDVPosNumber}
};
GaugeType code OSDVPosGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, SubMenuBar_Y_Start, GetOSDVPositionValue},
};
DrawGuageType code DrawOSDVPosGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {CPC_Yellow,  CPC_Black,    GaugeLens,  OSDVPosGuage}
};
MenuItemType code OSDVPosMenuItems[] =
{
    // 0
    {
        NULL, NULL, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        OSDControlMenu,// NextMenuPage;
        DWI_Icon,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustOSDVPosition,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawOSDVPosNumber,// DrawNumberType
            DrawOSDVPosGuage,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};
#if ENABLE_OSD_ROTATION
MenuItemType code OSDRotationMenuItems[] =
{
    // 0 OSD 0
    {
        NULL,NULL,//6, 5, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        OSDRotationMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//C6500KText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetOSDRotateMode, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 1 OSD 90
    {
        NULL,NULL,//6, 5, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        OSDRotationMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//C6500KText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetOSDRotateMode, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 2 OSD 180
    {
        NULL,NULL,//6, 5, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        OSDRotationMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//C6500KText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetOSDRotateMode, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 3 OSD 270
    {
        NULL,NULL,//6, 5, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        OSDRotationMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//C6500KText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetOSDRotateMode, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 4 Exit
    {
        NULL,NULL,//6, 5, // XPos, YPos;
        NULL, NULL, // ForeColor, BackColor;
        NULL, NULL, // SelForeColor, SelBackColor;
        OSDControlMenu, // NextMenuPage;
        DWI_Icon, // DrawMenuItemType;
        NULL,//C6500KText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
};
#endif
#endif

#if AudioFunc
//--------------------VolumeNumber--------------------------
NumberType code VolumeNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*OtherSettingsMenuItems_Volume), GetVolumeValue},
};
DrawNumberType code DrawVolumeNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {CPC_White, CPC_Black,     VolumeNumber}
};

GaugeType code VolumeGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*OtherSettingsMenuItems_Volume), GetVolumeValue},
};
DrawGuageType code DrawVolumeGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {CPC_White, CPC_Black,  9,  VolumeGuage}
};

MenuItemType code VolumeMenuItems[] =
{
    // 0 Volume
    {
        Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_Volume), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        NULL,//OtherSettingsMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        VolumeText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustVolume,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawVolumeNumber,// DrawNumberType
            DrawVolumeGuage,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};
MenuItemType code AudioSourceSelMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*AudioSourceMenuItems_Digital), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        NULL, //OtherSettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        AudioSourceDigitalText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL,   //Adjust3DLUTMode, // AdjustFunction
            SetAudioSource,//SetGammaOnOff, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable// Flags
    },
    // 1
    {
        Layer3XPos, (LayerYPos+2*AudioSourceMenuItems_LineIn ), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        NULL,//OtherSettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        AudioSourceLineInText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL,   //Adjust3DLUTMode,	// AdjustFunction
            SetAudioSource,//SetGammaOnOff, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//--------------------VolumeNumber--------------------------
NumberType code SourceVolumeNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiRadioGroup | dwiEnd,        24,  3, GetVolumeValue},
};
DrawNumberType code DrawSourceVolumeNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {34, Color_4,     SourceVolumeNumber}
};

//--------------------VolumeIconGuge--------------------------
GaugeType code SourceVolumeGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiRadioGroup | dwiEnd,             16,    3,  GetVolumeValue},
};
DrawGuageType code DrawSourceVolumeGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0x22, Color_4,  9,  SourceVolumeGuage}
};

//--------------------AudioSourceText--------------------------
RadioTextType code DrawAudioSourceText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    //{dwiRadioGroup, 1, 5,   AudioSourceText},
    { dwiHotKeyCenterArrowAlign |dwiEnd, 22, 5,   AudioSourceValue},
};
DrawRadioGroupType code DrawAudioSourceRationText[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {34,  Color_4, NULL, DrawAudioSourceText}
};
#endif

MenuItemType code HotKeyBrightnessMenuItems[] =
{
    // 0 Brightness
    {
        8, 1, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black,  // SelForeColor, SelBackColor;
        RootMenu,///NextMenuPage;
        DWI_Icon,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustBrightness,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawHotBrightnessNumber,// DrawNumberType
            DrawHotBrightnessGuage,// DrawGuageType
            NULL,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};
#if AudioFunc
//============ Volume MenuItem==============
MenuItemType code HotKeyVolMenuItems[] =
{
    // 0 Volume
    {
        1, 4, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black,  // SelForeColor, SelBackColor;
        RootMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        VolumeText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustVolume,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawVolumeNumber,// DrawNumberType
            DrawVolumeGuage,// DrawGuageType
            NULL,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        MenuPage106, //Font
        mibSelectable // Flags
    },
};
#if (ENABLE_HDMI || ENABLE_DP_INPUT) && ENABLE_DAC
//============ SourceVolume MenuItem==============
MenuItemType code SourceVolumeMenuItems[] =
{
    // 0 Volume
    {
        1, 3, // XPos, YPos;
        74, Color_2, // ForeColor, BackColor;
        34, Color_4,  // SelForeColor, SelBackColor;
        RootMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        VolumeText, // DisplayText;
        AudioNaviKeyEvent,
        {
            NULL,//AdjustVolume,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            NULL,//DrawVolumeNumber,// DrawNumberType
            NULL,//DrawVolumeGuage,// DrawGuageType
            NULL,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 0 Volume
    {
        1, 3, // XPos, YPos;
        74, Color_2, // ForeColor, BackColor;
        34, Color_4,  // SelForeColor, SelBackColor;
        RootMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL,//VolumeText, // DisplayText;
        AudioAdjustKeyEvent,
        {
            AdjustVolume,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawSourceVolumeNumber,// DrawNumberType
            DrawSourceVolumeGuage,// DrawGuageType
            NULL,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        NULL,//mibSelectable // Flags
    },
    // 1 Audio Source Select          //111012 Rick create
    {
        1, 5, // XPos, YPos;
        74, Color_2, // ForeColor, BackColor;
        34, Color_4,  // SelForeColor, SelBackColor;
        RootMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        AudioSourceText,//AudioSourceText, // DisplayText;
        AudioNaviKeyEvent,
        {
            NULL,//AdjustAudioSource,//AdjustVolume,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            NULL,//DrawVolumeNumber,// DrawNumberType
            NULL,//DrawVolumeGuage,// DrawGuageType
            NULL,//DrawAudioSourceRationText,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },

    // 1 Audio Source Select          //111012 Rick create
    {
        22, 5, // XPos, YPos;
        74, Color_2, // ForeColor, BackColor;
        34, Color_4,  // SelForeColor, SelBackColor;
        RootMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL,//AudioSourceText,//AudioSourceText, // DisplayText;
        AudioAdjustKeyEvent,
        {
            NULL,//AdjustAudioSource,//AdjustVolume,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            NULL,//DrawVolumeNumber,// DrawNumberType
            NULL,//DrawVolumeGuage,//DrawVolumeGuage,// DrawGuageType
            DrawAudioSourceRationText,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        NULL,//mibSelectable // Flags
    },
};
#endif
#endif

#if (ENABLE_VGA_INPUT)
//--------------------HPositionNumber--------------------------
NumberType code HPositionNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*DisplaySettingsMenuItems_HPos), GetHPositionValue},
};
DrawNumberType code DrawHPositionNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {14, Color_2,     HPositionNumber}
};
//--------------------HPositionIconGuge--------------------------
GaugeType code HPositionGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*DisplaySettingsMenuItems_HPos), GetHPositionValue},
};
DrawGuageType code DrawHPositionGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {6,  Color_4,    9,  HPositionGuage}
};

MenuItemType code HPositionMenuItems[] =
{
    // 0 HPosition
    {
        Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_HPos), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_Red, CPC_Black, // SelForeColor, SelBackColor;
        MainMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        HPositionText, // DisplayText;
        AdjusterKey1Event,
        {
            AdjustHPosition,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawHPositionNumber,// DrawNumberType
            DrawHPositionGuage,// DrawGuageType
            NULL, // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable | mibVGAItem // Flags
    },
};

//--------------------VPositionNumber--------------------------
NumberType code VPositionNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*DisplaySettingsMenuItems_VPos), GetVPositionValue},
};
DrawNumberType code DrawVPositionNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {14, Color_2,     VPositionNumber}
};
//--------------------VPositionIconGuge--------------------------
GaugeType code VPositionGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*DisplaySettingsMenuItems_VPos), GetVPositionValue},
};
DrawGuageType code DrawVPositionGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {6,  Color_4,    9,  VPositionGuage}
};

MenuItemType code VPositionMenuItems[] =
{
    // 0 VPosition
    {
        Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_VPos), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_Red, CPC_Black, // SelForeColor, SelBackColor;
        MainMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        VPositionText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustVPosition,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawVPositionNumber,// DrawNumberType
            DrawVPositionGuage,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable | mibVGAItem // Flags
    },
};
#endif

#if ENABLE_SHARPNESS
//--------------------------------------------------------------------------
NumberType code SharpnessNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*DisplaySettingsMenuItems_Sharpness), GetSharpnessValue},
};
DrawNumberType code DrawSharpnessNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {14, Color_2,     SharpnessNumber}
};

GaugeType code SharpnessGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*DisplaySettingsMenuItems_Sharpness), GetSharpnessValue},
};
DrawGuageType code DrawSharpnessGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {6,  Color_4,    9,  SharpnessGuage}
};

MenuItemType code SharpnessMenuItems[] =
{
    // 0 Sharpness
    {
        Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_Sharpness), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_Red, CPC_Black, // SelForeColor, SelBackColor;
        NULL,//DisplaySettingsMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        SharpnessText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustSharpness,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawSharpnessNumber,// DrawNumberType
            DrawSharpnessGuage,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable //| mibVGAItem // Flags
    },
};
#endif

#if (ENABLE_VGA_INPUT)
//--------------------------------------------------------------------------
//--------------------ClockNumber--------------------------
NumberType code ClockNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*DisplaySettingsMenuItems_Clock), GetClockValue},
};
DrawNumberType code DrawClockNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {14, Color_2,     ClockNumber}
};
//--------------------ClockIconGuge--------------------------
GaugeType code ClockGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*DisplaySettingsMenuItems_Clock), GetClockValue},
};
DrawGuageType code DrawClockGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {6,  Color_4,    9,  ClockGuage}
};

MenuItemType code ClockMenuItems[] =
{
    // 0 Clock
    {
        Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_Clock), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_Red, CPC_Black, // SelForeColor, SelBackColor;
        MainMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        ClockText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustClock,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawClockNumber,// DrawNumberType
            DrawClockGuage,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable | mibVGAItem // Flags
    },
};

//--------------------------------------------------------------------------
//--------------------FocusNumber--------------------------
NumberType code FocusNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*DisplaySettingsMenuItems_Phase), GetFocusValue},
};
DrawNumberType code DrawFocusNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {14, Color_2,     FocusNumber}
};
//--------------------FocusIconGuge--------------------------
GaugeType code FocusGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*DisplaySettingsMenuItems_Phase), GetFocusValue},
};
DrawGuageType code DrawFocusGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {6,  Color_4,    9,  FocusGuage}
};
MenuItemType code FocusMenuItems[] =
{
    // Phase
    {
        Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_Phase), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_Red, CPC_Black, // SelForeColor, SelBackColor;
        MainMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        PhaseText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustFocus,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawFocusNumber,// DrawNumberType
            DrawFocusGuage,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable | mibVGAItem // Flags
    },
};

#endif

//--------------------------------------------------------------------------
NumberType code OSDTransNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*OtherSettingsMenuItems_OSDTrans), GetOSDTransparencyValue},
};
DrawNumberType code DrawOSDTransNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {14, Color_2,     OSDTransNumber}
};

GaugeType code OSDTransGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*OtherSettingsMenuItems_OSDTrans), GetOSDTransparencyValue},
};
DrawGuageType code DrawOSDTransGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {6,  Color_4,    9,  OSDTransGuage}
};
MenuItemType code OSDTransMenuItems[] =
{
    //
    {
        Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_OSDTrans), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        MainMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        OSDTransparencyText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustOSDTransparency,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawOSDTransNumber,// DrawNumberType
            DrawOSDTransGuage,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

#if LiteMAX_Baron_OSD_TEST
//============ ColorTemp MenuItem==============
MenuItemType code ColorTempMenuItems[] =
{
	// 0 6500K
	{
		6, 5, // XPos, YPos;
		CPC_White, CPC_Black, // ForeColor, BackColor;
		CPC_White, CPC_Black, // SelForeColor, SelBackColor;
		ColorTempMenu, // NextMenuPage;
		DWI_Icon, // DrawMenuItemType;
		DefaultText, // DisplayText;
		NaviExecKeyEvent,
		{
			NULL, // AdjustFunction
			OSD_SetColorTemp, // ExecFunction
		},
		{
			NULL, // DrawNumberType
			NULL, // DrawGuageType
			NULL  // DrawMenuRadioGroup,
		},
		NULL, //Font
		mibSelectable // Flags
	},
	// 1 CTEMP_USER
	{
		14, 5, // XPos, YPos;
		CPC_White, CPC_Black, // ForeColor, BackColor;
		CPC_White, CPC_Black, // SelForeColor, SelBackColor;
		ColorSettingsMenu, // NextMenuPage;
		DWI_Icon, // DrawMenuItemType;
		CUserText, // DisplayText;
		NaviExec2KeyEvent,
		{
			NULL, // AdjustFunction
			OSD_SetColorTemp, // ExecFunction
		},
		{
			NULL, // DrawNumberType
			NULL, // DrawGuageType
			NULL  // DrawMenuRadioGroup,
		},
		NULL, //Font
		mibSelectable // Flags
	},
	// 2 Return
	{
		24, 5, // XPos, YPos;
		CPC_White, CPC_Black, // ForeColor, BackColor;
		CPC_White, CPC_Black, // SelForeColor, SelBackColor;
		MainMenu, // NextMenuPage;
		DWI_Icon, // DrawMenuItemType;
		ReturnText, // DisplayText;
		NaviKeyEvent,
		{
			NULL, // AdjustFunction
			NULL, // ExecFunction
		},
		{
			NULL, // DrawNumberType
			NULL, // DrawGuageType
			NULL  // DrawMenuRadioGroup,
		},
		NULL, //Font
		mibSelectable // Flags
	},
};
#endif

MenuItemType code ColorFormatMenuItems[] =
{
    // RGB
    {
        Layer3XPos, (LayerYPos+2*INPUTCOLOR_RGB), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorSettingsMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        RGBText, // DisplayText;
        NaviExecFuncGotoPrevKeyEvent,
        {
            NULL, // AdjustFunction
            SetColorFormat, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, // Font
        mibSelectable//|mibVGAItem // Flags
    },
    // YUV
    {
        Layer3XPos, (LayerYPos+2*INPUTCOLOR_YUV), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorSettingsMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        YUVText, // DisplayText;
        NaviExecFuncGotoPrevKeyEvent,
        {
            NULL, // AdjustFunction
            SetColorFormat, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, // Font
        mibSelectable//|mibVGAItem // Flags
    },
    // Auto
    {
        Layer3XPos, (LayerYPos+2*INPUTCOLOR_AUTO), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorSettingsMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        AutoText, // DisplayText;
        NaviExecFuncGotoPrevKeyEvent,
        {
            NULL, // AdjustFunction
            SetColorFormat, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, // Font
        mibSelectable//|mibVGAItem // Flags
    },
};

//============ ColorTempSelectMenuItem==============
MenuItemType code ColorTempSelectMenuItems[] =
{
    // 0 ColorTempSelect
    {
        SUB_TEXT_XPOS, (SUB_TEXT_YPOS+COLORTEMP_COLORTEMPSELECT_ITEM*2),                        // XPos, YPos;
        14, Color_2,                                        // ForeColor, BackColor;
        12, Color_2,                                        // SelForeColor, SelBackColor;
        ColorTempMenu,                                  //NextMenuPage;
        DWI_Text,                                       // DrawMenuItemType;
        ColorTempText,                                  // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustColorTempMode,                        // AdjustFunction
            NULL,                                           // ExecFunction
        },
        {
            NULL,                                           // DrawNumberType
            NULL,                                           // DrawGuageType
            NULL        //DrawMenuRadioGroup,
        },
        NULL,//Font
        mibSelectable // Flags
    },
};

//============ UserColorRed MenuItem==============
//--------------------UserColorRedNumber--------------------------
NumberType code RedNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*ColorSettingsMenuItems_Red), GetRedColorValue},
};
DrawNumberType code DrawRedNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {CPC_MTK_Gold, CPC_Black, RedNumber}
};
//--------------------UserColorRedIconGuge--------------------------
GaugeType code RedGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*ColorSettingsMenuItems_Red), GetRedColorValue},
};
DrawGuageType code DrawRedGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {6,  Color_4,    9,  RedGuage}
};

MenuItemType code RedMenuItems[] =
{
    // 0 UserColorRedMENU
    {
        Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_Red), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorSettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        RedText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustRedColor,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawRedNumber,// DrawNumberType
            DrawRedGuage,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//============ UserColorGreen MenuItem==============
//--------------------UserColorGreenNumber--------------------------
NumberType code GreenNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*ColorSettingsMenuItems_Green),  GetGreenColorValue},
};
DrawNumberType code DrawGreenNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {CPC_MTK_Gold, CPC_Black, GreenNumber}
};
//--------------------UserColorGreenIconGuge--------------------------
GaugeType code GreenGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*ColorSettingsMenuItems_Green), GetGreenColorValue},
};
DrawGuageType code DrawGreenGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {6,  Color_4,    9,  GreenGuage}
};

MenuItemType code GreenMenuItems[] =
{
    // 0 UserColorGreenMENU
    {
        Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_Green), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorSettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        GreenText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustGreenColor,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawGreenNumber,// DrawNumberType
            DrawGreenGuage,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//============ UserColorBlue MenuItem==============
//--------------------UserColorBlueNumber--------------------------
NumberType code BlueNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*ColorSettingsMenuItems_Blue), GetBlueColorValue},
};
DrawNumberType code DrawBlueNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {CPC_MTK_Gold, CPC_Black, BlueNumber}
};
//--------------------UserColorBlueIconGuge--------------------------
GaugeType code BlueGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*ColorSettingsMenuItems_Blue), GetBlueColorValue},
};
DrawGuageType code DrawBlueGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {6,  Color_4,    9,  BlueGuage}
};

MenuItemType code BlueMenuItems[] =
{
    // 0 UserColorBlueMENU
    {
        Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_Blue), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorSettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        BlueText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustBlueColor,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawBlueNumber,// DrawNumberType
            DrawBlueGuage,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};


//--------------------------------------------
NumberType code HueNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*ColorSettingsMenuItems_Hue), GetHueValue},
};
DrawNumberType code DrawHueNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {CPC_MTK_Gold, CPC_Black, HueNumber}
};
//--------------------------------------------
GaugeType code HueGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*ColorSettingsMenuItems_Hue), GetHueValue},
};
DrawGuageType code DrawHueGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {6,  Color_4,    9,  HueGuage}
};
//--------------------------------------------
MenuItemType code HueMenuItems[] =
{
    {
        Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_Hue), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorSettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        HueText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustVideoHUE,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawHueNumber,// DrawNumberType
            DrawHueGuage,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//--------------------------------------------
NumberType code SaturationNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*ColorSettingsMenuItems_Saturation), GetSaturationValue},
};
DrawNumberType code DrawSaturationNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {CPC_MTK_Gold, CPC_Black, SaturationNumber}
};
//--------------------------------------------
GaugeType code SaturationGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*ColorSettingsMenuItems_Saturation), GetSaturationValue},
};
DrawGuageType code DrawSaturationGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {6,  Color_4,    9,  SaturationGuage}
};
//--------------------------------------------
MenuItemType code SaturationMenuItems[] =
{
    {
        Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_Saturation), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorSettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        SaturationText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustVideoSaturation,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawSaturationNumber,// DrawNumberType
            DrawSaturationGuage,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//----------------------------------------------
RadioTextType code InputSourceStatusRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  NULL, Layer3XPos, (LayerYPos+2*InputSourceMenuItems_SourceSelect), SourceSelectStatusText},
#if ENABLE_DP_INPUT
    {  dwiEnd, Layer3XPos, (LayerYPos+2*InputSourceMenuItems_DPVersion), DPVersionStatusText},
#else
    { dwiEnd, Layer3XPos, (LayerYPos+2*InputSourceMenuItems_Max), NullText},
#endif
};
DrawRadioGroupType code DrawInputSourceStatusRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, InputSourceStatusRatioText}
};
//----------------------------------------------
//------------- InputSelectMenuItem  --------------------

MenuItemType code InputSelectMenuItems[] =
{
    //Source Select
    {
        Layer2XPos, (LayerYPos+2*InputSourceMenuItems_SourceSelect), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        NULL,//SourceSelectMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        SourceSelectText,/*InputSelectText*/ // DisplayText;
        NaviKeyEvent,//AdjustExecKeyEvent,
        {
            NULL,//AdjustInputSource,// AdjustFunction
            NULL, //ChangeSource,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            DrawInputSourceStatusRatioGroup,//DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
#if ENABLE_DP_INPUT
    //DP Version
    {
        Layer2XPos, (LayerYPos+2*InputSourceMenuItems_DPVersion), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        NULL,//DPVersionMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        DPVersionText,/*InputSelectText*/ // DisplayText;
        NaviKeyEvent,//AdjustExecKeyEvent,
        {
            NULL,//AdjustInputSource,// AdjustFunction
            NULL, //ChangeSource,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            DrawInputSourceStatusRatioGroup,//DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
#endif
    // Return
    {
        Layer2XPos, (LayerYPos+2*InputSourceMenuItems_Return), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        RootMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        ReturnText, // DisplayText;
        ReturnEvent,
        {
            NULL,//AdjustInputSource,// AdjustFunction
            NULL, //ChangeSource,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,//DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

MenuItemType code SourceSelectMenuItems[] =
{
    //AUTO
    {
        Layer3XPos, (LayerYPos+2*SourceSelectMenuItems_Auto), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        NULL,//SourceSelectMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        AutoSelectText,/*InputSelectText*/ // DisplayText;
        NaviExecKeyEvent,//AdjustExecKeyEvent,
        {
            NULL,//AdjustInputSource,// AdjustFunction
            ChangeSource,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,//DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    #if (INPUT_TYPE&INPUT_1A)    
    //VGA
    {
        Layer3XPos, (LayerYPos+2*SourceSelectMenuItems_VGA), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_Red, CPC_Black, // SelForeColor, SelBackColor;
        NULL,//SourceSelectMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        VGAText,/*InputSelectText*/ // DisplayText;
        NaviExecKeyEvent,//AdjustExecKeyEvent,
        {
            NULL,//AdjustInputSource,// AdjustFunction
            ChangeSource,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,//DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    #endif
    #if (INPUT_TYPE>=INPUT_1C)
    //DIGITAL 0
    {
        Layer3XPos, (LayerYPos+2*SourceSelectMenuItems_Digital0), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        NULL,//SourceSelectMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        DigitalPort0Text,/*InputSelectText*/ // DisplayText;
        NaviExecKeyEvent,//AdjustExecKeyEvent,
        {
            NULL,//AdjustInputSource,// AdjustFunction
            ChangeSource,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,//DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    #endif
    #if (INPUT_TYPE>=INPUT_2C)
    //DIGITAL 1
    {
        Layer3XPos, (LayerYPos+2*SourceSelectMenuItems_Digital1), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        NULL, //SourceSelectMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        DigitalPort1Text,/*InputSelectText*/ // DisplayText;
        NaviExecKeyEvent,//AdjustExecKeyEvent,
        {
            NULL,//AdjustInputSource,// AdjustFunction
            ChangeSource,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,//DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    #endif
    #if (INPUT_TYPE>=INPUT_3C)
    //DIGITAL 2
    {
        Layer3XPos, (LayerYPos+2*SourceSelectMenuItems_Digital2), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        NULL, //SourceSelectMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        DigitalPort2Text, // DisplayText;
        NaviExecKeyEvent,//AdjustExecKeyEvent,
        {
            NULL,//AdjustInputSource,// AdjustFunction
            ChangeSource,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,//DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    #endif
    #if (INPUT_TYPE>=INPUT_4C)
    //DIGITAL 3
    {
        Layer3XPos, (LayerYPos+2*SourceSelectMenuItems_Digital3), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        NULL,//SourceSelectMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        DigitalPort3Text, // DisplayText;
        NaviExecKeyEvent,//AdjustExecKeyEvent,
        {
            NULL,//AdjustInputSource,// AdjustFunction
            ChangeSource,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,//DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    #endif
};

//------------- DCCCIMenuItem  --------------------
#if DDCCI_ENABLE
RadioTextType code DrawDDCCIIcon[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {dwiCenterArrowAlign|dwiEnd, (CENTER_ALIGN_ENDPOS-(CENTER_ALIGN_LEN/2)+1), (SUB_TEXT_YPOS+EXTRA_DDCCIONOFF_ITEM*2),    DDCCIValueText},
};
DrawRadioGroupType code DrawDDCCI2IconRatioText[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {14, Color_2, NULL, DrawDDCCIIcon}
};

MenuItemType code DDCCIMenuItems[] =
{
    {
        SUB_TEXT_XPOS, (SUB_TEXT_YPOS+EXTRA_DDCCIONOFF_ITEM*2), // XPos, YPos;
        14, Color_2, // ForeColor, BackColor;
        12, Color_2, // SelForeColor, SelBackColor;
        RootMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        DDCCIText, // DisplayText;
        AdjustNoExecKeyEvent, //Benz 2007.4.13   22:32:56    AdjustExecKeyEvent,

        {
            AdjustDDCCI,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            DrawDDCCI2IconRatioText,//DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

#endif


#if (ENABLE_VGA_INPUT)
//------------- AutoMenuItem  --------------------
MenuItemType code AutoMenuItems[] =
{
    {
        0, 5, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        RootMenu, // NextMenuPage;
        DWI_CenterText,// DrawMenuItemType;
        AutoAdjustProgressText, // DisplayText;
        NothingKeyEvent,// KeyEvent
        {
            NULL, // AdjustFunction
            NULL    // ExecFunction
        },// *DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL, // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    }
};
#endif


//------------- HotInputSelectMenuItem  --------------------
#if HotInputSelect
RadioTextType code DrawHotInputSelectText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {dwiEnd, 1, 1,   InputSourceText},

};
DrawRadioGroupType code DrawHotInputSelectRadioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Yellow, NULL, DrawHotInputSelectText} // 100831
};
MenuItemType code HotInputSelectMenuItems[] =
{
    //AUTO
    {
        HotLayerXPos, (HotLayerYPos+SourceSelectMenuItems_Auto), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_Yellow, CPC_Black, // SelForeColor, SelBackColor;
        MainMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        AutoSelectText, // DisplayText;
        NaviExecKeyEvent/*HotInputSelectKeyEvent*/,
        {
            NULL,// AdjustFunction
            ChangeSource,// ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawHotInputSelectRadioGroup, // DrawRadioGroupType
        },
        MenuPage103, // Font
        mibSelectable // Flags
    },
#if (INPUT_TYPE&INPUT_1A)
    //VGA
    {
        HotLayerXPos, (HotLayerYPos+SourceSelectMenuItems_VGA), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_Yellow, CPC_Black, // SelForeColor, SelBackColor;
        MainMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        VGAText, // DisplayText;
        NaviExecKeyEvent/*HotInputSelectKeyEvent*/,
        {
            NULL,// AdjustFunction
            ChangeSource // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawHotInputSelectRadioGroup, // DrawRadioGroupType
        },
        MenuPage103, // Font
        mibSelectable // Flags
    },
#endif
    #if (INPUT_TYPE>=INPUT_1C)
    //DIGITAL 0
    {
        HotLayerXPos, (HotLayerYPos+SourceSelectMenuItems_Digital0), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_Yellow, CPC_Black, // SelForeColor, SelBackColor;
        MainMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        DigitalPort0Text, // DisplayText;
        NaviExecKeyEvent/*HotInputSelectKeyEvent*/,
        {
            NULL,// AdjustFunction
            ChangeSource,// ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawHotInputSelectRadioGroup, // DrawRadioGroupType
        },
        MenuPage103, // Font
        mibSelectable // Flags
    },
    #endif
    #if (INPUT_TYPE>=INPUT_2C)
    //DIGITAL 1
    {
        HotLayerXPos, (HotLayerYPos+SourceSelectMenuItems_Digital1), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_Yellow, CPC_Black, // SelForeColor, SelBackColor;
        MainMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        DigitalPort1Text, // DisplayText;
        NaviExecKeyEvent/*HotInputSelectKeyEvent*/,
        {
            NULL,// AdjustFunction
            ChangeSource,// ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawHotInputSelectRadioGroup, // DrawRadioGroupType
        },
        MenuPage103, // Font
        mibSelectable // Flags
    },
    #endif
    #if (INPUT_TYPE>=INPUT_3C)
    //DIGITAL 2
    {
        HotLayerXPos, (HotLayerYPos+SourceSelectMenuItems_Digital2), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_Yellow, CPC_Black, // SelForeColor, SelBackColor;
        MainMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        DigitalPort2Text, // DisplayText;
        NaviExecKeyEvent/*HotInputSelectKeyEvent*/,
        {
            NULL,// AdjustFunction
            ChangeSource,// ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawHotInputSelectRadioGroup, // DrawRadioGroupType
        },
        MenuPage103, // Font
        mibSelectable // Flags
    },
    #endif
    #if (INPUT_TYPE>=INPUT_4C)
    //DIGITAL 2
    {
        HotLayerXPos, (HotLayerYPos+SourceSelectMenuItems_Digital3), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        MainMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        DigitalPort3Text, // DisplayText;
        NaviExecKeyEvent/*HotInputSelectKeyEvent*/,
        {
            NULL,// AdjustFunction
            ChangeSource,// ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawHotInputSelectRadioGroup, // DrawRadioGroupType
        },
        MenuPage103, // Font
        mibSelectable // Flags
    },
    #endif
};

#endif
//------------- HotKeyECOMenuItem  --------------------
RadioTextType code DrawHotKeyECOText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {dwiRadioGroup, 1, 4,   ECOModeText},
    {dwiOptionBar | dwiRadioGroup | dwiEnd, 19, 4,   ECOModeValue},

};
DrawRadioGroupType code DrawHotKeyECOIconRationText[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {73,  Color_2, NULL, DrawHotKeyECOText}
};
MenuItemType code HotKeyECOMenuItems[] =
{
    //0  Icon
    {
        0, 5, // XPos, YPos;
        14, Color_2, // ForeColor, BackColor;
        62,  Color_2, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_CenterText,// DrawMenuItemType;
        NULL,//LogoText, // DisplayText;
        ECOHotKeyEvent,// KeyEvent
        {
            AdjustECOMode, // AdjustFunction
            NULL    // ExecFunction
        },// *DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawHotKeyECOIconRationText// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    }
};

MenuItemType code ResetWaitMenuMenuItems[] =
{
    {
        0, 5, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        RootMenu, // NextMenuPage;
        DWI_CenterText,// DrawMenuItemType;
        ResetProgressText, // DisplayText;
        NothingKeyEvent,// KeyEvent
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },// *DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL, // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    }
};

#if (ENABLE_VGA_INPUT)
MenuItemType code ADCAutoColorMenuItems[] =
{
    {
        0, 5, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        RootMenu, // NextMenuPage;
        DWI_CenterText,// DrawMenuItemType;
        AutoColorProcessText, // DisplayText;
        NothingKeyEvent,// KeyEvent
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },// *DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL, // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    }
};
#endif

//------------- InputInfoMenu  --------------------
MenuItemType code InputInfoMenuItems[] =
{
    {
        0, 1, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        RootMenu, // NextMenuPage;
        DWI_CenterText,// DrawMenuItemType;
        InputStatusText, // DisplayText;
        NothingKeyEvent,// KeyEvent
        {
            NULL, // AdjustFunction
            NULL    // ExecFunction
        },// *DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL, // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    }
};

//------------- OSDLOCKMenuItem  --------------------
RadioTextType code DrawOsdLock[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {dwiRadioGroup | dwiEnd, 0, 4,     OsdLockText },

};
DrawRadioGroupType code DrawOsdLockIconRationText[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {62, Color_2, NULL, DrawOsdLock} // 100722 change the blue word
};

MenuItemType code OsdLockMenuItems[] =
{
    // OsdLockMenu
    {
        0, 4, // XPos, YPos;
        14, Color_2,// ForeColor, BackColor;
        14, Color_2, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        LockKeyEvent,// KeyEvent
        {
            NULL, // AdjustFunction
            NULL            // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawOsdLockIconRationText // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    }
};

#if DDCCI_ENABLE
//------------- DdcciInfoMenu  --------------------
RadioTextType code DrawDdcciInfo[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {dwiRadioGroup | dwiEnd,  20, 4,      DDCCIValueText},

};
DrawRadioGroupType code DrawDdcciInfoIconRationText[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {0, 6, NULL, DrawDdcciInfo}
};
MenuItemType code DdcciInfoMenuItems[] =
{
    //0 Reset Icon
    {
        4, 4, // XPos, YPos;
        CP_BlackColor, CP_WhiteColor, // ForeColor, BackColor;
        0,  6, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        DDCCIText, // DisplayText;
        LockKeyEvent,// KeyEvent //Benz 2007.4.24   21:48:06
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// *DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawDdcciInfoIconRationText   // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    }
};
#endif

MenuItemType code AutoColorMenuItems[] =
{
    //0 Reset Icon
    {
        10, 5, // XPos, YPos;
        CP_BlackColor, CP_WhiteColor, // ForeColor, BackColor;
        0,  6, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        AutoColorMenuText,//LogoText, // DisplayText;
        NothingKeyEvent,// KeyEvent
        {
            NULL, // AdjustFunction
            NULL    // ExecFunction
        },// *DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL, // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    }
};


//===============================================================
MenuItemType code BurninMenuItems[] =
{
    // Contrast Icon
    {
        2, 1, // XPos, YPos;
        CP_RedColor, CP_WhiteColor, // ForeColor, BackColor;
        CP_RedColor, CP_WhiteColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_CenterText,// DrawMenuItemType;
        NULL, // DisplayText;
        NothingKeyEvent,// KeyEvent
        {
            NULL, // AdjustFunction
            NULL            // ExecFunction
        },// *DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    }
};


MenuItemType code LogoMenuItems[] =
{
    //0 Reset Icon
    {
        0, 9, // 8, // XPos, YPos;
        CP_BlackColor, CP_WhiteColor, // ForeColor, BackColor;
        CP_BlackColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
#if 0 //Leo-temp       
        NULL,//DWI_Text,// DrawMenuItemType;
#else
        DWI_None, //NULL,//DWI_Text,// DrawMenuItemType;
#endif
        NULL,//LogoText, // DisplayText;
        LogoKeyEvent,// KeyEvent
        {
            NULL, // AdjustFunction
            NULL    // ExecFunction
        },// *DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    }
};

//==============================================================================================
MenuPageType code tblMenus[] =
{
    // 0 PowerOffMenu
    {
        16, 3, // XSize, YSize;
        PowerOnMenu,// PrevMenuPage;
        PowerOffMenuItems, // MenuItems;
        sizeof( PowerOffMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
#ifdef TSUMR2_FPGA
        NULL,
#else
        PowerOffSystem,// ExecFunction;
#endif
        NULL, // Fonts
        mpbInvisible | mpbStay //     Flags;
    },
    // 1 PowerOnMenu
    {
        16, 3, // XSize, YSize;
        RootMenu,// PrevMenuPage;
        NULL, // MenuItems;
        NULL, // MenuItemCount;
        NULL, //PowerOnSystem,// ExecFunction;
        NULL, // Fonts
        mpbInvisible //   Flags;
    },
//----Eson Start-----------------------------------------------------
    // StandbyMenu
    {
        MessageMenu2HSize, MessageMenu2VSize, //  XSize, YSize;
        RootMenu,// PrevMenuPage;
        StandbyMenuItems, // MenuItems;
        sizeof( StandbyMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, //EnablePowerDownCounter, // ExecFunction;
        MenuPage0, //MenuPage2, // Fonts
        mpbStay | mpbRedraw | mpbCenter | mpbMoving// Flags; //mpbStay | mpbRedraw | mpbCenter // Flags;
    },
    // CableNotConnectMenu
    {
        MessageMenu2HSize, MessageMenu2VSize, //  XSize, YSize;
        RootMenu,// PrevMenuPage;
        CableNotConnectedMenuItems, // MenuItems;
        sizeof( CableNotConnectedMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL,//OsdTimerCountDown, //EnablePowerDownCounter, //NULL, // ExecFunction;
        MenuPage0, //MenuPage4, // Fonts
        mpbStay | mpbRedraw | mpbCenter | mpbMoving// Flags; //mpbMoving | mpbStay | mpbCenter // Flags;
    },
    // UnsupportedModeMenu
    {
        MessageMenuHSize, MessageMenuVSize, //  XSize, YSize;
        RootMenu,// PrevMenuPage;
        UnsupportedModeMenuItems, // MenuItems;
        sizeof( UnsupportedModeMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL,//OsdTimerCountDown, //NULL, // ExecFunction;
        MenuPage0, //MenuPage5, // Fonts
        mpbStay | mpbRedraw | mpbCenter | mpbMoving// Flags; //mpbMoving | mpbStay | mpbCenter // Flags;
    },
    // RootMenu
    {
        16, 3, // XSize, YSize;
        RootMenu,// PrevMenuPage;
        RootMenuItems, // MenuItems;
        sizeof( RootMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        EnterRootMenu, // ExecFunction;
        NULL, // Fonts
        mpbInvisible | mpbStay //   Flags;
    },
    #if LiteMAX_Baron_OSD_TEST	
	// MainMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        RootMenu,// PrevMenuPage;
        MainMenuItems, // MenuItems;
        sizeof( MainMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        MenuPage0, // Fonts
        mpbStay | mpbRedraw //   Flags;
    },
    // LuminanceMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        MainMenu,// PrevMenuPage;
        LuminanceMenuItems, // MenuItems;
        sizeof( LuminanceMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
    // SignalMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        MainMenu,// PrevMenuPage;
        SignalMenuItems, // MenuItems;
        sizeof( SignalMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
    // SoundMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        MainMenu,// PrevMenuPage;
        SoundMenuItems, // MenuItems;
        sizeof( SoundMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
    // ColorMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        MainMenu,// PrevMenuPage;
        ColorMenuItems, // MenuItems;
        sizeof( ColorMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay  //   Flags;
    },
    // ImageMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        MainMenu,// PrevMenuPage;
        ImageMenuItems, // MenuItems;
        sizeof( ImageMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay  //   Flags;
    },
    // ToolMenu,
    {
        MAIN_MENU_H_SIZE, MAIN_MENU_V_SIZE, // XSize, YSize;
        MainMenu,// PrevMenuPage;
        ToolMenuItems ,// MenuItems;
        sizeof( ToolMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // Fonts
        mpbStay //   Flags;
    },
    #endif
    #if LiteMAX_Baron_OSD_TEST
	// ColorSettingsMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        ColorTempMenu,// PrevMenuPage;
        ColorSettingsMenuItems, // MenuItems;
        sizeof( ColorSettingsMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay  //   Flags;
    },
    #endif
	// InputSourceMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        MainMenu,// PrevMenuPage;
        InputSelectMenuItems, // MenuItems;
        sizeof( InputSelectMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay  //   Flags;
    },
	// ContrastMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        MainMenu,// PrevMenuPage;
        ContrastMenuItems, // MenuItems;
        sizeof( ContrastMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // BrightnessMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        MainMenu,// PrevMenuPage;
        BrightnessMenuItems, // MenuItems;
        sizeof( BrightnessMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // SaturationMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        ColorSettingsMenu,// PrevMenuPage;
        SaturationMenuItems, // MenuItems;
        sizeof( SaturationMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
#if (ENABLE_VGA_INPUT)
    // HPositionMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        MainMenu,// PrevMenuPage;
        HPositionMenuItems, // MenuItems;
        sizeof( HPositionMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // VPositionMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        MainMenu,// PrevMenuPage;
        VPositionMenuItems, // MenuItems;
        sizeof( VPositionMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
#endif
#if ENABLE_SHARPNESS
    // SharpnessMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        MainMenu,// PrevMenuPage;
        SharpnessMenuItems, // MenuItems;
        sizeof( SharpnessMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
#endif
#if (ENABLE_VGA_INPUT)
    // ClockMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        MainMenu,// PrevMenuPage;
        ClockMenuItems, // MenuItems;
        sizeof( ClockMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // FocusMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        MainMenu,// PrevMenuPage;
        FocusMenuItems, // MenuItems;
        sizeof( FocusMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
#endif
    // OSDTransMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        MainMenu,// PrevMenuPage;
        OSDTransMenuItems, // MenuItems;
        sizeof( OSDTransMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    #if LiteMAX_Baron_OSD_TEST
	// ColorTempMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        ColorMenu,// PrevMenuPage;
        ColorTempMenuItems, // MenuItems;
        sizeof( ColorTempMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
    #endif
	// SourceSelectMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //    XSize, YSize;
        InputSourceMenu,// PrevMenuPage;
        SourceSelectMenuItems, // MenuItems;
        sizeof( SourceSelectMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
#if (ENABLE_VGA_INPUT)
    // ADCAutoColorMenu,
    {
        MessageMenuHSize, MessageMenuVSize, //  XSize, YSize;
        RootMenu,// PrevMenuPage;
        ADCAutoColorMenuItems ,// MenuItems;
        sizeof( ADCAutoColorMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        AutoColor, // ExecFunction;
        MenuPage11, // // Fonts
        mpbCenter //   Flags;
    },
    // AutoMenu,
    {
        MessageMenuHSize, MessageMenuVSize, //  XSize, YSize;
        RootMenu,// PrevMenuPage;
        AutoMenuItems ,// MenuItems;
        sizeof( AutoMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        AutoConfig, // ExecFunction;
        MenuPage8, // // Fonts
        mpbCenter//   Flags;
    },
#endif
    // InputInfoMenu
    {
        MessageMenuHSize, MessageMenuVSize, //  XSize, YSize;
        RootMenu,// PrevMenuPage;
        InputInfoMenuItems, // MenuItems;
        sizeof( InputInfoMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        MenuPage0, //MenuPage10, // Fonts
        mpbStay | mpbRedraw | mpbCenter// Flags; //mpbStay | mpbCenter //   Flags;
    },
    #if HotInputSelect
    // HotInputSelectMenu
    {
        30 , (5+SourceSelectMenuItems_Max),
        RootMenu,// PrevMenuPage;
        HotInputSelectMenuItems ,// MenuItems;
        sizeof( HotInputSelectMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL ,// ExecFunction;
        MenuPage0, // Fonts
        mpbRedraw | mpbStay | mpbCenter //   Flags;
    },
#endif
	//20 ColorTempSelectMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        ColorTempMenu,// PrevMenuPage;
        ColorTempSelectMenuItems, // MenuItems;
        sizeof( ColorTempSelectMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL,//SetColorTemp, // NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay | mpbBrowse //   Flags;
    },
#if INPUT_TYPE!=INPUT_1A
    //50 InputSelectMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        MainMenu,// PrevMenuPage;
        InputSelectMenuItems ,// MenuItems;
        sizeof( InputSelectMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay | mpbBrowse //   Flags;
    },
#endif
#if DDCCI_ENABLE
    //52 DDCCIMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        RootMenu,// PrevMenuPage;
        DDCCIMenuItems ,// MenuItems;
        sizeof( DDCCIMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay | mpbBrowse //   Flags;
    },
#endif
    // 54 HotKeyECOMenu,
    {
        31/*29*/, 9, // XSize, YSize;
        RootMenu,// PrevMenuPage;
        HotKeyECOMenuItems ,// MenuItems;
        sizeof( HotKeyECOMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // Fonts
        mpbStay | mpbCenter| mpbBrowse //|mpbRedraw //   Flags;
    },

    // 54 HotKeyBrightnessMenu,
    {
        HotMenuHSize, HotMenuVSize, // XSize, YSize;
        RootMenu,// PrevMenuPage;
        HotKeyBrightnessMenuItems ,// MenuItems;
        sizeof( HotKeyBrightnessMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // Fonts
        mpbStay | mpbCenter |mpbBrowse //|mpbRedraw //   Flags;
    },
#if AudioFunc
    // 54 HotKeyVolMenu,
    {
        31/*29*/, 9, // XSize, YSize;
        RootMenu,// PrevMenuPage;
        HotKeyVolMenuItems ,// MenuItems;
        sizeof( HotKeyVolMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // Fonts
        mpbStay | mpbCenter |mpbBrowse //|mpbRedraw //   Flags;
    },
#if (ENABLE_HDMI || ENABLE_DP_INPUT) && ENABLE_DAC
    // 55 HotKeySourceVolMenu,
    {
        31/*29*/, 9, // XSize, YSize;
        RootMenu,// PrevMenuPage;
        SourceVolumeMenuItems ,// MenuItems;
        sizeof( SourceVolumeMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // Fonts
        mpbStay | mpbCenter |mpbBrowse //|mpbRedraw //   Flags;
    },

#endif
#endif

    //56 OsdLockMenu,
    {
        31 , 9, //  XSize, YSize;
        RootMenu,// PrevMenuPage;
        OsdLockMenuItems ,// MenuItems;
        sizeof( OsdLockMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        OsdCountDown, // ExecFunction;
        NULL, // Fonts
        mpbStay | mpbCenter //   Flags;
    },

    // 57 AutoColorMenu
    {
        31/*29*/, 13, //  XSize, YSize;
        RootMenu,// PrevMenuPage;
        AutoColorMenuItems, // MenuItems;
        sizeof( AutoColorMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        AutoColor, // ExecFunction;
        NULL, // Fonts
        mpbCenter | mpbRedraw //   Flags;
    },
    // 58 BurninMenu
    {
        29, 13, //    XSize, YSize;
        RootMenu,// PrevMenuPage;
        BurninMenuItems, // MenuItems;
        sizeof( BurninMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        EnableBurninMode, // ExecFunction;
        NULL, // Fonts
        mpbStay | mpbInvisible //   Flags;
    },
#if  1
    // 59 FactoryMenu
    {
        FACTORY_MENU_H_SIZE, FACTORY_MENU_V_SIZE,  // XSize, YSize;
        RootMenu,// PrevMenuPage;
        FactoryMenuItems, // MenuItems;
        sizeof( FactoryMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // Fonts
        mpbStay //   Flags;
    },
#endif
    // 60 LogoMenu
    {
        42, 11, //    XSize, YSize;
        MainMenu,// PrevMenuPage;
        LogoMenuItems, // MenuItems;
        sizeof( LogoMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        EnterDisplayLogo, // ExecFunction;
        NULL, // Fonts
        mpbStay | mpbCenter | mpbLogoFrame //   Flags;
    },
    #if LiteMAX_Baron_OSD_TEST
    // SoundAdjustMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        SoundMenu,// PrevMenuPage;
        SoundAdjustMenuItems, // MenuItems;
        sizeof( SoundAdjustMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
    // ImageClockMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        ImageMenu,// PrevMenuPage;
        ImageClockMenuItems, // MenuItems;
        sizeof( ImageClockMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
    // ImagePhaseMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        ImageMenu,// PrevMenuPage;
        ImagePhaseMenuItems, // MenuItems;
        sizeof( ImagePhaseMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
    // ImageHPosMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        ImageMenu,// PrevMenuPage;
        ImageHPosMenuItems, // MenuItems;
        sizeof( ImageHPosMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
    // ImageVPosMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        ImageMenu,// PrevMenuPage;
        ImageVPosMenuItems, // MenuItems;
        sizeof( ImageVPosMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
    // OSDControlMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        ToolMenu,// PrevMenuPage;
        OSDControlMenuItems, // MenuItems;
        sizeof( OSDControlMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
    // OSDTimeMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        OSDControlMenu,// PrevMenuPage;
        OSDTimeMenuItems, // MenuItems;
        sizeof( OSDTimeMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
    // OSDHPosMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        OSDControlMenu,// PrevMenuPage;
        OSDHPosMenuItems, // MenuItems;
        sizeof( OSDHPosMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
    // OSDVPosMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        OSDControlMenu,// PrevMenuPage;
        OSDVPosMenuItems, // MenuItems;
        sizeof( OSDVPosMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
#if ENABLE_OSD_ROTATION
    // OSDRotationMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        OSDControlMenu,// PrevMenuPage;
        OSDRotationMenuItems, // MenuItems;
        sizeof( OSDRotationMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
#endif
	#if LiteMAX_Baron_OSD_TEST
	// DefaultMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        MainMenu,// PrevMenuPage;
        LoadDefaultMenuItems, // MenuItems;
        sizeof( LoadDefaultMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay  //   Flags;
    },
    #endif
	
    #endif
};




