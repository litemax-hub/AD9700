
#include <string.h>
#include "Global.h"
#include "msACE.h"
#include "AutoGamma.h"
#include "drvLutAutoDownload.h"

#define drvAdjust_DEBUG    1
#if ENABLE_MSTV_UART_DEBUG && drvAdjust_DEBUG
#define drvAdjust_PRINT(format, ...)     printf(format, ##__VA_ARGS__)
#else
#define drvAdjust_PRINT(format, ...)
#endif
static BOOL _IsEnNonStdCSC = FALSE;
// 0 ~ 100 (default 50)
void mStar_AdjustUserPrefBlacklevel( BYTE Redblacklevel, BYTE Greenblacklevel, BYTE Blueblacklevel )
{
    Redblacklevel = ((( WORD )Redblacklevel * ( MaxBlackLevelValue - MinBlackLevelValue ))+50) / 100 + MinBlackLevelValue;
    Greenblacklevel = ((( WORD )Greenblacklevel * ( MaxBlackLevelValue - MinBlackLevelValue))+50) / 100 + MinBlackLevelValue;
    Blueblacklevel = ((( WORD )Blueblacklevel * ( MaxBlackLevelValue - MinBlackLevelValue))+50) / 100 + MinBlackLevelValue;
    //printData("###mStar_AdjustUserPrefBlacklevel(output) = %x\n", Redblacklevel);
    msSetRGBOffset(MAIN_WINDOW, Redblacklevel, Greenblacklevel, Blueblacklevel);
    msAdjustPreRGBOffset(MAIN_WINDOW);
}

/******************************************************************************************************************************************
* Custom Hue R/G/B/C/M/Y
******************************************************************************************************************************************/
void msDrvCustomHueEnable(BYTE u8WinIdx, Bool bEnable)
{
    if(u8WinIdx == MAIN_WINDOW)
    {
        msWriteByteMask(SC66_C2, (bEnable ? BIT0: 0),BIT0);
    }
    else
    {
        msWriteByteMask(SC66_C3, (bEnable ? BIT0: 0),BIT0);
    }
}

void msDrvAdjustCustomHue(BYTE u8Value, BYTE u8ColorIndex)
{
    msWriteByte(SC66_CE + u8ColorIndex, (u8Value & 0x7F));
}

/******************************************************************************************************************************************
* Custom Saturation R/G/B/C/M/Y for RGB
******************************************************************************************************************************************/
void msDrvCustomSatEnable(BYTE u8WinIdx, Bool bEnable)
{
    if(u8WinIdx == MAIN_WINDOW)
    {
        msWriteByteMask(SC66_C2, (bEnable ? BIT4: 0),BIT4);
    }
    else
    {
        msWriteByteMask(SC66_C3, (bEnable ? BIT4: 0),BIT4);
    }
}

void msDrvAdjustCustomSat(BYTE u8Value, BYTE u8ColorIndex)
{
    msWriteByte(SC66_D4 + u8ColorIndex, (u8Value & 0x7F));
}

/******************************************************************************************************************************************
* Custom Brightness R/G/B/C/M/Y for RGB
******************************************************************************************************************************************/
void msDrvCustomBriEnable(BYTE u8WinIdx, Bool bEnable)
{
    if(u8WinIdx == MAIN_WINDOW)
    {
        msWriteByteMask(SC66_C2, (bEnable ? BIT5: 0),BIT5);
    }
    else
    {
        msWriteByteMask(SC66_C3, (bEnable ? BIT5: 0),BIT5);
    }
}

void msDrvAdjustCustomBri(BYTE u8Value, BYTE u8ColorIndex)
{
    msWriteByte(SC66_DA + u8ColorIndex, (u8Value & 0x7F));
}

#if ENABLE_LUT_AUTODOWNLOAD

void msDrvClearDMADeGammaDataBlock(void)
{
    DWORD u32Gamma_write_ptr = DEGAMMA_ADL_ADDR;
    DWORD idx;
    for( idx=0; idx<DEGAMMA_ADL_SIZE; idx++)
    {
        msMemWriteByte(u32Gamma_write_ptr + idx, 0x00);
    }
}

void msDrvClearDMAFixGammaDataBlock(void)
{
    DWORD u32Gamma_write_ptr = FIXGAMMA_ADL_ADDR;
    DWORD idx;
    for( idx=0; idx<FIXGAMMA_ADL_SIZE; idx++) //
    {
        msMemWriteByte(u32Gamma_write_ptr + idx, 0x00);
    }
}

void msDrvClearDMAPostGammaDataBlock(void)
{
    DWORD u32Gamma_write_ptr = POSTGAMMA_ADL_ADDR;
    DWORD idx;
    for( idx=0; idx<POSTGAMMA_ADL_SIZE; idx++)
    {
        msMemWriteByte(u32Gamma_write_ptr + idx, 0x00);
    }
}

void msDrvDeGammaWriteTbl_1024E_16B_ByDMA(BYTE u8Color, BYTE **pu8GammaTblIdx)
{

    WORD entries;
    DWORD u32Address=0,addrMode = SC25_C1;
    msWriteBit(addrMode,(DEGAMMA_TABLE_ENTRIES==1024)?1:0 , _BIT3); //0:512E 1:1024E
    // 19bits alignment to 32bits
    u32Address = DEGAMMA_ADL_ADDR+(u8Color*DEGAMMA_TABLE_ENTRIES*sizeof(DWORD));

    //deGamma mode, 0: 512E, 1: 1024E
    entries = DEGAMMA_TABLE_ENTRIES;

    WORD u16Idx = 0;
    DWORD u32Data = 0;
    for(u16Idx=0; u16Idx<entries; u16Idx++)
    {
        u32Data = pu8GammaTblIdx[u8Color][u16Idx*2] + (pu8GammaTblIdx[u8Color][(u16Idx*2)+1] << 8);
        msMemWrite4Byte(u32Address, u32Data);
        u32Address+=sizeof(DWORD);
    }
}

void msDrvXvyccGamma_WaitADLFinished(BYTE u8WinIdx)
{
    UNUSED(u8WinIdx);
    DWORD addrEn = SC67_22;

    TimeOutCounter =50;
    while((!(msReadByte(addrEn) &_BIT2)) && TimeOutCounter){}

    TimeOutCounter = 50;
    while((msReadByte(addrEn) &_BIT2) && TimeOutCounter){}
}

void msDrvDeGamma_DMA_Reader_Trigger(BYTE u8WinIdx)
{
    DWORD addrEn = SC67_22;
    DWORD addrAddressPort = SC67_2C;
    DWORD addrDepth = SC67_34;
    DWORD addrLength = SC67_3A;
    DWORD addrInit = SC67_40;

    msWrite4Byte(addrAddressPort, (DEGAMMA_ADL_ADDR>>5));
    msWrite2Byte(addrDepth, 0x1C2); //0x1C2? // the amount of client6 auto download data (1024E)
    msWrite2Byte(addrLength, 0x1C2);
    msWrite2Byte(addrInit, 0x00);

    msWriteBit(addrEn, TRUE, BIT2);

    msDrvXvyccGamma_WaitADLFinished(u8WinIdx);

}

void msDrvFixGammaWriteTbl_256E_12B_ByDMA(BYTE u8Color, BYTE **pu8GammaTblIdx, BOOL bExtendMode)
{
    WORD i=0,
    u16Idx=0,
    u16FixDeGammaEntries=FIXGAMMA_TABLE_ENTRIES+FIXGAMMA_EX_TABLE_ENTRIES;
    DWORD u32Address=0;

    // 12bits alignment to 16bits
    u32Address = FIXGAMMA_ADL_ADDR+(u8Color*u16FixDeGammaEntries*sizeof(WORD));
    WORD u16Data = 0;
    for(i=0; i<u16FixDeGammaEntries; i++)
    {
        u16Idx = i;
        if(!bExtendMode)
        {
            u16Idx = (u16Idx >= FIXGAMMA_TABLE_ENTRIES) ? (FIXGAMMA_TABLE_ENTRIES-1) : i;
        }


        u16Data = pu8GammaTblIdx[u8Color][u16Idx*2] + (pu8GammaTblIdx[u8Color][(u16Idx*2)+1] << 8);
        msMemWrite2Byte(u32Address, u16Data);
        u32Address+=sizeof(WORD);
    }
    msWrite2Byte(SC25_E2+u8Color*2,u16Data); //max data
}

