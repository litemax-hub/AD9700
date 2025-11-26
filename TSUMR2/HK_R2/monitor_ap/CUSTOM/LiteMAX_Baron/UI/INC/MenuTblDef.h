
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
    MIA_ExecFunc, // BTN_Menu
    MIA_JumpNaviItem,     // BTN_Exit
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


//==============================================================================================
NumberType code DrawFContrast[] =
{
    // Flags,  XPos,    YPos,   GetValue
    {dwiEnd, 11+4, 2+5, GetContrastValue}
};
DrawNumberType code DrawFContrastNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor, DrawFContrast}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawFBrightness[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd,    11+4, 3+5,  GetBrightnessValue}
};
DrawNumberType code DrawFBrightnessNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {CP_RedColor,   CP_BlueColor, DrawFBrightness}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawGainR[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd,    9+4,  2+5,  GetRedGainValue}
};
DrawNumberType code DrawGainRNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor, DrawGainR}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawGainG[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd,    16+4, 2+5,  GetGreenGainValue}
};
DrawNumberType code DrawGainGNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {CP_RedColor,   CP_BlueColor,   DrawGainG}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawGainB[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd,    23+4, 2+5,  GetBlueGainValue}
};
DrawNumberType code DrawGainBNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {CP_RedColor,   CP_BlueColor,   DrawGainB}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawOffsetR[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd, 9+4,  3+5,  GetAdcRedOffsetValue}
};
DrawNumberType code DrawOffsetRNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {CP_RedColor,   CP_BlueColor,   DrawOffsetR}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawOffsetG[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd,    16+4, 3+5,  GetAdcGreenOffsetValue}
};
DrawNumberType code DrawOffsetGNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor,   DrawOffsetG}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawOffsetB[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd,    23+4, 3+5,  GetAdcBlueOffsetValue}
};
DrawNumberType code DrawOffsetBNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {CP_RedColor,   CP_BlueColor,   DrawOffsetB}
};
//------------------------------------------------------------------------------------------------------------
//================================================================================================
//------------------------------------------------------------------------------------------------------------
NumberType code DrawRedColorCool1[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd,    9+4,  5+5,  GetRColorCool1Value}
};
DrawNumberType code DrawRedColorCool1Number[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {CP_RedColor,   CP_BlueColor,   DrawRedColorCool1}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawGreenColorCool1[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd,    16+4, 5+5,  GetGColorCool1Value}
};
DrawNumberType code DrawGreenColorCool1Number[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor,   DrawGreenColorCool1}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawBlueColorCool1[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd, 23+4, 5+5,  GetBColorCool1Value}
};
DrawNumberType code DrawBlueColorCool1Number[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor,   DrawBlueColorCool1}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawFContrastCool1[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd, 23+4, 6+5,  GetContrastCool1Value}
};
DrawNumberType code DrawFContrastCool1Number[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {CP_RedColor,   CP_BlueColor,    DrawFContrastCool1}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawFBrightnessCool1[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd, 9+4, 6+5,   GetBrightnessCool1Value}
};
DrawNumberType code DrawFBrightnessCool1Number[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor, DrawFBrightnessCool1}
};
//------------------------------------------------------------------------------------------------------------
//================================================================================================
//------------------------------------------------------------------------------------------------------------

#if 0 //Cool2

NumberType code DrawRedColorCool2[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd,    9,  8,  GetRColorCool2Value}
};
DrawNumberType code DrawRedColorCool2Number[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {CP_RedColor,   CP_BlueColor,   DrawRedColorCool2}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawGreenColorCool2[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd,    15, 8,  GetGColorCool2Value}
};
DrawNumberType code DrawGreenColorCool2Number[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor,   DrawGreenColorCool2}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawBlueColorCool2[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd, 21, 8,  GetBColorCool2Value}
};
DrawNumberType code DrawBlueColorCool2Number[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor,   DrawBlueColorCool2}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawFContrastCool2[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd, 21, 9,  GetContrastCool2Value}
};
DrawNumberType code DrawFContrastCool2Number[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {CP_RedColor,   CP_BlueColor,    DrawFContrastCool2}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawFBrightnessCool2[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd, 9, 9,   GetBrightnessCool2Value}
};
DrawNumberType code DrawFBrightnessCool2Number[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor, DrawFBrightnessCool2}
};
#endif //Cool2

