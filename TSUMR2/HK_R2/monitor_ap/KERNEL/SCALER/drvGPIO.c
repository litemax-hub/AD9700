///////////////////////////////////////////////////////////////////////////////
/// @file drvScalerGPIO.c
/// @brief Scaler GPIO/PWM access
/// @author MStarSemi Inc.
///
/// Driver for Scaler GPIO/PWM access.
///
/// Features
///  -
///  -
///////////////////////////////////////////////////////////////////////////////
#define _DRVGPIO_C

#include "types.h"
#include "board.h"
//#include "global.h"
#include "ms_reg.h"
#include "Ms_rwreg.h"
//#include "DebugMsg.h"
#include "Debug.h"
#include "drvGPIO.h"

#define GPIO_DEBUG 0
#if ENABLE_DEBUG && GPIO_DEBUG
    #define GPIO_printData(str, value)   printData(str, value)
    #define GPIO_printMsg(str)           printMsg(str)
#else
    #define GPIO_printData(str, value)
    #define GPIO_printMsg(str)
#endif

static WORD PWM_CLK;
static BYTE u8BackupPWMDuty[_DISP_PWM_MAX_];
static WORD u16BackupPWMShift[_DISP_PWM_MAX_];
static DWORD u32BackupPWMShiftRegVal[_DISP_PWM_MAX_];
static DWORD u32BackupPWMPeriodRegVal[_DISP_PWM_MAX_];
const WORD u16PadMappingTable[4][_DISP_PWM_MAX_ - _DISP_PWM0_] = {
    {_DISP_PWM0_, _DISP_PWM1_, _DISP_PWM2_, _DISP_PWM3_, _DISP_PWM4_, _DISP_PWM5_},
    {_DISP_PWM2_, _DISP_PWM3_, _DISP_PWM4_, _DISP_PWM5_, _DISP_PWM0_, _DISP_PWM1_},
    {_DISP_PWM4_, _DISP_PWM5_, _DISP_PWM0_, _DISP_PWM1_, _DISP_PWM2_, _DISP_PWM3_},
    {_DISP_PWM2_, _DISP_PWM3_, _DISP_PWM0_, _DISP_PWM1_, _DISP_PWM4_, _DISP_PWM5_}
};
static BOOL bBackupPWMPolarity[_DISP_PWM_MAX_];
static BOOL bSWPWMDoubleBuffer[_DISP_PWM_MAX_];
static BYTE u8PWMMappingType;

void msAPIDispPWMMappingConfig(BYTE u8MappingType)
{
    u8PWMMappingType = u8MappingType&0x03;
    msWriteByteMask(REG_103D71, u8PWMMappingType, 0x03);
}
//PWM driver
WORD msDrvPWMMaskTransfer(BYTE u8PWMCh)
{
    if(u8PWMCh ==_NO_USE_)
        return _DISP_PWM_MAX_;
    else if((u8PWMCh ==_PWM0_GP0_)||(u8PWMCh ==_PWM0_GP1_))
        return _PWM0_;
    else if((u8PWMCh ==_PWM1_GP0_)||(u8PWMCh ==_PWM1_GP1_))
        return _PWM1_;
    else if((u8PWMCh ==_PWM2_GP0_)||(u8PWMCh ==_PWM2_GP1_)||(u8PWMCh ==_PWM2_GP2_))
        return _PWM2_;
    else if((u8PWMCh ==_PWM3_GP0_)||(u8PWMCh ==_PWM3_GP1_)||(u8PWMCh ==_PWM3_GP2_))
        return _PWM3_;
    else if((u8PWMCh ==_PWM4_GP0_))
        return _PWM4_;
    else if((u8PWMCh ==_PWM5_GP0_))
        return _PWM5_;
    else if((u8PWMCh ==_PWM6_GP0_))
        return _PWM6_;
    else if((u8PWMCh ==_PWM7_GP0_)||(u8PWMCh ==_PWM7_GP1_))
        return _PWM7_;
    else if((u8PWMCh ==_PWM8_GP0_)||(u8PWMCh ==_PWM8_GP1_)||(u8PWMCh ==_PWM8_GP2_))
        return _PWM8_;
    else if((u8PWMCh ==_PWM9_GP0_)||(u8PWMCh ==_PWM9_GP1_)||(u8PWMCh ==_PWM9_GP2_))
        return _PWM9_;
    else if(u8PWMCh ==_DISP_PWM0_GP0_)
        return u16PadMappingTable[u8PWMMappingType][_DISP_PWM0_ - _DISP_PWM0_];
#if CHIP_ID == CHIP_MT9700
    else if((u8PWMCh ==_DISP_PWM1_GP0_)||(u8PWMCh ==_DISP_PWM1_VSYNC_LIKE_))
#else
    else if(u8PWMCh ==_DISP_PWM1_GP0_)
#endif
        return u16PadMappingTable[u8PWMMappingType][_DISP_PWM1_ - _DISP_PWM0_];
    else if(u8PWMCh ==_DISP_PWM2_GP0_)
        return u16PadMappingTable[u8PWMMappingType][_DISP_PWM2_ - _DISP_PWM0_];
    else if(u8PWMCh ==_DISP_PWM3_GP0_)
        return u16PadMappingTable[u8PWMMappingType][_DISP_PWM3_ - _DISP_PWM0_];
    else if(u8PWMCh ==_DISP_PWM4_GP0_)
        return u16PadMappingTable[u8PWMMappingType][_DISP_PWM4_ - _DISP_PWM0_];
    else if(u8PWMCh ==_DISP_PWM5_GP0_)
        return u16PadMappingTable[u8PWMMappingType][_DISP_PWM5_ - _DISP_PWM0_];
    else
        return _DISP_PWM_MAX_;
}

