
#ifndef _DRV3DLUT_C_H_
#define _DRV3DLUT_C_H_
#define ENABLE_3DLUT_AutoDownload 1

typedef enum
{
    e3dLutDelta1X = 0,
    e3dLutDelta2X,
    e3dLutDelta4X,
    e3dLutDelta8X
} _3dLutDeltaEnum;

extern void drv3DLutInit(void);

extern void mStar_Setup3DLUT(void);

extern void ms3DLutGrayGuard(BYTE enable);
extern void ms3DLutEnable(BYTE ucSrcIdx, BYTE enable);
extern void drv3DLut_WriteS7Table(BYTE u8WinIndex);
extern void drv3DLut_WriteS9ColorFmtTable( BYTE u8WinIndex, BOOL bEnable);
extern void msSet3DLUT_Tbl(WORD* pTbl);
#endif

