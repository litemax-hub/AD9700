#define _MS_Menuload_C

#include "msMenuload.h"
#include "Ms_rwreg.h"
#include "ms_reg_TSUMR2.h"
#include "MsOS.h"
#include "Global.h"


#define ML_DEBUG_MSG    0
#if ENABLE_DEBUG && ML_DEBUG_MSG
#define ML_printData(str, value)   printData(str, value)
#define ML_printMsg(str)           printMsg(str)
#else
#define ML_printData(str, value)
#define ML_printMsg(str)
#endif


const BYTE SC00_WORD_ACCESS_REG[] =
{
    //16-bit SC00 addr
    //0x59 low-byte is BYTE access, but not used
    0x28, 0x29, 0x2A, 0x2B, 0x2C,
    0x2D, 0x59, 0x5A, 0x60, 0x61,
    0x62, 0x63, 0x64, 0x6C, 0x6D
};


#if ENABLE_MENULOAD || ENABLE_MENULOAD_2 || ENABLE_DYNAMICSCALING || ENABLE_PQ_R2
void msMLoad_Set_Riu(BOOL bEnable)
{
    if(bEnable)
        msWriteByteMask(SC1F_21, 0x10, 0x10);
    else
        msWriteByteMask(SC1F_21, 0x00, 0x10);
}


void msMLoad_DS_Init(void)
{
#if (CHIP_ID == CHIP_MT9700)
    msWrite2ByteMask(SC1F_26, 0x4000, 0xC000); // IMI bus sel
#else
    msWrite2ByteMask(SC1F_26, 0xC000, 0xC000); // MIU bus sel
#endif

    msWrite2ByteMask(0x00012E, 0x0001, 0x0001); // enable bit_operation
    msWrite2ByteMask(SC1F_0E, 0xC000, 0xE000);

    //sw rst before set SC1F_70[0]
    msWrite2ByteMask(SC1F_26, 0x1C00, 0x1C00);
    msWrite2ByteMask(SC1F_26, 0x0000, 0x1C00);

    msWrite2ByteMask(SC1F_E0, 0x4003, 0xC003);

    //MLoad WORD access and bit mask enable
    msWriteByteMask(SC1F_2E, 0x0F, 0x0F);

    msMLoad_Set_Riu(_ENABLE);

    //--- MLoad
    //trigger sel
    msWriteByteMask(SC1F_3B, MENULOAD_TRIG << 4, 0x70); // mload
    msWriteByteMask(SC1F_4B, MENULOAD_2_TRIG << 4, 0x70); // mload2
    
    //IP/OP trig sel
    msWriteByteMask(SC0D_6A, 0x00, 0x0F); // IP triggered by VSync falling and OP triggered by VDE falling
    //--- MLoad end

#if ENABLE_PQ_R2
    msWrite4Byte(MBX_MLOAD_2_START, ML_2_START_ADDR);
    msWrite4Byte(MBX_MLOAD_2_SIZE, ML_2_SIZE);
#endif
}
#endif


#if ENABLE_MENULOAD || ENABLE_MENULOAD_2
BYTE msMLoad_Reg_Offset(MS_MLOAD_DEV eDev)
{
    BYTE reg_offset = 0;

    if(eDev == ML_DEV_2)
    {
        reg_offset = 0x40;
    }

    return reg_offset;
}


WORD msMLoad_Get_Status(MS_MLOAD_DEV eDev)
{
    BYTE reg_offset;

    reg_offset = msMLoad_Reg_Offset(eDev);

    return ((msRead2Byte(SC1F_04 + reg_offset) & 0x8000) >> 15);
}


BOOL msMLoad_Check_Done(MS_MLOAD_DEV eDev)
{
    BOOL bEn;

    if(msMLoad_Get_Status(eDev))
        bEn = FALSE;
    else
        bEn = TRUE;
        
    return bEn;
}


void msMLoad_Set_Len(WORD u16Len)
{
    u16Len &= 0x7FF;
    msWrite2ByteMask(SC1F_04, u16Len, 0x7FF);
}


void msMLoad_Set_Depth(WORD u16depth, MS_MLOAD_DEV eDev)
{
    BYTE reg_offset;

    reg_offset = msMLoad_Reg_Offset(eDev);

    msWrite2Byte(SC1F_02 + reg_offset, u16depth);
}


BOOL msMLoad_Set_Base_Addr(DWORD u32addr, MS_MLOAD_DEV eDev)
{
    BOOL bRtn = FALSE;
    BYTE reg_offset;

    reg_offset = msMLoad_Reg_Offset(eDev);

    if(u32addr % MS_MLOAD_MEM_BASE_UNIT)
    {
        ML_printMsg("[FAIL] MLoad base addr is not align REG unit\n");
    }
    else
    {
        u32addr /= MS_MLOAD_MEM_BASE_UNIT;

        // base addr 29 bits
        msWrite2Byte(SC1F_06 + reg_offset, (WORD)(u32addr & 0xFFFF));
        msWrite2ByteMask(SC1F_08 + reg_offset, (WORD)((u32addr & 0x1FFF0000)>>16), 0x1FFF);

        bRtn = TRUE;
    }

    return bRtn;
}


void msMLoad_Set_On_Off(BOOL bEn, MS_MLOAD_DEV eDev)
{
    BYTE reg_offset;

    reg_offset = msMLoad_Reg_Offset(eDev);

    if(bEn)
        msWrite2ByteMask(SC1F_04 + reg_offset, 0x8000, 0x8000);
    else
        msWrite2ByteMask(SC1F_04 + reg_offset, 0x0000, 0x8000);
}


BOOL msMLoad_Trigger(DWORD u32BaseAddr, WORD u16CmdCnt, MS_MLOAD_DEV eDev)
{
    BOOL bRtn;
    
    ML_printData("ML DEV_%d msMLoad_Trigger\n", eDev);
    ML_printData("u32BaseAddr: 0x%x\n", u32BaseAddr);
    ML_printData("u16CmdCnt: %d\n", u16CmdCnt);

    msMLoad_Set_Len(MS_MLOAD_REQ_LEN); //length of DMA request
    bRtn = msMLoad_Set_Base_Addr(u32BaseAddr, eDev);

    if(bRtn)
    {
        msMLoad_Set_Depth(u16CmdCnt, eDev);
        msMLoad_Set_On_Off(_ENABLE, eDev);
    }

    return bRtn;
}


