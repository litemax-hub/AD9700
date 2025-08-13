#ifndef HDCPMBX_H
#define HDCPMBX_H

//----------------------------------------------------------------------//
// defines
//----------------------------------------------------------------------//
#define DEF_HDCPMBX_BANK            0x103300
#define DEF_HDCPMBX_BASE            0x400

#define DEF_PORT_NUMBER               DEF_HDCP_PORT_NUM
#define DEF_HDCPMBX_CMD_LEN           8 //sizeof(stHdcpMbxCommand);

#define DEF_HDCPMBX_CMD_HEADER_OFFSET 0 //offset of command header
#define DEF_HDCPMBX_CMD_INFO_OFFSET   1 //offset of command info
#define DEF_HDCPMBX_SARG_OFFSET       2 //offset of (size of argument)
#define DEF_HDCPMBX_AARG_OFFSET       4 //offset of (address of argument)

//bit index of header field
#define DEF_HDCPMBX_ACT_BIT_BITIDX    7 //bit index of active bit
#define DEF_HDCPMBX_ROLE_BITIDX       6 //bit index of role field
#define DEF_HDCPMBX_CMD_ID_BITIDX     0 //bit index of command ID

//bit mask of header field
#define DEF_HDCPMBX_ACT_BIT_BITMASK    (1 << DEF_HDCPMBX_ACT_BIT_BITIDX) //0x80 //bit mask of active bit
#define DEF_HDCPMBX_ROLE_BITMASK       (1 << DEF_HDCPMBX_ROLE_BITIDX) //0x40 //bit mask of role field
#define DEF_HDCPMBX_CMD_ID_BITMASK     0x3F //bit mask of command ID

//bit index of Command Info Field
#define DEF_HDCPMBX_CMD_HOST_BITIDX   7 //bit index of command host
#define DEF_HDCPMBX_ARG_FLAG_BITIDX   6 //bit index of argument flag; 1:with arg, 0:without arg;
#define DEF_HDCPMBX_ERR_CODE_BITIDX   3 //bit index of error code
#define DEF_HDCPMBX_CMD_STATE_BITIDX  0 //bit index of command state

//bit mask of Command Info Field
#define DEF_HDCPMBX_CMD_HOST_BITMASK   (1 << DEF_HDCPMBX_CMD_HOST_BITIDX) //0x80 //bit mask of command host
#define DEF_HDCPMBX_ARG_FLAG_BITMASK   (1 << DEF_HDCPMBX_ARG_FLAG_BITIDX) //0x40 //bit mask of argument flag; 1:with arg, 0:without arg;
#define DEF_HDCPMBX_ERR_CODE_BITMASK   0x38 //bit mask of error code
#define DEF_HDCPMBX_CMD_STATE_BITMASK  0x07 //bit mask of command state

