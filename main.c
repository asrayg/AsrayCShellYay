#include "shell.h"
#include "builtins.h"
#include "utils.h"
#include "job_control.h"
#include "readline.h"

char prompt[BUFFER_SIZE] = "> ";
int job_count = 0;
job* job_list = NULL;
FILE* log_file = NULL;
char* dir_stack[MAX_DIR_STACK];
int dir_stack_count = 0;
alias alias_list[MAX_ALIASES];
int alias_count = 0;
shell_var var_list[MAX_VARS];
int var_count = 0;

void shell_loop();
int execute(char** args);
int execute_subshell(char* command);

int main() {
    initialize_readline();
    load_history();
    load_aliases();
    start_logging();

    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);
    signal(SIGCHLD, sigchld_handler);
    signal(SIGCONT, sigcont_handler);

    shell_loop();

    return 0;
}

void shell_loop() {
    char* line;
    char** args;
    int status;

    do {
        line = read_line();
        args = split_line(line);
        status = execute(args);

        free(line);
        free(args);
    } while (status);
}

int execute(char** args) {
    int i;

    if (args[0] == NULL) {
        return 1;
    }

    if (args[0][0] == '(' && args[0][strlen(args[0]) - 1] == ')') {
        char subshell_command[BUFFER_SIZE];
        strncpy(subshell_command, args[0] + 1, strlen(args[0]) - 2);
        subshell_command[strlen(args[0]) - 2] = '\0';
        return execute_subshell(subshell_command);
    }

    for (i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    char** args2 = NULL;
    for (i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "|") == 0) {
            args[i] = NULL;
            args2 = &args[i + 1];
            break;
        }
    }

    if (args2 != NULL) {
        execute_piped(args, args2);
    } else {
        if (handle_redirection(args) == -1) {
            return 1;
        }

        pid_t pid;
        int background = 0;
        int status;

        if (strcmp(args[i - 1], "&") == 0) {
            background = 1;
            args[i - 1] = NULL;
        }

        pid = fork();
        if (pid == 0) {
            char* full_path = resolve_path(args[0]);
            if (!full_path) {
                fprintf(stderr, "shell: command not found: %s\n", args[0]);
                exit(EXIT_FAILURE);
            }
            if (execv(full_path, args) == -1) {
                handle_error("shell");
            }
            free(full_path);
        } else if (pid < 0) {
            handle_error("shell");
        } else {
            if (background) {
                printf("[%d] %s\n", pid, args[0]);
                add_job(pid, args[0]);
            } else {
                waitpid(pid, &status, 0);
            }
        }
    }

    return 1;
}

int execute_subshell(char* command) {
    pid_t pid = fork();
    if (pid == 0) {
        char* subshell_command[] = {"sh", "-c", command, NULL};
        if (execvp(subshell_command[0], subshell_command) == -1) {
            perror("shell");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("shell");
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
    return 1;
}
