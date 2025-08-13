#ifndef DRV_IMI_C
#define DRV_IMI_C

//----------------------------------------------------------------------//
// includes
//----------------------------------------------------------------------//
#include "Global.h"
#include "types.h"
#include "drvIMI.h"
//#include "Utl.h"


#define msIMIRegs        ((unsigned char volatile xdata *) DEF_IMI_BASE_ADDR)
//----------------------------------------------------------------------//
// global
//----------------------------------------------------------------------//

//----------------------------------------------------------------------//
// functions
//----------------------------------------------------------------------//
void IMI_Init(DWORD u32BaseAddr, DWORD u32Size)
{
    DWORD  i;
    //BYTE u8ImiRead = 0xFF;
    BYTE u8ImiWrite = 0x00;

    for(i = 0;i < u32Size; i++)
    {

        IMI_WriteBytes((u32BaseAddr + i), &u8ImiWrite, 1);
#if 0
        IMI_ReadBytes((u32BaseAddr + i),&u8ImiRead,1);

        if(u8ImiRead != 0x00)
        {
            printData("IMI init fail at %d", (u32BaseAddr + i));
            printData(" is %d", u8ImiRead);
            break;
        }
#endif
    }
}

void IMI_WriteByte(DWORD u32Offset, BYTE u8Data)
{
    msIMIRegs[u32Offset] = u8Data;
}

void IMI_Write2Bytes(DWORD u32Offset, WORD u16Data)
{
    msIMIRegs[u32Offset] = (BYTE)(u16Data & 0xFF);
	msIMIRegs[u32Offset+1] = (BYTE)(u16Data >> 8);
}


BYTE IMI_ReadByte(DWORD u32Offset)
{
    return msIMIRegs[u32Offset];
}

void IMI_WriteBytes(DWORD u32Addr, BYTE *pu8Data, DWORD u32DataLen)
{
    DWORD u32DataCnt = 0;

    for ( u32DataCnt = 0; u32DataCnt < u32DataLen; u32DataCnt++ )
    {
        msIMIRegs[u32Addr + u32DataCnt] = *(BYTE*)(pu8Data+u32DataCnt);
    }
}

void IMI_ReadBytes(DWORD u32Addr, BYTE *pu8Data, DWORD u32DataLen)
{
    DWORD u32DataCnt = 0;

    for ( u32DataCnt = 0; u32DataCnt < u32DataLen; u32DataCnt++ )
    {
        *(pu8Data + u32DataCnt) = msIMIRegs[u32Addr + u32DataCnt];
    }
}

BYTE* IMI_GetIMIRegs(DWORD u32Addr)
{
    return (BYTE*)&msIMIRegs[u32Addr];
}

#if DEF_HDCP2RX_ISR_MODE
void MEM_MSWRITE_2BYTE( DWORD u32RegLoAddr, WORD u16Value )
{
    MEM_MSWRITE_BYTE( u32RegLoAddr, ( BYTE )u16Value );
    MEM_MSWRITE_BYTE( u32RegLoAddr + 1, ( BYTE )( u16Value >> 8 ) );
}
void MEM_EXT0MSWRITE_2BYTE( DWORD u32RegLoAddr, WORD u16Value )
{
    MEM_MSWRITE_BYTE( u32RegLoAddr, ( BYTE )u16Value );
    MEM_MSWRITE_BYTE( u32RegLoAddr + 1, ( BYTE )( u16Value >> 8 ) );
}

#if 0
void mcuISR_4kXdataMapToDRAMIMI(DWORD dwADDR, BYTE ucDestSel)
{
    return;
    #if 0
    MEM_MSWRITE_BYTE(REG_103C1C, (ucDestSel ? 0x25 : 0x24));
    MEM_MSWRITE_2BYTE(REG_002BCC, dwADDR>>12);
    MEM_MSWRITE_2BYTE(REG_002BCE, dwADDR>>28);
    #endif
}

void mcuISREXT0_4kXdataMapToDRAMIMI(DWORD dwADDR, BYTE ucDestSel)
{
    return;
    #if 0
    MEM_MSWRITE_BYTE(REG_103C1C, (ucDestSel ? 0x25 : 0x24));
    MEM_EXT0MSWRITE_2BYTE(REG_002BCC, dwADDR>>12);
    MEM_EXT0MSWRITE_2BYTE(REG_002BCE, dwADDR>>28);
    #endif
}
#endif

void IMI_ISR_WriteByte(DWORD u32Offset, BYTE u8Data)
{
    msIMIRegs[u32Offset] = u8Data;
}

void IMI_ISREXT0_WriteByte(DWORD u32Offset, BYTE u8Data)
{
    msIMIRegs[u32Offset] = u8Data;
}

BYTE IMI_ISR_ReadByte(DWORD u32Offset)
{
    return msIMIRegs[u32Offset];
}

BYTE IMI_ISREXT0_ReadByte(DWORD u32Offset)
{
    return msIMIRegs[u32Offset];
}

void IMI_ISR_WriteBytes(DWORD u32Addr, BYTE *pu8Data, DWORD u32DataLen)
{
    DWORD u32DataCnt = 0;

    for ( u32DataCnt = 0; u32DataCnt < u32DataLen; u32DataCnt++ )
    {
        msIMIRegs[u32Addr + u32DataCnt] = *(BYTE*)(pu8Data+u32DataCnt);
    }
}

void IMI_ISR_ReadBytes(DWORD u32Addr, BYTE *pu8Data, DWORD u32DataLen)
{
    DWORD u32DataCnt = 0;

    for ( u32DataCnt = 0; u32DataCnt < u32DataLen; u32DataCnt++ )
    {
        *(pu8Data + u32DataCnt) = msIMIRegs[u32Addr + u32DataCnt];
    }
}

void IMI_ISREXT0_ReadBytes(DWORD u32Addr, BYTE *pu8Data, DWORD u32DataLen)
{
    DWORD u32DataCnt = 0;

    for ( u32DataCnt = 0; u32DataCnt < u32DataLen; u32DataCnt++ )
    {
        *(pu8Data + u32DataCnt) = msIMIRegs[u32Addr + u32DataCnt];
    }
}
#endif

#endif

