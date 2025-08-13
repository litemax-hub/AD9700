#ifndef HDCP_COMMON_H
#define HDCP_COMMON_H

#include "HdcpError.h"
//[MT9700]#include "combo_config.h"
#include "ComboApp.h"

//----------------------------------------------------------------------//
// defines
//----------------------------------------------------------------------//
//#define RNG(x)	(BYTE)(MDrv_ReadByte(DEF_REG_DSCRMB0_02_L + (x%2)))
//#define DEF_LG_USE                      1


//general
#define DEF_HDCP_PORT_NUM               1//[MT9700] (COMBO_IP_SUPPORT_TYPE + COMBO_OP_SUPPORT_TYPE)
#define DEF_HDCPMSG_MAX_PRIORITY        (2*DEF_HDCP_PORT_NUM + 1)
#define DEF_DESCRMB_INT_MODE_EN         0
#define DEF_SIZE_OF_HDCP1X_KEY          304
#define DEF_SHA256_DIGEST_SIZE          32
#define DEF_AES_DATA_SIZE               16
#define DEF_HDCP2_GOLDEN_SAMPLE_EN      0
#define DEF_HDCP_STORED_KM_CAPACITY     1

//hdcp14 relative
#define DEF_HDCP1X_BCAPS_SIZE           1
#define DEF_HDCP1X_BSTATUS_SIZE         2
#define DEF_HDCP1X_RI_SIZE              2
#define DEF_HDCP1X_KSV_SIZE             5
#define DEF_HDCP1X_AN_SIZE              8
#define DEF_HDCP1X_AINFO_SIZE           1
#define DEF_HDCP1X_V_SIZE               20
#define DEF_HDCP1X_M0_SIZE              8
#define DEF_HDCP1X_ENC_KEY_SIZE         304 //same for tx, rx key
#define DEF_HDCP1X_DBG_SIZE             0
#define DEF_HDCP1X_KSV_FIFO_SIZE        1
#define DEF_HDCP1X_MAX_KSV_SIZE         127

#define DEF_HDCP1X_RX_KEY_SIZE                  284
#define DEF_HDCP1X_BKSV_SIZE                 5

//hdcp22 relative
#define DEF_HDCP2_CMD_NUM               21

//x74 relative
#define DEF_HDCP2_VER_NFO_SIZE          1
#define DEF_HDCP2_RXSTATUS_SIZE         2

#define DEF_HDCP2_RECV_ID_SIZE          5
#define DEF_HDCP2_KPUBRX_MODULUS_SIZE   128
#define DEF_HDCP2_KPUBRX_EXPONENT_SIZE  3
#define DEF_HDCP2_CERTRESERVED_SIZE     2
#define DEF_HDCP2_KPUBDCP_SIZE          384
#define DEF_HDCP2_KPUBTX_EXP_SIZE       1

#define DEF_KPUBRX_P_SIZE               64   //512 bits
#define DEF_KPUBRX_Q_SIZE               64   // 512 bits
#define DEF_KPUBRX_DP_SIZE              64   // 512 bits
#define DEF_KPUBRX_DQ_SIZE              64   // 512 bits
#define DEF_KPUBRX_QINV_SIZE            64   // 512 bits
#define DEF_HDCP2_RX_PRIV_KEY_SIZE      (DEF_KPUBRX_P_SIZE + DEF_KPUBRX_Q_SIZE + DEF_KPUBRX_DP_SIZE + DEF_KPUBRX_DQ_SIZE + DEF_KPUBRX_QINV_SIZE)

