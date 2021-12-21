/**
 * @file player.h
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Interface for audio playback with cs42l51 codec.
 * @version 0.3
 * @date 2021-12-11
 * 
 * @copyright Copyright (c) 2021 Niklaus Leuenberger
 * 
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Size for audio buffer.
 * 
 * A PCM stream requested with \ref player_load_data_callback should have this
 * many halfwords (two bytes) as length.
 */
#define PLAYER_BUFFER_SIZE (1024U)

/**
 * @brief Load data callback prototype.
 * 
 * When the player is playing audio i.e. \ref player_play was called, then the
 * player calls this callback to load the first PLAYER_BUFFER_SIZE halfwords of
 * the bitstream that should be played. While that data is being transfered over
 * DMA to the audio codec, the callback is called again to load the next data.
 * After the first part is finished playing the second part will be transfered
 * with DMA. Then a third part is requested with this callback. This goes on
 * until the callback gives a length of less than \ref PLAYER_BUFFER_SIZE back.
 * When that happens, no more data will be requested, instead the player will
 * pause.
 * 
 * The PCM bitstream should have:
 *  - a samplerate of 48000 Hz
 *  - interleaved left and right channels (starting with left)
 *  - bit depth of 16 bits
 * 
 * @param[in,out] data pointer to a buffer of size PLAYER_BUFFER_SIZE
 * @param[out] length size of valid buffer data, \ref PLAYER_BUFFER_SIZE or less
 * @retval 0 on success (valid data and length, will continue playing)
 * @retval -1 on failure (invalid data or length, will stop playing)
 */
typedef int (*player_load_data_callback)(int16_t *data, size_t *length);

/**
 * @brief Initialize audio hardware.
 * 
 * Configures the audio hardware, sets up DMA stream and interrupt.
 * 
 * @param callback function that gets called when new data should be read in
 * @retval 0 on success
 * @retval -1 on failure
 */
int player_init(player_load_data_callback callback);

/**
 * @brief Main loop of player module.
 * 
 * Needs to be called periodically, at least once for every: size of audiofile
 * in halfwords divided by PLAYER_BUFFER_SIZE. Will return as fast as possible
 * if nothing has to be done.
 * 
 * @retval 0 on success
 * @retval -1 on failure
 */
int player_loop();

/**
 * @brief Start playing audio.
 * 
 * @retval 0 on success
 * @retval -1 on failure
 */
int player_play();

/**
 * @brief Stop playing audio.
 * 
 * @retval 0 on success
 * @retval -1 on failure
 */
int player_stop();

/**
 * @brief Set output volume.
 * 
 * @note The codec states a volume level of +12.0 dB for the maximum setting.
 * 
 * @param volume 0 = mute, 255 = max
 */
void player_set_volume(uint8_t volume);
