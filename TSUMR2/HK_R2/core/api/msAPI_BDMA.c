////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (MStar Confidential Information) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
/// @file   drvdma.c
/// @brief  DRAM BDMA control driver
/// @author MStar Semiconductor Inc.
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Include List
////////////////////////////////////////////////////////////////////////////////
#include "drvGlobal.h"
#include "board.h"
#include "SW_Config.h"
#include "default_option_define.h"
#include "SysInit.h"
#include "debug.h"

#include "msAPI_Flash.h"
#include "msAPI_BDMA.h"
#include "ms_decompress.h"
#include "MsCommon.h"
#include "msflash.h"
#include "Utl.h"

#ifdef MSOS_TYPE_LINUX
#include <stdlib.h>
#endif
#if defined(MIPS_CHAKRA) || defined(ARM_CHAKRA)
#include <stdlib.h>
#endif
////////////////////////////////////////////////////////////////////////////////
// Local defines & local structures
////////////////////////////////////////////////////////////////////////////////
#define DECOMPRESS_BLOCK_SIZE   (32*1024)

////////////////////////////////////////////////////////////////////////////////
// Local Global Variables
////////////////////////////////////////////////////////////////////////////////
static BOOLEAN bMIU_XCopy_FWStatus = FALSE; /* TRUE: Nand Flash; FALSE: SPI Flash; */

////////////////////////////////////////////////////////////////////////////////
// External Funciton
////////////////////////////////////////////////////////////////////////////////
//extern void mhal_dcache_flush(U32 u32Base, U32 u32Size);

////////////////////////////////////////////////////////////////////////////////
// Global Funciton
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: _MApi_BDMA_Convert_Type()
/// @brief \b Function \b Description : convert type to bdma copy type
/// @param <IN>        \b eType       : MEMCOPYTYPE
/// @param <OUT>       \b None :
/// @param <RET>       \b BDMA_CpyType : BDMA copy type
/// @param <GLOBAL>    \b None        :
////////////////////////////////////////////////////////////////////////////////
static BDMA_CpyType _MApi_BDMA_Convert_Type(MEMCOPYTYPE eType)
{
    switch(eType)
    {
    case MIU_FLASH2SDRAM:
        return E_BDMA_FLASH2SDRAM;

    case MIU_SDRAM2SRAM:
        return E_BDMA_SDRAM2SRAM1K_HK51;

    case MIU_SDRAM2SDRAM:
    case MIU_SDRAM2SDRAM_I:
        return E_BDMA_SDRAM2SDRAM;

    case MIU_SDRAM02SDRAM1:
        return E_BDMA_SDRAM2SDRAM1;

    case MIU_SDRAM12SDRAM0:
        return E_BDMA_SDRAM12SDRAM;

    case MIU_SDRAM12SDRAM1:
        return E_BDMA_SDRAM12SDRAM1;

    case MIU_FLASH2VDMCU:
        return E_BDMA_FLASH2VDMCU;

	case MIU_SDRAM02SDRAM2:
		return E_BDMA_SDRAM2SDRAM2;
	case MIU_SDRAM12SDRAM2:
		return E_BDMA_SDRAM12SDRAM2;
	case MIU_SDRAM22SDRAM2:
		return E_BDMA_SDRAM22SDRAM2;
	case MIU_SDRAM22SDRAM1:
		return E_BDMA_SDRAM22SDRAM1;
	case MIU_SDRAM22SDRAM0:
		return E_BDMA_SDRAM22SDRAM;
	case MIU_SDRAM02FLASH:
	    return E_BDMA_SDRAM02FLASH0;
	case MIU_SDRAM12FLASH:
		return E_BDMA_SDRAM12FLASH0;
	case MIU_SDRAM22FLASH:
		return E_BDMA_SDRAM22FLASH0;
	case MIU_SDRAM02FLASH1:
		return E_BDMA_SDRAM02FLASH1;
	case MIU_SDRAM12FLASH1:
    	return E_BDMA_SDRAM12FLASH1;
	case MIU_SDRAM22FLASH1:
		return E_BDMA_SDRAM22FLASH1;
	case MIU_FLASH2SDRAM1:
		return E_BDMA_FLASH2SDRAM1;
	case MIU_FLASH2SDRAM2:
		return E_BDMA_FLASH2SDRAM2;
	case MIU_FLASH12SDRAM0:
		return E_BDMA_FLASH12SDRAM;
	case MIU_FLASH12SDRAM1:
		return E_BDMA_FLASH12SDRAM1;
	case MIU_FLASH12SDRAM2:
		return E_BDMA_FLASH12SDRAM2;
	case MIU_SDRAM02SDRAM0:
		return E_BDMA_SDRAM2SDRAM;

    default:
        return E_BDMA_CPYTYPE_MAX;
    }

}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MApi_BDMA_XCopySetFWStatus()
/// @brief \b Function \b Description : Set MIU XCOPY F/W Source Status
/// @param <IN>        \b bFWStatus   : F/W source status
///                                      - -TRUE:   F/W is on Nand Flash
///                                      - -FALSE:  F/W is on SPI Flash
/// @param <OUT>       \b None    :
/// @param <RET>       \b None    :
/// @param <GLOBAL>    \b None    :
////////////////////////////////////////////////////////////////////////////////
BDMA_Result MApi_BDMA_XCopySetFWStatus(BOOLEAN bFWStatus)
{
    bMIU_XCopy_FWStatus = bFWStatus;
    return E_BDMA_OK;
}

