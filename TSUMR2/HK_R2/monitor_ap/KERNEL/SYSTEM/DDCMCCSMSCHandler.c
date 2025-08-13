#include "datatype.h"
#include "DDCMCCSMSCHandler.h"
#include "imginfo.h"
#include "SysInit.h"
#include "Utl.h"
#include "msflash.h"
#include "drvUSBDwld.h"

enum
{
    MCCS_GET_VCP = 0x01,
    MCCS_SET_VCP = 0x03,
};

enum
{
    MSC_GET_VENDORID_OUI_L = 0xE0,
    MSC_GET_VENDORID_OUI_H = 0xE1,
    MSC_GET_VENDORMCCS_VERSION = 0xE2,
    MSC_RESERVED = 0xE3,
    MSC_FW_CTRL = 0xE4,
    MSC_SET_START_ADDR_L = 0xE5,
    MSC_SET_START_ADDR_H = 0xE6,
    MSC_END_ADDR_L = 0xE7,
    MSC_END_ADDR_H = 0xE8,
    MSC_GET_BUF_LEN = 0xE9,
    MSC_BUF_WORD = 0xEA,
    MSC_BUF_TABEL = 0xEB,
    MSC_GET_EXE_STATUS = 0xEC,
    MSC_CRC = 0xED,
};

enum
{
    MSC_FWCTRL_OPTYPE_RESERVED = 0x00,
    MSC_FWCTRL_OPTYPE_ERASE = 0x01,
    MSC_FWCTRL_OPTYPE_WRITE = 0x02,
    MSC_FWCTRL_OPTYPE_READ = 0x03,
};

enum
{
    MSC_FWCTRL_INIT = 0x00,
    MSC_FWCTRL_OPTRIGGER = 0x01, //00 reserved, 01 erase, 02 write, 03 read
    MSC_FWCTRL_REBOOT = 0x02,
    MSC_FWCTRL_IMGADDR = 0x03,
    MSC_FWCTRL_HWVER = 0x04,
    MSC_FWCTRL_FWVER = 0x05,
    MSC_FWCTRL_USBCFG = 0x06, //01 host, 02 device
};

enum
{
    MSC_FWCTRL_STA_IDLE = 0x00,
    MSC_FWCTRL_STA_ERASE_FAIL = 0x01,
    MSC_FWCTRL_STA_WRITE_FAIL = 0x02,
    MSC_FWCTRL_STA_READ_FAIL = 0x03,
    MSC_FWCTRL_STA_BUSY = 0xBB,
    MSC_FWCTRL_STA_SUCCESS = 0xCC,
};

enum 
{
    MSC_FWCTRL_USBMODE_RESERVED = 0x00,
    MSC_FWCTRL_USBMODE_HOST = 0x01,
    MSC_FWCTRL_USBMODE_DEVICE = 0x02,
    MSC_FWCTRL_USBMODE_READCFG = 0x03,
    MSC_FWCTRL_USBMODE_EXIT = 0x04,
};

enum 
{
    MSC_FWCTRL_PAGE_ORIGINAL = 0x00,
    MSC_FWCTRL_PAGE_FWUPD = 0x01,    
};

typedef struct
{
    //Mem Config
    DWORD memAddr;   //DRAM mapping address, defalut : 0x87E00000;
    //buffer
    BYTE* pBuffer;            //Allocated buffer;

} FWFileConfig; //for tool send a file to FW

static const DWORD _MTK_MSC_OUI = 0x00000CE7;
static const DWORD _HWVER = (HW_MODEL << 16) | HW_VERSION ;
static const DWORD _FWVER = (AP_SW_MODEL << 16) | AP_SW_VERSION;
static BYTE _FWCtrlCode = MSC_FWCTRL_INIT;
static DWORD _u32MSCFWStartAddr = 0;
static DWORD _u32MSCFWEndAddr = 0;
static DWORD _u32MSCBurstRWCnt = 0;
static WORD _u16CRC = 0;
static BYTE _u8FwOpStatus = MSC_FWCTRL_STA_IDLE;
static WORD _u8MSCVersion = 0x00;
static BYTE _u8MSCUSBMode = MSC_FWCTRL_USBMODE_RESERVED;
static const WORD _16MSCBufLen = 4096;
//static FWFileConfig _MSCFWFile = {MS_TOOL_ADR | 0x80000000  /*mem addr |0x80000000 PA2VA*/, 0};
static BYTE _u8MSCPageIdx = MSC_FWCTRL_PAGE_ORIGINAL;
static BOOL _u8MSCEnabled = FALSE;
static const BYTE _u8MSCStartCode = MSC_GET_VENDORID_OUI_L;
static const BYTE _u8MSCEndCode = MSC_CRC;
static const BYTE _u8MSCVendorInfoEndCode = MSC_RESERVED;
static fpMSCHandler _fpMSCHandler = NULL;

