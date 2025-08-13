///////////////////////////////////////////////////////////////////////////////
/// @file ms_rwreg.h
/// @brief MStar Scaler register access.
/// @author MStarSemi Inc.
///
/// Functions for MStar scaler register access.
///
/// Features
///  -Write byte/word/bulk data
///  -Read byte/word data.
///  -
///  -
///////////////////////////////////////////////////////////////////////////////
#define _MS_RWREG_C

#include "datatype.h"
#include "board.h"
#include "Global.h"
#include "ms_reg.h"
#include "misc.h"
#include "Ms_rwreg.h"

#define MSRWREG_DEBUG    1
#if ENABLE_MSTV_UART_DEBUG && MSRWREG_DEBUG
#define MSRWREG_PRINT(format, ...)     printf(format, ##__VA_ARGS__)
#else
#define MSRWREG_PRINT(format, ...)
#endif

U8 msMemReadByte(U32 u32MemAddr )
{
    U8 *pu8;
    
    pu8 = (U8*)(u32MemAddr | (0x80000000)); // non-cached
    return *pu8;
}

void msMemWriteByte(U32 u32MemAddr, U8 u8Data )
{
    U8 *pu8;
    pu8 = (U8*)(u32MemAddr| (0x80000000)); // non-cached
    *pu8 = u8Data;
}

void msMemWriteByteMask( U32 u32Reg, U8 u8Value, U8 u8Mask )
{
    msMemWriteByte( u32Reg, ( (msMemReadByte( u32Reg ) & ( ~u8Mask )) | ( u8Value & u8Mask ) ) );
}

U16 msMemRead2Byte(U32 u32MemAddr )
{
    return ((( U16 )msMemReadByte( u32MemAddr+1 ) ) << 8 ) | msMemReadByte( u32MemAddr );
}

void msMemWrite2Byte(U32 u32MemAddr, U16 u16Data )
{
    msMemWriteByte( u32MemAddr, ( U8 )u16Data );
    msMemWriteByte( u32MemAddr + 1, ( U8 )( u16Data >> 8 ) );
}

void msMemWrite2ByteMask( U32 u32RegLoAddr, U16 u16Value , U16 u16Mask )
{
    msMemWriteByteMask( u32RegLoAddr, ( U8 )u16Value, ( U8 ) u16Mask );
    msMemWriteByteMask( u32RegLoAddr + 1, ( U8 )( u16Value >> 8 ), ( U8 )( u16Mask >> 8 ) );
}

U32 msMemRead3Byte(U32 u32MemAddr )
{
    return ((( U32 )msMemReadByte( u32MemAddr+2 ) ) << 16 ) | ((( U32 )msMemReadByte( u32MemAddr+1 ) ) << 8 ) | msMemReadByte( u32MemAddr );
}

void msMemWrite3Byte( U32 u32RegLoAddr, U32 u32Value )
{
    msMemWriteByte( u32RegLoAddr, ( U8 )u32Value );
    msMemWriteByte( u32RegLoAddr + 1, ( U8 )( u32Value >> 8 ) );
    msMemWriteByte( u32RegLoAddr + 2, ( U8 )( u32Value >> 16 ) );
}

void msMemWrite3ByteMask( U32 u32RegLoAddr, U32 u32Value , U32 u32Mask )
{
    msMemWriteByteMask( u32RegLoAddr, ( U8 )u32Value, ( U8 ) u32Mask );
    msMemWriteByteMask( u32RegLoAddr + 1, ( U8 )( u32Value >> 8 ), ( U8 )( u32Mask >> 8 ) );
    msMemWriteByteMask( u32RegLoAddr + 2, ( U8 )( u32Value >> 16 ), ( U8 )( u32Mask >> 16 ) );
}

U32 msMemRead4Byte(U32 u32MemAddr )
{
    return ((( U32 )msMemReadByte( u32MemAddr+3 ) ) << 24 ) | ((( U32 )msMemReadByte( u32MemAddr+2 ) ) << 16 ) | ((( U32 )msMemReadByte( u32MemAddr+1 ) ) << 8 ) | msMemReadByte( u32MemAddr );
}

