
#define _MS_FB_C
#include "Global.h"
#if CHIP_ID==CHIP_MT9700
#define MSFB_DEBUG    1

#if ENABLE_DEBUG && MSFB_DEBUG
#define MSFB_printData(str, value)   printData(str, value)
#define MSFB_printMsg(str)           printMsg(str)
#else
#define MSFB_printData(str, value)
#define MSFB_printMsg(str)
#endif

code BYTE tMemTestMode[] = { 0x07, 0x09, 0x05, 0x03, 0x0B };
#ifdef TSUMR2_FPGA
code RegUnitType tInitMemory[] =
{
    {REG_101202, 0x58},
    {REG_101203, 0x08},
    {REG_101204, 0xC5},
    {REG_101205, 0x01},
    {REG_101206, 0x20},
    {REG_101207, 0x01},
    {REG_101208, 0x33},
    {REG_101209, 0x08},
    {REG_10120A, 0x33},
    {REG_10120B, 0x0C},
    {REG_10120C, 0x11},
    {REG_10120D, 0x71},
    {REG_10120E, 0x0E},
    {REG_10120F, 0x10},
    {REG_101210, 0x31},
    {REG_101211, 0x00},
    {REG_10122A, 0x0f},
    {REG_10122B, 0x40},
    {REG_101200, 0x00},
    {REG_101201, 0x00},
    {REG_101200, 0x08},
    {REG_101201, 0x00},
    {REG_101200, 0x0C},
    {REG_101201, 0x00},
    {REG_101200, 0x0e},
    {REG_101201, 0x00},
    {REG_101200, 0x1F},
    {REG_101201, 0x00},
    {REG_10121E, 0x08},
    {REG_10121F, 0x1A},
};
#endif

Bool msMemoryBist(void)
{
    BYTE xdata i, bist_try;
    WORD xdata u16TestMode, u16ExtraMode;
    BOOL xdata bReturn = 0;
    DWORD xdata u32BistSize = 0x1000;

    bist_try = 10;
    u16TestMode  = (BIT2|BIT1); //BIST test mode [2:1]
    u16ExtraMode = u16TestMode;

    MSFB_printData("(BIST_SIZE = 0x%x)", (unsigned int)u32BistSize);

    msWrite2Byte(REG_101246, 0x7FFE); //mask group0 other request
    msWrite2Byte(REG_101266, 0xFFFF); //mask group1 other request
    msWrite2Byte(REG_101286, 0xFFFF); //mask group2 other request
    msWrite2Byte(REG_1012A6, 0xFFFF); //mask group3 other request

    //DQS reset
    msWrite2Byte(REG_10110E, msRead2Byte(REG_10110E)|BIT6);
    msWrite2Byte(REG_10110E, msRead2Byte(REG_10110E)&~BIT6);
//-------------
//miu self test
//-------------
    msWrite2Byte(REG_1012E0, 0x0000);
    msWriteByteMask(REG_101203, 0x00, 0x08);

    msWrite2Byte(REG_1012E0, 0x0000);
    msWrite2Byte(REG_1012E2, 0x0000/BIST_BASE_UNIT);      //test base address
    msWrite4Byte(REG_1012E4, u32BistSize/MIU_UNIT);       //test length
    msWrite2Byte(REG_1012E8, 0xff00);                     //test pattern

    msWrite2Byte(REG_1012E0, u16ExtraMode);
    ForceDelay1ms(1);
    msWrite2Byte(REG_1012E0, u16ExtraMode|BIT0);

    for (i = 0; i < bist_try; i++)
    {
        const DWORD xdata BIST_TRY_MAX = 100;
        DWORD xdata u32BistTry = 0;

        while(!(msRead2Byte(REG_1012E0)&BIT15) && (u32BistTry++ < BIST_TRY_MAX));

        if (!(msRead2Byte(REG_1012E0)&0x6000) && (msRead2Byte(REG_1012E0)&BIT15))
        {
            MSFB_printData("try(%d) passed...", i);
            bReturn = TRUE;
            break;
        }
        MSFB_printData("bist = 0x%x", msRead2Byte(REG_1012E0));
        msWrite2Byte(REG_1012E0, u16ExtraMode);
        ForceDelay1ms(1);
        msWrite2Byte(REG_1012E0, u16ExtraMode|BIT0);
    }

    if (i >= bist_try)
    {
        MSFB_printMsg("failed...\r\n\r\n");
        bReturn = FALSE;
    }

    msWrite2Byte(REG_101246, 0x0000); //mask group0 other request
    msWrite2Byte(REG_101266, 0x0000); //mask group1 other request
    msWrite2Byte(REG_101286, 0x0000); //mask group2 other request
    msWrite2Byte(REG_1012A6, 0x0000); //mask group3 other request

    return bReturn;
}

