#define _MODE_C_
#include <math.h>
#include "types.h"
#include "board.h"
#include "Mode.h"
#include "Global.h"

#include "ms_reg.h"
#include "Ms_rwreg.h"
#include "Detect.h"
#include "misc.h"
#include "drvADC.h"
#include "AutoFunc.h"
#include "Common.h"
//#include "halRwreg.h"
#if 1
#include "ComboApp.h"
#endif
#include "mapi_DPRx.h"

#define MODE_DEBUG    1
#if ENABLE_DEBUG&&MODE_DEBUG
    #define MODE_printData(str, value)   printData(str, value)
    #define MODE_printMsg(str)           printMsg(str)
#else
    #define MODE_printData(str, value)
    #define MODE_printMsg(str)
#endif

extern BYTE mStar_GetInputStatus( void );

#define SIMILAR_TIMING_BUFFER_MAX   10
BYTE xdata ucSimilarTimingNums;
BYTE xdata ucSimilarTimingBuffer[SIMILAR_TIMING_BUFFER_MAX]; // record similar timing buffer


//=====================================================================================
InputResolutionType code StandardModeResolution[] =
{
    { 640,  350},   // Res_640x350,
    { 640,  400},   // Res_640x400,
    { 720,  400},   // Res_720x400,
    { 640,  480},   // Res_640x480,
    //  { 720,  483},  //Res_720x483,
    {700,   570},   //Res_700x570
    { 720,  576},   //Res_720x576,
    { 800,  600},   // Res_800x600,
    { 832,  624},   // Res_832x624,
    { 884,  930},   // Res_884x930,
    {1024,  768},   // Res_1024x768,
    {1280, 1024},   // Res_1280x1024,
    {1600, 1200},   // Res_1600x1200,
    {1152,  864},   // Res_1152x864,
    {1152,  870},   // Res_1152x870,
    {1152,  900},   // RES_1152x900,
    {1280,  720},   // Res_1280x720,
    {1280,  960},   // Res_1280x960,
    { 720,  480},   // Res_720x240
    { 800,  1209},  // Res_800x1209
    {1792,  1344},  // Res_1792x1344
    {1400,  1050},  // Res_1400x1050
    {1920,  1080},  // Res_1920x1080
    {1920,  1200},  // Res_1920x1200
    {848,    480},  // Res_848x480
    {1280,   768},  // Res_1280x768
    {1360,   768},  // Res_1360x768
    {1366,   768},  // Res_1366x768
    {1440,   900},  // Res1440x900
    {1680,   1050}, // Res_1680x1050
    {1280,   800},  // Res_1280x800
    {1600,   900},  // Res_1600x900
    {1856,   1392}, // Res_1856x1392
    {1920,   1440}, //Res_1920x1440
    {2048,   1536}, //Res_2048x1536
    {1280,   600},  //Res_1280x600
    {1440,   1050}, //Res_1440x1050
    {2560,   1600}, //Res_2560x1600
    #if _3D_TIMING
    {1920,   2160}, //Res_1920x2160
    {1280, 1440},//Res_1280x720P_FP
    #endif
    {0,        0},//Max_Res
};
InputModeType code StandardMode[] =
{
	//=====IMPORTANT    DON'T DELETE IT, if mode table is for digital input usage. 
	{ 	
		1,                  // Flags
		(ResolutionType)1,  // ResIndex
		1, 1,               // HFreq, VFreq
		1, 1,               // HTotal, VTotal
		1, 1,               // HStart, VStart
	},
    // 0 848X480 60HZ
    {
        SyncHpVp, // Flags
        Res_848x480, // ResIndex
        310, 600, // HFreq, VFreq
        1088, 517, // HTotal, VTotal
        224 - 5, 31 - 8 - 1, // HStart, VStart
    },
    // 1 848X480 75HZ (CVT)
    {
        SyncHnVp,   // Flags
        Res_848x480, // ResIndex
        377, 748, // HFreq, VFreq
        1088, 504, // HTotal, VTotal
        200 - 5, 21 - 5 - 1, // HStart, VStart
    },
    // 2 1280X768 60Hz Reduced Blanking(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x768, // ResIndex
        474, 599, // HFreq, VFreq
        1440, 790, // HTotal, VTotal
        112 - 5, 19 - 7 - 1, // HStart, VStart
    },
    // 3 1280X768 60Hz Normal Blanking(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x768, // ResIndex
        477, 598, // HFreq, VFreq
        1664, 798, // HTotal, VTotal
        320 - 5, 27 - 7 - 1, // HStart, VStart
    },
    // 4 1280X768 75Hz(VESA)
    {
        SyncHnVp, // Flags
        Res_1280x768, // ResIndex
        603, 748, // HFreq, VFreq
        1696, 805, // HTotal, VTotal
        336 - 7, 34 - 7 - 1, // HStart, VStart
    },
    // 5 1280X768 85HZ(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x768, // ResIndex
        686, 848, // HFreq, VFreq
        1712, 809, // HTotal, VTotal
        352 - 6, 38 - 7 - 1, // HStart, VStart
    },
    // 6 1280x960 60Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,
        Res_1280x960, // ResIndex
        600, 600, // HFreq, VFreq
        1696, 996, //1800, 1000, // HTotal, VTotal
        424, 39, // HStart, VStart
    },
    // 7 1280x960 60Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,
        Res_1280x960, // ResIndex
        600, 600, // HFreq, VFreq
        1800, 1000, // HTotal, VTotal
        424 - 5, 39 - 3 - 1, // HStart, VStart
    },
    // 8 1280x960 70Hz (Tune For PC)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x960, // ResIndex
        700, 700, // HFreq, VFreq
        1728, 1000, // HTotal, VTotal
        354, 34, // HStart, VStart
    },
    // 9 1280x960 72Hz(Tune For PC)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn , // Flags
        Res_1280x960, // ResIndex
        723, 722, // HFreq, VFreq
        1728, 1002,//1728, 1001, // HTotal, VTotal
        354, 36, // HStart, VStart
    },
    // 10 1280x960 75Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x960, // ResIndex
        750, 750, // HFreq, VFreq
        1728, 1002,//1680, 1000, // HTotal, VTotal
        360 - 5, 39 - 3 - 1, // HStart, VStart
    },
    // 11 1280x960 85Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x960, // ResIndex
        859, 850, // HFreq, VFreq
        1728, 1011, // HTotal, VTotal
        384 - 6, 50 - 3 - 1, // HStart, VStart
    },
    // 12 1360X768 60HZ(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,// Flags
        Res_1360x768, // ResIndex
        477, 600, // HFreq, VFreq
        1792, 795 - 1, // HTotal, VTotal
        368 - 5, 24 - 6 - 1, // HStart, VStart
    },
    // 13 1440X900 60Hz Reduce Blanking(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1440x900, // ResIndex
        554, 599, // HFreq, VFreq
        1600, 926, // HTotal, VTotal
        112 - 5, 23 - 6 - 1, // HStart, VStart
    },
    // 14 1440X900 60Hz Normal Blanking(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1440x900, // ResIndex
        559, 598, // HFreq, VFreq
        1904, 934, // HTotal, VTotal
        384 - 6, 31 - 6 - 1, // HStart, VStart
    },
    // 15 1440X900 75HZ(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1440x900, // ResIndex
        706, 750, // HFreq, VFreq
        1936, 942, // HTotal, VTotal
        400 - 6,  39 - 6 - 1, // HStart, VStart
    },
    // 16 1440X900 85Hz(VESA)
    {
        SyncHnVp, // Flags
        Res_1440x900, // ResIndex
        804, 848, // HFreq, VFreq
        1952, 948, // HTotal, VTotal
        408 - 6,  45 - 6 - 1, // HStart, VStart
    },

    // 17 1680X1050 60Hz Reduced Blanking(VESA)
    {
        SyncHnVp | SyncHpVn | SyncHpVp | SyncHnVn, // Flags
        Res_1680x1050, // ResIndex
        646,  598, // HFreq, VFreq
        1840, 1080, // HTotal, VTotal
        112 - 8,   27 - 6 - 1, // HStart, VStart
    },
    // 18 1680X1050 60Hz Normal Blanking(VESA)
    {
        SyncHnVp | SyncHpVn | SyncHpVp | SyncHnVn, // Flags
        Res_1680x1050, // ResIndex
        652 , 599, // HFreq, VFreq
        2240, 1089, // HTotal, VTotal
        456 - 8 ,  36 - 6 - 1, // HStart, VStart
    },
    // 19 1680X1050 75HZ
    {
        SyncHnVp | SyncHpVn | SyncHpVp | SyncHnVn, // Flags
        Res_1680x1050, // ResIndex
        823 , 748, // HFreq, VFreq
        2272, 1099, // HTotal, VTotal
        472 - 9 ,  46 - 6 - 1, // HStart, VStart
    },
    //====================================
    //===== Normal Mode Start ============
    //====================================
    // 20 640x350 70Hz (IMB VGA)
    {
        SyncHpVn, // Flags
        Res_640x350, // ResIndex
        315, 700, // HFreq, VFreq
        800, 449, // HTotal, VTotal
        145, 62 - 2 - 1,//144 - 5, 62 - 2 - 1, // HStart, VStart    //PS. spec 144, 61
    },
    //21 640x350 85Hz (VESA)
    {
        SyncHpVn, // Flags
        Res_640x350, // ResIndex
        379, 851, // HFreq, VFreq
        832, 445, // HTotal, VTotal
        160 - 5, 63 - 3 - 1, // HStart, VStart
    },
    //22 720x400 70Hz (VGA)
    {
        SyncHpVp | SyncHnVp | SyncHnVn, // Flags
        Res_720x400, // ResIndex
        315, 700, // HFreq, VFreq
        900, 449, // HTotal, VTotal
        162, 37 - 2 - 1,//160 - 6, 37 - 2 - 1, //160 - 6, 37 - 2 - 1, // HStart, VStart     //PS.spec 153, 29
    },
    //23 720x400 85Hz (VESA)     //Haven't tune Chroma No this Timing
    {
        SyncHpVp | SyncHnVp | SyncHnVn, // Flags
        Res_720x400, // ResIndex
        379, 850, // HFreq, VFreq
        936, 446, // HTotal, VTotal
        180 - 5, 45 - 3 - 1, // HStart, VStart
    },
    //24 640x400 56Hz (NEC)
    {
        SyncHnVn, // Flags
        Res_640x400, // ResIndex
        249, 564, // HFreq, VFreq
        848, 440, // HTotal, VTotal
        144 - 5, 33 - 8 - 1, // HStart, VStart
    },
    //25 640x400 70Hz (IMB VGA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_640x400, // ResIndex
        379, 851, // HFreq, VFreq
        832, 445, // HTotal, VTotal
        160 - 5, 44 - 3 - 1, // HStart, VStart
    },
    //===============================================
    //===>Conflict with WSVGA(848x480@60Hz HpVp) ====
    //===============================================
    //26 640x480 60Hz (VESA)
    {
        //SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_640x480, // ResIndex
        315, 599, // HFreq, VFreq
        800, 525, // HTotal, VTotal
        136 + 8, 27 + 5, //136, 27 - 3, //139, 32, // HStart, VStart           //spec 136, 27   //111111 Rick modified
    },

    //27 640x480 66Hz (MAC)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_640x480, // ResIndex
        350, 667, // HFreq, VFreq
        864, 525, // HTotal, VTotal
        160, 42, // HStart, VStart
    },

    //28 640x480 72Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_640x480, // ResIndex
        335, 671, // HFreq, VFreq
        816, 499, // HTotal, VTotal
        153, 14, // HStart, VStart
    },

    //29 640x480 72Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_640x480, // ResIndex
        379, 728, // HFreq, VFreq
        832, 520, // HTotal, VTotal
        160 - 5, 23 - 3 - 1, // HStart, VStart
    },
    //30 640x480 75Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_640x480, // ResIndex
        375, 750, // HFreq, VFreq
        840, 500, // HTotal, VTotal
        184 - 5, 19 - 3 - 1, // HStart, VStart
    },
    //31 640x480 85Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_640x480, // ResIndex
        433, 850, // HFreq, VFreq
        832, 509, // HTotal, VTotal
        136 - 5, 28 - 3 - 1, // HStart, VStart
    },
    //32 800x600 56Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_800x600, // ResIndex
        352, 562, // HFreq, VFreq
        1024, 625, // HTotal, VTotal
        200 - 5, 24 - 2 - 1, // HStart, VStart
    },
    //33 800x600 60Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_800x600, // ResIndex
        379, 603, // HFreq, VFreq
        1056, 628, // HTotal, VTotal
        216 - 5, 27 - 4 - 1, // HStart, VStart
    },
    // 34 800x600 72Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_800x600, // ResIndex
        481, 722, // HFreq, VFreq
        1040, 666, // HTotal, VTotal
        184 - 5, 29 - 6 - 1, // HStart, VStart
    },
    // 35 800x600 75Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_800x600, // ResIndex
        469, 750, // HFreq, VFreq
        1056, 625, // HTotal, VTotal
        240 - 5, 24 - 3 - 1, // HStart, VStart
    },
    // 36 800x600 85Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_800x600, // ResIndex
        537, 851, // HFreq, VFreq
        1048, 631, // HTotal, VTotal
        216 - 5, 30 - 3 - 1, // HStart, VStart
    },
    // 37 832x624 75Hz (MAC)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_832x624, // ResIndex
        497, 746, // HFreq, VFreq
        1152, 665,//667, // HTotal, VTotal
        288 - 5, 42 - 3 - 1, // HStart, VStart
    },
    //========================================================
    //===>Conflict with WXVGA(1280x768@60Hz HpVp;HnVp) ====
    //========================================================

    // 38 1024x768 60Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flagsc
        Res_1024x768, // ResIndex
        484, 600, // HFreq, VFreq
        1344, 806, // HTotal, VTotal
        296 - 5, 35 - 6 - 1, // HStart, VStart
    },
    // 39 1024x768 60Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flagsc
        Res_1024x768, // ResIndex
        484, 600, // HFreq, VFreq
        1312, 813, // HTotal, VTotal
        264 - 5, 39 - 3 - 1, // HStart, VStart
    },
    // 40 1024x768 70Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1024x768, // ResIndex
        565, 700, // HFreq, VFreq
        1328, 806, // HTotal, VTotal
        280 - 6, 35 - 6 - 1, // HStart, VStart
    },

    //===============================================
    //===>Conflict with WXVGA(1280x768@75Hz HnVp) ===
    //===============================================
    // 41 1024x768 75Hz(VESA)
    {
        //SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        SyncHpVp | SyncHpVn | SyncHnVn, // Flags
        Res_1024x768, // ResIndex
        600, 750, // HFreq, VFreq
        1312, 800, // HTotal, VTotal
        272 - 5, 31 - 3 - 1, // HStart, VStart
    },
    // 42 1024x768 75Hz (MAC)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1024x768, // ResIndex
        602, 749, // HFreq, VFreq
        1328, 804, // HTotal, VTotal
        272 - 5, 33 - 3 - 1, // HStart, VStart
    },
    //===============================================
    //===>Conflict with WXVGA(1280x768@85Hz HnVp) ===
    //===============================================
    // 43 1024x768 85Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1024x768, // ResIndex
        683, 850, // HFreq, VFreq
        1376, 808, // HTotal, VTotal
        304 - 5, 39 - 3 - 1, // HStart, VStart
    },
    // 44 1152x864 60Hz (VESA GTF)
    { SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1152x864,                                       // ResIndex
        537, 600,                                           // HFreq, VFreq
        1520, 895,                                          // HTotal, VTotal
        296, 26,                                            // HStart, VStart
    },
    // 45 1152x864 70Hz(Tune For PC)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1152x864, // ResIndex
        629, 699, // HFreq, VFreq
        1536, 900,//14, // HTotal, VTotal
        312 - 6, 35 - 3 - 1, // HStart, VStart
    },
    // 46 1152x864 70Hz (For ATI Card)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1152x864, // ResIndex
        662, 702, // HFreq, VFreq
        1536, 900,// HTotal, VTotal
        288, 49, // HStart, VStart
    },
    // 47 1152x864 75Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1152x864, // ResIndex
        675, 750, // HFreq, VFreq
        1560, 900, // HTotal, VTotal
        384 - 6, 35 - 3 - 1, // HStart, VStart
    },
    // 48 1152x864 85Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1152x864, // ResIndex
        771, 850, // HFreq, VFreq
        1576, 907, // HTotal, VTotal
        360, 42, // HStart, VStart
    },
    // 49 1152x870 75Hz (MAC)
    {
        SyncHpVp | SyncHpVn  | SyncHnVn, // Flags
        Res_1152x870, // ResIndex
        689, 751, // HFreq, VFreq
        1456, 915, // HTotal, VTotal
        272 - 7, 42 - 3 - 1, // HStart, VStart
    },
    // 50 1280x720 60Hz(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,//Flags
        Res_1280x720, // ResIndex
        450, 600, // HFreq, VFreq
        1650, 750, // HTotal, VTotal
        260 - 5, 25 - 5 - 1, // HStart, VStart
    },
    // 51 1280x720 70Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x720, // ResIndex
        524, 700, // HFreq, VFreq
        1688, 750, // HTotal, VTotal
        424, 39, // HStart, VStart
    },
    // 52 1280x720 75Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x720, // ResIndex
        563, 750, // HFreq, VFreq
        1696, 755, // HTotal, VTotal
        336 - 5, 32 - 5 - 1, // HStart, VStart
    },
    //========================================================
    //===>Conflict with SXVGA+(1400x1050@60Hz HpVn,HnVP ) ====
    //========================================================
    // 53 1280x1024 60Hz (VESA)
    {
        SyncHpVp | SyncHnVn | SyncHpVn | SyncHnVp, // Flags
        Res_1280x1024, // ResIndex
        640, 600, // HFreq, VFreq
        1688, 1066, // HTotal, VTotal
        360 - 6, 41 - 3 - 1, // HStart, VStart
    },

    // 54 1280x1024 75Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x1024, // ResIndex
        800, 750, // HFreq, VFreq
        1688, 1066, // HTotal, VTotal
        392 - 6, 41 - 3 - 1, // HStart, VStart
    },
    // 55 1280x1024 85Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x1024, // ResIndex
        911, 850, // HFreq, VFreq
        1728, 1072, // HTotal, VTotal
        384 - 7, 47 - 3 - 1, // HStart, VStart
    },
    // 56 1600x1200 60Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1600x1200, // ResIndex
        750, 600, // HFreq, VFreq
        2160, 1250, // HTotal, VTotal
        496 - 7, 49 - 3 - 1, // HStart, VStart
    },
    // 57 1600x1200 65Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1600x1200, // ResIndex
        813, 650, // HFreq, VFreq
        2160, 1250, // HTotal, VTotal
        496 - 9, 49 - 24 - 1, // HStart, VStart
    },
    // 58 1600x1200 70Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1600x1200, // ResIndex
        875, 700, // HFreq, VFreq
        2160, 1250, // HTotal, VTotal
        496 - 6, 49 - 24 - 1, // HStart, VStart
    },
    // 59 1600x1200 75Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1600x1200, // ResIndex
        938, 750, // HFreq, VFreq
        2160, 1250, // HTotal, VTotal
        496 - 6, 49 - 24 - 1, // HStart, VStart
    },
    // 60 1400x1050 60Hz Reudce Blanking(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1400x1050, // ResIndex
        647, 599, // HFreq, VFreq
        1560, 1080, // HTotal, VTotal
        112 - 5, 27 - 4 - 1, // HStart, VStart
    },
    // 61 1400x1050 60Hz Normal Blanking(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1400x1050, // ResIndex
        653, 599, // HFreq, VFreq
        1864, 1089, // HTotal, VTotal
        376 - 6, 36 - 4 - 1, // HStart, VStart
    },
    // 62 1400x1050 75Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1400x1050, // ResIndex
        823, 749,      // HFreq, VFreq
        1896, 1099,    // HTotal, VTotal
        392 - 6, 46 - 4 - 1, // HStart, VStart
    },
    // 63 1400x1050 85Hz (VESA)
    {
        SyncHnVp,      // Flags
        Res_1400x1050, // ResIndex
        939,  850,      // HFreq, VFreq
        1912, 1105,    // HTotal, VTotal
        408 - 6,  52 - 4 - 1, // HStart, VStart
    },
    // 64 1792x1344 60Hz (VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1792x1344, // ResIndex
        836, 600, // HFreq, VFreq
        2448, 1394, // HTotal, VTotal
        528 - 6, 49 - 3 - 1, // HStart, VStart
    },
    // 65 1920x1200 60Hz Normal Blanking(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1920x1200, // ResIndex
        746, 599, // HFreq, VFreq
        2592, 1245, // HTotal, VTotal
        536 - 6, 42 - 6 - 1, // HStart, VStart
    },
    // 66 1920x1200 60Hz Reduce Blanking(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1920x1200, // ResIndex
        740, 599, // HFreq, VFreq
        2080, 1235, // HTotal, VTotal
        112 - 5, 32 - 6 - 1, // HStart, VStart
    },
