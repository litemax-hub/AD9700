
#include "Global.h"
#include "drvIMI.h"
#include "MsCommon.h"

#define _MSOVD_G_

#if DEBUG_PRINT_ENABLE
    #define OD_DPUTSTR(str)    printMsg(str)
    #define OD_DPRINTF(str, x) printData(str, x)
#else
    #define OD_DPUTSTR(str)
    #define OD_DPRINTF(str, x)
#endif

BYTE code drvmsOVDNullData[] = {0};
void drvmsOVDDummy(void)
{
    BYTE xdata i = drvmsOVDNullData[0];
    i=i;
}

//#define OD_LIB_VER        0x03
//code ==================================================================

//*************************************************************************************************
//Function:  msODMemoryTest()
//Description: For Memory test
//
//           ex:    printData(" msODMemoryTest = %d"  , msODMemoryTest());
//           if the result is 0,
//           if the result is 1,
//
//*************************************************************************************************

//*************************************************************************************************
//Function:  msOD_Reset()
//Description: Reset MIU bank
//caller:        Power_TurnOnPanel of power.c
//
//           It need to reset MIU bank before turn on panel.
//*************************************************************************************************


#if ENABLE_RTE
void drvmsOverDriveAdjustWeighting(BYTE u8Weighting)
{
        msWriteByteMask(SC16_22, u8Weighting,0xFF);   //force to 0xFF for chip verify
}
//*************************************************************************************************
//Function:  msLoadODT()
//Description: OD look-up table loader.
//
//           ex: msLoadODT(tOverDrive)
//
//           It provides two kinds of format (normal and compressed) for OD look-up table.
//           The look-up table and OD lib should be located in the same bank!!
//
//*************************************************************************************************
void drvmsLoadODTable(const BYTE *pODTbl)
{
    WORD wCount/*,wi,wj*/;
    BYTE ucTARGET;

    if(0 == pODTbl) return;
    OD_DPUTSTR("Load_OD_Table");

    //msWriteBit(SC16_20,0, _BIT0); // Disable OD before load code
    drvmsOverDriveOnOff(FALSE);  // Disable OD before load code
    msWriteByte(SC16_02, 0x0E); // load code start
    msWriteByte(SC16_1C, 0x0E); //od 2nd table sram enable, r,g,b channel
    msWriteByte(SC16_1D, 0x00);


    // Uncompressed mode
    ucTARGET=*(pODTbl+9);// 10th
    for(wCount=0; wCount<289; wCount++)
    {
        if(wCount<272)
            msWriteByte(SC16_06, (wCount == 9)?ucTARGET:(ucTARGET ^ *(pODTbl+wCount)));//Od table sram1 write data
        else
            msWriteByte(SC16_06, 0x00);
        msWrite2Byte(SC16_04, wCount|0x8000);//Od table sram1 address
        while(_bit7_(msReadByte(SC16_05)));//Od table sram1 write enable (toggle type). , 0x3105[0]+ 0x3104[7:0] ==address [8:0]
    }
    ucTARGET=*(pODTbl+272+19);// 20th
    for(wCount=0; wCount<272; wCount++)
    {
        msWriteByte(SC16_0C, (wCount == 19)?ucTARGET:(ucTARGET ^ *(pODTbl+272+wCount)));
        msWrite2Byte(SC16_0A, wCount|0x8000);
        while(_bit7_(msReadByte(SC16_0B)));
    }
    ucTARGET=*(pODTbl+272*2+29);// 30th
    for(wCount=0; wCount<272; wCount++)
    {
        if(wCount<256)
            msWriteByte(SC16_12, (wCount == 29)?ucTARGET:(ucTARGET ^ *(pODTbl+272*2+wCount)));
        else
            msWriteByte(SC16_12, 0x00);
        msWrite2Byte(SC16_10, wCount|0x8000);
        while(_bit7_(msReadByte(SC16_11)));
    }
    ucTARGET=*(pODTbl+272*2+256+39);// 40th
    for(wCount=0; wCount<256; wCount++)
    {
        msWriteByte(SC16_18, (wCount == 39)?ucTARGET:(ucTARGET ^ *(pODTbl+272*2+256+wCount)));
        msWrite2Byte(SC16_16, wCount|0x8000);
        while(_bit7_(msReadByte(SC16_17)));
    }
    msWriteByte(SC16_02, 0x00);
}
//*************************************************************************************************
//Function:  msOD_OnOff()
//Description: Enable/Disable OverDrive and setup OD strength
//          ex:
//          msOD_OnOff( _OD_Disable, 0x50 ); //disable OD
//          msOD_OnOff( _OD_Enable, 0x5F ); //enable OD and set strength as 0x5F
//          OD strength: 0x50 (weak)  <------------>  0x5F (strong)
//
//*************************************************************************************************
void drvmsOverDriveOnOff(BOOL bEnable)
{
    if(bEnable)
    {
        msWriteByte(SC16_20, (OD_MODE<<1)|_BIT0);
    }
    else
    {
        msWriteBit(SC16_20, _DISABLE, _BIT0);
    }
}


