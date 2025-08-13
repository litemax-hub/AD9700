#define _USBSW_C_
#include "board.h"
#include "types.h"
#include "misc.h"
#include "Common.h"
#include "Ms_rwreg.h"
#include "ms_reg_TSUMR2.h"
#include "usbsw.h"
#include "mailbox.h"
#include "msEread.h"
#include "Mode.h"

#if ENABLE_USB_DEVICE
#include "drvUSBDwld.h"
#endif

#define USBSW_DEBUG    1
#if ENABLE_DEBUG&&USBSW_DEBUG
    #define USBSW_printData(str, value)   printData(str, value)
    #define USBSW_printMsg(str)           printMsg(str)
#else
    #define USBSW_printData(str, value)
    #define USBSW_printMsg(str)
#endif

#if ENABLE_USB_TYPEC
void msDrvUsbSwitchInit(void)
{
#if (CHIP_ID ==  CHIP_MT9700 ) // (CHIP_ID ==  CHIP_MT9700 ) {
#if U3_REDRIVER_LEGACY
	//UPLL &UTMI initial setting
	msWriteByte(REG_151800, 0xC0);
	msWriteByte(REG_15180E, 0x13);
	msWriteByte(REG_151804, 0x10);
	msWriteByte(REG_151805, 0x01); // For 12Mhz in UPLL

	msWriteByte(REG_150308, 0x0F);
	msWriteByte(REG_150309, 0x04);
	msWriteByte(REG_150300, 0x01);
	msWriteByte(REG_150301, 0x00);

	//GEN1 AUTOEQ setting
	msWriteByte(REG_152ACC, 0x04); // reg_autoeq_wait_cycle[7:0]
	msWriteByte(REG_152ACD, 0x00); // reg_autoeq_wait_cycle[9:8]
	msWriteByte(REG_152ABE, 0x00); // reg_autoeq_acc_timeout[7:0]
	msWriteByte(REG_152ABF, 0x01); // reg_autoeq_acc_timeout[15:8]
	msWriteByte(REG_152AC0, 0xE0); // reg_autoeq_acc_th
	msWriteByte(REG_152DCC, 0x04); // reg_autoeq_wait_cycle[7:0]
	msWriteByte(REG_152DCD, 0x00); // reg_autoeq_wait_cycle[9:8]
	msWriteByte(REG_152DBE, 0x00); // reg_autoeq_acc_timeout[7:0]
	msWriteByte(REG_152DBF, 0x01); // reg_autoeq_acc_timeout[15:8]
	msWriteByte(REG_152DC0, 0xE0); // reg_autoeq_acc_th

	//TX SSC setting
	msWriteByte(REG_152832, 0x74); // SSC set
	msWriteByte(REG_152833, 0x93);
	msWriteByte(REG_152834, 0x18);
	msWriteByte(REG_152838, 0xD8); // SSC span
	msWriteByte(REG_152839, 0x04);
	msWriteByte(REG_152836, 0x02); // SSC step
	msWriteByte(REG_152837, 0x70); // SSC step_frac
	msWriteByte(REG_15283A, 0x04); // SSC ssc_dn

	msWriteByte(REG_152B32, 0x74); // SSC set
	msWriteByte(REG_152B33, 0x93);
	msWriteByte(REG_152B34, 0x18);
	msWriteByte(REG_152B38, 0xD8); // SSC span
	msWriteByte(REG_152B39, 0x04);
	msWriteByte(REG_152B36, 0x02); // SSC step
	msWriteByte(REG_152B37, 0x70); // SSC step_frac
	msWriteByte(REG_152B3A, 0x04); // SSC ssc_dn

	//RX_SQH setting
	//Enable C/HUB_RX_SQH
	msWriteByteMask(REG_15287F, BIT5, BIT5); // reg_c_data_sqh_en=1
	msWriteByteMask(REG_152B7F, BIT5, BIT5); // reg_c_data_sqh_en=1
	//Set RX_SQH Deglitch & Vth
	msWriteByte(REG_152678, 0x89);			 // Reg_rx_sqh_deglitch[7:0] 
	msWriteByteMask(REG_152679, 0x04, 0x07); // Reg_rx_sqh_vth[2:0]
	msWriteByte(REG_152778, 0x89); 			 // Reg_rx_sqh_deglitch[7:0] 
	msWriteByteMask(REG_152779, 0x04, 0x07); // Reg_rx_sqh_vth[2:0]
	//Disable DTOP_CTRL SQH de-bounce
	msWriteByteMask(REG_152870, 0x00, 0x0F); // reg_data_sq_deb_set_counter
	msWriteByteMask(REG_152B70, 0x00, 0x0F); // reg_data_sq_deb_set_counter

	//U30C_CTRL Setting
	//AUTO_EQ timeout
	msWriteByte(REG_15286E, 0x90); // reg_train_ss_period_opt
	msWriteByte(REG_152B6E, 0x90); // reg_train_ss_period_opt
	//LFPS to TRAIN timeout
	msWriteByteMask(REG_15281B, 0x04, 0x0C); // reg_lfps_train_period_opt[1:0]
	msWriteByteMask(REG_152B1B, 0x04, 0x0C); // reg_lfps_train_period_opt[1:0]
	//dummy data pattern setting from LS to HS
    msWriteByte(REG_152926, 0xAA); // reg_rxtx_fifo_empty_data[15:0]
	msWriteByte(REG_152927, 0xAA);
	msWriteByte(REG_152C26, 0xAA); // reg_rxtx_fifo_empty_data[15:0]
	msWriteByte(REG_152C27, 0xAA);

	//Don!|t do RxDet in U23 of C side
	msWriteByteMask(REG_152B72, BIT5, BIT5); // Don!|t do rxdet in U23
	//Enable FSM jump U23 when u1 or u23 wakeup fail
	msWriteByteMask(REG_152872, 0x03, 0x03); // reg_u1_exit_to_go_u23, reg_u23_exit_to_go_u23
	msWriteByteMask(REG_152B72, 0x03, 0x03); // reg_u1_exit_to_go_u23, reg_u23_exit_to_go_u23
	//Set u1_enter_u23_period to 480ms (for u3_flag mode)
	msWriteByteMask(REG_15281A, 0x0C, 0x0C); // reg_u1_enter_u23_period_opt
	msWriteByteMask(REG_152B1A, 0x0C, 0x0C); // reg_u1_enter_u23_period_opt
	//8B/10B de-bounce enable for gen1 (for u3_flag mode)
	msWriteByteMask(REG_152814, BIT0, BIT0); // 8B/10B de-bounce enable
	msWriteByteMask(REG_152B14, BIT0, BIT0); // 8B/10B de-bounce enable
	//Warm_Reset detection setting
	msWriteByteMask(REG_152895, 0x0F, 0x0F); // Warm_Reset detection setting
	msWriteByteMask(REG_152B95, 0x0F, 0x0F); // Warm_Reset detection setting

	//Fix Polling_LFPS/SCD1 stick issue
	msWriteByteMask(REG_152AFE, 0x3F, 0x3F);
    msWriteByteMask(REG_152DFE, 0x3F, 0x3F);

    //Change eidle_period_opt from 32us to 8us
    msWriteByte(REG_152871, 0x3F); // eidle period opt
    msWriteByte(REG_152B71, 0x3F); // eidle period opt

    //u1_wakeup check timing lfps 32ns
    msWriteByteMask(REG_1529E0, 0x0C, 0x0C); // 32ns
    msWriteByteMask(REG_152CE0, 0x0C, 0x0C); // 32ns
    
    // Use u0_disable_timer0
    msWriteByteMask(REG_1529E0, 0x60, 0x60);
    msWriteByteMask(REG_152CE0, 0x60, 0x60);

    // Disable watch mutual lfps for Ux wakeup
    msWriteByteMask(REG_152818, 0x00, BIT1); // reg_watch_mutual_lfps_wakeup
    msWriteByteMask(REG_152872, BIT1, BIT1); // reg_u23_exit_to_go_u23
    msWriteByteMask(REG_152B18, 0x00, BIT1); // reg_watch_mutual_lfps_wakeup
    msWriteByteMask(REG_152B72, BIT1, BIT1); // reg_u23_exit_to_go_u23
    
    //TX_LOCK as RXPLL to 14!|d4990
    msWriteByte(REG_15285E, 0x7E); // TXPLL clock lock tolerance
    msWriteByte(REG_15285F, 0x13);
    msWriteByte(REG_152B5E, 0x7E); // TXPLL clock lock tolerance
    msWriteByte(REG_152B5F, 0x13);
    msWriteByteMask(REG_1528F0, BIT4, BIT4); // 12MHZ for Ux wakeup

    //Enable Compliance
    msWriteByteMask(REG_15286F, 0x00, BIT7); // reg_disable_compliance
    msWriteByteMask(REG_152B6F, 0x00, BIT7); // reg_disable_compliance

#if U3_REDRV_PM_MODE
    //Disable U3RD_enhance mode &  Enable U3_flag under u3redrv mode
    msWriteByteMask(REG_1529E8, 0x10, 0x1F); // [4]: reg_ltssm_u3_flag
    msWriteByteMask(REG_152CE8, 0x10, 0x1F);    
    
    // Enable u3rt 16-32 fifo
    msWriteByteMask(REG_152070, BIT0, BIT0); // reg_pipe_fifo_en_rx
    msWriteByteMask(REG_152370, BIT0, BIT0);

    // NODIE_PD_LFRX & NODIE_PD_LFRX_VALID
    msWriteByte(REG_1526D9, 0xFF);
    msWriteByte(REG_1526CD, 0x44);
    msWriteByte(REG_1527D9, 0xFF);
    msWriteByte(REG_1527CD, 0x44);
    
    // PD_BG_IGEN
    msWriteByteMask(REG_1526DC, 0xF0, 0xF0); // Reg_lin_ov_val_pd_bg
    msWriteByteMask(REG_1526D0, 0x40, 0xF0); // Reg_lin_ov_en_pd_bg
    msWriteByteMask(REG_1527DC, 0xF0, 0xF0); // Reg_lin_ov_val_pd_bg
    msWriteByteMask(REG_1527D0, 0x40, 0xF0); // Reg_lin_ov_en_pd_bg

    // PD_RX_LANE & PD_RX_LDO
    msWriteByteMask(REG_1526DF, 0x0F, 0x0F); // Reg_lin_ov_val_pd_rx_lane
    msWriteByteMask(REG_1526E1, 0xF0, 0xF0); // Reg_lin_ov_val_pd_rx_ldo
    msWriteByte(REG_1526D3, 0x44);
    msWriteByteMask(REG_1527DF, 0x0F, 0x0F); // Reg_lin_ov_val_pd_rx_lane
    msWriteByteMask(REG_1527E1, 0xF0, 0xF0); // Reg_lin_ov_val_pd_rx_ldo
    msWriteByte(REG_1527D3, 0x44);

    // PD_RXPLL
    msWriteByteMask(REG_1526E1, 0x0F, 0x0F); // Reg_link_ov_val_pd_rxpll
    msWriteByteMask(REG_1526D5, 0x04, 0x0F); // Reg_link_ov_en_pd_rxpll
    msWriteByteMask(REG_1527E1, 0x0F, 0x0F); // Reg_link_ov_val_pd_rxpll
    msWriteByteMask(REG_1527D5, 0x04, 0x0F); // Reg_link_ov_en_pd_rxpll

    // PD_RXPLL_UPDN_DMX
    msWriteByteMask(REG_1526E2, 0x0F, 0x0F); // Reg_lin_ov_val_pd_rxpll_updn_dmx
    msWriteByteMask(REG_1526D6, 0x04, 0x0F); // Reg_lin_ov_en_pd_rxpll_updn_dmx
    msWriteByteMask(REG_1527E2, 0x0F, 0x0F); // Reg_lin_ov_val_pd_rxpll_updn_dmx
    msWriteByteMask(REG_1527D6, 0x04, 0x0F); // Reg_lin_ov_en_pd_rxpll_updn_dmx

    // PD_TXPLL & TX_TAP
    msWriteByteMask(REG_1526E3, 0x0F, 0x0F); // Reg_lin_ov_val_pd_txpll
    msWrite2Byte(REG_1526E4, 0xFFFF); // Reg_link_ov_val_pd_tx_tap_0
    msWriteByte(REG_1526D7, 0x44); // Reg_link_ov_en_pd_txpll, Reg_link_ov_en_pd_tx_tap_0
    msWriteByteMask(REG_1527E3, 0x0F, 0x0F); // Reg_lin_ov_val_pd_txpll
    msWrite2Byte(REG_1527E4, 0xFFFF); // Reg_link_ov_val_pd_tx_tap_0
    msWriteByte(REG_1527D7, 0x44); // Reg_link_ov_en_pd_txpll, Reg_link_ov_en_pd_tx_tap_0

    // NODIE_PD_TX_CMFB & NODIE_PD_TX_LANE_SSTX
    msWrite2Byte(REG_1526DA, 0xFFFF); // Reg_link_ov_val_nodie_pd_tx_cmfb_sstx0/1
    msWrite2Byte(REG_1527DA, 0xFFFF); // Reg_link_ov_val_nodie_pd_tx_cmfb_sstx0/1

    msWrite2Byte(REG_1527CE, 0x0404); // Reg_link_ov_en_nodie_pd_tx_cmfb_sstx0, Reg_link_ov_en_nodie_pd_tx_lane_sstx0
#else
    //Disable U3RD_enhance mode
    msWriteByteMask(REG_1529E8, 0x00, 0x1F);
    msWriteByteMask(REG_152CE8, 0x00, 0x1F);    
#endif

    // HUB swing and pre-emphasis
    msWriteByte(REG_152760, 0x54);
    msWriteByte(REG_1527FE, 0x00);
    
#else // enhance mode
	// UPLL &UTMI initial setting
	msWriteByte(REG_151800, 0xC0);
	msWriteByte(REG_15180E, 0x13);
	msWriteByte(REG_151804, 0x10);
	msWriteByte(REG_151805, 0x01); // For 12Mhz in UPLL

	msWriteByte(REG_150308, 0x0F);
	msWriteByte(REG_150309, 0x04);
	msWriteByte(REG_150300, 0x01);
	msWriteByte(REG_150301, 0x00);

	//GEN1 AUTOEQ setting
	msWriteByte(REG_152ACC, 0x04); // reg_autoeq_wait_cycle[7:0]
	msWriteByte(REG_152ACD, 0x00); // reg_autoeq_wait_cycle[9:8]
	msWriteByte(REG_152ABE, 0x00); // reg_autoeq_acc_timeout[7:0]
	msWriteByte(REG_152ABF, 0x01); // reg_autoeq_acc_timeout[15:8]
	msWriteByte(REG_152AC0, 0xE0); // reg_autoeq_acc_th
	msWriteByte(REG_152DCC, 0x04); // reg_autoeq_wait_cycle[7:0]
	msWriteByte(REG_152DCD, 0x00); // reg_autoeq_wait_cycle[9:8]
	msWriteByte(REG_152DBE, 0x00); // reg_autoeq_acc_timeout[7:0]
	msWriteByte(REG_152DBF, 0x01); // reg_autoeq_acc_timeout[15:8]
	msWriteByte(REG_152DC0, 0xE0); // reg_autoeq_acc_th

    #if 0
	// TX SSC setting
	msWriteByte(REG_152832, 0x74); // SSC set
	msWriteByte(REG_152833, 0x93);
	msWriteByte(REG_152834, 0x18);
	msWriteByte(REG_152838, 0xD8); // SSC span
	msWriteByte(REG_152839, 0x04);
	msWriteByte(REG_152836, 0x02); // SSC step
	msWriteByte(REG_152837, 0x70); // SSC step_frac
	msWriteByte(REG_15283A, 0x04); // SSC ssc_dn

	msWriteByte(REG_152B32, 0x74); // SSC set
	msWriteByte(REG_152B33, 0x93);
	msWriteByte(REG_152B34, 0x18);
	msWriteByte(REG_152B38, 0xD8); // SSC span
	msWriteByte(REG_152B39, 0x04);
	msWriteByte(REG_152B36, 0x02); // SSC step
	msWriteByte(REG_152B37, 0x70); // SSC step_frac
	msWriteByte(REG_152B3A, 0x04); // SSC ssc_dn
    #endif

	//RX_SQH setting
	//Enable C/HUB_RX_SQH
	msWriteByteMask(REG_15287F, BIT5, BIT5); // reg_c_data_sqh_en=1
	msWriteByteMask(REG_152B7F, BIT5, BIT5); // reg_c_data_sqh_en=1
	//Set RX_SQH Deglitch & Vth
	msWriteByte(REG_152678, 0xB9);			 // Reg_rx_sqh_deglitch[7:0]
	msWriteByteMask(REG_152679, 0x02, 0x07); // Reg_rx_sqh_vth[2:0]
	msWriteByte(REG_152778, 0xB9); 			 // Reg_rx_sqh_deglitch[7:0]
	msWriteByteMask(REG_152779, 0x02, 0x07); // Reg_rx_sqh_vth[2:0]
	//Disable DTOP_CTRL SQH de-bounce
	msWriteByteMask(REG_152870, 0x00, 0x0F); // reg_data_sq_deb_set_counter
	msWriteByteMask(REG_152B70, 0x00, 0x0F); // reg_data_sq_deb_set_counter

	//U30C_CTRL Setting
	//AUTO_EQ timeout
	msWriteByte(REG_15286E, 0x90); // reg_train_ss_period_opt
	msWriteByte(REG_152B6E, 0x90); // reg_train_ss_period_opt
	//LFPS to TRAIN timeout
	msWriteByteMask(REG_15281B, 0x04, 0x0C); // reg_lfps_train_period_opt[1:0]
	msWriteByteMask(REG_152B1B, 0x04, 0x0C); // reg_lfps_train_period_opt[1:0]
	//dummy data pattern setting from LS to HS
	msWriteByte(REG_15280C, 0xAA); // reg_rxtx_fifo_empty_data[15:0]
	msWriteByte(REG_15280D, 0xAA);
	msWriteByte(REG_152B0C, 0xAA); // reg_rxtx_fifo_empty_data[15:0]
	msWriteByte(REG_152B0D, 0xAA);
    // RXDET threshold setting
    msWriteByte(REG_152802, 0x0A); // reg_rxdeet_connect_th[7:0]
    msWriteByte(REG_152B02, 0x0A);
    
	//Don't do RxDet in U23 of C side
	msWriteByteMask(REG_152B72, BIT5, BIT5); // Don!|t do rxdet in U23
	//Enable FSM jump U23 when u1 or u23 wakeup fail
	msWriteByteMask(REG_152872, 0x03, 0x03); // reg_u1_exit_to_go_u23, reg_u23_exit_to_go_u23
	msWriteByteMask(REG_152B72, 0x03, 0x03); // reg_u1_exit_to_go_u23, reg_u23_exit_to_go_u23
	//Set u1_enter_u23_period to 480ms (for u3_flag mode)
	msWriteByteMask(REG_15281A, 0x0C, 0x0C); // reg_u1_enter_u23_period_opt
	msWriteByteMask(REG_152B1A, 0x0C, 0x0C); // reg_u1_enter_u23_period_opt
	//8B/10B de-bounce enable for gen1 (for u3_flag mode)
	msWriteByteMask(REG_152814, BIT0, BIT0); // 8B/10B de-bounce enable
	msWriteByteMask(REG_152B14, BIT0, BIT0); // 8B/10B de-bounce enable
	//Warm_Reset detection setting
	msWriteByteMask(REG_152895, 0x0F, 0x0F); // Warm_Reset detection setting
	msWriteByteMask(REG_152B95, 0x0F, 0x0F); // Warm_Reset detection setting
	//TxGen issue dummy for tx clock unstable
	msWriteByteMask(REG_1528FA, BIT0, BIT0); // reg_dummy_7d[0]
	msWriteByteMask(REG_152BFA, BIT0, BIT0); // reg_dummy_7d[0]
	//Fix Polling_LFPS/SCD1 stick issue
	msWriteByteMask(REG_152AFE, 0x3F, 0x3F);
    msWriteByteMask(REG_152DFE, 0x3F, 0x3F);

    //Enable issue_u1_lfps
    msWriteByteMask(REG_1528E4, BIT6, BIT6); // Issue u1_lfps by u3rt_lfps_tx
    msWriteByteMask(REG_152BE4, BIT6, BIT6); // Issue u1_lfps by u3rt_lfps_tx

    //Change eidle_period_opt from 32us to 8us
    msWriteByte(REG_152871, 0x3F); // eidle period opt
    msWriteByte(REG_152B71, 0x3F); // eidle period opt

    //Extend TX FIFO depth
    msWriteByteMask(REG_15292C, 0x00, 0x07); // tx fifo to 32 depth
    msWriteByteMask(REG_152920, 0x10, 0x3F); // tx_fifo read point
    msWriteByteMask(REG_152C2C, 0x00, 0x07); // tx fifo to 32 depth
    msWriteByteMask(REG_152C20, 0x10, 0x3F); // tx_fifo read point

    // TXGEN OS SWITCH option
    msWriteByteMask(REG_1528FA, BIT1, BIT1); // reg_ksym_rpos_opt
    msWriteByteMask(REG_152BFA, BIT1, BIT1); // reg_ksym_rpos_opt
    msWriteByteMask(REG_1529B0, BIT0, BIT0); // reg_ov_en_skp_insert_opt
    msWriteByteMask(REG_152CB0, BIT0, BIT0); // reg_ov_en_skp_insert_opt    
    //U1 exit set to 300ns
    msWriteByte(REG_152032, 0x26); //reg_u1_bmin[7:0]
    msWriteByte(REG_152332, 0x26); //reg_u1_bmin[7:0]
    //Enable u3rt 16-32 fifo
    msWriteByteMask(REG_152070, BIT0, BIT0); //reg_pipe_fifo_en_rx
    msWriteByteMask(REG_152370, BIT0, BIT0); //reg_pipe_fifo_en_rx
    // Relax U1Exit handshake timing
    msWriteByte(REG_1520B6, 0x87); // reg_disable_check_U1exit_upper_bound
    msWriteByte(REG_1520B8, 0x64); // reg_disable_check_U1exit_upper_bound
    msWriteByte(REG_1520B9, 0x00); // reg_u1_exit_hsk_upper_bound[11:0]=800us
    msWriteByte(REG_1520BC, 0x0F); // reg_u1_exit_hsk_low_bound[11:0] = 120us
    msWriteByte(REG_1520BD, 0x00);

    //U3RD_enhance mode setting
    msWriteByteMask(REG_1529E8, 0x0E, 0x1F);
    msWriteByteMask(REG_152CE8, 0x0E, 0x1F);

    // HUB swing and pre-emphasis
    msWriteByte(REG_152760, 0x54);
    msWriteByte(REG_1527FE, 0x00);

    /****** SW option ******/
    // Disable look sqh enter U1 state
    msWriteByteMask(REG_15286F, 0x00, 0x60);
    msWriteByteMask(REG_152B6F, 0x00, 0x60);
    // Enable Active U1 exit
    msWriteByteMask(REG_1520E4, BIT6, BIT6);
    msWriteByteMask(REG_1523E4, BIT6, BIT6);
    // 20to20 AFIFO setting
    msWriteByteMask(REG_15292C, 0x00, 0x07); // reg_leo_20to20_afifo_half_fifo[2:0]
    msWriteByteMask(REG_152C2C, 0x00, 0x07); // reg_leo_20to20_afifo_half_fifo[2:0]
    // Extend reg_u1_exit_valid_period_opt for 100us long idle between LFPS and HS
    msWriteByte(REG_15286D, 0x32); //reg_u1_exit_valid_period_opt=8!|h32
    msWriteByte(REG_152B6D, 0x32); //reg_u1_exit_valid_period_opt=8!|h32
    
    // Reset u2 inactive timer when link disconnects
    msWriteByteMask(REG_1520BA, BIT1, BIT1);
    msWriteByteMask(REG_1523BA, BIT1, BIT1);
    // TX_synth_clk setting
    msWriteByte(REG_152832, 0x75); // SSC set
    msWriteByte(REG_152833, 0x93);
    msWriteByte(REG_152834, 0x18);
    msWriteByte(REG_152838, 0xD8); // SSC span
    msWriteByte(REG_152839, 0x04);    
    msWriteByte(REG_152836, 0x03); // SSC step
    msWriteByte(REG_152837, 0x30); // SSC step frac
    msWriteByteMask(REG_15283A, 0x00, BIT2); // SSC ssc_dn=0
    msWriteByte(REG_152B32, 0x75); // SSC set
    msWriteByte(REG_152B33, 0x93);
    msWriteByte(REG_152B34, 0x18);
    msWriteByte(REG_152B38, 0xD8); // SSC span
    msWriteByte(REG_152B39, 0x04);    
    msWriteByte(REG_152B36, 0x03); // SSC step
    msWriteByte(REG_152B37, 0x30); // SSC step frac
    msWriteByteMask(REG_152B3A, 0x00, BIT2); // SSC ssc_dn=0

    msWriteByte(REG_153010, 0xFE);
    msWriteByte(REG_153011, 0xFF);
    msWriteByte(REG_153012, 0xFF);
    msWriteByte(REG_153013, 0xFF);
    msWriteByte(REG_153014, 0xF0);
    msWriteByte(REG_153015, 0xCF);
    msWriteByte(REG_153018, 0xFF);
    msWriteByte(REG_153019, 0xF3);
    msWriteByte(REG_15301A, 0xD8);
    msWriteByte(REG_15301B, 0x7F);
    msWriteByte(REG_15301E, 0xBF);
    msWriteByteMask(REG_15301F, BIT7, BIT7); 
    msWriteByteMask(REG_1526F8, 0x71, 0x71);

#if  U3_REDRV_PM_MODE    
    // Enable u3rt 16-32 fifo
    //msWriteByteMask(REG_152070, BIT0, BIT0); // reg_pipe_fifo_en_rx
    //msWriteByteMask(REG_152370, BIT0, BIT0);

    // HW set NODIE_PD_LFRX=1 & NODIE_PD_LFRX_VALID=1 when link enter U3    
    msWriteByte(REG_1526D9, 0xFF);
    msWriteByte(REG_1526CD, 0x44);
    msWriteByte(REG_1527D9, 0xFF);
    msWriteByte(REG_1527CD, 0x44);
   
    // HW set PD_BG_IGEN=1 when link enter U3    
    msWriteByteMask(REG_1526DC, 0xF0, 0xF0); // Reg_lin_ov_val_pd_bg
    msWriteByteMask(REG_1526D0, 0x40, 0xF0); // Reg_lin_ov_en_pd_bg
    msWriteByteMask(REG_1527DC, 0xF0, 0xF0); // Reg_lin_ov_val_pd_bg
    msWriteByteMask(REG_1527D0, 0x40, 0xF0); // Reg_lin_ov_en_pd_bg

    // HW set PD_RX_LANE=1 & PD_RX_LDO=1 when link enter U3    
    msWriteByteMask(REG_1526DF, 0x0F, 0x0F); // Reg_lin_ov_val_pd_rx_lane
    msWriteByteMask(REG_1526E1, 0xF0, 0xF0); // Reg_lin_ov_val_pd_rx_ldo
    msWriteByte(REG_1526D3, 0x44);
    msWriteByteMask(REG_1527DF, 0x0F, 0x0F); // Reg_lin_ov_val_pd_rx_lane
    msWriteByteMask(REG_1527E1, 0xF0, 0xF0); // Reg_lin_ov_val_pd_rx_ldo
    msWriteByte(REG_1527D3, 0x44);

    // HW set PD_RXPLL=1 when link enter U3    
    msWriteByteMask(REG_1526E1, 0x0F, 0x0F); // Reg_link_ov_val_pd_rxpll
    msWriteByteMask(REG_1526D5, 0x04, 0x0F); // Reg_link_ov_en_pd_rxpll
    msWriteByteMask(REG_1527E1, 0x0F, 0x0F); // Reg_link_ov_val_pd_rxpll
    msWriteByteMask(REG_1527D5, 0x04, 0x0F); // Reg_link_ov_en_pd_rxpll

    // HW set PD_RXPLL_UPDN_DMX=1 when link enter U3    
    msWriteByteMask(REG_1526E2, 0x0F, 0x0F); // Reg_lin_ov_val_pd_rxpll_updn_dmx
    msWriteByteMask(REG_1526D6, 0x04, 0x0F); // Reg_lin_ov_en_pd_rxpll_updn_dmx
    msWriteByteMask(REG_1527E2, 0x0F, 0x0F); // Reg_lin_ov_val_pd_rxpll_updn_dmx
    msWriteByteMask(REG_1527D6, 0x04, 0x0F); // Reg_lin_ov_en_pd_rxpll_updn_dmx

    // HW set PD_TXPLL=1 & TX_TAP=16!|hff_ff when link enter U3
    msWriteByteMask(REG_1526E3, 0x0F, 0x0F); // Reg_lin_ov_val_pd_txpll
    msWrite2Byte(REG_1526E4, 0xFFFF); // Reg_link_ov_val_pd_tx_tap_0
    msWriteByte(REG_1526D7, 0x44); // Reg_link_ov_en_pd_txpll, Reg_link_ov_en_pd_tx_tap_0
    msWriteByteMask(REG_1527E3, 0x0F, 0x0F); // Reg_lin_ov_val_pd_txpll
    msWrite2Byte(REG_1527E4, 0xFFFF); // Reg_link_ov_val_pd_tx_tap_0
    msWriteByte(REG_1527D7, 0x44); // Reg_link_ov_en_pd_txpll, Reg_link_ov_en_pd_tx_tap_0

    // HW set NODIE_PD_TX_CMFB=1 & NODIE_PD_TX_LANE_SSTX=1    
    msWrite2Byte(REG_1526DA, 0xFFFF); // Reg_link_ov_val_nodie_pd_tx_cmfb_sstx0/1
    msWrite2Byte(REG_1527DA, 0xFFFF); // Reg_link_ov_val_nodie_pd_tx_cmfb_sstx0/1

    msWrite2Byte(REG_1527CE, 0x0404); // Reg_link_ov_en_nodie_pd_tx_cmfb_sstx0, Reg_link_ov_en_nodie_pd_tx_lane_sstx0
#endif
#endif
#elif(CHIP_ID == CHIP_MT9701) // } // (CHIP_ID == CHIP_MT9701) { 

    msWriteByte(REG_151940,0xC0);

    //msWriteByteMask(REG_151940,0xC0,BIT1|BIT6| BIT7);
    msWriteByte(REG_151942,0xA0);
    msWriteByte(REG_151943,0x14);
    msWriteByteMask(REG_15194E,BIT1,BIT1);
    while ((msReadByte(REG_15194A) & BIT0) == 0);        // polling bit <1> (CA_END)

	//GEN1 AUTOEQ setting
	msWriteByte(REG_152ACC, 0x04); // reg_autoeq_wait_cycle[7:0]
	msWriteByte(REG_152ACD, 0x00); // reg_autoeq_wait_cycle[9:8]
	msWriteByte(REG_152ABE, 0x00); // reg_autoeq_acc_timeout[7:0]
	msWriteByte(REG_152ABF, 0x01); // reg_autoeq_acc_timeout[15:8]
	msWriteByte(REG_152AC0, 0xE0); // reg_autoeq_acc_th
	msWriteByte(REG_152DCC, 0x04); // reg_autoeq_wait_cycle[7:0]
	msWriteByte(REG_152DCD, 0x00); // reg_autoeq_wait_cycle[9:8]
	msWriteByte(REG_152DBE, 0x00); // reg_autoeq_acc_timeout[7:0]
	msWriteByte(REG_152DBF, 0x01); // reg_autoeq_acc_timeout[15:8]
	msWriteByte(REG_152DC0, 0xE0); // reg_autoeq_acc_th

    #if 0
	// TX SSC setting
	msWriteByte(REG_152832, 0x74); // SSC set
	msWriteByte(REG_152833, 0x93);
	msWriteByte(REG_152834, 0x18);
	msWriteByte(REG_152838, 0xD8); // SSC span
	msWriteByte(REG_152839, 0x04);
	msWriteByte(REG_152836, 0x02); // SSC step
	msWriteByte(REG_152837, 0x70); // SSC step_frac
	msWriteByte(REG_15283A, 0x04); // SSC ssc_dn

	msWriteByte(REG_152B32, 0x74); // SSC set
	msWriteByte(REG_152B33, 0x93);
	msWriteByte(REG_152B34, 0x18);
	msWriteByte(REG_152B38, 0xD8); // SSC span
	msWriteByte(REG_152B39, 0x04);
	msWriteByte(REG_152B36, 0x02); // SSC step
	msWriteByte(REG_152B37, 0x70); // SSC step_frac
	msWriteByte(REG_152B3A, 0x04); // SSC ssc_dn
    #endif

	//RX_SQH setting
	//Enable C/HUB_RX_SQH
	msWriteByteMask(REG_15287F, BIT5, BIT5); // reg_c_data_sqh_en=1
	msWriteByteMask(REG_152B7F, BIT5, BIT5); // reg_c_data_sqh_en=1
	//Set RX_SQH Deglitch & Vth
	msWriteByte(REG_152678, 0x89); // Reg_rx_sqh_deglitch[7:0]
	msWriteByteMask(REG_152679, 0x04, 0x07); // Reg_rx_sqh_vth[2:0]
	msWriteByte(REG_152778, 0x89);// Reg_rx_sqh_deglitch[7:0]
	msWriteByteMask(REG_152779, 0x04, 0x07); // Reg_rx_sqh_vth[2:0]
	
	//Disable DTOP_CTRL SQH de-bounce
	msWriteByteMask(REG_152870, 0x00, 0x0F); // reg_data_sq_deb_set_counter
	msWriteByteMask(REG_152B70, 0x00, 0x0F); // reg_data_sq_deb_set_counter

	//U30C_CTRL Setting
	//AUTO_EQ timeout
	msWriteByte(REG_15286E, 0x90); // reg_train_ss_period_opt
	msWriteByte(REG_152B6E, 0x90); // reg_train_ss_period_opt
	//LFPS to TRAIN timeout
	msWriteByteMask(REG_15281B, BIT3, BIT2 | BIT3); // reg_lfps_train_period_opt[3:2]
	msWriteByteMask(REG_152B1B, BIT3, BIT2 | BIT3); // reg_lfps_train_period_opt[3:2]

    // RXDET threshold setting
    msWriteByte(REG_152802, 0x0A); // reg_rxdeet_connect_th[7:0]
    msWriteByte(REG_152B02, 0x0A);

	//Don't do RxDet in U23 of C side
	msWriteByteMask(REG_152B72, BIT5, BIT5); // Doni??i??t do rxdet in U23
	//Enable FSM jump U23 when u1 or u23 wakeup fail
	msWriteByteMask(REG_152872, 0x03, 0x03); // reg_u1_exit_to_go_u23, reg_u23_exit_to_go_u23
	msWriteByteMask(REG_152B72, 0x03, 0x03); // reg_u1_exit_to_go_u23, reg_u23_exit_to_go_u23
	//Set u1_enter_u23_period to 480ms (for u3_flag mode)
	msWriteByteMask(REG_15281A, 0x0C, 0x0C); // reg_u1_enter_u23_period_opt
	msWriteByteMask(REG_152B1A, 0x0C, 0x0C); // reg_u1_enter_u23_period_opt
	//8B/10B de-bounce enable for gen1 (for u3_flag mode)
	msWriteByteMask(REG_152814, BIT0, BIT0); // 8B/10B de-bounce enable
	msWriteByteMask(REG_152B14, BIT0, BIT0); // 8B/10B de-bounce enable
	//Warm_Reset detection setting
	msWriteByteMask(REG_152895, 0x0F, 0x0F); // Warm_Reset detection setting
	msWriteByteMask(REG_152B95, 0x0F, 0x0F); // Warm_Reset detection setting
	//TxGen issue dummy for tx clock unstable
	msWriteByteMask(REG_1528FA, BIT0, BIT0); // reg_dummy_7d[0]
	msWriteByteMask(REG_152BFA, BIT0, BIT0); // reg_dummy_7d[0]
	//new add: SSC detect tolerance setting.
	msWriteByteMask(REG_1529B5,0x04,0x0C); //reg_tx_syn_fdet_tolerance_opt[3:2]
	msWriteByteMask(REG_152CB5,0x04,0x0C); //reg_tx_syn_fdet_tolerance_opt[3:2]
	//new add: use clkswitch rise condition.
	msWriteByteMask(REG_1526F8, BIT4, BIT4); //reg_dummy_7c
	//Fix Polling_LFPS/SCD1 stick issue
	msWriteByteMask(REG_152AFE, 0x3F, 0x3F);
    msWriteByteMask(REG_152DFE, 0x3F, 0x3F);

    //Enable issue_u1_lfps. [Leger3] cancle
    //msWriteByteMask(REG_1528E4, BIT6, BIT6); // Issue u1_lfps by u3rt_lfps_tx
    //msWriteByteMask(REG_152BE4, BIT6, BIT6); // Issue u1_lfps by u3rt_lfps_tx

    //Change eidle_period_opt from 32us to 8us
    msWriteByte(REG_152871, 0x3F); // eidle period opt
    msWriteByte(REG_152B71, 0x3F); // eidle period opt

    //Extend TX FIFO depth
    msWriteByteMask(REG_15292C, 0x00, 0x07); // tx fifo to 32 depth
    msWriteByteMask(REG_152920, 0x10, 0x3F); // tx_fifo read point
    msWriteByteMask(REG_152C2C, 0x00, 0x07); // tx fifo to 32 depth
    msWriteByteMask(REG_152C20, 0x10, 0x3F); // tx_fifo read point

    // TXGEN OS SWITCH option
    msWriteByteMask(REG_1528FA, BIT1, BIT1); // reg_ksym_rpos_opt
    msWriteByteMask(REG_152BFA, BIT1, BIT1); // reg_ksym_rpos_opt
    msWriteByteMask(REG_1529B0, BIT0, BIT0); // reg_ov_en_skp_insert_opt
    msWriteByteMask(REG_152CB0, BIT0, BIT0); // reg_ov_en_skp_insert_opt    
    //U1 exit set to 300ns
    msWriteByte(REG_152032, 0x26); //reg_u1_bmin[7:0]
    msWriteByte(REG_152332, 0x26); //reg_u1_bmin[7:0]
    //Enable u3rt 16-32 fifo
    msWriteByteMask(REG_152070, BIT0, BIT0); //reg_pipe_fifo_en_rx
    msWriteByteMask(REG_152370, BIT0, BIT0); //reg_pipe_fifo_en_rx
    //U1Exit handshake timing
    msWriteByteMask(REG_1520B6, BIT1,BIT1); // reg_disable_check_U1exit_upper_bound
    msWriteByte(REG_1520B8, 0x64); // reg_u1_exit_hsk_upper_bound[11:0]=800us
    msWriteByte(REG_1520B9, 0x00); // reg_u1_exit_hsk_upper_bound[11:0]=800us
    msWriteByte(REG_1520BC, 0x0F); // reg_u1_exit_hsk_low_bound[11:0] = 120us
    msWriteByte(REG_1520BD, 0x00); // reg_u1_exit_hsk_low_bound[11:0] = 120us

    msWriteByteMask(REG_1523B6, BIT1,BIT1); // reg_disable_check_U1exit_upper_bound
    msWriteByte(REG_1523B8, 0x64); // reg_u1_exit_hsk_upper_bound[11:0]=800us
    msWriteByte(REG_1523B9, 0x00); // reg_u1_exit_hsk_upper_bound[11:0]=800us
    msWriteByte(REG_1523BC, 0x0F); // reg_u1_exit_hsk_low_bound[11:0] = 120us
    msWriteByte(REG_1523BD, 0x00); // reg_u1_exit_hsk_low_bound[11:0] = 120us

    //U3RD_enhance mode setting
    msWriteByteMask(REG_1529E8, 0x0E, 0x1F);
    msWriteByteMask(REG_152CE8, 0x0E, 0x1F);

//enable u3RT! TODO: need to check.
/*
    //Power On ATOP_DECAP
    msWriteByteMask(REG_152605, 0x00, BIT7); //reg_nodie_pd_decap
    msWriteByteMask(REG_152705 ,0x00, BIT7); //reg_nodie_pd_decap
    //Enable U30C_CTRL
    msWriteByteMask(REG_152817, BIT0, BIT0); //reg_nodie_pd_decap
    msWriteByteMask(REG_152816, BIT0, BIT0); //reg_nodie_pd_decap
*/
//end enable u3RT


    /******ECO option ******/
    // Disable look sqh enter U1 state
    msWriteByteMask(REG_15286F, 0x00, 0x60);
    msWriteByteMask(REG_152B6F, 0x00, 0x60);
    // Enable Active U1 exit
    msWriteByteMask(REG_1520E4, BIT6, BIT6);
    msWriteByteMask(REG_1523E4, BIT6, BIT6);
    // 20to20 AFIFO setting
    msWriteByteMask(REG_15292C, 0x00, 0x07); // reg_leo_20to20_afifo_half_fifo[2:0]
    msWriteByteMask(REG_152C2C, 0x00, 0x07); // reg_leo_20to20_afifo_half_fifo[2:0]
    // Extend reg_u1_exit_valid_period_opt for 100us long idle between LFPS and HS
    msWriteByte(REG_15286D, 0x32); //reg_u1_exit_valid_period_opt=8i??i??h32
    msWriteByte(REG_152B6D, 0x32); //reg_u1_exit_valid_period_opt=8i??i??h32

    // Reset u2 inactive timer when link disconnects
    msWriteByteMask(REG_1520BA, BIT1, BIT1);
    msWriteByteMask(REG_1523BA, BIT1, BIT1);
    // TX_synth_clk setting
    msWriteByte(REG_152832, 0x75); // SSC set
    msWriteByte(REG_152833, 0x93);
    msWriteByte(REG_152834, 0x18);
    msWriteByte(REG_152838, 0xD8); // SSC span
    msWriteByte(REG_152839, 0x04);
    msWriteByte(REG_152836, 0x03); // SSC step
    msWriteByte(REG_152837, 0x30); // SSC step frac
    msWriteByteMask(REG_15283A, 0x00, BIT2); // SSC ssc_dn=0
    msWriteByte(REG_152B32, 0x75); // SSC set
    msWriteByte(REG_152B33, 0x93);
    msWriteByte(REG_152B34, 0x18);
    msWriteByte(REG_152B38, 0xD8); // SSC span
    msWriteByte(REG_152B39, 0x04);
    msWriteByte(REG_152B36, 0x03); // SSC step
    msWriteByte(REG_152B37, 0x30); // SSC step frac
    msWriteByteMask(REG_152B3A, 0x00, BIT2); // SSC ssc_dn=0

    //msWriteByteMask(REG_15301F, BIT7, BIT7); 
    msWriteByteMask(REG_15301C,BIT1,BIT1); //ECO for host reset

    //RT_C_AINF (0x1526xx) 0xf8 [0] [4] [5] [6] = 1: enable pipe16to32fifo.
    msWriteByteMask(REG_1526F8, 0x71, 0x71);
    
#if  U3_REDRV_PM_MODE   // { 
        // Enable u3rt 16-32 fifo
        //msWriteByteMask(REG_152070, BIT0, BIT0); // reg_pipe_fifo_en_rx
        //msWriteByteMask(REG_152370, BIT0, BIT0);
    
        // HW set NODIE_PD_LFRX=1 & NODIE_PD_LFRX_VALID=1 when link enter U3    
        msWriteByte(REG_1526D9, 0xFF);
        msWriteByte(REG_1526CD, 0x44);
        msWriteByte(REG_1527D9, 0xFF);
        msWriteByte(REG_1527CD, 0x44);
       
        // HW set PD_BG_IGEN=1 when link enter U3    
        msWriteByteMask(REG_1526DC, 0xF0, 0xF0); // Reg_lin_ov_val_pd_bg
        msWriteByteMask(REG_1526D0, 0x40, 0xF0); // Reg_lin_ov_en_pd_bg
        msWriteByteMask(REG_1527DC, 0xF0, 0xF0); // Reg_lin_ov_val_pd_bg
        msWriteByteMask(REG_1527D0, 0x40, 0xF0); // Reg_lin_ov_en_pd_bg
    
        // HW set PD_RX_LANE=1 & PD_RX_LDO=1 when link enter U3    
        msWriteByteMask(REG_1526DF, 0x0F, 0x0F); // Reg_lin_ov_val_pd_rx_lane
        msWriteByteMask(REG_1526E1, 0xF0, 0xF0); // Reg_lin_ov_val_pd_rx_ldo
        msWriteByte(REG_1526D3, 0x44);
        msWriteByteMask(REG_1527DF, 0x0F, 0x0F); // Reg_lin_ov_val_pd_rx_lane
        msWriteByteMask(REG_1527E1, 0xF0, 0xF0); // Reg_lin_ov_val_pd_rx_ldo
        msWriteByte(REG_1527D3, 0x44);
    
        // HW set PD_RXPLL=1 when link enter U3    
        msWriteByteMask(REG_1526E1, 0x0F, 0x0F); // Reg_link_ov_val_pd_rxpll
        msWriteByteMask(REG_1526D5, 0x04, 0x0F); // Reg_link_ov_en_pd_rxpll
        msWriteByteMask(REG_1527E1, 0x0F, 0x0F); // Reg_link_ov_val_pd_rxpll
        msWriteByteMask(REG_1527D5, 0x04, 0x0F); // Reg_link_ov_en_pd_rxpll
    
        // HW set PD_RXPLL_UPDN_DMX=1 when link enter U3    
        msWriteByteMask(REG_1526E2, 0x0F, 0x0F); // Reg_lin_ov_val_pd_rxpll_updn_dmx
        msWriteByteMask(REG_1526D6, 0x04, 0x0F); // Reg_lin_ov_en_pd_rxpll_updn_dmx
        msWriteByteMask(REG_1527E2, 0x0F, 0x0F); // Reg_lin_ov_val_pd_rxpll_updn_dmx
        msWriteByteMask(REG_1527D6, 0x04, 0x0F); // Reg_lin_ov_en_pd_rxpll_updn_dmx
    
        // HW set PD_TXPLL=1 & TX_TAP=16i??i??hff_ff when link enter U3
        msWriteByteMask(REG_1526E3, 0x0F, 0x0F); // Reg_lin_ov_val_pd_txpll
        msWrite2Byte(REG_1526E4, 0xFFFF); // Reg_link_ov_val_pd_tx_tap_0
        msWriteByte(REG_1526D7, 0x44); // Reg_link_ov_en_pd_txpll, Reg_link_ov_en_pd_tx_tap_0
        msWriteByteMask(REG_1527E3, 0x0F, 0x0F); // Reg_lin_ov_val_pd_txpll
        msWrite2Byte(REG_1527E4, 0xFFFF); // Reg_link_ov_val_pd_tx_tap_0
        msWriteByte(REG_1527D7, 0x44); // Reg_link_ov_en_pd_txpll, Reg_link_ov_en_pd_tx_tap_0
    
        // HW set NODIE_PD_TX_CMFB=1 & NODIE_PD_TX_LANE_SSTX=1    
        msWrite2Byte(REG_1526DA, 0xFFFF); // Reg_link_ov_val_nodie_pd_tx_cmfb_sstx0/1
        msWrite2Byte(REG_1527DA, 0xFFFF); // Reg_link_ov_val_nodie_pd_tx_cmfb_sstx0/1
        msWrite2Byte(REG_1527CE, 0x0404); // Reg_link_ov_en_nodie_pd_tx_cmfb_sstx0, Reg_link_ov_en_nodie_pd_tx_lane_sstx0

#endif // }


#endif // } END (CHIP_ID == CHIP_MT9701)

}

