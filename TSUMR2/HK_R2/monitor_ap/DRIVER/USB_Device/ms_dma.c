#include "ms_config.h"
#include "type.h"
#include "ms_msd_global.h"
#include "ms_dma.h"
#include "ms_usb.h"
#include "Globalvar.h"

extern void printMsg( char *str );
extern void printData( char *str, unsigned int value );

#ifdef DMA
void Set32BitsReg(unsigned long volatile* Reg, unsigned long val)
{
    *(Reg) = (U16)(val & 0xffff);
    *(Reg+1)=(U16)(val >> 16);
}

U32 Get32BitsReg(U32 volatile* Reg)
{
     U32 temp;
     temp = ((*(Reg)) & 0xffff) + ((*(Reg+1))<<16) ;
     return temp;
}

S8 Get_DMA_Channel(USB_VAR *gUSBStruct)
{
	S8 i, otg_bit;

	//cyg_semaphore_wait(&ChannelDmaSem);
	for(i = 0, otg_bit = 1; i <  MAX_DMA_CHANNEL; i++, otg_bit <<= 1)
	{
		if(gUSBStruct->free_dma_channels & otg_bit)
		{
			gUSBStruct->free_dma_channels &= ~otg_bit;
			//DBG_MSG( "Get Channel:%x ",i+1);
			//cyg_semaphore_post(&ChannelDmaSem);
			return i+1;
		}
	}
	//printf( "no channel");
	//cyg_semaphore_post(&ChannelDmaSem);
	return -1;
}

void Release_DMA_Channel(S8 channel,USB_VAR *gUSBStruct)
{
    // DBG_MSG( "release channel:%x \n",channel);
    //cyg_semaphore_wait(&ChannelDmaSem);
    gUSBStruct->free_dma_channels |= (1 << (channel - 1));
    //cyg_semaphore_post(&ChannelDmaSem);
}

