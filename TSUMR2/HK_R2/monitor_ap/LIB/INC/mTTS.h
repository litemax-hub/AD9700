
#ifndef _MTTS_H_
#define _MTTS_H_


typedef struct
{
    DWORD u32BufSt;
    DWORD u32BufEnd;
    DWORD u32StrSts;
} ST_AUDIO_TTS_MBX;


typedef struct
{
    DWORD u32BufAddrSt;
    DWORD u32BufAddrEnd;
    DWORD u32InfoAddr;
    ST_AUDIO_TTS_MBX sMbxReg;
} ST_AUDIO_TTS_ADDR;


typedef struct // Need to sync w/ PQ_R2
{
    BOOL bUserEnable;
    BOOL bUserMute;
    BOOL bMix;
    BYTE u8SysMute;
    BYTE u8ScaleAudio; // x/16, x = 0~16
    BYTE u8ScaleTTS; // x/16, x = 0~15
    WORD u16Speed;
} ST_AUDIO_TTS_INFO;


extern BOOL mDrvAudioTTSIsSupport(void);
extern void mDrvAudioTTSInfoToPQR2Init(ST_AUDIO_TTS_ADDR sInfoAddr, WORD u16StrMaxSize);
extern BOOL mDrvAudioTTSPlay(char *strText);
extern void mDrvAudioTTSMixScale(BYTE u8Audio, BYTE u8TTS);
extern void mDrvAudioTTSSpeed(WORD u16Speed);
extern void mDrvAudioTTSMixEnable(BOOL bEnable);
extern void mDrvAudioTTSUserEnable(BOOL bEnable);
extern void mDrvAudioTTSUserMute(BOOL bEnable);
extern void mDrvAudioTTSSysMute(BYTE u8Val);


#endif

