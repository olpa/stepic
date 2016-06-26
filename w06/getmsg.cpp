//#include <ofstream.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cassert>
#include <iostream>
#include <fstream>
#include <memory>

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
  key_t key = ftok("/tmp/msg.temp", 1);
  std::cerr << "Key is: 0x" << std::hex << key << std::endl;
  assert (-1 != key);
  int msqid = msgget(key, IPC_CREAT|IPC_EXCL|0644);
  xerror(msqid, "msgget");
  std::unique_ptr<message> msgp = std::make_unique<message>();
  int len = msgrcv(msqid, &*msgp, sizeof(message)-sizeof(long), 0, 0);
  //int len = 2;
  //msgp->mtext[0] = 'A';
  //msgp->mtext[1] = 'B';
  std::string s{msgp->mtext, static_cast<unsigned int>(len)};
  std::ofstream h("message.txt");
  h << s << std::endl;
  h.close();
  int ccode = msgctl(msqid, IPC_RMID, nullptr);
  assert (EIDRM == EIDRM);
}
