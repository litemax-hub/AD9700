#include "types.h"
#include "Global.h"
#include "board.h"
#include "Keypaddef.h"
#include "menudef.h"
#include "MenuTextProp.h"


//--Eson Start----------------------------------
const BYTE *NullText( void )
{
    return 0;
}

const BYTE *NetAddrText( void )
{
    return 0;
}
#if LiteMAX_OSD_TEST
const BYTE *ResolutionText( void )
{
    return strResolutionM0[UserPrefLanguage];
}

const BYTE *PixelClockText( void )
{
    return strPixelClockM0[UserPrefLanguage];
}

const BYTE *xText( void )
{
    return strXM0[UserPrefLanguage];
}

const BYTE *HzText( void )
{
    return strHzM0[UserPrefLanguage];
}

const BYTE *MHzText( void )
{
    return strMHzM0[UserPrefLanguage];
}

const BYTE *DotText( void )
{
    return strDotM0[UserPrefLanguage];
}

const BYTE *LoadDefaultText( void )
{
    return strLoadDefaultM0[UserPrefLanguage];
}

const BYTE *BriteContText( void )
{
    return 0;
}
const BYTE *InputSourceText( void )
{
    return 0;
}
const BYTE *ColorSettingsText( void )
{
    return 0;
}
const BYTE *DisplaySettingsText( void )
{
    return 0;
}
const BYTE *OtherSettingsText( void )
{
    return 0;
}
const BYTE *PowerManagerText( void )
{
    return 0;
}
const BYTE *InformationText( void )
{
    return 0;
}
const BYTE *ExtColorSettingsText( void )
{
    return 0;
}
const BYTE *ExitText( void )
{
    return strExitM103[UserPrefLanguage];
}
const BYTE *ReturnText( void )
{
    return 0;
}
#else
const BYTE *BriteContText( void )
{
    return strBriteContM0[UserPrefLanguage];
}
const BYTE *InputSourceText( void )
{
    return strInputSourceM0[UserPrefLanguage];
}
const BYTE *ColorSettingsText( void )
{
    return strColorSettingsM0[UserPrefLanguage];
}
const BYTE *DisplaySettingsText( void )
{
    return strDisplaySettingsM0[UserPrefLanguage];
}
const BYTE *OtherSettingsText( void )
{
    return strOtherSettingsM0[UserPrefLanguage];
}
const BYTE *PowerManagerText( void )
{
    return strPowerManagerM0[UserPrefLanguage];
}
const BYTE *InformationText( void )
{
    return strInformationM0[UserPrefLanguage];
}
const BYTE *ExtColorSettingsText( void )
{
    return strExtColorSettingsM0[UserPrefLanguage];
}
const BYTE *ExitText( void )
{
    return strExitM0[UserPrefLanguage];
}
const BYTE *ReturnText( void )
{
    return strReturnM0[UserPrefLanguage];
}
#endif

const BYTE *NoSignalText( void )
{
    return strNoSignalM2[UserPrefLanguage];
}

const BYTE *CableNotConnectedText( void )
{
    return strCableNotConnectedM4[UserPrefLanguage];
}

const BYTE *OutofRangeText( void )
{
    return strOutofRangeM5[UserPrefLanguage];
}

const BYTE *ResetProgressText( void )
{
    return strResetProgressM7[UserPrefLanguage];
}

#if (ENABLE_VGA_INPUT)
const BYTE *AutoAdjustProgressText( void )
{
    return strAutoAdjustProgessM8[UserPrefLanguage];
}
#endif

const BYTE *InputStatusText( void )
{
    if (UserPrefInputPriorityType==Input_Priority_Auto)//Autoflag
    {
        if(CURRENT_INPUT_IS_VGA())
            return strAutoAnalogM10[UserPrefLanguage];
        else if(CURRENT_INPUT_IS_DVI())
            return strAutoDigitalM10[UserPrefLanguage];
        else if(CURRENT_INPUT_IS_HDMI())
            return strAutoHDMIM10[UserPrefLanguage];
        else if(CURRENT_INPUT_IS_DISPLAYPORT())
        {
            if(CURRENT_INPUT_IS_USBTYPEC())
            {
                return strAutoUSBCM10[UserPrefLanguage];
            }
            else
            {
                return strAutoDPM10[UserPrefLanguage];
            }
        }
        else
            return strAutoMHLM10[UserPrefLanguage];
    }
    else
    {
        if(CURRENT_INPUT_IS_VGA())
            return strAnalogInputM10[UserPrefLanguage];
        else if(CURRENT_INPUT_IS_DVI())
            return strDigitalInputM10[UserPrefLanguage];
        else if(CURRENT_INPUT_IS_HDMI())
            return strHDMIInputM10[UserPrefLanguage];
        else if(CURRENT_INPUT_IS_DISPLAYPORT())
        {
            if(CURRENT_INPUT_IS_USBTYPEC())
            {
                return strUSBC_InputM10[UserPrefLanguage];
            }
            else
            {
                return strDP_InputM10[UserPrefLanguage];
            }
        }
        else
            return strMHL_InputM10[UserPrefLanguage];
    }
}

