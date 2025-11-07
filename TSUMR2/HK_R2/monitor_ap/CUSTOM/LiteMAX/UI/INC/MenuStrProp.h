//--Eson Start----------------------------------
extern BYTE *NetAddrText( void );
extern BYTE *BriteContText( void );
#if (ENABLE_VGA_INPUT)
extern BYTE *AutoAdjustText( void );
#endif
extern BYTE *InputSourceText( void );
extern BYTE *ColorSettingsText( void );
extern BYTE *DisplaySettingsText( void );
extern BYTE *OtherSettingsText( void );
extern BYTE *InformationText( void );
extern BYTE *ExtColorSettingsText( void );
extern BYTE *ExitText( void );
extern BYTE *ReturnText( void );
extern BYTE *PowerManagerText( void );
extern BYTE *NoSignalText( void );

extern  BYTE *CableNotConnectedText( void );

extern  BYTE *OutofRangeText( void );

extern BYTE *ResetProgressText( void );

#if (ENABLE_VGA_INPUT)
extern BYTE *AutoAdjustProgressText( void );
#endif

extern BYTE *InputStatusText( void );

#if (ENABLE_VGA_INPUT)
extern BYTE *AutoColorProcessText( void );
#endif

extern BYTE *BrightnessText( void );
extern BYTE *ContrastText( void );
extern BYTE *DCRText( void );
extern BYTE *DLCText( void );
extern BYTE *DPSText( void );
#if (ENABLE_VGA_INPUT)
extern BYTE *ADCAutoColorText( void );
#endif
extern BYTE *DCRStatusText( void );
extern BYTE *DLCStatusText( void );
extern BYTE *DPSStatusText( void );
extern BYTE *OnM101Text( void );
extern BYTE *OffM101Text( void );
extern BYTE *ColorTempText( void );
extern BYTE *CsRGBText( void );
extern BYTE *C5700KText( void );
extern BYTE *C6500KText( void );
extern BYTE *C9300KText( void );
extern BYTE *UserColorText( void );
extern BYTE *RedText( void );
extern BYTE *GreenText( void );
extern BYTE *BlueText( void );
extern BYTE *HueText( void );
extern BYTE *SaturationText( void );
extern BYTE *ColorModeText( void );
extern BYTE *OffM102Text( void );
extern BYTE *EnhanceText( void );
#if (!MWEFunction)
extern BYTE *DemoText( void );
#endif
extern BYTE *SuperResolutionText( void );
extern BYTE *WeakText( void );
extern BYTE *MiddleText( void );
extern BYTE *StrongText( void );
extern BYTE *ColorTempStatusText( void );
extern BYTE *ColorModeStatusText( void );
extern BYTE *SuperResolutionStatusText( void );
//extern BYTE *ColorFormatStatusText( void );
extern BYTE *RGBText( void );
extern BYTE *YUVText( void );
extern BYTE *AutoText( void );
extern BYTE *AutoSelectText( void );
extern BYTE *SourceSelectText( void );
extern BYTE *SourceSelectStatusText(void);
#if (ENABLE_VGA_INPUT)
extern BYTE *VGAText( void );
#endif
extern BYTE *DigitalPort0Text( void );
extern BYTE *DigitalPort1Text( void );
extern BYTE *DigitalPort2Text( void );
extern BYTE *DigitalPort3Text( void );
#if ENABLE_DP_INPUT
extern BYTE *DPVersionText(void);
extern BYTE *DPVersionStatusText(void);
extern BYTE *DP1_1Text(void);
extern BYTE *DP1_2Text(void);
extern BYTE *DP1_4Text(void);
#endif
extern BYTE *_3DFormatText( void );
extern BYTE *_3DAutoText( void );
extern BYTE *_3DSidebySideText( void );
extern BYTE *_3DTopandBottomText( void );
extern BYTE *_2Dto3DText( void );
extern BYTE *OnM104Text( void );
extern BYTE *OffM104Text( void );
extern BYTE *SeparationText( void );
extern BYTE *DepthText( void );
extern BYTE *_3DFormatStatusText( void );
extern BYTE *_2Dto3DStatusText( void );
#if (ENABLE_VGA_INPUT)
extern BYTE *HPositionText( void );
extern BYTE *VPositionText( void );
#endif
extern BYTE *SharpnessText( void );
#if (ENABLE_VGA_INPUT)
extern BYTE *ClockText( void );
extern BYTE *PhaseText( void );
#endif
#if Enable_Expansion
extern BYTE *AspectRatioText( void );
extern BYTE *Ratio_FullText( void );
extern BYTE *Ratio_43Text( void );
extern BYTE *Ratio_169Text( void );
extern BYTE *Ratio_11Text( void );
extern BYTE *Ratio_FixedInputRatioText( void );
extern BYTE *Ratio_OverScanText( void );
#endif
#if ENABLE_RTE
extern BYTE *OverdriveText( void );
extern BYTE *OverdriveStatusText( void );
#endif
extern BYTE *GammaText( void );
extern BYTE *OnM105Text( void );
extern BYTE *OffM105Text( void );
#if Enable_Expansion
extern BYTE *AspectRatioStatusText( void );
#endif
extern BYTE *GammaStatusText( void );

extern BYTE *OSDTransparencyText( void );
extern BYTE *OSDHPositionText( void );
extern BYTE *OSDVPositionText( void );
extern BYTE *OSDTimeoutText( void );
#if ENABLE_OSD_ROTATION
extern BYTE *OSDRotateText( void );
extern BYTE *OSDRotateStatusText( void );
extern BYTE *OSDRotateOFFText( void );
extern BYTE *OSDRotate90Text( void );
extern BYTE *OSDRotate180Text( void );
extern BYTE *OSDRotate270Text( void );
#endif

