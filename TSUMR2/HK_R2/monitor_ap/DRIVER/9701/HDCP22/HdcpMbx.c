#ifndef HDCPMBX_C
#define HDCPMBX_C
//#if (ENABLE_SECU_R2 == 1) && (ENABLE_HDCP22 == 1)

//----------------------------------------------------------------------//
// Includes
//----------------------------------------------------------------------//
#include <string.h>
#include <stdlib.h>
#include "datatype.h"
#include "drvGlobal.h"
#include "HdcpMbx.h"
#include "HdcpLog.h"
#include "Utl.h"
#include "drv_Hdcp_IMI.h"//[MT9701]
#include "drvIMI.h"
#include "HdcpCommon.h"

//----------------------------------------------------------------------//
// defines
//----------------------------------------------------------------------//
#define DEF_HDCPMBX_CMD_BASE    8 //start address of command slot;
#define DEF_HDCPMBX_COMMON_BASE 0 //for common usage for all ports; ex: load key

#define HDCPMBX_REG(offset)                         (DEF_HDCPMBX_BANK + offset) //(DEF_HDCPMBX_BANK + DEF_HDCPMBX_CMD_BASE + offset)
#define HDCPMBX_ReadByte(offset)                    MDrv_ReadByte(HDCPMBX_REG(offset))
#define HDCPMBX_Read2Byte(offset)                   MDrv_Read2Byte(HDCPMBX_REG(offset))
#define HDCPMBX_Read4Byte(offset)                   MDrv_Read4Byte(HDCPMBX_REG(offset))
#define HDCPMBX_WriteByte(offset, value)            MDrv_WriteByte(HDCPMBX_REG(offset), value)
#define HDCPMBX_WriteByteMask(offset, value, mask)  MDrv_WriteByteMask(HDCPMBX_REG(offset), value, mask)
#define HDCPMBX_Write2Byte(offset, value)           MDrv_Write2Byte(HDCPMBX_REG(offset), value)
#define HDCPMBX_Write4Byte(offset, value)           MDrv_Write4Byte(HDCPMBX_REG(offset), value)


//----------------------------------------------------------------------//
// global
//----------------------------------------------------------------------//
//static ST_HDCPMBX_COMMAND gstHdcpMbxCommand[DEF_PORT_NUMBER];
//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
void __________PROTOTYPE__________(void);
BOOL _HDCPMBX_TriggerInt(ENUM_HDCPMBX_COMMAND_HOST enHost);
BOOL _HDCPMBX_GetOffset(BYTE u8PortIdx, BYTE* pu8Offset);
BOOL _HDCPMBX_CheckMutexFree(BYTE u8PortIdx);
BOOL _HDCPMBX_CheckCmdValid(BYTE u8PortIdx, ENUM_HDCPMBX_COMMAND_HOST enCmdHost);

void __________FUNCTION__________(void);

//----------------------------------------------------------------------//
// Functions
//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
// Internal Usage
//----------------------------------------------------------------------//
BOOL _HDCPMBX_TriggerInt(ENUM_HDCPMBX_COMMAND_HOST enHost)
{
    BOOL bRet = FALSE;

    do
    {
        if ((enHost != EN_HDCPMBX_CMD_HOST_HKR2) && (enHost != EN_HDCPMBX_CMD_HOST_SECUR2))
        {
            printf("ERROR: Invalid Host\r\n");
            break;
        }

        if (enHost == EN_HDCPMBX_CMD_HOST_HKR2)
        {
            MDrv_WriteByte(0x100540, 0x00);
            MDrv_WriteByte(0x100540, BIT2);
        }
        else
        {
            MDrv_WriteByte(0x002A4C, 0x00);
            MDrv_WriteByte(0x002A4C, BIT0);
        }

        bRet = TRUE;

    } while(FALSE);

    return bRet;
}

