#define _HWI2C_C_

#include "types.h"
#include "board.h"
//#include "MDebug.h"
#include "Common.h"
#include "misc.h"
#include "Global.h"
#include "ms_reg.h"
#include "Ms_rwreg.h"
#include "hwi2c.h"
//#include "assert.h"

/* These are definitions related to I2C spped */
#define HW_I2C_SPEED    400000ul // initila speed: 100KHz
#define SYSTEM_CLOCK    12000000ul
#define HIGH_COUNT      ((SYSTEM_CLOCK/HW_I2C_SPEED)*6/10) //30*6/10 = 0x12
#define LOW_COUNT       ((SYSTEM_CLOCK/HW_I2C_SPEED)*4/10) // 0x0C
#define START_COUNT     (LOW_COUNT>>2) //0x03
#define STOP_COUNT      (LOW_COUNT>>2) //0x03
#define SDA_COUNT       4
#define LATCH_COUNT     2

/*MIIC2 For HDMITx*/
#define HW_I2C_SPEED_MIIC2    50000ul // initila speed: 100KHz
#define SYSTEM_CLOCK_MIIC2    12000000ul
#define HIGH_COUNT_MIIC2      ((SYSTEM_CLOCK_MIIC2/HW_I2C_SPEED_MIIC2)*6/10)
#define LOW_COUNT_MIIC2       ((SYSTEM_CLOCK_MIIC2/HW_I2C_SPEED_MIIC2)*4/10)
#define START_COUNT_MIIC2     (LOW_COUNT_MIIC2>>2)
#define STOP_COUNT_MIIC2      (LOW_COUNT_MIIC2>>2)
#define SDA_COUNT_MIIC2       4
#define LATCH_COUNT_MIIC2     2

/* These are definitions of timeout counters for preventing infinite while loop */
#define BREAK_TIME      5           // unit: ms
//#define CHECK_ACK_TIME  25          // CHECK_ACK_TIME *4us
#define GET_MS_COUNT() ((ms_Counter <= BREAK_TIME)?(ms_Counter+ SystemTick):(ms_Counter))

/*
 * The states of master I2C.
 *
 * After sending stop bit, the state machine must go into IDLE STATE before
 * sending the start bit of next command. Similarly, the state machine must
 * go into WAITING_STATE after sending start bit or a byte.
 */
#define IDLE_STATE      0x00
#define WAITING_STATE   0x0C

#if (HW_I2C_SPEED > 400000ul)
#warning "The initial speek of master I2C must under 400kHz !"
#endif

#define HWI2C_DEBUG    1
#if ENABLE_DEBUG&&HWI2C_DEBUG
    #define HWI2C_PRINT(str, value)     printData(str, value)
    #define HWI2C_printMsg(str)           printMsg(str)
#else
    #define HWI2C_PRINT(str, value)
    #define HWI2C_printMsg(str)
#endif

#if ENABLE_DMA_MIIC
void hw_i2c_dma_rst(BYTE i2c_dev);
void hw_i2c_dma_miu_rst(BYTE i2c_dev);
#endif

struct error_info
{
    const int value;
    const char* const name;
    const char* const msg;
};

#define ENTRY(value, name, msg) {value, name, msg}
const struct error_info i2c_error_table[] =
{
    ENTRY(0, NULL, NULL),
    ENTRY(EMIICSTART, "EMIICSTART", "Fail to send start bit"),
    ENTRY(EMIICSTOP, "EMIICSTOP", "Fail to send stop bit"),
    ENTRY(EMIICSTATE, "EMIICSTATE", "Master I2C state machine error"),
    ENTRY(EMIICSB, "EMIICSB", "Fail to send a byte"),
    ENTRY(EMIICRB, "EMIICRB", "Fail to receive a byte"),
    ENTRY(EMIICACK, "EMIICACK", "No acknowledgement"),
};

