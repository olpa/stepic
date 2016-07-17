#include <iostream>
#include <string>
#include <unistd.h>
#include <assert.h>

struct ServerOptions {
  bool stay_foreground;
  bool config_ok;
  std::string ip_address;
  unsigned int port;
  std::string root;
};

ServerOptions parse_cmdline(int argc, char **argv) {
  ServerOptions opt;
  int c;
  while ((c = getopt(argc, argv, "Fh:p:d:")) != -1) {
    switch (c) {
      case 'F':
        opt.stay_foreground = true;
        std::cout << "StayForeground=true" << std::endl;
        break;
      case 'h':
        opt.ip_address = optarg;
        std::cout << "IP=" << opt.ip_address << std::endl;
        break;
      case 'p':
        opt.port = std::stoi(optarg);
        std::cout << "port=" << opt.port << std::endl;
        break;
      case 'd':
        opt.root = optarg;
        std::cout << "directory=" << opt.root << std::endl;
        break;
      case ':':
        std::cerr << "Option -" << static_cast<char>(optopt) << " requires an operand" << std::endl;
        return opt;
      case '?':
        std::cerr << "Unrecognized option -" << static_cast<char>(optopt) << std::endl;
        return opt;
      default:
        std::cerr << "Internal error parsing options" << std::endl;
        return opt;
    }
  }
  opt.config_ok = true;
  return opt;
}

int main(int argc, char **argv) {
  ServerOptions opt = parse_cmdline(argc, argv);
  if (! opt.config_ok) {
    return 1;                                              // exit
  }
}
