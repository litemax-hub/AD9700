

#define DEFAULT_PRODUCTION_WEEK             1
#define DEFAULT_PRODUCTION_YEAR             (2023-1990)

#define DEFAULT_EDID_SN1_ADDRESS            0x0C

#define DEFAULT_ID_SERIAL_NUM_LENGTH        4
#define DEFAULT_PRODUCT_SERIAL_NUM_LENGTH   13

#define EDID_DISP_DESCRIPTOR_ADDR_1         0x36
#define EDID_DISP_DESCRIPTOR_ADDR_2         0x48
#define EDID_DISP_DESCRIPTOR_ADDR_3         0x5A
#define EDID_DISP_DESCRIPTOR_ADDR_4         0x6C
#define EDID_DISP_DESCRIPTOR_SIZE           0x12
#define EDID_DISP_DESCRIPTOR_HEADER_SIZE    0x05

typedef struct
{
    BYTE u8IDSerialNum[4];
    BYTE u8ProductSerialNum[13];
    BYTE u8ProductSerialNumLength;
}EDID_SERIAL_NUM_INFO;

typedef enum
{
    eEdidDesc_Manufacturer_Reserved0,
    eEdidDesc_Manufacturer_Reserved1,
    eEdidDesc_Manufacturer_Reserved2,
    eEdidDesc_Manufacturer_Reserved3,
    eEdidDesc_Manufacturer_Reserved4,
    eEdidDesc_Manufacturer_Reserved5,
    eEdidDesc_Manufacturer_Reserved6,
    eEdidDesc_Manufacturer_Reserved7,
    eEdidDesc_Manufacturer_Reserved8,
    eEdidDesc_Manufacturer_Reserved9,
    eEdidDesc_Manufacturer_ReservedA,
    eEdidDesc_Manufacturer_ReservedB,
    eEdidDesc_Manufacturer_ReservedC,
    eEdidDesc_Manufacturer_ReservedD,
    eEdidDesc_Manufacturer_ReservedE,
    eEdidDesc_Manufacturer_ReservedF,
    eEdidDesc_Dummy                     =0x10,
    eEdidDesc_AdditionStandardTiming    =0xF7,
    eEdidDesc_Cvt3ByteTimingCode        =0xF8,
    eEdidDesc_DisplayColorManagement    =0xF9,
    eEdidDesc_DisplayName               =0xFC,
    eEdidDesc_DisplayRangeLimits        =0xFD,
    eEdidDesc_UnspecifiedText           =0xFE,
    eEdidDesc_DisplaySerialNumber       =0xFF,
}EDID_DESCRIPTOR_TYPE;


extern const BYTE gu8DefaultIdSerialNum[DEFAULT_ID_SERIAL_NUM_LENGTH];;
extern const BYTE gu8DefaultProductSerialNum[DEFAULT_PRODUCT_SERIAL_NUM_LENGTH];

extern void CustomEDID_SetProductionWeek(BYTE u8ProductionWeek);
extern void CustomEDID_SetProductionYear(BYTE u8ProductionYear);
extern void CustomEDID_SetCustomSN(BYTE *pIDSerialNum, BYTE *pProductSerialNum, BYTE u8ProductSerialNumLength);
extern void CustomEDID_ResetBaseBlockChecksum(BYTE * pEDID);
extern void CustomEDID_ReplaceEDIDCustomData(BYTE *pEDID);

extern void CustomEDID_UpdateEDID(BYTE u8InputPort);
extern void CustomEDID_InitInternalEDID(void);

