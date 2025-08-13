//#include <intrins.h>
#include "types.h"
#include "board.h"
#include "Global.h"
#include "asmCPU.h"
#if (MS_PM)
#include "Ms_PM.h"
#endif
#if ENABLE_DP_INPUT
#include "mapi_DPRx.h"
#endif

void _nop_(void)
{
asm __volatile__ ( "l.nop;" );
}

void Delay4us( void )
{
#if 1
    DelayUs(4);
#else
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
#if (MS_PM)
    if( g_bMcuPMClock )
    {
        BYTE i;
        for( i = 0; i < PM_DELAY4US_LOOP; i++ )
        {
            _nop_();
            _nop_();
            _nop_();
            _nop_();
            _nop_();
            _nop_();
            _nop_();
        }

    }
#endif
#endif

}
void ForceDelay1ms( WORD msNums )
{
    if( !msNums )
        return;

    u16DelayCounter = msNums;
    bDelayFlag = 1;

	while( bDelayFlag );
}

void ForceDelay1ms_Pure( WORD msNums )
{
    if( !msNums )
        return;

    u16DelayCounter = msNums;
    bDelayFlag = 1;

    while( bDelayFlag );
}

void Delay1ms( WORD msNums )
{
    if( !msNums )
        return;

    u16DelayCounter = msNums;
    bDelayFlag = 1;
    while( bDelayFlag )
    {
        if( InputTimingChangeFlag )
        {
            u16DelayCounter = 0;
            bDelayFlag = 0;
            break;
        }
    };
}




BYTE GetVSyncTime( void )
{
    WORD freq = 1;

    #if ENABLE_DP_INPUT
    if(IS_DP_FREESYNC())
    {
        WORD u16PixClk, u16Htt;

        u16PixClk = mapi_DPRx_GetPixelClock10K(SrcInputType);//msAPI_combo_IPGetPixelClk();
        u16Htt = msAPI_combo_IPGetGetHTotal();
        if((u16Htt!=0)&&(SrcVTotal!=0))
        {
            freq = ((DWORD)u16PixClk * 100 + u16Htt/2)/u16Htt;
        }
    }
    else
    #endif
    {
        freq = HFreq( SrcHPeriod );//(( DWORD )MST_CLOCK_MHZ * 10 + SrcHPeriod / 2 ) / SrcHPeriod; //get hfreq round 5
    }
    freq = (( WORD )SrcVTotal * 10 + ( freq / 2 ) ) / freq;
    return ( BYTE )freq;
}


void DELAY_NOP(BYTE msNums)
{
	while((msNums--))
    {
		_nop_();
	}
}

DWORD GetSortMiddleNumber(DWORD* arr, int len)
{
    int i = 0, j = 0;
    DWORD temp = 0;
    DWORD return_data = 0;

    for(i = 0; i < len; i++)
    {
        for(j = i+1;j<len;j++)
        {
            if(arr[i]>arr[j])
            {
                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }

    if(len%2)
    {
        return_data = arr[(len/2)+1];
    }
    else
    {
        return_data = arr[(len/2)];
    }
    return return_data;
}

//**************************************************************************
//  [Function Name]:
//                  msDrvSAREnableSwitch(WORD u16Ch, Bool bEnable)
//  [Description]
//                  enable / disable SAR channels
//  [Arguments]:
//                  bEnable: 0(disable) / 1(enable)
//  [Return]:
//
//**************************************************************************
void msDrvSAREnableSwitch(WORD u16Ch, Bool bEnable)
{
    msWriteByteMask(REG_003A62, bEnable?0x0F:0, u16Ch&0x0F); // GPIO Select / SARADC Input // 0: GPIO, 1: SARADC Input
    msWriteByteMask(REG_003A66, bEnable?0xFF:0, u16Ch>>4);
    msWrite2ByteMask(REG_003A68, bEnable?0xFFFF:0, u16Ch); // reg_ch_x_en // 0: disable, 1: enable
}

BYTE msReadSARData(BYTE ch)
{
	WORD predata1 = 0xFFFF,predata2 = 0xFFFF,currdata = 0xFFFF;
	BYTE counter = 0;

	msWriteByteMask(REG_003A67,BIT4,BIT4); // Update
	for(;counter<10;counter++)
	{
		currdata = msRead2Byte(REG_003A20+ch*2);
		if((abs(currdata-predata1)<0x20) && (abs(currdata-predata2)<0x20))
			break;
		predata2 = predata1;
		predata1 = currdata;
	}
	return (BYTE)(currdata>>2);
}

WORD msReadSARData_10bit(BYTE ch)
{
    WORD predata1 = 0xFFFF,predata2 = 0xFFFF,currdata = 0xFFFF;
    BYTE counter = 0;

    #if (CHIP_ID == CHIP_MT9701)
    msWriteByteMask(REG_003A67,BIT4,BIT4); // Update
    #endif
    for(;counter<10;counter++)
    {
        currdata = msRead2Byte(REG_003A20+ch*2);
        if((abs(currdata-predata1)<0x20) && (abs(currdata-predata2)<0x20))
            break;
        predata2 = predata1;
        predata1 = currdata;
    }
    return (currdata);
}

#define TimeProfilingStampResetValue    (unsigned)(0-1)
void BootTimeStamp_Set(BYTE type, BYTE index, BOOL firstTimeSet)
{
#if ENABLE_BOOT_TIME_PROFILING
    if(index >= _TIME_STAMP_NUM_MAX_)
    {
        return;
    }

    if(firstTimeSet && (g_u32TimeStamp[type][index] != TimeProfilingStampResetValue))
    {
        return;
    }

    g_u32TimeStamp[(TIME_STAMP_TYPE)type][index] = MAsm_GetSystemTime();

#else
    UNUSED(type);
    UNUSED(index);
    UNUSED(firstTimeSet);
#endif
}

MS_U32 BootTimeStamp_Get(BYTE type, BYTE index)
{
#if ENABLE_BOOT_TIME_PROFILING
    if(index >= _TIME_STAMP_NUM_MAX_)
        return 0;
    return g_u32TimeStamp[(TIME_STAMP_TYPE)type][index];
#else
    UNUSED(type);
    UNUSED(index);
    return 0;
#endif
}

void BootTimeStamp_Clr(void)
{
#if ENABLE_BOOT_TIME_PROFILING
    extern MS_U32 gsystem_time_ms;
    BYTE i, j;

    gsystem_time_ms = 0; // rst counter
    for(i=0; i<MAX_TS; i++)
        for(j=0; j<_TIME_STAMP_NUM_MAX_; j++)
            g_u32TimeStamp[i][j] = TimeProfilingStampResetValue;
#endif
}