void msDPCPadSwitchCfg(BYTE u8CC, BYTE u8PinAssign)
{
    if((u8PinAssign == 0x0) || (u8CC == 0x2)) //All USB
        msWriteByteMask(REG_152607, 0x00 , 0x0F);
    else if((u8PinAssign == BIT2) || (u8PinAssign == BIT4)) //All DP
        msWriteByteMask(REG_152607, 0x0F , 0x0F);
    else if(u8CC == 0) // 2 by 2, non flip
        msWriteByteMask(REG_152607, 0x09 , 0x0F);
    else // 2 by 2, flip
        msWriteByteMask(REG_152607, 0x06 , 0x0F);
}

void msDPCRtermCfg(BYTE u8CC, BYTE u8PinAssign)
{
    if((u8PinAssign == 0x0) || (u8CC == 0x2)) //All USB
        msWriteByteMask(REG_002011, 0x0F, 0x0F);
    else if((u8PinAssign == BIT2) || (u8PinAssign == BIT4)) //All DP
        msWriteByteMask(REG_002011, 0x00, 0x0F);
    else if(u8CC == 0) // 2 by 2, non flip
        msWriteByteMask(REG_002011, 0x03 , 0x0F);
    else // 2 by 2, flip
        msWriteByteMask(REG_002011, 0x0C , 0x0F);
}

