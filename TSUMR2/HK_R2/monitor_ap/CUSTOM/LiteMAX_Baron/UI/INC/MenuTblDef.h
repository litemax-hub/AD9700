
#include "menudef.h"
#include "MenuStr.h"

#define MAIN_MENU_H_SIZE     		33
#define MAIN_MENU_V_SIZE             8

#define MessageMenuHSize			26
#define MessageMenuVSize       		6

#define MessageMenu2HSize			20
#define MessageMenu2VSize      		4

#define MainMenuIcon_X_Start        3
#define MainMenuIcon_Y_Start        4
#define MainMenusStr_Y_Start        6
#define SubMenuIcon_X_Start         3
#define SubMenuIcon_Y_Start         3

#define SubMenuValue_Y_Start        11
#define SubMenuBar_Y_Start          13
#define UserRed_Y_Start             9
#define UserGreen_Y_Start           11
#define UserBlue_Y_Start            13
#define Source_Y_Start              11
//===============================================
#define FACTORY_MENU_H_SIZE     	50
#define FACTORY_MENU_V_SIZE          8

#define FactoryMenu_X1_Start        16
#define FactoryMenu_X2_Start        24
#define FactoryMenu_X3_Start        35

//=====================================================================================
// Key function definition
MenuItemActionType code InformationEvent[BTN_EndBTN] =
{
    MIA_NextItem,   // BTN_Plus
    MIA_PrevItem,   // BTN_Minus
    MIA_Nothing,    // BTN_Menu
    MIA_GotoPrev,   // BTN_Exit
    MIA_Power,      // BTN_Power
    MIA_GotoPrev,
};

#if INPUT_TYPE==INPUT_1A
MenuItemActionType code NaviKeyEvent[BTN_EndBTN] =
{
    MIA_NextItem,   // BTN_Plus
    MIA_PrevItem,   // BTN_Minus
    MIA_GotoNext,   // BTN_Menu
    MIA_GotoPrev,//MIA_Nothing,     // BTN_Exit // 070117 root page exit-key event
    MIA_Power,  // BTN_Power
    MIA_GotoPrev,
};
MenuItemActionType code NaviExecKeyEvent[BTN_EndBTN] =
{
    MIA_NextItem,   // BTN_Plus
    MIA_PrevItem,   // BTN_Minus
    MIA_ExecFunc,     // BTN_Menu
    MIA_GotoPrev,//MIA_Nothing,   // BTN_Exit // 070117
    MIA_Power,    // BTN_Power
    MIA_GotoPrev,
};
#else
MenuItemActionType code NaviKeyEvent[BTN_EndBTN] =
{
    MIA_NextItem,   // BTN_Plus
    MIA_PrevItem,   // BTN_Minus
    MIA_GotoNext,   // BTN_Menu
    MIA_GotoPrev,   // BTN_Exit
    MIA_Power,  // BTN_Power
    MIA_SourceSel, //2006-02-22
};
MenuItemActionType code NaviExecKeyEvent[BTN_EndBTN] =
{
    MIA_NextItem,     // BTN_Plus
    MIA_PrevItem,     // BTN_Minus
    MIA_ExecFunc,     // BTN_Menu
    MIA_GotoPrev,     // BTN_Exit
    MIA_Power,    // BTN_Power
    MIA_SourceSel, //2006-02-22
};
#if LiteMAX_Baron_OSD_TEST
MenuItemActionType code NaviExec2KeyEvent[BTN_EndBTN] =
{
    MIA_NextItem,   // BTN_Plus
    MIA_PrevItem,   // BTN_Minus
    MIA_GotoNextExec,     // BTN_Menu
    MIA_GotoPrev,//MIA_Nothing,   // BTN_Exit // 070117
    MIA_Power,    // BTN_Power
    MIA_GotoPrev,
};
#endif
#endif
MenuItemActionType code NaviExecFuncGotoPrevKeyEvent[BTN_EndBTN]=
{ 
    MIA_NextItem,   // BTN_Plus
    MIA_PrevItem,   // BTN_Minus
    MIA_ExecFunc_And_GotoPrev,  // BTN_Menu
    MIA_GotoPrev,               // BTN_Exit
    MIA_Power,                   // BTN_Power
    MIA_SourceSel, //2006-02-22
};

#if (AudioFunc && (ENABLE_HDMI || ENABLE_DP_INPUT))
MenuItemActionType code AudioNaviKeyEvent[BTN_EndBTN] =
{
    MIA_NextItem,     // BTN_Plus
    MIA_PrevItem,     // BTN_Minus
    MIA_JumpAdjustItem,   // BTN_Menu
    MIA_GotoPrev,     // BTN_Exit
    MIA_Power,    // BTN_Power
    MIA_SourceSel, //2006-02-22
};
MenuItemActionType code AudioAdjustKeyEvent[BTN_EndBTN] =
{
    MIA_IncValue,     // BTN_Plus
    MIA_DecValue,     // BTN_Minus
    MIA_NextItem,     // BTN_Menu
    MIA_PrevItem,     // BTN_Exit
    MIA_Power,    // BTN_Power
    MIA_SourceSel, //2006-02-22
};
#endif
MenuItemActionType code AdjusterKeyEvent[BTN_EndBTN] =
{
    MIA_IncValue,     // BTN_Plus
    MIA_DecValue,     // BTN_Minus
    MIA_GotoNext,     // BTN_Menu
    MIA_GotoPrev, // MIA_Exit,    // BTN_Exit
    MIA_Power,    // BTN_Power
    MIA_SourceSel, //2006-02-22
};

