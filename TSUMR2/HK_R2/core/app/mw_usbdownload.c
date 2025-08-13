//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of MStar Software is expressly prohibited, unless prior written
// permission has been granted by MStar.
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (!¡±MStar Confidential Information!¡L) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#define MW_USBDOWNLOAD_C

/******************************************************************************/
/*                              Header Files                                  */
/******************************************************************************/
// C Library
#include <stdio.h>
//#include <string.h>

// Global Layer
#include "datatype.h"
#include "MsTypes.h"
//#include "imginfo.h"
//#include "debug.h"
//#include "board.h"

// Driver Layer
#include "drvUSB.h"
#include "drvBDMA.h"

// API Layer
#include "msAPI_FCtrl.h"
#include "msAPI_MSDCtrl.h"
#include "msAPI_Memory.h"

// MW Layer
#include "mw_usbdownload.h"

// APP Layer
#include "MApp_SwUpdate.h"
//#include "msAPI_Timer.h"
#include "MApp_USBDownload.h"
#include "FSUtil.h"

#if(ENABLE_6M30_3D_PROCESS)
#include "drvUrsa6M30.h"
#include "MApp_Isp.h"
#endif

#include "msflash.h"
//#include "../drvGlobal.h"
//#include "IOUtil.h"
//#include "Utl.h"

//leo.chou need to implement the following requirement..
#define IsStorageInUse()      (FALSE)//(msAPI_InputSrcType_InUsedType(EN_SYS_INPUT_SOURCE_TYPE_STORAGE))


#if(ENABLE_USB_DOWNLOAD_BIN)
#include "drvSERFLASH.h"
#include "BinInfo.h"
#include "SysInit.h"
#include "msAPI_BDMA.h"
#endif

/******************************************************************************/
/*                                 Enum                                       */
/******************************************************************************/

/******************************************************************************/
/*                                 Macro                                      */
/******************************************************************************/

// Debug message for USB Download
#define USB_DL_DBG(x)           x
#define USB_DL_ASSERT(x)        x

// Max USB detection duration
#define MAX_USB_DETECTION_DURATION  2000    // ms
#define XD2PHY(xadr)        ((U32)(xadr))

#if 0   //ENABLE_USB_HOST_DUAL_IMAGE_SIGN
#define SIGN_SIZE                   (0x100+0x212)
#else
#define SIGN_SIZE                   0x00
#endif

/******************************************************************************/
/*                                 Structure                                  */
/******************************************************************************/

/******************************************************************************/
/*                             Public Variable                                */
/******************************************************************************/
pZUIDrawCB pZUIDrawPercentageCB = 0;

/******************************************************************************/
/*                             Private Variable                               */
/******************************************************************************/
static U8 _u8USBEnvironmentID = 0xFF;
static U8 _u8USBStatus = 0;
static U8 _u8USBPortNo = 0;
static U8 _u8UpgradeMode = 0;
static U8 _u8MemType = 0;
static U8 _u8FlashID = 0;
static U8 _pu8FileName[MAX_FILENAME_LEN] ={0};
static U32 _u32DRAMAddr = 0;
static U32 _u32DRAMLen = 0;
static U32 _u32FlashSize = 0;
U32 g_u32DownloadSize = 0;
U32 g_u32FileSize = 0;

static BOOLEAN  _bUsbInitialized = FALSE;
static BOOLEAN  _bAPBinFlag = FALSE;
static BOOLEAN  _bFlashID1 = FALSE;
static BOOLEAN  _bFlashID2 = FALSE;
static FileEntry _fileEntry2;
static FileEntry _fileEntry;

/******************************************************************************/
/*                           Public Function                                  */
/******************************************************************************/
void MW_UsbDownload_Init(U8 u8USBPortNo, U8 u8FlashID, U32 u32DRAMAddr, U32 u32DRAMLen, U32 u32FlashSize, U8 u8UpgradeMode, U8 u8MemType, U8 * pu8FileName, pZUIDrawCB ZUIDrawPercentage, BOOLEAN bAPBinFlag);
BOOLEAN MW_UsbDownload_Search(void);
BOOLEAN MW_UsbDownload_Start(void);

/******************************************************************************/
/*                             Private Function                                */
/******************************************************************************/
static BOOLEAN _MW_UsbDownload_StartDevice(S8 deviceIndex);
static BOOLEAN _MW_UsbDownload_SearchFileInRoot(U8* pu8FileName, FileEntry* pFileEntry);
static BOOLEAN _MW_UsbDownload_Copy(U32 u32DRAMAddr, U32 u32DRAMLen, U32 u32FlashSize, U32 u32DualImgOffset);
static BOOLEAN _MW_UsbDownload_InitFileSystem(void);
static void _MW_UsbDownload_Exit(void);
static BOOLEAN _MW_UsbDownload_CheckHeader(void);

/******************************************************************************/
/*                          External Function                                 */
/******************************************************************************/
extern void ASCIItoUnicode2(S8 *ps8FileName, U8 len);
extern U32 msAPI_Timer_GetTime0(void);
extern U32 msAPI_Timer_DiffTimeFromNow(U32 u32TaskTimer);
extern BOOLEAN MApp_MassStorage_DeviceDisconnect(void);
extern void MApp_UsbClearWatchDog(void);
#if (OBA2)
extern BOOLEAN MApp_Obama_SwUpdate(U32 u32Addr, U32 u32Size);
extern U32 MApp_UsbPA2VA(U32 u32x);
#endif

