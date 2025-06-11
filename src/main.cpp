#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <csdlib>

std::string getSubString(int index, std::string input, int length){
  std::string phrase{input.substr(index, length)};
  return phrase;
}
int checkDirForFile(std::string directory, std::string command){
  for (const auto & entry : std::filesystem::directory_iterator(directory)) {
    std::string path{entry.path()};
    if( path.contains(command) ){
      std::cout << command << " is " << directory;
      return 1;
    }
  }
  return 0;
}
std::vector<std::string> getPaths(std::string argPath){
  bool pathSearch = true;
  std::vector<std::string> paths;

  int startIndex{0};
  while (pathSearch){
    int index = argPath.find(':');
    if (index = -1){
      pathSearch = false;
      break;
    }
    paths.push_back(getSubString(0,argPath,index));
    argPath = getSubString(index,argPath,(argPath.length() - index));
  }

  return paths;
}

int main(int argc, char* argv[]) {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::vector<std::string> paths = getPaths(system("echo $PATH"));

  // Uncomment this block to pass the first stage
  while (true){
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);
    if (getSubString(0, input, 4) == "exit" ){
      if (input.contains("0")){
        return 0;
      }
    }
    else if (getSubString(0, input, 4) == "echo"){
      long unsigned int echoIndex{input.find(' ') + 1}; //Returns the index of the first character after echo
      std::string phrase = getSubString(echoIndex,input,(input.length() - echoIndex));
      std::cout << phrase << '\n';
    }
    else if (getSubString(0, input, 4) == "type"){
      long unsigned int typeIndex{input.find(' ') + 1}; //Returns the index of the first character after type
      std::string command = getSubString(typeIndex, input, (input.length() - typeIndex));
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
        for (int i = 0; i < paths.size(); i++){
          commandStatus = checkDirForFile(paths[i],command);
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