BOOL _HDCPMBX_GetOffset(BYTE u8PortIdx, BYTE* pu8Offset)
{
    BOOL bRet = FALSE;

    do
    {
        if (u8PortIdx > DEF_HDCP_PORT_NUM)
        {
             printf("ERROR: Invalid Port Index!\r\n");
             break;
        }

        if (pu8Offset == NULL)
        {
            printf("ERROR: pu8Offset is NULL, Invalid Input Argument!\r\n");
            break;
        }

        *pu8Offset = DEF_HDCPMBX_CMD_LEN*u8PortIdx + DEF_HDCPMBX_CMD_BASE;
        bRet = TRUE;

    } while(FALSE);

    return bRet;
}

BOOL HDCPMBX_ClearCmd(BYTE u8PortIdx)
{
    BOOL bRet = FALSE;
    BYTE u8Offset = 0;
    BYTE u8ByteCnt = 0;

    do
    {
        if (u8PortIdx > DEF_HDCP_PORT_NUM)
        {
             printf("ERROR: Invalid Port Index!\r\n");
             break;
        }

        if (_HDCPMBX_GetOffset(u8PortIdx, &u8Offset) == FALSE)
        {
            printf("ERROR: Get Offset fail!\r\n");
            break;
        }

        for (u8ByteCnt = 0; u8ByteCnt < DEF_HDCPMBX_CMD_LEN; u8ByteCnt++)
        {
            HDCPMBX_WriteByte(u8Offset + u8ByteCnt, 0x00);
        }

        bRet = TRUE;

    } while(FALSE);

    return bRet;
}

BOOL _HDCPMBX_CheckMutexFree(BYTE u8PortIdx)
{
    BOOL bRet = FALSE;
    BYTE u8Offset = 0;

    do
    {
        if (u8PortIdx > DEF_HDCP_PORT_NUM)
        {
             printf("ERROR: Invalid Port Index!\r\n");
             break;
        }

        if (_HDCPMBX_GetOffset(u8PortIdx, &u8Offset) == FALSE)
        {
            printf("ERROR: Get Offset fail!\r\n");
            break;
        }

        bRet = (HDCPMBX_ReadByte(u8Offset + DEF_HDCPMBX_CMD_HEADER_OFFSET) & DEF_HDCPMBX_ACT_BIT_BITMASK) ? FALSE : TRUE;
    } while(FALSE);

    return bRet;
}

BOOL _HDCPMBX_CheckCmdValid(BYTE u8PortIdx, ENUM_HDCPMBX_COMMAND_HOST enCmdHost)
{
    BOOL bRet = FALSE;
    BYTE u8Offset = 0;

    printf("_HDCPMBX_CheckCmdValid:: Port = 0x%d", u8PortIdx);
    do
    {
        if (u8PortIdx > DEF_HDCP_PORT_NUM)
        {
             printf("ERROR: Invalid Port Index!\r\n");
             break;
        }

        if (_HDCPMBX_GetOffset(u8PortIdx, &u8Offset) == FALSE)
        {
            printf("ERROR: Get Offset fail!\r\n");
            break;
        }

        //check if host is valid;
        if ((HDCPMBX_ReadByte(u8Offset + DEF_HDCPMBX_CMD_INFO_OFFSET) & DEF_HDCPMBX_CMD_HOST_BITMASK) == (enCmdHost << DEF_HDCPMBX_CMD_HOST_BITIDX))
        {
            printf("Invalid Host!\r\n");
            break;
        }

        //check active bit;
        if (!(HDCPMBX_ReadByte(u8Offset + DEF_HDCPMBX_CMD_HEADER_OFFSET) & DEF_HDCPMBX_ACT_BIT_BITMASK))
        {
            printf("Inactive Command!\r\n");
            break;
        }

        //check command state;
        if ((HDCPMBX_ReadByte(u8Offset + DEF_HDCPMBX_CMD_INFO_OFFSET) & DEF_HDCPMBX_CMD_STATE_BITMASK) != EN_HDCPMBX_STATE_WAITING)
        {
            printf("Not In Waiting State!\r\n");
            break;
        }

        bRet = TRUE;

    } while(FALSE);

    return bRet;
}

