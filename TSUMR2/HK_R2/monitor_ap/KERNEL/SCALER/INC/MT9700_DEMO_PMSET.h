#ifndef _MS_PMDEF_H
#define _MS_PMDEF_H


#define PM_CLOCK                		           XTAL//SLOW_CLK   //modified clock rate on PM mode - A059

#define PM_SUPPORT_SOG_TIME_SHARE       (0) 	// (1)supports to SOG time sharing function to reduce power consumption  (0)doesn't support to SOG time sharing function
#define PM_SUPPORT_DVI_TIME_SHARE       (0) 	// (1)supports to DVI time sharing function to reduce power consumption  (0)doesn't support to DVI time sharing function
#define PM_SUPPORT_ADC_TIME_SHARE       (0)
#define PM_SUPPORT_WAKEUP_DVI          	(1) 	// (1)supports to DVI wakeup;                                            (0)doesn't support to DVI wakeup
#define PM_SUPPORT_WAKEUP_HDMI_SQH      (0)
#define PM_SUPPORT_WAKEUP_HDMI_SCDC5V   (0) // 9700 TBD.
#define PM_SUPPORT_WAKEUP_DP          	(ENABLE_DP_INPUT)
#define PM_SUPPORT_DVI5V_TurnonRterm   	(0)
#define PM_POWERkEY_GETVALUE           	(1)
#define PM_CABLEDETECT_USE_GPIO         (0)
#define PM_CABLE_DETECT_USE_SAR         (CABLE_DETECT_VGA_USE_SAR||CABLE_DETECT_VGA_USE_SAR)

#define PM_POWERSAVING_WAKEUP_GPIO     	(0)
#define PM_POWERSAVING_WAKEUP_SAR      	(1)
#define PM_POWERSAVING_WAKEUP_SOG       (1)
#define PM_POWERSAVING_WAKEUP_MCCS      (1)
#define PM_POWERSAVING_SARmode        	ePMSAR_SAR2

#define PM_POWEROFF_WAKEUP_GPIO        	(0)
#define PM_POWEROFF_WAKEUP_SAR         	(0)
#define PM_POWEROFF_WAKEUP_SOG         	(0)
#define PM_POWEROFF_WAKEUP_MCCS        	(1)
#define PM_POWEROFF_SARmode          	ePMSAR_SAR2
#if PM_SUPPORT_WAKEUP_DVI
#define DVI5VP0_GPIO 1
#define DVI5VP1_GPIO 0
#endif

#define XBYTE       ((unsigned char volatile xdata *) 0)

//-------------------------------------------------------------------------------------------------
//  PM CLOCK
//-------------------------------------------------------------------------------------------------
#define   RCOSC         0
#define   XTAL          1
#define   SLOW_CLK      2

enum
{
    RECOVERY_CLK_24M = 0,
    RECOVERY_CLK_12M
};

enum
{
    SLOW_CLK_24M = 0,
    SLOW_CLK_12M = 1,
    SLOW_CLK_8M = 2,
    SLOW_CLK_6M = 3,
    SLOW_CLK_5M = 4,
    SLOW_CLK_4M = 5,
    SLOW_CLK_3M = 7,
}; // Slow clock = 24M/(DIV + 1), DIV = 0~31


//-------------------------------------------------------------------------------------------------
//  VGA
//-------------------------------------------------------------------------------------------------
#define REG_SYNC_DET    REG_PM_A0
#define EN_CSYNC_DET    _BIT7
#define EN_HSYNC_DET    _BIT6
#define EN_VSYNC_DET    _BIT5
#define EN_SOG_DET      _BIT4
#define EN_HVSYNC_DET   _BIT3
#define EN_HSYNC_LOST   _BIT2
#define EN_VSYNC_LOST   _BIT1
#define EN_SOG_LOST     _BIT0
#define EN_SYNC_DET_SET     EN_HVSYNC_DET
#define EN_SYNC_DET_MASK    EN_HSYNC_DET|EN_VSYNC_DET|EN_HVSYNC_DET|EN_CSYNC_DET

