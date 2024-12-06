#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#define MAX 1000

// Utility function to compare two integers for qsort
int compare(const void *a, const void *b)
{
  return (*(int *)a - *(int *)b);
}

// Read requests from binary file
int read_requests(const char *filename, int requests[])
{
  FILE *file = fopen(filename, "rb");
  if (!file)
  {
    perror("Error opening file");
    return -1;
  }
  int count = fread(requests, sizeof(int), MAX, file);
  fclose(file);
  return count;
}

// FCFS Algorithm
void FCFS(int arr[], int size, int head)
{
  int seek_count = 0;
  int cur_track, distance;

  printf("Seek Sequence: ");
  for (int i = 0; i < size; i++)
  {
    cur_track = arr[i];
    distance = abs(head - cur_track);
    seek_count += distance;
    head = cur_track;
    printf("%d ", cur_track);
  }
  printf("\nTotal head movements: %d\n", seek_count);
}

// SSTF Algorithm
void SSTF(int request[], int size, int head)
{
  if (size == 0)
  {
    printf("No requests to process.\n");
    return;
  }

  int diff[size][2];
  int seek_count = 0;
  int seek_sequence[size];
  int processed[size];

  memset(processed, 0, sizeof(processed));

  printf("Seek Sequence: ");
  for (int i = 0; i < size; i++)
  {
    for (int j = 0; j < size; j++)
    {
      if (!processed[j])
      {
        diff[j][0] = abs(head - request[j]);
        diff[j][1] = j;
      }
      else
      {
        diff[j][0] = INT_MAX;
      }
    }
    int min_index = -1;
    int min_distance = INT_MAX;
    for (int j = 0; j < size; j++)
    {
      if (diff[j][0] < min_distance)
      {
        min_distance = diff[j][0];
        min_index = diff[j][1];
      }
    }
    processed[min_index] = 1;
    seek_count += min_distance;
    head = request[min_index];
    seek_sequence[i] = head;
    printf("%d ", head);
  }
  printf("\nTotal head movements: %d\n", seek_count);
}

// SCAN Algorithm
void SCAN(int arr[], int size, int head, char direction[], int disk_size)
{
  int seek_count = 0;
  int left[MAX], right[MAX], seek_sequence[MAX];
  int left_size = 0, right_size = 0, seek_seq_size = 0;

  if (strcmp(direction, "LEFT") == 0)
    left[left_size++] = 0;
  else if (strcmp(direction, "RIGHT") == 0)
    right[right_size++] = disk_size - 1;

  for (int i = 0; i < size; i++)
  {
    if (arr[i] < head)
      left[left_size++] = arr[i];
    else
      right[right_size++] = arr[i];
  }

  qsort(left, left_size, sizeof(int), compare);
  qsort(right, right_size, sizeof(int), compare);

  if (strcmp(direction, "LEFT") == 0)
  {
    for (int i = left_size - 1; i >= 0; i--)
    {
      seek_sequence[seek_seq_size++] = left[i];
      seek_count += abs(head - left[i]);
      head = left[i];
    }
    for (int i = 0; i < right_size; i++)
    {
      seek_sequence[seek_seq_size++] = right[i];
      seek_count += abs(head - right[i]);
      head = right[i];
    }
  }
  else
  {
    for (int i = 0; i < right_size; i++)
    {
      seek_sequence[seek_seq_size++] = right[i];
      seek_count += abs(head - right[i]);
      head = right[i];
    }
    for (int i = left_size - 1; i >= 0; i--)
    {
      seek_sequence[seek_seq_size++] = left[i];
      seek_count += abs(head - left[i]);
      head = left[i];
    }
  }

  printf("Seek Sequence: ");
  for (int i = 0; i < seek_seq_size; i++)
  {
    printf("%d ", seek_sequence[i]);
  }
  printf("\nTotal head movements: %d\n", seek_count);
}

void CSCAN(int arr[], int size, int head, int disk_size, char direction[])
{
  int seek_count = 0;
  int left[MAX], right[MAX];
  int left_size = 0, right_size = 0;

  // Add boundaries
  left[left_size++] = 0;
  right[right_size++] = disk_size - 1;

  // Split requests into left and right
  for (int i = 0; i < size; i++)
  {
    if (arr[i] < head)
      left[left_size++] = arr[i];
    if (arr[i] > head)
      right[right_size++] = arr[i];
  }

  // Sort the arrays
  qsort(left, left_size, sizeof(int), compare);
  qsort(right, right_size, sizeof(int), compare);

  printf("Seek Sequence: ");

  if (strcmp(direction, "RIGHT") == 0)
  {
    // Process the right side of the head first
    for (int i = 0; i < right_size; i++)
    {
      printf("%d ", right[i]);
      seek_count += abs(head - right[i]);
      head = right[i];
    }

    // Jump from the end of the disk to the start
    seek_count += abs(disk_size - 1 - 0);
    head = 0;

    // Process the left side
    for (int i = 0; i < left_size; i++)
    {
      printf("%d ", left[i]);
      seek_count += abs(head - left[i]);
      head = left[i];
    }
  }
  else if (strcmp(direction, "LEFT") == 0)
  {
    // Process the left side of the head first
    for (int i = left_size - 1; i >= 0; i--)
    {
      printf("%d ", left[i]);
      seek_count += abs(head - left[i]);
      head = left[i];
    }

    // Jump from the start of the disk to the end
    seek_count += abs(disk_size - 1);
    head = disk_size - 1;

    // Process the right side
    for (int i = right_size - 1; i >= 0; i--)
    {
      printf("%d ", right[i]);
      seek_count += abs(head - right[i]);
      head = right[i];
    }
  }

  printf("\nTotal head movements: %d\n", seek_count);
}