BOOL MApi_BDMA_XCopyGetFWStatus(void)
{
    return bMIU_XCopy_FWStatus;
}
#if CHIP_ID == MST9U4
BDMA_Result _MApi_BDMA_SpiNor2MIU(MS_PHYADDR u32SrcAddr,MS_PHYADDR u32DstAddr, U32 u32Len, BDMA_SrcDev eSrcDev, BDMA_DstDev eDstDev)
{
	FLASH_SEL backupFlashSelection = GetFlashSelection();
	BDMA_Result result;
	if(eSrcDev == E_BDMA_SRCDEV_SPI1)
		SetFlashSelection(SPI1);
	else if(eSrcDev == E_BDMA_SRCDEV_FLASH)
		SetFlashSelection(SPI0);
	result = MDrv_BDMA_MultiFlashCopy2Dram((BDMA_Dev)eSrcDev,(BDMA_Dev)eDstDev,u32SrcAddr,u32DstAddr,u32Len);
	SetFlashSelection(backupFlashSelection);
	return result;
}


BDMA_Result _MApi_BDMA_SpiNand2MIU(MS_PHYADDR u32SrcAddr,MS_PHYADDR u32DstAddr, U32 u32Len,BDMA_SrcDev eSrcDev, BDMA_DstDev eDstDev)
{

	DWORD ReadStartaddr,ReadEndaddr;
	MS_U32 Readsize;
	DWORD FlashAddr;
	DWORD dwPageSize ;

	FLASH_SEL backupFlashSelection = GetFlashSelection();

	BDMA_Dev eBuffDev;
	if((BDMA_BUFFER_MEMORY_TYPE&BIT0) == 0)
		eBuffDev = E_BDMA_DEV_MIU0;
	else //if((BDMA_BUFFER_MEMORY_TYPE&BIT0) == 1) // TBD:Consider extend to MIU2
		eBuffDev = E_BDMA_DEV_MIU1;


	if(eSrcDev == E_BDMA_SRCDEV_SPI1)
	{
		SetFlashSelection(SPI1);
		dwPageSize = GetFlashInfo(SPI1).PageSize;
	}
	else if(eSrcDev == E_BDMA_SRCDEV_FLASH)
	{
		SetFlashSelection(SPI0);
		dwPageSize = GetFlashInfo(SPI0).PageSize;
	}
	else
		return E_BDMA_FAIL;

	while(u32Len > 0)
	{

		ReadStartaddr = u32SrcAddr % dwPageSize;
		ReadEndaddr = (ReadStartaddr+u32Len >= dwPageSize) ? dwPageSize : ReadStartaddr+u32Len;
		Readsize = ReadEndaddr - ReadStartaddr;

		NandFlashReadToCache(u32SrcAddr-ReadStartaddr);
		FlashAddr = NandFlashAddrCal(READ_FROM_CACHE,u32SrcAddr-ReadStartaddr);

		if((ReadStartaddr==0) &&(u32Len>=dwPageSize))
		{   // BDMA for whole page
			MDrv_BDMA_MultiFlashCopy2Dram((BDMA_Dev)eSrcDev,(BDMA_Dev)eDstDev,FlashAddr,u32DstAddr,Readsize);
		}
		else
		{   // BDMA not whole page
			MDrv_BDMA_MultiFlashCopy2Dram((BDMA_Dev)eSrcDev,eBuffDev,FlashAddr,BDMA_BUFFER_ADR,dwPageSize);
			MDrv_BDMA_MultiMemCopy(eBuffDev,(BDMA_Dev)eDstDev,BDMA_BUFFER_ADR+ReadStartaddr,u32DstAddr,Readsize);
		}
        FlashReadSR();
		u32SrcAddr += Readsize;
		u32DstAddr += Readsize;
		u32Len -= Readsize;
	}

	SetFlashSelection(backupFlashSelection);
	return E_BDMA_OK;
}
BDMA_Result MApi_BDMA_Spi2MIU(MS_PHYADDR u32SrcAddr,MS_PHYADDR u32DstAddr, U32 u32Len, MEMCOPYTYPE eType)
{
	BDMA_SrcDev eSrcDev = (BDMA_SrcDev)(_MApi_BDMA_Convert_Type(eType) & 0x0F);
	BDMA_DstDev eDstDev = (BDMA_DstDev)_RShift(_MApi_BDMA_Convert_Type(eType), 8);
    if(eSrcDev==(BDMA_SrcDev)E_BDMA_DEV_FLASH)
		SetFlashSelection(SPI0);
    else if(eSrcDev==(BDMA_SrcDev)E_BDMA_DEV_SRC_SPI1)
		SetFlashSelection(SPI1);
	else
		return E_BDMA_FAIL;
	if(GetFlashInfo(GetFlashSelection()).Type == NOR_FLASH)
		return _MApi_BDMA_SpiNor2MIU(u32SrcAddr,u32DstAddr,u32Len,eSrcDev,eDstDev);
	else if(GetFlashInfo(GetFlashSelection()).Type == NAND_FLASH)
		return _MApi_BDMA_SpiNand2MIU(u32SrcAddr,u32DstAddr,u32Len,eSrcDev,eDstDev);
	else
		return E_BDMA_FAIL;
}

