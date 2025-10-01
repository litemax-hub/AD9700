// $Change: 1185839 $
//*************************************************************************************************
//<MStar Software>
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2012-2014 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting
// therefrom.
//*************************************************************************************************

#ifndef DPRXCOMMON_H
#define DPRXCOMMON_H

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "Global.h"
#include "types.h"
#include "HdcpCommon.h"
#include "HdcpHandler.h"

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#ifdef TSUMR2_FPGA
#define FPGA_Verification					1
#define ASIC_Verification					0
#define FPGA_440                            1
#define FPGA_VU19P                          0
#define FPGA_VU9P                           0

#else
#define FPGA_Verification					0
#define ASIC_Verification					1
#define FPGA_440                            0
#define FPGA_VU19P                          0
#define FPGA_VU9P                           0
#endif
//******MT9701********
#define FunctionMuxMode_HW				    1
#define DellMode_EN						    0
//********************

#define DP_DEBUG_MESSAGE					1

#define CTS_SINKSTATUS						0

#define XDATA_PROGRAMDPCD					1
#define DPRX_SINK_COUNT_DEFAULT			    1

#define DP_NORMAL_PORT0						BIT2
#define DP_NORMAL_PORT1						BIT3
#define DP_NORMAL_PORT2						BIT4
#define DP_TYPEC_PORT0      				BIT5

#define GET_DPRx_FUNCTION_ENABLE_PORT(a)	(Bool)((ucDPFunctionEnableIndex & (BIT0 << (a)))? TRUE : FALSE)
#define GET_DPRx_FUNCTION_ENABLE_INDEX()	(ucDPFunctionEnableIndex)

#define DPRX_M_RANGE_NEW_MODE               1
#define DPRX_M_RANGE_NEW_MODE_VALUE         6 //Mrange = M*0.6%
#define DPRX_M_DETECT_RANGE_PIXEL_CLK		600 // Unit: MHz
#define DPRX_AUIDO_UNMUTE_NORMAL			1 // 1: Unmute normal, 0: Can unmute fast (8 pin sound for CTS 5.4.4.2)
#define DPRx_ASTRO_INTERLACE_PATCH			1

//*****************************************************************************
//		DP AUX
//*****************************************************************************
#define AUX_CMD_I2C_W_MOT0		0x00
#define AUX_CMD_I2C_R_MOT0		0x01
#define AUX_CMD_I2C_W			0x04
#define AUX_CMD_I2C_R			0x05
#define AUX_CMD_NATIVE_W		0x08
#define AUX_CMD_NATIVE_R		0x09
#define AUX_EDID_SLAVE_ADDR		0x50
#define AUX_EDID_SEGMENT_ADDR	0x30
#define AUX_MCCS_SLAVE_ADDR		0x37

//*****************************************************************************
//		DP RX
//*****************************************************************************
#define DPRX_SUPPORT_TP3					1
#define DPRX_SUPPORT_TP4					0
#define DPRX_SUPPORT_DPCD2200				1

#define DPRX_DP14_HDR						ENABLE_HDR
#define DPRX_DP14_YUV420    		    	ENABLE_DP_YUV420
#define DPRX_SUPPORT_VSCSDP					(1 & (DPRX_DP14_HDR | DPRX_DP14_YUV420))

#define DP_REG_OFFSET_AUX_PM0(a)			((a) * 0x300)
#define DP_REG_OFFSET_AUX_PM1(a)			((a) * 0x300)
#define DP_REG_OFFSET_AUX_PM2(a)			((a) * 0x300)
#define DP_REG_OFFSET_AUX_PM3(a)			((a) * 0x100)
#define DP_REG_OFFSET_AUX_PM4(a)			((a) * 0x100)
#define DP_REG_OFFSET_AUX_PM_TOP(a)			((a) * 0x000)
#define DP_REG_OFFSET_PHY0(a)	    		((a) * 0x400)
#define DP_REG_OFFSET_DPCD0(a)			    ((a) * 0x000)
#define DP_REG_OFFSET_DPCD1(a)              ((a) * 0x000)
#define DP_REG_OFFSET_DPCD2(a)              ((a) * 0x000)
#define DP_REG_OFFSET_DPCD_HDCP22_0(a)      ((a) * 0x000)
#define DP_REG_OFFSET_DPCD_HDCP22_1(a)      ((a) * 0x000)
#define DP_REG_OFFSET_DPCD_HDCP22_2(a)      ((a) * 0x000)
#define DP_REG_OFFSET_DPCD_HDCP22_3(a)      ((a) * 0x000)
#define DP_REG_OFFSET_DPCD_HDCP22_4(a)      ((a) * 0x000)
#define DP_REG_OFFSET_DECODER(a)            ((a) * 0x000)
#define DP_REG_OFFSET_DECODER1(a)           ((a) * 0x000)
#define DP_REG_OFFSET_TRANS_CTRL(a)         ((a) * 0x000)
#define DP_REG_OFFSET_RECEIVER(a)           ((a) * 0x000)
#define DP_REG_OFFSET_SDP(a)                ((a) * 0x000)
#define DP_REG_OFFSET_ALOCK(a)              ((a) * 0x000)
#define DP_REG_OFFSET_AUDIO_CTRL(a)         ((a) * 0x000)
#define DP_REG_OFFSET_RECEIVER_FEC(a)       ((a) * 0x000)
#define DP_REG_OFFSET040(a)					((a) * 0x040)

#define DP_GUID_SIZE						0x10
#define DPRX_AUX_REPLY_DELAY				0x2D2 // Normal case, unit is 1/xtal, it about 60us
#define DPRX_STATE_STABLE_DEBOUNCE_TIME		0x20

#define DPRx_MCCS_SUPPORT					1
#define DP_XDATA_PROGRAMMABLE_DPCD_MCCS		0 // PM mode mccs

#define DPRX_OFFLINE_TO_ONLINE_KEEP_LOCK    1
#define DPRX_AUDIO_NOVIDEO          0
#define ENABLE_DP_DCOFF_HPD_HIGH            0

//*****************************************************************************
//      Fast Training
//*****************************************************************************
#define DP_RX_FT_TIMER_A_WAIT_SQ_SIGNAL 	0x2
#define DP_RX_TRAINING_DEBOUNCE_TIME        250

