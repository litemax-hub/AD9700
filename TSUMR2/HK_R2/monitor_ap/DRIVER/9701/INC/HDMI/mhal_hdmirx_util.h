/*------------------------------------------------------------------------------
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
 *----------------------------------------------------------------------------*/

#ifndef MHAL_HDMIRX_UTIL_H
#define MHAL_HDMIRX_UTIL_H
#if 1
#if defined(__KERNEL__) /* Linux Kernel */
#include <linux/io.h>
#else
//@@@@@@#include "pqu_port.h"
#endif
#include "HDMI_RIU.h"


#define MHAL_HDMIRX_MSG_DEBUG(format, args...)      printf(format, ##args)
#define MHAL_HDMIRX_MSG_INFO(format, args...)       printf(format, ##args)
#define MHAL_HDMIRX_MSG_WARNING(format, args...)    printf(format, ##args)
#define MHAL_HDMIRX_MSG_ERROR(format, args...)      printf(format, ##args)
#define MHAL_HDMIRX_MSG_FATAL(format, args...)      printf(format, ##args)


MS_U8 *g_HDMIRx_RIUBase1;
MS_U8 *g_HDMIRx_RIUBaseDDC;
MS_U8 *g_HDMIRx_RIUBaseSCDC;
MS_U8 *g_HDMIRx_RIUBasePM_SLEEP;
MS_U8 *g_HDMIRx_RIUBasePHYPM;
MS_U8 *g_HDMIRx_RIUBaseEFUSE_0;
MS_U8 *g_HDMIRx_RIUBasePM_TOP;


// for general hdmi bank,
#define vIO32WriteFld_1(reg32, val32, fld) \
    __vIO32WriteFldMulti(0, g_HDMIRx_RIUBase1 + (reg32), val32, fld)
#define vIO32WriteFldMask_1(reg32, val32, fld) \
    __vIO32WriteFldMulti(1, g_HDMIRx_RIUBase1 + (reg32), val32, fld)

#define u4IO32ReadFld_1(reg32, fld)\
    u4IO32ReadFld(g_HDMIRx_RIUBase1 + (reg32), fld)

#define u2IO32Read2B_1(reg32)\
    u2IO32Read2B(g_HDMIRx_RIUBase1 + (reg32))

#define vIO32Write2B_1(reg32,val16)\
    vIO32Write2B(g_HDMIRx_RIUBase1 + (reg32),val16)

// ddc
#define vIO32WriteFld_DDC(reg32, val32, fld) \
    __vIO32WriteFldMulti(0, g_HDMIRx_RIUBaseDDC + (reg32), val32, fld)
#define u4IO32ReadFld_DDC(reg32, fld)\
    u4IO32ReadFld(g_HDMIRx_RIUBaseDDC + (reg32), fld)
#define u2IO32Read2B_DDC(reg32)\
    u2IO32Read2B(g_HDMIRx_RIUBaseDDC + (reg32))
#define vIO32Write2B_DDC(reg32,val16)\
    vIO32Write2B(g_HDMIRx_RIUBaseDDC + (reg32),val16)

// scdc
#define vIO32WriteFld_SCDC(reg32, val32, fld) \
    __vIO32WriteFldMulti(0, g_HDMIRx_RIUBaseSCDC + (reg32), val32, fld)
#define u4IO32ReadFld_SCDC(reg32, fld)\
    u4IO32ReadFld(g_HDMIRx_RIUBaseSCDC + (reg32), fld)
#define u2IO32Read2B_SCDC(reg32)\
    u2IO32Read2B(g_HDMIRx_RIUBaseSCDC + (reg32))
#define vIO32Write2B_SCDC(reg32,val16)\
    vIO32Write2B(g_HDMIRx_RIUBaseSCDC + (reg32),val16)

// scdc_xa8
#define vIO32WriteFld_SCDC_XA8(reg32, val32, fld) \
    __vIO32WriteFldMulti(0, g_HDMIRx_RIUBaseSCDC + (reg32), val32, fld)
#define u4IO32ReadFld_SCDC_XA8(reg32, fld)\
    u4IO32ReadFld(g_HDMIRx_RIUBaseSCDC + (reg32), fld)
#define u2IO32Read2B_SCDC_XA8(reg32)\
    u2IO32Read2B(g_HDMIRx_RIUBaseSCDC + (reg32))
#define vIO32Write2B_SCDC_XA8(reg32,val16)\
    vIO32Write2B(g_HDMIRx_RIUBaseSCDC + (reg32),val16)

// pm_sleep
#define vIO32WriteFld_PM_SLEEP(reg32, val32, fld) \
    __vIO32WriteFldMulti(0, g_HDMIRx_RIUBasePM_SLEEP + (reg32), val32, fld)
#define u4IO32ReadFld_PM_SLEEP(reg32, fld)\
    u4IO32ReadFld(g_HDMIRx_RIUBasePM_SLEEP + (reg32), fld)
#define u2IO32Read2B_PM_SLEEP(reg32)\
    u2IO32Read2B(g_HDMIRx_RIUBasePM_SLEEP + (reg32))
#define vIO32Write2B_PM_SLEEP(reg32,val16)\
    vIO32Write2B(g_HDMIRx_RIUBasePM_SLEEP + (reg32),val16)

// hd21_phy_pm
#define vIO32WriteFld_PM_PHY(reg32, val32, fld) \
    __vIO32WriteFldMulti(0, g_HDMIRx_RIUBasePHYPM + (reg32), val32, fld)
#define u4IO32ReadFld_PM_PHY(reg32, fld)\
    u4IO32ReadFld(g_HDMIRx_RIUBasePHYPM + (reg32), fld)
#define u2IO32Read2B_PM_PHY(reg32)\
    u2IO32Read2B(g_HDMIRx_RIUBasePHYPM + (reg32))
#define vIO32Write2B_PM_PHY(reg32,val16)\
    vIO32Write2B(g_HDMIRx_RIUBasePHYPM + (reg32),val16)

//efuse
#define vIO32WriteFld_EFUSE0(reg32, val32, fld) \
    __vIO32WriteFldMulti(0, g_HDMIRx_RIUBaseEFUSE_0 + (reg32), val32, fld)
#define u4IO32ReadFld_EFUSE0(reg32, fld)\
    u4IO32ReadFld(g_HDMIRx_RIUBaseEFUSE_0 + (reg32), fld)
#define u2IO32Read2B_EFUSE0(reg32)\
    u2IO32Read2B(g_HDMIRx_RIUBaseEFUSE_0 + (reg32))
#define vIO32Write2B_EFUSE0(reg32,val16)\
    vIO32Write2B(g_HDMIRx_RIUBaseEFUSE_0 + (reg32),val16)

 //pm_top
#define vIO32WriteFld_PM_TOP(reg32, val32, fld) \
    __vIO32WriteFldMulti(0, g_HDMIRx_RIUBasePM_TOP + (reg32), val32, fld)
#define u4IO32ReadFld_PM_TOP(reg32, fld)\
    u4IO32ReadFld(g_HDMIRx_RIUBasePM_TOP + (reg32), fld)
#define u2IO32Read2B_PM_TOP(reg32)\
    u2IO32Read2B(g_HDMIRx_RIUBasePM_TOP + (reg32))
#define vIO32Write2B_PM_TOP(reg32,val16)\
    vIO32Write2B(g_HDMIRx_RIUBasePM_TOP + (reg32),val16)

//-------------------------------------------------------------------------------------------------
//  Register base
//-------------------------------------------------------------------------------------------------
//PORT_BANK
#define REG_BANK_P0_OFFSET           0x00000UL
#define REG_BANK_P1_OFFSET           0x01300UL
#define REG_BANK_P2_OFFSET           0x02600UL


#if 0
// PM_SCDC
#define REG_PM_SCDC_P0_OFFSET           0x0UL
#define REG_PM_SCDC_P1_OFFSET           0x100UL
#define REG_PM_SCDC_P2_OFFSET           0x200UL
#define REG_PM_SCDC_P3_OFFSET           0x300UL
#else
// PM_SCDC
#define REG_PM_SCDC_P0_OFFSET           0x0UL
#define REG_PM_SCDC_P1_OFFSET           0x100UL
#define REG_PM_SCDC_P2_OFFSET           0x200UL
#endif

