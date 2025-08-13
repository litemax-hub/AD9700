#ifndef _LPLL_H_
#define _LPLL_H_

#define LPLL_REG_NUM          37

typedef struct
{
    BYTE  address;
    WORD value;
    WORD mask;
}TBLStruct,*pTBLStruct;

typedef enum
{
    E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_100to150MHz,           // 6
    E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_75to100MHz,            // 7
    E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_75to75MHz,             // 8
    E_PNL_SUPPORTED_LPLL_LVDS_1ch_1P_50to75MHz,             // 0
    E_PNL_SUPPORTED_LPLL_LVDS_1ch_1P_37_5to50MHz,           // 1
    E_PNL_SUPPORTED_LPLL_LVDS_1ch_1P_37_5to37_5MHz,         // 2
    E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_200to300MHz,           // 9
    E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_150to200MHz,           // 10
    E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_150to150MHz,           // 11
    E_PNL_SUPPORTED_LPLL_LVDS_2ch_1P_125to200MHz,           // 3
    E_PNL_SUPPORTED_LPLL_LVDS_2ch_1P_75to125MHz,            // 4
    E_PNL_SUPPORTED_LPLL_LVDS_2ch_1P_75to75MHz,             // 5
    E_PNL_SUPPORTED_LPLL_VBY1_2ch_10bit_150to300MHz,        //12
    E_PNL_SUPPORTED_LPLL_VBY1_2ch_10bit_150to150MHz,        //13
    E_PNL_SUPPORTED_LPLL_VBY1_4ch_10bit_150to400MHz,        //14
    E_PNL_SUPPORTED_LPLL_VBY1_4ch_10bit_75to150MHz,         //15
    E_PNL_SUPPORTED_LPLL_VBY1_4ch_10bit_75to75MHz,          //16
    E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case1_300to400MHz,  //17
    E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case1_150to300MHz,  //18
    E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case1_75to150MHz,   //19
    E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case1_75to75MHz,    //20
    E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case2_300to520MHz,  //21
    E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case2_200to300MHz,  //22
    E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case2_130to200MHz,  //23
    E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case2_75to130MHz,   //24
    E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case2_75to75MHz,    //25
    E_PNL_SUPPORTED_LPLL_VBY1_2ch_8bit_200to300MHz,         //26
    E_PNL_SUPPORTED_LPLL_VBY1_2ch_8bit_150to200MHz,         //27
    E_PNL_SUPPORTED_LPLL_VBY1_2ch_8bit_150to150MHz,         //28
    E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_200to400MHz,         //29
    E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_100to200MHz,         //30
    E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_75to100MHz,          //31
    E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_75to75MHz,           //32
    E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case1_200to400MHz,   //33
    E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case1_100to200MHz,   //34
    E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case1_75to100MHz,    //35
    E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case1_75to75MHz,     //36
    E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case2_200to520MHz,   //37
    E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case2_120to200MHz,   //38
    E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case2_75to120MHz,    //39
    E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case2_75to75MHz,     //40
    E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate_150to400MHz,          //41
    E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate_75to150MHz,          //42
    E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate_37_5to75MHz,          //43
    E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate_25to37_5MHz,          //44
    E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate_12_5to25MHz,          //45
    E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate_12_5to12_5MHz,          //46
    E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate_150to400MHz,          //47
    E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate_75to150MHz,          //48
    E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate_37_5to75MHz,          //49
    E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate_25to37_5MHz,          //50
    E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate_12_5to25MHz,          //51
    E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate_12_5to12_5MHz,          //52
     E_PNL_SUPPORTED_LPLL_MAX,
} E_PNL_SUPPORTED_LPLL_TYPE;

