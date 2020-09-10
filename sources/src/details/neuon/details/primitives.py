import keras.backend
import keras.utils
import numpy
import h5py
import pandas
import imageio


def euclidean_distance(vects):
    x, y = vects
    return keras.backend.sqrt(keras.backend.maximum(keras.backend.sum(keras.backend.square(x - y), axis=1, keepdims=True), keras.backend.epsilon()))


def contrastive_loss(y_true, y_pred ):
    # http://yann.lecun.com/exdb/publis/pdf/hadsell-chopra-lecun-06.pdf
    margin = 1
    mean = keras.backend.mean((1 - y_true) * keras.backend.square(y_pred) + y_true * keras.backend.square(keras.backend.maximum(margin - y_pred, 0)))
    return mean


def accuracy(y_true, y_pred):
    pred = keras.backend.cast(y_pred > 0.5, y_true.dtype)
    matched = keras.backend.equal(y_true, pred)
    return keras.backend.mean(matched)


def compute_accuracy(y_true, y_pred):
    pred = y_pred.ravel() > 0.5
    matched = pred == y_true
    return numpy.mean(matched)


def compute_precision(y_true, y_pred):
    pred = y_pred.ravel() > 0.5
    est = y_true[pred]
    return est.mean()


def compute_negative_predictive_value(y_true, y_pred):
    pred = y_pred.ravel() < 0.5
    est = 1 - y_true[pred]
    return est.mean()


class datalist_t(object):
    def __init__(self, path, video_scale, audio_scale):
        video = []
        audio = []
        self.timestamps = []
        self.distance = []

        datalist = pandas.read_csv(path, header=None, delim_whitespace=True, low_memory=False)
        for index, distance_value, picture_filename, mfcc_filename, *rest in datalist.itertuples():
            picture = imageio.imread(picture_filename)
            mfcc = numpy.loadtxt(mfcc_filename)

            v = numpy.reshape(picture, (-1, 60, 100, 1))
            a = numpy.reshape(mfcc, (-1, 45, 15, 1), )

            video += [v]
            audio += [a]
            self.timestamps += [rest]
            self.distance += [distance_value]

        self.timestamps = numpy.asanyarray(self.timestamps)
        self.distance = numpy.asanyarray(self.distance)
        self.data = [video_scale(video), audio_scale(audio)]



class predict_generator_t(keras.utils.Sequence):
    def __init__(self, path, batch_size):
        self.dataset = h5py.File(path, mode='r')
        self.video = self.dataset['video']
        self.audio = self.dataset['audio']
        self.distance = self.dataset['distance']
        self.timestamps = self.dataset['timestamps']
        self.batch_size = batch_size
        self.video_scale = lambda data: data
        self.audio_scale = lambda data: data

    def __len__(self):
        return int(numpy.ceil(len(self.video) / float(self.batch_size)))

    def __getitem__(self, idx):
        from_idx = idx * self.batch_size
        to_idx = (idx + 1) * self.batch_size

        batch_x_video = self.video_scale(self.video[from_idx:to_idx])
        batch_x_audio = self.audio_scale(self.audio[from_idx:to_idx])

        return [batch_x_video, batch_x_audio]


class train_generator_t(keras.utils.Sequence):
    def __init__(self, path, batch_size):
        self.dataset = h5py.File(path, mode='r')
        self.video = self.dataset['video']
        self.audio = self.dataset['audio']
        self.timestamps = self.dataset['timestamps']
        self.distance = self.dataset['distance']
        self.batch_size = batch_size
        self.video_scale = lambda data: numpy.asanyarray(data, dtype='float')
        self.audio_scale = lambda data: numpy.asanyarray(data, dtype='float')

    def __len__(self):
        return int(numpy.ceil(len(self.video) / float(self.batch_size)))

    def __getitem__(self, idx):
        from_idx = idx * self.batch_size
        to_idx = (idx + 1) * self.batch_size

        batch_x_video = self.video_scale(self.video[from_idx:to_idx])
        batch_x_audio = self.audio_scale(self.audio[from_idx:to_idx])

        batch_x_audio_shifted = numpy.roll(batch_x_audio, shift=int(self.batch_size / 2), axis=0)

        batch_y = numpy.asanyarray([0]*len(batch_x_video) + [1]*len(batch_x_video), dtype='float')

        return [numpy.append(batch_x_video, batch_x_video, axis=0), numpy.append(batch_x_audio, batch_x_audio_shifted, axis=0)], batch_y


if __name__ == '__main__':
    pass

