#!/usr/bin/env bash

FILELIST=$1
ORIGINAL_FOLDER=$2
SYNC_FOLDER=$3
FACEDB=$4

mkdir -p ${SYNC_FOLDER}

while IFS= read -r video_id
do
   description=$(find ${FACEDB} -name ${video_id}.txt -type f)
   while IFS= read -r clip
   do
        s=$(echo ${clip} | awk '//{print $2}')
        e=$(echo ${clip} | awk '//{print $3}')
        FFMPEG="ffmpeg -nostdin -y -noaccurate_seek -ss $s -i ${ORIGINAL_FOLDER}/$video_id.mp4 -to $e -c:v copy -c:a copy -copyts -avoid_negative_ts 1 ${SYNC_FOLDER}/${video_id}-$s-$e.mp4"
        echo "${FFMPEG}"
        ${FFMPEG}
    done <<< $(cat ${description} | grep ${video_id}_)
done < "$FILELIST"
