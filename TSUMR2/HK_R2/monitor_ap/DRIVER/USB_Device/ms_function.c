#include "ms_config.h"
#include "type.h"
#include "Globalvar.h"
#include "ms_drc.h"
#include "ms_function.h"
#include "ms_cpu.h"
#include "ms_otg.h"
#include "ms_usb.h"
#include "board.h"
#include "ms_hid.h"

extern void printMsg( char *str );
extern void printData( char *str, unsigned int value );

#define MSFUNCTION_DEBUG    0
#if ENABLE_DEBUG&&MSFUNCTION_DEBUG
#define MSFUNCTION_printData(str, value)   printData(str, value)
#define MSFUNCTION_printMsg(str)           printMsg(str)
#else
#define MSFUNCTION_printData(str, value)
#define MSFUNCTION_printMsg(str)
#endif

#if ENABLE_USB_DEVICE_HID_MODE
/*
 * HID Report Descriptor
 */
CODE const U8 otg_Msdfn_HID_Report_Dscr1[]= {
	/* HID Report Descriptor */
	0x06, 0x00, 0xFF,			/* Usage Page (Vendor Defined page)*/
	0x09, 0x01,					/* Usage (Vendor Usage 1)		*/
	0xA1, 0x01,					/* Collection (Application) 	*/
	0x15, 0x00,					/* Logical Minimum (0) 			*/
	0x26, 0xFF, 0x00,			/* Logical Maximum (255) 		*/
	0x75, 0x08,					/* Report size (8)		 		*/
	0x96, 0x00, HID_TRX_LENGTH/256,			/* Report Count(0x02:512 or 0x10:4096)	 		*/
	0x09, 0x01,					/* Usage (Vendor usage 1)		*/
	0x81, 0x02,					/* Input (Data,var,abs) 		*/
	0x09, 0x01, 				/* Usage (Vendor usage 1)		*/
	0x91, 0x02, 				/* Input (Data,var,abs) 		*/
	0xC0						/* End Colection			 	*/
};

CODE const U8 otg_Msdfn_HID_Report_Dscr2[]= {
	/* HID Report Descriptor */
	0x06, 0x00, 0xFF,			/* Usage Page (Vendor Defined page)*/
	0x09, 0x01,					/* Usage (Vendor Usage 1)		*/
	0xA1, 0x01,					/* Collection (Application) 	*/
	0x15, 0x00,					/* Logical Minimum (0) 			*/
	0x26, 0xFF, 0x00,			/* Logical Maximum (255) 		*/
	0x75, 0x08,					/* Report size (8)		 		*/
	0x96, 0x00, 0x02,			/* Report count(512)		 	*/
	0x09, 0x01,					/* Usage (Vendor usage 1)		*/
	0x91, 0x02,					/* Output (Data,var,abs) 		*/
	0xC0						/* End Colection			 	*/
};


/*
 * HID mode for USB device mode
 */
CODE const U8 otg_Msdfn_Dscr[]=
{
	/* Device Descriptor */
	0x12,						/* bLength 		*/
	0x01,						/* DEVICE		*/
	0x00,0x02,					/* USB 2.0		*/
	0x00,						/* CLASS		*/
	0x00,						/* Subclass		*/
	0x00,						/* Protocol		*/
	0x40,						/* bMaxPktSize0	*/
	LO_BYTE(HID_VID), HI_BYTE(HID_VID),		/* idVendor		*/
	LO_BYTE(HID_PID), HI_BYTE(HID_PID),		/* idProduct	*/
	0x00,0x01,					/* bcdDevice	*/
	0x01,						/* iManufacturer	*/
	0x02,						/* iProduct			*/
	0x03,  						/* iSerial Number	*/
	0x01,						/* One configuration*/
	/* Configuration Descriptor */
	0x09,						/* bLength			*/
	0x02,						/* CONFIGURATION	*/
	9+9+9+7,						/* length_L			*/
	0x00,						/* length_H			*/
	0x01,						/* bNumInterfaces	*/
	0x01,						/* bConfigurationValue*/
	0x00,						/* iConfiguration	*/
	0xC0,						/* bmAttributes		*/
	0x00,						/* power 500ma		*/
	/* Interface Descriptor */
	0x09,						/* bLength			*/
	0x04,						/* INTERFACE		*/
	0x00,						/* bInterfaceNumber */
	0x00,						/* bAlternateSetting*/
	0x01,						/* bNumEndpoints	*/
	0x03,						/* bInterfaceClass HID=3*/
	0x00,						/* bInterfaceSubClass*/
	0x00,						/* bInterfaceProtocol NONE=0*/
	0x00,						/* iInterface		*/
	/* HID Descriptor */
	0x09,						/* bLength			*/
	0x21,						/* HID				*/
	0x11,0x01,					/* bcdHID			*/
	0x00,						/* bCountryCode 	*/
	0x01,						/* bNumDescriptor	*/
	0x22,						/* bDescriptorType	*/
	(ARRAY_SIZE(otg_Msdfn_HID_Report_Dscr1) & 0xFF), (ARRAY_SIZE(otg_Msdfn_HID_Report_Dscr1) >> 8), 	/* sizeof ReportDesc*/
	/* Endpoint Descriptor	: Bulk-In */
	0x07,						/* bLength			*/
	0x05,						/* ENDPOINT 		*/
	0x81,						/* bEndpointAddress */
	0x03,						/* bmAttributes 	*/
	0x00, 0x02, 				/* wMaxPacketSize	*/
	0x01,						/* bInterval 8ms 2^(7-1)	*/
/* string descritor*/
/*langID*/
   0x04, 0x03, 0x09, 0x04,
/*iManufacturer*/
   0x12, 0x03, 'M', 0x00, 'e', 0x00, 'd', 0x00, 'i', 0x00, 'a', 0x00, 't', 0x00,'e', 0x00, 'k', 0x00,
/*DevID*/
   0x16, 0x03, 'H', 0x00, 'I', 0x00, 'D', 0x00, 0x20, 0x00, 'D', 0x00, 'e', 0x00,'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00,
/*iserID*/
   0x12, 0x03, 'S', 0x00, 'N', 0x00, '0', 0x00, '0', 0x00, '0', 0x00, '0', 0x00, '0', 0x00, '1', 0x00
};
#else
/*
 * default Mass Storage device class
 */