void msDrvFixGamma_DMA_Reader_Trigger(BYTE u8WinIdx, BOOL bExtendMode)
{
    DWORD addrEn = SC67_22;
    DWORD addrAddressPort = SC67_2C;
    DWORD addrDepth = SC67_34;
    DWORD addrLength = SC67_3A;
    DWORD addrInit = SC67_40;
    DWORD addrExtendMode = SC25_F1;

    msWriteBit(addrExtendMode, bExtendMode, _BIT3);

    msWrite4Byte(addrAddressPort, (DEGAMMA_ADL_ADDR>>5));
    msWrite2Byte(addrDepth, 0x1C2); // the amount of client6 auto download data (256E + 88E = 344E)
    msWrite2Byte(addrLength, 0x1C2);
    msWrite2Byte(addrInit, 0x00);

    msWriteBit(addrEn, TRUE, BIT2);

    msDrvXvyccGamma_WaitADLFinished(u8WinIdx);
}

void msDrvGammaWriteTbl_1024E_12B_N_ByDMA(BYTE u8Color, BYTE **pu8GammaTblIdx )
{
    WORD u16Idx=0;
    DWORD u32Address=0;

    // 14bits alignment to 16bits
    u32Address = POSTGAMMA_ADL_ADDR+(u8Color*POSTGAMMA_TABLE_ENTRIES*sizeof(WORD));

    for(u16Idx=0; u16Idx<POSTGAMMA_TABLE_ENTRIES; u16Idx++)
    {
        WORD u16Data = 0;
        u16Data = (pu8GammaTblIdx[u8Color][u16Idx*2] + (pu8GammaTblIdx[u8Color][(u16Idx*2)+1] << 8))/4;
        msMemWrite4Byte(u32Address, u16Data);
        u32Address+=sizeof(WORD);
    }
}

void msDrvGammaWriteTbl_1024E_12B_ByDMA(BYTE u8Color, BYTE **pu8GammaTblIdx )
{
    const int nSizeofGammaTable = 257;
    BYTE u8Idx=0;
    WORD u16Idx=0,
              u16Data = 0,
              u16TempGamma=0;
    DWORD u32Address=0,
                dwDecompressGamma[nSizeofGammaTable];

    // 14bits alignment to 16bits
    u32Address = POSTGAMMA_ADL_ADDR+(u8Color*POSTGAMMA_TABLE_ENTRIES*sizeof(WORD));
    msDrvDeCompressFMT46Gamma(*(pu8GammaTblIdx + u8Color), dwDecompressGamma);

    // Calculate Data
    for(u16Idx=0; u16Idx<nSizeofGammaTable; u16Idx++)
    {
        if (u16Idx > 0 && u16Idx < 256)
            u16TempGamma = (dwDecompressGamma[u16Idx] - dwDecompressGamma[u16Idx-1]) / 4;
        else if (u16Idx == 256)
            u16TempGamma = (dwDecompressGamma[u16Idx] - dwDecompressGamma[u16Idx-1]) / 3;
        for(u8Idx=0; u8Idx<4; u8Idx++)
        {
            if (u16Idx > 0)
            {
                if(u8Idx==3)
                {
                    if (u16Idx == 256)
                        continue;
                    u16Data = dwDecompressGamma[u16Idx];
                }
                else
                {
                    u16Data += u16TempGamma;
                }
            }
            else
            {
                u16Data = dwDecompressGamma[u16Idx];
            }

            msMemWrite2Byte(u32Address, (u16Data>>2));
            u32Address+=sizeof(WORD);

            if (u16Idx == 0)
            {
                break;
            }
        }
    }
}

void msDrvGamma_WaitADLFinished(BYTE u8WinIdx)
{
    UNUSED(u8WinIdx);
    DWORD addrEn = SC67_02;

    TimeOutCounter =50;
    while((!(msReadByte(addrEn) &BIT0)) && TimeOutCounter){}

    TimeOutCounter = 50;
    while((msReadByte(addrEn) &BIT0) && TimeOutCounter){}
}

void msDrvGamma_DMA_Reader_SWmode_Trigger(BYTE u8WinIdx)
{
    DWORD addrAddressPort = SC67_04;
    DWORD addrDepth = SC67_10;
    DWORD addrLength = SC67_16;
    DWORD addrInit = SC67_1C;
    DWORD addrMaxBase = SC10_FA;
    DWORD addrSWmodeEn = SC67_70;
    DWORD addrSWmodeTrigger = SC67_72;
    //Gamma R/G/B max base
    msWrite2Byte( addrMaxBase, 0x4000);
    msWrite2Byte( addrMaxBase + 2, 0x4000);
    msWrite2Byte( addrMaxBase + 4, 0x4000);

    msWrite4Byte(addrAddressPort, (POSTGAMMA_ADL_ADDR>>5));
    msWrite2Byte(addrDepth, 0xC0); // the amount of client6 auto download data (1024E)
    msWrite2Byte(addrLength, 0xC0);
    msWrite2Byte(addrInit, 0x00);
    msWriteBit(addrSWmodeEn, TRUE, BIT0);
    mdrv_Adjust_DS_WBit(addrSWmodeTrigger, TRUE, BIT0);
    msDrvGamma_WaitADLFinished(u8WinIdx);
}

void msDrvGamma_DMA_Reader_Trigger(BYTE u8WinIdx)
{
    DWORD addrEn = SC67_02;
    DWORD addrAddressPort = SC67_04;
    DWORD addrDepth = SC67_10;
    DWORD addrLength = SC67_16;
    DWORD addrInit = SC67_1C;
    DWORD addrMaxBase = SC10_FA;

    //Gamma R/G/B max base
    msWrite2Byte( addrMaxBase, 0x4000);
    msWrite2Byte( addrMaxBase + 2, 0x4000);
    msWrite2Byte( addrMaxBase + 4, 0x4000);

    msWrite4Byte(addrAddressPort, (POSTGAMMA_ADL_ADDR>>5));
    msWrite2Byte(addrDepth, 0xC0); // the amount of client6 auto download data (1024E)
    msWrite2Byte(addrLength, 0xC0);
    msWrite2Byte(addrInit, 0x00);
    msWriteBit(addrEn, TRUE, BIT0);
    msDrvGamma_WaitADLFinished(u8WinIdx);

}
#else
void msDrvGammaWriteTbl_1024E_12B(BYTE u8WinIdx, BYTE u8Color, BYTE **pu8GammaTblIdx )
    {
    UNUSED(u8WinIdx);
    const int nSizeofGammaTable = 257;
    BYTE  u8TriggerBits,u8Temp2;
    WORD u16WriteMask,
    u16Level,
    u16TempGamma=0,
    u16LatestData=0;
    DWORD dwDecompressGamma[nSizeofGammaTable];
    DWORD addrCmdPort = SC10_E0;
    DWORD addrAddressPort = SC10_E2;
    DWORD addrDataPort = SC10_E4;
    DWORD addrMaxBase = SC10_FA;

    switch (u8Color){
        case 0: {u8TriggerBits = 0;}break;
        case 1: {u8TriggerBits = BIT5;}break;
        case 2: {u8TriggerBits = BIT6;}break;
        default: {u8TriggerBits = 0;}break;
    }

    msDrvDeCompressFMT46Gamma(*(pu8GammaTblIdx + u8Color), dwDecompressGamma);
    // Write Gamma Table

    // burst write: [7] = main
    u16WriteMask = BIT7 | u8TriggerBits;
    // address port initial as 0
    msWrite2Byte( addrAddressPort, 0x00);
    // pre set value
    msWrite2Byte( addrDataPort, 0x00);

    msWriteByteMask( addrCmdPort, u16WriteMask, u16WriteMask);

    // Calculate Data
    for(u16Level=0; u16Level<257; u16Level++)
    {
        if (u16Level > 0 && u16Level < 256)
            u16TempGamma = (dwDecompressGamma[u16Level] - dwDecompressGamma[u16Level-1]) / 4;
        else if (u16Level == 256)
            u16TempGamma = (dwDecompressGamma[u16Level] - dwDecompressGamma[u16Level-1]) / 3;
        for(u8Temp2=0; u8Temp2<4; u8Temp2++)
        {
            if (u16Level > 0)
            {
                if(u8Temp2==3)
                {
                    if (u16Level == 256)
                        continue;
                    u16LatestData = dwDecompressGamma[u16Level];
                }
                else
                {
                    u16LatestData += u16TempGamma;
                }
            }
            else
            {
                u16LatestData = dwDecompressGamma[u16Level];
            }

            msWrite2Byte( addrDataPort, (u16LatestData>>2));

            if (u16Level == 0)
                break;

        }
    }


    //disable write & burst write cmd
    msWriteByteMask( addrCmdPort, 0x00, u16WriteMask);

    //Gamma R/G/B max base
    msWrite2Byte( addrMaxBase, 0x4000);
    msWrite2Byte( addrMaxBase + 2, 0x4000);
    msWrite2Byte( addrMaxBase + 4, 0x4000);

}


