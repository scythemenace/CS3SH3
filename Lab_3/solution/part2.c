#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>

#define NUM_THREADS 10

int amount = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t deposits_allowed;
sem_t withdrawals_allowed;

void *deposit(void *param);
void *withdraw(void *param);

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s transaction_amount\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int transaction_amount = atoi(argv[1]);
  pthread_t threads[NUM_THREADS];

  int num_deposit_threads = 7;
  int num_withdraw_threads = 3;

  // Initialize semaphores
  int initial_deposits_allowed = (400 - amount) / transaction_amount;
  int initial_withdrawals_allowed = amount / transaction_amount;

  sem_init(&deposits_allowed, 0, initial_deposits_allowed);
  sem_init(&withdrawals_allowed, 0, initial_withdrawals_allowed);

  // Create deposit threads
  for (int i = 0; i < num_deposit_threads; i++)
  {
    if (pthread_create(&threads[i], NULL, deposit, argv[1]) != 0)
    {
      perror("Failed to create deposit thread");
      exit(EXIT_FAILURE);
    }
  }

  // Create withdrawal threads
  for (int i = 0; i < num_withdraw_threads; i++)
  {
    if (pthread_create(&threads[num_deposit_threads + i], NULL, withdraw, argv[1]) != 0)
    {
      perror("Failed to create withdrawal thread");
      exit(EXIT_FAILURE);
    }
  }

  // Wait for all threads to finish
  for (int i = 0; i < NUM_THREADS; i++)
  {
    if (pthread_join(threads[i], NULL) != 0)
    {
      perror("Failed to join thread");
      exit(EXIT_FAILURE);
    }
  }

  // Destroy semaphores and mutex
  sem_destroy(&deposits_allowed);
  sem_destroy(&withdrawals_allowed);
  pthread_mutex_destroy(&mutex);

  printf("Final Amount = %d\n", amount);

  return 0;
}

void *deposit(void *param)
{
  printf("Executing deposit function\n");

  int deposit_amount = atoi(param);

  if (sem_wait(&deposits_allowed) != 0)
  {
    perror("sem_wait deposits_allowed");
    pthread_exit(NULL);
  }

  pthread_mutex_lock(&mutex);

  amount += deposit_amount;
  printf("Deposit: Amount = %d\n", amount);

  pthread_mutex_unlock(&mutex);

  if (sem_post(&withdrawals_allowed) != 0)
  {
    perror("sem_post withdrawals_allowed");
    pthread_exit(NULL);
  }

  pthread_exit(0);
}

void *withdraw(void *param)
{
  printf("Executing withdraw function\n");
  int withdraw_amount = atoi(param);

  if (sem_wait(&withdrawals_allowed) != 0)
  {
    perror("sem_wait withdrawals_allowed");
    pthread_exit(NULL);
  }

  pthread_mutex_lock(&mutex);

  amount -= withdraw_amount;
  printf("Withdraw: Amount = %d\n", amount);

  pthread_mutex_unlock(&mutex);

  if (sem_post(&deposits_allowed) != 0)
  {
    perror("sem_post deposits_allowed");
    pthread_exit(NULL);
  }

  pthread_exit(0);
}
