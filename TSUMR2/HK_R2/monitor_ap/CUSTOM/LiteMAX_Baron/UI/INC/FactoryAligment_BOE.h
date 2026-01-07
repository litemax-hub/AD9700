//=============================================================================================================================================================================================================================:
#ifdef _FactoryAligment_C_
#define _FactoryAligment_C_
#else
#define _FactoryAligment_C_                                     extern
#endif
//=============================================================================================================================================================================================================================:
#ifdef _FactoryAligment_C__
BYTE XDATA g_ucProductionFlag = 0;
WORD XDATA g_wFDCRCValue = 0;
BYTE XDATA g_ucForceOnlyRunDDCCI = 0;
BYTE xdata EDID_SRC;
#else
extern BYTE XDATA g_ucProductionFlag;
extern WORD XDATA g_wFDCRCValue;
extern BYTE XDATA g_ucForceOnlyRunDDCCI;
extern BYTE xdata EDID_SRC;

#endif //_FactoryAligment_C__

#define PRODUCT_MODE_ON_VALUE      0xBB66
#define PRODUCT_MODE_OFF_VALUE     0x66BB
//=============================================================================================================================================================================================================================:
typedef enum
{
	DEVICE_VGA1,      			// 0
    DEVICE_DVI1,        		// 1
    DEVICE_HDMI1,   			// 2
    DEVICE_DP1,         		// 3
    DEVICE_VGA2,        		// 4
    DEVICE_DVI2,        		// 5
    DEVICE_HDMI2,   			// 6
    DEVICE_DP2,         		// 7
    DEVICE_HDMI3,   			// 8
    DEVICE_HDMI4,   			// 9
    DEVICE_HDMI1_2,				// A
    DEVICE_HDMI2_2,				// B
    DEVICE_TYPE_C1,     		// C
    DEVICE_TYPE_C2,     		// D
    DEVICE_HDMI1_3,				// E
    DEVICE_HDMI2_3,				// F
    DEVICE_DP1_2,       		// 10
    DEVICE_DP2_2,       		// 11
    DEVICE_TYPE_C1_2,   		// 12
    DEVICE_HDMI3_2,   			// 13
    DEVICE_DP1_3,       		// 14
    DEVICE_OSDSN = 0xA0,		// A0 //Serial Number
	DEVICE_SERVICE_TAG = 0xA1,	// A1 //Service Tag
	DEVICE_FACTORY_SN = 0xFE,	// FE //Factory Serial Number
    DEVICE_NUMS = 0xFF
} drvFactoryDeviceType;

typedef enum drvFactoryModeType
{
    FACTORY_MODE_OFF,
    FACTORY_MODE_ON,
    FACTORY_MODE_MAX
} drvFactoryModeType;
//=============================================================================================================================================================================================================================:
typedef enum
{
    VGA1_EDID  		= BIT15,
    DVI1_EDID  		= BIT14,
    HDMI1_EDID 		= BIT13,
    DP1_EDID   		= BIT12,
    VGA2_EDID  		= BIT11,
    DVI2_EDID  		= BIT10,
    HDMI2_EDID 		= BIT9,
    DP2_EDID   		= BIT8,
    HDMI3_EDID 		= BIT7,
    HDMI4_EDID 		= BIT6,
    HDMI1_2_EDID 	= BIT5,
    HDMI2_2_EDID 	= BIT4,
    TYPE_C1_EDID 	= BIT3,
    INMPUT_EX 		= BIT2,
    EDID_IN_FLASH 	= BIT0,
    
	TYPE_C2_EDID  	= BIT15,
    HDMI1_3_EDID  	= BIT14,
    HDMI2_3_EDID  	= BIT13,
    DP1_2_EDID     	= BIT12,
	DP2_2_EDID     	= BIT11,
	TYPE_C1_2_EDID 	= BIT7,
    HDMI3_2_EDID 	= BIT6,
    ISP_SN     		= BIT5,  //steve_20190628_1
    SERVICE_TAG		= BIT4,
    DP1_3_EDID 		= BIT3,
} drvFactoryInputPortEDID;

