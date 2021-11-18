TOP_DIR = ../../..
sinclude $(TOP_DIR)/tools/W806/conf.mk

ifndef PDIR
GEN_LIBS = libwmrtos$(LIB_EXT)
COMPONENTS_libwmrtos = portable/libwmportable$(LIB_EXT)
endif

#DEFINES +=

sinclude $(TOP_DIR)/tools/W806/rules.mk
INCLUDES := $(INCLUDES) -I $(PDIR)include
PDIR := ../$(PDIR)
sinclude $(PDIR)Makefile
