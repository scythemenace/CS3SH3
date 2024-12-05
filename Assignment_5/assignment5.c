// FCFS Scheduling Algorithm
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
void FCFS(int arr[], int head, int size);
int findMinSeekTimeIndex(int arr[], bool visited[], int numRequests, int currentHead);
void SSTF(int arr[], int head, int size);
int compare(const void *a, const void *b);
void SCAN(int *arr, int n, int head, bool direction);
void CSCAN(int *arr, int n, int head, int disk_size);
void LOOK(int *arr, int head, int size);
void CLOOK(int arr[], int head, int size);

int main()
{
  int arr[8] = {176, 79, 34, 60, 92, 11, 41, 114};
  int head = 50;
  int size = sizeof(arr) / sizeof(arr[0]);

  FCFS(arr, head, size);
  SSTF(arr, head, size);
  SCAN(arr, size, head, false); // Left direction
  CSCAN(arr, size, head, 200);
  LOOK(arr, head, size);
  CLOOK(arr, head, size);

  return 0;
}

void FCFS(int arr[], int head, int size)
{

  int head_movements = 0;
  int cur_req, distance;

  for (int i = 0; i < size; i++)
  {
    cur_req = arr[i];

    // calculate absolute distance
    distance = abs(head - cur_req);

    head_movements += distance;

    // accessed track is now new head
    head = cur_req;
  }

  printf("Total number of head movements: %d\n", head_movements);
}

// Function to find the index of request with minimum seek time
int findMinSeekTimeIndex(int arr[], bool visited[], int numRequests, int currentHead)
{
  int minSeekTime = INT_MAX;
  int minIndex = -1;

  for (int i = 0; i < numRequests; i++)
  {
    // If request not already serviced
    if (!visited[i])
    {
      // Calculate absolute seek distance
      int seekTime = abs(arr[i] - currentHead);

      // Update minimum if this seek time is smaller
      if (seekTime < minSeekTime)
      {
        minSeekTime = seekTime;
        minIndex = i;
      }
    }
  }

  return minIndex;
}

// SSTF Disk Scheduling Algorithm
void SSTF(int arr[], int head, int size)
{
  // Array to track visitedarr
  bool *visited = (bool *)calloc(size, sizeof(bool));

  // Total seek operations
  int head_movements = 0;

  // Current head position
  int currentHead = head;

  // Service allarr
  for (int count = 0; count < size; count++)
  {
    // Find request with minimum seek time
    int nextRequest = findMinSeekTimeIndex(arr, visited, size, currentHead);

    if (nextRequest == -1)
    {
      break; // No more unservicedarr
    }

    // Calculate and add seek operations
    head_movements += abs(arr[nextRequest] - currentHead);

    // Update current head position
    currentHead = arr[nextRequest];

    // Mark request as visited
    visited[nextRequest] = true;
  }

  // Free dynamically allocated memory
  free(visited);

  printf("Total number of head movements: %d\n", head_movements);
}

int compare(const void *a, const void *b)
{
  return (*(int *)a - *(int *)b);
}

void SCAN(int *arr, int n, int head, bool direction)
{
  // If no arr, return 0
  if (n == 0)
    return;

  // Sort the request array
  qsort(arr, n, sizeof(int), compare);

  int head_movements = 0;
  int curr_req = head;

  // Find the position where head is located in the sorted array
  int index = 0;
  for (index = 0; index < n; index++)
  {
    if (arr[index] >= head)
      break;
  }

  // If direction is true, move right (increasing)
  // If direction is false, move left (decreasing)
  if (direction)
  {
    // First, service arr on the right side
    for (int i = index; i < n; i++)
    {
      head_movements += abs(curr_req - arr[i]);
      curr_req = arr[i];
    }

    // If there were arr on the right, go to the last track
    if (n > index)
    {
      head_movements += abs(curr_req - (n * 200 - 1)); // Assuming track range is 0-199
      curr_req = n * 200 - 1;
    }

    // Then service arr on the left side
    for (int i = index - 1; i >= 0; i--)
    {
      head_movements += abs(curr_req - arr[i]);
      curr_req = arr[i];
    }
  }
  else
  {
    // First, service arr on the left side
    for (int i = index - 1; i >= 0; i--)
    {
      head_movements += abs(curr_req - arr[i]);
      curr_req = arr[i];
    }

    // If there were arr on the left, go to track 0
    if (index > 0)
    {
      head_movements += abs(curr_req - 0);
      curr_req = 0;
    }

    // Then service arr on the right side
    for (int i = index; i < n; i++)
    {
      head_movements += abs(curr_req - arr[i]);
      curr_req = arr[i];
    }
  }

  printf("Total number of head movements: %d\n", head_movements);
}

