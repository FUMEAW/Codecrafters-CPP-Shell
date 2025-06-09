#include <iostream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // Uncomment this block to pass the first stage
  while (true){
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);
    if (input.contains("exit") ){
      if (input.contains("0")){
        return 0;
      }
    }
    else if (input.contains("echo")){
      int echoIndex{input.find(' ') + 1}; //Returns the index of the first character after echo
      std::string phrase{input.substr(echoIndex, (input.length() - echoIndex))};
      std::cout << phrase << '\n';
    }
    else {
      std::cout << input << ": command not found" << '\n';
    }
  }
  
}
