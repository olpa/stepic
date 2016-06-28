#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <cassert>
#include <vector>
#include <array>
#include <numeric>
#include <unistd.h>

const unsigned int nsem = 16;

int main() {
  key_t key = ftok("/tmp/sem.temp", 1);
  assert (-1 != key);
  int semid = semget(key, nsem, IPC_CREAT|IPC_EXCL|0777);
  assert (-1 != semid);
  std::array<unsigned short, nsem> vals;
  std::iota(std::begin(vals), std::end(vals), 0);
  int ccode = semctl(semid, 0, SETALL, vals.data());
  pause();
  ccode = semctl(semid, 0, IPC_RMID);
  assert (-1 != ccode);
}