BDMA_Result _MApi_BDMA_MIU2SpiNor(MS_PHYADDR u32SrcAddr,MS_PHYADDR u32DstAddr, U32 u32Len, BDMA_SrcDev eSrcDev, BDMA_DstDev eDstDev)
{
	DWORD dwSectorSize = 4096;// = GetFlashInfo(SPI0).SectorSize;
	DWORD SPIAlignAddr;
	MS_U32 ProgramSize;
	BOOL bPartialProgram;
	BDMA_Dev eBuffDev;
	DWORD pageSize = 0x100;
	DWORD i= 0;

	if(eDstDev == E_BDMA_DSTDEV_SPI1)
		SetFlashSelection(SPI1);
	else if (eDstDev == E_BDMA_DSTDEV_SPI0)
		SetFlashSelection(SPI0);
	else
		return E_BDMA_FAIL;

	if((BDMA_BUFFER_MEMORY_TYPE&BIT0) == 0)
		eBuffDev = E_BDMA_DEV_MIU0;
	else //if((BDMA_BUFFER_MEMORY_TYPE&BIT0) == 1) // TBD:Consider extend to MIU2
		eBuffDev = E_BDMA_DEV_MIU1;

	while(u32Len > 0)
	{
		SPIAlignAddr = dwSectorSize*(u32DstAddr/dwSectorSize);
		ProgramSize = dwSectorSize;
		if((u32DstAddr%dwSectorSize==0)&&(u32Len>=dwSectorSize))
			bPartialProgram = FALSE;
		else
		{
			bPartialProgram = TRUE;

			if(u32DstAddr%dwSectorSize==0)
				ProgramSize = u32Len;
            else if(((u32DstAddr%dwSectorSize)+u32Len)<=dwSectorSize)
				ProgramSize = u32Len;
			else
				ProgramSize = ((u32DstAddr/dwSectorSize)+1)*dwSectorSize - u32DstAddr; // dwSectorSize - u32DstAddr%dwSectorSize
			// BDMA SPI -> MIU
			if(eDstDev == E_BDMA_DSTDEV_SPI0)
			  _MApi_BDMA_SpiNor2MIU( SPIAlignAddr, BDMA_BUFFER_ADR,dwSectorSize,E_BDMA_SRCDEV_FLASH, (BDMA_DstDev)eBuffDev);
			else if (eDstDev == E_BDMA_DSTDEV_SPI1)
 			  _MApi_BDMA_SpiNor2MIU( SPIAlignAddr, BDMA_BUFFER_ADR,dwSectorSize,E_BDMA_SRCDEV_SPI1, (BDMA_DstDev)eBuffDev);
			else
			  return E_BDMA_FAIL;
			// BDMA MIU -> MIU
			MDrv_BDMA_MultiMemCopy((BDMA_Dev)eSrcDev,eBuffDev,u32SrcAddr,BDMA_BUFFER_ADR+(u32DstAddr%dwSectorSize),ProgramSize);
			// FSP Erase
		}

		{
			FlashSectorErase(0,u32DstAddr);

		    for(i=0;i*pageSize<dwSectorSize;i++)
		    {
                while(FlashReadSR()&BIT0);
			    FlashWriteEnable();
			    // PP
				FlashOutsideModeEnable(TRUE,pageSize);
				FlashWritePage(SPIAlignAddr+i*pageSize);
			    if(bPartialProgram)
                  MDrv_BDMA_MemCopySPI(BDMA_BUFFER_ADR+i*pageSize,0,pageSize,(BDMA_SrcDev)eBuffDev,eDstDev);
			    else
                  MDrv_BDMA_MemCopySPI(u32SrcAddr+i*pageSize,0,pageSize,eSrcDev,eDstDev);
			    drvFlashWaitSPINotBusy();
			    // BDMA
				FlashOutsideModeEnable(FALSE,pageSize);
			    FlashReadSR();
		    }
		}

		u32SrcAddr += ProgramSize;
		u32DstAddr += ProgramSize;
		u32Len -=  ProgramSize;
	}

	return E_BDMA_OK;
}

