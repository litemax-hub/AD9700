//=============================================================================================================================================================================================================================:
#define _FactoryAligment_C__
#include <string.h>
#include "datatype.h"
#include "Board.h"
#include "Global.h"
#include "msOSD.h"
#include "menudef.h"
#include "menufunc.h"
#include "DDC2Bi.H"
#include "NVRam.h"
#include "Adjust.h"
#include "ms_reg.h"
#include "Power.h"
#include "Ms_rwreg.h"
#include "misc.h"
#include "i2c.h"
#include "UserPref.h"
#include "Menu.h"
#include "FactoryAligment.h"
#include "drvDDC2Bi.h"
#include "UserPrefDef.h"

//=============================================================================================================================================================================================================================:
#if ENABLE_BOE_NEW_SZ_DDCCI_SPEC
#include "FactoryAligment_BOE.c"
#endif

//=============================================================================================================================================================================================================================:
#undef _FactoryAligment_C__

