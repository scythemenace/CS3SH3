#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#define MAX 1000

// Utility function to compare two integers for qsort
// This will be used to sort arrays of requests
int compare(const void *a, const void *b)
{
  return (*(int *)a - *(int *)b);
}

// Read requests from a binary file named 'filename' into the requests array
// Returns the number of requests read or -1 if file open fails
int read_requests(const char *filename, int requests[])
{
  FILE *file = fopen(filename, "rb");
  if (!file)
  {
    perror("Error opening file");
    return -1;
  }
  
  // fread attempts to read up to MAX integers from file into requests array
  int count = fread(requests, sizeof(int), MAX, file);
  fclose(file);
  return count;
}

// FCFS (First Come First Serve) Disk Scheduling Algorithm
// Processes disk requests in the order they appear
// arr[] contains the requests, size is the number of requests, head is initial head position
void FCFS(int arr[], int size, int head)
{
  int seek_count = 0; // Total head movements
  int cur_track, distance;

  // Iterate through each request in order
  for (int i = 0; i < size; i++)
  {
    cur_track = arr[i];                 // Current request to process
    distance = abs(head - cur_track);   // Calculate distance from current head position
    seek_count += distance;             // Add this movement to total
    head = cur_track;                   // Move head to the current request
    printf("%d ", cur_track);           // Print the processed track
  }
  printf("\n\nTotal head movements: %d\n", seek_count);
}

// SSTF (Shortest Seek Time First) Disk Scheduling Algorithm
// Always choose the next request with the shortest distance from the current head position
void SSTF(int request[], int size, int head)
{
  // If there are no requests, just print a message and return
  if (size == 0)
  {
    printf("No requests to process.\n");
    return;
  }

  int diff[size][2];       // For each request, store distance and index
  int seek_count = 0;      // Total head movements
  int seek_sequence[size]; // Sequence of serviced requests
  int processed[size];     // Whether a request has been processed

  memset(processed, 0, sizeof(processed)); // Initially no requests are processed

  // Repeat until all requests are processed
  for (int i = 0; i < size; i++)
  {
    // Compute the distance of each unprocessed request from current head
    for (int j = 0; j < size; j++)
    {
      if (!processed[j])
      {
        diff[j][0] = abs(head - request[j]);
        diff[j][1] = j;
      }
      else
      {
        diff[j][0] = INT_MAX; // Already processed requests are effectively ignored
      }
    }

    // Find the request with the minimum distance
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

    // Mark the chosen request as processed
    processed[min_index] = 1;
    // Update total seek count by adding the shortest distance found
    seek_count += min_distance;
    // Move head to the chosen request
    head = request[min_index];
    // Record the sequence of serviced tracks
    seek_sequence[i] = head;
    // Print the processed track
    printf("%d ", head);
  }
  printf("\n\nTotal head movements: %d\n", seek_count);
}

// SCAN Disk Scheduling Algorithm
// Moves the head in one direction, servicing all requests until it reaches the end,
// then reverses direction and continues servicing requests.
void SCAN(int arr[], int size, int head, char direction[], int disk_size)
{
  int seek_count = 0;                  // Total head movements
  int left[MAX], right[MAX];           // Arrays to hold requests on left and right side of head
  int seek_sequence[MAX];              // Serviced sequence
  int left_size = 0, right_size = 0;   // Counts of left and right side requests
  int seek_seq_size = 0;

  // Add boundary endpoints depending on direction
  // If direction is LEFT, we consider 0 as a boundary
  // If direction is RIGHT, we consider (disk_size-1) as a boundary
  if (strcmp(direction, "LEFT") == 0)
    left[left_size++] = 0;
  else if (strcmp(direction, "RIGHT") == 0)
    right[right_size++] = disk_size - 1;

  // Distribute requests into left or right arrays based on head position
  for (int i = 0; i < size; i++)
  {
    if (arr[i] < head)
      left[left_size++] = arr[i];
    else
      right[right_size++] = arr[i];
  }

  // Sort both arrays so we can service them in order
  qsort(left, left_size, sizeof(int), compare);
  qsort(right, right_size, sizeof(int), compare);

  // If moving LEFT first:
  if (strcmp(direction, "LEFT") == 0)
  {
    // Print the initial head position for clarity
    printf("%d ", head);
    // Move from head towards the left boundary
    for (int i = left_size - 1; i >= 0; i--)
    {
      seek_sequence[seek_seq_size++] = left[i];
      seek_count += abs(head - left[i]);
      head = left[i];
    }
    // After reaching the left boundary, move to the right side
    for (int i = 0; i < right_size; i++)
    {
      seek_sequence[seek_seq_size++] = right[i];
      seek_count += abs(head - right[i]);
      head = right[i];
    }
  }
  else // Direction is RIGHT first
  {
    // Move from head towards the right boundary
    for (int i = 0; i < right_size; i++)
    {
      seek_sequence[seek_seq_size++] = right[i];
      seek_count += abs(head - right[i]);
      head = right[i];
    }
    // After reaching the right boundary, move to the left side
    for (int i = left_size - 1; i >= 0; i--)
    {
      seek_sequence[seek_seq_size++] = left[i];
      seek_count += abs(head - left[i]);
      head = left[i];
    }
  }

  // Print the sequence of serviced requests
  for (int i = 0; i < seek_seq_size; i++)
  {
    printf("%d ", seek_sequence[i]);
  }
  printf("\n\nSCAN - Total head movements: %d\n", seek_count);
}