static void msDrvPWMSWDBStart(BYTE u8PWMCh, BOOL* pbPWMSWDB)
{
    XDATA WORD u16PWMCh = msDrvPWMMaskTransfer(u8PWMCh);
    XDATA BYTE u8PWMConfig;

    if(u16PWMCh<_DISP_PWM0_||u16PWMCh==_DISP_PWM_MAX_)
        return;
    u8PWMConfig = msReadByte(REG_103D09+6*(u16PWMCh-_DISP_PWM0_));
    if(bSWPWMDoubleBuffer[u16PWMCh] && (u8PWMConfig & BIT6))
    {
        msWriteByte(REG_103D09+6*(u16PWMCh-_DISP_PWM0_), ((u8PWMConfig&(~(BIT6|BIT0))) | bBackupPWMPolarity[u16PWMCh]));
        (*pbPWMSWDB) = bSWPWMDoubleBuffer[u16PWMCh];
        bSWPWMDoubleBuffer[u16PWMCh] = 0;
    }
}

static void msDrvPWMSWDBEnd(BYTE u8PWMCh,const BOOL* pbPWMSWDB)
{
    XDATA WORD u16PWMCh = msDrvPWMMaskTransfer(u8PWMCh);
    XDATA BYTE u8PWMConfig;
    
    if(u16PWMCh<_DISP_PWM0_||u16PWMCh==_DISP_PWM_MAX_)
        return;
    u8PWMConfig = msReadByte(REG_103D09+6*(u16PWMCh-_DISP_PWM0_));
    if((*pbPWMSWDB) && !(u8PWMConfig & BIT6))
    {
        msWriteByte(REG_103D09+6*(u16PWMCh-_DISP_PWM0_), ((u8PWMConfig&(~(BIT6|BIT0))) | BIT6 | bBackupPWMPolarity[u16PWMCh]));
        bSWPWMDoubleBuffer[u16PWMCh] = (*pbPWMSWDB);
    }
}

DWORD msDrvPWMPeriodGet(BYTE u8PWMCh)
{
    XDATA DWORD u32Period;
    XDATA WORD u16PWMCh = msDrvPWMMaskTransfer(u8PWMCh);

    if(u16PWMCh<_DISP_PWM0_||u16PWMCh==_DISP_PWM_MAX_)
        return 0;

    u32Period = u32BackupPWMPeriodRegVal[u16PWMCh];
    return u32Period;
}

DWORD msDrvPWMShiftGet(BYTE u8PWMCh)
{
    XDATA WORD u16PWMCh = msDrvPWMMaskTransfer(u8PWMCh);
    XDATA DWORD u32PWMShift = 0;

    if(u16PWMCh<_DISP_PWM0_||u16PWMCh==_DISP_PWM_MAX_)
        return 0;

    u32PWMShift = u32BackupPWMShiftRegVal[u16PWMCh];
    return u32PWMShift;
}

