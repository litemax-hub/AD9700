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
///////////////////////////////////////////////////////////////////////////////
//
//	File name: drvUsbMain.C
//	Version: 0.1
//	Date: 2004/9/20
//	Company: Faraday Tech. Corp.
///////////////////////////////////////////////////////////////////////////////
#if defined(MSOS_TYPE_NOS)
#include "MsVersion.h"
#include "../USBHostConfig.h"
//#include "../../include/hal_arch.h"
//#include "../../include/hal_cache.h"
//#include <stdlib.h>
//#include <stdarg.h>
#ifndef ATV_SERISE_USE
#include <stdio.h>
#endif
#include <string.h>
//#include "chipset.h"
//#include "flib.h"
//#include <MsCommon.h>
#ifndef ATV_SERISE_USE
#include "../../include/datatype.h"
#else
#include "UsbHostDataDef.h"
#endif
#include "../drvHostLib.h"
#include "../drvHost200.h"
#include "../drvUsbMain.h"
#ifndef ATV_SERISE_USE

#endif
#include "../../include/_drvUSB.h"
#include "../drvscsi.h"
#ifdef ATV_SERISE_USE
#include "common.h"
#include "uart.h"
#include "command.h"
#endif
#ifdef CERAMAL_SERISE_USE
#include "drvtimer.h"
#include "board.h"
#include "drvUTMI.h"
#else
#endif

#ifdef OnePort_OTG_EHCI
#include "MsCompiler.h"
#include "msusb.h"
#include "msDRC.h"
#include "msconfig.h"
#include "mscpu.h"
#include "drvisr.h"
#endif

#include "../../include/drvGlobal.h"

//#include "../drvUSBHost_PTP.h"
#include "MsCommon.h"
#include "MsIRQ.h"
#include "MsOS.h"
#include "board.h"

#ifdef DEVICE_ENUM_SEGMENT
U8  MDrv_Usb_Device_Enum_EX(void);
#endif
//extern void msAPI_Timer_ResetWDT(void);

#define DRV_USB_DBG(x)    x;
#define MINI_DEBUG(x)     x;

U8 gUsbStatus=USB_OK;
U8 gUsbTimeout=5;
U8 gUsbRetryCount=3;
U16 gUsbChipID = 0xFFFF;
U8 gUsbChipVersion = 0xFF;
U8 g_Port0_Switch = 0;
BOOLEAN g_Port0_AutoSwitchEnable = TRUE;
U8 g_Port0_CurntSwitch = 0;
U8 g_Port0_SwithDelay = 0;


#if 1
extern U8 gUsbDeviceState;
extern U8 gUsbDeviceState_Port2;
#else
#ifdef USB_POWER_SAVING_MODE
U8 gUsbDeviceState=POWER_SAVING;
#ifdef USBHOST2PORT
extern U8 gUsbDeviceState_Port2;
#endif
#else
U8 gUsbDeviceState=NO_DEVICE;
#endif
#endif
U8  NowIsHub;

extern U8 gSpeed;
//static U32 original_xdwin1_Base=0xFFFFFFFF; //use 0xFFFFFFFF to represent void
static U8 xdwin1_lockCount=0;
#if 0
U8 code USB_VBuf[32] = {'M', 'S', 'V', 'C', '0', '0',			    // 0, 0 fixed
                        'Z', '2',						            // Library ID
#ifdef   DTV_STANDARD_LIB
                        'A', '0', '0', '1', '4', '8',				// build number
#endif
#ifdef ATV_SERISE_USE
                        'B', '0', '0', '1', '2', '4',				// build number
#endif
#ifdef CERAMAL_SERISE_USE
                        'C', '0', '0', '1', '2', '4',				// build number
#endif

                        '0', '0', '0', '0', '0', '0', '0', '0',		// change list 46058
                        'A', '4', 'A', '0', '0', '0', '0', '0', '0',// A4: Saturn, A:LG
                        'T'};
#endif

#define USB_HOST_P1_DRV_VERSION                  /* Character String for DRV/API version             */  \
    MSIF_TAG,                           /* 'MSIF'                                           */  \
    MSIF_CLASS,                         /* '00'                                             */  \
    MSIF_CUS,                           /* 0x0000                                           */  \
    MSIF_MOD,                           /* 0x0000                                           */  \
    MSIF_CHIP,                                                                                  \
    MSIF_CPU,                                                                                   \
    {'U','S','B','1'},                  /* IP__                                             */  \
    {'0','1'},                          /* 0.0 ~ Z.Z                                        */  \
    {'8','2'},                          /* 00 ~ 99                                          */  \
    {'0','0','2','5','9','5','9','0'},  /* CL#                                              */  \
    MSIF_OS

static MSIF_Version _drv_usb_host_p1_version = {
    .DDI = { USB_HOST_P1_DRV_VERSION },
};

#ifdef OnePort_OTG_EHCI
U8 USBMode=0; //set default status=OTG device mode ,{default=0, otg=1, uhci=2}
extern U8 volatile usbUSBState;
#endif

extern U8 RecoveryFlag;
//extern BOOLEAN gDeviceFatalError;

#ifdef OnePort_OTG_EHCI
extern void MDrv_OTG_Init(void);
extern U8 usbIsUSBConfiged(void);
enum
{
    USBHostMode_disconnect = 0,
    USBHostMode_connect,
    USBDeivceMode_disconnect,
	USBDeivceMode_connect
};
#endif
BOOLEAN xfirst=FALSE;
extern void drvUSBHost_TurnOffPowerDownMode(void);
extern void drvUSBHost_TurnOnPowerDownMode(void);
extern BOOLEAN drvUSBHost_isPowerSaveModeEnable(void);
extern void drvUSBHost_UTMIInitial(void);
extern void drvUSBHost_UTMIInitial_Port2(void);
extern void UsbTurnOffPowerDownMode(void);
extern void UsbTurnOnPowerDownMode(void);

extern void drvUSBHost_TurnOffPowerDownMode(void);
extern void drvUSBHost_TurnOnPowerDownMode(void);

extern BOOLEAN drvUSBHost_isPort2Enable(void);
extern U8 drvUSBHost_PTP_Init(U8 u8UsbPort);
extern void XBYTE_OR(U32 Addr, U8 offset, U8 val);
extern void XBYTE_AND(U32 Addr, U8 offset,U8 val);
extern void XBYTE_SET(U32 Addr, U8 offset,U8 val);
extern U8 XBYTE_READ(U32 Addr, U8 offset);
extern U8 drvUSBHost_HID_Init(U8 u8UsbPort);
extern BOOLEAN Support_HID(void);
void MDrv_USBGetVIDPID(U16 *pVID, U16 *pPID);


void usb_bc_enable(U32 utmi_base, U32 bc_base, U8 enable);
void usb_bc_enable(U32 utmi_base, U32 bc_base, U8 enable)
{
	if (enable) {
		//printf("BC enable \r\n");
		writeb(readb((void *)(utmi_base+(0x1*2-1))) | 0x40, (void *)(utmi_base+(0x1*2-1)));  //IREF_PDN=1・b1. (utmi+0x01[6] )
		writeb(readb((void *)(bc_base+(0x3*2-1))) | 0x40, (void *)(bc_base+(0x3*2-1)));  // [6]= reg_host_bc_en
		writeb(readb((void *)(bc_base+(0xc*2))) | 0x40, (void *)(bc_base+(0xc*2)));  // [6]= reg_into_host_bc_sw_tri
		writew(0x0000, (void *)(bc_base));  // [15:0] = bc_ctl_ov_en
		writeb(readb((void *)(bc_base+(0xa*2))) | 0x80, (void *)(bc_base+(0xa*2)));  // [7]=reg_bc_switch_en
	}
	else {
		// disable BC
		//printf("BC disable \r\n");
		writeb(readb((void *)(bc_base+(0xc*2))) & (~0x40), (void *)(bc_base+(0xc*2)));  // [6]= reg_into_host_bc_sw_tri
		writeb(readb((void *)(bc_base+(0x3*2-1))) & (~0x40), (void *)(bc_base+(0x3*2-1)));  // [6]= reg_host_bc_en
		writeb(readb((void *)(utmi_base+(0x1*2-1))) & (~0x40), (void *)(utmi_base+(0x1*2-1)));  //IREF_PDN=1・b1. (utmi+0x01[6] )
	}
}


U8 MDrv_USB_Host_GetLibVer(const MSIF_Version **ppVersion);
U8 MDrv_USB_Host_GetLibVer(const MSIF_Version **ppVersion)
{
    if(!ppVersion)
        return 1;

    *ppVersion = &_drv_usb_host_p1_version;
    return 0;
}

U16 MDrv_USBGetChipID(void);
U16 MDrv_USBGetChipID(void)
{
#if defined(CHIP_EMERALD)
    return CHIPID_EMERALD;
#else
    if (gUsbChipID == 0xFFFF)
        gUsbChipID = MDrv_Read2Byte(0x1ecc);

    return gUsbChipID;
#endif
}

U8 MDrv_USBGetChipVersion(void);
U8 MDrv_USBGetChipVersion(void)
{
    if (gUsbChipVersion == 0xFF)
        gUsbChipVersion = MDrv_ReadByte(0x1ecf);

    return gUsbChipVersion;
}

BOOLEAN MDrv_USBGetPortAutoSwitchStatus(void);
BOOLEAN MDrv_USBGetPortAutoSwitchStatus(void)
{
    return g_Port0_AutoSwitchEnable;
}

U8 MDrv_USBGetAutoSwitchPort(void);
U8 MDrv_USBGetAutoSwitchPort(void)
{
    if((UTMI_READXBYTE(0x38)&BIT2))
    {
         return 1;
    }
    else
    {
       return 0;
    }
}

void MDrv_USBSetPortAutoSwitchStatus(BOOLEAN bEnable);
void MDrv_USBSetPortAutoSwitchStatus(BOOLEAN bEnable)
{
    if ( (CHIPID_MARIA10== MDrv_USBGetChipID()) ||
         (CHIPID_MACAW12==MDrv_USBGetChipID()) ||
         (CHIPID_JANUS2== MDrv_USBGetChipID()) ||
         (CHIPID_AMBER6== MDrv_USBGetChipID()) )
    {
        DRV_USB_DBG(printf("set USB port auto switch to %d\r\n", bEnable));
        g_Port0_AutoSwitchEnable = bEnable;
    }
}

void MDrv_USBSetPortSwitch(U8 uPort);
void MDrv_USBSetPortSwitch(U8 uPort)
{
    if ( (CHIPID_MARIA10== MDrv_USBGetChipID()) ||
         (CHIPID_MACAW12==MDrv_USBGetChipID()) ||
        (CHIPID_JANUS2== MDrv_USBGetChipID()) ||
        (CHIPID_AMBER6== MDrv_USBGetChipID()) )
    {
        if (MDrv_USBGetPortAutoSwitchStatus())
            MDrv_USBSetPortAutoSwitchStatus(FALSE);

        DRV_USB_DBG(printf("Force to set USB port switch to %d\r\n", uPort));
        if (uPort ==0)
        {
            UTMI_ANDXBYTE(0x38, ~BIT2);
            g_Port0_Switch=1;
        }
        else if (uPort ==1)
        {
            UTMI_ORXBYTE(0x38, BIT2);
             g_Port0_Switch=0;
        }

        g_Port0_SwithDelay = 3;
    }
}
#if 0
void UsbGetVerString(U8 *pVerString)
{

#if 0
    memcpy(pVerString, &USB_VBuf[0], sizeof(USB_VBuf));
#endif
}
#endif

void SetUsbTimeout(U8 x);
void SetUsbTimeout(U8 x)
{
  gUsbTimeout=x;
}

void UTMI_ORXBYTE(U8 offset,U8 val)
{
    XBYTE_OR(gUTMI_BASE, offset, val);
}

void UTMI_ANDXBYTE(U8 offset,U8 val)
{
    XBYTE_AND(gUTMI_BASE, offset, val);
}

void UTMI_SETXBYTE(U8 offset,U8 val)
{
    XBYTE_SET(gUTMI_BASE, offset, val);
}

U8 UTMI_READXBYTE(U8 offset)
{
    return XBYTE_READ(gUTMI_BASE, offset);
}

void UHC_ORXBYTE(U8 offset,U8 val)
{
    XBYTE_OR(gUHC_BASE, offset, val);
}

void UHC_ANDXBYTE(U8 offset,U8 val)
{
    XBYTE_AND(gUHC_BASE, offset, val);
}

void UHC_SETXBYTE(U8 offset,U8 val)
{
    XBYTE_SET(gUHC_BASE, offset, val);
}

U8 UHC_READXBYTE(U8 offset)
{
    return XBYTE_READ(gUHC_BASE, offset);
}

void USBC_ORXBYTE(U8 offset,U8 val)
{
    XBYTE_OR(gUSBC_BASE, offset, val);
}

void USBC_ANDXBYTE(U8 offset,U8 val)
{
    XBYTE_AND(gUSBC_BASE, offset, val);
}

void USBC_SETXBYTE(U8 offset,U8 val)
{
    XBYTE_SET(gUSBC_BASE, offset, val);
}

U8 USBC_READXBYTE(U8 offset)
{
    return XBYTE_READ(gUSBC_BASE, offset);
}