#define INMPUT_EX_3        0x00000600  

//=============================================================================================================================================================================================================================:
#define LanTChineseCode                                         0x01
#define LanEnglistCode                                          0x02
#define LanFrenchCode                                           0x03
#define LanGermanCode                                           0x04
#define LanItalianCode                                          0x05
#define LanJapaneseCode                                         0x06
#define LanKoreanCode                                           0x07
#define LanPortuguesePortugalCode                               0x08
#define LanRussianCode                                          0x09
#define LanSpanishCode                                          0x0A
#define LanSwedishCode                                          0x0B
#define LanTurkishCode                                          0x0C
#define LanSChineseCode                                         0x0D
#define LanPortugueseBrazilCode                                 0x0E
#define LanCzechCode                                            0x12
#define LanDanskCode                                            0x13
#define LanDutchCode                                            0x14
#define LanFinnishCode                                          0x16
#define LanNorskCode                                            0x1D
#define LanPolishCode                                           0x1E
#define LanHunggarianCode                                       0x20
#define LanHebraicCode                                          0x21
#define LanArabicCode                                           0x22

#define VCP_INPUT_SOURCE_VGA                                    0x01
#define VCP_INPUT_SOURCE_DVI1                                   0x03
#define VCP_INPUT_SOURCE_DVI2                                   0x04
#define VCP_INPUT_SOURCE_DP1                                    0x0F
#define VCP_INPUT_SOURCE_DP2                                    0x10
#define VCP_INPUT_SOURCE_HDMI1                                  0x11
#define VCP_INPUT_SOURCE_HDMI2                                  0x12
#define VCP_INPUT_SOURCE_HDMI3                                  0x13
//=============================================================================================================================================================================================================================:
_FactoryAligment_C_ WORD CRC16 ( BYTE nData, WORD nCRC );
_FactoryAligment_C_ Bool drvFactoryAlignBOE(void);
//=============================================================================================================================================================================================================================:
#define DDC2BI_PKTSIZE                                          DDCBuffer[0]
#define DDC2BI_CMD                                              DDCBuffer[1]
#define DDC2BI_FACTORYCODE                                      DDCBuffer[2]
#define DDC2BI_OPCODE                                           DDCBuffer[3]
#define DDC2BI_DATA0                                            DDCBuffer[4]
#define DDC2BI_DATA1                                            DDCBuffer[5]
#define DDC2BI_DATA2                                            DDCBuffer[6]
#define DDC2BI_DATA3                                            DDCBuffer[7]
#define DDC2BI_BOEALUE                                          DDCBuffer[4]
#define DDC2BI_VALUEH                                           DDCBuffer[5]
#define DDC2BI_VALUEL                                           DDCBuffer[6]
#define DDC2BI_RETVALUEH                                        DDCBuffer[7]
#define DDC2BI_RETVALUEL                                        DDCBuffer[8]
#define ENABLE_NEWDFM_REQUEST                                   1
#if ENABLE_NEWDFM_REQUEST
#define DDC2BI_REPLYBYTE                                        DDCBuffer[1]
#define DDC2BI_FDCRCHI                                          DDCBuffer[2]
#define DDC2BI_FDCRCLO                                          DDCBuffer[3]
#define DDC2BI_FDCRC_HDMI20                                     DDCBuffer[4]
#define DDC2BI_DEVICEADDRLEN                                    DDCBuffer[1]
#define DDC2BI_DEVICEADDRPORT1                                  DDCBuffer[2]
#define DDC2BI_DEVICEADDRPORT2                                  DDCBuffer[3]
#define DDC2BI_DEVICEADDRPORT3                                  DDCBuffer[4]
#define DDC2BI_DEVICEADDRPORT4                                  DDCBuffer[5]
#endif

