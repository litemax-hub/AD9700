#ifndef HDCP_ERR_H
#define HDCP_ERR_H
//----------------------------------------------------------------------//
// defines
//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
// data type
//----------------------------------------------------------------------//
typedef enum
{
    EN_HDCP_ERR_NONE = 0,
    EN_HDCP_ERR_UNKNOWN_STATE,
    EN_HDCP_ERR_INVALID_ARG,
    EN_HDCP_ERR_SECIP_BUSY,
    EN_HDCP_ERR_SET_MBX_CMD_FAIL,
    EN_HDCP_ERR_RESPONSE_TIMEROUT,
    EN_HDCP_ERR_PROCESS_FAIL
} ENUM_HDCP_ERR_CODE;
#endif //#ifndef HDCP_ERR_H
