import imageio
import numpy as np
import matplotlib.pyplot as plt
from optparse import OptionParser


def main():
    parser = OptionParser()
    parser.add_option("-f", "--file", dest="audiotxt",
                      help="audiotxt", metavar="FILE")

    (options, args) = parser.parse_args()

    audio = np.reshape(np.loadtxt(options.audiotxt), (-1, 15))

    plt.imsave(options.audiotxt + ".png", audio, cmap='gray')


if __name__ == '__main__':
    main()
