#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

#define NUM_THREADS 6

int amount = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t below_zero;
sem_t above_limit;
void *deposit(void *param);
void *withdraw(void *param);

int main(int argc, char *argv[])
{
  pthread_t runners[NUM_THREADS];
  sem_init(&below_zero, 0, 0);
  sem_init(&above_limit, 0, 1);

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
  sem_destroy(&below_zero);
  sem_destroy(&above_limit);

  printf("Final Amount = %d\n", amount);

  return 0;
}

void *deposit(void *param)
{
  int deposit_amount = atoi(param);
  sem_wait(&above_limit);

  pthread_mutex_lock(&mutex);
  if(amount + deposit_amount <= 400) {
    amount+= deposit_amount;
  }

  if (amount > 0) {
    sem_post(&below_zero);
  }

  if (amount < 400) {
    sem_post(&above_limit);
  }
  printf("Deposit Amount = %d\n", amount);
  pthread_mutex_unlock(&mutex);
  sem_post(&above_limit);

  pthread_exit(0);
}

void *withdraw(void *param)
{
  int withdraw_amount = atoi(param);
  sem_wait(&below_zero);

  pthread_mutex_lock(&mutex);
  if (amount - withdraw_amount >= 0) {
    amount -= withdraw_amount; 
  }

  if (amount > 0) {
    sem_post(&below_zero);
  }

  if (amount < 400) {
    sem_post(&above_limit);
  }

  printf("Withdraw Amount = %d\n", amount);
  pthread_mutex_unlock(&mutex);

  pthread_exit(0);
}