//------------------------------------------------------------------------------------------------------------
//================================================================================================
//------------------------------------------------------------------------------------------------------------
NumberType code DrawRedColorWarm1[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd,    9+4,  11+3, GetRColorWarm1Value}
};
DrawNumberType code DrawRedColorWarm1Number[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {CP_RedColor,   CP_BlueColor,   DrawRedColorWarm1}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawGreenColorWarm1[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd,    16+4, 11+3, GetGColorWarm1Value}
};
DrawNumberType code DrawGreenColorWarm1Number[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor,   DrawGreenColorWarm1}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawBlueColorWarm1[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd, 23+4, 11+3, GetBColorWarm1Value}
};
DrawNumberType code DrawBlueColorWarm1Number[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor,   DrawBlueColorWarm1}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawFContrastWarm1[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd, 23+4, 12+3, GetContrastWarm1Value}
};
DrawNumberType code DrawFContrastWarm1Number[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {CP_RedColor,   CP_BlueColor,    DrawFContrastWarm1}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawFBrightnessWarm1[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd, 9+4, 12+3,  GetBrightnessWarm1Value}
};
DrawNumberType code DrawFBrightnessWarm1Number[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor, DrawFBrightnessWarm1}
};
//------------------------------------------------------------------------------------------------------------
//================================================================================================
//------------------------------------------------------------------------------------------------------------
#if 0//Warm2

NumberType code DrawRedColorWarm2[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd,    9,  14, GetRColorWarm2Value}
};
DrawNumberType code DrawRedColorWarm2Number[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {CP_RedColor,   CP_BlueColor,   DrawRedColorWarm2}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawGreenColorWarm2[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd,    15, 14, GetGColorWarm2Value}
};
DrawNumberType code DrawGreenColorWarm2Number[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor,   DrawGreenColorWarm2}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawBlueColorWarm2[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd, 21, 14, GetBColorWarm2Value}
};
DrawNumberType code DrawBlueColorWarm2Number[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor,   DrawBlueColorWarm2}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawFContrastWarm2[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd, 21, 15, GetContrastWarm2Value}
};
DrawNumberType code DrawFContrastWarm2Number[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {CP_RedColor,   CP_BlueColor,    DrawFContrastWarm2}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawFBrightnessWarm2[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd, 9, 15,  GetBrightnessWarm2Value}
};
DrawNumberType code DrawFBrightnessWarm2Number[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor, DrawFBrightnessWarm2}
};
#endif //Warm2
//------------------------------------------------------------------------------------------------------------
//================================================================================================
//------------------------------------------------------------------------------------------------------------
NumberType code DrawRedColorNormal[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd,    9+4,  8+4,  GetRColorNormalValue}
};
DrawNumberType code DrawRedColorNormalNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {CP_RedColor,   CP_BlueColor,   DrawRedColorNormal}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawGreenColorNormal[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd,    16+4, 8+4,  GetGColorNormalValue}
};
DrawNumberType code DrawGreenColorNormalNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor,   DrawGreenColorNormal}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawBlueColorNormal[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd, 23+4, 8+4,  GetBColorNormalValue}
};
DrawNumberType code DrawBlueColorNormalNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor,   DrawBlueColorNormal}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawFContrastNormal[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd, 23+4, 9+4,  GetContrastNormalValue}
};
DrawNumberType code DrawFContrastNormalNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {CP_RedColor,   CP_BlueColor,    DrawFContrastNormal}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawFBrightnessNormal[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd, 9+4, 9+4,   GetBrightnessNormalValue}
};
DrawNumberType code DrawFBrightnessNormalNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor, DrawFBrightnessNormal}
};
//------------------------------------------------------------------------------------------------------------

