///////////////////////////////////////////////////////////////////////////////
/// @file CustomEDID.c
/// @brief Functions for custom project
/// @author Mediatek Inc.
///
///
///
/// Features
///  - 
///  - 
///
///////////////////////////////////////////////////////////////////////////////

#include "types.h"
#include "Common.h"
#include "Global.h"
#include "Utl.h"

#define CUSTOM_FUNC_DEBUG    1
#if ENABLE_MSTV_UART_DEBUG && CUSTOM_FUNC_DEBUG
#define CUSTOM_FUNC_printData(str, value)   printData(str, value)
#define CUSTOM_FUNC_printMsg(str)           printMsg(str)
#else
#define CUSTOM_FUNC_printData(str, value)
#define CUSTOM_FUNC_printMsg(str)
#endif



