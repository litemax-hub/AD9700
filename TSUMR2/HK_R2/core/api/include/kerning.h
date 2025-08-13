////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifdef MSAPI_OSD_C

#define kerningTblSize 42

typedef  struct{
U8 char1;
U8 char2;
U8 kVal;
} kerningStr;


code kerningStr kerningTable[] =
{
{ 'A','T',1},
{ 'A','V',1},
{ 'A','W',1},
{ 'A','Y',2},
{ 'A','v',1},
{ 'A','y',1},
{ 'F','A',1},
{ 'L','T',1},
{ 'L','V',1},
{ 'L','W',1},
{ 'L','Y',2},
{ 'L','y',1},
{ 'P','A',1},
{ 'R','Y',1},
{ 'T','A',1},
{ 'T','a',1},
{ 'T','c',1},
{ 'T','e',1},
{ 'T','o',1},
{ 'T','r',1},
{ 'T','s',1},
{ 'T','u',1},
{ 'T','w',1},
{ 'T','y',1},
{ 'V','A',1},
{ 'V','a',1},
{ 'V','e',1},
{ 'V','o',1},
{ 'V','r',1},
{ 'V','u',1},
{ 'V','y',1},
{ 'W','A',1},
{ 'W','a',1},
{ 'Y','A',2},
{ 'Y','a',1},
{ 'Y','e',1},
{ 'Y','i',1},
{ 'Y','o',1},
{ 'Y','p',1},
{ 'Y','q',1},
{ 'Y','u',1},
{ 'Y','v',1},
};
#endif
