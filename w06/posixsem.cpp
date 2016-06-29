#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <string>
#include <cassert>

int main() {
  std::string sname{"/test.sem"};
  sem_t *semid = sem_open(sname.c_str(), O_CREAT|O_EXCL, 0777, 66);
  assert (SEM_FAILED != semid);
  signal(SIGTERM, [](int){});
  pause();
  int ccode = sem_close(semid);
  assert (-1 != ccode);
  ccode = sem_unlink(sname.c_str());
  assert (-1 != ccode);
}