void msMLoad_AddCmd(MS_MLOAD_CMD u64Cmd, MS_MLOAD_DEV eDev)
{
    DWORD u32DRAMAddr;
    MS_MLOAD_CMD *pu64Addr = 0;

	UNUSED(pu64Addr);
    ML_printData("ML DEV_%d msMLoad_AddCmd\n", eDev);

    u32DRAMAddr = stMLoadInfo.PhyAddr[eDev] + (stMLoadInfo.u32WPoint[eDev]*sizeof(MS_MLOAD_CMD));
    ML_printData("DRAM_Addr = 0x%x\n", u32DRAMAddr);

    if(stMLoadInfo.u32WPoint[eDev] < stMLoadInfo.u32MaxCmdCnt[eDev])
    {
    #if (CHIP_ID == CHIP_MT9700)
        IMI_WriteBytes(u32DRAMAddr, (BYTE*)&u64Cmd, (DWORD)sizeof(MS_MLOAD_CMD));
    #else
        pu64Addr = (void *)(0x80000000 | (u32DRAMAddr));
        *pu64Addr = u64Cmd;
    #endif
    
        ML_printMsg("(WORD)Mask, (BYTE)spread mode, (WORD)Bank, (BYTE)addr, (WORD)data: ");
        ML_printData("%x", (u64Cmd.u8Mask_H << 8) | u64Cmd.u8Mask_L);
        ML_printData("%x", u64Cmd.u8SpreadMode);
        ML_printData("%x", (u64Cmd.u8Bank_H << 8) | u64Cmd.u8Bank_L);
        ML_printData("%x", u64Cmd.u8Addr);
        ML_printData("%x", (u64Cmd.u8Data_H << 8) | u64Cmd.u8Data_L);
    }
    else
    {
        ML_printData("[FAIL] ML DEV_%d Add Cmd Too Many !!!!!!!!!!!!!!!!!\n", eDev);
    }

    stMLoadInfo.u32WPoint[eDev]++;
}


BOOL msMLoad_KickOff(MS_MLOAD_DEV eDev)
{
    BOOL bRtn = FALSE;
    MS_MLOAD_CMD u64EndCmd;
    WORD u16CmdCnt;
    BYTE reg_offset;

    ML_printData("ML DEV_%d msMLoad_KickOff\n", eDev);

    reg_offset = msMLoad_Reg_Offset(eDev);

    if(msMLoad_Check_Done(eDev) && stMLoadInfo.u32WPoint[eDev])
    {
        if(stMLoadInfo.u8WByteEn[eDev] == 1)
        {
            stMLoadInfo.u8WByteEn[eDev] = 0;

            //SC1F_17[7:0] = 0x0F, enable WORD access for end cmd and bit mask is available
            u64EndCmd.u8Mask_H = 0x00;
            u64EndCmd.u8Mask_L = 0xFF;
            u64EndCmd.u8SpreadMode = 0x80;
            u64EndCmd.u8Bank_H = 0x13;
            u64EndCmd.u8Bank_L = 0x1F;
            u64EndCmd.u8Addr = 0x17;
            u64EndCmd.u8Data_H = 0x00;
            u64EndCmd.u8Data_L = 0x8F;

            msMLoad_AddCmd(u64EndCmd, eDev);
        }

        //SC1F_02[15], SC1F_22[15]
        u64EndCmd.u8Mask_H = 0x80;
        u64EndCmd.u8Mask_L = 0x00;
        u64EndCmd.u8SpreadMode = 0x80;
        u64EndCmd.u8Bank_H = 0x13;
        u64EndCmd.u8Bank_L = 0x1F;
        u64EndCmd.u8Addr = 0x02 + (reg_offset/2);
        u64EndCmd.u8Data_H = 0x00;
        u64EndCmd.u8Data_L = 0x00;

        msMLoad_AddCmd(u64EndCmd, eDev);

        //DUMMY CMD
        u64EndCmd.u8Mask_H = 0xFF;
        u64EndCmd.u8Mask_L = 0xFF;
        u64EndCmd.u8SpreadMode = 0x80;
        u64EndCmd.u8Bank_H = 0x13;
        u64EndCmd.u8Bank_L = 0x1F;
        u64EndCmd.u8Addr = 0x7F;
        u64EndCmd.u8Data_H = 0x00;
        u64EndCmd.u8Data_L = 0x00;

        while((stMLoadInfo.u32WPoint[eDev] % MS_MLOAD_CMD_ALIGN) || ((stMLoadInfo.u32WPoint[eDev]/MS_MLOAD_CMD_ALIGN) % MS_MLOAD_REQ_LEN))
            msMLoad_AddCmd(u64EndCmd, eDev); //ADD DUMMY CMD

        if((stMLoadInfo.u32WPoint[eDev] > stMLoadInfo.u32MaxCmdCnt[eDev]) || ((stMLoadInfo.u32WPoint[eDev] /MS_MLOAD_CMD_ALIGN) > 0xFFFF))
        {
            ML_printData("[FAIL] ML DEV_%d Cmd Too Many !!!!!!!!!!!!!!!!!\n", eDev);
        }
        else
        {
            u16CmdCnt = (WORD)(stMLoadInfo.u32WPoint[eDev] /MS_MLOAD_CMD_ALIGN);
            
        #if (CHIP_ID == CHIP_MT9701)
            MsOS_FlushMemory();
        #endif
        
            bRtn = msMLoad_Trigger(stMLoadInfo.PhyAddr[eDev], u16CmdCnt, eDev);

            ML_printData("ML after fire u32WPoint = %d\n", stMLoadInfo.u32WPoint[eDev]);
        }
    }
    else
    {
        ML_printData("[FAIL] ML Dev_%d KickOff\n", eDev);
    }

    return bRtn;
}


BOOL msMLoad_Fire(BOOL bImmediate, MS_MLOAD_DEV eDev)
{
    DWORD u32Delayms = 0;
    BOOL bRtn = TRUE;

    ML_printData("ML DEV_%d msMLoad_Fire\n", eDev);
    ML_printData("u32WPoint = %d\n", stMLoadInfo.u32WPoint[eDev]);

    bRtn = msMLoad_KickOff(eDev);

    if(bImmediate)
    {
        while((msMLoad_Check_Done(eDev) == FALSE) && (u32Delayms < 600))
        {
            Delay1ms(1);
            u32Delayms++;
        }

        // Patch HW outpu Vsync plus width too short and cause MLoad missing.
        // T3 U06 will fix it.
        if(bRtn == FALSE)
        {
            ML_printData("[FAIL] ML DEV_%d Fail to Trigger\n", eDev);
        }
        else if(msMLoad_Check_Done(eDev) == FALSE)
        {
            ML_printData("[FAIL] ML DEV_%d MLoad TimeOut\n", eDev);
            bRtn = FALSE;
        }
        else
        {
            ML_printData("ML DEV_%d MLoad Done!!!\n", eDev);
        }

        msMLoad_Reset(eDev);
    }

    return bRtn;
}


