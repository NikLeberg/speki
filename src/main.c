#include <stm32f4xx.h>
#include <carme.h>
#include <carme_io1.h>
#include <carme_io2.h>
#include <stdlib.h>

#include "utils.h"
#include "songs.h"
#include "player.h"
#include "display.h"
#include "dft.h"

#define MAX_SONGS 10                   //!< define how many songs can be loaded
static song_t songs[MAX_SONGS];        //!< array of possibly available songs
static size_t songs_count = MAX_SONGS; //!< count of really available songs
static song_t *selected_song;          //!< currently playing song

/**
 * @brief Load the next chunk of audio data and run dft.
 * 
 * @param[in,out] data pointer to a buffer of size PLAYER_BUFFER_SIZE
 * @param[out] length size of valid buffer data, \ref PLAYER_BUFFER_SIZE or less
 * @retval 0 when new data could be loaded
 * @retval -1 when data could not be loaded
 */
int load_audio_data(int16_t *data, size_t *length);

/**
 * @brief Check for new button presses or potentiometer changes.
 * 
 * Don't call this too often, reading of ADC takes a long time.
 */
void handle_input(void);

/**
 * @brief Main loop.
 * 
 * @retval 0 (never returns though)
 */
int main(void) {
    // initialize CARME IO
    CARME_IO1_Init(); // used for pushbuttons
    CARME_IO2_Init(); // used for potentiometer

    // initialize submodules
    utils_init();                          // starts SysTick timer
    songs_init();                          // mounts SD-card filesystem
    songs_list_songs(songs, &songs_count); // loads available songs from SD-card
    player_init(load_audio_data);          // starts audio hardware and DMA
    display_init();                        // starts lcd hardware
    display_set_list(songs, songs_count);  // give display the available songs
    dft_init();                            // precalculate twiddle factors

    // infinite loop
    while (1) {
        player_loop();
        display_loop();
        // React to button presses and poti changes every 100 ms.
        static uint32_t last_ticks;
        uint32_t ticks = get_ticks();
        if (ticks - last_ticks > 100) {
            last_ticks = ticks;
            handle_input();
        }
    }

    // never get here
    return 0;
}

int load_audio_data(int16_t *data, size_t *length) {
    int err = songs_read_song(selected_song, data, length);
    // The audio samples, that will be transformed with dft are out of sync with
    // the music being played by 20ms. Because in this function call the next
    // chunk of samples is preloaded while the previous is still playing. So by
    // calculating the dft based on this new chunk, and sending it to the lcd
    // we are out of sync. However, because the dft calculation uses quite some
    // time, it may just be in sync again.
    uint32_t magnitude[DFT_MAGNITUDE_SIZE];
    dft_transform(data, magnitude);
    display_set_spectogram(magnitude + 1, UINT32_MAX);
    return err;
}

void handle_input(void) {
    // React to (new) button presses:
    // Button 0: Play currently selected song (changes display to song view).
    // Button 1: Stop playing song (changes display to list view).
    // Button 2: Move selection down in list (has only an effect in list view).
    // Button 3: Move selection up in list (has only an effect in list view).
    static uint8_t last_buttons;
    uint8_t current_buttons;
    CARME_IO1_BUTTON_Get(&current_buttons);
    uint8_t changed_buttons = current_buttons & ~last_buttons;
    last_buttons = current_buttons;
    if (changed_buttons & 0x01) {
        // play
        // get selected song from display
        display_get_selection(&selected_song);
        // load the song (should not fail as the song was already validated)
        songs_open_song(selected_song->filename, selected_song);
        // start player
        player_play();
        // display song info
        display_set_song(selected_song);
    } else if (changed_buttons & 0x02) {
        // stop
        player_stop();
        display_set_list(songs, songs_count);
    } else if (changed_buttons & 0x04) {
        // move down
        display_move_selection(0);
    } else if (changed_buttons & 0x08) {
        // move up
        display_move_selection(1);
    }
    // React to (significant) potentiometer changes.
    static uint16_t last_poti;
    uint16_t poti;
    CARME_IO2_ADC_Get(CARME_IO2_ADC_PORT0, &poti);
    if (abs(poti - last_poti) > 10) {
        last_poti = poti;
        // map poti value from [0 to 955] to a volume of [128 to 255]
        uint8_t volume = map_value_u(poti, 0, 955, 128, 255);
        player_set_volume(volume);
    }
}

/**
 * @brief Halt program when a debug assert in the BSP was triggered.
 * 
 * @param file unused
 * @param line unused
 */
void assert_failed(uint8_t *file, uint32_t line) {
    (void)file;
    (void)line;
    while (1) {
        ;
    }
}
