#ifndef __LITEMAX_COMMDEF_H__
#define __LITEMAX_COMMDEF_H__

////////////////////////////////////////////////////////////
// LiteMAX EDID LIST
////////////////////////////////////////////////////////////
#define E_EDID_TBL_995991142502             1 // 1920x536

////////////////////////////////////////////////////////////
// IF not DEFINE
////////////////////////////////////////////////////////////
#ifndef EDID_DP
#define EDID_DP                             0
#endif
#ifndef BRIGHTNESS_INVERSE
#define BRIGHTNESS_INVERSE                             1
#endif

////////////////////////////////////////////////////////////
// LITEMAX MAINBOARD TYPE DEFINE
////////////////////////////////////////////////////////////
#define BD_MT9700_LITEMAX  		0x0180   // T9 with 1A1D1H1DP
#define BD_MT9700_LITEMAX_1DP 	0x0181   // T9 with 1DP

////////////////////////////////////////////////////////////
// INPUT_TYPE SELECTION
////////////////////////////////////////////////////////////
#define DVI_USB                         0       // 1: DVI from USB
#define DVI_PN_SWAP                     0
#define HDMI_PN_SWAP                    1
#define ENABLE_AUTOEQ                   0
#if MainBoardType == BD_FPGA    //FPGA
#define INPUT_TYPE                      (INPUT_1A|INPUT_4C)
#define ENABLE_VGA_INPUT                (INPUT_TYPE&INPUT_1A)
#define ENABLE_HDMI                     1       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_HDMI_EDID_INTERNAL_DATA  0
#define ENABLE_VGA_EDID_INTERNAL_DATA   0
#define ENABLE_DVI                      0       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 1       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_DP_OUTPUT                0
#define ENABLE_DP_AUDIO                 0
#define AudioFunc                       1
#define _NEW_SOG_DET_                   1
#define _NEW_SOG_WAKEUP_DET_            (_NEW_SOG_DET_)
#define ENABLE_TYPEC_SAR_DET            0       // default: 0, 1: enable only related SAR pins are reserved
#define DRAM_TYPE                       DRAM_NOUSE
#elif MainBoardType == BD_MTV19067
#define INPUT_TYPE                      (INPUT_1A|INPUT_3C)
#define ENABLE_VGA_INPUT                (INPUT_TYPE&INPUT_1A)
#define ENABLE_HDMI                     1       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_HDMI_EDID_INTERNAL_DATA  0
#define ENABLE_VGA_EDID_INTERNAL_DATA   0
#define ENABLE_DVI                      0       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 1       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_DP_OUTPUT                0
#define ENABLE_DP_AUDIO                 0
#define AudioFunc                       1
#define _NEW_SOG_DET_                   1
#define _NEW_SOG_WAKEUP_DET_            (_NEW_SOG_DET_)
#define ENABLE_TYPEC_SAR_DET            0       // default: 0, 1: enable only related SAR pins are reserved
#define DRAM_TYPE                       DRAM_NOUSE
#elif MainBoardType == BD_MTV19069
#define INPUT_TYPE                      (INPUT_1A|INPUT_3C)
#define ENABLE_VGA_INPUT                (INPUT_TYPE&INPUT_1A)
#define ENABLE_HDMI                     1       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_HDMI_EDID_INTERNAL_DATA  0
#define ENABLE_VGA_EDID_INTERNAL_DATA   0
#define ENABLE_DVI                      0       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 1       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_DP_OUTPUT                0
#define ENABLE_DP_AUDIO                 0
#define AudioFunc                       1
#define _NEW_SOG_DET_                   1
#define _NEW_SOG_WAKEUP_DET_            (_NEW_SOG_DET_)
#define ENABLE_TYPEC_SAR_DET            1       // default: 0, 1: enable only related SAR pins are reserved
#define DRAM_TYPE                       DRAM_NOUSE
#elif MainBoardType == BD_MTV19072
#define INPUT_TYPE                      (INPUT_4C)
#define ENABLE_VGA_INPUT                0
#define ENABLE_HDMI                     1       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_HDMI_EDID_INTERNAL_DATA  0
#define ENABLE_VGA_EDID_INTERNAL_DATA   0
#define ENABLE_DVI                      0       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 1       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_DP_OUTPUT                0
#define ENABLE_DP_AUDIO                 0
#define AudioFunc                       1
#define _NEW_SOG_DET_                   0
#define _NEW_SOG_WAKEUP_DET_            (_NEW_SOG_DET_)
#define ENABLE_TYPEC_SAR_DET            1       // default: 0, 1: enable only related SAR pins are reserved
#define DRAM_TYPE                       DRAM_NOUSE
#elif MainBoardType == BD_MTV19071
#define INPUT_TYPE                      (INPUT_1A|INPUT_3C)
#define ENABLE_VGA_INPUT                (INPUT_TYPE&INPUT_1A)
#define ENABLE_HDMI                     1       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_HDMI_EDID_INTERNAL_DATA  0
#define ENABLE_VGA_EDID_INTERNAL_DATA   0
#define ENABLE_DVI                      0       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 1       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_DP_OUTPUT                0
#define ENABLE_DP_AUDIO                 0
#define AudioFunc                       1
#define _NEW_SOG_DET_                   1
#define _NEW_SOG_WAKEUP_DET_            (_NEW_SOG_DET_)
#define ENABLE_TYPEC_SAR_DET            1       // default: 0, 1: enable only related SAR pins are reserved
#define DRAM_TYPE                       DRAM_NOUSE
#elif ((MainBoardType == BD_MST162A_A02A_S) || (MainBoardType == BD_MT9700_A7V2_P2_S))
#define INPUT_TYPE                      (INPUT_1A|INPUT_3C)
#define ENABLE_VGA_INPUT                (INPUT_TYPE&INPUT_1A)
#define ENABLE_HDMI                     1       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_HDMI_EDID_INTERNAL_DATA  0
#define ENABLE_VGA_EDID_INTERNAL_DATA   0
#define ENABLE_DVI                      0       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 1       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_DP_OUTPUT                0
#define ENABLE_DP_AUDIO                 0
#define AudioFunc                       1
#define _NEW_SOG_DET_                   1
#define _NEW_SOG_WAKEUP_DET_            (_NEW_SOG_DET_)
#define ENABLE_TYPEC_SAR_DET            0       // default: 0, 1: enable only related SAR pins are reserved
#define DRAM_TYPE                       DRAM_NOUSE
#elif ((MainBoardType == BD_MST162F_A01A_S) || (MainBoardType == BD_MST218B_B02A_S))
#define INPUT_TYPE                      (INPUT_1A|INPUT_1C)
#define ENABLE_VGA_INPUT                (INPUT_TYPE&INPUT_1A)
#define ENABLE_HDMI                     1       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_HDMI_EDID_INTERNAL_DATA  0
#define ENABLE_VGA_EDID_INTERNAL_DATA   0
#define ENABLE_DVI                      0       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 0       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_DP_OUTPUT                0
#define ENABLE_DP_AUDIO                 0
#define AudioFunc                       0
#define _NEW_SOG_DET_                   1
#define _NEW_SOG_WAKEUP_DET_            (_NEW_SOG_DET_)
#define ENABLE_TYPEC_SAR_DET            0       // default: 0, 1: enable only related SAR pins are reserved
#define DRAM_TYPE                       DRAM_NOUSE
#elif MainBoardType == BD_MST218B_B01A_S
#define INPUT_TYPE                      (INPUT_1A|INPUT_1C)
#define ENABLE_VGA_INPUT                (INPUT_TYPE&INPUT_1A)
#define ENABLE_HDMI                     0       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_HDMI_EDID_INTERNAL_DATA  0
#define ENABLE_VGA_EDID_INTERNAL_DATA   0
#define ENABLE_DVI                      0       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 1       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_DP_OUTPUT                0
#define ENABLE_DP_AUDIO                 0
#define AudioFunc                       0
#define _NEW_SOG_DET_                   1
#define _NEW_SOG_WAKEUP_DET_            (_NEW_SOG_DET_)
#define ENABLE_TYPEC_SAR_DET            0       // default: 0, 1: enable only related SAR pins are reserved
#define DRAM_TYPE                       DRAM_NOUSE
#elif MainBoardType == BD_MT9700_A2V1_P2_S
#define INPUT_TYPE                      (INPUT_1A|INPUT_3C)
#define ENABLE_VGA_INPUT                (INPUT_TYPE&INPUT_1A)
#define ENABLE_HDMI                     1       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_HDMI_EDID_INTERNAL_DATA  0
#define ENABLE_VGA_EDID_INTERNAL_DATA   0
#define ENABLE_DVI                      0       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 1       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_DP_OUTPUT                0
#define ENABLE_DP_AUDIO                 0
#define AudioFunc                       1
#define _NEW_SOG_DET_                   1
#define _NEW_SOG_WAKEUP_DET_            (_NEW_SOG_DET_)
#define ENABLE_TYPEC_SAR_DET            0       // default: 0, 1: enable only related SAR pins are reserved
#define DRAM_TYPE                       DRAM_NOUSE
#elif MainBoardType == BD_MT9700_A3V2_P2_S
#define INPUT_TYPE                      (INPUT_1A|INPUT_3C)
#define ENABLE_VGA_INPUT                (INPUT_TYPE&INPUT_1A)
#define ENABLE_HDMI                     1       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_HDMI_EDID_INTERNAL_DATA  0
#define ENABLE_VGA_EDID_INTERNAL_DATA   0
#define ENABLE_DVI                      0       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 1       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_DP_OUTPUT                0
#define ENABLE_DP_AUDIO                 0
#define AudioFunc                       1
#define _NEW_SOG_DET_                   1
#define _NEW_SOG_WAKEUP_DET_            (_NEW_SOG_DET_)
#define ENABLE_TYPEC_SAR_DET            1       // default: 0, 1: enable only related SAR pins are reserved
#define DRAM_TYPE                       DRAM_NOUSE
#elif MainBoardType == BD_MT9700_A4V2_P2_S
#define INPUT_TYPE                      (INPUT_1A|INPUT_3C)
#define ENABLE_VGA_INPUT                (INPUT_TYPE&INPUT_1A)
#define ENABLE_HDMI                     1       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_HDMI_EDID_INTERNAL_DATA  0
#define ENABLE_VGA_EDID_INTERNAL_DATA   0
#define ENABLE_DVI                      0       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 1       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_DP_OUTPUT                0
#define ENABLE_DP_AUDIO                 0
#define AudioFunc                       1
#define _NEW_SOG_DET_                   1
#define _NEW_SOG_WAKEUP_DET_            (_NEW_SOG_DET_)
#define ENABLE_TYPEC_SAR_DET            1       // default: 0, 1: enable only related SAR pins are reserved
#define DRAM_TYPE                       DRAM_NOUSE
#elif MainBoardType == BD_MT9700_A5V2_P2_S
#define INPUT_TYPE                      (INPUT_1A|INPUT_3C)
#define ENABLE_VGA_INPUT                (INPUT_TYPE&INPUT_1A)
#define ENABLE_HDMI                     1       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_HDMI_EDID_INTERNAL_DATA  0
#define ENABLE_VGA_EDID_INTERNAL_DATA   0
#define ENABLE_DVI                      0       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 1       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_DP_OUTPUT                0
#define ENABLE_DP_AUDIO                 0
#define AudioFunc                       1
#define _NEW_SOG_DET_                   1
#define _NEW_SOG_WAKEUP_DET_            (_NEW_SOG_DET_)
#define ENABLE_TYPEC_SAR_DET            0       // default: 0, 1: enable only related SAR pins are reserved
#define DRAM_TYPE                       DRAM_NOUSE
#elif MainBoardType == BD_MT9700_A6V2_P2_S
#define INPUT_TYPE                      (INPUT_4C)
#define ENABLE_VGA_INPUT                0
#define ENABLE_HDMI                     1       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_HDMI_EDID_INTERNAL_DATA  0
#define ENABLE_VGA_EDID_INTERNAL_DATA   0
#define ENABLE_DVI                      0       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 1       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_DP_OUTPUT                0
#define ENABLE_DP_AUDIO                 0
#define AudioFunc                       1
#define _NEW_SOG_DET_                   0
#define _NEW_SOG_WAKEUP_DET_            (_NEW_SOG_DET_)
#define ENABLE_TYPEC_SAR_DET            1       // default: 0, 1: enable only related SAR pins are reserved
#define DRAM_TYPE                       DRAM_NOUSE
#elif (MainBoardType == BD_MT9700_LITEMAX)
#define INPUT_TYPE                      (INPUT_1A|INPUT_2C)
#define ENABLE_VGA_INPUT                (INPUT_TYPE&INPUT_1A)
#define ENABLE_HDMI                     1       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_HDMI_EDID_INTERNAL_DATA  0
#define ENABLE_VGA_EDID_INTERNAL_DATA   0
#define ENABLE_DVI                      0       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 1       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_DP_OUTPUT                0
#define ENABLE_DP_AUDIO                 1
#define AudioFunc                       1
#define _NEW_SOG_DET_                   1
#define _NEW_SOG_WAKEUP_DET_            (_NEW_SOG_DET_)
#define ENABLE_TYPEC_SAR_DET            0       // default: 0, 1: enable only related SAR pins are reserved
#define DRAM_TYPE                       DRAM_NOUSE
#define ENABLE_DVI_HDMI_SWITCH          0
#elif (MainBoardType == BD_MT9700_LITEMAX_1DP)
#define INPUT_TYPE                      (INPUT_1C)
#define ENABLE_VGA_INPUT                0//(INPUT_TYPE&INPUT_1A)
#define ENABLE_HDMI                     0 // 1       //((Input_HDMI_C1 != Input_Nothing)||(Input_HDMI_C2 != Input_Nothing)||(Input_HDMI_C3 != Input_Nothing))
#define ENABLE_HDMI_1_4                 0
#define ENABLE_HDMI_EDID_INTERNAL_DATA  0
#define ENABLE_VGA_EDID_INTERNAL_DATA   0
#define ENABLE_DVI                      0       //((Input_DVI_C1 != Input_Nothing)||(Input_DVI_C2 != Input_Nothing)||(Input_DVI_C3 != Input_Nothing))
#define ENABLE_DP_INPUT                 1       //((Input_Displayport_C2 != Input_Nothing)||(Input_Displayport_C3 != Input_Nothing))
#define ENABLE_DP_OUTPUT                0
#define ENABLE_DP_AUDIO                 1
#define AudioFunc                       1
#define _NEW_SOG_DET_                   0 // 1
#define _NEW_SOG_WAKEUP_DET_            (_NEW_SOG_DET_)
#define ENABLE_TYPEC_SAR_DET            0       // default: 0, 1: enable only related SAR pins are reserved
#define DRAM_TYPE                       DRAM_NOUSE
#define ENABLE_DVI_HDMI_SWITCH          0
#else
#message "please define new board type"
#endif

#define MS_VGA_SOG_EN                   1

#endif