#define DEF_HDCP2_RTX_SIZE              8
#define DEF_HDCP2_RRX_SIZE              8
#define DEF_HDCP2_TXCAPS_SIZE           3
#define DEF_HDCP2_RXCAPS_SIZE           3
#define DEF_HDCP2_M_SIZE                (DEF_HDCP2_RTX_SIZE + DEF_HDCP2_RRX_SIZE)
#define DEF_HDCP2_KD_SIZE               32
#define DEF_HDCP2_CERT_SIZE             522
#define DEF_HDCP2_EKPUBKM_SIZE          128
#define DEF_HDCP2_EKHKM_SIZE            16
#define DEF_HDCP2_KM_SIZE               16
#define DEF_HDCP2_KH_SIZE               16
#define DEF_HDCP2_H_SIZE                32
#define DEF_HDCP2_RN_SIZE               8
#define DEF_HDCP2_L_SIZE                32
#define DEF_HDCP2_LPRIME_SIZE           32
#define DEF_HDCP2_EDKEYKS_SIZE          16
#define DEF_HDCP2_KS_SIZE               16
#define DEF_HDCP2_RIV_SIZE              8
#define DEF_HDCP2_RXINFO_SIZE           2
#define DEF_HDCP2_SEQ_NUM_V_SIZE        3
#define DEF_HDCP2_V_SIZE                32
#define DEF_HDCP2_VPRIME_SIZE           32
#define DEF_HDCP2_RECV_ID_LIST_SIZE     155
#define DEF_HDCP2_SEQ_NUM_M_SIZE        3
#define DEF_HDCP2_MAX_STREAM_MANAGE_K   63
#define DEF_HDCP2_STREAMID_TYPE_SIZE    (2 * DEF_HDCP2_MAX_STREAM_MANAGE_K)
#define DEF_HDCP2_K_SIZE                2
#define DEF_HDCP2_REP_M_SIZE            32
#define DEF_HDCP2_HPRIME_SIZE           32
#define DEF_HDCP2_RX_ENCKEY_SIZE        1044
#define DEF_HDCP2_TX_ENCKEY_SIZE        448
#define DEF_HDCP2_MAX_DEVICE_COUNT      31
#define DEF_HDCP2_MAX_DEPTH_LEVEL       4


//repeater relative
#define DEF_HDCP2_MAX_DEV_EXCEEDED      (1<<3)
#define DEF_HDCP2_MAX_CASCADE_EXCEEDED  (1<<2)
#define DEF_HDCP2_STREAM_MANAGE_RETRY_CNT   8   //avoid 1B-10 1 re-auth too fast

//secure storm relative
#define DEF_SEED_KEY_SIZE                   16
#define DEF_OUTER_HEADER_SIZE	            32
#define DEF_MAGIC_ID_SIZE		            32
#define DEF_INNER_HEADER_SIZE	            32 // value 0 for MsGenKey tool; value 32 for SecureStore tool
#define DEF_POST_HASH_SIZE		            32

#define DEF_HDCP2_TX_KEY_SIZE               448 //include padding
#define DEF_HDCP2_TX_KEY_NON_PADDING_SIZE   445
#define DEF_HDCP2_TX_ENCODE_KEY_SIZE        580

#define DEF_HDCP2_RX_KEY_SIZE               912 //include padding
#define DEF_HDCP2_RX_KEY_NON_PADDING_SIZE   902
#define DEF_HDCP2_RX_ENCODE_KEY_SIZE        1044

#define DEF_SECURESTORM_AES_KEY_SIZE        16
#define DEF_SS_KEY_INFO_INDEX               3 //stored if we already re-encrypt key

//timeout relative 14
#define DEF_HDCP1_CHK_R0_WDT                    100     // 100ms
#define DEF_HDCP1_CHK_RI_PERIOD_WDT             2000     // 2 sec
#define DEF_HDCP1_RPT_RDY_BIT_WDT               6000     // 5 sec

//timeout relative 22
#define DEF_HDCP2_AKESENDCERT_WDT               200     // 100ms
#define DEF_HDCP2_AKESENDHPRIME_NO_STORED_WDT   2000    // 1sec
#define DEF_HDCP2_AKESENDHPRIME_STORED_WDT      300     // 200sec
#define DEF_HDCP2_AKESENDPAIRINGINFO_WDT        160      // 20ms
#define DEF_HDCP2_LCSENDLPRIME_WDT              160     // 20ms
#define DEF_HDCP2_REPAUTHSENDRECVID_WDT         5000    // 3 sec
#define DEF_HDCP2_REPAUTHSTREAMRDY_WDT          500     // 100 ms
#define DEF_HDCP2_ENC_EN_TIMER                  200     // 200 ms
#define DEF_HDCP2_REP_SEND_ACK                  2000    // 2 Sec
//Patch for QD980 CTS
#define DEF_DELAY_R0_COMPARE                    1
//----------------------------------------------------------------------//
// data type define
//----------------------------------------------------------------------//

typedef enum
{
    EN_HDCP_OPCODE_WRITE    = 0x00,
    EN_HDCP_OPCODE_READ     = 0x01,
} ENUM_HDCP_OPERATION;