extern U32 MsOS_VA2PA(U32 addr);
extern void Chip_Flush_Memory(void);
extern void  flush_cache (unsigned long addr, unsigned long len);
S8 USB_Set_DMA(S8 ep, U32 mode, U32 maxsize,USB_VAR *gUSBStruct)
{
    S8 ch;
    U32 count, index, csr2;
    U16 control;
    U32 address;

	address = gUSBStruct->otgUSB_EP[ep].transfer_buffer + gUSBStruct->otgUSB_EP[ep].BytesProcessed;
	count = gUSBStruct->otgUSB_EP[ep].BytesRequested - gUSBStruct->otgUSB_EP[ep].BytesProcessed;

	if(count < MIN_DMA_TRANSFER_BYTES)      //use FIFO transfer
	{
		//printf("return FAILURE\n");
		return -1;
	}

	if((ch = Get_DMA_Channel(gUSBStruct)) < 0)       /* no free channel */
	{
		//printf("Get DMA channel fail\n");
		return -1;
	}

	 /* for multiple Bulk packets, set Mode 1 */
	 if (count > gUSBStruct->otgUSB_EP[ep].MaxEPSize)
	 {
		mode |= DMA_MODE_ONE;
	 }
	 else
	 {
		if(mode & DMA_TX)
			count = MIN(gUSBStruct->otgUSB_EP[ep].MaxEPSize, count);
		else
			count = MIN(gUSBStruct->otgUSB_EP[ep].FifoRemain, count);
	 }

	/* prepare DMA control register */
	control = DMA_ENABLE_BIT | mode | (gUSBStruct->otgUSB_EP[ep].BltEP << DMA_ENDPOINT_SHIFT) | (DMA_BurstMode<<9);


	//miu0_bus_base_addr=address;
	//printf("address:0x%x\n",address);
	flush_cache((unsigned long)address,count);
	Chip_Flush_Memory();
	address=MsOS_VA2PA(address);

    Set32BitsReg((U32 volatile*)DMA_ADDR_REGISTER(ch), address);
    Set32BitsReg((U32 volatile*)DMA_COUNT_REGISTER(ch), count);
	//printf("set count:0x%x\n",count);
	//printf("address:0x%x\n",address);
	/* save and set index register */
	//index = USB_REG_READ8(M_REG_INDEX);
	//USB_REG_WRITE8(M_REG_INDEX, gUSBStruct->otgUSB_EP[ep].BltEP);

	/* program DRC registers */
	switch(mode & DMA_MODE_MASK)
	{
		case DMA_RX | DMA_MODE_ZERO:
			//printf( "eprx_mod0 = %x, count = %x, Request = %x, control = %x\n",ep,count,gUSBStruct->otgUSB_EP[ep].BytesRequested,control);
			csr2 = USB_REG_READ8(M_REG_RXCSR2);
			Enable_RX_EP_Interrupt(ep);
			USB_REG_WRITE8(M_REG_RXCSR2, (csr2 & ~RXCSR2_MODE1));
			break;

		case DMA_TX | DMA_MODE_ZERO:
			//printf( "eptx_mod0 = %x, count = %x, Request = %x, control = %x\n",ep,count,gUSBStruct->otgUSB_EP[ep].BytesRequested,control);
			Enable_TX_EP_Interrupt(ep);
			csr2 = USB_REG_READ8(M_REG_TXCSR2);
			USB_REG_WRITE8(M_REG_TXCSR2, (csr2 & ~TXCSR2_MODE1));
			break;

		case DMA_RX | DMA_MODE_ONE:
			USB_REG_WRITE8(0x126, 0);
			//printf( "ep = %x, count = %x, Request = %x, control = %x\n",ep,count,gUSBStruct->otgUSB_EP[ep].BytesRequested,control);
			csr2 = USB_REG_READ8(M_REG_RXCSR2);
			*(DMA_CNTL_REGISTER(ch)) = control;
			//printf("ch1:0x%x\n",ch);
			//printf("d_cntl_reg:0x%x\n",*(DMA_CNTL_REGISTER(1)));
			//printf("control:0x%x\n",control);
			Enable_RX_EP_Interrupt(ep);
			USB_REG_WRITE8(M_REG_RXCSR2, (csr2 | RXCSR2_MODE1 | 0x10));
			//USB_REG_WRITE8(M_REG_RXCSR1, 0);//enable RX
			if (1)//(gUSBStruct->u8USBDevMode == E_USB_MassStorage)
			{
				//USB_REG_WRITE16(M_REG_DMA_MODE_CTL, (USB_REG_READ16(M_REG_DMA_MODE_CTL)| M_Mode1_P_Enable));
				//USB_REG_WRITE16(M_REG_DMA_MODE_CTL, (USB_REG_READ16(M_REG_DMA_MODE_CTL)|(count/gUSBStruct->otgUSB_EP[ep].MaxEPSize))); //disable set_ok2rcv[15]&ECO4NAK_en[14],wayne added
				//USB_REG_WRITE16(M_REG_DMA_MODE_CTL, (USB_REG_READ16(M_REG_DMA_MODE_CTL)|M_Mode1_P_OK2Rcv)); //enable set_ok2rcv[15]&ECO4NAK_en[14],wayne added
				//printf("pack:0x%x\n",(count/gUSBStruct->otgUSB_EP[ep].MaxEPSize));
				//printf("ok2rcv\n");
				//udelay(150);
				USB_REG_WRITE8(M_REG_DMA_MODE_CTL, (USB_REG_READ8(M_REG_DMA_MODE_CTL)) | (count/gUSBStruct->otgUSB_EP[ep].MaxEPSize));
				USB_REG_WRITE8(M_REG_DMA_MODE_CTL+1, (USB_REG_READ8(M_REG_DMA_MODE_CTL+1)) | 0x20 | 0x80);
				//printf("set dma mode ctl:0x%x\n",USB_REG_READ16(M_REG_DMA_MODE_CTL));
				//printf("dma mode ctl:0x%x\n",USB_REG_READ16(M_REG_DMA_MODE_CTL));
			}

			/* restore previous index value */
			//USB_REG_WRITE8(M_REG_INDEX, index);
			//printf("rxcsr:0x%x\n",USB_REG_READ16(M_REG_RXCSR1));
			//printf("rxcount:0x%x\n",USB_REG_READ16(0x128*2));
			//USB_DelayUS(200);
			//printf("x\n");
			//while(1);
			return 1;

		case DMA_TX | DMA_MODE_ONE:
			//printf( "count = %x, Request = %x, Process = %x\n",count,gUSBStruct->otgUSB_EP[ep].BytesRequested,gUSBStruct->otgUSB_EP[ep].BytesProcessed);
			Enable_TX_EP_Interrupt(ep);
			csr2 = USB_REG_READ8(M_REG_TXCSR2);
			USB_REG_WRITE8(M_REG_TXCSR2, (csr2 | TXCSR2_MODE1));
	}

	/* restore previous index value */
	//USB_REG_WRITE8(M_REG_INDEX, index);

	*(DMA_CNTL_REGISTER(ch)) = control; // set DMA active flag
	return 1;
}

