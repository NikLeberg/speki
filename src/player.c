/**
 * @file player.c
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Module for audio playback with cs42l51 codec.
 * @version 0.1
 * @date 2021-12-05
 * 
 * @copyright Copyright (c) 2021 Niklaus Leuenberger
 * 
 */

#include <i2c.h>
#include <cs42l51.h>
#include <assert.h>

#include "player.h"

#define TIMEOUT (1000U)

static enum {
    PLAYER_NOT_INITIALIZED = 0,
    PLAYER_STOPPED,
    PLAYER_PLAYING
} g_state;

static player_chunk_callback g_callback;
static struct {
    int16_t *data;
    size_t length;
} g_chunks[2];

static int request_chunk(int index);
static void set_chunk(int index);
static int chunk_is_finished(int index);
static int enable_or_disable_dma(FunctionalState newState);

int player_init(player_chunk_callback callback) {
    // check current state
    if (g_state != PLAYER_NOT_INITIALIZED) {
        // only allow initialization once
        return -1;
    }

    // check and save given callback
    if (!callback) {
        return -1;
    }
    g_callback = callback;

    // init codec (also set volume)
    CS42L51_Init(200);

    // disable i2s peripheral
    // (was enabled by CS42L51_Init, needed to be able to configure DMA)
    I2S_Cmd(CODEC_I2S, DISABLE);

    // enable I2S PLL clock
    // (needs to be done according to stm32f4xx_spi.c)
    RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S);
    RCC_PLLI2SCmd(ENABLE);
    for (int i = TIMEOUT; i > 0; --i) {
        // wait for successful activation
        if (RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY) == SET) {
            break;
        } else if (i == 0) {
            return -1; // timeout
        }
    }

    // stop DMA
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    DMA_Cmd(DMA1_Stream4, DISABLE);
    for (int i = TIMEOUT; i >= 0; --i) {
        // wait for end of operation
        if (DMA_GetCmdStatus(DMA1_Stream4) == DISABLE) {
            break;
        } else if (i == 0) {
            return -1; // timeout
        }
    }
    DMA_DeInit(DMA1_Stream4);

    // initialize DMA in circular double buffer mode
    DMA_InitTypeDef DMA_config;
    DMA_StructInit(&DMA_config);
    DMA_config.DMA_PeripheralBaseAddr = (uint32_t)&CODEC_I2S->DR;
    DMA_config.DMA_Memory0BaseAddr = (uint32_t)g_chunks[0].data;
    DMA_config.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_config.DMA_BufferSize = PLAYER_CHUNK_SIZE;
    DMA_config.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_config.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_config.DMA_Mode = DMA_Mode_Circular;
    DMA_DoubleBufferModeConfig(DMA1_Stream4, (uint32_t)g_chunks[1].data, DMA_Memory_0);
    DMA_DoubleBufferModeCmd(DMA2_Stream0, ENABLE);
    DMA_Init(DMA1_Stream4, &DMA_config);

    // setup transfer complete interrupt
    DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);
    DMA_ITConfig(DMA1_Stream4, DMA_IT_TC, ENABLE);
    NVIC_InitTypeDef NVIC_config;
    NVIC_config.NVIC_IRQChannel = DMA1_Stream4_IRQn;
    NVIC_config.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_config.NVIC_IRQChannelSubPriority = 1;
    NVIC_config.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_config);

    g_state = PLAYER_STOPPED;
    return 0;
}

int player_deinit() {
    // ToDo
    return 0;
}

int player_loop() {
    switch (g_state) {
    case (PLAYER_NOT_INITIALIZED):
        // can't run, not initialized
        return -1;
        break;
    case (PLAYER_STOPPED):
        // noting to do
        break;
    case (PLAYER_PLAYING):
        // Check if any of the chunks need to be reloaded and do so.
        if (chunk_is_finished(0)) {
            if (request_chunk(0)) {
                return -1;
            }
            set_chunk(0);
        } else if (chunk_is_finished(1)) {
            if (request_chunk(1)) {
                return -1;
            }
            set_chunk(1);
        }
        break;
    }

    return 0;
}

int player_play() {
    if (g_state == PLAYER_NOT_INITIALIZED) {
        return -1;
    }
    // request two chunks to preload bitstream
    if (request_chunk(0) || request_chunk(1)) {
        return -1;
    }
    set_chunk(0);
    set_chunk(1);
    // enable DMA
    if (enable_or_disable_dma(ENABLE)) {
        return -1;
    }
    g_state = PLAYER_PLAYING;
    return 0;
}

int player_pause() {
    if (g_state == PLAYER_NOT_INITIALIZED) {
        return -1;
    }
    // disable DMA
    if (enable_or_disable_dma(DISABLE)) {
        return -1;
    }
    g_state = PLAYER_STOPPED;
    return 0;
}

void DMA1_Stream4_IRQHandler(void) {
    // clear flag
    DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);
    // Flag the main loop to request a new chunk: The DMA should be transfering
    // the next chunk already. So check what memory pointer it currently works
    // with and clear the other memory pointer. The main loop will then reload
    // the chunk.
    int i = !DMA_GetCurrentMemoryTarget(DMA1_Stream4);
    g_chunks[i].data = NULL;
    g_chunks[i].length = 0;
}

static int request_chunk(int index) {
    assert(g_callback);
    if (g_callback(&g_chunks[index].data, &g_chunks[index].length)) {
        return -1;
    }
    return 0;
}

static void set_chunk(int index) {
    assert(g_chunks[index].data);
    assert(g_chunks[index].length > 0);
    assert(g_chunks[index].length <= PLAYER_CHUNK_SIZE);
    DMA_MemoryTargetConfig(DMA1_Stream4, (uint32_t)g_chunks[index].data,
                           index == 0 ? DMA_Memory_0 : DMA_Memory_1);
}

static int chunk_is_finished(int index) {
    return (g_chunks[index].data == NULL);
}

static int enable_or_disable_dma(FunctionalState newState) {
    // change DMA state
    DMA_Cmd(DMA1_Stream4, newState);
    SPI_I2S_DMACmd(CODEC_I2S, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, newState);
    for (int i = TIMEOUT; i >= 0; --i) {
        // wait for end of operation
        if (DMA_GetCmdStatus(DMA1_Stream4) == newState) {
            break;
        } else if (i == 0) {
            return -1; // timeout
        }
    }
    // change i2s state
    I2S_Cmd(CODEC_I2S, newState);
    return 0;
}