/*
#if ENABLE_USB_HOST_DUAL_IMAGE
extern DWORD MW_UsbDownload_GetDualImageOffset(void);
#endif
*/

static DWORD _u32DualImageOffset = 0;
static void mapi_MCCS_MSC_Reboot(void)
{
    MDrv_Write4Byte(WDT_TIMER_0, ( MDrv_Sys_WatchDogCycles( 1 ) ));
}

BOOL mapi_MCCS_MSC_GetEnabled(void)
{
    return _u8MSCEnabled;
}

void mapi_MCCS_MSC_SetEnabled(BOOL u8Enabled)
{
    _u8MSCEnabled = u8Enabled;
}

BOOL mapi_MCCS_MSC_IsValid(BYTE u8OpCode)
{
    if ((u8OpCode >= _u8MSCStartCode) && (u8OpCode <= _u8MSCEndCode))
    {
        return TRUE;
    }
    else
    {        
        return FALSE;
    }
}

static BOOL mapi_MCCS_MSC_IsVendorInfo(BYTE u8OpCode)
{
    if ((u8OpCode >= _u8MSCStartCode) && (u8OpCode <= _u8MSCVendorInfoEndCode))
    {
        return TRUE;
    }
    else
    {        
        return FALSE;
    }
}

BYTE mapi_MCCS_MSC_GetPageIndex(void)
{
    return _u8MSCPageIdx;
}

void mapi_MCCS_MSC_SetPageIndex(BYTE u8PageIdx)
{
    _u8MSCPageIdx = u8PageIdx;
}

static WORD mapi_MCCS_MSC_GetData(BYTE u8OpCode)
{    
    switch(_FWCtrlCode)
    {
        case MSC_FWCTRL_INIT: 
            if (MSC_END_ADDR_L == u8OpCode)
            {
                return (WORD)(_u32MSCFWEndAddr & 0xFFFF);                
            }
            else if (MSC_END_ADDR_H == u8OpCode)
            {
                return (WORD)(_u32MSCFWEndAddr >> 16);     
            }
        break;    
        case MSC_FWCTRL_HWVER:
            if (MSC_END_ADDR_L == u8OpCode)
            {
                return (WORD)(_HWVER & 0xFFFF);                
            }
            else if (MSC_END_ADDR_H == u8OpCode)
            {
                return (WORD)(_HWVER >> 16);     
            }            
        break;

        case MSC_FWCTRL_FWVER:
            if (MSC_END_ADDR_L == u8OpCode)
            {
                return (WORD)(_FWVER & 0xFFFF);        
            }
            else if (MSC_END_ADDR_H == u8OpCode)
            {
                return (WORD)(_FWVER >> 16);    
            }
             
        break;

        case MSC_FWCTRL_IMGADDR:
            if (MSC_END_ADDR_L == u8OpCode)
            {
                return (WORD)(_u32DualImageOffset & 0xFFFF);     
            }
            else if (MSC_END_ADDR_H == u8OpCode)
            {
                return (WORD)(_u32DualImageOffset >> 16);     
            }
             
        break;  

        case MSC_FWCTRL_USBCFG:
            if (MSC_END_ADDR_L == u8OpCode)
            {
                return (WORD)(_u8MSCUSBMode & 0xFFFF);     
            }
            else if (MSC_END_ADDR_H == u8OpCode)
            {
                return 0;     
            }            
        break;  
    }
    return 0;
}

static BOOL mapi_MCCS_MSC_Erase(DWORD u32StartAddr, DWORD u32EndAddr)
{
    if (u32EndAddr <= u32StartAddr)
    {
        return FALSE;
    }

    DWORD u32Addr = u32StartAddr;
    const DWORD u32SectorSize = 0x1000;

    for (u32Addr = u32StartAddr; u32Addr < u32EndAddr; u32Addr+= u32SectorSize)
    {
        FlashSectorErase(TRUE, u32Addr);
    }
    return TRUE;
}

