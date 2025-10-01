#ifndef _MAILBOX_H
#define _MAILBOX_H

#include "MsOS.h"

#ifdef _MAILBOX_C
#define _MAILBOXDEC_C
#else
#define _MAILBOXDEC_C extern
#endif

#if ENABLE_USB_TYPEC
#define BANK_MBX                            (0x001C00)
#define REG_MBX_PD51_TO_HKR2_CMD_ID         (BANK_MBX+0x00)  //0x00~0x3F is used for mbx PD51 to HK51
#define REG_MBX_PD51_TO_HKR2_CMD_LEN        (BANK_MBX+0x01)
#define REG_MBX_PD51_TO_HKR2_CMD_DATA_START (BANK_MBX+0x02)
#define REG_MBX_HKR2_BUSY_STATUS            (BANK_MBX+0x3E)
#define REG_MBX_HKR2_READY                  (BANK_MBX+0x3F)

#define REG_MBX_HKR2_TO_PD51_CMD_ID         (BANK_MBX+0x40)  //0x40~0x7F is used for mbx HK51 to PD51
#define REG_MBX_HKR2_TO_PD51_CMD_LEN        (BANK_MBX+0x41)
#define REG_MBX_HKR2_TO_PD51_CMD_DATA_START (BANK_MBX+0x42)
#define REG_MBX_HKR2_TO_PD51_CMD_READ_SIZE  (BANK_MBX+0x70)  //0x70 .. for HK read cmd and PD reply size here.
#define REG_MBX_HKR2_TO_PD51_CMD_READ_DATA  (BANK_MBX+0x71)  //0x71~0x78 .. for HK read cmd and PD reply data here.
#define REG_MBX_PD51_BUSY_STATUS            (BANK_MBX+0x7E)
#define REG_MBX_PD51_READY                  (BANK_MBX+0x7F)

#define REG_MBX_PD51_TO_HKR2_U3_STATE       (BANK_MBX+0x8F)  // 1: redriver in U3 state

#define REG_MBX_HKR2_TO_PD51_CUS_START      (BANK_MBX+0xE0)  // For customized usage, e.g. init data for PD51
#define REG_MBX_HKR2_TO_PD51_CUS_END        (BANK_MBX+0xE7)  // For customized usage, e.g. init data for PD51
#define REG_MBX_WDT_ACK                     (BANK_MBX+0xFF)

#define MBX_CNT_MINUS_1(x) {\
	                           MsOS_DisableAllInterrupts();\
	                           x --;\
	                           MsOS_RestoreAllInterrupts(0);\
                           }
typedef enum
{
    ///////////////////////////
    // PD51 -> HKR2 Write Cmd //
    //////////////////////////
    EN_PD_to_DISPLAYPORT_CMD_SET_PIN_ASSGN           = 0x00, // P1: CC status, P2: Pin Assignment, P3:0:portA, 1:port B
    EN_PD_to_HK_CMD_ATTACH_STATUS                    = 0x01, // P1: 0:portA, 1:port B, P2: 0:Detached, 1:Attached
    EN_PD_to_HK_CMD_CUSTOM                           = 0x02, // Parameters: customized.

    ////////////////////////////////
    // PD51 -> HKR2 Read Cmd (TBD) //
    ///////////////////////////////
    //EN_PD_to_HK_ReadCmd                              = 0x40,

    ///////////////////////////
    // HKR2 -> PD51 Write Cmd //
    ///////////////////////////
    EN_DISPLAYPORT_to_PD_CMD_HPD_CTRL                = 0x80, // P1:HPD high/low, P2:0:portA, 1:port B
    EN_HK_to_PD_CMD_MCU_SPEED_UPDATE                 = 0x81, // No Parameters
    EN_HK_to_PD_CMD_PM_STATUS_UPDATE                 = 0x82, // P1: PM status 0:ON, 1:DPMS, 2:DCOFF, P2:TypeC 0:OFF, 1:ON, 2:Auto_OFF, 3:Auto_ON
    EN_HK_to_PD_CMD_MULTI_FUNC_CONFIG                = 0x83, // P1: Multi function 1:Enable, 0:Disable
    EN_HK_to_PD_CMD_VIDEO_TYPEC_PORT                 = 0x84, // P1: 0:portA, 1:port B, 2:non TypeC port
    EN_HK_to_PD_CMD_UPLL_OFF                         = 0x85, // 0: UPLL ON, 1: UPLL OFF
    EN_HK_to_PD_CMD_CUSTOM                           = 0x86, // Parameters: customized.
    EN_HK_to_PD_CMD_BUBST_CONFIG                     = 0x87, // P1: bit mask, [0]: 1: disable buck-boost in pwr snk, [1]: disable buck-boost when cable is detached
    EN_HK_to_PD_CMD_BB_DISCONNECT                    = 0x88, // 1: Disconnect BB.

    ////////////////////////////////
    // HK51 -> PD51 Read Cmd(TBD) //
    ////////////////////////////////
    EN_HK_to_PD_RCMD_TCPMC_VER                       = 0xC0, // No Parameters, Read back 4B-TCPM & 2B-TCPC version.
	EN_HK_to_PD_RCMD_CC_IDLE                         = 0xC1, // No Parameters, Read back 1B-CC idle status, 1: idle, 0: busy.
}EN_MBX_COMMAND_IDX;

