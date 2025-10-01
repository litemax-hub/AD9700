
#ifndef _sysem_eDPTx_H_
#define _sysem_eDPTx_H_

//==============================================================================
//
//                              COMPILER OPTION
//
//==============================================================================

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================
//==============================================================================
//
//                              VARIABLES
//
//==============================================================================



//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

void System_eDPTx_Init_main(void);
void System_eDPTx_Training(void);
void System_eDPTx_PowerOnCheck(void);
void System_eDPTx_PowerDown(void);
void System_eDPTx_SignalOff(void);
void System_eDPTx_TestCommand(void);
void System_eDPTx_Handler(void);
void System_eDPTx_FineTuneTU_Disable(void);
void System_eDPTx_FineTuneTU(DWORD eDPTX_OUTBL_PixRateKhz);

#endif
