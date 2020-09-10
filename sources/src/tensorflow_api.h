#pragma once

#include "tensorflow/c/c_api.h"

#include <memory>
#include <functional>
#include <vector>

namespace tensorflow{
    using buffer_ptr = std::unique_ptr<TF_Buffer, std::function<void(TF_Buffer *p)>>;
    using import_graph_def_options_ptr = std::unique_ptr<TF_ImportGraphDefOptions, std::function<void(TF_ImportGraphDefOptions*)>>;
    using status_ptr = std::unique_ptr<TF_Status, std::function<void(TF_Status *p)>>;
    using session_options_ptr = std::unique_ptr<TF_SessionOptions, std::function<void(TF_SessionOptions*)>>;
    using graph_ptr = std::unique_ptr<TF_Graph, std::function<void(TF_Graph *)>>;
    using session_ptr = std::unique_ptr<TF_Session, std::function<void(TF_Session *)>>;
    using tensor_ptr = std::unique_ptr<TF_Tensor, std::function<void(TF_Tensor *)>>;

    class api_t{
    public:
        virtual ~api_t() = default;
        virtual graph_ptr TF_NewGraph() = 0;
        virtual import_graph_def_options_ptr TF_NewImportGraphDefOptions() = 0;
        virtual session_options_ptr TF_NewSessionOptions() = 0;
        virtual status_ptr TF_NewStatus() = 0;
        virtual TF_Code TF_GetCode(status_ptr& s) = 0;
        virtual const char *TF_Message(status_ptr& s) = 0;

        virtual buffer_ptr TF_NewBufferFromString(const void* proto, size_t proto_len) = 0;
        virtual session_ptr TF_NewSession(graph_ptr& graph,
                                          const session_options_ptr& opts,
                                          status_ptr& status) = 0;
        virtual const char* TF_Message(const status_ptr& s) = 0;

        virtual void TF_GraphImportGraphDef(graph_ptr& graph, const buffer_ptr& graph_def,
                                            const import_graph_def_options_ptr& options, status_ptr& status) = 0;

        virtual TF_Operation* TF_GraphOperationByName(graph_ptr& graph, const char* oper_name) = 0;
        virtual tensor_ptr TF_AllocateTensor(TF_DataType, const int64_t* dims, int num_dims, size_t len) = 0;

        virtual void TF_SessionRun( session_ptr& session, const buffer_ptr& run_options,
                                    const TF_Output* inputs, const tensor_ptr* input_values, int ninputs,
                                    const TF_Output* outputs, tensor_ptr* output_values, int noutputs,
                                    const TF_Operation* const* target_opers, int ntargets,
                                    buffer_ptr& run_metadata, status_ptr& status) = 0;

        virtual void* TF_TensorData(const tensor_ptr& tensor) = 0;
        virtual size_t TF_TensorByteSize(const tensor_ptr& tensor) = 0;
        virtual int64_t TF_TensorElementCount(const tensor_ptr& tensor) = 0;

        virtual const char* TF_OperationName(TF_Operation* oper) = 0;
        virtual TF_Operation* TF_GraphNextOperation(graph_ptr& graph, size_t* pos) = 0;
    };

    class impl_t{
    protected:
        static std::vector<TF_Tensor*> unbox_ptr_collection(const tensorflow::tensor_ptr * begin, int count );
    };
}
