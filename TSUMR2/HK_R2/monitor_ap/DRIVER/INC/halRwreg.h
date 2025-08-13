#ifndef _HALRWREG_H_
#define _HALRWREG_H_

// interrupt
// NON PM FIQ
#define INT_NPM_FIQ_SECU2HK51_ENABLE(Enable) (msWriteByteMask( REG_10190E, (Enable)?(0):(BIT2), BIT2 ))
// NON PM IRQ
#if(CHIP_ID == CHIP_MT9701)
#define INT_NPM_IRQ_SCINT_ENABLE(Enable)     (msWriteByteMask( REG_10192F, (Enable)?(0):(BIT0), BIT0 ))
#define INT_NPM_IRQ_DPRX_INT_ENABLE(Enable)  (msWriteByteMask( REG_101928, (Enable)?(0):(BIT4), BIT4 ))
#define INT_NPM_IRQ_DPRX2_INT_ENABLE(Enable) (msWriteByteMask( REG_101928, (Enable)?(0):(BIT5), BIT5 ))
#else
#define INT_NPM_IRQ_SCINT_ENABLE(Enable)     (msWriteByteMask( REG_10196F, (Enable)?(0):(BIT0), BIT0 ))
#define INT_NPM_IRQ_DPRX_INT_ENABLE(Enable)  (msWriteByteMask( REG_101968, (Enable)?(0):(BIT4), BIT4 ))
#define INT_NPM_IRQ_DPRX2_INT_ENABLE(Enable) (msWriteByteMask( REG_101968, (Enable)?(0):(BIT5), BIT5 ))
#endif

// PM FIQ
#define INT_PM_FIQ_ML_ENABLE(Enable)         (msWriteByteMask( REG_002B08, (Enable)?(0):(BIT4), BIT4 ))
#define INT_PM_FIQ_PD512HK51_ENABLE(Enable)  (msWriteByteMask( REG_002B09, (Enable)?(0):(BIT6), BIT6 ))
// PM IRQ
#define INT_PM_IRQ_NPMIRQ_ENABLE(Enable)     (msWriteByteMask( REG_002B28, (Enable)?(0):(BIT0), BIT0 ))
//#define INT_PM_IRQ_D2B_ENABLE(Enable)        (msWriteByteMask( REG_002B28, (Enable)?(0):(BIT2), BIT2 ))
#define INT_IRQ_D2B_ENABLE(Enable)           (msWriteByteMask(0x000D28, (Enable)?(0):(BIT2), BIT2))

#define INT_IRQ_DVI_ENABLE(Enable)    (old_msWriteByteMask( REG_2B19, ((Enable)?(0):(BIT7)), BIT7 ))


#define INT_SYNC_CHANGE_TRIGGER_TYPE() (msWriteByteMask( SC00_CA, BIT1 | BIT0, BIT1 | BIT0)) // BIT0 for level trigger, BIT1 for active high
#define INT_SCALER_A(Enable) (msWriteByte(SC00_CE, Enable))
#define INT_SCALER_B(Enable) (msWriteByte(SC00_CF, Enable))
#define INT_STATUS_CLEAR()   (msWriteByte(SC00_CC, 0), msWriteByte(SC00_CD, 0))
#define INT_SCALER_A_ISR(Enable) (MEM_MSWRITE_BYTE(SC00_CE, Enable))
#define INT_SCALER_B_ISR(Enable) (MEM_MSWRITE_BYTE(SC00_CF, Enable))
#define INT_STATUS_A_ISR(Status) (MEM_MSREAD_BYTE(SC00_CC) & (Status))
#define INT_STATUS_B_ISR(Status) (MEM_MSREAD_BYTE(SC00_CD) & (Status))
#define INT_STATUS_CLEAR_ISR() (MEM_MSWRITE_BYTE(SC00_CC, 0), MEM_MSWRITE_BYTE(SC00_CD, 0))

