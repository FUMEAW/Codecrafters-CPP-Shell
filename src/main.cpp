#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <stdlib.h>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <optional>

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

std::vector<std::string> splitString(const std::string& input, char delimiter) {
    std::vector<std::string> result;
    std::string::size_type start = 0;
    while (true) {
        auto end = input.find(delimiter, start);
        if (end == std::string::npos) {
            result.push_back(input.substr(start));
            break;
        }
        result.push_back(input.substr(start, end - start));
        start = end + 1;
    }
    return result;
}

std::string checkDirForFile(const std::string& directory, const std::string& command){
    try{
        for (const auto & entry : std::filesystem::directory_iterator(directory)) {
            std::string path{entry.path()};
            std::vector<std::string> folders = splitString(path, '/');
            if (!folders.empty() && folders.back() == command) {
                return path;
            } 
        }
    }
    catch (const std::filesystem::filesystem_error& e){
        return "";
    } 
    return "";
}

std::string checkDirsForFile(const std::vector<std::string>& paths, const std::string& command){
    for (const auto& dir : paths){
        std::string found = checkDirForFile(dir, command);
        if (!found.empty()){
            return found;
        }
    }
    return "";
}

int main() {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    std::string pathEnv = exec("echo $PATH");
    // Remove trailing newline if present
    if (!pathEnv.empty() && pathEnv.back() == '\n') pathEnv.pop_back();
    std::vector<std::string> paths = splitString(pathEnv, ':');

    while (true){
        std::cout << "$ ";
        std::string input;
        std::getline(std::cin, input);
        if (input.substr(0, 4) == "exit" ){
            if (input.contains("0")){
                return 0;
            }
        }
        else if (input.substr(0, 4) == "echo"){
            std::string::size_type echoIndex{input.find(' ') + 1};
            std::string phrase = input.substr(echoIndex);
            std::cout << phrase << '\n';
        }
        else if (input.substr(0,4) == "pwd"){
            std::cout << std::filesystem::current_path().string() << '\n';
        }
        else if (input.substr(0, 4) == "type"){
            std::string::size_type typeIndex{input.find(' ') + 1};
            std::string command = input.substr(typeIndex);
            if (command.contains("echo")){
                std::cout << "echo is a shell builtin" << '\n';
            }
            else if (command.contains("exit")){
                std::cout << "exit is a shell builtin" << '\n';
            }
            else if (command.contains("type")){
                std::cout << "type is a shell builtin" << '\n';
            }
            else if(command.contains("pwd")){
                std::cout << "pwd is a shell builtin" << '\n';
            }
            else{
                std::string commandStatus = checkDirsForFile(paths, command);
                if (commandStatus.empty()){
                    std::cout << command << ": not found" << '\n';
                }
                else{
                  std::cout << command << " is " << commandStatus << '\n';
                }
            }
        }
        else {
            std::vector<std::string> args = splitString(input, ' ');
            std::string commandStatus = checkDirsForFile(paths, args[0]);
            if (!commandStatus.empty()){
                std::cout << exec(input.c_str());
            } else {
                std::cout << input << ": command not found" << '\n';
            }
        }
    }
}