#ifdef Enable_Burning_Test
U8 buf1[512];
#if 0
// UsbTest for Performance test
U8 buf1[16384];
void UsbTest(void)
{
    U16 i,j;
    U32 idx=0,xxx;
    U8 VailLun,LunIndex=0;
    U8 /**buf1,*/ *PTestBuf;
    U32 StartTime, EndTime;
    U32 uTransLen;

    printf("\r\n USB Transfer Test\r\n",0);

//    buf1 = (U8*) msAPI_Memory_Allocate(65536, BUF_ID_USB_HOST );
    printf("buf1: %X\r\n", buf1);
    PTestBuf = KSEG02KSEG1(buf1);

    VailLun=MDrv_GET_MASS_VALID_LUN();
    printf("VailLun: %bd\r\n", VailLun);
    switch (VailLun)
    {
        case 0x01:
            LunIndex=0;
            break;
        case 0x02:
            LunIndex=1;
            break;
        case 0x04:
            LunIndex=2;
            break;
        case 0x08:
            LunIndex=3;
            break;
        case 0x10:
            LunIndex=4;
            break;
        case 0x20:
            LunIndex=5;
            break;
        case 0x40:
            LunIndex=6;
            break;
        case 0x80:
            LunIndex=7;
            break;
    }

    //printf("buf1 real addr: 0x%X\r\n", buf1);
    //printf("\r\n addr=%x",(VirtoPhyAddr((U32)buf1)>>16));
    //printf(",%x\r\n",(VirtoPhyAddr((U32)buf1)));
    printf("gSpeed: %bd\r\n", gSpeed);

    idx = 0;
    uTransLen = 10*1024*1024;
    StartTime = MsOS_GetSystemTime();
    while (uTransLen > 0)
    {
        #ifndef CERAMAL_SERISE_USE
        msAPI_Timer_ResetWDT();
        #endif

        if (MDrv_UsbBlockWriteFromMIU(LunIndex,idx,32,(U32)PTestBuf)==FALSE)
        {
            printf("\r\n write failed\r\n",0);
            break;
        }

        idx+=32;
        uTransLen -= 32*512;
    }
    EndTime = MsOS_GetSystemTime();
    printf("Time to wirte 10MB is %d ms\r\n", EndTime-StartTime);

    idx = 0;
    uTransLen = 10*1024*1024;
    StartTime = MsOS_GetSystemTime();
    while (uTransLen > 0)
    {
        #ifndef CERAMAL_SERISE_USE
        msAPI_Timer_ResetWDT();
        #endif

        if (MDrv_UsbBlockReadToMIU(LunIndex,idx,32,(U32)PTestBuf)==FALSE)
        {
            printf("\r\n read failed\r\n",0);
            break;
        }

        idx+=32;
        uTransLen -= 32*512;
    }
    EndTime = MsOS_GetSystemTime();
    printf("Time to Read 10MB is %d ms\r\n", EndTime-StartTime);

  //msAPI_Memory_Free((void*)buf1,BUF_ID_USB_HOST);
}
#else
void UsbTest(void)
{
  U16 i,j;
  U32 idx=0,xxx;
  U8 VailLun,LunIndex=0;
  //U8 *buf1;

  DRV_USB_DBG(printf("\r\n USB Burning Test\r\n"));
  //printf("maxlun :%02bx lunbyte:%02bx\r\n",maxlun,lunbyte);
  //MDrv_UsbHost_Init();

  //buf1 = (U8*) msAPI_Memory_Allocate(0x200, BUF_ID_USB_HOST );
  DRV_USB_DBG(printf("buf1: %x\r\n", (UINT)buf1));

  VailLun=MDrv_GET_MASS_VALID_LUN();
  DRV_USB_DBG(printf("VailLun: %d\r\n", VailLun));
  switch (VailLun)
  {
    case 0x01:
	         LunIndex=0;
			 break;
	case 0x02:
	         LunIndex=1;
			 break;
	case 0x04:
	         LunIndex=2;
			 break;
	case 0x08:
	         LunIndex=3;
			 break;
	case 0x10:
	         LunIndex=4;
			 break;
	case 0x20:
	         LunIndex=5;
			 break;
	case 0x40:
	         LunIndex=6;
			 break;
	  case 0x80:
	         LunIndex=7;
			 break;
  }

  //printf("buf1 real addr: 0x%X\r\n", buf1);
  //printf("\r\n addr=%x",(VirtoPhyAddr((U32)buf1)>>16));
  //printf(",%x\r\n",(VirtoPhyAddr((U32)buf1)));
  DRV_USB_DBG(printf("gSpeed: %d\r\n", gSpeed));

  while (1)
  {
    #ifndef CERAMAL_SERISE_USE
    //msAPI_Timer_ResetWDT();
    #endif

	for (i=0 ; i < 0x200 ; i++)
	{
	    buf1[i]= (i & 0xff);
	}

    idx++;
    //if ((idx & 0x3f)==0)
	{
	  DRV_USB_DBG(printf("\r\n Loop:%x",(U16)(idx>>16)));
	  DRV_USB_DBG(printf(",%x",(U16)(idx)));
	}
    //MDrv_MIU_Copy
	xxx=idx % 10000;
    if (MDrv_UsbBlockWriteFromMIU(LunIndex,50+xxx,1,(U32)VirtoPhyAddr(buf1))==FALSE)
    {
      DRV_USB_DBG(printf("\r\n write failed\r\n"));
      break;
    }
    if (MDrv_UsbBlockReadToMIU(LunIndex,50+xxx,1,(U32)VirtoPhyAddr(buf1))==FALSE)
    {
      DRV_USB_DBG(printf("\r\n read failed\r\n"));
      break;
    }
	//MsOS_DelayTask(10);
    for (j=0 ; j < 0x200 ; j++)
    {
      if (buf1[j]!= (j&0xff))
      {
        DRV_USB_DBG(printf("\r\n LBA:%d data error\r\n", (U16)(50+xxx)));
        while(1);
      }
    }
    MsOS_DelayTask(5);
  }

  //msAPI_Memory_Free((void*)buf1,BUF_ID_USB_HOST);
}
#endif
#endif
#if 0
U8 MDrv_UsbGetInterfaceClass(U8 i,U8 j)
{
  U8 tmp;
  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_SetXdataWindow1();
  #endif
  tmp=sAttachDevice->saCD[i].sInterface[j].bInterfaceClass;
  #ifdef USE_XDATA_ADDRESS_0XF000
  MDrv_USB_ReleaseXdataWindow1();
  #endif

  return tmp;
}
#endif

//====================================================================
// * Function Name: main
// * Description:
// * Input:
// * OutPut:
//====================================================================
#ifndef DEVICE_ENUM_SEGMENT
//extern	void msWriteByte( DWORD u32Reg, BYTE u8Val );
//extern	BYTE msReadByte( DWORD u32Reg );
void MDrv_UTMI_Setting(void);
void MDrv_UTMI_Setting(void)
{
	//MDrv_WriteByte(0x150302,MDrv_ReadByte(0x150302)|0x04);
	//MDrv_WriteByte(0x150303,MDrv_ReadByte(0x150303)|0x28);
	//MDrv_WriteByte(0x150303,MDrv_ReadByte(0x150303)&0xEF);
	//MDrv_WriteByte(0x150304,MDrv_ReadByte(0x150304)|0x60);
	UTMI_ORXBYTE(0x02,0x04);
	UTMI_ORXBYTE(0x03,0x28);
	UTMI_ANDXBYTE(0x03,0xEF);
	UTMI_ORXBYTE(0x04,0x60);
}

BOOLEAN MDrv_UsbHost_Init(void)
{
  BOOLEAN result=FALSE;

  DRV_USB_DBG(printf("HOST200_main\r\n"));
  xfirst=TRUE;

  MINI_DEBUG(printf("uinit\r\n"));
  if (gUsbStatus==USB_EJECT)
  {
    //printf("2430:%02bx\r\n",XBYTE[0x2430]);
    if (UHC_READXBYTE(0x30)&2)          //device connect
    {
      gUsbStatus=USB_OK;
    }
    else
      return FALSE;         //eject state
  }

    MDrv_UTMI_Setting();

    UTMI_ORXBYTE(0x06,0x40);                //Force HS TX current enable and CDR stage select
    UTMI_ANDXBYTE(0x06,~0x20);                      //clear bit 5

    UTMI_ORXBYTE(0x06,0x03);                //reset UTMI
    MsOS_DelayTask(2);
    UTMI_ANDXBYTE(0x06,0xfc);
    if ( (MDrv_USBGetChipID() == CHIPID_NEPTUNE) ||
        (MDrv_USBGetChipID() == CHIPID_ERIS) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA) ||
        (MDrv_USBGetChipID() == CHIPID_PLUTO) ||
        (MDrv_USBGetChipID() == CHIPID_TRITON) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA2) )
    {
        UTMI_SETXBYTE(0x29,0x08);
    }
    else if (MDrv_USBGetChipID()==CHIPID_EUCLID) // Euclid
    {
        UTMI_ANDXBYTE(0x29,0xF7);               //disable full speed retime
    }

#if defined(Enable_Issue_TestPacket) && !defined(ENABLE_HOST_TEST)
    UTMI_SETXBYTE(0x03,0);              //for device disconnect status bit
#else
    UTMI_SETXBYTE(0x03,0xa8);               //for device disconnect status bit
#endif
  //XBYTE[gUTMI_BASE+0x07]|=0x02;      //
  #ifdef DTV_STANDARD_LIB
   if  (MDrv_USBGetChipID()==CHIPID_NEPTUNE)            //Neptune , after U06
   {
      if (MDrv_USBGetChipVersion()>= 0x6)              //U07
        {
            UTMI_ORXBYTE(0x07, 0x02);
            UTMI_SETXBYTE(0x2c,0xc1);
            UTMI_SETXBYTE(0x2d,0x3b);//enable TX common mode,
            UTMI_ORXBYTE(0x2f, 0x0e);            //preemsis
        }
    }
    else if ((MDrv_USBGetChipID()>=CHIPID_ERIS)&&(MDrv_USBGetChipID()<=CHIPID_TITANIA))                    //Eris: 3 ,Titania: 4,
    {
            UTMI_SETXBYTE(0x2c,0xc5);
            UTMI_SETXBYTE(0x2d,0x3b);//enable TX common mode,
            UTMI_ORXBYTE(0x2f, 0x0e);            //preemsis
    }
    else if (MDrv_USBGetChipID()==CHIPID_PLUTO)          // Pluto: 5
    {
            UTMI_ORXBYTE(0x2c, 0xc5);
            UTMI_ORXBYTE(0x2d, 0x3);//enable TX common mode,
            UTMI_ORXBYTE(0x2f, 0x4a);            //preemsis
            UTMI_ORXBYTE(0x13, 0x70);
    }
    else if (MDrv_USBGetChipID()==CHIPID_TITANIA2)          // Titania 2
    {
            UTMI_ORXBYTE(0x2c, 0xc1);
            UTMI_ORXBYTE(0x2d, 0x3);//enable TX common mode,
            UTMI_ORXBYTE(0x2f, 0x4a);            //preemsis
    }
    else if (MDrv_USBGetChipID()==CHIPID_TRITON)        //Triton=6
    {
            UTMI_SETXBYTE(0x2c,0xc1);
            UTMI_SETXBYTE(0x2d,0x3b);//enable TX common mode,
            UTMI_ORXBYTE(0x2f,0x0e);            //preemsis
    }
    else if (MDrv_USBGetChipID()==CHIPID_EUCLID)          // Euclid
    {
            UTMI_ORXBYTE(0x2c, 0xc1);
            UTMI_ORXBYTE(0x2d, 0x3);//enable TX common mode,
            UTMI_ORXBYTE(0x2f, 0x4a);            //preemsis
    }
    else if ( (MDrv_USBGetChipID()==CHIPID_TITANIA3) ||          // Titania 3, Titania 4
                (MDrv_USBGetChipID()==CHIPID_TITANIA4) ||
                (MDrv_USBGetChipID()==CHIPID_TITANIA7) ||
                (MDrv_USBGetChipID()==CHIPID_TITANIA8) ||
                (MDrv_USBGetChipID()==CHIPID_TITANIA9) ||
                (MDrv_USBGetChipID()==CHIPID_TITANIA12) ||
                (MDrv_USBGetChipID()==CHIPID_TITANIA13) ||
                (MDrv_USBGetChipID()==CHIPID_JANUS) ||
                (MDrv_USBGetChipID() == CHIPID_MARIA10) ||
                (MDrv_USBGetChipID() == CHIPID_JANUS2)  ||
                (MDrv_USBGetChipID() == CHIPID_AMBER6) )
    {
            UTMI_ORXBYTE(0x2c, 0x10);  //TX-current adjust to 105%
            UTMI_ORXBYTE(0x2d, 0x02);  //Pre-emphasis enable
            UTMI_ORXBYTE(0x2f, 0x81);  //HS_TX common mode current enable (100mV);Pre-emphasis enable (10%)
            UTMI_ORXBYTE(0x09,0x81);     //UTMI RX anti-dead-lock, ISI improvement
    }
    else if( MDrv_USBGetChipID() == CHIPID_MACAW12 )
    {
            //PreEmphasis +40%, CM +20%
            UTMI_ORXBYTE(0x2c, 0x90);
            UTMI_ORXBYTE(0x2d, 0x03);
            UTMI_ORXBYTE(0x2f, 0x81);
            UTMI_ORXBYTE(0x09,0x81);     //UTMI RX anti-dead-lock, ISI improvement
    }
    else if ( MDrv_USBGetChipID() == CHIPID_AMBER1 )
    {
            //for Amber1 later 40nm
            UTMI_ORXBYTE(0x2c, 0x98);
            UTMI_ORXBYTE(0x2d, 0x02);
            UTMI_ORXBYTE(0x2e, 0x10);
            UTMI_ORXBYTE(0x2f, 0x01);
            UTMI_ORXBYTE(0x09,0x81);     //UTMI RX anti-dead-lock, ISI improvement
    }
    else  //for Agate later 40nm, same as 55nm setting 1
    {
            UTMI_ORXBYTE(0x2c, 0x10);
            UTMI_ORXBYTE(0x2d, 0x02);
            UTMI_ORXBYTE(0x2e, 0x00);
            UTMI_ORXBYTE(0x2f, 0x81);
            UTMI_ORXBYTE(0x09,0x81);     //UTMI RX anti-dead-lock, ISI improvement
    }
  #endif

  UTMI_ORXBYTE(0x08,0x08);    //bit<3> for 240's phase as 120's clock set 1, bit<4> for 240Mhz in mac 0 for faraday 1 for etron

  /* Enable Cross Point ECO 2012/03/20 */
  if ((MDrv_USBGetChipID() == CHIPID_MACAW12) ||
       (MDrv_USBGetChipID() == CHIPID_ELK) ||
       (MDrv_USBGetChipID() == CHIPID_AMBER5) ||
       (MDrv_USBGetChipID() == CHIPID_EAGLE) ||
       (MDrv_USBGetChipID() == CHIPID_EMERALD) )
  {
      UTMI_ORXBYTE(0x04,0x40);    //deglitch SE0 (low-speed cross point)
  }
  else if((MDrv_USBGetChipID() == CHIPID_JANUS2))
  {
      UTMI_ORXBYTE(0x0a,0x80);    //deglitch SE0 (low-speed cross point)
  }

  /* Enable Power Noice ECO 2012/03/20 */
  if ((MDrv_USBGetChipID() == CHIPID_JANUS2) ||
       (MDrv_USBGetChipID() == CHIPID_MACAW12) ||
       (MDrv_USBGetChipID() == CHIPID_ELK) ||
       (MDrv_USBGetChipID() == CHIPID_AMBER5) ||
       (MDrv_USBGetChipID() == CHIPID_EAGLE) ||
       (MDrv_USBGetChipID() == CHIPID_EMERALD) )
  {
      USBC_ORXBYTE(0x02,0x40);    //use eof2 to reset state machine (power noise)
  }

  /* Enable Tx/Rx Reset Clock Gatting ECO 2012/03/27 */
  if((MDrv_USBGetChipID() == CHIPID_ELK) ||
      (MDrv_USBGetChipID() == CHIPID_AMBER5) ||
      (MDrv_USBGetChipID() == CHIPID_EAGLE) ||
      (MDrv_USBGetChipID() == CHIPID_EMERALD) )
  {
      UTMI_ORXBYTE(0x04,0x20);    //hw auto deassert sw reset (tx/rx reset)
  }

  /* enable patch for the assertion of interrupt(Lose short packet interrupt) 2012/03/28 */
  if( (MDrv_USBGetChipID() == CHIPID_AMBER5) )
  {
      USBC_ORXBYTE(0x04,0x80);    //patch for the assertion of interrupt
  }

  /* enable add patch to Period_EOF1(babble problem) 2012/03/28 */
  if( (MDrv_USBGetChipID() == CHIPID_AMBER5) )
  {
      USBC_ORXBYTE(0x04,0x40);    //add patch to Period_EOF1
  }

  if (MDrv_USBGetChipID()==CHIPID_PLUTO)                   // If is Pluto
      UTMI_ORXBYTE(0x09,0x01);     //ISI improvement
  else
      UTMI_ORXBYTE(0x13,0x02);     //ISI improvement

  if ( (MDrv_USBGetChipID() == CHIPID_NEPTUNE) ||
      (MDrv_USBGetChipID() == CHIPID_ERIS) ||
      (MDrv_USBGetChipID() == CHIPID_TITANIA) ||
      (MDrv_USBGetChipID() == CHIPID_PLUTO) ||
      (MDrv_USBGetChipID() == CHIPID_TRITON) ||
      (MDrv_USBGetChipID() == CHIPID_TITANIA2) )
  {
  }
  else
  {
    UTMI_ORXBYTE(0x0b, 0x80); //TX timing select latch path
  }

  if (MDrv_USBGetChipID()==CHIPID_EUCLID) // Euclid
  {
      UTMI_SETXBYTE(0x09,0x81);//0x20;       //patch low tempture,FL meta issue and enable new FL RX engin
  }
  else
  {
      UTMI_ORXBYTE(0x09,0x60);//0x20;       //patch low tempture,FL meta issue and enable new FL RX engin
  }

  DRV_USB_DBG(printf("Host: %X\r\n", mwOTG20_Control_HOST_SPD_TYP_Rd()));
  if (mwOTG20_Control_HOST_SPD_TYP_Rd()==1)            //low speed,for HID
      UTMI_ANDXBYTE(0x09,~0x40);                      //old setting

  if ( (MDrv_USBGetChipID() == CHIPID_NEPTUNE) ||
      (MDrv_USBGetChipID() == CHIPID_ERIS) ||
      (MDrv_USBGetChipID() == CHIPID_TITANIA) ||
      (MDrv_USBGetChipID() == CHIPID_PLUTO) ||
      (MDrv_USBGetChipID() == CHIPID_TRITON) ||
      (MDrv_USBGetChipID() == CHIPID_TITANIA2) )
  {
      UTMI_ANDXBYTE(0x27,0xf3);
      UTMI_ORXBYTE(0x27,0x08);  //(1) Offset 27 (．h3AA7) bit <3:2> set 2・b10   // RX bias current => 60uA (default 40uA)

      //(2) Offset 2A (．h3AAA) bit <3:2> set 2・b11               // Squelch voltage => 100mV (default 150mV)
  #ifdef DTV_STANDARD_LIB
      UTMI_SETXBYTE(0x2a,0x07);
  #endif
  }

  if (  (MDrv_USBGetChipID() == CHIPID_NEPTUNE) ||
        (MDrv_USBGetChipID() == CHIPID_ERIS) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA) ||
        (MDrv_USBGetChipID() == CHIPID_PLUTO) ||
        (MDrv_USBGetChipID() == CHIPID_TRITON) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA2) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA3) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA4) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA7) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA8) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA9) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA12) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA13) ||
        (MDrv_USBGetChipID() == CHIPID_JANUS) ||
        (MDrv_USBGetChipID() == CHIPID_MARIA10) ||
        (MDrv_USBGetChipID() == CHIPID_MACAW12) ||
        (MDrv_USBGetChipID() == CHIPID_JANUS2) ||
        (MDrv_USBGetChipID() == CHIPID_AMBER1) ||
        (MDrv_USBGetChipID() == CHIPID_AMBER3)  ||
        (MDrv_USBGetChipID() == CHIPID_AMBER5)  ||
        (MDrv_USBGetChipID() == CHIPID_AMBER6)  ||
        (MDrv_USBGetChipID() == CHIPID_AMBER7)   ||
        (MDrv_USBGetChipID() == CHIPID_AMETHYST) )
    {
        UTMI_ORXBYTE(0x15,0x20); //HOST CHIRP Detect
    }
    else
    {
        UTMI_ORXBYTE(0x15,0x60); // change to 55 interface (bit6)
    }

  //(3) Offset 2D (．h3AAD) bit <5:3> set 3・b111           // HS_RTERM bias current 5/6
  //XBYTE[0x128a]=0x87;            //change UHC priority
  //XBYTE[0x128d]&=0x0f;                //clear bit 12~15
  //XBYTE[0x128e]|=0xf;            //upgrade UHC priority, set bit 0~3

  UTMI_ORXBYTE(0x09,0x80);
