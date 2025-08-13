
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
#if ENABLE_OD_AutoDownTBL
void drvmsLoadODTable(const BYTE *pODTbl, ODTBL_INDEX Tblindex)
{
     WORD wCount=0,wXORKeyIdx,wi,wj;
     BYTE ucXORKey,OD_data=0xFF,ucSramIdx,ucTblData;
     DWORD dwXdataAddr=0;
     BYTE ucReg;

    ucReg = msReadByte(SC16_20);

    msWriteBit(SC16_20, 0, _BIT0);
    while(msReadByte(SC16_20)&_BIT0);
    msWriteByte(SC16_02, 0x00);
    //clear Mem data
    for(wj=0; wj<289; wj++)
    {
        for(wi=0; wi<OD_BusLength; wi++)
        {
            msMemWriteByte(MIU_OD_ADL_ADDR_START + wi+wj*OD_BusLength, 0x00);
        }
    }
    //De-compress OD LUT and write to DRAM
    while(wCount<289)
    {
        for(ucSramIdx = 0;ucSramIdx < 4;ucSramIdx++)
        {
            if(ucSramIdx<3)
                wXORKeyIdx =(9 + 10*ucSramIdx) + 272*(WORD)(ucSramIdx);
            else
                wXORKeyIdx =(9 + 10*ucSramIdx) + 256*(WORD)(ucSramIdx-2) + 272*2;

            if( wCount == 9 && ucSramIdx == 0)
                ucXORKey = 0;
            else if( wCount == 19 && ucSramIdx == 1)
                ucXORKey = 0;
            else if( wCount == 29 && ucSramIdx == 2)
                ucXORKey = 0;
            else if( wCount == 39 && ucSramIdx == 3)
                ucXORKey = 0;
            else
                ucXORKey =*(pODTbl+wXORKeyIdx);

            if(ucSramIdx < 2 && wCount < 272)
            {
                ucTblData = *(pODTbl+(272*(WORD)ucSramIdx) + wCount ) ;
                OD_data = ucXORKey ^ ucTblData;
                msMemWriteByte(MIU_OD_ADL_ADDR_START+dwXdataAddr+ ucSramIdx  ,OD_data);
                if(wCount<10)
                {
                    OD_DPRINTF(" ReadDRAM==[%d] \n",msMemReadByte(MIU_OD_ADL_ADDR_START+dwXdataAddr+ ucSramIdx ));
                }
				 
            }
            else if(ucSramIdx >=2 && wCount < 256)
            {
                ucTblData = *(pODTbl+(256*(WORD)(ucSramIdx-2) + 272*2) + wCount ) ;
                OD_data = ucXORKey ^ ucTblData;
                msMemWriteByte(MIU_OD_ADL_ADDR_START+dwXdataAddr+ ucSramIdx  ,OD_data);
                if(wCount<10)
                {
                    OD_DPRINTF(" ReadDRAM==[%d] \n",msMemReadByte(MIU_OD_ADL_ADDR_START+dwXdataAddr+ ucSramIdx ));
                }
            }
            else if (((ucSramIdx ==0x00||ucSramIdx ==0x01) && (wCount >= 272 && wCount<289)) ||((ucSramIdx ==0x02||ucSramIdx ==0x03) && (wCount >= 256 && wCount<289)))
            {
                msMemWriteByte(MIU_OD_ADL_ADDR_START+dwXdataAddr+ ucSramIdx  ,0x00);
                if(wCount<10)
                {
                    OD_DPRINTF(" ReadDRAM==[%d] \n",msMemReadByte(MIU_OD_ADL_ADDR_START+dwXdataAddr+ ucSramIdx ));
                }
            }
            msMemWriteByte(MIU_OD_ADL_ADDR_START+dwXdataAddr+ 0x08  ,0x01);//enable 1st od table
            msMemWriteByteMask(MIU_OD_ADL_ADDR_START+dwXdataAddr+ 0x0E  ,(Tblindex<<1), _BIT2|_BIT1);//[114:113] table index
        }
        wCount++;
        dwXdataAddr += OD_BusLength; //16 or 32 bytes alligment
    }
    drvmsSetODAutoDownload(ENABLE, ADL_MODE_ENABLE, (MIU_OD_ADL_ADDR_START), 289);


    msWriteByteMask(SC16_03, _BIT2|_BIT0, _BIT2|_BIT0);//od table auto load enable
    ForceDelay1ms(50);
    msWriteByteMask(SC16_03, 0x00, _BIT2|_BIT0);//od table auto load disable
    msWriteByte(SC16_20, ucReg);

}

