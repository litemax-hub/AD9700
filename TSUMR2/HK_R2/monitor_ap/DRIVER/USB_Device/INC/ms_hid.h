#ifndef _USB_HID_H_
#define _USB_HID_H_

#include "Globalvar.h"

/* #define FW_UDPATE_GLUE */

#define HID_VID				0x0E8D
#define HID_PID				0x0300

#define ENOSUP				1
#define EBUSY				2
#define ENODATA				3
#define ENRDY				4

#define HID_GET_REPORT		0x01
#define HID_SET_REPORT		0x09
#define HID_SET_IDLE		0x0A

/* MTK USB Control Pipe vendor command */
#define VENDOR_GET_DATA		0x01
#define VENDOR_SET_DATA		0x09

#define ARRAY_SIZE(arr)  (sizeof(arr) / sizeof(arr[0]))

#define HID_TRX_LENGTH 		512  //4096


#define HID_EP0_RX_READY	0x01
#define HID_EP0_TX_READY	0x02
#define HID_EP1_TX_READY	0x04
#define HID_EP2_RX_READY	0x08
#define HID_EP2_RX_ON		0x10
#define HID_EPX_ON			0x80

#define LO_BYTE(x)	(x & 0xFF)
#define HI_BYTE(x)	((x >> 8) & 0xFF)

void HID_Init(void);

U8 *HID_GetBuff_ptr(void);

U8 IsHID_EP0_Rx_Request(USB_VAR *);

U8 HIDTxBuff(USB_VAR *);
U8 HIDRxBuff(USB_VAR *);

U8 HID_Set_Report_RxData(U8 *, USB_VAR *);
U8 HID_Vendor_Set_RxData(U8 *, USB_VAR *);
U8 *HID_Vendor_Get_TxData(U16 *, USB_VAR *);

void HID_Set_EPxRxLen(U16);
U8 *HID_Get_EPxRxBuff(void);
U8 *HID_Get_EP0RxBuff(void);
U16 HID_Get_EPxRxLen(void);
void HID_EPxTRX_GO(void);

U8 *HID_Rx_CtrlReq(void);
U8  HID_Tx_CtrlReq(U8 *, U16);
U8 *HID_Rx_Data(void);
U8  HID_Tx_Data(U8 *, U16, USB_VAR *);

//void HIDFNloop(USB_VAR *);
void HIDFNloop(USB_VAR *gUSBStruct, U8 *USB_Command);


#endif