//-------------------------------------------------------------------------------------------------
/// - For TSUMG, Write full bank command to the Menuload buffer
/// @param  u32Addr                 \b IN: the address (bank + 16-bit address)
/// @param  u16Data                 \b IN: the data
/// @param  u16Mask                 \b IN: the mask
//-------------------------------------------------------------------------------------------------
BOOL msMLoad_WriteCmd(DWORD u32Addr, WORD u16Data, WORD u16Mask, MS_MLOAD_DEV eDev)
{
    BOOL bRtn = FALSE;
    BOOL bByteAccess = TRUE;
    MS_MLOAD_CMD u64Cmd;
    MS_MLOAD_CMD u64SwitchCmd;
    WORD u16CurBank;
    BYTE u8CurSubBank;
    BYTE u8Data_H, u8Data_L;
    BYTE i;

    ML_printData("ML DEV_%d msMLoad_WriteCmd\n", eDev);
    ML_printData("u16Mask: 0x%x\n", u16Mask);
    ML_printData("u32Addr_H: 0x%x\n", (WORD)(u32Addr >> 16));
    ML_printData("u32Addr_L: 0x%x\n", (WORD)u32Addr);
    ML_printData("u16Data: 0x%x\n", u16Data);

    // MenuLoad CMD Format (MSB --- LSB):
    // Mask 		--> 16 bits [63:48]
    // Spread mode 	--> [47]
    // Don't care 	--> 7 bits [46:40]
    // Addr  		--> 24 bits [39:16]	(bank + 16 bit addr)
    // value data 	--> 16 bits [15:0]
    // once launch 64 bits data to RIU
    // i.e. 64bits command: 0xFF FF 80 1310 10 88 88
    // Mask:0xFFFF, Spread mode:0x80, Bank:1310(SC10), Addr:10, data:0x8888

    u16CurBank = (WORD)((u32Addr & 0x007FFF00) >> 8);
    u8CurSubBank = (BYTE)(u32Addr >> 24);

    if((u16Mask != 0x0000) && (u16CurBank & 0xFF00) && (msMLoad_Check_Done(eDev)))
    {
        u64Cmd.u8Mask_H = (BYTE)(u16Mask >> 8);
        u64Cmd.u8Mask_L = (BYTE)u16Mask;
        u64Cmd.u8SpreadMode = 0x80;
        u64Cmd.u8Addr = ((BYTE)u32Addr >> 1);

        for(i = 0; i < sizeof(SC00_WORD_ACCESS_REG); i++)
        {
            if(((u32Addr & 0x800000) && (u8CurSubBank == 0) && (u64Cmd.u8Addr == SC00_WORD_ACCESS_REG[i]))
                || ((u32Addr & 0x800000) && (u8CurSubBank != 0)) || ((u32Addr & 0x800000) == 0))
            {
                bByteAccess = FALSE;
                break;
            }
        }

        ML_printData("bByteAccess = %d\n", bByteAccess);

        //BYTE access REG lo byte set to hi byte: SC1F_17[1:0] = 2b'11 or 2b'10
        //BYTE access REG lo byte set to lo byte: SC1F_17[1:0] = 2b'01
        //SC1F_17[1:0] = 2b'10 or 2b'01 bit mask is not available.
        if(bByteAccess)
        {
            u64Cmd.u8Mask_L = u64Cmd.u8Mask_H; //For 8-bit access REG write hi byte mask
            u64Cmd.u8Bank_H = 0x13;
            u64Cmd.u8Bank_L = u8CurSubBank;

            u8Data_H = (BYTE)(u16Data >> 8);

            if((u16Mask & 0x00FF) == 0x00FF)
                u8Data_L = (BYTE)u16Data;
            else
                u8Data_L = ((msReadByte(u32Addr) & ~((BYTE)u16Mask)) | ((BYTE)u16Data & (BYTE)u16Mask));

            //To reduce cmd count, check last cmd is BYTE or WORD access first.
            while(u16Mask)
            {
                if(stMLoadInfo.u8WByteEn[eDev] == 1)
                {
                    if(u16Mask & 0x00FF)
                    {
                        u64Cmd.u8Data_L = u8Data_L;
                        u16Mask &= 0xFF00;
                    }
                    else
                    {
                        stMLoadInfo.u8WByteEn[eDev] = 0;

                        //SC1F_17[7:0] = 0x0F
                        u64SwitchCmd.u8Mask_H = 0x00;
                        u64SwitchCmd.u8Mask_L = 0xFF;
                        u64SwitchCmd.u8SpreadMode = 0x80;
                        u64SwitchCmd.u8Bank_H = 0x13;
                        u64SwitchCmd.u8Bank_L = 0x1F;
                        u64SwitchCmd.u8Addr = 0x17;
                        u64SwitchCmd.u8Data_H = 0x00;
                        u64SwitchCmd.u8Data_L = 0x8F;

                        msMLoad_AddCmd(u64SwitchCmd, eDev);
                        u64Cmd.u8Data_L = u8Data_H;
                        u16Mask &= 0x00FF;
                    }
                }
                else
                {
                    if(u16Mask & 0xFF00)
                    {
                        u64Cmd.u8Data_L = u8Data_H;
                        u16Mask &= 0x00FF;
                    }
                    else
                    {
                        stMLoadInfo.u8WByteEn[eDev] = 1;

                        //SC1F_17[7:0] = 0x0D
                        u64SwitchCmd.u8Mask_H = 0x00;
                        u64SwitchCmd.u8Mask_L = 0xFF;
                        u64SwitchCmd.u8SpreadMode = 0x80;
                        u64SwitchCmd.u8Bank_H = 0x13;
                        u64SwitchCmd.u8Bank_L = 0x1F;
                        u64SwitchCmd.u8Addr = 0x17;
                        u64SwitchCmd.u8Data_H = 0x00;
                        u64SwitchCmd.u8Data_L = 0x8D;

                        msMLoad_AddCmd(u64SwitchCmd, eDev);
                        u64Cmd.u8Data_L = u8Data_L;
                        u16Mask &= 0xFF00;
                    }
                }
                u64Cmd.u8Data_H = 0x00;
                msMLoad_AddCmd(u64Cmd, eDev);
            }
        }
        else
        {
            if(stMLoadInfo.u8WByteEn[eDev] == 1)
            {
                stMLoadInfo.u8WByteEn[eDev] = 0;

                //SC1F_17[7:0] = 0x0F
                u64SwitchCmd.u8Mask_H = 0x00;
                u64SwitchCmd.u8Mask_L = 0xFF;
                u64SwitchCmd.u8SpreadMode = 0x80;
                u64SwitchCmd.u8Bank_H = 0x13;
                u64SwitchCmd.u8Bank_L = 0x1F;
                u64SwitchCmd.u8Addr = 0x17;
                u64SwitchCmd.u8Data_H = 0x00;
                u64SwitchCmd.u8Data_L = 0x8F;

                msMLoad_AddCmd(u64SwitchCmd, eDev);
            }

            if(u32Addr & 0x800000)
            {
                u64Cmd.u8Bank_H = 0x13;
                u64Cmd.u8Bank_L = u8CurSubBank;
            }
            else
            {
                u64Cmd.u8Bank_H = (BYTE)(u16CurBank >> 8);
                u64Cmd.u8Bank_L = (BYTE)u16CurBank;
            }

            u64Cmd.u8Data_H = (BYTE)(u16Data >> 8);
            u64Cmd.u8Data_L = (BYTE)u16Data;

            msMLoad_AddCmd(u64Cmd, eDev);
        }
        
        bRtn = TRUE;
    }
    else
    {
        ML_printMsg("[FAIL] ML Write Cmd\n");

        if((u16CurBank & 0xFF00) == 0)
        {
            ML_printMsg("ML Cannot Write PM Domain REG\n");
        }
    }

    return bRtn;
}