#if 1
    // 67 720x480 50Hz (YCbCr)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn | bInterlaceMode, // Flags
        Res_720x480, // ResIndex
        156, 500, // HFreq, VFreq
        900, 625, // HTotal, VTotal
        144, 88, // HStart, VStart
    },
#endif
    // 68 1024x768 72Hz
    {
        //SyncHpVp | SyncHpVn | SyncHnVp  // Flags
        SyncHpVp | SyncHnVp, // Flags
        Res_1024x768, // ResIndex
        576, 721, // HFreq, VFreq
        1312, 800, // HTotal, VTotal
        272, 31, // HStart, VStart
    },

    // 69 1280 X 1024  72 Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x1024, // ResIndex
        779, 720, // HFreq, VFreq
        1728, 1082, // HTotal, VTotal
        360 - 5, 57 - 5 - 1, // HStart, VStart
    },

    // 70 1280 X 1024 70 Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x1024, // ResIndex
        748, 697, // HFreq, VFreq
        1696, 1064, // HTotal, VTotal
        378, 41, // HStart, VStart
    },

    // 71 1152 X 900 66 Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1152x900, // enResIndex
        618, 660, // u16HFreq, u16VFreq
        1504, 937, // u16HTotal, u16VTotal
        322 - 7, 35 - 4 - 1, // u16HStart, u16VStart
    },

    // 72 720x480I 60Hz (YPbPr)  --> 78  Start of YPbPr
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn | bInterlaceMode, // Flags
        Res_720x480, // resolution index
        157, 599,    // HFreq, VFreq
        858, 525,    // HTotal, VTotal
        238, 18,       // HStart, VStart
    },

    // 73 1920x1080i 50Hz (YPbPr)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn | bInterlaceMode, // Flags
        Res_1920x1080, // resolution index
        281, 500,      // HFreq, VFreq
        2640, 1125,    // HTotal, VTotal
        186, 24,       // HStart, VStart
    },

    // 74 1920x1080i 50Hz (YPbPr)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn | bInterlaceMode, // Flags
        Res_1920x1080, // resolution index
        312, 500,     // HFreq, VFreq
        2304, 1250,   // HTotal, VTotal
        186, 24,      // HStart, VStart
    },

    // 75 NTSC 1080i
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn | bInterlaceMode, // Flags
        Res_1920x1080, // resolution index
        337, 600,      // HFreq, VFreq
        2200, 1125,    // HTotal, VTotal
        232, 27,       // HStart, VStart
    },
    // 76 700x570 50Hz (For India TV Box)
    {
        SyncHpVp  , // Flags
        Res_700x570, // ResIndex
        313, 500, // HFreq, VFreq
        884, 625, // HTotal, VTotal
        179, 43, // HStart, VStart
    },
    // 77  1152x870 60Hz (MAC)
    {
        SyncHnVp  , // Flags
        Res_1152x870, // ResIndex
        540, 600, // HFreq, VFreq
        1480, 900, // HTotal, VTotal
        264, 24, // HStart, VStart
    },
    // 78 576P(YCbCr)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_720x576, // ResIndex
        312, 500, // HFreq, VFreq
        864, 625, // HTotal, VTotal
        125, 40, // HStart, VStart
    },
    // 79 //720x576 60Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_720x576, // ResIndex
        358, 597, // HFreq, VFreq
        912, 600, // HTotal, VTotal
        168 - 4, 23 - 3 - 1, // HStart, VStart
    },
    // 80 //720x576 75Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_720x576, // ResIndex
        455, 756, // HFreq, VFreq
        944, 602, // HTotal, VTotal
        129 - 5, 25 - 2 - 1, // HStart, VStart
    },
    // 81 576i
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn | bInterlaceMode,  // Flags
        Res_720x576, // ResIndex
        156, 500, // HFreq, VFreq
        864, 626, // HTotal, VTotal
        264, 22, // HStart, VStart
    },

    // 82 480P 60Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_720x480, // ResIndex
        315, 599, // HFreq, VFreq
        858, 525, // HTotal, VTotal
        122, 36,   // HStart, VStart
    },

    // 83     480i
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn | bInterlaceMode,  // Flags
        Res_640x480, // ResIndex
        157, 599, // HFreq, VFreq
        1716, 525, // HTotal, VTotal
        238, 21, // HStart, VStart
    },
    // 84 PAL 1080p
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1920x1080, // ResIndex
        562, 500, // HFreq, VFreq
        2640, 1125, // HTotal, VTotal
        112, 28, // HStart, VStart
    },
    // 85 NTSC 1080p
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1920x1080, // ResIndex
        674, 599, // HFreq, VFreq
        2200, 1125,// HTotal, VTotal
        192 - 7, 41 - 5 - 1, // HStart, VStart
    },
    // 86 832x624 75Hz (MAC)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_832x624, // ResIndex
        497, 746, // HFreq, VFreq
        1152, 662, // HTotal, VTotal
        288, 42, // HStart, VStart
    },
    // 87 1280x800 60_RHz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1280x800, // ResIndex
        493, 599, // HFreq, VFreq
        1440, 823,// HTotal, VTotal
        112 - 5, 20 - 6 - 1, // HStart, VStart
    },
    // 88 1280x800 60Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1280x800, // ResIndex
        497, 598, // HFreq, VFreq
        1680, 831,// HTotal, VTotal
        328 - 5, 28 - 6 - 1, // HStart, VStart
    },
    // 89 1280x800 60Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1280x800, // ResIndex
        497, 598, // HFreq, VFreq
        1696, 838,// HTotal, VTotal
        336 - 5, 35 - 6 - 1, // HStart, VStart
    },
    // 90 1280x800 60Hz NVIDIA
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1280x800, // ResIndex
        495, 598, // HFreq, VFreq
        1680, 828,// HTotal, VTotal
        329, 23,       // HStart, VStart
    },
    // 91 1280x800 75Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1280x800, // ResIndex
        628, 749, // HFreq, VFreq
        1696, 838,// HTotal, VTotal
        336 - 5, 35 - 6 - 1, // HStart, VStart
    },
    // 92 1280x800 76Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1280x800, // ResIndex
        647, 764, // HFreq, VFreq
        1568, 846,// HTotal, VTotal
        264, 43,       // HStart, VStart
    },
    //93 1080p 138.5M
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1920x1080, // ResIndex
        666, 599, // HFreq, VFreq
        2080, 1111,// HTotal, VTotal
        112 - 7, 28 - 5 - 1, // HStart, VStart
    },
    // 94 1080p 148.5M
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1920x1080, // ResIndex
        663, 599, // HFreq, VFreq
        2240, 1107,// HTotal, VTotal
        256 - 9, 26 - 3 - 1, // HStart, VStart
    },

    // 95 1920x1080P 24Hz
    {
        SyncHpVp |SyncHpVn |SyncHnVp |SyncHnVn, // Flags
        Res_1920x1080, // ResIndex
        270, 240, // HFreq, VFreq
        2750, 1125, // HTotal, VTotal
        229,60,//230, 60,//59,
    },
    // 96 1920x1080P 25Hz
    {
        SyncHpVp |SyncHpVn |SyncHnVp |SyncHnVn, // Flags
        Res_1920x1080, // ResIndex
        281, 250, // HFreq, VFreq
        2640, 1125, // HTotal, VTotal
        229,60,//230, 60,//59,
    },
    // 97 1920x1080P 30Hz
    {
        SyncHpVp |SyncHpVn |SyncHnVp |SyncHnVn, // Flags
        Res_1920x1080, // ResIndex
        338, 300, // HFreq, VFreq
        2200, 1125, // HTotal, VTotal
        229,60,//230, 60,//59,
    },