#if ENABLE_FREESYNC
extern BYTE *FreeSyncModeText( void );
#endif

extern BYTE *DisplayLogoText( void);
extern BYTE *DisplayLogoStatusText( void );
extern BYTE *OtherSettingONText( void );
extern BYTE *OtherSettingOFFText( void );
extern BYTE *FreeSyncModeStatusText( void );

#if AudioFunc
extern BYTE *VolumeText( void );
extern BYTE *AudioSourceText( void );
extern BYTE *AudioSourceValue( void );
extern BYTE *AudioSourceLineInText( void );
extern BYTE *AudioSourceDigitalText( void );
#endif
extern BYTE *FactoryResetText( void );

extern BYTE *PowerManagerPowerSavingText( void );
extern BYTE *PowerManagerPowerSavingStatusText( void );
extern BYTE *PowerManagerDCOffDischargeStatusText( void );
extern BYTE *PowerManagerDCOffDischargeText( void );
extern BYTE *PowerManagerOnText( void );
extern BYTE *PowerManagerOffText( void );


extern BYTE *ResolutionText( void );
extern BYTE *HFreqText( void );
extern BYTE *VFreqText( void );
extern BYTE *VTotalText( void );

extern BYTE *ColorFormatDisplayText( void );
extern BYTE *ColorFormatDisplayStatusText( void );
extern BYTE *ColorFormatText( void );
extern BYTE *ColorFormatStatusText( void );
extern BYTE *xText( void );
extern BYTE *HzText( void );
extern BYTE *KHzText( void );
extern BYTE *LineText( void );


//--Eson End----------------------------------

extern BYTE *ColorRangeText( void );
extern BYTE *IndependentHueText( void );
extern BYTE *IndependentSaturationText( void );
extern BYTE *IndependentBrightnessText( void );
extern BYTE *ColorRangeFullText( void );
extern BYTE *ColorRangeLimitText( void );
extern BYTE *ColorRangeAutoText( void );
extern BYTE *RText( void );
extern BYTE *GText( void );
extern BYTE *BText( void );
extern BYTE *YText( void );
extern BYTE *MText( void );
extern BYTE *CText( void );
extern BYTE *ColorRangeStatusText( void );

extern BYTE *VersionDateText( void );
extern BYTE *ChangeDateText( void );
extern BYTE *DateText( void );
extern BYTE *HourText( void );
extern BYTE *PotText( void );


extern BYTE *ContrastText( void );
extern BYTE *ECOModeText( void );
extern BYTE *ECOModeValue( void );


#if ENABLE_DeBlocking
extern BYTE *DeBlockingText( void );
extern BYTE *DeBlockingValueText( void );
#endif



#if MWEFunction
//extern BYTE *ColorBoostText( void );
extern BYTE *FullText( void );
extern BYTE *NatureSkinText( void );
extern BYTE *GreenFieldText( void );
extern BYTE *SkyBlueText( void );
extern BYTE *AutoDetectText( void );
extern BYTE *DemoText( void );
extern BYTE *FreeColorBarText( void );
extern BYTE *FreeColorFullModeText( void );
extern BYTE *FreeColorSkinProtectModeText( void );
extern BYTE *FreeColorGreenEnhanceModeText( void );
extern BYTE *FreeColorBlueEnhanceModeText( void );
extern BYTE *FreeColorAutoDetectModeText( void );
extern BYTE *FreeColorDemoModeText( void );
extern BYTE *BFHPositionText( void );
extern BYTE *BFVPositionText( void );
extern BYTE *BFPositionText( void );
//extern BYTE *PictureBoostText( void );
extern BYTE *BF_FrameSizeText( void );
extern BYTE *BF_BrightnessText( void );
extern BYTE *BF_ContrastText( void );
extern BYTE *BF_HueText( void );
extern BYTE *BF_SaturationText( void );
extern BYTE *BF_PositionText( void );
extern BYTE *BF_RightArrowText( void );
extern BYTE *BF_SwitchText( void );
extern BYTE *BF_SwitchValueText( void );
#endif
extern BYTE *ResetText( void );
extern BYTE *YesText( void );
extern BYTE *NoText( void );
extern BYTE *ExtraBarText( void );
#if DDCCI_ENABLE
extern BYTE *DDCCIText( void );
extern BYTE *DDCCIValueText( void );
#endif
#if USEFLASH
extern BYTE *EraseNVRamText( void );
extern BYTE *OffText(void);
#else
extern BYTE *EraseEEPROMText( void );
#endif
extern BYTE *OnText(void);

extern BYTE *InformationExtraText( void );
extern BYTE *AutoColorMenuText( void );
extern BYTE *AutoColorPassText( void );
extern BYTE *AutoColorFailText( void );
extern BYTE *OsdLockText( void );
extern BYTE *COOL1Text( void );
extern BYTE *COOL2Text( void );
extern BYTE *WARM1Text( void );
extern BYTE *WARM2Text( void );
extern BYTE *sRGBText( void );
extern BYTE *NORMALText( void );
extern BYTE *EnterBurninText( void );
extern BYTE *ExitBurninText( void );
#if LiteMAX_OSD_TEST
extern BYTE *PixelClockText( void );
extern BYTE *MHzText( void );
extern BYTE *DotText( void );
#endif
//====================================