#if 0 //Leo-temp
#define UART_READ_ES()  (ES)
#define UART_RI_FLAG()  (RI)
#define UART_TI_FLAG()  (TI)
#define UART_CLR_RI_FLAG()  (RI = 0)
#define UART_CLR_TI_FLAG()  (TI = 0)
#define UART_BUFFER()  //(SBUF)
#else
#define UART_READ_ES()  //(ES)
#define UART_RI_FLAG()  //(RI)
#define UART_TI_FLAG()  //(TI)
#define UART_CLR_RI_FLAG()  //(RI = 0)
#define UART_CLR_TI_FLAG()  //(TI = 0)
#define UART_BUFFER()  //(SBUF)
#endif

#if UART1
#define UART1_READ_ES()  ((IEN2&ES1) == ES1)
#define UART1_RI_FLAG()  //((S1CON & RI1) == RI1)
#define UART1_TI_FLAG()  //((S1CON & TI1) == TI1)
#define UART1_CLR_RI_FLAG()  //(S1CON &= ~RI1)
#define UART1_CLR_TI_FLAG()  //(S1CON &= ~TI1)
#define UART1_BUFFER()  //(S1BUF)
#endif

// cache
#define CACHE_STATUS()   (_bit3_(msReadByte(REG_001018)))
#define CACHE_ENABLE()   (msWriteBit( REG_001018, 1, _BIT3 ), msWriteBit( REG_002BA0, 0, _BIT0 ))
#define CACHE_DISABLE()  (msWriteBit( REG_002BA0, 1, _BIT0 ), msWriteBit( REG_001018, 0, _BIT3 ))

// watch dog
#define WDT_ENABLE()        {msWriteByte( REG_002C0B, (WATCH_DOG_TIME_RESET * CRYSTAL_CLOCK)>>24 );\
                             msWriteByte( REG_002C0A, (WATCH_DOG_TIME_RESET * CRYSTAL_CLOCK)>>16 );\
                             msWriteByte( REG_002C09, (WATCH_DOG_TIME_RESET * CRYSTAL_CLOCK)>>08 );\
                             msWriteByte( REG_002C08, (WATCH_DOG_TIME_RESET * CRYSTAL_CLOCK)>>00 );}
#define WDT_DISABLE()       (msWrite4Byte( REG_002C08, 0x00 ))
#define WDT_INTERVAL(secs)  (msWrite4Byte( REG_002C08, secs * CRYSTAL_CLOCK ))
#define WDT_CLEAR()         (msWriteBit( REG_002C00, 1, _BIT0 ))

// hdcp
#define HDCP_PORT_ENABLE()      0;//(old_msWriteByteMask(SC0_BD,0,BIT7))
#define HDCP_PORT_DISABLE()     0;//(old_msWriteByteMask(SC0_BD,BIT7,BIT7))
#define HDCP_CLEAR_STATUS()         (old_msWriteByte( REG_1539, 0xFF ))
#define HDCP_CPU_WRITE_ENABLE(Value, Mask)    (old_msWriteByteMask(REG_150F, Value, Mask))
#define HDCP_CPU_RW_ADDRESS(Value)  (old_msWrite2ByteMask(REG_150C, Value, 0x03FF))
#define HDCP_LOAD_ADDR_GEN_PULSE()  (old_msWriteByte(REG_153A, 0x10))
#define HDCP_CPU_WRITE_BKSV_START() (HDCP_CPU_WRITE_ENABLE(0x80, 0xC0), HDCP_CPU_RW_ADDRESS(0), HDCP_LOAD_ADDR_GEN_PULSE())

#if 1//CHIP_ID==CHIP_TSUMF // TBD
#define HDCP_XIU_WRITE_KEY_ENABLE()  (old_msWriteByteMask(REG_1A9A,0x0D,0x0D), old_msWrite2ByteMask(REG_1900, 0, 0x03FF))
#define HDCP_XIU_WRITE_KEY_DISABLE()  (old_msWriteByteMask(REG_1A9A,0,0x0D))
#define HDCP_XIU_WRITE_KEY(Value)  (old_msWriteByte(REG_1902,Value))
#endif
#define HDCP_CPU_WRITE_DATA(Value)   (old_msWriteByte( REG_150E, Value ), old_msWriteByte(REG_153A, 0x08))
#define HDCP_CPU_WRITE_BKSV(Address, Value)  (HDCP_CPU_WRITE_DATA(Value))
#define HDCP_CPU_WR_BUSY()    (_bit4_(old_msReadByte(REG_152E)))
#define HDCP_CPU_WRITE_BCAPS(Value) {old_msWriteByteMask(REG_150F, 0x80, 0xC0);\
                                                                    old_msWrite2ByteMask(REG_150C, 0x0040, 0x03FF);\
                                                                    old_msWriteByte(REG_153A, 0x10);\
                                                                    old_msWriteByte(REG_150E, Value);\
                                                                    old_msWriteByte(REG_153A, 0x08);}
