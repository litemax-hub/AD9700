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

#define SCA_TOOL_VERSION            "SN SCA V1.1.7 "

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
#define ENABLE_MIU_1                1
#define ENABLE_MIU_2                1
#define MIU_DRAM_LEN                0x0030000000
#define MIU_DRAM_LEN0               0x0010000000
#define MIU_DRAM_LEN1               0x0010000000
#define MIU_DRAM_LEN2               0x0010000000
#define MIU_INTERVAL                0x0000000000
#define MIU_INTERVAL2               0x0000000000
#define CPU_ALIGN                   0x0000001000

////////////////////////////////////////////////////////////////////////////////
//MIU SETTING
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//MEMORY TYPE
////////////////////////////////////////////////////////////////////////////////
#define MIU0                        (0x0000)
#define MIU1                        (0x0001)
#define MIU2                        (0x0002)

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
#define BEON_MEM_MEMORY_TYPE                                   (MIU0 | SW | WRITE_BACK)

/* CP_R2_MEM   */
//co_buffer L0
#define CP_R2_MEM_AVAILABLE                                    0x0000800000
#define CP_R2_MEM_ADR                                          0x0000800000  //Alignment 0x10000
#define CP_R2_MEM_GAP_CHK                                      0x0000000000
#define CP_R2_MEM_LEN                                          0x0000200000
#define CP_R2_MEM_MEMORY_TYPE                                  (MIU0 | SW | WRITE_BACK)

/* SECU_R2_MEM   */
//co_buffer L0
#define SECU_R2_MEM_AVAILABLE                                  0x0000A00000
#define SECU_R2_MEM_ADR                                        0x0000A00000  //Alignment 0x10000
#define SECU_R2_MEM_GAP_CHK                                    0x0000000000
#define SECU_R2_MEM_LEN                                        0x0000200000
#define SECU_R2_MEM_MEMORY_TYPE                                (MIU0 | SW | WRITE_BACK)

/* MENULOAD   */
//co_buffer L0
#define MENULOAD_AVAILABLE                                     0x0000C00000
#define MENULOAD_ADR                                           0x0000C00000  //Alignment 0
#define MENULOAD_GAP_CHK                                       0x0000000000
#define MENULOAD_LEN                                           0x0000001000
#define MENULOAD_MEMORY_TYPE                                   (MIU0 | SW | WRITE_BACK)

/* AUTOLOAD_OD   */
//co_buffer L0
#define AUTOLOAD_OD_AVAILABLE                                  0x0000C01000
#define AUTOLOAD_OD_ADR                                        0x0000C01000  //Alignment 0
#define AUTOLOAD_OD_GAP_CHK                                    0x0000000000
#define AUTOLOAD_OD_LEN                                        0x0000020000
#define AUTOLOAD_OD_MEMORY_TYPE                                (MIU0 | SW | WRITE_BACK)

/* AUTOLOAD_GMA   */
//co_buffer L0
#define AUTOLOAD_GMA_AVAILABLE                                 0x0000C21000
#define AUTOLOAD_GMA_ADR                                       0x0000C21000  //Alignment 0
#define AUTOLOAD_GMA_GAP_CHK                                   0x0000000000
#define AUTOLOAD_GMA_LEN                                       0x0000004000
#define AUTOLOAD_GMA_MEMORY_TYPE                               (MIU0 | SW | WRITE_BACK)

/* AUTOLOAD_KST   */
//co_buffer L0
#define AUTOLOAD_KST_AVAILABLE                                 0x0000C25000
#define AUTOLOAD_KST_ADR                                       0x0000C25000  //Alignment 0
#define AUTOLOAD_KST_GAP_CHK                                   0x0000000000
#define AUTOLOAD_KST_LEN                                       0x0000080000
#define AUTOLOAD_KST_MEMORY_TYPE                               (MIU0 | SW | WRITE_BACK)

