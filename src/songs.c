/**
 * @file songs.h
 * @author NikLeberg (niklaus.leuenb@gmail.com)
 * @brief Module for loading song files and their infos from the SD-Card.
 * @version 0.1
 * @date 2021-12-17
 * 
 * @copyright Copyright (c) 2021 Niklaus Leuenberger
 * 
 * The .WAV file parsing is based on the following webstes:
 * http://soundfile.sapp.org/doc/WaveFormat/
 * http://www.piclist.com/techref/io/serial/midi/wave.html
 * https://www.recordingblogs.com/wiki/list-chunk-of-a-wave-file
 * 
 */

#include <string.h>

#include "songs.h"

// general chunk header
typedef struct __attribute__((packed)) {
    char chunk_id[4];    // id of chunk, "RIFF", "fmt " and "LIST" supported
    uint32_t chunk_size; // size of chunk data, excluding the header (- 8 bytes)
} chunk_header_t;

// chunk id "RIFF"
typedef struct __attribute__((packed)) {
    char format[4]; // "WAVE"
} riff_chunk_t;

// chunk id "fmt "
typedef struct __attribute__((packed)) {
    uint16_t audio_format;    // = 1 for PCM
    uint16_t num_channels;    // Mono = 1, Stereo = 2
    uint32_t sample_rate;     // 8000, 44100, 48000 etc.
    uint32_t byte_rate;       // sample_rate * num_channels * (bits_per_sample / 8)
    uint16_t block_align;     // num_channels * (bits_per_sample / 8)
    uint16_t bits_per_sample; // 8, 16, 24 or 32
} fmt_chunk_t;

// chunk id "LIST"
typedef struct __attribute__((packed)) {
    char format[4]; // "INFO"
} list_chunk_t;

#define STRING_NOT_EQUAL(expected, str) (strncmp(str, expected, sizeof(str)) != 0)
#define STRING_EQUAL(expected, str) (strncmp(str, expected, sizeof(str)) == 0)

static FATFS main_fs;

static int open(char *name, song_t *song);
static int read(song_t *song, void *buffer, size_t length);
static void skip_chunk(song_t *song, chunk_header_t *header);
static int parse_default_header(song_t *song);
static int parse_info_header(song_t *song);
static int parse_data_header(song_t *song);

int songs_init() {
    // initialize FatFS and mount SD-Card
    // (source: BSP sdcard example from https://web.carme.bfh.science/)
    return (f_mount(&main_fs, "0:", 1) != FR_OK);
}

int songs_list_songs(song_t songs[], size_t *length) {
    // Check parameters.
    if (!songs || !length) {
        return -1;
    }
    // Open root directory to search for .wav files.
    DIR dir;
    if (f_opendir(&dir, "/") != FR_OK) {
        return -1;
    }
    // Loop over all files in the directory. Loop is not recursive,
    // subdirectories will be ignored.
    int song_nr = 0;
    while (1) {
        FILINFO fno;
        if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0) {
            // exit loop on error or at end of directory content
            break;
        }
        if (fno.fattrib & AM_DIR) {
            // its a directory, ignore
            continue;
        }
        // We found a file. Check if it is a .wav file.
        if (!strnstr(fno.fname, ".WAV", SONGS_MAX_FATFS_FILE_NAME_LENGTH)) {
            // did not find ".wav" in filename, ignore file
            continue;
        }
        // Try to open file. This parses all the wav file headers and checks the
        // validity of the file. Invalid files will be silently skipped!
        if (open(fno.fname, &songs[song_nr])) {
            // could not open song or it had invalid format, ignore
            continue;
        }
        // File could be opened and header was valid. Close filesystem object as
        // we don't want to read the audio data yet.
        f_close(&songs[song_nr].file);
        // If there is space for another song in the given songs array, continue
        // loop and search for more. Otherwise exit loop and ignore all
        // remaining files.
        song_nr++;
        if (song_nr >= *length) {
            break;
        }
    }
    f_closedir(&dir);
    *length = song_nr;
    return 0;
}

int songs_open_song(char *name, song_t *song) {
    // check parameters
    if (!name || !song) {
        return -1;
    }
    // reset song structure
    songs_close_song(song);
    // open .wav file if it exists
    if (open(name, song)) {
        return -1;
    }
    return 0;
}

int songs_close_song(song_t *song) {
    // parameters
    if (!song) {
        return -1;
    }
    // Clean up song structure, but don't clear the filename, could be in use by
    // the user.
    f_close(&song->file);
    song->name[0] = '\0';
    song->artist[0] = '\0';
    song->bmp_name[0] = '\0';
    song->samples = 0;
    song->samples_read = 0;
    return 0;
}

int songs_read_song(song_t *song, int16_t *buffer, size_t *length) {
    int ret = f_read(&song->file, buffer, 2 * *length, length) != FR_OK;
    *length /= 2;
    song->samples_read += *length;
    return ret;
}

