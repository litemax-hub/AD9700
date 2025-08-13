#include "board.h"

#if ENABLE_HDR

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
#include "HDR_GammaTable_Default.h"

#define HDRComm_debug 1
#define HDR_TBD_Segment 0

extern void SetHDRColorFormat(ST_COMBO_COLOR_FORMAT cf);

#if (ENABLE_DEBUG && HDRComm_debug)
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
xdata SMPTE2086Metadata _hdrMd;
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

    msAPI_GammaLoadTbl_256E_14B(MAIN_WINDOW, (BYTE**)DefaultHDRGammaTbl[0]);
    msAPI_GammaEnable(MAIN_WINDOW, 1);
}

BOOL msSetHDREnable(BYTE u8WinIdx, BYTE enHDR)
{
#if ENABLE_ACHDR_FUNCTION
    XDATA StoredHDRPanelInfo sHDRPanelInfo;
#endif
#if ENABLE_HDR_DYNAMIC_FUNCTION
    XDATA float gainEOTF;
    XDATA BYTE HDRBrightness;
#endif
    XDATA SMPTE2086Metadata md;


	//HDRComm_printMsg("---------- Enter msSetHDREnable ----------\n");

    //xdata ST_COMBO_COLOR_FORMAT cf;
    HDRComm_printData("HDR Window : %d\n",  u8WinIdx);
    HDRComm_printData("Set HDR Status : %d \n",  enHDR);
    msSetHDRDebugMsg(0);

    if (IsInitPanelInfo == FALSE && (enHDR == HDR_LOW))
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
            StoredFormatOfPostGamma StoredPostGamma;
            msLoadACHDRGamma(&StoredPostGamma);
            msSetHDRPostGamma((BYTE *)(&StoredPostGamma.Data[0][0]));

            StoredFormatOfHDRColorTemp StoredHDRColorTemp;
            msLoadACHDRColorTemp(&StoredHDRColorTemp);
            msSetHDRPanelColorTemp((BYTE *)(&StoredHDRColorTemp.mHDRColorTemp));
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
        //msSetHDRADLInfo(AUTOLOAD_HDR_ADR, AUTOLOAD_HDR_LEN, MS_TOOL_ADR, MS_TOOL_LEN);

        #if ENABLE_HDR_DYNAMIC_FUNCTION
        {
            #if ENABLE_ACHDR_FUNCTION
            StoredHDRLumMap sHDRLumMap;
            if (msLoadACHDRLumMap(&sHDRLumMap))
            {
                memcpy(HDRLuminanceMap, &sHDRLumMap.mHDRLumMap, sizeof(sHDRLumMap.mHDRLumMap));
            }
            else
            #endif
            {
                // Default LumMap[] for offline calibration
                WORD LumMap[] = {89, 144, 206, 265, 326, 384, 438, 495, 549, 601, 654};
                memcpy(HDRLuminanceMap, &LumMap, sizeof(LumMap));
            }
        }
        #endif
    }

    HDRComm_printMsg(">>>>>>>>>>>>>>>HDR metadata<<<<<<<<<<<<<<");
    md = msGetST2086MetadataByWin(u8WinIdx);
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


    memset(&_hdrPref, 0x00, sizeof(_hdrPref));

    //color tracking
    _hdrPref.gmd.IsValid = TRUE;
    _hdrPref.gmd.IsTraceNativeWhitePoint = TRUE;

    //D65 color temp gain
    _hdrPref.hdrColorTemp.IsValid = FALSE;

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


    //Dynamic backlight and EOTF control
    if (enHDR != (int)HDR_OFF)
    {
        msSetHDREOTFGain(u8WinIdx, 1.0);
        #if ENABLE_HDR_DYNAMIC_FUNCTION
        {
            if (enHDR == (int)HDR_DEMO)
            {
                md = GetDemoMetadata();
            }
            else
            {
                md = msGetST2086MetadataByWin(u8WinIdx);
            }

            if (md.MaxCLL < md.MaxDispLum)
                md.MaxCLL = PANEL_MAX_LUMINANCE;

            if (md.MaxCLL <= PCHDR_TIER)
            {
                gainEOTF = PANEL_MAX_LUMINANCE / (float)md.MaxCLL;

                if(md.MaxCLL < HDR_LUMINANCE_TO_BRIGHTNESS_0)
                {
                    HDRBrightness = HDR_MIN_DUTY;
                }else if(md.MaxCLL > HDR_LUMINANCE_TO_BRIGHTNESS_100)
                {
                    HDRBrightness = HDR_MAX_DUTY;
                }else
                {
                    HDRBrightness = msGetLumMapBrightness(md.MaxCLL);
                }
                #if ENABLE_LOCALDIMMING
                msSetLDMinMax(HDR_MIN_DUTY, HDRBrightness);
                #else
                mStar_AdjustBrightness(HDRBrightness);
                #endif

            }
            else
            {
                gainEOTF = 1.0;

                #if ENABLE_LOCALDIMMING
                msSetLDMinMax(HDR_MIN_DUTY, HDR_MAX_DUTY);
                #else
                mStar_AdjustBrightness(HDR_MAX_DUTY);
                #endif
            }
            msSetHDREOTFGain(u8WinIdx, gainEOTF);
        }
        #endif
    }


    //msSetHDRSrcIsDisplayPort(u8WinIdx, FALSE);
    currHDRState = msHDRHandler( u8WinIdx,  (HDR_Level)enHDR);
    currHDRState = msGetHDRStatus(u8WinIdx);
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
void msDrvHDRAutoDetect(BYTE u8WinIdx)
{
    if (PowerOnFlag && !SyncLossState() && !InputTimingChangeFlag && InputTimingStableFlag && !FreeRunModeFlag && !bRunToolFlag)   
    {
        _currHdrMd = msGetST2086MetadataByWin(u8WinIdx);

        if (((_currHdrMd.EOTF & 0x02) == 0x02) && (_currHdrMd.Length == 0x1A))
        {
            if (IsDiffMetadata(_lastHdrMd, _currHdrMd) || HDR_OFF == msGetHDRStatus(u8WinIdx))
            {
                HDRComm_printMsg("[msDrvHDRAutoDetect] Autodetect turn on HDR.\n");
                msSetHDREnable(u8WinIdx, HDR_LOW);
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

void AutodetectHDR_Handler(void)
{
    #if ENABLE_HDR_AUTODETECT
    msDrvHDRAutoDetect(MAIN_WINDOW);
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

#endif

