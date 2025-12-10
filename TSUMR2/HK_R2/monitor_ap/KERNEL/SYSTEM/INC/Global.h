#ifndef __GLOBAL_H__
#define __GLOBAL_H__
#include "board.h"
#include "UserPrefDef.h"
#include "Mode.h"

///////////////////////////////////////////////////////
#include <math.h>
#include <stdlib.h> //abs()
//#include <intrins.h>
#include "types.h"
#include "Debug.h"
#include "Common.h"

#include "Ms_rwreg.h"
#include "ms_reg.h"
#include "REG52.H"
#include "i2cdef.h"

#include "misc.h"
#include "mStar.h"
#include "menudef.h"
#include "drvADC.h"
#include "drvGPIO.h"

#include "halRwreg.h"
#include "drvMcu.h"
#include "drvAdjust.h"
#include "drvDDC2Bi.h"
//#include "drvPower.h" // the function moves to drvmStar
#include "msFB.h"
#include "drvmStar.h"
#include "msScaler.h"
#if ENABLE_RTE
#include "msOVD.h"
#include "drvmsOVD.h"
#endif
#include "appmStar.h"
#if ENABLE_TOUCH_PANEL
#include "touchpanel.h"
#endif
#if ENABLE_TOUCH_KEY
#include "touchkey.h"
#endif
#include "Ms_PM.h"
#include "ComboApp.h"
#include "drvUART.h"
extern volatile BOOL g_bEnableIsrDisableFlock;
extern volatile BOOL g_bOutputVsyncIsrFlag;

#if ENABLE_MENULOAD || ENABLE_MENULOAD_2
#include "msMenuload.h"
extern BYTE xdata u8MenuLoadFlag[ML_DEV_NUM];
extern BOOL bMLEnable;
#endif

#if ENABLE_DYNAMICSCALING
extern BYTE u8DSFlag;
#endif

#if ENABLE_WATCH_DOG
extern BYTE xdata u8WDT_Enable;
#endif

#if ENABLE_WATCH_DOG_INT
extern BYTE xdata u8WDT_Status;
#endif

#if EnableTime1Interrupt
extern  WORD XDATA g_u16MsCounter1; // for System timer
extern  BOOL XDATA bSecondTimer1;
extern  BYTE XDATA u8SecondTIMER1;
#endif

#define DEF_BurninChangePatternTime 7 // unit: Second
extern BYTE  xdata g_BurninChangePatternSec;

#define CURRENT_IS_FBL() (g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_0)

//#if ENABLE_DPS
extern idata BOOL bRunToolFlag;
//#endif


#if ENABLE_MSPI_FLASH_ACCESS
//extern DWORD gsystem_time_ms;
#endif

typedef enum {
    PM_INIT_TS,
    AP_INIT_TS,
    AP_LOOP_TS,
    MSTAR_INIT_TS,
    POWER_ON_TS,
    MAX_TS,
} TIME_STAMP_TYPE;
#if ENABLE_BOOT_TIME_PROFILING // use for measure system boot time from AC/DC/DMPS to displayOK
#define _TIME_STAMP_NUM_MAX_        25
extern MS_U32 g_u32TimeStamp[MAX_TS][_TIME_STAMP_NUM_MAX_];
#endif

extern DWORD DPTx_time_ms;

#if ENABLE_HPERIOD_16LINE_MODE
extern BOOL g_16ModePre;
#endif

//------------------------------
//
//  MBX, Need to sync w/ PQ_R2
//
//------------------------------
#if TTS
#define MBX_TTS_BUF_START     REG_1033E0 // 4-byte
#define MBX_TTS_BUF_END       MBX_TTS_BUF_START + 4 // 4-byte
#define MBX_TTS_STR_STATUS    MBX_TTS_BUF_END + 4 // 1-byte // 0x00: PQ_R2, 0x01: HK_R2
#endif

#if ENABLE_PQ_R2
#if TTS
#define MBX_MLOAD_2_START       (MBX_TTS_STR_STATUS + 1) // 4-byte
#else
#define MBX_MLOAD_2_START       REG_1033E0 // 4-byte
#endif
#define MBX_MLOAD_2_SIZE        4 // 4-byte
#if ENABLE_DOLBY_HDR
#define MBX_DV_BUF_START            (MBX_MLOAD_2_START+MBX_MLOAD_2_SIZE)
#define MBX_DV_BUF_SIZE             4
#else
#define MBX_DV_BUF_SIZE             0
#endif
#endif
// --- Need to sync w/ PQ_R2 end

#if (CHIP_ID == CHIP_MT9700)
#define XDATA_DDC_ADDR_START    0x880//XDATA FIFO: 2688 ~ 3711, MST9U5 moved the DDC FIFO buffer from xData to EDID sram.
#define XDATA_DDC_TX_ADDR_START  XDATA_DDC_ADDR_START + D2B_FIFO_XdataSize
#elif  (CHIP_ID == CHIP_MT9701)
#define XDATA_DDC_ADDR_START    0x600
#define XDATA_DDC_TX_ADDR_START  XDATA_DDC_ADDR_START + D2B_FIFO_XdataSize
#else
#define XDATA_DDC_ADDR_START    XDATA_SWDB_ADDR_START + 0x100 // DDC max = 256B*5=0x500, 0X4C00-0x0500=0x4700, In current driver code, maximum 256 Bytes needed.
#define XDATA_DDC_TX_ADDR_START  XDATA_DDC_ADDR_START + D2B_FIFO_XdataSize //
#endif

///////////////////////////////////////////////////////
#define CURRENT_INPUT_IS_VGA()          (SrcInputType == Input_VGA)
#define CURRENT_INPUT_IS_DVI()          ((SrcInputType == Input_DVI)||(SrcInputType == Input_DVI2)||(SrcInputType == Input_DVI3)||(SrcInputType == Input_DVI4)||(SrcInputType == Input_DualDVI)||(SrcInputType == Input_DualDVI2))
#define CURRENT_INPUT_IS_HDMI()         ((SrcInputType == Input_HDMI)||(SrcInputType == Input_HDMI2)||(SrcInputType == Input_HDMI3)||(SrcInputType == Input_HDMI4))
#define CURRENT_INPUT_IS_DISPLAYPORT()  ((SrcInputType == Input_Displayport)||(SrcInputType == Input_Displayport2)||(SrcInputType == Input_Displayport3)||(SrcInputType == Input_Displayport4)||(SrcInputType == Input_UsbTypeC_C3)||(SrcInputType == Input_UsbTypeC_C4))
#define CURRENT_INPUT_IS_USBTYPEC()     ((SrcInputType == Input_UsbTypeC3)||(SrcInputType == Input_UsbTypeC4))
#define CURRENT_INPUT_IS_TMDS()         (CURRENT_INPUT_IS_DVI() || CURRENT_INPUT_IS_HDMI())

#if ENABLE_DP_INPUT
#define CURRENT_INPUT_IS_DISPLAYPORT_PortC() ((SrcInputType == Input_Displayport3))
#define CURRENT_INPUT_IS_DISPLAYPORT_PortB() ((SrcInputType == Input_Displayport))
#define ENABLED_DISPLAYPORT   (( ((Input_Displayport >= Input_Nums)?0:(BIT0<<Input_Displayport))|((Input_Displayport2 >= Input_Nums)?0:(BIT0<<Input_Displayport2))|((Input_Displayport3 >= Input_Nums)?0:(BIT0<<Input_Displayport3))|((Input_Displayport4 >= Input_Nums)?0:(BIT0<<Input_Displayport4)) )|\
                                ( ((Input_UsbTypeC3 >= Input_Nums)?0:(BIT0<<Input_UsbTypeC3))|((Input_UsbTypeC4 >= Input_Nums)?0:(BIT0<<Input_UsbTypeC4))))
#endif

#define INPUT_IS_VGA(InputType)         (InputType == Input_VGA)
#define INPUT_IS_DVI(InputType)         ((InputType == Input_DVI)||(InputType == Input_DVI2)||(InputType == Input_DVI3)||(InputType == Input_DVI4)||(InputType == Input_DualDVI)||(InputType == Input_DualDVI2))
#define INPUT_IS_HDMI(InputType)        ((InputType == Input_HDMI)||(InputType == Input_HDMI2)||(InputType == Input_HDMI3)||(InputType == Input_HDMI4))
#define INPUT_IS_DISPLAYPORT(InputType) ((InputType == Input_Displayport)||(InputType == Input_Displayport2)||(InputType == Input_Displayport3)||(InputType == Input_Displayport4))
#define INPUT_IS_USBTYPEC(InputType)    ((InputType == Input_UsbTypeC3)||(InputType == Input_UsbTypeC4))
#define INPUT_IS_USBTYPEC2(InputType)   (InputType == Input_UsbTypeC3)
#define INPUT_IS_USBTYPEC3(InputType)   (InputType == Input_UsbTypeC4)
#define INPUT_IS_TMDS(InputType)        (INPUT_IS_DVI(InputType) || INPUT_IS_HDMI(InputType))
#define INPUT_IS_TMDS0(InputType)       ((InputType == Input_HDMI) || (InputType == Input_DVI))
#define INPUT_IS_TMDS1(InputType)       ((InputType == Input_HDMI2) || (InputType == Input_DVI2))
#define INPUT_IS_TMDS2(InputType)       ((InputType == Input_HDMI3) || (InputType == Input_DVI3))
#define INPUT_IS_TMDS3(InputType)       ((InputType == Input_HDMI4) || (InputType == Input_DVI4))

#define CURRENT_INPUT_IS_HDMI0()    (SrcInputType == Input_HDMI)
#define CURRENT_INPUT_IS_HDMI1()    (SrcInputType == Input_HDMI2)
#define CURRENT_INPUT_IS_HDMI2()    (SrcInputType == Input_HDMI3)
#define CURRENT_INPUT_IS_HDMI3()    (SrcInputType == Input_HDMI4)

#define CURRENT_INPUT_IS_DVI0()     (SrcInputType == Input_DVI)
#define CURRENT_INPUT_IS_DVI1()     (SrcInputType == Input_DVI2)
#define CURRENT_INPUT_IS_DVI2()     (SrcInputType == Input_DVI3)
#define CURRENT_INPUT_IS_DVI3()     (SrcInputType == Input_DVI4)

#define CURRENT_INPUT_IS_DISPLAYPORT0()    (SrcInputType == Input_Displayport)
#define CURRENT_INPUT_IS_DISPLAYPORT1()    (SrcInputType == Input_Displayport2)
#define CURRENT_INPUT_IS_DISPLAYPORT2()    (SrcInputType == Input_Displayport3)
#define CURRENT_INPUT_IS_DISPLAYPORT3()    (SrcInputType == Input_Displayport4)

#define CURRENT_INPUT_IS_USBTYPEC2()     (SrcInputType == Input_UsbTypeC3)
#define CURRENT_INPUT_IS_USBTYPEC3()     (SrcInputType == Input_UsbTypeC4)


#define EXPANSION_MODE_IS_FULL()    (UserprefExpansionMode == Expansion_Full)

#define COLOR_SPACE_IS_YUV()        (!IsColorspaceRGB())
#define CURRENT_SOURCE_IS_INTERLACE_MODE()  (SrcFlags & bInterlaceMode)

#define SOURCE_FORMAT_IN_NORMAL()   (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_NORMAL)

#define V_FREQ_IN   ((SrcVFreq+5)/10)
#define IS_INPUT_VFREQ_IN_PANEL_RANGE()             (SrcVFreq >= g_sPnlInfo.sPnlTiming.u16VFreqMin && SrcVFreq <= g_sPnlInfo.sPnlTiming.u16VFreqMax)
#define IS_INPUT_VFREQ_24Hz()                       (SrcVFreq >= 235 && SrcVFreq <= 245) // need this case for 60Hz panel
#define IS_INPUT_VFREQ_DOUBLE_IN_PANEL_RANGE()      ((V_FREQ_IN*2) >= (g_sPnlInfo.sPnlTiming.u16VFreqMin/10) && (V_FREQ_IN*2) <= (g_sPnlInfo.sPnlTiming.u16VFreqMax/10))
#define IS_INPUT_VFREQ_TRIPLE_IN_PANEL_RANGE()      ((V_FREQ_IN*3) >= (g_sPnlInfo.sPnlTiming.u16VFreqMin/10) && (V_FREQ_IN*3) <= (g_sPnlInfo.sPnlTiming.u16VFreqMax/10))
#define INPUT_TIMING_EXECUTE_FRAME_PLL_LOCK()       (!IS_INPUT_VFREQ_IN_PANEL_RANGE() && \
                                                      (IS_INPUT_VFREQ_24Hz() || \
                                                      IS_INPUT_VFREQ_DOUBLE_IN_PANEL_RANGE() || \
                                                      IS_INPUT_VFREQ_TRIPLE_IN_PANEL_RANGE()))