#if CHIP_ID == MST9U3
  UTMI_ORXBYTE(0x23,0x20);
#endif
#ifdef UHC_PREAMBLE_PATCH
  //Preamble setting
  UTMI_ORXBYTE(0x3f,0x80);
  USBC_ORXBYTE(0x0f,0x08);
  USBC_ORXBYTE(0x10,0x01);
  USBC_ORXBYTE(0x10,0x02);
  USBC_ORXBYTE(0x10,0x04);
  UTMI_ORXBYTE(0x03,0x04);
  UTMI_ANDXBYTE(0x02,~0x02);
#endif
  //disable USBBC
  usb_bc_enable(gUTMI_BASE, BC_Base, false);


	//HW ECO
	/* UHC speed type report should be reset by device disconnection */
	USBC_ORXBYTE(0x20, 0x01);

	/* Port Change Detect (PCD) is triggered by babble.
	 * Pulse trigger will not hang this condition.
	 */
	USBC_ORXBYTE(0x20, 0x02);

	/* generation of hhc_reset_u */
	USBC_ORXBYTE(0x20, 0x04);

	/* do SRAM clock gating automatically to save power */
	USBC_ANDXBYTE(0x20, ~0x10);

  //MDrv_Sys_SetXdataWindow1Base(USB_BUFFER_START_ADR_4K_ALIGN>>12);        //switch window 1
  gUsbStatus=USB_OK;
 // gUsbTimeout=3;  //set at flib_Host20_Enumerate()
  RecoveryFlag=0;             //initialize recovery flag
  gUsbDeviceState=USB11_DEVICE;           //1.1 at first

  //gDeviceFatalError=FALSE;
  if(flib_OTGH_Init(0))
    result =TRUE;

  MsOS_DelayTask(500);
  return result;

}
#else // support device enumeration dividable
BOOLEAN _MDrv_UsbHost_Init0(void)
{
  //BOOLEAN result=FALSE;

  DRV_USB_DBG(printf("HOST200_main\r\n"));
  xfirst=TRUE;

  MINI_DEBUG(printf("uinit\r\n"));
  if (gUsbStatus==USB_EJECT)
  {
    //printf("2430:%02bx\r\n",XBYTE[0x2430]);
    if (UHC_READXBYTE(0x30)&2)          //device connect
    {
      gUsbStatus=USB_OK;
    }
    else
      return FALSE;         //eject state
  }


    UTMI_ORXBYTE(0x06,0x40);                //Force HS TX current enable and CDR stage select
    UTMI_ANDXBYTE(0x06,~0x20);                      //clear bit 5

    UTMI_ORXBYTE(0x06,0x03);                //reset UTMI
    MsOS_DelayTask(2);
    UTMI_ANDXBYTE(0x06,0xfc);

    if ( (MDrv_USBGetChipID() == CHIPID_NEPTUNE) ||
        (MDrv_USBGetChipID() == CHIPID_ERIS) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA) ||
        (MDrv_USBGetChipID() == CHIPID_PLUTO) ||
        (MDrv_USBGetChipID() == CHIPID_TRITON) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA2) )
    {
        UTMI_SETXBYTE(0x29,0x08);
    }
    else if (MDrv_USBGetChipID()==CHIPID_EUCLID) // Euclid
    {
        UTMI_ANDXBYTE(0x29,0xF7);               //disable full speed retime
    }

#if defined(Enable_Issue_TestPacket) && !defined(ENABLE_HOST_TEST)
    UTMI_SETXBYTE(0x03,0);              //for device disconnect status bit
#else
    UTMI_SETXBYTE(0x03,0xa8);               //for device disconnect status bit