#endif

void msDrvUsbSwitchRtermEnDP(BOOL bEnableDP)
{
#if (CHIP_ID == CHIP_MT9700)
    if(Input_UsbTypeC_C4 != Input_Nothing)
    {
        msWriteByteMask(REG_0020D8, 0x00, BIT2);
        msWriteByteMask(REG_002011, bEnableDP?0x00:0x0F, 0x0F);
    }
#elif(CHIP_ID == CHIP_MT9701)
    if(Input_UsbTypeC_C3 != Input_Nothing)
    {
        msWriteByteMask(REG_0020D8, 0x00, BIT2);
        msWriteByteMask(REG_002011, bEnableDP?0x00:0x0F, 0x0F);
    }
#endif
}

void msDrvUsbSwitchRtermEnTypeC(BOOL bEnableTypeC)
{
    
#if (CHIP_ID == CHIP_MT9700)
    if(Input_UsbTypeC_C4 != Input_Nothing)
    {
        msWriteByteMask(REG_0020D8, 0x00, BIT2);
        msWriteByteMask(REG_002011, bEnableTypeC?0x00:0x0F, 0x0F);
    }
#elif(CHIP_ID == CHIP_MT9701)
    if(Input_UsbTypeC_C3 != Input_Nothing)
    {
        msWriteByteMask(REG_0020D8, 0x00, BIT2);
        msWriteByteMask(REG_002011, bEnableTypeC?0x00:0x0F, 0x0F);
    }
#endif
}

