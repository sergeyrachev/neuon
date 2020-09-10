#pragma once

#include <cstdlib>
#include <cstdint>
#include <chrono>

namespace neuon {

    static const size_t video_frames_per_entry = 9;
    static const size_t video_width_per_entry = 100;
    static const size_t video_height_per_entry = 60;

    static const size_t audio_samples_per_entry = 45;
    static const uint64_t fft_samples_per_entry = 64;

    static const size_t audio_features_per_entry = 15;
    static const size_t audio_derivation_per_entry = 3;

    static const std::chrono::microseconds audio_slice_overlap{0};
}
