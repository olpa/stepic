#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <asio.hpp>

// g++ -std=c++14 httpserver.cpp -lpthread

// TODO: check is multithreaded
//
// TODO: why lambdas do not work (20 jul)
//
// TODO: check for memory leaks
//
// error_code: with or without reference

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
// Session
//
using upsocket_t = std::unique_ptr<asio::ip::tcp::socket>;

class Session {
  upsocket_t upsocket_;
  asio::streambuf buf_;
public:
  Session(upsocket_t &upsocket): upsocket_(std::move(upsocket)) {}

  void start() {
    auto cb = std::bind(&Session::on_read, this, std::placeholders::_1);
    asio::ip::tcp::socket *sock = upsocket_.get();
    asio::async_read_until(*sock, buf_, "\r\n", cb);
  }

  void on_read(const asio::error_code& error) {
    if (error) {
      std::cerr << "on_read error: " << error << std::endl;
      delete this;
      return;
    }
    std::istream is(&buf_);
    std::string line;
    std::getline(is, line);
    std::cout << "buffer: " << line << std::endl;
    handle_http_request(line);
  }

  void handle_http_request(const std::string &req) {
    std::ostream os(&buf_);
    os << "HTTP/1.1 400 Bad Request\r\n";
    auto cb = std::bind(&Session::on_write, this, std::placeholders::_1);
    asio::ip::tcp::socket *sock = upsocket_.get();
    asio::async_write(*sock, buf_, cb);
    //std::string badreq{"HTTP/1.1 400 Bad Request"};
  }

  void on_write(const asio::error_code& error) {
    if (error) {
      std::cerr << "on_write error: " << error << std::endl;
    } else {
      std::cout << "on_write: done" << std::endl;
    }
    delete this;
  }
};

//
// Server
//
class Server {
    asio::io_service &service_;
    const ServerOptions opt;
    std::unique_ptr<asio::ip::tcp::acceptor> acceptor_;
    asio::ip::tcp::endpoint endpoint_;
  public:
    Server(asio::io_service &service, ServerOptions &opt):
      service_(service), opt(opt) { };

    asio::io_service& get_service() {
      return service_;
    }

    bool start() {
      asio::ip::tcp::resolver::query query(opt.host_name, std::to_string(opt.port));
      asio::ip::tcp::resolver dns(service_);
      asio::error_code ec;
      auto i = dns.resolve(query, ec);
      if (ec) {
        std::cerr << "Resolver error: " << ec << std::endl;
        return false;
      }
      endpoint_ = *i;
      std::cout << "Got endpoint " << endpoint_ << std::endl;
      try {
        acceptor_ = std::make_unique<asio::ip::tcp::acceptor>(service_, endpoint_);
        accept_again();
      } catch (asio::system_error e) {
        std::cerr << "Acceptor error: " << e.what() << std::endl;
        return false;
      };
      return true;
    }

    void accept_again() {
      auto sock = new asio::ip::tcp::socket(service_); // will be overtaken by Session
      /*
      auto cb =  [&](const asio::error_code& error) {
          psession->on_accept(error);
          accept_again();
          };
          */
      auto cb = std::bind(&Server::on_accept, this, sock, std::placeholders::_1);
      acceptor_->async_accept(*sock, endpoint_, cb);
    }

  void on_accept(asio::ip::tcp::socket* sock, const asio::error_code& error) {
    upsocket_t upsock(sock);
    if (error) {
      std::cerr << "on_accept error: " << error << std::endl;
    } else {
      std::cout << "Got connection" << std::endl;
      Session *psession = new Session(upsock); // freed in Session self using delete(self)
      // TODO: handle the connection in the new thread
      psession->start();
      accept_again();
    }
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
  static asio::io_service io_service; // static for lambda
  asio::error_code ec;
  Server server(io_service, opt);
  if (! server.start()) {
    return 1;                                              // exit
  }
  //
  // io_service loop
  //
  //asio::io_service::work work(io_service);
  signal(SIGINT, [](int){
      std::cout << "Normal exit on Ctrl+C" << std::endl;
      io_service.stop();
      });
  io_service.run();
  std::cout << "Exiting" << std::endl;
  return 0;
}