///////////////////////////////////////////////////////
extern BYTE  xdata g_SwitchSec;
extern BYTE  xdata g_CountSwitchPortTimeFlag;

//------Eson Start--------------------------------
extern xdata BYTE OsdFontColor;
#if CHIP_ID>=CHIP_TSUMV
#define CPC_Black       0x0F
#else
#define CPC_Black       0x00
#endif
#define CPC_White       0x01
#define CPC_Red         0x02
#define CPC_Gray        0x03
#define CPC_MTK_Gold    0x04
#define CPC_Yellow      0x05
#define CPC_Green       0x06
#define CPC_Blue        0x07

#define CPC_SmallLogo_4C        0x10

#define LayerXSize      18
#define Layer1XPos      1
#define Layer2XPos      (1+Layer1XPos+LayerXSize)
#define Layer3XPos      (1+Layer2XPos+LayerXSize)
#define LayerYPos       3
#define SaperationYPos       (LayerYPos+1)

#define HotLayerXPos      2
#define HotLayerYPos      4

#if (LiteMAX_UI == 1)
#define GaugeXPos           7
#define GaugeLens           18
#define RGBGaugeXPos        5
#define RGBGaugeLens        20
#define NumberXPos          (GaugeXPos+12)

#define MonoFontStart       0x00
#define SpaceFont           (MonoFontStart+0x01)
#define Saperation_2C       (MonoFontStart+0x02)
#define TopLine_2C          (MonoFontStart+0x03)
#define TopBottom_2C        (MonoFontStart+0x04)
#define BottomLine_2C       (MonoFontStart+0x05)
#define GaugeSelect_2C      (MonoFontStart+0x06)
#define GaugeNormal_2C      (MonoFontStart+0x07)

#define CursorFontStart     (Mark_2C+0x01)
#define CursorLT_2C     	(CursorFontStart+0x00)
#define CursorTop_2C    	(CursorFontStart+0x01)
#define CursorRT_2C     	(CursorFontStart+0x02)
#define CursorLeft_2C   	(CursorFontStart+0x03)
#define CursorRight_2C  	(CursorFontStart+0x04)
#define CursorLD_2C     	(CursorFontStart+0x05)
#define CursorDown_2C   	(CursorFontStart+0x06)
#define CursorRD_2C     	(CursorFontStart+0x07)

#define PropFontAddr1       0x20
#define PropFontAddr2       0x80

///////////////////////////////////////////////////////////////////////////////////////
//
//Memory Mapping Common Use - mono
//
///////////////////////////////////////////////////////////////////////////////////////
#define COMMON_FONT_START               0x00

#define NumberFontStart					(COMMON_FONT_START+0x02) // 0x02 ~ 0x09
#define Mark_2C             			(COMMON_FONT_START+0x16) // 0x16
#define	GaugeFontStart					(COMMON_FONT_START+0x17) // 0x17 ~ 0x1F
	#define GaugeFont_EdgeL					(GaugeFontStart+0x00)
	#define GaugeFont_EdgeR  				(GaugeFontStart+0x01)
	#define GaugeFont0_6    				(GaugeFontStart+0x02)
	#define GaugeFont1_6    				(GaugeFontStart+0x03)
	#define GaugeFont2_6    				(GaugeFontStart+0x04)
	#define GaugeFont3_6    				(GaugeFontStart+0x05)
	#define GaugeFont4_6    				(GaugeFontStart+0x06)
	#define GaugeFont5_6    				(GaugeFontStart+0x07)
	#define GaugeFont6_6    				(GaugeFontStart+0x08)
///////////////////////////////////////////////////////////////////////////////////////
//Memory Mapping under 0x100 - mono
///////////////////////////////////////////////////////////////////////////////////////
//Mono
#define PROP_FONT_START                 0x20
#define S0_PROP_FONT_START              PROP_FONT_START //for factory menu compatiable wit dual OSD engine

//Static
#define SM_CHECK_2C_FONT_START          0xFC // 2fonts // 0XFC~0xFD: Tick icon, 0xFE~0xFF: Return icon

///////////////////////////////////////////////////////////////////////////////////////
//Memory Mapping between 0x100 ~ 0x200 - mono
///////////////////////////////////////////////////////////////////////////////////////
#define MM_PROP_RESOLUTION_INFO_START   0x00    //Current timing
#define MM_PROP_PIXELCLOCK_INFO_START   0x10    //Pixel clock
#define MM_PROP_FONT_START              0x20    //L0 prop font end=8Dh

///////////////////////////////////////////////////////////////////////////////////////
//Memory Mapping between 0x200 ~ 0x300 - 4Color Icon
///////////////////////////////////////////////////////////////////////////////////////
#define _4ColorFontStart    0x200
#define SmallLogo_4C        _4ColorFontStart
// Frame                                   
#define Space4C                            _4ColorFontStart
#define Frame4C_LT                         (_4ColorFontStart+(1*2))
#define Frame4C_Top                        (_4ColorFontStart+(2*2))
#define Frame4C_RT                         (_4ColorFontStart+(3*2))
#define Frame4C_Left                       (_4ColorFontStart+(4*2))
#define Frame4C_Right                      (_4ColorFontStart+(5*2))
#define Frame4C_LB                         (_4ColorFontStart+(6*2))
#define Frame4C_Buttom                     (_4ColorFontStart+(7*2))
#define Frame4C_RB                         (_4ColorFontStart+(8*2))
#define Frame_Size                         9
// FrameNew                                
#define FrameNew4CFontStart                (Space4C+(Frame_Size*2))
#define FrameNew4C_LT                      (_4ColorFontStart+(9*2))
#define FrameNew4C_LT2                     (_4ColorFontStart+(10*2))
#define FrameNew4C_RT                      (_4ColorFontStart+(11*2))
#define FrameNew4C_Top                     (_4ColorFontStart+(12*2))
#define FrameNew4C_Top2                    (_4ColorFontStart+(13*2))
#define FrameNew_Size                      5
// Main Icon
#define MainIcon4C                         (Space4C+((Frame_Size+FrameNew_Size)*2))
#define MainIcon4C_0_MainMenuIcon          MainIcon4C
#define MainIcon4C_0_MainMenuIcon_Size     7*6
#define MainIcon4C_1_BrightnessSub         (MainIcon4C_0_MainMenuIcon+(MainIcon4C_0_MainMenuIcon_Size*2))
#define MainIcon4C_1_BrightnessSub_Size    6*6
#define MainIcon4C_2_AudioSub              (MainIcon4C_1_BrightnessSub+(MainIcon4C_1_BrightnessSub_Size*2))
#define MainIcon4C_2_AudioSub_Size         2*6
#define MainIcon4C_3_ColorSub              (MainIcon4C_2_AudioSub+(MainIcon4C_2_AudioSub_Size*2))
#define MainIcon4C_3_ColorSub_Size         8*6

#define LuminanceIconStar  				   (MainIcon4C_0_MainMenuIcon)
#define SignalIconStar  				   (MainIcon4C_0_MainMenuIcon+0x0C)
#define SoundIconStar  					   (MainIcon4C_0_MainMenuIcon+0x18)
#define ColorIconStar  					   (MainIcon4C_0_MainMenuIcon+0x24)
#define ImageIconStar  				   	   (MainIcon4C_0_MainMenuIcon+0x30)
#define OtherIconStar  				       (MainIcon4C_0_MainMenuIcon+0x3C)
#define ExitIconStar  				       (MainIcon4C_0_MainMenuIcon+0x48)
///////////////////////////////////////////////////////////////////////////////////////
//Memory Mapping between 0x300 ~ 0xxx - 4Color Icon
///////////////////////////////////////////////////////////////////////////////////////
#define MainIcon4C_4_ImageSub              (MainIcon4C_3_ColorSub+(MainIcon4C_3_ColorSub_Size*2))
#define MainIcon4C_4_ImageSub_Size         3*6

#define MainIcon4C_5_OtherSub              (MainIcon4C_4_ImageSub+(MainIcon4C_4_ImageSub_Size*2))
#define MainIcon4C_5_OtherSub_Size         6*6

#define SmallLogo_Size      39

#define Logo_4C         0x02
#define Logo_Size       120

#elif (LiteMAX_Baron_UI == 1)
#define GaugeXPos           8
#define GaugeLens           20
#define NumberXPos          (GaugeXPos+12)
#define RGBGaugeXPos        9
#define RGBGaugeLens        17

#define MonoFontStart       0x00
#define SpaceFont           (MonoFontStart+0x01)
#define Saperation_2C       (MonoFontStart+0x02)
#define TopLine_2C          (MonoFontStart+0x03)
#define TopBottom_2C        (MonoFontStart+0x04)
#define BottomLine_2C       (MonoFontStart+0x05)
#define GaugeSelect_2C      (MonoFontStart+0x06)
#define GaugeNormal_2C      (MonoFontStart+0x07)

#define CursorFontStart     (Mark_2C+0x01)
#define CursorLT_2C     	(CursorFontStart+0x00)
#define CursorTop_2C    	(CursorFontStart+0x01)
#define CursorRT_2C     	(CursorFontStart+0x02)
#define CursorLeft_2C   	(CursorFontStart+0x03)
#define CursorRight_2C  	(CursorFontStart+0x04)
#define CursorLD_2C     	(CursorFontStart+0x05)
#define CursorDown_2C   	(CursorFontStart+0x06)
#define CursorRD_2C     	(CursorFontStart+0x07)


///////////////////////////////////////////////////////////////////////////////////////
//
//Memory Mapping Common Use - mono
//
///////////////////////////////////////////////////////////////////////////////////////
#define COMMON_FONT_START               0x00
#define COMMON_FONT_2ND_START           0xC0

//Static
#define SM_CHECK_2C_FONT_START          0xFC // 2fonts // 0XFC~0xFD: Tick icon, 0xFE~0xFF: Return icon

#define NumberFontStart					(COMMON_FONT_START+0x02) // 0x02 ~ 0x09
#define Mark_2C             			(COMMON_FONT_START+0x16) // 0x16
#define	GaugeFontStart					(COMMON_FONT_START+0x17) // 0x17 ~ 0x1F
	#define GaugeFont_EdgeL					(GaugeFontStart+0x00)
	#define GaugeFont_EdgeR  				(GaugeFontStart+0x01)
	#define GaugeFont0_6    				(GaugeFontStart+0x02)
	#define GaugeFont1_6    				(GaugeFontStart+0x03)
	#define GaugeFont2_6    				(GaugeFontStart+0x04)
	#define GaugeFont3_6    				(GaugeFontStart+0x05)
	#define GaugeFont4_6    				(GaugeFontStart+0x06)
	#define GaugeFont5_6    				(GaugeFontStart+0x07)
	#define GaugeFont6_6    				(GaugeFontStart+0x08)
///////////////////////////////////////////////////////////////////////////////////////
//Memory Mapping under 0x100 - mono
///////////////////////////////////////////////////////////////////////////////////////
//Mono
#define PropFontAddr1       0x20 //CurrentMenu.Fonts
#define PropFontAddr2       0x80 //CurrentMenuItem.Fonts

///////////////////////////////////////////////////////////////////////////////////////
//Memory Mapping between 0x100 ~ 0x200 - mono
///////////////////////////////////////////////////////////////////////////////////////
#define Logo_4C         	0x101
#define Logo_Size       	120

///////////////////////////////////////////////////////////////////////////////////////
//Memory Mapping between 0x200 ~ 0x300 - 4Color Icon
///////////////////////////////////////////////////////////////////////////////////////
#define _4ColorFontStart    0x200
#define SmallLogo_4C        _4ColorFontStart
#define SmallLogo_Size      39