void msAPIPWMShift(BYTE u8PWMCh, DWORD u32PWMShift)
{
    XDATA WORD u16PWMCh = msDrvPWMMaskTransfer(u8PWMCh);
    XDATA BYTE u8TempDuty=0;
    if(u16PWMCh<_DISP_PWM0_||u16PWMCh==_DISP_PWM_MAX_)
        return;
    u32PWMShift = (u32PWMShift<360)?u32PWMShift:0;
    u16BackupPWMShift[u16PWMCh] = u32PWMShift;

    u32PWMShift = (unsigned long)(1+msDrvPWMPeriodGet(u8PWMCh)) * u32PWMShift / 360;
    u32PWMShift = u32PWMShift?u32PWMShift:1;

    //backup pwm shift register setting
    u32BackupPWMShiftRegVal[u16PWMCh] = u32PWMShift;

    u8TempDuty = u8BackupPWMDuty[u16PWMCh];
    drvGPIO_SetPWMDuty(u8PWMCh,u8TempDuty);
}

void msDrvPWMAlignSync(BYTE u8PWMCh,BYTE u8Config, BYTE u8Counter)
{
    XDATA WORD u16PWMCh = msDrvPWMMaskTransfer(u8PWMCh);
    XDATA BOOL bVEnable = (u8Config & _PWM_VSYNC_ALIGN_EN_)?TRUE:FALSE;
    XDATA BOOL bHEnable = (u8Config & _PWM_HSYNC_ALIGN_EN_)?TRUE:FALSE;

    if(u16PWMCh >= _DISP_PWM_MAX_)
        return;

    if(bVEnable && bHEnable)
    {
        GPIO_printMsg("Both Hsync and Vsync align are enable, please redefine config \n");
        return;
    }
    if(u16PWMCh >= _DISP_PWM0_)     //DISP PWM
    {
        msWriteBit(REG_103D09+6*(u16PWMCh-_DISP_PWM0_), bVEnable|bHEnable, BIT2);     //reset enable

        if(u8Counter == 0&&bVEnable)        //vsync align
        {
            if(u16PWMCh % 2)
                msWriteBit(REG_103D28+(u16PWMCh-_DISP_PWM1_), 0, BIT7);
            else
                msWriteBit(REG_103D29+(u16PWMCh-_DISP_PWM0_), 0, BIT7);
        }
        else if(bHEnable)        //vsync align
        {
        	XDATA DWORD u32PWMFreq;
        	XDATA DWORD u32ModHFreq;
        	//XDATA DWORD u32Period;

        	u32PWMFreq = (unsigned long)XTAL_CLOCK_KHZ*1000/(1+msDrvPWMPeriodGet(u8PWMCh));
		//switch MODpin to TTL mode,to check HV freq only
		#if 0
        	msWrite2Byte(REG_MOD1_34, 0xFFFF);
        	msWrite2Byte(REG_MOD1_20, 0x0000);
        	msWrite2Byte(REG_MOD1_22, 0x0000);
		#endif

        	u32ModHFreq=g_sPnlInfo.sPnlTiming.u16Vtt*GetVfreq();
        	//GPIO_PRINT(" mode hsync freq ===%d \r\n",u32ModHFreq);
        	//GPIO_PRINT(" PWM freq ===%d \r\n",u32PWMFreq);

        	if (u32PWMFreq>=(u8Counter*u32ModHFreq))
        	{
	            if(u16PWMCh % 2)
	            {
	                msWriteBit(REG_103D28+(u16PWMCh-_DISP_PWM1_), 1, BIT7);
                    msWriteByteMask(REG_103D28+(u16PWMCh-_DISP_PWM1_), u8Counter,0x0F);
	            }
	            else
	            {
	                msWriteBit(REG_103D29+(u16PWMCh-_DISP_PWM0_), 1, BIT7);
                    msWriteByteMask(REG_103D29+(u16PWMCh-_DISP_PWM0_), u8Counter,0x0F);
	            }
        	}
        	else
        	{
                if(u16PWMCh % 2)
                {
                    msWriteBit(REG_103D28+(u16PWMCh-_DISP_PWM1_), 0, BIT7);
                }
                else
                {
                    msWriteBit(REG_103D29+(u16PWMCh-_DISP_PWM0_), 0, BIT7);
                }
        	}
        }
        else        //vsync align with counter
        {
            if(u16PWMCh % 2)
            {
                msWriteBit(REG_103D28+(u16PWMCh-_DISP_PWM1_), bVEnable|bHEnable, BIT7);
                msWriteByteMask(REG_103D28+(u16PWMCh-_DISP_PWM1_), u8Counter, 0x0F);
            }
            else
            {
                msWriteBit(REG_103D29+(u16PWMCh-_DISP_PWM0_), bVEnable|bHEnable, BIT7);
                msWriteByteMask(REG_103D29+(u16PWMCh-_DISP_PWM0_), u8Counter, 0x0F);
            }
            msWriteBit(REG_103D68, bVEnable, TransNumToBit(u16PWMCh-_DISP_PWM0_));
        }
    }
    else        //PWM
    {
        if(u16PWMCh < _PWM3_)
        {
            if(bVEnable | bHEnable)
                msWriteBit(REG_000583+(u16PWMCh<<1), 1, (bVEnable? BIT3: BIT4));
            else
                msWriteBit(REG_000583+(u16PWMCh<<1), 0, BIT3|BIT4);
        }
        else
        {
            if(bVEnable | bHEnable)
                msWriteBit(REG_00059D+((u16PWMCh-_PWM3_)<<1), 1, (bVEnable? BIT3: BIT4));
            else
                msWriteBit(REG_00059D+((u16PWMCh-_PWM3_)<<1), 0, BIT3|BIT4);
        }
    }
}