#endif

void msDrvGammaLoadTbl_1024E_12B(BYTE u8WinIdx, BYTE **pu8GammaTblIdx)
{
    BYTE u8TgtChannel;

    #if ENABLE_LUT_AUTODOWNLOAD
    msDrvClearDMAPostGammaDataBlock();
    #endif

    for(u8TgtChannel=0; u8TgtChannel<3; u8TgtChannel++ )
    {
        #if ENABLE_LUT_AUTODOWNLOAD
        msDrvGammaWriteTbl_1024E_12B_ByDMA(u8TgtChannel, pu8GammaTblIdx);
        #else
        msDrvGammaWriteTbl_1024E_12B(u8WinIdx, u8TgtChannel, pu8GammaTblIdx);
        #endif
    }

    #if ENABLE_LUT_AUTODOWNLOAD
    msDrvGamma_DMA_Reader_SWmode_Trigger(u8WinIdx);
    #endif

}

void msDrvGammaLoadTbl_1024E_12B_N(BYTE u8WinIdx, BYTE **pu8GammaTblIdx)
{
    BYTE u8TgtChannel;
    #if ENABLE_LUT_AUTODOWNLOAD
    msDrvClearDMAPostGammaDataBlock();
    #endif

    for(u8TgtChannel=0; u8TgtChannel<3; u8TgtChannel++ )
    {
        #if ENABLE_LUT_AUTODOWNLOAD
        msDrvGammaWriteTbl_1024E_12B_N_ByDMA(u8TgtChannel, pu8GammaTblIdx);
        #else
        msDrvGammaWriteTbl_1024E_12B_N(u8WinIdx, u8TgtChannel, pu8GammaTblIdx);
        #endif
    }

    #if ENABLE_LUT_AUTODOWNLOAD
    msDrvGamma_DMA_Reader_SWmode_Trigger(u8WinIdx);
    #endif

}

void msDrvGammaWriteTbl_1024E_12B_N(BYTE u8WinIdx, BYTE u8Color, BYTE **pu8GammaTblIdx )
{
    UNUSED(u8WinIdx);
    BYTE u8TriggerBits;
    WORD u16WriteMask;
    DWORD addrCmdPort = SC10_E0;
    DWORD addrAddressPort = SC10_E2;
    DWORD addrDataPort = SC10_E4;
    DWORD addrMaxBase = SC10_FA;

    switch (u8Color){
        case 0: {u8TriggerBits = 0;}break;
        case 1: {u8TriggerBits = BIT5;}break;
        case 2: {u8TriggerBits = BIT6;}break;
        default: {u8TriggerBits = 0;}break;
    }

    // burst write: [7] = main
    u16WriteMask = BIT7 | u8TriggerBits;

    // address port initial as 0
    msWrite2Byte( addrAddressPort, 0x00);
    // pre set value
    msWrite2Byte(addrDataPort, 0x00);

    msWriteByteMask(addrCmdPort, u16WriteMask, u16WriteMask);

    WORD u16Data, i = 0;
    for(i = 0; i < 1024; i++)
    {
        u16Data = (pu8GammaTblIdx[u8Color][i*2] + (pu8GammaTblIdx[u8Color][i*2+1]<<8))/4;
        msWrite2Byte( addrDataPort, u16Data);
    }

   //disable write & burst write cmd
    msWriteByteMask(addrCmdPort, 0x00, u16WriteMask);

    //Gamma R/G/B max base
    msWrite2Byte( addrMaxBase, 0x4000);
    msWrite2Byte(addrMaxBase + 2, 0x4000);
    msWrite2Byte( addrMaxBase + 4, 0x4000);

}

void msDrvDeGammaLoadTbl_1024E_16B_N(BYTE u8WinIdx, BYTE u8Color, BYTE **pu8DeGammaTbl)
{
    UNUSED(u8WinIdx);
    BYTE u8TriggerBits,
            u8WriteMask;
    WORD entries=1024;
    DWORD addrCmdPort = SC25_C0,
            addrMode = SC25_C1,
            addrAddressPort = SC25_C2,
            addrDataPort1 = SC25_C4,
            addrDataPort2 = SC25_C6;

    switch (u8Color){
        case 0: {u8TriggerBits = 0;}break;
        case 1: {u8TriggerBits = BIT5;}break;
        case 2: {u8TriggerBits = BIT6;}break;
        default: {u8TriggerBits = 0;}break;
    }

    // [3] = write, [7] = burst write
    u8WriteMask = BIT3 | u8TriggerBits;

    // address port initial as 0
    msWrite2Byte( addrAddressPort, 0x00);
    // pre set value
    msWrite2Byte( addrDataPort1, 0x00);
    msWriteByte( addrDataPort2, 0x00);
    //Set deGamma mode, 0: 512E, 1: 1024E
    msWriteBit(addrMode, 1, _BIT3);
    entries = 1024;//bMode?1024:512;

    WORD i,u16Data;
    // [3] = write, [7] = burst write
    u8WriteMask = BIT3 | u8TriggerBits;
    for(i = 0; i < entries; i++)
    {
        u16Data = pu8DeGammaTbl[u8Color][i*2]+(pu8DeGammaTbl[u8Color][i*2+1]<<8);
        msWrite2Byte( addrAddressPort, i);
        msWrite2Byte( addrDataPort1, u16Data);
        msWriteByte( addrCmdPort, u8WriteMask);
    }
}


void msDrvFixGammaWriteTbl_256E_12B_Extend(BYTE u8WinIdx, BYTE u8Color,  BYTE **pu8FixGammaTblIdx, BOOL bExtendMode)
{
    UNUSED(u8WinIdx);
    BYTE u8TriggerBits,
    u8ExtendModeEnable;
    WORD u16WriteMask,u16CmdPortValue,
    u16Count;
    DWORD addrCmdPort = SC25_F0,
    addrAddressPort = SC25_F2,
    addrDataPort = SC25_F4;

    switch (u8Color){
        case 0: {u8TriggerBits = 0;}break;
        case 1: {u8TriggerBits = BIT5;}break;
        case 2: {u8TriggerBits = BIT6;}break;
        default: {u8TriggerBits = 0;}break;
    }

    u8ExtendModeEnable = (bExtendMode) ? BIT11 : 0;
    u16Count = (bExtendMode) ? (256+88) : 256;

    // [3] = write, [7] = burst write, [11] = extend mode
    u16WriteMask = BIT3 |BIT5 | BIT6 | BIT7 | BIT11;
    u16CmdPortValue = BIT3 | BIT7 | u8TriggerBits | u8ExtendModeEnable;
    // address port initial as 0
    msWrite2Byte(addrAddressPort, 0x00);
    // pre set value
    msWrite2Byte(addrDataPort, 0x00);

    msWrite2ByteMask(addrCmdPort, u16CmdPortValue, u16WriteMask);

    WORD i, u16Data;
    for(i = 0; i < u16Count; i++)
    {
        u16Data = pu8FixGammaTblIdx[u8Color][i*2] + (pu8FixGammaTblIdx[u8Color][i*2+1]<<8);
        msWrite2Byte( addrDataPort, u16Data);
    }
    msWrite2Byte(SC25_E2+u8Color*2,u16Data); //max data
    //disable write & burst write cmd
    u16WriteMask = BIT3 | BIT7 | u8TriggerBits;
    msWrite2ByteMask( addrCmdPort, 0x00, u16WriteMask);
    msWriteBit( addrCmdPort + 1, bExtendMode, BIT3);

}

