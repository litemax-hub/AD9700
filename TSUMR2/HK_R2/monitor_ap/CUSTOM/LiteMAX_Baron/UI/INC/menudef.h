
#ifndef MENUDEF_H
#define MENUDEF_H
#include "board.h"
#define Multi_Number    1
#define Multi_Gauge 1
#define OnlyDrawColor   BIT7
#define OnlyDrawCode    BIT6


//==================================
#define CENTER_ALIGN_LEN                12
#define CENTER_ALIGN_ENDPOS         0x29

typedef enum
{
    PowerOffMenu,       // 0
    PowerOnMenu,        // 1
//----Eson Start-----------------------------------------------------
    StandbyMenu,        // 2
    CableNotConnectedMenu,
    UnsupportedModeMenu,
    RootMenu,

    // user definition from here
    MainMenu,
    #if 1 //(LiteMAX_OSDtype==LiteMAX_OSD_standard)
	LuminanceMenu,
	SignalMenu,
	SoundMenu,
	ColorMenu,
	ImageMenu,
	ToolMenu,
	#endif
    BriteContMenu,
    ColorSettingsMenu,
    ExtColorSettingsMenu,
    InputSourceMenu,
    DisplaySettingsMenu,
    OtherSettingsMenu,
    PowerManagerMenu,

    ContrastMenu,
    BrightnessMenu,
    RedMenu,
    GreenMenu,
    BlueMenu,
    HueMenu,
    SaturationMenu,
#if (ENABLE_VGA_INPUT)
    HPositionMenu,
    VPositionMenu,
#endif
#if ENABLE_SHARPNESS
    SharpnessMenu,
#endif
#if (ENABLE_VGA_INPUT)
    ClockMenu,
    FocusMenu,
#endif
    OSDTransMenu,
    OSDHPositionMenu,
    OSDVPositionMenu,
    OSDTimeoutMenu,
    #if AudioFunc
    VolumeMenu,
    AudioSourceSelMenu,
    #endif
#if ENABLE_OSD_ROTATION
    OSDRotateMenu,
#endif
#if ENABLE_FREESYNC
    FreeSyncMenu, //Scar
#endif
    DisplayLogoMenu,
    GammaMenu,
#if Enable_Expansion
    AspectRatioMenu,
#endif
    #if ENABLE_RTE
    ODMenu,
    #endif
    ColorModeMenu,
#if ENABLE_SUPER_RESOLUTION
    SuperResolutionMenu,
#endif
    ColorFormatMenu,
    ColorTempMenu,
#if ENABLE_DLC
    DLCMenu,
#endif
#if ENABLE_DPS
    DPSMenu,
#endif

    DCRMenu,

    ColorRangeMenu,
    IndependentHueMenu,
    IndependentSaturationMenu,
    IndependentBrightnessMenu,

    IndependentHueRMenu,
    IndependentHueGMenu,
    IndependentHueBMenu,
    IndependentHueCMenu,
    IndependentHueMMenu,
    IndependentHueYMenu,
    IndependentSaturationRMenu,
    IndependentSaturationGMenu,
    IndependentSaturationBMenu,
    IndependentSaturationCMenu,
    IndependentSaturationMMenu,
    IndependentSaturationYMenu,
    IndependentBrightnessRMenu,
    IndependentBrightnessGMenu,
    IndependentBrightnessBMenu,
    IndependentBrightnessCMenu,
    IndependentBrightnessMMenu,
    IndependentBrightnessYMenu,

    SourceSelectMenu,
#if ENABLE_DP_INPUT
    DPVersionMenu,
#endif

    DCOffDischargeMenu,
    PowerSavingMenu,

    ResetWaitMenu,
#if (ENABLE_VGA_INPUT)
    ADCAutoColorMenu,
    AutoMenu,
#endif
    InputInfoMenu,

#if HotExpansion
    HotExpansionMenu,
#endif
#if HotInputSelect
    HotInputSelectMenu,
#endif

//---Eson End----------------------

#if ENABLE_DeBlocking
    DeBlockingMenu,
#endif
    ColorTempSelectMenu,
#if MWEFunction
    ColorBoostMenu,                                                           //27
    FullMenu, SkinProtectMenu, GreenEnhanceMenu, BlueEnhanceMenu, AutoDetectMenu, DemoMenu,// 28,29,30,31,32,33

    BrightFrameMenu,  //34
    BFFrameSizeMenu, BF_BrightnessMenu, BF_ContrastMenu, BFHPositionMenu, BFVPositionMenu, BFSwitchMenu,// 35,36,37,38,39,40
#endif


    #if INPUT_TYPE!=INPUT_1A
    InputSelectMenu,
    #endif
#if DDCCI_ENABLE
    DDCCIMenu,
#endif
    ResetMenu,

    HotKeyECOMenu, // 54
    HotKeyBrightnessMenu, // 55
#if AudioFunc
    HotKeyVolMenu, // 55
#if (ENABLE_HDMI || ENABLE_DP_INPUT) && ENABLE_DAC
    HotKeySourceVolMenu,        //111024 Rick add - A055
#endif
#endif
	
    OsdLockMenu,//58

    AutoColorMenu,    //59

    BurninMenu,//60

    FactoryMenu,//    61

    LogoMenu,     //62
	
#if 1//(LiteMAX_OSDtype==LiteMAX_OSD_standard)
  #if 0// jason 20200113  BrightnessLightSensorVR
	BrightnessTypeMenu,
  #endif
	//ContrastMenu,
	SoundAdjustMenu,
	ImageClockMenu,
	ImagePhaseMenu,
	ImageHPosMenu,
	ImageVPosMenu,
	OSDControlMenu,
	OSDTimeMenu,
	OSDHPosMenu,
	OSDVPosMenu,
  #if ENABLE_OSD_ROTATION
	OSDRotationMenu,
  #endif
  	DefaultMenu,
  	PowerLockMenu,
#endif
    //  OsdLockMenu,
    // user definition end
    MaxMenu
} MenuPageIndexType;

