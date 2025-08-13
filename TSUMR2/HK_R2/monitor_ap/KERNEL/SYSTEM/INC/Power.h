extern void Power_PowerHandler( void );
extern void Power_PowerOnSystem( void );
extern void Power_PowerOffSystem( void );
extern void Power_TurnOffLed( void );
extern void Power_TurnOnGreenLed( void );
extern void Power_TurnOffGreenLed( void );
extern void Power_TurnOnAmberLed( void );
extern void Power_TurnOffAmberLed( void );
#if Enable_PanelHandler
extern PANEL_POWER_TIMING_INFO Power_PanelCtrlCounter_Get(void);
extern void Power_PanelCtrlCounter_Set(PANEL_POWER_TIMING_INFO sPanelPowerTimingInfo);
extern ePANEL_STATE Power_PanelCtrlState_Get(void);
extern void Power_PanelCtrlState_ForceJump(ePANEL_STATE eState, MS_S32 CustomCounter);
extern BOOL Power_PanelCtrlStateStopFlag_Get(ePANEL_STATE eState);
extern void Power_PanelCtrlStateStopFlag_Set(ePANEL_STATE eState);
extern void Power_PanelCtrlStateStopFlag_Clr(ePANEL_STATE eState);
extern void Power_PanelCtrlStateMachineChange(BOOL ISRTrigger);
extern void Power_PanelCtrlHandler(void);
extern void Power_PanelCtrlIsr(void);
extern BOOL Power_PanelCtrlOnOff(BOOL bOn, BOOL bForce);
#else
extern void Power_TurnOnPanel( void );
extern void Power_TurnOffPanel( void );
#if Enable_ReducePanelPowerOnTime
extern void Power_ForcePowerOnPanel(void);
#endif
#endif
