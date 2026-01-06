/*************************************************************************
 * Copyright (c) 2016 MStar Semiconductor, Inc.
 * All rights reserved.
 *
 * Driver code for using MSPI to access SPI flash.
 *
 * Note: MSPI register settings for flash may conflict with MSPI
 *       for local dimming.
 *************************************************************************/
#define _MSPI_FLASH_C_
#include "types.h"
#include "Global.h"
#include "ms_reg.h"
#include "Debug.h"
#include "Common.h"
#include "board.h"

#include "Ms_rwreg.h"
#include "mspi_flash.h"
#include "MsOS.h"

#if ENABLE_MSPI_FLASH_ACCESS
#ifndef	MSPI_FLASH_DEBUG
#define MSPI_FLASH_DEBUG	0
#endif

#if (ENABLE_MSTV_UART_DEBUG && MSPI_FLASH_DEBUG)
#define MSPI_Flash_Print(str, value)					printData(str, value)
#else
#define MSPI_Flash_Print(str, value)
#endif

/******************************************************************************
 * MSPI flash registers
 *****************************************************************************/
#define MSPI_REG_BK(_mspi_)							(0x101700+(_mspi_<<8))
// registers related to DC timing
#define MSPI_REG_TR_STR_TIME(_mspi_,val) 			(msWriteByte(MSPI_REG_BK(_mspi_)+0x94,val))
#define MSPI_REG_TR_END_TIME(_mspi_,val)			(msWriteByte(MSPI_REG_BK(_mspi_)+0x95,val))
#define MSPI_REG_B_TO_B_TIME(_mspi_,val)			(msWriteByte(MSPI_REG_BK(_mspi_)+0x96,val))
#define MSPI_REG_W_TO_R_TIME(_mspi_,val)			(msWriteByte(MSPI_REG_BK(_mspi_)+0x97,val))
// registers related to R/W buffer and frame length
#define MSPI_REG_WD_BUFF(_mspi_, _addr_ ,val)		(msWriteByte(MSPI_REG_BK(_mspi_)+0x80+_addr_,val))
#define MSPI_REG_RD_BUFF(_mspi_, _addr_ )			(msReadByte(MSPI_REG_BK(_mspi_)+0x88+_addr_))
#define MSPI_REG_WBF_SIZE(_mspi_,val)				(msWriteByte(MSPI_REG_BK(_mspi_)+0x90,val))
#define MSPI_REG_RBF_SIZE(_mspi_,val)				(msWriteByte(MSPI_REG_BK(_mspi_)+0x91,val))
// registers related to clock and phase control
#define MSPI_REG_CTRL(_mspi_,val)					(msWriteByte(MSPI_REG_BK(_mspi_)+0x92,val))//(msRegs[MSPI_REG_BK(_mspi_)+0x92])
#define MSPI_REG_CLOCK_RATE(_mspi_,val)			(msWriteByte(MSPI_REG_BK(_mspi_)+0x93,val))//(msRegs[MSPI_REG_BK(_mspi_)+0x93])
// registers of chip select, trigger and status
#define MSPI_REG_TRIGGER(_mspi_,val)				(msWriteByte(MSPI_REG_BK(_mspi_)+0xB4,val))
#define MSPI_REG_STATUS(_mspi_)					(msReadByte(MSPI_REG_BK(_mspi_)+0xB6))//(msRegs[MSPI_REG_BK(_mspi_)+0xB6])
#define MSPI_REG_CLR_STATUS(_mspi_,val)			(msWriteByte(MSPI_REG_BK(_mspi_)+0xB8,val))//(msRegs[MSPI_REG_BK(_mspi_)+0xB8])
#define MSPI_REG_CHIP_SELECT(_mspi_,val)			(msWriteByte(MSPI_REG_BK(_mspi_)+0xBE,val))
// macros for setting R/W bit lengths of each buffer
#define MSPI_SET_WB0_BIT_LEN(_mspi_, _val_)		do{ msWriteByteMask((MSPI_REG_BK(_mspi_)+0x98), (_val_-1), BIT2|BIT1|BIT0); }while(0)
#define MSPI_SET_WB1_BIT_LEN(_mspi_, _val_)		do{ msWriteByteMask((MSPI_REG_BK(_mspi_)+0x98), (_val_-1)<<3, BIT5|BIT4|BIT3); }while(0)
#define MSPI_SET_WB2_BIT_LEN(_mspi_, _val_) 		do{ msWrite2ByteMask((MSPI_REG_BK(_mspi_)+0x98), (_val_-1)<<6, BIT8|BIT7|BIT6); }while(0)
#define MSPI_SET_WB3_BIT_LEN(_mspi_, _val_)		do{ msWriteByteMask((MSPI_REG_BK(_mspi_)+0x99), (_val_-1)<<1, BIT3|BIT2|BIT1); }while(0)
#define MSPI_SET_WB4_BIT_LEN(_mspi_, _val_)		do{ msWriteByteMask((MSPI_REG_BK(_mspi_)+0x9A), (_val_-1), BIT2|BIT1|BIT0); }while(0)
#define MSPI_SET_WB5_BIT_LEN(_mspi_, _val_)		do{ msWriteByteMask((MSPI_REG_BK(_mspi_)+0x9A), (_val_-1)<<3, BIT5|BIT4|BIT3); }while(0)
#define MSPI_SET_WB6_BIT_LEN(_mspi_, _val_) 		do{ msWrite2ByteMask((MSPI_REG_BK(_mspi_)+0x9A), (_val_-1)<<6, BIT8|BIT7|BIT6); }while(0)
#define MSPI_SET_WB7_BIT_LEN(_mspi_, _val_)		do{ msWriteByteMask((MSPI_REG_BK(_mspi_)+0x9B), (_val_-1)<<1, BIT3|BIT2|BIT1); }while(0)
#define MSPI_SET_RB0_BIT_LEN(_mspi_, _val_)			do{ msWriteByteMask((MSPI_REG_BK(_mspi_)+0x9C), (_val_-1), BIT2|BIT1|BIT0); }while(0)
#define MSPI_SET_RB1_BIT_LEN(_mspi_, _val_)			do{ msWriteByteMask((MSPI_REG_BK(_mspi_)+0x9C), (_val_-1)<<3, BIT5|BIT4|BIT3); }while(0)
#define MSPI_SET_RB2_BIT_LEN(_mspi_, _val_) 		do{ msWrite2ByteMask((MSPI_REG_BK(_mspi_)+0x9C), (_val_-1)<<6, BIT8|BIT7|BIT6); }while(0)
#define MSPI_SET_RB3_BIT_LEN(_mspi_, _val_)			do{ msWriteByteMask((MSPI_REG_BK(_mspi_)+0x9D), (_val_-1)<<1, BIT3|BIT2|BIT1); }while(0)
#define MSPI_SET_RB4_BIT_LEN(_mspi_, _val_)			do{ msWriteByteMask((MSPI_REG_BK(_mspi_)+0x9E), (_val_-1), BIT2|BIT1|BIT0); }while(0)
#define MSPI_SET_RB5_BIT_LEN(_mspi_, _val_)			do{ msWriteByteMask((MSPI_REG_BK(_mspi_)+0x9E), (_val_-1)<<3, BIT5|BIT4|BIT3); }while(0)
#define MSPI_SET_RB6_BIT_LEN(_mspi_, _val_) 		do{ msWrite2ByteMask((MSPI_REG_BK(_mspi_)+0x9E), (_val_-1)<<6, BIT8|BIT7|BIT6); }while(0)
#define MSPI_SET_RB7_BIT_LEN(_mspi_, _val_)			do{ msWriteByteMask((MSPI_REG_BK(_mspi_)+0x9F), (_val_-1)<<1, BIT3|BIT2|BIT1); }while(0)
#if 0
//-------------------------------------------------------------------------------------------------
/// Get current system time in timer ticks
/// @return system time in timer ticks
//-------------------------------------------------------------------------------------------------
DWORD MAsm_GetSystemTime (void)
{
    return gsystem_time_ms;
}
//-------------------------------------------------------------------------------------------------
/// Get current system time in ms
/// @return system time in ms
//-------------------------------------------------------------------------------------------------
DWORD MsOS_GetSystemTime (void) //   1ms/systime
{
    DWORD xdata systime;
    systime=MAsm_GetSystemTime ();
    //printf("[%s][%d] %s is not supported\n", __FUNCTION__, __LINE__, __FUNCTION__);
    return systime;
}
//-------------------------------------------------------------------------------------------------
///[OBSOLETE]
/// Time difference between current time and task time
/// @return system time diff in ms
//-------------------------------------------------------------------------------------------------
DWORD MsOS_Timer_DiffTimeFromNow(DWORD u32TaskTimer) //unit = ms
{
    return (MsOS_GetSystemTime() - u32TaskTimer);
}
#endif
/******************************************************************************
 * MSPI flash hardware abstract layer functions
 *****************************************************************************/
