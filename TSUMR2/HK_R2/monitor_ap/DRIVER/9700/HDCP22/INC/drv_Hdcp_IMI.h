#ifndef DRV_HDCP_IMI_H
#define DRV_HDCP_IMI_H
#include "HdcpIMI.h"//[MT9700]

//----------------------------------------------------------------------//
// defines
//----------------------------------------------------------------------//
#define DEF_IMI_HDCP_BASE_ADDR           0xD0000000//0x90000000//0x80000000//[MT9700]0x90000000
//#define DEF_IMI_LENGTH              0x1800  // 6KB//[MT9700]0x2C00 // 11KB
//#define DEF_IMI_HDCP_BUFF_START_ADDR     0xD0001A00//0x90001A00//0x80001A00//[MT9700]0x90001A00
//#define DEF_IMI_HDCP_BUFF_SIZE           0x00000800
//#define DEF_SECU_IMI_BASE_ADDR      0x80000000//0x90000000//[MT9700]0x80000000

#define DEF_IMI_RECVBUFF_ADDR           	0x00
#define DEF_IMI_TRANSBUFF_ADDR        		0xCC0
//#define DEF_IMI_ENAUTH_ADDR              	0x1140
//#define DEF_IMI_DP_GET_R0_ADDR            0x1144
#define DEF_IMI_SECU_POWERDOWN_ADDR       0x17FE
#define DEF_IMI_HDMI_DP_ADDR              0x17FF//0x1991
#define DEF_IMI_CERT_OFFSETADDR       		0x11D0//0x1200-->-0x30, maxsize =0x1800, and 0x1200+1044(COMBO_HDCP2_ENCODE_KEY_SIZE)+522(COMBO_HDCP2_AKE_CERTRX_SIZE)=0x181e>0x1800
#define DEF_IMI_HDCP14TxKey_OFFSETADDR      0x1860


//----------------------------------------------------------------------//
// macros
//----------------------------------------------------------------------//
#define IMIGetAddr(addr)            (0xD0000000|(addr))//(0x90000000|(addr))

//----------------------------------------------------------------------//
// structs
//----------------------------------------------------------------------//

typedef struct _ST_IMI_BUF_HANDLER
{
    BOOL    bInit;
    ULONG   u32TotalBlkCnt;
    WORD    u16CurBlkIndex;
} ST_IMI_BUF_HANDLER;

//----------------------------------------------------------------------//
// proto-type
//----------------------------------------------------------------------//
//ULONG IMI_AllocateBuff(ULONG u32BufSize);
WORD IMI_FreeBuff(ULONG u32BufSize);
void IMI_WriteBytes(ULONG u32Addr, BYTE *pu8Data, ULONG u32DataLen);
//void IMI_ReadBytes(ULONG u32Addr, BYTE *pu8Data, ULONG u32DataLen);
BYTE msIMIReadByte(U32 u32MemAddr );
void msIMIWriteByte(U32 u32MemAddr, U8 u8Data);

#endif
