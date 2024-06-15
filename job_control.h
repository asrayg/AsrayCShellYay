#ifndef JOB_CONTROL_H
#define JOB_CONTROL_H

void add_job(pid_t pid, char* command);
void remove_job(pid_t pid);
void list_jobs();
job* find_job_by_number(int job_number);
void sigint_handler(int sig);
void sigtstp_handler(int sig);
void sigchld_handler(int sig);
void sigcont_handler(int sig);

#endif
