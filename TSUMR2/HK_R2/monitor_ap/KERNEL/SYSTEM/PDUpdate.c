#define _PDUPDATE_C_

#include "types.h"
#include "board.h"
#include "Common.h"
#include "misc.h"
#include "Global.h"
#include "ms_reg.h"
#include "Ms_rwreg.h"
#include "hwi2c.h"
#include "PDUpdate.h"

#define PDUPDATE_DEBUG    0
#if ENABLE_MSTV_UART_DEBUG && PDUPDATE_DEBUG
    #define PDUPDATE_PRINT(str, value)      printData(str, value)
    #define PDUPDATE_printMsg(str)          printMsg(str)
#else
    #define PDUPDATE_PRINT(str, value)
    #define PDUPDATE_printMsg(str)
#endif

#if ENABLE_USB_TYPEC
#define xfr_FlashCode ((unsigned char  volatile *) 0x00000000)
#define DELAY_FOR_BUS_FREE()      PDI2C_DELAY(10)
#define DELAY_FOR_REPEAT_START()  PDI2C_DELAY(10)

BYTE XDATA u8PDSA=0;

void PDI2C_DELAY(WORD us)
{
    WORD delayUs;

    delayUs = us *10;
    while(delayUs--)
    {
        _nop_();
    }
}

BOOL msDrvPD_IICRead(BYTE u8SlaveAddr, BYTE *pu8CMD, BYTE u8CMDLen, BYTE *pu8Buf, WORD u16Buflen)
{
    BOOL result = TRUE;
    BOOL done = FALSE;
    WORD i;
    BYTE res, retry = 10;

    while(retry--)
    {
        // master start
        if(hw_i2c_start(USE_MIIC) != 0)
        {
            hw_i2c_stop(USE_MIIC);
            DELAY_FOR_BUS_FREE();
            continue;
        }

        if(hw_i2c_send_byte(USE_MIIC, u8SlaveAddr) != 0)
        {
            hw_i2c_stop(USE_MIIC);
            DELAY_FOR_BUS_FREE();
            continue;
        }

        // write address
        for(i=0;i<u8CMDLen;i++)
        {
            res = hw_i2c_send_byte(USE_MIIC, pu8CMD[i]);
            if( res != 0)
            {
                res = hw_i2c_stop(USE_MIIC);
                DELAY_FOR_BUS_FREE();
                continue;
            }
        }

        //
        if(hw_i2c_start(USE_MIIC) != 0)
        {
            hw_i2c_stop(USE_MIIC);
            DELAY_FOR_BUS_FREE();
            continue;
        }

        if(hw_i2c_send_byte(USE_MIIC, u8SlaveAddr|0x01) != 0)
        {
            hw_i2c_stop(USE_MIIC);
            DELAY_FOR_BUS_FREE();
            continue;
        }

        for (i=0; i<u16Buflen; i++) // loop to burst read
        {
            // when last read with burst, tx report NACK
            if(i == (u16Buflen-1) )
            {
                res = hw_i2c_receive_byte(USE_MIIC, 0, &pu8Buf[i]);
                if( res != 0)
                {
                    result = FALSE;
                }
            }
            else
            {
                res = hw_i2c_receive_byte(USE_MIIC, 1, &pu8Buf[i]);
                if( res != 0)
                {
                    result = FALSE;
                }
            }

        } // while
        done = TRUE;
        break;
    }

    hw_i2c_stop(USE_MIIC);
    DELAY_FOR_BUS_FREE();

    if(done == FALSE || result == FALSE)
    {
        result = FALSE;
        PDUPDATE_printMsg("\n!! msDrvPD_IICRead fail!! \n");
    }

    return result;
}

