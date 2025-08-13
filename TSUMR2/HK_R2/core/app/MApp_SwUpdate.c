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
#define MAPP_SWUPDATE_C

/******************************************************************************/
/*                              Header Files                                  */
/******************************************************************************/
// C Library
#include <stdio.h>
//#include <string.h>
#if 0
#if 0
#include "MsP4Identifier.h"

// Global Layer
#include "Utl.h"
#include "imginfo.h"

// Driver Layer

// API Layer
#include "msAPI_Memory.h"

// MW Layer
#define ENABLE_CMAC_CHECK       0     //1: enable CMAC check. NOTE: Makefile also needs to be modified to add CMAC 16 bytes at the end of MERGE.bin.
#include "mw_usbdownload.h"
#if ENABLE_CMAC_CHECK
#include "mw_usbcmac.h"
#endif

// APP Layer
#include "mapp_swupdate.h"
#include "mw_usbu_ver.h"
#else
//#include "MsP4Identifier.h"

// Global Layer
#include "Utl.h"
#include "imginfo.h"

// Driver Layer

// API Layer
#include "msAPI_Memory.h"

// MW Layer
#define ENABLE_CMAC_CHECK       0     //1: enable CMAC check. NOTE: Makefile also needs to be modified to add CMAC 16 bytes at the end of MERGE.bin.
#include "mw_usbdownload.h"
#if ENABLE_CMAC_CHECK
#include "mw_usbcmac.h"
#endif

// APP Layer
#include "MApp_SwUpdate.h"
#include "mw_usbu_ver.h"
#endif
/******************************************************************************/
/*                                 Macro                                      */
/******************************************************************************/
// Debug
#define BURN_DBG(x)             //x
#define APD_DBG(x)              //x   // Auto partial download

#define MAX_FLASH_SIZE          0x1000000               //SIZE_16MB
#define DOWNLOAD_WIN1_LEN       0x1000                  // 4K
#define DOWNLOAD_VERIFY_LEN     DOWNLOAD_WIN1_LEN/2     // 2K

#define CLEAN                   0
#define DIRTY                   1

#define SAME_IMAGE              0
#define DIFFERENT_IMAGE         1

// Partial update unit: depending on max flash block erase size and min bootloader size.
// Must be 64KB because that max block erase size is 64KB now and min bootloader size is 64KB
#define SW_UPDATE_BLOCK_SIZE    0x10000    //SIZE_64KB

#define MEMBER_OFFSET(struct_name, member_name) ( (U16) &(((struct_name *)0)->member_name) )                     // 16-bit Offset
#define EEPROM_OFFSET(member_name)              (RM_ADDR_DOWNLOAD+MEMBER_OFFSET(ST_DOWNLOAD_INFO, member_name))     // ST_DOWNLOAD_INFO member offset on EEPROM

#define FLASH_ID0   0x0
#define FLASH_ID1   0x1

//typedef unsigned char               MS_BOOL;
//typedef unsigned char               MS_U8;                              // 1 byte
//typedef unsigned long               MS_U32;                             // 4 bytes
/******************************************************************************/
/*                          Private Variables                                 */
/******************************************************************************/
static U8 _u8BarIndex;
static U8 *_pau8CurBar;
static U8 _u8MemCopyType;
static U32 _u32BufStartAddr;

// An update flag (one bit) that represents a block need update (value 1) or not (value 0)
// Max flash size is 16MB
static U8 _au8DirtyBlock[MAX_FLASH_SIZE/SW_UPDATE_BLOCK_SIZE] = {0};
static BOOLEAN _bDownloadType;
static BOOLEAN _bAPBinFlag;

// Progress bar
static U8 _u8CurPercent;
static U8 _u8PercentStart;
static U8 _u8PercentEnd;
static U32 _u32ProgressMaxSize;

static MSIF_Version _mw_swupdate_version = {
       .MW = { MW_USBU_VER },
};

/******************************************************************************/
/*                          External Variables                                */
/******************************************************************************/
extern pZUIDrawCB pZUIDrawPercentageCB;
extern U8 USBUpgrade_Percent[];
extern U32 u32ImgInfoOffset;

/******************************************************************************/
/*                           Public Function                                  */
/******************************************************************************/
BOOLEAN MApp_SwUpdate_Start(EN_SW_UPDATE_MODE u8UpdateMode, BOOLEAN allowTwoFlashMode, BOOLEAN bDownloadType, BOOLEAN bSecondRun, U32 u32DramStartAddr, U32 u32FileSize, U8 u8MemType, BOOLEAN bAPBinFlag);
void MDrv_Sys_WholeChipReset( void );

/******************************************************************************/
/*                          Private Function                                  */
/******************************************************************************/
static void _MApp_SwUpdate_SetAllDirtyBlocks(U8 u8Flag);
static void _MApp_SwUpdate_SetDirtyBlock(U16 u16BlockNo, U8 u8Flag);
static BOOLEAN _MApp_SwUpdate_IsDirtyBlock(U16 u16BlockNo);
static void _MApp_SwUpdate_ShowError(const S8 *pMsg);
static BOOLEAN _MApp_SwUpdate_BurnData(EN_SW_UPDATE_MODE u8UpdateMode, U32 u32DramStartAddr, U32 u32FlashStartAddr, U32 u32DataSize);
static BOOLEAN _MApp_SwUpdate_EraseFlash(U32 u32FlashStartAddr, U32 u32DataSize);
static BOOLEAN _MApp_SwUpdate_WriteFlash(U32 u32SrcAddr, U32 u32DstAddr, U32 u32DataSize);
static BOOLEAN _MApp_SwUpdate_VerifyFlash(U32 u32SrcAddr, U32 u32FlashAddr, U32 u32DataSize);
static U8 _MApp_SwUpdate_CompareImage(EN_SW_UPDATE_MODE u8UpdateMode, U32 u32SrcAddr, U32 u32FlashAddr, U32 u32DataSize);
static void _MApp_SwUpdate_ProgressBar_ShowByPercent( U8 u8Percent );
static void _MApp_SwUpdate_ProgressBar_ShowByValue( U32 u32Value );
static void _MApp_SwUpdate_NextBarPos0(U32 u32Size);
static void _MApp_SwUpdate_NextBarPos1(U32 u32Size);
static void _MApp_SwUpdate_ProgressBar_Init( U8 u8PercentStart, U8 u8PercentEnd, U32 u32Size );

/******************************************************************************/
/*                          External Function                                 */
/******************************************************************************/
extern void MApp_UsbClearWatchDog(void);
extern void MApp_UsbEnableWatchDog(void);
extern void MApp_UsbSetWatchDogTimer(U8 u8sec);
extern void MApp_UsbBDMA_Copy(U32 u32Srcaddr, U32 u32Dstaddr, U32 u32Len, U8 eType);
extern U32 MApp_UsbVA2PA(U32 u32x);
extern U32 MApp_UsbPA2VA(U32 u32x);
extern U32 msAPI_Timer_GetTime0(void);
extern U32 msAPI_Timer_DiffTimeFromNow(U32 u32TaskTimer);
extern MS_BOOL MDrv_FLASH_WriteProtect(MS_BOOL bEnable);
extern MS_BOOL MDrv_FLASH_AddressErase(MS_U32 u32StartAddr, MS_U32 u32Size, MS_U8 u8Wait);
extern MS_BOOL MDrv_FLASH_Read(MS_U32 u32addr, MS_U32 u32size, MS_U8 *pdat);
extern MS_BOOL MDrv_FLASH_Write(MS_U32 u32addr, MS_U32 u32size, MS_U8 *pdat);

#if 0  //leo.chou 20130925  NOT implmented in MST9U
extern BOOLEAN msFlash_ChipSelect(U8 u8FlashID);
#else
BOOLEAN msFlash_ChipSelect(U8 u8FlashID) { UNUSED(u8FlashID); return FALSE; }
#endif
/******************************************************************************/
/*                               Functions                                    */
/******************************************************************************/
/******************************************************************************
 *@brief  : Get library version
 *@param    ppVersion  : [OUT]  MSIF_Version
 *@return : status
/// - 1: success
/// - 0: false
 ******************************************************************************/
