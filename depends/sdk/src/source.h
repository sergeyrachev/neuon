#pragma once

#include "demuxer.h"
#include "decoder.h"
#include "frame_ptr.h"

#include <string>
#include <functional>
#include <memory>
#include <map>
#include <list>

typedef std::function<void(ffmpeg::frame_ptr)> consumer;

class source_t {
public:
    explicit source_t(const std::string& input, consumer video, consumer audio);
    void run(threads::interruption_t& interruption);
    ~source_t();
    std::unique_ptr<ffmpeg::demuxer> dmx;
    std::shared_ptr<ffmpeg::decoder> adec;
    std::shared_ptr<ffmpeg::decoder> vdec;

private:
    void setup(ffmpeg::demuxer& dmx, const consumer &video, const consumer &audio);

    std::map<int32_t, std::shared_ptr<ffmpeg::decoder>> decoders;
    std::map<int32_t, consumer> consumers;
};

