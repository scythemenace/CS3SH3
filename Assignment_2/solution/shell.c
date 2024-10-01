#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define ARGUMENT_SIZE 16      // Maximum number of arguments per command
#define COMMAND_READ_SIZE 100 // Maximum length of a command string
#define HISTORY_SIZE 5        // Maximum number of commands to store in history

/*NOTE:- Just for reference, wherever we have written, if(!strcmp(string1, string2)), the reason we do this is because,
 * strcmp() returns 0 if the string matches, but in C boolean terms, 0 represents false. In order to tell our program, that
 * we have found the string, we use not 0*/

int should_run = 1;               // Variable to determine if the program should run
char *args[ARGUMENT_SIZE];        // Array for storing the string tokens
char commands[COMMAND_READ_SIZE]; // variable for storing the command string
bool has_ampersand = false;       // To determine if there's an ampersand in the command string
int position;                     // Initializing a variable to keep track of the latest position of the args array
pid_t pid;                        // Global variable for getting the value of the process id
int status;                       // Global variable for getting the state of the child process

/*We had thought of using a circular buffer like orientation (not exactly the template implementation of a circular buffer, but a tweaked one for our usecase) for creation
our functions for the history command due to the fact that it's the most efficient data structure that automatically updates the value of recently entered values in O(1)
time and is perfect for things like history commands given the fact we don't have to shift each value up if we overwrite one, which is the case in normal arrays.*/

char history[HISTORY_SIZE][COMMAND_READ_SIZE]; // An array to store up to HISTORY_SIZE commands and [COMMAND_READ_SIZE] would be the max length of each stored command

/* The history_count while not explicitly, but if coupled with the print_function() we have created acts as the read pointer in normal circular buffer.
The main reason we couldn't have just used the total_commond_count is that the circular buffer only has 0-4 indices. Even though we might be printing them
based on the total number of commands we have entered, the index in the main circular buffer won't change. In order to access elements from the circular buffer
while printing we cannot use the total_command_count because in the case it becomes very large, we will get an out of bounds error. Therefore, it's crucial
we track the history_count differently. */
int history_count = 0;
int history_next = 0;        // Tracks the index where we have to overwrite/write the values (for reference, it's analogous to the write pointer in an actual circular buffer)
int total_command_count = 0; // Total number of commands entered by the user; just for the sake of printing aesthetics as written in the assignment

// Function to start the shell process
void start_shell();
// Function to get the string tokens and fill them in the args array
void get_args();
// Function to get the input command string from the user
void fetch_commands();
// Function to clean up terminated child processes
void cleanup_zombie_processes();
// Function to add a command to history
void add_to_history(const char *command);
// Function to print the command history
void print_history();
// Function to execute the most recent command (using !!)
void execute_last_command();
// Function to execute a given command
void execute_command(const char *cmd);

int main()
{
    start_shell(); // Starting the shell loop (the primary function)
    return 0;
}

/*Since any command using ampersand runs concurrently, we aren't waiting for it when running the command. Therefore, we need a function which loops and waits to
collect all zombie processes after they've exited (they do automatically since execvp() basically kills the c code image). Additionally, our shell should work
seamlessly, i.e. if we enter multiple ampersand commands, the parent process shouldn't even remotely stop working, thus we enter the WNOHANG option in our
waitpid(). That makes sure that if no child process has exited "yet", the parent doesn't sit there waiting for it and is free to execute commands. In the case
they've exited, we can reap the child since it's the first function being run*/
void cleanup_zombie_processes()
{
    pid_t wpid;
    int status;
    while ((wpid = waitpid(-1, &status, WNOHANG)) > 0)
        ;
}

// Starts our dummy shell
void start_shell()
{
    while (should_run)
    {
        cleanup_zombie_processes(); // First clean up any residue child processes we may have started concurrently using the ampersand
        fetch_commands();           // Secondly we have to get the command written by the user

        // Checking if the string is empty
        if (!strcmp("", commands))
        {
            continue;
        }

        // Break if the user writes "exit"
        if (!strcmp("exit", commands))
        {
            should_run = 0;
            continue;
        }

        // If the user entered 'history', print the command history and skip adding to history
        if (!strcmp("history", commands))
        {
            print_history();
            continue;
        }

        // Similarly, if the user entered '!!', execute the most recent command from history
        if (!strcmp("!!", commands))
        {
            execute_last_command();
            continue;
        }

        // Otherwise:
        // Add the current command to the history buffer
        add_to_history(commands);
        // Execute the current command
        execute_command(commands);
    }
}

// Gets the command entered by the user in the terminal
void fetch_commands()
{
    printf("osh> ");                                       // Indicating the user that our dummy shell is accepting inputs
    fflush(stdout);                                        // Sometimes our output can delay, but the user should get the UI signal that the prompt he has entered is received, thus we flush the prompt to display it immediately
    fgets(commands, COMMAND_READ_SIZE, stdin);            // Reads the input string. Useful because only reads till the maximum length given.

    size_t len = strlen(commands); // Get the length of the input string
    if (len > 0 && commands[len - 1] == '\n')
    {
        /*The reason we do this is because, our get_args function terminates when it sees a NULL character, so instead of having
        the default endline terminator when we press enter, we replace it with the \0 NULL character so that our get_args() function knows
        the command has been successfully read till the end.*/
        commands[len - 1] = '\0';
    }
}

