
#    include project/build/FILES_R2_MST9U_ZUI_256_256MB.mk


#    -DENABLE_DMP=1                                                  \
#    for above slash

CC_MTOPTS += \
    -DOBA2=0                                                        \
    -DMSOS_TYPE_NOS                                                 \
    -DENABLE_DMP=0                                                  \
    -DENABLE_PIP=0                                                  \
    -DFOR_BENCH_CODE=0                                              \
    -DENCODE_AUDIO_AP=0                                             \
    -DENABLE_DEMO_FINE_TUNE=0                                       \
    -DENABLE_3D=0                                                   \
    -DENABLE_FORCE_MM_HD_FB=0                                       \
    -DENABLE_MIRROR=0                                               \
#    -DModelName=MST9U_DEMO                                           \

#aeon1/aeonR2/aeon2
AEON_TYPE=aeonR2
CC_MTOPTS += -DMCU_AEON=1

##################################################################
# Bootloader
##################################################################
BL_SRC_FILE =

##################################################################
# Monitor Code
##################################################################
MONITOR_PATH                            = $(ROOT)/monitor_ap
MONITOR_PATH_DRIVER                 = $(MONITOR_PATH)/DRIVER
MONITOR_PATH_EXTDEVICE            = $(MONITOR_PATH)/EXTDEVICE
MONITOR_PATH_KERNEL_SCALER     = $(MONITOR_PATH)/KERNEL/SCALER
MONITOR_PATH_KERNEL_SYSTEM    = $(MONITOR_PATH)/KERNEL/SYSTEM
MONITOR_PATH_LIB                       = $(MONITOR_PATH)/LIB
MONITOR_PATH_LIB_SRC                = $(ROOT)/../LIB_SRC
#MONITOR_PATH_LIB_SRC              = $(ROOT)/lib_src/monitor_lib
MONITOR_PATH_PANEL                   = $(MONITOR_PATH)/PANEL
MONITOR_PATH_CUSTOM_MTK        = $(MONITOR_PATH)/CUSTOM/MTK
MONITOR_PATH_CUSTOM               = $(MONITOR_PATH_CUSTOM_MTK)
MONITOR_PATH_CUSTOM_UI           = $(MONITOR_PATH_CUSTOM)/UI
MONITOR_PATH_CUSTOM_USERDATA   = $(MONITOR_PATH_CUSTOM)/USERDATA
MONITOR_PATH_CUSTOM_USERFUNC   = $(MONITOR_PATH_CUSTOM)/USERFUNC

#MONITOR_PATH_CUSTOM_MSTAR_UI         = $(MONITOR_PATH)/CUSTOM/MSTAR/UI
#MONITOR_PATH_CUSTOM_MSTAR_SI         = $(MONITOR_PATH)/CUSTOM/MSTAR/UI/SI
#MONITOR_PATH_CUSTOM_MSTAR_ZUI        = $(MONITOR_PATH)/CUSTOM/MSTAR/ZUI
#MONITOR_PATH_CUSTOM_MSTAR_APP        = $(MONITOR_PATH)/CUSTOM/MSTAR/APP
#MONITOR_PATH_CUSTOM_MSTAR_USERDATA   = $(MONITOR_PATH)/CUSTOM/MSTAR/USERDATA
#MONITOR_PATH_CUSTOM_MSTAR_USERFUN    = $(MONITOR_PATH)/CUSTOM/MSTAR/USERFUN
#MONITOR_PATH_CUSTOM_MSTAR_USERFUN_SI = $(MONITOR_PATH)/CUSTOM/MSTAR/USERFUN/SI