TBLStruct code LPLLSettingTBL[E_PNL_SUPPORTED_LPLL_MAX][LPLL_REG_NUM]=
{
    { //E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_100to150MHz    NO.6
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0001,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0700,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0700,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0020,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x0000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x8000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0002,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0000,0x0800},//reg_lpll2_test[11]
        {0x39,0x1000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0200,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0600,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x0008,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0000,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0000,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_75to100MHz    NO.7
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0001,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0700,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0700,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x1000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0020,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x0000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x8000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0003,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0000,0x0800},//reg_lpll2_test[11]
        {0x39,0x1000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0200,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0600,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x0010,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0000,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0000,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_75to75MHz    NO.8
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0001,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0700,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0700,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x1000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0020,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x0000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x8000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0003,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0000,0x0800},//reg_lpll2_test[11]
        {0x39,0x1000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0200,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0600,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x0010,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0000,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0000,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_LVDS_1ch_1P_50to75MHz    NO.0
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0001,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0700,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0700,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x1000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0020,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x0000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x8000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0003,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0000,0x0800},//reg_lpll2_test[11]
        {0x39,0x1000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0200,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0600,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x0010,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0000,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0000,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_LVDS_1ch_1P_37_5to50MHz    NO.1
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0001,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0700,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0700,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x2000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0020,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x0000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x8000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0003,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x1000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0200,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0600,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x0010,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0000,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0000,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_LVDS_1ch_1P_37_5to37_5MHz    NO.2
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0001,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0700,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0700,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x2000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0020,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x0000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x8000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0003,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x1000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0200,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0600,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x0010,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0000,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0000,0x00FF},//reg_lpll_loop_div_ext
    },
    { //E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_200to300MHz    NO.9
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0001,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0700,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0700,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0020,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x0000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x8000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0001,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0000,0x0800},//reg_lpll2_test[11]
        {0x39,0x1000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0200,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0600,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x0004,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0000,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0000,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_150to200MHz    NO.10
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0001,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0700,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0700,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x1000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0020,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x0000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x8000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0002,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0000,0x0800},//reg_lpll2_test[11]
        {0x39,0x1000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0200,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0600,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x0008,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0000,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0000,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_150to150MHz    NO.11
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0001,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0700,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0700,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x1000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0020,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x0000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x8000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0002,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0000,0x0800},//reg_lpll2_test[11]
        {0x39,0x1000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0200,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0600,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x0008,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0000,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0000,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_LVDS_2ch_1P_125to200MHz    NO.3
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0001,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0700,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0700,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x1000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0020,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x0000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x8000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0002,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0000,0x0800},//reg_lpll2_test[11]
        {0x39,0x1000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0200,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0600,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x0008,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0000,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0000,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_LVDS_2ch_1P_75to125MHz    NO.4
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0001,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0700,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0700,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x2000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0020,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x0000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x8000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0002,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x1000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0200,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0600,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x0008,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0000,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0000,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_LVDS_2ch_1P_75to75MHz    NO.5
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0001,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0700,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0700,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x2000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0020,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x0000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x8000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0002,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x1000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0200,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0600,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x0008,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0000,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0000,0x00FF},//reg_lpll_loop_div_ext
    },
    { //E_PNL_SUPPORTED_LPLL_VBY1_2ch_10bit_150to300MHz    NO.12
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0200,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0500,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0000,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0001,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x1000,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000A,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0004,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_2ch_10bit_150to150MHz    NO.13
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0200,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0500,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0000,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0001,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x1000,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000A,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0004,0x00FF},//reg_lpll_loop_div_ext
    },
    { //E_PNL_SUPPORTED_LPLL_VBY1_4ch_10bit_150to400MHz    NO.14
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0200,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0500,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0000,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0001,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x1000,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000A,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0004,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_4ch_10bit_75to150MHz    NO.15
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0500,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0002,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0800,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000A,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0008,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_4ch_10bit_75to75MHz    NO.16
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0500,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0002,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0800,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000A,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0008,0x00FF},//reg_lpll_loop_div_ext
    },
    { //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case1_300to400MHz    NO.17
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0200,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0500,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0000,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0001,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x1000,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x0005,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0004,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case1_150to300MHz    NO.18
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0500,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0001,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0800,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x0005,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0004,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case1_75to150MHz    NO.19
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0500,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x1000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0002,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0800,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000A,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0008,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case1_75to75MHz    NO.20
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0500,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x1000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0002,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0800,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000A,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0008,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case2_300to520MHz    NO.21
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0200,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0500,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0000,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0001,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0001,0x0001},//reg_lpll1_fix_set
        {0x2C,0x1000,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x0005,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0004,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case2_200to300MHz    NO.22
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0500,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0001,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0001,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0800,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x0005,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0004,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case2_130to200MHz    NO.23
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0500,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0002,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0001,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0800,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x0005,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0008,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case2_75to130MHz    NO.24
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0500,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x1000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0003,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0001,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0800,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000A,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0010,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case2_75to75MHz    NO.25
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0500,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x1000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0003,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0001,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0800,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000A,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0010,0x00FF},//reg_lpll_loop_div_ext
    },
    { //E_PNL_SUPPORTED_LPLL_VBY1_2ch_8bit_200to300MHz    NO.26
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0200,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0300,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0000,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0400,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0001,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x2000,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000F,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0004,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_2ch_8bit_150to200MHz    NO.27
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0300,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0400,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0002,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x1000,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000F,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0008,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_2ch_8bit_150to150MHz    NO.28
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0300,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0400,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0002,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x1000,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000F,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0008,0x00FF},//reg_lpll_loop_div_ext
    },
    { //E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_200to400MHz    NO.29
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0200,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0300,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0000,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0400,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0001,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x2000,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000F,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0004,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_100to200MHz    NO.30
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0300,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0400,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0002,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x1000,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000F,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0008,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_75to100MHz    NO.31
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0300,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x1000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0400,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0003,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0800,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000F,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0010,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_75to75MHz    NO.32
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0300,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x1000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0400,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0003,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0800,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000F,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0010,0x00FF},//reg_lpll_loop_div_ext
    },
    { //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case1_200to400MHz    NO.33
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0200,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0300,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x1000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0000,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0400,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0001,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x2000,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000F,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0004,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case1_100to200MHz    NO.34
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0300,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x1000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0400,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0002,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x1000,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000F,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0008,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case1_75to100MHz    NO.35
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0300,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x2000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0400,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0003,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0800,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000F,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0010,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case1_75to75MHz    NO.36
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0300,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x2000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0400,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0003,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0000,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0800,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000F,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0010,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case2_200to520MHz    NO.37
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0200,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0300,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x1000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0000,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0400,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0001,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0001,0x0001},//reg_lpll1_fix_set
        {0x2C,0x2000,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000F,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0004,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case2_120to200MHz    NO.38
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0300,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x1000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0400,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0002,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0001,0x0001},//reg_lpll1_fix_set
        {0x2C,0x1000,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000F,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0008,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case2_75to120MHz    NO.39
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0300,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x2000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0400,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0003,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0001,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0800,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000F,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0010,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case2_75to75MHz    NO.40
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x1000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0300,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x2000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0000,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0400,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0000,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0010,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0003,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0001,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0800,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000F,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0010,0x00FF},//reg_lpll_loop_div_ext
    },
    { //E_PNL_SUPPORTED_LPLL_eDP_High_bit_rate _270to270MHz    NO.41
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0200,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0000,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0800,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0000,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0040,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0000,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0001,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0001,0x0001},//reg_lpll1_fix_set
        {0x2C,0x1000,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000A,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0004,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_eDP_High_bit_rate _270to270MHz    NO.42
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0200,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0000,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0800,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0000,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0040,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0000,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0002,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0001,0x0001},//reg_lpll1_fix_set
        {0x2C,0x1000,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000A,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0008,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_eDP_High_bit_rate _270to270MHz    NO.43
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0200,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0000,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0800,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0000,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0040,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0000,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0003,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0001,0x0001},//reg_lpll1_fix_set
        {0x2C,0x1000,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000A,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0010,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_eDP_High_bit_rate _270to270MHz    NO.44
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0200,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0000,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0800,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0000,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0040,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0000,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0004,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0002,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x000A,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0001,0x0001},//reg_lpll1_fix_set
        {0x2C,0x1000,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000A,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0C00,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0030,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_eDP_High_bit_rate _270to270MHz    NO.45
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0200,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0000,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0800,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0000,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0040,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0000,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x000A,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0001,0x0001},//reg_lpll1_fix_set
        {0x2C,0x1000,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000A,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0030,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_eDP_High_bit_rate _270to270MHz    NO.46
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0200,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0000,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0800,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0000,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0040,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0000,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x000A,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0001,0x0001},//reg_lpll1_fix_set
        {0x2C,0x1000,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000A,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0030,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_eDP_Reduce_bit_rate _162to162MHz    NO.47
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0000,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0800,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0040,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0000,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0001,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0001,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0800,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000A,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0004,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_eDP_Reduce_bit_rate _162to162MHz    NO.48
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0000,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0800,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0040,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0000,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0002,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0001,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0800,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000A,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0008,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_eDP_Reduce_bit_rate _162to162MHz    NO.49
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0000,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0800,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0040,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0000,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x0003,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0001,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0800,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000A,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0010,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_eDP_Reduce_bit_rate _162to162MHz    NO.50
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0000,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0800,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0040,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0000,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0004,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0002,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x000A,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0001,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0800,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000A,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0C00,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0030,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_eDP_Reduce_bit_rate _162to162MHz    NO.51
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0000,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0800,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0040,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0000,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x000A,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0001,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0800,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000A,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0030,0x00FF},//reg_lpll_loop_div_ext
    },

    { //E_PNL_SUPPORTED_LPLL_eDP_Reduce_bit_rate _162to162MHz    NO.52
      //Address,Value,Mask
        {0x03,0x0004,0x001C},//reg_lpll1_ibias_ictrl
        {0x15,0x0000,0x0003},//reg_lpll1_input_div_first
        {0x01,0x0003,0x0003},//reg_lpll1_loop_div_first
        {0x01,0x0000,0x1F00},//reg_lpll1_loop_div_second
        {0x02,0x0000,0x3000},//reg_lpll1_output_div_second[13:12]
        {0x02,0x0000,0x0F00},//reg_lpll1_output_div_second[11:8]
        {0x35,0x0000,0x7000},//reg_lpll1_skew_div
        {0x2E,0x0000,0x0007},//reg_lpll1_fifo_div
        {0x03,0x0800,0x0800},//reg_lpll1_fifo_div5_en
        {0x2E,0x1000,0x1000},//reg_lpll1_en_fix_clk
        {0x35,0x0000,0x0020},//reg_lpll_2ndpll_clk_sel
        {0x37,0x0008,0x0008},//reg_lpll1_test[19]
        {0x37,0x0000,0x0010},//reg_lpll1_test[20]
        {0x2E,0x8000,0x8000},//reg_lpll1_dual_lp_en
        {0x03,0x0000,0x0400},//reg_lpll1_sdiv2p5_en
        {0x2E,0x0000,0x4000},//reg_lpll1_en_mini
        {0x2E,0x0040,0x0040},//reg_lpll1_en_fifo
        {0x2E,0x0000,0x0010},//reg_lpll_en_scalar
        {0x37,0x0000,0x0004},//reg_lpll1_test[18]
        {0x36,0x0000,0x8000},//reg_lpll1_test[15]
        {0x37,0x0000,0x0001},//reg_lpll1_test[16]
        {0x37,0x0000,0x0040},//reg_lpll1_test[22]
        {0x33,0x0000,0x0020},//reg_lpll2_pd
        {0x33,0x0000,0x0004},//reg_lpll2_ibias_ictrl
        {0x30,0x0000,0x001F},//reg_lpll2_input_div_first
        {0x31,0x0001,0x0003},//reg_lpll2_loop_div_first
        {0x31,0x0300,0x1F00},//reg_lpll2_loop_div_second
        {0x32,0x000A,0x000F},//reg_lpll2_output_div_first
        {0x39,0x0800,0x0800},//reg_lpll2_test[11]
        {0x39,0x0000,0x1000},//reg_lpll2_test[12]
        {0x39,0x0000,0x0100},//reg_lpll2_test[8]
        {0x39,0x0000,0x0200},//reg_lpll2_test[9]
        {0x6A,0x0001,0x0001},//reg_lpll1_fix_set
        {0x2C,0x0800,0xFF00},//reg_lpll_loop_gain
        {0x2C,0x000A,0x00FF},//reg_lpll_loop_div
        {0x2D,0x0600,0xFF00},//reg_lpll_loop_gain_ext
        {0x2D,0x0030,0x00FF},//reg_lpll_loop_div_ext
    },
};