#if LiteMAX_Baron_OSD_TEST
typedef enum
{
  MAIN_BRIGHTNESS_ITEM = 0,
  MAIN_COLOR_ITEM,
  MAIN_DEFAULT_ITEM,
  MAIN_POWER_ENABLE_ITEM,
  MAIN_EXIT_ITEM,
  MAIN_MAX_ITEM
}MainMenuItemsType;
typedef enum
{
  MAIN_LUMINANCE_ICON = 0,
  MAIN_SIGNAL_ICON,
  MAIN_SOUND_ICON,
  MAIN_COLOR_ICON,
  MAIN_IMAGE_ICON,
  MAIN_OTHER_ICON,
  MAIN_EXIT_ICON,
  MAIN_MAX_ICON
}MainMenuIconType;
typedef enum
{
  MAIN_POWER_ENABLE_ICON = 0,
  MAIN_POWER_DISBLE_ICON,
  MAIN_POWER_MAX_ICON
}MainMenuPowerIconType;

//MAIN_COLOR_ITEM
typedef enum
{
  ColorTempMenuSub6500K_Item = 0,
  ColorTempMenuSubUserOption_Item,
  ColorTempMenuSubReturn_Item,
  ColorTempMenuSubMax_Item
}ColorTempMenuSubItemsType;

typedef enum
{
  ColorSub_Icon = 0,
  ColorSubsRGB_Icon,
  ColorSubUser_Icon,
  ColorSubRGB_Icon,
  ColorSubMaxRGB_Icon,
  ColorSubAuto_Icon,
  ColorSub6500K_Icon,
  ColorSub9300K_Icon,
  ColorSubMax_Icon
}ColorSubIconType;

typedef enum
{
  DefaultMenuSubYes_Item = 0,
  DefaultMenuSubNo_Item,
  DefaultMenuSubMax_Item
}DefaultMenuSubItemsType;