void msDrvFixGammaLoadTbl_256E_12B_Extend(BYTE u8WinIndex, BYTE **pu8TableIndex, BOOL bExtendMode)
{
    BYTE i;
    #if ENABLE_LUT_AUTODOWNLOAD
    msDrvClearDMAFixGammaDataBlock();
    #else
    msDrvFixGammaEnable(u8WinIndex, FALSE);
    #endif

    for (i = 0; i < 3; i++)
    #if ENABLE_LUT_AUTODOWNLOAD
        msDrvFixGammaWriteTbl_256E_12B_ByDMA(i, pu8TableIndex, bExtendMode);
        msDrvFixGamma_DMA_Reader_Trigger(u8WinIndex, bExtendMode);
    #else
        msDrvFixGammaWriteTbl_256E_12B_Extend(u8WinIndex, i, pu8TableIndex, bExtendMode);
    #endif

    msDrvFixGammaEnable(u8WinIndex, ENABLE);
    //for ( i = 0; i < DISPLAY_MAX_NUMS; i++)
    //    if (msAPIWinGetScDwEnable(i) && (ucFixGammaFunc[i] == _ENABLE))
    //        msDrvFixGammaEnable(i, TRUE);
}

void msDrvDeCompressFMT46Gamma(BYTE *pu8GammaTblIdx, DWORD* dwDecompressGamma)
{
    BYTE u8LowValue=0, u8Temp=0;
    WORD u16Data=0, u16Level=0, idx=0;

    for( u16Level=0; u16Level<310; u16Level+=5)
    {
        // Write gma data
        u8LowValue= pu8GammaTblIdx[u16Level];
        for(u8Temp=0; u8Temp<4; u8Temp++)
        {
            u16Data = pu8GammaTblIdx[u16Level+1+u8Temp];

            if (u16Level==0 && u8Temp==0)
            {
                dwDecompressGamma[u16Level/5*4+u8Temp] = ((u16Data << 2) + ((u8LowValue >> (u8Temp<<1)) & 0x03)) << 4;
            }
            else
            {
                dwDecompressGamma[u16Level/5*4+u8Temp] = (((dwDecompressGamma[u16Level/5*4+u8Temp-1]>>6) + (u16Data>>4)) << 6) + ((u16Data & 0x0F)<<2) +((u8LowValue >> (u8Temp<<1)) & 0x03);
            }
        }
    }
    // Write gma data
    u16Level = 310;
    u8LowValue= pu8GammaTblIdx[u16Level];
    for(u8Temp=0; u8Temp<8; u8Temp++)
    {
        u16Data = pu8GammaTblIdx[u16Level+1+u8Temp];
        dwDecompressGamma[u16Level/5*4+u8Temp] = (((dwDecompressGamma[u16Level/5*4+u8Temp-1]>>6) + (u16Data>>4)) << 6) + ((u16Data & 0x0F)<<2) + (((u8LowValue >> (u8Temp)) & 0x01)<<1);
    }
    u16Data = pu8GammaTblIdx[u16Level+1+u8Temp];
    dwDecompressGamma[u16Level/5*4+u8Temp] = (((dwDecompressGamma[u16Level/5*4+u8Temp-1]>>6) + (u16Data>>4)) << 6) + ((u16Data & 0x0F)<<2);

    for (idx=0; idx<256; idx++)
    {
        if (dwDecompressGamma[idx] > dwDecompressGamma[idx+1])
        {
            dwDecompressGamma[idx+1] = dwDecompressGamma[idx] ;
        }
    }

}




void msDrvDeGammaEnable(BYTE u8WinIdx, Bool bEnable)
{

    if(bEnable)
    {
        if(u8WinIdx == MAIN_WINDOW)  //Main
        {
            msWriteBit(SC25_02, TRUE, _BIT0);
        }
        else                //Sub
        {
            msWriteBit(SC25_22, TRUE, _BIT0);
        }
    }
    else
    {
        if(u8WinIdx == MAIN_WINDOW)  //Main
        {
            msWriteBit(SC25_02, FALSE, _BIT0);
        }
        else                //Sub
        {
            msWriteBit(SC25_22, FALSE, _BIT0);
        }
    }
}

void msDrvFixGammaEnable(BYTE u8WinIdx, Bool bEnable)
{
    if(bEnable)
    {
        if(u8WinIdx == MAIN_WINDOW)  //Main
        {
            msWriteBit(SC25_02, TRUE, _BIT2);
            msWriteBit(SC25_03, TRUE ,_BIT3);//Fix gamma dither
        }
        else                //Sub
        {
            msWriteBit(SC25_22, TRUE, _BIT2);
            msWriteBit(SC25_23, TRUE ,_BIT3);//Fix gamma dither
        }
    }
    else
    {
        if(u8WinIdx == MAIN_WINDOW)  //Main
        {
            msWriteBit(SC25_02, FALSE, _BIT2);
            msWriteBit(SC25_03, FALSE ,_BIT3);//Fix gamma dither
        }
        else                //Sub
        {
            msWriteBit(SC25_22, FALSE, _BIT2);
            msWriteBit(SC25_23, FALSE ,_BIT3);//Fix gamma dither
        }
    }

}

/******************************************************************************************************************************************
* Set Gamma LUT Enable/Disable
******************************************************************************************************************************************/
void msDrvGammaEnable(BYTE u8WinIdx, Bool bEnable)
{
    if(bEnable)
    {
        if(u8WinIdx == MAIN_WINDOW)  //Main
        {
            msWriteBit(SC10_A0, TRUE, _BIT0);
            msWriteBit(SC0F_AF, TRUE, _BIT4); // repeat gma table max value for interpolation
        }
        else                //Sub
        {
            msWriteBit(SC0F_30, TRUE, _BIT0);
            msWriteBit(SC0F_AF, TRUE, _BIT4); // repeat gma table max value for interpolation
        }

    }
    else
    {
        if(u8WinIdx == MAIN_WINDOW)  //Main
        {
            msWriteBit(SC10_A0, FALSE, _BIT0);
            msWriteBit(SC0F_AF, FALSE, _BIT4); // repeat gma table max value for interpolation
        }
        else                //Sub
        {
            msWriteBit(SC0F_30, FALSE, _BIT0);
            msWriteBit(SC0F_AF, FALSE, _BIT4); // repeat gma table max value for interpolation
        }

    }
}

/******************************************************************************************************************************************
* Set XVYCC Color Matrix Enable/Disable
******************************************************************************************************************************************/
void msDrvColorMatrixEnable(BYTE u8WinIdx, Bool bEnable, Bool bCarryEn)
{
    if(u8WinIdx == MAIN_WINDOW)  //Main
    {
    	msWriteBit(SC25_02, bEnable, _BIT1);
    	msWriteBit(SC25_02, bCarryEn, _BIT7);
    }
    else                //Sub
    {
    	msWriteBit(SC25_22, bEnable, _BIT1);
    	msWriteBit(SC25_22, bCarryEn, _BIT7);
	}
}

/******************************************************************************************************************************************
* Set Window Report Enable/Disable
******************************************************************************************************************************************/
void msDrvWinReportEnable(Bool bEnable)
{
    msWriteByteMask(SC0F_AF, (bEnable ? BIT0 : 0), BIT0);
}

/******************************************************************************************************************************************
* Set DisplayWindow Enable/Disable
******************************************************************************************************************************************/
void msDrvDisplayWindowEnable(Bool bEnable)
{
    msWriteByteMask(SC0F_AF, (bEnable)?_BIT1:0, _BIT1);
    msWriteByteMask(SC74_1B, (!bEnable)?_BIT7:0, _BIT7);
    msWriteByteMask(SC66_1B, (!bEnable)?_BIT7:0, _BIT7);
}

/******************************************************************************************************************************************
* Set WinReport Range
* Note. u16HStart have to > 0
******************************************************************************************************************************************/
void msDrvWinReportSetRange(WORD u16HStart, WORD u16HEnd, WORD u16VStart, WORD u16VEnd)
{
	msWrite2Byte(SC0F_88, u16VStart );
	msWrite2Byte(SC0F_8A, u16VEnd);
	msWrite2Byte(SC0F_84, u16HStart);
	msWrite2Byte(SC0F_86, u16HEnd );
}

