// Including necessary header files
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define NUM_WAITING_CHAIRS 3 // As defined in the assignment, there are 5 waiting chairs
#define NUM_STUDENTS 5 // Hard coding the number of students

// Semaphores and mutex locks
sem_t students_sem;                              // Counts the number of waiting students
sem_t student_sem[NUM_STUDENTS];         // Semaphores for each student to wait on TA help
pthread_mutex_t waiting_room_mutex;              // Protects access to waiting room chairs

// Shared variables
int waiting_room_chairs[NUM_WAITING_CHAIRS];
int number_students_waiting = 0;
int next_seating_position = 0;
int next_teaching_position = 0;

// Function prototypes
void* student_actions(void* student_id_ptr);
void* ta_actions(void* arg);

int main() {
    int i;
    int student_num = NUM_STUDENTS;      // Set the number of students

    int student_ids[student_num];
    pthread_t students[student_num];
    pthread_t ta;

    // Initialize semaphores and mutex
    sem_init(&students_sem, 0, 0);
    pthread_mutex_init(&waiting_room_mutex, NULL);

    // Initialize student semaphores
    for (i = 0; i < student_num; i++) {
        sem_init(&student_sem[i], 0, 0);
    }

    srand(time(NULL));                           // Seed random number generator

    // Create TA thread
    pthread_create(&ta, NULL, ta_actions, NULL);

    // Create student threads
    for (i = 0; i < student_num; i++) {
        student_ids[i] = i + 1;
        pthread_create(&students[i], NULL, student_actions, (void*)&student_ids[i]);
    }

    // Join threads (optional here since loops are infinite)
    pthread_join(ta, NULL);
    for (i = 0; i < student_num; i++) {
        pthread_join(students[i], NULL);
    }

    return 0;
}

void* ta_actions(void* arg) {
    while (1) {
        // Wait for a student to arrive
        sem_wait(&students_sem);

        // Help the next student
        pthread_mutex_lock(&waiting_room_mutex);

        // Get the next student to help
        int student_id = waiting_room_chairs[next_teaching_position];
        waiting_room_chairs[next_teaching_position] = 0; // Remove student from chair
        number_students_waiting--;
        next_teaching_position = (next_teaching_position + 1) % NUM_WAITING_CHAIRS;

        pthread_mutex_unlock(&waiting_room_mutex);

        // Help the student (simulate with sleep)
        int help_time = rand() % 5 + 1; // Help time between 1 and 5 seconds
        printf("TA: Helping student %d for %d seconds. Students waiting: %d\n", student_id, help_time, number_students_waiting);
        sleep(help_time);

        // Signal the student that help is done
        sem_post(&student_sem[student_id - 1]);
    }
}

void* student_actions(void* student_id_ptr) {
    int student_id = *(int*)student_id_ptr;

    while (1) {
        // Programming
        int programming_time = rand() % 5 + 1; // Between 1 and 5 seconds
        printf("Student %d: Programming for %d seconds.\n", student_id, programming_time);
        sleep(programming_time);

        // Need help, try to acquire a seat
        pthread_mutex_lock(&waiting_room_mutex);

        if (number_students_waiting < NUM_WAITING_CHAIRS) {
            // Take a seat
            waiting_room_chairs[next_seating_position] = student_id;
            number_students_waiting++;
            printf("Student %d: Takes a seat. Students waiting: %d\n", student_id, number_students_waiting);
            next_seating_position = (next_seating_position + 1) % NUM_WAITING_CHAIRS;

            pthread_mutex_unlock(&waiting_room_mutex);

            // Signal TA that a student is waiting
            sem_post(&students_sem);

            // Wait for TA to help
            sem_wait(&student_sem[student_id - 1]); // Wait until TA has helped this student
            printf("Student %d: Received help from TA.\n", student_id);

        } else {
            // No chairs available
            pthread_mutex_unlock(&waiting_room_mutex);
            printf("Student %d: No chairs available. Will try later.\n", student_id);
        }
    }
}

