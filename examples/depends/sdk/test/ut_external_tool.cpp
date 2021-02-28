#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "external_tool.h"

using posix::external_tool_t;

class uncovered_external_tool_t : public external_tool_t{
public:
    using external_tool_t::external_tool_t;
    using external_tool_t::arguments;
    using external_tool_t::execute;
};

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
        auto ret = tool.execute({}, STDERR_FILENO);
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
        return tool.execute({}, STDERR_FILENO);
    });

    auto i = async_exec.get();
    ASSERT_FALSE(i.has_terminated);
    ASSERT_EQ(1, i.code);
}
