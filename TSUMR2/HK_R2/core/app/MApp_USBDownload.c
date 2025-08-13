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
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#define MAPP_USBDOWNLOAD_C

/******************************************************************************/
/*                              Header Files                                  */
/******************************************************************************/
// Global Layer
#include "MsCommon.h"
#include "sysinfo.h"
#include "imginfo.h"

#include "Debug.h"

// Driver Layer
//#include "drvMIU.h"
#include "drvCPU.h"
//#include "drvSERFLASH.h"
#include "SysInit.h"

// API Layer
//#include "apiXC.h"
#include "msAPI_Memory.h"
//#include "msAPI_BDMA.h"

// AP Layer
#include "MApp_USBDownload.h"
#include "MApp_SwUpdate.h"
#if(ENABLE_6M30_3D_PROCESS)
#include "drvUrsa6M30.h"
#include "MApp_Isp.h"
#endif
#include <string.h>
#if( ENABLE_USB_DOWNLOAD_BIN )
    #include "drvUSB.h"
    #include "MApp_DMP_Main.h"
    #include "drvPQ_Define.h"
#endif
#include "msflash.h"

// Progress bar start and end positions
#ifdef SUPPORT_AP_BIN_IN_FLASH_2
// Download two images into different flash
U8 USBUpgrade_Percent[] =
{
    // Compare: 0 ~ 10
    0,
    // Erase: 10 ~ 25
    10,
    // Write: 25 ~ 40
    25,
    // Verify: 40 ~ 50
    40,
    50,
     // Compare: 50 ~ 60
    50,
    // Erase: 60 ~ 75
    60,
    // Write: 75 ~ 90
    75,
    // Verify: 90 ~ 100
    90,
    100
};
#else
// Single image (Boot or app)
U8 USBUpgrade_Percent[] =
{
    // Check CMAC: 0 ~ 10
    0,
    // Compare: 10 ~ 20
    10,
    // Erase: 20 ~ 50
    20,
    // Write: 50 ~ 80
    50,
    // Verify: 80 ~ 100
    80,
    100
};
#endif

// Below functions are used by swupdate library; do not remove them.
void MApp_UsbClearWatchDog(void)
{
    MDrv_Sys_ClearWatchDog();
}

void MApp_UsbEnableWatchDog(void)
{
    MDrv_Sys_EnableWatchDog();
}

void MApp_UsbSetWatchDogTimer(U8 u8sec)
{
    MDrv_Sys_SetWatchDogTimer(u8sec);
}

#if 0
void MApp_UsbBDMA_Copy(U32 u32Srcaddr, U32 u32Dstaddr, U32 u32Len, U8 eType)
{
    //Leo-temp TBD
    //MApi_BDMA_Copy(u32Srcaddr, u32Dstaddr, u32Len, (MEMCOPYTYPE)eType);
}
#endif

U32 MApp_UsbVA2PA(U32 u32x)
{
    return _VA2PA(u32x);
}

U32 MApp_UsbPA2VA(U32 u32x)
{
    return _PA2VA(u32x);
}

// Memory copy type enumerate
typedef enum MEMCOPYTYPE_t
{
    MIU_FLASH2SDRAM = 0x02,   ///< 0x02:Flash to SDRAM
    MIU_SRAM2SDRAM  = 0x12,   ///< 0x12:SRAM to SDRAM
    MIU_SDRAM2SRAM  = 0x21,   ///< 0x21:SDRAM to SRAM
    MIU_SDRAM2SDRAM = 0x22,   ///< 0x22:SDRAM to SDRAM
    MIU_SDRAM02SDRAM1 = 0x34, ///< 0x34:SDRAM0 to SDRAM1
    MIU_SDRAM12SDRAM0 = 0x43, ///< 0x43:SDRAM1 to SDRAM0
    MIU_SDRAM02SDRAM0 = 0x33, ///< 0x33:SDRAM0 to SDRAM0
    MIU_SDRAM12SDRAM1 = 0x44, ///< 0x44:SDRAM1 to SDRAM1
    MIU_SDRAM2SDRAM_I = 0x55, ///< 0x55:SDRAM to SDRAM Inverse BitBlt
    MIU_FLASH2VDMCU=0x60,     ///< 0x60:Flash to VD MCU
    MIU_FLASH2DRAM_AEON=0x70, ///< 0x70:Flash to DRAM Aeon
} MEMCOPYTYPE;