void msDrvDPCRtermTrimInitNonPM( void )
{
    BYTE xdata u8EfuseActive, u8EfuseValue;
#if (CHIP_ID == CHIP_MT9700)

    // DP1_RTERM_VALUE
    u8EfuseActive = msEread_GetDataFromEfuse(0x140);
    u8EfuseValue = msEread_GetDataFromEfuse(0x13D);
    if(u8EfuseActive & BIT7)
        msWriteByteMask(REG_152674, u8EfuseValue>>4, 0x0F);

    // DP0_RTERM_VALUE
    u8EfuseActive = msEread_GetDataFromEfuse(0x141);
    u8EfuseValue = msEread_GetDataFromEfuse(0x13E);
    if(u8EfuseActive & BIT0)
        msWriteByteMask(REG_152675, u8EfuseValue<<4, 0xF0);

    // TX0_RTERM_VALUE
    if(u8EfuseActive & BIT1)
        msWriteByteMask(REG_152670, u8EfuseValue>>4, 0x0F);

    // TX1_RTERM_VALUE
    u8EfuseValue = msEread_GetDataFromEfuse(0x13F);
    if(u8EfuseActive & BIT2)
        msWriteByteMask(REG_152671, u8EfuseValue<<4, 0xF0);

    // RX0_DP2_RTERM_VALUE
    if(u8EfuseActive & BIT3)
    {
        msWriteByteMask(REG_152670, u8EfuseValue, 0xF0);
        msWriteByteMask(REG_152674, u8EfuseValue, 0xF0);
    }

    // RX1_DP3_RTERM_VALUE
    u8EfuseValue = msEread_GetDataFromEfuse(0x140);
    if(u8EfuseActive & BIT4)
    {
        msWriteByteMask(REG_152671, u8EfuseValue, 0x0F);
        msWriteByteMask(REG_152675, u8EfuseValue, 0x0F);
    }

    // TX0_Swing_Setting
    if(u8EfuseActive & BIT5)
    {
        u8EfuseValue = msEread_GetDataFromEfuse(0x142);
        msWriteByte(REG_15263A, u8EfuseValue);
    }
    
    // TX1_Swing_Setting
    if(u8EfuseActive & BIT6)
    {
        u8EfuseValue = msEread_GetDataFromEfuse(0x143);
        msWriteByte(REG_15263B, u8EfuseValue);
    }

    // RX0_RTERM_MODE    
    u8EfuseActive = msEread_GetDataFromEfuse(0x146);
    u8EfuseValue = msEread_GetDataFromEfuse(0x144);
    if(u8EfuseActive & BIT0)
        msWriteByteMask(REG_152770, u8EfuseValue<<4, 0xF0);

    // TX0_RTERMP_MODE
    if(u8EfuseActive & BIT1)
        msWriteByteMask(REG_152770, u8EfuseValue>>4, 0x0F);

    // TX0_RTERMP_MODE
    if(u8EfuseActive & BIT2)
    {
        u8EfuseValue = msEread_GetDataFromEfuse(0x145);
        msWriteByte(REG_15273A, u8EfuseValue);
    }

#elif(CHIP_ID == CHIP_MT9701)
    // DP1_RTERM_VALUE
    u8EfuseActive = msEread_GetDataFromEfuse(0x145);
    u8EfuseValue = msEread_GetDataFromEfuse(0x140);
    if(u8EfuseActive & BIT0)
        msWriteByteMask(REG_152674, u8EfuseValue, 0x0F);

    // DP0_RTERM_VALUE
    if(u8EfuseActive & BIT1)
        msWriteByteMask(REG_152675, u8EfuseValue, 0xF0);

    // TX0_RTERM_VALUE
    u8EfuseValue = msEread_GetDataFromEfuse(0x141);
    if(u8EfuseActive & BIT2)
        msWriteByteMask(REG_152670, u8EfuseValue, 0x0F);

    // TX1_RTERM_VALUE
    if(u8EfuseActive & BIT3)
        msWriteByteMask(REG_152671, u8EfuseValue, 0xF0);

    // RX0_DP2_RTERM_VALUE
    u8EfuseValue = msEread_GetDataFromEfuse(0x142);
    if(u8EfuseActive & BIT4)
    {
        msWriteByteMask(REG_152670, u8EfuseValue << 4, 0xF0);
        msWriteByteMask(REG_152674, u8EfuseValue << 4, 0xF0);
    }

    // RX1_DP3_RTERM_VALUE
    if(u8EfuseActive & BIT5)
    {
        msWriteByteMask(REG_152671, u8EfuseValue >> 4, 0x0F);
        msWriteByteMask(REG_152675, u8EfuseValue >> 4, 0x0F);
    }

    // TX0_Swing_Setting
    u8EfuseValue = msEread_GetDataFromEfuse(0x143);
    if(u8EfuseActive & BIT6){
        msWriteByte(REG_15263A, u8EfuseValue);
    }

    // TX1_Swing_Setting
    u8EfuseValue = msEread_GetDataFromEfuse(0x144);
    if(u8EfuseActive & BIT7)
        msWriteByte(REG_15263B, u8EfuseValue);

    // RX0_RTERM_MODE    
    u8EfuseActive = msEread_GetDataFromEfuse(0x148);
    u8EfuseValue = msEread_GetDataFromEfuse(0x146);
    if(u8EfuseActive & BIT0)
        msWriteByteMask(REG_152770, u8EfuseValue<<4, 0xF0);

    // TX0_RTERMP_MODE
    if(u8EfuseActive & BIT1)
        msWriteByteMask(REG_152770, u8EfuseValue>>4, 0x0F);

    // TX0_RTERMP_MODE
    u8EfuseValue = msEread_GetDataFromEfuse(0x147);
    if(u8EfuseActive & BIT2)
        msWriteByte(REG_15273A, u8EfuseValue);


#endif
}