void msMLoad_Reset(MS_MLOAD_DEV eDev)
{
    BOOL bDone;
    MS_MLOAD_CMD u64Cmd;
    
    msMLoad_Set_On_Off(_DISABLE, eDev);

    bDone = msMLoad_Check_Done(ML_DEV_1);
    bDone &= msMLoad_Check_Done(ML_DEV_2);

#if ENABLE_DYNAMICSCALING
    bDone &= msDS_Done_Check();
#endif

    if(bDone)
    {
        //sw rst, to avoid DS/MLoad engine abnormal after last time being triggered error
        msWrite2ByteMask(SC1F_26, 0x1C00, 0x1C00);
        msWrite2ByteMask(SC1F_26, 0x0000, 0x1C00);
    }

    stMLoadInfo.u32WPoint[eDev] = 0;
    stMLoadInfo.u8WByteEn[eDev] = 0;

    //MLoad WORD access and bit mask enable, SC1F_17[7:0] = 0x0F
    u64Cmd.u8Mask_H = 0x00;
    u64Cmd.u8Mask_L = 0xFF;
    u64Cmd.u8SpreadMode = 0x80;
    u64Cmd.u8Bank_H = 0x13;
    u64Cmd.u8Bank_L = 0x1F;
    u64Cmd.u8Addr = 0x17;
    u64Cmd.u8Data_H = 0x00;
    u64Cmd.u8Data_L = 0x8F;

    msMLoad_AddCmd(u64Cmd, eDev);
}


//-------------------------------------------------------------------------------------------------
/// Initialize the Menuload
/// @param  PhyAddr                 \b IN: the physical address for the menuload
/// @param  u32BufByteLen           \b IN: the buffer length of the menu load commands
//-------------------------------------------------------------------------------------------------
void msMLoad_Init(void)
{
    stMLoadInfo.u8WByteEn[ML_DEV_1] = 0;
    stMLoadInfo.PhyAddr[ML_DEV_1] = ML_START_ADDR;
    stMLoadInfo.u32MaxCmdCnt[ML_DEV_1] = (DWORD)(ML_SIZE/sizeof(MS_MLOAD_CMD));

    stMLoadInfo.u8WByteEn[ML_DEV_2] = 0;
    stMLoadInfo.PhyAddr[ML_DEV_2] = ML_2_START_ADDR;
    stMLoadInfo.u32MaxCmdCnt[ML_DEV_2] = (DWORD)(ML_2_SIZE/sizeof(MS_MLOAD_CMD));

    ML_printData("MLoad base Addr = 0x%x\n", stMLoadInfo.PhyAddr[ML_DEV_1]);
    ML_printData("MLoad_2 base Addr = 0x%x\n", stMLoadInfo.PhyAddr[ML_DEV_2]);
}


void msSWDBWriteByte(DWORD dwReg, BYTE ucValue, MS_MLOAD_DEV eDev)
{
    if (dwReg & BIT0) //odd addr
        msMLoad_WriteCmd(dwReg, (WORD)(ucValue << 8), 0xFF00, eDev);
    else //even addr
        msMLoad_WriteCmd(dwReg, (WORD)ucValue, 0x00FF, eDev);
}


void msSWDBWrite2Byte(DWORD dwReg, WORD wValue, MS_MLOAD_DEV eDev)
{
    if (dwReg & BIT0) //odd addr
    {
        msMLoad_WriteCmd(dwReg, (WORD)(wValue << 8), 0xFF00, eDev);
        msMLoad_WriteCmd(dwReg + 1, (WORD)(wValue >> 8), 0x00FF, eDev);
    }
    else //even addr
        msMLoad_WriteCmd(dwReg, wValue, 0xFFFF, eDev);
}


void msSWDBWrite3Byte(DWORD dwReg, DWORD dwValue, MS_MLOAD_DEV eDev)
{
    if(dwReg & BIT0)//odd addr
    {
        msMLoad_WriteCmd(dwReg, (WORD)(dwValue << 8), 0xFF00, eDev);
        msMLoad_WriteCmd(dwReg + 1, (WORD)(dwValue >> 8), 0xFFFF, eDev);
    }
    else //even addr
    {
        msMLoad_WriteCmd(dwReg, (WORD)dwValue, 0xFFFF, eDev);
        msMLoad_WriteCmd(dwReg + 2, (WORD)(dwValue >> 16), 0x00FF, eDev);
    }
}


void msSWDBWrite4Byte(DWORD dwReg, DWORD dwValue, MS_MLOAD_DEV eDev)
{
    if(dwReg & BIT0) //odd addr
    {
        msMLoad_WriteCmd(dwReg, (WORD)(dwValue << 8), 0xFF00, eDev);
        msMLoad_WriteCmd(dwReg + 1, (WORD)(dwValue >> 8), 0xFFFF, eDev);
        msMLoad_WriteCmd(dwReg + 3, (WORD)(dwValue >> 24), 0x00FF, eDev);
    }
    else //even addr
    {
        msMLoad_WriteCmd(dwReg, (WORD)dwValue, 0xFFFF, eDev);
        msMLoad_WriteCmd(dwReg + 2, (WORD)(dwValue >> 16), 0xFFFF, eDev);
    }
}


void msSWDBWriteByteMask(DWORD dwReg, BYTE ucValue, BYTE bMask, MS_MLOAD_DEV eDev)
{
    if (dwReg & BIT0) //odd addr
        msMLoad_WriteCmd(dwReg, (WORD)(ucValue << 8), (WORD)bMask << 8, eDev);
    else //even addr
        msMLoad_WriteCmd(dwReg, (WORD)ucValue, (WORD)bMask, eDev);
}


void msSWDBWrite2ByteMask(DWORD dwReg, WORD wValue, WORD wMask, MS_MLOAD_DEV eDev)
{
    if (dwReg & BIT0) //odd addr
    {
        msMLoad_WriteCmd(dwReg, (WORD)(wValue << 8), wMask << 8, eDev);
        msMLoad_WriteCmd(dwReg + 1, (WORD)(wValue >> 8), wMask >> 8, eDev);
    }
    else //even addr
        msMLoad_WriteCmd(dwReg, wValue, wMask, eDev);
}