MenuItemActionType code AdjusterKey1Event[BTN_EndBTN] =
{
    MIA_DecValue,     // BTN_Plus
    MIA_IncValue,     // BTN_Minus
    MIA_GotoNext,     // BTN_Menu
    MIA_GotoPrev, // MIA_Exit,    // BTN_Exit
    MIA_Power,    // BTN_Power
    MIA_SourceSel, //2006-02-22
};

MenuItemActionType code AdjusterItemKeyEvent[BTN_EndBTN] =
{
    MIA_IncValue,     // BTN_Plus
    MIA_DecValue,     // BTN_Minus
    MIA_NextItem,     // BTN_Menu
    MIA_PrevItem, // MIA_Exit,    // BTN_Exit
    MIA_Power,    // BTN_Power
    MIA_SourceSel, //2006-02-22
};

#if INPUT_TYPE==INPUT_1A
MenuItemActionType code AdjustExecKeyEvent[BTN_EndBTN] =
{
    MIA_IncValue,     // BTN_Plus
    MIA_DecValue,     // BTN_Minus
    MIA_ExecFunc,     // BTN_Menu
    MIA_Exit,     // BTN_Exit
    MIA_Power,    // BTN_Power
    MIA_Exit,
};
MenuItemActionType code AdjustNoExecKeyEvent[BTN_EndBTN] =
{
    MIA_IncValue,     // BTN_Plus
    MIA_DecValue,     // BTN_Minus
    MIA_GotoPrev,     // BTN_Menu
    MIA_GotoPrev,     // BTN_Exit
    MIA_Power,    // BTN_Power
    MIA_Exit,
};
MenuItemActionType code AdjustDDCHotKeyEvent[BTN_EndBTN] =
{
    MIA_IncValue,     // BTN_Plus
    MIA_DecValue,     // BTN_Minus
    MIA_ExecFunc,     // BTN_Menu
    MIA_Nothing,  // BTN_Exit
    MIA_Power,    // BTN_Power
    MIA_Nothing
};
MenuItemActionType code DirAdjusterKeyEvent[BTN_EndBTN] =
{
    MIA_IncValue,     // BTN_Plus
    MIA_DecValue,     // BTN_Minus
    MIA_GotoPrev,     // BTN_Menu
    MIA_GotoPrev,//MIA_Nothing,   // BTN_Exit // 070117
    MIA_Power,    // BTN_Power
    MIA_GotoPrev,
};
MenuItemActionType code InvSubAdjusterKeyEvent[BTN_EndBTN] =
{
    MIA_DecValue,     // BTN_Plus
    MIA_IncValue,     // BTN_Minus
    MIA_GotoPrev,     // BTN_Menu
    MIA_GotoPrev,//MIA_Nothing,   // BTN_Exit // 070117
    MIA_Power,    // BTN_Power
    MIA_GotoPrev
};
#else
MenuItemActionType code AdjustExecKeyEvent[BTN_EndBTN] =
{
    MIA_IncValue,     // BTN_Plus
    MIA_DecValue,     // BTN_Minus
    MIA_ExecFunc,     // BTN_Menu
    MIA_Exit,     // BTN_Exit
    MIA_Power,    // BTN_Power
    MIA_SourceSel, //2006-02-22
};
MenuItemActionType code AdjustNoExecKeyEvent[BTN_EndBTN] =
{
    MIA_IncValue,     // BTN_Plus
    MIA_DecValue,     // BTN_Minus
    MIA_GotoPrev,     // BTN_Menu
    MIA_GotoPrev,     //MIA_Exit,     // BTN_Exit
    MIA_Power,    // BTN_Power
    MIA_SourceSel, //2006-02-22
};
MenuItemActionType code AdjustDDCHotKeyEvent[BTN_EndBTN] =
{
    MIA_IncValue,     // BTN_Plus
    MIA_DecValue,     // BTN_Minus
    MIA_ExecFunc,     // BTN_Menu
    MIA_Nothing,  // BTN_Exit
    MIA_Power,    // BTN_Power
    MIA_SourceSel, //2006-02-22
};
MenuItemActionType code DirAdjusterKeyEvent[BTN_EndBTN] =
{
    MIA_IncValue,     // BTN_Plus
    MIA_DecValue,     // BTN_Minus
    MIA_GotoPrev,     // BTN_Menu
    MIA_GotoPrev,     // BTN_Exit
    MIA_Power,    // BTN_Power
    MIA_SourceSel, //2006-02-22
};
MenuItemActionType code InvSubAdjusterKeyEvent[BTN_EndBTN] =
{
    MIA_DecValue,     // BTN_Plus
    MIA_IncValue,     // BTN_Minus
    MIA_GotoPrev,     // BTN_Menu
    MIA_GotoPrev,     // BTN_Exit
    MIA_Power,    // BTN_Power
    MIA_SourceSel, //2006-02-22
};
#endif


