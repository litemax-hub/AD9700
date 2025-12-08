///////////////////////////////////////////////////////////////////////////////
/// @file CustomEDID.c
/// @brief Functions for customize EDID content related feature
/// @author Mediatek Inc.
///
///
///
/// Features
///  - Internal EDID loading flow
///  - 
///
///////////////////////////////////////////////////////////////////////////////

#include "types.h"
#include "Common.h"
#include "Global.h"
#include "Utl.h"

#if ENABLE_DP_INPUT
#include "mapi_DPRx.h"
#include "dpCommon.h"
#endif

#if ENABLE_HDMI
//#include "mapi_hdmiRx.h"
#include "mdrv_hdmiRx.h"
#endif

#include "EDIDTable.h"
#include "CustomEDID.h"

#define CUSTOM_EDID_DEBUG    1
#if ENABLE_MSTV_UART_DEBUG && CUSTOM_EDID_DEBUG
#define CUSTOM_EDID_printData(str, value)   printData(str, value)
#define CUSTOM_EDID_printMsg(str)           printMsg(str)
#else
#define CUSTOM_EDID_printData(str, value)
#define CUSTOM_EDID_printMsg(str)
#endif

const BYTE gu8DefaultIdSerialNum[DEFAULT_ID_SERIAL_NUM_LENGTH] =           {0x78, 0x56, 0x34, 0x12};
const BYTE gu8DefaultProductSerialNum[DEFAULT_PRODUCT_SERIAL_NUM_LENGTH] = {0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30};

static BYTE _g_u8CustomProductionWeek = 0xFF;
static BYTE _g_u8CustomProductionYear = 0xFF;
static EDID_SERIAL_NUM_INFO _g_CustomSNInfo;

/// @brief 
/// @param pu8Src 
/// @param eDescriptorType 
/// @return descriptor address, if no such descriptor type be found, return 0 
static BYTE _CustomEDID_DescriptorFinder(BYTE* pu8Src, EDID_DESCRIPTOR_TYPE eDescriptorType)
{
    BYTE u8Idx=EDID_DISP_DESCRIPTOR_ADDR_1;    //check 0x36, 0x48, 0x5A, 0x6C (Display descriptor)

    if(pu8Src == NULL)
    {
        return 0x00;
    }

    for(;;u8Idx+=EDID_DISP_DESCRIPTOR_SIZE)
    {
        //Find continuous "00 00 00 XX 00"
        if(*(pu8Src+u8Idx) == 0x00)
        {
            if(*(pu8Src+u8Idx+1) == 0x00)
            {
                if(*(pu8Src+u8Idx+2) == 0x00)
                {
                    if(*(pu8Src+u8Idx+3) == eDescriptorType)
                    {
                        if(*(pu8Src+u8Idx+4) == 0x00)
                        {
                            //printf("Find product name: 0x%x\n", u8Idx);
                            return u8Idx;
                        }
                    }
                }
            }
        }
        if(u8Idx >= EDID_DISP_DESCRIPTOR_ADDR_4)
            break;
    }
    //printf("Can't find product name: 0x%x\n", u8Idx);
    return 0x00;
}

/// @brief Set Custom production week, it need to be set before system loading EDID
/// @param u8ProductionWeek 
void CustomEDID_SetProductionWeek(BYTE u8ProductionWeek)
{
    _g_u8CustomProductionWeek = u8ProductionWeek;
}

/// @brief Set Custom production year, it need to be set before system loading EDID
/// @param u8ProductionYear 
void CustomEDID_SetProductionYear(BYTE u8ProductionYear)
{
    _g_u8CustomProductionYear = u8ProductionYear+1990;
}

/// @brief 
/// @param  
/// @return cutsom production week
BYTE CustomEDID_GetProductionWeek(void)
{
    BYTE u8ProductionWeek = DEFAULT_PRODUCTION_WEEK;

    if((_g_u8CustomProductionWeek>=1) && (_g_u8CustomProductionWeek<=54))//week range 1~54
    {
        u8ProductionWeek = _g_u8CustomProductionWeek;
    }

    return u8ProductionWeek;
}

