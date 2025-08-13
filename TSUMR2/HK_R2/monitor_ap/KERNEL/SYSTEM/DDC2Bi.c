#include <math.h>
#include "types.h"
#include "board.h"
#include "Global.h"
#include "menudef.h"
#include "menufunc.h"
#include "Adjust.h"
//#include "msADC.h"
#include "drvADC.h"
#include "ms_reg.h"
#include "msOSD.h"
#include "AutoFunc.h"
#include "Ms_rwreg.h"
#include "Debug.h"
#include "Common.h"
#include "misc.h"
#include "mStar.h"
#include "Power.h"
#include "Keypaddef.h"
#include "DDC2Bi.H"
#include "UserPref.h"
#include "msflash.h"
#include "NVRam.h"
#include "DDC.h"
#include "GPIO_DEF.h"
#include "DDCColorHandler.h"
#if ENABLE_DPS
#include "drvOSD.h"
#include "drvGPIO.h"
#endif
#include "dpCommon.h"
#include "MsIRQ.h"
#include "MsOS.h"

////////////////////////////////////////////////////
#include "drvDDC2Bi.h"
//#include "halRwreg.h"
///////////////////////////////////////////////////

#if ENABLE_MTK_TEST
#include "DDCMtkTestHandler.h"
#endif

#if (CHIP_ID == CHIP_MT9700)
#include "DDCMCCSMSCHandler.h"
#endif

#define CTEMP_6500K CTEMP_Warm1
#define CTEMP_9300K CTEMP_Cool1
#define UserPrefRedColor6500K UserPrefRedColorWarm1
#define UserPrefGreenColor6500K UserPrefGreenColorWarm1
#define UserPrefBlueColor6500K UserPrefBlueColorWarm1
#define UserPrefRedColor9300K UserPrefRedColorCool1
#define UserPrefGreenColor9300K UserPrefGreenColorCool1
#define UserPrefBlueColor9300K UserPrefBlueColorCool1
extern Bool SetColor( void );
extern Bool SetColorTemp(void);
extern void CheckModeSettingRange( void );
extern Bool CheckFactorySettingOutOfRange( void );
extern void Init_FactorySetting( void );
extern Bool xdata bFlashWriteFactory;

BYTE AlignControl( void );
#define ASSETSTARTADDR          0x600
#define ASSETLENGTH             64
//#if ENABLE_DPS
//BYTE u8BackupOutDE_H,u8BackupOutDE_L;
//#endif

BYTE code MODEL_TYPE[] =
{
    Model
};
extern xdata BYTE MenuPageIndex;
extern xdata BYTE MenuItemIndex;
BYTE code CAP_VCP[] =
{
    "("
    "vcp(02 04 05 06 08 0E 10 12 14(01 05 06 08 0B) 16 18 1A 1E 20 30 3E "
    #if INPUT_TYPE!=INPUT_1A
    "60(01 03) "
    #endif
    #if AudioFunc
    "62 "
    #endif
    "6C 6E 70 C8 "
    "B0 B6 DF)"
    "prot(monitor)"
    "type(LCD)"
    "cmds(01 02 03 07 0C F3)"
    "mccs_ver(2.1)"
    "asset_eep(64)"
    "mpu_ver("FWVersion")"
    "model("Model")"
    "mswhql(1)"
    ")"
};
#if INPUT_TYPE!=INPUT_1A
BYTE code DVI_CAP_VCP[] =
{
    "("
    "vcp(02 04 05 08 10 12 14(01 05 06 08 0B) 16 18 1A 60(01 03)"
#if AudioFunc
    "62 "
#endif
    "6C 6E 70 C8 "
    "B0 B6 DF)"
    "prot(monitor)"
    "type(LCD)"
    "cmds(01 02 03 07 0C F3)"
    "mccs_ver(2.1)"
    "asset_eep(64)"
    "mpu_ver("FWVersion")"
    "model("Model")"
    "mswhql(1)"
    ")"
};
#endif
void DDC2Bi_InitRx( void )
{
    rxStatus = DDC2B_CLEAR;
    rxIndex = 0;
}
void DDC2Bi_Init( void )
{
    drvDDC2Bi_Init();
}
void DDC2Bi_GetTxBuffer( void )
{
    BYTE i;
    for( i = MAX_TXWAIT_DELAY; i > 0; i-- )
        if( txStatus == DDC2B_MSGON )
        {
            Delay1ms( 1 );
        }
    txStatus = DDC2B_CLEAR;
}
BYTE DDC2Bi_ComputeChecksum( BYTE count )
{
    BYTE CheckSum;
    BYTE i = 0;
    CheckSum = INITTxCK;
    for( ; count > 0; count-- )
        CheckSum ^= DDCBuffer[i++];
    return CheckSum;
}
void DDC2Bi_MessageReady( void )
{
    drvDDC2Bi_MessageReady();
}
void DDC2Bi_ParseCommand( void )
{
    drvDDC2Bi_ParseCommand();
}
void DDC2Bi_CommandHandler( void )
{
    if( rxStatus == DDC2B_COMPLETED )
    {
        DDC2Bi_ParseCommand();
        Clr_RxBusyFlag();
        DDC2Bi_InitRx();
    }
}
void DoDDCCI( BYTE Source ) // Annie 2006.7.5
{
    drvDoDDCCI(Source);
}
void ISR_DDC2Bi(void)
{
    BYTE u8BackupXsramData = msReadByte(0x003C00);
    WORD u16BackupXsramAddr = msRead2Byte(0x003C02);
    if( ADC_INT_FLAG &( BIT3 | BIT4 ) )
    {
        D2B_Enable_Clk_Gating_A0(FALSE); // A0 auto clock gating disable
        DoDDCCI( InputCombo_A0 );
        CLR_ADC_INT_FLAG();
    }
  #if DDC_Port_D0
    else if( DDCDVI0_INT_FLAG &( BIT3 | BIT4 ) )
    {
        D2B_Enable_Clk_Gating_D0(FALSE); //D0 auto clock gating disable
        DoDDCCI( InputCombo_D0 );
        CLR_DVI0_INT_FLAG();
    }
  #endif
  #if DDC_Port_D1
    else if( DDCDVI1_INT_FLAG &( BIT3 | BIT4 ) )
    {
        D2B_Enable_Clk_Gating_D1(FALSE); // D1 auto clock gating disable
        DoDDCCI( InputCombo_D1 );
        CLR_DVI1_INT_FLAG();
    }
  #endif
  #if DDC_Port_D2
    else if( DDCDVI2_INT_FLAG &( BIT3 | BIT4 ) )
    {
        D2B_Enable_Clk_Gating_D2(FALSE); // D2 auto clock gating disable
        DoDDCCI( InputCombo_D2 );
        CLR_DVI2_INT_FLAG();
    }
  #endif
	#if DDC_Port_D3
    else if( DDCDVI3_INT_FLAG &( BIT3 | BIT4 ) )
    {
        D2B_Enable_Clk_Gating_D3(FALSE);  // D3 auto clock gating disable
        DoDDCCI( InputCombo_D3 );
        CLR_DVI3_INT_FLAG();
    }
	#endif
    msWriteByte(0x003C00, u8BackupXsramData);
  msWrite2Byte(0x003C02, u16BackupXsramAddr);
   MsOS_EnableInterrupt(E_INT_PM_IRQ_D2B);
}


