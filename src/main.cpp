#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <stdlib.h>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>

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
std::string getSubString(std::string::size_type index, const std::string& input, std::string::size_type length){
  std::string phrase{input.substr(index, length)};
  return phrase;
}
std::vector<std::string> splitString(std::string_view argPath, char delimeter){
  std::string path{argPath};
  std::vector<std::string> paths;
  while (true){
    std::string::size_type index = path.find(delimeter);
    if (index == std::string::npos){
      break;
    }
    paths.push_back(path.substr(0, index));
    path = path.substr(index + 1);
  }
  if (!path.empty()) {
    paths.push_back(path);
  }
  return paths;
}
int checkDirForFile(const std::string& directory, const std::string& command){
  try{
    for (const auto & entry : std::filesystem::directory_iterator(directory)) {
    std::string path{entry.path()};
    std::vector<std::string> folders = splitString(path, '/');
    if (folders[folders.size() - 1] == command) {
      std::cout << command << " is " << path << '\n';
      return 1;
      } 
    }
  }
  catch (const std::filesystem::filesystem_error& e){
    return 0;
  } 
  return 0;
}



int main() {
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::vector<std::string> paths = splitString(exec("echo $PATH"), ':');

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
      else{
        int commandStatus{};
        for (std::size_t i = 0; i < paths.size(); i++){
          commandStatus = checkDirForFile(paths[i], command);
          if(commandStatus){
            break;
          }
        }
        if (commandStatus == 0){
          std::cout << command << ": not found" << '\n';
        }
      }
    }
    else {
      std::cout << input << ": command not found" << '\n';
    }
  }
}

