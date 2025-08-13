/* SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause */
/*******************************************************************************
 * MediaTek Inc. (C) 2018. All rights reserved.
 *
 * Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 ******************************************************************************/
#ifndef _MSRIU_H_
#define _MSRIU_H_
#ifdef __KERNEL__
#include <linux/types.h>
#include <asm/io.h>
#else
#include <stdint.h>
#endif

#include "Ms_rwreg.h"

#define UNUSED_x __attribute__((unused))
/*********************************************
 * ---------------- AC_FULL/MSK ----------------
 * B3[31:24] | B2[23:16] | B1[15:8] | B0[7:0]
 * W32[31:16] | W21[23:8] | W10 [15:0]
 * DW[31:0]
 * ---------------------------------------------
 *********************************************/
#define AC_FULLB0           1
#define AC_FULLB1           2
#define AC_FULLB2           3
#define AC_FULLB3           4
#define AC_FULLW10          5
#define AC_FULLW21          6
#define AC_FULLW32          7
#define AC_FULLDW           8
#define AC_MSKB0            9
#define AC_MSKB1            10
#define AC_MSKB2            11
#define AC_MSKB3            12
#define AC_MSKW10           13
#define AC_MSKW21           14
#define AC_MSKW32           15
#define AC_MSKDW            16
/*********************************************
 * -------------------- Fld --------------------
 * wid[31:16] | shift[15:8] | ac[7:0]
 * ---------------------------------------------
 *********************************************/
#define Fld(wid, shft, ac)    (((uint32_t)wid<<16)|(shft<<8)|ac)
#define Fld_wid(fld)          (uint8_t)((fld)>>16)
#define Fld_shft(fld)         (uint8_t)((fld)>>8)
#define Fld_ac(fld)           (uint8_t)((fld))
/*********************************************
 * Prototype and Macro
 *********************************************/
static inline uint8_t UNUSED_x
    u1IO32Read1B(void *reg32);
static inline uint16_t UNUSED_x
    u2IO32Read2B(void *reg32);
static inline uint32_t UNUSED_x
    u4IO32Read4B(void *reg32);
static inline uint32_t UNUSED_x
    u4IO32ReadFld(void *reg32, uint32_t fld);
static inline void UNUSED_x
    vIO32Write1B(void *reg32, uint8_t val8);
static inline void UNUSED_x
    vIO32Write2B(void *reg32, uint16_t val16);
static inline void UNUSED_x
    vIO32Write4B(void *reg32, uint32_t val32);

#define vIO32WriteFld(reg32, val32, fld) \
    __vIO32WriteFldMulti(0, reg32, val32, fld)
#define vIO32WriteFldMask(reg32, val32, fld) \
    __vIO32WriteFldMulti(1, reg32, val32, fld)

#define vIO32WriteFldMulti(reg32, num_fld, ...) \
	__vIO32WriteFldMulti_VA_ARG(0, reg32, num_fld, __VA_ARGS__)
#define vIO32WriteFldMultiMask(reg32, num_fld, ...) \
	__vIO32WriteFldMulti_VA_ARG(1, reg32, num_fld, __VA_ARGS__)

/*********************************************
 * Functions
 *********************************************/