//----------------------------------------------------------------------//
// struct
//----------------------------------------------------------------------//
typedef enum
{
/**** MStar Vendor Specific ****/
    EN_MBXCMD_CODEBASE_ADDR                 = 0,
    EN_MBXCMD_SET_DRAM_SEC_RANGE            = 1,
    EN_MBXCMD_INIT_MAILBOX_MEMORY           = 28,
/**** HDCP22 ****/
    EN_MBXCMD_HDCP2_AKE_INIT                = 2,
    EN_MBXCMD_HDCP2_AKE_SEND_CERT           = 3,
    EN_MBXCMD_HDCP2_AKE_NO_STORED_KM        = 4,
    EN_MBXCMD_HDCP2_AKE_STORED_KM           = 5,
    EN_MBXCMD_HDCP2_AKE_SEND_H_PRIME        = 7,
    EN_MBXCMD_HDCP2_AKE_SEND_PARING_INFO    = 8,
    EN_MBXCMD_HDCP2_LC_INIT                 = 9,
    EN_MBXCMD_HDCP2_LC_SEND_L_PRIME         = 10,
    EN_MBXCMD_HDCP2_SKE_SEND_EKS            = 11,
    EN_MBXCMD_HDCP2_REP_SEND_RECVID_LIST    = 12,
    EN_MBXCMD_HDCP2_REP_SEND_ACK            = 15,
    EN_MBXCMD_HDCP2_REP_STREAM_MANAGE       = 16,
    EN_MBXCMD_HDCP2_REP_STREAM_READY        = 17,
/**** HDCP14 ****/
    EN_MBXCMD_HDCP1_TX_INIT_HDCP            = 13,
    EN_MBXCMD_HDCP1_TX_SET_AUTH_PASS        = 14,
    EN_MBXCMD_HDCP1_TX_ENABLE_ENCEN         = 18,
    EN_MBXCMD_HDCP1_TX_PROCESS_AN           = 19,
    EN_MBXCMD_HDCP1_TX_GET_AKSV             = 20,
    EN_MBXCMD_HDCP1_TX_COMPARE_RI           = 21,
    EN_MBXCMD_HDCP1_TX_CONFIG_MODE          = 22,
    EN_MBXCMD_HDCP1_TX_GENERATE_CIPHER      = 23,
    EN_MBXCMD_HDCP1_TX_PROCESS_R0           = 24,
    EN_MBXCMD_HDCP1_TX_CHECK_VPRIME         = 25,
    EN_MBXCMD_HDCP1_TX_COMPUTE_H1XRX_VPRIME = 26,
    EN_MBXCMD_HDCP1_TX_COMPUTE_MST_SHA1LPRIME = 30,
    EN_MBXCMD_HDCP1_TX_GET_R0               = 31,  ////For CHIP after MST9U5, R0 is reported by SECU CPU instead of HK CPU

//special case
    EN_MBXCMD_HDCP2_RESET_HW                = 27,
    EN_MBXCMD_HDCP2_RESET_PORTINFO          = 29,
} ENUM_HDCPMBX_COMMAND_LIST;

typedef enum
{
    EN_HDCPMBX_ERR_NONE = 0,
    EN_HDCPMBX_ERR_INVALID_ARG,
} ENUM_HDCPMBX_ERR_CODE;

typedef enum
{
    EN_HDCPMBX_STATE_INIT = 0,
    EN_HDCPMBX_STATE_IDLE,
    EN_HDCPMBX_STATE_WAITING,
    EN_HDCPMBX_STATE_PROCESSING,
    EN_HDCPMBX_STATE_HALT,
    EN_HDCPMBX_STATE_DONE
} ENUM_HDCPMBX_STATE_LIST;

typedef enum
{
    EN_HDCPMBX_CMD_HOST_HKR2,
    EN_HDCPMBX_CMD_HOST_SECUR2
} ENUM_HDCPMBX_COMMAND_HOST;

typedef enum
{
    EN_HDCPMBX_PORT_TYPE_SINK,
    EN_HDCPMBX_PORT_TYPE_SOURCE
} ENUM_HDCPMBX_PORT_TYPE;

typedef enum
{
    EN_HDCPMBX_CC_HDCP2X_ENABLE_AUTH = 0,
    EN_HDCPMBX_CC_LOAD_HDCP1X_KEY,
    EN_HDCPMBX_CC_LOAD_HDCP2X_RX_KEY,
    EN_HDCPMBX_CC_LOAD_HDCP2X_TX_KEY,
    EN_HDCPMBX_CC_LOAD_HDCP2X_RX_KEY_WITH_CUSTOMIZE_KEY,
    EN_HDCPMBX_CC_LOAD_HDCP2X_TX_KEY_WITH_CUSTOMIZE_KEY,
    EN_HDCPMBX_CC_HDCP_ENABLE_ENC,
	EN_HDCPMBX_CC_SECU_POWER_DOWN,
    EN_HDCPMBX_CC_LOAD_HDCP2X_RX_RAWKEY,
    EN_HDCPMBX_CC_SECU_IS_HDMI,
} ENUM_HDCPMBX_COMMON_COMMAND_LIST;