// C-SCAN (Circular Scan) Disk Scheduling Algorithm
void CSCAN(int *arr, int n, int head, int disk_size)
{
  int head_movements = 0;
  int curr_req = head;

  // Create a copy of the arr array to avoid modifying the original
  int *sorted_requests = malloc(n * sizeof(int));
  for (int i = 0; i < n; i++)
  {
    sorted_requests[i] = arr[i];
  }

  // Sort the arr in ascending order
  qsort(sorted_requests, n, sizeof(int), compare);

  // Find the index where the head is located in the sorted array
  int head_index = 0;
  while (head_index < n && sorted_requests[head_index] < head)
  {
    head_index++;
  }

  // First, scan to the right of the head
  for (int i = head_index; i < n; i++)
  {
    head_movements += abs(curr_req - sorted_requests[i]);
    curr_req = sorted_requests[i];
  }

  // If we've reached the end of the disk, move to the beginning
  if (curr_req != disk_size - 1)
  {
    // Add seek to the end of the disk
    head_movements += abs(curr_req - (disk_size - 1));
    curr_req = disk_size - 1;

    // Add seek to the beginning of the disk
    head_movements += (disk_size - 1);
    curr_req = 0;
  }

  // Scan from the beginning to the point just before the head
  for (int i = 0; i < head_index; i++)
  {
    head_movements += abs(curr_req - sorted_requests[i]);
    curr_req = sorted_requests[i];
  }

  // Free the dynamically allocated memory
  free(sorted_requests);

  printf("Total number of head movements: %d\n", head_movements);
}

// LOOK Disk Scheduling Algorithm
void LOOK(int *arr, int head, int size)
{
  // If no arr, return 0
  if (size == 0)
    return;

  // Create a copy of the arr array to sort
  int *sorted_requests = malloc(size * sizeof(int));
  for (int i = 0; i < size; i++)
  {
    sorted_requests[i] = arr[i];
  }

  // Sort the arr in ascending order
  qsort(sorted_requests, size, sizeof(int), compare);

  // Find the position where head would be inserted in sorted array
  int head_index = 0;
  while (head_index < size && sorted_requests[head_index] < head)
  {
    head_index++;
  }

  int head_movements = 0;
  int current_position = head;

  // Move to the right first
  for (int i = head_index; i < size; i++)
  {
    head_movements += abs(current_position - sorted_requests[i]);
    current_position = sorted_requests[i];
  }

  // If we reached the end, change direction and move to the left
  if (head_index > 0)
  {
    // Move to the right-most element on right side
    int right_most = sorted_requests[size - 1];

    // Move to the left
    for (int i = head_index - 1; i >= 0; i--)
    {
      head_movements += abs(current_position - sorted_requests[i]);
      current_position = sorted_requests[i];
    }
  }

  // Free the dynamically allocated memory
  free(sorted_requests);

  printf("Total number of head movements: %d\n", head_movements);
}

void CLOOK(int arr[], int head, int size)
{
  // Sort the request array using qsort
  qsort(arr, size, sizeof(int), compare);

  // Find the index where the head is located
  int index = 0;
  for (int i = 0; i < size; i++)
  {
    if (arr[i] >= head)
    {
      index = i;
      break;
    }
  }

  int head_movements = 0;
  int curr_track = head;

  // Move to the right (increasing track numbers)
  for (int i = index; i < size; i++)
  {
    head_movements += abs(curr_track - arr[i]);
    curr_track = arr[i];
  }

  // Jump to the smallest track
  if (size > 0)
  {
    head_movements += abs(curr_track - arr[0]);
    curr_track = arr[0];
  }

  // Continue moving to the right up to the index before the head
  for (int i = 0; i < index; i++)
  {
    head_movements += abs(curr_track - arr[i]);
    curr_track = arr[i];
  }

  printf("Total number of seek operations = %d\n", head_movements);
}
