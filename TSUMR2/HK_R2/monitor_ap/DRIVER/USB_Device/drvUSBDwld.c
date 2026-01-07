#include "ms_config.h"
#include "type.h"
#include "Globalvar.h"
#include "sysusbdrv.h"
#include "drvUSBDwld.h"
#include "ms_drc.h"
#include "ms_msd_fn_scsi.h"
#include "ms_usb.h"
#include <string.h>
#include "msflash.h"
#include "ms_usb.h"
#include "board.h"
#include "msFB.h"
#include "drvMcu.h"
#include "drvIMI.h"
#include "Global.h"
#include "ms_hid.h"

extern void printMsg( char *str );
extern void printData( char *str, WORD value );
extern void upll_enable(void);

#define USB_DEBUG    1
#if ENABLE_MSTV_UART_DEBUG && USB_DEBUG
#define USB_printData(str, value)   printData(str, value)
#define USB_printMsg(str)           printMsg(str)
#else
#define USB_printData(str, value)
#define USB_printMsg(str)
#endif



#if 0
u8 readb(const volatile void *addr)
{
	return *(const volatile u8 *) addr;
}

void writeb(u8 b, volatile void *addr)
{
	*(volatile u8 *) addr = b;
}

u16 readw(const volatile void *addr)
{
	return *(const volatile u16 *) addr;
}

void writew(u16 b, volatile void *addr)
{
	*(volatile u16 *) addr = b;
}
#endif

void *otg_memcpy(void *dest, const void *src, unsigned int n)
{
    char *dp = dest;
    const char *sp = src;
    while (n--)
        *dp++ = *sp++;
    return dest;
}

U32 MDrv_USB_Read(U8 *buf, U32 len, USB_VAR *gUSBStruct)
{
    U32     rdLen;
    vendorCmdStruct *pCmdStruct = (vendorCmdStruct *)gUSBStruct->ptrVCmdStruct;
    //if (!isUsbInit)
    //    return 0;
    if (pCmdStruct->currDwldBytes - pCmdStruct->accReadBytes > len)
        rdLen = len;
    else
    {
        rdLen = pCmdStruct->currDwldBytes - pCmdStruct->accReadBytes;
        pCmdStruct->currDwldBytes = 0;
    }
    if (rdLen)
    {
	//USB_printData("rdLen:0x%x\n",rdLen);
        otg_memcpy(buf, pCmdStruct->ucptrDataBuf + pCmdStruct->accReadBytes, rdLen);
        if (!pCmdStruct->currDwldBytes)
            pCmdStruct->accReadBytes = 0;
        else
            pCmdStruct->accReadBytes += rdLen;
    }
    return  rdLen;
}

void MDrv_USB_Write(U8 *buf, U32 len, USB_VAR *gUSBStruct)
{
    vendorCmdStruct *pCmdStruct = (vendorCmdStruct *)gUSBStruct->ptrVCmdStruct;
    S32     wrLen;
    U32     u32offset = pCmdStruct->currUpldOffset;
	//TONY
	int n;

    //if (!isUsbInit)
    //    return 0;
    /* Flush USB buffer for writing first (USB R/W is a simplex channel) */
    pCmdStruct->currDwldBytes = 0;
    pCmdStruct->accReadBytes = 0;
    if (u32offset)
    {
        if (pCmdStruct->currUpldBytes)
            otg_memcpy(pCmdStruct->ucptrDataBuf, pCmdStruct->ucptrDataBuf + u32offset,
                   pCmdStruct->currUpldBytes);
        /* Reset currUpldOffset member */
        pCmdStruct->currUpldOffset = 0;
    }

#if 0
    u32offset = pCmdStruct->currUpldBytes;
    if (u32offset + len <= (16*1024))
        wrLen = len;
    else
        wrLen = (16*1024) - u32offset;
#else
    u32offset = 0;
    wrLen = len;
#endif

    //memcpy(pCmdStruct->ptrPPB2Buf/* + u32offset*/, buf, wrLen);
    for(n=0; n<wrLen; n++)
    {
		pCmdStruct->ptrPPB2Buf[n] = buf[n];
    }
    pCmdStruct->currUpldBytes += wrLen;
//    return  wrLen;
}