CODE const U8 otg_Msdfn_Dscr[]=
{
   /* Device Descriptor */
   0x12,									/* bLength				*/
   0x01,									/* DEVICE				*/
   0x00,0x02,								/* USB 2.0				*/
   0x00,									/* CLASS				*/
   0x00,									/* Subclass 			*/
   0x00,									/* Protocol 			*/
   0x40,									/* bMaxPktSize0 		*/
   0x20,0x1B,								/* idVendor 			*/
   0x00,0x03,								/* idProduct			*/
   0x00,0x01,								/* bcdDevice			*/
   0x00,									/* iManufacturer		*/
   0x00,									/* iProduct 			*/
   0x00,									/* iSerial Number		*/
   0x01,									/* One configuration	*/
   /* Device Qualifier */
   0x0a,					  /* bLength			  */
   0x06,					  /* DEVICE Qualifier	  */
   0x00,0x02,				  /* USB 2.0			  */
   0x00,					  /* CLASS				  */
   0x00,					  /* Subclass			  */
   0x00,					  /* Protocol			  */
   0x40,					  /* bMaxPktSize0		  */
   0x01,					  /* One configuration	  */
   0x00,					  /* Reserved			  */
#if 0
   /* Other Speed Configuration Descriptor */
   0x09,					  /* bLength				   */
   0x07,					  /* Other Speed Configuration */
   0x20,					  /* length 				   */
   0x0, 					  /* length 				   */
   0x01,					  /* bNumInterfaces 		   */
   0x01,					  /* bConfigurationValue	   */
   0x00,					  /* iConfiguration 		   */
   0xC0,					  /* bmAttributes			   */
   0,						  /* power					   */
#endif
   /* Configuration Descriptor */
   0x09,					  /* bLength			  */
   0x02,					  /* CONFIGURATION		  */
   0x20,					  /* length_L			  */
   0x00,					  /* length_H			  */
   0x01,					  /* bNumInterfaces 	  */
   0x01,					  /* bConfigurationValue  */
   0x00,					  /* iConfiguration 	  */
   0xC0,					  /* bmAttributes		  */
   0x00,					  /* power 500ma		  */
   /* Interface Descriptor */
   0x09,								   /* bLength			   */
   0x04,								   /* INTERFACE 		   */
   0x0, 								   /* bInterfaceNumber	   */
   0x0, 								   /* bAlternateSetting    */
   0x02,								   /* bNumEndpoints 	   */
   0x08,								   /* bInterfaceClass	   */
   0x06,								   /* bInterfaceSubClass   */
   0x50,								   /* bInterfaceProtocol   */
   0x00,								   /* iInterface		   */
   /* Endpoint Descriptor  : Bulk-In */
   0x07,								   /* bLength			   */
   0x05,								   /* ENDPOINT			   */
   0x81,								   /* bEndpointAddress	   */
   0x02,								   /* bmAttributes		   */
   0x00, 0x02,							   /* wMaxPacketSize	   */
   0x00,								   /* bInterval 		   */
   /* Endpoint Descriptor  : Bulk-Out */
   0x07,								   /* bLength			   */
   0x05,								   /* ENDPOINT			   */
   0x02,								   /* bEndpointAddress	   */
   0x02,								   /* bmAttributes		   */
   0x00, 0x02,							  /* wMaxPacketSize 	  */
   0x00,								   /* bInterval 		   */
#if 0
   //USB_DT_OTG_SIZE,
   //USB_DT_OTG,
   /* Configuration Descriptor */
   0x09,								   /* bLength			   */
   0x02,								   /* CONFIGURATION 	   */
   0x20,								   /* length			   */
   0x0, 								   /* length			   */
   0x01,								   /* bNumInterfaces	   */
   0x01,								   /* bConfigurationValue  */
   0x00,								   /* iConfiguration	   */
   0xC0,								   /* bmAttributes		   */
   0x0, 								   /* power 			   */
   /* Interface Descriptor */
   0x09,								   /* bLength			   */
   0x04,								   /* INTERFACE 		   */
   0x0, 								   /* bInterfaceNumber	   */
   0x0, 								   /* bAlternateSetting    */
   0x02,								   /* bNumEndpoints 	   */
   0x08,								   /* bInterfaceClass	   */
   0x06,								   /* bInterfaceSubClass   */
   0x50,								   /* bInterfaceProtocol   */
   0x00,								   /* iInterface		   */
   /* Endpoint Descriptor  : Bulk-In */
   0x07,								   /* bLength			   */
   0x05,								   /* ENDPOINT			   */
   0x81,								   /* bEndpointAddress	   */
   0x02,								   /* bmAttributes		   */
   0x40, 0x00,							   /* wMaxPacketSize	   */
   0x00,								   /* bInterval 		   */
   /* Endpoint Descriptor  : Bulk-Out */
   0x07,								   /* bLength			   */
   0x05,								   /* ENDPOINT			   */
   0x02,								   /* bEndpointAddress	   */
   0x02,								   /* bmAttributes		   */
   0x40, 0x00,							   /* wMaxPacketSize	   */
   0x00,
   //USB_DT_OTG_SIZE,
   //USB_DT_OTG,
#endif
 /* string descritor*/
 /*langID*/
   0x04, 0x03, 0x09, 0x04,
/*DevID*/
  0x28, 0x03, 'M', 0x00, 'a', 0x00, 's', 0x00, 's', 0x00, 0x20, 0x00, 'S', 0x00, 't', 0x00,
  'o', 0x00, 'r', 0x00, 'a', 0x00, 'g', 0x00, 'e', 0x00, 0x20, 0x00, 'D', 0x00, 'e', 0x00,
  'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00,
 /*iserID*/
  0x12, 0x03, 'B', 0x00, 'i', 0x00, 'g', 0x00,
  'B', 0x00, 'l', 0x00, 'u', 0x00, 'e', 0x00,
  '3', 0x00
};
#endif