//****************************************************************************
/// USB Download Init
/// @param u8PortNo IN: USB port number
/// @param ZUIDrawPercentage IN: UI draw call back function pointer
/// -@see pZUIDrawCB
/// @return void
//****************************************************************************
U8 SW_UPDATE_MODE = (U8) SW_UPDATE_MODE_PARTIAL_OAD;

void MApp_UsbDownload_Init(U8 u8PortNo, pZUIDrawCB ZUIDrawPercentage)
{
// Choose SW Update Mode: full burn or partial

#define DOWNLOAD_BUFFER_ADR_USB   ((DOWNLOAD_BUFFER_MEMORY_TYPE & MIU1) ? (DOWNLOAD_BUFFER_ADR | MIU_INTERVAL) : (DOWNLOAD_BUFFER_ADR))
#define DOWNLOAD_BUFFER_LEN_USB   DOWNLOAD_BUFFER_LEN
#define DOWNLOAD_BUFFER_TYPE_USB  MIU_SDRAM2SDRAM

#ifdef SUPPORT_AP_BIN_IN_FLASH_2
    BOOLEAN bSupportAPBINInFlash2 = TRUE;
#else
    BOOLEAN bSupportAPBINInFlash2 = FALSE;
#endif

    U8      u8FlashID = 0x0; // 0x0: flash ID0, 0x1: flash ID1
//search the bin file in the specific folder ex: "folder_name\\file_name.bin"
    U8      pu8FileName[MAX_FILENAME_LEN] = "MERGE.BIN";
    U32     u32FlashSize = 0;

#if 0
    U8      u8MIUProtectAeon_ID[2];
    U32     u32BaseAddr = 0;

    u32BaseAddr = MDrv_COPRO_GetBase();
    if(u32BaseAddr == 0xBF800000) //HK: MIPS
    {
        u8MIUProtectAeon_ID[0] = MIU_CLI_AEON_RW;
    }
    else if(u32BaseAddr == 0xA0000000) //HK: AEON
    {
        u8MIUProtectAeon_ID[0] = 0x05;
    }
    else
    {
        printf("Error> Code base error\n");
    }

    u8MIUProtectAeon_ID[1] = MIU_CLI_DMA_R;
    MDrv_MIU_Protect(1, &u8MIUProtectAeon_ID[0], (U32)DOWNLOAD_BUFFER_ADR_USB, (U32)(DOWNLOAD_BUFFER_ADR_USB + DOWNLOAD_BUFFER_LEN_USB-1), ENABLE);
#endif
#if (ENABLE_6M30_3D_PROCESS)
    //#define SW_NAME_MFC              "MFC.BIN"            // Capital
    if (MDrv_Ursa_USB_Update_GetChipType() == SWUpdateForMFC)
    {
        memcpy(pu8FileName, "MFC.BIN   ", 7);
        pu8FileName[7] = '\0';
    }
#endif

    #if (!BLOADER)
    /*
    *  Disable Some HW Engines for Downloading
    */
    {
#if 0
//leo.chou 20130904
//What drivers are supposed to be disabled during FW download?
        MS_VE_Output_Ctrl OutputCtrl;
        // disable VE
        OutputCtrl.bEnable = FALSE;
        msAPI_VE_SetOutputCtrl(&OutputCtrl);
        // disable DNR buffer
        MApi_XC_DisableInputSource(TRUE, MAIN_WINDOW);
        MW_AUD_SetSoundMute(SOUND_MUTE_ALL, E_MUTE_ON);
#else
        //MApi_XC_DisableInputSource(TRUE, MAIN_WINDOW);
#endif

    }
    #endif

    //if(!MDrv_SERFLASH_DetectSize(&u32FlashSize))
    if(!FlashDetectSize(&u32FlashSize))
    {
        printf("Error> Detect Flash Size Failed !!\n");
    }
    else
    {
#if 0
  #if (ENABLE_6M30_3D_PROCESS)
       if (MDrv_Ursa_USB_Update_GetChipType() == SWUpdateForHK)
  #endif
        u32FlashSize = u32FlashSize - 0x20000; // Data Base use the last 2 banks
#endif
        u32FlashSize <<= 20;
    }

    MW_UsbDownload_Init(u8PortNo, u8FlashID, (U32)DOWNLOAD_BUFFER_ADR_USB, (U32)DOWNLOAD_BUFFER_LEN_USB, u32FlashSize, (U8)SW_UPDATE_MODE, (U8)DOWNLOAD_BUFFER_TYPE_USB, pu8FileName, ZUIDrawPercentage, bSupportAPBINInFlash2);
}