static BOOL mapi_MCCS_MSC_Program(DWORD u32StartAddr, DWORD u32EndAddr, BYTE *pu8Data)
{
    if (u32EndAddr <= u32StartAddr)
    {
        return FALSE;
    }
    DWORD u32Size = u32EndAddr - u32StartAddr;
    if (u32Size > _16MSCBufLen)
    {
        return FALSE;
    }
    /*
    FlashUpdateFW_WinISP(TRUE, u32StartAddr, pu8Data, u32Size, FALSE);
    */
    UNUSED(pu8Data);
    return TRUE;
}

static BOOL mapi_MCCS_MSC_Read(DWORD u32StartAddr, DWORD u32EndAddr, BYTE *pu8Data)
{
    if (u32EndAddr <= u32StartAddr)
    {
        return FALSE;
    }
    DWORD u32Size = u32EndAddr - u32StartAddr;
    if (u32Size > _16MSCBufLen)
    {
        return FALSE;
    }
    /*
    Flash_ReadTbl(u32StartAddr, pu8Data, u32Size);
    */
    UNUSED(pu8Data);
    return TRUE;
}

static BYTE mapi_MCCS_MSC_Trigger(BYTE u8OpType)
{
    BYTE u8Result = MSC_FWCTRL_STA_IDLE;
    switch(u8OpType)
    {
        case MSC_FWCTRL_OPTYPE_ERASE:            
            if (mapi_MCCS_MSC_Erase(_u32MSCFWStartAddr, _u32MSCFWEndAddr))
            {
                u8Result = MSC_FWCTRL_STA_SUCCESS;    
            }
            else
            {
                u8Result = MSC_FWCTRL_STA_ERASE_FAIL;    
            }
        break;

        case MSC_FWCTRL_OPTYPE_WRITE:
                        
            /*if (mapi_MCCS_MSC_Program(_u32MSCFWStartAddr, _u32MSCFWEndAddr, _MSCFWFile.pBuffer))*/
            if (mapi_MCCS_MSC_Program(_u32MSCFWStartAddr, _u32MSCFWEndAddr, NULL))
            {
                u8Result = MSC_FWCTRL_STA_SUCCESS;    
            }
            else
            {
                u8Result = MSC_FWCTRL_STA_WRITE_FAIL;    
            }
                              
        break;

        case MSC_FWCTRL_OPTYPE_READ:    
                
            /*if (mapi_MCCS_MSC_Read(_u32MSCFWStartAddr, _u32MSCFWEndAddr, _MSCFWFile.pBuffer))*/
            if (mapi_MCCS_MSC_Read(_u32MSCFWStartAddr, _u32MSCFWEndAddr, NULL))
            {
                u8Result = MSC_FWCTRL_STA_SUCCESS;    
            }
            else
            {
                u8Result = MSC_FWCTRL_STA_READ_FAIL;    
            }             
        
        break;
    }
    return u8Result;
}

