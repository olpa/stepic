#include <signal.h>
#include <unistd.h>
#include <string>
#include <cassert>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <array>
#include <algorithm>

int main() {
  key_t key = ftok("/tmp/mem.temp", 1);
  assert (-1 != key);
  const int mlen = 1 * 1024 * 1024;
  int shmid = shmget(key, mlen, IPC_CREAT|IPC_EXCL|0777);
  assert (-1 != shmid);
  void *buf = shmat(shmid, nullptr, 0);
  assert ((void*)-1 != buf);
  std::array<unsigned char, mlen> data;
  std::fill(std::begin(data), std::end(data), 42);
  signal(SIGTERM, [](int){});
  pause();
  int ccode = shmdt(buf);
  assert (-1 != ccode);
  ccode = shmctl(shmid, IPC_RMID, nullptr);
  assert (-1 != ccode);
}
