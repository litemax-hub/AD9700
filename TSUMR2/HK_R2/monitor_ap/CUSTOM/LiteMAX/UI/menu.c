#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "board.h"
#include "Global.h"
#include "Keypaddef.h"
#include "ColorPalette.h"
#include "menudef.h"
#include "Keypad.h"
#include "Ms_rwreg.h"
#include "drvOSD.h"
#include "msOSD.h"
#include "mStar.h"
#include "menufunc.h"
#include "MenuStrProp.h"
#include "MenuTbl.h"
#include "ms_reg.h"
#include "Debug.h" //change position eshin
#include "misc.h"
#include "NVRam.h"
#include "Adjust.h"
#include "UserPref.h"
#include "Power.h"
#include "MsDLC.h"
#include "Detect.h"
//#include "PropFont.h"
#include "msflash.h"
#include "DDC.h"
#include "Mcu.h"
#include "Common.h"
#include "LoadCommonFont.h"
//#include "msid_v1.h"  // Rex 100706
#include "GPIO_DEF.h"
#include "LoadPropFont1218.h"
#include "MenuStrFunc.h"
//#include "halRwreg.h"
#if ENABLE_DPS
#include "msDPS_Setting.h"
#endif
#define CurrentMenu                 tblMenus[MenuPageIndex]
#define PrevMenu                    tblMenus[PrevMenuPageIndex]
#define NextMenuPage                CurrentMenuItem.NextPage
#define PrevMenuPage                CurrentMenu.PrevPage
#define CurrentMenuItems            CurrentMenu.MenuItems
#define PrevMenuItems               PrevMenu.MenuItems
#define MenuItemCount               CurrentMenu.ItemCount
#define CurrentMenuItem             CurrentMenu.MenuItems[MenuItemIndex]

#define CurrentMenuItemFunc     CurrentMenuItem.KeyFunction
#define KeyEvent                    CurrentMenuItem.KeyEvents[KeypadButton]


//========================================================================
BYTE xdata MenuPageIndex = 0;
BYTE xdata MenuItemIndex = 0;
BYTE xdata PrevMenuPageIndex = 0;
BYTE xdata PrevMenuItemIndex = 0;
BYTE xdata TurboKeyCounter = 0;
BYTE xdata moveX = 50;
BYTE xdata moveY = 50;

BYTE xdata ucClrItem;
//========================================================================
// 2006/11/10 10:0PM by KK move to userpref.c void SaveUserPrefSetting( BYTE menuPageIndex );
BYTE GetMenuItemIndex( BYTE menuPageIndex );
BYTE GetPrevItem( const MenuItemType *menuItem );
BYTE GetNextItem( const MenuItemType *menuItem );
Bool ExecuteKeyEvent( MenuItemActionType menuAction );
void DrawOsdMenu( void );
void DrawOsdMenuItem( BYTE drawIndex, const MenuItemType *menuItem );
void DrawOsdMenuItemText( BYTE itemIndex, const MenuItemType *menuItem );
void DrawOsdMenuItemValue( BYTE itemIndex, const DrawValueType *valueItem );
void DrawOsdMenuItemNumber( BYTE itemIndex, const DrawNumberType *numberItem );
void DrawOsdMenuItemGuage( BYTE itemIndex, const DrawGuageType *guageItem );
void DrawOsdMenuItemRadioGroup( BYTE itemIndex, const DrawRadioGroupType *RadioItem );
// 2006/11/10 10:0PM by KK move to userpref.c void SaveFactorySetting( BYTE itemIndex );
void DrawLogo( void );
void DrawOsdBackGround( void );
Bool DrawTimingInfo( void );
void DynamicLoadFont( const MenuFontType *menuFonts, BYTE addr );
Bool Menu_GetCableDetect(BYTE u8Inputport);
void Menu_RxCableStatePollingHandler(void);
//=========================================================================
void Menu_InitVariable( void )
{
//    BYTE i=0;

    MenuPageIndex = RootMenu;
    MenuItemIndex = 0;
    OsdCounter = 0;
    if( !PowerOnFlag )
    {
        MenuPageIndex = PowerOffMenu;
    }

/*
    if(i)
    {
        MenuFuncuncall();
    }
*/
}

void Menu_InitAction( void )
{
    MenuItemIndex = 0;
    if( DisplayLogoFlag )
    {
        //printMsg("displaymode");
        if( MenuPageIndex == LogoMenu )
        {
            return ;
        }
        MenuPageIndex = LogoMenu;
        //printMsg("displaymode_1");
    }
#if DISABLE_DPMS
    else if( SARwakeupFlag || FakeSleepFlag)

#else
    else if( SARwakeupFlag )
#endif
    {
        MenuPageIndex = HotInputSelectMenu;
		OsdCounter=10;
        Clr_SARwakeupFlag();
        if(SyncLossState())
            Set_BackToStandbyFlag();
    }
    else if( UnsupportedModeFlag )
    {
        MenuPageIndex = UnsupportedModeMenu;
        if( OsdLockModeFlag )
        {
            // Clr_OsdLockModeFlag();
            // Clr_ShowOsdLockFlag();
            // NVRam_WriteByte(nvrMonitorAddr(MonitorFlag), MonitorFlags);
        }
    }
    else if( SyncLossState() )
    {
        if(0)//( CableNotConnectedFlag ) //&&!ProductModeFlag)
        {
            if( DoBurninModeFlag )                // 2006/10/18 9:47PM by Emily test
            {
                Power_TurnOnAmberLed();
                MenuPageIndex = BurninMenu;
            }
            else
            {
                #if INPUT_TYPE!=INPUT_1A
                MenuPageIndex = StandbyMenu;
                #else
                if( ProductModeFlag )
                    MenuPageIndex = StandbyMenu;
                else
                {
                    MenuPageIndex = CableNotConnectedMenu;
                }
                #endif
            }
        }
        else
        {
            if(UserPrefPowerSavingEn == PowerSavingMenuItems_On)
            {
#if DISABLE_DPMS
            if(MenuPageIndex == HotInputSelectMenu)
            {
                Set_BackToStandbyFlag();
                return;
            }
            else            
#endif
                MenuPageIndex = StandbyMenu;
            }
            else
            {
                return;
            }

        }
        // 2006/9/29 3:48PM by Emily
        OsdCounter = 2;
    }
    else if( OsdLockModeFlag && ShowOsdLockFlag && !FactoryModeFlag )
    {
        MenuPageIndex = OsdLockMenu;
        Clr_ShowOsdLockFlag();
    }
    //  else if (UnsupportedModeFlag)
    //      MenuPageIndex=UnsupportedModeMenu;
    /******For PE Request*********
    else if (FactoryModeFlag)
    {
        MenuPageIndex = AutoColorMenu;
    }
    ****************************/
#if (ENABLE_VGA_INPUT)
    else if( UserPrefAutoTimes == 0 && CURRENT_INPUT_IS_VGA() && !ProductModeFlag )
    {
        MenuPageIndex = AutoMenu;
    }
#endif
    else if( ShowInputInfoFlag)// && !ProductModeFlag )
    {
        //printData(" ShowInputInfoFlag= %d", ShowInputInfoFlag);
        MenuPageIndex = InputInfoMenu;
        OsdCounter = 5; //10;
        Clr_ShowInputInfoFlag();
        //          DrawOsdMenu();
        //          Delay1ms(300); //(500);
    }
    else   // 100804 move to after source wake up
    {
        MenuPageIndex = RootMenu;
    }

    if(( UserPrefInputType != SrcInputType ) && ( !SyncLossState() ) )
    {
#if !USEFLASH
        NVRam_WriteByte( nvrMonitorAddr( InputType ), SrcInputType );
#else
        //SaveMonitorSetting();
        UserPref_EnableFlashSaveBit( bFlashSaveMonitorBit );
#endif

        UserPrefInputType = SrcInputType;
    }
    ExecuteKeyEvent( MIA_RedrawMenu );
    if( UnsupportedModeFlag )
    {
        if( !FreeRunModeFlag && CURRENT_INPUT_IS_VGA() && !ProductModeFlag )
        {
            if( UserPrefAutoTimes == 0 )
            {
                AutoConfig();
            }
        }
    }
}
void Menu_OsdHandler( void )
{
    BYTE menuAction = MIA_Nothing;

#if ENABLE_TOUCH_PANEL

    // dummy call
    if(menuAction != MIA_Nothing)
    {
        Key_ScanKeypad();
        ResetOsdTimer();
    }

/*
    if(InputTimingStableFlag && !SyncLossState() && !TPL_PollingCounter)
    {

#if 0 // just for test
        if(TPL_InitPassFlag == INIT_STATUS_FAIL)
        {
            TPL_InitSetting();
        }
#endif

        if(TPL_ReadSurface() == TRUE)
        {
            ParseFingerStatus();
            if(TPL_FingerStatus >= FINGER_STSTUS_MAX)
            {
                menuAction = MIA_Nothing;
            }
            else if( CurrentMenuItem.KeyEvents )
            {
                menuAction = *(( CurrentMenuItem.KeyEvents ) + TPL_FingerStatus );
            }

            if(TPL_FingerStatus != FINGER_LIFT)
                ResetOsdTimer();
        }
        TPL_PollingCounter = SPI_POLLING_INTERVAL;
    }
*/

#else

    if( StartScanKeyFlag )
    {
        Key_ScanKeypad();
        Clr_StartScanKeyFlag();
        TPDebunceCounter = SKPollingInterval;
    }

    if( bKeyReadyFlag )
    {
        Clr_bKeyReadyFlag();

        if( KeypadButton >= BTN_EndBTN )
        {
            menuAction = MIA_Nothing;
        }
        else if( CurrentMenuItem.KeyEvents )
        {
            menuAction = *(( CurrentMenuItem.KeyEvents ) + KeypadButton );
        }

#if HotInputSelect
        if( MenuPageIndex == RootMenu || MenuPageIndex == UnsupportedModeMenu )
        {
            if( PressExitFlag )
            {
                if( MenuPageIndex == UnsupportedModeMenu )
                {
                    menuAction = MIA_SourceSel;// 2008/9/17 08:07:5 PM
                    KeypadButton = BTN_Repeat;
                }
                if( HotKeyCounter == 0 || menuAction != MIA_Nothing )
                {
                    if( EnableShowAutoFlag )
                    {
                        menuAction = MIA_Auto;
                    }
                    else
                    {
                        menuAction = MIA_SourceSel;// 2008/9/17 08:07:5 PM
                    }
                    KeypadButton = BTN_Repeat;
                }
            }
        }
#endif


		#if ENABLE_FREESYNC
        if (MenuPageIndex == MainMenu && MenuItemIndex == MainMenuItems_Information && msAPI_combo_IPGetDDRFlag())
        {
    		if( ms1000Flag )
			{
				Osd_DrawNum( Layer2XPos+6, (LayerYPos+2*2), MenuFunc_GetVfreq() );
				Clr_ms1000Flag();
			}
        }
		#endif
        if( menuAction )
        {
            if (!InputTimingStableFlag||SyncLossState())    // Annie 2011.10.10 modify for BurnIn mode needn't press OSD
            {
                if (((menuAction!=MIA_Power) && (menuAction!=MIA_SourceSel)) && (MenuPageIndex!=HotInputSelectMenu))
                    menuAction=MIA_Nothing;
            }
            else if( menuAction != MIA_Power && OsdLockModeFlag ) //080103 prevent key event if Osd Locked
            {

                if( menuAction != MIA_SourceSel && menuAction != MIA_Auto )
                {
                    if( TurboKeyCounter > 0 )
                        menuAction = MIA_Nothing;

                    else if( MenuPageIndex == RootMenu )
                    {
                        MenuPageIndex = OsdLockMenu;
                        menuAction = MIA_RedrawMenu;
                    }
                    TurboKeyCounter = 1;
                }
            }

            /*
            if( !InputTimingStableFlag )
                // prevent osd action while mode changing
            {
                if( menuAction != MIA_Power )
                {
                    menuAction = MIA_Nothing;
                }
            }
            */
            if( ExecuteKeyEvent( menuAction ) )
            {
                if( MenuPageIndex > RootMenu )
                {
                    ResetOsdTimer();
                }
                Clr_OsdTimeoutFlag();
            }
            if( menuAction == MIA_IncValue || menuAction == MIA_DecValue || menuAction == MIA_Auto )
            {
                if( TurboKeyCounter < 100 )
                {
                    TurboKeyCounter++;
                }
            }
            else if(( menuAction == MIA_Exit ) || ( menuAction == MIA_GotoECO ) )
            {
                Delay1ms( 150 );
                KeypadButton = BTN_Repeat;
            }
        }
        else
        {
            TurboKeyCounter = 0;
        }
    }

#endif

    if( OsdTimeoutFlag )
    {
printf("\r\n OsdTimeoutFlag");
        if( DisplayLogoFlag )
        {
            Clr_DisplayLogoFlag();
#if Enable_PanelHandler
            Power_PanelCtrlOnOff(FALSE, TRUE);
#else
            Power_TurnOffPanel();
#endif
            #if DEBUG_PRINT_ENABLE
            //printMsg( "--LoadCommonFont--1" );
            #endif
            LoadCommonFont();
            //          InitOsdAfterLogo();
        }

        Clr_OsdTimeoutFlag();
        if( MenuPageIndex > RootMenu )
        {
            if (MenuPageIndex == ColorTempSelectMenu)   //111111 Rick modified to set ECO = Standard only while CTEMP = SRGB
            {
                if( UserPrefColorTemp == CTEMP_SRGB && UserPrefECOMode != ECO_Standard )
                {
                    UserPrefECOMode = ECO_Standard;
                    ReadColorTempSetting();
                }
            }
            if( BackToUnsupportFlag )
            {
                MenuPageIndex = UnsupportedModeMenu;
                Clr_BackToUnsupportFlag();
            }
            else if( BackToStandbyFlag )
            {
                if(SyncLossState())
                {
                    MenuPageIndex = StandbyMenu;
                }
                Clr_BackToStandbyFlag();
            }
            else
                MenuPageIndex = RootMenu;

            MenuItemIndex = 0;
            ExecuteKeyEvent( MIA_RedrawMenu );
        }
        if(PushECOHotKeyFlag)
            Clr_PushECOHotKeyFlag();
    }
    else if( DisplayLogoFlag )
    {
        if( FlashFlag )
        {
            Clr_FlashFlag();
            menuAction = msReadByte( SC00_4C ) & 0xF;
            menuAction = ( menuAction & 0xF8 ) | (( menuAction & 7 ) + 1 );
            msWriteByte( SC00_4C, menuAction );
        }
    }

    //}
    if( !DisplayLogoFlag )
        // 2006/10/18 9:17PM by Emily for AC on but do burnin
    {
        if( DoBurninModeFlag && ChangePatternFlag )
            // for burnin mode
        {
            Clr_ChangePatternFlag();
            PatternNo = ( PatternNo + 1 ) % 5;
            mStar_AdjustBackgoundColor( PatternNo );
        }
    }
	#if 0 //consider to add no-signal OSD
	if (g_CountSwitchPortTimeFlag && g_SwitchSec >=DEF_FORCE_DPMS && SyncLossState())
	{
		printData("222 MENU  =================g_SwitchSec==%x",g_SwitchSec);
		g_CountSwitchPortTimeFlag=FALSE;
		g_SwitchSec=0;
		EnablePowerDownCounter();
	}
	#endif

    if( ms500Flag )
        // for moving osd position
    {
        Clr_ms500Flag();
        if( CurrentMenu.Flags & mpbMoving && !PowerSavingFlag )
            // && FreeRunModeFlag) // for led flash
        {
            if( ReverseXFlag )
            {
                if( moveX == 0 )
                {
                    Clr_ReverseXFlag();
                }
                //else if( moveX < 0 )
                //{
                //    moveX = 0;
                //}
                else if( moveX > 100 )
                {
                    moveX = 100;
                }
                else
                {
                    moveX -= 1;
                }
            }
            else
            {
                moveX++;
            }
            if( moveX >= 100 )
            {
                Set_ReverseXFlag();
            }
            if( ReverseYFlag )
            {
                if( moveY == 0 )
                {
                    Clr_ReverseYFlag();
                }
                //else if( moveY < 0 )
                //{
                //    moveY = 0;
                //}
                else if( moveY > 100 )
                {
                    moveY = 100;
                }
                else
                {
                    moveY -= 2;
                }
            }
            else
            {
                moveY += 2;
            }
            if( moveY >= 100 )
            {
                Set_ReverseYFlag();
            }
            //printData("--->osd X:%d",moveX);
            //printData("--->osd Y:%d",moveY);
            Osd_SetPosition( moveX, moveY );
        }
    }

    Menu_RxCableStatePollingHandler();

    if( SaveBlacklitTimeFlag )
    {
        SaveBlacklitTimeFlag = 0;
#if USEFLASH
        UserprefBacklighttime = BlacklitTime;
        UserPref_EnableFlashSaveBit( bFlashForceSaveMonitor2Bit );
#else
        SaveBlacklitTime();
#endif
    }
}

void ClrItemText(BYTE ucLayer)
{
    BYTE i;

    OsdFontColor = (CPC_White<<4|CPC_Black);
    for (i=0; i<10; i++)
    {
        if (ucLayer == Layer1XPos)
            Osd_DrawContinuesChar( Layer1XPos, LayerYPos+(i*2), SpaceFont, LayerXSize );
        else if (ucLayer == Layer2XPos)
            Osd_DrawContinuesChar( Layer2XPos, LayerYPos+(i*2), SpaceFont, LayerXSize );
        else if (ucLayer == Layer3XPos)
            Osd_DrawContinuesChar( Layer3XPos, LayerYPos+(i*2), SpaceFont, LayerXSize );
    }
}

