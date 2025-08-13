#ifndef __TYPE_H__
#define __TYPE_H__

#include "types.h"

typedef U8 u8;

#if 0
#if defined(INTEL_8051)
#define u8   unsigned char
#define U8   unsigned char
#define u16  unsigned int
#define U16  unsigned int
#define u32  unsigned long
#define U32  unsigned long
#define S8   signed char
#define S16  signed int
#define S32  signed long

#else	/* 32 bits bus */

#define u8   unsigned char
#define U8   unsigned char
#define u16  unsigned short
#define U16  unsigned short
#define u32  unsigned int
#define U32  unsigned int
#define S8   signed char
#define S16  signed short
#define S32  signed int

#define BIT0 		0x01
#define BIT1		0x02
#define BIT2		0x04
#define BIT3		0x08
#define BIT4		0x10
#define BIT5		0x20
#define BIT6		0x40
#define BIT7		0x80
#endif 
#endif

#endif
