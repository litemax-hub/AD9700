////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
/// @file msAPI_Flash.h
/// This file includes MStar Flash control application interface
/// @brief API for Flash
/// @author MStar Semiconductor, Inc.
///
////////////////////////////////////////////////////////////////////////////////

#define MSAPI_FLASH_C

/******************************************************************************/
/*                              Header Files                                  */
/******************************************************************************/
// C Library
#include <stdio.h>

// Global Layer
#include "sysinfo.h"
#include "assert.h"

// Driver Layer
#include "drvSERFLASH.h"

// API Layer
#include "msAPI_Flash.h"

//#include "MApp_SaveData.h"

/******************************************************************************/
/*                                 Macro                                      */
/******************************************************************************/
#define APIFLASH_DBG(y) //y

#define XFR_FLASHCODE ((unsigned char  volatile *) 0xC0000000)

/******************************************************************************/
/*                                 Local                                      */
/******************************************************************************/

/******************************************************************************/
/*                               Functions                                    */
/******************************************************************************/
//----------------------------------------------------------------------------------
/// Description :  Calculate the whole flash 16 bits checksum
/// @param u32FlashSize : the size of flash
/// @return : a 16 bits chechsum value of whole image
//----------------------------------------------------------------------------------------------
#if 1 //reverse it if customer want get checksum with bin size rather than flash size
U16 msAPI_CalCheckSum(U32 u32FlashSize)
{

	extern void printData( char *str, WORD value );

	U16 u16CheckSum=0;
	U32 u32Index=0;
	U8 u8MagicNumIndex=0;
	U32 u32BinLength=0;
	BOOL bGetBinLength=FALSE;
	BYTE MagicNum[4]={0,};
	#if 0//for debug only
	U32 u32blockindex=1;
	#endif
	
	#if( ENABLE_WATCH_DOG )
    MDrv_Sys_DisableWatchDog();
	#endif  // end of #if( ENABLE_WATCH_DOG )

	//Find specific string to get the bin file size
	do
	{
		u16CheckSum+=XFR_FLASHCODE[u32Index];
		#if 0
		if (u32Index==u32blockindex*0x10000)
		{
			u32blockindex++;
			printf("$$$$$ u32Index==%x",u32Index);
			printData("$$$$$  u16Checksum=%x",u16CheckSum);
		}
		#endif
		for (u8MagicNumIndex = 0; u8MagicNumIndex < 4; u8MagicNumIndex++)
		{
			MagicNum[u8MagicNumIndex] = XFR_FLASHCODE[u32Index+u8MagicNumIndex];
			//printData(" ########### MagicNum[0]==%x",MagicNum[u8MagicNumIndex]);
			//printData(" %%%%%%%%%% XFR_FLASHCODE[0]==%x",XFR_FLASHCODE[u32Index+u8MagicNumIndex]);
			
			if(MagicNum[0]==0x55 && MagicNum[1]==0xAA && MagicNum[2]==0xAB && MagicNum[3]==0xCD)
			{
				bGetBinLength=TRUE;
				u32BinLength=u32Index;
				//printData("@@@@@@@@@@@@@@@@@@ u32BinLength==%x",u32BinLength);
				//printData("@@@@@@@@@@@@@@@@@@ u16CheckSum==%x",u16CheckSum);
				//printData("@@@@@@@@@@@@@@@@@@ XFR_FLASHCODE[u32Index]==%x",XFR_FLASHCODE[u32Index]);
			}
		}
		u32Index++;
		//printData(" u32Index==%x",u32Index);
	}while((bGetBinLength==FALSE)||(u32Index==u32FlashSize));

	//use the bin file size to calculate checksum
	if (bGetBinLength==TRUE && u32BinLength !=0x00)
		u32FlashSize=u32BinLength;

	if (bGetBinLength==TRUE)
	{
		for (u32Index=(u32BinLength+1); u32Index<u32BinLength+12; u32Index++)
		{
			u16CheckSum += XFR_FLASHCODE[u32Index];
			//printData("u32Index==%x",u32Index);
			//printData("XFR_FLASHCODE[u32Index]==%x",XFR_FLASHCODE[u32Index]);
		}
	}
	else //use flash size to calculate check while didnt get matched string
	{
		u16CheckSum = 0;
		for (u32Index = 0; u32Index < u32FlashSize; u32Index++)
		{
			u16CheckSum += XFR_FLASHCODE[u32Index];
		}
	}
	
	#if( ENABLE_WATCH_DOG )
    MDrv_Sys_EnableWatchDog();
	#endif  // end of #if( ENABLE_WATCH_DOG )
	
	return u16CheckSum;
}
#else
U16 msAPI_CalCheckSum(U32 u32FlashSize)
{
	U16 u16CheckSum;
	U32 u32Index;

	u16CheckSum = 0;
	for (u32Index = 0; u32Index < u32FlashSize; u32Index++)
	{
		u16CheckSum += XFR_FLASHCODE[u32Index];
	}
	return u16CheckSum;
}
#endif
//-------------------------------------------------------------------------------------------------
/// Description :  Set active flash among multi-spi flashes
/// @param  eFlashChipSel   \b IN: The Flash index, 0 for external #1 spi flash, 1 for external #2 spi flash
/// @return TRUE : succeed
/// @return FALSE : not succeed
/// @note   For Secure booting = 0, please check hw_strapping or e-fuse (the board needs to jump)
//----------------------------------------------------------------------------------------------
BOOLEAN msAPI_Flash_ChipSelect(FLASH_CHIP_SELECT eFlashChipSel)
{
#if 0 // No ISP in MST9U
    return msFlash_ChipSelect((U8)eFlashChipSel);
#else
    UNUSED(eFlashChipSel);
#endif
    return FALSE;
}