//*****************************************************************************
//      Fake Training
//*****************************************************************************
#define Faketrain_swing                 DP_SWING2
#define Faketrain_pre                   DP_PREEMPHASIS0

//*****************************************************************************
//		TypeC RX
//*****************************************************************************
#define DP_TYPE_C_EN  					ENABLE_USB_TYPEC
#define DP_TYPE_C_PORT_NUM	 			1

//*****************************************************************************
//		Decoder
//*****************************************************************************
#define DP_SDP_PACKET_MAX_DATA_LENGTH		32
#define DP_SDP_PACKET_MAX_HEAD_LENGTH		4
#define DP_DSC_SDP_PACKET_MAX_DATA_LENGTH	128

#define DP_VESA_ADAPTIVE_SYNC_SDP_SUPPORT   0

#define DPRX_PACKET_UPDATED_BY_VSYNC        COMBO_TMDS_PACKET_UPDATED_BY_VSYNC

#define DPRX_AUDIO_TIME_STAMP_LOST_CNT              0
#define DPRX_AUDIO_STREAM_LOST_CNT                  0
#define DPRX_EXT_LOST_CNT                           0
#define DPRX_ACM_LOST_CNT                           1
#define DPRX_ISRC_LOST_CNT                          1
#define DPRX_VSC_LOST_CNT                           0
#define DPRX_PPS_LOST_CNT                           0
#define DPRX_VSP_LOST_CNT                           1
#define DPRX_AVI_LOST_CNT                           0
#define DPRX_SPD_LOST_CNT                           30
#define DPRX_AUI_LOST_CNT                           0
#define DPRX_MPEG_LOST_CNT                          0
#define DPRX_NTSC_LOST_CNT                          0
#define DPRX_VSC_EXT_VESA_LOST_CNT                  0
#define DPRX_VSC_EXT_CEA_LOST_CNT                   0
#define DPRX_DRM_LOST_CNT                           0
#define DPRX_ADS_LOST_CNT                           0



//*****************************************************************************
//		DPCD
//*****************************************************************************
#define DPCD_00000				0x00000
#define DPCD_00001				0x00001
#define DPCD_00002				0x00002
#define DPCD_00003				0x00003
#define DPCD_00004				0x00004
#define DPCD_00005				0x00005
#define DPCD_00006				0x00006
#define DPCD_00007				0x00007
#define DPCD_00008				0x00008
#define DPCD_00009				0x00009
#define DPCD_0000A				0x0000A
#define DPCD_0000B				0x0000B
#define DPCD_0000C				0x0000C
#define DPCD_0000D				0x0000D
#define DPCD_0000E				0x0000E
#define DPCD_0000F				0x0000F
#define DPCD_00010				0x00010
#define DPCD_00011				0x00011
#define DPCD_00012				0x00012
#define DPCD_00013				0x00013
#define DPCD_00014				0x00014
#define DPCD_00015				0x00015
#define DPCD_00016				0x00016
#define DPCD_00017				0x00017
#define DPCD_00018				0x00018
#define DPCD_00019				0x00019
#define DPCD_0001A				0x0001A
#define DPCD_0001B				0x0001B
#define DPCD_0001C				0x0001C
#define DPCD_0001D				0x0001D
#define DPCD_0001E				0x0001E
#define DPCD_0001F				0x0001F
#define DPCD_00020				0x00020
#define DPCD_00021				0x00021
#define DPCD_00022				0x00022
#define DPCD_00023				0x00023
#define DPCD_00024				0x00024
#define DPCD_00025				0x00025
#define DPCD_00026				0x00026
#define DPCD_00027				0x00027
#define DPCD_00028				0x00028
#define DPCD_00029				0x00029
#define DPCD_0002A				0x0002A
#define DPCD_0002B				0x0002B
#define DPCD_0002C				0x0002C
#define DPCD_0002D				0x0002D
#define DPCD_0002E				0x0002E
#define DPCD_0002F				0x0002F

#define DPCD_00030				0x00030
#define DPCD_00031				0x00031
#define DPCD_00032				0x00032
#define DPCD_00033				0x00033
#define DPCD_00034				0x00034
#define DPCD_00035				0x00035
#define DPCD_00036				0x00036
#define DPCD_00037				0x00037
#define DPCD_00038				0x00038
#define DPCD_00039				0x00039
#define DPCD_0003A				0x0003A
#define DPCD_0003B				0x0003B
#define DPCD_0003C				0x0003C
#define DPCD_0003D				0x0003D
#define DPCD_0003E				0x0003E
#define DPCD_0003F				0x0003F

#define DPCD_00040				0x00040
#define DPCD_00041				0x00041
#define DPCD_00042				0x00042
#define DPCD_00043				0x00043
#define DPCD_00044				0x00044
#define DPCD_00045				0x00045
#define DPCD_00046				0x00046
#define DPCD_00047				0x00047
#define DPCD_00048				0x00048
#define DPCD_00049				0x00049
#define DPCD_0004A				0x0004A
#define DPCD_0004B				0x0004B
#define DPCD_0004C				0x0004C
#define DPCD_0004D				0x0004D
#define DPCD_0004E				0x0004E
#define DPCD_0004F				0x0004F

#define DPCD_00060				0x00060
#define DPCD_00061				0x00061
#define DPCD_00062				0x00062
#define DPCD_00063				0x00063
#define DPCD_00064				0x00064
#define DPCD_00065				0x00065
#define DPCD_00066				0x00066
#define DPCD_00067				0x00067
#define DPCD_00068				0x00068
#define DPCD_00069				0x00069
#define DPCD_0006A				0x0006A
#define DPCD_0006B				0x0006B
#define DPCD_0006C				0x0006C
#define DPCD_0006D				0x0006D
#define DPCD_0006E				0x0006E
#define DPCD_0006F				0x0006F

#define DPCD_00080				0x00080
#define DPCD_00081				0x00081
#define DPCD_00082				0x00082
#define DPCD_00083				0x00083
#define DPCD_00084				0x00084
#define DPCD_00085				0x00085
#define DPCD_00086				0x00086
#define DPCD_00087				0x00087
#define DPCD_00088				0x00088
#define DPCD_00089				0x00089
#define DPCD_0008A				0x0008A
#define DPCD_0008B				0x0008B
#define DPCD_0008C				0x0008C
#define DPCD_0008D				0x0008D
#define DPCD_0008E				0x0008E
#define DPCD_0008F				0x0008F

