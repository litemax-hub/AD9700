#include "types.h"
#include "REG52.H"
#include "i2cdef.h"
#include "board.h"
#include "Debug.h"
#include "Common.h"
#include "misc.h"
#include "Global.h"
#include "GPIO_DEF.h"
#include "ms_reg.h"
#include "Ms_rwreg.h"
#if (USE_SW_I2C == 0)
#include "Hwi2c.h"
#endif
#include "i2c.h"

#define i2cSlaveAddr(deviceID, addr)    deviceID|((addr>>8)<<1)
#define i2cWordAddr(addr)       addr&0xFF

#if !USEFLASH || (defined(UseVGACableReadWriteAllPortsEDID)&&!defined(UseInternalDDCRam))
#if (USE_SW_I2C == 1)
//====================================
// Setup i2c Start condition
Bool i2c_Start( void )
{
    Set_i2c_SDA();
    Set_i2c_SCL();
    Delay4us();
    if( i2c_SDALo() || i2c_SCLLo() )
        return FALSE;
    Clr_i2c_SDA();
    Delay4us();
    Clr_i2c_SCL();
    Delay4us();
    return TRUE;
}
//=============================================
// Setup i2c Stop condition
void i2c_Stop( void )
{
    // SCL=L, SDA=L, Stop condition.
    Clr_i2c_SCL();
    Clr_i2c_SDA();
    Delay4us();
    Set_i2c_SCL();
    Delay4us();
    Set_i2c_SDA();
    Delay4us();
}
//============================================
Bool Wait_i2C_Ack( void )
{
    BYTE i;
    for( i = 0; i < 5; i++ )
    {
        Delay4us();
        if( i2c_SDALo() )
            return TRUE;
    }

    if( i2c_SDALo() )
        return TRUE;
    else
        return FALSE;

}
//============================================
Bool i2c_SendByte( BYTE value )
{
    BYTE i;
    Bool result;

    for( i = 0; i < 8; i++ ) // Send data via i2c pin
    {
        if( value & BIT7 )
            Set_i2c_SDA();
        else
            Clr_i2c_SDA();
        Delay4us();
        Set_i2c_SCL();
        Delay4us();
        value <<= 1;
        Clr_i2c_SCL();
    }
    Set_i2c_SDA();
    result = Wait_i2C_Ack();
    Set_i2c_SCL();
    Delay4us();
    Clr_i2c_SCL();
    Delay4us();
    Clr_i2c_SDA();

    return result;
}

//============================================
BYTE i2c_ReceiveByte( const Bool ack )
{
    BYTE i;
    BYTE value = 0;

    for( i = 0; i < 8; i++ )
    {
        value <<= 1;
        Set_i2c_SDA();
        Delay4us();
        Set_i2c_SCL();
        Delay4us();
        if( i2c_SDAHi() )
            value |= BIT0;
        Clr_i2c_SCL();
    }
    if( ack )
    {
        Clr_i2c_SDA();
    }
    else
    {
        Set_i2c_SDA();
    }
    Delay4us();
    Set_i2c_SCL();
    Delay4us();
    Clr_i2c_SCL();

    return value;
}
void i2c_Start1(void)
{
    Set_i2c_SDA();
    Delay4us();
    Set_i2c_SCL();
    Delay4us();
    Clr_i2c_SDA();
    Delay4us();
    Clr_i2c_SCL();
}
void i2C_Intial(void)
{
    BYTE i, j;

    for(i=0 ;i < 20; i++)	// Generate SCL signals to reset EEPROM.
    {
        Set_i2c_SCL();
        Delay4us();
        Clr_i2c_SCL();
        Delay4us();
    }

    j = 20;

    while( j-- )	// Male EEPROM to Release I2C bus.
    {
        for(i=0 ;i < 9; i++)	// 9 STARTs
        {
            i2c_Start1();
        }
        i2c_Stop();
        ForceDelay1ms(10);

        if(i2c_SDAHi())
            break;
    }

}
#endif

Bool i2c_BurstWrite( BYTE count, BYTE *buffer )
{
    while( count-- )
    {
        if( i2c_SendByte( *( buffer++ ) ) == FALSE )
            return FALSE;
    }
    return TRUE;
}
Bool i2c_BurstRead( BYTE count, BYTE * buffer )
{
    BYTE i;
    for( i = 0; i < count - 1; i++ )
    {
        *( buffer + i ) = i2c_ReceiveByte( 1 );
    }
    *( buffer + i ) = i2c_ReceiveByte( 0 );
    i2c_Stop();
    return TRUE;
}
static void ForceDelayNVRAM(WORD u16Nums) //
{
    WORD u16Count;

    u16Count = u16Nums*0xE8;

    while (u16Count--)
    {
        Delay4us();
    }
}

