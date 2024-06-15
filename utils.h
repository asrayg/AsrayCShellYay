#ifndef UTILS_H
#define UTILS_H

void handle_error(const char* message);
char* resolve_path(char* command);
char* expand_env_variables(char* line);
char* command_substitution(char* line);
int handle_redirection(char** args);
char* history_expansion(char* line);
void set_prompt(char* new_prompt);
int set_shell_var(char* name, char* value);
char* get_shell_var(char* name);
char* expand_shell_variables(char* line);
void execute_piped(char** args1, char** args2);
void print_history_list();

#endif