//===========================================================
#if INPUT_TYPE==INPUT_1A
MenuItemActionType code FactoryNaviKeyEvent[BTN_EndBTN] =
{
    MIA_NextItem,     // BTN_Plus
    MIA_PrevItem,     // BTN_Minus
    MIA_JumpAdjustItem,   // BTN_Menu
    MIA_Exit,//MIA_Nothing,   // BTN_Exit // 070117
    MIA_Power,    // BTN_Power
    MIA_Exit,
};
MenuItemActionType code FactoryAdjustKeyEvent[BTN_EndBTN] =
{
    MIA_IncValue,     // BTN_Plus
    MIA_DecValue,     // BTN_Minus
    MIA_NextItem,     // BTN_Menu
    MIA_Exit,//MIA_Nothing,   // BTN_Exit // 070117
    MIA_Power,    // BTN_Power
    MIA_Exit,
};
MenuItemActionType code FactoryAdjustStayKeyEvent[BTN_EndBTN] =
{
    MIA_IncValue,     // BTN_Plus
    MIA_DecValue,     // BTN_Minus
    MIA_ExecFunc, // BTN_Menu
    MIA_JumpNaviItem,//MIA_Nothing,   // BTN_Exit // 070117
    MIA_Power,    // BTN_Power
    MIA_JumpNaviItem,
};
MenuItemActionType code FactoryExitKeyEvent[BTN_EndBTN] =// 2011.9.15 Rick modified
{
    MIA_NextItem,   // BTN_Plus
    MIA_PrevItem,   // BTN_Minus
    MIA_Exit,   // BTN_Menu
    MIA_Exit,//MIA_Nothing,     // BTN_Exit // 070117 root page exit-key event
    MIA_Power,  // BTN_Power
    MIA_Exit,
};
#else
MenuItemActionType code FactoryNaviKeyEvent[BTN_EndBTN] =
{
    MIA_NextItem,     // BTN_Plus
    MIA_PrevItem,     // BTN_Minus
    MIA_JumpAdjustItem,   // BTN_Menu
    MIA_Exit,     // BTN_Exit
    MIA_Power,    // BTN_Power
    MIA_SourceSel, //2006-02-22
};
MenuItemActionType code FactoryNaviExecKeyEvent[BTN_EndBTN] =
{
    MIA_NextItem,     // BTN_Plus
    MIA_PrevItem,     // BTN_Minus
    MIA_ExecFunc,   // BTN_Menu
    MIA_Exit,     // BTN_Exit
    MIA_Power,    // BTN_Power
    MIA_SourceSel, //2006-02-22
};
MenuItemActionType code FactoryAdjustKeyEvent[BTN_EndBTN] =
{
    MIA_IncValue,     // BTN_Plus
    MIA_DecValue,     // BTN_Minus
    MIA_NextItem,     // BTN_Menu
    MIA_Exit,     // BTN_Exit
    MIA_Power,    // BTN_Power
    MIA_SourceSel, //2006-02-22
};
MenuItemActionType code FactoryAdjustStayKeyEvent[BTN_EndBTN] =
{
    MIA_IncValue,     // BTN_Plus
    MIA_DecValue,     // BTN_Minus
    MIA_JumpNaviItem, // BTN_Menu
    MIA_Exit,     // BTN_Exit
    MIA_Power,    // BTN_Power
    MIA_SourceSel, //2006-02-22
};
MenuItemActionType code FactoryExitKeyEvent[BTN_EndBTN] = // 2011.9.15 Rick modified
{
    MIA_NextItem,   // BTN_Plus
    MIA_PrevItem,   // BTN_Minus
    MIA_Exit,   // BTN_Menu
    MIA_Exit,   // BTN_Exit
    MIA_Power,  // BTN_Power
    MIA_SourceSel, //2006-02-22
};
#endif

//===========================================================
MenuItemActionType code RootKeyEvent[BTN_EndBTN] =
{
    MIA_SourceSel,   // BTN_Plus
    MIA_Brite,  //MIA_VOL,   // BTN_Minus
    MIA_GotoNext,   // BTN_Menu
    MIA_GotoNext,   // BTN_Exit
    MIA_Power,      // BTN_Power
    MIA_SourceSel,
};
//============================================================
MenuItemActionType code LockKeyEvent[BTN_EndBTN] =
{
    MIA_GotoNext,         // BTN_Plus
    MIA_GotoNext,         // BTN_Minus
    MIA_GotoNext,     // BTN_Menu
    MIA_GotoNext,     // BTN_Exit
    MIA_Power,     // BTN_Power
    MIA_Nothing,
};
//============================================================
MenuItemActionType code LogoKeyEvent[BTN_EndBTN] =
{
    MIA_Nothing, // BTN_Plus KeyEvent[BTN_EndBTN];
    MIA_Nothing, // BTN_Minus
    MIA_Nothing, // BTN_Menu
    MIA_Nothing, // BTN_Exit
    MIA_Power, // BTN_Power
    MIA_Nothing

};


