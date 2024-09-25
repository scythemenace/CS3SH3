#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

#define ARGUMENT_SIZE 16
#define COMMAND_READ_SIZE 100

int should_run = 1;
char *args[ARGUMENT_SIZE];
char commands[COMMAND_READ_SIZE];
bool has_ampersand = false;
int position;

void start_shell();
void get_args();
void fetch_commands();
void handle_sigchld(int sig);

int main() {
    signal(SIGCHLD, handle_sigchld);
    start_shell();
    return 0;
}

void handle_sigchld(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void start_shell() {
    while (should_run) {
        fetch_commands();
        if (strcmp("", commands) == 0) {
            continue;
        }

        if (strcmp("exit", commands) == 0) {
            should_run = 0;
            continue;
        }

        get_args();

        pid_t pid = fork();

        if (pid == -1) {
            printf("child not created successfully");
        } else if (pid == 0) {
            // Child process
            if (execvp(args[0], args) == -1) {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        } else {
            // Parent process
            if (!has_ampersand) {
                int status;
                waitpid(pid, &status, 0);
            }
        }
    }
}

void fetch_commands() {
    printf("osh> ");
    fflush(stdout);
    if (fgets(commands, COMMAND_READ_SIZE, stdin) == NULL) {
        if (feof(stdin)) {
            printf("\n");
            exit(EXIT_SUCCESS);
        } else {
            perror("fgets");
            exit(EXIT_FAILURE);
        }
    }
    size_t len = strlen(commands);
    if (len > 0 && commands[len - 1] == '\n') {
        commands[len - 1] = '\0';
    }
}

void get_args() {
    char *token;
    position = 0;
    has_ampersand = false;

    token = strtok(commands, " ");
    while (token != NULL && position < ARGUMENT_SIZE - 1) {
        args[position++] = token;
        token = strtok(NULL, " ");
    }

    if (position > 0 && strcmp(args[position - 1], "&") == 0) {
        has_ampersand = true;
        args[--position] = NULL;
    } else {
        args[position] = NULL;
    }
}