BOOLEAN MApp_SwUpdate_GetLibVer(const MSIF_Version **ppVersion)
{
    if (!ppVersion)
    {
        return false;
    }

    *ppVersion = &_mw_swupdate_version;

    return true ;
}

static void _MApp_SwUpdate_SetAllDirtyBlocks(U8 u8Flag)
{
    if (u8Flag == 0)
    {
        memset(_au8DirtyBlock, 0, sizeof(_au8DirtyBlock));
    }
    else
    {
        memset(_au8DirtyBlock, 0xFF, sizeof(_au8DirtyBlock));
    }
}

static void _MApp_SwUpdate_SetDirtyBlock(U16 u16BlockNo, U8 u8Flag)
{
    U8  u8Mask;

    u8Mask = ( 1 << (U8)(u16BlockNo % 8) );
    _au8DirtyBlock[u16BlockNo / 8] &= (~u8Mask);

    if (u8Flag != 0)
    {
        _au8DirtyBlock[u16BlockNo / 8] |= u8Mask;
    }
}

static BOOLEAN _MApp_SwUpdate_IsDirtyBlock(U16 u16BlockNo)
{
    U8  u8Flag;

    u8Flag = _au8DirtyBlock[u16BlockNo / 8] >> (U8)(u16BlockNo % 8);
    u8Flag = u8Flag & 1;
    return (u8Flag == DIRTY);
}

static void _MApp_SwUpdate_ShowError(const S8 *pMsg)
{
    printf("\r\n%s\r\n", pMsg);
}

static BOOLEAN _MApp_SwUpdate_BurnData(EN_SW_UPDATE_MODE u8UpdateMode, U32 u32DramStartAddr, U32 u32FlashStartAddr, U32 u32DataSize)
{
    if (u32DataSize == 0)
    {
        return FALSE;
    }

    // Set dirty blocks
    if (SAME_IMAGE == _MApp_SwUpdate_CompareImage(u8UpdateMode, u32DramStartAddr, u32FlashStartAddr, u32DataSize))
    {
        _MApp_SwUpdate_ProgressBar_ShowByPercent(0xF9);  //return error flag
        return TRUE;
    }

    if (!MDrv_FLASH_WriteProtect(DISABLE))
    {
        printf("Failed to disable flash write protection\r\n");
        return FALSE;
    }

    if (_MApp_SwUpdate_EraseFlash(u32FlashStartAddr, u32DataSize))
    {
        if (_MApp_SwUpdate_WriteFlash(u32DramStartAddr, u32FlashStartAddr, u32DataSize))
        {
            if (_MApp_SwUpdate_VerifyFlash(u32DramStartAddr, u32FlashStartAddr, u32DataSize))
            {
                MDrv_FLASH_WriteProtect(ENABLE);
                return TRUE;
            }
        }
    }

    // Failed! Here the contents of flash may be destroyed.
    // Need to make the system unable to boot up
    // The way is to erase the start sector of the bootloader or AP
    MApp_UsbClearWatchDog();

    MDrv_FLASH_AddressErase( u32FlashStartAddr, SW_UPDATE_BLOCK_SIZE, TRUE );

    MDrv_FLASH_WriteProtect(ENABLE);

    return FALSE;
}

static BOOLEAN _MApp_SwUpdate_EraseFlash(U32 u32FlashStartAddr, U32 u32DataSize)
{
    U16     u16StartBlock, u16EndBlock, u16BlockNo;
    U32     u32FlashEndAddr;
    U8      u8Buf[16];
    U32     u32TimeStart = msAPI_Timer_GetTime0();
    printf("Erasing...\r\n");

    if (u32DataSize == 0)
    {
        return FALSE;
    }

    u32FlashEndAddr = u32FlashStartAddr + u32DataSize - 1;

    // Here, each block is treated as uniform size
    // They may be not the same as that of flash
    u16StartBlock  = u32FlashStartAddr/SW_UPDATE_BLOCK_SIZE;
    u16EndBlock    = u32FlashEndAddr/SW_UPDATE_BLOCK_SIZE;

    if(_bAPBinFlag == FALSE)
    {
        _MApp_SwUpdate_NextBarPos0(u16EndBlock-u16StartBlock+1);
    }
    else
    {
        _MApp_SwUpdate_NextBarPos1(u16EndBlock-u16StartBlock+1);
    }

    BURN_DBG(printf("SW_UPDATE_BLOCK_SIZE=0x%x\r\n", (U32)SW_UPDATE_BLOCK_SIZE));
    BURN_DBG(printf("u16StartBlock=%d, u16EndBlock=%d\r\n", u16StartBlock, u16EndBlock));

    for ( u16BlockNo = u16StartBlock; u16BlockNo <= u16EndBlock; u16BlockNo++ )
    {
        MApp_UsbClearWatchDog();

        _MApp_SwUpdate_ProgressBar_ShowByValue( u16BlockNo - u16StartBlock );

        if (_MApp_SwUpdate_IsDirtyBlock(u16BlockNo))
        {
            U32     u32FlashAddr;
            U8      u8I;

            APD_DBG( printf( "Erasing block %2d\r\n", u16BlockNo) );
            u32FlashAddr = (U32) u16BlockNo * SW_UPDATE_BLOCK_SIZE;

            if ( !MDrv_FLASH_AddressErase(u32FlashAddr, SW_UPDATE_BLOCK_SIZE, TRUE) )
            {
                _MApp_SwUpdate_ShowError((S8 *)"Error: Flash Erase!" );
                return FALSE;
            }

            // Check if the flash block is blank to early find out write problem problem
            // It's enough to check the first few bytes (length is 8-byte alignment) of each block to save check time.
            // If a block is not blank and no error is found here, _MApp_SwUpdate_VerifyFlash() would find it finally.

            MDrv_FLASH_Read(u32FlashAddr, (U32)16,(U8*)((U32)u8Buf));

            for (u8I = 0; u8I < 16; u8I++)
            {
                if (u8Buf[u8I] != 0xFF)
                {
                    _MApp_SwUpdate_ShowError((S8 *)"Error: Flash Blanking (maybe write protect problem)!" );
                    return FALSE;
                }
            }
        }
    }

    printf("   Time: %d sec\r\n", msAPI_Timer_DiffTimeFromNow(u32TimeStart)/1000);

    return TRUE;
}

static BOOLEAN _MApp_SwUpdate_WriteFlash(U32 u32SrcAddr, U32 u32FlashAddr, U32 u32DataSize)
{
    U32 u32LeftSize = u32DataSize;
    U16 u16CopySize = DOWNLOAD_WIN1_LEN;
    U16 u16PrevDirtyBlock = -1;
    U8  *pu8TempBuf = NULL;
    U32 u32TimeStart = msAPI_Timer_GetTime0();
    printf( "Writing...\r\n" );

    if(_bAPBinFlag == FALSE)
    {
        _MApp_SwUpdate_NextBarPos0(u32DataSize);
    }
    else
    {
        _MApp_SwUpdate_NextBarPos1(u32DataSize);
    }

    pu8TempBuf = (U8 *)msAPI_Memory_Allocate(u16CopySize, BUF_ID_USB_DOWNLOAD);
    if( pu8TempBuf == NULL )
    {
        BURN_DBG( printf( "WriteFlash Buf Allocation Fail\r\n" ) );
        return FALSE;
    }

    while ( u32LeftSize > 0 )
    {
        U16 u16BlockNo = u32FlashAddr/SW_UPDATE_BLOCK_SIZE;

        MApp_UsbClearWatchDog();

        _MApp_SwUpdate_ProgressBar_ShowByValue(u32DataSize-u32LeftSize);

        if (u32LeftSize < u16CopySize)
        {
            u16CopySize = u32LeftSize;
        }

        if (_MApp_SwUpdate_IsDirtyBlock(u16BlockNo))
        {
            if (u16PrevDirtyBlock != u16BlockNo)
            {
                u16PrevDirtyBlock = u16BlockNo;
                APD_DBG( printf( "Writing block %2d\r\n", u16BlockNo) );
            }

            MApp_UsbBDMA_Copy(u32SrcAddr, MApp_UsbVA2PA((U32)pu8TempBuf), (U32)u16CopySize, (U8)_u8MemCopyType);

            if ( !MDrv_FLASH_Write( u32FlashAddr, u16CopySize, pu8TempBuf ))
            {
                _MApp_SwUpdate_ShowError((S8 *)"Error: Writing flash failed!" );
                if (pu8TempBuf != NULL)
                {
                    msAPI_Memory_Free((void *)(pu8TempBuf), BUF_ID_USB_DOWNLOAD);
                    pu8TempBuf = NULL;
                }
                return FALSE;
            }
        }

        u32SrcAddr += u16CopySize;
        u32FlashAddr += u16CopySize;
        u32LeftSize -= u16CopySize;
    }

    if (pu8TempBuf != NULL)
    {
        msAPI_Memory_Free((void *)(pu8TempBuf), BUF_ID_USB_DOWNLOAD);
        pu8TempBuf = NULL;
    }

    printf("   Time: %d sec\r\n", msAPI_Timer_DiffTimeFromNow(u32TimeStart)/1000);

    return TRUE;
}

