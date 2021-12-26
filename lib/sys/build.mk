SYSDIR ?= sys

CFLAGS_INC += -I$(LIBDIR)/$(SYSDIR)/inc
#CFLAGS_DEF +=

# search files in this directory
SRCS += $(wildcard $(LIBDIR)/$(SYSDIR)/src/*.c)
SRCS_ASM += $(wildcard $(LIBDIR)/$(SYSDIR)/src/*.s)