/******************************************************************************/
/*                             Code Implementation                            */
/******************************************************************************/
static BOOLEAN _MW_UsbDownload_StartDevice(S8 deviceIndex)
{
    EN_MSD_STATUS status=msAPI_MSDCtrl_GetDeviceStatus(deviceIndex);

    USB_DL_DBG(printf("deviceStatus=%d for deviceIndex=%d\r\n",(U8)status,(U8)deviceIndex));

    if(status==MSD_STATUS_INSERTED)
    {
        return msAPI_FCtrl_ActiveDevice(deviceIndex);
    }
    else if(status==MSD_STATUS_IDLE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static BOOLEAN _MW_UsbDownload_SearchFileInRoot(U8* pu8FileName, FileEntry* pFileEntry)
{
    U8  len;
    U8 *pu8Name;
    U8 lenToCompare = 0;

    BOOLEAN result=FALSE;
    U16 *pFileNameToCompare=NULL;
    #if ENABLE_USB_DOWNLOAD_USE_VERSION_BY_FILENAME
    U16 *pPreFileNameToCompare=NULL;
    #endif
    BOOL bisDir = FALSE;
    U8 u8dirIndex=0;
    U8 u8FileExtIndex=0;

    USB_DL_DBG(printf("_MW_UsbDownload_SearchFileInRoot(pu8FileName=%s)\r\n", pu8FileName));

    len = strlen((char *)pu8FileName);

    if(len==0)
    {
        USB_DL_ASSERT(printf("Error> File name length is 0\r\n"));
        return FALSE;
    }

    if(!msAPI_FCtrl_DirectoryChangeToRoot(_u8USBEnvironmentID))
    {
        USB_DL_ASSERT(printf("Error> msAPI_FCtrl_DirectoryChangeToRoot failed\r\n"));
        return FALSE;
    }

    pu8Name=(U8 *)msAPI_Memory_Allocate(128*2, BUF_ID_USB_DOWNLOAD);
    if(pu8Name==NULL)
    {
        __ASSERT(0);
        USB_DL_ASSERT(printf("Error> 1. msAPI_Memory_Allocate failed\r\n"));
        return FALSE;
    }

    memset(pu8Name, 0, 128*2);

    pFileNameToCompare=(U16 *)msAPI_Memory_Allocate(128*2, BUF_ID_USB_DOWNLOAD);
    if(pFileNameToCompare==NULL)
    {
        __ASSERT(0);
        if (pu8Name != NULL)
        {
            msAPI_Memory_Free(pu8Name, BUF_ID_USB_DOWNLOAD);
            pu8Name = NULL;
        }
        USB_DL_ASSERT(printf("Error> 2. msAPI_Memory_Allocate failed\r\n"));
        return FALSE;
    }

#if ENABLE_USB_DOWNLOAD_USE_VERSION_BY_FILENAME
    FileEntry tempFileEntry;// = NULL;
    //tempFileEntry = (FileEntry*)malloc(sizeof(FileEntry));
    pPreFileNameToCompare=(U16 *)msAPI_Memory_Allocate(128*2, BUF_ID_USB_DOWNLOAD);
    if(pPreFileNameToCompare==NULL)
    {
        __ASSERT(0);
        if (pu8Name != NULL)
        {
            msAPI_Memory_Free(pu8Name, BUF_ID_USB_DOWNLOAD);
            pu8Name = NULL;
        }
        if(pFileNameToCompare !=NULL)
        {
            msAPI_Memory_Free(pFileNameToCompare, BUF_ID_USB_DOWNLOAD);
            pFileNameToCompare = NULL;
        }
        USB_DL_ASSERT(printf("Error> 2.1 msAPI_Memory_Allocate failed\n"));
        return FALSE;
    }
    memset(pPreFileNameToCompare,0,128*2);
#endif
searchDir:
    bisDir = FALSE;
    for(len=0;u8dirIndex<strlen((char *)pu8FileName);len++,u8dirIndex++)  // stretch file /directory name
    {
        if(pu8FileName[u8dirIndex]==0x5C && !bisDir) // if '\' then its a directory
        {
            bisDir = TRUE;
            u8FileExtIndex = 0xFF;
            u8dirIndex++;
            break;
        }
        else if(pu8FileName[u8dirIndex]!=0)
        {
          if(pu8FileName[u8dirIndex] == 0x2E) // if "." indicate the filename extension
            u8FileExtIndex = len;
          pu8Name[len] = pu8FileName[u8dirIndex];
        }
    }
    //ASCIItoUnicode2((S8 *)pu8Name, strlen((char *)pu8Name));
nextentry:
    while(msAPI_FCtrl_EntryNext(_u8USBEnvironmentID,pFileEntry)==ENTRY_NEXT_RESULT_SUCCESS)
    {
        lenToCompare = 0;

        //MApp_UsbClearWatchDog();
#if (OBA2)
        BOOL bShortNameOnly;
        lenToCompare = msAPI_FCtrl_EntryGetLongNameByFileEntry(_u8USBEnvironmentID,
                    pFileEntry, XD2PHY(pFileNameToCompare), 127, &bShortNameOnly);
#else
        lenToCompare = msAPI_FCtrl_EntryGetCurrentName(_u8USBEnvironmentID, XD2PHY(pFileNameToCompare), 127);
#endif
        if(!bisDir || lenToCompare==len)
        {
            U8 i;
            U16 c1, c2;
        #if ENABLE_USB_DOWNLOAD_USE_VERSION_BY_FILENAME
            U16 c3;
            U8 j;
            i = len;
            j = lenToCompare;
            while(i>0 && j>0 && pFileNameToCompare[j-1] != 0x2E &&  !bisDir) // compare the Filename extension match or not
            {
                c1 = pu8Name[--i];
                c2 = pFileNameToCompare[--j];
                if ((c1 >= 'a') && (c1 <= 'z'))
                {
                    c1 -= 'a' - 'A';
                }
                if ((c2 >= 'a') && (c2 <= 'z'))
                {
                    c2 -= 'a' - 'A';
                }
                if (c1 != c2 )
                {
                    goto nextentry;
                }
            }
        #endif
            for(i=0; i<lenToCompare; i++)
            {
                c1 = pu8Name[i];
                c2 = pFileNameToCompare[i];

                if ((c1 >= 'a') && (c1 <= 'z'))
                {
                    c1 -= 'a' - 'A';
                }

                if ((c2 >= 'a') && (c2 <= 'z'))
                {
                    c2 -= 'a' - 'A';
                }

                if (c1 != c2 )
                {
                #if ENABLE_USB_DOWNLOAD_USE_VERSION_BY_FILENAME  // when the filename match the format, we compare the rest number of the filename and find the maximum one
                    c3 = pPreFileNameToCompare[i];
                    if(i>=u8FileExtIndex && c2 > c3)
                    {
                        memset(pPreFileNameToCompare,0,128*2);
                        memcpy(pPreFileNameToCompare,pFileNameToCompare,128*2);
                        memcpy(&tempFileEntry,pFileEntry,sizeof(FileEntry));
                        result = TRUE;
                        goto nextentry;
                    }
                    else if(i>=u8FileExtIndex && c2 == c3)
                    {
                        continue;
                    }
                    else
                #endif
                    goto nextentry;
                }

            }
            if(bisDir)
            {
                if(!msAPI_FCtrl_DirectoryChange(_u8USBEnvironmentID,pFileEntry))
                {
                    USB_DL_ASSERT(printf("can not access direcory %s\r\n",pu8Name));
                    result = FALSE;
                    break;
                }
                goto searchDir;
            }
            USB_DL_DBG(printf("*Found %s\r\n", pu8FileName));

            result=TRUE;
            break;
        }
    }
    #if ENABLE_USB_DOWNLOAD_USE_VERSION_BY_FILENAME
    if(result) // return the max number of filename we found
    {
        memcpy(pFileEntry,&tempFileEntry,sizeof(FileEntry));
    }
    msAPI_Memory_Free(pPreFileNameToCompare, BUF_ID_USB_DOWNLOAD);
    pPreFileNameToCompare = NULL;
    #endif
    msAPI_Memory_Free(pFileNameToCompare, BUF_ID_USB_DOWNLOAD);
    pFileNameToCompare = NULL;

    msAPI_Memory_Free(pu8Name, BUF_ID_USB_DOWNLOAD);
    pu8Name = NULL;

    return result;
}

static BOOLEAN _MW_UsbDownload_Copy(U32 u32DRAMAddr, U32 u32DRAMLen, U32 u32FlashSize, U32 u32DualImgOffset)
{
    U8      u8HandleNo;
    U32     u32FileSize = 0;
    U32     u32LengthLeft = 0;
    U32     u32ReadSize, u32ReadAddr, u32CRC;

    u8HandleNo = msAPI_FCtrl_FileOpen(&_fileEntry, OPEN_MODE_FOR_READ);
    if(u8HandleNo == FCTRL_INVALID_FILE_HANDLE)
    {
        USB_DL_ASSERT(printf("Error> Image file open failed\r\n"));
        return FALSE;
    }

    g_u32FileSize = msAPI_FCtrl_FileLength(u8HandleNo);
    printf("File size: 0x%x\r\n", g_u32FileSize);
    printf("Flash size: 0x%x\r\n", u32FlashSize);
    printf("Download Buffer Addr: 0x%x\r\n", u32DRAMAddr);
    printf("Download Buffer Size: 0x%x\r\n", u32DRAMLen);

    if( (g_u32FileSize == 0)
     || (g_u32FileSize > ENABLE_USB_HOST_DUAL_IMAGE_OFFSET)
     || (u32FlashSize < (ENABLE_USB_HOST_DUAL_IMAGE_OFFSET*2))
    ){
        USB_DL_ASSERT(printf("Error> File size error\r\n"));
        msAPI_FCtrl_FileClose(u8HandleNo);
        return FALSE;
    }

    //MApp_UsbClearWatchDog();

    printf("Copying image from USB to DRAM\r\n");

    // checking CRC of the image file
    u32FileSize = g_u32FileSize;
    u32CRC = 0;
    while( u32FileSize )
    {
        if( u32FileSize > u32DRAMLen )
            u32ReadSize = u32DRAMLen;
        else
            u32ReadSize = u32FileSize;

        u32LengthLeft = msAPI_FCtrl_FileRead(u8HandleNo, u32DRAMAddr, u32ReadSize);

        if(u32LengthLeft == 0)
        {
            u32CRC = crc32(u32CRC, (void*)u32DRAMAddr, u32ReadSize);

            u32FileSize -= u32ReadSize;
        }
        else
        {
            USB_DL_ASSERT(printf("Error> Read file failed\r\n"));
            msAPI_FCtrl_FileClose(u8HandleNo);
            return FALSE;
        }
    }

    if( u32CRC )
    {
        USB_DL_ASSERT(printf("Error> CRC32 Integrity Check failed !!\r\n"));
        msAPI_FCtrl_FileClose(u8HandleNo);
        return FALSE;
    }
    else
    {
        printf("CRC32 Integrity Check pass !!\r\n");
    }

    // flash update
    msAPI_FCtrl_FileSeek(u8HandleNo, 0, FILE_SEEK_SET);
    g_u32DownloadSize = g_u32FileSize;
    while( g_u32DownloadSize )
    {
        if( g_u32DownloadSize > u32DRAMLen )
            u32ReadSize = u32DRAMLen;
        else
            u32ReadSize = g_u32DownloadSize;

        u32LengthLeft = msAPI_FCtrl_FileRead(u8HandleNo, u32DRAMAddr, u32ReadSize);

        // skip first image sboot section
        if( u32DualImgOffset == 0 )
        {
            u32DualImgOffset += ENABLE_USB_HOST_DUAL_IMAGE_SBOOT;
            g_u32DownloadSize -= ENABLE_USB_HOST_DUAL_IMAGE_SBOOT;
            u32ReadSize -= ENABLE_USB_HOST_DUAL_IMAGE_SBOOT;
            u32ReadAddr = u32DRAMAddr + ENABLE_USB_HOST_DUAL_IMAGE_SBOOT;
        }
        else
        {
            u32ReadAddr = u32DRAMAddr;
        }

        if(u32LengthLeft == 0)
        {
            printf("------ Flash Addr: 0x%x\r\n", u32DualImgOffset);
            printf("------ Size: 0x%x\r\n", u32ReadAddr);

            g_u32DownloadSize -= u32ReadSize;

            FlashUpdateFW(TRUE, u32DualImgOffset, (BYTE*)(u32ReadAddr), u32ReadSize, FALSE);

            u32DualImgOffset += u32ReadSize;
        }
        else
        {
            USB_DL_ASSERT(printf("Error> Read file failed\r\n"));
            msAPI_FCtrl_FileClose(u8HandleNo);
            return FALSE;
        }
    }

    msAPI_FCtrl_FileClose(u8HandleNo);

    return TRUE;
}

/////////////////////////////////////////////////////
static BOOLEAN _MW_UsbDownload_InitFileSystem(void)
{
    int i;

    USB_DL_DBG(printf("_MW_UsbDownload_InitFileSystem()\r\n"));

    _bUsbInitialized = FALSE;
    _u8USBEnvironmentID = 0xFF;

    msAPI_FS_Init();
    msAPI_MSDCtrl_CheckDevices();

    if (_u8USBPortNo == 0x1)
    {
        //EN_MSD_STATUS DeviceStatus = msAPI_MSDCtrl_GetDeviceStatus(MSD_USB_INDEX_START);
        for(i = MSD_USB_INDEX_START; i < MSD_USB_INDEX_START + MSD_USB_NUM; i++)
        {
            if(msAPI_MSDCtrl_GetDeviceStatus(i)==MSD_STATUS_INSERTED && _MW_UsbDownload_StartDevice(i))
            {
                _u8USBEnvironmentID=msAPI_FSEnv_Register(0x00 /*'A'*/);
                _bUsbInitialized=msAPI_FCtrl_DirectoryChangeToRoot(_u8USBEnvironmentID);
             break;
            }
        }
        if(!_bUsbInitialized)
        {
            USB_DL_ASSERT(printf("Error> USB PORT 1 DEVICE DETECT ERROR!!\r\n"));
        }
    }
    else if (_u8USBPortNo == 0x2)
    {
#if ENABLE_USB_2// fix coverity error-DEADCODE
        for(i = MSD_USB_2_INDEX_START; i < MSD_USB_2_INDEX_START + MSD_USB_2_NUM; i++)
        {
            if(msAPI_MSDCtrl_GetDeviceStatus(i)==MSD_STATUS_INSERTED && _MW_UsbDownload_StartDevice(i))
        {
            _u8USBEnvironmentID=msAPI_FSEnv_Register(0x00 /*'A'*/);
            _bUsbInitialized=msAPI_FCtrl_DirectoryChangeToRoot(_u8USBEnvironmentID);
             break;
        }
        }
#endif
        if(!_bUsbInitialized)
        {
            USB_DL_ASSERT(printf("Error> USB PORT 2 DEVICE DETECT ERROR!!\r\n"));
        }
    }
    else
    {
        USB_DL_ASSERT(printf("Error> USB PORT NUMBER ERROR!!\r\n"));
    }

    if (_bUsbInitialized)
    {
        return TRUE;
    }
    else
    {
        USB_DL_ASSERT(printf("Error> File system init error\r\n"));
        return FALSE;
    }
}

static void _MW_UsbDownload_Exit(void)
{
    USB_DL_DBG(printf("_MW_UsbDownload_Exit\r\n"));
    if (_bUsbInitialized)
    {
        //_bUsbInitialized = FALSE;
        //msAPI_FCtrl_FileHandleResetAll();
        msAPI_FSEnv_UnRegister(_u8USBEnvironmentID);

        //MApp_MassStorage_DeviceDisconnect();
        //msAPI_FCtrl_Init();
    }
}

static BOOLEAN _MW_UsbDownload_CheckHeader(void)
{
    U8   u8HandleNo, u8FirstFlag = 1, Result = FALSE;
    U32  u32LengthLeft = 0;
    U32  u32FileSize;
    U32  u32ReadSize;

    u8HandleNo = msAPI_FCtrl_FileOpen(&_fileEntry2, OPEN_MODE_FOR_READ);
    if(u8HandleNo == FCTRL_INVALID_FILE_HANDLE)
    {
        USB_DL_ASSERT(printf("Error> Open the 2nd flash BIN file failed\r\n"));
        msAPI_FCtrl_FileClose(u8HandleNo);
        return FALSE;
    }

    u32FileSize = msAPI_FCtrl_FileLength(u8HandleNo);
    USB_DL_DBG( printf("File size: %d ; Download Buffer Size: %d\r\n", (int)u32FileSize,(int)_u32DRAMLen) );

    while( u32FileSize )
    {
        if( u32FileSize > DOWNLOAD_BUFFER_LEN )
            u32ReadSize = DOWNLOAD_BUFFER_LEN;
        else
            u32ReadSize = u32FileSize;

        u32LengthLeft = msAPI_FCtrl_FileRead(u8HandleNo, _u32DRAMAddr, u32ReadSize);

        if(u32LengthLeft == 0)
        {
            u32FileSize -= u32ReadSize;

            if(u8FirstFlag)
            {
                // Check BIN file header: the first 5 bytes
                if((*((U8 *)_u32DRAMAddr) == 0x09) &&
                  (*((U8 *)_u32DRAMAddr+1) == 0x58) &&
                  (*((U8 *)_u32DRAMAddr+2) == 0x33) &&
                  (*((U8 *)_u32DRAMAddr+3) == 0x69)&&
                  (*((U8 *)_u32DRAMAddr+4) == 0x00))
                {
                    //USB_DL_DBG(printf("Find the 2nd flash BIN file header\r\n"));
                    Result = TRUE;
                }
                else
                {
                    USB_DL_ASSERT(printf("Error> Read the 2nd flash BIN file header failed\r\n"));
                    Result = FALSE;
                }

                g_u32DownloadSize = u32FileSize;
                u8FirstFlag = 0;
            }
        }
    }

    if(u32LengthLeft != 0)
    {
        USB_DL_ASSERT(printf("Error> Read the 2nd flash BIN file failed\r\n"));
        Result = FALSE;
    }

    msAPI_FCtrl_FileClose(u8HandleNo);
    return Result;
}

//****************************************************************************
/// USB Download Init
/// @param ZUIDrawPercentage IN: UI draw call back function pointer
/// -@see pZUIDrawCB
/// @return void
//****************************************************************************
void MW_UsbDownload_Init(U8 u8USBPortNo, U8 u8FlashID, U32 u32DRAMAddr, U32 u32DRAMLen, U32 u32FlashSize, U8 u8UpgradeMode, U8 u8MemType, U8 * pu8FileName, pZUIDrawCB ZUIDrawPercentage, BOOLEAN bAPBinFlag)
{
    USB_DL_DBG(printf("MW_UsbDownload_Init()\r\n"));
    USB_DL_DBG(printf(" u8USBPortNo=%d\r\n", u8USBPortNo); );
    USB_DL_DBG(printf(" u8FlashID=%d\r\n", u8FlashID); );
    USB_DL_DBG(printf(" u32DRAMAddr=0x%lx\r\n", u32DRAMAddr); );
    USB_DL_DBG(printf(" u32DRAMLen=0x%lx\r\n", u32DRAMLen); );
    USB_DL_DBG(printf(" u32FlashSize=0x%lx\r\n", u32FlashSize); );
    USB_DL_DBG(printf(" u8UpgradeMode=%d\r\n", u8UpgradeMode); );
    USB_DL_DBG(printf(" u8MemType=%d\r\n", u8MemType); );
    USB_DL_DBG(printf(" file name=%s\r\n", pu8FileName); );
    USB_DL_DBG(printf(" bAPBinFlag=%d\r\n", bAPBinFlag); );


    _u8USBStatus = 0;
    _u8USBPortNo = u8USBPortNo;
    _u8FlashID = u8FlashID;
    _u8MemType = u8MemType;
    _u8UpgradeMode = u8UpgradeMode;
    memcpy(_pu8FileName, pu8FileName, sizeof(_pu8FileName));
    _u32DRAMAddr = u32DRAMAddr;
    _u32DRAMLen = u32DRAMLen;
    _u32FlashSize = u32FlashSize;

    pZUIDrawPercentageCB = ZUIDrawPercentage;

    _bUsbInitialized = FALSE;

    if(bAPBinFlag == TRUE)
    {
        _bAPBinFlag = TRUE;
        _bFlashID1 = FALSE;
        _bFlashID2 = FALSE;
    }
}

//****************************************************************************
/// USB Download Search
/// @param void
/// @return BOOLEAN
/// - TURE : Search donwload file successfully.
/// - FALSE : Search download file failed.
//****************************************************************************
BOOLEAN MW_UsbDownload_Search(void)
{
    U8      u8HandleNo;
    U32     u32TimeStart = MsOS_GetSystemTime();

    USB_DL_DBG(printf("MW_UsbDownload_Search\r\n"));
    USB_DL_DBG(printf(" _u8USBPortNo=%u\r\n", _u8USBPortNo));

    ////MApp_UsbClearWatchDog();

    if(_u8USBPortNo == 0x1)
    {
        while (!MDrv_UsbDeviceConnect())
        {
            U32 u32Duration = MsOS_Timer_DiffTimeFromNow(u32TimeStart); //MDrv_Timer_DiffTimeFromNow(u32TimeStart);
            //MApp_UsbClearWatchDog();
            if (u32Duration > MAX_USB_DETECTION_DURATION)
            {
                USB_DL_ASSERT(printf("Error> USB Download Search: port 1 connection timeout\r\n"));
                return FALSE;
            }
        }
    }
#if (ENABLE_USB_2)
    else if(_u8USBPortNo == 0x2)
    {
        while (!MDrv_UsbDeviceConnect_Port2())
        {
            U32 u32Duration = MsOS_Timer_DiffTimeFromNow(u32TimeStart);//msAPI_Timer_DiffTimeFromNow(u32TimeStart);
            //MApp_UsbClearWatchDog();
            if (u32Duration > MAX_USB_DETECTION_DURATION)
            {
                USB_DL_ASSERT(printf("Error> USB Download Search: port 2 connection timeout\r\n"));
                return FALSE;
            }
        }
    }
#endif
    else
    {
        USB_DL_ASSERT(printf("Error> USB Download Search: port number error\r\n"));
        return FALSE;
    }

    // Init USB and file system, ...
    //MApp_UsbClearWatchDog();

    if (!_MW_UsbDownload_InitFileSystem())
    {
        _MW_UsbDownload_Exit();
        return FALSE;
    }

    // Search files
    //MApp_UsbClearWatchDog();
    if(_bAPBinFlag == TRUE)
    {
        if((_MW_UsbDownload_SearchFileInRoot((U8 *)(_pu8FileName), &_fileEntry2) == TRUE)&&(_MW_UsbDownload_CheckHeader() == TRUE))
        {
            _bFlashID2 = TRUE;
        }
        else
        {
            USB_DL_ASSERT(printf("Error> Search Merge2.bin failed\r\n"));
        }

        if(! _MW_UsbDownload_SearchFileInRoot((U8 *)(_pu8FileName), &_fileEntry))
        {
            USB_DL_ASSERT(printf("Error> Search Merge.bin failed\r\n"));
            if(! _MW_UsbDownload_SearchFileInRoot((U8 *)(_pu8FileName), &_fileEntry))
            {
                USB_DL_ASSERT(printf("Error> Search AP.bin failed\r\n"));
                if(_bFlashID2 == TRUE)
                {
                    return TRUE;// Only update BIN file
                }
                else
                {
                    return FALSE;
                }
            }
        }
        else
        {
            _bFlashID1 = TRUE;
        }
    }
    else
    {
        if(! _MW_UsbDownload_SearchFileInRoot((U8 *)(_pu8FileName), &_fileEntry))
        {
            if(! _MW_UsbDownload_SearchFileInRoot((U8 *)(_pu8FileName), &_fileEntry))
            {
                USB_DL_ASSERT(printf("Error> Search file failed\r\n"));
                if(!IsStorageInUse())
                {
                    _MW_UsbDownload_Exit();
                }
                return FALSE;
            }
        }
    }

    u8HandleNo = msAPI_FCtrl_FileOpen(&_fileEntry, OPEN_MODE_FOR_READ);
    if(u8HandleNo == FCTRL_INVALID_FILE_HANDLE)
    {
        USB_DL_ASSERT(printf("Error> Open file failed\r\n"));
        _MW_UsbDownload_Exit();
        return FALSE;
    }

    msAPI_FCtrl_FileClose(u8HandleNo);
    _u8USBStatus=TRUE;

    USB_DL_DBG(printf("USB Download Search: OK\r\n"));

#if (ENABLE_6M30_3D_PROCESS)
    if(MDrv_Ursa_USB_Update_GetChipType() == SWUpdateForMFC)
    {
        USB_DL_DBG( printf("MFC image found\r\n") );
    }
    else
    {
         USB_DL_DBG( printf("Merged image found\r\n") );
    }
#endif

    return TRUE;
}


#if (ENABLE_6M30_3D_PROCESS)
BOOLEAN MApp_Ursa_SwUpdate_Start(void)
{
    U8 u8Percent;

    if (MDrv_Ursa_USB_Update_GetChipType() == SWUpdateForHK)
    {
      return FALSE;
    }

    do
    {
        u8Percent = MApp_MFC_ISP_Program();
    }while ((u8Percent < 0xF0) && (u8Percent != 0));

    if (u8Percent == 0)
    {
        //printf("MFC.bin updating Success.\r\n");
        //s//printf(&(au8String[0]), "%s","Updated sucess!!!!");
        //USB_DL_ASSERT(printf("Updated sucess!!!!\r\n"));
        msAPI_Timer_Delayms(5000);
        return TRUE;
    }
    else
    {
        //printf("MFC.bin updating Fail.\r\n");
        //sprintf(&(au8String[0]), "%s","Updated error!!!!");
        msAPI_Timer_Delayms(5000);
        return FALSE;
    }
}
#endif

//****************************************************************************
/// USB Download Start
/// @param void
/// @return BOOLEAN
/// - TRUE : USB download succeedfully.
/// - FALSE : USB download failed.
//****************************************************************************
BOOLEAN MW_UsbDownload_Start(void)
{
    U32 u32DualImgOffset=0;

    USB_DL_DBG(printf("MW_UsbDownload_Start\r\n"));

    if(g_dwSpiDuelImageOffset < ENABLE_USB_HOST_DUAL_IMAGE_OFFSET)
        u32DualImgOffset = ENABLE_USB_HOST_DUAL_IMAGE_OFFSET;
    else
        u32DualImgOffset = 0;

    printf("g_dwSpiDuelImageOffset = 0x%x\r\n",g_dwSpiDuelImageOffset);
    printf("u32DualImgOffset = 0x%x\r\n",u32DualImgOffset);

    if (!_MW_UsbDownload_Copy(_u32DRAMAddr, _u32DRAMLen, _u32FlashSize, u32DualImgOffset))
    {
        //printf("== Case 4 ==\r\n");

        _MW_UsbDownload_Exit();

        return FALSE;
    }

    return TRUE;
}

BOOLEAN MApp_UsbSaveData_InitFileSystem(void)
{
    return _MW_UsbDownload_InitFileSystem();
}

void MApp_UsbSaveData_Exit(void)
{
    _MW_UsbDownload_Exit();
}

#if (ENABLE_SZ_FACTORY_USB_SAVE_DATABASE_FUNCTION == ENABLE ||ENABLE_SZ_FACTORY_USB_SAVE_MAP_FUNCTION == ENABLE)
void MApp_UsbSaveData_SetPort(U8 u8USBPortNo)
{
    _u8USBPortNo = u8USBPortNo;
}

U8 MApp_UsbSaveData_OpenNewFileForWrite(U16* pString,U8 charLen)
{
    return msAPI_FCtrl_FileCreate(_u8USBEnvironmentID ,pString, charLen, CREATE_MODE_NEW);
}

BOOLEAN MApp_UsbSaveData_SearchFileInRoot(U8* pu8FileName, FileEntry* pFileEntry)
{
    return _MW_UsbDownload_SearchFileInRoot(pu8FileName, pFileEntry);
}
#endif


#if( ENABLE_USB_DOWNLOAD_BIN )

#define DEBUG_USB_DL_BIN(x)   x


#include "msAPI_Memory.h"


static BOOL MW_EraseFlash(U32 u32DestFlashAddr, U32 u32Size)
{
    if( (u32DestFlashAddr&0xFFFF) != 0 )
    {
        printf("\r\n Error: Invalid u32DestFlashAddr=0x%lX\r\n", u32DestFlashAddr);
        return FALSE;
    }

    if( (u32Size&0xFFFF) != 0 )
    {
        printf("\r\n Error: Invalid u32Size=0x%lX\r\n", u32Size);
        return FALSE;
    }

    if( FALSE == MDrv_FLASH_AddressErase(u32DestFlashAddr, u32Size, TRUE) )
    {
        printf("\r\n Error: MDrv_FLASH_AddressErase() failed!\r\n");
        return FALSE;
    }

    {   // Check flash erase status
        U16 u16FlashBlockStart = u32DestFlashAddr >> 16;
        U16 u16FlashBlockEnd = (u32DestFlashAddr + u32Size - 1) >> 16;
        U16 u16BlockIdx;
        U32 i;
        U8 au8FlashBuf[16];

        for( u16BlockIdx = u16FlashBlockStart; u16BlockIdx <= u16FlashBlockEnd; ++ u16BlockIdx )
        {
            MDrv_FLASH_Read(u32DestFlashAddr, 16, au8FlashBuf );
            for( i = 0; i < 16; ++ i )
            {
                if( au8FlashBuf[i] != 0xFF )
                {
                    printf("\r\n Error: Flash blanking failed! addr=0x%lX\r\n", u32DestFlashAddr + i );
                    return FALSE;
                }
            }
        }
    }

    return TRUE;
}

EnuUsbDlErrCode MW_CopyDramToFlash(U32 u32SrcDramAddr, U32 u32DestFlashAddr, U32 u32Size)
{
    EnuUsbDlErrCode errCode = E_USB_DL__OK;
    U8 *flash_buf;
    U32 u32CopySize;
    //BOOL bResult = TRUE;
    BOOL bNeedEnableWriteProtect = FALSE;


#define DRAM_TO_FLASH_BUF   1024


    DEBUG_USB_DL_BIN( printf("MW_CopyDramToFlash()\r\n"); );

    //msAPI_Flash_ChipSelect((FLASH_CHIP_SELECT)0);

    if( (u32DestFlashAddr&0xFFFF) != 0 )
    {
        printf("\r\n Error: Invalid u32DestFlashAddr=0x%lX\r\n", u32DestFlashAddr);
        return E_USB_DL__PARAMETER_ERROR;
    }

    if( (u32Size&0xFFFF) != 0 )
    {
        printf("\r\n Error: Invalid u32Size=0x%lX\r\n", u32Size);
        return E_USB_DL__PARAMETER_ERROR;
    }

    flash_buf = (U8*)msAPI_Memory_Allocate(DRAM_TO_FLASH_BUF, BUF_ID_USB_DOWNLOAD_BIN);
    if (flash_buf == NULL)
    {
        //__ASSERT(0);
        printf("\r\nmalloc memory fail!\r\n");
        return E_USB_DL__MEMORY_ALLOC_FAILED;
    }


    do
    {
        if( FALSE == MDrv_FLASH_WriteProtect_Disable_Range_Set(u32DestFlashAddr, u32Size) )
        {
            printf("\r\nMDrv_FLASH_WriteProtect_Disable_Range_Set() failed!\r\n");
            //bResult = FALSE;
            errCode = E_USB_DL__FLASH_WRITE_PROTECT_FAILED;
            break;
        }

        bNeedEnableWriteProtect = TRUE;

        if( FALSE == MW_EraseFlash( u32DestFlashAddr, u32Size) )
        {
            //bResult = FALSE;
            errCode = E_USB_DL__ERASE_FLASH_FAILED;
            break;
        }

        // Copy dram to buf, then copy buf to flash
        MsOS_FlushMemory();
        while( u32Size > 0 )
        {
            u32CopySize = MIN(u32Size, DRAM_TO_FLASH_BUF);

            // Copy new data to flash_buf
            memcpy( (flash_buf), (void *)_PA2VA(u32SrcDramAddr),  u32CopySize);

            // Copy flash_buf to flash
            MDrv_FLASH_Write(u32DestFlashAddr, u32CopySize, flash_buf);

            u32SrcDramAddr += u32CopySize;
            u32DestFlashAddr += u32CopySize;
            u32Size -= u32CopySize;
        }
    }while(0);


    if( bNeedEnableWriteProtect )
    {
        MDrv_FLASH_WriteProtect(ENABLE);
    }

    if( flash_buf )
    {
        MSAPI_MEMORY_FREE(flash_buf, BUF_ID_USB_DOWNLOAD_BIN);
    }

    return errCode;
}


void MW_UsbDlBin_SearchFile_Exit(void)
{
    if( !IsStorageInUse() )
        _MW_UsbDownload_Exit();
}

EnuUsbDlErrCode MW_UsbDlBin_SearchFileAndCopyToDram(U8 u8USBPortNo, U8* pu8FileName, U32* pu32FileSize, U32 u32DramAddr, U32 u32DramSize )
{
    FileEntry fileEntry;
    U32 u32FileSize;
    U8 u8HandleNo;
    U32 u32LengthLeft = 0;


    // Search PQ bin file in USB...
    DEBUG_USB_DL_BIN( printf("MW_UsbDlBin_SearchFileAndCopyToDram()\r\n"); );


    _u8USBPortNo = u8USBPortNo;
    if( FALSE == _MW_UsbDownload_InitFileSystem() )
    {
        printf("\r\n Error: _MW_UsbDownload_InitFileSystem() failed!\r\n");
        return E_USB_DL__INIT_FILE_SYSTEM_FAILED;
    }

    if( FALSE == _MW_UsbDownload_SearchFileInRoot(pu8FileName, &fileEntry) )
    {
        printf("\r\n Error: Search file failed!\r\n");
        return E_USB_DL__SEARCH_FILE_FAILED;
    }
    else
    {
        //printf(" Search file OK~\r\n");
    }

    //MApp_UsbClearWatchDog();


    u8HandleNo = msAPI_FCtrl_FileOpen( &fileEntry, OPEN_MODE_FOR_READ );
    if( u8HandleNo == FCTRL_INVALID_FILE_HANDLE)
    {
        printf("\r\n Error: Open file failed\r\n");
        MW_UsbDlBin_SearchFile_Exit();
        return E_USB_DL__OPEN_FILE_FAILED;
    }


    // Copy file to dram ...
    *pu32FileSize = u32FileSize = msAPI_FCtrl_FileLength(u8HandleNo);
    DEBUG_USB_DL_BIN( printf(" File size: %lu\r\n", u32FileSize); );
    DEBUG_USB_DL_BIN( printf(" Dram Addr: 0x%lx\r\n", u32DramAddr); );
    DEBUG_USB_DL_BIN( printf(" Dram Size: 0x%lx\r\n", u32DramSize); );

    if( (u32FileSize == 0) || (u32FileSize > u32DramSize) )
    {
        printf("\r\n Error: File size=%lu error!\r\n", u32FileSize);
        msAPI_FCtrl_FileClose(u8HandleNo);
        MW_UsbDlBin_SearchFile_Exit();
        return E_USB_DL__FILE_SIZE_IS_ERROR;
    }

    //MApp_UsbClearWatchDog();

    DEBUG_USB_DL_BIN( printf(" Copying file from USB to DRAM\r\n"); );
    u32LengthLeft = msAPI_FCtrl_FileRead(u8HandleNo, u32DramAddr, u32FileSize);
    MsOS_FlushMemory();
    msAPI_FCtrl_FileClose(u8HandleNo);

    if( u32LengthLeft != 0 )
    {
        printf("\r\n Error: Copy file to dram failed!\r\n");
        MW_UsbDlBin_SearchFile_Exit();
        return E_USB_DL__READ_FILE_FAILED;
    }

    MW_UsbDlBin_SearchFile_Exit();

    return E_USB_DL__OK;
}

void MW_Miu_ClearProtectHitFlag(void)
{
    MDrv_WriteByteMask(0x1012DE, 0x01, 0x01); /* Clear hit bit */
    MDrv_WriteByteMask(0x1012DE, 0x00, 0x01);
}

BOOL MW_Miu_IsInvalidAccessHappened(U16 u16Flag)
{
    if( MDrv_ReadByte(0x1012DE)&0x10 )
    {
        printf("== Miu Invalid access happened!! u16Flag=%u ==\r\n", u16Flag);
        printf(" hit_protect_no=%d, hit_protect_id=%x\r\n", (MDrv_ReadByte(0x1012DE)&0xE0)>>5, (MDrv_ReadByte(0x1012DF)&0x3F) ); \
        printf("==============================================\r\n");
        MDrv_WriteByteMask(0x1012DE, 0x01, 0x01); /* Clear hit bit */
        MDrv_WriteByteMask(0x1012DE, 0x00, 0x01);
        return TRUE;
    }
    return FALSE;
}

#define ENABLE_CHECK_MIU_PROTECT    0
EnuUsbDlErrCode MW_Usb_Download_Bin(U8 u8USBPortNo, U8* pu8FileName, U32 u32DramBufAddr, U32 u32DramBufSize, U16 u16BinId)
{
    EnuUsbDlErrCode usbDlErrCode = E_USB_DL__OK;

    U32 u32FileSize = 0;

    U32 u32DramDownloadBufAddr1 = u32DramBufAddr;
    U32 u32DramDownloadBufAddr2 = u32DramDownloadBufAddr1 + 0x100000;
    U32 u32BinDataDramAddr;

    U32 u32Bin_FlashStart, u32Bin_FlashSize, u32Bin_FlashEnd;
    U32 u32FlashCopy_Start, u32FlashCopy_Size;

    BDMA_Result bdma_Result;


    printf("MW_Usb_Download_Bin(u8USBPortNo=%u, pu8FileName=%s, u16BinId=0x%X)\r\n",
                                    u8USBPortNo, pu8FileName, u16BinId);

    printf(" Dram buf: start=0x%lx, size=0x%lx\r\n", u32DramBufAddr, u32DramBufSize);

    if( u32DramBufSize < 0x200000 )
    {
        printf("\r\n Error: Dram buffer size(0x%lu) is small then 0x200000!\r\n", u32DramBufSize);
        return E_USB_DL__MEMORY_BUFFER_SIZE_ERROR;
    }

#if( ENABLE_CHECK_MIU_PROTECT )
    MW_Miu_ClearProtectHitFlag();
    MDrv_SysDelayUs(5000);

    if( MW_Miu_IsInvalidAccessHappened(__LINE__) )
    {
        printf("\r\n Error: Miu Invalid Access Happened!\r\n");
        return E_USB_DL__MEMORY_PROTECT_HIT_HAPPEN;
    }
#endif

    // Search file in USB and copy file data to dram buf-1
    printf(" Search file in USB and copy file data to dram buf-1(0x%lX)\r\n", u32DramDownloadBufAddr1);
    usbDlErrCode = MW_UsbDlBin_SearchFileAndCopyToDram( u8USBPortNo, pu8FileName, &u32FileSize, u32DramDownloadBufAddr1, 0x100000 );
    if( usbDlErrCode != E_USB_DL__OK )
    {
        //printf("\r\n Error: Search file failed!\r\n");
        return usbDlErrCode;
    }
    else
    {
        printf("  Search file OK~\r\n");
    }

#if( ENABLE_CHECK_MIU_PROTECT )
    if( MW_Miu_IsInvalidAccessHappened(__LINE__) )
    {
        printf("\r\n Error: Miu Invalid Access Happened!\r\n");
        return E_USB_DL__MEMORY_PROTECT_HIT_HAPPEN;
    }
#endif

    // Get bin info...
    printf(" Get bin info\r\n");
    {
        BININFO binInfo;
        BOOLEAN bResult;

        binInfo.B_ID = u16BinId;
        MDrv_Sys_Get_BinInfo(&binInfo, &bResult);
        if (bResult != PASS)
        {
            printf("\r\n Error: Could not find bin(0x%X) on flash.\r\n", u16BinId);
            return E_USB_DL__GET_BIN_INFO_FAILED;
        }

        u32Bin_FlashStart = binInfo.B_FAddr;
        u32Bin_FlashSize = binInfo.B_Len;
        u32Bin_FlashEnd = u32Bin_FlashStart + u32Bin_FlashSize - 1;
        printf(" Bin-Flash: start=%lu(0x%lX), size=%lu(0x%lX)\r\n",
            u32Bin_FlashStart, u32Bin_FlashStart, u32Bin_FlashSize, u32Bin_FlashSize);
    }

    // Check file and bin size...
    if( u32FileSize > u32Bin_FlashSize )
    {
        printf("\r\nError: File size(%lu) is big then bin size(%lu)!\r\n", u32FileSize, u32Bin_FlashSize);
        return E_USB_DL__FILE_SIZE_IS_BIG_THEN_BIN_SIZE;
    }

    // Check flash copy range
    {
        U32 u32FlashBankStart = u32Bin_FlashStart >> 16;
        U32 u32FlashBankEnd = u32Bin_FlashEnd >> 16;

        printf(" u32FlashBankStart=0x%X, u32FlashBankEnd=0x%X\r\n",
            u32FlashBankStart, u32FlashBankEnd );

        u32FlashCopy_Start = u32FlashBankStart << 16;
        u32FlashCopy_Size = (u32FlashBankEnd-u32FlashBankStart+1)*0x10000;
        printf("  FlashCopy: start=%lu(0x%lX), size=%lu(0x%lX)\r\n",
                    u32FlashCopy_Start, u32FlashCopy_Start, u32FlashCopy_Size, u32FlashCopy_Size);
    }

#if( ENABLE_CHECK_MIU_PROTECT )
    if( MW_Miu_IsInvalidAccessHappened(__LINE__) )
    {
        printf("\r\n Error: Miu Invalid Access Happened!\r\n");
        return E_USB_DL__MEMORY_PROTECT_HIT_HAPPEN;
    }
#endif

    // Copy flash bin data  to dram buf-2
    printf(" Copy flash bin data to dram:\r\n");
    printf("  Dram Addr-2 = 0x%lX\r\n", u32DramDownloadBufAddr2);
    bdma_Result = MApi_BDMA_Copy(u32FlashCopy_Start, u32DramDownloadBufAddr2, u32FlashCopy_Size, MIU_FLASH2SDRAM);
    if( E_BDMA_OK != bdma_Result )
    {
        printf("MApi_BDMA_Copy failed! BDMA_Result=%u\r\n", bdma_Result);
        return E_USB_DL__BDMA_FLASH_TO_DRAM_FAILED;
    }
    MsOS_FlushMemory();

#if( ENABLE_CHECK_MIU_PROTECT )
    if( MW_Miu_IsInvalidAccessHappened(__LINE__) )
    {
        printf("\r\n Error: Miu Invalid Access Happened!\r\n");
        return E_USB_DL__MEMORY_PROTECT_HIT_HAPPEN;
    }
#endif

    // Update bin data in dram by file data
    printf(" Update bin data in dram buf-2 by file data\r\n");

    u32BinDataDramAddr = u32DramDownloadBufAddr2 + (u32Bin_FlashStart&0xFFFF);

    printf("  Bin data dram addr = 0x%lX\r\n", u32BinDataDramAddr);

#if 1
    // Clear old bin data
    {
        U8 *pu8PQData;
        pu8PQData = (U8*)_PA2VA(u32BinDataDramAddr);
        memset( pu8PQData, 0, u32Bin_FlashSize );
        MsOS_FlushMemory();
    }
#endif

#if( ENABLE_CHECK_MIU_PROTECT )
    if( MW_Miu_IsInvalidAccessHappened(__LINE__) )
    {
        printf("\r\n Error: Miu Invalid Access Happened!\r\n");
        return E_USB_DL__MEMORY_PROTECT_HIT_HAPPEN;
    }
#endif

    // Copy file data from dram buf-1to dram buf-2
    bdma_Result = MApi_BDMA_Copy(u32DramDownloadBufAddr1, u32BinDataDramAddr, u32FileSize, MIU_SDRAM2SDRAM);
    if( E_BDMA_OK != bdma_Result )
    {
        printf("\r\n Error: MApi_BDMA_Copy failed! BDMA_Result=%u\r\n", bdma_Result);
        return E_USB_DL__BDMA_FLASH_TO_DRAM_FAILED;
    }
    MsOS_FlushMemory();

#if( ENABLE_CHECK_MIU_PROTECT )
    if( MW_Miu_IsInvalidAccessHappened(__LINE__) )
    {
        printf("\r\n Error: Miu Invalid Access Happened!\r\n");
        return E_USB_DL__MEMORY_PROTECT_HIT_HAPPEN;
    }
#endif

    // Copy dram data to flash
    printf(" Copy dram data to flash: DramAddr=0x%lx FlashAddr=0x%lx Size=0x%lx\r\n", u32DramDownloadBufAddr2, u32FlashCopy_Start, u32FlashCopy_Size);
    usbDlErrCode = MW_CopyDramToFlash(u32DramDownloadBufAddr2, u32FlashCopy_Start, u32FlashCopy_Size);
    if( E_USB_DL__OK != usbDlErrCode )
    {
        printf("\r\n Error: MW_CopyDramToFlash failed!\r\n");
        return usbDlErrCode;
    }

#if( ENABLE_CHECK_MIU_PROTECT )
    if( MW_Miu_IsInvalidAccessHappened(__LINE__) )
    {
        printf("\r\n Error: Miu Invalid Access Happened!\r\n");
        return E_USB_DL__MEMORY_PROTECT_HIT_HAPPEN;
    }
#endif

    printf(" Done~\r\n");

    return E_USB_DL__OK;
}


#endif

char g_sUSBFileName[] = "Test0000.bin";
U16 g_u16FileWriteCnt = 0;
char GetASCII(BYTE num)
{
    switch(num)
    {
        case 0:
            return '0';
        case 1:
            return '1';
        case 2:
            return '2';
        case 3:
            return '3';
        case 4:
            return '4';
        case 5:
            return '5';
        case 6:
            return '6';
        case 7:
            return '7';
        case 8:
            return '8';
        case 9:
            return '9';
        default:
            return '0';
    }
}

void Dump_DRAM_To_USB_FileWriteCnt_Set(U16 u16FileWriteCnt)
{
    g_u16FileWriteCnt = u16FileWriteCnt;
}

BOOLEAN Dump_DRAM_To_USB(U32 u32StartAddr, U32 u32Length)
{
    DWORD u32BaseAddr = 0;
    U16 s16FileName[16];
    U8 i=0, result=0, u8Fid;

    USB_DL_DBG(printf("StartAddr:%x\n", u32StartAddr));
    USB_DL_DBG(printf("Length:%x\n", u32Length));

#if 0//ENABLE_RTE //
    msAPI_OverDriveEnable(0); // USB & OD share the same space. Before USB F/W download, OD should be off first.
#endif
    USB_DL_DBG(printf("run usb_init..\r\n"));

    USB_Init_Configure();
    usb_init();

    u32BaseAddr = ALIGN_64(u32StartAddr) & (~(BIT31|BIT30));
    ForceDelay1ms(500);

    extern void sw_update_cb(U8 u8Percent);
    extern U32 msAPI_FCtrl_FileWrite(U8 u8HandleNo, U32 u32Buffer, U32 u32Length);
    extern void MApp_UsbSaveData_Exit(void);

    RETRY_USB_WRITE:
    MDrv_UsbDeviceConnect();
    MApp_UsbSaveData_InitFileSystem();
    _u8USBPortNo = BIT0;
    g_sUSBFileName[4] = GetASCII((BYTE)(g_u16FileWriteCnt/1000));
    g_sUSBFileName[5] = GetASCII((BYTE)((g_u16FileWriteCnt%1000)/100));
    g_sUSBFileName[6] = GetASCII((BYTE)((g_u16FileWriteCnt%100)/10));
    g_sUSBFileName[7] = GetASCII((BYTE)(g_u16FileWriteCnt%10));
    strncpy((char*)s16FileName, g_sUSBFileName,sizeof(s16FileName));
    FS_ASCII2Unicode((U8*)s16FileName);
    u8Fid = msAPI_FCtrl_FileCreate(_u8USBEnvironmentID ,s16FileName, strlen(g_sUSBFileName), CREATE_MODE_NEW_FORCE);

    i++;
    if( u8Fid == 0xFF )
    {
        if(i < 10)
        {
            ForceDelay1ms(500);
            goto RETRY_USB_WRITE;
        }
    }
    else
    {
        USB_DL_DBG(printf("Create usb file:%s OK\n", g_sUSBFileName));
        msAPI_FCtrl_FileWrite(u8Fid, (U32)(u32BaseAddr), (U32)u32Length);
        msAPI_FCtrl_FileClose(u8Fid);
        USB_DL_DBG(printf("Dump finish\n"));
        result = TRUE;
    }

    MApp_UsbSaveData_Exit();

    return result;
}


#undef MW_USBDOWNLOAD_C

