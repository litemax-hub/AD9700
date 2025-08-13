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
    {  NULL, Layer2XPos, (LayerYPos+2*BriteContMenuItems_DCR), DCRText},
#if (ENABLE_VGA_INPUT)
    {  dwiVGAItem, Layer2XPos, (LayerYPos+2*BriteContMenuItems_AutoColor), ADCAutoColorText},
#endif
    #if ENABLE_DLC
    {  NULL, Layer3XPos, (LayerYPos+2*BriteContMenuItems_DLC), DLCStatusText},
    #endif
    #if ENABLE_DPS
    {  NULL, Layer3XPos, (LayerYPos+2*BriteContMenuItems_DPS), DPSStatusText},
    #endif
    {dwiEnd, Layer3XPos, (LayerYPos+2*BriteContMenuItems_DCR), DCRStatusText},
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
    {  NULL, Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_ColorMode), ColorModeText},
    {  NULL, Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_ColorFormat), ColorFormatText},

    {  NULL, Layer3XPos, (LayerYPos+2*ColorSettingsMenuItems_ColorTemp), ColorTempStatusText},
#if ENABLE_SUPER_RESOLUTION
    {  NULL, Layer3XPos, (LayerYPos+2*ColorSettingsMenuItems_SuperResolution), SuperResolutionStatusText},
#endif
    {  NULL, Layer3XPos, (LayerYPos+2*ColorSettingsMenuItems_ColorMode), ColorModeStatusText},
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
    {  NULL, Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_DisplayLogo), DisplayLogoText},
    {  NULL, Layer3XPos, (LayerYPos+2*OtherSettingsMenuItems_DisplayLogo), DisplayLogoStatusText},
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
    { NULL, Layer2XPos, (LayerYPos+2*PowerManagerMenuItems_DCOffDischarge), PowerManagerDCOffDischargeText},
    {dwiEnd, Layer2XPos, (LayerYPos+2*PowerManagerMenuItems_PowerSaving), PowerManagerPowerSavingText},
};
DrawRadioGroupType code DrawPowerManagerMenuRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, PowerManagerMenuRatioText}
};

//=========================================================
MenuItemType code MainMenuItems[] =
{
    // 0
    {
        Layer1XPos, (LayerYPos+2*MainMenuItems_BriteCont), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        BriteContMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        BriteContText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            DrawBriteContMenuNumber, // DrawNumberType
            DrawBriteContMenutGuage, // DrawGuageType
            DrawBriteContMenuRatioGroup // DrawRadioGroupType
        },
        MenuPage101, // Font
        mibSelectable // Flags
    },
    // 1
    {
        Layer1XPos, (LayerYPos+2*MainMenuItems_ColorSettings), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorSettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        ColorSettingsText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            DrawColorSettingsMenuNumber, // DrawNumberType
            DrawColorSettingsMenuGuage, // DrawGuageType
            DrawColorSettingsMenuRatioGroup, // DrawRadioGroupType
        },
        MenuPage102, // Font
        mibSelectable // Flags
    },

    // 2
    {
        Layer1XPos, (LayerYPos+2*MainMenuItems_ExtColorSettings), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ExtColorSettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        ExtColorSettingsText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawExtColorSettingsMenuRatioGroup, // DrawRadioGroupType
        },
        MenuPage108, // Font
        mibSelectable // Flags
    },

    // 3
    {
        Layer1XPos, (LayerYPos+2*MainMenuItems_InputSource), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        InputSourceMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        InputSourceText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawInputSourceMenuRatioGroup, // DrawRadioGroupType
        },
        MenuPage103, // Font
        mibSelectable // Flags
    },

    // 4
    {
        Layer1XPos, (LayerYPos+2*MainMenuItems_DisplaySettings), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        DisplaySettingsMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        DisplaySettingsText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,   // AdjustFunction
            NULL,   // ExecFunction
        },
        {
            DrawDisplaySettingsMenuNumber, // DrawNumberType
            DrawDisplaySettingsMenuGuage, // DrawGuageType
            DrawDisplaySettingsMenuRatioGroup, // DrawRadioGroupType
        },
        MenuPage105, //Font
        mibSelectable // Flags
    },
    // 5
    {
        Layer1XPos, (LayerYPos+2*MainMenuItems_OtherSettings), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        OtherSettingsMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        OtherSettingsText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,   // AdjustFunction
            NULL,   // ExecFunction
        },
        {
            DrawOtherSettingsMenuNumber, // DrawNumberType
            DrawOtherSettingsMenuGuage, // DrawGuageType
            DrawOtherSettingsMenuRatioGroup, // DrawRadioGroupType
        },
        MenuPage106, //Font
        mibSelectable // Flags
    },
    // 6
    {
        Layer1XPos, (LayerYPos+2*MainMenuItems_PowerManager), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        PowerManagerMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        PowerManagerText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,   // AdjustFunction
            NULL,   // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawPowerManagerMenuRatioGroup, // DrawRadioGroupType
        },
        MenuPage109, //Font
        mibSelectable // Flags
    },
    // 7
    {
        Layer1XPos, (LayerYPos+2*MainMenuItems_Information), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        MainMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        InformationText, // DisplayText;
        InformationEvent,
        {
            NULL,   // AdjustFunction
            NULL,   // ExecFunction
        },
        {
            NULL,   // DrawNumberType
            NULL,   // DrawGuageType
            NULL,   // DrawRadioGroupType
        },
        MenuPage107, //Font
        mibSelectable // Flags
    },

};

//=========================================================
RadioTextType code BriteContMenuStatusRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    #if ENABLE_DLC
    {  NULL, Layer3XPos, (LayerYPos+2*BriteContMenuItems_DLC), DLCStatusText},
    #endif
    #if ENABLE_DPS
    {  NULL, Layer3XPos, (LayerYPos+2*BriteContMenuItems_DPS), DPSStatusText},
    #endif
    {dwiEnd, Layer3XPos, (LayerYPos+2*BriteContMenuItems_DCR), DCRStatusText},
};
DrawRadioGroupType code DrawBriteContMenuStatusRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, BriteContMenuStatusRatioText}
};

//---------------------------------------------------------------
MenuItemType code BriteContMenuItems[] =
{
    // 0 Brightness
    {
        Layer2XPos, (LayerYPos+2*BriteContMenuItems_Brightness), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        BrightnessMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        BrightnessText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawBriteContMenuNumber,// DrawNumberType
            DrawBriteContMenutGuage, // DrawGuageType
            DrawBriteContMenuStatusRatioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable | mibsRGBDisable | mibDCRDisable  // Flags
    },
    // 1 Contrast
    {
        Layer2XPos, (LayerYPos+2*BriteContMenuItems_Contrast), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ContrastMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        ContrastText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawBriteContMenuNumber,// DrawNumberType
            DrawBriteContMenutGuage, // DrawGuageType
            DrawBriteContMenuStatusRatioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable | mibsRGBDisable | mibDCRDisable  // Flags
    },
    #if ENABLE_DLC
    // 2 DLC
    {
        Layer2XPos, (LayerYPos+2*BriteContMenuItems_DLC), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        DLCMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        DLCText,  // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawBriteContMenuNumber,// DrawNumberType
            DrawBriteContMenutGuage, // DrawGuageType
            DrawBriteContMenuStatusRatioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    #endif
    #if ENABLE_DPS
    // 3 DPS
    {
        Layer2XPos, (LayerYPos+2*BriteContMenuItems_DPS), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        DPSMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        DPSText,  // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawBriteContMenuNumber,// DrawNumberType
            DrawBriteContMenutGuage, // DrawGuageType
            DrawBriteContMenuStatusRatioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    #endif
    // 4 DCR
    {
        Layer2XPos, (LayerYPos+2*BriteContMenuItems_DCR), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        DCRMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        DCRText,  // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawBriteContMenuNumber,// DrawNumberType
            DrawBriteContMenutGuage, // DrawGuageType
            DrawBriteContMenuStatusRatioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
#if (ENABLE_VGA_INPUT)
    // 5 AutoColor
    {
        Layer2XPos, (LayerYPos+2*BriteContMenuItems_AutoColor), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_Red, CPC_Black, // SelForeColor, SelBackColor;
        ADCAutoColorMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        ADCAutoColorText,// DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL, // DrawGuageType
            NULL // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable | mibVGAItem // Flags
    },
#endif

};

//---------------------------------------------------------
RadioTextType code ColorSettingsMenuStatusRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  NULL, Layer3XPos, (LayerYPos+2*ColorSettingsMenuItems_ColorTemp), ColorTempStatusText},
#if ENABLE_SUPER_RESOLUTION
    {  NULL, Layer3XPos, (LayerYPos+2*ColorSettingsMenuItems_SuperResolution), SuperResolutionStatusText},
#endif
    {  NULL, Layer3XPos, (LayerYPos+2*ColorSettingsMenuItems_ColorMode), ColorModeStatusText},
    {dwiEnd, Layer3XPos, (LayerYPos+2*ColorSettingsMenuItems_ColorFormat), ColorFormatStatusText},
};
DrawRadioGroupType code DrawColorSettingsMenuStatusRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, ColorSettingsMenuStatusRatioText}
};
//---------------------------------------------------------
MenuItemType code ColorSettingsMenuItems[] =
{
    // 0 color temp
    {
        Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_ColorTemp), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorTempMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        ColorTempText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawColorSettingsMenuNumber, // DrawNumberType
            DrawColorSettingsMenuGuage, // DrawGuageType
            DrawColorSettingsMenuStatusRatioGroup, // DrawRadioGroupType
        },
        NULL, // Font
        mibSelectable // Flags
    },
    // 1 red
    {
        Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_Red), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        RedMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        RedText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawColorSettingsMenuNumber, // DrawNumberType
            DrawColorSettingsMenuGuage, // DrawGuageType
            DrawColorSettingsMenuStatusRatioGroup, // DrawRadioGroupType
        },
        NULL, // Font
        mibSelectable | mibUserColor // Flags
    },
    // 2 green
    {
        Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_Green), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        GreenMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        GreenText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawColorSettingsMenuNumber, // DrawNumberType
            DrawColorSettingsMenuGuage, // DrawGuageType
            DrawColorSettingsMenuStatusRatioGroup, // DrawRadioGroupType
        },
        NULL, // Font
        mibSelectable | mibUserColor // Flags
    },
    // 3 blue
    {
        Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_Blue), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        BlueMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        BlueText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawColorSettingsMenuNumber, // DrawNumberType
            DrawColorSettingsMenuGuage, // DrawGuageType
            DrawColorSettingsMenuStatusRatioGroup, // DrawRadioGroupType
        },
        NULL, // Font
        mibSelectable | mibUserColor // Flags
    },
    // 4 hue
    {
        Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_Hue), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        HueMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        HueText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawColorSettingsMenuNumber, // DrawNumberType
            DrawColorSettingsMenuGuage, // DrawGuageType
            DrawColorSettingsMenuStatusRatioGroup, // DrawRadioGroupType
        },
        NULL, // Font
        mibSelectable | mibUserColor//|mibYUVEnable // Flags
    },
    // 5 saturation
    {
        Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_Saturation), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        SaturationMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        SaturationText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawColorSettingsMenuNumber, // DrawNumberType
            DrawColorSettingsMenuGuage, // DrawGuageType
            DrawColorSettingsMenuStatusRatioGroup, // DrawRadioGroupType
        },
        NULL, // Font
        mibSelectable | mibUserColor//|mibYUVEnable // Flags
    },
#if ENABLE_SUPER_RESOLUTION
    // 6 super resolution
    {
        Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_SuperResolution), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        SuperResolutionMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        SuperResolutionText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawColorSettingsMenuNumber, // DrawNumberType
            DrawColorSettingsMenuGuage, // DrawGuageType
            DrawColorSettingsMenuStatusRatioGroup, // DrawRadioGroupType
        },
        NULL, // Font
        mibSelectable // Flags
    },
#endif
    // 7 color mode
    {
        Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_ColorMode), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorModeMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        ColorModeText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawColorSettingsMenuNumber, // DrawNumberType
            DrawColorSettingsMenuGuage, // DrawGuageType
            DrawColorSettingsMenuStatusRatioGroup, // DrawRadioGroupType
        },
        NULL, // Font
        mibSelectable // Flags
    },
    // 8 color format
    {
        Layer2XPos, (LayerYPos+2*ColorSettingsMenuItems_ColorFormat), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorFormatMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        ColorFormatText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawColorSettingsMenuNumber, // DrawNumberType
            DrawColorSettingsMenuGuage, // DrawGuageType
            DrawColorSettingsMenuStatusRatioGroup, // DrawRadioGroupType
        },
        NULL, // Font
        mibSelectable//|mibVGAItem // Flags
    },
};