typedef enum
{
    EN_HDCP1X_X74_OFFSET_BKSV       = 0x00,
    EN_HDCP1X_X74_OFFSET_RIPRIME    = 0x08,
    EN_HDCP1X_X74_OFFSET_PJPRIME    = 0x0A,
    EN_HDCP1X_X74_OFFSET_AKSV       = 0x10,
    EN_HDCP1X_X74_OFFSET_AINFO      = 0x15,
    EN_HDCP1X_X74_OFFSET_AN         = 0x18,
    EN_HDCP1X_X74_OFFSET_VPRIME_H0  = 0x20,
    EN_HDCP1X_X74_OFFSET_VPRIME_H1  = 0x24,
    EN_HDCP1X_X74_OFFSET_VPRIME_H2  = 0x28,
    EN_HDCP1X_X74_OFFSET_VPRIME_H3  = 0x2C,
    EN_HDCP1X_X74_OFFSET_VPRIME_H4  = 0x30,
    EN_HDCP1X_X74_OFFSET_BCAPS      = 0x40,
    EN_HDCP1X_X74_OFFSET_BSTATUS    = 0x41,
    EN_HDCP1X_X74_OFFSET_KSVFIFO    = 0x43,
    EN_HDCP1X_X74_OFFSET_DBG        = 0xC0
} ENUM_HDCP1X_X74_OFFSET;

typedef enum
{
    EN_HDCP2X_X74_OFFSET_VERSION    = 0x50,
    EN_HDCP2X_X74_OFFSET_WRITEMSG   = 0x60,
    EN_HDCP2X_X74_OFFSET_RXSTATUS   = 0x70,
    EN_HDCP2X_X74_OFFSET_READMSG    = 0x80,
    EN_HDCP2X_X74_OFFSET_DBG        = 0xC0
} ENUM_HDCP2X_X74_OFFSET;

//b'0 =>  0: Rx,
//        1: Tx;
//b'1 =>  0: hdcp1.4,
//        1: hdcp2.2
typedef enum
{
    EN_HDCP_VER_NONE = 0,
    EN_HDCP_VER_FREERUN,
    EN_HDCP_VER_14,
    EN_HDCP_VER_22,
    EN_HDCP_REFLECT_14,
    EN_HDCP_REFLECT_22,
} ENUM_HDCP_VER_CTRL;

//hdcp14 relative
typedef enum
{
    EN_HDCP14TX_MS_A0        = 0x00,
    EN_HDCP14TX_MS_A1A2      = 0x01,
    EN_HDCP14TX_MS_A3        = 0x03,
    EN_HDCP14TX_MS_A4        = 0x04,
    EN_HDCP14TX_MS_A5        = 0x05,
    EN_HDCP14TX_MS_A6        = 0x06,
    EN_HDCP14TX_MS_A7        = 0x07,
    EN_HDCP14TX_MS_A8        = 0x08,
    EN_HDCP14TX_MS_A9        = 0x09
} ENUM_HDCP14TX_MAIN_STATE;

typedef enum
{
    EN_HDCP14TX_SS_IDLE            = 0x00,
    EN_HDCP14TX_SS_EXCHANGE_KSV    = 0x01,
    EN_HDCP14TX_SS_VERIFY_BKSV     = 0x02,
    EN_HDCP14TX_SS_COMPUTE         = 0x03,
    EN_HDCP14TX_SS_CHECK_R0        = 0x04,
    EN_HDCP14TX_SS_AUTH_DONE       = 0x05,
    EN_HDCP14TX_SS_POLLING_RDY     = 0x06,
    EN_HDCP14TX_SS_AUTH_REPEATER   = 0x07,
    EN_HDCP14TX_SS_CHECK_RI        = 0x08,
    EN_HDCP14TX_SS_AUTH_START      = 0x09,
    EN_HDCP14TX_SS_PROCESS_AN      = 0x0A,
    EN_HDCP14TX_SS_SET_KSVFIFO     = 0x0B,
    EN_HDCP14TX_SS_DP_GET_R0       = 0x0C, //For CHIP after MST9U5, R0 is reported by SECU CPU instead of HK CPU
} ENUM_HDCP14TX_SUB_STATE;

//hdcp22 relative
typedef enum
{
    EN_HDCP2TX_MS_H1P1 = 0,
    EN_HDCP2TX_MS_A0F0 = 1,
    EN_HDCP2TX_MS_A1F1 = 2,
    EN_HDCP2TX_MS_A2F2 = 3,
    EN_HDCP2TX_MS_A3F3 = 4,
    EN_HDCP2TX_MS_A4F4 = 5,
    EN_HDCP2TX_MS_A5F5 = 6,
    EN_HDCP2TX_MS_A6F6 = 7,
    EN_HDCP2TX_MS_A7F7 = 8,
    EN_HDCP2TX_MS_A8F8 = 9,
    EN_HDCP2TX_MS_A9F9 = 10
}ENUM_HDCP2TX_MAIN_STATE;