#define DQSN_4(value)   (value>>4)
void msAutoDQSPhase(void)
{
#if 0
    BYTE xdata u8Step;
    BYTE xdata u8Mode;
    BYTE xdata u8DqsSum;
    BYTE xdata u8DqsCnt;
    WORD xdata u16PhaseResult = 0;
    BYTE xdata u8Write;
    BYTE xdata u8DqsPhase;
#if ENABLE_DEBUG
    BYTE xdata strDqsPhase[17];
#endif

    old_msWrite2Byte(REG_12E2, 0x0000);     //SET_AUTO_DQS_START_ADDR );     //(4kbytes)/unit
    old_msWrite2Byte(REG_12E4, 0x1000);     //SET_AUTO_DQS_VERIFY_SIZE );     //(16 byte)/unit
    old_msWrite2Byte(REG_12E6, 0x0000);
    old_msWrite2Byte(REG_12E8, 0x5AA5);     //test data

    u8DqsSum = 0;
    u8DqsCnt = 0;

    u8DqsPhase = old_msReadByte(REG_113A);
#if ENABLE_DEBUG
    printMsg("[ AUTO DQS READ PHASE ]");
    printData("Def. Read Phase: %x", u8DqsPhase);
#endif

    // for ( u8Step = 0; u8Step < 64; u8Step++ )
    // DDR-16 DQS 0 only
    for ( u8Step = 16; u8Step < 32; u8Step++ )
    {
        if( DQSN_4(u8Step)==1 )
            u8Write = (((u8Step & 0x0F) << 4) | (u8DqsPhase&0x0F));
        else if( DQSN_4(u8Step)==0 )
            u8Write = ((u8Step & 0x0F) | (u8DqsPhase&0xF0));

        old_msWriteByte(REG_113A, u8Write);

        for ( u8Mode = 0 ; u8Mode < sizeof(tMemTestMode) ; ++u8Mode )
        {
            old_msWriteByte(REG_12E0, 0x00);
            old_msWriteByte(REG_12E0, tMemTestMode[u8Mode]);  // pattern from data

            while ( !(old_msReadByte(REG_12E1)&0x80) );

            if ( old_msReadByte(REG_12E1)&0x60 )
                break;
        }
        old_msWrite2Byte(REG_12E0, 0x0000 );

        if ( u8Mode ==  sizeof( tMemTestMode ) )
        {
            u8DqsSum += (u8Step & 0x0F);
            ++u8DqsCnt;
            u16PhaseResult |= (1 << (u8Step & 0x0F));
        }

        if ( (u8Step % 16 == 15) )
        {
            if (u8DqsCnt)
            {
                if(DQSN_4(u8Step)==1) u8DqsPhase=(u8DqsPhase & 0x0F);
                else if(DQSN_4(u8Step)==0) u8DqsPhase=(u8DqsPhase & 0xF0);
                u8DqsPhase |= ((u8DqsSum / u8DqsCnt) << (DQSN_4( u8Step ) * 4));
                u8DqsSum = 0;
                u8DqsCnt = 0;
            }

#if ENABLE_DEBUG
            for( u8Mode = 0 ; u8Mode < 16 ; u8Mode++ )
            {
                if( u16PhaseResult & (1 << u8Mode) )
                    strDqsPhase[u8Mode] = 'O';
                else
                    strDqsPhase[u8Mode] = '-';
            }
            strDqsPhase[u8Mode] = 0;
            printData("%s", (WORD)strDqsPhase);
#endif

            u16PhaseResult = 0;
        }
    }

    //refine the MIU DQS phase code flow for HW issue
    old_msWriteByte(REG_113A, u8DqsPhase );

#if ENABLE_DEBUG
    printData( "Auto Read Phase: %x", old_msRead2Byte(REG_113A) );
#endif

#else

    MSFB_printMsg("msAutoDQSPhase() TBD!");

#endif
}

#ifdef TSUMR2_FPGA
void msInitMemory( void )
{
    XDATA BYTE i, ucDDRInitCount=0;
    BOOL bDDR3InitState;

    for( i = 0; i < sizeof( tInitMemory ) / sizeof( RegUnitType ); i++ )
        msWriteByte( tInitMemory[i].u16Reg, tInitMemory[i].u8Value );


    bDDR3InitState = FALSE;
    while(ucDDRInitCount++<10)
    {
        if(msMemoryBist())
        {
            bDDR3InitState = TRUE;
            MSFB_printMsg("=====>   Memory Init PASS!!  <======\r\n");
            break;
        }
    }

    if(bDDR3InitState == FALSE)
    {
    #if ENABLE_DEBUG
        MSFB_printMsg("=====>   Memory Init Fail!!  <======\r\n");
        ForceDelay1ms(3000);
        ForceDelay1ms(3000);
        ForceDelay1ms(3000);
    #endif
    }

    msWriteByte( REG_1012E0, 0x00 );
    msWriteByte( REG_101246, 0x00 );  // MIU UnMask
    msWriteByte( REG_101247, 0x00 );

    //===================================
    old_msWriteByte( 0x00,tMemTestMode[0]);
    msAutoDQSPhase();
    msMiuProtectCtrl(0,0,0,0,0);
}
#else
#if 0
//////////////////////////////////////////////////////////////////
// Modulation Freq: 0~30KHz (freqMod -0~30), Range: 0~1% (range -0~10) //
//////////////////////////////////////////////////////////////////
void msSetDDRSSCEnable( Bool bEnable )
{
    MSFB_printData("DDRSSC Enable[%x]",bEnable);

    msWriteByteMask(REG_101129, bEnable?(BIT7|BIT6):BIT7, BIT7|BIT6);
}