//================================================================================================
//====================================================================================
//------------------------------------------------------------------------------------------------------------
NumberType code DrawRedColorSRGB[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd,    9+4,  14+2, GetRColorSRGBValue}
};
DrawNumberType code DrawRedColorSRGBNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor, DrawRedColorSRGB}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawGreenColorSRGB[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd,        16+4, 14+2, GetGColorSRGBValue}
};
DrawNumberType code DrawGreenColorSRGBNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor,  DrawGreenColorSRGB}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawBlueColorSRGB[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd,    23+4, 14+2, GetBColorSRGBValue}
};
DrawNumberType code DrawBlueColorSRGBNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor,   DrawBlueColorSRGB}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawFContrastSRGB[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd, 23+4, 15+2, GetContrastSRGBValue}
};
DrawNumberType code DrawFContrastSRGBNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor,       DrawFContrastSRGB}
};
//------------------------------------------------------------------------------------------------------------
NumberType code DrawFBrightnessSRGB[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd,    9+4, 15+2,  GetBrightnessSRGBValue}
};
DrawNumberType code DrawFBrightnessSRGBNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor,   DrawFBrightnessSRGB}
};
//------------------------------------------------------------------------------------------------------------

//====================================================================================

RadioTextType code BankNoText[] =
{
    // Flags,   XPos,   YPos,   DisplayText
    {dwiEnd,        6,  19, BankNameText}
};
DrawRadioGroupType code DrawBankNoRadioGroup[] =
{
    //ForeColor,    BackColor,      GetValue    RadioText
    {CP_RedColor, CP_BlueColor, NULL,   BankNoText}
};
//---------------------------------------------------------
NumberType code DrawAddr[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiHex | dwiEnd,  6, 20, GetAddrValue}
};
DrawNumberType code DrawAddrNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor,     DrawAddr}
};
//----------------------------------------------------------
NumberType code DrawAddrRegValue[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiHex | dwiEnd,     17, 20, GetAddrRegValue}
};
DrawNumberType code DrawAddrRegValueNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor,    DrawAddrRegValue}
};
//----------------------------------------------------------
NumberType code DrawRegValue[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiHex | dwiEnd, 17,  20, GetRegValue}
};
DrawNumberType code DrawRegValueNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {CP_RedColor,   CP_BlueColor, DrawRegValue}
};


//----------------------------------------------------------
RadioTextType code BurninRatioText[] =
{
    // Flags,   XPos,   YPos,   DisplayText
    // {dwiEnd, 10, 12, OffText},
    {dwiEnd,    13, 25, BurninOnOffText}
};
DrawRadioGroupType code DrawBurninRadioGroup[] =
{
    //ForeColor,    BackColor,      GetValue    RadioText
    {CP_RedColor, CP_BlueColor,  GetBurninValue, BurninRatioText}
};

RadioTextType code DDCWPRatioText[] =
{
    // Flags,   XPos,   YPos,   DisplayText
    // {dwiEnd, 10, 12, OffText},
    {dwiEnd,    13, 26, DDCWPOnOffText}
};
DrawRadioGroupType code DrawDDCWPRadioGroup[] =
{
    //ForeColor,    BackColor,      GetValue    RadioText
    {CP_RedColor, CP_BlueColor,  GetDDCWPActiveValue, DDCWPRatioText}
};
RadioTextType code ProductionRatioText[] =
{
    // Flags,   XPos,   YPos,   DisplayText
    // {dwiEnd, 10, 12, OffText},
    {dwiEnd,    13, 24, ProductionOnOffText}
};
DrawRadioGroupType code DrawProductionRadioGroup[] =
{
    //ForeColor,    BackColor,      GetValue    RadioText
    {CP_RedColor, CP_BlueColor,  GetProductionValue, ProductionRatioText}
};
#if 0//ENABLE_RTE
RadioTextType code OverDriveRatioText[] =
{
    // Flags,   XPos,   YPos,   DisplayText
    // {dwiEnd, 10, 12, OffText},
    {dwiEnd,    13, 27, OverDriveOnOffText}
};
DrawRadioGroupType code DrawOverDriveRadioGroup[] =
{
    //ForeColor,    BackColor,      GetValue    RadioText
    {CP_RedColor, CP_BlueColor,  GetOverDriveValue, OverDriveRatioText}
};
#endif
// 090310
#if PanelRSDS
NumberType code DrawVCOMValue[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiHex | dwiEnd, 10, 28, GetVCOMValue}
};
DrawNumberType code DrawVCOMValueNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {CP_RedColor,   CP_BlueColor, DrawVCOMValue}
};
#endif

//====================================================================================