BDMA_Result _MApi_BDMA_MIU2SpiNand(MS_PHYADDR u32SrcAddr,MS_PHYADDR u32DstAddr, U32 u32Len,BDMA_SrcDev eSrcDev, BDMA_DstDev eDstDev)
{
	DWORD dwBlockSize = GetFlashInfo(GetFlashSelection()).BlockSize;//0x800*64;
	DWORD dwPageSize = GetFlashInfo(GetFlashSelection()).PageSize;
	BYTE bPartialProgram = FALSE; // BDMA program area equal to a block
	MS_U32 ProgramSize = 0;
	DWORD SPIAlignAddr;
	FLASH_SEL backFlashSelection = GetFlashSelection();
	BDMA_Dev eBuffDev;
	U16 u16PageIndex = 0;
	if((BDMA_BUFFER_MEMORY_TYPE&BIT0) == 0)
		eBuffDev = E_BDMA_DEV_MIU0;
	else //if((BDMA_BUFFER_MEMORY_TYPE&BIT0) == 1) // TBD:Consider extend to MIU2
		eBuffDev = E_BDMA_DEV_MIU1;
	/*  TODO: TBD
	if(eDstDev == E_BDMA_DSTDEV_SPI0)
		SetFlashSelection(SPI0);
	else
	*/
	if(eDstDev == E_BDMA_DSTDEV_SPI1)
		SetFlashSelection(SPI1);
	else
		return E_BDMA_FAIL; // only valid for SPI0/SPI1

	while(u32Len > 0)
	{
		SPIAlignAddr = dwBlockSize*(u32DstAddr/dwBlockSize);
		ProgramSize = dwBlockSize;
		if((u32DstAddr%dwBlockSize==0)&&(u32Len>=dwBlockSize))
			bPartialProgram = FALSE;
		else
		{
			bPartialProgram = TRUE;

			if(u32DstAddr%dwBlockSize==0)
				ProgramSize = u32Len;
			else if(((u32DstAddr%dwBlockSize)+u32Len)<=dwBlockSize)
				ProgramSize = u32Len;
			else
				ProgramSize = ((u32DstAddr/dwBlockSize)+1)*dwBlockSize - u32DstAddr;
			// BDMA SPI -> MIU
			if(eDstDev == E_BDMA_DSTDEV_SPI1)
			  _MApi_BDMA_SpiNand2MIU( SPIAlignAddr, BDMA_BUFFER_ADR,dwBlockSize,E_BDMA_SRCDEV_SPI1, (BDMA_DstDev)eBuffDev);
			else if(eDstDev == E_BDMA_DSTDEV_SPI0)
			  _MApi_BDMA_SpiNand2MIU( SPIAlignAddr, BDMA_BUFFER_ADR,dwBlockSize,E_BDMA_SRCDEV_FLASH, (BDMA_DstDev)eBuffDev);
			else
			  return E_BDMA_FAIL;
			// BDMA MIU -> MIU
			MDrv_BDMA_MultiMemCopy((BDMA_Dev)eSrcDev,eBuffDev,u32SrcAddr,BDMA_BUFFER_ADR+(u32DstAddr%dwBlockSize),ProgramSize);

		}
		{
			// FSP Erase
			FlashBlockErase(0,u32DstAddr);
			// outside mode
			for(u16PageIndex = 0;u16PageIndex<(dwBlockSize/dwPageSize);u16PageIndex++)
			{
				//ForceDelay1ms(1);
				FlashWriteEnable();
				// PP
				FlashOutsideModeEnable(TRUE,dwPageSize);
				FlashWritePage(0);//(u32DstAddr+u16PageIndex*PAGE_SIZE);
				if(bPartialProgram)
                    MDrv_BDMA_MemCopySPI(BDMA_BUFFER_ADR+u16PageIndex*dwPageSize,SPIAlignAddr+u16PageIndex*dwPageSize,dwPageSize,(BDMA_SrcDev)eBuffDev,eDstDev);
                else
				MDrv_BDMA_MemCopySPI(u32SrcAddr+u16PageIndex*dwPageSize,SPIAlignAddr+u16PageIndex*dwPageSize,dwPageSize,eSrcDev,eDstDev);
				drvFlashWaitSPINotBusy();
				// BDMA
				FlashOutsideModeEnable(FALSE,dwPageSize);
				NandFlashProgramExecute(SPIAlignAddr+u16PageIndex*dwPageSize);
			}

			u32SrcAddr += ProgramSize;
			u32DstAddr += ProgramSize;
			u32Len -=  ProgramSize;
		}
	}
	SetFlashSelection(backFlashSelection);

	return E_BDMA_OK;
}

