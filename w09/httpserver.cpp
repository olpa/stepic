#include <iostream>
#include <string>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <asio.hpp>

//
// Command line
//
struct ServerOptions {
  bool stay_foreground;
  bool config_ok;
  std::string host_name;
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
        opt.host_name = optarg;
        std::cout << "IP=" << opt.host_name << std::endl;
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

//
// Server
//
class Server {
  private:
    asio::io_service &service_;
  public:
    Server(asio::io_service &service, ServerOptions &opt, asio::error_code &ec):
      service_(service) {
      asio::ip::tcp::resolver::query query(opt.host_name, std::to_string(opt.port));
      asio::ip::tcp::resolver dns(service_);
      auto i = dns.resolve(query, ec);
      if (ec) {
        std::cerr << "Resolver error: " << ec << std::endl;
        return;
      }
      asio::ip::tcp::endpoint endpoint = *i;
      std::cout << "Got endpoint " << endpoint << std::endl;
    }
};

//
// Entry point
//
int main(int argc, char **argv) {
  //
  // Command line
  //
  ServerOptions opt = parse_cmdline(argc, argv);
  if (! opt.config_ok) {
    return 1;                                              // exit
  }
  //
  // Bind on interface
  //
  //
  // io_service loop
  //
  static asio::io_service io_service; // static for lambda
  asio::error_code ec;
  Server srv(io_service, opt, ec);
  if (ec) {
    return 1;                                              // exit
  }
  asio::io_service::work work(io_service);
  signal(SIGINT, [](int){
      std::cout << "Normal exit on Ctrl+C" << std::endl;
      io_service.stop();
      });
  io_service.run();
  std::cout << "Exiting" << std::endl;
  return 0;
}
