//=============================================================================================================================================================================================================================:
#define BOE_MENUFUNC_DEBUG                                      0
#if ENABLE_MSTV_UART_DEBUG && BOE_MENUFUNC_DEBUG
#define BOE_MENUFUNC_printDatas(str,...)                        printf(str,##__VA_ARGS__)
#define BOE_MENUFUNC_printData(str,value)                       printData(str, value)
#define BOE_MENUFUNC_printMsg(str)                              printMsg(str)
#else
#define BOE_MENUFUNC_printDatas(str,...)
#define BOE_MENUFUNC_printData(str,value)
#define BOE_MENUFUNC_printMsg(str)
#endif
//=============================================================================================================================================================================================================================:
/* no use
void i2c_WriteTBL_EDID ( BYTE deviceID, WORD addr, BYTE *buffer, BYTE count );
void i2c_ReadTBL_EDID ( BYTE deviceID, WORD addr, BYTE *buffer, BYTE count );
//=============================================================================================================================================================================================================================:
typedef enum edid_source
{
    EDID_VGA, EDID_DVI, EDID_HDMI1, EDID_HDMI2, EDID_HDMI3
}
EDID_Source;
*/
extern xdata BYTE MenuPageIndex;
extern xdata BYTE MenuItemIndex;
//=============================================================================================================================================================================================================================:
Bool appFactorySetBurninFlag ( drvFactoryModeType ucControl )
{
    UNUSED(ucControl);
/*������make�ݬ�
    if ( ucControl )
    {
        msAPI_AdjustBrightness(100);
        SET_BURNIN_MODE_FLAG();
    }
    else
    {
        msAPI_AdjustBrightness(USER_PREF_BRIGHTNESS);
        CLR_BURNIN_MODE_FLAG();
    }

    Power_TurnOnAmberLed();
    #if USEFLASH
    //UserPref_EnableFlashSaveBit( bFlashSaveFactoryBit);
    #else
    SaveMonitorSetting();
    #endif
*/
    return TRUE;
}
//=============================================================================================================================================================================================================================:
#if 1
Bool appFactorySetProductModeFlag ( drvFactoryModeType ucControl )
{
    UNUSED(ucControl);
/*������make�ݬ�
    if ( ucControl )
    {
        Set_ProductModeFlag();
        USER_PREF_PRODUCT_MODE_VALUE = PRODUCT_MODE_ON_VALUE;
    }
    else
    {
        Clr_ProductModeFlag();
        USER_PREF_PRODUCT_MODE_VALUE = PRODUCT_MODE_OFF_VALUE;
    }

    #if USEFLASH
    UserPref_EnableFlashSaveBit ( bFlashSaveFactoryBit );
    #else
    FactoryBlockSave();
    #endif
*/
    return TRUE;
}
#endif
//=============================================================================================================================================================================================================================:
#define xfr_FlashCode                                           ((unsigned char  volatile *) 0xC0000000)
//=============================================================================================================================================================================================================================:
#if 0 //no use
BYTE appFactoryWriteEDID ( drvFactoryDeviceType ucDevice, WORD wAddr, BYTE *buf, BYTE ucLen )
{
    BYTE u8ReturnValue = 0;

    switch ( ucDevice )
    {
        case DEVICE_HDMI1:
        case DEVICE_HDMI2:
        case DEVICE_HDMI3:
            #if ENABLE_EXTERNAL_EDID
            if ( ucDevice == DEVICE_HDMI1 )
            {
                EDID_SRC = EDID_HDMI1;
            }
            else if ( ucDevice == DEVICE_HDMI2 )
            {
                EDID_SRC = EDID_HDMI2;
            }
            else
            {
                EDID_SRC = EDID_HDMI3;
            }

            i2c_WriteTBL_EDID ( 0xA0, wAddr, buf, ucLen );
            #else
            #endif
            break;

        case DEVICE_DP1:
            NVRam_WriteTbl ( ADDR_DP1_EDID_BLOCK + wAddr, buf, ucLen ); //DP1.2
            break;

		#if BOE_ENABLE_DP11_EDID
		case DEVICE_DP1_2:
            NVRam_WriteTbl ( ADDR_DP3_EDID_BLOCK + wAddr, buf, ucLen ); //DP1.1
            break;
		#endif

    #if ENABLE_USB_TYPEC
      #if ENABLE_OSD_SUBUSB3_0
		case DEVICE_TYPE_C1:
            NVRam_WriteTbl ( ADDR_DP2_EDID_BLOCK + wAddr, buf, ucLen );
            break;
		case DEVICE_TYPE_C1_2:
			NVRam_WriteTbl ( ADDR_MHL_EDID_BLOCK + wAddr, buf, ucLen );
            break;
      #else //ENABLE_OSD_SUBUSB3_0
		case DEVICE_TYPE_C1:
		case DEVICE_TYPE_C1_2:
            NVRam_WriteTbl ( ADDR_MHL_EDID_BLOCK + wAddr, buf, ucLen );
            break;
      #endif //ENABLE_OSD_SUBUSB3_0
	#else
        case DEVICE_DP2:
            NVRam_WriteTbl ( ADDR_DP2_EDID_BLOCK + wAddr, buf, ucLen );
            break;
    #endif

        case DEVICE_OSDSN:
            //TempSNLen=ucLen;
            NVRam_WriteTbl ( ADDR_SN_BLOCK, buf, ucLen ); //ucLen
            break;

		case DEVICE_FACTORY_SN:
            //TempSNLen=ucLen;
            NVRam_WriteTbl ( ADDR_BOE_SN_BLOCK, buf, ucLen ); //ucLen
            break;

        default:
            u8ReturnValue = 1;
            //DDC2BI_FDCRCLO = 0x00;
            break;
    }

    return u8ReturnValue;
}
//=============================================================================================================================================================================================================================:
void appFactoryGetEDID ( drvFactoryDeviceType ucDevice, WORD wAddr, BYTE *buf, BYTE ucLen )
{
    switch ( ucDevice )
    {
        case DEVICE_HDMI1:
        case DEVICE_HDMI2:
        case DEVICE_HDMI3:
            #if ENABLE_EXTERNAL_EDID
            if ( ucDevice == DEVICE_HDMI1 )
            {
                EDID_SRC = EDID_HDMI1;
            }
            else if ( ucDevice == DEVICE_HDMI2 )
            {
                EDID_SRC = EDID_HDMI2;
            }
            else
            {
                EDID_SRC = EDID_HDMI3;
            }

            i2c_ReadTBL_EDID ( 0xA0, wAddr, buf, ucLen );
            #else
            #endif
            break;

        case DEVICE_DP1:
            NVRam_ReadTbl ( ADDR_DP1_EDID_BLOCK + wAddr, buf, ucLen ); //DP1.2
            break;

		#if BOE_ENABLE_DP11_EDID
		case DEVICE_DP1_2:
            NVRam_ReadTbl ( ADDR_DP3_EDID_BLOCK + wAddr, buf, ucLen ); //DP1.1
            break;
		#endif

        #if ENABLE_USB_TYPEC
		case DEVICE_TYPE_C1:
            NVRam_ReadTbl ( ADDR_DP2_EDID_BLOCK + wAddr, buf, ucLen );
            break;
		case DEVICE_TYPE_C1_2:
            NVRam_ReadTbl ( ADDR_MHL_EDID_BLOCK + wAddr, buf, ucLen );
            break;
		#else
        case DEVICE_DP2:
            NVRam_ReadTbl ( ADDR_DP2_EDID_BLOCK + wAddr, buf, ucLen );
            break;
        #endif

        case DEVICE_OSDSN:
            NVRam_ReadTbl ( ADDR_SN_BLOCK, buf,  ucLen );
            break;

		case DEVICE_FACTORY_SN:
            NVRam_ReadTbl ( ADDR_BOE_SN_BLOCK, buf,  ucLen );
            break;

        default:
            break;
    }
}
#endif
//=============================================================================================================================================================================================================================:
WORD CRC16 ( BYTE nData, WORD nCRC )
{
    BYTE nMask;
    Bool fFeedback;

    for ( nMask = 0x80; nMask; nMask >>= 1 )
    {
        fFeedback = ( Bool ) ( ( ( nCRC & 0x8000 ) ? 1 : 0 ) ^ ( ( nMask & nData ) ? 1 : 0 ) );
        nCRC <<= 1;

        if ( fFeedback )
        {
            nCRC ^= 0x1021;
        }
    }

    return nCRC;
}

