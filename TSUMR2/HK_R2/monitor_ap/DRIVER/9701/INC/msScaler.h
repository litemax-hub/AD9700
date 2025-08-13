
#ifndef _MS_SCALER_H_
#define _MS_SCALER_H_

#ifdef _MS_SCALER_C_
#define INTERFACE
#else
#define INTERFACE   extern
#endif



#define IS_MRW_FMT_LA()         ((g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_SBSH) || \
                                (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_SBSF) || \
                                (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_LA) )

#define IS_FMT_IN_SBS()         ((g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_SBSH) || \
                                (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_SBSF) )

#define IS_VSP_FMT_2IMG()       (((g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FPI) ||  \
                                 (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FA) ||  \
                                 (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FP) ||  \
                                 (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_PF)) &&  \
                                 ((g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_3) ||  \
                                 (g_SetupPathInfo.ucSCPathMode == SC_PATH_MODE_4) ))

#define IS_FMT_OUT_PSV()        ((g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV) || \
                                (g_SetupPathInfo.ucSCFmtOut == SC_FMT_OUT_3D_PSV_4M) )

#define IS_FMT_IN_CB_PI()         ((g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_CB) || \
                                (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_PI) )

#define SC_LOCK_LINE            8
#define HSC_offset              1
#define SC_V_SC_NON_MINUS1MODE  0
#define SC_H_SC_NON_MINUS1MODE  0
#define FIELD_NUM               3

enum
{
    // MRW in/out src sel
    IP2MRW2VSP =  ((0x00)|(0x01<<2)),
    HSP2MRW2VSP = ((0x02)|(0x01<<2)),
    VSP2MRW2HSP = ((0x01)|(0x00<<2)),
    VSP2MRW2OP1 = ((0x01)|(0x02<<2)),
    IP2MRW2HSP =((0x00)|(0x00<<2)),
    HSP2MRW2OP1 =((0x02)|(0x02<<2)),

    // SCMI in/out src sel
    IP2SCMI2VSP =  ((0x00)|(0x01<<4)),
    HSP2SCMI2VSP = ((0x02)|(0x01<<4)),
    VSP2SCMI2HSP = ((0x01)|(0x00<<4)),
    VSP2SCMI2OP1 = ((0x01)|(0x02<<4)),
    IP2SCMI2HSP =((0x00)|(0x00<<4)),
    HSP2SCMI2OP1 =((0x02)|(0x02<<4)),

    // HSP/VSP/FIFO in/out src sel
    FIFO_VSP_HSP_FIFO = ((0x0D)|(0x01<<4)|(0x00<<6)|(0x00<<8)|(0x01<<10)|(0x00<<12)|(0x01<<14)),
    FIFO_HSP_VSP_FIFO = ((0x0D)|(0x00<<4)|(0x01<<6)|(0x01<<8)|(0x00<<10)|(0x01<<12)|(0x00<<14)),
    FIFO_HSP_MRW_VSP_FIFO = ((0x0C)|(0x00<<4)|(0x02<<6)|(0x02<<8)|(0x00<<10)|(0x01<<12)|(0x00<<14)),
    FIFO_VSP_MRW_HSP_FIFO = ((0x0C)|(0x02<<4)|(0x00<<6)|(0x00<<8)|(0x02<<10)|(0x00<<12)|(0x01<<14)),
    FIFO_MRW_HSP_VSP_FIFO = ((0x0C)|(0x02<<4)|(0x01<<6)|(0x01<<8)|(0x00<<10)|(0x01<<12)|(0x02<<14)),
    FIFO_MRW_VSP_HSP_FIFO = ((0x0C)|(0x01<<4)|(0x00<<6)|(0x02<<8)|(0x01<<10)|(0x00<<12)|(0x02<<14)),
    FIFO_VSP_HSP_MRW_FIFO = ((0x0C)|(0x01<<4)|(0x02<<6)|(0x00<<8)|(0x01<<10)|(0x02<<12)|(0x01<<14)),
    FIFO_HSP_VSP_MRW_FIFO = ((0x0C)|(0x00<<4)|(0x01<<6)|(0x01<<8)|(0x02<<10)|(0x02<<12)|(0x00<<14)),

};