void DrawItemSelect( BYTE itemIndex )
{
    BYTE XPos=0;

    if (!(Layer1_MenuPage || Layer2_MenuPage || Layer3_MenuPage))
        return;

    if (Layer1_MenuPage)
        XPos=Layer1XPos;
    else if (Layer2_MenuPage)
        XPos=Layer2XPos;
    else if (Layer3_MenuPage)
        XPos=Layer3XPos;

    if (itemIndex == MenuItemIndex)
    {
        OsdFontColor = (CPC_White<<4|CPC_Black);
        Osd_DrawCharDirect(XPos-1, LayerYPos+(itemIndex*2)-1, CursorLT_2C);
        Osd_DrawContinuesChar( XPos, LayerYPos+(itemIndex*2)-1, CursorTop_2C, LayerXSize );
        Osd_DrawCharDirect(XPos+LayerXSize, LayerYPos+(itemIndex*2)-1, CursorRT_2C);

        Osd_DrawCharDirect(XPos-1, LayerYPos+(itemIndex*2), CursorLeft_2C);
        Osd_DrawCharDirect(XPos+LayerXSize, LayerYPos+(itemIndex*2), CursorRight_2C);

        Osd_DrawCharDirect(XPos-1, LayerYPos+(itemIndex*2)+1, CursorLD_2C);
        Osd_DrawContinuesChar( XPos, LayerYPos+(itemIndex*2)+1, CursorDown_2C, LayerXSize );
        Osd_DrawCharDirect(XPos+LayerXSize, LayerYPos+(itemIndex*2)+1, CursorRD_2C);
    }
    else
    {
        OsdFontColor = (CPC_White<<4|CPC_Black);
        if (itemIndex == 0)
        {
            Osd_DrawCharDirect(XPos-1, LayerYPos+(itemIndex*2)-1, SpaceFont);
            Osd_DrawContinuesChar( XPos, LayerYPos+(itemIndex*2)-1, SpaceFont, LayerXSize );
            Osd_DrawCharDirect(XPos+LayerXSize, LayerYPos+(itemIndex*2)-1, SpaceFont);
        }
        else
        {
            Osd_DrawCharDirect(XPos-1, LayerYPos+(itemIndex*2)-1, SpaceFont);
            Osd_DrawContinuesChar( XPos, LayerYPos+(itemIndex*2)-1, Saperation_2C, LayerXSize );
            Osd_DrawCharDirect(XPos+LayerXSize, LayerYPos+(itemIndex*2)-1, SpaceFont);
        }

        Osd_DrawCharDirect(XPos-1, LayerYPos+(itemIndex*2), SpaceFont);
        Osd_DrawCharDirect(XPos+LayerXSize, LayerYPos+(itemIndex*2), SpaceFont);

        Osd_DrawCharDirect(XPos-1, LayerYPos+(itemIndex*2)+1, SpaceFont);
        Osd_DrawContinuesChar( XPos, LayerYPos+(itemIndex*2)+1, Saperation_2C, LayerXSize );
        Osd_DrawCharDirect(XPos+LayerXSize, LayerYPos+(itemIndex*2)+1, SpaceFont);
    }

}

void SetItemSelect( BYTE itemIndex )
{
    BYTE XPos=0;

    if (Layer1_MenuPage)
        XPos=Layer1XPos;
    else if (Layer2_MenuPage)
        XPos=Layer2XPos;
    else if (Layer3_MenuPage)
        XPos=Layer3XPos;

    OsdFontColor = (CPC_White<<4|CPC_Black);
    Osd_DrawCharDirect(XPos-1, LayerYPos+(itemIndex*2)-1, CursorLT_2C);
    Osd_DrawContinuesChar( XPos, LayerYPos+(itemIndex*2)-1, CursorTop_2C, LayerXSize );
    Osd_DrawCharDirect(XPos+LayerXSize, LayerYPos+(itemIndex*2)-1, CursorRT_2C);

    Osd_DrawCharDirect(XPos-1, LayerYPos+(itemIndex*2), CursorLeft_2C);
    Osd_DrawCharDirect(XPos+LayerXSize, LayerYPos+(itemIndex*2), CursorRight_2C);

    Osd_DrawCharDirect(XPos-1, LayerYPos+(itemIndex*2)+1, CursorLD_2C);
    Osd_DrawContinuesChar( XPos, LayerYPos+(itemIndex*2)+1, CursorDown_2C, LayerXSize );
    Osd_DrawCharDirect(XPos+LayerXSize, LayerYPos+(itemIndex*2)+1, CursorRD_2C);

}

void ClearItemSelect( BYTE itemIndex )
{
    BYTE XPos=0;

    if (Layer1_PrevMenuPage)
        XPos=Layer1XPos;
    else if (Layer2_PrevMenuPage)
        XPos=Layer2XPos;
    else if (Layer3_PrevMenuPage)
        XPos=Layer3XPos;

    OsdFontColor = (CPC_White<<4|CPC_Black);
    if (itemIndex == 0)
    {
        Osd_DrawCharDirect(XPos-1, LayerYPos+(itemIndex*2)-1, SpaceFont);
        Osd_DrawContinuesChar( XPos, LayerYPos+(itemIndex*2)-1, SpaceFont, LayerXSize );
        Osd_DrawCharDirect(XPos+LayerXSize, LayerYPos+(itemIndex*2)-1, SpaceFont);
    }
    else
    {
        Osd_DrawCharDirect(XPos-1, LayerYPos+(itemIndex*2)-1, SpaceFont);
        Osd_DrawContinuesChar( XPos, LayerYPos+(itemIndex*2)-1, Saperation_2C, LayerXSize );
        Osd_DrawCharDirect(XPos+LayerXSize, LayerYPos+(itemIndex*2)-1, SpaceFont);
    }

    Osd_DrawCharDirect(XPos-1, LayerYPos+(itemIndex*2), SpaceFont);
    Osd_DrawCharDirect(XPos+LayerXSize, LayerYPos+(itemIndex*2), SpaceFont);

    Osd_DrawCharDirect(XPos-1, LayerYPos+(itemIndex*2)+1, SpaceFont);
    Osd_DrawContinuesChar( XPos, LayerYPos+(itemIndex*2)+1, Saperation_2C, LayerXSize );
    Osd_DrawCharDirect(XPos+LayerXSize, LayerYPos+(itemIndex*2)+1, SpaceFont);

}

Bool Check_Disable_Item_Status( DWORD Flags )
{
    Bool Status = FALSE;

    // set the gary item
    /*if(( menuItem->Flags & mibVGAItem && ( CURRENT_INPUT_IS_DVI() || CURRENT_INPUT_IS_HDMI() ) )
    || ( menuItem->Flags & mibsRGBDisable && ( UserPrefColorTemp == CTEMP_SRGB && UserPrefECOMode == ECO_Standard ) )
    || ( menuItem->Flags & mibStdEnable && ( UserPrefECOMode != ECO_Standard ) )
    #if ENABLE_DP_INPUT
     ||( menuItem->Flags & mibVGAItem && ( CURRENT_INPUT_IS_DISPLAYPORT() ) )
    #endif
    || ( menuItem->Flags & mibDCRDisable && ( UserPrefDcrMode ) )
    || ( menuItem->Flags & mibUserColor  && ( UserPrefColorTemp != CTEMP_USER ) ) // jeff 1112
    #if MWEFunction
    || ( menuItem->Flags & mibBFEnable && ( !PictureBoostFlag ) )
    || ( menuItem->Flags & mibBFEnable && MenuPageIndex == BrightFrameMenu && UserPrefBFSize == 0 )
    || ( menuItem->Flags & mibDemoDisable && MenuPageIndex == BrightFrameMenu && DemoFlag )
    #endif
    )*/
    if ( ( (Flags & mibUserColor) && (UserPrefColorTemp != CTEMP_USER) )
      || ( (Flags & mibVGAItem) && (!CURRENT_INPUT_IS_VGA()) )
      || ( (Flags & mibYUVEnable)&& IsColorspaceRGB() )
      //|| ( Flags & dwiGameDisable && ( UserPrefColorMode == ECO_GAME) )
      //|| ( Flags & dwiSRGBDisable && ( UserPrefColorMode == ECO_SRGB) )
      //|| ( Flags & dwiPhotoDisable && ( UserPrefColorMode == ECO_PHOTO) )
	#if Enable_Expansion
      || ( Flags & mibExpansionDisable4_3 && AspRatio4_3_EnisableFlag==FALSE )
      || ( Flags & mibExpansionDisable16_9 && AspRatio16_9_EnisableFlag==FALSE )
      || ( Flags & mibExpansionDisable1_1 && AspRatio1_1_EnisableFlag==FALSE )
      || ( Flags & mibExpansionDisableFixInputRatio && AspRatioFixedInputRatioEnisableFlag==FALSE )
	#endif
      //|| ((Flags & mibAudioSelDisable) && !(CURRENT_INPUT_IS_HDMI()||CURRENT_INPUT_IS_DISPLAYPORT()) )
       #if ENABLE_DP_INPUT
      || ( (Flags & mibMSTDisable) && (UserPrefDPVersion == DPVersionMenuItems_DP1_1) )
       #endif
       )
    {
        Status = TRUE;
    }

    return Status;
}

Bool Check_Disable_ItemRadioGroup_Status( WORD Flags )
{
    Bool Status = FALSE;
    if ( ( (Flags & dwiUserColor) && (UserPrefColorTemp != CTEMP_USER) )
        || ( (Flags & dwiVGAItem) && (!CURRENT_INPUT_IS_VGA()) )
        || ( (Flags & dwiVideoOption) && IsColorspaceRGB() )
      //|| ( (Flags & dwiAudioSourceOption) && !(CURRENT_INPUT_IS_HDMI()||CURRENT_INPUT_IS_DISPLAYPORT()) )
      //|| ( Flags & dwiVideoDisable && ( UserPrefColorMode == ECO_VIDEO) )
      //|| ( Flags & dwiGameDisable && ( UserPrefColorMode == ECO_GAME) )
      //|| ( Flags & dwiSRGBDisable && ( UserPrefColorMode == ECO_SRGB) )
      //|| ( Flags & dwiPhotoDisable && ( UserPrefColorMode == ECO_PHOTO) )
       )
    {
        Status = TRUE;
    }
    return Status;
}

void DrawInformation( void )
{
	XDATA DWORD colck;
    WORD freq;
	BYTE Y_PosStart = 1;
	
    OsdFontColor = (CPC_White<<4|CPC_Black);

    //=================================================
    Osd_DrawPropStr( 3, Y_PosStart, ResolutionText() );
    Osd_Draw4Num( 10, Y_PosStart, SC0_READ_IMAGE_WIDTH() );
#if DECREASE_V_SCALING
    Osd_Draw4Num( 15, Y_PosStart, SC0_READ_IMAGE_HEIGHT()-DecVScaleValue );
#else
    Osd_Draw4Num( 15, Y_PosStart, SC0_READ_IMAGE_HEIGHT() );
#endif
    Osd_DrawPropStr( 14, Y_PosStart, xText() );

	freq = MenuFunc_GetVfreq();

    Osd_DrawNum( 20, Y_PosStart, freq);

    Osd_DrawPropStr( 22, Y_PosStart, HzText() );
    //=================================================
    Osd_DrawPropStr( 3, Y_PosStart+1, PixelClockText());
    colck = (DWORD)mSTar_GetInputHTotal() * SrcVTotal * SrcVFreq / 100000;
    //printData("colck=%d",colck);
    DrawNum_R(10,Y_PosStart+1,3,colck/100);
	Osd_DrawPropStr( 13, Y_PosStart+1, DotText());
    DrawNum(14,Y_PosStart+1,2,colck%100);
    Osd_DrawPropStr( 16, Y_PosStart+1, MHzText());
	//=================================================
}

#if ENABLE_3DLUT
extern Bool Adjust3DLUTMode( MenuItemActionType action );
#endif