/******************************************************************************************************************************************
* Get WinReport Weighting
* Current report stage is fixed on "C: Post_CON_BRI output"
* report stage selection(0: VOP2_dp input; 8: Pre_CON_BRI output;C: Post_CON_BRI output;E: OSD_Blending output)
******************************************************************************************************************************************/
void msDrvWinReportGetWeightingPixelCountBySetRange(WORD *Rweight, WORD *Gweight, WORD *Bweight)
{
	DWORD u32pixelcount, u32weighting;  
	msWriteByteMask(SC0F_AF, BIT0, BIT0);
	msWriteByteMask(SC0F_8C, 0x0C, 0x0E);   //Stage select      
	u32pixelcount = (msRead2Byte(SC0F_86)- msRead2Byte(SC0F_84) + 1) * (msRead2Byte(SC0F_8A) -  msRead2Byte(SC0F_88) + 1);
	u32weighting = msRead4Byte(SC2D_02);
	*Rweight = (u32pixelcount>0)?(u32weighting /u32pixelcount):0;
	u32weighting = msRead4Byte(SC2D_02+4);
	*Gweight = (u32pixelcount>0)?(u32weighting /u32pixelcount):0;
	u32weighting = msRead4Byte(SC2D_02+8);
	*Bweight = (u32pixelcount>0)?(u32weighting /u32pixelcount):0;
}

/******************************************************************************************************************************************
* swap PQGA OSDB LD position
******************************************************************************************************************************************/
void msPQGA_OSDB_LD_PositionSwitch(BYTE position)
{
    /*
    0: PQGA ->OSDB -> LD
    1: PQGA -> LD -> OSDB
    2: OSDB -> PQGA -> LD
    3: OSDB -> LD -> PQGA
    4: LD -> PQGA -> OSDB
    5: LD -> OSDB -> PQGA
    */
    if(position > 5)
        position = 5;
    msWriteByteMask(SC0D_7C, position, BIT0|BIT1|BIT2);
}

void msSetVIPBypass(BOOL bIsBypass)
{
	if(bIsBypass)
	{
		msWrite2ByteMask(SC07_20, BIT7, BIT7);
	}
	else
	{
		msWrite2ByteMask(SC07_20, 0, BIT7);
	}
}

void msSetVOP2Bypass(BOOL bIsBypass)
{
	if(bIsBypass)
	{
		msWrite2ByteMask(SC66_3E, BIT0, BIT0);
	}
	else
	{
		msWrite2ByteMask(SC66_3E, 0, BIT0);
	}
}

void msSetPQGABypass(BOOL bIsBypass)
{
	if(bIsBypass)
	{
		msWrite2ByteMask(SC74_3E, BIT0, BIT0);
	}
	else
	{
		msWrite2ByteMask(SC74_3E, 0, BIT0);
	}
}

static int _numOfDrvDSCmds = 0;
static void  _mdrv_Adjust_DSRegInfo_Get(DWORD u32Addr, BOOL *canUpdateByDS, BOOL *IsRegAtIP)
{
    BYTE u8BankNum =  u32Addr >> 24;
    BYTE u8Addr = u32Addr & 0xFF;   
    *IsRegAtIP = FALSE;
 
    switch(u8BankNum)
    {
        case 0x0B:
        case 0x22:
        case 0x67:
        case 0x79:
        case 0x7A:
        case 0x7B:
        case 0x7C:
        case 0x7D:
        case 0x7E:    
        {
            *IsRegAtIP = TRUE;
        }break;
    }

    *canUpdateByDS = FALSE;
    switch(u8BankNum)
    {
        case 0x22:
        case 0x0F:
        case 0x10:
        case 0x24:
        case 0x25:
        case 0x4C:
        case 0x67:
        case 0x79:
        case 0x7A:
        case 0x7B:
        //case 0x7C:
        case 0x7E:
        {
            *canUpdateByDS = TRUE;
        }break;
    }

    //XVYCC ADL settigns can referance to sc mask out.
    if (u8BankNum == 0x67)
    {
        if ((u8Addr >= 0x22) && (u8Addr <= 0x40))//client4/5/6
        {
             IsRegAtIP = FALSE;
        }
    }
    UNUSED(u8Addr);   
}

void mdrv_Adjust_DS_Start(void)
{
    DS_START();
}

void mdrv_Adjust_DS_End(void)
{
    int nDS_Start = MsOS_GetSystemTime();
    DS_END();
    int nDS_END = MsOS_GetSystemTime();
    drvAdjust_PRINT("msDrvDS_End, Num Of Cmds: %d\n", _numOfDrvDSCmds);
    drvAdjust_PRINT("CFD_DS_Fire  DS take = %d\n", nDS_END - nDS_Start);
    _numOfDrvDSCmds = 0;
}

void mdrv_Adjust_DS_W2BMask(DWORD u32Address, WORD u16Data, WORD u16Mask)
{
    static BOOL canUpdateByDS = FALSE;
    static BOOL IsRegAtIP = FALSE;
    _mdrv_Adjust_DSRegInfo_Get(u32Address, &canUpdateByDS, &IsRegAtIP);

    if (TRUE == canUpdateByDS)
    {
        if (TRUE == IsRegAtIP)
        {
            DS_W2BMask(u32Address, u16Data, u16Mask, DS_IP);
        }
        else
        {
            DS_W2BMask(u32Address, u16Data, u16Mask, DS_OP);
        }
        _numOfDrvDSCmds++;
    }
    else
    {
        msWrite2ByteMask(u32Address, u16Data, u16Mask);
    }
}

void mdrv_Adjust_DS_WBMask(DWORD u32Address, BYTE u8Data, BYTE u8Mask)
{
    static BOOL canUpdateByDS = FALSE;
    static BOOL IsRegAtIP = FALSE;
    _mdrv_Adjust_DSRegInfo_Get(u32Address, &canUpdateByDS, &IsRegAtIP);

    if (TRUE == canUpdateByDS)
    {
        if (TRUE == IsRegAtIP)
        {
            DS_WBMask(u32Address, u8Data, u8Mask, DS_IP);
        }
        else
        {
            DS_WBMask(u32Address, u8Data, u8Mask, DS_OP);
        }
        _numOfDrvDSCmds++;
    }
    else
    {
        msWriteByteMask(u32Address, u8Data, u8Mask);
    }

}

void mdrv_Adjust_DS_W4B(DWORD u32Address, DWORD u32Value)
{
    static BOOL canUpdateByDS = FALSE;
    static BOOL IsRegAtIP = FALSE;
    _mdrv_Adjust_DSRegInfo_Get(u32Address, &canUpdateByDS, &IsRegAtIP);

    if (TRUE == canUpdateByDS)
    {
        if (TRUE == IsRegAtIP)
        {
            DS_W4B(u32Address, u32Value, DS_IP);
        }
        else
        {
            DS_W2B(u32Address, u32Value, DS_OP);
        }
        _numOfDrvDSCmds++;
    }
    else
    {
        msWrite4Byte(u32Address, u32Value);
    }

}

void mdrv_Adjust_DS_W2B(DWORD u32Address, WORD u16Data)
{
    static BOOL canUpdateByDS = FALSE;
    static BOOL IsRegAtIP = FALSE;
    _mdrv_Adjust_DSRegInfo_Get(u32Address, &canUpdateByDS, &IsRegAtIP);

    if (TRUE == canUpdateByDS)
    {
        if (TRUE == IsRegAtIP)
        {
            DS_W2B(u32Address, u16Data, DS_IP);
        }
        else
        {
            DS_W2B(u32Address, u16Data, DS_OP);
        }
        _numOfDrvDSCmds++;
    }
    else
    {
        msWrite2Byte(u32Address, u16Data);
    }

}

void mdrv_Adjust_DS_WB(DWORD u32Address, BYTE u8Data)
{
    static BOOL canUpdateByDS = FALSE;
    static BOOL IsRegAtIP = FALSE;
    _mdrv_Adjust_DSRegInfo_Get(u32Address, &canUpdateByDS, &IsRegAtIP);

    if (TRUE == canUpdateByDS)
    {
        if (TRUE == IsRegAtIP)
        {
            DS_WB(u32Address, u8Data, DS_IP);
        }
        else
        {
            DS_WB(u32Address, u8Data, DS_OP);
        }
        _numOfDrvDSCmds++;
    }
    else
    {
        msWriteByte(u32Address, u8Data);
    }

}

void mdrv_Adjust_DS_WBit(DWORD u32Address, Bool bBit, BYTE u8BitPos)
{
    static BOOL canUpdateByDS = FALSE;
    static BOOL IsRegAtIP = FALSE;
    _mdrv_Adjust_DSRegInfo_Get(u32Address, &canUpdateByDS, &IsRegAtIP);

    if (TRUE == canUpdateByDS)
    {
        if (TRUE == IsRegAtIP)
        {
            DS_WBit(u32Address, bBit, u8BitPos, DS_IP);
        }
        else
        {
            DS_WBit(u32Address, bBit, u8BitPos, DS_OP);
        }
        _numOfDrvDSCmds++;
    }
    else
    {
        msWriteBit(u32Address, bBit, u8BitPos);
    }

}
#if ENABLE_SUPER_RESOLUTION
BYTE xdata SRmodeContent = SRMODE_Nums;