void hw_i2c_intial(BYTE i2c_dev)
{
    //assert((i2c_dev == MIIC0) || (i2c_dev == MIIC1));

    /* Configure PAD mux */
    if(i2c_dev == MIIC0)
    {
        msWriteByteMask(REG_000405, MIIC0_ENABLE, MIIC0_ENABLE_MASK);
        if(SYSTEM_CLOCK == 12000000ul)
            msWriteByteMask(REG_0003B1, 0x00, 0xD0);    // set clk to XTAL
        else if(SYSTEM_CLOCK == 72000000ul)
            msWriteByteMask(REG_0003B1, 0xC0, 0xD0);    // set clk to 72M
    }
    else if(i2c_dev == MIIC1)
    {
        msWriteByteMask(REG_000405, MIIC1_ENABLE, MIIC1_ENABLE_MASK);
        if(SYSTEM_CLOCK == 12000000ul)
            msWriteByteMask(REG_0003B2, 0x00, 0x0D);    // set clk to XTAL
        else if(SYSTEM_CLOCK == 72000000ul)
            msWriteByteMask(REG_0003B2, 0x0C, 0x0D);    // set clk to 72M
            msWriteByteMask(REG_000406, 0 ,BIT1|BIT0); //Disable S/PDIF output pad control on SPDIFO_AU (GPIO_A5).
    }

    /* Initial I2C speed */
    msWrite2Byte(HWI2C_HIGH_CNT_REG(i2c_dev), HIGH_COUNT);
    msWrite2Byte(HWI2C_LOW_CNT_REG(i2c_dev), LOW_COUNT);
    msWrite2Byte(HWI2C_START_CNT_REG(i2c_dev), START_COUNT);
    msWrite2Byte(HWI2C_STOP_CNT_REG(i2c_dev), STOP_COUNT);
    msWrite2Byte(HWI2C_SDA_CNT_REG(i2c_dev), SDA_COUNT);
    msWrite2Byte(HWI2C_LATCH_CNT_REG(i2c_dev), LATCH_COUNT);

#if !ENABLE_DMA_MIIC
    /* Reset I2C */
    msWriteByteMask(HWI2C_CONFIG_REG(i2c_dev), BIT6|BIT5|BIT0   , BIT6|BIT5|BIT0);     //Enable reg_oen_push_en
    msWriteByteMask(HWI2C_CONFIG_REG(i2c_dev), BIT6|BIT5        , BIT6|BIT5|BIT0);
#else
    /* Reset I2C */
    msWriteByteMask(HWI2C_CONFIG_REG(i2c_dev), BIT6|BIT5|BIT2|BIT0   , BIT6|BIT5|BIT2|BIT0);    //Enable reg_oen_push_en
    msWriteByteMask(HWI2C_CONFIG_REG(i2c_dev), BIT6|BIT5|BIT2        , BIT6|BIT5|BIT2|BIT0);

    
    //DMA Internal Buffer mode I2C initial
    //(1)Claer interrupt
    CLEAR_DMA_INT(i2c_dev);

    //(2) reset DMA
    //(2-1)reset DMA engin
    hw_i2c_dma_rst(i2c_dev);

    //(2-2)reset MIU module in DMA engin
    hw_i2c_dma_miu_rst(i2c_dev);

    //(3)default config
    //(3-1)address mode : normal mode
    msWriteByteMask(HWI2C_DMA_SLVCFG_H_REG(i2c_dev), 0 ,BIT2);

    //(3-2)MIU Priority
    msWriteByteMask(HWI2C_DMA_CONFIG_REG(i2c_dev), 0 ,BIT4);

    //(3-3)MIU Channel :MIU Ch0
    msWriteByteMask(HWI2C_DMA_CTL_REG(i2c_dev), 0 ,BIT7);

    //(3-4)Enable DMA transder interrupt
    msWriteByteMask(HWI2C_DMA_CONFIG_REG(i2c_dev), BIT2 ,BIT2);

    //(3-5)Set DMA USE Reg
    msWriteByteMask(HWI2C_DMA_CONFIG_REG(i2c_dev), BIT6 ,BIT6); //0: Dram 1:internal reg

    //(3-6)DMA Default default disable, enable when use it
    msWriteByteMask(HWI2C_CONFIG_REG(i2c_dev), 0 , BIT1);  //Enable DMA, Default is false


#endif


}

BYTE hw_i2c_start(BYTE i2c_dev)
{
    WORD break_count;

    /* Trigger start bit */
    STARTBIT(i2c_dev);
    break_count =  GET_MS_COUNT() - BREAK_TIME;
    while(WAIT_READY(i2c_dev) == FALSE)
    {
        if(break_count == ms_Counter)
            return EMIICSTART;
    }
    CLEARFLAG(i2c_dev);
    /* Wait I2C state machine going to WAITING_STATE */
    break_count = GET_MS_COUNT() - BREAK_TIME;
    while(MIIC_STATE(i2c_dev) != WAITING_STATE)
    {
        if(break_count == ms_Counter)
            return EMIICSTATE;
    }
    return 0;
}

BYTE hw_i2c_stop(BYTE i2c_dev)
{
    WORD break_count;

    /* Trigger stop bit */
    STOPBIT(i2c_dev);
    break_count =  GET_MS_COUNT() - BREAK_TIME;
    while(WAIT_READY(i2c_dev) == FALSE)
    {
        if(break_count == ms_Counter)
            return EMIICSTOP;
    }
    CLEARFLAG(i2c_dev);
    /* Wait IIC state machine going to IDLE_STATE */
    break_count = GET_MS_COUNT() - BREAK_TIME;
    while(MIIC_STATE(i2c_dev) != IDLE_STATE)
    {
        if(break_count == ms_Counter)
            return EMIICSTATE;
    }
    return 0;
}

BYTE hw_i2c_send_byte(BYTE i2c_dev, BYTE send_data)
{
    BYTE ack = 0;
    WORD break_count;

    msWriteByte(HWI2C_WDATA_REG(i2c_dev), send_data);
    break_count =  GET_MS_COUNT() - BREAK_TIME;
    while(WAIT_READY(i2c_dev) == FALSE)
    {
        if(break_count == ms_Counter)
            return EMIICSB;
    }
    if(CHECK_NONACK(i2c_dev))
        ack = EMIICACK;

    CLEARFLAG(i2c_dev);

    /* Wait IIC state machine going to WAITING_STATE */
    break_count = GET_MS_COUNT() - BREAK_TIME;
    while(MIIC_STATE(i2c_dev) != WAITING_STATE)
    {
        if(break_count == ms_Counter)
             return EMIICSTATE;
    }
    return ack;
}

BYTE hw_i2c_receive_byte(BYTE i2c_dev, BOOL ack_for_rev, BYTE *rev_buf)
{
    WORD break_count;

    //assert(rev_buf != NULL);

    if(ack_for_rev)
        ACK_REV(i2c_dev);
    else
        NACK_REV(i2c_dev);

    break_count =  GET_MS_COUNT() - BREAK_TIME;
    while(WAIT_READY(i2c_dev) == FALSE)
    {
        if(break_count == ms_Counter)
            return EMIICRB;
    }
    CLEARFLAG(i2c_dev);
    *rev_buf = msReadByte(HWI2C_RDATA_REG(i2c_dev));

    return 0;
}

BOOL hw_i2c_burst_write(BYTE u8IICIndex, WORD count, BYTE *buffer)
{
    while(count--)
    {
        if(hw_i2c_send_byte(u8IICIndex, *(buffer++)) != 0)
            return FALSE;
    }
    return TRUE;
}

BOOL hw_i2c_burst_read(BYTE u8IICIndex, BYTE count, BYTE * buffer)
{
    BYTE i, result;

    for(i = 0; i < count - 1; i++)
    {
        result = hw_i2c_receive_byte(u8IICIndex, TRUE, (buffer + i));

        if (result != 0)
            return FALSE;
    }
    result = hw_i2c_receive_byte(u8IICIndex, FALSE, (buffer + i));

    if (result != 0)
        return FALSE;

    return TRUE;
}