#endif
  //XBYTE[gUTMI_BASE+0x07]|=0x02;      //
  #ifdef DTV_STANDARD_LIB
   if  (MDrv_USBGetChipID()==CHIPID_NEPTUNE)            //Neptune , after U06
   {
      if (MDrv_USBGetChipVersion()>= 0x6)              //U07
        {
            UTMI_ORXBYTE(0x07, 0x02);
            UTMI_SETXBYTE(0x2c,0xc1);
            UTMI_SETXBYTE(0x2d,0x3b);//enable TX common mode,
            UTMI_ORXBYTE(0x2f, 0x0e);            //preemsis
        }
    }
    else if ((MDrv_USBGetChipID()>=CHIPID_ERIS)&&(MDrv_USBGetChipID()<=CHIPID_TITANIA))                    //Eris: 3 ,Titania: 4,
    {
            UTMI_SETXBYTE(0x2c,0xc5);
            UTMI_SETXBYTE(0x2d,0x3b);//enable TX common mode,
            UTMI_ORXBYTE(0x2f, 0x0e);            //preemsis
    }
    else if (MDrv_USBGetChipID()==CHIPID_PLUTO)          // Pluto: 5
    {
            UTMI_ORXBYTE(0x2c, 0xc5);
            UTMI_ORXBYTE(0x2d, 0x3);//enable TX common mode,
            UTMI_ORXBYTE(0x2f, 0x4a);            //preemsis
            UTMI_ORXBYTE(0x13, 0x70);
    }
    else if (MDrv_USBGetChipID()==CHIPID_TITANIA2)          // Titania 2
    {
            UTMI_ORXBYTE(0x2c, 0xc1);
            UTMI_ORXBYTE(0x2d, 0x3);//enable TX common mode,
            UTMI_ORXBYTE(0x2f, 0x4a);            //preemsis
    }
    else if (MDrv_USBGetChipID()==CHIPID_TRITON)        //Triton=6
    {
            UTMI_SETXBYTE(0x2c,0xc1);
            UTMI_SETXBYTE(0x2d,0x3b);//enable TX common mode,
            UTMI_ORXBYTE(0x2f,0x0e);            //preemsis
    }
    else if (MDrv_USBGetChipID()==CHIPID_EUCLID)          // Euclid
    {
            UTMI_ORXBYTE(0x2c, 0xc1);
            UTMI_ORXBYTE(0x2d, 0x3);//enable TX common mode,
            UTMI_ORXBYTE(0x2f, 0x4a);            //preemsis
    }
    else if ( (MDrv_USBGetChipID()==CHIPID_TITANIA3) ||          // Titania 3, Titania 4
                (MDrv_USBGetChipID()==CHIPID_TITANIA4) ||
                (MDrv_USBGetChipID()==CHIPID_TITANIA7) ||
                (MDrv_USBGetChipID()==CHIPID_TITANIA8) ||
                (MDrv_USBGetChipID()==CHIPID_TITANIA9) ||
                (MDrv_USBGetChipID()==CHIPID_TITANIA12) ||
                (MDrv_USBGetChipID()==CHIPID_TITANIA13) ||
                (MDrv_USBGetChipID()==CHIPID_JANUS) ||
                (MDrv_USBGetChipID() == CHIPID_MARIA10) ||
                (MDrv_USBGetChipID() == CHIPID_JANUS2)  ||
                (MDrv_USBGetChipID() == CHIPID_AMBER6) )
    {
            UTMI_ORXBYTE(0x2c, 0x10);  //TX-current adjust to 105%
            UTMI_ORXBYTE(0x2d, 0x02);  //Pre-emphasis enable
            UTMI_ORXBYTE(0x2f, 0x81);  //HS_TX common mode current enable (100mV);Pre-emphasis enable (10%)
            UTMI_ORXBYTE(0x09,0x81);     //UTMI RX anti-dead-lock, ISI improvement
    }
    else if ( MDrv_USBGetChipID() == CHIPID_MACAW12 )
    {
            //PreEmphasis +40%, CM +20%
            UTMI_ORXBYTE(0x2c, 0x90);
            UTMI_ORXBYTE(0x2d, 0x03);
            UTMI_ORXBYTE(0x2f, 0x81);
            UTMI_ORXBYTE(0x09,0x81);     //UTMI RX anti-dead-lock, ISI improvement
    }
    else  //for Amber1 later 40nm
    {
            UTMI_ORXBYTE(0x2c, 0x98);
            UTMI_ORXBYTE(0x2d, 0x02);
            UTMI_ORXBYTE(0x2e, 0x10);
            UTMI_ORXBYTE(0x2f, 0x01);
            UTMI_ORXBYTE(0x09,0x81);     //UTMI RX anti-dead-lock, ISI improvement
    }
  #endif

  //for Edison later 240M clock enhance, early chip will ignore this 2 bits
  UTMI_ORXBYTE(0x08,0x18);    //bit<3> for 240's phase as 120's clock set 1, bit<4> for 240Mhz in mac 1 for faraday 0 for etron

  /* Enable Cross Point ECO 2012/03/20 */
  if ((MDrv_USBGetChipID() == CHIPID_MACAW12) ||
       (MDrv_USBGetChipID() == CHIPID_ELK) ||
       (MDrv_USBGetChipID() == CHIPID_AMBER5) ||
       (MDrv_USBGetChipID() == CHIPID_EAGLE) ||
       (MDrv_USBGetChipID() == CHIPID_EMERALD) )
  {
      UTMI_ORXBYTE(0x04,0x40);    //deglitch SE0 (low-speed cross point)
  }
  else if((MDrv_USBGetChipID() == CHIPID_JANUS2))
  {
      UTMI_ORXBYTE(0x0a,0x80);    //deglitch SE0 (low-speed cross point)
  }

  /* Enable Power Noice ECO 2012/03/20 */
  if ((MDrv_USBGetChipID() == CHIPID_JANUS2) ||
       (MDrv_USBGetChipID() == CHIPID_MACAW12) ||
       (MDrv_USBGetChipID() == CHIPID_ELK) ||
       (MDrv_USBGetChipID() == CHIPID_AMBER5) ||
       (MDrv_USBGetChipID() == CHIPID_EAGLE) ||
       (MDrv_USBGetChipID() == CHIPID_EMERALD) )
  {
      USBC_ORXBYTE(0x02,0x40);    //use eof2 to reset state machine (power noise)
  }

  /* Enable Tx/Rx Reset Clock Gatting ECO 2012/03/27 */
  if((MDrv_USBGetChipID() == CHIPID_ELK) ||
      (MDrv_USBGetChipID() == CHIPID_AMBER5) ||
      (MDrv_USBGetChipID() == CHIPID_EAGLE) ||
      (MDrv_USBGetChipID() == CHIPID_EMERALD) )
  {
      UTMI_ORXBYTE(0x04,0x20);    //hw auto deassert sw reset (tx/rx reset)
  }

  /* enable patch for the assertion of interrupt(Lose short packet interrupt) 2012/03/28 */
  if( (MDrv_USBGetChipID() == CHIPID_AMBER5) )
  {
      USBC_ORXBYTE(0x04,0x80);    //patch for the assertion of interrupt
  }

  /* enable add patch to Period_EOF1(babble problem) 2012/03/28 */
  if( (MDrv_USBGetChipID() == CHIPID_AMBER5) )
  {
      USBC_ORXBYTE(0x04,0x40);    //add patch to Period_EOF1s
  }

  if (MDrv_USBGetChipID()==CHIPID_PLUTO)                   // If is Pluto
      UTMI_ORXBYTE(0x09,0x01);     //ISI improvement
  else
      UTMI_ORXBYTE(0x13,0x02);     //ISI improvement

  if ( (MDrv_USBGetChipID() == CHIPID_NEPTUNE) ||
      (MDrv_USBGetChipID() == CHIPID_ERIS) ||
      (MDrv_USBGetChipID() == CHIPID_TITANIA) ||
      (MDrv_USBGetChipID() == CHIPID_PLUTO) ||
      (MDrv_USBGetChipID() == CHIPID_TRITON) ||
      (MDrv_USBGetChipID() == CHIPID_TITANIA2) )
  {
  }
  else
  {
    UTMI_ORXBYTE(0x0b, 0x80); //TX timing select latch path
  }

  if (MDrv_USBGetChipID()==CHIPID_EUCLID) // Euclid
  {
      UTMI_SETXBYTE(0x09,0x81);//0x20;       //patch low tempture,FL meta issue and enable new FL RX engin
  }
  else
  {
      UTMI_ORXBYTE(0x09,0x60);//0x20;       //patch low tempture,FL meta issue and enable new FL RX engin
  }

  DRV_USB_DBG(printf("Host: %X\r\n", mwOTG20_Control_HOST_SPD_TYP_Rd()));
  if (mwOTG20_Control_HOST_SPD_TYP_Rd()==1)            //low speed,for HID
      UTMI_ANDXBYTE(0x09,~0x40);                      //old setting

  if ( (MDrv_USBGetChipID() == CHIPID_NEPTUNE) ||
      (MDrv_USBGetChipID() == CHIPID_ERIS) ||
      (MDrv_USBGetChipID() == CHIPID_TITANIA) ||
      (MDrv_USBGetChipID() == CHIPID_PLUTO) ||
      (MDrv_USBGetChipID() == CHIPID_TRITON) ||
      (MDrv_USBGetChipID() == CHIPID_TITANIA2) )
  {
      UTMI_ANDXBYTE(0x27,0xf3);
      UTMI_ORXBYTE(0x27,0x08);  //(1) Offset 27 (．h3AA7) bit <3:2> set 2・b10   // RX bias current => 60uA (default 40uA)

      //(2) Offset 2A (．h3AAA) bit <3:2> set 2・b11               // Squelch voltage => 100mV (default 150mV)
  #ifdef DTV_STANDARD_LIB
      UTMI_SETXBYTE(0x2a,0x07);
  #endif
  }

  if (  (MDrv_USBGetChipID() == CHIPID_NEPTUNE) ||
        (MDrv_USBGetChipID() == CHIPID_ERIS) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA) ||
        (MDrv_USBGetChipID() == CHIPID_PLUTO) ||
        (MDrv_USBGetChipID() == CHIPID_TRITON) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA2) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA3) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA4) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA7) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA8) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA9) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA12) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA13) ||
        (MDrv_USBGetChipID() == CHIPID_JANUS) ||
        (MDrv_USBGetChipID() == CHIPID_MARIA10) ||
        (MDrv_USBGetChipID() == CHIPID_MACAW12) ||
        (MDrv_USBGetChipID() == CHIPID_JANUS2) ||
        (MDrv_USBGetChipID() == CHIPID_AMBER1) ||
        (MDrv_USBGetChipID() == CHIPID_AMBER3)  ||
        (MDrv_USBGetChipID() == CHIPID_AMBER5)  ||
        (MDrv_USBGetChipID() == CHIPID_AMBER6)  ||
        (MDrv_USBGetChipID() == CHIPID_AMBER7)   ||
        (MDrv_USBGetChipID() == CHIPID_AMETHYST) )
    {
        UTMI_ORXBYTE(0x15,0x20); //HOST CHIRP Detect
    }
    else
    {
        UTMI_ORXBYTE(0x15,0x60); // change to 55 interface (bit6)
    }

  //(3) Offset 2D (．h3AAD) bit <5:3> set 3・b111           // HS_RTERM bias current 5/6
  //XBYTE[0x128a]=0x87;            //change UHC priority
  //XBYTE[0x128d]&=0x0f;                //clear bit 12~15
  //XBYTE[0x128e]|=0xf;            //upgrade UHC priority, set bit 0~3
  return TRUE;
}
  //MsOS_DelayTask(500);

BOOLEAN _MDrv_UsbHost_Init1(void)
{
  BOOLEAN result=FALSE;

  //MDrv_Sys_SetXdataWindow1Base(USB_BUFFER_START_ADR_4K_ALIGN>>12);        //switch window 1
  gUsbStatus=USB_OK;
 // gUsbTimeout=3;  //set at flib_Host20_Enumerate()
  RecoveryFlag=0;             //initialize recovery flag
  gUsbDeviceState=USB11_DEVICE;           //1.1 at first

  //gDeviceFatalError=FALSE;
  if(flib_OTGH_Init(0))
    result =TRUE;

  return result;

}
BOOLEAN MDrv_UsbHost_Init(void)
{
    BOOLEAN result;

    result = _MDrv_UsbHost_Init0();
    if (result == FALSE)
        return FALSE;

    MsOS_DelayTask(500);
    return(_MDrv_UsbHost_Init1());
}
#endif
BOOLEAN MDrv_UsbHost_Init_Enum(void)
{
  BOOLEAN result;
  result=MDrv_Usb_Device_Enum();
  return result;
}
U8 MDrv_UsbGetMaxLUNCount(void)
{
  if (gUsbDeviceState==BAD_DEVICE)
    return 0;

  if (mwHost20_PORTSC_ConnectStatus_Rd()==0)
    return 0;
  else return (Mass_stor_us1.max_lun+1);
}
U8 MDrv_GET_MASS_MAX_LUN(void)
{
  //printf("Get max lun func:%02bx\r\n",Mass_stor_us1.max_lun);
  if (mwHost20_PORTSC_ConnectStatus_Rd()==0)
    return 0;
  else return Mass_stor_us1.max_lun;
}

extern void GetValidLun(void);

#ifdef ATV_SERISE_USE
void DisableINT(void)
{
  XBYTE[0x2B03]|=0x20;
  XBYTE[0x2B18]|=0x80;
  XBYTE[0x2B19]|=0x04;
  XBYTE[0x2B1A]|=0x80;
}
void EnableINT(void)
{
  XBYTE[0x2B03]&=0xDF;
  XBYTE[0x2B18]&=0x7F;
  XBYTE[0x2B19]&=0xFB;
  XBYTE[0x2B1A]&=0x7F;
}
#endif
extern U8 ValidLunNum;
U8 MDrv_GET_VALID_LUN_NUM(void);
U8 MDrv_GET_VALID_LUN_NUM(void)
{
    return ValidLunNum;
}
U8 MDrv_GET_MASS_VALID_LUN(void)
{
  U8 LunMatrix=0,i;
  struct LUN_Device* LunDevice;
  //printf("Get valid lun func\r\n");
  if (mwHost20_PORTSC_ConnectStatus_Rd()==0)
    return USB_NOT_RESPONSE;               //device is not connected

  if (gUsbDeviceState==BAD_DEVICE) return 0;

  if (gSpeed==1)    return 0;           //low speed device

  //if (gDeviceFatalError) return USB_NOT_RESPONSE;
  if (gUsbStatus==USB_EJECT) return USB_NOT_RESPONSE;


  LunDevice = Mass_stor_us1.Mass_stor_device;
  if (sAttachDevice->saCD[0].sInterface[0].bInterfaceClass != USB_INTERFACE_CLASS_MSD)
  {
    DRV_USB_DBG(printf(" LUN structure not initialized!!\r\n"));
    return USB_NOT_RESPONSE;
  }

  #ifdef ATV_SERISE_USE
    #ifdef Enable_Close_INT
    //XBYTE[0x2B00]|=0x02;
    DisableINT();
    #endif
  #endif
  GetValidLun();
  #ifdef ATV_SERISE_USE
    #ifdef Enable_Close_INT
    //XBYTE[0x2B00]&=0xFD;
    EnableINT();
    #endif
  #endif

  if (gUsbStatus==USB_TIMEOUT)
  {
    #ifdef USE_XDATA_ADDRESS_0XF000
    MDrv_USB_ReleaseXdataWindow1();
    #endif
    return USB_NOT_RESPONSE;            //USB DEVICE not responding
  }

  for (i=0; i <= Mass_stor_us1.max_lun ; i++)
  {
    LunMatrix=LunMatrix<<1 ;
	//printf("bDeviceReady:%02bx\r\n",LunDevice[Mass_stor_us1.max_lun-i].bDeviceReady);
 	if (LunDevice[Mass_stor_us1.max_lun-i].bDeviceReady == TRUE)
 	{
	  LunMatrix|=1;
 	}
  }

  return LunMatrix;
}
U32 USB_BUFFER_START_ADR_4K_ALIGN_Var;
void MDrv_USB_Init(U32 USBAdr)
{
  //gProjectCode=ProjectCode;
  //gDeviceFatalError=FALSE;

  USB_BUFFER_START_ADR_4K_ALIGN_Var=USBAdr;

  gUsbRetryCount=3;

  if (USB_BUFFER_START_ADR_4K_ALIGN_Var % 4096 != 0)
  {
    DRV_USB_DBG( printf("Error USB Starting address is not 4K alignmented\r\n"));
  }

  if (!drvUSBHost_isPowerSaveModeEnable())
  {
    mbFUSBH200_VBUS_ON_Set();

    if ( (MDrv_USBGetChipID()==CHIPID_NEPTUNE) ||  //Neptune
         (MDrv_USBGetChipID()==CHIPID_ERIS) ||  //Eris
         (MDrv_USBGetChipID()==CHIPID_TITANIA) ||  //Titania
         (MDrv_USBGetChipID()==CHIPID_PLUTO) ||  //Pluto
         (MDrv_USBGetChipID()==CHIPID_TRITON) ||  //Triton
         (MDrv_USBGetChipID()==CHIPID_TITANIA2) )   // Titania 2
    {
        UHC_ANDXBYTE(0x34,0xBF); //set suspend
        UHC_ORXBYTE(0x34,0x40); //clr suspend
        MsOS_DelayTask(2);
        UTMI_ORXBYTE(0x00,0x01);		// override mode enable for power down control
        UTMI_ORXBYTE(0x01,0x40);	// enable IREF power down
        UTMI_ORXBYTE(0x01,0x02);   // enable PLL power down
        UTMI_ANDXBYTE(0x01,0xFD);   // disable PLL power down
    }

    mbHost20_USBCMD_HCReset_Set();
  }

}

U8  MDrv_GetUsbDeviceStatus(void)
{
  return gUsbDeviceState;
}

void MDrv_SetUsbDeviceStatus(U8 status)
{
  gUsbDeviceState = status;
}

void  MDrv_ClearUsbDeviceStatus(void)
{
  //if (gUsbDeviceState==BAD_DEVICE)
  //{
  //  printf("Clear bad device\r\n");
  //  gUsbDeviceState=USB11_DEVICE;
  //}
}
void ResetUsbHardware(void);
void ResetUsbHardware(void)
{
  UTMI_ORXBYTE(0x06,0x03);        //reset UTMI
  UTMI_ANDXBYTE(0x06,0xfc);
  mbHost20_USBCMD_HCReset_Set();
  //MsOS_DelayTask(200);
}
UINT32 UsbStartTime=0;
UINT32 UsbPowerSavingTimer=0;
UINT32 UsbPortSwitchTimer=0;


