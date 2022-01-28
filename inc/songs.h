/**
 * @file songs.h
 * @author Leuenberger Niklaus <leuen4@bfh.ch>
 * @brief Interface for loading song files and their infos from the SD-Card.
 * @version 0.1
 * @date 2021-12-17
 * 
 * @copyright Copyright (c) 2021 Niklaus Leuenberger
 * 
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <ff.h>

/**
 * @brief Maximum length for artist and name strings in \ref song_t structure.
 * 
 */
#define SONGS_MAX_STRING_LENGTH (30U)

/**
 * @brief Maximum length of FatFS filenames.
 * @note Do not change! The standard specifies this to be 13 bytes:
 *  - 8 chars for filename
 *  - 1 for the dot "."
 *  - 3 chars for file extension
 *  - 1 char for the null byte.
 */
#define SONGS_MAX_FATFS_FILE_NAME_LENGTH (sizeof(((FILINFO *)0)->fname))

/**
 * @brief Song file object.
 * 
 * Holds the necessary FatFS filepointer and meta info of artist and song name
 * as well as stats about the already read length.
 * @note The data is read only. Changing values will lead to incorrect function.
 */
typedef struct {
    FIL file;
    char filename[SONGS_MAX_FATFS_FILE_NAME_LENGTH];
    char name[SONGS_MAX_STRING_LENGTH];
    char artist[SONGS_MAX_STRING_LENGTH];
    char bmp_name[SONGS_MAX_FATFS_FILE_NAME_LENGTH]; // name of BMP file of album cover
    size_t samples;                                  // num of samples of the full song
    size_t samples_read;                             // num samples already read
} song_t;

/**
 * @brief Initialize filesystem.
 * 
 * @retval 0 on success
 * @retval -1 on failure
 */
int songs_init(void);

/**
 * @brief Retrieve a list (array) of all songs on the SD-Card.
 * 
 * This searches the root folder of the SD-Card for .wav files, checks their
 * validity and returns the filled array.
 * 
 * @param[in,out] songs in: an array of song_t structures
 *                      out: the first "length"-count elements of the array are
 *                      filled with song information.
 * @param[in,out] length in: how many elements in the array of song_t's are
 *                       out: how many valid songs were found
 * @retval 0 on success
 * @retval -1 on failure
 */
int songs_list_songs(song_t songs[], size_t *length);

/**
 * @brief Open song by name.
 * 
 * @note \par name has to comply to the fat file naming rules (8.3) e.g.
 * "test_abcd.wav" is accessed as "TEST_A~1.WAV".
 * @note The WAV file is validated for correct sample frequency of 48 kHz and
 * correct pcm format. Non conforming files will not be opened.
 * 
 * @param name name of the file to open (has to end in .wav)
 * @param[out] song opened song
 * @retval 0 on success
 * @retval -1 on failure
 */
int songs_open_song(char *name, song_t *song);

/**
 * @brief Close song.
 * 
 * @param song a with \ref songs_open_song() opened song
 * @retval 0 on success
 * @retval -1 on failure
 */
int songs_close_song(song_t *song);

/**
 * @brief Reads a song into a buffer.
 * 
 * @note Assumes the buffer has to be filled with the raw stereo pcm stream of
 * 48 kHz sampled data.
 * @note \par length is relative to the count of samples e.g. halfwords. A
 * length of 1 loads 2 bytes.
 * 
 * @param song song to read
 * @param[out] buffer buffer to read into
 * @param[in,out] length in: length to read, out: length actually read
 * @retval 0 on success
 * @retval -1 on failure
 */
int songs_read_song(song_t *song, int16_t *buffer, size_t *length);

/**
 * @brief Convert a count of samples into seconds.
 * 
 * A song is played with 48 kHz and is stereo. That is 2 * 48000 samples per
 * second.
 * 
 * @param samples count of samples
 * @return seconds
 */
#define SONGS_SAMPLES_TO_SECONDS(samples) (samples / (2U * 48000U))
