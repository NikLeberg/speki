/**
 * @file display.h
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Interface for displaying song info and spectogram to the LCD.
 * @version 0.1
 * @date 2021-12-19
 * 
 * @copyright Copyright (c) 2021 Niklaus Leuenberger
 * 
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "songs.h"

/**
 * @brief Number of spectogram bars to print.
 * 
 * The count can be ajusted as wished, but the algorithm to draw the bars only
 * ever fills the screen to the full extend with values 1, 11, 29 or 319. This
 * comes from the fact that the LCD has a width of 320 pixels and those values
 * are the prime factors of width - 1. For other values the screen will have an
 * unused black strip to the right.
 * 
 * @note This also sets the number of spectogram data points expected by
 * \ref display_set_spectogram().
 */
#define DISPLAY_NUM_OF_SPECTOGRAM_BARS (29U)

/**
 * @brief Initialise display driver and lower hardware.
 * 
 * @retval 0 on success
 * @retval -1 on failure
 */
int display_init();

/**
 * @brief Main loop of display module.
 * 
 * The actual writing to the LCD happens here.
 * 
 * @retval 0 on success
 * @retval -1 on failure
 */
int display_loop();

/**
 * @brief Set the display into mode "List" and display a list of songs.
 * 
 * @param songs array of songs with valid infos, will be printed on screen
 * @param length how many songs there are in the list
 * @retval 0 on success
 * @retval -1 on failure
 */
int display_set_list(const song_t songs[], size_t length);

/**
 * @brief Change the currently selected song in the list.
 * 
 * The currently selected song will be printed with inverted text colors (white
 * background, black letters). With this function the selection can be moved up
 * or down step by step. This is intended for navigation buttons.
 * 
 * @note Only call in mode "List" e.g. after \ref display_set_list() was called.
 * 
 * @param direction 0 = move down in the list, 1 = move up in the list
 * @retval 0 on success
 * @retval -1 on failure (wrong mode)
 */
int display_move_selection(int direction);

/**
 * @brief Get the currently selected song in the list.
 * 
 * @note Only call in mode "List" e.g. after \ref display_set_list() was called.
 * 
 * @param[out] song will be set to a pointer to the currently selected song in
 *                  the initially given list with \ref display_set_list()
 * @retval 0 on success
 * @retval -1 on failure (wrong mode)
 */
int display_get_selection(song_t **song);

/**
 * @brief Set the display into mode "Song" and display song informations.
 * 
 * @note If the \par song does not contain a valid BMP album cover simply a
 * black rectangle will be shown.
 * 
 * @param song song witch informations should be printed on screen
 * @retval 0 on success
 * @retval -1 on failure
 */
int display_set_song(const song_t *song);

/**
 * @brief Set the spectogram data that should be displayed.
 * 
 * @param spectogram array of spectogram data in the range 0 to \par max_value
 * @param max_value upper limit of data, if a datapoint has this value the
 *                  spectogram bar will be at its maximum height / size
 * @retval 0 on success
 * @retval -1 on failure (wrong mode)
 */
int display_set_spectogram(uint32_t spectogram[DISPLAY_NUM_OF_SPECTOGRAM_BARS], uint32_t max_value);
