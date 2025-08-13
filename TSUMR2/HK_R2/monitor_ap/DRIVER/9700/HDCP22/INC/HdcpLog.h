#ifndef HDCP_LOG_H
#define HDCP_LOG_H

//----------------------------------------------------------------------//
// defines
//----------------------------------------------------------------------//
#define DEF_HDCP_TAG                "Hdcp"
#define DEF_HDCP2X_TAG              "Hdcp2X"

#define DEF_COLOR_NONE              "\033[m"
#define DEF_COLOR_RED               "\033[0;32;31m"
#define DEF_COLOR_LIGHT_RED         "\033[1;31m"
#define DEF_COLOR_GREEN             "\033[0;32;32m"
#define DEF_COLOR_LIGHT_GREEN       "\033[1;32m"
#define DEF_COLOR_BLUE              "\033[0;32;34m"
#define DEF_COLOR_LIGHT_BLUE        "\033[1;34m"
#define DEF_COLOR_DARY_GRAY         "\033[1;30m"
#define DEF_COLOR_CYAN              "\033[0;36m"
#define DEF_COLOR_LIGHT_CYAN        "\033[1;36m"
#define DEF_COLOR_PURPLE            "\033[0;35m"
#define DEF_COLOR_LIGHT_PURPLE      "\033[1;35m"
#define DEF_COLOR_BROWN             "\033[0;33m"
#define DEF_COLOR_YELLOW            "\033[1;33m"
#define DEF_COLOR_LIGHT_GRAY        "\033[0;37m"
#define DEF_COLOR_WHITE             "\033[1;37m"

#define HDCP_DEBUG 0
//[MT9700]#define HDCP_DumpLog 0
//[MT9700]#define HDCPHTRACE_DEBUG 0

#if (HDCP_DEBUG && ENABLE_MSTV_UART_DEBUG)
#define HDCP_printData(str, value)  	printData(str, value)
#define HDCP_printMsg(str)			printMsg(str)
#else
#define HDCP_printData(str, value)
#define HDCP_printMsg(str)
#endif

void HDCP_PRINT(ULONG u32Priority, BYTE u8Version, const char* pu8Tag, const char* pu8Color, const char* pu8FnName, ULONG u32Line, const char* pu8Format, ...);
void HDCP_PRINT_BYTE(ULONG u32Priority, BYTE u8Version, const char* pu8Format, ...);

typedef enum
{
    EN_HLOG_HDCP = 0,
    EN_HLOG_HDCP2X,
}ENUM_HDCPLOG_TYPE_TAG;

typedef enum
{
    EN_HLOG_PRIORITY_UNKNOWN = 0,
    EN_HLOG_PRIORITY_DEFAULT,    /* only for SetMinPriority() */
    EN_HLOG_PRIORITY_VERBOSE,
    EN_HLOG_PRIORITY_INFO,
    EN_HLOG_PRIORITY_WARN,
    EN_HLOG_PRIORITY_DEBUG,
    EN_HLOG_PRIORITY_ERROR,
    EN_HLOG_PRIORITY_FATAL,
    EN_HLOG_PRIORITY_SILENT,
} ENUM_HDCPLOG_PRIORITY;

// output log msg with specific color and newline
#define HLOGVC(ver, color, fmt, ...)    HDCP_PRINT(EN_HLOG_PRIORITY_VERBOSE, ver, (ver > 0) ? DEF_HDCP2X_TAG : DEF_HDCP_TAG, color, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define HLOGIC(ver, color, fmt, ...)    HDCP_PRINT(EN_HLOG_PRIORITY_INFO   , ver, (ver > 0) ? DEF_HDCP2X_TAG : DEF_HDCP_TAG, color, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define HLOGWC(ver, color, fmt, ...)    HDCP_PRINT(EN_HLOG_PRIORITY_WARN   , ver, (ver > 0) ? DEF_HDCP2X_TAG : DEF_HDCP_TAG, color, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define HLOGDC(ver, color, fmt, ...)    HDCP_PRINT(EN_HLOG_PRIORITY_DEBUG  , ver, (ver > 0) ? DEF_HDCP2X_TAG : DEF_HDCP_TAG, color, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define HLOGEC(ver, color, fmt, ...)    HDCP_PRINT(EN_HLOG_PRIORITY_ERROR  , ver, (ver > 0) ? DEF_HDCP2X_TAG : DEF_HDCP_TAG, color, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define HLOGFC(ver, color, fmt, ...)    HDCP_PRINT(EN_HLOG_PRIORITY_FATAL  , ver, (ver > 0) ? DEF_HDCP2X_TAG : DEF_HDCP_TAG, color, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

