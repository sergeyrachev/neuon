import numpy
import dask
import dask.array


def statistic_t(data):
    data = dask.array.from_array(data, chunks=data.chunks)

    mean = dask.array.mean(data)
    std = dask.array.std(data)
    minimum = dask.array.min(data)
    maximum = dask.array.max(data)

    return minimum.compute(), maximum.compute(), mean.compute(), std.compute()


class normalization_t:
    def __init__(self, minimum, maximum):
        self.minimum = minimum
        self.maximum = maximum

    def __call__(self, data):
        return numpy.nan_to_num((numpy.asanyarray(data, dtype='float') - self.minimum) / (self.maximum - self.minimum), copy=False)


class standardization_t:
    def __init__(self, mean, std):
        self.mean = mean
        self.std = std

    def __call__(self, data):
        return numpy.nan_to_num((numpy.asanyarray(data, dtype='float') - self.mean) / self.std, copy=False)


class scale_t:
    def __init__(self, minimum, maximum, mean, std):
        self.norma = normalization_t(minimum, maximum)

        scaled_mean = (mean - minimum) / (maximum - minimum)
        scaled_std = std / (maximum - minimum)

        self.standard = standardization_t(scaled_mean, scaled_std)

    def __call__(self, data):
        return self.standard(self.norma(data))
