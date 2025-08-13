#ifndef _DRVHDRCOMM_H_
#define _DRVHDRCOMM_H_
//#if ENABLE_HDR
extern BOOL msSetHDREnable(BYTE u8WinIdx, BYTE enHDR);
extern void AutodetectHDR_Handler(void);
WORD msGetLumMapPWM(WORD u16Lum);

//Dynamic backlight and EOTF control
#define HDR_LUMINANCE_TO_BRIGHTNESS_0           90
#define HDR_LUMINANCE_TO_BRIGHTNESS_100         600
#define HDR_MIN_DUTY                            0x13
#define HDR_MAX_DUTY                            0xFF
#define HDR_10STEP_IS_VALID                     0
#endif
//#endif

#if (CHIP_ID == CHIP_MT9701)
void mdrv_HDR_DS_Start(void);
void mdrv_HDR_DS_End(void);
void mdrv_HDR_DS_W2BMask(DWORD u32Address, WORD u16Data, WORD u16Mask);
void mdrv_HDR_DS_WBMask(DWORD u32Address, BYTE u8Data, BYTE u8Mask);
void mdrv_HDR_DS_W4B(DWORD u32Address, DWORD u32Value);
void mdrv_HDR_DS_W2B(DWORD u32Address, WORD u16Data);
void mdrv_HDR_DS_WB(DWORD u32Address, BYTE u8Data);
void mdrv_HDR_DS_WBit(DWORD u32Address, Bool bBit, BYTE u8BitPos);
void mapi_HDR_FreeSync2Enable_Set(BOOL bSupport);
BOOL mapi_HDR_FreeSync2Enable_Get(void);

#if (ENABLE_DOLBY_HDR)
void mdrv_Dolby_Addr_Init(void);
void mdrv_Dolby_HKR2_Receive(void);
#endif


#endif