// DVI_PHY_PM
#define REG_DVI_PHY2P1_PM_P0_OFFSET     0x0UL
#define REG_DVI_PHY2P1_PM_P1_OFFSET     0x100UL
#define REG_DVI_PHY2P1_PM_P2_OFFSET     0x200UL
#define REG_DVI_PHY2P1_PM_P3_OFFSET     0x300UL

// DVI_PHY2P1
#define REG_DVI_PHY2P1_P0_OFFSET        0x0UL
#define REG_DVI_PHY2P1_P1_OFFSET        0xF00UL
#define REG_DVI_PHY2P1_P2_OFFSET        0x1E00UL
#define REG_DVI_PHY2P1_P3_OFFSET        0x2D00UL

// DVI_DTOP_DUAL
#define REG_DVI_DTOP_P0_OFFSET          0x0UL
#define REG_DVI_DTOP_P1_OFFSET          0x400UL
#define REG_DVI_DTOP_P2_OFFSET          0x800UL
#define REG_DVI_DTOP_P3_OFFSET          0xC00UL

//HDMIRX_P0_PKT_DEP 
#define REG_PKT_DEP_P0_OFFSET          0x00000UL
#define REG_PKT_DEP_P1_OFFSET          0x01300UL
#define REG_PKT_DEP_P2_OFFSET          0x02600UL

//HDMIRX_P0_PKT_DEC
#define REG_PKT_DEC_P0_OFFSET          0x00000UL
#define REG_PKT_DEC_P1_OFFSET          0x01300UL
#define REG_PKT_DEC_P2_OFFSET          0x02600UL


// HDMIRX_VE
#define REG_HDMIRX_VE_P0_OFFSET         0x0UL
#define REG_HDMIRX_VE_P1_OFFSET         0x100UL
#define REG_HDMIRX_VE_P2_OFFSET         0x200UL
#define REG_HDMIRX_VE_P3_OFFSET         0x300UL

//tgen
#define REG_TG0_OFFSET     0x0000UL
#define REG_TG1_OFFSET     0x0100UL
#define REG_TGNONE_OFFSET     0xFFFFUL

//tgen
#define REG_DSCD0_OFFSET     0x0000UL
#define REG_DSCD1_OFFSET     0x0100UL

// PM
#define REG_DDC_BASE                0x0UL
#define REG_PM_SCDC_P0_BASE         0x0UL
#define REG_PM_SLEEP_BASE           0x0UL
#define REG_PM_SCDC_P1_BASE         0x0UL
#define REG_PM_SCDC_P2_BASE         0x0UL
#define REG_PM_SCDC_P3_BASE         0x0UL
#define REG_PHY2P1_PM_P0_BASE       0x0UL
#define REG_PHY2P1_PM_P1_BASE       0x0UL
#define REG_PHY2P1_PM_P2_BASE       0x0UL
#define REG_PHY2P1_PM_P3_BASE       0x0UL

//HDCP_BANK
#define REG_HDCP_BANK_P0_OFFSET           0x0000UL
#define REG_HDCP_BANK_P1_OFFSET           0x1300UL
#define REG_HDCP_BANK_P2_OFFSET           0x2600UL

#if 0
// NONPM
#define REG_CLKGEN0_BASE            0x0UL
#define REG_CLKGEN1_BASE            0x0UL

//#define REG_DVI_DTOP_DUAL_P0_BASE       0x0UL
#define REG_POWERSAVE_P0_BASE           0x0UL
//#define REG_HDCP_DUAL_P0_BASE           0x0UL
#define REG_HDMIRX_DTOP_PKT_P0_BASE     0x0UL

