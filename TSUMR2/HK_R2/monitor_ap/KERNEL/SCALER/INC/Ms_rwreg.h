///////////////////////////////////////////////////////////////////////////////
/// @file ms_rwreg.h
/// @brief Head file of ms_rwreg.c
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
#include "Global.h"
#ifndef _MS_RWREG_H
#define _MS_RWREG_H

#ifdef _MS_RWREG_C
#define INTERFACE
#else
#define INTERFACE   extern
#endif

/////////////////////////////////////////////////////////////////////////////
//Must use sc series R/W function if input scaler bank address is BYTE data.
//Must use bank save/restore future while use sc series function.
/////////////////////////////////////////////////////////////////////////////

#define MAIN_WINDOW 0
#define SUB_WINDOW  1

#define __msRegs            ((unsigned char  volatile *) RIU_MAP)
#define msRegs(u32Reg)      __msRegs[(((u32Reg) & 0x007FFFFF) * 2) - ((u32Reg) & 1)]
////////////////////////////////////////////////////////////////////////////////
// Need Change Sub Bank Manually if necessary, for performanace consideration in ISR !!!! //
////////////////////////////////////////////////////////////////////////////////
#define MEM_MSREAD_BYTE(u32Reg)                        (msRegs(u32Reg))
#define MEM_MSWRITE_BYTE(u32Reg,u8Value)               (msRegs(u32Reg)=(u8Value))
#define MEM_MSWRITE_BYTE_MASK(u32Reg,u8Value,u8Mask)   (msWriteByteMask(u32Reg,u8Value,u8Mask))//(msRegs(u32Reg) = ( (msRegs(u32Reg) & (~(u8Mask))) | ((u8Value) & (u8Mask))))
#define MEM_MSWRITE_BIT(u32Reg,bTrue,u8Bit)            (msWriteBit(u32Reg,bTrue,u8Bit))//(msRegs(u32Reg) = ( (msRegs(u32Reg) & (~(u8Bit))) | ((u8True)?(u8Bit):0)))


// memory R/W functions - non-cached !! directly access memory.
INTERFACE U8 msMemReadByte(U32 u32MemAddr );
INTERFACE U16 msMemRead2Byte(U32 u32MemAddr );
INTERFACE U32 msMemRead3Byte(U32 u32MemAddr );
INTERFACE U32 msMemRead4Byte(U32 u32MemAddr );
INTERFACE void msMemWriteByte(U32 u32MemAddr, U8 u8Data );
INTERFACE void msMemWrite2Byte(U32 u32MemAddr, U16 u16Data );
INTERFACE void msMemWrite3Byte( U32 u32RegLoAddr, U32 u32Value );
INTERFACE void msMemWrite4Byte( U32 u32RegLoAddr, U32 u32Value );
INTERFACE void msMemWriteByteMask( U32 u32Reg, U8 u8Value, U8 u8Mask );
INTERFACE void msMemWrite2ByteMask( U32 u32RegLoAddr, U16 u16Value , U16 u16Mask );
INTERFACE void msMemWrite3ByteMask( U32 u32RegLoAddr, U32 u32Value , U32 u32Mask );
INTERFACE void msMemWrite4ByteMask( U32 u32RegLoAddr, U32 u32Value , U32 u32Mask );

#if ENABLE_MENULOAD
#define DB_WB(dwReg,ucVal)              (u8MenuLoadFlag[ML_DEV_1])?msSWDBWriteByte(dwReg,ucVal,ML_DEV_1):msWriteByte(dwReg,ucVal)
#define DB_W2B(dwReg,wVal)              (u8MenuLoadFlag[ML_DEV_1])?msSWDBWrite2Byte(dwReg,wVal,ML_DEV_1):msWrite2Byte(dwReg,wVal)
#define DB_W3B(dwReg,dwVal)             (u8MenuLoadFlag[ML_DEV_1])?msSWDBWrite3Byte(dwReg,dwVal,ML_DEV_1):msWrite3Byte(dwReg,dwVal)
#define DB_W4B(dwReg,dwVal)             (u8MenuLoadFlag[ML_DEV_1])?msSWDBWrite4Byte(dwReg,dwVal,ML_DEV_1):msWrite4Byte(dwReg,dwVal)
#define DB_WBMask(dwReg,ucVal,bMask)    (u8MenuLoadFlag[ML_DEV_1])?msSWDBWriteByteMask(dwReg,ucVal,bMask,ML_DEV_1):msWriteByteMask(dwReg,ucVal,bMask)
#define DB_W2BMask(dwReg,wVal,wMask)    (u8MenuLoadFlag[ML_DEV_1])?msSWDBWrite2ByteMask(dwReg,wVal,wMask,ML_DEV_1):msWrite2ByteMask(dwReg,wVal,wMask)
#define DB_WBit(dwReg,bBit,bBitPos)     (u8MenuLoadFlag[ML_DEV_1])?msSWDBWriteBit(dwReg,bBit,bBitPos,ML_DEV_1):msWriteBit(dwReg,bBit,bBitPos)
#define MENU_LOAD_START()               {u8MenuLoadFlag[ML_DEV_1] = bMLEnable?1:0; if(bMLEnable){msMLInfoInit(ML_DEV_1);}} //msWriteByteMask(SC00_01, 0x00, BIT0);}
#define MENU_LOAD_END()                 {if(bMLEnable){msSWDBTrigger(ML_DEV_1); u8MenuLoadFlag[ML_DEV_1] = 0;}} //msWriteByteMask(SC00_01, BIT0, BIT0);}
#define MENU_LOAD_ACTIVE()              bMLEnable = TRUE
#else
#define DB_WB(dwReg,ucVal)              msWriteByte(dwReg, ucVal)
#define DB_W2B(dwReg,wVal)              msWrite2Byte(dwReg, wVal)
#define DB_W3B(dwReg,dwVal)             msWrite3Byte(dwReg, dwVal)
#define DB_W4B(dwReg,dwVal)             msWrite4Byte(dwReg, dwVal)
#define DB_WBMask(dwReg,ucVal,bMask)    msWriteByteMask(dwReg, ucVal, bMask)
#define DB_W2BMask(dwReg,wVal,wMask)    msWrite2ByteMask(dwReg, wVal, wMask)
#define DB_WBit(dwReg,bBit,bBitPos)     msWriteBit(dwReg,bBit,bBitPos)
#define MENU_LOAD_START()
#define MENU_LOAD_END()
#define MENU_LOAD_ACTIVE()
#endif