#if 1
void UsbPort0UTMIInitial(void);
void UsbPort0UTMIInitial(void)
{
   mbFUSBH200_VBUS_ON_Set();
   UHC_ANDXBYTE(0x34,0xBF); //set suspend
   UHC_ORXBYTE(0x34,0x40); //clr suspend
   MsOS_DelayTask(2);
   UTMI_ORXBYTE(0x00,0x01);		// override mode enable for power down control
   UTMI_ORXBYTE(0x01,0x40);	// enable IREF power down
   UTMI_ORXBYTE(0x01,0x02);   // enable PLL power down
   UTMI_ANDXBYTE(0x01,0xFD);   // disable PLL power down
   UTMI_ANDXBYTE(0x00,~0x01);		// override mode enable for power down control

   mbHost20_USBCMD_HCReset_Set();
}
#else
#ifdef USB_POWER_SAVING_MODE
void UsbTurnOffPowerDownMode(void)
{
    //Power On
    XBYTE[gUTMI_BASE+8]&=0x7F;    //PD_BG_CURRENT
    XBYTE[gUTMI_BASE+0x2c]|=0x04;    //Switch current from DVI

    //Port0
    XBYTE[gUTMI_BASE+1]=0x6f;
    XBYTE[gUTMI_BASE]=0xc4;
    XBYTE[gUTMI_BASE+0x11]=0x00;
    XBYTE[gUTMI_BASE+0x10]=0x00;
    #ifdef USBHOST2PORT
    //Port1
    XBYTE[gUTMI2_BASE+1]=0x6f;
    XBYTE[gUTMI2_BASE]=0xc4;
    XBYTE[gUTMI2_BASE+0x11]=0x00;
    XBYTE[gUTMI2_BASE+0x10]=0x00;
    //Power On End
    #endif
    //open port 0
    XBYTE[gUTMI_BASE+1]=0x00;
    XBYTE[gUTMI_BASE]=0x00;
    XBYTE[gUSBC_BASE]|=0x08; //disable init suspend state
    XBYTE[gUSBC_BASE]|=0x20; //Enable UHC XIU
    MsOS_DelayTask(3);
    XBYTE[gUSBC_BASE+2]|=0x01;  //UHC select enable
    //end

    // Power On USB UHC0
    XBYTE[gUHC_BASE+0x34] |= 0x40;

    MsOS_DelayTask(2);
    XBYTE[gUHC_BASE+0x40] &=~0x10;

    #ifdef USBHOST2PORT
    //open port 1
    XBYTE[gUTMI2_BASE]=0x00;   //disable power down over write
    XBYTE[gUTMI2_BASE+1]=0x00;   //disable power down over write high byte
    XBYTE[gUSBC_BASE]|=0x08;  //disable init suspend state
    XBYTE[gUSBC_BASE]|=0x20;  //Enable UHC XIU
    MsOS_DelayTask(3);
    XBYTE[gUSBC_BASE+2]|=0x01;  //UHC select enable
    //end

    // Power On USB UHC1
    XBYTE[UHC2_BASE+0x34] |= 0x40;
    MsOS_DelayTask(2);
    XBYTE[UHC2_BASE+0x40] &=~0x10;
    #endif
}

void UsbTurnOnPowerDownMode(void)
{
    MsOS_DelayTask(2);

   XBYTE[gUSBC_BASE]&=0xD0; //Disable Port0 UHC XIU
   #ifdef USBHOST2PORT
   XBYTE[gUSBC2_BASE]&=0xD0; //Disable Port1 UHC XIU
   #endif
   XBYTE[gUTMI_BASE+8]|=0x80;    //PD_BG_CURRENT
   XBYTE[gUTMI_BASE+0x2c]&=0xFB;    //Switch current from REXT, not DVI

   //Port 0
   XBYTE[gUTMI_BASE+0x11]=0x88;     //bit<15>:HS_TX_OVERRIDE, bit<11>:FL_TX_OVERRIDE
   XBYTE[gUTMI_BASE+0x10]=0x00;
   XBYTE[gUTMI_BASE+3]=0x90;     //bit<15>:reg_hs_rterm_pdn, bit<7>:reg_bond_sel
   XBYTE[gUTMI_BASE+2]=0x80;
   XBYTE[gUTMI_BASE+1]=0xef;     //bit<15>:reg_pdn
   XBYTE[gUTMI_BASE]=0xc7;
   #ifdef USBHOST2PORT
   //Port 1
   XBYTE[gUTMI2_BASE+0x11]=0x88;
   XBYTE[gUTMI2_BASE+0x10]=0x00;
   XBYTE[gUTMI2_BASE+3]=0x90;
   XBYTE[gUTMI2_BASE+2]=0x80;
   XBYTE[gUTMI2_BASE+1]=0xef;
   XBYTE[gUTMI2_BASE]=0xc7;
   #endif
}

void UsbPort0UTMIInitial(void)
{
   mbFUSBH200_VBUS_ON_Set();
   UHC_XBYTE(0x34)&=0xBF; //set suspend
   UHC_XBYTE(0x34)|=0x40; //clr suspend
   MsOS_DelayTask(2);
   XBYTE[gUTMI_BASE+0x00]|=0x01;		// override mode enable for power down control
   XBYTE[gUTMI_BASE+0x01]|=0x40;	// enable IREF power down
   XBYTE[gUTMI_BASE+0x01]|=0x02;   // enable PLL power down
   XBYTE[gUTMI_BASE+0x01]&=0xFD;   // disable PLL power down
   XBYTE[gUTMI_BASE+0x00]&=~0x01;		// override mode enable for power down control

   mbHost20_USBCMD_HCReset_Set();
}
extern void UsbPort1UTMIInitial(void);
#endif
#endif

#ifdef Enable_Issue_TestPacketByHW
BOOLEAN isInitForTestPkt = TRUE;
void MDrv_UsbSendTestPacket(void);
#endif

BOOLEAN MDrv_UsbDeviceConnect(void)
{
  /*
  if (gDeviceFatalError)
  {
    gDeviceFatalError=FALSE;
    ResetUsbHardware();
    return FALSE;
  }*/
#ifdef Enable_Issue_TestPacketByHW
    if (isInitForTestPkt)
    {
        isInitForTestPkt = FALSE;
        MDrv_UsbSendTestPacket();
    }
    return FALSE;
#endif

  #if 1
  if (drvUSBHost_isPowerSaveModeEnable())
  {
    if (gUsbDeviceState==WAIT_INIT)
    {
        if (MsOS_Timer_DiffTimeFromNow(UsbPowerSavingTimer) < 300 )
       {
          return FALSE;
       }
    }
    if (gUsbDeviceState==POWER_SAVING)
    {
#ifdef ENABLE_HOST_TEST
       if (USBC_READXBYTE(8)&0xC0)
#else
       if (USBC_READXBYTE(8)&0x40)
#endif
       {
            drvUSBHost_TurnOffPowerDownMode();

            UsbPowerSavingTimer = MsOS_GetSystemTime();
            gUsbDeviceState=WAIT_INIT;
            return FALSE;
       }
       else
       {
           return FALSE;
       }
    }
  }
  #else
  #ifdef USB_POWER_SAVING_MODE
  #ifdef USBHOST2PORT
  if (gUsbDeviceState_Port2==WAIT_INIT)
      return FALSE;
  #endif
  if (gUsbDeviceState==WAIT_INIT)
  {
      if (MsOS_Timer_DiffTimeFromNow(UsbPowerSavingTimer) < 300 )
          return FALSE;
  }
  if (gUsbDeviceState==POWER_SAVING)
  {
     if (XBYTE[gUSBC_BASE+8]&0x40)
     {
         UsbTurnOffPowerDownMode();
         UsbPort0UTMIInitial();
         #ifdef USBHOST2PORT
         UsbPort1UTMIInitial();
         if (gUsbDeviceState_Port2==POWER_SAVING)
             gUsbDeviceState_Port2=NO_DEVICE;
         #endif
         UsbPowerSavingTimer = MsOS_GetSystemTime();
         gUsbDeviceState=WAIT_INIT;
         return FALSE;
         //MsOS_DelayTask(300);
     }
     else
     {
         return FALSE;
     }
  }
  #endif
  #endif
  if (gUsbStatus==USB_EJECT)
  {
    if (UHC_READXBYTE(0x30)&2)
    {
      gUsbStatus=USB_OK;
    }
    else
     {
       return FALSE;         //eject state
     }
  }

  if (g_Port0_SwithDelay > 0)
  {
      //printf("Return false for switch USB port %d\r\n", g_Port0_SwithDelay);
      g_Port0_SwithDelay--;
      return FALSE;
  }

  //FUSBH200_Driver_VBUS();             //make sure device is connected , then turn on VBUS
  if (mwHost20_PORTSC_ConnectStatus_Rd())
  {
    if ( (gUsbDeviceState==USB11_DEVICE) ||(gUsbDeviceState==USB20_DEVICE) )
    {

        // If the device is connected and we get a connection change.
        // It means that the user change the device and we just missed.
        if (mwHost20_PORTSC_ConnectChange_Rd())
        {
            DRV_USB_DBG(printf("dev changed, we missed\r\n"));
            gSpeed=0xff;  //reset this value
            UHC_ANDXBYTE(0x40, (U8)~0x80);//clear force enter FSmode
            gUsbDeviceState = NO_DEVICE;
            return FALSE;
        }
    }

     if (gUsbDeviceState==BAD_DEVICE) return FALSE;      //not repeating doing emunerate

#if 0
    if (mwOTG20_Control_HOST_SPD_TYP_Rd()==0x01)
    //if (gSpeed==0x01)
        {
#if 0
        U16 result;

           MINI_DEBUG(printf("ls-exit\r\n"));
        //    if (Support_HID()==FALSE) return FALSE;
        printf("is LS device\r\n");
#if 0
        if (xfirst)
     {
            MDrv_Usb_Device_Enum();
            xfirst=FALSE;
        }
#endif
        if (MDrv_GetUsbDeviceType() == USB_INTERFACE_CLASS_HID)
            result= MDrv_GET_JOYSTICK_STATUS();
#endif
           return FALSE;
        }
#endif
    if (gUsbDeviceState==NO_DEVICE)
        gUsbDeviceState=CONNECT_DEVICE;

    return TRUE;
  }
  else
  {
    if ( ( (CHIPID_MARIA10== MDrv_USBGetChipID()) ||
        (CHIPID_MACAW12== MDrv_USBGetChipID()) ||
        (CHIPID_JANUS2== MDrv_USBGetChipID()) ||
        (CHIPID_AMBER6== MDrv_USBGetChipID()) ) &&
         (MDrv_USBGetPortAutoSwitchStatus()) )
    {

        if(MsOS_Timer_DiffTimeFromNow(UsbPortSwitchTimer) > 500)
        {
            UsbPortSwitchTimer = MsOS_GetSystemTime();
            //The Janus has an internal switch for two ports.
            //We must set the switch and scan both ports.
            //printf("&");

            if (g_Port0_Switch == 0)
            {
                UTMI_ANDXBYTE(0x38, ~BIT2);
                g_Port0_Switch = 1;
            }
            else
            {
                UTMI_ORXBYTE(0x38, BIT2);
                g_Port0_Switch = 0;
            }
        }
    }

    gSpeed=0xff;  //reset this value
    UHC_ANDXBYTE(0x40,(U8)~0x80);//clear force enter FSmode
#if 1
    if (drvUSBHost_isPowerSaveModeEnable())
    {
        if  (gUsbDeviceState!=POWER_SAVING)
             drvUSBHost_TurnOnPowerDownMode();

        gUsbDeviceState=POWER_SAVING;
    }
    else
        gUsbDeviceState=NO_DEVICE;
#else
    #ifdef USB_POWER_SAVING_MODE
    #ifdef USBHOST2PORT
    if ((gUsbDeviceState_Port2==NO_DEVICE)||(gUsbDeviceState_Port2==POWER_SAVING))
    #endif
    {
        if  (gUsbDeviceState!=POWER_SAVING)
             UsbTurnOnPowerDownMode();
        gUsbDeviceState=POWER_SAVING;
        #ifdef USBHOST2PORT
        gUsbDeviceState_Port2=POWER_SAVING;
        #endif
    }
    #ifdef USBHOST2PORT
    else
        gUsbDeviceState=NO_DEVICE;
    #endif
    #else
    gUsbDeviceState=NO_DEVICE;
    #endif
#endif

    #ifndef ATV_SERISE_USE
    if (MsOS_Timer_DiffTimeFromNow(UsbStartTime) > 1000 )
    {

      UsbStartTime=MsOS_GetSystemTime();
      ResetUsbHardware();

    }
    #else
    UsbStartTime++;
    if(UsbStartTime>0x600)
    {
      UsbStartTime=0;
	  ResetUsbHardware();
    }
    #endif
    //MINI_DEBUG( printf("no_con \r\n"));
    return FALSE;
  }
}

void MDrv_EjectUsbDevice(void)
{
  struct LUN_Device* LunDevice;
  U8 i;


  //printf("eject\r\n");
  LunDevice = Mass_stor_us1.Mass_stor_device;
  for (i=0; i <= Mass_stor_us1.max_lun ; i++)
  {

    if (LunDevice[i].bDeviceReady == TRUE)
 	{
	  vSCSI_EJECT_DEVICE(i);
      gUsbStatus=USB_EJECT;
      mwHost20_PORTSC_ConnectChange_Set();        //clear port connect change bit
      //printf("ej_ok\r\n");
 	}
  }
  flib_Host20_Close();

}
void MDrv_UsbClose(void)
{
  flib_Host20_Close();
}

BOOLEAN MDrv_UsbBlockReadToMIU(U8 lun,U32 u32BlockAddr, U32 u32BlockNum,U32 u32MIUAddr)
{
  BOOLEAN result;
  U8 retrycnt=0;
  struct LUN_Device* LunDevice = Mass_stor_us1.Mass_stor_device;

  if (mwHost20_PORTSC_ConnectStatus_Rd()==0)
    return FALSE;               //device is not connected

  if (gUsbDeviceState==BAD_DEVICE) return FALSE;


  if (u32BlockAddr > Mass_stor_us1.Mass_stor_device[lun].u32BlockTotalNum)
  {
    MINI_DEBUG(printf("USBRead address is over the range:%lx\r\n",u32BlockAddr));
    return FALSE;
  }
  //printf("usb read sector:%lx\r\n",u32BlockNum);

  #ifdef ATV_SERISE_USE
    #ifdef Enable_Close_INT
    XBYTE[0x2B00]|=0x02;
    DisableINT();
    #endif
  #endif
  //result= bSCSI_Read_Write10(FALSE,lun,  u32BlockAddr, u32BlockNum, u32MIUAddr);
  while (1)
  {
    retrycnt++;
    result= bSCSI_Read_Write10(FALSE,lun,  u32BlockAddr, u32BlockNum, PhytoCacheAddr(u32MIUAddr));
    if (result==TRUE) break;
    if (result==FALSE)
    {
      if (retrycnt > gUsbRetryCount)
      {
       if (gUsbStatus==USB_TIMEOUT)
             gUsbDeviceState=BAD_DEVICE;    //mark as bad device
       else if(gUsbStatus == USB_OK)
       {//give another chance : to restart the host & device(for example : read failed with device STALL)
            if(retrycnt > gUsbRetryCount+1)
                break;
            if (MDrv_Usb_Device_Enum()==FALSE)
            {
              result=FALSE;
              break;
            }
            continue;
       }
       else
             LunDevice[lun].bDeviceValid=FALSE;      //mark as bad lun

        break;          //return FALSE
      }
      MINI_DEBUG( printf("USBDisk Read failed\r\n"));
       if (gUsbStatus==USB_TIMEOUT)
       {
        if ((retrycnt==2)&&(mwOTG20_Control_HOST_SPD_TYP_Rd()==2) )              //make sure it is hi speed
        {
          MINI_DEBUG(printf("Force FS\r\n"));
          UHC_ORXBYTE(0x40,0x80);//force enter FSmode
        }
	    #ifndef ATV_SERISE_USE
        //msAPI_Timer_ResetWDT();
		#else
          #ifdef Enable_Close_INT
          XBYTE[0x2B00]&=0xFD;
          EnableINT();
	      #endif
	    #endif

        #ifdef Enable_Low_Temperature_Patch
	    if(gSpeed==0)
	    {
	      //printf("\r\n CDR toggle!!",0);
          UTMI_SETXBYTE(0x0a, UTMI_READXBYTE(0x0a)^0x10); //invert CDR_CLOCK
          UTMI_ORXBYTE(0x06,0x03); //reset UTMI
          UTMI_ANDXBYTE(0x06,0xfc);
	    }
	    #endif

       // MDrv_Usb_Device_Enum();             //reinit usb device
          if (MDrv_Usb_Device_Enum()==FALSE)
          	{
              result=FALSE;
              break;
            }
      }
    }

  }

  #ifdef ATV_SERISE_USE
    #ifdef Enable_Close_INT
    XBYTE[0x2B00]&=0xFD;
    EnableINT();
    #endif
  #endif
  //MDrv_Sys_ReleaseXdataWindow1();
  if (result==FALSE)
  {
    DRV_USB_DBG(printf("USBDisk Read failed\r\n"));
    //printf("read failed\r\n");
  }

  return result;

}