#define DDC2BI_REPLYBYTE_OPCODE         						DDCBuffer[2]
#define DDC2BI_REPLYBYTE_DATA0									DDCBuffer[3]
#define DDC2BI_REPLYBYTE_DATA1									DDCBuffer[4]
#define DDC2BI_REPLYBYTE_DATA2									DDCBuffer[5]
#define DDC2BI_REPLYBYTE_DATA3									DDCBuffer[6]
#define DDC2BI_REPLYBYTE_DATA4									DDCBuffer[7]
#define DDC2BI_REPLYBYTE_DATA5									DDCBuffer[8]
#define DDC2BI_REPLYBYTE_DATA6									DDCBuffer[9]
#define DDC2BI_REPLYBYTE_DATA7									DDCBuffer[10]
#define DDC2BI_REPLYBYTE_DATA8									DDCBuffer[11]
#define DDC2BI_REPLYBYTE_DATA9									DDCBuffer[12]
#define DDC2BI_REPLYBYTE_DATA10									DDCBuffer[13]
#define DDC2BI_REPLYBYTE_DATA11									DDCBuffer[14]

#define CmdIndex                                                3
#define SIO_ID1                                                 DDCBuffer[0+CmdIndex]
#define SIO_TYPE1                                               DDCBuffer[1+CmdIndex]
#define SIO_TYPE2                                               DDCBuffer[2+CmdIndex]
#define SIO_CMD1                                                DDCBuffer[3+CmdIndex]
#define SIO_CMD2                                                DDCBuffer[4+CmdIndex]
#define SIO_DATA1                                               DDCBuffer[5+CmdIndex]
#define SIO_DATA2                                               DDCBuffer[6+CmdIndex]
#define SIO_DATA3                                               DDCBuffer[7+CmdIndex]
#define BOE_GETVCP                                              0x01
#define BOE_SETVCP                                              0x03
#define BOE_VCPFeature                                          0xFC

#define BOE_FCode0A                                             0xEA
#define BOE_PowerLogo                                           0xA7

#define BOE_HDCPReadWriteStart                                  0xE0
#define BOE_PowerMode                                           0xD6
#define BOE_Language                                            0xCC
#define BOE_MIN_RGB_GAIN                                        80
#define BOE_MAX_RGB_GAIN                                        148

typedef enum drvFactoryColorTempType
{
    FACTORY_COLOR_4500K = 1,
    FACTORY_COLOR_5000K,
    FACTORY_COLOR_5400K,
    FACTORY_COLOR_6500K,
    FACTORY_COLOR_7500K,
    FACTORY_COLOR_9300K,
    FACTORY_COLOR_USER,
    FACTORY_COLOR_SRGB,
    FACTORY_COLOR_NATIVE,
    FACTORY_COLOR_8200K,
    FACTORY_COLOR_10000K,
    FACTORY_COLOR_11500K,
    FACTORY_COLOR_SAVE,
    FACTORY_COLOR_MAX = 0x14
} drvFactoryColorTempType;
//BOE 0xFB Command----------------------------------------------------------------------------------
#define BOE_OSD_Reset                                         	0x04
#define BOE_ADJ_Brightness                                      0x10
#define BOE_ADJ_Contrast                                        0x12
//#define ADJ_Contrast                                            BOE_ADJ_Contrast
#define BOE_Select_Color_Preset                                 0x14
#define BOE_ADJ_Red_Gain                                        0x16
#define BOE_ADJ_Green_Gain                                      0x18
#define BOE_ADJ_Blue_Gain                                       0x1A
#define BOE_Auto_Setup                                          0x1E //no use
#define BOE_Auto_Color                                          0x1F

#define BOE_ADJ_Input_Source                                    0x60
#define BOE_ADJ_Volume                                          0x62
#define BOE_ADJ_Language                                        0xCC
#define BOE_PowerMode                                         	0xD6
 #define BOE_PowerMode_PowerSaving                              	0x02
 #define BOE_PowerMode_PowerOff                              		0x05