#if  ENABLE_DP_INPUT


//#if (CHIP_ID == CHIP_MT9700)
//MCCS usage
BYTE xdata g_ucDDC_TxRecord=0;
BYTE xdata dpNullMsg[3]={0x6e, 0x80, 0xbe};
BYTE xdata u8DDC_RxRecord = 0;
//================= DP =====================
#if (CHIP_ID == CHIP_MT9701)
void DDC2BI_DP(BYTE dprx_id)
{
    BYTE xdata u8AUXCH_RxCmd;
    BYTE xdata u8DDC_Length;
    BYTE xdata AuxData;
    WORD xdata  OffsetAux = 0;

    OffsetAux = DP_REG_OFFSET_AUX_PM0(dprx_id);

    u8AUXCH_RxCmd = msRegs(REG_DPRX_AUX_PM0_1C_L + OffsetAux)&0x0F;
    u8DDC_Length = msRegs(REG_DPRX_AUX_PM0_1D_L + OffsetAux);
    #if 1
    //===============//<write>PC Read Command============================
      if(u8AUXCH_RxCmd & _BIT0)    //DP Tx
      {
         if (u8DDC_Length != 0x0)
          {
              if((rxStatus == DDC2B_CLEAR) && txLength)
              {
                   while(u8DDC_Length-- && txLength)
                   {
                      if(g_ucDDC_TxRecord)
                      {
						  if((g_ucDDC_TxRecord - 1) < DDC_BUFFER_LENGTH)
						  {
						  	  msRegs(REG_DPRX_AUX_PM0_79_H + OffsetAux)= DDCBuffer[g_ucDDC_TxRecord-1];
						  }

                          txLength--;
                      }
                      else
                      {
                          msRegs(REG_DPRX_AUX_PM0_79_H + OffsetAux) = DDC2B_DEST_ADDRESS;
                      }
                      g_ucDDC_TxRecord++;
                      msRegs(REG_DPRX_AUX_PM0_79_L + OffsetAux) |= BIT0;
                      while( ! (((msRegs(REG_DPRX_AUX_PM0_79_L + OffsetAux))&BIT2)))     // Wait
                      {
                      };
                      msRegs(REG_DPRX_AUX_PM0_79_L + OffsetAux) |= BIT1;
                  }
              }
              else
              {
                  while(u8DDC_Length--)
                  {
                      msRegs(REG_DPRX_AUX_PM0_79_H + OffsetAux) = dpNullMsg[g_ucDDC_TxRecord++];
                      msRegs(REG_DPRX_AUX_PM0_79_L + OffsetAux) |= BIT0;
                      while( ! (((msRegs(REG_DPRX_AUX_PM0_79_L +OffsetAux))&BIT2)))     // Wait
                      {
                      };
                      msRegs(REG_DPRX_AUX_PM0_79_L + OffsetAux) |= BIT1;
                  }
              }
          }

      }
      //===============//<write>PC Write Command============================
      else if((u8AUXCH_RxCmd == 0x00) || (u8AUXCH_RxCmd == 0x04))    //DP Rx
      {
          if (u8DDC_Length)
          {
                  if(u8DDC_RxRecord == 0)
                  {
                      u8DDC_RxRecord = TRUE;
                      rxIndex = 0;
                      msRegs(REG_DPRX_AUX_PM0_78_L +OffsetAux) = msRegs(REG_DPRX_AUX_PM0_78_L + OffsetAux)|BIT0;
                      while( ! (msRegs(REG_DPRX_AUX_PM0_78_L + OffsetAux)&BIT2))     // Wait
                      {};
                      msRegs(REG_DPRX_AUX_PM0_78_L + OffsetAux) = msRegs(REG_DPRX_AUX_PM0_78_L + OffsetAux)|BIT1;
                      AuxData = msRegs(REG_DPRX_AUX_PM0_3A_H + OffsetAux);
                      u8DDC_Length--;
                  }
                  while(u8DDC_Length--)
                  {
                      msRegs(REG_DPRX_AUX_PM0_78_L + OffsetAux) = msRegs(REG_DPRX_AUX_PM0_78_L + OffsetAux)|BIT0;
                      while( ! (msRegs(REG_DPRX_AUX_PM0_78_L + OffsetAux)&BIT2))     // Wait
                      {};
                      msRegs(REG_DPRX_AUX_PM0_78_L + OffsetAux) = msRegs(REG_DPRX_AUX_PM0_78_L + OffsetAux)|BIT1;

					  if(rxIndex < DDC_BUFFER_LENGTH)
					  {
                      	  DDCBuffer[rxIndex++] = msRegs(REG_DPRX_AUX_PM0_3A_H + OffsetAux);
					  }
                  }
          }

         if (u8AUXCH_RxCmd == 0x00)
          {
              u8DDC_RxRecord=FALSE;
              g_ucDDC_TxRecord = 0;
              rxStatus = DDC2B_COMPLETED;
              txLength = 0;
              rxInputPort = D2B_FIFOMODE_DDCPORT2INPUT(dprx_id);
           }

      }

      msRegs(REG_DPRX_AUX_PM0_1F_L + OffsetAux) = 0;
      msRegs(REG_DPRX_AUX_PM0_3B_H + OffsetAux) = msRegs(REG_DPRX_AUX_PM0_3B_H + OffsetAux) |BIT0; //trigger
    #endif
      return;

}
//#endif
#else
void DDC2BI_DP(BYTE dprx_id)
{
    BYTE xdata u8AUXCH_RxCmd;
    BYTE xdata u8DDC_Length;
    BYTE xdata AuxData;
    WORD xdata  OffsetAux = 0;

    OffsetAux = DP_REG_OFFSETAUX(dprx_id);

    u8AUXCH_RxCmd = msRegs(REG_DPRX_AUX_1C_L + OffsetAux)&0x0F;
    u8DDC_Length = msRegs(REG_DPRX_AUX_1D_L + OffsetAux);
    #if 1
    //===============//<write>PC Read Command============================
      if(u8AUXCH_RxCmd & _BIT0)    //DP Tx
      {
         if (u8DDC_Length != 0x0)
          {
              if((rxStatus == DDC2B_CLEAR) && txLength)
              {
                   while(u8DDC_Length-- && txLength)
                   {
                      if(g_ucDDC_TxRecord)
                      {
						  if((g_ucDDC_TxRecord - 1) < DDC_BUFFER_LENGTH)
						  {
						  	  msRegs(REG_DPRX_AUX_79_H + OffsetAux)= DDCBuffer[g_ucDDC_TxRecord-1];
						  }

                          txLength--;
                      }
                      else
                      {
                          msRegs(REG_DPRX_AUX_79_H + OffsetAux) = DDC2B_DEST_ADDRESS;
                      }
                      g_ucDDC_TxRecord++;
                      msRegs(REG_DPRX_AUX_79_L + OffsetAux) |= BIT0;
                      while( ! (((msRegs(REG_DPRX_AUX_79_L + OffsetAux))&BIT2)))     // Wait
                      {
                      };
                      msRegs(REG_DPRX_AUX_79_L + OffsetAux) |= BIT1;
                  }
              }
              else
              {
                  while(u8DDC_Length--)
                  {
                      msRegs(REG_DPRX_AUX_79_H + OffsetAux) = dpNullMsg[g_ucDDC_TxRecord++];
                      msRegs(REG_DPRX_AUX_79_L + OffsetAux) |= BIT0;
                      while( ! (((msRegs(REG_DPRX_AUX_79_L +OffsetAux))&BIT2)))     // Wait
                      {
                      };
                      msRegs(REG_DPRX_AUX_79_L + OffsetAux) |= BIT1;
                  }
              }
          }

      }
      //===============//<write>PC Write Command============================
      else if((u8AUXCH_RxCmd == 0x00) || (u8AUXCH_RxCmd == 0x04))    //DP Rx
      {
          if (u8DDC_Length)
          {
                  if(u8DDC_RxRecord == 0)
                  {
                      u8DDC_RxRecord = TRUE;
                      rxIndex = 0;
                      msRegs(REG_DPRX_AUX_78_L +OffsetAux) = msRegs(REG_DPRX_AUX_78_L + OffsetAux)|BIT0;
                      while( ! (msRegs(REG_DPRX_AUX_78_L + OffsetAux)&BIT2))     // Wait
                      {};
                      msRegs(REG_DPRX_AUX_78_L + OffsetAux) = msRegs(REG_DPRX_AUX_78_L + OffsetAux)|BIT1;
                      AuxData = msRegs(REG_DPRX_AUX_3A_H + OffsetAux);
                      u8DDC_Length--;
                  }
                  while(u8DDC_Length--)
                  {
                      msRegs(REG_DPRX_AUX_78_L + OffsetAux) = msRegs(REG_DPRX_AUX_78_L + OffsetAux)|BIT0;
                      while( ! (msRegs(REG_DPRX_AUX_78_L + OffsetAux)&BIT2))     // Wait
                      {};
                      msRegs(REG_DPRX_AUX_78_L + OffsetAux) = msRegs(REG_DPRX_AUX_78_L + OffsetAux)|BIT1;

					  if(rxIndex < DDC_BUFFER_LENGTH)
					  {
                      	  DDCBuffer[rxIndex++] = msRegs(REG_DPRX_AUX_3A_H + OffsetAux);
					  }
                  }
          }

         if (u8AUXCH_RxCmd == 0x00)
          {
              u8DDC_RxRecord=FALSE;
              g_ucDDC_TxRecord = 0;
              rxStatus = DDC2B_COMPLETED;
              txLength = 0;
              rxInputPort = D2B_FIFOMODE_DDCPORT2INPUT(dprx_id);
           }

      }

      msRegs(REG_DPRX_AUX_1F_L + OffsetAux) = 0;
      msRegs(REG_DPRX_AUX_3B_H + OffsetAux) = msRegs(REG_DPRX_AUX_3B_H + OffsetAux) |BIT0; //trigger
    #endif
      return;

}
#endif
#endif