void UDC_UTMI_Init(void)
{
	UTMI_REG_WRITE8(0x02, (UTMI_REG_READ8(0x02)) | 0x04);
	UTMI_REG_WRITE8(0x03, (UTMI_REG_READ8(0x03) & ~0x10) | 0x08 | 0x20);
	UTMI_REG_WRITE8(0x04, (UTMI_REG_READ8(0x04)) | 0x40 | 0x20);
	UTMI_REG_WRITE8(0x06, (UTMI_REG_READ8(0x06) & ~0x04 & ~0x20) | 0x40);
	UTMI_REG_WRITE8(0x07, (UTMI_REG_READ8(0x07) & ~0x02));
	UTMI_REG_WRITE8(0x08, (UTMI_REG_READ8(0x08) & ~0x80) | 0x04 |0x08);
	UTMI_REG_WRITE8(0x09, (UTMI_REG_READ8(0x09)) | 0x80 |0x01);
	UTMI_REG_WRITE8(0x0b, (UTMI_REG_READ8(0x0b)) | 0x80);
	UTMI_REG_WRITE8(0x15, (UTMI_REG_READ8(0x15)) | 0x20 | 0x40);

	UTMI_REG_WRITE8(0x24, 0x00);
	UTMI_REG_WRITE8(0x29, 0x00);
	UTMI_REG_WRITE8(0x2a, 0x00);

	UTMI_REG_WRITE8(0x2C, 0x10);
	UTMI_REG_WRITE8(0x2D, 0x02);
	UTMI_REG_WRITE8(0x2E, 0x00);
	UTMI_REG_WRITE8(0x2F, 0x81);

	UTMI_REG_WRITE8(0x3f, (UTMI_REG_READ8(0x3f)) | 0x80);
}

void MDrv_USB_Init_Device(void)
{
	int ii=0;

	//USB_printMsg("MDrv_USB_Init++");
	/***	create irq
	hal_interrupt_mmp_create( CYGNUM_HAL_INTERRUPT_OTG, 0, (isr_entry)usbIsr, NULL);
	***/

	USBC_REG_WRITE8(0x02, (USBC_REG_READ8(0x02)& ~(BIT0|BIT1)) | (BIT1));
	UTMI_REG_WRITE8(0x3C, UTMI_REG_READ8(0x3C) | 0x1); // set CA_START as 1
	for(ii=0;ii<1000;ii++)
	{
		//nothing
	}
	UTMI_REG_WRITE8(0x3C, UTMI_REG_READ8(0x3C) & ~0x01); // release CA_START
	while ((UTMI_REG_READ8(0x3C) & 0x02) == 0);        // polling bit <1> (CA_END)

	// Reset OTG controllers
	USBC_REG_WRITE8(0, 0xC);

	// Unlock Register R/W functions  (RST_CTRL[6] = 1)
	// Enter suspend  (RST_CTRL[3] = 1)
	USBC_REG_WRITE8(0, 0x48);

//	USB_printMsg( "utmi++" );
//	UDC_UTMI_Init();
//	USB_printMsg( "utmi--" );

	// 2'b10: OTG enable
	USBC_REG_WRITE8(0x02, (USBC_REG_READ8(0x02)& ~(BIT0|BIT1)) | (BIT1));

	USB_REG_WRITE8(0x80, USB_REG_READ8(0x80)&0xFE); // Reset OTG
	USB_REG_WRITE8(0x80, USB_REG_READ8(0x80)|0x01);
	//USB_REG_WRITE16(0x80, USB_REG_READ16(0x80)|0x8000); /* Disable DM pull-down */

	// Set FAddr to 0
	USB_REG_WRITE8(M_REG_FADDR, 0);
	// Set Index to 0
	USB_REG_WRITE8(M_REG_INDEX, 0);
	USB_REG_WRITE8(M_REG_CFG6_H, USB_REG_READ8(M_REG_CFG6_H) | 0x08);
	USB_REG_WRITE8(M_REG_CFG6_H, USB_REG_READ8(M_REG_CFG6_H) | 0x40);

	UDC_UTMI_Init();

#if CHIP_ID == CHIP_MT9700
	//swap
	UTMI_REG_WRITE8(0x0b, UTMI_REG_READ8(0xb) | 0x20);
#endif

	//Pull up D+
	//printk("HIGH SPEED\n");
	USB_REG_WRITE8(M_REG_POWER, (USB_REG_READ8(M_REG_POWER) & ~M_POWER_ENSUSPEND) | M_POWER_SOFTCONN | M_POWER_HSENAB);

	USB_REG_WRITE8(M_REG_DEVCTL,0);

	// Flush the next packet to be transmitted/ read from the endpoint 0 FIFO
	USB_REG_WRITE16(M_REG_CSR0, USB_REG_READ16(M_REG_CSR0) | M_CSR0_FLUSHFIFO);
	// Flush the latest packet from the endpoint Tx FIFO
	USB_REG_WRITE8(M_REG_INDEX, 1);
	USB_REG_WRITE16(M_REG_TXCSR, USB_REG_READ16(M_REG_TXCSR) | M_TXCSR_FLUSHFIFO);

	// Flush the next packet to be read from the endpoint Rx FIFO
	USB_REG_WRITE8(M_REG_INDEX, 2);
	USB_REG_WRITE16(M_REG_RXCSR, USB_REG_READ16(M_REG_RXCSR) | M_RXCSR_FLUSHFIFO);

	USB_REG_WRITE8(M_REG_INDEX, 0);

	// Clear all control/status registers
	USB_REG_WRITE16(M_REG_CSR0, 0);
	USB_REG_WRITE8(M_REG_INDEX, 1);
	USB_REG_WRITE16(M_REG_TXCSR, 0);
	USB_REG_WRITE8(M_REG_INDEX, 2);
	USB_REG_WRITE16(M_REG_RXCSR, 0);

	USB_REG_WRITE8(M_REG_INDEX, 0);

	// Enable all endpoint interrupts
	USB_Set_DRC_Interrupts();

	USB_REG_WRITE8(M_REG_FADDR, 0);
	USB_REG_WRITE8(M_REG_CFG6_H, USB_REG_READ8(M_REG_CFG6_H) | 0x08);
	USB_REG_WRITE8(M_REG_POWER, M_POWER_SOFTCONN | M_POWER_HSENAB);

	USB_REG_WRITE8(M_REG_DEVCTL, 0);
	//USB_printMsg("MDrv_USB_Init--");
//	isUsbInit = 1; // for MDrv_USB_Read and MDrv_USB_Write error handling
}

#if ENABLE_USB_DEVICE_HID_MODE
void MDrv_USB_CmdLoop(void *pUsbStruct, U8 *pVendorCMD)
{
    if (otgIsUSBConfiged((USB_VAR *)pUsbStruct))
        HIDFNloop((USB_VAR *)pUsbStruct, pVendorCMD);
}
#else
void MDrv_USB_CmdLoop(void *pUsbStruct, U8 *pVendorCMD)
{
    if (otgIsUSBConfiged((USB_VAR *)pUsbStruct))
    {
        MSDFNCMDloop((USB_VAR *)pUsbStruct, pVendorCMD);
    }
}
#endif

#if ENABLE_USB_DEVICE

#define USBData    ((unsigned char  volatile *) DOWNLOAD_BUFFER_ADR)

#if ENABLE_USB_DEVICE_UPDATE_CODE
U32 u32SPIOffset = 0, u32DownloadOffset = 0;
U8 NonEraseSector=0;
#endif

U32 USBRxCnt, USBStoreCnt, USBTransferSize;
U16 FileCheckSum, DataCheckSum;

#if ENABLE_USB_DEVICE_HID_MODE
U8 USBRxBuf[HID_TRX_LENGTH], USBTxBuf[HID_TRX_LENGTH];
#else
U8 USBRxBuf[2048], USBTxBuf[20];
#endif

USB_VAR	usbDrvVar;
vendorCmdStruct	scsiVendorCmdVar;

void USB_Device_Init(void)
{
	USB_printMsg( "USBInit" );

    upll_enable();

    DataCheckSum = 0;
	/* Init. SCSI vendor command struct */
	Init_vendorCmd_Var(&scsiVendorCmdVar);
	Init_gUSB_Var(&usbDrvVar, &scsiVendorCmdVar);
#if ENABLE_USB_DEVICE_HID_MODE
	HID_Init();
#endif
	/* Init. Mass storage class buffer */
	otgInitMassBuffer(&usbDrvVar);
    MDrv_USB_Init_Device();

	usbDrvVar.DeviceConnect = 1;
    USBRxCnt = 0;

#if ENABLE_USB_DEVICE_HID_MODE
    HID_Tx_CtrlReq(USBTxBuf, HID_TRX_LENGTH);
#else
    scsiVendorCmdVar.ucptrDataBuf = (U8 *)(USBRxBuf);
    scsiVendorCmdVar.ptrPPB2Buf = (U8 *)(USBTxBuf);

    memset(scsiVendorCmdVar.ptrPPB2Buf,0x0,0x4);
#endif
}