#define BOE_EEPROM_INIT                                         0xE0
#define BOE_ENTER_FACTORY                                       0xE1
#define BOE_EXIT_FACTORY                                        0xE2
#define BOE_ENTER_BURNIN                                        0xE4
#define BOE_SAVE                                                0xE5
#define BOE_RESET_FACTORY_DATA                                  0xE6

#define BOE_FCode01                                             0xF0
#define BOE_FCode01_F1_EDID_ISP                                 0x01
#define BOE_FCode01_F1_EDID_WP                                  0x02
#define BOE_FCode01_F1_PRODUCT_MODE                             0x03
#define BOE_AUTO_COLOR_OFFSET                                   0xF6
#define BOE_AUTO_COLOR_GAIN                                     0xF7
#define BOE_EDID_DATA_TRANSMIT                                  0xF8
#define BOE_ADJ_BRIGHTNESS_CURVE              					0xFC
	#define BOE_ADJ_Brightness_LID              					0x00 //LID_DC=0, LID_PWM=1
	#define BOE_ADJ_Brightness_PWM0              					0x01 //no use
	#define BOE_ADJ_Brightness_PWM25              					0x02 //no use
	#define BOE_ADJ_Brightness_PWM50              					0x03 //no use
	#define BOE_ADJ_Brightness_PWM75              					0x04
	#define BOE_ADJ_Brightness_PWM100              					0x05 //no use
	#define BOE_ADJ_Brightness_PWM0_Auto              				0x06
	#define BOE_ADJ_Brightness_PWM100_Auto              			0x07
	#define BOE_ADJ_Brightness_DC0              					0x10
	#define BOE_ADJ_Brightness_DC25              					0x11
	#define BOE_ADJ_Brightness_DC50              					0x12
	#define BOE_ADJ_Brightness_DC75              					0x13
	#define BOE_ADJ_Brightness_DC100              					0x14
#define BOE_ADJ_GetBOEModelName                                 0xFD
#define BOE_ADJ_GetVersion              						0xFE
//BOE 0xFB Command-end------------------------------------------------------------------------------
#define PRODUCTION_MODE_BIT                                     _BIT0
#define DFM_POWERSAVING_BIT                                     _BIT1
#define DFM_WRITECMD_REPLY_BIT                                  _BIT2
#define DFM_READ32BYTECMD_REPLY_BIT                             _BIT3

#define DFM_POWERSAVING_FLAG                                    (g_ucProductionFlag&DFM_POWERSAVING_BIT)
#define SET_DFM_POWERSAVING_FLAG()                              (g_ucProductionFlag|=DFM_POWERSAVING_BIT)
#define CLR_DFM_POWERSAVING_FLAG()                              (g_ucProductionFlag&=~DFM_POWERSAVING_BIT)
#define DFM_WRITECMD_REPLY_FLAG                                 (g_ucProductionFlag&DFM_WRITECMD_REPLY_BIT)
#define SET_DFM_WRITECMD_REPLY_FLAG()                           (g_ucProductionFlag|=DFM_WRITECMD_REPLY_BIT)
#define CLR_DFM_WRITECMD_REPLY_FLAG()                           (g_ucProductionFlag&=~DFM_WRITECMD_REPLY_BIT)
#define DFM_READ32BYTECMD_REPLY_FLAG                            (g_ucProductionFlag&DFM_READ32BYTECMD_REPLY_BIT)
#define SET_DFM_READ32BYTECMD_REPLY_FLAG()                      (g_ucProductionFlag|=DFM_READ32BYTECMD_REPLY_BIT)
#define CLR_DFM_READ32BYTECMD_REPLY_FLAG()                      (g_ucProductionFlag&=~DFM_READ32BYTECMD_REPLY_BIT)

//=============================================================================================================================================================================================================================:
//_FactoryAligment_H_
//=============================================================================================================================================================================================================================:
