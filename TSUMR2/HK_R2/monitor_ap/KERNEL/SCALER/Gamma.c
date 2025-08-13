
//#define _MSGMATBL_C_

#include "board.h"


#if Enable_Gamma
#define _GAMMA_C_
#include "types.h"
#include "drvGamma.h"
//#include "Panel_GAMMATBL.h"
#include "ms_reg.h"
#include "Gamma.h"
#include "Ms_rwreg.h"
#include "Common.h"
#include "Global.h"
#include "menudef.h"
#include "Adjust.h"
#include "misc.h"



BYTE xdata GammaContent = GAMA_Nums;

void mStar_SetupGamma( BYTE GamaMode )
{
#ifdef TSUMR2_FPGA
    return;
#endif

    if(GammaContent == GamaMode)
        return;

    mStar_WaitForDataBlanking();
    drvGammaOnOff(_DISABLE, MAIN_WINDOW );
    drvGammaOnOff(_DISABLE, SUB_WINDOW );
    drvGammaLoadTbl( GamaMode );
    mStar_WaitForDataBlanking();
    drvGammaOnOff(_ENABLE, MAIN_WINDOW );
    drvGammaOnOff(_ENABLE, SUB_WINDOW );
    GammaContent = GamaMode;
}

#endif