#if (ENABLE_VGA_INPUT)
const BYTE *AutoColorProcessText( void )
{
    return strAutoColorProcessM11[UserPrefLanguage];
}
#endif

const BYTE *BrightnessText( void )
{
    return strBrightnessM101[UserPrefLanguage];
}
const BYTE *ContrastText( void )
{
    return strContrastM101[UserPrefLanguage];
}
const BYTE *DCRText( void )
{
    return strDCRM101[UserPrefLanguage];
}
#if ENABLE_DLC
const BYTE *DLCText( void )
{
    return strDLCM101[UserPrefLanguage];
}
#endif

#if ENABLE_DPS
const BYTE *DPSText( void )
{
    return strDPSM101[UserPrefLanguage];
}
const BYTE *DPSStatusText( void )
{
    if (UserprefDPSMode)
        return strOnM101[UserPrefLanguage];
    else
        return strOffM101[UserPrefLanguage];
}
#endif

const BYTE *DCRStatusText( void )
{
    if (UserPrefDcrMode)
        return strOnM101[UserPrefLanguage];
    else
        return strOffM101[UserPrefLanguage];
}
#if ENABLE_DLC
const BYTE *DLCStatusText( void )
{
    if (UserprefDLCMode)
        return strOnM101[UserPrefLanguage];
    else
        return strOffM101[UserPrefLanguage];
}
#endif
#if (ENABLE_VGA_INPUT)
const BYTE *ADCAutoColorText( void )
{
    return strAutoColorM101[UserPrefLanguage];
}
#endif

const BYTE *OnM101Text( void )
{
    return strOnM101[UserPrefLanguage];
}
const BYTE *OffM101Text( void )
{
    return strOffM101[UserPrefLanguage];
}


const BYTE *ColorTempText( void )
{
    return strColorTempM102[UserPrefLanguage];
}
const BYTE *CsRGBText( void )
{
    return strsRGBM102[UserPrefLanguage];
}
const BYTE *C5700KText( void )
{
    return str5700KM102[UserPrefLanguage];
}
const BYTE *C6500KText( void )
{
    return str6500KM102[UserPrefLanguage];
}
const BYTE *C9300KText( void )
{
    return str9300KM102[UserPrefLanguage];
}
const BYTE *UserColorText( void )
{
    return strUserColorM102[UserPrefLanguage];
}
const BYTE *RedText( void )
{
    return strRedM102[UserPrefLanguage];
}
const BYTE *GreenText( void )
{
    return strGreenM102[UserPrefLanguage];
}
const BYTE *BlueText( void )
{
    return strBlueM102[UserPrefLanguage];
}
const BYTE *HueText( void )
{
    return strHueM102[UserPrefLanguage];
}
const BYTE *SaturationText( void )
{
    return strSaturationM102[UserPrefLanguage];
}
const BYTE *ColorModeText( void )
{
    return strColorModeM102[UserPrefLanguage];
}
const BYTE *OffM102Text( void )
{
    return strOffM102[UserPrefLanguage];
}
const BYTE *EnhanceText( void )
{
    return strEnhanceM102[UserPrefLanguage];
}
#if (!MWEFunction)
const BYTE *DemoText( void )
{
    return strDemoM102[UserPrefLanguage];
}
#endif
#if ENABLE_SUPER_RESOLUTION
const BYTE *SuperResolutionText( void )
{
    return strSuperResolutionM102[UserPrefLanguage];
}
const BYTE *WeakText( void )
{
    return strWeakM102[UserPrefLanguage];
}
const BYTE *MiddleText( void )
{
    return strMiddleM102[UserPrefLanguage];
}
const BYTE *StrongText( void )
{
    return strStrongM102[UserPrefLanguage];
}

