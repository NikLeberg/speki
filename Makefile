# http://stackoverflow.com/questions/7004702/how-can-i-create-a-makefile-for-c-projects-with-src-obj-and-bin-subdirectories

# output name
TARGET ?= test

TOOLCHAIN_PATH ?= ''

# toolchain
CC = $(TOOLCHAIN_PATH)arm-none-eabi-gcc
OC = $(TOOLCHAIN_PATH)arm-none-eabi-objcopy
OD = $(TOOLCHAIN_PATH)arm-none-eabi-objdump
SZ = $(TOOLCHAIN_PATH)arm-none-eabi-size

# output structure
OBJDIR ?= obj
DEPDIR ?= dep
BINDIR ?= bin

# processor type
PTYPE = STM32F40_41xxx

# don't change anything under this line if you don't know what you're doing!
# ==========================================================================

# preparation for cross platform compatibility
RM := rm -rf
MKDIR := mkdir -p

# populate variables for includes
CFLAGS_INC :=
CFLAGS_DEF :=
CFLAGS_PROC :=
CFLAGS_BASE :=
LDFLAGS :=
SRCS :=
SRCS_ASM :=

# where are the libraries located?
LIBDIR := lib
BSPDIR ?= BSP
SGUIDIR ?= sGUI
PERIPHDIR ?= STM32F4xx_StdPeriph_Driver
CMSISDIR ?= CMSIS

# include libraries
include $(LIBDIR)/$(BSPDIR)/build.mk
include $(LIBDIR)/$(SGUIDIR)/build.mk
include $(LIBDIR)/$(PERIPHDIR)/build.mk
include $(LIBDIR)/$(CMSISDIR)/build.mk

# compiler flags
CFLAGS_BASE += -O2 -Wall -std=gnu11 -fdata-sections -ffunction-sections
CFLAGS_PROC += -mthumb -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16
CFLAGS_DEF += -D$(PTYPE) -DUSE_STDPERIPH_DRIVER
CFLAGS_DEF += -DHSE_VALUE=25000000 -DSYSCALL_USART=USART1
CFLAGS_INC += -Iinc
CFLAGS += $(CFLAGS_BASE) $(CFLAGS_PROC) $(CFLAGS_DEF) $(CFLAGS_INC)

# linker flags
LDFLAGS += -Tstm32f4_flash.ld
LDFLAGS += -Wl,-Map,"$(BINDIR)/$(TARGET).map"
LDFLAGS += -Wl,--start-group -lc -lm -Wl,--end-group
LDFLAGS += --specs=nosys.specs
LDFLAGS += -Wl,-cref -Wl,-static
LDFLAGS += -Wl,--gc-sections -Wl,--defsym=malloc_getpagesize_P=0x80

SRCS += $(wildcard src/*.c)
SRCS_ASM += $(wildcard src/*.s)

OBJS := $(SRCS:%.c=$(OBJDIR)/%.o) $(SRCS_ASM:%.s=$(OBJDIR)/%.o)

# TARGETS
# =======

# these are not real targets
.PHONY: all clean dirs size

# default target
all: $(BINDIR)/$(TARGET).elf | dirs

debug: CFLAGS := -ggdb -O0 $(filter-out -O -O0 -O1 -O2 -O3 -Os -Ofast,$(CFLAGS))
debug: $(BINDIR)/$(TARGET).elf | dirs

# link and create elf
$(BINDIR)/$(TARGET).elf: $(OBJS) | dirs
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)
	@echo "[LD] $@"

%.hex: %.elf
	@$(OC) -O ihex $< $@
	@echo "[OC] $@"

%.bin: %.elf
	@$(OC) -O binary $< $@
	@echo "[OC] $@"

%.lst: %.elf
	@$(OD) -x -S $< > $@
	@echo "[OD] $@"

# Include auto generated dependency files
-include $(wildcard $(DEPDIR)/*.d)

# compile and generate dependency files
$(OBJDIR)/%.o: %.s | dirs
	@$(CC) $(CFLAGS) -c -o $@ $< -MMD -MF $(DEPDIR)/$(*F).d
	@echo "[CC] $<"

# compile and generate dependency files
$(OBJDIR)/%.o: %.c | dirs
	@$(CC) $(CFLAGS) -c -o $@ $< -MMD -MF $(DEPDIR)/$(*F).d
	@echo "[CC] $<"

# output size information
size: $(BINDIR)/$(TARGET).elf
	@$(SZ) $(BINDIR)/$(TARGET).elf

run: $(BINDIR)/$(TARGET).elf
	openocd -f openocd.cfg

# create directories
dirs:
	@$(MKDIR) $(dir $(OBJS))
	@$(MKDIR) $(DEPDIR)
	@$(MKDIR) $(BINDIR)

# remove output files
clean:
	@echo "[RM] $(OBJDIR)"
	@$(RM) $(OBJDIR)
	@echo "[RM] $(DEPDIR)"
	@$(RM) $(DEPDIR)
	@echo "[RM] $(BINDIR)"
	@$(RM) $(BINDIR)