extern U32 MsOS_PA2KSEG0(U32 addr);

void USB_DMA_IRQ_Handler(U8 ch,USB_VAR *gUSBStruct)
{
	U8  index/*, endpoint, direction*/;
	U32 csr, mode, bytesleft, addr, bytesdone, control;
	S8  ep;
	U8  csr2;

	/* get DMA Mode, address and byte counts from DMA registers */
	control =*(DMA_CNTL_REGISTER(ch)) ;
	mode = *(DMA_CNTL_REGISTER(ch)) & 0xf;
	addr = Get32BitsReg((U32 volatile*)DMA_ADDR_REGISTER(ch));
	//printf("addr:0x%x\n",addr);
	//printf("MsOS_PA2KSEG0:0x%x\n",MsOS_PA2KSEG0(addr));
	//printf("MsOS_PA2KSEG1:0x%x\n",MsOS_PA2KSEG1(addr));
	bytesleft =Get32BitsReg((U32 volatile*)DMA_COUNT_REGISTER(ch));

	/* get endpoint, URB pointer */
	ep = (*(DMA_CNTL_REGISTER(ch)) & 0xf0) >> DMA_ENDPOINT_SHIFT;

	/* how many bytes were processed ? */
	addr=MsOS_PA2KSEG0(addr);

	bytesdone = addr - ((U32)(gUSBStruct->otgUSB_EP[ep].transfer_buffer + gUSBStruct->otgUSB_EP[ep].BytesProcessed));
	//printf("@@@@@@@@@@bytesdone:0x%x\n",bytesdone);

	gUSBStruct->otgUSB_EP[ep].BytesProcessed += bytesdone;
	/* save and set index register */
	index = USB_REG_READ8(M_REG_INDEX);
	USB_REG_WRITE8(M_REG_INDEX, gUSBStruct->otgUSB_EP[ep].BltEP);
	Release_DMA_Channel(ch,gUSBStruct);


	/* clean DMA setup in CSR  */
	if (mode & DMA_TX)
	{
		csr2 = USB_REG_READ8(M_REG_TXCSR2);
		USB_REG_WRITE8(M_REG_TXCSR2, (csr2 & ~TXCSR2_MODE1));
	}
	else
	{
		csr2 = USB_REG_READ8(M_REG_RXCSR2);
		USB_REG_WRITE8(M_REG_RXCSR2, (csr2 & ~RXCSR2_MODE1));
	}

	/* Bus Error */
	if (control & DMA_BUSERROR_BIT)
	{
		if (mode & DMA_TX)
			gUSBStruct->otgDMATXIntStatus=USB_ST_STALL;
		else
			gUSBStruct->otgDMARXIntStatus=USB_ST_STALL;
		//printf("stall\n");

		return;
	}

	if (mode & DMA_TX)
	{
		if (gUSBStruct->otgUSB_EP[ep].BytesProcessed == gUSBStruct->otgUSB_EP[ep].BytesRequested)
		{
			if (gUSBStruct->otgUSB_EP[ep].BytesProcessed % gUSBStruct->otgUSB_EP[ep].MaxEPSize) /* short packet */
			{
				gUSBStruct->otgUSB_EP[ep].LastPacket = 1; /* need to set TXPKTRDY manually */
			}
			else  /* the last packet size is equal to MaxEPSize */
			{
				if (mode & (DMA_MODE_ONE))
				{
					gUSBStruct->otgUSB_EP[ep].LastPacket = 1;
					gUSBStruct->otgDMATXIntStatus = USB_ST_NOERROR;
					//printf( "DMA TXONE\n");
					return;
				}
				else
				{
					if (usb_pipebulk(gUSBStruct->otgDataPhaseDir))
						gUSBStruct->otgUSB_EP[ep].LastPacket = 1;
					//printf( "DMA TXONE1\n");
				}
			}
		}
	}
	else
	{
		Enable_RX_EP_Interrupt(ep);
		gUSBStruct->otgUSB_EP[ep].FifoRemain = bytesleft;
		if (gUSBStruct->otgUSB_EP[ep].FifoRemain == 0)
		{
			if (gUSBStruct->otgUSB_EP[ep].BytesProcessed % gUSBStruct->otgUSB_EP[ep].MaxEPSize) /* short packet */
			{
				gUSBStruct->otgUSB_EP[ep].LastPacket = 1;
			}
			else	/* the last packet size is equal to MaxEPSize */
			{
				if (mode & DMA_MODE_ONE)
				{
					gUSBStruct->otgUSB_EP[ep].LastPacket = 1;
					gUSBStruct->otgDMARXIntStatus=USB_ST_NOERROR;
					//printf( "DMA RXONE1\n");
					return;
				}
			}
		}
	}

	/*  for short packet, CPU needs to handle TXPKTRDY/RXPKTRDY bit  */
	if (mode & DMA_TX)
	{
		USB_REG_WRITE8(M_REG_TXCSR1, M_TXCSR1_TXPKTRDY);
		gUSBStruct->otgDMATXIntStatus=USB_ST_NOERROR;  //wayne added
		//printf( "DMA TXONE2\n");
	}
	else
	{
		csr = USB_REG_READ8(M_REG_RXCSR1);
		USB_REG_WRITE8(M_REG_RXCSR1, (csr & ~M_RXCSR1_RXPKTRDY));
	}

	/* restore the index */
	USB_REG_WRITE8(M_REG_INDEX, index);

	if (mode & DMA_TX)
	{
		//printf( "DMA TXONE3\n");
		return;
	}
	else
	{
		if (((!gUSBStruct->otgUSB_EP[ep].FifoRemain) && (gUSBStruct->otgUSB_EP[ep].LastPacket)) || (gUSBStruct->otgUSB_EP[ep].BytesProcessed >= gUSBStruct->otgUSB_EP[ep].BytesRequested))
		{
			gUSBStruct->otgDMARXIntStatus=USB_ST_NOERROR;
		}
	}
}

void Control_EP_Interrupt(S8 ep, U32 mode,USB_VAR *gUSBStruct)
{
	U8 reg, current, otg_bit;
	U8 endpoint;

	endpoint = gUSBStruct->otgUSB_EP[ep].BltEP;

	if(mode & EP_IRQ_TX)
		reg = (endpoint < 8) ? M_REG_INTRTX1E : M_REG_INTRTX2E;
	else
		reg = (endpoint < 8) ? M_REG_INTRRX1E : M_REG_INTRRX2E;

	current = USB_REG_READ8(reg);

	otg_bit = 1 << (endpoint % 8);

	if(mode & EP_IRQ_ENABLE)
		USB_REG_WRITE8(reg, (current | otg_bit));
	else
		USB_REG_WRITE8(reg, (current & ~otg_bit));
}
#endif
