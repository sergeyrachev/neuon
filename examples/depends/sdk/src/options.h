#pragma once

#include <boost/program_options.hpp>
#include <boost/type_traits.hpp>
#include <iostream>

#include <functional>
#include <type_traits>
#include <algorithm>
#include <list>

namespace options{
    namespace details{
        struct boundary_validator_t{
        public:
            typedef void result_type;

        public:
            template<typename T>
            void operator()(const std::string& name, T v, T max, T min){
                if (v < min || v > max) {
                    throw boost::program_options::validation_error(boost::program_options::validation_error::invalid_option_value, name, std::to_string(static_cast<long long int>(v)));
                }
            }
        };
    }

    template<typename T>
    std::function<void(T)> is_value_valid(const std::string& opt_name, T max, T min = T()){
        return std::bind(details::boundary_validator_t(), opt_name, std::placeholders::_1, max, min);
    };
    
    bool is_args_valid(int argc, const char * const * argv, const boost::program_options::options_description &opt_desc,
                       const boost::program_options::positional_options_description &pos_opt_desc,
                       boost::program_options::variables_map &varmap, std::ostream &cerr, std::ostream &cout, const std::string& info = std::string());
}