// output log msg without color and with newline
#define HLOGV(ver, fmt, ...)            HDCP_PRINT(EN_HLOG_PRIORITY_VERBOSE, ver, (ver > 0) ? DEF_HDCP2X_TAG : DEF_HDCP_TAG, DEF_COLOR_NONE, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define HLOGI(ver, fmt, ...)            HDCP_PRINT(EN_HLOG_PRIORITY_INFO   , ver, (ver > 0) ? DEF_HDCP2X_TAG : DEF_HDCP_TAG, DEF_COLOR_NONE, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define HLOGW(ver, fmt, ...)            HDCP_PRINT(EN_HLOG_PRIORITY_WARN   , ver, (ver > 0) ? DEF_HDCP2X_TAG : DEF_HDCP_TAG, DEF_COLOR_NONE, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define HLOGD(ver, fmt, ...)            HDCP_PRINT(EN_HLOG_PRIORITY_DEBUG  , ver, (ver > 0) ? DEF_HDCP2X_TAG : DEF_HDCP_TAG, DEF_COLOR_NONE, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define HLOGE(ver, fmt, ...)            HDCP_PRINT(EN_HLOG_PRIORITY_ERROR  , ver, (ver > 0) ? DEF_HDCP2X_TAG : DEF_HDCP_TAG, DEF_COLOR_NONE, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define HLOGF(ver, fmt, ...)            HDCP_PRINT(EN_HLOG_PRIORITY_FATAL  , ver, (ver > 0) ? DEF_HDCP2X_TAG : DEF_HDCP_TAG, DEF_COLOR_NONE, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

// output log msg for dump bytes by bytes
#define HLOGVD(ver, fmt, ...)           HDCP_PRINTF_BYTE(EN_HLOG_PRIORITY_VERBOSE, ver, fmt, ##__VA_ARGS__)
#define HLOGID(ver, fmt, ...)           HDCP_PRINTF_BYTE(EN_HLOG_PRIORITY_INFO   , ver, fmt, ##__VA_ARGS__)
#define HLOGWD(ver, fmt, ...)           HDCP_PRINTF_BYTE(EN_HLOG_PRIORITY_WARN   , ver, fmt, ##__VA_ARGS__)
#define HLOGDD(ver, fmt, ...)           HDCP_PRINTF_BYTE(EN_HLOG_PRIORITY_DEBUG  , ver, fmt, ##__VA_ARGS__)
#define HLOGED(ver, fmt, ...)           HDCP_PRINTF_BYTE(EN_HLOG_PRIORITY_ERROR  , ver, fmt, ##__VA_ARGS__)
#define HLOGFD(ver, fmt, ...)           HDCP_PRINTF_BYTE(EN_HLOG_PRIORITY_FATAL  , ver, fmt, ##__VA_ARGS__)

// special usage
#define HTRACEE(ver) HLOGV(ver, "Enter %s() function", __FUNCTION__);
#define HTRACEL(ver) HLOGV(ver, "Leave %s() function", __FUNCTION__);

//----------------------------------------------------------------------//
// function proto-type
//----------------------------------------------------------------------//
void HDCPLOG_Hdcp1XSetLogPriority(int priority);
void HDCPLOG_Hdcp2XSetLogPriority(int priority);
void HDCPLOG_DumpHexValue(BYTE u8Version, unsigned char *pu8HexVal, unsigned int nLen);
#endif //#ifndef HDCP_LOG_H