// C-SCAN (Circular SCAN) Disk Scheduling Algorithm
// Similar to SCAN but instead of reversing direction, it jumps back to the start or end,
// treating the disk as circular.
void CSCAN(int arr[], int size, int head, int disk_size, char direction[])
{
  int seek_count = 0;    // Total head movements
  int left[MAX], right[MAX];
  int left_size = 0, right_size = 0;

  // Print initial head position
  printf("%d ", head);

  // Add boundaries: 0 and disk_size - 1 are always considered in C-SCAN
  left[left_size++] = 0;
  right[right_size++] = disk_size - 1;

  // Separate requests into those to the left and right of the head
  for (int i = 0; i < size; i++)
  {
    if (arr[i] < head)
      left[left_size++] = arr[i];
    if (arr[i] > head)
      right[right_size++] = arr[i];
  }

  // Sort both arrays
  qsort(left, left_size, sizeof(int), compare);
  qsort(right, right_size, sizeof(int), compare);

  // If direction is RIGHT
  if (strcmp(direction, "RIGHT") == 0)
  {
    // Service all requests to the right of the head
    for (int i = 0; i < right_size; i++)
    {
      printf("%d ", right[i]);
      seek_count += abs(head - right[i]);
      head = right[i];
    }

    // After reaching the rightmost end, jump back to the leftmost track
    seek_count += abs(disk_size - 1 - 0);
    head = 0;

    // Service all requests on the left side
    for (int i = 0; i < left_size; i++)
    {
      printf("%d ", left[i]);
      seek_count += abs(head - left[i]);
      head = left[i];
    }
  }
  else if (strcmp(direction, "LEFT") == 0)
  {
    // Service all requests to the left of the head (in reverse order)
    for (int i = left_size - 1; i >= 0; i--)
    {
      printf("%d ", left[i]);
      seek_count += abs(head - left[i]);
      head = left[i];
    }

    // After reaching the leftmost end, jump to the rightmost track
    seek_count += abs(disk_size - 1);
    head = disk_size - 1;

    // Service all requests on the right side (in reverse order)
    for (int i = right_size - 1; i >= 0; i--)
    {
      printf("%d ", right[i]);
      seek_count += abs(head - right[i]);
      head = right[i];
    }
  }

  printf("\n\nC-SCAN - Total head movements: %d\n", seek_count);
}