#define HDCP_MISC_SETTING(Value) (old_msWrite2Byte( REG_1500, Value ))
#define HDCP_ENABLE_DDC()   (old_msWriteByteMask( REG_150D, BIT2, BIT2 ))//(old_msWriteByteMask( REG_3E74, BIT5, BIT5 ))
#define HDCP_RESET(Enable)    (old_msWriteByteMask( REG_1752, (Enable)?(BIT4):(0),  BIT4))
#define HDCP_FORCE_ACK()    (old_msWriteByteMask(REG_1501, 0x40, 0x40))

// dvi
#define DVI_A_RCK_CTRL(On)  (old_msWriteByteMask( REG_01CE, ((On) ? 0 : BIT0), BIT0 )) //PortA clk
#define DVI_B_RCK_CTRL(On)  (old_msWriteByteMask( REG_01CE, ((On) ? 0 : BIT1), BIT1 )) //PortB clk
#define DVI_C_RCK_CTRL(On)  (old_msWriteByteMask( REG_01CE, ((On) ? 0 : BIT2), BIT2 )) //PortC clk
#define DVI_RELATED_CLOCK_CTRL_0(Value, Mask)    (old_msWrite2ByteMask( REG_1B16, Value, Mask ), old_msWrite2ByteMask( REG_1A92, Value, Mask ))
#define DVI_RELATED_CLOCK_CTRL_1(Value, Mask)    (old_msWrite2ByteMask( REG_1B18, Value, Mask ), old_msWrite2ByteMask( REG_1A94, Value, Mask ))
#define DVI_DE_STABLE()  (TRUE)// TBD (  (old_msReadByte( REG_155D) & (BIT6|BIT5))== BIT6) // DE stable
#define DVI_CLK_STABLE()  (  (old_msReadByte( REG_1B64) & (BIT3))== BIT3) // CLK stable
#define DVI_CLOCK_IS_MISSING()  ((msReadByte(SC00_CA)&BIT6) == BIT6)
#define DVI_RESET(Enable)    (old_msWriteByteMask( REG_1752, (Enable)?(0x85):(0),  0x85))

// dp
#define DP_INTERLACED_VIDEO_STREAM()    (FALSE)

