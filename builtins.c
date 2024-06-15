#include "shell.h"
#include "builtins.h"
#include "utils.h"

char* builtin_str[] = {
    "cd",
    "help",
    "exit",
    "setenv",
    "unsetenv",
    "jobs",
    "alias",
    "unalias",
    "cat",
    "ls",
    "cp",
    "mv",
    "rm",
    "pushd",
    "popd",
    "dirs",
    "source",
    "setprompt",
    "calc",
    "set",
    "get",
    "history",
    "which",
    "echo",
    "fg",
    "bg",
    "history_list"
};

int (*builtin_func[])(char**) = {
    &shell_cd,
    &shell_help,
    &shell_exit,
    &shell_setenv,
    &shell_unsetenv,
    &shell_jobs,
    &shell_alias,
    &shell_unalias,
    &shell_cat,
    &shell_ls,
    &shell_cp,
    &shell_mv,
    &shell_rm,
    &shell_pushd,
    &shell_popd,
    &shell_dirs,
    &shell_source,
    &shell_setprompt,
    &shell_calc,
    &shell_set,
    &shell_get,
    &shell_history,
    &shell_which,
    &shell_echo,
    &shell_fg,
    &shell_bg,
    &shell_history_list
};

int num_builtins() {
    return sizeof(builtin_str) / sizeof(char*);
}

int shell_cd(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "shell: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("shell");
        }
    }
    return 1;
}

int shell_help(char** args) {
    int i;
    printf("Simple Shell\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (i = 0; i < num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int shell_exit(char** args) {
    save_history();
    save_aliases();
    stop_logging();
    return 0;
}

int shell_setenv(char** args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "shell: expected arguments to \"setenv\"\n");
    } else {
        if (setenv(args[1], args[2], 1) != 0) {
            perror("shell");
        }
    }
    return 1;
}

int shell_unsetenv(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "shell: expected argument to \"unsetenv\"\n");
    } else {
        if (unsetenv(args[1]) != 0) {
            perror("shell");
        }
    }
    return 1;
}

int shell_cat(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "shell: expected argument to \"cat\"\n");
        return 1;
    }

    FILE* file = fopen(args[1], "r");
    if (!file) {
        perror("shell");
        return 1;
    }

    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);
    return 1;
}

int shell_ls(char** args) {
    DIR* dir;
    struct dirent* ent;

    if (args[1] == NULL) {
        dir = opendir(".");
    } else {
        dir = opendir(args[1]);
    }

    if (dir == NULL) {
        perror("shell");
        return 1;
    }

    while ((ent = readdir(dir)) != NULL) {
        printf("%s\n", ent->d_name);
    }

    closedir(dir);
    return 1;
}

int shell_cp(char** args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "shell: expected arguments to \"cp\"\n");
        return 1;
    }

    FILE* src = fopen(args[1], "r");
    if (!src) {
        perror("shell");
        return 1;
    }

    FILE* dest = fopen(args[2], "w");
    if (!dest) {
        fclose(src);
        perror("shell");
        return 1;
    }

    char buffer[BUFFER_SIZE];
    size_t n;
    while ((n = fread(buffer, 1, BUFFER_SIZE, src)) > 0) {
        fwrite(buffer, 1, n, dest);
    }

    fclose(src);
    fclose(dest);
    return 1;
}

int shell_mv(char** args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "shell: expected arguments to \"mv\"\n");
        return 1;
    }

    if (rename(args[1], args[2]) != 0) {
        perror("shell");
    }
    return 1;
}

int shell_rm(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "shell: expected argument to \"rm\"\n");
        return 1;
    }

    if (remove(args[1]) != 0) {
        perror("shell");
    }
    return 1;
}

int shell_pushd(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "shell: expected argument to \"pushd\"\n");
        return 1;
    }

    if (dir_stack_count >= MAX_DIR_STACK) {
        fprintf(stderr, "shell: directory stack overflow\n");
        return 1;
    }

    char cwd[BUFFER_SIZE];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("shell");
        return 1;
    }

    dir_stack[dir_stack_count++] = strdup(cwd);

    if (chdir(args[1]) != 0) {
        perror("shell");
        return 1;
    }

    shell_dirs(NULL);
    return 1;
}

int shell_popd(char** args) {
    if (dir_stack_count == 0) {
        fprintf(stderr, "shell: directory stack empty\n");
        return 1;
    }

    if (chdir(dir_stack[--dir_stack_count]) != 0) {
        perror("shell");
        return 1;
    }

    free(dir_stack[dir_stack_count]);
    shell_dirs(NULL);
    return 1;
}

int shell_dirs(char** args) {
    for (int i = 0; i < dir_stack_count; i++) {
        printf("%s ", dir_stack[i]);
    }
    printf("\n");
    return 1;
}