BOOL msDrvPD_IICWrite(BYTE u8SlaveAddr,  BYTE *pu8Buf, WORD u16Buflen, BOOL bSkipDataAck)
{
    BOOL  result = TRUE;
    BOOL  done = FALSE;
    WORD  i;
    BYTE  res, retry = 10;

    while (retry--)
    {
        hw_i2c_start(USE_MIIC);

        res = hw_i2c_send_byte(USE_MIIC, u8SlaveAddr);
        if( res != 0)
        {
            hw_i2c_stop(USE_MIIC);
            DELAY_FOR_BUS_FREE();
            continue;
        }

        for(i=0; i<u16Buflen; i++) // loop of writting data
        {
            res = hw_i2c_send_byte(USE_MIIC, pu8Buf[i]); //  == EMIICACK)
            if(bSkipDataAck && (res==EMIICACK))
                res = 0;

            if( res != 0)
            {
                result = FALSE;
                break;
            }
        }

        if(result == FALSE)
        {
            result = TRUE;
            DELAY_FOR_REPEAT_START();
            hw_i2c_stop(USE_MIIC);
            continue;
        }

        done = TRUE;
        break;
    }

    hw_i2c_stop(USE_MIIC);

    DELAY_FOR_BUS_FREE();

    if(result== FALSE || done == FALSE )
    {
        result = FALSE;
        PDUPDATE_printMsg("\n!! msDrvPD_IICWrite fail!!\n");
    }

    return result;
}


BOOL msDrvPD_ByteWriteMTP(WORD u16MTPAddr)
{
    BYTE u8Cmd[3];
    u8Cmd[0] = 0xF1;
    u8Cmd[1] = (BYTE)(((((unsigned char *)&u16MTPAddr)[1])>>2)|((u16MTPAddr&0x03)<<4));
    u8Cmd[2] = (BYTE)(u16MTPAddr>>2);
    return (msDrvPD_IICWrite(u8PDSA, u8Cmd, 3, FALSE)==TRUE)?0:1;
}

BOOL msDrvPD_LoadWriteBuffer(BYTE u8Data0, BYTE u8Data1, BYTE u8Data2, BYTE u8Data3)
{
    BYTE u8Cmd[5];
    u8Cmd[0] = 0xF2;
    u8Cmd[1] = u8Data0;
    u8Cmd[2] = u8Data1;
    u8Cmd[3] = u8Data2;
    u8Cmd[4] = u8Data3;
    return (msDrvPD_IICWrite(u8PDSA, u8Cmd, 5, FALSE)==TRUE)?0:1;
}

BOOL msDrvPD_ReadMTPStatus(BYTE *pu8MTPStatus)
{
    BYTE u8Cmd[1];
    u8Cmd[0] = 0xF3;
    return (msDrvPD_IICRead(u8PDSA, u8Cmd, 1, pu8MTPStatus, 1)==TRUE)?0:1;
}

BOOL msDrvPD_ReadMTPData(WORD u16MTPAddr, BYTE *pu8ReadData, WORD u16ReadLength)
{
    BYTE u8Cmd[3];
    u8Cmd[0] = 0xF4;
    u8Cmd[1] = (BYTE)(((((unsigned char *)&u16MTPAddr)[1])>>2)|((u16MTPAddr&0x03)<<4));
    u8Cmd[2] = (BYTE)(u16MTPAddr>>2);
    return (msDrvPD_IICRead(u8PDSA, u8Cmd, 3, pu8ReadData, u16ReadLength)==TRUE)?0:1;
}

BOOL msDrvPD_ConfigureMTPArea(WORD u16MTPAddrStart, WORD u16MTPAddrEnd)
{
    BYTE u8Cmd[5];
    u8Cmd[0] = 0xF7;
    u8Cmd[1] = (BYTE)(((((unsigned char *)&u16MTPAddrStart)[1])>>2)|((u16MTPAddrStart&0x03)<<4));
    u8Cmd[2] = (BYTE)(u16MTPAddrStart>>2);
    u8Cmd[3] = (BYTE)(((((unsigned char *)&u16MTPAddrEnd)[1])>>2)|((u16MTPAddrEnd&0x03)<<4));
    u8Cmd[4] = (BYTE)(u16MTPAddrEnd>>2);
    return (msDrvPD_IICWrite(u8PDSA, u8Cmd, 5, FALSE)==TRUE)?0:1;
}

