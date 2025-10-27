
#include <string.h>
#include "Global.h"
#include "msACE.h"
#include "AutoGamma.h"
#include "drvGamma.h"
extern ST_COMBO_COLOR_FORMAT GetInputCombColorFormat(void);
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
		msWriteByteMask(SC14_02, (bEnable ? BIT0: 0),BIT0);
	}
	else
	{
        msWriteByteMask(SC14_03, (bEnable ? BIT0: 0),BIT0);
	}
}

void msDrvAdjustCustomHue(BYTE u8Value, BYTE u8ColorIndex)
{
    msWriteByte(SC14_0E + u8ColorIndex, (u8Value & 0x7F));
}

/******************************************************************************************************************************************
* Custom Saturation R/G/B/C/M/Y for RGB
******************************************************************************************************************************************/
void msDrvCustomSatEnable(BYTE u8WinIdx, Bool bEnable)
{
    if(u8WinIdx == MAIN_WINDOW)
	{
		msWriteByteMask(SC14_02, (bEnable ? BIT4: 0),BIT4);
	}
	else
	{
        msWriteByteMask(SC14_03, (bEnable ? BIT4: 0),BIT4);
	}
}

void msDrvAdjustCustomSat(BYTE u8Value, BYTE u8ColorIndex)
{
    msWriteByte(SC14_14 + u8ColorIndex, (u8Value & 0x7F));
}

/******************************************************************************************************************************************
* Custom Brightness R/G/B/C/M/Y for RGB
******************************************************************************************************************************************/
void msDrvCustomBriEnable(BYTE u8WinIdx, Bool bEnable)
{
    if(u8WinIdx == MAIN_WINDOW)
	{
		msWriteByteMask(SC14_02, (bEnable ? BIT5: 0),BIT5);
	}
	else
	{
        msWriteByteMask(SC14_03, (bEnable ? BIT5: 0),BIT5);
	}
}

void msDrvAdjustCustomBri(BYTE u8Value, BYTE u8ColorIndex)
{
    msWriteByte(SC14_1A + u8ColorIndex, (u8Value & 0x7F));
}

/*void msDrvClearDMAGammaDataBlock(void)
{
    DWORD u32Gamma_write_ptr = AUTOLOAD_GMA_ADR;
    DWORD idx;
    for( idx=0; idx<AUTOLOAD_GMA_LEN; idx++)
    {
        msMemWriteByte(u32Gamma_write_ptr + idx, 0x00);
    }
}*/
#if ENABLE_LUT_AUTODOWNLOAD
void msDrvDeGammaWriteTbl(BYTE u8WinIdx, BYTE u8Color, BYTE **pu8GammaTblIdx )
{
	UNUSED(u8WinIdx);
	UNUSED(u8Color);
	BYTE* pu8Tab1;
	XDATA BYTE u8TgtChannel = 0;
	IMI_Init(FIXGAMMA_ADL_ADDR, FIXGAMMA_ADL_SIZE);

	while( u8TgtChannel < 3 )
	{
		pu8Tab1 = (BYTE*)((void*)pu8GammaTblIdx[0]);
		WriteDeGamma2Dram(u8TgtChannel, pu8Tab1);
		u8TgtChannel += 1;
	}
}

