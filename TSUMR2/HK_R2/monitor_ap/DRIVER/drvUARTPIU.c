////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
#include <math.h>
#include "drvUARTPIU.h"
#include "misc.h"
#include "halRwreg.h"
#include "MsOS.h"


#if ENABLE_UART_PIU
void uart_write_byte(BYTE u8Addr, BYTE u8Val)
{
    DWORD data u32Reg = (UART_BASE | (u8Addr * 2));
    msRegs(u32Reg) = u8Val;
}

BYTE uart_read_byte(BYTE u8Addr)
{
    DWORD data u32Reg = (UART_BASE | (u8Addr * 2));
    DWORD data ucdata = 0;

    ucdata = msRegs(u32Reg);
    return ucdata;
}

BYTE uart_isr_read_byte(BYTE u8Addr)
{
    DWORD data u32Reg = (UART_BASE | (u8Addr * 2));
    DWORD data ucdata = 0;

    ucdata = msRegs(u32Reg);

    return ucdata;
}

void piu_uart_init(UART_CLK_PIU e_piu_clock, DWORD uart_clock)
{
    DWORD piu_clock = 0;
    WORD divisor, tmp, timeout_counter = 0x0FFF;

    //Disable clock gating, using 12Mhz as default clock
    msWriteByte(REG_00038C, 0);
    
    // Disable all interrupts
    piu_uart_set_rx_callback(NULL);

    uart_write_byte(PIU_UART_IER, 0x00);

    // wait for TX FIFO clear
    for(tmp = 0; tmp < (250 * 32); tmp++)//delay 32ms
    {                    
        Delay4us();
    }

    // piu clock switch
    if( e_piu_clock == CLK_PIU_216M )
    {
        piu_clock = 216000000;
    }
    else if( e_piu_clock == CLK_PIU_172M )
    {
        piu_clock = 172000000;
    }
    else if( e_piu_clock == CLK_PIU_144M )
    {
        piu_clock = 144000000;
    }
    else if( e_piu_clock == CLK_PIU_108M )
    {
        piu_clock = 108000000;
    }
    else if( e_piu_clock == CLK_PIU_86M )
    {
        piu_clock = 86000000;
    }
    else if( e_piu_clock == CLK_PIU_54M )
    {
        piu_clock = 54000000;
    }
    else if( e_piu_clock == CLK_PIU_FRO12 )
    {
        piu_clock = 12000000;
    }
    else if( e_piu_clock == CLK_PIU_12M )
    {
        piu_clock = 12000000;
    }
    msWriteByte(REG_00038C, (e_piu_clock << 2));

    // Set "reg_mcr_loopback";
    uart_write_byte(PIU_UART_MCR, (uart_read_byte(PIU_UART_MCR) | 0x10));

    // Disable FIFO Buffer
    uart_write_byte(PIU_UART_FCR, 0);

    // Poll "reg_usr_busy" till 0;
    while((uart_read_byte(PIU_UART_USR) & 0x01) && timeout_counter)
        timeout_counter--;

    // Enable, Clear FIFO Buffer
    uart_write_byte(PIU_UART_FCR, (UART_FCR_ENABLE_FIFO | UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT | UART_FCR_TRIGGER_1));

    // Set 8 bit char, 1 stop bit, no parity
    uart_write_byte(PIU_UART_LCR, (UART_LCR_WLEN8 & ~(UART_LCR_STOP2 | UART_LCR_PARITY)));

    // Set baudrate
    divisor = ((piu_clock) / (16 * uart_clock));

    uart_write_byte(PIU_UART_LCR, (uart_read_byte(PIU_UART_LCR) | UART_LCR_DLAB));
    uart_write_byte(PIU_UART_DLL, (divisor & 0xFF));
    uart_write_byte(PIU_UART_DLM, ((divisor >> 8) & 0xFF));
    uart_write_byte(PIU_UART_LCR, (uart_read_byte(PIU_UART_LCR) & ~(UART_LCR_DLAB)));

    // Clear "reg_mcr_loopback";
    uart_write_byte(PIU_UART_MCR, (uart_read_byte(PIU_UART_MCR) & ~0x10));
}

void piu_uart_putc(char c)
{
    while (!(uart_read_byte(PIU_UART_LSR) & UART_LSR_THRE));
        uart_write_byte(PIU_UART_TX, c);
}

/*char piu_uart_getc(void)
{
    while (!(uart_read_byte(PIU_UART_LSR) & UART_LSR_DR));
    return (char)(uart_read_byte(PIU_UART_RX));
}

char piu_uart_kbhit(void)
{
    if (uart_read_byte(PIU_UART_LSR) & UART_LSR_DR)
        return 1;
    else
        return 0;

}
void piu_uart_interrupt_enable(Bool bEnable)
{
    if (bEnable)
        uart_write_byte(PIU_UART_IER, UART_IER_RDI);     // Receive data available
    else
        uart_write_byte(PIU_UART_IER, 0);                //Receive data disable
}*/
    
