#include "shell.h"
#include "job_control.h"

void add_job(pid_t pid, char* command) {
    job* new_job = malloc(sizeof(job));
    new_job->pid = pid;
    new_job->command = strdup(command);
    new_job->next = job_list;
    new_job->job_number = ++job_count;
    job_list = new_job;
}

void remove_job(pid_t pid) {
    job** current = &job_list;
    while (*current) {
        job* entry = *current;
        if (entry->pid == pid) {
            *current = entry->next;
            free(entry->command);
            free(entry);
            return;
        }
        current = &entry->next;
    }
}

void list_jobs() {
    job* current = job_list;
    while (current) {
        printf("[%d] %d %s\n", current->job_number, current->pid, current->command);
        current = current->next;
    }
}

job* find_job_by_number(int job_number) {
    job* current = job_list;
    while (current) {
        if (current->job_number == job_number) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void sigint_handler(int sig) {
    write(STDOUT_FILENO, "\n> ", 3);
}

void sigtstp_handler(int sig) {
    pid_t pid = getpid();
    if (pid > 0) {
        kill(pid, SIGSTOP);
    }
}

void sigchld_handler(int sig) {
    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            remove_job(pid);
        } else if (WIFSTOPPED(status)) {
            job* j = find_job_by_number(pid);
            if (j) {
                printf("\n[%d] %s stopped\n", j->job_number, j->command);
            }
        }
    }
}

void sigcont_handler(int sig) {
    pid_t pid = getpid();
    if (pid > 0) {
        kill(pid, SIGCONT);
    }
}
