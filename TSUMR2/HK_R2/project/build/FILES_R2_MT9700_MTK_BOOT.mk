
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

#MONITOR_FILES += \
#        $(MONITOR_PATH_KERNEL_SCALER)/ms_rwreg.c          \
        
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

# Source files
SRC_FILE += \
        $(MONITOR_FILES)      	\

# Add "Header (include) file" directories here ...
INC_DIR  += \
        $(MONITOR_FILES_INC)  	\

##################################################################
# Driver / API
##################################################################
#        $(ROOT)/core/driver/sys/MST9U/drvI2C.c                        \
#    for above slash
SYS_INIT_FILES += \
        $(ROOT)/core/driver/sys/MST9U/SysInit_boot.c                       \

DRV_FILES += \
        $(ROOT)/core/driver/sys/MST9U/drvUartDebug.c                  \
        $(ROOT)/core/driver/sys/MST9U/drvISR.c                        \
        $(ROOT)/core/driver/sys/MST9U/drvPadConf.c                    \
        $(ROOT)/core/driver/sys/MST9U/drvInit.c                       \

RSA_FILES += \
        $(ROOT)/core/driver/sys/MST9U/mbedtls_2_3_0/library/Platform.c                \
        $(ROOT)/core/driver/sys/MST9U/mbedtls_2_3_0/library/aes.c                \
        $(ROOT)/core/driver/sys/MST9U/mbedtls_2_3_0/library/asn1parse.c                \
        $(ROOT)/core/driver/sys/MST9U/mbedtls_2_3_0/library/asn1write.c                \
        $(ROOT)/core/driver/sys/MST9U/mbedtls_2_3_0/library/bignum.c                \
        $(ROOT)/core/driver/sys/MST9U/mbedtls_2_3_0/library/ctr_drbg.c                \
        $(ROOT)/core/driver/sys/MST9U/mbedtls_2_3_0/library/entropy.c                \
        $(ROOT)/core/driver/sys/MST9U/mbedtls_2_3_0/library/entropy_poll.c                \
        $(ROOT)/core/driver/sys/MST9U/mbedtls_2_3_0/library/md.c                \
        $(ROOT)/core/driver/sys/MST9U/mbedtls_2_3_0/library/md_wrap.c                \
        $(ROOT)/core/driver/sys/MST9U/mbedtls_2_3_0/library/md5.c                \
        $(ROOT)/core/driver/sys/MST9U/mbedtls_2_3_0/library/oid.c                \
        $(ROOT)/core/driver/sys/MST9U/mbedtls_2_3_0/library/padlock.c                \
        $(ROOT)/core/driver/sys/MST9U/mbedtls_2_3_0/library/ripemd160.c                \
        $(ROOT)/core/driver/sys/MST9U/mbedtls_2_3_0/library/rsa.c                \
        $(ROOT)/core/driver/sys/MST9U/mbedtls_2_3_0/library/sha1.c                \
        $(ROOT)/core/driver/sys/MST9U/mbedtls_2_3_0/library/sha256.c                \
        $(ROOT)/core/driver/sys/MST9U/mbedtls_2_3_0/library/sha512.c                \
        $(ROOT)/core/driver/sys/MST9U/mbedtls_2_3_0/library/timing.c                \
        $(ROOT)/core/driver/sys/MST9U/mbedtls_2_3_0/programs/aes/aescrypt2.c                \
        $(ROOT)/core/driver/sys/MST9U/mbedtls_2_3_0/programs/pkey/rsa_verify.c                \

UTIL_FILES += \
        $(ROOT)/core/util/util_symbol.c                             \
        $(ROOT)/core/util/uartdebug.c                               \


SRC_FILE += \
        $(SYS_INIT_FILES)                                           \
        $(DRV_FILES)                                                \
        $(RSA_FILES)                                                \
        $(UTIL_FILES)                                               \
        $(API_FILES)                                                \


#        -I$(ROOT)/core/middleware/mwutil/include                    \
#    for above slash
INC_DIR  += \
        -I$(ROOT)/project/image                                     \
        -I$(ROOT)/project/mmap                                      \
        -I$(ROOT)/core/driver/sys/MST9U/include             \
        -I$(ROOT)/core/util/include                                 \
        -I$(ROOT)/core/api/include                                  \
        -I$(ROOT)/core/api/utl                                      \
        -I$(ROOT)/include/std                                       \
        -I$(ROOT)/core/driver/sys/MST9U/mbedtls_2_3_0/include       \

LINT_INC  += \
        $(ROOT)/project/image                                       \
        $(ROOT)/core/util/include                                   \
        $(ROOT)/core/api/include                                    \

##################################################################
# MONITOR-AP
##################################################################
MONAP = $(ROOT)/monitor-ap

PROJS_FILES += \
#        $(ROOT)/project/image/default/InfoBlock.c                      \
#        $(ROOT)/project/image/imginfo.c                             \


# Source files
SRC_FILE += \
        $(PROJS_FILES)                                              \


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


ifeq ($(BUILD_LIB),y)
LIB_SRC_ROOT = $(ROOT)/../LIB_SRC/BSP

LIB_PRANA_ROOT = $(LIB_SRC_ROOT)/platform/prana2

LIB_MXLIB = mxlib
LIB_MXLIB_ROOT = $(LIB_SRC_ROOT)/project/MST9U_nos_r2
endif

#Middleware libraries

BIN_INFO = $(MONITOR_PATH)/BIN/BinInfo.h

include project/build/FILES_COMMON.mk

MONITOR_LIB += \
    $(COMMON_MONITOR_LIB)