// Frame                                   
#define Space4C                            _4ColorFontStart
#define Frame4C_LT                         (_4ColorFontStart+(1*2))
#define Frame4C_Top                        (_4ColorFontStart+(2*2))
#define Frame4C_RT                         (_4ColorFontStart+(3*2))
#define Frame4C_Left                       (_4ColorFontStart+(4*2))
#define Frame4C_Right                      (_4ColorFontStart+(5*2))
#define Frame4C_LB                         (_4ColorFontStart+(6*2))
#define Frame4C_Buttom                     (_4ColorFontStart+(7*2))
#define Frame4C_RB                         (_4ColorFontStart+(8*2))
#define Frame_Size                         9
// FrameNew                                
#define FrameNew4CFontStart                (Space4C+(Frame_Size*2))
#define FrameNew4C_LT                      (_4ColorFontStart+(9*2))
#define FrameNew4C_LT2                     (_4ColorFontStart+(10*2))
#define FrameNew4C_RT                      (_4ColorFontStart+(11*2))
#define FrameNew4C_Top                     (_4ColorFontStart+(12*2))
#define FrameNew4C_Top2                    (_4ColorFontStart+(13*2))
#define FrameNew_Size                      5
// Main Icon
#define MainIcon4C                         (Space4C+((Frame_Size+FrameNew_Size)*2))
#define MainIcon4C_0_MainMenuIcon          MainIcon4C
#define MainIcon4C_0_MainMenuIcon_Size     7*6
#define MainIcon4C_1_BrightnessSub         (MainIcon4C_0_MainMenuIcon+(MainIcon4C_0_MainMenuIcon_Size*2))
#define MainIcon4C_1_BrightnessSub_Size    6*6
#define MainIcon4C_2_AudioSub              (MainIcon4C_1_BrightnessSub+(MainIcon4C_1_BrightnessSub_Size*2))
#define MainIcon4C_2_AudioSub_Size         2*6
#define MainIcon4C_3_ColorSub              (MainIcon4C_2_AudioSub+(MainIcon4C_2_AudioSub_Size*2))
#define MainIcon4C_3_ColorSub_Size         8*6

///////////////////////////////////////////////////////////////////////////////////////
//Memory Mapping between 0x300 ~ 0xxx - 4Color Icon
///////////////////////////////////////////////////////////////////////////////////////
#define MainIcon4C_4_ImageSub              (MainIcon4C_3_ColorSub+(MainIcon4C_3_ColorSub_Size*2))
#define MainIcon4C_4_ImageSub_Size         3*6

#define MainIcon4C_5_OtherSub              (MainIcon4C_4_ImageSub+(MainIcon4C_4_ImageSub_Size*2))
#define MainIcon4C_5_OtherSub_Size         6*6

#define MainIcon4C_PowerKeyLock            (MainIcon4C_5_OtherSub+(MainIcon4C_5_OtherSub_Size*2))
#define MainIcon4C_PowerKeyLock_Size       2*6
#define MainIcon4C_LoadDefaultSub          (MainIcon4C_PowerKeyLock+(MainIcon4C_PowerKeyLock_Size*2))
#define MainIcon4C_LoadDefaultSub_Size     2*6

#else //(NEW_MTK_UI == 1)
#define GaugeXPos           (Layer3XPos+0)
#define NumberXPos          (GaugeXPos+12)

#define MonoFontStart       0x00
#define SpaceFont           (MonoFontStart+0x01)
#define Saperation_2C       (MonoFontStart+0x02)
#define TopLine_2C          (MonoFontStart+0x03)
#define TopBottom_2C        (MonoFontStart+0x04)
#define BottomLine_2C       (MonoFontStart+0x05)
#define GaugeSelect_2C      (MonoFontStart+0x06)
#define GaugeNormal_2C      (MonoFontStart+0x07)
#define Mark_2C             (MonoFontStart+0x08)

#define CursorFontStart     (Mark_2C+0x01)
#define CursorLT_2C     	(CursorFontStart+0x00)
#define CursorTop_2C    	(CursorFontStart+0x01)
#define CursorRT_2C     	(CursorFontStart+0x02)
#define CursorLeft_2C   	(CursorFontStart+0x03)
#define CursorRight_2C  	(CursorFontStart+0x04)
#define CursorLD_2C     	(CursorFontStart+0x05)
#define CursorDown_2C   	(CursorFontStart+0x06)
#define CursorRD_2C     	(CursorFontStart+0x07)
#define NumberFontStart 	(CursorRD_2C+0x01)

#define PropFontAddr1       0x20
#define PropFontAddr2       0x80

#define _4ColorFontStart    0x180
#define SmallLogo_4C        _4ColorFontStart
#define SmallLogo_Size      39

#define Logo_4C         0x02
#define Logo_Size       120

#endif

//------Eson End--------------------------------

#define xfr_regs ((BYTE xdata*)0xC000)
//#define HIBYTE(value)             ((BYTE)((value) >> 8))
#define LOBYTE(value)             ((BYTE)(value))

#define CommandFontStart     1
#define FfontStart    0x7F   //100907 aoivd overlap with arrow
#define DrawtTEXTIconStart  0xD2
#define DrawtTEXTIconSize   0x0C

#define Arrow    0xCE   // 100901
#define ArrowSize    0x02   // 100901
// 4Color--------------------------------
#define colorFontStart          0xCE
#define ColorGuageEdgeStar      0xEA
#define ColorGuageEdgeSize      0x02

#define ColorGuageFontStar    0x00//0x100
#define ColorGuageFontSize    0x05

#define SelectFrameStar 0x0A//0x10A
#define SelectFrameSize 0x08

//8Color------------------------------
#define Color_8ColorFontStart    0x1E
#define ColorIcon_8ColorFontStart       0x1E
#if Enable_CascadeMode
#define EXIT_8ColorFontStart                0x19
#define GradulFrame_8ColorFontStart     0x22
#define GrayIcon_8ColorFontStart            0xA3
#define GrayIcon_8ColorFontStart_2          0x00
#define SelectFrame_8ColorFontStart         0x5D// 100907 0x36
#else
#define EXIT_8ColorFontStart                    0x77
#define GradulFrame_8ColorFontStart     0x80
#define GrayIcon_8ColorFontStart            0x01
#define SelectFrame_8ColorFontStart         0xBB//100722 0x94
#endif

//Color Accuracy
#define COLOR_8_BIT         0x08
#define COLOR_10_BIT        0x0A
#define COLOR_ACCURACY      COLOR_8_BIT

extern bit g_bInputSOGFlag;
extern BYTE xdata bInputVGAisYUV;   // (0) VGA is RGB    (1) VGA is YUV //20150121


extern BYTE data g_ucTimer0_TH0;
extern BYTE data g_ucTimer0_TL0;
extern BYTE xdata g_u8SystemSpeedMode;
extern volatile bit g_bMcuPMClock;

extern bit SecondFlag;
extern bit ms10Flag;
extern bit ms50Flag;
extern bit ms500Flag; //
extern bit ms1000Flag; //

extern bit InputTimingChangeFlag;
extern bit g_bDisplayOK;

#define SetDisplayOKFlag()          (g_bDisplayOK = 1)
#define ClrDisplayOKFlag()          (g_bDisplayOK = 0)

#define Set_SecondFlag()            (SecondFlag = 1)
#define Clr_SecondFlag()            (SecondFlag = 0)

#define Set_ms10Flag()          (ms10Flag = 1)
#define Clr_ms10Flag()          (ms10Flag = 0)

#define Set_ms50Flag()          (ms50Flag = 1)
#define Clr_ms50Flag()          (ms50Flag = 0)

#define Set_ms500Flag()         (ms500Flag = 1)
#define Clr_ms500Flag()         (ms500Flag = 0)
#define Set_ms1000Flag()        (ms1000Flag = 1)
#define Clr_ms1000Flag()        (ms1000Flag = 0)
#define Set_InputTimingChangeFlag()         {                               \
                                                InputTimingChangeFlag = 1;  \
                                                if(g_bDisplayOK)            \
                                                {                           \
                                                    BootTimeStamp_Clr();    \
                                                    BootTimeStamp_Set(AP_INIT_TS, 0, TRUE);\
                                                    ClrDisplayOKFlag();     \
                                                }                           \
                                            }
#define Clr_InputTimingChangeFlag()         (InputTimingChangeFlag = 0)
extern volatile WORD data ms_Counter; // for System timer
extern BYTE xdata ModeDetectCounter;
extern volatile WORD xdata u16PanelOffOnCounter;
extern volatile bit bPanelOffOnFlag;
#define SetPanelOffOnCounter(x) ((x)?(bPanelOffOnFlag=0,u16PanelOffOnCounter=x,bPanelOffOnFlag=1):(bPanelOffOnFlag=0))

extern bit bPanelOnFlag;
extern BYTE xdata BackLightActiveFlag;

extern BYTE xdata DDCWPActiveFlag;
extern bit g_bServiceEDIDUnLock;

extern BYTE xdata KeypadButton;
#if DECREASE_V_SCALING
extern WORD xdata DecVScaleValue;
#define VSTART_OFFSET  (DecVScaleValue/2)
#endif

#if Enable_Expansion
extern WORD xdata DispalyWidth ;
extern WORD xdata HStartOffset ;
#endif

extern bit g_bSlowClkDetEnabled;
extern bit g_bSlowClkDetForceDisabled;
extern BYTE g_u8SlowClkTimer;

extern BYTE xdata Second;

extern BYTE xdata InputTimingStableCounter;
extern WORD xdata SystemFlags; // for system status flags
extern WORD xdata System2Flags;
extern BYTE xdata System3Flags;  // Rex 100701
extern BYTE xdata OsdCounter;
extern BYTE xdata HotKeyCounter;
extern BYTE xdata PowerDownCounter;
//extern BYTE PatternNo;
extern DWORD xdata BlacklitTime;
extern bit SaveBlacklitTimeFlag;
extern bit gBoolVisualKey;
extern BYTE xdata gByteVisualKey;
//extern BYTE SaveIndex;
//extern BYTE xdata ChipVer;
extern BYTE xdata SaveIndex;
extern BYTE xdata PatternNo;
/*
extern BYTE  UserPrefSubRedGain;
extern BYTE  UserPrefSubGreenGain;
extern BYTE  UserPrefSubBlueGain;
 */
extern BYTE xdata BrightnessTemp;
extern BYTE xdata PrevHistoValue;

#if (ENABLE_DEBUG && ENABLE_TIME_MEASUREMENT)
extern WORD volatile idata g_u16TimeMeasurementCounter;
extern bit bTimeMeasurementFlag;
extern WORD idata g_u16Time1;
extern WORD idata g_u16Time2;

#define SetTimeMeasurementCounter(x) ((x)?(bTimeMeasurementFlag=0,g_u16TimeMeasurementCounter=x,bTimeMeasurementFlag=1):(bTimeMeasurementFlag=0))
#define INIT_TIMER()           SetTimeMeasurementCounter(0xFFFF);
#define GET_T1()                g_u16Time1=g_u16TimeMeasurementCounter
#define GET_T2()                g_u16Time2=g_u16TimeMeasurementCounter
#define PRINT_DT()              printData(" Delta T=%d",(g_u16Time1-g_u16Time2))
#define TIME_MEASURE_BEGIN()    {INIT_TIMER();GET_T1();}
#define TIME_MEASURE_END()      {GET_T2();PRINT_DT();}
#else
#define TIME_MEASURE_BEGIN()
#define TIME_MEASURE_END()
#endif

#if Enable_PanelHandler
#define PanelISRFlag        (g_s32PanelCounter >= g_s32PanelMaxCounter) // inform Power_PanelCtrlIsr execute
#define PanelHandlerFlag    (g_s32PanelCounter >= g_s32PanelMinCounter) // inform Power_PanelCtrlHandler execute
typedef enum
{
    ePANEL_STATE_0, // panel not valid / wait for power off on
    ePANEL_STATE_1, // turn on/off panel
    ePANEL_STATE_2, // turn on/off MOD / training / data ready
    ePANEL_STATE_3, // turn on/off backlight
    ePANEL_STATE_4, // panel turn on done
    ePANEL_STATE_MAX,
} ePANEL_STATE;
typedef struct {
    MS_S32 state1_2_Min;   // state 1 -> state 2 minimum limitation
    MS_S32 state1_2_Max;   // state 1 -> state 2 maximum limitation
    MS_S32 state2_3_Min;   // state 2 -> state 3 minimum limitation
    MS_S32 state2_3_Max;   // state 2 -> state 3 maximum limitation (recommended set as -1)
    MS_S32 state3_2_Min;   // state 3 -> state 2 minimum limitation
    MS_S32 state3_2_Max;   // state 3 -> state 2 maximum limitation (recommended set as -1)
    MS_S32 state2_1_Min;   // state 2 -> state 1 minimum limitation
    MS_S32 state2_1_Max;   // state 2 -> state 1 maximum limitation
    MS_S32 paneloffon_Min; // power off -> on minimum limitation
} PANEL_POWER_TIMING_INFO;
extern BOOL g_bPanelISREn;
extern BOOL g_bPanelHandlerEn;
extern BOOL g_bPanelStateStopFlag[ePANEL_STATE_MAX];
extern ePANEL_STATE g_ePanelState;
extern BOOL g_bCurrentPanelCtrlOn;
extern BOOL g_bPreviousPanelCtrlOn;
extern PANEL_POWER_TIMING_INFO g_sPanelPowerTimingInfo;
extern volatile MS_S32 g_s32PanelCounter;
extern volatile MS_S32 g_s32PanelMinCounter;
extern volatile MS_S32 g_s32PanelMaxCounter;
extern volatile MS_S32 g_s32PanelCustomCounter;
#else
#if Enable_ReducePanelPowerOnTime
extern volatile WORD g_u16PanelPowerCounter;
typedef enum
{
    ePANEL_STATUS_NONE,        // Not valid
    ePANEL_STATUS_TurnOnData,  // Turn on data
    ePANEL_STATUS_DataRdy,     // Data ready
    ePANEL_STATUS_TurnOnBL,    // Turn on backlight
}ePANEL_STATUS_TYPE;
extern ePANEL_STATUS_TYPE g_ePanelStatus;
#endif
#endif

