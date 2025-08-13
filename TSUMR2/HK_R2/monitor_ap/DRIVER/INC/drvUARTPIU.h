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
#ifndef _DRVUARTPIU_H
#define _DRVUARTPIU_H

#include "types.h"
#include "Ms_rwreg.h"

#define PIU_UART_RX        (0 * 2)  // In:  Receive buffer (DLAB=0)
#define PIU_UART_TX        (0 * 2)  // Out: Transmit buffer (DLAB=0)
#define PIU_UART_DLL       (0 * 2)  // Out: Divisor Latch Low (DLAB=1)
#define PIU_UART_DLM       (1 * 2)  // Out: Divisor Latch High (DLAB=1)
#define PIU_UART_IER       (1 * 2)  // Out: Interrupt Enable Register
#define PIU_UART_IIR       (2 * 2)  // In:  Interrupt ID Register
#define PIU_UART_FCR       (2 * 2)  // Out: FIFO Control Register
#define PIU_UART_LCR       (3 * 2)  // Out: Line Control Register
#define PIU_UART_MCR       (4 * 2)  // Out: Modem Control Register
#define PIU_UART_LSR       (5 * 2)  // In:  Line Status Register
#define PIU_UART_MSR       (6 * 2)  // In:  Modem Status Register
#define PIU_UART_USR       (7 * 2)  // Out: USER Status Register

//
// FIFO Control Register
//
#define UART_FCR_ENABLE_FIFO        0x01    // Enable the FIFO
#define UART_FCR_CLEAR_RCVR         0x02    // Clear the RCVR FIFO
#define UART_FCR_CLEAR_XMIT         0x04    // Clear the XMIT FIFO
#define UART_FCR_DMA_SELECT         0x08    // For DMA applications
#define UART_FCR_TRIGGER_MASK       0xC0    // Mask for the FIFO trigger range
#define UART_FCR_TRIGGER_1          0x00    // Mask for trigger set at 1
#define UART_FCR_TRIGGER_4          0x40    // Mask for trigger set at 4
#define UART_FCR_TRIGGER_8          0x80    // Mask for trigger set at 8
#define UART_FCR_TRIGGER_14         0xC0    // Mask for trigger set at 14

//
// Line Control Register
// Note: if the word length is 5 bits (UART_LCR_WLEN5), then setting
// UART_LCR_STOP will select 1.5 stop bits, not 2 stop bits.
//
#define UART_LCR_DLAB               0x80    // Divisor latch access bit
#define UART_LCR_SBC                0x40    // Set break control
#define UART_LCR_SPAR               0x20    // Stick parity (?)
#define UART_LCR_EPAR               0x10    // Even parity select
#define UART_LCR_PARITY             0x08    // Parity Enable
#define UART_LCR_STOP1              0x00    // Stop bits: 0=1 stop bit, 1= 2 stop bits
#define UART_LCR_STOP2              0x04    // Stop bits: 0=1 stop bit, 1= 2 stop bits
#define UART_LCR_WLEN5              0x00    // Wordlength: 5 bits
#define UART_LCR_WLEN6              0x01    // Wordlength: 6 bits
#define UART_LCR_WLEN7              0x02    // Wordlength: 7 bits
#define UART_LCR_WLEN8              0x03    // Wordlength: 8 bits

//
// Line Status Register
//
#define UART_LSR_TEMT               0x40    // Transmitter empty
#define UART_LSR_THRE               0x20    // Transmit-hold-register empty
#define UART_LSR_BI                 0x10    // Break interrupt indicator
#define UART_LSR_FE                 0x08    // Frame error indicator
#define UART_LSR_PE                 0x04    // Parity error indicator
#define UART_LSR_OE                 0x02    // Overrun error indicator
#define UART_LSR_DR                 0x01    // Receiver data ready

//
// Interrupt Identification Register
//
#define UART_IIR_NO_INT             0x01    // No interrupts pending
#define UART_IIR_ID                 0x06    // Mask for the interrupt ID

#define UART_IIR_MSI                0x00    // Modem status interrupt
#define UART_IIR_THRI               0x02    // Transmitter holding register empty
#define UART_IIR_TOI                0x0c    // Receive time out interrupt
#define UART_IIR_RDI                0x04    // Receiver data interrupt
#define UART_IIR_RLSI               0x06    // Receiver line status interrupt

//
// Interrupt Enable Register
//
#define UART_IER_MSI                0x08    // Enable Modem status interrupt
#define UART_IER_RLSI               0x04    // Enable receiver line status interrupt
#define UART_IER_THRI               0x02    // Enable Transmitter holding reg empty int
#define UART_IER_RDI                0x01    // Enable receiver data available interrupt

//
// Modem Control Register
//
#define UART_MCR_LOOP               0x10    // Enable loopback test mode
#define UART_MCR_OUT2               0x08    // Out2 complement
#define UART_MCR_OUT1               0x04    // Out1 complement
#define UART_MCR_RTS                0x02    // RTS complement
#define UART_MCR_DTR                0x01    // DTR complement

//
// Modem Status Register
//
#define UART_MSR_DCD                0x80    // Data Carrier Detect
#define UART_MSR_RI                 0x40    // Ring Indicator
#define UART_MSR_DSR                0x20    // Data Set Ready
#define UART_MSR_CTS                0x10    // Clear to Send
#define UART_MSR_DDCD               0x08    // Delta DCD
#define UART_MSR_TERI               0x04    // Trailing edge ring indicator
#define UART_MSR_DDSR               0x02    // Delta DSR
#define UART_MSR_DCTS               0x01    // Delta CTS
#define UART_MSR_ANY_DELTA          0x0F    // Any of the delta bits!

// both Transmitter empty / Transmit-hold-register empty
#define BOTH_EMPTY (UART_LSR_TEMT | UART_LSR_THRE)

//-------------------------------------------------------------------------------------------------
// Defines
//-------------------------------------------------------------------------------------------------
#define UART_BASE              0x001D00  //0x100980//PIU Uart BK1009_40

typedef enum
{
    CLK_PIU_12M,
    CLK_PIU_FRO12,
    CLK_PIU_54M,
    CLK_PIU_86M,
    CLK_PIU_108M,
    CLK_PIU_144M,
    CLK_PIU_172M,
    CLK_PIU_216M
} UART_CLK_PIU;

#if ENABLE_UART_PIU
void piu_uart_init(UART_CLK_PIU e_piu_clock, DWORD uart_clock);
void piu_uart_putc(char c);
BYTE piu_uart_set_rx_callback(void (*rx_cb)(BYTE c));
void piu_uart_isr (void);
#endif

#endif //#ifndef _DRVUART_H
