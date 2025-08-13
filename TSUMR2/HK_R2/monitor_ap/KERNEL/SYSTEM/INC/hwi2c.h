#ifndef _HWI2C_H_
#define _HWI2C_H_

#ifdef _HWI2C_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

#define MIIC0                   0
#define MIIC1                   1

#define MIIC0_0_ENABLE          _BIT5   //MIIC_PM0 on GPIO14 GPIO15
#define MIIC0_1_ENABLE          _BIT6   //MIIC_PM0 on GPIO04 GPIO05
#define MIIC1_0_ENABLE          _BIT7   //MIIC_PM1 on GPIO01 GPIO02

#define MIIC0_ENABLE_MASK       _BIT6|_BIT5
#define MIIC1_ENABLE_MASK       _BIT7

#define HWI2C0_REG_BANK         0x001F00//0x111800
#define HWI2C1_REG_BANK         0x002200//0x111900

#define MIICRegBase(X)          (((X)==0)?(HWI2C0_REG_BANK):(HWI2C1_REG_BANK))

#define HWI2C_CONFIG_REG(X)     MIICRegBase(X) + 0x00
#define HWI2C_WDATA_REG(X)      MIICRegBase(X) + 0x04
#define HWI2C_RDATA_REG(X)      MIICRegBase(X) + 0x06
#define HWI2C_START_CNT_REG(X)  MIICRegBase(X) + 0x18
#define HWI2C_STOP_CNT_REG(X)   MIICRegBase(X) + 0x10
#define HWI2C_HIGH_CNT_REG(X)   MIICRegBase(X) + 0x12
#define HWI2C_LOW_CNT_REG(X)    MIICRegBase(X) + 0x14
#define HWI2C_SDA_CNT_REG(X)    MIICRegBase(X) + 0x16
#define HWI2C_LATCH_CNT_REG(X)  MIICRegBase(X) + 0x1a

#define MIIC_STATE(X)           (msReadByte(MIICRegBase(X)+0x0A))
#define WAIT_READY(X)           (msReadByte(MIICRegBase(X)+0x08) & BIT0)
#define CHECK_NONACK(X)         (msReadByte(MIICRegBase(X)+0x05) & BIT0)
#define CLEARFLAG(X)            (msWriteByte(MIICRegBase(X)+0x08, 0x00))
#define STARTBIT(X)            (msWriteByte(MIICRegBase(X)+0x02, 0x01))//( (msWriteByte(MIICRegBase(X)+0x02, 0x01)),printData("(MIICRegBase(X)==%x",MIICRegBase(X)),printData("msWriteByte(MIICRegBase(X)==%x", msReadByte(MIICRegBase(X))))
#define STOPBIT(X)              (msWriteByte(MIICRegBase(X)+0x03, 0x01))
#define ACK_REV(X)              (msWriteByte(MIICRegBase(X)+0x07, 0x01))
#define NACK_REV(X)             (msWriteByte(MIICRegBase(X)+0x07, 0x03))

#if ENABLE_DMA_MIIC
#define HWDMA_I2C_BUFFER_SIZE_MAX   32
#define HWI2C_DMA_CONFIG_REG(X)               MIICRegBase(X) + 0x40
#define HWI2C_DMA_MIU_ADDR0_L_REG(X)          MIICRegBase(X) + 0x42
#define HWI2C_DMA_MIU_ADDR0_H_REG(X)          MIICRegBase(X) + 0x43
#define HWI2C_DMA_MIU_ADDR1_L_REG(X)          MIICRegBase(X) + 0x44
#define HWI2C_DMA_MIU_ADDR1_H_REG(X)          MIICRegBase(X) + 0x45
#define HWI2C_DMA_CTL_REG(X)                  MIICRegBase(X) + 0x46

#define HWI2C_DMA_CMD_DATA0_REG(X)       MIICRegBase(X) + 0x4A
#define HWI2C_DMA_CMD_DATA1_REG(X)       MIICRegBase(X) + 0x4B
#define HWI2C_DMA_CMD_DATA2_REG(X)       MIICRegBase(X) + 0x4C
#define HWI2C_DMA_CMD_DATA3_REG(X)       MIICRegBase(X) + 0x4D

#define HWI2C_DMA_CMD_LEN_REG(X)         MIICRegBase(X) + 0x52

