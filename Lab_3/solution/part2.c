#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>

#define NUM_THREADS 10

int amount = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t below_zero;
sem_t above_limit;

void *deposit(void *param);
void *withdraw(void *param);

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("Usage: %s amount\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  pthread_t runners[NUM_THREADS];
  int ret;

  ret = sem_init(&below_zero, 0, 0);
  if (ret != 0)
  {
    printf("sem_init for below_zero failed");
    exit(EXIT_FAILURE);
  }

  ret = sem_init(&above_limit, 0, 1);
  if (ret != 0)
  {
    printf("sem_init for above_limit failed");
    exit(EXIT_FAILURE);
  }

  pthread_attr_t attr;
  ret = pthread_attr_init(&attr);
  if (ret != 0)
  {
    printf("pthread_attr_init failed: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  ret = pthread_create(&runners[0], &attr, withdraw, argv[1]);
  if (ret != 0)
  {
    printf("pthread_create failed for thread 0: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  ret = pthread_create(&runners[1], &attr, withdraw, argv[1]);
  if (ret != 0)
  {
    printf("pthread_create failed for thread 1: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  ret = pthread_create(&runners[2], &attr, withdraw, argv[1]);
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

  ret = pthread_create(&runners[6], &attr, deposit, argv[1]);
  if (ret != 0)
  {
    printf("pthread_create failed for thread 6: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  ret = pthread_create(&runners[7], &attr, deposit, argv[1]);
  if (ret != 0)
  {
    printf("pthread_create failed for thread 7: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  ret = pthread_create(&runners[8], &attr, deposit, argv[1]);
  if (ret != 0)
  {
    printf("pthread_create failed for thread 8: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  ret = pthread_create(&runners[9], &attr, deposit, argv[1]);
  if (ret != 0)
  {
    printf("pthread_create failed for thread 9: %s\n", strerror(ret));
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

  ret = sem_destroy(&below_zero);
  if (ret != 0)
  {
    printf("sem_destroy for below_zero failed");
    exit(EXIT_FAILURE);
  }

  ret = sem_destroy(&above_limit);
  if (ret != 0)
  {
    printf("sem_destroy for above_limit failed");
    exit(EXIT_FAILURE);
  }

  printf("Final Amount = %d\n", amount);

  return 0;
}

void *deposit(void *param)
{
  // printf("Executing deposit function\n");

  int deposit_amount = atoi(param);
  int ret;

  ret = pthread_mutex_lock(&mutex);
  if (ret != 0)
  {
    printf("pthread_mutex_lock failed in deposit: %s\n", strerror(ret));
    pthread_exit(NULL);
  }

  if (amount + deposit_amount <= 400)
  {
    amount += deposit_amount;

    if (amount > 0)
    {
      ret = sem_post(&below_zero);
      if (ret != 0)
      {
        printf("sem_post on below_zero failed in deposit");
      }
    }

    if (amount < 400)
    {
      ret = sem_post(&above_limit);
      if (ret != 0)
      {
        printf("sem_post on above_limit failed in deposit");
      }
    }
  }
  else
  {
    ret = sem_post(&above_limit);
    if (ret != 0)
    {
      printf("sem_post on above_limit failed in deposit");
    }
  }

  printf("Deposit Amount = %d\n", amount);

  ret = pthread_mutex_unlock(&mutex);
  if (ret != 0)
  {
    printf("pthread_mutex_unlock failed in deposit: %s\n", strerror(ret));
    pthread_exit(NULL);
  }

  ret = sem_wait(&above_limit);
  if (ret != 0)
  {
    printf("sem_wait on above_limit failed in deposit");
    pthread_exit(NULL);
  }

  pthread_exit(0);
}

void *withdraw(void *param)
{
  // printf("Executing withdraw function\n");
  int withdraw_amount = atoi(param);
  int ret;

  ret = pthread_mutex_lock(&mutex);
  if (ret != 0)
  {
    printf("pthread_mutex_lock failed in withdraw: %s\n", strerror(ret));
    pthread_exit(NULL);
  }

  if (amount - withdraw_amount >= 0)
  {
    amount -= withdraw_amount;
  }

  if (amount > 0)
  {
    ret = sem_post(&below_zero);
    if (ret != 0)
    {
      printf("sem_post on below_zero failed in withdraw");
    }
  }

  if (amount < 400)
  {
    ret = sem_post(&above_limit);
    if (ret != 0)
    {
      printf("sem_post on above_limit failed in withdraw");
    }
  }

  printf("Withdraw Amount = %d\n", amount);

  ret = pthread_mutex_unlock(&mutex);
  if (ret != 0)
  {
    printf("pthread_mutex_unlock failed in withdraw: %s\n", strerror(ret));
    pthread_exit(NULL);
  }

  ret = sem_wait(&below_zero);
  if (ret != 0)
  {
    printf("sem_wait on below_zero failed in withdraw");
    pthread_exit(NULL);
  }

  pthread_exit(0);
}