void USB_Handler(void)
{
    USB_CMDHandler();

#if !ENABLE_USB_DEVICE_HID_MODE
    if(USBTransferSize)
        USB_DataHandler();
#endif
}

#if ENABLE_USB_DEVICE_HID_MODE

#if ENABLE_USB_DEVICE_UPDATE_CODE
#define FLASH_SECTOR_SIZE 0x1000
BYTE SPIBuffer[FLASH_SECTOR_SIZE] = {0};
WORD SPIBuffer_counter = 0;
#endif

BYTE CheckHIDCommandValid(BYTE* Data)
{
    if((Data[0]==0x55)&&(Data[1]==0xAA)&&(Data[2]==0xAB)&&(Data[3]==0xCD) &&          \
       (Data[HID_TRX_LENGTH-4]==0xAB)&&(Data[HID_TRX_LENGTH-3]==0xCD)&&(Data[HID_TRX_LENGTH-2]==0xAA)&&(Data[HID_TRX_LENGTH-1] == 0x55))
        return TRUE;
    else
        return FALSE;
}

void USB_CMDHandler(void)
{
	USB_DRC_Interrupt(&usbDrvVar);
    MDrv_USB_CmdLoop((void*)(&usbDrvVar), USBRxBuf);

    if( CheckHIDCommandValid(USBRxBuf))
    {
        switch(USBRxBuf[4])
        {
            case XROM_USB_FILE_SIZE:
                USBTransferSize = (U32)(USBRxBuf[8]<<24|USBRxBuf[9]<<16|USBRxBuf[10]<<8|USBRxBuf[11]);
                USB_printData("USBTransferSize_H: 0x%x", USBTransferSize >> 16);
                USB_printData("USBTransferSize_L: 0x%x", USBTransferSize & 0xFFFF);

#if ENABLE_USB_DEVICE_UPDATE_CODE
                if(g_dwSpiDuelImageOffset < ENABLE_USB_HOST_DUAL_IMAGE_OFFSET)
                    u32SPIOffset = ENABLE_USB_HOST_DUAL_IMAGE_OFFSET;
                else
                {
                    u32SPIOffset = 0;
                    u32DownloadOffset = ENABLE_USB_HOST_DUAL_IMAGE_SBOOT;
                }

                mcuSetSpiMode(SPI_MODE_FR); // set FR mode to avoid continue switching mode, speed up program time
#else
                if(USBTransferSize > DOWNLOAD_BUFFER_LEN)
                {
                    USB_printMsg("[Warning!!] USBTransferSize > Buffer Size");
                }
#endif
                #if ENABLE_RTE
                    msAPI_OverDriveEnable(0); // USB & OD share the same space. Before USB F/W download, OD should be off first.
                #endif

                DataCheckSum = 0;
                FileCheckSum = 0;
                USBStoreCnt = 0;

#if ENABLE_USB_DEVICE_UPDATE_CODE
                SPIBuffer_counter = 0;
                NonEraseSector = 0;
                SPIWCnt = 0;
#endif
            break;

            case XROM_USB_FILE_CHECKSUM:
                FileCheckSum = (U16)(USBRxBuf[8]<<8|USBRxBuf[9]);
                USB_printData("FileCheckSum:0x%x", FileCheckSum);
            break;

            case XROM_USB_HID_DATA:
                USB_DataHandler(USBRxBuf);
            break;

            case XROM_USB_REBOOT_SYS:
                USB_printMsg("Reboot System\r\n\r\n");
                msWrite4Byte( REG_002C08, 1);           //Reboot System
                while(1);
            break;

            default:
             break;
        }

        //clear Rx buffer
        memset(HID_Get_EP0RxBuff(), 0, HID_TRX_LENGTH);
        memset(USBRxBuf, 0, HID_TRX_LENGTH);
    }
}