static uint32_t UNUSED_x __u4IO32AccessFld(
    uint8_t write, uint32_t tmp32, uint32_t val32, uint32_t fld)
{
#if 0
    uint32_t t = 0;
    switch (Fld_ac(fld)) {
    case AC_FULLB0:
    case AC_FULLB1:
    case AC_FULLB2:
    case AC_FULLB3:
        if (write == 1)
            t = (tmp32&(~((uint32_t)0xFF<<
                (8*(Fld_ac(fld)-AC_FULLB0))))) |
                ((val32&0xFF)<<(8*(Fld_ac(fld)-AC_FULLB0)));
        else
            t = (tmp32&((uint32_t)0xFF<<
                (8*(Fld_ac(fld)-AC_FULLB0)))) >>
                (8*(Fld_ac(fld)-AC_FULLB0));
        break;
    case AC_FULLW10:
    case AC_FULLW21:
    case AC_FULLW32:
        if (write == 1)
            t = (tmp32&(~((uint32_t)0xFFFF<<
                (8*(Fld_ac(fld)-AC_FULLW10))))) |
                ((val32&0xFFFF)<<(8*(Fld_ac(fld)-AC_FULLW10)));
        else
            t = (tmp32&(((uint32_t)0xFFFF<<
                (8*(Fld_ac(fld)-AC_FULLW10))))) >>
                (8*(Fld_ac(fld)-AC_FULLW10));
        break;
    case AC_FULLDW:
        t = val32;
        break;
    case AC_MSKB0:
    case AC_MSKB1:
    case AC_MSKB2:
    case AC_MSKB3:
        if (write == 1)
            t = (tmp32&(~(((((uint32_t)1<<Fld_wid(fld))-1)<<
                Fld_shft(fld))))) |
                (((val32&(((uint32_t)1<<Fld_wid(fld))-1))<<
                Fld_shft(fld)));
        else
            t = (tmp32&(((((uint32_t)1<<Fld_wid(fld))-1)<<
                Fld_shft(fld)))) >>
                Fld_shft(fld);
        break;
    case AC_MSKW10:
    case AC_MSKW21:
    case AC_MSKW32:
        if (write == 1)
            t = (tmp32&(~(((((uint32_t)1<<Fld_wid(fld))-1)<<
                Fld_shft(fld))))) |
                (((val32&(((uint32_t)1<<Fld_wid(fld))-1))<<
                Fld_shft(fld)));
        else
            t = (tmp32&(((((uint32_t)1<<Fld_wid(fld))-1)<<
                Fld_shft(fld)))) >>
                Fld_shft(fld);
        break;
    case AC_MSKDW:
        if (write == 1)
            t = (tmp32&(~(((((uint32_t)1<<Fld_wid(fld))-1)<<
                Fld_shft(fld))))) |
                (((val32&(((uint32_t)1<<Fld_wid(fld))-1))<<
                Fld_shft(fld)));
        else
            t = (tmp32&(((((uint32_t)1<<Fld_wid(fld))-1)<<
                Fld_shft(fld)))) >>
                Fld_shft(fld);
        break;
    default:
        break;
    }
    return t;
#else
        uint32_t t = 0;
    switch (Fld_ac(fld)) {
    case AC_FULLB0:
    case AC_FULLB1:
        if (write == 1)
            t = (tmp32&(~((uint32_t)0xFF<<
                (8*(Fld_ac(fld)-AC_FULLB0))))) |
                ((val32&0xFF)<<(8*(Fld_ac(fld)-AC_FULLB0)));
        else
            t = (tmp32&((uint32_t)0xFF<<
                (8*(Fld_ac(fld)-AC_FULLB0)))) >>
                (8*(Fld_ac(fld)-AC_FULLB0));
        break;
    case AC_FULLW10:
        if (write == 1)
            t = (tmp32&(~((uint32_t)0xFFFF<<
                (8*(Fld_ac(fld)-AC_FULLW10))))) |
                ((val32&0xFFFF)<<(8*(Fld_ac(fld)-AC_FULLW10)));
        else
            t = (tmp32&(((uint32_t)0xFFFF<<
                (8*(Fld_ac(fld)-AC_FULLW10))))) >>
                (8*(Fld_ac(fld)-AC_FULLW10));
        break;
    case AC_MSKB0:
    case AC_MSKB1:
        if (write == 1)
            t = (tmp32&(~(((((uint32_t)1<<Fld_wid(fld))-1)<<
                Fld_shft(fld))))) |
                (((val32&(((uint32_t)1<<Fld_wid(fld))-1))<<
                Fld_shft(fld)));
        else
            t = (tmp32&(((((uint32_t)1<<Fld_wid(fld))-1)<<
                Fld_shft(fld)))) >>
                Fld_shft(fld);
        break;
    case AC_MSKW10:
        if (write == 1)
            t = (tmp32&(~(((((uint32_t)1<<Fld_wid(fld))-1)<<
                Fld_shft(fld))))) |
                (((val32&(((uint32_t)1<<Fld_wid(fld))-1))<<
                Fld_shft(fld)));
        else
            t = (tmp32&(((((uint32_t)1<<Fld_wid(fld))-1)<<
                Fld_shft(fld)))) >>
                Fld_shft(fld);
        break;
    default:
        break;
    }
    return t;

#endif
}


static void UNUSED_x __vIO32WriteFldMulti(
    uint32_t mask, void *reg32, uint32_t val32, uint32_t fld)
{
    uint32_t tmp32;
    #if 0
    tmp32 = (mask == 0)?u4IO32Read4B(reg32):0;
    tmp32 = __u4IO32AccessFld(1, tmp32, val32, fld);
    vIO32Write4B(reg32, tmp32);
    #else
    tmp32 = (mask == 0)?msRead2Byte((uint32_t)reg32):0;
    tmp32 = __u4IO32AccessFld(1, tmp32, val32, fld);
    msWrite2ByteMask((uint32_t)reg32, (uint16_t)tmp32 , 0xFFFF);
    #endif
}


static void UNUSED_x __vIO32WriteFldMulti_VA_ARG(
	uint32_t mask, void *reg32, uint32_t num_fld, ...)
{
	uint32_t i, val32, fld, tmp32;
	va_list argp;

	tmp32 = (mask == 0)?u4IO32Read4B(reg32):0;
	va_start(argp, num_fld);
	for (i = 0; i < num_fld; i++) {
		val32 = va_arg(argp, uint32_t);
		fld = va_arg(argp, uint32_t);
		tmp32 = __u4IO32AccessFld(1, tmp32, val32, fld);
	}
	va_end(argp);
	vIO32Write4B(reg32, tmp32);
}