BYTE code u8LoopGain[E_PNL_SUPPORTED_LPLL_MAX]=
{
    6,           //E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_100to150MHz    NO.6
    6,           //E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_75to100MHz    NO.7
    6,           //E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_75to75MHz    NO.8
    6,           //E_PNL_SUPPORTED_LPLL_LVDS_1ch_1P_50to75MHz    NO.0
    6,           //E_PNL_SUPPORTED_LPLL_LVDS_1ch_1P_37_5to50MHz    NO.1
    6,           //E_PNL_SUPPORTED_LPLL_LVDS_1ch_1P_37_5to37_5MHz    NO.2
    6,           //E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_200to300MHz    NO.9
    6,           //E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_150to200MHz    NO.10
    6,           //E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_150to150MHz    NO.11
    6,           //E_PNL_SUPPORTED_LPLL_LVDS_2ch_1P_125to200MHz    NO.3
    6,           //E_PNL_SUPPORTED_LPLL_LVDS_2ch_1P_75to125MHz    NO.4
    6,           //E_PNL_SUPPORTED_LPLL_LVDS_2ch_1P_75to75MHz    NO.5
    16,          //E_PNL_SUPPORTED_LPLL_VBY1_2ch_10bit_150to300MHz    NO.12
    16,          //E_PNL_SUPPORTED_LPLL_VBY1_2ch_10bit_150to150MHz    NO.13
    16,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_10bit_150to400MHz   NO.14
    8,           //E_PNL_SUPPORTED_LPLL_VBY1_4ch_10bit_75to150MHz    NO.15
    8,           //E_PNL_SUPPORTED_LPLL_VBY1_4ch_10bit_75to75MHz    NO.16
    16,          //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case1_300to400MHz     NO.17
    8,           //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case1_150to300MHz    NO.18
    8,           //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case1_75to150MHz    NO.19
    8,           //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case1_75to75MHz    NO.20
    6,           //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case2_300to520MHz     NO.21
    6,           //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case2_200to300MHz    NO.22
    6,           //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case2_130to200MHz    NO.23
    6,           //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case2_75to130MHz    NO.24
    6,           //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case2_75to75MHz    NO.25
    32,          //E_PNL_SUPPORTED_LPLL_VBY1_2ch_8bit_200to300MHz    NO.26
    16,          //E_PNL_SUPPORTED_LPLL_VBY1_2ch_8bit_150to200MHz    NO.27
    16,          //E_PNL_SUPPORTED_LPLL_VBY1_2ch_8bit_150to150MHz    NO.28
    32,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_200to400MHz    NO.29
    16,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_100to200MHz    NO.30
    8,           //E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_75to100MHz    NO.31
    8,           //E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_75to75MHz    NO.32
    32,          //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case1_200to400MHz    NO.33
    16,          //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case1_100to200MHz    NO.34
    8,           //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case1_75to100MHz    NO.35
    8,           //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case1_75to75MHz    NO.36
    6,           //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case2_200to520MHz    NO.37
    6,           //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case2_120to200MHz    NO.38
    6,           //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case2_75to120MHz    NO.39
    6,           //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case2_75to75MHz    NO.40
    16,           //E_PNL_SUPPORTED_LPLL_eDP_High_bit_rate _270to270MHz    NO.41
    16,           //E_PNL_SUPPORTED_LPLL_eDP_High_bit_rate _270to270MHz    NO.42
    16,           //E_PNL_SUPPORTED_LPLL_eDP_High_bit_rate _270to270MHz    NO.43
    16,           //E_PNL_SUPPORTED_LPLL_eDP_High_bit_rate _270to270MHz    NO.44
    16,           //E_PNL_SUPPORTED_LPLL_eDP_High_bit_rate _270to270MHz    NO.45
    16,           //E_PNL_SUPPORTED_LPLL_eDP_High_bit_rate _270to270MHz    NO.46
    8,           //E_PNL_SUPPORTED_LPLL_eDP_Reduce_bit_rate _162to162MHz    NO.47
    8,           //E_PNL_SUPPORTED_LPLL_eDP_Reduce_bit_rate _162to162MHz    NO.48
    8,           //E_PNL_SUPPORTED_LPLL_eDP_Reduce_bit_rate _162to162MHz    NO.49
    8,           //E_PNL_SUPPORTED_LPLL_eDP_Reduce_bit_rate _162to162MHz    NO.50
    8,           //E_PNL_SUPPORTED_LPLL_eDP_Reduce_bit_rate _162to162MHz    NO.51
    8,           //E_PNL_SUPPORTED_LPLL_eDP_Reduce_bit_rate _162to162MHz    NO.52
};

