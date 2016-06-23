#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

using strings = std::vector<std::string>;

void pipemaker(strings &tokens) {
  auto from = std::begin(tokens);
  std::string const strpipe{"|"};
  while (from != std::end(tokens)) {
    auto pipe = std::find(from, std::end(tokens), strpipe);
    std::cout << "Command: ";
    std::copy(from, pipe, std::ostream_iterator<std::string>(std::cout, " + "));
    std::cout << std::endl;
    if (std::end(tokens) == pipe) {
      break;
    } else {
      from = ++pipe;
    }
  }
}

int main() {
  strings tokens;
  std::string cmd;
  while ((std::cin >> cmd)) {
    tokens.push_back(cmd);
    std::cout << "Token: '" << cmd << '\'' << std::endl;
  }
  pipemaker(tokens);
}