#if( ENABLE_USB_DOWNLOAD_BIN )
extern U8 MDrv_USBGetPortEnableStatus(void);

U8 MApp_CheckUsbPortStatus(void)
{
    U8 u8PortEnStatus;
    U8 u8UsbPortNo = 0;
    BOOL bUsbStatusIsGood = FALSE;
    U32 u32Time1;


    printf(" MApp_UsbDownloadBin_CheckUsbStatus()\n");

    // Check USB status
    u8PortEnStatus = MDrv_USBGetPortEnableStatus();
    printf("  u8PortEnStatus=%u\n", u8PortEnStatus);


    // Check port 1
    if( (u8PortEnStatus & BIT0) == BIT0 )
    {
        u32Time1 = MsOS_GetSystemTime();
        while( (MsOS_Timer_DiffTimeFromNow(u32Time1) < 1000) )
        {
            if( MDrv_UsbDeviceConnect() )
            {
                u8UsbPortNo = BIT0;
                bUsbStatusIsGood = TRUE;
                break;
            }
        }
    }

    if( bUsbStatusIsGood )
    {
        return u8UsbPortNo;
    }

    if( (u8PortEnStatus & BIT1) == BIT1 )
    {
        u32Time1 = MsOS_GetSystemTime();
        while( (MsOS_Timer_DiffTimeFromNow(u32Time1) < 1000) )
        {
            if( MDrv_UsbDeviceConnect_Port2() )
            {
                u8UsbPortNo = BIT1;
                bUsbStatusIsGood = TRUE;
                break;
            }
        }
    }

    if( bUsbStatusIsGood )
    {
        return u8UsbPortNo;
    }

    return 0;
}

#if(ENABLE_PQ_BIN)
static void MApp_UsbDownloadBin_DisableSomeEngine(BOOL bDisableEngine)
{
    if( bDisableEngine )
    {
        MS_VE_Output_Ctrl OutputCtrl;

        // disable VE
        OutputCtrl.bEnable = FALSE;
        msAPI_VE_SetOutputCtrl(&OutputCtrl);

        // disable DNR buffer
        MApi_XC_DisableInputSource(TRUE, MAIN_WINDOW);

        MW_AUD_SetSoundMute(SOUND_MUTE_ALL, E_MUTE_ON);

        MApp_DMP_SetDMPStat(DMP_STATE_RESET);

    }
    else
    {

    }
}
#endif

#define DEBUG_USB_DOWNLOAD_BIN_BUF__MIU_PROTECT 0

#if( DEBUG_USB_DOWNLOAD_BIN_BUF__MIU_PROTECT )
void MW_Miu_ClearProtectHitFlag(void);
BOOL MW_Miu_IsInvalidAccessHappened(U16 u16Flag);
#endif

