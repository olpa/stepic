#include <iostream>
#include <fstream>
#include <thread>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <signal.h>

void cond_waiter(pthread_cond_t &cond) {
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_wait(&cond, &mutex);
  int ccode = pthread_mutex_destroy(&mutex);
  assert (! ccode);
  std::cout << "Cond waiter ended" << std::endl;
}

void barrier_waiter(pthread_barrier_t &bp) {
  int ccode = pthread_barrier_wait(&bp);
  assert (EINVAL != ccode);
  std::cout << "Barrier waiter ended" << std::endl;
}

int main() {
  int ccode;
  {
    std::ofstream os{"main.pid"};
    assert(os);
    os << getpid() << std::endl;
  }
  //
  pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
  std::thread t1(cond_waiter, std::ref(cond));
  //
  pthread_barrier_t bp;
  ccode = pthread_barrier_init(&bp, nullptr, 2);
  assert (! ccode);
  std::thread t2(barrier_waiter, std::ref(bp));
  //
  signal(SIGINT, [](int){});
  pause();
  //
  ccode = pthread_barrier_wait(&bp);
  assert (EINVAL != ccode);
  t2.join();
  ccode = pthread_barrier_destroy(&bp);
  assert (! ccode);
  //
  ccode = pthread_cond_signal(&cond);
  assert (! ccode);
  t1.join();
  ccode = pthread_cond_destroy(&cond);
  assert (! ccode);
}