#define DPCD_00090				0x00090
#define DPCD_00091				0x00091

#define DPCD_000A0				0x000A0
#define DPCD_000A1				0x000A1
#define DPCD_000A2				0x000A2
#define DPCD_000A3				0x000A3
#define DPCD_000A4				0x000A4
#define DPCD_000A5				0x000A5
#define DPCD_000A6				0x000A6
#define DPCD_000A7				0x000A7
#define DPCD_000A8				0x000A8
#define DPCD_000A9				0x000A9
#define DPCD_000AA				0x000AA
#define DPCD_000AB				0x000AB
#define DPCD_000AC				0x000AC
#define DPCD_000AD				0x000AD
#define DPCD_000AE				0x000AE

#define DPCD_000B0				0x000B0
#define DPCD_000B1				0x000B1
#define DPCD_000B2				0x000B2
#define DPCD_000B3				0x000B3
#define DPCD_000B4				0x000B4

#define DPCD_00100				0x00100
#define DPCD_00101				0x00101
#define DPCD_00102				0x00102
#define DPCD_00103				0x00103
#define DPCD_00104				0x00104
#define DPCD_00105				0x00105
#define DPCD_00106				0x00106
#define DPCD_00107				0x00107
#define DPCD_00108				0x00108
#define DPCD_00109				0x00109

#define DPCD_0010A				0x0010A
#define DPCD_0010B				0x0010B
#define DPCD_0010C				0x0010C
#define DPCD_0010D				0x0010D
#define DPCD_0010E				0x0010E
#define DPCD_0010F				0x0010F
#define DPCD_00110				0x00110
#define DPCD_00111				0x00111
#define DPCD_00112				0x00112
#define DPCD_00113				0x00113
#define DPCD_00114				0x00114
#define DPCD_00115				0x00115

#define DPCD_00117				0x00117
#define DPCD_00118				0x00118
#define DPCD_00119				0x00119

#define DPCD_00120				0x00120
#define DPCD_00121				0x00121
#define DPCD_00122				0x00122
#define DPCD_00123				0x00123
#define DPCD_00124				0x00124
#define DPCD_00125				0x00125

#define DPCD_00160				0x00160
#define DPCD_00161				0x00161

#define DPCD_001A0				0x001A0
#define DPCD_001A1				0x001A1

#define DPCD_001B0				0x001B0

#define DPCD_001C0				0x001C0
#define DPCD_001C1				0x001C1
#define DPCD_001C2				0x001C2
#define DPCD_00200				0x00200
#define DPCD_00201				0x00201
#define DPCD_00202				0x00202
#define DPCD_00203				0x00203
#define DPCD_00204				0x00204
#define DPCD_00205				0x00205
#define DPCD_00206				0x00206
#define DPCD_00207				0x00207

#define DPCD_0020C				0x0020C
#define DPCD_0020D				0x0020D
#define DPCD_0020E				0x0020E

#define DPCD_00210				0x00210
#define DPCD_00212				0x00212
#define DPCD_00214				0x00214
#define DPCD_00218				0x00218
#define DPCD_00219				0x00219
#define DPCD_00220				0x00220
#define DPCD_00221				0x00221
#define DPCD_00222				0x00222
#define DPCD_00223				0x00223
#define DPCD_00224				0x00224
#define DPCD_00225				0x00225
#define DPCD_00226				0x00226
#define DPCD_00227				0x00227
#define DPCD_00228				0x00228
#define DPCD_00229				0x00229
#define DPCD_0022A				0x0022A
#define DPCD_0022B				0x0022B
#define DPCD_0022C				0x0022C
#define DPCD_0022D				0x0022D
#define DPCD_0022E				0x0022E
#define DPCD_0022F				0x0022F
#define DPCD_00230				0x00230
#define DPCD_00231				0x00231
#define DPCD_00232				0x00232
#define DPCD_00233				0x00233
#define DPCD_00234				0x00234

#define DPCD_00240				0x00240
#define DPCD_00241				0x00241
#define DPCD_00242				0x00242
#define DPCD_00243				0x00243
#define DPCD_00244				0x00244
#define DPCD_00245				0x00245

#define DPCD_00248				0x00248
#define DPCD_00249				0x00249
#define DPCD_0024A				0x0024A
#define DPCD_0024B				0x0024B

#define DPCD_00250				0x00250
#define DPCD_00251				0x00251
#define DPCD_00252				0x00252
#define DPCD_00253				0x00253
#define DPCD_00254				0x00254
#define DPCD_00255				0x00255
#define DPCD_00256				0x00256
#define DPCD_00257				0x00257
#define DPCD_00258				0x00258
#define DPCD_00259				0x00259

#define DPCD_0025A				0x0025A
#define DPCD_0025B				0x0025B

#define DPCD_00260				0x00260
#define DPCD_00261				0x00261
#define DPCD_00262				0x00262

#define DPCD_00270				0x00270
#define DPCD_00271				0x00271
#define DPCD_00272				0x00272
#define DPCD_00273				0x00273
#define DPCD_00274				0x00274
#define DPCD_00275				0x00275
#define DPCD_00276				0x00276
#define DPCD_00277				0x00277
#define DPCD_00278				0x00278
#define DPCD_00279				0x00279
#define DPCD_0027A				0x0027A
#define DPCD_0027B				0x0027B
#define DPCD_0027C				0x0027C
#define DPCD_0027D				0x0027D
#define DPCD_0027E				0x0027E
#define DPCD_0027F				0x0027F
#define DPCD_00280				0x00280
#define DPCD_00281				0x00281
#define DPCD_00282				0x00282

#define DPCD_002C0				0x002C0
#define DPCD_00300				0x00300
#define DPCD_00301				0x00301
#define DPCD_00302				0x00302
#define DPCD_00303				0x00303
#define DPCD_00304				0x00304
#define DPCD_00305				0x00305
#define DPCD_00306				0x00306
#define DPCD_00307				0x00307
#define DPCD_00308				0x00308
#define DPCD_00309				0x00309
#define DPCD_0030A				0x0030A
#define DPCD_0030B				0x0030B
#define DPCD_00314				0x00314
#define DPCD_00320				0x00320
#define DPCD_00321				0x00321

