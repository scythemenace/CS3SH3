#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 6

int amount = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void *deposit(void *param);
void *withdraw(void *param);

int main(int argc, char *argv[])
{
  pthread_t runners[NUM_THREADS];

  pthread_attr_t attr;
  pthread_attr_init(&attr);

  pthread_create(&runners[0], &attr, withdraw, argv[2]);
  pthread_create(&runners[1], &attr, withdraw, argv[2]);
  pthread_create(&runners[2], &attr, withdraw, argv[2]);
  pthread_create(&runners[3], &attr, deposit, argv[1]);
  pthread_create(&runners[4], &attr, deposit, argv[1]);
  pthread_create(&runners[5], &attr, deposit, argv[1]);
 
  for (int i = 0; i < NUM_THREADS; i++)
  {
    pthread_join(runners[i], NULL);
  }

  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&mutex);

  printf("Final Amount = %d\n", amount);

  return 0;
}

void *deposit(void *param)
{
  int deposit_amount = atoi(param);

  pthread_mutex_lock(&mutex);
  amount += deposit_amount; 
  printf("Deposit Amount = %d\n", amount);
  pthread_mutex_unlock(&mutex);

  pthread_exit(0);
}

void *withdraw(void *param)
{
  int withdraw_amount = atoi(param);

  pthread_mutex_lock(&mutex);
  amount -= withdraw_amount; 
  printf("Withdraw Amount = %d\n", amount);
  pthread_mutex_unlock(&mutex);

  pthread_exit(0);
}