void msSWDBWriteBit(DWORD dwReg, BOOL bBit, BYTE bBitPos, MS_MLOAD_DEV eDev)
{
    WORD wValue;

    if(bBit)
        wValue = (WORD)bBitPos;
    else
        wValue = 0x0000;

    if (dwReg & BIT0) //odd addr
        msMLoad_WriteCmd(dwReg, (WORD)(wValue << 8), (WORD)bBitPos << 8, eDev);
    else //even addr
        msMLoad_WriteCmd(dwReg, wValue, (WORD)bBitPos, eDev);
}


void msSWDBTrigger(MS_MLOAD_DEV eDev)
{
    BYTE bTrigSel;

    if(eDev == ML_DEV_1)
        bTrigSel = msReadByte(SC1F_3B) & 0x70;
    else
        bTrigSel = msReadByte(SC1F_4B) & 0x70;

    if(bTrigSel == 0x10)
        msMLoad_Fire(FALSE, eDev);
    else
        msMLoad_Fire(TRUE, eDev);
}


void msMLInfoInit(MS_MLOAD_DEV eDev)
{
    msMLoad_Reset(eDev);
}
#endif


#if ENABLE_DYNAMICSCALING
BOOL msDS_Done_Check(void)
{
    BOOL bEnable;
    WORD u16Status;
    
    u16Status = (msReadByte(SC1F_21) & 0x0A); //IPM and OPM

    if(u16Status)
        bEnable = FALSE;
    else
        bEnable = TRUE;

    return bEnable;
}


void msDS_Idx_Set(BYTE u8ForceIdx, BYTE u8MaxIdx)
{    
    msWrite2Byte(SC1F_EC, (u8ForceIdx << 8) | u8MaxIdx); // Idx_1 is dummy cmd
}


void msDS_Depth_Set(WORD u16depth)
{    
    msWriteByte(SC1F_26, u16depth); //IP
    msWriteByte(SC1F_90, (u16depth >> 8)); //IP ext
    msWrite2Byte(SC1F_BE, u16depth); //OP
}


BOOL msDS_BaseAddr_Set(DWORD u32addr, DWORD u32Offset)
{
    BOOL bRtn = FALSE;
    
    if((u32addr % MS_MLOAD_MEM_BASE_UNIT) || (u32Offset % MS_MLOAD_MEM_BASE_UNIT))
    {
        ML_printMsg("[FAIL] DS base addr is not align REG unit\n");
    }
    else
    {
        u32addr /= MS_MLOAD_MEM_BASE_UNIT;
        u32Offset /= MS_MLOAD_MEM_BASE_UNIT;

        //OPM
        msWrite4ByteMask(SC1F_1C, u32addr, 0x1FFFFFFF);
        
        //IPM
        msWrite4ByteMask(SC1F_22, (u32addr + u32Offset), 0x1FFFFFFF);

        bRtn = TRUE;
    }

    return bRtn;
}


void msDS_OP_Enable(BOOL bEnable)
{
    msWriteByteMask(SC1F_21, (bEnable << 1), BIT1);
}


void msDS_IP_Enable(BOOL bEnable)
{
    msWriteByteMask(SC1F_21, (bEnable << 3), BIT3);
}


BOOL msDS_Cmd_Add(MS_MLOAD_CMD u64Cmd, DS_IP_OP_SEL eIPOP)
{
    BOOL bRtn = FALSE;
    DWORD u32DRAMAddr;
    MS_MLOAD_CMD *pu64Addr;
    DWORD u32WPoint;
    DWORD u32Offset;

    ML_printData("msDS_Cmd_Add, DS_SEL = 0x%x\n", eIPOP);
    ML_printMsg("(WORD)Mask, (BYTE)spread mode, (WORD)Bank, (BYTE)addr, (WORD)data: ");
    ML_printData("%x", (u64Cmd.u8Mask_H << 8) | u64Cmd.u8Mask_L);
    ML_printData("%x", u64Cmd.u8SpreadMode);
    ML_printData("%x", (u64Cmd.u8Bank_H << 8) | u64Cmd.u8Bank_L);
    ML_printData("%x", u64Cmd.u8Addr);
    ML_printData("%x", (u64Cmd.u8Data_H << 8) | u64Cmd.u8Data_L);

    if(eIPOP == DS_OP) // OPM
    {
        u32Offset = 0;
        u32WPoint = stDSInfo.u32WPointOPM++;
    }
    else if(eIPOP == DS_IP) // IPM
    {
        u32Offset = stDSInfo.u32BaseAddrOffset;
        u32WPoint = stDSInfo.u32WPointIPM++;
    }
    else
    {
        ML_printData("[FAIL] DS_SEL ERROR = 0x%x\n", eIPOP);
        return bRtn;
    }

    if(stDSInfo.u32WPointAlign <= u32WPoint) // update u32WPointAlign to max(OPM,IPM)
        stDSInfo.u32WPointAlign++;    

    if(u32WPoint < (stDSInfo.u32MaxCmdCnt))
    {
        u32DRAMAddr = stDSInfo.PhyAddr + u32Offset + (u32WPoint *sizeof(MS_MLOAD_CMD));
        pu64Addr = (void *)(0x80000000 | (u32DRAMAddr));
        *pu64Addr = u64Cmd;

        bRtn = TRUE;

        ML_printData("pu64Addr = %04lx\n", (DWORD)pu64Addr);
    }
    else
    {
        ML_printData("[FAIL] Oversize, MaxCmdCnt = %lu\n", stDSInfo.u32MaxCmdCnt);
        ML_printData("[FAIL] u32WPointOPM = %lu\n", stDSInfo.u32WPointOPM);
        ML_printData("[FAIL] u32WPointIPM = %lu\n", stDSInfo.u32WPointIPM);
        ML_printData("[FAIL] u32WPointAlign = %lu\n", stDSInfo.u32WPointAlign);
    }

    return bRtn;
}


