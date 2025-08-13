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
#define MIU_DRAM_LEN                0x0004000000
#define MIU_DRAM_LEN0               0x0004000000

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
#define BEON_MEM_AVAILABLE                                     0x0000200000
#define BEON_MEM_ADR                                           0x0000200000  //Alignment 0x10000
#define BEON_MEM_GAP_CHK                                       0x0000000000
#define BEON_MEM_LEN                                           0x0000600000
#define BEON_MEM_MEMORY_TYPE                                   BEON_MEM_MEMORY_TYPE (MIU0 | SW |

/* MENULOAD   */
//co_buffer L0
#define MENULOAD_AVAILABLE                                     0x0000800000
#define MENULOAD_ADR                                           0x0000800000  //Alignment 0
#define MENULOAD_GAP_CHK                                       0x0000000000
#define MENULOAD_LEN                                           0x0000001000
#define MENULOAD_MEMORY_TYPE                                   (MIU0 | SW | WRITE_BACK)

/* AUTOLOAD_OD   */
//co_buffer L0
#define AUTOLOAD_OD_AVAILABLE                                  0x0000801000
#define AUTOLOAD_OD_ADR                                        0x0000801000  //Alignment 0
#define AUTOLOAD_OD_GAP_CHK                                    0x0000000000
#define AUTOLOAD_OD_LEN                                        0x0000020000
#define AUTOLOAD_OD_MEMORY_TYPE                                (MIU0 | SW | WRITE_BACK)

/* AUTOLOAD_GMA   */
//co_buffer L0
#define AUTOLOAD_GMA_AVAILABLE                                 0x0000821000
#define AUTOLOAD_GMA_ADR                                       0x0000821000  //Alignment 0
#define AUTOLOAD_GMA_GAP_CHK                                   0x0000000000
#define AUTOLOAD_GMA_LEN                                       0x0000020000
#define AUTOLOAD_GMA_MEMORY_TYPE                               (MIU0 | SW | WRITE_BACK)

/* MENULOAD_BUFFER   */
//co_buffer L0
#define MENULOAD_BUFFER_AVAILABLE                              0x0000841000
#define MENULOAD_BUFFER_ADR                                    0x0000841000  //Alignment 0
#define MENULOAD_BUFFER_GAP_CHK                                0x0000000000
#define MENULOAD_BUFFER_LEN                                    0x0000004000
#define MENULOAD_BUFFER_MEMORY_TYPE                            (MIU0 | SW | WRITE_BACK)

/* PQ_MAIN   */
//co_buffer L0
#define PQ_MAIN_AVAILABLE                                      0x0000845000
#define PQ_MAIN_ADR                                            0x0000845000  //Alignment 0
#define PQ_MAIN_GAP_CHK                                        0x0000000000
#define PQ_MAIN_LEN                                            0x0000064000
#define PQ_MAIN_MEMORY_TYPE                                    (MIU0 | SW | WRITE_BACK)

/* PQ_MAIN_EX   */
//co_buffer L0
#define PQ_MAIN_EX_AVAILABLE                                   0x00008A9000
#define PQ_MAIN_EX_ADR                                         0x00008A9000  //Alignment 0
#define PQ_MAIN_EX_GAP_CHK                                     0x0000000000
#define PQ_MAIN_EX_LEN                                         0x0000008000
#define PQ_MAIN_EX_MEMORY_TYPE                                 (MIU0 | SW | WRITE_BACK)

/* PQ_TEXT_MAIN   */
//co_buffer L0
#define PQ_TEXT_MAIN_AVAILABLE                                 0x00008B1000
#define PQ_TEXT_MAIN_ADR                                       0x00008B1000  //Alignment 0
#define PQ_TEXT_MAIN_GAP_CHK                                   0x0000000000
#define PQ_TEXT_MAIN_LEN                                       0x0000008000
#define PQ_TEXT_MAIN_MEMORY_TYPE                               (MIU0 | SW | WRITE_BACK)

/* GOP_GWIN_RB   */
//co_buffer L0
#define GOP_GWIN_RB_AVAILABLE                                  0x00008B9000
#define GOP_GWIN_RB_ADR                                        0x00008B9000  //Alignment 0x00008
#define GOP_GWIN_RB_GAP_CHK                                    0x0000000000
#define GOP_GWIN_RB_LEN                                        0x0000300000
#define GOP_GWIN_RB_MEMORY_TYPE                                (MIU0 | SW | WRITE_BACK)