void msMemWrite4Byte( U32 u32RegLoAddr, U32 u32Value )
{
    msMemWriteByte( u32RegLoAddr, ( U8 )u32Value );
    msMemWriteByte( u32RegLoAddr + 1, ( U8 )( u32Value >> 8 ) );
    msMemWriteByte( u32RegLoAddr + 2, ( U8 )( u32Value >> 16 ) );
    msMemWriteByte( u32RegLoAddr + 3, ( U8 )( u32Value >> 24 ) );
}

void msMemWrite4ByteMask( U32 u32RegLoAddr, U32 u32Value , U32 u32Mask )
{
    msMemWriteByteMask( u32RegLoAddr, ( U8 )u32Value, ( U8 ) u32Mask );
    msMemWriteByteMask( u32RegLoAddr + 1, ( U8 )( u32Value >> 8 ), ( U8 )( u32Mask >> 8 ) );
    msMemWriteByteMask( u32RegLoAddr + 2, ( U8 )( u32Value >> 16 ), ( U8 )( u32Mask >> 16 ) );
    msMemWriteByteMask( u32RegLoAddr + 3, ( U8 )( u32Value >> 24 ), ( U8 )( u32Mask >> 24 ) );
}

BYTE scReadByte( DWORD u32Reg )
{
    return msRegs(u32Reg);
}

void scWriteByte( DWORD u32Reg, BYTE u8Val )
{
    msRegs(u32Reg) = u8Val;
}

BYTE msReadByte( DWORD u32Reg )
{
    if( u32Reg & 0x00800000 ) // Sub bank
    {
        msRegs((u32Reg & 0x007FFF00)) = ( u32Reg >> 24 ) & 0xFF;
        return msRegs(u32Reg);
    }
    else
    {
        return msRegs(u32Reg);
    }
}

void msWriteByte( DWORD u32Reg, BYTE u8Val )
{
    if( u32Reg & 0x00800000 ) // Sub bank
    {
        msRegs((u32Reg & 0x007FFF00)) = ( u32Reg >> 24 ) & 0xFF;
        msRegs(u32Reg) = u8Val;
    }
    else
    {
        msRegs(u32Reg) = u8Val;
    }
}

void msWriteBit( DWORD u32Reg, Bool bBit, BYTE u8BitPos )
{
    BYTE u8Value;

    u8Value = msReadByte( u32Reg );
    if( bBit )
    {
        u8Value = u8Value | u8BitPos;
    }
    else
    {
        u8Value = u8Value & ( ~u8BitPos );
    }
    msWriteByte( u32Reg, u8Value );
}

WORD msRead2Byte( DWORD u32RegLoAddr )
{
    return ((( WORD )msReadByte( u32RegLoAddr+1 ) ) << 8 ) | msReadByte( u32RegLoAddr );
}

void msWrite2Byte( DWORD u32RegLoAddr, WORD u16Value )
{
    msWriteByte( u32RegLoAddr, ( BYTE )u16Value );
    msWriteByte( u32RegLoAddr + 1, ( BYTE )( u16Value >> 8 ) );
}

void scWriteByteMask( DWORD u32Reg, BYTE u8Value, BYTE u8Mask )
{
    scWriteByte( u32Reg, ( (scReadByte( u32Reg ) & ( ~u8Mask )) | ( u8Value & u8Mask ) ) );
}

void msWriteByteMask( DWORD u32Reg, BYTE u8Value, BYTE u8Mask )
{
    msWriteByte( u32Reg, ( (msReadByte( u32Reg ) & ( ~u8Mask )) | ( u8Value & u8Mask ) ) );
}

void msWrite2ByteMask( DWORD u32RegLoAddr, WORD u16Value , WORD u16Mask )
{
    msWriteByteMask( u32RegLoAddr, ( BYTE )u16Value, ( BYTE ) u16Mask );
    msWriteByteMask( u32RegLoAddr + 1, ( BYTE )( u16Value >> 8 ), ( BYTE )( u16Mask >> 8 ) );
}

DWORD msRead3Byte( DWORD u32RegLoAddr )
{
    return ((( DWORD )msReadByte( u32RegLoAddr+2 ) ) << 16 ) | ((( DWORD )msReadByte( u32RegLoAddr+1 ) ) << 8 ) | msReadByte( u32RegLoAddr );
}

