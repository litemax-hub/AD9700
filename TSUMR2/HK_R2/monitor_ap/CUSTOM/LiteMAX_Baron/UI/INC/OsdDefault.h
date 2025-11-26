///////////////////////////////////////////////////////////////////////////////
/// @file OsdDefault.h
/// @brief Head file for OSD default value definition.
/// @author MStarSemi Inc.
///
///
///////////////////////////////////////////////////////////////////////////////


#ifndef _OSD_DEFAULT_H
#define _OSD_DEFAULT_H

#include "board.h"
#include "UserPrefDef.h"
#include "menudef.h"
//======================================================================
// Audio Setup Item Define
//======================================================================
#define VOLUME_OSD_MAX     		(100)
#define VOLUME_OSD_MID          (50)
#define VOLUME_OSD_MIN     		(0)
#define DEF_VOLUME              (100)

//Define-----------------------------------------------------
#define VOLUME_REAL_MAX    100
#define VOLUME_REAL_MID    50
#define VOLUME_REAL_MIN    0
//-----------------------------------------------------------

#define DEF_OSD_TRANSPARENCY   	0
#define MAX_OSD_TRANSPARENCY    5

#define DIGITAL_AUDIO     		(0)
#define LINEIN_AUDIO     		(1)
#define DEF_PGA_GAIN     		(LINEIN_AUDIO)
#define DEF_INPUT_TYPE         	First_Input_Port
#define DEF_INPUT_PRIORITY     	Input_Priority_Auto
#define DEF_OD_Weighting     		(0x3F)  //~=1

#endif



