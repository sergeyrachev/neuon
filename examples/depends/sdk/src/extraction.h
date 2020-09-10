#pragma once

#include "source.h"
#include "resample.h"

#include <chrono>

class extraction_t{
public:

    using on_resample_callback_t = std::function<void(ffmpeg::frame_ptr, const std::chrono::microseconds&)>;

public:
    extraction_t(const std::string& media_filename, size_t target_video_height, size_t target_audio_samplerate, on_resample_callback_t video, on_resample_callback_t audio);
public:
    std::unique_ptr<source_t> src;

private:
    std::unique_ptr<audio_resample_t> aresample;
    std::unique_ptr<video_resample_t> vresample;
};

std::chrono::microseconds video_frame_duration(const std::string &media_filename);