//#define REG_DVI_DTOP_DUAL_P1_BASE       0x0UL
#define REG_POWERSAVE_P1_BASE           0x0UL
//#define REG_HDCP_DUAL_P1_BASE           0x0UL
#define REG_HDMIRX_DTOP_PKT_P1_BASE     0x0UL

#define REG_DVI_DTOP_DUAL_P2_BASE       0x0UL
#define REG_POWERSAVE_P2_BASE           0x0UL
#define REG_HDCP_DUAL_P2_BASE           0x0UL
#define REG_HDMIRX_DTOP_PKT_P2_BASE     0x0UL

#define REG_DVI_DTOP_DUAL_P3_BASE       0x0UL
#define REG_POWERSAVE_P3_BASE           0x0UL
#define REG_HDCP_DUAL_P3_BASE           0x0UL
#define REG_HDMIRX_DTOP_PKT_P3_BASE     0x0UL

#define REG_HDMIRX_VE_U0_BASE           0x0UL
#define REG_HDMIRX_VE_U1_BASE           0x0UL
#define REG_HDMIRX_VE_U2_BASE           0x0UL
#define REG_HDMIRX_VE_U3_BASE           0x0UL

#define REG_HDMI_DUAL_0_BASE           0x0UL
#define REG_HDMI2_DUAL_0_BASE           0x0UL
#define REG_HDMI3_DUAL_0_BASE          0x0UL
#define REG_HDMI_EM_U0_BASE             0x0UL

#define REG_COMBO_GP_TOP_BASE          0x0UL

#define REG_CHIPTOP_GPIO_FUNC_MUX_BASE  0x0UL

#define REG_PHY2P1_0_P0_BASE           0x0UL
#define REG_PHY2P1_1_P0_BASE          0x0UL
#define REG_PHY2P1_2_P0_BASE          0x0UL
#define REG_PHY2P1_3_P0_BASE           0x0UL
#define REG_PHY2P1_4_P0_BASE           0x0UL
#define REG_PHY_TRAIN_P0_BASE           0x0UL

#define REG_PHY2P1_0_P1_BASE            0x0UL
#define REG_PHY2P1_1_P1_BASE            0x0UL
#define REG_PHY2P1_2_P1_BASE            0x0UL
#define REG_PHY2P1_3_P1_BASE            0x0UL
#define REG_PHY2P1_4_P1_BASE            0x0UL
#define REG_PHY_TRAIN_P1_BASE           0x0UL

#define REG_PHY2P1_0_P2_BASE            0x0UL
#define REG_PHY2P1_1_P2_BASE            0x0UL
#define REG_PHY2P1_2_P2_BASE            0x0UL
#define REG_PHY2P1_3_P2_BASE            0x0UL
#define REG_PHY2P1_4_P2_BASE            0x0UL
#define REG_PHY_TRAIN_P2_BASE           0x0UL

#define REG_PHY2P1_0_P3_BASE            0x0UL
#define REG_PHY2P1_1_P3_BASE            0x0UL
#define REG_PHY2P1_2_P3_BASE            0x0UL
#define REG_PHY2P1_3_P3_BASE            0x0UL
#define REG_PHY2P1_4_P3_BASE            0x0UL
#define REG_PHY_TRAIN_P3_BASE           0x0UL
//#define REG_HDCPKEY_BASE                0x0UL//bank_hdcpkey
// SC
#define REG_SCALER_BASE                 0x0UL
#endif
#define REG_DDC_SRAM_SEL                 REG_DDC_22_L
    #define REG_DDC_SRAM_SEL_MASK        BMASK(12:11)

#define REG_DDC_A_ACCESS                 REG_DDC_21_L
    #define REG_DDC_A_ACCESS_BITS(x)     BITS(9:9,x)
    #define REG_DDC_A_ACCESS_MASK        BMASK(9:9)
#define REG_DDC_A_WRITEADDR              REG_DDC_23_L
    #define REG_DDC_A_WRITEADDR_BITS(x)  BITS(15:8,x)
    #define REG_DDC_A_WRITEADDR_MASK     BMASK(15:8)
