#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>

#define MAX_REQUESTS 20
#define DISK_SIZE 300

void FCFS(int arr[], int head, int size);
int findMinSeekTimeIndex(int arr[], bool visited[], int numRequests, int currentHead);
void SSTF(int arr[], int head, int size);
int compare(const void *a, const void *b);
void SCAN(int *arr, int n, int head, bool direction);
void CSCAN(int *arr, int n, int head, int disk_size, char direction[]);
void LOOK(int *arr, int head, int size);
void CLOOK(int arr[], int head, int size);

// Function to read requests from binary file
int readRequestsFromBinary(int *requests, const char *filename)
{
  FILE *file = fopen("request.bin", "rb");
  if (!file)
  {
    perror("Error opening file");
    exit(1);
  }

  int num_requests = fread(requests, sizeof(int), MAX_REQUESTS, file);
  fclose(file);

  return num_requests;
}

int main(int argc, char *argv[])
{
  // Check command line arguments
  if (argc != 3)
  {
    fprintf(stderr, "Usage: %s <initial_head_position> <direction>\n", argv[0]);
    fprintf(stderr, "Direction should be LEFT or RIGHT\n");
    return 1;
  }

  printf("Total requests = %d\n", MAX_REQUESTS);

  // Parse initial head position
  int initial_head = atoi(argv[1]);
  if (initial_head < 0 || initial_head >= DISK_SIZE)
  {
    fprintf(stderr, "Initial head position must be between 0 and %d\n", DISK_SIZE - 1);
    return 1;
  }

  // Parse direction
  bool direction;
  if (strcmp(argv[2], "RIGHT") == 0)
  {
    direction = true; // right
  }
  else if (strcmp(argv[2], "LEFT") == 0)
  {
    direction = false; // left
  }
  else
  {
    fprintf(stderr, "Direction must be LEFT or RIGHT\n");
    return 1;
  }

  // Array to store requests
  int requests[MAX_REQUESTS];

  // Read requests from binary file
  int num_requests = readRequestsFromBinary(requests, "requests.bin");

  printf("Initial Head Position: %d\n", initial_head);
  printf("Direction: %s\n\n", direction ? "RIGHT" : "LEFT");

  // Execute different disk scheduling algorithms
  printf("FCFS DISK SCHEDULING ALGORITHM:\n\n");
  FCFS(requests, initial_head, num_requests);

  printf("\nSSTF DISK SCHEDULING ALGORITHM:\n\n");
  SSTF(requests, initial_head, num_requests);

  printf("\nSCAN DISK SCHEDULING ALGORITHM:\n\n");
  SCAN(requests, num_requests, initial_head, direction);

  printf("\nC-SCAN DISK SCHEDULING ALGORITHM:\n\n");
  CSCAN(requests, num_requests, initial_head, DISK_SIZE, argv[2]);

  printf("\nLOOK DISK SCHEDULING ALGORITHM:\n\n");
  LOOK(requests, initial_head, num_requests);

  printf("\nC-LOOK DISK SCHEDULING ALGORITHM:\n\n");
  CLOOK(requests, initial_head, num_requests);

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

  // Req sequence
  for (int i = 0; i < size; i++)
  {
    if (i == size - 1)
    {
      printf("%d\n", arr[i]);
    }
    else
    {
      printf("%d, ", arr[i]);
    }
  }

  printf("\nFCFS - Total head movements: %d\n", head_movements);
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

  // Request sequence array to store order of serviced requests
  int *requestSequence = (int *)malloc(size * sizeof(int));
  int sequenceIndex = 0;

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

    // Store the current request in sequence
    requestSequence[sequenceIndex++] = arr[nextRequest];

    // Update current head position
    currentHead = arr[nextRequest];

    // Mark request as visited
    visited[nextRequest] = true;
  }

  // Print request sequence
  for (int i = 0; i < sequenceIndex; i++)
  {
    if (i == sequenceIndex - 1)
    {
      printf("%d\n", requestSequence[i]);
    }
    else
    {
      printf("%d, ", requestSequence[i]);
    }
  }

  // Free dynamically allocated memory
  free(visited);
  free(requestSequence);

  printf("\nSSTF - Total head movements: %d\n", head_movements);
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

  printf("%d", curr_req);

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
      printf(", %d", curr_req);
    }

    // If there were arr on the right, go to the last track
    if (n > index)
    {
      head_movements += abs(curr_req - (n * 200 - 1)); // Assuming track range is 0-199
      curr_req = n * 200 - 1;
      printf(", %d", curr_req);
    }

    // Then service arr on the left side
    for (int i = index - 1; i >= 0; i--)
    {
      head_movements += abs(curr_req - arr[i]);
      curr_req = arr[i];
      printf(", %d", curr_req);
    }
  }
  else
  {
    // First, service arr on the left side
    for (int i = index - 1; i >= 0; i--)
    {
      head_movements += abs(curr_req - arr[i]);
      curr_req = arr[i];
      printf(", %d", curr_req);
    }

    // If there were arr on the left, go to track 0
    if (index > 0)
    {
      head_movements += abs(curr_req - 0);
      curr_req = 0;
      printf(", %d", curr_req);
    }

    // Then service arr on the right side
    for (int i = index; i < n; i++)
    {
      head_movements += abs(curr_req - arr[i]);
      curr_req = arr[i];
      printf(", %d", curr_req);
    }
  }

  printf("\n\nSCAN - Total head movements: %d\n", head_movements);
}