#if(ENABLE_PQ_BIN)
EnuUsbDlErrCode MApp_Usb_Download_PQBin(void)
{
    U8 u8UsbPortNo;
    U8 acBinFilename1[] = "Macaw12_Main.bin";
    U8 acBinFilename2[] = "Macaw12_Main_Text.bin";
    U32 u32DownloadBufferAddr;
    U32 u32DownloadBufferSize;
    EnuUsbDlErrCode usbDlErrCode = E_USB_DL__OK;
    //EnuUsbDlErrCode usbDlErrCode2 = E_USB_DL__OK;
    BOOL bUpgradeTextBin = TRUE;


  #define USB_DOWNLOAD_BIN_BUF_SIZE   0x200000

 // Check download buffer
#if defined(DOWNLOAD_BUFFER_ADR) && (DOWNLOAD_BUFFER_LEN>=0x200000)
    u32DownloadBufferAddr = DOWNLOAD_BUFFER_ADR;
    u32DownloadBufferSize = USB_DOWNLOAD_BIN_BUF_SIZE; // 2M
#else
    u32DownloadBufferAddr = 0x3C00000; // 62M
    u32DownloadBufferSize = USB_DOWNLOAD_BIN_BUF_SIZE; // 2M
#endif


    printf("\nMApp_Usb_Download_PQBin()\n");

#if 0//(ENABLE_PQ_BIN==0)
    printf(" Error: ENABLE_PQ_BIN=0!!\n");
    return E_USB_DL__FAIL;
#endif

    u8UsbPortNo = MApp_CheckUsbPortStatus();
    if( u8UsbPortNo == 0 )
    {
        printf("\n Error: Usb device is not detected!\n");
        return E_USB_DL__USB_PORT_IS_NOT_DETECTED;
    }


    // Disable Some HW Engines for Downloading
    printf(" Disable Some HW Engines for Downloading\n");
    MApp_UsbDownloadBin_DisableSomeEngine(TRUE);


    // Protect download buffer
    CHECK_MIU_PROTECT_AND_CLEAR();

    { // Usb MIU protect 0
        MS_U8 au8ProtectId[4] = {MIU_CLIENT_R2M_W, MIU_CLIENT_BDMA_RW, MIU_CLIENT_USB_UHC0_RW, 0};

        printf(" Enable MIU protect 0 for USB upgrade~\n");
        MDrv_MIU_Protect(0, au8ProtectId, u32DownloadBufferAddr, (u32DownloadBufferAddr+u32DownloadBufferSize-1), ENABLE);
        MsOS_DelayTask(5);
    }

#if(DEBUG_USB_DOWNLOAD_BIN_BUF__MIU_PROTECT)
    if( MW_Miu_IsInvalidAccessHappened(__LINE__) )
    {
        printf("\n Error: Miu Invalid Access Happened!\n");
        return E_USB_DL__MEMORY_PROTECT_HIT_HAPPEN;
    }
#endif

    usbDlErrCode = MW_Usb_Download_Bin(u8UsbPortNo, (U8*)acBinFilename1, u32DownloadBufferAddr, u32DownloadBufferSize, BIN_ID_PQ_MAIN);

    if (usbDlErrCode == E_USB_DL__OK)
    {

    }
    else
    {
        printf("\n Error: USB upgrade bin failed! ErrcCode=%u\n", usbDlErrCode);
        bUpgradeTextBin = FALSE;
    }

#if(DEBUG_USB_DOWNLOAD_BIN_BUF__MIU_PROTECT)
    if( MW_Miu_IsInvalidAccessHappened(__LINE__) )
    {
        printf("\n Error: Miu Invalid Access Happened!\n");
        while(1){}
        return E_USB_DL__MEMORY_PROTECT_HIT_HAPPEN;
    }
#endif

    if( bUpgradeTextBin )
    {
        printf("\n==================================\n");
        usbDlErrCode = MW_Usb_Download_Bin(u8UsbPortNo, (U8*)acBinFilename2, u32DownloadBufferAddr, u32DownloadBufferSize, BIN_ID_PQ_MAIN_TEXT);

        if( usbDlErrCode != E_USB_DL__OK )
        {
            printf("\n Error: USB upgrade bin failed! ErrCode=%u\n", usbDlErrCode);
        }

    #if(DEBUG_USB_DOWNLOAD_BIN_BUF__MIU_PROTECT)
        if( MW_Miu_IsInvalidAccessHappened(__LINE__) )
        {
            printf("\n Error: Miu Invalid Access Happened!\n");
            while(1){}
            return E_USB_DL__MEMORY_PROTECT_HIT_HAPPEN;
        }
    #endif
    }

    //MApp_UsbDownloadBin_DisableSomeEngine(FALSE);


    //MDrv_Sys_WholeChipReset();

   // printf("\n Chip reset failed!\n");
   // printf("\n Please reboot system~~\n");
   // while(1){}

    return usbDlErrCode;
}
#endif // #if(ENABLE_PQ_BIN)

#endif // #if( ENABLE_USB_DOWNLOAD_BIN )

#undef MAPP_USBDOWNLOAD_C

