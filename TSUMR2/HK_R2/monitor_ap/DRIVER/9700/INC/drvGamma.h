

#ifndef _DRVGAMMA_H_
#define _DRVGAMMA_H_

extern void drvGammaLoadTbl( BYTE GamaMode );
extern void drvGammaOnOff(BYTE u8Switch, BYTE u8Window);


#if ENABLE_LUT_AUTODOWNLOAD
#define _xdata2Dram        ((unsigned char volatile xdata *) 0x0000)
#define BYTES_PER_WORD           (16) // 64 byte.
#define AUTO_DOWNLOAD_GMA_MAX_ADDR   4096
extern xdata DWORD _dramGmaAddr;// = MIU_AUTOLOAD_ADDR_START;
extern xdata DWORD _xdataGmaAddr;// = (WIN1_ADDR_START << 10);//AUTOLOAD_HDR_ADR  |0x80000000;
extern void ClearGmaDramDataADL(DWORD u16Addr);
extern void WriteTriggADLGamma(DWORD u16StartAddr, BYTE Enable);
#endif

#endif
