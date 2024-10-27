// Added Necessary Headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

// Prototypes
void* student_actions( void* student_id ); // Function executed by student thread
void* ta_actions(); // Function executed by TA thread

// Defined Macros for simplicity
#define NUM_WAITING_CHAIRS 3 // Max chairs in hallway, as defined in the assignment
#define NUM_STUDENTS 5 // Arbitrarily choosing number of students as 5, since we have to simulate any "n" students

sem_t sem_students; // Denotes number of students waiting to get help
sem_t sem_ta; // Denotes access to TA help
pthread_mutex_t mutex; // For protecting shared variables

int waiting_room_chairs[3]; // An array which represents max chairs in the hallway
int number_students_waiting = 0; // Number of students currently waiting
int next_seating_position = 0; // Index for the next chair available
int next_teaching_position = 0; // Index for the next student which needs help
int ta_sleeping_flag = 0; // Flag to indiciate if TA is sleeping or not

int main() {
 
	int student_num = NUM_STUDENTS; 
  /*Time in C is not truly randomized, it's very likely to return the same value if we just use rand().
   In order to truly randomize it we use srand() since it takes a seed. The reason we use time is that
   srand() will also return the same outputs for the same seed, time() is a variable that's changing every moment
   as time passes. Time is being recorded since the UNIX epoch and a new seed will be generated everytime for rand().
   Therefore, we use srand(time(NULL))*/
  srand(time(NULL));

  // Thread creation
	int student_ids[student_num];
	pthread_t students[student_num];
	pthread_t ta;

  // Initializing the semaphores
	sem_init(&sem_students, 0, 0); 
	sem_init(&sem_ta, 0, 1);

  // Initializing the mutex
	pthread_mutex_init(&mutex, NULL);
	pthread_create(&ta, NULL, ta_actions, NULL);
	for( int i = 0; i < student_num; i++ )
	{
		student_ids[i] = i + 1;
		pthread_create( &students[i], NULL, student_actions, (void*) &student_ids[i] );
	}

	//Join threads
	pthread_join(ta, NULL);
	for (int i = 0; i < student_num; i++ )
	{
		pthread_join( students[i],NULL );
	}

	return 0;
}

void* ta_actions() {

	printf("Checking for students.\n");

	while(1) {

		//if students are waiting
		if (number_students_waiting > 0) {

			ta_sleeping_flag = 0;
			sem_wait(&sem_students);
			pthread_mutex_lock(&mutex);

			int help_time = rand() % 5;

			//TA helping student.
			printf("Helping a student(student: %d) for %d seconds. Students waiting = %d.\n", waiting_room_chairs[next_teaching_position], help_time, (number_students_waiting - 1));

			waiting_room_chairs[next_teaching_position]=0;
			number_students_waiting--;
			next_teaching_position = ( next_teaching_position + 1 ) % NUM_WAITING_CHAIRS;

			sleep(help_time);

			pthread_mutex_unlock( &mutex );
			sem_post( &sem_ta );

		}
		//if no students are waiting
		else {

			if (ta_sleeping_flag == 0) {

				printf("No students waiting. Sleeping.\n");
				ta_sleeping_flag = 1;

			}
      usleep(100000);

		}

	}

}

void* student_actions(void* student_id) {

	int id_student = *(int*)student_id;

	while(1) {

		//if student is waiting, continue waiting
		if (isWaiting(id_student) == 1) {
      continue; 
    }

		//student is programming.
		int time = rand() % 5;
		printf("\tStudent %d is programming for %d seconds.\n", id_student, time);
		sleep(time);

		pthread_mutex_lock(&mutex);

		if(number_students_waiting < NUM_WAITING_CHAIRS) {

			waiting_room_chairs[next_seating_position] = id_student;
			number_students_waiting++;

			//student takes a seat in the hallway.
			printf("\t\tStudent %d takes a seat. Students waiting = %d.\n", id_student, number_students_waiting);
			next_seating_position = (next_seating_position + 1) % NUM_WAITING_CHAIRS;

			pthread_mutex_unlock(&mutex);

			//wake TA if sleeping
			sem_post(&sem_students);
			sem_wait(&sem_ta);

		}
		else {

			pthread_mutex_unlock(&mutex);

			//No chairs available. Student will try later.
			printf("\t\t\tStudent %d will try later.\n",id_student);

		}

	}

}

int isWaiting(int student_id) {
	int i;
	for (i = 0; i < 3; i++) {
		if (waiting_room_chairs[i] == student_id) {
      return 1; 
    }
	}
	return 0;
}
