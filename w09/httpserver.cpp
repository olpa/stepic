#include <iostream>
#include <fstream>
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
// TODO: error_code: with or without reference
//
// TODO: timeout
//
// TODO: delete(self) watchdog, a la unique_ptr

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

std::string resolve_uri(const ServerOptions& opt, const std::string& uri) {
  std::string fname(opt.root);
  if (! uri.empty()) {
    if ('/' != uri[0]) {
      fname.append("/"); // ok also for win32
    }
    fname.append(uri);
  }
  return fname;
}

//
//
//
template<class T>
class Killer {
  T    *obj_ = nullptr;
  bool kill_ = true;
public:
  Killer(T *obj): obj_(obj) {};
  Killer(Killer &) = delete;
  ~Killer() {
    if (kill_ and obj_) {
      std::cerr << "Killer deletes" << std::endl; // FIXME
      delete obj_;
    }
  }
  void escape() {
    kill_ = false;
  }
};

//
// Session
//
using upsocket_t = std::unique_ptr<asio::ip::tcp::socket>;

class Session {
  upsocket_t          upsocket_;
  asio::streambuf     buf_;
  const ServerOptions &opt_;
  std::ifstream       file_content;
  constexpr static size_t chunk_size = 1024;
  char  file_content_buf[chunk_size];
public:
  Session(upsocket_t& upsocket, const ServerOptions& opt): upsocket_(std::move(upsocket)), opt_(opt) {}

  void start() {
    Killer<Session> killer(this);
    auto cb = std::bind(&Session::after_read, this, std::placeholders::_1);
    asio::ip::tcp::socket *sock = upsocket_.get();
    asio::async_read_until(*sock, buf_, "\r\n", cb);
    killer.escape();
  }

  void after_read(const asio::error_code& error) {
    Killer<Session> killer(this);
    if (error) {
      std::cerr << "after_read error: " << error << std::endl;
      return;
    }
    std::istream is(&buf_);
    std::string method, uri;
    is >> method >> uri; // Assumed: no spaces in uri. Don't care about the http-version
    std::cout << "Got request (" << method << "):" << uri << std::endl;
    handle_http_request(method, uri);
    killer.escape();
  }

  void handle_http_request(const std::string &method, const std::string uri) {
    Killer<Session> killer(this);
    std::string response;
    if (method != "GET") {
      response = "HTTP/1.0 501 Not Implemented\r\n";
    } else {
      std::string fname = resolve_uri(opt_, uri);
      file_content.open(fname, std::ifstream::binary);
      if (file_content.good()) {
        response = "HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n";
      } else {
        response = "HTTP/1.0 404 Not found\r\n";
      }
    }
    auto cb = std::bind(&Session::loop_write_file_content, this, std::placeholders::_1);
    asio::ip::tcp::socket *sock = upsocket_.get();
    asio::async_write(*sock, asio::buffer(response.c_str(), response.size()), cb);
    killer.escape();
  }

  void loop_write_file_content(const asio::error_code& error) {
    Killer<Session> killer(this);
    if (error) {
      std::cerr << "after_write_file_content error: " << error << std::endl;
      return;
    }
    if (file_content.good()) {
      file_content.read(file_content_buf, chunk_size);
      auto cb = std::bind(&Session::loop_write_file_content, this, std::placeholders::_1);
      asio::ip::tcp::socket *sock = upsocket_.get();
      asio::async_write(*sock, asio::buffer(file_content_buf, file_content.gcount()), cb);
      killer.escape();
    }
  }
};

//
// Server
//
class Server {
    asio::io_service &service_;
    const ServerOptions opt_;
    std::unique_ptr<asio::ip::tcp::acceptor> acceptor_;
    asio::ip::tcp::endpoint endpoint_;
  public:
    Server(asio::io_service &service, ServerOptions &opt):
      service_(service), opt_(opt) { };

    asio::io_service& get_service() {
      return service_;
    }

    bool start() {
      asio::ip::tcp::resolver::query query(opt_.host_name, std::to_string(opt_.port));
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
      Session *psession = new Session(upsock, opt_); // freed in Session self using delete(self)
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
  // Deamon
  //
  if (! opt.stay_foreground) {
    std::cout << "PID = " << getpid() << std::endl;
    daemon(0, 0);
  }
  //
  // io_service loop
  //
  signal(SIGINT, [](int){
      std::cout << "Normal exit on Ctrl+C" << std::endl;
      io_service.stop();
      });
  io_service.run();
  std::cout << "Exiting" << std::endl;
  return 0;
}
