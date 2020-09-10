#include "tensorflow_static.h"

#include "tensorflow/c/c_api.h"

tensorflow::static_backend_t::static_backend_t() {

}

tensorflow::graph_ptr tensorflow::static_backend_t::TF_NewGraph() {
    return { ::TF_NewGraph(),::TF_DeleteGraph };
}

tensorflow::import_graph_def_options_ptr tensorflow::static_backend_t::TF_NewImportGraphDefOptions() {
    return {::TF_NewImportGraphDefOptions(), ::TF_DeleteImportGraphDefOptions};
}

tensorflow::session_options_ptr tensorflow::static_backend_t::TF_NewSessionOptions() {
    return {::TF_NewSessionOptions(), ::TF_DeleteSessionOptions};
}

tensorflow::status_ptr tensorflow::static_backend_t::TF_NewStatus() {
    return {::TF_NewStatus(), ::TF_DeleteStatus};
}

tensorflow::buffer_ptr tensorflow::static_backend_t::TF_NewBufferFromString(const void *proto, size_t proto_len) {
    return {::TF_NewBufferFromString(proto, proto_len), ::TF_DeleteBuffer};
}

tensorflow::session_ptr tensorflow::static_backend_t::TF_NewSession(tensorflow::graph_ptr &graph, const tensorflow::session_options_ptr &opts, tensorflow::status_ptr &status) {
    return {
        ::TF_NewSession(graph.get(), opts.get(), status.get()),
        [this](TF_Session* p){
            auto close_status = TF_NewStatus();
            TF_CloseSession(p, close_status.get());

            auto delete_status = TF_NewStatus();
            TF_DeleteSession(p, delete_status.get());
        }
    };
}

const char *tensorflow::static_backend_t::TF_Message(const tensorflow::status_ptr &s) {
    return ::TF_Message(s.get());
}

void tensorflow::static_backend_t::TF_GraphImportGraphDef(tensorflow::graph_ptr &graph, const tensorflow::buffer_ptr &graph_def, const tensorflow::import_graph_def_options_ptr &options, tensorflow::status_ptr &status) {
    ::TF_GraphImportGraphDef(graph.get(), graph_def.get(), options.get(), status.get());
}

TF_Operation *tensorflow::static_backend_t::TF_GraphOperationByName(tensorflow::graph_ptr &graph, const char *oper_name) {
    return ::TF_GraphOperationByName(graph.get(), oper_name);
}

tensorflow::tensor_ptr tensorflow::static_backend_t::TF_AllocateTensor(TF_DataType type, const int64_t *dims, int num_dims, size_t len) {
    return {
        ::TF_AllocateTensor(type, dims, num_dims, len),
        ::TF_DeleteTensor
    };
}

void tensorflow::static_backend_t::TF_SessionRun(
    tensorflow::session_ptr &session, const tensorflow::buffer_ptr &run_options,
    const TF_Output *inputs, const tensorflow::tensor_ptr *input_values, int ninputs,
    const TF_Output *outputs, tensorflow::tensor_ptr *output_values, int noutputs,
    const TF_Operation *const *target_opers, int ntargets,
    tensorflow::buffer_ptr &run_metadata, tensorflow::status_ptr &status) {

    const auto input_values_unsafe = unbox_ptr_collection(input_values, ninputs);

    TF_Tensor* output_collection = nullptr;
    auto output_values_unsafe = unbox_ptr_collection(output_values, noutputs);
    ::TF_SessionRun(session.get(),run_options.get(),
                    inputs, input_values_unsafe.data(), input_values_unsafe.size(),
                    outputs, &output_collection, noutputs,
                    target_opers, ntargets,
                    run_metadata.get(), status.get());

    output_values->reset(output_collection);
}

void *tensorflow::static_backend_t::TF_TensorData(const tensorflow::tensor_ptr & tensor) {
    return ::TF_TensorData(tensor.get());
}

size_t tensorflow::static_backend_t::TF_TensorByteSize(const tensorflow::tensor_ptr & tensor) {
    return ::TF_TensorByteSize(tensor.get());
}

int64_t tensorflow::static_backend_t::TF_TensorElementCount(const tensorflow::tensor_ptr &tensor) {
    return ::TF_TensorElementCount(tensor.get());
}

const char *tensorflow::static_backend_t::TF_OperationName(TF_Operation *oper) {
    return ::TF_OperationName(oper);
}

TF_Operation* tensorflow::static_backend_t::TF_GraphNextOperation(graph_ptr& graph, size_t* pos) {
    return ::TF_GraphNextOperation(graph.get(), pos);
}

TF_Code tensorflow::static_backend_t::TF_GetCode(tensorflow::status_ptr& s) {
    return ::TF_GetCode(s.get());
}

const char *tensorflow::static_backend_t::TF_Message(tensorflow::status_ptr& s) {
    return ::TF_Message(s.get());
}