static BOOLEAN _MApp_SwUpdate_VerifyFlash(U32 u32SrcAddr, U32 u32FlashAddr, U32 u32DataSize)
{
    U32     u32LeftSize = u32DataSize;
    U16     u16CopySize = DOWNLOAD_VERIFY_LEN;
    U8      *pu8VerBuf1 = NULL;
    U8      *pu8VerBuf2 = NULL;
    U32     u32TimeStart = msAPI_Timer_GetTime0();
    printf("Verifying...\r\n");

    BURN_DBG(printf("VerifyFlash - Download Buffer Addr: 0x%08X; Flash Addr: 0x%08X; DataSize: 0x%08X\r\n", u32SrcAddr, u32FlashAddr, u32DataSize));

    if(_bAPBinFlag == FALSE)
    {
        _MApp_SwUpdate_NextBarPos0(u32DataSize);
    }
    else
    {
        _MApp_SwUpdate_NextBarPos1(u32DataSize);
    }

    pu8VerBuf1 = (U8 *)msAPI_Memory_Allocate(u16CopySize, BUF_ID_USB_DOWNLOAD);
    if( pu8VerBuf1 == NULL )
    {
        BURN_DBG( printf( "Verify Buf1 Allocation Fail\r\n" ) );
        return FALSE;
    }

    pu8VerBuf2 = (U8 *)msAPI_Memory_Allocate(u16CopySize, BUF_ID_USB_DOWNLOAD);
    if( pu8VerBuf2 == NULL )
    {
        BURN_DBG( printf( "Verify Buf2 Allocation Fail\r\n" ) );
        return FALSE;
    }

    while (u32LeftSize > 0)
    {
        MApp_UsbClearWatchDog();

        _MApp_SwUpdate_ProgressBar_ShowByValue(u32DataSize-u32LeftSize);

        if (u32LeftSize < u16CopySize)
        {
            u16CopySize = u32LeftSize;
        }

        MDrv_FLASH_Read(u32FlashAddr, (U32)u16CopySize, pu8VerBuf1/*(U8*)MApp_UsbVA2PA((void*)pu8VerBuf1)*/);
        //MApp_UsbBDMA_Copy(u32FlashAddr, (U32)pu8VerBuf1, (U32)u16CopySize, (U8)MIU_FLASH2SDRAM);
        MApp_UsbBDMA_Copy(u32SrcAddr, MApp_UsbVA2PA((U32)pu8VerBuf2), (U32)u16CopySize, (U8)_u8MemCopyType);

        if (memcmp((U8 *)pu8VerBuf1, (U8 *)pu8VerBuf2, u16CopySize) != 0)
        {
            BURN_DBG( printf("Flash Addr: 0x%08X, DRAM Addr: 0x%08X", u32FlashAddr, u32SrcAddr) );
            _MApp_SwUpdate_ShowError((S8 *)"Error: Data error in verification!");
            if (pu8VerBuf1 != NULL)
            {
                msAPI_Memory_Free((void *)(pu8VerBuf1), BUF_ID_USB_DOWNLOAD);
                pu8VerBuf1 = NULL;
            }

            if (pu8VerBuf2 != NULL)
            {
                msAPI_Memory_Free((void *)(pu8VerBuf2), BUF_ID_USB_DOWNLOAD);
                pu8VerBuf2 = NULL;
            }

            return FALSE;
        }

        u32SrcAddr += u16CopySize;
        u32FlashAddr += u16CopySize;
        u32LeftSize -= u16CopySize;
    }

    BURN_DBG(printf("\r\n"));

    if (pu8VerBuf1 != NULL)
    {
        msAPI_Memory_Free((void *)(pu8VerBuf1), BUF_ID_USB_DOWNLOAD);
        pu8VerBuf1 = NULL;
    }

    if (pu8VerBuf2 != NULL)
    {
        msAPI_Memory_Free((void *)(pu8VerBuf2), BUF_ID_USB_DOWNLOAD);
        pu8VerBuf2 = NULL;
    }

    printf("   Time: %d sec\r\n", msAPI_Timer_DiffTimeFromNow(u32TimeStart)/1000);

    return TRUE;
}