#if (EEPROM_TYPE>=EEPROM_24C32)
#define I2C_SLAVE_ADDR(deviceID, addr)   i2cSlaveAddr(deviceID, addr)
#define I2C_WORD_ADDR(addr)              i2cWordAddr(addr)

BYTE ucADDR_HI_BYTE;

Bool i2c_MasterStart( I2C_Direction direct, BYTE addr )
{
    #define NVRAM_DEVICE    0xA0

    BYTE u8Retry=5;
    //BYTE u8NvRamID=NVRAM_DEVICE;
    if (direct==I2C_READ) // Set I2C direction bit.
    {
        addr=NVRAM_DEVICE;// get 0xA0
        addr|=BIT0;
    }
    else
        addr&=~BIT0;

    while (u8Retry--)
    {
        if (i2c_Start()==FALSE)
        {
            ForceDelayNVRAM(1); //tony 24/11/03
            continue;
        }
        if(direct==I2C_READ)
        {
            if (i2c_SendByte(addr)==TRUE) // send address success
                return TRUE;
        }
        else
        {
            if (i2c_SendByte(NVRAM_DEVICE)==TRUE) // send address success
            {

                if (i2c_SendByte(ucADDR_HI_BYTE)==TRUE) // send address success
                    return TRUE;
            }
        }
        i2c_Stop();
        ForceDelayNVRAM(1);
    }
    return FALSE;
}

void i2c_WriteTBL(BYTE u8DeviceID, WORD u16Addr, BYTE *pu8Buffer, WORD u16Count)
{
    BYTE u8SlaveAddr;
    BYTE u8WordAddr;
    WORD u16Offset=0, u16PrevOffset=0;
    BYTE u8TempSize;
    //Bool succ=TRUE;
    WORD u16BitCounter;

    u16BitCounter = (u16Addr&0xFF);
    ucADDR_HI_BYTE = (u16Addr>>8);
    while (u16Count)
    {
        u16BitCounter+=(u16Offset-u16PrevOffset);
        u16PrevOffset = u16Offset;
        if(u16BitCounter > 0x00FF)
        {
            ucADDR_HI_BYTE = ((u16Addr+u16Offset)>>8);
            u16BitCounter=0;
        }

        u8SlaveAddr = I2C_SLAVE_ADDR(u8DeviceID, (u16Addr+u16Offset));
        u8WordAddr = I2C_WORD_ADDR((u16Addr+u16Offset));
        if (i2c_MasterStart(I2C_WRITE, u8SlaveAddr)==FALSE)
        {
            i2c_Stop();
            ForceDelayNVRAM(15);
            break;
        }

        if (i2c_BurstWrite(1, &u8WordAddr)==FALSE)
        {
            i2c_Stop();
            ForceDelayNVRAM(15);
            break;
        }

        u8TempSize=0x10-(u8WordAddr&0xF);
        if (u16Count>u8TempSize)
        {
            if (i2c_BurstWrite(u8TempSize, (BYTE*)(pu8Buffer+u16Offset))==FALSE)
            {
                i2c_Stop();
                ForceDelayNVRAM(15);
                break;
            }
            i2c_Stop();
            u16Count-=u8TempSize;
            u16Offset+=u8TempSize;
        }
        else if (u16Count>0x10)
        {
            if (i2c_BurstWrite(0x10, (BYTE*)(pu8Buffer+u16Offset))==FALSE)
            {
                i2c_Stop();
                ForceDelayNVRAM(15);
                break;
            }
            i2c_Stop();
            u16Count-=0x10;
            u16Offset+=0x10;
        }
        else
        {
            if (i2c_BurstWrite(u16Count, (BYTE*)(pu8Buffer+u16Offset))==FALSE)
            {
                i2c_Stop();
                ForceDelayNVRAM(15);
                break;
            }
            i2c_Stop();
            u16Count=0;
        }
        ForceDelayNVRAM(15);
    }
}