#if ENABLE_DMA_MIIC
void hw_i2c_dma_rst(BYTE i2c_dev)
{
    msWriteByteMask(HWI2C_DMA_CONFIG_REG(i2c_dev), BIT1, BIT1);
    msWriteByteMask(HWI2C_DMA_CONFIG_REG(i2c_dev), 0, BIT1);
}

void hw_i2c_dma_miu_rst(BYTE i2c_dev)
{
    msWriteByteMask(HWI2C_DMA_CONFIG_REG(i2c_dev), BIT3, BIT3);
    msWriteByteMask(HWI2C_DMA_CONFIG_REG(i2c_dev), 0, BIT3);
}

void hw_i2c_dma_write_internal_buf(BYTE u8IICIndex, WORD count, BYTE *buffer)
{
    WORD u16_i = 0;
    //printf("Write internal buff = ");
    while(count--)
    {
        msWriteByte((MIICRegBase(u8IICIndex)+(0x80+u16_i)), buffer[u16_i]);
        //printf("0x%x ",msReadByte(MIICRegBase(u8IICIndex)+(0x80+u16_i)));
        u16_i++;
    }
    //printf("\r\n");
}

void hw_i2c_dma_get_internal_buf(BYTE u8IICIndex, WORD count, BYTE *buffer)
{
    WORD u16_i = 0;

    //printf("Read Data = \n");
    while(count--)
    {
        buffer[u16_i] = msReadByte(MIICRegBase(u8IICIndex)+(0xC0+u16_i));
        //printf("0x%x ",buffer[u16_i]);
        u16_i++;
    }
    //printf("\r\n");
}
void hw_i2c_dma_setSlaveID(BYTE u8IICIndex, DWORD u32SlaveAddr)
{
    //for 10-bit slave Address
    if(u32SlaveAddr & 0xFF00)
    {
        msWriteByte(HWI2C_DMA_SLVCFG_L_REG(u8IICIndex), (u32SlaveAddr>>9));
        msWriteByte(HWI2C_DMA_CMD_DATA0_REG(u8IICIndex), u32SlaveAddr);

        //1byte cmd for slave addr
        msWriteByte(HWI2C_DMA_CMD_LEN_REG(u8IICIndex), 1);
    }
    else
    {
        //7-bit slave Address
        //Set Slave Address
        //printf("DMA Slave address = 0x%x",u32SlaveAddr);
        msWriteByte(HWI2C_DMA_SLVCFG_L_REG(u8IICIndex), u32SlaveAddr);

        //No cmd for slave addr
        msWriteByte(HWI2C_DMA_CMD_LEN_REG(u8IICIndex), 0);
    }

}

void hw_i2c_dma_setDataLen(BYTE u8IICIndex, WORD count)
{
    //The internal buffer maxim is 32 byte
    msWriteByte(HWI2C_DMA_DATA_LEN0_L_REG(u8IICIndex), count);
    msWriteByte(HWI2C_DMA_DATA_LEN0_H_REG(u8IICIndex), (count>>8));
    msWriteByte(HWI2C_DMA_DATA_LEN1_L_REG(u8IICIndex), 0x00);
    msWriteByte(HWI2C_DMA_DATA_LEN1_H_REG(u8IICIndex), 0x00);
}

void hw_i2c_dma_setmiuaddr(BYTE u8IICIndex, DWORD u32MiuAddr)
{
    msWriteByte(HWI2C_DMA_MIU_ADDR0_L_REG(u8IICIndex), u32MiuAddr);
    msWriteByte(HWI2C_DMA_MIU_ADDR0_H_REG(u8IICIndex), (u32MiuAddr>>8));
    msWriteByte(HWI2C_DMA_MIU_ADDR1_L_REG(u8IICIndex), (u32MiuAddr>>16));
    msWriteByte(HWI2C_DMA_MIU_ADDR1_H_REG(u8IICIndex), (u32MiuAddr>>24));
}