//============================================================
MenuItemActionType code NothingKeyEvent[BTN_EndBTN] =
{
    MIA_Nothing, // BTN_Plus KeyEvent[BTN_EndBTN];
    MIA_Nothing, // BTN_Minus
    MIA_Nothing, // BTN_Menu
    MIA_Nothing, // BTN_Exit
    MIA_Power, // BTN_Power
    MIA_Nothing // MIA_SourceSel

};
#ifdef SourcekeyWakeup
MenuItemActionType code NothingKeyEvent1[BTN_EndBTN] =
{
    MIA_Nothing, // BTN_Plus KeyEvent[BTN_EndBTN];
    MIA_Nothing, // BTN_Minus
    MIA_Nothing, // BTN_Menu
    MIA_SourceMenu,//, // BTN_Exit
    MIA_Power, // BTN_Power
    #if INPUT_TYPE==INPUT_1A
    MIA_Nothing,
    #else
    MIA_SourceSel,
    #endif
};
#endif
//============================================================
#if INPUT_TYPE==INPUT_1A
MenuItemActionType code ECOHotKeyEvent[BTN_EndBTN] =
{
    MIA_IncValue,     // BTN_Plus
    MIA_DecValue,     // BTN_Minus
    MIA_GotoPrev, // BTN_Menu
    MIA_GotoPrev,//MIA_Nothing, // BTN_Exit // 070117
    MIA_Power, // BTN_Power
    MIA_GotoPrev,
};
#else
MenuItemActionType code ECOHotKeyEvent[BTN_EndBTN] =
{
    MIA_IncValue,     // BTN_Plus
    MIA_DecValue,     // BTN_Minus
    MIA_GotoPrev, // BTN_Menu
    MIA_GotoPrev, // BTN_Exit
    MIA_Power, // BTN_Power
    MIA_SourceSel, //2006-02-22
};
#endif
#if HotInputSelect

#ifdef SixKeyAutoDirectAndSourceSelect
MenuItemActionType code UnsupportMenuKeyEvent[BTN_EndBTN] =
{
    MIA_Nothing,  // BTN_Plus KeyEvent[BTN_EndBTN];
    MIA_Nothing,  // BTN_Minus
    MIA_Nothing,  // BTN_Menu
    MIA_Nothing,  // BTN_Exit
    MIA_Power,    // BTN_Power
    MIA_SourceSel   // BTN_SourceSelect
};
MenuItemActionType code HotInputSelectKeyEvent[BTN_EndBTN] =
{
    MIA_Nothing,    // BTN_Plus
    MIA_Nothing,    // BTN_Minus
    MIA_ExecFunc,   // BTN_Menu
    MIA_GotoPrev,   // BTN_Exit
    MIA_Power,        // BTN_Power
    MIA_NextItem,   // BTN_SourceSelect
};
#else
MenuItemActionType code HotInputSelectKeyEvent[BTN_EndBTN] =
{
    MIA_Nothing,  // BTN_Plus
    MIA_Nothing,  // BTN_Minus
    MIA_ExecFunc,     // BTN_Menu
    MIA_NextItem,     // BTN_Exit
    MIA_Power,    // BTN_Power
    MIA_SourceSel, //2006-02-22
};
#endif
#endif

MenuItemActionType code FacExecNvramKeyEvent[BTN_EndBTN] = // 2011.9.13 22:30 CC
{
    MIA_NextItem,   // BTN_Plus
    MIA_PrevItem,   // BTN_Minus
    MIA_EXEC_FACTORY_NVRAMINIT,    	         	// BTN_MENU
    MIA_NextItem,    			// BTN_EXIT
    MIA_Power,      					// BTN_POWER
    MIA_Nothing					// BTN_BSEL
};

MenuItemActionType code ReturnEvent[BTN_EndBTN] =
{
    MIA_NextItem,   // BTN_Plus
    MIA_PrevItem,   // BTN_Minus
    MIA_GotoPrev,    // BTN_Menu
    MIA_GotoPrev,   // BTN_Exit
    MIA_Power,      // BTN_Power
    MIA_GotoPrev,
};
//===========================================================
MenuItemActionType code OORKeyEvent[BTN_EndBTN] =
{
    MIA_SourceSel,   // BTN_Plus
    MIA_Nothing, // BTN_Minus
    MIA_Nothing, // BTN_Menu
    MIA_Nothing, // BTN_Exit
    MIA_Power, // BTN_Power
    MIA_SourceSel,

};