void i2c_ReadTBL(BYTE u8DeviceID, WORD u16Addr, BYTE *pu8Buffer, WORD u16Count)
{
    WORD u16Retry=10;
    BYTE u8SlaveAddr;
    BYTE u8WordAddr;
    WORD u16Offset=0;

    ucADDR_HI_BYTE = (u16Addr>>8);

    while (u16Count)
    {
        u8SlaveAddr=I2C_SLAVE_ADDR(u8DeviceID, (u16Addr+u16Offset));
        u8WordAddr=I2C_WORD_ADDR((u16Addr+u16Offset));

        if(((u16Addr&0x00FF)+u16Offset) > 0x00FF)
        {
            ucADDR_HI_BYTE = ((u16Addr+u16Offset)>>8);
        }

        u16Retry = 10;//Reset retry time

        while ((--u16Retry))
        {
            if (i2c_MasterStart(I2C_WRITE, u8SlaveAddr)==FALSE)
            {
                ForceDelayNVRAM(2);
                i2c_Stop();
                continue;
            }

            if (i2c_BurstWrite(1, &u8WordAddr)==FALSE)
                continue;

            if (i2c_MasterStart(I2C_READ, u8SlaveAddr)==FALSE)
                continue;

            if(u16Count >= 0x80)
            {
                if (i2c_BurstRead(0x80, pu8Buffer+u16Offset)==FALSE)
                    continue;
                u16Offset += 0x80;
                u16Count -= 0x80;
            }
            else
            {
                if (i2c_BurstRead(u16Count, pu8Buffer+u16Offset)==FALSE)
                    continue;
                u16Count = 0;
            }
            break;
        }

        if(u16Retry == 0)//Read Fail
        {
            break;
        }
    }
}

#else

#define PageWriteLength     16
#define PageWriteDelayTime  10

Bool i2c_MasterStart( I2C_Direction direct, BYTE addr )
{
    BYTE retry = 3;

    if( direct == I2C_READ ) // Set I2C direction bit.
        addr |= BIT0;
    else
        addr &= ~BIT0;

    while( retry-- )
    {
        if( i2c_Start() == FALSE )
        {
            i2c_Stop();
            continue;
        }

        if( i2c_SendByte( addr ) == TRUE ) // send address success
            return TRUE;
        i2c_Stop();
        ForceDelay1ms( 2 );
    }
    return FALSE;
}

void i2c_WriteTBL( BYTE deviceID, WORD addr, BYTE *buffer, WORD count )
{
    BYTE slaveAddr;
    BYTE wordAddr;
    WORD offset = 0;
    BYTE tempSize;
    //Bool succ = TRUE;

    while( count )
    {
        slaveAddr = i2cSlaveAddr( deviceID, (addr + offset) );
        wordAddr = i2cWordAddr( (addr + offset) );
        if( i2c_MasterStart( I2C_WRITE, slaveAddr ) == FALSE )
            break;
        if( i2c_BurstWrite( 1, &wordAddr ) == FALSE )
            break;
        tempSize = PageWriteLength - ( wordAddr & ( PageWriteLength - 1 ) );
        if( count > tempSize )
        {
            if( i2c_BurstWrite( tempSize, ( BYTE* )( buffer + offset ) ) == FALSE )
                break;
            i2c_Stop();
            count -= tempSize;
            offset += tempSize;
        }
        else if( count > PageWriteLength )
        {
            if( i2c_BurstWrite( 0x10, ( BYTE* )( buffer + offset ) ) == FALSE )
                break;
            i2c_Stop();
            count -= PageWriteLength;
            offset += PageWriteLength;
        }
        else
        {
            if( i2c_BurstWrite( count, ( BYTE* )( buffer + offset ) ) == FALSE )
                break;
            i2c_Stop();
            count = 0;
        }
        ForceDelayNVRAM( PageWriteDelayTime );
    }
    i2c_Stop();
}

void i2c_ReadTBL( BYTE deviceID, WORD addr, BYTE *buffer, WORD count )
{
    WORD retry = 5;
    BYTE slaveAddr;
    BYTE wordAddr;
    WORD offset = 0;

    while(count)
    {
        slaveAddr = i2cSlaveAddr( deviceID, (addr+offset));
        wordAddr = i2cWordAddr((addr+offset));

        while( retry-- )
        {
            if( i2c_MasterStart( I2C_WRITE, slaveAddr ) == FALSE )
                continue;
            if( i2c_BurstWrite( 1, &wordAddr ) == FALSE )
                continue;
            if( i2c_MasterStart( I2C_READ, slaveAddr ) == FALSE )
                continue;

            if( count >= 0x80 )
            {
                if ( i2c_BurstRead( 0x80, buffer+offset)==FALSE )
                    continue;
                offset += 0x80;
                count -= 0x80;
            }
            else
            {
                if ( i2c_BurstRead( count, buffer+offset)==FALSE )
                    continue;
                count = 0;
            }

            break;
        }
    }
}
#endif

#endif
