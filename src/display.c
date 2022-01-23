/**
 * @file display.c
 * @author Leuenberger Niklaus <leuen4@bfh.ch>
 * @brief Module for displaying song info and spectogram to the LCD.
 * @version 0.1
 * @date 2021-12-19
 * 
 * @copyright Copyright (c) 2021 Niklaus Leuenberger
 * 
 * This display module handles two modes of operation:
 *  1. List view
 *  2. Song view
 * 
 * In the list view, a list of all available songs is displayed on the entire
 * screen. Line by line with the content of "Artist" - "Songname". The currently
 * selected song is displayed in inverted colors. This selection can be moved.
 * +=============================+
 * | Artist1 - Song1             |
 * | Artist1 - Song2             |
 * | Artist2 - Song1             |
 * : ...                         :
 * |                             |
 * +=============================+
 * 
 * In the song view, a previously selected song is being played (by other
 * modules). In the bottom part of the screen the album cover, song title,
 * artist and playing progress (once as "loading bar" and once as "time played")
 * is displayed. On the rest of the screen the spectogram is displayed as a
 * series of bars with different heights.
 * +=============================+
 * | +     +       +       + +   |
 * | | +   |   +   |     + | | + |
 * | | |   | + |   |   + | | | | |
 * | + +   + + + + +   + + + + + |
 * | ######## Song1              |
 * | ######## Artist1            |
 * | ######## =====              |
 * | ######## 00:12 / 01:12      |
 * +=============================+
 */

#include <lcd.h>
#include <stdio.h>

#include "display.h"
#include "utils.h"

// Sizes of elements and units
#define MARGIN (6U)
#define BOTTOM_STRIP (80U)
#define ALBUM_COVER (80U)
#define PROGRESS_BAR (5U)
#define FONT_NORMAL (&font_8x13)
#define FONT_BOLD (&font_8x13B)
#define FONT_ITALIC (&font_8x13O)

// calculate positions and sizes of elements relative to eachother
#define LIST_FONT (FONT_NORMAL)

#define SPECTOGRAM_START_X (0U)
#define SPECTOGRAM_END_X (SCRN_RIGHT)
#define SPECTOGRAM_START_Y (0U)
#define SPECTOGRAM_END_Y (SCRN_BOTTOM - BOTTOM_STRIP)
#define SPECTOGRAM_HEIGHT (SPECTOGRAM_END_Y - SPECTOGRAM_START_Y)
#define SPECTOGRAM_WIDTH ( \
    (SPECTOGRAM_END_X - SPECTOGRAM_START_X - DISPLAY_NUM_OF_SPECTOGRAM_BARS * MARGIN) / DISPLAY_NUM_OF_SPECTOGRAM_BARS)

#define NAME_FONT (FONT_NORMAL)
#define NAME_START_X (ALBUM_COVER + MARGIN)
#define NAME_START_Y (SPECTOGRAM_END_Y + MARGIN)
#define NAME_END_Y (NAME_START_Y + NAME_FONT->height)

#define ARTIST_FONT (FONT_ITALIC)
#define ARTIST_START_X (NAME_START_X)
#define ARTIST_START_Y (NAME_END_Y + MARGIN)
#define ARTIST_END_Y (ARTIST_START_Y + ARTIST_FONT->height)

#define PROGRESS_START_X (ARTIST_START_X)
#define PROGRESS_END_X (SCRN_RIGHT - MARGIN)
#define PROGRESS_START_Y (ARTIST_END_Y + MARGIN)
#define PROGRESS_END_Y (PROGRESS_START_Y + PROGRESS_BAR)

#define PLAY_TIME_FONT (FONT_NORMAL)
#define PLAY_TIME_START_X (PROGRESS_START_X)
#define PLAY_TIME_START_Y (PROGRESS_END_Y + MARGIN)
#define PLAY_TIME_END_Y (PLAY_TIME_START_Y + PLAY_TIME_FONT->height)

/**
 * @brief Display states.
 * 
 */
static enum {
    DISPLAY_NOT_INITIALIZED = 0,
    DISPLAY_INITIALIZED,
    DISPLAY_INIT_LIST,
    DISPLAY_LIST,
    DISPLAY_INIT_SONG,
    DISPLAY_SONG
} g_state;

static const song_t *g_current_list; //!< pointer to the current song list
static size_t g_list_length;         //!< length of the list
static size_t g_list_selection;      //!< currently selected song in list

