#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <fcntl.h>
#include <unistd.h>

using strings = std::vector<std::string>;

// After fork,
// the parent runs the curent command, and
// the child continues the loop.
void pipemaker(strings &tokens) {
  auto from = std::begin(tokens);
  std::string const strpipe{"|"};
  while (from != std::end(tokens)) {
    auto itpipe = std::find(from, std::end(tokens), strpipe);
    assert (from != itpipe); // empty command not allowed
    if (std::end(tokens) != itpipe) {
      int pid, fd[2];
      if (-1 == pipe(fd)) {
        perror("pipe");
        exit(-1);
      }
      switch ((pid = fork())) {
        case -1: perror("fork");
                 exit(-1);
        case 0:  assert(-1 != close(fd[0]));
                 assert(-1 != dup2(fd[1], 1));
                 break;
        default: assert(-1 != close(fd[1]));
                 assert(-1 != dup2(fd[0], 0));
                 from = 1 + itpipe;
                 continue;                                 // loop
      }
    }
    std::vector<const char*> curcmd;
    for (auto i = from; i != itpipe; ++i) {
      curcmd.push_back((*i).c_str());
    }
    curcmd.push_back(NULL);
    //std::cout << "Command: ";
    //std::copy(curcmd.begin(), curcmd.end(), std::ostream_iterator<const char*>(std::cout, " + "));
    //std::cout << std::endl << std::flush;
    char *const *argv = const_cast<char *const *>(curcmd.data());
    const char *file  = const_cast<const char*>(curcmd[0]);
    execvp(file, argv);
  }
}

int main() {
  strings tokens;
  std::string cmd;
  std::string pipe("|");
  while ((std::cin >> cmd)) {
    std::size_t pos;
    while ((pos = cmd.find('|')) != std::string::npos) {
      if (pos > 0) {
        tokens.push_back(cmd.substr(0, pos));
      };
      tokens.push_back(pipe);
      cmd = cmd.substr(pos+1);
    }
    if (! cmd.empty()) {
      tokens.push_back(cmd);
    }
  }
  pipemaker(tokens);
}
