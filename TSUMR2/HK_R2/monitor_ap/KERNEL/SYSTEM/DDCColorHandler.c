#include "Global.h"
#include "DDCColorHandler.h"
#include "NVRam.h"
#include "msflash.h"
#include "msFB.h"
#include "drvMcu.h"
#include "halRwreg.h"
#include "drvHDR.h"
#include "AutoGamma.h"
#include "Adjust.h"
#include "drvMcu.h"
#include "drvHDRCommon.h"
#include "mw_usbdownload.h"
#include "drvIMI.h"
#include "imginfo.h"
#include "msACE.h"
#if ENABLE_LOCALDIMMING
#include "msLD_Handler.h"
#endif

XDATA WORD g_CheckSumResult;
XDATA BYTE xdata bToolWriteGamma=0;
XDATA WORD xdata u16ChkSum_Addr=0;
XDATA BYTE PreStoreCheckSum = 0xff;
XDATA BYTE PreGmaChennel = 0xff;
XDATA DWORD PreAddr = 0xff;
XDATA BYTE StoreCheckSum = 0;

#if !ENABLE_DeltaE
XDATA CSTransferSettingType g_CS_Setting[2];

XDATA BYTE* tAllFGamma3x3Matrix[2] =
{
     g_CS_Setting[0].CM,
     g_CS_Setting[1].CM,
};
#endif
XDATA WORD _Y2REn = 0xFFFF;

#if CHIP_ID==CHIP_MT9701
    #define WINISP_ReadByte(addr)   msMemReadByte(addr)
    #define WINISP_WriteByte(addr,data) msMemWriteByte(addr,data)
    #define WINISP_START_ADDR   MIU_OD_ADDR_START
    #define WINISP_BUFF_SIZE    MIU_OD_SIZE
    #define WINSP_BUFF_PTR(u32Addr) ((U8*)((u32Addr) | (0x80000000)))
#else
    #define WINISP_ReadByte(addr)   IMI_ReadByte(addr)
    #define WINISP_WriteByte(addr,data) IMI_WriteByte(addr,data)
    #define WINISP_START_ADDR   DEF_IMI_OD_START_ADDR
    #define WINISP_BUFF_SIZE    DEF_IMI_OD_SIZE
    #define WINSP_BUFF_PTR(u32Addr) IMI_GetIMIRegs(u32Addr)
#endif

#if (ENABLE_WINISP)
//ISP
//--------------------------------------------------
#define CommonVersion   "1.2.2"
#define SectorCompareISP 0
#define EnableDebugWinISP 0
#if EnableDebugWinISP
#define WinISP_PrintMsg(str)  printMsg(str)
#define WinISP_PrintData(str, value) printData(str, value)
#else
#define WinISP_PrintMsg(str)
#define WinISP_PrintData(str, value)
#endif
xdata WORD _checkSum;
xdata WORD _imgOneCheckSum;
xdata WORD _ISPStatus = 0x99; //0x99 : Idle, 0: Busy, 1: Sector ISP Fail, 2:Sector ISP Succes
xdata DWORD _pktCnt;
xdata DWORD _pktSize;
code WORD _sectorSize = 0x1000;
code DWORD _imageSize = 0x100000;
#define _xdata2DramISP        ((unsigned char volatile xdata *) 0x0000)
code DWORD _dramAddrISP = MIU_AUTOLOAD_ADDR_START;
code DWORD _xdataAddrISP = (WIN1_ADDR_START << 10);//AUTOLOAD_HDR_ADR  |0x80000000;
extern void msAPI_OverDriveEnable(BOOL bEnable);
//sector check sum
#if SectorCompareISP
xdata BYTE _sectorCHS[256];
xdata BYTE _imgSectorCHS[256];
#endif
BYTE _serialNumber[32];
PatternType _PtnType = 0;
XDATA static BYTE _Y2RCMStatus[8] = {0};
XDATA static BYTE _R2YCMStatus[8] = {0};
#if ENABLE_DeltaE
XDATA static BYTE _HDRCMStatus[8] = {0};
#endif



static double _percentOfCentralArea = 0.1;

//--------------------------------------------------
BOOL ExecuteISP(WORD fileCheckSum)
{

    XDATA WORD wrCnt = 0;
    XDATA DWORD wrAddr = 0;
    XDATA DWORD pktCnt = _pktSize;
    XDATA WORD idx = 0;
    XDATA WORD checkSum = 0;
    XDATA DWORD imgAddr = 0;
    XDATA DWORD errAddr = 0;
    XDATA BYTE retryCnt = 0;
    XDATA WORD sectorCheckSum = 0;

#if SectorCompareISP
    XDATA WORD sectorCnt = 0;
#endif

	U32 u32DualImgOffset=0;
	if(g_dwSpiDuelImageOffset < ENABLE_USB_HOST_DUAL_IMAGE_OFFSET)
		u32DualImgOffset = ENABLE_USB_HOST_DUAL_IMAGE_OFFSET;
	else
		u32DualImgOffset = 0;

	WinISP_PrintData("g_dwSpiDuelImageOffset = 0x%x",g_dwSpiDuelImageOffset);
	WinISP_PrintData("u32DualImgOffset = 0x%x",u32DualImgOffset);
	imgAddr = u32DualImgOffset;

    WinISP_PrintData("ISP start. Image Size (K Bytes) : %d", (WORD)(_pktSize >> 10) );
    WinISP_PrintData("ISP file#1 checksum : %x", fileCheckSum);
    while (pktCnt > 0)
    {

        if (pktCnt >= _sectorSize)
        {
            wrCnt = _sectorSize;
        }else{
            wrCnt = pktCnt;
        }

        //specifiy dram addr
        //mcu4kXdataMapToDRAMIMI(_dramAddrISP + wrAddr, 1);
        //printData("dram addr H: %x", (WORD)((_dramAddrISP + wrAddr) >> 16));
        //Delay1ms(0);
        for(idx = 0; idx < _sectorSize; idx++)
        {
            //checkSum += _xdata2DramISP[_xdataAddrISP + idx ];
			checkSum += WINISP_ReadByte(WINISP_START_ADDR + wrAddr + idx);
        }
        wrAddr += wrCnt;
        pktCnt -= wrCnt;

    }

    WinISP_PrintData("ISP file#2 checksum : %x", checkSum);

    if(checkSum != fileCheckSum)
    {
        WinISP_PrintMsg("ISP checkSum compare error #1");
        return FALSE;
    }

    //msDISet2ndInvalid();
    pktCnt = _pktSize;
    wrAddr = 0;
    checkSum = 0;

    while (pktCnt > 0)
    {
        if (pktCnt >= _sectorSize)
        {
            wrCnt = _sectorSize;
        }else{
            wrCnt = pktCnt;
        }

#if( ENABLE_WATCH_DOG )
        ClearWDT();
#endif
        errAddr = 0;
        //specifiy dram addr
        //mcu4kXdataMapToDRAMIMI(_dramAddrISP + wrAddr, 1);

#if SectorCompareISP
        sectorCnt = wrAddr  / _sectorSize;
        if(_sectorCHS[sectorCnt] != _imgSectorCHS[sectorCnt])
#endif
        {
        	//WinISP_PrintData("wrAddr: %x\n", wrAddr);
        	//WINISP_FlashWriteTbl(TRUE, imgAddr + wrAddr, (BYTE*)&_xdata2DramISP[_xdataAddrISP], wrCnt, 0);
            WINISP_FlashWriteTbl(TRUE, imgAddr + wrAddr, WINSP_BUFF_PTR(WINISP_START_ADDR+wrAddr), wrCnt, 0);
		}
        sectorCheckSum = 0;
        //Compare data
        //WinISP_PrintData("pktCnt : %x\n", pktCnt);
        for(idx = 0; idx < wrCnt; idx++)
        {
        	//if (_xdata2DramISP[_xdataAddrISP + idx] == FlashReadAnyByte(imgAddr + wrAddr + idx))
            if (WINISP_ReadByte(WINISP_START_ADDR + wrAddr + idx) == FlashReadAnyByte(imgAddr + wrAddr + idx))
            {
                //sectorCheckSum += _xdata2DramISP[_xdataAddrISP + idx];
				sectorCheckSum += WINISP_ReadByte(WINISP_START_ADDR + wrAddr + idx);
				//WinISP_PrintData("%x ", msIMIRegs[DEF_IMI_OD_START_ADDR + wrAddr + idx]);
                continue;
            }else{
                errAddr = imgAddr + wrAddr + idx;
                WinISP_PrintData("ISP verify fail at block(64K) addr : %x", (errAddr >> 16));
            }
        }

        if (errAddr == 0)
        {
            wrAddr += wrCnt;
            pktCnt -= wrCnt;
            retryCnt = 0;
            checkSum += sectorCheckSum;
        }else{
            retryCnt++;
        }

        if(retryCnt >10)
        {
            WinISP_PrintMsg("ISP verify fail.");
            return FALSE;
        }
    }

    if(checkSum != fileCheckSum)
    {
        WinISP_PrintMsg("ISP checkSum compare error #2");
        return FALSE;
    }

    //msDISet2ndValid();
    WinISP_PrintMsg("ISP end.");
    return TRUE;
}