Bool ExecuteKeyEvent( MenuItemActionType menuAction )
{
    Bool processEvent = TRUE;
    BYTE tempValue; //,Volume; wmz 20051019
#if USEFLASH        //110915 Rick
    BYTE i;
#endif
    while( processEvent )
    {
        processEvent = FALSE;


        switch( menuAction )
        {

            case MIA_IncValue:
            case MIA_DecValue:
                if( CurrentMenuItemFunc.AdjustFunction )
                {
                    if(( TurboKeyCounter > 0 ) && !( CurrentMenuItem.DisplayValue.DrawNumber ) //071225 adjust value once if not release key
                            && !( CurrentMenuItem.DisplayValue.DrawGuage ) )
                    {
                        Delay1ms( 100 ); //090908
                        break;
                    }

                    if( CurrentMenuItemFunc.AdjustFunction( menuAction ) )
                    {
                        // immediately show setting value
                        DrawOsdMenuItemValue( MenuItemIndex, &CurrentMenuItem.DisplayValue );
                        #if (ENABLE_VGA_INPUT)
                        if( MenuPageIndex >= ClockMenu && MenuPageIndex <= VPositionMenu )
                        #else
                        if( MenuPageIndex >= OSDTransMenu && MenuPageIndex <= SaturationMenu )
                        #endif
                            Set_SaveModeSettingFlag();
                        else
                            Set_SaveMonitorSettingFlag();
                        if( MenuPageIndex == FactoryMenu )
                            Set_SaveFactorySettingFlag();

                        if( TurboKeyCounter < 7 )
                        {
                            Delay1ms( 200 );
                        }

                        if( CurrentMenuItemFunc.AdjustFunction == AdjustAddr || CurrentMenuItemFunc.AdjustFunction == AdjustBankNo )
                        {
                            DrawOsdMenuItemNumber( 0, DrawAddrRegValueNumber );
                            TurboKeyCounter = 0;
                        }
                    }
                }
                break;
            case MIA_NextItem:
            case MIA_PrevItem:
                //PrevMenuItemIndex=MenuItemIndex;
                tempValue = MenuItemIndex;
                if( menuAction == MIA_NextItem )
                {
                    MenuItemIndex = GetNextItem( CurrentMenuItems );
                }
                else
                {
                    MenuItemIndex = GetPrevItem( CurrentMenuItems );
                }
                if( tempValue != MenuItemIndex )
                {
					#if 1//(LiteMAX_OSDtype==LiteMAX_OSD_standard)
					if( MenuPageIndex == MainMenu )
					{
                    	#if 0 
						if((MenuItemIndex==4)&&(!CURRENT_INPUT_IS_VGA()))
						{
							if( menuAction == MIA_NextItem )
								MenuItemIndex++;
							else
								MenuItemIndex--;
						}
                    	#endif
						OsdFontColor=FOUR_COLOR(6);						
						Osd_DrawContinuesChar( SubMenuIcon_X_Start, SubMenuIcon_Y_Start, Space4C, 26 );
						Osd_DrawContinuesChar( SubMenuIcon_X_Start, SubMenuIcon_Y_Start+1, Space4C, 26 );
					}
					#endif
                	#if 0
                    if (Layer1_MenuPage)
                    {
                        ClrItemText(Layer2XPos);
                        ClrItemText(Layer3XPos);
                    }
					#endif
                    if( CurrentMenuItem.Fonts )
                    {
                        DynamicLoadFont( CurrentMenuItem.Fonts, PropFontAddr2 );
                    }
					#if 0
                    DrawItemSelect(tempValue);
					#endif
                    DrawOsdMenuItem( tempValue, &CurrentMenuItems[tempValue] );
					#if 0
                    DrawItemSelect(MenuItemIndex);
					#endif
                    DrawOsdMenuItem( MenuItemIndex, &CurrentMenuItem );
					#if 0
                    if (MenuPageIndex == MainMenu && MenuItemIndex == MainMenuItems_Information)
                    {
                        DrawInformation();
                    }
					#endif
#if 0//ENABLE_3DLUT
                    if( MenuPageIndex == GammaMenu )
                        Adjust3DLUTMode(0);
#endif


                    if( MenuPageIndex == FactoryMenu )                        // for factory mode
                    {
                        if(( MenuItemIndex == 14 && tempValue < 14 ) || ( MenuItemIndex == 22 && tempValue > 23 ) )
                        {
                            SetFactoryColorTempCool1();
                        }
                        else if(( MenuItemIndex == 24 && tempValue < 24 ) || ( MenuItemIndex == 32 && tempValue > 33 ) )
                        {
                            SetFactoryColorTempNormal();
                        }
                        else if(( MenuItemIndex == 34 && tempValue < 34 ) || ( MenuItemIndex == 42 && tempValue > 43 ) )
                        {
                            SetFactoryColorTempWarm1();
                        }
                        else if(( MenuItemIndex == 44 && tempValue < 44 ) || ( MenuItemIndex == 52 && tempValue > 53 ) )
                        {
                            SetFactoryColorTempSRGB();
                        }

                        if( SaveFactorySettingFlag )
                        {
                            #if USEFLASH
                            //SaveFactorySetting();
                            UserPref_EnableFlashSaveBit( bFlashSaveFactoryBit );
                            #else
                            SaveFactorySettingByItem( tempValue );
                            #endif
                            Clr_SaveFactorySettingFlag();
                        }
                    }
                    Delay1ms( 300 );
                }
                break;
            case MIA_JumpAdjustItem:
            case MIA_JumpNaviItem:
                tempValue = MenuItemIndex;
                MenuItemIndex += (( menuAction == MIA_JumpAdjustItem ) ? ( 1 ) : ( - 1 ) );
            #if (ENABLE_DAC && ( ENABLE_HDMI || ENABLE_DP_INPUT))
                if (MenuPageIndex == HotKeySourceVolMenu)
                {
                    DrawOsdMenuItem( MenuItemIndex, &CurrentMenuItem );
                }
                else
            #endif
                {
                DrawOsdMenuItem( tempValue, &CurrentMenuItems[tempValue] );
                DrawOsdMenuItem( MenuItemIndex, &CurrentMenuItem );
                }
                Delay1ms( 300 );
                break;
			#if LiteMAX_OSD_TEST
			case MIA_GotoNextExec:
                if( CurrentMenuItemFunc.ExecFunction )
                {
                    processEvent = CurrentMenuItemFunc.ExecFunction();
                    processEvent = FALSE;
                }
			#endif
            case MIA_GotoNext:
            case MIA_GotoPrev:
                //printMsg(">>>>>>>>>>>>>>>MIA_GotoNext");
                PrevMenuPageIndex = MenuPageIndex;
                PrevMenuItemIndex = MenuItemIndex;
				#if LiteMAX_OSD_TEST
                MenuPageIndex = ( menuAction == MIA_GotoNext || menuAction == MIA_GotoNextExec) ? ( NextMenuPage ) : ( PrevMenuPage );
				#else
				MenuPageIndex = ( menuAction == MIA_GotoNext ) ? ( NextMenuPage ) : ( PrevMenuPage );
				#endif
                if( PrevMenuPageIndex == MainMenu && MenuPageIndex == RootMenu )
                {
                    if( !FactoryModeFlag )
                        UserPrefLastMenuIndex = MenuItemIndex;
                }
                if( PrevMenuPageIndex == RootMenu && MenuPageIndex == MainMenu && OsdLockModeFlag )
                    MenuPageIndex = OsdLockMenu;
                if(( PrevMenuPageIndex == RootMenu && MenuPageIndex == MainMenu ) )
                {
                    // In factory mode or in DVI but stay in ImageSetup page, default in menuitemindex 0
                    //if ((!FactoryModeFlag) && !(SrcInputType == Input_Digital && UserPrefLastMenuIndex == 1))
                #if 0
                    if( !FactoryModeFlag )
                        MenuItemIndex = UserPrefLastMenuIndex;
                    else
                #endif
                        MenuItemIndex = 0;                    // while enter factory, then stay in Lum.
                    //printData("111 MenuItemIndex==%d",MenuItemIndex);

                }
                else
                {
                    MenuItemIndex = GetMenuItemIndex( PrevMenuPageIndex );
                    //printData("22222 MenuItemIndex==%d",MenuItemIndex);
                }
				#if 1 //(LiteMAX_OSDtype==LiteMAX_OSD_standard)
                if(((PrevMenuPageIndex==LuminanceMenu) && (MenuPageIndex==BrightnessMenu||MenuPageIndex==ContrastMenu))||
#if 0// jason 20200113 //BrightnessLightSensorVR
                   ((PrevMenuPageIndex==LuminanceMenu) && (MenuPageIndex==BrightnessTypeMenu))||
                   ((MenuPageIndex==LuminanceMenu) && (PrevMenuPageIndex==BrightnessTypeMenu))||
#endif
                   ((PrevMenuPageIndex==SoundMenu) && (MenuPageIndex==SoundAdjustMenu))||
                   ((MenuPageIndex==SoundMenu) && (PrevMenuPageIndex==SoundAdjustMenu))||
                   ((PrevMenuPageIndex==ColorMenu) && (MenuPageIndex==ColorTempMenu))||
                   ((MenuPageIndex==ColorMenu) && (PrevMenuPageIndex==ColorTempMenu))||
                   ((PrevMenuPageIndex==ColorTempMenu) && (MenuPageIndex==ColorSettingsMenu))||
                   ((PrevMenuPageIndex==ImageMenu) && ((MenuPageIndex==ImageClockMenu)||(MenuPageIndex==ImagePhaseMenu)||(MenuPageIndex==ImageHPosMenu)||(MenuPageIndex==ImageVPosMenu)))||
                   ((MenuPageIndex==ImageMenu) && ((PrevMenuPageIndex==ImageClockMenu)||(PrevMenuPageIndex==ImagePhaseMenu)||(PrevMenuPageIndex==ImageHPosMenu)||(PrevMenuPageIndex==ImageVPosMenu)))||
                   ((PrevMenuPageIndex==ToolMenu) && (MenuPageIndex==OSDControlMenu||MenuPageIndex==SharpnessMenu))||
                   ((MenuPageIndex==ToolMenu) && (PrevMenuPageIndex==OSDControlMenu))||
                   ((PrevMenuPageIndex==OSDControlMenu) && (MenuPageIndex==ToolMenu))||
#if ENABLE_OSD_ROTATION
                   ((PrevMenuPageIndex==OSDRotationMenu) && (MenuPageIndex==OSDControlMenu))||
#endif
                   ((MenuPageIndex==ToolMenu) && (PrevMenuPageIndex==SharpnessMenu))||
                   ((PrevMenuPageIndex==OSDControlMenu) && (MenuPageIndex==OSDTimeMenu||MenuPageIndex==OSDHPosMenu||MenuPageIndex==OSDVPosMenu))||
                   ((MenuPageIndex==OSDControlMenu) && (PrevMenuPageIndex==OSDTimeMenu||PrevMenuPageIndex==OSDHPosMenu||PrevMenuPageIndex==OSDVPosMenu))||
                   ((PrevMenuPageIndex==BrightnessMenu||PrevMenuPageIndex==ContrastMenu) && (MenuPageIndex==LuminanceMenu)))
                {
                    OsdFontColor = FOUR_COLOR(5);
    #if 0 //SpanpixelOSD
                    Osd_DrawContinuesChar( 2, SubMenuItem_Y_Start, SpaceFont, 27 );
                    Osd_DrawContinuesChar( 2, SubMenuValue_Y_Start, SpaceFont, 27 );
                    Osd_DrawContinuesChar( 2, SubMenuBar_Y_Start, SpaceFont, 27 );
    #else
                    Osd_DrawContinuesChar( 2, 10, Space4C, 26 );
                    Osd_DrawContinuesChar( 2, 11, Space4C, 26 );
                    Osd_DrawContinuesChar( 2, 12, Space4C, 26 );
                    Osd_DrawContinuesChar( 2, 13, Space4C, 26 );
    #endif
                }
                else if((PrevMenuPageIndex==ColorSettingsMenu) && (MenuPageIndex==ColorTempMenu))
                {
                    OsdFontColor = FOUR_COLOR(5); 
    #if 0 //SpanpixelOSD
                    Osd_DrawContinuesChar( 2, SubMenuItem_Y_Start, SpaceFont, 27 );
                    Osd_DrawContinuesChar( 2, SubMenuValue_Y_Start, SpaceFont, 27 );
                    Osd_DrawContinuesChar( 2, SubMenuBar_Y_Start, SpaceFont, 27 );
    #else
                    Osd_DrawContinuesChar( 2,  9, Space4C, 27 );
                    Osd_DrawContinuesChar( 2, 10, Space4C, 27 );
                    Osd_DrawContinuesChar( 2, 11, Space4C, 27 );
                    Osd_DrawContinuesChar( 2, 12, Space4C, 27 );
                    Osd_DrawContinuesChar( 2, 13, Space4C, 27 );
    #endif
                }
#else				
                if (Layer2_PrevMenuPage && Layer3_MenuPage)
                {
                    ucClrItem = PrevMenuItemIndex;
                    //printData("ucClrItem 1[%d]", ucClrItem);
                }
                else if (Layer3_PrevMenuPage && IndependentColor_MenuPage)
                {
                    ucClrItem = PrevMenuItemIndex + 1;
                    //printData("ucClrItem 1[%d]", ucClrItem);
                }

#endif
                if( menuAction == MIA_GotoPrev && BackToUnsupportFlag )
                {
                    MenuPageIndex = UnsupportedModeMenu;
                    Clr_BackToUnsupportFlag();
                }
                else if( menuAction == MIA_GotoPrev && BackToStandbyFlag )
                {
                    if((SyncLossState()))
                    {
                        MenuPageIndex = StandbyMenu;
                    }
                    Clr_BackToStandbyFlag();
                }

                #if HotInputSelect
                Clr_PressExitFlag();
                Clr_EnableShowAutoFlag();
                #endif

            case MIA_RedrawMenu:
                DrawOsdMenu();

                if( CurrentMenu.ExecFunction )
                {
                    tempValue = CurrentMenu.ExecFunction();
                }
                if( !( CurrentMenu.Flags & mpbStay ) )
                {
                    processEvent = TRUE;
                    menuAction = MIA_GotoPrev;
                }
                if( SaveMonitorSettingFlag )
                {
                    Clr_SaveMonitorSettingFlag();
                    //SaveUserPref();
                #if USEFLASH
                    Set_FlashSaveMonitorFlag();
                #else
                    SaveMonitorSetting();
                #endif
                }
                if( SaveModeSettingFlag )
                {
                    Clr_SaveModeSettingFlag();
                #if USEFLASH
                    Set_FlashSaveModeFlag();
                #else
                    SaveModeSetting();
                #endif
                }
                break;
            case MIA_ExecFunc:
            case MIA_ExecFunc_And_GotoPrev:
                if( CurrentMenuItemFunc.ExecFunction )
                {
//                    tempValue = 0;
                    if( CurrentMenuItemFunc.ExecFunction == AutoColor )
                    {
                        Osd_SetTextColor( CP_WhiteColor, CP_BlueColor );
                        //Osd_DrawStr(12, 1, tSpace6);
                    }
                    processEvent = CurrentMenuItemFunc.ExecFunction();

/*
                    if( tempValue == 1 )
                    {
                        Osd_SetTextColor( CP_BlueColor, CP_WhiteColor );
                        DrawOsdMenuItemValue( MenuItemIndex, &CurrentMenuItem.DisplayValue );
                    }
*/
                    if( CurrentMenuItemFunc.ExecFunction == AutoColor )
                    {
                        Osd_SetTextMonoColor( CP_RedColor, CP_BlueColor );
                        if( processEvent && CURRENT_INPUT_IS_VGA() )//( processEvent && SrcInputType == Input_Analog1 )
                        {
                            Osd_DrawStr( 12, 6, PassText() );
                        }
                        else
                        {
                            Osd_DrawStr( 12, 6, FailText() );
                        }

                        Flash_ReadFactorySet(( BYTE* )&FactorySetting, FactorySettingSize );

#if 1 //2006-08-03 Andy
                        DrawOsdMenuItemNumber( 0, DrawGainRNumber );
                        DrawOsdMenuItemNumber( 0, DrawGainGNumber );
                        DrawOsdMenuItemNumber( 0, DrawGainBNumber );
                        DrawOsdMenuItemNumber( 0, DrawOffsetRNumber );
                        DrawOsdMenuItemNumber( 0, DrawOffsetGNumber );
                        DrawOsdMenuItemNumber( 0, DrawOffsetBNumber );
#endif
                    }
                    processEvent = FALSE;
                    #if INPUT_TYPE!=INPUT_1A
                    if(( MenuPageIndex == SourceSelectMenu
#if HotInputSelect
                            || MenuPageIndex == HotInputSelectMenu
#endif
                       ) && processEvent == FALSE )
                    {
                        menuAction = MIA_GotoPrev;
                        //MenuItemIndex = 0;
                        processEvent = TRUE;
                    }
                    #endif
                }

				#if ENABLE_OSD_ROTATION
                if( CurrentMenuItemFunc.ExecFunction == SetOSDRotateMode )
                {
					Osd_Hide();
					PrevMenuPageIndex = RootMenu;
					PrevMenuItemIndex = 0;
					MenuPageIndex = MainMenu;
					MenuItemIndex = 0;
					ExecuteKeyEvent( MIA_RedrawMenu );
					Osd_Show();
                }
				#endif

                if (menuAction == MIA_ExecFunc_And_GotoPrev)
                {
                    menuAction = MIA_GotoPrev;
                    processEvent = TRUE;
                }

                break;
            case MIA_Exit:
                if( PrevMenuPageIndex == MainMenu && MenuPageIndex == RootMenu )
                {
                    if( !FactoryModeFlag )
                        UserPrefLastMenuIndex = MenuItemIndex;
                }
                menuAction = MIA_RedrawMenu;
                PrevMenuPageIndex = MenuPageIndex; //WMZ 050816
                MenuPageIndex = RootMenu;
                MenuItemIndex = 0;
                processEvent = TRUE;
                if( FactoryModeFlag && PrevMenuPageIndex == FactoryMenu )
                {
                    SaveFactorySetting();
                }
#if HotInputSelect
                Clr_PressExitFlag();
                Clr_EnableShowAutoFlag();
#endif
                break;
            case MIA_ECO:
                if( FreeRunModeFlag )
                {
                    break;
                }
            #if 0
                if (UserPrefDcrMode)    //111019 Rick add for meet new OTS spec - A063
                    break;
            #endif
               // if( UserPrefColorTemp == CTEMP_SRGB )
                  //  UserPrefECOMode = ECO_Standard;

#ifdef DCRClearBacklight//When user press eco hotkey,turn on backlight.
                if( !BackLightActiveFlag )
                    hw_SetBlacklit();
#endif
                //SetECO();
                UserPrefDcrMode = 1;
                AdjustDcrMode( 0 );
#if USEFLASH
                //SaveMonitorSetting();
                UserPref_EnableFlashSaveBit( bFlashSaveMonitorBit );
#else
                NVRam_WriteByte( nvrMonitorAddr( DcrMode ), UserPrefDcrMode );
#endif
                PrevMenuPageIndex = MenuPageIndex;
                menuAction = MIA_RedrawMenu;
                MenuPageIndex = HotKeyECOMenu;
                MenuItemIndex = 0;
                processEvent = TRUE;
                Set_PushECOHotKeyFlag();
                break;
            case MIA_Brite:
                menuAction = MIA_RedrawMenu;
                MenuPageIndex = HotKeyBrightnessMenu;
                MenuItemIndex = 0;
                processEvent = TRUE;
                break;
#if ENABLE_DAC
            case MIA_VOL:
                if( FreeRunModeFlag )
                {
                    break;
                }
                menuAction = MIA_RedrawMenu;
        #if 0//ENABLE_HDMI
            if( stHDMIRxInfo.stPollingInfo.bHDMIModeFlag && CURRENT_INPUT_IS_HDMI())//SrcInputType >= Input_Digital )
                MenuPageIndex = HotKeySourceVolMenu;
            else
        #endif
        #if 0//ENABLE_DP_INPUT
            if(CURRENT_INPUT_IS_DISPLAYPORT())//(SrcInputType==Input_Displayport)
                MenuPageIndex = HotKeySourceVolMenu;
            else
        #endif
                MenuPageIndex = HotKeyVolMenu;
                MenuItemIndex = 0;
                processEvent = TRUE;
                break;
                /*
                case MIA_Mute:
                if ( FreeRunModeFlag )
                {
                break;
                }
                menuAction = MIA_RedrawMenu;
                MenuPageIndex = HotKeyMuteMenu;
                MenuItemIndex = 0;
                processEvent = TRUE;
                break;
                 */
#endif
#if HotExpansion
            case MIA_Expansion:
                menuAction = MIA_RedrawMenu;
                MenuPageIndex = HotExpansionMenu;
                MenuItemIndex = 0;
                processEvent = TRUE;
                break;
#endif
#if (ENABLE_VGA_INPUT)
            case MIA_Auto:
                if(!CURRENT_INPUT_IS_VGA())//( SrcInputType != Input_Analog1 )
                    break;
                menuAction = MIA_RedrawMenu;
                PrevMenuPageIndex = MenuPageIndex;
                MenuPageIndex = AutoMenu;
                MenuItemIndex = 0;
                processEvent = TRUE;
#if HotInputSelect
                Clr_PressExitFlag();
                Clr_EnableShowAutoFlag();
#endif
                break;
#endif

                //2006-02-22
            case MIA_SourceSel:
                Osd_Hide();///yuanzhi20100507_2 for mainmenu press sourceselectkey osd garbage.
                if( MenuPageIndex == UnsupportedModeMenu )
                {
                    Set_BackToUnsupportFlag();
                    Osd_Hide();
                    moveY = moveX = 50;
                }
#if Enable_PanelHandler
                Power_PanelCtrlOnOff(TRUE, TRUE);
#else
                Power_TurnOnPanel();
#endif
                menuAction = MIA_RedrawMenu;
                MenuPageIndex = HotInputSelectMenu;
                MenuItemIndex = UserPrefInputPriorityType;

                processEvent = TRUE;
                Clr_PressExitFlag();
                Clr_EnableShowAutoFlag();
                break;

            case MIA_EXEC_FACTORY_NVRAMINIT:
                Osd_SetTextMonoColor( CP_BlueColor, CP_BlueColor );
                Osd_DrawStr( 13, 29, tSpace7);
                Osd_SetTextMonoColor( CP_RedColor, CP_BlueColor );
                Osd_DrawStr( 13, 29, OnText() );
#if USEFLASH        //110915 Rick
                if( CurrentMenuItemFunc.ExecFunction == EraseFlashUserData)
                {
                    i = CurrentMenuItemFunc.ExecFunction();
                    Osd_SetTextMonoColor( CP_WhiteColor, CP_BlueColor );
                    Osd_DrawStr( 13, 29, OffText() );
                #if ODM_NAME==ODM_AOC
                    if( i )
                    {
                        Power_TurnOnAmberLed();
                    }
                #endif
                }
#endif
                break;

            case MIA_Power:
                if( PowerOnFlag )
                {
                    MenuPageIndex = PowerOffMenu;
                }
                else
                {
                    MenuPageIndex = PowerOnMenu;
                    PowerOnSystem();
                }
                MenuItemIndex = 0;
                menuAction = MIA_RedrawMenu;
                processEvent = TRUE;
                break;
            default:
                return FALSE;
                break;
        }
    }
    return TRUE;
}
//=========================================================================
void DynamicLoadFont( const MenuFontType *menuFonts, BYTE addr )
{
    if( menuFonts->Fonts )
    {
        Osd_DynamicLoadFont( addr, ( BYTE* )(( menuFonts + UserPrefLanguage )->Fonts ), ( menuFonts + UserPrefLanguage )->FontCount );
    }
}

