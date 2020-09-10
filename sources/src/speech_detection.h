#pragma once
#include "aquila/global.h"
#include "aquila/transform.h"
#include "aquila/source.h"

#include <vector>
#include <cassert>
#include <chrono>
#include <map>
#include <algorithm>

#include "dlib/array2d.h"

namespace neuon {
    class speech_detection_t {
    public:
        speech_detection_t(size_t sample_rate, const std::chrono::microseconds &window, const std::chrono::microseconds &overlap, size_t feature_count)
            : sample_rate(sample_rate)
            , window(window)
            , overlap(overlap)
            , feature_count(feature_count)
            , sample_count(std::chrono::duration_cast<std::chrono::seconds>(sample_rate * window).count())
            , released_samples_count(std::chrono::duration_cast<std::chrono::seconds>(sample_rate * (window - overlap)).count())
            , mfcc(sample_count) {

        }

        std::map<std::chrono::microseconds, dlib::array2d<double>> detect(const std::vector<double> &frame, const std::chrono::microseconds &pts) {
            auto expected_pts = cache.first + std::chrono::microseconds(1000000) * cache.second.size() / sample_rate;
            std::copy(frame.begin(), frame.end(), std::back_inserter(cache.second));

            const auto released_duration = window - overlap;

            std::map<std::chrono::microseconds, dlib::array2d<double>> timed_coeffs;
            while (cache.second.size() >= sample_count) {

                auto mfcc_set = mfcc.calculate(Aquila::SignalSource(cache.second.data(), sample_count, sample_rate), feature_count, true);

                dlib::array2d<double> a(1, mfcc_set.size());
                std::transform(mfcc_set.begin(), mfcc_set.end(), a.begin(), [](double v) {
                    double v_integer = v;
                    return v_integer;
                });
                timed_coeffs[cache.first] = std::move(a);

                cache.first += released_duration;
                cache.second.erase(cache.second.begin(), cache.second.begin() + released_samples_count);
            }

            return timed_coeffs;
        }

        const size_t sample_rate;

        const std::chrono::microseconds window;
        const std::chrono::microseconds overlap;
        const size_t feature_count;

        const size_t sample_count;
        const size_t released_samples_count;

        Aquila::Mfcc mfcc;
        std::pair<std::chrono::microseconds, std::vector<double>> cache;
    };
}
