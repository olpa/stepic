#include <fstream>
#include <cassert>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

void *thread_func(void *) {
  signal(SIGINT, [](int){});
  pause();
}

int main() {
  {
    std::ofstream os{"main.pid"};
    assert(os);
    os << getpid() << std::endl;
  }
  pthread_t thread;
  int arg = 0;
  int *parg = &arg;
  int ccode = pthread_create(&thread, nullptr, thread_func, &arg);
  assert (! ccode);
  signal(SIGINT, [](int){});
  pthread_join(thread, reinterpret_cast<void**>(&parg));
}
