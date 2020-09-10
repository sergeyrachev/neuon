#!/usr/bin/env bash

ANSWER=$1
FACEMARK=$2
INFOLDER=$3
OUTFOLDER=$4
FILELIST=$5


while IFS= read -r video
do
    ../cmake-build-relwithdebinfo/neuon-data -i "${INFOLDER}/${video}" -a ${ANSWER} -o "${OUTFOLDER}/${video}.data" -l ${FACEMARK}
done < "$FILELIST"