// scaler
#define SC0_SUB_BANK_SEL(SubBank)               (msWriteByte( SC00_00, SubBank ))
#define SC0_INPUT_SELECT(Value)                 (msWriteByteMask(SC00_02, Value, BIT2|BIT1|BIT0))
#define SC0_READ_INPUT_SETTING()                (msReadByte(SC00_02))
#define SC0_RECOVER_INPUT_SETTING(Value)        (msWriteByte(SC00_02, Value))
#define SC0_OUTPUT_LOCK_MODE(LockInput)         (msWriteByteMask(SC00_02, (LockInput)?(0):(BIT7) , BIT7))
#define SC0_SCALER_POWER_DOWN(Value, Mask)      (msWriteByteMask( SC00_F0, Value, Mask ))
#define SC0_SCALER_RESET(Value)                 (msWriteByte( SC00_F1, Value ))
#define SC0_NORMAL_MODE()                       (msWriteByte( SC00_F8, 0x00 ))
#define SC0_HPEROID_DETECT_MODE(Enable16lines)  (msWriteByte(SC00_DF, (Enable16lines)?(0):(BIT7))) //8/16 line switch. [7:4]: 0000: 16L, 1000: 8L, 0100: 4L, 0010: 2L
#define SC0_HPERIOD_16LINE_MODE()               ((msReadByte(SC00_DF)&BIT7)?FALSE:TRUE)
#define SC0_READ_HPEROID()                      (msRead2Byte(SC00_E4)&MASK_13BIT)
#define SC0_READ_VTOTAL()                       (msRead2Byte(SC00_E2)&MASK_13BIT)
#define SC0_READ_SYNC_STATUS()                  (msReadByte(SC00_E1))
#define SC0_READ_AUTO_START_H()                 (msRead2Byte(SC00_80)&MASK_13BIT)
#define SC0_READ_AUTO_END_H()                   (msRead2Byte(SC00_84)&MASK_13BIT)
#define SC0_READ_AUTO_START_V()                 (msRead2Byte(SC00_7E)&MASK_12BIT)
#define SC0_READ_AUTO_END_V()                   (msRead2Byte(SC00_82)&MASK_12BIT)
#define SC0_READ_AUTO_WIDTH()                   ((SC0_READ_AUTO_END_H()-SC0_READ_AUTO_START_H()+1)*(Input420Flag+1))
#define SC0_READ_AUTO_HEIGHT()                  (SC0_READ_AUTO_END_V()-SC0_READ_AUTO_START_V()+1)
#define SC0_READ_IMAGE_HEIGHT()                 (msRead2Byte(SC00_09)&MASK_12BIT)
#define SC0_READ_IMAGE_WIDTH()                  ((msRead2Byte(SC00_0B)&MASK_12BIT)*(Input420Flag+1))
#define SC0_SET_IMAGE_HEIGHT(Value)             (msWrite2ByteMask(SC00_09, Value, MASK_12BIT))
#define SC0_SET_IMAGE_WIDTH(Value)              (msWrite2ByteMask(SC00_0B, Value, MASK_12BIT))
#define SC0_SET_IMAGE_START_H(Value)            (msWrite2Byte(SC00_07, (Value)&MASK_12BIT))
#define SC0_SET_IMAGE_START_V(Value)            (msWrite2Byte(SC00_05, (Value)&MASK_12BIT))
#if CHIP_ID == CHIP_MT9701
#define SC0_WHITE_SCREEN_ENABLE()               (msWriteByteMask(SC65_43,BIT5,(BIT4|BIT5)))
#define SC0_BLACK_SCREEN_ENABLE()               (msWriteByteMask(SC65_43,BIT4,(BIT4|BIT5)))
#define SC0_BLACK_WHITE_SCREEN_DISABLE()        (msWriteByteMask(SC65_43,0,(BIT4|BIT5)))
#define SC0_SET_OUTPUT_VTOTAL(Value)            (msWrite2ByteMask(SC00_1E,((((Value)&BIT12)<<1)|((Value)&MASK_12BIT)), (BIT13|MASK_12BIT))) // Vtt: BIT13|[11:0]
#else
#define SC0_WHITE_SCREEN_ENABLE()               (msWriteByteMask(SC10_43,BIT5,(BIT4|BIT5)))
#define SC0_BLACK_SCREEN_ENABLE()               (msWriteByteMask(SC10_43,BIT4,(BIT4|BIT5)))
#define SC0_BLACK_WHITE_SCREEN_DISABLE()        (msWriteByteMask(SC10_43,0,(BIT4|BIT5)))
#define SC0_SET_OUTPUT_VTOTAL(Value)            (msWrite2ByteMask(SC00_1E, Value, MASK_12BIT))
#endif
#define SC0_DE_ONLY_MODE()                      ((msReadByte(SC00_04)&BIT6) == BIT6)
#define SC0_VIDEO_FIELD_INVERSION()             ((msReadByte(SC00_E9)&BIT3) == BIT3)
#define SC0_READ_POWER_DOWN_STATUS()            (msReadByte(SC00_F0))
#define SC0_ADC_COAST_ENABLE(Value)             (msWriteByte(SC00_ED, Value))
#define SC0_ADC_COAST_START(Value)              (msWriteByte(SC00_EE, Value))
#define SC0_ADC_COAST_END(Value)                (msWriteByte(SC00_EF, Value))
#define SC0_READ_ADC_COAST_ENABLE_VALUE()       (msReadByte(SC00_ED))
#define SC0_READ_ADC_COAST_START_VALUE()        (msReadByte(SC00_EE))
#define SC0_READ_ADC_COAST_END_VALUE()          (msReadByte(SC00_EF))
#define SC0_GLITCH_REMOVAL_ENABLE(Value)        (msWriteByte(SC00_F3, Value))
#define SC0_SAMPLE_CLOCK_INVERT(Value)          (msWriteByteMask(SC00_F3, ((Value)>165)?(BIT0):(0), BIT0))
#define SC0_VSYNC_WIDTH_REPORT(Enable)          (msWriteByteMask(SC00_FA, (Enable)?(BIT0):(0),BIT0))
#define SC0_READ_VSYNC_WIDTH()                  (msReadByte(SC00_E2))
#if CHIP_ID == CHIP_MT9701
#define SET_420to444_ENABLE()                   (msWriteByteMask(SC22_81, BIT7, BIT7))
#define SET_420to444_DISABLE()                  (msWriteByteMask(SC22_81,    0, BIT7))
#else
#define SET_420to444_ENABLE()                   (msWriteByteMask(SC0D_84,    0, BIT0))
#define SET_420to444_DISABLE()                  (msWriteByteMask(SC0D_84, BIT0, BIT0))
#endif