//============================================================
MenuItemType code PowerOffMenuItems[] =
{
    {
        0, 1, // XPos, YPos;
        0, 1, // ForeColor, BackColor;
        0, 1, // SelForeColor, SelBackColor;
        PowerOnMenu, //NextMenuPage;
        DWI_CenterText,// DrawMenuItemType;
        NULL, // DisplayText;
        NothingKeyEvent,//PowerOffKeyEvent, //KeyEvent
        {
            NULL, // AdjustFunction
            NULL            // ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    }
};
MenuItemType code StandbyMenuItems[] =
{
    {
        0, 2, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        RootMenu, // NextMenuPage;
        DWI_CenterText,// DrawMenuItemType;
        NoSignalText, // DisplayText;
        NothingKeyEvent, //OORKeyEvent,// KeyEvent
        {
            NULL, // AdjustFunction
            NULL  // ExecFunction
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


MenuItemType code CableNotConnectedMenuItems[] =
{
    {
        0, 2, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        RootMenu, // NextMenuPage;
        DWI_CenterText,// DrawMenuItemType;
        CableNotConnectedText, // DisplayText;
        NothingKeyEvent,//PowerOffKeyEvent, //KeyEvent
        {
            NULL, // AdjustFunction
            NULL  // ExecFunction
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

MenuItemType code UnsupportedModeMenuItems[] =
{
    {
        0, 2, // XPos, YPos;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        CPC_White, CPC_Black, // ForeColor, BackColor;
        RootMenu, // NextMenuPage;
        DWI_CenterText,// DrawMenuItemType;
        OutofRangeText, // DisplayText;
        NothingKeyEvent, //OORKeyEvent,//PowerOffKeyEvent, //KeyEvent
        {
            NULL, // AdjustFunction
            NULL  // ExecFunction
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

MenuItemType code RootMenuItems[] =
{
    // virtual item
    {
        0, 1, // XPos, YPos;
        0, 1, // ForeColor, BackColor;
        0, 1, // SelForeColor, SelBackColor;
        MainMenu, //NextMenuPage;
#if 0 //Leo-temp
        NULL, //DWI_CenterText,// DrawMenuItemType;
#else
        DWI_None, //DWI_CenterText,// DrawMenuItemType;
#endif
        NULL, // DisplayText;
        RootKeyEvent,
        {
            NULL, // AdjustFunction
            NULL,//AutoConfig// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    }
};

//----------------------------------------------------------
RadioTextType code BurninRatioText[] =
{
    // Flags,   XPos,   YPos,   DisplayText
    {dwiEnd,    FactoryMenu_X1_Start, 3, BurninOnOffText}
};
DrawRadioGroupType code DrawBurninRadioGroup[] =
{
    //ForeColor,    BackColor,      GetValue    RadioText
    {CP_WhiteColor, CP_Background,  GetBurninValue, BurninRatioText}
};

NumberType code F_RedNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, FactoryMenu_X1_Start, 4, GetRColorWarm1Value},
};
DrawNumberType code DrawF_RedNumber[] =
{
    // ForeColor, BackColor,  GetValue
    {CP_WhiteColor, CP_Background,     F_RedNumber}
};
GaugeType code F_RedGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, 0, 4, GetRColorWarm1Value},
};
DrawGuageType code DrawF_RedGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {CP_RedColor,  CP_Background,    14,  F_RedGuage}
};
NumberType code F_GreenNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, FactoryMenu_X1_Start, 5, GetGColorWarm1Value},
};
DrawNumberType code DrawF_GreenNumber[] =
{
    // ForeColor, BackColor,  GetValue
    {CP_WhiteColor, CP_Background,     F_GreenNumber}
};
GaugeType code F_GreenGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, 0, 5, GetGColorWarm1Value},
};
DrawGuageType code DrawF_GreenGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {CP_GreenColor,  CP_Background,    14,  F_GreenGuage}
};
NumberType code F_BlueNumber[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {dwiEnd, FactoryMenu_X1_Start, 6, GetBColorWarm1Value},
};
DrawNumberType code DrawF_BlueNumber[] =
{
    // ForeColor, BackColor,  GetValue
    {CP_WhiteColor, CP_Background,     F_BlueNumber}
};
GaugeType code F_BlueGuage[] =
{
    // Flags, ForeColor,    BackColor,  Length,     GetValue
    {dwiEnd, 0, 6, GetBColorWarm1Value},
};
DrawGuageType code DrawF_BlueGuage[] =
{
    //  ForeColor,  BackColor,  Length,,    GetValue
    {CP_BlueColor,  CP_Background,    14,  F_BlueGuage}
};
NumberType code F_Bri_0Number[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {  dwiEnd, FactoryMenu_X3_Start, 3, GetFBrightness_0Value},
};
DrawNumberType code DrawF_Bri_0Number[] =
{
    // ForeColor, BackColor,  GetValue
    {CP_WhiteColor, CP_Background,     F_Bri_0Number}
};
NumberType code F_Bri_25Number[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {  dwiEnd, FactoryMenu_X3_Start, 4, GetFBrightness_25Value},
};
DrawNumberType code DrawF_Bri_25Number[] =
{
    // ForeColor, BackColor,  GetValue
    {CP_WhiteColor, CP_Background,     F_Bri_25Number}
};
NumberType code F_Bri_50Number[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {  dwiEnd, FactoryMenu_X3_Start, 5, GetFBrightness_50Value},
};
DrawNumberType code DrawF_Bri_50Number[] =
{
    // ForeColor, BackColor,  GetValue
    {CP_WhiteColor, CP_Background,     F_Bri_50Number}
};
NumberType code F_Bri_75Number[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {  dwiEnd, FactoryMenu_X3_Start, 6, GetFBrightness_75Value},
};
DrawNumberType code DrawF_Bri_75Number[] =
{
    // ForeColor, BackColor,  GetValue
    {CP_WhiteColor, CP_Background,     F_Bri_75Number}
};
NumberType code F_Bri_100Number[] =
{
    // Flags,   XPos,   YPos,   GetValue
    {  dwiEnd, FactoryMenu_X3_Start, 7, GetFBrightness_100Value},
};
DrawNumberType code DrawF_Bri_100Number[] =
{
    // ForeColor, BackColor,  GetValue
    {CP_WhiteColor, CP_Background,     F_Bri_100Number}
};

//====================================================================================

RadioTextType code FactoryText[] =
{
	// Flags,	XPos,	YPos,	DisplayText
	{0,    		1,  	1, 		F_PanelTypeText},
	{0,    		15,		1, 		F_PanelText},
	{0, 	   	1, 		2,  	F_FirmwareText},
	{dwiEnd,	15, 	2,  	F_FwPartNoText},
};

DrawRadioGroupType code DrawFactoryRadioGroup[] =
{
    //ForeColor,    BackColor,      GetValue    RadioText
    {CP_RedColor, CP_Background,   NULL,   FactoryText}
};

MenuItemType code FactoryMenuItems[] =
{
	//0 Burnin Adjust
	{
		1, 3, // XPos, YPos;
		CP_WhiteColor, CP_Background, // ForeColor, BackColor;
		CP_YellowColor, CP_Background, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_Text,// DrawMenuItemType;
		F_BurninText, // DisplayText;
		FactoryNaviExecKeyEvent,
		{
			NULL, 			// AdjustFunction
			AdjustBurnin 	// ExecFunction
		},// DisplayValue;
		{
			NULL, // DrawNumberType
			NULL, // DrawGuageType
			DrawBurninRadioGroup	  // DrawRadioGroupType
		},
		NULL, //Font
		mibSelectable // Flags
	},   
	// 1 F Red
	{
		1, 4, // XPos, YPos;
		CP_WhiteColor, CP_Background, // ForeColor, BackColor;
		CP_YellowColor, CP_Background, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_None,// DrawMenuItemType;
		NULL, // DisplayText;
		FactoryNaviKeyEvent,
		{
			NULL, // AdjustFunction
			NULL// ExecFunction
		},// DisplayValue;
		{
			NULL, // DrawNumberType
			DrawF_RedGuage, // DrawGuageType
			NULL  // DrawRadioGroupType
		},
		NULL, //Font
		mibSelectable // Flags
	},
	// 2 F Red_Adj
	{
		1, 4, // XPos, YPos;
		CP_WhiteColor, CP_Background, // ForeColor, BackColor;
		CP_YellowColor, CP_Background, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_None,// DrawMenuItemType;
		NULL, // DisplayText;
		FactoryAdjustStayKeyEvent,
		{
			AdjustRedColorWarm1, // AdjustFunction      
			NULL// ExecFunction
		},// DisplayValue;
		{
			DrawF_RedNumber, // DrawNumberType
			DrawF_RedGuage, // DrawGuageType
			NULL	// DrawRadioGroupType
		},
		NULL, //Font
		mibDrawValue|mibSelectable // Flags
	},
	// 3 F Green
	{
		1, 5, // XPos, YPos;
		CP_WhiteColor, CP_Background, // ForeColor, BackColor;
		CP_YellowColor, CP_Background, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_None,// DrawMenuItemType;
		NULL, // DisplayText;
		FactoryNaviKeyEvent,
		{
			NULL, // AdjustFunction
			NULL// ExecFunction
		},// DisplayValue;
		{
			NULL, // DrawNumberType
			DrawF_GreenGuage, // DrawGuageType
			NULL	  // DrawRadioGroupType
		},
		NULL, //Font
		mibSelectable // Flags
	},
	// 4 F Green_Adj
	{
		1, 5, // XPos, YPos;
		CP_WhiteColor, CP_Background, // ForeColor, BackColor;
		CP_YellowColor, CP_Background, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_None,// DrawMenuItemType;
		NULL, // DisplayText;
		FactoryAdjustStayKeyEvent,
		{
			AdjustGreenColorWarm1, // AdjustFunction
			NULL// ExecFunction
		},// DisplayValue;
		{
			DrawF_GreenNumber, // DrawNumberType
			DrawF_GreenGuage, // DrawGuageType
			NULL	  // DrawRadioGroupType
		},
		NULL, //Font
		mibDrawValue|mibSelectable // Flags
	},
	// 5 F Blue
	{
		1, 6, // XPos, YPos;
		CP_WhiteColor, CP_Background, // ForeColor, BackColor;
		CP_YellowColor, CP_Background, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_None,// DrawMenuItemType;
		NULL, // DisplayText;
		FactoryNaviKeyEvent,
		{
			NULL, // AdjustFunction
			NULL// ExecFunction
		},// DisplayValue;
		{
			NULL, // DrawNumberType
			DrawF_BlueGuage, // DrawGuageType
			NULL	  // DrawRadioGroupType
		},
		NULL, //Font
		mibSelectable // Flags
	},
	// 6 F Blue_Adj
	{
		1, 6, // XPos, YPos;
		CP_WhiteColor, CP_Background, // ForeColor, BackColor;
		CP_YellowColor, CP_Background, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_None,// DrawMenuItemType;
		NULL, // DisplayText;
		FactoryAdjustStayKeyEvent,
		{
			AdjustBlueColorWarm1, // AdjustFunction
			NULL// ExecFunction
		},// DisplayValue;
		{
			DrawF_BlueNumber, // DrawNumberType
			DrawF_BlueGuage, // DrawGuageType
			NULL	  // DrawRadioGroupType
		},
		NULL, //Font
		mibDrawValue|mibSelectable // Flags
	},
	// 7 Factory Reset
	{
		1, 7, // XPos, YPos;
		CP_WhiteColor, CP_Background, // ForeColor, BackColor;
		CP_YellowColor, CP_Background, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_Text,// DrawMenuItemType;
		F_FactoryResetText,// ExecFunction
		NaviExecKeyEvent,
		{
			NULL, // AdjustFunction
			FactoryReset // ExecFunction
		},// DisplayValue;
		{
			NULL, // DrawNumberType
			NULL, // DrawGuageType
			NULL	// DrawRadioGroupType
		},
		NULL, //Font
		mibSelectable // Flags
	},
	// 8 Curve_0	FactoryCurve_0_Start 
	{
		FactoryMenu_X2_Start, 3, // XPos, YPos;
		CP_WhiteColor, CP_Background, // ForeColor, BackColor;
		CP_YellowColor, CP_Background, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_Text,// DrawMenuItemType;
		F_Curve_0Text, // DisplayText;
		FactoryNaviKeyEvent,
		{
			NULL, // AdjustFunction
			NULL,// ExecFunction
		},// DisplayValue;
		{
			NULL, // DrawNumberType
			NULL, // DrawGuageType
			NULL  // DrawRadioGroupType
		},
		NULL, //Font
		mibSelectable // Flags
	},
	// 9 Curve_0 Adj
	{
		FactoryMenu_X2_Start, 3, // XPos, YPos;
		CP_WhiteColor, CP_Background, // ForeColor, BackColor;
		CP_YellowColor, CP_DeepBlue, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_Text,// DrawMenuItemType;
		F_Curve_0Text, // DisplayText;
		FactoryAdjustStayKeyEvent,
		{
			AdjustFBrightness_0, // AdjustFunction
			NULL // ExecFunction
		},// DisplayValue;
		{
			DrawF_Bri_0Number, // DrawNumberType
			NULL, // DrawGuageType
			NULL  // DrawRadioGroupType
		},
		NULL, //Font
		mibDrawValue|mibSelectable // Flags
	},
	// 10 Curve_25
	{
		FactoryMenu_X2_Start, 4, // XPos, YPos;
		CP_WhiteColor, CP_Background, // ForeColor, BackColor;
		CP_YellowColor, CP_Background, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_Text,// DrawMenuItemType;
		F_Curve_25Text, // DisplayText;
		FactoryNaviKeyEvent,
		{
			NULL, // AdjustFunction
			NULL,// ExecFunction
		},// DisplayValue;
		{
			NULL, // DrawNumberType
			NULL, // DrawGuageType
			NULL  // DrawRadioGroupType
		},
		NULL, //Font
		mibSelectable // Flags
	},
	// 11 Curve_25 Adj
	{
		FactoryMenu_X2_Start, 4, // XPos, YPos;
		CP_WhiteColor, CP_Background, // ForeColor, BackColor;
		CP_YellowColor, CP_DeepBlue, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_Text,// DrawMenuItemType;
		F_Curve_25Text, // DisplayText;
		FactoryAdjustStayKeyEvent,
		{
			AdjustFBrightness_25, // AdjustFunction
			NULL // ExecFunction
		},// DisplayValue;
		{
			DrawF_Bri_25Number, // DrawNumberType
			NULL, // DrawGuageType
			NULL  // DrawRadioGroupType
		},
		NULL, //Font
		mibDrawValue|mibSelectable // Flags
	},
	// 12 Curve_50
	{
		FactoryMenu_X2_Start, 5, // XPos, YPos;
		CP_WhiteColor, CP_Background, // ForeColor, BackColor;
		CP_YellowColor, CP_Background, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_Text,// DrawMenuItemType;
		F_Curve_50Text, // DisplayText;
		FactoryNaviKeyEvent,
		{
			NULL, // AdjustFunction
			NULL,// ExecFunction
		},// DisplayValue;
		{
			NULL, // DrawNumberType
			NULL, // DrawGuageType
			NULL  // DrawRadioGroupType
		},
		NULL, //Font
		mibSelectable // Flags
	},
	// 13 Curve_50 Adj
	{
		FactoryMenu_X2_Start, 5, // XPos, YPos;
		CP_WhiteColor, CP_Background, // ForeColor, BackColor;
		CP_YellowColor, CP_DeepBlue, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_Text,// DrawMenuItemType;
		F_Curve_50Text, // DisplayText;
		FactoryAdjustStayKeyEvent,
		{
			AdjustFBrightness_50, // AdjustFunction
			NULL // ExecFunction
		},// DisplayValue;
		{
			DrawF_Bri_50Number, // DrawNumberType
			NULL, // DrawGuageType
			NULL  // DrawRadioGroupType
		},
		NULL, //Font
		mibDrawValue|mibSelectable // Flags
	},
	// 14 Curve_75
	{
		FactoryMenu_X2_Start, 6, // XPos, YPos;
		CP_WhiteColor, CP_Background, // ForeColor, BackColor;
		CP_YellowColor, CP_Background, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_Text,// DrawMenuItemType;
		F_Curve_75Text, // DisplayText;
		FactoryNaviKeyEvent,
		{
			NULL, // AdjustFunction
			NULL,// ExecFunction
		},// DisplayValue;
		{
			NULL, // DrawNumberType
			NULL, // DrawGuageType
			NULL  // DrawRadioGroupType
		},
		NULL, //Font
		mibSelectable // Flags
	},
	// 15 Curve_75 Adj
	{
		FactoryMenu_X2_Start, 6, // XPos, YPos;
		CP_WhiteColor, CP_Background, // ForeColor, BackColor;
		CP_YellowColor, CP_DeepBlue, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_Text,// DrawMenuItemType;
		F_Curve_75Text, // DisplayText;
		FactoryAdjustStayKeyEvent,
		{
			AdjustFBrightness_75, // AdjustFunction
			NULL // ExecFunction
		},// DisplayValue;
		{
			DrawF_Bri_75Number, // DrawNumberType
			NULL, // DrawGuageType
			NULL  // DrawRadioGroupType
		},
		NULL, //Font
		mibDrawValue|mibSelectable // Flags
	},
	// 16 Curve_100
	{
		FactoryMenu_X2_Start, 7, // XPos, YPos;
		CP_WhiteColor, CP_Background, // ForeColor, BackColor;
		CP_YellowColor, CP_Background, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_Text,// DrawMenuItemType;
		F_Curve_100Text, // DisplayText;
		FactoryNaviKeyEvent,
		{
			NULL, // AdjustFunction
			NULL,// ExecFunction
		},// DisplayValue;
		{
			NULL, // DrawNumberType
			NULL, // DrawGuageType
			NULL  // DrawRadioGroupType
		},
		NULL, //Font
		mibSelectable // Flags
	},
	// 17 Curve_100 Adj
	{
		FactoryMenu_X2_Start, 7, // XPos, YPos;
		CP_WhiteColor, CP_Background, // ForeColor, BackColor;
		CP_YellowColor, CP_DeepBlue, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_Text,// DrawMenuItemType;
		F_Curve_100Text, // DisplayText;
		FactoryAdjustStayKeyEvent,
		{
			AdjustFBrightness_100, // AdjustFunction
			NULL,// ExecFunction
		},// DisplayValue;
		{
			DrawF_Bri_100Number, // DrawNumberType
			NULL, // DrawGuageType
			NULL  // DrawRadioGroupType
		},
		NULL, //Font
		mibDrawValue|mibSelectable // Flags
	},
	// 18 Exit
	{
		45, 3, // XPos, YPos;
		CP_WhiteColor, CP_Background, // ForeColor, BackColor;
		CP_YellowColor, CP_Background, // SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_Text,// DrawMenuItemType;
		F_ExitText, // DisplayText;
		FactoryExitKeyEvent,
		{
			NULL, // AdjustFunction
			NULL,// ExecFunction
		},// DisplayValue;
		{
			NULL, // DrawNumberType
			NULL, // DrawGuageType
			NULL  // DrawRadioGroupType
		},
		NULL, //Font
		mibSelectable // Flags
	},
	// Descript
	{
		20, 0, // XPos, YPos;
		CP_WhiteColor, CP_Background, // ForeColor, BackColor;
		CP_WhiteColor, CP_Background,// SelForeColor, SelBackColor;
		RootMenu, //NextMenuPage;
		DWI_Text,// DrawMenuItemType;
		F_FactoryDialogText, // DisplayText;
		FactoryNaviKeyEvent,
		{
			NULL,// AdjustFunction
			NULL// ExecFunction
		},// DisplayValue;
		{
			NULL,// DrawNumberType
			NULL, // DrawGuageType
			DrawFactoryRadioGroup//DrawRadioGroupType
		},
		NULL, //Font
		mibDrawValue // Flags
	},
};