#if 0 //LiteMAX_OSD_TEST
void LoadMainMenuPropFont(void)
{
    OSD_FONT_HI_ADDR_SET_BITS(1); //enable bit 8 0x100~
    Osd_DynamicLoadFont(MM_PROP_FONT_START, (BYTE*)((MenuPage0+UserPrefLanguage)->Fonts), (MenuPage0+UserPrefLanguage)->FontCount);
    OSD_FONT_HI_ADDR_CLR_TO_0();
}
#endif

#if ENABLE_DEBUG
extern BYTE FlashReadSR( void );
#endif
void DrawOsdMenu( void )
{
    Bool redrawFlags;

#if ENABLE_OSD_ROTATION
    if (UserPrefOsdRotateMode==OSDRotateMenuItems_90)
        msWriteByteMask(OSD1_03, BIT6, BIT7|BIT6);
	else
	#if defined(_OSD_ROTATION_180_)
    if (UserPrefOsdRotateMode==OSDRotateMenuItems_180)
        msWriteByteMask(OSD1_03, BIT7, BIT7|BIT6);
	else
	#endif
	#if defined(_OSD_ROTATION_270_)
	if (UserPrefOsdRotateMode==OSDRotateMenuItems_270)
        msWriteByteMask(OSD1_03, BIT7|BIT6, BIT7|BIT6);
    else
	#endif
        msWriteByteMask(OSD1_03, 0, BIT7|BIT6);
#endif

    Clr_JustShowValueFlag();
#if(DisplayLogo==IMAGICLOGO)
    if( MenuPageIndex != LogoMenu )
        //old_msWriteByte( BK0_32, 0 );
        drvOSD_FrameColorEnable(FALSE);
#endif
    if( LoadOSDDataFlag ) //eshin
    {
        Clr_LoadOSDDataFlag();
        Osd_Hide();
        #if DEBUG_PRINT_ENABLE
        printMsg( "--LoadCommonFont--2" );
        #endif
        LoadCommonFont();
        //       Osd_LoadColorPalette(); // load osd color
    }
    if( CurrentMenu.Flags & mpbInvisible )
    {
        Osd_Hide();
    }
    else
    {
        BYTE i; //, j;

        redrawFlags = FALSE;        // setup osd window & position

        if( MenuPageIndex == MainMenu && FactoryModeFlag )
        {
            #if DEBUG_PRINT_ENABLE
            printData( "LoadFfont--DrawOSDMenu", 0 );
            #endif
            LoadFfont();
            MenuPageIndex = FactoryMenu;
        }

        if( CurrentMenu.Fonts )
        {
            DynamicLoadFont( CurrentMenu.Fonts, PropFontAddr1 );
        }

        if( CurrentMenuItem.Fonts )
        {
            DynamicLoadFont( CurrentMenuItem.Fonts, PropFontAddr2 );
        }

        if( MenuPageIndex == FactoryMenu )
        {
            Osd_Hide(); // 2006/8/25 6:26PM by Emily for hide the garbage
            Delay1ms( 50 );
            Osd_LoadFacoryFontCP();
            Set_LoadOSDDataFlag();//eshin
        }

        if(( CurrentMenu.XSize != OsdWindowWidth || CurrentMenu.YSize != OsdWindowHeight ||
        !( msReadByte( (OSD_MAIN_WND<<5)+OSD2_00 )&MWIN_B ) || CurrentMenu.Flags & mpbRedraw )
        && !( MenuPageIndex == MainMenu && PrevMenuPageIndex != RootMenu
        )
          )
        {
            redrawFlags = TRUE;
        }

        if( redrawFlags )
        {
            #if DEBUG_PRINT_ENABLE
            printData( "redrawFlags [%d]", MenuPageIndex );
            #endif
            Osd_Hide();
            //       LoadCommonFont(); // code test
            Delay1ms( 80 );
            Osd_SetWindowSize( CurrentMenu.XSize, CurrentMenu.YSize );
            if( FactoryModeFlag )
            {
                Osd_SetPosition( 0, 100 );
            }
            else if (CurrentMenu.Flags &mpbCenter)
            {
                moveX = 50;
                moveY = 50;
                Osd_SetPosition( 50, 50 );
            }
            else if ( !( CurrentMenu.Flags & mpbMoving ) )
            {
                Osd_SetPosition( UserPrefOsdHStart, UserPrefOsdVStart );
            }

            if( CurrentMenu.Flags & mpbLogoFrame )
            {
                {
                    #if DisplayLogo!=NoBrand
                    // printMsg("draw logo");
                    Osd_SetPosition( 50, 50 );
                    moveX = 50;
                    moveY = 50;
                    DrawLogo();
                    #if (DisplayLogo!=IMAGICLOGO)
                    //old_msWriteByte( BLENDC, 0x05 ); // Entire OSD transpancy
                    msWriteByte( SC00_4C, 0x20 ); // new transpancy function
                    #endif
                    #endif
                }
            }
            else if( MenuPageIndex == FactoryMenu )
            {
                //old_msWriteByte( BLENDC, 0x00 );
                Osd_SetTextMonoColor( CP_BlueColor, CP_BlueColor );
                for( i = 0; i < OsdWindowHeight; i++ )
                {
                    Osd_DrawContinuesChar( 0 , i , SpaceFont , OsdWindowWidth );
                }
            }
            else
            {
                if( MenuPageIndex == AutoColorMenu )
                    LoadCommonFont();

                //old_msWriteByte( BLENDC, 0x00 );
                DrawOsdBackGround();
            }
			#if 0
            DrawItemSelect(MenuItemIndex);
			#endif
            for ( i = 0; i < MenuItemCount; i++ )
            {
                DrawOsdMenuItem( i, &CurrentMenu.MenuItems[i] );
            }
			#if 0
            if (MenuPageIndex == MainMenu && MenuItemIndex == MainMenuItems_Information)
            {
                DrawInformation();
            }
			#endif
			if (MenuPageIndex == MainMenu)
            {
                DrawInformation();
            }
			#if 0
            else if (MenuPageIndex == InputInfoMenu || MenuPageIndex == UnsupportedModeMenu)
            {
                DrawInputInfo();
            }
			#endif
            else if (MenuPageIndex == StandbyMenu || MenuPageIndex == CableNotConnectedMenu)
            {
#if 1//(LiteMAX_OSDtype==LiteMAX_OSD_standard) // jason 20190611
                ;
#else
                DrawNosignalInfo();
#endif
            }

            Osd_Show();
        } // end redraw
        else
        {
        	#if 0
            //printData("ucClrItem 2[%d]", ucClrItem);
            if (Layer3_PrevMenuPage && (PrevMenu.Flags & mpbAdjust))
                ClearItemSelect(ucClrItem);
            else
                ClearItemSelect(PrevMenuItemIndex);

            if (Layer3_MenuPage && (CurrentMenu.Flags & mpbAdjust))
                SetItemSelect(ucClrItem);
            else
                DrawItemSelect(MenuItemIndex);

            if (Layer3_MenuPage && !(CurrentMenu.Flags & mpbAdjust))
                ClrItemText(Layer3XPos);
            else if (Layer3_PrevMenuPage && !(PrevMenu.Flags & mpbAdjust))
                ClrItemText(Layer3XPos);
			#endif
            for ( i = 0; i < MenuItemCount; i++ )
            {
                DrawOsdMenuItem( i, &CurrentMenu.MenuItems[i] );
            }
            //DrawOsdMenuItem(MenuItemIndex, &CurrentMenu.MenuItems[MenuItemIndex]);
            if (Layer2_PrevMenuPage && Layer1_MenuPage)
            {
                DrawInformation();
            }
        }

    }
}


//=========================================================================
void DrawOsdMenuItem( BYTE itemIndex, const MenuItemType *menuItem )
{
    if( menuItem->Flags & mibFactoryItem && !FactoryModeFlag )
    {
        return ;
    }

    if( menuItem->Flags &mibSelectable && itemIndex == MenuItemIndex )
    {
        OsdFontColor = ( menuItem->SelForeColor & 0xF ) << 4 | ( menuItem->SelBackColor & 0xF );
    }
    else
    {
        OsdFontColor = ( menuItem->ForeColor & 0xF ) << 4 | ( menuItem->BackColor & 0xF );
    }

    if ( Check_Disable_Item_Status ( menuItem->Flags ) )
    {
        OsdFontColor= ( CPC_Gray<<4 | CPC_Black );
    }

    DrawOsdMenuItemText( itemIndex, menuItem );
    if( MenuPageIndex == MainMenu && itemIndex == MenuItemIndex )
    {
        DrawOsdMenuItemValue( itemIndex, &( menuItem->DisplayValue ) );
    } //->DisplayValue);
    else if( MenuPageIndex != MainMenu )
    {
        DrawOsdMenuItemValue( itemIndex, &( menuItem->DisplayValue ) );
    } //->DisplayValue);
}

