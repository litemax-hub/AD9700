#include <string.h>
#include "ms_config.h"
#include "Globalvar.h"
#include "ms_otg.h"
#include "ms_drc.h"
#include "ms_usb.h"
#include "ms_hid.h"

extern void printMsg( char *str );
extern void printData( char *str, WORD value );

#define HID_DEBUG    1
#if (ENABLE_MSTV_UART_DEBUG && HID_DEBUG)
#define DBG_PRINTDATA(str, value)   printData(str, value)
#define DBG_PRINTMSG(str)           printMsg(str)
#else
#define DBG_PRINTDATA(str, value)
#define DBG_PRINTMSG(str)
#endif

typedef struct sHIDControl
{
	U8 bState;
	U8 *EP0TxBuf;
	U16 EP0TxLen;
	U8 *EP0RxBuf;
	U8 *EPxTxBuf;
	U16 EPxTxLen;
	U8 *EPxRxBuf;
	U16 EPxRxLen;
} HIDControl;


U8 Ep0Buff[HID_TRX_LENGTH] __attribute__((aligned(4)));
U8 EpxBuff[HID_TRX_LENGTH] __attribute__((aligned(4)));
HIDControl mHID;


void HID_Init(void)
{
	mHID.bState = 0;

	/* EP0 Rx buff */
	memset(Ep0Buff, 0, HID_TRX_LENGTH);
	mHID.EP0RxBuf = Ep0Buff;

	/* EP0 Tx buff */
	mHID.EP0TxBuf = NULL;
	mHID.EP0TxLen = 0;

	/* EP1 Tx buff */
	mHID.EPxTxBuf = NULL;
	mHID.EPxTxLen = 0;

	/* EP2 Rx buff */
	memset(EpxBuff, 0, HID_TRX_LENGTH);
	mHID.EPxRxBuf = EpxBuff;
	mHID.EPxRxLen = HID_TRX_LENGTH;
}

U8 *HID_GetBuff_ptr(void)
{
	return(Ep0Buff);
}

/*
 * HID received Host Control Write Request
 */
U8 IsHID_EP0_Rx_Request(USB_VAR *gUSBStruct)
{
	U8 State = 0;
	U16 wLength;

	if ((gUSBStruct->otgEP0Setup.bmRequestType & USB_TYPE_MASK) == USB_TYPE_CLASS) {
		/* handle HID Classs request */
		switch (gUSBStruct->otgEP0Setup.bRequest)
		{
			case HID_SET_REPORT:
				wLength = gUSBStruct->otgEP0Setup.wLength;
				HID_Set_Report_RxData((U8 *)gUSBStruct->otgUSB_EP[0].transfer_buffer, gUSBStruct);
				break;
			default:
				DBG_PRINTMSG("HID class un-supported class request\n");
				State = -ENOSUP;
				goto ERR_RETURN;
		}
	} else
	if ((gUSBStruct->otgEP0Setup.bmRequestType & USB_TYPE_MASK) == USB_TYPE_VENDOR) {
		/* handle HID Vendor request */
		switch (gUSBStruct->otgEP0Setup.bRequest)
		{
			case VENDOR_SET_DATA:
				HID_Vendor_Set_RxData((U8 *)gUSBStruct->otgUSB_EP[0].transfer_buffer, gUSBStruct);
				break;
			default:
				DBG_PRINTMSG("HID un-supported Vendor Request\n");
				State = -ENOSUP;
				goto ERR_RETURN;
		}
	}

	return State;

ERR_RETURN:
	return State;
}





/*
 * HID EP1 Tx buffer data to host
 */
U8 HIDTxBuff(USB_VAR *gUSBStruct)
{
	if (mHID.bState & HID_EP1_TX_READY) {
//		DBG_PRINTDATA("EP1 Tx %d data\n", mHID.EPxTxLen);
		SDRAM2USB_Bulk((U32)mHID.EPxTxBuf, (U32)mHID.EPxTxLen, gUSBStruct);
		mHID.bState &= ~ HID_EP1_TX_READY;
		return 0;
	}

	return -ENRDY;
}

/*
 * HID EP2 Rx buffer data from host
 */
U8 HIDRxBuff(USB_VAR *gUSBStruct)
{
    gUSBStruct = gUSBStruct;
	if (mHID.bState & HID_EP2_RX_READY)
		return (-EBUSY);

//	DBG_PRINTDATA("HID EP2 Rx Data %d\n", mHID.EPxRxLen);

	//USB2SDRAM_Bulk((U32)mHID.EPxRxBuf, (U32)mHID.EPxRxLen, gUSBStruct);
	mHID.bState |= HID_EP2_RX_READY;

	return(0);
}

U8 HID_Set_Report_RxData(U8 *pBuff, USB_VAR *gUSBStruct)
{
//	DBG_PRINTDATA("Set Report %x\n", gUSBStruct->otgEP0Setup.bRequest);
    gUSBStruct = gUSBStruct;

	if ((mHID.bState & HID_EP0_RX_READY))
		return -EBUSY;

	mHID.bState |= HID_EP0_RX_READY;
	mHID.EP0RxBuf = pBuff;
	return 0;
}

U8 HID_Vendor_Set_RxData(U8 *pBuff, USB_VAR *gUSBStruct)
{
	DBG_PRINTDATA("HID Vendor EP0 write %X data\n", gUSBStruct->otgEP0Setup.wLength);

	if ((mHID.bState & HID_EP0_RX_READY))
		return -EBUSY;

	mHID.bState |= HID_EP0_RX_READY;
	mHID.EP0RxBuf = pBuff;
	return 0;
}