#if ENABLE_INTERNAL_CC
void msDrvInternalCcTrimInitPM( void )
{
    BYTE xdata u8EfuseValue;

    u8EfuseValue = msEread_GetDataFromEfuse(0x137);
    if(u8EfuseValue & BIT7)
    {
        msWriteByteMask(REG_003322, u8EfuseValue<<6, 0xC0);
        msWriteByteMask(REG_003323, u8EfuseValue>>2, 0x0F);
    }
}

void msDrvInternalCcTrimInitNonPM(void)
{
    BYTE xdata u8EfuseActive, u8EfuseValue;

    u8EfuseActive = msEread_GetDataFromEfuse(0x136);
    // cc_atop
    if(u8EfuseActive & BIT7)
    {
        u8EfuseValue = msEread_GetDataFromEfuse(0x132);
        // VCONN OVP voltage, reference voltage 
        msWriteByteMask(REG_15320E, u8EfuseValue,0x1F);

        // LDO_TX voltage, RP current
        u8EfuseValue = msEread_GetDataFromEfuse(0x133);
        msWriteByte(REG_15320D, u8EfuseValue);

        u8EfuseValue = msEread_GetDataFromEfuse(0x134);
        // RD impedance @ CC1
        msWriteByteMask(REG_15320C, u8EfuseValue, 0x0F);
        // TX Impedance H
        msWriteByteMask(REG_15320F, u8EfuseValue>>1, 0x30);

        // RD impedance @ CC2
        u8EfuseValue = msEread_GetDataFromEfuse(0x135);
        msWriteByteMask(REG_15320C, u8EfuseValue<<4, 0xF0);
        // TX Impedance L
        msWriteByteMask(REG_153204, u8EfuseValue>>3, 0x0C);

        // VCONN OCP current    
        u8EfuseValue = msEread_GetDataFromEfuse(0x136);
        msWriteByteMask(REG_15320F, u8EfuseValue, 0x0F);
    }    

}
#endif

