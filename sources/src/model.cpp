// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "model.h"

#include <boost/numeric/ublas/tensor.hpp>

using neuon::model_t;

model_t::model_t(std::shared_ptr<tensorflow::api_t> api, const std::vector<uint8_t>& model_payload, const normalization_t& normalization )
: api(api)
, graph(api->TF_NewGraph())
, normalization(normalization){

    tensorflow::buffer_ptr buf(api->TF_NewBufferFromString(model_payload.data(), model_payload.size()));

    tensorflow::import_graph_def_options_ptr opts( api->TF_NewImportGraphDefOptions());

    tensorflow::status_ptr status( api->TF_NewStatus());

    api->TF_GraphImportGraphDef(graph, buf, opts, status);

    tensorflow::session_options_ptr session_opts(api->TF_NewSessionOptions());

    session = api->TF_NewSession(graph, session_opts, status);
}

model_t::~model_t() = default;

std::vector<float> model_t::predict(const dlib::matrix<uint8_t> &x_video, const std::vector<int64_t>& video_dims, const dlib::matrix<double> &x_audio, const std::vector<int64_t>& audio_dims ) {
    static const std::string input_layer_video_name = "input_1";
    static const std::string input_layer_audio_name = "input_2";
    static const std::string output_layer_name = "lambda_1/Sqrt";

    std::vector<TF_Output> inputs{{api->TF_GraphOperationByName(graph, input_layer_video_name.c_str()), 0}, {api->TF_GraphOperationByName(graph, input_layer_audio_name.c_str()), 0}};
    TF_Output output = {api->TF_GraphOperationByName(graph, output_layer_name.c_str()), 0};

    auto video_tensor = api->TF_AllocateTensor(TF_FLOAT, video_dims.data(), static_cast<int>(video_dims.size()), x_video.size() *  sizeof(float));
    boost::numeric::ublas::tensor<float> r_video(boost::numeric::ublas::shape({static_cast<size_t>(x_video.nr()), static_cast<size_t>(x_video.nc())}));
    std::copy(x_video.begin(), x_video.end(), r_video.begin());
    r_video.reshape({static_cast<size_t>(video_dims[3]),  static_cast<size_t>(video_dims[2]), static_cast<size_t>(video_dims[1]) });
    r_video -= normalization.video.mean;
    r_video /= normalization.video.std;

    std::copy(r_video.begin(), r_video.end(), reinterpret_cast<float*>(api->TF_TensorData(video_tensor)));

    auto audio_tensor = api->TF_AllocateTensor(TF_FLOAT, audio_dims.data(), static_cast<int>(audio_dims.size()), x_audio.size() *  sizeof(float));
    boost::numeric::ublas::tensor<float> r_audio(boost::numeric::ublas::shape({static_cast<size_t>(x_audio.nr()), static_cast<size_t>(x_audio.nc())}));
    std::copy(x_audio.begin(), x_audio.end(), r_audio.begin());
    r_audio.reshape({static_cast<size_t>(audio_dims[3]), static_cast<size_t>(audio_dims[2]), static_cast<size_t>(audio_dims[1])});
    r_audio -= normalization.audio.mean;
    r_audio /= normalization.audio.std;

    std::copy(r_audio.begin(), r_audio.end(), reinterpret_cast<float*>(api->TF_TensorData(audio_tensor)));

    std::vector<tensorflow::tensor_ptr> input_tensors;
    input_tensors.emplace_back(std::move(video_tensor));
    input_tensors.emplace_back(std::move(audio_tensor));

    std::vector<int64_t> dims(1, 1);
    tensorflow:: tensor_ptr output_tensor = api->TF_AllocateTensor(TF_FLOAT, dims.data(), dims.size(), sizeof(float));
    *reinterpret_cast<float*>(api->TF_TensorData(output_tensor)) = 0.0f;

    tensorflow::status_ptr status( api->TF_NewStatus() );
    tensorflow::buffer_ptr run_metadata;
    api->TF_SessionRun(session, nullptr,
                       inputs.data(), input_tensors.data(), inputs.size(),
                       &output, &output_tensor, 1,
                       nullptr, 0,
                       run_metadata,
                       status);

    std::vector<float> prediction(reinterpret_cast<float*>(api->TF_TensorData(output_tensor)), reinterpret_cast<float*>(api->TF_TensorData(output_tensor)) + api->TF_TensorElementCount(output_tensor));
    return prediction;
}