//----------------------------------------------------------------------//
// Command Header relative
//----------------------------------------------------------------------//
BOOL HDCPMBX_SetActiveBit(BYTE u8PortIdx, BOOL bActive)
{
    BOOL bRet = FALSE;
    BYTE u8Offset = 0;

    do
    {
        if (u8PortIdx > DEF_HDCP_PORT_NUM)
        {
             printf("ERROR: Invalid Port Index!\r\n");
             break;
        }

        if (_HDCPMBX_GetOffset(u8PortIdx, &u8Offset) == FALSE)
        {
            printf("ERROR: Get Offset fail!\r\n");
            break;
        }

        HDCPMBX_WriteByteMask(u8Offset + DEF_HDCPMBX_CMD_HEADER_OFFSET, \
            (bActive == TRUE ? 1 : 0) << DEF_HDCPMBX_ACT_BIT_BITIDX, DEF_HDCPMBX_ACT_BIT_BITMASK);
        bRet = TRUE;

    } while(FALSE);

    return bRet;
}

BOOL HDCPMBX_SetRoleField(BYTE u8PortIdx, ENUM_HDCPMBX_PORT_TYPE enRole)
{
    BOOL bRet = FALSE;
    BYTE u8Offset = 0;

    do
    {
        if (u8PortIdx > DEF_HDCP_PORT_NUM)
        {
             printf("ERROR: Invalid Port Index!\r\n");
             break;
        }

        if (_HDCPMBX_GetOffset(u8PortIdx, &u8Offset) == FALSE)
        {
            printf("ERROR: Get Offset fail!\r\n");
            break;
        }

        HDCPMBX_WriteByteMask(u8Offset + DEF_HDCPMBX_CMD_HEADER_OFFSET, \
            enRole << DEF_HDCPMBX_ROLE_BITIDX, DEF_HDCPMBX_ROLE_BITMASK);
        bRet = TRUE;
    } while(FALSE);

    return bRet;
}

BOOL HDCPMBX_SetCmdID(BYTE u8PortIdx, BYTE u8CmdID)
{
    BOOL bRet = FALSE;
    BYTE u8Offset = 0;

    do
    {
        if (u8PortIdx > DEF_HDCP_PORT_NUM)
        {
             printf("ERROR: Invalid Port Index!\r\n");
             break;
        }

        if (_HDCPMBX_GetOffset(u8PortIdx, &u8Offset) == FALSE)
        {
            printf("ERROR: Get Offset fail!\r\n");
            break;
        }

        HDCPMBX_WriteByteMask(u8Offset + DEF_HDCPMBX_CMD_HEADER_OFFSET, \
            u8CmdID << DEF_HDCPMBX_CMD_ID_BITIDX, DEF_HDCPMBX_CMD_ID_BITMASK);
        bRet = TRUE;

    } while(FALSE);

    return bRet;
}



//----------------------------------------------------------------------//
// Command Info relative
//----------------------------------------------------------------------//
BOOL HDCPMBX_SetState(BYTE u8PortIdx, ENUM_HDCPMBX_STATE_LIST enState)
{
    BOOL bRet = FALSE;
    BYTE u8Offset = 0;

    do
    {
        if (u8PortIdx > DEF_HDCP_PORT_NUM)
        {
             printf("ERROR: Invalid Port Index!\r\n");
             break;
        }

        if (_HDCPMBX_GetOffset(u8PortIdx, &u8Offset) == FALSE)
        {
            printf("ERROR: Get Offset fail!\r\n");
            break;
        }

        HDCPMBX_WriteByteMask(u8Offset + DEF_HDCPMBX_CMD_INFO_OFFSET, \
            enState << DEF_HDCPMBX_CMD_STATE_BITIDX, DEF_HDCPMBX_CMD_STATE_BITMASK);
        bRet = TRUE;

    } while(FALSE);

    return bRet;
}