// auto
#define AUTO_POSITION_RESULT_READY()                        (WaitAutoStatusReady(SC00_7B, BIT1))
#define AUTO_POSITION_SET_VALID_VALUE(Value)                (msWriteByteMask(SC00_7C, (Value) << 4, 0xF0))
#define AUTO_POSITION_READ_VALID_VALUE()                    (msReadByte(SC00_7C)>>4)
#define AUTO_POSITION_READ_HSTART()                         (GetAutoValue(SC00_80)&MASK_12BIT)
#define AUTO_POSITION_READ_HEND()                           (GetAutoValue(SC00_84)&MASK_12BIT)
#define AUTO_POSITION_READ_VSTART()                         (GetAutoValue(SC00_7E)&MASK_11BIT)
#define AUTO_POSITION_READ_VEND()                           (GetAutoValue(SC00_82)&MASK_11BIT)
#define AUTO_POSITION_READ_TRANSTION_POSITION(Delaytime)    (GetTranstionPosition( Delaytime, SC00_80 ))
#define AUTO_PHASE_RESULT_READY()                           (drvADC_WaitAutoStatusReady(SC00_8B, BIT1))
#define AUTO_PHASE_READ_VALUE()                             ((((DWORD)msRead2Byte(SC00_8E))<<16)|msRead2Byte(SC00_8C))

// OSD
#define OSD_MENU_EXIST()    ( (old_msReadByte( (OSD_MAIN_WND<<5)+OSD2_00 )&MWIN_B) == MWIN_B ) // V

// misc
#define MPLL_POWER_UP(Enable) (old_msWriteByteMask(REG_1ED1, (Enable)?(0):(BIT0), BIT0))
#define LPLL_POWER_UP(Enable) (old_msWriteByteMask(REG_1ED1, (Enable)?(0):(BIT4), BIT4)) // V, Y
#define MPLL_CLOCK_ADC(Enable)  (old_msWriteByteMask(REG_1EDC, (Enable)?(0):(BIT4|BIT5), BIT4|BIT5))
#define ADC_PLL_LOCKING_EDGE(TrailingEdge)  (msWriteByteMask(REG_ADC_DTOP_07_L, (TrailingEdge)?(BIT5):(0), BIT5))
#define WRITE_POWER_ON_TABLE()   msWritePowerOnTble()

// DCR
#define HISTOGRAM_WHOLE_VERTICAL_RANGE_EN(Enable)   (msWriteByteMask(SC03_B5, (Enable)?(0):(BIT0), BIT0))
#define HISTOGRAM_RGB_TO_Y_EN(Enable)   (msWriteByteMask(SC07_40, (Enable)?(BIT2):(0), BIT2))
#define MWE_FUNCTION_EN(Enable)   (msWriteByteMask(SC00_5C, (Enable)?(BIT3):(0), BIT3), msWriteByteMask(SC0B_28, (Enable)?(BIT1):(0), BIT1)) // SCB_28[1] follows sub window settings
#define STATISTIC_REQUEST_MAIN_ENABLE()  (msWriteByte(SC07_B8, (BIT2|BIT1)))
#define STATISTIC_REQUEST_SUB_ENABLE()  (msWriteByte(SC07_B8, (BIT2|BIT0)))
#define STATISTIC_REQUEST_EN(Enable) (msWriteByteMask(SC07_B8, (Enable)?(BIT2):(0), BIT2))
#define STATISTIC_ACKNOWLEDGE() ((msReadByte(SC07_B8)&BIT3) == BIT3)
#define Y_CHANNEL_MIN_PIXEL()   (msReadByte( SC07_39 ))
#define Y_CHANNEL_MAX_PIXEL()   (msReadByte( SC07_3A ))
#define TOTAL_PIXEL_COUNT()     1//((DWORD)(msRead2Byte(SC0_1A)+1)*(msRead2Byte(SC0_1C)-msRead2Byte(SC0_18)+1))
#define TOTAL_PIXEL_WEIGHT()    (msRead2Byte(SC07_36))
#define AVERAGE_LUMINANCE(TotalWeight, TotalCount)  ((((DWORD)(TotalWeight))*16384+((TotalCount)>>1))/(TotalCount))