void msDPCPadSwitchInit(void)
{ 
#if (CHIP_ID == CHIP_MT9700)
    #if(CInput_Displayport_C4!=CInput_Nothing)
        msWriteByteMask(REG_152607, 0x0F , 0x0F);
    #elif ENABLE_USB_TYPEC
       msWriteByteMask(REG_152607, 0x00 , 0x0F);
    #endif
#elif(CHIP_ID == CHIP_MT9701)
    #if(CInput_Displayport_C3!=CInput_Nothing)
        msWriteByteMask(REG_152607, 0x0F , 0x0F);
    #elif ENABLE_USB_TYPEC
       msWriteByteMask(REG_152607, 0x00 , 0x0F);
    #endif
#endif
}

void msDPCRtermModeInit(void)
{
    msWriteByteMask(REG_152673, 0x01 , 0x07); //SSTX0
    msWriteByteMask(REG_152673, 0x10 , 0x70); //SSTX1
}

void msDrvUsbSwitchSqulechInit(void)
{
    msWriteByteMask(REG_002010, 0x02, 0x07); //SQH_Vth
    msWriteByteMask(REG_002013, BIT0, BIT0); //SQ EN
    msWriteByteMask(REG_002010, 0xF0, 0xF0);
}

void msDrvDPCInit(void)
{

#if (CHIP_ID == CHIP_MT9700)

#if(CInput_UsbTypeC_C4!=CInput_Nothing) || (CInput_Displayport_C4!=CInput_Nothing)
	//msDPCRtermModeInit();
	msDrvUsbSwitchSqulechInit();
    msDPCPadSwitchInit();
#if ENABLE_INTERNAL_CC
    msDrvInternalCcTrimInitPM();
#endif
#if ENABLE_USB_TYPEC // combo4 is TypeC
	msDrvUsbSwitchRtermEnTypeC(FALSE);
	msDrvUsb2Init(); // For usb2 device(e.g. BB ... etc)
	msDrvUsbSwitchInit();
#endif
#endif

#elif(CHIP_ID == CHIP_MT9701)

#if(CInput_UsbTypeC_C3!=CInput_Nothing) || (CInput_Displayport_C3!=CInput_Nothing)
	//msDPCRtermModeInit();
    msDrvUsbSwitchSqulechInit();
    msDPCPadSwitchInit();

#if ENABLE_USB_TYPEC // combo4 is TypeC
	msDrvUsbSwitchRtermEnTypeC(FALSE);
	msDrvUsb2Init(); // For usb2 device(e.g. BB ... etc)
	msDrvUsbSwitchInit();
#endif

    //setting by Aiden Irving
    msWriteByte(REG_152607, 0x0f); //en DPC SW L0/1/2/3

    msWriteByte(REG_152688, 0xf0); //reg_ov_en_pd_rt_sstx 0/1
    msWriteByte(REG_1526BA, 0x00); //reg_ov_val_pd_rt_sstx 0/1 

    msWriteByte(REG_152689, 0x03); //reg_ov_en_reg_rterm_mode_sstx0/1
    msWriteByte(REG_1526BC, 0x11); //reg_ov_val_reg_rterm_mode_sstx0/1
    //USB initial mis-match modify  
    msWriteByte(REG_152606, 0x00); //reg_en_rxclko_to_prdt_div2

    msWriteByte(REG_152622, 0xCD); //reg_gcr_rxpll_divsel_in

    msWriteByte(REG_152684, 0x80); //reg_ov_en_gcr_rxpll_ictl_pd
    msWriteByte(REG_1526A4, 0x04); //reg_ov_val_gcr_rxpll_ictl_pd

    msWriteByte(REG_152685, 0x01); //reg_ov_en_gcr_rxpll_ictrl_pfd
    msWriteByte(REG_1526A5, 0x06); //reg_ov_vai_gcr_rxpll_ictrl_pfd

    msWriteByte(REG_152622, 0x6d); //reg_gcr_rxpll_lpdiv
    msWriteByte(REG_1526C2, 0x0c); //reg_ov_en_reg_tx_hpd_mode_sstx0/1
    msWriteByte(REG_1526A6, 0x00); //reg_ov_val_reg_tx_hpd_mode_sstx0/1

    msWriteByte(REG_152685, 0x21); //reg_ov_en_gcr_rx_eq
    msWriteByte(REG_1526A9, 0x00); //reg_ov_val_gcr_rx_eq
    msWriteByte(REG_152628, 0x80); //reg_Rxpll_test
    msWriteByte(REG_152660, 0xa9); //teg_TX_ictrl_TAP0
    msWriteByte(REG_1526FE, 0x27); //teg_TX_ictrl_TAP1

#endif
#endif
}