MONITOR_FILES_DRIVER  += \
        $(MONITOR_PATH_DRIVER)/9700/drvAdjust.c                            \
        $(MONITOR_PATH_DRIVER)/9700/drvMcu.c                               \
        $(MONITOR_PATH_DRIVER)/9700/msScaler.c                             \
        $(MONITOR_PATH_DRIVER)/9700/appmStar.c                             \
        $(MONITOR_PATH_DRIVER)/9700/drvmStar.c                             \
        $(MONITOR_PATH_DRIVER)/9700/msFB.c                                  \
        $(MONITOR_PATH_DRIVER)/9700/drvmsOVD.c                             \
        $(MONITOR_PATH_DRIVER)/9700/drvDAC.c                                \
        $(MONITOR_PATH_DRIVER)/9700/drvGamma.c                         \
        $(MONITOR_PATH_DRIVER)/9700/drvCEC.c                            \
        $(MONITOR_PATH_DRIVER)/9700/drvIMI.c                           \
		$(MONITOR_PATH_DRIVER)/9700/drvDeltaE.c                            \
        $(MONITOR_PATH_DRIVER)/drvDDC2Bi.c                            \
        $(MONITOR_PATH_DRIVER)/drvBDMA.c                            \
        $(MONITOR_PATH_DRIVER)/drvUARTPIU.c                        \
        $(MONITOR_PATH_DRIVER)/ms3DLUT.c                            \
		$(MONITOR_PATH_DRIVER)/drv3DLUT_C.c                            \
        $(MONITOR_PATH_DRIVER)/apiHDR.c                            \
        $(MONITOR_PATH_DRIVER)/drvHDR.c                            \
        $(MONITOR_PATH_DRIVER)/drvHDRCommon.c                 \
        $(MONITOR_PATH_DRIVER)/drvHDRLUT.c                     \
        $(MONITOR_PATH_DRIVER)/USB_Device/drvUSBDwld.c      \
        $(MONITOR_PATH_DRIVER)/USB_Device/ms_dma.c       \
        $(MONITOR_PATH_DRIVER)/USB_Device/ms_drc.c       \
        $(MONITOR_PATH_DRIVER)/USB_Device/ms_function.c       \
        $(MONITOR_PATH_DRIVER)/USB_Device/ms_hid.c            \
        $(MONITOR_PATH_DRIVER)/USB_Device/ms_msd_global.c       \
        $(MONITOR_PATH_DRIVER)/USB_Device/ms_otg.c       \
        $(MONITOR_PATH_DRIVER)/USB_Device/ms_usb.c       \
        $(MONITOR_PATH_DRIVER)/USB_Device/msd_fn_scsi.c       \
        $(MONITOR_PATH_DRIVER)/USB_Device/msd_fn_xfer.c       \
        $(MONITOR_PATH_DRIVER)/USB_Host/source/usb_host_p1/drvHost200.c                    \
        $(MONITOR_PATH_DRIVER)/USB_Host/source/usb_host_p1/drvUsbHostLib.c                \
        $(MONITOR_PATH_DRIVER)/USB_Host/source/usb_host_p1/drvHostLib.c                     \
        $(MONITOR_PATH_DRIVER)/USB_Host/source/usb_host_p1/drvMassStorage.c              \
        $(MONITOR_PATH_DRIVER)/USB_Host/source/usb_host_p1/drvScsi.c                          \
        $(MONITOR_PATH_DRIVER)/USB_Host/source/usb_host_p1/drvUsbMain.c                     \


#        $(MONITOR_PATH_DRIVER)/drv_msHDMI.c                      \
#        $(MONITOR_PATH_DRIVER)/drvMHL.c                              \
#        $(MONITOR_PATH_DRIVER)/drvDPRxApp.c                       \


MONITOR_FILES_EXTDEVICE  += \
        $(MONITOR_PATH_EXTDEVICE)/TouchPanel.c             \
        $(MONITOR_PATH_EXTDEVICE)/TouchKey.c               \
        $(MONITOR_PATH_EXTDEVICE)/USB.c               \