void drvGPIO_PWMAlignVSyncDelay(BYTE u8PWMCh,BYTE u8DlyNum)
{
    WORD u16PWMCh = msDrvPWMMaskTransfer(u8PWMCh);
    if((u8DlyNum==0xFF)||(u16PWMCh > _PWM9_))
        return;
    msWriteBit(REG_0005C4, BIT0, BIT0);
    msWriteByte(REG_0005B0+2*u16PWMCh, u8DlyNum);
}

void msDrvPWMDuty(BYTE u8PWMCh,DWORD u32Duty)
{
    XDATA WORD u16PWMCh = msDrvPWMMaskTransfer(u8PWMCh);
    XDATA DWORD u32Shift =  msDrvPWMShiftGet(u8PWMCh);
    XDATA DWORD u32Period =  msDrvPWMPeriodGet(u8PWMCh);
    XDATA WORD u16DutyExt = 0;
    XDATA BOOL bPWMSWDB = FALSE;

    if(u16PWMCh >= _DISP_PWM_MAX_)
        return;

    if(u16PWMCh >= _DISP_PWM0_)
    {
        msDrvPWMSWDBStart(u8PWMCh, &bPWMSWDB);
        if(u8BackupPWMDuty[u16PWMCh]==0x00 || u8BackupPWMDuty[u16PWMCh]==0xFF)
        {
            msWriteByteMask(REG_103D52+4*(u16PWMCh-_DISP_PWM0_), u8BackupPWMDuty[u16PWMCh]?(BIT2|BIT3):(BIT3), BIT2|BIT3);
        }
        else
        {
            if ((u32Duty+u32Shift)<u32Period)
            {
                u32Duty+=u32Shift;
            }
            else
            {
                u32Shift=((unsigned long) (u32Period-u32Duty)*360)/(1+u32Period);
                GPIO_printData(" ====>> maximum support shift is %d degree!! \r\n",u32Shift); //unit:n degree ,n=0~360

                //disable phase shift function.
                u32Shift=0;
            }
            msWrite3ByteMask(REG_103D50+4*(u16PWMCh-_DISP_PWM0_), u32Shift, 0xFFFFF);
            msWrite2Byte(REG_103D06+6*(u16PWMCh-_DISP_PWM0_), u32Duty);
            for(u16PWMCh=_DISP_PWM0_;u16PWMCh<_DISP_PWM_MAX_;u16PWMCh++)
            {
                u32Period = u32BackupPWMPeriodRegVal[u16PWMCh];
                u32Duty = LINEAR_MAPPING_VALUE(1+u8BackupPWMDuty[u16PWMCh], 0, 0x100, 0, 1+u32Period);
                u32Duty -= (u32Duty>0)?1:0;
                u32Shift =  u32BackupPWMShiftRegVal[u16PWMCh];
                if ((u32Duty+u32Shift)<u32Period)
                {
                    u32Duty+=u32Shift;
                }
                u16DutyExt |= (u32Duty >> 16) << ((u16PWMCh-_DISP_PWM0_)*2);
            }
            msWrite2Byte(REG_103D42, u16DutyExt);
        }     
        msDrvPWMSWDBEnd(u8PWMCh, &bPWMSWDB);
        //GPIO_PRINT("msDrvPWMDuty              u32Duty====%x,REG_103D06==%x\r\n\r\n",u32Duty   ,msRead2Byte(REG_103D06+6*(u16PWMCh-_DISP_PWM0_)));
        //GPIO_PRINT("msDrvPWMDuty              (u32Duty>>16)<<((u16PWMCh-_DISP_PWM0_)*2)==%x\r\n\r\n"   ,(u32Duty>>16)<<((u16PWMCh-_DISP_PWM0_)*2));
        //GPIO_PRINT("msDrvPWMDuty              (0x03)<<((u16PWMCh-_DISP_PWM0_)*2)==%x\r\n\r\n"   ,(0x03)<<((u16PWMCh-_DISP_PWM0_)*2));
    }
    else if(u16PWMCh < _PWM4_)
    {
        msWriteByte(REG_000590+(u16PWMCh), (BYTE)u32Duty);
        //GPIO_PRINT("duty_Add====%x,u32Duty==%x\r\n",REG_000590+(u16PWMCh),u32Duty);
    }
    else
    {
        msWriteByte(REG_0005AA+(u16PWMCh-_PWM4_), (BYTE)u32Duty);
        //GPIO_PRINT("duty_Add====%x,u32Duty==%x\r\n",REG_0005AA+(u16PWMCh-_PWM4_),u32Duty);
    }
}