#if ENABLE_MENULOAD_2
#define DB_2_WB(dwReg,ucVal)            {(u8MenuLoadFlag[ML_DEV_2])?msSWDBWriteByte(dwReg,ucVal,ML_DEV_2):msWriteByte(dwReg,ucVal);}
#define DB_2_W2B(dwReg,wVal)            {(u8MenuLoadFlag[ML_DEV_2])?msSWDBWrite2Byte(dwReg,wVal,ML_DEV_2):msWrite2Byte(dwReg,wVal);}
#define DB_2_W3B(dwReg,dwVal)           {(u8MenuLoadFlag[ML_DEV_2])?msSWDBWrite3Byte(dwReg,dwVal,ML_DEV_2):msWrite3Byte(dwReg,dwVal);}
#define DB_2_W4B(dwReg,dwVal)           {(u8MenuLoadFlag[ML_DEV_2])?msSWDBWrite4Byte(dwReg,dwVal,ML_DEV_2):msWrite4Byte(dwReg,dwVal);}
#define DB_2_WBMask(dwReg,ucVal,bMask)  {(u8MenuLoadFlag[ML_DEV_2])?msSWDBWriteByteMask(dwReg,ucVal,bMask,ML_DEV_2):msWriteByteMask(dwReg,ucVal,bMask);}
#define DB_2_W2BMask(dwReg,wVal,wMask)  {(u8MenuLoadFlag[ML_DEV_2])?msSWDBWrite2ByteMask(dwReg,wVal,wMask,ML_DEV_2):msWrite2ByteMask(dwReg,wVal,wMask);}
#define DB_2_WBit(dwReg,bBit,bBitPos)   {(u8MenuLoadFlag[ML_DEV_2])?msSWDBWriteBit(dwReg,bBit,bBitPos,ML_DEV_2):msWriteBit(dwReg,bBit,bBitPos);}
#define MENU_LOAD_2_START()             {u8MenuLoadFlag[ML_DEV_2] = bMLEnable?1:0; if(bMLEnable){msMLInfoInit(ML_DEV_2);}} //msWriteByteMask(SC00_01, 0x00, BIT0);}
#define MENU_LOAD_2_END()               {if(bMLEnable){msSWDBTrigger(ML_DEV_2); u8MenuLoadFlag[ML_DEV_2] = 0;}} //msWriteByteMask(SC00_01, BIT0, BIT0);}
#else
#define DB_2_WB(dwReg,ucVal)            msWriteByte(dwReg, ucVal)
#define DB_2_W2B(dwReg,wVal)            msWrite2Byte(dwReg, wVal)
#define DB_2_W3B(dwReg,dwVal)           msWrite3Byte(dwReg, dwVal)
#define DB_2_W4B(dwReg,dwVal)           msWrite4Byte(dwReg, dwVal)
#define DB_2_WBMask(dwReg,ucVal,bMask)  msWriteByteMask(dwReg, ucVal, bMask)
#define DB_2_W2BMask(dwReg,wVal,wMask)  msWrite2ByteMask(dwReg, wVal, wMask)
#define DB_2_WBit(dwReg,bBit,bBitPos)   msWriteBit(dwReg,bBit,bBitPos)
#define MENU_LOAD_2_START()
#define MENU_LOAD_2_END()
#endif