// Set dirty blocks by binary comparison
static U8 _MApp_SwUpdate_CompareImage(EN_SW_UPDATE_MODE u8UpdateMode, U32 u32SrcAddr, U32 u32FlashAddr, U32 u32DataSize)
{
    U8      u8CompResult;
    U32     u32TimeStart = msAPI_Timer_GetTime0();
    printf("Comparing image...\r\n");

    if (u8UpdateMode == SW_UPDATE_MODE_FULL)
    {
        u8CompResult = DIFFERENT_IMAGE;

        APD_DBG(printf("Full update\r\n"));
        // Set all blocks to be 'dirty'
        _MApp_SwUpdate_SetAllDirtyBlocks(DIRTY);
    }
    else
    {
        U32     u32LeftSize;
        U16     u16CopySize;
        U16     u16BlockNo;
        U16     u16StartBlock;
        U16     u16EndBlock;
        U8      *pu8VerBuf1 = NULL;
        U8      *pu8VerBuf2 = NULL;

        APD_DBG(printf("Auto partial update\r\n"));
        // Set all blocks to be 'clean'
        _MApp_SwUpdate_SetAllDirtyBlocks(CLEAN);

        APD_DBG(printf("Compare Size: %d\r\n", u32DataSize));
        u16BlockNo = 0;
        u32LeftSize = u32DataSize;
        u16CopySize = DOWNLOAD_VERIFY_LEN;
        u8CompResult = SAME_IMAGE;

        pu8VerBuf1 = (U8 *)msAPI_Memory_Allocate(DOWNLOAD_VERIFY_LEN, BUF_ID_USB_DOWNLOAD);
        if( pu8VerBuf1 == NULL )
        {
            BURN_DBG( printf( "Verify Buf1 Allocation Fail\r\n" ) );
            _MApp_SwUpdate_SetAllDirtyBlocks(DIRTY);
            u8CompResult = DIFFERENT_IMAGE;
            return u8CompResult;
        }

        pu8VerBuf2 = (U8 *)msAPI_Memory_Allocate(DOWNLOAD_VERIFY_LEN, BUF_ID_USB_DOWNLOAD);
        if( pu8VerBuf2 == NULL )
        {
            BURN_DBG( printf( "Verify Buf2 Allocation Fail\r\n" ) );
            _MApp_SwUpdate_SetAllDirtyBlocks(DIRTY);
            u8CompResult = DIFFERENT_IMAGE;
            return u8CompResult;
        }

        u16StartBlock = u32FlashAddr/SW_UPDATE_BLOCK_SIZE;
        u16EndBlock    = (u32FlashAddr + u32DataSize)/SW_UPDATE_BLOCK_SIZE;

        if(_bAPBinFlag == FALSE)
        {
            _MApp_SwUpdate_NextBarPos0( u16EndBlock -  u16StartBlock + 1);
        }
        else
        {
            _MApp_SwUpdate_NextBarPos1( u16EndBlock -  u16StartBlock + 1);
        }
        while (u32LeftSize > 0)
        {
            MApp_UsbClearWatchDog();

            u16BlockNo = u32FlashAddr/SW_UPDATE_BLOCK_SIZE;

            if (u32LeftSize < DOWNLOAD_VERIFY_LEN)
            {
                u16CopySize = u32LeftSize;
            }

            _MApp_SwUpdate_ProgressBar_ShowByValue( u16BlockNo - u16StartBlock );

            if ( !_MApp_SwUpdate_IsDirtyBlock(u16BlockNo) )
            {
                MDrv_FLASH_Read(u32FlashAddr, (U32)u16CopySize, (U8*)((void*)pu8VerBuf1));
                //MApp_UsbBDMA_Copy(u32FlashAddr, (U32)pu8VerBuf1, (U32)u16CopySize, (U8)MIU_FLASH2SDRAM);
                MApp_UsbBDMA_Copy(u32SrcAddr, MApp_UsbVA2PA((U32)pu8VerBuf2), (U32)u16CopySize, (U8)_u8MemCopyType);

                if (memcmp((U8 *)pu8VerBuf1, (U8 *)pu8VerBuf2, u16CopySize) != 0)
                {
                    APD_DBG (printf("Block %2d different\r\n", u16BlockNo));
#if 0
                    {
                       U32 i,j;
                       printf("u32FlashAddr = 0x%08X\r\n", u32FlashAddr);
                       printf("pu8VerBuf1 = 0x%08X\r\n", pu8VerBuf1);
                       for(i=0;i<(u16CopySize/0x10);i++)
                       {
                            for(j=0;j<0x10;j++)
                            {
                                printf("%02X ", *(pu8VerBuf1 + i * 16 + j));
                            }
                            printf("\r\n");
                        }

                        printf("pu8VerBuf2 = 0x%08X\r\n", pu8VerBuf2);
                        for(i=0;i<(u16CopySize/0x10);i++)
                        {
                            for(j=0;j<0x10;j++)
                            {
                                printf("%02X ", *(pu8VerBuf2 + i * 16 + j));
                            }
                            printf("\r\n");
                        }

                        printf("u32SrcAddr = 0x%08X\r\n", u32SrcAddr);
                        for(i=0;i<(u16CopySize/0x10);i++)
                        {
                            for(j=0;j<0x10;j++)
                            {
                                printf("%02X ", *((U8 *)MApp_UsbPA2VA(u32SrcAddr) + i * 16 + j));
                            }
                            printf("\r\n");
                        }

                        printf("Stop\r\n");
                        while(1);
                    }
#endif
                    u8CompResult = DIFFERENT_IMAGE;
                    _MApp_SwUpdate_SetDirtyBlock(u16BlockNo, DIRTY);
                }
            }

            u32SrcAddr += u16CopySize;
            u32FlashAddr += u16CopySize;
            u32LeftSize -= u16CopySize;
        }

        if (u8CompResult == SAME_IMAGE)
        {
            APD_DBG(printf("Same image\r\n"));
        }
        else
        {
            // App image may be involved (note: some projects do not have bootloader)
            // Need to make them dirty for the first and the last blocks
            // This is because that it may power off during SW upgrade. Integrity check in bootloader would find this error.

            APD_DBG(printf("Different image\r\n"));
            APD_DBG(printf("The last block: %d\r\n", u16BlockNo));

            // make app's first block be dirty
            _MApp_SwUpdate_SetDirtyBlock(0, DIRTY);
            // make app's last block be dirty
            _MApp_SwUpdate_SetDirtyBlock(u16BlockNo, DIRTY);
        }

        if (pu8VerBuf1 != NULL)
        {
            msAPI_Memory_Free((void *)(pu8VerBuf1), BUF_ID_USB_DOWNLOAD);
            pu8VerBuf1 = NULL;
        }

        if (pu8VerBuf2 != NULL)
        {
            msAPI_Memory_Free((void *)(pu8VerBuf2), BUF_ID_USB_DOWNLOAD);
            pu8VerBuf2 = NULL;
        }

    }

    printf("   Time: %d sec\r\n", msAPI_Timer_DiffTimeFromNow(u32TimeStart)/1000);

    return u8CompResult;
}

static void _MApp_SwUpdate_ProgressBar_ShowByPercent( U8 u8Percent )
{
    if ( u8Percent == _u8CurPercent )
    {
        // Avoid flicker
        return;
    }

    MApp_UsbClearWatchDog();

    _u8CurPercent = u8Percent;
    BURN_DBG( printf( "\r%d%%", (int)_u8CurPercent ) );

    if (pZUIDrawPercentageCB != NULL)
    {
        pZUIDrawPercentageCB(_u8CurPercent);
    }
}

static void _MApp_SwUpdate_ProgressBar_ShowByValue( U32 u32Value )
{
    U8  u8Percent;

    u8Percent = _u8PercentStart + u32Value * ( _u8PercentEnd - _u8PercentStart ) / _u32ProgressMaxSize;
    _MApp_SwUpdate_ProgressBar_ShowByPercent( u8Percent );
}

static void _MApp_SwUpdate_NextBarPos1(U32 u32Size)
{
    U8 start, end;

    MApp_UsbClearWatchDog();

    if (_pau8CurBar == NULL)
    {
        _MApp_SwUpdate_ProgressBar_Init(0, 100, u32Size);
    }
    else
    {
        start = _pau8CurBar[_u8BarIndex];
        if ((start >= 50)&&(_bDownloadType == FALSE))
        {
            start = 50;
        }
        else if((start >= 100)&&(_bDownloadType == TRUE))
        {
            start = 100;
        }
        else
        {
            _u8BarIndex++;
        }

        end = _pau8CurBar[_u8BarIndex];
        if ((end > 50) && (_bDownloadType == FALSE))
        {
            end = 50;
        }
        else if((start >= 100)&&(_bDownloadType == TRUE))
        {
            start = 100;
        }

        if (end < start)
        {
            end = start;
        }

        printf("start = %02d, end = %02d, u32Size = %d\r\n", start, end, u32Size);
        _MApp_SwUpdate_ProgressBar_Init(start, end, u32Size);
    }

    // Update progress bar in OSD
    _MApp_SwUpdate_ProgressBar_ShowByValue(0);
}

static void _MApp_SwUpdate_NextBarPos0(U32 u32Size)
{
    U8 start, end;

    MApp_UsbClearWatchDog();

    if (_pau8CurBar == NULL)
    {
        _MApp_SwUpdate_ProgressBar_Init(0, 100, u32Size);
    }
    else
    {
        start = _pau8CurBar[_u8BarIndex];
        if (start >= 100)
        {
            start = 100;
        }
        else
        {
            _u8BarIndex++;
        }

        end = _pau8CurBar[_u8BarIndex];
        if (end > 100)
        {
            end = 100;
        }

        if (end < start)
        {
            end = start;
        }

        _MApp_SwUpdate_ProgressBar_Init(start, end, u32Size);
    }

    // Update progress bar in OSD
    _MApp_SwUpdate_ProgressBar_ShowByValue(0);
}

static void _MApp_SwUpdate_ProgressBar_Init( U8 u8PercentStart, U8 u8PercentEnd, U32 u32Size )
{
    _u8CurPercent = u8PercentStart;

    // For updating progress bar after Init,
    // Need to let g_u8CurPercent be different with g_u8PercentStart
    if (_u8CurPercent > 0)
    {
        _u8CurPercent--;
    }

    _u8PercentStart = u8PercentStart;
    _u8PercentEnd = u8PercentEnd;
    _u32ProgressMaxSize = ( u32Size == 0 ) ? 1 : u32Size;

    MApp_UsbClearWatchDog();
}

static void _MApp_SwUpdate_Init_PrivateVariable(BOOLEAN bDownloadType)
{
    _bDownloadType = bDownloadType;

    // Progress bar
    if(_bDownloadType == FALSE)
    {
        _u8BarIndex = 1; // 0 is for check CMAC; 1 is for compare image
    }
    else
    {
        _u8BarIndex = 5;
    }

    _pau8CurBar = USBUpgrade_Percent;//NULL
    _u8CurPercent = 0;
    _u8PercentStart = 0;
    _u8PercentEnd = 100;
    _u32ProgressMaxSize = 100;

    _MApp_SwUpdate_SetAllDirtyBlocks(DIRTY);
}

