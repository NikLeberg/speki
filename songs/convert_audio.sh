#!/usr/bin/env bash

if [ $# -lt 3 ]; then
    echo "Expected tree arguments. Please call script as follows:"
    echo "./convert_audio.sh \"input_file.xyz\" \"artist\" \"title\""
    exit 1
fi

ffmpeg \
    -i $1 \
    -ar 48000 -ac 2 -sample_fmt s16 \
    -map_metadata -1 \
    -metadata title=$3 \
    -metadata artist=$2 \
    ${2/ /-}_${3/ /-}.wav