void USB_SendUpdateScuessCmd(BYTE u8Scuess)
{
    BYTE usb_magic[4] = {0xD0,0xD0,0xD0,0xD0};

    if(u8Scuess)
        MST_UpdateStatus = MST_UPDATE_STATUS_SUCCESS;
    else
        MST_UpdateStatus = MST_UPDATE_STATUS_FAIL;

    if(!u8Scuess)
        usb_magic[0] = 0xCC;

    USBTxBuf[0] = 0x55;
    USBTxBuf[1] = 0xAA;
    USBTxBuf[2] = 0xAB;
    USBTxBuf[3] = 0xCD;
    USBTxBuf[4] = 0x02;
    USBTxBuf[5] = 0xA0;
    USBTxBuf[6] = 0x00;
    USBTxBuf[7] = 0x04;
    USBTxBuf[8] = usb_magic[0];
    USBTxBuf[9] = usb_magic[1];
    USBTxBuf[10] = usb_magic[2];
    USBTxBuf[11] = usb_magic[3];
    USBTxBuf[HID_TRX_LENGTH-4] = 0xAB;
    USBTxBuf[HID_TRX_LENGTH-3] = 0xCD;
    USBTxBuf[HID_TRX_LENGTH-2] = 0xAA;
    USBTxBuf[HID_TRX_LENGTH-1] = 0x55;
}

void USB_DataHandler(BYTE* USB_Data)
{
	DWORD i = 0;
    BYTE CheckSum_Flag;
    WORD TransferSize;

#if ENABLE_USB_DEVICE_UPDATE_CODE
    WORD USB_remain_Data = 0;
#endif

    TransferSize = (WORD)(USB_Data[6]<<8|USB_Data[7]);
//    USB_printData("TransferSize:%x", TransferSize);

#if ENABLE_USB_DEVICE_UPDATE_CODE
    USBStoreCnt += TransferSize;

    if( (SPIBuffer_counter+TransferSize) < FLASH_SECTOR_SIZE )
    {
        memcpy(SPIBuffer + SPIBuffer_counter, USB_Data+8, TransferSize);
        SPIBuffer_counter += TransferSize;
    }
    else
    {
        USB_remain_Data = FLASH_SECTOR_SIZE - SPIBuffer_counter;
        memcpy(SPIBuffer+SPIBuffer_counter, USB_Data + 8, USB_remain_Data);

       if(SPIWCnt >= u32DownloadOffset) //skip 1st image's sboot
            WINISP_FlashWriteTbl(TRUE, u32SPIOffset+SPIWCnt, SPIBuffer, FLASH_SECTOR_SIZE, 0);

        //calculate checksum
        for(i=u32SPIOffset + SPIWCnt; i < u32SPIOffset + SPIWCnt + FLASH_SECTOR_SIZE; i++)
            DataCheckSum += FlashReadAnyByte(i);

        SPIWCnt += FLASH_SECTOR_SIZE;
        SPIBuffer_counter = 0;
        memcpy(SPIBuffer, USB_Data + 8 + USB_remain_Data, TransferSize - USB_remain_Data);
        SPIBuffer_counter += (TransferSize - USB_remain_Data);
    }

    if(USBStoreCnt >= USBTransferSize ) //last sector
    {
        if(SPIWCnt >= u32DownloadOffset) //skip 1st image's sboot
            WINISP_FlashWriteTbl(TRUE, u32SPIOffset+SPIWCnt, SPIBuffer, SPIBuffer_counter, 0);

        //calculate checksum
        for(i=u32SPIOffset + SPIWCnt; i < u32SPIOffset + SPIWCnt + SPIBuffer_counter; i++)
            DataCheckSum += FlashReadAnyByte(i);

        SPIWCnt += SPIBuffer_counter;
    }

    //processing
    if((USBStoreCnt % ((HID_TRX_LENGTH-12)*10)) == 0)
    {
        USB_printMsg(".");
    }

    // write end
    if(USBStoreCnt >= USBTransferSize)
    {
        if( DataCheckSum == FileCheckSum)
        {
            USB_printMsg("Checksum Compare pass");
            CheckSum_Flag = 1;
        }
        else
        {
            USB_printData("Checksum Compare fail! DataCheckSum = 0x%x", DataCheckSum);
            USB_printMsg("Download code FAILED");
            CheckSum_Flag = 0;
        }

        USB_SendUpdateScuessCmd(CheckSum_Flag);
    }
#else
    otg_memcpy((U8*)(USBData+USBStoreCnt), USB_Data+8, TransferSize);
    USBStoreCnt += TransferSize;

    if(USBStoreCnt >= USBTransferSize)
    {
        for(i=0; i< USBTransferSize;i++)
            DataCheckSum += USBData[i];

       if((DataCheckSum & 0xFFFF) == FileCheckSum)
        {
            USB_printMsg("CheckSum match\r\n");
            CheckSum_Flag = 1;
        }
        else
        {
            USB_printData("CheckSum Compare Fail! DataCheckSum = 0x%x",DataCheckSum);
            CheckSum_Flag = 0;
        }

        USB_SendUpdateScuessCmd(CheckSum_Flag);

        APPSystem_USBDeviceCB((U8*)USBData, USBTransferSize, CheckSum_Flag);
        USBTransferSize = 0;

        #if ENABLE_RTE
            msAPI_OverDriveEnable( UserprefOverDriveSwitch );
        #endif
    }
#endif
}