BOOL msDrvPD_Reset(void)
{
    BOOL rtn;
    BYTE u8Cmd[2];
    u8Cmd[0] = 0x78;
    u8Cmd[1] = 0x87;

    rtn = (msDrvPD_IICWrite(u8PDSA, u8Cmd, 2, TRUE)==TRUE)?0:1;
    u8PDSA = 0;

    return rtn;
}

BOOL msDrvPD_CheckSlaveAddress(BYTE u8PDSATest)
{
    return (msDrvPD_IICWrite(u8PDSATest, 0, 0, FALSE)==TRUE)?0:1;
}

// Initialization Before PD F/W Update
void msAPI_PDUpdateInit(void)
{
    // Stops PD51
    msWriteByte(REG_001B00, 0x01); // [0]:1:reset PD51

    // HWIIC initialization
    hw_i2c_intial(USE_MIIC);
}

#define PDCodeLength    0x4000
#define PDChkSumLength  0x3FF0
#define PDChkSumGolden  0x3FF8
#define PDCRCGolden     0x3FFC
#define PDVerifyUnit    0x40 // < u8VerifyIdx, a factor of PDCodeLength
// u16AddInFlash: Address of RT7880 FW in flash
// u8PDSA_MCU: PD slave address of MCU (F/W slave address)
// u8PDSA_PCB: PD slave address of PCB (if no F/W, slave address devided by PCB)
//// u16Length: Supposed to be 16KB(16384B)
// rtn: ePDUGSTS
ePDUGSTS msAPI_PDUpdateFW(DWORD u32AddInFlash, BYTE u8PDSA_MCU, BYTE u8PDSA_PCB)
{
    BYTE u8MTPSts, u8Rtn=0, u8VerifyIdx;
    BYTE u8MTPReadData[PDVerifyUnit];
    WORD u16Idx;
    DWORD u32ChkSum=0;

    ///////////////////////////////////////////////////////////
    // Check Checksum (0x0000~0x3FEF Byte Sum == {0x3FFB~0x3FF8})
    ///////////////////////////////////////////////////////////
    for(u16Idx=0 ; u16Idx<PDChkSumLength ; u16Idx++)
    {
        u32ChkSum += xfr_FlashCode[u32AddInFlash+u16Idx];
    }
    if(  (((unsigned char *)&u32ChkSum)[3] == xfr_FlashCode[u32AddInFlash+PDChkSumGolden+3])
       &&(((unsigned char *)&u32ChkSum)[2] == xfr_FlashCode[u32AddInFlash+PDChkSumGolden+2])
       &&(((unsigned char *)&u32ChkSum)[1] == xfr_FlashCode[u32AddInFlash+PDChkSumGolden+1])
       &&(((unsigned char *)&u32ChkSum)[0] == xfr_FlashCode[u32AddInFlash+PDChkSumGolden+0]) )
    {
        PDUPDATE_PRINT("[PDUPDATE] GOLDN_HW:%x",((WORD)xfr_FlashCode[u32AddInFlash+PDChkSumGolden+3]<<8)|xfr_FlashCode[u32AddInFlash+PDChkSumGolden+2]);
        PDUPDATE_PRINT("[PDUPDATE] GOLDN_LW:%x",((WORD)xfr_FlashCode[u32AddInFlash+PDChkSumGolden+1]<<8)|xfr_FlashCode[u32AddInFlash+PDChkSumGolden+0]);
        PDUPDATE_printMsg("[PDUPDATE] CHKSUM Pass !");
    }
    else
    {
        PDUPDATE_PRINT("[PDUPDATE] CHSUM_HW:%x",u32ChkSum>>16);
        PDUPDATE_PRINT("[PDUPDATE] CHSUM_LW:%x",u32ChkSum&0x0000FFFF);
        PDUPDATE_PRINT("[PDUPDATE] GOLDN_HW:%x",((WORD)xfr_FlashCode[u32AddInFlash+PDChkSumGolden+3]<<8)|xfr_FlashCode[u32AddInFlash+PDChkSumGolden+2]);
        PDUPDATE_PRINT("[PDUPDATE] GOLDN_LW:%x",((WORD)xfr_FlashCode[u32AddInFlash+PDChkSumGolden+1]<<8)|xfr_FlashCode[u32AddInFlash+PDChkSumGolden+0]);
        return ePDUGSTS_ER_CHKSUM;
    }
    ///////////////////////////////////////////////////////////
    // Check Slave Address
    ///////////////////////////////////////////////////////////
    if(!u8PDSA)
    {
        u8Rtn = msDrvPD_CheckSlaveAddress(u8PDSA_MCU);
        if(!u8Rtn)
        {
            u8PDSA = u8PDSA_MCU;
            PDUPDATE_PRINT("[PDUPDATE] SA_MCU:%x",u8PDSA);
        }
        else
        {
            u8Rtn = msDrvPD_CheckSlaveAddress(u8PDSA_PCB);
            if(!u8Rtn)
            {
                u8PDSA = u8PDSA_PCB;
                PDUPDATE_PRINT("[PDUPDATE] SA_PCB:%x",u8PDSA);
            }
            else
            {
                PDUPDATE_PRINT("[PDUPDATE] SA_MCU:%x",u8PDSA_MCU);
                PDUPDATE_PRINT("[PDUPDATE] SA_PCB:%x",u8PDSA_PCB);
                PDUPDATE_printMsg("[PDUPDATE] Both SA Failed !");
                return ePDUGSTS_ER_SLAVEADR;
            }
        }
    }
    ///////////////////////////////////////////////////////////
    // Enter Online Update Mode
    ///////////////////////////////////////////////////////////
    if((u8PDSA&0x1E) != 0x1E)
    {
        u8Rtn |= msDrvPD_ConfigureMTPArea(0x0000, PDCodeLength-1);
        if(u8Rtn)
        {
            PDUPDATE_printMsg("[PDUPDATE] CfgMTPArea IIC Failed !");
            return ePDUGSTS_ER_IIC;
        }
        else
        {
            u8PDSA = u8PDSA_PCB | 0x1E;

            u8Rtn = msDrvPD_CheckSlaveAddress(u8PDSA);
            if(!u8Rtn)
            {
                PDUPDATE_PRINT("[PDUPDATE] OLSA_UPDATE:%x",u8PDSA);
            }
            else
            {
                u8Rtn = msDrvPD_CheckSlaveAddress(PDSA_OLM);
                if(!u8Rtn)
                {
                    u8PDSA = PDSA_OLM;
                    PDUPDATE_PRINT("[PDUPDATE] OLSA_UPDATE:%x",PDSA_OLM);
                }
                else
                {
                    PDUPDATE_PRINT("[PDUPDATE] OLSA_PCB:%x",u8PDSA_PCB | 0x1E);
                    PDUPDATE_PRINT("[PDUPDATE] OLSA_PCB:%x",PDSA_OLM);
                    PDUPDATE_printMsg("[PDUPDATE] Both OLSA Failed !");
                    return ePDUGSTS_ER_SLAVEADR;
                }
            }
        }
    }
    ///////////////////////////////////////////////////////////
    // Programming Flow
    ///////////////////////////////////////////////////////////
    u16Idx=0;
    while(u16Idx<PDCodeLength)
    {
        // Load_Write_Buffer - 4B
        if((u16Idx%4)==0)
        {
            u8Rtn |= msDrvPD_LoadWriteBuffer(xfr_FlashCode[u32AddInFlash+u16Idx+0], xfr_FlashCode[u32AddInFlash+u16Idx+1], xfr_FlashCode[u32AddInFlash+u16Idx+2], xfr_FlashCode[u32AddInFlash+u16Idx+3]);
        }

        // Byte_Write_MTP - 1B
        u8Rtn |= msDrvPD_ByteWriteMTP(u16Idx);

        // Check Busy Flag==0 in 21ms
        SetTimOutConter(21);
        do
        {
            u8Rtn |= msDrvPD_ReadMTPStatus(&u8MTPSts);
        }while( (u8MTPSts&BIT7) && (!u8Rtn) && (bTimeOutCounterFlag));

        if(!bTimeOutCounterFlag)
        {
            PDUPDATE_printMsg("[PDUPDATE] ReadMTPSts Timeout 21ms !");
            return ePDUGSTS_ER_PROGTIMEOUT;
        }
        else if(!u8Rtn)
        {
            // Check Fail Flag
            if(u8MTPSts&BIT6)
            {
                PDUPDATE_printMsg("[PDUPDATE] ProgMTP Fail !");
                return ePDUGSTS_ER_PROGFAIL;
            }
        }

        // Check IIC Fail
        if(u8Rtn)
        {
            PDUPDATE_printMsg("[PDUPDATE] IIC Fail !");
            return ePDUGSTS_ER_IIC;
        }

        u16Idx++;
    }

    ///////////////////////////////////////////////////////////
    // Verifying Flow
    ///////////////////////////////////////////////////////////
    u16Idx=0;
    while(u16Idx<PDCodeLength)
    {
        // Read_MTP_Data - PDVerifyUnit
        u8Rtn |= msDrvPD_ReadMTPData(u16Idx, u8MTPReadData, PDVerifyUnit);

        // Compare With Flash Data
        if(!u8Rtn)
        {
            for(u8VerifyIdx=0 ; u8VerifyIdx<PDVerifyUnit ; u8VerifyIdx++ )
            {
                if(u8MTPReadData[u8VerifyIdx] != xfr_FlashCode[u32AddInFlash+u16Idx+u8VerifyIdx])
                {
                    PDUPDATE_PRINT("[PDUPDATE] Verify Fail Adr:%x",u16Idx+u8VerifyIdx);
                    PDUPDATE_PRINT("[PDUPDATE] MTP Data:%x",u8MTPReadData[u8VerifyIdx]);
                    PDUPDATE_PRINT("[PDUPDATE] Golden:%x",xfr_FlashCode[u32AddInFlash+u16Idx+u8VerifyIdx]);
                    return ePDUGSTS_ER_VERIFY;
                }
            }
        }
        else
        {
            PDUPDATE_printMsg("[PDUPDATE] Verify IIC Fail !");
            return ePDUGSTS_ER_IIC;
        }

        u16Idx += PDVerifyUnit;
    }

    // TCPC Reset
    u8Rtn |= msDrvPD_Reset();
    if(u8Rtn)
    {
        PDUPDATE_printMsg("[PDUPDATE] Reset IIC Fail !");
        return ePDUGSTS_ER_IIC;
    }

    PDUPDATE_printMsg("[PDUPDATE] Upgrade Pass !");
    return ePDUGSTS_OK;
}

