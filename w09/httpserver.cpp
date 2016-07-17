#include <iostream>
#include <string>
#include <unistd.h>
#include <assert.h>

int main(int argc, char **argv) {
  bool stay_foreground;
  std::string ip_address;
  unsigned int port;
  std::string root;
  int c;
  while ((c = getopt(argc, argv, "Fh:p:d:")) != -1) {
    switch (c) {
      case 'F':
        stay_foreground = true;
        std::cout << "StayForeground=true" << std::endl;
        break;
      case 'h':
        ip_address = optarg;
        std::cout << "IP=" << ip_address << std::endl;
        break;
      case 'p':
        port = std::stoi(optarg);
        std::cout << "port=" << port << std::endl;
        break;
      case 'd':
        root = optarg;
        std::cout << "directory=" << root << std::endl;
        break;
      case ':':
        std::cerr << "Option -" << static_cast<char>(optopt) << " requires an operand" << std::endl;
        return 1;
      case '?':
        std::cerr << "Unrecognized option -" << static_cast<char>(optopt) << std::endl;
        return 1;
      default:
        std::cerr << "Internal error parsing options" << std::endl;
        return 1;
    }
  }
}