MONITOR_FILES_KERNEL_SCALER  += \
        $(MONITOR_PATH_KERNEL_SCALER)/LoadPropFont1218.c          \
        $(MONITOR_PATH_KERNEL_SCALER)/drvGPIO.c                              \
        $(MONITOR_PATH_KERNEL_SCALER)/mStar.c               \
        $(MONITOR_PATH_KERNEL_SCALER)/detect.c              \
        $(MONITOR_PATH_KERNEL_SCALER)/Adjust.c              \
        $(MONITOR_PATH_KERNEL_SCALER)/DDC.c                 \
        $(MONITOR_PATH_KERNEL_SCALER)/msMenuload.c      \
        $(MONITOR_PATH_KERNEL_SCALER)/AutoFunc.c          \
        $(MONITOR_PATH_KERNEL_SCALER)/drvOSD.c            \
        $(MONITOR_PATH_KERNEL_SCALER)/AutoGamma.c    \
        $(MONITOR_PATH_KERNEL_SCALER)/drvADC.c            \
        $(MONITOR_PATH_KERNEL_SCALER)/msOVD.c             \
        $(MONITOR_PATH_KERNEL_SCALER)/Ms_PM.c             \
        $(MONITOR_PATH_KERNEL_SCALER)/Gamma.c             \
        $(MONITOR_PATH_KERNEL_SCALER)/msDPS_Handler.c   \
        $(MONITOR_PATH_KERNEL_SCALER)/msDPS_Setting.c   \
        $(MONITOR_PATH_KERNEL_SCALER)/apiCEC.c              \
        $(MONITOR_PATH_KERNEL_SCALER)/MsDLC.c              \
        $(MONITOR_PATH_KERNEL_SCALER)/msLD_Handler.c     \
        $(MONITOR_PATH_KERNEL_SCALER)/ComboApp.c          \
        $(MONITOR_PATH_DRIVER)/9700/mhal_DPRx.c     \
        $(MONITOR_PATH_DRIVER)/9700/mhal_DPRx_phy.c     \
        $(MONITOR_PATH_DRIVER)/9700/mdrv_DPRx.c     \
        $(MONITOR_PATH_DRIVER)/9700/mapi_DPRx.c     \
        $(MONITOR_PATH_DRIVER)/9700/system_eDPTx.c   \
        $(MONITOR_PATH_DRIVER)/9700/mapi_eDPTx.c     \
        $(MONITOR_PATH_DRIVER)/9700/mdrv_eDPTx.c     \
        $(MONITOR_PATH_DRIVER)/9700/mhal_eDPTx.c     \
        $(MONITOR_PATH_DRIVER)/9700/mhal_hdmiRx.c     \
	$(MONITOR_PATH_DRIVER)/9700/mdrv_hdmiRx.c     \
        $(MONITOR_PATH_DRIVER)/9700/HDCP22/HdcpHandler.c     \
        $(MONITOR_PATH_DRIVER)/9700/HDCP22/HdcpLog.c     \
        $(MONITOR_PATH_DRIVER)/9700/HDCP22/HdcpMbx.c     \
        $(MONITOR_PATH_DRIVER)/9700/HDCP22/HdcpMsgPool.c     \
        $(MONITOR_PATH_DRIVER)/9700/HDCP22/HdcpRx.c     \
        $(MONITOR_PATH_DRIVER)/9700/HDCP22/HdcpTx.c     \
        $(MONITOR_PATH_DRIVER)/9700/HDCP22/drv_Hdcp_IMI.c                           \
        $(MONITOR_PATH_KERNEL_SCALER)/msACE.c     \
        $(MONITOR_PATH_KERNEL_SCALER)/dri_table_gen.c     \

#        $(MONITOR_PATH_KERNEL_SCALER)/UserDataSpace.c    \

#        $(MONITOR_PATH_KERNEL_SCALER)/ms2DTo3D.c             \
#        $(MONITOR_PATH_KERNEL_SCALER)/LedControl.c        \
#        $(MONITOR_PATH_KERNEL_SCALER)/msHDMI.c       \
#        $(MONITOR_PATH_KERNEL_SCALER)/MsHDCP.c       \
#        $(MONITOR_PATH_KERNEL_SCALER)/EQ_Adjustment.c          \