code RegUnitType tSR_Mode_OFF_Tbl[]=
{
	{SC0B_20, 0x81},
 { SC0B_60 ,  0x00    },
	{SC0B_21, 0x93},
	{SC0B_22, 0x57},
	//{SC0B_27, 0x30},
	#if (CHIP_ID == CHIP_MT9701)
	{SC0B_30, 0x10}, // Gain in sharpness function
	{SC0B_31, 0x10}, // Gain in sharpness function
	#else
	{SC0B_30, 0x08}, // Gain in sharpness function
	{SC0B_31, 0x08}, // Gain in sharpness function
	#endif
	{SC0B_33, 0x08},
	{SC0B_34, 0x08},
	{SC0B_37, 0x08},
	{SC0B_38, 0x10},
	{SC0B_39, 0x32},
	{SC0B_3A, 0x54},
	{SC0B_3B, 0x76},
	{SC0B_3C, 0x10},
	{SC0B_3D, 0x32},
	{SC0B_3E, 0x54},
	{SC0B_3F, 0x76},
 { SC0B_62 ,  0x00    },
 { SC0B_64 ,  0xFF    },
 { SC0B_80 ,  0x15    },
 { SC0B_81 ,  0x15    },
 { SC0B_82 ,  0x10    },
 { SC0B_83 ,  0x10    },
 { SC0B_84 ,  0x15    },
 { SC0B_85 ,  0x15    },
 { SC0B_86 ,  0x20    },
 { SC0B_87 ,  0x20    },
 { SC0B_88 ,  0x20    },
 { SC0B_89 ,  0x20    },
 { SC0B_8A ,  0x20    },
 { SC0B_8B ,  0x20    },
 { SC0B_8C ,  0x20    },
 { SC0B_8D ,  0x20    },
 { SC0B_8E ,  0x20    },
 { SC0B_8F ,  0x20    },
 { SC0B_40 ,  0xFF    },
 { SC0B_41 ,  0xFF    },
 { SC0B_42 ,  0xFF    },
 { SC0B_43 ,  0xFF    },
	{SC0B_44, 0xFF},
	{SC0B_45, 0xFF},
	{SC0B_46, 0xFF},
	{SC0B_47, 0xFF},
	{SC0B_48, 0xFF},
	{SC0B_49, 0xFF},
	{SC0B_4A, 0xFF},
 { SC0B_4B ,  0xFF    },
	{SC0B_4C, 0xFF},
	{SC0B_4D, 0xFF},
	{SC0B_4E, 0xFF},
	{SC0B_4F, 0xFF},
 { SC0B_A1 ,  0x00    },
	{SC0B_00, 0x00},
	{SC0B_10, 0x02},
	{SC0B_14, 0x39},
	{SC0B_00, 0x0B},
 { SC0B_25 ,  0x00    },
 { SC0B_23 ,  0xFF    },
 { SC0B_65 ,  0xEF    },
 { SC0B_90 ,  0x1C    },
 { SC0B_91 ,  0x1C    },
 { SC0B_92 ,  0x20    },
 { SC0B_93 ,  0x20    },
 { SC0B_94 ,  0x24    },
 { SC0B_95 ,  0x24    },
 { SC0B_96 ,  0x28    },
 { SC0B_97 ,  0x28    },
 { SC0B_98 ,  0x24    },
 { SC0B_99 ,  0x24    },
 { SC0B_9A ,  0x20    },
 { SC0B_9B ,  0x20    },
 { SC0B_9C ,  0x18    },
 { SC0B_9D ,  0x18    },
 { SC0B_9E ,  0x10    },
 { SC0B_9F ,  0x10    },
	{SC0B_26, 0xAA},
	{SC0B_61, 0x03},
	{SC0B_68, 0x90},
	{SC0B_69, 0x09},
	{SC0B_C0, 0x84},
	{SC0B_C2, 0xFF},
 { SC0B_E0 ,  0x00    },
 { SC0B_E4 ,  0x00    },
 { SC0B_E5 ,  0x00    },
 { SC0B_E2 ,  0x00    },
	{SC0B_C1, 0x84},
	{SC0B_C3, 0xFF},
 { SC0B_EA ,  0x00    },
 { SC0B_EE ,  0x00    },
 { SC0B_EF ,  0x00    },
 { SC0B_EC ,  0x00    },
	{SC0B_24, 0x84},
 { SC0B_72 ,  0x00    },
 { SC0B_F4 ,  0x00    },
 { SC0B_F8 ,  0x00    },
 { SC0B_F9 ,  0x00    },
 { SC0B_F6 ,  0x00    },
};
code RegUnitType tSR_Mode_H_Tbl[]=
{
 { SC0B_20 	,	  0x81 	},
 { SC0B_60 ,  0x00    },
	{SC0B_21, 0x93},
 { SC0B_22 ,  0x57    },
#if (CHIP_ID != CHIP_MT9701)
	{SC0B_30, 0x10}, // Gain in sharpness function
	{SC0B_31, 0x10}, // Gain in sharpness function
	{SC0B_34, 0x1B},
	{SC0B_37, 0x1B},
	{SC0B_27, 0x30},
 { SC0B_33 ,  0x10    },
#else
	{SC0B_30, 0x2F}, // Gain in sharpness function
	{SC0B_31, 0x2F}, // Gain in sharpness function
	{SC0B_34, 0x2F},
	{SC0B_37, 0x2F},
 { SC0B_33 ,  0x18    },
#endif
	{SC0B_2F, 0x30},
 { SC0B_62 ,  0x9F    },
 { SC0B_64 ,  0xFF    },
 { SC0B_80 ,  0x15    },
 { SC0B_81 ,  0x15    },
 { SC0B_82 ,  0x10    },
 { SC0B_83 ,  0x10    },
 { SC0B_84 ,  0x15    },
 { SC0B_85 ,  0x15    },
 { SC0B_86 ,  0x20    },
 { SC0B_87 ,  0x20    },
 { SC0B_88 ,  0x20    },
 { SC0B_89 ,  0x20    },
 { SC0B_8A ,  0x20    },
 { SC0B_8B ,  0x20    },
 { SC0B_8C ,  0x20    },
 { SC0B_8D ,  0x20    },
 { SC0B_8E ,  0x20    },
 { SC0B_8F ,  0x20    },
	{SC0B_40, 0x7F},
	{SC0B_48, 0x7F},
	{SC0B_41, 0x7F},
	{SC0B_49, 0x7F},
 { SC0B_42 ,  0x20    },
 { SC0B_4A ,  0x20    },
 { SC0B_43 ,  0x20    },
 { SC0B_4B ,  0x20    },
 { SC0B_38 ,  0x10    },
 { SC0B_39 ,  0x32    },
 { SC0B_3C ,  0x10    },
 { SC0B_3D ,  0x32    },
 { SC0B_A1 ,  0x00    },
	{SC0B_00, 0x00},
	{SC0B_10, 0x02},
	{SC0B_14, 0x39},
	{SC0B_00, 0x0B},
 { SC0B_25 ,  0x00    },
 { SC0B_23 ,  0xFF    },
 { SC0B_65 ,  0xEF    },
 { SC0B_90 ,  0x1C    },
 { SC0B_91 ,  0x1C    },
 { SC0B_92 ,  0x20    },
 { SC0B_93 ,  0x20    },
 { SC0B_94 ,  0x24    },
 { SC0B_95 ,  0x24    },
 { SC0B_96 ,  0x28    },
 { SC0B_97 ,  0x28    },
 { SC0B_98 ,  0x24    },
 { SC0B_99 ,  0x24    },
 { SC0B_9A ,  0x20    },
 { SC0B_9B ,  0x20    },
 { SC0B_9C ,  0x18    },
 { SC0B_9D ,  0x18    },
 { SC0B_9E ,  0x10    },
 { SC0B_9F ,  0x10    },
 { SC0B_44 ,  0x25    },
 { SC0B_4C ,  0x25    },
 { SC0B_47 ,  0x25    },
 { SC0B_4F ,  0x25    },
 { SC0B_3A ,  0x54    },
 { SC0B_3B ,  0x76    },
 { SC0B_3E ,  0x54    },
 { SC0B_3F ,  0x76    },
 { SC0B_26 ,  0xAA    },
 { SC0B_61 ,  0x03    },
 { SC0B_68 ,  0x90    },
 { SC0B_69 ,  0x09    },
 { SC0B_45 ,  0x35    },
 { SC0B_4D ,  0x35    },
 { SC0B_46 ,  0x35    },
 { SC0B_4E ,  0x35    },
 { SC0B_C0 ,  0x84    },
 { SC0B_C2 ,  0xFF    },
 { SC0B_E0 ,  0x00    },
 { SC0B_E4 ,  0x00    },
 { SC0B_E5 ,  0x00    },
 { SC0B_E2 ,  0x00    },
 { SC0B_C1 ,  0x84    },
 { SC0B_C3 ,  0xFF    },
 { SC0B_EA ,  0x00    },
 { SC0B_EE ,  0x00    },
 { SC0B_EF ,  0x00    },
 { SC0B_EC ,  0x00    },
 { SC0B_24 ,  0x84    },
 { SC0B_72 ,  0x00    },
 { SC0B_F4 ,  0x00    },
 { SC0B_F8 ,  0x00    },
 { SC0B_F9 ,  0x00    },
 { SC0B_F6 ,  0x00    },
};