#define DPCD_00340				0x00340
#define DPCD_00341				0x00341
#define DPCD_00342				0x00342
#define DPCD_00343				0x00343
#define DPCD_00344				0x00344
#define DPCD_00345				0x00345
#define DPCD_00346				0x00346
#define DPCD_00347				0x00347
#define DPCD_00348				0x00348
#define DPCD_00349				0x00349
#define DPCD_0034A				0x0034A
#define DPCD_0034B				0x0034B
#define DPCD_0034C				0x0034C
#define DPCD_0034D				0x0034D
#define DPCD_0034E				0x0034E
#define DPCD_0034F				0x0034F
#define DPCD_00350				0x00350
#define DPCD_00351				0x00351
#define DPCD_00352				0x00352
#define DPCD_00353				0x00353
#define DPCD_00354				0x00354
#define DPCD_00355				0x00355
#define DPCD_00356				0x00356
#define DPCD_00357				0x00357
#define DPCD_00358				0x00358
#define DPCD_00359				0x00359

#define DPCD_00381				0x00381
#define DPCD_00382				0x00382
#define DPCD_00383				0x00383
#define DPCD_00384				0x00384

#define DPCD_00400				0x00400
#define DPCD_00401				0x00401
#define DPCD_00402				0x00402
#define DPCD_00403				0x00403
#define DPCD_00404				0x00404
#define DPCD_00405				0x00405
#define DPCD_00406				0x00406
#define DPCD_00407				0x00407
#define DPCD_00408				0x00408
#define DPCD_00409				0x00409
#define DPCD_0040A				0x0040A
#define DPCD_0040B				0x0040B

#define DPCD_0040F				0x0040F
#define DPCD_00410				0x00410
#define DPCD_00411				0x00411
#define DPCD_00412				0x00412
#define DPCD_00413				0x00413
#define DPCD_00414				0x00414
#define DPCD_00415				0x00415
#define DPCD_00416				0x00416
#define DPCD_00417				0x00417
#define DPCD_00418				0x00418
#define DPCD_00419				0x00419
#define DPCD_0041A				0x0041A
#define DPCD_0041B				0x0041B
#define DPCD_0041C				0x0041C
#define DPCD_0041D				0x0041D
#define DPCD_0041E				0x0041E
#define DPCD_0041F				0x0041F
#define DPCD_00420				0x00420
#define DPCD_00421				0x00421
#define DPCD_00422				0x00422
#define DPCD_00423				0x00423
#define DPCD_00424				0x00424
#define DPCD_00425				0x00425
#define DPCD_00426				0x00426
#define DPCD_00427				0x00427
#define DPCD_00428				0x00428
#define DPCD_00429				0x00429
#define DPCD_0042A				0x0042A
#define DPCD_0042B				0x0042B
#define DPCD_0042C				0x0042C
#define DPCD_0042D				0x0042D
#define DPCD_0042E				0x0042E
#define DPCD_0042F				0x0042F

#define DPCD_00430				0x00430
#define DPCD_00431				0x00431
#define DPCD_00432				0x00432
#define DPCD_00433				0x00433
#define DPCD_00434				0x00434
#define DPCD_00435				0x00435
#define DPCD_00436				0x00436
#define DPCD_00437				0x00437
#define DPCD_00438				0x00438
#define DPCD_00439				0x00439
#define DPCD_0043A				0x0043A
#define DPCD_0043B				0x0043B
#define DPCD_0043C				0x0043C

#define DPCD_0043E				0x0043E

#define DPCD_00440				0x00440
#define DPCD_00441				0x00441

#define DPCD_00500				0x00500
#define DPCD_00501				0x00501
#define DPCD_00502				0x00502
#define DPCD_00503				0x00503
#define DPCD_00504				0x00504
#define DPCD_00505				0x00505
#define DPCD_00506				0x00506
#define DPCD_00507				0x00507
#define DPCD_00508				0x00508
#define DPCD_00509				0x00509
#define DPCD_0050A				0x0050A
#define DPCD_0050B				0x0050B

#define DPCD_00600				0x00600
#define DPCD_00700				0x00700

#define DPCD_01000				0x01000
#define DPCD_01200				0x01200
#define DPCD_01400				0x01400
#define DPCD_01600				0x01600

#define DPCD_02002				0x02002
#define DPCD_02003				0x02003
#define DPCD_0200C				0x0200C
#define DPCD_0200D				0x0200D
#define DPCD_0200E				0x0200E
#define DPCD_0200F				0x0200F

#define DPCD_02200				0x02200
#define DPCD_02201				0x02201
#define DPCD_02202				0x02202
#define DPCD_02203				0x02203
#define DPCD_02204				0x02204
#define DPCD_02205				0x02205
#define DPCD_02206				0x02206
#define DPCD_02207				0x02207
#define DPCD_02208				0x02208
#define DPCD_02209				0x02209
#define DPCD_0220A				0x0220A
#define DPCD_0220B				0x0220B
#define DPCD_0220C				0x0220C
#define DPCD_0220D				0x0220D
#define DPCD_0220E				0x0220E
#define DPCD_0220F				0x0220F
#define DPCD_02210				0x02210
#define DPCD_02211				0x02211
#define DPCD_02212				0x02212
#define DPCD_02213				0x02213
#define DPCD_02214				0x02214
#define DPCD_02215				0x02215
#define DPCD_02216				0x02216
#define DPCD_02217 				0x02217