void drvGPIO_SetPWMDuty(BYTE u8PWMCh, BYTE u8ByteDuty)
{
    XDATA DWORD u32Temp=msDrvPWMPeriodGet(u8PWMCh);
    XDATA WORD u16PWMCh = msDrvPWMMaskTransfer(u8PWMCh);
    XDATA DWORD u32RealDutyMapToRegSet=0;

    if( u16PWMCh >= _DISP_PWM_MAX_ )
        return;
    
    u8BackupPWMDuty[u16PWMCh]=u8ByteDuty;
    //GPIO_PRINT("u8ByteDuty=%x\r\n",u8ByteDuty);
    //GPIO_PRINT("getPreiod=%x\r\n",u32Temp);
    //GPIO_PRINT("u32RealDutyMapToRegSet=%x\r\n",u32RealDutyMapToRegSet);

    if(u16PWMCh>=_DISP_PWM0_)
    {//input pwm duty 0~0xFF , extend to 0~0x3FFFF;
        u32RealDutyMapToRegSet = LINEAR_MAPPING_VALUE(1+u8ByteDuty, 0, 0x100, 0, 1+u32Temp);
        u32RealDutyMapToRegSet -= (u32RealDutyMapToRegSet>0)?1:0;
    }
    else
    {
        u32RealDutyMapToRegSet = u8ByteDuty;
    }

    //GPIO_PRINT("@@@@@@@@@ u32Temp==%x\r\n",u32RealDutyMapToRegSet);
    msDrvPWMDuty(u8PWMCh, u32RealDutyMapToRegSet);
}