#if ENABLE_OD_VRR
void drvmsLoadAlphaTable(const BYTE *pAlphaTbl)
{
    WORD wCount = 0;
    WORD wi,wj;

    DWORD dwXdataAddr=0;
    
    drvmsOverDriveOnOff(FALSE); // Disable OD before load code
    //clear Mem data
    for(wj=0; wj<512; wj++)
    {
        for(wi=0; wi<OD_BusLength; wi++)
        {
            msMemWriteByte(MIU_OD_ADL_ADDR_START + wi+wj*OD_BusLength, 0x00);
        }
    }
    
    while(wCount<512)
    {
        msMemWriteByte(MIU_OD_ADL_ADDR_START+dwXdataAddr  ,*(pAlphaTbl+wCount));
        msMemWriteByte(MIU_OD_ADL_ADDR_START+dwXdataAddr+ 0x08  ,0x01);//enable 1st od table
        msMemWriteByteMask(MIU_OD_ADL_ADDR_START+dwXdataAddr+ 0x0E  ,_BIT2|_BIT1, _BIT2|_BIT1);//[114:113] table index 2'b = 11 alpha table
        wCount++;
        dwXdataAddr += OD_BusLength; //16 or 32 bytes alligment
    }
    drvmsSetODAutoDownload(ENABLE, ADL_MODE_ENABLE, (MIU_OD_ADL_ADDR_START), 512);
    msWriteByteMask(SC16_03, _BIT2|_BIT0, _BIT2|_BIT0);//od table auto load enable
    ForceDelay1ms(50);
    msWriteByteMask(SC16_03, 0x00, _BIT2|_BIT0);//od table auto load disable
}
#endif

#else
void drvmsLoadODTable(const BYTE *pODTbl, ODTBL_INDEX Tblindex)
{
    WORD wCount/*,wi,wj*/;
    BYTE ucTARGET;

    if(0 == pODTbl) return;
    OD_DPUTSTR("Load_OD_Table");

    //msWriteBit(SC16_20,0, _BIT0); // Disable OD before load code
    drvmsOverDriveOnOff(FALSE);  // Disable OD before load code
    msWriteByteMask(SC16_03, (Tblindex<<4), _BIT5|_BIT4);// reg_od_sram_rw_io_sel[13:12] 
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

#if ENABLE_OD_VRR
void drvmsLoadAlphaTable(const BYTE *pAlphaTbl)
{
    WORD wCount;
    
    drvmsOverDriveOnOff(FALSE); // Disable OD before load code
    msWriteByte(SC16_02, 0x10); // h01[4] od alpha table sram enable

    for(wCount=0; wCount<512; wCount++)
    {
        msWriteByte(SC16_06, *(pAlphaTbl+wCount));
        msWrite2Byte(SC16_04, wCount|0x8000);//Od table sram1 address
        while(_bit7_(msReadByte(SC16_05)));
    }
    msWriteByte(SC16_02, 0x00); 

}
#endif

#endif
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
    DWORD u32OutputRes, u32PGBaseAddr, u32ODAddrLimit, u32DramOffSet, u32ODBaseAddr, u32WAdrLimit, u32RAdrLimit;
    WORD  u16PGAddrLimit, u16PnlHDE, u16PnlVDE, u16TargetBitRate;
    BYTE u8ODBitPerPix; 
    BYTE u8ODScaleEnable = 0;

    drvmsOverDriveOnOff(FALSE);
    u32ODBaseAddr = MIU_OD_ADDR_START/32;
    
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
    u16TargetBitRate = (u8ODBitPerPix * 64) + 3;
    

    //od frame buffer write address limit 
    u32WAdrLimit = ((u8ODBitPerPix * u16PnlHDE * u16PnlVDE + 4096) / 32) + 1;
    msWrite3Byte(SC16_72, u32WAdrLimit ); //reg_od_wadr_max_limit

    //od frame buffer read address limit 
    u32RAdrLimit = (u16PnlHDE * u16PnlVDE) + 1; 
    msWrite3Byte(SC16_76, u32RAdrLimit ); //reg_od_radr_max_limit

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
        msWriteByteMask(SC16_AB, 0x14, 0x3F);//set OD input Vsync start delay
    }
    u32OutputRes = ALIGN_64(u16PnlHDE) * u16PnlVDE;
    msWriteByte(SC16_D8,u8ODScaleEnable);
    msWrite2ByteMask(SC16_5A, u16TargetBitRate, 0x7FF);

    //OD base addr
    msWrite4Byte(SC16_2A, u32ODBaseAddr);

    //reg_pg_length
    msWriteByteMask(SC16_53, 0x03, 0x7F); // pg_length = (reg_pg_length+1)*4

    //reg_od_r_thrd
    msWriteByteMask(SC16_32, 0x10, 0xFF); //0x16 -> 0x2E for FPGA environment compress mode status NG
    
    //limit addr
    u16PGAddrLimit = (WORD)((float)u32OutputRes * u8ODBitPerPix * extra_size_factor  / pg_entry_per_req / pg_length / OD_MIU_BusLength);
    msWrite2ByteMask(SC16_4E, u16PGAddrLimit, 0x1FF);
    
    //Dram offset
    u32DramOffSet = (u16PGAddrLimit + 1) * pg_entry_per_req * pg_length;
    
    //pg base addr
    u32PGBaseAddr = (DWORD)u32ODBaseAddr + u32DramOffSet;
    msWrite4Byte(SC16_8E, u32PGBaseAddr);

    //OD limit addr
    u32ODAddrLimit = u32PGBaseAddr + u16PGAddrLimit;
    msWrite4Byte(SC16_2E, u32ODAddrLimit);
  
    //pg base limit enable
    msWriteByteMask(SC16_4F, BIT1, BIT4|BIT2|BIT1); //[1]:reg_pg_limit_en [2]:reg_pg_en
    //If run compress mode, it will switch to page mode, no need to set reg_pg_en

    //OD H scaling down size for compress mode
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

