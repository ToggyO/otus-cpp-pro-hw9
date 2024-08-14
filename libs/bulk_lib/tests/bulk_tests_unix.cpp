#include <gtest/gtest.h>
#include <filesystem>

#include "config.h"

// ***************************************************************************
//                     UNIX SPECIFIC TESTS
// ***************************************************************************

std::stringstream exec(std::string &command)
{
    int buffer_size = 128;
    char buffer[buffer_size];
    std::stringstream ss;

    auto pipe_reader = popen(command.c_str(), "r");
    if (!pipe_reader)
    {
        throw std::runtime_error("Failed to open pipe");
    }

    while (!feof(pipe_reader))
    {
        if (fgets(buffer, buffer_size, pipe_reader) != nullptr)
        {
            ss << buffer;
        }
    }

    pclose(pipe_reader);
    return ss;
}

TEST(BulkTests, Common) {
    static constexpr int k_block_size = 3;
    static const std::string k_pipe = " | ";

    auto project_root = get_test_project_root();
    auto executable = get_tested_executable_path();
    auto test_data_file_path = std::filesystem::path(project_root) / "commands.tsv";

    std::ostringstream oss;
    oss << "cat "
        << test_data_file_path << k_pipe
        << executable << " " << k_block_size
        << " " << "--file-out false";

    auto cmd = oss.str();
    auto output = exec(cmd);

    std::string results[3] = {
        "bulk: cmd1, cmd2",
        "bulk: cmd3, cmd4",
        "bulk: cmd5, cmd6, cmd7, cmd8, cmd9"
    };

    std::string line;
    int i = 0;
    while (std::getline(output, line))
    {
        EXPECT_EQ(line, results[i]);
        ++i;
    }
}