//Jison:110428 scWrite3Byte() ucReg was change to mean low addr.
#define MRW_L_WR_MSB_0          0;//SC6_10//SC6_12
#define MRW_L_WR_MSB_1          0;//SC6_13//SC6_15
#define MRW_L_WR_MSB_2          0;//SC6_16//SC6_18
#define MRW_L_WR_MSB_3          0;//SC6_19//SC6_1B
#define MRW_L_WR_MSB_4          0;//SC6_1C//SC6_1E
#define MRW_L_WR_MSB_5          0;//SC6_1F//SC6_21
#define MRW_L_WR_MSB_6          0;//SC6_22//SC6_24
#define MRW_L_WR_MSB_7          0;//SC6_25//SC6_27
#define MRW_R_WR_MSB_0          0;//SC6_28//SC6_2A
#define MRW_R_WR_MSB_1          0;//SC6_2B//SC6_2D
#define MRW_R_WR_MSB_2          0;//SC6_2E//SC6_30
#define MRW_R_WR_MSB_3          0;//SC6_31//SC6_33
#define MRW_R_WR_MSB_4          0;//SC6_34//SC6_36
#define MRW_R_WR_MSB_5          0;//SC6_37//SC6_39
#define MRW_R_WR_MSB_6          0;//SC6_3A//SC6_3C
#define MRW_R_WR_MSB_7          0;//SC6_3D//SC6_3F
                                
#define MRW_L_WR_LSB_0          0;//SC6_40//SC6_42
#define MRW_L_WR_LSB_1          0;//SC6_43//SC6_45
#define MRW_L_WR_LSB_2          0;//SC6_46//SC6_48
#define MRW_L_WR_LSB_3          0;//SC6_49//SC6_4B
#define MRW_L_WR_LSB_4          0;//SC6_4C//SC6_4E
#define MRW_L_WR_LSB_5          0;//SC6_4F//SC6_51
#define MRW_L_WR_LSB_6          0;//SC6_52//SC6_54
#define MRW_L_WR_LSB_7          0;//SC6_55//SC6_57
#define MRW_R_WR_LSB_0          0;//SC6_58//SC6_5A
#define MRW_R_WR_LSB_1          0;//SC6_5B//SC6_5D
#define MRW_R_WR_LSB_2          0;//SC6_5E//SC6_60
#define MRW_R_WR_LSB_3          0;//SC6_61//SC6_63
#define MRW_R_WR_LSB_4          0;//SC6_64//SC6_66
#define MRW_R_WR_LSB_5          0;//SC6_67//SC6_69
#define MRW_R_WR_LSB_6          0;//SC6_6A//SC6_6C
#define MRW_R_WR_LSB_7          0;//SC6_6D//SC6_6F
                                
#define MRW_L_RD_LSB_0          0;//SC6_70//SC6_72
#define MRW_L_RD_LSB_1          0;//SC6_73//SC6_75
#define MRW_L_RD_LSB_2          0;//SC6_76//SC6_78
#define MRW_L_RD_LSB_3          0;//SC6_79//SC6_7B
#define MRW_L_RD_LSB_4          0;//SC6_7C//SC6_7E
#define MRW_L_RD_LSB_5          0;//SC6_7F//SC6_81
#define MRW_L_RD_LSB_6          0;//SC6_82//SC6_84
#define MRW_L_RD_LSB_7          0;//SC6_85//SC6_87
#define MRW_L_RD_MSB_0          0;//SC6_88//SC6_8A
#define MRW_L_RD_MSB_1          0;//SC6_8B//SC6_8D
#define MRW_L_RD_MSB_2          0;//SC6_8E//SC6_90
#define MRW_L_RD_MSB_3          0;//SC6_91//SC6_93
#define MRW_L_RD_MSB_4          0;//SC6_94//SC6_96
#define MRW_L_RD_MSB_5          0;//SC6_97//SC6_99
#define MRW_L_RD_MSB_6          0;//SC6_9A//SC6_9C
#define MRW_L_RD_MSB_7          0;//SC6_9D//SC6_9F
                                
//Jison:110428 scWrite2Byte() ucReg was change to mean low addr.
#define MRW_L_MSB_OFFSET_LINE   0;//SC6_E4//SC6_E5
#define MRW_L_LSB_OFFSET_LINE   0;//SC6_E6//SC6_E7
#define MRW_R_MSB_OFFSET_LINE   0;//SC6_E8//SC6_E9
#define MRW_R_LSB_OFFSET_LINE   0;//SC6_EA//SC6_EB
#define MRW_L_MSB_OFFSET_ACT    0;//SC6_EC//SC6_ED
#define MRW_L_LSB_OFFSET_ACT    0;//SC6_EE//SC6_EF
#define MRW_R_MSB_OFFSET_ACT    0;//SC6_F0//SC6_F1
#define MRW_R_LSB_OFFSET_ACT    0;//SC6_F2//SC6_F3