void msSetDDRSSCFactor( BYTE freqMod, BYTE range )
{
    static DWORD xdata dwFactorBK = 0;
    DWORD xdata dwFactor;
    WORD xdata wStep;
    WORD xdata wSpan;
    DWORD xdata dwMPLL_MHZ = 432ul;

    if( (freqMod == 0) || (range == 0) )
    {
        dwFactorBK = 0;

        wStep = 0;
        wSpan = 0;
    }
    else
    {
        // freqMod: 0~30 means 0~30K
        if(freqMod > DDR_SSC_MODULATION_MAX)
            freqMod = DDR_SSC_MODULATION_MAX;

        // range: 0~10 means 0~1%
        if(range > DDR_SSC_PERCENTAGE_MAX)
            range = DDR_SSC_PERCENTAGE_MAX;

        dwFactor = msReadByte(REG_101132);
        dwFactor <<= 16;
        dwFactor |= msRead2Byte(REG_101130);
        if(dwFactorBK == dwFactor)
        {
            return;
        }

        dwFactorBK = dwFactor;

        wSpan = (((float)dwMPLL_MHZ*1000ul/freqMod) * 131072ul) / dwFactor;
        wStep = (((float)range*dwFactor+(DWORD)500*wSpan)/((DWORD)1000*wSpan));

        //drvmStar_printData("DDRSSC SPAN[%x]",wSpan);
        //drvmStar_printData("DDRSSC STEP[%x]",wStep);

        if (wStep > 0x3FF)
            wStep = 0x3FF;

        if(wSpan > 0x3FFF)
            wSpan = 0x3FFF;
    }

    msWrite2ByteMask(REG_101128, wStep, 0x3FF);
    msWrite2ByteMask(REG_10112A, wSpan, 0x3FFF);
}

