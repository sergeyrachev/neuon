#include "external_tool.h"

#include "logging_macro.h"

#include <algorithm>
#include <iterator>
#include <iostream>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/file.h>

#include <boost/asio.hpp>

using namespace posix;

namespace {
    void print(const std::string& exec, const std::vector<std::string>& args) {

        std::stringstream ss;
        ss << "Run tool '" << exec << "' cmdline: " << exec << " ";
        std::copy(args.begin(), args.end(), std::ostream_iterator<std::string>(ss, " "));
        debuglog() << ss.str() ;
    }
}

external_tool_t::external_tool_t(const std::string &exec)
    : exec(exec)
{
}

external_tool_t::exit_status_t external_tool_t::execute(const std::vector<std::string> &args, int pipe) {
    print(exec, args);

    exit_status_t ret{};
    int status(0);

    child_pid = fork();
    if (child_pid_error == *child_pid) {
        ret.code = -1;
    } else if (child_pid_default == *child_pid) {

        dup2(pipe, STDOUT_FILENO);
        dup2(pipe, STDERR_FILENO);
        close(pipe);

        auto argv = arguments(exec, args);
        // Never returns in case of success, exit child process in case of failure
        // Be aware - we cast pointer to const value to const pointer to value;
        // it would be nice to have non-const allocated memory to avoid explicit const_cast
        execve(exec.c_str(), const_cast<char *const *>(argv.data()), nullptr);
        exit(errno);
    } else if (*child_pid) {
        bool is_waiting(true);
        do {
            waitpid(*child_pid, &status, WUNTRACED | WCONTINUED);

            if (WIFEXITED(status)) {
                is_waiting = false;
                ret.code = WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                is_waiting = false;
                ret.has_terminated = true;
                ret.code = WTERMSIG(status);
            }
        } while (is_waiting);
    }

    return ret;
}

void external_tool_t::interrupt() const {
    if( child_pid ){
        kill(*child_pid, SIGINT);
    }
}

std::vector<const char *> external_tool_t::arguments(const std::string& exec, const std::vector <std::string> &args) {
    std::vector<const char *> argv;
    argv.push_back(exec.c_str());
    std::transform(args.begin(), args.end(), std::back_inserter(argv), [](const std::string &s) {
        return s.c_str();
    });
    argv.push_back(nullptr);

    return argv;
}