//=========================================================================
// draw Icon
void DrawOsdIcon( BYTE xPos, BYTE yPos, WORD charStart )
{
    Osd_DrawCharDirect(xPos, yPos, charStart);
    Osd_DrawCharDirect(xPos+1, yPos, charStart + 2);
    Osd_DrawCharDirect(xPos+2, yPos, charStart + 4);
    Osd_DrawCharDirect(xPos, yPos+1, charStart + 6);
    Osd_DrawCharDirect(xPos+1, yPos+1, charStart + 8);
    Osd_DrawCharDirect(xPos+2, yPos+1, charStart + 10);
}
//=========================================================================
#if 0//BrightnessLightSensorVR
// draw Icon
WORD DrawOsdBrightnessType(void)
{
    if(UserprefLITEMAX_LIGHTSENSOR == LITEMAX_LIGHTSENSOR_LS) // LS
        return MainIcon4C_1_BrightnessSub+4*(6*2);
    else if(UserprefLITEMAX_LIGHTSENSOR == LITEMAX_LIGHTSENSOR_VR) // VR
        return MainIcon4C_1_BrightnessSub+3*(6*2);
    else // OSD
        return MainIcon4C_1_BrightnessSub+0*(6*2);
}
WORD DrawOsdBrightnessOffset(void)
{
    if(UserprefLITEMAX_LIGHTSENSOR == LITEMAX_LIGHTSENSOR_LS) // Offset
        return MainIcon4C_1_BrightnessSub+5*(6*2);
    else // Brightness
        return MainIcon4C_1_BrightnessSub+1*(6*2);
}
#endif
//=========================================================================
// draw menu item display text
void DrawOsdMenuItemText( BYTE itemIndex, const MenuItemType *menuItem )
{
	if( menuItem->DisplayText == NULL && menuItem->DrawItemMethod != DWI_Icon)
    {
        return ;
    }
#if	LiteMAX_OSD_TEST
	if( menuItem->DrawItemMethod == DWI_Icon )
	{
		printf("\r\n DWI_Icon");
		printData("MainIcon4C_0_MainMenuIcon = %x", MainIcon4C_0_MainMenuIcon);
		printData("MenuPageIndex = %d", MenuPageIndex);
		printData("itemIndex = %d", itemIndex);
		printData("MenuItemIndex = %d", MenuItemIndex);

		if (MenuPageIndex == MainMenu)
		{
			if(MenuItemIndex == itemIndex)
            {
                if (itemIndex==MAIN_COLOR_ITEM)
                    OsdFontColor=FOUR_COLOR(12);
                else
                    OsdFontColor=FOUR_COLOR(10);
            }
			else
			{
				if(itemIndex == MAIN_SOUND_ITEM)
					OsdFontColor=FOUR_COLOR(7);
				else if(itemIndex == MAIN_COLOR_ITEM)
					OsdFontColor=FOUR_COLOR(8);
				else if(itemIndex == MAIN_LUMINANCE_ITEM)
					OsdFontColor=FOUR_COLOR(6);
				else if(itemIndex == MAIN_SIGNAL_ITEM)
					OsdFontColor=FOUR_COLOR(6);
				else if(itemIndex == MAIN_IMAGE_ITEM)
					if (!CURRENT_INPUT_IS_VGA())
                    OsdFontColor = FOUR_COLOR(15);
					else
					OsdFontColor=FOUR_COLOR(6);
				else if(itemIndex == MAIN_OTHER_ITEM)
					OsdFontColor=FOUR_COLOR(6);
				else if(itemIndex == MAIN_EXIT_ITEM)
					OsdFontColor=FOUR_COLOR(6);
			}
			DrawOsdIcon( (MainMenuIcon_X_Start+(itemIndex*4)), MainMenuIcon_Y_Start, MainIcon4C_0_MainMenuIcon+(itemIndex *(6*2)));
			
			if(MenuItemIndex == LUMINANCE_SUB_ITEM)
        	{
        		OsdFontColor=FOUR_COLOR(6);
#if 0//BrightnessLightSensorVR
        		if(UserprefLITEMAX_LIGHTSENSOR == LITEMAX_LIGHTSENSOR_VR)
            	{
            		DrawOsdIcon( 8,SubMenuIcon_Y_Start,DrawOsdBrightnessType());
                	DrawOsdIcon(14,SubMenuIcon_Y_Start,MainIcon4C_1_BrightnessSub+2*(6*2));
                	DrawOsdIcon(20,SubMenuIcon_Y_Start,MainIcon4C_0_MainMenu6Icon+5*(6*2));
            	}
            	else
            	{
                	DrawOsdIcon( 5,SubMenuIcon_Y_Start,DrawOsdBrightnessType());
                	DrawOsdIcon(11,SubMenuIcon_Y_Start,DrawOsdBrightnessOffset());
                	DrawOsdIcon(17,SubMenuIcon_Y_Start,MainIcon4C_1_BrightnessSub+2*(6*2));
                	DrawOsdIcon(23,SubMenuIcon_Y_Start,MainIcon4C_0_MainMenu6Icon+5*(6*2));
            	}
#else
            	DrawOsdIcon( 5, SubMenuIcon_Y_Start, MainIcon4C_1_BrightnessSub+0*(6*2));
            	DrawOsdIcon(11, SubMenuIcon_Y_Start, MainIcon4C_1_BrightnessSub+1*(6*2));
            	DrawOsdIcon(17, SubMenuIcon_Y_Start, MainIcon4C_1_BrightnessSub+2*(6*2));
            	DrawOsdIcon(23, SubMenuIcon_Y_Start, MainIcon4C_0_MainMenuIcon+MAIN_EXIT_ITEM*(6*2));
#endif			
			}
			if(MenuItemIndex == SIGNAL_SUB_ITEM)
        	{
        		OsdFontColor = (CPC_Yellow<<4|CPC_Black);
				#if 0
            	Osd_DrawPropStr( 4, SubMenuIcon_Y_Start+1, AutoSelectText());
				#endif
				Osd_DrawPropStr( 4, SubMenuIcon_Y_Start, AutoSelectText());
                Osd_DrawPropStr( 9, SubMenuIcon_Y_Start, VGAText());
                Osd_DrawPropStr(14, SubMenuIcon_Y_Start, DigitalPort0Text());
                Osd_DrawPropStr(19, SubMenuIcon_Y_Start, DigitalPort1Text());
                Osd_DrawPropStr(24, SubMenuIcon_Y_Start, ExitText());
			}
			else if(MenuItemIndex == SOUND_SUB_ITEM)
            {
                OsdFontColor=FOUR_COLOR(6);
                DrawOsdIcon( 8, SubMenuIcon_Y_Start, MainIcon4C_2_AudioSub+0*(6*2));
                if(1) //(UserPrefOSDSoundMute)
                {
					#if 0//BrightnessLightSensorVR
                    DrawOsdIcon(14, SubMenuIcon_Y_Start, MainIcon4C_2_AudioSub+1*(6*2));
					#else
					OsdFontColor=FOUR_COLOR(7);
                    DrawOsdIcon(14, SubMenuIcon_Y_Start, MainIcon4C_2_AudioSub+1*(6*2));
					#endif
                }
                else
                {
					OsdFontColor=FOUR_COLOR(6);
                    DrawOsdIcon(14, SubMenuIcon_Y_Start, MainIcon4C_0_MainMenuIcon+MAIN_SOUND_ITEM*(6*2));
                }
				OsdFontColor=FOUR_COLOR(6);
                DrawOsdIcon(20, SubMenuIcon_Y_Start, MainIcon4C_0_MainMenuIcon+MAIN_EXIT_ITEM*(6*2));
            }
			else if(MenuItemIndex == COLOR_SUB_ITEM)
            {
            	OsdFontColor=FOUR_COLOR(6);
				DrawOsdIcon(23, SubMenuIcon_Y_Start, MainIcon4C_0_MainMenuIcon+MAIN_EXIT_ITEM*(6*2));
				if (!CURRENT_INPUT_IS_VGA())
                    OsdFontColor = FOUR_COLOR(15);
				DrawOsdIcon( 5, SubMenuIcon_Y_Start, MainIcon4C_3_ColorSub+5*(6*2));
				OsdFontColor = FOUR_COLOR(8);
            	#if 0//MaxRGBPanel
                DrawOsdIcon(11, SubMenuIcon_Y_Start, MainIcon4C_3_ColorSub+4*(6*2));
            	#else
                DrawOsdIcon(11, SubMenuIcon_Y_Start, MainIcon4C_3_ColorSub+1*(6*2));
            	#endif
                DrawOsdIcon(17, SubMenuIcon_Y_Start, MainIcon4C_3_ColorSub+2*(6*2));
            }
			else if(MenuItemIndex == IMAGE_SUB_ITEM)
            {
                OsdFontColor=FOUR_COLOR(6);
				DrawOsdIcon( 4, SubMenuIcon_Y_Start, MainIcon4C_3_ColorSub+5*(6*2));
                DrawOsdIcon( 8, SubMenuIcon_Y_Start, MainIcon4C_4_ImageSub+0*(6*2));
				DrawOsdIcon(12, SubMenuIcon_Y_Start, MainIcon4C_0_MainMenuIcon+MAIN_IMAGE_ITEM*(6*2));
				DrawOsdIcon(16, SubMenuIcon_Y_Start, MainIcon4C_4_ImageSub+1*(6*2));
				DrawOsdIcon(20, SubMenuIcon_Y_Start, MainIcon4C_4_ImageSub+2*(6*2));
				DrawOsdIcon(24, SubMenuIcon_Y_Start, MainIcon4C_0_MainMenuIcon+MAIN_EXIT_ITEM*(6*2));
            }
			else if(MenuItemIndex == OTHER_SUB_ITEM)
            {
                OsdFontColor=FOUR_COLOR(6);
                DrawOsdIcon( 5, SubMenuIcon_Y_Start, MainIcon4C_5_OtherSub+0*(6*2));
				DrawOsdIcon(11, SubMenuIcon_Y_Start, MainIcon4C_5_OtherSub+1*(6*2));
				DrawOsdIcon(17, SubMenuIcon_Y_Start, MainIcon4C_5_OtherSub+2*(6*2));
				DrawOsdIcon(23, SubMenuIcon_Y_Start, MainIcon4C_0_MainMenuIcon+MAIN_EXIT_ITEM*(6*2));
            }
		}
		else if( MenuPageIndex == LuminanceMenu )
        {
			if(MenuItemIndex == itemIndex)
            {
                OsdFontColor = FOUR_COLOR(10);
            }
            else
            {
                OsdFontColor = FOUR_COLOR(6);
            }
			#if 0 //BrightnessLightSensorVR
            if(UserprefLITEMAX_LIGHTSENSOR == LITEMAX_LIGHTSENSOR_VR)
            {
                if(itemIndex == 0)
                    DrawOsdIcon( 8, SubMenuIcon_Y_Start, DrawOsdBrightnessType());
                else if(itemIndex == 2)
                    DrawOsdIcon(14, SubMenuIcon_Y_Start, MainIcon4C_1_BrightnessSub+2*(6*2));
                else if(itemIndex == 3)
                    DrawOsdIcon(20, SubMenuIcon_Y_Start, MainIcon4C_0_MainMenu6Icon+5*(6*2));
            }
            else
            {
                if(itemIndex == 0)
                    DrawOsdIcon( 5, SubMenuIcon_Y_Start, DrawOsdBrightnessType());
                else if(itemIndex == 1)
                    DrawOsdIcon(11, SubMenuIcon_Y_Start, DrawOsdBrightnessOffset());
                else if(itemIndex == 2)
                    DrawOsdIcon(17, SubMenuIcon_Y_Start, MainIcon4C_1_BrightnessSub+2*(6*2));
                else if(itemIndex == 3)
                    DrawOsdIcon(23, SubMenuIcon_Y_Start, MainIcon4C_0_MainMenu6Icon+5*(6*2));
            }
			#else
            if(itemIndex == 0)
                DrawOsdIcon( 5, SubMenuIcon_Y_Start, MainIcon4C_1_BrightnessSub+0*(6*2));
            else if(itemIndex == 1)
                DrawOsdIcon(11, SubMenuIcon_Y_Start, MainIcon4C_1_BrightnessSub+1*(6*2));
            else if(itemIndex == 2)
                DrawOsdIcon(17, SubMenuIcon_Y_Start, MainIcon4C_1_BrightnessSub+2*(6*2));
            else if(itemIndex == 3)
                DrawOsdIcon(23, SubMenuIcon_Y_Start, MainIcon4C_0_MainMenuIcon+MAIN_EXIT_ITEM*(6*2));
			#endif
		}
		else if( MenuPageIndex == SoundMenu )
        {
            if(MenuItemIndex == itemIndex)
            {
                if(itemIndex==2)
                    OsdFontColor = FOUR_COLOR(10);
                else
                    OsdFontColor = FOUR_COLOR(11);
            }
            else
            {
                if(itemIndex==2)
                    OsdFontColor = FOUR_COLOR(6);
                else
                    OsdFontColor = FOUR_COLOR(7);
            }
            if(itemIndex == 0)
                DrawOsdIcon( 8, SubMenuIcon_Y_Start, MainIcon4C_2_AudioSub+0*(6*2));
            else if(itemIndex == 1)
            {
                if (1)//(UserPrefOSDSoundMute)
					#if 0//BrightnessLightSensorVR
                    DrawOsdIcon(14, SubMenuIcon_Y_Start, MainIcon4C_2_SoundSub+1*(6*2));
					#else
                    DrawOsdIcon(14, SubMenuIcon_Y_Start, MainIcon4C_2_AudioSub+1*(6*2));
					#endif
                else
                    DrawOsdIcon(14, SubMenuIcon_Y_Start, MainIcon4C_0_MainMenuIcon+SOUND_SUB_ITEM*(6*2));
            }
            else if(itemIndex == 2)
                DrawOsdIcon(20, SubMenuIcon_Y_Start, MainIcon4C_0_MainMenuIcon+MAIN_EXIT_ITEM*(6*2));
        }
		else if( MenuPageIndex == ColorMenu )
        {
            if(MenuItemIndex == itemIndex)
            {
                if((itemIndex==0)||(itemIndex==3))
                    OsdFontColor = FOUR_COLOR(10);
                else
                    OsdFontColor = FOUR_COLOR(12);
            }
            else
            {
                if((itemIndex==0)||(itemIndex==3))
                    OsdFontColor = FOUR_COLOR(6);
                else
                    OsdFontColor = FOUR_COLOR(8);
            }
        #if 1//EnableIconDisableColor&&(EnableAutoColorIconDisableColor == 0)
            if ((itemIndex==0)&&(!CURRENT_INPUT_IS_VGA()))
                OsdFontColor = FOUR_COLOR(15);
        #endif

			if(itemIndex == 0)
                DrawOsdIcon( 5, SubMenuIcon_Y_Start, MainIcon4C_3_ColorSub+5*(6*2));
            else if(itemIndex == 1)
        		#if 0//MaxRGBPanel
                DrawOsdIcon(11, SubMenuIcon_Y_Start, MainIcon4C_3_ColorSub+4*(6*2));
        		#else
                DrawOsdIcon(11, SubMenuIcon_Y_Start, MainIcon4C_3_ColorSub+1*(6*2));
        		#endif
            else if(itemIndex == 2)
                DrawOsdIcon(17, SubMenuIcon_Y_Start, MainIcon4C_3_ColorSub+2*(6*2));
            else if(itemIndex == 3)
                DrawOsdIcon(23, SubMenuIcon_Y_Start, MainIcon4C_0_MainMenuIcon+MAIN_EXIT_ITEM*(6*2));
        }
		else if( MenuPageIndex == ImageMenu )
        {
            if(MenuItemIndex == itemIndex)
            {
                OsdFontColor = FOUR_COLOR(10);
            }
            else
            {
                OsdFontColor = FOUR_COLOR(6);
            }
            if(itemIndex == 0)
                DrawOsdIcon( 4, SubMenuIcon_Y_Start, MainIcon4C_3_ColorSub+5*(6*2));
            else if(itemIndex == 1)
                DrawOsdIcon(8, SubMenuIcon_Y_Start, MainIcon4C_4_ImageSub+0*(6*2));
            else if(itemIndex == 2)
                DrawOsdIcon(12, SubMenuIcon_Y_Start, MainIcon4C_0_MainMenuIcon+MAIN_IMAGE_ITEM*(6*2));
			else if(itemIndex == 3)
                DrawOsdIcon(16, SubMenuIcon_Y_Start, MainIcon4C_4_ImageSub+1*(6*2));
			else if(itemIndex == 4)
                DrawOsdIcon(20, SubMenuIcon_Y_Start, MainIcon4C_4_ImageSub+2*(6*2));
            else if(itemIndex == 5)
                DrawOsdIcon(24, SubMenuIcon_Y_Start, MainIcon4C_0_MainMenuIcon+MAIN_EXIT_ITEM*(6*2));
        }
		else if( MenuPageIndex == ToolMenu )
        {
            if(MenuItemIndex == itemIndex)
            {
                //if(itemIndex==2) //for sharpness
                //    OsdFontColor = FOUR_COLOR(13);
                //else
                    OsdFontColor = FOUR_COLOR(10);
            }
            else
            {
                //if(itemIndex==2) //for sharpness
                //    OsdFontColor = FOUR_COLOR(9);
                //else
                    OsdFontColor = FOUR_COLOR(6);
            }
            if(itemIndex == 0)
                DrawOsdIcon( 5, SubMenuIcon_Y_Start, MainIcon4C_5_OtherSub+0*(6*2));
            else if(itemIndex == 1)
                DrawOsdIcon(11, SubMenuIcon_Y_Start, MainIcon4C_5_OtherSub+1*(6*2));
            else if(itemIndex == 2)
                DrawOsdIcon(17, SubMenuIcon_Y_Start, MainIcon4C_5_OtherSub+2*(6*2));
            else if(itemIndex == 3)
                DrawOsdIcon(23, SubMenuIcon_Y_Start, MainIcon4C_0_MainMenuIcon+MAIN_EXIT_ITEM*(6*2));
        }
		else if( MenuPageIndex == BrightnessMenu )
        {
            OsdFontColor = FOUR_COLOR(6);
			#if 0//BrightnessLightSensorVR
            DrawOsdIcon(14, SubMenuIcon_Y_Start, DrawOsdBrightnessOffset());// jason 20190701 //DrawOsdIcon(14,10,DrawOsdBrightnessType());
			#else
            DrawOsdIcon(14, SubMenuIcon_Y_Start, MainIcon4C_1_BrightnessSub+1*(6*2));
			#endif
        }
        else if( MenuPageIndex == ContrastMenu )
        {
            OsdFontColor = FOUR_COLOR(6);
            DrawOsdIcon(14, SubMenuIcon_Y_Start, MainIcon4C_1_BrightnessSub+2*(6*2));
        }
        else if( MenuPageIndex == SoundAdjustMenu )
        {
            OsdFontColor = FOUR_COLOR(7);
            DrawOsdIcon(14, SubMenuIcon_Y_Start, MainIcon4C_2_AudioSub+0*(6*2));
        }
        else if(MenuPageIndex==ColorTempMenu)
        {
            if(MenuItemIndex == itemIndex)
            {
                if(itemIndex>=1)
                    OsdFontColor = FOUR_COLOR(10);
                else
                    OsdFontColor = FOUR_COLOR(12);
            }
            else
            {
                if(itemIndex>=1)
                    OsdFontColor = FOUR_COLOR(6);
                else
                    OsdFontColor = FOUR_COLOR(8);
            }
            if(itemIndex == 0)
                DrawOsdIcon( 5, SubMenuIcon_Y_Start, MainIcon4C_3_ColorSub+3*(6*2));
            else if(itemIndex == 1)
                DrawOsdIcon(11, SubMenuIcon_Y_Start, MainIcon4C_3_ColorSub+6*(6*2));
            else if(itemIndex == 2)
                DrawOsdIcon(17, SubMenuIcon_Y_Start, MainIcon4C_3_ColorSub+7*(6*2));
            else if(itemIndex == 3)
                DrawOsdIcon(23, SubMenuIcon_Y_Start, MainIcon4C_0_MainMenuIcon+MAIN_EXIT_ITEM*(6*2));
        }
        else if( MenuPageIndex == SharpnessMenu )
        {
            OsdFontColor = FOUR_COLOR(9);
            DrawOsdIcon(14, SubMenuIcon_Y_Start, MainIcon4C_5_OtherSub+3*(6*2));
        }
        else if( MenuPageIndex == OSDControlMenu )
        {
            if(MenuItemIndex == itemIndex)
            {
                    OsdFontColor = FOUR_COLOR(10);
            }
            else
            {
                    OsdFontColor = FOUR_COLOR(6);
            }
#if ENABLE_OSD_ROTATION
            if(itemIndex == 0)
                DrawOsdIcon( 4, SubMenuIcon_Y_Start, MainIcon4C_5_OtherSub+2*(6*2));
            else if(itemIndex == 1)
                DrawOsdIcon( 9, SubMenuIcon_Y_Start, MainIcon4C_5_OtherSub+3*(6*2));
            else if(itemIndex == 2)
                DrawOsdIcon(14, SubMenuIcon_Y_Start, MainIcon4C_5_OtherSub+4*(6*2));
            else if(itemIndex == 3)
                DrawOsdIcon(19, SubMenuIcon_Y_Start, MainIcon4C_5_OtherSub+5*(6*2));
            else if(itemIndex == 4)
                DrawOsdIcon(24, SubMenuIcon_Y_Start, MainIcon4C_0_MainMenuIcon+MAIN_EXIT_ITEM*(6*2));
#else
            if(itemIndex == 0)
                DrawOsdIcon( 5, SubMenuIcon_Y_Start, MainIcon4C_5_OtherSub+3*(6*2));
            else if(itemIndex == 1)
                DrawOsdIcon(11, SubMenuIcon_Y_Start, MainIcon4C_5_OtherSub+4*(6*2));
            else if(itemIndex == 2)
                DrawOsdIcon(17, SubMenuIcon_Y_Start, MainIcon4C_5_OtherSub+5*(6*2));
            else if(itemIndex == 3)
                DrawOsdIcon(23, SubMenuIcon_Y_Start, MainIcon4C_0_MainMenuIcon+MAIN_EXIT_ITEM*(6*2));
#endif
        }
#if ENABLE_OSD_ROTATION
        else if( MenuPageIndex == OSDRotationMenu )
        {
            if(MenuItemIndex == itemIndex)
            {
                    OsdFontColor = 0x14+0x10;
            }
            else
            {
                    OsdFontColor = 0x14;
            }
            if(itemIndex == 0)
                DrawOsdIcon( 4, SubMenuIcon_Y_Start, MainIcon4C_OSDRotate+0*(6*2));
            else if(itemIndex == 1)
                DrawOsdIcon( 9, SubMenuIcon_Y_Start, MainIcon4C_OSDRotate+1*(6*2));
            else if(itemIndex == 2)
                DrawOsdIcon(14, SubMenuIcon_Y_Start, MainIcon4C_OSDRotate+2*(6*2));
            else if(itemIndex == 3)
                DrawOsdIcon(19, SubMenuIcon_Y_Start, MainIcon4C_OSDRotate+3*(6*2));
            else if(itemIndex == 4)
                DrawOsdIcon(24, SubMenuIcon_Y_Start, MainIcon4C_0_MainMenuIcon+MAIN_EXIT_ITEM*(6*2));
        }
#endif
        else if( MenuPageIndex == OSDTimeMenu )
        {
            OsdFontColor = FOUR_COLOR(6);
            DrawOsdIcon(14, SubMenuIcon_Y_Start, MainIcon4C_5_OtherSub+3*(6*2));
        }
        else if( MenuPageIndex == OSDHPosMenu )
        {
            OsdFontColor = FOUR_COLOR(6);
            DrawOsdIcon(14, SubMenuIcon_Y_Start, MainIcon4C_5_OtherSub+4*(6*2));
        }
        else if( MenuPageIndex == OSDVPosMenu )
        {
            OsdFontColor = FOUR_COLOR(6);
            DrawOsdIcon(14, SubMenuIcon_Y_Start, MainIcon4C_5_OtherSub+5*(6*2));
        }
        else if( MenuPageIndex == ImageClockMenu )
        {
            OsdFontColor = FOUR_COLOR(6);
            DrawOsdIcon(14, SubMenuIcon_Y_Start, MainIcon4C_4_ImageSub+0*(6*2));
        }
        else if( MenuPageIndex == ImagePhaseMenu )
        {
            OsdFontColor = FOUR_COLOR(6);
            DrawOsdIcon(14, SubMenuIcon_Y_Start, MainIcon4C_0_MainMenuIcon+MAIN_IMAGE_ITEM*(6*2));
        }
        else if( MenuPageIndex == ImageHPosMenu )
        {
            OsdFontColor = FOUR_COLOR(6);
            DrawOsdIcon(14, SubMenuIcon_Y_Start, MainIcon4C_4_ImageSub+1*(6*2));
        }
        else if( MenuPageIndex == ImageVPosMenu )
        {
            OsdFontColor = FOUR_COLOR(6);
            DrawOsdIcon(14, SubMenuIcon_Y_Start, MainIcon4C_4_ImageSub+2*(6*2));
        }
        else if( MenuPageIndex == HotKeyBrightnessMenu )
        {
            OsdFontColor = FOUR_COLOR(6);
          #if 0//BrightnessLightSensorVR
            DrawOsdIcon( 9, 1,DrawOsdBrightnessOffset());
          #else
            DrawOsdIcon( 9, 1,MainIcon4C_1_BrightnessSub+1*(6*2));
          #endif
        }
	}
	else
#endif
    if( menuItem->DrawItemMethod == DWI_Text )
    {
        if( MenuPageIndex == FactoryMenu )
        {
            Osd_DrawStr( menuItem->XPos, menuItem->YPos, menuItem->DisplayText() );
        }
        else
        {
            if( MenuPageIndex == HotInputSelectMenu )
            {
                if (UserPrefInputPriorityType == itemIndex)
                    Osd_DrawCharDirect(menuItem->XPos, menuItem->YPos, Mark_2C);
                Osd_DrawPropStr( menuItem->XPos+2, menuItem->YPos, menuItem->DisplayText() );
            }
            else
                Osd_DrawPropStr( menuItem->XPos, menuItem->YPos, menuItem->DisplayText() );
        }
    }
    else
    {
        BYTE xPos=0;
#define len itemIndex
        if( menuItem->DrawItemMethod == DWI_FullText )
        {
            xPos = menuItem->XPos;
            Osd_DrawStr( xPos, menuItem->YPos, menuItem->DisplayText() );
        }
        else if( menuItem->DrawItemMethod == DWI_CenterText )
        {
            if( MenuPageIndex == FactoryMenu )
            {
                Osd_DrawStr( xPos, menuItem->YPos, menuItem->DisplayText() );
            }
            else
            {
                xPos=(OsdWindowWidth-(*( menuItem->DisplayText() + 1 )))/2;
                Osd_DrawPropStr( xPos, ( menuItem->YPos ), menuItem->DisplayText() );
            }
        }
#undef len
    }
}
//=========================================================================
// draw menu item display value
void DrawOsdMenuItemValue( BYTE itemIndex, const DrawValueType *valueItem )
{
    //printData("value address %x", (WORD)valueItem);
    if( valueItem->DrawNumber )
    {
        DrawOsdMenuItemNumber( itemIndex, valueItem->DrawNumber );
    }
    if( valueItem->DrawGuage )
    {
        DrawOsdMenuItemGuage( itemIndex, valueItem->DrawGuage );
    }
    if( valueItem->DrawRadioGroup )
    {
        DrawOsdMenuItemRadioGroup( itemIndex, valueItem->DrawRadioGroup );
    }
}
//=========================================================================
// draw number
void DrawOsdMenuItemNumber( BYTE itemIndex, const DrawNumberType *numberItem )
{
    itemIndex = 0;
#if Multi_Number
    {
        const NumberType *number;
        BYTE i = 0;
        number = numberItem->DisplayNumber;

        if( number )
        {
#define drawItem    (number+i)
#define xPos itemIndex
            while( 1 )
            {
                if( drawItem->Flags & dwiNULL )
                {
                    break;
                }
                xPos = drawItem->XPos;

                if (Check_Disable_ItemRadioGroup_Status(drawItem->Flags))
                    OsdFontColor = (CPC_Gray<<4|CPC_Black);
                else
                {
                	#if 0
                    if (Layer2_MenuPage)
                        OsdFontColor = (CPC_White<<4|CPC_Black);
                    else if (Layer3_MenuPage)
                        OsdFontColor = (CPC_MTK_Gold<<4|CPC_Black);
                    else
					#endif
                        OsdFontColor = ( numberItem->ForeColor & 0xF ) << 4 | ( numberItem->BackColor & 0xF );
                }

                if( drawItem->Flags & dwiHex )
                {
                    Osd_DrawHex( xPos, drawItem->YPos, drawItem->GetValue() );
                }
                else
                {
                    if( MenuPageIndex == FactoryMenu )
                        Osd_DrawFactoryNum( xPos, drawItem->YPos, drawItem->GetValue() );
                    else
                        Osd_DrawNum( xPos, drawItem->YPos, drawItem->GetValue() );
                }
                if( drawItem->Flags & dwiEnd )
                {
                    break;
                }
                i++;
            }
#undef xPos
#undef drawItem
        }
    }
#else
#define xPos itemIndex
    xPos = numberItem->XPos;
    if( xPos == 0 )
    {
        xPos = ( OsdWindowWidth - 4 ) / 2 - 1;
    }
    if( FactoryModeFlag && MenuPageIndex == FactoryMenu )
    {
        if( CurrentMenuItem.DisplayValue.DrawNumber == numberItem )
        {
    Osd_SetTextColor( numberItem->ForeColor, numberItem->BackColor );
        }
        else
        {
            Osd_SetTextColor( CP_WhiteColor, CP_BlueColor );
        }
    }
    else
    {
        Osd_SetTextColor( numberItem->ForeColor, numberItem->BackColor );
    }
    if( numberItem->Flags & dwiHex )
    {
        Osd_DrawHex( numberItem->XPos, numberItem->YPos, numberItem->GetValue() );
    }
    else
    {
        Osd_DrawNum( numberItem->XPos, numberItem->YPos, numberItem->GetValue() );
    }
#undef xPos
#endif
}
//=========================================================================
// draw guage
void DrawOsdMenuItemGuage( BYTE itemIndex, const DrawGuageType *gaugeItem )
{
    //BYTE yPos;
    //itemIndex = 0;
#if Multi_Gauge
	#if LiteMAX_OSD_TEST
	{
        BYTE i;
        const GaugeType *gauge;
        gauge = gaugeItem->DisplayGauge;


        if( gauge )
        {
            i = 0;
#define drawItem    (gauge+i)
            while( 1 )
            {
                BYTE u8Value;
                u8Value = (drawItem->GetValue()%101)/10;
//printData("Guage MenuItemIndex=%d",MenuItemIndex);
//printData("Guage itemIndex=%d",itemIndex);
                if (Check_Disable_ItemRadioGroup_Status(drawItem->Flags))
                    OsdFontColor = (CPC_Gray<<4|CPC_Black);
                else if( MenuPageIndex == ColorSettingsMenu && MenuItemIndex == itemIndex)
                    OsdFontColor = ( gaugeItem->ForeColor & 0xF ) << 4 | CPC_Yellow;
                else if( MenuPageIndex == FactoryMenu && MenuItemIndex == itemIndex)
                {
                    if( CurrentMenuItem.Flags & mibDrawValue)
                        OsdFontColor = ( gaugeItem->ForeColor & 0xF ) << 4 | CP_BlackColor;
                    else
                        OsdFontColor = ( gaugeItem->ForeColor & 0xF ) << 4 | CP_LightColor;
                }
                else
                    OsdFontColor = ( gaugeItem->ForeColor & 0xF ) << 4 | ( gaugeItem->BackColor & 0xF );
                Osd_DrawGuage( drawItem->XPos, drawItem->YPos, gaugeItem->Length, drawItem->GetValue() );

                if( drawItem->Flags & dwiEnd )
                {
                    break;
                }
                i++;
            }
#undef drawItem
        }
    }
	#else
    {
        BYTE i;
        const GaugeType *gauge;
        gauge = gaugeItem->DisplayGauge;


        if( gauge )
        {
            i = 0;
#define drawItem    (gauge+i)
#define xPos    itemIndex
            while( 1 )
            {
                if( drawItem->Flags & dwiNULL )
                {
                    break;
                }
                BYTE u8Value;
                u8Value = (drawItem->GetValue()%101)/10;

                xPos = drawItem->XPos;

                if (Check_Disable_ItemRadioGroup_Status(drawItem->Flags))
                    OsdFontColor = (CPC_Gray<<4|CPC_Black);
                else
                    OsdFontColor=(CPC_White<<4|CPC_Black);
                Osd_DrawContinuesChar(xPos+u8Value, drawItem->YPos, GaugeNormal_2C, 10-u8Value);

                if (Check_Disable_ItemRadioGroup_Status(drawItem->Flags))
                    OsdFontColor = (CPC_Gray<<4|CPC_Black);
                else
                {
                    if (Layer3_MenuPage)
                        OsdFontColor = (CPC_MTK_Gold<<4|CPC_Black);
                    else
                        OsdFontColor=(CPC_White<<4|CPC_Black);
                }
                Osd_DrawContinuesChar(xPos, drawItem->YPos, GaugeSelect_2C, u8Value);
                //Osd_DrawGuage( xPos - 1, drawItem->YPos, gaugeItem->Length, drawItem->GetValue() );

                if( drawItem->Flags & dwiEnd )
                {
                    break;
                }
                i++;
            }
#undef xPos
#undef drawItem
        }
    }
	#endif
#else
#define xPos    itemIndex
    xPos = gaugeItem->XPos;
    //yPos=gaugeItem->YPos;
    if( xPos == 0 )
    {
        xPos = ( OsdWindowWidth - gaugeItem->Length ) / 2;
    }
    if( !( CurrentMenu.Flags & mpbAdjust ) )
    {
        Osd_SetTextColor( gaugeItem->ForeColor, gaugeItem->BackColor );
    }
    else
    {
        Osd_SetTextColor( CP_RedColor, CP_WhiteColor );
    }
    Osd_DrawGuage( gaugeItem->XPos, gaugeItem->YPos, gaugeItem->Length, gaugeItem->GetValue() );
#undef xPos
#endif
}
//=========================================================================
// draw radio
void DrawOsdMenuItemRadioGroup( BYTE itemIndex, const DrawRadioGroupType *radioItem )
{
    const RadioTextType *radioText;

    radioText = radioItem->RadioText;
    if( radioText )
    {
        BYTE i = 0;
        BYTE xPos;
        BYTE center_flag;
        center_flag = 0;
        itemIndex = 0xFF;
        if( radioItem->GetValue )
        {
            itemIndex = radioItem->GetValue();
        }
#define drawItem    (radioText+i)
        while( 1 )
        {
            if( drawItem->DisplayText )
            {
                xPos = drawItem->XPos;

                if (Check_Disable_ItemRadioGroup_Status(drawItem->Flags))
                    OsdFontColor = (CPC_Gray<<4|CPC_Black);
                else
                    OsdFontColor = ( radioItem->ForeColor & 0xF ) << 4 | ( radioItem->BackColor & 0xF );

                if( MenuPageIndex == FactoryMenu
                )
                {
                    Osd_DrawStr( xPos, drawItem->YPos, drawItem->DisplayText() );
                }
                else
                {
                    Osd_DrawPropStr( xPos, drawItem->YPos, drawItem->DisplayText() );
                }
            }
            if( drawItem->Flags & dwiEnd )
            {
                break;
            }
            i++;
        }
#undef drawItem
    }
}