static const song_t *g_current_song; //!< pointer to the currently playing song
static uint16_t g_spectogram[DISPLAY_NUM_OF_SPECTOGRAM_BARS];

/**
 * @brief Flags for the main display loop.
 * 
 */
static struct {
    __IO int update_done;   //!< lcd updated the screen, is called with ~50 Hz
    int spectogram_updated; //!< new spectogram data was given, need to update
} g_flags;

/**
 * @brief Callback for LCD update.
 * 
 * This gets registered as LCD update callback and will be called from an ISR
 * after the LCD refreshed the screen. Use to synchronize to the framerate of
 * the LCD.
 */
static void update_callback();

/**
 * @brief Draw the song list.
 * 
 */
static void update_song_list();

/**
 * @brief Initialize spectogram.
 * 
 */
static void init_spectogram();

/**
 * @brief Draw the spectogram bars.
 * 
 */
static void update_spectogram();

/**
 * @brief Draw album cover and song meta information once.
 * 
 */
static void init_play_stats();

/**
 * @brief Update played song duration.
 * 
 */
static void update_play_stats();

int display_init() {
    // check current state
    if (g_state != DISPLAY_NOT_INITIALIZED) {
        // only allow initialization once
        return -1;
    }

    // init LCD
    LCD_Init();
    LCD_RegisterUpdateCallback(update_callback);
    LCD_Clear(GUI_COLOR_BLACK);

    g_state = DISPLAY_INITIALIZED;
    return 0;
}

int display_loop() {
    // only ever do something if the LCD was updated
    if (!g_flags.update_done) {
        return 0;
    }
    g_flags.update_done = 0;

    switch (g_state) {
    case (DISPLAY_NOT_INITIALIZED):
        // can't run, not initialized
        return -1;
        break;
    case (DISPLAY_INITIALIZED):
        // initialized but nothing to do
        return 0;
        break;
    case (DISPLAY_INIT_LIST):
        LCD_Clear(GUI_COLOR_BLACK);
        g_state = DISPLAY_LIST;
        // fallthrough
    case (DISPLAY_LIST):
        // draw a list of all songs
        update_song_list();
        break;
    case (DISPLAY_INIT_SONG):
        LCD_Clear(GUI_COLOR_BLACK);
        // init spectogram
        init_spectogram();
        // draw album cover and static song info once
        init_play_stats();
        g_state = DISPLAY_SONG;
        break;
    case (DISPLAY_SONG):
        // update spectogram
        update_spectogram();
        // update play stats
        update_play_stats();
        break;
    }
    return 0;
}

int display_set_list(const song_t songs[], size_t length) {
    if (g_state == DISPLAY_NOT_INITIALIZED) {
        return -1;
    }
    g_current_list = songs;
    g_list_length = length;
    g_state = DISPLAY_INIT_LIST;
    return 0;
}

int display_move_selection(int direction) {
    if (g_state != DISPLAY_LIST) {
        return -1;
    }
    // Move the current selection index up or down. If it goes out of bounds do
    // the most reasonable and set the index to the start or to the end.
    if (direction) {
        // up
        if (g_list_selection == 0) {
            g_list_selection = g_list_length - 1;
        } else {
            --g_list_selection;
        }
    } else {
        // down
        if (g_list_selection == g_list_length - 1) {
            g_list_selection = 0;
        } else {
            ++g_list_selection;
        }
    }
    return 0;
}

int display_get_selection(song_t **song) {
    if (g_state != DISPLAY_LIST) {
        return -1;
    }
    *song = (song_t *)&g_current_list[g_list_selection];
    return 0;
}

int display_set_song(const song_t *song) {
    if (g_state == DISPLAY_NOT_INITIALIZED) {
        return -1;
    }
    g_current_song = song;
    g_state = DISPLAY_INIT_SONG;
    return 0;
}

int display_set_spectogram(uint32_t spectogram[DISPLAY_NUM_OF_SPECTOGRAM_BARS], uint32_t max_value) {
    if (g_state != DISPLAY_SONG && g_state != DISPLAY_INIT_SONG) {
        return -1;
    }
    // Convert the input value from the range [0 to max_value] to
    // [SPECTOGRAM_HEIGHT to 0]. This inversion (max value in input is min value
    // in output) is necessary because the pixels on the LCD are counted from
    // the top down.
    for (int i = 0; i < DISPLAY_NUM_OF_SPECTOGRAM_BARS; ++i) {
        g_spectogram[i] = map_value_u(spectogram[i], 0, max_value,
                                      SPECTOGRAM_HEIGHT, SPECTOGRAM_START_Y);
    }
    g_flags.spectogram_updated = 1;
    return 0;
}

