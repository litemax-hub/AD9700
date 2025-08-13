////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// DRAM memory map
//
// Every Module Memory Mapping need 4 define,
// and check code in "msAPI_Memory_DumpMemoryMap"
// 1. XXX_AVAILABLE : For get avaialble memory start address
// 2. XXX_ADR       : Real Address with Alignment
// 3. XXX_GAP_CHK   : For Check Memory Gap, for avoid memory waste
// 4. XXX_LEN       : For the Memory size of this Module usage
////////////////////////////////////////////////////////////////////////////////
#define MIU_DRAM_LEN                0x0008000000
#define MIU_DRAM_LEN0               0x0008000000

#define ENABLE_MIU_1                0
#define MIU_INTERVAL                0x0000000000

////////////////////////////////////////////////////////////////////////////////
//MEMORY TYPE
////////////////////////////////////////////////////////////////////////////////
#define MIU0                        (0x0000)
#define MIU1                        (0x0001)

#define SW                          (0x0000 << 1)
#define HW                          (0x0001 << 1)
#define SW_HW                       (0x0002 << 1)

#define UNCACHED                    (0x0000 << 3)
#define WRITE_THROUGH               (0x0001 << 3)
#define WRITE_COMBINING             (0x0002 << 3)
#define WRITE_PROTECT               (0x0003 << 3)
#define WRITE_BACK                  (0x0004 << 3)


//MIU_0_START
/* AEON_MEM   */
//co_buffer L0
#define AEON_MEM_AVAILABLE                                     0x0000000000
#define AEON_MEM_ADR                                           0x0000000000  //Alignment 0
#define AEON_MEM_GAP_CHK                                       0x0000000000
#define AEON_MEM_LEN                                           0x0000200000
#define AEON_MEM_MEMORY_TYPE                                   (MIU0 | SW | WRITE_BACK)

/* BEON_MEM   */
//co_buffer L0
#define BEON_MEM_AVAILABLE                                     0x0000000000//0x0000200000
#define BEON_MEM_ADR                                           0x0000000000  //Alignment 0x10000
#define BEON_MEM_GAP_CHK                                       0x0000000000
#define BEON_MEM_LEN                                           0x0000200000
#define BEON_MEM_MEMORY_TYPE                                   (MIU0 | SW | WRITE_BACK)

/* CP_R2_MEM   */
//co_buffer L0
#define CP_R2_MEM_AVAILABLE                                    0x0000400000
#define CP_R2_MEM_ADR                                          0x0000400000  //Alignment 0x10000
#define CP_R2_MEM_GAP_CHK                                      0x0000000000
#define CP_R2_MEM_LEN                                          0x0000000000
#define CP_R2_MEM_MEMORY_TYPE                                  (MIU0 | SW | WRITE_BACK)

/* SECU_R2_MEM   */
//co_buffer L0
#define SECU_R2_MEM_AVAILABLE                                  0x0000400000
#define SECU_R2_MEM_ADR                                        0x0000400000  //Alignment 0x10000
#define SECU_R2_MEM_GAP_CHK                                    0x0000000000
#define SECU_R2_MEM_LEN                                        0x0000200000
#define SECU_R2_MEM_MEMORY_TYPE                                (MIU0 | SW | WRITE_BACK)

/* MENULOAD   */
//co_buffer L0
#define MENULOAD_AVAILABLE                                     0x0000600000
#define MENULOAD_ADR                                           0x0000600000  //Alignment 0
#define MENULOAD_GAP_CHK                                       0x0000000000
#define MENULOAD_LEN                                           0x0000001000
#define MENULOAD_MEMORY_TYPE                                   (MIU0 | SW | WRITE_BACK)

/* AUTOLOAD_OD   */
//co_buffer L0
#define AUTOLOAD_OD_AVAILABLE                                  0x0000601000
#define AUTOLOAD_OD_ADR                                        0x0000601000  //Alignment 0
#define AUTOLOAD_OD_GAP_CHK                                    0x0000000000
#define AUTOLOAD_OD_LEN                                        0x0000020000
#define AUTOLOAD_OD_MEMORY_TYPE                                (MIU0 | SW | WRITE_BACK)

/* AUTOLOAD_GMA   */
//co_buffer L0
#define AUTOLOAD_GMA_AVAILABLE                                 0x0000621000
#define AUTOLOAD_GMA_ADR                                       0x0000621000  //Alignment 0
#define AUTOLOAD_GMA_GAP_CHK                                   0x0000000000
#define AUTOLOAD_GMA_LEN                                       0x0000004000
#define AUTOLOAD_GMA_MEMORY_TYPE                               (MIU0 | SW | WRITE_BACK)

/* MENULOAD_BUFFER   */
//co_buffer L0
#define MENULOAD_BUFFER_AVAILABLE                              0x0000625000
#define MENULOAD_BUFFER_ADR                                    0x0000625000  //Alignment 0
#define MENULOAD_BUFFER_GAP_CHK                                0x0000000000
#define MENULOAD_BUFFER_LEN                                    0x0000004000
#define MENULOAD_BUFFER_MEMORY_TYPE                            (MIU0 | SW | WRITE_BACK)