void msInitMemoryWinbond( void )
{
    XDATA DWORD dwDdrSet;

#if ((DRAM_TYPE == SDR_1Mx16_QFP256) || (DRAM_TYPE == SDR_1Mx16_QFP216) || (DRAM_TYPE == SDR_1Mx16_QFP156))
    dwDdrSet = 0x00228F5C; // 200MHz
#elif((DRAM_TYPE == DDR_8Mx16_QFP256) || (DRAM_TYPE == DDR_8Mx16_QFP216) || (DRAM_TYPE == DDR_8Mx16_QFP156))
    dwDdrSet = 0x001BA5E3; // 500MHz
#endif

    dwDdrSet += ((dwDdrSet*DDR_SSC_DEVIATION)/(1000-DDR_SSC_DEVIATION));
#if ((DRAM_TYPE == SDR_1Mx16_QFP256) || (DRAM_TYPE == SDR_1Mx16_QFP216) || (DRAM_TYPE == SDR_1Mx16_QFP156))
    msWrite2Byte(REG_101220, 0x0000);
    msWrite2Byte(REG_10121E, 0x0C00);
    msWrite2Byte(REG_10121E, 0x0C00);
    msWrite2Byte(REG_10121E, 0x0C00);
    msWrite2Byte(REG_10121E, 0x0C01);
    msWrite2Byte(REG_101202, 0x0000);
    msWrite2Byte(REG_101246, 0xFFFE);
    msWrite2Byte(REG_101266, 0xFFFF);
    msWrite2Byte(REG_101286, 0xFFFF);
    msWrite2Byte(REG_1012A6, 0xFFFF);
    // set DDRPLL0 2000MHz
    msWrite2Byte(REG_101136, 0x4004);
    msWrite2Byte(REG_101134, 0x0000);
    msWrite2Byte(REG_101130, dwDdrSet);
    msWrite2Byte(REG_101132, dwDdrSet>>16);
    msWrite2Byte(REG_101178, 0x0001);
    msWrite2Byte(REG_101122, 0x2000);
    msWrite2Byte(REG_101124, 0x2000);
    msWrite2Byte(REG_101124, 0x3000);
    msWrite2Byte(REG_101124, 0x2000);
    ForceDelay1ms(10); // wait PLL stable
    msWrite2Byte(REG_101202, 0x0000);
    msWrite2Byte(REG_101204, 0x01CB);
    msWrite2Byte(REG_101206, 0x1120);
    msWrite2Byte(REG_101208, 0x0844);
    msWrite2Byte(REG_10120A, 0x1233);
    msWrite2Byte(REG_10120C, 0x7FF1);
    msWrite2Byte(REG_10120E, 0x100E);
    msWrite2Byte(REG_101210, 0x0031);
    msWrite2Byte(REG_10122A, 0x400F);
    msWrite2Byte(REG_101102, 0x0101);
    msWrite2Byte(REG_101138, 0x0004);
    msWrite2Byte(REG_10113A, 0x0040);
    msWrite2Byte(REG_10113C, 0x8000);
    msWrite2Byte(REG_10115C, 0x00FF);
    msWrite2Byte(REG_101174, 0x0000);
    msWrite2Byte(REG_101180, 0x0010);
    msWrite2Byte(REG_10110E, 0x0083);
    msWrite2Byte(REG_101114, 0x00FF);
    msWrite2Byte(REG_101180, 0x0010);
    msWrite2Byte(REG_101108, 0x003F);
    msWrite2Byte(REG_10110E, 0x0083);
    msWrite2Byte(REG_10110E, 0x00C3);
    msWrite2Byte(REG_10110E, 0x0083);
    msWrite2Byte(REG_10121E, 0x0C01);
    msWrite2Byte(REG_10121E, 0x0C00);
    // disable GPIO
    msWrite2Byte(REG_101100, 0x2010);
    msWrite2Byte(REG_101100, 0x0000);
    msWrite2Byte(REG_10117C, 0x0000);
    msWrite2Byte(REG_101100, 0x0001);
    msWrite2Byte(REG_10110E, 0x0083);
    msWrite2Byte(REG_10110E, 0x00C3);
    msWrite2Byte(REG_10110E, 0x0083);
    msWrite2Byte(REG_101200, 0x0000);
    msWrite2Byte(REG_101200, 0x0008);
    msWrite2Byte(REG_101200, 0x000C);
    ForceDelay1ms(1);
    msWrite2Byte(REG_101200, 0x000E);
    ForceDelay1ms(1);
    msWrite2Byte(REG_101200, 0x001F);
    ForceDelay1ms(10); // check REG_101200 #15 for wait initial done
    msWrite2Byte(REG_10121E, 0x1A08);
    msWrite2Byte(REG_101240, 0x8015);
    msWrite2Byte(REG_101260, 0x8015);
    msWrite2Byte(REG_101280, 0x8015);
    msWrite2Byte(REG_1012A0, 0x8015);
    msWrite2Byte(REG_101246, 0x0000);
    msWrite2Byte(REG_101266, 0x0000);
    msWrite2Byte(REG_101286, 0x0000);
    msWrite2Byte(REG_1012A6, 0x0000);
    msWrite2Byte(REG_1012FE, 0x80E1);
    msWrite2Byte(REG_1012E0, 0x0000);
    msWrite2Byte(REG_1012F0, 0x0000);
    msWrite2Byte(REG_10122A, 0x4000);
    msWrite2Byte(REG_101252, 0xFFFF);
    msWrite2Byte(REG_101272, 0xFFFF);
    msWrite2Byte(REG_101292, 0xFFFF);
    msWrite2Byte(REG_1012B2, 0xFFFF);
#elif ((DRAM_TYPE == DDR_8Mx16_QFP256) || (DRAM_TYPE == DDR_8Mx16_QFP216) || (DRAM_TYPE == DDR_8Mx16_QFP156))
    msWrite2Byte(REG_101220, 0x0000);
    msWrite2Byte(REG_10121E, 0x0C00);
    msWrite2Byte(REG_10121E, 0x0C00);
    msWrite2Byte(REG_10121E, 0x0C00);
    msWrite2Byte(REG_10121E, 0x0C01);
    //msWrite2Byte(REG_10121E, 0x0C00);
    msWrite2Byte(REG_101202, 0x0000);
    msWrite2Byte(REG_101246, 0xFFFE);
    msWrite2Byte(REG_101266, 0xFFFF);
    msWrite2Byte(REG_101286, 0xFFFF);
    msWrite2Byte(REG_1012A6, 0xFFFF);
    // set DDRPLL0 2000MHz
    msWrite2Byte(REG_101136, 0x4000);
    msWrite2Byte(REG_101134, 0x0000);
    msWrite2Byte(REG_101130, dwDdrSet);
    msWrite2Byte(REG_101132, dwDdrSet>>16);
    msWrite2Byte(REG_101178, 0x0001);
    msWrite2Byte(REG_101122, 0x2000);
    msWrite2Byte(REG_101124, 0x2000);
    msWrite2Byte(REG_101124, 0x3000);
    msWrite2Byte(REG_101124, 0x2000);
    ForceDelay1ms(10); // wait PLL stable
    // set DDR_16_2X_CL2_500
    msWrite2Byte(REG_101202, 0x0151);
    msWrite2Byte(REG_101204, 0x004B);
    msWrite2Byte(REG_101206, 0x1420);
    msWrite2Byte(REG_101208, 0x2266);
    msWrite2Byte(REG_10120A, 0x0C53);
    msWrite2Byte(REG_10120C, 0x5231);
    msWrite2Byte(REG_10120E, 0x1020);
    msWrite2Byte(REG_101210, 0x0031);
    msWrite2Byte(REG_101212, 0x4000);
    msWrite2Byte(REG_101214, 0x8000);
    msWrite2Byte(REG_101216, 0xC000);
    msWrite2Byte(REG_101102, 0xAAAA);
    msWrite2Byte(REG_101104, 0x0000);
    msWrite2Byte(REG_10113C, 0x0000);
    msWrite2Byte(REG_10113E, 0x0000);
#if (DRAM_TYPE == DDR_8Mx16_QFP156)
    msWrite2Byte(REG_10113A, 0x0053);
#else // (DRAM_TYPE == DDR_8Mx16_QFP256) || (DRAM_TYPE == DDR_8Mx16_QFP216)
    msWrite2Byte(REG_10113A, 0x0043);
#endif
    msWrite2Byte(REG_101148, 0x0000);
    msWrite2Byte(REG_10114C, 0x0000);
    msWrite2Byte(REG_10114E, 0x0000);
    msWrite2Byte(REG_10110A, 0x0000);
    msWrite2Byte(REG_101150, 0x0000);
#if (DRAM_TYPE == DDR_8Mx16_QFP256)
    msWrite2Byte(REG_101138, 0x0005);
#else // (DRAM_TYPE == DDR_8Mx16_QFP216) || (DRAM_TYPE == DDR_8Mx16_QFP156)
    msWrite2Byte(REG_101138, 0x0004);
#endif
    msWrite2Byte(REG_10114A, 0x0002);
    msWrite2Byte(REG_101152, 0x0000);
    msWrite2Byte(REG_10116C, 0x5555);
    msWrite2Byte(REG_10116E, 0x5555);
#if (DRAM_TYPE == DDR_8Mx16_QFP256)
    msWrite2Byte(REG_101174, 0x0033);
#else // (DRAM_TYPE == DDR_8Mx16_QFP216) || (DRAM_TYPE == DDR_8Mx16_QFP156)
    msWrite2Byte(REG_101174, 0x00FF);
#endif
    msWrite2Byte(REG_101184, 0x0000);
    //msWrite2Byte(REG_10110E, 0x00E5);
    //msWrite2Byte(REG_10110E, 0x00A9);
    //msWrite2Byte(REG_10121E, 0x0C01);
    //msWrite2Byte(REG_10121E, 0x0C00);
    msWrite2Byte(REG_10117E, 0x0000);
    msWrite2Byte(REG_101154, 0xC000);
#if (DRAM_TYPE == DDR_8Mx16_QFP256)
    msWrite2Byte(REG_101114, 0x00AA);
    msWrite2Byte(REG_10115C, 0x00BB);
#else // (DRAM_TYPE == DDR_8Mx16_QFP216) || (DRAM_TYPE == DDR_8Mx16_QFP156)
    msWrite2Byte(REG_101114, 0x00FF);
    msWrite2Byte(REG_10115C, 0x00FF);
#endif
    msWrite2Byte(REG_101180, 0x0001);
    msWrite2Byte(REG_101182, 0x0000);
    msWrite2Byte(REG_101184, 0x0000);
    msWrite2Byte(REG_101108, 0x003F);
    msWrite2Byte(REG_10110E, 0x00A9);
    msWrite2Byte(REG_10110E, 0x00E9);
    msWrite2Byte(REG_10110E, 0x00A9);
    //msWrite2Byte(REG_10121E, 0x0C01);
    msWrite2Byte(REG_10121E, 0x0C00);
    // disable GPIO
    msWrite2Byte(REG_101100, 0x2010);
    msWrite2Byte(REG_101100, 0x0000);
    msWrite2Byte(REG_10117C, 0x0000);
    msWrite2Byte(REG_101100, 0x0000);
    msWrite2Byte(REG_10110E, 0x00A9);
    msWrite2Byte(REG_10110E, 0x00E9);
    msWrite2Byte(REG_10110E, 0x00A9);
    msWrite2Byte(REG_101200, 0x0000);
    msWrite2Byte(REG_101200, 0x0008);
    msWrite2Byte(REG_101200, 0x000C);
    ForceDelay1ms(1);
    msWrite2Byte(REG_101200, 0x000E);
    ForceDelay1ms(1);
    msWrite2Byte(REG_101200, 0x001F);
    ForceDelay1ms(10); // check REG_101200 #15 for wait initial done
    msWrite2Byte(REG_10121E, 0x1A08);
    msWrite2Byte(REG_101240, 0x8015);
    msWrite2Byte(REG_101260, 0x8015);
    msWrite2Byte(REG_101280, 0x8015);
    msWrite2Byte(REG_1012A0, 0x8015);
    msWrite2Byte(REG_101246, 0x0000);
    msWrite2Byte(REG_101266, 0x0000);
    msWrite2Byte(REG_101286, 0x0000);
    msWrite2Byte(REG_1012A6, 0x0000);
    msWrite2Byte(REG_1012FE, 0x80E1);
    msWrite2Byte(REG_1012E0, 0x0000);
    msWrite2Byte(REG_1012F0, 0x0000);
    msWrite2Byte(REG_10122A, 0x4000);
    msWrite2Byte(REG_101252, 0xFFFF);
    msWrite2Byte(REG_101272, 0xFFFF);
    msWrite2Byte(REG_101292, 0xFFFF);
    msWrite2Byte(REG_1012B2, 0xFFFF);
#endif
}