U8 *HID_Vendor_Get_TxData(U16 *TxLength, USB_VAR *gUSBStruct)
{
	DBG_PRINTDATA("HID Vendor EP0 read %X data\n", gUSBStruct->otgEP0Setup.wLength);

	if ((mHID.bState& HID_EP0_TX_READY) == 0) {
		*TxLength = 0;
		return(0);
	}

	*TxLength = MIN(gUSBStruct->otgEP0Setup.wLength, HID_TRX_LENGTH);
	return(mHID.EP0TxBuf);
}


void HID_Set_EPxRxLen(U16 wLen)
{
	mHID.EPxRxLen = wLen;
}

U8 *HID_Get_EPxRxBuff(void)
{
	return(mHID.EPxRxBuf);
}

U16 HID_Get_EPxRxLen(void)
{
	return(mHID.EPxRxLen);
}

void HID_EPxTRX_GO(void)
{
	mHID.bState |= HID_EPX_ON;
}

U8 *HID_Get_EP0RxBuff(void)
{
	return(mHID.EP0RxBuf);
}

/*
 * if EP0 received vendor control write data:
 *    VENDOR_SET_DATA, HID_SET_REPORT
 *    EP0 Setup request is stored in gUSBStruct->otgEP0Setup
 * return data payload pointer
 */
U8 *HID_Rx_CtrlReq(void)
{
	if ((mHID.bState & HID_EP0_RX_READY) == 0)
		return(NULL);

	mHID.bState &= ~HID_EP0_RX_READY;
	return(mHID.EP0RxBuf);
}


/*
 * if EP0 wants to send vendor control read data:
 *    VENDOR_GET_DATA
 *    pBuff:  tx data pointer
 *    wLength: tx data length
 *    return 0-ok.
 *           failed code.
 */
U8 HID_Tx_CtrlReq(U8 *pBuff, U16 wLength)
{

	if ((mHID.bState & HID_EP0_TX_READY) == HID_EP0_TX_READY)
		return(-EBUSY);

	mHID.bState |= HID_EP0_TX_READY;
	mHID.EP0TxBuf = pBuff;
	mHID.EP0TxLen = wLength;
	return(0);
}

/*
 * EP2 received interrupt out data:
 * return data payload pointer
 */
U8 *HID_Rx_Data(void)
{
	if ((mHID.bState & HID_EP2_RX_READY) == 0)
		return(NULL);

	mHID.bState &= ~HID_EP2_RX_READY;
	return(mHID.EPxRxBuf);
}


/*
 * EP1 transfer interrupt IN data:
 *    pBuff: EP1 tx data pointer
 *    wLength is tx data length
 *    return
 *        0-ok.
 *        failed code.
 */
U8 HID_Tx_Data(U8 *pBuff, U16 wLength, USB_VAR *gUSBStruct)
{

	if ((mHID.bState & HID_EPX_ON) == 0)
		return(-ENRDY);

	if (mHID.bState & HID_EP1_TX_READY)
		return(-EBUSY);

	mHID.bState |= HID_EP1_TX_READY;
	mHID.EPxTxBuf = pBuff;
	mHID.EPxTxLen = wLength;
	HIDTxBuff(gUSBStruct);
	return(0);
}



/******************************************
 *
 * HID main function loop
 *
 ******************************************/

#define FW_UPDATE_GLUE

#ifdef FW_UPDATE_GLUE
//int dwIdleCount = 0;
#if 0
U16 *FWDL_EP0Tx_Data_Phase(U16 *wLength, struct devrequest *pEP0Setup)
{
		/* Parsing Setup */

		/* set Length */

/* return buffer pointer */
}
#endif

/*
 * FW update function can glue from here
 */
void HIDFNloop(USB_VAR *gUSBStruct, U8 *USB_Command)
{
	struct devrequest volatile *sPtr;
	U8 *ptr = 0;

	/* EP0 Rx Test */
	if (HID_Rx_CtrlReq())
    {
        sPtr = &gUSBStruct->otgEP0Setup;
        memcpy(USB_Command, HID_Get_EP0RxBuff(), HID_TRX_LENGTH);
	}

#if 0
	if ((++dwIdleCount & 0x007FFFFF) == 0x007FFFFF)
    {
		DBG_PRINTDATA("HIDFnLoop %X\n", dwIdleCount);

		/* EP1 Tx Test */
		 if (HID_Tx_Data( Ep0Buff, HID_TRX_LENGTH, gUSBStruct ) == 0)
        {
			 DBG_PRINTMSG(" Tx 512 data\n");
		}
	}
#endif

	/* EP2 Rx Test */
	if (mHID.bState & HID_EP2_RX_READY)
    {
		 DBG_PRINTDATA(" Rx data %d bytes\n", mHID.EPxRxLen);
		 ptr = HID_Rx_Data();
	}
}
#else
int dwIdleCount = 0;


/*
 * Demo code of HID class polling loop
 */
void HIDFNloop(USB_VAR *gUSBStruct)
{
	struct devrequest volatile *sPtr;
	U8 *ptr;

	/* EP0 Rx Test */
	if (HID_Rx_CtrlReq()) {
		 DBG_PRINTMSG("EP0 Request\n");
		 sPtr = &gUSBStruct->otgEP0Setup;
	}

	if ((++dwIdleCount & 0x007FFFFF) == 0x007FFFFF) {
		DBG_PRINTDATA("HIDFnLoop %X\n", dwIdleCount);

		/* EP1 Tx Test */
		 if (HID_Tx_Data( Ep0Buff, HID_TRX_LENGTH, gUSBStruct ) == 0) {
			 DBG_PRINTMSG(" Tx 512 data\n");
		}
	}

	/* EP2 Rx Test */
	if (mHID.bState & HID_EP2_RX_READY) {
		 DBG_PRINTDATA(" Rx data %d bytes\n", mHID.EPxRxLen);
		 ptr = HID_Rx_Data();
	}

}
#endif

