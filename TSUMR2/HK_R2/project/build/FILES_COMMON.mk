

ifeq ($(CHIP_FAMILY),M12)
    PQDIR ?= m12
endif


##################################################################
# debug msg
##################################################################
CC_MTOPTS += -DENABLE_DBG=0

##################################################################
# Driver / API
##################################################################


##############################################################
# MONITOR-AP
##############################################################
ifeq ($(ZUI), 1)
API_FILES1 += \
          $(ROOT)/core/api/msAPI_BDMA.c                    \
          $(ROOT)/core/api/msAPI_Flash.c                   \
          $(ROOT)/core/api/msAPI_Memory.c                  \
          $(ROOT)/core/api/msAPI_Font.c                    \
          $(ROOT)/core/api/msAPI_OCP.c                     \
          $(ROOT)/core/api/msAPI_OSD.c                     \
          $(ROOT)/core/api/msAPI_OSD2.c                    \
          $(ROOT)/core/api/msAPI_OSD_Resource.c
else
API_FILES1 += \
          $(ROOT)/core/api/msAPI_BDMA.c                    \
          $(ROOT)/core/api/msAPI_Flash.c                   \
          $(ROOT)/core/api/msAPI_Memory.c                  \
      	  $(ROOT)/core/api/msAPI_OSD.c					 

ifneq ($(BUILD_TARGET),BLOADER_SYSTEM)
API_FILES += \
          $(ROOT)/core/api/msAPI_Memory.c
endif          
endif

API_FILES1 += \
		  $(ROOT)/core/api/msAPI_MailBox.c                 \
		  
FS_FILES1 += \
        $(ROOT)/device/flash/msFlash.c                              \
        $(ROOT)/core/api/IOUtil.c                                   \
        $(ROOT)/core/api/msAPI_Timer.c                              \
        $(ROOT)/core/api/n51fs/msAPI_FCtrl.c                        \
        $(ROOT)/core/api/n51fs/msAPI_FAT.c                          \
        $(ROOT)/core/api/n51fs/FAT_Cache.c                          \
        $(ROOT)/core/api/n51fs/FSUtil.c                             \
        $(ROOT)/core/api/n51fs/msAPI_MSDCtrl.c                      \
        $(ROOT)/core/api/n51fs/msAPI_FSEnv.c                        \
        $(ROOT)/core/api/n51fs/msAPI_N51FS.c                        \
        $(ROOT)/core/api/n51fs/N51FS_File.c                         \
        $(ROOT)/core/api/n51fs/N51FS_Index.c                        \
        $(ROOT)/core/api/n51fs/N51FS_Bitmap.c                       \
        $(ROOT)/core/api/msAPI_FS_SysInfo.c                         \
        $(ROOT)/core/app/mw_usbdownload.c                           \
        $(ROOT)/core/app/MApp_SwUpdate.c                            \
        $(ROOT)/core/app/MApp_USBDownload.c                         \

ifneq ($(BUILD_TARGET),BLOADER_SYSTEM)
FS_FILES += \
        $(ROOT)/core/api/IOUtil.c                                   \
        $(ROOT)/core/api/msAPI_Timer.c                              \
        $(ROOT)/core/api/msAPI_FS_SysInfo.c                         \
        $(ROOT)/core/api/n51fs/msAPI_FCtrl.c                        \
        $(ROOT)/core/api/n51fs/msAPI_FAT.c                          \
        $(ROOT)/core/api/n51fs/FAT_Cache.c                          \
        $(ROOT)/core/api/n51fs/FSUtil.c                             \
        $(ROOT)/core/api/n51fs/msAPI_MSDCtrl.c                      \
        $(ROOT)/core/api/n51fs/msAPI_FSEnv.c                        \
        $(ROOT)/core/api/n51fs/msAPI_N51FS.c                        \
        $(ROOT)/core/api/n51fs/N51FS_File.c                         \
        $(ROOT)/core/api/n51fs/N51FS_Index.c                        \
        $(ROOT)/core/api/n51fs/N51FS_Bitmap.c                       \
        $(ROOT)/core/app/mw_usbdownload.c                           \
        $(ROOT)/core/app/MApp_SwUpdate.c                            \
        $(ROOT)/core/app/MApp_USBDownload.c
 endif
 
INC_DIR  += \
         -I$(ROOT)/device/flash                                     \
         -I$(ROOT)/core/api/n51fs/include                           \
         -I$(ROOT)/core/app/include                                 \

API_FILES += \
        $(FS_FILES)                                           \

UTIL_FILES += \
		$(ROOT)/core/api/utl/Utl.c                         \

ZUI_FILES += \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_ACTeffect.c                  \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_APIalphatables.c             \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_APIChineseIME.c              \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_APIcomponent.c               \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_APIcontrols.c                \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_APIdraw.c                    \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_APIgdi.c                     \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_APIpostables.c               \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_APIstrings.c                 \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_APIstyletables.c             \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_APItables.c                  \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_APIwindow.c                  \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_APIEasing.c              		 \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLanimation.c               \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLautoclose.c               \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLballprogbar.c             \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLballprogbarcolor.c        \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLbgtransparent.c           \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLbuttonclick.c             \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLdynabmp.c                 \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLdynacolortext.c           \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLdynaepgpunctext.c         \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLdynalist2.c               \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLdynalist.c                \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLdynapunctext.c            \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLdynatext.c                \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLdynatexteffect.c          \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLeffect.c                  \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLeffectcube.c              \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLeffectfall.c              \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLeffectflip.c              \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLeffectfold.c              \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLeffectpopup.c             \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLeffectroll.c              \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLeffectslideitem.c         \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLeffectzoom.c              \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLfake3d.c                  \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLgrid.c                    \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLkeyboard.c                \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLmainframe.c               \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLmarquee.c                 \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLmotiontrans.c             \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLpercentprogbar.c          \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLradiobutton.c             \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLrectprogbar.c             \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLrotatestatic.c            \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLscroller.c                \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLslider.c                  \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLspin.c                    \
          $(MONITOR_PATH)/COMMON/zui/MApp_ZUI_CTLtextviewer.c              \

INC_DIR  += \
         -I$(MONAP)/common/app/include                              \
         -I$(MONITOR_PATH)/COMMON/zui/include                              \

LINT_INC  += \
          $(MONAP)/common/app/                                      \
          $(MONAP)/common/app/include                               \

COMMON_MONITOR_LIB += \
    $(DRV_BSP_PATH)/libdrvIRQ.a                           \
    $(DRV_BSP_PATH)/libnos.a                                \
    $(DRV_BSP_PATH)/libdrvCPU.a                           \
    $(DRV_BSP_PATH)/libdrvUART.a                         \
    $(DRV_BSP_PATH)/libdrvMMIO.a                         \


