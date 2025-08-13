
#include "Global.h"
#include "mapi_DPRx.h"
#if ENABLE_SUPER_RESOLUTION
#include "menufunc.h"
#endif
#include "UserPref.h"
#include "CustomCbAPI.h"
#include "MsCommon.h"

BYTE code appmSstarNullData[] = {0};
void msapmStarDummy(void)
{
    BYTE xdata i = appmSstarNullData[0];
    i=i;
}

WORD mSTar_GetInputHTotal(void)
{
    WORD u16HTotal;

    msWriteByteMask(SC00_ED,BIT6,BIT6);
    u16HTotal=(msRead2Byte(SC00_E4)&0x1FFF)+1; //get input HTotal
    msWriteByteMask(SC00_ED,0,BIT6);

    return u16HTotal*(Input420Flag+1);
}

WORD mStar_DrvOutputHttAlign(WORD u16Htt)
{
    WORD u16HttAlign;

    if(g_sPnlInfo.ePnlTypeLvds == EN_PNL_LVDS_CH_2)
        u16HttAlign = ALIGN_2(u16Htt);
    else if(g_sPnlInfo.ePnlTypeLvds == EN_PNL_LVDS_CH_4)
        u16HttAlign = ALIGN_4(u16Htt);
    else if ((g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_2_10BIT) || (g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_2_8BIT))
        u16HttAlign = ALIGN_2(u16Htt);
    else if ((g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_4_10BIT) || (g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_4_8BIT))
        u16HttAlign = ALIGN_4(u16Htt);
    else if ((g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_8_10BIT) || (g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_CH_8_8BIT))
        u16HttAlign = ALIGN_8(u16Htt);
    else if (g_sPnlInfo.ePnlTypeEdp == EN_PNL_EDP_ENABLE)
        u16HttAlign = ALIGN_4(u16Htt);
    else
        u16HttAlign = u16Htt;
    
    return u16HttAlign; // aligned output Htt of scaler
}

WORD mStar_GetOutputHTotalTolFBL(void)
{
#if (DISABLE_SHORT_FRAME && !DISABLE_SHORT_LINE_TUNE)
    return 10; // keep original 10 for short line tune case
#else
    BOOL bIs16Line;
    WORD u16HPeriodIn, u16HeightIn, u16DevIn;
    DWORD u32HPeriodUnit;
    WORD u16HttTol;

    if(IS_DP_FREESYNC())
    {
        u16HttTol = 10; // keep original 10 for DP freesync
    }
    else
    {   
        bIs16Line = SC0_HPERIOD_16LINE_MODE();
        u16HPeriodIn = SC0_READ_HPEROID();
        u16HeightIn = SC0_READ_IMAGE_HEIGHT();
        if((u16HPeriodIn == 0) || (u16HeightIn <= 1)) // protect for /0
        {
            u16HttTol = 10; // keep original 10
#if DEBUG_PRINT_ENABLE
            printMsg( "[Warning] incorrect input Hperiod or image height \n");
#endif
        }
        else
        {                
            // in 16 line and 8 line mode,  deviation of each input H line are 0.125 XTAL and 0.25 XTAL, respectively
            // thus, total input pixel deviation is Hfreq * (0.125 or 0.25) / XTAL
            if(bIs16Line)
            {
                u32HPeriodUnit = MST_HPeriod_UINT_16; // KHz
                u16DevIn = 125;
            }
            else
            {
                u32HPeriodUnit = MST_HPeriod_UINT;
                u16DevIn = 250;
            }

            u16HttTol = (float)g_sPnlInfo.sPnlTiming.u16HttMin * u32HPeriodUnit / u16HPeriodIn * u16DevIn * (g_sPnlInfo.sPnlTiming.u16Height-1) / ((U64)CRYSTAL_CLOCK * (u16HeightIn-1));
            u16HttTol += 1; // add 1 for rounding up
        }
    }
    
    u16HttTol = mStar_DrvOutputHttAlign(u16HttTol); 
#if DEBUG_PRINT_ENABLE
    printData( "  u16HttTol=%d", u16HttTol);
#endif
    
    return u16HttTol;
#endif
}


