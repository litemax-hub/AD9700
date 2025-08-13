#ifndef _DRVLUTAUTODOWNLOAD_H_
#define _DRVLUTAUTODOWNLOAD_H_

#if ENABLE_LUT_AUTODOWNLOAD
#define DEGAMMA_ADL_SIZE        0x3000 // 32 * 384 (1024E/8entries(per dram data)*(256/8)*3(RGB)
#define FIXGAMMA_ADL_SIZE       0x840 //(256+88)/16 * (256/8) * 3
#define POSTGAMMA_ADL_SIZE      0x1800 //1024E * 2byte * 3
#define RGB3DLUT_ADL_SIZE		0xFA0
#define HDR_ADL_SIZE			0x2000
#if (ENABLE_DOLBY_HDR)
#define DV_ADL_SIZE				0xA000
#else
#define DV_ADL_SIZE				0x0000
#endif
#define DEGAMMA_ADL_ADDR        ALIGN_4K(MIU_AUTOLOAD_ADDR_START)
#define FIXGAMMA_ADL_ADDR       (DEGAMMA_ADL_ADDR + DEGAMMA_ADL_SIZE) //don't need align 4k,due to degamma & fixgamma is continueous memory block(same client)
#define POSTGAMMA_ADL_ADDR      ALIGN_4K(FIXGAMMA_ADL_ADDR + FIXGAMMA_ADL_SIZE)
#define RGB3DLUT_ADL_ADDR		ALIGN_4K(POSTGAMMA_ADL_ADDR+POSTGAMMA_ADL_SIZE)
#define HDR_ADL_ADDR			ALIGN_4K(RGB3DLUT_ADL_ADDR+RGB3DLUT_ADL_SIZE)
#define DV_ADL_ADDR				ALIGN_4K(HDR_ADL_ADDR+HDR_ADL_SIZE)
#endif
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

#endif