#if 0
Bool DrawTimingInfo( void )
{
    WORD freq;
    WORD HDisplay;
    #if ENABLE_DP_INPUT
    WORD VDisplay;
    #endif
    BYTE TimingXPos, TimingYPos;

    TimingXPos = 21;
    TimingYPos = (SUB_TEXTINFO_YPOS+EXTRAMENUINFO_RESOLUTION_ITEM);

    Osd_SetTextColor( 14, Color_2 );
    /***********************Show Timing Resolution Information******************************/
    #if ENABLE_DP_INPUT  //111027 Rick add for display DP InputDE on information menu - B39608
    if(CURRENT_INPUT_IS_DISPLAYPORT())//(SrcInputType==Input_Displayport)
     {
            extern Bool DPRxGetInputDE(WORD* pHDE, WORD* pVDE);
            DPRxGetInputDE(&HDisplay, &VDisplay);
             Osd_Draw4Num( TimingXPos, TimingYPos, HDisplay );
             Osd_Draw4Num( TimingXPos + 8, TimingYPos, VDisplay);
      }
    else
    #endif
    {
        HDisplay = SC0_READ_IMAGE_WIDTH();//old_msRead2Byte( SC0_0B ) & SC_MASK_H;
        if( abs( HDisplay - 720 ) < 5 )
            HDisplay = 720;
        else if( abs( HDisplay - 1280 ) < 5 ) //091218
            HDisplay = 1280;
        else if( abs( HDisplay - 1920 ) < 5 )
            HDisplay = 1920;
        else if(( HDisplay == 1366 ) && ( StandardModeGroup == Res_1360x768 ) )
            HDisplay = 1360;
        Osd_Draw4Num( TimingXPos, TimingYPos, HDisplay );
        Osd_DrawPropStr( TimingXPos + 6, TimingYPos, HResText() );

#if 0
        if( StandardModeGroup == Res_640x350 )
        {
            freq = old_msRead2Byte(SC0_82)-old_msRead2Byte(SC0_7E) + 1 ; // get DE width      //110929 Rick modified information display error - A040
            freq = ( freq / 10 ) * 10;
            Osd_Draw4Num( TimingXPos + 8, TimingYPos, freq & 0xfffe );
        }
        else
#endif
        {
            if( StandardModeGroup == Res_640x350 )              //110929 Rick modified information display error - A040
                Osd_Draw4Num( TimingXPos + 8, TimingYPos, 350 );
            else if( StandardModeGroup == Res_1920x1080 ) //&&(SrcFlags&bInterlaceMode))
                Osd_Draw4Num( TimingXPos + 8, TimingYPos, 1080 );
            else if( abs( SrcVTotal - 525 ) < 3 )
                Osd_Draw4Num( TimingXPos + 8, TimingYPos, 480 );
            else if( StandardModeGroup == Res_720x576 )
                Osd_Draw4Num( TimingXPos + 8, TimingYPos, 576 );
            else if( StandardModeGroup == Res_1280x720 )
                Osd_Draw4Num( TimingXPos + 8, TimingYPos, 720 );
            else if( StandardModeGroup == Res_720x480 && ( CURRENT_SOURCE_IS_INTERLACE_MODE() ) && !CURRENT_INPUT_IS_VGA())//SrcInputType >= Input_Digital )
                Osd_Draw4Num( TimingXPos + 8, TimingYPos, 576 );
            // 091218 coding, information error in DVI 640x350 70Hz
            else if( SrcInputType >= Input_Digital && !( CURRENT_SOURCE_IS_INTERLACE_MODE() ) )
                //Osd_Draw4Num( TimingXPos + 8, TimingYPos, freq = old_msRead2Byte(SC0_82)-old_msRead2Byte(SC0_7E)+1 );       //110915 Rick Modified - B012
                Osd_Draw4Num( TimingXPos + 8, TimingYPos, freq = SC0_READ_AUTO_HEIGHT());       //110915 Rick Modified - B012
            else
                Osd_Draw4Num( TimingXPos + 8, TimingYPos, SC0_READ_IMAGE_HEIGHT() );
        }
    }
        Osd_DrawPropStr( TimingXPos + 9, TimingYPos, X_Text() );
        Osd_DrawPropStr( TimingXPos + 14, TimingYPos, VResText() );
    /***********************************************************************************/
    //HFrequence=======================================
    freq = HFreq( SrcHPeriod );//(( DWORD )MST_CLOCK_MHZ * 10 + SrcHPeriod / 2 ) / SrcHPeriod; // round 5
    if( CURRENT_SOURCE_IS_INTERLACE_MODE() )
        freq *= 2;
    if(( freq % 10 ) >= 5 )
        //round off
    {
        Osd_DrawNum( TimingXPos + 2, TimingYPos + 1, ( freq + 10 ) / 10 );
    }
    else
    {
        Osd_DrawNum( TimingXPos + 2, TimingYPos + 1, freq / 10 );
    }
    Osd_DrawPropStr( TimingXPos + 6, TimingYPos + 1, KHzText() );
    //=================================================
    //VFrequence-----
    freq = (( DWORD )freq * 1000 ) / SrcVTotal;
    if(( freq % 10 ) >= 5 )        //round off          //111107 Rick modified to round off - A071
    {
        Osd_DrawNum( TimingXPos + 2, TimingYPos + 2, ( freq + 10 ) / 10 );
    }
    else
    {
        if( StandardModeGroup == Res_1280x1024 && abs( freq - 750 ) <= 10 )
            freq = 750;

        Osd_DrawNum( TimingXPos + 2, TimingYPos + 2, freq / 10 );
    }
    Osd_DrawPropStr( TimingXPos + 6, TimingYPos + 2, HzText() );
    return TRUE;
}
#endif
BYTE GetPrevItem( const MenuItemType *menuItem )
{
    signed char i;
    if( MenuItemIndex )
    {
        for( i = MenuItemIndex - 1; i >= 0; i-- )
        {
            if( Check_Disable_Item_Status ( menuItem[i].Flags ) )
            {
                continue;
            }

            if( menuItem[i].Flags & mibSelectable && !( menuItem[i].Flags & mibDrawValue ) )
            {
                return i;
            }
        }
    }

    for( i = MenuItemCount - 1; i >= 0; i-- )
    {
        if( Check_Disable_Item_Status ( menuItem[i].Flags ) )
        {
            continue;
        }

        if( menuItem[i].Flags & mibSelectable && !( menuItem[i].Flags & mibDrawValue ) )
        {
            return i;
        }
    }
    return MenuItemIndex;
}