//-------------------------------------------------------------------------------------------------
//  DVI
//-------------------------------------------------------------------------------------------------
#define REG_DVI_DET         REG_PM_9F
#define EN_DVI_LOST_A       _BIT5
#define EN_DVI_LOST_BC      _BIT4
#define EN_DVI_DET_A        _BIT1
#define EN_DVI_DET_BC       _BIT0
#define EN_DVI_DET_SET      EN_DVI_DET_BC
#define EN_DVI_DET_MASK     EN_DVI_DET_A|EN_DVI_DET_BC
//---------------------------------------
#define REG_DVI_CTRL        REG_PM_BE
#define EN_DVI_CHEN_AUTO    (_BIT14|_BIT15)
#define EN_DVI_CTRL         _BIT13
#define EN_DVI_ALWAYS_ON    _BIT12
#define DVI_CTRL_PERIOD     0x00    // <Janus>2010/07/14: valid range 0~7. control period = (DVI_CTRL_PERIOD+1) * 131.2ms
#define DVI_ON_PERIOD       0x08    // <Janus>2010/07/14: valid range 1~7. Period = DVI_ON_PERIOD * 8.2 ms
#define EN_DVI_CTRL_SET     EN_DVI_CTRL|(DVI_CTRL_PERIOD<<8)|DVI_ON_PERIOD
#define EN_DVI_CTRL_MASK    0xFFFF
//----------------------------------------
#define REG_DVI_CHEN        REG_PM_BF
#define EN_DVI_RTERM        _BIT3
#define EN_DVI0             _BIT0
#define EN_DVI1             _BIT1
#define EN_DVI0_DVI1        _BIT1|_BIT0
#define EN_DVI_CHEN_SET     EN_DVI_RTERM|EN_DVI0_DVI1
#define EN_DVI_CHEN_MASK    EN_DVI_RTERM|EN_DVI0_DVI1

//-------------------------------------------------------------------------------------------------
//  SAR
//-------------------------------------------------------------------------------------------------
#define REG_SAR_GPIO     	REG_PM_84
#define EN_SAR_DET          _BIT7
#define INV_SAR_POL         _BIT6
#define EN_SAR_DET_SET      EN_SAR_DET
#define EN_SAR_DET_MASK     EN_SAR_DET|INV_SAR_POL
//-- GPIO ---------------------------------
#define REG_PMGPIO_EN    	REG_PM_60  // use word access to this register
#define EN_GPIO21_DET    	_BIT0      // enable wakeup detect of PM GPIO 21
#define EN_GPIO20_DET    	_BIT1      // enable wakeup detect of PM GPIO 20
#define EN_GPIO25_DET      	_BIT2      // enable wakeup detect of PM GPIO 25
#define EN_GPIO26_DET      	_BIT3      // enable wakeup detect of PM GPIO 26
#define EN_GPIO00_DET      	_BIT4      // enable wakeup detect of PM GPIO 00
#define EN_GPIO01_DET      	_BIT5      // enable wakeup detect of PM GPIO 01
#define EN_GPIO24_DET    	_BIT6      // enable wakeup detect of PM GPIO 24
#define EN_GPIO27_DET    	_BIT7      // enable wakeup detect of PM GPIO 27
#define EN_GPIO30_DET      	_BIT8      // enable wakeup detect of PM GPIO 30
#define EN_GPIO31_DET      	_BIT9      // enable wakeup detect of PM GPIO 31
#define EN_GPIO36_DET      	_BIT10     // enable wakeup detect of PM GPIO 36
#define EN_GPIO37_DET      	_BIT11     // enable wakeup detect of PM GPIO 37
#define EN_GPIO02_DET    	_BIT12     // enable wakeup detect of PM GPIO 02
#define EN_GPIO11_DET    	_BIT13     // enable wakeup detect of PM GPIO 11
#define EN_GPIO50_DET      	_BIT14     // enable wakeup detect of PM GPIO 50
#define EN_GPIO51_DET      	_BIT15     // enable wakeup detect of PM GPIO 51