//---------------------------------------------------------
RadioTextType code ExtColorSettingsMenuStatusRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  dwiEnd, Layer3XPos, (LayerYPos+2*ExtColorSettingsMenuItems_ColorRange), ColorRangeStatusText},
};
DrawRadioGroupType code DrawExtColorSettingsMenuStatusRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, ExtColorSettingsMenuStatusRatioText}
};
//---------------------------------------------------------
MenuItemType code ExtColorSettingsMenuItems[] =
{
    // 0 color range
    {
        Layer2XPos, (LayerYPos+2*ExtColorSettingsMenuItems_ColorRange), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorRangeMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        ColorRangeText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawExtColorSettingsMenuStatusRatioGroup, // DrawRadioGroupType
        },
        NULL, // Font
        mibSelectable // Flags
    },
    // 1 Independent Hue
    {
        Layer2XPos, (LayerYPos+2*ExtColorSettingsMenuItems_IndependentHue), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentHueMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        IndependentHueText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawExtColorSettingsMenuStatusRatioGroup, // DrawRadioGroupType
        },
        NULL, // Font
        mibSelectable // Flags
    },
    // 2 Independent Saturation
    {
        Layer2XPos, (LayerYPos+2*ExtColorSettingsMenuItems_IndependentSaturation), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentSaturationMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        IndependentSaturationText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawExtColorSettingsMenuStatusRatioGroup, // DrawRadioGroupType
        },
        NULL, // Font
        mibSelectable // Flags
    },
    // 3 Independent Brightness
    {
        Layer2XPos, (LayerYPos+2*ExtColorSettingsMenuItems_IndependentBrightness), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentBrightnessMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        IndependentBrightnessText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawExtColorSettingsMenuStatusRatioGroup, // DrawRadioGroupType
        },
        NULL, // Font
        mibSelectable // Flags
    },
};

//----------------------------------------------------------
RadioTextType code DisplaySettingsMenuStatusRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  NULL, Layer3XPos, (LayerYPos+2*DisplaySettingsMenuItems_Gamma), GammaStatusText},
    #if Enable_Expansion
    {  NULL, Layer3XPos, (LayerYPos+2*DisplaySettingsMenuItems_Aspect), AspectRatioStatusText},
    #endif
    #if ENABLE_RTE
    {  NULL, Layer3XPos, (LayerYPos+2*DisplaySettingsMenuItems_OD), OverdriveStatusText},
    #endif
    {dwiEnd, Layer3XPos, (LayerYPos+2*DisplaySettingsMenuItems_Max), NullText},
};
DrawRadioGroupType code DrawDisplaySettingsMenuStatusRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, DisplaySettingsMenuStatusRatioText}
};

MenuItemType code DisplaySettingsMenuItems[] =
{
    // 0
    {
        Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_Gamma), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        GammaMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        GammaText,// DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawDisplaySettingsMenuNumber,   // DrawNumberType
            DrawDisplaySettingsMenuGuage,   // DrawGuageType
            DrawDisplaySettingsMenuStatusRatioGroup,   // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
#if (ENABLE_VGA_INPUT)
    // 1
    {
        Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_HPos), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_Red, CPC_Black, // SelForeColor, SelBackColor;
        HPositionMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        HPositionText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawDisplaySettingsMenuNumber,   // DrawNumberType
            DrawDisplaySettingsMenuGuage,   // DrawGuageType
            DrawDisplaySettingsMenuStatusRatioGroup,   // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable | mibVGAItem // Flags
    },
    // 2
    {
        Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_VPos), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_Red, CPC_Black, // SelForeColor, SelBackColor;
        VPositionMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        VPositionText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawDisplaySettingsMenuNumber,   // DrawNumberType
            DrawDisplaySettingsMenuGuage,   // DrawGuageType
            DrawDisplaySettingsMenuStatusRatioGroup,   // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable | mibVGAItem // Flags
    },
#endif
#if ENABLE_SHARPNESS
    // 3
    {
        Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_Sharpness), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        SharpnessMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        SharpnessText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawDisplaySettingsMenuNumber,   // DrawNumberType
            DrawDisplaySettingsMenuGuage,   // DrawGuageType
            DrawDisplaySettingsMenuStatusRatioGroup,   // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable //| mibVGAItem // Flags
    },
#endif
#if (ENABLE_VGA_INPUT)
    // 4
    {
        Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_Clock), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_Red, CPC_Black, // SelForeColor, SelBackColor;
        ClockMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        ClockText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawDisplaySettingsMenuNumber,   // DrawNumberType
            DrawDisplaySettingsMenuGuage,   // DrawGuageType
            DrawDisplaySettingsMenuStatusRatioGroup,   // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable | mibVGAItem // Flags
    },
    // 5
    {
        Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_Phase), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_Red, CPC_Black, // SelForeColor, SelBackColor;
        FocusMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        PhaseText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawDisplaySettingsMenuNumber,   // DrawNumberType
            DrawDisplaySettingsMenuGuage,   // DrawGuageType
            DrawDisplaySettingsMenuStatusRatioGroup,   // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable | mibVGAItem // Flags
    },
#endif
    // 6
#if Enable_Expansion
    {
        Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_Aspect), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        AspectRatioMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        AspectRatioText,// DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawDisplaySettingsMenuNumber,   // DrawNumberType
            DrawDisplaySettingsMenuGuage,   // DrawGuageType
            DrawDisplaySettingsMenuStatusRatioGroup,   // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable
    },
#endif
#if ENABLE_RTE
    // 7
    {
        Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_OD), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ODMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        OverdriveText,// DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawDisplaySettingsMenuNumber,   // DrawNumberType
            DrawDisplaySettingsMenuGuage,   // DrawGuageType
            DrawDisplaySettingsMenuStatusRatioGroup,   // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
#endif
#if (ENABLE_VGA_INPUT)
    // 8
    {
        Layer2XPos, (LayerYPos+2*DisplaySettingsMenuItems_AutoAdjust), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_Red, CPC_Black, // SelForeColor, SelBackColor;
        AutoMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        AutoAdjustText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,   // AdjustFunction
            NULL,   // ExecFunction
        },
        {
            NULL,   // DrawNumberType
            NULL,   // DrawGuageType
            NULL,   // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable|mibVGAItem // Flags
    },
#endif
};
//----------------------------------------------------------
RadioTextType code OtherSettingsMenuStatusRatioText[] =
    {
    // Flags,            XPos,  YPos,   DisplayText
    #if AudioFunc
    {  NULL, Layer3XPos, (LayerYPos+2*OtherSettingsMenuItems_AudioSource), AudioSourceValue},
    #endif  
    #if ENABLE_OSD_ROTATION
    {  NULL, Layer3XPos, (LayerYPos+2*OtherSettingsMenuItems_OSDRotate), OSDRotateStatusText},
    #endif
    #if ENABLE_FREESYNC
    {  NULL, Layer3XPos, (LayerYPos+2*OtherSettingsMenuItems_FreeSync), FreeSyncModeStatusText},
    #endif
    {  NULL, Layer3XPos, (LayerYPos+2*OtherSettingsMenuItems_DisplayLogo), DisplayLogoStatusText},
    {  dwiEnd, Layer3XPos, (LayerYPos+2*OtherSettingsMenuItems_Max), NULL},
};
DrawRadioGroupType code DrawOtherSettingsMenuStatusRatioGroup[] =
        {
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, OtherSettingsMenuStatusRatioText}
};

MenuItemType code OtherSettingsMenuItems[] =
{
    #if AudioFunc
    // 0
    {
        Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_Volume), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        VolumeMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        VolumeText,// DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawOtherSettingsMenuNumber, // DrawNumberType
            DrawOtherSettingsMenuGuage, // DrawGuageType
            DrawOtherSettingsMenuStatusRatioGroup, // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    {
        Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_AudioSource), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        AudioSourceSelMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        AudioSourceText,//AudioSourceText,// DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawOtherSettingsMenuNumber,   // DrawNumberType
            DrawOtherSettingsMenuGuage,   // DrawGuageType
            DrawOtherSettingsMenuStatusRatioGroup,   // DrawRadioGroupType
        },
        NULL, //Font
        NULL //mibSelectable|mibAudioSelDisable
    },    
    #endif
    // 1
    {
        Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_OSDTrans), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        OSDTransMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        OSDTransparencyText,// DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawOtherSettingsMenuNumber, // DrawNumberType
            DrawOtherSettingsMenuGuage, // DrawGuageType
            DrawOtherSettingsMenuStatusRatioGroup, // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 2
    {
        Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_OSDHPos), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        OSDHPositionMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        OSDHPositionText,// DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawOtherSettingsMenuNumber, // DrawNumberType
            DrawOtherSettingsMenuGuage, // DrawGuageType
            DrawOtherSettingsMenuStatusRatioGroup, // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 3
    {
        Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_OSDVPos), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        OSDVPositionMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        OSDVPositionText,// DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawOtherSettingsMenuNumber, // DrawNumberType
            DrawOtherSettingsMenuGuage, // DrawGuageType
            DrawOtherSettingsMenuStatusRatioGroup, // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 4
    {
        Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_OSDTimeout), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        OSDTimeoutMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        OSDTimeoutText,// DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawOtherSettingsMenuNumber, // DrawNumberType
            DrawOtherSettingsMenuGuage, // DrawGuageType
            DrawOtherSettingsMenuStatusRatioGroup, // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },

#if ENABLE_OSD_ROTATION
    {
        Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_OSDRotate), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        OSDRotateMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        OSDRotateText,// DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawOtherSettingsMenuNumber,   // DrawNumberType
            DrawOtherSettingsMenuGuage,   // DrawGuageType
            DrawOtherSettingsMenuStatusRatioGroup,   // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable
    },
#endif

#if ENABLE_FREESYNC
    {
        Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_FreeSync), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        FreeSyncMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FreeSyncModeText,// DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawOtherSettingsMenuNumber,   // DrawNumberType
            DrawOtherSettingsMenuGuage,   // DrawGuageType
            DrawOtherSettingsMenuStatusRatioGroup,   // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable
    },
#endif
    {
        Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_DisplayLogo), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        DisplayLogoMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        DisplayLogoText,// DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawOtherSettingsMenuNumber,   // DrawNumberType
            DrawOtherSettingsMenuGuage,   // DrawGuageType
            DrawOtherSettingsMenuStatusRatioGroup,   // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable
    },
    // 5
    {
        Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_Reset), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ResetWaitMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryResetText,// DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawOtherSettingsMenuNumber, // DrawNumberType
            DrawOtherSettingsMenuGuage, // DrawGuageType
            DrawOtherSettingsMenuStatusRatioGroup, // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};


RadioTextType code PowerManagerMenuStatusRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  0, Layer3XPos, (LayerYPos+2*PowerManagerMenuItems_DCOffDischarge), PowerManagerDCOffDischargeStatusText},
    {  dwiEnd, Layer3XPos, (LayerYPos+2*PowerManagerMenuItems_PowerSaving), PowerManagerPowerSavingStatusText},
};
DrawRadioGroupType code DrawPowerManagerMenuStatusRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, PowerManagerMenuStatusRatioText}
};


MenuItemType code PowerManagerMenuItems[] =
{
    {
        Layer2XPos, (LayerYPos+2*PowerManagerMenuItems_DCOffDischarge), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        DCOffDischargeMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        PowerManagerDCOffDischargeText,// DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL,   // DrawNumberType
            NULL,   // DrawGuageType
            DrawPowerManagerMenuStatusRatioGroup,   // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable
    },
    {
        Layer2XPos, (LayerYPos+2*PowerManagerMenuItems_PowerSaving), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        PowerSavingMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        PowerManagerPowerSavingText,// DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL,   // DrawNumberType
            NULL,   // DrawGuageType
            DrawPowerManagerMenuStatusRatioGroup,   // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable
    },
};

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
    {34, Color_4,     VolumeNumber}
};

GaugeType code VolumeGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*OtherSettingsMenuItems_Volume), GetVolumeValue},
};
DrawGuageType code DrawVolumeGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0x22, Color_4,  9,  VolumeGuage}
};