typedef enum
{
    EN_HDCP2RX_MS_B0 = 0,
    EN_HDCP2RX_MS_B1,
    EN_HDCP2RX_MS_B2,
    EN_HDCP2RX_MS_B3,
    EN_HDCP2RX_MS_B4,
} ENUM_HDCP2RX_MAIN_STATE;

typedef enum
{
    EN_HDCP2_MSG_ZERO                    = 0,
    EN_HDCP2_MSG_NULL                    = 1,
    EN_HDCP2_MSG_AKE_INIT                = 2,
    EN_HDCP2_MSG_AKE_SEND_CERT           = 3,
    EN_HDCP2_MSG_AKE_NO_STORED_KM        = 4,
    EN_HDCP2_MSG_AKE_STORED_KM           = 5,
    EN_HDCP2_MSG_AKE_SEND_RRX            = 6,
    EN_HDCP2_MSG_AKE_SEND_H_PRIME        = 7,
    EN_HDCP2_MSG_AKE_SEND_PAIRING_INFO   = 8,
    EN_HDCP2_MSG_LC_INIT                 = 9,
    EN_HDCP2_MSG_LC_SEND_L_PRIME         = 10,
    EN_HDCP2_MSG_SKE_SEND_EKS            = 11,
    EN_HDCP2_MSG_REPAUTH_SEND_RECVID_LIST = 12,
    EN_HDCP2_MSG_RTT_READY               = 13,
    EN_HDCP2_MSG_RTT_CHALLENGE           = 14,
    EN_HDCP2_MSG_REPAUTH_SEND_ACK        = 15,
    EN_HDCP2_MSG_REPAUTH_STREAM_MANAGE   = 16,
    EN_HDCP2_MSG_REPAUTH_STREAM_READY    = 17,
    EN_HDCP2_MSG_RECEIVER_AUTH_STATUS    = 18,
    EN_HDCP2_MSG_AKE_TRANSMITTER_INFO    = 19,
    EN_HDCP2_MSG_AKE_RECEIVER_INFO       = 20
} ENUM_HDCP2_MSG_LIST;

typedef enum
{
    EN_HDCP2_SEQNUMV_PASS       = 0,
    EN_HDCP2_SEQNUMV_FAIL       = 1,
    EN_HDCP2_SEQNUMV_ROLLOVER   = 2
} ENUM_HDCP2_SEQNUM_V_STATUS;

typedef struct
{
    BOOL        bStored;
    BYTE        u8RecvID[DEF_HDCP2_RECV_ID_SIZE];
    BYTE        u8M[DEF_HDCP2_M_SIZE];
    BYTE        u8KM[DEF_HDCP2_KM_SIZE];
    BYTE        u8EkhKM[DEF_HDCP2_EKHKM_SIZE];
} ST_HDCP2_PAIRING_INFO;//stHDCP22PairingInfo;

typedef struct
{
    BYTE                        u8SeqNumV[DEF_HDCP2_SEQ_NUM_V_SIZE];
    BYTE                        u8RetryCnt;
    ENUM_HDCP2_SEQNUM_V_STATUS  enSeqNumVStatus;
	BOOL                        bSeqNumVFull;
} ST_HDCP2_SEQ_NUM_HANDLER;

typedef struct
{
    BOOL        bEnc14;
    BYTE        u8Status;
    BYTE        u8AN[DEF_HDCP1X_AN_SIZE];
    BYTE        u8AKSV[DEF_HDCP1X_KSV_SIZE];
    BYTE        u8BKSV[DEF_HDCP1X_KSV_SIZE];
    BYTE        u8Ri[DEF_HDCP1X_RI_SIZE];
    BYTE        u8Bcaps;
    BYTE        u8BStatus[DEF_HDCP1X_BSTATUS_SIZE];
} ST_HDCP14_LG_INFO;    //stHDCP14Info;

typedef struct
{
    BOOL        bEnc22;
    BYTE        u8Status;
} ST_HDCP22_LG_INFO;    //stHDCP22Info;

#endif//#ifndef HDCP_COMMON_H
