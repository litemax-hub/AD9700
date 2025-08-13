#ifndef __DDCMCCSMSCHANDLER_H_
#define __DDCMCCSMSCHANDLER_H_
//Manufacturer Specific Controls, MSC
//Enable/Disable MTK MSC Commands
BOOL mapi_MCCS_MSC_GetEnabled(void);
void mapi_MCCS_MSC_SetEnabled(BOOL u8Enabled);

//Check VCP code is MSC (E0~FF)
BOOL mapi_MCCS_MSC_IsValid(BYTE u8OpCode);
BYTE mapi_MCCS_MSC_Handler(BYTE* pDDCBuf, BYTE u8DftDDCLen, BYTE u8VCPCode, BYTE u8OpCode, BYTE u8SH, BYTE u8SL, WORD u16Value);

//Select MSC Page Index, defaul index is orignal page.
BYTE mapi_MCCS_MSC_GetPageIndex(void);
void mapi_MCCS_MSC_SetPageIndex(BYTE u8PageIdx);

//Install call back funciton for orignal page MSC VCP Code E4~FF, E0~E3 VCP code is fixed for vendor info.
typedef BYTE (*fpMSCHandler)(BYTE* pDDCBuf, BYTE u8DftDDCLen, BYTE u8VCPCode, BYTE u8OpCode, BYTE u8SH, BYTE u8SL, WORD u16Value);
void mapi_MCCS_MSC_Install(fpMSCHandler fp);
#endif
