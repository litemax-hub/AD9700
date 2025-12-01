#include <math.h>    //MST_MCU
#include "types.h"
#include "board.h"
#include "Keypaddef.h"
#include "ms_reg.h"
#include "Global.h"
#include "Debug.h"
#include "Common.h"
#include "misc.h"
#include "Ms_rwreg.h"
#include "Mcu.h"
#include "menudef.h"
#include "GPIO_DEF.h"
#if USEFLASH
#include "UserPref.h"
#endif

BYTE xdata KeyDebug=0;
BYTE xdata LastKeypadButton=0;

extern void Osd_Hide( void );
extern void SaveMonitorSetting( void );
extern Bool ResetAllSetting( void );
extern Bool PowerOffSystem( void );
extern Bool ExecuteKeyEvent( MenuItemActionType menuAction );

//================================================================
extern BYTE xdata MenuPageIndex;
extern BYTE xdata MenuItemIndex;
//2006-07-10

BYTE Key_GetKeypadStatus( void )
{
    BYTE temp, temp1, retry_Key;
    BYTE keypad = 0xFF;
    if ( KeyDebug )
    {
#if 0//DEBUG_EN
        printData("@@@@ Key:%x", KeyDebug);
#endif
        LastKeypadButton = KeyDebug;
        keypad &= (~KeyDebug);
        KeyDebug = 0;
        return keypad;
    }
    else
    {
    retry_Key = 3;
    while( retry_Key )
    {
        temp = KEYPAD_ADC_A;
        Delay1ms( 2 );
        temp1 = KEYPAD_ADC_A;
        if( abs( temp - temp1 ) < 3 )
            break;
        retry_Key--;
    }

#if 1
    if(temp < 0x20) // 0
        keypad &= ~KEY_MINUS;
    else if(temp < 0xf0) // 0x48
        keypad &= ~KEY_MENU;
#else
#message "KEYPAD_ADC_A not coding"
#endif


#if ADC_KEY_PRESS_DEBUG_ENABLE
    if( temp < 0xfa )
        printData( "KEYPAD_ADC_A=0x%x", temp );
#endif

    retry_Key = 3;
    while( retry_Key )
    {
        temp = KEYPAD_ADC_B;
        Delay1ms( 2 );
        temp1 = KEYPAD_ADC_B;
        if( abs( temp - temp1 ) < 3 )
            break;
        retry_Key--;
    }

#if 1
    if(temp < 0x20) // 0
        keypad &= ~KEY_PLUS;
    else if(temp < 0x70) // 0x48
        keypad &= ~KEY_EXIT;

#else
#message "KEYPAD_ADC_B not coding"
#endif


#if ADC_KEY_PRESS_DEBUG_ENABLE
    if( temp < 0xfa )
        printData( "KEYPAD_ADC_B=0x%x", temp );
#endif
#if ADC_KEY_PRESS_DEBUG_ENABLE
        printData( "PowerKey=0x%x", PowerKey );
#endif

    if(PowerKey == 0 )
    {
        keypad &= ~KEY_POWER;
    }
    return keypad;
}
}
void CheckFactoryKeyStatus( void )
{
    BYTE keypadStatus;
    keypadStatus = ( Key_GetKeypadStatus() ^ KeypadMask ) &KeypadMask;
    Clr_FactoryModeFlag();

    if( keypadStatus == KEY_FACTORY )
    {
        Set_FactoryModeFlag();
        Set_PowerOnFlag();
    }
}
void Key_ScanKeypad( void )
{
#if ENABLE_TOUCH_KEY
    WORD keypadStatus=0;
#else
    BYTE keypadStatus=0;
#endif

    #if ENABLE_DEBUG
    static BYTE keypadStatus_Pre = 0;
    #endif

    if( gBoolVisualKey == 0 )
    {
#if ENABLE_TOUCH_KEY
        keypadStatus = ( TouchKey_GetKeyStatus() ^ KeypadMask ) &KeypadMask;
        Set_bKeyReadyFlag();
#else
        keypadStatus = ( Key_GetKeypadStatus() ^ KeypadMask ) &KeypadMask;

        #if ENABLE_DEBUG
        if(keypadStatus_Pre != keypadStatus)
        {
            printf("keypadStatus: 0x%x\n", keypadStatus);
            keypadStatus_Pre = keypadStatus;
        }
        #endif
        
        Set_bKeyReadyFlag();
#endif

    }
    else if( gBoolVisualKey == 1 )
    {
        gBoolVisualKey = 0;
        keypadStatus = ( gByteVisualKey ^ KeypadMask ) &KeypadMask;
    }

    if( !bKeyReadyFlag )
    {
        return;
    }

    if( keypadStatus )
    {
        if(FakeSleepFlag)
        {
 			printMsg("KEY, EXIT FAKE SLEEP\n");
            Clr_FakeSleepFlag();
            FakeSleepCounter = 0;
            Set_ShowInputInfoFlag();
            Set_InputTimingChangeFlag();
            SrcFlags |= SyncLoss;
            SwitchPortCntr = 0;
            if (SrcInputType != UserPrefInputType)
            {
                SrcInputType = UserPrefInputType;
                mStar_SetupInputPort();
            }
        }

        if (LastKeypadButton!=keypadStatus)
        {
            LastKeypadButton=keypadStatus;
            KeypadButton=BTN_Nothing;
            goto KeyEnd;
        }

        if( keypadStatus == KEY_PRODUCT && ProductModeFlag )
        {
            if( MenuPageIndex == RootMenu ||
                    MenuPageIndex == HotKeyECOMenu
#if AudioFunc
                    || MenuPageIndex == HotKeyVolMenu
#endif
              )
            {
                Osd_Hide();
                ResetAllSetting();
            }
            else
                KeypadButton = BTN_Nothing;

            return;
        }
        if( KeypadButton == BTN_Repeat && keypadStatus != KEY_LOCK
#if 1 //ENABLE_TOUCH_KEY
                && keypadStatus != KEY_FACTORY // 100524 coding addition for factory key
#endif
        )        // 0105
            return ;
        //0105

        #if ENABLE_MTK_TEST
        if(keypadStatus == KEY_RESET)
        {
            ResetAllSetting();
        }
        #endif

        {
#if 1
            if( keypadStatus == KEY_LOCK )
            {
                if( KeypadButton == BTN_Power || KeypadButton == BTN_LockRepeat )
                    KeypadButton = BTN_LockRepeat;
                else
                {
                    if( !PowerOnFlag && !( MonitorFlags & OsdLockModeFlag ) )
                    {
                        Set_OsdLockModeFlag();
                        //Set_PowerOnFlag();
                        Set_ShowOsdLockFlag();
                        KeypadButton = BTN_Power;
                    }
                    else if(( !PowerOnFlag ) && ( MonitorFlags & OsdLockModeFlag ) )
                    {
                        Clr_OsdLockModeFlag();
                        //Set_PowerOnFlag();
                        Clr_ShowOsdLockFlag();
                        KeypadButton = BTN_Power;
                    }
                }
                #if USEFLASH
                Set_FlashSaveMonitorFlag();
                #else
                SaveMonitorSetting();
                #endif
            }
#if 1 //ENABLE_TOUCH_KEY
            else if( keypadStatus == KEY_FACTORY ) // 100524 coding addition for factory key
            {
                if( PowerOnFlag && MenuPageIndex <= RootMenu)
                {
                    Clr_OsdLockModeFlag();
                    Clr_ShowOsdLockFlag();
                    Set_FactoryModeFlag();
                    MenuPageIndex = FactoryMenu; // 100908
                    MenuItemIndex = 0;
                    ExecuteKeyEvent( MIA_RedrawMenu );
                }
            }
#endif
            else
#endif
                if( keypadStatus == KEY_PLUS )
                {
                #if HotExpansion
                    if( (MenuPageIndex == RootMenu || MenuPageIndex == HotExpansionMenu) && ( KeypadButton == BTN_Plus || KeypadButton == BTN_Repeat ) )
                        KeypadButton = BTN_Repeat;
                #else
                    if( (MenuPageIndex == RootMenu) && ( KeypadButton == BTN_Plus || KeypadButton == BTN_Repeat ) )
                        KeypadButton = BTN_Repeat;
                #endif
                    else
                    {
                        KeypadButton = BTN_Plus;
						printMsg("KeypadButton == BTN_Plus \n");
                    }
                }
                else if( keypadStatus == KEY_MINUS )
                {
                    if( (MenuPageIndex == RootMenu || MenuPageIndex == HotKeyECOMenu) && ( KeypadButton == BTN_Minus || KeypadButton == BTN_Repeat ) )
                        KeypadButton = BTN_Repeat;
                    else
                    {
                        KeypadButton = BTN_Minus;
						printMsg("KeypadButton == BTN_Minus \n");
                    }
                }
                else if( keypadStatus == KEY_MENU )
                {
                    if( KeypadButton == BTN_Menu || KeypadButton == BTN_Repeat )
                        KeypadButton = BTN_Repeat;
                    else
                    {
                        KeypadButton = BTN_Menu;
						printMsg("KeypadButton == BTN_Menu \n");
                    }
                }
                else if( keypadStatus == KEY_EXIT )
                {
                    if( KeypadButton == BTN_Exit || KeypadButton == BTN_Repeat )
                        KeypadButton = BTN_Repeat;
                    else
                    {
#if HotInputSelect
#ifndef SixKeyAutoDirectAndSourceSelect
                        if( MenuPageIndex == RootMenu || MenuPageIndex == UnsupportedModeMenu )
                        {
                            if( !PressExitFlag )
                            {
                                Set_PressExitFlag();
                                HotKeyCounter = 2;
                            }
                            KeypadButton = BTN_Nothing;
                        }
                        else
#endif
#endif
                        {
                            KeypadButton = BTN_Exit;
							printMsg("KeypadButton == BTN_Exit \n");
                        }
                    }
                }
                else if( keypadStatus == KEY_SELECT )
                    //2006-02-22
                {
                    if( KeypadButton == BTN_Repeat )
                        KeypadButton = BTN_Repeat;
                    else
                    {
                        KeypadButton = BTN_Select;
						printMsg("KeypadButton == BTN_Select \n");
                    }
                }
                else if( keypadStatus == KEY_POWER )
                {
#if ENABLE_TOUCH_KEY
#if TOUCH_KEY_POWER_KEY_DEBOUNCE
                    bPowerKeyPressed = 1;
                    if( PowerKeyDebounceCntr >= POWER_KEY_DEBOUNCE_PEROID )
                    {
                        if( KeypadButton == BTN_Power || KeypadButton == BTN_Repeat )
                            KeypadButton = BTN_Repeat;
                        else
                            KeypadButton = BTN_Power;
                    }
                    else
                    {
                        KeypadButton = BTN_Nothing;
                    }
#else
                    if( KeypadButton == BTN_Power || KeypadButton == BTN_Repeat )
                        KeypadButton = BTN_Repeat;
                    else
                        KeypadButton = BTN_Power;
#endif
#else
                    if( KeypadButton == BTN_Power || KeypadButton == BTN_Repeat )
                        KeypadButton = BTN_Repeat;
                    else
                        KeypadButton = BTN_Power;
#endif

                }
                else
                {
#if ENABLE_TOUCH_KEY
#if TOUCH_KEY_POWER_KEY_DEBOUNCE
                    bPowerKeyPressed = 0;
                    PowerKeyDebounceCntr = 0;
#endif
#endif

                    KeypadButton = BTN_Nothing;
                }
        }
    }
    else
    {
#if HotInputSelect
        if( PressExitFlag )
        {
            if( MenuPageIndex == RootMenu || MenuPageIndex == UnsupportedModeMenu )
            {
                KeypadButton = BTN_Select;
                HotKeyCounter = 0;
                Clr_EnableShowAutoFlag();
            }
            //if( SyncLossState() )
                //Clr_PressExitFlag();
        }
#endif
        KeypadButton = BTN_Nothing;
    LastKeypadButton=keypadStatus;
#if ENABLE_TOUCH_KEY
#if TOUCH_KEY_POWER_KEY_DEBOUNCE
        bPowerKeyPressed = 0;
        PowerKeyDebounceCntr = 0;
#endif
#endif

    }
    KeyEnd:

    KeyDebug = 0;
}
//================================================================