BOOL HDCPMBX_SetErrorCode(BYTE u8PortIdx, ENUM_HDCPMBX_ERR_CODE enErrCode)
{
    BOOL bRet = FALSE;
    BYTE u8Offset = 0;

    do
    {
        if (u8PortIdx > DEF_HDCP_PORT_NUM)
        {
             printf("ERROR: Invalid Port Index!\r\n");
             break;
        }

        if (_HDCPMBX_GetOffset(u8PortIdx, &u8Offset) == FALSE)
        {
            printf("ERROR: Get Offset fail!\r\n");
            break;
        }

        HDCPMBX_WriteByteMask(u8Offset + DEF_HDCPMBX_CMD_INFO_OFFSET, \
            (enErrCode & 0x07) << DEF_HDCPMBX_ERR_CODE_BITIDX, DEF_HDCPMBX_ERR_CODE_BITMASK);
        bRet = TRUE;

    } while(FALSE);

    return bRet;
}

BOOL HDCPMBX_SetArgFlag(BYTE u8PortIdx, BOOL bArgFlag)
{
    BOOL bRet = FALSE;
    BYTE u8Offset = 0;

    do
    {
        if (u8PortIdx > DEF_HDCP_PORT_NUM)
        {
             printf("ERROR: Invalid Port Index!\r\n");
             break;
        }

        if (_HDCPMBX_GetOffset(u8PortIdx, &u8Offset) == FALSE)
        {
            printf("ERROR: Get Offset fail!\r\n");
            break;
        }

        HDCPMBX_WriteByteMask(u8Offset + DEF_HDCPMBX_CMD_INFO_OFFSET, \
            (bArgFlag == TRUE ? 1 : 0) << DEF_HDCPMBX_ARG_FLAG_BITIDX, DEF_HDCPMBX_ARG_FLAG_BITMASK);
        bRet = TRUE;

    } while(FALSE);

    return bRet;
}

BOOL HDCPMBX_SetCmdHost(BYTE u8PortIdx, ENUM_HDCPMBX_COMMAND_HOST enCmdHost)
{
    BOOL bRet = FALSE;
    BYTE u8Offset = 0;

    do
    {
        if (u8PortIdx > DEF_HDCP_PORT_NUM)
        {
             printf("ERROR: Invalid Port Index!\r\n");
             break;
        }

        if (_HDCPMBX_GetOffset(u8PortIdx, &u8Offset) == FALSE)
        {
            printf("ERROR: Get Offset fail!\r\n");
            break;
        }

        HDCPMBX_WriteByteMask(u8Offset + DEF_HDCPMBX_CMD_INFO_OFFSET, \
            (enCmdHost == EN_HDCPMBX_CMD_HOST_SECUR2 ? (1 << DEF_HDCPMBX_CMD_HOST_BITIDX) : 0), DEF_HDCPMBX_ARG_FLAG_BITMASK);
        bRet = TRUE;

    } while(FALSE);

    return bRet;
}

//----------------------------------------------------------------------//
// SArg relative
//----------------------------------------------------------------------//
BOOL HDCPMBX_SetArgSize(BYTE u8PortIdx, WORD u16Size)
{
    BOOL bRet = FALSE;
    BYTE u8Offset = 0;

    do
    {
        if (u8PortIdx > DEF_HDCP_PORT_NUM)
        {
             printf("ERROR: Invalid Port Index!\r\n");
             break;
        }

        if (_HDCPMBX_GetOffset(u8PortIdx, &u8Offset) == FALSE)
        {
            printf("ERROR: Get Offset fail!\r\n");
            break;
        }

        HDCPMBX_Write2Byte(u8Offset + DEF_HDCPMBX_SARG_OFFSET, u16Size);
        bRet = TRUE;

    } while(FALSE);

    return bRet;
}