BOOLEAN MDrv_UsbBlockWriteFromMIU(U8 lun, U32 u32BlockAddr, U32 u32BlockNum,U32 u32MIUAddr)
{
  BOOLEAN result;
  if (mwHost20_PORTSC_ConnectStatus_Rd()==0)
    return FALSE;               //device is not connected

   if (gUsbDeviceState==BAD_DEVICE) return FALSE;

  if (u32BlockAddr > Mass_stor_us1.Mass_stor_device[lun].u32BlockTotalNum)
  {
    MINI_DEBUG(printf("USBWrite address is over the range:%lx\r\n",u32BlockAddr));
    return FALSE;
  }


  result= bSCSI_Read_Write10(TRUE,lun,  u32BlockAddr, u32BlockNum, PhytoCacheAddr(u32MIUAddr));


  return result;

}
U32 MDrv_GetUsbBlockSize(U8 lun)
{
  return Mass_stor_us1.Mass_stor_device[lun].u32BlockSize;
}

U32 MDrv_GetUsbBlockNum(U8 lun)
{
  return Mass_stor_us1.Mass_stor_device[lun].u32BlockTotalNum;
}


U8 MDrv_USB_GetXDataExtStackCount(void);
U8 MDrv_USB_GetXDataExtStackCount(void)
{
  return xdwin1_lockCount;
}
//---if you don't know how to use it, don't use it

#if 0
void EnterUXBMode(void)
{
  MINI_DEBUG( printf("Enter UXB mode\r\n"));
  UHC_XBYTE(0x40)|=0x80;//force enter FSmode
  MDrv_Usb_Device_Enum();
}
void LeaveUXBMode(oid)
{
  MINI_DEBUG(printf("leave UXB mode \r\n"));
  UHC_XBYTE(0x40)&=~0x80;//leave  FSmode
  MDrv_Usb_Device_Enum();

}
#endif
#ifndef DEVICE_ENUM_SEGMENT
BOOLEAN  MDrv_Usb_Device_Enum(void)
{
  BOOLEAN result=FALSE;
  U8  RetryCount=0;

  U8 rootUSBDeviceInterfaceClass = USB_INTERFACE_CLASS_NONE;
  U32 devspeed;
  if (gUsbDeviceState==BAD_DEVICE) return FALSE;        //bad device , no more enumerate

  //20120711, Patch for kingston device, can't repeate to re-enumerate.
  if ( (gUsbDeviceState==USB11_DEVICE) ||(gUsbDeviceState==USB20_DEVICE) )
  {
        U16     uVID, uPID;

        MDrv_USBGetVIDPID(&uVID, &uPID);
        if (uVID == 0x951 && uPID == 0x1603)
        {
            DRV_USB_DBG(printf("Don't re-enumerate for special device."));
            return TRUE;
        }
  }

  NowIsHub=0;

#ifdef Enable_SOF_Only
  UHC_ORXBYTE(0x40,0x80);//force enter FSmode
#endif
redo_init:
  //USB ROOT
  RetryCount++;
  if (MDrv_UsbHost_Init()==FALSE) goto fail_exit;
  if  (flib_Host20_Enumerate(1,3)==0)
  {
    if ((gUsbStatus==USB_TIMEOUT)||(gUsbStatus==USB_INIT_FAIL))
    {
      #ifdef DTV_STANDARD_LIB
      if ((RetryCount==2)&&(mwOTG20_Control_HOST_SPD_TYP_Rd()==2))
      {                                                               //make sure it is hi speed
        MINI_DEBUG(printf("Force FS\r\n"));
        UHC_ORXBYTE(0x40,0x80);//force enter FSmode
      }
      #endif
      if (RetryCount >= 3)
      {
        MINI_DEBUG(printf("bad device\r\n"));
        gUsbDeviceState=BAD_DEVICE;
        goto fail_exit;
      }
      #ifndef ATV_SERISE_USE
      //msAPI_Timer_ResetWDT();
      #endif
      goto redo_init;
    }
    gUsbDeviceState=BAD_DEVICE;
    goto fail_exit;
  }
  else
  {
    UTMI_SETXBYTE(0x2a,0);

    rootUSBDeviceInterfaceClass= sAttachDevice->saCD[0].sInterface[0].bInterfaceClass;
    //MDrv_UsbGetInterfaceClass(0, 0);
    DRV_USB_DBG(printf("Check USD Device 4\r\n"));
#if 0 // We don't need to check here
    if(rootUSBDeviceInterfaceClass != 0x08 && rootUSBDeviceInterfaceClass != 0x09)
    {
      //printf("unsupport class\r\n",0);
      DRV_USB_DBG(printf("unsupport USB root class=%02bx\r\n", rootUSBDeviceInterfaceClass););
      result= FALSE;
      goto fail_exit;
    }
#endif

    DRV_USB_DBG(printf("USB root class=%x\r\n",rootUSBDeviceInterfaceClass));
    if (rootUSBDeviceInterfaceClass==USB_INTERFACE_CLASS_HUB)//HUB
    {
      U8 PortNum,i,devaddr,stor_devs;

#ifdef ENABLE_HOST_TEST
      DRV_USB_DBG(printf("Unsupport USB hub\r\n"));
      gUsbDeviceState=BAD_DEVICE;
      goto fail_exit;
#endif
	  sHubDevice = sAttachDevice;
      devaddr=6;
      stor_devs = 0;
      DRV_USB_DBG(printf("Find USB Hub!\r\n"));

	  pHost20_qHD_List_Control1->bDeviceAddress=sHubDevice->bAdd;
	  pHost20_qHD_List_Control1->bEdSpeed=sHubDevice->bSpeed;
	  pHost20_qHD_List_Control1->bMaxPacketSize=sHubDevice->sDD.bEP0MAXPACKETSIZE;
      PortNum=Usb_Hub_Port_Num();

      for (i=1; i <=PortNum ; i++)
      {
        pHost20_qHD_List_Control1->bDeviceAddress=sHubDevice->bAdd;
		pHost20_qHD_List_Control1->bEdSpeed=sHubDevice->bSpeed;
        if (USB_Hub_Handle(i,&devspeed)==FALSE)
        {
          result=FALSE;
          continue;
        }
        devaddr++;
        NowIsHub=1;
		pHost20_qHD_List_Control0->bEdSpeed=devspeed;
		pHost20_qHD_List_Control1->bEdSpeed=devspeed;


		if(sHubDevice->bSpeed==0 && devspeed==1)
		{
			UHC_ORXBYTE(0x34,0x0C);
		}

        if (flib_Host20_Enumerate(1,devaddr)==0)
        {
        	result=FALSE;
        	continue;
        }

        //MStar, 20111110, For card reader with internal hub
        if (sAttachDevice->saCD[0].sInterface[0].bInterfaceClass == 0x08)
        {
            DRV_USB_DBG(printf("		Find Storage device\r\n"));
            stor_devs++;
        }
        sAttachDevice->bAdd=devaddr;
        result=MassStorage_Init();              //do mass storage class init
        if (result==TRUE) break;
#ifdef	ENABLE_HID
        if (drvUSBHost_HID_Init(USB_PORT_0) == 0)
        {
            result = TRUE;
            DRV_USB_DBG(printf("		Find HID device\r\n"));
        }
#endif
      }

      if ( (result==FALSE) && (stor_devs == 0) )
      {
        MINI_DEBUG(printf("unsupport hub class device,->bad device\r\n"));
        gUsbDeviceState=BAD_DEVICE;
      }
    }

    //else if(rootUSBDeviceInterfaceClass==USB_INTERFACE_CLASS_MSD)//Mass storage class
    else if ( (rootUSBDeviceInterfaceClass == USB_INTERFACE_CLASS_MSD) &&
		   (sAttachDevice->saCD[0].sInterface[0].bInterfaceProtocol == 0x50) )
    {
      result=MassStorage_Init();
      #ifdef Enable_Burning_Test
      UsbTest();
      #endif

      #ifdef Enable_Low_Temperature_Patch
	  if((gSpeed==0)&&(gUsbStatus==USB_TIMEOUT))
	  {
	    //printf("\r\n CDR toggle!!",0);
        UTMI_SETXBYTE(0x0a, UTMI_READXBYTE(0x0a)^0x10); //invert CDR_CLOCK
        UTMI_ORXBYTE(0x06,0x03); //reset UTMI
        UTMI_ANDXBYTE(0x06,0xfc);
	  }
	  #endif

    }
#ifdef ENABLE_CBI_HOST
    else if ( (rootUSBDeviceInterfaceClass == USB_INTERFACE_CLASS_MSD) &&
		   ((sAttachDevice->saCD[0].sInterface[0].bInterfaceProtocol == 0x00)||
		   (sAttachDevice->saCD[0].sInterface[0].bInterfaceProtocol == 0x01)) )
    {
        DRV_USB_DBG(printf("My CBI MassStorage Device!!!\r\n"));

        result=MassStorage_Init();    //do mass storage class init
        return result;
    }
#endif
#ifdef ENABLE_PTP
    else if (rootUSBDeviceInterfaceClass == USB_INTERFACE_CLASS_IMAGE)
    {
        if (drvUSBHost_PTP_Init(USB_PORT_0) == PTP_OK)
        {
            result = TRUE;
            DRV_USB_DBG(printf("PTP initial ok\r\n"));
        }
    }
#endif
#ifdef	ENABLE_HID
    else if (rootUSBDeviceInterfaceClass==USB_INTERFACE_CLASS_HID)//HID
    {
        if (drvUSBHost_HID_Init(USB_PORT_0) == 0)
        {
            result = TRUE;
            DRV_USB_DBG(printf("HID initial ok\r\n"));
        }
    }
#endif
    else
    {

       MINI_DEBUG(printf("unsupport %x class device->bad device\r\n",rootUSBDeviceInterfaceClass));
       gUsbDeviceState=BAD_DEVICE;           //not belong to any above
    }

    DRV_USB_DBG(printf("USB_CON\r\n"));
    //return result;
  }

fail_exit:

  if (mwHost20_PORTSC_ConnectChange_Rd())
    mwHost20_PORTSC_ConnectChange_Set();

  return result;
}
#else // support device enumeration dividable
BOOLEAN  _MDrv_Usb_Device_Enum_OK(void)
{
    BOOLEAN result=FALSE;
    U8 rootUSBDeviceInterfaceClass = USB_INTERFACE_CLASS_NONE;

    UTMI_SETXBYTE(0x2a,0);

    rootUSBDeviceInterfaceClass= sAttachDevice->saCD[0].sInterface[0].bInterfaceClass;
    //MDrv_UsbGetInterfaceClass(0, 0);
    DRV_USB_DBG(printf("Check USD Device 4\r\n"));
#if 0 // We don't need to check here
    if(rootUSBDeviceInterfaceClass != 0x08 && rootUSBDeviceInterfaceClass != 0x09)
    {
      //printf("unsupport class\r\n",0);
      DRV_USB_DBG(printf("unsupport USB root class=%02bx\r\n", rootUSBDeviceInterfaceClass););
      result= FALSE;
      goto fail_exit;
    }
#endif

    DRV_USB_DBG(printf("USB root class=%02bx\r\n",rootUSBDeviceInterfaceClass));
    if (rootUSBDeviceInterfaceClass==USB_INTERFACE_CLASS_HUB)//HUB
    {
      U8 PortNum,i,devaddr,stor_devs;

      devaddr=6;
      stor_devs = 0;
      DRV_USB_DBG(printf("Hub class!\r\n"));

      PortNum=Usb_Hub_Port_Num();
      for (i=1; i <=PortNum ; i++)
      {
        sAttachDevice->bAdd=3;
        pHost20_qHD_List_Control1->bDeviceAddress=sAttachDevice->bAdd;
        if (USB_Hub_Handle(i)==FALSE)
        {
          result=FALSE;
          continue;
        }
        devaddr++;
        NowIsHub=1;
        if (flib_Host20_Enumerate(1,devaddr)==0)
        {
          result=FALSE;
          continue;
        }

        //MStar, 20111110, For card reader with internal hub
        if (sAttachDevice->saCD[0].sInterface[0].bInterfaceClass == 0x08)
            stor_devs++;

        sAttachDevice->bAdd=devaddr;
        result=MassStorage_Init();              //do mass storage class init
        if (result==TRUE) break;
      }

      if ( (result==FALSE) && (stor_devs == 0) )
      {
        MINI_DEBUG(printf("unsupport hub class device,->bad device\r\n"));
        gUsbDeviceState=BAD_DEVICE;
      }
    }

    //else if(rootUSBDeviceInterfaceClass==USB_INTERFACE_CLASS_MSD)//Mass storage class
    else if ( (rootUSBDeviceInterfaceClass == USB_INTERFACE_CLASS_MSD) &&
                (sAttachDevice->saCD[0].sInterface[0].bInterfaceProtocol == 0x50) )
    {
      result=MassStorage_Init();
      #ifdef Enable_Burning_Test
      UsbTest();
      #endif

      #ifdef Enable_Low_Temperature_Patch
        if((gSpeed==0)&&(gUsbStatus==USB_TIMEOUT))
        {
            //printf("\r\n CDR toggle!!",0);
            UTMI_SETXBYTE(0x0a, UTMI_READXBYTE(0x0a)^0x10); //invert CDR_CLOCK
            UTMI_ORXBYTE(0x06,0x03); //reset UTMI
            UTMI_ANDXBYTE(0x06,0xfc);
        }
        #endif

    }
#ifdef ENABLE_CBI_HOST
    else if ( (rootUSBDeviceInterfaceClass == USB_INTERFACE_CLASS_MSD) &&
                ((sAttachDevice->saCD[0].sInterface[0].bInterfaceProtocol == 0x00)||
                (sAttachDevice->saCD[0].sInterface[0].bInterfaceProtocol == 0x01)) )
    {
        DRV_USB_DBG(printf("My CBI MassStorage Device!!!\r\n"));

        result=MassStorage_Init();    //do mass storage class init
        //return result;
    }
#endif
#ifndef ENABLE_HOST_TEST
    else if (rootUSBDeviceInterfaceClass == USB_INTERFACE_CLASS_IMAGE)
    {
        if (drvUSBHost_PTP_Init(USB_PORT_0) == PTP_OK)
        {
            result = TRUE;
            DRV_USB_DBG(printf("PTP initial ok\r\n"));
        }
    }
    else if (rootUSBDeviceInterfaceClass==USB_INTERFACE_CLASS_HID)//HID
    {
        if (drvUSBHost_HID_Init(USB_PORT_0) == 0)
        {
            result = TRUE;
            DRV_USB_DBG(printf("HID initial ok\r\n"));
        }
    }
#endif
    else
    {
       MINI_DEBUG(printf("unsupport %02bx class device->bad device\r\n",rootUSBDeviceInterfaceClass));
       gUsbDeviceState=BAD_DEVICE;           //not belong to any above
    }

    DRV_USB_DBG(printf("USB_CON\r\n"));
    //return result;

  return result;
}
BOOLEAN  MDrv_Usb_Device_Enum(void)
{
      BOOLEAN result=FALSE;
      U8  RetryCount=0;

      if (gUsbDeviceState==BAD_DEVICE) return FALSE;        //bad device , no more enumerate

      //20120711, Patch for kingston device, can't repeate to re-enumerate.
      if ( (gUsbDeviceState==USB11_DEVICE) ||(gUsbDeviceState==USB20_DEVICE) )
      {
            U16     uVID, uPID;

            MDrv_USBGetVIDPID(&uVID, &uPID);
            if (uVID == 0x951 && uPID == 0x1603)
            {
                DRV_USB_DBG(printf("Don't re-enumerate for special device."));
                return TRUE;
            }
      }

      NowIsHub=0;

#ifdef Enable_SOF_Only
      UHC_ORXBYTE(0x40,0x80);//force enter FSmode
#endif
    redo_init:
      //USB ROOT
      RetryCount++;
      if (MDrv_UsbHost_Init()==FALSE) goto fail_exit;
      if  (flib_Host20_Enumerate(1,3)==0)
      {
        if ((gUsbStatus==USB_TIMEOUT)||(gUsbStatus==USB_INIT_FAIL))
        {
      #ifdef DTV_STANDARD_LIB
          if ((RetryCount==2)&&(mwOTG20_Control_HOST_SPD_TYP_Rd()==2))
          {                                                               //make sure it is hi speed
            MINI_DEBUG(printf("Force FS\r\n"));
            UHC_ORXBYTE(0x40,0x80);//force enter FSmode
          }
      #endif
          if (RetryCount >= 3)
          {
            MINI_DEBUG(printf("bad device\r\n"));
            gUsbDeviceState=BAD_DEVICE;
            goto fail_exit;
          }
      #ifndef ATV_SERISE_USE
          //msAPI_Timer_ResetWDT();
      #endif
          goto redo_init;
        }
        gUsbDeviceState=BAD_DEVICE;
        goto fail_exit;
      }
      else
      {
        result = _MDrv_Usb_Device_Enum_OK();
      }

    fail_exit:

      if (mwHost20_PORTSC_ConnectChange_Rd())
        mwHost20_PORTSC_ConnectChange_Set();

      return result;
}