//=============================================================================================================================================================================================================================:
/*
Bool EEPROM_Erase ( void )
{
    WORD xdata Index = 0;

    for ( Index = 0; Index < CNT_FACTORY_BLOCK; Index++ )
    {
        NVRam_WriteByte ( ADDR_FACTORY_BLOCK + Index, 0xFF );
    }

  	#if( ENABLE_WATCH_DOG )
    msAPI_ClearWDT();
  	#endif
    return TRUE;
}
*/
//=============================================================================================================================================================================================================================:
/*
Bool EEPROM_FACTORY(void)
{
	FactoryBlockInit();
    FactoryBlockSave();
	USER_PREF_BRIGHTNESS = DEF_BRIGHTNESS;
    CURRENT_CONTRAST = DEF_CONTRAST;
    msAPI_AdjustBrightness(USER_PREF_BRIGHTNESS);
    msAPI_AdjustContrast(USER_PREF_WIN_SEL, CURRENT_CONTRAST);

	return TRUE;
}
*/
//=============================================================================================================================================================================================================================:
void appFactoryAdjRedGain ( WORD wData )
{
    UNUSED(wData);
/*������make�ݬ�
    if(CURRENT_COLOR_TEMPERATURE == PCT_6500K)
    {
    	USER_PREF_RED_COLOR_6500K = wData;
    	msAPI_AdjustRGBColor(USER_PREF_WIN_SEL, USER_PREF_RED_COLOR_6500K, USER_PREF_GREEN_COLOR_6500K, USER_PREF_BLUE_COLOR_6500K);
    }
	else if(CURRENT_COLOR_TEMPERATURE == PCT_9300K)
	{
		USER_PREF_RED_COLOR_9300K = wData;
    	msAPI_AdjustRGBColor(USER_PREF_WIN_SEL, USER_PREF_RED_COLOR_9300K, USER_PREF_GREEN_COLOR_9300K, USER_PREF_BLUE_COLOR_9300K);
	}
	else
	{
		CURRENT_RED_COLOR = wData; // stored user color for user default
    	USER_PREF_RED_COLOR_USER = wData;
    	msAPI_AdjustRGBColor(USER_PREF_WIN_SEL, USER_PREF_RED_COLOR_USER, USER_PREF_GREEN_COLOR_USER, USER_PREF_BLUE_COLOR_USER);
	}
*/
}
//=============================================================================================================================================================================================================================:
void appFactoryAdjGreenGain ( WORD wData )
{
    UNUSED(wData);
/*������make�ݬ�
    if(CURRENT_COLOR_TEMPERATURE == PCT_6500K)
    {
    	USER_PREF_GREEN_COLOR_6500K = wData;
    	msAPI_AdjustRGBColor(USER_PREF_WIN_SEL, USER_PREF_RED_COLOR_6500K, USER_PREF_GREEN_COLOR_6500K, USER_PREF_BLUE_COLOR_6500K);
    }
	else if(CURRENT_COLOR_TEMPERATURE == PCT_9300K)
	{
		USER_PREF_GREEN_COLOR_9300K = wData;
    	msAPI_AdjustRGBColor(USER_PREF_WIN_SEL, USER_PREF_RED_COLOR_9300K, USER_PREF_GREEN_COLOR_9300K, USER_PREF_BLUE_COLOR_9300K);
	}
	else
	{
		CURRENT_GREEN_COLOR = wData; // stored user color for user default
    	USER_PREF_GREEN_COLOR_USER = wData;
    	msAPI_AdjustRGBColor(USER_PREF_WIN_SEL, USER_PREF_RED_COLOR_USER, USER_PREF_GREEN_COLOR_USER, USER_PREF_BLUE_COLOR_USER);
	}
*/
}
//=============================================================================================================================================================================================================================:
void appFactoryAdjBlueGain ( WORD wData )
{
    UNUSED(wData);
/*������make�ݬ�
    if(CURRENT_COLOR_TEMPERATURE == PCT_6500K)
    {
    	USER_PREF_BLUE_COLOR_6500K = wData;
    	msAPI_AdjustRGBColor(USER_PREF_WIN_SEL, USER_PREF_RED_COLOR_6500K, USER_PREF_GREEN_COLOR_6500K, USER_PREF_BLUE_COLOR_6500K);
    }
	else if(CURRENT_COLOR_TEMPERATURE == PCT_9300K)
	{
		USER_PREF_BLUE_COLOR_9300K = wData;
    	msAPI_AdjustRGBColor(USER_PREF_WIN_SEL, USER_PREF_RED_COLOR_9300K, USER_PREF_GREEN_COLOR_9300K, USER_PREF_BLUE_COLOR_9300K);
	}
	else
	{
		CURRENT_BLUE_COLOR = wData; // stored user color for user default
    	USER_PREF_BLUE_COLOR_USER = wData;
    	msAPI_AdjustRGBColor(USER_PREF_WIN_SEL, USER_PREF_RED_COLOR_USER, USER_PREF_GREEN_COLOR_USER, USER_PREF_BLUE_COLOR_USER);
	}
*/
}
//=============================================================================================================================================================================================================================:
BYTE appFactoryGetRedGain ( void )
{
    BYTE XDATA ucColor = 0;
	
/*������make�ݬ�
	if(CURRENT_COLOR_TEMPERATURE == PCT_6500K)
    {
		ucColor = USER_PREF_RED_COLOR_6500K;
	}
	else if(CURRENT_COLOR_TEMPERATURE == PCT_9300K)
	{
		ucColor = USER_PREF_RED_COLOR_9300K;
	}
	else
	{
		ucColor = CURRENT_RED_COLOR;
	}
*/
    return ucColor;
}
//=============================================================================================================================================================================================================================:
BYTE appFactoryGetGreenGain ( void )
{
    BYTE XDATA ucColor = 0;
	
/*������make�ݬ�
	if(CURRENT_COLOR_TEMPERATURE == PCT_6500K)
    {
		ucColor = USER_PREF_GREEN_COLOR_6500K;
	}
	else if(CURRENT_COLOR_TEMPERATURE == PCT_9300K)
	{
		ucColor = USER_PREF_GREEN_COLOR_9300K;
	}
	else
	{
    	ucColor = CURRENT_GREEN_COLOR;
	}
*/
    return ucColor;
}
//=============================================================================================================================================================================================================================:
BYTE appFactoryGetBlueGain ( void )
{
    BYTE XDATA ucColor = 0;

/*������make�ݬ�
	if(CURRENT_COLOR_TEMPERATURE == PCT_6500K)
    {
		ucColor = USER_PREF_BLUE_COLOR_6500K;
	}
	else if(CURRENT_COLOR_TEMPERATURE == PCT_9300K)
	{
		ucColor = USER_PREF_BLUE_COLOR_9300K;
	}
	else
    ucColor = CURRENT_BLUE_COLOR;
*/
    return ucColor;
}
//=============================================================================================================================================================================================================================:
#if 0 //no use
DWORD appFactorySetEdidWriteStart ( drvFactoryModeType ucControl )
{
    DWORD xdata u32InputEDID = 0;
    //Monitor Feedback: 6f 6e 83 02 EDIDPort Status chksum
    //Return Input Types Information
    //
    #if ENABLE_VGA_INPUT
    u32InputEDID |= VGA1_EDID;
    #endif
    #if ENABLE_DVI_INPUT
    u32InputEDID |= DVI1_EDID;
    #endif
    #if ENABLE_HDMI_INPUT
    u32InputEDID |= HDMI1_EDID;
    #endif
    #if ENABLE_HDMI2ND_INPUT
    u32InputEDID |= HDMI2_EDID;
    #endif
    #if ENABLE_HDMI3ND_INPUT
    u32InputEDID |= HDMI3_EDID;
    #endif
    #if ENABLE_DP_INPUT
    u32InputEDID |= DP1_EDID;
    #endif
    #if ENABLE_DP2ND_INPUT
	#if ENABLE_USB_TYPEC
	u32InputEDID |= TYPE_C1_EDID; 	// TypeC 4 Lane
	#else
    u32InputEDID |= DP2_EDID;
	#endif
    #endif

    //u16InputEDID = 0xA000;//aaa
    if ( ucControl == 0x01 )
    {
        u32InputEDID |= BIT1;
    }

    if ( ucControl == 0x00 )
    {
        hw_Set_DDC_WP();
        g_ucForceOnlyRunDDCCI = DISABLE;
    }
    else
    {
        hw_Clr_DDC_WP();
        g_ucForceOnlyRunDDCCI = ENABLE;
    }
    //u16InputEDID |= EDID_IN_FLASH;
#if BOE_ENABLE_SERIAL_NUMBER_ISP_SUPPORT
	  u32InputEDID |= INMPUT_EX;
#endif
	  u32InputEDID = ((u32InputEDID<<16)&0xFFFF0000);

  #if ENABLE_DP_INPUT
  #if BOE_ENABLE_DP11_EDID
	  u32InputEDID |= DP1_2_EDID; //DP1.1
  #endif
  #endif
  
  #if ENABLE_DP2ND_INPUT
  #if ENABLE_USB_TYPEC
	  u32InputEDID |= TYPE_C1_2_EDID; // TypeC 2 Lane
  #endif
  #endif
	
#if BOE_ENABLE_SERIAL_NUMBER_ISP_SUPPORT
	  u32InputEDID |= INMPUT_EX_3;
	  u32InputEDID |= (ISP_SN|BIT1);
#else
	  if(u32InputEDID&0x000000FF)
		  u32InputEDID |= INMPUT_EX_3;	
#endif
   
    return u32InputEDID;
}
//=============================================================================================================================================================================================================================:
DWORD appFactoryGetEdidDevice ( drvFactoryModeType ucControl )
{
    DWORD xdata u32InputEDID = 0;
    //Monitor Feedback: 6f 6e 83 02 EDIDPort Status chksum
    //Return Input Types Information
    //
    #if ENABLE_VGA_INPUT
    u32InputEDID |= VGA1_EDID;
    #endif
    #if ENABLE_DVI_INPUT
    u32InputEDID |= DVI1_EDID;
    #endif
    #if ENABLE_HDMI_INPUT
    u32InputEDID |= HDMI1_EDID;
    #endif
    #if ENABLE_HDMI2ND_INPUT
    u32InputEDID |= HDMI2_EDID;
    #endif
    #if ENABLE_HDMI3ND_INPUT
    u32InputEDID |= HDMI3_EDID;
    #endif
    #if ENABLE_DP_INPUT
    u32InputEDID |= DP1_EDID;
    #endif
    #if ENABLE_DP2ND_INPUT
	#if ENABLE_USB_TYPEC
	u32InputEDID |= TYPE_C1_EDID; 	// TypeC 4 Lane
	#else
    u32InputEDID |= DP2_EDID;
	#endif
    #endif

	if (!DDC_WP_FLAG)
    {
        u32InputEDID |= BIT1;
    }

    if ( ucControl == 0x01 )
    {
        g_ucForceOnlyRunDDCCI = ENABLE;
    }
  #if BOE_ENABLE_SERIAL_NUMBER_ISP_SUPPORT
    u32InputEDID |= INMPUT_EX;
  #endif
    u32InputEDID = ((u32InputEDID<<16)&0xFFFF0000);

	#if ENABLE_DP_INPUT
	#if BOE_ENABLE_DP11_EDID
	u32InputEDID |= DP1_2_EDID; //DP1.1
	#endif
	#endif
	
	#if ENABLE_DP2ND_INPUT
	#if ENABLE_USB_TYPEC
  	u32InputEDID |= TYPE_C1_2_EDID; // TypeC 2 Lane
  	#endif
	#endif

  #if BOE_ENABLE_SERIAL_NUMBER_ISP_SUPPORT
    u32InputEDID |= INMPUT_EX_3;
    u32InputEDID |= (ISP_SN|BIT1);
  #else
    if(u32InputEDID&0x000000FF)
		u32InputEDID |= INMPUT_EX_3;  
  #endif

    return u32InputEDID;
}
//=============================================================================================================================================================================================================================:
BYTE GetEdidDataNumber ( DWORD u32Data )
{
    BYTE i;
    
    if (((u32Data>>16)&INMPUT_EX)==INMPUT_EX)
    {    
        if (((u32Data>>8)&INMPUT_EX)==INMPUT_EX)
            i = 4;
        else 
            i = 3;
    }
    else
        i = 2;
        
    return i;    
}
#endif
//=============================================================================================================================================================================================================================:
/*
Bool appFactoryRecallColorPreset ( drvFactoryColorTempType ucColorTemp )
{
    switch ( ucColorTemp )
    {
        case FACTORY_COLOR_5000K:
            BOE_MENUFUNC_printDatas( "\r\n RecallColor 5000K", 0 );
            CURRENT_COLOR_TEMPERATURE = PCT_5000K;
            SetColorTempDDCCI();
            break;

        case FACTORY_COLOR_6500K:
            BOE_MENUFUNC_printDatas( "\r\n RecallColor 6500K", 0 );
            CURRENT_COLOR_TEMPERATURE = PCT_6500K;
            SetColorTempDDCCI();
            break;

        case FACTORY_COLOR_7500K:
            BOE_MENUFUNC_printDatas( "\r\n RecallColor 7500K", 0 );
            CURRENT_COLOR_TEMPERATURE = PCT_7500K;
            SetColorTempDDCCI();
            break;
		#if 0 //no use
        case FACTORY_COLOR_8200K:
            BOE_MENUFUNC_printDatas( "\r\n RecallColor 8200K", 0 );
            CURRENT_COLOR_TEMPERATURE = PCT_8200K;
            SetColorTempDDCCI();
            break;
		#endif
        case FACTORY_COLOR_9300K:
            BOE_MENUFUNC_printDatas( "\r\n RecallColor 9300K", 0 );
            CURRENT_COLOR_TEMPERATURE = PCT_9300K;
            SetColorTempDDCCI();
            break;
		#if 0 //no use
        case FACTORY_COLOR_11500K:
            BOE_MENUFUNC_printDatas( "\r\n RecallColor 11500K", 0 );
            CURRENT_COLOR_TEMPERATURE = PCT_11500K;
            SetColorTemperature ( USER_PREF_WIN_SEL,CURRENT_COLOR_TEMPERATURE );
            break;
		#endif
        #if 0 //ENABLE_sRGB //no use
        case FACTORY_COLOR_SRGB:
            BOE_MENUFUNC_printDatas( "\r\n RecallColor SRGB", 0 );
            CURRENT_COLOR_TEMPERATURE = PCT_SRGB;
            SetColorTempDDCCI();
            break;
        #endif
		case FACTORY_COLOR_USER:
            BOE_MENUFUNC_printDatas( "\r\n RecallColor user", 0 );
            CURRENT_COLOR_TEMPERATURE = PCT_DEFAULT;
            SetColorTempDDCCI();
            break;

        default:
            BOE_MENUFUNC_printDatas( "\r\n RecallColor default", 0 );
            return FALSE;
            break;
    }

    return TRUE;
}
*/
//=============================================================================================================================================================================================================================:
#if 0 //no use
Bool appFactorySaveColorPreset ( drvFactoryColorTempType ucColorTemp )
{
    switch ( ucColorTemp )
    {
        case FACTORY_COLOR_5000K:
            BOE_MENUFUNC_printDatas( "\r\n SaveColor 5000K", 0 );
            USER_PREF_RED_COLOR_5000K = CURRENT_RED_COLOR;
            USER_PREF_GREEN_COLOR_5000K = CURRENT_GREEN_COLOR;
            USER_PREF_BLUE_COLOR_5000K = CURRENT_BLUE_COLOR;
            SaveFactorySetting();
            break;

        case FACTORY_COLOR_6500K:
            BOE_MENUFUNC_printDatas( "\r\n SaveColor 6500K", 0 );
            USER_PREF_RED_COLOR_6500K = CURRENT_RED_COLOR;
            USER_PREF_GREEN_COLOR_6500K = CURRENT_GREEN_COLOR;
            USER_PREF_BLUE_COLOR_6500K = CURRENT_BLUE_COLOR;
            SaveFactorySetting();
            break;

        case FACTORY_COLOR_7500K:
            BOE_MENUFUNC_printDatas( "\r\n SaveColor 7500K", 0 );
            USER_PREF_RED_COLOR_7500K = CURRENT_RED_COLOR;
            USER_PREF_GREEN_COLOR_7500K = CURRENT_GREEN_COLOR;
            USER_PREF_BLUE_COLOR_7500K = CURRENT_BLUE_COLOR;
            SaveFactorySetting();
            break;
		#if 0 //no use
        case FACTORY_COLOR_8200K:
            BOE_MENUFUNC_printDatas( "\r\n SaveColor 8200K", 0 );
            USER_PREF_RED_COLOR_8200K = CURRENT_RED_COLOR;
            USER_PREF_GREEN_COLOR_8200K = CURRENT_GREEN_COLOR;
            USER_PREF_BLUE_COLOR_8200K = CURRENT_BLUE_COLOR;
            SaveFactorySetting();
            break;
		#endif

        case FACTORY_COLOR_9300K:
            BOE_MENUFUNC_printDatas( "\r\n SaveColor 9300K", 0 );
            USER_PREF_RED_COLOR_9300K = CURRENT_RED_COLOR;
            USER_PREF_GREEN_COLOR_9300K = CURRENT_GREEN_COLOR;
            USER_PREF_BLUE_COLOR_9300K = CURRENT_BLUE_COLOR;
            SaveFactorySetting();
            break;
		#if 0 //no use
        case FACTORY_COLOR_11500K:
            BOE_MENUFUNC_printDatas( "\r\n SaveColor 11500K", 0 );
            USER_PREF_RED_COLOR_11500K = CURRENT_RED_COLOR;
            USER_PREF_GREEN_COLOR_11500K = CURRENT_GREEN_COLOR;
            USER_PREF_BLUE_COLOR_11500K = CURRENT_BLUE_COLOR;
            SaveFactorySetting();
            break;
		#endif
		#if 0 //no use
        case FACTORY_COLOR_SRGB:
            BOE_MENUFUNC_printDatas( "\r\n SaveColor SRGB", 0 );
            USER_PREF_RED_COLOR_SRGB = CURRENT_RED_COLOR;
            USER_PREF_GREEN_COLOR_SRGB = CURRENT_GREEN_COLOR;
            USER_PREF_BLUE_COLOR_SRGB = CURRENT_BLUE_COLOR;
            SaveFactorySetting();
            break;
		#endif

        default:
            BOE_MENUFUNC_printDatas( "\r\n SaveColor default", 0 );
            return FALSE;
            break;
    }

    return TRUE;
}
#endif
//=============================================================================================================================================================================================================================:
#if 0 //no use
WORD appFactoryGetLanguage ( void )
{
    BYTE ucLanguage = LANG_English;
#if 1//LANGUAGE_TYPE == ASIA

    if ( USER_PREF_LANGUAGE == LANG_English ) //default
    {
        ucLanguage = LanEnglistCode;
    }
    else if ( USER_PREF_LANGUAGE == LANG_Russian )
    {
        ucLanguage = LanRussianCode;
    }
    else if ( USER_PREF_LANGUAGE == LANG_German )
    {
        ucLanguage = LanGermanCode;
    }
    else if ( USER_PREF_LANGUAGE == LANG_France )
    {
        ucLanguage = LanFrenchCode;
    }
    else if ( USER_PREF_LANGUAGE == LANG_Spanish )
    {
        ucLanguage = LanSpanishCode;
    }
    else if ( USER_PREF_LANGUAGE == LANG_Italian )
    {
        ucLanguage = LanItalianCode;
    }
    else if ( USER_PREF_LANGUAGE == LANG_Dutch )
    {
        ucLanguage = LanDutchCode;
    }
    else if ( USER_PREF_LANGUAGE == LANG_Finnish )
    {
        ucLanguage = LanFinnishCode;
    }
    else if ( USER_PREF_LANGUAGE == LANG_Turkish )
    {
        ucLanguage = LanTurkishCode;
    }
    else if ( USER_PREF_LANGUAGE == LANG_Polish )
    {
        ucLanguage = LanPolishCode;
    }
    else if ( USER_PREF_LANGUAGE == LANG_Portuguese )
    {
        ucLanguage = LanPortuguesePortugalCode;
    }
    else if ( USER_PREF_LANGUAGE == LANG_TChinese )
    {
        ucLanguage = LanTChineseCode;
    }
    else if ( USER_PREF_LANGUAGE == LANG_SChinese )
    {
        ucLanguage = LanSChineseCode;
    }
    else if ( USER_PREF_LANGUAGE == LANG_Japan )
    {
        ucLanguage = LanJapaneseCode;
    }
    else if ( USER_PREF_LANGUAGE == LANG_BrazilianPortuguese )
    {
        ucLanguage = LanPortugueseBrazilCode;
    }
  #if BOE_ENABLE_KOREAN_LANGUAGE
    else if ( USER_PREF_LANGUAGE == LANG_Korean )
    {
        ucLanguage = LanKoreanCode;
    }
  #endif

#endif
    return ucLanguage;
}
#endif
//=============================================================================================================================================================================================================================:
Bool drvFactoryAlignBOE()
{
    BYTE XDATA ucLen, ucCmd, ucFCode, ucOpcode, ucData0, ucData1, ucData2, ucData3;
    WORD XDATA WordValue;
    //static BYTE XDATA SNBuf[14];
    ucLen = DDC2BI_PKTSIZE & ( ~0x80 );
    ucCmd = DDC2BI_CMD;
	ucFCode = DDC2BI_FACTORYCODE;
    ucOpcode = DDC2BI_OPCODE;
    ucData0 = DDC2BI_DATA0;
    ucData1 = DDC2BI_DATA1;
    ucData2 = DDC2BI_DATA2;
    ucData3 = DDC2BI_DATA3;
printf("\r\nucCmd = %d\n",ucCmd);
printf("\r\nucOpcode = %d\n",ucOpcode);
	WordValue = ( ( DDC2BI_DATA0 << 8 ) | DDC2BI_DATA1 );

    if ( ucCmd == BOE_SETVCP || ucCmd == BOE_VCPFeature )
    {
        #if 0 //ENABLE_NEWDFM_REQUEST no use
        BYTE ucCount;

        if ( ( ucOpcode == BOE_FCode01 && ucData0 == 0xE0 ) || ( ucOpcode == BOE_FCode0A ) )
        {
        }
        else
        {
            g_wFDCRCValue = 0;

            if ( ucOpcode == BOE_EDID_DATA_TRANSMIT ) //if ( ucOpcode == BOE_FCode0F )
            {
                ucLen = DDC2BI_PKTSIZE - 0x86;
                DDC2BI_FDCRCHI = ucData2;
                DDC2BI_FDCRCLO = ucLen;
                DDC2BI_REPLYBYTE = ucData1;
                DDC2BI_PKTSIZE = 0x83;
            }
            else
            {
                for ( ucCount = 0; ucCount < ( ucLen - 0x02 ) ; ucCount++ )
                {
                    g_wFDCRCValue = CRC16 ( * ( &DDC2BI_OPCODE + ucCount ), g_wFDCRCValue );
                }

                DDC2BI_FDCRCHI = g_wFDCRCValue >> 8;
                DDC2BI_FDCRCLO = g_wFDCRCValue & 0xFF;
                DDC2BI_REPLYBYTE = 0x03;
                DDC2BI_PKTSIZE = 0x83;
            }

            DDC2Bi_MessageReady ( u8Source );
            SET_DFM_WRITECMD_REPLY_FLAG();
        }

        #endif

        switch ( ucOpcode )
        {
        	#if 0 //no use
            case BOE_Auto_Setup:
                #if ENABLE_VGA_INPUT
                if ( ucData1 == 0x00 && ucData2 == 0x00 )
                {
                    /*
                        AutoConfig();
                        #if ENABLE_NEWDFM_REQUEST
                        DDC2BI_REPLYBYTE = 0x02;
                        #endif
                    */
                    Bool result = 0;

                    if ( ucData2 == 0x00 )
                    {
                        result = mStar_AutoGeomtry ( USER_PREF_WIN_SEL );

                        if ( !result )
                        {
                            DDC2BI_REPLYBYTE = 0x04;
                        }
                        else
                        {
                            //wAutoProcessCount = 0;  //Stop wAutoProcess increase
                            //wAutoProcess0 = 100;
                            //wAutoProcess1 = 99;
                        }
                    }
                    else if ( ucData2 == 0x01 )
                    {
                        if ( !result )
                        {
                            DDC2BI_REPLYBYTE = 0x04;
                        }
                    }
                    else
                    {
                        DDC2BI_REPLYBYTE = 0x05;
                    }

                    if ( result )
                    {
                        DDC2BI_REPLYBYTE = 0x02;
                        USER_PREF_AUTOTIME ( USER_PREF_INPUT_TYPE ) = AUTO_TIMES_BIT;
                        SaveModeSetting();
                    }
                }

                #endif
                break;
				#endif
/*
            case BOE_Auto_Color :
                if ( ucData1 == 0x00 && ucData2 == 0x00 )
                {
                    #if ENABLE_VGA_INPUT
                    if ( mStar_AutoColor ( rxInputPort ) )
                    {
                        #ifdef ENABLE_AUTOCOLORENTERFACTORY //Antilan add for DFM new spec. 20120827
                        Set_AutoColorEnterFactoryFlag();
                        #endif
                        DDC2BI_REPLYBYTE_DATA0 = 0x01;
                    }
                    else
                    {
                        DDC2BI_REPLYBYTE_DATA0 = 0x00;
                    }

                    #endif
                    DDC2BI_PKTSIZE = 0x84;
                    DDC2BI_REPLYBYTE = 0x02;
                    DDC2BI_REPLYBYTE_OPCODE = ucOpcode;
                    DDC2BI_REPLYBYTE_DATA1 = 0x00;
                    SET_DFM_WRITECMD_REPLY_FLAG();
                }

                break;
*/
            case BOE_Select_Color_Preset : //0x14
            {
                if ( ( ucData1 >= BOE_MIN_RGB_GAIN ) && ( ucData1 <= BOE_MAX_RGB_GAIN ) ) //R
                {
                    if ( ucData0 == FACTORY_COLOR_6500K )
                    {
                        UserPrefRedColorWarm1 = ucData1;
                    }
/*
                    else if ( ucData0 == FACTORY_COLOR_7500K )
                    {
                        USER_PREF_RED_COLOR_7500K = ucData1;
                    }
*/
                    else if ( ucData0 == FACTORY_COLOR_9300K )
                    {
                        UserPrefRedColorCool1 = ucData1;
                    }
                    else if ( ucData0 == FACTORY_COLOR_SRGB )
                    {
                        UserPrefRedColorSRGB= ucData1;
                    }
                }

                if ( ( ucData2 >= BOE_MIN_RGB_GAIN ) && ( ucData2 <= BOE_MAX_RGB_GAIN ) ) //G
                {
                    if ( ucData0 == FACTORY_COLOR_6500K )
                    {
                        UserPrefGreenColorWarm1 = ucData2;
                    }
/*
                    else if ( ucData0 == FACTORY_COLOR_7500K )
                    {
                        USER_PREF_GREEN_COLOR_7500K = ucData2;
                    }
*/
                    else if ( ucData0 == FACTORY_COLOR_9300K )
                    {
                        UserPrefGreenColorCool1 = ucData2;
                    }
                    else if ( ucData0 == FACTORY_COLOR_SRGB )
                    {
                        UserPrefGreenColorSRGB = ucData2;
                    }
                }

                if ( ( ucData3 >= BOE_MIN_RGB_GAIN ) && ( ucData3 <= BOE_MAX_RGB_GAIN ) ) //B
                {
                    if ( ucData0 == FACTORY_COLOR_6500K )
                    {
                        UserPrefBlueColorWarm1 = ucData3;
                    }
/*
                    else if ( ucData0 == FACTORY_COLOR_7500K )
                    {
                        USER_PREF_BLUE_COLOR_7500K = ucData3;
                    }
*/
                    else if ( ucData0 == FACTORY_COLOR_9300K )
                    {
                        UserPrefBlueColorCool1 = ucData3;
                    }
                    else if ( ucData0 == FACTORY_COLOR_SRGB )
                    {
                        UserPrefBlueColorSRGB = ucData3;
                    }
                }

//                appFactoryRecallColorPreset ( ucData0 );
		UserPrefColorTemp = CTEMP_Warm1;
		SetColorTemp();
                DDC2BI_PKTSIZE = 0x82;
                DDC2BI_REPLYBYTE = 0x02;
                DDC2BI_REPLYBYTE_OPCODE = ucOpcode;
                SET_DFM_WRITECMD_REPLY_FLAG();
            }
            break;

            //***********************************BOE_FCode01 start*************************************************************
			case BOE_FCode01 : //0xF0 //Get number of input port
                if ( ucData0 == 0xF1 )
                {
                	#if 0 //no use
                    if ( ucData1 == BOE_FCode01_F1_EDID_ISP ) //0x03 0xF0 0xF1 0x01
                    {
                        DWORD u32FDCRCValue;
                    	BYTE u8SendDataNumber;

						DDC2BI_PKTSIZE = 0x83;
                        #if ENABLE_NEWDFM_REQUEST
                        DDC2BI_REPLYBYTE = 0x02;
                        #endif
                        u32FDCRCValue = appFactorySetEdidWriteStart ( ucData2 );
                        u8SendDataNumber = GetEdidDataNumber(u32FDCRCValue);
                        DDC2BI_DEVICEADDRPORT1 = u32FDCRCValue >> 24;
                        DDC2BI_DEVICEADDRPORT2 = u32FDCRCValue >> 16;
						if (u8SendDataNumber==3)
						{
                    	    DDC2BI_PKTSIZE = 0x84;
                            DDC2BI_DEVICEADDRPORT3 = u32FDCRCValue >> 8;
						}
						else if (u8SendDataNumber==4)
						{
                    	    DDC2BI_PKTSIZE = 0x85;
                            DDC2BI_DEVICEADDRPORT3 = u32FDCRCValue >> 8;
                            DDC2BI_DEVICEADDRPORT4 = u32FDCRCValue&0xFF;
						}
                        //DDC2BI_PKTSIZE = 0x83;
                    }
                    else if ( ucData1 == BOE_FCode01_F1_EDID_WP ) //0x03 0xF0 0xF1 0x02
                    {
                        if ( ucData2 == 0x00 )
                        {
                            hw_Set_DDC_WP();
                            DDC2BI_DEVICEADDRPORT4 = 0x00;
                        }
                        else
                        {
                            hw_Clr_DDC_WP();
                            DDC2BI_DEVICEADDRPORT4 = 0x01;
                        }

                        DDC2BI_PKTSIZE = 0x85;
                        DDC2BI_DEVICEADDRPORT1 = BOE_FCode01;
                        DDC2BI_DEVICEADDRPORT2 = 0xF1;
                        DDC2BI_DEVICEADDRPORT3 = BOE_FCode01_F1_EDID_WP;
                        #if ENABLE_NEWDFM_REQUEST
                        DDC2BI_REPLYBYTE = 0x02;
                        #endif
                    }	
                    else
					#endif
					if ( ucData1 == BOE_FCode01_F1_PRODUCT_MODE ) //0xFC 0xF0 0xF1 0x03
                    {
                        appFactorySetProductModeFlag ( ucData2 );
                        DDC2BI_PKTSIZE = 0x84;//0x85;
                        DDC2BI_DEVICEADDRPORT1 = BOE_FCode01;
                        DDC2BI_DEVICEADDRPORT2 = 0xF1;
                        DDC2BI_DEVICEADDRPORT3 = BOE_FCode01_F1_PRODUCT_MODE;
                        #if ENABLE_NEWDFM_REQUEST
                        DDC2BI_REPLYBYTE = 0x02;
                        #endif
                        SET_DFM_WRITECMD_REPLY_FLAG();
                    }
                }

                break;
            //***********************************BOE_FCode01 end*************************************************************

            case BOE_ADJ_Brightness :
            {
                #if 0 //WB-V4.04-180309.exe
                UserPrefBrightness = ucData1;
                #else
                UserPrefBrightness = WordValue;
                #endif
//                msAPI_AdjustBrightness(USER_PREF_BRIGHTNESS);
		mStar_AdjustBrightness(UserPrefBrightness);
                DDC2BI_PKTSIZE = 0x82;
                DDC2BI_REPLYBYTE = 0x02;
                DDC2BI_REPLYBYTE_OPCODE = ucOpcode;

                SET_DFM_WRITECMD_REPLY_FLAG();
            }
            break;

            case BOE_ADJ_Contrast :   // contrast
            {
                #if 0 //WB-V4.04-180309.exe
                UserPrefContrast = ucData1;
                #else
                UserPrefContrast = WordValue;
                #endif
//                msAPI_AdjustContrast(USER_PREF_WIN_SEL, CURRENT_CONTRAST);
		mStar_AdjustContrast(UserPrefContrast);

                DDC2BI_PKTSIZE = 0x82;
                DDC2BI_REPLYBYTE = 0x02;
                DDC2BI_REPLYBYTE_OPCODE = ucOpcode;
                SET_DFM_WRITECMD_REPLY_FLAG();
            }
            break;

            case BOE_ADJ_Red_Gain :        // Red Color
            {
                appFactoryAdjRedGain ( WordValue );
                DDC2BI_PKTSIZE = 0x82;
                DDC2BI_REPLYBYTE = 0x02;
                DDC2BI_REPLYBYTE_OPCODE = ucOpcode;
                SET_DFM_WRITECMD_REPLY_FLAG();
            }
            break;

            case BOE_ADJ_Green_Gain :  // Green Color
            {
                appFactoryAdjGreenGain ( WordValue );
                DDC2BI_PKTSIZE = 0x82;
                DDC2BI_REPLYBYTE = 0x02;
                DDC2BI_REPLYBYTE_OPCODE = ucOpcode;
                SET_DFM_WRITECMD_REPLY_FLAG();
            }
            break;

            case BOE_ADJ_Blue_Gain :           // Blue Color
            {
                appFactoryAdjBlueGain ( WordValue );
                DDC2BI_PKTSIZE = 0x82;
                DDC2BI_REPLYBYTE = 0x02;
                DDC2BI_REPLYBYTE_OPCODE = ucOpcode;
                SET_DFM_WRITECMD_REPLY_FLAG();
            }
            break;
/*
            case BOE_ADJ_Volume:   // Volume
            {
                #if 0
                USER_PREF_VOLUME = ucData1;
                #else
                CURRENT_CONTRAST = WordValue;
                #endif
                msAPI_AdjustVolume ( USER_PREF_VOLUME );

                DDC2BI_PKTSIZE = 0x82;
                DDC2BI_REPLYBYTE = 0x02;
                DDC2BI_REPLYBYTE_OPCODE = ucOpcode;
                SET_DFM_WRITECMD_REPLY_FLAG();
            }
            break;
*/
			#if 0 //no use
            case BOE_ADJ_Language://BOE_Language :       // Language
            {
                #if 1

                if ( ucExt == 0x00 ) //if ( ucData0 == 0x01 ) //WB-V4.04-180309.exe
                {
                    if ( ucData1 == LanEnglistCode )
                    {
                        USER_PREF_LANGUAGE = LANG_English;
                    }
                    else if ( ucData1 == LanRussianCode )
                    {
                        USER_PREF_LANGUAGE = LANG_Russian;
                    }
                    else if ( ucData1 == LanGermanCode )
                    {
                        USER_PREF_LANGUAGE = LANG_German;
                    }
                    else if ( ucData1 == LanFrenchCode )
                    {
                        USER_PREF_LANGUAGE = LANG_France;
                    }
                    else if ( ucData1 == LanSpanishCode )
                    {
                        USER_PREF_LANGUAGE = LANG_Spanish;
                    }
                    else if ( ucData1 == LanItalianCode )
                    {
                        USER_PREF_LANGUAGE = LANG_Italian;
                    }
                    else if ( ucData1 == LanDutchCode )
                    {
                        USER_PREF_LANGUAGE = LANG_Dutch;
                    }
                    else if ( ucData1 == LanFinnishCode )
                    {
                        USER_PREF_LANGUAGE = LANG_Finnish;
                    }
                    else if ( ucData1 == LanTurkishCode )
                    {
                        USER_PREF_LANGUAGE = LANG_Turkish;
                    }
                    else if ( ucData1 == LanPolishCode )
                    {
                        USER_PREF_LANGUAGE = LANG_Polish;
                    }
                    else if ( ucData1 == LanPortuguesePortugalCode )
                    {
                        USER_PREF_LANGUAGE = LANG_Portuguese;
                    }
                    else if ( ucData1 == LanTChineseCode )
                    {
                        USER_PREF_LANGUAGE = LANG_TChinese;
                    }
                    else if ( ucData1 == LanSChineseCode )
                    {
                        USER_PREF_LANGUAGE = LANG_SChinese;
                    }
                    else if ( ucData1 == LanJapaneseCode )
                    {
                        USER_PREF_LANGUAGE = LANG_Japan;
                    }
                    else if ( ucData1 == LanPortugueseBrazilCode )
                    {
                        USER_PREF_LANGUAGE = LANG_BrazilianPortuguese;
                    }
                #if BOE_ENABLE_KOREAN_LANGUAGE
                    else if ( ucData1 == LanKoreanCode )
                    {
                        USER_PREF_LANGUAGE = LANG_Korean;
                    }
                #endif

                    DDC2BI_PKTSIZE = 0x82;
                    DDC2BI_REPLYBYTE = 0x02;
                    DDC2BI_REPLYBYTE_OPCODE = ucOpcode;
                    SET_DFM_WRITECMD_REPLY_FLAG();
                }
                #endif
                else
                {
                    DDC2BI_PKTSIZE = 0x82;
                    DDC2BI_REPLYBYTE = 0x05;
                    DDC2BI_REPLYBYTE_OPCODE = ucOpcode;
                    SET_DFM_WRITECMD_REPLY_FLAG();
                }
            }
            break;
			#endif

            /*
			case BOE_OSD_Reset: //0x04
                //SET_DDCCI_BOE_OSD_Reset_FLAG();
                ResetAllSetting();
                DDC2BI_PKTSIZE = 0x82;
                DDC2BI_REPLYBYTE = 0x02;
                DDC2BI_REPLYBYTE_OPCODE = ucOpcode;
                SET_DFM_WRITECMD_REPLY_FLAG();
                break;
				
            case BOE_ALL_Reset :  // Factory All Reset
            {
                if ( ucData1 == 0x00 && ucData2 == 0x00 )
                {
                    if ( ResetAllSetting() )
                    {
                        DDCBuffer[u8Source][1] = 0x02;
                    }
                    else
                    {
                        DDCBuffer[u8Source][1] = 0x04;
                    }
                    DDC2BI_PKTSIZE = 0x82;
                	DDC2BI_REPLYBYTE = 0x02;
                	DDC2BI_REPLYBYTE_OPCODE = ucOpcode;
					SET_DFM_WRITECMD_REPLY_FLAG();
                }
            }
            break;

            case BOE_ADJ_Input_Source:
            {
                if ( ucData0 == 0x00 && ucData2 == 0x00 )
                {
                
                #if 0
                    if ( ucData1 == VCP_INPUT_SOURCE_VGA )  //VGA
                    {
                        mStar_SetupFreeRunMode();
                		SwitchInputSource(INPUT_PRIORITY_VGA);
                        #if ENABLE_NEWDFM_REQUEST
                        DDC2BI_REPLYBYTE = 0x02;
                        #endif
                    }
                    else
                #endif

                #if ENABLE_DVI_INPUT
                    if ( ucData1 == VCP_INPUT_SOURCE_DVI1 )  //DVI
                    {
						mStar_SetupFreeRunMode();
						SwitchInputSource(INPUT_PRIORITY_DVI);
                    }
                    else
                #endif
                #if ENABLE_HDMI_INPUT
                    if ( ucData1 == VCP_INPUT_SOURCE_HDMI1 )  //HDMI 1
                    {
						mStar_SetupFreeRunMode();
						SwitchInputSource(INPUT_PRIORITY_HDMI);
                    }
                    else
                #endif
                #if ENABLE_HDMI2ND_INPUT
                    if ( ucData1 == VCP_INPUT_SOURCE_HDMI2 )  //HDMI 2
                    {
						mStar_SetupFreeRunMode();
						SwitchInputSource(INPUT_PRIORITY_HDMI2ND);
                    }
                    else
                #endif
                #if ENABLE_HDMI3ND_INPUT
                    if ( ucData1 == VCP_INPUT_SOURCE_HDMI3 )  //HDMI 3
                    {
						mStar_SetupFreeRunMode();
						SwitchInputSource(INPUT_PRIORITY_HDMI3ND);
                    }
                    else
                #endif
                #if ENABLE_DP_INPUT
                    if ( ucData1 == VCP_INPUT_SOURCE_DP1 )  //DP 1
                    {
						mStar_SetupFreeRunMode();
						SwitchInputSource(INPUT_PRIORITY_DP);
                    }
                    else
                #endif
                #if ENABLE_DP2ND_INPUT
                    if ( ucData1 == VCP_INPUT_SOURCE_DP2 )  //DP 2
                    {
						mStar_SetupFreeRunMode();
						SwitchInputSource(INPUT_PRIORITY_DP2ND);
                    }
                    else
                #endif

                    {
                        #if ENABLE_NEWDFM_REQUEST
                        DDC2BI_REPLYBYTE = 0x05;
                        #endif
                    }
					DDC2BI_PKTSIZE = 0x82;
                	DDC2BI_REPLYBYTE = 0x02;
                	DDC2BI_REPLYBYTE_OPCODE = ucOpcode;
					SET_DFM_WRITECMD_REPLY_FLAG();
                }
            }
            break;

			case BOE_PowerMode : //0xD6
            {
                if ( POWER_ON_FLAG && (ucData0 == BOE_PowerMode_PowerOff) )
                {
					ExecuteKeyEvent (MIA_POWER);
                }
                else
                {
                    if ( POWER_ON_FLAG && BURNIN_MODE_FLAG && (ucData0 == BOE_PowerMode_PowerSaving) )
                    {
                        if ( !PowerSavingFlag )
                        {							
                        	appFactorySetBurninFlag ( DISABLE );
                            Set_ForcePowerSavingFlag();
                        }
                    }
                }
				DDC2BI_PKTSIZE = 0x82;
                DDC2BI_REPLYBYTE = 0x02;
                DDC2BI_REPLYBYTE_OPCODE = ucOpcode;
                SET_DFM_WRITECMD_REPLY_FLAG();
            }
            break;

            case BOE_EEPROM_INIT : //0xE0
                EEPROM_Erase();
                //USER_PREF_PRODUCT_MODE_VALUE = PRODUCT_MODE_ON_VALUE; //no use
                //Set_ProductModeFlag(); //no use
                DDC2BI_PKTSIZE = 0x82;
                DDC2BI_REPLYBYTE = 0x02;
                DDC2BI_REPLYBYTE_OPCODE = ucOpcode;
                SET_DFM_WRITECMD_REPLY_FLAG();
                break;
*/
            case BOE_ENTER_FACTORY : //0xE1
printf("\r\nBOE_ENTER_FACTORY\n");

                if( PowerOnFlag && MenuPageIndex <= RootMenu)
                {
                	Osd_Hide();
                    Clr_OsdLockModeFlag();
                    Clr_ShowOsdLockFlag();
                    Set_FactoryModeFlag();
                    MenuPageIndex = FactoryMenu; // 100908
                    MenuItemIndex = 0;
                    ExecuteKeyEvent( MIA_RedrawMenu );
                }

                DDC2BI_PKTSIZE = 0x82;
                DDC2BI_REPLYBYTE = 0x02;
                DDC2BI_REPLYBYTE_OPCODE = ucOpcode;

                SET_DFM_WRITECMD_REPLY_FLAG();
                break;

            case BOE_EXIT_FACTORY : //0xE2
/*
                CLR_FACTORY_MODE_FLAG();
                CLR_BURNIN_MODE_FLAG();
				USER_PREF_BACKLIGHT_MODE = DEF_BL_MODE;
				USER_PREF_FAC_LIDMODE = BL_LID_MODE;
*/
		SaveFactorySetting();
		ExecuteKeyEvent(MIA_Exit);
                g_ucForceOnlyRunDDCCI = DISABLE;

                DDC2BI_PKTSIZE = 0x82;
                DDC2BI_REPLYBYTE = 0x02;
                DDC2BI_REPLYBYTE_OPCODE = ucOpcode;
                SET_DFM_WRITECMD_REPLY_FLAG();
                break;
/*
            case BOE_ENTER_BURNIN : //0xE4
                appFactorySetBurninFlag ( ENABLE );
                DDC2BI_PKTSIZE = 0x82;
                DDC2BI_REPLYBYTE = 0x02;
                DDC2BI_REPLYBYTE_OPCODE = ucOpcode;
                SET_DFM_WRITECMD_REPLY_FLAG();
                break;

            case BOE_SAVE : //0xE5
                FactoryBlockSave();
                SystemBlockSave();
                ColorParameterBlockSave();
                DDC2BI_PKTSIZE = 0x82;
                DDC2BI_REPLYBYTE = 0x02;
                DDC2BI_REPLYBYTE_OPCODE = ucOpcode;
				SET_DFM_WRITECMD_REPLY_FLAG();
                break;
*/
            case BOE_RESET_FACTORY_DATA : //0xE6
                //EEPROM_FACTORY();
		 FactoryReset();
                g_ucForceOnlyRunDDCCI = ENABLE;
                DDC2BI_PKTSIZE = 0x82;
                DDC2BI_REPLYBYTE = 0x02;
                DDC2BI_REPLYBYTE_OPCODE = ucOpcode;
                SET_DFM_WRITECMD_REPLY_FLAG();
                break;

            case BOE_AUTO_COLOR_OFFSET : //0xF6
                #if ENABLE_NEWDFM_REQUEST
                DDC2BI_REPLYBYTE = 0x02;
                #endif
                break;

            case BOE_AUTO_COLOR_GAIN : //0xF7
                #if ENABLE_NEWDFM_REQUEST
                DDC2BI_REPLYBYTE = 0x02;
                #endif
                break;
			/* no use
            case BOE_EDID_DATA_TRANSMIT : //0xF8
            {
                BYTE u8Check;
                u8Check = appFactoryWriteEDID ( ucData0, ( ( WORD ) ucData1 << 8 ) + ucData2, ( BYTE * ) &DDC2BI_DATA3, ucLen ); //ucLen

                if ( u8Check > 0 )
                {
                    DDC2BI_FDCRCLO = 0x00;
                }

                break;
            }
			*/

			case BOE_ADJ_BRIGHTNESS_CURVE : //0xFC
            {
				if ( ucData0 == BOE_ADJ_Brightness_LID ) //adjust DDCCI_AdjustLID_Mode PWM for LiteMax test
				{
                	DDCCI_AdjustLID_Mode(ucData1); //LID_DC=0, LID_PWM=1
				}
#if LiteMAX_Baron_UI == 1
				else if ( ucData0 == BOE_ADJ_Brightness_PWM0 )
				{
                	DDCCI_AdjPWMCurve0(ucData1);
				}

				else if ( ucData0 == BOE_ADJ_Brightness_PWM75 )
				{
                	DDCCI_AdjPWMCurve75(ucData1);
				}
#endif
				else if ( ucData0 == BOE_ADJ_Brightness_PWM0_Auto ) //adjust PWM Brightness Curve 0 for LiteMax test
				{
					DDCCI_AdjPWMCurve0AutoCalculate(ucData1);
				}
				else if ( ucData0 == BOE_ADJ_Brightness_PWM100_Auto ) //adjust PWM Brightness Curve 100 for LiteMax test
				{
					DDCCI_AdjPWMCurve100AutoCalculate(ucData1);
				}
				else if ( ucData0 == BOE_ADJ_Brightness_DC0 ) //adjust DC Brightness Curve 0 for LiteMax test
				{
					DDCCI_AdjDCCurve0(ucData1);
				}
				else if ( ucData0 == BOE_ADJ_Brightness_DC25 ) //adjust DC Brightness Curve 25 for LiteMax test
				{
					DDCCI_AdjDCCurve25(ucData1);
				}
				else if ( ucData0 == BOE_ADJ_Brightness_DC50 ) //adjust DC Brightness Curve 50 for LiteMax test
				{
					DDCCI_AdjDCCurve50(ucData1);
				}
				else if ( ucData0 == BOE_ADJ_Brightness_DC75 ) //adjust DC Brightness Curve 75 for LiteMax test
				{
					DDCCI_AdjDCCurve75(ucData1);
				}
				else if ( ucData0 == BOE_ADJ_Brightness_DC100 ) //adjust DC Brightness Curve 100 for LiteMax test
				{
					DDCCI_AdjDCCurve100(ucData1);
				}
                DDC2BI_PKTSIZE = 0x82;
                DDC2BI_REPLYBYTE = 0x02;
                DDC2BI_REPLYBYTE_OPCODE = ucOpcode;
                SET_DFM_WRITECMD_REPLY_FLAG();
            }
            break;

            default:
                break;
        }
    }
    else if ( ucCmd == BOE_GETVCP ) //----------------------------------------------------------------
    {
        switch ( ucOpcode )
        {
            case BOE_ADJ_Brightness :
            {
                if ( ucData0 == 0x02 )
                {
                    g_wFDCRCValue = UserPrefBrightness;
                }
                else
                {
                    g_wFDCRCValue = UserPrefBrightness;
                }

				DDC2BI_PKTSIZE = 0x84;
                DDC2BI_REPLYBYTE = 0x02;
				DDC2BI_REPLYBYTE_OPCODE = BOE_ADJ_Brightness;
                DDC2BI_REPLYBYTE_DATA0 = g_wFDCRCValue >> 8;
                DDC2BI_REPLYBYTE_DATA1 = g_wFDCRCValue & 0xFF;
                SET_DFM_WRITECMD_REPLY_FLAG();
            }
            break;

            case BOE_ADJ_Contrast :
            {
                g_wFDCRCValue = UserPrefContrast;

				DDC2BI_PKTSIZE = 0x84;
                DDC2BI_REPLYBYTE = 0x02;
				DDC2BI_REPLYBYTE_OPCODE = BOE_ADJ_Contrast;
                DDC2BI_REPLYBYTE_DATA0 = g_wFDCRCValue >> 8;
                DDC2BI_REPLYBYTE_DATA1 = g_wFDCRCValue & 0xFF;
                SET_DFM_WRITECMD_REPLY_FLAG();
            }
            break;

            case BOE_ADJ_Red_Gain :
            {
                g_wFDCRCValue = appFactoryGetRedGain();
                DDC2BI_PKTSIZE = 0x84;
                DDC2BI_REPLYBYTE = 0x02;
				DDC2BI_REPLYBYTE_OPCODE = BOE_ADJ_Red_Gain;
                DDC2BI_REPLYBYTE_DATA0 = g_wFDCRCValue >> 8;
                DDC2BI_REPLYBYTE_DATA1 = g_wFDCRCValue & 0xFF;
                SET_DFM_WRITECMD_REPLY_FLAG();
            }
            break;

            case BOE_ADJ_Green_Gain:
            {
                g_wFDCRCValue = appFactoryGetGreenGain();
                DDC2BI_PKTSIZE = 0x84;
                DDC2BI_REPLYBYTE = 0x02;
				DDC2BI_REPLYBYTE_OPCODE = BOE_ADJ_Green_Gain;
                DDC2BI_REPLYBYTE_DATA0 = g_wFDCRCValue >> 8;
                DDC2BI_REPLYBYTE_DATA1 = g_wFDCRCValue & 0xFF;
                SET_DFM_WRITECMD_REPLY_FLAG();
            }
            break;

            case BOE_ADJ_Blue_Gain :
            {
                g_wFDCRCValue = appFactoryGetBlueGain();
                DDC2BI_PKTSIZE = 0x84;
                DDC2BI_REPLYBYTE = 0x02;
				DDC2BI_REPLYBYTE_OPCODE = BOE_ADJ_Blue_Gain;
                DDC2BI_REPLYBYTE_DATA0 = g_wFDCRCValue >> 8;
                DDC2BI_REPLYBYTE_DATA1 = g_wFDCRCValue & 0xFF;
                SET_DFM_WRITECMD_REPLY_FLAG();
            }
            break;
/*
            case BOE_ADJ_Volume:
            {
                g_wFDCRCValue = USER_PREF_VOLUME;

				DDC2BI_PKTSIZE = 0x84;
                DDC2BI_REPLYBYTE = 0x02;
				DDC2BI_REPLYBYTE_OPCODE = BOE_ADJ_Volume;
                DDC2BI_REPLYBYTE_DATA0 = g_wFDCRCValue >> 8;
                DDC2BI_REPLYBYTE_DATA1 = g_wFDCRCValue & 0xFF;
                SET_DFM_WRITECMD_REPLY_FLAG();
            }
            break;
*/
			/* no use
            case BOE_ADJ_Language :
            {
                g_wFDCRCValue = appFactoryGetLanguage();
                DDC2BI_PKTSIZE = 0x84;
                DDC2BI_REPLYBYTE = 0x02;
				DDC2BI_REPLYBYTE_OPCODE = BOE_ADJ_Language;
                DDC2BI_REPLYBYTE_DATA0 = g_wFDCRCValue >> 8;
                DDC2BI_REPLYBYTE_DATA1 = g_wFDCRCValue & 0xFF;
                SET_DFM_WRITECMD_REPLY_FLAG();
            }
            break;
			*/
			/* no use
            case BOE_FCode01 : //0xF0
                if ( ucData0 == 0xF1 )
                {
                    if ( ucData1 == BOE_FCode01_F1_EDID_ISP ) //0x03 0xF0 0xF1 0x01
                    {
                    	DWORD u32FDCRCValue;
                    	BYTE u8SendDataNumber;
                    	
                    	DDC2BI_PKTSIZE = 0x83;
                        #if ENABLE_NEWDFM_REQUEST
                        DDC2BI_REPLYBYTE = 0x02;
                        #endif
                        u32FDCRCValue = appFactoryGetEdidDevice ( ucData2 );
                        u8SendDataNumber = GetEdidDataNumber(u32FDCRCValue);
                        DDC2BI_DEVICEADDRPORT1 = u32FDCRCValue >> 24;
                        DDC2BI_DEVICEADDRPORT2 = u32FDCRCValue >> 16;
						if (u8SendDataNumber==3)
						{
                    	    DDC2BI_PKTSIZE = 0x84;
                            DDC2BI_DEVICEADDRPORT3 = u32FDCRCValue >> 8;
						}
						else if (u8SendDataNumber==4)
						{
                    	    DDC2BI_PKTSIZE = 0x85;
                            DDC2BI_DEVICEADDRPORT3 = u32FDCRCValue >> 8;
                            DDC2BI_DEVICEADDRPORT4 = u32FDCRCValue&0xFF;
						}
						SET_DFM_WRITECMD_REPLY_FLAG();
                    }
                }

                break;

            case BOE_EDID_DATA_TRANSMIT : //0xF8
            {
                WORD xdata wAddr;
                wAddr = ( ( WORD ) ucData1 << 8 ) + ucData2;
                ucLen = ucData3;
                appFactoryGetEDID ( ucData0, wAddr, ( BYTE * ) &DDC2BI_CMD, ucLen );
                DDC2BI_PKTSIZE = ucLen | 0x80;
                SET_DFM_WRITECMD_REPLY_FLAG();
            }
            break;
			*/
			case BOE_ADJ_GetVersion : //GET_DDCCI_VER_NEW={6E 51 83 01 FB FE B8 }
            {
				DDC2BI_PKTSIZE = 0x84;
                DDC2BI_REPLYBYTE = 0x02;
                DDC2BI_REPLYBYTE_OPCODE = BOE_ADJ_GetVersion;
                DDC2BI_REPLYBYTE_DATA0 = FWVersion[0];
                DDC2BI_REPLYBYTE_DATA1 = FWVersion[2];
                SET_DFM_WRITECMD_REPLY_FLAG();
            }
            break;

			case BOE_ADJ_GetBOEModelName : //Get BOE Model Name={6E 51 83 01 FB FD BB }
			{
                DDC2BI_PKTSIZE = 0x8E;
                DDC2BI_REPLYBYTE = 0x02;
                DDC2BI_REPLYBYTE_OPCODE = BOE_ADJ_GetBOEModelName;
                DDC2BI_REPLYBYTE_DATA0 = FwPartNo[0];
                DDC2BI_REPLYBYTE_DATA1 = FwPartNo[1];
                DDC2BI_REPLYBYTE_DATA2 = FwPartNo[2];
                DDC2BI_REPLYBYTE_DATA3 = FwPartNo[3];
                DDC2BI_REPLYBYTE_DATA4 = FwPartNo[4];
                DDC2BI_REPLYBYTE_DATA5 = FwPartNo[5];
                DDC2BI_REPLYBYTE_DATA6 = FwPartNo[6];
                DDC2BI_REPLYBYTE_DATA7 = FwPartNo[7];
                DDC2BI_REPLYBYTE_DATA8 = FwPartNo[8];
                DDC2BI_REPLYBYTE_DATA9 = FwPartNo[9];
				DDC2BI_REPLYBYTE_DATA10 = FwPartNo[10];
				DDC2BI_REPLYBYTE_DATA11 = FwPartNo[11];
				SET_DFM_WRITECMD_REPLY_FLAG();
			}
            break;

            default:
                break;
        }
    }

    return 0;
}
//=============================================================================================================================================================================================================================:

