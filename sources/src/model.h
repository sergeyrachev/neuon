#pragma once

#include "tensorflow_api.h"

#include "dlib/matrix.h"
#include <string>
#include <vector>
#include <memory>

namespace neuon{
    struct characteristics_t {
        double min;
        double max;
        double mean;
        double std;
    };

    struct normalization_t {
        characteristics_t audio;
        characteristics_t video;
    };

    class model_t{
    public:
        explicit model_t(std::shared_ptr<tensorflow::api_t> api, const std::vector<uint8_t>& model_payload, const normalization_t& normalization);
        ~model_t();

        std::vector<float> predict(const dlib::matrix<uint8_t> &x_video, const std::vector<int64_t>& video_dims, const dlib::matrix<double> &x_audio, const std::vector<int64_t>& audio_dims );

    private:
        std::shared_ptr<tensorflow::api_t> api;
        tensorflow::graph_ptr  graph;
        normalization_t normalization;

        tensorflow::session_ptr session;
    };
}