//----------------------------------------------------------------------//
// AArg relative
//----------------------------------------------------------------------//
BOOL HDCPMBX_SetArgAddr(BYTE u8PortIdx, ULONG u32ArgAddr)
{
    BOOL bRet = FALSE;
    BYTE u8Offset = 0;

    do
    {
        if (u8PortIdx > DEF_HDCP_PORT_NUM)
        {
             printf("ERROR: Invalid Port Index!\r\n");
             break;
        }

        if (_HDCPMBX_GetOffset(u8PortIdx, &u8Offset) == FALSE)
        {
            printf("ERROR: Get Offset fail!\r\n");
            break;
        }

        HDCPMBX_Write4Byte(u8Offset + DEF_HDCPMBX_AARG_OFFSET, u32ArgAddr);
        bRet = TRUE;

    } while(FALSE);

    return bRet;
}

//----------------------------------------------------------------------//
// General usage
//----------------------------------------------------------------------//
BOOL HDCPMBX_SetCmd(BYTE u8PortIdx, ENUM_HDCPMBX_PORT_TYPE enRole, BYTE u8CmdID, ENUM_HDCPMBX_COMMAND_HOST enCmdHost, WORD u16ArgSize, ULONG u32ArgAddr)
{
    BOOL bRet = FALSE;
    BYTE u8Offset = 0;

    do
    {
        HLOGV(EN_HLOG_HDCP, "Port[%d], Role = %d, Command ID = 0x%X, Host = %d, ArgSize = 0x%X, ArgAddr = 0x%X", \
                u8PortIdx, enRole, u8CmdID, enCmdHost, u16ArgSize, u32ArgAddr);

        if (u8PortIdx > DEF_HDCP_PORT_NUM)
        {
             printf("ERROR: Invalid Port Index!\r\n");
             break;
        }

        if (_HDCPMBX_GetOffset(u8PortIdx, &u8Offset) == FALSE)
        {
            printf("ERROR: Get Offset fail!\r\n");
            break;
        }

        //check mutex free;
        if (_HDCPMBX_CheckMutexFree(u8PortIdx) == FALSE)
        {
            printf("ERROR: This Port %02X Is busy currently\r\n",u8PortIdx);
            break;
        }

        //set header;
        HDCPMBX_SetCmdID(u8PortIdx, u8CmdID);
        //HDCPMBX_SetActiveBit(u8PortIdx, TRUE);  //move to last step, this bit will trigger SECU R2 fetch command
        HDCPMBX_SetRoleField(u8PortIdx, enRole);

        //set Command Info;
        HDCPMBX_SetCmdHost(u8PortIdx, enCmdHost);
        HDCPMBX_SetArgFlag(u8PortIdx, (u16ArgSize == 0)? FALSE : TRUE);
        HDCPMBX_SetErrorCode(u8PortIdx, EN_HDCPMBX_ERR_NONE);
        HDCPMBX_SetState(u8PortIdx, EN_HDCPMBX_STATE_WAITING);

        //set SArg;
        HDCPMBX_SetArgSize(u8PortIdx, u16ArgSize);

        //set AArg;
        HDCPMBX_SetArgAddr(u8PortIdx, ((u32ArgAddr & 0xFFFF)|(DEF_SECU_IMI_BASE_ADDR)));  //IMI: Hk- 0x9000 0000, Secu-0x8000 0000

        //trigger Int;
        //_HDCPMBX_TriggerInt(enCmdHost);

        HDCPMBX_SetActiveBit(u8PortIdx, TRUE);

		//[MT9701]if(u8CmdID == EN_MBXCMD_HDCP2_LC_INIT)
		{
			_HDCPMBX_TriggerInt(enCmdHost);
		}

        bRet = TRUE;
    } while(FALSE);

    return bRet;
}