// LOOK Algorithm
void LOOK(int arr[], int size, int head, char direction[])
{
  int seek_count = 0;
  int left[MAX], right[MAX], seek_sequence[MAX];
  int left_size = 0, right_size = 0, seek_seq_size = 0;

  for (int i = 0; i < size; i++)
  {
    if (arr[i] < head)
      left[left_size++] = arr[i];
    else
      right[right_size++] = arr[i];
  }

  qsort(left, left_size, sizeof(int), compare);
  qsort(right, right_size, sizeof(int), compare);

  if (strcmp(direction, "LEFT") == 0)
  {
    for (int i = left_size - 1; i >= 0; i--)
    {
      seek_sequence[seek_seq_size++] = left[i];
      seek_count += abs(head - left[i]);
      head = left[i];
    }
    for (int i = 0; i < right_size; i++)
    {
      seek_sequence[seek_seq_size++] = right[i];
      seek_count += abs(head - right[i]);
      head = right[i];
    }
  }
  else
  {
    for (int i = 0; i < right_size; i++)
    {
      seek_sequence[seek_seq_size++] = right[i];
      seek_count += abs(head - right[i]);
      head = right[i];
    }
    for (int i = left_size - 1; i >= 0; i--)
    {
      seek_sequence[seek_seq_size++] = left[i];
      seek_count += abs(head - left[i]);
      head = left[i];
    }
  }

  printf("Seek Sequence: ");
  for (int i = 0; i < seek_seq_size; i++)
  {
    printf("%d ", seek_sequence[i]);
  }
  printf("\nTotal head movements: %d\n", seek_count);
}

// CLOOK Algorithm
void CLOOK(int arr[], int size, int head)
{
  int seek_count = 0;
  int left[MAX], right[MAX], seek_sequence[MAX];
  int left_size = 0, right_size = 0, seek_seq_size = 0;

  for (int i = 0; i < size; i++)
  {
    if (arr[i] < head)
      left[left_size++] = arr[i];
    else
      right[right_size++] = arr[i];
  }

  qsort(left, left_size, sizeof(int), compare);
  qsort(right, right_size, sizeof(int), compare);

  for (int i = 0; i < right_size; i++)
  {
    seek_sequence[seek_seq_size++] = right[i];
    seek_count += abs(head - right[i]);
    head = right[i];
  }

  for (int i = 0; i < left_size; i++)
  {
    seek_sequence[seek_seq_size++] = left[i];
    seek_count += abs(head - left[i]);
    head = left[i];
  }

  printf("Seek Sequence: ");
  for (int i = 0; i < seek_seq_size; i++)
  {
    printf("%d ", seek_sequence[i]);
  }
  printf("\nTotal head movements: %d\n", seek_count);
}

// Main Function
int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("Usage: ./assignment5 <initial_head_position> <direction>\n");
    return 1;
  }

  int head = atoi(argv[1]);
  if (head < 0 || head > 299)
  {
    printf("Initial head position must be between 0 and 299.\n");
    return 1;
  }

  char direction[10];
  strcpy(direction, argv[2]);
  if (strcmp(direction, "LEFT") != 0 && strcmp(direction, "RIGHT") != 0)
  {
    printf("Direction must be either 'LEFT' or 'RIGHT'.\n");
    return 1;
  }

  int requests[MAX];
  int count = read_requests("request.bin", requests);
  if (count < 0)
  {
    return 1;
  }

  printf("\nFCFS DISK SCHEDULING ALGORITHM:\n");
  FCFS(requests, count, head);

  printf("\nSSTF DISK SCHEDULING ALGORITHM:\n");
  SSTF(requests, count, head);

  printf("\nSCAN DISK SCHEDULING ALGORITHM:\n");
  SCAN(requests, count, head, direction, 300);

  printf("\nC-SCAN DISK SCHEDULING ALGORITHM:\n");
  CSCAN(requests, count, head, 300, direction);

  printf("\nLOOK DISK SCHEDULING ALGORITHM:\n");
  LOOK(requests, count, head, direction);

  printf("\nC-LOOK DISK SCHEDULING ALGORITHM:\n");
  CLOOK(requests, count, head);

  return 0;
}