static void _MApp_SwUpdate_Reboot(void)
{
    #if 0
    MDrv_Sys_SetWatchDogTimer(0);
    MDrv_Sys_EnableWatchDog();
    while(1);   // Necessary because of using watch dog reset
    #else
    MDrv_Sys_WholeChipReset();
    #endif
}

//****************************************************************************
/// Software Update Start
/// @param u8UpdateMode IN: Update Mode
/// -@see EN_SW_UPDATE_MODE
/// @param allowTwoFlashMode \b IN: Allow two flash or not
/// -TRUE: Allow two flash
/// -FALSE: Not allow two flash
/// @param bDownloadType IN: Normal download or BIN file download
/// -TRUE: BIN file download
/// -FALSE: Normal download
/// @param bSecondRun IN: Download one image or two images
/// -TRUE: Download two different images to different flash
/// -FALSE: Download one image
/// @return BOOLEAN
/// -TRUE: Software update successfully.
/// -FALSE: Software update failed.
//****************************************************************************
BOOLEAN MApp_SwUpdate_Start(EN_SW_UPDATE_MODE u8UpdateMode, BOOLEAN allowTwoFlashMode, BOOLEAN bDownloadType, BOOLEAN bSecondRun, U32 u32DramStartAddr, U32 u32FileSize, U8 u8MemType, BOOLEAN bAPBinFlag)
{
    U32     u32DataSize;
    U8      u8CurImgType;
    BOOLEAN bResult;
    MS_IMG_INFO TempImgInfo;
    U32 imgBaseOffset;
    U32     u32TimeStart = msAPI_Timer_GetTime0();
    BURN_DBG( printf( "Updating...\r\n" ) );

    _u32BufStartAddr = u32DramStartAddr ;
    _u8MemCopyType = u8MemType ;
    _bAPBinFlag = bAPBinFlag;

    _MApp_SwUpdate_Init_PrivateVariable(bDownloadType);

    MApp_UsbClearWatchDog();

    bResult = FALSE;
    u32DataSize = u32FileSize;
    BURN_DBG(printf("Software size: %d (0x%08X)\r\n", u32DataSize, u32DataSize));
    BURN_DBG(printf("Software addr: 0x%08X\r\n", (U32)u32DramStartAddr));

    if (u32DataSize == 0)
    {
        BURN_DBG(printf("MApp_SwUpdate_UpdateSoftware length=0\r\n"));
        _MApp_SwUpdate_ShowError((S8 *)"Error: No data!");
        return FALSE;
    }
    #if ENABLE_CMAC_CHECK
    else
    {
        U32 u32TimeStartCMAC = msAPI_Timer_GetTime0();
        printf("Checking Signature...\r\n");
        if (Check_CMAC(MApp_UsbPA2VA(u32DramStartAddr), u32FileSize) != TRUE)
        {
            printf("Error> Signature checking failed !!\r\n");
            return FALSE;
        }
        printf("   Time: %d sec\r\n", msAPI_Timer_DiffTimeFromNow(u32TimeStartCMAC)/1000);
    }
    #endif

    // Get image type
    u8CurImgType = IMG_TYPE_NONE;
    BURN_DBG(printf("Checking image...\r\n"));
    MApp_UsbBDMA_Copy( _u32BufStartAddr + u32ImgInfoOffset, MApp_UsbVA2PA((U32)(&TempImgInfo)), sizeof( MS_IMG_INFO ), (U8)_u8MemCopyType);

    if(bDownloadType == FALSE)
    {
        if (MApp_ImgInfo_IsAppImage(&TempImgInfo))
        {
            u8CurImgType |= IMG_TYPE_APP;
        }
    }
    else
    {
        u8CurImgType |= IMG_TYPE_APP;
    }

    BURN_DBG( printf( "\r\nModel Name: %s\r\n", TempImgInfo.u8ModelName ) );
    BURN_DBG( printf( "\r\nu8CurImgType 0x%02x\r\n", u8CurImgType ) );

    //////////////////////////////////////////////////////////////////////
    /* Start to Burn AP/Bootcode from SDRAM to FALSH */
    //////////////////////////////////////////////////////////////////////
    imgBaseOffset=0;

    if ( u8CurImgType == IMG_TYPE_APP )
    {
        U32 u32FlashAddr;
        BURN_DBG( printf( "App image:\r\n" ) );
        BURN_DBG( printf( "  Updating app\r\n" ) );

        if(allowTwoFlashMode)
        {
            imgBaseOffset=0;
            msFlash_ChipSelect(FLASH_ID1);
            BURN_DBG(printf("update AP2!\r\n"));
        }
        else
        {
            imgBaseOffset=0;
            msFlash_ChipSelect(FLASH_ID0);
            BURN_DBG(printf("force to update AP1!\r\n"));
        }

        MDrv_FLASH_Read(u32ImgInfoOffset, sizeof( MS_IMG_INFO ), (U8 *) (void*)&TempImgInfo);

        u32FlashAddr = 0;

        BURN_DBG(printf("MApp_SwUpdate_Start - Download Buffer Addr: 0x%08X; Flash Addr: 0x%08X; DataSize: 0x%08X\r\n", _u32BufStartAddr, u32FlashAddr, u32DataSize));

        U32 u32Offset = 0;
        if(u8UpdateMode == SW_UPDATE_MODE_PARTIAL_OAD)
        {
            U32 AP_OFFSET, AP_OFFSET_ADDR;
            U8 chunk[128];
            AP_OFFSET_ADDR = 0x18;
            u32Offset = 0x20000;
            //Read AP offset
            MDrv_FLASH_Read(u32Offset, 128, (U8 *) (void*)chunk);
            AP_OFFSET = chunk[AP_OFFSET_ADDR+3];
            AP_OFFSET <<= 8;
            AP_OFFSET |= chunk[AP_OFFSET_ADDR+2];
            AP_OFFSET <<= 8;
            AP_OFFSET |= chunk[AP_OFFSET_ADDR+1];
            AP_OFFSET <<= 8;
            AP_OFFSET |= chunk[AP_OFFSET_ADDR+0];
            BURN_DBG( printf("AP_OFFSET: %08X\r\n", AP_OFFSET) );
            u32Offset = AP_OFFSET;
        }
        if ( _MApp_SwUpdate_BurnData( u8UpdateMode, _u32BufStartAddr+u32Offset, u32FlashAddr+u32Offset, u32DataSize-u32Offset ) == TRUE )
        {
            bResult = TRUE;
        }
        else
        {
            BURN_DBG( printf( "Fail to Burn AP Code\r\n" ) );
            _MApp_SwUpdate_ProgressBar_ShowByPercent(0xFD);  //return error flag
        }
    }
    else
    {
        _MApp_SwUpdate_ShowError( (S8 *)"Error: Unknown TV software!" );
        BURN_DBG( printf( "Unknown image type\r\n" ) );
        _MApp_SwUpdate_ProgressBar_ShowByPercent(0xFC);  //return error flag
        return FALSE;
    }

    if ((bResult == TRUE) && (_u8CurPercent != 0xF9))
    {

        _MApp_SwUpdate_ProgressBar_ShowByPercent(100);
        printf("Success.\r\n");
        printf("Update Time: %d sec\r\n", msAPI_Timer_DiffTimeFromNow(u32TimeStart)/1000);

        if(bSecondRun == FALSE)
        {
            BURN_DBG(printf("Rebooting...\r\n"));
            msFlash_ChipSelect(FLASH_ID0);
            _MApp_SwUpdate_Reboot();
            BURN_DBG(printf("Rebooting...fail\r\n"));
            _MApp_SwUpdate_ProgressBar_ShowByPercent(0xFB);  //return error flag
        }
    }
    else if(_u8CurPercent == 0xF9)
    {
        _MApp_SwUpdate_ProgressBar_ShowByPercent(0xF9);
        if(bSecondRun == FALSE)
        {
            BURN_DBG(printf("Rebooting...\r\n"));
            msFlash_ChipSelect(FLASH_ID0);
            _MApp_SwUpdate_Reboot();
            BURN_DBG(printf("Rebooting...fail\r\n"));
            _MApp_SwUpdate_ProgressBar_ShowByPercent(0xFB);  //return error flag
        }
    }
    else
    {
        printf("Failed!\r\n");
        BURN_DBG( printf( "\r\nError: Failed to update software.\r\n" ) );
        _MApp_SwUpdate_ProgressBar_ShowByPercent(0xFA);  //return error flag

        while(1)
        {
            MApp_UsbClearWatchDog();
        }
    }

    return bResult;
}
#endif