const BYTE *SuperResolutionStatusText( void )
{
    if( (UserPrefSuperResolutionMode % SRMODE_Nums)== SRMODE_STRONG)
        return strStrongM102[UserPrefLanguage];
    else if( (UserPrefSuperResolutionMode % SRMODE_Nums)== SRMODE_MIDDLE)
        return strMiddleM102[UserPrefLanguage];
    else if( (UserPrefSuperResolutionMode % SRMODE_Nums)== SRMODE_WEAK)
        return strWeakM102[UserPrefLanguage];
    else
        return strOffM102[UserPrefLanguage];
}
#endif
const BYTE *ColorTempStatusText( void )
{
    if( UserPrefColorTemp == CTEMP_Warm1 )
    {
        return str5700KM102[UserPrefLanguage];
    }
    else if( UserPrefColorTemp == CTEMP_Normal )
    {
        return str6500KM102[UserPrefLanguage];
    }
    else if( UserPrefColorTemp == CTEMP_Cool1 )
    {
        return str9300KM102[UserPrefLanguage];
    }
    else if( UserPrefColorTemp == CTEMP_SRGB )
    {
        return strsRGBM102[UserPrefLanguage];
    }
    else //if( UserPrefColorTemp == CTEMP_USER )
    {
        return strUserColorM102[UserPrefLanguage];
    }
}
const BYTE *ColorModeStatusText( void )
{
    if (UserPrefColorMode == ColorModeMenuItems_Enhance)
        return strEnhanceM102[UserPrefLanguage];
    else if (UserPrefColorMode == ColorModeMenuItems_Demo)
        return strDemoM102[UserPrefLanguage];
    else
        return strOffM102[UserPrefLanguage];
}

const BYTE *AutoSelectText( void )
{
    return strAutoSelectM103[UserPrefLanguage];
}
#if (INPUT_TYPE>=INPUT_1C)
const BYTE *DigitalPort0Text( void )
{
        if (Input_DVI_C1==Input_Digital||Input_DVI_C2==Input_Digital||Input_DVI_C3==Input_Digital)
            return strDVIM103[UserPrefLanguage];
        else if (Input_HDMI_C1==Input_Digital||Input_HDMI_C2==Input_Digital||Input_HDMI_C3==Input_Digital||Input_HDMI_C4==Input_Digital)
            return strHDMIM103[UserPrefLanguage];
        else if (Input_Displayport_C1==Input_Digital||Input_Displayport_C2==Input_Digital||Input_Displayport_C3==Input_Digital||Input_Displayport_C4==Input_Digital)
            return strDisplayPortM103[UserPrefLanguage];
        else if (Input_UsbTypeC_C3 == Input_Digital||Input_UsbTypeC_C4 ==Input_Digital)
            return strUSB_CM103[UserPrefLanguage];

}
#endif
#if (INPUT_TYPE>=INPUT_2C)
const BYTE *DigitalPort1Text( void )
{
        if (Input_DVI_C1==Input_Digital2||Input_DVI_C2==Input_Digital2||Input_DVI_C3==Input_Digital2)
        {
            return strDVIM103[UserPrefLanguage];
        }
        else if (Input_HDMI_C1==Input_Digital2||Input_HDMI_C2==Input_Digital2||Input_HDMI_C3==Input_Digital2||Input_HDMI_C4==Input_Digital2)
        {
            return strHDMIM103[UserPrefLanguage];
        }
        else if (Input_Displayport_C1==Input_Digital2||Input_Displayport_C2==Input_Digital2||Input_Displayport_C3==Input_Digital2||Input_Displayport_C4==Input_Digital2)
        {
            return strDisplayPortM103[UserPrefLanguage];
        }
        else if (Input_UsbTypeC_C3 == Input_Digital2||Input_UsbTypeC_C4 ==Input_Digital2)
        {
            return strUSB_CM103[UserPrefLanguage];
        }
}
#endif

#if (INPUT_TYPE>=INPUT_3C)
const BYTE *DigitalPort2Text( void )
{
        if (Input_DVI_C1==Input_Digital3||Input_DVI_C2==Input_Digital3||Input_DVI_C3==Input_Digital3)
            return strDVIM103[UserPrefLanguage];
        else if (Input_HDMI_C1==Input_Digital3||Input_HDMI_C2==Input_Digital3||Input_HDMI_C3==Input_Digital3||Input_HDMI_C4==Input_Digital3)
            return strHDMIM103[UserPrefLanguage];
        else if (Input_Displayport_C1==Input_Digital3||Input_Displayport_C2==Input_Digital3||Input_Displayport_C3==Input_Digital3||Input_Displayport_C4==Input_Digital3)
            return strDisplayPortM103[UserPrefLanguage];
        else if (Input_UsbTypeC_C3 == Input_Digital3||Input_UsbTypeC_C4 ==Input_Digital3)
            return strUSB_CM103[UserPrefLanguage];
}
#endif

