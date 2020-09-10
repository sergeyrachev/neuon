#!/usr/bin/env bash

SYNC_FOLDER=$1
UNSYNC_FOLDER=$2
DELAY=$3

DELAY_SEC=0$(echo "${DELAY}/1000" | bc -l )

for f in $(ls ${SYNC_FOLDER}/*.mp4)
do
    file=$(basename ${f})
    ffmpeg -nostdin -y -i ${SYNC_FOLDER}/${file} -itsoffset ${DELAY_SEC} -i ${SYNC_FOLDER}/${file} -c:v copy -c:a copy -map 0:v -map 1:a -shortest ${UNSYNC_FOLDER}/${file}
done