// Initialization Before PD F/W Update
ePDUGSTS msAPI_PDUpdateAuto(DWORD u32AddInFlash, DWORD u32Length, BYTE u8PDSA_MCU, BYTE u8PDSA_PCB)
{
    BYTE u8Rtn;
    BYTE u8MTPCRC;

    if(u32Length < 0x24000) // TCPM: 0x20000, TCPM:0x4000.
    {
        PDUPDATE_printMsg("[PDUPDATE] TCPC Code Size Error!");
        return ePDUGSTS_ER_TCPCSIZE;
    }
    u32AddInFlash += 0x20000; // offset to TCPC FW
    u32AddInFlash -= msRead4Byte(REG_002D3C);//R2 spi offset

    ///////////////////////////////////////////////////////////
    // HWIIC init
    ///////////////////////////////////////////////////////////
    msAPI_PDUpdateInit();

    ///////////////////////////////////////////////////////////
    // Check Slave Address
    ///////////////////////////////////////////////////////////
    u8Rtn = msDrvPD_CheckSlaveAddress(u8PDSA_MCU);
    if(!u8Rtn)
    {
        u8PDSA = u8PDSA_MCU;
        PDUPDATE_PRINT("[PDUPDATE] SA_MCU:%x",u8PDSA);
    }
    else
    {
        u8Rtn = msDrvPD_CheckSlaveAddress(u8PDSA_PCB);
        if(!u8Rtn)
        {
            u8PDSA = u8PDSA_PCB;
            PDUPDATE_PRINT("[PDUPDATE] SA_PCB:%x",u8PDSA);
        }
        else
        {
            PDUPDATE_PRINT("[PDUPDATE] SA_MCU:%x",u8PDSA_MCU);
            PDUPDATE_PRINT("[PDUPDATE] SA_PCB:%x",u8PDSA_PCB);
            PDUPDATE_printMsg("[PDUPDATE] Both SA Failed !");
            return ePDUGSTS_ER_SLAVEADR;
        }
    }

    ///////////////////////////////////////////////////////////
    // Enter Online Update Mode
    ///////////////////////////////////////////////////////////
    u8Rtn |= msDrvPD_ConfigureMTPArea(0x0000, PDCodeLength-1);
    if(u8Rtn)
    {
        PDUPDATE_printMsg("[PDUPDATE] CfgMTPArea IIC Failed !");
        return ePDUGSTS_ER_IIC;
    }
    else
    {
        u8PDSA = u8PDSA_PCB | 0x1E;

        u8Rtn = msDrvPD_CheckSlaveAddress(u8PDSA);
        if(!u8Rtn)
        {
            PDUPDATE_PRINT("[PDUPDATE] OLSA_UPDATE:%x",u8PDSA);
        }
        else
        {
            u8Rtn = msDrvPD_CheckSlaveAddress(PDSA_OLM);
            if(!u8Rtn)
            {
                u8PDSA = PDSA_OLM;
                PDUPDATE_PRINT("[PDUPDATE] OLSA_UPDATE:%x",PDSA_OLM);
            }
            else
            {
                PDUPDATE_PRINT("[PDUPDATE] OLSA_PCB:%x",u8PDSA_PCB | 0x1E);
                PDUPDATE_PRINT("[PDUPDATE] OLSA_PCB:%x",PDSA_OLM);
                PDUPDATE_printMsg("[PDUPDATE] Both OLSA Failed !");
                return ePDUGSTS_ER_SLAVEADR;
            }
        }
    }

    // Check CRC match or not.
    u8Rtn |= msDrvPD_ReadMTPData(PDCRCGolden, &u8MTPCRC, 1);
    if(!u8Rtn)
    {
        if(u8MTPCRC != xfr_FlashCode[u32AddInFlash+PDCRCGolden])
        {
            PDUPDATE_PRINT("[PDUPDATE] CRC in Flash:%x",xfr_FlashCode[u32AddInFlash+PDCRCGolden]);
            PDUPDATE_PRINT("[PDUPDATE] CRC in TCPC:%x",u8MTPCRC);
            PDUPDATE_printMsg("[PDUPDATE] Start FW Upgrade!");
            return msAPI_PDUpdateFW(u32AddInFlash, u8PDSA_MCU, u8PDSA_PCB);
        }
        else
        {
            // TCPC Reset
            u8Rtn |= msDrvPD_Reset();
            if(u8Rtn)
            {
                PDUPDATE_printMsg("[PDUPDATE] Reset IIC Fail !");
                return ePDUGSTS_ER_IIC;
            }
            PDUPDATE_printMsg("[PDUPDATE] CRC Same, Skip Upgrade!");
            return ePDUGSTS_SKIP;
        }
    }
    else
    {
        // TCPC Reset
        u8Rtn |= msDrvPD_Reset();
        if(u8Rtn)
        {
            PDUPDATE_printMsg("[PDUPDATE] Reset IIC Fail !");
            return ePDUGSTS_ER_IIC;
        }
        PDUPDATE_printMsg("[PDUPDATE] CRC IIC Fail !");
        return ePDUGSTS_ER_IIC;
    }
}

#endif

