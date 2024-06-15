#ifndef READLINE_H
#define READLINE_H

char* read_line();
char** split_line(char* line);
char* command_generator(const char* text, int state);
char** command_completion(const char* text, int start, int end);
void initialize_readline();

#endif