BOOL msDS_WriteCmd(DWORD u32Addr, WORD u16Data, WORD u16Mask, DS_IP_OP_SEL eIPOP)
{
    BOOL bRtn = FALSE;
    BOOL bByteAccess = TRUE;
    MS_MLOAD_CMD u64Cmd;
    MS_MLOAD_CMD u64SwitchCmd;
    WORD u16CurBank;
    BYTE u8CurSubBank;
    BYTE u8Data_H, u8Data_L;
    BYTE i;

    ML_printData("msDS_WriteCmd, DS_SEL = 0x%x\n", eIPOP);
    ML_printData("u16Mask: 0x%x\n", u16Mask);
    ML_printData("u32Addr_H: 0x%x\n", (WORD)(u32Addr >> 16));
    ML_printData("u32Addr_L: 0x%x\n", (WORD)u32Addr);
    ML_printData("u16Data: 0x%x\n", u16Data);

    // MenuLoad CMD Format (MSB --- LSB):
    // Mask         --> 16 bits [63:48]
    // Spread mode  --> [47]
    // Don't care   --> 7 bits [46:40]
    // Addr         --> 24 bits [39:16]	(bank + 16 bit addr)
    // value data   --> 16 bits [15:0]
    // once launch 64 bits data to RIU
    // i.e. 64bits command: 0xFF FF 80 1310 10 88 88
    // Mask:0xFFFF, Spread mode:0x80, Bank:1310(SC10), Addr:10, data:0x8888

    u16CurBank = (WORD)((u32Addr & 0x007FFF00) >> 8);
    u8CurSubBank = (BYTE)(u32Addr >> 24);

    if((u16Mask != 0x0000) && (u16CurBank & 0xFF00) && msDS_Done_Check())
    {
        u64Cmd.u8Mask_H = (BYTE)(u16Mask >> 8);
        u64Cmd.u8Mask_L = (BYTE)u16Mask;
        u64Cmd.u8SpreadMode = 0x80;
        u64Cmd.u8Addr = ((BYTE)u32Addr >> 1);

        for(i = 0; i < sizeof(SC00_WORD_ACCESS_REG); i++)
        {
            if(((u32Addr & 0x800000) && (u8CurSubBank == 0) && (u64Cmd.u8Addr == SC00_WORD_ACCESS_REG[i]))
                || ((u32Addr & 0x800000) && (u8CurSubBank != 0)) || ((u32Addr & 0x800000) == 0))
            {
                bByteAccess = FALSE;
                break;
            }
        }

        ML_printData("bByteAccess = %d\n", bByteAccess);

        //BYTE access REG lo byte set to hi byte: SC1F_17[1:0] = 2b'11 or 2b'10
        //BYTE access REG lo byte set to lo byte: SC1F_17[1:0] = 2b'01
        //SC1F_17[1:0] = 2b'10 or 2b'01 bit mask is not available.
        if(bByteAccess)
        {
            u64Cmd.u8Mask_L = u64Cmd.u8Mask_H; //For 8-bit access REG write hi byte mask
            u64Cmd.u8Bank_H = 0x13;
            u64Cmd.u8Bank_L = u8CurSubBank;
            u64Cmd.u8Data_H = 0;

            u8Data_H = (BYTE)(u16Data >> 8);

            if((u16Mask & 0x00FF) == 0x00FF)
                u8Data_L = (BYTE)u16Data;
            else
                u8Data_L = ((msReadByte(u32Addr) & ~((BYTE)u16Mask)) | ((BYTE)u16Data & (BYTE)u16Mask));

            //To reduce cmd count, check last cmd is BYTE or WORD access first.
            while(u16Mask)
            {
                if(stDSInfo.u8WByteEn[eIPOP] == 1)
                {
                    if(u16Mask & 0x00FF)
                    {
                        u64Cmd.u8Data_L = u8Data_L;
                        u16Mask &= 0xFF00;
                    }
                    else
                    {
                        stDSInfo.u8WByteEn[eIPOP] = 0;

                        //SC1F_17[7:0] = 0x0F
                        u64SwitchCmd.u8Mask_H = 0x00;
                        u64SwitchCmd.u8Mask_L = 0xFF;
                        u64SwitchCmd.u8SpreadMode = 0x80;
                        u64SwitchCmd.u8Bank_H = 0x13;
                        u64SwitchCmd.u8Bank_L = 0x1F;
                        u64SwitchCmd.u8Addr = 0x17;
                        u64SwitchCmd.u8Data_H = 0x00;
                        u64SwitchCmd.u8Data_L = 0x8F;

                        msDS_Cmd_Add(u64SwitchCmd, eIPOP);
                        u64Cmd.u8Data_L = u8Data_H;
                        u16Mask &= 0x00FF;
                    }
                }
                else
                {
                    if(u16Mask & 0xFF00)
                    {
                        u64Cmd.u8Data_L = u8Data_H;
                        u16Mask &= 0x00FF;
                    }
                    else
                    {
                        stDSInfo.u8WByteEn[eIPOP] = 1;

                        //SC1F_17[7:0] = 0x0D
                        u64SwitchCmd.u8Mask_H = 0x00;
                        u64SwitchCmd.u8Mask_L = 0xFF;
                        u64SwitchCmd.u8SpreadMode = 0x80;
                        u64SwitchCmd.u8Bank_H = 0x13;
                        u64SwitchCmd.u8Bank_L = 0x1F;
                        u64SwitchCmd.u8Addr = 0x17;
                        u64SwitchCmd.u8Data_H = 0x00;
                        u64SwitchCmd.u8Data_L = 0x8D;

                        msDS_Cmd_Add(u64SwitchCmd, eIPOP);
                        u64Cmd.u8Data_L = u8Data_L;
                        u16Mask &= 0xFF00;
                    }
                }
                
                msDS_Cmd_Add(u64Cmd, eIPOP);
            }
        }
        else
        {
            if(stDSInfo.u8WByteEn[eIPOP] == 1)
            {
                stDSInfo.u8WByteEn[eIPOP] = 0;

                //SC1F_17[7:0] = 0x0F
                u64SwitchCmd.u8Mask_H = 0x00;
                u64SwitchCmd.u8Mask_L = 0xFF;
                u64SwitchCmd.u8SpreadMode = 0x80;
                u64SwitchCmd.u8Bank_H = 0x13;
                u64SwitchCmd.u8Bank_L = 0x1F;
                u64SwitchCmd.u8Addr = 0x17;
                u64SwitchCmd.u8Data_H = 0x00;
                u64SwitchCmd.u8Data_L = 0x8F;

                msDS_Cmd_Add(u64SwitchCmd, eIPOP);
            }

            if(u32Addr & 0x800000)
            {
                u64Cmd.u8Bank_H = 0x13;
                u64Cmd.u8Bank_L = u8CurSubBank;
            }
            else
            {
                u64Cmd.u8Bank_H = (BYTE)(u16CurBank >> 8);
                u64Cmd.u8Bank_L = (BYTE)u16CurBank;
            }

            u64Cmd.u8Data_H = (BYTE)(u16Data >> 8);
            u64Cmd.u8Data_L = (BYTE)u16Data;

            msDS_Cmd_Add(u64Cmd, eIPOP);
        }
        
        bRtn = TRUE;
    }
    else
    {
        ML_printMsg("[FAIL] DS Write Cmd\n");

        if((u16CurBank & 0xFF00) == 0)
        {
            ML_printMsg("DS Cannot Write PM Domain REG\n");
        }
    }

    return bRtn;
}


