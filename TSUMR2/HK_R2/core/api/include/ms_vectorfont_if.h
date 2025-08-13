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
/// @file  Ms_VectorFont.h
/// @brief MVF Header File
/// @author MStar Semiconductor Inc.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef _APP_VECTORFONT_IF_H__
#define _APP_VECTORFONT_IF_H__

////////////////////////////////////////////////////////////////////////////////
// Type & Structure Declaration
////////////////////////////////////////////////////////////////////////////////

typedef struct Cha_mailbox {
    unsigned char u8CmdClass;
    unsigned char u8Param[10];
} Chakra_Mail;

////////////////////////////////////////////////////////////////////////////////
// Local Global Variables
////////////////////////////////////////////////////////////////////////////////
Chakra_Mail Chakra_MailBox[2];

////////////////////////////////////////////////////////////////////////////////
// Function Prototype Declaration
////////////////////////////////////////////////////////////////////////////////

signed long ms_MVF_LoadFont(void);
signed long ms_MVF_MakeFont2(void);
BOOLEAN msAPI_MVF_init_mempool (void *pool, U32 size);

#endif // _APP_VECTORFONT_IF_H__