/* AUTOLOAD_HDR   */
//co_buffer L0
#define AUTOLOAD_HDR_AVAILABLE                                 0x0000CA5000
#define AUTOLOAD_HDR_ADR                                       0x0000CA5000  //Alignment 0
#define AUTOLOAD_HDR_GAP_CHK                                   0x0000000000
#define AUTOLOAD_HDR_LEN                                       0x0000640000
#define AUTOLOAD_HDR_MEMORY_TYPE                               (MIU0 | SW | WRITE_BACK)

/* AUTOLOAD_DEMURA   */
//co_buffer L0
#define AUTOLOAD_DEMURA_AVAILABLE                              0x00012E5000
#define AUTOLOAD_DEMURA_ADR                                    0x00012E5000  //Alignment 0
#define AUTOLOAD_DEMURA_GAP_CHK                                0x0000000000
#define AUTOLOAD_DEMURA_LEN                                    0x0000080000
#define AUTOLOAD_DEMURA_MEMORY_TYPE                            (MIU0 | SW | WRITE_BACK)

/* MENULOAD_BUFFER   */
//co_buffer L0
#define MENULOAD_BUFFER_AVAILABLE                              0x0001365000
#define MENULOAD_BUFFER_ADR                                    0x0001365000  //Alignment 0
#define MENULOAD_BUFFER_GAP_CHK                                0x0000000000
#define MENULOAD_BUFFER_LEN                                    0x0000010000
#define MENULOAD_BUFFER_MEMORY_TYPE                            (MIU0 | SW | WRITE_BACK)

/* DOWNLOAD_BUFFER   */
//co_buffer L0
#define DOWNLOAD_BUFFER_AVAILABLE                              0x0001375000
#define DOWNLOAD_BUFFER_ADR                                    0x0001375000  //Alignment 0
#define DOWNLOAD_BUFFER_GAP_CHK                                0x0000000000
#define DOWNLOAD_BUFFER_LEN                                    0x0000400000
#define DOWNLOAD_BUFFER_MEMORY_TYPE                            (MIU0 | SW | WRITE_BACK)

/* BDMA_BUFFER   */
//co_buffer L0
#define BDMA_BUFFER_AVAILABLE                                  0x0001775000
#define BDMA_BUFFER_ADR                                        0x0001775000  //Alignment 0
#define BDMA_BUFFER_GAP_CHK                                    0x0000000000
#define BDMA_BUFFER_LEN                                        0x0000100000
#define BDMA_BUFFER_MEMORY_TYPE                                (MIU0 | SW | WRITE_BACK)

/* FILE_SYSTEM_POOL   */
//co_buffer L0
#define FILE_SYSTEM_POOL_AVAILABLE                             0x0001875000
#define FILE_SYSTEM_POOL_ADR                                   0x0001875000  //Alignment 0x01000
#define FILE_SYSTEM_POOL_GAP_CHK                               0x0000000000
#define FILE_SYSTEM_POOL_LEN                                   0x0000060000
#define FILE_SYSTEM_POOL_MEMORY_TYPE                           (MIU0 | SW | WRITE_BACK)

/* MBX_XD2MIU   */
//co_buffer L0
#define MBX_XD2MIU_AVAILABLE                                   0x00018D5000
#define MBX_XD2MIU_ADR                                         0x00018E0000  //Alignment 0x10000
#define MBX_XD2MIU_GAP_CHK                                     0x000000B000
#define MBX_XD2MIU_LEN                                         0x0000010000
#define MBX_XD2MIU_MEMORY_TYPE                                 (MIU0 | SW | WRITE_BACK)

/* AudioPCM   */
//co_buffer L0
#define AudioPCM_AVAILABLE                                     0x00018F0000
#define AudioPCM_ADR                                           0x00018F0000  //Alignment 0
#define AudioPCM_GAP_CHK                                       0x0000000000
#define AudioPCM_LEN                                           0x0000800000
#define AudioPCM_MEMORY_TYPE                                   (MIU0 | SW | WRITE_BACK)

