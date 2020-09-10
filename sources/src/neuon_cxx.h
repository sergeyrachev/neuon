#pragma once

#include <memory>
#include <vector>
#include <fstream>
#include <chrono>

namespace neuon{
    class face_detection_t;
    class speech_detection_t;
    class composer_t;
    class model_t;
}

namespace neuon {
    struct video_sample_t{
        const uint8_t *p;
        size_t bytes;
        size_t width;
        size_t height;
        size_t stride;
        std::chrono::microseconds pts;
    };

    struct audio_sample_t{
        const uint8_t *p;
        size_t bytes;
        size_t samples;
        std::chrono::microseconds pts;
    };

    class neuon_t {
    public:
        neuon_t(std::unique_ptr<neuon::face_detection_t> face_detection, std::unique_ptr<neuon::speech_detection_t> speech_detection, std::unique_ptr<neuon::composer_t> composer);
        ~neuon_t();
        void put(const audio_sample_t& sample);
        void put(const video_sample_t& sample);

    protected:
        std::unique_ptr<neuon::face_detection_t> face_detection;
        std::unique_ptr<neuon::speech_detection_t> speech_detection;
        std::unique_ptr<neuon::composer_t> composer;
    };

    uint64_t target_audio_samplerate(const std::chrono::microseconds& video_frame_duration);
}
