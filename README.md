# NEUON
Neural Network for Lip Sync detection in video streaming 


##Concept

https://arxiv.org/pdf/1706.05739.pdf

https://www.robots.ox.ac.uk/~vgg/publications/2016/Chung16a/chung16a.pdf

##Dataset

Self-made accordingly The VoxCeleb1 Dataset description http://www.robots.ox.ac.uk/~vgg/data/voxceleb/vox1.html

Current folder layout(31.01.2019) differs from previous one when the download and clipping scripts were made. To use the latest dataset description script adaptation is needed.
   

## Used Components

 - youtube-dl
    To download video from Youtube by URL
    
 - FFMpeg(licenced under LGPL2.1 )
    Use with dynamic linking to decode media source and perform video framerate conversion and audio samplerate conversion
      
 - Aquila(licensed under MIT) 
    Used for audio feature extraction. Original code was patched to provide MFEC feature alongside original MFCC
    
 - Dlib(Licensed under Boost 1.0 License and CC-0 for pretrained model)
    Library provides routines for face landmark detection with pre-trained model
    
 - Keras(Licensed under MIT) + other Python libraries(documentation is in progress)
 
 
##Implementation

Full cycle to maintain a project:
 - Download video from Youtube(res/download.sh)
 - Clip the media file based on markers from VoxCelebs1 dataset description(res/clip.sh)
 - Process clips with neuon-data tool that extracts audio and video features(res/dataset.sh)
 - combine all features data samples to dataset list(cat + awk + paste.... WIP)
 - train network with datates(src/neuon-training.py)
 - verify network with other samples(src/neuon-test.py)
   
 
##Known limitation

 - File only
 - Resample audio to strange samplerate to fulfill Aquila implementation limitation: sample amount before FFT must be equal power of 
  but sample amount depends on video framerate to have aligned data sample size to use in neural network.
 - hard to deliver due to python dependencies and hardware requirements for training
    


## Windows Env

Prereq: python3, cmake, git, 7zip, wget, VS2019 Community for C++
```shell

b2 -j8 link=static threading=multi variant=release toolset=msvc-16.0 --build-type=complete runtime-link=shared --prefix=d:\work\env\boost\1.72\stage --build-dir=d:\work\env\boost\1.72\build --layout=tagged architecture=x86 address-model=64 install


```
