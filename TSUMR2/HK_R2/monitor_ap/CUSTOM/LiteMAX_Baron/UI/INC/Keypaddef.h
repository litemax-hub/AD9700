typedef enum
{
    KEY_POWER = BIT0, // power
    KEY_PLUS     = BIT2, //BIT2, // right
    KEY_MINUS = BIT3, //BIT3, // left
    KEY_MENU     = BIT1, //BIT1, // menu
    KEY_EXIT     = BIT4, //BIT4, // exit/auto
    KEY_SELECT  = BIT6, //2006-02-22  source select

    KEY_FACTORY = KEY_PLUS | KEY_MINUS, 
    //  KEY_ISP     =KEY_PLUS|KEY_MINUS,//KEY_MENU|KEY_EXIT,
    KEY_PRODUCT = KEY_POWER | KEY_EXIT | KEY_MENU,

    KEY_LOCK        = KEY_POWER | KEY_MENU,
    KEY_MUTE        = KEY_EXIT | KEY_MENU,
    KEY_RESET       = KEY_PLUS | KEY_MINUS | KEY_POWER,
    KEY_NOTHING = 0
}
KeypadMaskType;


typedef enum
{
    BTN_Plus,
    BTN_Minus,
    BTN_Menu,
    BTN_Exit,
    BTN_Power,
    BTN_Select,

    BTN_EndBTN,
    BTN_Repeat,
    BTN_LockRepeat,
    
    BTN_Nothing
} ButtonType;

#define KeypadMask (KEY_POWER|KEY_PLUS|KEY_MINUS|KEY_MENU|KEY_EXIT|KEY_SELECT|KEY_RESET)