void msDS_Reset(void)
{
    BOOL bDone = TRUE;
    MS_MLOAD_CMD u64Cmd;
    
    msDS_OP_Enable(FALSE);
    msDS_IP_Enable(FALSE);
    msDS_Depth_Set(0x0000);
    msDS_Idx_Set(0x01, 0x01);


#if ENABLE_MENULOAD
    bDone = msMLoad_Check_Done(ML_DEV_1);
#endif

#if ENABLE_MENULOAD_2
    bDone &= msMLoad_Check_Done(ML_DEV_2);
#endif

    if(bDone)
    {
        //sw rst, to avoid DS/MLoad engine abnormal after last time being triggered error
        msWrite2ByteMask(SC1F_26, 0x1C00, 0x1C00);
        msWrite2ByteMask(SC1F_26, 0x0000, 0x1C00);
    }

    stDSInfo.u32WPointOPM = 0;
    stDSInfo.u32WPointIPM = 0;
    stDSInfo.u32WPointAlign = 0;
    stDSInfo.u8WByteEn[DS_OP] = 0;  
    stDSInfo.u8WByteEn[DS_IP] = 0;    

    //MLoad/DS WORD access and bit mask enable, SC1F_17[7:0] = 0x0F
    u64Cmd.u8Mask_H = 0x00;
    u64Cmd.u8Mask_L = 0xFF;
    u64Cmd.u8SpreadMode = 0x80;
    u64Cmd.u8Bank_H = 0x13;
    u64Cmd.u8Bank_L = 0x1F;
    u64Cmd.u8Addr = 0x17;
    u64Cmd.u8Data_H = 0x00;
    u64Cmd.u8Data_L = 0x8F;

    msDS_Cmd_Add(u64Cmd, DS_OP);
    msDS_Cmd_Add(u64Cmd, DS_IP);
}


BOOL msDS_Trigger(DWORD u32BaseAddr, WORD u16CmdCnt)
{
    BOOL bRtn;
    DWORD u32Offset = stDSInfo.u32BaseAddrOffset;
    
    ML_printMsg("msDS_Trigger\n");
    ML_printData("u32BaseAddr = 0x%lx\n", u32BaseAddr);
    ML_printData("u16CmdCnt = %u\n\n", u16CmdCnt);

    bRtn = msDS_BaseAddr_Set(u32BaseAddr, u32Offset);

    if(bRtn)
    {
        msDS_OP_Enable(TRUE);
        msDS_IP_Enable(TRUE);
        msDS_Depth_Set(u16CmdCnt); //Enable IP/OP DS by setting depth not as zero
        msDS_Idx_Set(0x00, 0x00);
    }

    return bRtn;
}


BOOL msDS_KickOff(void)
{
    BOOL bRtn = FALSE;
    BYTE u8DSSelIdx;
    WORD u16Index;
    DWORD u32WPointOP;
    DWORD u32WPointIP;
    DWORD u32WPointAlign;
    MS_MLOAD_CMD u64Cmd;

    ML_printMsg("msDS_KickOff\n");

    for(u8DSSelIdx = 0; u8DSSelIdx < (BYTE)DS_SEL_NUM; u8DSSelIdx++)
    {
        if(stDSInfo.u8WByteEn[u8DSSelIdx] == 1)
        {
            stDSInfo.u8WByteEn[u8DSSelIdx] = 0;

            //SC1F_17[7:0] = 0x0F, enable WORD access for end cmd and bit mask is available
            u64Cmd.u8Mask_H = 0x00;
            u64Cmd.u8Mask_L = 0xFF;
            u64Cmd.u8SpreadMode = 0x80;
            u64Cmd.u8Bank_H = 0x13;
            u64Cmd.u8Bank_L = 0x1F;
            u64Cmd.u8Addr = 0x17;
            u64Cmd.u8Data_H = 0x00;
            u64Cmd.u8Data_L = 0x8F;

            msDS_Cmd_Add(u64Cmd, u8DSSelIdx);
        }
    }

    //OP end cmd
    u64Cmd.u8Mask_H = 0x02;
    u64Cmd.u8Mask_L = 0x00;
    u64Cmd.u8SpreadMode = 0x80;
    u64Cmd.u8Bank_H = 0x13;
    u64Cmd.u8Bank_L = 0x1F;
    u64Cmd.u8Addr = 0x10;
    u64Cmd.u8Data_H = 0x00;
    u64Cmd.u8Data_L = 0x00;
    
    msDS_Cmd_Add(u64Cmd, DS_OP);

    //IP end cmd
    u64Cmd.u8Mask_H = 0x08;
    u64Cmd.u8Mask_L = 0x00;
    u64Cmd.u8SpreadMode = 0x80;
    u64Cmd.u8Bank_H = 0x13;
    u64Cmd.u8Bank_L = 0x1F;
    u64Cmd.u8Addr = 0x10;
    u64Cmd.u8Data_H = 0x00;
    u64Cmd.u8Data_L = 0x00;

    msDS_Cmd_Add(u64Cmd, DS_IP);

    u32WPointAlign = stDSInfo.u32WPointAlign;

    // Fill Dummy Cmd for Idx 0/1
    while(stDSInfo.u32WPointAlign % MS_MLOAD_CMD_ALIGN != 0)
        u32WPointAlign = ++stDSInfo.u32WPointAlign;

    ML_printData("stDSInfo.u32WPointAlign = 0x%x\n", stDSInfo.u32WPointAlign);

    u32WPointOP = stDSInfo.u32WPointOPM;
    u32WPointIP = stDSInfo.u32WPointIPM;

    if(((stDSInfo.u32WPointAlign / MS_MLOAD_CMD_ALIGN) > 0xFFFF) || (stDSInfo.u32WPointAlign > stDSInfo.u32MaxCmdCnt)) //max of one idx cmd depth is 0xFFFF
    {
        ML_printData("[FAIL] Command Too Many = %lu\n", stDSInfo.u32WPointAlign);
    }
    else
    {
        u64Cmd.u8Mask_H = 0xFF;
        u64Cmd.u8Mask_L = 0xFF;
        u64Cmd.u8SpreadMode = 0x80;
        u64Cmd.u8Bank_H = 0x13;
        u64Cmd.u8Bank_L = 0x1F;
        u64Cmd.u8Addr = 0x7F;
        u64Cmd.u8Data_H = 0x00;
        u64Cmd.u8Data_L = 0x00;

        // Add OP Null
        for(u16Index = 0; u16Index < (u32WPointAlign *stDSInfo.u8IndexNum - u32WPointOP); u16Index++)
            bRtn = msDS_Cmd_Add(u64Cmd, DS_OP);

        // Add IP Null        
        for(u16Index = 0; u16Index < (u32WPointAlign *stDSInfo.u8IndexNum - u32WPointIP); u16Index++)
            bRtn = msDS_Cmd_Add(u64Cmd, DS_IP);
    }
    
    MsOS_FlushMemory();

    if(bRtn)
    {
        bRtn = msDS_Trigger(stDSInfo.PhyAddr, (WORD)(u32WPointAlign /MS_MLOAD_CMD_ALIGN));
    }

    return bRtn;
}