/// @brief 
/// @param  
/// @return cutsom production year
BYTE CustomEDID_GetProductionYear(void)
{
    return _g_u8CustomProductionYear;
}

/// @brief Set custom SN, it need to be set before system loading EDID
/// @param pIDSerialNum 
/// @param pProductSerialNum 
/// @param u8ProductSerialNumLength 
void CustomEDID_SetCustomSN(BYTE *pIDSerialNum, BYTE *pProductSerialNum, BYTE u8ProductSerialNumLength)
{

   if(u8ProductSerialNumLength>13)
   {
        u8ProductSerialNumLength = 13;
        CUSTOM_EDID_printMsg("<WARNING>u8ProductSerialNumLength over 13 bytes\r\n");
   }

    if(pIDSerialNum != NULL)
    {
        memcpy(_g_CustomSNInfo.u8IDSerialNum, pIDSerialNum, 4);
    }
    if(pProductSerialNum != NULL)
    {
        memcpy(_g_CustomSNInfo.u8ProductSerialNum, pProductSerialNum, u8ProductSerialNumLength);
    }
    _g_CustomSNInfo.u8ProductSerialNumLength = u8ProductSerialNumLength;
}

/// @brief Get custom product serial num
/// @param pProductSerialNum
/// @param u8ProcductSNLength the data size of pProductSerialNum buffer need align u8ProcductSNLength
const EDID_SERIAL_NUM_INFO* CustomEDID_GeCustomSN(void)
{
    return (const EDID_SERIAL_NUM_INFO*) (&_g_CustomSNInfo);
}

/// @brief 
/// @param pEDID The EDID you want to re-calculate base block checksum 
void CustomEDID_ResetEDIDBaseBlockChecksum(BYTE *pEDID)
{
    WORD i, Sum = 0;

    if(pEDID == NULL)
    {
        return;
    }

    for (i = 0; i < 127; i++)
    {
        Sum += *(pEDID + i);
    }

    *(pEDID + 127) = 0x100 - (BYTE)Sum;
}

/// @brief Replace the date code of EDID
/// @param u8Week 
/// @param u8Year 
/// @param pEDID the EDID you want to process the replacement
void CustomEDID_ReplaceEDIDWeekYear(BYTE u8Week, BYTE u8Year, BYTE *pEDID)
{
    if(pEDID == NULL)
    {
        return;
    }

    *(pEDID + 0x10) = u8Week;
    *(pEDID + 0x11) = u8Year;
}

/// @brief Replace the serial num of EDID
/// @param pEDID the EDID you want to process the replacement
void CustomEDID_ReplaceEDIDSerialNum(const EDID_SERIAL_NUM_INFO* CustomSNInfo, BYTE *pEDID)
{
    BYTE i = 0;
    BYTE u8ProductSerialNumAddr = 0;;
    
    if((pEDID == NULL) || (CustomSNInfo == NULL))
    {
        CUSTOM_EDID_printMsg("<WARNING> NULL pointer!!!\r\n");
        return;
    }

    u8ProductSerialNumAddr = _CustomEDID_DescriptorFinder(pEDID, eEdidDesc_DisplaySerialNumber);

    for(i = 0; i < 4; i++)//ID SN is fixed to 4 byte
    {
        *(pEDID + DEFAULT_EDID_SN1_ADDRESS + i) = CustomSNInfo->u8IDSerialNum[i];
    }
    
    
    if(u8ProductSerialNumAddr && (CustomSNInfo->u8ProductSerialNumLength <= 13))//Max product SN length is 13
    {
        for(i = 0; i < CustomSNInfo->u8ProductSerialNumLength; i++)
        {
            *(pEDID + u8ProductSerialNumAddr + EDID_DISP_DESCRIPTOR_HEADER_SIZE + i) = CustomSNInfo->u8ProductSerialNum[i];
        }
    }
    else
    {
        CUSTOM_EDID_printMsg("<WARNING>Product SN error\r\n");
        CUSTOM_EDID_printData("u8ProductSerialNumAddr: %d\r\n", u8ProductSerialNumAddr);
        CUSTOM_EDID_printData("CustomSNInfo.u8ProductSerialNumLength: %d, %d\r\n", CustomSNInfo->u8ProductSerialNumLength);
    }
    
}

