SGUIDIR ?= sGUI

CFLAGS_INC += -I$(LIBDIR)/$(SGUIDIR)/inc
#CFLAGS_DEF +=

# search files in these directories
SRCS += $(wildcard $(LIBDIR)/$(SGUIDIR)/src/*.c)