#else //Non HID device

void USB_CMDHandler(void)
{
    BYTE USB_Command[16];

	USB_DRC_Interrupt(&usbDrvVar);
    MDrv_USB_CmdLoop((void*)(&usbDrvVar), USB_Command);

    if( USB_Command[0] == SCSI_VENDOR_XROM)
    {
        switch(USB_Command[1])
        {
            case XROM_USB_FILE_SIZE:
                USBTransferSize = (U32)(USB_Command[2]<<16|USB_Command[3]<<8|USB_Command[4]);
                USB_printData("USBTransferSize_H: 0x%x", USBTransferSize >> 16);
                USB_printData("USBTransferSize_L: 0x%x", USBTransferSize & 0xFFFF);

#if ENABLE_USB_DEVICE_UPDATE_CODE
                if(g_dwSpiDuelImageOffset < ENABLE_USB_HOST_DUAL_IMAGE_OFFSET)
                    u32SPIOffset = ENABLE_USB_HOST_DUAL_IMAGE_OFFSET;
                else
                {
                    u32SPIOffset = 0;
                    u32DownloadOffset = ENABLE_USB_HOST_DUAL_IMAGE_SBOOT;
                }

                mcuSetSpiMode(SPI_MODE_FR); // set FR mode to avoid continue switching mode, speed up program time
#else
                if(USBTransferSize > DOWNLOAD_BUFFER_LEN)
                {
                    USB_printMsg("[Warning!!] USBTransferSize > Buffer Size");
                }
#endif
                #if ENABLE_RTE
                    msAPI_OverDriveEnable(0); // USB & OD share the same space. Before USB F/W download, OD should be off first.
                #endif

                DataCheckSum = 0;
                FileCheckSum = 0;
                USBStoreCnt = 0;
                USBRxCnt = 0;

#if ENABLE_USB_DEVICE_UPDATE_CODE
                NonEraseSector = 0;
                SPIWCnt = 0;
#endif
            break;

    		case XROM_USB_REBOOT_SYS:

    	    break;

            case XROM_USB_FILE_CHECKSUM:
                FileCheckSum = (U16)(USB_Command[2]<<8|USB_Command[3]);
                USB_printData("FileCheckSum:0x%x", FileCheckSum);
            break;

            default:
             break;
        }
    }
}

