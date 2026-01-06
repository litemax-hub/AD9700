#ifndef _DEBUG_H_
#define  _DEBUG_H_

#ifdef _DEBUG_C_
  #define _DEBUGDEC_
#else
  #define _DEBUGDEC_ extern
#endif

#include "board.h"

#define CABLE_DETECT_DEBUG_ENABLE          	0
#define ADC_KEY_PRESS_DEBUG_ENABLE	        0
#define DDCCI_DEBUG_ENABLE              	0
#define DEBUG_SCANKEY       				0
#define DEBUG_OSD                           0
// use to find timing change first point during sync stable to sync loss
#define ENABLE_DEBUG_TIMING_CHANGE_POINT    0

_DEBUGDEC_ void putSIOChar( unsigned char sendData );
_DEBUGDEC_ BOOL ProcessCommand( void );
_DEBUGDEC_ Bool GetCommand( void );
_DEBUGDEC_ Bool ProcessCommandExt(void);
_DEBUGDEC_ void printData_UART1(char * str, WORD value);
_DEBUGDEC_ void printMsg_UART1(char * str);
_DEBUGDEC_ void putSIOChar_UART1(unsigned char sendData);

_DEBUGDEC_ void putSIOChar_UART1(unsigned char sendData);
_DEBUGDEC_ void ProcessCommand_UART1( void );
_DEBUGDEC_ Bool GetCommand_UART1( void );
_DEBUGDEC_ void UART1_Handler(void);

#if EnableSerialPortTXInterrupt
#define BufIdx(x, shift)   ((x >= shift)?((x - shift)):(x + _UART_TX_BUFFER_LENGTH_ - shift))
_DEBUGDEC_ void uart_set_polling(BYTE u8TrueFlag);
_DEBUGDEC_ void uart_tx_initial(void);
_DEBUGDEC_ void uart_tx_increase_send_cnt(void);
_DEBUGDEC_ BYTE check_uart_tx_buffer_empty(void);
#endif

#if ENABLE_MSTV_UART_DEBUG_PIU
_DEBUGDEC_ void MDrv_UART_PIU_DebugInit(void);
#endif

#define SIO_ID1     SIORxBuffer[0]
#define SIO_TYPE1   SIORxBuffer[1]
#define SIO_TYPE2   SIORxBuffer[2]
#define SIO_CMD1    SIORxBuffer[3]
#define SIO_CMD2    SIORxBuffer[4]
#define SIO_DATA1   SIORxBuffer[5]
#define SIO_DATA2   SIORxBuffer[6]
#define SIO_DATA3       SIORxBuffer[7]
#define SIOData10   (SIO_DATA1-0x30)*256
#define SIOData11   ((SIO_DATA1-0x41)+10)*256
#define SIOData20   (SIO_DATA2-0x30)*16
#define SIOData21   ((SIO_DATA2-0x41)+10)*16
#define SIOData30   (SIO_DATA3-0x30)
#define SIOData31   (SIO_DATA3-0x41)+10

#if ENABLE_DEBUG_TIMING_CHANGE_POINT
extern bit bTimingChangeFirstPointFlag;
extern BYTE xdata TimingChangeFirstPoint;
typedef enum
{
    TimingChange_Def = 0,
    TimingChange_ISR_Vchange = 1,
    TimingChange_ISR_Hchange = 2,
    TimingChange_ISR_Vdisappear = 3,
    TimingChange_ISR_Hdisappear = 4,
    TimingChange_ISR_CLKchange = 5,
    TimingChange_Hperoid = 6,
    TimingChange_Vtotal = 7,
    TimingChange_TMDS_DE = 8,
    TimingChange_SyncPol = 9,
    TimingChange_SrcHPeriod = 10,
    TimingChange_SrcVTotal = 11,
    TimingChange_SrcTMDSWidth = 12,
    TimingChange_SrcTMDSHeight = 13,
    TimingChange_SrcTMDSHStart = 14,
    TimingChange_SrcTMDSVStart = 15,
    TimingChange_FindMode = 16,
    TimingChange_CaptureWindow = 17,
    TimingChange_SetupMode = 18,
    TimingChange_HDMIModeChange = 19,
    TimingChange_DPColorFormatChange = 20,
}TimingChangeFirstPointType;
extern void ResetTimingChangeFirstPoint(void);
extern void SetTimingChangeFirstPoint(TimingChangeFirstPointType firstpoint);
#endif

#endif