#define HWI2C_DMA_DATA_LEN0_L_REG(X)     MIICRegBase(X) + 0x54
#define HWI2C_DMA_DATA_LEN0_H_REG(X)     MIICRegBase(X) + 0x55
#define HWI2C_DMA_DATA_LEN1_L_REG(X)     MIICRegBase(X) + 0x56
#define HWI2C_DMA_DATA_LEN1_H_REG(X)     MIICRegBase(X) + 0x57

#define HWI2C_DMA_SLVCFG_L_REG(X)        MIICRegBase(X) + 0x5C
#define HWI2C_DMA_SLVCFG_H_REG(X)        MIICRegBase(X) + 0x5D

#define WAIT_DMA_DONE(X)                 (msReadByte(MIICRegBase(X)+0x48) & BIT0)
#define CLEAR_DMA_INT(X)                 (msWriteByteMask(MIICRegBase(X)+0x48, BIT0, BIT0))
#define TRIG_DMA_START(X)                (msWriteByteMask(MIICRegBase(X)+0x5E, BIT0, BIT0))
#define RETRIG_DMA_START(X)              (msWriteByteMask(MIICRegBase(X)+0x5F, BIT0, BIT0))
#define CLEAR_DMA_TRANS_FLAG_DONE(X)     (msWriteByteMask(MIICRegBase(X)+0x48, BIT0, BIT0))
#define TRANSF_DMA_COUNT(X)              (msReadByte(MIICRegBase(X)+0x58))
#endif

/* The error numbers of master I2C */
#define EMIICSTART  1
#define EMIICSTOP   2
#define EMIICSTATE  3
#define EMIICSB     4
#define EMIICRB     5
#define EMIICACK    6

enum
{
    eHWIIC_SYSTEM_CLOCK_XTAL,   // [3:2] = 2'b00
    eHWIIC_SYSTEM_CLOCK_72MHZ,  // [3:2] = 2'b01
    eHWIIC_SYSTEM_CLOCK_36MHZ,  // [3:2] = 2'b10
    eHWIIC_SYSTEM_CLOCK_54MHZ,  // [3:2] = 2'b11
};

INTERFACE void hw_i2c_intial(BYTE);
INTERFACE BYTE hw_i2c_start(BYTE);
INTERFACE BYTE hw_i2c_stop(BYTE);
INTERFACE BYTE hw_i2c_send_byte(BYTE, BYTE);
INTERFACE BYTE hw_i2c_receive_byte(BYTE, BOOL, BYTE *);
INTERFACE BOOL hw_i2c_burst_write(BYTE, WORD, BYTE *);
INTERFACE BOOL hw_i2c_burst_read(BYTE, BYTE, BYTE *);
INTERFACE void hw_i2c_uncall(void);

#if ENABLE_DMA_MIIC
INTERFACE void hw_i2c_dma_write_internal_buf(BYTE u8IICIndex, WORD count, BYTE *buffer);
INTERFACE void hw_i2c_dma_get_internal_buf(BYTE u8IICIndex, WORD count, BYTE *buffer);
INTERFACE void hw_i2c_dma_setSlaveID(BYTE u8IICIndex, DWORD u32SlaveAddr);
INTERFACE void hw_i2c_dma_setDataLen(BYTE u8IICIndex, WORD count);
INTERFACE void hw_i2c_dma_setmiuaddr(BYTE u8IICIndex, DWORD u32MiuAddr);
INTERFACE void hw_i2c_dma_ctl(BYTE u8IICIndex, BOOL bRead, BOOL bStop);
INTERFACE BYTE hw_i2c_dma_send_byte(BYTE u8IICIndex, DWORD u32SlaveAddr, WORD count, BYTE *buffer, BOOL stop);
INTERFACE BYTE hw_i2c_dma_read_byte(BYTE u8IICIndex, DWORD u32SlaveAddr, WORD count, BYTE *buffer, BOOL stop);
#endif


#if (USE_SW_I2C == 0)
INTERFACE void i2C_Intial(void);
INTERFACE BOOL i2c_Start(void);
INTERFACE void i2c_Stop(void);
INTERFACE BOOL i2c_SendByte(BYTE ucWriteData);
INTERFACE BYTE i2c_ReceiveByte(BOOL bAck);
#if ENABLE_DMA_MIIC
INTERFACE BOOL i2c_dma_SendByte(DWORD u32SlaveAddr, WORD count, BYTE *buffer, BOOL stop);
INTERFACE BOOL i2c_dma_ReadByte(DWORD u32SlaveAddr, WORD count, BYTE *buffer, BOOL stop);
#endif

#endif

#undef INTERFACE
#endif


