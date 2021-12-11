/**
 * @file player.h
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Interface for audio playback with cs42l51 codec.
 * @version 0.1
 * @date 2021-12-05
 * 
 * @copyright Copyright (c) 2021 Niklaus Leuenberger
 * 
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Size for audio chunks.
 * 
 * A PCM stream requested with \ref player_chunk_callback should have this many
 * halfwords (two bytes) as length.
 */
#define PLAYER_CHUNK_SIZE (1024U)

/**
 * @brief Chunk callback prototype.
 * 
 * When the player is playing audio i.e. \ref player_play was called, then the
 * player calls this callback to get a chunk of the bitstream that should be
 * played. While that chunk is being transfered over DMA to the audio codec, the
 * callback is called again to get the next chunk. After the first chunk is
 * finished playing the second chunk will be transfered with DMA. Then a third
 * chunk is requested with this callback. This goes on until the callback gives
 * a length of less than \ref PLAYER_CHUNK_SIZE back. When that happens, no more
 * chunks will be requested, instead the player will pause.
 * 
 * The PCM bitstream should have:
 *  - a samplerate of 48000 Hz
 *  - interleaved left and right channels (starting with left)
 *  - bit depth of 16 bits
 * 
 * @note bitstream is not copied but used in place
 * 
 * @param data pointer to pcm bitstream to play
 * @param length size of bitstream, \ref PLAYER_CHUNK_SIZE or less
 * @retval int 0 on success (valid data and length)
 * @retval int -1 on failure (invalid data or length)
 * 
 */
typedef int (*player_chunk_callback)(int16_t **data, size_t *length);

/**
 * @brief Initialize audio hardware.
 * 
 * Configures the audio hardware, sets up DMA stream and interrupt.
 * 
 * @param callback function that gets called when new data should be read in
 * @retval int 0 on success
 * @retval int -1 on failure
 */
int player_init(player_chunk_callback callback);

/**
 * @brief Deinitialize audio hardware and release ressources.
 * 
 * @retval int 0 on success
 * @retval int -1 on failure
 */
int player_deinit();

/**
 * @brief Main loop of player module.
 * 
 * Needs to be called periodically, at least once for every chunk of audio that
 * needs to be played. Will return as fast as possible if nothing has to be
 * done.
 * 
 * @retval int 0 on success
 * @retval int -1 on failure
 */
int player_loop();

/**
 * @brief Start playing audio.
 * 
 * @retval int 0 on success
 * @retval int -1 on failure
 */
int player_play();

/**
 * @brief Pause playing audio.
 * 
 * @retval int 0 on success
 * @retval int -1 on failure
 */
int player_pause();