typedef enum
{
  LoadDefaultSubYes_Icon = 0,
  LoadDefaultSubNo_Icon,
  LoadDefaultSubMax_Icon
}LoadDefaultSubIconType;

typedef enum
{
  OtherSubOSDControl_Icon = 0,
  OtherSubOSDReset_Icon,
  OtherSubSharpness_Icon,
  OtherSubOSDTime_Icon,
  OtherSubOSDHpos_Icon,
  OtherSubOSDVpos_Icon,
  OtherSubMax_Icon
}OtherSubIconType;
#endif

typedef enum
{
  LUMINANCE_CONTRAST_ITEM,
  LUMINANCE_BRIGHTNESS_ITEM,
  LUMINANCE_ECO_ITEM,
  #if Enable_Gamma
  LUMINANCE_GAMMA_ITEM,
  #endif
  LUMINANCE_DCR_ITEM,

  LUMINANCE_MAX_ITEM,
}LuminanceMenuItemsType;

typedef enum
{
  IMAGESETUP_CLOCK_ITEM,
  IMAGESETUP_PHASE_ITEM,
  IMAGESETUP_HPOSITION_ITEM,
  IMAGESETUP_VPOSITION_ITEM,
  #if Enable_Expansion
  IMAGESETUP_IMAGERATIO_ITEM,
  #endif
#if ENABLE_DPS
  IMAGESETUP_DPS_ITEM,
#endif
#if ENABLE_SUPER_RESOLUTION
  IMAGESETUP_SUPERRESOLUTION_ITEM,
#endif
#if ENABLE_SHARPNESS
  IMAGESETUP_SHARPNESS_ITEM,
#endif
#if ENABLE_DeBlocking
  IMAGESETUP_DEBLOCKING_ITEM,
#endif

  IMAGESETUP_MAX_ITEM,
}ImageSetupMenuItemsType;

typedef enum
{
  COLORTEMP_COLORTEMPSELECT_ITEM,
  COLORTEMP_USERCOLOR_R_ITEM,
  COLORTEMP_USERCOLOR_G_ITEM,
  COLORTEMP_USERCOLOR_B_ITEM,
  COLORTEMP_MAX_ITEM,
}ColorTempMenuItemsType;

typedef enum
{
  COLORBOOST_FULL_ITEM,
  COLORBOOST_SKINPROTECT_ITEM,
  COLORBOOST_GREENENHANCE_ITEM,
  COLORBOOST_BLUEENHANCE_ITEM,
  COLORBOOST_AUTODETECT_ITEM,
  COLORBOOST_DEMO_ITEM,
  COLORBOOST_MAX_ITEM,
}ColorBoostMenuItemsType;

typedef enum
{
  BRIGHTFRAME_FRAMESIZE_ITEM,
  BRIGHTFRAME_BRIGHTNESS_ITEM,
  BRIGHTFRAME_CONTRAST_ITEM,
  BRIGHTFRAME_HPOSITION_ITEM,
  BRIGHTFRAME_VPOSITION_ITEM,
  BRIGHTFRAME_SWITCH_ITEM,
  BRIGHTFRAME_MAX_ITEM,
}BrightFrameMenuItemsType;

typedef enum
{
  OSDSETUP_HPOSITION_ITEM,
  OSDSETUP_VPOSITION_ITEM,
  OSDSETUP_TIMEOUT_ITEM,
  OSDSETUP_LANGUAGE_ITEM,
  OSDSETUP_MAX_ITEM,
}OSDSetupMenuItemsType;

typedef enum
{
  #if INPUT_TYPE!=INPUT_1A
  EXTRA_INPUTSELECT_ITEM,
  #endif
  EXTRA_DDCCIONOFF_ITEM,
  EXTRA_FACTORYRECALL_ITEM,
  EXTRA_MAX_ITEM,
}ExtraMenuItemsType;