Bool appmStar_SetPanelTiming( void )
{
    float factor;
    DWORD xdata dclk;
    WORD xdata sclk;
    WORD xdata height;
    WORD xdata dstHTotal;
    WORD xdata hPeriod;
    WORD xdata hTotal;
    DWORD xdata oVtotal; //Estimated output vtotal in frame lock mode.
    float wDclkMax, wDclkMin;
    BYTE u8SSCPercentage = 0;
    BOOL bIs16Line = SC0_HPERIOD_16LINE_MODE();
    WORD u16HttOutTol;
    
    if( g_sPnlInfo.ePnlTypeEdp != EN_PNL_EDP_ENABLE )
    {
#if ENABLE_FACTORY_SSCADJ
        u8SSCPercentage = FactorySetting.SSCPercentage;
#else
        if( g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_NONE )
            u8SSCPercentage = PANEL_SSC_PERCENTAGE_DEF;
        else
            u8SSCPercentage = PANEL_SSC_PERCENTAGE_DEF_VX1;
#endif
    }

    if( u8SSCPercentage )
    {
        wDclkMax = (float)g_sPnlInfo.sPnlTiming.u16DClkMax*1000/(1000+u8SSCPercentage);
        wDclkMin = (float)g_sPnlInfo.sPnlTiming.u16DClkMin*1000/(1000-u8SSCPercentage);
    }
    else
    {
        wDclkMax = g_sPnlInfo.sPnlTiming.u16DClkMax;
        wDclkMin = g_sPnlInfo.sPnlTiming.u16DClkMin;
    }

    hPeriod = SC0_READ_HPEROID();
    if(bIs16Line)
    {        
        if( abs( ((hPeriod + 8) / 16) - SrcHPeriod ) > HPeriod_Torlance )// prevent from getting the wrong hperiod to generate unknown panel clock
            hPeriod = SrcHPeriod * 16;
    }
    else
    {
        if( abs( ((hPeriod + 4) / 8) - SrcHPeriod ) > HPeriod_Torlance ) // prevent from getting the wrong hperiod to generate unknown panel clock
            hPeriod = SrcHPeriod * 8;
    }

    hPeriod = (hPeriod?hPeriod:1);
    // calculate input timing dot clock for comparing with output dot clock
    if(CURRENT_INPUT_IS_VGA())
        sclk = (( DWORD )UserPrefHTotal * (bIs16Line ? MST_HPeriod_UINT_16 : MST_HPeriod_UINT) + (( DWORD )hPeriod * 1000 / 2)) / (( DWORD )hPeriod * 1000);
    else
    {
        dclk = mSTar_GetInputHTotal();
        sclk = (( DWORD )dclk * (bIs16Line ? MST_HPeriod_UINT_16 : MST_HPeriod_UINT) + (( DWORD )hPeriod * 1000 / 2)) / (( DWORD )hPeriod * 1000 );
    }

        #if 0//DEBUG_PRINT_ENABLE
        printData( "  sclk=%d", sclk );
        #endif

    height = SC0_READ_IMAGE_HEIGHT();//old_msRead2Byte(SC0_09)&SC_MASK_V;
    height = (height?height:4);
    oVtotal = ((DWORD)SrcVTotal * g_sPnlInfo.sPnlTiming.u16Height + (height / 2)) / height;

    if( CURRENT_SOURCE_IS_INTERLACE_MODE() )
    {
        height /= 2;
    }

    u16HttOutTol = mStar_GetOutputHTotalTolFBL();
    dstHTotal = g_sPnlInfo.sPnlTiming.u16HttMin + u16HttOutTol;

    //***************************************************************
    if( sclk )
    {
        // in expansion mode, chip will assume output clock is faster than input clock,
        // if input clock is faster than output clock, must make output clock faster than input clock
        if(IS_DP_FREESYNC())
        {
            hTotal = msAPI_combo_IPGetGetHTotal();
            hTotal = (hTotal?hTotal:1);
            factor = (float)mapi_DPRx_GetPixelClock10K(SrcInputType) *10 * dstHTotal / hTotal * (g_sPnlInfo.sPnlTiming.u16Height-1)  / ((height-1)); // output dot clock in KHz
        }
        else
        {
            factor = (float)(bIs16Line ? MST_HPeriod_UINT_16 : MST_HPeriod_UINT) / hPeriod * dstHTotal * (g_sPnlInfo.sPnlTiming.u16Height-1) / (height-1); // output dot clock in KHz
        }
        dclk = ( factor + 500 ) / 1000;
        #if DEBUG_PRINT_ENABLE
        printData( "  dclk=%d", dclk );
        printData( "  PanelMaxDCLK=%d", g_sPnlInfo.sPnlTiming.u16DClkMax );
        printData( "  wDclkMax=%d", wDclkMax );
        printData( "  oVotal=%d", oVtotal );
        printData( "  PanelMaxVTotal=%d", g_sPnlInfo.sPnlTiming.u16VttMax );
        printData( "  hPeriod=%d", hPeriod );
        printData( "  IS_HDMI_FREESYNC()=%d", IS_HDMI_FREESYNC() );
        #endif

#if DECREASE_V_SCALING
        if(((dclk > wDclkMax) || ((!IS_HDMI_FREESYNC()) && (oVtotal > g_sPnlInfo.sPnlTiming.u16VttMax))) && (!g_SetupPathInfo.bFBMode) )
        {
            WORD wHeight;
            //WORD wTmpDecVScaleValue;

            BYTE xdata VBlanking = (SC_LOCK_LINE+3);

            if(CURRENT_SOURCE_IS_INTERLACE_MODE())
            {
                height *= 2;
                VBlanking *= 2;
            }

            if((!IS_HDMI_FREESYNC()) && (SrcVTotal-height > VBlanking))
            {
#if 1
                wHeight = ((DWORD)dstHTotal*g_sPnlInfo.sPnlTiming.u16Height*((bIs16Line ? MST_HPeriod_UINT_16 : MST_HPeriod_UINT)/1000))/((float)hPeriod*wDclkMax) + 1;
                DecVScaleValue = wHeight-height;
                if(DecVScaleValue%2)
                    DecVScaleValue +=1;
                            
                if(((DecVScaleValue/2) > SC0_READ_AUTO_START_V()) || ((DecVScaleValue/2) > ((SrcVTotal - (SC0_READ_AUTO_START_V() + height)))))
                {
                    #if DEBUG_PRINT_ENABLE
                    printMsg("bUnsupportMode 1\n");
                    #endif
                    SrcFlags|=bUnsupportMode;
                    return FALSE;
                }
#else
                DecVScaleValue = SrcVTotal-height-VBlanking;
                if(DecVScaleValue%2)
                    DecVScaleValue +=1;
                //modified some timing display unsymmetrically
                DecVScaleValue = ((VSTART_OFFSET>=SC0_READ_AUTO_START_V())?(SC0_READ_AUTO_START_V()*2):(DecVScaleValue));

                //limit DecVScaleValue to avoid over Vtotal
                wTmpDecVScaleValue = SrcVTotal - (SC0_READ_AUTO_START_V() + height);

                if(wTmpDecVScaleValue%2)
                    wTmpDecVScaleValue -= 1;

                DecVScaleValue = (wTmpDecVScaleValue < (DecVScaleValue/2))?(wTmpDecVScaleValue*2):DecVScaleValue; 
#endif
            }
            else
            {
                #if DEBUG_PRINT_ENABLE
                printMsg("bUnsupportMode 2\n");
                #endif
                SrcFlags|=bUnsupportMode;
                return FALSE;
            }

            wHeight = height+DecVScaleValue;
            oVtotal = ((DWORD)SrcVTotal * g_sPnlInfo.sPnlTiming.u16Height + (wHeight / 2)) / wHeight;

            if(oVtotal < g_sPnlInfo.sPnlTiming.u16VttMin)
            {
                wHeight = ((DWORD)SrcVTotal * g_sPnlInfo.sPnlTiming.u16Height + (g_sPnlInfo.sPnlTiming.u16VttMin / 2)) / g_sPnlInfo.sPnlTiming.u16VttMin;
                DecVScaleValue = wHeight - height;
                oVtotal = g_sPnlInfo.sPnlTiming.u16VttMin;
            }

            if(CURRENT_SOURCE_IS_INTERLACE_MODE())
                wHeight /= 2;
            factor=((float)dstHTotal*(g_sPnlInfo.sPnlTiming.u16Height-1)*(bIs16Line ? MST_HPeriod_UINT_16 : MST_HPeriod_UINT))/((DWORD)hPeriod*(wHeight-1)); // output dclk
            dclk=(factor+500)/1000;

            if((dclk > wDclkMax) || (oVtotal > g_sPnlInfo.sPnlTiming.u16VttMax))
            {
                #if DEBUG_PRINT_ENABLE
                printMsg("bUnsupportMode 3\n");
                #endif
                SrcFlags|= bUnsupportMode;
                return FALSE;
            }

            if(dclk < g_sPnlInfo.sPnlTiming.u16DClkMin)
            {
                dclk = (float)g_sPnlInfo.sPnlTiming.u16DClkMin * 1000;
                dstHTotal = (((DWORD)dclk*hPeriod*(wHeight-1)) + ((bIs16Line ? MST_HPeriod_UINT_16 : MST_HPeriod_UINT)*(g_sPnlInfo.sPnlTiming.u16Height-1)/2)) / ((bIs16Line ? MST_HPeriod_UINT_16 : MST_HPeriod_UINT)*(g_sPnlInfo.sPnlTiming.u16Height-1));
                if((dstHTotal < (g_sPnlInfo.sPnlTiming.u16HttMin+u16HttOutTol)) || (dstHTotal > (g_sPnlInfo.sPnlTiming.u16HttMax-u16HttOutTol)))
                {
                    #if DEBUG_PRINT_ENABLE
                    printMsg("bUnsupportMode 4\n");
                    #endif
                    SrcFlags|= bUnsupportMode;
                    return FALSE;
                }
            }

            mStar_SetCaptureWindow();
            mStar_SetupPath();

            height += DecVScaleValue;
            if(CURRENT_SOURCE_IS_INTERLACE_MODE())
                height /= 2;
        }
        else
        {
            DecVScaleValue = 0;
        }
#endif

        if(dclk < wDclkMin) 
        {
            WORD wHeight;
            
            wHeight = ((DWORD)dstHTotal*g_sPnlInfo.sPnlTiming.u16Height*((bIs16Line ? MST_HPeriod_UINT_16 : MST_HPeriod_UINT)/1000))/((float)hPeriod*wDclkMax) + 1;

            dclk = (float)wDclkMin * 1000;
            dstHTotal = (((DWORD)dclk*hPeriod*(wHeight-1)) + ((bIs16Line ? MST_HPeriod_UINT_16 : MST_HPeriod_UINT)*(g_sPnlInfo.sPnlTiming.u16Height-1)/2)) / ((bIs16Line ? MST_HPeriod_UINT_16 : MST_HPeriod_UINT)*(g_sPnlInfo.sPnlTiming.u16Height-1));
            if((dstHTotal < (g_sPnlInfo.sPnlTiming.u16HttMin+u16HttOutTol)) || (dstHTotal > (g_sPnlInfo.sPnlTiming.u16HttMax-u16HttOutTol)))
            {
                SrcFlags|= bUnsupportMode;
                return FALSE;
            }
        }
    }

    SC0_SAMPLE_CLOCK_INVERT(sclk);

    //SC0_SET_OUTPUT_VTOTAL(0xFFF);//set in the following msSetFrameSyncMode //old_msWrite2ByteMask(SC0_1E, 0xFFF, SC_MASK_V); // output vtotal //Jison 110527 lost to add.
    if(IS_DP_FREESYNC())
    {
        hTotal = msAPI_combo_IPGetGetHTotal();
        hTotal = (hTotal?hTotal:1);
        factor = (float)mapi_DPRx_GetPixelClock10K(SrcInputType) *10 * dstHTotal / hTotal * (g_sPnlInfo.sPnlTiming.u16Height-1)  / ((height-1)); // output dot clock in KHz
    }
    else
    {
        factor = (float)(bIs16Line ? MST_HPeriod_UINT_16 : MST_HPeriod_UINT) / hPeriod * dstHTotal * (g_sPnlInfo.sPnlTiming.u16Height-1) / (height-1); // output dot clock in KHz
    }
    //mStar_ResetDClkPLL(); // reset output PLL to prevent PLL works abnormally

//****************Setup line buffer clock*************************************
  #if 0//DEBUG_EN
    PRINT_DATA(" current idclk=%d",(((DWORD)USER_PREF_H_TOTAL*MST_HPeriod_UINT)/((DWORD)u16HPeriod*1000)));
  #endif
    if(g_SetupPathInfo.bFBMode)
    {
        g_SetupPathInfo.ucFrameLockMode = FLM_FB_FPLL_FLOCK_RESET_AUTO_TGEN;
        msDrvSetFrameDivider(V_FREQ_IN);
        msDrvResetTGen();
        factor = msDrvGetDClkPLLFromIVS();
        if((factor < (DWORD)g_sPnlInfo.sPnlTiming.u16DClkMin * 1000UL)
            || (factor > (DWORD)g_sPnlInfo.sPnlTiming.u16DClkMax * 1000UL)// frame pll unlock
#if ENABLE_DOLBY_HDR
#ifndef DOLBY_IDK_VERIFICATION
#define DOLBY_IDK_VERIFICATION 0
#endif
            || DOLBY_IDK_VERIFICATION
#endif
            ) 

        {
            #if !ENABLE_DOLBY_HDR
            #if DEBUG_PRINT_ENABLE
            printMsg( "Clock out of panel spec!! Change to no lock!!\r\n");
            #endif
            #else
            #if !DOLBY_IDK_VERIFICATION
            #if DEBUG_PRINT_ENABLE
            printMsg( "Clock out of panel spec!! Change to no lock!!\r\n");
            #endif
            #endif
            #endif

            g_SetupPathInfo.ucFrameLockMode = FLM_FB_FPLL_NOLOCK;
            msDrvResetTGen();
            factor = (DWORD)g_sPnlInfo.sPnlTiming.u16DClk * 1000UL;
        }
#if ENABLE_DOLBY_HDR
#undef DOLBY_IDK_VERIFICATION
#endif
    }
    else
    {
        msDrvSetFrameDivider(V_FREQ_IN);
        if(ENABLE_FBL_FRAMEPLL)
        {
            factor = (float)(bIs16Line ? MST_HPeriod_UINT_16 : MST_HPeriod_UINT) / hPeriod * dstHTotal * g_sPnlInfo.sPnlTiming.u16Height / height; // output dot clock in KHz
            g_SetupPathInfo.ucFrameLockMode = FLM_FBL_FRAMEPLL;
        }
        else
        {
            g_SetupPathInfo.ucFrameLockMode = FLM_FBL;
        }
    }

    msDrvSetDClkPLL(factor);

    msSetFrameSyncMode(g_SetupPathInfo.ucFrameLockMode, height, dstHTotal, factor);

    mStar_SetTimingGen(g_sPnlInfo.sPnlTiming.u16Width, g_sPnlInfo.sPnlTiming.u16Height, FALSE);

    mStar_CheckFastClock(hPeriod, hPeriod);

    mStar_SetPanelSSC( 0, 0 );

    Delay1ms(2);
    SC0_OUTPUT_LOCK_MODE(TRUE);//old_msWriteByteMask(SC0_02, 0 , NIS_B);// enable lock input mode
    Delay1ms(100);

#if ENABLE_FACTORY_SSCADJ // 2011-09-16 19:46 CC
    mStar_SetPanelSSC(FactorySetting.SSCModulation, FactorySetting.SSCPercentage);
#else
    if( g_sPnlInfo.ePnlTypeVby1 == EN_PNL_VBY1_NONE )
        mStar_SetPanelSSC(PANEL_SSC_MODULATION_DEF, PANEL_SSC_PERCENTAGE_DEF);
    else
        mStar_SetPanelSSC(PANEL_SSC_MODULATION_DEF_VX1, PANEL_SSC_PERCENTAGE_DEF_VX1);
#endif

    if(g_SetupPathInfo.bFBMode)
    {
        if(g_SetupPathInfo.ucFrameLockMode == FLM_FB_FAST_LOCK
            || g_SetupPathInfo.ucFrameLockMode == FLM_FB_FAST_LOCK_AUTO_TGEN )
        {
            //WaitOutputFastLockStable();
        }
        else
        {
            SetTimOutConter(FRAMEPLL_TIMEOUT);
            if(g_SetupPathInfo.ucFrameLockMode == FLM_FB_FPLL_FLOCK_RESET
                || g_SetupPathInfo.ucFrameLockMode == FLM_FB_FPLL_FLOCK_RESET_AUTO_TGEN )
            {
                g_bEnableIsrDisableFlock = 1;
                msWriteByteMask(SC00_90, BIT5,BIT6|BIT5|BIT4);
                msWriteByteMask(SC00_0F, 0x02, 0x0F); //Ylock line
                msWriteByteMask(SC00_0C, 0,BIT6|BIT5|BIT4);
                msWriteByteMask(SC00_28,0,BIT3);
                msWriteByteMask(SC00_CC,0,BIT3); //clear status
                msWriteByteMask(SC00_CE,BIT3,BIT3); //enable INT
                while( g_bEnableIsrDisableFlock && (bTimeOutCounterFlag) && (!InputTimingChangeFlag));
                if(InputTimingChangeFlag)
                {
                    msWriteByteMask(SC00_CE,0,BIT3); //disable INT
                    g_bEnableIsrDisableFlock = 0;
                    MsOS_DisableInterrupt(E_INT_PM_FIQ_EXTIMER0);
                    msDrvEnableExtTimer(FALSE);
                }
            }
            //while(((msRead2Byte(REG_LPLL_54)&0x1F4) !=0x1F4 )&& (bTimeOutCounterFlag) && (!InputTimingChangeFlag));
        }
        #if DEBUG_PRINT_ENABLE
        //printData( "EXECUTE_FRAME_PLL_LOCK takes time: %d", FRAMEPLL_TIMEOUT-u16TimeOutCounter );
        #endif
    }
    else if(g_SetupPathInfo.ucFrameLockMode == FLM_FBL_FRAMEPLL)
    {    
        WORD u16OutVTotal;
        
        u16OutVTotal = ((DWORD)SrcVTotal*PANEL_HEIGHT) / ((CURRENT_SOURCE_IS_INTERLACE_MODE()?2:1)*height);
        msWriteByteMask(SC00_28,0,BIT3);
        Delay1ms(10000/SrcVFreq+2); // enable flock and wait 1 frame for input reset output Vsyn
        SC0_SET_OUTPUT_VTOTAL((SC_MASK_V > (u16OutVTotal-1)) ? (u16OutVTotal-1) : SC_MASK_V);

        MENU_LOAD_START();
        DB_WBMask(REG_LPLL_1B,BIT1|BIT0,BIT1|BIT0);//enable ivs detect power down
        DB_WBMask(REG_LPLL_18,BIT3,BIT3);
        DB_WBMask(SC00_28,BIT3,BIT3);
        MENU_LOAD_END();

        WaitOutputFpllStable();
        #if DEBUG_PRINT_ENABLE
        if((u16OutVTotal > g_sPnlInfo.sPnlTiming.u16VttMax) || ((u16OutVTotal-1) > SC_MASK_V))
            printMsg("Output Vtt of FLM_FBL_FRAMEPLL over spec!!!");
        
        printData( "FLM_FBL_FRAMEPLL takes time: %d", FRAMEPLL_TIMEOUT-u16TimeOutCounter );
        #endif
    }
    #if DEBUG_PRINT_ENABLE
        printData( "g_SetupPathInfo.ucFrameLockMode: %d", g_SetupPathInfo.ucFrameLockMode );
    #endif
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////
///////////////////////App MStar CB Fubction  ///////////////////////////
/////////////////////////////////////////////////////////////////////////
static bool b_appmStar_CbInitDone = FALSE;
fpappmStarCb appmStar_CB_FuncList[eCB_appmStar_CBMaxNum];

void appmStar_CbAttach(BYTE CbType, void *CbFunc)
{
    appmStar_CB_FuncList[CbType] = (fpappmStarCb)CbFunc;
}

bool appmStar_CbGetInitFlag(void)
{
    return b_appmStar_CbInitDone;
}

void appmStar_CbSetInitFlag(bool bInit)
{
    b_appmStar_CbInitDone = bInit;
}