BYTE code u8LoopDiv[E_PNL_SUPPORTED_LPLL_MAX]=
{
    8,           //E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_100to150MHz    NO.6
    16,          //E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_75to100MHz    NO.7
    16,          //E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_75to75MHz    NO.8
    16,          //E_PNL_SUPPORTED_LPLL_LVDS_1ch_1P_50to75MHz    NO.0
    16,          //E_PNL_SUPPORTED_LPLL_LVDS_1ch_1P_37_5to50MHz    NO.1
    16,          //E_PNL_SUPPORTED_LPLL_LVDS_1ch_1P_37_5to37_5MHz    NO.2
    4,           //E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_200to300MHz    NO.9
    8,           //E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_150to200MHz    NO.10
    8,           //E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_150to150MHz    NO.11
    8,           //E_PNL_SUPPORTED_LPLL_LVDS_2ch_1P_125to200MHz    NO.3
    8,           //E_PNL_SUPPORTED_LPLL_LVDS_2ch_1P_75to125MHz    NO.4
    8,           //E_PNL_SUPPORTED_LPLL_LVDS_2ch_1P_75to75MHz    NO.5
    10,          //E_PNL_SUPPORTED_LPLL_VBY1_2ch_10bit_150to300MHz    NO.12
    10,          //E_PNL_SUPPORTED_LPLL_VBY1_2ch_10bit_150to150MHz    NO.13
    10,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_10bit_150to400MHz    NO.14
    10,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_10bit_75to150MHz     NO.15
    10,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_10bit_75to75MHz     NO.16
    5,           //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case1_300to400MHz    NO.17
    5,           //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case1_150to300MHz    NO.18
    10,          //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case1_75to150MHz NO.19
    10,          //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case1_75to75MHz    NO.20
    4,           //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case2_300to520MHz    NO.21
    4,           //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case2_200to300MHz    NO.22
    8,           //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case2_130to200MHz    NO.23
    16,          //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case2_75to130MHz NO.24
    16,          //E_PNL_SUPPORTED_LPLL_VBY1_8ch_10bit_case2_75to75MHz    NO.25
    15,          //E_PNL_SUPPORTED_LPLL_VBY1_2ch_8bit_200to300MHz    NO.26
    15,          //E_PNL_SUPPORTED_LPLL_VBY1_2ch_8bit_150to200MHz    NO.27
    15,          //E_PNL_SUPPORTED_LPLL_VBY1_2ch_8bit_150to150MHz    NO.28
    15,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_200to400MHz    NO.29
    15,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_100to200MHz    NO.30
    15,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_75to100MHz    NO.31
    15,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_75to75MHz    NO.32
    15,          //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case1_200to400MHz    NO.33
    15,          //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case1_100to200MHz    NO.34
    15,          //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case1_75to100MHz    NO.35
    15,          //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case1_75to75MHz    NO.36
    4,           //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case2_200to520MHz    NO.37
    8,           //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case2_120to200MHz    NO.38
    16,          //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case2_75to120MHz    NO.39
    16,          //E_PNL_SUPPORTED_LPLL_VBY1_8ch_8bit_case2_75to75MHz    NO.40
    10,           //E_PNL_SUPPORTED_LPLL_eDP_High_bit_rate _270to270MHz    NO.41
    10,           //E_PNL_SUPPORTED_LPLL_eDP_High_bit_rate _270to270MHz    NO.42
    10,           //E_PNL_SUPPORTED_LPLL_eDP_High_bit_rate _270to270MHz    NO.43
    10,           //E_PNL_SUPPORTED_LPLL_eDP_High_bit_rate _270to270MHz    NO.44
    10,           //E_PNL_SUPPORTED_LPLL_eDP_High_bit_rate _270to270MHz    NO.45
    10,           //E_PNL_SUPPORTED_LPLL_eDP_High_bit_rate _270to270MHz    NO.46
    10,           //E_PNL_SUPPORTED_LPLL_eDP_Reduce_bit_rate _162to162MHz    NO.47
    10,           //E_PNL_SUPPORTED_LPLL_eDP_Reduce_bit_rate _162to162MHz    NO.48
    10,           //E_PNL_SUPPORTED_LPLL_eDP_Reduce_bit_rate _162to162MHz    NO.49
    10,           //E_PNL_SUPPORTED_LPLL_eDP_Reduce_bit_rate _162to162MHz    NO.50
    10,           //E_PNL_SUPPORTED_LPLL_eDP_Reduce_bit_rate _162to162MHz    NO.51
    10,           //E_PNL_SUPPORTED_LPLL_eDP_Reduce_bit_rate _162to162MHz    NO.52
};

