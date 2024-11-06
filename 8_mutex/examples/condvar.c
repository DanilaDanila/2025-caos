#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <unistd.h>

// описание аргументов для функции
typedef struct {
  pthread_mutex_t *mutex;
  pthread_cond_t *condvar;

  volatile int64_t *next_out;
} gen_args_t;


void *do_work(void *args) {
  // параметры, которые мы получили
  gen_args_t *params = (gen_args_t*)args;

  pthread_mutex_lock(params->mutex);

  // итерируемся от 0 до +inf
  for (int64_t i=0; ; ++i) {
    // записываем следующий элемент
    *params->next_out = i;

    // будим основной поток
    pthread_cond_signal(params->condvar);

    // ждем, когда основной поток заберет элемент
    // (в этот момент мьютекс отпускается)
    while (*params->next_out != -1)
        pthread_cond_wait(params->condvar, params->mutex);
  }
}

int main(int argc, char **argv) {
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t condvar = PTHREAD_COND_INITIALIZER;


  volatile int64_t next_out;
  gen_args_t thread_args = {
      .mutex = &mutex,
      .condvar = &condvar,
      .next_out = &next_out,
  };

  pthread_t worker;
  pthread_create(&worker, NULL, do_work, &thread_args);

  next_out = -1;
  const int N = 10;

  pthread_mutex_lock(&mutex);
  for (int32_t count = 0; count < N; ++count) {
    // ждем следующего числа
    // (в этот момент мьютекс отпускается)
    while (next_out == -1)
      pthread_cond_wait(&condvar, &mutex);

    // печатаем то, что получили от функции
    printf("%lld\n", next_out);

    next_out = -1;  // говорим, что "забрали" элемент
    if (count != N - 1)
      pthread_cond_signal(&condvar);
  }

  // убиваем поток-генератор
  pthread_cancel(worker);

  pthread_cond_destroy(&condvar);
  pthread_mutex_destroy(&mutex);
  return 0;
}

