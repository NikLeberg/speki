CFLAGS_INC += -I$(LIBDIR)/$(PERIPHDIR)/inc
#CFLAGS_DEF += -DUSE_STDPERIPH_DRIVER -DUSE_FULL_ASSERT

# search files in these directories
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_adc.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_crc.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_dbgmcu.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_dma.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_exti.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_flash.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_gpio.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_i2c.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_iwdg.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_pwr.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_rcc.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_rtc.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_sdio.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_spi.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_syscfg.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_tim.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_usart.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_wwdg.c

ifeq ($(PTYPE),STM32F40_41xxx)
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_cryp.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_hash.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_rng.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_can.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_dac.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_dcmi.c
SRCS += $(LIBDIR)/$(PERIPHDIR)/src/stm32f4xx_fsmc.c
endif
