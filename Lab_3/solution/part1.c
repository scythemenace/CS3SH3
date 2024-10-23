#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define NUM_THREADS 6

int amount = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void *deposit(void *param);
void *withdraw(void *param);

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    printf("Usage: %s deposit_amount withdraw_amount\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  pthread_t runners[NUM_THREADS];
  pthread_attr_t attr;
  int ret;

  ret = pthread_attr_init(&attr);
  if (ret != 0)
  {
    printf("pthread_attr_init failed: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  ret = pthread_create(&runners[0], &attr, withdraw, argv[2]);
  if (ret != 0)
  {
    printf("pthread_create failed for thread 0: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  ret = pthread_create(&runners[1], &attr, withdraw, argv[2]);
  if (ret != 0)
  {
    printf("pthread_create failed for thread 1: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  ret = pthread_create(&runners[2], &attr, withdraw, argv[2]);
  if (ret != 0)
  {
    printf("pthread_create failed for thread 2: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  ret = pthread_create(&runners[3], &attr, deposit, argv[1]);
  if (ret != 0)
  {
    printf("pthread_create failed for thread 3: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  ret = pthread_create(&runners[4], &attr, deposit, argv[1]);
  if (ret != 0)
  {
    printf("pthread_create failed for thread 4: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  ret = pthread_create(&runners[5], &attr, deposit, argv[1]);
  if (ret != 0)
  {
    printf("pthread_create failed for thread 5: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < NUM_THREADS; i++)
  {
    ret = pthread_join(runners[i], NULL);
    if (ret != 0)
    {
      printf("pthread_join failed for thread %d: %s\n", i, strerror(ret));
      exit(EXIT_FAILURE);
    }
  }

  ret = pthread_attr_destroy(&attr);
  if (ret != 0)
  {
    printf("pthread_attr_destroy failed: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  ret = pthread_mutex_destroy(&mutex);
  if (ret != 0)
  {
    printf("pthread_mutex_destroy failed: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  printf("Final Amount = %d\n", amount);

  return 0;
}

void *deposit(void *param)
{
  int deposit_amount = atoi(param);
  int ret;

  ret = pthread_mutex_lock(&mutex);
  if (ret != 0)
  {
    printf("pthread_mutex_lock failed in deposit: %s\n", strerror(ret));
    pthread_exit(NULL);
  }

  amount += deposit_amount;
  printf("Deposit Amount = %d\n", amount);

  ret = pthread_mutex_unlock(&mutex);
  if (ret != 0)
  {
    printf("pthread_mutex_unlock failed in deposit: %s\n", strerror(ret));
    pthread_exit(NULL);
  }

  pthread_exit(0);
}

void *withdraw(void *param)
{
  int withdraw_amount = atoi(param);
  int ret;

  ret = pthread_mutex_lock(&mutex);
  if (ret != 0)
  {
    printf("pthread_mutex_lock failed in withdraw: %s\n", strerror(ret));
    pthread_exit(NULL);
  }

  amount -= withdraw_amount;
  printf("Withdraw Amount = %d\n", amount);

  ret = pthread_mutex_unlock(&mutex);
  if (ret != 0)
  {
    printf("pthread_mutex_unlock failed in withdraw: %s\n", strerror(ret));
    pthread_exit(NULL);
  }

  pthread_exit(0);
}