/* GOP_REGDMABASE   */
//co_buffer L0
#define GOP_REGDMABASE_AVAILABLE                               0x0000BB9000
#define GOP_REGDMABASE_ADR                                     0x0000BB9000  //Alignment 0x01000
#define GOP_REGDMABASE_GAP_CHK                                 0x0000000000
#define GOP_REGDMABASE_LEN                                     0x0000001000
#define GOP_REGDMABASE_MEMORY_TYPE                             (MIU0 | SW | WRITE_BACK)

/* POOL_BUFFER   */
//co_buffer L0
#define POOL_BUFFER_AVAILABLE                                  0x0000BBA000
#define POOL_BUFFER_ADR                                        0x0000C00000  //Alignment 0x80000
#define POOL_BUFFER_GAP_CHK                                    0x0000046000
#define POOL_BUFFER_LEN                                        0x0001D00000
#define POOL_BUFFER_MEMORY_TYPE                                (MIU0 | SW | WRITE_BACK)

/* BMP_START   */
//co_buffer L0
#define BMP_START_AVAILABLE                                    0x0002900000
#define BMP_START_ADR                                          0x0002900000  //Alignment 0x00800
#define BMP_START_GAP_CHK                                      0x0000000000
#define BMP_START_LEN                                          0x0000300000
#define BMP_START_MEMORY_TYPE                                  (MIU0 | SW | WRITE_BACK)

/* STRING_START   */
//co_buffer L0
#define STRING_START_AVAILABLE                                 0x0002C00000
#define STRING_START_ADR                                       0x0002C00000  //Alignment 0x00800
#define STRING_START_GAP_CHK                                   0x0000000000
#define STRING_START_LEN                                       0x0000050000
#define STRING_START_MEMORY_TYPE                               (MIU0 | SW | WRITE_BACK)

/* FONT_START   */
//co_buffer L0
#define FONT_START_AVAILABLE                                   0x0002C50000
#define FONT_START_ADR                                         0x0002C50000  //Alignment 0x00008
#define FONT_START_GAP_CHK                                     0x0000000000
#define FONT_START_LEN                                         0x00000E1000
#define FONT_START_MEMORY_TYPE                                 (MIU0 | SW | WRITE_BACK)

/* OD_L   */
//co_buffer L0
#define OD_L_AVAILABLE                                         0x0002D31000
#define OD_L_ADR                                               0x0002D31000  //Alignment 0
#define OD_L_GAP_CHK                                           0x0000000000
#define OD_L_LEN                                               0x00000EA600
#define OD_L_MEMORY_TYPE                                       (MIU0 | SW | WRITE_BACK)

/* OD_R   */
//co_buffer L0
#define OD_R_AVAILABLE                                         0x0002E1B600
#define OD_R_ADR                                               0x0002E1B600  //Alignment 0
#define OD_R_GAP_CHK                                           0x0000000000
#define OD_R_LEN                                               0x00000EA600
#define OD_R_MEMORY_TYPE                                       (MIU0 | SW | WRITE_BACK)

/* FB_L0   */
//co_buffer L0
#define FB_L0_AVAILABLE                                        0x0002F05C00
#define FB_L0_ADR                                              0x0002F06000  //Alignment 0x02000
#define FB_L0_GAP_CHK                                          0x0000000400
#define FB_L0_LEN                                              0x0000753000
#define FB_L0_MEMORY_TYPE                                      (MIU0 | SW | WRITE_BACK)

/* FB_R0   */
//co_buffer L0
#define FB_R0_AVAILABLE                                        0x0003659000
#define FB_R0_ADR                                              0x000365A000  //Alignment 0x02000
#define FB_R0_GAP_CHK                                          0x0000001000
#define FB_R0_LEN                                              0x0000000000
#define FB_R0_MEMORY_TYPE                                      (MIU0 | SW | WRITE_BACK)

/* FB_L1   */
//co_buffer L0
#define FB_L1_AVAILABLE                                        0x000365A000
#define FB_L1_ADR                                              0x000365A000  //Alignment 0x02000
#define FB_L1_GAP_CHK                                          0x0000000000
#define FB_L1_LEN                                              0x0000753000
#define FB_L1_MEMORY_TYPE                                      (MIU0 | SW | WRITE_BACK)

/* FB_R1   */
//co_buffer L0
#define FB_R1_AVAILABLE                                        0x0003DAD000
#define FB_R1_ADR                                              0x0003DAE000  //Alignment 0x02000
#define FB_R1_GAP_CHK                                          0x0000001000
#define FB_R1_LEN                                              0x0000000000
#define FB_R1_MEMORY_TYPE                                      (MIU0 | SW | WRITE_BACK)

//MIU_1_START
//MIU_END
#define MIU0_END_ADR                                           0x0003DAE000
