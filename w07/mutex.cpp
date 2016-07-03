#include <fstream>
#include <cassert>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <iostream>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread_mutex(void *) {
  int ccode = pthread_mutex_lock(&mutex);
  assert (! ccode);
  ccode = pthread_mutex_unlock(&mutex);
  assert (! ccode);
  std::cout << "End: mutex" << std::endl;
}

pthread_spinlock_t spinlock;

void *thread_spin(void *) {
  int ccode = pthread_spin_lock(&spinlock);
  assert (! ccode);
  ccode = pthread_spin_unlock(&spinlock);
  assert (! ccode);
  std::cout << "End: spin" << std::endl;
}

pthread_rwlock_t rwlock3 = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t rwlock4 = PTHREAD_RWLOCK_INITIALIZER;

void *thread_block_r(void *) {
  int ccode = pthread_rwlock_rdlock(&rwlock3);
  assert (! ccode);
  ccode = pthread_rwlock_unlock(&rwlock3);
  assert (! ccode);
  std::cout << "End: block R" << std::endl;
}

void *thread_block_w(void *) {
  int ccode = pthread_rwlock_wrlock(&rwlock4);
  assert (! ccode);
  ccode = pthread_rwlock_unlock(&rwlock4);
  assert (! ccode);
  std::cout << "End: block W" << std::endl;
}

int main() {
  {
    std::ofstream os{"main.pid"};
    assert(os);
    os << getpid() << std::endl;
  }
  pthread_t thread1, thread2, thread3, thread4;
  int arg = 0;
  int *parg = &arg;
  //
  int ccode = pthread_mutex_lock(&mutex);
  assert (! ccode);
  ccode = pthread_create(&thread1, nullptr, thread_mutex, &arg);
  assert (! ccode);
  //
  ccode = pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
  assert (! ccode);
  ccode = pthread_spin_lock(&spinlock);
  assert (! ccode);
  ccode = pthread_create(&thread2, nullptr, thread_spin, &arg);
  assert (! ccode);
  //
  ccode = pthread_rwlock_wrlock(&rwlock3);
  assert (! ccode);
  ccode = pthread_rwlock_rdlock(&rwlock4);
  assert (! ccode);
  ccode = pthread_create(&thread3, nullptr, thread_block_r, &arg);
  assert (! ccode);
  ccode = pthread_create(&thread4, nullptr, thread_block_w, &arg);
  assert (! ccode);
  //
  signal(SIGINT, [](int){});
  pause();
  //
  ccode = pthread_rwlock_unlock(&rwlock4);
  assert (! ccode);
  ccode = pthread_rwlock_unlock(&rwlock3);
  assert (! ccode);
  pthread_join(thread4, reinterpret_cast<void**>(&parg));
  pthread_join(thread3, reinterpret_cast<void**>(&parg));
  ccode = pthread_rwlock_destroy(&rwlock4);
  ccode = pthread_rwlock_destroy(&rwlock3);
  assert (! ccode);
  //
  ccode = pthread_spin_unlock(&spinlock);
  assert (! ccode);
  pthread_join(thread2, reinterpret_cast<void**>(&parg));
  ccode = pthread_spin_destroy(&spinlock);
  assert (! ccode);
  //
  ccode = pthread_mutex_unlock(&mutex);
  assert (! ccode);
  pthread_join(thread1, reinterpret_cast<void**>(&parg));
  ccode = pthread_mutex_destroy(&mutex);
  if (ccode) {
    std::cout << "pthread_mutex_destroy error " << ccode << std::endl;
    perror("pthread_mutex_destroy");
    assert (! ccode);
  }
}
