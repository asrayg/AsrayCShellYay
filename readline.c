#include "shell.h"
#include "readline.h"
#include "utils.h"

char* read_line() {
    char* line = readline(prompt);
    if (line && *line) {
        add_history(line);
        line = expand_env_variables(line);
        line = command_substitution(line);
        line = expand_shell_variables(line);
        line = history_expansion(line);
        log_command(line);
    }
    return line;
}

char** split_line(char* line) {
    int bufsize = TOKEN_BUFSIZE, position = 0;
    char** tokens = malloc(bufsize * sizeof(char*));
    char* token;

    if (!tokens) {
        fprintf(stderr, "allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOKEN_DELIMITERS);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += TOKEN_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, TOKEN_DELIMITERS);
    }
    tokens[position] = NULL;
    return tokens;
}

char* command_generator(const char* text, int state) {
    static int list_index, len;
    char* name;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    while ((name = builtin_str[list_index++])) {
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }

    return NULL;
}

char** command_completion(const char* text, int start, int end) {
    char** matches = NULL;

    if (start == 0) {
        matches = rl_completion_matches(text, command_generator);
    } else {
        matches = rl_completion_matches(text, rl_filename_completion_function);
    }

    return matches;
}

void initialize_readline() {
    rl_attempted_completion_function = command_completion;
}
