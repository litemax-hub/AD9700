extern WORD GetNonFuncValue( void );

extern Bool ResetOsdTimer( void );
extern Bool EnablePowerDownCounter( void );
extern Bool PowerOffSystem( void );
extern Bool PowerOnSystem( void );
extern Bool EnterRootMenu( void );
extern Bool OsdCountDown( void );
//==============ready================
extern Bool AdjustContrast( MenuItemActionType action );
extern WORD GetContrastValue( void );
#if AudioFunc
extern Bool AdjustVolume( MenuItemActionType action );
extern WORD GetVolumeValue( void );
extern Bool SetAudioSource(void);
#endif
extern Bool AdjustBrightness( MenuItemActionType action );
extern WORD GetBrightnessValue( void );

extern Bool AdjustFocus( MenuItemActionType action );
extern WORD GetFocusValue( void );
extern Bool AdjustClock( MenuItemActionType action );
extern WORD GetClockValue( void );
extern Bool AdjustHPosition( MenuItemActionType action );
extern WORD GetHPositionValue( void );
extern Bool AdjustVPosition( MenuItemActionType action );
extern WORD GetVPositionValue( void );
extern WORD GetSourceValue( void );
extern Bool AdjustSource( MenuItemActionType action );
extern WORD GetLanguageValue( void );
extern Bool AdjustLanguage( MenuItemActionType action );
extern Bool AdjustBlueColor( MenuItemActionType action );
extern WORD GetBlueColorValue( void );
extern Bool AdjustGreenColor( MenuItemActionType action );
extern WORD GetGreenColorValue( void );
extern Bool AdjustRedColor( MenuItemActionType action );
extern WORD GetRedColorValue( void );
extern WORD GetSharpnessValue( void );
extern WORD GetSharpnessRealValue( void );
extern Bool AdjustSharpness( MenuItemActionType action );
extern Bool ResetAllSetting( void );
extern Bool AutoConfig( void );
extern Bool SwitchSource( void );
//extern void SaveUserPref(void);
extern Bool ChangeSourceToDigital( void );
extern Bool ChangeSourceToHDMI( void );
extern Bool ChangeSource( void );
#if ENABLE_DP_INPUT
extern Bool ChangeMSTOnOff(void);
extern Bool ChangeDPVersion(void);
extern Bool SetMSTOnOff( void );
extern Bool SetMSTOff( void );
extern Bool SetDPVersion( BYTE DPVersion );
extern void SetDPOfflinePortMSToff(BYTE InputType);
#endif
extern Bool AdjustECOMode( MenuItemActionType action );
#if Enable_Gamma
extern Bool AdjustGamaMode( MenuItemActionType action );
#endif
extern Bool AdjustDcrMode( MenuItemActionType action );
extern Bool SetDCRmode(void);