void msDrvFixGammaWriteTbl(BYTE u8WinIdx, BYTE u8Color, BYTE **pu8GammaTblIdx )
{
    UNUSED(u8WinIdx);
    UNUSED(u8Color);
    BYTE* pu8Tab1;
    XDATA BYTE u8TgtChannel = 0;
    IMI_Init(FIXGAMMA_ADL_ADDR, FIXGAMMA_ADL_SIZE);

    while( u8TgtChannel < 3 )
    {
        pu8Tab1 = (BYTE*)((void*)pu8GammaTblIdx[0]);
#if XVYCC_GAMMA_EXT
        WriteFixGamma2Dram(u8TgtChannel, pu8Tab1, TRUE);
#else
        WriteFixGamma2Dram(u8TgtChannel, pu8Tab1, FALSE);
#endif
        u8TgtChannel += 1;
    }
}
#endif
void msDrvDeGammaLoadTbl_256E_16B_N(BYTE u8WinIdx, BYTE u8Color, BYTE **pu8DeGammaTbl)
{
    UNUSED(u8WinIdx);
    //DWORD u32BaseAddr;
    //WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    WORD u16Data, i, j;
    BYTE u8TriggerBits;
    //u32BaseAddr = SCRegBase(MapWin2ScIndex(u8WinIdx));
    WORD entries = 1024;
    BYTE mainSubBitCtrl = 0;

    switch (u8Color){
        case 0: {u8TriggerBits = 0;}break;
        case 1: {u8TriggerBits = BIT5;}break;
        case 2: {u8TriggerBits = BIT6;}break;
        default: {u8TriggerBits = 0;}break;
    }

    //msDrvSCCommonWrite(u16ScalerMask, TRUE);
    // address port initial as 0
    msWrite2Byte(SC2D_C2, 0x00);
    // pre set value
    msWrite2Byte(SC2D_C4, 0x00);

    msWrite2Byte(SC2D_C6, 0x00);
    entries = 600;
    mainSubBitCtrl = BIT0|BIT1;

    // [3] = write, [7] = burst write
    msWrite2ByteMask(SC2D_C0, mainSubBitCtrl|BIT3 |BIT7 | u8TriggerBits, mainSubBitCtrl|BIT3 | BIT7 | u8TriggerBits);
    j = 0;
    for(i = 0; i < 256; i++, j++)
    {
        u16Data = pu8DeGammaTbl[u8Color][i*2]+(pu8DeGammaTbl[u8Color][i*2+1]<<8);
        msWrite2Byte(SC2D_C4, u16Data);
    }
    for(i = 256; i < entries; i++)
    {
        msWrite2Byte(SC2D_C4, u16Data);
        msWrite2Byte(SC2D_C6, 0x00);
    }
    //close write & burst write cmd
    msWrite2Byte(SC2D_C0, 0);

    //reset addr & data  port
    msWrite2Byte(SC2D_C2, 0);
    msWrite2Byte(SC2D_C4, 0);
    //msDrvSCCommonWrite(u16ScalerMask, FALSE);

}

void msDrvFixGammaLoadTbl_256E_10B(BYTE u8WinIdx, BYTE u8Color,  BYTE **pu8FixGammaTblIdx)
{
	UNUSED(u8WinIdx);
    //DWORD u32BaseAddr;
    WORD u16Count,i, u16Data, j;

    //WORD u16ScalerMask = g_DisplayWindow[u8WinIdx].u16DwScMaskOut;
    BYTE u8TriggerBits;

    switch (u8Color){
        case 0: {u8TriggerBits = 0;}break;
        case 1: {u8TriggerBits = BIT5;}break;
        case 2: {u8TriggerBits = BIT6;}break;
        default: {u8TriggerBits = 0;}break;
    }

    //u32BaseAddr = SCRegBase(MapWin2ScIndex(u8WinIdx));

    u16Count = 256;

    //msDrvSCCommonWrite(u16ScalerMask, TRUE);
    // address port initial as 0
    msWrite2Byte(SC25_F2, 0x0);
    // pre set value
    msWrite2Byte(SC25_F4, 0x0);

    // RGB write together and write one time for clear write bit.
    msWrite2ByteMask(SC25_F0, BIT(3) | u8TriggerBits, BIT(3) | u8TriggerBits);
    j = 0;
    for(i=0; i<u16Count; i++, j++)
    {

        if ((j % 3) == 0)   //0, 2, 4
            u16Data = pu8FixGammaTblIdx[u8Color][j] + (LOWNIBBLE(pu8FixGammaTblIdx[u8Color][j+2]) << 8);
        else
        {
            u16Data = pu8FixGammaTblIdx[u8Color][j] + (HIGHNIBBLE(pu8FixGammaTblIdx[u8Color][j+1]) << 8);
            j++;
        }

        // waiting ready.
        while (msRead2Byte(SC25_F0) & BIT3);

        msWrite2Byte(SC25_F2, i); //address
        msWrite2Byte(SC25_F4, u16Data); //data
        msWrite2ByteMask(SC25_F0, BIT3, BIT3); //io_w enable

    }

    msWrite2Byte(SC25_F0, 0x0);

    //reset addr & data  port
    msWrite2Byte(SC25_F2, 0);
    msWrite2Byte(SC25_F4, 0);
    //msDrvSCCommonWrite(u16ScalerMask, FALSE);

}