void msDrvOverDriveMode(BYTE mode, BYTE h_scale, BYTE v_scale)
{
    DWORD u32OutputRes, u32PGBaseAddr, u32ODAddrLimit, u32DramOffSet;
    WORD u16ODBaseAddr, u16PGAddrLimit, u16PnlHDE, u16PnlVDE, u16TargetBitRate;
    BYTE u8ODBitPerPix, u8PGLength;
    BYTE u8ODScaleEnable = 0;

    drvmsOverDriveOnOff(FALSE);
    u8PGLength = 4;
    u16ODBaseAddr = DEF_IMI_OD_START_ADDR/32;

    switch(mode)
    {
        case OD_MODE_444:
            u8ODBitPerPix = 12;
            break;
        case OD_MODE_565:
            u8ODBitPerPix = 16;
            break;
        case OD_MODE_666:
            u8ODBitPerPix = 18;
            break;
        case OD_MODE_555:
            u8ODBitPerPix = 15;
            break;
        case OD_MODE_888:
            u8ODBitPerPix = 24;
            break;
        case OD_MODE_485:
            u8ODBitPerPix = 12;
            break;
        default:
            u8ODBitPerPix = 12;
            break;
    }

    u16PnlHDE = g_sPnlInfo.sPnlTiming.u16Width;
    u16PnlVDE = g_sPnlInfo.sPnlTiming.u16Height;
    u16TargetBitRate = (u8ODBitPerPix *64) + 3;

    if (mode == OD_MODE_485)
    {
        msWrite2Byte(SC16_42,0x1218);//compression factor
        msWrite2Byte(SC16_44,0x060C);
        msWrite2Byte(SC16_46,0x1417);
        msWrite2Byte(SC16_48,0x0E11);
    }

    if(h_scale)
    {
        u8ODScaleEnable |= 0x41;
        u16PnlHDE /= 2;
    }

    if(v_scale)
    {
        u8ODScaleEnable |= 0x42;
        u16PnlVDE /=2;
        msWriteByteMask(SC16_AB, 0x13, 0x3F);//set OD input Vsync start delay
    }
    u32OutputRes = ALIGN_64(u16PnlHDE) * u16PnlVDE;
    msWriteByte(SC16_D8,u8ODScaleEnable);
    msWrite2ByteMask(SC16_5A, u16TargetBitRate, 0x7FF);

    //OD base addr
    msWrite4Byte(SC16_2A, u16ODBaseAddr);

    //reg_pg_length
    msWriteByteMask(SC16_53, 0x00, 0x7F);// pg_length = (reg_pg_length+1)*4

    //reg_od_r_thrd
    msWriteByteMask(SC16_32, 0x16, 0xFF);

    //limit addr
    if ((g_sPnlInfo.sPnlTiming.u16Width==1920)&& g_sPnlInfo.sPnlTiming.u16Height==1200)
        u16PGAddrLimit = (WORD)((float)u32OutputRes * u8ODBitPerPix * 1.05  / 128 / u8PGLength / 256);
    else
        u16PGAddrLimit = (WORD)((float)u32OutputRes * u8ODBitPerPix * 1.1  / 128 / u8PGLength / 256);
    msWrite2ByteMask(SC16_4E, u16PGAddrLimit, 0x1FF);

    //Dram offset
    u32DramOffSet = (u16PGAddrLimit+1)*128*u8PGLength;

    //pg base addr
    u32PGBaseAddr = (DWORD)u16ODBaseAddr + u32DramOffSet;
    msWrite4Byte(SC16_8E, u32PGBaseAddr);

    //OD limit addr
    u32ODAddrLimit = u32PGBaseAddr + u16PGAddrLimit;
    msWrite4Byte(SC16_2E, u32ODAddrLimit);
    
    OD_DPRINTF("Required IMI OD SIZE(KB) = %d" , (u32ODAddrLimit - u16ODBaseAddr) * 32 / 1024);
    if(((u32ODAddrLimit - u16ODBaseAddr) * 32) > DEF_IMI_OD_SIZE)
    {
        OD_DPUTSTR("Required IMI OD SIZE EXCEEDED!!!");
    }
    
    //pg base limit enable
    msWriteByteMask(SC16_4F, BIT1, BIT4|BIT2|BIT1); //[2]:reg_pg_limit_en [1]:reg_pg_en

    //OD H scaling down size (for compress mode)
    if(h_scale)
    {
        msWrite2Byte(SC16_5E, ALIGN_4(u16PnlHDE));
    }
	else
	{
	    msWrite2Byte(SC16_5E, ALIGN_2(u16PnlHDE));
	}
	
    //OD Hsize & Vsize
    msWrite2Byte(SC16_DE,g_sPnlInfo.sPnlTiming.u16Width);
    msWrite2Byte(SC29_1E,g_sPnlInfo.sPnlTiming.u16Height);

    msWriteByteMask(SC16_22, UserprefOverDriveWeighting,0xFF); //OD weighting

    //OD on/off
    drvmsOverDriveOnOff(UserprefOverDriveSwitch);

    //OD_DPRINTF("===============PANEL_HEIGHT = %x  \r\n", PANEL_HEIGHT);
    //OD_DPRINTF("OD_h_scale = %x", h_scale);
}