/* We will use the get_args to parse the raw command string stored in 'commands' into individual arguments by basically tokenizing the string
based on spaces and filling the 'args' array with each token*/
void get_args()
{
    char *token;           // Each token in the string will be stored in this pointer
    position = 0;          // Reinitializing the position to the first element i.e. 0 so that it doesn't get affected by it's prev val
    has_ampersand = false; // Reinitializing the has_ampersand boolean to false so that it doesn't get affected by it's prev val

    token = strtok(commands, " ");                        // Getting the first word, i.e. initializing the pointer to the first token
    while (token != NULL && position < ARGUMENT_SIZE - 1) // Exit condition is if we encounter a null pointer (we replaced \n with \0)
    {
        args[position++] = token;  // Fills each position with individual token from the string and increments the position value
        token = strtok(NULL, " "); // Shifts the pointer to the next word
    }

    // Checking if the last token was an ampersand
    if (position > 0 && !strcmp("&", args[position - 1]))
    {
        has_ampersand = true;    // Recorded the boolean to be true which will indicate the execute_command() function if there's an ampersand
        args[--position] = NULL; // Settings the index before the ampersand to be NULL since we don't really need the '&' when executing
    }
    else
    {
        args[position] = NULL; // Settings the next index after the last string related to the command to be NULL due to how execvp() works.
    }
}

// Adds any command except for history and !! to the buffer
void add_to_history(const char *command)
{
    // To make sure no commands like 'history', or '!!' are added to the history buffer by any chance
    if (strlen(command) == 0 || !strcmp(command, "history") || !strcmp(command, "!!"))
    {
        return;
    }

    strcpy(history[history_next], command);           // Copy the command string into the history circular buffer at the 'history_next' index
    history_next = (history_next + 1) % HISTORY_SIZE; // Update the 'history_next' index. In the case the buffer is full, it's set to 0 in order to update elements from there

    // If the history buffer isn't full yet, increment the history_count, if it's full, we won't need to increment it since it basically acts as the read pointer of the buffer
    if (history_count < HISTORY_SIZE)
    {
        history_count++;
    }

    // Increment the total number of commands entered in order to print them since history_count is being used for a different purpose and won't increment after 4 (max index)
    total_command_count++;
}

// Prints up to 5 recent commands (with their arguments) that have been invoked.
void print_history()
{
    // If the history buffer is empty, inform the user and exit the function
    if (history_count == 0)
    {
        printf("No commands in history.\n");
        return;
    }

    /*Using the history count to print the commands in order. By that we mean, let's say we filled the array completely [a, b, c, d, e] and in the case
    we rewrite the oldest element 'a' as 'f', the new array will be [f, b, c, d, e] but we don't want to start printing with f as it is the most recent element
    which we print first because unlike normal array printing, in this case we know the oldest command entered is b not f even though it's the first index.
    Therefore, we use the start variable for that*/
    int start = (history_next - history_count + HISTORY_SIZE) % HISTORY_SIZE;
    for (int i = history_count - 1; i >= 0; i--)
    {
        /*Since what we have is basically a circular data structure, the printing order of certain elements can get messy, therefore we use the index pointer
        instead of the iterator i to fetch the elements we have to print from the history array buffer.*/
        int index = (start + i) % HISTORY_SIZE;
        if (total_command_count > 5)
        {
            /*The assignment mentioned that we could exceed 5 commands entered and may have a large number of entered commands. Although we are only displaying
            the most recent 5 elements, it was mentioned we need to give it the value based on the order in which it exists. Therefore we have the total_command_count
            variable. AAdditionally, the reason we add it to (i - 4) is because the assignment specifies that the most recent element should be displayed at the top.*/
            printf("%d %s\n", total_command_count + (i - 4), history[index]);
        }
        else
        {
            /*Uf the total number of commands in the history doesn't exceed 5, we don't even need to use the total_command_count*/
            printf("%d %s\n", i + 1, history[index]);
        }
    }
}

// The function helps execute the last entered command in history and add it again to the history buffer
void execute_last_command()
{
    // Basic checks to ensure the user knows there are no recent command that can be invoked again
    if (history_count == 0)
    {
        printf("No commands in history.\n");
        return;
    }

    /* We calculate the actual last_index based on what we either overwrote or placed in the array
    because getting the most recent command is not as simple as getting the last index of an array
    in the case of a circular buffer.
    */
    int last_index = (history_next - 1 + HISTORY_SIZE) % HISTORY_SIZE;
    printf("%s\n", history[last_index]); // As mentioned in the assignment, we echo it before executing it
    add_to_history(history[last_index]);
    execute_command(history[last_index]);
}

void execute_command(const char *cmd)
{
    strcpy(commands, cmd);
    get_args(); // Convert the input string to an array of individual tokens

    /*NOTE: We are going to execute the command using execvp(), the thing is execvp() hands over the whole control of the program
     to the command that has to be executed, which basically means anything we write after that will become null and void
     since our shell.c file won't have the control. We don't want that, we want to keep executing commands. Therefore, we
     execute the command in a child process*/

    pid = fork(); // Create a child process

    if (pid == -1)
    {
        printf("Child not created successfully");
    }
    else if (pid == 0)
    {
        execvp(args[0], args);
    }
    else
    {
        if (!has_ampersand)
        {
            // If doesn't have ampersand, waits for the child process to finish
            waitpid(pid, &status, 0);
        }
    }
}
