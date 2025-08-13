#ifndef __DDC_MTK_TEST_HANDLER_H__
#define __DDC_MTK_TEST_HANDLER_H__

#include <Global.h>

// command code
#define MTK_TEST_CMD                    0xE1

// vcp code
#define MTK_TEST_GET_IP_NUM             0x20
#define MTK_TEST_GET_IP_STATUS          0x21
#define MTK_TEST_GET_SC_IN_STATUS       0x22
#define MTK_TEST_GET_SC_OUT_STATUS      0x23
#define MTK_TEST_GET_DW_STATUS          0x24
#define MTK_TEST_GET_PM_STATUS          0x25
#define MTK_TEST_GET_USBC_PD_STATUS     0x26
#define MTK_TEST_GET_TIMING_INFO        0x27
#define MTK_TEST_GET_HDCP_CAP           0x28
#define MTK_TEST_GET_MST_STATUS         0x29
#define MTK_TEST_GET_AUDIO_STATUS       0x2A

#define MTK_TEST_SWITCH_PORT            0x40
#define MTK_TEST_SWITCH_POWER_DC        0x41
#define MTK_TEST_SWITCH_HDCP_CAP        0x42
#define MTK_TEST_SWITCH_MST             0x43
#define MTK_TEST_TIMING_CHANGE          0x44

#define MTK_TEST_GET_FLAG_POWER_SAVING  0x80
#define MTK_TEST_SET_FLAG_POWER_SAVING  0x81
#define MTK_TEST_GET_FLAG_PORT_SCAN     0x82
#define MTK_TEST_SET_FLAG_PORT_SCAN     0x83
#define MTK_TEST_GET_FLAG_AUDIO_MUTE    0x84
#define MTK_TEST_SET_FLAG_AUDIO_MUTE    0x85

extern void InitMtkTest(void);
extern void MtkTestUpdateHandler(void);
extern BYTE MtkTestCmdHandler(void);

#endif
