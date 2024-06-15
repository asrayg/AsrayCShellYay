#ifndef BUILTINS_H
#define BUILTINS_H

extern char* builtin_str[];
extern int (*builtin_func[])(char**);
extern int num_builtins();

int shell_cd(char** args);
int shell_help(char** args);
int shell_exit(char** args);
int shell_setenv(char** args);
int shell_unsetenv(char** args);
int shell_cat(char** args);
int shell_ls(char** args);
int shell_cp(char** args);
int shell_mv(char** args);
int shell_rm(char** args);
int shell_pushd(char** args);
int shell_popd(char** args);
int shell_dirs(char** args);
int shell_alias(char** args);
int shell_unalias(char** args);
int shell_jobs(char** args);
int shell_source(char** args);
int shell_setprompt(char** args);
int shell_calc(char** args);
int shell_set(char** args);
int shell_get(char** args);
int shell_history(char** args);
int shell_which(char** args);
int shell_echo(char** args);
int shell_fg(char** args);
int shell_bg(char** args);
int shell_history_list(char** args);

#endif