BYTE u8EXT_LoopGain[E_PNL_SUPPORTED_LPLL_MAX]=
{
    0,           //E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_100to150MHz    NO.6
    0,          //E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_75to100MHz    NO.7
    0,          //E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_75to75MHz    NO.8
    0,          //E_PNL_SUPPORTED_LPLL_LVDS_1ch_1P_50to75MHz    NO.0
    0,          //E_PNL_SUPPORTED_LPLL_LVDS_1ch_1P_37_5to50MHz    NO.1
    0,          //E_PNL_SUPPORTED_LPLL_LVDS_1ch_1P_37_5to37_5MHz    NO.2
    0,           //E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_200to300MHz    NO.9
    0,           //E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_150to200MHz    NO.10
    0,           //E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_150to150MHz    NO.11
    0,           //E_PNL_SUPPORTED_LPLL_LVDS_2ch_1P_125to200MHz    NO.3
    0,           //E_PNL_SUPPORTED_LPLL_LVDS_2ch_1P_75to125MHz    NO.4
    0,           //E_PNL_SUPPORTED_LPLL_LVDS_2ch_1P_75to75MHz    NO.5
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_2ch_10bit_150to300MHz    NO.12
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_2ch_10bit_150to150MHz    NO.13
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_10bit_150to400MHz    NO.14
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_10bit_75to150MHz     NO.15
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_10bit_75to75MHz     NO.16
    6,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_10bit_case1_300to400MHz    NO.17
    6,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_10bit_case1_150to300MHz    NO.18
    6,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_10bit_case1_75to150MHz    NO.19
    6,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_10bit_case1_75to75MHz    NO.20
    16,          //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_10bit_case2_300to520MHz    NO.21
    8,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_10bit_case2_200to300MHz    NO.22
    8,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_10bit_case2_130to200MHz    NO.23
    8,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_10bit_case2_75to130MHz    NO.24
    8,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_10bit_case2_75to75MHz    NO.25
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_2ch_8bit_200to300MHz    NO.26
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_2ch_8bit_150to200MHz    NO.27
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_2ch_8bit_150to150MHz    NO.28
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_200to400MHz    NO.29
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_100to200MHz    NO.30
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_75to100MHz    NO.31
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_75to75MHz    NO.32
    6,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_8bit_case1_200to400MHz    NO.33
    6,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_8bit_case1_100to200MHz    NO.34
    6,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_8bit_case1_75to100MHz    NO.35
    6,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_8bit_case1_75to75MHz    NO.36
    32,          //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_8bit_case2_200to520MHz    NO.37
    16,          //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_8bit_case2_120to200MHz    NO.38
    8,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_8bit_case2_75to120MHz    NO.39
    8,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_8bit_case2_75to75MHz    NO.40
    6,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate _150to400MHz    NO.41
    6,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate _75to150MHz    NO.42
    6,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate _37_5to75MHz    NO.43
    12,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate _25to37_5MHz    NO.44
    6,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate _12_5to25MHz    NO.45
    6,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate _12_5to12_5MHz    NO.46
    6,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate _150to400MHz    NO.47
    6,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate _75to150MHz    NO.48
    6,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate _37_5to75MHz    NO.49
    12,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate _25to37_5MHz    NO.50
    6,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate _12_5to25MHz    NO.51
    6,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate _12_5to12_5MHz    NO.52
};

