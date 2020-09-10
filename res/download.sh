#!/usr/bin/env bash

VIDEOLIST=$1
ORIGINAL_FOLDER=$2

mkdir -p ${ORIGINAL_FOLDER}

while IFS='' read -r video_id || [[ -n "$video_id" ]]
do
    echo "Downloading $video_id"
    youtube-dl -f "bestvideo[ext=mp4]+bestaudio[ext=m4a]/mp4" --output "${ORIGINAL_FOLDER}/%(id)s.mp4" "$video_id"
    sleep .2
done < "$VIDEOLIST"
