#include "types.h"
#include "Global.h"
#include "board.h"
#include "MenuText.h"

//======================================================================
const BYTE *PassText( void )
{
    return strPass;
}

const BYTE *FailText( void )
{
    return strFail;
}
#if 1
//for Factory Alignment
const BYTE *VersionDateText( void )
{
    return strVersionDate;
}
const BYTE *ChangeDateText( void )
{
    return strChangeDate;
}
const BYTE *FactoryBrightnessText( void )
{
    return strFactoryBrightness;
}
const BYTE *FactoryContrastText( void )
{
    return strFactoryContrast;
}
const BYTE *PanelText( void )
{
    return strPanel;
}
const BYTE *PanelOnText( void )
{
    return strPanelOn;
}
const BYTE *ModelNameText( void )
{
    return strModelName;
}
const BYTE *ChipNameText( void )
{
    return strChipName;
}
const BYTE *BacklightHoursText( void )
{
    return strBacklightHours;
}
const BYTE *SpreadSpectrumText( void )
{
    return strSpreadSpectrum;
}
const BYTE *FExitText( void )
{
    return strFExit;
}
const BYTE *FreqText( void )
{
    return strFreq;
}
const BYTE *AmpText( void )
{
    return strAmp;
}
//BYTE* DateText(void)
//{ return strDate;
//}
const BYTE *AutoColorText( void )
{
    return strAutoColor;
}
#if !USEFLASH
const BYTE *EraseEEPROMText( void )
{
    return strEraseEEPROM;
}
#else
const BYTE *EraseNVRamText(void)
{
    return strEraseNVRam;
}
const BYTE *OffText(void)
{
    return strOff;
}
#endif

const BYTE *OnText(void)
{
    return strOn;
}

const BYTE *AdcGainText( void )
{
    return strAdcGain;
}
const BYTE *FactoryRedText( void )
{
    return strFactoryRed;
}
const BYTE *FactoryGreenText( void )
{
    return strFactoryGreen;
}
const BYTE *FactoryBlueText( void )
{
    return strFactoryBlue;
}
const BYTE *AdcOffsetText( void )
{
    return strAdcOffset;
}
const BYTE *COOL1Text( void )
{
    return strFCool1;
}
//BYTE* COOL2Text(void)
//{ return strFCool2;
//}
const BYTE *WARM1Text( void )
{
    return strFWarm1;
}
//BYTE* WARM2Text(void)
//{ return strFWarm2;
//}
const BYTE *sRGBText( void )
{
    return strFsRGB;
}
const BYTE *NORMALText( void )
{
    return strFNORMAL;
}
#endif
#if 1
const BYTE *BurninText( void )
{
    return strBurnin;
}
#endif

const BYTE *EDIDWPText( void )
{
    return strEDIDWP;
}
const BYTE *DDCWPOnOffText( void )
{
    return ( DDCWPActiveFlag) ? ( strOnOff[1] ) : ( strOnOff[0] );
}
const BYTE *ProductionText( void )
{
    return strProduction;
}
const BYTE *ProductionOnOffText( void )
{
    return ( ProductModeFlag) ? ( strOnOff[1] ) : ( strOnOff[0] );
}

#if 1
const BYTE *BurninOnOffText( void )
{
    return ( BurninModeFlag ) ? ( strOnOff[1] ) : ( strOnOff[0] );
}
const BYTE *BankText( void )
{
    return strBank;
}
const BYTE *BankNameText( void )
{
    return strBankName[MSBankNo];
}
const BYTE *AddressText( void )
{
    return strAddress;
}
const BYTE *ValueText( void )
{
    return strValue;
}
#endif

#if 0//ENABLE_RTE
/* Multiple definition as in MenuStrProp.c
BYTE code strOverDrive[] =
{
    //123456789012345678901234567890
    "OVD"
};

const BYTE *OverDriveText( void )
{
    return strOverDrive;
}
*/
const BYTE *OverDriveOnOffText( void )
{
    return ( OverDriveOnFlag ) ? ( strOnOff[1] ) : ( strOnOff[0] );
}
#endif
#if PanelRSDS
BYTE code strVCOM[] =
{
    //123456789012345678901234567890
    "VCOM"
};

const BYTE *VCOMText( void )
{
    return strVCOM;
}

#endif