// LOOK Disk Scheduling Algorithm
// Similar to SCAN but does not go to the extreme ends (no boundary tracks are forced).
// Moves in one direction servicing requests, then reverses direction.
void LOOK(int arr[], int size, int head, char direction[])
{
  int seek_count = 0;                    // Total head movements
  int left[MAX], right[MAX], seek_sequence[MAX];
  int left_size = 0, right_size = 0, seek_seq_size = 0;

  // Divide requests into left and right subsets relative to head
  for (int i = 0; i < size; i++)
  {
    if (arr[i] < head)
      left[left_size++] = arr[i];
    else
      right[right_size++] = arr[i];
  }

  // Sort them so we can pick requests in order
  qsort(left, left_size, sizeof(int), compare);
  qsort(right, right_size, sizeof(int), compare);

  // If direction is LEFT, service all left requests first (from nearest to farthest),
  // then service right requests
  if (strcmp(direction, "LEFT") == 0)
  {
    printf("%d ", head);
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
  else // Direction is RIGHT, service right first, then left
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

  // Print serviced sequence
  for (int i = 0; i < seek_seq_size; i++)
  {
    printf("%d ", seek_sequence[i]);
  }
  printf("\n\nLOOK - Total head movements: %d\n", seek_count);
}

// C-LOOK Disk Scheduling Algorithm
// Similar to C-SCAN, but the head only goes as far as the last request in one direction
// before jumping back to the other end. It doesn't go all the way to the boundaries.
void CLOOK(int arr[], int size, int head, char direction[])
{
  int seek_count = 0;    // Total head movements
  int distance, cur_track;
  int left[MAX], right[MAX];
  int left_size = 0, right_size = 0;
  int seek_sequence[MAX];
  int seq_index = 0;

  // Print the initial head position for clarity
  printf("%d ", head);

  // Divide requests into those on the left and right of the head
  for (int i = 0; i < size; i++)
  {
    if (arr[i] < head)
      left[left_size++] = arr[i];
    else if (arr[i] > head)
      right[right_size++] = arr[i];
  }

  // Sort left and right arrays
  qsort(left, left_size, sizeof(int), compare);
  qsort(right, right_size, sizeof(int), compare);

  // If direction is RIGHT, service the right requests first, then jump to the left side
  if (strcmp(direction, "RIGHT") == 0)
  {
    // Service all right-side requests in ascending order
    for (int i = 0; i < right_size; i++)
    {
      cur_track = right[i];
      seek_sequence[seq_index++] = cur_track;
      distance = abs(cur_track - head);
      seek_count += distance;
      head = cur_track;
    }

    // After finishing right side, jump to the leftmost request
    if (left_size > 0)
    {
      seek_count += abs(head - left[0]);
      head = left[0];

      // Then service all left-side requests in ascending order
      for (int i = 0; i < left_size; i++)
      {
        cur_track = left[i];
        seek_sequence[seq_index++] = cur_track;
        distance = abs(cur_track - head);
        seek_count += distance;
        head = cur_track;
      }
    }
  }
  else if (strcmp(direction, "LEFT") == 0)
  {
    // If direction is LEFT, service left-side requests in descending order first
    for (int i = left_size - 1; i >= 0; i--)
    {
      cur_track = left[i];
      seek_sequence[seq_index++] = cur_track;
      distance = abs(cur_track - head);
      seek_count += distance;
      head = cur_track;
    }

    // After finishing left side, jump to the rightmost request
    if (right_size > 0)
    {
      seek_count += abs(head - right[right_size - 1]);
      head = right[right_size - 1];

      // Then service right-side requests in descending order
      for (int i = right_size - 1; i >= 0; i--)
      {
        cur_track = right[i];
        seek_sequence[seq_index++] = cur_track;
        distance = abs(cur_track - head);
        seek_count += distance;
        head = cur_track;
      }
    }
  }

  // Print all serviced requests in order
  for (int i = 0; i < seq_index; i++)
  {
    printf("%d ", seek_sequence[i]);
  }
  printf("\n\nC-LOOK - Total head movements: %d\n", seek_count);
}

// Main function
int main(int argc, char *argv[])
{
  // Expect 3 arguments: the executable name, initial head position, and direction
  if (argc != 3)
  {
    printf("Usage: ./assignment5 <initial_head_position> <direction>\n");
    return 1;
  }

  // Convert initial head position to integer
  int head = atoi(argv[1]);
  // Check that head position is within valid range
  if (head < 0 || head > 299)
  {
    printf("Initial head position must be between 0 and 299.\n");
    return 1;
  }

  char direction[10];
  strcpy(direction, argv[2]);
  // Direction must be either LEFT or RIGHT
  if (strcmp(direction, "LEFT") != 0 && strcmp(direction, "RIGHT") != 0)
  {
    printf("Direction must be either 'LEFT' or 'RIGHT'.\n");
    return 1;
  }

  int requests[MAX];
  // Read requests from binary file "request.bin"
  int count = read_requests("request.bin", requests);
  if (count < 0)
  {
    return 1;
  }

  // Print basic info
  printf("Total requests: %d\n", count);
  printf("Initial head position: %d\n", head);
  printf("Direction of Head: %s\n", direction);

  // Demonstrate FCFS
  printf("\nFCFS DISK SCHEDULING ALGORITHM:\n\n");
  FCFS(requests, count, head);

  // Demonstrate SSTF
  printf("\nSSTF DISK SCHEDULING ALGORITHM:\n\n");
  SSTF(requests, count, head);

  // Demonstrate SCAN
  printf("\nSCAN DISK SCHEDULING ALGORITHM:\n\n");
  SCAN(requests, count, head, direction, 300);

  // Demonstrate C-SCAN
  printf("\nC-SCAN DISK SCHEDULING ALGORITHM:\n\n");
  CSCAN(requests, count, head, 300, direction);

  // Demonstrate LOOK
  printf("\nLOOK DISK SCHEDULING ALGORITHM:\n\n");
  LOOK(requests, count, head, direction);

  // Demonstrate C-LOOK
  printf("\nC-LOOK DISK SCHEDULING ALGORITHM:\n\n");
  CLOOK(requests, count, head, direction);

  return 0;
}