/* DOWNLOAD_BUFFER   */
//co_buffer L0
#define DOWNLOAD_BUFFER_AVAILABLE                              0x0000629000
#define DOWNLOAD_BUFFER_ADR                                    0x0000629000  //Alignment 0
#define DOWNLOAD_BUFFER_GAP_CHK                                0x0000000000
#define DOWNLOAD_BUFFER_LEN                                    0x0000200000
#define DOWNLOAD_BUFFER_MEMORY_TYPE                            (MIU0 | SW | WRITE_BACK)

/* FILE_SYSTEM_POOL   */
//co_buffer L0
#define FILE_SYSTEM_POOL_AVAILABLE                             0x0000829000
#define FILE_SYSTEM_POOL_ADR                                   0x0000829000  //Alignment 0x01000
#define FILE_SYSTEM_POOL_GAP_CHK                               0x0000000000
#define FILE_SYSTEM_POOL_LEN                                   0x0000060000
#define FILE_SYSTEM_POOL_MEMORY_TYPE                           (MIU0 | SW | WRITE_BACK)

/* MBX_XD2MIU   */
//co_buffer L0
#define MBX_XD2MIU_AVAILABLE                                   0x0000889000
#define MBX_XD2MIU_ADR                                         0x0000890000  //Alignment 0x10000
#define MBX_XD2MIU_GAP_CHK                                     0x0000007000
#define MBX_XD2MIU_LEN                                         0x0000010000
#define MBX_XD2MIU_MEMORY_TYPE                                 (MIU0 | SW | WRITE_BACK)

/* AudioPCM   */
//co_buffer L0
#define AudioPCM_AVAILABLE                                     0x00008A0000
#define AudioPCM_ADR                                           0x00008A0000  //Alignment 0
#define AudioPCM_GAP_CHK                                       0x0000000000
#define AudioPCM_LEN                                           0x0000000000
#define AudioPCM_MEMORY_TYPE                                   (MIU0 | SW | WRITE_BACK)

/* PQ_MAIN   */
//co_buffer L0
#define PQ_MAIN_AVAILABLE                                      0x00008A0000
#define PQ_MAIN_ADR                                            0x00008A0000  //Alignment 0
#define PQ_MAIN_GAP_CHK                                        0x0000000000
#define PQ_MAIN_LEN                                            0x0000000000
#define PQ_MAIN_MEMORY_TYPE                                    (MIU0 | SW | WRITE_BACK)

/* PQ_MAIN_EX   */
//co_buffer L0
#define PQ_MAIN_EX_AVAILABLE                                   0x00008A0000
#define PQ_MAIN_EX_ADR                                         0x00008A0000  //Alignment 0
#define PQ_MAIN_EX_GAP_CHK                                     0x0000000000
#define PQ_MAIN_EX_LEN                                         0x0000000000
#define PQ_MAIN_EX_MEMORY_TYPE                                 (MIU0 | SW | WRITE_BACK)

/* PQ_TEXT_MAIN   */
//co_buffer L0
#define PQ_TEXT_MAIN_AVAILABLE                                 0x00008A0000
#define PQ_TEXT_MAIN_ADR                                       0x00008A0000  //Alignment 0
#define PQ_TEXT_MAIN_GAP_CHK                                   0x0000000000
#define PQ_TEXT_MAIN_LEN                                       0x0000000000
#define PQ_TEXT_MAIN_MEMORY_TYPE                               (MIU0 | SW | WRITE_BACK)

/* MS_TOOL   */
//co_buffer L0
#define MS_TOOL_AVAILABLE                                      0x00008A0000
#define MS_TOOL_ADR                                            0x00008A0000  //Alignment 0x01000
#define MS_TOOL_GAP_CHK                                        0x0000000000
#define MS_TOOL_LEN                                            0x0000200000
#define MS_TOOL_MEMORY_TYPE                                    (MIU0 | SW | WRITE_BACK)

/* GOP_GWIN_RB   */
//co_buffer L0
#define GOP_GWIN_RB_AVAILABLE                                  0x0000AA0000
#define GOP_GWIN_RB_ADR                                        0x0000AA0000  //Alignment 0x00008
#define GOP_GWIN_RB_GAP_CHK                                    0x0000000000
#define GOP_GWIN_RB_LEN                                        0x0000000000
#define GOP_GWIN_RB_MEMORY_TYPE                                (MIU0 | SW | WRITE_BACK)

/* GOP_REGDMABASE   */
//co_buffer L0
#define GOP_REGDMABASE_AVAILABLE                               0x0000AA0000
#define GOP_REGDMABASE_ADR                                     0x0000AA0000  //Alignment 0x04000
#define GOP_REGDMABASE_GAP_CHK                                 0x0000000000
#define GOP_REGDMABASE_LEN                                     0x0000000000
#define GOP_REGDMABASE_MEMORY_TYPE                             (MIU0 | SW | WRITE_BACK)