/*
Device Enumerate State:
0: global variable initial
1: ready to start new enumerating process
2: in-progress, stage 1
3: in-progress, stage 2
return value:
0: success
1: in-progress
2: fail
*/
U8 enum_state = 0;
U32 usbDevEnumTimer;
U8  MDrv_Usb_Device_Enum_EX(void)
{
  BOOLEAN result=FALSE;
  static U8  RetryCount;
  static U8 fr;

  if (gUsbDeviceState==BAD_DEVICE) return DEVENUM_FAIL;        //bad device , no more enumerate

redo_init_ex:
  //USB ROOT

    switch (enum_state)
    {
        case 0: // initial global variables
          NowIsHub=0;

#ifdef Enable_SOF_Only
          UHC_ORXBYTE(0x40,0x80);//force enter FSmode
#endif
          RetryCount = 0;
        case 1: // first entry
            RetryCount++;
            if (_MDrv_UsbHost_Init0()==FALSE) goto fail_exit_ex; // MDrv_UsbHost_Init() part 1
            enum_state = 2;
            usbDevEnumTimer = MsOS_GetSystemTime();
            return DEVENUM_INPROGRESS;
        case 2: // in-progress stage 1
            if (MsOS_Timer_DiffTimeFromNow(usbDevEnumTimer) < 500)
                return DEVENUM_INPROGRESS;
            _MDrv_UsbHost_Init1(); // MDrv_UsbHost_Init() part 2
            fr = _flib_Host20_Enumerate(1, 3); // flib_Host20_Enumerate() part 1
            enum_state = 3;
            usbDevEnumTimer = MsOS_GetSystemTime();
            return DEVENUM_INPROGRESS;
        case 3: // in-progress stage 2
            if (MsOS_Timer_DiffTimeFromNow(usbDevEnumTimer) < 150)
                return DEVENUM_INPROGRESS;
            enum_state = 1; // retry use
    }

  if  (fr==0)
  {
    if ((gUsbStatus==USB_TIMEOUT)||(gUsbStatus==USB_INIT_FAIL))
    {
      #ifdef DTV_STANDARD_LIB
      if ((RetryCount==2)&&(mwOTG20_Control_HOST_SPD_TYP_Rd()==2))
      {                                                               //make sure it is hi speed
        MINI_DEBUG(printf("Force FS\r\n"));
        UHC_ORXBYTE(0x40,0x80);//force enter FSmode
      }
      #endif
      if (RetryCount >= 3)
      {
        MINI_DEBUG(printf("bad device\r\n"));
        gUsbDeviceState=BAD_DEVICE;
        goto fail_exit_ex;
      }
      #ifndef ATV_SERISE_USE
      //msAPI_Timer_ResetWDT();
      #endif
      goto redo_init_ex;
    }
    gUsbDeviceState=BAD_DEVICE;
    goto fail_exit_ex;
  }
  else
  {
    result = _MDrv_Usb_Device_Enum_OK();
  }

fail_exit_ex:

  if (mwHost20_PORTSC_ConnectChange_Rd())
    mwHost20_PORTSC_ConnectChange_Set();
  enum_state = 0; // reset to initial

  //return result;
  return ( result ? DEVENUM_OK : DEVENUM_FAIL);
}
#endif
//////////////////////////////////////////////////////////////////////////////////////////
//
// USB Issue Test packet function !!
//
/////////////////////////////////////////////////////////////////////////////////////////
U8 UTMI_Reg_14 = 0xff, UTMI_Reg_15 = 0xff;
void Port0_IssueTestPacket_Initial(void);
void Port0_IssueTestPacket_Initial(void)
{
    UTMI_SETXBYTE(0x02, 0x84);
    UTMI_SETXBYTE(0x03, 0x20);

    USBC_SETXBYTE(0x00, 0x0A);
    USBC_SETXBYTE(0x00, 0x28);

    UTMI_SETXBYTE(0x06, 0x00);
    UTMI_SETXBYTE(0x07, 0x00);
    UTMI_SETXBYTE(0x10, 0x00);
    UTMI_SETXBYTE(0x11, 0x00);
    UTMI_SETXBYTE(0x2c, 0x00);
    UTMI_SETXBYTE(0x2d, 0x00);
    UTMI_SETXBYTE(0x2e, 0x00);
    UTMI_SETXBYTE(0x2f, 0x00);
    if (UTMI_Reg_14 == 0xff)
        UTMI_Reg_14 = UTMI_READXBYTE(0x14);
    else
        UTMI_SETXBYTE(0x14, UTMI_Reg_14);

    if (UTMI_Reg_15 == 0xff)
        UTMI_Reg_15 = UTMI_READXBYTE(0x15);
    else
        UTMI_SETXBYTE(0x15, UTMI_Reg_15);

    UTMI_ORXBYTE(0x06,0x40);                //Force HS TX current enable and CDR stage select
    UTMI_ANDXBYTE(0x06,~0x20);                      //clear bit 5

    UTMI_ORXBYTE(0x06,0x03);                //reset UTMI
    MsOS_DelayTask(2);
    UTMI_ANDXBYTE(0x06,0xfc);

    UTMI_SETXBYTE(0x03, 0xa8);         //for device disconnect status bit
    //XBYTE[UTMIBaseAddr+0x07]|=0x02;      //

    if  (MDrv_USBGetChipID()==CHIPID_NEPTUNE)            //Neptune , after U06
   {
      if (MDrv_USBGetChipVersion()>= 0x6)              //U07
        {
            UTMI_ORXBYTE(0x07, 0x02);
            UTMI_SETXBYTE(0x2c,0xc1);
            UTMI_SETXBYTE(0x2d,0x3b);//enable TX common mode,
            UTMI_ORXBYTE(0x2f, 0x0e);            //preemsis
        }
    }
    else if ((MDrv_USBGetChipID()>=CHIPID_ERIS)&&(MDrv_USBGetChipID()<=CHIPID_TITANIA))                    //Eris: 3 ,Titania: 4,
    {
            UTMI_SETXBYTE(0x2c,0xc5);
            UTMI_SETXBYTE(0x2d,0x3b);//enable TX common mode,
            UTMI_ORXBYTE(0x2f, 0x0e);            //preemsis
    }
    else if (MDrv_USBGetChipID()==CHIPID_PLUTO)          // Pluto: 5
    {
            UTMI_ORXBYTE(0x2c, 0xc5);
            UTMI_ORXBYTE(0x2d, 0x3);//enable TX common mode,
            UTMI_ORXBYTE(0x2f, 0x4a);            //preemsis
            UTMI_ORXBYTE(0x13, 0x70);
    }
    else if (MDrv_USBGetChipID()==CHIPID_TITANIA2)          // Titania 2
    {
            UTMI_ORXBYTE(0x2c, 0xc1);
            UTMI_ORXBYTE(0x2d, 0x3);//enable TX common mode,
            UTMI_ORXBYTE(0x2f, 0x4a);            //preemsis
    }
    else if (MDrv_USBGetChipID()==CHIPID_TRITON)        //Triton=6
    {
            UTMI_SETXBYTE(0x2c,0xc1);
            UTMI_SETXBYTE(0x2d,0x3b);//enable TX common mode,
            UTMI_ORXBYTE(0x2f,0x0e);            //preemsis
    }
    else if (MDrv_USBGetChipID()==CHIPID_EUCLID)          // Euclid
    {
            UTMI_ORXBYTE(0x2c, 0xc1);
            UTMI_ORXBYTE(0x2d, 0x3);//enable TX common mode,
            UTMI_ORXBYTE(0x2f, 0x4a);            //preemsis
    }
    else if ( (MDrv_USBGetChipID()==CHIPID_TITANIA3) ||          // Titania 3, Titania 4
                (MDrv_USBGetChipID()==CHIPID_TITANIA4) ||
                (MDrv_USBGetChipID()==CHIPID_TITANIA7) ||
                (MDrv_USBGetChipID()==CHIPID_TITANIA8) ||
                (MDrv_USBGetChipID()==CHIPID_TITANIA9) ||
                (MDrv_USBGetChipID()==CHIPID_TITANIA12) ||
                (MDrv_USBGetChipID()==CHIPID_TITANIA13) ||
                (MDrv_USBGetChipID()==CHIPID_JANUS) ||
                (MDrv_USBGetChipID() == CHIPID_MARIA10) ||
                (MDrv_USBGetChipID() == CHIPID_MACAW12) ||
                (MDrv_USBGetChipID() == CHIPID_JANUS2)  ||
                (MDrv_USBGetChipID() == CHIPID_AMBER6) )
    {
            UTMI_ORXBYTE(0x2c, 0x10);  //TX-current adjust to 105%
            UTMI_ORXBYTE(0x2d, 0x02);  //Pre-emphasis enable
            UTMI_ORXBYTE(0x2f, 0x81);  //HS_TX common mode current enable (100mV);Pre-emphasis enable (10%)
            UTMI_ORXBYTE(0x09,0x81);     //UTMI RX anti-dead-lock, ISI improvement
    }
    else  //for Amber1 later 40nm
    {
            UTMI_ORXBYTE(0x2c, 0x50);
            UTMI_ORXBYTE(0x2d, 0x02);
            UTMI_ORXBYTE(0x2f, 0x01);
            UTMI_ORXBYTE(0x09,0x81);     //UTMI RX anti-dead-lock, ISI improvement
    }

    if (MDrv_USBGetChipID()==CHIPID_PLUTO)                   // If is Pluto
        UTMI_ORXBYTE(0x09,0x01);     //ISI improvement
    else
        UTMI_ORXBYTE(0x13,0x02);     //ISI improvement

  if ( (MDrv_USBGetChipID() == CHIPID_NEPTUNE) ||
      (MDrv_USBGetChipID() == CHIPID_ERIS) ||
      (MDrv_USBGetChipID() == CHIPID_TITANIA) ||
      (MDrv_USBGetChipID() == CHIPID_PLUTO) ||
      (MDrv_USBGetChipID() == CHIPID_TRITON) ||
      (MDrv_USBGetChipID() == CHIPID_TITANIA2) )
  {
  }
  else
  {
    UTMI_ORXBYTE(0x0b, 0x80); //TX timing select latch path
  }

    UTMI_ORXBYTE(0x09,0x60);//0x20;       //patch low tempture,FL meta issue and enable new FL RX engin

  if (  (MDrv_USBGetChipID() == CHIPID_NEPTUNE) ||
        (MDrv_USBGetChipID() == CHIPID_ERIS) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA) ||
        (MDrv_USBGetChipID() == CHIPID_PLUTO) ||
        (MDrv_USBGetChipID() == CHIPID_TRITON) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA2) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA3) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA4) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA7) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA8) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA9) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA12) ||
        (MDrv_USBGetChipID() == CHIPID_TITANIA13) ||
        (MDrv_USBGetChipID() == CHIPID_JANUS) ||
        (MDrv_USBGetChipID() == CHIPID_MARIA10) ||
        (MDrv_USBGetChipID() == CHIPID_MACAW12) ||
        (MDrv_USBGetChipID() == CHIPID_JANUS2) ||
        (MDrv_USBGetChipID() == CHIPID_AMBER1) ||
        (MDrv_USBGetChipID() == CHIPID_AMBER3)  ||
        (MDrv_USBGetChipID() == CHIPID_AMBER5)  ||
        (MDrv_USBGetChipID() == CHIPID_AMBER6)  ||
        (MDrv_USBGetChipID() == CHIPID_AMBER7)   ||
        (MDrv_USBGetChipID() == CHIPID_AMETHYST) )
    {
        UTMI_ORXBYTE(0x15,0x20); //HOST CHIRP Detect
    }
    else
    {
        UTMI_ORXBYTE(0x15,0x60); // change to 55 interface (bit6)
    }

    //UHC_ORXBYTE(0x32,0x01); //Force issue test packet
    MsOS_DelayTask(10);
}

