#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "external_tool.h"

#include "async_pipe.h"

using namespace magic::restclient;
class uncovered_external_tool_t : public external_tool_t{
public:
    using external_tool_t::external_tool_t;
    using external_tool_t::arguments;
    using external_tool_t::execute;
};

TEST(external_tool, logging_single_file){
    external_tool_t tool("/usr/bin/ls");
    logging::async_pipe_t log("/dev/stdout");
    auto async_exec = std::async(std::launch::async, [&tool, &log](){
        return tool.execute({}, log.native_handle());
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

TEST(external_tool, logging_rotate_file){
    external_tool_t tool("/usr/bin/ls");
    logging::async_pipe_t log("log", 20, 1 );
    auto async_exec = std::async(std::launch::async, [&tool, &log](){
        return tool.execute({}, log.native_handle());
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

TEST(external_tool, arguments){
    std::vector<std::string> args{"arg1", "arg2"};

    auto argv = uncovered_external_tool_t::arguments("exec", args);
    ASSERT_STREQ("exec", argv[0]);
    ASSERT_STREQ("arg1", argv[1]);
    ASSERT_STREQ("arg2", argv[2]);
    ASSERT_EQ(nullptr, argv[3]);
}

TEST(external_tool, interruption){
    uncovered_external_tool_t tool("/usr/bin/yes");
    auto async_exec = std::async(std::launch::async, [&tool](){
        logging::async_pipe_t log("/dev/null");
        auto ret = tool.execute({}, log.native_handle());
        return ret;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    tool.interrupt();

    auto i = async_exec.get();
    ASSERT_TRUE(i.has_terminated);
    ASSERT_EQ(2, i.code);
}

TEST(external_tool, gracefull_exit){

    uncovered_external_tool_t tool("/usr/bin/test");
    auto async_exec = std::async(std::launch::async, [&tool](){
        logging::async_pipe_t log("/dev/null");
        return tool.execute({}, log.native_handle());
    });

    auto i = async_exec.get();
    ASSERT_FALSE(i.has_terminated);
    ASSERT_EQ(1, i.code);
}

TEST(external_tool, check_log_message_format){
    uncovered_external_tool_t tool("/usr/bin/usefull_tool");
    logging::async_pipe_t log("/dev/null");
    tool.execute({"-h", "\"quotes\""}, log.native_handle());
}