//#define rx_buf_len 256
//char piu_uart_rx_buf[rx_buf_len] = {0};
void (*piu_rx_callback)(BYTE c);

void piu_uart_isr (void)
{
    BYTE c;
    BYTE iir;
    BYTE usr;
    
    iir = uart_isr_read_byte(PIU_UART_IIR);
    usr = uart_isr_read_byte(PIU_UART_USR);
    if (!(iir & UART_IIR_NO_INT))
    {
        switch (iir & UART_IIR_ID)
        {
            case UART_IIR_RDI:
                while (uart_isr_read_byte(PIU_UART_LSR) & UART_LSR_DR)
                {
                    c = uart_isr_read_byte(PIU_UART_RX);
                    #if 0
                    if (piu_uart_rx_buf)
                    {
                        piu_uart_rx_buf[u->rx_buf_in++] = c;
                        if (u->rx_buf_in >= u->rx_buf_len)
                        {
                            u->rx_buf_in = 0;
                        }
                    }
                    #endif
                    if(piu_rx_callback)
                    {
                        (piu_rx_callback)(c);
                    }
                }
                // fall through
            break;

            #if 0
            case UART_IIR_THRI:
                if (u->tx_buf_in == u->tx_buf_out)
                {
                    u->tx_flags |= TX_FLAG_DIRECT;
                }
                else
                {
                    while ((AEON_REG8(UART_LSR) & UART_LSR_THRE)
                        && (u->tx_buf_in != u->tx_buf_out))
                    {
                        c = u->tx_buf[u->tx_buf_out++];
                        if (!(u->tx_flags & TX_FLAG_DISABLED))
                        {
                            AEON_REG8(UART_TX) = c;
                            if (u->tx_buf_out >= u->tx_buf_len)
                            {
                                u->tx_buf_len = 0;
                            }
                        }
                    }
                }
            break;
            #endif
        }
    }

    MsOS_EnableInterrupt(E_INT_PM_IRQ_PM_UART0);    
}

//Call back function and drvUARTPIU.c should be in the same bank.
BYTE piu_uart_set_rx_callback(void (*rx_cb)(BYTE c))
{
    if (rx_cb)
    {
        piu_rx_callback = rx_cb;
        MsOS_AttachInterrupt(E_INT_PM_IRQ_PM_UART0, (InterruptCb)piu_uart_isr);
        MsOS_EnableInterrupt(E_INT_PM_IRQ_PM_UART0);
        uart_write_byte(PIU_UART_IER, (uart_read_byte(PIU_UART_IER) | UART_IER_RDI));
    }
    else
    {
        MsOS_DisableInterrupt(E_INT_PM_IRQ_PM_UART0);
        uart_write_byte(PIU_UART_IER, (uart_read_byte(PIU_UART_IER) & ~UART_IER_RDI));
        MsOS_DetachInterrupt(E_INT_PM_IRQ_PM_UART0);
        piu_rx_callback = NULL;
    }
    return 0;
}

void piu_print_hex(unsigned long num)
{
    int i;
    U32 digit;

    piu_uart_putc('0');
    piu_uart_putc('x');
    for (i=7; i>=0; i--)
    {
        digit = (num>>(4*i))&0xF;
        if (digit >9)
            piu_uart_putc(digit-10+'A');
        else
            piu_uart_putc(digit+'0');
    }
    piu_uart_putc('\r');
    piu_uart_putc('\n');
}
void piu_rx_test(BYTE c)
{
    piu_uart_putc('\r');
    piu_uart_putc('\n');
    piu_print_hex(c);
}

void piu_uart_test(void)
{
    piu_uart_init(CLK_PIU_172M, UART_PIU_BAUDRATE);
    MsOS_DelayTask(10);
    mcuSetUartMux(UART_ENGINE_DW_UART, UART_PIU_GPIO_PORT);
    MsOS_DelayTask(10);

    int i;
    for (i=0; i<5; i++)
    {
        piu_uart_putc('\r');
        piu_uart_putc('\n');
        piu_uart_putc('0' + i);
        piu_uart_putc('H');
        piu_uart_putc('E');
        piu_uart_putc('L');
        piu_uart_putc('L');
        piu_uart_putc('O');
        piu_uart_putc('_');
        piu_uart_putc('F');
        piu_uart_putc('R');
        piu_uart_putc('O');
        piu_uart_putc('M');
        piu_uart_putc('_');
        piu_uart_putc('P');
        piu_uart_putc('I');
        piu_uart_putc('U');
        piu_uart_putc('\r');
        piu_uart_putc('\n');
        MsOS_DelayTask(100);
    }

    piu_uart_set_rx_callback(piu_rx_test);
}



#endif