#if ENABLE_DYNAMICSCALING
#define DS_WB(dwReg,ucVal,eIPOP)             {u8DSFlag?msDSWriteByte(dwReg,ucVal,eIPOP):msWriteByte(dwReg,ucVal);}
#define DS_W2B(dwReg,wVal,eIPOP)             {u8DSFlag?msDSWrite2Byte(dwReg,wVal,eIPOP):msWrite2Byte(dwReg,wVal);}
#define DS_W3B(dwReg,dwVal,eIPOP)            {u8DSFlag?msDSWrite3Byte(dwReg,dwVal,eIPOP):msWrite3Byte(dwReg,dwVal);}
#define DS_W4B(dwReg,dwVal,eIPOP)            {u8DSFlag?msDSWrite4Byte(dwReg,dwVal,eIPOP):msWrite4Byte(dwReg,dwVal);}
#define DS_WBMask(dwReg,ucVal,ucMask,eIPOP)  {u8DSFlag?msDSWriteByteMask(dwReg,ucVal,ucMask,eIPOP):msWriteByteMask(dwReg,ucVal,ucMask);}
#define DS_W2BMask(dwReg,wVal,wMask,eIPOP)   {u8DSFlag?msDSWrite2ByteMask(dwReg,wVal,wMask,eIPOP):msWrite2ByteMask(dwReg,wVal,wMask);}
#define DS_WBit(dwReg,bBit,ucBitPos,eIPOP)   {u8DSFlag?msDSWriteBit(dwReg,bBit,ucBitPos,eIPOP):msWriteBit(dwReg,bBit,ucBitPos);}
#define DS_START()                           {u8DSFlag = 1; msDS_Config();}
#define DS_END()                             {if(u8DSFlag){msDS_Fire(TRUE); u8DSFlag = 0;}}
#else
#define DS_WB(dwReg,ucVal,eIPOP)             msWriteByte(dwReg,ucVal)
#define DS_W2B(dwReg,wVal,eIPOP)             msWrite2Byte(dwReg,wVal)
#define DS_W3B(dwReg,dwVal,eIPOP)            msWrite3Byte(dwReg,dwVal)
#define DS_W4B(dwReg,dwVal,eIPOP)            msWrite4Byte(dwReg,dwVal)
#define DS_WBMask(dwReg,ucVal,ucMask,eIPOP)  msWriteByteMask(dwReg,ucVal,ucMask)
#define DS_W2BMask(dwReg,wVal,wMask,eIPOP)   msWrite2ByteMask(dwReg,wVal,wMask)
#define DS_WBit(dwReg,bBit,ucBitPos,eIPOP)   msWriteBit(dwReg,bBit,ucBitPos)
#define DS_START()
#define DS_END()
#endif


INTERFACE BYTE scReadByte( DWORD u32Reg );
INTERFACE void scWriteByte( DWORD u32Reg, BYTE u8Val );
INTERFACE BYTE msReadByte( DWORD u32Reg );
INTERFACE void msWriteByte( DWORD u32Reg, BYTE u8Val );
INTERFACE WORD msRead2Byte( DWORD u32RegLoAddr );
INTERFACE void msWriteBit( DWORD u32Reg, Bool bBit, BYTE u8BitPos );
INTERFACE void msWrite2Byte( DWORD u32RegLoAddr, WORD u16Value );
INTERFACE void scWriteByteMask( DWORD u32Reg, BYTE u8Value, BYTE u8Mask );
INTERFACE void msWriteByteMask( DWORD u32Reg, BYTE u8Value, BYTE u8Mask );
INTERFACE void msWrite2ByteMask( DWORD u32RegLoAddr, WORD wValue , WORD ucMask );
INTERFACE DWORD msRead3Byte( DWORD u32RegLoAddr );
INTERFACE void msWrite3Byte( DWORD u32RegLoAddr, DWORD u32Value );
INTERFACE void msWrite3ByteMask( DWORD u32RegLoAddr, DWORD u32Value , DWORD u32Mask );
INTERFACE DWORD msRead4Byte( DWORD u32RegLoAddr );
INTERFACE void msWrite4Byte( DWORD u32RegLoAddr, DWORD u32Value );
INTERFACE void msWrite4ByteMask( DWORD u32RegLoAddr, DWORD u32Value , DWORD u32Mask );


//////////////////////
/////// old ///////////
#if 1//Old r/w function
//These old r/w functions are supposed to be removed.
INTERFACE BYTE old_msReadByte( DWORD u32Reg );
INTERFACE void old_msWriteByte( DWORD u32Reg, BYTE u8Val );
INTERFACE void old_msWriteBit( DWORD u32Reg, Bool bBit, BYTE u8BitPos );
INTERFACE BYTE old_scReadByte( BYTE u8Reg );
INTERFACE void old_scWriteByte( BYTE u8Reg, BYTE u8Value );
INTERFACE void old_msWrite2Byte( DWORD u32RegLoAddr, WORD u16Value );
INTERFACE WORD old_msRead2Byte( DWORD u32RegLoAddr );
INTERFACE void old_msWrite2ByteMask( DWORD u32RegLoAddr, WORD u16Value, WORD u16Mask );
INTERFACE void old_msWriteByteMask( DWORD u32Reg, BYTE u8Value, BYTE u8Mask );
#endif
/////// old ///////////
//////////////////////


#undef INTERFACE


#endif   ///_MS_RWREG_H
