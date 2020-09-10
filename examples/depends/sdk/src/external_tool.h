#pragma once

#include <string>
#include <vector>
#include <future>

#include <boost/optional.hpp>

namespace posix{
    class external_tool_t {
        static const pid_t child_pid_default{0};
        static const pid_t child_pid_error{-1};
    public:
        struct exit_status_t{
            bool has_terminated{false};
            int code{0};
        };
    public:
        explicit external_tool_t(const std::string &exec);
        exit_status_t execute(const std::vector<std::string> &args, int pipe);
        void interrupt() const;

    protected:
        static std::vector<const char *> arguments(const std::string& exec, const std::vector<std::string> &args);

    private:
        const std::string exec;
        boost::optional<pid_t> child_pid;
    };
}
