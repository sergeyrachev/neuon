#pragma once

#include <map>
#include <functional>
#include <chrono>
#include <iostream>
#include <vector>
#include <algorithm>

#include "dlib/array2d.h"
#include "dlib/array.h"
#include "dlib/image_processing.h"

namespace neuon {

    class composer_t {

        template<typename T>
        T clamp(T v, T min, T max) {
            if (v < min) return min;
            if (v > max) return max;
            return v;
        }

    public:
        using handler_t = std::function<
            void(const dlib::matrix<uint8_t> &,
                 const std::chrono::microseconds &,
                 const std::chrono::microseconds &,
                 const dlib::matrix<double> &,
                 const std::chrono::microseconds &,
                 const std::chrono::microseconds &
            )>;

        const size_t frames;
        const size_t samples;

        composer_t(size_t frames, size_t samples, const handler_t &handler)
            : frames(frames)
            , samples(samples)
            , handler(handler) {

        }

        void add_video(const std::chrono::microseconds &pts, dlib::array2d<uint8_t> &&feature) {

            assert(pts.count() >= 0);

            if (!feature.size()) {
                flush(pts, audio_features);
                flush(pts, video_features);
                return;
            }

            video_features.emplace(pts, std::move(feature));
            check_completeness(video_features, audio_features);
        }

        void add_audio(const std::chrono::microseconds &pts, dlib::array2d<double> &&feature) {

            assert(pts.count() >= 0);
            audio_features.emplace(pts, std::move(feature));
            check_completeness(video_features, audio_features);
        }

        void check_completeness(
            std::map<std::chrono::microseconds, dlib::array2d<uint8_t>> &video_features,
            std::map<std::chrono::microseconds, dlib::array2d<double>> &audio_features) {
            if (video_features.size() <= frames) {
                return;
            }

            auto first_video_pts = video_features.begin()->first;
            flush(first_video_pts, audio_features);

            if (audio_features.size() <= samples) {
                return;
            }

            auto video_last = video_features.begin();
            std::advance(video_last, frames);

            if (video_last->first >= audio_features.crbegin()->first) {
                return;
            }

            auto video_it = video_features.begin();
            dlib::matrix<uint8_t> video_set;
            for (auto j = 0; j < frames; ++j) {
                video_set = dlib::join_cols(video_set, dlib::mat(video_it->second));
                ++video_it;
            }
            auto last_video_pts = video_it->first;

            dlib::matrix<double> audio_set;
            auto audio_it = audio_features.begin();
            auto first_audio_pts = audio_it->first;

            size_t i = 0;
            for (; i < samples; ++audio_it, i++) {
                dlib::matrix<double> sample = dlib::mat(audio_it->second);
                audio_set = dlib::join_cols(audio_set, sample);
            }
            auto last_audio_pts = audio_it->first;

            dlib::matrix<double> audio_delta_set;
            for (int64_t j = 0; j < audio_set.nr(); ++j) {

                int64_t prev_idx = clamp<int64_t>(j - 1, 0, audio_set.nr() - 1);
                int64_t next_idx = clamp<int64_t>(j + 1, 0, audio_set.nr() - 1);

                dlib::matrix<double> delta = (dlib::rowm(audio_set, next_idx) - dlib::rowm(audio_set, prev_idx)) / 2;
                audio_delta_set = dlib::join_cols(audio_delta_set, delta);
            }

            dlib::matrix<double> audio_deltadelta_set;
            for (int64_t j = 0; j < audio_delta_set.nr(); ++j) {

                int64_t prev_idx = clamp<int64_t>(j - 1, 0, audio_delta_set.nr() - 1);
                int64_t next_idx = clamp<int64_t>(j + 1, 0, audio_delta_set.nr() - 1);

                dlib::matrix<double> delta = (dlib::rowm(audio_delta_set, next_idx) - dlib::rowm(audio_delta_set, prev_idx)) / 2;
                audio_deltadelta_set = dlib::join_cols(audio_deltadelta_set, delta);
            }

            audio_set = dlib::join_cols(audio_set, audio_delta_set);
            audio_set = dlib::join_cols(audio_set, audio_deltadelta_set);

            handler(video_set, first_video_pts, last_video_pts, audio_set, first_audio_pts, last_audio_pts);

            flush(last_video_pts, video_features);
            flush(last_video_pts, audio_features);
        }

        template<typename T>
        void flush(const std::chrono::microseconds &pts, std::map<std::chrono::microseconds, T> &features) {
            while (features.begin() != features.end() && features.begin()->first < pts) {
                features.erase(features.begin());
            }
        }

    private:
        handler_t handler;
        std::map<std::chrono::microseconds, dlib::array2d<uint8_t>> video_features;
        std::map<std::chrono::microseconds, dlib::array2d<double>> audio_features;
    };
}