MenuItemType code VolumeMenuItems[] =
{
    // 0 Volume
    {
        Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_Volume), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        OtherSettingsMenu, // NextMenuPage;
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
        OtherSettingsMenu, // NextMenuPage;
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
        OtherSettingsMenu, // NextMenuPage;
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
#if ENABLE_OSD_ROTATION
MenuItemType code OSDRotateMenuItems[] =
{
    // OFF
    {
        Layer3XPos, (LayerYPos+2*OSDRotateMenuItems_Off), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        OtherSettingsMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        OSDRotateOFFText, // DisplayText;
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
        NULL, // Font
        mibSelectable // Flags
    },
    // 90
    {
        Layer3XPos, (LayerYPos+2*OSDRotateMenuItems_90), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        OtherSettingsMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        OSDRotate90Text, // DisplayText;
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
        NULL, // Font
        mibSelectable // Flags
    },
    #if defined(_OSD_ROTATION_180_)
    // 180
    {
        Layer3XPos, (LayerYPos+2*OSDRotateMenuItems_180), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        OtherSettingsMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        OSDRotate180Text, // DisplayText;
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
        NULL, // Font
        mibSelectable // Flags
    },
    #endif
    #if defined(_OSD_ROTATION_270_)
    // 270
    {
        Layer3XPos, (LayerYPos+2*OSDRotateMenuItems_270), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        OtherSettingsMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        OSDRotate270Text, // DisplayText;
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
        NULL, // Font
        mibSelectable // Flags
    },
    #endif

};
#endif

#if ENABLE_FREESYNC
MenuItemType code FreeSyncMenuItems[] =
{
    // OFF
    {
        Layer3XPos, (LayerYPos+2*FreeSyncMenuItems_Off), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        OtherSettingsMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        OtherSettingOFFText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetDRRFunction, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, // Font
        mibSelectable // Flags
    },
    // On
    {
        Layer3XPos, (LayerYPos+2*FreeSyncMenuItems_On), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        OtherSettingsMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        OtherSettingONText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetDRRFunction, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, // Font
        mibSelectable // Flags
    },
};
#endif
MenuItemType code DisplayLogoMenuItems[] =
{
    // OFF
    {
        Layer3XPos, (LayerYPos+2*DisplayLogoMenuItems_Off), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        OtherSettingsMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        OtherSettingOFFText, // DisplayText;
        NaviExecFuncGotoPrevKeyEvent,
        {
            NULL, // AdjustFunction
            MenuFunc_DisplayLogoEn_Set, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, // Font
        mibSelectable // Flags
    },
    // On
    {
        Layer3XPos, (LayerYPos+2*DisplayLogoMenuItems_On), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        OtherSettingsMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        OtherSettingONText, // DisplayText;
        NaviExecFuncGotoPrevKeyEvent,
        {
            NULL, // AdjustFunction
            MenuFunc_DisplayLogoEn_Set, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, // Font
        mibSelectable // Flags
    },
};



//---------------------------------------------------------------
//============ Contrast MenuItem==============
//--------------------ContrastNumber--------------------------
NumberType code ContrastNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {  dwiEnd, NumberXPos, (LayerYPos+2*BriteContMenuItems_Contrast), GetContrastValue},
};
DrawNumberType code DrawContrastNumber[] =
{
    // ForeColor, BackColor,  GetValue
    {CPC_MTK_Gold, CPC_Black,     ContrastNumber}
};
//--------------------ContrastIconGuge--------------------------
GaugeType code ContrastGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*BriteContMenuItems_Contrast), GetContrastValue},
};
DrawGuageType code DrawContrastGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {6,  Color_4,    9,  ContrastGuage}
};

MenuItemType code ContrastMenuItems[] =
{
    // 0 Contrast
    {
        Layer2XPos, (LayerYPos+2*BriteContMenuItems_Contrast), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        BriteContMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        ContrastText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustContrast,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
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

//============ Brightness MenuItem==============
//--------------------BrightnessNumber--------------------------
NumberType code BrightnessNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {  dwiEnd, NumberXPos, (LayerYPos+2*BriteContMenuItems_Brightness), GetBrightnessValue},
};
DrawNumberType code DrawBrightnessNumber[] =
{
    // ForeColor, BackColor,  GetValue
    {CPC_MTK_Gold, CPC_Black,     BrightnessNumber}
};

//--------------------BrightnessIconGuge--------------------------
GaugeType code BrightnessGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*BriteContMenuItems_Brightness), GetBrightnessValue},
};
DrawGuageType code DrawBrightnessGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {6,  Color_4,    9,  BrightnessGuage}
};

MenuItemType code BrightnessMenuItems[] =
{
    // 0 Brightness
    {
        Layer2XPos, (LayerYPos+2*BriteContMenuItems_Brightness), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        BriteContMenu,// NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        BrightnessText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustBrightness,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
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





MenuItemType code GammaMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*GammaMenuItems_Off), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        DisplaySettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        OffM105Text, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL,   //Adjust3DLUTMode, // AdjustFunction
            SetGammaOnOff, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 1
    {
        Layer3XPos, (LayerYPos+2*GammaMenuItems_On), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        DisplaySettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        OnM105Text, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL,   //Adjust3DLUTMode,  // AdjustFunction
            SetGammaOnOff, // ExecFunction
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

#if Enable_Expansion
MenuItemType code AspectRatioMenuItems[] =
{
    // Full
    {
        Layer3XPos, (LayerYPos+2*AspectRatioMenuItems_Full), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        DisplaySettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        Ratio_FullText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            OSDSetExpansionMode, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 4:3
    {
        Layer3XPos, (LayerYPos+2*AspectRatioMenuItems_4_3), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        DisplaySettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        Ratio_43Text, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            OSDSetExpansionMode, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable|mibExpansionDisable4_3 // Flags
    },
    // 16:9
    {
        Layer3XPos, (LayerYPos+2*AspectRatioMenuItems_16_9), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        DisplaySettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        Ratio_169Text, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            OSDSetExpansionMode, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable|mibExpansionDisable16_9 // Flags
    },
    // 1:1
    {
        Layer3XPos, (LayerYPos+2*AspectRatioMenuItems_1_1), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        DisplaySettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        Ratio_11Text, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            OSDSetExpansionMode, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable|mibExpansionDisable1_1 // Flags
    },
    // Keep input Ratio
    {
        Layer3XPos, (LayerYPos+2*AspectRatioMenuItems_KeepInputRatio), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        DisplaySettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        Ratio_FixedInputRatioText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            OSDSetExpansionMode, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable|mibExpansionDisableFixInputRatio // Flags
    },
    // OVERSCAN
    {
        Layer3XPos, (LayerYPos+2*AspectRatioMenuItems_OverScan), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        DisplaySettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        Ratio_OverScanText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            OSDSetExpansionMode, // ExecFunction
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

#if ENABLE_RTE
MenuItemType code ODMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*ODMenuItems_Off), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        DisplaySettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        OffM105Text, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetOverDriveOnOffmode, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 1
    {
        Layer3XPos, (LayerYPos+2*ODMenuItems_On), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        DisplaySettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        OnM105Text, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetOverDriveOnOffmode, // ExecFunction
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
//--------------------------------------------------------------------------
#if ENABLE_DLC
MenuItemType code DLCMenuItems[] =
{
    // Off
    {
        Layer3XPos, (LayerYPos+2*Items_Off), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        BriteContMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        OffM101Text, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetDLCmode, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, // Font
        mibSelectable // Flags
    },
    // On
    {
        Layer3XPos, (LayerYPos+2*Items_On), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        BriteContMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        OnM101Text, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetDLCmode, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, // Font
        mibSelectable // Flags
    },
};
#endif
#if ENABLE_DPS
//--------------------------------------------------------------------------
MenuItemType code DPSMenuItems[] =
{
    // Off
    {
        Layer3XPos, (LayerYPos+2*Items_Off), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        BriteContMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        OffM101Text, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetDPSmode, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, // Font
        mibSelectable // Flags
    },
    // On
    {
        Layer3XPos, (LayerYPos+2*Items_On), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        BriteContMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        OnM101Text, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetDPSmode, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, // Font
        mibSelectable // Flags
    },

};
#endif
MenuItemType code DCRMenuItems[] =
{
    // Off
    {
        Layer3XPos, (LayerYPos+2*Items_Off), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        BriteContMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        OffM101Text, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetDCRmode, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // On
    {
        Layer3XPos, (LayerYPos+2*Items_On), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        BriteContMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        OnM101Text, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetDCRmode, // ExecFunction
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

//--------------------Independent Hue R Number--------------------------
NumberType code IndependentHueRNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*(IndependentHueMenuItems_R+1)), GetIndependentHueRValue},
};
DrawNumberType code DrawIndependentHueRNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {34, Color_4,     IndependentHueRNumber}
};

GaugeType code IndependentHueRGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*(IndependentHueMenuItems_R+1)), GetIndependentHueRValue},
};
DrawGuageType code DrawIndependentHueRGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0x22, Color_4,  9,  IndependentHueRGuage}
};

RadioTextType code IndependentHueRRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  dwiEnd, Layer3XPos, (LayerYPos+2*IndependentHueMenuItems_R), RText},
};
DrawRadioGroupType code DrawIndependentHueRRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, IndependentHueRRatioText}
};

MenuItemType code IndependentHueRMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*IndependentHueMenuItems_R), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentHueMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustIndependentHue_R,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawIndependentHueRNumber,// DrawNumberType
            DrawIndependentHueRGuage,// DrawGuageType
            DrawIndependentHueRRatioGroup,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//--------------------Independent Hue G Number--------------------------
NumberType code IndependentHueGNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*(IndependentHueMenuItems_G+1)), GetIndependentHueGValue},
};
DrawNumberType code DrawIndependentHueGNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {34, Color_4,     IndependentHueGNumber}
};

GaugeType code IndependentHueGGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*(IndependentHueMenuItems_G+1)), GetIndependentHueGValue},
};
DrawGuageType code DrawIndependentHueGGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0x22, Color_4,  9,  IndependentHueGGuage}
};

RadioTextType code IndependentHueGRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  dwiEnd, Layer3XPos, (LayerYPos+2*IndependentHueMenuItems_G), GText},
};
DrawRadioGroupType code DrawIndependentHueGRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, IndependentHueGRatioText}
};

MenuItemType code IndependentHueGMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*IndependentHueMenuItems_G), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentHueMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustIndependentHue_G,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawIndependentHueGNumber,// DrawNumberType
            DrawIndependentHueGGuage,// DrawGuageType
            DrawIndependentHueGRatioGroup,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//--------------------Independent Hue B Number--------------------------
NumberType code IndependentHueBNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*(IndependentHueMenuItems_B+1)), GetIndependentHueBValue},
};
DrawNumberType code DrawIndependentHueBNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {34, Color_4,     IndependentHueBNumber}
};

GaugeType code IndependentHueBGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*(IndependentHueMenuItems_B+1)), GetIndependentHueBValue},
};
DrawGuageType code DrawIndependentHueBGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0x22, Color_4,  9,  IndependentHueBGuage}
};

RadioTextType code IndependentHueBRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  dwiEnd, Layer3XPos, (LayerYPos+2*IndependentHueMenuItems_B), BText},
};
DrawRadioGroupType code DrawIndependentHueBRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, IndependentHueBRatioText}
};

MenuItemType code IndependentHueBMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*IndependentHueMenuItems_B), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentHueMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustIndependentHue_B,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawIndependentHueBNumber,// DrawNumberType
            DrawIndependentHueBGuage,// DrawGuageType
            DrawIndependentHueBRatioGroup,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//--------------------Independent Hue C Number--------------------------
NumberType code IndependentHueCNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*(IndependentHueMenuItems_C+1)), GetIndependentHueCValue},
};
DrawNumberType code DrawIndependentHueCNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {34, Color_4,     IndependentHueCNumber}
};

GaugeType code IndependentHueCGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*(IndependentHueMenuItems_C+1)), GetIndependentHueCValue},
};
DrawGuageType code DrawIndependentHueCGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0x22, Color_4,  9,  IndependentHueCGuage}
};

RadioTextType code IndependentHueCRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  dwiEnd, Layer3XPos, (LayerYPos+2*IndependentHueMenuItems_C), CText},
};
DrawRadioGroupType code DrawIndependentHueCRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, IndependentHueCRatioText}
};

MenuItemType code IndependentHueCMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*IndependentHueMenuItems_C), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentHueMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustIndependentHue_C,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawIndependentHueCNumber,// DrawNumberType
            DrawIndependentHueCGuage,// DrawGuageType
            DrawIndependentHueCRatioGroup,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//--------------------Independent Hue M Number--------------------------
NumberType code IndependentHueMNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*(IndependentHueMenuItems_M+1)), GetIndependentHueMValue},
};
DrawNumberType code DrawIndependentHueMNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {34, Color_4,     IndependentHueMNumber}
};

GaugeType code IndependentHueMGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*(IndependentHueMenuItems_M+1)), GetIndependentHueMValue},
};
DrawGuageType code DrawIndependentHueMGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0x22, Color_4,  9,  IndependentHueMGuage}
};

RadioTextType code IndependentHueMRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  dwiEnd, Layer3XPos, (LayerYPos+2*IndependentHueMenuItems_M), MText},
};
DrawRadioGroupType code DrawIndependentHueMRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, IndependentHueMRatioText}
};

MenuItemType code IndependentHueMMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*IndependentHueMenuItems_M), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentHueMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustIndependentHue_M,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawIndependentHueMNumber,// DrawNumberType
            DrawIndependentHueMGuage,// DrawGuageType
            DrawIndependentHueMRatioGroup,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//--------------------Independent Hue Y Number--------------------------
NumberType code IndependentHueYNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*(IndependentHueMenuItems_Y+1)), GetIndependentHueYValue},
};
DrawNumberType code DrawIndependentHueYNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {34, Color_4,     IndependentHueYNumber}
};

GaugeType code IndependentHueYGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*(IndependentHueMenuItems_Y+1)), GetIndependentHueYValue},
};
DrawGuageType code DrawIndependentHueYGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0x22, Color_4,  9,  IndependentHueYGuage}
};

RadioTextType code IndependentHueYRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  dwiEnd, Layer3XPos, (LayerYPos+2*IndependentHueMenuItems_Y), YText},
};
DrawRadioGroupType code DrawIndependentHueYRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, IndependentHueYRatioText}
};