//-------------------------------------------------------------------------------------------------
/// Description : Protect whole Flash chip
/// @param  bEnable \b IN: TRUE/FALSE: enable/disable protection
/// @return TRUE : succeed
/// @return FALSE : fail before timeout
/// @note   Not allowed in interrupt context
//-------------------------------------------------------------------------------------------------
BOOLEAN msAPI_Flash_WriteProtect(BOOL bEnable)
{
    return MDrv_FLASH_WriteProtect(bEnable);
}
//-------------------------------------------------------------------------------------------------
/// Description : Erase Flash by the specific Flash address
/// @param  u32StartAddr    \b IN: the start address of the Flash
/// @param  u32EraseSize  \b IN: the Size to erase
/// @param  bWait  \b IN: wait write done or not
/// @return TRUE : succeed
/// @return FALSE : illegal parameters or fail.
/// @note   Not allowed in interrupt context
//-------------------------------------------------------------------------------------------------
BOOLEAN msAPI_Flash_AddressErase(U32 u32StartAddr,U32 u32EraseSize,BOOL bWait)
{
    BOOL bRet = FALSE;
    U32 u32FlashSize = 0;

    MDrv_SERFLASH_DetectSize(&u32FlashSize);

    ASSERT((u32StartAddr < u32FlashSize));
    ASSERT((u32EraseSize < u32FlashSize));

    // review this flow, because it's potential danger to erase important data without any perceive.

    MDrv_FLASH_WriteProtect(DISABLE);

    bRet = MDrv_FLASH_AddressErase(u32StartAddr, u32EraseSize, bWait);

    if(msAPI_Flash_IsIndividBlockWPSupport())
    {
	MDrv_FLASH_WriteProtect(ENABLE);
    }
    else
    {
#if 0 ///TODO: fixme
#if ( EEPROM_DB_STORAGE !=  EEPROM_SAVE_ALL)
	 MDrv_FLASH_WriteProtect_Disable_Range_Set(QUICK_DB_GENSETTING_BANK*SYSTEM_BANK_SIZE, (FLASH_SIZE-QUICK_DB_GENSETTING_BANK*SYSTEM_BANK_SIZE));
#endif
#endif
    }

    return bRet;
}

//-------------------------------------------------------------------------------------------------
/// Description : Erase Flash by the specific Flash block
/// @param  u32StartBlock    \b IN: the start block of the Flash
/// @param  u32EndBlock  \b IN: the end block of the Flash
/// @param  bWait  \b IN: wait write done or not
/// @return TRUE : succeed
/// @return FALSE : illegal parameters or fail.
/// @note   Not allowed in interrupt context
//-------------------------------------------------------------------------------------------------
BOOLEAN msAPI_Flash_BlockErase(U32 u32StartBlock, U32 u32EndBlock, BOOL bWait)
{
    BOOL bRet = FALSE;

    // review this flow, because it's potential danger to erase important data without any perceive.

    MDrv_FLASH_WriteProtect(DISABLE);

    bRet = MDrv_SERFLASH_BlockErase(u32StartBlock, u32EndBlock, bWait);

    if(msAPI_Flash_IsIndividBlockWPSupport())
    {
	MDrv_FLASH_WriteProtect(ENABLE);
    }
    else
    {
#if 0 ///TODO: fixme
#if ( EEPROM_DB_STORAGE !=  EEPROM_SAVE_ALL)
	 MDrv_FLASH_WriteProtect_Disable_Range_Set(QUICK_DB_GENSETTING_BANK*SYSTEM_BANK_SIZE, (FLASH_SIZE-QUICK_DB_GENSETTING_BANK*SYSTEM_BANK_SIZE));
#endif
#endif
    }

    return bRet;
}