BYTE GetNextItem( const MenuItemType *menuItem )
{
    BYTE i;
    if( MenuItemIndex < MenuItemCount - 1 )
        // current item is not last item.
    {
        for( i = MenuItemIndex + 1; i < MenuItemCount; i++ )
        {
            if( Check_Disable_Item_Status ( menuItem[i].Flags ) )
            {
                continue;
            }

            if( menuItem[i].Flags & mibSelectable && !( menuItem[i].Flags & mibDrawValue ) )
            {
                return i;
            }
        }
    }
    for( i = 0; i < MenuItemIndex; i++ )
    {
        if( Check_Disable_Item_Status ( menuItem[i].Flags ) )
        {
            continue;
        }

        if( menuItem[i].Flags & mibSelectable && !( menuItem[i].Flags & mibDrawValue ) )
        {
            return i;
        }
    }
    return MenuItemIndex;
}
// menuPageIndex => prev page index
// MenuPageIndex => current page index
BYTE GetMenuItemIndex( BYTE menuPageIndex )
{
#if 1//(LiteMAX_OSDtype==LiteMAX_OSD_standard)
	{
		if( MenuPageIndex == MainMenu )
		{
			if( menuPageIndex == LuminanceMenu )
			{
				return MAIN_LUMINANCE_ITEM;
			}
			else if( menuPageIndex == SignalMenu )
			{
				return MAIN_SIGNAL_ITEM;
			}
			else if( menuPageIndex == SoundMenu )
			{
				return MAIN_SOUND_ITEM;
			}
			else if( menuPageIndex == ColorMenu )
			{
				return MAIN_COLOR_ITEM;
			}
			else if( menuPageIndex == ImageMenu )
			{
				return MAIN_IMAGE_ITEM;
			}
			else if( menuPageIndex == ToolMenu )
			{
				return MAIN_OTHER_ITEM;
			}
		}
		else if( MenuPageIndex == LuminanceMenu )
		{
			if( menuPageIndex == BrightnessMenu )
				return 1;
			else if( menuPageIndex == ContrastMenu )
				return 2;
		}
		else if( MenuPageIndex == ImageMenu )
		{
			if( menuPageIndex == ImageClockMenu )
				return 1;//ImageClockMenu-ImageClockMenu+1;
			else if( menuPageIndex == ImagePhaseMenu )
				return 2;//ImagePhaseMenu-ImageClockMenu+1;
			else if( menuPageIndex == ImageHPosMenu )
				return 3;//ImageHPosMenu-ImageClockMenu+1;
			else if( menuPageIndex == ImageVPosMenu )
				return 4;//ImageVPosMenu-ImageClockMenu+1;
		}
		else if( MenuPageIndex == ToolMenu )
		{
			if( menuPageIndex == SharpnessMenu )
				return 2;
		}
		else if( MenuPageIndex == OSDControlMenu )
		{
			if( menuPageIndex == OSDTimeMenu )
				return 0;
			else if( menuPageIndex == OSDHPosMenu )
				return 1;
			else if( menuPageIndex == OSDVPosMenu )
				return 2;
#if ENABLE_OSD_ROTATION
			else if( menuPageIndex == OSDRotationMenu )
				return 3;
#endif
		}
		else if( MenuPageIndex == ColorTempMenu )
		{
			return UserPrefColorTemp;
		}
		else if(MenuPageIndex == ColorMenu)
		{
			if( menuPageIndex == ColorTempMenu )
				return 2;
			else if(!CURRENT_INPUT_IS_VGA())
				return 1;
		}
		else if(MenuPageIndex == SignalMenu)
		{
			if (CURRENT_INPUT_IS_VGA())
				return 1;
			#if 0
			else if(CURRENT_INPUT_IS_DVI())
				return 1;
			#endif
			else if(CURRENT_INPUT_IS_HDMI())
				return 2;
			else if(CURRENT_INPUT_IS_DISPLAYPORT())
				return 3;
			else
				return 4;
		}
		return 0;
	}
	
#else

    if( MenuPageIndex == MainMenu )
    {
        if( menuPageIndex == BriteContMenu )
        {
            return MainMenuItems_BriteCont;
        }
        else if( menuPageIndex == ColorSettingsMenu )
        {
            return MainMenuItems_ColorSettings;
        }
        else if( menuPageIndex == ExtColorSettingsMenu )
        {
            return MainMenuItems_ExtColorSettings;
        }
        else if( menuPageIndex == InputSourceMenu)
        {
            return MainMenuItems_InputSource;
        }
        else if( menuPageIndex == DisplaySettingsMenu )
        {
            return MainMenuItems_DisplaySettings;
        }
        else if( menuPageIndex == OtherSettingsMenu )
        {
            return MainMenuItems_OtherSettings;
        }
        else if( menuPageIndex == PowerManagerMenu )
        {
            return MainMenuItems_PowerManager;
        }

    }
    else if( MenuPageIndex == BriteContMenu )
    {
        if ( menuPageIndex == BrightnessMenu )
        {
            return BriteContMenuItems_Brightness;
        }
        else if ( menuPageIndex == ContrastMenu )
        {
            return BriteContMenuItems_Contrast;
        }
	#if ENABLE_DLC
        else if( menuPageIndex == DLCMenu )
        {
            return BriteContMenuItems_DLC;
        }
	#endif
    #if ENABLE_DPS
        else if( menuPageIndex == DPSMenu )
        {
            return BriteContMenuItems_DPS;
        }
    #endif
    #if ENABLE_DCR
        else if( menuPageIndex == DCRMenu )
        {
            return BriteContMenuItems_DCR;
        }
    #endif
    }
    else if( MenuPageIndex == DCRMenu )
    {
        if( UserPrefDcrMode )
        {
            return Items_On;
        }
    }
	#if ENABLE_DLC
    else if( MenuPageIndex == DLCMenu )
    {
        return UserprefDLCMode;
    }
	#endif
    #if ENABLE_DPS
    else if( MenuPageIndex == DPSMenu )
    {
        return UserprefDPSMode;
    }
    #endif
    else if( MenuPageIndex == ColorSettingsMenu )
    {
        if( menuPageIndex == ColorTempMenu )
        {
            return ColorSettingsMenuItems_ColorTemp;
        }
        else if( menuPageIndex == RedMenu )
        {
            return ColorSettingsMenuItems_Red;
        }
        else if( menuPageIndex == GreenMenu )
        {
            return ColorSettingsMenuItems_Green;
        }
        else if( menuPageIndex == BlueMenu )
        {
            return ColorSettingsMenuItems_Blue;
        }
        else if( menuPageIndex == HueMenu )
        {
            return ColorSettingsMenuItems_Hue;
        }
        else if( menuPageIndex == SaturationMenu )
        {
            return ColorSettingsMenuItems_Saturation;
        }
    #if ENABLE_SUPER_RESOLUTION
        else if( menuPageIndex == SuperResolutionMenu )
        {
            return ColorSettingsMenuItems_SuperResolution;
        }
    #endif
    #if ENABLE_COLORMODE_DEMO
        else if( menuPageIndex == ColorModeMenu )
        {
            return ColorSettingsMenuItems_ColorMode;
        }
    #endif
        else if( menuPageIndex == ColorFormatMenu )
        {
            return ColorSettingsMenuItems_ColorFormat;
        }
    }
    else if( MenuPageIndex == ColorTempMenu )
    {
        return UserPrefColorTemp;
    }
    else if( MenuPageIndex == ColorModeMenu )
    {
        return UserPrefColorMode;
    }
    else if( MenuPageIndex == ColorFormatMenu )
    {
        return UserPrefInputColorFormat;
    }

#if ENABLE_SUPER_RESOLUTION
    else if( MenuPageIndex == SuperResolutionMenu )
    {
        return UserPrefSuperResolutionMode;
    }
#endif
    else if( MenuPageIndex == ExtColorSettingsMenu )
    {
        if( menuPageIndex == ColorRangeMenu )
        {
            return ExtColorSettingsMenuItems_ColorRange;
        }
        else if( menuPageIndex == IndependentHueMenu)
        {
            return ExtColorSettingsMenuItems_IndependentHue;
        }
        else if( menuPageIndex == IndependentSaturationMenu)
        {
            return ExtColorSettingsMenuItems_IndependentSaturation;
        }
        else if( menuPageIndex == IndependentBrightnessMenu)
        {
            return ExtColorSettingsMenuItems_IndependentBrightness;
        }
    }
    else if( MenuPageIndex == ColorRangeMenu )
    {
        return UserPrefInputColorRange;
    }
    else if( MenuPageIndex == IndependentHueMenu )
    {
        if( menuPageIndex == IndependentHueRMenu )
        {
            return IndependentHueMenuItems_R;
        }
        else if( menuPageIndex == IndependentHueGMenu)
        {
            return IndependentHueMenuItems_G;
        }
        else if( menuPageIndex == IndependentHueBMenu)
        {
            return IndependentHueMenuItems_B;
        }
        else if( menuPageIndex == IndependentHueCMenu)
        {
            return IndependentHueMenuItems_C;
        }
        else if( menuPageIndex == IndependentHueMMenu)
        {
            return IndependentHueMenuItems_M;
        }
        else if( menuPageIndex == IndependentHueYMenu)
        {
            return IndependentHueMenuItems_Y;
        }
    }
    else if( MenuPageIndex == IndependentSaturationMenu)
    {
        if( menuPageIndex == IndependentSaturationRMenu )
        {
            return IndependentSaturationMenuItems_R;
        }
        else if( menuPageIndex == IndependentSaturationGMenu)
        {
            return IndependentSaturationMenuItems_G;
        }
        else if( menuPageIndex == IndependentSaturationBMenu)
        {
            return IndependentSaturationMenuItems_B;
        }
        else if( menuPageIndex == IndependentSaturationCMenu)
        {
            return IndependentSaturationMenuItems_C;
        }
        else if( menuPageIndex == IndependentSaturationMMenu)
        {
            return IndependentSaturationMenuItems_M;
        }
        else if( menuPageIndex == IndependentSaturationYMenu)
        {
            return IndependentSaturationMenuItems_Y;
        }
    }
    else if( MenuPageIndex == IndependentBrightnessMenu)
    {
        if( menuPageIndex == IndependentBrightnessRMenu )
        {
            return IndependentBrightnessMenuItems_R;
        }
        else if( menuPageIndex == IndependentBrightnessGMenu)
        {
            return IndependentBrightnessMenuItems_G;
        }
        else if( menuPageIndex == IndependentBrightnessBMenu)
        {
            return IndependentBrightnessMenuItems_B;
        }
        else if( menuPageIndex == IndependentBrightnessCMenu)
        {
            return IndependentBrightnessMenuItems_C;
        }
        else if( menuPageIndex == IndependentBrightnessMMenu)
        {
            return IndependentBrightnessMenuItems_M;
        }
        else if( menuPageIndex == IndependentBrightnessYMenu)
        {
            return IndependentBrightnessMenuItems_Y;
        }
    }
    else if( MenuPageIndex == InputSelectMenu)
    {
        if( menuPageIndex == SourceSelectMenu)
        {
            return InputSourceMenuItems_SourceSelect;
        }
#if ENABLE_DP_INPUT
        else if( menuPageIndex == DPVersionMenu)
        {
            return InputSourceMenuItems_DPVersion;
        }
#endif
    }
    else if( MenuPageIndex == SourceSelectMenu || MenuPageIndex == HotInputSelectMenu )
    {
        return UserPrefInputPriorityType;
    }
#if ENABLE_DP_INPUT
    else if( MenuPageIndex == DPVersionMenu)
    {
        return UserPrefDPVersion;
    }
#endif
    else if( MenuPageIndex == DisplaySettingsMenu )
    {
        if( menuPageIndex == GammaMenu )
        {
            return DisplaySettingsMenuItems_Gamma;
        }
#if (ENABLE_VGA_INPUT)
        else if( menuPageIndex == HPositionMenu )
        {
            return DisplaySettingsMenuItems_HPos;
        }
        else if( menuPageIndex == VPositionMenu )
        {
            return DisplaySettingsMenuItems_VPos;
        }
#endif
#if ENABLE_SHARPNESS
        else if( menuPageIndex == SharpnessMenu )
        {
            return DisplaySettingsMenuItems_Sharpness;
        }
#endif
#if (ENABLE_VGA_INPUT)
        else if( menuPageIndex == ClockMenu )
        {
            return DisplaySettingsMenuItems_Clock;
        }
        else if( menuPageIndex == FocusMenu )
        {
            return DisplaySettingsMenuItems_Phase;
        }
#endif
	#if Enable_Expansion
        else if( menuPageIndex == AspectRatioMenu )
        {
            return DisplaySettingsMenuItems_Aspect;
        }
	#endif
	#if ENABLE_RTE
        else if( menuPageIndex == ODMenu )
        {
            return DisplaySettingsMenuItems_OD;
        }
	#endif
#if (ENABLE_VGA_INPUT)
        else if( menuPageIndex == AutoMenu )
        {
            return DisplaySettingsMenuItems_AutoAdjust;
        }
#endif
    }
    else if( MenuPageIndex == OtherSettingsMenu )
    {
    #if AudioFunc
        if( menuPageIndex == VolumeMenu )
        {
            return OtherSettingsMenuItems_Volume;
        }
        else if( menuPageIndex == AudioSourceSelMenu )
        {
            return OtherSettingsMenuItems_AudioSource;
        }
        else
    #endif
        if( menuPageIndex == OSDTransMenu )
        {
            return OtherSettingsMenuItems_OSDTrans;
        }
        else if( menuPageIndex == OSDHPositionMenu )
        {
            return OtherSettingsMenuItems_OSDHPos;
        }
        else if( menuPageIndex == OSDVPositionMenu )
        {
            return OtherSettingsMenuItems_OSDVPos;
        }
        else if( menuPageIndex == OSDTimeoutMenu )
        {
            return OtherSettingsMenuItems_OSDTimeout;
        }
		#if ENABLE_OSD_ROTATION
        else if( menuPageIndex == OSDRotateMenu )
        {
            return OtherSettingsMenuItems_OSDRotate;
        }
		#endif
        else if( menuPageIndex == DisplayLogoMenu )
        {
            return OtherSettingsMenuItems_DisplayLogo;
        }
    }
#if Enable_Gamma
    else if( MenuPageIndex == GammaMenu )
    {
            return UserPrefGamaOnOff;
    }
#endif
#if ENABLE_RTE
    else if( MenuPageIndex == ODMenu )
    {
    		return UserprefOverDriveSwitch;
    }
#endif
#if ENABLE_FREESYNC
    else if( MenuPageIndex == FreeSyncMenu )
    {
    		return UserprefFreeSyncMode;
    }
#endif
    else if(MenuPageIndex == DisplayLogoMenu)
    {
        return UserPrefDisplayLogoEn;
    }
    else if(MenuPageIndex == PowerManagerMenu)
    {
#if ENABLE_DCOFF_CHARGE
        if( menuPageIndex == DCOffDischargeMenu )
        {
            return PowerManagerMenuItems_DCOffDischarge;
        }
        else if(menuPageIndex == PowerSavingMenu)
        {
            return PowerManagerMenuItems_PowerSaving;
        }
        else
        {
            return PowerManagerMenuItems_DCOffDischarge;
        }
#else
        return PowerManagerMenuItems_PowerSaving;
#endif
    }
    else if(MenuPageIndex == DCOffDischargeMenu)
    {
        return UserPrefDCOffDischarge;
    }
    else if(MenuPageIndex == PowerSavingMenu)
    {
        return UserPrefPowerSavingEn;
    }
    return 0;

#endif
}




#define MTK_2BCODE(x) (x)
BYTE code strSmallLogoWindow[39]=
{
    MTK_2BCODE(0x00), MTK_2BCODE(0x01), MTK_2BCODE(0x02), MTK_2BCODE(0x02), MTK_2BCODE(0x02), MTK_2BCODE(0x02), MTK_2BCODE(0x02), MTK_2BCODE(0x02), MTK_2BCODE(0x02), MTK_2BCODE(0x02), MTK_2BCODE(0x02), MTK_2BCODE(0x02), MTK_2BCODE(0x03), 
    MTK_2BCODE(0x04), MTK_2BCODE(0x05), MTK_2BCODE(0x06), MTK_2BCODE(0x07), MTK_2BCODE(0x08), MTK_2BCODE(0x09), MTK_2BCODE(0x0A), MTK_2BCODE(0x0B), MTK_2BCODE(0x0C), MTK_2BCODE(0x0D), MTK_2BCODE(0x0E), MTK_2BCODE(0x0F), MTK_2BCODE(0x10), 
    MTK_2BCODE(0x11), MTK_2BCODE(0x02), MTK_2BCODE(0x02), MTK_2BCODE(0x02), MTK_2BCODE(0x02), MTK_2BCODE(0x02), MTK_2BCODE(0x02), MTK_2BCODE(0x02), MTK_2BCODE(0x02), MTK_2BCODE(0x02), MTK_2BCODE(0x02), MTK_2BCODE(0x12), MTK_2BCODE(0x00), 
};
      