MenuItemType code IndependentHueYMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*IndependentHueMenuItems_Y), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentHueMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustIndependentHue_Y,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawIndependentHueYNumber,// DrawNumberType
            DrawIndependentHueYGuage,// DrawGuageType
            DrawIndependentHueYRatioGroup,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};
 
//--------------------Independent Saturation R Number--------------------------
NumberType code IndependentSaturationRNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*(IndependentSaturationMenuItems_R+1)), GetIndependentSaturationRValue},
};
DrawNumberType code DrawIndependentSaturationRNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {34, Color_4,     IndependentSaturationRNumber}
};

GaugeType code IndependentSaturationRGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*(IndependentSaturationMenuItems_R+1)), GetIndependentSaturationRValue},
};
DrawGuageType code DrawIndependentSaturationRGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0x22, Color_4,  9,  IndependentSaturationRGuage}
};

RadioTextType code IndependentSaturationRRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  dwiEnd, Layer3XPos, (LayerYPos+2*IndependentSaturationMenuItems_R), RText},
};
DrawRadioGroupType code DrawIndependentSaturationRRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, IndependentSaturationRRatioText}
};

MenuItemType code IndependentSaturationRMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*IndependentSaturationMenuItems_R), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentSaturationMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustIndependentSaturation_R,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawIndependentSaturationRNumber,// DrawNumberType
            DrawIndependentSaturationRGuage,// DrawGuageType
            DrawIndependentSaturationRRatioGroup,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//--------------------Independent Saturation G Number--------------------------
NumberType code IndependentSaturationGNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*(IndependentSaturationMenuItems_G+1)), GetIndependentSaturationGValue},
};
DrawNumberType code DrawIndependentSaturationGNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {34, Color_4,     IndependentSaturationGNumber}
};

GaugeType code IndependentSaturationGGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*(IndependentSaturationMenuItems_G+1)), GetIndependentSaturationGValue},
};
DrawGuageType code DrawIndependentSaturationGGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0x22, Color_4,  9,  IndependentSaturationGGuage}
};

RadioTextType code IndependentSaturationGRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  dwiEnd, Layer3XPos, (LayerYPos+2*IndependentSaturationMenuItems_G), GText},
};
DrawRadioGroupType code DrawIndependentSaturationGRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, IndependentSaturationGRatioText}
};

MenuItemType code IndependentSaturationGMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*IndependentSaturationMenuItems_G), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentSaturationMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustIndependentSaturation_G,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawIndependentSaturationGNumber,// DrawNumberType
            DrawIndependentSaturationGGuage,// DrawGuageType
            DrawIndependentSaturationGRatioGroup,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//--------------------Independent Saturation B Number--------------------------
NumberType code IndependentSaturationBNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*(IndependentSaturationMenuItems_B+1)), GetIndependentSaturationBValue},
};
DrawNumberType code DrawIndependentSaturationBNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {34, Color_4,     IndependentSaturationBNumber}
};

GaugeType code IndependentSaturationBGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*(IndependentSaturationMenuItems_B+1)), GetIndependentSaturationBValue},
};
DrawGuageType code DrawIndependentSaturationBGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0x22, Color_4,  9,  IndependentSaturationBGuage}
};

RadioTextType code IndependentSaturationBRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  dwiEnd, Layer3XPos, (LayerYPos+2*IndependentSaturationMenuItems_B), BText},
};
DrawRadioGroupType code DrawIndependentSaturationBRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, IndependentSaturationBRatioText}
};

MenuItemType code IndependentSaturationBMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*IndependentSaturationMenuItems_B), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentSaturationMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustIndependentSaturation_B,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawIndependentSaturationBNumber,// DrawNumberType
            DrawIndependentSaturationBGuage,// DrawGuageType
            DrawIndependentSaturationBRatioGroup,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//--------------------Independent Saturation C Number--------------------------
NumberType code IndependentSaturationCNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*(IndependentSaturationMenuItems_C+1)), GetIndependentSaturationCValue},
};
DrawNumberType code DrawIndependentSaturationCNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {34, Color_4,     IndependentSaturationCNumber}
};

GaugeType code IndependentSaturationCGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*(IndependentSaturationMenuItems_C+1)), GetIndependentSaturationCValue},
};
DrawGuageType code DrawIndependentSaturationCGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0x22, Color_4,  9,  IndependentSaturationCGuage}
};

RadioTextType code IndependentSaturationCRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  dwiEnd, Layer3XPos, (LayerYPos+2*IndependentSaturationMenuItems_C), CText},
};
DrawRadioGroupType code DrawIndependentSaturationCRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, IndependentSaturationCRatioText}
};

MenuItemType code IndependentSaturationCMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*IndependentSaturationMenuItems_C), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentSaturationMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustIndependentSaturation_C,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawIndependentSaturationCNumber,// DrawNumberType
            DrawIndependentSaturationCGuage,// DrawGuageType
            DrawIndependentSaturationCRatioGroup,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//--------------------Independent Saturation M Number--------------------------
NumberType code IndependentSaturationMNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*(IndependentSaturationMenuItems_M+1)), GetIndependentSaturationMValue},
};
DrawNumberType code DrawIndependentSaturationMNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {34, Color_4,     IndependentSaturationMNumber}
};

GaugeType code IndependentSaturationMGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*(IndependentSaturationMenuItems_M+1)), GetIndependentSaturationMValue},
};
DrawGuageType code DrawIndependentSaturationMGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0x22, Color_4,  9,  IndependentSaturationMGuage}
};

RadioTextType code IndependentSaturationMRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  dwiEnd, Layer3XPos, (LayerYPos+2*IndependentSaturationMenuItems_M), MText},
};
DrawRadioGroupType code DrawIndependentSaturationMRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, IndependentSaturationMRatioText}
};

MenuItemType code IndependentSaturationMMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*IndependentSaturationMenuItems_M), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentSaturationMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustIndependentSaturation_M,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawIndependentSaturationMNumber,// DrawNumberType
            DrawIndependentSaturationMGuage,// DrawGuageType
            DrawIndependentSaturationMRatioGroup,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//--------------------Independent Saturation Y Number--------------------------
NumberType code IndependentSaturationYNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*(IndependentSaturationMenuItems_Y+1)), GetIndependentSaturationYValue},
};
DrawNumberType code DrawIndependentSaturationYNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {34, Color_4,     IndependentSaturationYNumber}
};

GaugeType code IndependentSaturationYGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*(IndependentSaturationMenuItems_Y+1)), GetIndependentSaturationYValue},
};
DrawGuageType code DrawIndependentSaturationYGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0x22, Color_4,  9,  IndependentSaturationYGuage}
};

RadioTextType code IndependentSaturationYRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  dwiEnd, Layer3XPos, (LayerYPos+2*IndependentSaturationMenuItems_Y), YText},
};
DrawRadioGroupType code DrawIndependentSaturationYRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, IndependentSaturationYRatioText}
};

MenuItemType code IndependentSaturationYMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*IndependentSaturationMenuItems_Y), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentSaturationMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustIndependentSaturation_Y,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawIndependentSaturationYNumber,// DrawNumberType
            DrawIndependentSaturationYGuage,// DrawGuageType
            DrawIndependentSaturationYRatioGroup,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//--------------------Independent Brightness R Number--------------------------
NumberType code IndependentBrightnessRNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*(IndependentBrightnessMenuItems_R+1)), GetIndependentBrightnessRValue},
};
DrawNumberType code DrawIndependentBrightnessRNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {34, Color_4,     IndependentBrightnessRNumber}
};

GaugeType code IndependentBrightnessRGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*(IndependentBrightnessMenuItems_R+1)), GetIndependentBrightnessRValue},
};
DrawGuageType code DrawIndependentBrightnessRGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0x22, Color_4,  9,  IndependentBrightnessRGuage}
};

RadioTextType code IndependentBrightnessRRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  dwiEnd, Layer3XPos, (LayerYPos+2*IndependentBrightnessMenuItems_R), RText},
};
DrawRadioGroupType code DrawIndependentBrightnessRRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, IndependentBrightnessRRatioText}
};

MenuItemType code IndependentBrightnessRMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*IndependentBrightnessMenuItems_R), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentBrightnessMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustIndependentBrightness_R,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawIndependentBrightnessRNumber,// DrawNumberType
            DrawIndependentBrightnessRGuage,// DrawGuageType
            DrawIndependentBrightnessRRatioGroup,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//--------------------Independent Brightness G Number--------------------------
NumberType code IndependentBrightnessGNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*(IndependentBrightnessMenuItems_G+1)), GetIndependentBrightnessGValue},
};
DrawNumberType code DrawIndependentBrightnessGNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {34, Color_4,     IndependentBrightnessGNumber}
};

GaugeType code IndependentBrightnessGGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*(IndependentBrightnessMenuItems_G+1)), GetIndependentBrightnessGValue},
};
DrawGuageType code DrawIndependentBrightnessGGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0x22, Color_4,  9,  IndependentBrightnessGGuage}
};

RadioTextType code IndependentBrightnessGRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  dwiEnd, Layer3XPos, (LayerYPos+2*IndependentBrightnessMenuItems_G), GText},
};
DrawRadioGroupType code DrawIndependentBrightnessGRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, IndependentBrightnessGRatioText}
};

MenuItemType code IndependentBrightnessGMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*IndependentBrightnessMenuItems_G), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentBrightnessMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustIndependentBrightness_G,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawIndependentBrightnessGNumber,// DrawNumberType
            DrawIndependentBrightnessGGuage,// DrawGuageType
            DrawIndependentBrightnessGRatioGroup,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//--------------------Independent Brightness B Number--------------------------
NumberType code IndependentBrightnessBNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*(IndependentBrightnessMenuItems_B+1)), GetIndependentBrightnessBValue},
};
DrawNumberType code DrawIndependentBrightnessBNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {34, Color_4,     IndependentBrightnessBNumber}
};

GaugeType code IndependentBrightnessBGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*(IndependentBrightnessMenuItems_B+1)), GetIndependentBrightnessBValue},
};
DrawGuageType code DrawIndependentBrightnessBGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0x22, Color_4,  9,  IndependentBrightnessBGuage}
};

RadioTextType code IndependentBrightnessBRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  dwiEnd, Layer3XPos, (LayerYPos+2*IndependentBrightnessMenuItems_B), BText},
};
DrawRadioGroupType code DrawIndependentBrightnessBRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, IndependentBrightnessBRatioText}
};

MenuItemType code IndependentBrightnessBMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*IndependentBrightnessMenuItems_B), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentBrightnessMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustIndependentBrightness_B,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawIndependentBrightnessBNumber,// DrawNumberType
            DrawIndependentBrightnessBGuage,// DrawGuageType
            DrawIndependentBrightnessBRatioGroup,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//--------------------Independent Brightness C Number--------------------------
NumberType code IndependentBrightnessCNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*(IndependentBrightnessMenuItems_C+1)), GetIndependentBrightnessCValue},
};
DrawNumberType code DrawIndependentBrightnessCNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {34, Color_4,     IndependentBrightnessCNumber}
};

GaugeType code IndependentBrightnessCGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*(IndependentBrightnessMenuItems_C+1)), GetIndependentBrightnessCValue},
};
DrawGuageType code DrawIndependentBrightnessCGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0x22, Color_4,  9,  IndependentBrightnessCGuage}
};

RadioTextType code IndependentBrightnessCRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  dwiEnd, Layer3XPos, (LayerYPos+2*IndependentBrightnessMenuItems_C), CText},
};
DrawRadioGroupType code DrawIndependentBrightnessCRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, IndependentBrightnessCRatioText}
};

MenuItemType code IndependentBrightnessCMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*IndependentBrightnessMenuItems_C), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentBrightnessMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustIndependentBrightness_C,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawIndependentBrightnessCNumber,// DrawNumberType
            DrawIndependentBrightnessCGuage,// DrawGuageType
            DrawIndependentBrightnessCRatioGroup,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//--------------------Independent Brightness M Number--------------------------
NumberType code IndependentBrightnessMNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*(IndependentBrightnessMenuItems_M+1)), GetIndependentBrightnessMValue},
};
DrawNumberType code DrawIndependentBrightnessMNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {34, Color_4,     IndependentBrightnessMNumber}
};

GaugeType code IndependentBrightnessMGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*(IndependentBrightnessMenuItems_M+1)), GetIndependentBrightnessMValue},
};
DrawGuageType code DrawIndependentBrightnessMGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0x22, Color_4,  9,  IndependentBrightnessMGuage}
};

RadioTextType code IndependentBrightnessMRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  dwiEnd, Layer3XPos, (LayerYPos+2*IndependentBrightnessMenuItems_M), MText},
};
DrawRadioGroupType code DrawIndependentBrightnessMRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, IndependentBrightnessMRatioText}
};

MenuItemType code IndependentBrightnessMMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*IndependentBrightnessMenuItems_M), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentBrightnessMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustIndependentBrightness_M,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawIndependentBrightnessMNumber,// DrawNumberType
            DrawIndependentBrightnessMGuage,// DrawGuageType
            DrawIndependentBrightnessMRatioGroup,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//--------------------Independent Brightness Y Number--------------------------