//#include "../drvGlobal.h"
#include "Utl.h"
#include "MApp_SwUpdate.h"
#include "default_option_define.h"
#include "msflash.h"
#include "drvUSB.h"
#include "MApp_USBDownload.h"
#include "mw_usbdownload.h"

#include "drvMMIO.h"
#include "drvBDMA.h"
#include "SysInit.h"
#include "usbsw.h"
#include "SysInit.h"

//#include "../hwreg_MST9U.h"
//#include "../mbedtls-2.3.0/include/mbedtls/rsa_verify.h"



#if (ENABLE_USB==1)
#include "drvHostLib.h"
#include "drvscsi.h"
#include "drvHost200.h"

#include "risc32_spr.h"

extern void ProcessTimer0Isr(  MHAL_SavedRegisters *pHalReg, U32 vector );

void Init_Timer2(void);
void Init_Timer2(void)
{
#if 0
    mhal_interrupt_attach( MHAL_INTERRUPT_TICK_TIMER, ProcessTimer0Isr, 0 );
    mtspr( SPR_TTCR, 0);
    mtspr( SPR_TTMR, 0x60000000 | (R2_CLK/1000) );
    mhal_interrupt_enable();
#endif
}
void Init_ICache(void);
void Init_ICache(void)
{
    // Enable I-Cache to speed up the load code process
    __asm__ __volatile__ (
        /* Invalidate I-Cache */
        "\tl.addi  r13,r0,0\n"
        "1:\n"
        "\tl.mtspr %2,r13,0\n"
        "\tl.sfne  r13,%3\n"
        #ifdef __AEONR2__
        "\tl.addi  r13,r13,%4\n"
        "\tl.bf    1b\n"
        #else
        "\tl.bf    1b\n"
        "\tl.addi  r13,r13,%4\n"
        #endif

        /* Enable I-Cache */
        "\tl.mfspr r13,%0,0\n"
        "\tl.ori   r13,r13,%1\n"
        "\tl.mtspr %0,r13,0\n"

        /* Flush instructions out of instruction buffer */
        "\tl.nop\n"
        "\tl.nop\n"
        "\tl.nop\n"
        "\tl.nop\n"
        "\tl.nop\n"
        : // read out from in-line asm
        : //write to in-line asm
          "r" (SPR_SR), "i" (SPR_SR_ICE), "r" (SPR_ICBIR),
          "r" (8192), "i" (32)//  "r" (8192*2), "i" (16)
        : "r13"); // clober define - before enter in-line assembly, stack this reg


}
 void Init_DCache(void);
 void Init_DCache(void)
{
    // Enable D-Cache to speed up decompress
    __asm__ __volatile__ (
        /* Invalidate DC    */
        "\tl.addi  r13,r0,0\n"
        "1:\n"

        "\tl.invalidate_line  0(r13), 0\n"
        "\tl.invalidate_line  0(r13), 1\n"
        "\tl.invalidate_line  0(r13), 2\n"
        "\tl.invalidate_line  0(r13), 3\n"

        "\tl.sfne  r13,%2\n"
        #ifdef __AEONR2__
        "\tl.addi  r13,r13,%3\n"
        "\tl.bf    1b\n"
        #else
        "\tl.bf    1b\n"
        "\tl.addi  r13,r13,%3\n"
        #endif

        /* Enable D-Cache */
        "\tl.mfspr r13,%0,0\n"
        "\tl.ori   r13,r13,%1\n"
        "\tl.mtspr %0,r13,0\n"
        :
        : "r" (SPR_SR), "i" (SPR_SR_DCE),
          "r" ((8192/4) - 16),
          "i" (16)
        : "r13");
}
void Init_BigVarInMem(void);
void Init_BigVarInMem(void)
{
#if 1
    //extern unsigned char _usb_big_var_in_mem[];
    extern U8     *UsbCtrlBuf;//[0x100] __attribute__ ((aligned (16)));
    extern UINT8  *pUsbData;//[256] __attribute__ ((aligned (16))) ;
    extern U8     *QtdBuf;//[Host20_qTD_SIZE*Host20_qTD_MAX+0x20] __attribute__ ((aligned (32)));
    extern U8     *usb_temp_buf;//[Scsi_Max_Transfer_Len] __attribute__ ((aligned (128)));
    extern Host20_Attach_Device_Structure    *sAttachDevice;// __attribute__  ((aligned (8)));
    extern Host20_Attach_Device_Structure    *sHubDevice;// __attribute__  ((aligned (8)));
    extern qHD_Structure *Host20_qHD_List_Control0;//  __attribute__ ((aligned (32)));
    extern qHD_Structure  *Host20_qHD_List_Control1;//  __attribute__ ((aligned (32)));
    extern qHD_Structure  *Host20_qHD_List_Bulk0;//    __attribute__ ((aligned (32)));
    extern qHD_Structure  *Host20_qHD_List_Bulk1;//   __attribute__ ((aligned (32)));
    //extern OTGH_PT_BLK_Struct      *OTGH_PT_BLK;//   __attribute__ ((aligned (16)));
#if 0
    UsbCtrlBuf = _usb_big_var_in_mem+0x00000; //_usb_big_var_in_mem+0x00000;//malloc(0x100);
    pUsbData = _usb_big_var_in_mem+0x10000;//malloc(256);
    QtdBuf = _usb_big_var_in_mem+0x20000;//malloc(Host20_qTD_SIZE*Host20_qTD_MAX+0x20);
    usb_temp_buf = _usb_big_var_in_mem+0x30000;//malloc(Scsi_Max_Transfer_Len);
    sAttachDevice = (Host20_Attach_Device_Structure *)(_usb_big_var_in_mem+0x40000);//malloc(sizeof(Host20_Attach_Device_Structure));
    sHubDevice = (Host20_Attach_Device_Structure *)(_usb_big_var_in_mem+0x50000);//malloc(sizeof(Host20_Attach_Device_Structure));
    Host20_qHD_List_Control0 = (qHD_Structure *)(_usb_big_var_in_mem+0x60000);//malloc(sizeof(Host20_Attach_Device_Structure));
    Host20_qHD_List_Control1 = (qHD_Structure *)(_usb_big_var_in_mem+0x70000);//malloc(sizeof(Host20_Attach_Device_Structure));
    Host20_qHD_List_Bulk0 = (qHD_Structure *)(_usb_big_var_in_mem+0x80000);//malloc(sizeof(Host20_Attach_Device_Structure));
    Host20_qHD_List_Bulk1 = (qHD_Structure *)(_usb_big_var_in_mem+0x90000);//malloc(sizeof(Host20_Attach_Device_Structure));
    OTGH_PT_BLK = (OTGH_PT_BLK_Struct *)(_usb_big_var_in_mem+0xA0000);//malloc(sizeof(Host20_Attach_Device_Structure));
#else
    UsbCtrlBuf = (U8 *)(USB_HOST_BUFFER_ADR+0x00000); //_usb_big_var_in_mem+0x00000;//malloc(0x100);
    pUsbData = (U8 *)((U32)UsbCtrlBuf+0x100);//malloc(256);
    QtdBuf = (U8 *)((U32)pUsbData+256);//malloc(Host20_qTD_SIZE*Host20_qTD_MAX+0x20);
    usb_temp_buf = (U8 *)ALIGN_32(((U32)QtdBuf+Host20_qTD_SIZE*Host20_qTD_MAX+0x20));//malloc(Scsi_Max_Transfer_Len);
    sAttachDevice = (Host20_Attach_Device_Structure *)ALIGN_32(((U32)usb_temp_buf+Scsi_Max_Transfer_Len));//malloc(sizeof(Host20_Attach_Device_Structure));
    sHubDevice = (Host20_Attach_Device_Structure *)ALIGN_32(((U32)sAttachDevice+sizeof(Host20_Attach_Device_Structure)));//malloc(sizeof(Host20_Attach_Device_Structure));
    Host20_qHD_List_Control0 = (qHD_Structure *)ALIGN_32(((U32)sHubDevice+sizeof(Host20_Attach_Device_Structure)));//malloc(sizeof(Host20_Attach_Device_Structure));
    Host20_qHD_List_Control1 = (qHD_Structure *)ALIGN_32(((U32)Host20_qHD_List_Control0+sizeof(Host20_Attach_Device_Structure)));//malloc(sizeof(Host20_Attach_Device_Structure));
    Host20_qHD_List_Bulk0 = (qHD_Structure *)ALIGN_32(((U32)Host20_qHD_List_Control1+sizeof(Host20_Attach_Device_Structure)));//malloc(sizeof(Host20_Attach_Device_Structure));
    Host20_qHD_List_Bulk1 = (qHD_Structure *)ALIGN_32(((U32)Host20_qHD_List_Bulk0+sizeof(Host20_Attach_Device_Structure)));//malloc(sizeof(Host20_Attach_Device_Structure));
    OTGH_PT_BLK = (OTGH_PT_BLK_Struct *)ALIGN_32(((U32)Host20_qHD_List_Bulk1+sizeof(Host20_Attach_Device_Structure)));//malloc(sizeof(Host20_Attach_Device_Structure));

    printf("UsbCtrlBuf=0x%x\r\n", (unsigned int)UsbCtrlBuf);
    printf("pUsbData=0x%x\r\n", (unsigned int)pUsbData);
    printf("QtdBuf=0x%x\r\n", (unsigned int)QtdBuf);
    printf("usb_temp_buf=0x%x\r\n", (unsigned int)usb_temp_buf);
    printf("sAttachDevice=0x%x\r\n", (unsigned int)sAttachDevice);
    printf("Host20_qHD_List_Control0=0x%x\r\n", (unsigned int)Host20_qHD_List_Control0);
    printf("Host20_qHD_List_Control1=0x%x\r\n", (unsigned int)Host20_qHD_List_Control1);
    printf("Host20_qHD_List_Bulk0=0x%x\r\n", (unsigned int)Host20_qHD_List_Bulk0);
    printf("Host20_qHD_List_Bulk1=0x%x\r\n", (unsigned int)Host20_qHD_List_Bulk1);
    printf("Host20_qHD_List_Bulk1=0x%x\r\n", (unsigned int)Host20_qHD_List_Bulk1);
    printf("OTGH_PT_BLK=0x%x\r\n", (unsigned int)OTGH_PT_BLK);
#endif
    memset(UsbCtrlBuf, 0, 0x100);
    memset(pUsbData, 0, 256);
    memset(QtdBuf, 0, (Host20_qTD_SIZE*Host20_qTD_MAX+0x20));
    memset(usb_temp_buf, 0, Scsi_Max_Transfer_Len);
    memset(sAttachDevice, 0, (sizeof(Host20_Attach_Device_Structure)));
    memset(sHubDevice, 0, (sizeof(Host20_Attach_Device_Structure)));
    memset(Host20_qHD_List_Control0, 0, (sizeof(qHD_Structure)));
    memset(Host20_qHD_List_Control1, 0, (sizeof(qHD_Structure)));
    memset(Host20_qHD_List_Bulk0, 0, (sizeof(qHD_Structure)));
    memset(Host20_qHD_List_Bulk1, 0, (sizeof(qHD_Structure)));
    memset(OTGH_PT_BLK, 0, (sizeof(OTGH_PT_BLK_Struct)));
#endif
}