MONITOR_FILES_KERNEL_SYSTEM  += \
        $(MONITOR_PATH_KERNEL_SYSTEM)/isr.c                      \
        $(MONITOR_PATH_KERNEL_SYSTEM)/Common.c           \
        $(MONITOR_PATH_KERNEL_SYSTEM)/Global.c                 \
        $(MONITOR_PATH_KERNEL_SYSTEM)/main.c                   \
        $(MONITOR_PATH_KERNEL_SYSTEM)/mcu.c                    \
        $(MONITOR_PATH_KERNEL_SYSTEM)/misc.c                   \
        $(MONITOR_PATH_KERNEL_SYSTEM)/NVRam.c                \
        $(MONITOR_PATH_KERNEL_SYSTEM)/power.c                 \
        $(MONITOR_PATH_KERNEL_SYSTEM)/DDC2Bi.c                \
        $(MONITOR_PATH_KERNEL_SYSTEM)/i2c.c                      \
        $(MONITOR_PATH_KERNEL_SYSTEM)/DDCColorHandler.c    \
		$(MONITOR_PATH_KERNEL_SYSTEM)/DDCMCCSMSCHandler.c                  \
        $(MONITOR_PATH_KERNEL_SYSTEM)/mailbox.c                \
        $(MONITOR_PATH_KERNEL_SYSTEM)/usbsw.c                 \
        $(MONITOR_PATH_KERNEL_SYSTEM)/mode.c                  \
        $(MONITOR_PATH_KERNEL_SYSTEM)/hwi2c.c                  \
        $(MONITOR_PATH_KERNEL_SYSTEM)/mspi_flash.c            \
        $(MONITOR_PATH_KERNEL_SYSTEM)/Debug.c                 \
        $(MONITOR_PATH_KERNEL_SYSTEM)/msAPI_MSBHK.c                 \
        $(MONITOR_PATH_KERNEL_SYSTEM)/COLOR_VERIFY.c       \
        $(MONITOR_PATH_KERNEL_SYSTEM)/MsFlash.c         \
        $(MONITOR_PATH_KERNEL_SYSTEM)/PDUpdate.c         \
        $(MONITOR_PATH_KERNEL_SYSTEM)/PDUpdateReserved.c         \

#        $(MONITOR_PATH_KERNEL_SYSTEM)/USB/drvUSBDwld.c      \
#        $(MONITOR_PATH_KERNEL_SYSTEM)/USB/ms_dma.c       \
#        $(MONITOR_PATH_KERNEL_SYSTEM)/USB/ms_drc.c       \
#        $(MONITOR_PATH_KERNEL_SYSTEM)/USB/ms_function.c       \
#        $(MONITOR_PATH_KERNEL_SYSTEM)/USB/ms_msd_global.c       \
#        $(MONITOR_PATH_KERNEL_SYSTEM)/USB/ms_otg.c       \
#        $(MONITOR_PATH_KERNEL_SYSTEM)/USB/ms_usb.c       \
#        $(MONITOR_PATH_KERNEL_SYSTEM)/USB/msd_fn_scsi.c       \
#        $(MONITOR_PATH_KERNEL_SYSTEM)/USB/msd_fn_xfer.c       \


MONITOR_FILES_LIB_SRC  += \
#        $(MONITOR_PATH_LIB_SRC)/DualImage/Code/msDualImage.c       \
#        $(MONITOR_PATH_LIB_SRC)/Efuse/Code/msEread.c       \
#        $(MONITOR_PATH_LIB_SRC)/ComboRx/Code/msComboRx.c       \
#        $(MONITOR_PATH_LIB_SRC)/ACE/Code/msACE.c                      \
#        $(MONITOR_PATH_LIB_SRC)/AutoGamma_Lib/Source/AutoGamma.c       \


MONITOR_FILES_LIB_COMMON  += \
         $(MONITOR_PATH_LIB_SRC)/Efuse/Code/msEread.c       \
#        $(MONITOR_PATH_LIB_SRC)/DualImage/Code/msDualImage.c       \
#        $(MONITOR_PATH_LIB_SRC)/ComboRx/Code/msComboRx.c       \


MONITOR_FILES_LIB_FONT  += \
#        $(MONITOR_PATH_LIB_SRC)/CompFont.c                  \
#       $(MONITOR_PATH_LIB_SRC)/LoadPropFont1218.c          \

MONITOR_FILES_LIB_ACE  += \
#        $(MONITOR_PATH_LIB_SRC)/msACE.c                     \

MONITOR_FILES_LIB_DLC  += \
#        $(MONITOR_PATH_LIB_SRC)/MsDLC.c                     \

MONITOR_FILES_LIB_DPS  += \
#        $(MONITOR_PATH_LIB_SRC)/msDPS_Handler.c             \

MONITOR_FILES_LIB_DAISY_CHAIN  += \
#        $(MONITOR_PATH_LIB_SRC)/msDaisyChain.c              \

MONITOR_FILES_LIB_VBY1_TO_EDP  += \
#        $(MONITOR_PATH_LIB_SRC)/msVby1ToEDP.c               \

MONITOR_FILES_LIB_LD += \
#				$(MONITOR_PATH_LIB_SRC)/drvLdAlgorithm.c					\