// ICE
#define ICE_COLOR_R     0
#define ICE_COLOR_G     1
#define ICE_COLOR_B     2
#define ICE_COLOR_C     3
#define ICE_COLOR_M     4
#define ICE_COLOR_Y     5
#define ICE_MAIN_BRI_CTRL(Enable)   (msWriteByteMask(SC14_02, (Enable)?(BIT5):(0), BIT5))
#define ICE_MAIN_SAT_CTRL(Enable)   (msWriteByteMask(SC14_02, (Enable)?(BIT4):(0), BIT4))
#define ICE_MAIN_HUE_CTRL(Enable)   (msWriteByteMask(SC14_02, (Enable)?(BIT0):(0), BIT0))
#define ICE_SUB_BRI_CTRL(Enable)   (msWriteByteMask(SC14_03, (Enable)?(BIT5):(0), BIT5))
#define ICE_SUB_SAT_CTRL(Enable)   (msWriteByteMask(SC14_03, (Enable)?(BIT4):(0), BIT4))
#define ICE_SUB_HUE_CTRL(Enable)   (msWriteByteMask(SC14_03, (Enable)?(BIT0):(0), BIT0))
#define ICE_MAIN_CTRL(Enable)   (msWriteByte(SC14_02, (Enable)?(BIT5|BIT4|BIT0):(0)))
#define ICE_SUB_CTRL(Enable)   (msWriteByte(SC14_03, (Enable)?(BIT5|BIT4|BIT0):(0)))
#define ICE_DEFINE_RANGE_RGB(Color, Value)  (msWriteByte(SC14_04+(Color), Value))
#define ICE_ACTIVE_RANGE_RGBCMY(Color, Value)   (msWriteByte(SC14_08+(Color), Value))
#define ICE_SATURATION_RGBCMY(Color, Value) (msWriteByte(SC14_14+(Color), Value))
#define ICE_HUE_RGBCMY(Color, Value)    (msWriteByte(SC14_0E+(Color), Value))
#define ICE_BRIGHTNESS_RGBCMY(Color, Value)    (msWriteByte(SC14_1A+(Color), Value))

#define CSC_MAIN_ENABLE(Enable) (msWriteByteMask(SC07_40, (Enable)?(BIT0):(0), BIT0))
#define CSC_SUB_ENABLE(Enable) (msWriteByteMask(SC07_40, (Enable)?(BIT4):(0), BIT4))

// OSD: Code/Attr RAM
#define OSD_TEXT_HI_ADDR_SET_BIT8()    ( g_u8OsdFontDataHighByte = BIT0)//enable bit 8
#define OSD_TEXT_HI_ADDR_SET_BITS(u8Addr)     (g_u8OsdFontDataHighByte = u8Addr)
#define OSD_TEXT_HI_ADDR_CLR_TO_0()     g_u8OsdFontDataHighByte = 0

#define PORT_CODE_ADDR  OSD1_66
#define PORT_CODE_DATA  OSD1_66
#define PORT_ATTR_ADDR  OSD1_66
#define PORT_ATTR_DATA  OSD1_66
#define PORT_FONT_ADDR  OSD1_66
#define PORT_FONT_DATA  OSD1_66