#if 1//PANEL_WIDTH==1600&&PANEL_HEIGHT==900     //110919 Rick modified - A030
    // 98 1600x900 60Hz(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1600x900, // ResIndex
        600, 600, // HFreq, VFreq
        1800, 1000,// HTotal, VTotal
        176 - 6, 99 - 3 - 1, // HStart, VStart
    },
    // 99 1600x900 60Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1600x900, // ResIndex
        559, 600, // HFreq, VFreq
        2128, 932,// HTotal, VTotal
        176, 99,       // HStart, VStart
    },
    // 100 1600x900 60Hz(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,   // Flags
        Res_1600x900, // ResIndex
        555, 600, // HFreq, VFreq
        1760, 926,// HTotal, VTotal
        112 - 6, 23 - 5 - 1, // HStart, VStart
    },
#endif
#if 1//PANEL_WIDTH==1366&&PANEL_HEIGHT==768
    // 101 1366x768 60Hz Normal Blanking(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,
        Res_1366x768, // ResIndex
        477, 598, // HFreq, VFreq
        1792, 798, // HTotal, VTotal
        356 - 6, 27 - 3 - 1, // HStart, VStart
    },
    // 102 1360X768 60Hz(VESA)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,
        Res_1360x768, // ResIndex
        477, 600, // HFreq, VFreq
        1792, 795 - 1, // HTotal, VTotal
        368 - 5 - 3, 24 - 6 - 1, // HStart, VStart
    },
