#!/usr/bin/env python
from __future__ import absolute_import
from __future__ import print_function

import keras.models
import optparse
import numpy
import json
import os

import neuon.details.primitives
import neuon.details.preprocessing


def main():
    os.environ["CUDA_VISIBLE_DEVICES"] = "0"
    parser = optparse.OptionParser()
    parser.add_option("-m", "--model", dest="model", help="model", metavar="FILE")
    parser.add_option("-d", "--dataset", dest="dataset", help="data", metavar="FILE")
    parser.add_option("-l", "--datalist", dest="datalist", help="data", metavar="FILE")
    parser.add_option("-n", "--normale", dest="normale", help="data", metavar="FILE")

    (options, args) = parser.parse_args()

    model = keras.models.load_model(options.model, custom_objects={'contrastive_loss': neuon.details.primitives.contrastive_loss, 'keras': keras}, compile=True)
    model.summary()

    with open(options.normale, "r") as normale:
        statistic = json.load(normale)
        video_scale = neuon.details.preprocessing.standardization_t(statistic['video']['mean'], statistic['video']['std'])
        audio_scale = neuon.details.preprocessing.standardization_t(statistic['audio']['mean'], statistic['audio']['std'])

    if options.dataset is not None:
        test_generator = neuon.details.primitives.predict_generator_t(options.dataset, 32)
        test_generator.video_scale = video_scale
        test_generator.audio_scale = audio_scale

        predictions = model.predict_generator(generator=test_generator, verbose=1)
        distance = test_generator.distance[:]
        timestamps = test_generator.timestamps[:, 0]
    else:
        try:
            datalist = neuon.details.primitives.datalist_t(options.datalist, video_scale, audio_scale)
            predictions = model.predict(datalist.data, verbose=1)
            distance = datalist.distance
            timestamps = datalist.timestamps[:, 0]
        except ValueError as e:
            print("WARN: It seems no data samples were extracted from the file to use to make a prediction so we just exit.")
            print("WARN: Please contact to the author and share your movie clip to help with improving this product.")
            return

    for idx, (video_pts, predicted_distance) in enumerate(zip(timestamps, predictions)):
        print("Video sample #%d at %7d ms is %5.2f %% differs from audio. Is in sync? %s" % (idx, video_pts / 1000, predicted_distance * 100, predicted_distance < 0.5))

    te_acc = neuon.details.primitives.compute_accuracy(distance, predictions)
    print('* Match: %0.2f%%' % (100 * te_acc))

    result = numpy.mean(predictions.ravel()) < 0.5
    print("In average stream is in sync: %s " % result)


if __name__ == '__main__':
    main()


