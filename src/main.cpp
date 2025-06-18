#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <cstdlib>
#include <algorithm>

#include <unistd.h>

#include "Console.hpp"

#include <readline/history.h>

std::string exec(const char *cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

std::vector<std::string> splitString(const std::string &input, char delimiter)
{
    std::vector<std::string> result;
    std::string::size_type start = 0;
    while (true)
    {
        auto end = input.find(delimiter, start);
        if (end == std::string::npos)
        {
            result.push_back(input.substr(start));
            break;
        }
        result.push_back(input.substr(start, end - start));
        start = end + 1;
    }
    return result;
}

std::string checkDirForFile(const std::string &directory, const std::string &command)
{
    try
    {
        for (const auto &entry : std::filesystem::directory_iterator(directory))
        {
            std::string path{entry.path()};
            std::vector<std::string> folders = splitString(path, '/');
            if (!folders.empty() && folders.back() == command)
            {
                return path;
            }
        }
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        return "";
    }
    return "";
}

std::string checkDirsForFile(const std::vector<std::string> &paths, const std::string &command)
{
    for (const auto &dir : paths)
    {
        std::string found = checkDirForFile(dir, command);
        if (!found.empty())
        {
            return found;
        }
    }
    return "";
}

namespace cr = CppReadline;
using ret = cr::Console::ReturnCode;

unsigned echo(std::vector<std::string> inputVector)
{
    if (inputVector.size() > 1)
    {
        for (long unsigned i = 1; i < inputVector.size(); i++)
        {
            std::cout << inputVector[i].c_str() << " ";
            if (i == (inputVector.size() - 1))
            {
                std::cout << '\n';
            }
        }
    }
    return 0;
}

unsigned my_exit(std::vector<std::string> inputVector)
{
    if (inputVector.size() > 1 && inputVector[1] == "0")
    {
        std::exit(0);
    }
    return 0;
}

unsigned pwd([[maybe_unused]] std::vector<std::string> inputVector)
{
    std::cout << std::filesystem::current_path().string() << '\n';
    return 0;
}

unsigned cd(std::vector<std::string> inputVector)
{
    if (inputVector.size() > 1)
    {
        std::string directory = inputVector[1];
        if (directory.contains("~"))
        {
            std::string_view home{std::getenv("HOME")};
            auto pos{directory.find("~")};
            directory.replace(pos, pos + 1, home);
        };
        if (chdir(directory.c_str()) == -1)
        {
            std::cout << "cd: " << directory << ": No such file or directory" << '\n';
            return 1;
        }
    }
    return 0;
}

unsigned type(std::vector<std::string> inputVector)
{
    if (inputVector.size() > 1)
    {
        std::string command = inputVector[1];
        std::string pathEnv = exec("echo $PATH");
        if (!pathEnv.empty() && pathEnv.back() == '\n')
            pathEnv.pop_back(); // Remove trailing newline if present
        std::vector<std::string> paths = splitString(pathEnv, ':');
        std::string commandStatus = checkDirsForFile(paths, command);

        if (command.contains("echo"))
            std::cout << "echo is a shell builtin" << '\n';
        else if (command.contains("exit"))
            std::cout << "exit is a shell builtin" << '\n';
        else if (command.contains("type"))
            std::cout << "type is a shell builtin" << '\n';
        else if (command.contains("pwd"))
            std::cout << "pwd is a shell builtin" << '\n';
        else if (command.contains("history"))
        {
            std::cout << "history is a shell builtin" << '\n';
        }
        else if (commandStatus.empty())
        {
            std::cout << command << ": not found" << '\n';
            return 1;
        }
        else
        {
            std::cout << command << " is " << commandStatus << '\n';
            return 0;
        }
    }
    return 0;
}

int main()
{
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    cr::Console c("$ ");

    std::string pathEnv{std::getenv("PATH")};
    if (!pathEnv.empty() && pathEnv.back() == '\n')
        pathEnv.pop_back(); // Remove trailing newline if present


    c.registerCommand("echo", echo);
    c.registerCommand("exit", my_exit);
    c.registerCommand("pwd", pwd);
    c.registerCommand("cd", cd);
    c.registerCommand("type", type);

    while (c.readLine() != ret::Quit)
    {
        HIST_ENTRY **histArray[]{c.getHistory()};
        int size = sizeof(histArray) / sizeof(histArray[0]);
        for (int i = size - 5; i < size; i++)
        {
            std::string_view currentEntry{histArray[0][0]->line};
        }
    }
}