#endif
    //Add a new mode for some PC 1080P timing
    // 103 1920X1080  60Hz        // 103 QD-CVT1960H , ouput DCLK=173MHz
    {
        SyncHnVp | SyncHpVn | SyncHpVp | SyncHnVn, // Flags
        Res_1920x1080, // resolution index
        672, 600,     // HFreq, VFreq
        2576,1120,    // HTotal, VTotal
        0x208,0x1F,//528, 37,      // HStart, VStart
    },
    // 104 1600x900 75Hz
    {
        SyncHnVp | SyncHpVn | SyncHpVp | SyncHnVn,   // Flags
        Res_1600x900,                                      // ResIndex
        705, 749,                                           // HFreq, VFreq
        2144, 942,                                         // HTotal, VTotal
        440, 39,                                            // HStart, VStart
    },
    // 105 1680x1050 60Hz  (VESA GTF)
    {
        SyncHnVp | SyncHpVn | SyncHpVp | SyncHnVn,  // Flags
        Res_1680x1050,                                      // ResIndex
        652,600,                                            // HFreq, VFreq
        2256, 1087,                                         // HTotal, VTotal
        472, 36,                                            // HStart, VStart
    },
    // 106 1280x768 75Hz (VESA GTF)
    {
        SyncHnVp | SyncHpVn | SyncHpVp | SyncHnVn,  // Flags
        Res_1280x768,                                       // ResIndex
        602, 750,                                           // HFreq, VFreq
        1712, 802,                                          // HTotal, VTotal
        352, 33,                                            // HStart, VStart
    },
    // 107 1920x1080 75Hz (CVT)  DCLK=220.75MHz      CVT1975H
    {
        SyncHnVp | SyncHpVn | SyncHpVp | SyncHnVn,  // Flags
        Res_1920x1080,                                   // ResIndex
        846, 749,                                           // HFreq, VFreq
        2608,1130,                                          // HTotal, VTotal
        344, 42,                                            // HStart, VStart
    },
    // 108 1856x1392 60Hz        DCLK=217.25MHz      QD-CVT1860
    {
        SyncHnVp | SyncHpVn | SyncHpVp | SyncHnVn,  // Flags
        Res_1856x1392,                                      // ResIndex
        865, 599,                                           // HFreq, VFreq
        2512, 1443,                                         // HTotal, VTotal
        527, 35,                                            // HStart, VStart
    },
    // 109 1280x768 60Hz GTF, vsync_width=3
    {
        SyncHnVp | SyncHpVn | SyncHpVp | SyncHnVn,  // Flags
        Res_1280x768,                                       // ResIndex
        477, 600,                                           // HFreq, VFreq
        1680, 795,                                          // HTotal, VTotal
        328, 22,                                            // HStart, VStart
    },