#define REG_DDC_A_WRITEDATA              REG_DDC_24_L
    #define REG_DDC_A_WRITEDATA_BITS(x)  BITS(7:0,x)
    #define REG_DDC_A_WRITEDATA_MASK     BMASK(7:0)
#define REG_DDC_A_WRITEPULSE             REG_DDC_21_L
    #define REG_DDC_A_WRITEPULSE_BITS(x) BITS(15:15,x)
    #define REG_DDC_A_WRITEPULSE_MASK    BMASK(15:15)
#define REG_DDC_A_READDATA               REG_DDC_07_L
    #define REG_DDC_A_READDATA_BITS(x)   BITS(15:8,x)
    #define REG_DDC_A_READDATA_MASK      BMASK(15:8)
#define REG_DDC_A_READPULSE              REG_DDC_21_L
    #define REG_DDC_A_READPULSE_BITS(x)  BITS(14:14,x)
    #define REG_DDC_A_READPULSE_MASK     BMASK(14:14)
#define REG_DDC_A_SRAM_BASEADDR          REG_DDC_75_L
    #define REG_DDC_A_BASEADDR_BITS(x)  BITS(12:8,x)
    #define REG_DDC_A_BASEADDR_MASK     BMASK(12:8)

#define REG_DDC_D_ACCESS                 REG_DDC_21_L
    #define REG_DDC_D_ACCESS_BITS(x)     BITS(9:9,x)
    #define REG_DDC_D_ACCESS_MASK        BMASK(9:9)
#define REG_DDC_D_WRITEADDR              REG_DDC_23_L
    #define REG_DDC_D_WRITEADDR_BITS(x)  BITS(15:8,x)
    #define REG_DDC_D_WRITEADDR_MASK     BMASK(15:8)
#define REG_DDC_D_WRITEDATA              REG_DDC_24_L
    #define REG_DDC_D_WRITEDATA_BITS(x)  BITS(7:0,x)
    #define REG_DDC_D_WRITEDATA_MASK     BMASK(7:0)
#define REG_DDC_D_WRITEPULSE             REG_DDC_21_L
    #define REG_DDC_D_WRITEPULSE_BITS(x) BITS(15:15,x)
    #define REG_DDC_D_WRITEPULSE_MASK    BMASK(15:15)
#define REG_DDC_D_READDATA               REG_DDC_07_L
    #define REG_DDC_D_READDATA_BITS(x)   BITS(15:8,x)
    #define REG_DDC_D_READDATA_MASK      BMASK(15:8)
#define REG_DDC_D_READPULSE              REG_DDC_21_L
    #define REG_DDC_D_READPULSE_BITS(x)  BITS(14:14,x)
    #define REG_DDC_D_READPULSE_MASK     BMASK(14:14)
#define REG_DDC_D0_SRAM_BASEADDR         REG_DDC_76_L
    #define REG_DDC_D0_BASEADDR_BITS(x)  BITS(4:0,x)
    #define REG_DDC_D0_BASEADDR_MASK     BMASK(4:0)
#define REG_DDC_D1_SRAM_BASEADDR         REG_DDC_76_L
    #define REG_DDC_D1_BASEADDR_BITS(x)  BITS(12:8,x)
    #define REG_DDC_D1_BASEADDR_MASK     BMASK(12:8)
#define REG_DDC_D2_SRAM_BASEADDR         REG_DDC_77_L
    #define REG_DDC_D2_BASEADDR_BITS(x)  BITS(4:0,x)
    #define REG_DDC_D2_BASEADDR_MASK     BMASK(4:0)
#define REG_DDC_D3_SRAM_BASEADDR         REG_DDC_77_L
    #define REG_DDC_D3_BASEADDR_BITS(x)  BITS(12:8,x)
    #define REG_DDC_D3_BASEADDR_MASK     BMASK(12:8)

#define REG_DDC_DVI0_EN         (REG_DDC_22_L)//H
    #define REG_DDC_DVI0_EN_MASK        15
#define REG_DDC_DVI1_EN         (REG_DDC_26_L)//H
    #define REG_DDC_DVI1_EN_MASK        15