#if ENABLE_OD_AutoDownTBL
void drvmsSetODAutoDownload(BOOL bEnable, ADL_MODE eMode, DWORD u32Addr, WORD u16Length)
{
    msWriteBit(SC67_02, eMode, BIT4);
    msWriteBit(SC67_02, bEnable, BIT1);//client2_1
    msWriteBit(SC67_42, bEnable, BIT0);//client2_2
    msWriteBit(SC67_42, bEnable, BIT1);//client2_3
    msWrite2Byte(SC67_08, u32Addr>>5);
    msWrite2Byte(SC67_0A, u32Addr>>21);
    msWrite2Byte(SC67_12, u16Length);
    msWrite2Byte(SC67_18, u16Length);
}

void drvmsLoadODTable2DRAM(const BYTE *pODTbl, ODTBL_INDEX Tblindex, DWORD OD_ADL_ADDR_START)
{
     WORD wCount=0,wXORKeyIdx,wi,wj;
     BYTE ucXORKey,OD_data=0xFF,ucSramIdx,ucTblData;
     DWORD dwXdataAddr=0;
  
    //clear Mem data
    for(wj=0; wj<289; wj++)
    {
        for(wi=0; wi<OD_BusLength; wi++)
        {
            msMemWriteByte(OD_ADL_ADDR_START + wi+wj*OD_BusLength, 0x00);
        }
    }
    //De-compress OD LUT and write to DRAM
    while(wCount<289)
    {
        for(ucSramIdx = 0;ucSramIdx < 4;ucSramIdx++)
        {
            if(ucSramIdx<3)
                wXORKeyIdx =(9 + 10*ucSramIdx) + 272*(WORD)(ucSramIdx);
            else
                wXORKeyIdx =(9 + 10*ucSramIdx) + 256*(WORD)(ucSramIdx-2) + 272*2;

            if( wCount == 9 && ucSramIdx == 0)
                ucXORKey = 0;
            else if( wCount == 19 && ucSramIdx == 1)
                ucXORKey = 0;
            else if( wCount == 29 && ucSramIdx == 2)
                ucXORKey = 0;
            else if( wCount == 39 && ucSramIdx == 3)
                ucXORKey = 0;
            else
                ucXORKey =*(pODTbl+wXORKeyIdx);

            if(ucSramIdx < 2 && wCount < 272)
            {
                ucTblData = *(pODTbl+(272*(WORD)ucSramIdx) + wCount ) ;
                OD_data = ucXORKey ^ ucTblData;
                msMemWriteByte(OD_ADL_ADDR_START+dwXdataAddr+ ucSramIdx  ,OD_data);
                if(wCount<10)
                {
                    OD_DPRINTF(" ReadDRAM==[0x%x] \n",msMemReadByte(OD_ADL_ADDR_START+dwXdataAddr+ ucSramIdx ));
                }
            }
            else if(ucSramIdx >=2 && wCount < 256)
            {
                ucTblData = *(pODTbl+(256*(WORD)(ucSramIdx-2) + 272*2) + wCount ) ;
                OD_data = ucXORKey ^ ucTblData;
                msMemWriteByte(OD_ADL_ADDR_START+dwXdataAddr+ ucSramIdx  ,OD_data);
                if(wCount<10)
                {
                    OD_DPRINTF(" ReadDRAM==[0x%x] \n",msMemReadByte(OD_ADL_ADDR_START+dwXdataAddr+ ucSramIdx ));
                }
            }
            else if (((ucSramIdx ==0x00||ucSramIdx ==0x01) && (wCount >= 272 && wCount<289)) ||((ucSramIdx ==0x02||ucSramIdx ==0x03) && (wCount >= 256 && wCount<289)))
            {
                msMemWriteByte(OD_ADL_ADDR_START+dwXdataAddr+ ucSramIdx  ,0x00);
                if(wCount<10)
                {
                    OD_DPRINTF(" ReadDRAM==[%d] \n",msMemReadByte(OD_ADL_ADDR_START+dwXdataAddr+ ucSramIdx ));
                }
            }
            msMemWriteByte(OD_ADL_ADDR_START+dwXdataAddr+ 0x08  ,0x01);//enable 1st od table
            msMemWriteByteMask(OD_ADL_ADDR_START+dwXdataAddr+ 0x0E  ,(Tblindex<<1), _BIT2|_BIT1);//[114:113] table index
        }
        wCount++;
        dwXdataAddr += OD_BusLength; //16 or 32 bytes alligment
    }
}

