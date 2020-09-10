#include "tensorflow_dynamic.h"

#include "tensorflow/c/c_api.h"

#include <boost/filesystem.hpp>
#include <boost/dll.hpp>

#include <utility>
#include <functional>

#define tf_import(library, NAME) library->get<decltype(::NAME)>(std::string(#NAME))

tensorflow::dynamic_backend_t::dynamic_backend_t(const std::string &shared_lib_path)
    : library(std::make_shared<boost::dll::shared_library>(shared_lib_path, boost::dll::load_mode::append_decorations | boost::dll::load_mode::search_system_folders)){

}

tensorflow::graph_ptr tensorflow::dynamic_backend_t::TF_NewGraph() {
    auto tf_new_graph = tf_import(library, TF_NewGraph);
    return {
        tf_new_graph(),
        [this](TF_Graph* p){
            auto tf_delete_graph = tf_import(this->library, TF_DeleteGraph);
            tf_delete_graph(p);
        }
    };
}

tensorflow::import_graph_def_options_ptr tensorflow::dynamic_backend_t::TF_NewImportGraphDefOptions() {
    auto tf_new_import_graph_def_options = tf_import(library, TF_NewImportGraphDefOptions);
    return {
        tf_new_import_graph_def_options(),
        [this](TF_ImportGraphDefOptions* p){
            auto tf_delete_import_graph_def_options = tf_import(library, TF_DeleteImportGraphDefOptions);
            tf_delete_import_graph_def_options(p);
        }
    };
}

tensorflow::session_options_ptr tensorflow::dynamic_backend_t::TF_NewSessionOptions() {
    auto tf_new_session_options = tf_import(library, TF_NewSessionOptions);
    return {
        tf_new_session_options(),
        [this](TF_SessionOptions* p){
            auto tf_delete_session_options = tf_import(library, TF_DeleteSessionOptions);
            tf_delete_session_options(p);
        }
    };
}

tensorflow::status_ptr tensorflow::dynamic_backend_t::TF_NewStatus() {
    auto tf_new_status = tf_import(library, TF_NewStatus);
    return {
        tf_new_status(),
        [this](TF_Status* p){
            auto tf_delete_status = tf_import(library, TF_DeleteStatus);
            tf_delete_status(p);
        }
    };
}

tensorflow::buffer_ptr tensorflow::dynamic_backend_t::TF_NewBufferFromString(const void *proto, size_t proto_len) {
    auto tf_new_buffer_from_string = tf_import(library, TF_NewBufferFromString);
    return {
        tf_new_buffer_from_string(proto, proto_len),
        [this](TF_Buffer* p){
            auto tf_delete_buffer = tf_import(library, TF_DeleteBuffer);
            tf_delete_buffer(p);
        }
    };
}

tensorflow::session_ptr tensorflow::dynamic_backend_t::TF_NewSession(tensorflow::graph_ptr &graph, const tensorflow::session_options_ptr &opts, tensorflow::status_ptr &status) {
    auto tf_new_session = tf_import(library, TF_NewSession);
    return {
        tf_new_session(graph.get(), opts.get(), status.get()),
        [this](TF_Session* p){
            auto tf_close_session = tf_import(library, TF_CloseSession);
            auto close_status = TF_NewStatus();
            tf_close_session(p, close_status.get());

            auto tf_delete_session = tf_import(library, TF_DeleteSession);
            auto delete_status = TF_NewStatus();
            tf_delete_session(p, delete_status.get());
        }
    };
}

const char *tensorflow::dynamic_backend_t::TF_Message(const tensorflow::status_ptr &s) {
    auto tf_message = tf_import(library, TF_Message);
    return tf_message(s.get());
}

void tensorflow::dynamic_backend_t::TF_GraphImportGraphDef(tensorflow::graph_ptr &graph, const tensorflow::buffer_ptr &graph_def, const tensorflow::import_graph_def_options_ptr &options, tensorflow::status_ptr &status) {
    auto tf_graph_import_graph_def = tf_import(library, TF_GraphImportGraphDef);
    tf_graph_import_graph_def(graph.get(), graph_def.get(), options.get(), status.get());
}

TF_Operation *tensorflow::dynamic_backend_t::TF_GraphOperationByName(tensorflow::graph_ptr &graph, const char *oper_name) {
    auto tf_graph_operation_by_name = tf_import(library, TF_GraphOperationByName);
    return tf_graph_operation_by_name(graph.get(), oper_name);
}

tensorflow::tensor_ptr tensorflow::dynamic_backend_t::TF_AllocateTensor(TF_DataType type, const int64_t *dims, int num_dims, size_t len) {
    auto tf_allocate_tensor = tf_import(library, TF_AllocateTensor);
    return {
        tf_allocate_tensor(type, dims, num_dims, len),
        [this](TF_Tensor* p){
            auto tf_delete_tensor = tf_import(library, TF_DeleteTensor);
            tf_delete_tensor(p);
        }
    };
}

void tensorflow::dynamic_backend_t::TF_SessionRun(tensorflow::session_ptr &session, const tensorflow::buffer_ptr &run_options,
    const TF_Output *inputs, const tensorflow::tensor_ptr *input_values, int ninputs,
    const TF_Output *outputs, tensorflow::tensor_ptr *output_values, int noutputs,
    const TF_Operation *const *target_opers, int ntargets,
    tensorflow::buffer_ptr &run_metadata, tensorflow::status_ptr &status) {

    auto tf_session_run = tf_import(library, TF_SessionRun);
    auto input_values_unsafe = unbox_ptr_collection(input_values, ninputs);
    auto output_values_unsafe = unbox_ptr_collection(output_values, noutputs);

    tf_session_run(
        session.get(),run_options.get(),
        inputs, input_values_unsafe.data(), input_values_unsafe.size(),
        outputs, output_values_unsafe.data(), output_values_unsafe.size(),
        target_opers, ntargets,
        run_metadata.get(), status.get()
    );
}

void *tensorflow::dynamic_backend_t::TF_TensorData(const tensorflow::tensor_ptr & tensor) {
    auto tf_tensor_data = tf_import(library, TF_TensorData);
    return tf_tensor_data(tensor.get());
}

size_t tensorflow::dynamic_backend_t::TF_TensorByteSize(const tensorflow::tensor_ptr & tensor) {
    auto tf_tensor_byte_size = tf_import(library, TF_TensorByteSize);
    return tf_tensor_byte_size(tensor.get());
}

TF_Code tensorflow::dynamic_backend_t::TF_GetCode(tensorflow::status_ptr &s) {
    auto tf_get_code = tf_import(library, TF_GetCode);
    return tf_get_code(s.get());
}

const char *tensorflow::dynamic_backend_t::TF_Message(tensorflow::status_ptr &s) {
    auto tf_message = tf_import(library, TF_Message);
    return tf_message(s.get());
}

#undef tf_import