//Add by Yong-Guan
    // 110 1856x1392 75Hz (VESA DMT)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,          // Flags
        Res_1856x1392,                                      // ResIndex
        1125, 750,                                          // HFreq, VFreq
        2560, 1500,                                         // HTotal, VTotal
        576, 107,                                               // HStart, VStart
    },
    // 111 1920x1080 85Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,          // Flags
        Res_1920x1080,                                      // ResIndex
        965, 848,                                           // HFreq, VFreq
        2624, 1137,                                         // HTotal, VTotal
        560, 54,                                            // HStart, VStart
    },
    // 112 1920x1200 85Hz (VESA GTF)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,          // Flags
        Res_1920x1200,                                      // ResIndex
        1071, 850,                                          // HFreq, VFreq
        2640, 1260,                                         // HTotal, VTotal
        568, 59,                                            // HStart, VStart
    },
    // 113 1920x1440 85Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1920x1440,                                      // ResIndex
        1285, 849,                                           // HFreq, VFreq
        2640, 1514,                                         // HTotal, VTotal
        568, 71,                                            // HStart, VStart
    },
    // 114 2048x1536 75Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_2048x1536,                                      // ResIndex
        1203, 749,                                           // HFreq, VFreq
        2816, 1606,                                         // HTotal, VTotal
        608, 67,                                            // HStart, VStart
    },
    // 115 640x480 120Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,          // Flags
        Res_640x480,                                        // ResIndex
        619, 1195,                                          // HFreq, VFreq
        848, 518,                                           // HTotal, VTotal
        168, 35,                                            // HStart, VStart
    },
    // 116 800x600 85Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,          // Flags
        Res_800x600,                                        // ResIndex
        537, 848,                                           // HFreq, VFreq
        1056, 633,                                          // HTotal, VTotal
        208, 30,                                            // HStart, VStart
    },
    // 117 1024x768 100Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1024x768,                                      // ResIndex
        815, 999,                                           // HFreq, VFreq
        1376, 816,                                         // HTotal, VTotal
        280, 45,                                            // HStart, VStart
    },
    // 118 1024x768 120Hz (VESA GTF)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1024x768,                                       // ResIndex
        988, 1200,                                          // HFreq, VFreq
        1408, 823,                                          // HTotal, VTotal
        304, 54,                                            // HStart, VStart
    },
    // 119 1280x960 85Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1280x960,                                      // ResIndex
        857, 848,                                           // HFreq, VFreq
        1728, 1011,                                         // HTotal, VTotal
        360, 48,                                            // HStart, VStart
    },
    // 120 1280x960 120Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1280x960,                                      // ResIndex
        1237, 1198,                                           // HFreq, VFreq
        1760, 1032,                                         // HTotal, VTotal
        376, 69,                                            // HStart, VStart
    },
    // 121 1600x1200 100Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1600x1200,                                      // ResIndex
        1272, 998,                                           // HFreq, VFreq
        2208, 1274,                                         // HTotal, VTotal
        480, 71,                                            // HStart, VStart
    },
    // 122 1680x1050 85Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1680x1050,                                      // ResIndex
        938, 849,                                           // HFreq, VFreq
        2288, 1105,                                         // HTotal, VTotal
        480, 52,                                            // HStart, VStart
    },
    // 123 1280x960 75Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1280x960,                                      // ResIndex
        752, 748,                                           // HFreq, VFreq
        1728, 1005,                                         // HTotal, VTotal
        360, 42,                                            // HStart, VStart
    },
    // 124 1920x1440 75Hz (VESA DMT)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1920x1440,                                      // ResIndex
        1125, 750,                                           // HFreq, VFreq
        2640, 1500,                                         // HTotal, VTotal
        576, 59,                                            // HStart, VStart
    },
    // 125 640x480 100Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_640x480,                                      // ResIndex
        510, 997,                                           // HFreq, VFreq
        832, 512,                                         // HTotal, VTotal
        160, 29,                                            // HStart, VStart
    },
    // 126 800x600 120Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_800x600,                                      // ResIndex
        774, 1198,                                           // HFreq, VFreq
        1072, 646,                                         // HTotal, VTotal
        216, 43,                                            // HStart, VStart
    },
    // 127 640x480 70Hz (VESA GTF)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_640x480,                                        // ResIndex
        350, 700,                                           // HFreq, VFreq
        816, 500,                                           // HTotal, VTotal
        152, 19,                                            // HStart, VStart
    },
    // 128 1280x600 60Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1280x600,                                      // ResIndex
        373, 599,                                           // HFreq, VFreq
        1632, 624,                                         // HTotal, VTotal
        304, 21,                                            // HStart, VStart
    },
    // 129 1792x1344 75Hz  (VESA GTF)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1792x1344,                                      // ResIndex
        1052, 750,                                          // HFreq, VFreq
        2464, 1403,                                         // HTotal, VTotal
        536, 58,                                            // HStart, VStart
    },
    // 130 800x600 100Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_800x600,                                      // ResIndex
        636, 996,                                           // HFreq, VFreq
        1056, 639,                                         // HTotal, VTotal
        208, 36,                                            // HStart, VStart
    },
    // 131 1152x864 85Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1152x864,                                      // ResIndex
        771, 847,                                           // HFreq, VFreq
        1552, 910,                                         // HTotal, VTotal
        320, 43,                                            // HStart, VStart
    },
    // 132 1152x864 100Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1152x864,                                      // ResIndex
        916, 998,                                           // HFreq, VFreq
        1568, 918,                                         // HTotal, VTotal
        330, 51,                                            // HStart, VStart
    },
    // 133 1280x720 85Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1280x720,                                      // ResIndex
        643, 848,                                           // HFreq, VFreq
        1712, 759,                                         // HTotal, VTotal
        350, 36,                                            // HStart, VStart
    },
    // 134 1280x720 100Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1280x720,                                      // ResIndex
        763, 997,                                           // HFreq, VFreq
        1728, 766,                                         // HTotal, VTotal
        360, 43,                                            // HStart, VStart
    },
    // 135 1280x720 120Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1280x720,                                      // ResIndex
        928, 1198,                                           // HFreq, VFreq
        1744, 775,                                         // HTotal, VTotal
        368, 52,                                            // HStart, VStart
    },
    // 136 1280x800 85Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1280x800,                                      // ResIndex
        715, 848,                                           // HFreq, VFreq
        1712, 843,                                         // HTotal, VTotal
        352, 40,                                            // HStart, VStart
    },
    // 137 1280x800 120Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1280x800,                                      // ResIndex
        1030, 1198,                                           // HFreq, VFreq
        1744, 860,                                         // HTotal, VTotal
        368, 57,                                            // HStart, VStart
    },
    // 138 1280x1024 85Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1280x1024,                                      // ResIndex
        914, 848,                                           // HFreq, VFreq
        1744, 1078,                                         // HTotal, VTotal
        368, 51,                                            // HStart, VStart
    },
    // 139 1280x1024 100Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1280x1024,                                      // ResIndex
        1086, 999,                                           // HFreq, VFreq
        1744, 1087,                                         // HTotal, VTotal
        368, 60,                                            // HStart, VStart
    },
    // 140 1280x1024 120Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1280x1024,                                      // ResIndex
        1318, 1198,                                           // HFreq, VFreq
        1760, 1100,                                         // HTotal, VTotal
        376, 73,                                            // HStart, VStart
    },
    // 141 1360x768 120Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1360x768,                                      // ResIndex
        990, 1198,                                           // HFreq, VFreq
        1856, 826,                                         // HTotal, VTotal
        392, 55,                                            // HStart, VStart
    },
    // 142 1400x1050 85Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1400x1050,                                      // ResIndex
        938, 849,                                           // HFreq, VFreq
        1912, 1105,                                         // HTotal, VTotal
        408, 52,                                            // HStart, VStart
    },
    // 143 1400x1050 120Hz  (VESA GTF)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1400x1050,                                      // ResIndex
        1350, 1200,                                          // HFreq, VFreq
        1944, 1125,                                         // HTotal, VTotal
        424, 74,                                            // HStart, VStart
    },
    // 144 1440x900 85Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1440x900,                                      // ResIndex
        804, 848,                                           // HFreq, VFreq
        1952, 948,                                         // HTotal, VTotal
        408, 45,                                            // HStart, VStart
    },
    // 145 1440x900 120Hz  (VESA GTF)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1440x900,                                       // ResIndex
        1158, 1200,                                         // HFreq, VFreq
        1984, 965,                                          // HTotal, VTotal
        432, 64,                                            // HStart, VStart
    },
    // 146 1600x900 85Hz  (VESA GTF)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1600x900,                                       // ResIndex
        803, 850,                                           // HFreq, VFreq
        2176, 945,                                          // HTotal, VTotal
        464, 44,                                            // HStart, VStart
    },
    // 147 1600x900 100Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1600x900,                                      // ResIndex
        954, 998,                                           // HFreq, VFreq
        2176, 956,                                         // HTotal, VTotal
        456, 53,                                            // HStart, VStart
    },
    // 148 1600x900 120Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1600x900,                                      // ResIndex
        1160, 1199,                                           // HFreq, VFreq
        2192, 967,                                         // HTotal, VTotal
        464, 64,                                            // HStart, VStart
    },
    // 149 1600x1200 85Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1600x1200,                                      // ResIndex
        1072, 849,                                           // HFreq, VFreq
        2192, 1262,                                         // HTotal, VTotal
        464, 59,                                            // HStart, VStart
    },
    // 150 1680x1050 120Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1680x1050,                                      // ResIndex
        1352, 1198,                                           // HFreq, VFreq
        2320, 1128,                                         // HTotal, VTotal
        504, 75,                                            // HStart, VStart
    },
    // 151 1920x1080 100Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1920x1080,                                      // ResIndex
        1145, 998,                                           // HFreq, VFreq
        2640, 1147,                                         // HTotal, VTotal
        568, 64,                                            // HStart, VStart
    },
    // 152 1600x1200 120Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1600x1200,                                      // ResIndex
        1544, 1198,                                           // HFreq, VFreq
        2224, 1289,                                         // HTotal, VTotal
        488, 86,                                            // HStart, VStart
    },
    // 153 1024x768 85Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn ,                                        // Flags
        Res_1024x768,                                       // ResIndex
        686, 848,                                           // HFreq, VFreq
        1376, 809,                                          // HTotal, VTotal
        280, 38,                                            // HStart, VStart
    },
    // 154 800x600 120Hz RB
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_800x600,                                      // ResIndex
        763, 1199,                                           // HFreq, VFreq
        960, 636,                                         // HTotal, VTotal
        112, 33,                                            // HStart, VStart
    },

    // 155 1024x768 120Hz RB
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn ,                                        // Flags
        Res_1024x768,                                       // ResIndex
        975, 1199,                                           // HFreq, VFreq
        1184, 813,                                          // HTotal, VTotal
        112, 42,                                            // HStart, VStart
    },
    // 156 1280x768 120Hz RB
    {
        SyncHnVp ,                                        // Flags
        Res_1280x768,                                       // ResIndex
        973, 1197,                                           // HFreq, VFreq
        1440, 813,                                          // HTotal, VTotal
        112,42,                                             // HStart, VStart
    },
    // 157 1280x800 120Hz RB
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1280x800,                                      // ResIndex
        1025, 1199,                                           // HFreq, VFreq
        1440, 847,                                         // HTotal, VTotal
        112, 44,                                            // HStart, VStart
    },
    // 158 1280x960 120Hz RB
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1280x960,                                      // ResIndex
        1218, 1198,                                           // HFreq, VFreq
        1440, 1017,                                         // HTotal, VTotal
        112, 54,                                            // HStart, VStart
    },
    // 159 1280x1024 120Hz RB
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1280x1024,                                      // ResIndex
        1300, 1199,                                           // HFreq, VFreq
        1440, 1084,                                         // HTotal, VTotal
        112, 57,                                            // HStart, VStart
    },
    // 160 1360x768 120Hz RB
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1360x768,                                      // ResIndex
        975, 1199,                                           // HFreq, VFreq
        1520, 813,                                         // HTotal, VTotal
        112, 42,                                            // HStart, VStart
    },
    // 161 1440x1050 120Hz RB
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1440x1050,                                      // ResIndex
        1334, 1199,                                           // HFreq, VFreq
        1600, 1112,                                         // HTotal, VTotal
        112, 59,                                            // HStart, VStart
    },
    // 162 1440x900 120Hz RB
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1440x900,                                      // ResIndex
        1142, 1199,                                           // HFreq, VFreq
        1600, 953,                                         // HTotal, VTotal
        112, 50,                                            // HStart, VStart
    },
    // 163 1680x1050 120Hz RB
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1680x1050,                                      // ResIndex
        1334, 1199,                                           // HFreq, VFreq
        1840, 1112,                                         // HTotal, VTotal
        112, 59,                                            // HStart, VStart
    },
    // 164 1366x768 60Hz RB
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1366x768,                                      // ResIndex
        473, 599,                                           // HFreq, VFreq
        1520, 790,                                         // HTotal, VTotal
        112, 19,                                            // HStart, VStart
    },
    // 165 1600x900 60Hz RB (VESA DMT)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1600x900,                                      // ResIndex
        600, 600,                                           // HFreq, VFreq
        1800, 1000,                                         // HTotal, VTotal
        176, 99,                                            // HStart, VStart
    },
    // 166 640x480 50Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_640x480,                                      // ResIndex
        246, 496,                                           // HFreq, VFreq
        800, 497,                                         // HTotal, VTotal
        144, 14,                                            // HStart, VStart
    },
    // 167 800x600 50Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_800x600,                                      // ResIndex
        309, 499,                                           // HFreq, VFreq
        992, 621,                                         // HTotal, VTotal
        168, 18,                                            // HStart, VStart
    },
    // 168 1280x1024 50Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1280x1024,                                      // ResIndex
        526, 498,                                           // HFreq, VFreq
        1680, 1057,                                         // HTotal, VTotal
        328, 30,                                            // HStart, VStart
    },
    // 169 1280x768 50Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1280x768,                                      // ResIndex
        395, 499,                                           // HFreq, VFreq
        1648, 793,                                         // HTotal, VTotal
        312, 22,                                            // HStart, VStart
    },
    // 170 848x480 50Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_848x480,                                      // ResIndex
        246, 495,                                           // HFreq, VFreq
        1056, 497,                                         // HTotal, VTotal
        184, 14,                                            // HStart, VStart
    },
    // 171 1360x768 50Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1360x768,                                      // ResIndex
        395, 498,                                           // HFreq, VFreq
        1744, 793,                                         // HTotal, VTotal
        328, 22,                                            // HStart, VStart
    },
    // 172 1360x768 75Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1360x768,                                      // ResIndex
        602, 748,                                           // HFreq, VFreq
        1808, 805,                                         // HTotal, VTotal
        368, 34,                                            // HStart, VStart
    },
    // 173 1920x1080 60Hz RB
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1920x1080,                                      // ResIndex
        665, 599,                                           // HFreq, VFreq
        2080, 1111,                                         // HTotal, VTotal
        112, 28,                                            // HStart, VStart
    },
    // 174 1280x960 50Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1280x960,                                      // ResIndex
        494, 498,                                           // HFreq, VFreq
        1680, 991,                                         // HTotal, VTotal
        328, 28,                                            // HStart, VStart
    },
    // 175 1400x1050 50Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1400x1050,                                      // ResIndex
        541, 499,                                           // HFreq, VFreq
        1848, 1083,                                         // HTotal, VTotal
        368, 30,                                            // HStart, VStart
    },
    // 176 1280x768 120Hz (VESA GTF)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1280x768,                                       // ResIndex
        988, 1200,                                          // HFreq, VFreq
        1760, 823,                                          // HTotal, VTotal
        384, 54,                                            // HStart, VStart
    },
    // 177 2560x1600 60Hz
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_2560x1600,                                      // ResIndex
        995, 600,                                           // HFreq, VFreq
        3504, 1658,                                         // HTotal, VTotal
        752, 55,                                            // HStart, VStart
    },
    // 178 1280x960 60Hz (VESA DMT)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1280x960,                                       // ResIndex
        600, 600,                                           // HFreq, VFreq
        1800, 1000,                                          // HTotal, VTotal
        424, 39,                                            // HStart, VStart
    },
    // 179 1400x1050 60Hz  (VESA GTF)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1400x1050,                                      // ResIndex
        652, 600,                                           // HFreq, VFreq
        1880, 1087,                                         // HTotal, VTotal
        392, 36,                                            // HStart, VStart
    },
    // 180 1600x900 60Hz  (VESA GTF)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1600x900,                                       // ResIndex
        559, 600,                                           // HFreq, VFreq
        2128, 932,                                          // HTotal, VTotal
        432, 31,                                            // HStart, VStart
    },
    // 181 1600x1200 85Hz  (VESA DMT)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1600x1200,                                      // ResIndex
        1063, 850,                                          // HFreq, VFreq
        2160, 1250,                                         // HTotal, VTotal
        496, 49,                                            // HStart, VStart
    },
    // 182 1680x1050 60Hz  (VESA GTF)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1680x1050,                                      // ResIndex
        652,600,                                            // HFreq, VFreq
        2256, 1087,                                         // HTotal, VTotal
        472, 36,                                            // HStart, VStart
    },
    // 183 1680x1050 75Hz  (VESA GTF)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1680x1050,                                      // ResIndex
        822,750,                                            // HFreq, VFreq
        2288, 1096,                                         // HTotal, VTotal
        488, 45,                                            // HStart, VStart
    },
    // 184 1920x1200 75Hz  (VESA GTF)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1920x1200,                                      // ResIndex
        940, 750,                                           // HFreq, VFreq
        2624, 1253,                                         // HTotal, VTotal
        560, 52,                                            // HStart, VStart
    },
    // 185 1920x1440 60Hz  (VESA DMT)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1920x1440,                                      // ResIndex
        900, 600,                                           // HFreq, VFreq
        2600, 1500,                                         // HTotal, VTotal
        552, 59,                                            // HStart, VStart
    },
    //186 1920x1080 50Hz  (CVT)
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn,  // Flags
        Res_1920x1080,                                      // ResIndex
        556, 499,                                           // HFreq, VFreq
        2544, 1114,                                         // HTotal, VTotal
        506, 25,                                            // HStart, VStart
    },