#ifdef Enable_Issue_TestPacket

extern void USB_DACHE_FLUSH(U32 addr, U32 length);

void IssueTestPacket(U8 xdata *TestDataAddr)
{
  U32 DMAAddress,datreg32;
  U8 xdata *TestData;
  #ifdef Issue_TestPacket
  U16 i;
  #endif

  DRV_USB_DBG(printf("Start Test Packet on Port0\r\n"));

  TestData=(U8*) KSEG02KSEG1(TestDataAddr);

  #ifdef Issue_OUTTestPacket
  TestData[0]=0x55;
  TestData[1]=0x53;
  TestData[2]=0x42;
  TestData[3]=0x43;
  TestData[4]=0x88;
  TestData[5]=0x88;
  TestData[6]=0x89;
  TestData[7]=0xa0;
  TestData[8]=0x00;
  TestData[9]=0x02;
  TestData[10]=0x00;
  TestData[11]=0x00;
  TestData[12]=0x80;
  TestData[13]=0x00;
  TestData[14]=0x0a;
  TestData[15]=0x28;
  TestData[16]=0x00;
  TestData[17]=0x00;
  TestData[18]=0x00;
  TestData[19]=0x00;
  TestData[20]=0x84;
  TestData[21]=0x00;
  TestData[22]=0x00;
  TestData[23]=0x01;
  TestData[24]=0x00;
  TestData[25]=0x00;
  TestData[26]=0x00;
  TestData[27]=0x00;
  TestData[28]=0x00;
  TestData[29]=0x00;
  TestData[30]=0x00;
  #endif

  //printf("3AAC:%x\r\n",XBYTE[ 0x3AAC]);
  //printf("UTMI(0x2c):%x\r\n",XBYTE[gUTMI_BASE+0x2C]);
  #ifdef Issue_TestPacket
  TestData[0]=0x0;
  TestData[1]=0x0;
  TestData[2]=0x0;
  TestData[3]=0x0;
  TestData[4]=0x0;
  TestData[5]=0x0;
  TestData[6]=0x0;
  TestData[7]=0x0;
  TestData[8]=0x0;
  TestData[9]=0xaa;
  TestData[10]=0xaa;
  TestData[11]=0xaa;
  TestData[12]=0xaa;
  TestData[13]=0xaa;
  TestData[14]=0xaa;
  TestData[15]=0xaa;
  TestData[16]=0xaa;
  TestData[17]=0xee;
  TestData[18]=0xee;
  TestData[19]=0xee;
  TestData[20]=0xee;
  TestData[21]=0xee;
  TestData[22]=0xee;
  TestData[23]=0xee;
  TestData[24]=0xee;
  TestData[25]=0xfe;
  TestData[26]=0xff;
  TestData[27]=0xff;
  TestData[28]=0xff;
  TestData[29]=0xff;
  TestData[30]=0xff;
  TestData[31]=0xff;
  TestData[32]=0xff;
  TestData[33]=0xff;
  TestData[34]=0xff;
  TestData[35]=0xff;
  TestData[36]=0xff;
  TestData[37]=0x7f;
  TestData[38]=0xbf;
  TestData[39]=0xdf;
  TestData[40]=0xef;
  TestData[41]=0xf7;
  TestData[42]=0xfb;
  TestData[43]=0xfd;
  TestData[44]=0xfc;
  TestData[45]=0x7e;
  TestData[46]=0xbf;
  TestData[47]=0xdf;
  TestData[48]=0xfb;
  TestData[49]=0xfd;
  TestData[50]=0xfb;
  TestData[51]=0xfd;
  TestData[52]=0x7e;

  for (i=53; i<128; i++)
    TestData[i]= 0;

  #endif

  USB_DACHE_FLUSH((U32)TestDataAddr, 128);

  //printf("[9]=%2bx\r\n", XBYTE[((U16)TestData)+9]);

  //DbgPortEanble();

  //UHC_XBYTE(0x50)|=0x14; //enable test packet and lookback
  UHC_ORXBYTE(0x50,0x14); //enable test packet and lookback

  UTMI_ORXBYTE(0x06,0x03); //TR/RX reset
  UTMI_ANDXBYTE(0x06,0xFC);

  while(1)
  {

     DMAAddress=(U32)VA2PA(TestData);

     //set DMA memory base address
     UHC_SETXBYTE(0x74,(U8)DMAAddress);
     UHC_SETXBYTE(0x75,(U8)(DMAAddress>>8));
     UHC_SETXBYTE(0x76,(U8)(DMAAddress>>16));
     UHC_SETXBYTE(0x77,(U8)(DMAAddress>>24));

    //printf("start check 2474=%2bx\r\n",XBYTE[0x2474]);
    //printf("start check 2475=%2bx\r\n",XBYTE[0x2475]);
    //printf("start check 2476=%2bx\r\n",XBYTE[0x2476]);
    //printf("start check 2477=%2bx\r\n",XBYTE[0x2477]);


    //set DMA data Length and type(memory to FIFO)
    #ifdef Issue_TestPacket
    datreg32 = 53;
    #else
    datreg32 = 31;
    #endif

    datreg32 = datreg32 << 8;
    datreg32 = datreg32 | 0x02;

    UHC_SETXBYTE(0x70,(U8)datreg32);
    UHC_SETXBYTE(0x71,(U8)(datreg32>>8));
    UHC_SETXBYTE(0x72,(U8)(datreg32>>16));
    UHC_SETXBYTE(0x73,(U8)(datreg32>>24));

    UHC_ORXBYTE(0x70,0x01);//DMA start

    //printf("start check 2470=%2bx\r\n",XBYTE[0x2470]);
    //printf("start check 2471=%2bx\r\n",XBYTE[0x2471]);
    //printf("start check 2472=%2bx\r\n",XBYTE[0x2472]);
    //printf("start check 2473=%2bx\r\n",XBYTE[0x2473]);

    //MsOS_DelayTask(1000);

    //printf("start check 2444=%2bx\r\n",XBYTE[0x2444]);

    while(!(UHC_READXBYTE(0x44) &0x08))
    {
      //printf("XBYTE[0x2444]=%2bx\r\n",XBYTE[0x2444]);
      //MsOS_DelayTask(10);//delay
    }

    // printf("Dma success\r\n",0);

    MsOS_DelayTask(10);
    }

    }

#endif

void MDrv_SendTestPacketByHW(void);
void MDrv_SendTestPacketByHW(void)
{
    DRV_USB_DBG(printf("Start test packet on port 0\r\n"));
    UTMI_SETXBYTE(0x14, 0x00);
    UTMI_SETXBYTE(0x15, 0x06);

    UTMI_SETXBYTE(0x10, 0x38);
    UTMI_SETXBYTE(0x11, 0x00);

    UTMI_SETXBYTE(0x32, 0xFE);
    UTMI_SETXBYTE(0x33, 0x0B);
}

void IssueTestJ(void);
void IssueTestJ(void)
{
    DRV_USB_DBG(printf("TEST_J on Port0\r\n"));

    UTMI_SETXBYTE(0x2c, 0x04);
    UTMI_SETXBYTE(0x2d, 0x20);
    UTMI_SETXBYTE(0x2e, 0x00);
    UTMI_SETXBYTE(0x2f, 0x00);

    USBC_ORXBYTE(0, 0x02); //Enable UHC_RST
    MsOS_DelayTask(10);
    USBC_ANDXBYTE(0, 0xFD);

    //UHCREG(0x32)|=0x01;
    UHC_ORXBYTE(0x32,0x01);
    MsOS_DelayTask(10);
    //UHCREG(0x32)&=0xfe;
    UHC_ANDXBYTE(0x32,0xfe);

    UHC_ORXBYTE(0x50,0x01); //enable test J
}

void IssueTestK(void);
void IssueTestK(void)
{
    DRV_USB_DBG(printf("TEST_K on port0\r\n"));

    UTMI_SETXBYTE(0x2c, 0x04);
    UTMI_SETXBYTE(0x2d, 0x20);
    UTMI_SETXBYTE(0x2e, 0x00);
    UTMI_SETXBYTE(0x2f, 0x00);

    USBC_ORXBYTE(0, 0x02); //Enable UHC_RST
    MsOS_DelayTask(10);
    USBC_ANDXBYTE(0, 0xFD);

    //UHCREG(0x32)|=0x01;
    UHC_ORXBYTE(0x32,0x01);
    MsOS_DelayTask(10);
    //UHCREG(0x32)&=0xfe;
    UHC_ANDXBYTE(0x32,0xfe);

    MsOS_DelayTask(10);
    UHC_ORXBYTE(0x50,0x02); //enable test K
}

void IssueSE0(void);
void IssueSE0(void)
{
    DRV_USB_DBG(printf("SE0 on port0\r\n"));

    UTMI_SETXBYTE(0x06, 0xA0);
    UTMI_SETXBYTE(0x07, 0x04);

    MsOS_DelayTask(10);
    USBC_ORXBYTE(0, 0x02); //Enable UHC_RST
    MsOS_DelayTask(10);
    USBC_ANDXBYTE(0, 0xFD);

    UHC_ORXBYTE(0x32,0x01);
    MsOS_DelayTask(10);
    //UHCREG(0x32)&=0xfe;
    UHC_ANDXBYTE(0x32,0xfe);
}


void MDrv_UsbSendTestPacket(void);
void MDrv_UsbSendTestPacket(void)
{
    drvUSBHost_TurnOffPowerDownMode();
    MsOS_DelayTask(500);
    Port0_IssueTestPacket_Initial();
    MDrv_SendTestPacketByHW();
}

void MDrv_UsbSendSE0(void);
void MDrv_UsbSendSE0(void)
{
    drvUSBHost_TurnOffPowerDownMode();
    MsOS_DelayTask(500);
    Port0_IssueTestPacket_Initial();
    IssueSE0();
}

void MDrv_UsbSendTestJ(void);
void MDrv_UsbSendTestJ(void)
{
    drvUSBHost_TurnOffPowerDownMode();
    MsOS_DelayTask(500);
    Port0_IssueTestPacket_Initial();
    IssueTestJ();
}

void MDrv_UsbSendTestK(void);
void MDrv_UsbSendTestK(void)
{
    drvUSBHost_TurnOffPowerDownMode();
    MsOS_DelayTask(500);
    Port0_IssueTestPacket_Initial();
    IssueTestK();
}

U8 MDrv_GetUsbDeviceType(void)
{
    U8      u8DevType;

    if ( (gUsbDeviceState==BAD_DEVICE) ||
        (gUsbDeviceState==NO_DEVICE) ||
        (gUsbDeviceState==POWER_SAVING) )
        return USB_INTERFACE_CLASS_NONE;

    u8DevType = sAttachDevice->saCD[0].sInterface[0].bInterfaceClass;


    return (u8DevType);
}


void MDrv_GetUsbString(U8 u8StrID, S8 *pStrBuf, U8 u8BufLen)
{
    U8 ii;


    pStrBuf[0] = 0;

    if (u8StrID == USB_STR_VENDOR)
    {
        for (ii=0; ii<u8BufLen; ii++)
        {
            pStrBuf[ii] = sAttachDevice->bStringManufacture[ii];
            if (pStrBuf[ii] == 0)
                break;
        }
    }
    else if (u8StrID == USB_STR_PRODUCT)
    {
        for (ii=0; ii<u8BufLen; ii++)
        {
            pStrBuf[ii] = sAttachDevice->bStringProduct[ii];
            if (pStrBuf[ii] == 0)
                break;
        }
    }


}

U8 MDrv_USBGetqTDTimeoutValue(void)
{
    return gUsbTimeout;
}

void MDrv_USBSetqTDTimeoutValue(U8 u8Value)
{
    gUsbTimeout = u8Value;
}

U8 MDrv_USBGetIORetryCount(void)
{
    return gUsbRetryCount;
}

void MDrv_USBSetIORetryCount(U8 u8Value)
{
    gUsbRetryCount = u8Value;
}

void MDrv_USBGetVIDPID(U16 *pVID, U16 *pPID)
{
    *pVID = ((U16) sAttachDevice->sDD.bVIDHighByte << 8 ) |
                  (U16) sAttachDevice->sDD.bVIDLowByte;

    *pPID = ((U16) sAttachDevice->sDD.bPIDHighByte<< 8 ) |
                  (U16) sAttachDevice->sDD.bPIDLowByte;
}

void MDrv_GetUsbStorString(U8 uLun, U8 u8StrID, S8 *pStrBuf, U8 u8BufLen);
void MDrv_GetUsbStorString(U8 uLun, U8 u8StrID, S8 *pStrBuf, U8 u8BufLen)
{
    U8 ii;

    pStrBuf[0] = 0;

    if (u8StrID == USB_STR_VENDOR)
    {
        for (ii=0; ii<u8BufLen; ii++)
        {
            if (ii >= 8)
                break;

            pStrBuf[ii] = Mass_stor_us1.Mass_stor_device[uLun].u8VendorID[ii];
        }
        pStrBuf[ii] = 0; //Null terminal
    }
    else if (u8StrID == USB_STR_PRODUCT)
    {
        for (ii=0; ii<u8BufLen; ii++)
        {
            if (ii >= 16)
                break;

            pStrBuf[ii] = Mass_stor_us1.Mass_stor_device[uLun].u8ProductID[ii];
        }
        pStrBuf[ii] = 0; //Null terminal
    }

}

U8 MDrv_GetUsbDevInterfaceClass(void);
U8 MDrv_GetUsbDevInterfaceClass(void)
{
    U8      u8DevType;

    u8DevType = sAttachDevice->saCD[0].sInterface[0].bInterfaceClass;

    return (u8DevType);
}

#endif //#if defined(MSOS_TYPE_NOS)


