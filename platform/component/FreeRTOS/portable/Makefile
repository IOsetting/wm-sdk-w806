TOP_DIR = ../../../..
sinclude $(TOP_DIR)/tools/W806/conf.mk

ifndef PDIR
GEN_LIBS = libwmportable$(LIB_EXT)
COMPONENTS_libwmportable = MemMang/libptmemmang$(LIB_EXT) \
					             xt804/libptxt804$(LIB_EXT)
endif

#DEFINES +=

sinclude $(TOP_DIR)/tools/W806/rules.mk

PDIR := ../$(PDIR)
sinclude $(PDIR)Makefile