#if ENABLE_DLC
extern Bool AdjustDLCMode( MenuItemActionType action );
extern Bool SetDLCmode(void);
#endif
#if ENABLE_3DLUT
extern Bool Adjust3DLUTMode( MenuItemActionType action );
#endif
#if ENABLE_DPS
extern Bool AdjustDPSMode( MenuItemActionType action );
extern Bool SetDPSmode(void);
#endif
#if ENABLE_SUPER_RESOLUTION
extern Bool AdjustSuperResolutionMode( MenuItemActionType action );
#endif
#if ENABLE_RTE
extern Bool SetOverDriveOnOffmode(void);
#endif
#if ENABLE_DeBlocking
extern Bool AdjustDeBlockingMode( MenuItemActionType action );
extern void msDeBlockingOnOff( BYTE ucSwitch, BYTE win);
extern void msInitDeBlocking( void );
#endif
extern Bool AdjustColorTempMode( MenuItemActionType action );
#if MWEFunction
extern Bool AdjustColorBoostMode( MenuItemActionType action );
extern Bool AdjustSkinProtectMode( MenuItemActionType action );
extern Bool AdjustGreenEnhanceMode( MenuItemActionType action );
extern Bool AdjustBlueEnhanceMode( MenuItemActionType action );
extern Bool AdjustAutoDetectMode( MenuItemActionType action );
extern Bool AdjustDemoMode( MenuItemActionType action );
#endif
extern Bool SetToEnglish( void );
#if 1
extern Bool AutoColor( void );
/*#if USEFLASH
    extern Bool EraseFlashSettingCell(void);
#else*/
#if !USEFLASH
extern Bool EraseAllEEPROMCell( void );
#else
extern Bool EraseFlashUserData(void); //2011.9.13 14:54 CC
#endif
extern Bool AdjustBlueGain( MenuItemActionType action );
extern WORD GetBlueGainValue( void );
extern Bool AdjustGreenGain( MenuItemActionType action );
extern WORD GetGreenGainValue( void );
extern Bool AdjustRedGain( MenuItemActionType action );
extern WORD GetRedGainValue( void );
extern Bool AdjustAdcBlueOffset( MenuItemActionType action );
extern WORD GetAdcBlueOffsetValue( void );
extern Bool AdjustAdcGreenOffset( MenuItemActionType action );
extern WORD GetAdcGreenOffsetValue( void );
extern Bool AdjustAdcRedOffset( MenuItemActionType action );
extern WORD GetAdcRedOffsetValue( void );
extern Bool SetFactoryColorTempCool1( void );
extern Bool SetFactoryColorTempCool2( void );
extern Bool SetFactoryColorTempWarm1( void );
extern Bool SetFactoryColorTempWarm2( void );
extern Bool SetFactoryColorTempNormal( void );
extern Bool SetFactoryColorTempSRGB( void );
extern Bool AdjustRedColorWarm1( MenuItemActionType action );
extern Bool AdjustGreenColorWarm1( MenuItemActionType action );
extern Bool AdjustBlueColorWarm1( MenuItemActionType action );
extern Bool AdjustBrightnessWarm1( MenuItemActionType action );
extern Bool AdjustContrastWarm1( MenuItemActionType action );
extern WORD GetBColorWarm1Value( void );
extern WORD GetGColorWarm1Value( void );
extern WORD GetRColorWarm1Value( void );
extern WORD GetBrightnessWarm1Value( void );
extern WORD GetContrastWarm1Value( void );
extern Bool AdjustRedColorWarm2( MenuItemActionType action );
extern Bool AdjustGreenColorWarm2( MenuItemActionType action );
extern Bool AdjustBlueColorWarm2( MenuItemActionType action );
extern Bool AdjustBrightnessWarm2( MenuItemActionType action );
extern Bool AdjustContrastWarm2( MenuItemActionType action );
extern WORD GetBColorWarm2Value( void );
extern WORD GetGColorWarm2Value( void );
extern WORD GetRColorWarm2Value( void );
extern WORD GetBrightnessWarm2Value( void );
extern WORD GetContrastWarm2Value( void );

extern Bool AdjustRedColorCool1( MenuItemActionType action );
extern Bool AdjustGreenColorCool1( MenuItemActionType action );
extern Bool AdjustBlueColorCool1( MenuItemActionType action );
extern Bool AdjustBrightnessCool1( MenuItemActionType action );
extern Bool AdjustContrastCool1( MenuItemActionType action );
extern WORD GetBColorCool1Value( void );
extern WORD GetGColorCool1Value( void );
extern WORD GetRColorCool1Value( void );
extern WORD GetBrightnessCool1Value( void );
extern WORD GetContrastCool1Value( void );
extern Bool AdjustRedColorCool2( MenuItemActionType action );
extern Bool AdjustGreenColorCool2( MenuItemActionType action );
extern Bool AdjustBlueColorCool2( MenuItemActionType action );
extern Bool AdjustBrightnessCool2( MenuItemActionType action );
extern Bool AdjustContrastCool2( MenuItemActionType action );
extern WORD GetBColorCool2Value( void );
extern WORD GetGColorCool2Value( void );
extern WORD GetRColorCool2Value( void );
extern WORD GetBrightnessCool2Value( void );
extern WORD GetContrastCool2Value( void );
extern Bool AdjustRedColorNormal( MenuItemActionType action );
extern Bool AdjustGreenColorNormal( MenuItemActionType action );
extern Bool AdjustBlueColorNormal( MenuItemActionType action );
extern Bool AdjustBrightnessNormal( MenuItemActionType action );
extern Bool AdjustContrastNormal( MenuItemActionType action );
extern WORD GetBColorNormalValue( void );
extern WORD GetGColorNormalValue( void );
extern WORD GetRColorNormalValue( void );
extern WORD GetBrightnessNormalValue( void );
extern WORD GetContrastNormalValue( void );
extern Bool AdjustRedColorSRGB( MenuItemActionType action );
extern Bool AdjustGreenColorSRGB( MenuItemActionType action );
extern Bool AdjustBlueColorSRGB( MenuItemActionType action );
extern Bool AdjustBrightnessSRGB( MenuItemActionType action );
extern Bool AdjustContrastSRGB( MenuItemActionType action );
extern WORD GetBColorSRGBValue( void );
extern WORD GetGColorSRGBValue( void );
extern WORD GetRColorSRGBValue( void );
extern WORD GetBrightnessSRGBValue( void );
extern WORD GetContrastSRGBValue( void );
extern Bool AdjustYColor( MenuItemActionType action );
extern Bool AdjustCColor( MenuItemActionType action );
extern Bool AdjustMColor( MenuItemActionType action );
extern WORD GetYColorValue( void );
extern WORD GetCColorValue( void );
extern WORD GetMColorValue( void );
extern Bool EnableBurninMode( void );
extern WORD GetBurninValue( void );
extern Bool AdjustBurnin( void );

