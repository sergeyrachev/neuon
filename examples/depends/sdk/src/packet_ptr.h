#pragma once

#include <functional>
#include <memory>

struct AVPacket;

namespace ffmpeg{
    typedef std::function<void(AVPacket * )> packet_deleter_t;
    typedef std::unique_ptr<AVPacket, packet_deleter_t> packet_ptr;
}