void RebootSys(void)
{
    msWrite4Byte( REG_002C08, 1);
}

void ClearMemory(void)
{
    DWORD cnt = 0;
    int idx = 0;
#if CHIP_ID!=CHIP_MT9701
	IMI_Init(WINISP_START_ADDR, WINISP_BUFF_SIZE);
#endif
	WinISP_PrintMsg("[ClearMemory]IMI_Init(WINISP_START_ADDR, WINISP_BUFF_SIZE)");

	for(cnt = 0; cnt < _pktSize; cnt+=_sectorSize)
	{
		//mcu4kXdataMapToDRAMIMI(_dramAddrISP + cnt, 1);
		for(idx = 0; idx < _sectorSize; idx++)
		{
			//_xdata2DramISP[_xdataAddrISP + idx] = 0xFF;
			WINISP_WriteByte(WINISP_START_ADDR + idx, 0xFF);
		}
	}
}
//-----------------------------------------------------
//---The user data area settings may different in different project.
BOOL IsUserDataArea(DWORD addr)
{
    if ((addr >= 0x7B000) && (addr <= 0x7FFFF))
    {
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------
#endif

#if !ENABLE_WINISP
//for avoiding compiler error (TSUMG.lin)
BYTE code _ispDummy = 0;
void WinISPDummy(void)
{
    BYTE xdata i = _ispDummy;
    i = i;
}
#endif


void SetFrameColorRGB(BYTE u8Red, BYTE u8Green, BYTE u8Blue)
{
    msWriteByte(SC10_33, u8Red);    // Set Red frame color
    msWriteByte(SC10_34, u8Green);  // Set Green frame color
    msWriteByte(SC10_35, u8Blue);   // Set Blue frame color
}

void msEnableColorEngineTestPattern(Bool bEnable)
{
    //Initial condition.
    if (_Y2REn == 0xFFFF)
    {
         _Y2REn = msRead2Byte(SC08_80);
    }

    if(bEnable)
    {
        _Y2REn = msRead2Byte(SC08_80);
        msWriteByte(OSD1_03, 0x28);
        msWriteBit(OSD1_02, 1, BIT2);
        msWriteBit(OSD1_03, 1, BIT4);
        msWriteByte(OSD2_E0, 0x05);
        msWriteByte(OSD2_E8, 0x80);
        msWriteByte(OSD2_E9, 0x07);
        msWriteByte(OSD2_EA, 0x38);
        msWriteByte(OSD2_EB, 0x04);
        msWriteByte(OSD2_EC, 0x3F);
        msWriteByte(OSD2_F4, 0x66);
        msWriteByte(OSD2_F5, 0x06);
        //Disabled Y2R when OSD test pattern is enabled.
        msWrite2Byte(SC08_80, 0);
    }
    else
    {
        msWrite2Byte(SC08_80, _Y2REn);
        msWriteByte(OSD1_03, 0x25);
        msWriteBit(OSD1_02, 0, BIT2);
        msWriteBit(OSD1_03, 0, BIT4);
        msWriteByte(OSD2_E0, 0x00);
    }
}

/*void msSetColorEngineTestPattern(BYTE u8Red, BYTE u8Green, BYTE u8Blue)
{
    msWriteByte(OSD2_F6, u8Red);
    msWriteByte(OSD2_F7, u8Green);
    msWriteByte(OSD2_F8, u8Blue);
}*/
BYTE CurrentOutputPattern[6];
BYTE _lastCmdLog[8] = {0};
void SaveLastCmd(BYTE cmd, BYTE data0, BYTE data1, BYTE data2, BYTE data3, BYTE data4, BYTE data5, BYTE data6)
{
   _lastCmdLog[0] = cmd;
   _lastCmdLog[1] = data0;
   _lastCmdLog[2] = data1;
   _lastCmdLog[3] = data2;
   _lastCmdLog[4] = data3;
   _lastCmdLog[5] = data4;
   _lastCmdLog[6] = data5;
   _lastCmdLog[7] = data6;

}

BOOL IsCommonCommand (BYTE CommandCode);

const BYTE CommonCommand[] = {
	MS_EN_TEST_PATTERN_WITHOUTSIGNAL,
    MS_EN_INTERNAL_PATTERN,
    MS_SET_INTERNAL_PATTERN,
    MS_EN_COLOR_ENGINE_PATTERN,
    MS_SET_COLOR_ENGINE_PATTERN,
    MS_SET_CSCCM_TEMP_DISABLE,
    MS_SET_Y2RCM_TEMP_DISABLE,
    MS_EN_XPercentPIP_PATTERN,
    MS_SET_XPercentPIP_PATTERN,
    MS_EN_HDR,
    MS_SET_HDR_COLOR_ENGINE_PATTERN,
    MS_SET_OSDBrightness,
    MS_GET_MODEL_NAME,
    MS_GET_MonitorSN,
    MS_BRIGHTNESS,
    MS_CheckDDC,
    MS_Read_ACK,
    MS_R2Reset51Flag,
    MS_WR_BLOCK,
    MS_DDCWriteReg,
    MS_DDCReadReg,
    MS_GetToolFlagStatus,
    MS_Read_LastCmd,
    MS_GetCurrentPatternOutput,
    MS_GetCommonVersion,
    MS_RunISP,
    MS_GetMemAddr,
    MS_Read_Status,
    MS_Set_LD_01Region_MaxBri,
    MS_Set_LD_01Region_DynaBri,
    MS_Set_LD_Gamma_Adjustment,
    MS_ISP_CHECK_CRC,
    MS_ISP_SET_RECVINFO,
    MS_ISP_GET_RECVINFO,
    MS_ISP_SETDATA,
    MS_ISP_GET_SENTINFO,
    MS_ISP_SET_SENTINFO,
    MS_ISP_RUN,
    MS_RUN_REGMAP_ISP,
    MS_ISP_GET_STATUS,
    MS_ISP_GET_ACT_IMG_NUM,
    MS_ISP_ACTIVATE_IMG_ONE,
    MS_ISP_REBOOT,
    MS_ISP_RESTORE_IMG_ONE,
    MS_ISP_GET_IMG_ONE_CHECKSUM,
    MS_ISP_GET_ALIVE,
    MS_ISP_GET_INFO,
    MS_MSBHK_Bypass,
    MS_WFlash_Test,
};

BYTE ColorCalibrationHandler(BYTE u8WinIdx)
{
#if ENABLE_DeltaE
	BYTE subCode = DDCBuffer[2];
    if (IsCommonCommand(subCode))
    {
        return CommonHandler(u8WinIdx);
    }
    else if (IsAutoColorCommand(subCode))
    {
        return AutoColorCalibrationHandler(u8WinIdx, &g_CheckSumResult);
    }
#else
	return CommonHandler(u8WinIdx);
#endif

    return 0;
}

BOOL IsCommonCommand (BYTE CommandCode)
{
    BYTE idx = 0;
    for (idx = 0; idx < sizeof(CommonCommand)/sizeof(BYTE); idx++)
    {
        if (CommonCommand[idx] == CommandCode)
            return TRUE;
    }
    return FALSE;
}

BYTE CommonHandler(BYTE u8WinIdx)
{
    switch(DDCBuffer[2])
    {
            case MS_BRIGHTNESS:
            {
                printf("[MS_BRIGHTNESS]");
                BYTE sub_cmd = DDCBuffer[3];

                if(sub_cmd == 0) //GetBrightnessPWM
                {

                }
                else if(sub_cmd == 1) //SetBrightnessPWM
                {
                    WORD value = 0;
                    value = (WORD)((DDCBuffer[5]<<8) | (DDCBuffer[4]));
                    printf("value = %x\n", value);
                    #if (ENABLE_HDR && LD_ENABLE/* && !LD_ACTIVEDIMMIING_ENABLE*/)
                    HDR_Level currHDRState = msGetHDRStatus(u8WinIdx);
                    if(currHDRState != HDR_OFF)
                        msAPI_SetGlobalBrightnessPWM(value);
                    else
                    #endif
                        msAPI_SetGlobalBrightnessPWM(PWMBoundaryClamp(value)); //FAE needs to modify Upper & lower bound used in PWMBoundaryClamp function. 			
                    }
                    else if(sub_cmd == 2) //Set Default BrightnessPWM (If spec. needs, FAE needs to modify BRIGHTNESS_REAL_MID value.)
                    {
                        msAPI_SetGlobalBrightnessPWM(BRIGHTNESS_REAL_MID);
                    }
            }
            break;
            case MS_SET_Y2RCM_TEMP_DISABLE :
            {
                //When HDR is enabled, ingore this command.
                //if (msGetHDRStatus(u8WinIdx) == 0)
                //{
                //   msDisableY2RColorMatrix(DDCBuffer[3]);
                //}
                if (DDCBuffer[3] == 0)
                {
                    msWriteByte( SC10_5E, _Y2RCMStatus[0] );
                    msWriteByte( SC0F_4C, _Y2RCMStatus[1] );
                }
                else
                {
                    _Y2RCMStatus[0] = msReadByte(SC10_5E);
                    _Y2RCMStatus[1] = msReadByte(SC0F_4C);
                    msWriteByte(SC10_5E, 0);
                    msWriteByte(SC0F_4C, 0);
                }
            }
            break;
            case MS_SET_CSCCM_TEMP_DISABLE:
            {
                g_CheckSumResult = MS_SET_CSCCM_TEMP_DISABLE + (((WORD)DDCBuffer[3]) << 8);
                if (DDCBuffer[3] == 0)
                {
                    //R2Y
                    msWriteByte(SC22_56, _R2YCMStatus[0]);
                    msWriteByte(SC22_57, _R2YCMStatus[1]);
                    msWriteByte(SC07_40, _R2YCMStatus[2]);
                    //Y2R
                    msWriteByte(SC10_5E, _Y2RCMStatus[0]);
                    msWriteByte(SC0F_4C, _Y2RCMStatus[1]);
                }
                else
                {
                    //R2Y
                    _R2YCMStatus[0] = msReadByte(SC22_56);
                    _R2YCMStatus[1] = msReadByte(SC22_57);
                    _R2YCMStatus[2] = msReadByte(SC07_40);
                    msWriteByte(SC22_56, 0);
                    msWriteByte(SC22_57, 0);
                    msWriteBit( SC07_40, 0, _BIT0 );
                    //Y2R
                    _Y2RCMStatus[0] = msReadByte(SC10_5E);
                    _Y2RCMStatus[1] = msReadByte(SC0F_4C);
                    msWriteByte(SC10_5E, 0);
                    msWriteByte(SC0F_4C, 0);
                }
            }
            break;
            case MS_EN_HDR:
            {
                if(DDCBuffer[3] == 1)
                    msWriteByteMask(SC79_0E, BIT1, BIT1);
                else
                    msWriteByte(SC79_0E, 0x00);
            }
            break;
            case MS_EN_XPercentPIP_PATTERN :
            {
                _checkSum = DDCBuffer[3];
                g_CheckSumResult = DDCBuffer[3];
	            msWriteByteMask(SC7A_02, DDCBuffer[3]?BIT2:0, BIT2); // set HDRy2r bypass enable
                msWriteByteMask(SC7A_02, DDCBuffer[3]?BIT6:0, BIT6); // set HDRr2y bypass enable

                if(DDCBuffer[3] == 0)
                {
                    msSetIP2TestPattern_Off();
                }
                else
                {
                    msSetIP2TestPattern_ImageSize(u8WinIdx, SC0_READ_AUTO_WIDTH(),
                                                                SC0_READ_AUTO_HEIGHT());
                    _percentOfCentralArea = ((double)DDCBuffer[4])/100;
                }
            #if CHIP_ID == CHIP_MT9701
                _PtnType = PTN_IP2;
            #endif
            }
            break;
            case MS_SET_XPercentPIP_PATTERN :
            {
                msSetIP2TestPattern_XPercentPIP(u8WinIdx, _percentOfCentralArea, 
                                                (DDCBuffer[3]<<8|DDCBuffer[4]), 
                                                (DDCBuffer[5]<<8|DDCBuffer[6]),
                                                (DDCBuffer[7]<<8|DDCBuffer[8]));
                int i = 0;
                for(i=0;i<6;++i)
                    CurrentOutputPattern[i]=DDCBuffer[i+3];
            }
            break;
            case MS_ColorEngine_OnOff :
            {
                if ( DDCBuffer[3] == 0x00 )
                {
                    bToolWriteGamma=1;
                    //Color Engine Off
                    msWriteByteMask(SC25_02,  0, _BIT2|_BIT1 | _BIT0);	// fix-gamma/color-matrix/de-gamma disable.
                    msWriteByteMask(SC08_3F, 0, _BIT7); //post gamma off
                }
                else
                {
                    bToolWriteGamma=0;
                    msWriteByteMask(SC25_02,  _BIT2|_BIT1 | _BIT0, _BIT2|_BIT1 | _BIT0);  // fix-gamma/color-matrix/de-gamma enable.
                    msWriteByteMask(SC08_3F, _BIT7, _BIT7);//post gamma on
                }
            }
            break;
            case MS_SET_OSDBrightness :
            {
                // Use OSD API to Adjust Brightness from 0 to 100 
                _checkSum = DDCBuffer[3];
                #if (CHIP_ID==CHIP_MT9701)
                msAPI_SetGlobalBrightnessPWM(DDCBuffer[3]);
                #else
                BYTE u8Duty=0xFF;
                u8Duty = LINEAR_MAPPING_VALUE(DDCBuffer[3], 0, 0xFF, 0, 100);
                mStar_AdjustBrightness(u8Duty);
                #endif
            }
            break;
            case MS_GET_MODEL_NAME :
            {
				int idx =0, model_len = 0;

				while(tool_model_panel_name[model_len])
					++model_len;
				++model_len; //for the \0

				DDCBuffer[0] = model_len;
				for( idx = 0 ; idx < model_len-1 ; ++idx)
					  DDCBuffer[idx+1] = tool_model_panel_name[idx];
				DDCBuffer[model_len] = 0x00;
				return model_len;
            }
            break;
            case MS_GET_MonitorSN:
            {
                int idx =0, SN_len = 0;

                while(USER_PREF_SERIALNUMBER(SN_len))
                {
                    ++SN_len;
                }
                ++SN_len; //for the \0

                DDCBuffer[0] =SN_len;
                for( idx = 0 ; idx < SN_len-1 ; ++idx)
                {
                	  DDCBuffer[idx+1] = USER_PREF_SERIALNUMBER(idx);
                }
                DDCBuffer[SN_len] = 0x00;
                return SN_len;
            }
            break;
#if !ENABLE_DeltaE
			case MS_AutoGamma_OnOff :
            {
                if ( DDCBuffer[3] == 0x00 )
                {
                    //Off
		 		    msWriteByteMask(SC00_02, 0, BIT7)	;
				    msWriteByteMask(SC00_28, 0, BIT4|BIT5);
		 		    msWriteByteMask(SC10_32, 0, BIT4); //frame color disable

                }
                else
                {
                    //On
				    msWriteByteMask(SC00_02, BIT7, BIT7);
				    msWriteByteMask(SC00_28, BIT4|BIT5, BIT4|BIT5);
				    msWriteByteMask(SC10_32, BIT4, BIT4); //frame color enable
                }
            }
			break;

            case MS_AutoGamma_SetBGColor :
            {
                SetFrameColorRGB(DDCBuffer[3], DDCBuffer[4], DDCBuffer[5]);
            }
            break;

            case MS_WR_PostGamma :
            {
                XDATA DWORD addr, j;
                XDATA BYTE Sum, cnt, GmaMode, GmaChannel;
                XDATA DWORD CheckSum_addr=0,WordAddr=0;
                Sum=0;
                g_CheckSumResult =0 ;
                GmaMode = DDCBuffer[3] ; // gamma mode
                GmaChannel = DDCBuffer[4] ; // gamma channel
                cnt = DDCBuffer[5] ; // data size
                addr = (WORD)(DDCBuffer[6] << 8 );
                addr += DDCBuffer[7]; // start addr-hi, lo

                if((GmaChannel == PreGmaChennel) && (addr == PreAddr))
                    StoreCheckSum = PreStoreCheckSum;
                else
                    PreStoreCheckSum = StoreCheckSum;

                if((GmaChannel == 0) &&(addr  == 0))
                {
                    StoreCheckSum = 0;
                    PreStoreCheckSum = 0xff;
                    PreGmaChennel = 0xff;
                    PreAddr = 0xf;;
                }


                if (GmaMode == 0)
                {
                #if ENABLE_ColorMode_FUNCTION
                    WordAddr = BGammaTblAddr + GmaChannel*GammaTblSize + addr;
                    CheckSum_addr = BGammaCheckSumStartAddr;
				#endif
                }
				#if ENABLE_DICOM_FUNCTION
        		else if(GmaMode == 1)  //dicom
    			{
    				WordAddr = DicomGammaTblAddr + GmaChannel*DicomGammaTblSize + addr;
                    CheckSum_addr = DicomGammaCheckSumStartAddr;
    			}
				#endif
				#if ENABLE_HDR
                else if(GmaMode == 2)  //achdr
    			{
    				WordAddr = ACHDRGammaTblAddr + GmaChannel*ACHDRGammaTblSize + addr;
                    CheckSum_addr = ACHDRGammaCheckSumStartAddr;
    			}
                #endif

                for(j=0; j< cnt; j++)
                {
                    Sum ^=	DDCBuffer[8+j] & 0xFF;
                    #if USEFLASH
                    FlashWriteByte(TRUE, WordAddr+j, DDCBuffer[8+j]);
                    //printData("DDCBuffer[8+j]: 0x%x", DDCBuffer[8+j]);
                    #else
                    NVRam_WriteByte(WordAddr+j, DDCBuffer[8+j]);
                    #endif
                }

                g_CheckSumResult = Sum;
                PreGmaChennel = GmaChannel;
                PreAddr = addr;
                StoreCheckSum ^= g_CheckSumResult;

                #if USEFLASH
                if((GmaChannel == 2) && (addr == 40))
                    FlashWriteByte(TRUE, CheckSum_addr, StoreCheckSum);
                #else
                if((GmaChannel == 2) && (addr == 40))
                    NVRam_WriteByte(CheckSum_addr, StoreCheckSum);
                #endif

            }
            break;

            case MS_LoadColorMode :
            {
                XDATA BYTE type = 0, funcIdx = 0;
                type = DDCBuffer[3];
                funcIdx = DDCBuffer[4];

                switch(type)
                {
                    case AutoColorFunction_ColorMode :
                    {
                        switch(funcIdx)
                        {
                            #if ENABLE_ColorMode_FUNCTION
                            case 0x00: //sRGB
                                LoadDeltaEColorMode(u8WinIdx, 0);
                            break;
			    			#endif
                        }
                    }
                    break;

                    case AutoColorFunction_DICOMMode :
                    break;

                    case AutoColorFunction_GammaCurve :
                    break;

                    case AutoColorFunction_ColorTemp :
                    break;

                    case AutoColorFunction_ColorTrack :
                    break;

                    case AutoColorFunction_HDR :
                    #if ENABLE_ACHDR_FUNCTION
                    {
                        msSetHDREnable(u8WinIdx, funcIdx);
                    }
                    #endif
                    break;

                    case AutoColorFunction_UHDA :
                    break;

                    default :
                    break;
                }
            }

            #if ENABLE_HDR
            case MS_WR_HDRPanelInfo :
            {
                XDATA BYTE idx = 0, Sum = 0;
                XDATA DWORD CheckSum_addr = 0;
                g_CheckSumResult = 0;

                for(idx = 0 ; idx < sizeof(HDRPanelInfo) ; idx++)
                {
                    #if USEFLASH
                    FlashWriteByte(TRUE, HDRPanelInfoAddr+idx, DDCBuffer[3+idx]);
                    #else
                    NVRam_WriteByte(HDRPanelInfoAddr+idx, DDCBuffer[3+idx]);
                    #endif
                    Sum ^= DDCBuffer[3+idx];
                }

                g_CheckSumResult = Sum;
                CheckSum_addr = HDRPanelInfoCheckSumStartAddr;
                #if USEFLASH
                FlashWriteByte(TRUE, CheckSum_addr, g_CheckSumResult);
                #else
                NVRam_WriteByte(CheckSum_addr, g_CheckSumResult);
                #endif
            }
            break;

            case MS_WR_HDRColorTempGain :
            {
                XDATA BYTE idx = 0, Sum = 0;
                XDATA DWORD CheckSum_addr = 0;
                g_CheckSumResult = 0;

                for(idx = 0 ; idx < sizeof(HDRColorTemp) ; idx++)
                {
                    #if USEFLASH
                    FlashWriteByte(TRUE, HDRColorTempAddr+idx, DDCBuffer[3+idx]);
                    #else
                    NVRam_WriteByte(HDRColorTempAddr+idx, DDCBuffer[3+idx]);
                    #endif
                    Sum ^= DDCBuffer[3+idx];
                }

                g_CheckSumResult = Sum;
                CheckSum_addr = HDRColorTempCheckSumStartAddr;
                #if USEFLASH
                FlashWriteByte(TRUE, CheckSum_addr, g_CheckSumResult);
                #else
                NVRam_WriteByte(CheckSum_addr, g_CheckSumResult);
                #endif
            }
            break;

            case MS_LoadHDRColorMode :
            {

            }
            break;

            case MS_HDR_BacklightToMax:
            {
                if ( DDCBuffer[3] == 0x00 )
                {
                    //Set backlight in normal case
                }
                else
                {
                    // Two cases:
                    // 1. Make backlight to max luminance without local dimming driver
                    // 2. The middle area of dimming to max luminance(under current protection)

                }
            }
            break;

            case MS_WR_HDR_10StepLumMap:
            {
                XDATA BYTE idx = 0, Sum = 0;
                XDATA DWORD CheckSum_addr = 0;
                g_CheckSumResult = 0;

                for(idx = 0 ; idx < (sizeof(StoredHDRLumMap)-1); idx++)
                {
                    #if USEFLASH
                    FlashWriteByte(TRUE, HDRLumMapAddr+idx, DDCBuffer[3+idx]);
                    #else
                    NVRam_WriteByte(HDRLumMapAddr+idx, DDCBuffer[3+idx]);
                    #endif
                    Sum ^= DDCBuffer[3+idx];
                }

                g_CheckSumResult = Sum;
                CheckSum_addr = HDRLumMapCheckSumStartAddr;
                #if USEFLASH
                FlashWriteByte(TRUE, CheckSum_addr, g_CheckSumResult);
                #else
                NVRam_WriteByte(CheckSum_addr, g_CheckSumResult);
                #endif
            }
            break;

            case MS_Set_HDR_10StepLumMap:
            {
                BYTE HDRBrightness;
                HDRBrightness = HDR_MIN_DUTY + (DDCBuffer[3]*(HDR_MAX_DUTY - HDR_MIN_DUTY)/10);
                // Need to implement PWM control for measuring 10 step luminance value
                // ex.mStar_AdjustBrightness(HDRBrightness);
            }
            break;
            #endif
            case MS_WR_ColorMatrix :
            {
#if ENABLE_ColorMode_FUNCTION                 
                BYTE XDATA cnt, type,i;
                cnt = DDCBuffer[4];
                type = DDCBuffer[3];
                g_CheckSumResult = 0;
                for(i = 0 ; i < 18 ; i++)
                {
                    //tAllFGamma3x3Matrix[type][i] = DDCBuffer[5+i];
                    g_CS_Setting[type].CM[i] = DDCBuffer[5+i];
                    g_CheckSumResult ^= DDCBuffer[5+i];

                    #if USEFLASH
                        FlashWriteByte(TRUE, (RM_COLORSPACE_TRANS_BASEADDR + i) + (sizeof(CSTransferSettingType)*type) + (1*type) , g_CS_Setting[type].CM[i]);
                    #else
                        NVRam_WriteByte( (RM_COLORSPACE_TRANS_BASEADDR + i) + (sizeof(CSTransferSettingType)*type)+ (1*type), g_CS_Setting[type].CM[i]);
                    #endif
                }
		      //Save checksum
                    #if USEFLASH
                        FlashWriteByte(TRUE, (RM_COLORSPACE_TRANS_BASEADDR + sizeof(CSTransferSettingType)) + (sizeof(CSTransferSettingType)*type) + (1* type) + 1 , g_CheckSumResult);
                    #else
                        NVRam_WriteByte( (RM_COLORSPACE_TRANS_BASEADDR + sizeof(CSTransferSettingType)) + (sizeof(CSTransferSettingType)*type) +(1* type) + 1 , g_CheckSumResult);
                    #endif
#endif
            }
            break;
#else //ENABLE_DeltaE on

			case MS_EN_INTERNAL_PATTERN:
			{
				g_CheckSumResult = DDCBuffer[3];
	            if ( DDCBuffer[3] == 0x00 )
	            {
	                //Off
	                msSetTestPattern(FALSE, 0, 0, 0);
	            }
	            else
	            {
	                //On
	                msSetTestPattern(TRUE, 0, 0, 0);
                    _PtnType = PTN_INTERNAL;
	            }
			}
			break;
 			case MS_SET_INTERNAL_PATTERN:
 			{
 				CurrentOutputPattern[0] = 0;
 	            CurrentOutputPattern[2] = 0;
 	            CurrentOutputPattern[4] = 0;
 	            CurrentOutputPattern[1] = DDCBuffer[3];
 	            CurrentOutputPattern[3] = DDCBuffer[4];
 	            CurrentOutputPattern[5] = DDCBuffer[5];
 				msSetTestPattern(TRUE, DDCBuffer[3], DDCBuffer[4], DDCBuffer[5]);
 			}
 			break;
			case MS_Read_LastCmd:
	        {
	            int i = 0;
	            for(i=0;i<8;++i)
	            {
	        	  DDCBuffer[i+1] = _lastCmdLog[i];
	            }
	            DDCBuffer[0] = 8;
	            return 8;
	        }break;
			case MS_SET_HDR_COLOR_ENGINE_PATTERN :
	        {
				SaveLastCmd(MS_SET_HDR_COLOR_ENGINE_PATTERN,
                            DDCBuffer[3],
                            DDCBuffer[4],
                            DDCBuffer[5],
                            DDCBuffer[6],
                            0,
                            0,
                            0);
                if (DDCBuffer[3] == 0)
                {
				    msSetHDRColorEngineTestPattern(u8WinIdx, DDCBuffer[3], DDCBuffer[4], DDCBuffer[5], DDCBuffer[6], 0);
                    //rever HDR PQ:R2Y,Y2R
                    msWriteByte(SC7A_02, _HDRCMStatus[0]);
                }
                else
                {
                    //Backup HDR PQ:R2Y,Y2R
                    _HDRCMStatus[0] = msReadByte(SC7A_02);
				    msSetHDRColorEngineTestPattern(u8WinIdx, DDCBuffer[3], DDCBuffer[4], DDCBuffer[5], DDCBuffer[6], 0);
                }    

                if(DDCBuffer[3] == 0x01)
                    _PtnType = PTN_IP2;
	        }
	        break;
			#if ENABLE_OUTPUT_PATTERN_WITHOUTSIGNAL
 			case MS_EN_TEST_PATTERN_WITHOUTSIGNAL :
	        {
                #if (CHIP_ID == CHIP_MT9700)
				XDATA static BOOL _IsInitTestPatternWithoutSignal = FALSE;
				//printMsg("MS_EN_TEST_PATTERN_WITHOUTSIGNAL:");
	            g_CheckSumResult = DDCBuffer[3];
	            if ( DDCBuffer[3] == 0x00 )
	            {
	                //Off
	                printf("Off\n");
	                msExitTestPattern_WithoutSignal();
					_IsInitTestPatternWithoutSignal = FALSE;
	            }
	            else
	            {
	                //On
					if ( !_IsInitTestPatternWithoutSignal && SyncLossState() && FreeRunModeFlag)
	                {
	                	printf("On\n");
						msInitTestPattern_WithoutSignal();
						bRunToolFlag = TRUE;
						_IsInitTestPatternWithoutSignal = TRUE;
						extern void Osd_Hide( void );
						Osd_Hide();
	                }
	            }
                #endif
                #if (CHIP_ID == CHIP_MT9701)
                XDATA static BOOL _IsInitTestPatternWithoutSignal = FALSE;
				//printMsg("MS_EN_TEST_PATTERN_WITHOUTSIGNAL:");
	            g_CheckSumResult = DDCBuffer[3];
	            if ( DDCBuffer[3] == 0x00 )
	            {
	                //Off
	                printf("Off\n");
	                msExitIP2TestPattern_WithoutSignal();
					_IsInitTestPatternWithoutSignal = FALSE;
	            }
	            else
	            {
                    //On
					if ( !_IsInitTestPatternWithoutSignal && SyncLossState() && FreeRunModeFlag)
	                {
	                	printf("On\n");
						msInitIP2TestPattern_WithoutSignal();
						bRunToolFlag = TRUE;
						_IsInitTestPatternWithoutSignal = TRUE;
						extern void Osd_Hide( void );
						Osd_Hide();
	                }
	            }  
                #endif
	        }
	        break;
			#endif
 			case MS_EN_COLOR_ENGINE_PATTERN :
            {
                g_CheckSumResult = DDCBuffer[3];
 	            if ( DDCBuffer[3] == 0x00 )
 	            {
 	                //Off
 	                msSetColorEngineTestPattern(u8WinIdx, _DISABLE, 0x00, 0x00, 0x00);
 	            }
 	            else
 	            {
 	                //On
 	                msSetColorEngineTestPattern(u8WinIdx, _ENABLE, 0x00, 0x00, 0x00);
                    _PtnType = PTN_XVYCC;
 	            }
            }
            break;

			case MS_SET_COLOR_ENGINE_PATTERN :
			{
				int i = 0;
				for(i=0;i<6;++i)
				    	CurrentOutputPattern[i]=DDCBuffer[i+3];
			    msSetColorEngineTestPattern(u8WinIdx, _ENABLE, (DDCBuffer[3]<<8|DDCBuffer[4]), (DDCBuffer[5]<<8|DDCBuffer[6]), (DDCBuffer[7]<<8|DDCBuffer[8]));
			}
			break;

			case MS_WFlash_Test:
			{
			/*
			WORD u16Addr = 0xF900;
			BYTE i = 0;
			for(; i<16; i++)
			    FlashWriteByte(TRUE, u16Addr+i, i);
			*/
				FlashSectorErase( TRUE, ADDR_DELTAE_BLOCK );
			//WINISP_FlashSectorErase( TRUE, ADDR_DELTAE_BLOCK );
			}
			break;

			case MS_GetCurrentPatternOutput:
			{
			     int i = 0;
			     for(i=0;i<6;++i)
				 	DDCBuffer[i+1]=CurrentOutputPattern[i];
			        DDCBuffer[0] = 6;
			        return 6;
			}
			break;
#endif
            case MS_CheckDDC:
            {
                static int tValue = 0;
                BYTE command_write = DDCBuffer[3];
                BYTE value = DDCBuffer[4];
                if(command_write)
                {
                	tValue = value;
                }
                else
                {
                    DDCBuffer[1] = tValue & 0xFF;
                    DDCBuffer[0] = 1;
                    return 1;
                }
            }
            break;
            case MS_GetCommonVersion:
            {
                BYTE XDATA idx, len;
                len = sizeof(CommonVersion);
                DDCBuffer[0] = len;
                for( idx = 0 ; idx < len -1 ; ++idx)
                {
                    DDCBuffer[idx+1] = CommonVersion[idx];
                }
                DDCBuffer[len] = 0x00;
                return len;
            }
            break;
            case MS_Read_ACK :
            {
                DDCBuffer[2] = g_CheckSumResult >>8;
                DDCBuffer[1] = g_CheckSumResult & 0xFF;
                DDCBuffer[0] = 2;
                return 2;
            }
            break;

            case MS_WR_BLOCK :
            {
                bToolWriteGamma = DDCBuffer[3] ;
                bRunToolFlag = DDCBuffer[3];
            #if CHIP_ID == CHIP_MT9701
                if(mdrv_DeltaE_AutoColorToolStatus_Get() == ColorToolStatus_End)
                {
                    if(_PtnType == PTN_XVYCC)
                    {
                        msSetHDRColorEngineTestPattern(u8WinIdx, 0, 0xFF, 0xFF, 0xFF, 2);
                        msSetColorEngineTestPattern(u8WinIdx, _DISABLE, 0x00, 0x00, 0x00);
                    }
                    else if (_PtnType == PTN_IP2)
                    {
                        msSetHDRColorEngineTestPattern(u8WinIdx, 0, 0xFF, 0xFF, 0xFF, 2);
                    }
                    else if (_PtnType == PTN_INTERNAL)
                    {
                        msSetTestPattern(FALSE, 0, 0, 0);
                    }
                    Set_InputTimingChangeFlag();
                    mdrv_DeltaE_AutoColorToolStatus_Set(0);
                }
            #endif
            }
            break;

            case MS_GetToolFlagStatus :
            {
                DDCBuffer[1] = bRunToolFlag & 0xFF;
                DDCBuffer[0] = 1;
                return 1;
            }
            break;
    //#endif
#if (ENABLE_WINISP)
            case MS_ISP_CHECK_CRC:
            {
                DWORD crc = 0;
                int idx = 0;
                for(idx = 0; idx < 21; idx++)
                {
                    crc += FlashReadAnyByte(0xFFFE0 + idx);
                }
                DDCBuffer[4] = crc >>24;
                DDCBuffer[3]= crc >>16;
                DDCBuffer[2]= crc >>8;
                DDCBuffer[1]= crc & 0xFF;
                DDCBuffer[0] = 0x84;
                return  4;
            }break;

        case MS_ISP_SET_RECVINFO : {
            _pktSize = DDCBuffer[3] + (((DWORD)DDCBuffer[4]) << 8)
                                                   + (((DWORD)DDCBuffer[5]) << 16)
                                                   + (((DWORD)DDCBuffer[6]) << 24);

			if(_pktSize > WINISP_BUFF_SIZE)
			{
				_pktSize = 0;
				WinISP_PrintMsg("ERROR: _pktSize > WINISP_BUFF_SIZE.");
				return 0;
			}
            //disabled OD for using OD occupied memory
            msAPI_OverDriveEnable(FALSE);

            ClearMemory();
            _checkSum = 0;
            _pktCnt = 0;
            bRunToolFlag = TRUE;
            _ISPStatus = 0x99;
            }break;

        case MS_ISP_GET_RECVINFO : {
            DDCBuffer[4] = _pktSize >>24;
            DDCBuffer[3]= _pktSize >>16;
            DDCBuffer[2]= _pktSize >>8;
            DDCBuffer[1]= _pktSize & 0xFF;
            DDCBuffer[0] = 0x84;
            return  4;
            }break;

        case MS_ISP_SETDATA :
        {
            XDATA int idx = 0;
            XDATA int len = (DDCBuffer[0] & 0x7F) - 2; //-2 meas remove occupied the length of command codes
            _ISPStatus = 0;
            if(len <= 0)
                break;

            for(idx = 0; idx < len; idx++)
            {
                //----switch xdata map per 4K bytes
                if( _pktCnt % _sectorSize == 0 )
                {
                    //mcu4kXdataMapToDRAMIMI(_dramAddrISP + _pktCnt, 1);
                }
                //_xdata2DramISP[_xdataAddrISP + (_pktCnt & 0xFFF)] = DDCBuffer[3 + idx];
				WINISP_WriteByte(WINISP_START_ADDR + _pktCnt, DDCBuffer[3 + idx]);
				//_checkSum += _xdata2DramISP[_xdataAddrISP + (_pktCnt & 0xFFF)];
				_checkSum += WINISP_ReadByte(WINISP_START_ADDR + _pktCnt);
                _pktCnt++;
            }
            _ISPStatus = 0x99;
        }break;


        case MS_ISP_GET_SENTINFO :
        {
            DDCBuffer[6] = _pktCnt >>24;
            DDCBuffer[5]= _pktCnt >>16;
            DDCBuffer[4]= _pktCnt >>8;
            DDCBuffer[3]= _pktCnt & 0xFF;
            DDCBuffer[2]= _checkSum >>8;
            DDCBuffer[1]= _checkSum & 0xFF;
            DDCBuffer[0] = 0x86;
            return  6;
        }break;

        case MS_ISP_SET_SENTINFO :
        {
            _pktCnt = DDCBuffer[3] + (((DWORD)DDCBuffer[4]) << 8)
                                                  + (((DWORD)DDCBuffer[5]) << 16)
                                                  + (((DWORD)DDCBuffer[6]) << 24);
            _checkSum = 0;
            //mcu4kXdataMapToDRAMIMI(_dramAddrISP + _pktCnt, 1);
        }break;

        case MS_ISP_RUN:
        {
            _ISPStatus = 0;
            if (ExecuteISP(DDCBuffer[3] + (((WORD)DDCBuffer[4]) << 8)))
            {
                _ISPStatus = 2;
            }else
            {
                _ISPStatus = 1;
            }
        }break;

        case MS_ISP_GET_STATUS:
        {
            DDCBuffer[1]= _ISPStatus & 0xFF;
            DDCBuffer[0] = 0x81;
            return  1;
        }break;

        case MS_ISP_GET_ACT_IMG_NUM:
        {
            if(0)   // (msDIGet2ndValid())
            {
                //current activate image num is #2
                DDCBuffer[1]= 1;
            }else{
                //current activate image num is #1
                DDCBuffer[1]= 0;
            }
            DDCBuffer[0] = 0x81;
            return  1;
        }break;

        case MS_ISP_ACTIVATE_IMG_ONE:
        {
            //msDISet2ndInvalid();
        }break;

        case MS_ISP_REBOOT:
        {
            RebootSys();
        }break;

        case MS_ISP_RESTORE_IMG_ONE:
        {
            DWORD idx;
            WORD fileCheckSum;
#if SectorCompareISP
            WORD sectorCnt = 0;
#endif
            fileCheckSum =DDCBuffer[3] + (((WORD)DDCBuffer[4]) << 8);
            idx = 0;
            _imgOneCheckSum = 0;
            //disabled OD for using OD occupied memory
            msAPI_OverDriveEnable(FALSE);
            WinISP_PrintData("ISP move first image to memory start. Image Size (K): %d", (WORD)(_pktSize >> 10));
            WinISP_PrintData("ISP file#1 checkSum : %x", fileCheckSum);

            if (_pktSize > _imageSize)
            {
                WinISP_PrintMsg("ERROR : File size is large then image size.");
                break;
            }

            for(idx = 0; idx < _pktSize; idx++)
            {
                if(idx % _sectorSize == 0 )
                {
#if( ENABLE_WATCH_DOG )
                    ClearWDT();
#endif
                    //mcu4kXdataMapToDRAMIMI(_dramAddrISP + idx, 1);
#if SectorCompareISP
                    sectorCnt = idx / _sectorSize;
                    _imgSectorCHS[sectorCnt] = 0;
#endif
                }

#if SectorCompareISP
                //_xdata2DramISP[_xdataAddrISP + (idx & 0xFFF)] = FlashReadAnyByte(idx);
				WINISP_WriteByte(WINISP_START_ADDR + (idx & 0xFFF), FlashReadAnyByte(idx));
                //_imgSectorCHS[sectorCnt] += _xdata2DramISP[_xdataAddrISP + (idx & 0xFFF)];
                _imgSectorCHS[sectorCnt] += WINISP_ReadByte(WINISP_START_ADDR + (idx & 0xFFF));
#else
                //The binary data in user data area are all 0xFF
                if (IsUserDataArea(idx))
                {
                    //_xdata2DramISP[_xdataAddrISP + (idx & 0xFFF)] = 0xFF;
					WINISP_WriteByte(WINISP_START_ADDR + (idx & 0xFFF), 0xFF);
                }else{
                    //_xdata2DramISP[_xdataAddrISP + (idx & 0xFFF)] = FlashReadAnyByte(idx);
					WINISP_WriteByte(WINISP_START_ADDR + (idx & 0xFFF), FlashReadAnyByte(idx));
                }
#endif

                //_imgOneCheckSum +=  _xdata2DramISP[_xdataAddrISP + (idx & 0xFFF)];
				_imgOneCheckSum +=  WINISP_ReadByte(WINISP_START_ADDR + (idx & 0xFFF));
            }

#if SectorCompareISP

            printMsg("each sector check sum : ");
            for(idx = 0; idx < 256; idx++)
            {
                printData("%x", _imgSectorCHS[idx]);
                printData("%x", _sectorCHS[idx]);
            }

#else
            if (_imgOneCheckSum == fileCheckSum)
            {
                WinISP_PrintMsg("ISP move first image to memory success. ");
            }else{
                ClearMemory();
                WinISP_PrintMsg("ISP move first image to memory fail. ");
            }
            WinISP_PrintData("ISP file#2 checkSum : %x", _imgOneCheckSum);
#endif
            WinISP_PrintMsg("ISP move first image to memory end. ");
        }break;

         case MS_ISP_GET_IMG_ONE_CHECKSUM:
        {
            DDCBuffer[2]= _imgOneCheckSum >>8;
            DDCBuffer[1]= _imgOneCheckSum & 0xFF;
            DDCBuffer[0] = 0x82;
            return 2;
        }break;

        case MS_ISP_GET_ALIVE:
        {
            DDCBuffer[2] = DDCBuffer[3] + DDCBuffer[4];
            //chip type ==> 0: not defined, 1 : TSUMxxxG, 2: MST9UxxxT
            DDCBuffer[1] = 1;
            DDCBuffer[0] = 0x82;
            return  2;
        }break;

        case MS_ISP_GET_INFO:
        {
            BYTE u8InfoType = DDCBuffer[3];
            BYTE u8Len = 00;

            switch(u8InfoType)
            {
                case 0 : //Get HW_MODEL & HW_VERTION
                {
                    DDCBuffer[1] = HW_MODEL & 0xFF;
                    DDCBuffer[2] = HW_MODEL >> 8;
                    DDCBuffer[3] = HW_VERSION & 0xFF;
                    DDCBuffer[4] = HW_VERSION >> 8;
                    u8Len = 4;
                }break;

                case 1 : //Get HW_MODEL & HW_VERTION
                {
                    DDCBuffer[1] = AP_SW_MODEL & 0xFF;
                    DDCBuffer[2] = AP_SW_MODEL >> 8;
                    DDCBuffer[3] = AP_SW_VERSION & 0xFF;
                    DDCBuffer[4] = AP_SW_VERSION >> 8;
                    u8Len = 4;
                }break;
            }

            DDCBuffer[0] = 0x80 | u8Len;
            return  u8Len;
        }break;    

#if SectorCompareISP
        case MS_ISP_SET_SECTOR_CHS:
        {
            WORD offset = DDCBuffer[3] + (((WORD)DDCBuffer[4]) << 8);
            BYTE len = DDCBuffer[5];
            WORD idx = 0;
            for(idx = 0; idx < len; idx++)
            {
                _sectorCHS[idx + offset] = DDCBuffer[idx];
            }
        }break;

        case MS_ISP_GET_SECTOR_CHS:
        {
            WORD offset = DDCBuffer[3] + (((WORD)DDCBuffer[4]) << 8);
            BYTE len = DDCBuffer[5];
            BYTE type = DDCBuffer[6];
            WORD idx = 0;
            if(type == 0)
            {
                for(idx = 0; idx < len; idx++)
                {
                    DDCBuffer[idx+1] = _imgSectorCHS[idx + offset];
                }
            }else if(type == 1)
            {
                for(idx = 0; idx < len; idx++)
                {
                    DDCBuffer[idx+1] = _sectorCHS[idx + offset];
                }
            }else{
                for(idx = 0; idx < len; idx++)
                {
                    DDCBuffer[idx+1] = 0x00;
                }
            }
            DDCBuffer[0] = 0x80 | len;
            return len;
        }break;
#endif
        #endif

    }

    return 0;
}

#if 0//ENABLE_HDR
void msDisableY2RColorMatrix(Bool bEnable)
{
    XDATA BYTE idx = 0;

    //Matrix Enable
    if(!bEnable)
    {
        //limit YCC 2020 NCL to full RGB
        XDATA WORD limitYTofullRGB_BT2020[9] = {0x06BC, 0x04AC, 0x0000,
                                                0x1D64, 0x04AC, 0x1F40,
                                                0x0000, 0x04AC, 0x0898};

        XDATA WORD *p3x3 = limitYTofullRGB_BT2020;
        for(idx = 0; idx < 9 ; idx++)
        {
            msWrite2Byte(SC08_82 + idx * 2, p3x3[idx]);
        }

        msWriteByte(SC08_80, 0xE8);
        msWriteByte(SC08_81, 0xE8);
    }
    else
    {
        msWriteByte(SC08_80, 0x00);
        msWriteByte(SC08_81, 0x00);
    }

}
#endif