typedef enum _SYSTEMIP_HPD
{
    SYSTEMIP_HPD_LOW = 0,
    SYSTEMIP_HPD_HIGH = 1,
    SYSTEMIP_HPD_IRQ_HPDLOW = 2,
    SYSTEMIP_HPD_IRQ_HPDHIGH = 3,
} SYSTEMIP_HPD;


typedef enum
{
    eDeSelect    =0x00,
    ePinType_A   =0x01,
    ePinType_B   =0x02,
    ePinType_C   =0x04,
    ePinType_D   =0x08,
    ePinType_E   =0x10
}eUFPDPinAssignment;

typedef enum
{
    eCC1        =0x00,
    eCC2        =0x01,
}eCCPin;

typedef enum
{
    EN_PM_TYPEC_OFF,
    EN_PM_TYPEC_ON,
    EN_PM_TYPEC_AUTO_OFF,
    EN_PM_TYPEC_AUTO_ON,
}EN_PM_TYPEC_MODE;


#define MBX_CMD_MAX_LEN         32 //max 60 need set same value as hk51
#define MBX_BUF_LEN             (MBX_CMD_MAX_LEN*4)
typedef struct
{
    EN_MBX_COMMAND_IDX u8CommandId;
    unsigned char u8Length;
    unsigned char u8CmdData[MBX_CMD_MAX_LEN];
    unsigned char u8CmdEmpty;
} ST_MBX_DATA;

extern BYTE XDATA mbx_buffer[MBX_BUF_LEN];
extern BYTE XDATA mbx_idx_rx; // receive index in ISR
extern BYTE XDATA mbx_idx_st; // process index in mbx handler
extern BYTE XDATA mbx_cmd_cnt;
extern BYTE XDATA glSYS_TypeC_PinAssign[2]; // 0:PortA, 1:PortB
extern BYTE XDATA glSYS_CC_Pin[2]; // 0:PortA, 1:PortB
extern BYTE XDATA glSYS_Attach[2]; // idx 0:PortA, 1:PortB, attached status: 0:detached, 1:attached
extern BYTE XDATA glSYS_TypeC_PortIndex;

_MAILBOXDEC_C void MBX_init(void);
_MAILBOXDEC_C void drvmbx_receive_cmd_isr(void);
_MAILBOXDEC_C BOOL drvmbx_send_HPD_Ctrl_CMD(BYTE u8Ctrl, BYTE u8TypeCPortIdx);
_MAILBOXDEC_C BOOL drvmbx_send_MCU_Speed_Update_CMD(BYTE u8SpeedIdx);
_MAILBOXDEC_C BOOL drvmbx_send_PM_Status_Update_CMD(BYTE u8PMStatus, BYTE u8TypeCMode);
_MAILBOXDEC_C BOOL drvmbx_send_Multi_Func_Config_CMD(BYTE u8Enable);
_MAILBOXDEC_C BOOL drvmbx_send_Video_TypeC_Port_CMD(BYTE u8TypeCPortIdx);
_MAILBOXDEC_C BOOL drvmbx_send_Upll_Off(BYTE u8Off);
_MAILBOXDEC_C BOOL drvmbx_send_CC_IDLE_RCMD(BYTE *pu8CcIdle);
_MAILBOXDEC_C BOOL drvmbx_send_BuBst_Config_CMD(BYTE u8Config);
_MAILBOXDEC_C BOOL drvmbx_send_BB_Disconnect_CMD(BYTE u8Enable);
_MAILBOXDEC_C void MBX_handler(void);
_MAILBOXDEC_C void MBX_TPC_EnterDPAltModeCheck(void);
_MAILBOXDEC_C BOOL mapi_CC_GetAttachStatus(BYTE u8TypeCPortIdx);

#else
_MAILBOXDEC_C void MBXDummy(void);
#endif

#endif