void msDrvDeCompressFMT46Gamma(BYTE *pu8GammaTblIdx, DWORD* dwDecompressGamma)
{
    BYTE u8LowValue=0, u8Temp=0;
    WORD u16Data=0, u16Level=0, idx=0;

    for( u16Level=0; u16Level<310; u16Level+=5)
    {
        // Write gamma data
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
    // Write gamma data
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

void msDrvGammaLoadTbl_256E_14B(BYTE u8WinIdx, BYTE **GammaTblIdx)
{
	UNUSED(u8WinIdx);
    BYTE u8TgtChannel;

    for(u8TgtChannel=0; u8TgtChannel<3; u8TgtChannel++ )
    {
        // Write data to gamma channel
#if ENABLE_LUT_AUTODOWNLOAD
		#if(GammaTableSize == 320)
			msDrvGammaWriteTbl_256E_14B_ByDMA(u8WinIdx, u8TgtChannel, GammaTblIdx);
		#elif(GammaTableSize == 64)
			BYTE* ptr = (BYTE*)((void*)GammaTblIdx[u8TgtChannel]);
	        WritePostComprssGamma2Dram(u8TgtChannel, ptr);
		#endif
#else
		WORD* ptr = NULL;
		extern void ScalerAPI_GammaLoadTable(BYTE u8Color, WORD *p_table);
        ptr = (WORD*)((void*)GammaTblIdx[u8TgtChannel]);
		ScalerAPI_GammaLoadTable(u8TgtChannel, ptr);
#endif
    }
    #if ENABLE_LUT_AUTODOWNLOAD
        extern void WriteTriggerADLPostGamma(DWORD u16StartAddr, BOOL Enable);
		WriteTriggerADLPostGamma(POSTGAMMA_ADL_ADDR, TRUE);
    #endif
}

void msDrvGammaLoadTbl_256E_14B_76Bytes(BYTE u8WinIdx, BYTE **GammaTblIdx)
{
	UNUSED(u8WinIdx);
    BYTE u8TgtChannel;

    for(u8TgtChannel=0; u8TgtChannel<3; u8TgtChannel++ )
    {
        // Write data to gamma channel
#if ENABLE_LUT_AUTODOWNLOAD

		 BYTE* ptr = (BYTE*)((void*)GammaTblIdx[u8TgtChannel]);
	        WritePostComprssGamma2Dramby76Bytes(u8TgtChannel, ptr);
#else
		WORD* ptr = NULL;
		extern void ScalerAPI_GammaLoadTable(BYTE u8Color, WORD *p_table);
        ptr = (WORD*)((void*)GammaTblIdx[u8TgtChannel]);
		ScalerAPI_GammaLoadTable(u8TgtChannel, ptr);
#endif
    }
    #if ENABLE_LUT_AUTODOWNLOAD
        extern void WriteTriggerADLPostGamma(DWORD u16StartAddr, BOOL Enable);
		WriteTriggerADLPostGamma(POSTGAMMA_ADL_ADDR, TRUE);
    #endif
}


#if ENABLE_LUT_AUTODOWNLOAD
void WritePostComprssGamma2Dramby76Bytes( BYTE u8Color, BYTE *pu8GammaTblIdx)
{
	WORD i = 0;
	WORD GammaTbl[4], DiffValue0, DiffValue1;
	DWORD dwDecompressGamma[256];
	memset(GammaTbl, 0, 4*sizeof(WORD));
	memset(dwDecompressGamma, 0, 256*sizeof(DWORD));
	WORD SumValue = 0;
	for (i = 0; i < 76; i++)
	{
		DiffValue1 = pu8GammaTblIdx[i] & 0x0F;
		DiffValue0 = (pu8GammaTblIdx[i] & 0xF0) >> 4;

		if (i < 16)
		{
			if (i == 0)
			{
				dwDecompressGamma[i] = 0;
				SumValue = SumValue + pu8GammaTblIdx[i] ;
			}	
			else
			{
				dwDecompressGamma[i] = SumValue<<4;
				SumValue = SumValue + pu8GammaTblIdx[i];
				GammaTbl[3] = SumValue;
			}

		}
		else
		{			
			GammaTbl[0] = GammaTbl[3] + DiffValue0;
			GammaTbl[1] = GammaTbl[0] + DiffValue0;
			GammaTbl[2] = GammaTbl[1] + DiffValue1;
			GammaTbl[3] = GammaTbl[2] + DiffValue1;
			
			dwDecompressGamma[(i - 16) * 4 + 16] = (GammaTbl[0])<<4;
			dwDecompressGamma[(i - 16) * 4 + 17] = (GammaTbl[1])<<4;
			dwDecompressGamma[(i - 16) * 4 + 18] = (GammaTbl[2])<<4;
			dwDecompressGamma[(i - 16) * 4 + 19] = (GammaTbl[3])<<4;
		}
	
	}


	DWORD u32Address=0;
    u32Address = POSTGAMMA_ADL_ADDR+(u8Color*POSTGAMMA_TABLE_ENTRIES*sizeof(WORD));

	WORD u16Idx = 0;

	for(u16Idx = 0; u16Idx < 256; u16Idx++)
	{
		IMI_Write2Bytes(u32Address, (WORD)(dwDecompressGamma[u16Idx]));
		u32Address+=sizeof(WORD);
	}
}

void WritePostComprssGamma2Dram ( BYTE u8Color, BYTE *pu8GammaTblIdx)
{
    WORD i = 0;
    WORD GammaTbl[4], DiffValue0, DiffValue1;
	DWORD dwDecompressGamma[256];
	memset(GammaTbl, 0, 4*sizeof(WORD));
	memset(dwDecompressGamma, 0, 256*sizeof(DWORD));

    for ( i = 0; i < GammaTableSize; i++ )
    {
        DiffValue1 = pu8GammaTblIdx[i] & 0x0F;
        DiffValue0 = ( pu8GammaTblIdx[i] & 0xF0 ) >> 4;

        if ( i == 0 )
        {
            GammaTbl[0] = 0;
            GammaTbl[1] =   DiffValue0 * 2;
        }
        else
        {
            GammaTbl[0] = GammaTbl[3] ;
            GammaTbl[0] = GammaTbl[0] + DiffValue0 ;
            GammaTbl[1] = GammaTbl[0] + DiffValue0 ;
        }

        //GammaTbl[1] = GammaTbl[0] + DiffValue0 ;
        GammaTbl[2] = GammaTbl[1] + DiffValue1 ;
        GammaTbl[3] = GammaTbl[2] + DiffValue1 ;

		dwDecompressGamma[i*4]   = (GammaTbl[0] << 4);
		dwDecompressGamma[i*4+1] = (GammaTbl[1] << 4);
		dwDecompressGamma[i*4+2] = (GammaTbl[2] << 4);
		dwDecompressGamma[i*4+3] = (GammaTbl[3] << 4);

		//printf(" %x", dwDecompressGamma[i*4]);
		//printf(" %x", dwDecompressGamma[i*4+1]);
		//printf(" %x", dwDecompressGamma[i*4+2]);
		//printf(" %x", dwDecompressGamma[i*4+3]);
    }

	DWORD u32Address=0;
    u32Address = POSTGAMMA_ADL_ADDR+(u8Color*POSTGAMMA_TABLE_ENTRIES*sizeof(WORD));

	WORD u16Idx = 0;

	for(u16Idx = 0; u16Idx < 256; u16Idx++)
	{
		IMI_Write2Bytes(u32Address, (WORD)(dwDecompressGamma[u16Idx]));
		u32Address+=sizeof(WORD);
	}
}
void msDrvGammaWriteTbl_256E_14B_ByDMA(BYTE u8WinIdx, BYTE u8Color, BYTE **pu8GammaTblIdx )
{
	UNUSED(u8WinIdx);

    const int nSizeofGammaTable = 257;
    DWORD dwDecompressGamma[nSizeofGammaTable];
    memset(dwDecompressGamma, 0, nSizeofGammaTable*sizeof(DWORD));

    msDrvDeCompressFMT46Gamma(*(pu8GammaTblIdx + u8Color), dwDecompressGamma);

	DWORD u32Address=0;
    u32Address = POSTGAMMA_ADL_ADDR+(u8Color*POSTGAMMA_TABLE_ENTRIES*sizeof(WORD));

	WORD u16Idx = 0;

	for(u16Idx = 0; u16Idx < nSizeofGammaTable-1; u16Idx++)
	{
		if(u16Idx == 255)
			IMI_Write2Bytes(u32Address, (WORD)(dwDecompressGamma[256]));
		else
			IMI_Write2Bytes(u32Address, (WORD)(dwDecompressGamma[u16Idx]));
		u32Address+=sizeof(WORD);
	}
}
#endif
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
        }
        else                //Sub
        {
            msWriteBit(SC25_22, TRUE, _BIT2);
        }
    }
    else
    {
        if(u8WinIdx == MAIN_WINDOW)  //Main
        {
            msWriteBit(SC25_02, FALSE, _BIT2);
        }
        else                //Sub
        {
            msWriteBit(SC25_22, FALSE, _BIT2);
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
        }
        else                //Sub
        {
            msWriteBit(SC0F_30, TRUE, _BIT0);
        }
		//msWriteBit(SC0F_AF, TRUE, _BIT4); // repeat gamma table max value for interpolation
	msWriteBit(SC0F_AF, TRUE, _BIT5); // noise dither enable
    }
    else
    {
        if(u8WinIdx == MAIN_WINDOW)  //Main
        {
            msWriteBit(SC10_A0, FALSE, _BIT0);
        }
        else                //Sub
        {
            msWriteBit(SC0F_30, FALSE, _BIT0);
        }
		//msWriteBit(SC0F_AF, FALSE, _BIT4); // repeat gamma table max value for interpolation
	msWriteBit(SC0F_AF, FALSE, _BIT5); // noise dither enable
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
	msWriteByteMask(SC10_34, (bEnable)?_BIT0:0, _BIT0);
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

void mdrv_IP2Pattern_Enable(bool bEnable)
{
    WORD u16Height = SC0_READ_IMAGE_HEIGHT();
    WORD u16Width = SC0_READ_IMAGE_WIDTH();

    msWriteBit(SC53_15, (!bEnable), _BIT7);

    msWrite2ByteMask(SC53_18, u16Width, 0x1FFF);
    msWrite2ByteMask(SC53_1C, u16Height, 0x1FFF);
}

void mdrv_IP2Pattern_Reset(void)
{
    msWrite2Byte(SC53_02, 0x0000);
    msWrite2Byte(SC53_04, 0x0000);
    msWrite2Byte(SC53_06, 0x0000);

    msWrite2Byte(SC53_08, 0x0000);
    msWrite2Byte(SC53_0A, 0x0000);
    msWrite2Byte(SC53_0C, 0x0000);

    msWrite2Byte(SC53_0E, 0x0000);
    msWrite2Byte(SC53_10, 0x0000);
    msWrite2Byte(SC53_12, 0x0000);

    msWriteByte(SC53_14, 0x00);
    msWriteByteMask(SC53_15, 0x00, 0x7F);
    msWrite2Byte(SC53_16, 0x0000);

    msWrite2Byte(SC53_22, 0x0000);

    msWrite2Byte(SC53_24, 0x0000);
    msWrite2Byte(SC53_26, 0x0000);
    msWrite2Byte(SC53_28, 0x0000);

    msWrite2Byte(SC53_34, 0x0000);
    msWrite2Byte(SC53_36, 0x0000);
    msWrite2Byte(SC53_38, 0x0000);
    msWrite2Byte(SC53_3A, 0x0000);
}

void mdrv_IP2Pattern_SetPureColorPattern(IP2PATTERNColor *stColor)
{
    mdrv_IP2Pattern_Reset();
    /*msWriteByteMask(SC53_03, (stColor->R)>>2, 0x3F);
    msWriteByteMask(SC53_02, (stColor->R)<<6, 0xC0);

    msWriteByteMask(SC53_09, (stColor->G)>>2, 0x3F);
    msWriteByteMask(SC53_08, (stColor->G)<<6, 0xC0);

    msWriteByteMask(SC53_0F, (stColor->B)>>2, 0x3F);
    msWriteByteMask(SC53_0E, (stColor->B)<<6, 0xC0);*/

    msWriteBit(SC53_15, true, _BIT1);
    msWrite2ByteMask(SC53_34, 0x00, 0xFFF);
    msWrite2ByteMask(SC53_36, SC0_READ_IMAGE_WIDTH(), 0xFFF);

    msWrite2ByteMask(SC53_38, 0x00, 0xFFF);
    msWrite2ByteMask(SC53_3A, SC0_READ_IMAGE_HEIGHT(), 0xFFF);

    msWriteByteMask(SC53_24, (stColor->R)<<2, 0xFC); //8bits to 10bits
    msWriteByteMask(SC53_25, (stColor->R)>>6, 0x03);

    msWriteByteMask(SC53_26, (stColor->G)<<2, 0xFC);
    msWriteByteMask(SC53_27, (stColor->G)>>6, 0x03);

    msWriteByteMask(SC53_28, (stColor->B)<<2, 0xFC);
    msWriteByteMask(SC53_29, (stColor->B)>>6, 0x03);

    msWriteByteMask(SC53_04, 0x00, 0x3F);
    msWriteByteMask(SC53_07, 0x00, 0xC0);

    msWriteByteMask(SC53_0A, 0x00, 0x3F);
    msWriteByteMask(SC53_0D, 0x00, 0xC0);

    msWriteByteMask(SC53_10, 0x00, 0x3F);
    msWriteByteMask(SC53_13, 0x00<<6, 0xC0);
}

void mdrv_IP2Pattern_SetGradientPattern(PATTERN_DIRECTION dir,IP2PATTERNColor *stColor)
{
    mdrv_IP2Pattern_Reset();
    const int level = 256;
    DWORD diff = ((1023 * 4096)/(level-1));
    DWORD Ratio = (SC0_READ_IMAGE_WIDTH()!=0)?((diff*level)/(SC0_READ_IMAGE_WIDTH())):(0);

    if (Pat_Vertical_Flipped==dir || Pat_Vertical==dir)
    {
        Ratio = diff/(SC0_READ_IMAGE_HEIGHT()/level);
        msWriteBit(SC53_15, _BIT6, _BIT6);
    }

    msWrite2Byte(SC53_16, diff&0xFFFF);
    msWriteByteMask(SC53_14, (diff>>16)&0x7, 0x07);

    msWrite2Byte(SC53_22, Ratio&0xFFFF);
    msWriteByteMask(SC53_21, (Ratio>>16)&0x7, 0x07);


    if (Pat_Horizontal==dir || Pat_Vertical==dir)
    {
    msWriteByteMask(SC53_04, (stColor->R)>>2, 0x3F);
    msWriteByteMask(SC53_07, (stColor->R)<<6, 0xC0);

    msWriteByteMask(SC53_0A, (stColor->G)>>2, 0x3F);
    msWriteByteMask(SC53_0D, (stColor->G)<<6, 0xC0);

    msWriteByteMask(SC53_10, (stColor->B)>>2, 0x3F);
    msWriteByteMask(SC53_13, (stColor->B)<<6, 0xC0);
    }
    else
    {
        msWriteByteMask(SC53_03, (stColor->R)>>2, 0x3F);
        msWriteByteMask(SC53_02, (stColor->R)<<6, 0xC0);

        msWriteByteMask(SC53_09, (stColor->G)>>2, 0x3F);
        msWriteByteMask(SC53_08, (stColor->G)<<6, 0xC0);

        msWriteByteMask(SC53_0F, (stColor->B)>>2, 0x3F);
        msWriteByteMask(SC53_0E, (stColor->B)<<6, 0xC0);

        msWriteBit(SC53_15, _BIT5, _BIT5);
    }
}

void mdrv_IP2Pattern_SetPIPPattern(IP2PATTERNPIPWIN *stWin, IP2PATTERNColor *stColor, IP2PATTERNColor *stBGColor)
{
    mdrv_IP2Pattern_Reset();
    msWriteBit(SC53_15, true, _BIT1);
    msWrite2ByteMask(SC53_34, stWin->HStart, 0xFFF);
    msWrite2ByteMask(SC53_36, stWin->HEnd, 0xFFF);

    msWrite2ByteMask(SC53_38, stWin->VStart, 0xFFF);
    msWrite2ByteMask(SC53_3A, stWin->VEnd, 0xFFF);

    msWriteByteMask(SC53_24, (stColor->R)<<2, 0xFC);
    msWriteByteMask(SC53_25, (stColor->R)>>6, 0x03);

    msWriteByteMask(SC53_26, (stColor->G)<<2, 0xFC);
    msWriteByteMask(SC53_27, (stColor->G)>>6, 0x03);

    msWriteByteMask(SC53_28, (stColor->B)<<2, 0xFC);
    msWriteByteMask(SC53_29, (stColor->B)>>6, 0x03);

    msWriteByteMask(SC53_04, (stBGColor->R)>>2, 0x3F);
    msWriteByteMask(SC53_07, (stBGColor->R)<<6, 0xC0);

    msWriteByteMask(SC53_0A, (stBGColor->G)>>2, 0x3F);
    msWriteByteMask(SC53_0D, (stBGColor->G)<<6, 0xC0);

    msWriteByteMask(SC53_10, (stBGColor->B)>>2, 0x3F);
    msWriteByteMask(SC53_13, (stBGColor->B)<<6, 0xC0);

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
	    
        printf("tSR_Mode_H_Tbl");//MenuFunc_printMsg("tSR_Mode_H_Tbl");
        for(i=0;i<sizeof(tSR_Mode_H_Tbl)/sizeof(RegUnitType);i++)
            DB_WB(tSR_Mode_H_Tbl[i].u16Reg,tSR_Mode_H_Tbl[i].u8Value);//msWriteByte(tSR_Mode_H_Tbl[i].u16Reg,tSR_Mode_H_Tbl[i].u8Value);
            break;

        case SRMODE_MIDDLE :
        printf("tSR_Mode_M_Tbl");//MenuFunc_printMsg("tSR_Mode_M_Tbl");
        for(i=0;i<sizeof(tSR_Mode_M_Tbl)/sizeof(RegUnitType);i++)
            DB_WB(tSR_Mode_M_Tbl[i].u16Reg,tSR_Mode_M_Tbl[i].u8Value);//msWriteByte(tSR_Mode_M_Tbl[i].u16Reg,tSR_Mode_M_Tbl[i].u8Value);
            break;

        case SRMODE_WEAK:
        printf("tSR_Mode_L_Tbl");//MenuFunc_printMsg("tSR_Mode_L_Tbl");
        for(i=0;i<sizeof(tSR_Mode_L_Tbl)/sizeof(RegUnitType);i++)
            DB_WB(tSR_Mode_L_Tbl[i].u16Reg,tSR_Mode_L_Tbl[i].u8Value);//msWriteByte(tSR_Mode_L_Tbl[i].u16Reg,tSR_Mode_L_Tbl[i].u8Value);
            break;

        default:
        printf("tSR_Mode_OFF_Tbl");//MenuFunc_printData("tSR_Mode_OFF_Tbl",0);
        for(i=0;i<sizeof(tSR_Mode_OFF_Tbl)/sizeof(RegUnitType);i++)
            DB_WB(tSR_Mode_OFF_Tbl[i].u16Reg,tSR_Mode_OFF_Tbl[i].u8Value);//msWriteByte(tSR_Mode_OFF_Tbl[i].u16Reg,tSR_Mode_OFF_Tbl[i].u8Value);
            break;
    }
     MENU_LOAD_END(); 
    SRmodeContent = SuperResolutionMode;
}