/* PQ_MAIN   */
//co_buffer L0
#define PQ_MAIN_AVAILABLE                                      0x00020F0000
#define PQ_MAIN_ADR                                            0x00020F0000  //Alignment 0
#define PQ_MAIN_GAP_CHK                                        0x0000000000
#define PQ_MAIN_LEN                                            0x0000064000
#define PQ_MAIN_MEMORY_TYPE                                    (MIU0 | SW | WRITE_BACK)

/* PQ_MAIN_EX   */
//co_buffer L0
#define PQ_MAIN_EX_AVAILABLE                                   0x0002154000
#define PQ_MAIN_EX_ADR                                         0x0002154000  //Alignment 0
#define PQ_MAIN_EX_GAP_CHK                                     0x0000000000
#define PQ_MAIN_EX_LEN                                         0x0000008000
#define PQ_MAIN_EX_MEMORY_TYPE                                 (MIU0 | SW | WRITE_BACK)

/* PQ_TEXT_MAIN   */
//co_buffer L0
#define PQ_TEXT_MAIN_AVAILABLE                                 0x000215C000
#define PQ_TEXT_MAIN_ADR                                       0x000215C000  //Alignment 0
#define PQ_TEXT_MAIN_GAP_CHK                                   0x0000000000
#define PQ_TEXT_MAIN_LEN                                       0x0000008000
#define PQ_TEXT_MAIN_MEMORY_TYPE                               (MIU0 | SW | WRITE_BACK)

/* MS_TOOL   */
//co_buffer L0
#define MS_TOOL_AVAILABLE                                      0x0002164000
#define MS_TOOL_ADR                                            0x0002164000  //Alignment 0x01000
#define MS_TOOL_GAP_CHK                                        0x0000000000
#define MS_TOOL_LEN                                            0x0000400000
#define MS_TOOL_MEMORY_TYPE                                    (MIU0 | SW | WRITE_BACK)

/* GOP_GWIN_RB   */
//co_buffer L0
#define GOP_GWIN_RB_AVAILABLE                                  0x0002564000
#define GOP_GWIN_RB_ADR                                        0x0002564000  //Alignment 0x01000
#define GOP_GWIN_RB_GAP_CHK                                    0x0000000000
#define GOP_GWIN_RB_LEN                                        0x0003600000
#define GOP_GWIN_RB_MEMORY_TYPE                                (MIU0 | SW | WRITE_BACK)

/* GOP_REGDMABASE   */
//co_buffer L0
#define GOP_REGDMABASE_AVAILABLE                               0x0005B64000
#define GOP_REGDMABASE_ADR                                     0x0005B64000  //Alignment 0x04000
#define GOP_REGDMABASE_GAP_CHK                                 0x0000000000
#define GOP_REGDMABASE_LEN                                     0x0000001000
#define GOP_REGDMABASE_MEMORY_TYPE                             (MIU0 | SW | WRITE_BACK)

/* POOL_BUFFER   */
//co_buffer L0
#define POOL_BUFFER_AVAILABLE                                  0x0005B65000
#define POOL_BUFFER_ADR                                        0x0005B80000  //Alignment 0x80000
#define POOL_BUFFER_GAP_CHK                                    0x000001B000
#define POOL_BUFFER_LEN                                        0x0001D00000
#define POOL_BUFFER_MEMORY_TYPE                                (MIU0 | SW | WRITE_BACK)

/* BMP_START   */
//co_buffer L0
#define BMP_START_AVAILABLE                                    0x0007880000
#define BMP_START_ADR                                          0x0007880000  //Alignment 0x01000
#define BMP_START_GAP_CHK                                      0x0000000000
#define BMP_START_LEN                                          0x0000A00000
#define BMP_START_MEMORY_TYPE                                  (MIU0 | SW | WRITE_BACK)

