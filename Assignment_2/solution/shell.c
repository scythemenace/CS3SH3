#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define ARGUMENT_SIZE 16
#define COMMAND_READ_SIZE 100

/*NOTE:- Just for reference, wherever we have written, if(!strcmp(string1, string2)), the reason we do this is because,
 * strcmp() returns 0 if the string matches, but in C boolean terms, 0 represents false. In order to tell our program, that
 * we have found the string, we use not 0*/

int should_run = 1; // Variable to determine if the program should run
char *args[ARGUMENT_SIZE]; // Array for storing the string tokens
char commands[COMMAND_READ_SIZE]; // variable for storing the command string
bool has_ampersand = false;
int position; // Initializing a variable to keep track of the latest position of the args array 
pid_t pid; // Global variable for getting the value of the process id
int status; // Global variable for getting the state of the child process

void start_shell(); // Function to start the shell process
void get_args(); // Function to get the string tokens and fill them in the args array
void fetch_commands(); // Function to get the input command string from the user

int main () {
  
  start_shell(); // Starting the shell
  return 0;
}

void start_shell() {
  while(should_run) {
    fetch_commands(); // First we have to get the command written by the user
    has_ampersand = false; // Reinitializing the has_ampersand boolean to false so that it doesn't get affected by it's prev val
    
    // Checking if the string is empty
    if (!strcmp("", commands)) {
      continue;
    }

    if (!strcmp("exit", commands)) {
      should_run = 0; 
    }

    get_args(); // Convert the input string to an array of individual token
    
    /*NOTE: We are going to execute the command using execvp(), the thing is execvp() hands over the whole control of the program
      to the command that has to be executed, which basically means anything we write after that will become null and void
      since our shell.c file won't have the control. We don't want that, we want to keep executing commands. Therefore, we 
      execute the command in a child process*/

    pid = fork(); // Create a child process 

    // Checking which process are we in i.e. parent, child, etc.
    if (pid == -1) {
      printf("Child not created successfully");
    } else if (pid == 0) {
      execvp(args[0], args);
    } else {
      // If doesn't have ampersand, waits for the child process to finish
      if (!has_ampersand) {
        wait(&status);
      }
    }
  }
}

void fetch_commands() {
  printf("osh> ");
  fgets(commands, COMMAND_READ_SIZE, stdin); // Reads the input string. Useful because only reads till the maximum length given.
  if ((strlen(commands) > 0) && (commands[strlen(commands) - 1] == '\n')) {
    commands[strlen(commands) - 1] = '\0';
    /*The reason we do this is because, our get_args function terminates when it sees a NULL character, so instead of having
     the default endline terminator when we press enter, we replace it with the \0 NULL character*/
  }
}

void get_args() {
  char *token; //Each token in the string will be stored in this pointer 
  position = 0; // Reinitializing the position to the first element i.e. 0 so that it doesn't get affected by it's prev val
  token = strtok(commands, " "); // Getting the first word, i.e. initializing the pointer to the first token
  while (token != NULL) { // Exit condition is if we encounter a null pointer (we replaced \n with \0)
    args[position] = token; // Fills each position with individual token from the string
    position++; // Incrementing position value
    token = strtok(NULL, " "); // Shifts the pointer to the next word
  }

  // Checking if the last token was an ampersand
  if (!strcmp("&", args[position-1])) {
    has_ampersand = true;
    args[position - 1] = NULL;
    return;
  }

  args[position] = NULL;
}