//-------------------------------------------------------------------------------------------------
/// Description : Write data to Flash by the sprcific flash address
/// @param  u32StartAddr    \b IN: the start address of the Flash (4-B aligned)
/// @param  u32WriteSize    \b IN: write data size in Bytes (4-B aligned)
/// @param  user_buffer \b IN: Virtual Buffer Address ptr to flash write data
/// @return TRUE : succeed
/// @return FALSE : fail before timeout or illegal parameters
/// @note   Not allowed in interrupt context
//-------------------------------------------------------------------------------------------------
BOOLEAN msAPI_Flash_Write(U32 u32StartAddr, U32 u32WriteSize, U8 * user_buffer)
{
    BOOL bRet = FALSE;
    U32 u32FlashSize = 0;

    MDrv_SERFLASH_DetectSize(&u32FlashSize);

    ASSERT((u32StartAddr < u32FlashSize));
    ASSERT((u32WriteSize < u32FlashSize));

    MDrv_FLASH_WriteProtect(DISABLE);

    bRet = MDrv_SERFLASH_Write(u32StartAddr, u32WriteSize, user_buffer);

    if(msAPI_Flash_IsIndividBlockWPSupport())
    {
	MDrv_FLASH_WriteProtect(ENABLE);
    }
    else
    {
#if 0 ///TODO: fixme
#if ( EEPROM_DB_STORAGE !=  EEPROM_SAVE_ALL)
	 MDrv_FLASH_WriteProtect_Disable_Range_Set(QUICK_DB_GENSETTING_BANK*SYSTEM_BANK_SIZE, (FLASH_SIZE-QUICK_DB_GENSETTING_BANK*SYSTEM_BANK_SIZE));
#endif
#endif
    }

    return bRet;
}

//-------------------------------------------------------------------------------------------------
/// Description : Read data from Flash by the sprcific flash address
/// @param  u32StartAddr    \b IN: the start address of the Flash (4-B aligned)
/// @param  u32ReadSize    \b IN: read data size in Bytes (4-B aligned)
/// @param  user_buffer \b OUT: Virtual Buffer Address ptr to flash write data
/// @return TRUE : succeed
/// @return FALSE : fail before timeout or illegal parameters
/// @note   Not allowed in interrupt context
//-------------------------------------------------------------------------------------------------
BOOLEAN msAPI_Flash_Read(U32 u32StartAddr, U32 u32ReadSize, U8 * user_buffer)
{
    U32 u32FlashSize = 0;

    MDrv_SERFLASH_DetectSize(&u32FlashSize);

    ASSERT((u32StartAddr < u32FlashSize));
    ASSERT((u32ReadSize < u32FlashSize));

    return MDrv_FLASH_Read(u32StartAddr, u32ReadSize, user_buffer);
}

//-------------------------------------------------------------------------------------------------
/// Description : Detect flash type by reading the MID and DID
/// @return TRUE : succeed
/// @return FALSE : unknown flash type (if it occurs, please inform flash maintainer.)
/// @note   Not allowed in interrupt context
//-------------------------------------------------------------------------------------------------
BOOLEAN msAPI_Flash_DetectType(void)
{
    return MDrv_SERFLASH_DetectType();
}

//-------------------------------------------------------------------------------------------------
/// Description : Get flash start block index of the flash address
/// @param  u32FlashAddr    \b IN: flash address
/// @param  pu32BlockIndex    \b OUT: poniter to store the returning block index
/// @return TRUE : succeed
/// @return FALSE : illegal parameters
/// @note   Not allowed in interrupt context
//-------------------------------------------------------------------------------------------------
BOOLEAN msAPI_Flash_AddressToBlock(U32 u32FlashAddr, U32 *pu32BlockIndex)
{
    return MDrv_SERFLASH_AddressToBlock(u32FlashAddr, pu32BlockIndex);
}

//-------------------------------------------------------------------------------------------------
/// Description : Get flash start address of a block index
/// @param  u32BlockIndex    \b IN: block index
/// @param  pu32FlashAddr    \b OUT: pointer to store the returning flash address
/// @return TRUE : succeed
/// @return FALSE : illegal parameters
/// @note   Not allowed in interrupt context
//-------------------------------------------------------------------------------------------------
BOOLEAN msAPI_Flash_BlockToAddress(U32 u32BlockIndex, U32 *pu32FlashAddr)
{
    return MDrv_SERFLASH_BlockToAddress(u32BlockIndex, pu32FlashAddr);
}

//-------------------------------------------------------------------------------------------------
/// Description : Check write done in Serial Flash
/// @return TRUE : Done
/// @return FALSE : Wait until TimeOut
/// @note   Not allowed in interrupt context
//-------------------------------------------------------------------------------------------------
BOOLEAN msAPI_Flash_CheckWriteDone()
{
    return MDrv_SERFLASH_CheckWriteDone();
}

BOOLEAN msAPI_Flash_IsIndividBlockWPSupport()
{
    return 1;//Need add driver code to detect Flash type
}

#undef MSAPI_FLASH_C