void msInitMemoryESMT( void )
{
    XDATA DWORD dwDdrSet;

#if ((DRAM_TYPE == SDR_1Mx16_QFP256) || (DRAM_TYPE == SDR_1Mx16_QFP216) || (DRAM_TYPE == SDR_1Mx16_QFP156))
    dwDdrSet = 0x00228F5C; // 200MHz
#else//((DRAM_TYPE == DDR_8Mx16_QFP256) || (DRAM_TYPE == DDR_8Mx16_QFP216) || (DRAM_TYPE == DDR_8Mx16_QFP156))
    dwDdrSet = 0x001BA5E3; // 500MHz
#endif

    dwDdrSet += ((dwDdrSet*DDR_SSC_DEVIATION)/(1000-DDR_SSC_DEVIATION));
#if ((DRAM_TYPE == SDR_1Mx16_QFP256) || (DRAM_TYPE == SDR_1Mx16_QFP216) || (DRAM_TYPE == SDR_1Mx16_QFP156))
    msWrite2Byte(REG_101220, 0x0000);
    msWrite2Byte(REG_10121E, 0x0C00);
    msWrite2Byte(REG_10121E, 0x0C00);
    msWrite2Byte(REG_10121E, 0x0C00);
    msWrite2Byte(REG_10121E, 0x0C01);
    msWrite2Byte(REG_101202, 0x0000);
    msWrite2Byte(REG_101246, 0xFFFE);
    msWrite2Byte(REG_101266, 0xFFFF);
    msWrite2Byte(REG_101286, 0xFFFF);
    msWrite2Byte(REG_1012A6, 0xFFFF);
    // set DDRPLL0 2000MHz
    msWrite2Byte(REG_101136, 0x4004);
    msWrite2Byte(REG_101134, 0x0000);
    msWrite2Byte(REG_101130, dwDdrSet);
    msWrite2Byte(REG_101132, dwDdrSet>>16);
    msWrite2Byte(REG_101178, 0x0001);
    msWrite2Byte(REG_101122, 0x2000);
    msWrite2Byte(REG_101124, 0x2000);
    msWrite2Byte(REG_101124, 0x3000);
    msWrite2Byte(REG_101124, 0x2000);
    ForceDelay1ms(10); // wait PLL stable
    msWrite2Byte(REG_101202, 0x0000);
    msWrite2Byte(REG_101204, 0x01CB);
    msWrite2Byte(REG_101206, 0x1120);
    msWrite2Byte(REG_101208, 0x0844);
    msWrite2Byte(REG_10120A, 0x1233);
    msWrite2Byte(REG_10120C, 0x7FF1);
    msWrite2Byte(REG_10120E, 0x100E);
    msWrite2Byte(REG_101210, 0x0031);
    msWrite2Byte(REG_10122A, 0x400F);
    msWrite2Byte(REG_101102, 0x0101);
#if (DRAM_TYPE == SDR_1Mx16_QFP256)
    msWrite2Byte(REG_101138, 0x0005);
#else // (DRAM_TYPE == SDR_1Mx16_QFP256) || (DRAM_TYPE == SDR_1Mx16_QFP156)
    msWrite2Byte(REG_101138, 0x0004);
#endif
    msWrite2Byte(REG_10113A, 0x0030);
    msWrite2Byte(REG_10113C, 0x8000);
    msWrite2Byte(REG_10115C, 0x00FF);
    msWrite2Byte(REG_101174, 0x0000);
    msWrite2Byte(REG_101180, 0x0010);
    msWrite2Byte(REG_10110E, 0x0083);
    msWrite2Byte(REG_101114, 0x00FF);
    msWrite2Byte(REG_101180, 0x0010);
    msWrite2Byte(REG_101108, 0x003F);
    msWrite2Byte(REG_10110E, 0x0083);
    msWrite2Byte(REG_10110E, 0x00C3);
    msWrite2Byte(REG_10110E, 0x0083);
    msWrite2Byte(REG_10121E, 0x0C01);
    msWrite2Byte(REG_10121E, 0x0C00);
    // disable GPIO
    msWrite2Byte(REG_101100, 0x2010);
    msWrite2Byte(REG_101100, 0x0000);
    msWrite2Byte(REG_10117C, 0x0000);
    msWrite2Byte(REG_101100, 0x0001);
    msWrite2Byte(REG_10110E, 0x0083);
    msWrite2Byte(REG_10110E, 0x00C3);
    msWrite2Byte(REG_10110E, 0x0083);
    msWrite2Byte(REG_101200, 0x0000);
    msWrite2Byte(REG_101200, 0x0008);
    msWrite2Byte(REG_101200, 0x000C);
    ForceDelay1ms(1);
    msWrite2Byte(REG_101200, 0x000E);
    ForceDelay1ms(1);
    msWrite2Byte(REG_101200, 0x001F);
    ForceDelay1ms(10); // check REG_101200 #15 for wait initial done
    msWrite2Byte(REG_10121E, 0x1A08);
    msWrite2Byte(REG_101240, 0x8015);
    msWrite2Byte(REG_101260, 0x8015);
    msWrite2Byte(REG_101280, 0x8015);
    msWrite2Byte(REG_1012A0, 0x8015);
    msWrite2Byte(REG_101246, 0x0000);
    msWrite2Byte(REG_101266, 0x0000);
    msWrite2Byte(REG_101286, 0x0000);
    msWrite2Byte(REG_1012A6, 0x0000);
    msWrite2Byte(REG_1012FE, 0x80E1);
    msWrite2Byte(REG_1012E0, 0x0000);
    msWrite2Byte(REG_1012F0, 0x0000);
    msWrite2Byte(REG_10122A, 0x4000);
    msWrite2Byte(REG_101252, 0xFFFF);
    msWrite2Byte(REG_101272, 0xFFFF);
    msWrite2Byte(REG_101292, 0xFFFF);
    msWrite2Byte(REG_1012B2, 0xFFFF);
#elif ((DRAM_TYPE == DDR_8Mx16_QFP256) || (DRAM_TYPE == DDR_8Mx16_QFP216) || (DRAM_TYPE == DDR_8Mx16_QFP156))
    msWrite2Byte(REG_101220, 0x0000);
    msWrite2Byte(REG_10121E, 0x0C00);
    msWrite2Byte(REG_10121E, 0x0C00);
    msWrite2Byte(REG_10121E, 0x0C00);
    msWrite2Byte(REG_10121E, 0x0C01);
    //msWrite2Byte(REG_10121E, 0x0C00);
    msWrite2Byte(REG_101202, 0x0000);
    msWrite2Byte(REG_101246, 0xFFFE);
    msWrite2Byte(REG_101266, 0xFFFF);
    msWrite2Byte(REG_101286, 0xFFFF);
    msWrite2Byte(REG_1012A6, 0xFFFF);
    // set DDRPLL0 2000MHz
    msWrite2Byte(REG_101136, 0x4000);
    msWrite2Byte(REG_101134, 0x0000);
    msWrite2Byte(REG_101130, dwDdrSet);
    msWrite2Byte(REG_101132, dwDdrSet>>16);
    msWrite2Byte(REG_101178, 0x0001);
    msWrite2Byte(REG_101122, 0x2000);
    msWrite2Byte(REG_101124, 0x2000);
    msWrite2Byte(REG_101124, 0x3000);
    msWrite2Byte(REG_101124, 0x2000);
    ForceDelay1ms(10); // wait PLL stable
    // set DDR_16_2X_CL2_500
    msWrite2Byte(REG_101202, 0x0151);
    msWrite2Byte(REG_101204, 0x004B);
    msWrite2Byte(REG_101206, 0x1420);
    msWrite2Byte(REG_101208, 0x2266);
    msWrite2Byte(REG_10120A, 0x0C53);
    msWrite2Byte(REG_10120C, 0x5231);
    msWrite2Byte(REG_10120E, 0x1020);
    msWrite2Byte(REG_101210, 0x0031);
    msWrite2Byte(REG_101212, 0x4000);
    msWrite2Byte(REG_101214, 0x8000);
    msWrite2Byte(REG_101216, 0xC000);
    msWrite2Byte(REG_101102, 0xAAAA);
    msWrite2Byte(REG_101104, 0x0000);
    msWrite2Byte(REG_10113C, 0x0000);
    msWrite2Byte(REG_10113E, 0x0000);
    msWrite2Byte(REG_10113A, 0x0042);
    msWrite2Byte(REG_101148, 0x0000);
    msWrite2Byte(REG_10114C, 0x0000);
    msWrite2Byte(REG_10114E, 0x0000);
    msWrite2Byte(REG_10110A, 0x0000);
    msWrite2Byte(REG_101150, 0x0000);
    msWrite2Byte(REG_101138, 0x0005);
    msWrite2Byte(REG_10114A, 0x0002);
    msWrite2Byte(REG_101152, 0x0000);
    msWrite2Byte(REG_10116C, 0x5555);
    msWrite2Byte(REG_10116E, 0x5555);
    msWrite2Byte(REG_101174, 0x0033);
    msWrite2Byte(REG_101184, 0x0000);
    //msWrite2Byte(REG_10110E, 0x00E5);
    //msWrite2Byte(REG_10110E, 0x00A9);
    //msWrite2Byte(REG_10121E, 0x0C01);
    //msWrite2Byte(REG_10121E, 0x0C00);
    msWrite2Byte(REG_10117E, 0x0000);
    msWrite2Byte(REG_101154, 0xC000);
    msWrite2Byte(REG_101114, 0x00AA);
    msWrite2Byte(REG_10115C, 0x00BB);
    msWrite2Byte(REG_101180, 0x0001);
    msWrite2Byte(REG_101182, 0x0000);
    msWrite2Byte(REG_101184, 0x0000);
    msWrite2Byte(REG_101108, 0x003F);
    msWrite2Byte(REG_10110E, 0x00A9);
    msWrite2Byte(REG_10110E, 0x00E9);
    msWrite2Byte(REG_10110E, 0x00A9);
    //msWrite2Byte(REG_10121E, 0x0C01);
    msWrite2Byte(REG_10121E, 0x0C00);
    // disable GPIO
    msWrite2Byte(REG_101100, 0x2010);
    msWrite2Byte(REG_101100, 0x0000);
    msWrite2Byte(REG_10117C, 0x0000);
    msWrite2Byte(REG_101100, 0x0000);
    msWrite2Byte(REG_10110E, 0x00A9);
    msWrite2Byte(REG_10110E, 0x00E9);
    msWrite2Byte(REG_10110E, 0x00A9);
    msWrite2Byte(REG_101200, 0x0000);
    msWrite2Byte(REG_101200, 0x0008);
    msWrite2Byte(REG_101200, 0x000C);
    ForceDelay1ms(1);
    msWrite2Byte(REG_101200, 0x000E);
    ForceDelay1ms(1);
    msWrite2Byte(REG_101200, 0x001F);
    ForceDelay1ms(10); // check REG_101200 #15 for wait initial done
    msWrite2Byte(REG_10121E, 0x1A08);
    msWrite2Byte(REG_101240, 0x8015);
    msWrite2Byte(REG_101260, 0x8015);
    msWrite2Byte(REG_101280, 0x8015);
    msWrite2Byte(REG_1012A0, 0x8015);
    msWrite2Byte(REG_101246, 0x0000);
    msWrite2Byte(REG_101266, 0x0000);
    msWrite2Byte(REG_101286, 0x0000);
    msWrite2Byte(REG_1012A6, 0x0000);
    msWrite2Byte(REG_1012FE, 0x80E1);
    msWrite2Byte(REG_1012E0, 0x0000);
    msWrite2Byte(REG_1012F0, 0x0000);
    msWrite2Byte(REG_10122A, 0x4000);
    msWrite2Byte(REG_101252, 0xFFFF);
    msWrite2Byte(REG_101272, 0xFFFF);
    msWrite2Byte(REG_101292, 0xFFFF);
    msWrite2Byte(REG_1012B2, 0xFFFF);
#endif
}
#endif // #if 0