#define REG_PMGPIO_POL     	REG_PM_62  // use word access to this register
#define INV_GPIO21_POL     	_BIT0      // invert input signal polarity of PM GPIO 21
#define INV_GPIO20_POL     	_BIT1      // invert input signal polarity of PM GPIO 20
#define INV_GPIO25_POL     	_BIT2      // invert input signal polarity of PM GPIO 25
#define INV_GPIO26_POL     	_BIT3      // invert input signal polarity of PM GPIO 26
#define INV_GPIO00_POL     	_BIT4      // invert input signal polarity of PM GPIO 00
#define INV_GPIO01_POL     	_BIT5      // invert input signal polarity of PM GPIO 01
#define INV_GPIO24_POL     	_BIT6      // invert input signal polarity of PM GPIO 24
#define INV_GPIO27_POL     	_BIT7      // invert input signal polarity of PM GPIO 27
#define INV_GPIO30_POL     	_BIT8      // invert input signal polarity of PM GPIO 30
#define INV_GPIO31_POL     	_BIT9      // invert input signal polarity of PM GPIO 31
#define INV_GPIO36_POL     	_BIT10     // invert input signal polarity of PM GPIO 36
#define INV_GPIO37_POL     	_BIT11     // invert input signal polarity of PM GPIO 37
#define INV_GPIO02_POL     	_BIT12     // invert input signal polarity of PM GPIO 02
#define INV_GPIO11_POL     	_BIT13     // invert input signal polarity of PM GPIO 11
#define INV_GPIO50_POL     	_BIT14     // invert input signal polarity of PM GPIO 50
#define INV_GPIO51_POL     	_BIT15     // invert input signal polarity of PM GPIO 51

#define REG_PMGPIO_STS    	REG_PM_64  // use word access to this register
#define GPIO21_INT         	_BIT0      // wakeup status of PM GPIO 21
#define GPIO20_INT         	_BIT1      // wakeup status of PM GPIO 20
#define GPIO25_INT         	_BIT2      // wakeup status of PM GPIO 25
#define GPIO26_INT         	_BIT3      // wakeup status of PM GPIO 26
#define GPIO00_INT         	_BIT4      // wakeup status of PM GPIO 00
#define GPIO01_INT         	_BIT5      // wakeup status of PM GPIO 01
#define GPIO24_INT         	_BIT6      // wakeup status of PM GPIO 24
#define GPIO27_INT         	_BIT7      // wakeup status of PM GPIO 27
#define GPIO30_INT         	_BIT8      // wakeup status of PM GPIO 30
#define GPIO31_INT         	_BIT9      // wakeup status of PM GPIO 31
#define GPIO36_INT         	_BIT10     // wakeup status of PM GPIO 36
#define GPIO37_INT         	_BIT11     // wakeup status of PM GPIO 37
#define GPIO02_INT         	_BIT12     // wakeup status of PM GPIO 02
#define GPIO11_INT         	_BIT13     // wakeup status of PM GPIO 11
#define GPIO50_INT         	_BIT14     // wakeup status of PM GPIO 50
#define GPIO51_INT         	_BIT15     // wakeup status of PM GPIO 51

#define PM_POWERKEY_INT     PM_POWERKEY_WAKEUP

#define EN_GPIO_DET_SET     PM_POWERKEY_WAKEUP

#define EN_GPIO_DET_MASK    ( EN_GPIO21_DET | EN_GPIO20_DET | EN_GPIO25_DET | EN_GPIO26_DET \
                                       | EN_GPIO00_DET | EN_GPIO01_DET | EN_GPIO24_DET | EN_GPIO27_DET \
                                       | EN_GPIO30_DET | EN_GPIO31_DET | EN_GPIO36_DET | EN_GPIO37_DET \
                                       | EN_GPIO02_DET | EN_GPIO11_DET | EN_GPIO50_DET | EN_GPIO51_DET )

#define INV_GPIO_POL_SET  	( PM_POWERKEY_INT )
#define INV_GPIO_POL_MASK   ( GPIO21_INT | GPIO20_INT | GPIO25_INT | GPIO26_INT \
                                       | GPIO00_INT | GPIO01_INT | GPIO24_INT | GPIO27_INT \
                                       | GPIO30_INT | GPIO31_INT | GPIO36_INT | GPIO37_INT \
                                       | GPIO02_INT | GPIO11_INT | GPIO50_INT | GPIO51_INT )

//----------------------------------------
// SAR
//----------------------------------------
#define REG_SAR_ANYKEY          REG_003A24
#define REG_SARADC              REG_003A60
#define REG_SAR_CH_EN           REG_003A68

#define EN_SAR_05V              0x00
#define EN_SAR_14V              0x40
#define EN_SAR_25V              0xC0

