
#ifndef _DRV3DLUT_C_H_
#define _DRV3DLUT_C_H_
#define ENABLE_3DLUT_Test           1
#define ENABLE_3DLUT_AutoDownload   (1 && ENABLE_LUT_AUTODOWNLOAD)

typedef enum
{
    e3dLutDelta1X = 0,
    e3dLutDelta2X,
    e3dLutDelta4X,
    e3dLutDelta8X
} _3dLutDeltaEnum;

#if ENABLE_3DLUT_Test

extern void drv3DLutWriteReadTest(void);
#endif

//extern void ms3DLutEnable(BYTE window, BYTE enable);
extern void drv3DLutInit(void);

extern void mStar_Setup3DLUT(void);

#endif