/* BMP_PALETTE_BUF_START   */
//co_buffer L0
#define BMP_PALETTE_BUF_START_AVAILABLE                        0x0008280000
#define BMP_PALETTE_BUF_START_ADR                              0x0008280000  //Alignment 0
#define BMP_PALETTE_BUF_START_GAP_CHK                          0x0000000000
#define BMP_PALETTE_BUF_START_LEN                              0x0000001000
#define BMP_PALETTE_BUF_START_MEMORY_TYPE                      (MIU0 | SW | WRITE_BACK)

/* STRING_START   */
//co_buffer L0
#define STRING_START_AVAILABLE                                 0x0008281000
#define STRING_START_ADR                                       0x0008281000  //Alignment 0x01000
#define STRING_START_GAP_CHK                                   0x0000000000
#define STRING_START_LEN                                       0x0000050000
#define STRING_START_MEMORY_TYPE                               (MIU0 | SW | WRITE_BACK)

/* FONT_START   */
//co_buffer L0
#define FONT_START_AVAILABLE                                   0x00082D1000
#define FONT_START_ADR                                         0x00082D1000  //Alignment 0x01000
#define FONT_START_GAP_CHK                                     0x0000000000
#define FONT_START_LEN                                         0x00000E1000
#define FONT_START_MEMORY_TYPE                                 (MIU0 | SW | WRITE_BACK)

/* OD_L   */
//co_buffer L0
#define OD_L_AVAILABLE                                         0x00083B2000
#define OD_L_ADR                                               0x00083B2000  //Alignment 0
#define OD_L_GAP_CHK                                           0x0000000000
#define OD_L_LEN                                               0x0000E00000
#define OD_L_MEMORY_TYPE                                       (MIU0 | SW | WRITE_BACK)

/* OD_R   */
//co_buffer L0
#define OD_R_AVAILABLE                                         0x00091B2000
#define OD_R_ADR                                               0x00091B2000  //Alignment 0
#define OD_R_GAP_CHK                                           0x0000000000
#define OD_R_LEN                                               0x0000E00000
#define OD_R_MEMORY_TYPE                                       (MIU0 | SW | WRITE_BACK)

/* OSDC_L   */
//co_buffer L0
#define OSDC_L_AVAILABLE                                       0x0009FB2000
#define OSDC_L_ADR                                             0x0009FB2000  //Alignment 0
#define OSDC_L_GAP_CHK                                         0x0000000000
#define OSDC_L_LEN                                             0x0000000000
#define OSDC_L_MEMORY_TYPE                                     (MIU0 | SW | WRITE_BACK)

/* OSDC_R   */
//co_buffer L0
#define OSDC_R_AVAILABLE                                       0x0009FB2000
#define OSDC_R_ADR                                             0x0009FB2000  //Alignment 0
#define OSDC_R_GAP_CHK                                         0x0000000000
#define OSDC_R_LEN                                             0x0000000000
#define OSDC_R_MEMORY_TYPE                                     (MIU0 | SW | WRITE_BACK)

/* LOCAL_DIMMING_L   */
//co_buffer L0
#define LOCAL_DIMMING_L_AVAILABLE                              0x0009FB2000
#define LOCAL_DIMMING_L_ADR                                    0x0009FB2000  //Alignment 0
#define LOCAL_DIMMING_L_GAP_CHK                                0x0000000000
#define LOCAL_DIMMING_L_LEN                                    0x0000400000
#define LOCAL_DIMMING_L_MEMORY_TYPE                            (MIU0 | SW | WRITE_BACK)

/* LOCAL_DIMMING_R   */
//co_buffer L0
#define LOCAL_DIMMING_R_AVAILABLE                              0x000A3B2000
#define LOCAL_DIMMING_R_ADR                                    0x000A3B2000  //Alignment 0
#define LOCAL_DIMMING_R_GAP_CHK                                0x0000000000
#define LOCAL_DIMMING_R_LEN                                    0x0000400000
#define LOCAL_DIMMING_R_MEMORY_TYPE                            (MIU0 | SW | WRITE_BACK)

