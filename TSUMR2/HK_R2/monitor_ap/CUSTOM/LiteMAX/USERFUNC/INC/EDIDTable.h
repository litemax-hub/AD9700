
#if (CHIP_ID == CHIP_MT9700)
#include "EDIDTable_MT9700.h"
#elif (CHIP_ID == CHIP_MT9701)
#include "EDIDTable_MT9701.h"
#else
#warning "No reference EDID table!"
#endif