typedef struct
{
    BYTE    u8Header;        // active bit(bit 7) + port type(bit6) + command ID(bit 5 ~ bit 0)
    BYTE    u8CommandInfo;   // Command host(bit 7) + Arg Flag(bit 6) + Error Code(bit 5 ~ bit 3) + State (bit 2 ~ bit 0)
    WORD    u16ArgSize;
    ULONG   u32ArgAddr;
} ST_HDCPMBX_COMMAND;

//----------------------------------------------------------------------//
// Function prototype
//----------------------------------------------------------------------//
//----------------------------------------------------------------
/// HDCPMBX_ClearCmd()
/// @brief Clear mailbox content of certain port
/// @param u8PortIdx \b IN: Port Index
/// @return bool
//----------------------------------------------------------------
BOOL HDCPMBX_ClearCmd(BYTE u8PortIdx);

//----------------------------------------------------------------
/// HDCPMBX_SetActiveBit()
/// @brief Set active field of mailbox header byte
/// @param u8PortIdx \b IN: Port Index
/// @param bActive   \b IN: Indicate active or inactive
/// @return bool
//----------------------------------------------------------------
BOOL HDCPMBX_SetActiveBit(BYTE u8PortIdx, BOOL bActive);

//----------------------------------------------------------------
/// HDCPMBX_SetRoleField()
/// @brief Set role field of mailbox header byte
/// @param u8PortIdx \b IN: Port Index
/// @param enRole    \b IN: Indicate sink or source command
/// @return bool
//----------------------------------------------------------------
BOOL HDCPMBX_SetRoleField(BYTE u8PortIdx, ENUM_HDCPMBX_PORT_TYPE enRole);

//----------------------------------------------------------------
/// HDCPMBX_SetCmdID()
/// @brief Set command ID field of mailbox header byte
/// @param u8PortIdx \b IN: Port Index
/// @param u8CmdID   \b IN: Command ID
/// @return bool
//----------------------------------------------------------------
BOOL HDCPMBX_SetCmdID(BYTE u8PortIdx, BYTE u8CmdID);

//----------------------------------------------------------------
/// HDCPMBX_SetState()
/// @brief Set state field of command info byte
/// @param u8PortIdx \b IN: Port Index
/// @param enState   \b IN: Enum of command states
/// @return void
//----------------------------------------------------------------
BOOL HDCPMBX_SetState(BYTE u8PortIdx, ENUM_HDCPMBX_STATE_LIST enState);

//----------------------------------------------------------------
/// HDCPMBX_SetErrorCode()
/// @brief Set error code field of command info byte
/// @param u8PortIdx \b IN: Port Index
/// @param enErrCode \b IN: Enum of error code
/// @return bool
//----------------------------------------------------------------
BOOL HDCPMBX_SetErrorCode(BYTE u8PortIdx, ENUM_HDCPMBX_ERR_CODE enErrCode);

//----------------------------------------------------------------
/// HDCPMBX_SetArgFlag()
/// @brief Set Argument bit of command info byte
/// @param u8PortIdx \b IN: Port Index
/// @param bArgFlag  \b IN: Indicate this command with or without argument
/// @return bool
//----------------------------------------------------------------
BOOL HDCPMBX_SetArgFlag(BYTE u8PortIdx, BOOL bArgFlag);

//----------------------------------------------------------------
/// HDCPMBX_SetCmdHost()
/// @brief Set host field of command info byte
/// @param u8PortIdx \b IN: Port Index
/// @param enCmdHost \b IN: Indicate which CPU issues the command
/// @return bool
//----------------------------------------------------------------
BOOL HDCPMBX_SetCmdHost(BYTE u8PortIdx, ENUM_HDCPMBX_COMMAND_HOST enCmdHost);

//----------------------------------------------------------------
/// HDCPMBX_SetArgSize()
/// @brief Set size of argument
/// @param u8PortIdx \b IN: Port Index
/// @param u16Size   \b IN: Size of arguments of this command
/// @return bool
//----------------------------------------------------------------
BOOL HDCPMBX_SetArgSize(BYTE u8PortIdx, WORD u16Size);

