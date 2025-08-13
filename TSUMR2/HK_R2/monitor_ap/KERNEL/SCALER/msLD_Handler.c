#include "types.h"
#include "string.h"
#include "Ms_rwreg.h"
#include "ms_reg.h"
#include "msLD_Handler.h"
#include "Adjust.h"
#if 0//(CHIP_ID == CHIP_MT9701)
#include "HDR_GammaTable_Default.h"
#endif
#if ENABLE_LOCALDIMMING

#define LD_SPI_NEW		(1 && (CHIP_ID == CHIP_MT9701))
#define hw_SetVsync()           MEM_MSWRITE_BIT(_REG_GPIOL0_OUT, 1, BIT1)
#define hw_ClrVsync()           MEM_MSWRITE_BIT(_REG_GPIOL0_OUT, 0, BIT1)
#define Init_hwVsync_Pin()      MEM_MSWRITE_BIT(_REG_GPIOL0_OEZ, 0, BIT1)

#if (CHIP_ID == CHIP_MT9701)
#define ENABLE_ACTIVEDIMMING 0
xdata float g_fLowPassFilter = 1.0;
xdata BYTE g_LD_Mode = LD_LOCAL_MODE;
#else
#define ENABLE_ACTIVEDIMMING 1
xdata float g_fLowPassFilter = 1.0;//0.95;
xdata BYTE g_LD_Mode = LD_GLOBAL_DIMMING_MODE;//LD_GLOBAL_DIMMING_MODE_OFF;//LD_GLOBAL_DIMMING_MODE_OFF;////LD_GLOBAL_DIMMING_MODE;
#endif
#define ENABLE_PWMLUT 1

xdata BOOL _IsDebugLDMsg = FALSE;
xdata LDRegionReport g_RegionReport[LD_H_COUNT*LD_V_COUNT];
//xdata LDRegionReport g_PreRegionReport[LD_H_COUNT*LD_V_COUNT];
xdata float g_fRegionAlphaBlending = 0;//0.95;

MS_U32 tf_strength_dn =0xA0;// MHal_LD_GetSWTemporalFilterStrengthDn(); // 0 ~ 0xFF
MS_U32 tf_strength_up = 0xFC;//MHal_LD_GetSWTemporalFilterStrengthUp(); // 0 ~ 0xFF
BOOL bTemporalFilterEn = 1; 
xdata WORD u16MaxDuty = 0x009F;
xdata WORD u16MinDuty = 0x001F;
xdata BYTE u8Brightness = 0xFF;
xdata BYTE g_pre_PWM = 0, g_pre_gain =0, g_pre_alpha = 0, g_AlphaBlending = 0;
xdata MS_U16 n_leds = 5;
//xdata WORD SPIControlValue[LD_REGION_COUNT];
//---------------------------------Fine tune parameter------------------------------

code BYTE LeonaTMOCurve[256] = //170426, Finetune for HDR Gamma tbl 0.2
{
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 
    96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 
    208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 
};
code BYTE Tbl_Mapping_LED_To_DRAM[] =
{
    // 4, 5, 6, 7, 0, 1, 2, 3,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
    17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
};
#if (LD_DRIVE_TYPE == LD_DRIVE_O2)
xdata BYTE SPIOutputData[] =
{
0x40, 0x01,
     //dev 1
      //0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x89, 0x00, 0x9F, 0xA0, 0xA0, 0xFE, 0x00, 0x00, 0x0D, 0x00, 0x1A, 0x00, 0x27, 0x00, 0x35, 0x00, 0x42, 0x00, 0x4F, 0x00, 0x5C, 0x00,
      //0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      //0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, // addr
        0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F ,0xFF, 0x0F, 0xFF, 0x0F,
      //0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, // addr
        0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F ,0xFF, 0x0F,
};
#elif (LD_DRIVE_TYPE == LD_DRIVE_AMS)
xdata BYTE SPIOutputData[] =
{
0xBF, 0x20, 0x37,
     //dev 1
      //0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46,// addr
        0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F ,0xFF, 0x0F, 0xFF, 0x0F,
      //0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56,// addr
        0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0x55, 0x0A, 0x55, 0x05 ,0xAA, 0x0A,

     //dev 2
      //0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46,// addr
        0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F ,0xFF, 0x0F, 0xFF, 0x0F,
      //0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56,// addr
        0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xAA, 0x0A ,0xFF, 0x0F,

     0x00,
};
#elif (LD_DRIVE_TYPE == PWM_DRIVE)
xdata BYTE SPIOutputData[LD_H_COUNT*LD_V_COUNT];
#endif

void LD_PrintMsg( char *str)
{
    if (_IsDebugLDMsg == FALSE)
	return;
    printMsg(str);
    //printMsg("\n");
}
void msSetLDDebugMsg(BOOL IsEnDebugMsg)
{
    _IsDebugLDMsg = IsEnDebugMsg;
}

void LD_PrintData( char *str, WORD value )
{
    if (_IsDebugLDMsg == FALSE)
	return;
    printData(str, value);
    //printMsg("\n");
}

