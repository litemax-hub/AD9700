#ifndef _MHAL_HDMIRX_DUMMY_H_
#define _MHAL_HDMIRX_DUMMY_H_

/* To compile pass, we fake these information.
 */

// fake base address
// TODO
#define REG_FRL_TOP_P2_BASE       0x0000
#define REG_FRL_TOP_P3_BASE       0x0000
#define REG_FRL_DEC_P2_BASE       0x0000
#define REG_FRL_DEC_P3_BASE       0x0000
#define REG_PLL_TOP_P2_BASE       0x0000
#define REG_PLL_TOP_P3_BASE       0x0000
#define REG_DSCD_SVCE_P2_BASE     0x0000
#define REG_DSCD_SVCE_P3_BASE     0x0000
#define REG_HDMI_EM_8P_P2_BASE    0x0000
#define REG_HDMI_EM_8P_P3_BASE    0x0000
#define REG_HDMI_8P_P2_BASE       0x0000
#define REG_HDMI_8P_P3_BASE       0x0000
#define REG_HDMI2_8P_P2_BASE      0x0000
#define REG_HDMI2_8P_P3_BASE      0x0000
#define REG_HDMI3_8P_P2_BASE      0x0000
#define REG_HDMI3_8P_P3_BASE      0x0000

// PM_SCDC_XA8
#define REG_PM_SCDC_XA8_P0_OFFSET           0x0UL
#define REG_PM_SCDC_XA8_P1_OFFSET           0x100UL
#define REG_PM_SCDC_XA8_P2_OFFSET           0x200UL
#define REG_PM_SCDC_XA8_P3_OFFSET           0x300UL

#define REG_PM_SCDC_XA8_P0_BASE 0x0000
#define REG_PM_SCDC_XA8_P1_BASE 0x0000
#define REG_PM_SCDC_XA8_P2_BASE 0x0000
#define REG_PM_SCDC_XA8_P3_BASE 0x0000

#define REG_DMD_ANA_MISC_1A_L 0x0000
#define REG_DMD_ANA_MISC_47_L 0x0000

//#define REG_SECURE_TZPC_BASE 0x0000

#define UNUSED_PARA(x) x __attribute__ ((unused))

#define PM_W2BYTE W2BYTEMSK // W2BYTE
#define HDCP_W2BYTEMSK W2BYTEMSK

#endif
