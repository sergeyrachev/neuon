#!/usr/bin/env python
from __future__ import absolute_import
from __future__ import print_function

import optparse
import numpy
import json
import os

import neuon.details.preprocessing
import neuon.details.primitives

import tensorflow as tf
from tensorflow.python.platform import gfile

def main():
    os.environ["CUDA_VISIBLE_DEVICES"] = "0"
    parser = optparse.OptionParser()
    parser.add_option("-m", "--model", dest="model", help="model", metavar="FILE")
    parser.add_option("-d", "--datalist", dest="datalist", help="data", metavar="FILE")
    parser.add_option("-n", "--normale", dest="normale", help="data", metavar="FILE")

    (options, args) = parser.parse_args()

    with open(options.normale, "r") as normale:
        statistic = json.load(normale)
        video_scale = neuon.details.preprocessing.standardization_t(statistic['video']['mean'], statistic['video']['std'])
        audio_scale = neuon.details.preprocessing.standardization_t(statistic['audio']['mean'], statistic['audio']['std'])

    try:
        datalist = neuon.details.primitives.datalist_t(options.datalist, video_scale, audio_scale)
        distance = datalist.distance
        timestamps = datalist.timestamps[:, 0]
    except ValueError as e:
        print("WARN: It seems no data samples were extracted from the file to use to make a prediction so we just exit.")
        print("WARN: Please contact to the author and share your movie clip to help with improving this product.")
        return

    with tf.Session(graph=tf.Graph()) as sess:
        with gfile.FastGFile(options.model, 'rb') as model:
            graph_def = tf.GraphDef()
            graph_def.ParseFromString(model.read())
            sess.graph.as_default()
            g_in = tf.import_graph_def(graph_def)

        print('===== output operation names =====\n')
        for op in sess.graph.get_operations():
            print(op)

        tensor_output = sess.graph.get_tensor_by_name('import/lambda_1/Sqrt:0')
        tensor_input_video = sess.graph.get_tensor_by_name('import/input_1:0')
        tensor_input_audio = sess.graph.get_tensor_by_name('import/input_2:0')

        predictions = sess.run(tensor_output, {tensor_input_video: datalist.data[0], tensor_input_audio: datalist.data[1]})

        for idx, (video_pts, predicted_distance) in enumerate(zip(timestamps, predictions)):
            print("Video sample #%d at %7d ms is %5.2f %% differs from audio. Is in sync? %s" % (idx, video_pts / 1000, predicted_distance * 100, predicted_distance < 0.5))

        te_acc = neuon.details.primitives.compute_accuracy(distance, predictions)
        print('* Match: %0.2f%%' % (100 * te_acc))

        result = numpy.mean(predictions.ravel()) < 0.5
        print("In average stream is in sync: %s " % result)


if __name__ == '__main__':
    main()


