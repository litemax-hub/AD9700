#ifndef _DRVBDMA_H_
#define _DRVBDMA_H_

#include "Global.h"

typedef enum
{
    CHANNEL_AUTO,
    CHANNEL_0,
    CHANNEL_1,
} BDMA_CHANNEL;

typedef enum
{
	DW_1BYTE = 0x00,
	DW_2BYTE = 0x10,
	DW_4BYTE = 0x20,
	DW_8BYTE = 0x30,
	DW_16BYTE = 0x40,
}BDMA_DATA_WIDTH;

#if (CHIP_ID == CHIP_MT9701)
typedef enum
{
    SOURCE_MIU0 = 0,
    SOURCE_MIU1 = 1,
    SOURCE_MEMORY_FILL = 4,
    SOURCE_SPI = 5,
    SOURCE_HKR2_IQM = 7,
    SOURCE_HKR2_DQM = 8,
    SOURCE_SECR2_IQM = 9,
    SOURCE_SECR2_DQM = 10,
}BDMA_SOURCE_TYPE;

typedef enum
{
    DEST_MIU0 = 0,
    DEST_MIU1 = 1,
    DEST_PATTERN_SEARCH = 2,
    DEST_CRC32= 3,
    DEST_PD51_PSRAM = 6,
    DEST_HKR2_IQMEM = 7,
    DEST_HKR2_DQMEM = 8,
    DEST_SECR2_IQMEM = 9,
    DEST_SECR2_DQMEM = 10,
}BDMA_DESTINATION_TYPE;
#else
typedef enum
{
    SOURCE_MIU0 = 0,
    SOURCE_MIU1 = 1,
    SOURCE_MEMORY_FILL = 4,
    SOURCE_SPI = 5,
    SOURCE_R2_IQM = 8,
    SOURCE_R2_DQM = 9,
}BDMA_SOURCE_TYPE;

typedef enum
{
    DEST_MIU0 = 0,
    DEST_MIU1 = 1,
    DEST_PATTERN_SEARCH = 2,
    DEST_CRC32= 3,
    DEST_VDMCU = 6,
    DEST_PD51_PSRAM = 7,
    DEST_SECR2_IQMEM = 8,
    DEST_SECR2_DQMEM = 9,
    DEST_HK51_PSRAM = 10,
}BDMA_DESTINATION_TYPE;
#endif

typedef enum
{
    DOWNLOAD_CODE,
    PATTERN_FILL,
    PATTERN_SEARCH,
} BDMA_CMD;

#define BDMA_SEARCH_ALL_MATCHED (0)
#define BDMA_CRC32_POLY         (0x04C11DB7)
#define BDMA_CRC16_POLY         (0x8005)
#define BDMA_CRC_SEED_0         (0)
#define BDMA_CRC_SEED_F         (0xFFFFFFFF)

extern DWORD BDMA_Operation(BYTE ch, BYTE SourceId, BYTE DestinId,DWORD dwSourceAddr, DWORD dwDestinAddr, DWORD dwByteCount, DWORD dwPattern, DWORD dwExternPattern);
U32 crc32(U32 crc, const void *buf, size_t size);

#endif