#define DPCD_02230				0x02230
#define DPCD_02231				0x02231
#define DPCD_02232				0x02232
#define DPCD_02233				0x02233
#define DPCD_02234				0x02234
#define DPCD_02235				0x02235
#define DPCD_02236				0x02236
#define DPCD_02237				0x02237
#define DPCD_02238				0x02238
#define DPCD_02239				0x02239
#define DPCD_0223A				0x0223A
#define DPCD_0223B				0x0223B
#define DPCD_0223C				0x0223C
#define DPCD_0223D				0x0223D
#define DPCD_0223E				0x0223E
#define DPCD_0223F				0x0223F
#define DPCD_02240				0x02240
#define DPCD_02241				0x02241
#define DPCD_02242				0x02242
#define DPCD_02243				0x02243
#define DPCD_02244				0x02244
#define DPCD_02245				0x02245
#define DPCD_02246				0x02246
#define DPCD_02247				0x02247
#define DPCD_02248				0x02248
#define DPCD_02249				0x02249
#define DPCD_0224A				0x0224A
#define DPCD_0224B				0x0224B
#define DPCD_0224C				0x0224C
#define DPCD_0224D				0x0224D
#define DPCD_0224E				0x0224E
#define DPCD_0224F				0x0224F
#define DPCD_02250				0x02250
#define DPCD_02251				0x02251
#define DPCD_02260				0x02260
#define DPCD_02261				0x02261
#define DPCD_02262				0x02262
#define DPCD_02263				0x02263
#define DPCD_02264				0x02264
#define DPCD_02265				0x02265
#define DPCD_02266				0x02266
#define DPCD_02267				0x02267
#define DPCD_02268				0x02268
#define DPCD_02269				0x02269
#define DPCD_0226A				0x0226A
#define DPCD_0226B				0x0226B
#define DPCD_0226C				0x0226C
#define DPCD_0226D				0x0226D
#define DPCD_0226E				0x0226E
#define DPCD_0226F				0x0226F
#define DPCD_02270				0x02270
#define DPCD_02271				0x02271
#define DPCD_02272				0x02272
#define DPCD_02273				0x02273

#define DPCD_02300				0x02300
#define DPCD_02301				0x02301
#define DPCD_02302				0x02302
#define DPCD_02303				0x02303
#define DPCD_02304				0x02304
#define DPCD_02305				0x02305
#define DPCD_02306				0x02306
#define DPCD_02307				0x02307
#define DPCD_02308				0x02308
#define DPCD_02309				0x02309
#define DPCD_0230A				0x0230A
#define DPCD_0230B				0x0230B
#define DPCD_0230C				0x0230C
#define DPCD_0230D				0x0230D
#define DPCD_0230E				0x0230E
#define DPCD_0230F				0x0230F
#define DPCD_02310				0x02310
#define DPCD_02311				0x02311
#define DPCD_02312				0x02312
#define DPCD_02313				0x02313
#define DPCD_02314				0x02314
#define DPCD_02315				0x02315
#define DPCD_02316				0x02316

#define DPCD_03000				0x03000
#define DPCD_03001				0x03001

#define DPCD_03030				0x03030
#define DPCD_03031				0x03031
#define DPCD_03032				0x03032
#define DPCD_03033				0x03033
#define DPCD_03034				0x03034

#define DPCD_03050				0x03050
#define DPCD_03051				0x03051
#define DPCD_03052				0x03052
#define DPCD_03053				0x03053
#define DPCD_03054				0x03054
#define DPCD_03055				0x03055
#define DPCD_03056				0x03056
#define DPCD_03057				0x03057
#define DPCD_03058				0x03058
#define DPCD_03059				0x03059

#define DPCD_68000				0x68000
#define DPCD_68001				0x68001
#define DPCD_68002				0x68002
#define DPCD_68003				0x68003
#define DPCD_68004				0x68004
#define DPCD_68005				0x68005
#define DPCD_68006				0x68006
#define DPCD_68007				0x68007
#define DPCD_68008				0x68008
#define DPCD_68009				0x68009
#define DPCD_6800A				0x6800A
#define DPCD_6800B				0x6800B
#define DPCD_6800C				0x6800C
#define DPCD_6800D				0x6800D
#define DPCD_6800E				0x6800E
#define DPCD_6800F				0x6800F
#define DPCD_68010				0x68010
#define DPCD_68011				0x68011
#define DPCD_68012				0x68012
#define DPCD_68013				0x68013
#define DPCD_68014				0x68014
#define DPCD_68015				0x68015
#define DPCD_68016				0x68016
#define DPCD_68017				0x68017
#define DPCD_68018				0x68018
#define DPCD_68019				0x68019
#define DPCD_6801A				0x6801A
#define DPCD_6801B				0x6801B
#define DPCD_6801C				0x6801C
#define DPCD_6801D				0x6801D
#define DPCD_6801E				0x6801E
#define DPCD_6801F				0x6801F
#define DPCD_68020				0x68020
#define DPCD_68021				0x68021
#define DPCD_68022				0x68022
#define DPCD_68023				0x68023
#define DPCD_68024				0x68024
#define DPCD_68025				0x68025
#define DPCD_68026				0x68026
#define DPCD_68027				0x68027
#define DPCD_68028				0x68028
#define DPCD_68029				0x68029
#define DPCD_6802A				0x6802A
#define DPCD_6802B				0x6802B

#define DPCD_E0000				0xE0000
#define DPCD_E0001				0xE0001
#define DPCD_E0002				0xE0002
#define DPCD_E0003				0xE0003
#define DPCD_E0004				0xE0004
#define DPCD_E0005				0xE0005
#define DPCD_E0006				0xE0006
#define DPCD_E0007				0xE0007
#define DPCD_E0008				0xE0008
#define DPCD_E0009				0xE0009
#define DPCD_E000A				0xE000A
#define DPCD_E000B				0xE000B

#define DPCD_E000D				0xE000D

#define DPCD_F0000				0xF0000
#define DPCD_F0001				0xF0001
#define DPCD_F0002				0xF0002
#define DPCD_F0003				0xF0003
#define DPCD_F0004				0xF0004

#define DPCD_FFF00				0xFFF00
#define DPCD_FFF01				0xFFF01

#define DPCD_FFFFC				0xFFFFC
#define DPCD_FFFFD				0xFFFFD
#define DPCD_FFFFE				0xFFFFE
#define DPCD_FFFFF				0xFFFFF


//*****************************************************************************
//		HDCP
//*****************************************************************************
#define DPRX_HDCP14_Repeater_ENABLE		(0 & ENABLE_SECU_R2 & ENABLE_DP_OUTPUT)
#define DPRX_HDCP14_LONGHPD_WDT			650

#define DPRX_HDCP2_ENABLE				ENABLE_HDCP22
#define DPRx_HDCP2_ISR_MODE				(DPRX_HDCP2_ENABLE & DEF_HDCP2RX_ISR_MODE)

#define DP_EDID_SIZE_1024				1024
#define DP_EDID_SIZE_512				512
#define DP_EDID_MAX_SIZE                DP_EDID_SIZE_512