#if ENABLE_FACTORY_SSCADJ
extern  Bool AdjustFactorySSCModulation( MenuItemActionType action );//CC 2011.9.13 18:51
extern  WORD GetFactorySSCModulationValue( void );//CC 2011.9.13 18:51
extern  Bool AdjustFactorySSCPercentage( MenuItemActionType action );//CC 2011.9.13 18:51
extern  WORD GetFactorySSCPercentageValue( void );//CC 2011.9.13 18:51
#endif
//extern WORD GetSSCStepValue( void ); //CC 2011.9.9 15:26
//extern WORD GetSSCSpanValue( void ); //CC 2011.9.9 15:26
extern WORD GetProductionValue( void ); //CC 2011.9.9 10:00
extern Bool AdjustProduction(MenuItemActionType action); //CC 2011.9.9 10:00
extern WORD GetDDCWPActiveValue( void ); //CC 2011.9.9 16:00
extern Bool AdjustDDCWP( MenuItemActionType action ); //CC 2011.9.9 16:00
extern Bool AdjustBankNo( MenuItemActionType action );
extern Bool AdjustAddr( MenuItemActionType action );
extern WORD GetAddrValue( void );
extern Bool AdjustRegValue( MenuItemActionType action );
extern WORD GetAddrRegValue( void );
extern WORD GetRegValue( void );
extern Bool WriteRegValue( void );
//extern WORD GetOSDTime100Value(void);
extern Bool SetColorTemp( void );
extern void ReadColorTempSetting( void );
extern void SaveFactorySettingByItem( BYTE itemIndex );
extern void SaveFactorySetting( void );
extern void SetECO( void );
extern Bool EnterISPMode( void );
#endif
extern Bool AdjustOSDHPosition( MenuItemActionType action );
extern WORD GetOSDHPositionValue( void );
extern Bool AdjustOSDVPosition( MenuItemActionType action );
extern WORD GetOSDVPositionValue( void );
extern Bool AdjustOSDTime( MenuItemActionType action );
extern WORD GetOSDTimeValue( void );
extern WORD GetOSDTime100Value( void );
extern Bool AdjustOSDTransparency( MenuItemActionType action );
extern WORD GetOSDTransparencyValue( void );

//extern Bool DrawMenuTitle(void);
extern Bool DrawTimingInfo( void );
extern Bool EnterDisplayLogo( void );
#if DDCCI_ENABLE
extern Bool AdjustDDCCI( MenuItemActionType action );
//    extern Bool ExecDDC2Bi(void);
extern Bool ResetGeometry( void );
extern Bool ResetColorTemp( void );
extern void AdjustTempColorTemp( BYTE ctemp );
#endif
#if Enable_Expansion
extern void SetExpansionMode( void );
extern Bool OSDSetExpansionMode( void );
#endif
extern Bool AdjustBlue( MenuItemActionType action );
extern Bool AdjustGreen( MenuItemActionType action );
extern Bool AdjustMWE( MenuItemActionType action );
#if MWEFunction
extern Bool AdjustSubContrast( MenuItemActionType action );
extern Bool AdjustSubBrightness( MenuItemActionType action );
extern Bool AdjustBFSize( MenuItemActionType action );
extern Bool AdjustBFHstart( MenuItemActionType action );
extern WORD GetBFHstartValue( void );
extern Bool AdjustBFVstart( MenuItemActionType action );
extern WORD GetBFVstartValue( void );
extern WORD GetBFSizeValue( void );
extern WORD GetSubContrast100Value( void );
extern WORD GetSubBrightness100Value( void );
extern Bool AdjustBF_Switch( MenuItemActionType action );
extern WORD GetBF_SwitchValue( void );
extern Bool SetBFSize( BYTE ucSize );
#if !ENABLE_HDMI
extern void SetColorEnhance( bit enable );
#endif
#endif
extern WORD GetPanelOnTimeValue( void );
extern Bool ResetGeometry( void );
#if 0//ENABLE_RTE
extern  WORD GetOverDriveValue( void );
extern  Bool AdjustOverDrive( MenuItemActionType action );
#endif
#if PanelRSDS
extern WORD GetVCOMValue( void );
extern Bool AdjustVCOMValue( MenuItemActionType action );
#endif