BOOL HDCPMBX_GetCmdState(BYTE u8PortIdx, ENUM_HDCPMBX_STATE_LIST* enState)
{
    BOOL bRet = FALSE;
    BYTE u8Offset = 0;

    do
    {
        if (u8PortIdx > DEF_HDCP_PORT_NUM)
        {
             printf("ERROR: Invalid Port Index!\r\n");
             break;
        }

        if (_HDCPMBX_GetOffset(u8PortIdx, &u8Offset) == FALSE)
        {
             printf("ERROR: Get offset fail!\r\n");
            break;
        }

        *enState = (ENUM_HDCPMBX_STATE_LIST)(HDCPMBX_ReadByte(u8Offset + DEF_HDCPMBX_CMD_INFO_OFFSET) & DEF_HDCPMBX_CMD_STATE_BITMASK);
        bRet = TRUE;
    } while(FALSE);

    return bRet;
}

BOOL HDCPMBX_GetCmd(ENUM_HDCPMBX_COMMAND_HOST enCmdHost, BYTE u8PortIdx, ST_HDCPMBX_COMMAND* pstHdcpMbxCmd)
{
    BOOL bRet = FALSE;
    BYTE u8Offset = 0;
    ENUM_HDCPMBX_STATE_LIST enState;

    do
    {
        if (u8PortIdx > DEF_HDCP_PORT_NUM)
        {
             HLOGE(EN_HLOG_HDCP, "ERROR: Invalid Port Index!\r\n");
             break;
        }

        if (_HDCPMBX_CheckCmdValid(u8PortIdx, enCmdHost) == FALSE)
        {
            HLOGE(EN_HLOG_HDCP, "ERROR: Invalid Command!\r\n");
            break;
        }

        if (_HDCPMBX_GetOffset(u8PortIdx, &u8Offset) == FALSE)
        {
            HLOGE(EN_HLOG_HDCP, "ERROR: Get offset fail!\r\n");
            break;
        }

        if (HDCPMBX_GetCmdState(u8PortIdx, &enState) == FALSE)
        {
            HLOGE(EN_HLOG_HDCP, "Get State Fail!");
            break;
        }

        if (enState != EN_HDCPMBX_STATE_WAITING)
        {
            HLOGI(EN_HLOG_HDCP, "Not in WAITING State!");
            break;
        }

        //fetch content;
        pstHdcpMbxCmd->u8Header = HDCPMBX_ReadByte(u8Offset + DEF_HDCPMBX_CMD_HEADER_OFFSET); //command header;
        pstHdcpMbxCmd->u8CommandInfo = HDCPMBX_ReadByte(u8Offset + DEF_HDCPMBX_CMD_INFO_OFFSET); //command info;
        pstHdcpMbxCmd->u16ArgSize = HDCPMBX_Read2Byte(u8Offset + DEF_HDCPMBX_SARG_OFFSET);
        pstHdcpMbxCmd->u32ArgAddr = HDCPMBX_Read4Byte(u8Offset + DEF_HDCPMBX_AARG_OFFSET);

        bRet = TRUE;
    } while(FALSE);

    return bRet;
}

