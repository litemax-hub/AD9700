#ifndef _MISC_H
#define _MISC_H

extern void _nop_(void);
extern void Delay4us( void );
extern void Delay1ms( WORD msNums );
extern void ForceDelay1ms( WORD msNums );
extern void ForceDelay1ms_Pure( WORD msNums );
extern BYTE GetVSyncTime( void );
extern void DELAY_NOP(BYTE msNums);
extern DWORD GetSortMiddleNumber(DWORD* arr, int len);
extern void msDrvSAREnableSwitch(WORD u16Ch,Bool bEnable);
extern BYTE msReadSARData(BYTE ch);
extern WORD msReadSARData_10bit(BYTE ch);
extern void BootTimeStamp_Set(BYTE type, BYTE index, BOOL firstTimeSet);
extern MS_U32 BootTimeStamp_Get(BYTE type, BYTE index);
extern void BootTimeStamp_Clr(void);
#endif