void CustomEDID_ReplaceEDIDCustomData(BYTE *pEDID)
{
    if(pEDID == NULL)
    {
        return;
    }

    //Replace EDID week/year with custom data
    BYTE u8Week, u8Year;
    u8Week = CustomEDID_GetProductionWeek();
    u8Year = CustomEDID_GetProductionYear();
    CustomEDID_ReplaceEDIDWeekYear(u8Week, u8Year, pEDID);

    //Replace EDID SN with custom data
    const EDID_SERIAL_NUM_INFO *CustomSNInfo = CustomEDID_GeCustomSN();
    CustomEDID_ReplaceEDIDSerialNum(CustomSNInfo, pEDID);

    //Caculate check sum
    CustomEDID_ResetEDIDBaseBlockChecksum(pEDID);
}



//

//


BYTE* CustomEDID_InternalEDID_Get(BYTE u8InputPort);


#if ENABLE_HDMI
extern BYTE tEDID_TABLE_COMBO_PORT0_NO_FREESYNC[256];
extern BYTE tEDID_TABLE_COMBO_PORT1_NO_FREESYNC[256];
extern BYTE tEDID_TABLE_COMBO_PORT0[256];
extern BYTE tEDID_TABLE_COMBO_PORT1[256];
#endif

#if (ENABLE_DP_INPUT && (CHIP_ID == CHIP_MT9701))
extern BYTE EDID_DPRX_PORT0[DP_EDID_MAX_SIZE];
extern BYTE EDID_DPRX_PORT1[DP_EDID_MAX_SIZE];
extern BYTE EDID_DPRX_PORT2[DP_EDID_MAX_SIZE];

extern BYTE EDID_DPRX_PORT0_NO_HDR[DP_EDID_MAX_SIZE];
extern BYTE EDID_DPRX_PORT1_NO_HDR[DP_EDID_MAX_SIZE];
extern BYTE EDID_DPRX_PORT2_NO_HDR[DP_EDID_MAX_SIZE];
#endif