xdata BYTE g_PreSPIOutputData[sizeof(SPIOutputData)] = {0};
void msLDInit(void)
{
    msWrite2Byte(SC2E_06, (WORD)g_sPnlInfo.sPnlTiming.u16Width);
    msWriteBit(SC2E_03, TRUE, _BIT7);
    msLDSetRegionCount(LD_H_COUNT, LD_V_COUNT);

    //Init SPI control
    #if (LD_DRIVE_TYPE != PWM_DRIVE)
    #if 1
    //msWriteBit(REG_0400+0x0E*2, TRUE, _BIT0);                   //vsync on gpio51 , DISP_PWM[1]
    Init_hwVsync_Pin ();
    hw_ClrVsync();
    #else
    msWrite2ByteMask(REG_0400+0x0C*2, _BIT1|_BIT2, _BIT1|_BIT2);    //PWM1 on GPIO51 (vsync) & PWM2 on GPIO55 (CZ)
    #endif

    msWriteByteMask(REG_041B, _BIT4, _BIT4|_BIT5);            // 1: Full MSPI on GPIO_L10(MSPI_DI), GPIO_L11(MSPI_CK), GPIO56(MSPI_DO), GPIO_L0(MSPI_CZ)
    
    #if (LD_SPI_NEW == 0)
    //LD_DMA
    msWrite2Byte(REG_LDMA_CMD, 0x0000); //command to SPI

    msWrite2Byte(REG_LDMA_TRIG_DLY0, 0x100);
    msWrite2Byte(REG_LDMA_TRIG_DLY1, 0x200);
    msWrite2Byte(REG_LDMA_TRIG_DLY_CTRL, 0x0001);

    msWrite2ByteMask(REG_LDMA_CTRL, 0x0000, 0x83);
    #endif
    #endif

    //LD init
    msWriteByte(SC2E_A0, /*LD_H_COUNT-1*/ 0x3F); //15);
    msWriteByte(SC2E_A1, /*LD_V_COUNT-1*/ 0x3F); //5);
    msWrite2Byte(SC2E_A2, 0x0000);
    msWriteByte(SC2E_A4, /*LD_H_COUNT-1*/ 0x3F); //15);

    msWriteByte(SC2E_A8, 0x02); // disp_PWM2
    msWrite2Byte(SC2E_A6, /*LD_V_COUNT*/ 0x3F); //5);
    
    #if (LD_SPI_NEW == 1)
    msWrite2Byte(SC2E_B8, 0x0100);
    msWriteByteMask(SC2E_80,1, 1);
    msWrite2Byte(SC2E_88,0x00); 

	int legth = sizeof(SPIOutputData);
	msWrite2Byte(SC2E_88,legth-1);
    mStar_WaitForDataBlanking();
    #else
    msWriteByteMask(SC2E_B5, _BIT5, _BIT5 | _BIT6); //spi data inv, 0: [15:0] , 1: {[7:0], [15:8]} , 2: [0:15]
    #endif
	msWriteByte(SC2E_A5, 0x07); //bit0 dma_en, bit1 depack16b_mod, bit2 pwm mode


    #if (LD_DRIVE_TYPE != PWM_DRIVE)
    //PWM control
    //PWM2 -- CZ
    //msWrite2Byte(REG_103D00 + 0x08*2, 0x7000);
    //msWrite2Byte(REG_103D00 + 0x09*2, 0x0C00);
    //msWrite2Byte(REG_103D00 + 0x0A*2, 0x5408);
    //msWrite2Byte(REG_103D00 + 0x2C*2, 0x0C00);
    WORD wDuty = ((0.001 * LD_SPI_TRIGGER_DELAY)  * (g_sPnlInfo.sPnlTiming.u16VFreq/10) * 255 );
    msAPIPWMConfig(_DISP_PWM2_GP0_,(g_sPnlInfo.sPnlTiming.u16VFreq-0)/10,wDuty,(_PWM_DB_EN_ | _PWM_VSYNC_ALIGN_EN_ | _PWM_DB_VSYNC_MODE_ ),0,1);   // cs signal
    ForceDelay1ms(50);
    //PWM1 -- VSync
    //...
    wDuty = (0.0001 * (g_sPnlInfo.sPnlTiming.u16VFreq/10) * 255 );
    msAPIPWMConfig(_DISP_PWM1_GP0_, ((g_sPnlInfo.sPnlTiming.u16VFreq -0)/10 ),wDuty,(_PWM_DB_EN_ | _PWM_VSYNC_ALIGN_EN_ | _PWM_DB_VSYNC_MODE_ ),0,1);   // cs signal
    #endif
    //=========================

    #if ENABLE_ACTIVEDIMMING
    msWriteByteMask( SC4E_02, 1 ,1);//enable LD engine
    msWriteByteMask( SC4E_78, 0xF0,0xF0 );//Compensation blending alpha set to compensated pixel
    msWriteByteMask( SC4E_8C, 0x40,0x40 );//enable compensation
    #endif

    #if (CHIP_ID == CHIP_MT9701)
    MHal_LD_Set_SW_SpatialFilterStrength1(LD_SF_STRENGTH_1);
    MHal_LD_Set_SW_SpatialFilterStrength2(LD_SF_STRENGTH_2);
    MHal_LD_Set_SW_SpatialFilterStrength3(LD_SF_STRENGTH_3);
    MHal_LD_Set_SW_SpatialFilterStrength4(LD_SF_STRENGTH_4);
    MHal_LD_Set_SW_SpatialFilterStrength5(LD_SF_STRENGTH_5);
    #endif

    #if (LD_DRIVE_TYPE != PWM_DRIVE)
    #if (LD_SPI_NEW == 1)
    msWrite2Byte(0x1017A2,0x003F);
	msWrite2Byte(0x1017A0,0x0011);
	msWrite2Byte(0x1017A4,legth);
	#endif

	//MSPI
    msWrite2Byte(REG_MSPI_WBF_SIZE, 0x0002);
    msWrite2Byte(REG_MSPI_CTRL, 0x0203);
    msWrite2Byte(REG_MSPI_TIME_CTRL, 0x0001);
    msWrite2Byte(REG_MSPI_BTB_TIME, 0x003);

	#if (LD_SPI_NEW == 1)
	msWriteByte(0x103C20, 0x0F);
	#endif
    #endif
}

void msLDSetRegionCount(BYTE hCount, BYTE vCount)
{
    xdata WORD u16RegionCount = hCount*vCount - 1;
    xdata DWORD dwRatio_h_in = (((DWORD)hCount)<<20)/g_sPnlInfo.sPnlTiming.u16Width + 1;
    xdata DWORD dwRatio_v_in = (((DWORD)vCount)<<20)/g_sPnlInfo.sPnlTiming.u16Height + 1;

    msWrite4Byte(SC2E_16, dwRatio_h_in);
    msWrite4Byte(SC2E_1A, dwRatio_v_in);
    msWriteByte(SC2E_1E, (BYTE)u16RegionCount);
}

BOOL msLDCheckWriteOverRead(void)
{
    if (msReadByte(SC2E_09) & _BIT6)    // check write_over_read error
    {
        msWriteBit(SC2E_0C, TRUE, _BIT6);   // writeoverread_clr toggle
        msWriteBit(SC2E_0C, FALSE, _BIT6);

        if (msReadByte(SC2E_09) & _BIT6) // check write_over_read error
            return FALSE;
        }
    return TRUE;
}

