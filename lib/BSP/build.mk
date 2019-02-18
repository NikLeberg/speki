BSPDIR ?= BSP

CFLAGS_INC += -I$(LIBDIR)/$(BSPDIR)/inc
#CFLAGS_DEF +=

# search files in these directories
SRCS += $(wildcard $(LIBDIR)/$(BSPDIR)/src/*.c)