void USB_Init_Configure(void);
void USB_Init_Configure(void)
{
    U32 ii=0;

    msDrvUsb2Init();

#if CHIP_ID == CHIP_MT9700
	//swap
	UTMI_REG_WRITE8(0x0b, UTMI_REG_READ8(0xb) | 0x20);
#endif

    printf("\r\n  USB_Init_Configure\r\n");

    //Init_ICache();
    //Init_DCache();
    //Init_Timer();
    Init_BigVarInMem();

    //MDrv_BDMA_Init(MIU_INTERVAL);
    //MDrv_MMIO_Init(); // I/O remap
    g_ucFlashID = ReadFlashID();


    // Set USB 5V --- Need refine GPIO by board define
    hw_SetUSB5V();

#if CHIP_ID == MST9U4
    MDrv_WriteByte(0x112200, 0x0A); // Disable MAC initial suspend, Reset UHC
    MDrv_WriteByte(0x112200, 0x28); // Release UHC reset, enable UHC XIU function

    MDrv_WriteRegBit(0x112300+0x3C, ENABLE, 0x01);  // set CA_START as 1
    MDrv_SysDelayUs(10000);
    MDrv_WriteRegBit(0x112300+0x3C, DISABLE, 0x01);  // release CA_START

    while( (MDrv_ReadByte(0x112300+0x3C) & 0x02) == 0)  // polling bit <1> (CA_END)
    {
        if (ii++>10000)
        {
            printf("Poll USB port0 CA_END timeout\n");
            break;
        }
    }

    MDrv_WriteByteMask(0x112200+0x02, BIT0, (BIT0|BIT1));  //UHC select enable

    MDrv_WriteRegBit(0x112500+0x40, DISABLE, BIT4); //0: VBUS On.
    MDrv_SysDelayUs(10000);

    //improve the efficiency of USB access MIU when system is busy
    MDrv_WriteRegBit(0x112500+0x81, ENABLE, 0xF);
    //Disable battery charge mode
    MDrv_WriteRegBit(0x112400+0x0C, DISABLE, 0x40);// [6]= reg_into_host_bc_sw_tri
    MDrv_WriteRegBit(0x112400+0x03, DISABLE, 0x40);// [6]= reg_host_bc_en
    MDrv_WriteRegBit(0x112400+0x01, DISABLE, 0x40);//IREF_PDN=1!|b1. (utmi+0x01[6] )

#else
    MDrv_WriteByte(0x150200, 0x0A); // Disable MAC initial suspend, Reset UHC
    MDrv_WriteByte(0x150200, 0x28); // Release UHC reset, enable UHC XIU function

    MDrv_WriteRegBit(0x150300+0x3C, ENABLE, 0x01);  // set CA_START as 1
    MDrv_SysDelayUs(10000);
    MDrv_WriteRegBit(0x150300+0x3C, DISABLE, 0x01);  // release CA_START

    while( (MDrv_ReadByte(0x150300+0x3C) & 0x02) == 0)  // polling bit <1> (CA_END)
    {
        if (ii++>10000)
        {
            printf("Poll USB port0 CA_END timeout\r\n");
            break;
        }
    }

    MDrv_WriteByteMask(0x150200+0x02, BIT0, (BIT0|BIT1));  //UHC select enable

    MDrv_WriteRegBit(0x150500+0x40, DISABLE, BIT4); //0: VBUS On.
    MDrv_SysDelayUs(10000);

    //improve the efficiency of USB access MIU when system is busy
    MDrv_WriteRegBit(0x150500+0x81, ENABLE, 0xF);
    //Disable battery charge mode
    MDrv_WriteRegBit(0x150400+0x0C, DISABLE, 0x40);// [6]= reg_into_host_bc_sw_tri
    MDrv_WriteRegBit(0x150400+0x03, DISABLE, 0x40);// [6]= reg_host_bc_en
    MDrv_WriteRegBit(0x150400+0x01, DISABLE, 0x40);//IREF_PDN=1!|b1. (utmi+0x01[6] )
#endif

    printf("  USB_Init_Configure done...\r\n\r\n");
}
void sw_update_cb(U8 u8Percent);
void sw_update_cb(U8 u8Percent)
{
    if(u8Percent > 100)
        u8Percent = 100;
    //MApp_BL_DisplaySystem(u8Percent);
    printf("%s: u8Percent: %u, at %d\r\n", __func__, u8Percent, __LINE__);
    return;
}

int usb_init(void);
int usb_init(void)
{
    const U32 max_count = 0x1000;
    U32 connect_count = 0;

    while(connect_count++ < max_count)
    {
        printf("Connecting....\r\n");
        if (MDrv_UsbDeviceConnect()==1)
        {
            printf("Connect\r\n");
            if (MDrv_Usb_Device_Enum() == 1)
            {
                printf("USB enumeration success!!\r\n");
                break;
            }
            else
            {
                printf("USB enumeration fail!!\r\n");
            }
        }
        else
        {
            //MAIN_PRINT("NO connect\r\n");
        }
    }
    return 1;
}

