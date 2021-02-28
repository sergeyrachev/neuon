#pragma once

#include "neuon/neuon_c.h"

#include <sstream>

namespace neuon {

    class log_stream_t : public std::ostringstream {
    public:
        log_stream_t(neuon_logging_t * impl, log_function_t log_function): impl(impl), log_function(log_function){

        }

        ~log_stream_t(){
            (*this) << std::endl;
            log_function(impl, this->str().c_str());
        }
    private:
        neuon_logging_t * impl;
        log_function_t log_function;
    };

    class log_t {
    public:
        explicit log_t(neuon_logging_t* app_log):impl(app_log){

        }

        log_stream_t debug() const{
            return log_stream_t(impl, impl->debug ? impl->debug : dummy_log_function);
        }
        log_stream_t info() const{
            return log_stream_t(impl, impl->info ? impl->info : dummy_log_function);
        }
        log_stream_t error() const{
            return log_stream_t(impl, impl->error ? impl->error : dummy_log_function);
        }

    private:
        static void dummy_log_function(neuon_logging_t *, const char *){
        }
        neuon_logging_t* impl;
    };
}