MONITOR_LIB_COMMON = $(MONITOR_PATH_LIB)/Common.lib
MONITOR_LIB_FONT  = $(MONITOR_PATH_LIB)/LoadFont.lib
MONITOR_LIB_ACE   = $(MONITOR_PATH_LIB)/ACE.lib
MONITOR_LIB_HDR   = $(MONITOR_PATH_LIB)/MST9U4/HDR.lib
MONITOR_LIB_DLC   = $(MONITOR_PATH_LIB)/DLC.lib
MONITOR_LIB_DPS   = $(MONITOR_PATH_LIB)/DPS.lib
MONITOR_LIB_DAISY_CHAIN = $(MONITOR_PATH_LIB)/DaisyChain.lib
MONITOR_LIB_VBY1_TO_EDP = $(MONITOR_PATH_LIB)/Vby1ToEDP.lib
MONITOR_LIB_LD    = $(MONITOR_PATH_LIB)/LD.lib

MONITOR_FILES_UI  += \
        $(MONITOR_PATH_CUSTOM_UI)/menu.c                   \
        $(MONITOR_PATH_CUSTOM_UI)/msOSD.c                 \
        $(MONITOR_PATH_CUSTOM_UI)/MenuStrProp.c         \
        $(MONITOR_PATH_CUSTOM_UI)/MenuFunc.c             \
        $(MONITOR_PATH_CUSTOM_UI)/MenuStr.c               \
        $(MONITOR_PATH_CUSTOM_UI)/MenuStrFunc.c         \
        $(MONITOR_PATH_CUSTOM_UI)/ColorPalette.c           \
        $(MONITOR_PATH_CUSTOM_UI)/LoadCommonFont.c    \
        $(MONITOR_PATH_CUSTOM_UI)/Keypad.c                  \

MONITOR_FILES_CUSTOM_USERDATA += \
        $(MONITOR_PATH_CUSTOM_USERDATA)/UserPref.c                          \
		
MONITOR_FILE_CUSTOM_USERFUNC  += \
        $(MONITOR_PATH_CUSTOM_USERFUNC)/CustomCbAPI.c       \
		$(MONITOR_PATH_CUSTOM_USERFUNC)/CustomEDID.c       \
		$(MONITOR_PATH_CUSTOM_USERFUNC)/CustomFunc.c       \

MONITOR_FILES += \
        $(MONITOR_PATH_KERNEL_SCALER)/ms_rwreg.c          \

MONITOR_FILES1 += \
        $(MONITOR_FILES_DRIVER)                     \
        $(MONITOR_FILES_EXTDEVICE)                 \
        $(MONITOR_FILES_KERNEL_SCALER)          \
        $(MONITOR_FILES_KERNEL_SYSTEM)         \
        $(MONITOR_FILES_UI)                             \
        $(MONITOR_FILES_LIB_SRC)                     \
        $(MONITOR_FILES_CUSTOM_USERDATA)      \
        $(MONITOR_FILE_CUSTOM_USERFUNC)       \
#        $(MONITOR_FILES_LIB)                        \
#        $(MONITOR_FILES_CUSTOM_MSTAR_SI)            \
#        $(MONITOR_FILES_CUSTOM_MSTAR_APP)           \
#        $(MONITOR_FILES_CUSTOM_MSTAR_USERDATA)      \
#        $(MONITOR_FILES_CUSTOM_MSTAR_USERFUN)       \
#        $(MONITOR_FILES_CUSTOM_MSTAR_USERFUN_SI)    \

ifneq ($(BUILD_TARGET), BLOADER_SYSTEM)
MONITOR_FILES += \
        $(MONITOR_FILES1)
endif

ifeq ($(ZUI), 1)
MONITOR_FILES += \
        $(MONITOR_FILES_CUSTOM_MSTAR_ZUI)           \
        $(ZUI_FILES)

ifeq ($(ZUI_BM), 1)
MONITOR_FILES += \
        $(MONITOR_FILES_CUSTOM_MSTAR_ZUI_BM)
endif

endif