typedef struct
{
    WORD wImgSizeInH;       /* Input L or R Image Size */
    WORD wImgSizeInV;
    WORD wImgSizeOutH;      /* Output L or R Image Size */
    WORD wImgSizeOutV;
    WORD wMRWSizeInH;
    WORD wMRWSizeInV;
    WORD wMRWSizeOutH;
    WORD wMRWSizeOutV;
    BYTE ucSCFmtIn;
    BYTE ucSCFmtOut;
    BYTE ucMemFormat;
    BYTE ucFieldNum;
    WORD wFieldMode;

    BYTE ucSCPathMode;
    BYTE ucIPLRSel;
    BYTE ucOut3DLRSel;

    FSyncMode ucFrameLockMode;

    BYTE bOverrideSCFmtIn:1;
    BYTE bPsvPnlOutLRInvert:1;
    BYTE bScalingUpH:1;
    BYTE bScalingUpV:1;
    BYTE bHSDEnable:1;
    BYTE bMRWDual:1;
    BYTE bMRW422:1;
    BYTE bMRWR2Y:1;

    BYTE bMRWLLRR:1;
    BYTE bVBIExtend:1;
    BYTE bFBMode:1;

    BYTE b2DTo3DEnable:1;
    BYTE bLoadOdTbl:1;
    BYTE bHMirrorMode:1;
    BYTE bVFlipMode:1;
    BYTE bOverrideSCPathMode:1;
    Bool b3DTo2DEnable:1;
    Bool bReadCurFrame:1;
} SetupPathInfo;

enum
{
    SC_PATH_MODE_0,         /* Frame bufferless */
    SC_PATH_MODE_1,
    SC_PATH_MODE_2,
    SC_PATH_MODE_3,
    SC_PATH_MODE_4,
    SC_PATH_MODE_5,
    SC_PATH_MODE_6,
};

enum
{
    SC_FMT_IN_NORMAL,
    SC_FMT_IN_3D_PF,        /* page flipping */
    SC_FMT_IN_3D_FP,        /* frame packing */
    SC_FMT_IN_3D_FPI,       /* frame packing interlace */
    SC_FMT_IN_3D_TB,        /* top bottom */
    SC_FMT_IN_3D_SBSH,      /* side by side half */
    SC_FMT_IN_3D_LA,        /* line interleave */
    SC_FMT_IN_3D_SBSF,      /* side by side full */
    SC_FMT_IN_3D_CB,        /* chess board */
    SC_FMT_IN_3D_FA,        /* filed alternative */
    SC_FMT_IN_3D_PI,        /* pixel interleave */
};
enum
{
    SC_FMT_OUT_3D_FS,       /* frame sequential */
    SC_FMT_OUT_3D_PSV,      /* passive */
    SC_FMT_OUT_3D_PSV_4M,   /* 4M embedded passive */
    SC_FMT_OUT_3D_TB,       /* top and bottom */
    SC_FMT_OUT_3D_SBS,      /* side by side */
    SC_FMT_OUT_3D_CB,   /* chess board */
};

enum
{
    OUT_3D_FIXED_NORMAL,
    OUT_3D_FIXED_L,
    OUT_3D_FIXED_R,
};

enum{
    MEM_FMT_NO_USE,
    MEM_FMT_422,
    MEM_FMT_444_8,
    MEM_FMT_444_10,
    MEM_FMT_VC_10to8,
    MEM_FMT_VC_10to6
};

INTERFACE void mStar_SetupPathInit(void);
INTERFACE void mStar_SetupPath(void);
INTERFACE void mStar_SetScalingFilter( void );
INTERFACE void mStar_SetScalingFactor( void );
INTERFACE void mStar_AdjustContrast( BYTE contrast );
INTERFACE void mStar_SetMemFreeze( BOOL bEnable );
INTERFACE BOOL mStar_FrameBufferDump( DWORD u32DestAddress, size_t u32DestSize, BOOL bIsPixelOrder, BYTE u8ColorFormat );
INTERFACE void mStar_FblCBAttach( BOOL (*fpFblOv)(void) );
INTERFACE BOOL mStar_FblOv(void);
INTERFACE BOOL mStar_FrameBufferSizeCheck(void);
extern void mStar_WaitForDataBlanking(void);
extern xdata SetupPathInfo g_SetupPathInfo;


#undef INTERFACE
#endif