RadioTextType code FactoryText[] =
{
    // Flags,   XPos,   YPos,   DisplayText
    { 0,        23+1+4, 0,  VersionDateText},
    {0,         14+1+4, 0,  ChangeDateText},
    {0,         1, 0,  ModelNameText},
    {0,         1, 1,  ChipNameText},
    { 0,        1,  2+5,  AdcGainText},
    { 0,        1,  3+5,  AdcOffsetText},
    { 0,        1,  5+5,  COOL1Text},
    { 0,        1,  8+4,      NORMALText},
    { 0,        1,  11+3,  WARM1Text},
    {0,         1, 14+2,     sRGBText},
	{ 0,         1, 2,     BacklightHoursText},
	//{ 0,              22, 3,     PanelText},
    {0,              1, 3,     PanelOnText},

    {dwiEnd,    14,  3, PanelText},
};
#if USEFLASH
RadioTextType code NvramOffText[] =
{
    {dwiEnd,    13,  29, OffText},
};
DrawRadioGroupType code NvramInitItemInfoMenuGroup[] =
{
    //ForeColor,    BackColor,      GetValue    RadioText
    {CP_RedColor, CP_BlueColor,   NULL,   NvramOffText}
};
#endif

DrawRadioGroupType code DrawFactoryRadioGroup[] =
{
    //ForeColor,    BackColor,      GetValue    RadioText
    {CP_WhiteColor, CP_BlueColor,   NULL,   FactoryText}
};
NumberType code FactoryPanelOnTimeNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd, 23+4, 2, GetPanelOnTimeValue}
};
DrawNumberType code DrawFactoryPanelOnTimeNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor,       FactoryPanelOnTimeNumber}
};
#if ENABLE_FACTORY_SSCADJ
NumberType code FactorySSCModulationNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd, 4, 22, GetFactorySSCModulationValue}
};
DrawNumberType code DrawFactorySSCModulationNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor,       FactorySSCModulationNumber}
};
NumberType code FactorySSCPercentageNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {dwiEnd, 13, 22, GetFactorySSCPercentageValue}
};
DrawNumberType code DrawFactorySSCPercentageNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor,       FactorySSCPercentageNumber}
};
#endif
/*
RadioTextType code FSpreadSpectrumText[] =
{
    // Flags,   XPos,   YPos,   DisplayText
	{ 0,         1, 22,     FreqText},
	{ 0,         11, 22,     AmpText},
	{ dwiEnd,  1, 21,     SpreadSpectrumText},

};

DrawRadioGroupType code DrawSpreadSpectrumRadioGroup[] =
{
    //ForeColor,    BackColor,      GetValue    RadioText
    {CP_WhiteColor, CP_BlueColor,   NULL,   FSpreadSpectrumText}
};

NumberType code SpreadSpectrumNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {0		, 2, 22, GetSSCStepValue}    ,
    {dwiEnd, 11, 22, GetSSCSpanValue}
};

DrawNumberType code DrawSpreadSpectrumNumber[] =
{
    // Flags, ForeColor,    BackColor,  XPos,   YPos,   GetValue
    {   CP_RedColor,    CP_BlueColor,       SpreadSpectrumNumber}
};
*/
MenuItemType code FactoryMenuItems[] =
{
    // Exit
    {
        1, 31, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FExitText, // DisplayText;
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

    // 0.Auto Color
    {
        1, 6, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        AutoColorText, // DisplayText;
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            AutoColor// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },

    // 1. Gain R
    {
        8+4, 7, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryRedText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 2.Gain R adjust
    {
        10+4, 7, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustRedGain,  // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawGainRNumber,    // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 3.Gain G
    {
        15+4, 7, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryGreenText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable// Flags
    },
    // 4.Gain G adjust
    {
        17+4, 7, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustGreenGain,    // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawGainGNumber,    // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 5.Gain B
    {
        22+4, 7, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryBlueText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 6.Gain B adjust
    {
        24+4, 7, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustBlueGain, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawGainBNumber,    // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 7.Offset R
    {
        8+4, 8, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryRedText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 8.Offset R adjust
    {
        10+4, 8, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustAdcRedOffset, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawOffsetRNumber,  // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 9.Offset G
    {
        15+4, 8, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryGreenText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            AutoColor// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 10.Offset G adjust
    {
        17+4, 8, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustAdcGreenOffset, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawOffsetGNumber,  // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 11.Offset B
    {
        22+4, 8, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryBlueText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 12.Offset B adjust
    {
        24+4, 8, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustAdcBlueOffset,    // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawOffsetBNumber,  // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },


#if 1

    //===Cool1===============================================
    // 13.Cool1_ R
    {
        8+4, 10, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryRedText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 14.Cool1_R adjust
    {
        10+4, 10, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustRedColorCool1,    // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawRedColorCool1Number,    // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 15.Cool_G
    {
        15+4, 10, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryGreenText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 16.Cool_G adjust
    {
        17+4, 10, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustGreenColorCool1,  // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawGreenColorCool1Number,  // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 17.Cool1 B
    {
        22+4, 10, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryBlueText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },

    // 18.Cool1 B adjust
    {
        24+4, 10, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustBlueColorCool1, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawBlueColorCool1Number, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 19.C2- Cool1 Brightness
    {
        1, 11, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryBrightnessText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 20.C2- Cool1 Brightness adjust
    {
        10, 11, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustBrightnessCool1, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawFBrightnessCool1Number,   // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 21.C2- Cool1 Contrast
    {
        15+4, 11, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryContrastText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 22.C2-Cool1 Contrast adjust
    {
        23+4, 11, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustContrastCool1,   // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawFContrastCool1Number,  // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    //==========Cool2====================================

#if 0

    // 23.Cool2_ R
    {
        8, 8, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryRedText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 24.Cool2_R adjust
    {
        10, 8, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustRedColorCool2,    // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawRedColorCool2Number,    // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 25.Cool_G
    {
        14, 8, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryGreenText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 26.Cool_G adjust
    {
        16, 8, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustGreenColorCool2,  // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawGreenColorCool2Number,  // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 27.Cool2 B
    {
        20, 8, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryBlueText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },

    // 28.Cool2 B adjust
    {
        20, 8, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustBlueColorCool2, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawBlueColorCool2Number, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 29.C2- Cool2 Brightness
    {
        1, 9, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryBrightnessText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 30.C2- Cool2 Brightness adjust
    {
        10, 9, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustBrightnessCool2, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawFBrightnessCool2Number,   // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    //31.C2- Cool2 Contrast
    {
        14, 9, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryContrastText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 32.C2-Cool2 Contrast adjust
    {
        22, 9, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustContrastCool2,   // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawFContrastCool2Number,  // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },

#endif

    //==========Normal====================================
    // 53.Normal_ R
    {
        8+4, 12, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryRedText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 54.Normal_R adjust
    {
        10+4, 12, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustRedColorNormal,   // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawRedColorNormalNumber,   // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 55.Normal_G
    {
        15+4, 12, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryGreenText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 56.Normal_G adjust
    {
        17+4, 12, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustGreenColorNormal, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawGreenColorNormalNumber, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 57.Normal B
    {
        22+4, 12, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryBlueText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },

    // 58.Normal B adjust
    {
        24+4, 12, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustBlueColorNormal, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawBlueColorNormalNumber, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 59.C2- Normal Brightness
    {
        1, 13, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryBrightnessText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 60.C2- Normal Brightness adjust
    {
        10+4, 13, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustBrightnessNormal, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawFBrightnessNormalNumber,      // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 61.C2- Normal Contrast
    {
        15+4, 13, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryContrastText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 62.C2-Normal Contrast adjust
    {
        23+4, 13, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustContrastNormal,   // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawFContrastNormalNumber,  // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },

    //==========Warm1====================================
    // 33.Warm1_ R
    {
        8+4, 14, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryRedText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 34.Warm1_R adjust
    {
        10+4, 14, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustRedColorWarm1,    // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawRedColorWarm1Number,    // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 35.Cool_G
    {
        15+4, 14, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryGreenText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 36.Cool_G adjust
    {
        17+4, 14, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustGreenColorWarm1,  // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawGreenColorWarm1Number,  // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 37.Warm1 B
    {
        22+4, 14, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryBlueText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },

    // 38.Warm1 B adjust
    {
        24+4, 14, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustBlueColorWarm1, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawBlueColorWarm1Number, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 39.C2- Warm1 Brightness
    {
        1, 15, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryBrightnessText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 40.C2- Warm1 Brightness adjust
    {
        10+4, 15, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustBrightnessWarm1, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawFBrightnessWarm1Number,   // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 41.C2- Warm1 Contrast
    {
        15+4, 15, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryContrastText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 42.C2-Warm1 Contrast adjust
    {
        23+4, 15, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustContrastWarm1,   // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawFContrastWarm1Number,  // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    //==========Warm2====================================


#if 0
    // 43.Warm2_ R
    {
        8, 14, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryRedText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 44.Warm2_R adjust
    {
        10, 14, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustRedColorWarm2,    // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawRedColorWarm2Number,    // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 45.Cool_G
    {
        14, 14, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryGreenText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 46.Cool_G adjust
    {
        16, 14, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustGreenColorWarm2,  // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawGreenColorWarm2Number,  // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 47.Warm2 B
    {
        20, 14, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryBlueText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },

    // 48.Warm2 B adjust
    {
        20, 14, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustBlueColorWarm2, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawBlueColorWarm2Number, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 49.C2- Warm2 Brightness
    {
        1, 15, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryBrightnessText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 50.C2- Warm2 Brightness adjust
    {
        10, 15, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustBrightnessWarm2, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawFBrightnessWarm2Number,   // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 51.C2- Warm2 Contrast
    {
        14, 15, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryContrastText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 52.C2-Warm2 Contrast adjust
    {
        22, 15, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustContrastWarm2,   // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawFContrastWarm2Number,  // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },

#endif
#if 0 // move upper place
    //==========Normal====================================
    // 53.Normal_ R
    {
        8, 11, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryRedText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 54.Normal_R adjust
    {
        10, 11, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustRedColorNormal,   // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawRedColorNormalNumber,   // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 55.Cool_G
    {
        14, 11, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryGreenText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 56.Cool_G adjust
    {
        16, 11, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustGreenColorNormal, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawGreenColorNormalNumber, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 57.Normal B
    {
        20, 11, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryBlueText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },

    // 58.Normal B adjust
    {
        20, 11, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustBlueColorNormal, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawBlueColorNormalNumber, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 59.C2- Normal Brightness
    {
        1, 12, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryBrightnessText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 60.C2- Normal Brightness adjust
    {
        10, 12, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustBrightnessNormal, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawFBrightnessNormalNumber,      // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 61.C2- Normal Contrast
    {
        14, 12, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryContrastText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 62.C2-Normal Contrast adjust
    {
        22, 12, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustContrastNormal,   // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawFContrastNormalNumber,  // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
#endif
    //========================================================================================================
    //63. SRGB R
    {
        8+4, 16, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryRedText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 64.SRGB R adjust
    {
        10+4, 16, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustRedColorSRGB, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawRedColorSRGBNumber, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 65.SRGB G
    {
        15+4, 16, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryGreenText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },

    // 66.SRGB G adjust
    {
        17+4, 16, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustGreenColorSRGB, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawGreenColorSRGBNumber, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 67.SRGB B
    {
        22+4, 16, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryBlueText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 68.SRGB B adjust
    {
        24+4, 16, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustBlueColorSRGB, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawBlueColorSRGBNumber, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 69.Brightness
    {
        1, 17, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryBrightnessText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 70.SRGB Brightness adjust
    {
        10+4, 17, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustBrightnessSRGB, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawFBrightnessSRGBNumber,   // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // 71.SRGB Contrast
    {
        15+4, 17, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FactoryContrastText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    //72.SRGB Contrast adjust
    {
        23+4, 17, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustContrastSRGB,   // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawFContrastSRGBNumber,  // DrawNumberType
            NULL, // DrawGuageType
            NULL  // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // Bank
    {
        1, 19, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        BankText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // Bank Adjust
    {
        2, 19, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustBankNo, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawBankNoRadioGroup// DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // Address
    {
        1,  20, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        AddressText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // Address Adjust
    {
        1,  20, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustAddr, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawAddrNumber, // DrawNumberType
            NULL, // DrawGuageType
            NULL// DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    // ValueText
    {
        11,  20, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        ValueText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // ValueText Adjust
    {
        11,  20, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustStayKeyEvent,
        {
            AdjustRegValue, // AdjustFunction
            WriteRegValue// ExecFunction
        },// DisplayValue;
        {
            DrawRegValueNumber, // DrawNumberType
            NULL, // DrawGuageType
            NULL// DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    /*
	//spread spectrum
	{
        1, 21, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_WhiteColor, CP_BlueColor,// SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            NULL,// AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawSpreadSpectrumNumber,// DrawNumberType
            NULL, // DrawGuageType
            DrawSpreadSpectrumRadioGroup//DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue // Flags
    },
			*/
#if ENABLE_FACTORY_SSCADJ
	//spread spectrum
	{
        1, 21, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_WhiteColor, CP_BlueColor,// SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        SpreadSpectrumText, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            NULL,// AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL,// DrawNumberType
            NULL, // DrawGuageType
            NULL,//DrawSpreadSpectrumRadioGroup//DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue // Flags
    },
	    //73 FreqText
    {
        1, 22, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        FreqText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // Freq Adjust
    {
        6, 22, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustFactorySSCModulation, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawFactorySSCModulationNumber, // DrawNumberType
            NULL, // DrawGuageType
            NULL// DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
        //73 AmpText
    {
        11, 22, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        AmpText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // Amp Adjust
    {
        15, 22, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustFactorySSCPercentage, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawFactorySSCPercentageNumber, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    #endif
    //Production
    {
        1, 24, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        ProductionText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // Production Adjust
    {
        1, 24, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustProduction,// AdjustFunction
            NULL,// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawProductionRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    //73 Burnin
    {
        1, 25, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        BurninText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 74Burnin Adjust
    {
        1, 25, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustBurnin, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawBurninRadioGroup    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
    //73 DDC (EDID)
    {
        1, 26, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        EDIDWPText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 74DDC(EDID) Adjust
    {
        10, 26, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustDDCWP, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawDDCWPRadioGroup    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
#if 0//ENABLE_RTE
    {
        1, 27, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        OverDriveText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // 74Burnin Adjust
    {
        10, 27, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustOverDrive,// AdjustFunction
            NULL,// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            DrawOverDriveRadioGroup // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
#endif
    // 090310
#if  PanelRSDS
    {
        1, 28, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        VCOMText, // DisplayText;
        FactoryNaviKeyEvent,
        {
            NULL, // AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    // VCOM Adjust
    {
        10, 28, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            AdjustVCOMValue,// AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawVCOMValueNumber, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue | mibSelectable // Flags
    },
#endif
    //==============================================================================================


#if !USEFLASH
    {
        14, 27, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        EraseEEPROMText,// ExecFunction
        NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
            EraseAllEEPROMCell// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NULL    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
 #else
    {
        1, 29, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        EraseNVRamText,// ExecFunction
        FacExecNvramKeyEvent,//NaviExecKeyEvent,
        {
            NULL, // AdjustFunction
          	 EraseFlashUserData,//EraseAllEEPROMCell,//// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NvramInitItemInfoMenuGroup    // DrawRadioGroupType
        },
        NULL, //Font
        mibSelectable // Flags
    },
    /*
    {
        1, 29, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_RedColor, CP_BlueColor, // SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        EraseNVRamText,// ExecFunction
        FacExecNvramKeyEvent,
        {
            NULL, // AdjustFunction
          	 EraseFlashUserData,//EraseAllEEPROMCell,//// ExecFunction
        },// DisplayValue;
        {
            NULL, // DrawNumberType
            NULL, // DrawGuageType
            NvramInitItemInfoMenuGroup    // DrawRadioGroupType
        },
        NULL, //Font
        0 // Flags
    },
    */
#endif


    // Descript
    {
        20, 15, // XPos, YPos;
        CP_WhiteColor, CP_BlueColor, // ForeColor, BackColor;
        CP_WhiteColor, CP_BlueColor,// SelForeColor, SelBackColor;
        RootMenu, //NextMenuPage;
        DWI_Text,// DrawMenuItemType;
        NULL, // DisplayText;
        FactoryAdjustKeyEvent,
        {
            NULL,// AdjustFunction
            NULL// ExecFunction
        },// DisplayValue;
        {
            DrawFactoryPanelOnTimeNumber,// DrawNumberType
            NULL, // DrawGuageType
            DrawFactoryRadioGroup//DrawRadioGroupType
        },
        NULL, //Font
        mibDrawValue // Flags
    },
#endif
};







