TOP_DIR = ../..
sinclude $(TOP_DIR)/tools/W806/conf.mk

ifndef PDIR
GEN_LIBS = libwmarch$(LIB_EXT)
COMPONENTS_libwmarch = xt804/libwmxt804$(LIB_EXT)
endif

#DEFINES +=

sinclude $(TOP_DIR)/tools/W806/rules.mk
INCLUDES := $(INCLUDES) -I $(PDIR)include
PDIR := ../$(PDIR)
sinclude $(PDIR)Makefile