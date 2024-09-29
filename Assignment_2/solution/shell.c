#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define ARGUMENT_SIZE 16
#define COMMAND_READ_SIZE 100
#define HISTORY_SIZE 5

int should_run = 1;
char *args[ARGUMENT_SIZE];
char commands[COMMAND_READ_SIZE];
bool has_ampersand = false;
int position;
pid_t pid;
int status;

// Circular buffer for history
char history[HISTORY_SIZE][COMMAND_READ_SIZE];
int history_count = 0;
int history_next = 0;  // Changed from history_start to history_next
int total_command_count = 0;

void start_shell();
void get_args();
void fetch_commands();
void cleanup_zombie_processes();
void add_to_history(const char *command);
void print_history();
void execute_last_command();
void execute_command(const char *cmd);

int main() {
    start_shell();
    return 0;
}

void cleanup_zombie_processes() {
    pid_t wpid;
    int status;
    while ((wpid = waitpid(-1, &status, WNOHANG)) > 0);
}

void start_shell() {
    while (should_run) {
        cleanup_zombie_processes();
        fetch_commands();

        if (!strcmp("", commands)) {
            continue;
        }

        if (!strcmp("exit", commands)) {
            should_run = 0;
            continue;
        }

        if (!strcmp("history", commands)) {
            print_history();
            continue;
        }

        if (!strcmp("!!", commands)) {
            execute_last_command();
            continue;
        }

        add_to_history(commands);
        execute_command(commands);
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

    if (position > 0 && !strcmp("&", args[position - 1])) {
        has_ampersand = true;
        args[--position] = NULL;
    } else {
        args[position] = NULL;
    }
}

void add_to_history(const char *command) {
    if (strlen(command) == 0 || !strcmp(command, "history") || !strcmp(command, "!!")) {
        return;  // Don't add empty commands, "history", or "!!" to history
    }
    
    strcpy(history[history_next], command);
    history_next = (history_next + 1) % HISTORY_SIZE;
    if (history_count < HISTORY_SIZE) {
        history_count++;
    }

    total_command_count++;
}

void print_history() {
    if (history_count == 0) {
        printf("No commands in history.\n");
        return;
    }

    int start = (history_next - history_count + HISTORY_SIZE) % HISTORY_SIZE;
    for (int i = history_count - 1; i >= 0; i--) {
        int index = (start + i) % HISTORY_SIZE;
        if (total_command_count > 5) {
          printf("%d %s\n", total_command_count + (i - 4), history[index]);
        } else {
          printf("%d %s\n", i + 1, history[index]);
        }
    }
}

void execute_last_command() {
    if (history_count == 0) {
        printf("No commands in history.\n");
        return;
    }

    int last_index = (history_next - 1 + HISTORY_SIZE) % HISTORY_SIZE;
    printf("%s\n", history[last_index]);
    add_to_history(history[last_index]);
    execute_command(history[last_index]);
}

void execute_command(const char *cmd) {
    strcpy(commands, cmd);
    get_args();

    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    } else {
        if (!has_ampersand) {
            waitpid(pid, &status, 0);
        }
    }
}