void USB_Function_Initial(USB_VAR *gUSBStruct)
{
    if (gUSBStruct->otgSpeed==HIGH_SPEED)
    {
        USB_Setup_Endpoint(0,FIFO_DUAL,PIPE_CONTROL,0,0x40,gUSBStruct);
        USB_Setup_Endpoint(1,EP_TX,PIPE_BULK,0,0x200,gUSBStruct);
        USB_Setup_Endpoint(2,EP_RX,PIPE_BULK,0,0x200,gUSBStruct);
        //USB_Setup_Endpoint(3,EP_TX,PIPE_INTERRUPT,0,0x200,gUSBStruct);
    }
	#if 0
    else if (gUSBStruct->otgSpeed==FULL_SPEED)
    {
        USB_Setup_Endpoint(0,FIFO_DUAL,PIPE_CONTROL,0,0x40,gUSBStruct);
        USB_Setup_Endpoint(1,EP_TX,PIPE_BULK,0,0x40,gUSBStruct);
        USB_Setup_Endpoint(2,EP_RX,PIPE_BULK,0,0x40,gUSBStruct);
    }
	#endif
}

U8 USB_Function_Load_Fifo(U8 ep,USB_VAR *gUSBStruct)
{
    U8    outreg;
    outreg = M_CSR0_TXPKTRDY;
    USB_DRC_Fifo_Write((U8 *)gUSBStruct->otgUSB_EP[ep].transfer_buffer,ep,gUSBStruct);
    if  (gUSBStruct->otgUSB_EP[ep].LastPacket)
    {
	outreg |= M_CSR0_P_DATAEND;
	gUSBStruct->otgUSB_EP[ep].IOState = EP_IDLE;
    }
    return outreg;
}

