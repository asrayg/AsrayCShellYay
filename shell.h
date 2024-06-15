#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
#include <readline/readline.h>
#include <readline/history.h>

#define BUFFER_SIZE 1024
#define TOKEN_BUFSIZE 64
#define TOKEN_DELIMITERS " \t\r\n\a"
#define MAX_ALIASES 100
#define MAX_DIR_STACK 100
#define MAX_VARS 100

typedef struct job {
    pid_t pid;
    char* command;
    int job_number;
    struct job* next;
} job;

typedef struct alias {
    char* name;
    char* command;
} alias;

typedef struct shell_var {
    char* name;
    char* value;
} shell_var;

extern char prompt[BUFFER_SIZE];
extern int job_count;
extern job* job_list;
extern FILE* log_file;
extern char* dir_stack[MAX_DIR_STACK];
extern int dir_stack_count;
extern alias alias_list[MAX_ALIASES];
extern int alias_count;
extern shell_var var_list[MAX_VARS];
extern int var_count;

void add_job(pid_t pid, char* command);
void remove_job(pid_t pid);
void list_jobs();
void start_logging();
void log_command(char* command);
void stop_logging();
void load_history();
void save_history();
void save_aliases();
void load_aliases();
void sigint_handler(int sig);
void sigtstp_handler(int sig);
void sigchld_handler(int sig);
void sigcont_handler(int sig);

#endif
