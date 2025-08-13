////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (!¡±MStar Confidential Information!¡L) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MW_USBU_VER_H
#define MW_USBU_VER_H

#include "MsVersion.h"

//-------------------------------------------------------------------------------------------------
// Version Control
//-------------------------------------------------------------------------------------------------
#define MSIF_USBU_LIB_CODE          {'U','S','B','U'}                    //Lib code
#define MSIF_USBU_LIBVER            {'0','1'}                            //LIB version
#define MSIF_USBU_BUILDNUM          {'0','0'}                            //Build Number
#define MSIF_USBU_CHANGELIST        {'0','0','2','5','2','8','8','8'}    //P4 ChangeList Number

#define MW_USBU_VER                  /* Character String for DRV/API version             */  \
    MSIF_TAG,                       /* 'MSIF'                                           */  \
    MSIF_CLASS,                     /* '00'                                             */  \
    MSIF_CUS,                       /* 0x0000                                           */  \
    MSIF_MOD,                       /* 0x0000                                           */  \
    MSIF_CHIP,                                                                              \
    MSIF_CPU,                                                                               \
    MSIF_USBU_LIB_CODE,              /* IP__                                             */  \
    MSIF_USBU_LIBVER,                /* 0.0 ~ Z.Z                                        */  \
    MSIF_USBU_BUILDNUM,              /* 00 ~ 99                                          */  \
    MSIF_USBU_CHANGELIST,            /* CL#                                              */  \
    MSIF_OS

#if 1
#include "MsTypes.h"
#else
typedef union _MSIF_Version
{
    struct _DDI
    {
        unsigned char                       tag[4];
        unsigned char                       type[2];
        unsigned short                      customer;
        unsigned short                      model;
        unsigned short                      chip;
        unsigned char                       cpu;
        unsigned char                       name[4];
        unsigned char                       version[2];
        unsigned char                       build[2];
        unsigned char                       change[8];
        unsigned char                       os;
    } DDI;
    struct _MW
    {
        unsigned char                                     tag[4];
        unsigned char                                     type[2];
        unsigned short                                    customer;
        unsigned short                                    mod;
        unsigned short                                    chip;
        unsigned char                                     cpu;
        unsigned char                                     name[4];
        unsigned char                                     version[2];
        unsigned char                                     build[2];
        unsigned char                                     changelist[8];
        unsigned char                                     os;
    } MW;
    struct _APP
    {
        unsigned char                                     tag[4];
        unsigned char                                     type[2];
        unsigned char                                     id[4];
        unsigned char                                     quality;
        unsigned char                                     version[4];
        unsigned char                                     time[6];
        unsigned char                                     changelist[8];
        unsigned char                                     reserve[3];
    } APP;
} MSIF_Version;
#endif

#endif