#if 0
BOOL msLDSWGetHistogramReport(BYTE u8RegionCount, LDRegionReport *pRegionReport)
{
    xdata BYTE idx = 0;

    msWriteByteMask(SC2E_09, 0x01, 0x1F); //enable sw mode get histogram
    if (msLDCheckWriteOverRead() && (msReadByte(SC2E_09) & _BIT7))
    {
        //Do CPU read
        msWriteBit(SC2E_0C, TRUE, _BIT5);   // writedone_clr toggle
        msWriteBit(SC2E_0C, FALSE, _BIT5);

        for (idx=0; idx<u8RegionCount; idx++)
        {
            msWriteByte(SC2E_08, idx); //set sw mode index
            msWriteBit(SC2E_0C, TRUE, _BIT4); //trigger
            if (msReadByte(SC2E_15) & _BIT7)
            {
                pRegionReport->LDMean = msReadByte(SC2E_0E);
                pRegionReport->LDMax  = msReadByte(SC2E_10);
                //pRegionReport->LDMin  = msReadByte(SC2E_12);
                //pRegionReport->LDSP   = msReadByte(SC2E_14);

                //msWriteBit(SC2E_0D, TRUE, _BIT0);   // software clear sram data out valid after read data
                //msWriteBit(SC2E_0D, FALSE, _BIT0);
            }
            else
            {
                msWriteBit(SC2E_0D, TRUE, _BIT1);   // software clear read pointer
                msWriteBit(SC2E_0D, FALSE, _BIT1);
                msWriteBit(SC2E_0D, TRUE, _BIT0);   // software clear sram data out valid after read data
                msWriteBit(SC2E_0D, FALSE, _BIT0);
            }
            pRegionReport++;
        }
    }
    else
    {
        return FALSE;
    }
}
#endif
BOOL msLDGetHistogramReport(BYTE u8RegionCount, LDRegionReport *pRegionReport)
{
    xdata BYTE idx = 0;
    xdata BYTE tempRegvalue = 0;
    xdata BYTE tempTriggerRegvalue = 0;

    msWriteByteMask(SC2E_09, 0x00, 0x1F); //disable sw mode get histogram
    msWriteByte(SC2E_08, 0x00);

    if (msLDCheckWriteOverRead() && (msReadByte(SC2E_09) & _BIT7))
    {
        //Do CPU read
        msWriteBit(SC2E_0C, TRUE, _BIT5);   // writedone_clr toggle
        msWriteBit(SC2E_0C, FALSE, _BIT5);

        msWriteBit(SC2E_0D, TRUE, _BIT1);   //set read pointer to initial condition
        msWriteBit(SC2E_0D, FALSE, _BIT1);

        tempRegvalue = msReadByte(SC2E_0D);
        tempTriggerRegvalue = msReadByte(SC2E_0C);
        for (idx=0; idx<u8RegionCount; idx++)
        {
            msWriteByte(SC2E_0C, tempTriggerRegvalue|_BIT4);
            //msWriteBit(SC2E_0C, TRUE, _BIT4); //trigger
            if (msReadByte(SC2E_15) & _BIT7)
            {
                pRegionReport->LDMean = msReadByte(SC2E_0E);
                pRegionReport->LDMax  = msReadByte(SC2E_10);
                //pRegionReport->LDMin  = msReadByte(SC2E_12);
                //pRegionReport->LDSP   = msReadByte(SC2E_14);

                //msWriteByte(SC2E_0D, tempRegvalue|0x01);
                //msWriteByte(SC2E_0D, tempRegvalue&0xFE);
                //msWriteBit(SC2E_0D, TRUE, _BIT0);   // software clear sram data out valid after read data
                //msWriteBit(SC2E_0D, FALSE, _BIT0);
            }
            else
            {
                msWriteByte(SC2E_0D, tempRegvalue|0x03);
                msWriteByte(SC2E_0D, tempRegvalue&0xFC);
                //msWriteBit(SC2E_0D, TRUE, _BIT1);   // software clear read pointer
                //msWriteBit(SC2E_0D, FALSE, _BIT1);
                //msWriteBit(SC2E_0D, TRUE, _BIT0);   // software clear sram data out valid after read data
                //msWriteBit(SC2E_0D, FALSE, _BIT0);
            }
            pRegionReport++;
        }

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

#if LD_DRIVE_TYPE == LD_DRIVE_AMS
BYTE u8LD_InitData1[] =
{
    0xC0, 0x36, 0xCA, 0x00,

};

BYTE u8LD_InitData2[] =
{
    0x80, 0x13, 0x03,
    //0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15  <- register index
      0x97,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x80,0x00,0xFF,0xFF,0x00,0x00,0x04,0x42,0xFF,0x0F, //  value
    0x00,
};

BYTE u8LD_InitData3[] =
{
    0x80, 0x02, 0x01,
    //0x01,0x02,    <- register index
      0xFF, 0xFF,   //  value
    0x00,
};

BYTE u8LD_InitData4[] =
{
    0xC0, 0x36, 0xAC, 0x00,

};

#if LD_CUSTOM_DELAY_TIME_EN
xdata BYTE LED_Device_AS3820_DelayTime[]=
{
    0xBF, 0x20, 0x16,       //delay time...
    //dev1
  //0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25, // addr
    0x31,0x00,0x37,0x00,0x3D,0x00,0x43,0x00,0x49,0x00,0x4F,0x00,0x55,0x00,0x5B,0x00,
  //0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,0x31,0x32,0x33,0x34,0x35, // addr
    0x00,0x00,0x07,0x00,0x0D,0x00,0x13,0x00,0x19,0x00,0x1F,0x00,0x25,0x00,0x2B,0x00,

    //dev2
    0x2B,0x00,0x25,0x00,0x1F,0x00,0x19,0x00,0x13,0x00,0x0D,0x00,0x07,0x00,0x00,0x00,
    0x5B,0x00,0x55,0x00,0x4F,0x00,0x49,0x00,0x43,0x00,0x3D,0x00,0x37,0x00,0x31,0x00,

    0x00,
};
#endif

#elif LD_DRIVE_TYPE == LD_DRIVE_O2

#endif

xdata BOOL bInitDev = FALSE;
xdata BYTE u8InitIndex = 0;
xdata BYTE u8RepeatCnt = 10;
typedef enum {
    LD_DEVICE_UNLOCK,
    LD_DEVICE_SET_INIT,
#if LD_CUSTOM_DELAY_TIME_EN == 1
    LD_DEVICE_SET_DLY,
#endif
    LD_DEVICE_OUT_EN,
    LD_DEVICE_LOCK,
    LD_DEVICE_INIT_DONE
} LD_DEVICE_INIT_STEP;

void msMspiWrite( BYTE *out, WORD wLen)
{
    xdata WORD idx;

    //for Vsync Signal
    //msWriteBit(REG_0400+0x0E*2, TRUE, _BIT0);
#if (LD_SPI_NEW == 0)
    //LD_DMA
    msWrite2Byte(REG_LDMA_CMD, 0x0000); //command to SPI
    msWrite2Byte(REG_LDMA_CMD_LEN, 0x0000); // cmd length
    msWrite2Byte(REG_LDMA_TRIG_DLY0, 0x100);
    msWrite2Byte(REG_LDMA_TRIG_DLY1, 0x200);
    msWrite2Byte(REG_LDMA_TRIG_DLY_CTRL, 0x0001);
    msWrite2Byte(REG_LDMA_ML_NUM, (wLen-1)/2 + 1);
    msWrite2ByteMask(REG_LDMA_CTRL, 0x0000, 0x83);
#else
    msWrite2Byte(SC2E_88,wLen-1);	
    msWrite2Byte(0x1017A4,wLen);
#endif	
    /*
    sramIdx = msReadByte(SC2E_B0) & _BIT4;
    if (sramIdx == 0) //index = 0
        msWriteByteMask(SC2E_BF, _BIT4 | _BIT5, _BIT4 | _BIT5);
    else
        msWriteByteMask(SC2E_BF, _BIT4 , _BIT4 | _BIT5);
        */

    int modeLen = 32;
    #if (LD_SPI_NEW == 1)
    modeLen = 16;
    #endif
    for (idx = 0; idx < wLen; idx++)
    {
        #if (LD_SPI_NEW == 0)
            msWriteByte(SC2E_DF-(idx%32), out[idx]);
        #else
            msWriteByte(SC2E_DE-((idx*2)%32), out[idx]);
            //LD_PrintData("addr = 0x%x", SC2E_DE-((idx*2)%32));
        #endif
        
        if ( (idx > 0) && (((idx+1)%modeLen == 0) || (idx == wLen -1)))
        {
            msWriteByteMask(SC2E_BF, _BIT4 | _BIT5, _BIT4 | _BIT5);
            msWriteByte(SC2E_BE, idx/modeLen);
            //LD_PrintData("Index = 0x%x", idx/modeLen);
            msWriteBit(SC2E_BF, TRUE, _BIT2);
            while (msReadByte(SC2E_BF) & _BIT2) {}
            msWriteByteMask(SC2E_BF, _BIT4 , _BIT4 | _BIT5);
            msWriteBit(SC2E_BF, TRUE, _BIT2);
            while (msReadByte(SC2E_BF) & _BIT2) {}
        }
    }

    //msWrite2ByteMask(REG_LDMA_CTRL, 0x81, 0x83);
    //msWriteBit(REG_LDMA_RT_TRIG, TRUE, _BIT0);
}
xdata BYTE count = 0;
xdata BOOL bStart = FALSE;

void msLD_Isr(BOOL bVsync)
{
    if(LD_DRIVE_TYPE == PWM_DRIVE)
        return;

#if 0
    BYTE xdata ucBank;
    ucBank = MEM_MSREAD_BYTE( SC00_00 );
    if (bVsync)
    {
        MEM_MSWRITE_BYTE(SC00_00, 0x00);
        MEM_MSWRITE_BYTE ( SC00_CE, MEM_MSREAD_BYTE( SC00_CE ) & ( ~BIT3 ) ); //scRegs[0xCE] = scRegs[0xCE]&(~BIT3);
        MEM_MSWRITE_BYTE ( SC00_CC, MEM_MSREAD_BYTE( SC00_CC ) & ( ~BIT3 ) ); //scRegs[0xCC] = scRegs[0xCC]&(~BIT3);
        bStart = TRUE;
        count = 0;
        msWriteByte(REG_MSPI_CS, 0xFE);
        MEM_MSWRITE_BYTE(SC00_00, 0x00);
        MEM_MSWRITE_BYTE_MASK ( SC00_CE,  BIT3 ,BIT3); //scRegs[0xCE] = scRegs[0xCE]|BIT3;
        MEM_MSWRITE_BYTE ( SC00_00, ucBank );
    }

   if (bStart)
    {


        if (count == LD_SPI_TRANSFER_TIME)  //transfer end
        {
            bStart = FALSE;
            msWriteByte(REG_MSPI_CS, 0xFF);

        }
        count++;
    }
#else
    UNUSED(bVsync );
   if (bInitDev == FALSE) return;
    if ( ((count)%30) == 0)   //set vsync
    {
        hw_SetVsync();
    }

    if ( ((count)%30) == 1)   //clr vsync
    {
        hw_ClrVsync();
#if (LD_SPI_NEW == 0)		
        msWriteBit(REG_LDMA_RT_EN, TRUE, _BIT0);
#endif
    }

    if ( ((count)%30) == 2)   //CS = low
    {
#if (LD_SPI_NEW == 0)	    
        msWriteByte(REG_MSPI_CS, 0xFE);
        msWriteBit(REG_LDMA_RT_TRIG, TRUE, _BIT0);
#endif	
    }

    if ( ((count)%30) == 6)   //CS = high
    {
#if (LD_SPI_NEW == 0)	    
        msWriteByte(REG_MSPI_CS, 0xFF);
#endif
    }

    if ( count > 30)
        count = 0;

   count++;
  #endif
}

void msLDInitDevice(void)
{
    msWrite2ByteMask(REG_LDMA_CTRL, 0x0000, 0x83);
    xdata BYTE size = 0;
    xdata BYTE* pu8InitDataArray = 0;

    #if (LD_DRIVE_TYPE == LD_DRIVE_AMS)

    {
        if (u8InitIndex == LD_DEVICE_UNLOCK)
        {
            size = sizeof(u8LD_InitData1);
            pu8InitDataArray = u8LD_InitData1;
        }
        else if (u8InitIndex == LD_DEVICE_SET_INIT)
        {
            size = sizeof(u8LD_InitData2);
            pu8InitDataArray = u8LD_InitData2;
        }
#if LD_CUSTOM_DELAY_TIME_EN
        else if (u8InitIndex == LD_DEVICE_SET_DLY)
        {
            size = 3+ LD_DRIVE_NUM * 16 * 2 + (LD_DRIVE_NUM-1);//sizeof(LED_Device_AS3820_DelayTime);
            pu8InitDataArray = LED_Device_AS3820_DelayTime;
        }
#endif
        else if (u8InitIndex == LD_DEVICE_OUT_EN)
        {
            size = sizeof(u8LD_InitData3);
            pu8InitDataArray = u8LD_InitData3;
        }
        else if (u8InitIndex == LD_DEVICE_LOCK)
        {
            size = sizeof(u8LD_InitData4);
            pu8InitDataArray = u8LD_InitData4;
        }

        msMspiWrite(pu8InitDataArray, size);
        u8RepeatCnt--;
        if (u8RepeatCnt == 0)
        {
            u8InitIndex++;
            u8RepeatCnt = 10;
        }
        if (u8InitIndex == LD_DEVICE_INIT_DONE)
        {
            bInitDev = TRUE;
            u8InitIndex = 0;
        }
    }

    #elif (LD_DRIVE_TYPE == LD_DRIVE_O2)
    {
        size = sizeof(SPIOutputData);
        pu8InitDataArray = SPIOutputData;
        msMspiWrite(pu8InitDataArray, size);
        bInitDev = TRUE;
        /*
        msWrite2Byte(REG_LDMA_ML_NUM, 8);
        msWrite2Byte(REG_LDMA_CMD_LEN, 0xFE); // cmd length = 7, cmd0,1,2..,6 (bit 1, 2,3...,7)
        */
    }
    #elif (LD_DRIVE_TYPE == PWM_DRIVE)
    UNUSED(size);
    UNUSED(pu8InitDataArray);
    #endif

}
#define max(a, b) (((a) > (b)) ? (a) : (b))
void msLDSetSPIControlValue(void)
{
    xdata BYTE idx = 0;
    xdata BYTE alphaData[LD_REGION_COUNT] = {0x00};

    xdata WORD globalAverage = 0;
    //xdata WORD globalMin = 0xFFFF;
    //xdata WORD globalMax = 0x00;
    //xdata BOOL bIsHisChanged = FALSE;
    //xdata BOOL bIsSPIChanged = FALSE;
    xdata float fTMOValue = 0.0;
    xdata WORD val;

    static U16 SD_blocks[LD_REGION_COUNT]={0};
    static U16 TF_blocks[LD_REGION_COUNT]={0};

    //-------------Alpha blending---------------------------------------------------------------------------------
    for (idx =0; idx<LD_REGION_COUNT; idx++)
    {
        //printData("[msLDSetSPIControlValue] g_RegionReport[idx].LDMean = %d", g_RegionReport[idx].LDMean);
        alphaData[idx] = ((float)(g_RegionReport[idx].LDMean)*g_fRegionAlphaBlending)+((float)(g_RegionReport[idx].LDMax)*(1-g_fRegionAlphaBlending));
        globalAverage += alphaData[idx];
    }
    globalAverage /= LD_REGION_COUNT;
    //----------------------------------------------------------------------------------------------------------

    //-------------Calculate all region output PWM control--------------------------------------------------------------
    //The backlight control range is 0~7FF
    for (idx =0; idx<LD_REGION_COUNT; idx++)
    {
        //SPIControlValue[LDPanelLedOrder[idx]]= u16MinDuty +(((float)(alphaData[idx]*fAlpha + (1-fAlpha)*globalAverage))/255.0)*(u16MaxDuty-u16MinDuty);
        fTMOValue = (alphaData[idx]*g_fLowPassFilter + (1-g_fLowPassFilter)*globalAverage);
        fTMOValue = LeonaTMOCurve[(int)fTMOValue];
        //SPIControlValue[idx]= u16MinDuty +((fTMOValue)/255.0)*(u16MaxDuty-u16MinDuty);
        val = u16MinDuty +((fTMOValue)/255.0)*(u16MaxDuty-u16MinDuty);
        if (g_LD_Mode == LD_GLOBAL_MODE)
        {
            val = u16MinDuty + (u8Brightness  * (u16MaxDuty-u16MinDuty) ) / 0xFF;
        }
        SD_blocks[idx] = val;
    }
    
    MDrv_LDAlgo_SpatialFilter(SD_blocks, TF_blocks);
    //----------------------------------------------------------------------------------------------------------
    for (idx =0; idx<LD_REGION_COUNT; idx++)
    {
        #if (LD_DRIVE_TYPE == LD_DRIVE_O2)
        SPIOutputData[Tbl_Mapping_LED_To_DRAM[idx]*2 + 40] = TF_blocks[idx] & 0xFF;
        SPIOutputData[Tbl_Mapping_LED_To_DRAM[idx]*2+1 + 40] = (TF_blocks[idx]>>8) & 0xFF;
        #elif (LD_DRIVE_TYPE == LD_DRIVE_AMS)
        SPIOutputData[Tbl_Mapping_LED_To_DRAM[idx]*2 + 3] = TF_blocks[idx] & 0xFF;
        SPIOutputData[Tbl_Mapping_LED_To_DRAM[idx]*2+1 + 3] = (TF_blocks[idx]>>8) & 0xFF;
        #elif (LD_DRIVE_TYPE == PWM_DRIVE)
        SPIOutputData[idx] = TF_blocks[idx];
        #endif
    }
    //----------------------------------------------------------------------------------------------------------

    // This API should be implemented by different control panel.
    //msDrvTriggerLDSPIControlTimer();
#if 0
    if (bInitDev == FALSE)
    {
        msLDInitDevice();
    }
    else
    {
        msMspiWrite(&SPIOutputData, sizeof(SPIOutputData));
    }
#endif
    //----------------------------------------------------------------------------------------------------------
}

/*
BOOL IsHistogramChanged(LDRegionReport *pStructDest, LDRegionReport *pStructTarget)
{
    BYTE *pDest = (BYTE*)pStructDest;
    BYTE *pTarget = (BYTE*)pStructTarget;
    xdata int nSize = sizeof(LDRegionReport);
    xdata int idx = 0;
    for (idx = 0; idx<nSize; idx++)
    {
        if (*pDest != *pTarget)
        {
            return TRUE;
        }
        pTarget++;
        pDest++;
    }
    return FALSE;
}
*/

#if 1//ENABLE_ACTIVEDIMMING
static void MDrv_LD_AlgoCurrentTemporalFilter(const MS_U16 u16Mean,const MS_U16* IN_blocks, MS_U16* OUT_blocks)
{
    static MS_U16 luma_pre = 0;
    static MS_U16 PRE_blocks[1];
    MS_U16 block_idx = 0;
    //MS_U32 tf_strength_dn =0xFC;// MHal_LD_GetSWTemporalFilterStrengthDn(); // 0 ~ 0xFF
    //MS_U32 tf_strength_up = 0xFC;//MHal_LD_GetSWTemporalFilterStrengthUp(); // 0 ~ 0xFF
    MS_U32 luma_low = 10;// MHal_LD_GetSWTemporalFilterLowTh(); // 0 ~ 0xFF
    MS_U32 luma_high =16 + (IN_blocks[0]/4);//dynamic threshold, 16 ~ 32
    MS_U32 alpha = 0, alpha_up = 0, alpha_dn = 0; // 0 ~ 0xFF
    MS_U16 max_speed = 0;//MHal_LD_GetTempFilterMaxSpeed();
    MS_U16 block_cur = 0, block_pre = 0;

    if(0 == bTemporalFilterEn)
    {
        for (block_idx = 0; block_idx < n_leds; block_idx++)
        {
            OUT_blocks[block_idx] = IN_blocks[block_idx];
        }
        return;
    }

    // get frame average luma
    MS_U32 luma_cur = 0, luma_diff = 0;
    MS_U32 luma_sum = 0;

    //Dynamic y adaptive setting
    //MS_U16 dynamic_y_alpha;
    //static U16 q1pre = 0, q3pre =0;

    //CHECK_POINTER_V(IN_blocks);
    //CHECK_POINTER_V(OUT_blocks);

   for (block_idx = 0; block_idx < n_leds; block_idx++)
        luma_sum += IN_blocks[block_idx]<<8;
    luma_cur = u16Mean / n_leds; // 0 ~ 0xFF00

	
    luma_diff = (luma_cur > luma_pre) ? luma_cur - luma_pre : luma_pre - luma_cur;
    luma_pre = luma_cur; // 0 ~ 0xFF00

    // get blending alpha based on luma diff
    //uma_low <<= 8;
    //luma_high <<= 8;
    if (luma_diff >= luma_high)
    {
        alpha_up = 0;
        alpha_dn = 0;
	LD_PrintData(" get blending alpha 1 %d", NULL);	
    }
    else if (luma_diff <= luma_low)
    {
        alpha_up = tf_strength_up;
        alpha_dn = tf_strength_dn;
    }
    else
    {
        alpha_up = tf_strength_up * (luma_high - luma_diff) / (luma_high - luma_low);
        alpha_dn = tf_strength_dn * (luma_high - luma_diff) / (luma_high - luma_low);
    }

    max_speed <<= 8;
    for (block_idx = 0; block_idx < n_leds; block_idx++)
    {
        block_cur = IN_blocks[block_idx]<<8; // 0 ~ 0xFF00
        block_pre = PRE_blocks[block_idx]; // 0 ~ 0xFF00

        // 16-bit IIR
        alpha =  (block_cur > block_pre) ? alpha_up : alpha_dn;
        block_cur = (alpha*block_pre + (256-alpha)*block_cur + 0x80) >> 8;
   if (block_cur == block_pre)
        {
            if (block_cur < IN_blocks[block_idx]<<8)
                block_cur ++;
            else if (block_cur > IN_blocks[block_idx]<<8)
                block_cur --;
        }

        // limit the max speed
        if (max_speed > 0)
        {
            if (block_cur > block_pre && block_cur - block_pre > max_speed)
                block_cur = block_pre + max_speed;
            else if (block_cur < block_pre && block_pre - block_cur > max_speed)
                block_cur = block_pre - max_speed;
        }

        OUT_blocks[block_idx] = block_cur>>8;
        PRE_blocks[block_idx] = block_cur;
    }


}
#if (1) //(CHIP_ID == CHIP_MT9701)
// h41[7:0] reg_dummy7
void MHal_LD_Set_SW_SpatialFilterStrength1(U8 u8Str)  { msWriteByte(SC4E_82, u8Str); }
U8   MHal_LD_Get_SW_SpatialFilterStrength1(void)      { return msReadByte(SC4E_82); }
// h41[15:8] reg_dummy7
void MHal_LD_Set_SW_SpatialFilterStrength2(U8 u8Str)  { msWriteByte(SC4E_83, u8Str); }
U8   MHal_LD_Get_SW_SpatialFilterStrength2(void)      { return msReadByte(SC4E_83); }
// h42[7:0] reg_dummy8
void MHal_LD_Set_SW_SpatialFilterStrength3(U8 u8Str)  { msWriteByte(SC4E_84, u8Str); }
U8   MHal_LD_Get_SW_SpatialFilterStrength3(void)      { return msReadByte(SC4E_84); }
// h42[15:8] reg_dummy8
void MHal_LD_Set_SW_SpatialFilterStrength4(U8 u8Str)  { msWriteByte(SC4E_85, u8Str); }
U8   MHal_LD_Get_SW_SpatialFilterStrength4(void)      { return msReadByte(SC4E_85); }
// h43[7:0] reg_dummy9
void MHal_LD_Set_SW_SpatialFilterStrength5(U8 u8Str)  { msWriteByte(SC4E_86, u8Str); }
U8   MHal_LD_Get_SW_SpatialFilterStrength5(void)      { return msReadByte(SC4E_86); }
#endif
void MDrv_LDAlgo_SpatialFilter(const MS_U16* IN_blocks, MS_U16* OUT_blocks)
{
    U16 led_width = LD_H_COUNT;
    U16 led_height = LD_V_COUNT;
    int i = 0, j = 0, k = 0, p = 0, q = 0;
    U16 block_idx = 0, neighbor_idx = 0;
    for (block_idx = 0; block_idx < LD_REGION_COUNT; block_idx++)
        OUT_blocks[block_idx] = IN_blocks[block_idx];
    
    U16 sf_str[5];
    sf_str[0] = MHal_LD_Get_SW_SpatialFilterStrength1();
    sf_str[1] = MHal_LD_Get_SW_SpatialFilterStrength2();
    sf_str[2] = MHal_LD_Get_SW_SpatialFilterStrength3();
    sf_str[3] = MHal_LD_Get_SW_SpatialFilterStrength4();
    sf_str[4] = MHal_LD_Get_SW_SpatialFilterStrength5();

    // direct type
    const int N1_direct = 8; // dist = 1.0
    const int xoffset1_direct[8]  = { -1,  0,  1,  0, -1,  1,  1, -1 };
    const int yoffset1_direct[8]  = {  0, -1,  0,  1, -1, -1,  1,  1 };
    const int N2_direct = 16; // dist = 2.0
    const int xoffset2_direct[16] = { -2,  0,  2,  0, -2, -1,  1,  2,  2,  1, -1, -2, -2,  2,  2, -2 };
    const int yoffset2_direct[16] = {  0, -2,  0,  2, -1, -2, -2, -1,  1,  2,  2,  1, -2, -2,  2,  2 };
    const int N3_direct = 24; // dist = 3.0
    const int xoffset3_direct[24] = { -3,  0,  3,  0, -3, -1,  1,  3,  3,  1, -1, -3, -3, -2,  2,  3,  3,  2, -2, -3, -3,  3,  3, -3 };
    const int yoffset3_direct[24] = {  0, -3,  0,  3, -1, -3, -3, -1,  1,  3,  3,  1, -2, -3, -3, -2,  2,  3,  3,  2, -3, -3,  3,  3 };
    const int N4_direct = 32; // dist = 4.0
    const int xoffset4_direct[32]  = { -4,  0,  4,  0, -4, -1,  1,  4,  4,  1, -1, -4, -4, -2,  2,  4,  4,  2, -2, -4, -4, -3,  3,  4,  4,  3, -3, -4, -4,  4,  4, -4 };
    const int yoffset4_direct[32]  = {  0, -4,  0,  4, -1, -4, -4, -1,  1,  4,  4,  1, -2, -4, -4, -2,  2,  4,  4,  2, -3, -4, -4, -3,  3,  4,  4,  3, -4, -4,  4,  4 };
    const int N5_direct = 40; // dist = 5.0
    const int xoffset5_direct[40]  = { -5,  0,  5,  0, -5, -1,  1,  5,  5,  1, -1, -5, -5, -2,  2,  5,  5,  2, -2, -5, -5, -3,  3,  5,  5,  3, -3, -5, -5, -4,  4,  5,  5,  4, -4, -5, -5,  5,  5, -5 };
    const int yoffset5_direct[40]  = {  0, -5,  0,  5, -1, -5, -5, -1,  1,  5,  5,  1, -2, -5, -5, -2,  2,  5,  5,  2, -3, -5, -5, -3,  3,  5,  5,  3, -4, -5, -5, -4,  4,  5,  5,  4, -5, -5,  5,  5 };

    // left-right type
    const int N1_LR = 2; // dist = 1.0
    const int xoffset1_LR[2] = {  0,  0 };
    const int yoffset1_LR[2] = {  1, -1 };
    const int N2_LR = 2; // dist = 2.0
    const int xoffset2_LR[2] = {  0,  0 };
    const int yoffset2_LR[2] = {  2, -2 };
    const int N3_LR = 2; // dist = 3.0
    const int xoffset3_LR[2] = {  0,  0 };
    const int yoffset3_LR[2] = {  3, -3 };
    const int N4_LR = 2; // dist = 4.0
    const int xoffset4_LR[2] = {  0,  0 };
    const int yoffset4_LR[2] = {  4, -4 };
    const int N5_LR = 2; // dist = 5.0
    const int xoffset5_LR[2] = {  0,  0 };
    const int yoffset5_LR[2] = {  5, -5 };

    // top-bottom type
    const int N1_TB = 2; // dist = 1.0
    const int xoffset1_TB[2] = {  1, -1 };
    const int yoffset1_TB[2] = {  0,  0 };
    const int N2_TB = 2; // dist = 2.0
    const int xoffset2_TB[2] = {  2, -2 };
    const int yoffset2_TB[2] = {  0,  0 };
    const int N3_TB = 2; // dist = 3.0
    const int xoffset3_TB[2] = {  3, -3 };
    const int yoffset3_TB[2] = {  0,  0 };
    const int N4_TB = 2; // dist = 4.0
    const int xoffset4_TB[2] = {  4, -4 };
    const int yoffset4_TB[2] = {  0,  0 };
    const int N5_TB = 2; // dist = 5.0
    const int xoffset5_TB[2] = {  5, -5 };
    const int yoffset5_TB[2] = {  0,  0 };

    int N1 = 0, N2 = 0, N3 = 0, N4 = 0, N5 = 0;
    const int *xoffset1 = NULL, *xoffset2 = NULL, *xoffset3 = NULL, *xoffset4 = NULL, *xoffset5 = NULL;
    const int *yoffset1 = NULL, *yoffset2 = NULL, *yoffset3 = NULL, *yoffset4 = NULL, *yoffset5 = NULL;

    if (led_height <= 2) //(led_type == HAL_LD_EDGE_TB_TYPE)
    {
        N1 = N1_TB; N2 = N2_TB; N3 = N3_TB;
        xoffset1 = xoffset1_TB; xoffset2 = xoffset2_TB; xoffset3 = xoffset3_TB;
        yoffset1 = yoffset1_TB; yoffset2 = yoffset2_TB; yoffset3 = yoffset3_TB;
        N4 = N4_TB; N5 = N5_TB;
        xoffset4 = xoffset4_TB; xoffset5 = xoffset5_TB;
        yoffset4 = yoffset4_TB; yoffset5 = yoffset5_TB;
    }
    else if (led_width <= 2) //(led_type == HAL_LD_EDGE_LR_TYPE)
    {
        N1 = N1_LR; N2 = N2_LR; N3 = N3_LR;
        xoffset1 = xoffset1_LR; xoffset2 = xoffset2_LR; xoffset3 = xoffset3_LR;
        yoffset1 = yoffset1_LR; yoffset2 = yoffset2_LR; yoffset3 = yoffset3_LR;
        N4 = N4_LR; N5 = N5_LR;
        xoffset4 = xoffset4_LR; xoffset5 = xoffset5_LR;
        yoffset4 = yoffset4_LR; yoffset5 = yoffset5_LR;
    }
    else// if (led_type == HAL_LD_DIRECT_TYPE)
    {
        N1 = N1_direct; N2 = N2_direct; N3 = N3_direct;
        xoffset1 = xoffset1_direct; xoffset2 = xoffset2_direct; xoffset3 = xoffset3_direct;
        yoffset1 = yoffset1_direct; yoffset2 = yoffset2_direct; yoffset3 = yoffset3_direct;
        N4 = N4_direct; N5 = N5_direct;
        xoffset4 = xoffset4_direct; xoffset5 = xoffset5_direct;
        yoffset4 = yoffset4_direct; yoffset5 = yoffset5_direct;
    }

    for (k = 0; k < 5; k++)
        sf_str[k] <<= 8; // 0 ~ 0xFF00

    for (j = 0; j < led_height; j++)
    {
        for (i = 0; i < led_width; i++)
        {
            block_idx = i + led_width*j;

            // 3x3 non-linear filter
            if (sf_str[0] < 0xFF00)
            {
                for (k = 0; k < N1; k++)
                {
                    q = j + yoffset1[k];
                    if (q < 0 || q >= led_height) continue;
                    p = i + xoffset1[k];
                    if (p < 0 || p >= led_width) continue;

                    neighbor_idx = p + led_width*q;
                    if (sf_str[0] + OUT_blocks[block_idx] < IN_blocks[neighbor_idx])
                        OUT_blocks[block_idx] = IN_blocks[neighbor_idx] - sf_str[0];
                }
            }

            // 5x5 non-linear filter
            if (sf_str[1] < 0xFF00)
            {
                for (k = 0; k < N2; k++)
                {
                    q = j + yoffset2[k];
                    if (q < 0 || q >= led_height) continue;
                    p = i + xoffset2[k];
                    if (p < 0 || p >= led_width) continue;

                    neighbor_idx = p + led_width*q;
                    if (sf_str[1] + OUT_blocks[block_idx] < IN_blocks[neighbor_idx])
                        OUT_blocks[block_idx] = IN_blocks[neighbor_idx] - sf_str[1];
                }
            }

            // 7x7 non-linear filter
            if (sf_str[2] < 0xFF00)
            {
                for (k = 0; k < N3; k++)
                {
                    q = j + yoffset3[k];
                    if (q < 0 || q >= led_height) continue;
                    p = i + xoffset3[k];
                    if (p < 0 || p >= led_width) continue;

                    neighbor_idx = p + led_width*q;
                    if (sf_str[2] + OUT_blocks[block_idx] < IN_blocks[neighbor_idx])
                        OUT_blocks[block_idx] = IN_blocks[neighbor_idx] - sf_str[2];
                }
            }

            // 9x9 non-linear filter
            if (sf_str[3] < 0xFF00)
            {
                for (k = 0; k < N4; k++)
                {
                    q = j + yoffset4[k];
                    if (q < 0 || q >= led_height) continue;
                    p = i + xoffset4[k];
                    if (p < 0 || p >= led_width) continue;

                    neighbor_idx = p + led_width*q;
                    if (sf_str[3] + OUT_blocks[block_idx] < IN_blocks[neighbor_idx])
                        OUT_blocks[block_idx] = IN_blocks[neighbor_idx] - sf_str[3];
                }
            }

            // 11x11 non-linear filter
            if (sf_str[4] < 0xFF00)
            {
                for (k = 0; k < N5; k++)
                {
                    q = j + yoffset5[k];
                    if (q < 0 || q >= led_height) continue;
                    p = i + xoffset5[k];
                    if (p < 0 || p >= led_width) continue;

                    neighbor_idx = p + led_width*q;
                    if (sf_str[4] + OUT_blocks[block_idx] < IN_blocks[neighbor_idx])
                        OUT_blocks[block_idx] = IN_blocks[neighbor_idx] - sf_str[4];
                }
            }
       }
    }
}
#endif

#define REG_LD_SATEN                    SC73_A0
#define REG_LD_SATEN_MSK                (0x0080)
#define REG_LD_SATEN_SHIFT              (7)
#define REG_LD_SATA                     SC73_A2
#define REG_LD_SATA_MSK                 (0x00FF)
#define REG_LD_SATC                     SC73_AA
#define REG_LD_SATC_MSK                 (0x0FFF)
#define REG_LD_SATCSFT                  SC73_AC
#define REG_LD_SATCSFT_MSK              (0x000F)
void MHal_LD_SetSaturationEn(BOOL bSaturationEn)
{
//h50 [7]reg_sat_en
msWriteByteMask(REG_LD_SATEN, ((U16)bSaturationEn<<REG_LD_SATEN_SHIFT), REG_LD_SATEN_MSK);
}

void MHal_LD_SetSatA(U8 u8Reg_sat_a)
{
//h51 [7:0]reg_sat_a
msWriteByteMask(REG_LD_SATA, u8Reg_sat_a, REG_LD_SATA_MSK);
}
void MHal_LD_SetSatC(U16 U16Reg_sat_c)
{
//h55 [11:0]reg_sat_c
msWrite2ByteMask(REG_LD_SATC, U16Reg_sat_c, REG_LD_SATC_MSK);
}
void MHal_LD_SetSatCSft(U8 u8Reg_sat_c_sft)
{
//h56 [3:0]reg_sat_c_sft
msWriteByteMask(REG_LD_SATCSFT, u8Reg_sat_c_sft, REG_LD_SATCSFT_MSK);
}

void MHal_LD_Set_SatACUserGain(U8 u8SatACUserGain)
{
    msWriteByteMask(SC73_A2, u8SatACUserGain, 0x1F);
}
//U8 MHal_LD_Get_SatACUserGain(void)
//{
  //  return msWriteByteMask((LD3_BK | 0xA3))&0x1F;
//}
void MHal_LD_Set_SatUserGain(U8 u8SatUserGain)
{
    msWriteByteMask(SC73_A0, u8SatUserGain, 0x1F);
}
//U8 MHal_LD_Get_SatUserGain(void)
//{
//    return MHal_LD_ReadByte((LD3_BK | 0xA0))&0x1F;
//}
void MHal_LD_Set_SatGain(U8 u8SatGain)
{
    MHal_LD_Set_SatUserGain(u8SatGain); MHal_LD_Set_SatACUserGain(u8SatGain);
}
void MHal_LD_Set_SatInAlpha(U8 u8SatACInAlpha)
{
    msWriteByteMask(SC73_B3, u8SatACInAlpha, 0x1F);
}
void MDrv_LD_Set_SaturationProt(U8 u8SatTP, U8 u8SatGain)
{
    U16 u16SatC =0;
    U8 u8SatCSft = 0;

    U64 sat_b_inv = (0xFFF<<10) / (0xFFF - (u8SatTP << 4));
    while ((sat_b_inv>>10) >= 4)
    {
        sat_b_inv /= 2;
        u8SatCSft += 1;
    }
    u16SatC = (U16)((sat_b_inv * 1024) >> 10);
    if (u16SatC >= 0xFFF) //invalid value
        //LD_ERROR("ld sat parameter error %d\n", u16SatC);

    MHal_LD_Set_SatInAlpha(0x10);//Force blending to Max RGB
    MHal_LD_SetSatA(u8SatTP);
    MHal_LD_SetSatC(u16SatC);
    MHal_LD_SetSatCSft(u8SatCSft);
    MHal_LD_Set_SatGain(u8SatGain);

}

#if 1//ENABLE_ACTIVEDIMMING
void msGDMain(void)
{
    xdata BYTE idx = 0;
    xdata BYTE alphaData[LD_REGION_COUNT] = {0x00};

    xdata WORD globalAverage = 0;
    xdata WORD globalMax = 0x00;
    xdata WORD globalMaxTF = 0x00;
    xdata WORD PWM = 0x00;
    xdata WORD backLightVal,compensationVal;
    xdata WORD minPWM = 4;
    U8 u8SatTP = 160, u8SatGain = 16;
    double dbGain;
    double oriPWM,clampedPWM,gainMod;
    double IdealLum = 0, RealLum =0;
    xdata BYTE PWMLut_GAP= 32; 
    xdata BYTE PWM_MAX = 255;	 

    #if (CHIP_ID == CHIP_MT9701)
    #if 0
    double PWM_LUT[9];
    double PWM_Index[9];	
    memcpy(PWM_LUT, &tblPWMLUT, sizeof(tblPWMLUT));
    memcpy(PWM_Index, &tblPWMIndex, sizeof(tblPWMIndex));
    #else
    double tblPWMLUT[9] = {7.80, 66.65, 130.96, 195.13, 259.03, 322.81, 386.54, 449.61, 513.90, };
    double tblPWMIndex[9] = {0, 32, 64, 96, 128, 160, 192, 224, 255};
    #endif
    #else
    double tblPWMLUT[9] = {7, 71, 137, 202, 267, 332, 395, 458,515};
    double tblPWMIndex[9] ={4, 32, 64, 96, 128, 160, 192, 224, 255};	
    #endif
	
#if 0
   double PWM_LUT[33] = {7,19,38,55,72,88,104,121,137,153,170,185,202,218,234,250,266,282,298,313,330,345,361,377,392,408,424,440,455,471,487,505,511};
   double PWM_Index[33] ={4,8,16,24,32,40,48,56,64,72,80,88,96,104,112,120,128,136,144,152,160,168,176,184,192,200,208,216,224,232,240,248,255};

    double pwmMod[41] = {1.145452531295,1.145452531295,1.145452531295,1.145452531295,
	1.145452531295,0.97473604826546,0.894463667820069,0.876059065601549,0.840394188763588,0.833199033037873,0.830922532947605,0.829069174138057,0.830355350331259,0.834077934971457,0.837295390132454,0.840103997400065,0.846976910868786,0.848932676518883,0.854663176746108,0.857941394820735
	,0.86091336747013,0.865375418460067,0.869472155333868,0.873246676947933,0.8767355590885,0.878444965507867,0.88150042625746,0.885779554540263,0.889790398918188,0.889501942985969,0.893150211626501,0.895313110999385,0.898593232306773,0.900482938801362,0.90345128877239,0.905112044817927
	,0.906686152721958,0.908180221241039,0.909600203717851,0.91095147736514,0.914295819793532};
#endif
   
    msWrite2Byte( SC73_B8,0);// comp offset in
    msWrite2Byte( SC73_BA,0);// comp offset off

    msWriteByteMask( SC73_B0, 0x00,0x80);//turn off dynamic y
    MDrv_LD_Set_SaturationProt(u8SatTP, u8SatGain); 

    //-------------Alpha blending---------------------------------------------------------------------------------
    for (idx =0; idx<LD_REGION_COUNT; idx++)
    {
        alphaData[idx] = ((float)(g_RegionReport[idx].LDMean)*g_fRegionAlphaBlending)+((float)(g_RegionReport[idx].LDMax)*(1-g_fRegionAlphaBlending));

        if(globalMax < alphaData[idx])
        {
            globalMax = alphaData[idx];
        }
        globalAverage += alphaData[idx];
    }

     globalAverage /= LD_REGION_COUNT;

    if(g_pre_alpha != globalMax )
    {
        LD_PrintData(" globalAverage %d \n", globalAverage);
        LD_PrintData(" globalMax %d \n ", globalMax);
        g_pre_alpha = globalMax;
    }

    //msWrite2Byte( SC4E_AE, globalMax );
    MDrv_LD_AlgoCurrentTemporalFilter(globalAverage,&globalMax, &globalMaxTF);

    oriPWM =globalMaxTF/255.0;
    
    clampedPWM = max(minPWM / 255.0,oriPWM);
    backLightVal = max(globalMaxTF,1);//shift of 6 minimum
	
    #if ENABLE_PWMLUT
    PWM = (WORD)((clampedPWM*255)+0.5); //pwm is linear, so we compensate for 2.2 data, data data does not need compensation
	
    #if 0//ENABLE_DARKLEVEL_LUT
    if(PWM<40)
        gainMod = pow(((oriPWM / clampedPWM) * pwmMod[PWM]),1/2.2);
    else
        gainMod = pow((oriPWM / clampedPWM),1/2.2);
    #endif
    if(PWM < PWM_MAX)
    {
        IdealLum =  (PWM * tblPWMLUT[8] ) / PWM_MAX; 
        RealLum =  ((tblPWMLUT[PWM /PWMLut_GAP + 1] -tblPWMLUT[PWM /PWMLut_GAP] ) / PWMLut_GAP ) * (PWM -tblPWMIndex[PWM /PWMLut_GAP] ) + tblPWMLUT[PWM /PWMLut_GAP];
        gainMod = pow(((oriPWM / clampedPWM) * (IdealLum / RealLum)),1/2.2);	
    }
    else
        gainMod = pow((oriPWM / clampedPWM),1/2.2);
    #else
	gainMod = pow(oriPWM / clampedPWM,1/2.2); //gainMod = pow(oriPWM / clampedPWM,1); //
    #endif
    //calculate gain resolution start
    int gain = pow(255.0/max(globalMaxTF,1), 1/2.2) * gainMod;
    int gainBits = 0;
    while ((gain>>gainBits) != 1 && (gain != 0))
        gainBits++;
    gainBits++;
    int gainShift = 6-gainBits;
    //compensationVal = (0xFF<<(6+gainShift)) / backLightVal;
    double shiftValue = (double)(1<<(6+gainShift));
    //calculate gain resolution fin
    dbGain = pow (255.0/max(globalMaxTF,1), 1/2.2) * gainMod;
	 
    compensationVal = (WORD)((dbGain * shiftValue) + 0.5);
    //gainShift =0;

    #if !ENABLE_PWMLUT
    PWM = (WORD)((clampedPWM*255)+0.5); //pwm is linear, so we compensate for 2.2 data, data data does not need compensation
    #endif

    if(g_pre_PWM != backLightVal )
    {
        LD_PrintData(" PWM %x", PWM);
        LD_PrintData(" compensationVal %x", compensationVal);
        g_pre_PWM = backLightVal;
    }
	
    if(0 == msRead2Byte( SC4E_B0 ))//enable, use dummy for now
    {
        //msWriteByteMask( SC4E_02, 1 ,1);//enable LD engine
        //msWriteByteMask( SC4E_78, 0xF0,0xF0 );//Compensation blending alpha set to compensated pixel
        //msWriteByteMask( SC4E_8C, 0x40,0x40 );//enable compensation
        #if ENABLE_ACTIVEDIMMING
        #if BRIGHTNESS_INVERSE  //+Duty power
        drvGPIO_SetBacklightDuty(BrightnessPWM, 0xFF-PWM);
        #else                           //-Duty power
        drvGPIO_SetBacklightDuty(BrightnessPWM, PWM);
        #endif
        #endif

        mStar_WaitForDataBlanking();
        msWrite2Byte( SC73_9E, (gainShift<<12)+compensationVal);//set compensation gain resolution
    }
    else//disable
    {
        #if ENABLE_ACTIVEDIMMING
        #if BRIGHTNESS_INVERSE  //+Duty power
        drvGPIO_SetBacklightDuty(BrightnessPWM, 0);
        #else                           //-Duty power
        drvGPIO_SetBacklightDuty(BrightnessPWM, 0xFF);
        #endif
        msWriteByteMask( SC4E_02, 0 ,1);//enable LD engine
        #endif
    }
    //----------------------------------------------------------------------------------------------------------
}
#endif


void msLD_SPIHandler(void)
{
    xdata int idx = 0;
    int legth = sizeof(SPIOutputData);
    for (idx = 0; idx < legth; idx++)
    {
        if (SPIOutputData[idx] != g_PreSPIOutputData[idx])
        {
            msMspiWrite(&SPIOutputData[0], sizeof(SPIOutputData));
            memcpy(g_PreSPIOutputData, SPIOutputData, sizeof(SPIOutputData));
            break;
        }
    }
}

void msLD_PWMHandler(void)
{
    xdata int idx = 0;
    // PWM API & PWM Mapping (16 to 8) depends on the project behavior
    // The below is an example
    for (idx = 0; idx < LD_REGION_COUNT; idx++)
    {
        #if BRIGHTNESS_INVERSE  //+Duty power
        drvGPIO_SetBacklightDuty(BrightnessPWM, 0xFF-SPIOutputData[idx]);
        #else                           //-Duty power
        drvGPIO_SetBacklightDuty(BrightnessPWM, SPIOutputData[idx]);
        #endif
    }
}

void msLDHandler(void)
{
    if (bInitDev == FALSE)
    {
        msLDInitDevice();
        return;
    }
    if (g_LD_Mode == LD_LOCAL_MODE || g_LD_Mode == LD_GLOBAL_DIMMING_MODE)
    {
        msLDGetHistogramReport(LD_REGION_COUNT, g_RegionReport);

    }
#if ENABLE_ACTIVEDIMMING    	
    if(g_LD_Mode == LD_GLOBAL_DIMMING_MODE)
    {
    	if(!DisplayLogoFlag)
            msGDMain();
    }
    else if(g_LD_Mode == LD_GLOBAL_DIMMING_MODE_OFF) // Standard Global Dimming
    {
	mStar_AdjustBrightness( UserPrefBrightness ); //drvGPIO_SetBacklightDuty(BrightnessPWM, 0xFF);
	msWriteByteMask( SC4E_8C, 0x00,0x40 ); //disable compensation
    }
    else
#endif		
    {
        #if (CHIP_ID == CHIP_MT9701)
        msGDMain(); // Calculate Compensate Value
        #endif
        msLDSetSPIControlValue();
        
        #if (LD_DRIVE_TYPE == PWM_DRIVE)
        msLD_PWMHandler();
        #else
        msLD_SPIHandler();
        #endif
    }
} 

void msLD_SetMode(LD_TYPE type)
{
    g_LD_Mode = type;

}

void msSetLDMinMax(WORD u16Min, WORD u16Max)
{
    u16MaxDuty = u16Max;
    u16MinDuty = u16Min;
}

void msLD_SetGlobalDimming(BYTE u8Bri)
{
    u8Brightness = u8Bri;
}



#endif
