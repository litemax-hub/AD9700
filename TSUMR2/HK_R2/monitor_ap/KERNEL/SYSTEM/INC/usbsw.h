#ifndef _USBSW_H_
#define _USBSW_H_

#ifdef _USBSW_C_
#define _USBSWDEC_
#else
#define _USBSWDEC_    extern
#endif

#define U3_REDRIVER_LEGACY 0 // need to syc with PD51
#define U3_REDRV_PM_MODE   (1 && ENABLE_USB_TYPEC) // need to syc with PD51, 1: enable retimer U3 state

#if (CHIP_ID == CHIP_MT9701)
#if (U3_REDRV_PM_MODE)
_USBSWDEC_ XDATA BYTE bEnterU3State; // used for normal mode
_USBSWDEC_ XDATA BYTE bIsInU3State; // used for normal mode
_USBSWDEC_ XDATA BOOL bU3WakeupMaskEnable;
_USBSWDEC_ BOOL xdata g_bRedrU3Status, g_bHubU3Status; // 1: in U3 state, 0: others, used for PM mode
#endif
#if ENABLE_USB_TYPEC
_USBSWDEC_ BYTE g_u8Usb2Alive;
#endif
#endif

#define IsInU3State()     (msReadByte(REG_MBX_PD51_TO_HKR2_U3_STATE))

#define UTMIBaseAddress  0x150300	//0x1f200000+(0x3a80*2)
//#define otgRegAddress    0x151000  	//0x1f200000+(0x11700*2)
#define USBCBaseAddress  0x150200	//0x1f200000+(0x0700*2)

#define UTMI_REG_WRITE8(r,v)   msWriteByte(UTMIBaseAddress+r, v)//(DRC_OUT8(UTMIBaseAddress + (r),(v)))
#define UTMI_REG_WRITE16(r,v)  msWrite2Byte(UTMIBaseAddress+r, v)//(DRC_OUT16(UTMIBaseAddress + (r),(v)))
#define UTMI_REG_READ8(r)      msReadByte(UTMIBaseAddress+r)//(DRC_IN8(UTMIBaseAddress + (r)))
#define UTMI_REG_READ16(r)     msRead2Byte(UTMIBaseAddress+r)//(DRC_IN16(UTMIBaseAddress + (r)))
#define USBC_REG_WRITE8(r,v)   msWriteByte(USBCBaseAddress+r, v)//(DRC_OUT8(USBCBaseAddress + (r),(v)))
#define USBC_REG_WRITE16(r,v)  msWrite2Byte(USBCBaseAddress+r, v)//(DRC_OUT16(USBCBaseAddress + (r),(v)))
#define USBC_REG_READ8(r)      msReadByte(USBCBaseAddress+r)//(DRC_IN8(USBCBaseAddress + (r)))
#define USBC_REG_READ16(r)     msRead2Byte(USBCBaseAddress+r)//(DRC_IN16(USBCBaseAddress + (r)))

_USBSWDEC_ void msDrvUsbSwitchInit(void);
_USBSWDEC_ void msDrvUsbSwitchRtermEnDP(BOOL bEnableDP);
_USBSWDEC_ void msDrvUsbSwitchRtermEnTypeC(BOOL bEnableTypeC);
#if ENABLE_USB_TYPEC
_USBSWDEC_ void msDPCPadSwitchCfg(BYTE u8CC, BYTE u8PinAssign);
_USBSWDEC_ void msDPCRtermCfg(BYTE u8CC, BYTE u8PinAssign);
#endif
#if ENABLE_INTERNAL_CC
_USBSWDEC_ void msDrvInternalCcTrimInitNonPM(void);
_USBSWDEC_ void msDrvInternalCcTrimInitPM(void);
#endif
_USBSWDEC_ void msDrvDPCRtermTrimInitNonPM(void);
_USBSWDEC_ void msDPCRtermModeInit(void);
_USBSWDEC_ void msDrvUsb2Init(void);
_USBSWDEC_ void msDrvDPCInit(void);

#if (CHIP_ID == CHIP_MT9701 && U3_REDRV_PM_MODE)
_USBSWDEC_ void msSetUSBEnterU3State(BYTE u8CCPin);
_USBSWDEC_ void msSetUSBExitU3State(void);
_USBSWDEC_ void msUSB3Handler(void);
#endif

_USBSWDEC_ void USBSWDummy(void);
#endif

