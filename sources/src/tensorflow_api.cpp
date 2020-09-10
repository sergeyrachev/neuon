#include "tensorflow_api.h"

#include <algorithm>

std::vector<TF_Tensor *> tensorflow::impl_t::unbox_ptr_collection(const tensorflow::tensor_ptr *begin, int count) {
    std::vector<TF_Tensor*> unsafe_ptrs;
    std::transform(begin, begin + count, std::back_inserter(unsafe_ptrs), [](const tensorflow::tensor_ptr& tensor){
        return tensor.get();
    });
    return unsafe_ptrs;
}
