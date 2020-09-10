#include "neuon_cxx.h"

#include "face_detection.h"
#include "speech_detection.h"
#include "composer.h"
#include "configuration.h"

neuon::neuon_t::neuon_t(std::unique_ptr<neuon::face_detection_t> face_detection, std::unique_ptr<neuon::speech_detection_t> speech_detection, std::unique_ptr<neuon::composer_t> composer)
    : face_detection(std::move(face_detection))
    , speech_detection(std::move(speech_detection))
    , composer(std::move(composer)) {

}
neuon::neuon_t::~neuon_t() = default;

void neuon::neuon_t::put(const neuon::audio_sample_t &sample) {
    std::vector<double> frame(sample.samples, 0.0);
    for (int i = 0; i < sample.samples; ++i) {
        const size_t bytes_per_sample = sizeof(double);
        frame[i] = *reinterpret_cast<const double *>(&sample.p[i * bytes_per_sample]);
    }

    auto speech_fragments = speech_detection->detect(frame, sample.pts);

    for (auto &fragment : speech_fragments) {
        auto &timestamp = fragment.first;
        auto &speech = fragment.second;
        composer->add_audio(timestamp, std::move(speech));
    }
}

void neuon::neuon_t::put(const neuon::video_sample_t &sample) {
    dlib::array2d<uint8_t> picture(sample.height, sample.width);
    for (int i = 0; i < sample.height; ++i) {
        for (int j = 0; j < sample.width; ++j) {
            picture[i][j] = sample.p[i * sample.stride + j];
        }
    }

    auto mouth = face_detection->detect(picture);
    composer->add_video(sample.pts, std::move(mouth));
}

uint64_t neuon::target_audio_samplerate(const std::chrono::microseconds& video_frame_duration) {
    const std::chrono::microseconds audio_slice_estimation(video_frame_duration * neuon::video_frames_per_entry / neuon::audio_samples_per_entry);
    const uint64_t target_audio_samplerate = std::chrono::microseconds::period::den * neuon::fft_samples_per_entry / audio_slice_estimation.count();
    return target_audio_samplerate;
}
