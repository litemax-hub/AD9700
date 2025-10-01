#ifndef _DRV_USBDWLD_H
#define _DRV_USBDWLD_H
#include "board.h"

#define XROM_USB_UPLOAD_FIXEDLEN    0x00
#define XROM_USB_DOWNLOAD_KEEP      0x01
#define XROM_USB_UPLOAD_SIZE        0x02
#define XROM_USB_ERR_CODE           0x03
#define XROM_USB_DOWNLOAD_END       0x04
#define XROM_USB_SEL_ROMRAM         0x05
#define XROM_USB_UPLOAD_VARLEN      0x06
#define XROM_USB_INVALID_FILE       0x07
#define XROM_USB_FILE_SIZE          0x08
#define XROM_USB_REBOOT_SYS         0x09
#define XROM_USB_FILE_CHECKSUM      0x0A
#define XROM_USB_UPLOAD_LOG	        0x16
#define XROM_USB_GET_DP_SYNC_CNT    0x20
#define XROM_USB_GET_UPDATE_STATUS  0x21
#define XROM_USB_SET_UPDATE_BITMAP  0x22
#define XROM_USB_SET_DELAY_TIME     0x23
#define XROM_USB_GET_PROGRAMMED_SIZE    0x27
#define XROM_USB_WRITE_SC_REG       0xB2
#define XROM_USB_READ_SC_REG        0xB3
#define XROM_USB_WRITE_NM_REG       0xB4
#define XROM_USB_READ_NM_REG        0xB5
#define XROM_USB_HID_DATA           0xC0


typedef enum _USB_MST_UPDATE_STATUS
{
    MST_UPDATE_STATUS_IDLE          = 0x0,
    MST_UPDATE_STATUS_FAIL          = 0x1,
    MST_UPDATE_STATUS_SUCCESS       = 0x2,
    MST_UPDATE_STATUS_INPROGRESS    = 0x3
}USB_MST_UPDATE_STATUS;

void MDrv_USB_Init_Device(void);
void UDC_upll_enable(void);
void usb_connect(void);
void USB_Device_Init(void);
void USB_Handler(void);
void USB_CMDHandler(void);

#if ENABLE_USB_DEVICE_HID_MODE
void USB_DataHandler(BYTE* USB_Data);
#else
void USB_DataHandler(void);
#endif

extern void msWrite4Byte( unsigned long u32RegLoAddr, unsigned long u32Value );
extern void APPSystem_USBDeviceCB(BYTE* DataAddress,DWORD DataSize, BYTE CheckSum_flag);
void USB_SendUpdateScuessCmd(BYTE u8Scuess);
void USB_Device_Disable(void);
void USB_Device_Enable(void);

U16 MST_FW_Update_Bitmap, MST_FW_Update_DealyTime;
U8 MST_UpdateStatus;

#if ENABLE_USB_DEVICE_UPDATE_CODE
U32 SPIWCnt;
#endif

#endif  // _DRV_USBDWLD_H