typedef enum
{
  EXTRAMENUINFO_INFORMATION_ITEM,
  EXTRAMENUINFO_RESOLUTION_ITEM,
  EXTRAMENUINFO_HFREQ_ITEM,
  EXTRAMENUINFO_VFREQ_ITEM,
}ExtraMenuInfoItemsType;

typedef enum
{
    COLORBOOST_FULL,       //4 //1
    COLORBOOST_HALF,    // 2
    COLORBOOST_OFF,   // 3
    COLORBOOST_Nums

} ColorBoostType;


typedef enum
{
    ECO_Standard,       //4 //1
    ECO_Text,          // 2
    ECO_Internet,   // 3
    ECO_Game,        // 4
    ECO_Movie,        // 5
    ECO_Sports,       //4 // 6

    ECO_Nums

} ECOModeType;

#if 1//Enable_Expansion
typedef enum
{
    Expansion_Full,
    Expansion_4_3,
    Expansion_16_9,
    Expansion_1_1,
    Expansion_KeepInputRatio,
    Expansion_OverScan,
    Expansion_Nums
} ExpansionType;
#endif

typedef enum
{
    SRMODE_OFF,//SRMODE1,          // 0   high
    SRMODE_WEAK,//SRMODE2,   // 1     middle
    SRMODE_MIDDLE,//SRMODE3,        // 2    low
    SRMODE_STRONG,//SRMODE_OFF,     // 3

    SRMODE_Nums
} SRModeType;


typedef enum
{
    NRMODE1,          // 0   high
    NRMODE2,      // 1     middle
    NRMODE3,          // 2    low
    NRMODE_OFF,     // 3

    NRMODE_Nums
} NRModeType;

typedef enum
{
    CFMT_RGB,
    CFMT_YUV,
    CFMT_Auto,
    
    CFMT_Nums
} ColorFormatType;

#if LiteMAX_Baron_OSD_TEST
typedef enum
{
	CTEMP_Warm1,  // 6500k
	CTEMP_USER, //	USER
	CTEMP_Nums,
	CTEMP_Cool1,   //  9300k
	CTEMP_Normal,  // 7800K
	CTEMP_SRGB	 //  SRGB
} ColorTempType;

#else
typedef enum
{
    CTEMP_Cool1,   //  8500K
    //  CTEMP_Warm2,  // 6500K
    CTEMP_Normal,  // 7800K
    CTEMP_SRGB,   //  SRGB
    CTEMP_USER, //  USER
    CTEMP_Warm1,  // 5700K//   CTEMP_Cool2,   //  9300K

    CTEMP_Nums,
} ColorTempType;
#endif

#if LANGUAGE_TYPE==ASIA
typedef enum
{
    LANG_English,
    LANG_Spanish,
    LANG_France,
    LANG_Portugues,
    LANG_Russia,
    LANG_Korea,
    LANG_TChina,
    LANG_SChina,
    LANG_Nums
} LanguageType;
#elif LANGUAGE_TYPE==SIXTEEN                    //090330 xiandi.yu
typedef enum
{
    LANG_English,
    LANG_German,
    LANG_France,
    LANG_Italian,
    LANG_Spanish,
    LANG_SChina,
    LANG_Japanese,
    LANG_Norsk,
    LANG_Russia,
    LANG_Swedish,
    LANG_Finnish,
    LANG_Dansk,
    LANG_Polish,
    LANG_Dutch,
    LANG_Portugues,
    LANG_Korea,
    LANG_Nums
} LanguageType;
#elif LANGUAGE_TYPE==AllLanguage                    //100324 xiandi.yu
typedef enum
{
    LANG_TChina,
    LANG_English,
    LANG_France,
    LANG_German,
    LANG_Italian,
    LANG_Japanese,
    LANG_Korea,
    LANG_Russia,
    LANG_Spanish,
    LANG_Swedish,
    LANG_Turkish,
    LANG_SChina,
    LANG_Portugues,
    LANG_Czech,
    LANG_Dutch,
    LANG_Finnish,
    LANG_Polish,
    LANG_Nums
} LanguageType;