code RegUnitType tSR_Mode_M_Tbl[]=
{
 { SC0B_20 	,	  0x81 	},
 { SC0B_60 ,  0x00    },
	{SC0B_21, 0x93},
 { SC0B_22 ,  0x57    },
#if (CHIP_ID != CHIP_MT9701)
	{SC0B_30, 0x10}, // Gain in sharpness function
	{SC0B_31, 0x10}, // Gain in sharpness function
	{SC0B_34, 0x1B},
	{SC0B_37, 0x1B},
	{SC0B_27, 0x20},
 { SC0B_33 ,  0x10    },
#else
	{SC0B_30, 0x18}, // Gain in sharpness function
	{SC0B_31, 0x18}, // Gain in sharpness function
	{SC0B_34, 0x18},
	{SC0B_37, 0x18},
 { SC0B_33 ,  0x15    },
#endif
	{SC0B_2F, 0x20},
 { SC0B_62 ,  0x9F    },
 { SC0B_64 ,  0xFF    },
 { SC0B_80 ,  0x15    },
 { SC0B_81 ,  0x15    },
 { SC0B_82 ,  0x10    },
 { SC0B_83 ,  0x10    },
 { SC0B_84 ,  0x15    },
 { SC0B_85 ,  0x15    },
 { SC0B_86 ,  0x20    },
 { SC0B_87 ,  0x20    },
 { SC0B_88 ,  0x20    },
 { SC0B_89 ,  0x20    },
 { SC0B_8A ,  0x20    },
 { SC0B_8B ,  0x20    },
 { SC0B_8C ,  0x20    },
 { SC0B_8D ,  0x20    },
 { SC0B_8E ,  0x20    },
 { SC0B_8F ,  0x20    },
	{SC0B_40, 0x7F},
	{SC0B_48, 0x7F},
	{SC0B_41, 0x7F},
	{SC0B_49, 0x7F},
 { SC0B_42 ,  0x20    },
 { SC0B_4A ,  0x20    },
 { SC0B_43 ,  0x20    },
 { SC0B_4B ,  0x20    },
 { SC0B_38 ,  0x10    },
 { SC0B_39 ,  0x32    },
 { SC0B_3C ,  0x10    },
 { SC0B_3D ,  0x32    },
 { SC0B_A1 ,  0x00    },
	{SC0B_00, 0x00},
	{SC0B_10, 0x02},
	{SC0B_14, 0x39},
	{SC0B_00, 0x0B},
 { SC0B_25 ,  0x00    },
 { SC0B_23 ,  0xFF    },
 { SC0B_65 ,  0xEF    },
 { SC0B_90 ,  0x1C    },
 { SC0B_91 ,  0x1C    },
 { SC0B_92 ,  0x20    },
 { SC0B_93 ,  0x20    },
 { SC0B_94 ,  0x24    },
 { SC0B_95 ,  0x24    },
 { SC0B_96 ,  0x28    },
 { SC0B_97 ,  0x28    },
 { SC0B_98 ,  0x24    },
 { SC0B_99 ,  0x24    },
 { SC0B_9A ,  0x20    },
 { SC0B_9B ,  0x20    },
 { SC0B_9C ,  0x18    },
 { SC0B_9D ,  0x18    },
 { SC0B_9E ,  0x10    },
 { SC0B_9F ,  0x10    },
	{SC0B_44, 0x25},
	{SC0B_4C, 0x25},
	{SC0B_47, 0x25},
	{SC0B_4F, 0x25},
 { SC0B_3A ,  0x54    },
 { SC0B_3B ,  0x76    },
 { SC0B_3E ,  0x54    },
 { SC0B_3F ,  0x76    },
	{SC0B_26, 0xAA},
 { SC0B_61 ,  0x03    },
	{SC0B_68, 0x90},
	{SC0B_69, 0x09},
	{SC0B_45, 0x35},
	{SC0B_4D, 0x35},
	{SC0B_46, 0x35},
	{SC0B_4E, 0x35},
 { SC0B_C0 ,  0x84    },
 { SC0B_C2 ,  0xFF    },
 { SC0B_E0 ,  0x00    },
 { SC0B_E4 ,  0x00    },
 { SC0B_E5 ,  0x00    },
 { SC0B_E2 ,  0x00    },
 { SC0B_C1 ,  0x84    },
 { SC0B_C3 ,  0xFF    },
 { SC0B_EA ,  0x00    },
 { SC0B_EE ,  0x00    },
 { SC0B_EF ,  0x00    },
 { SC0B_EC ,  0x00    },
 { SC0B_24 ,  0x84    },
 { SC0B_72 ,  0x00    },
 { SC0B_F4 ,  0x00    },
 { SC0B_F8 ,  0x00    },
 { SC0B_F9 ,  0x00    },
 { SC0B_F6 ,  0x00    },
};
code RegUnitType tSR_Mode_L_Tbl[]=
{
 { SC0B_20 	,	  0x81 	},
 { SC0B_60 ,  0x00    },
	{SC0B_21, 0x93},
 { SC0B_22 ,  0x57    },
#if (CHIP_ID != CHIP_MT9701)
	{SC0B_30, 0x10}, // Gain in sharpness function
	{SC0B_31, 0x10}, // Gain in sharpness function
	{SC0B_34, 0x1B},
	{SC0B_37, 0x1B},
	{SC0B_27, 0x15},
 { SC0B_33 ,  0x10    },
#else
	{SC0B_30, 0x12}, // Gain in sharpness function
	{SC0B_31, 0x12}, // Gain in sharpness function
	{SC0B_34, 0x12},
	{SC0B_37, 0x12},
 { SC0B_33 ,  0x12    },
#endif
	{SC0B_2F, 0x15},
 { SC0B_62 ,  0x9F    },
 { SC0B_64 ,  0xFF    },
 { SC0B_80 ,  0x15    },
 { SC0B_81 ,  0x15    },
 { SC0B_82 ,  0x10    },
 { SC0B_83 ,  0x10    },
 { SC0B_84 ,  0x15    },
 { SC0B_85 ,  0x15    },
 { SC0B_86 ,  0x20    },
 { SC0B_87 ,  0x20    },
 { SC0B_88 ,  0x20    },
 { SC0B_89 ,  0x20    },
 { SC0B_8A ,  0x20    },
 { SC0B_8B ,  0x20    },
 { SC0B_8C ,  0x20    },
 { SC0B_8D ,  0x20    },
 { SC0B_8E ,  0x20    },
 { SC0B_8F ,  0x20    },
	{SC0B_40, 0x7F},
	{SC0B_48, 0x7F},
	{SC0B_41, 0x7F},
	{SC0B_49, 0x7F},
 { SC0B_42 ,  0x20    },
 { SC0B_4A ,  0x20    },
 { SC0B_43 ,  0x20    },
 { SC0B_4B ,  0x20    },
 { SC0B_38 ,  0x10    },
 { SC0B_39 ,  0x32    },
 { SC0B_3C ,  0x10    },
 { SC0B_3D ,  0x32    },
 { SC0B_A1 ,  0x00    },
	{SC0B_00, 0x00},
	{SC0B_10, 0x02},
	{SC0B_14, 0x39},
	{SC0B_00, 0x0B},
 { SC0B_25 ,  0x00    },
 { SC0B_23 ,  0xFF    },
 { SC0B_65 ,  0xEF    },
 { SC0B_90 ,  0x1C    },
 { SC0B_91 ,  0x1C    },
 { SC0B_92 ,  0x20    },
 { SC0B_93 ,  0x20    },
 { SC0B_94 ,  0x24    },
 { SC0B_95 ,  0x24    },
 { SC0B_96 ,  0x28    },
 { SC0B_97 ,  0x28    },
 { SC0B_98 ,  0x24    },
 { SC0B_99 ,  0x24    },
 { SC0B_9A ,  0x20    },
 { SC0B_9B ,  0x20    },
 { SC0B_9C ,  0x18    },
 { SC0B_9D ,  0x18    },
 { SC0B_9E ,  0x10    },
 { SC0B_9F ,  0x10    },
	{SC0B_44, 0x25},
	{SC0B_4C, 0x25},
	{SC0B_47, 0x25},
	{SC0B_4F, 0x25},
 { SC0B_3A ,  0x54    },
 { SC0B_3B ,  0x76    },
 { SC0B_3E ,  0x54    },
 { SC0B_3F ,  0x76    },
	{SC0B_26, 0xAA},
 { SC0B_61 ,  0x03    },
	{SC0B_68, 0x90},
	{SC0B_69, 0x09},
	{SC0B_45, 0x35},
	{SC0B_4D, 0x35},
	{SC0B_46, 0x35},
	{SC0B_4E, 0x35},
 { SC0B_C0 ,  0x84    },
 { SC0B_C2 ,  0xFF    },
 { SC0B_E0 ,  0x00    },
 { SC0B_E4 ,  0x00    },
 { SC0B_E5 ,  0x00    },
 { SC0B_E2 ,  0x00    },
 { SC0B_C1 ,  0x84    },
 { SC0B_C3 ,  0xFF    },
 { SC0B_EA ,  0x00    },
 { SC0B_EE ,  0x00    },
 { SC0B_EF ,  0x00    },
 { SC0B_EC ,  0x00    },
 { SC0B_24 ,  0x84    },
 { SC0B_72 ,  0x00    },
 { SC0B_F4 ,  0x00    },
 { SC0B_F8 ,  0x00 },
 { SC0B_F9 ,  0x00 },
 { SC0B_F6 ,  0x00 },
};