void msWrite3Byte( DWORD u32RegLoAddr, DWORD u32Value )
{
    msWriteByte( u32RegLoAddr, ( BYTE )u32Value );
    msWriteByte( u32RegLoAddr + 1, ( BYTE )( u32Value >> 8 ) );
    msWriteByte( u32RegLoAddr + 2, ( BYTE )( u32Value >> 16 ) );
}

void msWrite3ByteMask( DWORD u32RegLoAddr, DWORD u32Value , DWORD u32Mask )
{
    msWriteByteMask( u32RegLoAddr, ( BYTE )u32Value, ( BYTE ) u32Mask );
    msWriteByteMask( u32RegLoAddr + 1, ( BYTE )( u32Value >> 8 ), ( BYTE )( u32Mask >> 8 ) );
    msWriteByteMask( u32RegLoAddr + 2, ( BYTE )( u32Value >> 16 ), ( BYTE )( u32Mask >> 16 ) );
}

DWORD msRead4Byte( DWORD u32RegLoAddr )
{
    return ((( DWORD )msReadByte( u32RegLoAddr+3 ) ) << 24 ) | ((( DWORD )msReadByte( u32RegLoAddr+2 ) ) << 16 ) | ((( DWORD )msReadByte( u32RegLoAddr+1 ) ) << 8 ) | msReadByte( u32RegLoAddr );
}

void msWrite4Byte( DWORD u32RegLoAddr, DWORD u32Value )
{
    msWriteByte( u32RegLoAddr, ( BYTE )u32Value );
    msWriteByte( u32RegLoAddr + 1, ( BYTE )( u32Value >> 8 ) );
    msWriteByte( u32RegLoAddr + 2, ( BYTE )( u32Value >> 16 ) );
    msWriteByte( u32RegLoAddr + 3, ( BYTE )( u32Value >> 24 ) );
}

void msWrite4ByteMask( DWORD u32RegLoAddr, DWORD u32Value , DWORD u32Mask )
{
    msWriteByteMask( u32RegLoAddr, ( BYTE )u32Value, ( BYTE ) u32Mask );
    msWriteByteMask( u32RegLoAddr + 1, ( BYTE )( u32Value >> 8 ), ( BYTE )( u32Mask >> 8 ) );
    msWriteByteMask( u32RegLoAddr + 2, ( BYTE )( u32Value >> 16 ), ( BYTE )( u32Mask >> 16 ) );
    msWriteByteMask( u32RegLoAddr + 3, ( BYTE )( u32Value >> 24 ), ( BYTE )( u32Mask >> 24 ) );
}

#if 1//Old r/w function
//These old r/w functions are supposed to be removed.
DWORD OldRWFuncCnt=0;
BYTE old_msReadByte( DWORD u32Reg )
{
    u32Reg = u32Reg;
    OldRWFuncCnt++;
    return 0;
}

void old_msWriteByte( DWORD u32Reg, BYTE u8Val )
{
    u32Reg = u8Val;
    OldRWFuncCnt++;
}
void old_msWriteBit( DWORD u32Reg, Bool bBit, BYTE u8BitPos )
{
    u32Reg = u8BitPos | bBit;
    OldRWFuncCnt++;
}

BYTE old_scReadByte( BYTE u8Reg )
{
    u8Reg=u8Reg;
    OldRWFuncCnt++;
    return 0;
}

void old_scWriteByte( BYTE u8Reg, BYTE u8Value )
{
    u8Reg=u8Value;
    OldRWFuncCnt++;
}

void old_msWrite2Byte( DWORD u32RegLoAddr, WORD u16Value )
{
    //msWrite2Byte(u32RegLoAddr, u16Value);
    u32RegLoAddr = u16Value;
    OldRWFuncCnt++;
}

WORD old_msRead2Byte( DWORD u32RegLoAddr )
{
    //msRead2Byte(u32RegLoAddr);
    u32RegLoAddr = u32RegLoAddr;
    OldRWFuncCnt++;
    return 0;
}

void old_msWrite2ByteMask( DWORD u32RegLoAddr, WORD u16Value, WORD u16Mask )
{
    //msWrite2Byte(u32RegLoAddr, u16Value);
    u32RegLoAddr = u16Value | u16Mask;
    OldRWFuncCnt++;
}

void old_msWriteByteMask( DWORD u32Reg, BYTE u8Value, BYTE u8Mask )
{
    u32Reg = u8Value | u8Mask;
    OldRWFuncCnt++;
}
#endif

