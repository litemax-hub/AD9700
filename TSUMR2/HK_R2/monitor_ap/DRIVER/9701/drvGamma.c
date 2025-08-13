#include "Global.h"
#include "drvLutAutoDownload.h"
#if Enable_Gamma
#include "Panel_GAMMATBL.h"
#include "drvGamma.h"
#include "AutoGamma.h"
#include "drvAdjust.h"


#if 0//ENABLE_LUT_AUTODOWNLOAD
xdata DWORD _dramGmaAddr = MIU_AUTOLOAD_ADDR_START;
xdata DWORD _xdataGmaAddr = (WIN1_ADDR_START << 10);//AUTOLOAD_HDR_ADR  |0x80000000;
#endif

#if !ENABLE_LUT_AUTODOWNLOAD

#if USE_NORMAL_GAMMA_TBL
void drvGammaNonCompressedLoadTbl(BYTE channel, const BYTE *tblPtr)
{
    WORD i,u16Data;
    //BYTE j,u8Bit0_1;
    msWriteByteMask( SC08_22, BIT0, BIT0);
    msWriteByteMask( SC08_22, channel , BIT3 | BIT2 );
    msWrite2Byte( SC08_20, 0 );
    for (i = 0;i <128; i++)
    {
        u16Data = tblPtr[i*3] | ((tblPtr[i*3+2]&0x0F)<<8) ;
        msWrite2Byte(SC08_24, u16Data);
        u16Data = tblPtr[i*3+1] | ((tblPtr[i*3+2]&0xF0)<<4) ;
        msWrite2Byte(SC08_24, u16Data);
    }

    msWriteByteMask( SC08_22, 0, BIT1 | BIT0 );
}
#else
void drvGammaLoadCompressedTbl(BYTE channel, const BYTE* ucTab1, const BYTE* ucTab2)
{
    WORD u16CurVal = 0;
    WORD u16OutCnt = 0;
    BYTE u8Inc, u8Len;
    BYTE i ;
    BYTE u8lsb;

    msWriteByteMask( SC08_22, BIT0, BIT0);
    msWriteByteMask( SC08_22, channel , BIT3 | BIT2 );
    msWrite2Byte( SC08_20, 0 );

    for( i = 0; u16OutCnt < 256; i++ )
    {
        u8Inc = ucTab1[i] >> 5;
        u8Len = ( ucTab1[i] & 0x1f ) + 1;

        while( u8Len > 0 )
        {
            u16CurVal += u8Inc;
            if( 0 == u16OutCnt % 4 )
                u8lsb = ucTab2[u16OutCnt / 4];
            msWrite2Byte( SC08_24, (u16CurVal << 2) | (u8lsb & (BIT1 | BIT0)) );

            u8lsb >>= 2;
            u16OutCnt++;
            u8Len--;
        }
    }
    msWriteByteMask( SC08_22, 0, BIT1 | BIT0 );

}
#endif

#endif

#if 0//ENABLE_LUT_AUTODOWNLOAD
void ClearGmaDramDataADL(DWORD u16Addr)
{
    int cnt = 0;
    mcu4kXdataMapToDRAMIMI(u16Addr, 0);
    for(cnt = 0; cnt < 4096; cnt++)
    {
        _xdata2Dram[_xdataGmaAddr + cnt] = 0x00;
    }
    mcu4kXdataMapToDRAMIMI(u16Addr + 4096, 0);
    for(cnt = 0; cnt < 4096; cnt++)
    {
        _xdata2Dram[_xdataGmaAddr + cnt] = 0x00;
    }
}

void WritePostGamma2Dram(BYTE indx, BYTE *pu8GammaTblIdx)
{
	WORD u16Level;
    WORD u16DataAddr = 0;
    WORD u16LatestData;
	WORD j=0;

	for( u16Level=0; u16Level<128; u16Level++)
	{
		for( j=0; j<2; j++ )
		{
			 u16LatestData = 0;
			 if(j==0)
			 	u16LatestData = (*(pu8GammaTblIdx  + (u16Level*3))) + (((*(pu8GammaTblIdx  + (u16Level*3) + 2)) & 0x0F)<< 8) ;
			 else
			 	u16LatestData = (*(pu8GammaTblIdx  + (u16Level*3) + 1)) + (((*(pu8GammaTblIdx  + (u16Level*3) + 2)) & 0xF0)<< 4);

			u16DataAddr = u16Level *2 + j ;

			if( indx == 0 ) // Red
			{
				_xdata2Dram[_xdataGmaAddr+(u16DataAddr*16+(72)/8)] = (u16LatestData<<(72%8)) & 0xFF ;
				_xdata2Dram[_xdataGmaAddr+(u16DataAddr*16+(72)/8 + 1)] = (u16LatestData<<(72%8)) >> 8 & 0xFF;
				_xdata2Dram[_xdataGmaAddr+(u16DataAddr*16+(72)/8 + 2)] = (u16LatestData<<(72%8)) >> 16 & 0x0F;
				_xdata2Dram[_xdataGmaAddr+(u16DataAddr*16+(110/8))] = _xdata2Dram[_xdataGmaAddr+(u16DataAddr*16+(110/8))] + (1<<(110%8));

			}
			else if( indx == 1 ) // Green
			{
				_xdata2Dram[_xdataGmaAddr+ u16DataAddr*16 +(36)/8] =  (u16LatestData<<(36%8)) & 0xFF;
				_xdata2Dram[_xdataGmaAddr+(u16DataAddr*16 +(36)/8)+ 1] = (u16LatestData<<(36%8)) >> 8 & 0xFF;
				_xdata2Dram[_xdataGmaAddr+(u16DataAddr*16 +(36)/8)+ 2] =  (u16LatestData<<(36%8)) >> 16 & 0x0F;
				_xdata2Dram[_xdataGmaAddr+(u16DataAddr*16+(109/8))] =  _xdata2Dram[_xdataGmaAddr+(u16DataAddr*16+(109/8))] + (1<<(109%8)) ;
			}
			else  // Blue
			{
				_xdata2Dram[_xdataGmaAddr+(u16DataAddr*16)] = u16LatestData & 0xFF;
				_xdata2Dram[_xdataGmaAddr+(u16DataAddr*16 + 1)] = u16LatestData >> 8 & 0xFF;
				_xdata2Dram[_xdataGmaAddr+(u16DataAddr*16 + 2)] = u16LatestData >> 16 & 0x0F;
				_xdata2Dram[_xdataGmaAddr+(u16DataAddr*16+(108/8))] = _xdata2Dram[_xdataGmaAddr+(u16DataAddr*16+(108/8))] + (1<<(108%8));
			}
		}
	}
}