void msInitMemory( void )
{
#if 0
    XDATA BYTE i;

    if(msReadByte(REG_101EFA)&BIT0) // 1: Wibond, 0: ESMT
        msInitMemoryWinbond();
    else
        msInitMemoryESMT();

    if(msMemoryBist())
    {
        MSFB_printMsg("=====>   Memory Init PASS!!   <=====\r\n");
    }
    else
    {
        MSFB_printMsg("\r\n\r\n\r\n=====>   Memory Init Fail!!   <=====\r\n\r\n\r\n");
    }

    msSetDDRSSCFactor(DDR_SSC_MODULATION_DEF, DDR_SSC_PERCENTAGE_DEF);
    msSetDDRSSCEnable(ENABLE_DDR_SSC);

    if(i!=i) // disable uncall warning message
    {
        msAutoDQSPhase();
        msMiuProtectCtrl(0,0,0,0,0);
    }
#endif
}
#endif

void msMiuProtectCtrl(BYTE ucGroup, BOOL bCtrl, WORD ucID, DWORD wAddrStart, DWORD wAddrEnd)
{
#if 0
    xdata BYTE ucIDNum ,i,j;
    xdata DWORD ucProtectID;

    ucIDNum =(ucGroup == MIU_PROTECT_0) ? 4:2;
    ucProtectID = 0;
    j = 0;

    for(i=0;i<16;i++)
    {
        if(ucID&(1<<i))
        {
            ucProtectID |=((DWORD)i<<(j*8));
            j++;
        }
        if(j==ucIDNum)
            break;
    }

    switch(ucGroup)
    {
        case MIU_PROTECT_0:
            old_msWrite2Byte(REG_12C2,ucProtectID);
            old_msWrite2Byte(REG_12C4,ucProtectID>>16);
            old_msWrite2Byte(REG_12C6,wAddrStart/0x800);
            old_msWrite2Byte(REG_12C8,wAddrEnd/0x800-1);
            old_msWriteBit(REG_12C0,bCtrl,MIU_PROTECT_0);
            break;
        case MIU_PROTECT_1:
            old_msWrite2Byte(REG_12CA,ucProtectID);
            old_msWrite2Byte(REG_12CC,wAddrStart/0x800);
            old_msWrite2Byte(REG_12CE,wAddrEnd/0x800-1);
            old_msWriteBit(REG_12C0,bCtrl,MIU_PROTECT_1);
            break;
        case MIU_PROTECT_2:
            old_msWrite2Byte(REG_12D0,ucProtectID);
            old_msWrite2Byte(REG_12D2,wAddrStart/0x800);
            old_msWrite2Byte(REG_12D4,wAddrEnd/0x800-1);
            old_msWriteBit(REG_12C0,bCtrl,MIU_PROTECT_2);
            break;
        case MIU_PROTECT_3:
            old_msWrite2Byte(REG_12D6,ucProtectID);
            old_msWrite2Byte(REG_12D8,wAddrStart/0x800);
            old_msWrite2Byte(REG_12DA,wAddrEnd/0x800-1);
            old_msWriteBit(REG_12C0,bCtrl,MIU_PROTECT_3);
            break;
    }

#else

	ucGroup = ucGroup;
	bCtrl = bCtrl;
	ucID = ucID;
	wAddrStart = wAddrStart;
	wAddrEnd = wAddrEnd;
    MSFB_printMsg("msMiuProtectCtrl() TBD!");

#endif
}

#endif

