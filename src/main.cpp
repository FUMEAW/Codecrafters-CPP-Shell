#include <iostream>
#include <string>

std::string getSubString(int index, std::string input, int length){
  std::string phrase{input.substr(index, length)};
  return phrase;
}
int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

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
      int echoIndex{input.find(' ') + 1}; //Returns the index of the first character after echo
      std::string phrase = getSubString(echoIndex,input,(input.length() - echoIndex));
      std::cout << phrase << '\n';
    }
    else if (getSubString(0, input, 4) == "type"){
      int typeIndex{input.find(' ') + 1}; //Returns the index of the first character after type
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
        std::cout << command << ": not found" << '\n';
      }
    }
    else {
      std::cout << input << ": command not found" << '\n';
    }
  }
  
}