#endif

void msDrv_VideoHueSaturation(void)
{
        bool RGBflow=WIN_COLOR_YUV; //Set RGB flow is WIN_COLOR_YUV: R2Y Y2R, WIN_COLOR_RGB: only R2R
        msACESetRGBMode(FALSE);

        if (IsColorspaceRGBInput())
        {
        	if(RGBflow ==WIN_COLOR_YUV)
        	{
        		//printf("RGB is R2Y,Y2R flow \n");
        		msACESetRGBColorRange(MAIN_WINDOW,FALSE,FALSE);
		        msACESetRGBColorRange(SUB_WINDOW,FALSE,FALSE);
		        msWriteByteMask( SC07_40, BIT4|BIT0, BIT4|BIT0);//enable R2Y
		        msWriteByteMask(SC0F_30, 0, BIT3);  //B-16, FOR RGB domain
		        msWriteByteMask(SC0F_31, 0, BIT1);  //R-16, FOR RGB domain
		        #if ENABLE_HDR
		        if ((GetInputCombColorFormat().ucColorRange == COMBO_COLOR_RANGE_LIMIT) || (msGetHDRStatus(MAIN_WINDOW) != HDR_OFF))//RGB Limit
		        #else
		        if (GetInputCombColorFormat().ucColorRange == COMBO_COLOR_RANGE_LIMIT)//RGB Limit
		        #endif
	            {
	                msWriteByteMask(SC07_41, FALSE, BIT4|BIT0);//R2Y Full to Full
	                msWriteByteMask(SC0B_A0, BIT0|BIT4, BIT0|BIT4);
	                msWriteByteMask(SC0F_AE, BIT6|BIT7, BIT6|BIT7);
	            }
	            else//RGB Full
	            {
					msWriteByteMask(SC07_41, BIT4|BIT0, BIT4|BIT0);//R2Y Full to limit
	                msWriteByteMask(SC0F_AE, BIT6, BIT6);
	                msWriteByteMask(SC0B_A0, BIT0|BIT4, BIT0|BIT4);
	            }

	        }
	        else
	        {
	            //printf("RGB is R2R flow \n");
		        #if ENABLE_HDR
		        if ((GetInputCombColorFormat().ucColorRange == COMBO_COLOR_RANGE_LIMIT) || (msGetHDRStatus(MAIN_WINDOW) != HDR_OFF))//RGB Limit
		        #else
		        if (GetInputCombColorFormat().ucColorRange == COMBO_COLOR_RANGE_LIMIT)//RGB Limit
		        #endif
	            {
		            msACESetRGBColorRange(MAIN_WINDOW,FALSE,TRUE);
		            msACESetRGBColorRange(SUB_WINDOW,FALSE,TRUE);
	            }
	            else
	            {
		            msACESetRGBColorRange(MAIN_WINDOW,FALSE,FALSE);
		            msACESetRGBColorRange(SUB_WINDOW,FALSE,FALSE);                
	            }
	            msSetPCColorMatrix(MAIN_WINDOW);
	            msSetPCColorMatrix(SUB_WINDOW); 
        	}
        }
        else    //for YUV input, disable CSC2_Y2R
        {
        	//printf("YUV flow \n");
			msWriteByteMask(SC07_40, FALSE, BIT4|BIT0); //disable CSC2_Y2R
            msWriteByteMask(SC0F_30, 0, BIT3);  //B-16, FOR RGB domain
            msWriteByteMask(SC0F_31, 0, BIT1);  //R-16, FOR RGB domain
			msWriteByteMask(SC0F_AE, BIT6|BIT7, BIT6|BIT7); //sub 16
        }

}

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