#define COMBO_HDCP_KEY_SIZE				284
#define COMBO_HDCP_BKSV_SIZE			5

#define DP_HDCP_ADDR_Bksv				0x68000
#define DP_HDCP_ADDR_R0_				0x68005 // R0'
#define DP_HDCP_ADDR_Aksv				0x68007
#define DP_HDCP_ADDR_An					0x6800C
#define DP_HDCP_ADDR_V_H0				0x68014	// V'H0
#define DP_HDCP_ADDR_V_H1				0x68018 // V'H1
#define DP_HDCP_ADDR_V_H2				0x6801C // V'H2
#define DP_HDCP_ADDR_V_H3				0x68020 // V'H3
#define DP_HDCP_ADDR_V_H4				0x68024 // V'H4
#define DP_HDCP_ADDR_Bcaps				0x68028
#define DP_HDCP_ADDR_Bstatus			0x68029
#define DP_HDCP_ADDR_Binfo				0x6802A
#define DP_HDCP_ADDR_KSV_FIFO			0x6802C
#define DP_HDCP_ADDR_Ainfo				0x6803B

#define DP_HDCP2_DPCD_HprimeReadDone	0x692DF
#define DP_HDCP2_DPCD_PairingReadDone	0x692EF
#define DP_HDCP2_DPCD_Rxinfo			0x69330

#define DPRX_HDCP_Step1_IntegrityFail_Timer	    160 // Unit:ms
#define DPRX_HDCP_Step2_LoseLock_Timer	        160
#define DPRX_HDCP_Step3_HPDLong_Timer	        300

#define diff(a, b)				(((a) > (b))? ((a) - (b)) : ((b) - (a)))
#define ABS_MINUS_DP(a, b)		(((a) > (b))? ((a) - (b)) : ((b) - (a)))

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
typedef enum _DPRx_ID
{
	DPRx_ID_0 = 0x0,
	DPRx_ID_1 = 0x1,
	DPRx_ID_2 = 0x2,
	DPRx_ID_MAX
}DPRx_ID;

typedef enum _DPRx_AUX_ID
{
	DPRx_AUX_ID_0 = 0x0,
	DPRx_AUX_ID_1 = 0x1,
	DPRx_AUX_ID_2 = 0x2,
	DPRx_AUX_ID_MAX
}DPRx_AUX_ID;

typedef enum _DPRx_PHY_ID
{
	DPRx_PHY_ID_0 = 0x0,
	DPRx_PHY_ID_1 = 0x1,
	DPRx_PHY_ID_MAX
}DPRx_PHY_ID;

typedef enum _DPRx_DECODER_ID
{
	DPRx_DECODER_ID_0 = 0x0,
	DPRx_DECODER_ID_MAX
}DPRx_DECODER_ID;

typedef enum _DP_CHIP_MUX
{
	DP_CHIP_MUX_DECODE0 	   = 0x0,
	DP_CHIP_MUX_DECODE1 	   = 0x1,
	DP_CHIP_MUX_TX_ENCODE0 	   = 0x2,
	DP_CHIP_MUX_TX_TRANSMITTER = 0x3,
	DP_CHIP_MUX_MAX
}DP_CHIP_MUX;

typedef enum _DP_CHIP_MUX_DATA_PATH
{
	DP_CHIP_MUX_PATH_RECEIVER0 	   = 0x0,
	DP_CHIP_MUX_PATH_MST_TO_DECODE = 0x1,
	DP_CHIP_MUX_PATH_DECODE0 	   = 0x2, // Only used for DP_CHIP_MUX_TX_ENCODE0
	DP_CHIP_MUX_PATH_DECODE1 	   = 0x3, // Only used for DP_CHIP_MUX_TX_ENCODE0
	DP_CHIP_MUX_PATH_ENCODE0 	   = 0x4,
	DP_CHIP_MUX_PATH_TX_OUTPUT_MST = 0x5,
	DP_CHIP_MUX_PATH_MAX
}DP_CHIP_MUX_DATA_PATH;

typedef enum _DP_VERSION
{
	DP_VERSION_11 = 0x11,
	DP_VERSION_12 = 0x12,
	DP_VERSION_14 = 0x14,
	DP_VERSION_12_14 = 0x16,
	DP_VERSION_14_14 = 0x17,
	DP_VERSION_MAX
}DP_VERSION;

typedef enum _DP_LINKRATE
{
	DP_LINKRATE_RBR = 0x6,
	DP_LINKRATE_HBR = 0xA,
	DP_LINKRATE_HBR2 = 0x14,
	DP_LINKRATE_HBR25 = 0x19,
	DP_LINKRATE_HBR3 = 0x1E
}DP_LINKRATE;

typedef enum _DP_LANECOUNT
{
	DP_LANECOUNT_1 = 0x1,
	DP_LANECOUNT_2 = 0x2,
	DP_LANECOUNT_4 = 0x4,
}DP_LANECOUNT;

typedef enum _DP_SWING_NUM
{
    DP_SWING0 = 0x00,
    DP_SWING1 = 0x01,
    DP_SWING2 = 0x02,
    DP_SWING3 = 0x03,
}DP_SWING_NUM;

typedef enum _DP_PREEMPHASIS_NUM
{
    DP_PREEMPHASIS0 = 0x00,
    DP_PREEMPHASIS1 = 0x01,
    DP_PREEMPHASIS2 = 0x02,
    DP_PREEMPHASIS3 = 0x03,
}DP_PREEMPHASIS_NUM;

typedef enum _DPRx_HANDLER_STATE_TYPE
{
    DPRx_STATE_INITIAL = 0,   // 0
    DPRx_STATE_RETRAIN,   // 1
    DPRx_STATE_STARTUP,   // 2
    DPRx_STATE_CKECKLOCK, // 3
    DPRx_STATE_MEASURE,   // 4
    DPRx_STATE_NORMAL,    // 5
    DPRx_STATE_POWERSAVE, // 6
    DPRx_STATE_DPIDLE,     // 7
    #if(DPRX_AUDIO_NOVIDEO == 0x1)
    DPRx_STATE_VIDEOMUTE,   // 8
    #endif
}DPRx_HANDLER_STATE_TYPE;

typedef enum _DP_HDCP_STATE
{
    DP_HDCP_NO_ENCRYPTION = 0,
    DP_HDCP_1_4 = 1,
    DP_HDCP_2_2 = 2
}DP_HDCP_STATE;