void drvGPIO_SetPWMFreq(BYTE u8PWMCh,DWORD u32PWMFreq)
{
    //_PWM0_GP0_ ~_PWM9_GP1_ :XTAL/((N+1)*256) NOTE:N=0~2048	 Freq:27Hz~55.9KHz
    //_DISP_PWM0_GP0_ ~_DISP_PWM5_GP1_ :	 Freq:~60~6M

    XDATA WORD u16PWMCh = msDrvPWMMaskTransfer(u8PWMCh), i;
    XDATA WORD u16TempShift=0;
    XDATA DWORD u32PWMPeriod;
    XDATA BYTE u8CheckEnableDB=0;
    XDATA WORD u16PWMDivider=1;
    XDATA BOOL bPWMSWDB = 0;
    u8CheckEnableDB = u8CheckEnableDB;
    if( u16PWMCh == _DISP_PWM_MAX_ )
        return;

    //GPIO_PRINT("\r\n\r\n\r\nu32PWMFreq==%x\r\n",u32PWMFreq);

    //PWM DIV
    if(u16PWMCh>=_DISP_PWM0_)
    {
        msDrvPWMSWDBStart(u8PWMCh, &bPWMSWDB);
        u32PWMPeriod = (u32PWMFreq?(((unsigned long)XTAL_CLOCK_KHZ*1000/u32PWMFreq)):0);
        //GPIO_PRINT(" u32PWMPeriod==%x     \r\n",u32PWMPeriod);

        while(u32PWMPeriod>0x40000)
        {
            u32PWMPeriod>>=1;
            u16PWMDivider<<=1;
        }
        msWriteByte(REG_103D08+6*(u16PWMCh-_DISP_PWM0_),(BYTE)(u16PWMDivider-1));
        u32PWMPeriod -= (u32PWMPeriod > 0)?1:0;
        //backup period setting
        u32BackupPWMPeriodRegVal[u16PWMCh] = u32PWMPeriod;
        msWrite2Byte(REG_103D04+6*(u16PWMCh-_DISP_PWM0_), u32PWMPeriod);
        u32PWMPeriod = 0;
        for(i=_DISP_PWM0_;i<_DISP_PWM_MAX_;i++)
            u32PWMPeriod |= u32BackupPWMPeriodRegVal[i] >> 16 << ((i-_DISP_PWM0_)*2);
        msWrite2Byte(REG_103D40,u32PWMPeriod);
        u16TempShift = u16BackupPWMShift[u16PWMCh];
        msAPIPWMShift(u8PWMCh, u16TempShift);
        msDrvPWMSWDBEnd(u8PWMCh, &bPWMSWDB);
        #if 0 //if enable DB of disp_pwm, the duty isnt correct if we set duty after setting frequency immediately.
        u8CheckEnableDB=msReadByte(REG_103D09+6*(u16PWMCh-_DISP_PWM0_));
        if (u8CheckEnableDB & BIT3)
        {
            ForceDelay1ms(50);
        }
        #endif
    }
    else
    {
        u32PWMPeriod = (u32PWMFreq?(((unsigned long)PWM_CLK*1000/(u32PWMFreq*256))-1):0);

        if (u32PWMPeriod<0x800)
        {
            if(u16PWMCh<_PWM3_)
            {
                msWrite2ByteMask(REG_000582+2*(u16PWMCh-_PWM0_),u32PWMPeriod,0x7FF);
                GPIO_printData("u32PWMPeriod=%x\n\r",msRead2Byte(REG_000582+(2*(u16PWMCh-_PWM0_)))&0x7FF);
            }
            else
            {
                msWrite2ByteMask(REG_00059C+2*(u16PWMCh-_PWM3_),u32PWMPeriod,0x7FF);
                GPIO_printData("u32PWMPeriod=%x\n\r",msRead2Byte(REG_00059C+(2*(u16PWMCh-_PWM3_)))&0x7FF);
            }
        }
    }
}

void drvGPIO_SetBacklightDuty(BYTE u8PWMCh, BYTE u8ByteDuty)
{
    drvGPIO_SetPWMDuty(u8PWMCh, u8ByteDuty);
}