/// @brief Get custom EDID by port, CSA can customize this function by different requirement
/// @param u8InputPort 
/// @return Custom EDID pointer
BYTE* CustomEDID_InternalEDID_Get(BYTE u8InputPort)
{
    BYTE *pEDID = NULL;
    #if (CHIP_ID == CHIP_MT9701)
    if(INPUT_IS_DISPLAYPORT(u8InputPort)||INPUT_IS_USBTYPEC(u8InputPort))
    {
        if(UserPrefDPVersion==DPVersionMenuItems_DP1_4)
        {
            if((Input_Displayport != Input_Nothing) && (u8InputPort == Input_Displayport))
            {
                //pEDID = EDID_DPRX_PORT0;
                pEDID = tEDID_AllFeature_DP_14_P0;
            }
            else if((Input_Displayport2 != Input_Nothing) && (u8InputPort == Input_Displayport2))
            {
                //pEDID = EDID_DPRX_PORT1;
                pEDID = tEDID_AllFeature_DP_14_P1;
            }
            else if((Input_Displayport3 != Input_Nothing) && (u8InputPort == Input_Displayport3))
            {
                //pEDID = EDID_DPRX_PORT2;
                pEDID = tEDID_AllFeature_DP_14_P2;
            }
            else if((Input_UsbTypeC3 != Input_Nothing) && (u8InputPort == Input_UsbTypeC3))
            {
                //pEDID = EDID_DPRX_PORT2;
                pEDID = tEDID_AllFeature_DP_14_P2;
            }
            else
            {
                //pEDID = EDID_DPRX_PORT0;
                pEDID = tEDID_AllFeature_DP_14_P0;
            }
        }
        else
        {
            if((Input_Displayport!=Input_Nothing) && (u8InputPort == Input_Displayport))
            {
                //pEDID = EDID_DPRX_PORT0_NO_HDR;
                pEDID = tEDID_AllFeature_DP_12_P0;
            }
            else if((Input_Displayport2!=Input_Nothing) && (u8InputPort == Input_Displayport2))
            {
                //pEDID = EDID_DPRX_PORT1_NO_HDR;
                pEDID = tEDID_AllFeature_DP_12_P1;
            }
            else if((Input_Displayport3!=Input_Nothing) && (u8InputPort == Input_Displayport3))
            {
                //pEDID = EDID_DPRX_PORT2_NO_HDR;
                pEDID = tEDID_AllFeature_DP_12_P2;
            }
            else if((Input_UsbTypeC3 != Input_Nothing) && (u8InputPort == Input_UsbTypeC3))
            {
                //pEDID = EDID_DPRX_PORT2_NO_HDR;
                pEDID = tEDID_AllFeature_DP_12_P2;
            }
            else
            {
                //pEDID = EDID_DPRX_PORT0_NO_HDR;
                pEDID = tEDID_AllFeature_DP_12_P0;
            }
        }
    }
	#if ENABLE_HDMI
    else if(INPUT_IS_HDMI(u8InputPort))
    {
        #if ENABLE_FREESYNC
        if(UserprefFreeSyncMode == FreeSyncMenuItems_On)
        {
            if((Input_HDMI!=Input_Nothing) && (u8InputPort == Input_HDMI))
            {
                //pEDID = tEDID_TABLE_COMBO_PORT0;
                pEDID = tEDID_HDMI_P0;
            }
            else if((Input_HDMI2!=Input_Nothing) && (u8InputPort == Input_HDMI2))
            {
                //pEDID = tEDID_TABLE_COMBO_PORT1;
                pEDID = tEDID_HDMI_P1;
            }
            else
            {
                //pEDID = tEDID_TABLE_COMBO_PORT0;
                pEDID = tEDID_HDMI_P0;
            }
        }
        else if(UserprefFreeSyncMode == FreeSyncMenuItems_Off)
        #endif
        {
            if((Input_HDMI!=Input_Nothing) && (u8InputPort == Input_HDMI))
            {
                //pEDID = tEDID_TABLE_COMBO_PORT0_NO_FREESYNC;
                pEDID = tEDID_NoFreeSync_HDMI_P0;
            }
            else if((Input_HDMI2!=Input_Nothing) && (u8InputPort == Input_HDMI2))
            {
                //pEDID = tEDID_TABLE_COMBO_PORT1_NO_FREESYNC;
                pEDID = tEDID_NoFreeSync_HDMI_P1;
            }
            else
            {
                //pEDID = tEDID_TABLE_COMBO_PORT0_NO_FREESYNC;
                pEDID = tEDID_NoFreeSync_HDMI_P0;
            }

        }
    }
	#endif
    #elif (CHIP_ID == CHIP_MT9700)
    if(INPUT_IS_DISPLAYPORT(u8InputPort)||INPUT_IS_USBTYPEC(u8InputPort))
    {
        #if (ENABLE_DP_INPUT == 0x1)
        if((Input_Displayport!=Input_Nothing) && (u8InputPort == Input_Displayport))
        {
            pEDID = EDID_DPRX_PORT0;
        }
        else if((Input_Displayport2!=Input_Nothing) && (u8InputPort == Input_Displayport2))
        {
            pEDID = EDID_DPRX_PORT1;
        }
        else if((Input_Displayport3!=Input_Nothing) && (u8InputPort == Input_Displayport3))
        {
            pEDID = EDID_DPRX_PORT2;
        }
        else if((Input_Displayport4!=Input_Nothing) && (u8InputPort == Input_Displayport4))
        {
            pEDID = EDID_DPRX_PORT3;
        }
        else if((Input_UsbTypeC3 != Input_Nothing) && (u8InputPort == Input_UsbTypeC3))
        {
            pEDID = EDID_DPRX_PORT3;
        }
        else
        {
            pEDID = EDID_DPRX_PORT0;
        }
        #endif

    }
	#if ENABLE_HDMI
    else if(INPUT_IS_HDMI(u8InputPort))
    {
        #if ENABLE_FREESYNC
        if(UserprefFreeSyncMode == FreeSyncMenuItems_On)
        {
            if((Input_HDMI!=Input_Nothing) && (u8InputPort == Input_HDMI))
            {
                pEDID = tEDID_TABLE_COMBO_PORT0_Freesync;
            }
            else if((Input_HDMI2!=Input_Nothing) && (u8InputPort == Input_HDMI2))
            {
                pEDID = tEDID_TABLE_COMBO_PORT1_Freesync;
            }
            else if((Input_HDMI3!=Input_Nothing) && (u8InputPort == Input_HDMI3))
            {
                pEDID = tEDID_TABLE_COMBO_PORT2_Freesync;
            }
            else if((Input_HDMI4!=Input_Nothing) && (u8InputPort == Input_HDMI4))
            {
                pEDID = tEDID_TABLE_COMBO_PORT3_Freesync;
            }
            else
            {
                pEDID = tEDID_TABLE_COMBO_PORT0_Freesync;
            }
        }
        else if (UserprefFreeSyncMode == FreeSyncMenuItems_Off)
        #endif
        {
            if((Input_HDMI!=Input_Nothing) && (u8InputPort == Input_HDMI))
            {
                pEDID = tEDID_TABLE_COMBO_PORT0;
            }
            else if((Input_HDMI2!=Input_Nothing) && (u8InputPort == Input_HDMI2))
            {
                pEDID = tEDID_TABLE_COMBO_PORT1;
            }
            else if((Input_HDMI3!=Input_Nothing) && (u8InputPort == Input_HDMI3))
            {
                pEDID = tEDID_TABLE_COMBO_PORT2;
            }
            else if((Input_HDMI4!=Input_Nothing) && (u8InputPort == Input_HDMI4))
            {
                pEDID = tEDID_TABLE_COMBO_PORT3;
            }
            else
            {
                pEDID = tEDID_TABLE_COMBO_PORT0;
            }
        }
    }
	#endif
    #endif
    else
    {
        CUSTOM_EDID_printData("<WARNING> Can't get EDID, port: %d\r\n", u8InputPort);
    }
    
    return pEDID;
}

