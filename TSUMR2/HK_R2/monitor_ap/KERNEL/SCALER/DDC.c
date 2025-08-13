#include "types.h"
#include "ms_reg.h"
#include "Ms_rwreg.h"
#include "Global.h"
#include "msflash.h"
#include "Power.h"

#ifdef UseInternalDDCRam
void mStar_WriteDDC( BYTE EDIDType );

void mStar_ClrDDC_WP( void )  //turn off Ram WP
{
    old_msWriteByte( BK0_B8, BIT7 );
    #if INPUT_TYPE!=INPUT_1A
    mStar_WriteByteMask( BK0_B4, (BIT5 | BIT7), (BIT5 | BIT7) );    // DVI
    #endif
}
void mStar_SetDDC_WP( void )  //turn on Ram WP
{
    mStar_WriteByteMask( BK0_B8, (BIT7 | BIT5), (BIT7 | BIT5 | BIT0) );
    #if INPUT_TYPE!=INPUT_1A
    old_msWriteByte( BK0_B4, BIT7 );
    #endif
}

void mStar_InitDDC( void )
{
    BYTE xdata i;
    BYTE xdata dd;

    old_msWriteByte( BK0_B8, BIT7 );
    old_msWriteByte( BK0_B9, BIT7 );

    for( i = 0; i < 128; i++ )
    {
        dd = FlashReadByte( DDCAKEYSET_START + i );
        old_msWriteByte( BK0_BA, i );
        old_msWriteByte( BK0_BB, dd );
    }
    #if INPUT_TYPE!=INPUT_1A
    #ifndef DVI_EDID_OnlyINEEPROM  //DVI in eeprom ,VGA in flash  //AMY 2008/5/13
    mStar_WriteByteMask( BK0_B4, BIT7, BIT7 );
    old_msWriteByte( BK0_B5, BIT7 );
    for( i = 0; i < 128; i++ )
    {
        dd = FlashReadByte( DDCDKEYSET_START + i );
        old_msWriteByte( BK0_B6, i );
        old_msWriteByte( BK0_B7, dd );
    }
    #endif
    #endif
    // VGA set B8h[5], enable iic write protection.
    // DVI clr B4h[5], enable iic write protection
    mStar_WriteByteMask( BK0_B8, (BIT5 | BIT7), (BIT5 | BIT7) );
    #ifndef DVI_EDID_OnlyINEEPROM  //DVI in eeprom ,VGA in flash  //AMY 2008/5/13
    mStar_WriteByteMask( BK0_B4, (BIT5 | BIT7), (BIT5 | BIT7) );    // DVI
    #endif

    mStar_SetDDC_WP();
}

Bool mStar_ReadDDC( BYTE EDIDType ) //read DDC from RAM and prepare for write to flash
{
    BYTE i;
    BYTE CheckSum = 0;
    if( EDIDType == VGA_EDID )
    {
        old_msWriteByte( BK0_BA, BIT7 );
        for( i = 0; i < 128; i++ )
        {
            DDC1[i] = old_msReadByte( BK0_BB );
            CheckSum += ( ~DDC1[i] );
        }
    }
#if INPUT_TYPE!=INPUT_1A
#ifndef DVI_EDID_OnlyINEEPROM  //DVI in eeprom ,VGA in flash  //AMY 2008/5/13
    else if( EDIDType == DVI_EDID )
    {
        old_msWriteByte( BK0_B6, BIT7 );
        for( i = 0; i < 128; i++ )
        {
            DDC1[i] = old_msReadByte( BK0_B7 );
            CheckSum += ( ~DDC1[i] );
        }
    }
#endif
#endif
    if( CheckSum == 0 )//EDID Data is right
        return TRUE;
    else
        return FALSE;
}