void msDrvPWMDoubleBuffer(BYTE u8PWMCh, BYTE u8Config)
{
    XDATA WORD u16PWMCh = msDrvPWMMaskTransfer(u8PWMCh);
    XDATA BOOL bEnable = (u8Config & _PWM_DB_EN_)?TRUE:FALSE;
    XDATA BOOL bDBVsyncAlign = (u8Config & _PWM_DB_VSYNC_MODE_)?TRUE:FALSE;

    if( u16PWMCh == _DISP_PWM_MAX_ )
        return;

    if(u16PWMCh>=_DISP_PWM0_)
    {
        bSWPWMDoubleBuffer[u16PWMCh] = (u8Config & _PWM_SWDB_EN_)?TRUE:FALSE;
        if(bEnable)
        {
            msWriteByteMask(REG_103D09+6*(u16PWMCh-_DISP_PWM0_), (bDBVsyncAlign?(BIT6|BIT1):(BIT6|BIT3) | bBackupPWMPolarity[u16PWMCh]),BIT3|BIT1|BIT0);
        }
        else
        {
            msWriteByteMask(REG_103D09+6*(u16PWMCh-_DISP_PWM0_), ((BIT6)| bBackupPWMPolarity[u16PWMCh]), BIT6|BIT3|BIT1|BIT0);
        }
    }
    else
    {
        if(u16PWMCh<_PWM3_)
        {
            msWriteBit(REG_000583+2*u16PWMCh,bEnable,BIT5);
            //GPIO_PRINT("DoubleBuffer_ADD==%x\r\n",REG_000583+2*u16PWMCh);
        }
        else
        {
            msWriteBit(REG_00059D+2*(u16PWMCh-_PWM3_),bEnable,BIT5);
            //GPIO_PRINT("DoubleBuffer_ADD==%x\r\n",REG_00059D+2*(u16PWMCh-_PWM3_));
        }
    }
}

void msDrvPWMPolarity(BYTE u8PWMCh, BYTE u8Config)
{
    XDATA WORD u16PWMCh = msDrvPWMMaskTransfer(u8PWMCh);
    XDATA BOOL bEnable = (u8Config & _PWM_POLARITY_EN_)?TRUE:FALSE;
    XDATA BOOL bPWMSWDB = FALSE;

    if(u16PWMCh<_DISP_PWM0_||u16PWMCh==_DISP_PWM_MAX_)
        return;

    bBackupPWMPolarity[u16PWMCh] = bEnable;
    msDrvPWMSWDBStart(u8PWMCh, &bPWMSWDB);
    msWriteBit(REG_103D09+6*(u16PWMCh-_DISP_PWM0_), bEnable, BIT0);
    msDrvPWMSWDBEnd(u8PWMCh, &bPWMSWDB);
}

void msAPIPWMConfig(BYTE u8PWMCh, DWORD u32Freq, BYTE u8Duty, BYTE u8Config, BYTE u8Counter ,WORD u16Shift)
{
    BOOL bPWMSWDB = FALSE;
    //enable double buffer before setting duty to avoid incorrected duty after adjust pwm frequency.
    msDrvPWMDoubleBuffer(u8PWMCh,u8Config);
    msDrvPWMSWDBStart(u8PWMCh, &bPWMSWDB);
    drvGPIO_SetPWMFreq(u8PWMCh,u32Freq);
    msAPIPWMShift(u8PWMCh,u16Shift);
    drvGPIO_SetPWMDuty(u8PWMCh,u8Duty);
    msDrvPWMAlignSync(u8PWMCh,u8Config,u8Counter);
    msDrvPWMPolarity(u8PWMCh,u8Config);
    msDrvPWMSWDBEnd(u8PWMCh, &bPWMSWDB);
}