typedef enum
{
    DP_AUDIO_FMT_NON_PCM = 0,
    DP_AUDIO_FMT_PCM     = 1,
    DP_AUDIO_FMT_UNKNOWN = 2
}DP_AUDIO_FORMAT;

typedef enum _DP_PACKET_TYPE
{
	DP_AUDIO_TIMESTAMP = 0,  // (HB1 packet type = 0x1)
    DP_AUDIO_TIMESREAM, // (HB1 packet type = 0x2)
    DP_EXT_PACKET, //(HB1 packet type = 0x4)
    DP_ACM_PACKET, //(HB1 packet type = 0x5)
    DP_ISRC_PACKET,//(HB1 packet type = 0x6)
    DP_VSC_PACKET, //(HB1 packet type = 0x7)
    DP_PPS_PACKET, //(HB1 packet type = 0x10)
    DP_VSP_PACKET, //(HB1 packet type = 0x81)
	DP_AVI_PACKET, //(HB1 packet type = 0x82)
    DP_SPD_PACKET, //(HB1 packet type = 0x83)
    DP_AUDIO_PACKET,//(HB1 packet type = 0x84)
    DP_MPEG_PACKET, //(HB1 packet type = 0x85)
    DP_NTSC_PACKET, //(HB1 packet type = 0x86)
    DP_VSC_EXT_CEA_PACKET, //(HB1 packet type = 0x20)
    DP_VSC_EXT_VESA_PACKET,//(HB1 packet type = 0x21)
    DP_HDR_PACKET, // HDR metadata, (HB1 packet type = 0x87)
    DP_ADS_PACKET, //(HB1 packet type = 0x22)
    DP_AUDIO_CHANNEL_STATUS, //40 of 192 bits from Audio stream packet
    DP_PACKET_MAX
}DP_PACKET_TYPE;

typedef enum _DP_SDP_SEL
{
	DP_SDP_SEL_AUDIO_TIMESTAMP = 0x0,
	DP_SDP_SEL_AUDIO_STREAM    = 0x1,
	DP_SDP_SEL_EXT 			   = 0x2,
	DP_SDP_SEL_ACM 			   = 0x3,
	DP_SDP_SEL_ISRC 		   = 0x4,
	DP_SDP_SEL_VSC			   = 0x5,
	DP_SDP_SEL_PPS			   = 0x6,
	DP_SDP_SEL_VSP 			   = 0x7,
	DP_SDP_SEL_AVI 			   = 0x8,
	DP_SDP_SEL_SPD 			   = 0x9,
	DP_SDP_SEL_AUI 			   = 0xA,
	DP_SDP_SEL_MPEG 		   = 0xB,
	DP_SDP_SEL_NTSC 		   = 0xC,
	DP_SDP_SEL_VSC_EXT_VESA    = 0xD,
	DP_SDP_SEL_VSC_EXT_CEA 	   = 0xE,
	DP_SDP_SEL_DRM 			   = 0xF, // HDR metadata
	DP_SDP_SEL_ADS			   = 0x10,
	DP_SDP_SEL_MAX
}DP_SDP_SEL;

typedef enum _DP_VSC_EXT_TYPE
{
    VESA = 0x20,
    CEA = 0x21
}DP_VSC_EXT_TYPE;

typedef enum _DP_COLOR_FORMAT_TYPE
{
	DP_COLOR_FORMAT_RGB      = 0,     				// HDMI RGB 444 Color Format
	DP_COLOR_FORMAT_YUV_422  = 1,     				// HDMI YUV 422 Color Format
	DP_COLOR_FORMAT_YUV_444  = 2,     				// HDMI YUV 444 Color Format
	DP_COLOR_FORMAT_YUV_420  = 3,     				// HDMI YUV 420 Color Format
	DP_COLOR_FORMAT_YONLY    = 4,     				// DP Y Only Color Format
	DP_COLOR_FORMAT_RAW      = 5,     				// DP RAW color format
	DP_COLOR_FORMAT_RESERVED = 6,    	 			// Reserve
	DP_COLOR_FORMAT_DEFAULT  = DP_COLOR_FORMAT_RGB, // Default setting
	DP_COLOR_FORMAT_UNKNOWN  = 15  					// Unknow color format
}DP_COLOR_FORMAT_TYPE;

typedef enum _DP_COLORIMETRY_TYPE
{
    DP_COLORIMETRY_xvYCC601       = 0, // HDMI xvYCC601 colorimetry
    DP_COLORIMETRY_xvYCC709       = 1, // HDMI xvYCC709 colorimetry
    DP_COLORIMETRY_sYCC601        = 2, // HDMI sYCC601 colorimetry
    DP_COLORIMETRY_ADOBEYCC601    = 3, // HDMI Adobe YCC 601 colorimetry
    DP_COLORIMETRY_ADOBERBG       = 4, // HDMI Adobe RGB colorimetry
    DP_COLORIMETRY_BT2020YcCbcCrc = 5, // HDMI BT2020 YcCbcCrc colorimetry
    DP_COLORIMETRY_BT2020RGBYCbCr = 6, // HDMI BT2020 RGB or YCbCr colorimetry
    DP_COLORIMETRY_ITU601         = 7,
    DP_COLORIMETRY_ITU709         = 8,
    DP_COLORIMETRY_NONE        	  = 15
}DP_COLORIMETRY_TYPE;

typedef enum _DP_COLOR_RANGE_TYPE
{
    DP_COLOR_RANGE_DEFAULT  = 0, // Depend on video format
    DP_COLOR_RANGE_LIMIT    = 1,
    DP_COLOR_RANGE_FULL     = 2,
    DP_COLOR_RANGE_RESERVED = 3
}DP_COLOR_RANGE_TYPE;

typedef enum _DP_COLOR_DEPTH_TYPE
{
    DP_COLOR_DEPTH_6BIT = 0,
    DP_COLOR_DEPTH_8BIT,
    DP_COLOR_DEPTH_10BIT,
    DP_COLOR_DEPTH_12BIT,
    DP_COLOR_DEPTH_16BIT,
    DP_COLOR_DEPTH_NONE,
}DP_COLOR_DEPTH_TYPE;

