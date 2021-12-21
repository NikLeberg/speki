#include <stdio.h>
#include <stm32f4xx.h>
#include <carme.h>
#include <carme_io1.h>
#include <carme_io2.h>
#include <uart.h>
#include "player.h"
#include <ff.h>
#include <cs42l51.h>

FATFS main_fs;

static FIL f;

int load_audio_data(int16_t *data, size_t *length) {
    if (!f.fs) {
        return -1;
    }
    size_t bytes;
    f_read(&f, data, 2 * PLAYER_BUFFER_SIZE, &bytes);
    *length = bytes / 2; // we read bytes but data is in halfwords
    if (bytes < 2 * PLAYER_BUFFER_SIZE) {
        f_close(&f);
        f.fs = NULL;
    }
    return 0;
}

int main(void) {
    // init USART for printf
    USART_InitTypeDef USART_init;
    USART_StructInit(&USART_init);
    USART_init.USART_BaudRate = 115200;
    CARME_UART_Init(CARME_UART0, &USART_init);
    setvbuf(stdout, NULL, _IONBF, 0); // disable linebuffering
    printf("\033c");                  // reset to initial state
    printf("\033[2J");                // clear screen
    printf("\033[?25l");              // cursor off
    printf("Welcome to CARME-M4.\n");

    CARME_IO1_Init();
    CARME_IO2_Init();

    if (f_mount(&main_fs, "0:", 1) != FR_OK) {
        while (1) {
            ;
        }
    }

    player_init(load_audio_data);
    player_play();

    // infinite loop
    while (1) {
        if (player_loop()) {
            puts("player stopped");
        }
        // open file
        uint8_t buttons;
        CARME_IO1_BUTTON_Get(&buttons);
        if (buttons) {
            f_close(&f);
            player_play();
        }
        if (buttons & 0x01) {
            f_open(&f, "ACCORD~1.PCM", FA_OPEN_EXISTING | FA_READ);
        } else if (buttons & 0x02) {
            f_open(&f, "ATMOSP~1.PCM", FA_OPEN_EXISTING | FA_READ);
        } else if (buttons & 0x04) {
            f_open(&f, "MELANC~1.PCM", FA_OPEN_EXISTING | FA_READ);
        } else if (buttons & 0x08) {
            //
        }
        // volume control (0 - 955)
        static uint16_t last_value;
        uint16_t value;
        CARME_IO2_ADC_Get(CARME_IO2_ADC_PORT0, &value);
        value = (value / 8) + 128;
        if (value > last_value + 1 || value < last_value - 1) {
            // vol: min 128, max 255
            // pot: min 0, max 955
            CS42L51_VolumeOutCtrl(value);
            printf("vol: %d\n", value);
            last_value = value;
        }
    }
    return 0;
}

void assert_failed(uint8_t *file, uint32_t line) {
    printf("\r\nassert_failed(). file: %s, line: %d\r\n", file, (int)line);
    while (1) {
        ;
    }
}