#if (INPUT_TYPE>=INPUT_4C)
const BYTE *DigitalPort3Text( void )
{
        if (Input_DVI_C1==Input_Digital4||Input_DVI_C2==Input_Digital4||Input_DVI_C3==Input_Digital4)
            return strDVIM103[UserPrefLanguage];
        else if (Input_HDMI_C1==Input_Digital4||Input_HDMI_C2==Input_Digital4||Input_HDMI_C3==Input_Digital4||Input_HDMI_C4==Input_Digital4)
            return strHDMIM103[UserPrefLanguage];
        else if (Input_Displayport_C1==Input_Digital4||Input_Displayport_C2==Input_Digital4||Input_Displayport_C3==Input_Digital4||Input_Displayport_C4==Input_Digital4)
            return strDisplayPortM103[UserPrefLanguage];
        else if (Input_UsbTypeC_C3 == Input_Digital4||Input_UsbTypeC_C4 ==Input_Digital4)
            return strUSB_CM103[UserPrefLanguage];
}
#endif

#if (ENABLE_VGA_INPUT)
const BYTE *VGAText( void )
{
    return strVGAM103[UserPrefLanguage];
}
#endif
const BYTE *SourceSelectText( void )
{
    return strSourceSelectM103[UserPrefLanguage];
}
const BYTE *SourceSelectStatusText( void )
{
    if( UserPrefInputPriorityType == Input_Priority_Auto )
    {
        return strAutoSelectM103[UserPrefLanguage];
    }
#if (INPUT_TYPE&INPUT_1A)
    else if( UserPrefInputPriorityType == Input_Priority_Analog1 )
    {
        return strVGAM103[UserPrefLanguage];
    }
#endif
#if (INPUT_TYPE>=INPUT_1C)
    else if ( UserPrefInputPriorityType == Input_Priority_Digital1 )
    {
        return DigitalPort0Text();
    }
#endif
#if (INPUT_TYPE>=INPUT_2C)
    else if ( UserPrefInputPriorityType == Input_Priority_Digital2 )
    {
        return DigitalPort1Text();
    }
#endif
#if (INPUT_TYPE>=INPUT_3C)
    else if ( UserPrefInputPriorityType == Input_Priority_Digital3 )
    {
        return DigitalPort2Text();
    }
#endif
#if (INPUT_TYPE>=INPUT_4C)
    else if ( UserPrefInputPriorityType == Input_Priority_Digital4 )
    {
        return DigitalPort3Text();
    }
#endif
    else
    {
        return strAutoSelectM103[UserPrefLanguage];
    }
}

#if ENABLE_DP_INPUT
const BYTE *DPVersionText( void )
{
    return strDPVersionM103[UserPrefLanguage];
}
const BYTE *DP1_1Text( void )
{
    return strDP1_1M103[UserPrefLanguage];
}
const BYTE *DP1_2Text( void )
{
    return strDP1_2M103[UserPrefLanguage];
}
const BYTE *DP1_4Text( void )
{
    return strDP1_4M103[UserPrefLanguage];
}
const BYTE *DPVersionStatusText( void )
{
    if (UserPrefDPVersion == DPVersionMenuItems_DP1_1)
        return strDP1_1M103[UserPrefLanguage];
    else if (UserPrefDPVersion == DPVersionMenuItems_DP1_2)
        return strDP1_2M103[UserPrefLanguage];
    else //if (UserPrefDPVersion == DPVersionMenuItems_DP1_4)
        return strDP1_4M103[UserPrefLanguage];
}
#endif

const BYTE *_3DFormatText( void )
{
    return str3DFormatM104[UserPrefLanguage];
}
const BYTE *_3DAutoText( void )
{
    return strAutoM104[UserPrefLanguage];
}
const BYTE *_3DSidebySideText( void )
{
    return strSidebySideM104[UserPrefLanguage];
}
const BYTE *_3DTopandBottomText( void )
{
    return strTopandBottomM104[UserPrefLanguage];
}
const BYTE *_2Dto3DText( void )
{
    return str2Dto3DM104[UserPrefLanguage];
}
const BYTE *OnM104Text( void )
{
    return strOnM104[UserPrefLanguage];
}
const BYTE *OffM104Text( void )
{
    return strOffM104[UserPrefLanguage];
}
const BYTE *SeparationText( void )
{
    return strSeparationM104[UserPrefLanguage];
}
const BYTE *DepthText( void )
{
    return strDepthM104[UserPrefLanguage];
}
const BYTE *_3DFormatStatusText( void )
{
    if (1)
        return strAutoM104[UserPrefLanguage];
    else if (1)
        return strSidebySideM104[UserPrefLanguage];
    else
        return strTopandBottomM104[UserPrefLanguage];
}
const BYTE *_2Dto3DStatusText( void )
{
    if (1)
        return strOnM104[UserPrefLanguage];
    else
        return strOffM104[UserPrefLanguage];
}