static inline uint8_t UNUSED_x u1IO32Read1B(void *reg32)
{
#ifdef __KERNEL__
    return readb(reg32);
#else
    return (*((volatile uint8_t*)reg32));
#endif
}
static inline uint16_t UNUSED_x u2IO32Read2B(void *reg32)
{
#ifdef __KERNEL__
    return readw(reg32);
#else
    //return (*((volatile uint16_t*)reg32));
    return msRead2Byte((uint32_t)reg32);
#endif
}
static inline uint32_t UNUSED_x u4IO32Read4B(void *reg32)
{
#ifdef __KERNEL__
    return readl(reg32);
#else
    return (*((volatile uint32_t*)reg32));
#endif
}
static inline uint32_t UNUSED_x u4IO32ReadFld(void *reg32, uint32_t fld)
{
    //return __u4IO32AccessFld(0, u4IO32Read4B(reg32), 0, fld);
    return __u4IO32AccessFld(0, msRead2Byte((uint32_t)reg32), 0, fld);
}
static inline void UNUSED_x vIO32Write1B(void *reg32, uint8_t val8)
{
#ifdef __KERNEL__
    writeb(val8, reg32);
#else
    *((volatile uint8_t*)reg32) = val8;
#endif
}
static inline void UNUSED_x vIO32Write2B(void *reg32, uint16_t val16)
{
#ifdef __KERNEL__
    writew(val16, reg32);
#else
    //*((volatile uint16_t*)reg32) = val16;
    msWrite2Byte((uint32_t)reg32, (uint16_t)val16);
#endif
}
static inline void UNUSED_x vIO32Write4B(void *reg32, uint32_t val32)
{
#ifdef __KERNEL__
    writel(val32, reg32);
#else
    *((volatile uint32_t*)reg32) = val32;
#endif
}


//#define Fld2Msk32(fld)  /*lint -save -e504 */ (((uint32_t)0xffffffff>>(32-Fld_wid(fld)))<<Fld_shft(fld))
//#define P_Fld(val,fld) ((sizeof(upk)>1)?Fld2Msk32(fld):(((uint32_t)(val)&((1<<Fld_wid(fld))-1))<<Fld_shft(fld)))


#if 0
static inline void UNUSED vIO32WriteFldMulti(void *reg32,list) /*lint -save -e506 -e504 -e514 -e62 -e737 -e572 -e961 -e648 -e701 -e732 -e571 */ \
{
	uint16_t upk;
	int32_t msk=(int32_t)(list);
	UINT8 upk;
	((UINT32)msk==0xff)?vIO32Write1B(reg32,(list)),0:(
	((UINT32)msk==0xff00)?vIO32Write1B(reg32+1,(list)>>8),0:(
	((UINT32)msk==0xff0000)?vIO32Write1B(reg32+2,(list)>>16),0:(
	((UINT32)msk==0xff000000)?vIO32Write1B(reg32+3,(list)>>24),0:(
	((UINT32)msk==0xffff)?vIO32Write2B(reg32,(list)),0:(
	((UINT32)msk==0xffff00)?vIO32Write2B(reg32+1,(list)>>8),0:(
	((UINT32)msk==0xffff0000)?vIO32Write2B(reg32+2,(list)>>16),0:(
	((UINT32)msk==0xffffffff)?vIO32Write4B(reg32,(list)),0:(
	(((UINT32)msk&0xff)&&(!((UINT32)msk&0xffffff00)))?vIO32Write1BMsk(reg32,(list),(UINT8)(UINT32)msk),0:(
	(((UINT32)msk&0xff00)&&(!((UINT32)msk&0xffff00ff)))?vIO32Write1BMsk(reg32+1,(list)>>8,(UINT8)((UINT32)msk>>8)),0:(
	(((UINT32)msk&0xff0000)&&(!((UINT32)msk&0xff00ffff)))?vIO32Write1BMsk(reg32+2,(list)>>16,(UINT8)((UINT32)msk>>16)),0:(
	(((UINT32)msk&0xff000000)&&(!((UINT32)msk&0x00ffffff)))?vIO32Write1BMsk(reg32+3,(list)>>24,(UINT8)((UINT32)msk>>24)),0:(
	(((UINT32)msk&0xffff)&&(!((UINT32)msk&0xffff0000)))?vIO32Write2BMsk(reg32,(list),(uint16_t)(UINT32)msk),0:(
	(((UINT32)msk&0xffff00)&&(!((UINT32)msk&0xff0000ff)))?vIO32Write2BMsk(reg32+1,(list)>>8,(uint16_t)((UINT32)msk>>8)),0:(
	(((UINT32)msk&0xffff0000)&&(!((UINT32)msk&0x0000ffff)))?vIO32Write2BMsk(reg32+2,(list)>>16,(uint16_t)((UINT32)msk>>16)),0:(
	((UINT32)msk)?vIO32Write4BMsk(reg32,(list),((UINT32)msk)),0:0
	)))))))))))))));
}/*lint -restore */
#endif

#endif