void drvmsOverDriverInit(void)
{
    msWriteByte(SC16_24, 0x08);  //od active threshold
    msWriteBit(SC16_75,0,BIT7);  //Disable strength gradually slop bypass

    msDrvOverDriveMode(OD_MODE, OD_HSCAL_EN, OD_VSCAL_EN);
}

void drvmsOverDriveWindow(BYTE enable, WORD u16HST,WORD u16HEND,  WORD u16VST, WORD u16VEND)
{
    //BYTE ucMode = (msReadByte(SC16_20)>>1)&0x07;
    //BYTE ucH_Scale = msReadByte(SC16_D8)&0x01;
    //BYTE i;

    if(enable)
    {
        msWriteByteMask(SC16_20,BIT6|BIT7,BIT6|BIT7);

        //HST
        msWriteByteMask(SC16_7A,(u16HST<<4)&0x70,0x70);          //[6:4] LSB
        msWriteByteMask(SC16_26,(u16HST>>3)&0xFF,0xFF);          //[7:0]
        msWriteByteMask(SC16_7D,((u16HST>>11)<<4)&0x10,0x10);    //[4]   MSB
        //HEND
        msWrite2ByteMask(SC16_7A,(u16HEND<<7)&(BIT9|BIT8|BIT7),(BIT9|BIT8|BIT7));             //[9:7] LSB
        msWriteByteMask(SC16_27,(u16HEND>>3)&0xFF,0xFF);          //[7:0]
        msWriteByteMask(SC16_7D,((u16HEND>>11)<<5)&0x20,0x20);    //[5]   MSB

        //VST
        msWriteByteMask(SC16_7B,(u16VST<<2)&0x1C,0x1C);          //[4:2] LSB
        msWriteByteMask(SC16_28,(u16VST>>3)&0xFF,0xFF);          //[7:0]
        msWriteByteMask(SC16_7D,((u16VST>>11)<<6)&0x40,0x40);    //[6]   MSB
        //VEND
        msWriteByteMask(SC16_7B,(u16VEND<<5)&0xE0,0xE0);          //[7:5] LSB
        msWriteByteMask(SC16_29,(u16VEND>>3)&0xFF,0xFF);          //[7:0]
        msWriteByteMask(SC16_7D,((u16VEND>>11)<<7)&0x80,0x80);    //[7]   MSB
    }
    else
    {
        msWriteByteMask(SC16_20,0x00,BIT6|BIT7);
    }
#if 0
    msWriteByteMask(SC16_6C,BIT3, BIT3);
    Delay1ms(1000);
    msWriteByteMask(SC16_6C,0x00, BIT3);

    for(i=0;i<5;i++)
    {
        OD_DPRINTF("OD_Status = %x", msReadByte(SC16_70));
    }
#endif
}

#if ENABLE_OD_MTG
void msLoad_MultiODGain(WORD wVstart, BYTE u8MTGinterval, BYTE *pODMTGTbl)
{
    BYTE ucMultiGainIdx;

    if(wVstart > 511)
    {
        OD_DPUTSTR(" wVstart must be Less than 512\n");  
        wVstart = 511;
    }	

    if(u8MTGinterval > 3)
    {
        OD_DPUTSTR(" u8MTGinterval must be Less than 4\n");  
        u8MTGinterval = 3;
    }
 
    msWriteByteMask(SC29_80, (u8MTGinterval<<4), _BIT5|_BIT4); //[5:4]:Interval. 0:32,1:64,2:128,3:256 
    msWrite2ByteMask(SC29_82, wVstart , 0x1FF);  //[8:0]:OD dynamic mutli-gain V start
    
    for(ucMultiGainIdx = 0; ucMultiGainIdx < 11; ucMultiGainIdx++)
    {
        mdrv_SC_MultiODGain_Adjust(ucMultiGainIdx, pODMTGTbl[ucMultiGainIdx]);
        //OD_DPRINTF("pODMTGTbl[ucMultiGainIdx]==[%x] \n", pODMTGTbl[ucMultiGainIdx]);
    }
    
}

void mdrv_SC_MultiODGain_Enable(Bool bEnable)
{ 
    msWriteBit(SC29_02, bEnable, BIT0);//[0]:reg_gain_table_int_en
}

void mdrv_SC_MultiODGain_Adjust(BYTE u8MultiGainIdx, BYTE u8ODGain)
{
    if(u8MultiGainIdx > 10)
    {
        OD_DPUTSTR(" MultiGainIdx must be Less than 11\n");  
        return;
    }
 
    msWriteByte((SC29_04+u8MultiGainIdx), u8ODGain);//region gain value 0~10
}
#endif


#endif