BDMA_Result MApi_BDMA_MIU2Spi(MS_PHYADDR u32SrcAddr,MS_PHYADDR u32DstAddr, U32 u32Len, MEMCOPYTYPE eType)
{
	BDMA_SrcDev eSrcDev = (BDMA_SrcDev)(_MApi_BDMA_Convert_Type(eType) & 0x0F);
	BDMA_DstDev eDstDev = (BDMA_DstDev)_RShift(_MApi_BDMA_Convert_Type(eType), 8);

    if(eDstDev==(BDMA_DstDev)E_BDMA_DEV_DST_SPI0)
		SetFlashSelection(SPI0);
    else if(eDstDev==(BDMA_DstDev)E_BDMA_DEV_DST_SPI1)
		SetFlashSelection(SPI1);
	else
		return E_BDMA_FAIL;

	if(GetFlashInfo(GetFlashSelection()).Type == NOR_FLASH)
		return _MApi_BDMA_MIU2SpiNor(u32SrcAddr,u32DstAddr,u32Len,eSrcDev,eDstDev);
	else if(GetFlashInfo(GetFlashSelection()).Type == NAND_FLASH)
		return _MApi_BDMA_MIU2SpiNand(u32SrcAddr,u32DstAddr,u32Len,eSrcDev,eDstDev);
	else
		return E_BDMA_FAIL;
}
#endif
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MApi_BDMA_Copy()
/// @brief \b Function \b Description : Get Specific Bin information
/// @param <IN>        \b None        :
/// @param <OUT>       \b pBinInfo    : Get Bin Information
/// @param <RET>       \b BOOL     : Success or Fail
/// @param <GLOBAL>    \b None        :
////////////////////////////////////////////////////////////////////////////////
BDMA_Result MApi_BDMA_Copy(MS_PHYADDR u32Srcaddr, MS_PHYADDR u32Dstaddr, U32 u32Len, MEMCOPYTYPE eType)
{
    U8 u8OpCfg = (MIU_SDRAM2SDRAM_I == eType) ? BDMA_OPCFG_INV_COPY : BDMA_OPCFG_DEF;
	FLASH_SEL backupFlashSelection = GetFlashSelection();
	SetFlashSelection(SPI0);
    switch( eType )
    {
        case MIU_FLASH2SDRAM:
            MDrv_SERFLASH_CopyHnd(u32Srcaddr, u32Dstaddr, u32Len, E_SPIDMA_DEV_MIU0, SPIDMA_OPCFG_DEF);
            break;
        case MIU_FLASH2VDMCU:
            MDrv_SERFLASH_CopyHnd(u32Srcaddr, u32Dstaddr, u32Len, E_SPIDMA_DEV_VDMCU, SPIDMA_OPCFG_DEF);
            break;
#if CHIP_ID == MST9U4
		case MIU_FLASH2SDRAM1:
		case MIU_FLASH2SDRAM2:
		case MIU_FLASH12SDRAM0:
		case MIU_FLASH12SDRAM1:
		case MIU_FLASH12SDRAM2:
			MApi_BDMA_Spi2MIU(u32Srcaddr,u32Dstaddr,u32Len,eType);
			break;
		case MIU_SDRAM02FLASH:
		case MIU_SDRAM02FLASH1:
		case MIU_SDRAM12FLASH:
		case MIU_SDRAM12FLASH1:
		case MIU_SDRAM22FLASH:
		case MIU_SDRAM22FLASH1:
			MApi_BDMA_MIU2Spi(u32Srcaddr,u32Dstaddr,u32Len,eType);
			break;
#endif
		case MIU_SDRAM02SDRAM0:
		case MIU_SDRAM02SDRAM1:
		case MIU_SDRAM02SDRAM2:
		case MIU_SDRAM12SDRAM0:
		case MIU_SDRAM12SDRAM1:
		case MIU_SDRAM12SDRAM2:
		case MIU_SDRAM22SDRAM0:
		case MIU_SDRAM22SDRAM1:
		case MIU_SDRAM22SDRAM2:
        default:
            MDrv_BDMA_CopyHnd(u32Srcaddr, u32Dstaddr, u32Len, _MApi_BDMA_Convert_Type(eType), u8OpCfg);
            break;
    }
	SetFlashSelection(backupFlashSelection);
    return E_BDMA_OK;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MApi_BDMA_XCopy
/// @brief \b Function \b Description : Memory copy for specific operation
/// @param <IN>        \b type    : Memory copy operation type
/// @param <IN>        \b u32Srcaddr : Source address
/// @param <IN>        \b u32Dstaddr : Destination address
/// @param <IN>        \b u32Len     : Length of data
/// @param <OUT>       \b None    :
/// @param <RET>       \b None    :
/// @param <GLOBAL>    \b None    :
////////////////////////////////////////////////////////////////////////////////
BDMA_Result MApi_BDMA_XCopy(MCPY_TYPE eType, U32 u32Srcaddr, U32 u32Dstaddr, U32 u32Len)
{
	FLASH_SEL backupFlashSelection = GetFlashSelection();
	SetFlashSelection(SPI0);
    switch (eType)
    {
    case MCPY_LOADFONT:
    case MCPY_LOADBITMAP:
        MApi_BDMA_CopyFromResource(u32Srcaddr,_PA2VA(u32Dstaddr),u32Len);
        break;

    case MCPY_LOADVDMCUFW:
        MApi_BDMA_Copy(u32Srcaddr, u32Dstaddr, u32Len, MIU_FLASH2VDMCU);
        break;

    case MCPY_LOADLOGO:
        MApi_BDMA_Copy(u32Srcaddr, u32Dstaddr, u32Len, MIU_FLASH2SDRAM);
        break;

    case MCPY_CCS:
        MApi_BDMA_Copy(u32Srcaddr, u32Dstaddr, u32Len, MIU_SDRAM2SDRAM);
        break;

    default:
        MS_DEBUG_MSG(printf("BDMA Xcopy not support:%u", eType));
        break;
    }
	SetFlashSelection(backupFlashSelection);
    return E_BDMA_OK;
}

BOOLEAN (*DoDecompressionFunc)(U8 *pSrc, U8 *pDst, U8 *pTmp, U32 srclen);

eRETCODE MDrv_DMA_LoadBin(BININFO *pBinInfo, U32 u32DstVA, U32 u32DecVA, U32 u32TmpVA)
{
    U32 u32BinLen;
    U32 u32BinAddr;
    #ifndef MSOS_TYPE_LINUX
    int i;
    #endif

    u32BinLen = pBinInfo->B_Len;
    u32BinAddr = pBinInfo->B_FAddr;

//20091127EL
    switch(pBinInfo->B_IsComp)
    {
    case LZSS_COMPRESS:

        // fixme: need to find a buffer to put compressed data => u32DecAddr

        #if 0//(ENABLE_BOOTTIME)
             gU32CompressStepTime = msAPI_Timer_GetTime0();
        #endif

        MApi_BDMA_CopyFromResource(u32BinAddr,u32DecVA,u32BinLen);

        if (ms_DecompressInit((U8*)u32DstVA) == FALSE)
        {
            MS_DEBUG_MSG(printf("decompress fail!\n"));
            return FAILURE;
        }

        ms_Decompress((U8*)u32DecVA, u32BinLen);
        ms_DecompressDeInit();

        #if 0//(ENABLE_BOOTTIME)
            gU32TmpTime = msAPI_Timer_DiffTimeFromNow(gU32CompressStepTime);
            gU32CompressTotalStepTime += gU32TmpTime;
            printf("[boot step time][Decompress time]Bin ID = %x, Decompress time = %ld\n", pBinInfo->B_ID, gU32TmpTime);
        #endif

        break;

    #ifndef MSOS_TYPE_LINUX
    case MS_COMPRESS:
    case MS_COMPRESS7:

        #if 0//(ENABLE_BOOTTIME)
             gU32CompressStepTime = msAPI_Timer_GetTime0();
        #endif

        MApi_BDMA_CopyFromResource(u32BinAddr,u32DecVA,u32BinLen);

        //search compressed file real length
        for(i=3;i<12;i++)
        {
            if ( *((U8 *)(u32DecVA+u32BinLen-i)) == 0xEF )
            {
                if ( *((U8 *)(u32DecVA+u32BinLen-i-1)) == 0xBE )
                    u32BinLen -= (i+1);
            }
        }

        // restore the real length of decompressed bin to Bininfo 20100108EL
	if ( pBinInfo->B_IsComp == MS_COMPRESS7 )
	{
		U32 OriginalFileLength = 0;

		DoDecompressionFunc = DoMsDecompression7;

		for (i = 0; i < 8; i++)
			OriginalFileLength += ( (U32)((U8 *)u32DecVA)[5 + i] << (i << 3) );

		pBinInfo->B_Len = OriginalFileLength;
	}
	else
       {
    	      DoDecompressionFunc = DoMsDecompression;

                memcpy(&(pBinInfo->B_Len), (void*)(u32DecVA+u32BinLen-4), 4);
                //printf("^^^G3^^^ pBinInfo->B_Len = %ld\n", pBinInfo->B_Len);
       }

	 if ( !DoDecompressionFunc((U8 *)(u32DecVA & NON_CACHEABLE_TO_CACHEABLE_MASK), (U8 *)(u32DstVA & NON_CACHEABLE_TO_CACHEABLE_MASK), (U8 *)(u32TmpVA & NON_CACHEABLE_TO_CACHEABLE_MASK), u32BinLen) )
        {
            MS_DEBUG_MSG(printf("msdecompress fail!\n"));
            return FAILURE;
        }

        MsOS_Dcache_Flush((MS_U32)((u32DstVA & NON_CACHEABLE_TO_CACHEABLE_MASK) & (~15)),(MS_U32)((pBinInfo->B_Len+15) & (~15)));
        MsOS_FlushMemory();

        #if 0//(ENABLE_BOOTTIME)
            gU32TmpTime = msAPI_Timer_DiffTimeFromNow(gU32CompressStepTime);
            gU32CompressTotalStepTime += gU32TmpTime;
            printf("[boot step time][Decompress time]Bin ID = %x, Decompress time = %ld\n", pBinInfo->B_ID, gU32TmpTime);
        #endif

        break;
    #endif

    default:    //no compressed bin
        #if (!BLOADER)
        //if((pBinInfo->B_ID == BIN_ID_OSDCP_TEXT)
        //   || (pBinInfo->B_ID == FONT_MSTAR_UNICODE_MVF) )  //20100301EL
        {
            //printf("~~~~~~~~Do nothing at these bins !!\n");
        }
        //else
            MApi_BDMA_CopyFromResource(u32BinAddr,u32DstVA,u32BinLen);
        #else
        MApi_BDMA_CopyFromResource(u32BinAddr,u32DstVA,u32BinLen);
        #endif
        break;
    }

    return SUCCESS;

}

eRETCODE MDrv_DMA_Copy(U32 u32Srcaddr, U32 u32Dstaddr, U32 u32Len, MEMCOPYTYPE eType)
{
    MApi_BDMA_Copy(u32Srcaddr, u32Dstaddr, u32Len, eType);
    return SUCCESS;
}

#ifdef MSOS_TYPE_LINUX
static char RES_FILE[]="/chakra/RES.bin";
#endif

eRETCODE MApi_BDMA_CopyFromResource(U32 offset,U32 destVA,U32 len)
{
    if(bMIU_XCopy_FWStatus)
    {
#ifdef MSOS_TYPE_LINUX
        extern BOOLEAN ReadFromFile(const char* fname,U32 offset,void* dest,U32 len);
//        printf("[CopyFromResource] from RES.bin 0x%08X to 0x%08X(VA)\n", offset, (U32)destVA);
        if(!ReadFromFile(RES_FILE,offset,(void *)destVA,len))
        {
            return FAILURE;
        }
#else
        //UNUSED(RES_FILE);
        MS_DEBUG_MSG(printf("ERROR: MApi_BDMA_CopyFromResource() has not yet implemented for No NAND system"));
        return FAILURE;
#endif
    }
    else
    {
        //printf("[CopyFromResource] from FLASH 0x%08X to 0x%08X(VA)\n", offset, (U32)destVA);
//      change to use msAPI_Flash_Read due to strange stuck problems when BDMA small amount of data
      //MApi_BDMA_Copy(offset, destVA, len, MIU_FLASH2SDRAM);
      msAPI_Flash_Read(offset, len, (U8 *)destVA);
    }
    return SUCCESS;
}