void UTMI_Init(void)
{
	UTMI_REG_WRITE8(0x02, (UTMI_REG_READ8(0x02)) | 0x04);
	UTMI_REG_WRITE8(0x03, (UTMI_REG_READ8(0x03) & ~0x10) | 0x08 | 0x20);
	UTMI_REG_WRITE8(0x04, (UTMI_REG_READ8(0x04)) | 0x40 | 0x20);
	UTMI_REG_WRITE8(0x06, (UTMI_REG_READ8(0x06) & ~0x04 & ~0x20) | 0x40);
	UTMI_REG_WRITE8(0x07, (UTMI_REG_READ8(0x07) & ~0x02));
	UTMI_REG_WRITE8(0x08, (UTMI_REG_READ8(0x08) & ~0x80) | 0x04 |0x08);
	UTMI_REG_WRITE8(0x09, (UTMI_REG_READ8(0x09)) | 0x80 |0x01);
	UTMI_REG_WRITE8(0x0b, (UTMI_REG_READ8(0x0b)) | 0x80);
	UTMI_REG_WRITE8(0x15, (UTMI_REG_READ8(0x15)) | 0x20 | 0x40);

	UTMI_REG_WRITE8(0x24, 0x00);
//	UTMI_REG_WRITE8(0x29, 0x00); //same address with USB Rterm trimming
	UTMI_REG_WRITE8(0x2a, 0x00);

	UTMI_REG_WRITE8(0x3f, (UTMI_REG_READ8(0x3f)) | 0x80);
}

