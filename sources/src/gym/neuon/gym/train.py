#!/usr/bin/env python

from __future__ import absolute_import
from __future__ import print_function
from keras.models import Sequential, Model
from keras.layers import *
from keras.layers.advanced_activations import *
from keras.optimizers import *

from optparse import OptionParser
import json
import os
import numpy
import keras.utils
import keras.callbacks

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot
matplotlib.pyplot.ioff()
import matplotlib.gridspec

import neuon.details.primitives
import neuon.details.preprocessing


def create_model():
    input_v = Input(shape=(9, 60, 100, 1))
    video_seq = Sequential(name='video')
    video_seq.add(Convolution3D(filters=16, kernel_size=(3, 3, 3),  input_shape=(9, 60, 100, 1), padding='valid', name='video-conv-1'))
    video_seq.add(PReLU())
    video_seq.add(MaxPooling3D(pool_size=(1, 3, 3), strides=(1, 2, 2), padding='valid', name='video-pool-1'))

    video_seq.add(Convolution3D(filters=32, kernel_size=(3, 3, 3), padding='valid', name='video-conv-2'))
    video_seq.add(PReLU())
    video_seq.add(MaxPooling3D(pool_size=(1, 3, 3), strides=(1, 2, 2), padding='valid', name='video-pool-2'))

    video_seq.add(Convolution3D(filters=64, kernel_size=(3, 3, 3), padding='valid', name='video-conv-3'))
    video_seq.add(PReLU())
    video_seq.add(MaxPooling3D(pool_size=(1, 3, 3), strides=(1, 2, 2), padding='valid', name='video-pool-3'))

    video_seq.add(Convolution3D(filters=128, kernel_size=(3, 3, 3),  padding='valid', name='video-conv-4'))
    video_seq.add(PReLU())
    video_seq.add(Flatten())

    video_seq.add(Dense(units=256, name='video-dense'))
    video_seq.add(PReLU())

    video_seq.add(Dense(units=64, name='video-output'))
    video_seq.add(ReLU())

    input_a = Input(shape=(3, 45, 15, 1))
    audio_seq = Sequential(name='audio')
    audio_seq.add(Convolution3D(filters=16, kernel_size=(3, 5, 3), input_shape=(3, 45, 15, 1), padding='valid', name='audio-conv-1'))
    audio_seq.add(PReLU())
    audio_seq.add(MaxPooling3D(pool_size=(1, 2, 1), strides=(1, 2, 1), padding='valid', name='audio-pool-1'))

    audio_seq.add(Convolution3D(filters=32, kernel_size=(1, 4, 3), padding='valid', name='audio-conv-2'))
    audio_seq.add(PReLU())

    audio_seq.add(Convolution3D(filters=32, kernel_size=(1, 4, 3), padding='valid', name='audio-conv-3'))
    audio_seq.add(PReLU())
    audio_seq.add(MaxPooling3D(pool_size=(1, 2, 1), strides=(1, 2, 1), padding='valid', name='audio-pool-2'))

    audio_seq.add(Convolution3D(filters=64, kernel_size=(1, 3, 3), padding='valid', name='audio-conv-4'))
    audio_seq.add(PReLU())

    audio_seq.add(Convolution3D(filters=64, kernel_size=(1, 3, 3), padding='valid', name='audio-conv-5'))
    audio_seq.add(PReLU())

    audio_seq.add(Convolution3D(filters=128, kernel_size=(1, 2, 3), padding='valid', name='audio-conv-6'))
    audio_seq.add(PReLU())
    audio_seq.add(Flatten())

    audio_seq.add(Dense(units=64,  name='audio-output'))
    audio_seq.add(ReLU())

    processed_v = video_seq(input_v)
    processed_a = audio_seq(input_a)

    distance = Lambda(neuon.details.primitives.euclidean_distance)([processed_v, processed_a])

    model = Model(inputs=[input_v, input_a], outputs=distance)
    return model


def evaluate(model, generator, expected, name):
    prediction = model.predict_generator(generator=generator)
    accuracy = neuon.details.primitives.compute_accuracy(expected, prediction)
    precision = neuon.details.primitives.compute_precision(expected, prediction)
    negative_predictive_value = neuon.details.primitives.compute_negative_predictive_value(expected, prediction)
    print('%s accuracy: %0.2f%% ( PPV %0.2f%%, NPV %0.2f%% )' % (name, 100 * accuracy, 100 * precision, 100 * negative_predictive_value))


def draw_data(features, name):

    for feature_idx, feature in enumerate(features):
        for sample_idx, sample in enumerate(feature):
            video_sample_dims = sample.shape[0]
            video_sample_channels = sample.shape[3]
            main_grid_size = int(np.ceil(np.sqrt(video_sample_channels)))
            main_grid = matplotlib.gridspec.GridSpec(main_grid_size, main_grid_size, wspace=0.1, hspace=0.1)

            figure = matplotlib.pyplot.figure(figsize=(32, 32))
            for channel_idx in range(video_sample_channels):
                video_grid_size = int(np.ceil(np.sqrt(video_sample_dims)))
                video_grid = matplotlib.gridspec.GridSpecFromSubplotSpec(video_grid_size, video_grid_size, subplot_spec=main_grid[channel_idx], wspace=0.0, hspace=0.0)
                for dim_idx in range(video_sample_dims):
                    ax = matplotlib.pyplot.Subplot(figure, video_grid[dim_idx])
                    ax.imshow(sample[dim_idx, :, :, channel_idx], cmap='gray')
                    ax.set_xticks([])
                    ax.set_yticks([])
                    figure.add_subplot(ax)

            figure.savefig('%s-sample-%02d-feature-%02d.png' % (name, sample_idx, feature_idx))
            matplotlib.pyplot.close(figure)


