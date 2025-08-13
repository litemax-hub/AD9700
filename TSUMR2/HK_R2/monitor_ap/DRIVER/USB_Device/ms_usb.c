#include "ms_config.h"
#include "type.h"
#include "ms_msd_global.h"
#include "ms_cpu.h"
#include "ms_drc.h"

extern void printMsg( char *str );
extern void printData( char *str, unsigned int value );
extern void ForceDelay1ms( unsigned int msNums );


S8 USB_Send_Stall(U8 ep,USB_VAR *gUSBStruct)
{
    unsigned long csr;
	unsigned long reg;
	unsigned long reg_bit;
    unsigned long timeout = 0;

    if (gUSBStruct->otgUSB_EP[ep].BltEP == 0)
    {
        reg = 0x102;//M_REG_CSR0;
          reg_bit = M_CSR0_P_SENDSTALL;
    }
    else
    {
        //USB_DRC_Index_Select(gUSBStruct->otgUSB_EP[ep].BltEP);
        if (gUSBStruct->otgUSB_EP[ep].DRCDir)   /* RX */
        {
        	//printMsg("rx sedn stall\n");
            reg = 0x126;//M_REG_RXCSR1;
            reg_bit = M_RXCSR1_P_SENDSTALL;
        }
        else             /* TX */
        {
        	//printMsg("tx sedn stall\n");
            reg = 0x112;//M_REG_TXCSR1;
            reg_bit = M_TXCSR1_P_SENDSTALL;
        }
    }

    csr = USB_REG_READ8(reg);
    USB_REG_WRITE8(reg, (csr | reg_bit));
	//printData("csr:%x",USB_REG_READ8(reg));
	ForceDelay1ms(100);
    if (gUSBStruct->otgUSB_EP[ep].DRCDir)   /* RX */
    {
//        printf("R:%x \n",USB_REG_READ8(reg));
        while((USB_REG_READ8(0x126/*M_REG_RXCSR1*/)&M_RXCSR1_P_SENDSTALL)&&(gUSBStruct->u8USBDeviceMode==1))
        {
            /* wait 1 second timeout */
            if(timeout > 100000)
                break;
            timeout++;
            //hal_delay_us(10);
        };
    }
    else             /* TX */
    {
//        printf("T:%x \n",USB_REG_READ8(reg));
		while (!OTG_Interrupt_Polling(0x20, gUSBStruct)) ;//polling ep0 interrupt
        while((USB_REG_READ8(0x112/*M_REG_TXCSR1*/)&M_TXCSR1_P_SENDSTALL)&&(gUSBStruct->u8USBDeviceMode==1))
        {
            /* wait 1 second timeout */
            if(timeout > 100000)
                break;
            timeout++;
            //hal_delay_us(10);
        };
    }
    return 1;
}

void USB_SWOP_Setup(struct devrequest *sP)
{
    SWOP(sP->wValue);
    SWOP(sP->wIndex);
    SWOP(sP->wLength);
}

U8 otgIsUSBConfiged(USB_VAR *gUSBStruct)
{
      if (gUSBStruct->otgUSBState==USB_CONFIGURED)
          return 1;
      else
          return 0;
}

void USB_Change_USB_State(U8 toUSB,USB_VAR *gUSBStruct)
{
    switch  (toUSB)
    {
        case USB_POWERED:   /* HOST:CONNECT DRC INT;PER:exit active mode*/
            gUSBStruct->otgSuspended = 0;
            break;
        case USB_DEFAULT:   /* HOST:Reset complete;PER: RESET DRC INT */
            gUSBStruct->otgHNPEnabled = 0;
            gUSBStruct->otgHNPSupport = 0;
            gUSBStruct->otgRemoteWakeup = 0;
            gUSBStruct->otgSuspended = 0;
            gUSBStruct->u8USBDeviceMode = 0;	// 20110310
            break;
        case USB_ADDRESS:
            gUSBStruct->otgConfig = 0;
            break;
        case USB_CONFIGURED:
         gUSBStruct->u8USBDeviceMode = 1;
            //DBG_MSG("USB Configured\n");
            break;
        default:
            break;
    }
    gUSBStruct->otgUSBState = toUSB;
}