NumberType code IndependentBrightnessYNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*(IndependentBrightnessMenuItems_Y+1)), GetIndependentBrightnessYValue},
};
DrawNumberType code DrawIndependentBrightnessYNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {34, Color_4,     IndependentBrightnessYNumber}
};

GaugeType code IndependentBrightnessYGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*(IndependentBrightnessMenuItems_Y+1)), GetIndependentBrightnessYValue},
};
DrawGuageType code DrawIndependentBrightnessYGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0x22, Color_4,  9,  IndependentBrightnessYGuage}
};

RadioTextType code IndependentBrightnessYRatioText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {  dwiEnd, Layer3XPos, (LayerYPos+2*IndependentBrightnessMenuItems_Y), YText},
};
DrawRadioGroupType code DrawIndependentBrightnessYRatioGroup[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_Black, NULL, IndependentBrightnessYRatioText}
};

MenuItemType code IndependentBrightnessYMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*IndependentBrightnessMenuItems_Y), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentBrightnessMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustIndependentBrightness_Y,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            DrawIndependentBrightnessYNumber,// DrawNumberType
            DrawIndependentBrightnessYGuage,// DrawGuageType
            DrawIndependentBrightnessYRatioGroup,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};
 

MenuItemType code ColorRangeMenuItems[] =
{
    // Full
    {
        Layer3XPos, (LayerYPos+2*ColorRangeMenuItems_Full), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ExtColorSettingsMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        ColorRangeFullText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetColorRange, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // Limit
    {
        Layer3XPos, (LayerYPos+2*ColorRangeMenuItems_Limit), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ExtColorSettingsMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        ColorRangeLimitText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetColorRange, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // Auto
    {
        Layer3XPos, (LayerYPos+2*ColorRangeMenuItems_Auto), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ExtColorSettingsMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        ColorRangeAutoText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetColorRange, // ExecFunction
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

MenuItemType code IndependentHueMenuItems[] =
{
    //R
    {
        Layer3XPos, (LayerYPos+2*IndependentHueMenuItems_R), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentHueRMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        RText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    //G
    {
        Layer3XPos, (LayerYPos+2*IndependentHueMenuItems_G), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentHueGMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        GText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    //B
    {
        Layer3XPos, (LayerYPos+2*IndependentHueMenuItems_B), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentHueBMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        BText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    //C
    {
        Layer3XPos, (LayerYPos+2*IndependentHueMenuItems_C), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentHueCMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        CText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    //M
    {
        Layer3XPos, (LayerYPos+2*IndependentHueMenuItems_M), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentHueMMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        MText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    //Y
    {
        Layer3XPos, (LayerYPos+2*IndependentHueMenuItems_Y), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentHueYMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        YText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

MenuItemType code IndependentSaturationMenuItems[] =
{
    //R
    {
        Layer3XPos, (LayerYPos+2*IndependentSaturationMenuItems_R), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentSaturationRMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        RText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    //G
    {
        Layer3XPos, (LayerYPos+2*IndependentSaturationMenuItems_G), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentSaturationGMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        GText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    //B
    {
        Layer3XPos, (LayerYPos+2*IndependentSaturationMenuItems_B), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentSaturationBMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        BText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    //C
    {
        Layer3XPos, (LayerYPos+2*IndependentSaturationMenuItems_C), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentSaturationCMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        CText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    //M
    {
        Layer3XPos, (LayerYPos+2*IndependentSaturationMenuItems_M), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentSaturationMMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        MText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    //Y
    {
        Layer3XPos, (LayerYPos+2*IndependentSaturationMenuItems_Y), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentSaturationYMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        YText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

MenuItemType code IndependentBrightnessMenuItems[] =
{
    //R
    {
        Layer3XPos, (LayerYPos+2*IndependentBrightnessMenuItems_R), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentBrightnessRMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        RText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    //G
    {
        Layer3XPos, (LayerYPos+2*IndependentBrightnessMenuItems_G), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentBrightnessGMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        GText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    //B
    {
        Layer3XPos, (LayerYPos+2*IndependentBrightnessMenuItems_B), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentBrightnessBMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        BText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    //C
    {
        Layer3XPos, (LayerYPos+2*IndependentBrightnessMenuItems_C), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentBrightnessCMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        CText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    //M
    {
        Layer3XPos, (LayerYPos+2*IndependentBrightnessMenuItems_M), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentBrightnessMMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        MText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    //Y
    {
        Layer3XPos, (LayerYPos+2*IndependentBrightnessMenuItems_Y), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        IndependentBrightnessYMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        YText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            NULL,// DrawRadioGroupType
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
        62, Color_2, // ForeColor, BackColor;
        34, Color_4,  // SelForeColor, SelBackColor;
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
        NULL, //Font
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
        DisplaySettingsMenu, // NextMenuPage;
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
        DisplaySettingsMenu, // NextMenuPage;
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
        DisplaySettingsMenu, // NextMenuPage;
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
        DisplaySettingsMenu, // NextMenuPage;
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
        DisplaySettingsMenu, // NextMenuPage;
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
        OtherSettingsMenu, // NextMenuPage;
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

//--------------------------------------------------------------------------
NumberType code OsdHPositionNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*OtherSettingsMenuItems_OSDHPos), GetOSDHPositionValue},
};
DrawNumberType code DrawOsdHPositionNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {14, Color_2,     OsdHPositionNumber}
};

GaugeType code OsdHPositionGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*OtherSettingsMenuItems_OSDHPos), GetOSDHPositionValue},
};
DrawGuageType code DrawOsdHPositionGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {6, Color_4, 9,  OsdHPositionGuage}
};

MenuItemType code OSDHPositionMenuItems[] =
{
    // 0 OsdHPosition
    {
        Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_OSDHPos), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        OtherSettingsMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        OSDHPositionText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustOSDHPosition,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawOsdHPositionNumber,// DrawNumberType
            DrawOsdHPositionGuage,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//--------------------------------------------------------------------------
NumberType code OsdVPositionNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*OtherSettingsMenuItems_OSDVPos), GetOSDVPositionValue},
};
DrawNumberType code DrawOsdVPositionNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {14, Color_2,     OsdVPositionNumber}
};

GaugeType code OsdVPositionGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*OtherSettingsMenuItems_OSDVPos), GetOSDVPositionValue},
};
DrawGuageType code DrawOsdVPositionGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {6, Color_4, 9,  OsdVPositionGuage}
};

MenuItemType code OSDVPositionMenuItems[] =
{
    // 0 OsdVPosition
    {
        Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_OSDVPos), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        OtherSettingsMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        OSDVPositionText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustOSDVPosition,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawOsdVPositionNumber,// DrawNumberType
            DrawOsdVPositionGuage,// DrawGuageType
            NULL,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};


//--------------------------------------------------------------------------
NumberType code OsdTimeOutNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, NumberXPos, (LayerYPos+2*OtherSettingsMenuItems_OSDTimeout), GetOSDTime100Value},
};
DrawNumberType code DrawOsdTimeOutNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {14, Color_2,     OsdTimeOutNumber}
};

GaugeType code OsdTimeOutGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, GaugeXPos, (LayerYPos+2*OtherSettingsMenuItems_OSDTimeout), GetOSDTime100Value},
};
DrawGuageType code DrawOsdTimeOutGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {6, Color_4, 9,  OsdTimeOutGuage}
};

MenuItemType code OSDTimeoutMenuItems[] =
{
    // 0 OsdTimeOutPosition
    {
        Layer2XPos, (LayerYPos+2*OtherSettingsMenuItems_OSDTimeout), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        OtherSettingsMenu, // NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        OSDTimeoutText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustOSDTime,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawOsdTimeOutNumber,// DrawNumberType
            DrawOsdTimeOutGuage,// DrawGuageType
            NULL,// DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

#if ENABLE_SUPER_RESOLUTION
MenuItemType code SuperResolutionMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*SuperResolutionMenuItems_Off), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorSettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        OffM102Text, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetSuperResolutionMode, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 1
    {
        Layer3XPos, (LayerYPos+2*SuperResolutionMenuItems_Weak), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorSettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        WeakText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetSuperResolutionMode, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 2
    {
        Layer3XPos, (LayerYPos+2*SuperResolutionMenuItems_Middle), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorSettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        MiddleText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetSuperResolutionMode, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 3
    {
        Layer3XPos, (LayerYPos+2*SuperResolutionMenuItems_Strong), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorSettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        StrongText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetSuperResolutionMode, // ExecFunction
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

#if ENABLE_DeBlocking
//============ DeBlockingMenu ==============
RadioTextType code DrawDeBlockingMenuText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {dwiCenterArrowAlign | dwiEnd,  (CENTER_ALIGN_ENDPOS-(CENTER_ALIGN_LEN/2)+1), (SUB_TEXT_YPOS+IMAGESETUP_DEBLOCKING_ITEM*2),     DeBlockingValueText },
};
DrawRadioGroupType code DrawDeBlockingMenuIconRatioText[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {10,    Color_2, NULL, DrawDeBlockingMenuText}
};
MenuItemType code DeBlockingMenuItems[] =
{
    // 0 DeBlockingMenu
    {
        SUB_TEXT_XPOS, (SUB_TEXT_YPOS+IMAGESETUP_DEBLOCKING_ITEM*2), // XPos, YPos;
        14, Color_2,            // ForeColor, BackColor;
        12, Color_2,            // SelForeColor, SelBackColor;
        RootMenu,       //NextMenuPage;
        DWI_Text,           // DrawMenuItemType;
        DeBlockingText,             // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustDeBlockingMode,   // AdjustFunction
            NULL,               // ExecFunction
        },
        {
            NULL,               // DrawNumberType
            NULL,               // DrawGuageType
            DrawDeBlockingMenuIconRatioText        //DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
};
#endif

MenuItemType code ColorModeMenuItems[] =
{
    // 0
    {
        Layer3XPos, (LayerYPos+2*ColorModeMenuItems_Off), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorSettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        OffM102Text, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetColorMode, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 1
    {
        Layer3XPos, (LayerYPos+2*ColorModeMenuItems_Enhance), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorSettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        EnhanceText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetColorMode, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 2
    {
        Layer3XPos, (LayerYPos+2*ColorModeMenuItems_Demo), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorSettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        DemoText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            SetColorMode, // ExecFunction
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

MenuItemType code ColorTempMenuItems[] =
{
    // 0 CTEMP_Cool1
    {
        Layer3XPos, (LayerYPos+2*CTEMP_Cool1), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorSettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        C9300KText, // DisplayText;
        NaviExecFuncGotoPrevKeyEvent,
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
    // 1 CTEMP_Normal
    {
        Layer3XPos, (LayerYPos+2*CTEMP_Normal), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorSettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        C6500KText, // DisplayText;
        NaviExecFuncGotoPrevKeyEvent,
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
    // 2 CTEMP_Warm1
    {
        Layer3XPos, (LayerYPos+2*CTEMP_Warm1), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorSettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        C5700KText, // DisplayText;
        NaviExecFuncGotoPrevKeyEvent,
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
    // 3 CTEMP_SRGB
    {
        Layer3XPos, (LayerYPos+2*CTEMP_SRGB), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorSettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        CsRGBText, // DisplayText;
        NaviExecFuncGotoPrevKeyEvent,
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
    // 4 CTEMP_USER
    {
        Layer3XPos, (LayerYPos+2*CTEMP_USER), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        ColorSettingsMenu, // NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        UserColorText, // DisplayText;
        NaviExecFuncGotoPrevKeyEvent,
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
};

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
        NaviExecKeyEvent,
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
        NaviExecKeyEvent,
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
        NaviExecKeyEvent,
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
        SourceSelectMenu,///NextMenuPage;
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
        DPVersionMenu,///NextMenuPage;
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
};

MenuItemType code SourceSelectMenuItems[] =
{
    //AUTO
    {
        Layer3XPos, (LayerYPos+2*SourceSelectMenuItems_Auto), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        SourceSelectMenu,///NextMenuPage;
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
        SourceSelectMenu,///NextMenuPage;
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
        SourceSelectMenu,///NextMenuPage;
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
        SourceSelectMenu,///NextMenuPage;
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
        SourceSelectMenu,///NextMenuPage;
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
        SourceSelectMenu,///NextMenuPage;
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


//------------- ResetMenuItem  --------------------
RadioTextType code DrawResetYesIcon[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {dwiCenterArrowAlign|dwiEnd, (CENTER_ALIGN_ENDPOS-(CENTER_ALIGN_LEN/2)+1), (SUB_TEXT_YPOS+EXTRA_FACTORYRECALL_ITEM*2),    YesText},
};
DrawRadioGroupType code DrawResetYesIconRationText[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {14, Color_2, NULL, DrawResetYesIcon}
};

RadioTextType code DrawResetNoIcon[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {dwiCenterArrowAlign|dwiEnd, (CENTER_ALIGN_ENDPOS-(CENTER_ALIGN_LEN/2)+1), (SUB_TEXT_YPOS+EXTRA_FACTORYRECALL_ITEM*2),    NoText},
};
DrawRadioGroupType code DrawResetNoIconRationText[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {14, Color_2, NULL, DrawResetNoIcon}
};
MenuItemType code ResetMenuItems[] =
{
    // 1 No
    {
        SUB_TEXT_XPOS, (SUB_TEXT_YPOS+EXTRA_FACTORYRECALL_ITEM*2), // XPos, YPos;
        14, Color_2, // ForeColor, BackColor;
        12, Color_2, // SelForeColor, SelBackColor;
        RootMenu,///NextMenuPage;
        DWI_Text,//// DrawMenuItemType;
        ResetText, // DisplayText;
        NaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL,//Reset// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            DrawResetNoIconRationText,//DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 0 Yes
    {
        SUB_TEXT_XPOS, (SUB_TEXT_YPOS+EXTRA_FACTORYRECALL_ITEM*2), // XPos, YPos;
        14, Color_2, // ForeColor, BackColor;
        12, Color_2, // SelForeColor, SelBackColor;
        ResetWaitMenu,///NextMenuPage;
        DWI_Text,//// DrawMenuItemType;
        ResetText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,// AReset// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            DrawResetYesIconRationText,//DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },

};
#if HotExpansion
//------------- HotKeyECOMenuItem  --------------------
RadioTextType code DrawHotKeyExpansionText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {dwiEnd, 1, 1,   AspectRatioText},

};
DrawRadioGroupType code DrawHotKeyExpansionIconRationText[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {CPC_White, CPC_MTK_Gold, NULL, DrawHotKeyExpansionText} // 100831
};
MenuItemType code HotKeyExpansionMenuItems[] =
{
    {
        HotLayerXPos, (HotLayerYPos+2*0), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        Ratio_FullText, // DisplayText;
        NaviKeyEvent,// KeyEvent
        {
            NULL, // AdjustFunction
            NULL    // ExecFunction
        },// *DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawHotKeyExpansionIconRationText// DrawRadioGroupType
        },
        MenuPage105, // Font
        mibSelectable // Flags
    },
    {
        HotLayerXPos, (HotLayerYPos+2*1), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        Ratio_43Text, // DisplayText;
        NaviKeyEvent,// KeyEvent
        {
            NULL, // AdjustFunction
            NULL    // ExecFunction
        },// *DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawHotKeyExpansionIconRationText// DrawRadioGroupType
        },
        MenuPage105, // Font
        mibSelectable // Flags
    }
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
    {CPC_White, CPC_MTK_Gold, NULL, DrawHotInputSelectText} // 100831
};
MenuItemType code HotInputSelectMenuItems[] =
{
    //AUTO
    {
        HotLayerXPos, (HotLayerYPos+SourceSelectMenuItems_Auto), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
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
        CPC_Red, CPC_Black, // SelForeColor, SelBackColor;
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
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
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
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
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
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
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
        0, 5, // XPos, YPos;
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


#if MWEFunction
//-------------------------------------------------------

//============ ColorBoostMenuItem==============

//--------------------FULLText--------------------------
RadioTextType code DrawFullText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    //  {dwiRadioGroup, 4, 3,   ColorTempText},
    {dwiOptionBar | dwiEnd,   21, 3,      FreeColorFullModeText },
};
DrawRadioGroupType code DrawFullIconRatioText[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {0, 12, NULL, DrawFullText}
};

//--------------------NatureSkinText--------------------------
RadioTextType code DrawSkinProtectText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    //  {dwiRadioGroup, 4, 3,   ColorTempText},
    {dwiOptionBar | dwiEnd,   21, 5,      FreeColorSkinProtectModeText },
};
DrawRadioGroupType code DrawSkinProtectIconRatioText[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {0, 12, NULL, DrawSkinProtectText}
};

//--------------------GreenFieldText--------------------------
RadioTextType code DrawGreenEnhanceText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    //  {dwiRadioGroup, 4, 3,   ColorTempText},
    {dwiOptionBar | dwiEnd,   21, 7,      FreeColorGreenEnhanceModeText },
};
DrawRadioGroupType code DrawGreenEnhanceIconRatioText[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {0, 12, NULL, DrawGreenEnhanceText}
};

//--------------------SkyBlueText--------------------------
RadioTextType code DrawBlueEnhanceText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    //  {dwiRadioGroup, 4, 3,   ColorTempText},
    {dwiOptionBar | dwiEnd,   21, 9,      FreeColorBlueEnhanceModeText },
};
DrawRadioGroupType code DrawBlueEnhanceIconRatioText[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {0, 12, NULL, DrawBlueEnhanceText}
};


//--------------------AutoDetectText--------------------------
RadioTextType code DrawAutoDetectText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    //  {dwiRadioGroup, 4, 3,   ColorTempText},
    {dwiOptionBar | dwiEnd,   21, 11,     FreeColorAutoDetectModeText },
};
DrawRadioGroupType code DrawAutoDetectIconRatioText[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {0, 12, NULL, DrawAutoDetectText}
};


//--------------------DemoText--------------------------
RadioTextType code DrawDemoText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    //  {dwiRadioGroup, 4, 3,   ColorTempText},
    {dwiOptionBar | dwiEnd,   21, 13,     FreeColorDemoModeText },
};
DrawRadioGroupType code DrawDemoIconRatioText[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {0, 12, NULL, DrawDemoText}
};

MenuItemType code ColorBoostMenuItems[] =
{
    // 0 Full
    {
        4, 3, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 12, // SelForeColor, SelBackColor;
        FullMenu,//MainMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FullText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            DrawFullIconRatioText,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 1 SkinProtect
    {
        4, 5, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 12, // SelForeColor, SelBackColor;
        SkinProtectMenu,//MainMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NatureSkinText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            DrawSkinProtectIconRatioText,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable// Flags
    },
    // 2 GreenEnhance
    {
        4, 7, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 12, // SelForeColor, SelBackColor;
        GreenEnhanceMenu,//MainMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        GreenFieldText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            DrawGreenEnhanceIconRatioText,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable// Flags
    },

    // 3 BlueEnhance
    {
        4, 9, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 12, // SelForeColor, SelBackColor;
        BlueEnhanceMenu,//MainMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        SkyBlueText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            DrawBlueEnhanceIconRatioText,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 4 AutoDetect
    {
        4, 11, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 12, // SelForeColor, SelBackColor;
        AutoDetectMenu,//MainMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        AutoDetectText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            DrawAutoDetectIconRatioText,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 5 Demo
    {
        4, 13, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 12, // SelForeColor, SelBackColor;
        DemoMenu,//MainMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        DemoText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            DrawDemoIconRatioText,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable// Flags
    },

};


//===========  FreeColorFullMode MenuItem==============
//------------- FreeColorFullModeText  --------------------
RadioTextType code DrawFull[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    //  {dwiRadioGroup, 4, 3,   ColorTempText},
    {dwiOptionBar | dwiEnd,   21, 3,      FreeColorFullModeText },
};
DrawRadioGroupType code DrawFullModeIconRatioText[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {0, 12, NULL, DrawFull}
};


MenuItemType code FreeColorFullMODEMenuItems[] =
{
    // 0 FullMemu
    {
        4, 3, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 14, // SelForeColor, SelBackColor;
        ColorBoostMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FullText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustColorBoostMode,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            DrawFullModeIconRatioText,//DrawMenuRadioGroup
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//------------- SkinProtectModeText  --------------------
RadioTextType code DrawSkinProtect[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    //  {dwiRadioGroup, 4, 3,   ColorTempText},
    {dwiOptionBar | dwiEnd,   21, 5,      FreeColorSkinProtectModeText},
};
DrawRadioGroupType code DrawSkinProtectModeIconRatioText[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {0, 12, NULL, DrawSkinProtect}
};


MenuItemType code FreeColorSkinProtectMODEMenuItems[] =
{
    // 0 SkinProtectMemu
    {
        4, 5, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 14, // SelForeColor, SelBackColor;
        ColorBoostMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NatureSkinText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustSkinProtectMode,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            DrawSkinProtectModeIconRatioText,//DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
};


//------------- GreenEnhanceModeText  --------------------
RadioTextType code DrawGreenEnhance[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    //  {dwiRadioGroup, 4, 3,   ColorTempText},
    {dwiOptionBar | dwiEnd,   21, 7,      FreeColorGreenEnhanceModeText},
};
DrawRadioGroupType code DrawGreenEnhanceModeIconRatioText[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {0, 12, NULL, DrawGreenEnhance}
};


MenuItemType code FreeColorGreenEnhanceMenuItems[] =
{
    // 0 GreenEnhanceMemu
    {
        4, 7, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 14, // SelForeColor, SelBackColor;
        ColorBoostMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        GreenFieldText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustGreenEnhanceMode,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            DrawGreenEnhanceModeIconRatioText,//DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
};


//------------- BlueEnhanceModeText  --------------------
RadioTextType code DrawBlueEnhance[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    //  {dwiRadioGroup, 4, 3,   ColorTempText},
    {dwiOptionBar | dwiEnd,   21, 9,      FreeColorBlueEnhanceModeText},
};
DrawRadioGroupType code DrawBlueEnhanceModeIconRatioText[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {0, 12, NULL, DrawBlueEnhance}
};


MenuItemType code FreeColorBlueEnhanceMenuItems[] =
{
    // 0 BlueEnhanceMemu
    {
        4, 9, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 14, // SelForeColor, SelBackColor;
        ColorBoostMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        SkyBlueText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustBlueEnhanceMode,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            DrawBlueEnhanceModeIconRatioText,//DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
};


//------------- AutoDetectModeText  --------------------
RadioTextType code DrawAutoDetect[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    //  {dwiRadioGroup, 4, 3,   ColorTempText},
    {dwiOptionBar | dwiEnd,   21, 11,     FreeColorAutoDetectModeText},
};
DrawRadioGroupType code DrawAutoDetectModeIconRatioText[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {0, 12, NULL, DrawAutoDetect}
};


MenuItemType code FreeColorAutoDetectMenuItems[] =
{
    // 0 AutoDetectMemu
    {
        4, 11, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 14, // SelForeColor, SelBackColor;
        ColorBoostMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        AutoDetectText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustAutoDetectMode,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            DrawAutoDetectModeIconRatioText,//DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//------------- DemoModeText  --------------------
RadioTextType code DrawDemo[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    //  {dwiRadioGroup, 4, 3,   ColorTempText},
    {dwiOptionBar | dwiEnd,   21, 13,      FreeColorDemoModeText},
};
DrawRadioGroupType code DrawDemoModeIconRatioText[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {0, 12, NULL, DrawDemo}
};


MenuItemType code ColorBoostDemoMenuItems[] =
{
    // 0 DemoMemu
    {
        4, 13, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 14, // SelForeColor, SelBackColor;
        ColorBoostMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        DemoText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustDemoMode,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            DrawDemoModeIconRatioText,//DrawMenuRadioGroup,
        },
        NULL, //Font
        mibSelectable // Flags
    },
};


//--------------BrightMenuItems --------------------------
//--------------BF_Size--------------------------
NumberType code BF_SizeNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd,        26,  3, GetBFSizeValue},
};
DrawNumberType code DrawBF_SizeNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {0, 12,    BF_SizeNumber}
};
//--------------------BF_SizeIconGuge--------------------------
GaugeType code BF_BFSizeGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd,             18,    3,  GetBFSizeValue},
};
DrawGuageType code DrawBF_SizeGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0,  12, 9,  BF_BFSizeGuage}
};

//--------------BF_BrightNumber--------------------------
NumberType code BF_BrightNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd,        26,  5, GetSubBrightness100Value},
};
DrawNumberType code DrawBF_BrightNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {0, 12,    BF_BrightNumber}
};
//--------------------BF_BrightIconGuge--------------------------
GaugeType code BF_BrightGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd,             18,    5,  GetSubBrightness100Value},
};
DrawGuageType code DrawBF_BrightGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0,  12, 9,  BF_BrightGuage}
};

//--------------BF_ContrastNumber--------------------------
NumberType code BF_ContrastNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd,        26,  7, GetSubContrast100Value},
};
DrawNumberType code DrawBF_ContrastNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {0, 12,    BF_ContrastNumber}
};
//--------------------BF_ContrastIconGuge--------------------------
GaugeType code BF_ContrastGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd,             18,    7,  GetSubContrast100Value},
};
DrawGuageType code DrawBF_ContrastGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0,  12, 9,  BF_ContrastGuage}
};
//--------------------BFHPositionNumber--------------------------
NumberType code BFHPositionNumber[] =
{
    // Flags,   XPos,   YPos,   GetHalue
    {dwiEnd,        26,  9, GetBFHstartValue},
};
DrawNumberType code DrawBFHPositionNumber[] =
{
    // F ForeColor, BackColor,  GetHalue
    {0, 12,    BFHPositionNumber}
};
//--------------------BFHPositionIconGuge--------------------------
GaugeType code BFHPositionGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetHalue
    {dwiEnd,             18,    9,  GetBFHstartValue},
};
DrawGuageType code DrawBFHPositionGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetHalue
    {0,  12, 9,  BFHPositionGuage}
};

