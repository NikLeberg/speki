/**
 * @file player.c
 * @author Leuenberger Niklaus <leuen4@bfh.ch>
 * @brief Module for audio playback with cs42l51 codec.
 * @version 0.3
 * @date 2021-12-11
 * 
 * @copyright Copyright (c) 2021 Niklaus Leuenberger
 * 
 */

#include <i2c.h>
#include <cs42l51.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "player.h"

#define TIMEOUT (1000U)

/**
 * @brief Player states.
 * 
 * Operating sequence is:
 * --[reset]-> NOT_INITIALIZED
 *   --init()--> STOPPED
 *     --play()--> PLAYING
 *       --stop()--> STOPPING
 *         --[ISR]---> STOPPED
 */
static __IO enum {
    PLAYER_NOT_INITIALIZED = 0,
    PLAYER_STOPPED,
    PLAYER_PLAYING,
    PLAYER_STOPPING
} g_state;

enum {
    LOWER_HALF = 0,
    UPPER_HALF,
    MAX_HALF
};

static player_load_data_callback g_callback;
static int16_t g_buffer[MAX_HALF * PLAYER_BUFFER_SIZE];
static struct {
    __IO int valid[MAX_HALF]; //!< 1 if lower / upper half of buffer has valid data
} g_flags;

static size_t load_data(int half);

int player_init(player_load_data_callback callback) {
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
    CS42L51_Init(127);

    // disable i2s peripheral
    // (was enabled by CS42L51_Init, needed to be able to configure DMA)
    I2S_Cmd(CODEC_I2S, DISABLE);

    // enable I2S PLL clock
    // (needs to be done according to stm32f4xx_spi.c)
    RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S);
    RCC_PLLI2SCmd(ENABLE);
    for (int i = TIMEOUT; i >= 0; --i) {
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

    // initialize DMA in circular mode
    DMA_InitTypeDef DMA_config;
    DMA_StructInit(&DMA_config);
    DMA_config.DMA_PeripheralBaseAddr = (uint32_t)&CODEC_I2S->DR;
    DMA_config.DMA_Memory0BaseAddr = (uint32_t)g_buffer;
    DMA_config.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_config.DMA_BufferSize = MAX_HALF * PLAYER_BUFFER_SIZE;
    DMA_config.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_config.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_config.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_config.DMA_Mode = DMA_Mode_Circular;
    DMA_Init(DMA1_Stream4, &DMA_config);

    // setup interrupts:
    // - transfer half complete
    // - transfer complete
    DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_HTIF4 | DMA_IT_TCIF4);
    DMA_ITConfig(DMA1_Stream4, DMA_IT_HT | DMA_IT_TC, ENABLE);
    NVIC_InitTypeDef NVIC_config;
    NVIC_config.NVIC_IRQChannel = DMA1_Stream4_IRQn;
    NVIC_config.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_config.NVIC_IRQChannelSubPriority = 1;
    NVIC_config.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_config);

    // enable DMA
    DMA_Cmd(DMA1_Stream4, ENABLE);
    SPI_I2S_DMACmd(CODEC_I2S, SPI_I2S_DMAReq_Tx, ENABLE);
    for (int i = TIMEOUT; i >= 0; --i) {
        // wait for end of operation
        if (DMA_GetCmdStatus(DMA1_Stream4) == ENABLE) {
            break;
        } else if (i == 0) {
            return -1; // timeout
        }
    }

    // enable i2s peripheral
    I2S_Cmd(CODEC_I2S, ENABLE);

    // From here on the DMA stream is always running and the i2s peripheral e.g.
    // the codec is always fed with data. To "stop" the output of audio the
    // buffer is filled with zeros so that only silence is sent out.
    g_state = PLAYER_STOPPED;
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
        // Check if the lower (0) or upper (1) half of the buffer was already
        // transfered. If so, request new data from registered callback.
        for (int i = LOWER_HALF; i < MAX_HALF; ++i) {
            if (!g_flags.valid[i]) {
                size_t length = load_data(i);
                g_flags.valid[i] = 1;
                if (length < PLAYER_BUFFER_SIZE) {
                    // We got less than the buffersize of data back. Fill the
                    // remainder of the buffer with silence.
                    size_t offset = i * PLAYER_BUFFER_SIZE + length;
                    size_t remainder = (PLAYER_BUFFER_SIZE - length) * sizeof(int16_t);
                    memset(g_buffer + offset, 0, remainder);
                    g_state = PLAYER_STOPPING;
                }
            }
        }
        // The upper code fills the buffer always from the lower half on. But
        // that half could be in use by the dma and the loaded data would never
        // be sent out. (As the ISR resets the valid flag, the upper code would
        // then overwrite the unsent data.) So up to PLAYER_BUFFER_SIZE of data
        // will not be sent out. But "normal" sound data would anyway start near
        // silent and PLAYER_BUFFER_SIZE relative to the whole audio stream size
        // is less than 1 %.
        break;
    case (PLAYER_STOPPING):
        // We are stopping and ISR is sending the last data. Wait until all data
        // is sent and then clear buffer.
        if (!g_flags.valid[LOWER_HALF] && !g_flags.valid[UPPER_HALF]) {
            memset(g_buffer, 0, sizeof(g_buffer[0]) * MAX_HALF * PLAYER_BUFFER_SIZE);
            g_state = PLAYER_STOPPED;
        }
    }
    return 0;
}

int player_play() {
    if (g_state == PLAYER_NOT_INITIALIZED) {
        return -1;
    }
    g_state = PLAYER_PLAYING;
    return 0;
}

int player_stop() {
    if (g_state == PLAYER_NOT_INITIALIZED) {
        return -1;
    }
    g_state = PLAYER_STOPPING;
    return 0;
}

void player_set_volume(uint8_t volume) {
    CS42L51_Mute(volume == 0);
    // ToDo: Verify that volume levels are correctly used.
    CS42L51_VolumeOutCtrl((int8_t)volume);
}

void DMA1_Stream4_IRQHandler(void) {
    // Check what triggered the interrupt. If it was transfer half complete then
    // we set a flag to reload the lower half of the buffer. If the interrupt
    // was from the transfer complete we set a flag to reload the upper half.
    int half;
    if (DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4) == SET) {
        // transfer complete
        half = UPPER_HALF;
    } else if (DMA_GetITStatus(DMA1_Stream4, DMA_IT_HTIF4) == SET) {
        // transfer half complete
        half = LOWER_HALF;
    } else {
        // should not get here, ignore
        return;
    }
    // clear the interrupt flags
    DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_HTIF4 | DMA_IT_TCIF4);
    // tell the main loop to reload the buffer half
    g_flags.valid[half] = 0;
}

static size_t load_data(int half) {
    size_t length = PLAYER_BUFFER_SIZE;
    if (g_callback(&g_buffer[half * PLAYER_BUFFER_SIZE], &length)) {
        // On error give a length of 0 back to initiate a stop sequence.
        length = 0;
    }
    return length;
}
