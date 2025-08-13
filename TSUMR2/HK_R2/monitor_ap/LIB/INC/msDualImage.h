#ifndef _MSDUALIMAGE_H_
#define _MSDUALIMAGE_H_

#ifdef _MSDUALIMAGE_C_
  #define _MSDUALIMAGEDEC_
#else
  #define _MSDUALIMAGEDEC_ extern
#endif

_MSDUALIMAGEDEC_ void msDISetBankOffset(BYTE u8BankOffset);
_MSDUALIMAGEDEC_ BYTE msDIGetBankOffset(void);

#endif

