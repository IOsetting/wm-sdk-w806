TOP_DIR = ../..
sinclude $(TOP_DIR)/tools/W806/conf.mk

ifndef PDIR
GEN_LIBS = libwmcomponent$(LIB_EXT)
COMPONENTS_libwmcomponent = FreeRTOS/libwmrtos$(LIB_EXT) \
                            auto_dl/libautodl$(LIB_EXT) \
                            FreeRTOS/libfatfs$(LIB_EXT) \
                            SpiSD/libspisd$(LIB_EXT)
endif

#DEFINES +=

sinclude $(TOP_DIR)/tools/W806/rules.mk

PDIR := ../$(PDIR)
sinclude $(PDIR)Makefile