void hw_i2c_dma_ctl(BYTE u8IICIndex, BOOL bRead, BOOL bStop)
{
    WORD val = 0;
    //bit 5 => 1: no Stop, 0:Stop
    //bit 6 => 1:Read, 0:write

    val = msReadByte(HWI2C_DMA_CTL_REG(u8IICIndex));

    if(bRead)
        val |= BIT6;
    else
        val &= ~BIT6;

    if(!bStop)
        val |= BIT5;
    else
        val &= ~BIT5;

    msWriteByte(HWI2C_DMA_CTL_REG(u8IICIndex), val);
}

BYTE hw_i2c_dma_send_byte(BYTE u8IICIndex, DWORD u32SlaveAddr, WORD count, BYTE *buffer, BOOL stop)
{
    BYTE ack = 0;
    WORD break_count = 0;

    u32SlaveAddr = u32SlaveAddr&0x7F;

    if ( count > HWDMA_I2C_BUFFER_SIZE_MAX)
    {
        HWI2C_printMsg("EXCEED MAXIMUM DMA IIC BUFFER");
        return EMIICSB;
    }

    //Enable DMA mode
    msWriteByteMask(HWI2C_CONFIG_REG(u8IICIndex), BIT1, BIT1);  //Enable DMA, Default is false

    //(1)Set Slave address
    hw_i2c_dma_setSlaveID(u8IICIndex, u32SlaveAddr);

    //(2)Set data transfer length
    hw_i2c_dma_setDataLen(u8IICIndex, count);

    // use DMA internal buffer
	// (3)set data address to 0
    hw_i2c_dma_setmiuaddr(u8IICIndex, 0);

    //(4)Fill Data to internal buffer
    hw_i2c_dma_write_internal_buf(u8IICIndex, count, buffer);

    //(5)Reset dma engine before trigger dma
    hw_i2c_dma_rst(u8IICIndex);

    //(6)Reset dma miu before trigger dma
    hw_i2c_dma_miu_rst(u8IICIndex);

    //(7)direction & transfer with stop
	hw_i2c_dma_ctl(u8IICIndex, false, stop);

	//(8)trigger DMA
	TRIG_DMA_START(u8IICIndex);

	//(9)wait DMA done
	break_count =  GET_MS_COUNT() - BREAK_TIME;
    while(WAIT_DMA_DONE(u8IICIndex) == FALSE)
    {
        if(break_count == ms_Counter)
            ack = EMIICSB;
    }

    if (count != TRANSF_DMA_COUNT(u8IICIndex))
        ack = EMIICACK;

    MsOS_DelayTaskUs(60);

    //(10)Clear DMA Done Flag
    CLEAR_DMA_TRANS_FLAG_DONE(u8IICIndex);

    //Disable DMA mode
    msWriteByteMask(HWI2C_CONFIG_REG(u8IICIndex), 0, BIT1);  

    return ack;
}
BYTE hw_i2c_dma_read_byte(BYTE u8IICIndex, DWORD u32SlaveAddr, WORD count, BYTE *buffer, BOOL stop)
{
    BYTE ack = 0;
    WORD break_count = 0;

    u32SlaveAddr = (u32SlaveAddr&0x7F);
    
    if ( count > HWDMA_I2C_BUFFER_SIZE_MAX)
    {
        HWI2C_printMsg("EXCEED MAXIMUM DMA IIC BUFFER");
        return EMIICRB;
    }

    //Enable DMA mode
    msWriteByteMask(HWI2C_CONFIG_REG(u8IICIndex), BIT1, BIT1);  //Enable DMA, Default is false

    //(1)Set Slave address
    hw_i2c_dma_setSlaveID(u8IICIndex, u32SlaveAddr);

    //(2)Set data transfer length
    hw_i2c_dma_setDataLen(u8IICIndex, count);

    // use DMA internal buffer
	//(3)set data address to 0
    hw_i2c_dma_setmiuaddr(u8IICIndex, 0);

#if 0
    //(4)Fill Data to internal buffer
    hw_i2c_dma_write_internal_buf(u8IICIndex, count, buffer);
#endif

    //(5)Reset dma engine before trigger dma
    hw_i2c_dma_rst(u8IICIndex);

    //(6)Reset dma miu before trigger dma
    hw_i2c_dma_miu_rst(u8IICIndex);

    //(7)direction & transfer with stop
	hw_i2c_dma_ctl(u8IICIndex, true, stop);

	//(8)trigger DMA
	TRIG_DMA_START(u8IICIndex);

	//(9)wait DMA done
	break_count =  GET_MS_COUNT() - BREAK_TIME;
    while(WAIT_DMA_DONE(u8IICIndex) == FALSE)
    {
        if(break_count == ms_Counter)
            ack = EMIICRB;
    }

    if (count != TRANSF_DMA_COUNT(u8IICIndex))
        ack = EMIICACK;
	
    MsOS_DelayTaskUs(60);

    //(10)Clear DMA Done Flag
    CLEAR_DMA_TRANS_FLAG_DONE(u8IICIndex);

    hw_i2c_dma_get_internal_buf(u8IICIndex, count, buffer);

    //Disable DMA mode
    msWriteByteMask(HWI2C_CONFIG_REG(u8IICIndex), 0, BIT1);  

    return ack;
}
#endif

