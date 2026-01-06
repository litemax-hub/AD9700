#include "Global.h"
//#if ENABLE_HDR
#include "drvHDRPanelInfo.h"
#include "drvHDRCommon.h"
#include "drvHDR.h"
#include <string.h>
#include "ComboApp.h"
#include "AutoGamma.h"
#include "msLD_Handler.h"
#include "Adjust.h"
#include "MsOS.h"
#include "HDR_GammaTable_Default.h"
#include "drvLutAutoDownload.h"
#include "misc.h"
#include "mStar.h"
#include "msScaler.h"
#include "mapi_DPRx.h"

#define HDRComm_debug 1

#define HDR_TBD_Segment 0

extern void SetHDRColorFormat(ST_COMBO_COLOR_FORMAT cf);
extern void mdrv_HDR_DS_Start(void);
extern void mdrv_HDR_DS_End(void);

#if (ENABLE_MSTV_UART_DEBUG && HDRComm_debug)
    #define HDRComm_printData(str, value)   printData(str, value)
    #define HDRComm_printMsg(str)           printMsg(str)
#else
    #define HDRComm_printData(str, value)
    #define HDRComm_printMsg(str)
#endif

#if ENABLE_HDR_AUTODETECT
xdata SMPTE2086Metadata _currHdrMd;
xdata SMPTE2086Metadata _lastHdrMd;
#endif
xdata HDRUserPrefSettings _hdrPref;
xdata BYTE currHDRState = HDR_OFF;
xdata BOOL IsInitPanelInfo = FALSE;

#if HDR_TBD_Segment
XDATA WORD LumMap[] = {89, 93, 97, 102, 106, 110, 115, 119, 123, 127, 131, 136, 141, 144, 149, 154, 159, 163, 168, 172, 176, 181, 185, 189, 193, 196, 200, 206, 211, 213, 218, 223, 228, 232, 237, 241, 245, 249, 253, 258, 262, 265, 269, 274, 279, 282, 287, 293, 297, 301, 305, 309, 313, 318, 322, 326, 330, 333, 337, 342, 345, 350, 355, 360, 364, 368, 372, 376, 380, 384, 388, 392, 396, 398, 403, 408, 411, 415, 421, 425, 430, 434, 438, 442, 446, 450, 454, 458, 461, 464, 468, 473, 476, 481, 486, 491, 495, 499, 502, 506, 510, 514, 518, 522, 525, 528, 532, 536, 540, 544, 549, 553, 557, 561, 565, 569, 573, 576, 580, 584, 587, 590, 593, 598, 601, 605, 610, 615, 618, 622, 626, 630, 634, 637, 641, 645, 648, 651, 654};
XDATA WORD u16PWM;
#endif

#if ENABLE_HDR_DYNAMIC_FUNCTION
XDATA WORD HDRLuminanceMap[11];
#endif
 /******************************************************************************************************************************************
 * Free Sync2
 ******************************************************************************************************************************************/
static BOOL _IsEnFreeSync2 = FALSE;

#if ENABLE_HDR_DYNAMIC_FUNCTION
BYTE msGetLumMapBrightness(WORD u16Lum)
{
    XDATA BYTE idx = 0;
    XDATA BYTE idx_Min_Duty = 0;
    XDATA BYTE idx_Max_Duty = 0;

    #if HDR_10STEP_IS_VALID
    {
        if (u16Lum <= HDRLuminanceMap[0])
            return HDR_MIN_DUTY;

        for (idx = 0; idx<sizeof(HDRLuminanceMap)/(sizeof(BYTE)); idx++)
        {
            if(HDRLuminanceMap[idx] >= u16Lum)
            {
                idx_Min_Duty = HDR_MIN_DUTY+((idx-1)*(HDR_MAX_DUTY-HDR_MIN_DUTY)/10);
                idx_Max_Duty = HDR_MIN_DUTY+(idx*(HDR_MAX_DUTY-HDR_MIN_DUTY)/10);

                return (idx_Min_Duty + ((u16Lum-HDRLuminanceMap[idx-1])*(idx_Max_Duty-idx_Min_Duty) / (HDRLuminanceMap[idx]-HDRLuminanceMap[idx-1])));
            }
        }

        return HDR_MAX_DUTY;
    }
    #else
    {
        return (HDR_MIN_DUTY + ((u16Lum-HDR_LUMINANCE_TO_BRIGHTNESS_0)*(HDR_MAX_DUTY-HDR_MIN_DUTY) / (HDR_LUMINANCE_TO_BRIGHTNESS_100-HDR_LUMINANCE_TO_BRIGHTNESS_0)));
    }
    #endif
}
#endif

static void mdrv_HDR_Gamma_Preset(void)
 {
    code BYTE *DefaultHDRGammaTbl[][3] =
    {
        {
        tblHDRGammaR,
        tblHDRGammaG,
        tblHDRGammaB,
        }
    };

    msAPI_GammaLoadTbl_1024E_12B(MAIN_WINDOW, (BYTE**)DefaultHDRGammaTbl[0]);
 }