void DrawOsdBackGround(void)
{
#if LiteMAX_OSD_TEST
	BYTE i;

	#if 0
    if ((IS_WARNING_MENU(MenuPageIndex) || IS_MSG_STATUS(MenuPageIndex))
    {
    	#if 0//(LiteMAX_OSDtype == LiteMAX_OSD_Baron)
    	if (g_u8MenuPageIndex == MENU_HOT_BRIGHTNESS)
    	{
            BYTE i;
            OsdFontColor=FOUR_COLOR(4);
            
            for (i=0; i<OsdWindowHeight; i++)
                Osd_DrawContinuousCharDirect( 0, i, Space4C, OsdWindowWidth );
            
            Osd_DrawRealCharDirect( 1, 0, Frame4C_LT);
            Osd_DrawContinuousCharDirect( 2, 0, Frame4C_Top, OsdWindowWidth-4 );
            Osd_DrawRealCharDirect( OsdWindowWidth-2, 0, Frame4C_RT);
    	    
            for (i=1; i<OsdWindowHeight-1; i++)
                Osd_DrawRealCharDirect( 1, i, Frame4C_Left);
            for (i=1; i<OsdWindowHeight-1; i++)
                Osd_DrawRealCharDirect( OsdWindowWidth-2, i, Frame4C_Right);
            
            Osd_DrawRealCharDirect( 1, OsdWindowHeight-1, Frame4C_LB);
            Osd_DrawContinuousCharDirect( 2, OsdWindowHeight-1, Frame4C_Buttom, OsdWindowWidth-4 );
            Osd_DrawRealCharDirect( OsdWindowWidth-2, OsdWindowHeight-1, Frame4C_RB);
    	}
    	else
    	#endif
    	{
    		/*
            if (IS_WARNING_MENU(8MenuPageIndex)
                #if ENABLE_MULTI_INPUT
                || MenuPageIndex==MENU_HOT_INPUT
                #endif
                || MenuPageIndex==MENU_HOT_BRIGHTNESS
                #if OSD_MENU_BACKLIGHT_MODE
		    	|| MenuPageIndex==MENU_HOT_LS_OFFSET
		    	#endif
		    	#if ENABLE_AUDIO_SETTINGS_MENU
		    	|| MenuPageIndex==MENU_HOT_VOLUME
		    	#endif
                )
                OsdFontColor=MONO_COLOR(CP_MSG_BK,CP_MSG_BK);
            else
				*/
                OsdFontColor=MONO_COLOR(CP_BK_COLOR_L0,CP_BK_COLOR_L0);
            Osd_DrawBlankPlane(0,0,GET_MENU_XSIZE(CurrentMenu),GET_MENU_YSIZE(CurrentMenu));
            
            #if 0//ENABLE_MULTI_INPUT && (LiteMAX_OSDtype == LiteMAX_OSD_Baron) //240726
            if (g_u8MenuPageIndex==MENU_HOT_INPUT)
            {
            	if (!g_stMenuFlag.bPressSrcInputKey)
            	{
            		DrawResolutionRealText(1, 4);
                    DrawPixelClockRealText(16, 4);
            	}
            }
            #endif
        }
    }
    else
	#endif
	#if ENABLE_OSD_ROTATION
    if((MenuPageIndex == MainMenu)||(MenuPageIndex == OSDRotationMenu))
    #else
	if (MenuPageIndex == MainMenu)
	#endif
    {
        #if 0 //(LiteMAX_OSDtype == LiteMAX_OSD_Baron)
        BYTE i;
        OsdFontColor=FOUR_COLOR(4);

        for (i=0; i<OsdWindowHeight; i++)
            Osd_DrawContinuousCharDirect( 0, i, Space4C, OsdWindowWidth );

        Osd_DrawRealCharDirect( 1, 0, Frame4C_LT);
        Osd_DrawContinuousCharDirect( 2, 0, Frame4C_Top, OsdWindowWidth-4 );
        Osd_DrawRealCharDirect( OsdWindowWidth-2, 0, Frame4C_RT);
    	
        for (i=1; i<OsdWindowHeight-1; i++)
            Osd_DrawRealCharDirect( 1, i, Frame4C_Left);
        for (i=1; i<OsdWindowHeight-1; i++)
            Osd_DrawRealCharDirect( OsdWindowWidth-2, i, Frame4C_Right);

        Osd_DrawRealCharDirect( 1, OsdWindowHeight-1, Frame4C_LB);
        Osd_DrawContinuousCharDirect( 2, OsdWindowHeight-1, Frame4C_Buttom, OsdWindowWidth-4 );
        Osd_DrawRealCharDirect( OsdWindowWidth-2, OsdWindowHeight-1, Frame4C_RB);
        
        #if 1 //240726
        DrawResolutionRealText(10, 1);
        DrawPixelClockRealText(10, 2);
        #endif
                
        #else
		
		OsdFontColor=FOUR_COLOR(5);
        for (i=0; i<OsdWindowHeight; i++)
            Osd_DrawContinuesChar( 0, i, Space4C, OsdWindowWidth );

        Osd_DrawCharDirect( 1, 4, Frame4C_LT);
        for(i=0;i<7;i++)
        {
            if(i>0)
            {
                Osd_DrawCharDirect( 4*i+1, 4, FrameNew4C_LT2);
            }
            Osd_DrawCharDirect( 4*i+2, 4, Frame4C_Top);
            Osd_DrawCharDirect( 4*i+3, 4, Frame4C_Top);
            Osd_DrawCharDirect( 4*i+4, 4, Frame4C_Top);
            Osd_DrawCharDirect( 4*i+1, 5, Frame4C_Left);
            Osd_DrawCharDirect( 4*i+1, 6, Frame4C_Left);
        }
        Osd_DrawCharDirect( 29, 4, Frame4C_RT);
        Osd_DrawCharDirect( 29, 5, Frame4C_Right);
        Osd_DrawCharDirect( 29, 6, Frame4C_Right);

        Osd_DrawCharDirect( 1, 7, FrameNew4C_LT);
        Osd_DrawContinuesChar( 2, 7, FrameNew4C_Top, OsdWindowWidth-4 );
        Osd_DrawCharDirect( OsdWindowWidth-2, 7, FrameNew4C_RT);
        for(i=1;i<7;i++)
            Osd_DrawCharDirect( 4*i+1, 7, FrameNew4C_Top2);
        
        for (i=8; i<OsdWindowHeight-1; i++)
            Osd_DrawCharDirect( 1, i, Frame4C_Left);
        for (i=8; i<OsdWindowHeight-1; i++)
            Osd_DrawCharDirect( OsdWindowWidth-2, i, Frame4C_Right);

        Osd_DrawCharDirect( 1, OsdWindowHeight-1, Frame4C_LB);
        Osd_DrawContinuesChar( 2, OsdWindowHeight-1, Frame4C_Buttom, OsdWindowWidth-4 );
        Osd_DrawCharDirect( OsdWindowWidth-2, OsdWindowHeight-1, Frame4C_RB);
        #endif

        //-- End  --
    }
	else
	{
		OsdFontColor=FOUR_COLOR(5);
    	for (i=0; i<OsdWindowHeight; i++)
        	Osd_DrawContinuesChar( 0, i, Space4C, OsdWindowWidth );
	}
	/*
    else if (IS_MAIN_L1(g_u8MenuPageIndex))   //SubMenu
    {
    }
    else if(!IS_MAIN_L1(g_u8MenuPageIndex))    //Message Menu
    {
        OsdFontColor=MONO_COLOR(CP_BK_COLOR_L0,CP_BK_COLOR_L0);
        Osd_DrawBlankPlane(0,0,OsdWindowWidth,OsdWindowHeight);
    }
	*/

#else //#if LiteMAX_OSD_TEST
	
    BYTE i;

    OsdFontColor = (CPC_Black<<4|CPC_Black);
    for (i=0; i<OsdWindowHeight; i++)
        Osd_DrawContinuesChar( 0, i, SpaceFont, OsdWindowWidth );

    OsdFontColor = (CPC_White<<4|CPC_MTK_Gold);
    Osd_DrawContinuesChar( 0, 0, TopLine_2C, OsdWindowWidth );

    if( HotMenuPage )
    {
        OsdFontColor = (CPC_MTK_Gold<<4|CPC_MTK_Gold);
        Osd_DrawContinuesChar( 0, 1, SpaceFont, OsdWindowWidth );
        Osd_DrawContinuesChar( 0, 2, SpaceFont, OsdWindowWidth );
    }
    else
    {
        OsdFontColor = (CPC_Black<<4|CPC_MTK_Gold);
        Osd_DrawContinuesChar( 0, 1, TopBottom_2C, OsdWindowWidth );
    }

    OsdFontColor = (CPC_White<<4|CPC_Black);
    Osd_DrawContinuesChar( 0, OsdWindowHeight - 1, BottomLine_2C, OsdWindowWidth );


    if( MenuPageIndex == MainMenu )
    {
        OsdFontColor = (CPC_White<<4|CPC_Black);
        for (i=0; i<9; i++)
        {
            Osd_DrawContinuesChar( Layer1XPos, SaperationYPos+(i*2), Saperation_2C, LayerXSize );
            Osd_DrawContinuesChar( Layer2XPos, SaperationYPos+(i*2), Saperation_2C, LayerXSize );
            Osd_DrawContinuesChar( Layer3XPos, SaperationYPos+(i*2), Saperation_2C, LayerXSize );
        }

        OsdFontColor = CPC_SmallLogo_4C;
        for (i=0; i<13; i++)
        {
            Osd_DrawCharDirect(1+i, OsdWindowHeight - 4, SmallLogo_4C+strSmallLogoWindow[i+13*0]*2);
            Osd_DrawCharDirect(1+i, OsdWindowHeight - 3, SmallLogo_4C+strSmallLogoWindow[i+13*1]*2);
            Osd_DrawCharDirect(1+i, OsdWindowHeight - 2, SmallLogo_4C+strSmallLogoWindow[i+13*2]*2);
        }

        //OsdFontColor = (CPC_White<<4|CPC_Black);
        //Osd_DrawPropStr( OsdWindowWidth-0x0D, OsdWindowHeight - 3, NetAddrText() );

    }
#endif //#if LiteMAX_OSD_TEST
}
//======================================================================================
#if DisplayLogo!=NoBrand

BYTE code strLogWindow[11*42]=
{
    MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), 
    MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), 
    MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x01), MTK_2BCODE(0x02), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x03), MTK_2BCODE(0x04), 
    MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x05), MTK_2BCODE(0x06), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x08), MTK_2BCODE(0x09), 
    MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x0A), MTK_2BCODE(0x0B), MTK_2BCODE(0x07), MTK_2BCODE(0x0C), MTK_2BCODE(0x0D), MTK_2BCODE(0x0E), MTK_2BCODE(0x0F), MTK_2BCODE(0x10), MTK_2BCODE(0x11), MTK_2BCODE(0x12), MTK_2BCODE(0x13), MTK_2BCODE(0x14), MTK_2BCODE(0x15), MTK_2BCODE(0x16), MTK_2BCODE(0x17), MTK_2BCODE(0x18), MTK_2BCODE(0x19), MTK_2BCODE(0x1A), MTK_2BCODE(0x1B), MTK_2BCODE(0x1C), MTK_2BCODE(0x1D), MTK_2BCODE(0x1E), MTK_2BCODE(0x1F), MTK_2BCODE(0x20), MTK_2BCODE(0x21), MTK_2BCODE(0x22), MTK_2BCODE(0x13), MTK_2BCODE(0x23), MTK_2BCODE(0x24), MTK_2BCODE(0x25), MTK_2BCODE(0x26), MTK_2BCODE(0x27), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x28), MTK_2BCODE(0x29), MTK_2BCODE(0x00), 
    MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x2A), MTK_2BCODE(0x2B), MTK_2BCODE(0x07), MTK_2BCODE(0x2C), MTK_2BCODE(0x2D), MTK_2BCODE(0x2E), MTK_2BCODE(0x2F), MTK_2BCODE(0x30), MTK_2BCODE(0x31), MTK_2BCODE(0x32), MTK_2BCODE(0x33), MTK_2BCODE(0x34), MTK_2BCODE(0x35), MTK_2BCODE(0x36), MTK_2BCODE(0x37), MTK_2BCODE(0x38), MTK_2BCODE(0x39), MTK_2BCODE(0x3A), MTK_2BCODE(0x3B), MTK_2BCODE(0x3C), MTK_2BCODE(0x3D), MTK_2BCODE(0x3E), MTK_2BCODE(0x3F), MTK_2BCODE(0x40), MTK_2BCODE(0x41), MTK_2BCODE(0x42), MTK_2BCODE(0x43), MTK_2BCODE(0x44), MTK_2BCODE(0x45), MTK_2BCODE(0x46), MTK_2BCODE(0x47), MTK_2BCODE(0x48), MTK_2BCODE(0x49), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x4A), MTK_2BCODE(0x4B), MTK_2BCODE(0x00), MTK_2BCODE(0x00), 
    MTK_2BCODE(0x00), MTK_2BCODE(0x4C), MTK_2BCODE(0x4D), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x4E), MTK_2BCODE(0x4F), MTK_2BCODE(0x50), MTK_2BCODE(0x51), MTK_2BCODE(0x52), MTK_2BCODE(0x53), MTK_2BCODE(0x54), MTK_2BCODE(0x55), MTK_2BCODE(0x56), MTK_2BCODE(0x57), MTK_2BCODE(0x58), MTK_2BCODE(0x59), MTK_2BCODE(0x5A), MTK_2BCODE(0x5B), MTK_2BCODE(0x5C), MTK_2BCODE(0x5D), MTK_2BCODE(0x5E), MTK_2BCODE(0x5F), MTK_2BCODE(0x60), MTK_2BCODE(0x61), MTK_2BCODE(0x62), MTK_2BCODE(0x63), MTK_2BCODE(0x64), MTK_2BCODE(0x65), MTK_2BCODE(0x56), MTK_2BCODE(0x66), MTK_2BCODE(0x67), MTK_2BCODE(0x68), MTK_2BCODE(0x69), MTK_2BCODE(0x6A), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x6B), MTK_2BCODE(0x6C), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), 
    MTK_2BCODE(0x6D), MTK_2BCODE(0x6E), MTK_2BCODE(0x6F), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x07), MTK_2BCODE(0x70), MTK_2BCODE(0x71), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), 
    MTK_2BCODE(0x72), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x73), MTK_2BCODE(0x74), MTK_2BCODE(0x75), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), 
    MTK_2BCODE(0x76), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x77), MTK_2BCODE(0x78), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), 
    MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), MTK_2BCODE(0x00), 
};
void DrawLogo( void )
{
    BYTE i;
    BYTE j;
    //  MonoColorSetting();
    //printMsg("Draw Logo Frame ");
    Osd_LoadLogoFontCP();

    OsdFontColor = CPC_SmallLogo_4C;
    for( i = 0; i < 11; i++ )
        for( j = 0; j < 42; j++ )
            Osd_DrawCharDirect( j, i, Logo_4C+(strLogWindow[i*42+j])*2 );

}
#endif
//=============================================================================
#define HDMI_CABLE_DETECT_INVERT    1
Bool Menu_GetCableDetect(BYTE u8Inputport)
{
    Bool bCablStatus = 0;
    if(u8Inputport == Input_Nothing)
    {
        bCablStatus = FALSE;
    }
    if(u8Inputport == Input_HDMI_C1)
    {
        #if HDMI_CABLE_DETECT_INVERT
        bCablStatus = !hwDVI0Cable_Pin;
        #else
        bCablStatus = hwDVI0Cable_Pin;
        #endif
    }
    if(u8Inputport == Input_HDMI_C2)
    {
        #if HDMI_CABLE_DETECT_INVERT
        bCablStatus = !hwDVI1Cable_Pin;
        #else
        bCablStatus = hwDVI1Cable_Pin;
        #endif
    }
    if(u8Inputport == Input_HDMI_C3)
    {
        #if HDMI_CABLE_DETECT_INVERT
        bCablStatus = !hwDVI2Cable_Pin;
        #else
        bCablStatus = hwDVI2Cable_Pin;
        #endif
    }
    if(u8Inputport == Input_HDMI_C4)
    {
        #if HDMI_CABLE_DETECT_INVERT
        bCablStatus = !hwDVI3Cable_Pin;
        #else
        bCablStatus = hwDVI3Cable_Pin;
        #endif
    }
    if(u8Inputport == Input_Displayport_C1)
    {
        bCablStatus = (!hwGNDDP0_Pin);
    }
    if(u8Inputport == Input_Displayport_C2)
    {
        bCablStatus = (!hwGNDDP1_Pin);
    }
    if(u8Inputport == Input_Displayport_C3)
    {
        bCablStatus = (!hwGNDDP2_Pin);
    }
    if(u8Inputport == Input_Displayport_C4)
    {
        bCablStatus = (!hwGNDDP3_Pin);
    }
    if(u8Inputport == Input_UsbTypeC_C3)
    {
        //bCablStatus = hwSDM_AuxP_TYPEC3Pin_Pin;
        //Can't use type c cable detect with demo board settings
    }
    if(u8Inputport == Input_UsbTypeC_C4)
    {
        //bCablStatus = hwSDM_AuxP_TYPEC4Pin_Pin;
        //Can't use type c cable detect with demo board settings
    }

    return bCablStatus;
}

void Menu_RxCableStatePollingHandler(void)
{
    BYTE i=0;
    static Bool u8PrevState[Input_Nums] = {0};
    Bool u8CurrentState[Input_Nums] = {0}; 

    if(g_bFirstPowerOn[eFIRST_POWER_ON_MENU_CABLE_DETECT])
    {
        for(i=0; i<Input_Nums; i++)
        {
            u8PrevState[i] = 0;
        }
    }

    for(i=0; i<Input_Nums; i++)
    {
        u8CurrentState[i] = Menu_GetCableDetect(i);

        if(u8CurrentState[i] != u8PrevState[i])
        {
            if(SrcInputType == i)
            {
                InputTimingStableCounter = 1;//Reset stable counter when cable status change. 
                
                if(u8CurrentState[i] && (!g_bFirstPowerOn[eFIRST_POWER_ON_MENU_CABLE_DETECT]))
                {
                    BootTimeStamp_Clr();
                    BootTimeStamp_Set(AP_LOOP_TS, 0, TRUE);
                }
                #if 0//ENABLE_DEBUG
                printf("***___Reset timing stable counter___***\n");
                #endif
            }

            #if ENABLE_BOOT_TIME_PROFILING
            MDrv_UART_SetIsPrint(TRUE);
            printf("cable status change, port: %d, %d -> %d\n", i,  u8PrevState[i], u8CurrentState[i]);
            MDrv_UART_SetIsPrint(FALSE);
            #endif
            u8PrevState[i] = u8CurrentState[i];
        }
    }

    if(g_bFirstPowerOn[eFIRST_POWER_ON_MENU_CABLE_DETECT])
    {
        g_bFirstPowerOn[eFIRST_POWER_ON_MENU_CABLE_DETECT] = FALSE;
    }
}