#if (ENABLE_VGA_INPUT)
const BYTE *HPositionText( void )
{
    return strHPositionM105[UserPrefLanguage];
}
const BYTE *VPositionText( void )
{
    return strVPositionM105[UserPrefLanguage];
}
#endif
#if ENABLE_SHARPNESS
const BYTE *SharpnessText( void )
{
    return strSharpnessM105[UserPrefLanguage];
}
#endif
#if (ENABLE_VGA_INPUT)
const BYTE *ClockText( void )
{
    return strClockM105[UserPrefLanguage];
}
const BYTE *PhaseText( void )
{
    return strPhaseM105[UserPrefLanguage];
}
#endif
#if Enable_Expansion
const BYTE *AspectRatioText( void )
{
    return strAspectRatioM105[UserPrefLanguage];
}
const BYTE *Ratio_FullText( void )
{
    return strFullM105[UserPrefLanguage];
}
const BYTE *Ratio_43Text( void )
{
    return strRatio_43M105[UserPrefLanguage];
}
const BYTE *Ratio_169Text( void )
{
    return strRatio_169M105[UserPrefLanguage];
}
const BYTE *Ratio_11Text( void )
{
    return strRatio_11M105[UserPrefLanguage];
}
const BYTE *Ratio_FixedInputRatioText( void )
{
    return strRatio_FixedInputRatioM105[UserPrefLanguage];
}
const BYTE *Ratio_OverScanText( void )
{
    return strRatio_OverScanM105[UserPrefLanguage];
}
#endif
#if ENABLE_RTE
const BYTE *OverdriveText( void )
{
    return strOverdriveM105[UserPrefLanguage];
}
const BYTE *OverdriveStatusText( void )
{
     if (UserprefOverDriveSwitch)
        return strOnM105[UserPrefLanguage];
    else
        return strOffM105[UserPrefLanguage];
}
#endif
const BYTE *GammaText( void )
{
    return strGammaM105[UserPrefLanguage];
}
const BYTE *OnM105Text( void )
{
    return strOnM105[UserPrefLanguage];
}
const BYTE *OffM105Text( void )
{
    return strOffM105[UserPrefLanguage];
}
#if (ENABLE_VGA_INPUT)
const BYTE *AutoAdjustText( void )
{
    return strAutoAdjustM105[UserPrefLanguage];
}
#endif
#if Enable_Expansion
const BYTE *AspectRatioStatusText( void )
{
        if (UserprefExpansionMode == Expansion_4_3)  //20150728
            return strRatio_43M105[UserPrefLanguage];
        else if (UserprefExpansionMode == Expansion_16_9)
            return strRatio_169M105[UserPrefLanguage];
        else if (UserprefExpansionMode == Expansion_1_1)
            return strRatio_11M105[UserPrefLanguage];
        else if (UserprefExpansionMode == Expansion_KeepInputRatio)
            return strRatio_FixedInputRatioM105[UserPrefLanguage];
        else if (UserprefExpansionMode == Expansion_OverScan)
            return strRatio_OverScanM105[UserPrefLanguage];
        else
            return strFullM105[UserPrefLanguage];
}
#endif

const BYTE *GammaStatusText( void )
{
    if (UserPrefGamaOnOff)
        return strOnM105[UserPrefLanguage];
    else
        return strOffM105[UserPrefLanguage];
}


const BYTE *OSDTransparencyText( void )
{
    return strOSDTransparencyM106[UserPrefLanguage];
}
const BYTE *OSDHPositionText( void )
{
    return strOSDHPositionM106[UserPrefLanguage];
}
const BYTE *OSDVPositionText( void )
{
    return strOSDVPositionM106[UserPrefLanguage];
}
const BYTE *OSDTimeoutText( void )
{
    return strOSDTimeoutM106[UserPrefLanguage];
}
#if AudioFunc
const BYTE *VolumeText( void )
{
    return strVolumeM106[UserPrefLanguage];
}
const BYTE *AudioSourceText( void )
{
    return strAudioSourceM106[UserPrefLanguage];
}
const BYTE *AudioSourceValue( void )
{
	if (CURRENT_INPUT_IS_HDMI()||CURRENT_INPUT_IS_DISPLAYPORT())
	{
	    if( UserPrefAudioSource == AudioSourceMenuItems_LineIn)
	    {
	        return strAudioSourceLineInM106[UserPrefLanguage];
	    }
	    else
	    {
	        return strAudioSourceDigitalM106[UserPrefLanguage];
	    }
	}
	else
	{
	    return strAudioSourceLineInM106[UserPrefLanguage];
	}
}
const BYTE *AudioSourceLineInText( void )
{
    return strAudioSourceLineInM106[UserPrefLanguage];
}
const BYTE *AudioSourceDigitalText( void )
{
    return strAudioSourceDigitalM106[UserPrefLanguage];
}
#endif