BOOL msSetHDREnable(BYTE u8WinIdx, BYTE enHDR)
{
#if ENABLE_ACHDR_FUNCTION
    XDATA StoredHDRPanelInfo sHDRPanelInfo;
#endif
    XDATA SMPTE2086Metadata md;


	//HDRComm_printMsg("---------- Enter msSetHDREnable ----------\n");

    //xdata ST_COMBO_COLOR_FORMAT cf;
    HDRComm_printData("HDR Window : %d\n",  u8WinIdx);
    HDRComm_printData("Set HDR Status : %d \n",  enHDR);
    msSetHDRDebugMsg(0);

    if (IsInitPanelInfo == FALSE && (enHDR == HDR_HDR10))
    {
        IsInitPanelInfo = TRUE;
#if ENABLE_ACHDR_FUNCTION
        if (msLoadACHDRPanelInfo(&sHDRPanelInfo))
        {
            msSetHDRPanelInfo(PANEL_MAX_LUMINANCE,//(double)sHDRPanelInfo.mHDRPanelInfo.maxLum / 10000.0,
                              PANEL_MIN_LUMINANCE,//(double)sHDRPanelInfo.mHDRPanelInfo.minLum / 10000.0,
                              (double)sHDRPanelInfo.mHDRPanelInfo.Rx     / 10000.0,
                              (double)sHDRPanelInfo.mHDRPanelInfo.Ry     / 10000.0,
                              (double)sHDRPanelInfo.mHDRPanelInfo.Gx     / 10000.0,
                              (double)sHDRPanelInfo.mHDRPanelInfo.Gy     / 10000.0,
                              (double)sHDRPanelInfo.mHDRPanelInfo.Bx     / 10000.0,
                              (double)sHDRPanelInfo.mHDRPanelInfo.By     / 10000.0,
                              (double)sHDRPanelInfo.mHDRPanelInfo.Wx     / 10000.0,
                              (double)sHDRPanelInfo.mHDRPanelInfo.Wy     / 10000.0);
        }
        else
#endif
        {
            msSetHDRPanelInfo(PANEL_MAX_LUMINANCE,
                              PANEL_MIN_LUMINANCE,
                              PANEL_GAMUT_Rx,
                              PANEL_GAMUT_Ry,
                              PANEL_GAMUT_Gx,
                              PANEL_GAMUT_Gy,
                              PANEL_GAMUT_Bx,
                              PANEL_GAMUT_By,
                              PANEL_GAMUT_Wx,
                              PANEL_GAMUT_Wy);
            mdrv_HDR_Gamma_Preset();
        }

    }

    md = msGetST2086MetadataByWin(u8WinIdx);
    if(enHDR != HDR_DEMO_HDR10)
    {
        HDRComm_printMsg(">>>>>>>>>>>>>>>HDR metadata<<<<<<<<<<<<<<");
        //HDRComm_printData("EOTF Type : %d\n", md.EOTF);
        //HDRComm_printData("Metadata description ID :  0x%x\n", md.MetadataDescID);
        HDRComm_printData("Rx : 0x%x\n", md.Rx);
        HDRComm_printData("Ry:  0x%x\n", md.Ry);
        HDRComm_printData("Gx :  0x%x\n", md.Gx);
        HDRComm_printData("Gy :  0x%x\n", md.Gy);
        HDRComm_printData("Bx : 0x%x\n", md.Bx);
        HDRComm_printData("By :  0x%x\n", md.By);
        HDRComm_printData("Wx :  0x%x\n", md.Wx);
        HDRComm_printData("Wy :  0x%x\n", md.Wy);
        HDRComm_printData("Mastering Display Max Luminace : 0x%x\n", md.MaxDispLum);
        HDRComm_printData("Mastering Display Min Luminace :  0x%x\n", md.MinDispLum);
        HDRComm_printData("Max Content Luminance Level :  0x%x\n", md.MaxCLL);
        HDRComm_printData("Frame Average Luminance Level :  0x%x\n", md.MaxFALL);
    }


    memset(&_hdrPref, 0x00, sizeof(_hdrPref));

    //color tracking
    _hdrPref.gmd.IsValid = TRUE;
    _hdrPref.gmd.IsTraceNativeWhitePoint = TRUE;

    msSetHDRUserPref(&_hdrPref);

    //Set Color Format
    SetHDRColorFormat(msAPI_combo_IPGetColorFormat(SrcInputType));

    //cf.ucColorType = COMBO_COLOR_FORMAT_RGB;
    //cf.ucColorRange = COMBO_COLOR_RANGE_FULL;
    //cf.ucYuvColorimetry = COMBO_YUV_COLORIMETRY_EC;
    //cf.ucColorimetry = COMBO_COLORIMETRY_BT2020RGBYCbCr;
    //msAPI_combo_IPGetHDRPacket(SrcInputType, );
    //SetHDRColorFormat(cf);

    //Set gamut mapping is referenced from AVI infoframe(1) or HDR infoframe(0)
    msSetGamutMappingType(1);

    mdrv_HDR_DS_Start();
    //msSetHDRSrcIsDisplayPort(u8WinIdx, FALSE);
    currHDRState = msHDRHandler( u8WinIdx,  (HDR_Level)enHDR);
    currHDRState = msGetHDRStatus(u8WinIdx);
    mdrv_HDR_DS_End();
    HDRComm_printData("Get HDR Status : %d \n",  currHDRState);
    if (currHDRState == HDR_OFF)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

#if ENABLE_HDR_AUTODETECT
void drvHDRVsyncUpdatePacket(BYTE u8WinIdx)
{
    if (PowerOnFlag && !SyncLossState() && !InputTimingChangeFlag && InputTimingStableFlag && !FreeRunModeFlag && !bRunToolFlag)
    {
        _currHdrMd = msGetST2086MetadataByWin(u8WinIdx);
    }
}

void msDrvHDRAutoDetect(BYTE u8WinIdx)
{
    if (PowerOnFlag && !SyncLossState() && !InputTimingChangeFlag && InputTimingStableFlag && !FreeRunModeFlag && !bRunToolFlag)
    {
#if (!COMBO_TMDS_PACKET_UPDATED_BY_VSYNC)
       _currHdrMd = msGetST2086MetadataByWin(u8WinIdx);
#endif
        if (((_currHdrMd.EOTF & 0x02) == 0x02) && (_currHdrMd.Length == 0x1A))
        {
            if (IsDiffMetadata(_lastHdrMd, _currHdrMd) || HDR_OFF == msGetHDRStatus(u8WinIdx))
            {
                HDRComm_printMsg("[msDrvHDRAutoDetect] Autodetect turn on HDR.\n");
                msSetHDREnable(u8WinIdx, HDR_HDR10);
            }
        }
        else if (HDR_OFF != msGetHDRStatus(u8WinIdx))
        {
            HDRComm_printMsg("[msDrvHDRAutoDetect] Autodetect turn off HDR.\n");
            msSetHDREnable(u8WinIdx, HDR_OFF);
        }

        _lastHdrMd = _currHdrMd;
    }
}
//Get/Set Metadata for HDR
void mdrv_HDR_SMPTE2086Metadata_Set(BYTE u8WinIdx, SMPTE2086Metadata metadata)
{
    UNUSED(u8WinIdx);
     _currHdrMd = metadata;
}

SMPTE2086Metadata mdrv_HDR_SMPTE2086Metadata_Get(BYTE u8WinIdx)
{
    UNUSED(u8WinIdx);
    return _currHdrMd;
}

BOOL mdrv_HDR_Metadata_Update(BYTE u8WinIdx)
{
    BOOL bIsPktReceived = FALSE;
    UNUSED(u8WinIdx);
    SMPTE2086Metadata metadata;
    BYTE header[4];
    BYTE packet[32];
    memset(header, 0x00, sizeof(header));
    memset(packet, 0x00, sizeof(packet));
    memset(&metadata, 0x00, sizeof(SMPTE2086Metadata));
    bIsPktReceived = msAPI_combo_IPGetHDRPacket(SrcInputType, header, packet);
    if ((INPUT_IS_DISPLAYPORT(SrcInputType) && bIsPktReceived)||(INPUT_IS_USBTYPEC(SrcInputType) && bIsPktReceived))
        {
       //HDR_PrintMsg("DP HDR Packet");
        metadata.Version = packet[0];
        metadata.Length = packet[1];
        //metadata.Checksum = packet[2];
        metadata.EOTF = packet[2];
        metadata.MetadataDescID = packet[3];
        metadata.Rx = (((WORD)packet[13]) << 8) + packet[12];
        metadata.Ry = (((WORD)packet[15]) << 8 ) + packet[14];
        metadata.Gx = (((WORD)packet[5])<< 8 ) + packet[4];
        metadata.Gy = (((WORD)packet[7]) << 8 ) + packet[6];
        metadata.Bx = (((WORD)packet[9]) << 8 ) + packet[8];
        metadata.By = (((WORD)packet[11]) << 8 ) + packet[10];
        metadata.Wx = (((WORD)packet[17]) << 8 ) + packet[16];
        metadata.Wy =(((WORD)packet[19]) << 8 ) + packet[18];
        metadata.MaxDispLum = (((WORD)packet[21]) << 8 ) + packet[20];
        metadata.MinDispLum = (((WORD)packet[23]) << 8 ) + packet[22];
        metadata.MaxCLL = (((WORD)packet[25]) << 8 ) + packet[24];
        metadata.MaxFALL = (((WORD)packet[27]) << 8 ) + packet[26];
    }
    else if(INPUT_IS_HDMI(SrcInputType)&& bIsPktReceived)
    {
        //HDMI
        metadata.Version = packet[0];
        metadata.Length = packet[1];
        metadata.Checksum = packet[2];
        metadata.EOTF = packet[3];
        metadata.MetadataDescID = packet[4];
        metadata.Rx = (((WORD)packet[6]) << 8) + packet[5];
        metadata.Ry = (((WORD)packet[8]) << 8 ) + packet[7];
        metadata.Gx = (((WORD)packet[10])<< 8 ) + packet[9];
        metadata.Gy = (((WORD)packet[12]) << 8 ) + packet[11];
        metadata.Bx = (((WORD)packet[14]) << 8 ) + packet[13];
        metadata.By = (((WORD)packet[16]) << 8 ) + packet[15];
        metadata.Wx = (((WORD)packet[18]) << 8 ) + packet[17];
        metadata.Wy =(((WORD)packet[20]) << 8 ) + packet[19];
        metadata.MaxDispLum = (((WORD)packet[22]) << 8 ) + packet[21];
        metadata.MinDispLum = (((WORD)packet[24]) << 8 ) + packet[23];
        metadata.MaxCLL = (((WORD)packet[26]) << 8 ) + packet[25];
        metadata.MaxFALL = (((WORD)packet[28]) << 8 ) + packet[27];
    }
    mdrv_HDR_SMPTE2086Metadata_Set(u8WinIdx, metadata);
    return bIsPktReceived;
}

BOOL mdrv_HDR_PktSPDNativeColorActive_Get(BYTE u8WinIdx)
{
    BYTE packet[8];
    EN_PACKET_DEFINE_TYPE pktType = COMBO_PACKET_SPD_INFOFRAME;
    BOOL bIsPktReceived = FALSE;
    UNUSED(u8WinIdx);
    msAPI_combo_IPGetPacketContent(SrcInputType, pktType, sizeof(packet), packet);

    if (bIsPktReceived == FALSE)
    {
        return FALSE;
    }

    if (INPUT_IS_DISPLAYPORT(SrcInputType)||INPUT_IS_USBTYPEC(SrcInputType))
    {
        if((packet[1] == 0x1A) && (packet[2] == 0x00) && (packet[3] == 0x00))
        {
            return (packet[6]&BIT3)?TRUE:FALSE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        if((packet[0] == 0x1A) && (packet[1] == 0x00) && (packet[2] == 0x00))
        {
            return (packet[5]&BIT3)?TRUE:FALSE;
        }
        else
        {
            return FALSE;
        }
    }
}

BOOL mdrv_HDR_IsDetectedHDR10Input_Get(BYTE u8WinIdx)
{
    SMPTE2086Metadata md  =  mdrv_HDR_SMPTE2086Metadata_Get(u8WinIdx);
    if (((md.EOTF & 0x02) == 0x02) &&(md.Length == 0x1A))
    {
        return TRUE;
    }
    return FALSE;
}

/******************************************************************************************************************************************
 * Free Sync2
 ******************************************************************************************************************************************/
void mapi_HDR_FreeSync2Enable_Set(BOOL bSupport)
{
    _IsEnFreeSync2 = bSupport;
}

BOOL mapi_HDR_FreeSync2Enable_Get(void)
{
    return _IsEnFreeSync2;
}

static BOOL mdrv_HDR_MaxCLL_IsDiff(SMPTE2086Metadata m1, SMPTE2086Metadata m2)
{
    if ((m1.MaxDispLum != m2.MaxDispLum)||
        (m1.MaxCLL!= m2.MaxCLL))
    {
        return true;
    }
    return false;
}

void mdrv_HDR_AutoDetect_Handler(void)
{
    BYTE u8WinIdx = MAIN_WINDOW;
    if ( !SyncLossState() && InputTimingStableFlag && !FreeRunModeFlag && !bRunToolFlag)
    {
        //The actual metadata doen't be referanced in case  of HDR demo mode.
        if (HDR_DEMO_HDR10 == msGetHDRStatus(u8WinIdx))
        {
            return;
        }

        if ((mdrv_HDR_Metadata_Update(u8WinIdx) == FALSE) && (HDR_OFF != msGetHDRStatus(u8WinIdx)))
        {
            HDRComm_printMsg("[msDrvHDRAutoDetect] Autodetect turn off HDR(0).\n");
            msSetHDREnable(u8WinIdx, HDR_OFF);
            return;
        }

        SMPTE2086Metadata md =  mdrv_HDR_SMPTE2086Metadata_Get(u8WinIdx);
        if (mapi_HDR_FreeSync2Enable_Get() && mdrv_HDR_PktSPDNativeColorActive_Get(u8WinIdx) && HDR_FREESYNC_PRO!= msGetHDRStatus(u8WinIdx))
        {
            HDRComm_printMsg("[msDrvHDRAutoDetect] FreeSync2 Native Color (HDR_FREESYNC_PRO).\n");
            msSetHDREnable(u8WinIdx, HDR_FREESYNC_PRO);

             //HDR Gamma 2.2 + D65
            #if ENABLE_ACHDR_FUNCTION
            #if 0
            if (HDR_USE_PANEL_GAMMA == mdrv_HDR_GammaCorrectionPos_Get())
            {
                 msReloadPanelGammaData(GetAddrOfHDRPostGamma());
            }
            else if  (HDR_USE_POST_GAMMA == mdrv_HDR_GammaCorrectionPos_Get())
            {
                msReloadGammaData(GetAddrOfHDRPostGamma());
            }
            #endif
            #endif
        }
        #if 0
        else if(mapi_HDR_IsDetectedHLGInput_Get(u8WinIdx))
        {
            if (IsDiffMetadata(LastMetaData[u8WinIdx], md) || HDR_HLG != msGetHDRStatus(u8WinIdx))
            {
                HDRComm_printMsg("[msDrvHDRAutoDetect] Autodetect turn on HLG.\n");
                msSetHDREnable(u8WinIdx, HDR_HLG);
            }
        }
        #endif
        else if (mdrv_HDR_IsDetectedHDR10Input_Get(u8WinIdx))
        {
            if (mdrv_HDR_MaxCLL_IsDiff(_lastHdrMd, md) || HDR_HDR10 != msGetHDRStatus(u8WinIdx))
            {
                HDRComm_printMsg("[msDrvHDRAutoDetect] Autodetect turn on HDR.\n");
                msSetHDREnable(u8WinIdx, HDR_HDR10);
            }
        }
        #if 0
        else if (USER_PREF_DS_HLG==eMOPT_HLG_ON)
        {
            if (HDR_DEMO_HLG != msGetHDRStatus(u8WinIdx))
            {
                HDRComm_printMsg("[msDrvHDRAutoDetect] Autodetect turn on HLG DEMO.\n");
                mapp_ColorManage_HDREnable_Set(u8WinIdx, HDR_DEMO_HLG);
            }
        }
        #endif
        else if (HDR_OFF != msGetHDRStatus(u8WinIdx))
        {
            HDRComm_printMsg("[msDrvHDRAutoDetect] Autodetect turn off HDR(1).\n");
            msSetHDREnable(u8WinIdx, HDR_OFF);
        }
        _lastHdrMd = md;
    }
    else
    {
        if (HDR_OFF != msGetHDRStatus(u8WinIdx))
        {
            HDRComm_printMsg("[msDrvHDRAutoDetect] Autodetect turn off HDR(2).\n");
            msSetHDREnable(u8WinIdx, HDR_OFF);
        }
    }
}

void AutodetectHDR_Handler(void)
{
    #if ENABLE_HDR_AUTODETECT
        //msDrvHDRAutoDetect(MAIN_WINDOW);
        mdrv_HDR_AutoDetect_Handler();
    #endif
}
#endif


#if HDR_TBD_Segment
WORD msGetLumMapPWM(WORD u16Lum)
{
    XDATA WORD idx = 0;
    if (u16Lum < LumMap[0])
        return u16MinDuty;

    for (idx = 0; idx < sizeof(LumMap)/(sizeof(WORD)); idx++)
    {
        if (LumMap[idx] >= u16Lum)
        {
            return (idx + u16MinDuty);
        }
    }
    return u16MaxDuty;
}
#endif

static int _numOfDrvDSCmds = 0;
static void  _mdrv_HDR_DSRegInfo_Get(DWORD u32Addr, BOOL *canUpdateByDS, BOOL *IsRegAtIP)
{
    BYTE u8BankNum =  u32Addr >> 24;
    BYTE u8Addr = u32Addr & 0xFF;
    *IsRegAtIP = FALSE;

    switch(u8BankNum)
    {
        case 0x0B:
        case 0x22:
        case 0x67:
        case 0x79:
        case 0x7A:
        case 0x7B:
        case 0x7C:
        case 0x7D:
        case 0x7E:
        {
            *IsRegAtIP = TRUE;
        }break;
    }

    *canUpdateByDS = FALSE;
    switch(u8BankNum)
    {
        case 0x22:
        case 0x0F:
        case 0x10:
        case 0x24:
        case 0x25:
        case 0x4C:
        case 0x67:
        case 0x79:
        case 0x7A:
        case 0x7B:
        //case 0x7C:
        case 0x7E:
        {
            *canUpdateByDS = TRUE;
        }break;
    }

    //XVYCC ADL settigns can referance to sc mask out.
    if (u8BankNum == 0x67)
    {
        if ((u8Addr >= 0x22) && (u8Addr <= 0x40))//client4/5/6
        {
             IsRegAtIP = FALSE;
        }
    }
    UNUSED(u8Addr);
}

void mdrv_HDR_DS_Start(void)
{
    DS_START();
}

void mdrv_HDR_DS_End(void)
{
    int nDS_Start = MsOS_GetSystemTime();
    DS_END();
    int nDS_END = MsOS_GetSystemTime();
    HDRComm_printData("msDrvDS_End, Num Of Cmds: %d\n", _numOfDrvDSCmds);
    HDRComm_printData("CFD_DS_Fire  DS take = %d\n", nDS_END - nDS_Start);
    _numOfDrvDSCmds = 0;
}

void mdrv_HDR_DS_W2BMask(DWORD u32Address, WORD u16Data, WORD u16Mask)
{
    static BOOL canUpdateByDS = FALSE;
    static BOOL IsRegAtIP = FALSE;
    _mdrv_HDR_DSRegInfo_Get(u32Address, &canUpdateByDS, &IsRegAtIP);

    if (TRUE == canUpdateByDS)
    {
        if (TRUE == IsRegAtIP)
        {
            DS_W2BMask(u32Address, u16Data, u16Mask, DS_IP);
        }
        else
        {
            DS_W2BMask(u32Address, u16Data, u16Mask, DS_OP);
        }
        _numOfDrvDSCmds++;
    }
    else
    {
        msWrite2ByteMask(u32Address, u16Data, u16Mask);
    }
}

void mdrv_HDR_DS_WBMask(DWORD u32Address, BYTE u8Data, BYTE u8Mask)
{
    static BOOL canUpdateByDS = FALSE;
    static BOOL IsRegAtIP = FALSE;
    _mdrv_HDR_DSRegInfo_Get(u32Address, &canUpdateByDS, &IsRegAtIP);

    if (TRUE == canUpdateByDS)
    {
        if (TRUE == IsRegAtIP)
        {
            DS_WBMask(u32Address, u8Data, u8Mask, DS_IP);
        }
        else
        {
            DS_WBMask(u32Address, u8Data, u8Mask, DS_OP);
        }
        _numOfDrvDSCmds++;
    }
    else
    {
        msWriteByteMask(u32Address, u8Data, u8Mask);
    }

}

void mdrv_HDR_DS_W4B(DWORD u32Address, DWORD u32Value)
{
    static BOOL canUpdateByDS = FALSE;
    static BOOL IsRegAtIP = FALSE;
    _mdrv_HDR_DSRegInfo_Get(u32Address, &canUpdateByDS, &IsRegAtIP);

    if (TRUE == canUpdateByDS)
    {
        if (TRUE == IsRegAtIP)
        {
            DS_W4B(u32Address, u32Value, DS_IP);
        }
        else
        {
            DS_W2B(u32Address, u32Value, DS_OP);
        }
        _numOfDrvDSCmds++;
    }
    else
    {
        msWrite4Byte(u32Address, u32Value);
    }

}

void mdrv_HDR_DS_W2B(DWORD u32Address, WORD u16Data)
{
    static BOOL canUpdateByDS = FALSE;
    static BOOL IsRegAtIP = FALSE;
    _mdrv_HDR_DSRegInfo_Get(u32Address, &canUpdateByDS, &IsRegAtIP);

    if (TRUE == canUpdateByDS)
    {
        if (TRUE == IsRegAtIP)
        {
            DS_W2B(u32Address, u16Data, DS_IP);
        }
        else
        {
            DS_W2B(u32Address, u16Data, DS_OP);
        }
        _numOfDrvDSCmds++;
    }
    else
    {
        msWrite2Byte(u32Address, u16Data);
    }

}

void mdrv_HDR_DS_WB(DWORD u32Address, BYTE u8Data)
{
    static BOOL canUpdateByDS = FALSE;
    static BOOL IsRegAtIP = FALSE;
    _mdrv_HDR_DSRegInfo_Get(u32Address, &canUpdateByDS, &IsRegAtIP);

    if (TRUE == canUpdateByDS)
    {
        if (TRUE == IsRegAtIP)
        {
            DS_WB(u32Address, u8Data, DS_IP);
        }
        else
        {
            DS_WB(u32Address, u8Data, DS_OP);
        }
        _numOfDrvDSCmds++;
    }
    else
    {
        msWriteByte(u32Address, u8Data);
    }

}

void mdrv_HDR_DS_WBit(DWORD u32Address, Bool bBit, BYTE u8BitPos)
{
    static BOOL canUpdateByDS = FALSE;
    static BOOL IsRegAtIP = FALSE;
    _mdrv_HDR_DSRegInfo_Get(u32Address, &canUpdateByDS, &IsRegAtIP);

    if (TRUE == canUpdateByDS)
    {
        if (TRUE == IsRegAtIP)
        {
            DS_WBit(u32Address, bBit, u8BitPos, DS_IP);
        }
        else
        {
            DS_WBit(u32Address, bBit, u8BitPos, DS_OP);
        }
        _numOfDrvDSCmds++;
    }
    else
    {
        msWriteBit(u32Address, bBit, u8BitPos);
    }

}

//#endif

#if (ENABLE_DOLBY_HDR)
BYTE ucDVPQR2EventFlag = 0;
#define DOLBY_IDK_VERIFICATION 0
static int g_DumpFrame = 0;
void mdrv_DolbyIDK_USB_Dump(BOOL bIsPixelOrder, BYTE u8ColorFormat, BYTE startIdx, BYTE endIdx);
// DolbyHandShake struct, Need to sync w/ PQ_R2
#define DolbyHandShakeVersion   "0.0.1"
typedef struct{
#if DOLBY_IDK_VERIFICATION
    BOOL bIsPixelOrder;
    BYTE u8ColorFormat;
    BYTE u8StartIdx;
    BYTE u8EndIdx;
    BOOL bStartToDumpUSB;
#else

#endif
}Dolby_PQR2_Nofity_Data;

typedef struct{
    DWORD u32Dolby_ADL_ADDR;
#if DOLBY_IDK_VERIFICATION
    BYTE u8DumpFrame;
#else
    BYTE ucPacketLength;
    BYTE *pPacketData;
#endif
}Dolby_HKR2_Nofity_Data;
// --- Need to sync w/ PQ_R2 end
typedef enum
{
    EN_DOLBY_STATE_INIT = 0,
    EN_DOLBY_STATE_IDLE,
    EN_DOLBY_STATE_WAITING,
    EN_DOLBY_STATE_PROCESSING,
    EN_DOLBY_STATE_HALT,
    EN_DOLBY_STATE_DONE
} ENUM_DOLY_STATE_LIST;

void mdrv_Dolby_HKR2_Messaging(ENUM_DOLY_STATE_LIST enState)
{
    static Dolby_HKR2_Nofity_Data *pDolby_HKR2_Nofity_Data = (Dolby_HKR2_Nofity_Data*)(MIU_DOLBY_INFO_ADDR_START | 0x80000000); // noncache
if(enState == EN_DOLBY_STATE_INIT){
    pDolby_HKR2_Nofity_Data->u32Dolby_ADL_ADDR = DV_ADL_ADDR;
    pDolby_HKR2_Nofity_Data->u8DumpFrame = 0;
}
if(enState == EN_DOLBY_STATE_PROCESSING && g_DumpFrame > 0)
    pDolby_HKR2_Nofity_Data->u8DumpFrame = g_DumpFrame;

    HDRComm_printMsg("HKR2 trigger INT to PQR2\n");
    msWriteByte(REG_100540, 0);
    msWriteByte(REG_100540, BIT1);
}

void mdrv_Dolby_Addr_Init(void)
{
    printf("[%s]MBX_DV_BUF_START:0x%x, MIU_DOLBY_INFO_ADDR_START:0x%x\n", __FUNCTION__, MBX_DV_BUF_START, MIU_DOLBY_INFO_ADDR_START);
    msWrite4Byte(MBX_DV_BUF_START, MIU_DOLBY_INFO_ADDR_START);
#if DOLBY_IDK_VERIFICATION
    mdrv_Dolby_HKR2_Messaging(EN_DOLBY_STATE_INIT);
#endif
}

Dolby_PQR2_Nofity_Data* mdrv_Dolby_PQR2_Data_Read(DWORD u32MemAddr)
{
    static Dolby_PQR2_Nofity_Data *pDolby_PQR2_Nofity_Data;
    pDolby_PQR2_Nofity_Data = (Dolby_PQR2_Nofity_Data*)(u32MemAddr | 0x80000000); // noncache
    return pDolby_PQR2_Nofity_Data;
}

void mdrv_Dolby_SetDVPQR2EventFlag(void)
{
    ucDVPQR2EventFlag = 1;
    //mdrv_Dolby_HKR2_Receive();
}

void DVPQR2Event_Handler(void)
{
    //printf("DVPQR2Event_Handler\n");
    if(ucDVPQR2EventFlag)
    {
        ucDVPQR2EventFlag = 0;
        mdrv_Dolby_HKR2_Receive();
    }
}

void mdrv_Dolby_HKR2_Receive(void)
{
    printf("%s()\n", __FUNCTION__);
    Dolby_PQR2_Nofity_Data *pDolby_PQR2_Nofity_Data = mdrv_Dolby_PQR2_Data_Read(MIU_DOLBY_INFO_ADDR_START+sizeof(Dolby_HKR2_Nofity_Data));
#if DOLBY_IDK_VERIFICATION
    if(pDolby_PQR2_Nofity_Data->bStartToDumpUSB){
        mdrv_DolbyIDK_IP2_Bypass();
        //bypass core2 for debug purpose
        //msWriteBit(SC39_03, TRUE, BIT3);
        mdrv_DolbyIDK_USB_Dump(pDolby_PQR2_Nofity_Data->bIsPixelOrder, pDolby_PQR2_Nofity_Data->u8ColorFormat, pDolby_PQR2_Nofity_Data->u8StartIdx, pDolby_PQR2_Nofity_Data->u8EndIdx);
        pDolby_PQR2_Nofity_Data->bStartToDumpUSB = false;
    }
#else
#endif
}

void mdrv_DolbyIDK_IP2_Bypass(void)
{
    //hdmi420to444 disable
//    msWriteBit(SC22_81, FALSE, BIT7);
    //hdmi422_pack force
    msWriteBit(SC22_1E, FALSE, BIT4);
    msWriteBit(SC22_1E, TRUE, BIT5);
    //422to444
    msWriteByteMask(SC22_1F, 0, BIT4|BIT5);
    //HDR10
//    msWriteBit(SC79_0E, FALSE, BIT1);
    //csc
    msWriteBit(SC22_56, FALSE, BIT4);
    msWriteBit(SC79_06, TRUE, BIT6);
    msWriteBit(SC79_06, FALSE, BIT7);
    msWriteBit(SC79_4E, TRUE, BIT0);
}

void mdrv_DolbyIDK_USB_Dump(BOOL bIsPixelOrder, BYTE u8ColorFormat, BYTE startIdx, BYTE endIdx)
{
    BYTE i = 0;
    int totalTime = 0;
    static BYTE u8SkipDump = 0x0;
    static BYTE u8DumpMode = 0x1; //0: auto mode, 1: manual mode
    static BYTE u8DumpDelayTime = 0x1E;
    //true: PixelOrder; false: PlanarOrder
    //COMBO_COLOR_FORMAT_RGB         = 0,     ///< HDMI RGB 444 Color Format
    //COMBO_COLOR_FORMAT_YUV_422     = 1,     ///< HDMI YUV 422 Color Format
    //COMBO_COLOR_FORMAT_YUV_444     = 2,     ///< HDMI YUV 444 Color Format
    //COMBO_COLOR_FORMAT_YUV_420     = 3,     ///< HDMI YUV 420 Color Format

    printf("[mStar_FrameBufferDump]bIsPixelOrder:%x, u8ColorFormat:%x, startIdx:%x, endIdx:%x\n", bIsPixelOrder, u8ColorFormat, startIdx, endIdx);
    extern xdata SetupPathInfo g_SetupPathInfo;
    BYTE ucBackupMemFormat = g_SetupPathInfo.ucMemFormat;
    printf("g_SetupPathInfo.ucMemFormat:%x\n", g_SetupPathInfo.ucMemFormat);
    g_SetupPathInfo.ucMemFormat = MEM_FMT_444_10;
    extern Bool mStar_SetupMode( void );
    mStar_SetupMode();
    ForceDelay1ms(50);
    //mdrv_DolbyIDK_IP2_Bypass();
    printf("mdrv_DolbyIDK_IP2_Bypass Done.\n");
    extern BOOL mStar_FrameBufferDump(DWORD u32DestAddress, size_t u32DestSize, BOOL bIsPixelOrder, BYTE u8ColorFormat);
    extern BOOLEAN Dump_DRAM_To_USB(U32 u32StartAddr, U32 u32Length);
    extern void Dump_DRAM_To_USB_FileWriteCnt_Set(U16 u16FileWriteCnt);
    printf("%s()Start to Dump ...\n", __FUNCTION__);
    g_DumpFrame = 0;
    for (i = startIdx; i <= endIdx; i++)
    {
        int startTime = 0, endTime = 0, endTime1 = 0;
        startTime = MsOS_GetSystemTime();
        printf("mdrv_DolbyIDK_USB_Dump Frame: %d \n", i);
        mdrv_Dolby_HKR2_Messaging(EN_DOLBY_STATE_PROCESSING);
        MsOS_DelayTask(1000);
        if (!u8SkipDump)
        {
            mStar_FrameBufferDump(MIU_DOLBY_DUMP_ADDR_START, MIU_DOLBY_DUMP_SIZE, bIsPixelOrder, u8ColorFormat);

            Dump_DRAM_To_USB_FileWriteCnt_Set(i);
            if(u8ColorFormat == 0)//444 12bits to YUV444 12
                Dump_DRAM_To_USB(MIU_DOLBY_DUMP_ADDR_START, (1920*1080*6)); //(1920*1080*3*2)
            else
                Dump_DRAM_To_USB(MIU_DOLBY_DUMP_ADDR_START, (1920*1080*4)); //(1920*1080*3*2)
            g_DumpFrame++;
            printf("Dolby vision Dump frame %d / %d complete.\n", i, endIdx);
        }

        endTime = MsOS_GetSystemTime();

        if (i + 1 <= endIdx)
        {
            if (u8DumpMode == 0)
            {
                HDRComm_printMsg("\n\n");
                HDRComm_printMsg("#############################\n");
                HDRComm_printData("#    Wait input frame %ds.   #\n", u8DumpDelayTime);
                HDRComm_printMsg("#############################\n");
                HDRComm_printMsg("\n\n");
                MsOS_DelayTask(u8DumpDelayTime * 1000);
            }
            else if (u8DumpMode == 1)
            {
                int u32DumpWasteTime = (endTime - startTime);
                if (u32DumpWasteTime > u8DumpDelayTime * 1000)
                {
                    HDRComm_printMsg("Warning: add dump delay time\n");
                }

                HDRComm_printMsg("\n\n");
                HDRComm_printMsg("#############################\n");
                HDRComm_printData("#  Wait input frame %04dms.  #\n", ((i - startIdx + 1) * u8DumpDelayTime * 1000 - (u32DumpWasteTime + totalTime)));
                HDRComm_printMsg("#############################\n");
                HDRComm_printMsg("\n\n");
                MsOS_DelayTask(((i - startIdx + 1) * u8DumpDelayTime * 1000 - (u32DumpWasteTime + totalTime)));
            }
        }
        else
        {
            HDRComm_printData("Dolby vision Dump all frames (%d) complete.\n", (endIdx - startIdx + 1));
            mdrv_Dolby_HKR2_Messaging(EN_DOLBY_STATE_PROCESSING);
        }
        endTime1 = MsOS_GetSystemTime();
        totalTime += (endTime1 - startTime);

        printf("*******input frame %d wasting time %d ms.******\n", i, (endTime1 - startTime));
    }
    g_SetupPathInfo.ucMemFormat = ucBackupMemFormat;
    mStar_SetupMode();
}
#endif

