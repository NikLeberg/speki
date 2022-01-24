#!/usr/bin/env bash

if [ ! $# -eq 3 ]; then
    echo "Expected three arguments. Please call script as follows:"
    echo "./convert_cover.sh \"input_file.xyz\" \"artist\" \"title\""
    exit 1
fi

convert \
    $1 \
    -thumbnail '80x80>' -crop 80x80+0+0\! \
    -gravity center -background black -flatten \
    -alpha off \
    +compress \
    BMP3:${2/ /-}_${3/ /-}.bmp
