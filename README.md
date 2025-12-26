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
    Use with dynamic linking to decode media source and perform video framerate conversion and audio samplerate conversion in example application
      
 - Aquila(licensed under MIT) 
    Used for audio feature extraction. Original code was patched to provide MFEC feature alongside original MFCC
    
 - Dlib(Licensed under Boost 1.0 License and CC-0 for pretrained model)
    Library provides routines for face landmark detection with pre-trained model
    
 - Keras(Licensed under MIT) + other Python libraries(documentation is in progress)
  
##Implementation details

WIP   
 
##Known limitation

WIP   

## How to build
Refer to ci folder to resolve dependencies and install prerequisites. Then:

``` shell
cmake -DBUILD_SHARED_LIBS=On -DNEUON_PREFIX_PATH=${PWD}/shared -DCMAKE_POSITION_INDEPENDENT_CODE=On -DCMAKE_PREFIX_PATH=${PWD}/static:${PWD}/shared -DCMAKE_INSTALL_PREFIX="neuon" -DCPACK_SET_DESTDIR=On -DCMAKE_BUILD_TYPE=Release -Dversion=0.0.0.0 -Drevision=00000000 ..
```
