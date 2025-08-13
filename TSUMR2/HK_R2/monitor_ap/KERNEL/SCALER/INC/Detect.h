#ifndef _DETECT_H_
#define _DETECT_H_

extern void mStar_MonitorInputTiming( void );
extern void mStar_ModeHandler( void );
extern Bool mStar_ValidTimingDetect( void );
//extern Bool mStar_IsStatusChanged( void );
extern Bool IsCableNotConnected( void );
extern WORD GetImageHeight(void);
extern WORD GetImageWidth(void);
extern void DetectDummy(void);
extern Bool msCheckFakeSleep(void);
extern BYTE GetVSyncWidth(void);
#endif
extern void msAPI_CheckAgainOnThd_Set(WORD u16CheckAgainOnThd);
extern WORD msAPI_CheckAgainOnThd_Get(void);
extern void msAPI_FakeSleepTimeOutCnt_Set(MS_S16 u16TimeOutCnt);
extern MS_S16 msAPI_FakeSleepTimeOutCnt_Get(void);
extern void msAPI_FakeSleepTimeOutThd_Set(WORD u16TimeOutThd);
extern WORD msAPI_FakeSleepTimeOutThd_Get(void);
extern void msAPI_FakeSleepTimeOutEn_Set(BOOL bEn);
extern BOOL msAPI_FakeSleepTimeOutEn_Get(void);
