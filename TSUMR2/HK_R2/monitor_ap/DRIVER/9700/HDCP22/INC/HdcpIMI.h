#ifndef DRV_HDCPIMI_H
#define DRV_HDCPIMI_H

#include "HdcpCommon.h"
#include "drvIMI.h"

//----------------------------------------------------------------------//
// defines
//----------------------------------------------------------------------//
#define DEF_HK51_IMI_BANK0_SETTING           0x0000  //0xF000~FFFF
#define DEF_HK51_IMI_BANK1_SETTING           0x1000 // 0x10000~10FFF

#define DEF_HK_IMI_BASE_ADDR           0xF000
#define DEF_IMI_LENGTH              0x2000 // 8KB
#define DEF_IMI_BUFF_START_OffsetADDR     0x1000
#define DEF_IMI_BUFF_START_ADDR     DEF_HK_IMI_BASE_ADDR+DEF_IMI_BUFF_START_OffsetADDR
#define DEF_IMI_BUFF_SIZE           0x00001000
#define DEF_SECU_IMI_BASE_ADDR           0xD0000000//0x80000000

//[MT9700]#define DEF_IMI_RECVBUFF_ADDR           0x0
//[MT9700]#define DEF_IMI_TRANSBUFF_ADDR        0x440
//[MT9700]#define DEF_IMI_ENAUTH_ADDR              0x6F0
//[MT9700]#define DEF_IMI_SECU_POWERDOWN_ADDR              0x6FF
//[MT9700]#define DEF_IMI_CERT_OFFSETADDR       0x700
//[MT9700]#define DEF_IMI_HDCP14TxKey_OFFSETADDR       0xD60

#define DEF_SECU_IMI_HDCP14TxKeyADDR       DEF_SECU_IMI_BASE_ADDR+DEF_IMI_HDCP14TxKey_OFFSETADDR
#define DEF_SECU_IMI_CERTADDR                     DEF_SECU_IMI_BASE_ADDR+DEF_IMI_CERT_OFFSETADDR


//----------------------------------------------------------------------//
// macros
//----------------------------------------------------------------------//

//----------------------------------------------------------------------//
// structs
//----------------------------------------------------------------------//
#if 0//[MT9700]
typedef struct _ST_IMI_BUF_HANDLER
{
    BOOL    bInit:1;
    DWORD   u32TotalBlkCnt;
    WORD    u16CurBlkIndex;
} ST_IMI_BUF_HANDLER;
#endif
//----------------------------------------------------------------------//
// proto-type
//----------------------------------------------------------------------//


#endif