const BYTE *PowerManagerPowerSavingText( void )
{
    return strPowerSavingM109[UserPrefLanguage];
}
const BYTE *PowerManagerPowerSavingStatusText( void )
{
    if(UserPrefPowerSavingEn)
    {
        return strPowerManagerOnM109[UserPrefLanguage];
    }
    else
    {
        return strPowerManagerOffM109[UserPrefLanguage];
    }
};

const BYTE *PowerManagerDCOffDischargeStatusText( void )
{
    if(UserPrefDCOffDischarge)
    {
        return strPowerManagerOnM109[UserPrefLanguage];
    }
    else
    {
        return strPowerManagerOffM109[UserPrefLanguage];
    }
};
const BYTE *PowerManagerDCOffDischargeText( void )
{
    return strDCOffDischargeM109[UserPrefLanguage];
}
const BYTE *PowerManagerOnText( void )
{
    return strPowerManagerOnM109[UserPrefLanguage];
}
const BYTE *PowerManagerOffText( void )
{
    return strPowerManagerOffM109[UserPrefLanguage];
}
#if ENABLE_OSD_ROTATION
const BYTE *OSDRotateText( void )
{
    return strOSDRotateM106[UserPrefLanguage];
}
const BYTE *OSDRotateStatusText( void )
{
    if (UserPrefOsdRotateMode==OSDRotateMenuItems_90)
        return strOSDRotate90M106[UserPrefLanguage];
    else
    #if defined(_OSD_ROTATION_180_)
    if (UserPrefOsdRotateMode==OSDRotateMenuItems_180)
        return strOSDRotate180M106[UserPrefLanguage];
    else
    #endif
    #if defined(_OSD_ROTATION_270_)
    if (UserPrefOsdRotateMode==OSDRotateMenuItems_270)
        return strOSDRotate270M106[UserPrefLanguage];
    else
    #endif
        return strOSDRotateOffM106[UserPrefLanguage];
}
const BYTE *OSDRotateOFFText( void )
{
    return strOSDRotateOffM106[UserPrefLanguage];
}
const BYTE *OSDRotate90Text( void )
{
    return strOSDRotate90M106[UserPrefLanguage];
}
#if defined(_OSD_ROTATION_180_)
const BYTE *OSDRotate180Text( void )
{
    return strOSDRotate180M106[UserPrefLanguage];
}
#endif
#if defined(_OSD_ROTATION_270_)
const BYTE *OSDRotate270Text( void )
{
    return strOSDRotate270M106[UserPrefLanguage];
}
#endif
#endif
const BYTE *FactoryResetText( void )
{
    return strFactoryResetM106[UserPrefLanguage];
}

#if ENABLE_FREESYNC
const BYTE *FreeSyncModeText( void )
{
    return strFREESYNCMODEM106[UserPrefLanguage];
}
const BYTE *FreeSyncModeStatusText( void )
{
    if (UserprefFreeSyncMode)
        return strOtherSettingOnM106[UserPrefLanguage];
    else
        return strOtherSettingOffM106[UserPrefLanguage];
}
#endif
const BYTE *DisplayLogoText( void )
{
    return strDisplayLogoM106[UserPrefLanguage];
}
const BYTE *DisplayLogoStatusText( void )
{
    if (UserPrefDisplayLogoEn)
        return strOtherSettingOnM106[UserPrefLanguage];
    else
        return strOtherSettingOffM106[UserPrefLanguage];
}
const BYTE *OtherSettingOFFText( void )
{
    return strOtherSettingOffM106[UserPrefLanguage];
}
const BYTE *OtherSettingONText( void )
{
    return strOtherSettingOnM106[UserPrefLanguage];
}
#if MTK_OSD_UI
const BYTE *ResolutionText( void )
{
    return strResolutionM107[UserPrefLanguage];
}
#endif
const BYTE *HFreqText( void )
{
    return strHFreqM107[UserPrefLanguage];
}
const BYTE *VFreqText( void )
{
    return strVFreqM107[UserPrefLanguage];
}
const BYTE *VTotalText( void )
{
    return strVTotalM107[UserPrefLanguage];
}

const BYTE *ColorFormatText( void )
{
    return strColorFormatM102[UserPrefLanguage];
}
const BYTE *ColorFormatStatusText( void )
{
    if (UserPrefInputColorFormat == CFMT_RGB)
        return strRGBM102[UserPrefLanguage];
    else if (UserPrefInputColorFormat == CFMT_YUV)
        return strYUVM102[UserPrefLanguage];
    else
        return strAutoM102[UserPrefLanguage];
    
}
const BYTE *RGBText( void )
{
    return strRGBM102[UserPrefLanguage];
}
const BYTE *YUVText( void )
{
    return strYUVM102[UserPrefLanguage];
}
const BYTE *AutoText( void )
{
    return strAutoM102[UserPrefLanguage];
}