/// @brief Set EDID to target input port
/// @param u8InputPort 
/// @param pEDID 
void CustomEDID_InternalEDID_Set(BYTE u8InputPort, BYTE* pEDID)
{
    if(pEDID == NULL)
    {
        CUSTOM_EDID_printMsg("<WARNING> NULL pointer pEDID\r\n");
        return;
    }

    #if EDID_WEEK_YEAR_SN_REPLACEMENT
    CustomEDID_ReplaceEDIDCustomData(pEDID);
    #endif

    if(INPUT_IS_DISPLAYPORT(u8InputPort)||INPUT_IS_USBTYPEC(u8InputPort))
    {
        #if (ENABLE_DP_INPUT == 0x1)
        mapi_DPRx_Load_EDID(u8InputPort, pEDID);
        #endif
    }
    else if(INPUT_IS_HDMI(u8InputPort))
    {
        #if ENABLE_HDMI_EDID_INTERNAL_DATA
        msAPI_combo_HDMIRx_EDIDLoad(u8InputPort, pEDID);
        #endif
    }
}

/// @brief reload EDID on target input port, notice that the condition in CustomEDID_InternalEDID_Get need to be changed before calling this function
/// @param u8InputPort 
void CustomEDID_UpdateEDID(BYTE u8InputPort)
{
    BYTE *pEDID = NULL;

    pEDID = CustomEDID_InternalEDID_Get(u8InputPort);
    if(pEDID != NULL)
    {
        CustomEDID_InternalEDID_Set(u8InputPort, pEDID);
    }
}

void CustomEDID_InitInternalEDID(void)
{
    BYTE u8InputPort = 0;
    
    #if EDID_WEEK_YEAR_SN_REPLACEMENT
    CustomEDID_SetProductionWeek(DEFAULT_PRODUCTION_WEEK);
    CustomEDID_SetProductionWeek(DEFAULT_PRODUCTION_YEAR);
    CustomEDID_SetCustomSN((BYTE*)gu8DefaultIdSerialNum, (BYTE*)gu8DefaultProductSerialNum, DEFAULT_PRODUCT_SERIAL_NUM_LENGTH);
    #endif

    for(u8InputPort = 0; u8InputPort < Input_Nums; u8InputPort++)
    {
        CustomEDID_UpdateEDID(u8InputPort);
    }
}