#elif LANGUAGE_TYPE==EUROPE
typedef enum
{
    LANG_English,
    LANG_France,
    LANG_Spanish,
    LANG_German,
    LANG_Russia,
    LANG_Italian,
    LANG_Swedish,
    LANG_Polish,
    LANG_Czech,
    LANG_Nums
} LanguageType;
#endif


#if Enable_Gamma
typedef enum
{
    GAMA1,          // 0
    GAMA2,   // 1
    GAMA3,        // 2

    GAMA_Nums
} GAMAModeType;
#endif

typedef enum
{
    MIA_Nothing,
    MIA_GotoNext, // goto next
    MIA_GotoPrev, // goto prev
    MIA_PrevItem,
    MIA_NextItem,
    MIA_JumpNaviItem,
    MIA_JumpAdjustItem,
    MIA_Exit, // Exit
    MIA_IncValue, // increase value
    MIA_DecValue, // decrease value
    MIA_ExecFunc,
    MIA_ExecFunc_And_GotoPrev,   // goto Prev Page
    #if LiteMAX_Baron_OSD_TEST
    MIA_GotoNextExec, // goto next
    #endif
    MIA_Auto,
    MIA_Brite,
    MIA_Cont,
    MIA_InvIncValue,
    MIA_InvDecValue,
    MIA_RedrawMenu,
    MIA_GotoECO,
    MIA_ECO,
    MIA_SourceMenu,
#if AudioFunc
    MIA_VOL,
    MIA_Mute,
#endif
    MIA_SourceSel, //2006-02-22
#if HotExpansion
    MIA_Expansion,
#endif
    MIA_EXEC_FACTORY_NVRAMINIT,
    MIA_Power // power control
} MenuItemActionType;


typedef enum
{
    DWI_Nothing,
    DWI_Icon,
    DWI_Text,
    DWI_FullText,
    DWI_CenterText,
    DWI_FullCenterText,
    DWI_None
} DrawMenuItemType;

typedef BYTE *( *fpDisplayText )( void );
typedef Bool( *fpExecFunc )( void );
typedef BYTE( *fpItemIndex )( void );
typedef Bool( *fpAdjustValue )( MenuItemActionType action );
typedef WORD( *fpGetValue )( void );
typedef WORD( *fpGetMaxValue )( void );
typedef WORD( *fpSetMaxValue )( void );
#define mpbStay         0x001 // for stay in menu page
#define mpbInvisible    0x002 // menu page visible or not
#define mpbBrowse       0x004
#define mpbAdjust       0x008
#define mpbRedraw       0x010
#define mpbMoving       0x020
#define mpbCenter       0x040
#define mpbLogoFrame    0x080

#define mibSelectable   0x01 // for menu item selectable
#define mibInvisible    0x02 // for menu item selectable
#define mibAudioSelDisable   0x04
#define mibVGAItem   0x08
#define mibFactoryItem  0x016
//1015  #define mibClrInner1    0x10
#define mibYUVEnable       0x20
//#define mibCenter       0x10
#define mibsRGBDisable 	0x40
#define mibClrInner 	0x80
#define mibDrawValue    0x100
#define mibUserColor    0x0200
#define mibStdEnable    0x0400
#define mibDCRDisable   0x0800
#define mibDLCDisable           0x1000
#if ENABLE_DP_INPUT
#define mibMSTDisable      0x2000
#endif
#define mibBFEnable             0x4000
#define mibDemoDisable          0x8000
#if Enable_Expansion
#define mibExpansionDisable4_3              0x10000
#define mibExpansionDisable16_9             0x20000
#define mibExpansionDisable1_1              0x40000
#define mibExpansionDisableFixInputRatio    0x80000
#endif


#define dwiNumber                   0x01
#define dwiNULL                     0x02
#define dwiRadioGroup               0x04
#define dwiEnd                      0x08
#define dwiHex                          0x10
#define dwiOptionBar                    0x20
#define dwiUserColor                    0x40
#define dwiVideoOption   0x80
#define dwiAudioSourceOption   0x100
#define dwiVGAItem              0x200

