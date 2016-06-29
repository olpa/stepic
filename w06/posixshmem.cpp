#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string>
#include <cassert>

int main() {
  std::string sname{"/test.shm"};
  int shmid = shm_open(sname.c_str(), O_CREAT|O_EXCL|O_RDWR, 0777);
  assert (-1 != shmid);
  unsigned int mlen = 1024 * 1024;
  int ccode = ftruncate(shmid, mlen);
  if (-1 == ccode) {
    perror("ftruncate");
  }
  assert (-1 != ccode);
  void *buf = mmap(nullptr, mlen, PROT_READ|PROT_WRITE, MAP_SHARED, shmid, 0);
  assert ((void*)-1 != buf);
  int i = 0;
  for (unsigned char *p = static_cast<unsigned char*>(buf); i < mlen; i++) {
    *p++ = 13;
  }
  signal(SIGTERM, [](int){});
  pause();
  ccode = munmap(buf, mlen);
  assert (-1 != ccode);
  ccode = shm_unlink(sname.c_str());
  assert (-1 != ccode);
}