enum
{
  eFIRST_POWER_ON_MENU_CABLE_DETECT,
  eFIRST_POWER_ON_INDEX_MAX
};
extern BOOL g_bFirstPowerOn[eFIRST_POWER_ON_INDEX_MAX];

#if (HDMI_HPD_AC_ON_EARLY_PULL_LOW == 1)
extern volatile MS_U16 g_u16HDMIHPDAcOnEarlyPullLowCnt;
#endif
#if ENABLE_TOUCH_KEY
extern BYTE xdata TouchKeyRestortCounter;

    #if ENABLE_LOW_CONTACT
    extern BYTE xdata TouchKeyLowContactDelay; // unit: 50mS
    extern BYTE xdata TouchKeyLowContactDetectCntr; // unit: 1S, use to detect low contact peroid
    #define TOUCH_KEY_LOW_CONTACT_DETECT_PEROID     20 // unit: 1S
    #endif


    #if TOUCH_KEY_CTRL_LED
    //extern BYTE xdata TouchKeySetLEDStatus;
    typedef enum
    {
        TouchKeyLEDStatus_OFF,
        TouchKeyLEDStatus_AMBER_ON,
        TouchKeyLEDStatus_GREEN_ON,
    }TouchKeyLEDStatusType;
    #endif
#endif

#if ENABLE_HDCP
extern xdata BYTE HDCPFlag;
#endif

typedef enum
{
    eBOOT_STATUS_ACON,   // AC on
    eBOOT_STATUS_DPMSON, // DPMS wake up
    eBOOT_STATUS_DCON,    // DC on
    eBOOT_STATUS_CHECKAGAINON// Check again wakeup
}eBOOT_STATUS_TYPE;

extern XDATA eBOOT_STATUS_TYPE g_eBootStatus;

typedef enum
{
    VGA1,// 0
    DVI1,// 1
    HDMI1,// 2
    DP1,// 3
    VGA2,// 4
    DVI2,// 5
    HDMI2,// 6
    DP2,// 7
    EEPROM = 0x10,
    HDCP1,
    HDCP2//for DP
} DeviceID;

typedef enum
{
    VGA_Port1 = BIT7,
    VGA_Port2 = BIT6,
    DVI_Port1 = BIT5,
    DVI_Port2 = BIT4,
    HDMI_Port1 = BIT3,
    HDMI_Port2 = BIT2,
    DP_Port1 = BIT1,
    DP_Port2 = BIT0,
} InputPortID;

#ifndef DDC_BUFFER_LENGTH
#define DDC_BUFFER_LENGTH   50//MaxBufLen 50 // ddc buffer length
#endif

#define _UART_BUFFER_LENGTH_    DDC_BUFFER_LENGTH

extern BYTE xdata SIORxBuffer[_UART_BUFFER_LENGTH_];
extern volatile BYTE xdata SIORxIndex;
extern volatile BYTE xdata u8ReSyncCounter;
extern bit TI0_FLAG;

extern BYTE xdata SIORxBuffer1[_UART_BUFFER_LENGTH_];
extern volatile WORD xdata SIORxIndex1;
extern volatile BYTE u8ReSyncCounter1;
extern bit TI1_FLAG;

#if 1//Enable_Expansion
extern bit bExpansionBit;
#endif

extern BYTE xdata g_u8DDCCIAControl;
extern volatile WORD data u16DelayCounter;
extern volatile bit bDelayFlag;
extern volatile U32 u16TimeOutCounter;
extern volatile bit bTimeOutCounterFlag;
#define SetTimOutConter(x) ((x)?(bTimeOutCounterFlag=0,u16TimeOutCounter=x,bTimeOutCounterFlag=1):(bTimeOutCounterFlag=0))

extern BYTE xdata TPDebunceCounter;
#define SKPollingInterval   50 // unit: ms
extern bit StartScanKeyFlag;
#define Set_StartScanKeyFlag()       (StartScanKeyFlag=1)
#define Clr_StartScanKeyFlag()       (StartScanKeyFlag=0)
extern bit bKeyReadyFlag;
#define Set_bKeyReadyFlag()         (bKeyReadyFlag=1)
#define Clr_bKeyReadyFlag()         (bKeyReadyFlag=0)
#if ENABLE_TOUCH_KEY
#if TOUCH_KEY_POWER_KEY_DEBOUNCE
extern bit bPowerKeyPressed;
extern BYTE xdata PowerKeyDebounceCntr;
#endif
#endif


//extern xdata BYTE xfr_regs[256];
//extern xdata BYTE g_FlashID;
extern BYTE xdata LanguageIndex;
extern BYTE xdata SecondTblAddr;
extern BYTE xdata HistogramNow;
extern BYTE xdata HistogramPrev;
extern BYTE xdata HistogramFlag;
extern BYTE xdata BacklightNow;
extern BYTE xdata BacklightPrev;
extern BYTE xdata LastMenuPage;


extern OverScanType  xdata OverScanSetting;

#if CompressPropFont
typedef WORD PropFontType;
#else
typedef struct
{
    BYTE SpaceWidth;
    WORD LineData[18];
} PropFontType;
#endif
#if ENABLE_HDCP&&HDCPKEY_IN_Flash
extern BYTE xdata HDCPDataBuffer[6];
extern BYTE code g_u8SecureStormAESKey[16];
#endif
#define BKSVDataAddr  0
#define HDCPKetAddr     5

#if ENABLE_USB_TYPEC
extern BYTE xdata g_u8TypeCPMDBCCnt; // PM TypeC Auto Mode Debounce Counter
#endif
#if ENABLE_SECU_R2
extern BYTE xdata g_u8SecuR2Alive; // SecuR2 status. 1:alive, 0:dead
#endif

extern BYTE xdata InputColorFormat; // indicate input color space is RGB or YUV
typedef enum
{
    INPUTCOLOR_RGB,
    INPUTCOLOR_YUV,
    INPUTCOLOR_AUTO,

    INPUTCOLOR_MAX
} InputColorFormatType;

extern BYTE xdata InputColorRange; // indicate input color space is Full or Limit
typedef enum
{
    INPUTCOLOR_FULL,
    INPUTCOLOR_LIMIT,
    INPUTCOLOR_DEFAULT
} InputColorRangeType;


extern BYTE xdata SwitchPortCntr;

//extern InputModeType code StandardMode[];
//extern InputResolutionType code StandardModeResolution[];

extern PanelSettingType xdata PanelSetting;
extern InputTimingType xdata InputTiming;
extern MonitorSettingType xdata MonitorSetting;
#if USEFLASH
extern MonitorSettingType2 xdata  MonitorSetting2;
#endif
extern ModeSettingType xdata ModeSetting;
#if 1//USEFLASH
extern FactorySettingType xdata FactorySetting;
#endif
#ifdef UseInternalDDCRam
extern BYTE xdata DDC1[128];
extern bit EnCheckVccFlag;
extern BYTE SystemVccDropDownCnt;
#endif
//extern bit PowerGoodExistFlag;



#if Enable_Expansion
#define DefExpansion    Expansion_Full
#endif


#define DEF_ADC_PGA_GAIN    0x0C

#define DEF_ADC_RED_GAIN    0x1000
#define DEF_ADC_GREEN_GAIN  0x1000
#define DEF_ADC_BLUE_GAIN   0x1000
#define DEF_ADC_OFFSET      0x00
#if (LiteMAX_Baron_UI == 1)
#ifndef DEF_FAC_BRIGHTNESS_0
#define DEF_FAC_BRIGHTNESS_0	20
#endif
#ifndef DEF_FAC_BRIGHTNESS_25
#define	DEF_FAC_BRIGHTNESS_25	40
#endif
#ifndef DEF_FAC_BRIGHTNESS_50
#define	DEF_FAC_BRIGHTNESS_50	60
#endif
#ifndef DEF_FAC_BRIGHTNESS_75
#define	DEF_FAC_BRIGHTNESS_75	80
#endif
#ifndef DEF_FAC_BRIGHTNESS_100
#if (LiteMAX_Baron_UI == 1)
#define	DEF_FAC_BRIGHTNESS_100	99
#else
#define	DEF_FAC_BRIGHTNESS_100	100
#endif
#endif
#endif

//#define ADC_GAIN_MAX    0x400*

// for srgb
//=======================================================================================//
//#define DefsRGB_RedColor        DefWarm_RedColor
//#define DefsRGB_GreenColor  DefWarm_GreenColor
//#define DefsRGB_BlueColor       DefWarm_BlueColor

#if PanelRSDS
#define DefVcomValue       0x80
#endif

extern volatile WORD xdata TimeOutCounter;

#if (LiteMAX_Baron_UI == 1)
#define MaxBrightnessValue	99
#else
#define MaxBrightnessValue 100 // 160//0x80 wmz 20051017
#endif

#define MinBrightnessValue  0 //0x80    wmz 20051017

#if (LiteMAX_Baron_UI == 1)
#define DefBrightness       75
#else
#define DefBrightness       100
#endif

#define DefRedColor     0x80//0xB2//0xAB // 96(0x60)
#define DefGreenColor   0x80//0xB2//0xAB //
#define DefBlueColor    0x80//0xB2//0xAB

#define DefColorUser            0x80//136//148//155 // 155x106/128 = 128

#define DefHue                  50
#define DefSaturation           0x80

#define DefIndependentH                  50
#define DefIndependentS                  0x80
#define DefIndependentY                  50

#define MinSubContrast       55
#define MaxSubContrast       255

#define MinSubBrightness     55
#define MaxSubBrightness     255

#define DefSubContrast          ((MaxSubContrast-MinSubContrast)/2+MinSubContrast)
#define DefSubBrightness        ((MaxSubBrightness-MinSubBrightness)/2+MinSubBrightness)


#define MaxContrastValue        (0x80+50) // 156 // 160//0x80 wmz 20051017
#define MinContrastValue        (0x80-50)//56//36 // 32 //0x80    wmz 20051017

#define DefContrast         ((MaxContrastValue-MinContrastValue)/2+MinContrastValue)//106 //96 // 92 // 0x5c // 0x60 //32~96~160 171(0xAB)

#if ENABLE_SHARPNESS
#if (LiteMAX_UI == 1)
#define MaxSharpness    0x08   //user sharpness adjust gain: 0x10~0x1F==>1.0~ 1.9
#define MinSharpness    0x00     //user sharpness adjust gain  0x00~0x0F==>0.0~ 0.9
#define DefSharpness         ((MaxSharpness-MinSharpness)/2+MinSharpness)
#else
#define MaxSharpness    0x1F   //user sharpness adjust gain: 0x10~0x1F==>1.0~ 1.9
#define MinSharpness    0x00     //user sharpness adjust gain  0x00~0x0F==>0.0~ 0.9
#define DefSharpness         ((MaxSharpness-MinSharpness)/2+MinSharpness)
#endif
#endif

#if (NEW_MTK_UI == 1)
#define MaxColorValue           (0xff-28)//((UserPrefColorTemp == CTEMP_USER)?(0xFF):(DefColorUser+100))
#define MinColorValue           (0+28)//((UserPrefColorTemp == CTEMP_USER)?(0x10):(DefColorUser-100))
#else
#define MaxColorValue           (0xff)
#define MinColorValue           (0)
#endif

#define ClockAdjRange   100
#define DefRGBgainBase  20
#define DefContrastBase   0//48
#define MAX_VIDEO_SATURATION    0xFF
#define MIN_VIDEO_SATURATION    0
#define MAX_VIDEO_HUE           100
#define MIN_VIDEO_HUE           0
#define MaxVolume         100//74 // 79
#define MinVolume          00//24 // 26 // 27
#define MaxHStart   UserPrefAutoHStart+50
#define MinHStart   UserPrefAutoHStart-50
#define MaxVStart 2*UserPrefAutoVStart-1
#define MinVStart 1  // 2006/10/26 4:18PM by Emily  0
#define MaxClock    (StandardModeHTotal+100)
#define MinClock    (StandardModeHTotal-100)