void drvmsTrigODADL(DWORD OD_ADL_ADDR_START)
{
    BYTE ucReg;

    ucReg = msReadByte(SC16_20);

    msWriteBit(SC16_20, 0, _BIT0);
    while(msReadByte(SC16_20)&_BIT0);
    msWriteByte(SC16_02, 0x00);

    drvmsSetODAutoDownload(ENABLE, ADL_MODE_ENABLE, (OD_ADL_ADDR_START), 289);

    msWriteByteMask(SC16_03, _BIT2|_BIT0, _BIT2|_BIT0);//od table auto load enable
    ForceDelay1ms(50);
    msWriteByteMask(SC16_03, 0x00, _BIT2|_BIT0);//od table auto load disable
    msWriteByte(SC16_20, ucReg);
}

#if ENABLE_OD_VRR
void drvmsLoadODAlphaTable2DRAM(const BYTE *pAlphaTbl, DWORD OD_alpha_ADL_ADDR_START)
{
    WORD wCount = 0;
    WORD wi,wj;

    DWORD dwXdataAddr=0;
    
    drvmsOverDriveOnOff(FALSE); // Disable OD before load code
    //clear Mem data
    for(wj=0; wj<512; wj++)
    {
        for(wi=0; wi<OD_BusLength; wi++)
        {
            msMemWriteByte(OD_alpha_ADL_ADDR_START + wi+wj*OD_BusLength, 0x00);
        }
    }
    
    while(wCount<512)
    {
        msMemWriteByte(OD_alpha_ADL_ADDR_START+dwXdataAddr  ,*(pAlphaTbl+wCount));
        msMemWriteByte(OD_alpha_ADL_ADDR_START+dwXdataAddr+ 0x08  ,0x01);//enable 1st od table
        msMemWriteByteMask(OD_alpha_ADL_ADDR_START+dwXdataAddr+ 0x0E  ,_BIT2|_BIT1, _BIT2|_BIT1);//[114:113] table index 2'b = 11 alpha table
        wCount++;
        dwXdataAddr += OD_BusLength; //16 or 32 bytes alligment
    }

}

void drvmsTrigODAlphaADL(DWORD OD_alpha_ADL_ADDR_START)
{
    BYTE ucReg;

    ucReg = msReadByte(SC16_20);

    msWriteBit(SC16_20, 0, _BIT0);
    while(msReadByte(SC16_20)&_BIT0);
    msWriteByte(SC16_02, 0x00);

    drvmsSetODAutoDownload(ENABLE, ADL_MODE_ENABLE, (OD_alpha_ADL_ADDR_START), 512);
    msWriteByteMask(SC16_03, _BIT2|_BIT0, _BIT2|_BIT0);//od table auto load enable
    ForceDelay1ms(50);
    msWriteByteMask(SC16_03, 0x00, _BIT2|_BIT0);//od table auto load disable
    msWriteByte(SC16_20, ucReg);
}
#endif
#endif