BOOL HDCPMBX_GetCommonCmd(ST_HDCPMBX_COMMAND* pstHdcpMbxCmd)
{
    BOOL bRet = FALSE;

    do
    {
        //check if host is valid;
        if ((HDCPMBX_ReadByte(DEF_HDCPMBX_COMMON_BASE + DEF_HDCPMBX_CMD_INFO_OFFSET) & DEF_HDCPMBX_CMD_HOST_BITMASK) != EN_HDCPMBX_CMD_HOST_HKR2)
        {
            printf("Invalid Host!\r\n");
            break;
        }

        //check active bit;
        if (!(HDCPMBX_ReadByte(DEF_HDCPMBX_COMMON_BASE + DEF_HDCPMBX_CMD_HEADER_OFFSET) & DEF_HDCPMBX_ACT_BIT_BITMASK))
        {
            printf("Inactive Command!\r\n");
            break;
        }

        //check command state;
        if ((HDCPMBX_ReadByte(DEF_HDCPMBX_COMMON_BASE + DEF_HDCPMBX_CMD_INFO_OFFSET) & DEF_HDCPMBX_CMD_STATE_BITMASK) != EN_HDCPMBX_STATE_WAITING)
        {
            printf("Not In Waiting State!\r\n");
            break;
        }

        pstHdcpMbxCmd->u8Header = HDCPMBX_ReadByte(DEF_HDCPMBX_CMD_HEADER_OFFSET + DEF_HDCPMBX_COMMON_BASE);        // active bit(bit 7) + port type(bit6) + command ID(bit 5 ~ bit 0)
        pstHdcpMbxCmd->u8CommandInfo = HDCPMBX_ReadByte(DEF_HDCPMBX_CMD_INFO_OFFSET + DEF_HDCPMBX_COMMON_BASE);;   // Command host(bit 7) + Arg Flag(bit 6) + Error Code(bit 5 ~ bit 3) + State (bit 2 ~ bit 0)
        pstHdcpMbxCmd->u16ArgSize = HDCPMBX_Read2Byte(DEF_HDCPMBX_SARG_OFFSET+ DEF_HDCPMBX_COMMON_BASE);
        pstHdcpMbxCmd->u32ArgAddr = HDCPMBX_Read4Byte(DEF_HDCPMBX_AARG_OFFSET+ DEF_HDCPMBX_COMMON_BASE);

        bRet = TRUE;
    } while(FALSE);

    return bRet;
}

void HDCPMBX_SetCommonCmdState(ENUM_HDCPMBX_STATE_LIST enCmdState)
{
    HDCPMBX_WriteByteMask(DEF_HDCPMBX_CMD_HEADER_OFFSET + DEF_HDCPMBX_COMMON_BASE,  enCmdState << DEF_HDCPMBX_CMD_STATE_BITIDX,  DEF_HDCPMBX_CMD_STATE_BITMASK);
}