//-------------------------------------------------------------------------------------------------
//  MCCS
//-------------------------------------------------------------------------------------------------
//#define REG_D2B_EN              REG_PM_90
//#define D2B_EN_A0               _BIT3
//#define D2B_EN_A1               _BIT2
//#define D2B_EN_D0               _BIT1
//#define D2B_EN_D1               _BIT0
//#define REG_D2B_EN_MASK         D2B_EN_A0|D2B_EN_A1|D2B_EN_D0|D2B_EN_D1
//#define REG_D2B_EN_SET          D2B_EN_A0|D2B_EN_A1|D2B_EN_D0|D2B_EN_D1

//-------------------------------------------------------------------------------------------------
//  DDC
//-------------------------------------------------------------------------------------------------
//#define REG_DDC_EN              REG_PM_91
//#define DDC_EN_A0               _BIT3
//#define DDC_EN_A1               _BIT2
//#define DDC_EN_D0               _BIT1
//#define DDC_EN_D1               _BIT0
//#define REG_DDC_EN_MASK         DDC_EN_A0|DDC_EN_A1|DDC_EN_D0|DDC_EN_D1
//#define REG_DDC_EN_SET          DDC_EN_A0|DDC_EN_A1|DDC_EN_D0|DDC_EN_D1

//-------------------------------------------------------------------------------------------------
//  Others
//-------------------------------------------------------------------------------------------------
typedef enum
{
    DVI_AVT_PORT0 = BIT0,
    DVI_AVT_PORT1 = BIT1,
    DVI_AVT_PORT2 = BIT2,
    DVI_AVT_PORT3 = BIT3,
    DVI_AVT__PORT_NONE = 0,
}EN_DVI_ACT_PORT;
//-------------------------------------------------------------------------------------------------
//  Wakeup Event
//-------------------------------------------------------------------------------------------------
#define REG_EVENT1          REG_PM_85
#define HSYNC_DET_0         BIT7
#define VSYNC_DET_0         BIT6
#define SOG_DET_0           BIT5
#define HV_DET_0            BIT4
#define REG_EVENT2          REG_PM_86
#define D2B_WAKEUP          BIT7
#define MHL_CBUS_WAKEUP     BIT6
#define DVI_CLK_DET_0       BIT5
#define DVI_CLK_DET_1       BIT0
#define DVI_CLK_DET_2_3       BIT1
//#define SAR_IN_DET          BIT4
#define DP_WAKEUP           BIT3
#define CS_DET              BIT2 //composite sync detect status
#define CEC_WAKEUP          BIT6
#define REG_EVENT3          REG_PM_8B
#define REG_EVENT4          REG_CECALIVE_11_L
#define REG_DVI_RAW_CLK     REG_PM_A2
#define DVI_RAW_CLK_DET     BIT6
// Slow clock recovery event
#define FIQ_PD51_1_UART_DET       BIT7
#define FIQ_PD51_0_UART_DET       BIT8
#define FIQ_DW_0_UART_DET         BIT9
#define FIQ_HKR2_0_UART_DET       BIT10

#define IRQ_D2B_INT_0             BIT2
#define IRQ_GPIO_C_WKUP_0         BIT3  
#define IRQ_GPIO_C_WKUP_1         BIT4  
#define IRQ_GPIO_C_WKUP_2         BIT6  
#define IRQ_GPIO_C_WKUP_3         BIT7 
#define IRQ_PM_UART0_INT          BIT10
#define IRQ_GPIO_C_WKUP_4         BIT16 
#define IRQ_GPIO_C_WKUP_5         BIT17 
#define IRQ_GPIO_C_WKUP_6         BIT18 
#define IRQ_GPIO_C_WKUP_7         BIT19 
#define IRQ_GPIO_C_WKUP_8         BIT20 
#define IRQ_GPIO_C_WKUP_9         BIT21
#define IRQ_PD_CC_SWITCH_INT      BIT25
#define IRQ_U31_WAKEUP_INT_UP     BIT26	
#define IRQ_OCP_SAR_INT           BIT31
// Select slow clock recover event
//-------------------------------------------------------------------------------------------------
//  Slow Clock
//-------------------------------------------------------------------------------------------------
#define HK_R2_RECOVERY_CLOCK       RECOVERY_CLK_24M
#define HK_R2_SLOW_CLOCK           SLOW_CLK_3M

#define SLOW_CLK_TIMER             2//sec
#endif

