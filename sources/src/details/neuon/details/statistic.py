#!/usr/bin/env python

import optparse
import h5py
import json
import neuon.details.preprocessing


def main():
    parser = optparse.OptionParser()
    parser.add_option("-d", "--dataset", dest="dataset", help="HDF5 Dataset storage file", metavar="FILE")
    parser.add_option("-n", "--normale", dest="normale", help="HDF5 Dataset storage file", metavar="FILE")

    (options, args) = parser.parse_args()

    with h5py.File(options.dataset, mode='r') as dataset:
        video_dataset = dataset['video']
        audio_dataset = dataset['audio']

        min_video, max_video, mean_video, std_video = neuon.details.preprocessing.statistic_t(video_dataset)
        min_audio, max_audio, mean_audio, std_audio = neuon.details.preprocessing.statistic_t(audio_dataset)

        with open(options.normale, "w") as normale:
            json.dump({'audio': {'min': float(min_audio),
                                 'max': float(max_audio),
                                 'mean': float(mean_audio),
                                 'std': float(std_audio)
                                 },
                       'video': {'min': float(min_video),
                                 'max': float(max_video),
                                 'mean': float(mean_video),
                                 'std': float(std_video)
                                 }
                       }, normale)


if __name__ == '__main__':
    main()
