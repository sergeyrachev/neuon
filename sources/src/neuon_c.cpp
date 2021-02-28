#include "neuon/neuon_c.h"

#include "neuon_cxx.h"
#include "configuration.h"

#include "composer.h"
#include "speech_detection.h"
#include "face_detection.h"
#include "model.h"
#include "tensorflow_api.h"
#include "tensorflow_static.h"
#include "license.h"
#include "demo.h"
#include "log.h"
#include "birthday.h"

#include <boost/filesystem.hpp>

#include "dlib/image_processing.h"

#include <json/json.h>

struct neuon_context_t{
    std::unique_ptr<neuon::neuon_t> engine;
};

uint64_t neuon_estimate_target_audio_samplerate(neuon_microseconds_t video_frame_duration) {
    return neuon::target_audio_samplerate(std::chrono::microseconds(video_frame_duration));
}

neuon_context_t *neuon_create_engine(const char *model_filename, const char *norma_filename, const char *landmark_filepath, const char *license_filename, const neuon_options_t *options, neuon_logging_t* app_log, neuon_user_data_t *user_data) {
    if(!app_log){
        return nullptr;
    }

    auto log = neuon::log_t(app_log);

    log.debug() << "Try to open license file if available...";

    std::string license_payload;
    if (license_filename) {
        std::ifstream license_file(license_filename);
        if(license_file){
            std::getline(license_file, license_payload);
        }
    }

    log.debug() << "Try to read license information...";
    common::license_t license(license_payload);
    if (license.demo()) {
        common::demo_t demo(neuon::birthday);
        if(demo.has_expired){
            log.error() << "Demo time period limit exceeded! Please contact the seller.";
            return nullptr;
        }
    }

    log.debug() << "Try to open neural network model...";
    std::ifstream model_file(model_filename, std::ios::binary);
    if(!model_file){
        log.error() << "Neural network model file is not available. Check the path and permissions for " << model_filename;
        return nullptr;
    }

    log.debug() << "Try to open face landmark database...";
    std::ifstream landmark_file(landmark_filepath, std::ios::binary);
    if (!landmark_file) {
        log.error() << "Face landmark database file is not available. Check the path and permissions for " << landmark_filepath;
        return nullptr;
    }

    log.debug() << "Try to open normalization parameters...";
    std::ifstream norma_file(norma_filename, std::ios::binary);
    if (!norma_file) {
        log.error() << "Normalization parameters file is not available. Check the path and permissions for " << norma_filename;
        return nullptr;
    }

    log.debug() << "Read normalization parameters...";
    Json::Value root;
    norma_file >> root;

    neuon::normalization_t normalization{};
    normalization.audio.min = root["audio"]["min"].asDouble();
    normalization.audio.max = root["audio"]["max"].asDouble();
    normalization.audio.mean = root["audio"]["mean"].asDouble();
    normalization.audio.std = root["audio"]["std"].asDouble();

    normalization.video.min = root["video"]["min"].asDouble();
    normalization.video.max = root["video"]["max"].asDouble();
    normalization.video.mean = root["video"]["mean"].asDouble();
    normalization.video.std = root["video"]["std"].asDouble();

    log.debug() << "Face detection engine initialization...";
    std::unique_ptr<neuon::face_detection_t> face_detection(new neuon::face_detection_t(neuon::video_width_per_entry, neuon::video_height_per_entry, landmark_file));

    log.debug() << "Speech detection engine initialization...";
    const std::chrono::microseconds audio_slice(static_cast<uint64_t>(std::round(neuon::fft_samples_per_entry * 1000000.0 / options->target_audio_samplerate)));
    std::unique_ptr<neuon::speech_detection_t> speech_detection(new neuon::speech_detection_t(options->target_audio_samplerate, audio_slice, neuon::audio_slice_overlap, neuon::audio_features_per_entry));

    if(speech_detection->sample_count != neuon::fft_samples_per_entry){
        log.error() << "Engine could not initializate speech detection engine properly due to rounding error. Report an issue and provide the media file causing the issue. ";
        return nullptr;
    }

    log.debug() << "Read neural network model...";
    std::vector<uint8_t> model_payload(boost::filesystem::file_size(model_filename));
    model_file.read(reinterpret_cast<char *>(model_payload.data()), model_payload.size());

    log.debug() << "Load neural network model...";
    std::shared_ptr<neuon::model_t> model = std::make_shared<neuon::model_t>(std::make_shared<tensorflow::static_backend_t>(), model_payload, normalization);

    log.debug() << "Initialize A/V Sync Engine...";
    std::unique_ptr<neuon::composer_t> composer(
        new neuon::composer_t(
            neuon::video_frames_per_entry,
            neuon::audio_samples_per_entry,
            [model, user_data, log](
                const dlib::matrix<uint8_t> &video_set,
                const std::chrono::microseconds &first_video_pts,
                const std::chrono::microseconds &last_video_pts,
                const dlib::matrix<double> &audio_set,
                const std::chrono::microseconds &first_audio_pts,
                const std::chrono::microseconds last_audio_pts) {

                    log.debug() << "The next data sample is evaluated. Video PTS: " << first_video_pts.count() << " - "<< last_video_pts.count() << "Audio PTS : " << first_audio_pts.count() << " - "<< last_audio_pts.count();
                    auto prediction = model->predict(video_set, {1, neuon::video_frames_per_entry, neuon::video_height_per_entry, neuon::video_width_per_entry, 1}, audio_set, {1, neuon::audio_derivation_per_entry, neuon::audio_samples_per_entry, neuon::audio_features_per_entry, 1});
                    neuon_outcome_t outcome{neuon_microseconds_t(first_video_pts.count()), prediction[0]};
                    user_data->on_result(user_data, &outcome);
                }
            )
        );

    log.debug() << "Engine content initialization.";
    return new neuon_context_t{std::unique_ptr<neuon::neuon_t>{new neuon::neuon_t{std::move(face_detection), std::move(speech_detection), std::move(composer)}}};
}

void neuon_free_engine(neuon_context_t * context) {
    delete context;
}

void neuon_put_video(const neuon_context_t *context, const uint8_t *p, size_t bytes, size_t width, size_t height, size_t stride, neuon_microseconds_t pts) {
    neuon::video_sample_t sample{p, bytes, width, height, stride, std::chrono::microseconds(pts)};
    context->engine->put(sample);
}

void neuon_put_audio(const neuon_context_t *context, const uint8_t *p, size_t bytes, size_t samples, neuon_microseconds_t pts) {
    neuon::audio_sample_t sample{p, bytes, samples, std::chrono::microseconds(pts)};
    context->engine->put(sample);
}