//#define dwiRightAlign                   0x80
#define dwiCenterArrowAlign          0x200
#define dwiHotKeyCenterArrowAlign   0x400

typedef struct
{
    fpAdjustValue AdjustFunction;
    fpExecFunc ExecFunction;
} MenuItemFuncType;
#if Multi_Number //multi-number

typedef struct
{
    //BYTE Flags;
    WORD Flags;
    BYTE XPos, YPos;
    fpGetValue GetValue;
} NumberType;

typedef struct
{
    // BYTE Flags;
    BYTE ForeColor, BackColor;
    const NumberType *DisplayNumber;
} DrawNumberType;
#else // single number
typedef struct
{
    BYTE Flags;
    BYTE ForeColor, BackColor;
    BYTE XPos, YPos;
    fpGetValue GetValue;
} DrawNumberType;
#endif
#if Multi_Gauge // multi-gauge
typedef struct
{
    //BYTE Flags;
	WORD Flags;
    BYTE XPos, YPos;
    fpGetValue GetValue;
} GaugeType;
typedef struct
{
    //BYTE Flags;
    BYTE ForeColor, BackColor;
    BYTE Length;
    const GaugeType *DisplayGauge;
} DrawGuageType;
#else // single gauge
typedef struct
{
    BYTE Flags;
    BYTE ForeColor, BackColor;
    BYTE Length;
    BYTE XPos, YPos;
    fpGetValue GetValue;
} DrawGuageType;
#endif
typedef struct
{
    WORD Flags;
    BYTE XPos, YPos;
    const fpDisplayText DisplayText;
} RadioTextType;
typedef struct
{
    // BYTE Flags;
    BYTE ForeColor, BackColor;
    fpGetValue GetValue;
    const RadioTextType *RadioText;
} DrawRadioGroupType;
typedef struct
{
    const DrawNumberType *DrawNumber;
    const DrawGuageType *DrawGuage;
    const DrawRadioGroupType *DrawRadioGroup;
} DrawValueType;
typedef struct
{
    const BYTE *Fonts;
    WORD FontCount;
} MenuFontType;
#if 0
typedef struct
{
    BYTE XPos, YPos;
    BYTE ForeColor, BackColor;
    BYTE SelForeColor, SelBackColor;
    BYTE NextPage;
    DrawMenuItemType DrawItemMethod;
    fpDisplayText DisplayText;
    MenuItemActionType *KeyEvents;
    MenuItemFuncType KeyFunction;
    DrawValueType DisplayValue;
    BYTE Flags;
} MenuItemType;
#endif
typedef struct
{
    BYTE XPos, YPos;
    BYTE ForeColor, BackColor;
    BYTE SelForeColor, SelBackColor;
#if 0
    BYTE NextPage;
#else
    MenuPageIndexType NextPage;
#endif
    DrawMenuItemType DrawItemMethod;
    fpDisplayText DisplayText;
    const MenuItemActionType *KeyEvents;
    MenuItemFuncType KeyFunction;
    DrawValueType DisplayValue;
    const MenuFontType *Fonts;
    DWORD Flags; // 2006/9/6 5:41PM by Emily BYTE Flags;
} MenuItemType;
#if 0
typedef struct
{
    BYTE SpaceWidth;
    WORD LineData[18];
} PropFontType;
#endif
#if 0
typedef struct
{
    BYTE XSize, YSize;
    BYTE PrevPage;
    MenuItemType *MenuItems;
    BYTE ItemCount;
    fpExecFunc ExecFunction;
    BYTE Flags;
} MenuPageType;
#endif
typedef struct
{
    BYTE XSize, YSize;
    BYTE PrevPage;
    const MenuItemType *MenuItems;
    BYTE ItemCount;
    fpExecFunc ExecFunction;
    const MenuFontType *Fonts;
    WORD Flags;
} MenuPageType;
#endif