//MIU_1_START
/* Reserved1   */
//co_buffer L0
#define Reserved1_AVAILABLE                                    0x0000000000
#define Reserved1_ADR                                          0x0000000000  //Alignment 0
#define Reserved1_GAP_CHK                                      0x0000000000
#define Reserved1_LEN                                          0x0000200000
#define Reserved1_MEMORY_TYPE                                  (MIU1 | SW | WRITE_BACK)

/* HDR_BUFFER0   */
//co_buffer L0
#define HDR_BUFFER0_AVAILABLE                                  0x0000200000
#define HDR_BUFFER0_ADR                                        0x0000200000  //Alignment 0
#define HDR_BUFFER0_GAP_CHK                                    0x0000000000
#define HDR_BUFFER0_LEN                                        0x0001000000
#define HDR_BUFFER0_MEMORY_TYPE                                (MIU1 | SW | WRITE_BACK)

/* HDR_BUFFER1   */
//co_buffer L0
#define HDR_BUFFER1_AVAILABLE                                  0x0001200000
#define HDR_BUFFER1_ADR                                        0x0001200000  //Alignment 0
#define HDR_BUFFER1_GAP_CHK                                    0x0000000000
#define HDR_BUFFER1_LEN                                        0x0001000000
#define HDR_BUFFER1_MEMORY_TYPE                                (MIU1 | SW | WRITE_BACK)

/* FB_MIU1   */
//co_buffer L0
#define FB_MIU1_AVAILABLE                                      0x0002200000
#define FB_MIU1_ADR                                            0x0002200000  //Alignment 0x02000
#define FB_MIU1_GAP_CHK                                        0x0000000000
#define FB_MIU1_LEN                                            0x000D200000
#define FB_MIU1_MEMORY_TYPE                                    (MIU1 | SW | WRITE_BACK)

//MIU_END
//MIU_2_START
/* Reserved2   */
//co_buffer L0
#define Reserved2_AVAILABLE                                    0x0000000000
#define Reserved2_ADR                                          0x0000000000  //Alignment 0
#define Reserved2_GAP_CHK                                      0x0000000000
#define Reserved2_LEN                                          0x0000200000
#define Reserved2_MEMORY_TYPE                                  (MIU2 | SW | WRITE_BACK)

/* HDR_BUFFER2   */
//co_buffer L0
#define HDR_BUFFER2_AVAILABLE                                  0x0000200000
#define HDR_BUFFER2_ADR                                        0x0000200000  //Alignment 0
#define HDR_BUFFER2_GAP_CHK                                    0x0000000000
#define HDR_BUFFER2_LEN                                        0x0001000000
#define HDR_BUFFER2_MEMORY_TYPE                                (MIU2 | SW | WRITE_BACK)

/* HDR_BUFFER3   */
//co_buffer L0
#define HDR_BUFFER3_AVAILABLE                                  0x0001200000
#define HDR_BUFFER3_ADR                                        0x0001200000  //Alignment 0
#define HDR_BUFFER3_GAP_CHK                                    0x0000000000
#define HDR_BUFFER3_LEN                                        0x0001000000
#define HDR_BUFFER3_MEMORY_TYPE                                (MIU2 | SW | WRITE_BACK)

/* FB_MIU2   */
//co_buffer L0
#define FB_MIU2_AVAILABLE                                      0x0002200000
#define FB_MIU2_ADR                                            0x0002200000  //Alignment 0x02000
#define FB_MIU2_GAP_CHK                                        0x0000000000
#define FB_MIU2_LEN                                            0x000D200000
#define FB_MIU2_MEMORY_TYPE                                    (MIU2 | SW | WRITE_BACK)

//MIU_2_END

#define MIU0_END_ADR                                           0x000A7B2000
#define MIU1_END_ADR                                           0x000F400000
#define MIU2_END_ADR                                           0x000F400000
#define MMAP_COUNT                                             0x0000000029

#define TEST_4K_ALIGN                   1

/* CHK_VALUE = 4200963599 */