#if DDCCI_FILTER_FUNCTION // 111228 coding not ready
Bool DDCCICommandExecutable(void)
{
    BYTE CommandCode, CPCode;
    Bool bresult = TRUE;

    CommandCode = DDCBuffer[1];
    CPCode = DDCBuffer[2];

// check others conditions

    if(!DDCciFlag)
    {
        if( CommandCode == SETVCPFeature || CommandCode == GETVCPFeature )
        {
            CPCode = DDCBuffer[2];
            if( CPCode != 0xFE )
                return FALSE;
        }
        else
        {
            return FALSE;
        }
    }

    if(CommandCode == SETVCPFeature || CommandCode == SETPagedVCP)
    {
        switch(CPCode)
        {
            case Geometry_Reset:
            case ADJ_Clock:
            case Auto_Setup:
            case Auto_Color:
            case ADJ_HorizontalPos:
            case ADJ_VerticalPos:
            case ADJ_Phase:
                if(!CURRENT_INPUT_IS_VGA())
                    bresult = FALSE;
            break;

            case ADJ_Brightness:
            case ADJ_Contrast:
                if(!UserPrefDcrMode&&(UserPrefECOMode==ECO_Standard))
                    bresult = FALSE;
            break;

            default:
            break;
        }
    }
    else if(CommandCode == GETVCPFeature || CommandCode == GETPagedVCP)
    {
        switch(CPCode)
        {
            case Geometry_Reset:
            case ADJ_Clock:
            case Auto_Setup:
            case Auto_Color:
            case ADJ_HorizontalPos:
            case ADJ_VerticalPos:
            case ADJ_Phase:
                if(!CURRENT_INPUT_IS_VGA())
                    bresult = FALSE;
            break;

            default:
            break;
        }
    }

    return bresult;

}
#endif
//===================================================
BYTE AlignControl( void )
{
    BYTE CommandCode, CPCode, ValueH, ValueL, RetValueH, RetValueL=0, PageValue;
    BYTE TPValue;
    WORD WordValue;
    WORD WordAddr;

    CommandCode = DDCBuffer[1];

    PageValue = 0;
#if DDCCI_ENABLE
    if ( !DDCciFlag )           //We can adjust the W/B By DDC2BI When DDCCI Is Disable
#endif //When DDCCI_ENABLE=0,we use below to disable DDCCI and Use it to adjust W/B
    {
        if( CommandCode == SETVCPFeature || CommandCode == GETVCPFeature )
        {
            CPCode = DDCBuffer[2];
            if( CPCode != 0xFE )
                return FALSE;
        }
        else
            return FALSE;
    }

#if DDCCI_DEBUG_ENABLE
    Osd_Show();
    Osd_Draw4Num( 15, 0, DDCBuffer[1] );
    Osd_Draw4Num( 15, 1, DDCBuffer[2] );
    Osd_Draw4Num( 15, 2, DDCBuffer[3] );
    Osd_Draw4Num( 15, 3, DDCBuffer[4] );
    Osd_Draw4Num( 15, 4, DDCBuffer[5] );
    //Delay1ms(1000);
#endif

    if( CommandCode == SETVCPFeature )
    {
        CPCode = DDCBuffer[2];
        RetValueH = DDCBuffer[3];
        RetValueL = DDCBuffer[4];
        WordValue = (( WORD )DDCBuffer[3] << 8 ) | (( WORD )DDCBuffer[4] );
    }
    else if( CommandCode == GETVCPFeature )
    {
        CPCode = DDCBuffer[2];
    }
    else if( CommandCode == ModeChangeRequest )
    {
        CPCode = DDCBuffer[2];
        ValueH = DDCBuffer[3];
    }
    else if( CommandCode == GETPagedVCP )
    {
        PageValue = DDCBuffer[2];
        CPCode = DDCBuffer[3];
    }
    else if( CommandCode == SETPagedVCP )
    {
        PageValue = DDCBuffer[2];
        CPCode = DDCBuffer[3];
        RetValueH = DDCBuffer[4];
        RetValueL = DDCBuffer[5];
        WordValue = (( WORD )DDCBuffer[4] << 8 ) | (( WORD )DDCBuffer[5] );
    }
    else if( CommandCode == CapabilitiesRequest )        // 1-6
    {
        RetValueH = DDCBuffer[2];
        RetValueL = DDCBuffer[3];
        WordAddr = (( WORD )DDCBuffer[2] << 8 ) | (( WORD )DDCBuffer[3] );
        #if INPUT_TYPE!=INPUT_1A
        if( CURRENT_INPUT_IS_TMDS()
#if ENABLE_DP_INPUT
        || CURRENT_INPUT_IS_DISPLAYPORT()
#endif
            )
            WordValue = sizeof( DVI_CAP_VCP );
        else
        #endif
            WordValue = sizeof( CAP_VCP );
        if( WordAddr >= WordValue )
        {
            DDCBuffer[0] = 0x83;
            DDCBuffer[1] = 0xE3;
            return 3;
        }
#define CapLen CPCode
        CapLen = ( WordAddr + 32 >= WordValue ) ? ( WordValue - WordAddr ) : 32;
        DDCBuffer[0] = CapLen + 3 + 0x80;
        DDCBuffer[1] = 0xE3;
        DDCBuffer[2] = RetValueH;
        DDCBuffer[3] = RetValueL;
        for( ValueL = 4; ValueL < CapLen + 4; ValueL++ )
        {
            #if INPUT_TYPE!=INPUT_1A
            if( CURRENT_INPUT_IS_TMDS()
        #if ENABLE_DP_INPUT
        || CURRENT_INPUT_IS_DISPLAYPORT()
        #endif
        )
                DDCBuffer[ValueL] = DVI_CAP_VCP[WordAddr + ValueL - 4];
            else
            #endif
                DDCBuffer[ValueL] = CAP_VCP[WordAddr + ValueL - 4];
        }
#undef CapLen
        return DDCBuffer[0] &~0x80;
    }
    else if( CommandCode == EnableApp )
    {
        DDCBuffer[0] = 0x80;
        return 0; //0xFE;
    }
    else if( CommandCode == AppTest )
    {
        DDCBuffer[0] = 0x80;
        return 0; //0xFD;
    }
    else if( CommandCode == GetTimingRequest )
    {
        DDCBuffer[0] = 0x06;
        DDCBuffer[1] = 0x4E;
        DDCBuffer[2] = SyncPolarity( SrcFlags );
        if( UnsupportedModeFlag )
            DDCBuffer[2] |= 0x80;
        if( !InputTimingStableFlag )
            DDCBuffer[2] |= 0x40;
        WordValue = HFreq( SrcHPeriod ) * 10;
        DDCBuffer[3] = WordValue >> 8;
        DDCBuffer[4] = WordValue & 0x00FF;
        WordValue = VFreq( WordValue, SrcVTotal );
        DDCBuffer[5] = WordValue >> 8;
        DDCBuffer[6] = WordValue & 0x00FF;
        return 6;
    }
//*******************************************************************************************
//MSTART OnLine AutoColorTool COMMAND***************************************************************************
//*******************************************************************************************
	else if ( CommandCode == MStar_COMMAND )
	{
        return ColorCalibrationHandler(MAIN_WINDOW);
	}
#if ENABLE_MTK_TEST
    else if (MTK_TEST_CMD == CommandCode)
        return MtkTestCmdHandler();
#endif

#if 0//ENABLE_DPS
     else if( CommandCode == MS_DPS_COMMAND)
    {
        if(DDCBuffer[2] == MS_SET_EnableFrameColor_DPS)
        {
            if(DDCBuffer[3]==0x01)
            {
                bRunToolFlag = 1;
                #if 0//CHIP_ID == CHIP_TSUMV
                old_msWriteBit(OSD1_02,1,BIT2);
                old_msWriteBit(OSD1_03,1,BIT3);
                old_msWriteByte(OSD2_E0,0x05);
                old_msWriteByte(OSD2_E8,0x80);
                old_msWriteByte(OSD2_E9,0x07);
                old_msWriteByte(OSD2_EA,0x38);
                old_msWriteByte(OSD2_EB,0x04);
                old_msWriteByte(OSD2_EC,0x3F);
                old_msWriteByte(OSD2_F4,0x66);
                old_msWriteByte(OSD2_F5,0x06);
                old_msWriteBit( SC0_04,0, BIT6 );
                #elif 0//CHIP_ID == CHIP_TSUMU
                old_msWriteBit(0x1C81, 0 , BIT7);
                old_msWriteByteMask(0x1C15,BIT3|BIT2, BIT3|BIT2);
                old_msWriteBit(0x1C16, 1 , BIT7);
                old_msWriteBit(0x1D04, 1 , BIT2);
                #else
                mStar_SetupFreeRunMode();
                drvOSD_FrameColorEnable(1);
                #endif
                return 3;
            }
            else
            {
                bRunToolFlag = 0;
                #if 0//CHIP_ID == CHIP_TSUMV
                old_msWriteByte(OSD2_E0,00);
                old_msWriteBit(OSD1_02,0,BIT2);
                old_msWriteBit(OSD1_03,0,BIT3);
                old_msWriteBit( SC0_04,1, BIT6 );
                #elif 0//CHIP_ID == CHIP_TSUMU
                old_msWriteByteMask(0x1C15,0, BIT3|BIT2);
                old_msWriteBit(0x1C16, 0 , BIT7);
                old_msWriteBit(0x1D04, 0 , BIT2);
                #else
                //old_msWriteBit( SC0_02,0, BIT7 );
                drvOSD_FrameColorEnable(0);
                #endif
                return 3;
            }
        }
        else if(DDCBuffer[2] == MS_SET_FrameColor_DPS)
        {
                #if 0//CHIP_ID == CHIP_TSUMV
                old_msWriteByte(OSD2_F6,DDCBuffer[3]);
                old_msWriteByte(OSD2_F7,DDCBuffer[4]);
                old_msWriteByte(OSD2_F8,DDCBuffer[5]);
                #elif 0//CHIP_ID == CHIP_TSUMU
                old_msWriteByte(0x1D06,DDCBuffer[3]);
                old_msWriteByte(0x1D07,DDCBuffer[4]);
                old_msWriteByte(0x1D08,DDCBuffer[5]);
                #else
                drvOSD_FrameColorRGB(DDCBuffer[3],DDCBuffer[4],DDCBuffer[5]);
                #endif
                return 5;
        }
        else if(DDCBuffer[2] == MS_SET_AdjustPWM_DPS)
        {
            mStar_AdjustBrightness(DDCBuffer[3]);
        }
        else if(DDCBuffer[2] == MS_SET_Adjust_REAL_PWM_DPS)
        {
            drvGPIO_SetBacklightDuty(BrightnessPWM,DDCBuffer[3]);
        }
    }
#endif

        //preliminary judge cmd finish, start detail process--------------------------------------------------

    if( CommandCode == GETVCPFeature || CommandCode == GETPagedVCP )
    {
        if( CommandCode == GETVCPFeature )
        {
            DDCBuffer[0] = 0x88;
            DDCBuffer[1] = 0x02;
            DDCBuffer[2] = 0x00;
            DDCBuffer[3] = CPCode;
            DDCBuffer[4] = 0x01;
            ValueH = 0x00;
            ValueL = 0xFF;
            RetValueH = 0x00;
            TPValue = 0x01;
        }
        else
        {
            DDCBuffer[0] = 0x89;
            DDCBuffer[1] = 0xC7;
            DDCBuffer[2] = 0x00;
            DDCBuffer[3] = PageValue;
            DDCBuffer[4] = CPCode;
            DDCBuffer[5] = 0x01;
            ValueH = 0x00;
            ValueL = 0xFF;
            RetValueH = 0x00;
            TPValue = 0x01;
        }

#if ENABLE_HDMI_DRR_MCCS
        if((CommandCode == GETVCPFeature) && (CPCode == HDMI_FREESYNC_VCP))
        {
            TPValue = 0x00;
            ValueH = 0x00;
            ValueL = 0x01;
            RetValueH = 0x00;
            RetValueL = UserprefFreeSyncMode;
        }
        else
#endif
        if( CPCode == NewControl && PageValue == 0 )
        {
            RetValueL = 1;
            ValueL = 2;
        }
        else if( CPCode == ALL_Reset && PageValue == 0 )
        {
            RetValueL = 0;
            ValueL = 1;
        }

        else if( CPCode == BriCon_Reset && PageValue == 0 )
        {
            RetValueL = 0;
            ValueL = 1;
        }
        else if( CPCode == Geometry_Reset && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            RetValueL = 0;
            ValueL = 1;
        }
        else if( CPCode == Color_Reset && PageValue == 0 )
        {
            RetValueL = 0;
            ValueL = 1;
        }
        else if( CPCode == ADJ_Clock && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            RetValueH = 0;
            RetValueL = GetClockValue();
            ValueH = 0;
            ValueL = 100;
            TPValue = 0x00;
        }
        else if( CPCode == ADJ_Brightness && PageValue == 0 )
        {
            ValueL = MaxBrightnessValue - MinBrightnessValue;
            RetValueL = UserPrefBrightness - MinBrightnessValue;
            TPValue = 0x00;
        }
        else if( CPCode == ADJ_Contrast && PageValue == 0 )
        {
            RetValueH = 0;
            RetValueL = GetContrastValue();
            ValueH = 0;
            ValueL = 100;
            TPValue = 0x00;
        }
        else if( CPCode == Select_Color_Preset && PageValue == 0 )
        {
            ValueL = 0x0D; //0x0B
            if( UserPrefColorTemp == CTEMP_SRGB )
                RetValueL = 0x01;
            else if( UserPrefColorTemp == CTEMP_6500K )
                RetValueL = 0x05;
            else if( UserPrefColorTemp == CTEMP_Normal )
                RetValueL = 0x06;
            else if( UserPrefColorTemp == CTEMP_9300K )
                RetValueL = 0x08;
            else if( UserPrefColorTemp == CTEMP_USER )
                RetValueL = 0x0B;
            TPValue = 0x00;
        }
        else if(( CPCode == ADJ_Red_Gain ) && PageValue == 0 )
        {
            ValueL = 100;
            RetValueL = GetRedColorValue();
            TPValue = 0x00;
        }
        else if(( CPCode == ADJ_Green_Gain ) && PageValue == 0 )
        {
            ValueL = 100;
            RetValueL = GetGreenColorValue();
            TPValue = 0x00;
        }
        else if(( CPCode == ADJ_Blue_Gain ) && PageValue == 0 )
        {
            ValueL = 100;
            RetValueL = GetBlueColorValue();
            TPValue = 0x00;
        }
        else if( CPCode == Auto_Setup && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            RetValueL = 0;
            ValueL = 1;
        }
        else if( CPCode == Auto_Color && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            RetValueL = 0;
            ValueL = 1;
        }
        else if( CPCode == ADJ_HorizontalPos && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            ValueH = 0; //(MaxHStart - MinHStart)>> 8;
            ValueL = 100; // (MaxHStart - MinHStart) & 0x00FF;
            RetValueH = ( MaxHStart - UserPrefHStart ) >> 8;
            RetValueL = ( MaxHStart - UserPrefHStart ) & 0x00FF;
            TPValue = 0x00;
        }
        else if( CPCode == ADJ_VerticalPos && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            RetValueH = 0x00;
            RetValueL = GetVPositionValue();
            ValueH = 0x0;
            ValueL = 100;
            TPValue = 0x00;
        }
        else if( CPCode == ADJ_Phase && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            RetValueH = 0x00;
            RetValueL = GetFocusValue();
            ValueH = 0x0;
            ValueL = 100;
            TPValue = 0x00;
        }
        else if( CPCode == Select_InputSource )
        {
            ValueH = 0x00;
            ValueL = 0x04;
            if(CURRENT_INPUT_IS_VGA())
                RetValueL = 0x01;
            else
                RetValueH = 0x03;
            TPValue = 0x00;
        }
#if AudioFunc
        else if( CPCode == ADJ_SpeakerVolume && PageValue == 0 )
        {
            ValueH = 0;
            ValueL = 100;
            RetValueL = UserPrefVolume;
            RetValueH = 0;
            TPValue = 0x00;
        }
#endif
        else if( CPCode == ADJ_Red_Blacklevel && PageValue == 0 )
        {
            ValueH = 0x00;
            ValueL = 100;
            RetValueL = UserprefRedBlackLevel;
            TPValue = 0;
        }
        else if( CPCode == ADJ_Green_Blacklevel && PageValue == 0 )
        {
            ValueH = 0x00;
            ValueL = 100;
            RetValueL = UserprefGreenBlackLevel;
            TPValue = 0;
        }
        else if( CPCode == ADJ_Blue_Blacklevel && PageValue == 0 )
        {
            ValueH = 0x00;
            ValueL = 100;
            RetValueL = UserprefBlueBlackLevel;
            TPValue = 0;
        }
        else if( CPCode == H_Frequency && PageValue == 0 )
        {
            ValueH = 0xFF;
            ValueL = 0xFF;
            WordValue = HFreq( SrcHPeriod ) * 10;
            RetValueH = WordValue >> 8;
            RetValueL = WordValue & 0x00ff;
            TPValue = 0x01;
        }
        else if( CPCode == V_Frequency && PageValue == 0 )
        {
            ValueH = 0xFF;
            ValueL = 0xFF;
            WordValue = HFreq( SrcHPeriod ) * 10;
            WordValue = VFreq( WordValue, SrcVTotal );
            RetValueH = WordValue >> 8;
            RetValueL = WordValue & 0x00ff;
            TPValue = 0x01;
        }
        else if( CPCode == FlatPanellayout )
        {
            ValueL = 8;
            RetValueL = 1;
        }
        else if( CPCode == MonitorType && PageValue == 0 )
        {
            RetValueH = 0;
            RetValueL = 3;
            ValueH = 0x00;
            ValueL = 0x08;
            TPValue = 0x00;
        }
        else if( CPCode == DisplayControlType )
        {
            ValueH = 0xFF;
            ValueL = 0x16;
            RetValueH = 0;
            RetValueL = 5; // Mean Mstar
        }
        else if( CPCode == FWLevel )
        {
            ValueH = 0xFF;
            ValueL = 0xFF;
            RetValueL = 0;
            RetValueH = 0;
        }
        else if( CPCode == ADJ_Language && PageValue == 0 )
        {
#if 0//LANGUAGE_TYPE
            ValueL = LANG_Nums;
            if( UserPrefLanguage == LANG_English )
                RetValueL = 2;
            else if( UserPrefLanguage == LANG_Spanish )
                RetValueL = 10;
            else if( UserPrefLanguage == LANG_France )
                RetValueL = 3;
            else if( UserPrefLanguage == LANG_German )
                RetValueL = 4;
            else if( UserPrefLanguage == LANG_Italian )
                RetValueL = 5;
            else if( UserPrefLanguage == LANG_Portugues )
                RetValueL = 8;
            else if( UserPrefLanguage == LANG_Russia )
                RetValueL = 9;
            else if( UserPrefLanguage == LANG_SChina )
                RetValueL = 1;
            TPValue = 0x00;
#endif
        }
        else if( CPCode == VCPVersion && PageValue == 0 )
        {
            RetValueL = 1;
            RetValueH = 2;
            ValueH = 0xFF;
            ValueL = 0xFF;
        }
        else
        {
            DDCBuffer[2] = 0x01;
            RetValueL = 0x00;
            ValueH = 0xFF;
            ValueL = 0xFF;
        }

        if( CommandCode == GETVCPFeature )
        {
            DDCBuffer[4] = TPValue;
            DDCBuffer[5] = ValueH;
            DDCBuffer[6] = ValueL;
            DDCBuffer[7] = RetValueH;
            DDCBuffer[8] = RetValueL;
            #if (CHIP_ID == CHIP_MT9700)
            if ((mapi_MCCS_MSC_GetEnabled() == TRUE) && (mapi_MCCS_MSC_IsValid(CPCode) == TRUE))
                return mapi_MCCS_MSC_Handler((BYTE*)(&DDCBuffer[0]), 8, GETVCPFeature, CPCode, DDCBuffer[3], DDCBuffer[4],  ((WORD)DDCBuffer[3]<<8) | ((WORD)DDCBuffer[4]));
            else
            #endif
			    return 8;
        }
        else
        {
            DDCBuffer[5] = TPValue;
            DDCBuffer[6] = ValueH;
            DDCBuffer[7] = ValueL;
            DDCBuffer[8] = RetValueH;
            DDCBuffer[9] = RetValueL;
            return 9;
        }

    }

    else if( CommandCode == SETVCPFeature || CommandCode == SETPagedVCP )
    {
        DDCBuffer[2] = 0x00; // Result Code
        ValueH = 0x00;
        ValueL = 0xFF;
        TPValue = 0x01;
        if( CPCode == NewControl && PageValue == 0 )
        {
            ValueL = 2;
        }
        else if( CPCode == ALL_Reset && PageValue == 0 )
        {
            if( WordValue != 0 )
            {
                ResetAllSetting();
                // wait for checking
                //old_msWriteByte( BK0_2B, 0x80 );
                //old_msWriteByte( BK0_2C, 0x80 );
                //old_msWriteByte( BK0_2D, 0x80 );
            }
            ValueL = 1;
        }
        else if( CPCode == BriCon_Reset && PageValue == 0 )
        {
            if( WordValue != 0 )
            {
                UserPrefBrightness = DefBrightness;
                UserPrefContrast = DefContrast;

                UserPrefBrightnessUser = UserPrefBrightness;
                UserPrefContrastUser = UserPrefContrast;

                UserPrefBrightnessCool1 = UserPrefBrightness;
                UserPrefContrastCool1 = UserPrefContrast;

                UserPrefBrightnessWarm1 = UserPrefBrightness;
                UserPrefContrastWarm1 = UserPrefContrast;

                UserPrefBrightnessNormal = UserPrefBrightness;
                UserPrefContrastNormal = UserPrefContrast;

                UserPrefBrightnessSRGB = UserPrefBrightness;
                UserPrefContrastSRGB = UserPrefContrast;


                mStar_AdjustBrightness( UserPrefBrightness );
                mStar_AdjustContrast( UserPrefContrast );
            }
            ValueL = 1;
        }
#if ENABLE_VGA_INPUT
        else if( CPCode == Geometry_Reset && PageValue == 0 && INPUT_IS_VGA(rxInputPort))
        {
            if( WordValue != 0 && CURRENT_INPUT_IS_VGA())
                ResetGeometry();
            ValueL = 1;
            TPValue = 0x00;
        }
#endif
        else if( CPCode == Color_Reset && PageValue == 0 )
        {
            UserPrefRedColorUser = DefRedColor;
            UserPrefGreenColorUser = DefGreenColor;
            UserPrefBlueColorUser = DefBlueColor;
            UserprefRedBlackLevel = 50;
            UserprefGreenBlackLevel = 50;
            UserprefBlueBlackLevel = 50;
            mStar_AdjustUserPrefBlacklevel( UserprefRedBlackLevel, UserprefGreenBlackLevel, UserprefBlueBlackLevel );
            UserPrefColorTemp = CTEMP_Warm1; //CTEMP_9300K 20051115 wmz
            if( WordValue != 0 )
                SetColorTemp();
            ValueL = 1;
        }
#if ENABLE_VGA_INPUT
        else if( CPCode == ADJ_Clock && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            {
                UserPrefHTotal = (( WordValue * ( MaxClock - MinClock ) + 50 ) / 0x64 + MinClock );
                drvADC_AdjustHTotal( UserPrefHTotal );
            }
            TPValue = 0x00;
        }
#endif 
        else if( CPCode == ADJ_Brightness && PageValue == 0 )
        {
           if(!UserPrefDcrMode&&(UserPrefECOMode==ECO_Standard))  //111108 add B/C adjustment condition - ID40189
            {
                UserPrefBrightness = RetValueL + MinBrightnessValue;
                if( UserPrefColorTemp == CTEMP_USER )
                {
                    UserPrefBrightnessUser = UserPrefBrightness;
                }
                else if( UserPrefColorTemp == CTEMP_Cool1 )
                {
                    UserPrefBrightnessCool1 = UserPrefBrightness;
                }
                else if( UserPrefColorTemp == CTEMP_Warm1 )
                {
                    UserPrefBrightnessWarm1 = UserPrefBrightness;
                }
                else if( UserPrefColorTemp == CTEMP_Normal )
                {
                    UserPrefBrightnessNormal = UserPrefBrightness;
                }
                else if( UserPrefColorTemp == CTEMP_SRGB )
                {
                    UserPrefBrightnessSRGB = UserPrefBrightness;
                }
                mStar_AdjustBrightness( UserPrefBrightness );
            }
            ValueL = MaxBrightnessValue - MinBrightnessValue;
            TPValue = 0x00;
        }
        else if( CPCode == ADJ_Contrast && PageValue == 0 )
        {

            //Osd_Hide();
            //if (RetValueL <= (MaxContrastValue - MinContrastValue))
            if(!UserPrefDcrMode&&(UserPrefECOMode==ECO_Standard))  //111108 add B/C adjustment condition - ID40189
            {
                UserPrefContrast = ( WORD )( RetValueL * ( MaxContrastValue - MinContrastValue ) ) / 100 + MinContrastValue;
                if( UserPrefColorTemp == CTEMP_USER )
                {
                    UserPrefContrastUser = UserPrefContrast;
                }
                else if( UserPrefColorTemp == CTEMP_Cool1 )
                {
                    UserPrefContrastCool1 = UserPrefContrast;
                }
                else if( UserPrefColorTemp == CTEMP_Warm1 )
                {
                    UserPrefContrastWarm1 = UserPrefContrast;
                }
                else if( UserPrefColorTemp == CTEMP_Normal )
                {
                    UserPrefContrastNormal = UserPrefContrast;
                }
                else if( UserPrefColorTemp == CTEMP_SRGB )
                {
                    UserPrefContrastSRGB = UserPrefContrast;
                }
                mStar_AdjustContrast( UserPrefContrast );
            }
            ValueL = 100;
            TPValue = 0x00;
        }
        else if( CPCode == Select_Color_Preset && PageValue == 0 )
        {
            if( RetValueL == 0x01 )
                UserPrefColorTemp = CTEMP_SRGB;
            else if( RetValueL == 0x05 )
                UserPrefColorTemp = CTEMP_6500K;
            else if( RetValueL == 0x06 )
                UserPrefColorTemp = CTEMP_Normal;
            else if( RetValueL == 0x08 )
                UserPrefColorTemp = CTEMP_9300K;
            else if( RetValueL == 0x0B )
                UserPrefColorTemp = CTEMP_USER;
            SetColorTemp();//Setcolortemp();
            ValueL = 0x0D; //0x0B
            TPValue = 0x00;
        }
        else if( CPCode == ADJ_Red_Gain && PageValue == 0 )
        {
            if( UserPrefColorTemp == CTEMP_USER )
            {
                //if(RetValueL<=20)
                // RetValueL=20;
                UserPrefRedColorUser = UserPrefRedColor = ((( DWORD )RetValueL * ( MaxColorValue - MinColorValue ) ) / 100 + MinColorValue );
                //UserPrefRedColorUser=UserPrefRedColor = RetValueL*MaxColorValue/100;
                mStar_AdjustRedColor( UserPrefRedColor, UserPrefContrast );
            }
            ValueL = 100;//(MaxColorValue - MinColorValue);
            TPValue = 0x00;
        }
        else if( CPCode == ADJ_Green_Gain && PageValue == 0 )
        {
            if( UserPrefColorTemp == CTEMP_USER )
            {
                //if(RetValueL<=20)
                // RetValueL=20;
                UserPrefGreenColorUser = UserPrefGreenColor = ((( DWORD )RetValueL * ( MaxColorValue - MinColorValue ) ) / 100 + MinColorValue );
                // UserPrefGreenColorUser = UserPrefGreenColor = RetValueL*MaxColorValue/100 ;
                mStar_AdjustGreenColor( UserPrefGreenColor, UserPrefContrast );
            }
            ValueL = 100;//(MaxColorValue - MinColorValue);
            TPValue = 0x00;
        }
        else if( CPCode == ADJ_Blue_Gain && PageValue == 0 )
        {
            if( UserPrefColorTemp == CTEMP_USER )
            {
                //if(RetValueL<=20)
                //RetValueL=20;
                UserPrefBlueColorUser = UserPrefBlueColor = ((( DWORD )RetValueL * ( MaxColorValue - MinColorValue ) ) / 100 + MinColorValue );
                //UserPrefBlueColorUser = UserPrefBlueColor = RetValueL*MaxColorValue/100;
                mStar_AdjustBlueColor( UserPrefBlueColor, UserPrefContrast );
            }
            ValueL = 100;//(MaxColorValue - MinColorValue);
            TPValue = 0x00;
        }
#if ENABLE_VGA_INPUT
        else if( CPCode == Auto_Setup && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            if( WordValue != 0 )
                AutoConfig();
            ValueL = 1;
        }
        else if( CPCode == Auto_Color && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            if( WordValue != 0 )
                mStar_AutoColor();
            ValueL = 1;
        }
        else if( CPCode == ADJ_HorizontalPos && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            {
                UserPrefHStart = MaxHStart - WordValue;
                mStar_AdjustHPosition( UserPrefHStart );
            }
            ValueH = ( MaxHStart - MinHStart ) >> 8;
            ValueL = ( MaxHStart - MinHStart ) & 0x00FF;
            TPValue = 0x00;
        }
        else if( CPCode == ADJ_VerticalPos && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            UserPrefVStart = ( WORD )WordValue * ( MaxVStart - MinVStart ) / 100 + MinVStart;
            mStar_AdjustVPosition( UserPrefVStart );
            TPValue = 0x00;
        }
        else if( CPCode == ADJ_Phase && PageValue == 0 && CURRENT_INPUT_IS_VGA())
        {
            {
                UserPrefPhase = ( WORD )(WordValue * (MAX_PHASE_VALUE+1)+50) / 100 ;
                if( UserPrefPhase > MAX_PHASE_VALUE )
                    UserPrefPhase = MAX_PHASE_VALUE;
                drvADC_SetPhaseCode( UserPrefPhase );
            }
            TPValue = 0x00;
        }
#endif
        else if( CPCode == DDCCI_Settings && PageValue == 0 )
        {
            if(CURRENT_INPUT_IS_VGA())
            {
            #if USEFLASH
                Set_FlashForceSaveModeFlag();
            #else
                SaveModeSetting();
            #endif
            }
            #if USEFLASH
            Set_FlashForceSaveMonitorFlag();
            #else
            SaveMonitorSetting();
            #endif
            TPValue = 0x00;
        }
        else if( CPCode == Select_InputSource && PageValue == 0 )
        {
            if( RetValueL == 0x01 || RetValueL == 0x02 )
            {
 				UserPrefInputType=Input_VGA;
				ChangeSource();
             }

            if( RetValueL == 0x03 || RetValueL == 0x04 )
            {
                #if INPUT_TYPE!=INPUT_1A
				UserPrefInputType=Input_Digital;
				ChangeSource();
				#endif
            }

            TPValue = 0x00;
        }
#if AudioFunc
        else if( CPCode == ADJ_SpeakerVolume && PageValue == 0 )
        {
            UserPrefVolume = RetValueL;
            msAPI_AdjustVolume( UserPrefVolume );
            TPValue = 0x00;
        }
#endif
        else if(( CPCode == ADJ_Red_Blacklevel || CPCode == ADJ_Green_Blacklevel || CPCode == ADJ_Blue_Blacklevel ) && PageValue == 0 )
        {
            if( CPCode == ADJ_Red_Blacklevel )
                UserprefRedBlackLevel = RetValueL;
            else if( CPCode == ADJ_Green_Blacklevel )
                UserprefGreenBlackLevel = RetValueL;
            else if( CPCode == ADJ_Blue_Blacklevel )
                UserprefBlueBlackLevel = RetValueL;
            mStar_AdjustUserPrefBlacklevel( UserprefRedBlackLevel, UserprefGreenBlackLevel, UserprefBlueBlackLevel );
            //SaveMonitorSetting();
        }
        else if( CPCode == ADJ_Language && PageValue == 0 )
        {
#if 0//LANGUAGE_TYPE
            if( RetValueL == 2 )
                UserPrefLanguage = LANG_English;
            else if( RetValueL == 10 )
                UserPrefLanguage = LANG_Spanish;
            else if( RetValueL == 3 )
                UserPrefLanguage = LANG_France;
            else if( RetValueL == 4 )
                UserPrefLanguage = LANG_German;
            else if( RetValueL == 5 )
                UserPrefLanguage = LANG_Italian;
            else if( RetValueL == 8 )
                UserPrefLanguage = LANG_Portugues;
            else if( RetValueL == 9 )
                UserPrefLanguage = LANG_Russia;
            else if( RetValueL == 1 )
                UserPrefLanguage = LANG_SChina;
            ValueL = LANG_Nums;
            TPValue = 0x00;
#endif
        }
        else if( CPCode == SyncType && PageValue == 0 )
        {
            ValueL = 3;
            RetValueL = 0;
        }
        else if( CPCode == PowerMode && PageValue == 0 )
        {
            printf("[MCCS] Set PowerMode : %d\n", RetValueL);
            if(RetValueL == PowerMode_On)
            {
            }
            else if(RetValueL == PowerMode_Standby)
            {
                Set_ForceMCCSWakeUpFlag();
                PowerOffSystem();
            }
            else if(RetValueL == PowerMode_Suspend)
            {
                Set_ForceMCCSWakeUpFlag();
                PowerOffSystem();
            }
            else if(RetValueL == PowerMode_Off)
            {
                Set_ForceMCCSWakeUpFlag();
                PowerOffSystem();
            }
            else if(RetValueL == PowerMode_SwitchOff)
            {
                Set_ForceMCCSWakeUpFlag();
                PowerOffSystem();
            }

        }
        else
        {
            DDCBuffer[2] = 0x01;
        }

        if( CommandCode == SETVCPFeature )
        {
            #if (CHIP_ID == CHIP_MT9700)
            if ((mapi_MCCS_MSC_GetEnabled() == TRUE) && (mapi_MCCS_MSC_IsValid(CPCode) == TRUE))
                return mapi_MCCS_MSC_Handler((BYTE*)(&DDCBuffer[0]), 0, SETVCPFeature, CPCode, DDCBuffer[3], DDCBuffer[4],  ((WORD)DDCBuffer[3]<<8) | ((WORD)DDCBuffer[4]));
            else
            #endif
                return 0;
        }
        else
        {
            DDCBuffer[0] = 0x89;
            DDCBuffer[1] = 0xC7;
            DDCBuffer[3] = PageValue;
            DDCBuffer[4] = CPCode;
            DDCBuffer[5] = TPValue;
            DDCBuffer[6] = ValueH;
            DDCBuffer[7] = ValueL;
            DDCBuffer[8] = RetValueH;
            DDCBuffer[9] = RetValueL;
            return 9;
        }
    }
    else if( CommandCode == SaveCurrentSetting )
    {
    #if USEFLASH
        Set_FlashForceSaveMonitorFlag();
        Set_FlashForceSaveModeFlag();
    #else
        SaveMonitorSetting();
        SaveModeSetting();
    #endif
        return 0;
    }
    else if( CommandCode == GETVCPPage )
    {
        DDCBuffer[0] = 0x83;
        DDCBuffer[1] = 0xC5;
        DDCBuffer[2] = 0;
        DDCBuffer[3] = 3;
        return 3;
    }
    DDCBuffer[0] = 0x80;
    return 0;
}
