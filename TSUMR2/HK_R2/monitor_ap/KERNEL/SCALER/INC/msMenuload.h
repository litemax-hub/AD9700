#ifndef _MS_Menuload_H
#define _MS_Menuload_H

#include "board.h"
#include "types.h"
#include "drvMcu.h"
#include "drvIMI.h"


#ifdef _MS_Menuload_C
#define INTERFACE
#else
#define INTERFACE   extern
#endif

#if (CHIP_ID == CHIP_MT9700)
#define ML_START_ADDR       DEF_IMI_ML_START_ADDR
#define ML_SIZE             (ENABLE_MENULOAD_2?(DEF_IMI_ML_SIZE/2):DEF_IMI_ML_SIZE)
#define ML_2_START_ADDR     (ML_START_ADDR + ML_SIZE)
#define ML_2_SIZE           (DEF_IMI_ML_SIZE - ML_SIZE)
#else
#define ML_START_ADDR       MIU_MENULOAD_ADDR_START
#define ML_SIZE             MIU_MENULOAD_SIZE
#define ML_2_START_ADDR     MIU_MENULOAD_2_ADDR_START
#define ML_2_SIZE           MIU_MENULOAD_2_SIZE
#define DS_START_ADDR       MIU_DS_ADDR_START
#define DS_SIZE             MIU_DS_SIZE
#endif

typedef enum
{
    ML_DEV_1,
    ML_DEV_2,
    ML_DEV_NUM
}MS_MLOAD_DEV;

typedef struct
{
    MS_PHYADDR PhyAddr[ML_DEV_NUM];
    DWORD u32WPoint[ML_DEV_NUM];
    DWORD u32MaxCmdCnt[ML_DEV_NUM];
    BYTE u8WByteEn[ML_DEV_NUM];
}MS_MLoad_Info;

typedef struct
{
    BYTE u8Data_L;
    BYTE u8Data_H;
    BYTE u8Addr;
    BYTE u8Bank_L;
    BYTE u8Bank_H;
    BYTE u8SpreadMode;
    BYTE u8Mask_L;
    BYTE u8Mask_H;
}MS_MLOAD_CMD;

typedef enum
{
    OP_TRIG,
    SW_TRIG,
    IP_TRIG
}MS_MLOAD_TRIG_SEL;

typedef enum
{
    DS_IP,
    DS_OP,
    DS_SEL_NUM,
}DS_IP_OP_SEL;

typedef struct
{
    MS_PHYADDR PhyAddr;
    DWORD u32BaseAddrOffset;
    BYTE u8IndexNum;
    DWORD u32WPointOPM;
    DWORD u32WPointIPM;    
    DWORD u32WPointAlign; //indicate the max counts between OPM/IPM
    DWORD u32MaxCmdCnt;
    BYTE u8WByteEn[DS_SEL_NUM]; 
}MS_DS_Info;

#if (CHIP_ID == CHIP_MT9700)
#define MS_MLOAD_IMI_BUS_WIDTH  128 //unit: bit
#define MS_MLOAD_MEM_BASE_UNIT  (MS_MLOAD_IMI_BUS_WIDTH / 8) //unit: byte
#define MS_MLOAD_CMD_ALIGN      (MS_MLOAD_MEM_BASE_UNIT / sizeof(MS_MLOAD_CMD))
#define MS_MLOAD_REQ_LEN        0x02 //how many cmd ML request from IMI at a time, cmd unit: MS_MLOAD_IMI_BUS_WIDTH
#else
#define MS_MLOAD_MIU_BUS_WIDTH  256 //unit: bit
#define MS_MLOAD_MEM_BASE_UNIT  (MS_MLOAD_MIU_BUS_WIDTH / 8) //unit: byte
#define MS_MLOAD_CMD_ALIGN      (MS_MLOAD_MEM_BASE_UNIT / sizeof(MS_MLOAD_CMD))
#define MS_MLOAD_REQ_LEN        0x02 //how many cmd ML request from MIU at a time, cmd unit: MS_MLOAD_MIU_BUS_WIDTH

#define MS_DS_REQ_LEN           0x08 // MIU request length
#define MS_DS_REQ_THD           0x08 // MIU request threshold
#endif


#if ENABLE_MENULOAD || ENABLE_MENULOAD_2 || ENABLE_DYNAMICSCALING || ENABLE_PQ_R2
INTERFACE void msMLoad_DS_Init(void);
#endif


#if ENABLE_MENULOAD || ENABLE_MENULOAD_2
INTERFACE MS_MLoad_Info stMLoadInfo;

INTERFACE BOOL msMLoad_Fire(BOOL bImmediate, MS_MLOAD_DEV eDev);
INTERFACE BOOL msMLoad_WriteCmd(DWORD u32Addr, WORD u16Data, WORD u16Mask, MS_MLOAD_DEV eDev);
INTERFACE void msMLoad_Reset(MS_MLOAD_DEV eDev);
INTERFACE void msMLoad_Init(void);
INTERFACE void msSWDBWriteByte(DWORD dwReg, BYTE ucValue, MS_MLOAD_DEV eDev);
INTERFACE void msSWDBWrite2Byte(DWORD dwReg, WORD wValue, MS_MLOAD_DEV eDev);
INTERFACE void msSWDBWrite3Byte(DWORD dwReg, DWORD dwValue, MS_MLOAD_DEV eDev);
INTERFACE void msSWDBWrite4Byte(DWORD dwReg, DWORD dwValue, MS_MLOAD_DEV eDev);
INTERFACE void msSWDBWriteByteMask(DWORD dwReg, BYTE ucValue, BYTE bMask, MS_MLOAD_DEV eDev);
INTERFACE void msSWDBWrite2ByteMask(DWORD dwReg, WORD wValue, WORD wMask, MS_MLOAD_DEV eDev);
INTERFACE void msSWDBWriteBit(DWORD dwReg, BOOL bBit, BYTE bBitPos, MS_MLOAD_DEV eDev);
INTERFACE void msSWDBTrigger(MS_MLOAD_DEV eDev);
INTERFACE void msMLInfoInit(MS_MLOAD_DEV eDev);
#endif


#if ENABLE_DYNAMICSCALING
INTERFACE MS_DS_Info stDSInfo;

INTERFACE BOOL msDS_Done_Check(void);
INTERFACE BOOL msDS_Fire(BOOL bImmediate);
INTERFACE void msDS_Config(void);
INTERFACE void msDS_Init(void);
INTERFACE void msDSWriteByte(DWORD dwReg, BYTE ucVal, DS_IP_OP_SEL eIPOP);
INTERFACE void msDSWrite2Byte(DWORD dwReg, WORD wVal, DS_IP_OP_SEL eIPOP);
INTERFACE void msDSWrite3Byte(DWORD dwReg, DWORD dwValue, DS_IP_OP_SEL eIPOP);
INTERFACE void msDSWrite4Byte(DWORD dwReg, DWORD dwValue, DS_IP_OP_SEL eIPOP);
INTERFACE void msDSWriteByteMask(DWORD dwReg, BYTE ucVal, BYTE ucMask, DS_IP_OP_SEL eIPOP);
INTERFACE void msDSWrite2ByteMask(DWORD dwReg, WORD wVal, WORD wMask, DS_IP_OP_SEL eIPOP);
INTERFACE void msDSWriteBit(DWORD dwReg, BOOL bBit, BYTE ucBitPos, DS_IP_OP_SEL eIPOP);
#endif


#undef INTERFACE

#endif

