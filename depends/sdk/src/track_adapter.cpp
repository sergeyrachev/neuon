// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "track_adapter.h"


ffmpeg::track_adapter::track_adapter(const AVStream &stream)
    : _parameters(avcodec_parameters_alloc(), [](AVCodecParameters* p){ avcodec_parameters_free(&p);})
    , index(static_cast<size_t>(stream.index))
    , parameters(*_parameters)
{
    avcodec_parameters_copy(_parameters.get(), stream.codecpar);
}