/* POOL_BUFFER   */
//co_buffer L0
#define POOL_BUFFER_AVAILABLE                                  0x0000AA0000
#define POOL_BUFFER_ADR                                        0x0000B00000  //Alignment 0x80000
#define POOL_BUFFER_GAP_CHK                                    0x0000060000
#define POOL_BUFFER_LEN                                        0x0000000000
#define POOL_BUFFER_MEMORY_TYPE                                (MIU0 | SW | WRITE_BACK)

/* BMP_START   */
//co_buffer L0
#define BMP_START_AVAILABLE                                    0x0000B00000
#define BMP_START_ADR                                          0x0000B00000  //Alignment 0x00800
#define BMP_START_GAP_CHK                                      0x0000000000
#define BMP_START_LEN                                          0x0000000000
#define BMP_START_MEMORY_TYPE                                  (MIU0 | SW | WRITE_BACK)

/* BMP_PALETTE_BUF_START   */
//co_buffer L0
#define BMP_PALETTE_BUF_START_AVAILABLE                        0x0000B00000
#define BMP_PALETTE_BUF_START_ADR                              0x0000B00000  //Alignment 0
#define BMP_PALETTE_BUF_START_GAP_CHK                          0x0000000000
#define BMP_PALETTE_BUF_START_LEN                              0x0000000000
#define BMP_PALETTE_BUF_START_MEMORY_TYPE                      (MIU0 | SW | WRITE_BACK)

/* STRING_START   */
//co_buffer L0
#define STRING_START_AVAILABLE                                 0x0000B00000
#define STRING_START_ADR                                       0x0000B00000  //Alignment 0x00800
#define STRING_START_GAP_CHK                                   0x0000000000
#define STRING_START_LEN                                       0x0000000000
#define STRING_START_MEMORY_TYPE                               (MIU0 | SW | WRITE_BACK)

/* FONT_START   */
//co_buffer L0
#define FONT_START_AVAILABLE                                   0x0000B00000
#define FONT_START_ADR                                         0x0000B00000  //Alignment 0x00008
#define FONT_START_GAP_CHK                                     0x0000000000
#define FONT_START_LEN                                         0x0000000000
#define FONT_START_MEMORY_TYPE                                 (MIU0 | SW | WRITE_BACK)

/* OD_L   */
//co_buffer L0
#define OD_L_AVAILABLE                                         0x0000B00000
#define OD_L_ADR                                               0x0000B00000  //Alignment 0
#define OD_L_GAP_CHK                                           0x0000000000
#define OD_L_LEN                                               0x0000500000
#define OD_L_MEMORY_TYPE                                       (MIU0 | SW | WRITE_BACK)

/* OD_R   */
//co_buffer L0
#define OD_R_AVAILABLE                                         0x0001000000
#define OD_R_ADR                                               0x0001000000  //Alignment 0
#define OD_R_GAP_CHK                                           0x0000000000
#define OD_R_LEN                                               0x0000500000
#define OD_R_MEMORY_TYPE                                       (MIU0 | SW | WRITE_BACK)

/* OSDC_L   */
//co_buffer L0
#define OSDC_L_AVAILABLE                                       0x0001500000
#define OSDC_L_ADR                                             0x0001500000  //Alignment 0
#define OSDC_L_GAP_CHK                                         0x0000000000
#define OSDC_L_LEN                                             0x0000000000
#define OSDC_L_MEMORY_TYPE                                     (MIU0 | SW | WRITE_BACK)

/* OSDC_R   */
//co_buffer L0
#define OSDC_R_AVAILABLE                                       0x0001500000
#define OSDC_R_ADR                                             0x0001500000  //Alignment 0
#define OSDC_R_GAP_CHK                                         0x0000000000
#define OSDC_R_LEN                                             0x0000000000
#define OSDC_R_MEMORY_TYPE                                     (MIU0 | SW | WRITE_BACK)

/* FB_L0   */
//co_buffer L0
#define FB_L0_AVAILABLE                                        0x0001500000
#define FB_L0_ADR                                              0x0001500000  //Alignment 0x02000
#define FB_L0_GAP_CHK                                          0x0000000000
#define FB_L0_LEN                                              0x0003500000
#define FB_L0_MEMORY_TYPE                                      (MIU0 | SW | WRITE_BACK)

/* FB_R0   */
//co_buffer L0
#define FB_R0_AVAILABLE                                        0x0004A00000
#define FB_R0_ADR                                              0x0004A00000  //Alignment 0x02000
#define FB_R0_GAP_CHK                                          0x0000000000
#define FB_R0_LEN                                              0x0003500000
#define FB_R0_MEMORY_TYPE                                      (MIU0 | SW | WRITE_BACK)

//MIU_1_START
//MIU_END
#define MIU0_END_ADR                                           0x0007F00000