#define REG_DDC_DVI2_EN         (REG_DDC_2C_L)//L
    #define REG_DDC_DVI2_EN_MASK        7
#define REG_DDC_DVI3_EN         (REG_DDC_54_L)//L
    #define REG_DDC_DVI3_EN_MASK        7
#define REG_DDC_ADC0_EN         (REG_DDC_24_L)//H
    #define REG_DDC_ADC0_EN_MASK        15

#define REG_DDC_DVI0_WP         (REG_DDC_22_L)//H
    #define REG_DDC_DVI0_WP_MASK        13
#define REG_DDC_DVI1_WP         (REG_DDC_26_L)//H
    #define REG_DDC_DVI1_WP_MASK        13
#define REG_DDC_DVI2_WP        (REG_DDC_2C_L)//L
    #define REG_DDC_DVI2_WP_MASK        5
#define REG_DDC_DVI3_WP         (REG_DDC_54_L)//L
    #define REG_DDC_DVI3_WP_MASK        5
#define REG_DDC_ADC0_WP        (REG_DDC_24_L)//H
    #define REG_DDC_ADC0_WP_MASK        13

#define DEF_HDCP_DUAL_BASE  0x171200
#define DEF_HDCP_DUAL_00_L  DEF_HDCP_DUAL_BASE + 0x00
#define DEF_HDCP_DUAL_00_H  DEF_HDCP_DUAL_BASE + 0x01
#define DEF_HDCP_DUAL_01_L  DEF_HDCP_DUAL_BASE + 0x02
#define DEF_HDCP_DUAL_01_H  DEF_HDCP_DUAL_BASE + 0x03
#define DEF_HDCP_DUAL_02_L  DEF_HDCP_DUAL_BASE + 0x04
#define DEF_HDCP_DUAL_02_H  DEF_HDCP_DUAL_BASE + 0x05
#define DEF_HDCP_DUAL_03_L  DEF_HDCP_DUAL_BASE + 0x06
#define DEF_HDCP_DUAL_03_H  DEF_HDCP_DUAL_BASE + 0x07

#define ALIGN_4(_x_)                (((_x_) + 3) & ~3)
#define ALIGN_8(_x_)                (((_x_) + 7) & ~7)
#define ALIGN_16(_x_)               (((_x_) + 15) & ~15)           // No data type specified, optimized by complier
#define ALIGN_32(_x_)               (((_x_) + 31) & ~31)           // No data type specified, optimized by complier

#ifndef MIN
#define MIN(_a_, _b_)               ((_a_) < (_b_) ? (_a_) : (_b_))
#endif
#ifndef MAX
#define MAX(_a_, _b_)               ((_a_) > (_b_) ? (_a_) : (_b_))
#endif

#ifndef BIT //for Linux_kernel type, BIT redefined in <linux/bitops.h>
#define BIT(_bit_)                  (1 << (_bit_))
#endif
#define BIT_(x)                     BIT(x) //[OBSOLETED] //TODO: remove it later
#define BITS(_bits_, _val_)         ((BIT(((1)?_bits_)+1)-BIT(((0)?_bits_))) & (_val_<<((0)?_bits_)))
#define BMASK(_bits_)               (BIT(((1)?_bits_)+1)-BIT(((0)?_bits_)))
#define BIT_64(_bit_)                (1ULL << (_bit_))

#endif

// #define FLD_BMSK(a,b)      Fld(((a)-(b)+1),(b),AC_MSKW10)
#define FLD_BMSK(_bits_)      Fld((((1)?_bits_)-((0)?_bits_)+1),((0)?_bits_),AC_MSKW10)
#define FLD_BIT(x)      Fld(1,(x),AC_MSKW10)

#ifdef DUMP_INFO_ENABLE
void KHal_DumpHDMIInfo(MS_U32 g_u32_loop_cnt,stHDMI_POLLING_INFO *stHDMIPollingInfo);
#endif
void KHal_HDMI_Cmd(MS_U32 u32_p0, MS_U32 u32_p1, MS_U32 u32_p2);

#endif  //#ifndef MHAL_HDMIRX_HW_H