void msDrv_SuperResolutionOnOff(BOOL bOn)
{
	BYTE LOCAL_TYPE ucBank;
	ucBank = scReadByte(SC00_00);
	MENU_LOAD_START(); 
	DB_WB(SC00_00, 0x0B);//msWriteByte(SC00_00, 0x0B);
	DB_WBMask(SC0B_20,  (bOn)?(BIT0):(0), BIT0);//msWriteByteMask(SC0B_20,  (bOn)?(BIT0):(0), BIT0);  //BIT0:enable
	DB_WB(SC00_00, ucBank);//msWriteByte(SC00_00, ucBank);
	MENU_LOAD_END(); 
}

void msDrv_InitalSRSetting(void)
{
	msWriteByteMask(SC0B_20, BIT7, BIT7);
	msWriteByteMask(SC0B_28, BIT1, BIT1);
}

void msDrv_SetupSuperResolution( BYTE SuperResolutionMode )
{
    BYTE i=0;

	if(SuperResolutionMode == SRMODE_OFF) 
  	msDrv_SuperResolutionOnOff(0);//msSuperResolutionOnOff(0);
   else
   	msDrv_SuperResolutionOnOff(1);//msSuperResolutionOnOff(1);

    MENU_LOAD_START(); 
    switch(SuperResolutionMode)
    {
        case SRMODE_STRONG:
	    
        drvAdjust_PRINT("tSR_Mode_H_Tbl");//MenuFunc_printMsg("tSR_Mode_H_Tbl");
        for(i=0;i<sizeof(tSR_Mode_H_Tbl)/sizeof(RegUnitType);i++)
            DB_WB(tSR_Mode_H_Tbl[i].u16Reg,tSR_Mode_H_Tbl[i].u8Value);//msWriteByte(tSR_Mode_H_Tbl[i].u16Reg,tSR_Mode_H_Tbl[i].u8Value);
            break;

        case SRMODE_MIDDLE :
        drvAdjust_PRINT("tSR_Mode_M_Tbl");//MenuFunc_printMsg("tSR_Mode_M_Tbl");
        for(i=0;i<sizeof(tSR_Mode_M_Tbl)/sizeof(RegUnitType);i++)
            DB_WB(tSR_Mode_M_Tbl[i].u16Reg,tSR_Mode_M_Tbl[i].u8Value);//msWriteByte(tSR_Mode_M_Tbl[i].u16Reg,tSR_Mode_M_Tbl[i].u8Value);
            break;

        case SRMODE_WEAK:
        drvAdjust_PRINT("tSR_Mode_L_Tbl");//MenuFunc_printMsg("tSR_Mode_L_Tbl");
        for(i=0;i<sizeof(tSR_Mode_L_Tbl)/sizeof(RegUnitType);i++)
            DB_WB(tSR_Mode_L_Tbl[i].u16Reg,tSR_Mode_L_Tbl[i].u8Value);//msWriteByte(tSR_Mode_L_Tbl[i].u16Reg,tSR_Mode_L_Tbl[i].u8Value);
            break;

        default:
        drvAdjust_PRINT("tSR_Mode_OFF_Tbl");//MenuFunc_printData("tSR_Mode_OFF_Tbl",0);
        for(i=0;i<sizeof(tSR_Mode_OFF_Tbl)/sizeof(RegUnitType);i++)
            DB_WB(tSR_Mode_OFF_Tbl[i].u16Reg,tSR_Mode_OFF_Tbl[i].u8Value);//msWriteByte(tSR_Mode_OFF_Tbl[i].u16Reg,tSR_Mode_OFF_Tbl[i].u8Value);
            break;
    }
     MENU_LOAD_END(); 
    SRmodeContent = SuperResolutionMode;
}
#endif

Bool msDrv_SetColorModeDemo(void)
{
    //printf("Color Mode: Demo\n");
    WORD u16VDEStart,
         u16VDEEnd,
         u16HDEStart,
         u16HDEEnd;

    // Main win: Enhance, Sub win: OFF
    mStar_AdjustBrightness(UserPrefBrightness);
    mStar_AdjustContrast(UserPrefContrast);


    mdrv_ACE_MweEnable_Set(TRUE);// enable MWE function

    //msWriteBit(SC00_27, TRUE, _BIT7); // enable auto ouput V sync
    msWriteBit(SC0B_20, TRUE, _BIT7);// 2D peaking line-buffer sram active
    msWriteBit(SC0B_28, TRUE, _BIT1);// 2D peaking line-buffer sram active for sub


    u16VDEStart = msRead2Byte(SC00_10);
    u16HDEStart = msRead2Byte(SC00_12);
    u16VDEEnd = msRead2Byte(SC00_14);
    u16HDEEnd = msRead2Byte(SC00_16);

    msWrite2Byte(SC03_1E, u16VDEStart-2); //VDE Start
    //msWrite2Byte(SC03_18, u16HDEStart + (u16HDEEnd*0.5)); //HDE Start
    msWrite2Byte(SC03_18, u16HDEStart + ((u16HDEEnd-u16HDEStart)*0.5)); //HDE Start
    msWrite2Byte(SC03_1A, u16VDEEnd-2); //VDE End
    msWrite2Byte(SC03_1C, u16HDEEnd); //HDE End

    mStar_WaitForDataBlanking();
    drvGammaOnOff(_ENABLE, MAIN_WINDOW );
	drvGammaOnOff(_DISABLE, SUB_WINDOW);

    //2023/02/23, remove to msACECSCControl()
    //msWriteByteMask( SC0F_AE, BIT7, BIT7);
    return TRUE;
}
void mdrv_Adjust_EnableNonStdCSC_Set(BOOL u8Enable)
{
     _IsEnNonStdCSC = u8Enable;
}

BOOL mdrv_Adjust_EnableNonStdCSC_Get(void)
{
     return _IsEnNonStdCSC;
}
