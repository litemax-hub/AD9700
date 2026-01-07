#define _COMMON_C_
#include "board.h"
#include "types.h"
#include "DebugDef.h"
#include "Global.h"
#include "ms_reg.h"
#include "Ms_rwreg.h"
#include "misc.h"
#include "Debug.h"
#include "Common.h"

#if ENABLE_UART_PIU
#include "drvUARTPIU.h"
#endif

#if ENABLE_MSTV_UART_DEBUG

#if ENABLE_MSTV_UART_DEBUG_PIU
#define _UART_PUTC_(_X_)     piu_uart_putc(_X_);
#else
#define _UART_PUTC_(_X_)     putSIOChar(_X_);
#endif

void printMsg(char *str)
{
#if 1
    printf(str);
    putchar( 0xd );
    putchar(( char )'\n' );
#else
    char data sendData;

    #if !ENABLE_MSTV_UART_DEBUG_PIU
    {
#if 0 // Leo-temp
        if(!UART_READ_ES())//(!ES)
            return;
#endif
    }
    #endif

    sendData = *(str++);

    while(sendData)   //!='\0')
    {
        _UART_PUTC_((unsigned char)sendData);
        sendData = *(str++);
    }

    _UART_PUTC_(0xd);
    _UART_PUTC_((char)'\n');
#endif
}

void printData(char *str, WORD value)
{
#if 1
    printf(str, value);
    putchar( 0xd );
    putchar(( char )'\n' );
#else
    char data sendData;

    #if !ENABLE_MSTV_UART_DEBUG_PIU
    {
#if 0 // Leo-temp
        if(!UART_READ_ES())//(!ES)
            return;
#endif
    }
    #endif

    sendData = Send_MsgValue;

    while(sendData = *(str++))
    {
        if(sendData == (char)'%')   // %
        {
            sendData = *(str++);
            if(sendData == (char)'d' || sendData == (char)'x')   // d
            {
                if(value)
                {
                    Bool data noneZero = FALSE;
                    WORD data divider = 10000;
                    char data dispValue;
                    if(sendData == (char)'x')
                    {
                        divider = 0x1000;
                    }
                    while(divider)
                    {
                        dispValue = value / divider;
                        value -= dispValue * divider;
                        if(dispValue)
                        {
                            noneZero = TRUE;
                        }
                        if(noneZero)
                        {
                            if(dispValue > 9)
                            {
                                dispValue += 55;
                            }
                            else
                            {
                                dispValue += 0x30;
                            }

                            _UART_PUTC_(dispValue);
                        }
                        if(sendData == (char)'d')
                        {
                            divider /= 10;
                        }
                        else
                        {
                            divider /= 0x10;
                        }
                    }
                }
                else
                {
                    _UART_PUTC_('0');
                }
            }
            else if(sendData == (char)'s')
            {
                BYTE xdata *sPtr = (BYTE xdata *) value;
                while(*sPtr)
                    _UART_PUTC_(*sPtr++);
                break;
            }
        }
        else
        {
            _UART_PUTC_(sendData);
        }
    }

    _UART_PUTC_(0xd);
    _UART_PUTC_((char)'\n');
#endif
}
#endif