def main():
    os.environ["CUDA_VISIBLE_DEVICES"] = "0"
    parser = OptionParser()
    parser.add_option("-m", "--model", dest="model", help="model", metavar="FILE")
    parser.add_option("-n", "--normale", dest="normale", help="model", metavar="FILE")
    parser.add_option("-l", "--train-set", dest="train_set", help="data", metavar="FILE")
    parser.add_option("-v", "--validation-set", dest="validation_set", help="data", metavar="FILE")
    parser.add_option("-t", "--test-set", dest="test_set", help="data", metavar="FILE")
    parser.add_option("-i", "--info", dest="verbosity", help="data",  action="store_true", default=False)

    (options, args) = parser.parse_args()

    with open(options.normale, "r") as normale:
        statistic = json.load(normale)
        # video_scale = neuon.details.preprocessing.normalization_t(statistic['video']['min'], statistic['video']['max'])
        # audio_scale = neuon.details.preprocessing.normalization_t(statistic['audio']['min'], statistic['audio']['max'])
        video_scale = neuon.details.preprocessing.standardization_t(statistic['video']['mean'], statistic['video']['std'])
        audio_scale = neuon.details.preprocessing.standardization_t(statistic['audio']['mean'], statistic['audio']['std'])
        # video_scale = neuon.details.preprocessing.scale_t(statistic['video']['min'], statistic['video']['max'], statistic['video']['mean'], statistic['video']['std'])
        # audio_scale = neuon.details.preprocessing.scale_t(statistic['audio']['min'], statistic['audio']['max'], statistic['audio']['mean'], statistic['audio']['std'])

    batch_size = 32
    train_generator = neuon.details.primitives.train_generator_t(options.train_set, batch_size)
    train_generator.video_scale = video_scale
    train_generator.audio_scale = audio_scale

    validation_generator = neuon.details.primitives.train_generator_t(options.validation_set, batch_size)
    validation_generator.video_scale = video_scale
    validation_generator.audio_scale = audio_scale

    model = create_model()
    parallel_model = model

    parallel_model.compile(loss=neuon.details.primitives.contrastive_loss, optimizer=Adam(lr=0.0001), metrics=[neuon.details.primitives.accuracy])
    parallel_model.summary()

    callbacks = [
        keras.callbacks.EarlyStopping(monitor='val_loss', patience=2, verbose=0, restore_best_weights=True),
        keras.callbacks.ModelCheckpoint(options.model+".weights.{epoch:04d}-{val_loss:.2f}.hdf5", monitor='val_loss', save_best_only=True, verbose=1),
        keras.callbacks.TensorBoard(write_grads=True, write_images=True, update_freq='batch')
    ]

    history = parallel_model.fit_generator(generator=train_generator, validation_data=validation_generator, callbacks=callbacks, epochs=10, verbose=1)

    model.save(options.model)

    matplotlib.pyplot.rcParams["figure.figsize"] = [16, 9]
    matplotlib.pyplot.plot(history.history['accuracy'])
    matplotlib.pyplot.plot(history.history['val_accuracy'])
    matplotlib.pyplot.plot(history.history['loss'])
    matplotlib.pyplot.plot(history.history['val_loss'])
    matplotlib.pyplot.title('Neuon learning curve')
    matplotlib.pyplot.xlabel('epoch')
    matplotlib.pyplot.legend(['accuracy', 'validation_accuracy', 'loss', 'validation_loss'], loc='upper left')
    matplotlib.pyplot.savefig('history.png')

    train_generator = neuon.details.primitives.predict_generator_t(options.train_set, batch_size)
    train_generator.video_scale = video_scale
    train_generator.audio_scale = audio_scale

    validation_generator = neuon.details.primitives.predict_generator_t(options.validation_set, batch_size)
    validation_generator.video_scale = video_scale
    validation_generator.audio_scale = audio_scale

    test_generator = neuon.details.primitives.predict_generator_t(options.test_set, batch_size)
    test_generator.video_scale = video_scale
    test_generator.audio_scale = audio_scale

    [batch_video, batch_audio] = train_generator[0]
    video_input = model.layers[2].layers[0].input
    audio_input = model.layers[3].layers[0].input

    audio_convs = [model.layers[3].get_layer(name='audio-conv-1'),
                   model.layers[3].get_layer(name='audio-conv-2'),
                   model.layers[3].get_layer(name='audio-conv-3'),
                   model.layers[3].get_layer(name='audio-conv-4'),
                   model.layers[3].get_layer(name='audio-conv-5'),
                   model.layers[3].get_layer(name='audio-conv-6')
                   ]

    video_convs = [model.layers[2].get_layer(name='video-conv-1'),
                   model.layers[2].get_layer(name='video-conv-2'),
                   model.layers[2].get_layer(name='video-conv-3'),
                   model.layers[2].get_layer(name='video-conv-4'),
                   ]

    audio_features = [batch_audio] + [keras.backend.function([audio_input], [layer.output])([batch_audio])[0]
                                      for idx, layer in enumerate(audio_convs)
                                      ]

    video_features = [batch_video] + [keras.backend.function([video_input], [layer.output])([batch_video])[0]
                                      for idx, layer in enumerate(video_convs)]

    draw_data(video_features, "video")
    draw_data(audio_features, "audio")

    evaluate(parallel_model, train_generator, train_generator.distance[:], "train")
    evaluate(parallel_model, validation_generator, validation_generator.distance[:], "validation")
    evaluate(parallel_model, test_generator, test_generator.distance[:], "test")


if __name__ == '__main__':
    main()