S32 USB_Parse_Received_Setup(USB_VAR *gUSBStruct)
{
    S32     retstatus;
    S32     bytesrem;
    S32     length;
    U8    *scanP;
    U8    i, descindex;
    U8    desctype;
    U8  /*S32*/    endpoint /*, direction */;
    pipe_t  pipe;
    void *sp;

	retstatus = USB_PID_STALL;
	if  ((gUSBStruct->otgEP0Setup.bmRequestType & USB_TYPE_MASK) != USB_TYPE_STANDARD)
	{
		MSFUNCTION_printMsg("USB no standard");
#if	ENABLE_USB_DEVICE_HID_MODE
		if ((gUSBStruct->otgEP0Setup.bmRequestType & USB_TYPE_MASK) == USB_TYPE_VENDOR)
		{
			switch (gUSBStruct->otgEP0Setup.bRequest)
			{
				case VENDOR_GET_DATA:
					if (gUSBStruct->otgEP0Setup.wLength) {
						gUSBStruct->otgUSB_EP[0].transfer_buffer = (U32)HID_Vendor_Get_TxData((U16 *)&length, gUSBStruct);;
						gUSBStruct->otgUSB_EP[0].transfer_buffer_length = length;
						retstatus =	USB_PID_IN;
					} else
						retstatus = USB_PID_ACK;
					break;
				case VENDOR_SET_DATA:
					if (gUSBStruct->otgEP0Setup.wLength) {
						length = MIN(HID_Get_EPxRxLen(), HID_TRX_LENGTH);
						gUSBStruct->otgUSB_EP[0].transfer_buffer_length=MIN(gUSBStruct->otgEP0Setup.wLength, length);
						gUSBStruct->otgUSB_EP[0].transfer_buffer=(U32)HID_GetBuff_ptr();
						retstatus =	USB_PID_OUT;
					} else
						retstatus = USB_PID_ACK;
					break;
			}
		} else
#endif
		if ((gUSBStruct->otgEP0Setup.bmRequestType & USB_TYPE_MASK) == USB_TYPE_CLASS)
		{
			switch (gUSBStruct->otgEP0Setup.bRequest)
			{
#if	ENABLE_USB_DEVICE_HID_MODE
				case HID_SET_IDLE:
					/* printf("HID Set Idle request\n"); */
					HID_EPxTRX_GO();
					retstatus = USB_PID_ACK;
					break;
				case HID_SET_REPORT:
					/* printf("HID Set Report request wValue=%x\n", gUSBStruct->otgEP0Setup.wValue); */
#ifdef FW_UDATE_FUNC
					length = 0;
					gUSBStruct->otgUSB_EP[0].transfer_buffer= FWDL_EP0Tx_Data_Phase((U16 *)&length, &gUSBStruct->otgEP0Setup);
					gUSBStruct->otgUSB_EP[0].transfer_buffer_length=length;
#else
                    gUSBStruct->otgUSB_EP[0].transfer_buffer_length=HID_TRX_LENGTH;
                    gUSBStruct->otgUSB_EP[0].transfer_buffer=(U32)HID_GetBuff_ptr();
#endif
					retstatus = USB_PID_OUT;
					break;
				case HID_GET_REPORT:
					if (gUSBStruct->otgEP0Setup.wLength) {
						gUSBStruct->otgUSB_EP[0].transfer_buffer = (U32)HID_Vendor_Get_TxData((U16 *)&length, gUSBStruct);;
						gUSBStruct->otgUSB_EP[0].transfer_buffer_length = length;
						retstatus = USB_PID_IN;
					} else
						retstatus = USB_PID_ACK;
					break;
#endif
				case    MSDFN_BOT_RESET:
					retstatus = USB_PID_ACK;
					break;
				 case    MSDFN_BOT_GET_MAXLUN:
                    gUSBStruct->otgUSB_EP[0].transfer_buffer_length=1;
                    gUSBStruct->otgUSB_EP[0].transfer_buffer=(U32)&gUSBStruct->otgPerMassmax_Lun;
                    if ((gUSBStruct->otgEP0Setup.bmRequestType & USB_RECIP_MASK) == USB_RECIP_INTERFACE)
                        retstatus = USB_PID_IN;
                    break;
			}
		}
		return(retstatus);
	}
	switch (gUSBStruct->otgEP0Setup.bRequest)
	{
		MSFUNCTION_printMsg("bRequest");
		case    USB_REQ_GET_STATUS:
			MSFUNCTION_printMsg("USB_REQ_GET_STATUS");
			gUSBStruct->otgGetStatusResponse = 0;
			gUSBStruct->otgUSB_EP[0].transfer_buffer_length=gUSBStruct->otgEP0Setup.wLength;
			gUSBStruct->otgUSB_EP[0].transfer_buffer=(U32)&gUSBStruct->otgGetStatusResponse;
			if	((gUSBStruct->otgEP0Setup.bmRequestType & USB_RECIP_MASK) == USB_RECIP_DEVICE)
			{
				if	(gUSBStruct->otgRemoteWakeup)
					gUSBStruct->otgGetStatusResponse = 0x02;
				/* We are Self-Powered */
				gUSBStruct->otgGetStatusResponse |= 0x01;
				retstatus = USB_PID_IN;
				break;
			}
			if	((gUSBStruct->otgEP0Setup.bmRequestType & USB_RECIP_MASK) == USB_RECIP_INTERFACE)
			{
				retstatus = USB_PID_IN;
				break;
			}
			if	((gUSBStruct->otgEP0Setup.bmRequestType & USB_RECIP_MASK) == USB_RECIP_ENDPOINT)
			{
				pipe = GETLSB(gUSBStruct->otgEP0Setup.wIndex) & 0xff;
							endpoint = usb_pipeendpoint(pipe);
							//direction = usb_pipein(pipe) ? 0 : 1;
					//ep = USB_Ret_Blt_EP_Object(direction);
							if(endpoint == 0)
							   break;
				if	(gUSBStruct->otgUSB_EP[endpoint].Halted)
				   gUSBStruct->otgGetStatusResponse = 0x01;
				retstatus = USB_PID_IN;
				break;
			}
			break;
		case    USB_REQ_CLEAR_FEATURE:
//			printMsg("USB_REQ_CLEAR_FEATURE");
			if	(gUSBStruct->otgUSBState < USB_ADDRESS)
				break;							/* STALL */
			if	((gUSBStruct->otgEP0Setup.bmRequestType & USB_RECIP_MASK) == USB_RECIP_DEVICE)
			{
				  if  (GETLSB(gUSBStruct->otgEP0Setup.wValue) == DEVICE_REMOTE_WAKEUP)
				  {
					   gUSBStruct->otgRemoteWakeup = 0;
					   retstatus = USB_PID_ACK;
				  }
			}
            else if ((gUSBStruct->otgEP0Setup.bmRequestType & USB_RECIP_MASK) == USB_RECIP_INTERFACE)
            {
                  if  (gUSBStruct->otgUSBState != USB_CONFIGURED )
                      break;                  /* STALL */
                  retstatus = USB_PID_ACK;
            }
			else if ((gUSBStruct->otgEP0Setup.bmRequestType & USB_RECIP_MASK) == USB_RECIP_ENDPOINT)
			{
				if  ((gUSBStruct->otgUSBState == USB_ADDRESS) && (GETLSB(gUSBStruct->otgEP0Setup.wIndex) != 0))
					break;                              /* STALL */
				if  (GETLSB(gUSBStruct->otgEP0Setup.wValue) == ENDPOINT_HALT)
				{
					pipe = GETLSB(gUSBStruct->otgEP0Setup.wIndex) & 0xff;
					endpoint = usb_pipeendpoint(pipe);
					if (endpoint)
					{
						gUSBStruct->otgUSB_EP[endpoint].Halted = 0;
						//USB_DRC_Index_Select(gUSBStruct->otgUSB_EP[endpoint].BltEP);
						if (gUSBStruct->otgUSB_EP[endpoint].DRCDir)
						{
							USB_REG_WRITE8(0x126, M_RXCSR_CLRDATATOG);//USB_REG_WRITE8(M_REG_RXCSR1, M_RXCSR_CLRDATATOG);
							USB_REG_WRITE8(0x126, 0);//USB_REG_WRITE8(M_REG_RXCSR1, 0);
						}
						else
						{
							USB_REG_WRITE8(0x112, M_TXCSR_CLRDATATOG);//USB_REG_WRITE8(M_REG_TXCSR1, M_TXCSR_CLRDATATOG);
							USB_REG_WRITE8(0x112, 0);//USB_REG_WRITE8(M_REG_TXCSR1, 0);
						}
						//USB_DRC_Index_Select(0);   /* Back to EP0. */
						gUSBStruct->otgClearedEP=1;
					}
					retstatus = USB_PID_ACK;
				}
			}
			break;
		case    USB_REQ_SET_FEATURE:
			MSFUNCTION_printMsg("USB_REQ_SET_FEATURE");
			if  (gUSBStruct->otgUSBState < USB_DEFAULT)
				break;
			if  ((gUSBStruct->otgEP0Setup.bmRequestType & USB_RECIP_MASK) == USB_RECIP_DEVICE)
			{
				if  (GETLSB(gUSBStruct->otgEP0Setup.wValue) == DEVICE_REMOTE_WAKEUP)
				{
					gUSBStruct->otgRemoteWakeup = 1;
					retstatus = USB_PID_ACK;
				}
#if 0
				else if  (GETLSB(gUSBStruct->otgEP0Setup.wValue) == TEST_MODE)
				{
					gUSBStruct->otgTestMode = gUSBStruct->otgEP0Setup.wIndex;
				}
				else if  (GETLSB(gUSBStruct->otgEP0Setup.wValue) == b_hnp_enable)
				{
					 if (gUSBStruct->otgMyDeviceHNPSupport)
						gUSBStruct->otgHNPEnabled = 1;  /* we can become host now */
					 else
						break;
				}
				else if  (GETLSB(gUSBStruct->otgEP0Setup.wValue) == a_hnp_support)
                {
                      if  (gUSBStruct->otgMyDeviceHNPSupport)
                          gUSBStruct->otgHNPSupport = 1;
                      else
                          break;
                }
#endif
				else
				{
					retstatus = USB_PID_STALL;
					break;
				}
				retstatus = USB_PID_ACK;
				break;
			}
			if  ((gUSBStruct->otgEP0Setup.bmRequestType & USB_RECIP_MASK) == USB_RECIP_INTERFACE)
			{
				if  (gUSBStruct->otgUSBState != USB_CONFIGURED )
					break;
				if ((GETLSB(gUSBStruct->otgEP0Setup.wValue) == b_hnp_enable) || (GETLSB(gUSBStruct->otgEP0Setup.wValue) == a_hnp_support) ||
				(GETLSB(gUSBStruct->otgEP0Setup.wValue) == a_alt_hnp_support))
					retstatus = USB_PID_ACK;
					break;
			}
			if	((gUSBStruct->otgEP0Setup.bmRequestType & USB_RECIP_MASK) == USB_RECIP_ENDPOINT)
			{
				 if  ((gUSBStruct->otgUSBState == USB_ADDRESS) && (GETLSB(gUSBStruct->otgEP0Setup.wIndex) != 0))
					  break;
				 if  (GETLSB(gUSBStruct->otgEP0Setup.wValue) == ENDPOINT_HALT)
				 {
								 pipe = GETLSB(gUSBStruct->otgEP0Setup.wIndex) & 0xff;
								 endpoint = usb_pipeendpoint(pipe);
								 //direction = usb_pipein(pipe) ? 0 : 1;
						 //ep = USB_Ret_Blt_EP_Object(direction);
								 if (endpoint)
					 {
						 gUSBStruct->otgUSB_EP[endpoint].Halted = 1;
						 //USB_DRC_Index_Select(gUSBStruct->otgUSB_EP[endpoint].BltEP);
						 if (gUSBStruct->otgUSB_EP[endpoint].DRCDir)
							 USB_REG_WRITE8(0x126, M_RXCSR1_P_SENDSTALL);//USB_REG_WRITE8(M_REG_RXCSR1, M_RXCSR1_P_SENDSTALL);
						 else
							 USB_REG_WRITE8(0x112, M_TXCSR1_P_SENDSTALL);//USB_REG_WRITE8(M_REG_TXCSR1, M_TXCSR1_P_SENDSTALL);
						 //USB_DRC_Index_Select(0);
					 }
					 retstatus = USB_PID_ACK;
					 break;
				 }
			}
			break;
		case    USB_REQ_SET_ADDRESS:
			MSFUNCTION_printMsg("USB_REQ_SET_ADDRESS");
                if(GETLSB(gUSBStruct->otgEP0Setup.wValue) <= 127)
                {
                    gUSBStruct->otgFaddr = GETLSB(gUSBStruct->otgEP0Setup.wValue);
                    if ((gUSBStruct->otgUSBState == USB_DEFAULT) && (gUSBStruct->otgFaddr != 0))
                        gUSBStruct->otgSetFaddr = 0;
					//printf("otgFaddr:0x%x\n",gUSBStruct->otgFaddr);
                    retstatus = USB_PID_ACK;
                }
                break;
		case    USB_REQ_GET_DESCRIPTOR:
#if ENABLE_USB_DEVICE_HID_MODE
				if ((gUSBStruct->otgEP0Setup.bmRequestType & USB_RECIP_MASK) == USB_RECIP_INTERFACE) {
					MSFUNCTION_printMsg( "USB_GET_HID_REPORT_DESCRIPTOR only" );
	                desctype = GETMSB(gUSBStruct->otgEP0Setup.wValue);
					if (desctype != USB_DT_HID_REPORT) {
						printf("Not supported\n");
						return(retstatus);	/* STALL if not valid request */
					}

					MSFUNCTION_printMsg( "USB HID Report descriptor");
					if (desctype == USB_DT_HID_REPORT) {
						if (gUSBStruct->otgEP0Setup.wIndex == 0) {
							scanP = (U8 *)(otg_Msdfn_HID_Report_Dscr1);
							bytesrem = (U32)sizeof(otg_Msdfn_HID_Report_Dscr1);
							length = (U32)sizeof(otg_Msdfn_HID_Report_Dscr1);
						} else {
							scanP = (U8 *)(otg_Msdfn_HID_Report_Dscr2);
							bytesrem = (U32)sizeof(otg_Msdfn_HID_Report_Dscr2);
							length = (U32)sizeof(otg_Msdfn_HID_Report_Dscr2);
						}

						gUSBStruct->otgUSB_EP[0].transfer_buffer_length=MIN(gUSBStruct->otgEP0Setup.wLength, length);
						gUSBStruct->otgUSB_EP[0].transfer_buffer=(unsigned long)scanP;

						retstatus = USB_PID_IN;
						break;
					}
				}
#endif

				/*
				 * Standard Get USB Device Descriptor
				 */
				MSFUNCTION_printMsg( "USB_REQ_GET_DESCRIPTOR" );
				USB_Function_Initial(gUSBStruct); // 20110310
                desctype = GETMSB(gUSBStruct->otgEP0Setup.wValue);
                if  (!((desctype == USB_DT_DEVICE) || (desctype == USB_DT_CONFIG)||
                    (desctype == USB_DT_STRING)||(desctype == USB_DT_DEVICE_QUALIFIER) ||
                    (desctype == USB_DT_OTHER_SPEED)))
                {
			//printf("ERROR\n");
			return(retstatus);  /* STALL if not valid request */
                }
                if (1)//(gUSBStruct->u8USBDevMode == E_USB_MassStorage)
                {
                    scanP = (U8 *)(otg_Msdfn_Dscr);//(U8 *)gUSBStruct->USB_Msdfn_Dscr;
                    bytesrem = (U32)sizeof(otg_Msdfn_Dscr);//gUSBStruct->SizeofUSB_Msdfn_Dscr;
                    //if (gUSBStruct->otgSpeed == HIGH_SPEED)
                    //    {
                    //       *(scanP+82)=0x0;
                    //       *(scanP+83)=0x02;
                    //       *(scanP+89)=0x0;
                    //       *(scanP+90)=0x02;
                    //    }
                }
				else
                    break;
                if  ((!scanP) || (bytesrem <=0))
                {
                    return(retstatus);
                }
                descindex = GETLSB(gUSBStruct->otgEP0Setup.wValue);/* !=0 for string,config*/

                i = descindex;
				#if 1
                while   (bytesrem > 0)
                {
                     if (*(scanP+1) == desctype)
                     {
                         if  (i == 0)
                             break;
                         i--;
                     }
                     bytesrem -= *scanP;
                     scanP += *scanP;
                }
				#endif
                if  (bytesrem <= 0)
                {
					return(retstatus);
                }
                if  ((desctype == USB_DT_CONFIG)||(desctype == USB_DT_OTHER_SPEED))
                {
                     length = (*(scanP+3) << 8) + *(scanP+2);
                }
                else
                     length = *scanP;
                gUSBStruct->otgUSB_EP[0].transfer_buffer_length=MIN(gUSBStruct->otgEP0Setup.wLength,length);
                gUSBStruct->otgUSB_EP[0].transfer_buffer=(unsigned long)scanP;

				//printData("buf addr:%x",  gUSBStruct->otgUSB_EP[0].transfer_buffer);
				//for(j=0;j<gUSBStruct->otgUSB_EP[0].transfer_buffer_length;j++)
				//{
				//	printData("D b_buf:%x", *((U8 *)(gUSBStruct->otgUSB_EP[0].transfer_buffer+j)));
				//}
				//printf("len:0x%x\n",gUSBStruct->otgUSB_EP[0].transfer_buffer_length);
                retstatus = USB_PID_IN;
                break;
		case    USB_REQ_SET_DESCRIPTOR:
                //sp = USB_Malloc(gUSBStruct->otgEP0Setup.wLength);
                sp=(void*)gUSBStruct->UsbSetupCmdBuf;
                gUSBStruct->otgUSB_EP[0].transfer_buffer_length=gUSBStruct->otgEP0Setup.wLength;
                gUSBStruct->otgUSB_EP[0].transfer_buffer=(U32)sp;
                //USB_Free(sp);
                retstatus = USB_PID_OUT;
                break;
	        case    USB_REQ_GET_CONFIGURATION:
                gUSBStruct->otgUSB_EP[0].transfer_buffer_length=gUSBStruct->otgEP0Setup.wLength;
                gUSBStruct->otgUSB_EP[0].transfer_buffer=(U32)&gUSBStruct->otgConfig;
                retstatus = USB_PID_IN;
                break;
	        case    USB_REQ_SET_CONFIGURATION:
                //fnUSBInsert(); //inform shell
				USB_Enable_EP_DRC(1,gUSBStruct);
				USB_Enable_EP_DRC(2,gUSBStruct);
                if  (gUSBStruct->otgUSBState < USB_ADDRESS)
                    break;
                if  (!GETLSB(gUSBStruct->otgEP0Setup.wValue))
                {
                     USB_Change_USB_State(USB_ADDRESS,gUSBStruct);
                     retstatus = USB_PID_ACK;
                     break;
                }
                if  (GETLSB(gUSBStruct->otgEP0Setup.wValue))
                {
                     gUSBStruct->otgConfig = GETLSB(gUSBStruct->otgEP0Setup.wValue);
                     USB_Change_USB_State(USB_CONFIGURED,gUSBStruct);
                     retstatus = USB_PID_ACK;
                     break;
                }
                break;
			case    USB_REQ_GET_INTERFACE:
				if  (gUSBStruct->otgUSBState != USB_CONFIGURED)
					break;
				gUSBStruct->otgUSB_EP[0].transfer_buffer_length=gUSBStruct->otgEP0Setup.wLength;
				gUSBStruct->otgUSB_EP[0].transfer_buffer=(U32)&gUSBStruct->otgInterface;
				retstatus = USB_PID_IN;
				break;
			case    USB_REQ_SET_INTERFACE:
				if  (gUSBStruct->otgUSBState == USB_CONFIGURED)
					retstatus = USB_PID_ACK;
				USB_Function_Initial(gUSBStruct); // 20110310
				USB_Enable_EP_DRC(1,gUSBStruct);
				USB_Enable_EP_DRC(2,gUSBStruct);
				break;
	        case    USB_REQ_SYNCH_FRAME:
	                break;
	        default:
	                break;
	}
	return(retstatus);
}