BOOL msDS_Fire(BOOL bImmediate)
{
    BOOL bRtn;
    DWORD u32Delayms = 0;

    ML_printMsg("msDS_Fire\n");
    ML_printData("u32WPointOPM = %u\n", stDSInfo.u32WPointOPM);
    ML_printData("u32WPointIPM = %u\n", stDSInfo.u32WPointIPM);
    ML_printData("u32WPointAlign = %u\n", stDSInfo.u32WPointAlign);

    bRtn = msDS_KickOff();

    if(bImmediate)
    {
        while((msDS_Done_Check() == FALSE) && (u32Delayms < 600))
        {
            Delay1ms(1);
            u32Delayms++;
        }

        if(bRtn == FALSE)
        {
            ML_printMsg("[FAIL] DS Fail to Trigger\n");
        }
        else if(msDS_Done_Check() == FALSE)
        {
            ML_printMsg("[FAIL] DS TimeOut\n");
            bRtn = FALSE;
        }
        else
        {
            ML_printMsg("DS Done!!!\n");
            bRtn = TRUE;
        }

        msDS_Reset();
    }

    return bRtn;
}


void msDS_Config(void)
{
    if(SyncLossState() || (!InputTimingStableFlag)
        || (SC0_READ_INPUT_SETTING() & BIT7))
    {
        u8DSFlag = 0;
        ML_printMsg("In/Out Timing is not ready, cannot do Dynamic Scaling\n");
    }
    else
    {
        msDS_Reset();
    }
}


void msDS_Init(void)
{
    DWORD u32BufByteLen;
    
    msDS_Idx_Set(0x01, 0x01);
    msWriteByteMask(SC1F_BD, 0x80, 0x80); // IP/OP idx depth will be set in different REG
    msDS_Depth_Set(0x0000);
    
    u32BufByteLen = DS_SIZE /2; // no sub scaler, share w/ OPM/IPM

    stDSInfo.PhyAddr = DS_START_ADDR;
    stDSInfo.u32BaseAddrOffset = u32BufByteLen; 
    stDSInfo.u8IndexNum = 2; // one index for dummy cmd    
    stDSInfo.u32WPointOPM = 0;
    stDSInfo.u32WPointIPM = 0;    
    stDSInfo.u32WPointAlign = 0;
    stDSInfo.u32MaxCmdCnt = (DWORD)(u32BufByteLen / stDSInfo.u8IndexNum / sizeof(MS_MLOAD_CMD)); //one cmd is 64-bit
    stDSInfo.u8WByteEn[DS_IP] = 0;
    stDSInfo.u8WByteEn[DS_OP] = 0;

    msWriteByteMask(SC1F_2E, 0x80, 0x80); //enable different base address for IPM/IPS/OP
    msWriteByteMask(SC1F_27, 0x00, 0x02);
    msWriteByteMask(SC1F_28, 0x00, 0x02);
    msWriteByte(SC1F_EE, 0x05); // init idx mode as force mode    
    msWriteByteMask(SC1F_20, MS_DS_REQ_THD, 0x0F);
    msWriteByteMask(SC1F_20, (MS_DS_REQ_LEN << 4), 0xF0);

    ML_printData("DS base Addr = 0x%x\n", stDSInfo.PhyAddr);
    ML_printData("DS addr offset = 0x%x\n", stDSInfo.u32BaseAddrOffset);
}


void msDSWriteByte(DWORD dwReg, BYTE ucVal, DS_IP_OP_SEL eIPOP)
{
    if (dwReg & BIT0) //odd addr
        msDS_WriteCmd(dwReg, (WORD)(ucVal << 8), 0xFF00, eIPOP);
    else //even addr
        msDS_WriteCmd(dwReg, (WORD)ucVal, 0x00FF, eIPOP);
}


void msDSWrite2Byte(DWORD dwReg, WORD wVal, DS_IP_OP_SEL eIPOP)
{
    if (dwReg & BIT0) //odd addr
        {
            msDS_WriteCmd(dwReg, (WORD)(wVal << 8), 0xFF00, eIPOP);
            msDS_WriteCmd(dwReg + 1, (WORD)(wVal >> 8), 0x00FF, eIPOP);
        }
        else //even addr
            msDS_WriteCmd(dwReg, wVal, 0xFFFF, eIPOP);
}


void msDSWrite3Byte(DWORD dwReg, DWORD dwValue, DS_IP_OP_SEL eIPOP)
{
    if(dwReg & BIT0)//odd addr
    {
        msDS_WriteCmd(dwReg, (WORD)(dwValue << 8), 0xFF00, eIPOP);
        msDS_WriteCmd(dwReg + 1, (WORD)(dwValue >> 8), 0xFFFF, eIPOP);
    }
    else //even addr
    {
        msDS_WriteCmd(dwReg, (WORD)dwValue, 0xFFFF, eIPOP);
        msDS_WriteCmd(dwReg + 2, (WORD)(dwValue >> 16), 0x00FF, eIPOP);
    }
}


void msDSWrite4Byte(DWORD dwReg, DWORD dwValue, DS_IP_OP_SEL eIPOP)
{
    if(dwReg & BIT0) //odd addr
    {
        msDS_WriteCmd(dwReg, (WORD)(dwValue << 8), 0xFF00, eIPOP);
        msDS_WriteCmd(dwReg + 1, (WORD)(dwValue >> 8), 0xFFFF, eIPOP);
        msDS_WriteCmd(dwReg + 3, (WORD)(dwValue >> 24), 0x00FF, eIPOP);
    }
    else //even addr
    {
        msDS_WriteCmd(dwReg, (WORD)dwValue, 0xFFFF, eIPOP);
        msDS_WriteCmd(dwReg + 2, (WORD)(dwValue >> 16), 0xFFFF, eIPOP);
    }
}


void msDSWriteByteMask(DWORD dwReg, BYTE ucVal, BYTE ucMask, DS_IP_OP_SEL eIPOP)
{
    if (dwReg & BIT0) //odd addr
        msDS_WriteCmd(dwReg, (WORD)(ucVal << 8), (WORD)(ucMask << 8), eIPOP);
    else //even addr
        msDS_WriteCmd(dwReg, (WORD)ucVal, (WORD)(ucMask), eIPOP);
}


void msDSWrite2ByteMask(DWORD dwReg, WORD wVal, WORD wMask, DS_IP_OP_SEL eIPOP)
{
    if (dwReg & BIT0) //odd addr
    {
        msDS_WriteCmd(dwReg, (WORD)(wVal << 8), (WORD)(wMask << 8), eIPOP);
        msDS_WriteCmd(dwReg + 1, (WORD)(wVal >> 8), (WORD)(wMask >> 8), eIPOP);
    }
    else //even addr
    {
        msDS_WriteCmd(dwReg, wVal, wMask, eIPOP);
    }
}


void msDSWriteBit(DWORD dwReg, BOOL bBit, BYTE ucBitPos, DS_IP_OP_SEL eIPOP)
{
    WORD wVal;

    if(bBit)
        wVal = (WORD)ucBitPos;
    else
        wVal = 0x0000;

    if (dwReg & BIT0) //odd addr
        msDS_WriteCmd(dwReg, (WORD)(wVal << 8), (WORD)ucBitPos << 8, eIPOP);
    else //even addr
        msDS_WriteCmd(dwReg, wVal, (WORD)ucBitPos, eIPOP);
}
#endif

