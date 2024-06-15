#include "shell.h"
#include "utils.h"

void handle_error(const char* message) {
    perror(message);
    fprintf(stderr, "shell: an error occurred with message: %s\n", message);
    exit(EXIT_FAILURE);
}

char* resolve_path(char* command) {
    char* path_env = getenv("PATH");
    if (!path_env) {
        return NULL;
    }

    char* path = strdup(path_env);
    char* dir = strtok(path, ":");
    while (dir) {
        char* full_path = malloc(strlen(dir) + strlen(command) + 2);
        sprintf(full_path, "%s/%s", dir, command);
        if (access(full_path, X_OK) == 0) {
            free(path);
            return full_path;
        }
        free(full_path);
        dir = strtok(NULL, ":");
    }
    free(path);
    return NULL;
}

char* expand_env_variables(char* line) {
    char* result = malloc(BUFFER_SIZE);
    char* p = line;
    char* q = result;
    char* var_start;
    char var_name[BUFFER_SIZE];
    char* var_value;

    while (*p) {
        if (*p == '$') {
            p++;
            var_start = p;
            while (*p && (isalnum(*p) || *p == '_')) {
                p++;
            }
            strncpy(var_name, var_start, p - var_start);
            var_name[p - var_start] = '\0';
            var_value = getenv(var_name);
            if (var_value) {
                strcpy(q, var_value);
                q += strlen(var_value);
            }
        } else {
            *q++ = *p++;
        }
    }
    *q = '\0';
    return result;
}

char* command_substitution(char* line) {
    char* result = malloc(BUFFER_SIZE);
    char* p = line;
    char* q = result;
    char command[BUFFER_SIZE];
    FILE* fp;

    while (*p) {
        if (*p == '`' || (*p == '$' && *(p + 1) == '(')) {
            char* command_start = p;
            if (*p == '`') {
                p++;
                while (*p && *p != '`') {
                    p++;
                }
                strncpy(command, command_start + 1, p - command_start - 1);
                command[p - command_start - 1] = '\0';
                p++;
            } else {
                p += 2;
                while (*p && (*p != ')' || *(p - 1) == '\\')) {
                    p++;
                }
                strncpy(command, command_start + 2, p - command_start - 2);
                command[p - command_start - 2] = '\0';
                p++;
            }

            fp = popen(command, "r");
            if (fp == NULL) {
                perror("shell");
                return NULL;
            }

            while (fgets(q, BUFFER_SIZE, fp)) {
                q += strlen(q);
            }

            pclose(fp);
        } else {
            *q++ = *p++;
        }
    }
    *q = '\0';
    return result;
}

int handle_redirection(char** args) {
    int fd;
    int i = 0;

    while (args[i] != NULL) {
        if (strcmp(args[i], ">") == 0) {
            args[i] = NULL;
            fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("shell");
                return -1;
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            return 1;
        } else if (strcmp(args[i], "<") == 0) {
            args[i] = NULL;
            fd = open(args[i + 1], O_RDONLY);
            if (fd < 0) {
                perror("shell");
                return -1;
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
            return 1;
        }
        i++;
    }
    return 0;
}

char* history_expansion(char* line) {
    if (line[0] == '!') {
        int index = atoi(&line[1]);
        HIST_ENTRY* entry = history_get(index);
        if (entry) {
            free(line);
            return strdup(entry->line);
        } else {
            fprintf(stderr, "shell: no such history entry: %d\n", index);
        }
    }
    return line;
}

void set_prompt(char* new_prompt) {
    strncpy(prompt, new_prompt, BUFFER_SIZE - 1);
    prompt[BUFFER_SIZE - 1] = '\0';
}

int set_shell_var(char* name, char* value) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(var_list[i].name, name) == 0) {
            free(var_list[i].value);
            var_list[i].value = strdup(value);
            return 1;
        }
    }

    if (var_count < MAX_VARS) {
        var_list[var_count].name = strdup(name);
        var_list[var_count].value = strdup(value);
        var_count++;
        return 1;
    }

    return 0;
}

char* get_shell_var(char* name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(var_list[i].name, name) == 0) {
            return var_list[i].value;
        }
    }
    return NULL;
}

char* expand_shell_variables(char* line) {
    char* result = malloc(BUFFER_SIZE);
    char* p = line;
    char* q = result;
    char* var_start;
    char var_name[BUFFER_SIZE];
    char* var_value;

    while (*p) {
        if (*p == '$') {
            p++;
            var_start = p;
            while (*p && (isalnum(*p) || *p == '_')) {
                p++;
            }
            strncpy(var_name, var_start, p - var_start);
            var_name[p - var_start] = '\0';
            var_value = get_shell_var(var_name);
            if (var_value) {
                strcpy(q, var_value);
                q += strlen(var_value);
            }
        } else {
            *q++ = *p++;
        }
    }
    *q = '\0';
    return result;
}

void history_list() {
    HIST_ENTRY** history = history_list();
    if (history) {
        for (int i = 0; history[i]; i++) {
            printf("%d: %s\n", i + history_base, history[i]->line);
        }
    }


void execute_piped(char** args1, char** args2) {
    int pipefd[2];
    pid_t p1, p2;

    if (pipe(pipefd) < 0) {
        perror("shell");
        return;
    }

    p1 = fork();
    if (p1 < 0) {
        perror("shell");
        return;
    }

    if (p1 == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (execvp(args1[0], args1) == -1) {
            perror("shell");
            exit(EXIT_FAILURE);
        }
    } else {
        p2 = fork();
        if (p2 < 0) {
            perror("shell");
            return;
        }

        if (p2 == 0) {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);

            if (execvp(args2[0], args2) == -1) {
                perror("shell");
                exit(EXIT_FAILURE);
            }
        } else {
            close(pipefd[0]);
            close(pipefd[1]);
            waitpid(p1, NULL, 0);
            waitpid(p2, NULL, 0);
        }
    }
}