#if ENABLE_OD_VRR
void drvmsOverDriverVRRInit(void)
{
    drvmsOverDriveOnOff(FALSE);

    //VRR frame rate filter setting
    drvmsOverDriveVRRFrameRateFLTSet();
 
    //Set VRR frame rate range according to PanelVfreq
	drvmsOverDriveVRRFrameRateNodeSet((((PanelMinVfreq+PanelVfreq)/2)/10), (PanelVfreq/10), (((PanelVfreq+PanelMaxVfreq)/2)/10));

    //60H[8] = 1  //set interpolation enable
    msWriteByteMask(SC15_C1, BIT0, BIT0);
    //60H[15] = 1  // default force table0
    drvmsOverDriveForceTableEnable(TRUE);
	drvmsOverDriveVRRTableSelect(OD_TABLE0_0);
    drvmsOverDriveOnOff(UserprefOverDriveSwitch);
}

void drvmsOverDriveForceTableEnable(BOOL bEnable)
{
    //h60[15] reg_vrr_force_table_en 
    msWriteBit(SC15_C1, bEnable, BIT7);
}

void drvmsOverDriveVRRTableSelect(VRR_TBLSEC_INDEX Tblindex)
{
    //This setting is valid when h60[15]=1 (force table enable)
    // h60[14:12] reg_vrr_sel_table_idx
    msWriteByteMask(SC15_C1, (Tblindex<<4), 0x70);
}

void drvmsOverDriveVRRFrameRateNodeSet(WORD u16VRRNode0, WORD u16VRRNode1, WORD u16VRRNode2)
{
    //set VRR frame rate node0  h69[8:0] 
    msWrite2ByteMask(SC15_D2, u16VRRNode0, 0x1FF);
    //set VRR frame rate node1  h6a[8:0] 
    msWrite2ByteMask(SC15_D4, u16VRRNode1, 0x1FF);
    //set VRR frame rate node2  h6b[8:0] 
    msWrite2ByteMask(SC15_D6, u16VRRNode2, 0x1FF);
	
    OD_DPRINTF("FrameRate Node 0 = %d Hz",  u16VRRNode0);
    OD_DPRINTF("FrameRate Node 1 = %d Hz",  u16VRRNode1);
    OD_DPRINTF("FrameRate Node 2 = %d Hz",  u16VRRNode2);
	
}

WORD drvmsOverDriveVRRFrameRateGet(void)
{
    WORD u16FrameRate;
    //Frame rate report register readback SC_15 h5e[9:0] 9.1 bits 
    u16FrameRate = (msRead2Byte(SC15_BC) & 0x3FF);
    
    if(u16FrameRate & BIT0)
        return ((u16FrameRate >> 1)+1); //rounding
    else
        return (u16FrameRate >> 1);
}

void drvmsOverDriveVRRFrameRateFLTSet(void)
{
    //h61[3:0]   = 0x4 od vrr stable seq length 
    //h61[7:4]   = 0x3 od vrr prev diff length
    //h61[11:8]  = 0x2 od vrr positive seq length
    //h61[15:12] = 0x4 od vrr negative seq length
    msWrite2Byte(SC15_C2, 0x4234);

    //h62[14:12] = 0x2 od vrr stable seq threshold
	msWriteByteMask(SC15_C5, 0x20, 0x70);
  
	//h63[6:0]   = 0x32 od vrr diff seq threshold
	//h63[12:8]  = 0x10 od vrr diff stable seq threshold
	msWrite2Byte(SC15_C6, 0x1032);
	
	//h64[9:0]   = 0x1FF od vrr positive seq threshold 
	msWrite2Byte(SC15_C8, 0x01FF);
	
	//h65[9:0]   = 0x14  od vrr negative seq threshold
	msWrite2Byte(SC15_CA, 0x0014);

	//h66[5:0]   = 0x6 od vrr positive sense threshold
	//h66[13:8]  = 0x6 od vrr negative sense threshold
	msWrite2Byte(SC15_CC, 0x0606);

	//h67[4:0]   = 0x4 od vrr positive freq threshold
	//h67[12:8]  = 0x4 od vrr negative freq threshold
	msWrite2Byte(SC15_CE, 0x0404);

	//h68[4:0]   = 0x4 od vrr delta over min
	msWrite2Byte(SC15_D0, 0x0004);
	
}

#endif

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