void WriteTriggADLGamma(DWORD u16StartAddr, BYTE Enable)
{
	BYTE u8Cnt;
	//msWriteByte(SC67_01,0x00);
	msWriteByte(SC67_04,u16StartAddr & 0xFF);
	msWriteByte(SC67_05,u16StartAddr>>8 & 0xFF);
	msWriteByte(SC67_06,u16StartAddr>>16 & 0xFF);
	msWriteByte(SC67_07,u16StartAddr>>24 & 0xFF);
	msWriteByte(SC67_10,0x00);
	msWriteByte(SC67_11,0x01);
	msWriteByte(SC67_16,0x00);
	msWriteByte(SC67_17,0x01);
	msWriteByte(SC67_1C,0x00);
	msWriteByte(SC67_1D,0x00);
	msWriteByteMask(SC67_02,Enable,BIT0);
	msWriteByte(SC67_03,0x00);

	while(msReadByte(SC67_02) & BIT0)
	{
		while((msReadByte(SC67_02) & BIT0) == 0 || u8Cnt)
			break;
		Delay4us();//Delay1ms(1);
		u8Cnt--;
	}

}

void WriteADLGamma(BYTE u8Mode)
{
	BYTE* pu8Tab1;
	xdata BYTE u8TgtChannel = 0;

	ClearGmaDramDataADL(_dramGmaAddr);
	mcu4kXdataMapToDRAMIMI(_dramGmaAddr, 0);

	while( u8TgtChannel < 3 )
	{
		pu8Tab1 = tAllGammaTab[u8Mode][u8TgtChannel];
		WritePostGamma2Dram(u8TgtChannel, pu8Tab1);
		u8TgtChannel += 1;
	}

	WriteTriggADLGamma(_dramGmaAddr, 1);
}

#endif


void drvGammaLoadTbl( BYTE GamaMode )
{
    extern code BYTE *tAllPostGammaTbl[][3];
    msAPI_GammaLoadTbl_1024E_12B_N(MAIN_WINDOW, (BYTE**)tAllPostGammaTbl[GamaMode]);
/*
#if ENABLE_LUT_AUTODOWNLOAD
    BYTE u8TgtChannel=0;
    extern void WritePostGamma2Dram(BYTE indx, BYTE *pu8GammaTblIdx);
    BYTE* ptr = NULL;
    extern code BYTE *tAllPostGammaTbl[][3];

    for (u8TgtChannel=0; u8TgtChannel<3; u8TgtChannel++)
    {
        ptr = (BYTE*)((void*)tAllPostGammaTbl[GamaMode][u8TgtChannel]);
        WritePostGamma2Dram(u8TgtChannel, ptr);
    }

    extern void WriteTriggerADLPostGamma(DWORD u16StartAddr, BOOL Enable);
    WriteTriggerADLPostGamma(POSTGAMMA_ADL_ADDR, TRUE);
#else
    extern void ScalerAPI_GammaLoadTable(BYTE u8Color, WORD *p_table);
    WORD* ptr = NULL;
    extern code BYTE *tAllPostGammaTbl[][3];

    BYTE idx =0;
    for (idx=0; idx<3; idx++)
    {
        ptr = (WORD*)((void*)tAllPostGammaTbl[GamaMode][idx]);
        ScalerAPI_GammaLoadTable(idx, ptr);
    }
#endif
*/
}

/////////////////////////////////////////////////////////////////
#ifndef GAMMA_DITHER
#define GAMMA_DITHER        0x00
#endif
void drvGammaOnOff(BYTE u8Switch, BYTE u8Window)
{
    if( MAIN_WINDOW == u8Window )
    {
        //msWriteByte(SC00_00, 0x10);
        mdrv_Adjust_DS_WBit(SC10_A0, u8Switch, _BIT0);
        mdrv_Adjust_DS_WBit(SC0F_AF, u8Switch, _BIT4);
    }
    else
    {
        //msWriteByte(SC00_00, 0x0F);
	    mdrv_Adjust_DS_WBit(SC0F_30, u8Switch, _BIT0);
        mdrv_Adjust_DS_WBit(SC0F_AF, u8Switch, _BIT4);
    }
}

#else
BYTE code msGammaNullData[] = {0};
void msGammaDummy(void)
{
    BYTE xdata i = msGammaNullData[0];
}
#endif