//========================================================
/*
 * These functions (i2C_Intial, i2c_Start, i2c_Stop, i2c_SendByte, i2c_ReceiveByte)
 * share the same names with software I2C functions in i2c.c. They are mainly used
 * for read/write EEROM through master I2C 1 (MIIC1).
 */
//========================================================
#if (USE_SW_I2C == 0)
void i2C_Intial(void)
{
   hw_i2c_intial(USE_MIIC);
}
BOOL i2c_Start(void)
{
    BYTE result;
    if ((result = hw_i2c_start(USE_MIIC)) > 0)
    {
        HWI2C_printMsg(i2c_error_table[result].msg);
        return FALSE;
    }
    return TRUE;
}
void i2c_Stop(void)
{
    BYTE result;

    if ((result = hw_i2c_stop(USE_MIIC)) > 0)
    {
        HWI2C_printMsg(i2c_error_table[result].msg);
    }
}
BOOL i2c_SendByte(BYTE ucWriteData)
{
    BYTE result;
    if ((result = hw_i2c_send_byte(USE_MIIC, ucWriteData)) > 0)
    {
        HWI2C_printMsg(i2c_error_table[result].msg);
        return FALSE;
    }
    return TRUE;
}
BYTE i2c_ReceiveByte(BOOL bAck)
{
    BYTE ucReceiveByte = 0, result;

    if ((result = hw_i2c_receive_byte(USE_MIIC, bAck, &ucReceiveByte)) > 0)
    {
        HWI2C_printMsg(i2c_error_table[result].msg);
    }
    return ucReceiveByte;
}

#if ENABLE_DMA_MIIC
BOOL i2c_dma_SendByte(DWORD u32SlaveAddr, WORD count, BYTE *buffer, BOOL stop)
{
    BYTE result;

    if ((result = hw_i2c_dma_send_byte(USE_MIIC, u32SlaveAddr, count, buffer, stop)) > 0)
    {
        HWI2C_printMsg(i2c_error_table[result].msg);
        return FALSE;
    }
    return TRUE;
}

BOOL i2c_dma_ReadByte(DWORD u32SlaveAddr, WORD count, BYTE *buffer, BOOL stop)
{
    BYTE result;

    if ((result = hw_i2c_dma_read_byte(USE_MIIC, u32SlaveAddr, count, buffer, stop)) > 0)
    {
        HWI2C_printMsg(i2c_error_table[result].msg);
        return FALSE;
    }
    return TRUE;
}
#endif
#endif              /* End of  (USE_SW_I2C == 0) */


void hw_i2c_uncall(void)
{
	hw_i2c_intial(0);
	hw_i2c_start(0);
	hw_i2c_stop(0);
	hw_i2c_send_byte(0, 0);
	hw_i2c_receive_byte(0, 0, 0);
	hw_i2c_burst_write(0, 0, 0);
	hw_i2c_burst_read(0, 0, 0);
}
#undef _HWI2C_C_

