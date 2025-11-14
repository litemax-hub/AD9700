#include "Panel.h"
//======== MAIN MENU ICON ===============//
#if MWEFunction
BYTE code strLuminanceIcon[7] =
{
    //123456789012345678901234567890
    0xc6, 0xc9, 0xcc, 0xcf, 0xd2, 0xd5, '\0'
};
BYTE code strImageSetUpIcon[7] =
{
    //123456789012345678901234567890
    0xd8, 0xdb, 0xde, 0xe1, 0xe4, 0xe7, '\0'
};
BYTE code strColorTempIcon[7] =
{
    //123456789012345678901234567890
    0xea, 0xed, 0xf0, 0xf3, 0xf6, 0xf9, '\0'
};
BYTE code strFreeColorIcon[7] =
{
    //123456789012345678901234567890
    0xfc, 0x00, 0x03, 0x06, 0x09, 0x0c, '\0'
};
BYTE code strBrightFrameIcon[7] =
{
    //123456789012345678901234567890
    0x0f, 0x12, 0x15, 0x18, 0x1b, 0x1e, '\0'
};
BYTE code strOsdSetUpIcon[7] =
{
    //123456789012345678901234567890
    0x21, 0x24, 0x27, 0x2a, 0x2d, 0x30, '\0'
};
BYTE code strExtraIcon[7] =
{
    //123456789012345678901234567890
    0x33, 0x36, 0x39, 0x3c, 0x3f, 0x42, '\0'
};
#else
BYTE code strLuminanceIcon[7] =
{
    //123456789012345678901234567890
    0xc6, 0xc9, 0xcc, 0xcf, 0xd2, 0xd5, '\0'
};
BYTE code strImageSetUpIcon[7] =
{
    //123456789012345678901234567890
    0xd8, 0xdb, 0xde, 0xe1, 0xe4, 0xe7, '\0'
};
BYTE code strColorTempIcon[7] =
{
    //123456789012345678901234567890
    0xea, 0xed, 0xf0, 0xf3, 0xf6, 0xf9, '\0'
};
BYTE code strOsdSetUpIcon[7] =
{
    //123456789012345678901234567890
    0xfc, 0x00, 0x03, 0x06, 0x09, 0x0c, '\0'
};
BYTE code strExtraIcon[7] =
{
    //123456789012345678901234567890
    0x0f, 0x12, 0x15, 0x18, 0x1b, 0x1e, '\0'
};
#endif
//=================================================================
BYTE code strVersionDate[] =
{
    //123456789012345678901234567890
    "V"FWVersion  //Benz 2007.4.14   21:46:49
};
BYTE code strChangeDate[] =
{
    //123456789012345678901234567890
    ChangeDate  //Benz 2007.4.15   22:47:41
};
BYTE code strModelName[] =
{
    //123456789012345678901234567890
    Model//cc 2011.9.8 22:00
};
BYTE code strChipName[] =
{
    //123456789012345678901234567890
    "MST "ChipName//cc 2011.9.8 22:00
};
BYTE code strHour[] =
{
    //123456789012345678901234567890
    // "HRS"
    "\xf5\xf6\xf7"
};
BYTE code strPot[] =
{
    //123456789012345678901234567890
    //"POT\x086"
    "\x0f2\x0f3\x0f4\x086"
};

BYTE code strPanel[] =  //
{
    //123456789012345678901234567890

    PanelName
};
BYTE code strPanelOn[] =  //
{
    //123456789012345678901234567890
      "Panel Select"// "Panel\X001On"
};
BYTE code strBacklightHours[] =  //
{
    //123456789012345678901234567890
    "Backlight Hours"
};
BYTE code strSpreadSpectrum[] =  //
{
    //123456789012345678901234567890
    "Spread spectrum"
};
BYTE code strFExit[] =  //
{
    //123456789012345678901234567890
    "Exit"
};
BYTE code strFreq[] =  //
{
    //123456789012345678901234567890
    "Freq"
};
BYTE code strAmp[] =  //
{
    //123456789012345678901234567890
    "Amp"
};

BYTE code strAutoColor[] =
{
    //123456789012345678901234567890
    "Auto\x001Color"
};
#if USEFLASH
BYTE code strEraseNVRam[] =
{
    //123456789012345678901234567890
    "EraseNVRAM"
};
BYTE code strOff[] =
{
    //123456789012345678901234567890
    "Off"
};
#else
BYTE code strEraseEEPROM[] =
{
    //123456789012345678901234567890
    "Erase\x001EEPROM"
};
#endif
BYTE code strOn[] =
{
    //123456789012345678901234567890
    "On"
};


BYTE code strPass[] =
{
    //123456789012345678901234567890
    "Pass"
};
BYTE code strFail[] =
{
    //123456789012345678901234567890
    "Fail"
};
BYTE code strAdcGain[] =
{
    //123456789012345678901234567890
    "Gain"
};
BYTE code strAdcOffset[] =
{
    //123456789012345678901234567890
    "Offset"
};
BYTE code strFactoryRed[] =
{
    //123456789012345678901234567890
    "R"
};
BYTE code strFactoryGreen[] =
{
    //123456789012345678901234567890
    "G"
};
BYTE code strFactoryBlue[] =
{
    //123456789012345678901234567890
    "B"
};
BYTE code strFCool1[] =
{
    //123456789012345678901234567890
    // "C2"//"C2\x08393"//9300K"
    "Cool"
};
BYTE code strFWarm1[] =
{
    //123456789012345678901234567890
    // "C2"//"C2\x08393"//9300K"
    "Warm"
};
BYTE code strFsRGB[] =
{
    //123456789012345678901234567890
    "sRGB" //\x08365"//6500K"
};
BYTE code strFNORMAL[] =
{
    //123456789012345678901234567890
    "Normal" //\x08365"//6500K"
};
BYTE code strFactoryBrightness[] =
{
    //123456789012345678901234567890
    "Bri" //\x08365"//6500K"
};
BYTE code strFactoryContrast[] =
{
    //123456789012345678901234567890
    "Con" //\x08365"//6500K"
};
BYTE code strBurnin[] =
{
    //123456789012345678901234567890
    "BurnIn"
};
BYTE code strEDIDWP[] =
{
    //123456789012345678901234567890
    "EDID WP"
};
BYTE code strOnOff[2][4] =
{
    //123456789012345678901234567890
    //  O    f   f
    {"Off"},
    //  O    n
    {"On "}
};
BYTE code strBank[] =
{
    //123456789012345678901234567890
    "Bank"
};
BYTE code strProduction[] =
{
    //123456789012345678901234567890
    "Production"
};
BYTE code strBankName[3][9] =
{
    //123456789012345678901234567890
    {"0\x011Scaler"},
    {"1\x011 ADC  "},
    {"2\x011TCON  "}
};
BYTE code strAddress[] =
{
    //123456789012345678901234567890
    "Addr"
};
BYTE code strValue[] =
{
    //123456789012345678901234567890
    "Value"
};