//============================================================================================
// current input timing information
#define SrcHPeriod      InputTiming.HPeiod
#define SrcHTotalMSA    InputTiming.HTotalMSA
#define SrcVTotal       InputTiming.VTotal
#define SrcModeIndex    InputTiming.ModeIndex
#define SrcFlags        InputTiming.fFlags
#define SrcInputType    InputTiming.InputType
#define SrcVFreq        InputTiming.VFreq
#if ENABLE_CHECK_TMDS_DE
#define SrcTMDSWidth    InputTiming.TMDSWidth
#define SrcTMDSHeight   InputTiming.TMDSHeight
#define SrcTMDSHStart   InputTiming.TMDSHStart
#define SrcTMDSVStart   InputTiming.TMDSVStart
#endif
#define Input420Flag    InputTiming.YUV420Flag
#define InputALLMFlag   InputTiming.ALLMFlag
#define SrcIVSPeriod    InputTiming.IVSPeriod
//============================================================================================
// VESA Standard mode default settings
#if   0
#define StandardModeWidth          GetStandardModeWidth(StandardModeGroup)//StandardModeResolution[StandardModeGroup].DispWidth
#define StandardModeHeight  GetStandardModeHeight(StandardModeGroup)//StandardModeResolution[StandardModeGroup].DispHeight
#define StandardModeGroup   GetStandardModeGroup(SrcModeIndex)//StandardMode[SrcModeIndex].ResIndex
#define StandardModeHStart  GetStandardModeHStart(SrcModeIndex)//StandardMode[SrcModeIndex].HStart
#define StandardModeVStart  GetStandardModeVStart(SrcModeIndex)//StandardMode[SrcModeIndex].VStart
#define StandardModeHTotal  GetStandardModeHTotal(SrcModeIndex)//StandardMode[SrcModeIndex].HTotal
#define StandardModeVTotal  GetStandardModeVTotal(SrcModeIndex)//StandardMode[SrcModeIndex].VTotal
#define StandardModeHFreq   GetStandardModeHFreq(SrcModeIndex)//StandardMode[SrcModeIndex].HFreq
#define StandardModeVFreq       GetStandardModeVFreq(SrcModeIndex)//StandardMode[SrcModeIndex].VFreq
#endif
#define StandardModeWidth   GetStandardModeWidth()//StandardModeResolution[StandardModeGroup].DispWidth
#define StandardModeHeight  GetStandardModeHeight()
#define StandardModeGroup   GetStandardModeGroup()
#define StandardModeHStart  GetStandardModeHStart()
#define StandardModeVStart  GetStandardModeVStart()
#define StandardModeHTotal  GetStandardModeHTotal()
#define StandardModeVTotal  GetStandardModeVTotal()
#define StandardModeHFreq   GetStandardModeHFreq()
#define StandardModeVFreq   GetStandardModeVFreq()
//============================================================================================

#if 1
// Monitor Setting
#define MonitorFlags                MonitorSetting.MonitorFlag
#define UserPrefColorTemp           MonitorSetting.ColorTemp
#define UserPrefBrightness          MonitorSetting.Brightness
#define UserPrefContrast            MonitorSetting.Contrast
#define UserPrefBrightnessWarm1     MonitorSetting.BrightnessWarm1
#define UserPrefContrastWarm1       MonitorSetting.ContrastWarm1
#define UserPrefBrightnessNormal    MonitorSetting.BrightnessNormal
#define UserPrefContrastNormal      MonitorSetting.ContrastNormal
#define UserPrefBrightnessCool1     MonitorSetting.BrightnessCool1
#define UserPrefContrastCool1       MonitorSetting.ContrastCool1
#define UserPrefBrightnessSRGB      MonitorSetting.BrightnesssRGB
#define UserPrefContrastSRGB        MonitorSetting.ContrastsRGB
#define UserPrefRedColor            MonitorSetting.RedColor
#define UserPrefGreenColor          MonitorSetting.GreenColor
#define UserPrefBlueColor           MonitorSetting.BlueColor
#define UserPrefBrightnessUser      MonitorSetting.BrightnessUser
#define UserPrefContrastUser        MonitorSetting.ContrastUser
#define UserPrefRedColorUser        MonitorSetting.RedColorUser
#define UserPrefGreenColorUser      MonitorSetting.GreenColorUser
#define UserPrefBlueColorUser       MonitorSetting.BlueColorUser


#define UserPrefHue                 MonitorSetting.Hue
#define UserPrefSaturation          MonitorSetting.Saturation

#define UserPrefIndependentHueR     MonitorSetting.IndependentHueR
#define UserPrefIndependentHueG     MonitorSetting.IndependentHueG
#define UserPrefIndependentHueB     MonitorSetting.IndependentHueB
#define UserPrefIndependentHueC     MonitorSetting.IndependentHueC
#define UserPrefIndependentHueM     MonitorSetting.IndependentHueM
#define UserPrefIndependentHueY     MonitorSetting.IndependentHueY
#define UserPrefIndependentSaturationR          MonitorSetting.IndependentSaturationR
#define UserPrefIndependentSaturationG          MonitorSetting.IndependentSaturationG
#define UserPrefIndependentSaturationB          MonitorSetting.IndependentSaturationB
#define UserPrefIndependentSaturationC          MonitorSetting.IndependentSaturationC
#define UserPrefIndependentSaturationM          MonitorSetting.IndependentSaturationM
#define UserPrefIndependentSaturationY          MonitorSetting.IndependentSaturationY
#define UserPrefIndependentBrightnessR          MonitorSetting.IndependentBrightnessR
#define UserPrefIndependentBrightnessG          MonitorSetting.IndependentBrightnessG
#define UserPrefIndependentBrightnessB          MonitorSetting.IndependentBrightnessB
#define UserPrefIndependentBrightnessC          MonitorSetting.IndependentBrightnessC
#define UserPrefIndependentBrightnessM          MonitorSetting.IndependentBrightnessM
#define UserPrefIndependentBrightnessY          MonitorSetting.IndependentBrightnessY

#if ENABLE_DP_INPUT
#define UserPrefMSTOnOff             MonitorSetting.MSTOnOff
#define UserPrefDPVersion            MonitorSetting.DPVersion
#endif

#define UserPrefDCOffDischarge      MonitorSetting.DCOffDischarge
#define UserPrefPowerSavingEn       MonitorSetting.PowerSaving
#if MWEFunction
#define UserPrefColorFlags          MonitorSetting.ColorFlag
#define UserPrefSubContrast         MonitorSetting.SubContrast
#define UserPrefSubBrightness       MonitorSetting.SubBrightness
#define UserPrefBFSize              MonitorSetting.BF_Size
#define UserPrefBFHSize             MonitorSetting.BF_HSize
#define UserPrefBFVSize             MonitorSetting.BF_VSize
#define UserPrefBFHStart            MonitorSetting.BF_Hstart
#define UserPrefBFVStart            MonitorSetting.BF_Vstart
#endif

#if Enable_Gamma
#define UserPrefGamaMode            MonitorSetting.GamaMode
#define UserPrefGamaOnOff           MonitorSetting.GamaOnOff
#endif

#if (AudioFunc )
#define UserPrefAudioSource         MonitorSetting.AudioSource
#if (LiteMAX_UI == 1)
#define UserPrefOSDSoundMute        MonitorSetting.OSDSoundMute
#endif
#endif

#define UserPrefVolume              MonitorSetting.Volume
#define UserPrefECOMode             MonitorSetting.ECOMode
#define UserPrefLanguage            MonitorSetting.Language
#define UserPrefOsdHStart           MonitorSetting.OsdHStart
#define UserPrefOsdVStart           MonitorSetting.OsdVStart
#define UserPrefOsdTime             MonitorSetting.OsdTime
#define UserPrefOsdTransparency     MonitorSetting.OsdTransparency
#if ENABLE_OSD_ROTATION
#define UserPrefOsdRotateMode       MonitorSetting.OSDRotateMode
#endif
#if ENABLE_FREESYNC
#define UserprefFreeSyncMode        MonitorSetting.FreeSyncMode
#endif
#define UserPrefDisplayLogoEn         MonitorSetting.DisplayLogoEn
#define UserPrefInputType           MonitorSetting.InputType
#define UserPrefInputPriorityType   MonitorSetting.InputSelectType
//#define UserPrefLastMenuIndex        MonitorSetting.LastMenuPage
#define UserPrefSavedModeIndex      MonitorSetting.SavedModeIndex // current mode index can be used
#define UserprefRedBlackLevel       MonitorSetting.RedBlackLevel
#define UserprefGreenBlackLevel     MonitorSetting.GreenBlackLevel
#define UserprefBlueBlackLevel      MonitorSetting.BlueBlackLevel


#define UserPrefDcrMode             MonitorSetting.DcrMode

#if Enable_Expansion
#define UserprefExpansionMode       MonitorSetting.ExpansionMode
#endif

#if ENABLE_DLC
#define UserprefDLCMode             MonitorSetting.DLCMode
#endif

#if ENABLE_DPS
#define UserprefDPSMode             MonitorSetting.DPSMode
#endif
#if ENABLE_RTE
#define UserprefOverDriveSwitch     MonitorSetting.OverDriveMode
#define UserprefOverDriveWeighting  MonitorSetting.OverDriveWeighting
#endif
#if ENABLE_SUPER_RESOLUTION
#define UserPrefSuperResolutionMode MonitorSetting.SuperResolutionMode
#endif
#if ENABLE_SHARPNESS
#define UserPrefSharpness           MonitorSetting.Sharpness
#endif
#if ENABLE_DeBlocking
#define UserPrefDeBlocking          MonitorSetting.DeBlocking
#endif
#if ENABLE_3DLUT
#define UserPref3DLUTmode           MonitorSetting.RGB3DLUTMode
#endif

#define UserPrefColorMode           MonitorSetting.ColorMode
#define UserPrefInputColorFormat    MonitorSetting.InputColorFormat
#define UserPrefInputColorRange     MonitorSetting.InputColorRange

#define UserprefHistogram1          MonitorSetting.Histogram1
#define UserprefHistogram2          MonitorSetting.Histogram2
#define UserprefALha                MonitorSetting.ALhaA
#define UserprefBata                MonitorSetting.BataA
#define MSBankNo                    MonitorSetting.BankNo
#define MSAddr                      MonitorSetting.Addr
#define MSValue                     MonitorSetting.Value
#if USEFLASH
#define UserprefBacklighttime       MonitorSetting2.Backlighttime
#else
#define UserprefBacklighttime       MonitorSetting.Backlighttime
#endif
#endif

#define UserPref2Dto3D              MonitorSetting.T3DMode
#define UserPref2Dto3DGameMode      MonitorSetting.T3DGameMode
#define UserPref2Dto3DGain          MonitorSetting.T3DGain
#define UserPref2Dto3DOffset        MonitorSetting.T3DOffset

#if (LiteMAX_Baron_UI == 1)
#define UserprefPowerKeyEnable      MonitorSetting.PowerKeyEnable

//======== FACTORY MENU FUNCTION ===============//
#define FUserPrefBrightness_0       FactorySetting.FBrightness_0
#define FUserPrefBrightness_25      FactorySetting.FBrightness_25
#define FUserPrefBrightness_50      FactorySetting.FBrightness_50
#define FUserPrefBrightness_75      FactorySetting.FBrightness_75
#define FUserPrefBrightness_100     FactorySetting.FBrightness_100
#endif

#define UserPrefAdcPgaGain          FactorySetting.AdcPgaGain
#define UserPrefAdcRedGain          FactorySetting.AdcRedGain
#define UserPrefAdcGreenGain        FactorySetting.AdcGreenGain
#define UserPrefAdcBlueGain         FactorySetting.AdcBlueGain
#define UserPrefAdcRedOffset        FactorySetting.AdcRedOffset
#define UserPrefAdcGreenOffset      FactorySetting.AdcGreenOffset
#define UserPrefAdcBlueOffset       FactorySetting.AdcBlueOffset

