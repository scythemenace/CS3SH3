#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 2
#define SIZE 20

typedef struct {
  int from_index;
  int to_index;
} parameters;

int list[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
int total_sum = 0;
void *summing1(void *param);
void *summing2(void *param);

int main()
{
  parameters *data1 = (parameters *) malloc (sizeof(parameters));
  parameters *data2 = (parameters *) malloc (sizeof(parameters));
  data1->from_index = 0;
  data1->to_index = (SIZE / 2);
  data2->from_index = (SIZE / 2); 
  data2->to_index = SIZE;

  pthread_t workers[NUM_THREADS];

  pthread_attr_t attr;
  pthread_attr_init(&attr);

  pthread_create(&workers[0], &attr, summing1, data1);
  pthread_create(&workers[1], &attr, summing2, data2);

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(workers[i], NULL);
  }

  printf("%d\n", total_sum);

  pthread_attr_destroy(&attr);

  return 0;
}

void *summing1 (void *param) {
  parameters *data = (parameters *)param;

  for (int i = data->from_index; i < data->to_index; i++) {
    total_sum += list[i];
  }

  free(data);

  pthread_exit(0);
}

void *summing2 (void *param) {
  parameters *data = (parameters *)param;

  for (int i = data->from_index; i < data->to_index; i++) {
    total_sum += list[i];
  }

  free(data);

  pthread_exit(0);
}