static int open(char *name, song_t *song) {
    // save filename into structure
    strncpy(song->filename, name, SONGS_MAX_FATFS_FILE_NAME_LENGTH);
    // open .wav file if it exists
    if (f_open(&song->file, song->filename, FA_OPEN_EXISTING | FA_READ) != FR_OK) {
        return -1;
    }
    // The wav file is open, now read and validate the file headers. First comes
    // the default RIFF and WAV format header, then the optional LIST INFO
    // header and last the actual data header after which the raw pcm stream is.
    if (parse_default_header(song)) {
        return -1;
    }
    if (parse_info_header(song)) {
        // song info was not present or not valid, set to unknown
        strncpy(song->artist, "Unknown", SONGS_MAX_STRING_LENGTH);
        strncpy(song->name, "Unknown", SONGS_MAX_STRING_LENGTH);
    }
    if (parse_data_header(song)) {
        return -1;
    }
    // look for album cover
    strncpy(song->bmp_name, song->filename, SONGS_MAX_FATFS_FILE_NAME_LENGTH);
    char *file_extension = strnstr(song->bmp_name, ".WAV", SONGS_MAX_FATFS_FILE_NAME_LENGTH);
    if (file_extension) {
        strcpy(file_extension, ".BMP");
        // try to open album
        FIL bmp;
        if (f_open(&bmp, song->bmp_name, FA_OPEN_EXISTING | FA_READ) != FR_OK) {
            // cover does not exist
            song->bmp_name[0] = '\0';
        }
        f_close(&bmp);
    }
    return 0;
}

static int read(song_t *song, void *buffer, size_t length) {
    size_t read_bytes = 0;
    f_read(&song->file, buffer, length, &read_bytes);
    return (read_bytes != length);
}

static void skip_chunk(song_t *song, chunk_header_t *header) {
    // If a chunk has an uneven length it is padded with an extra zero byte. So
    // do also skip over one byte extra if chunk_size is uneven.
    header->chunk_size += header->chunk_size % 2;
    f_lseek(&song->file, song->file.fptr + header->chunk_size);
}

static int parse_default_header(song_t *song) {
    // read chunk header for whole file
    //  - should be of id "RIFF"
    chunk_header_t header = {0};
    if (read(song, &header, sizeof(chunk_header_t)) ||
        STRING_NOT_EQUAL("RIFF", header.chunk_id)) {
        return -1;
    }
    // chunk data for id "RIFF"
    //  - should be just "WAVE"
    riff_chunk_t riff = {0};
    if (read(song, &riff, sizeof(riff_chunk_t)) ||
        STRING_NOT_EQUAL("WAVE", riff.format)) {
        return -1;
    }
    // read chunk header for format
    //  - should be of id "fmt "
    //  - should have a size of 16 bytes
    if (read(song, &header, sizeof(chunk_header_t)) ||
        STRING_NOT_EQUAL("fmt ", header.chunk_id) ||
        header.chunk_size != sizeof(fmt_chunk_t)) {
        return -1;
    }
    // chunk data for id "fmt ", should have:
    // - 16 bits depth
    // - 48 kHz sample rate
    // - stereo channel
    // - uncompressed pcm encoding
    fmt_chunk_t fmt = {0};
    if (read(song, &fmt, sizeof(fmt_chunk_t)) ||
        fmt.audio_format != 1 ||
        fmt.num_channels != 2 ||
        fmt.sample_rate != 48000 ||
        fmt.bits_per_sample != 16) {
        return -1;
    }
    return 0;
}

static int parse_info_header(song_t *song) {
    // read chunk header for list info (and skip unknown chunks if neccessary)
    //  - should be of id "LIST"
    chunk_header_t header = {0};
    while (1) {
        if (read(song, &header, sizeof(chunk_header_t)) ||
            header.chunk_size == 0) {
            // failed to read data
            return -1;
        }
        if (STRING_NOT_EQUAL("LIST", header.chunk_id)) {
            // this chunk is unknown, skip it
            skip_chunk(song, &header);
            continue;
        } else {
            // header was found, stop loop
            break;
        }
    }
    // chunk data for id "LIST"
    //  - should be just "INFO"
    list_chunk_t list = {0};
    if (read(song, &list, sizeof(list_chunk_t)) ||
        STRING_NOT_EQUAL("INFO", list.format)) {
        return -1;
    }
    // read chunk header of info (and skip unknown chunks if neccessary)
    //  - should be "IART" (name of artist)
    //  - or should be "INAM" (name of song)
    while (1) {
        if (read(song, &header, sizeof(chunk_header_t)) ||
            header.chunk_size == 0) {
            // failed to read data
            return -1;
        }
        if (STRING_EQUAL("IART", header.chunk_id) || STRING_EQUAL("INAM", header.chunk_id)) {
            // wo got the name of the artist or of the song, save it
            char temp[header.chunk_size];
            read(song, temp, header.chunk_size);
            char *dest = (header.chunk_id[1] == 'A') ? song->artist : song->name;
            strncpy(dest, temp, SONGS_MAX_STRING_LENGTH);
            dest[SONGS_MAX_STRING_LENGTH - 1] = '\0';
            if (header.chunk_size % 2) {
                f_lseek(&song->file, song->file.fptr + 1);
            }
        } else if (STRING_EQUAL("data", header.chunk_id)) {
            // We went too far and reached the end of the info chunk. The next
            // chunk is already the pcm data. Rewind the filepointer and exit.
            f_lseek(&song->file, song->file.fptr - sizeof(chunk_header_t));
            break;
        } else {
            // this chunk is unknown, skip it
            skip_chunk(song, &header);
        }
    }
    return 0;
}

static int parse_data_header(song_t *song) {
    // read chunk header for data (and skip unknown chunks if neccessary)
    //  - should be of id "data"
    chunk_header_t header = {0};
    while (1) {
        if (read(song, &header, sizeof(chunk_header_t)) ||
            header.chunk_size == 0) {
            // failed to read data
            return -1;
        }
        if (STRING_NOT_EQUAL("data", header.chunk_id)) {
            // this chunk is unknown, skip it
            skip_chunk(song, &header);
            continue;
        } else {
            // header was found, stop loop
            break;
        }
    }
    song->samples = header.chunk_size / 2;
    // File pointer is now at the end of all headers, what follows is just the
    // raw pcm bitstream.
    return 0;
}