void CSCAN(int *arr, int n, int head, int disk_size, char direction[])
{
  // Sort the requests
  qsort(arr, n, sizeof(int), compare);

  int head_movements = 0;
  int curr_head = head;

  // Find the position where the head fits in the sorted array
  int head_index = 0;
  while (head_index < n && arr[head_index] <= head)
  {
    head_index++;
  }

  // Print initial head position
  printf("%d", head);

  if (strcmp(direction, "RIGHT") == 0)
  { // Moving RIGHT
    // First, scan to the right of the head
    for (int i = head_index; i < n; i++)
    {
      head_movements += abs(curr_head - arr[i]);
      curr_head = arr[i];
      printf(", %d", curr_head);
    }

    // If we haven't reached the end of the disk, go to the end
    if (curr_head != disk_size - 1)
    {
      head_movements += abs(curr_head - (disk_size - 1));
      curr_head = disk_size - 1;
      printf(", %d", curr_head);
    }

    // Go to the start of the disk
    head_movements += (disk_size - 1);
    curr_head = 0;
    printf(", %d", curr_head);

    // Scan from the beginning to the point just before the head
    for (int i = 0; i < head_index; i++)
    {
      head_movements += abs(curr_head - arr[i]);
      curr_head = arr[i];
      printf(", %d", curr_head);
    }
  }
  else
  { // Moving LEFT
    // Process requests to the left of head
    for (int i = head_index - 1; i >= 0; i--)
    {
      head_movements += abs(curr_head - arr[i]);
      curr_head = arr[i];
      printf(", %d", curr_head);
    }

    // Move to the beginning if necessary
    if (curr_head > 0)
    {
      head_movements += abs(curr_head - 0);
      curr_head = 0;
      printf(", %d", curr_head);
    }

    // Wrap around to the end
    head_movements += disk_size - 1; // Move from start to end
    curr_head = disk_size - 1;
    printf(", %d", curr_head);

    // Process requests to the right of the initial head position
    for (int i = n - 1; i >= head_index; i--)
    {
      head_movements += abs(curr_head - arr[i]);
      curr_head = arr[i];
      printf(", %d", curr_head);
    }
  }

  printf("\n\nC-SCAN - Total head movements: %d\n", head_movements);
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

  printf("LOOK - Total head movements: %d\n", head_movements);
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

  printf("C-LOOK - Total head movements: %d\n", head_movements);
}
