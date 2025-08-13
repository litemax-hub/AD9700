#ifndef _PDUPDATE_H_
#define _PDUPDATE_H_

#ifdef _PDUPDATE_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

/* ***** Config MIIC IO Used for PD .. in board define***** */
//#define MIIC0_ENABLE    MIIC0_0_ENABLE  //PD: MIIC0 on GPIO14 GPIO15
//#define USE_MIIC     MIIC0

/* ***** Config PD(RT7880) MIIC Slave Address ***** */
#define PDSA_MCU  (0x1C<<1) // PD slave address by RT7880 F/W
#define PDSA_PCB  (0x1E<<1) // PD slave address by PCB(GPIO2/AIN2)
// DON'T CHANGE PDSA_OLM //
#define PDSA_OLM  (0x2F<<1) // PD slave address by RT7880 F/W in online mode.


typedef enum
{
    ePDUGSTS_OK,
    ePDUGSTS_SKIP,
    ePDUGSTS_ER_CHKSUM,
    ePDUGSTS_ER_SLAVEADR,
    ePDUGSTS_ER_IIC,
    ePDUGSTS_ER_PROGTIMEOUT,
    ePDUGSTS_ER_PROGFAIL,
    ePDUGSTS_ER_VERIFY,
    ePDUGSTS_ER_TCPCSIZE,
}ePDUGSTS; // PD Upgrade Status

INTERFACE void PDUpdateDummy(void);
INTERFACE void msAPI_PDUpdateInit(void);
INTERFACE ePDUGSTS msAPI_PDUpdateFW(DWORD u32AddInFlash, BYTE u8PDSA_MCU, BYTE u8PDSA_PCB);
INTERFACE ePDUGSTS msAPI_PDUpdateAuto(DWORD u32AddInFlash, DWORD u32Length, BYTE u8PDSA_MCU, BYTE u8PDSA_PCB);

#undef INTERFACE
#endif