typedef enum _DP_YPBPR_COLORIMETRY_TYPE
{
    DP_YUV_COLORIMETRY_ITU601 = 0,
    DP_YUV_COLORIMETRY_ITU709,
    DP_YUV_COLORIMETRY_NoData,
    DP_YUV_COLORIMETRY_EC,
    DP_YUV_COLORIMETRY_MAX
}DP_YPBPR_COLORIMETRY_TYPE;

typedef enum
{
    DP_ePM_POWERON = 0x0,
    DP_ePM_STANDBY,
    DP_ePM_POWEROFF,
    DP_ePM_POWEROFF_NoUsedPort,
    DP_ePM_INVAILD,
}DP_ePM_Mode;

typedef enum _DP_HDCP2_MESSAGE_ID_TYPE
{
    DP_HDCP2_MSG_ID_NONE 					 = 0,
	DP_HDCP2_MSG_ID_AUTHDONE 				 = 1,
    DP_HDCP2_MSG_ID_AKE_INIT 				 = 2,
    DP_HDCP2_MSG_ID_AKE_SEND_CERT 			 = 3,
    DP_HDCP2_MSG_ID_AKE_NO_STORED_KM 		 = 4,
    DP_HDCP2_MSG_ID_AKE_STORED_KM 			 = 5,
    DP_HDCP2_MSG_ID_AKE_H_PRIME_READONE 	 = 6,
    DP_HDCP2_MSG_ID_AKE_SEND_H_PRIME 		 = 7,
    DP_HDCP2_MSG_ID_AKE_SEND_PAIRING_INFO 	 = 8,
    DP_HDCP2_MSG_ID_LC_INIT 				 = 9,
    DP_HDCP2_MSG_ID_LC_SEND_L_PRIME 		 = 10,
    DP_HDCP2_MSG_ID_SKE_SEND_EKS 			 = 11,
    DP_HDCP2_MSG_ID_REPAUTH_SEND_RECVID_LIST = 12,
    DP_HDCP2_MSG_ID_RTT_READY                = 13,
    DP_HDCP2_MSG_ID_RTT_CHALLENGE            = 14,
    DP_HDCP2_MSG_ID_REPAUTH_SEND_ACK         = 15,
    DP_HDCP2_MSG_ID_REPAUTH_STREAM_MANAGE    = 16,
    DP_HDCP2_MSG_ID_REPAUTH_STREAM_READY     = 17,
    DP_HDCP2_MSG_ID_RECEIVER_AUTH_STATUS     = 18,
    DP_HDCP2_MSG_ID_AKE_TRANSMITTER_INFO     = 19,
    DP_HDCP2_MSG_ID_AKE_RECEIVER_INFO        = 20
}DP_HDCP2_MESSAGE_ID_TYPE;

typedef enum _DP_HDCP2_INFORMATION_TYPE
{
    DP_HDCP2_INFO_NONE = 0,
    DP_HDCP2_INFO_AKE_RTX,
    DP_HDCP2_INFO_AKE_TXCAPS,
    DP_HDCP2_INFO_AKE_CERTRX,
    DP_HDCP2_INFO_AKE_RRX,
    DP_HDCP2_INFO_AKE_RXCAPS,
    DP_HDCP2_INFO_AKE_EKPUB_KM,
    DP_HDCP2_INFO_AKE_EKH_KM_WR,
    DP_HDCP2_INFO_AKE_M,
    DP_HDCP2_INFO_AKE_H_PRIME,
    DP_HDCP2_INFO_AKE_EKH_KM_RD,
    DP_HDCP2_INFO_LC_RN,
    DP_HDCP2_INFO_LC_L_PRIME,
    DP_HDCP2_INFO_SKE_EDKEY_KS,
    DP_HDCP2_INFO_SKE_RIV,
    DP_HDCP2_INFO_RECVLIST_RXINFO,
    DP_HDCP2_INFO_RECVLIST_SEQ_NUM_V,
    DP_HDCP2_INFO_RECVLIST_VPRIME,
    DP_HDCP2_INFO_RECVLIST_RECEIVER_ID_LIST,
    DP_HDCP2_INFO_ACK_SEND_ACK_V,
    DP_HDCP2_INFO_STREAM_SEQ_NUM_M,
    DP_HDCP2_INFO_STREAM_k,
    DP_HDCP2_INFO_STREAM_STREAM_ID_TYPE,
    DP_HDCP2_INFO_STREAM_MPRIME
}DP_HDCP2_INFORMATION_TYPE;

typedef enum _DP_HDCP2_RXSTATUS_TYPE
{
    DP_HDCP2_RXSTATUS_READY 				 = BIT0,
    DP_HDCP2_RXSTATUS_H_PRIME_AVAILABLE 	 = BIT1,
    DP_HDCP2_RXSTATUS_PAIRING_AVAILABLE 	 = BIT2,
    DP_HDCP2_RXSTATUS_REAUTH_REQ 			 = BIT3,
    DP_HDCP2_RXSTATUS_LINK_INTEGRITY_FAILURE = BIT4
}DP_HDCP2_RXSTATUS_TYPE;

typedef enum _DP_INPUTPORT_TYPE
{
    DP_INPUTPORT_IS_NONE       = 0x0,
    DP_INPUTPORT_IS_DP_PORT    = BIT0,
    DP_INPUTPORT_IS_DPC_PORT   = BIT1,
    DP_INPUTPORT_IS_USB_PORT   = BIT2,
}DP_INPUTPORT_TYPE;

typedef enum _DPRx_MCCS_WAKEUP
{
    DPRx_MCCS_D1 = 0x0,
    DPRx_MCCS_D4 = 0x1,
    DPRx_MCCS_D5 = 0x2,
    DPRx_MCCS_MAX
}DPRx_MCCS_WAKEUP;

typedef BOOL(*DPRx_HDCP2_CB_FUNC)(BYTE, BYTE*); // Need sync mapi_dprx.h "DPRx_HDCP2_CALL_BACK_FUNC"
typedef void(*DPRX_HDCP2_Config_Repeater_CB_FUNC)(BYTE, BYTE, BOOL);
typedef BOOL(*DPRx_HDCP14_CHECK_VPRIME_CB_FUNC)(BYTE);
typedef void(*DPRx_HDCP14AKSVReceived_CB_FUNC)(BYTE);
#endif

