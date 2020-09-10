#pragma once

#include <functional>
#include <memory>

struct AVFrame;

namespace ffmpeg{
    typedef std::function<void(AVFrame *)> frame_deleter_t;
    typedef std::unique_ptr<AVFrame, frame_deleter_t> frame_ptr;
}