void upll_enable(void)
{
    if( msReadByte( 0x151800) & BIT1)
    {
        msWrite2Byte(0x151800, msRead2Byte(0x151800) & ~BIT1 & ~BIT4 & ~BIT5);  // upll power on
        msWrite2Byte(0x15180e, msRead2Byte(0x15180e) | BIT0);
    }

    if(!((msRead2Byte(0x150322) == 0x2088) && (msRead2Byte(0x150320) == 0x8051)))
    {
    	//utmi:0x1503, usbc:0x1502
    	msWrite2Byte( 0x150308, 0x040f);
    	msWrite2Byte( 0x150300, 0x7f05);
    	msWriteByte( 0x150352, 0x00);

    	msWriteByte( 0x150200, 0x0a);
    	msWriteByte( 0x150200, 0x28);

    	msWrite2Byte( 0x150322, 0x2088);
    	msWrite2Byte( 0x150320, 0x8051);
    	msWrite2Byte( 0x150302, 0x2084);
    	msWrite2Byte( 0x150308, 0x0426);

    	msWrite2Byte( 0x150300, 0x6bc3);
    	msWriteByte( 0x150352, 0x3f);
    	ForceDelay1ms( 1 );
    	msWrite2Byte( 0x150300, 0x69c3);
    	msWriteByte( 0x150352, 0x3f);
    	ForceDelay1ms( 2 );
    	msWrite2Byte( 0x150300, 0x0001);
    	msWriteByte( 0x150352, 0x00);
    }
}

void msDrvUsb2Init(void)
{
	upll_enable();
    {
    	int ii=0;

    	USBC_REG_WRITE8(0x02, (USBC_REG_READ8(0x02)& ~(BIT0|BIT1)) | (BIT1));
    	UTMI_REG_WRITE8(0x3C, UTMI_REG_READ8(0x3C) | 0x1); // set CA_START as 1
    	for(ii=0;ii<1000;ii++)
    	{
    		//nothing
    	}

    	UTMI_REG_WRITE8(0x3C, UTMI_REG_READ8(0x3C) & ~0x01); // release CA_START
    	while ((UTMI_REG_READ8(0x3C) & 0x02) == 0);        // polling bit <1> (CA_END)

    	// Reset OTG controllers
    	USBC_REG_WRITE8(0, 0xC);

    	// Unlock Register R/W functions  (RST_CTRL[6] = 1)
    	// Enter suspend  (RST_CTRL[3] = 1)
    	USBC_REG_WRITE8(0, 0x48);

    	UTMI_Init();

    	// 2'b10: OTG enable
    	USBC_REG_WRITE8(0x02, (USBC_REG_READ8(0x02)& ~(BIT0|BIT1)) | (BIT1));
    }
    
#if CHIP_ID == CHIP_MT9700
	//swap
	UTMI_REG_WRITE8(0x0b, UTMI_REG_READ8(0xb) | 0x20);
#endif

#if ENABLE_USB_DEVICE
    USB_Device_Init();
#endif
}


BYTE code USBSWNullData[] = {0};
void USBSWDummy(void)
{
    BYTE xdata i = USBSWNullData[0];
    i = i;
}

