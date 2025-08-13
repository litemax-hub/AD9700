#ifndef HDCP_LOG_C
#define HDCP_LOG_C

//----------------------------------------------------------------------//
// includes
//----------------------------------------------------------------------//
#include "stdio.h"
#include <string.h>
#include <stdarg.h>
#include "datatype.h"
#include "drvGlobal.h"
#include "HdcpCommon.h"
#include "HdcpLog.h"
#include "Utl.h"

//----------------------------------------------------------------------//
// global
//----------------------------------------------------------------------//
static ENUM_HDCPLOG_PRIORITY g_enHdcp1xLogPriority = EN_HLOG_PRIORITY_SILENT;//[MT9701]EN_HLOG_PRIORITY_SILENT;//EN_HLOG_PRIORITY_INFO;//EN_HLOG_PRIORITY_INFO;
static ENUM_HDCPLOG_PRIORITY g_enHdcp2xLogPriority = EN_HLOG_PRIORITY_SILENT;//[MT9701]EN_HLOG_PRIORITY_SILENT;//EN_HLOG_PRIORITY_INFO;//EN_HLOG_PRIORITY_INFO;
void HDCP_PRINT(ULONG u32Priority, BYTE u8Version, const char* pu8Tag, const char* pu8Color, const char* pu8FnName, ULONG u32Line, const char* pu8Format, ...);
//----------------------------------------------------------------------//
// functions
//----------------------------------------------------------------------//
void HDCP_PRINT(ULONG u32Priority, BYTE u8Version, const char* pu8Tag, const char* pu8Color, const char* pu8FnName, ULONG u32Line, const char* pu8Format, ...)
{
    ULONG u32LogPriority = 0;
    char u8Msg[2048] = {0};
    va_list args;

    u32LogPriority = (u8Version > 0) ? g_enHdcp2xLogPriority : g_enHdcp1xLogPriority;
    if (u32Priority < u32LogPriority)
    {
        return;
    }

    va_start(args, pu8Format);
    if(vsnprintf(u8Msg, sizeof(u8Msg), pu8Format, args) < 0)
    {
        return;
    }

    va_end(args);

    printf("%s[%s] %s ((%s:%d))%s\r\n", pu8Color, pu8Tag, u8Msg, pu8FnName, (int)u32Line, DEF_COLOR_NONE);

}

void HDCP_PRINT_BYTE(ULONG u32Priority, BYTE u8Version, const char* pu8Format, ...)
{
    ULONG u32LogPriority = 0;
    char u8Msg[1024] = {0};
    va_list args;

    u32LogPriority = (u8Version > 0) ? g_enHdcp2xLogPriority : g_enHdcp1xLogPriority;
    if (u32Priority < u32LogPriority)
    {
        return;
    }

    va_start(args, pu8Format);
    if(vsnprintf(u8Msg, sizeof(u8Msg), pu8Format, args) < 0)
    {
        return;
    }
    //vsprintf(u8Msg, pu8Format, args);
    va_end(args);

    printf("%s", u8Msg);
}

void HDCPLOG_Hdcp1XSetLogPriority(int priority)
{
    g_enHdcp1xLogPriority = (ENUM_HDCPLOG_PRIORITY)priority;
    HDCP_PRINT(EN_HLOG_PRIORITY_SILENT, EN_HLOG_HDCP, DEF_HDCP_TAG, DEF_COLOR_NONE, __FUNCTION__, __LINE__, "g_enHdcp1xLogPriority: %d", g_enHdcp1xLogPriority);
}

void HDCPLOG_Hdcp2XSetLogPriority(int priority)
{
    g_enHdcp2xLogPriority = (ENUM_HDCPLOG_PRIORITY)priority;
    HDCP_PRINT(EN_HLOG_PRIORITY_SILENT, EN_HLOG_HDCP2X, DEF_HDCP2X_TAG, DEF_COLOR_NONE, __FUNCTION__, __LINE__, "g_enHdcp2xLogPriority: %d", g_enHdcp2xLogPriority);
}

void HDCPLOG_DumpHexValue(BYTE u8Version, unsigned char *pu8HexVal, unsigned int nLen)
{

    unsigned int i = 0;
    unsigned char u8HexBuf[64] = {'\0'};
    unsigned char u8Tmp[4] = {'\0'};

    do
    {
        if (pu8HexVal == NULL || nLen == 0)
        {
            break;
        }

        for (i=0; i<nLen; ++i)
        {
            if (snprintf((char *)u8Tmp,sizeof(u8Tmp), "%2X ", pu8HexVal[i]) < 0)
            {
                return;
            }
            //sprintf((char *)u8Tmp, "%2X ", pu8HexVal[i]);
            strcat((char *)u8HexBuf, (char *)u8Tmp);
            if (i != 0 && ((i%16) == 15 || i==(nLen-1)))
            {
                HLOGEC(u8Version, DEF_COLOR_LIGHT_PURPLE, "%s", u8HexBuf);
                memset(u8HexBuf, '\0', sizeof(u8HexBuf));
            }
        }
    } while (FALSE);
}
#endif //#ifndef HDCP_LOG_C