MONITOR_FILES_INC += \
        -I$(MONITOR_PATH_DRIVER)/                                   \
        -I$(MONITOR_PATH_DRIVER)/INC                              \
        -I$(MONITOR_PATH_DRIVER)/USB_Device/INC          \
        -I$(MONITOR_PATH_DRIVER)/USB_Host/source         \
        -I$(MONITOR_PATH_DRIVER)/USB_Host                   \
        -I$(MONITOR_PATH_DRIVER)/9700/INC                     \
        -I$(MONITOR_PATH_DRIVER)/9700/HDCP22/INC          \
        -I$(MONITOR_PATH_EXTDEVICE)/INC                         \
        -I$(MONITOR_PATH_KERNEL_SCALER)/INC                  \
        -I$(MONITOR_PATH_KERNEL_SYSTEM)/INC                 \
        -I$(MONITOR_PATH_LIB)/INC                                   \
        -I$(MONITOR_PATH_PANEL)/INC                               \
        -I$(MONITOR_PATH_CUSTOM_UI)/INC                                    \
        -I$(MONITOR_PATH_CUSTOM_USERDATA)/INC                                    \
        -I$(MONITOR_PATH_CUSTOM_USERFUNC)/INC                                    \
        -I$(MONITOR_PATH_CUSTOM)/BOARD                                \
        -I$(MONITOR_PATH_CUSTOM)/MODEL                                \
        -I$(MONITOR_PATH_LIB_SRC)/ACE/Code                  \
        -I$(MONITOR_PATH_LIB_SRC)/AutoGamma_Lib/Source/Include                  \
        -I$(MONITOR_PATH_LIB_SRC)/ComboRx/Code                  \
        -I$(MONITOR_PATH_LIB_SRC)/DualImage/Code                  \
        -I$(MONITOR_PATH_LIB_SRC)/Efuse/Code                  \
        -I$(MONITOR_PATH)/BIN                                   \

#        -I$(MONITOR_PATH_DRIVER)/usb_host/source                            \
#        -I$(MONITOR_PATH_DRIVER)/usb_host                            \


##################################################################
# TJPGDEC
##################################################################
# TJpgDec source files
TJPGDEC_PATH  		= $(ROOT)/tjpgdec
TJPGDEC_FILES 		+= 					\
		$(TJPGDEC_PATH)/tjpgd.c			\

# Source files
SRC_FILE += \
        $(MONITOR_FILES)      	\

#        $(TJPGDEC_FILES)      	\

# Add "Header (include) file" directories here ...
INC_DIR  += \
        $(MONITOR_FILES_INC)  	\

#		-I$(TJPGDEC_PATH)  	\

##################################################################
# Driver / API
##################################################################
#        $(ROOT)/core/driver/sys/MST9U/drvI2C.c                        \
#    for above slash
SYS_INIT_FILES += \
        $(ROOT)/core/driver/sys/MST9U/SysInit.c                       \

DRV_FILES += \
        $(ROOT)/core/driver/sys/MST9U/drvUartDebug.c                  \
        $(ROOT)/core/driver/sys/MST9U/drvISR.c                        \
        $(ROOT)/core/driver/sys/MST9U/drvPadConf.c                    \
        $(ROOT)/core/driver/sys/MST9U/drvInit.c                       \


#        $(ROOT)/core/middleware/mwutil/mw_debug.c                   \
#        $(ROOT)/core/util/util_minidump.c                           \
#        $(ROOT)/core/util/mstar_debug.c                             \
#        $(ROOT)/core/util/util_checkversion.c                       \
#    for above slash

UTIL_FILES += \
        $(ROOT)/core/util/util_symbol.c                             \
        $(ROOT)/core/util/uartdebug.c                               \


SRC_FILE += \
        $(SYS_INIT_FILES)                                           \
        $(DRV_FILES)                                                \
        $(UTIL_FILES)                                               \
        $(API_FILES)                                                \


#        -I$(ROOT)/core/middleware/mwutil/include                    \
#    for above slash
INC_DIR  += \
        -I$(ROOT)/project/image                                     \
        -I$(ROOT)/project/mmap                                      \
        -I$(ROOT)/core/driver/sys/MST9U/include                       \
        -I$(ROOT)/core/util/include                                 \
        -I$(ROOT)/core/api/include                                  \
        -I$(ROOT)/core/api/utl                                      \
        -I$(ROOT)/include/std                                       \

LINT_INC  += \
        $(ROOT)/project/image                                       \
        $(ROOT)/core/util/include                                   \
        $(ROOT)/core/api/include                                    \

##################################################################
# MONITOR-AP
##################################################################
MONAP = $(ROOT)/monitor-ap

#APP_FILES += \
#        $(MONAP)/main/app/MApp_Main.c                                 \


