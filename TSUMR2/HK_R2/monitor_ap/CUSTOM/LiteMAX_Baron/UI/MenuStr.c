#include "types.h"
#include "Global.h"
#include "board.h"
#include "MenuText.h"

//======================================================================
//for Factory Menu
//======================================================================
const BYTE *F_FactoryDialogText( void )
{
    return strF_FactoryDialog;
}
const BYTE *F_PanelTypeText( void )
{
    return strF_PanelType;
}
const BYTE *F_PanelText( void )
{
    return strF_Panel;
}
const BYTE *F_FirmwareText( void )
{
    return strF_Firmware;
}
const BYTE *F_FwPartNoText( void )
{
    return strF_PartNo;
}
const BYTE *VersionDateText( void )
{
    return strF_VersionDate;
}
const BYTE *ChangeDateText( void )
{
    return strF_ChangeDate;
}
const BYTE *F_BurninText( void )
{
    return strF_Burnin;
}
const BYTE *BurninOnOffText( void )
{
    return ( BurninModeFlag ) ? ( strF_Yes ) : ( strF_No );
}
const BYTE *F_FactoryResetText( void )
{
    return strF_FactoryReset;
}
const BYTE *F_Curve_0Text( void )
{
    return strF_Curve_0;
}
const BYTE *F_Curve_25Text( void )
{
    return strF_Curve_25;
}
const BYTE *F_Curve_50Text( void )
{
    return strF_Curve_50;
}
const BYTE *F_Curve_75Text( void )
{
    return strF_Curve_75;
}
const BYTE *F_Curve_100Text( void )
{
    return strF_Curve_100;
}
const BYTE *F_ExitText( void )
{
    return strF_Exit;
}
//====================================================
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

#if 0
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