const BYTE *ColorFormatDisplayText( void )
{
    return strColorFormatM107[UserPrefLanguage];
}
const BYTE *ColorFormatDisplayStatusText( void )
{
    ST_COMBO_COLOR_FORMAT cf;
    cf = msAPI_combo_IPGetColorFormat(SrcInputType);
    
    if (CURRENT_INPUT_IS_VGA())
    {
        if (UserPrefInputColorFormat == CFMT_YUV)
            return strYUV444M107[UserPrefLanguage];
        else
            return strRGBM107[UserPrefLanguage];
    }
    else
    {
        if (cf.ucColorType == COMBO_COLOR_FORMAT_YUV_444)
            return strYUV444M107[UserPrefLanguage];
        else if (cf.ucColorType == COMBO_COLOR_FORMAT_YUV_422)
            return strYUV422M107[UserPrefLanguage];
/* //TBD
        else if (cf.ucColorType == COMBO_COLOR_FORMAT_YUV_420)
            return strYUV422M107[UserPrefLanguage];
*/
        else
            return strRGBM107[UserPrefLanguage];
    }
}
#if MTK_OSD_UI
const BYTE *xText( void )
{
    return strxM107[UserPrefLanguage];
}
const BYTE *HzText( void )
{
    return strHzM107[UserPrefLanguage];
}
#endif
const BYTE *KHzText( void )
{
    return strKHzM107[UserPrefLanguage];
}
const BYTE *LineText( void )
{
    return strLineM107[UserPrefLanguage];
}


const BYTE *ColorRangeText( void )
{
    return strColorRangeM108[UserPrefLanguage];
}
const BYTE *IndependentHueText( void )
{
    return strIndependentHueM108[UserPrefLanguage];
}
const BYTE *IndependentSaturationText( void )
{
    return strIndependentSaturationM108[UserPrefLanguage];
}
const BYTE *IndependentBrightnessText( void )
{
    return strIndependentBrightnessM108[UserPrefLanguage];
}
const BYTE *ColorRangeFullText( void )
{
    return strFullM108[UserPrefLanguage];
}
const BYTE *ColorRangeLimitText( void )
{
    return strLimitM108[UserPrefLanguage];
}
const BYTE *ColorRangeAutoText( void )
{
    return strAutoM108[UserPrefLanguage];
}
const BYTE *RText( void )
{
    return strRM108[UserPrefLanguage];
}
const BYTE *GText( void )
{
    return strGM108[UserPrefLanguage];
}
const BYTE *BText( void )
{
    return strBM108[UserPrefLanguage];
}
const BYTE *YText( void )
{
    return strYM108[UserPrefLanguage];
}
const BYTE *MText( void )
{
    return strMM108[UserPrefLanguage];
}
const BYTE *CText( void )
{
    return strCM108[UserPrefLanguage];
}
const BYTE *ColorRangeStatusText( void )
{
    if( UserPrefInputColorRange == ColorRangeMenuItems_Limit)
    {
        return strLimitM108[UserPrefLanguage];
    }
    else if( UserPrefInputColorRange == ColorRangeMenuItems_Full)
    {
        return strFullM108[UserPrefLanguage];
    }
    else
    {
        return strAutoM108[UserPrefLanguage];
    }
}

//--Eson End----------------------------------


const BYTE *OsdLockText( void )
{
    return strOsdLockM56[UserPrefLanguage];
}

const BYTE *ECOModeText( void )
{
    return strEcoModeM7[UserPrefLanguage];
}
const BYTE *ECOModeValue( void )
{
    if( UserPrefECOMode == 0 )
    {
        return strECOStandM7[UserPrefLanguage];
    }
    else if( UserPrefECOMode == 1 )
    {
        return strECOTextM7[UserPrefLanguage];
    }
    else if( UserPrefECOMode == 2 )
    {
        return strECOInternetM7[UserPrefLanguage];
    }
    else if( UserPrefECOMode == 3 )
    {
        return strECOgameM7[UserPrefLanguage];
    }
    else if( UserPrefECOMode == 4 )
    {
        return strECOMovieM7[UserPrefLanguage];
    }
    else if( UserPrefECOMode == 5 )
    {
        return strECOSportM7[UserPrefLanguage];
    }
    else
    {
        return strECOStandM7[UserPrefLanguage];
    }
}