BOOL HDCPMBX_SetCommonCmd(ENUM_HDCPMBX_COMMON_COMMAND_LIST enCmd, WORD u16ArgSize, ULONG u32ArgAddr)
{
    BOOL bRet = FALSE;
    do
    {
        switch (enCmd)
        {
            case EN_HDCPMBX_CC_LOAD_HDCP1X_KEY:
            case EN_HDCPMBX_CC_LOAD_HDCP2X_RX_KEY:
            case EN_HDCPMBX_CC_LOAD_HDCP2X_TX_KEY:
            case EN_HDCPMBX_CC_HDCP2X_ENABLE_AUTH:
            case EN_HDCPMBX_CC_LOAD_HDCP2X_RX_KEY_WITH_CUSTOMIZE_KEY:
            case EN_HDCPMBX_CC_LOAD_HDCP2X_TX_KEY_WITH_CUSTOMIZE_KEY:
      			case EN_HDCPMBX_CC_SECU_POWER_DOWN:
            case EN_HDCPMBX_CC_LOAD_HDCP2X_RX_RAWKEY:
            case EN_HDCPMBX_CC_SECU_IS_HDMI:
            {
                //fill header;
                HDCPMBX_WriteByte(DEF_HDCPMBX_CMD_HEADER_OFFSET + DEF_HDCPMBX_COMMON_BASE, (enCmd & DEF_HDCPMBX_CMD_ID_BITMASK) | DEF_HDCPMBX_ACT_BIT_BITMASK);
                bRet = TRUE;
            }
            break;

            default:
            {
                HLOGEC(EN_HLOG_HDCP, DEF_COLOR_RED, "ERROR: Invalid Common Command!");
            }
            break;
        }

        if (bRet == TRUE)
        {
            ENUM_HDCPMBX_STATE_LIST enCmdState = EN_HDCPMBX_STATE_IDLE;
            //check command state;
            enCmdState = (ENUM_HDCPMBX_STATE_LIST)((HDCPMBX_ReadByte(DEF_HDCPMBX_CMD_INFO_OFFSET + DEF_HDCPMBX_COMMON_BASE) & DEF_HDCPMBX_CMD_STATE_BITMASK) >> DEF_HDCPMBX_CMD_STATE_BITIDX);

            if (((enCmdState == EN_HDCPMBX_STATE_DONE ) || (enCmdState == EN_HDCPMBX_STATE_HALT)) || enCmdState == EN_HDCPMBX_STATE_INIT)
            {
                //set Command Info;
                HDCPMBX_WriteByteMask(DEF_HDCPMBX_CMD_INFO_OFFSET + DEF_HDCPMBX_COMMON_BASE, EN_HDCPMBX_CMD_HOST_HKR2 << DEF_HDCPMBX_CMD_HOST_BITIDX, DEF_HDCPMBX_CMD_HOST_BITMASK); //host;
                HDCPMBX_WriteByteMask(DEF_HDCPMBX_CMD_INFO_OFFSET + DEF_HDCPMBX_COMMON_BASE, ((u16ArgSize == 0)? 0 : 1) << DEF_HDCPMBX_ARG_FLAG_BITIDX, DEF_HDCPMBX_ARG_FLAG_BITMASK); //arg flag;
                HDCPMBX_WriteByteMask(DEF_HDCPMBX_CMD_INFO_OFFSET + DEF_HDCPMBX_COMMON_BASE, EN_HDCPMBX_ERR_NONE << DEF_HDCPMBX_ERR_CODE_BITIDX, DEF_HDCPMBX_ERR_CODE_BITMASK);//err code;
                HDCPMBX_WriteByteMask(DEF_HDCPMBX_CMD_INFO_OFFSET + DEF_HDCPMBX_COMMON_BASE, EN_HDCPMBX_STATE_WAITING << DEF_HDCPMBX_CMD_STATE_BITIDX, DEF_HDCPMBX_CMD_STATE_BITMASK); //state;

                //set SArg;
                HDCPMBX_Write2Byte(DEF_HDCPMBX_SARG_OFFSET + DEF_HDCPMBX_COMMON_BASE, u16ArgSize);

                //set AArg;
                HDCPMBX_Write4Byte(DEF_HDCPMBX_AARG_OFFSET + DEF_HDCPMBX_COMMON_BASE, ((u32ArgAddr & 0xFFFF)|(DEF_SECU_IMI_BASE_ADDR))); //IMI: Hk- 0x9000 0000, Secu-0x8000 0000

                _HDCPMBX_TriggerInt(EN_HDCPMBX_CMD_HOST_HKR2);
            }
            else
            {
                HLOGWC(EN_HLOG_HDCP,  DEF_COLOR_YELLOW,  "Wait Previous Process");
            }
        }

    } while(FALSE);

    return bRet;
}

void HDCPMBX_Init(void)
{
    BYTE u8PortCnt = 0;
    BYTE u8ByteCnt = 0;

    //clear command slot;
    for (u8PortCnt = 0; u8PortCnt < DEF_HDCP_PORT_NUM; u8PortCnt++)
    {
        //clear mailbox bank from address 0x08;
        HDCPMBX_ClearCmd(u8PortCnt);
    }

    //clear common command slot;
    for (u8ByteCnt = 0; u8ByteCnt < sizeof(ST_HDCPMBX_COMMAND); u8ByteCnt++)
    {
        HDCPMBX_WriteByte(DEF_HDCPMBX_CMD_HEADER_OFFSET + DEF_HDCPMBX_COMMON_BASE + u8ByteCnt, 0x00);
    }
}

//#endif //#if (ENABLE_SECU_R2 == 1) && (ENABLE_HDCP22 == 1)
#endif //#ifndef HDCPMBX_C