#define WRITE_CAFSRAM_ADDR()            msWriteByteMask(OSD1_68, 0, 0x1F)
#define WRITE_CODE()                    (msWriteByteMask(OSD1_68, 1, 0x1F),\
                                        msWriteByteMask(OSD1_67, g_u8OsdFontDataHighByte, 0x0F))
#define WRITE_ATTRIBUTE()               msWriteByteMask(OSD1_68, 2, 0x1F)
#define WRITE_FONT()                    msWriteByteMask(OSD1_68, 3, 0x1F)
#define CAFSRAM_ERASE_TRIG()            msWriteByteMask(OSD1_68, 4, 0x1F)
#define CAFSRAM_READ_TRIG()             msWriteByteMask(OSD1_68, 5, 0x1F)

#define WRITE_ATTRIBUTE()               msWriteByteMask(OSD1_68, 2, 0x1F)
#define WRITE_FONT()                    msWriteByteMask(OSD1_68, 3, 0x1F)
#define CAFSRAM_ERASE_TRIG()            msWriteByteMask(OSD1_68, 4, 0x1F)
#define CAFSRAM_READ_TRIG()             msWriteByteMask(OSD1_68, 5, 0x1F)

#define WRITE_PSRAM0_ADDR()             msWriteByteMask(OSD1_68, 10, 0x1F)
#define WRITE_PSRAM0_MASK()             msWriteByteMask(OSD1_68, 11, 0x1F)
#define WRITE_PSRAM0_COLOR_KEY()        msWriteByteMask(OSD1_68, 12, 0x1F)
#define WRITE_PSRAM0_COLOR()            msWriteByteMask(OSD1_68, 13, 0x1F)
#define PSRAM0_ERASE_TRIG()             msWriteByteMask(OSD1_68, 14, 0x1F)
#define PSRAM0_READ_TRIG()              msWriteByteMask(OSD1_68, 15, 0x1F)

#define WRITE_PSRAM1_ADDR()             msWriteByteMask(OSD1_68, 20, 0x1F)
#define WRITE_PSRAM1_COLOR_KEY()        msWriteByteMask(OSD1_68, 21, 0x1F)
#define WRITE_PSRAM1_COLOR()            msWriteByteMask(OSD1_68, 22, 0x1F)
#define PSRAM1_ERASE_TRIG()             msWriteByteMask(OSD1_68, 23, 0x1F)
#define PSRAM1_READ_TRIG()              msWriteByteMask(OSD1_68, 24, 0x1F)

#define OSD_WRITE_FONT_ADDRESS(u8Addr)  WRITE_CAFSRAM_ADDR();\
                                        msWrite2Byte(OSD1_66, GET_FONT_RAM_ADDR(u8Addr));

#define SET_OSD_CTRL_REG_WR_PORT(V)     {msWriteByte(OSD1_66, (V)); msWriteByteMask(OSD1_67,((V)>>8),0x7F);}

// ADC
#if _NEW_SOG_DET_
#define NEW_SOG_DET_FLUSH()             MEM_MSWRITE_BYTE_MASK(REG_ADC_PMATOP_7A_L,BIT3,BIT3);\
                                        MEM_MSWRITE_BYTE_MASK(REG_ADC_PMATOP_7A_L, 0, BIT3)
#endif

#ifdef _NEW_SOG_WAKEUP_DET_
#define NEW_SOG_WAKEUP_FLUSH()          (MEM_MSWRITE_BYTE_MASK(REG_0025F4, BIT3, BIT3), MEM_MSWRITE_BYTE_MASK(REG_0025F4, 0, BIT3))
#endif

#define KEYPAD_SAR00         msReadSARData(0)
#define KEYPAD_SAR01         msReadSARData(1)
#define KEYPAD_SAR02         msReadSARData(2)
#define KEYPAD_SAR03         msReadSARData(3)
#define KEYPAD_SAR04         msReadSARData(4)
#define KEYPAD_SAR05         msReadSARData(5)
#define KEYPAD_SAR06         msReadSARData(6)
#define KEYPAD_SAR07         msReadSARData(7)
#define KEYPAD_SAR08         msReadSARData(8)
#define KEYPAD_SAR09         msReadSARData(9)
#define KEYPAD_SAR10         msReadSARData(10)
#define KEYPAD_SAR11         msReadSARData(11)

#endif