int shell_alias(char** args) {
    if (args[1] == NULL) {
        for (int i = 0; i < alias_count; i++) {
            printf("alias %s='%s'\n", alias_list[i].name, alias_list[i].command);
        }
    } else {
        char* name = strtok(args[1], "=");
        char* command = strtok(NULL, "=");

        if (name && command) {
            alias_list[alias_count].name = strdup(name);
            alias_list[alias_count].command = strdup(command);
            alias_count++;
        } else {
            fprintf(stderr, "shell: alias format is alias name='command'\n");
        }
    }
    return 1;
}

int shell_unalias(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "shell: unalias: usage: unalias name\n");
    } else {
        for (int i = 0; i < alias_count; i++) {
            if (strcmp(alias_list[i].name, args[1]) == 0) {
                free(alias_list[i].name);
                free(alias_list[i].command);
                for (int j = i; j < alias_count - 1; j++) {
                    alias_list[j] = alias_list[j + 1];
                }
                alias_count--;
                break;
            }
        }
    }
    return 1;
}

int shell_jobs(char** args) {
    list_jobs();
    return 1;
}

int shell_source(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "shell: expected argument to \"source\"\n");
        return 1;
    }

    FILE* file = fopen(args[1], "r");
    if (!file) {
        perror("shell");
        return 1;
    }

    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), file)) {
        char** args = split_line(line);
        execute(args);
        free(args);
    }

    fclose(file);
    return 1;
}

int shell_setprompt(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "shell: expected argument to \"setprompt\"\n");
    } else {
        set_prompt(args[1]);
    }
    return 1;
}

int shell_calc(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "shell: expected expression to \"calc\"\n");
        return 1;
    }

    char* expression = args[1];
    double result = 0.0;
    char operator;
    double num;

    while (*expression) {
        if (sscanf(expression, "%lf", &num) == 1) {
            result += num;
            while (*expression && (*expression == '.' || isdigit(*expression))) {
                expression++;
            }
        } else if (sscanf(expression, "%c", &operator) == 1) {
            if (operator == '+') {
                expression++;
            } else if (operator == '-') {
                result -= num * 2;
                expression++;
            } else {
                fprintf(stderr, "shell: unsupported operator in \"calc\"\n");
                return 1;
            }
        } else {
            expression++;
        }
    }

    printf("Result: %lf\n", result);
    return 1;
}

int shell_set(char** args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "shell: expected arguments to \"set\"\n");
    } else {
        set_shell_var(args[1], args[2]);
    }
    return 1;
}

int shell_get(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "shell: expected argument to \"get\"\n");
    } else {
        char* value = get_shell_var(args[1]);
        if (value) {
            printf("%s\n", value);
        } else {
            fprintf(stderr, "shell: variable not found\n");
        }
    }
    return 1;
}

int shell_history_list(char** args) {
    print_history_list();
    return 1;
}

// int shell_history(char** args) {
//     HIST_ENTRY** history_list = history_list();
//     if (history_list) {
//         for (int i = 0; history_list[i]; i++) {
//             printf("%d: %s\n", i + history_base, history_list[i]->line);
//         }
//     }
//     return 1;
// }

void print_history_list() {
    HIST_ENTRY** history = history_list();
    if (history) {
        for (int i = 0; history[i]; i++) {
            printf("%d: %s\n", i + history_base, history[i]->line);
        }
    }
}


int shell_which(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "shell: expected argument to \"which\"\n");
        return 1;
    }

    char* full_path = resolve_path(args[1]);
    if (full_path) {
        printf("%s\n", full_path);
        free(full_path);
    } else {
        fprintf(stderr, "shell: command not found: %s\n", args[1]);
    }
    return 1;
}

int shell_echo(char** args) {
    for (int i = 1; args[i]; i++) {
        printf("%s ", args[i]);
    }
    printf("\n");
    return 1;
}

int shell_fg(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "shell: expected argument to \"fg\"\n");
        return 1;
    }

    int job_number = atoi(args[1]);
    job* j = find_job_by_number(job_number);
    if (j) {
        int status;
        waitpid(j->pid, &status, 0);
        remove_job(j->pid);
    } else {
        fprintf(stderr, "shell: no such job: %s\n", args[1]);
    }
    return 1;
}

int shell_bg(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "shell: expected argument to \"bg\"\n");
        return 1;
    }

    int job_number = atoi(args[1]);
    job* j = find_job_by_number(job_number);
    if (j) {
        kill(j->pid, SIGCONT);
        printf("[%d] %d %s\n", j->job_number, j->pid, j->command);
    } else {
        fprintf(stderr, "shell: no such job: %s\n", args[1]);
    }
    return 1;
}

// int shell_history_list(char** args) {
//     HIST_ENTRY** history_list = history_list();
//     if (history_list) {
//         for (int i = 0; history_list[i]; i++) {
//             printf("%d: %s\n", i + history_base, history_list[i]->line);
//         }
//     }
//     return 1;
// }