Bool CheckEDIDData( BYTE EDIDType )
{
    BYTE i;
    BYTE CheckSum = 0;
    if( EDIDType == VGA_EDID )
    {
        old_msWriteByte( BK0_BA, BIT7 );
        for( i = 0; i < 8; i++ )
        {
            DDC1[i] = old_msReadByte( BK0_BB );
            CheckSum += ( ~DDC1[i] );
        }
    }
    #if INPUT_TYPE!=INPUT_1A
    #ifndef DVI_EDID_OnlyINEEPROM  //DVI in eeprom ,VGA in flash  //AMY 2008/5/13
    else if( EDIDType == DVI_EDID )
    {
        old_msWriteByte( BK0_B6, BIT7 );
        for( i = 0; i < 8; i++ )
        {
            DDC1[i] = old_msReadByte( BK0_B7 );
            CheckSum += ( ~DDC1[i] );
        }
    }
    #endif
    #endif
    if( DDC1[0] == 0x00 && DDC1[1] == 0xFF && DDC1[2] == 0xFF && DDC1[3] == 0xFF && DDC1[4] == 0xFF && DDC1[5] == 0xFF && DDC1[6] == 0xFF && DDC1[7] == 0x00 )
    {
        mStar_WriteDDC( EDIDType );
    }
    //mStar_ReadDDC(EDIDType);
    if( DDC1[0] == 0xFF && DDC1[1] == 0x00 && DDC1[2] == 0x00 && DDC1[3] == 0x00 && DDC1[4] == 0x00 && DDC1[5] == 0x00 && DDC1[6] == 0x00 && DDC1[7] == 0xFF )
        return TRUE;
    else
        return FALSE;
}
void mStar_WriteDDC( BYTE EDIDType )
{
    BYTE i;
    BYTE dd;

    if( EDIDType == VGA_EDID )
    {
        old_msWriteByte( BK0_B8, BIT7 );
        for( i = 0; i < 128; i++ )
        {
            dd = FlashReadByte( DDCAKEYSET_START + i );
            old_msWriteByte( BK0_BA, i );
            old_msWriteByte( BK0_BB, dd );
        }
    }
    #if INPUT_TYPE!=INPUT_1A
    #ifndef DVI_EDID_OnlyINEEPROM  //DVI in eeprom ,VGA in flash  //AMY 2008/5/13
    else if( EDIDType == DVI_EDID )
    {
        mStar_WriteByteMask( BK0_B4, (BIT7), (BIT7) );
        for( i = 0; i < 128; i++ )
        {
            dd = FlashReadByte( DDCDKEYSET_START + i );
            old_msWriteByte( BK0_B6, i );
            old_msWriteByte( BK0_B7, dd );
        }
    }
    #endif
    #endif
    //mStar_WriteByteMask(BK0_B8, (BIT7|BIT5), (BIT7|BIT5|BIT0) );
}
extern void Delay1ms( WORD msNums );
extern void ForceDelay1ms( WORD msNums );

void mStar_CheckRAM( void )
{
    BYTE temp, temp2;
    bit WriteVGAEDID = 0;
#if INPUT_TYPE!=INPUT_1A
    bit WriteDVIEDID = 0;
#endif
    temp = old_msReadByte( BK0_B8 );
    temp2 = old_msReadByte( BK0_B4 );

#if INPUT_TYPE!=INPUT_1A
    if((( temp & BIT0 ) && ( temp & ( BIT2 | BIT5 ) ) == 0x00 ) || (( temp2 & BIT0 ) && ( temp2 & ( BIT2 | BIT5 ) ) == 0x20 ) )
#else
    if(( temp & BIT0 ) && ( temp & ( BIT2 | BIT5 ) ) == 0x00 )
#endif
    {
        temp = 0;
        while( temp < 60 ) //while( temp < 100 )  // xiacong 20080401 modify
        {
            ForceDelay1ms( 2 );
#if INPUT_TYPE!=INPUT_1A
            if(( old_msReadByte( BK0_B8 ) & BIT2 ) || ( old_msReadByte( BK0_B4 ) & BIT2 ) )
#else
            if( old_msReadByte( BK0_B8 ) & BIT2 )
#endif
            {
                temp = 0;   // DDC busy
            }
            else
            {
                temp++;
            }
        }

        if( CheckEDIDData( VGA_EDID ) )
            WriteVGAEDID = 1;
#if INPUT_TYPE!=INPUT_1A
        if( CheckEDIDData( DVI_EDID ) )
            WriteDVIEDID = 1;
#endif
        if( WriteVGAEDID
#if INPUT_TYPE!=INPUT_1A
                || WriteDVIEDID
#endif
          )
            ForceDelay1ms( 100 );
        else
            return;
        if( WriteVGAEDID ) //CheckEDIDData(VGA_EDID))
        {
            Power_TurnOffLed();
            if( mStar_ReadDDC( VGA_EDID ) )
            {
                Flash_Write_Factory_KeySet( FLASH_KEY_DDCA, 0, DDCAKEY_SIZE, DDC1, DDCAKEY_SIZE );
            }
            mStar_WriteDDC( VGA_EDID );
            Power_TurnOnGreenLed();
        }
#if INPUT_TYPE!=INPUT_1A
        #ifndef DVI_EDID_OnlyINEEPROM  //DVI in eeprom ,VGA in flash  //AMY 2008/5/13
        if( WriteDVIEDID ) //CheckEDIDData(DVI_EDID))
        {
            if( mStar_ReadDDC( DVI_EDID ) )
            {
                Flash_Write_Factory_KeySet( FLASH_KEY_DDCD, 0, 128, DDC1, 128 );
            }
            mStar_WriteDDC( DVI_EDID );
            mStar_WriteByteMask( BK0_B4, (BIT5), (BIT5) );
            Power_TurnOnGreenLed();
        }
#endif
#endif
    }
}
#endif