BYTE mapi_MCCS_MSC_Handler(BYTE* pDDCBuf, BYTE u8DftDDCLen, BYTE u8VCPCode, BYTE u8OpCode, BYTE u8SH, BYTE u8SL, WORD u16Value)
{
    BOOL IsValid = mapi_MCCS_MSC_IsValid(u8OpCode);
    BOOL IsVendorInfo = mapi_MCCS_MSC_IsVendorInfo(u8OpCode);
    BYTE u8RetValueH = 0, u8RetValueL = 0;
    WORD u16RetValue = 0;
     
    if (MCCS_GET_VCP == u8VCPCode)
    {
        if (IsValid == FALSE)
        {       
            return u8DftDDCLen;
        }
        
        //VCP Code is E4~FF
        if ((IsVendorInfo == FALSE) && (_u8MSCPageIdx != MSC_FWCTRL_PAGE_FWUPD))
        {
            if (_u8MSCPageIdx == MSC_FWCTRL_PAGE_ORIGINAL)
            {
                if (_fpMSCHandler == NULL)
                {                
                    return u8DftDDCLen;        
                }
                else
                {    
                    return _fpMSCHandler(pDDCBuf, u8DftDDCLen, u8VCPCode, u8OpCode, u8SH, u8SL, u16Value);
                }
            }
            else
            {
                return u8DftDDCLen;
            }                
        }  

        switch (u8OpCode)
        {
            case MSC_GET_VENDORID_OUI_H:               
                u8RetValueH = (BYTE)(_MTK_MSC_OUI >> 24);
                u8RetValueL = (BYTE)(_MTK_MSC_OUI >> 16);   
            break;
            case MSC_GET_VENDORID_OUI_L:                
                u8RetValueH = (BYTE)(_MTK_MSC_OUI >> 8);
                u8RetValueL = (BYTE)(_MTK_MSC_OUI & 0xFF );                
            break;

            case MSC_GET_VENDORMCCS_VERSION:
                u8RetValueH = _u8MSCVersion;
                u8RetValueL = _u8MSCPageIdx;                 
            break;

            case MSC_SET_START_ADDR_L:
                u16RetValue = (WORD)(_u32MSCFWStartAddr & 0xFFFF);
                u8RetValueH = (BYTE)(u16RetValue >> 8);
                u8RetValueL = (BYTE)(u16RetValue & 0xFF);
            break;

            case MSC_SET_START_ADDR_H:
                u16RetValue = (WORD)(_u32MSCFWStartAddr >> 16);
                u8RetValueH = (BYTE)(u16RetValue >> 8);
                u8RetValueL = (BYTE)(u16RetValue & 0xFF);
            break;
            
            case MSC_END_ADDR_L:
            case MSC_END_ADDR_H:
                u16RetValue = mapi_MCCS_MSC_GetData(u8OpCode);
                u8RetValueH = (BYTE)(u16RetValue >> 8);
                u8RetValueL = (BYTE)(u16RetValue & 0xFF);
            break;

            case MSC_GET_BUF_LEN:                
                u8RetValueH = (BYTE)(_16MSCBufLen >> 8);
                u8RetValueL = (BYTE)(_16MSCBufLen & 0xFF);                
            break;
            
            case MSC_BUF_WORD:
            {
                /*
                u8RetValueL = _MSCFWFile.pBuffer[_u32MSCBurstRWCnt];
                u8RetValueH = _MSCFWFile.pBuffer[_u32MSCBurstRWCnt + 1];
                */
                _u32MSCBurstRWCnt += 2;                
            }
            break;
            
            case MSC_GET_EXE_STATUS:                 
                u8RetValueH = 0;
                u8RetValueL = _u8FwOpStatus;                
            break;

            case MSC_CRC:               
                u8RetValueH = (BYTE)(_u16CRC >> 8);
                u8RetValueL = (BYTE)(_u16CRC & 0xFF);                               
            break;
        } 
        pDDCBuf[0] = 0x88;      
        pDDCBuf[1] = MCCS_GET_VCP;
        pDDCBuf[2] = 0x00;
        pDDCBuf[3] = u8OpCode;
        pDDCBuf[4] = 00;
        pDDCBuf[5] = 00;
        pDDCBuf[6] = 00;
        pDDCBuf[7] = u8RetValueH;
        pDDCBuf[8] = u8RetValueL;     
        return 8;  
    }
    else if (MCCS_SET_VCP == u8VCPCode)
    {        
        if (IsValid == FALSE)
        {       
            return u8DftDDCLen;
        } 
        
        //VCP Code is E4~FF      
        if ((IsVendorInfo == FALSE) && (_u8MSCPageIdx != MSC_FWCTRL_PAGE_FWUPD))
        {
            if (_u8MSCPageIdx == MSC_FWCTRL_PAGE_ORIGINAL)
            {
                if (_fpMSCHandler == NULL)
                {                
                    return u8DftDDCLen;        
                }
                else
                {    
                    return _fpMSCHandler(pDDCBuf, u8DftDDCLen, u8VCPCode, u8OpCode, u8SH, u8SL, u16Value);
                }
            }
            else
            {
                return u8DftDDCLen;
            }                
        }        
        pDDCBuf[0] = 0x80 | u8DftDDCLen;
        switch (u8OpCode)
        {
            case MSC_GET_VENDORMCCS_VERSION:                
                _u8MSCPageIdx = u8SL;
            break;

            case MSC_FW_CTRL:                
                _FWCtrlCode = u8SH;                  
                _u8FwOpStatus = MSC_FWCTRL_STA_IDLE;
                switch(_FWCtrlCode)         
                {
                    case MSC_FWCTRL_INIT:
                        /*
                        #if ENABLE_USB_HOST_DUAL_IMAGE
                        _u32DualImageOffset = MW_UsbDownload_GetDualImageOffset();  
                        #endif
                        //reset image buffer 
                        _MSCFWFile.pBuffer = (BYTE*)_MSCFWFile.memAddr;
                          memset(_MSCFWFile.pBuffer , 0xFF, _16MSCBufLen); 
                        */
                        _u8FwOpStatus = MSC_FWCTRL_STA_SUCCESS;    
                        _u32MSCBurstRWCnt = 0;  
                        _FWCtrlCode = MSC_FWCTRL_INIT;
                    break;

                    case MSC_FWCTRL_REBOOT:                        
                        mapi_MCCS_MSC_Reboot();  
                        _u8FwOpStatus = MSC_FWCTRL_STA_SUCCESS;                       
                    break;                   

                    case MSC_FWCTRL_OPTRIGGER:
                        _u8FwOpStatus = mapi_MCCS_MSC_Trigger(u8SL);
                    break;
                   
                    case MSC_FWCTRL_USBCFG:
                        
                        switch(u8SL)
                        {
                            case MSC_FWCTRL_USBMODE_RESERVED:
                                
                            break;
                            case MSC_FWCTRL_USBMODE_HOST:                                
                                _u8MSCUSBMode = MSC_FWCTRL_USBMODE_HOST;                                
                            break;
                            case MSC_FWCTRL_USBMODE_DEVICE:
                                _u8MSCUSBMode = MSC_FWCTRL_USBMODE_DEVICE;                                     
                                #if ENABLE_USB_DEVICE                           
                                USB_Device_Enable();                                
                                #endif
                            break;
                            case MSC_FWCTRL_USBMODE_READCFG:
                                _FWCtrlCode = MSC_FWCTRL_USBCFG;
                            break;
                            case MSC_FWCTRL_USBMODE_EXIT:                                
                                #if ENABLE_USB_DEVICE
                                USB_Device_Disable();                               
                                #endif
                                _u8MSCUSBMode = MSC_FWCTRL_USBMODE_RESERVED;                                
                            break;    
                        }                                                
                        _u8FwOpStatus = MSC_FWCTRL_STA_SUCCESS; 
                    break;                    
                }           
            break;

            case MSC_SET_START_ADDR_L:
                _u32MSCFWStartAddr &= 0xFFFF0000;
                _u32MSCFWStartAddr |= u16Value;            
                _u32MSCBurstRWCnt = 0;
            break;

            case MSC_SET_START_ADDR_H:
                _u32MSCFWStartAddr &= 0x0000FFFF;
                _u32MSCFWStartAddr |= (u16Value << 16);                
                _u32MSCBurstRWCnt = 0;
            break;

            case MSC_END_ADDR_L:
                _u32MSCFWEndAddr &= 0xFFFF0000;
                _u32MSCFWEndAddr |= u16Value;
                _FWCtrlCode = MSC_FWCTRL_INIT;
            break;

            case MSC_END_ADDR_H:
                _u32MSCFWEndAddr &= 0x0000FFFF;
                _u32MSCFWEndAddr |= (u16Value << 16);
                _FWCtrlCode = MSC_FWCTRL_INIT;
            break;
            
            case MSC_BUF_WORD:
            {
                /*
                _MSCFWFile.pBuffer[_u32MSCBurstRWCnt] = (BYTE)(u16Value & 0xFF);
                _MSCFWFile.pBuffer[_u32MSCBurstRWCnt + 1] = (BYTE)(u16Value >> 8);
                */
                _u32MSCBurstRWCnt += 2;
                _u16CRC += u16Value;                              
            }
            break;

            case MSC_CRC:
                _u16CRC = u16Value;
            break;
        } 
    }
    return u8DftDDCLen;
}

void mapi_MCCS_MSC_Install(fpMSCHandler fp)
{
    _fpMSCHandler = fp;
}

