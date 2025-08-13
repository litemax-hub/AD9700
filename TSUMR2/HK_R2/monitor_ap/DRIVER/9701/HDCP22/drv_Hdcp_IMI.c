#ifndef DRV_IMI_C
#define DRV_IMI_C

//----------------------------------------------------------------------//
// includes
//----------------------------------------------------------------------//
#include "datatype.h"
//#include "drvIMI.h"
#include "drv_Hdcp_IMI.h"
#include "Utl.h"
//----------------------------------------------------------------------//
// global
//----------------------------------------------------------------------//
ST_IMI_BUF_HANDLER g_stIMIBufHandler;

//----------------------------------------------------------------------//
// functions
//----------------------------------------------------------------------//
#if 0
ULONG IMI_AllocateBuff(ULONG u32BufSize)
{
    WORD u16BlkNum = 0;
    ULONG u32RetAddr = 0;

    do
    {
        if (g_stIMIBufHandler.bInit == FALSE)
        {
            g_stIMIBufHandler.u32TotalBlkCnt = DEF_IMI_HDCP_BUFF_SIZE >> 4;
            g_stIMIBufHandler.u16CurBlkIndex = 0;
            //memset((void*)DEF_IMI_BUFF_START_ADDR, 0x00, DEF_IMI_HDCP_BUFF_START_ADDR);
            memset((char*)DEF_IMI_BUFF_START_ADDR, 0x00, DEF_IMI_HDCP_BUFF_START_ADDR);

            g_stIMIBufHandler.bInit = TRUE;
        }

        if (u32BufSize >= 16)
        {
            u16BlkNum = ((u32BufSize % 16) == 0) ? (u32BufSize >> 4) : ((u32BufSize >> 4) + 1);
        }
        else
        {
            u16BlkNum = 1;
        }

        if ((g_stIMIBufHandler.u16CurBlkIndex + u16BlkNum) > g_stIMIBufHandler.u32TotalBlkCnt)
        {
            printf("Allocate IMI buffer FAILED!!!!! NOT enough space!!!!\r\n");
            break;
        }
        u32RetAddr = (g_stIMIBufHandler.u16CurBlkIndex << 4) + DEF_IMI_HDCP_BUFF_START_ADDR;
        g_stIMIBufHandler.u16CurBlkIndex += u16BlkNum;


    } while(FALSE);

    return u32RetAddr;
}
#endif
WORD IMI_FreeBuff(ULONG u32BufSize)
{
    WORD u16BlkNum = 0;

    do
    {
        if (u32BufSize >= 16)
        {
            u16BlkNum = ((u32BufSize % 16) == 0) ? (u32BufSize >> 4) : ((u32BufSize >> 4) + 1);
        }
        else
        {
            u16BlkNum = 1;
        }

        if (g_stIMIBufHandler.u16CurBlkIndex == 0)
        {
            printf("Free IMI buffer FAILED!!!! No Used Buffer Currently\r\n");
            break;
        }

        if (g_stIMIBufHandler.u16CurBlkIndex < u16BlkNum)
        {
            g_stIMIBufHandler.u16CurBlkIndex = 0;
            printf("Free IMI buffer FAILED!!!! NOT Enough Used Buffer Currently\r\n");
            break;
        }

        g_stIMIBufHandler.u16CurBlkIndex -= u16BlkNum;

    } while(FALSE);

    return g_stIMIBufHandler.u16CurBlkIndex;

}
#if 0//[MT9701]
void IMI_WriteBytes(ULONG u32Addr, BYTE *pu8Data, ULONG u32DataLen)
{
    ULONG u32DataCnt = 0;

    for ( u32DataCnt = 0; u32DataCnt < u32DataLen; u32DataCnt++ )
    {
        msIMIWriteByte(u32Addr + u32DataCnt, *(pu8Data + u32DataCnt));
    }
}
void IMI_ReadBytes(ULONG u32Addr, BYTE *pu8Data, ULONG u32DataLen)
{
    ULONG u32DataCnt = 0;

    for ( u32DataCnt = 0; u32DataCnt < u32DataLen; u32DataCnt++ )
    {
        *(pu8Data + u32DataCnt) = msIMIReadByte(u32Addr + u32DataCnt);
    }
}
#endif
BYTE msIMIReadByte(U32 u32MemAddr )
{
    U8 *pu8;
    pu8 = (U8*)(u32MemAddr | (DEF_IMI_HDCP_BASE_ADDR));
    return *pu8;
}
void msIMIWriteByte(U32 u32MemAddr, U8 u8Data )
{
    U8 *pu8;
    pu8 = (U8*)(u32MemAddr| (DEF_IMI_HDCP_BASE_ADDR));
    *pu8 = u8Data;
}
#endif