void msDrvPWMEnableSwitch(BYTE u8PWMCh, Bool bEnable, BYTE u8Config)//para1:_PWM0_GP0_~_DISP_PWM5_GP0_
{
    //this function will not disable previous selected PWM
    DWORD u32PWMCh;

    if (u8PWMCh == _NO_USE_)
    {
        GPIO_printMsg("Can not use this  PWM channel\n");
        return;
    }
    else
    {
        u32PWMCh = ((DWORD)BIT0<<(u8PWMCh - 1));
    }

    if((u8PWMCh ==_PWM0_GP0_)||(u8PWMCh ==_PWM0_GP1_))
    {
        msWriteBit(REG_000406, 0, BIT0);//reg_spdif_out_en
        msWriteBit(REG_000580, bEnable, BIT0);
    }
    else if((u8PWMCh ==_PWM1_GP0_)||(u8PWMCh ==_PWM1_GP1_))
        msWriteBit(REG_000580, bEnable, BIT1);
    else if((u8PWMCh ==_PWM2_GP0_)||(u8PWMCh ==_PWM2_GP1_)||(u8PWMCh ==_PWM2_GP2_))
        msWriteBit(REG_000580, bEnable, BIT2);
    else if((u8PWMCh ==_PWM3_GP0_)||(u8PWMCh ==_PWM3_GP1_)||(u8PWMCh ==_PWM3_GP2_))
        msWriteBit(REG_000580, bEnable, BIT7);
    else if((u8PWMCh ==_PWM4_GP0_))
        msWriteBit(REG_000581, bEnable, BIT0);
    else if((u8PWMCh ==_PWM5_GP0_))
        msWriteBit(REG_000581, bEnable, BIT1);
    else if((u8PWMCh ==_PWM6_GP0_))
        msWriteBit(REG_000581, bEnable, BIT2);
    else if((u8PWMCh ==_PWM7_GP0_)||(u8PWMCh ==_PWM7_GP1_))
        msWriteBit(REG_000581, bEnable, BIT3);
    else if((u8PWMCh ==_PWM8_GP0_)||(u8PWMCh ==_PWM8_GP1_)||(u8PWMCh ==_PWM8_GP2_))
        msWriteBit(REG_000581, bEnable, BIT4);
    else if((u8PWMCh ==_PWM9_GP0_)||(u8PWMCh ==_PWM9_GP1_)||(u8PWMCh ==_PWM9_GP2_))
        msWriteBit(REG_000581, bEnable, BIT5);

    //PM PWM pad control
    if(u8Config & _PWM_OD_EN_)//open drain
    {
        msWrite2ByteMask(REG_00040A,(bEnable ? u32PWMCh:0),(u32PWMCh)&0x3FF);
        #if CHIP_ID == CHIP_MT9701
        msWriteByteMask(REG_00040D,(bEnable ? u32PWMCh>>10:0),((u32PWMCh)>>10)&0x7F);
        #else
        msWriteByteMask(REG_00040D,(bEnable ? u32PWMCh>>10:0),((u32PWMCh)>>10)&0x0F);
        msWriteByteMask(REG_00040F,(bEnable ? u32PWMCh>>14:0),((u32PWMCh)>>14)&0x0F);
        #endif
        msWriteByteMask(REG_000419,(bEnable ? ((u32PWMCh)>>(_DISP_PWM0_GP0_-1)):0),((u32PWMCh)>>(_DISP_PWM0_GP0_-1))&0x3F);//DISP PWM
    }
    else
    {
		msWrite2ByteMask(REG_000408,(bEnable ? u32PWMCh:0),(u32PWMCh)&0x3FF);
        #if CHIP_ID == CHIP_MT9701
        msWriteByteMask(REG_00040C,(bEnable ? u32PWMCh>>10:0),((u32PWMCh)>>10)&0x7F);
        #else
		msWriteByteMask(REG_00040C,(bEnable ? u32PWMCh>>10:0),((u32PWMCh)>>10)&0x0F);
        msWriteByteMask(REG_00040E,(bEnable ? u32PWMCh>>14:0),((u32PWMCh)>>14)&0x0F);
        #endif
        #if CHIP_ID == CHIP_MT9700
        if(u8PWMCh == _DISP_PWM1_VSYNC_LIKE_)   // DISP PWM1 output at pad GPIOL1
            msWriteByteMask(REG_00041C, 1, BIT0|BIT1|BIT2); 
        else                                    // DISP PWM1 output at pad GPIO52
        #endif
        msWriteByteMask(REG_000418,(bEnable ? ((u32PWMCh)>>(_DISP_PWM0_GP0_-1)):0),((u32PWMCh)>>(_DISP_PWM0_GP0_-1))&0x3F); //DISP PWM
    }
}

void msAPIPWMClkSel(BYTE u8ClkType)
{
    if(u8ClkType > PWM_CLK_FRO_12M)
    {
        GPIO_printMsg("Wrong CLK Type Selection!!!\n");
        return;
    }

    switch(u8ClkType)
    {
        case PWM_CLK_XTAL:
            PWM_CLK = XTAL_CLOCK_KHZ;
            break;

        case PWM_CLK_FRO_12M:
            PWM_CLK = FRO_12M_KHZ;
            break;

        default:
            break;
    }

    msWriteByteMask(REG_000390, u8ClkType, (BIT1|BIT0));

}

void drvGPIO_uncall(void)
{
    drvGPIO_PWMAlignVSyncDelay(0, 0);
}