void USB_Parse_DRC_Int_Peripheral(drcintitem_t *dP,USB_VAR *gUSBStruct)
{
	U8      ep;
    U8      parsestatus;    /* setup parse result */
    U8      outreg;         /* reset EP0 DRC regs with this */

	outreg = 0;
	ep = dP->IEP;
	gUSBStruct->otgIntStatus = 0xff;           /* unused URB status code*/
	//USB_DRC_Index_Select(ep);      /* get our idx in window */
	do{
		if  (!ep)                  /* commence EP0 analysis */
		{
			#if 0
			if (gUSBStruct->otgTestMode)
			{
				//no test mode
			}
			#endif
			if  (dP->IReg & M_CSR0_P_SENTSTALL)
			{
				gUSBStruct->otgUSB_EP[0].IOState = EP_IDLE;
				USB_REG_WRITE8(0x102, outreg);//USB_REG_WRITE8(M_REG_CSR0, 0);
			}
			if  (dP->IReg & M_CSR0_P_SETUPEND)
			{
				outreg =  M_CSR0_P_SVDSETUPEND;
				gUSBStruct->otgUSB_EP[0].IOState = EP_IDLE;
				USB_REG_WRITE8(0x102, outreg);//USB_REG_WRITE8(M_REG_CSR0, outreg);
			}
			if (gUSBStruct->otgUSB_EP[0].IOState == EP_IDLE)
			{
				if  (dP->IReg & M_CSR0_RXPKTRDY)
				{
					outreg = M_CSR0_P_SVDRXPKTRDY;
					gUSBStruct->otgUSB_EP[0].FifoRemain = dP->ICount;
					gUSBStruct->otgUSB_EP[0].BytesRequested = 8;
					gUSBStruct->otgUSB_EP[0].BytesProcessed = 0;
					USB_DRC_Fifo_Read((U8 *)&gUSBStruct->otgEP0Setup, 0,gUSBStruct);
					USB_SWOP_Setup((struct devrequest *)&gUSBStruct->otgEP0Setup);
					parsestatus = USB_Parse_Received_Setup(gUSBStruct);

					//USB_REG_WRITE8(M_REG_INDEX, 0);
					if  (parsestatus == USB_PID_ACK)
					{
						outreg |= M_CSR0_P_DATAEND;
						if  ((!gUSBStruct->otgSetFaddr) && (gUSBStruct->otgFaddr) && (gUSBStruct->otgUSBState < USB_ADDRESS))
						{
							USB_Change_USB_State(USB_ADDRESS,gUSBStruct);
							gUSBStruct->otgSetFaddr = 1;
						}
					}
					else if (parsestatus == USB_PID_OUT)
					{
						USB_REG_WRITE8(0x102, outreg);//USB_REG_WRITE8(M_REG_CSR0, outreg);
						outreg = 0;
						gUSBStruct->otgUSB_EP[0].IOState = EP_RX;
						gUSBStruct->otgUSB_EP[0].BytesProcessed = 0;
						gUSBStruct->otgUSB_EP[0].BytesRequested = gUSBStruct->otgEP0Setup.wLength;
						break;
					}
					else if (parsestatus == USB_PID_IN)
					{
						USB_REG_WRITE8(0x102, outreg);//USB_REG_WRITE8(M_REG_CSR0, outreg);
						outreg = 0;
						gUSBStruct->otgUSB_EP[0].IOState = EP_TX;
						gUSBStruct->otgUSB_EP[0].LastPacket = 0;
						gUSBStruct->otgUSB_EP[0].BytesProcessed = 0;
						gUSBStruct->otgUSB_EP[0].BytesRequested = gUSBStruct->otgUSB_EP[0].transfer_buffer_length;

					}
					else
					{
						outreg = M_CSR0_P_SENDSTALL | M_CSR0_P_SVDRXPKTRDY;
					}
				}
				else
				{
					if  (gUSBStruct->otgSetFaddr)
						USB_REG_WRITE8(M_REG_FADDR, gUSBStruct->otgFaddr);
					gUSBStruct->otgSetFaddr = 0;
					if  (gUSBStruct->otgClearedEP)
					{
						gUSBStruct->otgClearedEP = 0;
					}
				}
			}
			if (gUSBStruct->otgUSB_EP[0].IOState == EP_TX)
			{
				outreg = USB_Function_Load_Fifo(0,gUSBStruct);
			}
			if (gUSBStruct->otgUSB_EP[0].IOState == EP_RX)   //wayne added
			{
				if  (dP->IReg & M_CSR0_RXPKTRDY)
				{
					gUSBStruct->otgUSB_EP[0].FifoRemain = dP->ICount;
					USB_DRC_Fifo_Read((U8 *)gUSBStruct->otgUSB_EP[0].transfer_buffer,0,gUSBStruct);
					outreg = 0;
					if (gUSBStruct->otgUSB_EP[0].BytesProcessed >= gUSBStruct->otgUSB_EP[0].BytesRequested)
					{
						outreg |= M_CSR0_P_DATAEND;
						gUSBStruct->otgUSB_EP[0].IOState = EP_IDLE;
						//USB_REG_WRITE8(M_REG_INDEX, 0);
						USB_REG_WRITE8(0x102, outreg);//USB_REG_WRITE8(M_REG_CSR0, outreg);
					}
					outreg = M_CSR0_P_SVDRXPKTRDY;
					USB_REG_WRITE8(0x102, outreg);//USB_REG_WRITE8(M_REG_CSR0, outreg);
					outreg = 0;
#if ENABLE_USB_DEVICE_HID_MODE
					if (IsHID_EP0_Rx_Request(gUSBStruct)) {
						printf("Rx HID EP0 request\n");
					}
#endif
					break;
				}
			}
			if  (outreg)
			{
				USB_REG_WRITE8(0x102, outreg);//USB_REG_WRITE8(M_REG_CSR0, outreg);
				//printf("M_REG_CSR0:%x\n",USB_REG_READ8(M_REG_CSR0));
			}
			break;
		}
		if  (dP->IIntSrc == EP_RX)
		{
			if	((dP->IReg & M_RXCSR1_P_SENTSTALL)||!(dP->IReg & M_RXCSR1_RXPKTRDY))
			{
				gUSBStruct->otgIntStatus = USB_ST_STALL;
			//				  printf("RX_STALL\n");
				outreg = 0;
				USB_REG_WRITE8(0x126, outreg);//USB_REG_WRITE8(M_REG_RXCSR1, outreg);
				break;
			}
			else if (gUSBStruct->otgUSB_EP[ep].Halted)
			{
				outreg = M_RXCSR1_P_SENDSTALL;
			//				  printf("RX_Halt\n");
				USB_REG_WRITE8(0x126, outreg);//USB_REG_WRITE8(M_REG_RXCSR1, outreg);
				break;
			}
			else  //normal condition
			{
				gUSBStruct->otgUSB_EP[ep].FifoRemain = dP->ICount;
				//USB_DBG_MSG("ep%dRem:%x\n",  ep,	gUSBStruct->otgUSB_EP[ep].FifoRemain);
				//printf("dev:%d ", gUSBStruct->u8USBDevMode);
				#if 0
				if (gUSBStruct->u8USBDevMode == E_USB_VirtCOM)
				{
					//printf("ep%dRem:%x\n",  ep,  gUSBStruct->otgUSB_EP[ep].FifoRemain);
					gUSBStruct->otgUSB_EP[ep].BytesProcessed=0;
					gUSBStruct->otgUSB_EP[ep].BytesRequested=dP->ICount;
					//cdcRxBuflen+=dP->ICount;
					//gUSBStruct->otgUSB_EP[ep].transfer_buffer=otgCDCRxBufAddr+(cdcRxBuflen-dP->ICount);
					gUSBStruct->otgUSB_EP[ep].transfer_buffer_length=0;
					//if (USB_Set_DMA(ep, DMA_RX_ZERO_IRQ, 0,gUSBStruct) < 0)
					{
						USB_DRC_Fifo_Read((U8 *)(gUSBStruct->otgUSB_EP[ep].transfer_buffer), ep,gUSBStruct);
						gUSBStruct->otgUSB_EP[ep].FifoRemain = USB_Read_RxCount();
					}
				}
				else
				#endif
				{
#if ENABLE_USB_DEVICE_HID_MODE
					/*
					 *	HID mode EP2 RX
					 */
					printf("%s %d FIFORemain=%X\n", __func__, __LINE__, gUSBStruct->otgUSB_EP[ep].FifoRemain);
					printf("      dP->ICount=%X\n", dP->ICount);

					USB_Set_ClrRXMode1();

					if ( gUSBStruct->otgUSB_EP[ep].FifoRemain < HID_TRX_LENGTH )
						HID_Set_EPxRxLen(gUSBStruct->otgUSB_EP[ep].FifoRemain);

					gUSBStruct->otgUSB_EP[ep].BytesProcessed=0;
					gUSBStruct->otgUSB_EP[ep].BytesRequested=dP->ICount;
					gUSBStruct->otgUSB_EP[ep].transfer_buffer = (U32)HID_Get_EPxRxBuff();
					gUSBStruct->otgUSB_EP[ep].transfer_buffer_length=0;
					USB_DRC_Fifo_Read((U8 *)(gUSBStruct->otgUSB_EP[ep].transfer_buffer), ep, gUSBStruct);

					gUSBStruct->otgUSB_EP[ep].FifoRemain = USB_Read_RxCount();
				    printf("FifoRemain :%x\n", gUSBStruct->otgUSB_EP[ep].FifoRemain);
					if	(!gUSBStruct->otgUSB_EP[ep].FifoRemain)
					{
						USB_REG_WRITE8(0x126, 0);//USB_REG_WRITE8(M_REG_RXCSR1, 0);
						while(USB_REG_READ8(0x126))
						{
							USB_REG_WRITE8(0x126, 0);
						}
					}

					printf("RxCSR = %X\n", USB_REG_READ8(0x126));

					HIDRxBuff(gUSBStruct);

					USB_REG_WRITE8(0x126, 0);	//USB_REG_WRITE8(M_REG_RXCSR1, 0);
					while(USB_REG_READ8(0x126)) 	{
						USB_REG_WRITE8(0x126, 0);
					}
					printf("clear RxCSR = %X\n", USB_REG_READ8(0x126));
#else
					if ((gUSBStruct->otgUSB_EP[ep].FifoRemain == 30) ||
						(gUSBStruct->otgUSB_EP[ep].FifoRemain == 31) ||
						(gUSBStruct->otgUSB_EP[ep].FifoRemain == 32))	/* USBCV MSC test will issue 30 bytes CBW */
					{
						gUSBStruct->otgUSB_EP[ep].BytesProcessed=0;
						gUSBStruct->otgUSB_EP[ep].BytesRequested=dP->ICount;
						gUSBStruct->otgUSB_EP[ep].transfer_buffer=gUSBStruct->otgCBWCB_Addr;
						gUSBStruct->otgUSB_EP[ep].transfer_buffer_length=0;
						if (gUSBStruct->otgUSB_EP[ep].FifoRemain < gUSBStruct->otgUSB_EP[ep].MaxEPSize)
							gUSBStruct->otgUSB_EP[ep].LastPacket = 1;
						else
						{
							if ((gUSBStruct->otgUSB_EP[ep].BytesProcessed + gUSBStruct->otgUSB_EP[ep].FifoRemain) >= gUSBStruct->otgUSB_EP[ep].BytesRequested)
								gUSBStruct->otgUSB_EP[ep].LastPacket = 1;
							else
								gUSBStruct->otgUSB_EP[ep].LastPacket = 0;
						}
									USB_DRC_Fifo_Read((U8 *)(gUSBStruct->otgUSB_EP[ep].transfer_buffer), ep,gUSBStruct);
							//printf("cmd:%x\n",((U8*)gUSBStruct->otgUSB_EP[ep].transfer_buffer)[15]);
									if	(((!gUSBStruct->otgUSB_EP[ep].FifoRemain) && (gUSBStruct->otgUSB_EP[ep].LastPacket)) || (gUSBStruct->otgUSB_EP[ep].BytesProcessed >= gUSBStruct->otgUSB_EP[ep].BytesRequested))
									{
											gUSBStruct->otgIntStatus = USB_ST_NOERROR;
									}
									gUSBStruct->otgUSB_EP[ep].FifoRemain = USB_Read_RxCount();
									   //  printf("FifoRemain2:%x\n",	 gUSBStruct->otgUSB_EP[ep].FifoRemain);
									if	(!gUSBStruct->otgUSB_EP[ep].FifoRemain)
									{
										USB_REG_WRITE8(0x126, 0);//USB_REG_WRITE8(M_REG_RXCSR1, 0);
										while(USB_REG_READ8(0x126))
										{
											USB_REG_WRITE8(0x126, 0);
										}
									}
							gUSBStruct->otgMassCmdRevFlag=1;
							//printf("set_flag\n");
					}
					else
					{
						if (gUSBStruct->otgUSB_EP[ep].FifoRemain>0)
						{
							gUSBStruct->otgMassRxDataReceived=1;
							//printf("isr_RxDataRcv\n");
						}
						gUSBStruct->otgMassCmdRevFlag=0;
					}
#endif
				}
					break;
			}
		}
		else
		{
#if ENABLE_USB_DEVICE_HID_MODE
			/*
			 *	HID mode EP1 TX
			 */
#else
			if (dP->IReg & M_TXCSR1_P_SENTSTALL)
			{
				gUSBStruct->otgIntStatus = USB_ST_STALL;
				USB_REG_WRITE8(0x112, 0);//USB_REG_WRITE8(M_REG_TXCSR1, 0);
				return;
			}
			if  (gUSBStruct->otgUSB_EP[ep].LastPacket)
			{
				//printf("tx USB_ST_NOERROR\n");
				gUSBStruct->otgIntStatus = USB_ST_NOERROR;
				break;
			}
			if  (gUSBStruct->otgUSB_EP[ep].Halted)
			{
				USB_REG_WRITE8(0x112, M_TXCSR1_P_SENDSTALL);//USB_REG_WRITE8(M_REG_TXCSR1, M_TXCSR1_P_SENDSTALL);
				break;
			}
			//printf("tx_fifo\n");
			USB_DRC_Fifo_Write((U8 *)gUSBStruct->otgUSB_EP[ep].transfer_buffer,ep,gUSBStruct);
			USB_REG_WRITE8(0x112, M_TXCSR1_TXPKTRDY);//USB_REG_WRITE8(M_REG_TXCSR1, M_TXCSR1_TXPKTRDY);
			while (!OTG_Interrupt_Polling(0x21, gUSBStruct)) ;
#endif
		}
	}
	while   (0);
	if  (gUSBStruct->otgIntStatus != 0xff)
	{
		//if	(!(usb_pipeisoc(gUSBStruct->otgUSB_EP[ep].pipe)))
		{
			gUSBStruct->otgUSB_EP[ep].IOState = 0;
			if (gUSBStruct->otgUSB_EP[ep].FifoRemain)
				USB_DRC_Fifo_Read((U8 *)gUSBStruct->otgUSB_EP[ep].transfer_buffer,ep,gUSBStruct);
		}
	}
	return;
}