#define UserPrefRedColorWarm1       FactorySetting.RedColorWarm1
#define UserPrefGreenColorWarm1     FactorySetting.GreenColorWarm1
#define UserPrefBlueColorWarm1      FactorySetting.BlueColorWarm1
#define UserPrefRedColorNormal      FactorySetting.RedColorNormal
#define UserPrefGreenColorNormal    FactorySetting.GreenColorNormal
#define UserPrefBlueColorNormal     FactorySetting.BlueColorNormal
#define UserPrefRedColorCool1       FactorySetting.RedColorCool1
#define UserPrefGreenColorCool1     FactorySetting.GreenColorCool1
#define UserPrefBlueColorCool1      FactorySetting.BlueColorCool1
#define UserPrefRedColorSRGB        FactorySetting.RedColorsRGB
#define UserPrefGreenColorSRGB      FactorySetting.GreenColorsRGB
#define UserPrefBlueColorSRGB       FactorySetting.BlueColorsRGB
#define FUserPrefBrightnessWarm1    FactorySetting.FBrightnessWarm1
#define FUserPrefContrastWarm1      FactorySetting.FContrastWarm1
#define FUserPrefBrightnessNormal   FactorySetting.FBrightnessNormal
#define FUserPrefContrastNormal     FactorySetting.FContrastNormal
#define FUserPrefBrightnessCool1    FactorySetting.FBrightnessCool1
#define FUserPrefContrastCool1      FactorySetting.FContrastCool1
#define FUserPrefBrightnessSRGB     FactorySetting.FBrightnesssRGB
#define FUserPrefContrastSRGB       FactorySetting.FContrastsRGB

#if PanelRSDS
#define UserPrefVcomValue           FactorySetting.VcomValue
#endif

#if USEFLASH
#define FactoryProductModeValue     FactorySetting.ProductModeValue
#else
#define FactoryCheckSum             FactorySetting.CheckSum
#endif

#define UserPrefLastMenuIndex       LastMenuPage


#if MWEFunction
#define bColorAutoDetectBit           BIT0
#define bSkinBit                      BIT1
#define bBlueBit                      BIT2
#define bGreenBit                     BIT3
#define bMWEModeBit                   BIT4
#define bDemoBit                      BIT5
#define bFullEnhanceBit               BIT6
#define bPictureBoostBit              BIT7
#define MWEModeFlag                 (UserPrefColorFlags&bMWEModeBit)
#define Set_MWEModeFlag()           (UserPrefColorFlags|=bMWEModeBit)
#define Clr_MWEModeFlag()           (UserPrefColorFlags&=~bMWEModeBit)
#define ColorAutoDetectFlag         (UserPrefColorFlags&bColorAutoDetectBit)
#define Set_ColorAutoDetectFlag()   (UserPrefColorFlags|=bColorAutoDetectBit )
#define Clr_ColorAutoDetectFlag()   (UserPrefColorFlags&=~bColorAutoDetectBit )
#define SkinFlag                    (UserPrefColorFlags&bSkinBit)
#define Set_SkinFlag()              (UserPrefColorFlags|=bSkinBit)
#define Clr_SkinFlag()              (UserPrefColorFlags&=~bSkinBit)
#define BlueFlag                    (UserPrefColorFlags&bBlueBit)
#define Set_BlueFlag()              (UserPrefColorFlags|=bBlueBit)
#define Clr_BlueFlag()              (UserPrefColorFlags&=~bBlueBit)
#define GreenFlag                   (UserPrefColorFlags&bGreenBit)
#define Set_GreenFlag()             (UserPrefColorFlags|=bGreenBit)
#define Clr_GreenFlag()             (UserPrefColorFlags&=~bGreenBit)
#define DemoFlag                    (UserPrefColorFlags&bDemoBit    )
#define Set_DemoFlag()              (UserPrefColorFlags|=bDemoBit   )
#define Clr_DemoFlag()              (UserPrefColorFlags&=~bDemoBit  )
#define FullEnhanceFlag             (UserPrefColorFlags&bFullEnhanceBit )
#define Set_FullEnhanceFlag()       (UserPrefColorFlags|=bFullEnhanceBit    )
#define Clr_FullEnhanceFlag()       (UserPrefColorFlags&=~bFullEnhanceBit )
#define PictureBoostFlag            (UserPrefColorFlags&bPictureBoostBit    )
#define Set_PictureBoostFlag()      (UserPrefColorFlags|=bPictureBoostBit )
#define Clr_PictureBoostFlag()      (UserPrefColorFlags&=~bPictureBoostBit  )
#endif
//============================================================================================
// current input timing setting
#define UserPrefHTotal          ModeSetting.HTotal
#define UserPrefHStart          ModeSetting.HStart
#define UserPrefVStart          ModeSetting.VStart
#define UserPrefPhase           ModeSetting.Phase
#define UserPrefAutoHStart      ModeSetting.AutoHStart
#define UserPrefAutoVStart      ModeSetting.AutoVStart  //2004/01/15 num11
#define UserPrefAutoTimes       ModeSetting.AutoTimes
//======================================================================================
#define SyncLoss                (bHSyncLoss|bVSyncLoss)
#define SyncPolarity(status)    (status&0x03)
#define GetSyncPolarity(status) (1<<(status&0x03))
//=================================================================================
// Input Timing information flags
#define SyncLossState()         (SrcFlags&SyncLoss)
#define UserModeFlag            (SrcFlags&bUserMode)
#define UnsupportedModeFlag     (SrcFlags&bUnsupportMode)
#define NativeModeFlag          (SrcFlags&bNativeMode)
#define ShrinkModeFlag          (SrcFlags&bShrinkMode)
//==================================================================================
// Monitor status flags for storing into NVRAM
#define bPowerOnBit             BIT0    // system power on/off status
#define bFactoryModeBit         BIT1    // factory mode
#define bBurninModeBit          BIT2    // burnin mode
#define bDoBurninModeBit        BIT3
#define bOsdLockModeBit         BIT4
#define bShowOsdLockBit         BIT5
//    #define bMuteOnBit                BIT6
//#define bOverDriveOnBit         BIT6
#define bDCCCIONOBit                BIT7
#define bTNROnBit               BIT8
#if ENABLE_OSD_ROTATION
#define bMenuRotationBit           BIT9
#endif
//========= flags status
#define PowerOnFlag             (MonitorFlags&bPowerOnBit)
#define FactoryModeFlag         (MonitorFlags&bFactoryModeBit)
#define BurninModeFlag          (MonitorFlags&bBurninModeBit)
#define DoBurninModeFlag        (MonitorFlags&bDoBurninModeBit)
#define OsdLockModeFlag         (MonitorFlags&bOsdLockModeBit)
#define ShowOsdLockFlag         (MonitorFlags&bShowOsdLockBit)
//#define MuteOnFlag                    (MonitorFlags&bMuteOnBit)
//#define OverDriveOnFlag             (MonitorFlags&bOverDriveOnBit)
#define DDCciFlag               (MonitorFlags&bDCCCIONOBit)
#define TNROnFlag               (MonitorFlags&bTNROnBit)

#if ENABLE_OSD_ROTATION
#define MENU_ROTATION_FLAG      (MonitorFlags&bMenuRotationBit)
#endif
//--------- flags setting
#define Set_PowerOnFlag()       (MonitorFlags|=bPowerOnBit)
#define Set_FactoryModeFlag()   (MonitorFlags|=bFactoryModeBit)
#define Set_BurninModeFlag()    (MonitorFlags|=bBurninModeBit)
#define Set_DoBurninModeFlag()  (MonitorFlags|=bDoBurninModeBit)
#define Set_OsdLockModeFlag()   (MonitorFlags|=bOsdLockModeBit)
#define Set_ShowOsdLockFlag()   (MonitorFlags|=bShowOsdLockBit)
//#define Set_MuteOnFlag()          (MonitorFlags|=bMuteOnBit)
//#define Set_OverDriveOnFlag()   (MonitorFlags|=bOverDriveOnBit)
#define Set_DDCciFlag()         (MonitorFlags|=bDCCCIONOBit)
#define Set_TNROnFlag()         (MonitorFlags|=bTNROnBit)

#define Clr_PowerOnFlag()        (MonitorFlags&=~bPowerOnBit)
#define Clr_FactoryModeFlag()    (MonitorFlags&=~bFactoryModeBit)
#define Clr_BurninModeFlag()     (MonitorFlags&=~bBurninModeBit)
#define Clr_DoBurninModeFlag()   (MonitorFlags&=~bDoBurninModeBit)
#define Clr_OsdLockModeFlag()    (MonitorFlags&=~bOsdLockModeBit)
#define Clr_ShowOsdLockFlag()    (MonitorFlags&=~bShowOsdLockBit)
//#define Clr_MuteOnFlag()          (MonitorFlags&=~bMuteOnBit)
//#define Clr_OverDriveOnFlag()   (MonitorFlags&=~bOverDriveOnBit)
#define Clr_DDCciFlag()          (MonitorFlags&=~bDCCCIONOBit)
#define Clr_TNROnFlag()          (MonitorFlags&=~bTNROnBit)
#if ENABLE_OSD_ROTATION
#define SET_MENU_ROTATION_FLAG() (MonitorFlags|=bMenuRotationBit)
#define CLR_MENU_ROTATION_FLAG() (MonitorFlags&=~bMenuRotationBit)
#endif


//==================================================================================
// System status flags
//#define bInputTimingChangeBit       BIT0
#define bPowerSavingBit             BIT1    // system power saving status
#define bForcePowerSavingBit        BIT2    // indicated system must enter power down
#define bInputTimingStableBit       BIT3    // input timing stable
#define bDoModeSettingBit           BIT4    //
#define bCableNotConnectedBit       BIT5
#define bFreeRunModeBit             BIT6
#define bOsdTimeoutBit              BIT7    // osd timeout
#define bSaveMonitorSettingBit      BIT8    // flag for storing settings into NVRam
#define bSaveModeSettingBit         BIT9
#define bChangePatternBit           BIT10   // switch input port
#define bRxBusyBit                  BIT11   // debug is busy
#define bms500Bit                   BIT12
#define bReverseXBit                BIT13
#define bReverseYBit                BIT14
#define bSARwakeupBit               BIT15
//=========System2 Flag Bit================================================================
#define bDisplayLogoBit             BIT0
#define bFlashBit                   BIT1
#define bReadyForChangeBit          BIT2
#define bLoadOSDDataBit             BIT3
//#define bLoadFontBit              BIT4
#define bPushECOHotKeyBit           BIT4
#define bAutoColorBit               BIT5
#define bDLKOverBit                 BIT6
#define bPressExitKeyBit            BIT7
#define bEnableShowAutoBit          BIT8

#define bShowInputInfoBit           BIT9
#define bJustShowValueBit           BIT10
#define bDecVScaleBit               BIT11
//#define bPressMenuKeyBit          BIT12
//#define bMuteBit                  BIT13
#define bBackToStandbyBit           BIT12
#define bBackToUnsupportBit         BIT13
#define bProductModeBit             BIT14
#define bForcePowerDownBit          BIT15
//=============================System3 Flag Bit==============================
#define bOSDTimeOutSet3DBit         BIT0  // Rex 100701
#define bFakeSleepBit               BIT1
#define bForceMCCSWakeUp            BIT2

//========== system flags status
//#define InputTimingChangeFlag             (SystemFlags&bInputTimingChangeBit)
#define PowerSavingFlag             (SystemFlags&bPowerSavingBit)
#define ForcePowerSavingFlag        (SystemFlags&bForcePowerSavingBit)
#define InputTimingStableFlag       (SystemFlags&bInputTimingStableBit)
#define DoModeSettingFlag           (SystemFlags&bDoModeSettingBit)
#define CableNotConnectedFlag       (SystemFlags&bCableNotConnectedBit)
#define FreeRunModeFlag             (SystemFlags&bFreeRunModeBit)
#define RxBusyFlag                  (SystemFlags&bRxBusyBit)
#define OsdTimeoutFlag              (SystemFlags&bOsdTimeoutBit)
#define ChangePatternFlag           (SystemFlags&bChangePatternBit)
#define SaveMonitorSettingFlag      (SystemFlags&bSaveMonitorSettingBit)
#define SaveModeSettingFlag         (SystemFlags&bSaveModeSettingBit)
//#define ms500Flag                               (SystemFlags&bms500Bit)
#define ReverseXFlag                (SystemFlags&bReverseXBit)
#define ReverseYFlag                (SystemFlags&bReverseYBit)
#define SARwakeupFlag               (SystemFlags&bSARwakeupBit)
//#define AutoColorFlag                           (SystemFlags&bAutoColorBit)
//--------- System2 Flag status------
#define DisplayLogoFlag             (System2Flags&bDisplayLogoBit)
#define FlashFlag                   (System2Flags&bFlashBit)
#define ReadyForChangeFlag          (System2Flags&bReadyForChangeBit)
#define LoadOSDDataFlag             (System2Flags&bLoadOSDDataBit)
//#define LoadFontFlag                           (System2Flags&bLoadFontBit)
#define PushECOHotKeyFlag           (System2Flags&bPushECOHotKeyBit)     //111110 Rick modified adjust ECO condition - B40139
//#define AutoColorFlag                           (System2Flags&bAutoColorBit)
//#define DisableLuminanceFlag                (System2Flags&bDisableLuminanceBit)
//#define DLKOverFlag                             (System2Flags&bDLKOverBit)
//#define SpeedupIncBrighnessFlag           (System2Flags&bSpeedupIncBrighnessBit)
#define ShowInputInfoFlag           (System2Flags&bShowInputInfoBit)
#define JustShowValueFlag           (System2Flags&bJustShowValueBit)