//--------------------BFVPositionNumber--------------------------
NumberType code BFVPositionNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd,        26,  11,    GetBFVstartValue},
};
DrawNumberType code DrawBFVPositionNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    {0, 12,    BFVPositionNumber}
};
//--------------------BFVPositionIconGuge--------------------------
GaugeType code BFVPositionGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd,             18,    11, GetBFVstartValue},
};
DrawGuageType code DrawBFVPositionGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {0,  12, 9,  BFVPositionGuage}
};
//--------------------BF_SwitchMenuText--------------------------
RadioTextType code DrawSwitchText[] =
{
    // Flags,            XPos,  YPos,   DisplayText
    {dwiOptionBar | dwiEnd,  19, 13,   BF_SwitchValueText},
};
DrawRadioGroupType code DrawSwitchIconRatioText[] =
{
    //ForeColor,    BackColor,  GetValue    RadioText
    {0, 12, NULL, DrawSwitchText}
};

MenuItemType code BrightFrameMenuItems[] =
{
    // 0 BF_FrameSize
    {
        4, 3, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 12, // SelForeColor, SelBackColor;
        BFFrameSizeMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        BF_FrameSizeText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawBF_SizeNumber,// DrawNumberType
            DrawBF_SizeGuage,// DrawGuageType
            NULL,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable | mibBFEnable | mibDemoDisable // Flags
    },

    // 1 BF_Brightness
    {
        4, 5, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 12, // SelForeColor, SelBackColor;
        BF_BrightnessMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        BF_BrightnessText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawBF_BrightNumber,// DrawNumberType
            DrawBF_BrightGuage,// DrawGuageType
            NULL,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable | mibBFEnable // Flags
    },
    // 2 BF_Contrastness
    {
        4, 7, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 12, // SelForeColor, SelBackColor;
        BF_ContrastMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        BF_ContrastText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawBF_ContrastNumber,// DrawNumberType
            DrawBF_ContrastGuage,// DrawGuageType
            NULL,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable | mibBFEnable // Flags
    },
    // 2 BFHPositionMenu
    {
        4, 9, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 12, // SelForeColor, SelBackColor;
        BFHPositionMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        BFHPositionText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawBFHPositionNumber,// DrawNumberType
            DrawBFHPositionGuage,// DrawGuageType
            NULL,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable | mibBFEnable // Flags
    },
    // 3 BFVPositionMenu
    {
        4, 11, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 12, // SelForeColor, SelBackColor;
        BFVPositionMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        BFVPositionText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawBFVPositionNumber,// DrawNumberType
            DrawBFVPositionGuage,// DrawGuageType
            NULL,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable | mibBFEnable // Flags
    },
    // 3 BF_Switch
    {
        4, 13, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 12, // SelForeColor, SelBackColor;
        BFSwitchMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        BF_SwitchText, // DisplayText;
        NaviKeyEvent,
        {
            NULL,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            DrawSwitchIconRatioText ,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};
//============ BF_BrightnessMnuItem==============
MenuItemType code BF_BrightnessMenuItems[] =
{
    // 0 BF_BrightnessMENU
    {
        4, 5, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 14, // SelForeColor, SelBackColor;
        BrightFrameMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        BF_BrightnessText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustSubBrightness,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawBF_BrightNumber,// DrawNumberType
            DrawBF_BrightGuage,// DrawGuageType
            NULL,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//============ BF_ContrastMnuItem==============
MenuItemType code BF_ContrastMenuItems[] =
{
    // 0 BF_ContrastMENU
    {
        4, 7, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 14, // SelForeColor, SelBackColor;
        BrightFrameMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        BF_ContrastText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustSubContrast,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawBF_ContrastNumber,// DrawNumberType
            DrawBF_ContrastGuage,// DrawGuageType
            NULL,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};
//============ BFHPositionMenuMenuItems ==============
MenuItemType code BFHPositionMenuMenuItems[] =
{
    // 0 BF_HPositionMENU
    {
        4, 9, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 14, // SelForeColor, SelBackColor;
        BrightFrameMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        BFHPositionText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustBFHstart,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawBFHPositionNumber,// DrawNumberType
            DrawBFHPositionGuage,// DrawGuageType
            NULL,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//============ BFVPositionMenuMenuItems ==============
MenuItemType code BFVPositionMenuMenuItems[] =
{
    // 0 BF_VPositionMENU
    {
        4, 11, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 14, // SelForeColor, SelBackColor;
        BrightFrameMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        BFVPositionText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustBFVstart,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawBFVPositionNumber,// DrawNumberType
            DrawBFVPositionGuage,// DrawGuageType
            NULL,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};

//============ BFSwitchMenuItems ==============
MenuItemType code BFSwitchMenuItems[] =
{
    // 0 BF_SaturationMENU
    {
        4, 13, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 14, // SelForeColor, SelBackColor;
        BrightFrameMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        BF_SwitchText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustBF_Switch,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            NULL,// DrawNumberType
            NULL,// DrawGuageType
            DrawSwitchIconRatioText,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};
//--------------------BFHSizeNumber--------------------------
NumberType code BFSizeNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd,        26,  3, GetBFSizeValue},
};
DrawNumberType code DrawBFSizeNumber[] =
{
    // F ForeColor, BackColor,  GetValue
    0, 12,    BFSizeNumber
};
//--------------------BFHSizeIconGuge--------------------------
GaugeType code BFSizeGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd,             18,    3,  GetBFSizeValue},
};
DrawGuageType code DrawBFSizeGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    0,  12, 9,  BFSizeGuage
};

//============ BFHSizeMenuMenuItems ==============
MenuItemType code BFFrameSizeMenuItems[] =
{
    // 0 BF_HSizeMENU
    {
        4, 3, // XPos, YPos;
        0, 6, // ForeColor, BackColor;
        0, 14, // SelForeColor, SelBackColor;
        BrightFrameMenu,///NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        BF_FrameSizeText, // DisplayText;
        AdjusterKeyEvent,
        {
            AdjustBFSize,// AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },
        {
            DrawBFSizeNumber,// DrawNumberType
            DrawBFSizeGuage,// DrawGuageType
            NULL,//DrawMenuRadioGroup,//DrawAutoSetupRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
};
#endif




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

#if ENABLE_DP_INPUT

MenuItemType code DPVersionMenuItems[] =
{
    // DP1.1
    {
        Layer3XPos, (LayerYPos+2*DPVersionMenuItems_DP1_1), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        InputSourceMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        DP1_1Text, // DisplayText;
        NaviExecFuncGotoPrevKeyEvent,
        {
            NULL, // AdjustFunction
            ChangeDPVersion, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, // Font
        mibSelectable // Flags
    },
    // DP1.2
    {
        Layer3XPos, (LayerYPos+2*DPVersionMenuItems_DP1_2), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        InputSourceMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        DP1_2Text, // DisplayText;
        NaviExecFuncGotoPrevKeyEvent,
        {
            NULL, // AdjustFunction
            ChangeDPVersion, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, // Font
        mibSelectable // Flags
    },
    // DP1.4
    {
        Layer3XPos, (LayerYPos+2*DPVersionMenuItems_DP1_4), // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_MTK_Gold, CPC_Black, // SelForeColor, SelBackColor;
        InputSourceMenu,// NextMenuPage;
        DWI_Text, // DrawMenuItemType;
        DP1_4Text, // DisplayText;
        NaviExecFuncGotoPrevKeyEvent,
        {
            NULL, // AdjustFunction
            ChangeDPVersion, // ExecFunction
        },
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawMenuRadioGroup,
        },
        NULL, // Font
        mibSelectable // Flags
    },
};
#endif
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
        MessageMenuHSize, MessageMenuVSize, //  XSize, YSize;
        RootMenu,// PrevMenuPage;
        StandbyMenuItems, // MenuItems;
        sizeof( StandbyMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        EnablePowerDownCounter, // ExecFunction;
        MenuPage2, // Fonts
        mpbStay | mpbRedraw | mpbCenter // Flags;
    },
    // CableNotConnectMenu
    {
        MessageMenuHSize, MessageMenuVSize, //  XSize, YSize;
        RootMenu,// PrevMenuPage;
        CableNotConnectedMenuItems, // MenuItems;
        sizeof( CableNotConnectedMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL,//OsdTimerCountDown, //EnablePowerDownCounter, //NULL, // ExecFunction;
        MenuPage4, // Fonts
        mpbMoving | mpbStay | mpbCenter // Flags;
    },
    // UnsupportedModeMenu
    {
        MessageMenuHSize, MessageMenuVSize, //  XSize, YSize;
        RootMenu,// PrevMenuPage;
        UnsupportedModeMenuItems, // MenuItems;
        sizeof( UnsupportedModeMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL,//OsdTimerCountDown, //NULL, // ExecFunction;
        MenuPage5, // Fonts
        mpbMoving | mpbStay | mpbCenter // Flags;
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
    // BriteContMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        MainMenu,// PrevMenuPage;
        BriteContMenuItems, // MenuItems;
        sizeof( BriteContMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay  //   Flags;
    },
    // ColorSettingsMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        MainMenu,// PrevMenuPage;
        ColorSettingsMenuItems, // MenuItems;
        sizeof( ColorSettingsMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay  //   Flags;
    },
    // ExtColorSettingsMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        MainMenu,// PrevMenuPage;
        ExtColorSettingsMenuItems, // MenuItems;
        sizeof( ExtColorSettingsMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay  //   Flags;
    },
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
    // DisplaySettingsMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        MainMenu,// PrevMenuPage;
        DisplaySettingsMenuItems, // MenuItems;
        sizeof( DisplaySettingsMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay  //   Flags;
    },
    // OtherSettingsMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        MainMenu,// PrevMenuPage;
        OtherSettingsMenuItems, // MenuItems;
        sizeof( OtherSettingsMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay  //   Flags;
    },
    // PowerManagerMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        MainMenu,// PrevMenuPage;
        PowerManagerMenuItems, // MenuItems;
        sizeof( PowerManagerMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay  //   Flags;
    },



    // ContrastMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        BriteContMenu,// PrevMenuPage;
        ContrastMenuItems, // MenuItems;
        sizeof( ContrastMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // BrightnessMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        BriteContMenu,// PrevMenuPage;
        BrightnessMenuItems, // MenuItems;
        sizeof( BrightnessMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // RedMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        ColorSettingsMenu,// PrevMenuPage;
        RedMenuItems, // MenuItems;
        sizeof( RedMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // GreenMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        ColorSettingsMenu,// PrevMenuPage;
        GreenMenuItems, // MenuItems;
        sizeof( GreenMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // BlueMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        ColorSettingsMenu,// PrevMenuPage;
        BlueMenuItems, // MenuItems;
        sizeof( BlueMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // HueMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        ColorSettingsMenu,// PrevMenuPage;
        HueMenuItems, // MenuItems;
        sizeof( HueMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
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
        DisplaySettingsMenu,// PrevMenuPage;
        HPositionMenuItems, // MenuItems;
        sizeof( HPositionMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // VPositionMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        DisplaySettingsMenu,// PrevMenuPage;
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
        DisplaySettingsMenu,// PrevMenuPage;
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
        DisplaySettingsMenu,// PrevMenuPage;
        ClockMenuItems, // MenuItems;
        sizeof( ClockMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // FocusMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        DisplaySettingsMenu,// PrevMenuPage;
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
        OtherSettingsMenu,// PrevMenuPage;
        OSDTransMenuItems, // MenuItems;
        sizeof( OSDTransMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // OSDHPositionMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        OtherSettingsMenu,// PrevMenuPage;
        OSDHPositionMenuItems ,// MenuItems;
        sizeof( OSDHPositionMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // OSDVPositionMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        OtherSettingsMenu,// PrevMenuPage;
        OSDVPositionMenuItems ,// MenuItems;
        sizeof( OSDVPositionMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // OSDTimeoutMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        OtherSettingsMenu,// PrevMenuPage;
        OSDTimeoutMenuItems ,// MenuItems;
        sizeof( OSDTimeoutMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
#if AudioFunc
    // VolumeMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        OtherSettingsMenu,// PrevMenuPage;
        VolumeMenuItems ,// MenuItems;
        sizeof( VolumeMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // AudioSourceSelMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        OtherSettingsMenu,// PrevMenuPage;
        AudioSourceSelMenuItems ,// MenuItems;
        sizeof( AudioSourceSelMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },    
#endif

#if ENABLE_OSD_ROTATION
    // OSDRotateMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        OtherSettingsMenu,// PrevMenuPage;
        OSDRotateMenuItems ,// MenuItems;
        sizeof( OSDRotateMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
 #endif
#if ENABLE_FREESYNC
    // FreeSyncMenu,    //Scar
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        OtherSettingsMenu,// PrevMenuPage;
        FreeSyncMenuItems ,// MenuItems;
        sizeof( FreeSyncMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
#endif
    // DisplayLogoMenu,    //Scar
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        OtherSettingsMenu,// PrevMenuPage;
        DisplayLogoMenuItems ,// MenuItems;
        sizeof( DisplayLogoMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
    // GammaMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        DisplaySettingsMenu,// PrevMenuPage;
        GammaMenuItems, // MenuItems;
        sizeof( GammaMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
#if Enable_Expansion
    // AspectRatioMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        DisplaySettingsMenu,// PrevMenuPage;
        AspectRatioMenuItems ,// MenuItems;
        sizeof( AspectRatioMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
#endif
#if ENABLE_RTE
    // ODMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        DisplaySettingsMenu,// PrevMenuPage;
        ODMenuItems ,// MenuItems;
        sizeof( ODMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
#endif
    // ColorModeMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        ColorSettingsMenu,// PrevMenuPage;
        ColorModeMenuItems, // MenuItems;
        sizeof( ColorModeMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
#if ENABLE_SUPER_RESOLUTION
    // SuperResolutionMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        ColorSettingsMenu,// PrevMenuPage;
        SuperResolutionMenuItems, // MenuItems;
        sizeof( SuperResolutionMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
#endif
    // ColorFormatMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        ColorSettingsMenu,// PrevMenuPage;
        ColorFormatMenuItems, // MenuItems;
        sizeof( ColorFormatMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
    // ColorTempMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        ColorSettingsMenu,// PrevMenuPage;
        ColorTempMenuItems, // MenuItems;
        sizeof( ColorTempMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
    #if ENABLE_DLC
    // DLCMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //    XSize, YSize;
        BriteContMenu,// PrevMenuPage;
        DLCMenuItems, // MenuItems;
        sizeof( DLCMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
    #endif
    #if ENABLE_DPS
    // DPSMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //    XSize, YSize;
        BriteContMenu,// PrevMenuPage;
        DPSMenuItems, // MenuItems;
        sizeof( DPSMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
    #endif
    // DCRMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //    XSize, YSize;
        BriteContMenu,// PrevMenuPage;
        DCRMenuItems, // MenuItems;
        sizeof( DCRMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
    // ColorRangeMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //    XSize, YSize;
        ExtColorSettingsMenu,// PrevMenuPage;
        ColorRangeMenuItems, // MenuItems;
        sizeof( ColorRangeMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
    // IndependentHueMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        ExtColorSettingsMenu,// PrevMenuPage;
        IndependentHueMenuItems, // MenuItems;
        sizeof( IndependentHueMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
    // IndependentSaturationMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        ExtColorSettingsMenu,// PrevMenuPage;
        IndependentSaturationMenuItems, // MenuItems;
        sizeof( IndependentSaturationMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
    // IndependentBrightnessMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        ExtColorSettingsMenu,// PrevMenuPage;
        IndependentBrightnessMenuItems, // MenuItems;
        sizeof( IndependentBrightnessMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay //   Flags;
    },
    // IndependentHueRMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        IndependentHueMenu,// PrevMenuPage;
        IndependentHueRMenuItems, // MenuItems;
        sizeof( IndependentHueRMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // IndependentHueGMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        IndependentHueMenu,// PrevMenuPage;
        IndependentHueGMenuItems, // MenuItems;
        sizeof( IndependentHueGMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // IndependentHueBMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        IndependentHueMenu,// PrevMenuPage;
        IndependentHueBMenuItems, // MenuItems;
        sizeof( IndependentHueBMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // IndependentHueCMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        IndependentHueMenu,// PrevMenuPage;
        IndependentHueCMenuItems, // MenuItems;
        sizeof( IndependentHueCMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // IndependentHueMMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        IndependentHueMenu,// PrevMenuPage;
        IndependentHueMMenuItems, // MenuItems;
        sizeof( IndependentHueMMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // IndependentHueYMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        IndependentHueMenu,// PrevMenuPage;
        IndependentHueYMenuItems, // MenuItems;
        sizeof( IndependentHueYMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // IndependentSaturationRMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        IndependentSaturationMenu,// PrevMenuPage;
        IndependentSaturationRMenuItems, // MenuItems;
        sizeof( IndependentSaturationRMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // IndependentSaturationGMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        IndependentSaturationMenu,// PrevMenuPage;
        IndependentSaturationGMenuItems, // MenuItems;
        sizeof( IndependentSaturationGMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // IndependentSaturationBMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        IndependentSaturationMenu,// PrevMenuPage;
        IndependentSaturationBMenuItems, // MenuItems;
        sizeof( IndependentSaturationBMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // IndependentSaturationCMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        IndependentSaturationMenu,// PrevMenuPage;
        IndependentSaturationCMenuItems, // MenuItems;
        sizeof( IndependentSaturationCMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // IndependentSaturationMMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        IndependentSaturationMenu,// PrevMenuPage;
        IndependentSaturationMMenuItems, // MenuItems;
        sizeof( IndependentSaturationMMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // IndependentSaturationYMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        IndependentSaturationMenu,// PrevMenuPage;
        IndependentSaturationYMenuItems, // MenuItems;
        sizeof( IndependentSaturationYMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // IndependentBrightnessRMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        IndependentBrightnessMenu,// PrevMenuPage;
        IndependentBrightnessRMenuItems, // MenuItems;
        sizeof( IndependentBrightnessRMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // IndependentBrightnessGMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        IndependentBrightnessMenu,// PrevMenuPage;
        IndependentBrightnessGMenuItems, // MenuItems;
        sizeof( IndependentBrightnessGMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // IndependentBrightnessBMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        IndependentBrightnessMenu,// PrevMenuPage;
        IndependentBrightnessBMenuItems, // MenuItems;
        sizeof( IndependentBrightnessBMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // IndependentBrightnessCMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        IndependentBrightnessMenu,// PrevMenuPage;
        IndependentBrightnessCMenuItems, // MenuItems;
        sizeof( IndependentBrightnessCMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // IndependentBrightnessMMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        IndependentBrightnessMenu,// PrevMenuPage;
        IndependentBrightnessMMenuItems, // MenuItems;
        sizeof( IndependentBrightnessMMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
    // IndependentBrightnessYMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        IndependentBrightnessMenu,// PrevMenuPage;
        IndependentBrightnessYMenuItems, // MenuItems;
        sizeof( IndependentBrightnessYMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay|mpbAdjust //   Flags;
    },
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
#if ENABLE_DP_INPUT    
    // DPVersionMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //    XSize, YSize;
        InputSourceMenu,// PrevMenuPage;
        DPVersionMenuItems, // MenuItems;
        sizeof( DPVersionMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay  //   Flags;
    },
#endif
    //DCOffDischargeMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //    XSize, YSize;
        PowerManagerMenu,// PrevMenuPage;
        DCOffDischargeMenuItems, // MenuItems;
        sizeof( DCOffDischargeMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay  //   Flags;
    },
    //PowerSavingMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //    XSize, YSize;
        PowerManagerMenu,// PrevMenuPage;
        PowerSavingMenuItems, // MenuItems;
        sizeof( DCOffDischargeMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay  //   Flags;
    },
    // ResetWaitMenu,
    {
        MessageMenuHSize, MessageMenuVSize, //  XSize, YSize;
        RootMenu,// PrevMenuPage;
        ResetWaitMenuMenuItems ,// MenuItems;
        sizeof( ResetWaitMenuMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        ResetAllSetting, // ExecFunction;
        MenuPage7, // // Fonts
        mpbCenter //   Flags;
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
        MenuPage10, // Fonts
        mpbStay | mpbCenter //   Flags;
    },


#if HotExpansion
    // HotExpansionMenu
    {
        30 , 8,
        RootMenu,// PrevMenuPage;
        HotKeyExpansionMenuItems ,// MenuItems;
        sizeof( HotKeyExpansionMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL ,// ExecFunction;
        NULL, // Fonts
        mpbRedraw | mpbStay | mpbCenter //   Flags;
    },
#endif
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

//----Eson End-----------------------------------------------------

#if ENABLE_DeBlocking
    //13 DeBlockingMenu
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //    XSize, YSize;
        RootMenu,// PrevMenuPage;
        DeBlockingMenuItems, // MenuItems;
        sizeof( DeBlockingMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay | mpbBrowse //|mpbAdjust //   Flags;
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

#if MWEFunction
    //24 ColorBoostMenu,
    {
        31/*29*/, 18, //    XSize, YSize;
        MainMenu,// PrevMenuPage;
        ColorBoostMenuItems, // MenuItems;
        sizeof( ColorBoostMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay | mpbBrowse //   Flags;
    },
    //25 FullMenu,
    {
        31/*29*/, 18, //    XSize, YSize;
        ColorBoostMenu,// PrevMenuPage;
        FreeColorFullMODEMenuItems, // MenuItems;
        sizeof( FreeColorFullMODEMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay | mpbBrowse //   Flags;
    },
    //26 SkinProtectMenu,
    {
        31/*29*/, 18, //    XSize, YSize;
        ColorBoostMenu,// PrevMenuPage;
        FreeColorSkinProtectMODEMenuItems, // MenuItems;
        sizeof( FreeColorSkinProtectMODEMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay | mpbBrowse //   Flags;
    },
    //27 GreenEnhanceMenu,
    {
        31/*29*/, 18, //    XSize, YSize;
        ColorBoostMenu,// PrevMenuPage;
        FreeColorGreenEnhanceMenuItems, // MenuItems;
        sizeof( FreeColorGreenEnhanceMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay | mpbBrowse //   Flags;
    },
    //28 BlueEnhanceMenu,
    {
        31/*29*/, 18, //    XSize, YSize;
        ColorBoostMenu,// PrevMenuPage;
        FreeColorBlueEnhanceMenuItems, // MenuItems;
        sizeof( FreeColorBlueEnhanceMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay | mpbBrowse //   Flags;
    },
    //29 AutoDetectMenu,
    {
        31/*29*/, 18, //    XSize, YSize;
        ColorBoostMenu,// PrevMenuPage;
        FreeColorAutoDetectMenuItems, // MenuItems;
        sizeof( FreeColorAutoDetectMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay | mpbBrowse //   Flags;
    },
    //30 DemoMenu,
    {
        31/*29*/, 18, //    XSize, YSize;
        ColorBoostMenu,// PrevMenuPage;
        ColorBoostDemoMenuItems, // MenuItems;
        sizeof( ColorBoostDemoMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay | mpbBrowse //   Flags;
    },

    //31 BrightFrameMenu,
    {
        31/*29*/, 18, //    XSize, YSize;
        MainMenu,// PrevMenuPage;
        BrightFrameMenuItems, // MenuItems;
        sizeof( BrightFrameMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // Fonts
        mpbStay //   Flags;
    },
    //32 BF_FrameSizeMenu
    {
        31/*29*/, 18, //      XSize, YSize;
        BrightFrameMenu,// PrevMenuPage;
        BFFrameSizeMenuItems, // MenuItems;
        sizeof( BFFrameSizeMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay | mpbBrowse //   Flags;
    },

    //33 BF_BrightnessMenu
    {
        31/*29*/, 18, //    XSize, YSize;
        BrightFrameMenu,// PrevMenuPage;
        BF_BrightnessMenuItems, // MenuItems;
        sizeof( BF_BrightnessMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay | mpbBrowse //   Flags;
    },
    //34 BF_ContrastMenu
    {
        31/*29*/, 18, //    XSize, YSize;
        BrightFrameMenu,// PrevMenuPage;
        BF_ContrastMenuItems, // MenuItems;
        sizeof( BF_ContrastMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay | mpbBrowse //   Flags;
    },
    //35 BF_HPosition
    {
        31/*29*/, 18, //    XSize, YSize;
        BrightFrameMenu, // BrightFrameMenu,// PrevMenuPage;
        BFHPositionMenuMenuItems, // MenuItems;
        sizeof( BFHPositionMenuMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay | mpbBrowse //   Flags;
    },
    //36 BF_VPositionMenu
    {
        31/*29*/, 18, //    XSize, YSize;
        BrightFrameMenu, // BrightFrameMenu,// PrevMenuPage;
        BFVPositionMenuMenuItems, // MenuItems;
        sizeof( BFVPositionMenuMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay | mpbBrowse //   Flags;
    },
    //35 BFSwitchMenu
    {
        31/*29*/, 18, //      XSize, YSize;
        BrightFrameMenu,// PrevMenuPage;
        BFSwitchMenuItems, // MenuItems;
        sizeof( BFSwitchMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay | mpbBrowse //     Flags;
    },
#endif
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
    //52 ResetMenu,
    {
        MAIN_MENU_H_SIZE/*29*/, MAIN_MENU_V_SIZE, //  XSize, YSize;
        RootMenu,// PrevMenuPage;
        ResetMenuItems ,// MenuItems;
        sizeof( ResetMenuItems ) / sizeof( MenuItemType ), // MenuItemCount;
        NULL, // ExecFunction;
        NULL, // // Fonts
        mpbStay | mpbBrowse //   Flags;
    },
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
        33, 32,//29,     XSize, YSize;  2011.9.13 cc - A022
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
};