void USB_DataHandler(void)
{
	DWORD i = 0;
    BYTE CheckSum_Flag;

#if !ENABLE_USB_DEVICE_UPDATE_CODE
    WORD TransferSize;
#endif

    USBRxCnt += MDrv_USB_Read((U8*)(USBRxBuf), USBTransferSize, &usbDrvVar); // 1 transfer max: 2048 Bytes

	if(USBRxCnt)
#if ENABLE_USB_DEVICE_UPDATE_CODE
    {
        // write to SPI
        if( USBRxCnt > SPIWCnt)
        {
            if(SPIWCnt >= u32DownloadOffset) //skip 1st image's sboot
            {
                WINISP_FlashWriteTbl(TRUE, u32SPIOffset+SPIWCnt, USBRxBuf, USBRxCnt-SPIWCnt, NonEraseSector);
            }

            //calculate checksum
            for(i=0x0; i<USBRxCnt - SPIWCnt;i++)
            {
                if((SPIWCnt+i)<USBTransferSize)
                    DataCheckSum += FlashReadAnyByte(u32SPIOffset+SPIWCnt+i);
            }

            SPIWCnt += (USBRxCnt-SPIWCnt);
            NonEraseSector = !NonEraseSector;

            //print each 64KB
            if((USBRxCnt & 0xFFFF) == 0x800)
            {
                USB_printMsg(".");
            }
        }

        // write end
        if(USBRxCnt >= USBTransferSize)
        {
            if( DataCheckSum == FileCheckSum)
            {
                USB_printMsg("Download code SUCCEED, Reboot System");
                CheckSum_Flag = 1;
            }
            else
            {
                USB_printData("CheckSum Compare Fail! DataCheckSum = 0x%x",DataCheckSum);
                USB_printMsg("Download code FAILED, Reboot System");
                CheckSum_Flag = 0;
            }

            USB_SendUpdateScuessCmd(CheckSum_Flag);

            USB_printMsg("Ready to Reboot System");
 //           USBC_REG_WRITE8(0x02, (USBC_REG_READ8(0x02)& ~(BIT0|BIT1))); //USB disable
            msWrite4Byte( REG_002C08, 1); 	//Reboot System
            while(1);
        }
    }
#else
    {
        TransferSize = USBRxCnt - USBStoreCnt;
        otg_memcpy((U8*)(USBData+USBStoreCnt), USBRxBuf, TransferSize);
        USBStoreCnt = USBRxCnt;

        if(USBRxCnt >= USBTransferSize)
        {
            for(i=0; i< USBTransferSize;i++)
                DataCheckSum += USBData[i];

           if((DataCheckSum & 0xFFFF) == FileCheckSum)
            {
                USB_printMsg("CheckSum match\r\n");
                CheckSum_Flag = 1;
            }
            else
            {
                USB_printData("CheckSum Compare Fail! DataCheckSum = 0x%x",DataCheckSum);
                CheckSum_Flag = 0;
            }

            USB_SendUpdateScuessCmd(CheckSum_Flag);

            APPSystem_USBDeviceCB((U8*)USBData, USBTransferSize, CheckSum_Flag);
            USBRxCnt = 0;
            USBTransferSize = 0;

            #if ENABLE_RTE
                msAPI_OverDriveEnable( UserprefOverDriveSwitch );
            #endif
        }
    }
#endif

}

void USB_SendUpdateScuessCmd(BYTE u8Scuess)
{
	WORD u16Retry;
    BYTE usb_magic[4] = {0xD0,0xD0,0xD0,0xD0}, USB_Command[16];;

    if(u8Scuess)
        MST_UpdateStatus = MST_UPDATE_STATUS_SUCCESS;
    else
        MST_UpdateStatus = MST_UPDATE_STATUS_FAIL;

    if(!u8Scuess)
        usb_magic[0] = 0xCC;

    MDrv_USB_Write(usb_magic, 4, &usbDrvVar);

    u16Retry = 0xFFFF;
    do
    {
        USB_DRC_Interrupt(&usbDrvVar);
        MDrv_USB_CmdLoop((void*)(&usbDrvVar), USB_Command);
        u16Retry--;
    }while (u16Retry && (scsiVendorCmdVar.ptrPPB2Buf[0] != 0));
}
#endif

void USB_Device_Disable(void)
{
    USB_REG_WRITE8(M_REG_POWER, (USB_REG_READ8(M_REG_POWER) & ~M_POWER_SOFTCONN));
}

void USB_Device_Enable(void)
{
#if ENABLE_USB_TYPEC
    extern BOOL drvmbx_send_BB_Disconnect_CMD(BYTE u8Enable);
    SetUSBDL();
    drvmbx_send_BB_Disconnect_CMD(1);
#endif

    USB_Device_Init();
}

#endif

#if !ENABLE_USB_DEVICE_UPDATE_CODE
void APPSystem_USBDeviceCB(BYTE* DataAddress, DWORD DataSize, BYTE CheckSum_flag) //DataAddress: USB Data address, DataSize: USB Data Size, CheckSum_flag:Compare USB Data Valid
{
    DataAddress = DataAddress;
    DataSize = DataSize;

    if(CheckSum_flag)
    {
#if 0
        DWORD i;
        for(i=0;i<0x100;i++)
        {
            if(i%16 == 0)
                printf("\r\n");
            printf("0x%02x ", *(DataAddress+i));
        }
        printf("\r\n");

        for(i=DataSize-0x100;i<DataSize;i++)
        {
            if(i%16 == 0)
                printf("\r\n");
            printf("0x%02x ", *(DataAddress+i));
        }
        printf("\r\n");
#endif
    }
    else
    {
        USB_printMsg("USB transfer data CheckSum Fail!");
    }
}
#endif