//    #define PanelOnFlag                          (System2Flags&bPanelOnBit)
#define BackToStandbyFlag            (System2Flags&bBackToStandbyBit)
#define BackToUnsupportFlag          (System2Flags&bBackToUnsupportBit)

//=======================================================================================//
// for hot key control
//=======================================================================================//
#define PressExitFlag               (System2Flags&bPressExitKeyBit)
#define EnableShowAutoFlag          (System2Flags&bEnableShowAutoBit)
//#define PressMenuFlag                         (System2Flags&bPressMenuKeyBit)
//#define MuteFlag                                 (System2Flags&bMuteBit)
#define ProductModeFlag             (System2Flags&bProductModeBit)
//#define ForcePowerDownFlag                 (System2Flags&bForcePowerDownBit)
//--------- system flags setting
//#define Set_InputTimingChangeFlag()     (SystemFlags|=bInputTimingChangeBit)
//#define Clr_InputTimingChangeFlag()      (SystemFlags&=~bInputTimingChangeBit)
#define Set_PowerSavingFlag()       (SystemFlags|=bPowerSavingBit)
#define Clr_PowerSavingFlag()       (SystemFlags&=~bPowerSavingBit)
#define Set_ForcePowerSavingFlag()  (SystemFlags|=bForcePowerSavingBit)
#define Clr_ForcePowerSavingFlag()  (SystemFlags&=~bForcePowerSavingBit)
#define Set_InputTimingStableFlag() (SystemFlags|=bInputTimingStableBit)
#define Clr_InputTimingStableFlag() (SystemFlags&=~bInputTimingStableBit)
#define Set_RxBusyFlag()            (SystemFlags|=bRxBusyBit)
#define Clr_RxBusyFlag()            (SystemFlags&=~bRxBusyBit)
#define Set_DoModeSettingFlag()     (SystemFlags|=bDoModeSettingBit)
#define Clr_DoModeSettingFlag()     (SystemFlags&=~bDoModeSettingBit)
#define Set_OsdTimeoutFlag()        (SystemFlags|=bOsdTimeoutBit)
#define Clr_OsdTimeoutFlag()        (SystemFlags&=~bOsdTimeoutBit)
#define Set_CableNotConnectedFlag() (SystemFlags|=bCableNotConnectedBit)
#define Clr_CableNotConnectedFlag() (SystemFlags&=~bCableNotConnectedBit)
#define Set_FreeRunModeFlag()       (SystemFlags|=bFreeRunModeBit)
#define Clr_FreeRunModeFlag()       (SystemFlags&=~bFreeRunModeBit)
#define Set_ChangePatternFlag()     (SystemFlags|=bChangePatternBit)
#define Clr_ChangePatternFlag()     (SystemFlags&=~bChangePatternBit)
#define Set_SaveMonitorSettingFlag()    (SystemFlags|=bSaveMonitorSettingBit)
#define Clr_SaveMonitorSettingFlag()    (SystemFlags&=~bSaveMonitorSettingBit)
#define Set_SaveModeSettingFlag()   (SystemFlags|=bSaveModeSettingBit)
#define Clr_SaveModeSettingFlag()   (SystemFlags&=~bSaveModeSettingBit)
//#define Set_ms500Flag()             (SystemFlags|=bms500Bit)
//#define Clr_ms500Flag()             (SystemFlags&=~bms500Bit)
#define Set_ReverseXFlag()          (SystemFlags|=bReverseXBit)
#define Clr_ReverseXFlag()          (SystemFlags&=~bReverseXBit)
#define Set_ReverseYFlag()          (SystemFlags|=bReverseYBit)
#define Clr_ReverseYFlag()          (SystemFlags&=~bReverseYBit)
#define Set_SARwakeupFlag()         (SystemFlags|=bSARwakeupBit)
#define Clr_SARwakeupFlag()         (SystemFlags&=~bSARwakeupBit)

//#define Set_ISPFlag()                 (SystemFlags|=bISPBit)
//#define Clr_ISPFlag()                 (SystemFlags&=~bISPBit)
//=============================================================
//Set AOC LOGO STATUS
#define Set_DisplayLogoFlag()          (System2Flags|=bDisplayLogoBit)
#define Clr_DisplayLogoFlag()          (System2Flags&=~bDisplayLogoBit)
#define Set_FlashFlag()                (System2Flags|=bFlashBit)
#define Clr_FlashFlag()                (System2Flags&=~bFlashBit)
#define Set_ReadyForChangeFlag()       (System2Flags|=bReadyForChangeBit)
#define Clr_ReadyForChangeFlag()       (System2Flags&=~bReadyForChangeBit)
#define Set_LoadOSDDataFlag()          (System2Flags|=bLoadOSDDataBit)
#define Clr_LoadOSDDataFlag()          (System2Flags&=~bLoadOSDDataBit)
#define Set_PushECOHotKeyFlag()        (System2Flags|=bPushECOHotKeyBit)
#define Clr_PushECOHotKeyFlag()        (System2Flags&=~bPushECOHotKeyBit)
//#define Set_AutoColorFlag()                       (System2Flags|=bAutoColorBit)
//#define Clr_AutoColorFlag()                        (System2Flags&=~bAutoColorBit)
//#define Set_DisableLuminanceFlag()            (System2Flags|=bDisableLuminanceBit)
//#define Clr_DisableLuminanceFlag()             (System2Flags&=~bDisableLuminanceBit)
//#define Set_DLKOverFlag()                         (System2Flags|=bDLKOverBit)
//#define Clr_DLKOverFlag()                          (System2Flags&=~bDLKOverBit)
//#define Set_SpeedupIncBrighnessFlag()        (System2Flags|=bSpeedupIncBrighnessBit)
//#define Clr_SpeedupIncBrighnessFlag()         (System2Flags&=~bSpeedupIncBrighnessBit)
#define Set_ShowInputInfoFlag()        (System2Flags|=bShowInputInfoBit)
#define Clr_ShowInputInfoFlag()        (System2Flags&=~bShowInputInfoBit)
#define Set_JustShowValueFlag()        (System2Flags|=bJustShowValueBit)
#define Clr_JustShowValueFlag()        (System2Flags&=~bJustShowValueBit)

//#define Set_PanelOnFlag()                          (System2Flags|=bPanelOnBit)
//#define Clr_PanelOnFlag()                           (System2Flags&=~bPanelOnBit)
#define Set_PressExitFlag()             (System2Flags|=bPressExitKeyBit)
#define Clr_PressExitFlag()             (System2Flags&=~bPressExitKeyBit)
#define Set_EnableShowAutoFlag()        (System2Flags|=bEnableShowAutoBit)
#define Clr_EnableShowAutoFlag()        (System2Flags&=~bEnableShowAutoBit)
//#define Set_PressMenuFlag()                       (System2Flags|=bPressMenuKeyBit)
//#define Clr_PressMenuFlag()                        (System2Flags&=~bPressMenuKeyBit)
//#define Set_MuteFlag()                               (System2Flags|=bMuteBit)
//#define Clr_MuteFlag()                                (System2Flags&=~bMuteBit)
#define Set_ProductModeFlag()           (System2Flags|=bProductModeBit)
#define Clr_ProductModeFlag()           (System2Flags&=~bProductModeBit)
//#define Set_ForcePowerDownFlag()               (System2Flags|=bForcePowerDownBit)
//#define Clr_ForcePowerDownFlag()                (System2Flags&=~bForcePowerDownBit)

#define Set_BackToStandbyFlag()         (System2Flags|=bBackToStandbyBit)
#define Clr_BackToStandbyFlag()         (System2Flags&=~bBackToStandbyBit)

#define Set_BackToUnsupportFlag()       (System2Flags|=bBackToUnsupportBit)
#define Clr_BackToUnsupportFlag()       (System2Flags&=~bBackToUnsupportBit)

//#define Set_PowerGoodExistFlag()               (PowerGoodExistFlag=1)
//#define Clr_PowerGoodExistFlag()                (PowerGoodExistFlag=0)
//====================System3flags===================================
#define OSDTimeOutSet3D                 (System3Flags&bOSDTimeOutSet3DBit)  // Rex 100701
#define Set_OSDTimeOutSet3DFlag()       (System3Flags|=bOSDTimeOutSet3DBit)
#define Clr_OSDTimeOutSet3DFlag()       (System3Flags&=~bOSDTimeOutSet3DBit)

#define FakeSleepFlag                   (System3Flags&bFakeSleepBit)
#define Set_FakeSleepFlag()             (System3Flags|=bFakeSleepBit)
#define Clr_FakeSleepFlag()             (System3Flags&=~bFakeSleepBit)

#define ForceMCCSWakeUpFlag                   (System3Flags&bForceMCCSWakeUp)
#define Set_ForceMCCSWakeUpFlag()             (System3Flags|=bForceMCCSWakeUp, printf("[Flag] : %s() : %d : Set_ForceMCCSWakeUpFlag()\n", __FUNCTION__, __LINE__))
#define Clr_ForceMCCSWakeUpFlag()             (System3Flags&=~bForceMCCSWakeUp, printf("[Flag] : %s() : %d : Clr_ForceMCCSWakeUpFlag()\n", __FUNCTION__, __LINE__))

extern BYTE xdata FakeSleepCounter;
//===============================================================

#if 1 // ENABLE_HDCP
//=======================================================================================//
// for
//=======================================================================================//
#define bHdcpNVRamBit               BIT1
#define bWriteHDCPcodeBit           BIT2
#define bLoadHDCPKeyBit             BIT3
#define LoadHDCPKeyFlag             (HDCPFlag&bLoadHDCPKeyBit)
#define Set_LoadHDCPKeyFlag()       (HDCPFlag|=bLoadHDCPKeyBit)
#define Clr_LoadHDCPKeyFlag()       (HDCPFlag&=~bLoadHDCPKeyBit)
#define HdcpNVRamFlag               (HDCPFlag&bHdcpNVRamBit)
#define Set_HdcpNVRamFlag()         (HDCPFlag|=bHdcpNVRamBit)
#define Clr_HdcpNVRamFlag()         (HDCPFlag&=~bHdcpNVRamBit)
#define WriteHDCPcodeFlag           (HDCPFlag&bWriteHDCPcodeBit)
#define Set_WriteHDCPcodeFlag()     (HDCPFlag|=bWriteHDCPcodeBit)
#define Clr_WriteHDCPcodeFlag()     (HDCPFlag&=~bWriteHDCPcodeBit)
#endif
//=================================================================


extern bit DebugOnlyFlag;
#define Set_DebugOnlyFlag()          (DebugOnlyFlag=1)
#define Clr_DebugOnlyFlag()          (DebugOnlyFlag=0)
extern bit SaveFactorySettingFlag;
#define Set_SaveFactorySettingFlag() (SaveFactorySettingFlag=1)
#define Clr_SaveFactorySettingFlag() (SaveFactorySettingFlag=0)

extern BYTE GetPow2(BYTE expre);
extern BYTE GetLog(DWORD value);
extern BYTE TransNumToBit(BYTE value);

extern Bool IsOverScan(void);

#if EnableSerialPortTXInterrupt
#define _UART_TX_BUFFER_LENGTH_     480
extern WORD idata SIOTxBufferIndex;
extern WORD idata SIOTxSendIndex;
extern bit idata UART_SEND_DONE;
extern bit idata UartPolling;
extern BYTE xdata SIOTxBuffer[_UART_TX_BUFFER_LENGTH_];
#endif

typedef enum
{
    Items_Off,
    Items_On,
}Items_t;

typedef enum
{
    MainMenuItems_BriteCont,
    MainMenuItems_ColorSettings,
    MainMenuItems_ExtColorSettings,
    MainMenuItems_InputSource,
    MainMenuItems_DisplaySettings,
    MainMenuItems_OtherSettings,
    MainMenuItems_PowerManager,
    MainMenuItems_Information,
    MainMenuItems_Exit,

    MainMenuItems_Max,
}MainMenuItems_t;