#if  _3D_TIMING
//1920x1080 24Hz FP
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1920x2160, // ResIndex
        541, 240, // HFreq, VFreq
        2750, 2250, // HTotal, VTotal
        192, 41, // HStart, VStart
    },
    //1280x72P 60Hz FP
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x720P_FP, // resolution index
        900, 600,      // HFreq, VFreq
        1650, 1500,    // HTotal, VTotal
        220, 20,       // HStart, VStart
    },
    //76, 1280x72P 50Hz FP
    {
        SyncHpVp | SyncHpVn | SyncHnVp | SyncHnVn, // Flags
        Res_1280x720P_FP, // resolution index
        750, 500,      // HFreq, VFreq
        1980, 1500,    // HTotal, VTotal
        220, 20,       // HStart, VStart
    },


#endif
    //=====IMPORTANT    DON'T DELETE IT, OR IT WILL CAUSE LOOP INIFINITE
    {
        0, // Flags
        0, // ResIndex
        0, 0, // HFreq, VFreq
        0, 0, // HTotal, VTotal
        0, 0, // HStart, VStart
    }
};
WORD GetStandardModeWidth( void )
{
    return StandardModeResolution[StandardModeGroup].DispWidth;
}
WORD GetStandardModeHeight( void )
{
    return StandardModeResolution[StandardModeGroup].DispHeight;
}
WORD GetStandardModeGroup( void )
{
    return StandardMode[SrcModeIndex].ResIndex;
}
WORD GetStandardModeHStart( void)
{
    return StandardMode[SrcModeIndex].HStart;
}
WORD GetStandardModeVStart( void)
{
    return StandardMode[SrcModeIndex].VStart;
}
WORD GetStandardModeVTotal( void )
{
    return StandardMode[SrcModeIndex].VTotal;
}
WORD GetStandardModeHTotal( void )
{
    return StandardMode[SrcModeIndex].HTotal;
}
WORD GetStandardModeHFreq( void )
{
    return StandardMode[SrcModeIndex].HFreq;
}
WORD GetStandardModeVFreq( void )
{
    return StandardMode[SrcModeIndex].VFreq;
}
//========================================================================================
#if 1
#define PHASE_MAX          (0x80)
#define PHASE_STEP         (0x07)
#define PHASE_CENTER    0x40

#define CHECK_MODE_WIDTH   0

#if ENABLE_VGA_INPUT
void appCheckSimilarTiming(void)
{
    WORD xdata wWidth;
    BYTE vsyncTime, i, ucIndex, uccount;
    DWORD maxcksum, cksum, maxcksum1;
    BYTE adjPhase;

    if(ucSimilarTimingNums <= 1 || (SrcFlags&bUnsupportMode))
        return;

    vsyncTime = GetVSyncTime();
    i = 0;
    maxcksum = maxcksum1 = 0;
    ucIndex = SrcModeIndex;
    uccount = (ucSimilarTimingNums>=SIMILAR_TIMING_BUFFER_MAX)?(SIMILAR_TIMING_BUFFER_MAX):(ucSimilarTimingNums);

            //printData(" ucSimilarTimingNums:%d", ucSimilarTimingNums);

    while(uccount--)
    {
        //111027 Rick add for reload ADC table while check similar timing - B39609
        wWidth = HFreq( SrcHPeriod );//(( DWORD )MST_CLOCK_MHZ * 10 + SrcHPeriod / 2 ) / SrcHPeriod; //calculate hfreq: round 5
        wWidth = (( DWORD )wWidth * StandardModeHTotal + 5000 ) / 10000; //dclk= hfreq * htotal
        drvADC_SetModewithPLLProtection(g_bInputSOGFlag ? ADC_INPUTSOURCE_YPBPR : ADC_INPUTSOURCE_RGB, wWidth, StandardModeHTotal);

        drvADC_AdjustHTotal(StandardModeHTotal);

#if CHECK_MODE_WIDTH
        wWidth = SearchMaxWidth(vsyncTime);

        //printData(" width:%d", wWidth);

        if(abs(wWidth - StandardModeWidth) < 2)     //110930 Rick modified check similar timing condition
        {
            //printData(" similar width:%d", wWidth);

            return;
        }
#endif

        SC0_SET_IMAGE_WIDTH(StandardModeWidth);//old_msWrite2Byte( SC0_0B, StandardModeWidth);
        SC0_SET_IMAGE_HEIGHT(StandardModeHeight);//old_msWrite2Byte( SC0_09, StandardModeHeight );

        for( adjPhase = 0; adjPhase < PHASE_MAX; adjPhase += PHASE_STEP)
        {
            drvADC_SetPhaseCode( adjPhase );
            Delay1ms( vsyncTime + 3 );
            AUTO_PHASE_RESULT_READY();//WaitAutoStatusReady(SC0_8B, BIT1);
            //cksum = old_msRead2Byte(SC0_8E);
            //cksum = ( cksum << 16 ) | old_msRead2Byte(SC0_8C);
            cksum = AUTO_PHASE_READ_VALUE();
            if( cksum > maxcksum )
            {
                maxcksum = cksum;
            }
        }

        if(maxcksum > maxcksum1)
        {
            maxcksum1 = maxcksum;
            ucIndex = SrcModeIndex;
            //printData(" SrcModeIndex_MaxCksm:%d", SrcModeIndex);
        }

        SrcModeIndex = ucSimilarTimingBuffer[i++];

    }

    SrcModeIndex = ucIndex;

    if (SrcModeIndex==MD_640x480_60_VESA || SrcModeIndex==MD_720x480_60_YCbCr)
    {
        if (GetVSyncWidth()==6)
            SrcModeIndex = MD_720x480_60_YCbCr;
        else
            SrcModeIndex = MD_640x480_60_VESA;
    }
    else if ((ucIndex == MD_1366x768_60)||(ucIndex == MD_1360x768_60_VESA)||(ucIndex == MD_1360x768_60))
    {
        if(GetVSyncWidth() == 6)
            SrcModeIndex = MD_1360x768_60_VESA;
        else
            SrcModeIndex = MD_1366x768_60;
    }
    else if((ucIndex==MD_1280x960_60_VESA)||(ucIndex==MD_1600X900_60_VESA))
    {
        if(GetVSyncWidth()==3)
        {
            SrcModeIndex = MD_1600X900_60_VESA;
        }
        else
        {
            SrcModeIndex = MD_1280x960_60_VESA;
        }
    }
    else if((ucIndex==MD_1280x768_50)||(ucIndex==MD_1360x768_50))
    {
        if(GetVSyncWidth()==7)
        {
            SrcModeIndex = MD_1280x768_50;
        }
        else
        {
            SrcModeIndex = MD_1360x768_50;
        }
    }
    else if((ucIndex==MD_848x480_50)||(ucIndex==MD_640x480_50))
    {
        if(GetVSyncWidth()==4)
        {
            SrcModeIndex = MD_640x480_50;
        }
        else
        {
            SrcModeIndex = MD_848x480_50;
        }
    }
            //printData(" SrcModeIndexFinal:%d", SrcModeIndex);
}
#endif

#define DIGITAL_TOLERANCE   3
Bool mStar_FindMode( void )
{
    WORD hFreq, vFreq = 0;
    BYTE htol, vtol;
    WORD tempHeight=0, tempWidth=0;
    bit bDigitalInput = 0;

#define fStatus  hFreq
    fStatus = mStar_GetInputStatus();
    if( SyncPolarity( SrcFlags ) != SyncPolarity( fStatus ) )        // Sync polarity changed
    {
        return FALSE;
    }
    SrcFlags &= 0x0F;
#undef fStaus

    if (!IS_DP_FREESYNC())
    {
        hFreq = (SC0_HPERIOD_16LINE_MODE() ? (SC0_READ_HPEROID()+8)/16 : (SC0_READ_HPEROID()+4)/8);
        if( abs( hFreq - SrcHPeriod ) > HPeriod_Torlance )        // HPeriod changed
        {
            return FALSE;
        }
    }

    if (!IS_HDMI_FREESYNC())
    {
        vFreq = SC0_READ_VTOTAL();//old_msRead2Byte(SC0_E2) & 0xFFF;
        if( abs( vFreq - SrcVTotal ) > VTotal_Torlance )        // vtotal changed
        {
            return FALSE;
        }
    }

    if(IS_HDMI_FREESYNC())
        msDrv_forceIP1VsyncPolarityEnable();

    if(IS_DP_FREESYNC())
    {
        WORD u16PixClk, u16Htt;

        u16PixClk = mapi_DPRx_GetPixelClock10K(SrcInputType);//msAPI_combo_IPGetPixelClk();
        u16Htt = msAPI_combo_IPGetGetHTotal();
        if((u16Htt!=0)&&(SrcVTotal!=0))
        {
            hFreq = ((DWORD)u16PixClk * 100 + u16Htt/2)/u16Htt;
            vFreq = ((DWORD)hFreq * 1000 + SrcVTotal/2)/SrcVTotal;
            SrcHPeriod = (CRYSTAL_CLOCK + (hFreq * 100) / 2) / (hFreq * 100);
        }
    }
    else
    {
        hFreq = HFreq( SrcHPeriod );
        vFreq = VFreq( hFreq, SrcVTotal );
    }

    //======== for interlace mode
    if( ((SC0_READ_SYNC_STATUS()&INTM_B) && ((!IS_HDMI_FREESYNC()))) //old_msReadByte( SC0_E1 ) &INTM_B
#if ENABLE_DP_INPUT
||(CURRENT_INPUT_IS_DISPLAYPORT() && msAPI_combo_IPGetInterlace() )
#endif
    )
    {
        SrcFlags |= bInterlaceMode;
        vFreq *= 2;
    }

    SrcVFreq = vFreq;
    #if CHIP_ID == CHIP_MT9701
    SrcIVSPeriod = msDrvGetIVSPeriod();
    if(!IS_DP_FREESYNC())
        SrcVFreq = SrcIVSPeriod?(((CRYSTAL_CLOCK*10)+(SrcIVSPeriod>>1))/SrcIVSPeriod):vFreq;
    #endif

    MODE_printData( " FindMode_hFreq:%d", hFreq );
    MODE_printData( " FindMode_vFreq:%d", vFreq );
    MODE_printData( " SrcVFreq:%d", SrcVFreq );
    MODE_printData( " SrcHPeriod:%d", SrcHPeriod );

    if(Input420Flag)
    {
        MODE_printMsg( "YUV420 Format" );
        SET_420to444_ENABLE();
    }
    else
        SET_420to444_DISABLE();

    //====================================
    // check if input timing is out of range
    htol = HFreq_Torlance; //0527 add for getting more accurate mode
    vtol = VFreq_Torlance;

    ucSimilarTimingNums = 0;
    SrcModeIndex = 0xFF;

    if(!CURRENT_INPUT_IS_VGA())//(SrcInputType >= Input_Digital)
    {
        bDigitalInput = 1;
        tempWidth = SC0_READ_AUTO_WIDTH();
        tempHeight = SC0_READ_AUTO_HEIGHT();
        SrcModeIndex = 0x00;
	return TRUE;
    }

    if (g_SetupPathInfo.ucSCFmtIn == SC_FMT_IN_3D_FP)
        tempHeight=tempHeight*2-SrcVTotal;

    // search input mode index
    {
        Bool found = FALSE;
        BYTE ucModeIndex;
        BYTE ucHVtolerance_min = HFreq_Torlance+VFreq_Torlance;
        BYTE ucHVtolerance = HFreq_Torlance+VFreq_Torlance;
        BYTE ucVtotal_min = VTotal_Torlance;
        BYTE ucVtotal = VTotal_Torlance;
        const InputModeType *modePtr = StandardMode;
        BYTE ucSimilarindex=0;

        // get standard mode index
        for(ucModeIndex = 0; modePtr->HFreq > 0; modePtr++, ucModeIndex++)
        {
        if(bDigitalInput)
        {
                if((abs(tempWidth - StandardModeResolution[modePtr->ResIndex].DispWidth) > DIGITAL_TOLERANCE) ||
                        (abs(tempHeight - StandardModeResolution[modePtr->ResIndex].DispHeight) > DIGITAL_TOLERANCE))
                    continue;
        }

            if((CURRENT_SOURCE_IS_INTERLACE_MODE()) != (modePtr->Flags & bInterlaceMode))
                continue;

            if(SrcVTotal < StandardModeResolution[modePtr->ResIndex].DispHeight)
                continue;

            if((abs(hFreq - modePtr->HFreq) < HFreq_Torlance) &&
                (abs(vFreq - modePtr->VFreq) < VFreq_Torlance) &&
                (abs(SrcVTotal - modePtr->VTotal) < VTotal_Torlance)&&
                (GetSyncPolarity( SrcFlags )&modePtr->Flags))
            {
                ucHVtolerance = abs(hFreq - modePtr->HFreq) + abs(vFreq - modePtr->VFreq);
                ucVtotal = abs(SrcVTotal - modePtr->VTotal);

                if((ucHVtolerance_min >= ucHVtolerance) && (ucVtotal_min >= ucVtotal))
                {
                    found = TRUE;
                    //ucHVtolerance_min = ucHVtolerance;
                    //ucVtotal_min = ucVtotal;
                    SrcModeIndex = ucModeIndex;
                    ucSimilarTimingBuffer[ucSimilarindex] = ucModeIndex;
                    ucSimilarTimingNums ++;
                    ucSimilarindex = (ucSimilarindex+1)%SIMILAR_TIMING_BUFFER_MAX;
                }
            }
        }

        if( !found )
        {
            modePtr = StandardMode;
            ucHVtolerance_min = HFREQ_TOLERANCE_ENLARGE + VFREQ_TOLERANCE_ENLARGE;
            ucVtotal_min = VTotal_Delta;

            for(ucModeIndex = 0; modePtr->HFreq > 0; modePtr++, ucModeIndex++)
            {
                if(bDigitalInput)
                {
                    if((abs(tempWidth - StandardModeResolution[modePtr->ResIndex].DispWidth) > DIGITAL_TOLERANCE) ||
                        (abs(tempHeight - StandardModeResolution[modePtr->ResIndex].DispHeight) > DIGITAL_TOLERANCE))
                        continue;
                }

                if((CURRENT_SOURCE_IS_INTERLACE_MODE()) != (modePtr->Flags & bInterlaceMode))
                    continue;

                if(SrcVTotal < StandardModeResolution[modePtr->ResIndex].DispHeight)
                    continue;

                if((abs(hFreq - modePtr->HFreq) < HFREQ_TOLERANCE_ENLARGE) &&
                    (abs(vFreq - modePtr->VFreq) < VFREQ_TOLERANCE_ENLARGE) &&
                    (abs(SrcVTotal - modePtr->VTotal) < VTotal_Delta))
                {
                    ucHVtolerance = abs(hFreq - modePtr->HFreq) + abs(vFreq - modePtr->VFreq);
                    ucVtotal = abs(SrcVTotal - modePtr->VTotal);

                    if((ucHVtolerance_min >= ucHVtolerance) && (ucVtotal_min >= ucVtotal))
                    {
                        found = TRUE;
                        ucHVtolerance_min = ucHVtolerance;
                        ucVtotal_min = ucVtotal;
                        SrcModeIndex = ucModeIndex;
                        ucSimilarTimingBuffer[ucSimilarindex] = ucModeIndex;
                        ucSimilarTimingNums ++;
                        ucSimilarindex = (ucSimilarindex+1)%SIMILAR_TIMING_BUFFER_MAX;
                    }
                }
            }
        }

        if( !found )            // out of standard input range
        {
            modePtr = StandardMode;
            ucVtotal_min = VTotal_Delta;

            for(ucModeIndex = 0; modePtr->HFreq > 0; modePtr++, ucModeIndex++)
            {
                if((CURRENT_SOURCE_IS_INTERLACE_MODE()) != (modePtr->Flags & bInterlaceMode))
                    continue;

                if(SrcVTotal < StandardModeResolution[modePtr->ResIndex].DispHeight)
                    continue;

                if(abs(SrcVTotal - modePtr->VTotal) < VTotal_Delta)
                {
                    ucVtotal = abs(SrcVTotal - modePtr->VTotal);

                    if(ucVtotal_min > ucVtotal)
                    {
                        found = TRUE;
                        SrcFlags |= bUserMode;
                        ucVtotal_min = ucVtotal;
                        SrcModeIndex = ucModeIndex;
                        ucSimilarTimingBuffer[ucSimilarindex] = ucModeIndex;
                        ucSimilarTimingNums ++;
                        ucSimilarindex = (ucSimilarindex+1)%SIMILAR_TIMING_BUFFER_MAX;
                    }
                }
            }
        } // out of standard input range
#if ENABLE_VGA_INPUT
        if(CURRENT_INPUT_IS_VGA())
            appCheckSimilarTiming();
#endif
        if( !found )
        {

            MODE_printMsg( "Not Found--> not support" );
    	    SrcModeIndex = 0x00;
            //BenLu
            //SrcFlags |= bUnsupportMode;
        }
        else   // found
        {
                MODE_printData("  ucSimilarTimingNums:%d", ucSimilarTimingNums);
                MODE_printData("  SrcModeIndex:%d", SrcModeIndex);
                MODE_printData("  StandardModeWidth:%d", StandardModeWidth);
                MODE_printData("  StandardModeHeight:%d", StandardModeHeight);
        }

    } // search mode index

    return TRUE;
}
#endif




