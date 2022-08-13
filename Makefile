TOP_DIR := .
sinclude $(TOP_DIR)/tools/W806/conf.mk

ifndef PDIR # {
GEN_IMAGES= $(TARGET).elf
GEN_BINS = $(TARGET).bin
SUBDIRS = \
    $(TOP_DIR)/app \
    $(TOP_DIR)/demo 
endif # } PDIR

ifndef PDIR # {
ifeq ($(USE_LIB), 0)
SUBDIRS += \
    $(TOP_DIR)/platform/arch        \
    $(TOP_DIR)/platform/component       \
    $(TOP_DIR)/platform/drivers    
endif
endif

COMPONENTS_$(TARGET) =    \
    $(TOP_DIR)/app/libappuser$(LIB_EXT)     \
    $(TOP_DIR)/demo/libdemo$(LIB_EXT)    

ifeq ($(USE_LIB), 0)
COMPONENTS_$(TARGET) += \
    $(TOP_DIR)/platform/boot/libwmarch$(LIB_EXT)        \
    $(TOP_DIR)/platform/component/libwmcomponent$(LIB_EXT)        \
    $(TOP_DIR)/platform/drivers/libdrivers$(LIB_EXT)  
endif

LINKLIB =     \
    $(TOP_DIR)/lib/$(CONFIG_ARCH_TYPE)/libdsp$(LIB_EXT)
ifeq ($(USE_LIB), 1)
LINKLIB +=    \
    $(TOP_DIR)/lib/$(CONFIG_ARCH_TYPE)/libwmarch$(LIB_EXT)    \
    $(TOP_DIR)/lib/$(CONFIG_ARCH_TYPE)/libwmcomponent$(LIB_EXT)    \
    $(TOP_DIR)/lib/$(CONFIG_ARCH_TYPE)/libdrivers$(LIB_EXT)
endif

LINKFLAGS_$(TARGET) =  \
    $(LINKLIB)

CONFIGURATION_DEFINES =    

DEFINES +=                \
    $(CONFIGURATION_DEFINES)

DDEFINES +=                \
    $(CONFIGURATION_DEFINES)

INCLUDES := $(INCLUDES) -I$(PDIR)include
INCLUDES += -I ./

sinclude $(TOP_DIR)/tools/$(CONFIG_ARCH_TYPE)/rules.mk

.PHONY: FORCE
FORCE:
