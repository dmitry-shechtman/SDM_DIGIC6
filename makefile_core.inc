# Build rules for platform independant code (except modules) - code built in THUMB mode

include $(topdir)makefile_base.inc
include $(topdir)arm_rules.inc

ifndef TARGET_CAM
TARGET_CAM=$(PLATFORM)
endif
ifndef TARGET_FW
TARGET_FW=$(PLATFORMSUB)
endif
ifndef TARGET_PID
TARGET_PID=$(PLATFORMID)
endif
ifeq ($(TARGET_PID),)
override TARGET_PID=$(PLATFORMID)
endif

CFLAGS+= -DPLATFORM=\"$(TARGET_CAM)\" -DPLATFORMSUB=\"$(TARGET_FW)\" -DPLATFORMID=$(TARGET_PID) -DCAMERA_$(PLATFORM)=1 -mthumb -mthumb-interwork -DCHDK_MODULE_CODE

include $(topdir)build_rules.inc

# Define empty recipes for source files (including the makefiles)
# to prevent make from trying implicit rules to create them. Speeds up build process
$(topdir)makefile_core.inc: ;