// Config MSPI pad as full/half MSPI
void MHal_MSPI_Flash_Pad_Config(EN_MSPI mspi, BYTE val)
{
	if (mspi == MSPI0)
	{
		//1: Full MSPI on GPIO52(MSPI_DI), GPIO53(MSPI_CK), GPIO54(MSPI_DO), GPIO55(MSPI_CZ)
		msWriteByteMask(REG_00041B, val<<4, BIT5|BIT4);
	}
	else
	{
		MSPI_Flash_Print(": No such MSPI engine\n",0);
		return;
	}
	return;
}
// Set the bit length of all R/W buffers
void MHal_MSPI_Flash_Set_Buffer_Bit_Length(EN_MSPI mspi, BYTE val)
{
	MSPI_SET_WB0_BIT_LEN(mspi, val);
	MSPI_SET_WB1_BIT_LEN(mspi, val);
	MSPI_SET_WB2_BIT_LEN(mspi, val);
	MSPI_SET_WB3_BIT_LEN(mspi, val);
	MSPI_SET_WB4_BIT_LEN(mspi, val);
	MSPI_SET_WB5_BIT_LEN(mspi, val);
	MSPI_SET_WB6_BIT_LEN(mspi, val);
	MSPI_SET_WB7_BIT_LEN(mspi, val);
	MSPI_SET_RB0_BIT_LEN(mspi, val);
	MSPI_SET_RB1_BIT_LEN(mspi, val);
	MSPI_SET_RB2_BIT_LEN(mspi, val);
	MSPI_SET_RB3_BIT_LEN(mspi, val);
	MSPI_SET_RB4_BIT_LEN(mspi, val);
	MSPI_SET_RB5_BIT_LEN(mspi, val);
	MSPI_SET_RB6_BIT_LEN(mspi, val);
	MSPI_SET_RB7_BIT_LEN(mspi, val);
}
// Set MSPI clock gen
void MHal_MSPI_Flash_Set_Clock_Gen(EN_MSPI mspi, EN_MSPI_CLOCK_GEN ckg)
{
	if (mspi == MSPI0)
	{
		msWriteByteMask(REG_120F0E, ckg<<2, BIT4|BIT3|BIT2);
		msWriteByteMask(REG_120F0E, 0x00, BIT1|BIT0);

        if(ckg == MSPI_CKG_XTAL)
		    msWriteByteMask(REG_120F0E, 0x00, BIT5);    // clk_spi_m_p2 sel -> 0: xtal, 1: P1
		else
            msWriteByteMask(REG_120F0E, BIT5, BIT5);

		msWriteByteMask(REG_120F0F, 0x00, 0x0F);		// div = 1
	}
	else
	{
		MSPI_Flash_Print("MHal_MSPI_Flash_Set_Clock_Gen: No such MSPI engine\n", 0);
		return;
	}
	return;
}
// Check if the transfer is completed.
BOOL MHal_MSPI_Flash_Wait_Done_Flag(EN_MSPI mspi)
{
    WORD  u16Retry = 0xFFFF;
    while(!(MSPI_REG_STATUS(mspi)&BIT0) && u16Retry--)
    Delay1ms(1);
    if (MSPI_REG_STATUS(mspi)&BIT0)						// trandfer is completed
    {
        MSPI_REG_CLR_STATUS(mspi, 0x01) ; 				// clear flag or interrupt
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
/******************************************************************************
 * MSPI flash driver functions
 *****************************************************************************/
///////////////////////////////////////////////////////////////////////////////
// <Description> Initial MSPI engine.
// <Return> None
///////////////////////////////////////////////////////////////////////////////
void MDrv_MSPI_Flash_Init(EN_MSPI mspi, BYTE ctrl, EN_MSPI_CLOCK_DIV clk_div, EN_MSPI_TR_STR_DELAY tstr, EN_MSPI_TR_END_DELAY tend, EN_MSPI_B_TO_B_DELAY tb, EN_MSPI_W_TO_R_DELAY trw)
{
	// Disable ldm_dma
    //msWriteByteMask(0x101000+0x80*mspi, 0x00, 0x83); // menuload trigger src = one shot
    //msWriteByteMask(0x10100A+0x80*mspi, BIT0, BIT0); // menuload abort one shot
    //msWriteByteMask(0x10100C+0x80*mspi, 0x00, BIT0); // menuload disable one shot

    MHal_MSPI_Flash_Pad_Config(mspi, 1);               // pad config, use full MSPI as default
    MHal_MSPI_Flash_Set_Buffer_Bit_Length(mspi, 8);    // assume the lengths of all R/W buffers are 8-bits.
    MHal_MSPI_Flash_Set_Clock_Gen(mspi, MSPI_CKG_54M); // max pad spi clk 54M / 2 = 27M

    MSPI_REG_CLOCK_RATE(mspi, clk_div);                // mspi internal clock divider
    MSPI_REG_TR_STR_TIME(mspi, tstr);
    MSPI_REG_TR_END_TIME(mspi, tend);
    MSPI_REG_B_TO_B_TIME(mspi, tb);
    MSPI_REG_W_TO_R_TIME(mspi, trw);

    MSPI_REG_CTRL(mspi, ctrl);
    MSPI_REG_CLR_STATUS(mspi, 0x01);                   // clear done flag or interrupt

	return;
}

void MDrv_MSPI_Flash_Write_Read_Once(EN_MSPI mspi, EN_MSPI_SLAVE slave, ST_MSPI_TRANSFER_DATA* pstWriteData, ST_MSPI_TRANSFER_DATA* pstReadData)
{
    BYTE  xdata u8TempCnt = 0;

    if ((pstWriteData->u8DataByteLength > MSPI_FLASH_WBF_MAX_SIZE) || (pstReadData->u8DataByteLength > MSPI_FLASH_RBF_MAX_SIZE))
    {
        MSPI_Flash_Print("MDrv_MSPI_Flash_Write_Read_Once: Write/Read Buffer Length %u/%u is invalid\n", 0);
        return;
    }

    if ((pstWriteData->u8DataByteLength == 0) && (pstReadData->u8DataByteLength == 0))
    {
        MSPI_Flash_Print("MDrv_MSPI_Flash_Write_Read_Once: No Data to transfer\n", 0);
        return;
    }

    // setup R/W buffer sizes
    MSPI_REG_WBF_SIZE(mspi, pstWriteData->u8DataByteLength); //0x101790
    MSPI_REG_RBF_SIZE(mspi, pstReadData->u8DataByteLength);  //0x101791

    for (u8TempCnt = 0; u8TempCnt < pstWriteData->u8DataByteLength; u8TempCnt++)
    {
    	MSPI_REG_WD_BUFF(mspi, u8TempCnt, pstWriteData->au8DataBuffer[u8TempCnt]); //0x101780
    }
    MSPI_REG_CHIP_SELECT(mspi,~(1<<slave));	//select slave device, set cz to low //0x1017BE
    MSPI_REG_TRIGGER(mspi,0x01); //trigger MSPI operation //0x1017B4

    if (MHal_MSPI_Flash_Wait_Done_Flag(mspi) != TRUE)
    {
        MSPI_Flash_Print("MDrv_MSPI_Flash_Write_Read_Once: Transfer timeout\n", 0);
        MSPI_REG_CHIP_SELECT(mspi, 0xFF); //disable cz
        return;
    }

    MSPI_REG_CHIP_SELECT(mspi, 0xFF); //disable cz

    // store data
    for (u8TempCnt=0; u8TempCnt<pstReadData->u8DataByteLength; u8TempCnt++)
    {
        pstReadData->au8DataBuffer[u8TempCnt] = MSPI_REG_RD_BUFF(mspi, u8TempCnt);
    }
}

BYTE MDrv_MSPI_Flash_Read_Status_Register(EN_MSPI mspi, EN_MSPI_SLAVE slave)
{
    ST_MSPI_TRANSFER_DATA stMSPIWriteData, stMSPIReadData;

    stMSPIWriteData.u8DataByteLength 	= 1;
    stMSPIWriteData.au8DataBuffer[0] 	= SPI_CMD_RDSR;
    stMSPIReadData.u8DataByteLength		= 1;
    MDrv_MSPI_Flash_Write_Read_Once(mspi, slave, &stMSPIWriteData, &stMSPIReadData);

    return stMSPIReadData.au8DataBuffer[0];
}
BYTE MDrv_MSPI_Flash_Read_Status_Register2(EN_MSPI mspi, EN_MSPI_SLAVE slave)
{
    ST_MSPI_TRANSFER_DATA stMSPIWriteData, stMSPIReadData;

    stMSPIWriteData.u8DataByteLength	= 1;
    stMSPIWriteData.au8DataBuffer[0] 	= SPI_CMD_RDSR2;
    stMSPIReadData.u8DataByteLength		= 1;
    MDrv_MSPI_Flash_Write_Read_Once(mspi, slave, &stMSPIWriteData, &stMSPIReadData);

    return stMSPIReadData.au8DataBuffer[0];
}
void MDrv_MSPI_Flash_Write_Status_Register(EN_MSPI mspi, EN_MSPI_SLAVE slave, BYTE u8Status)
{
    ST_MSPI_TRANSFER_DATA stMSPIWriteData, stMSPIReadData;

    stMSPIWriteData.u8DataByteLength	= 2;
    stMSPIWriteData.au8DataBuffer[0] 	= SPI_CMD_WRSR; //0x01
    stMSPIWriteData.au8DataBuffer[1] 	= u8Status;
    stMSPIReadData.u8DataByteLength = 0;
    MDrv_MSPI_Flash_Write_Read_Once(mspi, slave, &stMSPIWriteData, &stMSPIReadData);
}
void MDrv_MSPI_Flash_Enable_Write_Status_Register(EN_MSPI mspi, EN_MSPI_SLAVE slave)
{
    ST_MSPI_TRANSFER_DATA stMSPIWriteData, stMSPIReadData;

    stMSPIWriteData.u8DataByteLength 	= 1;
    stMSPIWriteData.au8DataBuffer[0] 	= 0x50;
    stMSPIReadData.u8DataByteLength 	= 0;
    MDrv_MSPI_Flash_Write_Read_Once(mspi, slave, &stMSPIWriteData, &stMSPIReadData);
}
void MDrv_MSPI_Flash_Write_Enable(EN_MSPI mspi, EN_MSPI_SLAVE slave)
{
    ST_MSPI_TRANSFER_DATA stMSPIWriteData, stMSPIReadData;

    stMSPIWriteData.u8DataByteLength 	= 1;
    stMSPIWriteData.au8DataBuffer[0] 	= SPI_CMD_WREN;  //0x06
    stMSPIReadData.u8DataByteLength 	= 0;
    MDrv_MSPI_Flash_Write_Read_Once(mspi, slave, &stMSPIWriteData, &stMSPIReadData);
}
void MDrv_MSPI_Flash_Write_Disable(EN_MSPI mspi, EN_MSPI_SLAVE slave)
{
    ST_MSPI_TRANSFER_DATA stMSPIWriteData, stMSPIReadData;

    stMSPIWriteData.u8DataByteLength 	= 1;
    stMSPIWriteData.au8DataBuffer[0] 	= SPI_CMD_WRDI; //0x04
    stMSPIReadData.u8DataByteLength 	= 0;
    MDrv_MSPI_Flash_Write_Read_Once(mspi, slave, &stMSPIWriteData, &stMSPIReadData);
}

DWORD MDrv_MSPI_Flash_Read_ID(EN_MSPI mspi, EN_MSPI_SLAVE slave)
{
    ST_MSPI_TRANSFER_DATA stMSPIWriteData, stMSPIReadData;

    stMSPIWriteData.u8DataByteLength	= 1;
    stMSPIWriteData.au8DataBuffer[0] 	= SPI_CMD_RDID;
    stMSPIReadData.u8DataByteLength		= 3;
    MDrv_MSPI_Flash_Write_Read_Once(mspi, slave, &stMSPIWriteData, &stMSPIReadData);

    // return Value: [23:16] Manufacture, [15:8] Memory Type, [7:0] Capacity
    return ((((DWORD)stMSPIReadData.au8DataBuffer[0])<<16) | (((DWORD)stMSPIReadData.au8DataBuffer[1])<<8)| stMSPIReadData.au8DataBuffer[2]);
}

void MDrv_MSPI_Flash_Sector_Erase(EN_MSPI mspi, EN_MSPI_SLAVE slave, DWORD u32FlashStartAddress)
{
    ST_MSPI_TRANSFER_DATA stMSPIWriteData, stMSPIReadData;

    stMSPIWriteData.u8DataByteLength = 4;
    stMSPIWriteData.au8DataBuffer[0] = SPI_CMD_SE;
    stMSPIWriteData.au8DataBuffer[1] = (BYTE)((u32FlashStartAddress>>16)&0xFF);
    stMSPIWriteData.au8DataBuffer[2] = (BYTE)((u32FlashStartAddress>>8)&0xFF);
    stMSPIWriteData.au8DataBuffer[3] = (BYTE)(u32FlashStartAddress&0xFF);
    stMSPIReadData.u8DataByteLength = 0;
    MDrv_MSPI_Flash_Write_Read_Once(mspi, slave, &stMSPIWriteData, &stMSPIReadData);
}

void MDrv_MSPI_Flash_32KB_Block_Erase(EN_MSPI mspi, EN_MSPI_SLAVE slave, DWORD u32FlashStartAddress)
{
    ST_MSPI_TRANSFER_DATA stMSPIWriteData, stMSPIReadData;

    stMSPIWriteData.u8DataByteLength = 4;
    stMSPIWriteData.au8DataBuffer[0] = SPI_CMD_32BE;
    stMSPIWriteData.au8DataBuffer[1] = (BYTE)((u32FlashStartAddress>>16)&0xFF);
    stMSPIWriteData.au8DataBuffer[2] = (BYTE)((u32FlashStartAddress>>8)&0xFF);
    stMSPIWriteData.au8DataBuffer[3] = (BYTE)(u32FlashStartAddress&0xFF);
    stMSPIReadData.u8DataByteLength = 0;
    MDrv_MSPI_Flash_Write_Read_Once(mspi, slave, &stMSPIWriteData, &stMSPIReadData);
}

void MDrv_MSPI_Flash_64KB_Block_Erase(EN_MSPI mspi, EN_MSPI_SLAVE slave, DWORD u32FlashStartAddress)
{
    ST_MSPI_TRANSFER_DATA stMSPIWriteData, stMSPIReadData;

    stMSPIWriteData.u8DataByteLength = 4;
    stMSPIWriteData.au8DataBuffer[0] = SPI_CMD_64BE;
    stMSPIWriteData.au8DataBuffer[1] = (BYTE)((u32FlashStartAddress>>16)&0xFF);
    stMSPIWriteData.au8DataBuffer[2] = (BYTE)((u32FlashStartAddress>>8)&0xFF);
    stMSPIWriteData.au8DataBuffer[3] = (BYTE)(u32FlashStartAddress&0xFF);
    stMSPIReadData.u8DataByteLength = 0;
    MDrv_MSPI_Flash_Write_Read_Once(mspi, slave, &stMSPIWriteData, &stMSPIReadData);
}
void MDrv_MSPI_Flash_Chip_Erase(EN_MSPI mspi, EN_MSPI_SLAVE slave)
{
    ST_MSPI_TRANSFER_DATA stMSPIWriteData, stMSPIReadData;

    stMSPIWriteData.u8DataByteLength = 1;
    stMSPIWriteData.au8DataBuffer[0] = SPI_CMD_CE;
    stMSPIReadData.u8DataByteLength = 0;
    MDrv_MSPI_Flash_Write_Read_Once(mspi, slave, &stMSPIWriteData, &stMSPIReadData);
}
BOOL MDrv_MSPI_Flash_Wait_Busy_Flag(EN_MSPI mspi, EN_MSPI_SLAVE slave, DWORD u32TimeOut)
{
    DWORD  xdata u32CurrentTime = 0;

    u32CurrentTime = MsOS_GetSystemTime();
    while(1)
    {
    	// polling busy flag
        if ((MDrv_MSPI_Flash_Read_Status_Register(mspi, slave)&BIT0) == 0)
            return TRUE;

		// time out
        if (MsOS_Timer_DiffTimeFromNow(u32CurrentTime) > u32TimeOut)
        {
            MSPI_Flash_Print("MDrv_MSPI_Flash_Wait_Busy_Flag: Flash Busy Time Out %u!\n", u32TimeOut);
            return FALSE;
        }
    }
}
BOOL MDrv_MSPI_Flash_Erase_All(EN_MSPI mspi, EN_MSPI_SLAVE slave)
{
    if (!MDrv_MSPI_Flash_Wait_Busy_Flag(mspi, slave, MSPI_POLLING_BUSY_FLAG_TIMEOUT))
        return FALSE;

    MDrv_MSPI_Flash_Write_Enable(mspi, slave);
    MDrv_MSPI_Flash_Chip_Erase(mspi, slave);

    if (!MDrv_MSPI_Flash_Wait_Busy_Flag(mspi, slave, MSPI_POLLING_TIMEOUT_ERASE))
        return FALSE;

    return TRUE;
}
BOOL MDrv_MSPI_Flash_Page_Program(EN_MSPI mspi, EN_MSPI_SLAVE slave, DWORD u32FlashAddr, DWORD u32WriteByteLength, const BYTE *pu8Buff)
{
	BYTE  xdata u8TempCnt = 0;

        MSPI_Flash_Print(" MDrv_MSPI_Flash_Page_Program ST \n\r", u32WriteByteLength);

    if (u32WriteByteLength == 0)
    {
        MSPI_Flash_Print(" MDrv_MSPI_Flash_Page_Program: WriteByteLength is 0\n", 0);
        return FALSE;
    }

    if ((u32FlashAddr>>8) != ((u32FlashAddr+u32WriteByteLength-1)>>8))
    {
        MSPI_Flash_Print("MDrv_MSPI_Flash_Page_Program: Write Range is not in the same page\r\n", 0);
        return FALSE;
    }

    // send page program command and address (flash address is big endian)
    MSPI_REG_WBF_SIZE(mspi, 4);
    MSPI_REG_RBF_SIZE(mspi, 0);
    MSPI_REG_WD_BUFF(mspi, 0, SPI_CMD_PP);
    MSPI_REG_WD_BUFF(mspi, 1, (BYTE)((u32FlashAddr>>16)&0xFF));
    MSPI_REG_WD_BUFF(mspi, 2, (BYTE)((u32FlashAddr>>8)&0xFF));
    MSPI_REG_WD_BUFF(mspi, 3, (BYTE)(u32FlashAddr&0xFF));
    MSPI_REG_CHIP_SELECT(mspi, ~(1<<slave)); // select slave device, set cz to low
    MSPI_REG_TRIGGER(mspi,0x01); // trigger MSPI operation

    if (MHal_MSPI_Flash_Wait_Done_Flag(mspi) != TRUE)
    {
    	  MSPI_Flash_Print("111 MHal_MSPI_Flash_Wait_Done_Flag: Transfer timeout\r\n", 0);
    	  MSPI_REG_CHIP_SELECT(mspi, 0xFF); // disable cz
        return FALSE;
    }

	// send out (u32WriteByteLength % MSPI_FLASH_WBF_MAX_SIZE) bytes data
	if ((u32WriteByteLength%MSPI_FLASH_WBF_MAX_SIZE) != 0)
	{
	    MSPI_REG_WBF_SIZE(mspi, u32WriteByteLength & (MSPI_FLASH_WBF_MAX_SIZE-1));
	    for (u8TempCnt = 0; u8TempCnt < (u32WriteByteLength&(MSPI_FLASH_WBF_MAX_SIZE-1)); u8TempCnt++)
	    {
	    	MSPI_REG_WD_BUFF(mspi, u8TempCnt, *pu8Buff);
	    	pu8Buff++;
	    }
	    MSPI_REG_TRIGGER(mspi, 0x01); // trigger MSPI operation
	    if (MHal_MSPI_Flash_Wait_Done_Flag(mspi) != TRUE)
	    {
    	  	MSPI_Flash_Print("222 MHal_MSPI_Flash_Wait_Done_Flag: Transfer timeout\r\n", 0);
	    	MSPI_REG_CHIP_SELECT(mspi, 0xFF); // disable cz
	        return FALSE;
	    }
	    u32WriteByteLength -= u32WriteByteLength&(MSPI_FLASH_WBF_MAX_SIZE-1); //not -8??Scar
	}

    // send out 8 bytes data each time
    MSPI_REG_WBF_SIZE(mspi, MSPI_FLASH_WBF_MAX_SIZE);
    while (u32WriteByteLength > 0)
    {
    	for (u8TempCnt = 0; u8TempCnt < MSPI_FLASH_WBF_MAX_SIZE; u8TempCnt++)
    	{
    		MSPI_REG_WD_BUFF(mspi, u8TempCnt, *pu8Buff);
    		pu8Buff++;
        	u32WriteByteLength--;
    	}
	    MSPI_REG_TRIGGER(mspi, 0x01); // trigger MSPI operation
    	if (MHal_MSPI_Flash_Wait_Done_Flag(mspi) != TRUE)
    	{
    		MSPI_Flash_Print("333  MHal_MSPI_Flash_Wait_Done_Flag: Transfer timeout\r\n", 0);
    		MSPI_REG_CHIP_SELECT(mspi, 0xFF); // disable cz
    		return FALSE;
    	}
    }

    MSPI_Flash_Print(" @@@@@@@@@@ 2222 MDrv_MSPI_Flash_Page_Program END \n\r", u32WriteByteLength);
    MSPI_REG_CHIP_SELECT(mspi, 0xFF); // disable cz
    return TRUE;
}

BOOL MDrv_MSPI_Flash_Write(EN_MSPI mspi, EN_MSPI_SLAVE slave, DWORD u32FlashAddr, DWORD u32WriteByteLength, const BYTE *pu8Buff)
{
    DWORD  xdata u32FlashStartPage, u32FlashEndPage, u32FlashPageCnt;
    DWORD  xdata u32FlashPageStartAddress;
    DWORD  xdata u32FlashPageWriteLength;

    if (u32WriteByteLength == 0)
    {
        MSPI_Flash_Print(" @@@@ MDrv_MSPI_Flash_Write: WriteByteLength is 0\n", 0);
        return FALSE;
    }

    u32FlashStartPage = (u32FlashAddr >> 8);
    u32FlashEndPage = ((u32FlashAddr + u32WriteByteLength - 1) >> 8);

    for (u32FlashPageCnt = u32FlashStartPage; u32FlashPageCnt <= u32FlashEndPage; u32FlashPageCnt++)
    {
    	if (u32FlashStartPage == u32FlashEndPage)
    	{
            u32FlashPageStartAddress = u32FlashAddr;
            u32FlashPageWriteLength = u32WriteByteLength;
    	}
        else if (u32FlashPageCnt == u32FlashStartPage)
        {
            u32FlashPageStartAddress = u32FlashAddr;
            u32FlashPageWriteLength = (MSPI_FLASH_PAGE_LENGTH - (u32FlashAddr&0xFF));
        }
        else if (u32FlashPageCnt == u32FlashEndPage)
        {
            u32FlashPageStartAddress = u32FlashPageCnt * MSPI_FLASH_PAGE_LENGTH;
            u32FlashPageWriteLength = ((u32FlashAddr+u32WriteByteLength-1)&0xFF) + 1;
        }
        else
        {
            u32FlashPageStartAddress = u32FlashPageCnt * MSPI_FLASH_PAGE_LENGTH;
            u32FlashPageWriteLength = MSPI_FLASH_PAGE_LENGTH;
        }
        if (!MDrv_MSPI_Flash_Wait_Busy_Flag(mspi, slave, MSPI_POLLING_TIMEOUT_PAGE_PROGRAME))
			return FALSE;

        MDrv_MSPI_Flash_Write_Enable(mspi, slave);
        MDrv_MSPI_Flash_Page_Program(mspi, slave, u32FlashPageStartAddress, u32FlashPageWriteLength, pu8Buff+(u32FlashPageStartAddress-u32FlashAddr));
    }

    if (!MDrv_MSPI_Flash_Wait_Busy_Flag(mspi, slave, MSPI_POLLING_TIMEOUT_PAGE_PROGRAME))
        return FALSE;

    return TRUE;
}
BOOL MDrv_MSPI_Flash_Read(EN_MSPI mspi, EN_MSPI_SLAVE slave, DWORD u32FlashAddr, DWORD u32ReadByteLength, BYTE *pu8ReturnBuff)
{
    BYTE  xdata u8TempCnt = 0;

    if (u32ReadByteLength == 0)
    {
        MSPI_Flash_Print("MDrv_MSPI_Flash_Read u32ReadByteLength is 0\n", 0);
        return FALSE;
    }

    if (!MDrv_MSPI_Flash_Wait_Busy_Flag(mspi, slave, MSPI_POLLING_BUSY_FLAG_TIMEOUT))
    {
        MSPI_Flash_Print("MDrv_MSPI_Flash_Wait_Busy_Flag  timeout \n", 0);
        return FALSE;
    }

    // send read command and address (flash address is big endian)
    MSPI_REG_WBF_SIZE(mspi, 4);
    MSPI_REG_RBF_SIZE(mspi, 0);
    MSPI_REG_WD_BUFF(mspi, 0, SPI_CMD_READ);
    MSPI_REG_WD_BUFF(mspi, 1, (BYTE)((u32FlashAddr>>16)&0xFF));
    MSPI_REG_WD_BUFF(mspi, 2, (BYTE)((u32FlashAddr>>8)&0xFF));
    MSPI_REG_WD_BUFF(mspi, 3, (BYTE)(u32FlashAddr&0xFF));
    MSPI_REG_CHIP_SELECT(mspi, ~(1<<slave)); // select slave device, set cz to low
    MSPI_REG_TRIGGER(mspi, 0x01); // trigger MSPI operation
    if (MHal_MSPI_Flash_Wait_Done_Flag(mspi) != TRUE)
    {
        MSPI_Flash_Print("111 MDrv_MSPI_Flash_Read: Transfer timeout\n",0);
        MSPI_REG_CHIP_SELECT(mspi, 0xFF); // disable cz
        return FALSE;
    }

    MSPI_REG_WBF_SIZE(mspi, 0);
    // get (u32ReadByteLength % MSPI_FLASH_RBF_MAX_SIZE) bytes of data
    if (u32ReadByteLength%MSPI_FLASH_RBF_MAX_SIZE != 0)
    {
	    MSPI_REG_RBF_SIZE(mspi, u32ReadByteLength & (MSPI_FLASH_RBF_MAX_SIZE-1));
	    MSPI_REG_TRIGGER(mspi,0x01);
	    if (MHal_MSPI_Flash_Wait_Done_Flag(mspi) != TRUE)
	    {
	        MSPI_Flash_Print("222 MDrv_MSPI_Flash_Read: Transfer timeout\n", 0);
	        MSPI_REG_CHIP_SELECT(mspi, 0xFF); 	// disable cz
	        return FALSE;
	    }
	    for (u8TempCnt = 0; u8TempCnt < (u32ReadByteLength&(MSPI_FLASH_RBF_MAX_SIZE-1)); u8TempCnt++)
	    {
	        *pu8ReturnBuff++ = MSPI_REG_RD_BUFF(mspi, u8TempCnt);	// store data
	    }
	    u32ReadByteLength -= u32ReadByteLength&(MSPI_FLASH_RBF_MAX_SIZE-1);
    }

    // get 8 bytes of date each time
    MSPI_REG_RBF_SIZE(mspi, MSPI_FLASH_RBF_MAX_SIZE);
    while (u32ReadByteLength > 0)
    {
    	MSPI_REG_TRIGGER(mspi, 0x01);
    	if (MHal_MSPI_Flash_Wait_Done_Flag(mspi) != TRUE)
    	{
	    	MSPI_Flash_Print("333 MHal_MSPI_Flash_Wait_Done_Flag: Transfer timeout\n", 0);
	    	MSPI_REG_CHIP_SELECT(mspi, 0xFF); //disable cz
	    	return FALSE;
    	};
    	for (u8TempCnt = 0; u8TempCnt < MSPI_FLASH_RBF_MAX_SIZE; u8TempCnt++)
    	{
	    	*pu8ReturnBuff++ = MSPI_REG_RD_BUFF(mspi, u8TempCnt); //store data
	    	u32ReadByteLength--;
    	}
    }

    MSPI_REG_CHIP_SELECT(mspi, 0xFF); //disable cz

    return TRUE;
}

#endif	// end of ENABLE_MSPI_FLASH_ACCESS