BYTE u8EXT_LoopDiv[E_PNL_SUPPORTED_LPLL_MAX]=
{
    0,           //E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_100to150MHz    NO.6
    0,          //E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_75to100MHz    NO.7
    0,          //E_PNL_SUPPORTED_LPLL_HS_LVDS_1ch_75to75MHz    NO.8
    0,          //E_PNL_SUPPORTED_LPLL_LVDS_1ch_1P_50to75MHz    NO.0
    0,          //E_PNL_SUPPORTED_LPLL_LVDS_1ch_1P_37_5to50MHz    NO.1
    0,          //E_PNL_SUPPORTED_LPLL_LVDS_1ch_1P_37_5to37_5MHz    NO.2
    0,           //E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_200to300MHz    NO.9
    0,           //E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_150to200MHz    NO.10
    0,           //E_PNL_SUPPORTED_LPLL_HS_LVDS_2ch_150to150MHz    NO.11
    0,           //E_PNL_SUPPORTED_LPLL_LVDS_2ch_1P_125to200MHz    NO.3
    0,           //E_PNL_SUPPORTED_LPLL_LVDS_2ch_1P_75to125MHz    NO.4
    0,           //E_PNL_SUPPORTED_LPLL_LVDS_2ch_1P_75to75MHz    NO.5
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_2ch_10bit_150to300MHz    NO.12
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_2ch_10bit_150to150MHz    NO.13
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_10bit_150to400MHz    NO.14
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_10bit_75to150MHz     NO.15
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_10bit_75to75MHz     NO.16
    4,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_10bit_case1_300to400MHz    NO.17
    4,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_10bit_case1_150to300MHz    NO.18
    8,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_10bit_case1_75to150MHz    NO.19
    8,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_10bit_case1_75to75MHz    NO.20
    5,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_10bit_case2_300to520MHz    NO.21
    5,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_10bit_case2_200to300MHz    NO.22
    5,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_10bit_case2_130to200MHz    NO.23
    10,          //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_10bit_case2_75to130MHz    NO.24
    10,          //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_10bit_case2_75to75MHz    NO.25
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_2ch_8bit_200to300MHz    NO.26
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_2ch_8bit_150to200MHz    NO.27
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_2ch_8bit_150to150MHz    NO.28
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_200to400MHz    NO.29
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_100to200MHz    NO.30
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_75to100MHz    NO.31
    0,          //E_PNL_SUPPORTED_LPLL_VBY1_4ch_8bit_75to75MHz    NO.32
    4,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_8bit_case1_200to400MHz    NO.33
    8,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_8bit_case1_100to200MHz    NO.34
    16,          //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_8bit_case1_75to100MHz    NO.35
    16,          //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_8bit_case1_75to75MHz    NO.36
    15,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_8bit_case2_200to520MHz    NO.37
    15,           //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_8bit_case2_120to200MHz    NO.38
    15,          //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_8bit_case2_75to120MHz    NO.39
    15,          //E_PNL_SUPPORTED_LPLL_EXT_VBY1_8ch_8bit_case2_75to75MHz    NO.40
    4,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate _150to400MHz    NO.41
    8,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate _75to150MHz    NO.42
    16,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate _37_5to75MHz    NO.43
    48,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate _25to37_5MHz    NO.44
    48,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate _12_5to25MHz    NO.45
    48,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_High_bit_rate _12_5to12_5MHz    NO.46
    4,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate _150to400MHz    NO.47
    8,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate _75to150MHz    NO.48
    16,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate _37_5to75MHz    NO.49
    48,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate _25to37_5MHz    NO.50
    48,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate _12_5to25MHz    NO.51
    48,           //E_PNL_SUPPORTED_LPLL_EXT_eDP_Reduce_bit_rate _12_5to12_5MHz    NO.52
};

#endif