#if ENABLE_SUPER_RESOLUTION
extern Bool SetSuperResolutionMode(void);
#endif
#if ENABLE_SHARPNESS
extern Bool AdjustSharpness( MenuItemActionType action );
extern WORD GetSharpnessValue( void );
#endif

extern void MenuFuncuncall(void);

extern Bool SetColorFormat(void);


#if ENABLE_OSD_ROTATION
extern Bool SetOSDRotateMode(void);
#endif
#if ENABLE_FREESYNC
BYTE GetSrcFreesyncFPS(void);
extern Bool SetDRRFunction(void);
#endif

extern Bool OSD_SetColorTemp(void);
extern Bool AdjustVideoSaturation(MenuItemActionType enAction);
extern WORD GetSaturationValue( void );

extern Bool AdjustVideoHUE(MenuItemActionType enAction);
extern WORD GetHueValue( void );

extern Bool AdjustIndependentHue_R(MenuItemActionType enAction);
extern WORD GetIndependentHueRValue( void );
extern Bool AdjustIndependentHue_G(MenuItemActionType enAction);
extern WORD GetIndependentHueGValue( void );
extern Bool AdjustIndependentHue_B(MenuItemActionType enAction);
extern WORD GetIndependentHueBValue( void );
extern Bool AdjustIndependentHue_C(MenuItemActionType enAction);
extern WORD GetIndependentHueCValue( void );
extern Bool AdjustIndependentHue_M(MenuItemActionType enAction);
extern WORD GetIndependentHueMValue( void );
extern Bool AdjustIndependentHue_Y(MenuItemActionType enAction);
extern WORD GetIndependentHueYValue( void );
extern Bool AdjustIndependentSaturation_R(MenuItemActionType enAction);
extern WORD GetIndependentSaturationRValue( void );
extern Bool AdjustIndependentSaturation_G(MenuItemActionType enAction);
extern WORD GetIndependentSaturationGValue( void );
extern Bool AdjustIndependentSaturation_B(MenuItemActionType enAction);
extern WORD GetIndependentSaturationBValue( void );
extern Bool AdjustIndependentSaturation_C(MenuItemActionType enAction);
extern WORD GetIndependentSaturationCValue( void );
extern Bool AdjustIndependentSaturation_M(MenuItemActionType enAction);
extern WORD GetIndependentSaturationMValue( void );
extern Bool AdjustIndependentSaturation_Y(MenuItemActionType enAction);
extern WORD GetIndependentSaturationYValue( void );
extern Bool AdjustIndependentBrightness_R(MenuItemActionType enAction);
extern WORD GetIndependentBrightnessRValue( void );
extern Bool AdjustIndependentBrightness_G(MenuItemActionType enAction);
extern WORD GetIndependentBrightnessGValue( void );
extern Bool AdjustIndependentBrightness_B(MenuItemActionType enAction);
extern WORD GetIndependentBrightnessBValue( void );
extern Bool AdjustIndependentBrightness_C(MenuItemActionType enAction);
extern WORD GetIndependentBrightnessCValue( void );
extern Bool AdjustIndependentBrightness_M(MenuItemActionType enAction);
extern WORD GetIndependentBrightnessMValue( void );
extern Bool AdjustIndependentBrightness_Y(MenuItemActionType enAction);
extern WORD GetIndependentBrightnessYValue( void );

extern Bool SetGammaOnOff(void);
extern Bool SetColorRange(void);
extern Bool SetColorMode(void);
extern Bool SetColorModeOFF(void);
extern Bool SetColorModeEnhance(void);
extern int GetHSYMappingValue(BYTE u8Mode, WORD u16Value);
extern void AdjustAllIndependentHSYValue(void);
extern void AdjustVideoHueSaturation(void);
extern Bool MenuFunc_DCOffDischarge_Set(void);
extern Bool MenuFunc_PowerSavingEn_Set(void);
extern Bool MenuFunc_DisplayLogoEn_Set(void);
extern WORD MenuFunc_GetVfreq(void);
extern Bool AdjustFBrightness_0( MenuItemActionType action );
extern WORD GetFBrightness_0Value( void );
extern Bool AdjustFBrightness_25( MenuItemActionType action );
extern WORD GetFBrightness_25Value( void );
extern Bool AdjustFBrightness_50( MenuItemActionType action );
extern WORD GetFBrightness_50Value( void );
extern Bool AdjustFBrightness_75( MenuItemActionType action );
extern WORD GetFBrightness_75Value( void );
extern Bool AdjustFBrightness_100( MenuItemActionType action );
extern WORD GetFBrightness_100Value( void );
extern Bool FactoryReset(void);