typedef enum
{
    BriteContMenuItems_Brightness,
    BriteContMenuItems_Contrast,
#if ENABLE_DLC
    BriteContMenuItems_DLC,
#endif
#if ENABLE_DPS
    BriteContMenuItems_DPS,
#endif
#if ENABLE_DCR
    BriteContMenuItems_DCR,
#endif
#if (ENABLE_VGA_INPUT)
    BriteContMenuItems_AutoColor,
#endif
    BriteContMenuItems_Return,

    BriteContMenuItems_Max,
}BriteContMenuItems_t;

typedef enum
{
    ColorSettingsMenuItems_ColorTemp,
    ColorSettingsMenuItems_Red,
    ColorSettingsMenuItems_Green,
    ColorSettingsMenuItems_Blue,
    ColorSettingsMenuItems_Hue,
    ColorSettingsMenuItems_Saturation,
#if ENABLE_SUPER_RESOLUTION
    ColorSettingsMenuItems_SuperResolution,
#endif
#if ENABLE_COLORMODE_DEMO
    ColorSettingsMenuItems_ColorMode,
#endif
    ColorSettingsMenuItems_ColorFormat,
    ColorSettingsMenuItems_Return,

    ColorSettingsMenuItems_Max,
}ColorSettingsMenuItems_t;

typedef enum
{
    ExtColorSettingsMenuItems_ColorRange,
    ExtColorSettingsMenuItems_IndependentHue,
    ExtColorSettingsMenuItems_IndependentSaturation,
    ExtColorSettingsMenuItems_IndependentBrightness,
    ExtColorSettingsMenuItems_Return,

    ExtColorSettingsMenuItems_Max,
}ExtColorSettingsMenuItems_t;

typedef enum
{
    IndependentHueMenuItems_R,
    IndependentHueMenuItems_G,
    IndependentHueMenuItems_B,
    IndependentHueMenuItems_C,
    IndependentHueMenuItems_M,
    IndependentHueMenuItems_Y,
    IndependentHueMenuItems_Return,

    IndependentHueMenuItems_Max,
}IndependentHueMenuItems_t;

typedef enum
{
    IndependentSaturationMenuItems_R,
    IndependentSaturationMenuItems_G,
    IndependentSaturationMenuItems_B,
    IndependentSaturationMenuItems_C,
    IndependentSaturationMenuItems_M,
    IndependentSaturationMenuItems_Y,
    IndependentSaturationMenuItems_Return,

    IndependentSaturationMenuItems_Max,
}IndependentSaturationMenuItems_t;

typedef enum
{
    IndependentBrightnessMenuItems_R,
    IndependentBrightnessMenuItems_G,
    IndependentBrightnessMenuItems_B,
    IndependentBrightnessMenuItems_C,
    IndependentBrightnessMenuItems_M,
    IndependentBrightnessMenuItems_Y,
    IndependentBrightnessMenuItems_Return,

    IndependentBrightnessMenuItems_Max,
}IndependentBrightnessMenuItems_t;

typedef enum
{
    ColorModeMenuItems_Off,
    ColorModeMenuItems_Enhance,
    ColorModeMenuItems_Demo,

    ColorModeMenuItems_Max,
}ColorModeMenuItems_t;

typedef enum
{
    SuperResolutionMenuItems_Off,
    SuperResolutionMenuItems_Weak,
    SuperResolutionMenuItems_Middle,
    SuperResolutionMenuItems_Strong,

    SuperResolutionMenuItems_Max,
}SuperResolutionMenuItems_t;

typedef enum
{
    InputSourceMenuItems_SourceSelect,
#if ENABLE_DP_INPUT
    InputSourceMenuItems_DPVersion,
#endif
    InputSourceMenuItems_Return,
    
    InputSourceMenuItems_Max,
}InputSourceMenuItems_t;

typedef enum
{
    SourceSelectMenuItems_Auto,
#if (ENABLE_VGA_INPUT)
    SourceSelectMenuItems_VGA,
#endif
    SourceSelectMenuItems_Digital0,
    SourceSelectMenuItems_Digital1,
    SourceSelectMenuItems_Digital2,
    SourceSelectMenuItems_Digital3,

    SourceSelectMenuItems_Max,
}SourceSelectMenuItems_t;

#if ENABLE_DP_INPUT
typedef enum
{
    DPVersionMenuItems_DP1_1,
    DPVersionMenuItems_DP1_2,
    DPVersionMenuItems_DP1_4,

    DPVersionMenuItems_Max,
}DPVersionMenuItems_t;
#endif

typedef enum
{
    _3DMenuItems_3DFormat,
    _3DMenuItems_2Dto3D,
    _3DMenuItems_Separation,
    _3DMenuItems_Depth,

    _3DMenuItems_Max,
}_3DMenuItems_t;

typedef enum
{
    _3DFormatMenuItems_Auto,
    _3DFormatMenuItems_SidebySide,
    _3DFormatMenuItems_TopandBottom,

    _3DFormatMenuItems_Max,
}_3DFormatMenuItems_t;

typedef enum
{
    _2Dto3DMenuItems_Off,
    _2Dto3DMenuItems_On,

    _2Dto3DMenuItems_Max,
}_2Dto3DMenuItems_t;

typedef enum
{
    DisplaySettingsMenuItems_Gamma,
#if (ENABLE_VGA_INPUT)
    DisplaySettingsMenuItems_HPos,
    DisplaySettingsMenuItems_VPos,
#endif
#if ENABLE_SHARPNESS
    DisplaySettingsMenuItems_Sharpness,
#endif
#if (ENABLE_VGA_INPUT)
    DisplaySettingsMenuItems_Clock,
    DisplaySettingsMenuItems_Phase,
#endif
#if Enable_Expansion
    DisplaySettingsMenuItems_Aspect,
#endif
#if ENABLE_RTE
    DisplaySettingsMenuItems_OD,
#endif
#if (ENABLE_VGA_INPUT)
    DisplaySettingsMenuItems_AutoAdjust,
#endif
    DisplaySettingsMenuItems_Return,

    DisplaySettingsMenuItems_Max,
}DisplaySettingsMenuItems_t;

typedef enum
{
    GammaMenuItems_Off,
    GammaMenuItems_On,

    GammaMenuItems_Max,
}GammaMenuItems_t;

typedef enum
{
    ColorRangeMenuItems_Full,
    ColorRangeMenuItems_Limit,
    ColorRangeMenuItems_Auto,

    ColorRangeMenuItems_Max,
}ColorRangeMenuItems_t;

typedef enum
{
    AspectRatioMenuItems_Full,
    AspectRatioMenuItems_4_3,
    AspectRatioMenuItems_16_9,
    AspectRatioMenuItems_1_1,
    AspectRatioMenuItems_KeepInputRatio,
    AspectRatioMenuItems_OverScan,
    AspectRatioMenuItems_Max,
}AspectRatioMenuItems_t;

typedef enum
{
    ODMenuItems_Off,
    ODMenuItems_On,

    ODMenuItems_Max,
}ODMenuItems_t;

typedef enum
{
#if AudioFunc
    OtherSettingsMenuItems_Volume,
    OtherSettingsMenuItems_AudioSource,
#endif
    OtherSettingsMenuItems_OSDTrans,
    OtherSettingsMenuItems_OSDHPos,
    OtherSettingsMenuItems_OSDVPos,
    OtherSettingsMenuItems_OSDTimeout,
#if ENABLE_OSD_ROTATION
    OtherSettingsMenuItems_OSDRotate,
#endif
#if ENABLE_FREESYNC
    OtherSettingsMenuItems_FreeSync,
#endif
    OtherSettingsMenuItems_Reset,
    OtherSettingsMenuItems_Return,

    OtherSettingsMenuItems_Max,
OtherSettingsMenuItems_DisplayLogo,
}OtherSettingsMenuItems_t;
#if AudioFunc
typedef enum
{
    AudioSourceMenuItems_Digital,
    AudioSourceMenuItems_LineIn,

    AudioSourceMenuItems_Max,
}AudioSourceMenuItems_t;
#endif

typedef enum
{
#if ENABLE_DCOFF_CHARGE
    PowerManagerMenuItems_DCOffDischarge,
#endif
    PowerManagerMenuItems_PowerSaving,
    PowerManagerMenuItems_Return
}PowerManagerMenuItems_t;

typedef enum
{
    DCOffDisChargerMenuItems_Off,
    DCOffDisChargerMenuItems_On
}DCOffDisChargerMenuItems_t;

typedef enum
{
    PowerSavingMenuItems_Off,
    PowerSavingMenuItems_On
}PowerSavingMenuItems_t;
#if ENABLE_OSD_ROTATION
typedef enum
{
    OSDRotateMenuItems_Off,
    OSDRotateMenuItems_90,
#if defined(_OSD_ROTATION_180_)
    OSDRotateMenuItems_180,
#endif
#if defined(_OSD_ROTATION_270_)
    OSDRotateMenuItems_270,
#endif
    OSDRotateMenuItems_Max,
}OSDRotateMenuItems_t;
#endif

#if ENABLE_FREESYNC
typedef enum
{
    FreeSyncMenuItems_Off,
    FreeSyncMenuItems_On,
    FreeSyncMenuItems_Max,
}FreeSyncMenuItems_t;

#define IS_HDMI_FREESYNC_ISR()  (msAPI_combo_IPGetDDRFlag_ISR()&&CURRENT_INPUT_IS_HDMI())
#define IS_HDMI_FREESYNC()      (msAPI_combo_IPGetDDRFlag()&&CURRENT_INPUT_IS_HDMI())
#define IS_DP_FREESYNC()        (msAPI_combo_IPGetDDRFlag()&&CURRENT_INPUT_IS_DISPLAYPORT())
#else
#define IS_HDMI_FREESYNC_ISR()  (FALSE)
#define IS_HDMI_FREESYNC()      (FALSE)
#define IS_DP_FREESYNC()        (FALSE)
#endif

typedef enum
{
    DisplayLogoMenuItems_Off,
    DisplayLogoMenuItems_On,
    DisplayLogoMenuItems_Max,
}DisplayLogoMenuItems_t;

#define Layer1_MenuPage         ( MenuPageIndex == MainMenu )
#define Layer1_PrevMenuPage     ( PrevMenuPageIndex == MainMenu )

#if (LiteMAX_UI == 1)
#define Layer2_MenuPage         ( MenuPageIndex >= BrightnessMenu && MenuPageIndex <= DefaultMenu)
#define Layer2_PrevMenuPage     ( PrevMenuPageIndex >= BrightnessMenu && PrevMenuPageIndex <= DefaultMenu)

#define Layer3_MenuPage         ( MenuPageIndex >= ColorSettingsMenu && MenuPageIndex <= ColorSettingsMenu )
#define Layer3_PrevMenuPage     ( PrevMenuPageIndex >= ColorSettingsMenu && PrevMenuPageIndex <= ColorSettingsMenu )
#elif (LiteMAX_Baron_UI == 1)
#define Layer2_MenuPage         ( MenuPageIndex >= BrightnessMenu && MenuPageIndex <= DefaultMenu)
#define Layer2_PrevMenuPage     ( PrevMenuPageIndex >= BrightnessMenu && PrevMenuPageIndex <= DefaultMenu)

#define Layer3_MenuPage         ( MenuPageIndex >= ColorSettingsMenu && MenuPageIndex <= ColorSettingsMenu )
#define Layer3_PrevMenuPage     ( PrevMenuPageIndex >= ColorSettingsMenu && PrevMenuPageIndex <= ColorSettingsMenu )
#else
#define Layer2_MenuPage         ( MenuPageIndex >= BriteContMenu && MenuPageIndex <= PowerManagerMenu)
#define Layer2_PrevMenuPage     ( PrevMenuPageIndex >= BriteContMenu && PrevMenuPageIndex <= PowerManagerMenu)

#define Layer3_MenuPage         ( MenuPageIndex >= ContrastMenu && MenuPageIndex <= PowerSavingMenu )
#define Layer3_PrevMenuPage     ( PrevMenuPageIndex >= ContrastMenu && PrevMenuPageIndex <= PowerSavingMenu )
#endif

#if (NEW_MTK_UI == 1)
#define IndependentColor_MenuPage ( MenuPageIndex >= IndependentHueRMenu && MenuPageIndex <= IndependentBrightnessYMenu )
#endif

#if HotExpansion
#define HotMenuPage             ( MenuPageIndex >= HotExpansionMenu && MenuPageIndex <= HotInputSelectMenu)
#else
#define HotMenuPage             ( MenuPageIndex >= HotInputSelectMenu && MenuPageIndex <= HotInputSelectMenu)
#endif

#endif
