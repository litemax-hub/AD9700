#include "board.h"
#include "drvLutAutoDownload.h"

#if ENABLE_LUT_AUTODOWNLOAD
#include "msFB.h"

void ColorFuncDramSizeCheck(void)
{
    // To be noticed! This function is used to check NVRam size for color functions.
    // If the address of color data block is greater than NVRam size, it will trigger compile error.
    // Consequently, developer have to check whether whole of the EEPROM data structures(EepromMap.h) are necessary.
    // And also check if NVRam size is expected.
	#if ENABLE_LUT_AUTODOWNLOAD
	BUILD_BUG_ON(ALIGN_4K(DV_ADL_ADDR+DV_ADL_SIZE)-DEGAMMA_ADL_ADDR > MIU_AUTOLOAD_SIZE);
	#endif
}

#endif

