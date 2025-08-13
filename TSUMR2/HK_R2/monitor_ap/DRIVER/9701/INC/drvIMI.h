#ifndef DRV_IMI_H
#define DRV_IMI_H

#include "HdcpCommon.h"
#include "HdcpHandler.h"
#include "msFB.h"
//----------------------------------------------------------------------//
// IMI MAP
//----------------------------------------------------------------------//
#define DEF_IMI_BASE_ADDR           0x40000000
#define DEF_IMI_START_ADDR          0x00
#define DEF_IMI_SIZE                0xEE400 // 953KB

#define DEF_IMI_HDCP_START_ADDR     DEF_IMI_START_ADDR
#define DEF_IMI_HDCP_SIZE           0x1800  // 6KB

#define DEF_IMI_ML_START_ADDR       (DEF_IMI_HDCP_START_ADDR + DEF_IMI_HDCP_SIZE)
#define DEF_IMI_ML_SIZE             0x800   // 2KB

#define DEF_IMI_ADL_START_ADDR      (DEF_IMI_ML_START_ADDR + DEF_IMI_ML_SIZE)
#define DEF_IMI_ADL_SIZE            0x4000  // 16KB

#define DEF_IMI_OD_START_ADDR       (DEF_IMI_ADL_START_ADDR + DEF_IMI_ADL_SIZE)
#define DEF_IMI_OD_SIZE             0xE8000 // 928KB

#define DEF_IMI_USB_START_ADDR      (DEF_IMI_BASE_ADDR + DEF_IMI_OD_START_ADDR) // USB & OD share the same space. Before USB F/W download, OD should be off/mask first.
#define DEF_IMI_USB_SIZE            DEF_IMI_OD_SIZE

#if 1
//+++++++++++++++++++++++++++++++++++++++++++++
/* FILE_SYSTEM_POOL & DOWNLOAD_BUFFER   */
#define MIU0                        (0x0000)
#define MIU1                        (0x0001)

#define SW                          (0x0000 << 1)
#define HW                          (0x0001 << 1)
#define SW_HW                       (0x0002 << 1)

#define UNCACHED                    (0x0000 << 3)
#define WRITE_THROUGH               (0x0001 << 3)
#define WRITE_COMBINING             (0x0002 << 3)
#define WRITE_PROTECT               (0x0003 << 3)
#define WRITE_BACK                  (0x0004 << 3)
#define MIU_INTERVAL                0x0000000000

/* USB_HOST_DRIVER_BUFFER   */
#define USB_HOST_BUFFER_AVAILABLE                   MIU_USBBUF_ADDR_START
#define USB_HOST_BUFFER_ADR                         (MIU_USBBUF_ADDR_START | 0x80000000)
#define USB_HOST_BUFFER_GAP_CHK                     0x0000000000
#define USB_HOST_BUFFER_LEN                         0x00006000     // 24KB  //0x0000060000
#define USB_HOST_BUFFER_MEMORY_TYPE                 (MIU0 | SW | WRITE_BACK)

/* FILE_SYSTEM_POOL   */
#define FILE_SYSTEM_POOL_AVAILABLE                  ((DWORD)(USB_HOST_BUFFER_AVAILABLE+USB_HOST_BUFFER_LEN))
#define FILE_SYSTEM_POOL_ADR               	        ((DWORD)((USB_HOST_BUFFER_AVAILABLE+USB_HOST_BUFFER_LEN) | 0x80000000))
#define FILE_SYSTEM_POOL_GAP_CHK                    0x0000000000
#define FILE_SYSTEM_POOL_LEN                        0x00050000     // 320KB  //0x0000060000
#define FILE_SYSTEM_POOL_MEMORY_TYPE                (MIU0 | SW | WRITE_BACK)

/* USB_DOWNLOAD_BUFFER   */
#define DOWNLOAD_BUFFER_AVAILABLE                   (DWORD)(FILE_SYSTEM_POOL_AVAILABLE+FILE_SYSTEM_POOL_LEN))
#define DOWNLOAD_BUFFER_ADR                         ((DWORD)((FILE_SYSTEM_POOL_AVAILABLE+FILE_SYSTEM_POOL_LEN) | 0x80000000))
#define DOWNLOAD_BUFFER_GAP_CHK                     0x0000000000
#define DOWNLOAD_BUFFER_LEN                         (MIU_USBBUF_SIZE- USB_HOST_BUFFER_LEN - FILE_SYSTEM_POOL_LEN)
#define DOWNLOAD_BUFFER_MEMORY_TYPE                 (MIU0 | SW | WRITE_BACK)
#endif


//----------------------------------------------------------------------//
// proto-type
//----------------------------------------------------------------------//
BYTE* IMI_GetIMIRegs(DWORD u32Addr);
void IMI_WriteByte(DWORD u32Offset, BYTE u8Data);
void IMI_Write2Bytes(DWORD u32Offset, WORD u16Data);
BYTE IMI_ReadByte(DWORD u32Offset);
void IMI_WriteBytes(DWORD u32Addr, BYTE *pu8Data, DWORD u32DataLen);
void IMI_ReadBytes(DWORD u32Addr, BYTE *pu8Data, DWORD u32DataLen);
void IMI_Init(DWORD u32BaseAddr, DWORD u32Size);
#if DEF_HDCP2RX_ISR_MODE
void MEM_MSWRITE_2BYTE( DWORD u32RegLoAddr, WORD u16Value );
void MEM_EXT0MSWRITE_2BYTE( DWORD u32RegLoAddr, WORD u16Value );
//void mcuISR_4kXdataMapToDRAMIMI(DWORD dwADDR, BYTE ucDestSel);
//void mcuISREXT0_4kXdataMapToDRAMIMI(DWORD dwADDR, BYTE ucDestSel);
void IMI_ISR_WriteByte(DWORD u32Offset, BYTE u8Data);
void IMI_ISREXT0_WriteByte(DWORD u32Offset, BYTE u8Data);
BYTE IMI_ISR_ReadByte(DWORD u32Offset);
BYTE IMI_ISREXT0_ReadByte(DWORD u32Offset);
void IMI_ISR_WriteBytes(DWORD u32Addr, BYTE *pu8Data, DWORD u32DataLen);
void IMI_ISR_ReadBytes(DWORD u32Addr, BYTE *pu8Data, DWORD u32DataLen);
void IMI_ISREXT0_ReadBytes(DWORD u32Addr, BYTE *pu8Data, DWORD u32DataLen);
#endif

#endif