//----------------------------------------------------------------
/// HDCPMBX_SetArgAddr()
/// @briefinitiallze hardware setting of hdcp1.x Tx module
/// @param u8PortIdx  \b IN: Port Index
/// @param u32ArgAddr \b IN: Address of the command's argument
/// @return bool
//----------------------------------------------------------------
BOOL HDCPMBX_SetArgAddr(BYTE u8PortIdx, ULONG u32ArgAddr);

//----------------------------------------------------------------
/// HDCPMBX_SetCmd()
/// @brief Set all require field of command and trigger interrupt to notify receiver
/// @param u8PortIdx  \b IN: Port Index
/// @param enRole     \b IN: Indicate sink or source command
/// @param u8CmdID    \b IN: Command ID
/// @param enCmdHost  \b IN: Indicate which CPU issues the command
/// @param u16ArgSize \b IN: Size of command's arguments
/// @param u32ArgAddr \b IN: Address of command's arguments
/// @return bool
//----------------------------------------------------------------
BOOL HDCPMBX_SetCmd(BYTE u8PortIdx, ENUM_HDCPMBX_PORT_TYPE enRole, BYTE u8CmdID, ENUM_HDCPMBX_COMMAND_HOST enCmdHost, WORD u16ArgSize, ULONG u32ArgAddr);

//----------------------------------------------------------------
/// HDCPMBX_GetCmd()
/// @brief Get and verify command of certain command slot according to port index
/// @param enCmdHost     \b IN: Indicate which CPU issues the command
/// @param u8PortIdx     \b IN: Port Index
/// @param pstHdcpMbxCmd \b IN: Pointer to return mailbox command
/// @return bool
//----------------------------------------------------------------
BOOL HDCPMBX_GetCmd(ENUM_HDCPMBX_COMMAND_HOST enCmdHost, BYTE u8PortIdx, ST_HDCPMBX_COMMAND* pstHdcpMbxCmd);

//----------------------------------------------------------------
/// HDCPMBX_GetCmd()
/// @brief Get command state of specific port
/// @param u8PortIdx     \b IN: Port Index
/// @param enState     \b IN: pointer to return command state
/// @return bool
//----------------------------------------------------------------
BOOL HDCPMBX_GetCmdState(BYTE u8PortIdx, ENUM_HDCPMBX_STATE_LIST* enState);

//----------------------------------------------------------------
/// HDCPMBX_GetCommonCmd()
/// @brief Set common commands of all port, ex: load hdcp key..etc
/// @param pstHdcpMbxCmd      \b: IN: Pointer to return mailbox command
/// @return bool
//----------------------------------------------------------------
BOOL HDCPMBX_GetCommonCmd(ST_HDCPMBX_COMMAND* pstHdcpMbxCmd);

//----------------------------------------------------------------
/// HDCPMBX_SetCommonCmdState()
/// @brief Set commands state of common command
/// @param enCmdState      \b: IN: state of common command
/// @return none
//----------------------------------------------------------------
void HDCPMBX_SetCommonCmdState(ENUM_HDCPMBX_STATE_LIST enCmdState);

//----------------------------------------------------------------
/// HDCPMBX_SetCommonCmd()
/// @brief Set common commands of all port, ex: load hdcp key..etc
/// @param enCmd      \b: IN: List of common command
/// @param u16ArgSize \b IN: Size of command's arguments
/// @param u32ArgAddr \b IN: Address of command's arguments
/// @return bool
//----------------------------------------------------------------
BOOL HDCPMBX_SetCommonCmd(ENUM_HDCPMBX_COMMON_COMMAND_LIST enCmd, WORD u16ArgSize, ULONG u32ArgAddr);

//----------------------------------------------------------------
/// HDCPMBX_Init()
/// @brief Initial HDCP mailbox; clear all command slot
/// @return void
//----------------------------------------------------------------
void HDCPMBX_Init(void);

#endif //#ifndef HDCPMBX_H