int usb_check(void)
{
    U8 u8PortEnStatus = 0;

#if ENABLE_RTE //
    msAPI_OverDriveEnable(0); // USB & OD share the same space. Before USB F/W download, OD should be off first.
#endif

    printf("\r\nrun usb_init..\r\n");

    USB_Init_Configure();
    usb_init();

    printf("run usb_check..\r\n");
    //USB init
    MDrv_UsbDeviceConnect();

    printf("MDrv_UsbDeviceConnect done..\r\n");

    //MDrv_UsbDeviceConnect_Port2();
    MDrv_SysDelayUs(500000);

    u8PortEnStatus = 1;//MDrv_USBGetPortEnableStatus();
    printf("u8PortEnStatus = %d\r\n", u8PortEnStatus);

    //MDrv_Sys_ClearWatchDog();

    if((u8PortEnStatus & BIT0) == BIT0)
    {
        if (!MDrv_UsbDeviceConnect())
        {
            //if((u8PortEnStatus & BIT1) != BIT1)
            {
                printf("USB Port1 is Not detected!\r\n");
            }
        }
        else
        {
            printf("USB Port1 is detected.\r\n");
            MApp_UsbDownload_Init(BIT0, sw_update_cb);
            FlashProgressFuncAttach( sw_update_cb);

            if (MW_UsbDownload_Search())
            {
                printf("updated file in USB is found.\r\n");
                //Thomas update for BOOTLOADER
                //MApp_BL_DisplaySystem_clear(); //clear screen
                //MApp_BL_DisplaySystem_setStatus((S8*)"Software Update(USB)");

                //MApp_BL_DisplaySystem(0);  //leo.chou

                if( MW_UsbDownload_Start() )
                {
                    printf("system reboot...\r\n");
                    MDrv_Write4Byte(0x002C08, (  (((5) * MST_XTAL_CLOCK_HZ)) ));
                    while(1);   // Necessary because of using watch dog reset
                }


                //MDrv_MIU_SPI_SetOffset(0LU);
                //MDrv_Sys_SetWatchDogTimer(0);
                //MDrv_Sys_EnableWatchDog();
                //break;
            }
            else //no sw file detected
            {
                //if((u8PortEnStatus & BIT1) != BIT1)
                {
                    printf("No updated file in USB is found!\r\n");
                }
            }
        }
    }
/*
    else
    {
        printf(".");
    }
*/

#if ENABLE_RTE
    msAPI_OverDriveEnable( UserprefOverDriveSwitch );
#endif

    return 0;
}

#if 0
// check if doing usb download
// current condition is the CRC32 in original merge.bin.
// if CRC32 fail ==> do usb_check()
extern unsigned char _sboot_end[];
BOOL usb_dl_condition(void)
{
    BOOL bRtn;

    MDrv_BDMA_Init(MIU_INTERVAL);
    MDrv_MMIO_Init(); // I/O remap

#if 0  //Leo-temp, need to use different bdma drivers for CRC32
    bRtn = (0x00!=MDrv_BDMA_CRC32(0x00000000, (*(U32 *)((U32)_sboot_end+0x20))+4, BDMA_CRC32_POLY, BDMA_CRC_SEED_0, E_BDMA_SRCDEV_FLASH, FALSE));
#else
    bRtn = TRUE;
#endif

    if(bRtn)
    {
        printf("CRC32 of original merge.bin FAIL\r\n");
    }
    else
    {
        printf("CRC32 of original merge.bin PASS\r\n");
    }

    return bRtn;
}

#if ENABLE_USB_HOST_DUAL_IMAGE
#if ENABLE_USB_HOST_DUAL_IMAGE_SIGN
#define CHECKSUM_PAD_SIZE           (0x04+0x100+0x212)
#else
#define CHECKSUM_PAD_SIZE           0x04
#endif
BOOL usb_dual_image_setting(void)
{
    #define ALIGN_64K(_x_)    (((_x_) + 65535) & ~65535)
    BOOL bRtn;
    U32 u32ImageSize1st;
    U32 u32ImgSize2nd;
    U32 u32DateCode1, u32DateCode2;

#if (ENABLE_DRAM_SELFREFRESH)
    if(IS_WAKEUP_FROM_PM)
    {
        g_u32DualImgOffset = (U32)((U32)MDrv_Read2Byte(0x0010F8)<<16) | MDrv_Read2Byte(0x0010F6);
        g_u32DualImgOffset -= ((U32)_sboot_end+0x80);

        return TRUE;
    }
#endif

    MDrv_BDMA_Init(MIU_INTERVAL);
    MDrv_MMIO_Init(); // I/O remap

    u32DateCode1 = 0;
    u32DateCode2 = 0;
    g_bImg1stNewer = FALSE;
#if ENABLE_USB_HOST_DUAL_IMAGE_TURN
    g_u32DualImgOffset = ENABLE_USB_HOST_DUAL_IMAGE_OFFSET;
    u32DateCode1 = (*(U32 *)((U32)_sboot_end+0x60));
    u32DateCode2 = (*(U32 *)((U32)_sboot_end+0x60+g_u32DualImgOffset));
    printf("1st Date:%x\r\n", (unsigned int)u32DateCode1);
    printf("2nd Date:%x\r\n", (unsigned int)u32DateCode2);
    if( u32DateCode1 > u32DateCode2 )
        g_bImg1stNewer = TRUE;
#else
    g_u32DualImgOffset = ALIGN_64K(((*(U32 *)((U32)_sboot_end+0x20))+CHECKSUM_PAD_SIZE)); // 4 bytes CRC
#endif
    u32ImageSize1st = ((*(U32 *)((U32)_sboot_end+0x20))+0x04);
    u32ImgSize2nd = ((*(U32 *)((U32)_sboot_end+0x20+g_u32DualImgOffset))+0x04);

#if ENABLE_USB_HOST_DUAL_IMAGE_SIGN
    {
        mbedtls_rsa_verify_content_ptr = (unsigned char*)g_u32DualImgOffset;
        mbedtls_rsa_verify_content_size = u32ImgSize2nd;
        mbedtls_rsa_verify_content_size_1st = u32ImageSize1st;
        mbedtls_rsa_verify_sig_ptr = (unsigned char*)(g_u32DualImgOffset + u32ImgSize2nd);
        // only 1st image w/ public key, 2nd image should be released w/o public key
        mbedtls_rsa_verify_key_ptr = (unsigned char*)(mbedtls_rsa_verify_sig_ptr + 0x100);
        bRtn = 1;
        printf("RSA Signature Checking ..\r\n");
    }
#else
    bRtn = (0x00==MDrv_BDMA_CRC32(g_u32DualImgOffset, u32ImgSize2nd, BDMA_CRC32_POLY, BDMA_CRC_SEED_0, E_BDMA_SRCDEV_FLASH, FALSE));
    if(bRtn)
    {
        printf("CRC32 of dual image merge.bin PASS\r\n");
        if( g_bImg1stNewer )
        {
            // check 1st image CRC
            bRtn = (0x00==MDrv_BDMA_CRC32(0, u32ImageSize1st, BDMA_CRC32_POLY, BDMA_CRC_SEED_0, E_BDMA_SRCDEV_FLASH, FALSE));
            if(bRtn)
            {
                g_u32DualImgOffset = 0;
                printf("1st Image Newer\r\n");
            }
            else
            {
                printf("1st Newer Image CRC Error\r\n");
            }
        }
    }
    else
    {
        g_u32DualImgOffset = 0;
        printf("CRC32 of 1st image merge.bin FAIL\r\n");
    }
#endif

    //set PM.bin address offset for dual image
    MDrv_Write2Byte(0x0010F6, (WORD)((DWORD)_sboot_end+0x80+g_u32DualImgOffset)); // 0x80: chunk header size
    MDrv_Write2Byte(0x0010F8, (((DWORD)_sboot_end+0x80+g_u32DualImgOffset)>>16));

    return bRtn;
}
#endif
#endif

#endif

#undef MAPP_SWUPDATE_C

