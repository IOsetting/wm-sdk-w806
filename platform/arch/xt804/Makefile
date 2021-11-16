TOP_DIR = ../../..
sinclude $(TOP_DIR)/tools/W806/conf.mk

ifndef PDIR
GEN_LIBS = libwmxt804$(LIB_EXT)
COMPONENTS_libwmxt804 = bsp/libwmbsp$(LIB_EXT) \
					    libc/libwmlibc$(LIB_EXT)
endif

#DEFINES +=

sinclude $(TOP_DIR)/tools/W806/rules.mk
INCLUDES := $(INCLUDES) -I $(PDIR)include
PDIR := ../$(PDIR)
sinclude $(PDIR)Makefile