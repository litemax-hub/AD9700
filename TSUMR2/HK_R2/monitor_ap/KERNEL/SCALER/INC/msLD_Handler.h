#ifndef _DRVLDALGORITHM_H_
#define _DRVLDALGORITHM_H_

#if ENABLE_LOCALDIMMING

#define LD_H_COUNT  0x10
#define LD_V_COUNT  0x01
#define LD_REGION_COUNT (LD_H_COUNT*LD_V_COUNT)

#define LD_DRIVE_NUM    1
#define LD_DRIVE_AMS    0
#define LD_DRIVE_O2     1
#define PWM_DRIVE  2
#define LD_DRIVE_TYPE     LD_DRIVE_O2
#define LD_CUSTOM_DELAY_TIME_EN     0   //for AMS

#define LD_SPI_TRANSFER_TIME                3
#define LD_SPI_TRIGGER_DELAY                1 // unit: ms

#if (CHIP_ID == CHIP_MT9701)
#define LD_SF_STRENGTH_1    0xFF
#define LD_SF_STRENGTH_2    0xFF
#define LD_SF_STRENGTH_3    0xFF
#define LD_SF_STRENGTH_4    0xFF
#define LD_SF_STRENGTH_5    0xFF
#endif

typedef struct
{
    BYTE LDMean;
    BYTE LDMax;
    BYTE LDMin;
    BYTE LDSP;
}LDRegionReport;

typedef enum _LD_TYPE
{
LD_LOCAL_MODE,
LD_GLOBAL_MODE,
LD_GLOBAL_DIMMING_MODE,
LD_CUSTOM_MODE,
LD_GLOBAL_DIMMING_MODE_OFF,
LD_TYPE_NUM

}LD_TYPE;

extern void msLDInit(void);
extern void msLDSetRegionCount(BYTE hCount, BYTE vCount);
//extern BOOL msLDSWGetHistogramReport(BYTE u8RegionCount, LDRegionReport *pRegionReport);
extern BOOL msLDGetHistogramReport(BYTE u8RegionCount, LDRegionReport *pRegionReport);
extern void msLDHandler(void);
extern void msSetLDMinMax(WORD u16Min, WORD u16Max);

extern void msLD_SetMode(LD_TYPE type);
extern void msLD_SetGlobalDimming(BYTE u8Bri);
extern void msLD_Isr(BOOL bVsync);

extern xdata float g_fRegionAlphaBlending; 
extern MS_U32 tf_strength_dn ;
extern MS_U32 tf_strength_up ;
extern xdata BYTE g_AlphaBlending;
extern MS_U16 n_leds;
extern void msSetLDDebugMsg(BOOL IsEnDebugMsg);
extern xdata BYTE g_LD_Mode;

void MDrv_LDAlgo_SpatialFilter(const MS_U16* IN_blocks, MS_U16* OUT_blocks);

// h41[7:0] reg_dummy7
extern void MHal_LD_Set_SW_SpatialFilterStrength1(U8 u8Str);
extern U8   MHal_LD_Get_SW_SpatialFilterStrength1(void);
// h41[15:8] reg_dummy7
extern void MHal_LD_Set_SW_SpatialFilterStrength2(U8 u8Str);
extern U8   MHal_LD_Get_SW_SpatialFilterStrength2(void);
// h42[7:0] reg_dummy8
extern void MHal_LD_Set_SW_SpatialFilterStrength3(U8 u8Str);
extern U8   MHal_LD_Get_SW_SpatialFilterStrength3(void);
// h42[15:8] reg_dummy8
extern void MHal_LD_Set_SW_SpatialFilterStrength4(U8 u8Str);
extern U8   MHal_LD_Get_SW_SpatialFilterStrength4(void);
// h43[7:0] reg_dummy9
extern void MHal_LD_Set_SW_SpatialFilterStrength5(U8 u8Str);
extern U8   MHal_LD_Get_SW_SpatialFilterStrength5(void);


#endif

#endif

