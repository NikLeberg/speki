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

#define MAX_SONGS 10
static song_t songs[MAX_SONGS];
static size_t songs_count = MAX_SONGS;
static song_t *selected_song;

int load_audio_data(int16_t *data, size_t *length);

void handle_input();

int main(void) {
    CARME_IO1_Init();
    CARME_IO2_Init();

    utils_init();

    songs_init();
    songs_list_songs(songs, &songs_count);

    player_init(load_audio_data);

    display_init();
    display_set_list(songs, songs_count);

    // infinite loop
    while (1) {
        player_loop();  // 36.0 %, 25.8 %, 25.8 %
        display_loop(); // 45.2 %, 47.3 %
        // React to button presses and poti changes every 100 ms.
        static uint32_t last_ticks;
        uint32_t ticks = get_ticks();
        if (ticks - last_ticks > 100) {
            last_ticks = ticks;
            profile_enter(3);
            handle_input();
            profile_leave(3);
        }
    }
    return 0;
}

int load_audio_data(int16_t *data, size_t *length) {
    int speki[DISPLAY_NUM_OF_SPECTOGRAM_BARS];
    for (int i = 0; i < DISPLAY_NUM_OF_SPECTOGRAM_BARS; ++i) {
        speki[i] = rand();
    }
    display_set_spectogram(speki, RAND_MAX);
    return songs_read_song(selected_song, data, length);
}

void handle_input() {
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
        // profile
        uint32_t min1, max1, avg1;
        float load1;
        profile_stop(1, NULL, &min1, &max1, &avg1, &load1);
        uint32_t min2, max2, avg2;
        float load2;
        profile_stop(2, NULL, &min2, &max2, &avg2, &load2);
        uint32_t min3, max3, avg3;
        float load3;
        profile_stop(3, NULL, &min3, &max3, &avg3, &load3);
        delay_ms(1);
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
        uint8_t volume = map_value(poti, 0, 955, 128, 255);
        player_set_volume(volume);
    }
}

void assert_failed(uint8_t *file, uint32_t line) {
    while (1) {
        ;
    }
}