static void update_callback() {
    g_flags.update_done = 1;
}

static void update_song_list() {
    LCD_SetFont(LIST_FONT);
    for (int i = 0; i < g_list_length; ++i) {
        // invert the colors for the currently selected list entry
        LCD_SetTextColor(i == g_list_selection ? GUI_COLOR_BLACK : GUI_COLOR_WHITE);
        LCD_SetBackColor(i == g_list_selection ? GUI_COLOR_WHITE : GUI_COLOR_BLACK);
        char tmp[(SONGS_MAX_STRING_LENGTH * 2) + 3]; // "artist" + " - " + "title"
        snprintf(tmp, sizeof(tmp), "%s - %s", g_current_list[i].artist, g_current_list[i].name);
        LCD_DisplayStringLine(i, tmp);
    }
    LCD_SetTextColor(GUI_COLOR_WHITE);
    LCD_SetBackColor(GUI_COLOR_BLACK);
}

static void init_spectogram() {
    for (int i = 0; i < DISPLAY_NUM_OF_SPECTOGRAM_BARS; ++i) {
        g_spectogram[i] = 0;
    }
}

static void update_spectogram() {
    if (g_flags.spectogram_updated) {
        g_flags.spectogram_updated = 0;
        // Instead of clearing the space of the spectogram to black first and
        // then drawing the individual bars ontop, use a more efficient aproach:
        // For each bar, clear the part that isnt "filled" to black and then set
        // the normal "filled" part of the bar to white. This removes the need
        // to "sweep" two times over the screen space.
        for (int i = 0; i < DISPLAY_NUM_OF_SPECTOGRAM_BARS; ++i) {
            uint16_t bar_start_x =
                SPECTOGRAM_START_X + (MARGIN / 2U) + i * (SPECTOGRAM_WIDTH + MARGIN);
            LCD_FillArea(bar_start_x, SPECTOGRAM_START_Y,
                         bar_start_x + SPECTOGRAM_WIDTH, g_spectogram[i], GUI_COLOR_BLACK);
            LCD_FillArea(bar_start_x, g_spectogram[i],
                         bar_start_x + SPECTOGRAM_WIDTH, SPECTOGRAM_END_Y, GUI_COLOR_WHITE);
        }
    }
}

static void init_play_stats() {
    // album cover
    LCD_BMP_DrawBitmap(g_current_song->bmp_name, 0, SPECTOGRAM_END_Y);
    // song name and artist
    LCD_SetFont(NAME_FONT);
    LCD_DisplayStringXY(NAME_START_X, NAME_START_Y, g_current_song->name);
    LCD_SetFont(ARTIST_FONT);
    LCD_DisplayStringXY(ARTIST_START_X, ARTIST_START_Y, g_current_song->artist);
    // playing time
    char tmp[14];
    int secs = SONGS_SAMPLES_TO_SECONDS(g_current_song->samples);
    int mins = secs / 60;
    secs -= mins * 60;
    snprintf(tmp, sizeof(tmp), "00:00 / %02d:%02d", mins, secs);
    LCD_SetFont(PLAY_TIME_FONT);
    LCD_DisplayStringXY(PLAY_TIME_START_X, PLAY_TIME_START_Y, tmp);
}

static void update_play_stats() {
    // progress bar
    static int last_bar_end;
    uint16_t bar_end_x =
        map_value_u(g_current_song->samples_read, 0, g_current_song->samples, PROGRESS_START_X, PROGRESS_END_X);
    if (bar_end_x != last_bar_end) {
        last_bar_end = bar_end_x;
        LCD_FillArea(PROGRESS_START_X, PROGRESS_START_Y, bar_end_x, PROGRESS_END_Y, GUI_COLOR_WHITE);
    }
    // playing time
    static int last_seconds;
    int secs = SONGS_SAMPLES_TO_SECONDS(g_current_song->samples_read);
    if (secs != last_seconds) {
        last_seconds = secs;
        int mins = secs / 60;
        secs -= mins * 60;
        char tmp[6];
        snprintf(tmp, sizeof(tmp), "%02d:%02d", mins, secs);
        LCD_SetFont(PLAY_TIME_FONT);
        LCD_DisplayStringXY(PLAY_TIME_START_X, PLAY_TIME_START_Y, tmp);
    }
}
