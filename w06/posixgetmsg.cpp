#include <cassert>
#include <iostream>
#include <fstream>
#include <memory>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

struct message {
    long mtype;
    char mtext[80];
};

void xerror(int code, std::string func) {
  if (-1 == code) {
    perror(func.c_str());
    exit(-1);
  }
}

int main() {
  std::string qname{"/test.mq"};
  mqd_t mqdes = mq_open(qname.c_str(), O_RDWR|O_CREAT|O_EXCL, 0777, nullptr);
  xerror(mqdes, "mq_open");

  char buf[10240]; // better to get the size from the system
  //int len = mq_receive(mqdes, buf, sizeof(buf), nullptr);
  int len = 2;
  buf[0] = 'A';
  buf[1] = 'B';
  std::string s{buf, static_cast<unsigned int>(len)};
  std::ofstream h("message.txt");
  h << s << std::endl;
  h.close();

  int ccode = mq_close(mqdes);
  xerror(ccode, "mq_close");
  ccode = mq_unlink(qname.c_str());
  xerror(ccode, "mq_unlink");
}