PROJS_FILES += \
        $(ROOT)/project/image/default/InfoBlock.c                      \
        $(ROOT)/project/image/imginfo.c                             \



# Source files
SRC_FILE += \
        $(PROJS_FILES)                                              \

#        $(APP_FILES)                                                \


# Add "Header (include) file" directories here ...
INC_DIR  += \
        -I$(ROOT)/project/boarddef                                  \
        -I$(ROOT)/project/swdef                                     \
        -I$(ROOT)/project/image/default                             \
        -I$(ROOT)/project/mmap                                      \


LINT_INC  += \
        $(ROOT)/project/boarddef                                    \
        $(ROOT)/project/swdef                                       \
        $(ROOT)/project/image/default                               \
        $(ROOT)/project/mmap                                        \


LIB_PRANA_PATH = $(ROOT)/core/lib/MST9U
LIB_PRANA      = libprana_MST9U.a

LDLIB += -Wl,--whole-archive $(LIB_PRANA_PATH)/$(LIB_PRANA) -Wl,--no-whole-archive

DRV_LIB_PATH   = $(ROOT)/core/driver
API_LIB_PATH   = $(ROOT)/core/api
DRV_BSP_PATH   = $(ROOT)/core/drv_bsp/MST9U_nos_r2/lib
MIDDLE_LIB_PATH   = $(ROOT)/core/middleware


ifeq ($(BUILD_APLIB), y)
MONITOR_FILES += \
        $(MONITOR_FILES_LIB_COMMON)           \

#        $(MONITOR_FILES_LIB_FONT)                   \
#        $(MONITOR_FILES_LIB_ACE)                    \
#        $(MONITOR_FILES_LIB_HDR)                    \
#        $(MONITOR_FILES_LIB_DLC)                    \
#        $(MONITOR_FILES_LIB_DPS)                    \
#        $(MONITOR_FILES_LIB_DAISY_CHAIN)        \
#        $(MONITOR_FILES_LIB_VBY1_TO_EDP)		\
#        $(MONITOR_FILES_LIB_LD)				\

else
MONITOR_AP_LIB += \
        $(MONITOR_LIB_COMMON)           \

#        $(MONITOR_LIB_FONT)                         \
#        $(MONITOR_LIB_ACE)                          \
#        $(MONITOR_LIB_HDR)                          \
#        $(MONITOR_LIB_DLC)                          \
#        $(MONITOR_LIB_DPS)                          \
#        $(MONITOR_LIB_DAISY_CHAIN)              \
#        $(MONITOR_LIB_VBY1_TO_EDP)            \
#        $(MONITOR_LIB_LD)					\

endif


#MONITOR_LIB += \
#    $(DRV_BSP_PATH)/libdrvWDT.a                \
#    $(DRV_BSP_PATH)/libapiXC.a                \
#    $(DRV_BSP_PATH)/libdrvMBX.a			\
#    $(DRV_BSP_PATH)/libdrvBDMA.a               \
#    $(DRV_BSP_PATH)/libdrvMIU.a                 \
#    $(DRV_BSP_PATH)/libdrvSERFLASH.a         \
#    $(DRV_BSP_PATH)/libdrvWDT.a                \
#    $(DRV_BSP_PATH)/libdrvSEM.a                 \
#    $(DRV_BSP_PATH)/libapiGFX.a                  \
#    $(DRV_BSP_PATH)/libapiGOP.a                  \
#    $(API_LIB_PATH)/mvf/libmvf_aeonR2.a       \


ifeq ($(VERIFY_GE), y)
	MONITOR_LIB += $(DRV_BSP_PATH)/libapiVerGE.a
endif

ifeq ($(BUILD_LIB),y)
LIB_SRC_ROOT = $(ROOT)/../LIB_SRC/BSP

LIB_PRANA_ROOT = $(LIB_SRC_ROOT)/platform/prana2

LIB_MXLIB = mxlib
LIB_MXLIB_ROOT = $(LIB_SRC_ROOT)/project/MST9U_nos_r2
endif

#Middleware libraries

BIN_INFO = $(MONITOR_PATH)/BIN/BinInfo.h

####  REBUILD_FILES=$(MONAP)/main/ui2_M10/MApp_ZUI_ACTmenufunc.c

include project/build/FILES_COMMON.mk

MONITOR_LIB += \
    $(COMMON_MONITOR_LIB)