#if MWEFunction
/*
const BYTE *ColorBoostText( void )
{
    return strColorBoostM26[UserPrefLanguage];
}
*/
const BYTE *FullText( void )
{
    return strFullM26[UserPrefLanguage];
}
const BYTE *NatureSkinText( void )
{
    return strNatureSkinM26[UserPrefLanguage];
}
const BYTE *GreenFieldText( void )
{
    return strGreenFieldM26[UserPrefLanguage];
}
const BYTE *SkyBlueText( void )
{
    return strSkyBlueM26[UserPrefLanguage];
}
const BYTE *AutoDetectText( void )
{
    return strAutoDetectM26[UserPrefLanguage];
}
const BYTE *DemoText( void )
{
    return strDemoM26[UserPrefLanguage];
}
const BYTE *FreeColorFullModeText( void )
{
    if( FullEnhanceFlag )
    {
        return strOnM26[UserPrefLanguage];
    }
    else
    {
        return strOffM26[UserPrefLanguage];
    }
}
const BYTE *FreeColorSkinProtectModeText( void )
{
    if( SkinFlag )
    {
        return strOnM26[UserPrefLanguage];
    }
    else
    {
        return strOffM26[UserPrefLanguage];
    }
}
const BYTE *FreeColorGreenEnhanceModeText( void )
{
    if( GreenFlag )
    {
        return strOnM26[UserPrefLanguage];
    }
    else
    {
        return strOffM26[UserPrefLanguage];
    }
}
const BYTE *FreeColorBlueEnhanceModeText( void )
{
    if( BlueFlag )
    {
        return strOnM26[UserPrefLanguage];
    }
    else
    {
        return strOffM26[UserPrefLanguage];
    }
}
const BYTE *FreeColorAutoDetectModeText( void )
{
    if( ColorAutoDetectFlag )
    {
        return strOnM26[UserPrefLanguage];
    }
    else
    {
        return strOffM26[UserPrefLanguage];
    }
}
const BYTE *FreeColorDemoModeText( void )
{
    if( DemoFlag )
    {
        return strOnM26[UserPrefLanguage];
    }
    else
    {
        return strOffM26[UserPrefLanguage];
    }
}
/*
const BYTE *PictureBoostText( void ) // picture boost
{
    return strPictureBoostM33[UserPrefLanguage];
}
*/
const BYTE *BF_FrameSizeText( void ) // frame size
{
    return strBF_FrameSizeM33[UserPrefLanguage];
}

const BYTE *BF_BrightnessText( void ) // brightness
{
    return strBF_BrightnessM33[UserPrefLanguage];
}

const BYTE *BF_ContrastText( void ) // contrast
{
    return strBF_ContrastM33[UserPrefLanguage];
}

const BYTE *BFHPositionText( void )
{
    return strBF_HPositionM33[UserPrefLanguage];
}

const BYTE *BFVPositionText( void )
{
    return strBF_VPositionM33[UserPrefLanguage];
}

const BYTE *BF_SwitchText( void ) // switch(brith frame)
{
    return strBF_SwitchM33[UserPrefLanguage];
}

const BYTE *BF_SwitchValueText( void )
{
    if( PictureBoostFlag )
    {
        return strOnM33[UserPrefLanguage];
    }
    else
    {
        return strOffM33[UserPrefLanguage];
    }
}
#endif


const BYTE *ResetText( void )
{
    return strResetM49[UserPrefLanguage];
}


#if DDCCI_ENABLE
const BYTE *DDCCIText( void )
{
    return strDDCCIM49[UserPrefLanguage];
}
const BYTE *DDCCIValueText( void )
{
    if( DDCciFlag )
    {
        return strOnM49[UserPrefLanguage];
    }
    else
    {
        return strOffM49[UserPrefLanguage];
    }
}
#endif

#if ENABLE_DeBlocking
const BYTE *DeBlockingText( void )
{
    return strDEBLOCKINGM13[UserPrefLanguage];
}
const BYTE *DeBlockingValueText( void )
{
    if( UserPrefDeBlocking)
    {
        return strOnM13[UserPrefLanguage];
    }
    else
    {
        return strOffM13[UserPrefLanguage];
    }
}
#endif

const BYTE *YesText( void )
{
    return strYesM49[UserPrefLanguage];
}

const BYTE *NoText( void )
{
    return strNoM49[UserPrefLanguage];
}

const BYTE *AutoColorPassText( void )
{
    return strAutoColorPassM57[UserPrefLanguage];
}
const BYTE *AutoColorFailText( void )
{
    return strAutoColorFailM57[UserPrefLanguage];
}
const BYTE *AutoColorMenuText( void )
{
    return strAutoColorM57[UserPrefLanguage];
}
#if 0
const BYTE *FactoryIcon( void )
{
    return strFactoryIcon;
}
const BYTE *OsdLockText( void )
{
    return strOsdLockText[UserPrefLanguage];
}
#endif
//======================================================================
