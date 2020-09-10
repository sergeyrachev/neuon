#!/usr/bin/env bash

if [[ -z $1 ]]; then
    echo "Usage: $0 <media.clip>" && exit
fi

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")"

MEDIACLIP="$1"
FACEMARKS="${HERE}/../share/shape_predictor_68_face_landmarks.dat"
MODEL="${HERE}/model.hdf5"
NORMALE="${HERE}/../share/normale.json"

DATAFILE="/tmp/neuon/clip.data"

/opt/neuon/bin/neuon-data -i "${MEDIACLIP}" -a 0 -o "${DATAFILE}" -l "${FACEMARKS}"
PYTHONUNBUFFERED=0 neuon-test -m "${MODEL}" -l "${DATAFILE}" -n "${NORMALE}"
