# Format of Necessary Files

For legal reasons no copyrighted example audio files will be found in this repository. But it is very easy to get your own files in the required format for Speki. Just use the commands as layed out in the forthcoming two sections and copy the converted files to the SD-Card.

## Audio Files

This project needs audio files in the following .wav format:
 - uncompressed PCM
 - 48 kHz
 - 16 bit depth
 - stereo
 - metadata in RIFF LIST INFO format

To create to such files `ffmpeg` can be used. With the following command a given input file will be converted to the required format:

```bash
ffmpeg \
    -i input_file.mp3 \               # file to convert, ffmpeg supports almost all file formats
    -ar 48000 -ac 2 -sample_fmt s16 \ # samplerate of 48 kHz with stereo 16 bit samples
    -map_metadata -1 \                # (optional) removes all existing  metadata
    -metadata title="title" \         # set tile of song
    -metadata artist="artist" \       # set artist of song
    output_file.wav                   # output file, has to end in .wav
```

Or just use the supplied script `convert_audio.sh` and call it as follows:

```bash
./convert_audio.sh "input_file.xyz" "artist" "title"
```

## Audio Cover

Optionally an album cover with the following requirements can be supplied:
 - same file name as audio file (e.g. if audio file is named `example.wav` it should be named `example.bmp`)
 - in uncompressed BMP format
 - bitmaps with 16, 24 or 32 bits per pixel
 - a dimension of 80x80 pixels

To create to such files `imagemagick` can be used. With the following command a given input file will be converted to the required format:

```bash
convert \
    input_file.jpg \                             # file to convert, imagemagick supports most formats
    -thumbnail '80x80>' -crop 80x80+0+0\! \      # crop to 80x80 pixels
    -gravity center -background black -flatten \ # create a black letterbox if input is not square
    -alpha off \                                 # disable alpha channel
    +compress \                                  # disable compression
    BMP3:output_file.bmp                         # output file, has to end in .bmp
```

Or just use the supplied script `convert_cover.sh` and call it as follows:

```bash
./convert_cover.sh "input_file.xyz" "artist" "title"
```
